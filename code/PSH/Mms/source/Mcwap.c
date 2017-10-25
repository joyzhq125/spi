/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */






#include "We_Lib.h"    
#include "We_Cfg.h"    
#include "We_Def.h"     
#include "We_Log.h"     
#include "We_Mem.h"    
#include "We_Core.h"   
#include "We_Chrs.h"   
#include "We_File.h"    
#include "We_Cmmn.h"    

#include "Stk_If.h"     

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     
#include "Mmsrpl.h"     
#include "Msig.h"       
#include "Mtimer.h"     
#include "Mmem.h"       
#include "Mconfig.h"    
#include "Mcpdu.h"      
#include "Mcwap.h"      
#include "Mcget.h"      
#include "Mcpost.h"     
#include "Mutils.h"     
#include "Mmain.h"      
#include "Fldmgr.h"     


#define MMS_MAX_CONNECTIONS     10
#define MMS_USER_AGENT          "User-Agent: "




 
#define MMS_LONG_STK_TIMEOUT    (3*60*60*100)
#define MMS_STK_CANNOT_HANDLE_TIMEOUTS 1





typedef enum
{
    WAP_STARTING,   
    WAP_STARTED,    
    WAP_STOP_PENDING, 
    WAP_STOPPING,   
    WAP_STOPPED     
} CohWapFsmState;


typedef enum
{
    TIMER_CONNECTION,   
    TIMER_IDLE          
} TimerType;


typedef struct
{
    MmsStateMachine fsm;        
    long            instance;   
    MmsSignalId     signal;     
} CohWapConnectionData;

















static MmsRequestId mmsRequestId;


static long mmsTransactionId;


static CohWapFsmState mmsCohState;


static int mmsConnections;


static CohWapConnectionData connected[MMS_MAX_CONNECTIONS];


static char *httpHeaderPost;


static char *httpHeaderGet;


static void configureChannel(void);
static void connectedRemove( MmsStateMachine fsm, long instance);
static WE_BOOL connectedStore(MmsStateMachine fsm, long instance, MmsSignalId signal);
static WE_BOOL convStr2SockAddr( const char *str, we_sockaddr_t *address);
static void handleStart( MmsStateMachine fsm, long instance, MmsSignalId signal);
static void handleStartResponse(MmsResult result);
static void handleStop( MmsStateMachine fsm, long instance, MmsSignalId signal);
static void handleStopResponse(MmsResult result);
static void handleTimerExpired(TimerType timerType);
static void mmsWapMain(MmsSignal *sig);


#ifdef WE_LOG_MODULE 



const char *fsmCOHWAPSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_COH_WAP_START:
        return "MMS_SIG_COH_WAP_START";
    case MMS_SIG_COH_WAP_START_RESPONSE:
        return "MMS_SIG_COH_WAP_START_RESPONSE";
    case MMS_SIG_COH_WAP_STOP:
        return "MMS_SIG_COH_WAP_STOP";
    case MMS_SIG_COH_WAP_STOP_RESPONSE:
        return "MMS_SIG_COH_WAP_STOP_RESPONSE";
    default:
        return 0;
    }
} 
#endif




static void configureChannel(void)
{
    char *userAgent;
    char *wapGatewayAddress;
    size_t userAgentLen = 0;
    size_t size;
    we_sockaddr_t proxyAddress;

    userAgent = cfgGetStr(MMS_CFG_CLIENT_USER_AGENT);
    if (userAgent != NULL)
    {
        userAgentLen = strlen(userAgent);
    } 

    if (userAgentLen == 0)
    {
        httpHeaderPost = we_cmmn_strdup( WE_MODID_MMS, MMS_POST_HTTP_HEADER);
        httpHeaderGet = we_cmmn_strdup( WE_MODID_MMS, MMS_GET_HTTP_HEADER);
    }
    else
    {
        size = sizeof(MMS_USER_AGENT) + sizeof(MMS_POST_HTTP_HEADER) + 
            userAgentLen + 1;
        httpHeaderPost = M_ALLOC(size);
        if ((size_t) sprintf( httpHeaderPost, "%s%s\n%s", 
            MMS_USER_AGENT, userAgent, MMS_POST_HTTP_HEADER) > size)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Wrote outside allocated memory!\n",
                __FILE__, __LINE__));
            mmsNotifyError(MMS_RESULT_RESTART_NEEDED);

            return;
        } 

        size = sizeof(MMS_USER_AGENT) + sizeof(MMS_GET_HTTP_HEADER) + 
            userAgentLen + 1;
        httpHeaderGet = M_ALLOC(size);
        if ((size_t) sprintf( httpHeaderGet,"%s%s\n%s", 
            MMS_USER_AGENT, userAgent, MMS_GET_HTTP_HEADER) > size)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Wrote outside allocated memory!\n",
                __FILE__, __LINE__));
            mmsNotifyError(MMS_RESULT_RESTART_NEEDED);

            return;
        } 
    } 

    wapGatewayAddress = cfgGetStr(MMS_CFG_GW_ADDRESS);
    if (wapGatewayAddress != NULL && strlen(wapGatewayAddress) > 0 &&
        convStr2SockAddr( wapGatewayAddress, &proxyAddress))
    {
        proxyAddress.port = (WE_UINT16)cfgGetInt(MMS_CFG_GW_HTTP_PORT);

        STKif_configureProxyChannel( WE_MODID_MMS, MMS_STK_CHANNEL_ID,
            cfgGetInt(MMS_CFG_STK_CONNECTION_TYPE), 
            cfgGetInt(MMS_CFG_NETWORK_ACCOUNT), 
#if MMS_STK_CANNOT_HANDLE_TIMEOUTS == 1
            MMS_LONG_STK_TIMEOUT,
#else
            MAX( MMS_POST_TIMEOUT, MMS_GET_TIMEOUT), 
#endif
            &proxyAddress,
            cfgGetInt(MMS_CFG_GW_SECURE_PORT),
            cfgGetStr(MMS_CFG_GW_USERNAME),
            cfgGetStr(MMS_CFG_GW_PASSWORD),
            cfgGetStr(MMS_CFG_GW_REALM),
            STK_CHANNEL_OPTION_USE_WTP_SAR,
            cfgGetStr(MMS_CFG_CLIENT_HTTP_HEADER));
    }
    else
    {
        STKif_configureChannel( WE_MODID_MMS, MMS_STK_CHANNEL_ID,
            cfgGetInt(MMS_CFG_STK_CONNECTION_TYPE), 
            cfgGetInt(MMS_CFG_NETWORK_ACCOUNT), 
#if MMS_STK_CANNOT_HANDLE_TIMEOUTS == 1
            MMS_LONG_STK_TIMEOUT,
#else
            MAX( MMS_POST_TIMEOUT, MMS_GET_TIMEOUT), 
#endif
            STK_CHANNEL_OPTION_USE_WTP_SAR,
            cfgGetStr(MMS_CFG_CLIENT_HTTP_HEADER));
    } 
} 







static void connectedRemove( MmsStateMachine fsm, long instance)
{
    int i;

    if (mmsConnections <= 1)
    {
        mmsConnections = 0;

        
        mmsWapConnectionCheck();
    }
    else
    {
        
        for (i = 0; i < mmsConnections - 1; ++i)
        {
            if (fsm == connected[i].fsm && instance == connected[i].instance)
            {
                memmove( &connected[i], &connected[i + 1], 
                    (size_t) (mmsConnections - i - 1) * sizeof(CohWapConnectionData));
                break;
            } 
        } 

        --mmsConnections;
    } 
} 









static WE_BOOL connectedStore(MmsStateMachine fsm, long instance, MmsSignalId signal)
{
    int i;

    if (mmsConnections >= MMS_MAX_CONNECTIONS)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Maximum connections already setup %d\n", 
            __FILE__, __LINE__, mmsConnections));

        return FALSE;
    } 

    
    for (i = 0; i < mmsConnections; ++i)
    {
        if (fsm == connected[i].fsm && instance == connected[i].instance)
        {
            connected[i].signal = signal;
            return TRUE;
        } 
    } 

    connected[mmsConnections].fsm = fsm;
    connected[mmsConnections].instance = instance;
    connected[mmsConnections].signal = signal;
    ++mmsConnections;

    return TRUE;
} 








static WE_BOOL convStr2SockAddr( const char *inStr, we_sockaddr_t *address)
{
#define ADDRESS_DELIMITER "."
#define MAX_SUB_ADDRESS   255
    char *subStr;
    int pos = 0;
    int subAddr = 0;
    char *str;

    memset( address, 0, sizeof(we_sockaddr_t));

    if (inStr == NULL)
    {
        return FALSE;
    }
    
    


    str = (char *)M_CALLOC(strlen(inStr)+1); 
    if (str == NULL)
    {
        return FALSE;
    }
    strcpy(str, inStr);

    subStr = mmsStrtok( str, ADDRESS_DELIMITER);
    while (subStr != NULL)
    {
        subAddr = atoi(subStr);
        if (subAddr > (int) MAX_SUB_ADDRESS || pos >= (int) sizeof(address->addr))
        {
            M_FREE(str);
            return FALSE;
        } 

        address->addr[pos++] = (unsigned char)subAddr;

        subStr = mmsStrtok( NULL, ADDRESS_DELIMITER);
    } 

    address->addrLen = (WE_INT16)pos;
    
    M_FREE(str);
    return TRUE;
} 








static void handleStart( MmsStateMachine fsm, long instance, MmsSignalId signal)
{
    
    switch (mmsCohState)
    {
    case WAP_STARTED :
        if (connectedStore( fsm, instance, signal))
        {   
            M_SIGNAL_SENDTO_IU( fsm, signal, instance, MMS_RESULT_OK);
        }
        else
        {   
            M_SIGNAL_SENDTO_IU( fsm, signal, instance, MMS_RESULT_BUSY);
        } 
        break;
    case WAP_STOP_PENDING :
    case WAP_STOPPING :
        
        M_SIGNAL_SENDTO_IU( fsm, signal, instance, MMS_RESULT_BUSY);
        break;
    case WAP_STOPPED :
        mmsConnections = 0;
        M_TIMER_SET_I( M_FSM_COH_WAP, MMS_CONNECT_TIMEOUT, TIMER_CONNECTION);
        mmsCoreModuleStart(WE_MODID_STK, TRUE, M_FSM_COH_WAP, MMS_SIG_COH_WAP_START_RESPONSE, MMS_RESULT_OK);
        STKif_startInterface();
        mmsCohState = WAP_STARTING;
        
    case WAP_STARTING :
        if ( !connectedStore( fsm, instance, signal) )
        {  
            M_SIGNAL_SENDTO_IU( fsm, signal, instance, MMS_RESULT_BUSY);
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): too many consecutive calls %d\n", 
                __FILE__, __LINE__, mmsConnections));
        } 
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): unknown state %d\n", __FILE__, __LINE__, mmsCohState));
        break;
    } 
} 






static void handleStartResponse(MmsResult result)
{
    int i;

    
    switch (mmsCohState)
    {
    case WAP_STARTING :
        M_TIMER_RESET_I( M_FSM_COH_WAP, TIMER_CONNECTION); 
        if (result == MMS_RESULT_OK)
        {
            mmsCohState = WAP_STARTED;
            configureChannel();
        }
        else
        {
            mmsCohState = WAP_STOPPED;
        } 

        for (i = 0; i < mmsConnections; ++i)
        {   
            M_SIGNAL_SENDTO_IU( connected[i].fsm, connected[i].signal, 
                connected[i].instance, result);
        } 
        break;
    case WAP_STARTED :
    case WAP_STOP_PENDING :
    case WAP_STOPPING :
    case WAP_STOPPED :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): received START_RESPONSE in wrong state %d\n", 
            __FILE__, __LINE__, mmsCohState));
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): unknown state %d\n", __FILE__, __LINE__, mmsCohState));
        break;
    } 
} 








static void handleStop( MmsStateMachine fsm, long instance, MmsSignalId signal)
{
    
    switch (mmsCohState)
    {
    case WAP_STARTING :
        
        M_SIGNAL_SENDTO_IU( fsm, signal, instance, MMS_RESULT_BUSY);
        break;
    case WAP_STARTED :
    case WAP_STOP_PENDING :
    case WAP_STOPPING :
    case WAP_STOPPED :
        connectedRemove( fsm, instance);
        M_SIGNAL_SENDTO_IU( fsm, signal, instance, MMS_RESULT_OK);
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): unknown state %d\n", __FILE__, __LINE__, mmsCohState));
        break;
    } 
} 






static void handleStopResponse(MmsResult result)
{
    switch (mmsCohState)
    {
    case WAP_STARTING :
    case WAP_STARTED :
    case WAP_STOP_PENDING :
    case WAP_STOPPED :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): received STOP_RESPONSE in wrong state %d\n", 
            __FILE__, __LINE__, mmsCohState));

        mmsConnections = 0;
        mmsCohState = WAP_STOPPED;
        break;
    case WAP_STOPPING :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Stopped stack result=%d\n", __FILE__, __LINE__, result));

        mmsConnections = 0;
        mmsCohState = WAP_STOPPED;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): unknown state %d\n", __FILE__, __LINE__, mmsCohState));
        break;
    } 
} 





static void handleTimerExpired(TimerType timerType)
{
    int i;

    switch (mmsCohState)
    {
    case WAP_STARTING :
        mmsCohState = WAP_STOPPED;

        for (i = 0; i < mmsConnections; ++i)
        {   
            M_SIGNAL_SENDTO_IU( connected[i].fsm, 
                connected[i].signal, 
                connected[i].instance, MMS_RESULT_COMM_TIMEOUT);
        } 

        mmsConnections = 0;
        break;
    case WAP_STOP_PENDING :
        if (timerType == TIMER_IDLE)    
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): IDLE! Disconnecting.\n", __FILE__, __LINE__));

            mmsCohState = WAP_STOPPING;
            STKif_removeChannel( WE_MODID_MMS, MMS_STK_CHANNEL_ID);
            mmsCoreModuleStop(WE_MODID_STK);
        } 
        break;
    case WAP_STARTED :
    case WAP_STOPPING :
    case WAP_STOPPED :
         
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): received TIMER_EXPIRED when not expecting it %d %d.\n", 
            __FILE__, __LINE__, mmsCohState, timerType));
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): unknown state %d\n", __FILE__, __LINE__, mmsCohState));
        break;
    } 
} 





void mmsWapChannelDisconnect(void)
{
    
    if (mmsCohState == WAP_STARTED)
    {
        STKif_removeChannel( WE_MODID_MMS, MMS_STK_CHANNEL_ID);
        mmsCohState = WAP_STOPPED;
    } 
} 











void mmsWapChannelIsDisconnected(void)
{
    
    if (mmsCohState == WAP_STARTED)
    {
        STKif_removeChannel( WE_MODID_MMS, MMS_STK_CHANNEL_ID);
        mmsCohState = WAP_STOPPED;
        configureChannel();
    } 
} 





void mmsWapConnectionCheck(void)
{
    
    if (mmsConnections == 0 && mmsCohState == WAP_STARTED &&
        cfgGetInt(MMS_CFG_DISCONNECT_ON_IDLE) == TRUE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Disconnect in %d deciseconds\n", 
            __FILE__, __LINE__, MMS_DISCONNECT_ON_IDLE_TIMEOUT));
        M_TIMER_SET_I( M_FSM_COH_WAP, MMS_DISCONNECT_ON_IDLE_TIMEOUT, TIMER_IDLE);

        mmsCohState = WAP_STOP_PENDING;
    } 
} 







void mmsWapEmergencyAbort(void)
{
    mmsConnections = 0;
    mmsCohState = WAP_STOPPED;

    mSignalDeregister(M_FSM_COH_WAP);
} 






void mmsWapFreeContentParams(MmsHttpContent *returned)
{
    if (returned != NULL)
    {
        if (returned->data.buffer != NULL)
        {
            M_FREE(returned->data.buffer);
        } 

        M_FREE(returned);
    } 
} 






MmsRequestId mmsWapGetRequestId(void)
{
    return mmsRequestId++;
} 





long mmsWapGetTransactionId(void)
{
    return mmsTransactionId++;
} 





void mmsWapHttpCancel(MmsRequestId reqId)
{
    if (mmsCohState == WAP_STARTED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "MMS: cancelling/cleaning up HTTP operation reqId=%d.\n", reqId));

        STKif_cancelRequest( WE_MODID_MMS, reqId);
    } 
} 







void mmsWapHttpGet(MmsRequestId requestId, const char *uri)
{
    int option = STK_OPTION_NO_CACHE | STK_OPTION_DO_NOT_CACHE_REPLY;

    STKif_httpRequest( WE_MODID_MMS, MMS_STK_CHANNEL_ID, requestId, uri, 
        STK_METHOD_GET, option, httpHeaderGet, NULL, 0, 0, NULL);
} 








void mmsWapHttpPostFile(MmsRequestId requestId, const char *uri, char *fileName)
{
    stk_content_t stkData;
    int option = STK_OPTION_NO_CACHE | STK_OPTION_DO_NOT_CACHE_REPLY;

    stkData.contentType = (char *) MMS_CONTENT;
    stkData.charset = WE_CHARSET_ASCII;
    stkData.name = NULL;
    stkData.fileName = NULL;
    stkData.dataType = STK_BODY_FILE;
    stkData.dataLen = WE_FILE_GETSIZE(fileName);
    stkData._u.pathName = fileName;

    STKif_httpRequest( WE_MODID_MMS, MMS_STK_CHANNEL_ID, requestId, uri, 
        STK_METHOD_POST, option, httpHeaderPost, NULL, 0, 1, &stkData);
} 









void mmsWapHttpPostPipe(MmsRequestId requestId, const char *uri, char *pipeName, 
    WE_INT32 dataLen)
{
    stk_content_t stkData;
    int option = STK_OPTION_NO_CACHE | STK_OPTION_DO_NOT_CACHE_REPLY;

    stkData.contentType = (char *) MMS_CONTENT;
    stkData.charset = WE_CHARSET_ASCII;
    stkData.name = NULL;
    stkData.fileName = NULL;
    stkData.dataType = STK_BODY_PIPE;
    stkData.dataLen = dataLen;
    stkData._u.pipeName = pipeName;

    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMS POST: Request STK to POST %d bytes from pipe <%s>\n",
        stkData.dataLen, stkData._u.pipeName));

    STKif_httpRequest( WE_MODID_MMS, MMS_STK_CHANNEL_ID, requestId, uri, 
        STK_METHOD_POST, option, httpHeaderPost, NULL, 0, 1, &stkData);
} 









void mmsWapHttpPostPdu(MmsRequestId requestId, const char *uri, void *pdu, 
    WE_INT32 size)
{
    stk_content_t stkData;
    int option = STK_OPTION_NO_CACHE | STK_OPTION_DO_NOT_CACHE_REPLY;

    stkData.contentType = (char *) MMS_CONTENT;
    stkData.charset = WE_CHARSET_ASCII;
    stkData.name = NULL;
    stkData.fileName = NULL;
    stkData.dataType = STK_BODY_BUFFER;
    stkData.dataLen = size;
    stkData._u.data = pdu;

    STKif_httpRequest( WE_MODID_MMS, MMS_STK_CHANNEL_ID, requestId, uri, 
        STK_METHOD_POST, option, httpHeaderPost, NULL, 0, 1, &stkData);
} 















void mmsWapHttpResponse(MmsRequestId wid, WE_INT16 status, 
    WE_INT16 dataType, WE_INT32 dataLen, void *data)
{
    long instance = -1L;
    MmsHttpContent *content = (MmsHttpContent *)M_CALLOC(sizeof(MmsHttpContent));

    if (content == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
            "%s(%d) Out of memory\n", __FILE__, __LINE__));
        mmsNotifyError(MMS_RESULT_RESTART_NEEDED);
        return;
    } 

    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
        "mmsWapHttpResponse(%d, %d, %d, %d, %d)\n", 
        wid, status, dataType, dataLen, (int)data));

    content->wid = wid;
    content->errorNo = mmsWapTranslateError(status);
    content->dataType = dataType;
    content->dataLen = dataLen;
    content->data.buffer = data;

    
    if ( (instance = cohGetInstance(wid)) != -1L )
    {
        M_SIGNAL_SENDTO_IP( M_FSM_COH_GET, (int)MMS_SIG_COH_GET_RSP, 
            instance, content);
    }
    else if ( (instance = cohPostInstance(wid)) != -1L )
    {
        M_SIGNAL_SENDTO_IP( M_FSM_COH_POST, (int)MMS_SIG_COH_POST_RSP, 
            instance, content);
    }
    else
    {
        if (content->data.buffer != NULL)
        {
            M_FREE(content->data.buffer);
        } 

        M_FREE(content);

        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): instance not found for reqId=%d, maybe already cancelled.\n", 
            __FILE__, __LINE__, wid));
    } 
} 





void mmsWapInit(void)
{
    mmsCohState = WAP_STOPPED;
    mmsConnections = 0;
    mmsRequestId = 0;
    mmsTransactionId = (long) WE_TIME_GET_CURRENT();
    httpHeaderPost = NULL;
    httpHeaderGet = NULL;

    mSignalRegisterDst(M_FSM_COH_WAP, mmsWapMain);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH WAP: initialized\n"));
} 






static void mmsWapMain(MmsSignal *sig)
{
    

    if (mmsCohState == WAP_STOP_PENDING && 
        sig->type != MMS_SIG_COMMON_TIMER_EXPIRED)
    {
        M_TIMER_RESET_I( M_FSM_COH_WAP, TIMER_IDLE);
        mmsCohState = WAP_STARTED;
    } 

    switch (sig->type)
    {
    case MMS_SIG_COH_WAP_START :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH WAP: Received MMS_SIG_COH_WAP_START\n"));
        handleStart( (MmsStateMachine)sig->u_param1, sig->i_param, 
            (MmsSignalId)sig->u_param2);
        break;
    case MMS_SIG_COH_WAP_START_RESPONSE :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH WAP: Received MMS_SIG_COH_WAP_START_RESPONSE\n"));
        handleStartResponse((MmsResult)sig->i_param);
        break;
    case MMS_SIG_COH_WAP_STOP :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH WAP: Received MMS_SIG_COH_WAP_STOP\n"));
        handleStop( (MmsStateMachine)sig->u_param1, sig->i_param, 
            (MmsSignalId)sig->u_param2);
        break;
    case MMS_SIG_COH_WAP_STOP_RESPONSE :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH WAP: Received MMS_SIG_COH_WAP_STOP_RESPONSE\n"));
        handleStopResponse((MmsResult)sig->u_param1);
        break;
    case MMS_SIG_COMMON_TIMER_EXPIRED :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Received TIMER_EXPIRED %d\n", 
            __FILE__, __LINE__, sig->i_param));
        handleTimerExpired((TimerType)sig->i_param);
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): received unknown signal %d\n", 
            __FILE__, __LINE__, sig->type));
        break;
    } 
    
    mSignalDelete(sig);
} 





void mmsWapTerminate(void)
{
    if (httpHeaderPost != NULL)
    {
        M_FREE(httpHeaderPost);
        httpHeaderPost = NULL;
    } 

    if (httpHeaderGet != NULL)
    {
        M_FREE(httpHeaderGet);
        httpHeaderGet = NULL;
    } 

    if (mmsConnections > 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): ===> %d connections active when terminating\n", 
            __FILE__, __LINE__, mmsConnections));
    } 

    


    switch (mmsCohState)
    {
    case WAP_STARTING :
    case WAP_STOP_PENDING :
    case WAP_STARTED :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Terminate! Disconnecting.\n", __FILE__, __LINE__));
        
        STKif_removeChannel( WE_MODID_MMS, MMS_STK_CHANNEL_ID);
        break;
    case WAP_STOPPING :
    case WAP_STOPPED :
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): unknown state %d\n", __FILE__, __LINE__, mmsCohState));
        break;
    } 

    mmsCoreModuleStop(WE_MODID_STK);
    mmsConnections = 0;
    mmsCohState = WAP_STOPPED;

    mSignalDeregister(M_FSM_COH_WAP);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH WAP: terminated\n"));
} 






MmsResult mmsWapTranslateError(WE_INT32 error)
{
    MmsResult  res = MMS_RESULT_OK;

    if (error != STK_HTTP_OK)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Stack error (%d)\n", __FILE__, __LINE__, error));
    } 
    
    
    switch (error)
    {
    case STK_HTTP_OK :
        res = MMS_RESULT_OK;
        break;

    

    case STK_HTTP_NO_CONTENT :
        res = MMS_RESULT_OK;
        break;

    case STK_HTTP_USE_PROXY :
    case STK_HTTP_PROXY_AUTH_REQUIRED :
        res = MMS_RESULT_COMM_PROXY_ERROR;
        break;
    case STK_HTTP_UNAUTHORIZED :
        res = MMS_RESULT_COMM_UNAUTHORIZED;
        break;
    case STK_HTTP_FORBIDDEN :
    case STK_HTTP_METHOD_NOT_ALLOWED :
        res = MMS_RESULT_COMM_FORBIDDEN;
        break;
    case STK_HTTP_NOT_FOUND :
        res = MMS_RESULT_COMM_FILE_NOT_FOUND;
        break;
    case STK_HTTP_REQUEST_URI_TOO_LARGE :
        res = MMS_RESULT_COMM_URI_TOO_LARGE;
        break;
    case STK_HTTP_BAD_GATEWAY :
        res = MMS_RESULT_COMM_BAD_GATEWAY;
        break;
    case STK_HTTP_SERVICE_UNAVAILABLE :
        res = MMS_RESULT_COMM_UNAVAILABLE;
        break;
    case STK_HTTP_REQUEST_TIMEOUT :
    case STK_HTTP_GATEWAY_TIMEOUT :
        res = MMS_RESULT_COMM_TIMEOUT;
        break;
    case STK_HTTP_VERSION_NOT_SUPPORTED :
        res = MMS_RESULT_COMM_UNSUPPORTED_VERSION;
        break;
    case STK_HTTP_REQUEST_TOO_LARGE:
        res = MMS_RESULT_COMM_ENTITY_TOO_LARGE;
            break;
    case STK_HTTP_UNSUPPORTED_MEDIA_TYPE :
        res = MMS_RESULT_COMM_UNSUPPORTED_MEDIA_TYPE;
        break;

    
    case STK_ERR_WTLS_BAD_RECORD :
    case STK_ERR_WTLS_HANDSHAKE_FAILURE :
    case STK_ERR_WTLS_TIMED_OUT :
    case STK_ERR_WTLS_UNSUPPORTED_CERT :
    case STK_ERR_WTLS_UNKNOWN_CA :
    case STK_ERR_WTLS_BAD_CERTIFICATE :
    case STK_ERR_WTLS_CERTIFICATE_EXPIRED :
    case STK_ERR_WTLS_CRYPTOLIB :
    case STK_ERR_WTLS_FATAL_ALERT :
    case STK_ERR_WTLS_CRITICAL_ALERT :
    case STK_ERR_WTLS_TOO_MANY_WARNINGS :
    case STK_ERR_WTLS_INTERNAL_ERROR :
    
    case STK_ERR_TLS_BAD_RECORD :
    case STK_ERR_TLS_DECRYPTION_FAILED :
    case STK_ERR_TLS_RECEIVED_ALERT :
    case STK_ERR_TLS_HANDSHAKE_FAILURE :
    case STK_ERR_TLS_UNSUPPORTED_CERT :
    case STK_ERR_TLS_UNKNOWN_CA :
    case STK_ERR_TLS_BAD_CERTIFICATE :
    case STK_ERR_TLS_CERTIFICATE_EXPIRED :
    case STK_ERR_TLS_INTERNAL_ERROR :
    
    case STK_ERR_SKT_SECURITY_CLIENTHELLO :
    case STK_ERR_SKT_SECURITY_SERVERHELLO :
    case STK_ERR_SKT_SECURITY_KEYEXCHANGE :
    case STK_ERR_SKT_SECURITY_FINISHED :
    case STK_ERR_SKT_SECURITY_ENCRYPT :
    case STK_ERR_SKT_SECURITY_DECRYPT :
    case STK_ERR_SKT_SECURITY_TIMEOUT :
        res = MMS_RESULT_COMM_SECURITY_ERROR;
        break;

    default :
        res = MMS_RESULT_COMM_FAILED;
        break;
    } 

    return res;
} 






MmsResult mmsWapTranslateResponseStatus(MmsResponseStatus status)
{
    MmsResult  res = MMS_RESULT_OK;

    
    switch (status)
    {
    case MMS_RSP_STATUS_OK :
        res = MMS_RESULT_OK;
        break;
    case MMS_RSP_STATUS_10_MSG_NOT_FOUND :
    case MMS_RSP_STATUS_T_MESSAGE_NOT_FOUND :
    case MMS_RSP_STATUS_P_MESSAGE_NOT_FOUND :
        res = MMS_RESULT_COMM_FILE_NOT_FOUND;
        break;
    case MMS_RSP_STATUS_10_SERVICE_DENIED :
    case MMS_RSP_STATUS_P_SERVICE_DENIED :
    case MMS_RSP_STATUS_P_REPLY_CHARG_FORWARDING_DENIED :
    case MMS_RSP_STATUS_P_REPLY_CHARG_REQUEST_NOT_ACCEPTED :
        res = MMS_RESULT_COMM_FORBIDDEN;
        break;
    case MMS_RSP_STATUS_10_UNSPECIFIED :
    case MMS_RSP_STATUS_10_NETWORK_PROBLEM :
    case MMS_RSP_STATUS_T_FAILURE :
    case MMS_RSP_STATUS_T_NETWORK_PROBLEM :
    case MMS_RSP_STATUS_P_FAILURE :
        res = MMS_RESULT_COMM_FAILED;
        break;
    case MMS_RSP_STATUS_10_CONTENT_NOT_ACCEPTED :
    case MMS_RSP_STATUS_P_CONTENT_NOT_ACCEPTED :
        res = MMS_RESULT_COMM_UNSUPPORTED_MEDIA_TYPE;
        break;
    case MMS_RSP_STATUS_10_MSG_FORMAT_CORRUPT :
    case MMS_RSP_STATUS_P_MESSAGE_FORMAT_CORRUPT :
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
        break;
    case MMS_RSP_STATUS_UNSUPPORTED_MSG :
        res = MMS_RESULT_COMM_MMSC_VERSION;
        break;
    case MMS_RSP_STATUS_10_ADDRESS_UNRESOLVED :
    case MMS_RSP_STATUS_T_SENDING_ADDRESS_UNRESOLVED :
    case MMS_RSP_STATUS_P_SENDING_ADDRESS_UNRESOLVED :
        res = MMS_RESULT_COMM_ADDRESS_UNRESOLVED;
        break;
    case MMS_RSP_STATUS_P_REPLY_CHARG_NOT_SUPPORTED :
        res = MMS_RESULT_COMM_UNAVAILABLE;
        break;
    case MMS_RSP_STATUS_P_REPLY_CHARG_LIMITATIONS_NOT_MET :
        res = MMS_RESULT_COMM_LIMITATIONS_NOT_MET;
        break;
    default :
        if (status >= MMS_RSP_STATUS_P_FAILURE)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unlisted MMS 1.1 Permanent Response Status (%d)\n", 
                __FILE__, __LINE__, status));
        }
        else if (status >= MMS_RSP_STATUS_T_FAILURE)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unlisted MMS 1.1 Transient Response Status (%d)\n", 
                __FILE__, __LINE__, status));
        }
        else
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unlisted MMS 1.0 Response Status (%d)\n", 
                __FILE__, __LINE__, status));
        } 

        res = MMS_RESULT_COMM_FAILED;
        break;
    } 

    return res;
} 






MmsResult mmsWapTranslateRetrieveStatus(MmsRetrieveStatus status)
{
    MmsResult  res = MMS_RESULT_OK;

    
    switch (status)
    {
    case MMS_RTRV_STATUS_OK :
        res = MMS_RESULT_OK;
        break;
    case MMS_RTRV_STATUS_T_MESSAGE_NOT_FOUND :
    case MMS_RTRV_STATUS_P_MESSAGE_NOT_FOUND :
        res = MMS_RESULT_COMM_FILE_NOT_FOUND;
        break;
    case MMS_RTRV_STATUS_P_SERVICE_DENIED :
        res = MMS_RESULT_COMM_FORBIDDEN;
        break;
    case MMS_RTRV_STATUS_T_FAILURE :
    case MMS_RTRV_STATUS_T_NETWORK_PROBLEM :
    case MMS_RTRV_STATUS_P_FAILURE :
        res = MMS_RESULT_COMM_FAILED;
        break;
    case MMS_RTRV_STATUS_P_CONTENT_UNSUPPORTED :
        res = MMS_RESULT_COMM_UNSUPPORTED_MEDIA_TYPE;
        break;
    default :
        if (status >= MMS_RTRV_STATUS_P_FAILURE)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unlisted MMS 1.1 Permanent Retrieve Status (%d)\n", 
                __FILE__, __LINE__, status));
        }
        else if (status >= MMS_RTRV_STATUS_T_FAILURE)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unlisted MMS 1.1 Transient Retrieve Status (%d)\n", 
                __FILE__, __LINE__, status));
        }
        else
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unlisted MMS 1.0 Retrieve Status (%d)\n", 
                __FILE__, __LINE__, status));
        } 

        res = MMS_RESULT_COMM_FAILED;
        break;
    } 

    return res;
} 

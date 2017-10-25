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
#include "We_Pipe.h"   
#include "We_Cmmn.h"    

#include "Stk_If.h"     

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     
#include "Mmsrpl.h"     
#include "Msig.h"       
#include "Mtimer.h"     
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mcpost.h"     
#include "Mcwap.h"      
#include "Mconfig.h"    
#include "Mutils.h"     
#include "Fldmgr.h"     
#include "Masync.h"     


#define MMS_PIPE_NAME_LEN           (sizeof(MMS_FOLDER) + 10)
#define MMS_MAX_FSM_POST_INSTANCES  5
#define LEN_POST_PORT               5       
#define START_POS_TRANSACTION_ID    3       




#define MMS_MAX_SIZE_POST_ANSWER    256




typedef struct
{
    int             handle;
    char            *name;
} CohPipe;



typedef struct
{
    CohPipe         pipe;
    char            *buffer;        
    WE_UINT32      bytesRead;      
    int             bytesToRead;    
} CohResponse;



typedef struct
{
    CohPipe         pipe;
    char            *buffer;        
    WE_UINT32      bufferSize;     
    WE_UINT32      bytesInBuffer;  
    WE_UINT32      bytesWritten;   
    WE_UINT32      totalWritten;   
    WE_UINT32      fileSize;       
    MmsMsgId        wid;             
    long            asyncId;        
    AsyncOperationHandle asyncOper; 
} CohPostMsg;




typedef struct
{
    unsigned char   *buffer;        
    WE_UINT32      bufferSize;     
} CohPostPdu;


typedef struct
{
    
    MmsStateMachine requestingFsm;  
    long            fsmInstance;    
    MmsSignalId     returnSig;      
    WE_BOOL        isResponseRequested; 

    
    MmsRequestId    requestId;      
    char            *uri;           
    int             retries;        
    WE_BOOL        connected;      
    char            transactionId[MAX_LEN_TRANSACTION_ID]; 
    CohResponse     response;       
    
    
    MmsMessageType  type;           
    union
    {
        CohPostPdu  pdu;            
        CohPostMsg  msg;            
    } post;
    WE_UINT32      lastProgress;   
} CohPostInstanceData;




















static CohPostInstanceData *fsmInstance[MMS_MAX_FSM_POST_INSTANCES];


static void closePostPipe(CohPostInstanceData *fsm);
static void abortResponsePipe(CohPostInstanceData *fsm);
static MmsResult checkResponseGeneral( void *data, WE_UINT32 length, 
    WE_BOOL *permanentError);
static MmsResult checkResponseMSendReq( const CohPostInstanceData *fsm, void *data, 
    unsigned long length, WE_BOOL *permanentError, char **serverMsgId);
static MmsResult checkResponseMForwardReq( const CohPostInstanceData *fsm, 
    void *data, unsigned long length, WE_BOOL *permanentError);
static void cohPostMain(MmsSignal *sig);
static void collectData(const MmsSignal *sig);
static void deleteInstance(long instance);
static long getInstance(const MmsSignal *sig);
static long getOwnerAsyncId(long asyncId);
static int getOwnerPipe(int handle);
static void handleAsyncRead( long asyncId, MmsResult result, WE_UINT32 read);
static void handleConnected( long instance, MmsResult result);
static void handlePipeNotify( int handle, int event);
static void handleResponse(long instance);
static void postFinished(long instance, char *serverMsgId, MmsResult ret);
static void postIt(long instance);
static void postMsg(long instance);
static long postMsgInit(MmsSignal *sig);
static void postPdu(long instance);
static void postRetry(long instance, MmsResult ret);
static WE_BOOL readPipe(long instance);
static long selectInstance(MmsMessageType type, MmsMsgId msgId);
static WE_BOOL storeTransactionId(long instance);
static WE_BOOL stringIsValid( const char *str, int len);
static WE_BOOL writePipe( long instance, WE_BOOL atEof);
static void cohPostProgressStatus(CohPostInstanceData *fsm, WE_UINT8 state);


#ifdef WE_LOG_MODULE 



const char *fsmCOHPostSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_COH_POST:
        return "MMS_SIG_COH_POST";
    case MMS_SIG_COH_POST_RSP:
        return "MMS_SIG_COH_POST_RSP";
    case MMS_SIG_COH_POST_DATA_COLLECTED:
        return "MMS_SIG_COH_POST_DATA_COLLECTED";
    case MMS_SIG_COH_POST_PIPE_NOTIFY:
        return "MMS_SIG_COH_POST_PIPE_NOTIFY";
    case MMS_SIG_COH_POST_CANCEL:
        return "MMS_SIG_COH_POST_CANCEL";
    case MMS_SIG_COH_POST_ASYNC_READ_FINISHED:
        return "MMS_SIG_COH_POST_ASYNC_READ_FINISHED";
    case MMS_SIG_COH_POST_CONNECTED:
        return "MMS_SIG_COH_POST_CONNECTED";
    case MMS_SIG_COH_POST_DISCONNECTED:
        return "MMS_SIG_COH_POST_DISCONNECTED";
    default:
        return 0;
    }

} 
#endif








static void closePostPipe(CohPostInstanceData *fsm)
{
    fsm->post.msg.bytesInBuffer = 0;
    fsm->post.msg.bytesWritten = 0;

    if (fsm->post.msg.pipe.handle >= 0)
    {
        (void) WE_PIPE_CLOSE(fsm->post.msg.pipe.handle);
        fsm->post.msg.pipe.handle = -1;
    } 

    if (fsm->post.msg.pipe.name != NULL)
    {
        

        M_FREE( fsm->post.msg.pipe.name);
        fsm->post.msg.pipe.name = NULL;
    } 
} 






static void abortResponsePipe(CohPostInstanceData *fsm)
{
    fsm->response.bytesRead = 0;
    fsm->response.bytesToRead = 0;

    if (fsm->response.pipe.handle >= 0)
    {
        (void) WE_PIPE_CLOSE(fsm->response.pipe.handle);
        fsm->response.pipe.handle = -1;
    } 

    if (fsm->response.pipe.name != NULL)
    {
        (void) WE_PIPE_DELETE(fsm->response.pipe.name);
        M_FREE( fsm->response.pipe.name);
        fsm->response.pipe.name = NULL;
    } 
} 







void cohPostEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_COH_POST);
} 









static MmsResult checkResponseGeneral( void *data, WE_UINT32 length, 
    WE_BOOL *permanentError)
{
    MmsResult           res = MMS_RESULT_OK;
    MmsHeaderValue      read;

    *permanentError = FALSE;

    if (data == NULL || length == 0)
    {
        

    }
    else if ( !mmsPduGet( data, length, X_MMS_MESSAGE_TYPE, &read) )
    {
        

    }
    else if (read.messageType == MMS_M_SEND_CONF) 
    {   


        if ( !mmsPduGet( data, length, X_MMS_RESPONSE_STATUS, &read) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): No data X_MMS_RESPONSE_STATUS header tag\n",
                __FILE__, __LINE__));
            res = MMS_RESULT_COMM_ILLEGAL_PDU;
        }
        else if (read.responseStatus == MMS_RSP_STATUS_UNSUPPORTED_MSG)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): MMSC didn't recognize PDU, response status (%d)\n", 
                __FILE__, __LINE__, read.responseStatus));

            *permanentError = TRUE;
            res = mmsWapTranslateResponseStatus((MmsResponseStatus)read.responseStatus);
        } 
    } 

    return res;
} 











static MmsResult checkResponseMSendReq( const CohPostInstanceData *fsm, 
    void *data, unsigned long length, WE_BOOL *permanentError, char **serverMsgId)
{
    MmsResult           res = MMS_RESULT_OK;
    MmsHeaderValue      read;
    MmsHeaderValue      server;
    unsigned long       maxLen = 0L;
    unsigned long       strSize = 0L;

    *permanentError = FALSE;
    *serverMsgId = NULL;

    
    if (data == NULL || length == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST checkResponseMSendReq: No data returned (%d)\n", 
            fsm->requestId));
        res = MMS_RESULT_COMM_FAILED;
    }
    else if ( mmsPduUnrecognized( data, length) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Unrecognized PDU!\n", __FILE__, __LINE__));
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if ( !mmsPduSanityCheck( data, length) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Badly formatted PDU!\n", __FILE__, __LINE__));
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if ( !mmsPduGet( data, length, X_MMS_MESSAGE_TYPE, &read) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No data X_MMS_MESSAGE_TYPE header tag\n",
            __FILE__, __LINE__));
        res = MMS_RESULT_COMM_HEADER_TAG;
    }
    else if (read.messageType != MMS_M_SEND_CONF)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong message type (%d)\n", 
            __FILE__, __LINE__, read.messageType));
        res = MMS_RESULT_COMM_UNEXPECTED_MESSAGE;
    }
    else if ( !mmsPduGet( data, length, X_MMS_TRANSACTION_ID, &read) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No data X_MMS_TRANSACTION_ID header tag\n",
            __FILE__, __LINE__));
        res = MMS_RESULT_COMM_HEADER_TAG;
    }
    else if ( read.transactionId == NULL ||
        (strcmp( read.transactionId, (char *)fsm->transactionId) != 0 ))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong transaction WID (%s) != (%s)\n", 
            __FILE__, __LINE__,
            read.transactionId == NULL ? "NULL" : read.transactionId,
            fsm->transactionId == NULL ? "NULL" : fsm->transactionId));
        res = MMS_RESULT_COMM_TRANSACTION_ID;
    }
    else if ( !mmsPduGet( data, length, X_MMS_RESPONSE_STATUS, &read) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No data X_MMS_RESPONSE_STATUS header tag\n",
            __FILE__, __LINE__));
        res = MMS_RESULT_COMM_HEADER_TAG;
    }
    else if (read.responseStatus != MMS_RSP_STATUS_OK)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong response status (%d)\n", 
            __FILE__, __LINE__, read.responseStatus));
        if (read.responseStatus >= MMS_RSP_STATUS_P_FAILURE ||
            read.responseStatus <= MMS_RSP_STATUS_UNSUPPORTED_MSG)
        {
            *permanentError = TRUE;
        } 

        res = mmsWapTranslateResponseStatus((MmsResponseStatus)read.responseStatus);
    }
    else
    {
        


        if ( mmsPduGet( data, length, MMS_MESSAGE_ID, &server) )
        {
            maxLen = (unsigned long)data - (unsigned long)server.messageId + length;

            if (maxLen > 0)
            {
                strSize = strlen(server.messageId) + 1;
                *serverMsgId = M_CALLOC(strSize);
                strcpy( *serverMsgId, server.messageId);
            } 
        } 

        res = MMS_RESULT_OK;
    } 

    return res;
} 









static MmsResult checkResponseMForwardReq( const CohPostInstanceData *fsm, 
    void *data, unsigned long length, WE_BOOL *permanentError)
{
    MmsResult           res = MMS_RESULT_OK;
    MmsHeaderValue      read;
    
    *permanentError = FALSE;

    
    if (data == NULL || length == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST checkResponseMSendReq: No data returned (%d)\n", 
            fsm->requestId));
        res = MMS_RESULT_COMM_FAILED;
    }
    else if ( mmsPduUnrecognized( data, length) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Unrecognized PDU!\n", __FILE__, __LINE__));
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if ( !mmsPduSanityCheck( data, length) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Badly formatted PDU!\n", __FILE__, __LINE__));
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if ( !mmsPduGet( data, length, X_MMS_MESSAGE_TYPE, &read) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No X_MMS_MESSAGE_TYPE header tag\n",
            __FILE__, __LINE__));
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if (read.messageType != MMS_M_FORWARD_CONF)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong PDU as response.\n",
            __FILE__, __LINE__));
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if ( !mmsPduGet( data, length, X_MMS_TRANSACTION_ID, &read) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No data X_MMS_TRANSACTION_ID header tag\n",
            __FILE__, __LINE__));
        res = MMS_RESULT_COMM_HEADER_TAG;
    }
    else if (read.transactionId == NULL ||
        strcmp( (char *)read.transactionId, (char *)fsm->transactionId) != 0 )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong transaction WID (%s) != (%s)\n", 
            __FILE__, __LINE__,
            read.transactionId == NULL ? "NULL" : read.transactionId,
            fsm->transactionId == NULL ? "NULL" : fsm->transactionId));
        res = MMS_RESULT_COMM_TRANSACTION_ID;
    }
    else if ( !mmsPduGet( data, length, X_MMS_RESPONSE_STATUS, &read) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No data X_MMS_RESPONSE_STATUS header tag\n",
            __FILE__, __LINE__));
        res = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if (read.responseStatus != MMS_RSP_STATUS_OK)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong response status (%d)\n", 
            __FILE__, __LINE__, read.responseStatus));

        if (read.responseStatus >= MMS_RSP_STATUS_P_FAILURE ||
            read.responseStatus <= MMS_RSP_STATUS_UNSUPPORTED_MSG)
        {
            *permanentError = TRUE;
        } 

        res = mmsWapTranslateResponseStatus((MmsResponseStatus)read.responseStatus);
    } 

    return res;
} 

static void cohPostProgressStatus(CohPostInstanceData *fsm, WE_UINT8 state)
{
    if (fsm->type == MMS_M_SEND_REQ || fsm->type == MMS_M_READ_REC_IND || fsm->type==MMS_M_NOTIFY_RESP || fsm->type==MMS_M_READ_ORIG_IND)
    {
           replyProgressStatus(MMS_PROGRESS_STATUS_SEND, state,
                fsm->post.msg.wid, fsm->post.msg.totalWritten, fsm->post.msg.fileSize,
                &fsm->lastProgress);
    }
} 









WE_BOOL cohPostIsPipeOwner(int handle)
{
    return getOwnerPipe(handle) != -1;
} 





void cohPostCancel(MmsMsgId msgId)
{
    M_SIGNAL_SENDTO_U( M_FSM_COH_POST, (int)MMS_SIG_COH_POST_CANCEL, msgId);
} 





void cohPostInit(void)
{
    int i;
    
    for (i = 0; i < MMS_MAX_FSM_POST_INSTANCES; ++i)
    {
        fsmInstance[i] = NULL;
    } 

    mSignalRegisterDst(M_FSM_COH_POST, cohPostMain);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH POST: initialized\n"));
} 
 












long cohPostInstance(MmsRequestId wid)
{
    long i;
    
    
    for (i = 0L; i < MMS_MAX_FSM_POST_INSTANCES; ++i)
    {
        if (fsmInstance[i] != NULL && fsmInstance[i]->requestId == wid)
        {
            return i;
        } 
    } 

    return -1L;
} 






static void cohPostMain(MmsSignal *sig)
{
    long instance = -1L;
    int i;

    switch (sig->type)
    {
    case MMS_SIG_COH_POST : 
        
        if ((instance = postMsgInit(sig)) != -1L)
        {   
            M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_START, instance, 
                M_FSM_COH_POST, MMS_SIG_COH_POST_CONNECTED);

            cohPostProgressStatus(fsmInstance[instance], MMS_PROGRESS_STATUS_START);        
        } 
        break;        
    case MMS_SIG_COH_POST_CONNECTED : 
        handleConnected( sig->i_param, (MmsResult)sig->u_param1);
        break; 
    case MMS_SIG_COH_POST_DISCONNECTED : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH POST: Received MMS_SIG_COH_POST_DISCONNECTED\n"));
        break; 
    case MMS_SIG_COH_POST_ASYNC_READ_FINISHED :
        handleAsyncRead( sig->i_param, (MmsResult)sig->u_param1, sig->u_param2);
        break;
    case MMS_SIG_COH_POST_CANCEL : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH POST: MMS_SIG_COH_POST_CANCEL %d\n", sig->u_param1));

        for (i = 0; i < MMS_MAX_FSM_POST_INSTANCES; ++i)
        {
            if (fsmInstance[i] != NULL)
            {
                

                if (fsmInstance[i]->type == MMS_M_SEND_REQ && 
                    fsmInstance[i]->post.msg.wid == sig->u_param1)
                {
                    

                    postFinished( i, NULL, MMS_RESULT_CANCELLED_BY_USER);
                } 
            } 
        } 
        break;
    case MMS_SIG_COH_POST_RSP : 
        collectData(sig);
        break;
    case MMS_SIG_COH_POST_DATA_COLLECTED :
        handleResponse(sig->i_param);
        break;
    case MMS_SIG_COH_POST_PIPE_NOTIFY :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH POST: Received MMS_SIG_COH_POST_PIPE_NOTIFY\n"));

        handlePipeNotify( (int) sig->u_param1, sig->i_param);
        break;
    case MMS_SIG_COMMON_TIMER_EXPIRED :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH POST: Received TIMER_EXPIRED\n"));

        if ((instance = getInstance(sig)) != -1)
        {
            postRetry( instance, MMS_RESULT_COMM_TIMEOUT);
        } 
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH POST: received unknown signal %d\n", sig->type));
        break;
    } 
    
    mSignalDelete(sig);
} 






static void collectData(const MmsSignal *sig)
{
    MmsResult           res = MMS_RESULT_OK;
    long                instance;
    MmsHttpContent      *p; 
    CohPostInstanceData *fsm;   

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH POST: Received MMS_SIG_COH_POST_RSP\n"));

    if ((instance = getInstance(sig)) == -1)
    {
        return;
    } 

    fsm = fsmInstance[instance];

    
    M_TIMER_RESET_I( M_FSM_COH_POST, instance);

    
    p = (MmsHttpContent *)sig->p_param;
    if (p == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST collectData: No data returned (%d)\n", instance));
        res = MMS_RESULT_COMM_FAILED;
    }
    else if (p->errorNo != MMS_RESULT_OK)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST collectData: Wrong return code from STK, error (%d)\n", 
            p->errorNo));

        res = p->errorNo;
    }
    else if (p->wid != fsm->requestId)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST collectData: Wrong request WID %d != %d\n", 
            p->wid, fsm->requestId));
        res = MMS_RESULT_COMM_FAILED;
    }
    else if (p->dataType != STK_BODY_PIPE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST collectData: Data expected in pipe, error (%d)\n", 
            p->errorNo));
        if ( p->errorNo == MMS_RESULT_OK)
        {
            res = MMS_RESULT_COMM_FAILED;
        }
        else
        {
            res = p->errorNo;
        } 
    }
    else if (p->data.pipeName == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST collectData: No pipe name returned, error (%d)\n", 
            p->errorNo));

        if ( p->errorNo == MMS_RESULT_OK)
        {
            res = MMS_RESULT_COMM_FAILED;
        }
        else
        {
            res = p->errorNo;
        } 
    }
    else if ((fsm->response.pipe.handle = WE_PIPE_OPEN( WE_MODID_MMS, p->data.pipeName)) < 0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "POST collectData: Failed to open the pipe <%s>\n", p->data.pipeName));
        res = MMS_RESULT_COMM_FAILED;
    }
    else
    {
        fsm->response.buffer = M_CALLOC(MMS_MAX_SIZE_POST_ANSWER);
        fsm->response.bytesToRead = MMS_MAX_SIZE_POST_ANSWER;
        fsm->response.pipe.name = we_cmmn_strdup( WE_MODID_MMS, p->data.pipeName);

        
        if (!readPipe(instance))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                "POST collectData: readPipe failed\n"));
            res = MMS_RESULT_COMM_FAILED;
        } 
    } 

    if (res != MMS_RESULT_OK)
    {
        if (NULL != p && p->dataType == STK_BODY_PIPE && 
            NULL != p->data.pipeName)
        {
            (void) WE_PIPE_DELETE(p->data.pipeName);
        }
        postRetry( instance, res);
    } 
    
    mmsWapFreeContentParams(p);
} 





void cohPostTerminate(void)
{
    long i;

    
    for (i = 0L; i < MMS_MAX_FSM_POST_INSTANCES; ++i)
    {
        if (fsmInstance[i] != NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "******* POST FSM active at terminate ******* %s %s %s%d\n",
                fsmInstance[i]->uri == NULL ? 
                    "NULL" : fsmInstance[i]->uri, 
                fsmInstance[i]->transactionId, 
                fsmInstance[i]->type == MMS_M_SEND_REQ ? 
                    "msgId=" : "pdu type=",
                fsmInstance[i]->type == MMS_M_SEND_REQ ? 
                    fsmInstance[i]->post.msg.wid : 
                    (int)fsmInstance[i]->post.pdu.buffer[1]));

            deleteInstance(i);
        } 
    } 

    mSignalDeregister(M_FSM_COH_POST);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH POST: terminated\n"));
} 







static void deleteInstance(long instance)
{
    CohPostInstanceData *fsm;   

    if (instance < 0 || instance >= MMS_MAX_FSM_POST_INSTANCES || 
        fsmInstance[instance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d) Trying to delete wrong instance %d\n", 
            __FILE__, __LINE__, instance));
        return;
    } 

    fsm = fsmInstance[instance];

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "====> POST deleting instance %d, requesting FSM %d, returnSig %d\n", 
        instance, fsm->requestingFsm, fsm->returnSig, fsm->retries));
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "      retries %d, transId <%s>, reqId %d\n", 
        fsm->retries, fsm->transactionId, fsm->requestId));

    M_TIMER_RESET_I( M_FSM_COH_POST, instance); 

    
    mmsWapHttpCancel(fsm->requestId);

    if (fsm->uri != NULL)
    {
        M_FREE(fsm->uri);
    } 

    abortResponsePipe(fsm);

    if (fsm->response.buffer != NULL)
    {
        M_FREE(fsm->response.buffer);
        fsm->response.buffer = NULL;
    } 

    
    switch (fsm->type)
    {
    case MMS_M_SEND_REQ :
        closePostPipe(fsm);

        if (fsm->post.msg.buffer != NULL)
        {
            M_FREE(fsm->post.msg.buffer);
            fsm->post.msg.buffer = NULL;
        } 

        if (fsm->post.msg.asyncOper != NULL)
        {
            asyncOperationStop(&fsm->post.msg.asyncOper);
        } 
        break;
    case MMS_M_NOTIFY_RESP :
    case MMS_M_RETRIEVE_CONF :
    case MMS_M_ACKNOWLEDGE_IND :
    case MMS_M_READ_REC_IND :
    case MMS_M_FORWARD_REQ :
        M_FREE(fsm->post.pdu.buffer);
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST: Illegal type (%d)!\n", fsm->type));
        break;
    } 

    if (fsmInstance[instance]->connected)
    {
        
        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_STOP, instance, 
            M_FSM_COH_POST, MMS_SIG_COH_POST_DISCONNECTED);
    } 

    M_FREE(fsmInstance[instance]);
    fsmInstance[instance] = NULL;
} 







static long getOwnerAsyncId(long asyncId)
{
    int i;
    
    for (i = 0; i < MMS_MAX_FSM_POST_INSTANCES; ++i)
    {
        if (fsmInstance[i] != NULL)
        {
            if (fsmInstance[i]->type == MMS_M_SEND_REQ && 
                fsmInstance[i]->post.msg.asyncId == asyncId)
            {
                return i;
            } 
        } 
    } 

    return -1;
} 







static int getOwnerPipe(int handle)
{
    int i;
    
    for (i = 0; i < MMS_MAX_FSM_POST_INSTANCES; ++i)
    {
        if (fsmInstance[i] != NULL)
        {
            if (fsmInstance[i]->response.pipe.handle == handle ||
                fsmInstance[i]->post.msg.pipe.handle == handle)
            {
                return i;
            } 
        } 
    } 

    return -1;
} 







static long getInstance(const MmsSignal *sig)
{
    long instance = sig->i_param;

    if (instance < 0L || instance >= MMS_MAX_FSM_POST_INSTANCES ||
        fsmInstance[instance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST getInstance: instance out of bounds (%d)\n", instance));
        instance = -1L;
    } 

    return instance;
} 








static void handleAsyncRead( long asyncId, MmsResult result, WE_UINT32 read)
{
    CohPostInstanceData *fsm;   
    long instance = getOwnerAsyncId(asyncId);

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH POST: Received MMS_SIG_COH_POST_ASYNC_READ_FINISHED\n"));
    if (instance < 0 || instance >= MMS_MAX_FSM_POST_INSTANCES || 
        fsmInstance[instance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong instance %d\n", __FILE__, __LINE__, instance));
        return;
    } 

    fsm = fsmInstance[instance];
    if (result != MMS_RESULT_OK && result != MMS_RESULT_EOF)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Async operation failed %d\n", 
            __FILE__, __LINE__, result));

        postFinished( instance, NULL, result);
        return;
    } 

    fsm->post.msg.bytesWritten = 0;
    fsm->post.msg.bytesInBuffer = read;
    if (fsm->type == MMS_M_SEND_REQ && fsm->post.msg.bytesInBuffer > 0 &&
        fsm->transactionId[0] == WE_EOS)
    {   
        if ( !storeTransactionId(instance) )
        {
            postFinished( instance, NULL, result);
            return;
        } 
    } 

    if ( !writePipe( instance, result == MMS_RESULT_EOF) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Write operation failed.\n", __FILE__, __LINE__));

        postFinished( instance, NULL, MMS_RESULT_COMM_FAILED);
        return;
    } 
} 







static void handleConnected( long instance, MmsResult result)
{
    CohPostInstanceData *fsm;   

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS POST: Received MMS_SIG_COH_POST_CONNECTED, instance %d, result %d\n",
        instance, result));
    if (instance < 0 || instance >= MMS_MAX_FSM_POST_INSTANCES ||
        fsmInstance[instance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong instance %d\n", __FILE__, __LINE__, instance));
        return;
    } 

    fsm = fsmInstance[instance];
    if (result == MMS_RESULT_OK)
    {
        fsm->connected = TRUE;
        postIt(instance);
    }
    else if (++fsm->retries < MMS_POST_RETRY)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't establish connection %d, retry=%d\n", 
            __FILE__, __LINE__, result, fsm->retries));

        
        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_START, 
            instance, M_FSM_COH_POST, MMS_SIG_COH_POST_CONNECTED);
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't establish connection %d. No more retries left.\n", 
            __FILE__, __LINE__, result));
        postFinished( instance, NULL, result);
    } 
} 







static void handlePipeNotify( int handle, int event)
{
    int instance = getOwnerPipe(handle);
    
    if (instance < 0)
    {   
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Received notify for unknown pipe handle <%d>\n", 
            __FILE__, __LINE__, handle));
        return;
    } 
    
    switch (event)
    {
    case WE_PIPE_EVENT_WRITE:
        if ( !writePipe( instance, FALSE) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Write operation failed.\n", __FILE__, __LINE__));

            postFinished( instance, NULL, MMS_RESULT_COMM_FAILED);
        } 
        break;
    case WE_PIPE_EVENT_READ:
        if (!readPipe(instance))
        {
            


            M_SIGNAL_SENDTO_I( M_FSM_COH_POST, (int)MMS_SIG_COH_POST_DATA_COLLECTED,
                instance);
        } 
        break;
    case WE_PIPE_EVENT_CLOSED:
        
        M_SIGNAL_SENDTO_I( M_FSM_COH_POST, (int)MMS_SIG_COH_POST_DATA_COLLECTED,
            instance);
        break;
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Unexpected pipe event=%d, instance=%d\n", __FILE__, __LINE__, 
            event, instance));
        break;
    } 
} 






static void handleResponse(long instance)
{
    MmsResult           res = MMS_RESULT_OK;
    char                *serverMsgId = NULL;
    WE_BOOL            permanentError = FALSE;
    CohPostInstanceData *fsm;   

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH POST: Received MMS_SIG_COH_POST_DATA_COLLECTED\n"));

    if (instance < 0L || instance >= MMS_MAX_FSM_POST_INSTANCES ||
        fsmInstance[instance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST handleResponse: instance out of bounds (%d)\n", instance));
        return;
    } 

    fsm = fsmInstance[instance];

    if ( (res = checkResponseGeneral( fsm->response.buffer, 
        fsm->response.bytesRead, &permanentError)) != MMS_RESULT_OK)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST handleResponse: General failure res=%d, request WID=%d\n", 
            res, fsm->requestId));
    }
    else 
    {
        
        
        switch (fsm->type)
        {
        case MMS_M_SEND_REQ :
            res = checkResponseMSendReq( fsm, fsm->response.buffer, 
                fsm->response.bytesRead, &permanentError, &serverMsgId);
            break;
        case MMS_M_FORWARD_REQ :
            res = checkResponseMForwardReq( fsm, fsm->response.buffer, 
                fsm->response.bytesRead, &permanentError);
            break;
        case MMS_M_NOTIFY_RESP :
        case MMS_M_ACKNOWLEDGE_IND :
        case MMS_M_READ_REC_IND :
            
            break;
        default :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "POST: Can't be here. Already tested (%d)!\n", fsm->type));
            return;
        } 
    } 

    if (res == MMS_RESULT_OK || permanentError )
    {
        postFinished( instance, serverMsgId, res);
    }
    else
    {
        postRetry(instance, res);
    } 
} 








static void postFinished( long instance, char *serverMsgId, MmsResult res)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "====> POST Finished (%s): ", 
        fsmInstance[instance]->type == MMS_M_SEND_REQ ? "MMS_M_SEND_REQ" :
            fsmInstance[instance]->type == MMS_M_NOTIFY_RESP ? "MMS_M_NOTIFY_RESP" :
            fsmInstance[instance]->type == MMS_M_ACKNOWLEDGE_IND ? "MMS_M_ACKNOWLEDGE_IND" :
            fsmInstance[instance]->type == MMS_M_READ_REC_IND ? "MMS_M_READ_REC_IND" :
            fsmInstance[instance]->type == MMS_M_FORWARD_REQ ? "MMS_M_FORWARD_REQ" :
            "UNKNOWN"));

    if (fsmInstance[instance]->type == MMS_M_SEND_REQ)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "Result=%d instance=%d transId=<%s> msgId=%d bytesSent=%d\n", 
            res, instance, fsmInstance[instance]->transactionId,
            fsmInstance[instance]->post.msg.wid,
            fsmInstance[instance]->post.msg.totalWritten));        
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "Result=%d instance=%d transId=<%s> msgId=%d bytesSent=%d\n", 
            res, instance, fsmInstance[instance]->transactionId,
            -1,
            fsmInstance[instance]->post.pdu.bufferSize));
    }

    if (res != MMS_RESULT_OK)
    {
        cohPostProgressStatus(fsmInstance[instance], MMS_PROGRESS_STATUS_COMPLETED);
    }
    else
    {
        cohPostProgressStatus(fsmInstance[instance], MMS_PROGRESS_STATUS_STOP);
    }
            
    if (fsmInstance[instance]->isResponseRequested)
    {
        
        M_SIGNAL_SENDTO_IUP(fsmInstance[instance]->requestingFsm, 
            fsmInstance[instance]->returnSig, 
            fsmInstance[instance]->fsmInstance,
            res,
            serverMsgId);
    } 

    deleteInstance(instance);
} 






static void postIt(long instance)
{
    
    switch (fsmInstance[instance]->type)
    {
    case MMS_M_SEND_REQ :
        postMsg(instance);
        break;
    case MMS_M_NOTIFY_RESP :
    case MMS_M_ACKNOWLEDGE_IND :
    case MMS_M_READ_REC_IND :
    case MMS_M_FORWARD_REQ :
        postPdu(instance);
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST: Illegal type (%d)!\n", fsmInstance[instance]->type));

        postFinished( instance, NULL, MMS_RESULT_ERROR);
        return;
    } 
} 






static void postMsg(long instance)
{
    CohPostInstanceData *fsm;   

    fsm = fsmInstance[instance];

    M_TIMER_SET_I( M_FSM_COH_POST, MMS_POST_TIMEOUT, instance);

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "====> POST MSG %d (%d bytes) to uri=<%s>, transId=<%s>, reqId=%d\n", 
        fsm->post.msg.wid, fsm->post.msg.fileSize, fsm->uri, 
        fsm->transactionId, fsm->requestId));

    fsm->post.msg.pipe.name = M_CALLOC(MMS_PIPE_NAME_LEN);

    

    if (sprintf( fsm->post.msg.pipe.name, "%s%lx", MMS_FOLDER, 
        mmsWapGetTransactionId()) > ((int)MMS_PIPE_NAME_LEN))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrote outside allocated memory!\n",
            __FILE__, __LINE__));
        mmsNotifyError(MMS_RESULT_RESTART_NEEDED);

        return;
    } 

    if ( (fsm->post.msg.pipe.handle = WE_PIPE_CREATE( WE_MODID_MMS, 
        fsm->post.msg.pipe.name)) < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't create pipe <%s> for posting. Error %d.\n",
            __FILE__, __LINE__, fsm->post.msg.pipe.name, fsm->post.msg.pipe.handle));
        postRetry( instance, MMS_RESULT_COMM_FAILED);
    }
    else
    {
        mmsWapHttpPostPipe( fsm->requestId, fsm->uri, fsm->post.msg.pipe.name,
            (WE_INT32) fsm->post.msg.fileSize);

        asyncRead( fsm->post.msg.asyncOper, 0, fsm->post.msg.buffer, fsm->post.msg.bufferSize);
    } 
} 







static long postMsgInit(MmsSignal *sig)
{
    long instance = -1L;
    int port = -1;
    unsigned len = 0;
    char *server = NULL;
    char *uri = NULL;
    char *scheme = NULL;
    const char *slash = "";

    MmsResult result = MMS_RESULT_OK;
    CohPostInstanceData *fsm;   
    MmsSigCohPostParam *param;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH POST: Received MMS_SIG_COH_POST\n"));

    param = (MmsSigCohPostParam *)sig->p_param;

    
    server = cfgGetStr(MMS_CFG_PROXY_RELAY);
    port = cfgGetInt(MMS_CFG_PORT);
    uri = cfgGetStr(MMS_CFG_POST_URI);
    scheme = cfgGetStr(MMS_CFG_PROXY_RELAY_SCHEME);

    if (uri && uri[0] != '/')
    {
        


        slash = "/";
    }
    
    if (param == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Parameter error (%d)\n", __FILE__, __LINE__, param));
        result = MMS_RESULT_ERROR;
    }
    else if (param->type < MMS_M_SEND_REQ || param->type > MMS_M_FORWARD_CONF)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Illegal POST requested %d\n", __FILE__, __LINE__,
            param->type));
        result = MMS_RESULT_ERROR;
    }
    else
    {
        instance = selectInstance(param->type, param->data.msgId);
    } 

    
    if (instance == -1L)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): We're BUSY! Return!\n", __FILE__, __LINE__));
        result = MMS_RESULT_BUSY;
    }
    else if (instance < 0L || instance >= MMS_MAX_FSM_POST_INSTANCES ||
        fsmInstance[instance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): instance out of bounds (%d)\n",
            __FILE__, __LINE__, instance));
        result = MMS_RESULT_ERROR;
    }
    else if (scheme == NULL || server == NULL || port < 0 || uri == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "Illegal configuration: Missing scheme, server, port or URI.\n"));
        result = MMS_RESULT_CONFIG_ERROR;
    } 

    


    if (result != MMS_RESULT_OK || instance == -1L || uri == NULL || 
        server == NULL || param == NULL)
    {
        
        M_SIGNAL_SENDTO_IU( (MmsStateMachine)sig->u_param1, 
            sig->u_param2, sig->i_param, result);

        if (param != NULL && param->type != MMS_M_SEND_REQ)
        {
            M_FREE(param->data.pdu.packet);
        } 

        if (sig->p_param != NULL)
        {
            M_FREE(sig->p_param);
        } 
        
        deleteInstance(instance);
        return -1L;
    } 

    fsm = fsmInstance[instance];

    fsm->lastProgress = 0;  
    
    
    fsm->requestingFsm = (MmsStateMachine)sig->u_param1;
    fsm->returnSig = (MmsSignalId)sig->u_param2;
    fsm->fsmInstance = sig->i_param;

    fsm->isResponseRequested = param->isResponseRequested;

    
    
    switch (fsm->type)
    {
    case MMS_M_SEND_REQ :
        fsm->post.msg.wid = param->data.msgId;
        fsm->post.msg.bufferSize = MIN( MMS_MAX_CHUNK_SIZE, fsm->post.msg.fileSize);
        fsm->post.msg.buffer = M_CALLOC(fsm->post.msg.bufferSize);
        break;
    case MMS_M_NOTIFY_RESP :
    case MMS_M_ACKNOWLEDGE_IND :
    case MMS_M_READ_REC_IND :
    case MMS_M_FORWARD_REQ :
        fsm->post.pdu.buffer = param->data.pdu.packet;
        fsm->post.pdu.bufferSize = param->data.pdu.length;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "POST: Can't be here. Already tested (%d)!\n", fsm->type));
        return -1L;
    } 

    M_FREE(sig->p_param);

    if (scheme == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Unable to create URL\n",
            __FILE__, __LINE__));
        return -1L;
    }

    if (port == 0)
    {
        
        len = strlen(scheme) + strlen(server) + strlen(slash) + strlen(uri) + 1;
        fsm->uri = M_ALLOC(len);

        if (sprintf( fsm->uri, "%s%s%s%s", scheme, server, slash, uri) > (int)len)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Wrote outside allocated memory\n",
                __FILE__, __LINE__));
            mmsNotifyError(MMS_RESULT_RESTART_NEEDED);
            return -1L;
        } 
    }
    else
    {
        
        len = strlen(scheme) + strlen(server) + 1 + LEN_POST_PORT + strlen(slash) + strlen(uri) + 1;
        fsm->uri = M_ALLOC(len);

        if (sprintf( fsm->uri, "%s%s:%d%s%s", scheme, server, port, slash, uri) > (int)len)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Wrote outside allocated memory\n",
                __FILE__, __LINE__));
            mmsNotifyError(MMS_RESULT_RESTART_NEEDED);
            return -1L;
        } 
    } 

    fsm->requestId = mmsWapGetRequestId();

    return instance;
} 






static void postPdu(long instance)
{
    CohPostInstanceData *fsm;   
    MmsHeaderValue read;

    fsm = fsmInstance[instance];

    memset( fsm->transactionId, 0, sizeof(fsm->transactionId));
    if ( mmsPduGet( (char *)fsm->post.pdu.buffer, 
        fsm->post.pdu.bufferSize, X_MMS_TRANSACTION_ID, &read) && 
        read.transactionId != NULL)
    {
        strncpy( fsm->transactionId, read.transactionId, 
            sizeof(fsm->transactionId) - 1);
    } 

    M_TIMER_SET_I( M_FSM_COH_POST, MMS_POST_TIMEOUT, instance);

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "====> POST %s (%d bytes) to uri=<%s>, instance=%d, transId=<%s>, reqId=%d\n", 
        fsm->type == MMS_M_SEND_REQ ? "MMS_M_SEND_REQ" :
            fsm->type == MMS_M_NOTIFY_RESP ? "MMS_M_NOTIFY_RESP" :
            fsm->type == MMS_M_ACKNOWLEDGE_IND ? "MMS_M_ACKNOWLEDGE_IND" :
            fsm->type == MMS_M_READ_REC_IND ? "MMS_M_READ_REC_IND" :
            fsm->type == MMS_M_FORWARD_REQ ? "MMS_M_FORWARD_REQ" : "UNKNOWN",
        fsm->post.pdu.bufferSize, fsm->uri, instance, fsm->transactionId, 
        fsm->requestId));

    mmsWapHttpPostPdu( fsm->requestId, fsm->uri, fsm->post.pdu.buffer, 
        (WE_INT32) fsm->post.pdu.bufferSize);
} 







static void postRetry(long instance, MmsResult ret)
{
    CohPostInstanceData *fsm;   

    fsm = fsmInstance[instance];

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "******* POST Retry ******* uri=<%s> transId=<%s> reqId=%d %s%d\n",
        fsm->uri == NULL ? "NULL" : fsm->uri, 
        fsm->transactionId, fsm->requestId,
        fsm->type == MMS_M_SEND_REQ ? "msgId=" : "pduType=",
        fsm->type == MMS_M_SEND_REQ ? 
            fsm->post.msg.wid : (int)fsm->post.pdu.buffer[1]));

    ++fsm->retries;
    if (((fsm->retries < MMS_POST_RETRY) && fsm->type!=MMS_M_SEND_REQ) || ((fsm->retries < MMS_POST_MSG_RETRY) && fsm->type==MMS_M_SEND_REQ))
    {
      
        mmsWapHttpCancel(fsmInstance[instance]->requestId);

        abortResponsePipe(fsm);

        switch (fsm->type)
        {
        case MMS_M_SEND_REQ :
            fsm->post.msg.totalWritten = 0;
            closePostPipe(fsm);

            if (fsm->post.msg.asyncOper != NULL)
            {
                asyncOperationStop(&fsm->post.msg.asyncOper);
            } 

            


            fsm->post.msg.asyncId = mmsWapGetTransactionId();

            fsm->post.msg.asyncOper = asyncOperationStart(
                Mms_FldrMsgIdToName(fsm->post.msg.wid, MMS_SUFFIX_SEND_REQ), 
                M_FSM_COH_POST, fsm->post.msg.asyncId, 
                MMS_SIG_COH_POST_ASYNC_READ_FINISHED, ASYNC_MODE_READ);
            break;
        case MMS_M_NOTIFY_RESP :
        case MMS_M_ACKNOWLEDGE_IND :
        case MMS_M_READ_REC_IND :
        case MMS_M_FORWARD_REQ :
            break;
            
        case MMS_M_SEND_CONF:
        case MMS_M_NOTIFICATION_IND:
        case MMS_M_RETRIEVE_CONF:
        case MMS_M_READ_ORIG_IND:
        case MMS_M_DELIVERY_IND:
        case MMS_M_FORWARD_CONF:
        default :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Illegal type (%d)!\n", __FILE__, __LINE__, fsm->type));
            break;
        } 

        fsm->requestId = mmsWapGetRequestId();
        fsm->transactionId[0] = WE_EOS;

        


        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_START, instance, 
            M_FSM_COH_POST, MMS_SIG_COH_POST_CONNECTED);
    }
    else
    {
        
        postFinished( instance, NULL, ret);
    } 
} 






static WE_BOOL readPipe(long instance)
{
    int bytesRead;
    char *offset;
    int isOpen;
    long available;
    int status;    
    CohPostInstanceData *fsm;   

    fsm = fsmInstance[instance];
    offset = fsm->response.buffer;
    
    do
    {
        bytesRead = WE_PIPE_READ(fsm->response.pipe.handle, offset, 
            fsm->response.bytesToRead);
        
        if (bytesRead > 0)
        {
            offset += bytesRead;
            fsm->response.bytesToRead -= bytesRead;
            fsm->response.bytesRead += (WE_UINT32) bytesRead;
        }
        else if (bytesRead == WE_PIPE_ERROR_DELAYED)
        {
            
            status = WE_PIPE_STATUS(fsm->response.pipe.handle, &isOpen, &available);
            if (status == 0)
            {
                (void) WE_PIPE_POLL(fsm->response.pipe.handle);
            }
            else
            {
                WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                    "%s(%d): Wrong status from pipe\n", __FILE__, __LINE__,
                    status));
                return FALSE;
            } 
        } 
    } while (bytesRead > 0);
    
    if (bytesRead == WE_PIPE_ERROR_CLOSED || fsm->response.bytesToRead == 0)
    {
        
        M_SIGNAL_SENDTO_I( M_FSM_COH_POST, (int)MMS_SIG_COH_POST_DATA_COLLECTED,
            instance);
    }
    else if (bytesRead < 0 && bytesRead != WE_PIPE_ERROR_DELAYED)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Pipe read error %d\n", __FILE__, __LINE__, bytesRead));
        return FALSE;
    } 
    
    return TRUE;
} 









static long selectInstance(MmsMessageType type, MmsMsgId msgId)
{
    long i;
    long size;

    CohPostInstanceData *fsm;   
    
    for (i = 0L; i < MMS_MAX_FSM_POST_INSTANCES; i++) 
    {
        if (fsmInstance[i] == NULL)
        {
            
            fsm = (CohPostInstanceData *)M_CALLOC(sizeof(CohPostInstanceData));

            fsm->requestingFsm = M_FSM_MAX_REGS;
            fsm->returnSig = MMS_SIG_COMMON_BASE;
            fsm->connected = FALSE;
            fsm->response.pipe.handle = -1;
            fsm->response.bytesRead = 0;
            fsm->type = type;

            switch (fsm->type)
            {
            case MMS_M_SEND_REQ :
                fsm->post.msg.pipe.handle = -1;
                fsm->post.msg.wid = msgId;

                


                fsm->post.msg.asyncId = mmsWapGetTransactionId();

                size = WE_FILE_GETSIZE( 
                    Mms_FldrMsgIdToName(msgId, MMS_SUFFIX_SEND_REQ));
                if (size < 0)
                {
                    return -1L;
                }
                fsm->post.msg.fileSize = (WE_UINT32) size;
                fsm->post.msg.asyncOper = asyncOperationStart(
                    Mms_FldrMsgIdToName(fsm->post.msg.wid, MMS_SUFFIX_SEND_REQ), 
                    M_FSM_COH_POST, fsm->post.msg.asyncId, 
                    MMS_SIG_COH_POST_ASYNC_READ_FINISHED, ASYNC_MODE_READ);
                break;
            case MMS_M_NOTIFY_RESP :
            case MMS_M_ACKNOWLEDGE_IND :
            case MMS_M_READ_REC_IND :
            case MMS_M_FORWARD_REQ :
                break;

            case MMS_M_SEND_CONF:
            case MMS_M_NOTIFICATION_IND:
            case MMS_M_RETRIEVE_CONF:
            case MMS_M_READ_ORIG_IND:
            case MMS_M_DELIVERY_IND:
            case MMS_M_FORWARD_CONF:
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Illegal type (%d)!\n", __FILE__, __LINE__, fsm->type));
                break;
            } 

            fsmInstance[i] = fsm;
            
            return i;
        } 
    } 

    return -1L;
} 








static WE_BOOL storeTransactionId(long instance)
{
    MmsHeaderValue  read;
    CohPostInstanceData *fsm;   
    int maxLenTransId;
    WE_BOOL ret = FALSE;

    fsm = fsmInstance[instance];
    maxLenTransId = MIN(((int)fsm->post.msg.bytesInBuffer) - START_POS_TRANSACTION_ID, 
        MAX_LEN_TRANSACTION_ID);

    


    if (fsm->post.msg.asyncOper == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Not enough memory.\n", __FILE__, __LINE__));
    }
    else if ( !mmsPduGet( fsm->post.msg.buffer, fsm->post.msg.bytesInBuffer, 
        X_MMS_TRANSACTION_ID, &read))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't find transaction WID in msg to POST.\n", 
            __FILE__, __LINE__));
    }
    else if ( !stringIsValid( read.transactionId, maxLenTransId) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Invalid transaction WID in msg to POST.\n", 
            __FILE__, __LINE__));
    }
    else
    {
        strncpy( fsm->transactionId, read.transactionId, sizeof(fsm->transactionId));
        fsm->transactionId[sizeof(fsm->transactionId) - 1] = WE_EOS;
        ret = TRUE;
    } 

    return ret;
} 








static WE_BOOL stringIsValid( const char *str, int len)
{
    if (str == NULL || len <= 0)
    {
        return FALSE;
    } 

    while (*str++ && --len)
    {
        
    } 

    return len > 0;
} 








static WE_BOOL writePipe( long instance, WE_BOOL atEof)
{
    CohPostInstanceData *fsm;
    int written;
    int bytesToWrite;
    char *pos;

    fsm = fsmInstance[instance];
    bytesToWrite = ((int) fsm->post.msg.bytesInBuffer) - ((int) fsm->post.msg.bytesWritten);
    pos = fsm->post.msg.buffer + fsm->post.msg.bytesWritten;

    

    M_TIMER_RESTART_I(M_FSM_COH_POST, MMS_POST_TIMEOUT, instance);

    while (bytesToWrite > 0)
    {
        if (pos >= fsm->post.msg.buffer + fsm->post.msg.bufferSize)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Buffer overwrite.\n", __FILE__, __LINE__));
            return FALSE;
        } 

        written = WE_PIPE_WRITE( fsm->post.msg.pipe.handle, pos, 
            bytesToWrite);
        if (written == 0 || written == WE_PIPE_ERROR_DELAYED)
        {
            return (WE_PIPE_POLL( fsm->post.msg.pipe.handle) >= 0);
        }
        else if (written < 0)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to write to pipe. Error=%d.\n", 
                __FILE__, __LINE__, written));
            return FALSE;
        }
        else
        {
            fsm->post.msg.bytesWritten += (WE_UINT32) written;
            fsm->post.msg.totalWritten += (WE_UINT32) written;
            bytesToWrite -= written;
            pos += written;
        } 
    } 

    if ( !atEof && fsm->post.msg.totalWritten < fsm->post.msg.fileSize)
    {
        

        fsm->post.msg.bytesInBuffer = 0;
        fsm->post.msg.bytesWritten = 0;
        asyncReadContinue( fsm->post.msg.asyncOper, fsm->post.msg.bufferSize);
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS POST: Written everything to STK, close PIPE.\n")); 

        closePostPipe(fsm); 
    } 

    cohPostProgressStatus(fsm, MMS_PROGRESS_STATUS_PROGRESS);
    return TRUE;
} 

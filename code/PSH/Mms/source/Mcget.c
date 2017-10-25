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
#include "We_File.h"    
#include "We_Pipe.h"   
#include "We_Cmmn.h"    

#include "Stk_If.h"     

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     

#include "Msig.h"       
#include "Mtimer.h"     
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mcget.h"      
#include "Mcpost.h"     
#include "Mcwap.h"      
#include "Mconfig.h"    
#include "Masync.h"     
#include "Mcwsp.h"      
#include "Mmsrpl.h"     


#define MMS_MAX_FSM_GET_INSTANCES   1
#define MMS_MIN_GET_BUFFER_SIZE     512




typedef struct
{
    MmsStateMachine eRequestingFsm;  
    long            lFsmInstance;    
    MmsSignalId     eReturnSig;      

    MmsRequestId    iRequestId;      
    char            *pcUri;           
    int             iRetries;        

    WE_BOOL        bConnected;      
    char            *pcNotifTransId;  
    char            *pcRetrieveTransId; 
    WE_UINT32      uiMsgId;          
    char            *pcPath;          

    MmsHttpContent  *pstContent;       
    int             iPipeHandle;     
    char            *pcPipeName;      
    char            *pcBuffer;        
    WE_UINT32      uiBufferSize;     
    WE_UINT32      uiBytesInBuffer;  
    WE_UINT32      uiBytesWritten;   
    unsigned long   ulMsgSize;        
    WE_BOOL        bHeaderIsVerified; 
    WE_BOOL        bIsAcknowledgeIndRequested;
    AsyncOperationHandle pAsyncOper; 
    WE_BOOL        bIsImmediate;    
    WE_UINT32      uiLastProgress;   
} ST_COHGETINSTANCEDATA;

static ST_COHGETINSTANCEDATA *apstFsmInstance[MMS_MAX_FSM_GET_INSTANCES];


static void abortPipe(ST_COHGETINSTANCEDATA *pstFsm);
static void cohGetMain(MmsSignal *pstSig);
static int cohGetPipeOwner(int iHandle);
static void collectData
(
	long lInstance,
	const MmsSignal *pstSig
);
static void deleteInstance(long lInstance);
static void getFinished
(
	long lInstance,
	MmsResult eRet
);
static long getInstance(const MmsSignal *pstSig);
static void getMsg(long lInstance);
static long getMsgInit(const MmsSignal *pstSig);
static void getRetry
(
	long lInstance,
	MmsResult eRes
);
static void handleCancel(MmsMsgId uiMsgId);
static void handleConnected
(
	long lInstance,
	MmsResult eResult
);
static void handleWriteFinished
(
	long lInstance,
	MmsResult eResult, 
       WE_UINT32 uiBytesWritten
);
static WE_BOOL readPipe(long lInstance);
static long selectInstance
(
	const char *pcPath,
	unsigned long ulSize
);
static MmsResult verifyHeader
(
	long lInstance,
	WE_BOOL *pbPermanentError
);
static void cohSendProgressStatus
(
	ST_COHGETINSTANCEDATA *pstFsm,
	WE_UINT8 uiState
);

#ifdef WE_LOG_MODULE 
const char *fsmCOHGetSigName(WE_UINT32 uiSigType)
{
    switch (uiSigType)
    {
    case MMS_SIG_COH_GET_CANCEL:
        return "MMS_SIG_COH_GET_CANCEL";
    case MMS_SIG_COH_GET_MSG:
        return "MMS_SIG_COH_GET_MSG";
    case MMS_SIG_COH_GET_RSP:
        return "MMS_SIG_COH_GET_RSP";
    case MMS_SIG_COH_GET_CONNECTED:
        return "MMS_SIG_COH_GET_CONNECTED";
    case MMS_SIG_COH_GET_DISCONNECTED:
        return "MMS_SIG_COH_GET_DISCONNECTED";
    case MMS_SIG_COH_GET_DATA_COLLECTED:
        return "MMS_SIG_COH_GET_DATA_COLLECTED";
    case MMS_SIG_COH_GET_PIPE_NOTIFY:
        return "MMS_SIG_COH_GET_PIPE_NOTIFY";
    case M_FSM_COH_GET_ASYNC_WRITE_FINISHED:
        return "M_FSM_COH_GET_ASYNC_WRITE_FINISHED";
    default:
        return 0;
    }

} 
#endif

static void abortPipe(ST_COHGETINSTANCEDATA *pstFsm)
{
    if (pstFsm->iPipeHandle >= 0)
    {
        (void) WE_PIPE_CLOSE(pstFsm->iPipeHandle);
        pstFsm->iPipeHandle = -1;
    } 

    if (pstFsm->pcPipeName != NULL)
    {
        (void) WE_PIPE_DELETE(pstFsm->pcPipeName);
        M_FREE(pstFsm->pcPipeName);
        pstFsm->pcPipeName = NULL;
    } 
} 

void cohGetEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_COH_GET);
} 

void cohGetCancel(MmsMsgId uiMsgId)
{
    M_SIGNAL_SENDTO_U( M_FSM_COH_GET, (int)MMS_SIG_COH_GET_CANCEL, uiMsgId);
} 

void cohGetInit(void)
{
    int i;
    
    for (i = 0; i < MMS_MAX_FSM_GET_INSTANCES; ++i)
    {
        apstFsmInstance[i] = NULL;
    } 

    mSignalRegisterDst(M_FSM_COH_GET, cohGetMain);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH GET: initialized\n"));
} 

long cohGetInstance(MmsRequestId wid)
{
    long i;
    
    
    for (i = 0L; i < MMS_MAX_FSM_GET_INSTANCES; ++i)
    {
        if (apstFsmInstance[i] != NULL && apstFsmInstance[i]->iRequestId == wid)
        {
            return i;
        } 
    } 

    return -1L;
} 

static void cohSendProgressStatus
(
	ST_COHGETINSTANCEDATA *pstFsm,
	WE_UINT8 uiState
)
{
    if (pstFsm->bIsImmediate)
    {
        replyProgressStatus(MMS_PROGRESS_STATUS_IMMRETRIEVE, uiState,
            pstFsm->uiMsgId, pstFsm->uiBytesWritten, pstFsm->ulMsgSize,
            &pstFsm->uiLastProgress);
    }
    else
    {
        replyProgressStatus(MMS_PROGRESS_STATUS_RETRIEVE, uiState,
            pstFsm->uiMsgId, pstFsm->uiBytesWritten, pstFsm->ulMsgSize,
            &pstFsm->uiLastProgress);
    }
} 
WE_BOOL cohGetIsPipeOwner(int iHandle)
{
    return cohGetPipeOwner(iHandle) != -1;
} 
static int cohGetPipeOwner(int iHandle)
{
    int i;

    for (i = 0; i < MMS_MAX_FSM_GET_INSTANCES; ++i)
    {
        if (apstFsmInstance[i] != NULL && apstFsmInstance[i]->iPipeHandle == iHandle)
        {
            return i;
        } 
    } 

    return -1;
} 
static void cohGetMain(MmsSignal *pstSig)
{
    long lInstance = -1L;

    switch (pstSig->type)
    {
    case MMS_SIG_COH_GET_CANCEL : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: MMS_SIG_COH_GET_CANCEL for uiMsgId=%d\n", 
            pstSig->u_param1));
        handleCancel((MmsMsgId)pstSig->u_param1);
        break;
    case MMS_SIG_COH_GET_MSG :  
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: MMS_SIG_COH_GET_MSG\n"));

        
        if ((lInstance = getMsgInit(pstSig)) != -1L)
        {
            
            M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_START, 
                lInstance, M_FSM_COH_GET, MMS_SIG_COH_GET_CONNECTED);

            cohSendProgressStatus(apstFsmInstance[lInstance], MMS_PROGRESS_STATUS_START);
        } 
        break;        
    case MMS_SIG_COH_GET_CONNECTED : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: Received MMS_SIG_COH_GET_CONNECTED\n"));
        handleConnected( pstSig->i_param, (MmsResult)pstSig->u_param1);
        break;        
    case MMS_SIG_COH_GET_DISCONNECTED : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: Received MMS_SIG_COH_GET_DISCONNECTED\n"));
        break;        
    case MMS_SIG_COH_GET_RSP : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: Received MMS_SIG_COH_GET_RSP\n"));

        if ((lInstance = getInstance(pstSig)) != -1)
        {
            collectData( lInstance, pstSig);
        } 
        break;
    case MMS_SIG_COH_GET_PIPE_NOTIFY :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: Received MMS_SIG_COH_GET_PIPE_NOTIFY\n"));

        if ( (lInstance = cohGetPipeOwner((WE_INT32) pstSig->u_param1)) != -1)
        {
            (void) readPipe(lInstance);
        } 
        break;
    case MMS_SIG_COH_GET_DATA_COLLECTED :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: Received MMS_SIG_COH_GET_DATA_COLLECTED\n"));

        if ((lInstance = getInstance(pstSig)) != -1)
        {
            if ((MmsResult)pstSig->u_param1 != MMS_RESULT_OK)
            {
                getRetry( lInstance, (MmsResult)pstSig->u_param1);
            }
            else
            {
                getFinished( lInstance, MMS_RESULT_OK);
            } 
        } 
        break;
    case M_FSM_COH_GET_ASYNC_WRITE_FINISHED :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: Received M_FSM_COH_GET_ASYNC_WRITE_FINISHED\n"));
        handleWriteFinished( pstSig->i_param, (MmsResult)pstSig->u_param1, pstSig->u_param2);
        break;
    case MMS_SIG_COMMON_TIMER_EXPIRED :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: Received TIMER_EXPIRED\n"));

        if ((lInstance = getInstance(pstSig)) != -1)
        {
            getRetry( lInstance, MMS_RESULT_COMM_TIMEOUT);
        } 
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH GET: received unknown signal\n"));
        break;
    } 
    
    mSignalDelete(pstSig);
} 

void cohGetTerminate(void)
{
    long i;

    
    for (i = 0L; i < MMS_MAX_FSM_GET_INSTANCES; ++i)
    {
        if (apstFsmInstance[i] != NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "******* GET FSM active at terminate ******* %s %s %d\n",
                apstFsmInstance[i]->pcUri == NULL ?
                    "NULL" : apstFsmInstance[i]->pcUri, 
                apstFsmInstance[i]->pcNotifTransId == NULL ?
                    "NULL" : apstFsmInstance[i]->pcNotifTransId, 
                apstFsmInstance[i]->uiMsgId));

            deleteInstance(i);
        } 
    } 

    mSignalDeregister(M_FSM_COH_GET);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH GET: terminated\n"));
} 
static void collectData
(
	long lInstance,
	const MmsSignal *pstSig
)
{
    MmsResult           eRes = MMS_RESULT_OK;
    MmsHttpContent      *p; 
    ST_COHGETINSTANCEDATA  *pstFsm;   

    pstFsm = apstFsmInstance[lInstance];

    
    M_TIMER_RESET_I( M_FSM_COH_GET, lInstance);

    
    p = pstFsm->pstContent = (MmsHttpContent *)pstSig->p_param;
    if (p == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "GET collectData: No data returned (%d)\n", lInstance));
        eRes = MMS_RESULT_COMM_FAILED;
    }
    else if (p->errorNo != MMS_RESULT_OK)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "GET collectData: Wrong return code from STK, error (%d)\n", 
            p->errorNo));

        eRes = p->errorNo;
    }
    else if (p->wid != pstFsm->iRequestId)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "GET collectData: Wrong request WID %d != %d\n", 
            p->wid, pstFsm->iRequestId));
        eRes = MMS_RESULT_COMM_FAILED;
    }
    else if (p->dataType != STK_BODY_PIPE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "GET collectData: Data expected in pipe, error (%d)\n", 
            p->errorNo));

        if ( p->errorNo == MMS_RESULT_OK)
        {
            eRes = MMS_RESULT_COMM_FAILED;
        }
        else
        {
            eRes = p->errorNo;
        } 
    }
    else if (p->data.pipeName == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "GET collectData: No pipe name returned, error (%d)\n", 
            p->errorNo));
        if ( p->errorNo == MMS_RESULT_OK)
        {
            eRes = MMS_RESULT_COMM_FAILED;
        }
        else
        {
            eRes = p->errorNo;
        } 
    }
    else if ((pstFsm->iPipeHandle = WE_PIPE_OPEN( WE_MODID_MMS, p->data.pipeName)) < 0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "GET collectData: Failed to open the pipe <%s>\n", p->data.pipeName));
        eRes = MMS_RESULT_COMM_FAILED;
    }
    else if (!readPipe(lInstance))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "GET collectData: readPipe failed\n"));
        eRes = MMS_RESULT_COMM_FAILED;
    }
    else
    {
        pstFsm->pcPipeName = we_cmmn_strdup( WE_MODID_MMS, p->data.pipeName);
    } 

    if (eRes != MMS_RESULT_OK)
    {
        if (NULL != p && p->dataType == STK_BODY_PIPE && 
            NULL != p->data.pipeName)
        {
            (void) WE_PIPE_DELETE(p->data.pipeName);
        }

        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "GET collectData: starting retry\n"));
        getRetry( lInstance, eRes);
    } 

    mmsWapFreeContentParams(p);
} 
static void deleteInstance(long lInstance)
{
    ST_COHGETINSTANCEDATA *pstFsm;   

    if (lInstance < 0 || (pstFsm = apstFsmInstance[lInstance]) == NULL)
    {
        return;
    } 

    M_TIMER_RESET_I( M_FSM_COH_GET, lInstance); 

    
    mmsWapHttpCancel( apstFsmInstance[lInstance]->iRequestId);

    if (pstFsm->pAsyncOper != NULL)
    {
        asyncOperationStop(&pstFsm->pAsyncOper);
    } 

    if (pstFsm->bConnected)
    {
        
        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_STOP,lInstance, 
            M_FSM_COH_GET, MMS_SIG_COH_GET_DISCONNECTED);
    } 

    abortPipe(pstFsm);

    M_FREE(pstFsm->pcNotifTransId);
    M_FREE(pstFsm->pcRetrieveTransId);
    M_FREE(pstFsm->pcPath);
    M_FREE(pstFsm->pcUri);
    M_FREE(pstFsm->pcBuffer);
    M_FREE(pstFsm);
    apstFsmInstance[lInstance] = NULL;
} 
static void getFinished
(
	long lInstance,
	MmsResult eRet
)
{
    MmsSigCohGetResultParam *pstParam = M_ALLOCTYPE(MmsSigCohGetResultParam);

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "====> GET Finished: Result=%d ulSize=%d lInstance=%d pcUri=<%s>\n", 
        eRet, apstFsmInstance[lInstance]->uiBytesWritten,
        lInstance, apstFsmInstance[lInstance]->pcUri));

    if (pstParam == NULL) 
    {
        return;
    }

    pstParam->msgId = apstFsmInstance[lInstance]->uiMsgId;
    pstParam->result = eRet;
    pstParam->size = apstFsmInstance[lInstance]->uiBytesWritten;
    if (apstFsmInstance[lInstance]->bIsAcknowledgeIndRequested)
    {
        pstParam->transactionId = apstFsmInstance[lInstance]->pcRetrieveTransId;
        apstFsmInstance[lInstance]->pcRetrieveTransId = NULL;
    }
    else
    {
        pstParam->transactionId = NULL;
    }
    
    if (eRet == MMS_RESULT_OK)
    {
        cohSendProgressStatus(apstFsmInstance[lInstance], MMS_PROGRESS_STATUS_COMPLETED);
    }
    else
    {
        cohSendProgressStatus(apstFsmInstance[lInstance], MMS_PROGRESS_STATUS_STOP);
    }
    
    
    M_SIGNAL_SENDTO_IP(apstFsmInstance[lInstance]->eRequestingFsm, 
        apstFsmInstance[lInstance]->eReturnSig, 
        apstFsmInstance[lInstance]->lFsmInstance,
        pstParam);

    deleteInstance(lInstance);
} 
static long getInstance(const MmsSignal *pstSig)
{
    long lInstance = -1L;

    lInstance = pstSig->i_param;

    if (lInstance < 0L || lInstance >= MMS_MAX_FSM_GET_INSTANCES ||
        apstFsmInstance[lInstance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "GET getInstance: lInstance out of bounds (%d)\n", lInstance));
        lInstance = -1L;
    } 

    return lInstance;
} 

static void getMsg(long lInstance)
{
    ST_COHGETINSTANCEDATA *pstFsm;   

    pstFsm = apstFsmInstance[lInstance];

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "====> GET PDU pcUri=<%s> iRequestId %d\n", pstFsm->pcUri, pstFsm->iRequestId));

    M_TIMER_SET_I( M_FSM_COH_GET, MMS_GET_TIMEOUT, lInstance);
    mmsWapHttpGet( pstFsm->iRequestId, pstFsm->pcUri);
} 
static long getMsgInit(const MmsSignal *pstSig)
{
    long lInstance = 0;
    MmsResult eRes = MMS_RESULT_OK;
    ST_COHGETINSTANCEDATA *pstFsm;   
    MmsSigCohGetParam   *pstParam = (MmsSigCohGetParam *)pstSig->p_param;

    if (pstParam == NULL || pstParam->msgId == 0 || pstParam->uri == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Illegal indata: Parameters are not given correct!\n",
            __FILE__, __LINE__));

        eRes = MMS_RESULT_ERROR; 
    }
    else if ((lInstance = selectInstance(pstParam->path, pstParam->size)) == -1L)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): We're BUSY! Return!\n", __FILE__, __LINE__));

        eRes = MMS_RESULT_BUSY; 
    }
    else if (lInstance < 0L || lInstance >= MMS_MAX_FSM_GET_INSTANCES ||
        apstFsmInstance[lInstance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): lInstance out of bounds (%d)\n", 
            __FILE__, __LINE__, lInstance));

        eRes = MMS_RESULT_ERROR; 
    } 

    if (eRes != MMS_RESULT_OK)
    {
        
        M_SIGNAL_SENDTO_IU( (MmsStateMachine)pstSig->u_param1, pstSig->u_param2, 
            pstSig->i_param, eRes);

        if (pstParam != NULL)
        {
            if (pstParam->path != NULL)
            {
                M_FREE(pstParam->path);
            } 

            if (pstParam->uri != NULL)
            {
                M_FREE(pstParam->uri);
            } 

            M_FREE(pstParam);
        } 

        return -1L;
    } 
    pstFsm = apstFsmInstance[lInstance];
    pstFsm->eRequestingFsm = (MmsStateMachine)pstSig->u_param1;
    pstFsm->eReturnSig = (MmsSignalId)pstSig->u_param2;
    pstFsm->lFsmInstance = pstSig->i_param;
    pstFsm->bIsImmediate = pstParam->isImmediate;
        
    pstFsm->ulMsgSize = pstParam->size;
    pstFsm->uiBytesWritten = 0;
    pstFsm->uiBytesInBuffer = 0;
    pstFsm->iRetries = 0;
    pstFsm->uiMsgId = pstParam->msgId;

    

    pstFsm->pcUri = pstParam->uri;
    pstFsm->pcPath = pstParam->path;
    pstFsm->pcNotifTransId = pstParam->transactionId;
    M_FREE(pstParam);  

    pstFsm->iRequestId = mmsWapGetRequestId();

    return lInstance;
} 

static void getRetry
(
	long lInstance,
	MmsResult eRes
)
{
    ST_COHGETINSTANCEDATA *pstFsm;   

    pstFsm = apstFsmInstance[lInstance];
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "******* GET Retry ******* pcUri=<%s> transId=<%s> iRequestId=%d uiMsgId=%d\n",
        pstFsm->pcUri == NULL ? "NULL" : pstFsm->pcUri, 
        pstFsm->pcNotifTransId == NULL ? "NULL" : pstFsm->pcNotifTransId, 
        pstFsm->iRequestId, pstFsm->uiMsgId));

    
    pstFsm->uiBytesWritten = 0;
    pstFsm->uiBytesInBuffer = 0;
    pstFsm->bHeaderIsVerified = FALSE;
    abortPipe(pstFsm);

    if (++pstFsm->iRetries < MMS_GET_RETRY)
    {
        
        mmsWapHttpCancel(pstFsm->iRequestId);
        
        pstFsm->iRequestId = mmsWapGetRequestId();

        


        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_START, lInstance, 
            M_FSM_COH_GET, MMS_SIG_COH_GET_CONNECTED);
    }
    else
    {   
        getFinished( lInstance, eRes);
    } 
} 
static void handleCancel(MmsMsgId uiMsgId)
{
    int i;

    for (i = 0; i < MMS_MAX_FSM_GET_INSTANCES; ++i)
    {
        if (apstFsmInstance[i] != NULL)
        {
            if (apstFsmInstance[i]->uiMsgId == uiMsgId)
            {
                getFinished( i, MMS_RESULT_CANCELLED_BY_USER);
            } 
        } 
    } 
} 
static void handleConnected
(
	long lInstance,
	MmsResult eResult
)
{
    if (lInstance < 0 || lInstance >= MMS_MAX_FSM_GET_INSTANCES || 
        apstFsmInstance[lInstance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong lInstance %d\n", __FILE__, __LINE__, lInstance));
    }
    else if (eResult == MMS_RESULT_OK)
    {
        apstFsmInstance[lInstance]->bConnected = TRUE;
        getMsg(lInstance);
    }
    else if (++apstFsmInstance[lInstance]->iRetries < MMS_GET_RETRY)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't establish connection %d. Retry!\n", 
            __FILE__, __LINE__, eResult));

        
        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_START, 
            lInstance, M_FSM_COH_GET, MMS_SIG_COH_GET_CONNECTED);
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't establish connection %d\n", 
            __FILE__, __LINE__, eResult));
        
        getFinished( lInstance, eResult);
    } 
} 
static void handleWriteFinished
(
	long lInstance,
	MmsResult eResult, 
       WE_UINT32 uiBytesWritten
)
{
    WE_BOOL pbPermanentError = FALSE;
    WE_BOOL shallReadMore = FALSE;
    ST_COHGETINSTANCEDATA *pstFsm;   

    if (lInstance < 0 || lInstance >= MMS_MAX_FSM_GET_INSTANCES || 
        apstFsmInstance[lInstance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong lInstance %d\n", __FILE__, __LINE__, lInstance));
        return;
    } 

    pstFsm = apstFsmInstance[lInstance];
    if (eResult == MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Persistent storage is full %d\n", 
            __FILE__, __LINE__, eResult));
        
        getFinished( lInstance, eResult);
    }
    else if (eResult != MMS_RESULT_OK)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Async operation failed %d\n", 
            __FILE__, __LINE__, eResult));

        getRetry( lInstance, eResult);
    }
    else if (cfgGetInt(MMS_CFG_MAX_RETRIEVAL_SIZE) > 0 && 
        pstFsm->uiBytesWritten > 
        (WE_UINT32)cfgGetInt(MMS_CFG_MAX_RETRIEVAL_SIZE))
    {
        eResult = MMS_RESULT_MAX_RETRIEVAL_SIZE_EXCEEDED;
        getFinished( lInstance, eResult);
    }
    else if (uiBytesWritten != pstFsm->uiBytesInBuffer)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrote only %d bytes out of %d\n", 
            __FILE__, __LINE__, uiBytesWritten, pstFsm->uiBytesInBuffer));

        getRetry( lInstance, eResult);
    }
    else if ( pstFsm->bHeaderIsVerified )
    {
        shallReadMore = TRUE;
    }
    else
    {
        eResult = verifyHeader( lInstance, &pbPermanentError);
          

        if (pbPermanentError)
        {
            getFinished( lInstance, eResult);
        }
        else if ( eResult != MMS_RESULT_OK)
        {
            getRetry( lInstance, eResult);
        }
        else
        {
            pstFsm->bHeaderIsVerified = TRUE;
            shallReadMore = TRUE;
        } 
    } 

    if (shallReadMore)
    {
        pstFsm->uiBytesInBuffer = 0;
        pstFsm->uiBytesWritten += uiBytesWritten;
        (void) readPipe(lInstance);
    } 
} 
static WE_BOOL readPipe(long lInstance)
{
    long lBytesRead;
    char *pcOffset;
    int iOpen;
    long lAvailable;
    int iStatus;    
    ST_COHGETINSTANCEDATA *pstFsm;   

    
    M_TIMER_RESET_I( M_FSM_COH_GET, lInstance);
    pstFsm = apstFsmInstance[lInstance];
    pcOffset = pstFsm->pcBuffer + pstFsm->uiBytesInBuffer;
    
    do
    {
        if (pstFsm->uiBytesInBuffer >= pstFsm->uiBufferSize)
        {
            
            asyncWrite( pstFsm->pAsyncOper, (WE_INT32) pstFsm->uiBytesWritten, pstFsm->pcBuffer, 
                pstFsm->uiBytesInBuffer);

            cohSendProgressStatus(pstFsm, MMS_PROGRESS_STATUS_PROGRESS);
            return TRUE;
        } 

        lBytesRead = WE_PIPE_READ(pstFsm->iPipeHandle, pcOffset, 
                    (long) (pstFsm->uiBufferSize - pstFsm->uiBytesInBuffer));
        
        if (lBytesRead > 0)
        {
            pcOffset += lBytesRead;
            pstFsm->uiBytesInBuffer += (WE_UINT32) lBytesRead;
        }
        else if (lBytesRead == WE_PIPE_ERROR_DELAYED)
        {
            
            iStatus = WE_PIPE_STATUS( pstFsm->iPipeHandle, &iOpen, &lAvailable);
            if (iStatus == 0)
            {  

                (void) WE_PIPE_POLL(pstFsm->iPipeHandle);
            }
            else
            {
                WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                    "%s(%d): Wrong status from pipe\n", __FILE__, __LINE__,
                    iStatus));
                return FALSE;
            } 
        }
        else if (lBytesRead == WE_PIPE_ERROR_CLOSED)
        {   

            

            if (pstFsm->uiBytesInBuffer > 0)
            {
                asyncWrite( pstFsm->pAsyncOper, (WE_INT32) pstFsm->uiBytesWritten, pstFsm->pcBuffer, 
                    pstFsm->uiBytesInBuffer);
            }
            else
            {
                M_SIGNAL_SENDTO_I( M_FSM_COH_GET, (int)MMS_SIG_COH_GET_DATA_COLLECTED,
                    lInstance);
            } 
        }
        else if (lBytesRead < 0)
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                "%s(%d): Pipe read error %d\n", __FILE__, __LINE__, lBytesRead));
            return FALSE;
        } 
    } while (lBytesRead > 0);
    
    cohSendProgressStatus(pstFsm, MMS_PROGRESS_STATUS_PROGRESS);
    return TRUE;
} 
static long selectInstance
(
	const char *pcPath,
	unsigned long ulSize
)
{
    long i;
    ST_COHGETINSTANCEDATA *pstFsm;   

    for (i = 0L; i < MMS_MAX_FSM_GET_INSTANCES; i++) 
    {
        if (apstFsmInstance[i] == NULL)
        {
            
            pstFsm = (ST_COHGETINSTANCEDATA *)M_CALLOC(sizeof(ST_COHGETINSTANCEDATA));

            if ( (pstFsm->pAsyncOper = asyncOperationStart( pcPath, M_FSM_COH_GET, 
                i, M_FSM_COH_GET_ASYNC_WRITE_FINISHED, ASYNC_MODE_WRITE)) == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): No memory for async operation.\n", __FILE__, __LINE__));

                M_FREE(pstFsm);
                return -1;
            } 

            pstFsm->uiBufferSize = MIN( MMS_MAX_CHUNK_SIZE, ulSize);
            pstFsm->uiBufferSize = MAX( MMS_MIN_GET_BUFFER_SIZE, pstFsm->uiBufferSize);
            pstFsm->pcBuffer = M_CALLOC(pstFsm->uiBufferSize);
            pstFsm->bHeaderIsVerified = FALSE;
            pstFsm->uiBytesInBuffer = 0;
            pstFsm->uiBytesWritten = 0;
            pstFsm->iPipeHandle = -1;
            pstFsm->eRequestingFsm = M_FSM_MAX_REGS;
            pstFsm->eReturnSig = MMS_SIG_COMMON_BASE;
            pstFsm->bConnected = FALSE;

            apstFsmInstance[i] = pstFsm;

            return i;
        } 
    } 

    return -1L;
} 
static MmsResult verifyHeader
(
	long lInstance,
	WE_BOOL *pbPermanentError
)
{
    MmsResult       eRes = MMS_RESULT_OK;
    MmsHeaderValue  stRead;
    ST_COHGETINSTANCEDATA *pstFsm;   

    *pbPermanentError = FALSE;
    pstFsm = apstFsmInstance[lInstance];
    pstFsm->bIsAcknowledgeIndRequested = FALSE;

    if ( mmsPduUnrecognized( pstFsm->pcBuffer, pstFsm->uiBytesInBuffer) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Unrecognized PDU!\n", __FILE__, __LINE__));
        
        

        *pbPermanentError = TRUE;

        eRes = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if ( !mmsPduSanityCheck( pstFsm->pcBuffer, pstFsm->uiBytesInBuffer) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Badly formatted PDU!\n", __FILE__, __LINE__));
        eRes = MMS_RESULT_COMM_ILLEGAL_PDU;
    }
    else if ( !mmsPduGet( pstFsm->pcBuffer, pstFsm->uiBytesInBuffer, X_MMS_MESSAGE_TYPE, &stRead) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No data X_MMS_MESSAGE_TYPE header tag\n", 
            __FILE__, __LINE__));
        eRes = MMS_RESULT_COMM_HEADER_TAG;
    }
    else if (stRead.messageType != MMS_M_RETRIEVE_CONF)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong message type (%d)\n", __FILE__, __LINE__, 
            stRead.messageType));
        eRes = MMS_RESULT_COMM_UNEXPECTED_MESSAGE;
    }
#if MMS_STRICT_PDU_CHECK
    else if ( !mmsPduGet( pstFsm->pcBuffer, pstFsm->uiBytesInBuffer, MMS_DATE, &stRead) )
    {
        


        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "%s(%d): No data MMS_DATE header tag\n", __FILE__, __LINE__));
        eRes = MMS_RESULT_COMM_HEADER_TAG;
    }
#endif 
    else if ( !mmsPduGet( pstFsm->pcBuffer, pstFsm->uiBytesInBuffer, MMS_CONTENT_TYPE, &stRead) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No data MMS_CONTENT_TYPE header tag\n", 
            __FILE__, __LINE__));
        eRes = MMS_RESULT_COMM_HEADER_TAG;
    }
    else
    {
        
        if ( mmsPduGet( pstFsm->pcBuffer, pstFsm->uiBytesInBuffer, X_MMS_RETRIEVE_STATUS, &stRead) )
        {
            if (stRead.retrieveStatus != MMS_RTRV_STATUS_OK)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Wrong retrieve status (%d)\n", 
                    __FILE__, __LINE__, stRead.retrieveStatus));
                if (stRead.retrieveStatus >= MMS_RTRV_STATUS_P_FAILURE)
                {
                    *pbPermanentError = TRUE;
                } 

                eRes = mmsWapTranslateRetrieveStatus((MmsRetrieveStatus)stRead.retrieveStatus);
            } 
        } 

        
        if (eRes == MMS_RESULT_OK && 
            mmsPduGet( pstFsm->pcBuffer, pstFsm->uiBytesInBuffer, X_MMS_TRANSACTION_ID, &stRead))
        {
#if MMS_STRICT_TRANSACTION_ID_CHECK
			if (pstFsm->pcNotifTransId == NULL || 
                strcmp(pstFsm->pcNotifTransId, stRead.transactionId) != 0)
            {
                pstFsm->pcRetrieveTransId = we_cmmn_strdup( WE_MODID_MMS, 
                    stRead.transactionId);
                pstFsm->bIsAcknowledgeIndRequested = TRUE;
            } 
#else
			pstFsm->pcRetrieveTransId = we_cmmn_strdup( WE_MODID_MMS, 
                    stRead.transactionId);
            pstFsm->bIsAcknowledgeIndRequested = TRUE;
#endif 
        } 
    } 

    return eRes;
} 

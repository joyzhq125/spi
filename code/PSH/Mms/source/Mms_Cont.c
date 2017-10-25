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
#include "We_Pipe.h"   
#include "We_Sock.h"   
#include "We_Neta.h"   

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     

#include "Msig.h"       
#include "Mmem.h"       
#include "Mms_Cont.h"   
#include "Mmsrec.h"     
#include "Mmsrpl.h"     








typedef enum
{
    MMS_CONT_STATE_IDLE = 0,   
    MMS_CONT_STATE_RECEIVE,    
    MMS_CONT_STATE_MAX         
} MmsContState;



typedef struct AsyncOper
{
    MmsContState        state; 
    we_content_data_t* data;   
    int                 hPipe; 
    char                pdu[MMS_MAX_NOTIFICATION_SIZE]; 
    int                 pos;   
} MmsContOperation;

 

 

 

 

 

 
static MmsContOperation *contentOper;
static int contentQueueState = 0;
static WE_BOOL contStarted = FALSE;
static we_content_data_t* contentQueue[MMS_NOTIF_QUEUE_SIZE];



static MSignalFunction* mmsContSignalHandlers[MMS_CONT_STATE_MAX];

 
static void mmsContMain(MmsSignal* sig);
static void mmsContIdleStateHandler(const MmsSignal* sig);
static void mmsContReceiveStateHandler(const MmsSignal* sig);
static WE_BOOL mmsContCreateOperation(void);
static void mmsContCleanupOperation(void);
static void mmsContDeallocContentData(we_content_data_t* p);
static WE_BOOL mmsContOpenPipe(void);
static WE_BOOL mmsContReadFromPipe(void);
static void mmsContHandleNotif(WE_INT16 eventType, WE_INT32 handle);
static void mmsContChangeState(MmsContState newState);


#ifdef WE_LOG_MODULE 



const char *fsmCONTSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_CONT_START:
        return "MMS_SIG_CONT_START";
    case MMS_SIG_CONT_NOTIF:
        return "MMS_SIG_CONT_NOTIF";
    default:
        return 0;
    }
} 
#endif








void contQueue(we_content_data_t* pOurContent)
{
    if (!contStarted)
    {
        if (contentQueueState >= MMS_NOTIF_QUEUE_SIZE)
        {
            
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM CONT: Failed to queue content notification\n"));

            mmsContDeallocContentData(pOurContent);
            return ;
        }

        contentQueue[contentQueueState] = pOurContent;
        contentQueueState++;
    }
    else
    {
        M_SIGNAL_SENDTO_P(M_FSM_CONT, (int)MMS_SIG_CONT_START, pOurContent);        
    }
} 








void contDeQueue(void)
{
    contStarted = TRUE;
    while (contentQueueState > 0)
    {
        contentQueueState--;
        M_SIGNAL_SENDTO_P(M_FSM_CONT, (int)MMS_SIG_CONT_START, contentQueue[contentQueueState]);
        contentQueue[contentQueueState] = 0;
    }
} 







static void contFreeQueue()
{
    contStarted = FALSE;
    while (contentQueueState > 0)
    {
        contentQueueState--;
        mmsContDeallocContentData(contentQueue[contentQueueState]);
        contentQueue[contentQueueState] = 0;
    }
} 









void contEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_CONT);
    contStarted = FALSE;
    contentQueueState = 0;
    
    if (contentOper != NULL && contentOper->hPipe != -1)
    {
        (void)WE_PIPE_CLOSE(contentOper->hPipe);
        (void)WE_PIPE_DELETE(contentOper->data->contentSource);
        contentOper->hPipe = -1;
    } 

} 





void contInit(void)
{
    if (contentOper)
    {
        contTerminate();
    } 
    contentOper = NULL;
    mmsContSignalHandlers[MMS_CONT_STATE_IDLE] = (MSignalFunction*) mmsContIdleStateHandler;
    mmsContSignalHandlers[MMS_CONT_STATE_RECEIVE] = (MSignalFunction*) mmsContReceiveStateHandler;

    mSignalRegisterDst(M_FSM_CONT, mmsContMain);
    
    contDeQueue();
} 




void contTerminate(void)
{
    if (contentOper)
    {
        M_FREE(contentOper);
        contentOper = NULL;
    } 
    contFreeQueue();

    mSignalDeregister(M_FSM_CONT);
} 






static void mmsContMain(MmsSignal *sig)
{
    MmsContState state;

    if (sig->type == MMS_SIG_CONT_START && !mmsContCreateOperation() )
    {
        mmsContDeallocContentData((we_content_data_t*)sig->p_param);
        sig->p_param = NULL;
    }
    else if (contentOper != NULL &&
        contentOper->state >= MMS_CONT_STATE_IDLE &&
        contentOper->state < MMS_CONT_STATE_MAX)
    {
        state = contentOper->state;
        mmsContSignalHandlers[state](sig);
    } 

    
    mSignalDelete (sig);
} 






static void mmsContIdleStateHandler(const MmsSignal* sig)
{
    int bearer;

    switch (sig->type)
    {
    case MMS_SIG_CONT_START:
        contentOper->data = (we_content_data_t*)sig->p_param;
        if (NULL == contentOper->data)
        {
            mmsContCleanupOperation();
            return;
        } 
        
        if (contentOper->data->contentDataType == WE_CONTENT_DATA_RAW)
        {
            bearer = WE_NETWORK_ACCOUNT_GET_BEARER(
                contentOper->data->networkAccountId);
            recPush( bearer == WE_SOCKET_BEARER_GSM_SMS,
                contentOper->data->contentDataLength, 
                contentOper->data->contentData);
            mmsContCleanupOperation();
        }
        else if ( !mmsContOpenPipe())
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM CONT: Failed to open pipe\n"));
            mmsContCleanupOperation();
        }
        else if ( !mmsContReadFromPipe())
        {
            mmsContCleanupOperation();
        }
        else
        {
            mmsContChangeState(MMS_CONT_STATE_RECEIVE);
        } 
        break;
        
    default:
        break;
    } 
    
} 






static void mmsContReceiveStateHandler(const MmsSignal* sig)
{
    switch(sig->type)
    {
    case MMS_SIG_CONT_NOTIF:
        mmsContHandleNotif((WE_INT16)sig->i_param, (WE_INT32)sig->u_param1);
        break;
       
    default:
        break;
    } 
} 





static WE_BOOL mmsContCreateOperation(void)
{
    if (contentOper)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM CONT: Busy\n"));
        return FALSE;
    } 
    
    if (NULL == (contentOper = M_ALLOCTYPE(MmsContOperation)))
    {
        return FALSE;
    } 
    
    contentOper->state         = MMS_CONT_STATE_IDLE;
    contentOper->data  = NULL;
    contentOper->hPipe         = -1;
    contentOper->pos           = 0;
    
    return TRUE;
} 





static void mmsContCleanupOperation(void)
{
    if (contentOper->hPipe != -1)
    {
        (void)WE_PIPE_CLOSE(contentOper->hPipe);
        contentOper->hPipe = -1;
        (void)WE_PIPE_DELETE(contentOper->data->contentSource);
    } 

    mmsContDeallocContentData(contentOper->data);
    M_FREE(contentOper);
    contentOper = NULL;
} 

 





static void mmsContDeallocContentData(we_content_data_t* p)
{
    if (!p)
    {
        return;
    } 
    
    if (p->routingIdentifier)
    {
        M_FREE(p->routingIdentifier);
    } 
    
    if (p->contentUrl)
    {
        M_FREE(p->contentUrl);
    } 
    
    if (p->contentSource)
    {
        M_FREE(p->contentSource);
    } 
    
    if (p->contentParameters)
    {
        M_FREE(p->contentParameters);
    } 
    
    if (p->contentData)
    {
        M_FREE(p->contentData);
    } 
    
    M_FREE(p);
} 






static WE_BOOL mmsContOpenPipe(void)
{
    contentOper->hPipe = WE_PIPE_OPEN(WE_MODID_MMS, 
        contentOper->data->contentSource);
    if (contentOper->hPipe < 0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Pipe open error %d\n", __FILE__, __LINE__, 
            contentOper->hPipe));
        return FALSE;
    } 
    
    return TRUE;
} 





static WE_BOOL mmsContReadFromPipe(void)
{
    int   bytesRead;
    int   isOpen;
    long  available;
    int   status;
    
    do
    {
        if (contentOper->pos == MMS_MAX_NOTIFICATION_SIZE)
        {
        

            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                "MMS FSM CONT: Failed to get status from pipe\n"));
            mmsNotifyError(MMS_RESULT_MAX_NOTIFICATION_SIZE_EXCEEDED);
            return FALSE; 
        } 
        
        bytesRead = WE_PIPE_READ(contentOper->hPipe, 
            &contentOper->pdu[contentOper->pos], 
            MMS_MAX_NOTIFICATION_SIZE - contentOper->pos);
        
        if (bytesRead > 0)
        {
            
            contentOper->pos += bytesRead;
        }
        else if (bytesRead == WE_PIPE_ERROR_DELAYED)
        {
            
            status = WE_PIPE_STATUS(contentOper->hPipe, &isOpen, &available);
            if (status == 0)
            {
                

                (void)WE_PIPE_POLL(contentOper->hPipe);
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
    
    if (bytesRead == WE_PIPE_ERROR_CLOSED)
    {
        
    

        
        recPush(FALSE , (unsigned long)contentOper->pos,
            contentOper->pdu);
        return FALSE; 
    }
    else if (bytesRead < 0 && bytesRead != WE_PIPE_ERROR_DELAYED)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Pipe read error %d\n", __FILE__, __LINE__, bytesRead));
        return FALSE;
    } 
    
    return TRUE;
} 







static void mmsContHandleNotif(WE_INT16 eventType, WE_INT32 handle)
{
    int bearer;

    if (contentOper == NULL || contentOper->hPipe != handle)
    {
        
        return;
    } 
    
    switch (eventType)
    {
    case WE_PIPE_EVENT_READ:
        if (!mmsContReadFromPipe())
        {
            mmsContCleanupOperation();
        } 
        break;
    case WE_PIPE_EVENT_CLOSED:
        
        bearer = WE_NETWORK_ACCOUNT_GET_BEARER(
            contentOper->data->networkAccountId);
        recPush( bearer == WE_SOCKET_BEARER_GSM_SMS,
            (unsigned int)contentOper->pos, contentOper->pdu);
        mmsContCleanupOperation();
        break;
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Unexpected pipe notification %d\n", 
            __FILE__, __LINE__, eventType));
        break;
    } 
    
} 






static void mmsContChangeState(MmsContState newState)
{
    contentOper->state = newState;
} 


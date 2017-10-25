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
 
/*! \file mms_env.c
 * \brief  Exported functions from the MMS Client.
 */

/*--- Include files ---*/
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"    /* WE: Module definition */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Core.h"   /* WE: System core call */ 
#include "We_Dcvt.h"   /* WE: Type conversion routines */ 
#include "We_Mem.h"    /* WE: Memory handling */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Cmmn.h"   /* WE: WE common functions, e.g. string handling */ 
#include "We_Pipe.h"   /* WE: Pipe handling */ 
#include "We_File.h"   /* WE: File handling */ 


#include "Stk_If.h"     /* STK: WSP/HTTP Stack signals and types */

#ifdef WE_MODID_DRS
#include "Drs_If.h"     /* DRS: DRM Interface definitions */
#endif

#include "Mms_Cfg.h"    /* MMS: Configuration definitions */
#include "Mms_Def.h"    /* MMS: Exported types in adapter/connector functions */
#include "Mms_If.h"     /* MMS: Exported signal interface */
#include "Mms_Env.h"    /* MMS: Exported WE environment functions */


#include "Mmsrec.h"     /* MMS: Receive function forwards sig to internal sig */
#include "Mmsrpl.h"     /* MMS: Reply signal functions */

#include "Msig.h"       /* MMS: Signal handling functions */
#include "Mtimer.h"     /* MMS: Timer functions */
#include "Mmem.h"       /* MMS: Memory handling functions */
#include "Mcpdu.h"      /* MMS: Definitions and prototypes for PDU handling */
#include "Mcwap.h"      /* MMS: Utility functions and types from WAP Handler */
#include "Mmain.h"      /* MMS: FSM for initialize and stop */
#include "Masync.h"     /* MMS: Async file handling functions */
#include "Mcpost.h"     /* MMS: FSM COH POST */
#include "Mcget.h"      /* MMS: FSM COH GET */
#include "Mms_Cont.h"   /* MMS: FSM CONT */
#include "Mms_Vrsn.h"   /* MMS: Version file */
#include "Fldmgr.h"     /* MMS: Folder functions */
#include "Msgread.h"    /* MMS: Message read access functions */
#include "Mhandler.h"   /* MMS: Message creation functions */

#ifdef MMS_UBS_IN_USE
#include "Mmsui.h"
#endif /* MMS_UBS_IN_USE */
/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/
#define MMS_MAX_REGISTERED_APPL 8
static WE_UINT8 g_aRegisteredApps[MMS_MAX_REGISTERED_APPL];
static WE_UINT8 g_numRegisteredApps = 0;

/*--- Static variables ---*/
static WE_BOOL     isActiveSent;   /* IS_ACTIVE should only be sent once */

/* The following two variables are used to handle out-of-memory situations. The
 * naming of variables and functions for these are somewhat different due to 
 * WE. The variable mms_inside_run is used to indicate if the call comes 
 * via the MMSc_run function so that it is safe to longjmp back to MMSc_run in
 * case of an out-of-memory situation. It is actually a BOOL but in order to
 * be exactly the same as other modules, it has type int and will use 0 and 1
 * to indicate TRUE and FALSE. */
static jmp_buf      mms_jmpbuf;     /* Buffer for longjmp */
static int          mms_inside_run; /* If call originates from MMSc_run or not */

/*--- Prototypes ---*/
static void handleHttpStkRequestAborted( stk_http_request_aborted_t *reply);
static void handleWeModuleStatus( const we_module_status_t *data);
static void handleStkHttpReply( stk_http_reply_t *reply);
static void mms_exception_handler(void);
static void mmsReceiveExtSignal(WE_UINT8 srcModule, WE_UINT16 signal,
    void *sigdata);

/*****************************************************************************/

/*
 *	\brief Registers applications for reception of spontanous notification signals from MMS service.
 *
 * \param module to register.
 *****************************************************************************/
static void mmsAppRegister(WE_UINT8 srcmodule)
{
    if (g_numRegisteredApps < MMS_MAX_REGISTERED_APPL )
    {
        g_aRegisteredApps[g_numRegisteredApps] = srcmodule;
        g_numRegisteredApps++;
    }
} /* handleUIRegister */

/*
 *	\brief Unregisters applications for reception of spontanous notification signals from MMS service.
 *
 * \param module to unregister.
 *****************************************************************************/
static void mmsAppUnregister(WE_UINT8 srcmodule)
{
    int       i;
    int       j;
    
    for(i=0; i<g_numRegisteredApps; i++)
    {
        if (g_aRegisteredApps[i] == srcmodule)
        {
            for(j=i+1; j<g_numRegisteredApps; j++)
            {
                g_aRegisteredApps[j-1] = g_aRegisteredApps[j];
            }
            --g_numRegisteredApps;
            
            return ;
        }
    }
} /* mmsAppUnregister */


/*!
 * \brief Sends subscription signal to all registered applications.
 *
 * \returns the module identification 
 *
 * ONLY FOR MMS SERVICE INTERNAL USAGE.
 *****************************************************************************/
void mmsSendApplSubscription(WE_UINT8 senderModId,
    WE_UINT16 signal, void *data, MmsIfConvertFunction *cvtFunPtr)
{
    int i;
    
    for(i=0; i<g_numRegisteredApps; i++)
    {
        if ( !mmsSendSignalExt( senderModId, g_aRegisteredApps[i], signal, data, cvtFunPtr))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): signal send failed.\n", __FILE__, __LINE__));           
        } /* if */
    }

}

/*****************************************************************************/

/*!
 * \brief Handle an incoming signal of type WE_SIG_MODULE_STATUS
 * 
 *****************************************************************************/
static void handleWeModuleStatus( const we_module_status_t *data)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
        "MMS received handleWeModuleStatus: Module=%d, Status=%d\n", 
        data->modId, data->status));

    if (data == NULL)
    {
        return ;
    }
    
    if (data->status == MODULE_STATUS_ACTIVE)
    {
        mmsCoreModuleStarted(data->modId);
    }
    else if (data->status == MODULE_STATUS_TERMINATED)
    {
        mmsCoreModuleStopped(data->modId);
    }
    else if (data->modId == WE_MODID_STK)
    {
        /* Since we don't know what the original operation was about we
         * can't respond with the correct response signal. We have to 
         * answer with something similar. */
        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_STOP_RESPONSE,
                            0, 0, MMS_RESULT_COMM_FAILED);
    } /* if */
} /* handleWeModuleStatus */


/*!
 * \brief Handle an incoming signal of type WE_SIG_MODULE_STATUS
 *
 * All pointer data needed later are copied to be able to remove signal now.
 * 
 * \param reply Reply data as supplied by the STK
 *****************************************************************************/
static void handleStkHttpReply( stk_http_reply_t *reply)
{
    char *ptr;

#ifdef _DEBUG
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "%s(%d) Received STK_SIG_HTTP_REPLY, reqId=%d, status=%d (0x%x)\n", 
        __FILE__, __LINE__, reply->requestId, reply->status, reply->status));
#endif /* _DEBUG */

    if (reply == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Illegal STK response, replyPtr=%d\n", 
            __FILE__, __LINE__, reply));
    }
    else if (reply->numEntries == 0)
    { 
        /* No content in answer, only e.g. the header "HTTP 200 OK".
         * This is OK, return it to MMS Service. 
         * Indicate it by returning an empty buffer. */
        mmsWapHttpResponse( reply->requestId, reply->status, 
            STK_BODY_BUFFER, 0, NULL);
    }
    else if (reply->content == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Illegal STK reply->content=%d\n", 
            __FILE__, __LINE__, reply == NULL ? -1 : (int)reply->content));
    }
    else
    {
        switch (reply->content->dataType)
        {
        case STK_BODY_BUFFER :
            ptr = M_ALLOC((WE_UINT32)reply->content->dataLen);
            memcpy( ptr, reply->content->_u.data, (WE_UINT32) reply->content->dataLen);
            mmsWapHttpResponse( reply->requestId, reply->status, 
                reply->content->dataType, reply->content->dataLen,
                ptr);
            break;
        case STK_BODY_FILE :
            mmsWapHttpResponse( reply->requestId, reply->status, 
                reply->content->dataType, reply->content->dataLen,
                we_cmmn_strdup( WE_MODID_MMS, reply->content->_u.pathName));
            break;
        case STK_BODY_PIPE :
            mmsWapHttpResponse( reply->requestId, reply->status, 
                reply->content->dataType, reply->content->dataLen,
                we_cmmn_strdup( WE_MODID_MMS, reply->content->_u.pipeName));
            break;
        default :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): response of wrong type %d\n", __FILE__, __LINE__,
                reply->content->dataType));
            break;
        } /* switch */
    } /* if */
} /* handleStkHttpReply */

/*!
 * \brief Handle an incoming signal of type STK_SIG_HTTP_REQUEST_ABORTED
 * 
 * \param reply Reply data as supplied by the STK
 *****************************************************************************/
static void handleHttpStkRequestAborted( stk_http_request_aborted_t *reply)
{
    if (reply == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Illegal indata! Data=%d\n", __FILE__, __LINE__, reply));
        return;
    }
    else if (reply->errorCode == STK_ERR_INVALID_CHANNEL_ID ||
        reply->errorCode == STK_ERR_COMM_CONNECTION_CLOSED)
    {
        mmsWapChannelIsDisconnected();
    } /* if */

    mmsWapHttpResponse( reply->requestId, reply->errorCode, 0, 0, NULL);
} /* handleHttpStkRequestAborted */

/*
 * Cleanup when finished
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSc_kill(void)
{
    WE_PIPE_CLOSE_ALL(WE_MODID_MMS);
    WE_FILE_CLOSE_ALL(WE_MODID_MMS);
    WE_MEM_FREE_ALL(WE_MODID_MMS);
    WE_KILL_TASK(WE_MODID_MMS);
} /* MMSc_kill */

/*
 * Perform an execution 'tick' for the MMS client.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSc_run(void)
{
    mms_inside_run = 1;

    if (setjmp( mms_jmpbuf ) == 0)
    {
        if (WE_SIGNAL_QUEUE_LENGTH(WE_MODID_MMS) > 0)
        {
            /* Well, We collect all external signals to avoid buffer explosion.
             * This on the other hand may cause an ever groving internal queue.
             * But, with no directives available, this is the way we do it.
             */
            while (WE_SIGNAL_RETRIEVE(WE_MODID_MMS, mmsReceiveExtSignal))
            {
                /* Empty while, all is done in the condition */
            };
        } /* if */

        mSignalProcess();   /* Process one signal and return. */
    }
    else
    {
       /*
        * A longjmp from mmsMemAlloc. If we are here there is no more
        * memory, and the situation could not be worse.
        * Terminate the MMS Client and let the TDA re-initialise it again.
        */
        coreMainEmergencyAbort();
    } /* if */

    mms_inside_run = 0;
} /* MMSc_run */

/*
 * Prepares the MMS client for running.
 *
 *****************************************************************************/
void MMSc_start(void)
{
    mms_inside_run = 0;
    isActiveSent = FALSE;
    
    memset( mms_jmpbuf, 0, sizeof(mms_jmpbuf));

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, "MMS_start(void)\n"));
    
    /* Initialize the memory allocator. Needed in most other functions. */
    WE_MEM_INIT(WE_MODID_MMS, mms_exception_handler, MMS_MEM_INIT_EXT);

    /* Register the signal queue */ 
    WE_SIGNAL_REGISTER_QUEUE(WE_MODID_MMS);

    /* Register conversion and destruction functions */
    WE_SIGNAL_REG_FUNCTIONS( WE_MODID_MMS, mmsConvert, mmsDestruct);

    /* Initiate utility functions */
    mSignalInit();
    coreMainInit();
    createFolder();

    M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, 
        MMS_MAIN_FIRST_START_ELEMENT);

    /* Registrate the module into WE */
    WE_MODULE_IS_CREATED(WE_MODID_MMS, (char *) MMS_VERSION);

} /* MMSc_start */

/*
 * Clean up before stopping the MMS client.
 *
 *****************************************************************************/
void MMSc_terminate(void)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, "MMS_terminate(void)\n"));
    
    M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, 
        MMS_MAIN_FIRST_TERMINATE_ELEMENT);    
} /* MMSc_terminate */

/*
 * Check if MMS wants to run. 
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int MMSc_wantsToRun(void)
{
    return !mSignalQueueIsEmpty() || WE_SIGNAL_QUEUE_LENGTH(WE_MODID_MMS) > 0;
} /* MMSc_wantsToRun */

/*!
 * \brief Used to handle memory exceptions and to emergency stop the MMS Service
 *
 * Odd function name to be consistent with external requirements.
 *
 *****************************************************************************/
static void mms_exception_handler(void)
{
    if (mms_inside_run)
    {
        longjmp( mms_jmpbuf, 1);
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Insufficient memory!\n", __FILE__, __LINE__));

        mmsNotifyError(MMS_RESULT_RESTART_NEEDED);
    } /* if */
} /* mms_exception_handler */


/*
 * Handles the incoming external signals
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
static void mmsReceiveExtSignal(WE_UINT8 module, WE_UINT16 signal, 
    void *sigData)
{
#ifdef MMS_UBS_IN_USE
    if (TRUE == mmsHandleExtUBSignal(
        module,      /* Sending module */
        signal,
        sigData))
    {
        /*
         *	Signal consumed by UI
         */
        WE_SIGNAL_DESTRUCT( WE_MODID_MMS, signal, sigData);
        return ;
    }
#endif /* MMS_UBS_IN_USE */

    /* The following signals are handled */
    switch (signal)
    {
    /* 
     * Received signals from the DRS module
     */ 

#ifdef WE_MODID_DRS
    /*
     *	Reply to DRSif_storeObject()
     */
    case DRS_SIG_STORE_FINISHED :
        mmsMMHhandleDrmStoreFinished(sigData);
        break;

    /*
     * Reply to DRSif_getDcfProperties()
     */
    case DRS_SIG_DCF_PROPERTIES:
        (void)mmsMRhandleDcfProperties(&mmsMRFSM, sigData);
        break;

    /*
     * Reply to DRSif_openFile()
     */
    case DRS_SIG_OPEN_FILE_REPLY:
        (void)mmsMRhandleOpenFileReply(&mmsMRFSM, sigData);
        break;

    /*
     * Reply to DRSif_readFile()
     */
    case DRS_SIG_READ_FILE_REPLY:
        (void)mmsMRhandleReadFileReply(&mmsMRFSM, sigData);
        break;

#endif

    /* 
     * Received signals from the STK
     */ 
    case STK_SIG_HTTP_REPLY :
        handleStkHttpReply(sigData);
        break;

    case STK_SIG_HTTP_STATUS :
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
            "%s(%d): Received STK_SIG_HTTP_STATUS. Not used by MMS.\n", 
            __FILE__, __LINE__));
        break;

    case STK_SIG_HTTP_REQUEST_ABORTED :
#ifdef _DEBUG
        {
            stk_http_request_aborted_t *reply = sigData;
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
                "%s(%d): Received STK_SIG_HTTP_REQUEST_ABORTED, "
                "reqId=%d, error=%d (0x%x).\n", __FILE__, __LINE__, 
                reply->requestId, reply->errorCode, reply->errorCode));
        }
#endif /* _DEBUG */

        handleHttpStkRequestAborted(sigData);
        break;

    /* 
     * Received signals from the WE   
     */ 
    case WE_SIG_MODULE_EXECUTE_COMMAND:
        {
            we_module_execute_cmd_t* pExecCmdData = (we_module_execute_cmd_t*)sigData;

            if (pExecCmdData != NULL)
            {
                recContentRouting(pExecCmdData->contentData);
            }
            else
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
                    "%s(%d): Received WE_SIG_MODULE_EXECUTE_COMMAND "
                    "with illegal indata %d.\n", __FILE__, __LINE__, 
                    pExecCmdData));
            } /* if */

            if (!isActiveSent)
            {
                M_SIGNAL_SENDTO( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_EXECUTE_COMMAND);
                isActiveSent = TRUE;
            }
        }
        break;

    case WE_SIG_MODULE_STATUS:
        handleWeModuleStatus((we_module_status_t*)sigData);
        break;

    case WE_SIG_MODULE_TERMINATE:
        {
            MMSc_terminate();  
        } 
        break;
    case WE_SIG_TIMER_EXPIRED:
        {
            we_timer_expired_t *data;
            data = (we_timer_expired_t*)sigData; 
            mTimerExpired(data->timerID);
        } 
        break;
    case WE_SIG_FILE_NOTIFY:
        {
            we_file_notify_t *data;
            data = (we_file_notify_t*)sigData; 
            M_SIGNAL_SENDTO_IU( M_FSM_ASYNC, (int)MMS_SIG_ASYNC_FILE_NOTIFY, 
                data->eventType, (WE_UINT32) data->fileHandle);
        } 
        break;
    case WE_SIG_PIPE_NOTIFY:
        {
            we_pipe_notify_t* p = (we_pipe_notify_t*)sigData;
            if (asyncIsPipeOwner(p->handle))
            {
                M_SIGNAL_SENDTO_IU( M_FSM_ASYNC, (int)MMS_SIG_ASYNC_PIPE_NOTIFY, 
                        p->eventType, (WE_UINT32) p->handle);
            }
            if (cohPostIsPipeOwner(p->handle))
            {
                M_SIGNAL_SENDTO_IU( M_FSM_COH_POST, (int)MMS_SIG_COH_POST_PIPE_NOTIFY, 
                    p->eventType, (WE_UINT32) p->handle);
            }
            else if (cohGetIsPipeOwner(p->handle))
            {
                M_SIGNAL_SENDTO_IU( M_FSM_COH_GET, (int)MMS_SIG_COH_GET_PIPE_NOTIFY, 
                    p->eventType, (WE_UINT32) p->handle);
            }
            else
            {
                /* If the pipe is not managed by COH we assume that it is CONT */
                M_SIGNAL_SENDTO_IU(M_FSM_CONT, (int)MMS_SIG_CONT_NOTIF, 
                  p->eventType, (WE_UINT32) p->handle);
            }
        }
        break;

    case WE_SIG_REG_RESPONSE:
        recRegResponse((we_registry_response_t*)sigData);
        break;

    /* 
     * Interface signals      
     */ 

    case MMS_SIG_CREATE_MSG:       
        {
            st_MmsFileTypeSig  *pstData;
            pstData = (st_MmsFileTypeSig*)sigData;
            recCreateMessage(module, pstData->eFileType);
        }
        break;
    case MMS_SIG_DELETE_MSG:
        {
            MmsMsgSig  *data;
            data = (MmsMsgSig*)sigData;
            recDeleteMessage(module, data->msgId);
        }
        break;
    case MMS_SIG_GET_FLDR:
        {
            MmsMsgContentReq *data = (MmsMsgContentReq*)sigData;
            recGetMessageIdList(module, data->folderId, (WE_UINT32) data->from, 
                (WE_UINT32) (data->to - data->from + 1));
        }
        break;
    case MMS_SIG_GET_MSG_INFO:
        {
            MmsMsgSig *data = (MmsMsgSig*)sigData;
            recGetMessageInfo(module, data->msgId);
        }
        break;        
    case MMS_SIG_FORWARD_REQ:        
        {
            MmsPduSig *data = (MmsPduSig*)sigData;
            recForwardReq(module, data->pdu, data->length);
        }
        break;
       
    case MMS_SIG_GET_NUM_OF_MSG:  
        {
            MmsFolderSig *data; 
            data = (MmsFolderSig*)sigData; 
            recGetNumberOfMessages(module, data->folderId);
        }    
        break;
    case MMS_SIG_MSG_DONE: 
        {
            MmsMsgFolderSig  *data;
            data = (MmsMsgFolderSig*)sigData;
            recCreateMessageDone(module, data->msgId, data->folder);
        }
        break;
    case MMS_SIG_MOVE_MSG:           
        {
            MmsMsgFolderSig *data;
            data = (MmsMsgFolderSig*)sigData; 
            recMoveMessage(module, data->msgId, data->folder);
        }
        break;
    case MMS_SIG_RETRIEVE_MSG:
        {
            MmsMsgSig  *data;
            data = (MmsMsgSig*)sigData;
            recRetrieveMessage(module, data->msgId);
        }      
        break;
    case MMS_SIG_RETRIEVE_MSG_CANCEL:
        {
            MmsMsgSig  *data;
            data = (MmsMsgSig*)sigData;
            recRetrieveCancel(data->msgId);
        }  
        break;
    case MMS_SIG_SEND_MSG:  
        {
            recSendMessage(module, ((MmsMsgSig*) sigData)->msgId);
        }
        break;
    case MMS_SIG_SEND_MSG_CANCEL:  
        {
            recSendCancel(((MmsMsgSig*) sigData)->msgId);
        }
        break;
    case MMS_SIG_READ_REPORT: 
        {
            MmsSendReadReport *data; 
            data = (MmsSendReadReport*)sigData; 
            recSendReadReport(module, data->msgId, data->readStatus);
        }
        break;
    case MMS_SIG_SET_READ_MARK: 
        {
            MmsSetReadMark *data; 
            data = (MmsSetReadMark*)sigData; 
            recSetReadMark(data->msgId, data->value); 
        }
        break; 
    case  MMS_SIG_GET_REPORT: 
        {
            MmsMsgSig *data; 
            data = (MmsMsgSig*)sigData; 
            recGetReport(data->msgId);
        }
        break;
    case MMS_SIG_FOLDER_STATUS: 
        {
            MmsFolderSig *data; 
            data = (MmsFolderSig*)sigData; 
            getFolderStatus(data->source, data->folderId);
        }
        break;
    case MMS_SIG_GET_MSG_HEADER: 
        {
            MmsGetMsgRequest *data; 
            data = (MmsGetMsgRequest*)sigData; 
            mmsMRgetMsgHeader(&mmsMRFSM, module, data->msgId, data->userData);
        }
        break;
    case MMS_SIG_GET_MSG_SKELETON:
        {
            MmsGetMsgRequest *data; 
            data = (MmsGetMsgRequest*)sigData; 
            mmsMRreadGetMsgSkeleton(&mmsMRFSM, module, data->msgId, data->userData);
        }
        break;
    case MMS_SIG_GET_BODY_PART: 
        {
            MmsGetBodyPartRequest *data; 
            data = (MmsGetBodyPartRequest*)sigData; 
            mmsMRgetMsgBodyPart(&mmsMRFSM, module, data->msgId, data->bodyPartId, data->userData);
        }
        break;
    case MMS_SIG_APP_REG :
        mmsAppRegister(module);
        break;
    case MMS_SIG_APP_UNREG :
        mmsAppUnregister(module);
        break;
    default:
        /* Received unsupported signal! */
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Received unhandled signal %d from module %d\n", 
            __FILE__, __LINE__, signal, module));
        break; 
    }  /* switch */

    WE_SIGNAL_DESTRUCT( WE_MODID_MMS, signal, sigData);
} /* mmsReceiveExtSignal */

/*
 * \brief Returns the number of registered applications.
 *
 * For MMS internal usage.
 *
 * \return g_numRegisteredApps.
 *****************************************************************************/
WE_UINT8 mmsNoOfRegisteredApps(void)
{
    return g_numRegisteredApps;
}

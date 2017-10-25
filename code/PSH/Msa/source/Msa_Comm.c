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

/* !\file macomm.c
 * Network communication handling.
 */

/* WE */
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Int.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Cmmn.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_Def.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Intsig.h"
#include "Msa_Rc.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Uipm.h"
#include "Msa_Comm.h"
#include "Msa_Conf.h"
#include "Msa_Del.h"
#include "Msa_Utils.h"
#include "Msa_Uicmn.h"
#include "Msa_Uidia.h"
#include "Msa_Core.h"
#include "Msa_Srh.h"
#include "Msa_Cfg.h"
#include "Msa_Uicomm.h"
#include "Msa_Notif.h"

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*! \struct MsaSendInstance
 * Message sending data */
typedef struct
{
    MmsMsgId        msgId;        /* The WID of the message to send */
    MsaStateMachine resultFsm;    /* The FSM to send the response to */
    int             resultSignal; /* The signals that is sent to the 
                                     resultFsm */
    WE_BOOL        showWarning;  /* Whether to show warning if size breaks
                                     conformance */
    WE_UINT32      size;         /* Message size */
}MsaSendInstance;

/*! \struct MsaDownloadInstance
 *  Message retrieval */
typedef struct
{
    MmsMsgId        msgId;        /* The WID of the notification */
    MmsMsgType      msgType;      /* The WID of the notification */
    MsaStateMachine resultFsm;    /* The FSM to send the response to */
    int             resultSignal; /* The signals that is sent to the 
                                     resultFsm */
    MsaGetNotifInstance *getNotifInstance;  /* notification instance */
}MsaDownloadInstance;

/******************************************************************************
 * Variables
 *****************************************************************************/

static MsaSendInstance *s_Msa_pstSendInstance;
static MsaDownloadInstance *s_Msa_pstDownloadInstance;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static unsigned int Msa_CalculateProgress(unsigned int uiProgress, 
    unsigned int uiTotalSize);
static void Msa_CleanupSendInstance(MmsResult eResult, WE_BOOL bSilent);
static WE_BOOL Msa_CreateSendInstance(MsaStateMachine eFsm, unsigned int uiSignal, 
    MmsMsgId uiMsgId, WE_BOOL bShowWarning);
static void Msa_CleanupDownloadInstance(MmsResult eResult, WE_BOOL bSilent);
static WE_BOOL Msa_CreateDownloadInstance(MsaStateMachine eFsm, 
    unsigned int uiSignal, MmsMsgId uiMsgId);
static void Msa_CommSignalHandler(MsaSignal *pstSig);
static void Msa_HandleProgress(const MmsProgressStatus *pstStatus);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*! Calculates the current progress in per cent
 *
 * \param progress The current progress in bytes
 * \param totalSize The current size in bytes
 * \return The current progress in per cent 
 *****************************************************************************/
static unsigned int Msa_CalculateProgress
(
    unsigned int uiProgress, 
    unsigned int uiTotalSize
)
{
    unsigned int uiPerCent;
    /* Calculate the current progress */
    uiPerCent = 0;
    if (uiTotalSize > 0)
    {
        uiPerCent = (uiProgress * 100) / uiTotalSize;
    }
    if (uiPerCent > 100)
    {
        return 100;
    }
    return uiPerCent;
}

/*!
 * \brief Handles the download progress.
 *
 * \param pStatus The current progress from the MMS Service.
 *****************************************************************************/
static void Msa_HandleProgress(const MmsProgressStatus *pstStatus)
{
    char *pcInitialStr = NULL;
    char *pcEndStr = NULL;
    char *pcOutStr = NULL;
    char acBuffer[sizeof(char)*10 + sizeof('/') + sizeof(char)*10 + sizeof('\0')];

    if (NULL != pstStatus)
    {
        switch (pstStatus->operation)
        {
        case MMS_PROGRESS_STATUS_SEND:
            if ((NULL != s_Msa_pstSendInstance) && 
                (s_Msa_pstSendInstance->msgId == pstStatus->msgId))
            {
                /* Create progress text */
                memset(acBuffer, 0, sizeof(char)*10 + sizeof('/') + 
                    sizeof(char)*10 + sizeof('\0'));
                pcInitialStr = msaGetStringFromHandle(
                    MSA_GET_STR_ID(MSA_STR_ID_SEND_STATUS));
                if (MMS_PROGRESS_STATUS_START == pstStatus->state)
                {
                    pcEndStr = msaGetStringFromHandle(
                        MSA_GET_STR_ID(MSA_STR_ID_CONNECTING));
                }
                else
                {
                    sprintf(acBuffer, "%lu/%lu", pstStatus->progress, 
						pstStatus->totalLength);
                    pcEndStr = msaGetStringFromHandle(
                        MSA_GET_STR_ID(MSA_STR_ID_BYTES_SENT));
                }
                if ((NULL == pcInitialStr) || (NULL == pcEndStr))
                {
                    pcOutStr = NULL;
                }
                else
                {    
                    pcOutStr = we_cmmn_str3cat(WE_MODID_MSA, pcInitialStr, 
                        acBuffer, pcEndStr);
                }
                MSA_FREE(pcInitialStr);
                MSA_FREE(pcEndStr);
                msaPmSetProgress(Msa_CalculateProgress(pstStatus->progress, 
                    pstStatus->totalLength), pcOutStr);
                MSA_FREE(pcOutStr);
            }
            break;
        case MMS_PROGRESS_STATUS_RETRIEVE:
            if (NULL != s_Msa_pstDownloadInstance)
            {
                /* Create progress text */
                memset(acBuffer, 0, sizeof(char)*10 + sizeof('/') + 
                    sizeof(char)*10 + sizeof('\0'));
                pcInitialStr = msaGetStringFromHandle(
                    MSA_GET_STR_ID(MSA_STR_ID_DOWNLOAD_STATUS));
                if (MMS_PROGRESS_STATUS_START == pstStatus->state)
                {
                    pcEndStr = msaGetStringFromHandle(
                        MSA_GET_STR_ID(MSA_STR_ID_CONNECTING));
                }
                else
                {
                    sprintf(acBuffer, "%lu/%lu", pstStatus->progress, 
                        pstStatus->totalLength);
                    pcEndStr = msaGetStringFromHandle(
                        MSA_GET_STR_ID(MSA_STR_ID_BYTES_RECEIVED));
                }
                if ((NULL == pcInitialStr) || (NULL == pcEndStr))
                {
                    pcOutStr = NULL;
                }
                else
                {    
                    pcOutStr = we_cmmn_str3cat(WE_MODID_MSA, pcInitialStr, 
                        acBuffer, pcEndStr);
                }
                MSA_FREE(pcInitialStr);
                MSA_FREE(pcEndStr);                
                msaPmSetProgress(Msa_CalculateProgress(pstStatus->progress, 
                    pstStatus->totalLength), pcOutStr);
                MSA_FREE(pcOutStr);
            }
            break;
        case MMS_PROGRESS_STATUS_IMMRETRIEVE:
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) Immediate retrieval, progress=%x, "
                "totalLength=%x, state=%d\n", __FILE__, __LINE__, 
                pstStatus->progress, pstStatus->totalLength, pstStatus->state));
            break;
        default:
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) Erroneous operation type, operation=%d.\n",
                __FILE__, __LINE__, pstStatus->operation));
            break;
        }
    }
}

/*!
 * \brief Initiates the communication signal handler
 *****************************************************************************/
void Msa_CommInit(void)
{
    /* Register queue */
    s_Msa_pstSendInstance     = NULL;
    s_Msa_pstDownloadInstance = NULL;
    msaSignalRegisterDst(MSA_COMM_FSM, Msa_CommSignalHandler);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "COMM initialised\n"));
}

/*!
 * \brief Terminates the communication handler.
 *****************************************************************************/
void Msa_CommTerminate(void)
{
    /* Deregister queue */
    msaSignalDeregister(MSA_COMM_FSM);
    Msa_CleanupSendInstance(MMS_RESULT_OK, TRUE);
    Msa_CleanupDownloadInstance(MMS_RESULT_OK, TRUE);
}

/*!
 * \brief  Check network settings and show an error dialog upon any errors
 *
 * \param signal The signal that is sent when closing the error dialog 
 * \return TRUE if settings are OK, FALSE if they are not and dialog is present
 *****************************************************************************/
static WE_BOOL Msa_CheckSettings(int iSignal)
{
    if(strlen(msaGetConfig()->proxyHost ? msaGetConfig()->proxyHost : ""))
    {
        /* all checks OK */
        return TRUE;
    }

    if (!msaShowSettingsErrorDialog(MSA_COMM_FSM, iSignal))
    {
        (void)MSA_SIGNAL_SENDTO(MSA_COMM_FSM, iSignal);
    }
    return FALSE;
}

/*!
 * \brief  Activates FSM and sends a message.
 *
 * \param fsm    The fsm to post the result to.
 * \param retSig The signal that is sent as a result of the operation.
 * \param msgId  The message identity of the message to send.
 * \param showWarning Whether to show conformance warning
 *****************************************************************************/
void Msa_SendMessage
(
    MsaStateMachine eFsm,
    unsigned int uiRetSig,
    MmsMsgId uiMsgId,
    WE_BOOL bShowWarning
)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH,
                                WE_MODID_MSA,
                                "(%s) (%d) Send message initiated, uiMsgId=%d\n",
                                __FILE__,
                                __LINE__,
                                uiMsgId));

    if (!Msa_CreateSendInstance(eFsm, uiRetSig, uiMsgId, bShowWarning) || uiMsgId < 1) 
    {
        Msa_CleanupSendInstance(MMS_RESULT_ERROR, FALSE);
        return;
    } /* if */
    /* verify network settings, and fail on faulty settings */
    if (!Msa_CheckSettings(MSA_SIG_COMM_SETTINGS_FAILED_DIALOG_RSP))
    {
        return;
    }
    /* refresh roaming/bearer status */
    msaUpdateNetworkStatusInfo(MSA_COMM_FSM,  
        MSA_SIG_COMM_SEND_NETWORK_STATUS_RSP);                
}

/*!
 * \brief Creates a instance for sending a message.
 *
 * \param fsm    The fsm to post the result to.
 * \param signal The signal that is sent as a result of the operation.
 * \param msgId  The message identity of the message to send.
 * \param showWarning If a warning when breaking conformance should be shown
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL Msa_CreateSendInstance
(
    MsaStateMachine eFsm,
    unsigned int uiSignal, 
    MmsMsgId uiMsgId,
    WE_BOOL bShowWarning
)
{
    /* Check that a instance can be created */
    if (NULL != s_Msa_pstSendInstance) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) unable to create instance.\n", __FILE__, __LINE__));
        return FALSE;
    }
    else
    {
        /* Create instance */
        s_Msa_pstSendInstance = MSA_ALLOC(sizeof(MsaSendInstance));
        memset(s_Msa_pstSendInstance, 0, sizeof(MsaSendInstance));
        s_Msa_pstSendInstance->msgId        = uiMsgId;
        s_Msa_pstSendInstance->showWarning  = bShowWarning;
        s_Msa_pstSendInstance->resultFsm    = eFsm;
        s_Msa_pstSendInstance->resultSignal = (int)uiSignal;
        return TRUE;
    }
}

/*!
 * \brief Deallocates the current "send" instance and its resources.
 *
 * \param result The result to send back to the originating FSM.
 *****************************************************************************/
static void Msa_CleanupSendInstance
(
    MmsResult eResult,
    WE_BOOL bSilent
)
{
    if (NULL != s_Msa_pstSendInstance)
    {
        /* Remove progress meter */
        msaPmViewRemove();

        if (!bSilent)
        {
            /* Send response to the originating FSM, *if called for* */
            if (MSA_NOTUSED_FSM != s_Msa_pstSendInstance->resultFsm) 
            {
                (void)MSA_SIGNAL_SENDTO_UU(s_Msa_pstSendInstance->resultFsm, 
                                                                        s_Msa_pstSendInstance->resultSignal,
                                                                        eResult,
                                                                        s_Msa_pstSendInstance->msgId);
            } /* if */
            else
            {
                /* No caller to report to, show dialog */
                Msa_ShowSendDialog(eResult, MSA_NOTUSED_FSM, 0);
            }
        }
        /*
         *	Check if we should delete the message after sending.
         */
        if (!msaGetConfig()->saveOnSend && (MMS_RESULT_OK == eResult))
        {
            msaDeleteMsg(s_Msa_pstSendInstance->msgId, FALSE);
        }

        /* Note that the dialog created above is displayed for the 
           termination command, i.e., the application is going to terminate
           when the dialog is closed */
        if ((MSA_STARTUP_MODE_SEND == msaGetStartupMode()) || 
            (MSA_NOTUSED_FSM == s_Msa_pstSendInstance->resultFsm))
        {
            (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
        }
        MSA_FREE(s_Msa_pstSendInstance);
        s_Msa_pstSendInstance = NULL;
    }
}

/*!
 * \brief  Downloads a message.
 *
 * \param fsm    The fsm to post the result to.
 * \param retSig The signal that is sent as a result of the operation.
 * \param msgId  The message identity of the message to send.
 *****************************************************************************/
void Msa_DownloadMessage
(
    MsaStateMachine eFsm,
    unsigned int uiRretSig, 
    MmsMsgId uiMsgId
)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH,
                                WE_MODID_MSA, 
                                "(%s) (%d) Message download initiated, uiMsgId=%d\n",
                                __FILE__,
                                __LINE__, 
                                uiMsgId));
    (void)MSA_SIGNAL_SENDTO_IUU(MSA_COMM_FSM,
                                    MSA_SIG_COMM_DOWNLOAD, 
                                    (long)uiRretSig,
                                    (unsigned int)eFsm,
                                    uiMsgId);
}

/*!
 * \brief Creates a instance for message download.
 *
 * \param fsm    The fsm to post the result to.
 * \param signal The signal that is sent as a result of the operation.
 * \param msgId  The message identity of the message to download.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL Msa_CreateDownloadInstance
(
    MsaStateMachine eFsm, 
    unsigned int uiSignal,
    MmsMsgId uiMsgId
)
{
    /* Check that a instance can be created */
    if (NULL != s_Msa_pstDownloadInstance) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH,
                                    WE_MODID_MSA,
                                    "(%s) (%d) unable to create download instance.\n",
                                    __FILE__,
                                    __LINE__));
        return FALSE;
    }
    else
    {
        /* Create instance */
        s_Msa_pstDownloadInstance = MSA_ALLOC(sizeof(MsaDownloadInstance));
        memset(s_Msa_pstDownloadInstance, 0, sizeof(MsaDownloadInstance));
        s_Msa_pstDownloadInstance->msgId = uiMsgId;
        s_Msa_pstDownloadInstance->resultFsm = eFsm;
        s_Msa_pstDownloadInstance->resultSignal = (int)uiSignal;
        /* Display the progress meter */
        if (!msaPmViewShow(MSA_STR_ID_DOWNLOADING_MM, TRUE,
            TRUE, MSA_COMM_FSM, MSA_SIG_COMM_DOWNLOAD_CANCEL))
        {
            msaPmViewRemove();
            MSA_FREE(s_Msa_pstDownloadInstance);
            s_Msa_pstDownloadInstance = NULL;
            return FALSE;
        }
        return TRUE;
    }
}

/*!
 * \brief Deallocates the current download instance and its resources.
 *
 * \param result The result to send back to the originating FSM.
 *****************************************************************************/
static void Msa_CleanupDownloadInstance
(
    MmsResult eResult,
    WE_BOOL bSilent
)
{
    if (NULL != s_Msa_pstDownloadInstance)
    {
        if ((MSA_NOTUSED_FSM != s_Msa_pstDownloadInstance->resultFsm) && 
            !bSilent)
        {
            /* Send response to the originating FSM */
            (void)MSA_SIGNAL_SENDTO_UU(s_Msa_pstDownloadInstance->resultFsm, 
                s_Msa_pstDownloadInstance->resultSignal, eResult, 
                s_Msa_pstDownloadInstance->msgId);
        }
        else
        {
            /* Handle error dialogs if no other FSM is retrieving the result */
            if (MMS_RESULT_OK != eResult)
            {
                (void)Msa_ShowDownloadErrorDialog(eResult, 
                    s_Msa_pstDownloadInstance->resultFsm, 
                    s_Msa_pstDownloadInstance->resultSignal);
                
            }
        }
#ifndef WE_MODID_DRS
         if(MMS_RESULT_DRM_NOT_SUPPORTED == eResult)
         {
                MMSif_deleteMessage(WE_MODID_MSA,s_Msa_pstDownloadInstance->getNotifInstance->msgId);
         }
#endif
        if (s_Msa_pstDownloadInstance->getNotifInstance)
        {
            msaCleanupGetNotifInstance(&s_Msa_pstDownloadInstance->getNotifInstance);            
        }

        MSA_FREE(s_Msa_pstDownloadInstance);
        s_Msa_pstDownloadInstance = NULL;
        /* Remove progress meter */
        msaPmViewRemove();
        if (MSA_STARTUP_MODE_DOWNLOAD == msaGetStartupMode())
        {
            /* Terminate if started in download mode. Note that this FSM can
             * also be used in "view" mode.
             */
            (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
        }
    }
}

/*!
 * \brief Verify roaming status and possibly ask user what to do
 *
 * \param signal What signal to send to comm when done.
 *****************************************************************************/
static void Msa_VerifySendMode(int iSignal)
{
    /* show cost warning dialog when roaming and not immediate retrieval */
    if (msaGetConfig()->roamingStatus &&
        MSA_RETRIEVAL_MODE_ROAMING_RESTRICTED == 
        msaGetConfig()->retrievalModeRoam)
    {
        (void)msaShowRoamingSendDialog(MSA_COMM_FSM, iSignal, 
            s_Msa_pstSendInstance->size);  
    }
    else
    {
        (void)MSA_SIGNAL_SENDTO_UU(MSA_COMM_FSM,
            iSignal, MSA_CONFIRM_DIALOG_OK, 0);
    }      
}


/*!
 * \brief Verify retrieval mode/roaming status and possibly ask user what to do
 *
 * \param signal What signal to send to comm when done.
 *****************************************************************************/
static void Msa_VerifyRetrievalMode(int iSignal)
{
    /* show cost warning dialog when roaming and not immediate retrieval */
    if (msaGetConfig()->roamingStatus &&
        (MSA_RETRIEVAL_MODE_ROAMING_AUTOMATIC != msaGetConfig()->retrievalModeRoam ||
        MSA_RETRIEVAL_MODE_ALWAYS_ASK == msaGetConfig()->retrievalMode))
    {           
        (void)msaShowRoamingRetrievalDialog(MSA_COMM_FSM, iSignal, 
            s_Msa_pstDownloadInstance->getNotifInstance->notif->length);  
    }
    else if (!msaGetConfig()->roamingStatus &&
        MSA_RETRIEVAL_MODE_ALWAYS_ASK == msaGetConfig()->retrievalMode)
    {
        (void)msaShowHomeRetrievalDialog(MSA_COMM_FSM, iSignal, 
            s_Msa_pstDownloadInstance->getNotifInstance->notif->length);        
    }
    else
    {
        (void)MSA_SIGNAL_SENDTO_UU(MSA_COMM_FSM,
            iSignal, MSA_CONFIRM_DIALOG_OK, 0);
    }      
}

/*!
 * \brief Signal handler for the comm FSM.
 *
 * \param sig The received signal
 *****************************************************************************/
static void Msa_CommSignalHandler(MsaSignal *pstSig)
{
    MmsProgressStatus       *pstMsgStatus;
    int iRet;

    switch(pstSig->type)
    {
    /******************************************************************************
     *  Send signals	
     *****************************************************************************/
        case MSA_SIG_COMM_SEND_NETWORK_STATUS_RSP:
        {
            msaSrhGetMessageInfo(MSA_COMM_FSM, 
                                                    MSA_SIG_COMM_SEND_MESSAGE_INFO_RETRIEVED,
                                                    s_Msa_pstSendInstance->msgId);
            break;
        }

        case MSA_SIG_COMM_SEND_MESSAGE_INFO_RETRIEVED:
        {
            if (NULL  == pstSig->p_param)
            {
                Msa_CleanupSendInstance(MMS_RESULT_ERROR, FALSE);
                break;
            }
            s_Msa_pstSendInstance->size = ((MmsMessageFolderInfo *)pstSig->p_param)->size;
            if (s_Msa_pstSendInstance->size > MSA_CFG_MESSAGE_SIZE_WARNING) 
            {
                (void)msaShowConfirmDialog(MSA_STR_ID_MESSAGE_SIZE_WARNING,
                                                                NULL,
                                                                MSA_COMM_FSM,
                                                                MSA_SIG_COMM_SEND_MESSAGE_SIZE_OK,
                                                                ((MmsMessageFolderInfo *)pstSig->p_param)->msgId);
            }
            else
            {   /* fake dialog OK */
                MSA_SIGNAL_SENDTO_UU(MSA_COMM_FSM,
                                                                MSA_SIG_COMM_SEND_MESSAGE_SIZE_OK,
                                                                MSA_CONFIRM_DIALOG_OK,
                                                                ((MmsMessageFolderInfo *)pstSig->p_param)->msgId);
            } /* if */
            MSA_FREE(pstSig->p_param);
            pstSig->p_param = NULL;
            break; 
        }
        case MSA_SIG_COMM_SEND_MESSAGE_SIZE_OK:
        {
            if (MSA_CONFIRM_DIALOG_OK == (MsaConfirmDialogResult)pstSig->u_param1)
            {
                Msa_VerifySendMode(MSA_SIG_COMM_SEND_VERIFICATION_RSP);
            }
            else
            {
                Msa_CleanupSendInstance(MMS_RESULT_CANCELLED_BY_USER, FALSE);
            } /* if */
            break;
        }
        case MSA_SIG_COMM_SEND_VERIFICATION_RSP:
        {
            if (MSA_CONFIRM_DIALOG_OK == (MsaConfirmDialogResult)pstSig->u_param1)
            {
                if (!msaPmViewShow(MSA_STR_ID_SENDING_MSG,
                            TRUE,
                            TRUE,
                            MSA_COMM_FSM,
                            MSA_SIG_COMM_SEND_CANCEL))
                {
                    msaPmViewRemove();
                    Msa_CleanupSendInstance(MMS_RESULT_ERROR, FALSE);
                    return;
                }            
                MMSif_sendMessage(WE_MODID_MSA, s_Msa_pstSendInstance->msgId);
            }
            else
            {
                Msa_CleanupSendInstance(MMS_RESULT_CANCELLED_BY_USER, FALSE);            
            } /* if */
            break;
        }    
        case MSA_SIG_COMM_SEND_MSG_RESPONSE:
        {
            Msa_CleanupSendInstance((MmsResult)pstSig->u_param1, FALSE);
            break;
        }
        case MSA_SIG_COMM_SEND_CANCEL:
        {
            if (NULL != s_Msa_pstSendInstance)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH,
                                            WE_MODID_MSA,
                                            "(%s) (%d) cancelling a send operation!\n",
                                            __FILE__, 
                                            __LINE__));
                MMSif_sendCancel(WE_MODID_MSA, s_Msa_pstSendInstance->msgId);
            }
            else
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH,
                                            WE_MODID_MSA,
                                            "(%s) (%d) Failed to cancel a send operation\n",
                                            __FILE__,
                                            __LINE__));
            }
            /* will generate a SEND_MSG_RESPONSE with result CANCELLED_BY_USER */
            break;
        }
    /******************************************************************************
     *  Download signals	
     *****************************************************************************/
        case MSA_SIG_COMM_DOWNLOAD:
        {
            /* init instance, get message info */
            if (pstSig->u_param2 > 0)
            {
                if (Msa_CreateDownloadInstance((MsaStateMachine)pstSig->u_param1, 
                                                                        (unsigned int )pstSig->i_param,
                                                                        pstSig->u_param2))
                {
                    /* verify network settings, and fail on faulty settings */
                    if (!Msa_CheckSettings(MSA_SIG_COMM_SETTINGS_FAILED_DIALOG_RSP))
                    {
                        break;
                    }
                    /* Get the message type */
                    msaSrhGetMessageInfo(MSA_COMM_FSM,
                                                            MSA_SIG_COMM_DOWNLOAD_MSG_TYPE_RSP,
                                                            s_Msa_pstDownloadInstance->msgId);
                }
                else
                {
                    Msa_CleanupDownloadInstance(MMS_RESULT_ERROR, FALSE);
                }
            }
            else
            {
                Msa_CleanupDownloadInstance(MMS_RESULT_ERROR, FALSE);
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH,
                                            WE_MODID_MSA,
                                            "(%s) (%d) erroneous message WID!\n",
                                            __FILE__,
                                            __LINE__));
            }
            break;
        }
        case MSA_SIG_COMM_DOWNLOAD_MSG_TYPE_RSP:
        {
            if ((NULL != pstSig->p_param) && 
                ((MMS_SUFFIX_NOTIFICATION == ((MmsMessageFolderInfo *)pstSig->p_param)->suffix) ||
                (MMS_SUFFIX_IMMEDIATE == ((MmsMessageFolderInfo *)pstSig->p_param)->suffix)))
            {
                /* The msg type is ok, save on move on */
                s_Msa_pstDownloadInstance->msgType = 
                    ((MmsMessageFolderInfo *)pstSig->p_param)->suffix;
                /* refresh roaming/bearer status */
                msaUpdateNetworkStatusInfo(MSA_COMM_FSM,
                                                                MSA_SIG_COMM_DOWNLOAD_NETWORK_STATUS_RSP);
                MSA_FREE(pstSig->p_param);
                pstSig->p_param = NULL;
            }
            else
            {
                Msa_CleanupDownloadInstance(MMS_RESULT_ERROR, FALSE);
            }
            break;
        }
        case MSA_SIG_COMM_DOWNLOAD_NETWORK_STATUS_RSP:
        {
            /* We should read a notification */
            if (!msaCreateGetNotifInstance(&s_Msa_pstDownloadInstance->getNotifInstance,
                                                                s_Msa_pstDownloadInstance->msgId,
                                                                s_Msa_pstDownloadInstance->msgType,
                                                                MSA_COMM_FSM,
                                                                MSA_SIG_COMM_DOWNLOAD_NOTIF_FILE_NOTIFY_RSP))
            {
                Msa_CleanupDownloadInstance(MMS_RESULT_ERROR, FALSE);
                break;
            }

            MSA_SIGNAL_SENDTO(MSA_COMM_FSM, 
                MSA_SIG_COMM_DOWNLOAD_NOTIF_FILE_NOTIFY_RSP);
            break;
        }
        case MSA_SIG_COMM_DOWNLOAD_NOTIF_FILE_NOTIFY_RSP:
        {
            iRet = msaGetNotifFSM(s_Msa_pstDownloadInstance->getNotifInstance);
            if (MSA_READ_NOTIF_DELAYED == iRet) 
            {   /* async, lets wait for file notify */
                WE_LOG_MSG((WE_LOG_DETAIL_LOW,
                                            WE_MODID_MSA,
                                            "(%s) (%d) delayed file operation\n",
                                            __FILE__,
                                            __LINE__));
                break; 
            }
            else if (MSA_READ_NOTIF_DONE != iRet) /* something went wrong */
            {
                (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_NOTIF),
                                                    MSA_DIALOG_ERROR);
                Msa_CleanupDownloadInstance(MMS_RESULT_ERROR, FALSE);
                break;
            }
            /* verify roaming/retrieval modes (immediate, delayed, restricted...)*/
            Msa_VerifyRetrievalMode(MSA_SIG_COMM_DOWNLOAD_VERIFICATION_RSP);
            break;
        }
        case MSA_SIG_COMM_DOWNLOAD_VERIFICATION_RSP:
        {
            /* End user has chosen whether to download message or not */
            if (MSA_CONFIRM_DIALOG_OK != pstSig->u_param1)
            {
                Msa_CleanupDownloadInstance(MMS_RESULT_CANCELLED_BY_USER, FALSE);
                break;
            }

            MMSif_retrieveMessage(WE_MODID_MSA, 
                s_Msa_pstDownloadInstance->getNotifInstance->msgId);
            break;
        }
        case MSA_SIG_COMM_DOWNLOAD_RESPONSE:
        {
            s_Msa_pstDownloadInstance->msgId = pstSig->u_param2;
            Msa_CleanupDownloadInstance((MmsResult)pstSig->u_param1, FALSE);
            break;
        }    
        case MSA_SIG_COMM_DOWNLOAD_CANCEL:
        {
            if (NULL != s_Msa_pstDownloadInstance)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) Download cancel received, msgId = %d!\n", __FILE__,
                    __LINE__, s_Msa_pstDownloadInstance->getNotifInstance->msgId));
                MMSif_retrieveCancel(WE_MODID_MSA, 
                    s_Msa_pstDownloadInstance->getNotifInstance->msgId);
                /* The cleanup is handled when the MSA_SIG_COMM_DOWNLOAD_RESPONSE
                   is received */
            }
            else
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) erroneous operation! no instance exists!\n", 
                    __FILE__, __LINE__));   
            }
            break;
        }
        case MSA_SIG_COMM_PROGRESS_STATUS:
        {
            pstMsgStatus = (MmsProgressStatus *)pstSig->p_param;
            Msa_HandleProgress(pstMsgStatus);
            MSA_FREE(pstSig->p_param);
            pstSig->p_param = NULL;
            break;
        }
        case MSA_SIG_COMM_SETTINGS_FAILED_DIALOG_RSP:
        {
            Msa_CleanupDownloadInstance(MMS_RESULT_ERROR, FALSE);
            break;
        }
        default:
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH,
                                        WE_MODID_MSA,
                                        "(%s) (%d) Unhandled signal received (%d).\n",
                                        __FILE__,
                                        __LINE__,
                                        pstSig->type));
        }
    }
    /* Remove signal data */
    msaSignalDelete(pstSig);
}

/*! \brief Displays an error dialog that corresponds with the download result
 *         code.
 *
 * \param result The #MsaDownloadResult.
 * \param fsm    The callback FSM.
 * \param signal The callback signal.
 *****************************************************************************/
WE_BOOL Msa_ShowDownloadErrorDialog
(
    MmsResult eResult,
    MsaStateMachine eFsm,
    int iSignal
)
{
    WeStringHandle strHandle = 0;
    switch(eResult)
    {
        case MMS_RESULT_OK:
        {
            /* Do nothing */
            break;
        }
        case MMS_RESULT_MAX_RETRIEVAL_SIZE_EXCEEDED:
        {
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_RETR_SIZE_EXCEEDED);
            break;
        }

        case MMS_RESULT_INVALID_ADDRESS:
        case MMS_RESULT_INVALID_APPLICATION_HEADER:
        case MMS_RESULT_INVALID_CONTENT_ID:
        case MMS_RESULT_INVALID_CONTENT_LOCATION:
        case MMS_RESULT_INVALID_CONTENT_TYPE:
        case MMS_RESULT_INVALID_DATE:
        case MMS_RESULT_INVALID_EXPIRY_TIME:
        case MMS_RESULT_INVALID_HEADER_SIZE:
        case MMS_RESULT_INVALID_MSG_CLASS:
        case MMS_RESULT_INVALID_REPLY_CHARGING_DEADLINE:
        case MMS_RESULT_INVALID_SIZE:
        case MMS_RESULT_INVALID_STORAGE_TYPE:
        case MMS_RESULT_INVALID_SUBJECT:
        case MMS_RESULT_MESSAGE_CORRUPT:
        case MMS_RESULT_COMM_ILLEGAL_PDU:
        case MMS_RESULT_COMM_UNEXPECTED_MESSAGE:
        {
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_INVALID_MESSAGE);
            break;
        }
        case MMS_RESULT_INVALID_VERSION:
        case MMS_RESULT_COMM_WRONG_MMS_VERSION:
        {
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_INVALID_MMS_VERSION);
            break;
        }
        case MMS_RESULT_COMM_TIMEOUT:
        {
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_COMM_TIMEOUT);
            break;
        }
        case MMS_RESULT_COMM_BAD_GATEWAY:
        case MMS_RESULT_COMM_FORBIDDEN:
        case MMS_RESULT_COMM_PROXY_ERROR:
        case MMS_RESULT_COMM_UNAUTHORIZED:
        case MMS_RESULT_COMM_UNAVAILABLE:
        case MMS_RESULT_COMM_SERVER_ERROR:
        {
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_SERVER_ERROR);
            break;
        }
        case MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE:
        {
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_NO_PERSISTENT_STORAGE);
            break;
        }    
        case MMS_RESULT_CANCELLED_BY_USER:
        {
            /* No dialog */
            break;
        }
        case MMS_RESULT_DRM_NOT_SUPPORTED:
        {
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_NO_DRM_SUPPORT);
            break;
        }
        case MMS_RESULT_COMM_ENTITY_TOO_LARGE:
        case MMS_RESULT_COMM_FAILED:
        case MMS_RESULT_COMM_FILE_NOT_FOUND:
        case MMS_RESULT_COMM_HEADER_TAG:
        case MMS_RESULT_COMM_LIMITATIONS_NOT_MET:
        case MMS_RESULT_COMM_SECURITY_ERROR:
        case MMS_RESULT_COMM_STATUS:
        case MMS_RESULT_COMM_TRANSACTION_ID:
        case MMS_RESULT_COMM_UNSUPPORTED_VERSION:
        case MMS_RESULT_COMM_MMSC_VERSION:
        case MMS_RESULT_COMM_URI_TOO_LARGE:
        case MMS_RESULT_ERROR:
        default:
        {
            /* Use a general */
            strHandle = MSA_GET_STR_ID(MSA_STR_ID_DOWNLOAD_FAILED);
            break;
        }
    }
    if (0 != strHandle)
    {
        (void)msaShowDialogWithCallback(strHandle,
                                                MSA_DIALOG_ERROR,
                                                eFsm,
                                                iSignal);
        return TRUE;
    }
    return FALSE;
}

/*! \brief Displays an error dialog that corresponds with the download result
 *         code.
 *
 * \param result The #MsaDownloadResult.
 * \param fsm    The callback FSM.
 * \param signal The callback signal.
 *****************************************************************************/
WE_BOOL Msa_ShowSendDialog
(
    MmsResult eResult,
    MsaStateMachine eFsm,
    int iSignal)
{
    WeStringHandle uiStrHandle = 0;
    MsaDialogType  eDialogType = MSA_DIALOG_ERROR;

    switch(eResult)
    {
        case MMS_RESULT_OK:
        {
            uiStrHandle = MSA_GET_STR_ID(MSA_STR_ID_MESSAGE_SENT);
            eDialogType = MSA_DIALOG_INFO;
            break;
        }
        case MMS_RESULT_COMM_UNSUPPORTED_MEDIA_TYPE:
        {
            uiStrHandle = MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_SEND_REFUSED);
            break;
        }
        case MMS_RESULT_CANCELLED_BY_USER:
        {
            /* Do not display any dialog for cancel */
            break;
        }
        case MMS_RESULT_COMM_TIMEOUT:
        {
            uiStrHandle = MSA_GET_STR_ID(MSA_STR_ID_COMM_TIMEOUT);
            break;
        }
        case MMS_RESULT_INVALID_VERSION:
        case MMS_RESULT_COMM_WRONG_MMS_VERSION:
        {
            uiStrHandle = MSA_GET_STR_ID(MSA_STR_ID_INVALID_MMS_VERSION);
            break;
        }    
        case MMS_RESULT_COMM_BAD_GATEWAY:
        case MMS_RESULT_COMM_FORBIDDEN:
        case MMS_RESULT_COMM_PROXY_ERROR:
        case MMS_RESULT_COMM_UNAUTHORIZED:
        case MMS_RESULT_COMM_UNAVAILABLE:
        case MMS_RESULT_COMM_SERVER_ERROR:
        {
            uiStrHandle = MSA_GET_STR_ID(MSA_STR_ID_SERVER_ERROR);
            break;
        }    
        case MMS_RESULT_COMM_ENTITY_TOO_LARGE:
        case MMS_RESULT_COMM_FAILED:
        case MMS_RESULT_COMM_FILE_NOT_FOUND:
        case MMS_RESULT_COMM_HEADER_TAG:
        case MMS_RESULT_COMM_LIMITATIONS_NOT_MET:
        case MMS_RESULT_COMM_SECURITY_ERROR:
        case MMS_RESULT_COMM_STATUS:
        case MMS_RESULT_COMM_TRANSACTION_ID:
        case MMS_RESULT_COMM_UNSUPPORTED_VERSION:
        case MMS_RESULT_COMM_MMSC_VERSION:
        case MMS_RESULT_COMM_URI_TOO_LARGE:
        case MMS_RESULT_ERROR:
        default:
        {
            /* Display "failed to send" for all other error codes */
            uiStrHandle = MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_SEND);
            break;
        }
    }
    /* Check if the dialog should be displayed */
    if (0 != uiStrHandle)
    {
        (void)msaShowDialogWithCallback(uiStrHandle, eDialogType, eFsm, iSignal);
        return TRUE;
    }
    return FALSE;
}

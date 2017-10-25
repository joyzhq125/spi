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

/*! \file mamv.c
 *  \brief Message viewer control logic. 
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"
#include "We_Core.h"
#include "We_Cmmn.h"
#include "We_Act.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Env.h"
#include "Msa_Intsig.h"
#include "Msa_Mv.h"
#include "Msa_Mmv.h"
#include "Msa_Mr.h"
#include "Msa_Uimv.h"
#include "Msa_Mem.h"
#include "Msa_Crh.h"
#include "Msa_Uidia.h"
#include "Msa_Uipm.h"
#include "Msa_Core.h"
#include "Msa_Comm.h"
#include "Msa_Notif.h"
#include "Msa_Addr.h"
#include "Msa_Pbh.h"
#include "Msa_Uicmn.h"
#include "Msa_Srh.h"
#include "Msa_Conf.h"

/*!\enum MsaMvInstance
 */
typedef struct MsaMvInstanceSt 
{
    MsaMessage              *theMessage;
    MsaGetNotifInstance     *getNotifInstance;
    MmsMessageFolderInfo    *fInfo;
}MsaMvInstance;

/******************************************************************************
 * Constants
 *****************************************************************************/
#define MSA_MAX_DATE_SIZE               50 /* !< Maximum buffer size for date
                                                 conversion */

/******************************************************************************
 * Static variables
 *****************************************************************************/
static MsaMvInstance *mvInstance;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void activatePropertyView(const MsaMvInstance *inst);
static WE_BOOL handleReadReport(MsaMvInstance *inst, MmsMsgReadReport *rr);
static WE_BOOL handleDeliveryReport(MsaMvInstance *inst, 
    MmsMsgDeliveryReportInd *dr);
static void handleActivate(const MmsMessageFolderInfo *info);
static void msaViewer(MsaSignal *sig);

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief       Deals with opening of read reports 
 *
 * \param rr    The read report
 *****************************************************************************/
static WE_BOOL handleReadReport(MsaMvInstance *inst, MmsMsgReadReport *rr)
{
    char *time = MSA_ALLOC(MSA_MAX_DATE_SIZE); 
    WE_UINT32 status;
    
    if (NULL == rr)
    {
        return FALSE;
    }
    if ((NULL == inst) || (NULL == inst->fInfo) || 
        (inst->fInfo->msgId != rr->msgIdFile))
    {
        /* This is not a valid request */
        MSA_FREE(rr->address);
        MSA_FREE(rr->serverMessageId);
        MSA_FREE(rr->text);
        MSA_FREE(rr);
        return FALSE;
    }
    /* Create text for the status field */
    switch(rr->readStatus)
    {
    case MMS_READ_STATUS_READ:
        status = MSA_STR_ID_READ;
        break;
    case MMS_READ_STATUS_DELETED_WITHOUT_BEING_READ:
        status = MSA_STR_ID_NOT_READ;
        break;
    default:
        status = MSA_STR_ID_UNKOWN;
        break;
    }
    /* Get current time */
    we_cmmn_time2str(rr->date, time);
    /* Crete GUI */
    if (!msaCreateMvRrDrView(MSA_STR_ID_READ_REPORT, rr->address, status, time))
    {
        /* Delete the GUI so that all resources are de-allocated */
        msaDeleteMvRrDrView();
        return FALSE;
    }
    if (0 != rr->msgIdFile)
    {
        /* Set the read report as "read" */
        MMSif_setReadMark(WE_MODID_MSA, rr->msgIdFile, TRUE);
    }
    MSA_FREE(time);
    MSA_FREE(rr->address);
    MSA_FREE(rr->serverMessageId);
    MSA_FREE(rr->text);
    /*lint -e{774} */
    MSA_FREE(rr);
    msaPmViewRemove();
    return TRUE;
}

/*!
 * \brief       Deals with opening of delivery reports 
 *
 * \param dr    The delivery report
 *****************************************************************************/
static WE_BOOL handleDeliveryReport(MsaMvInstance *inst, 
    MmsMsgDeliveryReportInd *dr)
{
    char *time = MSA_ALLOC(MSA_MAX_DATE_SIZE);
    WE_UINT32 status;

    if (NULL == dr)
    {
        return FALSE;
    }
    if ((NULL == inst) || (NULL == inst->fInfo) || 
        (inst->fInfo->msgId != dr->msgIdFile))
    {
        /* This is not a valid request */
        MSA_FREE(dr->address);
        MSA_FREE(dr->serverMsgId);
        MSA_FREE(dr);
        return FALSE;
    }
    
    switch (dr->status)
    {
    case MMS_STATUS_EXPIRED:
        status = MSA_STR_ID_EXPIRED;
        break;
    case MMS_STATUS_RETRIEVED:
        status = MSA_STR_ID_RETRIEVED;
        break;
    case MMS_STATUS_REJECTED:
        status = MSA_STR_ID_REJECTED;
        break;
    case MMS_STATUS_DEFERRED:
        status = MSA_STR_ID_DEFERRED;
        break;
    case MMS_STATUS_UNRECOGNIZED:
        status = MSA_STR_ID_UNRECOGNIZED;
        break;
    case MMS_STATUS_INDETERMINATE:
        status = MSA_STR_ID_INDETERMINATE;
        break;
    case MMS_STATUS_FORWARDED:
        status = MSA_STR_ID_FORWARDED;
        break;
    default:
        status = MSA_STR_ID_UNKOWN;
        break;
    }
    /* Convert time as integer to time as string */
    we_cmmn_time2str(dr->date, time);
    /* Show GUI */
    if (!msaCreateMvRrDrView(MSA_STR_ID_DELIV_REPORT, dr->address, status, time))
    {
        /* Delete the GUI so that all resources are de-allocated */
        msaDeleteMvRrDrView();
        return FALSE;
    }
    /* Set the message read */
    if (0 != dr->msgIdFile)
    {
        MMSif_setReadMark(WE_MODID_MSA, dr->msgIdFile, TRUE);
    }
    MSA_FREE(time);
    MSA_FREE(dr->address);
    MSA_FREE(dr->serverMsgId);
    MSA_FREE(dr->text);
    /*lint -e{774} */
    MSA_FREE(dr);
    msaPmViewRemove();
    return TRUE;
}

/*!
 * \brief           Handles activation of the viewer, i.e. what to do once the 
 *                  user has decided to view a message.
 * \param info      Information about the message to open/view
 *****************************************************************************/
static void handleActivate(const MmsMessageFolderInfo *info)
{
    switch (info->suffix)
    {
    case MMS_SUFFIX_READREPORT:
    case MMS_SUFFIX_DELIVERYREPORT:
        MMSif_getReport(WE_MODID_MSA, info->msgId);
        break;
    case MMS_SUFFIX_SEND_REQ:
    case MMS_SUFFIX_MSG:
    case MMS_SUFFIX_TEMPLATE:
        msaStartMr(MSA_MR_STARTUP_VIEW, info->msgId, info->suffix,
            MSA_MV_FSM, MSA_SIG_MV_MR_DONE);
        break;
    case MMS_SUFFIX_NOTIFICATION:
    case MMS_SUFFIX_IMMEDIATE:
        Msa_DownloadMessage(MSA_MV_FSM, MSA_SIG_MV_DOWNLOAD_RSP, info->msgId);
         break; 
    case MMS_SUFFIX_INFO:
    case MMS_SUFFIX_ERROR:
    default:
        msaPmViewRemove();        
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_WRONG_MESSAGE_TYPE), 
            MSA_DIALOG_ERROR);
        (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
        break;
    }
}

/*! \brief
 *	
 * \param inst
 * \param termApp Tells if the this FSM is responsible for terminating the
 *                application.
 *****************************************************************************/
static void deActivate(MsaMvInstance **inst, WE_BOOL termApp)
{
    if (inst && *inst)
    {
        MSA_FREE((*inst)->fInfo);
        (*inst)->fInfo = NULL;
        msaFreeMessage(&(*inst)->theMessage);
        if ((*inst)->getNotifInstance) 
        {
            msaCleanupGetNotifInstance(&(*inst)->getNotifInstance);
        } /* if */
        MSA_FREE((*inst));
        *inst = NULL;
    }
    msaDeleteMvPropView();
    msaDeleteMvRrDrView();
    msaDeleteMvNavMenu();
    msaPmViewRemove();
    if (termApp && (MSA_STARTUP_MODE_VIEW == msaGetStartupMode()))
    {
        (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
    }    
}


/*! \brief Name lookup
 *
 * \param addrItem The current address items.
 *****************************************************************************/
static void nameLookup(const MsaAddrItem *addrItem)
{
    while(NULL != addrItem)
    {
        if (NULL == addrItem->name)
        {
            msaPbhLookupName(MSA_MV_FSM, MSA_SIG_MV_NAME_LOOKUP_RSP, 
                addrItem->address, addrItem->addrType);
        }
        addrItem = addrItem->next;
    }
}

/*!
 * \brief Activates the property view.
 * 
 *\param inst The current instance, see #MsaMessage.
 *****************************************************************************/
static void activatePropertyView(const MsaMvInstance *inst)
{
    MsaMessage  *theMessage = inst->theMessage;
    char        *str        = NULL;

    if (!inst || !inst->theMessage || !inst->theMessage->msgProperties) 
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM), 
            MSA_DIALOG_ERROR);
        (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) erroneous message header\n", __FILE__, __LINE__));
        return;
    } /* if */
    
    /* Name lookup */
    nameLookup(theMessage->msgProperties->to);
    nameLookup(theMessage->msgProperties->cc);
    /* Display the form */
    if (!msaCreateMvPropView((MmsFileType)theMessage->fileType, 
        theMessage->msgProperties, 
        MSA_MV_FSM, MSA_SIG_MV_PROP_OK, 
        MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE))
    {
        msaDeleteMvPropView();
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM), 
            MSA_DIALOG_ALERT);
        (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
        return;
    }
    /*lint -e{774} */
    MSA_FREE(str);
    str = NULL;
    MMSif_setReadMark(WE_MODID_MSA, theMessage->msgId, TRUE);

    /* Check for read report. Message must be of type MMS_SUFFIX_MSG */
    if (theMessage->msgProperties->readReport && 
        MMS_SUFFIX_MSG == theMessage->fileType && 
        NULL != theMessage->msgProperties->from)
    {
        if (!inst->fInfo->read)
        {
            if (MSA_SEND_READ_REPORT_ON_REQUEST == 
                (MsaSendReadReportType)msaGetConfig()->sendReadReport)
            {
                /* ask End User whether or not to send a read report */
                (void)msaShowConfirmDialog(MSA_STR_ID_READ_REPORT_SEND,
                    NULL, MSA_MV_FSM, MSA_SIG_MV_SEND_READ_REPORT, 0);
            }
            else if (MSA_SEND_READ_REPORT_ALWAYS ==
                (MsaSendReadReportType)msaGetConfig()->sendReadReport)
            {
                /* send a read report without asking */
                (void)MSA_SIGNAL_SENDTO_U(MSA_MV_FSM, 
                    MSA_SIG_MV_SEND_READ_REPORT,
                    (MsaConfirmDialogResult)MSA_CONFIRM_DIALOG_OK);
            }
        }
    }
}

/*!
 * \brief Signal handler for the message viewer set of views
 * \param sig The received signal 
 *****************************************************************************/
static void msaViewer(MsaSignal *sig)
{
    MmsMsgId    wid;
    MsaAddrItem *tmpAddrItem;
    
    switch ((MsaMvSignalId) sig->type)
    {
    case MSA_SIG_MV_ACTIVATE: /* IU = type, wid */
        wid      = sig->u_param1;
        
        if (wid < 1) 
        {/* Nothing to view */
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM), 
                MSA_DIALOG_ERROR);
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            break; 
        } /* if */
               
        /* always request msg-info (read-status, etc) */
        msaSrhGetMessageInfo(MSA_MV_FSM, MSA_SIG_MV_GET_MSG_TYPE_RSP, wid);

        /* Allocate the instance */
        MSA_CALLOC(mvInstance, sizeof(MsaMvInstance));
        break;

    case MSA_SIG_MV_GET_MSG_TYPE_RSP:
        if (NULL == sig->p_param) 
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Failed to retrieve message information\n", 
                __FILE__, __LINE__));            
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM), 
                MSA_DIALOG_ERROR);
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            break;
        } /* if */

        mvInstance->fInfo = (MmsMessageFolderInfo *)sig->p_param;

        handleActivate(mvInstance->fInfo);
        break;
       
    case MSA_SIG_MV_MR_DONE:
        /* A message has been loaded by MR! */
        mvInstance->theMessage = sig->p_param;
        msaPmViewRemove();
        if (MSA_MR_OK != sig->u_param1 )
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) failed to create instance\n", __FILE__, __LINE__));
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM), 
                MSA_DIALOG_ERROR);
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            break;
        }
      
        (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_PROP_ACTIVATE);
        break;
        
    case MSA_SIG_MV_DR:
        /* A delivery report has been read! */
        if (!handleDeliveryReport(mvInstance, (MmsMsgDeliveryReportInd *)sig->p_param))
        {
           if ((NULL == mvInstance) || (NULL == mvInstance->fInfo) || 
                (mvInstance->fInfo->msgId != ((MmsMsgReadReport *)sig->p_param)->msgIdFile))
            {
                /*Probably a notification about a new report, and not a report we asked for*/
            }else
            {
                
                (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM), 
                    MSA_DIALOG_ERROR);
                (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            }
            break;
        }
        break;
        
    case MSA_SIG_MV_RR:
        /* A read reply has been read! */
        if (!handleReadReport(mvInstance, (MmsMsgReadReport *)sig->p_param))
        {
            if ((NULL == mvInstance) || (NULL == mvInstance->fInfo) || 
                (mvInstance->fInfo->msgId != ((MmsMsgReadReport *)sig->p_param)->msgIdFile))
            {
                /*Probably a notification about a new report, and not a report we asked for*/
            }else
            {
                (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM), 
                    MSA_DIALOG_ERROR);
                (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            }
            break;
        }
        break;        
        
    case MSA_SIG_MV_DEACTIVATE:            
        if (mvInstance) 
        {
            deActivate(&mvInstance, TRUE);
        } /* if */
        break;

    case MSA_SIG_MV_VIEW_SMIL:
        /* Start the SIA module */
        msaPlaySmil(mvInstance->theMessage->msgId, (MmsFileType)
            mvInstance->theMessage->fileType, MSA_MV_FSM, MSA_SIG_MV_PLAY_DONE);
        break;

    case MSA_SIG_MV_SEND_READ_REPORT:
        if ((MsaConfirmDialogResult)sig->u_param1 == MSA_CONFIRM_DIALOG_OK)
        {
            /* Send the read report */
            MMSif_sendReadReport(WE_MODID_MSA, mvInstance->theMessage->msgId, 
                MMS_READ_STATUS_READ);
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) read report sent!\n", __FILE__, __LINE__));
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) read report not requested by the End User\n", 
                __FILE__, __LINE__));
        }
        break;
        
    case MSA_SIG_MV_PROP_ACTIVATE:
        activatePropertyView(mvInstance);
        break;

    /* user selected VIEW in menu or Ok in prop menu */
    case MSA_SIG_MV_VIEW_CONTENT:
    case MSA_SIG_MV_PROP_OK:
        if (MSA_MR_SMIL == mvInstance->theMessage->msgType) 
        {
            /* SMIL! Let SIA handle it. */
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_VIEW_SMIL);
        }
        else if (mvInstance->theMessage->smilInfo 
            &&   mvInstance->theMessage->smilInfo->mediaObjects)
        {
            /* Not SMIL, but has attachments we've found and decided we can 
             * handle. Start multipart/mixed viewer (also handles plain/text
             */
            (void)MSA_SIGNAL_SENDTO_P(MSA_MMV_FSM, MSA_SIG_MMV_ACTIVATE,
                mvInstance->theMessage);                
        }
        else
        {
            /* Got nothing to show .. let's die. */
             (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_MMS_EMPTY), 
                MSA_DIALOG_INFO);
           
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);            
        } /* if */
        break;
            
    case MSA_SIG_MV_NAV_ACTIVATE:
        if (!msaCreateMvNavMenu(mvInstance->theMessage->msgType))
        {
            msaDeleteMvNavMenu();
        }
        break;

    case MSA_SIG_MV_NAV_DEACTIVATE:
        msaDeleteMvNavMenu();
        break;

    case MSA_SIG_MV_PLAY_DONE:
        /*Did we play the message correctly or should we try Multipart/Mixed instead ? */
       
#ifdef MSA_BAD_SMIL_AS_MULTIPART_MIXED
        if(sig->u_param1!=0)
        {
        
            (void)MSA_SIGNAL_SENDTO_P(MSA_MMV_FSM, MSA_SIG_MMV_ACTIVATE,
                mvInstance->theMessage);
        }
#endif
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) Done playing the SMIL presentation\n", __FILE__, 
            __LINE__));
        break;

    case MSA_SIG_MV_DOWNLOAD_RSP:
        /* Check if the download operation was successful */
        if (MMS_RESULT_OK != (MmsResult)sig->u_param1)
        {
            if (!Msa_ShowDownloadErrorDialog((MmsResult)sig->u_param1, 
                MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE))
            {
                /* Clean-up if no dialog is displayed */
                (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            }
            break; 
        }
        /* Deallocate the the current instance and start with again with the 
           new msgId */
        deActivate(&mvInstance, FALSE);
        /* If the download was successful show the message */
        (void)MSA_SIGNAL_SENDTO_IU(MSA_MV_FSM, MSA_SIG_MV_ACTIVATE, 
            MMS_SUFFIX_MSG, sig->u_param2);
        /* Update folders */
        break;

    case MSA_SIG_MV_NAME_LOOKUP_RSP:
        if (NULL != mvInstance->theMessage)

        {
            /* Marge the name with the current items */
            Msa_AddrItemMerge(mvInstance->theMessage->msgProperties->to, 
                sig->p_param);
            Msa_AddrItemMerge(mvInstance->theMessage->msgProperties->cc, 
                sig->p_param);
            /* Update the message properties */
            if (!msaSetMvProperties(mvInstance->theMessage->msgProperties))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                    "(%s) (%d) failed to display message properties\n", 
                    __FILE__, __LINE__));
                (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            }
        }
        if (NULL != sig->p_param)
        {
            tmpAddrItem = sig->p_param;
            Msa_AddrListFree(&tmpAddrItem);
        }
        break;

    default :
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) MV: Unknown signal recieved(%d)\n", __FILE__, __LINE__,
            sig->type));
        break;
    } 
    msaSignalDelete(sig);
}

/*!
 * \brief Registers signal handler
 *****************************************************************************/
void msaMvInit(void)
{
    mvInstance = NULL;
    msaSignalRegisterDst(MSA_MV_FSM, msaViewer); /* set sig-handler */
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "MSA MV: initialized\n"));
}

/*!
 * \brief Terminates, by de-registering signal handler
 *****************************************************************************/
void msaMvTerminate(void)
{
    deActivate(&mvInstance, FALSE);
    /* Deregister signal handler */
    msaSignalDeregister(MSA_MV_FSM);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "MSA MV: terminated\n"));

}











































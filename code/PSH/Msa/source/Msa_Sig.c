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

/*! \file masig.c
 *  \brief Signal dispatcher for external (WE) signals.
 */

/* WE */
#include "We_Core.h"
#include "We_Log.h"
#include "We_Cfg.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"
#include "We_Act.h"

/* MMS Service */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* SIA */
#include "Sia_If.h"

/* PRS */
#include "prs_if.h"

/* MSA*/
#include "Msa_Rc.h"
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Conf.h"
#include "Msa_Sig.h"
#include "Msa_Core.h"
#include "Msa_Mv.h"
#include "Msa_Mr.h"
#include "Msa_Uisig.h"
#include "Msa_Async.h"
#include "Msa_Mcr.h"
#include "Msa_Crh.h"
#include "Msa_Uidia.h"
#include "Msa_Comm.h"
#include "Msa_Uipm.h"
#include "Msa_Del.h"
#include "Msa_Uicmn.h"
#include "Msa_Ph.h"
#include "Msa_Crh.h"
#include "Msa_Srh.h"
#include "Msa_Utils.h"
#include "Msa_Del.h"

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void weSignalHandler(WE_UINT16 signal, void *p);
static void mmsSignalHandler(WE_UINT16 signal, void *p);
static void siaSignalHandler(WE_UINT16 signal, void *p);
static void sisSignalHandler(WE_UINT16 signal, void *p);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Handler/converter for signals received from WE
 * \param signal The external signal received
 * \param p The serialized signal data
 *****************************************************************************/
static void weSignalHandler(WE_UINT16 signal, void *p)
{
    we_module_status_t *moduleStatus;
    we_module_execute_cmd_t *cl;

    /* Check that the signal is valid */
    if (NULL == p)
    {
        msaPanic(TRUE);
    }
    switch(signal)
    {
    case WE_SIG_WIDGET_NOTIFY:
    case WE_SIG_WIDGET_ACTION:
    case WE_SIG_WIDGET_USEREVT:
        if (NULL != p)
        {
            msaWidgetSignalHandler(signal, p);
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) erroneous widget signal\n", __FILE__, __LINE__));
        }
        break;

    case WE_SIG_MODULE_STATUS:
        if (NULL == (moduleStatus = ((we_module_status_t*)p)))
        {
            break;
        }

        if (MODULE_STATUS_TERMINATED == moduleStatus->status)
        {
            if (WE_MODID_MMS == moduleStatus->modId)
            {
                (void)MSA_SIGNAL_SENDTO_U(MSA_CORE_FSM, 
                    MSA_SIG_CORE_MODULE_TERMINATED, WE_MODID_MMS);
            }
            else if (WE_MODID_SIA == moduleStatus->modId)
            {
                (void)MSA_SIGNAL_SENDTO_U(MSA_CORE_FSM, 
                    MSA_SIG_CORE_MODULE_TERMINATED, WE_MODID_SIA);
            }
            else if (WE_MODID_SIS == moduleStatus->modId)
            {
                (void)MSA_SIGNAL_SENDTO_U(MSA_CORE_FSM, 
                    MSA_SIG_CORE_MODULE_TERMINATED, WE_MODID_SIS);
            }
        }
        else if (MODULE_STATUS_ACTIVE == moduleStatus->status)
        {
            /* MMS */
            if (WE_MODID_MMS == moduleStatus->modId)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                    "The MMS Service is started.\n"));
                msaModuleStarted(WE_MODID_MMS);
                /* Get notifications for new messages */
                MMSif_applicationReg(WE_MODID_MSA);
                /* The MMS Service is started, get the configuration data */
                (void)MSA_SIGNAL_SENDTO(MSA_CONFIG_FSM, 
                    MSA_SIG_CONF_INITIAL_ACTIVATE);

            } /* SIA */
            else if (WE_MODID_SIA == moduleStatus->modId)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                    "The SIA is started.\n"));
                (void)MSA_SIGNAL_SENDTO_U(MSA_CORE_FSM, 
                    MSA_SIG_CORE_MODULE_ACTIVE, WE_MODID_SIA);
            }
            else if (WE_MODID_SIS == moduleStatus->modId)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                    "The SIA is started.\n"));
                (void)MSA_SIGNAL_SENDTO_U(MSA_CORE_FSM, 
                    MSA_SIG_CORE_MODULE_ACTIVE, WE_MODID_SIS);
            } 
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA,
                "weSignalHandler erroneous signal module status\n"));
        }
        break;

    case WE_SIG_MODULE_EXECUTE_COMMAND:
        cl = ((we_module_execute_cmd_t*)p);
        if (MSA_STARTUP_MODE_NONE == msaGetStartupMode())
        {
            WE_MODULE_IS_ACTIVE(WE_MODID_MSA);
            msaSetStartupParameters(cl);
            if (MSA_STARTUP_MODE_PROVISIONING == msaGetStartupMode())
            {
                /*
                 *	Don't need to start MMS. Just lay low and wait.
                 */
            }
            else
            {
                if (!msaIsModuleStarted(WE_MODID_MMS))
                {
                    /*
                     *	Start MMS Service. MSA startup continues on receival
                     *  of MMS startup confirmation (WE_SIG_MODULE_STATUS)
                     */
                    msaModuleStart(WE_MODID_MMS, FALSE, (MsaStateMachine)0, 
                        0);
                }
            } /* if */
        }
        else
        {
			WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) The MSA is busy!\n", __FILE__, 
                __LINE__));
        }
        break;

    case WE_SIG_MODULE_TERMINATE:
        (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);        
        break;

    case WE_SIG_TIMER_EXPIRED:
        /* Timer handling */
        if (NULL == p)
        {
            break;
        }
        if (MSA_PM_TIMER_ID == ((we_timer_expired_t *)p)->timerID)
        {
            msaPmHandleTimer();
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
                    "Unhandled timer event timerId=%d",((we_timer_expired_t *)p)->timerID));
        }
        break;

    case WE_SIG_FILE_NOTIFY:
        if (NULL != p)
        {
            (void)MSA_SIGNAL_SENDTO_IU(MSA_ASYNC_FSM, MSA_SIG_ASYNC_NOTIFY,
                ((we_file_notify_t *)p)->eventType, 
                (unsigned)((we_file_notify_t *)p)->fileHandle);
        }
        break;

    case WE_SIG_PIPE_NOTIFY:
        (void)MSA_SIGNAL_SENDTO_IU(MSA_ASYNC_FSM, MMS_SIG_ASYNC_PIPE_NOTIFY, 
            ((we_pipe_notify_t *)p)->eventType, 
            ((we_pipe_notify_t *)p)->handle);
        break;

    case WE_SIG_REG_RESPONSE:
        (void)MSA_SIGNAL_SENDTO_P(MSA_CONFIG_FSM, MSA_SIG_CONF_GET_REG_RSP, p);
        /* Do not destruct this signal yet */
        return;

    case WE_SIG_CONTENT_SEND_ACK:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) unhandled content routing response!\n", __FILE__, 
                __LINE__));
        break;

    case WE_SIG_TEL_MAKE_CALL_RESPONSE:
        msaSrhMakeCallRsp((we_tel_make_call_resp_t *)p);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA,
            "(%s) (%d) Erroneous signal received!\n", __FILE__,  __LINE__));
        break;
    }
    /* Delete signal */
    WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p);
}

/*!
 * \brief Handler/converter for signals received from the MMS Service. 
 * \param signal The external signal received that we should convert into an 
 *           internal representation
 * \param p The serialized signal data.
 *****************************************************************************/
static void mmsSignalHandler(WE_UINT16 signal, void *p)
{
    MmsRRReplySig *rrSig = NULL;
    MmsProgressStatus *msgStatus;

    switch(signal)
    {
    case MMS_SIG_CREATE_MSG_REPLY:
        {
            MmsMsgReplySig *rep = (MmsMsgReplySig *)p;
            (void)MSA_SIGNAL_SENDTO_UU(MSA_MCR_FSM, 
                MSA_SIG_MCR_CREATE_MSG_RESPONSE, rep->result, rep->msgId);
        }
        break;

    case MMS_SIG_SEND_MSG_REPLY:
        (void)MSA_SIGNAL_SENDTO_U(MSA_COMM_FSM, MSA_SIG_COMM_SEND_MSG_RESPONSE, 
            ((MmsResultSig*)p)->result);
        break;
        
    case MMS_SIG_MSG_DONE_REPLY:
        (void)MSA_SIGNAL_SENDTO_UU(MSA_MCR_FSM, 
            MSA_SIG_MCR_CREATE_MSG_DONE_RESPONSE, 
            ((MmsMsgReplySig*)p)->result, 
            ((MmsMsgReplySig*)p)->msgId);
        break;        

    case MMS_SIG_ERROR:
        msaHandleMmsErrors(((MmsResultSig*)p)->result);
        break;
       
    case MMS_SIG_DELETE_MSG_REPLY:
        msaDelHandleDeleteRsp(((MmsResultSig*)p)->result);
        break;

    case MMS_SIG_READ_REPORT_IND:
        /* A read report is ready to be viewed */
        (void)MSA_SIGNAL_SENDTO_P(MSA_MV_FSM, MSA_SIG_MV_RR, p);
        return;

    case MMS_SIG_DELIVERY_REPORT:
        /* A delivery report is ready to be viewed */
        (void)MSA_SIGNAL_SENDTO_P(MSA_MV_FSM, MSA_SIG_MV_DR, p);
        return;

    case MMS_SIG_RETRIEVE_MSG_REPLY:
        (void)MSA_SIGNAL_SENDTO_UU(MSA_COMM_FSM, 
            MSA_SIG_COMM_DOWNLOAD_RESPONSE, ((MmsMsgReplySig*)p)->result,  
            ((MmsMsgReplySig*)p)->msgId);
        break;

    case MMS_SIG_SEND_RR_REPLY:
        rrSig = (MmsRRReplySig*)p;
        if (MMS_RESULT_OK == rrSig->result ||
            MMS_RESULT_CANCELLED_BY_USER == rrSig->result)
        {
            msaDeleteMsg(rrSig->rrId, FALSE);
        }
        else
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_SEND_RR), 
                MSA_DIALOG_ALERT);            
        }
        break;

    case MMS_SIG_GET_MSG_HEADER_REPLY:
        if (MSA_GET_HEADER_RSP_MR == ((MmsGetMsgHeaderReply*)p)->userData)
        {
            (void)MSA_SIGNAL_SENDTO_UP(MSA_MR_FSM, MSA_SIG_MR_GET_HEADER_RSP, 
                ((MmsGetMsgHeaderReply*)p)->result, 
                ((MmsGetMsgHeaderReply*)p)->header);
            /* Prevent the destruct function from releasing the header data */
            ((MmsGetMsgHeaderReply*)p)->header = NULL;
        }
        else if (MSA_GET_HEADER_RSP_MCR == ((MmsGetMsgHeaderReply*)p)->userData)
        {
            (void)MSA_SIGNAL_SENDTO_UP(MSA_MCR_FSM, MSA_SIG_MCR_GET_HEADER_RSP, 
                ((MmsGetMsgHeaderReply*)p)->result, 
                ((MmsGetMsgHeaderReply*)p)->header);
            /* Prevent the destruct function from releasing the header data */
            ((MmsGetMsgHeaderReply*)p)->header = NULL;
        }
        break;

    case MMS_SIG_GET_MSG_SKELETON_REPLY:
        if (MSA_GET_SKELETON_RSP_MR == ((MmsGetSkeletonReply*)p)->userData)
        {
            (void)MSA_SIGNAL_SENDTO_UP(MSA_MR_FSM, MSA_SIG_MR_GET_SKELETON_RSP, 
                ((MmsGetSkeletonReply*)p)->result, 
                ((MmsGetSkeletonReply*)p)->infoList);
            /* Prevent the destruct function from releasing the info list */
            ((MmsGetSkeletonReply*)p)->infoList = NULL;
        }
        else if (MSA_GET_SKELETON_RSP_MCR == ((MmsGetSkeletonReply*)p)->userData)
        {
            (void)MSA_SIGNAL_SENDTO_UP(MSA_MCR_FSM, MSA_SIG_MCR_GET_SKELETON_RSP, 
                ((MmsGetSkeletonReply*)p)->result, 
                ((MmsGetSkeletonReply*)p)->infoList);
            /* Prevent the destruct function from releasing the info list */
            ((MmsGetSkeletonReply*)p)->infoList = NULL;
        }
        break;

    case MMS_SIG_GET_BODY_PART_REPLY:
        /* Send the response to the correct FSM */
        if (MSA_GET_BP_RSP_DEST_MR == ((MmsGetBodyPartReply*)p)->userData)
        {
            (void)MSA_SIGNAL_SENDTO_UUP(MSA_MR_FSM, MSA_SIG_MR_GET_BODYPART_RSP, 
                ((MmsGetBodyPartReply*)p)->result, ((MmsGetBodyPartReply*)p)->bodyPartSize,
                ((MmsGetBodyPartReply*)p)->pipeName);
            /* Prevent the destruct function from releasing the pipe-name */
            ((MmsGetBodyPartReply*)p)->pipeName = NULL;
        }
        else if (MSA_GET_BP_RSP_DEST_CRH == ((MmsGetBodyPartReply*)p)->userData)
        {
            msaCrhHandleGetBpRsp((MmsGetBodyPartReply*)p);
        }
        break;
    
    case MMS_SIG_PROGRESS_STATUS_REPLY:
        if (NULL != p)
        {
            msgStatus = MSA_ALLOC(sizeof(MmsProgressStatus));
            memcpy(msgStatus, (MmsProgressStatus*)p, 
                sizeof(MmsProgressStatus));

            (void)MSA_SIGNAL_SENDTO_P(MSA_COMM_FSM, 
                MSA_SIG_COMM_PROGRESS_STATUS, msgStatus);
        }
        break;
    case MMS_SIG_GET_MSG_INFO_REPLY:
        msaSrhGetMessageInfoRsp((MmsListOfProperties*)p);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA,
            "(%s) (%d) invalid signal mmsServiceSignalHandler\n", __FILE__, 
            __LINE__));
        break;
    }
    /* Delete signal */
    WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p);
}

/*!
 * \brief Handler/converter for signals received from the SMIL player.
 *
 * \param signal The external signal received that we should convert into an
 *           internal representation.
 * \param p The serialized signal data.
 *****************************************************************************/
static void siaSignalHandler(WE_UINT16 signal, void *p)
{
    switch(signal)
    {
    case SIA_SIG_PLAY_SMIL_RSP:
        /* Finished playing SMIL. Close the SIA module */
        (void)MSA_SIGNAL_SENDTO_U(MSA_CORE_FSM, MSA_SIG_CORE_SIA_STOP, 
            ((SiaPlaySmilRsp *)p)->result);
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) Unhandled signal from the SIA module!\n", __FILE__, 
            __LINE__));
        break;
    }
    WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p); 
}

/*!
 * \brief Handler/converter for signals received from the SIS. 
 * \param signal The external signal received that we should convert into an 
 *           internal representation
 * \param p The serialized signal data
 *****************************************************************************/
static void sisSignalHandler(WE_UINT16 signal, void* p)
{
    SisCloseSmilRsp     *closeSmilRsp;
    SisGetSlideInfoRsp  *slideInfoRsp;
    SisOpenSmilRsp      *openSmilRsp;

    switch(signal)
    {
    case SIS_OPEN_SMIL_RSP:
        openSmilRsp = (SisOpenSmilRsp *)p;
        /* Forward the signal to the MRC FSM */
        (void)MSA_SIGNAL_SENDTO_P(MSA_MR_FSM, MSA_SIG_MR_OPEN_SMIL_RSP, 
            openSmilRsp);
        /* Prevent deallocation of the  signal data */
        p = NULL;
        break;

    case SIS_GET_SLIDE_INFO_RSP:
        slideInfoRsp = (SisGetSlideInfoRsp *)p;
        (void)MSA_SIGNAL_SENDTO_P(MSA_MR_FSM, MSA_SIG_MR_GET_SLIDE_INFO_RSP,
            slideInfoRsp);
        /* Prevent deallocation of the  signal data */
        p = NULL;
        break;

    case SIS_CLOSE_SMIL_RSP:
        closeSmilRsp = (SisCloseSmilRsp *)p;
        /* Forward the signal to the MRC FSM */
        (void)MSA_SIGNAL_SENDTO_UU(MSA_MR_FSM, MSA_SIG_MR_CLOSE_SMIL_RSP, 
            closeSmilRsp->result, closeSmilRsp->userData);
        break;
        
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unhandled signal from SIS!\n", __FILE__, __LINE__));
        break;
    }
    /* Delete the external signal */
    WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p);
}

/*!
 * \brief Handler/converter for signals received from the Provisioning Service
 *
 * \param signal The external signal received that we should convert into an
 *           internal representation.
 * \param p The serialized signal data.
 *****************************************************************************/
static void prsSignalHandler(WE_UINT16 signal, void *p)
{
    switch(signal)
    {
    case PRS_SIG_PROVISIONING_OUTPUT:
        if (NULL != p)
        {
            msaSaveProvisioningData((we_registry_response_t *)p);
        } /* if */
        WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p); 
        MSA_TERMINATE;
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) Unhandled signal from the PRS module!\n", 
            __FILE__, __LINE__));
        break;
    }
    WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p); 
}

/*!
 * \brief Router for external signals received. Routes them to the appropriate 
 *          signal handler/converter. 
 * \param src_module The internal destination the signal is intended for
 * \param signal The external signal received that we should route to its 
 *                  destination "within" MSA
 * \param p The serialized signal data
 *****************************************************************************/
void msaSignalReceive(WE_UINT8 src_module, WE_UINT16 signal, void* p)
{
    /* Check if the package handler consumes this signal */
    if (msaPhHandleSignal(signal, p) || msaCrhHandleSignal(signal, p) ||
        msaDiaHandleSignal(signal, p))
    {
        WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p);
        return;
    }
    /* Dispatch signals to the correct module */
    switch(src_module)
    {
    case WE_MODID_PRS:
        prsSignalHandler(signal, p);
        break;

    case WE_MODID_SIA:
        siaSignalHandler(signal, p);
        break;

    case WE_MODID_FRW:
        weSignalHandler(signal, p);
        break;
   
    case WE_MODID_MMS:
        mmsSignalHandler(signal, p);
        break;

    case WE_MODID_SIS:
        sisSignalHandler(signal, p);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA,
            "(%s) (%d) Unhandled signal recieved in msaSignalReceive!\n", 
            __FILE__, __LINE__));
        WE_SIGNAL_DESTRUCT(WE_MODID_MSA, signal, p);
        break;
    }
}


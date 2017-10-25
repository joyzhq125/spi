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

/* \file madel.c
 * \brief  Handling of message deletion.
 */

/* WE */
#include "We_Int.h"
#include "We_Core.h"
#include "We_Cfg.h"
#include "We_Log.h"
#include "We_Lib.h"

/* MMS Service */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Uidia.h"
#include "Msa_Core.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaDelSignals 
 *  Signals used to communicate with DEL logic */
typedef enum
{
    /*! The response from the MMS Service when deleting a message
     *
     */
    MSA_SIG_DEL_DELETE_MSG_RSP,

    /*! Delete a message.
     * 
     * u_param1 = MSA_CONFIRM_DIALOG_OK or MSA_CONFIRM_DIALOG_CANCEL
     * u_param2 = msgId
     */
    MSA_SIG_DEL_DELETE_MSG

}MsaDelSignals;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void msaDelSignalHandler(MsaSignal *sig);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Deletes a message with an optional confirm.
 *
 * \param msgId The WID of the message to delete
 * \param confirm TRUE if the deletion should be confirmed, otherwise FALSE.
 *****************************************************************************/
void msaDeleteMsg(MmsMsgId msgId, WE_BOOL confirm)
{
    if (msgId == 0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "%s(%d): Removal of non-existent message!\n", __FILE__, __LINE__));
    }
    else if (confirm)
    {
        /* Ask before deletion */
        (void)msaShowConfirmDialog(MSA_STR_ID_DELETE_MSG_QUEST, NULL, 
            MSA_DEL_FSM, MSA_SIG_DEL_DELETE_MSG, msgId);
    }
    else
    {
        /* Delete message without asking */
        MMSif_deleteMessage(WE_MODID_MSA, msgId);
    }
}

/*!
 * \brief Initiates the delete msg FSM
 *
 *****************************************************************************/
void msaDelInit(void)
{
    msaSignalRegisterDst(MSA_DEL_FSM, msaDelSignalHandler);
}

/*!
 * \brief Terminates the delete msg FSM
 *
 *****************************************************************************/
void msaDelTerminate(void)
{
    msaSignalDeregister(MSA_DEL_FSM);
}

/*! \brief Handles the delete response from the MMS Service.
 *	
 *****************************************************************************/
void msaDelHandleDeleteRsp(MmsResult result)
{
    (void)MSA_SIGNAL_SENDTO_U(MSA_DEL_FSM, MSA_SIG_DEL_DELETE_MSG_RSP, result);
}

/*!
 * \brief Signal handler for the delete msg FSM.
 *
 * \param sig The signal received.
 *****************************************************************************/
static void msaDelSignalHandler(MsaSignal *sig)
{
    switch(sig->type)
    {
    case MSA_SIG_DEL_DELETE_MSG_RSP:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "DEL: MSA_SIG_DEL_DELETE_MSG_RSP received.\n"));
        if (MSA_STARTUP_MODE_DELETE == msaGetStartupMode())
        {
            /* The MSA is started in delete mode, stop the application after
               the delete operation*/
            (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
        }
        break;
        
    case MSA_SIG_DEL_DELETE_MSG:
        if ((MsaConfirmDialogResult)sig->u_param1 == MSA_CONFIRM_DIALOG_OK)
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Trying to delete msgId=%d\n", __FILE__, __LINE__, 
                sig->u_param2));
            MMSif_deleteMessage(WE_MODID_MSA, (MmsMsgId)sig->u_param2);
        }
        else
        {
            /* Close the application if delete mode is active*/
            if (MSA_STARTUP_MODE_DELETE == msaGetStartupMode())
            {
                (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, 
                    MSA_SIG_CORE_TERMINATE_APP);
            }
        }
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d)msaCoreSignalHandler(%d): unknown signal\n", __FILE__, 
            __LINE__, sig->type));
        break;
    }
    msaSignalDelete(sig);
}

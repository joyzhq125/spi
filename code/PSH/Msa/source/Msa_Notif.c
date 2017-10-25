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

/*!\file manotif.c
 * \brief Reads a MMS notification. 
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Def.h"
#include "Mmsl_if.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Notif.h"
#include "Msa_Mem.h"
#include "Msa_Async.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*! \brief Retrieves the notification information
 *
 * \param inst The current instance.
 * \return The result of the operation, see #MsaReadNotifResult.
 *****************************************************************************/
MsaReadNotifResult msaGetNotifFSM(MsaGetNotifInstance *inst)
{
    int fileHandle;
    if (NULL == inst)
    {
        return MSA_READ_NOTIF_ERROR;
    }
   
    switch (inst->state) 
    {
    case MSA_MAL_OPEN_NOTIF: 
        inst->mmsRes = MMSlib_messageOpen(WE_MODID_MSA, inst->msgId, 
            inst->msgType, &inst->msgHandle);
        if (MMS_RESULT_OK != inst->mmsRes)
        {
            return MSA_READ_NOTIF_ERROR;
        }
        inst->mmsRes = MMSlib_transactionOpen(WE_MODID_MSA, &inst->tr);
        if (MMS_RESULT_OK != inst->mmsRes)
        {
            return MSA_READ_NOTIF_ERROR;
        }
        inst->state = MSA_MAL_READ_NOTIF;
        /*lint -fallthrough */
        
    case MSA_MAL_READ_NOTIF:
        /* Read the notification */
        inst->mmsRes = MMSlib_getNotification(WE_MODID_MSA, inst->notif, 
            &inst->msgHandle, &inst->tr);
        if (MMS_RESULT_DELAYED == inst->mmsRes)
        {
            fileHandle = MMSlib_getFileHandle(WE_MODID_MSA, &inst->tr);
            Msa_AsyncRegister(fileHandle, inst->notifyFsm, inst->notifySignal, 
                0);            
            return MSA_READ_NOTIF_DELAYED; /* Async! */
        }
        else if (MMS_RESULT_OK != inst->mmsRes)
        {
            return MSA_READ_NOTIF_ERROR;
        } 
        (void)MMSlib_transactionClose(WE_MODID_MSA, &inst->tr);
        (void)MMSlib_messageClose(WE_MODID_MSA, &inst->msgHandle);
        return MSA_READ_NOTIF_DONE;
    default:
        break;
    }
    return MSA_READ_NOTIF_ERROR;
}

/*! \brief Creates a instance for handling a notification.
 *
 * \param inst The new instance.
 * \param msgId The msgId of the notification to open.
 * \param fsm The callback FSM for async operations.
 * \param signal The callback signal for async operations.
 * \return 
 *****************************************************************************/
WE_BOOL msaCreateGetNotifInstance(MsaGetNotifInstance **inst, MmsMsgId msgId,
    MmsFileType msgType, MsaStateMachine fsm, int signal)
{
    MsaGetNotifInstance *getNotifInstance;
    if (NULL != *inst)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) instance busy\n", __FILE__, __LINE__));
        return FALSE;
    }
    else
    {
        getNotifInstance = MSA_ALLOC(sizeof(MsaGetNotifInstance));
        *inst = getNotifInstance;
        memset(getNotifInstance, 0, sizeof(MsaGetNotifInstance));
        getNotifInstance->notif = MSA_ALLOC(sizeof(MmsNotification));
        memset(getNotifInstance->notif, 0, sizeof(MmsNotification));
        /* Initial state */
        getNotifInstance->msgId         = msgId;
        getNotifInstance->msgType       = msgType;
        getNotifInstance->state         = MSA_MAL_OPEN_NOTIF;
        getNotifInstance->notifyFsm     = fsm;
        getNotifInstance->notifySignal  = signal;
    }
    return TRUE;
}

/*! \brief De-allocates the structure for handling notifications.
 *
 * \param inst The current instance.
 *****************************************************************************/
void msaCleanupGetNotifInstance(MsaGetNotifInstance **inst)
{
    MsaGetNotifInstance *getNotifInstance;

    if (NULL != *inst)
    {
        getNotifInstance = *inst;
        if (NULL != getNotifInstance->notif)
        {
            MMSlib_freeMmsNotification(WE_MODID_MSA, getNotifInstance->notif);
            MSA_FREE(getNotifInstance->notif);
            getNotifInstance->notif = NULL;
        }

        /* Close message if previously opened */
        if (NULL != getNotifInstance->msgHandle)
        {
            (void)MMSlib_messageClose(WE_MODID_MSA, 
                &getNotifInstance->msgHandle);
            getNotifInstance->msgHandle = NULL;
        }
        /* Close transaction if previously created */
        if (NULL != getNotifInstance->tr)
        {
            (void)MMSlib_transactionClose(WE_MODID_MSA, 
                &getNotifInstance->tr);
            getNotifInstance->tr = NULL;
        }
        MSA_FREE(getNotifInstance);
        getNotifInstance = NULL;
        *inst = NULL;
    }
}

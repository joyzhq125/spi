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

/*! \file masrh.c
 * !\brief Handles the signal routing case where one interface function can be 
 *         called from multiple FSM's, .i.e., when the response signal needs to 
 *         be routed to more than one destination.
 */

/* WE */
#include "We_Core.h"
#include "We_Log.h"
#include "We_Cfg.h"
#include "We_Lib.h"
#include "We_Mem.h"
#include "We_Tel.h"

#include "We_Wid.h"

/* MMS Service */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* MSA*/
#include "Msa_Rc.h"
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Core.h"
#include "Msa_Uicmn.h"
#include "Msa_Uidia.h"


/******************************************************************************
 * Constants
 *****************************************************************************/
#define MSA_SRH_ITEM_COUNT   3   /*!< Concurrent routing operations */

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \enum MsaSrhOp
 *  Contains the different types of signal routing this module can handle
 */
typedef enum
{
    MSA_SRH_GET_MSG_INFO,   /*!< Get message info operation */
    MSA_SRH_MAKE_CALL       /*!< Make call operation */
}MsaSrhOp;

/*! \struct MsaSrhItem
 *  The information kept for every ongoing operation
 */
typedef struct 
{
    MsaStateMachine         fsm;            /*!< The callback FSM */
    int                     signal;         /*!< The callback signal */
    MsaSrhOp                operation;      /*!< The current operation type */
    WE_UINT16              wid;             /*!< Current operation wid-number */
}MsaSrhItem;

/******************************************************************************
 * Static variables
 *****************************************************************************/
static MsaSrhItem   *srhItems[MSA_SRH_ITEM_COUNT];  /* Slots for open operations */

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static WE_UINT16 srhAddItem(MsaStateMachine fsm, int signal, MsaSrhOp op);
static void srhDeleteItem(int index);
static MsaSrhItem *srhFindItem(MsaSrhOp op, int wid);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*! \brief Initiates the signal routing handler
 *
 *****************************************************************************/
void msaSrhInit(void)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) Initiating the signal router.\n", __FILE__, __LINE__));
    memset(srhItems, 0, sizeof(MsaSrhItem*)*MSA_SRH_ITEM_COUNT);
} /* msaSrhInit */

/*! \brief Terminates the signal routing handler.
 *
 *****************************************************************************/
void msaSrhTerminate(void)
{
    int i;
    for (i=0; i < MSA_SRH_ITEM_COUNT; i++)
    {
        /* Delete all open items */
        srhDeleteItem(i);
    }
    memset(srhItems, 0, sizeof(MsaSrhItem*)*MSA_SRH_ITEM_COUNT);
} /* msaSrhTerminate */

/*!\brief Adds an item to the list of operations
 *
 * \param fsm The callback FSM.
 * \param signal The callback signal.
 * \param op The current operation.
 *****************************************************************************/
static WE_UINT16 srhAddItem(MsaStateMachine fsm, int signal, MsaSrhOp op)
{
    int i;
    for (i = 0; i < MSA_SRH_ITEM_COUNT; i++)
    {
        /* Find an empty slot */
        if (NULL == srhItems[i])
        {
            /* Create a new item */
            MSA_CALLOC(srhItems[i], sizeof(MsaSrhItem));
            srhItems[i]->fsm        = fsm;
            srhItems[i]->signal     = signal;
            srhItems[i]->operation  = op;
            srhItems[i]->wid         = (WE_UINT16)(((unsigned int)fsm) | 
                ((unsigned int)signal << 8));
            return srhItems[i]->wid;
        }
    }
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) the number of concurrent signal routing operations are to"
        "low!\n", __FILE__, __LINE__));
    MSA_TERMINATE;
    return 0;
} /* srhAddItem */

/*! \brief Deletes a signal routing item from a specific "slot"
 *
 * \param index The index to delete starting from 0.
 *****************************************************************************/
static void srhDeleteItem(int index)
{
    if (NULL != srhItems[index])
    {
        MSA_FREE(srhItems[index]);
        srhItems[index] = NULL;
    }
} /* srhDeleteItem */

/*! \brief Extracts an operation from the list of ongoing operations.
 *
 * \param op The operation to find.
 * \param wid The wid of the operation to find or zero if irrelevant
 * \return The current operation, the caller is responsible for de-allocating 
 *         the data.
 *****************************************************************************/
static MsaSrhItem *srhFindItem(MsaSrhOp op, int wid)
{
    int i;
    MsaSrhItem *ret;
    for (i = 0; i < MSA_SRH_ITEM_COUNT; i++)
    {
        /* Find matching slot, only match on wid if non-zero */
        if ((NULL != srhItems[i]) && (op == srhItems[i]->operation) &&
            ((0 == wid) || (wid == srhItems[i]->wid)))
        {
            ret = srhItems[i];
            srhItems[i] = NULL; /* Assume caller deallocates this entry */
            return ret;
        }
    }
    return NULL;
} /* srhFindItem */

/*! \brief Calls the MMS interface function #MMSif_getMessageInfo
 *
 * \param fsm The callback FSM.
 * \param signal The callback signal.
 * \param msgId The message identity of the message to get information about.
 *****************************************************************************/
void msaSrhGetMessageInfo(MsaStateMachine fsm, int signal, MmsMsgId msgId)
{
    (void)srhAddItem(fsm, signal, MSA_SRH_GET_MSG_INFO);
    /* Call the interface function */
    MMSif_getMessageInfo(WE_MODID_MSA, msgId);
} /* msaSrhGetMessageInfo */

/*! \brief Handles the response signal for the MMSif_getMessageInfo operation.
 *
 * \param list The response data.
 *****************************************************************************/
void msaSrhGetMessageInfoRsp(const MmsListOfProperties *list)
{
    MsaSrhItem *item;
    MmsMessageFolderInfo *fInfo = NULL;
    /* Get the callback data */
    item = srhFindItem(MSA_SRH_GET_MSG_INFO, 0);
    
    if (NULL != item)
    {
        if ((NULL != list) && (1 == list->noOfElements))
        {
            /* Copy the signal data */
            fInfo = MSA_ALLOC(sizeof(MmsMessageFolderInfo));
            memcpy(fInfo, &(list->msgPropertyList[0].msgFolderInfo), sizeof(MmsMessageFolderInfo));
        }
        /* Send the response */
        (void)MSA_SIGNAL_SENDTO_P(item->fsm, item->signal, fInfo);
        /*lint -e{774} */
        MSA_FREE(item);
    }
}

/*!
 * \brief Calls the WE Telephone API function WE_TEL_MAKE_CALL. 
 *        Result is passed as TRUE/FALSE in u_param1 using the requested 
 *        return signal. Multiple calls using the same callback FSM/signal 
 *        combination is not possible!
 * 
 * \param fsm       The callback FSM.
 * \param signal    The callback signal.
 * \param number    The number to call
 * \param wid        The wid of this call operation
 *****************************************************************************/
void msaSrhMakeCall(MsaStateMachine fsm, int signal, const char *number)
{
    WE_TEL_MAKE_CALL((WE_UINT8)WE_MODID_MSA, 
        srhAddItem(fsm, signal, MSA_SRH_MAKE_CALL), number);
} /* msaSrhMakeCall */

/*!
 * \brief Handles the response signal for the WE_TEL_MAKE_CALL operation
 * 
 * \param callRsp   The response data
 *****************************************************************************/
void msaSrhMakeCallRsp(const we_tel_make_call_resp_t *callRsp)
{
    MsaSrhItem *item;
    WE_BOOL ret;

    /* Get the callback data */
    item = srhFindItem(MSA_SRH_MAKE_CALL, 
        (NULL != callRsp) ? callRsp->telId : 0);

    /* Analyze the callback data */
    ret = (NULL != item) && (NULL != callRsp) && 
        (WE_TEL_OK == callRsp->result);

    /* Send the response */
    if (NULL != item)
    {
        (void)MSA_SIGNAL_SENDTO_U(item->fsm, item->signal, (unsigned)ret);
    }

    /* Show dialogs on error */
    if ((FALSE == ret) && (NULL != callRsp))
    {
        switch (callRsp->result)
        {
        case WE_TEL_ERROR_CALLED_PARTY_IS_BUSY:
            /* The called party is busy*/
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_CALL_BUSY), 
                MSA_DIALOG_ERROR);    
            break;
        case WE_TEL_ERROR_NETWORK_NOT_AVAILABLE:
            /*The network is not available.*/
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_CALL_NO_NETWORK), 
                MSA_DIALOG_ERROR);    
            break;
        case WE_TEL_ERROR_CALLED_PARTY_NO_ANSWER:
            /*The called party did not answer.*/
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_CALL_NO_ANSWER), 
                MSA_DIALOG_ERROR);    
            break;
        case WE_TEL_ERROR_INVALID:
            /* The number parameter is not a phone-number */
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_CALL_NOT_A_NUMBER), 
                MSA_DIALOG_ERROR);    
            break;

        case WE_TEL_ERROR_UNSPECIFIED:
        default:
            /* Unspecified error.*/
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_CALL_ERROR), 
                MSA_DIALOG_ERROR);    
            break;
        } /* switch */
    }
    else if (NULL == callRsp)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_CALL_ERROR), 
            MSA_DIALOG_ERROR);    
    } /* if */

    MSA_FREE(item);
} /* msaSrhMakeCallRsp */

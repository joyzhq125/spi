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

/* \file macrh.c
 * \brief Content routing handler.
 */

/* WE */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Int.h"
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Pipe.h"
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Pck.h"
#include "We_Act.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* MSA */
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Crh.h"
#include "Msa_Mem.h"
#include "Msa_Pckg.h"
#include "Msa_Uicmn.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaCrhSignalId
 *  Signal destinations.*/
typedef enum
{
    /*! Response from the get body-part operation.
     *
     */
    MSA_SIG_CRH_GET_BODY_PART_RSP,

    /*! Handles the response from the package handler.
     *	
     */
    MSA_SIG_CRH_HANDLE_PCK_RSP,

    /* Handles the response from MMS get body-part
     *	
     */
    MSA_SIG_CRH_HANDLE_ACT_SEND_RSP

}MsaCrhSignalId;

/*! \brief MsaCrhOp
 *	
 */
typedef enum
{
    MSA_CRH_OP_GET_ACTIONS = 0x01,  /*!< Retrieve the list of actions for a 
                                         specific object */
    MSA_CRH_OP_SEND_CONTENT         /*!< Send the content to another module */
}MsaCrhOp;

/* Action-package callbacks
 *	
 */
static const msa_pckg_fnc_list_t funcList = {
        we_act_init, 
        we_act_handle_signal,
        we_act_get_private_data, 
        we_act_set_private_data,
        we_act_terminate, 
        we_act_delete,
        we_act_get_result,
        we_act_result_free
};

/* A list of actions to exclude */
static const char *excludeActions[] = MSA_CFG_EXCLUDE_ACTIONS;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct CrhRequest
 *  Content routing request data */
typedef struct
{
    MsaCrhOp                state;

    msa_pkg_id_t            actPckId;       /* !< Dialog package WID */
    msa_pkg_handle_t        *actHandle;     /* !< Dialog instance WID */
    WE_INT32               operId;         /* !< The wid of the current op */

    we_pck_attr_list_t     *fileAttributes;/* !< File attributes for the 
                                                  content to send */
    char                    *mimeType;      /* !< The mime-type of the content 
                                                  to send*/
    char                    *fileName;      /* !< The name of the file to send */

    MmsMsgId                msgId;          /* !< The message containing the data */
    WE_UINT32              bodyPartIndex;  /* !< The body-part index of the 
                                                  content to send */

    MsaStateMachine         fsm;            /* !< Callback state machine */
    int                     signal;         /* !< Callback signal */

    char                    *cmd;           /* !< Action command */
    we_act_action_entry_t  *actions;       /* !< List of possible actions for 
                                                  the content */
    WE_UINT32              actionCount;    /* !< Total number of actions */
    WE_UINT32              actionId;       /* !< */

    char                    *pipeName;      /* !< The name of the pipe to send
                                                  data through */
}CrhInstance;

/******************************************************************************
 * Static variables
 *****************************************************************************/
static CrhInstance          *crhInstance;
static msa_pckg_handle_t    *pckHandle;     /* Package encapsulation handle */

/******************************************************************************
 * Function prototypes
 *****************************************************************************/
static void  crhMain(MsaSignal *sig);
static int   crhCreateInstance(CrhInstance **inst, MsaStateMachine fsm, 
    int signal);
static void  crhCleanupInstance(CrhInstance **inst, MsaCrhResult result);
static WE_BOOL handleBodyPartRsp(const CrhInstance *inst);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*! \brief Copy a list of actions.
 *
 * \param actionList
 *****************************************************************************/
we_act_action_entry_t *msaCopyActionList(
    const we_act_action_entry_t *actionList, WE_UINT32 count)
{
    we_act_action_entry_t *cpy;
    unsigned int i;

    if (NULL == actionList)
    {
        return NULL;
    }
    MSA_CALLOC(cpy, sizeof(we_act_action_entry_t)*count);
    for(i = 0; i < count; i++)
    {
        cpy[i].action_id = actionList[i].action_id;
        cpy[i].string_id = actionList[i].string_id;
        cpy[i].act_cmd = we_cmmn_strdup(WE_MODID_MSA, actionList[i].act_cmd);
    }
    return cpy;
}

/*! \brief Deallocates a list of actions.
 *
 * \param actionList The list of actions.
 * \param count The number of actions in the list
 *****************************************************************************/
void msaFreeActionList(we_act_action_entry_t *actionList, WE_UINT32 count)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        MSA_FREE(actionList[i].act_cmd);
    }
    MSA_FREE(actionList);
}

/*! \brief MSA package operation completion entry point function.
 *
 * \param fsm      Fsm to forward signal to.
 * \param signal   Signal to send to fsm.
 * \param pkg_inst Package instance to completed operation.
 * \param wid       Package operation WID.
 * \param result   Package operation result (type-cast per operation)
 * \param data     Private data provided at registration of operation.
 *
 * \return 0.
 *
 * NOTE: The result is deallocated when this function returns.
 *       If result data is to be kept, either copy or snatch pointer
 *       and set it to NULL (so that the free operation not can free it). 
 *****************************************************************************/
static int pckSigHandler(WE_INT32 fsm, WE_INT32 signal,
    msa_pkg_id_t pckg_id, msa_pkg_handle_t *pkg_inst, WE_INT32 wid, 
    msa_pckg_result_t *result, void *data)
{
    we_act_result_get_actions_t *getActionsResult;
    we_act_action_entry_t       *actionList = NULL;
    we_act_error_result_object_action_t *objRes = NULL;
    WE_INT32 actRes;

    (void)data;
    (void)wid;
    (void)pkg_inst;

    if (crhInstance->actPckId == pckg_id)
    {
        if (crhInstance->state == MSA_CRH_OP_GET_ACTIONS)
        {
            getActionsResult = (we_act_result_get_actions_t *)
                msa_pckg_get_result_data(result);

            /* Copy the result */
            if (getActionsResult->result == WE_ACT_OK)
            {
                actionList = msaCopyActionList(getActionsResult->action_list, 
                    (WE_UINT32)getActionsResult->num_actions);

                /* Send a signal in order to avoid recursive calls to get result when
                   syncron operation is used */
                crhInstance->actionCount    = (WE_UINT32)getActionsResult->num_actions;
                crhInstance->actions        = actionList;
            }
            /* Send the response, use a signal in order to avoid 
               semi-recursive calls  */
            (void)MSA_SIGNAL_SENDTO_U((MsaStateMachine)fsm, signal, 
                (getActionsResult->result == WE_ACT_OK) ? MSA_CRH_OK: MSA_CRH_ERROR);
            return TRUE;
        }
        else if (crhInstance->state == MSA_CRH_OP_SEND_CONTENT)
        {
            actRes = msa_pckg_get_result_int(result);
            objRes = (we_act_error_result_object_action_t *)
                msa_pckg_get_result_additional_data(result);
            /* Send the response, use a signal in order to avoid 
               semi-recursive calls  */
            if (WE_ACT_CANCELLED == actRes)
            {
                (void)MSA_SIGNAL_SENDTO_UU((MsaStateMachine)fsm, signal, TRUE,
                    (unsigned)objRes->caller_owns_pipe);
            }
            else
            {
                (void)MSA_SIGNAL_SENDTO_UU((MsaStateMachine)fsm, signal, 
                    TRUE, FALSE);
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 *\brief Initiates the content routing handler
 *
 *****************************************************************************/
WE_BOOL msaCrhInit(void)
{
    msaSignalRegisterDst(MSA_CRH_FSM, crhMain);
    /* Initiate encapsulation */
    if (NULL == (pckHandle = msa_pckg_create(WE_MODID_MSA)))
    {
        return FALSE;
    }
    /* Sets callback function */
    if (!msa_pckg_set_response_handler(pckHandle, pckSigHandler))
    {
        return FALSE;
    }
    crhInstance = NULL;
    return TRUE;
}

/*!
 *\brief Terminates the content routing handler
 *
 *****************************************************************************/
void msaCrhTerminate(void)
{
    /* Remove any instance data */
    crhCleanupInstance(&crhInstance, MSA_CRH_INTERNAL_TERMINATION);
    /* De-allocate package handler */
    if (0 != pckHandle)
    {
        msa_pckg_destroy(pckHandle);
        pckHandle = 0;
    }
    msaSignalDeregister(MSA_CRH_FSM);
}

/*!
 *\brief Signal handler for the content routing FSM.
 *
 * \param sig The signal received.
 *****************************************************************************/
static void crhMain(MsaSignal *sig)
{
    /* Signal handler */
    switch ((MsaCrhSignalId)sig->type)
    {
    case MSA_SIG_CRH_HANDLE_PCK_RSP:
        if (MSA_CRH_OK == sig->u_param1)
        {
            /* Result data*/
            crhCleanupInstance(&crhInstance, MSA_CRH_OK);
        }
        else
        {
            crhInstance->actions        = NULL;
            crhInstance->actionCount    = 0;
            /* The operation failed, clean-up and report */
            crhCleanupInstance(&crhInstance, MSA_CRH_ERROR);
        }

        break;
        
    case MSA_SIG_CRH_GET_BODY_PART_RSP:
        if (!handleBodyPartRsp(crhInstance))
        {
            crhCleanupInstance(&crhInstance, MSA_CRH_ERROR);
        }
        break;

    case MSA_SIG_CRH_HANDLE_ACT_SEND_RSP:
        /* Check if the pipe-name can be deleted */
        if (!sig->u_param2)
        {
            /* Crh does not need to delete the pipe, de-allocate the 
               pipe-name */
            MSA_FREE(crhInstance->pipeName);
            crhInstance->pipeName = NULL;
        }
        /* Send the response */
        crhCleanupInstance(&crhInstance, (sig->u_param1) ? MSA_CRH_OK: 
            MSA_CRH_ERROR);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) unhandled signal\n", __FILE__, __LINE__));
        break;            
    }
    /* Deallocate the signal */
    msaSignalDelete(sig);
}

/*!
 * \brief Initiates data-structures for a content routing session
 *
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL crhCreateInstance(CrhInstance **inst, MsaStateMachine fsm, 
    int signal)
{
    /* Check that no operation is in progress */
    if (NULL != *inst)
    {
        return FALSE;
    }
    /* Allocate memory */
    MSA_CALLOC(*inst, sizeof(CrhInstance));
    /* Setup initial data */
    (*inst)->fsm    = fsm;
    (*inst)->signal = signal;

    /* Init packages */
    if (0 == (crhInstance->actPckId = msa_pckg_reg_pckg(pckHandle, &funcList)))
    {
        return FALSE;
    }
    /* Register package */
    if (NULL == (crhInstance->actHandle = msa_pckg_create_inst(pckHandle, 
        crhInstance->actPckId)))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Cleanup of the current content routing request.
 *
 * \param result The result that is sent back to the originating fsm, if
 *               the result is MSA_CRH_TERMINATE no signal is sent back.
 *****************************************************************************/
static void crhCleanupInstance(CrhInstance **inst, MsaCrhResult result)
{
    if (NULL == *inst)
    {
        return;
    }
    else
    {
        if (MSA_CRH_INTERNAL_TERMINATION != result)
        {
            if ((*inst)->state == MSA_CRH_OP_GET_ACTIONS)
            {

                (void)MSA_SIGNAL_SENDTO_UUP((MsaStateMachine)crhInstance->fsm, 
                    crhInstance->signal, result, crhInstance->actionCount, 
                    crhInstance->actions);
                /* The data is now owned by the */
                crhInstance->actions     = NULL;
                crhInstance->actionCount = 0;
            }
            else
            {
                (void)MSA_SIGNAL_SENDTO_U((MsaStateMachine)crhInstance->fsm, 
                    crhInstance->signal, result);
            }
        }
        /* MIME-type */
        if (NULL != (*inst)->mimeType)
        {
            MSA_FREE((*inst)->mimeType);
            (*inst)->mimeType = NULL;
        }
        /* File-name */
        if (NULL != (*inst)->fileName)
        {
            MSA_FREE((*inst)->fileName);
            (*inst)->fileName = NULL;
        }
        if (crhInstance->actions != NULL)
        {
            msaFreeActionList(crhInstance->actions, crhInstance->actionCount);
        }
       
 

        /* Unreg action package */
        if (0 != (*inst)->actHandle)
        {
            (void)msa_pckg_terminate(pckHandle, (*inst)->actHandle, FALSE);
            (*inst)->actHandle = 0;
        }

        /* Unreg current op */
        if (0 != (*inst)->actPckId)
        {
            (void)msa_pckg_unreg_pckg(pckHandle, (*inst)->actPckId);
        }
        
        if (NULL != (*inst)->fileAttributes)
        {
            we_pck_attr_free(WE_MODID_MSA, (*inst)->fileAttributes);
        }
        /* Handle the pipe-operation */
        if (NULL != (*inst)->pipeName)
        {
            (void)WE_PIPE_DELETE((*inst)->pipeName);
            MSA_FREE((*inst)->pipeName);
            (*inst)->pipeName = NULL;
        }
        /* Action list */
        if (NULL != (*inst)->actions)
        {
            MSA_FREE((*inst)->actions);
        }

        /* Action command */
        MSA_FREE((*inst)->cmd);
        MSA_FREE((*inst));
        (*inst) = NULL;
    }
}

/*! \brief Tells if any actions can be performed on a certain object-type.
 *
 * \param contentType The content-type of the current object.
 * \return TRUE if actions can be performed on this type, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL preFilter(MmsContentType *contentType)
{
    MmsAllParams *params;

    params = contentType->params;
    while(params)
    {
        if (MMS_CHARSET == params->param)
        {
            if ((MMS_UTF8 == params->value.integer) || 
             (MMS_US_ASCII == params->value.integer))
            {
                /* Allow: the object have a supported charset */
                return TRUE;
            }
            else
            {
                /* Reject: the object have a unsupported charset */
                return FALSE;
            }
        }
        params = params->next;
    }
    /* The object do not define a charset, i.e., allow */
    return TRUE;
}

/*! \brief Retrieves a list of actions that are possible to execute on the 
 *         specified object.
 *
 * \param contentType The content-type of the message.
 * \param drmContentType The DRM content-type if the object is embedded.
 * \param fsm The callback fsm.
 * \param signal The callback signal.
 *****************************************************************************/
void msaCrhGetActions(MmsContentType *contentType, const char *drmContentType, 
    MsaStateMachine fsm, int signal)
{
    if (!crhCreateInstance(&crhInstance, fsm, signal))
    {
        /* Failed, cleanup and report */
        crhCleanupInstance(&crhInstance, MSA_CRH_ERROR);
        return ;
    }
    if (!preFilter(contentType))
    {
        /* Do not perform any actions on this object */
        crhCleanupInstance(&crhInstance, MSA_CRH_OK);
        return ;
    }
    crhInstance->state      = MSA_CRH_OP_GET_ACTIONS;
    /* Ask for the list of actions */
    if (0 == (crhInstance->operId = 
        we_act_get_actions(crhInstance->actHandle, (char *)contentType->strValue, 
        WeResourcePipe, drmContentType, TRUE, excludeActions, 
        sizeof(excludeActions)/sizeof(excludeActions[0]))))
    {
        crhCleanupInstance(&crhInstance, MSA_CRH_ERROR);
        return ;
    }
    /* Get the list of action, the result is handled in the */
    if (!msa_pckg_reg_operation(pckHandle, crhInstance->actHandle, 
        crhInstance->operId, MSA_CRH_FSM, MSA_SIG_CRH_HANDLE_PCK_RSP, NULL))
    {
        crhCleanupInstance(&crhInstance, MSA_CRH_ERROR);
        return ;
    }
}

/*! \brief Handles the body-part response signal from the MMS Service.
 *
 * \param inst The current instance.
 * \return TRUE on success, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL handleBodyPartRsp(const CrhInstance *inst)
{
    we_act_content_t content;
    const char *tmp[1];

    /* Perform the object action */
    memset(&content, 0, sizeof(content));
    content.data_type    = WeResourcePipe;
    content.src_path     = inst->pipeName;

    content.mime_type    = inst->mimeType;
    content.default_name = inst->fileName;

    tmp[0] = inst->cmd;
    if (0 == (crhInstance->operId = 
        we_act_object_action(inst->actHandle, (WE_INT32)inst->actionId, &content, 
            inst->fileAttributes, FALSE, msaGetScreenHandle(), tmp, 1)))
    {
        return FALSE;
    }

    /* Get the list of action, the result is handled in the */
    if (!msa_pckg_reg_operation(pckHandle, inst->actHandle, 
        crhInstance->operId, MSA_CRH_FSM, MSA_SIG_CRH_HANDLE_ACT_SEND_RSP, 
        NULL))
    {
        return FALSE;
    }
    return TRUE;
}

/*!\brief Send content to another module in the FRW.
 *
 * \param fileAttributes The file attributes.
 * \param fsm The callback FSM.
 * \param signal The callback signal.
 * \param cmd The command to execute.
 * \param msgId The message WID of the message containing the data to send.
 * \param bodyPartIndex The body-part index of the content 
 *****************************************************************************/
void msaCrhSendContent(we_pck_attr_list_t *fileAttributes, char *fileName,
    MsaStateMachine fsm, int signal, const we_act_action_entry_t *actionEntry, 
    MmsMsgId msgId, WE_UINT16 bodyPartIndex)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
        "(%s) (%d) Sending content to another module: cmd=%s\n", __FILE__, 
        __LINE__, actionEntry->act_cmd));
    /* Create the instance */
    if (!crhCreateInstance(&crhInstance, fsm, signal))
    {
        we_pck_attr_free(WE_MODID_MSA, fileAttributes);
        crhCleanupInstance(&crhInstance, MSA_CRH_ERROR);
        return ;
    }
    /* Copy instance data */
    crhInstance->fileAttributes = we_pck_attr_dupl(WE_MODID_MSA, 
        fileAttributes);
    crhInstance->msgId          = msgId;
    crhInstance->bodyPartIndex  = bodyPartIndex;
    crhInstance->actionId       = actionEntry->action_id;
    crhInstance->cmd            = we_cmmn_strdup(WE_MODID_MSA, actionEntry->act_cmd);
    crhInstance->state          = MSA_CRH_OP_SEND_CONTENT;
    crhInstance->fileName       = we_cmmn_strdup(WE_MODID_MSA, fileName);

    /* Copy the MIME-type for further use */
    while (fileAttributes != NULL)
    {
        if ((fileAttributes->attribute == WE_PCK_ATTRIBUTE_MIME) &&
            (WE_PCK_ATTR_TYPE_STRING == fileAttributes->type))
        {
            crhInstance->mimeType = we_cmmn_strdup(WE_MODID_MSA, 
                fileAttributes->_u.s);
        }
        fileAttributes = fileAttributes->next;
    }
        
    /* Get the current body-part */
    MMSif_getBodyPart(WE_MODID_MSA, msgId, bodyPartIndex, 
        MSA_GET_BP_RSP_DEST_CRH);
}

/*! \brief Calls package encapsulation signal handler.
 *
 * \param signal   External signal to handle.
 * \param p        Signal data.
 *
 * \return Returns TRUE if signal is consumed otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCrhHandleSignal(WE_UINT16 signal, void *p)
{
    int res;

    res = msa_pckg_handle_signal(pckHandle, signal, p);
    switch(res)
    {
    case WE_PACKAGE_OPERATION_COMPLETE:
    case WE_PACKAGE_SIGNAL_NOT_HANDLED:
        return FALSE;
    case WE_PACKAGE_SIGNAL_HANDLED:
        return TRUE;
    default:
        return FALSE;
    }
}

/*! \brief Handles the response signal from the current "get body-part" 
 *         operation.
 *
 * \param bodyPart The info about the current body-part, see 
 *                 #MmsReplyGetBodyPart.
 *****************************************************************************/
void msaCrhHandleGetBpRsp(const MmsGetBodyPartReply *bodyPart)
{
    /* Guard */
    if (crhInstance == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Missing CRH instance. Rejecting the 'get body-part "
            "response'\n", __FILE__, __LINE__));
        return;
    }
    /* Check that the body-part operation was successfully */
    if (MMS_RESULT_OK == bodyPart->result)
    {
        /* The pipe-name is all that is needed to continue */
        crhInstance->pipeName = we_cmmn_strdup(WE_MODID_MSA, 
            (char *)bodyPart->pipeName);
        /* Avoid locking execution during signal retrieving */
        (void)MSA_SIGNAL_SENDTO(MSA_CRH_FSM, MSA_SIG_CRH_GET_BODY_PART_RSP);
    }
    else
    {
        /* Operation failed, clean-up and send the response */
        crhCleanupInstance(&crhInstance, MSA_CRH_ERROR);
    }
}

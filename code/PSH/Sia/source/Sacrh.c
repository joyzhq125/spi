/*
 * Copyright (C) Techfaith, 2002-2005.
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

/* SIS */
#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* SMA */
#include "Sia_cfg.h"
#include "Sia_def.h"
#include "Satypes.h"
#include "Saintsig.h"
#include "Sacrh.h"
#include "Samem.h"
#include "Sapckg.h"
#include "Sauiform.h"
#include "Saui.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum SiaCrhSignalId
 *  Signal destinations.*/
typedef enum
{
    /*! Response from the get body-part operation.
     *
     */
    SIA_SIG_CRH_GET_BODY_PART_RSP,

    /*! Handles the response from the package handler.
     *	
     */
    SIA_SIG_CRH_HANDLE_PCK_RSP,

    /* Handles the response from MMS get body-part
     *	
     */
    SIA_SIG_CRH_HANDLE_ACT_SEND_RSP

}SiaCrhSignalId;

/*! \brief SiaCrhOp
 *	
 */
typedef enum
{
    SIA_CRH_OP_GET_ACTIONS = 0x01,  /*!< Retrieve the list of actions for a 
                                         specific object */
    SIA_CRH_OP_SEND_CONTENT         /*!< Send the content to another module */
}SiaCrhOp;

/* Action-package callbacks
 *	
 */
static const Sia_pckg_fnc_list_t funcList = {
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
static const char *excludeActions[] = SIA_CFG_EXCLUDE_ACTIONS;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct CrhRequest
 *  Content routing request data */
typedef struct
{
    SiaCrhOp                state;

    Sia_pkg_id_t            actPckId;       /* !< Dialog package ID */
    Sia_pkg_handle_t        *actHandle;     /* !< Dialog instance ID */
    WE_INT32               operId;         /* !< The id of the current op */

    we_pck_attr_list_t     *fileAttributes;/* !< File attributes for the 
                                                  content to send */
    char                    *mimeType;      /* !< The mime-type of the content 
                                                  to send*/
    char                    *fileName;      /* !< The name of the file to send */

    MmsMsgId                msgId;          /* !< The message containing the data */
    WE_UINT32              bodyPartIndex;  /* !< The body-part index of the 
                                                  content to send */

    SiaStateMachine         fsm;            /* !< Callback state machine */
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
static Sia_pckg_handle_t    *pckHandle;     /* Package encapsulation handle */

/******************************************************************************
 * Function prototypes
 *****************************************************************************/
static void  crhMain(SiaSignal *sig);
static int   crhCreateInstance(CrhInstance **inst, SiaStateMachine fsm, 
    int signal);
static void  crhCleanupInstance(CrhInstance **inst, SiaCrhResult result);
static WE_BOOL handleBodyPartRsp(const CrhInstance *inst);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*! \brief Copy a list of actions.
 *
 * \param actionList
 *****************************************************************************/
we_act_action_entry_t *smaCopyActionList(
    const we_act_action_entry_t *actionList, WE_UINT32 count)
{
    we_act_action_entry_t *cpy;
    unsigned int i;

    if (NULL == actionList)
    {
        return NULL;
    }
    cpy = SIA_CALLOC(sizeof(we_act_action_entry_t)*count);
    for(i = 0; i < count; i++)
    {
        cpy[i].action_id = actionList[i].action_id;
        cpy[i].string_id = actionList[i].string_id;
        cpy[i].act_cmd = we_cmmn_strdup(WE_MODID_SIA, actionList[i].act_cmd);
    }
    return cpy;
}

/*! \brief Deallocates a list of actions.
 *
 * \param actionList The list of actions.
 * \param count The number of actions in the list
 *****************************************************************************/
void smaFreeActionList(we_act_action_entry_t *actionList, WE_UINT32 count)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        SIA_FREE(actionList[i].act_cmd);
    }
    SIA_FREE(actionList);
}

/*! \brief SMA package operation completion entry point function.
 *
 * \param fsm      Fsm to forward signal to.
 * \param signal   Signal to send to fsm.
 * \param pkg_inst Package instance to completed operation.
 * \param id       Package operation ID.
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
    Sia_pkg_id_t pckg_id, Sia_pkg_handle_t *pkg_inst, WE_INT32 id, 
    Sia_pckg_result_t *result, void *data)
{
    we_act_result_get_actions_t *getActionsResult;
    we_act_action_entry_t       *actionList = NULL;
    we_act_error_result_object_action_t *objRes = NULL;
    WE_INT32 actRes;

    (void)data;
    (void)id;
    (void)pkg_inst;

    if (crhInstance->actPckId == pckg_id)
    {
        if (crhInstance->state == SIA_CRH_OP_GET_ACTIONS)
        {
            getActionsResult = (we_act_result_get_actions_t *)
                Sia_pckg_get_result_data(result);

            /* Copy the result */
            if (getActionsResult->result == WE_ACT_OK)
            {
                actionList = smaCopyActionList(getActionsResult->action_list, 
                    (WE_UINT32)getActionsResult->num_actions);

                /* Send a signal in order to avoid recursive calls to get result when
                   syncron operation is used */
                crhInstance->actionCount    = (WE_UINT32)getActionsResult->num_actions;
                crhInstance->actions        = actionList;
            }
            /* Send the response, use a signal in order to avoid 
               semi-recursive calls  */
            (void)SIA_SIGNAL_SENDTO_U((SiaStateMachine)fsm, signal, 
                (getActionsResult->result == WE_ACT_OK) ? SIA_CRH_OK: SIA_CRH_ERROR);
            return TRUE;
        }
        else if (crhInstance->state == SIA_CRH_OP_SEND_CONTENT)
        {
            actRes = Sia_pckg_get_result_int(result);
            objRes = (we_act_error_result_object_action_t *)
                Sia_pckg_get_result_additional_data(result);
            /* Send the response, use a signal in order to avoid 
               semi-recursive calls  */
            if (WE_ACT_CANCELLED == actRes)
            {
                (void)SIA_SIGNAL_SENDTO_UU((SiaStateMachine)fsm, signal, TRUE,
                    (unsigned)objRes->caller_owns_pipe);
            }
            else
            {
                (void)SIA_SIGNAL_SENDTO_UU((SiaStateMachine)fsm, signal, 
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
WE_BOOL smaCrhInit(void)
{
    smaSignalRegisterDst(SIA_FSM_CRH, crhMain);
    /* Initiate encapsulation */
    if (NULL == (pckHandle = Sia_pckg_create(WE_MODID_SIA)))
    {
        return FALSE;
    }
    /* Sets callback function */
    if (!Sia_pckg_set_response_handler(pckHandle, pckSigHandler))
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
void smaCrhTerminate(void)
{
    /* Remove any instance data */
    crhCleanupInstance(&crhInstance, SIA_CRH_INTERNAL_TERMINATION);
    if (0 != pckHandle)
    {
        Sia_pckg_destroy(pckHandle);
        pckHandle = 0;
    }
    smaSignalDeregister(SIA_FSM_CRH);
}

/*!
 *\brief Signal handler for the content routing FSM.
 *
 * \param sig The signal received.
 *****************************************************************************/
static void crhMain(SiaSignal *sig)
{
    /* Signal handler */
    switch ((SiaCrhSignalId)sig->type)
    {
    case SIA_SIG_CRH_HANDLE_PCK_RSP:
        if (SIA_CRH_OK == sig->u_param1)
        {
            /* Result data*/
            crhCleanupInstance(&crhInstance, SIA_CRH_OK);
        }
        else
        {
            crhInstance->actions        = NULL;
            crhInstance->actionCount    = 0;
            /* The operation failed, clean-up and report */
            crhCleanupInstance(&crhInstance, SIA_CRH_ERROR);
        }

        break;
        
    case SIA_SIG_CRH_GET_BODY_PART_RSP:
        if (!handleBodyPartRsp(crhInstance))
        {
            crhCleanupInstance(&crhInstance, SIA_CRH_ERROR);
        }
        break;

    case SIA_SIG_CRH_HANDLE_ACT_SEND_RSP:
        /* Check if the pipe-name can be deleted */
        if (!sig->u_param2)
        {
            /* Crh does not need to delete the pipe, de-allocate the 
               pipe-name */
            SIA_FREE(crhInstance->pipeName);
            crhInstance->pipeName = NULL;
        }
        /* Send the response */
        crhCleanupInstance(&crhInstance, (sig->u_param1) ? SIA_CRH_OK: 
            SIA_CRH_ERROR);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "(%s) (%d) unhandled signal\n", __FILE__, __LINE__));
        break;            
    }
    /* Deallocate the signal */
    smaSignalDelete(sig);
}

/*!
 * \brief Initiates data-structures for a content routing session
 *
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL crhCreateInstance(CrhInstance **inst, SiaStateMachine fsm, 
    int signal)
{
    /* Check that no operation is in progress */
    if (NULL != *inst)
    {
        return FALSE;
    }
    /* Allocate memory */
    *inst = SIA_CALLOC(sizeof(CrhInstance));
    /* Setup initial data */
    (*inst)->fsm    = fsm;
    (*inst)->signal = signal;

    /* Init packages */
    if (0 == (crhInstance->actPckId = Sia_pckg_reg_pckg(pckHandle, &funcList)))
    {
        return FALSE;
    }
    /* Register package */
    if (NULL == (crhInstance->actHandle = Sia_pckg_create_inst(pckHandle, 
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
 *               the result is SIA_CRH_TERMINATE no signal is sent back.
 *****************************************************************************/
static void crhCleanupInstance(CrhInstance **inst, SiaCrhResult result)
{
    if (NULL == *inst)
    {
        return;
    }
    else
    {
        if (SIA_CRH_INTERNAL_TERMINATION != result)
        {
            if ((*inst)->state == SIA_CRH_OP_GET_ACTIONS)
            {

                (void)SIA_SIGNAL_SENDTO_UUP((SiaStateMachine)crhInstance->fsm, 
                    crhInstance->signal, result, crhInstance->actionCount, 
                    crhInstance->actions);
                /* The data is now owned by the */
                crhInstance->actions     = NULL;
                crhInstance->actionCount = 0;
            }
            else
            {
                (void)SIA_SIGNAL_SENDTO_U((SiaStateMachine)crhInstance->fsm, 
                    crhInstance->signal, result);
            }
        }
        /* MIME-type */
        if (NULL != (*inst)->mimeType)
        {
            SIA_FREE((*inst)->mimeType);
            (*inst)->mimeType = NULL;
        }
        /* File-name */
        if (NULL != (*inst)->fileName)
        {
            SIA_FREE((*inst)->fileName);
            (*inst)->fileName = NULL;
        }
        if (crhInstance->actions != NULL)
        {
            smaFreeActionList(crhInstance->actions, crhInstance->actionCount);
        }
       
        /* Unreg action package */
        if (0 != (*inst)->actHandle)
        {
            (void)Sia_pckg_terminate(pckHandle, (*inst)->actHandle, FALSE);
            (*inst)->actHandle = 0;
        }
            /* Unreg current op */
        if (0 != (*inst)->actPckId)
        {
            (void)Sia_pckg_unreg_pckg(pckHandle, (*inst)->actPckId);
        }

       
       
      

        if (NULL != (*inst)->fileAttributes)
        {
            we_pck_attr_free(WE_MODID_SIA, (*inst)->fileAttributes);
        }
        /* Handle the pipe-operation */
        if (NULL != (*inst)->pipeName)
        {
            (void)WE_PIPE_DELETE((*inst)->pipeName);
            SIA_FREE((*inst)->pipeName);
            (*inst)->pipeName = NULL;
        }
        /* Action list */
        if (NULL != (*inst)->actions)
        {
            SIA_FREE((*inst)->actions);
        }

        /* Action command */
        SIA_FREE((*inst)->cmd);
        SIA_FREE((*inst));
        (*inst) = NULL;
    }
}

/*! \brief Tells if any actions can be performed on a certain object-type.
 *
 * \param contentType The content-type of the current object.
 * \return TRUE if actions can be performed on this type, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL preFilter(const MmsContentType *contentType)
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
void smaCrhGetActions(const MmsContentType *contentType, const char *drmContentType, 
    SiaStateMachine fsm, int signal)
{
    if (!crhCreateInstance(&crhInstance, fsm, signal))
    {
        /* Failed, cleanup and report */
        crhCleanupInstance(&crhInstance, SIA_CRH_ERROR);
        return ;
    }
    if (!preFilter(contentType))
    {
        /* Do not perform any actions on this object */
        crhCleanupInstance(&crhInstance, SIA_CRH_OK);
        return ;
    }
    crhInstance->state      = SIA_CRH_OP_GET_ACTIONS;
    /* Ask for the list of actions */
    if (0 == (crhInstance->operId = 
        we_act_get_actions(crhInstance->actHandle, (char *)contentType->strValue, 
        WeResourcePipe, drmContentType, TRUE, excludeActions, 
        sizeof(excludeActions)/sizeof(excludeActions[0]))))
    {
        crhCleanupInstance(&crhInstance, SIA_CRH_ERROR);
        return ;
    }
    /* Get the list of action, the result is handled in the */
    if (!Sia_pckg_reg_operation(pckHandle, crhInstance->actHandle, 
        (WE_UINT32)crhInstance->operId, 
        SIA_FSM_CRH, SIA_SIG_CRH_HANDLE_PCK_RSP, NULL))
    {
        crhCleanupInstance(&crhInstance, SIA_CRH_ERROR);
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
            inst->fileAttributes, FALSE, smaGetScreenHandle(), tmp, 1)))
    {
        return FALSE;
    }

    /* Get the list of action, the result is handled in the */
    if (!Sia_pckg_reg_operation(pckHandle, inst->actHandle, 
        crhInstance->operId, SIA_FSM_CRH, SIA_SIG_CRH_HANDLE_ACT_SEND_RSP, 
        NULL))
    {
        return FALSE;
    }
    return TRUE;
}

/*!\brief Send content to another module in the framework.
 *
 * \param fileAttributes The file attributes.
 * \param fsm The callback FSM.
 * \param signal The callback signal.
 * \param cmd The command to execute.
 * \param msgId The message ID of the message containing the data to send.
 * \param bodyPartIndex The body-part index of the content 
 *****************************************************************************/
void smaCrhSendContent(we_pck_attr_list_t *fileAttributes, const char *fileName,
    SiaStateMachine fsm, int signal, const we_act_action_entry_t *actionEntry, 
    MmsMsgId msgId, WE_UINT16 bodyPartIndex)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA,
        "(%s) (%d) Sending content to another module: cmd=%s\n", __FILE__, 
        __LINE__, actionEntry->act_cmd));
    /* Create the instance */
    if (!crhCreateInstance(&crhInstance, fsm, signal))
    {
        we_pck_attr_free(WE_MODID_SIA, fileAttributes);
        crhCleanupInstance(&crhInstance, SIA_CRH_ERROR);
        return ;
    }
    /* Copy instance data */
    crhInstance->fileAttributes = we_pck_attr_dupl(WE_MODID_SIA, 
        fileAttributes);
    crhInstance->msgId          = msgId;
    crhInstance->bodyPartIndex  = bodyPartIndex;
    crhInstance->actionId       = (WE_UINT32)actionEntry->action_id;
    crhInstance->cmd            = we_cmmn_strdup(WE_MODID_SIA, actionEntry->act_cmd);
    crhInstance->state          = SIA_CRH_OP_SEND_CONTENT;
    crhInstance->fileName       = we_cmmn_strdup(WE_MODID_SIA, fileName);

    /* Copy the MIME-type for further use */
    while (fileAttributes != NULL)
    {
        if ((fileAttributes->attribute == WE_PCK_ATTRIBUTE_MIME) &&
            (WE_PCK_ATTR_TYPE_STRING == fileAttributes->type))
        {
            crhInstance->mimeType = we_cmmn_strdup(WE_MODID_SIA, 
                fileAttributes->_u.s);
        }
        fileAttributes = fileAttributes->next;
    }
        
    /* Get the current body-part */
    MMSif_getBodyPart(WE_MODID_SIA, msgId, bodyPartIndex, 
        SIA_GET_BP_RSP_DEST_CRH);
}

/*! \brief Calls package encapsulation signal handler.
 *
 * \param signal   External signal to handle.
 * \param p        Signal data.
 *
 * \return Returns TRUE if signal is consumed otherwise FALSE.
 *****************************************************************************/
WE_BOOL smaCrhHandleSignal(WE_UINT16 signal, void *p)
{
    int res;

    res = Sia_pckg_handle_signal(pckHandle, signal, p);
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
void smaCrhHandleGetBpRsp(const MmsGetBodyPartReply *bodyPart)
{
    /* Guard */
    if (crhInstance == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA,
            "(%s) (%d) Missing CRH instance. Rejecting the 'get body-part "
            "response'\n", __FILE__, __LINE__));
        return;
    }
    /* Check that the body-part operation was successfully */
    if (MMS_RESULT_OK == bodyPart->result)
    {
        /* The pipe-name is all that is needed to continue */
        crhInstance->pipeName = we_cmmn_strdup(WE_MODID_SIA, 
            (char *)bodyPart->pipeName);
        /* Avoid locking execution during signal retrieving */
        (void)SIA_SIGNAL_SENDTO(SIA_FSM_CRH, SIA_SIG_CRH_GET_BODY_PART_RSP);
    }
    else
    {
        /* Operation failed, clean-up and send the response */
        crhCleanupInstance(&crhInstance, SIA_CRH_ERROR);
    }
}

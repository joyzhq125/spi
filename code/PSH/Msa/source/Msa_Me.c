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

/* !\file mame.c
 *  \brief Message editor (properties) control logic. 
 */

/* WE */
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Int.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_Def.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Intsig.h"
#include "Msa_Env.h"
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Se.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Mcr.h"
#include "Msa_Uimenu.h"
#include "Msa_Uime.h"
#include "Msa_Conf.h"
#include "Msa_Core.h"
#include "Msa_Comm.h"
#include "Msa_Uidia.h"
#include "Msa_Slh.h"
#include "Msa_Uiform.h"
#include "Msa_Uicmn.h"
#include "Msa_Addr.h"
#include "Msa_Pbh.h"
#include "Msa_Cth.h"
#include "Msa_Srh.h"
#include "Msa_Mme.h"
#include "Msa_Utils.h"
#include "Msa_Moh.h"

/******************************************************************************
 * Macros
 *****************************************************************************/
/* Checks whether the message type is needed */
#define MSA_ME_IS_MSG_TYPE_NEEDED(m) ((m == MSA_ME_STARTUP_EDIT) ||\
    (m == MSA_ME_STARTUP_FORWARD)   || (m == MSA_ME_STARTUP_REPLY) ||\
    (m == MSA_ME_STARTUP_REPLY_ALL) || (m == MSA_ME_STARTUP_FROM_TEMPLATE))

/* Checks if a new message is going to be created */
#define MSA_ME_CREATE_NEW_MSG(m) ((m == MSA_ME_STARTUP_NEW_MIXED) ||\
    (m == MSA_ME_STARTUP_NEW_SMIL) || (m == MSA_ME_STARTUP_NEW_TEMPLATE))

/* Check whether the message properties needs to be verified before save */
#define MSA_IS_PROP_CHECK_NEEDED(type) ((MSA_SIG_ME_SAVE_PREVIEW != type) &&\
    (MSA_SIG_ME_SAVE_AS_TEMPLATE != type) && (MSA_SIG_ME_SAVE_TEMPLATE != type))

#define MSA_CONTENT_TYPE_BOOKMARK       "application/x-techfaith-bookmark"
/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void msaMeSignalHandler(MsaSignal *sig);
static WE_BOOL createInstance(MsaMeInstance **inst, MsaMeStartupMode mode);
static void deleteInstance(void);
static WE_BOOL handleStartupMode(MsaMeInstance *inst, MsaStateMachine fsm, 
    int sig);
static void handleCt(MsaMeInstance *inst);
static WE_BOOL handleStartup(const MsaMeStartupMode mode, MmsMsgId origId);
static void insertSignature(MsaMessage *msg);
static WE_BOOL showMenuEntry(int entryId);
static WE_BOOL msaDisplayProperty(MsaMeProperty theProperty);
static WE_BOOL msaCheckPropFields(MsaPropItem prop);
/******************************************************************************
 * Static variables 
 *****************************************************************************/
static MsaMeInstance        *curInst;    /*!< The message data */

typedef enum
{
    MSA_ME_NAV_NOT_USED,
    MSA_ME_NAV_SEND,
    MSA_ME_NAV_SAVE,
    MSA_ME_NAV_SAVE_AS_TEMPLATE,
    MSA_ME_NAV_SAVE_TEMPLATE,
    MSA_ME_NAV_EDIT_CONTENT,
    MSA_ME_NAV_OPTIONS,
    MSA_ME_NAV_NAV_BACK
}MsaMeNavMenuEntries;

static const MsaNavMenuItem msaMeNavMenuItems[] = 
{
    /*! Send message */
    {MSA_ME_NAV_SEND, MSA_STR_ID_SEND, MSA_ME_FSM, MSA_SIG_ME_SEND, showMenuEntry},
        
        /*! Save message */
    {MSA_ME_NAV_SAVE, MSA_STR_ID_SAVE, MSA_ME_FSM, MSA_SIG_ME_SAVE, showMenuEntry},
    
    /*! Save message as template */
    {MSA_ME_NAV_SAVE_AS_TEMPLATE, MSA_STR_ID_SAVE_AS_TEMPLATE, MSA_ME_FSM, 
    MSA_SIG_ME_SAVE_AS_TEMPLATE, showMenuEntry},

    /*! Save template */
    {MSA_ME_NAV_SAVE_TEMPLATE, MSA_STR_ID_SAVE_TEMPLATE, MSA_ME_FSM, 
    MSA_SIG_ME_SAVE_TEMPLATE, showMenuEntry},
    
    /*! Edit content part of the message */
    {MSA_ME_NAV_EDIT_CONTENT, MSA_STR_ID_EDIT_CONTENT, MSA_ME_FSM, 
    MSA_SIG_ME_EDIT_CONTENT, showMenuEntry},
    
    /*! Message options */
    {MSA_ME_NAV_OPTIONS, MSA_STR_ID_OPTIONS, MSA_ME_FSM, 
    MSA_SIG_ME_EDIT_PROPERTIES, showMenuEntry},
    
    /*! Back, in property editor */
    {MSA_ME_NAV_NAV_BACK, MSA_STR_ID_BACK, MSA_ME_FSM, MSA_SIG_ME_PROP_BACK,
    NULL},
    
    {MSA_ME_NAV_NOT_USED, 0, MSA_NOTUSED_FSM, 0, NULL}
};

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Function called by menu system to decide if a menu entry should 
 *        be shown
 * 
 * \param entryId   The entryId of the selected menu entry
 * \return TRUE if save as template menu entry should be displayed
 *****************************************************************************/
static WE_BOOL showMenuEntry(int entryId)
{
    WE_BOOL ret = TRUE;
    switch ((MsaMeNavMenuEntries)entryId)
    {
    case MSA_ME_NAV_SEND:
        if ((curInst != NULL ) && (!MSA_ME_ALLOW_SEND(curInst->mode, 
            curInst->origFileType) ||
            (curInst->theMessage->fileType == MMS_SUFFIX_TEMPLATE) ||
            (curInst->theMessage->fileType == MMS_SUFFIX_MSG)))
        {
            /* Do not display send for templates and retrieve conf. */
            ret = FALSE;
        }
        break;
    case MSA_ME_NAV_SAVE:
        if ((curInst != NULL ) && !MSA_ME_ALLOW_SAVE(curInst->mode, 
            curInst->origFileType))
        {
            /* Do not display save during a forward operation, or for 
               templates and retrieve conf. */
            ret = FALSE;
        }
		/*TR 17578*/
		curInst->mcrFsm = MSA_ME_FSM;
        curInst->mcrSig = MSA_SIG_ME_MENU_SAVE_DONE;

        break;
    case MSA_ME_NAV_SAVE_AS_TEMPLATE:
        if ((curInst != NULL ) && !MSA_ME_ALLOW_SAVE_AS_TEMPLATE(curInst->mode))
        {
            /* Do not display "save as template" during a forward operation */
            ret = FALSE;
        }
        break;
    case MSA_ME_NAV_SAVE_TEMPLATE:
        if ((curInst != NULL ) && !MSA_ME_ALLOW_SAVE_TEMPLATE(curInst->mode))
        {
            /* Do not display "save as template" during a forward operation */
            ret = FALSE;
        }
        break;
    case MSA_ME_NAV_EDIT_CONTENT:
        if ((curInst != NULL ) && (curInst->mode == MSA_ME_STARTUP_FORWARD))
        {
            /* Do not display "Edit content" during a forward operation */
            ret = FALSE;
        }
		/* Templates which are edited and sent should not be handled by
           MSA_SE_FSM, therefore alert any waiting FSM that network status  
           data has been updated */
        if ((curInst != NULL ) && 
            (curInst->mode == MSA_ME_STARTUP_FROM_TEMPLATE))
        {
            curInst->mcrFsm = MSA_NOTUSED_FSM;
            curInst->mcrSig = 0;
        }
        else
        {
            /* TR 18363 set MSA_SE_FSM as return FSM so that it can start to 
               view mms */
		    curInst->mcrFsm = MSA_SE_FSM;
            curInst->mcrSig = MSA_SIG_SE_PREVIEW_SAVE_DONE;
        }
        break;
    case MSA_ME_NAV_OPTIONS:
    case MSA_ME_NAV_NAV_BACK:
    case MSA_ME_NAV_NOT_USED:
    default:
        break;
    } /* switch */

    return ret; /* Always show all menu entries */
} /* showMenuEntry */

/*! 
 * \brief Closes the GUI for the message editor 
 *****************************************************************************/
void msaMeTerminateGui(void)
{
    msaMeDeleteNavMenu();
    msaMeDeletePropView();
}

/*!
 * \brief Initiates the message editor by registering its signal handler
 *****************************************************************************/
void msaMeInit(void)
{
    curInst = NULL;
    msaSignalRegisterDst(MSA_ME_FSM, msaMeSignalHandler);
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, "ME initialized\n"));
}

/*!
 * \brief Terminates the message editor.
 *****************************************************************************/
void msaMeTerminate(void)
{
    /* GUI clean-up*/
    msaMeTerminateGui();
    /* Clean-up message data */
    deleteInstance();
    /* Deregister queue */
    msaSignalDeregister(MSA_ME_FSM);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) ME terminated\n", __FILE__, __LINE__));
}

/*!
 * \brief Creates a message editor instance
 *
 * \param inst The new instance
 * \param mode The start-up mode for the ME.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL createInstance(MsaMeInstance **inst, MsaMeStartupMode mode)
{
    MsaMeInstance *newInst;
    
    if (NULL != *inst)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) cannot initialize!\n", __FILE__, __LINE__));
        return FALSE;
    }    
    
    /* Allocate the new instance */
    MSA_CALLOC(newInst, sizeof(MsaMeInstance));
    /* Set the mode */
    newInst->mode = mode;
    /* Set the new instance */
    *inst = newInst;
    if (!msaIsCreateConfValid())
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ME_INVALID_CONFIG), 
            MSA_DIALOG_ERROR);
        return FALSE;
    } 
    return TRUE;
}

/*!
 * \brief Deletes the ME instance.
 *****************************************************************************/
static void deleteInstance(void)
{
    MsaMeInstance **inst = &curInst;

    if (NULL == *inst)
    {
        return;
    }
    
    /* Containing SMIL presentation? */
    if ((*inst)->theMessage && (*inst)->theMessage->smilInfo)
    {
        msaCleanupSmilInstance(&(*inst)->theMessage->smilInfo);
    }
    /* Delete message properties */
    msaFreeMessage(&(*inst)->theMessage);
    /* Check if the "content insert" data has been de-allocated */
    msaCtFreeContentInsert((*inst)->ct);
    /* De-allocate the instance */
    MSA_FREE(*inst);
    *inst = NULL;
}

/*!
 * \brief Handles the structure passed to the ME FSM if Cross Triggering is 
 *        Initiating the start-up.
 *****************************************************************************/
static void handleCt(MsaMeInstance *inst)
{
    void *data;
    MmsContentType *contentType;
    /* Check if the instance is valid */
    if (NULL != inst->ct)
    {
        if ((NULL != inst->ct->buffer) && (inst->ct->name != NULL) && 
            (NULL != msaInsertSlide(inst->theMessage->smilInfo)))
        {
            MSA_CALLOC(contentType, sizeof(MmsContentType));

            if (MSA_MEDIA_GROUP_TEXT == 
                msaMimeToObjGroup(inst->ct->mimeType))
            {
                /* Make sure that text type have '\0' termination at the end */
                MSA_CALLOC(data, inst->ct->bufferSize + 1);
                memcpy(data, inst->ct->buffer, inst->ct->bufferSize);
                MSA_FREE(inst->ct->buffer);
                inst->ct->buffer = data;

                contentType->strValue = (unsigned char *)we_cmmn_strdup(
                    WE_MODID_MSA, msaGetTextMimeType());
            }
            else
            {
                contentType->strValue = (unsigned char *)we_cmmn_strdup(
                    WE_MODID_MSA, inst->ct->mimeType);
            }
            contentType->knownValue = MMS_VALUE_AS_STRING;
            if (NULL == msaSetSlideMo(inst->theMessage->smilInfo, 
                contentType, inst->ct->buffer, inst->ct->bufferSize, 
                inst->ct->name))
            {
                msaCtFreeContentInsert(inst->ct);
            }
            inst->ct->buffer     = NULL;
            inst->ct->bufferSize = 0;
            inst->ct->name       = NULL;
        }
        if (NULL != inst->ct->propItem)
        {
            inst->theMessage->msgProperties->to = inst->ct->propItem->to;
            inst->ct->propItem->to = NULL;
            inst->theMessage->msgProperties->cc = inst->ct->propItem->cc;
            inst->ct->propItem->cc = NULL;
            inst->theMessage->msgProperties->bcc = inst->ct->propItem->bcc;
            inst->ct->propItem->bcc = NULL;
            inst->theMessage->msgProperties->subject = 
                inst->ct->propItem->subject;
            inst->ct->propItem->subject = NULL;
        }
    }
    /* The content is copied, removed unused data */
    msaCtFreeContentInsert(inst->ct);
    inst->ct = NULL;
}

/*!
 * \brief Handles the different ways that the ME can be started
 * 
 * \param inst The current ME instance.
 * \param fsm Callback FSM, used when a message needs to be loaded first
 * \param sig Callback signal, used when a message needs to be loaded first
 * \return TRUE if everything went ok, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL handleStartupMode(MsaMeInstance *inst, MsaStateMachine fsm, 
    int sig)
{
    MsaConfig *config;
    MsaPropItem *propItem;
    MsaMrStartupMode mode;
    
    if (!inst) 
    {
        return FALSE;
    } /* if */

    if (MSA_ME_CREATE_NEW_MSG(inst->mode))
    {
        MSA_CALLOC(inst->theMessage, sizeof(MsaMessage));
        MSA_CALLOC(inst->theMessage->msgProperties, sizeof(MsaPropItem));
        /* Always create SMIL instance for new messages */
        if ((MSA_ME_STARTUP_NEW_SMIL == inst->mode) || 
            (MSA_ME_STARTUP_NEW_TEMPLATE == inst->mode))
        {
            if (!msaCreateSmilInstance(&inst->theMessage->smilInfo))
            {
                return FALSE;
            }
        }

        /* Insert content, if any */
        handleCt(inst);

        /* Insert signature */
        insertSignature(inst->theMessage);

        /* Copy configuration data to new message */
        config = msaGetConfig();
        propItem = inst->theMessage->msgProperties;
        propItem->deliveryReport        = config->deliveryReport;
        propItem->readReport            = config->readReport;
        propItem->senderVisibility      = config->senderVisibility;
        propItem->expiryTime            = config->expiryTime;
        propItem->priority              = config->priority;
        propItem->replyCharging         = config->replyCharging;
        propItem->replyChargingDeadline = config->replyChargingDeadline;
        propItem->replyChargingSize     = config->replyChargingSize;
        
        /* For new messages, show the GUI directly */
        if (!msaMeCreatePropView(inst->theMessage->msgProperties, inst->mode))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) failed to create message property form!\n", 
                __FILE__, __LINE__));
            msaMeDeletePropView();
            return FALSE;
        }
        return TRUE;
    } 
    else
    {
        /* Check which mode to start the MR FSM in */
        switch (inst->mode)
        {
        case MSA_ME_STARTUP_FROM_TEMPLATE:
        case MSA_ME_STARTUP_EDIT:
            mode = MSA_MR_STARTUP_NORMAL;
            break;
        case MSA_ME_STARTUP_REPLY:
            mode = MSA_MR_STARTUP_REPLY;
            break;
        case MSA_ME_STARTUP_FORWARD:
            mode = MSA_MR_STARTUP_FORWARD;
            break;
        case MSA_ME_STARTUP_REPLY_ALL:
            mode = MSA_MR_STARTUP_REPLY_ALL;
            break;
        case MSA_ME_STARTUP_NEW_MIXED:
        case MSA_ME_STARTUP_NEW_SMIL:
        case MSA_ME_STARTUP_NEW_TEMPLATE:
        default:
            return FALSE;
        }
        msaStartMr(mode, inst->origMsgId, inst->origFileType, fsm, 
            (unsigned int)sig);
        return TRUE;
    } /* if */
}

/*! \brief Adds recipient addresses.
 *
 * \param propItem The current addresses
 * \param items The address to add.
 *****************************************************************************/
static void setAddr(MsaAddrItem **current, MsaAddrItem *items)
{
    unsigned int addCount;
    unsigned int currentCount;

    addCount = Msa_AddrItemCount(*current);
    currentCount = Msa_AddrItemCount(items);
    if ((addCount + currentCount) > MSA_CFG_MAX_RECIPIENT_ADDRESSES)
    {
        /* To many items */
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_TO_MANY_RECIPIENTS), 
            MSA_DIALOG_ERROR);
        Msa_AddrListFree(&items);
        return;
    }
    /* Insert items */
    *current = Msa_AddrItemAppend(*current, items);
    msaPbhLookupName(MSA_ME_FSM, MSA_SIG_ME_LOOKUP_RSP, items->address, 
        items->addrType);
}

/*!
 * \brief Reports an MFS_BOOL result back to a requesting FSM, or 
 *        deactivates ME in the case of no requesting FSM
 * 
 * \param result The result to report
 * \param fsm    The FSM to report to
 * \parm sig     The signal to carry the report
 *****************************************************************************/
static void reportOrClose(WE_BOOL result, MsaStateMachine *fsm, int *sig)
{
    if (MSA_NOTUSED_FSM != *fsm)
    {
        (void)MSA_SIGNAL_SENDTO_U(*fsm, *sig, (unsigned long)result);
    }
    else
    {
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
    } /* if */

    /* Used now, don't need'em anymore */
    *fsm = MSA_NOTUSED_FSM;
    *sig = 0;    
} /* reportOrClose */

/*!
 * \brief Handles startup of ME, i.e. decides what to do next
 * 
 * \param mode      The ME startup mode
 * \param origId    The msgid of the originating message, if any
 * \return TRUE on successful startup, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL handleStartup(const MsaMeStartupMode mode, MmsMsgId origId)
{
    /* Check if the file type for the originating message is needed */
    if (MSA_ME_IS_MSG_TYPE_NEEDED(mode))
    {
        /* Get the file type of the originating message */
        msaSrhGetMessageInfo(MSA_ME_FSM, MSA_SIG_ME_GET_MSG_INFO_RSP, origId);
        return TRUE;
    }
    else if (MSA_ME_CREATE_NEW_MSG(mode))
    {
        /* Initiate new message in next state */
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_MR_START);
        return TRUE;
    }

    (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_EDIT), 
        MSA_DIALOG_ERROR);
    return FALSE;
} /* handleStartup */

/*!
 * \brief Handles returning from MR FSM. 
 * 
 * \param inst          The current instance
 * \param result        The MR result code
 * \param message       The message read by MR
 * \param largeObjects  Number of objects larger than limitations
 * \return <whatever is returned>
 *****************************************************************************/
static void handleReturnFromMr(MsaMeInstance *inst, MsaMrResult result, 
    MsaMessage *message, int largeObjects)
{
    inst->theMessage = message;

    if ((MSA_MR_OK != result) || 
        (NULL == inst->theMessage) || 
        (NULL == inst->theMessage->msgProperties))
    {
        /* Clean-up */
        msaMeTerminateGui();
        if (MSA_MR_UNSUPPORTED_HEADER_FIELDS == result)
        {
            (void)msaShowDialog(
                MSA_GET_STR_ID(MSA_STR_ID_UNSUPPORTED_HEADER_FIELDS),
                MSA_DIALOG_ERROR);
        }
        else if (MSA_MR_FORWARD_LOCK == result)
        {
            (void)msaShowDialog(
                MSA_GET_STR_ID(MSA_STR_ID_FAILED_FORWARD_LOCK), MSA_DIALOG_ERROR);
        }
        else
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_EDIT), 
                MSA_DIALOG_ERROR);            
        }

        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        return;
    }
    
    inst->theMessage->msgProperties->senderVisibility = 
        msaGetConfig()->senderVisibility;
    
    /* No SMIL will have been read and allocated in these startup modes */
    if ((MSA_ME_STARTUP_REPLY     == inst->mode) ||
        (MSA_ME_STARTUP_REPLY_ALL == inst->mode))
    {
        if (!msaCreateSmilInstance(&inst->theMessage->smilInfo) ||
            (NULL == msaInsertSlide(inst->theMessage->smilInfo)))
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ERROR), 
                MSA_DIALOG_ERROR);
            return;
        }
        inst->theMessage->msgType = MSA_MR_SMIL;
    } /* if */
    
    if (!msaMeCreatePropView(inst->theMessage->msgProperties, 
        inst->mode))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) failed to create message property form!\n", 
            __FILE__, __LINE__));
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        return;
    }
    /* Name lookup */
    Msa_NameLookup(inst->theMessage->msgProperties->to, 
        MSA_ME_FSM, MSA_SIG_ME_LOOKUP_RSP);
    Msa_NameLookup(inst->theMessage->msgProperties->cc, 
        MSA_ME_FSM, MSA_SIG_ME_LOOKUP_RSP);
    Msa_NameLookup(inst->theMessage->msgProperties->bcc, 
        MSA_ME_FSM, MSA_SIG_ME_LOOKUP_RSP);
    /* Set message properties */
    if (!msaMeSetPropertyValues(inst->theMessage->msgProperties))
    {
        /* Clean-up */
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
    }
    
    /* display dialog if there are size warnings */
    if (largeObjects > 0)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(
            MSA_STR_ID_MULTIPLE_OBJECTS_SIZE_WARNING), MSA_DIALOG_WARNING);
    }
} /* handleReturnFromMr */

/*!
 * \brief The message editor signal handling logic
 *
 * \param sig The received signal
 *****************************************************************************/
static void msaMeSignalHandler(MsaSignal *sig)
{
    MsaAddrItem     *tmpAddrItem;
    MsaMeInstance   *inst;
    MmsFolderType   folder;
    MsaAddrItem     *item = NULL;
    WE_BOOL        deleteMsg;

    inst = curInst;
    if ((sig->type != MSA_SIG_ME_ACTIVATE) && (inst == NULL)) 
    {
        msaSignalDelete(sig);
        return;
    } /* if */

    switch(sig->type)
    {

    /*
     *	ACTIVATION/DEACTIVATION
     */
    case MSA_SIG_ME_ACTIVATE:
        if (!createInstance(&curInst, (MsaMeStartupMode)sig->u_param1))
        {
            msaCtFreeContentInsert(sig->p_param);
            /* Failed to create the instance */
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
            break;
        }
        inst = curInst;
        /* Save activation parameters */
        inst->ct          = (MsaCtContentInsert *)sig->p_param;
        inst->origMsgId   = (MmsMsgId)sig->u_param2;

        /*  Handle startup. Will generate MSA_SIG_ME_GET_MSG_INFO_RSP or 
         *  MSA_SIG_ME_MR_START.
         */
        if (! handleStartup(inst->mode, inst->origMsgId))
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        } /* if */
        break;
        
    case MSA_SIG_ME_DEACTIVATE:
        /* Bring down the GUI*/
        msaMeTerminateGui();
        /* Clean-up the text editor instance */
        deleteInstance();
        (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
        break;
        
    case MSA_SIG_ME_GET_MSG_INFO_RSP:
        /* Got the file type of the originating message */
        if (NULL == sig->p_param)
        {
            /* The file type could not be handled ... exit */
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) failed to get the file type!\n", __FILE__, 
                __LINE__));
            (void)msaShowDialog(MSA_GET_STR_ID(
                MSA_STR_ID_FAILED_TO_GET_MESSAGE_INFO), MSA_DIALOG_ERROR);
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
            break;
        }
        inst->origFileType = 
            ((MmsMessageFolderInfo *)sig->p_param)->suffix;
        MSA_FREE(sig->p_param);
        /* Get message in next state */
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_MR_START);
        break;
        
    /*
     *	MR (Message Reader) INTERACTION
     */
    case MSA_SIG_ME_MR_START:
        /*
         *	Start reading of originating message from disk or content router, 
         *  or setup a fresh new one
         */
        if (!handleStartupMode(inst, MSA_ME_FSM, MSA_SIG_ME_MR_DONE))
        {
            /* Failed to start-up, clean-up and exit */
            deleteInstance();
            /* If messages can not be created then something must be terribly
             * wrong, restart!
             */
            msaSignalDelete(sig);
            MSA_TERMINATE;
        }
        break;
        
    case MSA_SIG_ME_MR_DONE:
        /* Message has been read from disk */
        msaFreeMessage(&inst->theMessage);
        handleReturnFromMr(inst, (MsaMrResult)sig->u_param1, 
            (MsaMessage *)sig->p_param,(int)sig->u_param2);
        break;

    /*
     *	SAVING and SENDING
     */
    case MSA_SIG_ME_SAVE:
    case MSA_SIG_ME_SAVE_AS_TEMPLATE:
    case MSA_SIG_ME_SAVE_TEMPLATE:
    case MSA_SIG_ME_SEND:
    case MSA_SIG_ME_SAVE_PREVIEW:
        /*
         *	Save/send requests are generated by navigation menus,
         *  or requesting FSMs. requesting FSMs want return signals, menus no.
         *  u_param1/2 will be zero when _UU isn't used to send this signal, 
         *  which is ok! (MSA_NOTUSED_FSM)
         */
        if (MSA_NOTUSED_FSM == inst->mcrFsm)
        {
            /* Only set if not already set */
            inst->mcrFsm = (MsaStateMachine)sig->u_param1; 
            inst->mcrSig = (int)sig->u_param2;
        } /* if */
        
        /* Properties doesn't have to be OK for previewing */
        if (MSA_IS_PROP_CHECK_NEEDED(sig->type) && 
            (!msaCheckPropFields(*inst->theMessage->msgProperties)))
        {
            reportOrClose(FALSE, &inst->mcrFsm, &inst->mcrSig);
            break;
        }
        inst->sendFlg = (MSA_SIG_ME_SEND == sig->type);
        /* Start message creation FSM */
        switch(sig->type)
        {
        case MSA_SIG_ME_SAVE_AS_TEMPLATE:
            folder = MMS_TEMPLATES;
            break;
        case MSA_SIG_ME_SAVE_TEMPLATE:
            folder = MMS_TEMPLATES;
            break;
        case MSA_SIG_ME_SEND:
        case MSA_SIG_ME_SAVE:
            folder = MMS_DRAFTS;
            break;
        case MSA_SIG_ME_SAVE_PREVIEW:
            folder = MMS_HIDDEN;
            break;
        default:
            folder = MMS_DRAFTS;
            break;
        }
        /* Delete the message or not? */
        if (MMS_SUFFIX_MSG == inst->origFileType)
        {
            /* Do not delete retrive.conf PDU's */
            deleteMsg = FALSE;
        }
        else
        {
            /* Do not delete the originating message if it is a template or if
               it is a preview operation */
            if ((sig->type == MSA_SIG_ME_SAVE_PREVIEW) || 
                (sig->type == MSA_SIG_ME_SAVE_AS_TEMPLATE))
            {
                deleteMsg = FALSE;
            }
            else
            {
                deleteMsg = (MSA_ME_STARTUP_EDIT == inst->mode) ||
                    ((MSA_ME_STARTUP_FROM_TEMPLATE == inst->mode));
            }
        }
        /* Create the message */
        msaCreateMessage(MSA_ME_FSM, MSA_SIG_ME_SAVE_DONE, inst->theMessage, 
            inst->origMsgId, deleteMsg, folder, inst->origFileType,
            (MSA_ME_STARTUP_FORWARD == inst->mode) ? TRUE: FALSE);
        break;
        
    case MSA_SIG_ME_SAVE_DONE:
        if (MSA_MCR_OK != (MsaMcrResult)sig->u_param1)
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_SAVE), 
                MSA_DIALOG_ERROR);
        }
        else
        {
            if ((NULL != inst) && (NULL != inst->theMessage))
            {
                inst->theMessage->msgId    = (MmsMsgId)sig->u_param2;
                inst->theMessage->fileType = (MmsFileType)sig->i_param;
            }
        }

        /* On error and if done, report and break */
        /*lint -e{613} */
        if ( !(inst->sendFlg) || MSA_MCR_OK != (MsaMcrResult)sig->u_param1)
        {
            /*lint -e{613} */
            reportOrClose((MSA_MCR_OK == (MsaMcrResult)sig->u_param1), 
                &inst->mcrFsm, &inst->mcrSig);
            break;
        } /* if */
        /*lint -e{613} */        
        Msa_SendMessage(MSA_ME_FSM, MSA_SIG_ME_SEND_DONE, 
            inst->theMessage->msgId, FALSE);
        break;
        
    case MSA_SIG_ME_SEND_DONE:
        Msa_ShowSendDialog((MmsResult)sig->u_param1, MSA_NOTUSED_FSM, 0);
        reportOrClose((MmsResult)MMS_RESULT_OK == sig->u_param1, 
            &inst->mcrFsm, &inst->mcrSig);
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        break;                
        
    /*
     *	NAVIGATION MENU 
     */
    case MSA_SIG_ME_NAV_ACTIVATE:
        if (!msaMeCreateNavMenu((MsaNavMenuItem *)msaMeNavMenuItems))
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        } /* if */
        break;

    case MSA_SIG_ME_NAV_DEACTIVATE:
        msaMeDeleteNavMenu();
        break;

    /*
     *	CONTENT EDITING
     */
    case MSA_SIG_ME_EDIT_CONTENT:
        if (MSA_ME_IS_NEW_SMIL_MSG(inst->mode) ||
            ((NULL != inst->theMessage) && 
            (MSA_MR_SMIL == inst->theMessage->msgType)))
        {
            (void)MSA_SIGNAL_SENDTO_P(MSA_SE_FSM, MSA_SIG_SE_ACTIVATE,
                inst);
        }
        else if ((MSA_MR_PLAIN_TEXT == inst->theMessage->msgType) 
            || (MSA_MR_MULTIPART_MIXED == inst->theMessage->msgType))
        {
            /* Activate the right editor - SMIL or Multipart/Mixed */
            (void)MSA_SIGNAL_SENDTO_P(MSA_MME_FSM, MSA_SIG_MME_ACTIVATE,
                inst->theMessage);
        }     
        else
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);            
        }
        break;
        
    /*
     *	PROPERTY EDITING
     */
    case MSA_SIG_ME_EDIT_PROPERTIES:
        if (NULL != inst->theMessage->msgProperties)
        {
            (void)MSA_SIGNAL_SENDTO_P(MSA_CONFIG_FSM, 
                MSA_SIG_CONF_ACTIVATE_SETTINGS, 
                inst->theMessage->msgProperties);
        }
        else
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        } /* if */
        break;

    case MSA_SIG_ME_EDIT_PROPERTY:
        if (!msaDisplayProperty((MsaMeProperty)sig->u_param1))
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_DISPLAY_MM),
                MSA_DIALOG_ERROR);
        }
        break;

    case MSA_SIG_ME_PROP_NEXT:
        if (MSA_ME_STARTUP_FORWARD == inst->mode)
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_NAV_ACTIVATE);
        }
        else
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_EDIT_CONTENT);
        }
        break;

    case MSA_SIG_ME_PROP_BACK:
        if (msaUiMeIsPropertyViewActive())
        {
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        } /* if */
        break;

    case MSA_SIG_ME_MENU_SEND_DONE:
    case MSA_SIG_ME_MENU_SAVE_DONE:
    case MSA_SIG_ME_MENU_SAVE_AS_TEMPLATE_DONE:
    case MSA_SIG_ME_MENU_SAVE_TEMPLATE_DONE:
    case MSA_SIG_ME_PROP_SEND_DONE:     
    case MSA_SIG_ME_PROP_SAVE_DONE:
    case MSA_SIG_ME_PROP_SAVE_AS_TEMPLATE_DONE:        
    case MSA_SIG_ME_PROP_SAVE_TEMPLATE_DONE:        
        if (TRUE == sig->u_param1)
        {    
            /*
             *	All done, let's shut down. 
             *  Any error messages will have been shown by others. 
             */
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        } /* if */                
        break;

    case MSA_SIG_ME_ADDR_INPUT_DEACTIVATE:
        msaMeDeleteAddrInputView();
        break;

    case MSA_SIG_ME_ADD_RECEPIENTS:
        switch(sig->u_param1)
        {
        case MSA_ME_TO:
            setAddr(&(inst->theMessage->msgProperties->to),
                (MsaAddrItem *)sig->p_param);
            break;
        case MSA_ME_CC:
            setAddr(&(inst->theMessage->msgProperties->cc),
                (MsaAddrItem *)sig->p_param);
            break;
        case MSA_ME_BCC:
            setAddr(&(inst->theMessage->msgProperties->bcc),
                (MsaAddrItem *)sig->p_param);
            break;
        }
        /* Update the property view */
        if (!msaMeSetPropertyValues(inst->theMessage->msgProperties))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(
                MSA_STR_ID_FAILED_TO_OPEN_WINDOW), MSA_DIALOG_ERROR);
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) failed to display message properties\n", 
                __FILE__, __LINE__));
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        }
        break;

    case MSA_SIG_ME_CHANGE_RECEPIENT:
        switch(sig->u_param1)
        {
        case MSA_ME_TO:
            Msa_ChangeAddr(inst->theMessage->msgProperties->to, 
                (MsaAddrItem *)sig->p_param, sig->u_param2);
            break;
        case MSA_ME_CC:
            Msa_ChangeAddr(inst->theMessage->msgProperties->cc,
                (MsaAddrItem *)sig->p_param, sig->u_param2);
            break;
        case MSA_ME_BCC:
            Msa_ChangeAddr(inst->theMessage->msgProperties->bcc,
                (MsaAddrItem *)sig->p_param, sig->u_param2);
            break;
        }
        /* Update the */
        if (!msaMeSetPropertyValues(inst->theMessage->msgProperties))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(
                MSA_STR_ID_FAILED_TO_OPEN_WINDOW), MSA_DIALOG_ERROR);
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) failed to display message properties\n", 
                __FILE__, __LINE__));
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        }            
        break;
        
    case MSA_SIG_ME_LOOKUP_RSP:
        if ((NULL != inst) && 
            (NULL != inst->theMessage) && 
            (NULL != inst->theMessage->msgProperties) && 
            ((NULL != inst->theMessage->msgProperties->to) || 
             (NULL != inst->theMessage->msgProperties->cc) ||
             (NULL != inst->theMessage->msgProperties->bcc)))            
        {
            Msa_AddrItemMerge(inst->theMessage->msgProperties->to, 
                sig->p_param);
            Msa_AddrItemMerge(inst->theMessage->msgProperties->cc, 
                sig->p_param);
            Msa_AddrItemMerge(inst->theMessage->msgProperties->bcc,
                sig->p_param);
            /* Update the message properties */
            if (!msaMeSetPropertyValues(inst->theMessage->msgProperties))
            {
                (void)msaShowDialog(MSA_GET_STR_ID(
                    MSA_STR_ID_FAILED_TO_OPEN_WINDOW), MSA_DIALOG_ERROR);
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                    "(%s) (%d) failed to display message properties\n", 
                    __FILE__, __LINE__));
                (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
            }
        }
        if (NULL != sig->p_param)
        {
            tmpAddrItem = (MsaAddrItem *)sig->p_param;
            Msa_AddrListFree(&tmpAddrItem);
        }
        break;

    case MSA_SIG_ME_ADDR_LIST_DEACTIVATE:
        msaMeDeleteAddrListView();
        break;

    case MSA_SIG_ME_ADDR_LIST_MENU_ACTIVATE:
        if (!msaMeCreateAddrListMenu())
        {
            msaMeDeleteAddrListMenu();
        }
        break;

    case MSA_SIG_ME_ADDR_LIST_MENU_DEACTIVATE:
        msaMeDeleteAddrListMenu();
        break;

    case MSA_SIG_ME_DELETE_ADDR_ITEM:
        switch(sig->u_param1)
        {
        case MSA_ME_TO:
            Msa_AddrItemDelete(&inst->theMessage->msgProperties->to, sig->u_param2);
            item = inst->theMessage->msgProperties->to;
            break;
        case MSA_ME_CC:
            Msa_AddrItemDelete(&inst->theMessage->msgProperties->cc, sig->u_param2);
            item = inst->theMessage->msgProperties->cc;
            break;
        case MSA_ME_BCC:
            Msa_AddrItemDelete(&inst->theMessage->msgProperties->bcc, sig->u_param2);
            item = inst->theMessage->msgProperties->bcc;
            break;
        }
        if (!msaMeUpdateAddrListView(item))
        {
            msaMeDeleteAddrListView();
        }
        if (!msaMeSetPropertyValues(inst->theMessage->msgProperties))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(
                MSA_STR_ID_FAILED_TO_OPEN_WINDOW), MSA_DIALOG_ERROR);
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
        }
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "ME: Erroneous signal received (%d).\n", sig->type));
        break;
    }
    /* Remove signal data */
    msaSignalDelete(sig);
}

/*!
 * \brief When a message subject is changed this callback function is called
 *
 * \param strValue The string value
 * \param notUsed Not used
 * \param notUsed2 Not used
 * \param theProperty The property that was changed, see #MsaMeProperty
 * \return TRUE on successful propagation of properties, FALSE otherwise
 *****************************************************************************/
/*lint -e{818} */
static WE_BOOL msaMeHandleSubjectChanges(char *strValue, unsigned int notUsed, 
    void *notUsed2, unsigned int theProperty)
{
    MsaMeProperty prop = (MsaMeProperty)theProperty;
    char **property = NULL;
    if ((0 != notUsed) || (NULL != notUsed2))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) Uninitialized variables!\n", __FILE__, __LINE__));
    }
    /*lint -e{788} */
    switch(prop)
    {
    case MSA_ME_SUBJECT:        
        property = NULL;

        if ((NULL != curInst) &&
            (NULL != curInst->theMessage) &&
            (NULL != curInst->theMessage->msgProperties))
        {
            property = &(curInst->theMessage->msgProperties->subject);        
        } /* if */        
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) erroneous message property referenced!\n", 
            __FILE__, __LINE__));
        return FALSE;
    }
    /* Free old property*/
    if (NULL != property)
    {
        MSA_FREE(*property);
        *property = NULL;
        *property = strValue;
    }
    /* Set the new property */
    
    /* Update the GUI */
    if (!msaMeSetPropertyValues(curInst->theMessage->msgProperties))
    {
        return FALSE;
    }

    return TRUE;
}

/*!
 * \brief Displayes the input form for a specific message property
 *
 * \param theProperty Which property to display.
 * \return TRUE on successful input form creation, FALSE otherwise
 *****************************************************************************/
static WE_BOOL msaDisplayProperty(MsaMeProperty theProperty)
{
    MsaInputItem item;
    WE_UINT32 strId;
    MsaMeInstance *meInstance = curInst;
    MsaAddrItem **addr;

    memset(&item, 0, sizeof(MsaInputItem));

    if ((NULL == meInstance) ||
        (NULL == meInstance->theMessage) ||
        (NULL == meInstance->theMessage->msgProperties))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) instance or data missing!\n", __FILE__, __LINE__));
        return FALSE;
    }
    /* Check which property that is active */
    addr = NULL;
    switch(theProperty)
    {
    case MSA_ME_TO:
        addr = &(meInstance->theMessage->msgProperties->to);
        break;
    case MSA_ME_CC:
        addr = &(meInstance->theMessage->msgProperties->cc);
        break;
    case MSA_ME_BCC:
        addr = &(meInstance->theMessage->msgProperties->bcc);
        break;
    case MSA_ME_SUBJECT:
        item.text           = meInstance->theMessage->msgProperties->subject;
        item.maxTextSize    = MSA_ME_SUBJECT_MAX_SIZE;
        item.type           = MSA_SINGLE_LINE_INPUT;
        strId               = MSA_STR_ID_SUBJECT;
        item.callback       = msaMeHandleSubjectChanges;
        item.uintValue      = theProperty;
        /* Display the input form */
        if (!msaCreateInputForm(&item, MSA_GET_STR_ID(strId)))
        {
            return FALSE;
        }
        return TRUE;

    case MSA_ME_NONE:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) erroneous property value!\n", __FILE__, __LINE__));
        return FALSE;
    }

    /* TO/CC/BCC */
    if (NULL == *addr)
    {
        if (!msaMeCreateAddrInputView(theProperty, *addr, 0))
        {
            msaMeDeleteAddrInputView();
            return FALSE;
        }
    }
    else
    {
        if (!msaMeCreateAddrListView(theProperty, addr))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*!
 * \brief Verifies if at least one of the properties are set.
 *
 * \param prop Which property to display.
 * \return TRUE if at least one property field is present and the mode is
 *         not TEMPLATE-mode.
 *****************************************************************************/
static WE_BOOL msaCheckPropFields(MsaPropItem prop)
{
    MsaMeInstance *meInstance = curInst;

    if (!(meInstance->mode == MSA_ME_STARTUP_NEW_TEMPLATE))
    {
        /*
         *	We must test all combinations of empty address fields.
         *  Both NULL-fields and ""-fields.
         */
        if ((NULL == prop.cc) && (NULL == prop.to) && ((NULL == prop.bcc)))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ADDRESS_FIELD_MISSING), 
                MSA_DIALOG_ERROR);
            return FALSE;
        }
    }
    return TRUE;
}

/*! \brief Inserts a pre-defined signature into the message. The signature
 *         is appended on the first text-part or inserted as a new part
 *         if no text-part is present.
 * \param msg The current message.
 *****************************************************************************/
static void insertSignature(MsaMessage *msg)
{
    MsaSmilSlide *slides;
    MsaMoDataItem *mo = NULL;
    char *newStr;
    MmsContentType *contentType;
    MsaConfig *tmpConfig;
    tmpConfig = msaGetConfig();

    /* Check if signature should be inserted */
    if (tmpConfig && tmpConfig->useSignature && tmpConfig->signature)
    {
        /* Find a text-part to insert the signature into */
        if (NULL != msg->smilInfo)
        {
            /* Is it a SMIL message? */
            slides = msg->smilInfo->slides;
            while(NULL != slides)
            {
                msg->smilInfo->currentSlide = slides;
                if (NULL != (mo = msaGetMo(msg->smilInfo, MSA_MEDIA_GROUP_TEXT)))
                {
                    break;
                }
                slides = slides->next;
            }
        }
        else
        {
            /* Non SMIL messages */
            mo = msaFindObjByType(msg->smilInfo->mediaObjects, MSA_MEDIA_GROUP_TEXT);
        }
        if (mo != NULL)
        {
            /* Append the signature */
            newStr = we_cmmn_str3cat(WE_MODID_MSA, (char *)mo->data, "\n", 
                tmpConfig->signature);
            MSA_FREE(mo->data);
            mo->data = (unsigned char*)newStr;
            mo->size = strlen(newStr) + 1;
        }
        else
        {
            /* No part found insert a new part */
            contentType = MSA_ALLOC_TYPE(MmsContentType);
            contentType->knownValue = MMS_VALUE_AS_STRING;
            contentType->strValue = (unsigned char *)
                we_cmmn_strdup(WE_MODID_MSA, "text/plain");
            contentType->params = NULL;
            contentType->drmInfo = NULL;
            if (NULL != msg->smilInfo)
            {
                if (0 == msg->smilInfo->noOfSlides)
                {
                    (void)msaInsertSlide(msg->smilInfo);
                }
                /* Insert a new part into the SMIL message */
                (void)msaSetSlideMo(msg->smilInfo, contentType, 
                    (unsigned char *)we_cmmn_strcat(WE_MODID_MSA, "\r\n",
                    tmpConfig->signature), strlen(tmpConfig->signature) + 3, 
                    msaCreateFileNameFromType((char *)contentType->strValue));
            }
            else
            {
                (void)msaAddMo(&msg->smilInfo->mediaObjects, (unsigned char*)
                    we_cmmn_strcat(WE_MODID_MSA, "\r\n", tmpConfig->signature), 
                    strlen(tmpConfig->signature) + 3, contentType, 
                    msaCreateFileNameFromType((char *)contentType->strValue));
            }
        }
    }
}

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

/*!\file mamme.c
 * \brief Message editor (Multi-Part/Mixed) control logic. 
 */

/* WE */
#include "We_Mem.h"
#include "We_Int.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Dlg.h"

/* MMS Service/Lib */
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA*/
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Mme.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Utils.h"
#include "Msa_Slh.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Uicmn.h"
#include "Msa_Utils.h"
#include "Msa_Uidia.h"
#include "Msa_Ph.h"
#include "Msa_Mob.h"
#include "Msa_Uimme.h"
#include "Msa_Moh.h"

/******************************************************************************
 * Constants
 *****************************************************************************/
#define MSA_MME_TYPE_SEP ","

#define MSA_MCR_PLAIN_TEXT  "text/plain"

/******************************************************************************
 * Static data-types
 *****************************************************************************/
static MsaMessage   *currentMsg;        /*!< The current message */
static WE_BOOL     replaceFlag;        /*!< Whether add == replace */

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void handleMobAction(MsaMobOperation op, int index);
static MsaSeverity setMediaObject(const MsaMessage *msg, WE_BOOL replace, 
    MsaAfiReadResultData *objData, MsaAfiReadResult readResult);
static void restartMob(void);
static void mmeSignalHandler(MsaSignal *sig);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Handles events from the MOB
 * 
 * \param op    The operation or event 
 * \param index The index of the selected object while the event occurred
 *****************************************************************************/
static void handleMobAction(MsaMobOperation op, int index)
{
    MsaMobSpecialOp param;

    switch(op) 
    {
    case MSA_MOB_SELECT: 
    case MSA_MOB_MENU: 
        /* Menu button pressed on entry */
        (void)MSA_SIGNAL_SENDTO_I(MSA_MME_FSM, MSA_SIG_MME_NAV_MENU_ACTIVATE, 
            index);
        break;

    case MSA_MOB_FOCUS_ACTIVITY:
        /* Changed focus within attachment list */
        msaUiMmeStopAudio();
        break;

    case MSA_MOB_SPECIAL_OP:
        param = (MsaMobSpecialOp)index;
        if (MSA_MOB_SOP_ADD_MO & param) 
        {
            (void)msaOpenDlgCreateMediaInput(MSA_MME_FSM, 
            MSA_SIG_MME_GET_MEDIA_OBJECT, 
            MSA_CFG_OPEN_IMAGE_TYPES    MSA_MME_TYPE_SEP \
            MSA_CFG_OPEN_TEXT_TYPES     MSA_MME_TYPE_SEP \
            MSA_CFG_OPEN_AUDIO_TYPES);            
        } /* if */
        break;    
        
    case MSA_MOB_DEACTIVATED:
    default:
        /* no action*/
        break;
    }    
} /* handleMobAction */

/*! \brief Adds a media object to a message
 *
 * \param msg The message to add the MO to
 * \param replace Whether to replace current object 
 * \param objData the media object and information about it
 * \param readResult the result of the load object operation
 * \return MSA_SIZE_INFO_OK, otherwise how a dialog should be shown
 *****************************************************************************/
static MsaSeverity setMediaObject(const MsaMessage *msg, WE_BOOL replace, 
    MsaAfiReadResultData *objData, MsaAfiReadResult readResult)
{
	WE_UINT32      newMsgSize;
    unsigned int    oldMoSize = 0;
    unsigned int    newMoSize = 0;
    MsaMoDataItem   *item;
    MsaSeverity     result;
    MmsContentType  *contentType;

    if (NULL == objData)
    {
        return MSA_SEVERITY_ERROR;
    }

    result = MSA_SEVERITY_OK;

    if (NULL != objData->mimeType)
    {
        /* oldMoSize = size before inclusion */
        if (NULL != 
            (item = msaGetMo(msg->smilInfo, msaMimeToObjGroup(objData->mimeType))))
        {
            oldMoSize = item->size;
        }
        /* newMoSize = size of new object */
        newMoSize = objData->size;
    } /* if */

    if ((MSA_AFI_READ_RESULT_TO_LARGE != readResult) && /* handled below */
        (MSA_MEDIA_GROUP_NONE == msaMimeToObjGroup(objData->mimeType)))
    {
        (void)msaShowDialogWithCallback(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_READ_MO), 
            MSA_DIALOG_ERROR, MSA_MME_FSM, MSA_SIG_MME_RESTART_MOB_IND);
        return MSA_SEVERITY_ERROR;
    } /* if */

    /* The size the message will get after inclusion of this media object */
	newMsgSize = msaCalculateMsgSize(msg->smilInfo, msg->msgProperties) 
        - (replace ? oldMoSize : 0) + newMoSize;
       
    /* compare message and object sizes */
    result = msaShowSizeWarningDialog(MSA_AFI_READ_RESULT_TO_LARGE == readResult
        ? 0xffffffff : newMsgSize, newMoSize, msaMimeToObjGroup(objData->mimeType), 
        MSA_MME_FSM, MSA_SIG_MME_RESTART_MOB_IND);
    if (result == MSA_SEVERITY_ERROR)
    {
        return MSA_SEVERITY_ERROR;
    }
    
    if (replace) 
    {
        msaDeleteMo(&msg->smilInfo->mediaObjects, NULL, 
            msaUiMmeGetSelectedMo());        
    } /* if */    

    if (NULL == objData->fileName)
    {
        objData->fileName = msaCreateFileNameFromType(objData->mimeType);
        if (NULL == objData->fileName)
        {
            (void)msaShowDialogWithCallback(MSA_GET_STR_ID(
                MSA_STR_ID_FAILED_TO_READ_MO), MSA_DIALOG_ERROR, MSA_MME_FSM,
                MSA_SIG_MME_RESTART_MOB_IND);
            return MSA_SEVERITY_ERROR;
        } /* if */
    }
    msaRemPathFromFileName(objData->fileName);

    MSA_CALLOC(contentType, sizeof(MmsContentType));
    contentType->knownValue = MMS_VALUE_AS_STRING;
    contentType->strValue = (unsigned char *)objData->mimeType;
    item = msaAddMo(&msg->smilInfo->mediaObjects, objData->data, objData->size, 
        contentType, objData->fileName);
    if (NULL == item)
    {
        (void)msaShowDialogWithCallback(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_READ_MO),
                MSA_DIALOG_ERROR, MSA_MME_FSM, MSA_SIG_MME_RESTART_MOB_IND);
        return MSA_SEVERITY_ERROR;
    }
    objData->data = NULL;
    objData->fileName = NULL;
    objData->mimeType = NULL;
    return result;
} /* setMediaObject */


/*! \brief Restart mob
 *****************************************************************************/
static void restartMob(void)
{
    (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);            
    msaMobActivate(currentMsg->smilInfo->mediaObjects, MSA_MOB_SOP_ADD_MO, 
        MSA_STR_ID_EDIT_CONTENT, MSA_MME_FSM, 
        MSA_SIG_MME_MOB_OP);             
}

/*!
 * \brief The Multipart/mixed editor signal handling logic
 *
 * \param sig The received signal
 *****************************************************************************/
static void mmeSignalHandler(MsaSignal *sig)
{
    int             spaceLeft;
    char            *text;
    MsaMoDataItem   *mo;
    MmsContentType  *contentType;

    if ((MSA_SIG_MME_ACTIVATE != sig->type) && (NULL == currentMsg)) 
    {
        return; 
    }

    switch (sig->type)
    {
    /*
     *	Activation/deactivation
     */
    case MSA_SIG_MME_ACTIVATE: 
        /*
         *	Activation. 
         */
        currentMsg = (MsaMessage*)sig->p_param;
        if (!currentMsg) 
        {
            break;
        } /* if */
        msaMmeUiSetMsg(currentMsg);

        /* activate MOB */
        msaMobActivate(currentMsg->smilInfo->mediaObjects, 
            MSA_MOB_SOP_ADD_MO, MSA_STR_ID_EDIT_CONTENT,  
            MSA_MME_FSM, MSA_SIG_MME_MOB_OP);             
        break; 

    case MSA_SIG_MME_DEACTIVATE: 
        msaUiMmeStopAudio();
        msaMmeUiSetMsg(NULL);
        (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);
        /* terminate ME */
        msaMmeTerminateGui();
        currentMsg = NULL; 
        break;
        
    /*
     *	Interaction with the MOB
     */
    case MSA_SIG_MME_MOB_OP:
        /* End User did something in MOB view */
        msaUiMmeStopAudio();
        handleMobAction((MsaMobOperation)sig->u_param1, sig->i_param);        
        break;

    case MSA_SIG_MME_GET_MEDIA_OBJECT_REPLACEMENT:
    case MSA_SIG_MME_GET_MEDIA_OBJECT:
        replaceFlag = (MSA_SIG_MME_GET_MEDIA_OBJECT_REPLACEMENT == sig->type);

        if (WE_DLG_RETURN_OK == sig->u_param1)
        {
            /*
             *	No use loading media objects larger than the lesser of 
             *  a) the 100k barrier, and b) the maximum media object size
             */
            spaceLeft = MIN(
                MSA_CFG_MESSAGE_SIZE_MAXIMUM - msaCalculateMsgSize(
                currentMsg->smilInfo, currentMsg->msgProperties), 
                MSA_CFG_INSERT_MEDIA_OBJECT_MAX_SIZE);

            if (!msaAfiLoadFromFile(MSA_MME_FSM, MSA_SIG_MME_GET_MEDIA_OBJECT_RSP, 
                sig->p_param, NULL, (unsigned int)spaceLeft))
            {
                (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_READ_MO), 
                    MSA_DIALOG_ERROR);
            }
        }
        break;

    case MSA_SIG_MME_GET_MEDIA_OBJECT_RSP:
        /*
         *	Data has been fetched for inclusion into MM.
         */

        if (MSA_SEVERITY_OK == setMediaObject(currentMsg, replaceFlag,
            (MsaAfiReadResultData *)sig->p_param, 
            (MsaAfiReadResult)sig->u_param1))
        {
            /* no dialogs visible, can restart MOB directly */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_RESTART_MOB_IND);
        } /* if */
        msaAfiLoadFreeData((MsaAfiReadResultData *)sig->p_param);

        break;

    case MSA_SIG_MME_RESTART_MOB_IND:
        /* object was added and MOB must be restarted */
        restartMob();
        break;

    case MSA_SIG_MME_PLAY_SOUND:
        msaUiMmePlayAudio();
        break;
        
    /*
     *	Text editing
     */
    case MSA_SIG_MME_DELETE_MO: 
        msaDeleteMo(&currentMsg->smilInfo->mediaObjects, NULL, 
            (MsaMoDataItem *)sig->p_param);        
        sig->p_param = NULL;
        /* Restart MOB because it cannot be modified */
        restartMob();
        break;

    case MSA_SIG_MME_ADD_TEXT:
        if (!msaUiMmeCreateTextView(NULL, 
            msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_ADD_TEXT)),
            MSA_MME_FSM, MSA_SIG_MME_ADD_TEXT_OK, MSA_SIG_MME_TEXT_BACK, -1))
        {
            /* Text creation failed terminate FSM */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_DEACTIVATE);
        }
        break;

    case MSA_SIG_MME_EDIT_TEXT:
        mo = (MsaMoDataItem *)sig->p_param;
        if (!msaUiMmeCreateTextView(mo ? (char *)mo->data : NULL, 
            mo ? mo->name : NULL, MSA_MME_FSM, MSA_SIG_MME_TEXT_OK, 
            MSA_SIG_MME_TEXT_BACK, -1))
        {
            /* Text creation failed terminate FSM */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_DEACTIVATE);
        }
        break;

    case MSA_SIG_MME_ADD_TEXT_OK:
        if ((NULL != (text = msaUiMmeGetMessageText())) &&
            (strlen(text) > 0))
        {
			MSA_CALLOC(contentType,sizeof(MmsContentType));
            memset(contentType, 0, sizeof(MmsContentType));
            contentType->knownValue = MMS_VALUE_AS_STRING;
            contentType->strValue = 
                (unsigned char *)we_cmmn_strdup(WE_MODID_MSA, MSA_MCR_PLAIN_TEXT);

            if (NULL == msaAddMo(&currentMsg->smilInfo->mediaObjects, 
                (unsigned char*)text, strlen(text)+1, contentType,
                msaCreateFileNameFromType(MSA_MCR_PLAIN_TEXT)))
            {
                (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ERROR),
                    MSA_DIALOG_ERROR);
                msaUiMmeDeleteTextView();    
                break;
            }
        } /* if */
        msaUiMmeDeleteTextView();
        /* Restart MOB because it cannot be modified */
        restartMob();
        break;

    case MSA_SIG_MME_TEXT_OK:
        if ((NULL != (mo = msaUiMmeGetSelectedMo()))  &&
            (NULL != (text = msaUiMmeGetMessageText())))
        {
            if (strlen(text) > 0)
            {
                MSA_FREE(mo->data);
                mo->data = (unsigned char *)text;
                mo->size = strlen(text)+1;            
            }
            else
            {
                (void)MSA_SIGNAL_SENDTO_P(MSA_MME_FSM, MSA_SIG_MME_DELETE_MO,
                    mo);
                msaUiMmeDeleteTextView();
                break;
            }
        } /* if */
        msaUiMmeDeleteTextView();
        /* Restart MOB because it cannot be modified */
        restartMob();
        break;

    case MSA_SIG_MME_TEXT_BACK:
        msaUiMmeDeleteTextView();
        break;

    /*
     *	Saving/sending - done via ME.
     */
    case MSA_SIG_MME_SEND: 
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SEND, 
            MSA_MME_FSM, MSA_SIG_MME_SEND_DONE);
        break;
        
    case MSA_SIG_MME_SAVE: 
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE, 
            MSA_MME_FSM, MSA_SIG_MME_SAVE_DONE);
        break;
        
    case MSA_SIG_MME_SAVE_AS_TEMPLATE: 
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE_AS_TEMPLATE,
            MSA_MME_FSM, MSA_SIG_MME_SAVE_AS_TEMPLATE_DONE);
        break;

    case MSA_SIG_MME_SAVE_TEMPLATE: 
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE_TEMPLATE,
            MSA_MME_FSM, MSA_SIG_MME_SAVE_TEMPLATE_DONE);
        break;

    case MSA_SIG_MME_SEND_DONE:
    case MSA_SIG_MME_SAVE_DONE:
    case MSA_SIG_MME_SAVE_AS_TEMPLATE_DONE:
    case MSA_SIG_MME_SAVE_TEMPLATE_DONE:
        if (TRUE == sig->u_param1)
        {    
            /*
             *	All done, let's shut down. 
             *  Any error messages will have been shown by ME.
             */
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM,  MSA_SIG_ME_DEACTIVATE);
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_DEACTIVATE);
        } /* if */        
        break;
        
    /*
     *	Navigation menu, context sensitive
     */
    case MSA_SIG_MME_NAV_MENU_DEACTIVATE: 
        msaUiMmeDeleteNavMenu();        
        break;       
        
    case MSA_SIG_MME_NAV_MENU_ACTIVATE: 
        if (!msaUiMmeCreateNavMenu(sig->i_param, currentMsg->fileType))
        {
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_DEACTIVATE);
        }
        break;       

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "%s(%d): mmeSignalHandler erroneous signal received(%d)\n", 
            __FILE__, __LINE__, sig->type));
        break;
    }
    msaSignalDelete(sig);
} /* mmeSignalHandler */

/***************************************************************************** 
 * Exported functions 
 *****************************************************************************/

/*!
 * \brief Initiates the Multipart/mixed editor by registering its signal 
 *        handler and initiating the data structures etc
 *****************************************************************************/
void msaMmeInit(void)
{
    msaSignalRegisterDst(MSA_MME_FSM, mmeSignalHandler);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
        "%s(%d): MME FSM initialised!\n", __FILE__, __LINE__));
    currentMsg = NULL; 
} /* msaMmeInit */

/*!
 * \brief Terminates by de-registering the signal handler and 
 *          freeing all data structures used
 *****************************************************************************/
void msaMmeTerminate(void)
{
    msaSignalDeregister(MSA_MME_FSM);
    msaMmeTerminateGui();
    currentMsg = NULL; 
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
        "%s(%d): MME FSM terminated!\n", __FILE__, __LINE__));
} /* msaMmeTerminate */

/*!
 * \brief Terminates the GUI for the Multipart/mixed editor.
 *****************************************************************************/
void msaMmeTerminateGui(void)
{
    /* One-state shutdown */
    msaUiMmeStopAudio();
    msaUiMmeDeleteTextView();
    msaUiMmeDeleteNavMenu();
} /* msaMmeTerminateGui */





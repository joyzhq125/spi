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

/*!\file mase.c
 * SMIL editor control logic. 
 */

/* WE */
#include "We_Mem.h"
#include "We_Int.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Core.h"
#include "We_Dlg.h"

/* MMS Service/Lib */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA*/
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Se.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Utils.h"
#include "Msa_Slh.h"
#include "Msa_Uise.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Core.h"
#include "Msa_Uicmn.h"
#include "Msa_Rc.h"
#include "Msa_Utils.h"
#include "Msa_Uidia.h"
#include "Msa_Del.h"
#include "Msa_Ph.h"
#include "Msa_Uicols.h"
#include "Msa_Uisemm.h"
#include "Msa_Moh.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! Colors used for setting back-ground and forground color in SMIL.
 *	 
 */
static const WE_UINT32 bgColors[] = MSA_CFG_SMIL_BG_COLORS;
static const WE_UINT32 txtColors[] = MSA_CFG_SMIL_TEXT_COLORS;

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static data-types
 *****************************************************************************/

/*!\brief The current message */
static MsaMeInstance *currentMsg;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static WE_BOOL handleActivate(const MsaMessage *msg);
static void handleGetMediaObject(const MsaMessage *msg, int dlgResult, 
    char *fileName);
static WE_BOOL insertSlide(const MsaMessage *msg);
static WE_BOOL deleteSlide(const MsaMessage *msg);
static void changeSlide(const MsaMessage *msg, WE_BOOL next);
static void exitSmilCreate(WE_BOOL showDialog, WE_UINT32 strId);
static void seSignalHandler(MsaSignal *sig);
static WE_BOOL editText(const MsaMessage *msg);
static WE_BOOL editMediaObject(const MsaMessage *msg, MsaMediaGroupType objType);
static MsaSeverity setMediaObject(MsaSmilInfo *smilInfo, 
    const MsaPropItem *propItem, MsaAfiReadResultData *objData, 
    MsaAfiReadResult readResult);
static void handleItemTiming(const MsaMeInstance *msg, MsaMediaGroupType gType);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Edits the current media object or inserts new content.
 *
 * \param smilInfo The current SMIL presentation.
 * \return TRUE on OK else FALSE
 *****************************************************************************/
static WE_BOOL editMediaObject(const MsaMessage *msg, MsaMediaGroupType objType)
{
    switch (objType)
    {
    case MSA_MEDIA_GROUP_IMAGE:
        return msaOpenDlgCreateMediaInput(MSA_SE_FSM, MSA_SIG_SE_GET_MEDIA_OBJECT, 
            MSA_CFG_OPEN_IMAGE_TYPES);
    case MSA_MEDIA_GROUP_AUDIO:
        return msaOpenDlgCreateMediaInput(MSA_SE_FSM, MSA_SIG_SE_GET_MEDIA_OBJECT, 
            MSA_CFG_OPEN_AUDIO_TYPES);
    case MSA_MEDIA_GROUP_TEXT:
            return editText(msg);
    case MSA_MEDIA_GROUP_NONE:
        return FALSE;
    }
    return FALSE;
} /* editMediaObjects */

/*! \brief Edit the text part of the current slide
 *	
 * \param msg The current message.
 *****************************************************************************/
static WE_BOOL editText(const MsaMessage *msg)
{
    char *text = NULL;
    /* get the current text */
    if ((NULL != msaGetCurrentSlide(msg->smilInfo)) &&
        (NULL != msaGetMo(msg->smilInfo, MSA_MEDIA_GROUP_TEXT)))
    {
        text = (char *)(msaGetMo(msg->smilInfo, MSA_MEDIA_GROUP_TEXT)->data);
    }
    /* Display the text */
    if (!msaSeCreateTextView(text, msg->msgProperties->subject))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "Create text view failed!\n"));
        return FALSE;
    } /* if */
    return TRUE;
}

/*!
 * \brief Exit the SMIL creation.
 *
 * \param showDialog TRUE if an error dialog should be displayed, otherwise
 *                   FALSE.
 * \param strId The string WID of the text to display in the dialog.
 *****************************************************************************/
static void exitSmilCreate(WE_BOOL showDialog, WE_UINT32 strId)
{
    /* Display dialog */
    if (showDialog)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(strId), MSA_DIALOG_ERROR);
    }
    /* terminate ME och SE */
    msaMeTerminateGui();
    msaSeTerminateGui();
    (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
    (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_DEACTIVATE);
} /* exitSmilCreate */

/*! \brief Adds a media object to a SMIL structure and removes the old object
 *         if any.
 *
 * \param smilInfo the SMIL structure to insert the media object into
 * \param propItem properties of the message to modify
 * \param objData the media object and information about it
 * \param readResult the result of the load object operation
 * \return MSA_SIZE_INFO_OK, otherwise how a dialog should be shown
 *****************************************************************************/
static MsaSeverity setMediaObject(MsaSmilInfo *smilInfo, 
    const MsaPropItem *propItem, MsaAfiReadResultData *objData, 
    MsaAfiReadResult readResult)
{
	WE_UINT32          newMsgSize;
    unsigned int        oldMoSize = 0;
    unsigned int        newMoSize = 0;
    MsaMoDataItem       *item;
    MsaSeverity         result;
    MmsContentType      *contentType;

    if ((NULL == objData))
    {
        return MSA_SEVERITY_ERROR;
    }
    if (NULL != objData->mimeType) 
    {
        if (NULL != (item = msaGetMo(smilInfo, msaMimeToObjGroup(objData->mimeType))))
        {
            oldMoSize = item->size;
            /* Delete text object if nothing was entered */
            if (!objData->size && MSA_MEDIA_GROUP_TEXT == 
                msaMimeToObjGroup(objData->mimeType))
            {
                (void)msaDeleteSlideMo(smilInfo, msaMimeToObjGroup(objData->mimeType));
            }
        }
        /* Calculate the new size of the message */
        newMoSize = (NULL != objData) ? objData->size : 0;
    } /* if */

    if ((MSA_AFI_READ_RESULT_TO_LARGE != readResult) && /* handled below */
        (MSA_MEDIA_GROUP_NONE == msaMimeToObjGroup(objData->mimeType)))
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_READ_MO),
            MSA_DIALOG_ERROR);
        return MSA_SEVERITY_WARNING;
    } /* if */

    /* The size the message will get after inclusion of this media object */
	newMsgSize = msaCalculateMsgSize(smilInfo, propItem) 
        - oldMoSize + newMoSize;

    /* compare message and object sizes */
    result = msaShowSizeWarningDialog(MSA_AFI_READ_RESULT_TO_LARGE == readResult
        ? 0xffffffff : newMsgSize, newMoSize, msaMimeToObjGroup(objData->mimeType), 
        MSA_NOTUSED_FSM, 0);
    if (result == MSA_SEVERITY_ERROR)
    {
        return MSA_SEVERITY_WARNING;
    }
    if (NULL == objData->fileName)
    {
        if (NULL == (objData->fileName = msaCreateFileNameFromType(objData->mimeType)))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_READ_MO),
                MSA_DIALOG_ERROR);
            return MSA_SEVERITY_ERROR;
        }
    }
    msaRemPathFromFileName(objData->fileName);

    MSA_CALLOC(contentType, sizeof(MmsContentType));
    contentType->knownValue = MMS_VALUE_AS_STRING;
    contentType->strValue = (unsigned char *)objData->mimeType;

    if (objData->size && (NULL == msaSetSlideMo(smilInfo, contentType,
        objData->data, objData->size, objData->fileName)))
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_READ_MO),
            MSA_DIALOG_ERROR);
        MMSif_freeMmsContentType(WE_MODID_MSA, contentType, TRUE);
        return MSA_SEVERITY_ERROR;
    }
    /* The data is consumed by the msaSetSlideMo, set pointers to NULL */
    objData->data = NULL;
    objData->fileName = NULL;
    objData->mimeType = NULL;

    msaOptimizeMoList(smilInfo);
    return result;
}

/*! \brief Handles the activation of the SMIL editor
 *	
 * \param msg The current message.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL handleActivate(const MsaMessage *msg)
{
    /* No regions defined */
    if ((NULL == msg) || (NULL == msg->smilInfo))
    {
        return FALSE;
    }
   
    /* Check if a slide needs to be inserted */
    if (0 == msaGetSlideCount(currentMsg->theMessage->smilInfo))
    {
        /* Insert the first slide */
        if (NULL == msaInsertSlide(currentMsg->theMessage->smilInfo))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Insert slide failed in message activation\n", 
                __FILE__, __LINE__));
            return FALSE;
        } 
    }
    /* Display the editor */
    if (!msaSeCreateEditorView())
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Creation of editor view failed!\n", __FILE__, __LINE__));
        return FALSE;
    } 
    else if (!msaSeShowSlide(currentMsg->theMessage->smilInfo, 
        currentMsg->theMessage->msgProperties))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Show slide failed in!\n", __FILE__, __LINE__));
        return FALSE;
    }
    return TRUE;
}

/*! \brief Handles to load a media object from persistent storage.
 *
 * \param msg the current message.
 * \param dlgResult The result from the open dialog.
 * \param fileName The file name of the object to load.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static void handleGetMediaObject(const MsaMessage *msg, int dlgResult, 
    char *fileName)
{
    WE_UINT32 spaceLeft = 0;
    WE_UINT32 size;
    if (WE_DLG_RETURN_OK == dlgResult)
    {
        /*
         *	No use loading media objects larger than;
         *  a) the #MSA_CFG_MESSAGE_SIZE_MAXIMUM barrier, and 
         *  b) the maximum media object size; #MSA_CFG_INSERT_MEDIA_OBJECT_MAX_SIZE
         */
        size = MSA_CFG_MESSAGE_SIZE_MAXIMUM - 
            msaCalculateMsgSize(msg->smilInfo, msg->msgProperties);
        spaceLeft = MIN(size, MSA_CFG_INSERT_MEDIA_OBJECT_MAX_SIZE);
        
        if (!msaAfiLoadFromFile(MSA_SE_FSM, MSA_SIG_SE_GET_MEDIA_OBJECT_RSP, 
            fileName, NULL, spaceLeft))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_FAILED_TO_READ_MO), 
                MSA_DIALOG_ERROR);
            msaSeSetBusy(FALSE);
        }
    }
    else
    {
        msaSeSetBusy(FALSE);
    }
}

/*! \brief insert a new slide into the SMIL.
 *	
 * \param msg The current message.
 * \return TRUE on success, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL insertSlide(const MsaMessage *msg)
{
    if ((NULL == msg) || (NULL == msg->smilInfo))
    {
        return FALSE;
    }
    if (msaGetSlideCount(msg->smilInfo) < MSA_CFG_SLIDE_LIMIT)
    {
        if(NULL == msaInsertSlide(msg->smilInfo))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Insertion of slide failed!\n", __FILE__, __LINE__));
            return FALSE;
        } 
        if(!msaSeShowSlide(msg->smilInfo, msg->msgProperties))
        {
            return FALSE;
        }
    }
    else
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_SLIDE_LIMIT_REACHED), 
            MSA_DIALOG_ALERT);
    }
    return TRUE;
}

/*! \brief Deles a slide in the current SMIL presentation.
 *	
 * \param msg The current message.
 * \return TRUE on success, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL deleteSlide(const MsaMessage *msg)
{
    if ((NULL == msg) || (NULL == msg->smilInfo))
    {
        return FALSE;
    }
    if (!msaDeleteSlide(msg->smilInfo))
    {
        return FALSE;
    }
    if (msaGetSlideCount(currentMsg->theMessage->smilInfo) < 1)
    {
        /* Insert a new slide if there are no more slides */
        if (NULL == msaInsertSlide(currentMsg->theMessage->smilInfo))
        {
            return FALSE;
        }
    }
    if(!msaSeShowSlide(currentMsg->theMessage->smilInfo, 
        currentMsg->theMessage->msgProperties))
    {
        return FALSE;
    }
    return TRUE;
}

/*! \brief Changes the curently active slide.
 *	
 * \param msg The current message.
 * \param next TRUE if the next slide should be displayed. FALSE for the 
 *             previous slide.
 *****************************************************************************/
static void changeSlide(const MsaMessage *msg, WE_BOOL next)
{
    MsaSmilSlide *changeTo;
    if (NULL == msg->smilInfo)
    {
        return;
    }
    changeTo = (next) ? msaGetNextSlide(msg->smilInfo): 
        msaGetPreviousSlide(msg->smilInfo);

    if (NULL == changeTo)
    {
        return;
    }
    if(!msaSeShowSlide(msg->smilInfo, msg->msgProperties))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(5s) (%d) Show slide failed!\n", __FILE__, __LINE__));
        exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
    } /* if */
}

/*! \brief Handles the creation of the item timing GUI.
 *
 * \param msg The current message.
 * \param gType The type of media to change the timing for.
 *****************************************************************************/
static void handleItemTiming(const MsaMeInstance *msg, MsaMediaGroupType gType)
{
    MsaObjInfo *objInfo;
    MsaSmilSlide *sSlide;
    WE_UINT32 objDuration;

    if ((NULL != msg->theMessage) && (NULL != msg->theMessage->smilInfo))
    {
        if (!msaGetObjInfo(msg->theMessage->smilInfo, gType, &objInfo))
        {
            return ;
        }
        if (NULL == (sSlide = msaGetCurrentSlide(msg->theMessage->smilInfo)))
        {
            return;
        }
        objDuration = objInfo->duration == 0 ?
                        sSlide->duration - objInfo->begin:
                        objInfo->duration;
        if (!msaSeCreateItemTiming(objInfo->begin, objDuration, 
            sSlide->duration, gType))
        {
            /* Make sure that everything is deleted */
            msaSeDeleteItemTiming();
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Failed to create item timing menu!\n", __FILE__, 
                __LINE__));
        }
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Failed to create item timing menu!\n", __FILE__, 
            __LINE__));
    }
}

/*!
 * \brief The SMIL editor signal handling logic
 *
 * \param sig The received signal
 *****************************************************************************/
static void seSignalHandler(MsaSignal *sig)
{
    MsaSmilSlide            *smilSlide;
    MsaAfiReadResultData    *objData;
    MsaAfiReadResult        readResult;
    MsaObjInfo              *objInfo;

    /* Verify that a valid instance is used */
    if ((sig->type != MSA_SIG_SE_ACTIVATE) && (NULL == currentMsg))
    {
        msaSignalDelete(sig);
        return;
    }

    switch(sig->type)
    {
    case MSA_SIG_SE_ACTIVATE:
        currentMsg = sig->p_param;
        if (!handleActivate(currentMsg->theMessage))
        {
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
        }
        break;

    case MSA_SIG_SE_GET_MEDIA_OBJECT:
        if (NULL == currentMsg->theMessage)
        {
            exitSmilCreate(FALSE, 0);
            break;
        }
        handleGetMediaObject(currentMsg->theMessage, (int)sig->u_param1, 
            sig->p_param);
        break;

    case MSA_SIG_SE_GET_MEDIA_OBJECT_RSP:
        /*
         *	Data has been fetched for inclusion into MM.
         */
        objData = (MsaAfiReadResultData *)sig->p_param;
        readResult = (MsaAfiReadResult)sig->u_param1; 
        if ((MSA_SEVERITY_ERROR == setMediaObject(currentMsg->theMessage->smilInfo, 
            currentMsg->theMessage->msgProperties, 
            objData, readResult)) || !msaSeShowSlide(currentMsg->theMessage->smilInfo,
            currentMsg->theMessage->msgProperties))
        {
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            break;
        } 
        msaSeSetBusy(FALSE);
        msaAfiLoadFreeData(objData);
        break;

    case MSA_SIG_SE_HANDLE_OBJECT:
        if (!editMediaObject(currentMsg->theMessage, 
            (MsaMediaGroupType)sig->u_param1))
        {
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            msaSeSetBusy(FALSE);
        }
        break;
        
    case MSA_SIG_SE_DEACTIVATE:
        currentMsg = NULL;
        msaSeTerminateGui();
        break;

    case MSA_SIG_SE_INSERT_SLIDE:
        if (!insertSlide(currentMsg->theMessage))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "Show slide failed in (%d)\n", sig->type));
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
        }
        break;

    case MSA_SIG_SE_DELETE_SLIDE:
        if (!deleteSlide(currentMsg->theMessage))
        {
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            break;
        }
        break;

    case MSA_SIG_SE_NEXT_SLIDE:
        changeSlide(currentMsg->theMessage, TRUE);
        break;

    case MSA_SIG_SE_PREVIOUS_SLIDE:
        changeSlide(currentMsg->theMessage, FALSE);        
        break;

    case MSA_SIG_SE_REM_OBJECT:
        if (!msaDeleteSlideMo(currentMsg->theMessage->smilInfo, 
            (MsaMediaGroupType)sig->u_param1))
        {
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            break;
        }
        if (!msaSeShowSlide(currentMsg->theMessage->smilInfo, 
            currentMsg->theMessage->msgProperties))
        {
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            break;
        }
        msaSeSetBusy(FALSE);
        break;

    /*
     *	PREVIEW
     */
    case MSA_SIG_SE_PREVIEW_SMIL:
        /* Create a new message */
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE_PREVIEW, 
            MSA_SE_FSM, MSA_SIG_SE_PREVIEW_SAVE_DONE);
        msaSeSetBusy(TRUE);
        break;

    case MSA_SIG_SE_PREVIEW_SAVE_DONE:
        /* Start the SIA */
        if ((WE_BOOL)sig->u_param1)
        {
            /*
             *	Start the SIA in order to play the message, all preview 
             *  messages are of type MMS_SUFFIX_SEND_REQ
             */
            msaPlaySmil(currentMsg->theMessage->msgId, 
                currentMsg->theMessage->fileType, MSA_SE_FSM, 
                MSA_SIG_SE_PREVIEW_DONE);
        }
        else
        {
            /* Failed to save, ME will have shown an error message  */
            msaSeSetBusy(FALSE);
        }
        break;

    case MSA_SIG_SE_PREVIEW_DONE:
        /* The preview has been performed. Delete the temporary message */
        if (NULL != currentMsg->theMessage)
        {
            msaDeleteMsg(currentMsg->theMessage->msgId, FALSE);
            currentMsg->theMessage->msgId = 0;
        }
        msaSeSetBusy(FALSE);
        break;

    /*
     *	SAVING/SENDING
     */
    case MSA_SIG_SE_SAVE:
        msaSeSetBusy(TRUE);
        /* Check for "edit template" */
        if ((currentMsg->origMsgId != 0) && 
            (MMS_SUFFIX_TEMPLATE == currentMsg->origFileType))
        {

            /* If editing a template and trying to save the content, it should
               then be saved as a template */
            (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE_TEMPLATE, 
                MSA_SE_FSM, MSA_SIG_SE_SMIL_SAVE_RSP);
        }
        else
        {
            /* Perform an ordinary save */
            (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE, MSA_SE_FSM, 
                MSA_SIG_SE_SMIL_SAVE_RSP);
        }
        break;
        
    case MSA_SIG_SE_SAVE_AS_TEMPLATE:
        msaSeSetBusy(TRUE);
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE_AS_TEMPLATE, 
            MSA_SE_FSM, MSA_SIG_SE_SMIL_SAVE_RSP);
        break;
        
    case MSA_SIG_SE_SAVE_TEMPLATE:
        msaSeSetBusy(TRUE);
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SAVE_TEMPLATE, 
            MSA_SE_FSM, MSA_SIG_SE_SMIL_SAVE_RSP);
        break;

    case MSA_SIG_SE_SEND:
        msaSeSetBusy(TRUE);
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SEND, MSA_SE_FSM, 
            MSA_SIG_SE_SMIL_SAVE_RSP);
        break;

    case MSA_SIG_SE_SMIL_SAVE_RSP:
        msaSeSetBusy(FALSE);
        if ((WE_BOOL)sig->u_param1)
        {    
            /*
             *	All done, let's shut down. 
             *  Any error messages will have been shown by ME.
             */
            exitSmilCreate(FALSE, 0);
        } /* if */        
        break;        
    
    /*
     *	MENU interaction
     */
    case MSA_SIG_SE_NAV_MENU_ACTIVATE:
        if ((NULL != currentMsg->theMessage) &&
            (NULL != currentMsg->theMessage->smilInfo))
        {
            smilSlide = msaGetCurrentSlide(currentMsg->theMessage->smilInfo);
            if (NULL != smilSlide)
            {
                if (!msaSeCreateNavMenu( 
                    (signed)msaGetSlideCount(currentMsg->theMessage->smilInfo), 
                    msaIsSmilValid(currentMsg->theMessage->smilInfo), 
                    currentMsg->mode, currentMsg->origFileType))
                {
                    msaSeDeleteNavMenu();
                }
            }
            else
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "Get current slide failed in (%d)\n", sig->type));
                exitSmilCreate(FALSE, 0);
            } /* if */
        }
        break;

    case MSA_SIG_SE_NAV_MENU_DEACTIVATE:
        msaSeDeleteNavMenu();
        break;

        /* Colors
         *	
         */

    case MSA_SIG_SE_SET_BG_COLOR:
        if (!msaCreateColorSelector(bgColors, sizeof(bgColors)/sizeof(bgColors[0]), 
            MSA_COLOR_NOT_VISIBLE, currentMsg->theMessage->smilInfo->bgColor, 
            MSA_SE_FSM, MSA_SIG_SE_SET_BG_COLOR_RSP))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) failed to create the color selection dialog!\n"));
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
        }
        break;

    case MSA_SIG_SE_SET_BG_COLOR_RSP:
        if ((NULL != currentMsg->theMessage) && 
            (NULL != currentMsg->theMessage->smilInfo))
        {
            currentMsg->theMessage->smilInfo->bgColor = (WE_UINT32)sig->u_param1;
            if (!msaSeShowSlide(currentMsg->theMessage->smilInfo, 
                currentMsg->theMessage->msgProperties))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) failed to update the SMIL editor GUI!\n"));
                exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            }
        }
        break;

    case MSA_SIG_SE_SET_TEXT_COLOR:
        if (!msaGetObjInfo(currentMsg->theMessage->smilInfo, MSA_MEDIA_GROUP_TEXT, &objInfo) ||
            (!msaCreateColorSelector(txtColors, sizeof(txtColors)/sizeof(txtColors[0]), 
            MSA_COLOR_NOT_VISIBLE, objInfo->textColor, MSA_SE_FSM, 
            MSA_SIG_SE_SET_TEXT_COLOR_RSP)))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) failed to create the color selection dialog!\n"));
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
        }
        break;

    case MSA_SIG_SE_SET_TEXT_COLOR_RSP:
        if ((NULL != currentMsg->theMessage) && 
            (NULL != currentMsg->theMessage->smilInfo) &&
            (msaGetObjInfo(currentMsg->theMessage->smilInfo, 
            MSA_MEDIA_GROUP_TEXT, &objInfo)))
        {
            objInfo->textColor = (WE_UINT32)sig->u_param1;
            if (!msaSeShowSlide(currentMsg->theMessage->smilInfo, 
                currentMsg->theMessage->msgProperties))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) failed to update the SMIL editor GUI!\n"));
                exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            }
        }
        break;

         case MSA_SIG_SE_SET_TEXT_COLOR_ALL:
        if (!msaGetObjInfo(currentMsg->theMessage->smilInfo, MSA_MEDIA_GROUP_TEXT, &objInfo) ||
            (!msaCreateColorSelector(txtColors, sizeof(txtColors)/sizeof(txtColors[0]), 
            MSA_COLOR_NOT_VISIBLE, objInfo->textColor, MSA_SE_FSM, 
            MSA_SIG_SE_SET_TEXT_COLOR_ALL_RSP)))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) failed to create the color selection dialog!\n"));
            exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
        }
        break;

    case MSA_SIG_SE_SET_TEXT_COLOR_ALL_RSP:
        if (NULL != currentMsg) 
        {
            
            MsaSmilSlide* slide=currentMsg->theMessage->smilInfo->slides;
            currentMsg->theMessage->smilInfo->fgColorDefault=(WE_UINT32)sig->u_param1;
            while(slide!=NULL)
            {
                int i=0;
                for(i;i<MSA_MAX_SMIL_OBJS;i++)     
                {
                    if(slide->objects[i]!=NULL)
                    {
                        slide->objects[i]->textColor= (WE_UINT32)sig->u_param1;
                    }
                }
                
                slide=slide->next;
            }
            
            
            if (!msaSeShowSlide(currentMsg->theMessage->smilInfo, 
                currentMsg->theMessage->msgProperties))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) failed to update the SMIL editor GUI!\n"));
                exitSmilCreate(TRUE, MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED);
            }
        }
        break;

        /* Item timing
         *	
         */
    case MSA_SIG_SE_SET_ITEM_TIMING:
        handleItemTiming(currentMsg, (MsaMediaGroupType)sig->u_param1);
        break;

        /* The text renderer (MTR)
         *	
         */

    case MSA_SIG_SE_GADGET_GOT_FOCUS:
        msaSeHandleNotif(WeNotifyFocus);
        break;

    case MSA_SIG_SE_GADGET_LOST_FOCUS:
        msaSeHandleNotif(WeNotifyLostFocus);
        break;

    case MSA_SIG_SE_EVENT_KEY_DOWN:
        msaSeHandleEvent(WeKey_Down);
        break;

    case MSA_SIG_SE_EVENT_KEY_UP:
        msaSeHandleEvent(WeKey_Up);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "seSignalHandler erroneous signal received(%d)\n", sig->type));
        break;
    }

    msaSignalDelete(sig);
} /* seSignalHandler */

/*!
 * \brief Changes the duration for the current slide.
 *
 * \param duration A string with the new duration value.
 * \param notUsed Not used.
 * \param dataPtr Not used.
 * \param uintValue Not used.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaSeHandleDuration(char *duration, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue)
{
    int value;
    WE_BOOL ret = FALSE;
    if ((0 != notUsed) && (NULL != dataPtr) && (0 != uintValue))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) erroneous default value!\n", __FILE__, __LINE__));
    }

    if (NULL != duration)
    {
        value = atoi(duration);
        if ((value >= MSA_CFG_MIN_DURATION) && (value < MSA_CFG_MAX_DURATION))
        {
            msaSetDuration(currentMsg->theMessage->smilInfo, (unsigned)value*1000);
            ret = TRUE;
        }
        else
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
                MSA_DIALOG_ERROR);
        }
    }
    MSA_FREE(duration);
    return ret;
}

/*!
 * \brief Gets the duration for the current slide.
 *
 * \return The duration for the current slide.
 *****************************************************************************/
unsigned int msaSeGetDuration(void) 
{
    unsigned int value = MSA_CFG_DEFAULT_DURATION;
    if ((NULL != currentMsg) && (NULL != currentMsg->theMessage) && 
        (NULL != currentMsg->theMessage->smilInfo))
    {
        value = msaGetDuration(currentMsg->theMessage->smilInfo);
    }
    else
    {
        exitSmilCreate(FALSE, 0);
    }
    return value;
}

/*!
 * \brief Initiates the SMIL editor by registering its signal handler and 
 *          initiating the data structures used for handling SMIL slides
 *****************************************************************************/
void msaSeInit(void)
{
    msaSignalRegisterDst(MSA_SE_FSM, seSignalHandler);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
        "SE FSM initialised!\n"));
} /* msaSeInit */

/*!
 * \brief Terminates the SMIL editor by de-registering its signal handler and 
 *          freeing all data structures used
 *****************************************************************************/
void msaSeTerminate(void)
{
    msaSignalDeregister(MSA_SE_FSM);
    currentMsg = NULL;
    /* Delete all dialogs */
    msaSeTerminateGui();
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, "SE FSM terminated!\n"));
} /* msaSeTerminate */

/*!
 * \brief Terminates the GUI for the SMIL editor.
 *****************************************************************************/
void msaSeTerminateGui()
{
    msaSeDeleteEditorView();
    (void)msaOpenDlgMediaInputAbort();
    (void)msaAfiLoadFromFileAbort();
    msaSeDeleteItemTiming();
    msaDeleteColorSelector();
    msaSeDeleteNavMenu();
}

/*! \brief Handles the start item timing changes.
 *
 * \param str The new value as a string.
 * \param notUsed
 * \param dataPtr Not used.
 * \param uintValue The media type, see #MsaMediaGroupType.
 *****************************************************************************/
WE_BOOL msaSeHandleObjStart(char *str, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue)
{
    MsaSmilSlide *sSlide;
    int start = 0;
    WE_UINT32 newStart;
    WE_UINT32 newDuration;
    WE_UINT32 oldDuration;

    (void)dataPtr;
    (void)notUsed;
    /* Find the current object */
    if ((NULL != str) && (NULL != currentMsg) && (NULL != currentMsg->theMessage))
    {
        if (NULL == (sSlide = msaGetCurrentSlide(currentMsg->theMessage->smilInfo)))
        {
            return FALSE;
        }
        msaGetObjDuration(currentMsg->theMessage->smilInfo, uintValue, &oldDuration);
        /* Boundary check */
        if ((start = atoi(str)*1000) < 0)
        {
            start = 0;
        }
        if (!msaSetObjStart(currentMsg->theMessage->smilInfo, uintValue, start))
        {
            return TRUE;
        }
        (void)msaGetObjStart(currentMsg->theMessage->smilInfo, uintValue, &newStart);
        (void)msaGetObjDuration(currentMsg->theMessage->smilInfo, uintValue, &newDuration);
        /* Check if the desired timing was set or if the start or duration
           was changed in order to be within page timing limits */
        if ((newStart != (WE_UINT32)start) || (oldDuration != newDuration))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_TIMING_OUT_OF_BOUNDS), 
                MSA_DIALOG_WARNING);
        }
        msaSeUpdateItemTiming(newStart, newDuration, sSlide->duration);
    }
    return TRUE;
}

/*! \brief Handles the item duration changes.
 *
 * \param str The new value as a string.
 * \param notUsed
 * \param dataPtr Not used.
 * \param uintValue The media type, see #MsaMediaGroupType.
 *****************************************************************************/
WE_BOOL msaSeHandleObjDur(char *str, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue)
{
    MsaSmilSlide *sSlide;
    int duration;
    WE_UINT32 oldStart;
    WE_UINT32 newStart;
    WE_UINT32 newDuration;

    (void)dataPtr;
    (void)notUsed;

    /* Find the current object */
    if ((NULL != str) && (currentMsg) && (NULL != currentMsg->theMessage))
    {
        if (NULL == (sSlide = msaGetCurrentSlide(currentMsg->theMessage->smilInfo)))
        {
            return FALSE;
        }
        (void)msaGetObjStart(currentMsg->theMessage->smilInfo, uintValue, &oldStart);
        /* Boundary check */
        if ((duration = atoi(str) * 1000) < 0)
        {
            duration = MSA_OBJ_DURATION_MIN;
        }
        if (!msaSetObjDuration(currentMsg->theMessage->smilInfo, uintValue, 
            duration))
        {
            return TRUE;
        }
        /* Update the GUI */
        (void)msaGetObjStart(currentMsg->theMessage->smilInfo, uintValue, &newStart);
        (void)msaGetObjDuration(currentMsg->theMessage->smilInfo, uintValue, &newDuration);
        /* Check if the desired timing was set or if the start or duration
           was changed in order to be within page timing limits */
        if ((newStart != oldStart) || (newDuration != (WE_UINT32)duration))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_TIMING_OUT_OF_BOUNDS), 
                MSA_DIALOG_WARNING);
        }
        msaSeUpdateItemTiming(newStart, newDuration, sSlide->duration);
    }
    return TRUE;
}

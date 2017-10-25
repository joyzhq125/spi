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

/* !\file mauimme.c
 *  \brief Functionality for the Multipart/mixed editor UI.
 */

/* WE */
#include "We_Mem.h"
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Lib.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Uimenu.h"
#include "Msa_Mme.h"
#include "Msa_Mr.h"
#include "Msa_Uisig.h"
#include "Msa_Uicmn.h"
#include "Msa_Uimme.h"
#include "Msa_Ph.h"
#include "Msa_Utils.h"

/******************************************************************************
 * Definitions of internal data-structures for the GUI.
 *****************************************************************************/
/*! \struct MsaMmeEditor
 *  Main view 
 */
typedef struct
{
    MsaMessage          *msg;
    MsaMoDataItem       *mo;
    MsaMediaGroupType   gType;
    WE_UINT32          audioHandle;
}MsaMmeEditor;

/*! \struct MsaMmeTextView
 *  Text editing view
 */
typedef struct
{
    WeWindowHandle formHandle;
}MsaMmeTextView;

/*! \struct MsaMmeMenuItem
 *	
 */
typedef struct  
{
    WE_UINT32  strId;      /*!< String to display in menu */
    int         signal;     /*!< Signal to send on selection */
    int         mediaGroup; /*!< Show menu entry for these media groups */
    MmsFileType blockedType;/*!< Tells if a menu alternative needs to be blocked
                                 for a specific message-type */
    MmsFileType blockedType2;/*!< Tells if a menu alternative needs to be blocked
                                 for a specific message-type */
}MsaMmeMenuItem;

/******************************************************************************
 * Constants
 *****************************************************************************/
/* Shorthand for "any type of object" */
#define ALL_MEDIA_GROUPS (MSA_MEDIA_GROUP_NONE | MSA_MEDIA_GROUP_IMAGE | \
    MSA_MEDIA_GROUP_TEXT | MSA_MEDIA_GROUP_AUDIO)
/* The maximum number of characters in the text editor */
#define MSA_MME_TEXT_MAX_SIZE            512

/* Menu items for the navigation menu */
static const MsaMmeMenuItem menuItems[] = 
{
    {MSA_STR_ID_ADD_TEXT, MSA_MME_ADD_TEXT, MSA_MEDIA_GROUP_NONE, MMS_SUFFIX_ERROR, MMS_SUFFIX_ERROR},
    {MSA_STR_ID_ADD_MEDIA_OBJECT, MSA_MME_ADD_MEDIA_OBJECT, MSA_MEDIA_GROUP_NONE, MMS_SUFFIX_ERROR, MMS_SUFFIX_ERROR},
    {MSA_STR_ID_REM_MEDIA_OBJ, MSA_MME_DELETE_MEDIA_OBJECT, ALL_MEDIA_GROUPS, MMS_SUFFIX_ERROR, MMS_SUFFIX_ERROR},
    {MSA_STR_ID_EDIT_TEXT, MSA_MME_EDIT_TEXT, MSA_MEDIA_GROUP_TEXT, MMS_SUFFIX_ERROR, MMS_SUFFIX_ERROR},
    {MSA_STR_ID_PLAY_SOUND, MSA_MME_PLAY_SOUND, MSA_MEDIA_GROUP_AUDIO, MMS_SUFFIX_ERROR, MMS_SUFFIX_ERROR},
    {MSA_STR_ID_SAVE, MSA_MME_SAVE, MSA_MEDIA_GROUP_NONE, MMS_SUFFIX_MSG, MMS_SUFFIX_ERROR},  /* Block save for retrieve conf. */
    {MSA_STR_ID_SAVE_AS_TEMPLATE, MSA_MME_SAVE_AS_TEMPLATE, MSA_MEDIA_GROUP_NONE, MMS_SUFFIX_ERROR},
    {MSA_STR_ID_SAVE_TEMPLATE, MSA_MME_SAVE_TEMPLATE, MSA_MEDIA_GROUP_NONE, MMS_SUFFIX_ERROR},
    {MSA_STR_ID_SEND, MSA_MME_SEND, MSA_MEDIA_GROUP_NONE, MMS_SUFFIX_MSG, MMS_SUFFIX_TEMPLATE}, /* Block send for retrieve conf. and templates */
    {MSA_STR_ID_BACK, MSA_MME_BACK, MSA_MEDIA_GROUP_NONE, MMS_SUFFIX_ERROR, MMS_SUFFIX_ERROR},
    {0,0,0}        
};

/******************************************************************************
 * Internal data-structures for the GUI.
 *****************************************************************************/
static MsaMmeEditor   msaMmeEditorView;
static MsaMmeTextView msaMmeTextView;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Initiates internal data-structures for the MME.
 *****************************************************************************/
void msaMmeUiInit(void)
{
    memset(&msaMmeEditorView, 0, sizeof(MsaMmeEditor));
    msaMmeEditorView.gType = MSA_MEDIA_GROUP_NONE;
} /* msaMmeUiInit */

/*!
 * \brief Initiates message pointer for MME UI
 * \param msg The message being edited
 *****************************************************************************/
void msaMmeUiSetMsg(MsaMessage *msg)
{
    msaMmeEditorView.msg = msg;        
}

/*!
 * \brief Signal converter callback function for the context sensitive menu.
 *
 * \param actionType The type of action that was generated.
 *****************************************************************************/
static void navMenuSignalConverter(MsaNavAction actionType)
{
    if (MSA_NAV_ACTION_BACK == actionType)
    {
        (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_NAV_MENU_DEACTIVATE);
    }

    if (MSA_NAV_ACTION_OK == actionType)
    {
        switch(msaGetNavMenuItem())
        {
        case MSA_MME_ADD_TEXT:
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_ADD_TEXT);
            break;
        case MSA_MME_ADD_MEDIA_OBJECT:       /*!< Add MO to the message */
            (void)msaOpenDlgCreateMediaInput(MSA_MME_FSM, 
                MSA_SIG_MME_GET_MEDIA_OBJECT, 
                MSA_CFG_OPEN_IMAGE_TYPES","MSA_CFG_OPEN_TEXT_TYPES",\
                "MSA_CFG_OPEN_AUDIO_TYPES);
            break;
        case MSA_MME_DELETE_MEDIA_OBJECT:    /*!< Delete MO from the message */
            (void)MSA_SIGNAL_SENDTO_P(MSA_MME_FSM, MSA_SIG_MME_DELETE_MO, 
                msaMmeEditorView.mo);
            break;
        case MSA_MME_REPLACE_MEDIA_OBJECT:   /*!< Replace MO in the message */
            (void)msaOpenDlgCreateMediaInput(MSA_MME_FSM, 
                MSA_SIG_MME_GET_MEDIA_OBJECT_REPLACEMENT, 
                MSA_CFG_OPEN_IMAGE_TYPES","MSA_CFG_OPEN_TEXT_TYPES",\
                "MSA_CFG_OPEN_AUDIO_TYPES);
            break;
        case MSA_MME_EDIT_TEXT:              /*!< Edit text */
            (void)MSA_SIGNAL_SENDTO_P(MSA_MME_FSM, MSA_SIG_MME_EDIT_TEXT, 
                msaMmeEditorView.mo);
            break;
        case MSA_MME_PLAY_SOUND:             /*!< Play sound */
            (void)MSA_SIGNAL_SENDTO_P(MSA_MME_FSM, MSA_SIG_MME_PLAY_SOUND, 
                msaMmeEditorView.mo);
                break;
        case MSA_MME_SAVE:                   /*!< Save message */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_SAVE);
            break;
        case MSA_MME_SAVE_AS_TEMPLATE:       /*!< Save message as template */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_SAVE_AS_TEMPLATE);
            break;
        case MSA_MME_SAVE_TEMPLATE:       /*!< Save template */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_SAVE_TEMPLATE);
            break;
        case MSA_MME_SEND:                   /*!< Send message */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_SEND);
            break;
        case MSA_MME_PREVIEW:                /*!< Preview message */
            break;
        case MSA_MME_BACK:                   /*!< Go back */
            (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_DEACTIVATE);
            break;
        default:
            break;
        }
        (void)MSA_SIGNAL_SENDTO(MSA_MME_FSM, MSA_SIG_MME_NAV_MENU_DEACTIVATE);
    }
} /* navMenuSignalConverter */

/*!
 * \brief Creates menu items for the navigation menu for the MME.
 * \return TRUE on OK else FALSE
 *****************************************************************************/
static WE_BOOL createMenuItems(MmsFileType msgType)
{
    int i = 0;

    /* Iterate through menu items */
    for (i=0; menuItems[i].strId != 0; i++)
    {
        /* add menu entry if suitable for selected media object */
        if (((menuItems[i].mediaGroup == MSA_MEDIA_GROUP_NONE) || 
            (msaMmeEditorView.gType & menuItems[i].mediaGroup) ||
            (msaMmeEditorView.gType == MSA_MEDIA_GROUP_NONE && msaMmeEditorView.mo != NULL)) && 
            (menuItems[i].blockedType != msgType) &&
            (menuItems[i].blockedType2 != msgType) &&
            (! msaAddNavMenuItem(MSA_GET_STR_ID(menuItems[i].strId), 
            menuItems[i].signal)))
        {
            return FALSE;
        } /* if */
    } /* for */

    return TRUE;
} /* createMenuItems */

/*!
 * \brief Creates a navigation menu for the MME.
 * \param index the selected object on previous screen
 * \return TRUE on OK else FALSE
 *****************************************************************************/
WE_BOOL msaUiMmeCreateNavMenu(int index, MmsFileType msgType)
{
    int i;

    /* Find selected mo, get media group info */
    msaMmeEditorView.mo = NULL;
    msaMmeEditorView.gType = MSA_MEDIA_GROUP_NONE;
    if ((-1 != index) && 
        (msaMmeEditorView.msg) &&
        (msaMmeEditorView.msg->smilInfo) &&
        (msaMmeEditorView.msg->smilInfo->mediaObjects))
    {
        msaMmeEditorView.mo = msaMmeEditorView.msg->smilInfo->mediaObjects;
        for (i=0; i<index; i++)
        {
            msaMmeEditorView.mo = msaMmeEditorView.mo->next;
        } /* for */
        msaMmeEditorView.gType = 
            msaMimeToObjGroup((char *)msaMmeEditorView.mo->type->strValue);
    } /* if */
    

    /* Create navigation menu */
    if (!msaCreateNavMenu(0, navMenuSignalConverter))
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    else if (!createMenuItems(msgType))
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    else if (!msaShowNavMenu())
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    return TRUE;
} 

/*!
 * \brief Deletes the navigation menu.
 *****************************************************************************/
void msaUiMmeDeleteNavMenu(void)
{
    msaRemoveNavMenu();
} 

/*!
 * \brief Returns selected MO in UI
 * \return selected MO in UI
 *****************************************************************************/
MsaMoDataItem *msaUiMmeGetSelectedMo(void)
{
    return msaMmeEditorView.mo;
} 

/*!
 * \brief Creates the text edit view.
 *
 * \param text The initial text.
 * \param subject The window title
 * \param retFsm The FSM to receive signals on button presses
 * \param retSigOk The signal to receive on OK presses
 * \param retSigBack The signal to receive on Back presses
 * \param retSigMenu The signal to receive on Menu presses
 * \return TRUE if the view was created successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaUiMmeCreateTextView(const char *text, const char *subject, 
    MsaStateMachine retFsm, int retSigOk, int retSigBack, int retSigMenu)
{
    WeStringHandle textStringHandle;
    WeStringHandle strHandle;
    const char      *tmpStr;
    WE_UINT32      maxLength;

    if (0 != msaMmeTextView.formHandle)
    {
        return FALSE; /* already running.. */
    }
    memset(&msaMmeTextView, 0, sizeof(MsaMmeTextView));
    
    /* Create text */
    tmpStr = (NULL != text) ? (char *)text: "";
    if (0 == (textStringHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
        tmpStr, WeUtf8, (int)strlen(tmpStr) + 1, 0)))
    {
        return FALSE;
    }
    /* Checks if the total number of character to input needs to be higher
     * than default. This can be the case if a retrieved message is used as a
     * template.
     */
    if ((maxLength = WE_WIDGET_STRING_GET_LENGTH(textStringHandle, 0, WeUtf8)) < 
        MSA_MME_TEXT_MAX_SIZE)
    {
        maxLength = MSA_MME_TEXT_MAX_SIZE;
    }
    /* Create editor */
    if (0 == (msaMmeTextView.formHandle = WE_WIDGET_EDITOR_CREATE(
        WE_MODID_MSA, msaGetEmptyStringHandle(), textStringHandle, WeText, 
        0, 0, maxLength, FALSE, NULL, WE_WINDOW_PROPERTY_TITLE, 0))) /* Q04A */
    {
        (void)WE_WIDGET_RELEASE(textStringHandle);
        return FALSE;
    }
    (void)WE_WIDGET_RELEASE(textStringHandle);

    /* Set the title */
    tmpStr = (NULL != subject) ? subject: "";
    if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
        tmpStr, WeUtf8, (int)strlen(tmpStr) + 1, 0)))
    {
        (void)WE_WIDGET_RELEASE(strHandle);
        return FALSE;
    }
    (void)msaSetTitle(msaMmeTextView.formHandle, strHandle, 
        WE_WINDOW_PROPERTY_TITLE);

    /* Create actions */
    if ((-1 != retSigOk) && 
        !msaCreateAction(msaMmeTextView.formHandle, WeOk, NULL, retFsm, 
        retSigOk, MSA_STR_ID_OK))
    {
        return FALSE;
    }
    if ((-1 != retSigBack) && 
        !msaCreateAction(msaMmeTextView.formHandle, WeCancel, NULL, retFsm, 
        retSigBack, MSA_STR_ID_CANCEL))
    {
        return FALSE;
    }
    if ((-1 != retSigMenu) && 
        !msaCreateAction(msaMmeTextView.formHandle, WeMenu, NULL, retFsm, 
        retSigMenu, MSA_STR_ID_MENU))
    {
        return FALSE;
    }
    /* Show editor */
    return msaDisplayWindow(msaMmeTextView.formHandle, msaGetPosLeftTop());
}

/*!
 * \brief Deletes the text edit view.
 *****************************************************************************/
void msaUiMmeDeleteTextView(void)
{
    /* Delete window and actions */
    (void)msaDeleteWindow(msaMmeTextView.formHandle);
    /* Delete obsolete handles */
    memset(&msaMmeTextView, 0, sizeof(MsaMmeTextView));
}

/*!
 * \brief Retrieves text from text edit view
 *
 * \return The text string. It is the callers responsibility to free the 
 *         string. NULL is returned if no data can be retrieved.
 *****************************************************************************/
char *msaUiMmeGetMessageText(void)
{
    return (0 != msaMmeTextView.formHandle) 
        ? msaGetTextFromGadget(msaMmeTextView.formHandle) 
        : NULL;
}

/*!
 * \brief Stops the currently playing sound from playing
 *****************************************************************************/
void msaUiMmeStopAudio(void)
{
    if (msaMmeEditorView.audioHandle)
    {
        (void)WE_WIDGET_SOUND_STOP(msaMmeEditorView.audioHandle);
        (void)WE_WIDGET_RELEASE(msaMmeEditorView.audioHandle);
    }
    msaMmeEditorView.audioHandle = 0;
}

/*!
 * \brief Plays the currently selected sound
 *****************************************************************************/
void msaUiMmePlayAudio(void)
{
    WeDirectData directData;
    WeCreateData createData;
    createData.directData = &directData;
    
    msaUiMmeStopAudio();
    directData.data = (const char*)msaMmeEditorView.mo->data;
    directData.dataSize = (int)msaMmeEditorView.mo->size;
    msaMmeEditorView.audioHandle = WE_WIDGET_SOUND_CREATE(
        WE_MODID_MSA, &createData, 
        (char *)msaMmeEditorView.mo->type->strValue, WeResourceBuffer);
    (void)WE_WIDGET_SOUND_PLAY(msaMmeEditorView.audioHandle);
}


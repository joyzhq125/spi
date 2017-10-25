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

/* !\file mauise.c
 *  \brief Functionality for the SMIL editor GUI
 */

/* WE */
#include "We_Mem.h"
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Lib.h"
#include "We_Log.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MTR */
#include "Msa.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Mem.h"
#include "Msa_Se.h"
#include "Msa_Slh.h"
#include "Msa_Uise.h"
#include "Msa_Uisig.h"
#include "Msa_Uicmn.h"
#include "Msa_Uiform.h"
#include "Msa_Ph.h"
#include "Msa_Utils.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

#define EDITOR_PROPERTIES           (WE_WINDOW_PROPERTY_TITLE)
#define PAR_CHAR_RIGHT              ((unsigned char)'(')
#define PAR_CHAR_LEFT               ((unsigned char)')')
#define SLASH_CHAR                  ((unsigned char)'/')

#define MSA_SE_OBJECT_PADDING       (6)
#define MSA_SE_MAX_IMAGE_WIDTH      (800)
#define MSA_SE_MAX_IMAGE_HEIGHT     (600)

#define MSA_SE_TEXT_MIME_TYPE       "text/plain"

/* Max character input for the item timing */
#define MSA_SE_ITEM_TIMING_MAX_CHARS    3

/* Sets the color distance for when the text color (cursor) in a SMIL is to
 * close to the background color and needs to be changed.
 */
#define MSA_MIN_COLOR_DIFF      0x08

/******************************************************************************
 * Definitions of internal data-structures for the GUI.
 *****************************************************************************/

/*! \struct MsaSeEditor
 *  Main view */
typedef struct
{
    WeWindowHandle formHandle;

    /* Image handling */
    WeGadgetHandle imageGadgetHandle;

    /* Text handling */
    WeGadgetHandle textGadgetHandle;
    int textBoxInstance;
    WeStringHandle textHandle;

    /* Audio handling */
    WeGadgetHandle audioGadgetHandle;

    /* Input gadgets */
    WeGadgetHandle textInputHandle;
    WeGadgetHandle imageInputHandle;
    WeGadgetHandle audioInputHandle;

    /* The currently active object */
    MsaSeGadgetDef  activeMediaObject;   /* The type of media that is currently 
                                                active */
}MsaSeEditor;

/*! \struct MsaSeItemTiming
 *	
 */
typedef struct 
{
    WeWindowHandle     formHandle;
    WeGadgetHandle     startGadgetHandle;
    WeGadgetHandle     durationGadgetHandle;

    MsaMediaGroupType   gType;
    WE_UINT32          begin;
    WE_UINT32          duration;

}MsaSeItemTiming;


/******************************************************************************
 * Internal data-structures for the GUI.
 *****************************************************************************/

static MsaSeEditor     *msaSeView;

static MsaSeItemTiming *msaSeItemTiming;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void cleanupGadgets(void);
static WE_BOOL handleSeTvOkAction(char *text, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue);
static void handleSeOkAction(WeWindowHandle winHandle);
static void handleItSelAction(WeWindowHandle winHandle);
static void adjustTextColor(WeColor *textColor, const WeColor *bgColor);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Initiates internal data-structures for the SE.
 *****************************************************************************/
void msaSeUiInit(void)
{
    msaSeView = NULL;
} /* msaSeUiInit */

/*!
 *  \brief Ok signal handler for the editor view.
 *  \param winHandle The current window handle.
 *****************************************************************************/
static void handleSeOkAction(WeWindowHandle winHandle)
{
    (void)winHandle;
    /* Set which gadget that is currently active */
    msaSeSetActiveObj();

    /* perform actions accordingly */
    switch(msaSeGetActiveGadget())
    {
    case MSA_SE_GADGET_IMAGE:
    case MSA_SE_GADGET_INP_IMAGE:
        (void)MSA_SIGNAL_SENDTO_U(MSA_SE_FSM, MSA_SIG_SE_HANDLE_OBJECT, 
            MSA_MEDIA_GROUP_IMAGE);
        break;
    case MSA_SE_GADGET_TEXT:
    case MSA_SE_GADGET_INP_TEXT:
        (void)MSA_SIGNAL_SENDTO_U(MSA_SE_FSM, MSA_SIG_SE_HANDLE_OBJECT, 
            MSA_MEDIA_GROUP_TEXT);
        break;
    case MSA_SE_GADGET_AUDIO:
    case MSA_SE_GADGET_INP_AUDIO:
        (void)MSA_SIGNAL_SENDTO_U(MSA_SE_FSM, MSA_SIG_SE_HANDLE_OBJECT, 
            MSA_MEDIA_GROUP_AUDIO);
        break;
    case MSA_SE_GADGET_NONE:
        (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_NAV_MENU_ACTIVATE);
        break;
    }
} /* handleSeEvOkAction */ 

/*!
 *  \brief Ok signal handler for the text editor.
 *  \param text The edited text
 *  \param notUsed N/A.
 *  \param dataPtr N/A.
 *  \param uintValue N/A.
 *****************************************************************************/
/*lint -e{818} */
static WE_BOOL handleSeTvOkAction(char *text, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue)
{
    MsaAfiReadResultData *resultData;
    int size = 0;
    if ((0 != notUsed) && (NULL != dataPtr) && (0 != uintValue))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) Erroneous parameter value!\n", __FILE__, __LINE__));
    }
    resultData = MSA_ALLOC(sizeof(MsaAfiReadResultData));
    resultData->mimeType = MSA_ALLOC(sizeof(MSA_SE_TEXT_MIME_TYPE));
    strcpy(resultData->mimeType, MSA_SE_TEXT_MIME_TYPE);
    if (!*text) 
    {
        MSA_FREE(text);
        text = NULL;
        size = 0;
    }
    else
    {
        size = (int)strlen(text) + 1;
    }
    resultData->data = text;
    resultData->size = (WE_UINT32)size;
    resultData->fileName = msaCreateFileNameFromType(MSA_SE_TEXT_MIME_TYPE); /* should not fail */
    (void)MSA_SIGNAL_SENDTO_UP(MSA_SE_FSM, MSA_SIG_SE_GET_MEDIA_OBJECT_RSP, 
            MSA_AFI_READ_RESULT_OK, resultData);
    return TRUE;
} /* handleSeTvOkAction */

/*!
 * \brief Creates the text edit view SMIL edit.
 *
 * \param text The initial text.
 * \param title The title for the form.
 * \return TRUE if the view was created successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSeCreateTextView(char *text, const char* title)
{
    MsaInputItem item;
    WE_BOOL ret;
    WeStringHandle strHandle;
    memset(&item, 0, sizeof(MsaInputItem));
    item.type           = MSA_TEXT_INPUT;
    item.callback       = handleSeTvOkAction;
    item.text           = text;
    item.maxTextSize    = MSA_SE_TEXT_MAX_CHARACTERS;
    item.textType       = WeText;

    /*CR20287*/
    if ((NULL != text) && MSA_SE_TEXT_MAX_CHARACTERS <= strlen(text))
    {
        item.maxTextSize = strlen(text) + 50;
    }

    if (title)
    {
        if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, title, 
            WeUtf8, (int)strlen(title) + 1, 0)))
        {
            (void)WE_WIDGET_RELEASE(strHandle);
            return FALSE;
        }
        ret = msaCreateInputForm(&item, strHandle);
        (void)WE_WIDGET_RELEASE(strHandle);
    }
    else
    {
        ret = msaCreateInputForm(&item, MSA_GET_STR_ID(MSA_STR_ID_EDIT_TEXT));
    }
    return ret;
} /* msaSeCreateTextView */

/*!
 * \brief Creates the main view for the SMIL editor.
 * 
 * \return TRUE if the view was created successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSeCreateEditorView(void)
{
    WeStyleHandle styleHandle;
    if (NULL != msaSeView)
    {
        return FALSE;
    }
    MSA_CALLOC(msaSeView, sizeof(MsaSeEditor));
    msaSeView->textBoxInstance = -1;
    /* Create a style for this form, since the background is going to be 
       changed */
    styleHandle = WE_WIDGET_STYLE_CREATE(WE_MODID_MSA, NULL, NULL, NULL, 
        NULL, NULL, NULL, NULL);

    /* Create the form */
    if (0 == (msaSeView->formHandle = WE_WIDGET_FORM_CREATE(
        WE_MODID_MSA, NULL, EDITOR_PROPERTIES, styleHandle)))
    {
        return FALSE;
    }
    /* Actions */
    if (0 == msaCreateAction(msaSeView->formHandle, WeSelect, 
        handleSeOkAction, (MsaStateMachine)0, 0, MSA_STR_ID_SELECT))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaSeView->formHandle, WeBack, 
        NULL, MSA_SE_FSM, MSA_SIG_SE_DEACTIVATE, MSA_STR_ID_BACK))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaSeView->formHandle, WeMenu, 
        NULL, MSA_SE_FSM, MSA_SIG_SE_NAV_MENU_ACTIVATE, 
        MSA_STR_ID_MENU))
    {
        return FALSE;
    }
    /* Handle the event for the "right" key */
    if (msaRegisterUserEvent(msaSeView->formHandle, WeKey_Right, NULL, MSA_SE_FSM, 
        MSA_SIG_SE_NEXT_SLIDE) < 0)
    {
        return FALSE;
    }
    /* Handle the event for the "left" key */
    if (msaRegisterUserEvent(msaSeView->formHandle, WeKey_Left, NULL, MSA_SE_FSM, 
        MSA_SIG_SE_PREVIOUS_SLIDE) < 0)
    {
        return FALSE;
    }
    /* Display form */
    return msaDisplayWindow(msaSeView->formHandle, msaGetPosLeftTop());
} /* msaSeCreateEditorView */

/*!
 * \brief Sets the title of the SMIL editor
 *
 * \param subject The subject of the message.
 * \param slideIndex The index of the current slide.
 * \param slideCount The total number of slides.
 * \param msgSize The size of the message in bytes.
 *****************************************************************************/
static void seSetTitle(const char *subject, unsigned int slideIndex, 
    unsigned int slideCount, unsigned int msgSize)
{
    WeStringHandle strHandle;
    char *str;
    char *buffer;
    unsigned int len;
    int kb;
    /* Get the kilo byte string */
    if (NULL == (str = msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_KB))))
    {
        return;
    }

    /* Calculate the buffer length for the string to format */
    len = ((NULL != subject) ? strlen(subject) : 0) + 
        ((sizeof(unsigned int) * 3) * 2) / 10 + 
        sizeof(PAR_CHAR_RIGHT) + sizeof(SLASH_CHAR) + sizeof(PAR_CHAR_LEFT) + 
        strlen(str) + sizeof(char)*3 + sizeof(char)*6 + sizeof(char);
    buffer = MSA_ALLOC((WE_UINT32)len);
    /* Format string */
    kb = (int)((msgSize + 999) / 1000); /* Calculate the size in kilo bytes using ceil */
    sprintf(buffer, "%c%d%c%d%c %d %s ", PAR_CHAR_RIGHT, slideIndex + 1, 
        SLASH_CHAR, slideCount, PAR_CHAR_LEFT, kb, str);
    if (NULL != subject)
    {
        strcat(buffer, subject);
    }
    strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, buffer, WeUtf8, 
        (int)strlen(buffer) + 1, 0);
    (void)msaSetTitle(msaSeView->formHandle, strHandle, 
        EDITOR_PROPERTIES);
    (void)WE_WIDGET_RELEASE(strHandle);
    /*lint -e{774} */
    MSA_FREE(str);
    /*lint -e{774} */
    MSA_FREE(buffer);
}

/*! Gets the currently active MsaSeGadgetDef.
 *	
 * \return The currently active media group type, see #MsaSeGadgetDef.
 */
MsaSeGadgetDef msaSeGetActiveGadget(void)
{
    if (NULL != msaSeView)
    {
        return msaSeView->activeMediaObject;
    }
    return MSA_SE_GADGET_NONE;
}

/*! \brief Sets the currently active media object gadget. This function can 
 *         only be called when the SE form has focus
 *
 *****************************************************************************/
void msaSeSetActiveObj(void)
{
    if (NULL == msaSeView)
    {
        return;
    }
    else if ((0 != msaSeView->imageGadgetHandle) &&
        WE_WIDGET_HAS_FOCUS(msaSeView->imageGadgetHandle))
    {
        msaSeView->activeMediaObject = MSA_SE_GADGET_IMAGE;
    }
    else if ((msaSeView->imageInputHandle) && 
        WE_WIDGET_HAS_FOCUS(msaSeView->imageInputHandle))
    {
        msaSeView->activeMediaObject = MSA_SE_GADGET_INP_IMAGE;
    }else if ((0 != msaSeView->textGadgetHandle) &&
        WE_WIDGET_HAS_FOCUS(msaSeView->textGadgetHandle))
    {
        msaSeView->activeMediaObject = MSA_SE_GADGET_TEXT;
    }
    else if ((msaSeView->textInputHandle) && 
        WE_WIDGET_HAS_FOCUS(msaSeView->textInputHandle))
    {
        msaSeView->activeMediaObject = MSA_SE_GADGET_INP_TEXT;
    } 
    else if ((0 != msaSeView->audioGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaSeView->audioGadgetHandle))
    {
        msaSeView->activeMediaObject = MSA_SE_GADGET_AUDIO;
    }
    else if ((msaSeView->audioInputHandle) && 
        WE_WIDGET_HAS_FOCUS(msaSeView->audioInputHandle))
    {
        msaSeView->activeMediaObject = MSA_SE_GADGET_INP_AUDIO;
    }
    else
    {
        msaSeView->activeMediaObject = MSA_SE_GADGET_NONE;
    }
}

/*!
 * \brief Delets the main view for the SMIL editor.
 *****************************************************************************/
void msaSeDeleteEditorView(void)
{
    /* Gadgets */
    if (NULL != msaSeView)
    {
        cleanupGadgets();
        /* Delete window and handles */    
        (void)msaDeleteWindow(msaSeView->formHandle);
        MSA_FREE(msaSeView);
        msaSeView = NULL;
    }
} /* msaSeDeleteEditorView */

/*!
 * \brief Set/unset busy state for the SMIL editor view.
 * 
 * All actions are blocked during a busy state, i.e., no End User input is
 * received.
 *
 * \param busy TRUE to set the form in a busy state, FALSE to set the form in
 *             a "normal" state again.
 *****************************************************************************/
void msaSeSetBusy(WE_BOOL busy)
{
    if ((NULL != msaSeView) && (0 != msaSeView->formHandle))
    {
        if (busy)
        {
            /* Display the busy icon and do not allow any actions */
            (void)WE_WIDGET_WINDOW_SET_PROPERTIES(msaSeView->formHandle, 
                EDITOR_PROPERTIES | WE_WINDOW_PROPERTY_BUSY, 0); 
        }
        else
        {
            /* Remove the busy icon and allow actions */
            (void)WE_WIDGET_WINDOW_SET_PROPERTIES(msaSeView->formHandle, 
                EDITOR_PROPERTIES, 0); /* Q04A */
        }
    }
}

/*!
 * \brief Removes dynamicly created gadgets in the main SE view.
 *****************************************************************************/
static void cleanupGadgets(void)
{
    if (NULL != msaSeView)
    {
        /* Text input */
        if (0 != msaSeView->textInputHandle)
        {
            (void)WE_WIDGET_REMOVE(msaSeView->formHandle,
                msaSeView->textInputHandle);
            (void)WE_WIDGET_RELEASE(msaSeView->textInputHandle);
            msaSeView->textInputHandle = 0;
        }
        /* Image input */
        if (0 != msaSeView->imageInputHandle)
        {
            (void)WE_WIDGET_REMOVE(msaSeView->formHandle,
                msaSeView->imageInputHandle);
            (void)WE_WIDGET_RELEASE(msaSeView->imageInputHandle);
            msaSeView->imageInputHandle = 0;
        }
        /* Audio input */
        if (0 != msaSeView->audioInputHandle)
        {
            (void)WE_WIDGET_REMOVE(msaSeView->formHandle,
                msaSeView->audioInputHandle);
            (void)WE_WIDGET_RELEASE(msaSeView->audioInputHandle);
            msaSeView->audioInputHandle = 0;
        }
        /* Image */
        if (0 != msaSeView->imageGadgetHandle)
        {
            (void)WE_WIDGET_REMOVE(msaSeView->formHandle,
                msaSeView->imageGadgetHandle);
            (void)WE_WIDGET_RELEASE(msaSeView->imageGadgetHandle);
            msaSeView->imageGadgetHandle = 0;
        }
        /* Audio input */
        if (0 != msaSeView->audioGadgetHandle)
        {
            (void)WE_WIDGET_REMOVE(msaSeView->formHandle,
                msaSeView->audioGadgetHandle);
            (void)WE_WIDGET_RELEASE(msaSeView->audioGadgetHandle);
            msaSeView->audioGadgetHandle = 0;
        }
        /* Text */
        if (0 != msaSeView->textGadgetHandle)
        {
            (void)WE_WIDGET_REMOVE(msaSeView->formHandle,
                msaSeView->textGadgetHandle);
            msaSeView->textGadgetHandle = 0;
            (void)mtrDeleteTextBox(msaSeView->textBoxInstance);
            msaSeView->textBoxInstance = -1;
            if (msaSeView->textHandle) 
            {
                (void)WE_WIDGET_RELEASE(msaSeView->textHandle);
                msaSeView->textHandle = 0;
            }
        }
    }
} /* cleanupGadgets */

/*!
 * \brief Creates a image handle
 *
 * \param imageData The data to create the handle from
 * \param size The size of the imageData buffer
 * \param mimeType The MIME-type of the imageData
 * \return Handle to a image, the handle will be 0 on error
 *****************************************************************************/
static WeImageHandle msaCreateImageHandle(const unsigned char *imageData, 
    int size, const char *mimeType)
{
    WeImageHandle imageHandle = 0;
    WeDirectData directData;
    WeCreateData createData;
    createData.directData = &directData;

    if (imageData != NULL)
    {
        directData.data = (const char*)imageData;
        directData.dataSize = size;
        imageHandle = WE_WIDGET_IMAGE_CREATE(WE_MODID_MSA, 
            &createData, mimeType, /* Q04A */
            WeResourceBuffer, 0);
    } /* if */

    return imageHandle;
} /* msaCreateImageHandle */

/*! Handles the gadget placement.
 *
 * \param handle The handle 
 * \param size The size of the gadget is written here.
 * \param pos The position tho place the gadget is written here.
 *****************************************************************************/
static void handleImgPlacement(WeGadgetHandle handle, WeSize *size, 
    WePosition *pos)
{
    WeSize dSize;
    WePosition formPos;
    (void)WE_WIDGET_GET_SIZE(handle, size);
    /* Calculate the position */
   (void)WE_WIDGET_GET_INSIDE_AREA(msaSeView->formHandle, &formPos, &dSize); /* Q04A */
    /* center horizontally */
    pos->x = (WE_INT16)((WE_UINT16)(dSize.width - size->width) >> 1);
}

/*! \brief Adjusts the text color so that it wont clash with the background 
 *         color.
 *
 * \param textColor The current text color from the default style.
 * \param bgColor The background color.
 *****************************************************************************/
static void adjustTextColor(WeColor *textColor, const WeColor *bgColor)
{
    int diff[3];
    unsigned int i;
    int level;
    diff[0] = textColor->r - bgColor->r;
    diff[1] = textColor->g - bgColor->g;
    diff[2] = textColor->b - bgColor->b;
    /* ABS */
    for (i = 0; i < 3; i++)
    {
        if (diff[i] < 0)
        {
            diff[i] = -diff[i];
        }
    }
    /* Check if the color is ok */
    for (i = 0; i < 3; i++)
    {
        if (MSA_MIN_COLOR_DIFF < diff[i])
        {
            /* This color is ok to display */
            return;
        }
    }
    /* Create a new color that can be displayed */

    /* Get the "brightness" level */
    level = (textColor->r + textColor->g + textColor->b)/3;
    /* Adjust level up or down */
    if ((level + 0x7f) <= 0xff)
    {
        level += 0x7f;
    }
    else
    {
        level -= 0x7f;
    }
    /* Create gray scale color with the caluclated color */
    textColor->r = textColor->g = textColor->b = level;
}

/*! Creates a cursor for the SMIL presentation
 *	
 * \param vertPos The vertical position to display the gadget.
 * \param size The size of the new gadget
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL displayInput(WeGadgetHandle *gHandle, WeColor *bgColor, 
    WE_UINT32 strId, WE_INT16 vertPos, WeSize *size)
{
    WePosition pos, formPos;
    WeSize dSize;
    WeStyleHandle style;
    WeColor textColor;
    WeStringHandle strHandle;
    char *tmp;

    /* Check the color of the text */
    strHandle = MSA_GET_STR_ID(strId);
    /* Copy the handle in order to be able to change the style */
    tmp = msaGetStringFromHandle(strHandle);
    if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, tmp, 
        WeUtf8, strlen(tmp) + 1, 0)))
    {
        MSA_FREE(tmp);
        return FALSE;
    }
    MSA_FREE(tmp);

    (void)WE_WIDGET_STYLE_GET_DEFAULT(WeStringStyle, &textColor, NULL, NULL, 
        NULL, NULL, NULL, NULL);
    /* Make sure that the text color is not the same as the background color */
    adjustTextColor(&textColor, bgColor);
    (void)WE_WIDGET_SET_COLOR(strHandle, &textColor, 0);

    if (0 == (style = WE_WIDGET_STYLE_CREATE(WE_MODID_MSA, NULL, bgColor, 
        NULL, NULL, NULL, NULL, NULL)))
    {
        (void)WE_WIDGET_RELEASE(strHandle);
        return FALSE;
    }
    if (0 == (*gHandle = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
        strHandle, NULL, 0, TRUE, WE_GADGET_PROPERTY_FOCUS, style)))
    {
        (void)WE_WIDGET_RELEASE(strHandle);
        (void)WE_WIDGET_RELEASE(style);
        return FALSE;
    }
    (void)WE_WIDGET_RELEASE(strHandle);
    (void)WE_WIDGET_RELEASE(style);
    
    if ((WE_WIDGET_GET_SIZE(*gHandle, size) < 0) ||
        (WE_WIDGET_GET_INSIDE_AREA(msaSeView->formHandle, &formPos, &dSize) < 0)) /* Q04A */
    {
        return FALSE;
    }
    pos.y = vertPos;
    pos.x = (WE_INT16)((WE_UINT16)(dSize.width - size->width)>>1);
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaSeView->formHandle, 
        *gHandle, &pos)) /* Q04A */
    {
        return FALSE;
    }
    /* Everything went ok, return */
    return TRUE;
}

/*! Displays an image.
 *
 * \param objInfo The image object.
 * \param pos The position to display the gadget.
 * \param size (out) The size of the new gadget.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL displayImage(const MsaObjInfo *objInfo, WE_UINT16 vertPos, 
    WeSize *size)
{
    WeImageHandle imgHandle = 0;
    WeSize dSize;
    WE_UINT16 newWidth;
    WePosition pos;

    /* Check if there is a valid picture to display */
    if (NULL != objInfo->object->data)
    {
        if (0 != (imgHandle = msaCreateImageHandle(objInfo->object->data, 
            (int)objInfo->object->size, 
            (char *)objInfo->object->type->strValue)))
        {
            (void)WE_WIDGET_GET_SIZE(imgHandle, size);
            /* Check that the image is not to large */
            if ((size->width > MSA_SE_MAX_IMAGE_WIDTH) || 
                (size->height > MSA_SE_MAX_IMAGE_HEIGHT))
            {
                (void)WE_WIDGET_RELEASE(imgHandle);
                imgHandle = 0;
            }
        }
    }
    /* Check if an icon should be displayed instead */
    if (0 == imgHandle)
    {
        /* Create */
        imgHandle = MSA_GET_IMG_ID(MSA_IMG_BROKEN_IMAGE);
    }
    /* Handle the image scaling */
    (void)WE_WIDGET_GET_SIZE(imgHandle, size);
    msaGetDisplaySize(&dSize);
    /* Check if the image can fit horizontally on the display  */
    if (size->width > dSize.width - 2 * MSA_SE_OBJECT_PADDING)
    {
        /* Scale using the same aspect */
        newWidth = (WE_UINT16)(dSize.width - 2 * MSA_SE_OBJECT_PADDING);
        size->height = (WE_INT16)((size->height * newWidth) / (size->width));
        size->width = (WE_INT16)newWidth;
    }
    /* Image position */
    pos.x = (WE_INT16)((WE_UINT16)(dSize.width - size->width) >> 1);
    pos.y = (WE_INT16)vertPos;

    /* Display the image */
    if (0 == (msaSeView->imageGadgetHandle = 
        WE_WIDGET_IMAGE_GADGET_CREATE(WE_MODID_MSA, imgHandle, size, 
        WeImageZoomAutofit, WE_GADGET_PROPERTY_FOCUS, 0)))
    {
        if (NULL != objInfo->object->data)
        {
            (void)WE_WIDGET_RELEASE(imgHandle);
        }
        return FALSE;
    }
    /* Release the image handle */
    if (NULL != objInfo->object->data)
    {
        (void)WE_WIDGET_RELEASE(imgHandle);
    }
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaSeView->formHandle, 
        msaSeView->imageGadgetHandle, &pos)) /* Q04A */
    {
        return FALSE;
    }
    return TRUE;
}


/*! Displays a text gadget.
 *
 * \param objInfo The text object.
 * \param pos The position to display the gadget.
 * \param size (out) The size of the new gadget.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL displayText(MsaObjInfo *objInfo, WE_INT16 vertPos, 
    WeSize *size, const WeColor *bgColor)
{
    WeStringHandle strHandle;
    WeSize dSize;
    WePosition pos = {0, 0};
    MtrCreateRspData *rsp;
    MtrCreateTextBoxProperties prop;
    WeColor fgColor;
    WePosition formPos;
    
    if ((NULL != objInfo->object->data) &&
        (NULL != objInfo->object->type->strValue))
    {
        /* Create the string gadget */
        if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
            (char *)objInfo->object->data, WeUtf8, 
            (int)strlen((char *)objInfo->object->data) + 1, 0)))
        {
            return FALSE;
        }

    }
    else
    {
        /* No text can be displayed, display a default text */
        strHandle = MSA_GET_STR_ID(MSA_STR_ID_TEXT_MISSING);
    }
    msaIntToWeColor(objInfo->textColor, &fgColor);
    /* Adjust color so that the text is going to be visible */
    adjustTextColor(&fgColor, bgColor);

    /* Write color back if it is used and has changed by adjustTextColor() */
    if (objInfo->textColor != SIS_COLOR_NOT_USED)
    {
        objInfo->textColor = msaWeToIntColor(&fgColor);
    }   
    
    /* Get the display size */
    if (WE_WIDGET_GET_INSIDE_AREA(msaSeView->formHandle, &formPos, &dSize) /* Q04A */
        < 0)
    {
        return FALSE;
    }
    /* Set text box properties */
    memset(&prop, 0, sizeof(MtrCreateTextBoxProperties));
    /* Colors */
    memcpy(&prop.bgColor, bgColor, sizeof(WeColor));
    memcpy(&prop.textColor, &fgColor, sizeof(WeColor));
    prop.alignment = WeCenter;
    /* Calculate maximum size for text box */
    prop.size.height = (WE_INT16)((WE_UINT16)dSize.height >> 1);
    prop.size.width = (WE_INT16)(dSize.width - 2 * MSA_SE_OBJECT_PADDING);
    prop.flags = 
        MTR_PROP_RENDER_LINKS | 
        MTR_PROP_VERTICAL_RESIZE | 
        MTR_PROP_HORIZONTAL_RESIZE |
        MTR_PROP_HIDE_CURSOR;

    if (MTR_RESULT_OK != mtrCreateTextBox(strHandle, &prop, &rsp)) 
    {
        return FALSE;
    }
    msaSeView->textBoxInstance = rsp->instance;
    msaSeView->textGadgetHandle = rsp->gadgetHandle;
    msaSeView->textHandle = strHandle;
    size->height = rsp->gadgetSize.height;
    size->width = rsp->gadgetSize.width;
    /* Image position */
    pos.x = (WE_INT16)((WE_UINT16)(dSize.width - rsp->gadgetSize.width) >> 1);
    pos.y = vertPos;

    (void)msaRegisterNotification(rsp->gadgetHandle, WeNotifyFocus, 0, 
        MSA_SE_FSM, MSA_SIG_SE_GADGET_GOT_FOCUS);
    (void)msaRegisterNotification(rsp->gadgetHandle, WeNotifyLostFocus, 0, 
        MSA_SE_FSM, MSA_SIG_SE_GADGET_LOST_FOCUS);
    
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaSeView->formHandle, 
        rsp->gadgetHandle, &pos)) /* Q04A */
    {
        return FALSE;
    }
    mtrFreeMtrCreateRspData(&rsp);
    return TRUE;
}


/*! Displays a "sound" gadget.
 *
 * \param pos The position to display the gadget.
 * \param size (out) The size of the new gadget.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL displayAudioIcon(WE_INT16 vertPos, WeSize *size)
{
    WePosition pos;
    WeImageHandle imgHandle;

    /* Get the image */
    imgHandle = MSA_GET_IMG_ID(MSA_IMG_AUDIO);
    /* Handle the layout */
    handleImgPlacement(imgHandle, size, &pos);
    pos.y = vertPos;
    /* Create the gadget */
    if (0 == (msaSeView->audioGadgetHandle = 
        WE_WIDGET_IMAGE_GADGET_CREATE(WE_MODID_MSA, imgHandle, NULL, 
        WeImageZoom100, WE_GADGET_PROPERTY_FOCUS, 0))) 
    {
        return FALSE;
    }
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaSeView->formHandle, 
        msaSeView->audioGadgetHandle, &pos)) /* Q04A */
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Displayes a slide.
 *
 * \param smilInfo The information about the SMIL presentation.
 * \param propItem The current message properties.
 * \return TRUE if the slide was displayed successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSeShowSlide(const MsaSmilInfo *smilInfo, 
    const MsaPropItem *propItem)
{
    WeColor     bgColor;
    WeGadgetHandle firstGadget = 0;
    WeSize      size = {0, 0};
    WePosition  pos = {0, 0};
    unsigned int i;
    MsaSmilSlide *sSlide = msaGetCurrentSlide(smilInfo);
    MsaMediaGroupType visibleObjs;
    if (NULL == msaSeView)
    {
        return FALSE;
    }
    /* Remove old gadgets and window bindings */
    cleanupGadgets();
    seSetTitle(propItem->subject, 
        msaGetSlideIndex(smilInfo, sSlide), msaGetSlideCount(smilInfo), 
        msaCalculateMsgSize(smilInfo, propItem));
    /* Set the background color */
    if (SIS_COLOR_NOT_USED != smilInfo->bgColor)
    {
        msaIntToWeColor(smilInfo->bgColor, &bgColor);
    }
    else
    {
        msaIntToWeColor(MSA_CFG_DEFAULT_SMIL_BG_COLOR, &bgColor);
    }
    
    (void)WE_WIDGET_SET_COLOR(msaSeView->formHandle, &bgColor, 1);
    /* Create new gadgets */
    for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
    {
        if (NULL != sSlide->objects[i])
        {
            switch(msaMimeToObjGroup(
                (char *)sSlide->objects[i]->object->type->strValue))
            {
            case MSA_MEDIA_GROUP_IMAGE:
                if (!displayImage(sSlide->objects[i], (WE_UINT16)pos.y, &size))
                {
                    cleanupGadgets();
                    return FALSE;
                }
                if (!firstGadget) 
                {
                    firstGadget = msaSeView->imageGadgetHandle;
                }
                break;
            case MSA_MEDIA_GROUP_TEXT:
                if (!displayText(sSlide->objects[i], pos.y, &size, &bgColor))
                {
                    cleanupGadgets();
                    return FALSE;
                }
                if (!firstGadget) 
                {
                    firstGadget = msaSeView->textGadgetHandle;
                }
                break;
            case MSA_MEDIA_GROUP_AUDIO:
                if (!displayAudioIcon(pos.y, &size))
                {
                    cleanupGadgets();
                    return FALSE;
                }
                if (!firstGadget) 
                {
                    firstGadget = msaSeView->audioGadgetHandle;
                }
                break;
            case MSA_MEDIA_GROUP_NONE:
            default:
                break;
            }
            pos.y = (WE_INT16)(pos.y + size.height + MSA_SE_OBJECT_PADDING);
        }
    }
    /* Create "insert" alternatives */
    visibleObjs = msaGetSlideObjectDef(smilInfo);
    /* Text input */
    if ((MSA_MEDIA_GROUP_TEXT & visibleObjs) == 0)
    {
        if (!displayInput(&(msaSeView->textInputHandle), &bgColor, 
            MSA_STR_ID_ADD_TEXT, pos.y, &size))
        {
            cleanupGadgets();
            return FALSE;
        }
        pos.y = (WE_INT16)(pos.y + size.height + MSA_SE_OBJECT_PADDING);
        if (!firstGadget) 
        {
            firstGadget = msaSeView->textInputHandle;
        }
    }
    /* Image input */
    if ((MSA_MEDIA_GROUP_IMAGE & visibleObjs) == 0)
    {
        if (!displayInput(&(msaSeView->imageInputHandle), &bgColor,
            MSA_STR_ID_ADD_IMAGE, pos.y, &size))
        {
            cleanupGadgets();
            return FALSE;
        }
        pos.y = (WE_INT16)(pos.y + size.height + MSA_SE_OBJECT_PADDING);
        if (!firstGadget) 
        {
            firstGadget = msaSeView->imageInputHandle;
        }
    }
    /* Audio input */
    if ((MSA_MEDIA_GROUP_AUDIO & visibleObjs) == 0)
    {
        if (!displayInput(&(msaSeView->audioInputHandle), &bgColor, 
            MSA_STR_ID_ADD_AUDIO, pos.y, &size))
        {
            cleanupGadgets();
            return FALSE;
        }
        if (!firstGadget) 
        {
            firstGadget = msaSeView->audioInputHandle;
        }        
    }
    /* Set focus on first object */
    if (firstGadget) 
    {
        (void)WE_WIDGET_SET_IN_FOCUS(firstGadget, 1);
    }
    return TRUE;
} /* msaSeShowSlide */

/*! \brief 
 *
 * \return The type of media that is currently "selected", or 
 *         MSA_MEDIA_GROUP_NONE if no object is selected.
 *****************************************************************************/
MsaMediaGroupType msaSeGetActiveObjType(void)
{
    switch(msaSeGetActiveGadget())
    {
    case MSA_SE_GADGET_IMAGE:
    case MSA_SE_GADGET_INP_IMAGE:
        return MSA_MEDIA_GROUP_IMAGE;
    case MSA_SE_GADGET_TEXT:
    case MSA_SE_GADGET_INP_TEXT:
        return MSA_MEDIA_GROUP_TEXT;
    case MSA_SE_GADGET_AUDIO:
    case MSA_SE_GADGET_INP_AUDIO:
        return MSA_MEDIA_GROUP_AUDIO;
    case MSA_SE_GADGET_NONE:
    default:
        return MSA_MEDIA_GROUP_NONE;
    }
}

/*! \brief 
 *
 * \return The type of media that is currently "selected", or 
 *         MSA_MEDIA_GROUP_NONE if no object is selected.
 *****************************************************************************/
void msaSeHandleEvent(WeEventType event)
{
    WE_BOOL lostFocus;
    
    if (NULL == msaSeView)
    {
        return;
    }

    if (WeKey_Down == event || WeKey_Up == event)
    {
        (void)mtrRedraw(msaSeView->textBoxInstance, event, &lostFocus);
        if (lostFocus) 
        {
            (void)msaDeleteUserEvent(msaSeView->formHandle, WeKey_Down);
            (void)msaDeleteUserEvent(msaSeView->formHandle, WeKey_Up);        
        }        
    }
}

/*! \brief 
 *
 * \return The type of media that is currently "selected", or 
 *         MSA_MEDIA_GROUP_NONE if no object is selected.
 *****************************************************************************/
void msaSeHandleNotif(WeNotificationType notif)
{
    if (NULL == msaSeView)
    {
        return;
    }

    if (WeNotifyFocus == notif) 
    {
        /* Register user events */
        (void)msaRegisterUserEvent(msaSeView->formHandle, WeKey_Down, NULL, 
            MSA_SE_FSM, MSA_SIG_SE_EVENT_KEY_DOWN);
        (void)msaRegisterUserEvent(msaSeView->formHandle, WeKey_Up, NULL, 
            MSA_SE_FSM, MSA_SIG_SE_EVENT_KEY_UP);
    }
    else if (WeNotifyLostFocus == notif)
    {
        (void)msaDeleteUserEvent(msaSeView->formHandle, WeKey_Down);
        (void)msaDeleteUserEvent(msaSeView->formHandle, WeKey_Up);        
    }
}

/*!
 * \brief Creates a text input gadget.
 *
 * \param win The handle of destination window for the new text input gadget.
 * \param gadget A pointer to a gadget handle, the created gadget handle is
 *               written here.
 * \param strHandle The string handle of the text to display
 * \param pos   The position of the text input gadget.
 * \return      TRUE if the gadget was created successfully, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL createTextGadget(WeWindowHandle win, WeGadgetHandle *gadget, 
    WeStringHandle strHandle, WePosition *pos)
{
    WeSize s;
    WeSize dSize;
    WePosition dPos;
    /* Create gadget */
    (void)WE_WIDGET_GET_INSIDE_AREA(win, &dPos, &dSize);
    dSize.width = (WE_UINT16)(dSize.width - 2*MSA_GADGET_DEF_PADDING);
    if (0 == (*gadget = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
        strHandle, NULL, dSize.width, TRUE, 
        WE_GADGET_PROPERTY_FOCUS, 0)))
    {
        return FALSE;
    }
    /* */
    (void)WE_WIDGET_GET_SIZE(*gadget, &s);
    pos->x = (WE_UINT16)(dPos.x + MSA_GADGET_DEF_PADDING);
    /* Bind gadget to form */
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(win, *gadget, pos)) /* Q04A */
    {
        return FALSE;
    }
    pos->y = (WE_INT16)(pos->y + s.height + MSA_GADGET_DEF_PADDING);
    return TRUE;
}

/*! \brief Handles select action for the "item timing" form
 *
 * \param winHandle Not used
 *****************************************************************************/
static void handleItSelAction(WeWindowHandle winHandle)
{
    MsaInputItem item;
    char str[7 + 1]; /* 32 bit/1000 is the max value + termination */
    int newValue;

    (void)winHandle;
    memset(&item, 0, sizeof(MsaInputItem));

    item.type        = MSA_SINGLE_LINE_INPUT;
    item.uintValue   = msaSeItemTiming->gType;
    item.maxTextSize = MSA_SE_ITEM_TIMING_MAX_CHARS;
    item.textType    = WeInteger;

    if (WE_WIDGET_HAS_FOCUS(msaSeItemTiming->startGadgetHandle))
    {
        newValue = (msaSeItemTiming->begin+999)/1000;
        sprintf(str, "%d", newValue);  /* Seconds ceiled */
        item.text = str;
        item.callback = msaSeHandleObjStart;
        if (!msaCreateInputForm(&item, MSA_GET_STR_ID(MSA_STR_ID_START_TIME)))
        {
            msaSeDeleteItemTiming();
        }
    }
    else
    {
        newValue = (msaSeItemTiming->duration + 999)/1000;
        sprintf(str, "%d", newValue); /* Seconds ceiled */
        item.text = str;
        item.callback = msaSeHandleObjDur;
        if (!msaCreateInputForm(&item, MSA_GET_STR_ID(MSA_STR_ID_OBJ_OB_DURATION)))
        {
            msaSeDeleteItemTiming();
        }
    }
}

/*! \brief Handles select action for the "item timing" form
 *
 * \param winHandle Not used
 *****************************************************************************/
static void handleItBackAction(WeWindowHandle winHandle)
{
    (void)winHandle;
    /* Delete the form */
    msaSeDeleteItemTiming();
}

/*! \brief Creates a form for inputing the object timing.
 *
 * \param start The object start time in ms.
 * \param dur The object duration in ms.
 * \param slideDur The duration for the current slide.
 * \param gType The object type of he current object.
 *****************************************************************************/
WE_BOOL msaSeCreateItemTiming(WE_UINT32 start, WE_UINT32 dur, 
    WE_UINT32 slideDur, MsaMediaGroupType gType)
{
    WePosition pos = {MSA_GADGET_DEF_PADDING, MSA_GADGET_DEF_PADDING};
    WeStringHandle strHandle;
    char *str;
    char *p;
    char *s;
    char *title;
    int durInSec;

    if (NULL != msaSeItemTiming)
    {
        /* Instance is used already */
        return FALSE;
    }

    msaSeItemTiming = MSA_ALLOC_TYPE(MsaSeItemTiming);
    memset(msaSeItemTiming, 0, sizeof(MsaSeItemTiming));

    msaSeItemTiming->gType    = gType; /* Save the value to use in the callback */
    msaSeItemTiming->begin    = start;
    msaSeItemTiming->duration = dur;

    /* Create the form */
    if (0 == (msaSeItemTiming->formHandle = WE_WIDGET_FORM_CREATE(
        WE_MODID_MSA, NULL, 0, 0)))
    {
        return FALSE;
    }
    /* Actions */
    if (0 == msaCreateAction(msaSeItemTiming->formHandle, WeSelect, 
        handleItSelAction, (MsaStateMachine)0, 0, MSA_STR_ID_SELECT))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaSeItemTiming->formHandle, WeBack, 
        handleItBackAction, MSA_NOTUSED_FSM, 0, MSA_STR_ID_BACK))
    {
        return FALSE;
    }
    /* Start */
    if (!createTextGadget(msaSeItemTiming->formHandle, 
        &msaSeItemTiming->startGadgetHandle, MSA_GET_STR_ID(MSA_STR_ID_OBJ_START), &pos))
    {
        return FALSE;
    }
    /* Duration */
    if (!createTextGadget(msaSeItemTiming->formHandle, 
        &msaSeItemTiming->durationGadgetHandle, MSA_GET_STR_ID(MSA_STR_ID_OBJ_DURATION), &pos))
    {
        return FALSE;
    }

    /* Title */
    p = msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_PAGE_COLON));
    s = msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_SHORT_SECOND));
    title = msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_ITEM_TIMING));

    MSA_CALLOC(str, sizeof(PAR_CHAR_LEFT) + 10 + 1 + sizeof(PAR_CHAR_RIGHT) + 
        strlen(s) + strlen(p) + strlen(title));
    durInSec = (slideDur + 999)/1000;
    sprintf(str, "%s %c%s %d %s%c", title, PAR_CHAR_RIGHT, p, durInSec, s, 
        PAR_CHAR_LEFT);
    MSA_FREE(p);
    MSA_FREE(s);
    MSA_FREE(title);
    if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, str, WeUtf8,
        strlen(str), 0)))
    {
        MSA_FREE(str);
        return FALSE;
    }
    MSA_FREE(str);
    msaSetTitle(msaSeItemTiming->formHandle, strHandle, 0);
    (void)WE_WIDGET_RELEASE(strHandle);

    /* Update GUI values.*/
    msaSeUpdateItemTiming(start, dur, slideDur);

    /* Display form */
    return msaDisplayWindow(msaSeItemTiming->formHandle, msaGetPosLeftTop());
}

/*! \brief Deletes the form for displaying the object timing input.
 *
 *****************************************************************************/
void msaSeDeleteItemTiming(void)
{
    if (NULL != msaSeItemTiming)
    {
        /* Delete the window and its actions */
        msaDeleteWindow(msaSeItemTiming->formHandle);
        /* Delete gadget handles */
        if (0 != msaSeItemTiming->startGadgetHandle)
        {
            (void)WE_WIDGET_RELEASE(msaSeItemTiming->startGadgetHandle);
        }
        if (0 != msaSeItemTiming->durationGadgetHandle)
        {
            (void)WE_WIDGET_RELEASE(msaSeItemTiming->durationGadgetHandle);
        }
        MSA_FREE(msaSeItemTiming);
        msaSeItemTiming = NULL;
    }
}

/* !\brief Updates the item timing GUI.
 *
 * \param objStart The new object start time in ms.
 * \param objDuration The new object duration in ms.
 * \param slideDuration The duration of the current slide.
 *****************************************************************************/
void msaSeUpdateItemTiming(WE_UINT32 objStart, WE_UINT32 objDuration, 
    WE_UINT32 slideDuration)
{
    char *str;
    char *first  = NULL;
    char *second = NULL;
    int startInSec;
    int durInSec;

    if (NULL != msaSeItemTiming)
    {
        msaSeItemTiming->begin    = objStart;
        msaSeItemTiming->duration = objDuration;
        /* Object start time */
        MSA_CALLOC(first, 10 + 1);
        startInSec = (objStart + 999)/1000;
        sprintf(first, "%d", startInSec);
        second = msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_SECONDS));
        str = we_cmmn_str3cat(WE_MODID_MSA, first, second, NULL);
        MSA_FREE(second);
        MSA_FREE(first);
        if (!msaSetStringGadgetValue(msaSeItemTiming->startGadgetHandle, 
            MSA_GET_STR_ID(MSA_STR_ID_OBJ_START), str))
        {
            MSA_FREE(str);
            return ;
        }
        MSA_FREE(str);
        
        /* Object duration */
        /* Use seconds for validation */
        if ((objStart + 999)/1000 + (objDuration + 999)/1000 >= 
            (slideDuration + 999)/1000)
        {
            str = msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_MAXIMUM));
        }
        else
        {
            MSA_CALLOC(first, 10 + 1);
            durInSec = (objDuration + 999)/1000;
            sprintf(first, "%d", durInSec);
            second = msaGetStringFromHandle(MSA_GET_STR_ID(MSA_STR_ID_SECONDS));
            str = we_cmmn_str3cat(WE_MODID_MSA, first, second, NULL);
            MSA_FREE(second);
            MSA_FREE(first);
        }

        if (!msaSetStringGadgetValue(msaSeItemTiming->durationGadgetHandle, 
            MSA_GET_STR_ID(MSA_STR_ID_OBJ_DURATION), str))
        {
            MSA_FREE(str);
            return ;
        }
        MSA_FREE(str);
    }
}

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

/*! \file mauicmn.c
 *  \brief Common functions for the GUI.
 */

/* WE */
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Core.h"
#include "We_Def.h"
#include "We_Mem.h"
#include "We_Cmmn.h"
#include "We_Act.h"

/* MMS Service */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Uimenu.h"
#include "Msa_Uicmn.h"
#include "Msa_Uiconf.h"
#include "Msa_Uimv.h"
#include "Msa_Uime.h"
#include "Msa_Uimmv.h"
#include "Msa_Uimme.h"
#include "Msa_Uisig.h"
#include "Msa_Uise.h"
#include "Msa_Uipm.h"
#include "Msa_Uidia.h"
#include "Msa_Uimenu.h"
#include "Msa_Uiform.h"
#include "Msa_Utils.h"
#include "Msa_Uimmv.h"
#include "Msa_Uicols.h"

/******************************************************************************
 * Constants
 *****************************************************************************/
static const WePosition      posLeftTop     = {0, 0};

/******************************************************************************
 * Global variables
 *****************************************************************************/

static WeScreenHandle   msaScreenHandle;

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Create the initial data-structures needed for MSA to run.
 *
 *****************************************************************************/
void msaInitGui()
{
    /* Q04A change */
    msaScreenHandle = WE_WIDGET_SCREEN_CREATE(WE_MODID_MSA, 0);
    (void)WE_WIDGET_DISPLAY_ADD_SCREEN(msaScreenHandle);
    /* End Q04A change */
    
    /* Initiate widget signal handler */
    msaInitWidgetSignalHandler();
    /* Init the progress meter */
    msaInitPm();
    /* Init the dialog handling */
    msaInitDia();
    /* Init input form handling */
    msaInitForms();
    /* Init the menu handling */
    msaInitNavMenu();
    /* Config */
    msaConfigUiInit();
    /* Message viewer */
    msaMvUiInit();
    /* Message editor */
    msaMeUiInit();
    /* SMIL editor */
    msaSeUiInit();
    /* The Multipart/Mixed viewer */
    msaMmvUiInit();
    /* The Multipart/Mixed editor */
    msaMmeUiInit();
    /* The color selector */
    msaColorSelectorInit();
}

/*!
 * \brief Free the resources allocated in msaInitGui.
 *
 *****************************************************************************/
void msaTerminateGui()
{
    /* Remove the progress meter */
    msaTerminatePm();
    /* Terminate input form handling */
    msaTerminateForms();
    /* Terminate dialogs */
    msaTerminateDia();
    /* Terminate navigation menu */
    msaTerminateNavMenu();
    /* Color selector */
    msaColorSelectorTerminate();
    /* Terminate the widget signal handler */
    msaTerminateWidgetSignalHandler();
    
    /* Delete the screen */
    if (0 != msaScreenHandle)
    {
        (void)WE_WIDGET_RELEASE(msaScreenHandle);
        msaScreenHandle = 0;
    }
}

/*!
 * \brief Gets the size of the display.
 *
 * \param ms A pointer to a WeSize struct.
 *****************************************************************************/
void msaGetDisplaySize(WeSize *ms)
{
    WeDisplayProperties dp;
    (void)WE_WIDGET_DISPLAY_GET_PROPERTIES(&dp);
    *ms = dp.displaySize;
}

/*!
 * \brief Gets an empty string.
 *
 * \return A string-handle containing a empty string.
 *****************************************************************************/
WeStringHandle msaGetEmptyStringHandle()
{
    return MSA_GET_STR_ID(MSA_STR_ID_EMPTY_STRING);
}

/*!
 * \brief Gets the screen handle of the main screen.
 *
 * \return The screen handle of the main screen, if no screen handle is 
 *         available 0 is returned.
 *****************************************************************************/
WeScreenHandle msaGetScreenHandle()
{
    return msaScreenHandle;
}

/*!
 * \brief Finds the first selected choice index.
 *
 * \param choiceHandle The handle of a choiceGroup.
 * \return The index of the first selected item, -1 if no item is selected.
 *          
 *****************************************************************************/
int msaGetSelectedChoiceIndex(WE_UINT32 choiceHandle)
{
    int i, count;
    if (0 == choiceHandle)
        return -1;
    count = WE_WIDGET_CHOICE_SIZE(choiceHandle);
    for (i=0; i < count; i++)
    {
        if (WE_CHOICE_ELEMENT_SELECTED & WE_WIDGET_CHOICE_GET_ELEM_STATE(
            choiceHandle, i))
            return i;
    }
    return -1;
}

/*!
 * \brief Binds a window the the MSA screen handle and sets the window in 
 *        focus.
 *
 * \param WindowHandle The handle of the window to show.
 * \param pos A pointer to the position the window should be displayed.
 * \return TRUE if the operation was successful, otherwise FALSE.
 *****************************************************************************/
int msaDisplayWindow(WeWindowHandle windowHandle, const WePosition *pos)
{
    if (0 <= WE_WIDGET_SCREEN_ADD_WINDOW(msaGetScreenHandle(), 
        windowHandle, (WePosition *)pos)) /* Q04A change */
    {
        (void)WE_WIDGET_SET_IN_FOCUS(msaGetScreenHandle(), TRUE);
        (void)WE_WIDGET_SET_IN_FOCUS(windowHandle, TRUE);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*\brief Returns the position for widget placement left, top.
 *
 *\return A pointer to a WePostion = {0,0}.
 *****************************************************************************/
WePosition *msaGetPosLeftTop()
{
    return (WePosition *)&posLeftTop;
}

/*!
 * \brief Gets text from a widget.
 *
 * \param gadgetHandle The handle of the gadget to retrieve text from.
 * \return A pointer to a new string or NULL. The caller is responsible for 
 *         returning the allocated string.
 *****************************************************************************/
char *msaGetTextFromGadget(WeGadgetHandle gadgetHandle)
{
    WeStringHandle strHandle;
    char *tmpStr;
    int length;

    if (0 == (strHandle = WE_WIDGET_STRING_CREATE_TEXT(WE_MODID_MSA, 
        gadgetHandle))) /* Q04A */
    {
        return NULL;
    }
    if (0 >= (length = WE_WIDGET_STRING_GET_LENGTH(strHandle, TRUE, 
        WeUtf8)))
    {
        (void)WE_WIDGET_RELEASE(strHandle);
        return NULL;
    }
    /* Add space for NULL termination*/
    length += 1;
    tmpStr = (char *)MSA_ALLOC(sizeof(char)*(unsigned)length);
    memset(tmpStr, 0, (unsigned int)length);
    /* Get string data */
    if (0 > WE_WIDGET_STRING_GET_DATA(strHandle, tmpStr, WeUtf8))
    {
        (void)WE_WIDGET_RELEASE(strHandle);
        /*lint -e{774} */
        MSA_FREE(tmpStr);
        tmpStr = NULL;
        return NULL;    
    }
    (void)WE_WIDGET_RELEASE(strHandle);
    return tmpStr;
}

/*!
 * \brief Extracts the string from a string handle.
 *
 * \param strHandle The string handle to extract text from
 * \return The string from the string handle. Note the caller is responsible
 *         for deallocating the returned string.
 *****************************************************************************/
char *msaGetStringFromHandle(WeStringHandle strHandle)
{
    char *tmpStr = NULL;
    unsigned int length;
    length = (unsigned)WE_WIDGET_STRING_GET_LENGTH(strHandle, TRUE, WeUtf8);
    tmpStr = MSA_ALLOC(length);
    if (0 > WE_WIDGET_STRING_GET_DATA(strHandle, tmpStr, WeUtf8))
    {
        return NULL;    
    }
    return tmpStr;
}

/*!
 * \brief Sets the text part of a gadget.
 *
 * \param gadgetHandle The handle of the gadget to set.
 * \param maxLen Maximum length of the text.
 * \param newStringValue The text to set.
 * \return TRUE if the operation was successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSetTextInputGadgetValue(WeGadgetHandle gadgetHandle, int maxLen, 
    const char *newStringValue)
{
    WeStringHandle tmpStrHandle;
    int len;

    if (NULL != newStringValue)
    {
        len = (int)strlen(newStringValue) + 1;
        tmpStrHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, newStringValue, 
            WeUtf8, len, 0);
        /* Set the new value */
        if (0 == tmpStrHandle)
        {
            return FALSE;
        }
        if (0 > WE_WIDGET_TEXT_SET_TEXT(gadgetHandle, 0, WeText, maxLen, 
            tmpStrHandle, TRUE))
        {
            (void)WE_WIDGET_RELEASE(tmpStrHandle);
            return FALSE;
        }
        (void)WE_WIDGET_RELEASE(tmpStrHandle);
    }
    else
    {
        if (0 > WE_WIDGET_TEXT_SET_TEXT(gadgetHandle, 0, WeText, maxLen, 
            msaGetEmptyStringHandle(), TRUE))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*! 
 * \brief Sets a window title.
 *
 * \param win The window to set title.
 * \param strHandle The string handle of the title.
 * \param currentProperties The current properties set for the window.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSetTitle(WeWindowHandle win, WeStringHandle strHandle, 
    int currentProperties)
{
    /* Title*/
    if (0 > WE_WIDGET_SET_TITLE(win, strHandle, 0)) /* Q04A */
    {
        return FALSE;
    }
    (void)WE_WIDGET_WINDOW_SET_PROPERTIES(win,
        WE_WINDOW_PROPERTY_TITLE | currentProperties, 0); /* Q04A */
    return TRUE;
}

/*!
 *  \brief Sets the string value for a gadget.
 *  \param gadgetHandle The handle of the gadget to set the text for.
 *  \param strHandle The handle of the static string to display
 *  \param str The string to set.
 *  \return TRUE on success, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSetStringGadgetValue(WeGadgetHandle gadgetHandle,
    WeStringHandle strHandle, const char *str)
{
    unsigned int len;
    char *buffer;
    WeStringHandle newStrHandle;
    len = ((0 != strHandle) ? (unsigned)WE_WIDGET_STRING_GET_LENGTH(
        strHandle, TRUE, WeUtf8):0) + 
        ((NULL != str) ? (unsigned)strlen(str): 0) + 1;

    buffer = MSA_ALLOC(len);
    memset(buffer, 0, len);
    if (0 != strHandle)
    {
        if (0 > WE_WIDGET_STRING_GET_DATA(strHandle, buffer, WeUtf8))
        {
            return FALSE;
        }
    }
    if (NULL != str)
    {
        strcat(buffer, str);
    }
    if (0 == (newStrHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, buffer, 
        WeUtf8, (int)strlen(buffer) + 1, 0)))
    {
        /*lint -e{774} */
        MSA_FREE(buffer);
        return FALSE;
    }
    /*lint -e{774} */
    MSA_FREE(buffer);
    if (0 > WE_WIDGET_STRING_GADGET_SET(gadgetHandle, newStrHandle, TRUE))
    {
        (void)WE_WIDGET_RELEASE(newStrHandle);
        return FALSE;
    }
    (void)WE_WIDGET_RELEASE(newStrHandle);
    return TRUE;
}

/*!
 *  \brief Gets a string handle to a string on the form
 *         prefix minVal infix maxVal
 *  \param prefix The resource WID for the prefix string.
 *  \param minVal The first number.
 *  \param infix The resource WID for the prefix string.
 *  \param maxVal The second number.
 *  \return The new string handle on success, otherwise 0.
 *****************************************************************************/
char *msaGetIntervalString(WE_UINT32 prefix, int minVal, 
                                     WE_UINT32 infix, int maxVal)
{
    char *str1;
    char *str2;
    char *newStr, *tmpStr;
    char buffer[10 * sizeof(char) + sizeof(char)/*WE_EOS*/];
    
    str1 = msaGetStringFromHandle(MSA_GET_STR_ID(prefix));
    str2 = msaGetStringFromHandle(MSA_GET_STR_ID(infix));
    sprintf(buffer, "%d", minVal);
    if (NULL == (tmpStr = we_cmmn_str3cat(WE_MODID_MSA, str1, buffer, str2)))
    {
        return NULL;
    }
    MSA_FREE(str1);
    MSA_FREE(str2);

    sprintf(buffer, "%d", maxVal);
    if (NULL == (newStr = we_cmmn_str3cat(WE_MODID_MSA, tmpStr, buffer, 
        NULL)))
    {
        return NULL;
    }
    /*lint -e{774} */
    MSA_FREE(tmpStr);
    return newStr;
    
}

/*! \brief Compares configured limitations against current message size and
 *         object size and shows warning/error dialogs accordingly
 * \param newMsgSize    the potential message size after adding object
 * \param objectSize    the size of the controversial object 
 * \param mediaGroupType the media type of the controversial object
 * \param fsm           the fsm to call when dialog is closed
 * \param signal        the signal to send to the fsm when dialog is closed
 * \return TRUE if adding can proceed, FALSE if serious error level.
 *****************************************************************************/           
MsaSeverity msaShowSizeWarningDialog(WE_UINT32 newMsgSize, 
    WE_UINT32 objectSize, MsaMediaGroupType mediaGroupType, MsaStateMachine fsm, 
    int signal)
{
	/* Check if inclusion of this media object would break the 100k barrier */
	if (newMsgSize > MSA_CFG_MESSAGE_SIZE_MAXIMUM)
	{
        if (MSA_NOTUSED_FSM == fsm)
        {
            (void)msaShowDialog(MSA_GET_STR_ID(
                MSA_STR_ID_MESSAGE_SIZE_MAXIMUM_REACHED), MSA_DIALOG_ERROR);
        }
        else
        {
            (void)msaShowDialogWithCallback(MSA_GET_STR_ID(
                MSA_STR_ID_MESSAGE_SIZE_MAXIMUM_REACHED), MSA_DIALOG_ERROR,
                fsm, signal);
        }
        return MSA_SEVERITY_ERROR;
	} /* if */

    /* check specific limitations for images/text... */
    if (MSA_MEDIA_GROUP_IMAGE == mediaGroupType && 
        objectSize > MSA_CFG_INSERT_IMAGE_MAX_SIZE ||
        MSA_MEDIA_GROUP_TEXT == mediaGroupType &&
        objectSize > MSA_CFG_INSERT_TEXT_MAX_SIZE)
    {
        if (MSA_NOTUSED_FSM == fsm)
        {
            (void)msaShowDialog(MSA_GET_STR_ID(
                MSA_STR_ID_SINGLE_OBJECT_SIZE_WARNING), MSA_DIALOG_WARNING);
        }
        else
        {
            (void)msaShowDialogWithCallback(MSA_GET_STR_ID(
                MSA_STR_ID_SINGLE_OBJECT_SIZE_WARNING), MSA_DIALOG_WARNING, fsm,
                signal);
        }
        return MSA_SEVERITY_WARNING;
    }
	/*
	 *	.. or the 40k conformance barrier, in which case a warning is shown 
     *  warning is only shown 'going up' past the 40k barrier
	 */ 
	if ((newMsgSize > MSA_CFG_MESSAGE_SIZE_WARNING) && 
        (newMsgSize - objectSize  < MSA_CFG_MESSAGE_SIZE_WARNING))
    {
        if (MSA_NOTUSED_FSM == fsm)
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_MESSAGE_SIZE_WARNING), 
                MSA_DIALOG_ERROR);
        }
        else
        {
            (void)msaShowDialogWithCallback(MSA_GET_STR_ID(
                MSA_STR_ID_MESSAGE_SIZE_WARNING), MSA_DIALOG_ERROR, fsm,
                signal);
        }
        return MSA_SEVERITY_WARNING;
	} /* if */

    return MSA_SEVERITY_OK;
}

/*! \brief Converts an integer containing an RGB value (8-bits per channel) 
 *         into a #WeColor data-structure.
 *
 * \param rgbColor The color to convert into a #WeColor structure.
 * \param weColor A reference to a #WeColor data-structure.
 *****************************************************************************/
void msaIntToWeColor(WE_UINT32 rgbColor, WeColor *weColor)
{
    weColor->r = (rgbColor >> 16) & 0xff;
    weColor->g = (rgbColor >> 8)  & 0xff;
    weColor->b = (rgbColor >> 0)  & 0xff;
}

/*! \brief Converts an integer containing an #WeColor data-structure
 *         into a RGB value (8-bits per channel).
 *
 * \param weColor A reference to a #WeColor data-structure.
 * \return The corresponding color as an integer.
 *****************************************************************************/
WE_UINT32 msaWeToIntColor(WeColor *weColor)
{
    return (weColor->r << 16) | (weColor->g << 8) | (weColor->b);
}

/*!
 * \brief Calculates the size of a gadget from a set of criterias.
 *        See inline comment for information on how the values are calculated.
 * \param data A pointer to a #MsaCmnStringGadgetData struct.
 *****************************************************************************/
void msaCalculateStringGadgetSize(WeWindowHandle window, 
    WeGadgetHandle gadget, WeStringHandle strHandle, const WePosition *pos, 
    const WeSize *maxSize, WeSize *newSize, WE_BOOL multipleLines)
{
    WeSize winSize;
    WePosition winPos;
    WeFont font;
    we_cmmn_line_break_t trav;
    char *utf8Str;
    int fontHeight = 0, tmp;
    int len = 0, index, gadgetHeight;

    memset(&trav, 0, sizeof(we_cmmn_line_break_t));
    /* First get the size of the window */
    if (WE_WIDGET_GET_INSIDE_AREA(window, &winPos, &winSize) < 0) /* Q04A */
    {
        return;
    }
    /* Get the font to determine the height */
    if (WE_WIDGET_STYLE_GET_DEFAULT(WeStringStyle, NULL, NULL, NULL, NULL, 
        NULL, &font, NULL) < 0) 
    {
        return;
    }
    /* Get the heigh of the text in order to calculate the height of the 
     * gadget later on */
    if (WE_WIDGET_FONT_GET_VALUES(&font, &tmp, &fontHeight, &tmp) < 0)
    {
        return;
    }
    if (WE_WIDGET_GET_SIZE(gadget, newSize) < 0) 
    {
        return;
    }
    if (!strHandle) 
    {
        if (0 == newSize->width || (pos->x + newSize->width) > (winSize.width - pos->x))
        {
            /* Default width is too large. Decrease width to fit screen */
            newSize->width = (WE_INT16)((int)winSize.width - (int)pos->x);
        }
        if (0 == newSize->height) 
        {
            newSize->height = (WE_INT16)(fontHeight + MSA_CMN_BASELINE_SIZE);
        }
        return;
    }
    /* Calculate width */
    if (0 == newSize->width || (pos->x + newSize->width) > (winSize.width - pos->x))
    {
        /* Default width is too large. Decrease width to fit screen */
        newSize->width = (WE_INT16)((int)winSize.width - (int)pos->x);
    }
    /* The text needs multiple line, iterate until all lines fits */
    gadgetHeight = 0;
    utf8Str = msaGetStringFromHandle(strHandle);
    len = WE_WIDGET_STRING_GET_LENGTH(strHandle, 0, WeUtf8);
    index = 0;
    if (len)
    {
        if (multipleLines) 
        {
            trav.continue_at = utf8Str;
            while (trav.continue_at && *trav.continue_at) 
            {
                /* Add the visible height */
                gadgetHeight = (WE_INT16)(gadgetHeight + fontHeight + 
                    MSA_CMN_BASELINE_SIZE);
                /* Check how many characters that can be displayed */
                if (!we_cmmn_next_line_break(trav.continue_at, strHandle, 0, 
                    newSize->width, trav.start_idx, FALSE, &trav)) 
                {
                    MSA_FREE(utf8Str); 
                    return;
                }
            }
        }
        else
        {
            gadgetHeight = (WE_INT16)(fontHeight + MSA_CMN_BASELINE_SIZE);
            trav.continue_at = utf8Str;
            /* Check how many characters that can be displayed */
            if (!we_cmmn_next_line_break(trav.continue_at, strHandle, 0, 
                newSize->width, trav.start_idx, FALSE, &trav)) 
            {
                MSA_FREE(utf8Str); 
                return;
            }
            if (!trav.fits) 
            {
                newSize->width = (WE_INT16)((int)winSize.width - (int)pos->x);
            }
            else
            {
                newSize->width = trav.width;
            }
        }
        newSize->height = (WE_INT16)gadgetHeight;
        if (newSize->height > maxSize->height) 
        {
            newSize->height = maxSize->height;
        }
    }
    else
    {
        newSize->height = (WE_INT16)(fontHeight + MSA_CMN_BASELINE_SIZE);
    }
    MSA_FREE(utf8Str);
}


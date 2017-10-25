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

/*--- Include files ---*/
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Wid.h"
#include "We_Log.h"
#include "We_Mem.h"
#include "We_Int.h"

#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"

#include "Sia_if.h"
#include "Satypes.h"
#include "Samem.h"
#include "Saintsig.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Saintsig.h"
#include "Sauidia.h"
#include "Sauisig.h"
#include "Sauidia.h"


/*--- Types ---*/

/******************************************************************************
 * Constants
 *****************************************************************************/
static const WePosition      posLeftTop     = {0, 0};


/*! Menu information */
typedef struct
{
    WeWindowHandle menuHandle;
    WeActionHandle actionOk;
    WeActionHandle actionBack;
} SiaSlideMenuView;

/* main window */
typedef struct
{
    WeScreenHandle screenHandle;
    WeSize displaySize;
} SiaScreenView;

/*--- Static variables ---*/
static SiaSlideMenuView smaSlideMenu;
static SiaScreenView smaScreen;

/*--- Definitions/Declarations ---*/
    
/*--- Prototypes ---*/
static void deleteSlideMenu(void);

/*!
 * \brief Initiates the UI signal handler
 *****************************************************************************/
void smaUiInit(void)
{
    smaScreen.screenHandle = 0;
    smaInitWidgetSignalHandler();
    /* Init the dialog handling */
    (void)smaInitDia();
} /* smaUiInit */


/*!
 * \brief Terminate function for the SMA UI FSM
 *****************************************************************************/
void smaUiTerminate(void)
{
    /* Delete all GUI */
    widgetDeleteScreen();
    smaTerminateWidgetSignalHandler();
    /* terminate dialog handling */
    smaTerminateDia();
    deleteStatusIconHandle();
} /* smaUiTerminate */

/*!
 * \brief Returns the handle of the screen used by SMA.
 *
 * \return The handle of the screen used by SMA.
 *****************************************************************************/
WeScreenHandle smaGetScreenHandle(void)
{
    return smaScreen.screenHandle;
}

/*! \brief creates the screen. 
 *\return Result of the operation
 *****************************************************************************/
SiaResult createGUI(void)
{
    SiaResult result = SIA_RESULT_OK;
    WeDisplayProperties dp;

    /* create screen */
    smaScreen.screenHandle = WE_WIDGET_SCREEN_CREATE(WE_MODID_SIA, 0);

    if (smaScreen.screenHandle == (WeScreenHandle)0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: unable to "
            "create screen in createGUI\n"));
        return SIA_RESULT_ERROR;
    } /* if */

    (void)WE_WIDGET_DISPLAY_ADD_SCREEN(smaScreen.screenHandle);
    
    (void)WE_WIDGET_SET_IN_FOCUS(smaScreen.screenHandle, TRUE);
    
    if (-1 == WE_WIDGET_DISPLAY_GET_PROPERTIES(&dp))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: unable to get "
            "device properies in createGUI\n"));
        return SIA_RESULT_ERROR;
    } /* if */

    smaScreen.displaySize = dp.displaySize;

    return result;
} /* createGUI */

/*!
 * \brief Removes the slide menu
 *****************************************************************************/
static void deleteSlideMenu(void)
{
	/* release actions */
	if (smaSlideMenu.actionBack != 0)
    {
        (void)WE_WIDGET_RELEASE(smaSlideMenu.actionBack);
        smaSlideMenu.actionBack  = 0;
    } /* if */
    if (smaSlideMenu.actionOk != 0)
    {
        (void)WE_WIDGET_RELEASE(smaSlideMenu.actionOk);
        smaSlideMenu.actionOk  = 0;
    } /* if */

	/* remove the menu */
	if (smaSlideMenu.menuHandle != 0)
    {
        (void)WE_WIDGET_REMOVE(smaScreen.screenHandle, smaSlideMenu.menuHandle);
        (void)WE_WIDGET_RELEASE(smaSlideMenu.menuHandle);
		smaSlideMenu.menuHandle  = 0;
    } /* if */

    memset(&smaSlideMenu, 0x00, sizeof(smaSlideMenu));
} /* deleteSlideMenu */


/*! \brief Deletes all handles and actions in the widget screen
 *****************************************************************************/
void widgetDeleteScreen(void)
{
    deleteSlideMenu();

    if (smaScreen.screenHandle != 0)
    {
        (void)WE_WIDGET_RELEASE(smaScreen.screenHandle);
        smaScreen.screenHandle = 0;
    } /* if */

    memset (&smaScreen, 0x00, sizeof(smaScreen));

} /* widgetDeleteScreen */

/*! \brief Finds selected index in a IMPLICIT menu
 *  \param choiceHandle Handle to a menu to find selected in.
 *  \return index of the selection.
 *****************************************************************************/
int widgetGetSelected(WE_UINT32 choiceHandle)
{
    int index = 0;
    int numberOfElements = 0;
    
    numberOfElements = WE_WIDGET_CHOICE_SIZE(choiceHandle);
    
    for (index = 0; index < numberOfElements; index++)
    {
        if (WE_CHOICE_ELEMENT_SELECTED & 
            WE_WIDGET_CHOICE_GET_ELEM_STATE(choiceHandle, index))
        {
            return index;
        } /* if */
    } /* for */

    return -1;
} /* widgetGetSelected */


/*! \brief Converts an integer (hex RRGGBB) representing a color 
 * (rgb 8-bit each) into a WeColor.
 *
 * \param color The color to convert into a WeColor.
 * \param mColor destination color
 *
 * \return TRUE if color is used, else FALSE
 *****************************************************************************/
WE_BOOL smaIntegerToWeColor(WE_UINT32 color, WeColor *mColor)
{
	if (color == SIS_COLOR_NOT_USED)
	{
		mColor->r = 0;
        mColor->g = 0;
        mColor->b = 0;
        return FALSE;
	} /* if */
	mColor->r = (color >> 16) & 0xff;
	mColor->g = (color >>  8) & 0xff;
	mColor->b = (color >>  0) & 0xff;
    return TRUE;
} /* smaIntegerToWeColor */



/*! \brief delete all action handles
 *****************************************************************************/                   
void deleteStatusIconHandle(void)
{
#ifdef JOFR
    if (smaSlideForm.iconGadgetHandle != 0)
    {
        (void)WE_WIDGET_REMOVE((WE_UINT32)smaSlideForm.formHandle, 
            (WE_UINT32)smaSlideForm.iconGadgetHandle);
		(void)WE_WIDGET_RELEASE(smaSlideForm.iconGadgetHandle);
        smaSlideForm.iconGadgetHandle = 0;
    } /* if */
#endif
} /* deleteStatusIconHandle */



/*! \brief Gets the image zoom factor
 * \param object object to search
 * \return enum WeImageZoom
 *****************************************************************************/
WeImageZoom smaGetImageZoom(SisFit slsFitValue)
{
	switch (slsFitValue)
	{
	case SIS_ATTR_FILL:
		return WeImageZoomAutofit;
    case SIS_ATTR_HIDDEN:
    case SIS_ATTR_MEET:
    case SIS_ATTR_SCROLL:
    case SIS_ATTR_SLICE:
    case SIS_ATTR_NOT_USED:
    default:
		return WeImageZoom100;
	} /* switch */
} /* smaGetImageZoom */


/*!
 * \brief Returns the size of the display.
 *
 * \return size of the display
 *****************************************************************************/
void smaGetWidnowInsideArea(WeWindowHandle window, WeSize *size)
{
    WePosition     pos = {1, 1};

    (void)WE_WIDGET_GET_INSIDE_AREA(window, &pos, size);
} /* smaGetWidnowInsideArea */


/*\brief Returns the position for widget placement left, top.
 *
 *\return A pointer to a WePostion = {0,0}.
 *****************************************************************************/
WePosition *smaGetPosLeftTop()
{
    return (WePosition *)&posLeftTop;
} /* *smaGetPosLeftTop */

/*!
 * \brief Binds a window the the SMA screen handle and sets the window in 
 *        focus.
 *
 * \param WindowHandle The handle of the window to show.
 * \param pos A pointer to the position the window should be displayed.
 * \return TRUE if the operation was successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL smaBindAndShowWindow(WeWindowHandle windowHandle, const WePosition *pos)
{
    if (-1 != WE_WIDGET_SCREEN_ADD_WINDOW(smaGetScreenHandle(), 
        windowHandle, (WePosition *)pos))
    {
        (void)WE_WIDGET_SET_IN_FOCUS(smaGetScreenHandle(), TRUE);
        (void)WE_WIDGET_SET_IN_FOCUS(windowHandle, TRUE);
        return TRUE;
    }
    else
    {
        return FALSE;
    } /* if */
} /* smaBindAndShowWindow */


/*!\brief Creates a string handle.
 *
 * \param str The content of the string to create.
 *****************************************************************************/
WeStringHandle smaCreateString(const char *str)
{
    if ( (NULL != str) && (0 != strlen(str)))
    {
        return WE_WIDGET_STRING_CREATE(WE_MODID_SIA, str, WeUtf8, 
            (int)(strlen(str) + 1), 0);
    }
    else
    {
        return WE_WIDGET_STRING_CREATE(WE_MODID_SIA, (char *)"", WeUtf8, 
            1, 0);
    } /* if */
} /* smaCreateString */


/*!
 * \brief Finds the first selected choice index.
 *
 * \param choiceHandle The handle of a choiceGroup.
 * \return The index of the first selected item, -1 if no item is selected.
 *          
 *****************************************************************************/
WE_INT32 smaGetSelectedChoiceIndex(WE_UINT32 choiceHandle)
{
    WE_INT32 i;
	int count;

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
} /* smaGetSelectedChoiceIndex */


/*! 
 * \brief Sets a window title.
 *
 * \param win The window to set title.
 * \param strHandle The string handle of the title.
 * \param currentProperties The current properties set for the window.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL smaSetTitle(WeWindowHandle win, WeStringHandle strHandle, 
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
} /* smaSetTitle */



/*!
 * \brief Extracts the string from a string handle.
 *
 * \param strHandle The string handle to extract text from
 * \return The string from the string handle. Note the caller is responsible
 *         for deallocating the returned string.
 *****************************************************************************/
char *smaGetStringFromHandle(WeStringHandle strHandle)
{
    char *tmpStr = NULL;
    unsigned int length;
    length = (unsigned)WE_WIDGET_STRING_GET_LENGTH(strHandle, TRUE, WeUtf8);
    tmpStr = SIA_ALLOC(length);
    if (0 > WE_WIDGET_STRING_GET_DATA(strHandle, tmpStr, WeUtf8))
    {
        return NULL;    
    } /* if */
    return tmpStr;
} /* smaGetStringFromHandle */

/*!
 * \brief Gets a character buffer from a string handle
 *
 * \param strHandle The string handle to get buffer from
 * \return The newly allocated string if successful, otherwise NULL.
 *****************************************************************************/
char *smaGetStringBufferFromHandle(WeStringHandle strHandle)
{
    char *tmpStr;
    int length;
    
    if (0 >= (length = WE_WIDGET_STRING_GET_LENGTH(strHandle, TRUE, 
        WeUtf8)))
    {
        return NULL;
    }
    /* Add space for NULL termination*/
    tmpStr = (char *)SIA_ALLOC(sizeof(char) * 
        (unsigned)length);
    memset(tmpStr, 0, (unsigned int)length);
    /* Get string data */
    if (0 > WE_WIDGET_STRING_GET_DATA(strHandle, tmpStr, WeUtf8))
    {
        /*lint -e{774} */
        SIA_FREE(tmpStr);
        tmpStr = NULL;
        return NULL;    
    }
    return tmpStr;
    
}

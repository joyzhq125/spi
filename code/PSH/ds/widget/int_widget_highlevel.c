/*==================================================================================================

    MODULE NAME : int_widget_highlevel.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   XXXXX            Cxxxxxx     Initial file creation.
    07/03/2003   Steven Lai       P000729     Fix the bug in removing an element from a choice
    7/30/2003    Zhuxq            P001026     Apply the AddGadget function to all window, and modify the MsfEditor Create function
    04/08/2003   Zhuxq            P001052     Fix some bugs, and modify the method of getting the start position of a gadget
    08/05/2003   Steven Lai       P001140     Change the method of handling screen focus
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    8/11/2003    Chenjs           P001091     Update text input widget.
    08/20/2003   Zhuxq            P001279     modify the function of TPIa_widgetImageGadgetCreate
    08/20/2003   Steven Lai       P001312     Fix the bug that some lowlevel widgets may not be released in time
    08/22/2003   Zhuxq            P001290     Modify the MsfAction 
    08/23/2003   Zhuxq            P001347     Fix some bugs of the Input Method, and delete the invalid references
    08/27/2003   Zhuxq            P001383     Add receive progressbar and fixed some bugs
    08/29/2003   Zhuxq            P001396     Add scroll-contents feature to string gadget
    09/03/2003   Zhuxq            P001479     Modify the meanings of the  parameter singleLine of TPIa_widgetStringGadgetSet 
    09/25/2003   Zhuxq            P001696     Re-implement the dialog
    10/17/2003   Zhuxq            C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq            P001842     Optimize the paintbox and input method solution and fix some bug
    12/03/2003   Zhuxq            P002139     Enhance the sensitivity of scrollbar in WAP&MMS
    12/30/2003   linda wang       P002286     Modify the macro define for wap module menu title and item margine.
    01/05/2004   Zhuxq            CNXT072     Add callback function to each widget in the widget system
    01/05/2004   Zhuxq            P002351     Update widget contents in batches    
    01/15/2004   Zhuxq            P002352     All editing actions of MsfTextInput turns to MsfEditor
    02/10/2004   chenxiao         p002405     add form scrollbar  in the dolphin project
    02/16/2004   Zhuxq            P002469     Add button gadget to widget system and add modId parameter to event callback functions
    02/16/2004   chenxiao         p002474     add bitmap animation function in the dolphin project    
    02/17/2004   Zhuxq            P002483     Replace the MSF_GADGET_PROPERTY_PEN_HIT_TO_OK with MSF_GADGET_PROPERTY_PEN_CLICK
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    02/19/2004   Zhuxq            P002504     Add box gadget to widget system
    02/20/2004   Zhuxq            P002517     Adjust the handle order of pen events in widget system
    02/24/2004   chenxiao         p002530     modify WAP & MSG main menu     
    02/28/2004   Zhuxq            P002556     Fix some widget bugs
    03/03/2004   MengJianghong    P002568     Add text get data function.
    03/26/2004   Zbzhang          C002699     Add UMB module
    03/19/2004   Zhuxq            P002687     Fix some bugs in widget system
    03/19/2004   Chenxiao         p002688     change main menu  of wap and fix some bugs from NEC feedback     
    03/27/2004   Zhuxq            P002760     Fix some bugs in widget system
    03/30/2004   zhuxq            P002700     Fix UMB bugs
    04/03/2004   chenxiao         p002806     The display information is incorrect.        
    04/02/2004   zhuxq            P002789     Fix UMB bugs
    04/09/2004   Lhjie            p002799     include the opus_events.h
    04/15/2004   zhuxq            P002977     Fix some bugs in UMB for Dolphin
    04/14/2004   Dingjianxin      p002961     Delete conditional compile
    04/19/2004   chenxiao         p003028     fix umb setting bugs and widget bugs    
    04/22/2004   Andrew           P002659     Fix some bugs of projects that bases Dolphin
    04/24/2004   zhuxq            P003022     Fix some bugs in UMB
    04/23/2004   Dingjianxin      P005087     Modified two merger error
    04/29/2004   zhuxq            P005196     Fix some bugs in UMB
    05/04/2004   chenxiao         p005248     fix some umb and widget bugs    
    05/09/2004   Zbzhang          p005266     Fix bugs of umb
    05/18/2004   Zhang Youkun     p005280     set cursor position for fixing bug 5127
    05/24/2004   zhuxq            P005568     Adjust UI implementation in UMB and fix some bugs
    06/07/2004   zhangyoukun      p005917     fix bugs about push focusing and viewing.
    06/02/2004   chenxiao         p005852     Initialize some variable 
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/07/2004   Hover            p005972     fix bug 7338 Int the sms(mms) input 
    06/09/2004   Jianghb          P006036     Add WHALE1,WHALE2 and NEMO compilerswitch to some module
    06/09/2004   zhuxq            P006048     forbid Menu to respond successive multiple actions
    06/12/2004   zhuxq            P006085     Add lotus macro for lotus   
    06/15/2004   majingtao        P006201     fix bugs about push message viewing
    06/18/2004   zhuxq            P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/15/2004   zhuxq            P007016     Fix the bug that paintbox can't be updated with imm 
    07/20/2004   zhuxq            P007108     Add playing GIF animation in some wap page with multi-frame GIF
    08/07/2004   Hugh zhang       p007279     Add progress bar when sending or downloading mms.
    08/18/2004   zhuxq            P007793     Trace widget memory issues, and reduce allocation times of MsfString
    08/30/2004   zhuxq            c007998     improve the implementation of predefined string to reduce the times of memory allocation
    09/14/2004   Hover            p008309     change whale1 and nemo dra backgroud and change violet dialog size
    
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#ifdef WIN32
#include "windows.h"
#include "portab_new.h"
#else
#include "wcdtypes.h"
#include "portab.h"
#endif

#include "OPUS_events.h"
#include "Globlapi.h"    
#include "SP_sysutils.h"

#include "utility_string_func.h"
#include "SP_list.h"

#include "msf_int.h"
#include "msf_lib.h"

#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "Ds_int_def.h"

#include "int_widget_txt_input.h"
#include "int_widget_custom.h"
#include "int_widget_show.h"
#include "int_widget_remove.h"
#include "int_widget_focus.h"
#include "Rm_icon.h"
#include "Ds_util.h"
#include "rm_animation.h"
#include "rm_bitmap.h"
#include "rm_resource_manager.h"

#include "Rm_resource_manager.h"

/* waiting window default propositional height */
#ifndef WW_PROMPT_HEIGHT
#define WW_PROMPT_HEIGHT GADGET_LINE_HEIGHT
#endif

#ifdef  _KEYPAD
extern const OP_UINT32 gadgetSoftKeyIDs[][3];
#endif

extern void widgetBarAdjustValue( MsfBar *pBar );

extern int widgetGadgetGetPosition(const MsfGadget* pGadget,  OP_INT16  *pX, OP_INT16  *pY);

extern void OPUS_Stop_Timer
(
    OPUS_EVENT_ENUM_TYPE   opus_event
);

extern int  widgetSelectgroupAdjustScrollbar( MsfSelectgroup  *pSelectgroup);

extern int  widgetBoxAdjustScrollbar( MsfBox  *pBox);

extern void ds_refresh_for_wap_mms(void);

#if 0
/* Main menu up arrow button's normal background  */
const RM_RESOURCE_ID_T UpArrowNormalBackgroundRes = ICON_UP_BLUE;

/* Main menu up arrow button's focus background */
const RM_RESOURCE_ID_T UpArrowFocusBackgroundRes = ICON_UP_RED;

/* Main menu down arrow button's normal background  */
const RM_RESOURCE_ID_T DownArrowNormalBackgroundRes = ICON_DOWN_BLUE;

/* Main menu down arrow button's focus background */
const RM_RESOURCE_ID_T DownArrowFocusBackgroundRes = ICON_DOWN_RED;
#endif

static int ImageGadgetOnTimer(MsfGadgetHandle gadgetHandle, OP_UINT32 iTimerId );
static int WaitingWinOnTimer( MSF_UINT32 hWaitWin, MSF_UINT32 iTimerId );


static int WaitingWinStateChangeCb( MSF_UINT8 modId, MSF_UINT32 hWin, MsfNotificationType notType, void *pData)
{
    MsfWindow       *pWin;
    MsfWidgetType  wt;

    pWin = (MsfWindow*)seekWidget(hWin, &wt);
    if( !pWin || wt != MSF_WAITINGWIN || pWin->parent == OP_NULL)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    /* can't remove the waiting window from the screen, or will restart mechine */
    if( notType == MsfNotifyLostFocus )
    {
        TPIa_widgetStartTimer( 10, MSF_FALSE, hWin, WaitingWinOnTimer);
    }

    return TPI_WIDGET_OK;
}

static int WaitingWinOnTimer( MSF_UINT32 hWaitWin, MSF_UINT32 iTimerId )
{
    MsfWindow       *pWin;
    MsfWidgetType  wt;

    TPIa_widgetStopTimer( hWaitWin, iTimerId );
    
    pWin = (MsfWindow*)seekWidget(hWaitWin, &wt);
    if( pWin && wt == MSF_WAITINGWIN && pWin->parent != OP_NULL)
    {
        TPIa_widgetRemove(pWin->parent->screenHandle, hWaitWin);
    }
    
    return TPI_WIDGET_OK;
}

/****************************************************************
 SCREEN 
 ***************************************************************/
/*==================================================================================================
    FUNCTION:  TPIa_widgetScreenCreateEx

    DESCRIPTION:
        Creates an empty MsfScreen and returns the MsfScreenHandle, it  owns its state change callback function

    ARGUMENTS PASSED:
        modId           The MSF Module identifier
        notify          Turns On/Off the notification functionality, 1 means On,
                        0 means Off.
        defaultStyle    The MsfStyle identifier. If this parameter is 0 the default
                        style of the device will be used.
        cbStateChange  the state change function                
        
    RETURN VALUE:
        The new MsfScreenHandle if the operation was
        successful, otherwise 0.
    
    IMPORTANT NOTES:
        None
==================================================================================================*/
MsfScreenHandle TPIa_widgetScreenCreateEx 
(
    MSF_UINT8 modId, 
    int notify, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfScreen *pScreen;
    ScreenInDisplay *pScreenInDisplay, *pTemp;
    MsfScreenHandle newScreenHandle = INVALID_HANDLE;

    if (MSF_IS_MODULE_ID_VALID(modId))
    {
        pScreen = WIDGET_NEW(MsfScreen);

        if (pScreen != OP_NULL)
        {
            op_memset(pScreen, 0, sizeof(MsfScreen));
            newScreenHandle = getNewHandle(modId, MSF_SCREEN);
            pScreen->screenHandle = newScreenHandle;
            pScreen->notify = notify;

            if (defaultStyle != INVALID_HANDLE)
            {
                replaceHandle((OP_UINT32 *)&pScreen->style, defaultStyle);
            }
            else
            {
                // TODO: use the default style
            }
            pScreen->modId = modId;
            pScreen->focusedChild = INVALID_HANDLE;
            pScreen->children = OP_NULL;
            pScreen->cbStateChange = cbStateChange;
            pScreenInDisplay = WIDGET_NEW(ScreenInDisplay);
            if( !pScreenInDisplay )
            {
                WIDGET_FREE(pScreen);
                return INVALID_HANDLE;
            }
            op_memset(pScreenInDisplay, 0, sizeof(ScreenInDisplay));
            
            SP_list_insert_to_head(&widgetQueue[MSF_SCREEN], pScreen);

            pScreenInDisplay->screen = pScreen;

            pTemp = pDisplay->screenList;
            if (pTemp)
            {
                while (pTemp->next)
                {
                    pTemp = pTemp->next;
                }
                pTemp->next = pScreenInDisplay;
            }
            else
            {
                pDisplay->screenList = pScreenInDisplay;
            }
        }
    }
    
    return newScreenHandle;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetScreenSetNotify

    DESCRIPTION:
        Turns On/Off the notification functionality for the specified MsfScreen.

    ARGUMENTS PASSED:
        screen  The MsfScreen identifier.
        notify  Turns on or off the notification functionality; 1 means On, 0
                means Off.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.
    
    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetScreenSetNotify (MsfScreenHandle screen, int notify)
{
    MsfScreen *pScreen;
    MsfWidgetType pWidgetType;
    int ret = TPI_WIDGET_OK;
    
    pScreen = (MsfScreen *)seekWidget(screen, &pWidgetType);
    if (pScreen == OP_NULL || pWidgetType != MSF_SCREEN)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pScreen->notify = notify;
    }
    return ret;
}
  
/*==================================================================================================
  FUNCTION:  TPIa_widgetScreenAddWindow

  DESCRIPTION:
      Adds the specified MsfWindow to the specified MsfScreen. The MsfWindow is
      placed in the specified MsfPosition. If the MsfWindow to be added is an MsfDialog
      the module can use the parameters nextScreen and nextWindow to specify which
      MsfScreen and MsfWindow that is to be shown when the MsfDialog times out or is
      dismissed. If the MsfWindow to be added is not an MsfDialog, the parameters
      nextScreen and nextWindow are ignored. If the MsfWindow to be added is an
      MsfDialog the position parameter is ignored by the integration.
      
      The MsfWindow that is added to the MsfScreen will get the lowest z-order, which
      means that it will be placed "behind " any other MsfWindows in that MsfScreen. The
      added MsfWindow will not be set in focus. If the added MsfWindow is the only
      MsfWindow in the MsfScreen it will become visible but it will still be out of focus.


  ARGUMENTS PASSED:
      screen        The MsfScreen identifier
      window        The MsfWindow identifier
      position      The MsfPosition of the MsfWindow on the MsfScreen.
      nextScreen    The identifier of the next MsfScreen to be shown
      nextWindow    The identifier of the next MsfWindow to be shown
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise
      the appropriate return code.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetScreenAddWindow 
(
    MsfScreenHandle screen, 
    MsfWindowHandle window, 
    MsfPosition     *position, 
    MsfScreenHandle nextScreen, 
    MsfWindowHandle nextWindow
)
{
    MsfScreen *pScreen;
    void      *ptr;
    MsfWindow *pWindow;
    WindowsInScreen *pWindowInScreen, *pTemp;
    MsfWidgetType widgetType1, widgetType2;
    int ret = TPI_WIDGET_OK;

    pScreen = (MsfScreen *)seekWidget(screen, &widgetType1);
    ptr = seekWidget(window, &widgetType2);
    pWindow = (MsfWindow *)ptr;
    pWindowInScreen = WIDGET_NEW(WindowsInScreen);
    if (pScreen == OP_NULL || 
        widgetType1 != MSF_SCREEN ||
        pWindow == OP_NULL || 
        !IS_WINDOW(widgetType2) ||
        pWindow->parent != OP_NULL || /* a window can attached to only one screen at the same time*/
        pWindowInScreen == OP_NULL ||
        position == OP_NULL)
    {
        if (pWindowInScreen != OP_NULL)
        {
            WIDGET_FREE(pWindowInScreen);
        }
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        op_memset(pWindowInScreen, 0, sizeof(WindowsInScreen));
        op_memcpy(&pWindow->position, position, sizeof(MsfPosition));
        pWindow->parent = pScreen;
        pWindow->referenceCounter++;
        
        pWindowInScreen->window = (void *)pWindow;

        if (pScreen->children == OP_NULL)
        {
            pScreen->children = pWindowInScreen;
        }
        else
        {
            /* let the new added window have the lowest z-order */
            pTemp = pScreen->children;
            while (pTemp->next != OP_NULL)
            {
                pTemp = pTemp->next;
            }
            pTemp->next = pWindowInScreen;
            pWindowInScreen->prev = pTemp;
        }

        /* If it is dialog, we need set the nextScreen and nextWindow elements */
        if (widgetType2 == MSF_DIALOG)
        {
            MsfDialog *pDialog;

            pWindow->position.x = (DS_SCRN_MAX_X - pWindow->size.width) >> 1;
            pWindow->position.y = (WAP_SCRN_TEXT_HIGH - pWindow->size.height) >> 1;
            
            pDialog = (MsfDialog *)ptr;
            pDialog->nextScreen = nextScreen;
            pDialog->nextWindow = nextWindow;
        }
    
    }
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetScreenActiveWindow

  DESCRIPTION:
      Returns the handle to the currently active MsfWindow in the specified MsfScreen.

  ARGUMENTS PASSED:
      screen        The MsfScreen identifier
      
  RETURN VALUE:
      The MsfWindowHandle if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetScreenActiveWindow (MsfScreenHandle screen)
{
    MsfScreen *pScreen;
    MsfWidgetType widgetType;
    MsfWindowHandle ret = TPI_WIDGET_OK;

    pScreen = (MsfScreen *)seekWidget(screen, &widgetType);
    if (pScreen == OP_NULL || widgetType != MSF_SCREEN)
    {
        ret = INVALID_HANDLE;
    }
    else
    {
        ret = pScreen->focusedChild;
    }
    return ret;
}
  


/****************************************************************
 WINDOW 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetWindowAddGadget

  DESCRIPTION:
      Adds the specified MsfGadget to the specified MsfWindow at the specified
      MsfPosition. If the position parameter is set to 0 the integration will use the parameter
      alignment to place the MsfGadget. If the position parameter has a value (!= 0) the
      integration will use this value to place the MsfGadget and will ignore the alignment
      parameter. If both position and alignment parameters are set to 0, the integration is
      responsible for placing the MsfGadget. This function is applicable only to
      MsfPaintbox and MsfForm.
      
      The MsfGadget that is added to the MsfWindow will get the lowest z-order, which
      means that it will be placed "behind " any other MsfGadgets in that MsfWindow. The
      added MsfGadget will not be set in focus. If the added MsfGadget is the only
      MsfGadget in the MsfWindow it will become visible but it will still be out of focus.


  ARGUMENTS PASSED:
      handle        The MsfWindow identifier, that is, the MsfForm or
                    MsfPaintbox   identifier.
      gadget        The MsfGadget identifier
      position      Pointer to an MsfPosition struct, specifying the position of
                    the MsfGadget
      alignment     The alignment of the MsfGadget.  
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the appropriate 
      return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetWindowAddGadget
(
    MsfWindowHandle window, 
    MsfGadgetHandle gadget, 
    MsfPosition* position, 
    MsfAlignment* alignment
)
{
    MsfWindow *pWindow;
    MsfGadget *pGadget;
    GadgetsInWindow *pGadgetInWindow, *pTemp;
    MsfWidgetType windowType, gadgetType;
    MsfPosition      pos;
    MsfSize           size;
    int ret = TPI_WIDGET_OK;

    pWindow = (MsfWindow *)seekWidget(window, &windowType);
    pGadget = (MsfGadget *)seekWidget(gadget, &gadgetType);
    pGadgetInWindow = WIDGET_NEW(GadgetsInWindow);

    if (pWindow == OP_NULL || 
        !IS_WINDOW(windowType)||
        pGadget == OP_NULL || 
        !IS_GADGET(gadgetType)||
        pGadget->parent != OP_NULL ||
        pGadgetInWindow == OP_NULL)
    {
        if (pGadgetInWindow != OP_NULL)
        {
            WIDGET_FREE(pGadgetInWindow);
        }
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        op_memset(pGadgetInWindow, 0, sizeof(GadgetsInWindow));
        
        pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
        pGadget->alignment.verticalPos = MsfVerticalPosDefault;
        
        if (position != OP_NULL)
        {
            op_memcpy(&pGadget->position, position, sizeof(MsfPosition));
        }
        else if (alignment != OP_NULL)
        {
            op_memcpy(&pGadget->alignment, alignment, sizeof(MsfAlignment));
        }
        else
        {
            op_memcpy(&pGadget->position, &DefaultPropTab[gadgetType].pos, sizeof(MsfPosition));
        }
        
        pGadget->parent = pWindow;
        pGadget->referenceCounter++;
        
        pGadgetInWindow->gadget = pGadget;

        if (pWindow->children == OP_NULL)
        {
            pWindow->children = pGadgetInWindow;
        }
        else
        {
            pTemp = pWindow->children;
            while (pTemp->next != OP_NULL)
            {
                pTemp = pTemp->next;
            }
            pTemp->next = pGadgetInWindow;
            pGadgetInWindow->prev = pTemp;
        }

        /* if the parent window (FORM) has no focused gadget, and this gadget 
         * can be focused, set it as default focused gadget.
         * NOTE: This operation is some deferent from Teleca's requirement
         */
        if ((pWindow->focusedChild == OP_NULL) && 
            (pGadget->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
            &&(windowType != MSF_PAINTBOX))
        {
            pWindow->focusedChild = pGadgetInWindow;
            if (msfWidFocusFuncs[gadgetType].pWidGetFocus)
            {
                (void)msfWidFocusFuncs[gadgetType].pWidGetFocus(pGadget);
            }
        }

        /* add the gadget's related horizontal scrollbar */
        if(ISVALIDHANDLE(pGadget->hHScrollBar))
        {
            if( position != OP_NULL )
            {
                /* align the scrollbar at the bottomof the gadget */
                widgetGadgetGetSize( pGadget, &size);
                pos = *position;
                pos.y += size.height;

                position = &pos;
            }
            else if( alignment == OP_NULL )
            {
                /* the gadget use the default position,
                  * so align the scrollbar at the bottomof of the gadget
                  */
                widgetGadgetGetSize( pGadget, &size);
                pos = pGadget->position;
                pos.y += size.height;
                position = &pos;
            }
            
            TPIa_widgetWindowAddGadget(
                                            window,
                                            pGadget->hHScrollBar,
                                            position,
                                            alignment);
        }
        /* add the gadget's related vertical scrollbar */
        if(ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            if( position != OP_NULL )
            {
                /* align the scrollbar at the bottomof the gadget */
                widgetGadgetGetSize( pGadget, &size);
                pos = *position;
                pos.x += size.width;

                position = &pos;
            }
            else if( alignment == OP_NULL )
            {
                /* the gadget use the default position,
                  * so align the scrollbar at the bottomof of the gadget
                  */
                widgetGadgetGetSize( pGadget, &size);
                pos = pGadget->position;
                pos.x += size.width;
                position = &pos;
            }
            
            TPIa_widgetWindowAddGadget(
                                            window,
                                            pGadget->hVScrollBar,
                                            position,
                                            alignment);
        }
        
        if(MSF_FORM == windowType)
        {
            MsfForm *pForm;
            pForm = (MsfForm *)pWindow;
            widgetFormAdjustScrollbar(pForm);
            
        }
        widgetRedraw((void*)pGadget, gadgetType, gadget, OP_TRUE );
        
    }
    
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWindowSetTicker

  DESCRIPTION:
      Sets a ¡°ticker¡± to the specified MsfWindow. A ¡°ticker¡± is a piece of text that runs
      continuously across the MsfWindow. The appearing text is specified by the
      MsfStringHandle tickerText.


  ARGUMENTS PASSED:
      window        The MsfWindow identifier.
      tickerText    The MsfString identifier containing the ticker text.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise
      the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/

int TPIa_widgetWindowSetTicker (MsfWindowHandle window, MsfStringHandle tickerText)
{
    MsfWindow *pWindow;
    MsfWidgetType widgetType;
    int ret = TPI_WIDGET_OK;
    
    pWindow = (MsfWindow *)seekWidget(window, &widgetType);
    if (pWindow == OP_NULL ||
        !IS_WINDOW(widgetType)||
        !ISVALIDHANDLE(tickerText))
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        replaceHandle((OP_UINT32 *)&pWindow->ticker, tickerText);
        pWindow->propertyMask |= MSF_WINDOW_PROPERTY_TICKER;
    }
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWindowSetProperties

  DESCRIPTION:
      Turns on or off the different attributes and properties of the specified MsfWindow.
      Ticker-mode means that MsfStrings or texts that are too long to be visible at once are
      displayed one part at a time.


  ARGUMENTS PASSED:
      window        The MsfWindow identifier.
      propertyMask  MSF_WINDOW_PROPERTY_SCROLLBARVER
                    (0x0001), shows/hides the vertical scrollbar if needed, 1
                    means show.
                    
                    MSF_WINDOW_PROPERTY_SCROLLBARHOR
                    (0x0002), shows or hides the horizontal scrollbar if
                    needed, 1 means show.
                    
                    MSF_WINDOW_PROPERTY_TITLE (0x0004), shows
                    or hides the title, 1 means show.
                    
                    MSF_WINDOW_PROPERTY_BORDER (0x0008),
                    shows or hides the border, 1 means show.
                    
                    MSF_WINDOW_PROPERTY_SECURE (0x0010),
                    shows or hides the security look/icon, 1 means show.
                    
                    MSF_WINDOW_PROPERTY_BUSY (0x0020), shows
                    or hides the busy look/icon, 1 means show.
                    
                    MSF_WINDOW_PROPERTY_TICKERMODE
                    (0x0040), turns on or off the ticker-mode, 1 means On.
                    
                    MSF_WINDOW_PROPERTY_TICKER (0x0080),
                    shows or hides the ticker, 1 means show.
                    
                    MSF_WINDOW_PROPERTY_NOTIFY (0x0100),
                    turns on or off the notification functionality, 1 means
                    On.
                    
                    MSF_WINDOW_PROPERTY_SINGLEACTION
                    (0x0200), turns on or off the MsfAction blocking
                    functionality, 1 means On.
                    
                    MSF_WINDOW_PROPERTY_ALWAYSONTOP
                    (0x0400), turns on or off the AlwaysOnTop
                    functionality, 1 means On.
                    
                    MSF_WINDOW_PROPERTY_DISABLED (0x0800),
                    disables (1) or enables (0) all of the MsfActions
                    attached to this MsfWindow.
                    
                    MSF_WINDOW_PROPERTY_MOVERESIZE
                    (0x1000), specifies if the integration is allowed to move
                    or resize the MsfWindow due to user events, 1 means
                    that the integration is allowed to move or resize the
                    MsfWindow.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetWindowSetProperties (MsfWindowHandle window, int propertyMask)
{
    MsfWindow        *pWindow;
    MsfWidgetType widgetType;
    int                  ret = TPI_WIDGET_OK;
    OP_BOOLEAN    bUpdateSoftKey;
    
    pWindow = (MsfWindow *)seekWidget(window, &widgetType);
    if (pWindow == OP_NULL ||
        !IS_WINDOW(widgetType))
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        /*calc if action disable is changed */
        if((pWindow->propertyMask ^ propertyMask) & MSF_WINDOW_PROPERTY_DISABLED)
        {
            bUpdateSoftKey = OP_TRUE;
        }
        else
        {
            bUpdateSoftKey = OP_FALSE;
        }
        
        pWindow->propertyMask = propertyMask;
        
        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);

        /*   need update the softkey area */
        if( bUpdateSoftKey )
        {
            widgetWindowShowSoftKey( pWindow, OP_TRUE );
        }
    }
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWindowSpecificSize

  DESCRIPTION:
      Retrieves a specific MsfSize of an MsfWindow.
      
      If the minimalSize parameter is not set (minimalSize = 0) this function retrieves the
      MsfSize of the area available for editing in the specified MsfWindow; that is, the area
      available for editing when the chosen MsfWindow attributes (for example title, ticker,
      and MsfActions) have been drawn.
      
      If the minimalSize parameter is set (minimalSize = 1) this function retrieves the
      minimum MsfSize of the specified MsfWindow; that is, the MsfSize of the editable
      area after all of the possible MsfWindow attributes have been added and drawn.


  ARGUMENTS PASSED:
        window      The MsfWindow identifier.
        size        Pointer to an empty MsfSize struct, where the minimum
                    MsfSize of the MsfWindow is returned.
        minimalSize Specifies if the returned MsfSize is the minimal (1)
                    MsfSize or the editable (0) MsfSize of the MsfWindow.     
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetWindowSpecificSize 
(
    MsfWindowHandle window, 
    MsfSize* size, 
    int minimalSize
)
{
    MsfWindow * pWin;
    MsfWidgetType  wt;
    MsfPosition    pos;
    
    if( size == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( minimalSize == 1 )
    {
        size->height = 20;
        size->width = 20;
        return TPI_WIDGET_OK;
    }
    else
    {
        pWin = (MsfWindow*)seekWidget( window, &wt );
        if( pWin && IS_WINDOW(wt))
        {
            return widgetGetClientRect( pWin, &pos, size );
        }
    }
    
    return TPI_WIDGET_ERROR_UNEXPECTED;
}


/****************************************************************
 PAINTBOX 
 ***************************************************************/

/*==================================================================================================
  FUNCTION:  TPIa_widgetPaintboxCreate

  DESCRIPTION:
      Creates an empty MsfPaintbox, with the specified size and returns the MsfWindowHandle. The
      MsfStyle of the MsfPaintbox MsfWindow and the MsfPaintbox brush is set to the MsfStyle
      identified by the defaultStyle parameter. If the MsfStyle of the MsfPaintbox brush is changed
      by the module using any of the set-functions in the style-API, this change must not be reflected
      in the MsfStyle of the MsfPaintbox MsfWindow and vice versa.


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      size          Pointer to an MsfSize struct that specifies the size of
                    the MsfPaintbox
      propertyMask  See TPIa_widgetWindowSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfPaintbox.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the
                    default style of the device will be used.
      
  RETURN VALUE:
      The new MsfPaintboxHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetPaintboxCreate 
(
    MSF_UINT8 modId, 
    MsfSize* size, 
    int propertyMask, 
    MsfStyleHandle defaultStyle
)
{
    MsfWindow *pWindow;
    MsfPaintbox *pPaintbox;
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
//    MsfBrushHandle newBrushHandle = INVALID_HANDLE;

    if (MSF_IS_MODULE_ID_VALID(modId) &&
        (defaultStyle == 0 ||isValidHandleOfType(defaultStyle, MSF_STYLE)))
    {
        pPaintbox = WIDGET_NEW(MsfPaintbox);
        if( !pPaintbox )
        {
            return INVALID_HANDLE;
        }
        
        newWindowHandle = getNewHandle(modId, MSF_PAINTBOX);
        //newBrushHandle = TPIa_widgetStyleCreate(modId, MsfColor * color, MsfColor * backgroundColor, MsfPattern * foreground, MsfPattern * background, MsfLineStyle * lineStyle, MsfFont * font, MsfTextProperty * textProperty)
        if ( newWindowHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pPaintbox);
            newWindowHandle = INVALID_HANDLE;
        }
        else
        {
            op_memset(pPaintbox, 0, sizeof(MsfPaintbox));
            pWindow = (MsfWindow *)pPaintbox;
            pWindow->windowHandle = newWindowHandle;
            pWindow->modId = modId;
            if (size != OP_NULL)
            {
                op_memcpy(&pWindow->size, size, sizeof(MsfSize));
            }
            else
            {
                op_memcpy(&pWindow->size, &DefaultPropTab[MSF_PAINTBOX].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pWindow->position, &DefaultPropTab[MSF_PAINTBOX].pos, sizeof(MsfPosition));

            if(  propertyMask == 0x8000 )
            {
                pWindow->propertyMask = DefaultPropTab[MSF_PAINTBOX].propertyMask;
            }
            else
            {
                pWindow->propertyMask = propertyMask;
            }
            
            if (defaultStyle != 0)
            {
                replaceHandle((OP_UINT32 *)&pWindow->style, defaultStyle);
            }
            else
            {
                // TODO: use the system default style
                //pWindow->style = 
            }
            
            /*   create brush    */ 
            pPaintbox->brush = TPIa_widgetStyleCreate( modId, 
                                                                                         (MsfColor*)(DefaultPropTab[MSF_STYLE].fgColors), 
                                                                                         (MsfColor*)(DefaultPropTab[MSF_STYLE].bgColors), 
                                                                                         (MsfPattern*)&DEFAULT_STYLE_FOREGROUND, 
                                                                                         (MsfPattern*)&DEFAULT_STYLE_BACKGROUND, 
                                                                                         (MsfLineStyle*)&DEFAULT_STYLE_LINESTYLE,
                                                                                         (MsfFont*)&DEFAULT_STYLE_FONT,
                                                                                         (MsfTextProperty*)&DEFAULT_STYLE_TEXTPROPERTY);
            pWindow->referenceCounter = 1;
            pWindow->isReleased = OP_FALSE;

            /*  adjust client area   */
            widgetWindowAdjustClientArea(pWindow);
            
            /* add this item to paintbox queue */
            SP_list_insert_to_head(&widgetQueue[MSF_PAINTBOX], pPaintbox);
        }
    }
    
    return newWindowHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetPaintboxRedrawArea

  DESCRIPTION:
      Gets the area that must be redrawn by the module when the specified MsfPaintbox
      gets focus again; that is, the part of the MsfPaintbox that has been covered by other
      widgets since it lost focus. If there are parts of the MsfPaintbox that have not been
      covered by any other widget since the last time the MsfPaintbox was in focus, those
      parts do not have to be redrawn by the module, since these parts are already visible
      and have not changed. This function can typically be called by the module when the
      specified MsfPaintbox gets focus again. This is a way for the module to find out how
      much of the MsfPaintbox that needs to be redrawn.

  ARGUMENTS PASSED:
      paintbox  The MsfPaintbox identifier.
      size      The MsfSize of the area that has been covered and that needs to
                be redrawn by the module.
      pos       The MsfPosition of the area that has been covered and that
                needs to be redrawn by the module.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetPaintboxRedrawArea 
(
    MsfWindowHandle paintbox, 
    MsfSize* size, 
    MsfPosition* pos
)
{
    return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}
  

/****************************************************************
 FORM 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetFormCreateEx

  DESCRIPTION:
      Creates an empty MsfForm with the specified MsfSize and returns the
      MsfWindowHandle, and the form can own its state change callback function


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      size          Pointer to an MsfSize struct that specifies the size of the
                    MsfForm
      propertyMask  See TPIa_widgetWindowSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfForm.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange  the state change callback function                    
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetFormCreateEx 
(
    MSF_UINT8 modId, 
    MsfSize* size, 
    int propertyMask,
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfWindow *pWindow;
    MsfForm   *pForm;
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    if( !MSF_IS_MODULE_ID_VALID(modId) )
    {
        return INVALID_HANDLE;
    }
    
    pForm = WIDGET_NEW(MsfForm);
    if (pForm != OP_NULL)
    {
        op_memset(pForm, 0, sizeof(MsfForm));
        pWindow = (MsfWindow *)pForm;
        newWindowHandle = getNewHandle(modId, MSF_FORM);
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        if (size != OP_NULL)
        {
            op_memcpy(&pWindow->size, size, sizeof(MsfSize));
        }
        else
        {
            op_memcpy(&pWindow->size, &DefaultPropTab[MSF_FORM].size, sizeof(MsfSize));
        }
        
        op_memcpy(&pWindow->position, &DefaultPropTab[MSF_FORM].pos, sizeof(MsfPosition));

        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_FORM].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
        
        replaceHandle((OP_UINT32 *)&pWindow->style, defaultStyle);
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;
        pWindow->cbStateChange = cbStateChange;

        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);
        
        SP_list_insert_to_head(&widgetQueue[MSF_FORM], pForm);
        
    }
    return newWindowHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetFormResetViewPort

  DESCRIPTION:
      Reset the view port of the form specified by the hForm. It will reset the scrollPos with 0, 
      and reset the value of the form's scrollbar if the scrollbar exists.


  ARGUMENTS PASSED:
      hForm             :The identifier of the MsfForm .
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetFormResetViewPort( MsfWindowHandle hForm )
{
    MsfWindow   *pWin;
    MsfForm       *pForm;
    MsfWidgetType  wt;

    pWin = (MsfWindow*)seekWidget(hForm, &wt);
    if( !pWin || wt != MSF_FORM )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin->scrollPos.x = 0;
    pWin->scrollPos.y = 0;

    pForm = (MsfForm*)pWin;

    if( pForm->pHScrollBar )
    {
        TPIa_widgetBarSetValues(pForm->pHScrollBar->gadgetData.gadgetHandle, 0, pForm->iMaxScrollAreaX);
    }
    
    if( pForm->pVScrollBar )
    {
        TPIa_widgetBarSetValues(pForm->pVScrollBar->gadgetData.gadgetHandle, 0, pForm->iMaxScrollAreaY);
    }

    return TPI_WIDGET_OK;
}

/****************************************************************
 TEXT 
 ***************************************************************/
    
/*==================================================================================================
    FUNCTION:  TPIa_widgetTextSetText

    DESCRIPTION:
        Sets the specified properties of the specified MsfEditor/TextInput, replacing the
        previous contents and properties. The contents of the MsfString, specified by the
        inputString parameter in this function call, are copied into the string contained in the
        MsfText widget. If any of the parameters initialString, maxSize, or inputString in this
        function call is set to 0, that parameter is ignored; that is, if the initialString parameter
        is set to 0, the initial string of the MsfEditor/TextInput will remain unchanged. If the
        parameter type is set to MsfNoTextType, that parameter is ignored and the
        MsfTextType of the MsfEditor/TextInput will remain unchanged.

    ARGUMENTS PASSED:
        text             :The identifier of the MsfEditor or MsfTextInput .
        initialString     :The MsfString identifier, containing the text to be set as initial string.
        type         :Specifies the MsfTextType of the MsfEditor.
        maxSize         :Specifies the maximum number of characters that can be entered into
                     the MsfEditor or MsfTextInput. 0 means infinite size, which in reality 
                     means the maximum number of characters the integration can handle.
        inputString     :The MsfString identifier, containing the text to be set as input string.
        singleLine     :Specifies if the entered text is to be shown on one line (1) or on multiple 
                     lines (0), if the content size is bigger than the MsfSize of the MsfText 
                     widget. This identifies whether the content should be horizontally or 
                     vertically scrollable. If this parameter is set to -1, the integration will 
                     use the default value of singleLine.

    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetTextSetText
(
    MSF_UINT32 text, 
    MsfStringHandle initialString,
    MsfTextType type, 
    int maxSize, 
    MsfStringHandle inputString, 
    int singleLine
)
{
    MsfInput            *pInput;
    int                      iRet;
    MsfSize             size;
    OP_UINT16 iLabLineCnt;

    iRet = getInput( text, &pInput,&size,&iLabLineCnt);
    if( iRet < 0 )
    {
        return iRet;
    }

    /*Initial string*/
    if( initialString != 0)
    {
        replaceHandle((OP_UINT32 *)&pInput->initString, initialString);
    }
    
    /* type */
    if( type != MsfNoTextType )
    {
        pInput->textType = type;
    }

    /*singleLine*/
    pInput->singleLine = singleLine;

    /*maxSize*/
    if(pInput->txtInputBufMng.maxSize < maxSize)
    {
        OP_UINT8 *pDataBuf;

        pDataBuf = pInput->txtInputBufMng.pTxtInputBuf;
        /* alloc a new buffer */
        pInput->txtInputBufMng.pTxtInputBuf = WIDGET_ALLOC(maxSize * 2 + 2);
        if(!pInput->txtInputBufMng.pTxtInputBuf )
        {
            return TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        }
        /* copy the old string data into the new buffer */
        Ustrcpy(pInput->txtInputBufMng.pTxtInputBuf , pDataBuf);
        /* free the old buffer */
        WIDGET_FREE(pDataBuf);

        pInput->txtInputBufMng.maxCharCount = maxSize;
        pInput->txtInputBufMng.maxSize = maxSize;
    }

    /*InputString*/
    if(ISVALIDHANDLE(inputString))
    {
        MsfString     *pStr;
        MsfWidgetType widgetType;
        OP_UINT8      *pChar = OP_NULL;
        MsfStyleHandle hStyle = INVALID_HANDLE;

        pStr = (MsfString*)seekWidget(inputString, &widgetType);
        if( pStr ) 
        {
            if( widgetType == MSF_STRING_RES )
            {
                pChar = (OP_UINT8*)pStr;
            }
            else if( widgetType == MSF_STRING && pStr->data != OP_NULL )
            {
                pChar = pStr->data;
                hStyle = pStr->style;
            }
        }
        
        if(pChar)
        {
            replaceHandle((OP_UINT32 *)&pInput->inputString, inputString);
            pInput->txtInputBufMng.format = MsfUnicode;
            replaceHandle((OP_UINT32 *)&pInput->txtInputBufMng.hStyle, hStyle);
            ///*reserver*/pInput->pTxtInputBufMng->hStyle = pMsfString->style;
            
            TBM_SetContent(&(pInput->txtInputBufMng), pChar,0);
            TBM_AdjustInitCursorPos((TBM_STRUCTURE_NAME_T *) (&pInput->txtInputBufMng));
        }        	
    }

    widgetRedraw(OP_NULL, MSF_TEXTINPUT, text, OP_TRUE);
    
    return TPI_WIDGET_OK;
}
    
/*==================================================================================================
    FUNCTION:  TPIa_widgetTextMaxSize

    DESCRIPTION:
        Returns the maximum number of characters in the specified MsfEditor/TextInput.
    
    ARGUMENTS PASSED:
        text         :The identifier of the MsfEditor or MsfTextInput .
        
    RETURN VALUE:
        The maximum number of characters if the operation was
        successful, otherwise the appropriate return code is given.
    
    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetTextMaxSize
(
    MSF_UINT32 text
)
{
    MsfInput        *pInput;
    int                      iRet;

    iRet = getInput( text, &pInput,OP_NULL,OP_NULL );
    if( iRet < 0 )
    {
        return iRet;
    }
    
    return getInputTextStringLength(pInput, OP_FALSE, OP_TRUE, OP_TRUE);
}
  
/*==================================================================================================
    FUNCTION:  TPIa_widgetTextClear

    DESCRIPTION:
        Clears the contents of the input string in the specified MsfEditor/TextInput, that is,
        sets the input string to ""

    ARGUMENTS PASSED:
        text         :The identifier of the MsfEditor or MsfTextInput
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the
        appropriate return code is given.
    
    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetTextClear
(
    MSF_UINT32 text
)
{
    MsfInput        *pInput;
    int                      iRet;

    iRet = getInput( text, &pInput ,OP_NULL,OP_NULL);
    if( iRet < 0 )
    {
        return iRet;
    }
        
    TBM_ClearContent(&(pInput->txtInputBufMng));
    return iRet;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetTextSetProperty

  DESCRIPTION:
      Sets the text properties for text entered by the user for the specified widget. This
      function is applicable to MsfEditor and MsfTextInput widgets and also MsfDialog of
      type MsfPrompt.


  ARGUMENTS PASSED:
      msfHandle     The identifier of the MsfEditor or MsfTextInput or
                    MsfDialog of type MsfPrompt.
                    color Specifies the MsfColor with which the entered text is to
                    be drawn.
      font          Specifies the MsfFont with which the entered text is to
                    be drawn.
      textProperty  Specifies the MsfTextProperty with which the entered
                    text is to be drawn.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetTextSetProperty 
(
    MSF_UINT32 handle, 
    MsfColor* color, 
    MsfFont* font, 
    MsfTextProperty* textProperty
)
{
    void           *ptr;
    MsfInput       *pInput;
    MsfDialog      *pDlg;
    GadgetsInWindow  *pGadgetNode;
    MsfTextInput        *pTextInput;
    MsfWidgetType  widgetType;
    int            ret = TPI_WIDGET_OK;
    
    ptr = seekWidget(handle, &widgetType);
    if(ptr  == OP_NULL 
    || (widgetType != MSF_EDITOR && widgetType != MSF_TEXTINPUT && widgetType != MSF_DIALOG))    
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        if( widgetType == MSF_EDITOR ||widgetType == MSF_TEXTINPUT )
        {
            if( widgetType == MSF_TEXTINPUT )
            {
                pInput = &((MsfTextInput *)ptr)->input;
            }
            else 
            {
                pInput = &((MsfEditor*)ptr)->input->input;
            }
            
            if( color )
            {
                pInput->color = *color;
            }
                
            if( font )
            {
                pInput->font = *font;
            }
                
            if( textProperty )
            {
                pInput->textProp = *textProperty;
            }
        }
        else        /*  MsfDialog  */
        {
            pDlg = (MsfDialog*)ptr;
            if( pDlg->type == MsfPrompt )
            {
                pGadgetNode = pDlg->windowData.children;
                while( pGadgetNode )
                {
                    pTextInput = (MsfTextInput*)pGadgetNode->gadget;
                    if( pTextInput )
                    {
                        if( color )
                        {
                            pTextInput->input.color = *color;
                        }
                            
                        if( font )
                        {
                            pTextInput->input.font = *font;
                        }
                            
                        if( textProperty )
                        {
                            pTextInput->input.textProp = *textProperty;
                        }
                    }
                    else
                    {
                        ret = TPI_WIDGET_ERROR_UNEXPECTED;
                    }

                    pGadgetNode = pGadgetNode->next;
                }
            }
        }
    }    
    
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetTextGetProperty

  DESCRIPTION:
      Returns the text properties for text entered by the user for the specified widget. This
      function is applicable to MsfEditor and MsfTextInput widgets and also MsfDialog of
      type MsfPrompt. This function can also be applied to an MsfStringGadget. The
      returned values in the MsfStringGadget case will be the same as the style values of the
      MsfString, except for the baseline parameter which returns the MsfPosition of the start
      of the baseline of the MsfString.


  ARGUMENTS PASSED:
      msfHandle         The identifier of the MsfEditor or MsfTextInput or
                        MsfDialog of type MsfPrompt.
      color             Returns the MsfColor of the entered text.
      font              Returns the MsfFont of the entered text.
      textProperty      Returns the MsfTextProperty of the entered text.
      baseline          Returns the MsfPosition of the start of the baseline of
                        the entered text. If the widget specified by the parameter
                        handle is an MsfDialog, this parameter will return 0.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetTextGetProperty
(
    MSF_UINT32 handle, 
    MsfColor* color, 
    MsfFont* font, 
    MsfTextProperty* textProperty, 
    MsfPosition* baseline
)
{
    void           *ptr;
    MsfInput       *pInput;
    MsfDialog       *pDlg;
    GadgetsInWindow *pGadgetNode;
    MsfTextInput       *pTextInput;
    int                 ret;
    MsfWidgetType   widgetType;

    ret = TPI_WIDGET_OK;
    
    ptr = seekWidget(handle, &widgetType);
    if(ptr == OP_NULL 
    || (widgetType != MSF_EDITOR && widgetType != MSF_TEXTINPUT && widgetType != MSF_DIALOG))    
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        if( widgetType == MSF_EDITOR ||widgetType == MSF_TEXTINPUT )
        {
            if( widgetType == MSF_TEXTINPUT )
            {
                pInput = &((MsfTextInput*)ptr)->input;
            }
            else
            {
                pInput = &((MsfEditor*)ptr)->input->input;
            }
            
            if( color )
            {
                *color = pInput->color;
            }
                
            if( font )
            {
                *font = pInput->font;
            }
                
            if( textProperty )
            {
                *textProperty = pInput->textProp;
            }
        }
        else        /*  MsfDialog  */
        {
            pDlg = (MsfDialog*)ptr;
            if( pDlg->type == MsfPrompt )
            {
                pGadgetNode = pDlg->windowData.children;
                if( pGadgetNode )
                {
                    pTextInput = (MsfTextInput*)pGadgetNode->gadget;
                    
                    if( color )
                    {
                        *color = pTextInput->input.color;
                    }
                        
                    if( font )
                    {
                        *font = pTextInput->input.font;
                    }
                        
                    if( textProperty )
                    {
                        *textProperty = pTextInput->input.textProp;
                    }
                }
                else
                {
                    ret = TPI_WIDGET_ERROR_UNEXPECTED;
                }
            }
        }
    }    
    
    return ret;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetTextMaxSize

    DESCRIPTION:
        Returns the current number of characters in the specified MsfEditor/TextInput.
    
    ARGUMENTS PASSED:
        text         :The identifier of the MsfEditor or MsfTextInput .
        
    RETURN VALUE:
        The current number of characters if the operation was
        successful, otherwise the appropriate return code is given.
    
    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetTextCurrentSize
(
    MSF_UINT32 text
)
{
    MsfInput        *pInput;
    int                      iRet;

    iRet = getInput( text, &pInput,OP_NULL,OP_NULL );
    if( iRet < 0 )
    {
        return iRet;
    }
    
    return pInput->txtInputBufMng.iValidCharCount;
}
  

/*==================================================================================================
      FUNCTION:  TPIa_widgetTextGetData
  
      DESCRIPTION:
        Retrieves the data contained in the specified MsfString and puts it in buffer.
        The returned string is always null-terminated.

      ARGUMENTS PASSED:
        handle  i   : The identifier of the MsfEditor or MsfTextInput or MsfDialog of type MsfPrompt
        index   i   : The index of the input field, in the case of an MsfDialog of type MsfPrompt. 
                     this parameter is not used in the case of an MsfEditor or MsfTextInput.
        buffer  o   : The buffer in which the actual MsfString data is returned.
        format  i   : Specifies the MsfStringFormat to be used in the returned buffer.

      RETURN VALUE:
        A positive integer if the operation was successful, 
        otherwise the appropriate return code is given.
        
      IMPORTANT NOTES:
          None
  ==================================================================================================*/
int TPIa_widgetTextGetData(MSF_UINT32 handle, int index, char *buffer, MsfStringFormat format)
{   
    int                    dstLen;
    MsfDialog              *pDlg;
    MsfInput               *pInput;
    MsfTextInput           *pTextInput;
    MsfWidgetType          widgetType;
    OP_UINT8               *pData;
    int                    textLen;
    pInput = OP_NULL;
    pData   = OP_NULL;

    if( !buffer )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( ISVALIDHANDLE(handle) )
    {
        pDlg = (MsfDialog*)seekWidget(handle, &widgetType);
        if(pDlg )
        {
            if(widgetType == MSF_DIALOG && pDlg->type == MsfPrompt )
            {
                pTextInput = widgetDialogGetInputByIndex( pDlg, index);
                if( pTextInput )
                {
                    pInput = &pTextInput->input;
                }
            }
            else if( widgetType== MSF_EDITOR) 
            {
                pInput = &(((MsfEditor*)pDlg)->input->input);
            }
            else if (widgetType == MSF_TEXTINPUT )
            {
                pInput = &(((MsfTextInput*)pDlg)->input);
            }
            else
            {
                return TPI_WIDGET_ERROR_INVALID_PARAM;
            }
        }
        else
        {
            return TPI_WIDGET_ERROR_INVALID_PARAM;
        }
        
        pData = pInput->txtInputBufMng.pTxtInputBuf;
        if( pData )
        {
            if( format != MsfUtf8 )        /* regard as unicode */
            {
                textLen = pInput->txtInputBufMng.iValidCharCount * 2;
                if( textLen > 0 )
                {
                    op_memcpy((OP_UINT8*)buffer, (OP_UINT8*)pData, textLen );
                    buffer[textLen] = buffer[textLen+1] = 0;
                }
                else
                {
                    buffer[0] = buffer[1] = 0;
                }
            }
            else
            {
                dstLen = 10000;
                textLen = pInput->txtInputBufMng.iValidCharCount << 1;
                unicode_to_utf8((char *)pData, &textLen, buffer, &dstLen);
                buffer[dstLen]   = '\0';   
            }

            return TPI_WIDGET_OK;
        }
    }
    return TPI_WIDGET_ERROR_INVALID_PARAM;
}

/****************************************************************
 EDITOR 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetEditorCreate

  DESCRIPTION:
      Creates an MsfEditor with the specified MsfSize and initial MsfString and returns the
      MsfWindowHandle.The MsfEditor can have its state change callback function. The MsfEditor has the specified maximum size and
      MsfTextType constraint. The contents including the MsfStyle values of the MsfString,
      specified by the inputString parameter in this function call, are copied into the string
      contained in the MsfEditor.

  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      initialString Specifies the text to be shown initially in the
                    MsfEditor
      inputString   The MsfString identifier, containing the text to be set
                    as input string.
      type          Specifies the MsfTextType of the MsfEditor
      maxSize       Specifies the maximum number of characters that can
                    be entered into the MsfEditor
      singleLine    Specifies if the entered text is to be shown on one
                    single line (1) or on multiple lines (0), if the content
                    size is bigger than the MsfSize of the MsfEditor. i.e.
                    if the content should be horisontally scrollable or
                    vertically scrollable.
      size          Pointer to an MsfSize struct that specifies the size of
                    the MsfEditor
      propertyMask  See TPIa_widgetWindowSetProperties(). If this
                    parameter is set to 0x8000, the integration will use
                    the device default properties for the MsfEditor.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the
                    default style of the device will be used.      
      cbStateChange  The state change callback function                    
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetEditorCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle initialString, 
    MsfStringHandle inputString, 
    MsfTextType type, 
    int maxSize, 
    int singleLine, 
    MsfSize* size, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfWindow *pWindow;
    MsfEditor *pEditor;
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    
    MsfGadgetHandle   hInput;
    MsfSize  inputsize;
    MsfPosition pos;
    
    if (!MSF_IS_MODULE_ID_VALID(modId) ||
        /*isValidHandleOfType(initialString, MSF_STRING) == OP_FALSE ||
        isValidHandleOfType(inputString, MSF_STRING) == OP_FALSE ||*/
        /*size == OP_NULL || */
        ( defaultStyle != 0 && isValidHandleOfType(defaultStyle, MSF_STYLE) == OP_FALSE))
    {
        return INVALID_HANDLE;
    }

    pEditor = WIDGET_NEW(MsfEditor);
    if( !pEditor )
    {
        return INVALID_HANDLE;
    }
    
    newWindowHandle = getNewHandle(modId, MSF_EDITOR);
    if (newWindowHandle == INVALID_HANDLE)
    {
        WIDGET_FREE(pEditor);
        newWindowHandle = INVALID_HANDLE;
    }
    else
    {
        op_memset(pEditor, 0, sizeof(MsfEditor));
        pWindow = (MsfWindow *)pEditor;
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        if (size != OP_NULL)
        {
            op_memcpy(&pWindow->size, size, sizeof(MsfSize));
        }
        else
        {
            op_memcpy(&pWindow->size, &DefaultPropTab[MSF_EDITOR].size, sizeof(MsfSize));
        }
        
        op_memcpy(&pWindow->position, &DefaultPropTab[MSF_EDITOR].pos, sizeof(MsfPosition));

        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_EDITOR].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
        
        replaceHandle((OP_UINT32 *)&pWindow->style, defaultStyle);
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;

        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);
        
        widgetGetClientRect( pWindow, &pos, &inputsize );
        
        /*   create textinput  */
        hInput = TPIa_widgetTextInputCreate( 
                                            modId, 
                                            initialString, 
                                            inputString, 
                                            type, 
                                            maxSize, 
                                            singleLine, 
                                            &inputsize, 
                                            MSF_GADGET_PROPERTY_BORDER |
                                            MSF_GADGET_PROPERTY_FOCUS | 
#ifdef  ALWAYS_SHOW_SCROLLBAR_IN_EDITOR
                                            MSF_GADGET_PROPERTY_ALWAYS_SHOW_SCROLL_BAR|
#endif                                            
                                            MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM, 
                                            defaultStyle);

        if( !ISVALIDHANDLE( hInput ) )
        {
            WIDGET_FREE(pEditor);
            return INVALID_HANDLE;
        }

        pEditor->input = (MsfTextInput*)seekWidget( hInput, OP_NULL );
        pWindow->cbStateChange = cbStateChange;
        
        SP_list_insert_to_head(&widgetQueue[MSF_EDITOR], pEditor);
        
        TPIa_widgetWindowAddGadget( newWindowHandle, hInput, &pos, OP_NULL );
        TPIa_widgetRelease( hInput );
    }

        
    return newWindowHandle;
}

MsfWindowHandle TPIa_widgetEditorCreateCss (MSF_UINT8 modId, MsfStringHandle initialString, 
                                           MsfStringHandle inputString, MsfTextType type, 
                                           const char* formatString, int inputRequired, 
                                           int maxSize, int singleLine, 
                                           MsfSize* size, int propertyMask, MsfStyleHandle defaultStyle)
{
    return TPIa_widgetEditorCreate( modId, initialString, inputString, type, maxSize, singleLine, size, propertyMask, defaultStyle);
}


/****************************************************************
 CHOICE 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceSetElement

  DESCRIPTION:
      If the insert parameter is set (insert = 1) this function call inserts a new element with
      the specified MsfStrings and MsfImages before the element specified by index. The
      new element is added to the specified MsfMenu/MsfSelectGroup. The function
      returns the index of the inserted element.
      If the insert parameter is not set (insert = 0) this function call sets the element
      specified by index in the specified MsfMenu/MsfSelectGroup. The element contents
      are replaced with the specified MsfStrings and MsfImages.

  ARGUMENTS PASSED:
      choice    The identifier of the MsfMenu or MsfSelectGroup.
      index     Specifies the element index that is to be set.
      string1   Specifies the first text attribute to be shown in the element.
      string2   Specifies the second text attribute to be shown in the element.
      image1    Specifies the first image attribute to be shown in the element.
      image2    Specifies the second image attribute to be shown in the
                element.
      insert    Specifies if the specified attributes are to be inserted in a new
                element or if an already existing element is to be overwritten.
      
  RETURN VALUE:
      The element index if the operation was successful, otherwise
      the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceSetElement 
(
    MSF_UINT32 choice, 
    int index, 
    MsfStringHandle string1, 
    MsfStringHandle string2, 
    MsfImageHandle image1, 
    MsfImageHandle image2, 
    int insert
)
{
    void               *ptr;
    MsfSelectgroup     *pSelectgroup;
    MsfWidgetType       wt;
//    MsfGadget          *pGadget = OP_NULL;

    MsfChoice          *pChoice;
    choiceElement      *pItem;
    choiceElement      *pPreItem;
    int                 iRet;

    iRet = getChoice(choice, &pChoice );
    if( iRet < 0 )
    {
        return iRet;
    }
    
    if( insert == 0 )   /* modify an  item */
    {
        if (pChoice->firstElement == OP_NULL )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        pItem = getChoiceElementByIndex(pChoice,  index);
        if( pItem == OP_NULL )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        replaceHandle((OP_UINT32 *)&pItem->string1, string1);
        replaceHandle((OP_UINT32 *)&pItem->string2, string2);
        replaceHandle((OP_UINT32 *)&pItem->image1, image1);
        replaceHandle((OP_UINT32 *)&pItem->image2, image2);
        iRet = index;
    }
    else if( insert == 1 )  //  add an item
    {
        pItem = WIDGET_NEW(choiceElement);
        if( !pItem )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        op_memset( pItem, 0, sizeof(choiceElement));
        
        if( !pChoice->firstElement )        /* should insert the first item node */
        {
            pChoice->firstElement = pItem;
            pChoice->cur_item = 0;
            pChoice->top_item = 0;
            /* If it is the first item added, set as focus */
            pItem->state |= MSF_CHOICE_ELEMENT_FOCUSED;

            pChoice->count++;
            iRet = 0;
        }
        else if( index == 0 )
        {
            pItem->next = pChoice->firstElement;
            pChoice->firstElement = pItem;
            pChoice->count++;
            iRet = 0;
            pChoice->cur_item++;
        }
        else
        {
            pPreItem = getChoiceElementByIndex(pChoice, index -1 );

            if( !pPreItem ) /*insert to the last*/
            {
                pPreItem = getLastChoiceElement(pChoice);
                if( !pPreItem )
                {
                    WIDGET_FREE(pItem );
                    return TPI_WIDGET_ERROR_UNEXPECTED;
                }
                iRet = pChoice->count;
            }
            else
            {
                iRet = index;
            }
            
            pItem->next = pPreItem->next;
            pPreItem->next = pItem;
            pChoice->count++;
            
            if (iRet <= pChoice->top_item)
            {
                pChoice->top_item++;
            }

            if (iRet <= pChoice->cur_item)
            {
                pChoice->cur_item++;
                if (pChoice->cur_item > pChoice->top_item + pChoice->elementsPerPage)
                {
                    pChoice->top_item++;
                }
            }
        }
        
        replaceHandle((OP_UINT32 *)&pItem->string1, string1);
        replaceHandle((OP_UINT32 *)&pItem->string2, string2);
        replaceHandle((OP_UINT32 *)&pItem->image1, image1);
        replaceHandle((OP_UINT32 *)&pItem->image2, image2);

        /* Get gadget handle */    
        ptr = seekWidget( choice, &wt );
        if( wt == MSF_SELECTGROUP || wt == MSF_BOX)
        {
            pSelectgroup = (MsfSelectgroup*)ptr;
            if( pChoice->count <= pChoice->elementsPerPage )
            {
                pSelectgroup->bItemPosCalculated = OP_FALSE;
            }
        }

        if( wt == MSF_MENU )
        {
            widgetSelectgroupAdjustScrollbar(((MsfMenuWindow *)ptr)->pSelectgroup );
        }
        else if(MSF_MAINMENU == wt)
        {
            widgetSelectgroupAdjustScrollbar(((MsfMainMenuWindow *)ptr)->pSelectgroup );
        }
        else if( wt == MSF_SELECTGROUP)
        {
            widgetSelectgroupAdjustScrollbar((MsfSelectgroup *)ptr);
        }
    }
    else
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    widgetRedraw( OP_NULL, MSF_MAX_WIDGETTYPE, choice, OP_TRUE);
    
    return iRet;          
    
}


#ifdef MSF_CONFIG_TOOLTIP
int TPIa_widgetChoiceSetElementTT (MSF_UINT32 choice, int index, 
                                   MsfStringHandle string1, MsfStringHandle string2, 
                                   MsfImageHandle image1, MsfImageHandle image2, 
                                   MsfStringHandle tooltip, 
                                   int insert)
{
    return -1;
}
#endif



/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceRemoveElement

  DESCRIPTION:
      Removes the element specified by index in the specified MsfMenu/MsfSelectGroup.


  ARGUMENTS PASSED:
      choice    The identifier of the MsfMenu or MsfSelectGroup or MsfBox.
      index     Specifies the element index that is to be removed.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceRemoveElement 
(
    MSF_UINT32 choice, 
    int index
)
{
    MsfChoice          *pChoice;
    choiceElement      *pItem;
    choiceElement      *pPreItem;
    MsfSelectgroup     *pSelectgroup;
    MsfWidgetType       wt;
    
    pSelectgroup = (MsfSelectgroup*)seekWidget( choice, &wt );
    if( !pSelectgroup )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    if( wt == MSF_MENU )
    {
        pSelectgroup =((MsfMenuWindow*)pSelectgroup)->pSelectgroup;
    }
    else if(MSF_MAINMENU == wt)
    {
        pSelectgroup =((MsfMainMenuWindow*)pSelectgroup)->pSelectgroup;
    }
    else if( MSF_SELECTGROUP != wt && MSF_BOX != wt )
    {   
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pChoice = &pSelectgroup->choice;
    if (pChoice->count <= 0)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    pItem = OP_NULL;
    
    /* find the item to be deleted  */
    if( index == 0 )
    {
        pItem = pChoice->firstElement;
        if( pItem )
        {
            pChoice->firstElement = pItem->next;
            /* if this element is in focus, reset focus to the next one */
            if ((pItem->state & MSF_CHOICE_ELEMENT_FOCUSED) && pChoice->firstElement)
            {
                pChoice->firstElement->state |= MSF_CHOICE_ELEMENT_FOCUSED;
            }
            else /* not in focus, so there must be some other elements */
            {
                pChoice->cur_item--;
            }
            
            if (pChoice->top_item > 0)
            {
                pChoice->top_item--;
            }

            if( pChoice->count == 1 )
            {
                pChoice->cur_item = -1;
            }
        }
    }
    else if( index > 0 && index < pChoice->count )
    {
        pPreItem = getChoiceElementByIndex(pChoice, index-1 );
        if (pPreItem)
        {
            pItem = pPreItem->next;
            if( pItem )
            {
                pPreItem->next = pItem->next;

                /* Adjust the top item */
                if (index <= pChoice->top_item)
                {
                    pChoice->top_item--;
                }

                /*
                  * if this element is in focus, reset focus to the next one
                  */
                if (index < pChoice->cur_item)
                {
                    pChoice->cur_item--;
                }
                else if (index == pChoice->cur_item)
                {
                    if (pPreItem->next)
                    {
                        pPreItem->next->state |= MSF_CHOICE_ELEMENT_FOCUSED;
                    }
                    else
                    {
                        pPreItem->state |= MSF_CHOICE_ELEMENT_FOCUSED;
                        pChoice->cur_item--;
                    }
                }
            }
        }
    }
    else
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pItem )
    {
        if (pItem->string1 != INVALID_HANDLE)
        {
            decrease_ref_count(pItem->string1);
        }
        if (pItem->string2 != INVALID_HANDLE)
        {
            decrease_ref_count(pItem->string2);
        }
        if (pItem->image1 != INVALID_HANDLE)
        {
            decrease_ref_count(pItem->image1);
        }
        if (pItem->image2 != INVALID_HANDLE)
        {
            decrease_ref_count(pItem->image2);
        }

        if( pSelectgroup->gadgetData.isFocused 
            && (pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            && pSelectgroup->choice.bTimerStarted == OP_TRUE)
        {
            OPUS_Stop_Timer(OPUS_TIMER_CHOICITEM_SCROLL);
            pSelectgroup->choice.bTimerStarted = OP_FALSE;
        }
        
        WIDGET_FREE(pItem );

        pChoice->count--;
        if( pChoice->count <= pChoice->elementsPerPage )
        {
            pChoice->top_item = 0;
        }
        else if( pChoice->top_item != 0 
                && pChoice->count - pChoice->top_item < pChoice->elementsPerPage )
        {
            pChoice->top_item = pChoice->count - pChoice->elementsPerPage;
        }
        
        if( wt == MSF_SELECTGROUP || wt == MSF_BOX)
        {
            if( pChoice->count < pChoice->elementsPerPage )
            {
                pSelectgroup->bItemPosCalculated = OP_FALSE;
            }
        }
//        else if( wt == MSF_MENU )
        if( wt != MSF_BOX )
        {
            widgetSelectgroupAdjustScrollbar(pSelectgroup );
        }
        
        widgetRedraw( OP_NULL, MSF_MAX_WIDGETTYPE, choice, OP_TRUE);
        
        return TPI_WIDGET_OK;
    }

    return TPI_WIDGET_ERROR_NOT_FOUND;
}  

/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceGetElemState

  DESCRIPTION:
      Retrieves the current state of the element specified by index in the specified
      MsfMenu/MsfSelectGroup. The state of the element is determined by the following
      bitmasks:
      MSF_CHOICE_ELEMENT_SELECTED 0x01
      MSF_CHOICE_ELEMENT_DISABLED 0x02
      MSF_CHOICE_ELEMENT_FOCUSED 0x04
      If MSF_CHOICE_ELEMENT_SELECTED is set to 1, it means that the element is in
      the selected state.
      If MSF_CHOICE_ELEMENT_DISABLED is set to 1, it means that the element is in
      the disabled state, that is, not selectable by the user.
      If MSF_CHOICE_ELEMENT_ FOCUSED is set to 1, it means that the specified
      element currently is in focus.
      A disabled element cannot be in focus, but with that exception any combination of
      these flags is allowed.
      Note that if an error occurs and an error code is returned the bitmasks defined above
      are not applicable and the module cannot compare the returned error code value with
      any of the defined bitmask values above. The returned error code might have the value
      ¨C1, which means that all of the bits in the returned integer would be set to 1.


  ARGUMENTS PASSED:
      choice    The identifier of the MsfMenu or MsfSelectGroup.
      index     Specifies the element index.
      
  RETURN VALUE:
      The state of the specified element according to the bitmask,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceGetElemState (MSF_UINT32 choice, int index)
{
    MsfChoice            *pChoice;
    choiceElement      *pItem;
    int                       iRet;

    iRet = getChoice(choice, &pChoice );
    if( iRet < 0 )
    {
        return iRet;
    }
    
    pItem = getChoiceElementByIndex( pChoice,  index);
    if( pItem )
    {
        return pItem->state;
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceSetElemState

  DESCRIPTION:
      Sets the element specified by index in the specified MsfMenu/MsfSelectGroup to the
      specified state according to the following bitmasks:
      MSF_CHOICE_ELEMENT_SELECTED 0x01
      MSF_CHOICE_ELEMENT_DISABLED 0x02
      MSF_CHOICE_ELEMENT_FOCUSED 0x04
      If MSF_CHOICE_ELEMENT_SELECTED is set to 1, it means that the element will
      be set in the selected state.
      If MSF_CHOICE_ELEMENT_DISABLED is set to 1, it means that the element will
      be set in the disabled state, that is, not selectable by the user.
      If MSF_CHOICE_ELEMENT_ FOCUSED is set to 1, it means that the specified
      element will be set in focus.
      A disabled element cannot be set in focus, but with that exception any combination of
      these flags is allowed.

  ARGUMENTS PASSED:
      choice    The identifier of the MsfMenu or MsfSelectGroup.
      index     Specifies the element index that is to be set in the selected state.
      state     Specifies the state of the element according to the bitmask.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceSetElemState 
(
    MSF_UINT32 choice, 
    int index, 
    int state
)
{
    MsfSelectgroup   *pSelectgroup;
    MsfWidgetType wt;
    MsfChoice         *pChoice;
    choiceElement     *pItem;
    OP_BOOLEAN        clearFocus;
    OP_BOOLEAN          clearSelect;
    OP_BOOLEAN         bNeedUpdateScrollbar = OP_FALSE;
    int               i;

    pSelectgroup = (MsfSelectgroup*)seekWidget( choice, &wt );
    if( !pSelectgroup )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    if( wt == MSF_MENU )
    {
        pSelectgroup =((MsfMenuWindow*)pSelectgroup)->pSelectgroup;
    }
    else if(MSF_MAINMENU == wt)
    {
        pSelectgroup =((MsfMainMenuWindow*)pSelectgroup)->pSelectgroup;
    }
    else if( MSF_SELECTGROUP != wt && MSF_BOX != wt )
    {   
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pChoice = &pSelectgroup->choice;
    if (pChoice->count <= 0)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    clearFocus = OP_FALSE;
    clearSelect = OP_FALSE;
    
    if ((state & MSF_CHOICE_ELEMENT_SELECTED) &&
        (pChoice->type != MsfMultipleChoice))
    {
        /* need to clear current selected item state */
        clearSelect = OP_TRUE;
    }

    if (state & MSF_CHOICE_ELEMENT_FOCUSED)
    {
        /* need to clear current focused item state */
        clearFocus = OP_TRUE;
    }

    pItem = pChoice->firstElement;
    i = 0;
    while (pItem != OP_NULL)
    {
        if (i == index)
        {
            pItem->state |= state;
            if (clearFocus)
            {
                /* 
                 * set a new focused item, so we need to reset the 
                 * top_item and cur_item properties 
                 */
                pChoice->cur_item = index;
                if( wt != MSF_BOX )
                {
                    if( index > pChoice->top_item )
                    {
                        if( pChoice->top_item + pChoice->elementsPerPage <= index )
                        {
                            pChoice->top_item = index - pChoice->elementsPerPage + 1;
                            bNeedUpdateScrollbar = OP_TRUE;
                        }
                    }
                    else if( index < pChoice->top_item )
                    {
                        pChoice->top_item = index;
                        bNeedUpdateScrollbar = OP_TRUE;
                    }

                    if( ISVALIDHANDLE(pSelectgroup->gadgetData.hVScrollBar) && bNeedUpdateScrollbar)
                    {
                        TPIa_widgetBarSetValues( pSelectgroup->gadgetData.hVScrollBar, pChoice->top_item, pChoice->count -1);
                    }
                }
            }
            if (clearSelect == OP_FALSE && clearFocus == OP_FALSE)
            {
                break;
            }
        }
        else
        {
            if ((clearSelect == OP_TRUE) &&
                (pItem->state & MSF_CHOICE_ELEMENT_SELECTED))
            {
                pItem->state &= ~(MSF_CHOICE_ELEMENT_SELECTED);
            }
            if ((clearFocus == OP_TRUE) &&
                (pItem->state & MSF_CHOICE_ELEMENT_FOCUSED))
            {
                pItem->state &= ~(MSF_CHOICE_ELEMENT_FOCUSED);
            }
        }
        pItem = pItem->next;
        i++;
    }
    
    if( pChoice->bTimerStarted == OP_TRUE )
    {
        OPUS_Stop_Timer(OPUS_TIMER_CHOICITEM_SCROLL);
        pChoice->bTimerStarted = OP_FALSE;
    }
    widgetRedraw( OP_NULL, MSF_MAX_WIDGETTYPE, choice, OP_TRUE);
    
    return TPI_WIDGET_OK;
    
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceAttrProperty

  DESCRIPTION:
      Tells the MsfChoice in which order the element attributes should be presented when
      shown on the MsfDisplay.
      The bitmask parameter tells the MsfChoice which attributes should be visible when
      shown on the MsfDisplay. Attribute visibility is determined by the following
      bitmasks:
      MSF_CHOICE_ELEMENT_ICON 0x01
      MSF_CHOICE_ELEMENT_INDEX 0x02
      MSF_CHOICE_ELEMENT_STRING_1 0x04
      MSF_CHOICE_ELEMENT_STRING_2 0x08
      MSF_CHOICE_ELEMENT_IMAGE_1 0x10
      MSF_CHOICE_ELEMENT_IMAGE_2 0x20
      The integration may choose to ignore additional MsfStrings or MsfImages if it
      exceeds the capacity of the device to display it. It must always be possible to display
      at least one MsfString or one MsfImage and also the index.
  
  ARGUMENTS PASSED:
      choice        The identifier of the MsfMenu or MsfSelectGroup.
      elementPos    Specifies the order of the element attributes. If this
                    parameter is 0, the integration will ignore it. The integration
                    may also ignore this parameter if it exceeds the capacity of
                    the device to alter the order in which the element attributes
                    are shown.
      bitmask       Specifies which element attributes that are to be shown. If
                    this parameter is 0, the integration will ignore it.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise
      the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceAttrProperty 
(
    MSF_UINT32 choice, 
    MsfElementPosition* elementPos, 
    int bitmask
)
{
    MsfChoice     *pChoice;
    int                       iRet;

    iRet = getChoice(choice, &pChoice );
    if( iRet >= 0 )
    {
        if( elementPos != OP_NULL )
        {
            op_memcpy( &pChoice->elementPos, elementPos,sizeof(MsfElementPosition));
        }

        if( bitmask != 0 )
        {

#ifdef MAIN_MENU_STYLE_TEXT
            if ( getWidgetType(choice)==MSF_MAINMENU) 
            {
                bitmask &= ~(MSF_CHOICE_ELEMENT_INDEX |MSF_CHOICE_ELEMENT_IMAGE_1 |MSF_CHOICE_ELEMENT_IMAGE_2);
            }
#endif

#ifdef MENU_STYLE_TEXT
            if (getWidgetType(choice)==MSF_MENU )
            {
                bitmask &= ~(MSF_CHOICE_ELEMENT_INDEX |MSF_CHOICE_ELEMENT_IMAGE_1 |MSF_CHOICE_ELEMENT_IMAGE_2);
            }
#endif 
            pChoice->bitmask = bitmask;
        }
    }
    
    return iRet;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceElemProperty

  DESCRIPTION:
      


  ARGUMENTS PASSED:
      
      
  RETURN VALUE:
      
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceElemProperty(MSF_UINT32 choice, int index, MsfElementPosition* elementPos, int bitmask)
{
    return 0;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceSize

  DESCRIPTION:
      Returns the number of elements present in the specified MsfMenu/MsfSelectGroup.


  ARGUMENTS PASSED:
      choice    The identifier of the MsfMenu or MsfSelectGroup.
      
  RETURN VALUE:
      The number of elements present, if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceSize
(
	MSF_UINT32 choice
)
{
    MsfChoice     *pChoice;
    int                iRet;

    iRet = getChoice(choice, &pChoice );
    if( iRet >= 0 )
    {
        iRet = pChoice->count;
    }

    return iRet;
}


/*==================================================================================================
  FUNCTION:  TPIa_widgetChoiceGetCurrentItemState

  DESCRIPTION:
     Get the index and state of the current item of the choice specified by the choice parameter.
     If the current item does not exist, will return error

  ARGUMENTS PASSED:
      choice            The identifier of the MsfMenu or MsfSelectGroup.
      pCurItemIndex  Returned the index of current item in choice
      pState            Returned the state of current item in choice
      
  RETURN VALUE:
      The number of elements present, if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetChoiceGetCurrentItemState
(
	MSF_UINT32 choice,
	int              *pCurItemIndex,
	int              *pState
)
{
    MsfChoice       *pChoice;
    MsfBox          *pBox;
    MsfWidgetType  wt;
    choiceElement  *pItem;
    int                iRet;

    if( !pCurItemIndex || !pState )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    *pCurItemIndex = -1;
    *pState = 0;
    
    iRet = getChoice(choice, &pChoice );
    if( iRet < 0 )
    {
        return iRet;
    }

    pItem = getChoiceElementByIndex( pChoice,  pChoice->cur_item);
    if( pItem )
    {
        if( getWidgetType(choice) == MSF_BOX )
        {
            pBox = (MsfBox*)seekWidget(choice, &wt);
            
            *pCurItemIndex = pChoice->cur_item + pBox->iCurPos;
        }
        else
        {
            *pCurItemIndex = pChoice->cur_item;
        }
        
        *pState = pItem->state;
        return TPI_WIDGET_OK;
    }
    
    return TPI_WIDGET_ERROR_UNEXPECTED;
}

/****************************************************************
 MENU 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetMenuCreate

  DESCRIPTION:
      Creates an empty MsfMenu with the specified MsfSize and MsfChoiceType and its own state change callback function
      returns the MsfWindowHandle. If the MsfMenu is of type MsfImplicit an
      MsfActionHandle to a MsfSelect MsfAction must be passed as input to the function in
      the parameter implicitSelectAction. When an element in the MsfMenu is selected, the
      integration will set the label in this MsfAction to String1 of the selected element and
      send the MsfAction to the MsfAction handling functionality, which will result in a call
      to the TPIc_widgetAction() function. If the MsfMenu is of another type than
      MsfImplicit the implicitSelectAction parameter should be 0.

  ARGUMENTS PASSED:
      modId             The MSF Module identifier
      type              Specifies the MsfChoiceType of the MsfMenu
      size              Pointer to an MsfSize struct that specifies the size of the
                        MsfMenu
      implicitSelect    Specifies the MsfAction that is to be used in an
      Action            MsfImplicit MsfMenu
      elementPos        Specifies the order of the element attributes. If this
                        parameter is 0, the integration will ignore it. The
                        integration may also ignore this parameter if it exceeds
                        the capacity of the device to alter the order in which the
                        element attributes that are shown.
      bitmask           Specifies which element attributes that are to be shown. If
                        this parameter is 0, the integration will ignore it.
      propertyMask      See TPIa_widgetWindowSetProperties(). If this
                        parameter is set to 0x8000, the integration will use the
                        device default properties for the MsfMenu.
      defaultStyle      The MsfStyle identifier. If this parameter is 0 the default
                        style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetMenuCreateEx 
(
    MSF_UINT8 modId, 
    MsfChoiceType type, 
    MsfSize* size, 
    MsfActionHandle implicitSelectAction, 
    MsfElementPosition* elementPos, 
    int bitmask, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfWindow *pWindow;
    MsfMenuWindow *pMenu;
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    MsfGadget       *pGadget;

    MsfGadgetHandle   hChoice;
    MsfPosition             pos;
    MsfSize                   selectgroupSize;

    if (!MSF_IS_MODULE_ID_VALID(modId) ||
         (defaultStyle != 0 && isValidHandleOfType(defaultStyle, MSF_STYLE) == OP_FALSE))
    {
        return INVALID_HANDLE;
    }

    pMenu = WIDGET_NEW(MsfMenuWindow);
    newWindowHandle = getNewHandle(modId, MSF_MENU);
    if (pMenu == OP_NULL || newWindowHandle == INVALID_HANDLE)
    {
        WIDGET_FREE(pMenu);
        newWindowHandle = INVALID_HANDLE;
    }
    else
    {
        op_memset(pMenu, 0, sizeof(MsfMenuWindow));
        pWindow = (MsfWindow *)pMenu;
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        if (size != OP_NULL)
        {
            op_memcpy(&pWindow->size, size, sizeof(MsfSize)); 
        }
        else
        {
            op_memcpy(&pWindow->size, &DefaultPropTab[MSF_MENU].size, sizeof(MsfSize));
        }
        
        op_memcpy(&pWindow->position, &DefaultPropTab[MSF_MENU].pos, sizeof(MsfPosition));

        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_MENU].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
        if (defaultStyle != 0)
        {
            replaceHandle((OP_UINT32 *)&pWindow->style, defaultStyle);
        }
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;
        pMenu->isAppMenu = OP_FALSE;      
        pWindow->cbStateChange = cbStateChange;

        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);
        
        widgetGetClientRect( pWindow, &pos, &selectgroupSize);
        
        pos.y += MENU_TITLE_AND_ITEM_OFFSET;
        selectgroupSize.height -= MENU_TITLE_AND_ITEM_OFFSET;
        
#if (PROJECT_ID == DRAGON_M_PROJECT)
        selectgroupSize.height -= MENU_TITLE_AND_ITEM_OFFSET;
#endif
        
#ifdef MENU_STYLE_TEXT
        bitmask &= ~(MSF_CHOICE_ELEMENT_INDEX |MSF_CHOICE_ELEMENT_IMAGE_1 |MSF_CHOICE_ELEMENT_IMAGE_2);
#endif 

        /*   create selectgroup  */
        hChoice = TPIa_widgetSelectgroupCreateEx(
                                            modId, 
                                            type, 
                                            &selectgroupSize, 
                                            elementPos,
                                            bitmask,
                                            DefaultPropTab[MSF_SELECTGROUP].propertyMask, 
                                            SG_SHOW_STYLE_MENU 
                                            |SG_SHOW_STYLE_NO_BORDER 
                                            |SG_SHOW_STYLE_SEQUENCE_LAYOUT
#ifndef  MENU_SHOW_SCROLLBAR
                                            |SG_SHOW_STYLE_NO_SCROLLBAR
#endif                                            
                                            |SG_SHOW_STYLE_FOCUS_DRAW_LINE, 
                                            defaultStyle,
                                            OP_NULL);

        if( !ISVALIDHANDLE( hChoice ) )
        {
            WIDGET_FREE(pMenu);
            return INVALID_HANDLE;
        }

        pGadget = (MsfGadget*)seekWidget( hChoice, OP_NULL );
        pMenu->pSelectgroup = (MsfSelectgroup*)pGadget;

        pGadget->softKeyList[0] = pGadget->softKeyList[1] = pGadget->softKeyList[2] = OP_NULL;
        
        /* calc the elementsPerPage */
        pMenu->pSelectgroup->choice.elementsPerPage = selectgroupSize.height / CHOICE_MENU_LINE_HEIGHT;

        /* scroll circularly  */
        pMenu->pSelectgroup->choice.scrollCircularly  = OP_TRUE;
        
        /* not alow the item scroll-period show  */
//        pMenu->pSelectgroup->choice.bTimerStarted = OP_TRUE;
        
        SP_list_insert_to_head(&widgetQueue[MSF_MENU], pMenu);
        
        TPIa_widgetWindowAddGadget( newWindowHandle, hChoice, &pos, OP_NULL );
        TPIa_widgetRelease( hChoice );  
        
        TPIa_widgetAddAction(newWindowHandle, implicitSelectAction);
        
    }
    return newWindowHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetMenuAppCreate

  DESCRIPTION:
      Creates an empty MsfApplicationMenu with its own state change callback function and returns the MsfWindowHandle. An
      MsfActionHandle to a MsfSelect MsfAction must be passed as input to the function in
      the parameter implicitSelectAction. When an element in the MsfApplicationMenu is
      selected, the integration will set the label in this MsfAction to String1 of the selected
      element and send the MsfAction to the MsfAction handling functionality, which will
      result in a call to the TPIc_widgetAction() function.

  ARGUMENTS PASSED:
      modId             The MSF Module identifier
      implicitSelect    Specifies the MsfAction that is to be used in an
      Action            MsfImplicit MsfMenu
      elementPos        Specifies the order of the element attributes. If this
                        parameter is 0, the integration will ignore it. The
                        integration may also ignore this parameter if it exceeds
                        the capacity of the device to alter the order in which the
                        element attributes that are shown.
      bitmask           Specifies which element attributes that are to be shown. If
                        this parameter is 0, the integration will ignore it.
      propertyMask      See TPIa_widgetWindowSetProperties().If this
                        parameter is set to 0x8000, the integration will use the
                        device default properties for the MsfMenu.
      defaultStyle      The MsfStyle identifier. If this parameter is 0 the default
                        style of the device will be used.
      cbStateChange    The state change callback function
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetMenuAppCreateEx 
(
    MSF_UINT8 modId, 
    MsfActionHandle implicitSelectAction,
    MsfElementPosition* elementPos, 
    int bitmask,
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfWindow *pWindow;
    MsfMenuWindow *pMenu;
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    
    MsfGadgetHandle   hChoice;
    MsfPosition             pos;
    MsfSize                   selectgroupSize;
    MsfGadget             *pGadget;
    
    if (!MSF_IS_MODULE_ID_VALID(modId) ||
         (isValidHandleOfType(defaultStyle, MSF_STYLE) == OP_FALSE && defaultStyle != 0))
    {
        return INVALID_HANDLE;
    }
    
    pMenu = WIDGET_NEW(MsfMenuWindow);
    newWindowHandle = getNewHandle(modId, MSF_MENU);
    if (pMenu == OP_NULL || newWindowHandle == INVALID_HANDLE)
    {
        WIDGET_FREE(pMenu);
        newWindowHandle = INVALID_HANDLE;
    }
    else
    {
        op_memset(pMenu, 0, sizeof(MsfMenuWindow));
        pWindow = (MsfWindow *)pMenu;
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        
        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_MENU].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
        
        replaceHandle((OP_UINT32 *) &pWindow->style, defaultStyle );
        op_memcpy(&pWindow->size, &DefaultPropTab[MSF_MENU].size, sizeof(MsfSize));
        op_memcpy(&pWindow->position, &DefaultPropTab[MSF_MENU].pos, sizeof(MsfPosition));
        
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;
        pMenu->isAppMenu = OP_TRUE;
        pWindow->cbStateChange = cbStateChange;
        
        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);
        
        widgetGetClientRect( pWindow, &pos, &selectgroupSize);
        
        pos.y += MENU_TITLE_AND_ITEM_OFFSET;
        selectgroupSize.height -= MENU_TITLE_AND_ITEM_OFFSET;
        
#if (PROJECT_ID == DRAGON_M_PROJECT)
        selectgroupSize.height -= MENU_TITLE_AND_ITEM_OFFSET;
#endif
        
#ifdef MENU_STYLE_TEXT
        bitmask &= ~(MSF_CHOICE_ELEMENT_INDEX |MSF_CHOICE_ELEMENT_IMAGE_1 |MSF_CHOICE_ELEMENT_IMAGE_2);
#endif 
        /*   create selectgroup  */
        hChoice = TPIa_widgetSelectgroupCreateEx(
                                            modId, 
                                            MsfImplicitChoice, 
                                            &selectgroupSize, 
                                            elementPos,
                                            bitmask,
                                            DefaultPropTab[MSF_SELECTGROUP].propertyMask, 
                                            SG_SHOW_STYLE_MENU
                                            | SG_SHOW_STYLE_NO_BORDER
                                            | SG_SHOW_STYLE_SEQUENCE_LAYOUT
#ifndef  MENU_SHOW_SCROLLBAR
                                            | SG_SHOW_STYLE_NO_SCROLLBAR
#endif                                            
                                            | SG_SHOW_STYLE_FOCUS_DRAW_LINE,
                                            defaultStyle,
                                            OP_NULL);

        if( !ISVALIDHANDLE( hChoice ) )
        {
            WIDGET_FREE(pMenu);
            return INVALID_HANDLE;
        }

        pGadget = (MsfGadget*)seekWidget( hChoice, OP_NULL );
        pMenu->pSelectgroup = (MsfSelectgroup*)pGadget;

        pGadget->softKeyList[0] = pGadget->softKeyList[1] = pGadget->softKeyList[2] = OP_NULL;
        
        /* calc the elementsPerPage */
        pMenu->pSelectgroup->choice.elementsPerPage = selectgroupSize.height / CHOICE_MENU_LINE_HEIGHT;

        /* scroll circularly  */
        pMenu->pSelectgroup->choice.scrollCircularly  = OP_TRUE;
        
        /* not alow the item scroll-period show  */
//        pMenu->pSelectgroup->choice.bTimerStarted = OP_TRUE;
        
        SP_list_insert_to_head(&widgetQueue[MSF_MENU], pMenu);
        
        TPIa_widgetWindowAddGadget( newWindowHandle, hChoice, &pos, OP_NULL );
        TPIa_widgetRelease( hChoice );  
        
        TPIa_widgetAddAction(newWindowHandle, implicitSelectAction);
    }
    return newWindowHandle;
}


/****************************************************************
 DIALOG 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetDialogCreate

  DESCRIPTION:
      Creates an MsfDialog with the specified MsfDialogType and its own state change callback function and returns the
      MsfWindowHandle. The MsfDialog contains the specified MsfString.

  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      dialogText    Specifies the text that is to be shown in the MsfDialog
      type          Specifies the MsfDialogType of the MsfDialog
      timeoutTime   Specifies the timeout time in milliseconds.
      propertyMask  See TPIa_widgetWindowSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfDialog.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange  The state change callback function
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetDialogCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle dialogText, 
    MsfDialogType type, 
    MSF_UINT32 timeoutTime, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfWindow *pWindow;
    MsfDialog *pDialog;
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    RM_ICON_T   *pIcon;
    RM_BITMAP_T *pBitmap;
    
    if (!MSF_IS_MODULE_ID_VALID(modId) ||
        !ISVALIDHANDLE(dialogText) ||
        (defaultStyle != 0 && isValidHandleOfType(defaultStyle, MSF_STYLE) == OP_FALSE))
    {
        return INVALID_HANDLE;
    }

    pDialog = WIDGET_NEW(MsfDialog);
    newWindowHandle = getNewHandle(modId, MSF_DIALOG);
    
    if (pDialog == OP_NULL || newWindowHandle == INVALID_HANDLE)
    {
        WIDGET_FREE(pDialog);
        newWindowHandle = INVALID_HANDLE;
    }
    else
    {
        op_memset(pDialog, 0, sizeof(MsfDialog));
        pWindow = (MsfWindow *)pDialog;
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        
/*
        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_DIALOG].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
*/        
        pWindow->propertyMask = MSF_WINDOW_PROPERTY_SINGLEACTION | MSF_WINDOW_PROPERTY_NOTIFY | MSF_WINDOW_PROPERTY_ALWAYSONTOP;
        
        if (defaultStyle != 0)
        {
            replaceHandle((OP_UINT32 *) &pWindow->style, defaultStyle );
        }

#ifdef DIALOG_BG_ONLY_TYPE_PICTURE
        /*  calc the dialog size  */
        if(RM_IsItBitmap(BMP_POPUP_WARNING))
        {
            pBitmap = (RM_BITMAP_T *)util_get_bitmap_from_res(BMP_POPUP_WARNING);
            pWindow->size.width = (MSF_INT16)pBitmap->biWidth;
            pWindow->size.height= (MSF_INT16)pBitmap->biHeight;
        } 
        else if( RM_IsItIcon(BMP_POPUP_WARNING) )
        {
            pIcon = (RM_ICON_T *)util_get_icon_from_res(BMP_POPUP_WARNING);
            pWindow->size.width = (MSF_INT16)pIcon->biWidth;
            pWindow->size.height= (MSF_INT16)pIcon->biHeight;
        
        }       
#else
        /*  calc the dialog size  */
        if(RM_IsItBitmap(BMP_POPUP_BACKGROUND))
        {
            pBitmap = (RM_BITMAP_T *)util_get_bitmap_from_res(BMP_POPUP_BACKGROUND);
            pWindow->size.width = (MSF_INT16)pBitmap->biWidth;
            pWindow->size.height= (MSF_INT16)pBitmap->biHeight;
        } 
        else if( RM_IsItIcon(BMP_POPUP_BACKGROUND) )
        {
            pIcon = (RM_ICON_T *)util_get_icon_from_res(BMP_POPUP_BACKGROUND);
            pWindow->size.width = (MSF_INT16)pIcon->biWidth;
            pWindow->size.height= (MSF_INT16)pIcon->biHeight;
        
        }       
#endif
        
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;

        if (dialogText != 0)
        {
            replaceHandle((OP_UINT32 *)&pDialog->dialogText, dialogText);
        }
        pDialog->type = type;
        pDialog->timeoutTime = timeoutTime;
        pDialog->inputCount = 0;
        
        /*  adjust client area   */
//        widgetWindowAdjustClientArea(pWindow);

        pWindow->clientOrigin.x = DIALOG_LEFT_BORDER;
        pWindow->clientOrigin.y = DIALOG_TOP_BORDER;
        pWindow->cbStateChange = cbStateChange;
        
        SP_list_insert_to_head(&widgetQueue[MSF_DIALOG], pDialog);
    }
    
    return newWindowHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetDialogSetAttr

  DESCRIPTION:
      Sets the attributes used in the specified MsfDialog.
      
      The parameter dialogText specifies the MsfString containing the text that is to be
      shown on the specified MsfDialog.
      
      The parameter timeoutTime specifies the timeout for the specified MsfDialog, that is,
      how long the MsfDialog is to be shown. The timeout time is specified in milliseconds.
      The value 0 means that the MsfDialog must be dismissed by the user.


  ARGUMENTS PASSED:
      dialog        The MsfDialog identifier.
      dialogText    Specifies the dialog text, 0 means that the dialog text
                    remains unchanged.
      timeoutTime   Specifies the timeout time in milliseconds.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetDialogSetAttr 
(
    MsfWindowHandle dialog, 
    MsfStringHandle dialogText, 
    MSF_UINT32 timeoutTime
)
{
    MsfDialog *pDialog;
    MsfWidgetType widgetType;
    int ret = TPI_WIDGET_OK;

    pDialog = (MsfDialog *)seekWidget(dialog, &widgetType);
    if (pDialog != OP_NULL && 
        widgetType == MSF_DIALOG )
    {
        if( ISVALIDHANDLE( dialogText ))
        {
            replaceHandle((OP_UINT32 *)&pDialog->dialogText, dialogText);
        }
        
        pDialog->timeoutTime = timeoutTime;

        if( pDialog->type == MsfPrompt )
        {
            pDialog->bNeedAdjustInputPos = OP_TRUE;
        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetDialogAddInput

  DESCRIPTION:
      Adds an input field to the specified MsfDialog and returns the input field index
      (starting from zero). The MsfDialog must be of the type MsfPrompt.

  ARGUMENTS PASSED:
      dialog        The MsfDialog identifier.
      label         Specifies the label of the input field.
      inputText     Specifies the default text that is to be shown in the input
                    field. The contents of the MsfString specified by this
                    parameter are copied into the string contained in the
                    MsfDialog input field.
      inputMode     Specifies the MsfTextType of the input field.
      maxSize       Specifies the maximum number of characters that can be
                    entered into the MsfDialog.
      
  RETURN VALUE:
      The input field index if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetDialogAddInput 
(
    MsfWindowHandle dialog, 
    MsfStringHandle label, 
    MsfStringHandle inputText, 
    MsfTextType inputMode, 
    int maxSize
)
{
    MsfDialog *pDialog;
    MsfWidgetType widgetType;
    MsfGadgetHandle hInput;
    MsfPosition         clientPos;
    MsfSize              clientSize;
    int                    iRet = TPI_WIDGET_ERROR_UNEXPECTED;

    pDialog = (MsfDialog *)seekWidget(dialog, &widgetType);

    if (pDialog != OP_NULL 
        && widgetType == MSF_DIALOG
        && pDialog->type == MsfPrompt) /* only MsfPrompt can have inputfield */
    {
        widgetGetClientRect((const MsfWindow*)pDialog, &clientPos, &clientSize);
        
        clientSize.width -= 4;
        
        if( ISVALIDHANDLE(label))
        {
            clientSize.height = 36;
        }
        else
        {
            clientSize.height = 20;
        }
        
        hInput = TPIa_widgetTextInputCreate( 
                                    getModID(dialog),
                                    0, 
                                    inputText,
                                    inputMode, 
                                    maxSize, 
                                    1, 
                                    &clientSize, 
                                    0x8000, 
                                    0);
                                    
        if(ISVALIDHANDLE(hInput))
        {
            if( ISVALIDHANDLE(label) )
            {
                TPIa_widgetSetTitle(hInput, label );
            }
            
            pDialog->inputCount++;
            pDialog->bNeedAdjustInputPos = OP_TRUE;
            iRet = TPIa_widgetWindowAddGadget(dialog, hInput, &clientPos, OP_NULL);
            TPIa_widgetRelease(hInput);
            if( iRet < 0 )
            {
                pDialog->bNeedAdjustInputPos = OP_FALSE;
                pDialog->inputCount--;
                return iRet;
            }
            
            iRet = pDialog->inputCount - 1;
        }
        else 
        {
            return TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        }
        
    }
    
    return iRet;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetDialogRemoveInput

  DESCRIPTION:
      Removes the specified input field from the specified MsfDialog. The MsfDialog must
      be of the type MsfPrompt.


  ARGUMENTS PASSED:
      dialog    The MsfDialog identifier.
      index     The index of the input field that is to be removed.
      
  RETURN VALUE:
      The MsfGadget index if the operation was successful, otherwise
      the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetDialogRemoveInput (MsfWindowHandle dialog, int index)
{
    MsfDialog          *pDialog;
    MsfWidgetType widgetType;
    MsfTextInput       *pInput;
    int                   iRet = TPI_WIDGET_ERROR_INVALID_PARAM;
    
    pDialog = (MsfDialog *)seekWidget(dialog, &widgetType);
    if( pDialog && widgetType == MSF_DIALOG && pDialog->type == MsfPrompt )
    {
        pInput = widgetDialogGetInputByIndex( pDialog, index );
        if( pInput )
        {
            pDialog->inputCount--;
            pDialog->bNeedAdjustInputPos = OP_TRUE;
            TPIa_widgetRemove(dialog, pInput->gadgetData.gadgetHandle );
            iRet = TPI_WIDGET_OK;
        }
    }
    
    return iRet;
}
/*==================================================================================================
  FUNCTION:  TPIa_widgetDialogSetInputAttr

  DESCRIPTION:
      Sets the specified attributes of an input field to the specified MsfDialog and returns
      the input field index (starting from zero). The MsfDialog must be of the type
      MsfPrompt.


  ARGUMENTS PASSED:
      dialog        The MsfDialog identifier.
      index         The index of the input field that is to be modified.
      label         Specifies the label of the input field.
      inputText     Specifies the default text that is to be shown in the input field.
                    The contents of the MsfString specified in this parameter are
                    copied into the string contained in the MsfDialog input field.
      inputMode     Specifies the MsfTextType of the input field.
      maxSize       Specifies the maximum number of characters that can be
                    entered into the MsfDialog
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise
      the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetDialogSetInputAttr 
(
    MsfWindowHandle dialog, 
    int index, 
    MsfStringHandle label, 
    MsfStringHandle inputText, 
    MsfTextType inputMode, 
    int maxSize
)
{
    MsfDialog *pDialog;
    MsfGadget        *pGadget;
    MsfWidgetType  widgetType;
    int                   iRet = TPI_WIDGET_ERROR_INVALID_PARAM;

    pDialog = (MsfDialog *)seekWidget(dialog, &widgetType);
    if( pDialog && widgetType == MSF_DIALOG && pDialog->type == MsfPrompt )
    {
        pGadget = (MsfGadget*)widgetDialogGetInputByIndex( pDialog, index );
        if( pGadget )
        {
            TPIa_widgetTextSetText( pGadget->gadgetHandle, 0, inputMode, maxSize, inputText, 1);
            if(ISVALIDHANDLE(pGadget->title ) )
            {
                if( !ISVALIDHANDLE(label))
                {
                    pGadget->size.height -= 16;
                    pDialog->bNeedAdjustInputPos = OP_TRUE;
                }
            }
            else if( ISVALIDHANDLE(label))
            {
                pGadget->size.height += 16;
                pDialog->bNeedAdjustInputPos = OP_TRUE;
            }
            
            TPIa_widgetSetTitle(pGadget->gadgetHandle, label);

            iRet = TPI_WIDGET_OK;
        }
    }
    
    return iRet;
}

/****************************************************************
 ACTION 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetActionCreateEx

  DESCRIPTION:
      Creates a new MsfAction with the specified label and its own state change callback function, MsfActionType and priority


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      label         Specifies the label of the MsfAction
      actionType    Specifies the MsfActionType of the MsfAction
      priority      Specifies the priority of the MsfAction
      propertyMask  MSF_ACTION_PROPERTY_ENABLED (0x0001),
                    enables/disables the MsfAction, 1 means enable.
                    
                    MSF_ACTION_PROPERTY_SINGLEACTION
                    (0x0002), turns On/Off the MsfAction blocking
                    functionality, 1 means On.
                    
                    If this parameter is set to 0x8000, the integration will use
                    the device default properties for the MsfAction.
     cbStateChange  The state change callback function
      
  RETURN VALUE:
      The new MsfActionHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
#ifndef   WIDGET_DEBUG
MsfActionHandle TPIa_widgetActionCreateEx(MSF_UINT8 modId, MsfStringHandle label, int actionType, int priority, int propertyMask, widget_action_callback_t cbActionNotify)
{
#else
MsfActionHandle WIDGET_DEBUG_ACTION_CREATE(MSF_UINT8 modId, MsfStringHandle label, int actionType, int priority, int propertyMask, widget_action_callback_t cbActionNotify, const char *filename, int lineno)
{
#endif
    MsfAction *pAction;
    MsfActionHandle newActionHandle = INVALID_HANDLE;
    if (!MSF_IS_MODULE_ID_VALID(modId))
    {
        return INVALID_HANDLE;
    }
    
    pAction = WIDGET_NEW(MsfAction);
    
    if (pAction == OP_NULL)
    {
        return INVALID_HANDLE;
    }

    op_memset(pAction, 0, sizeof(MsfAction));
    newActionHandle = getNewHandle(modId, MSF_ACTION);
    
    pAction->actionHandle = newActionHandle;
    pAction->actionType = (MsfActionType)actionType;
    replaceHandle((OP_UINT32 *)&pAction->label, label);
    pAction->modId = modId;
    pAction->priority = priority;
    if( propertyMask = 0x8000 )
    {
        propertyMask = DefaultPropTab[MSF_ACTION].propertyMask;
    }
    pAction->propertyMask = propertyMask;
    pAction->referenceCounter = 1;
    pAction->isReleased = OP_FALSE;
    pAction->cbActionNotify = cbActionNotify;
    SP_list_insert_to_head(&widgetQueue[MSF_ACTION], pAction);

#ifdef   WIDGET_DEBUG
    {
        const char *pPtr;
        
        pPtr = strrchr(filename, '\\');
        if( pPtr )
        {
            pPtr++;
            op_printf( "\n%s line=%d [ACTION] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)newActionHandle );
        }
    }
#endif    
    return newActionHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetActionSetAttr

  DESCRIPTION:
      Sets the attributes from the specified MsfAction.


  ARGUMENTS PASSED:
      action        The MsfAction identifier.
      actionType    Specifies the MsfActionType of the MsfAction.
      priority      Specifies the priority of the MsfAction.
      
  RETURN VALUE:
      The MsfAction priority if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetActionSetAttr 
(
    MsfActionHandle action, 
    int actionType, 
    int priority
)
{
    MsfAction* pAction;
    MsfWidgetType pWidgetType;
    int ret = TPI_WIDGET_OK;
    
    pAction = (MsfAction*)seekWidget(action, &pWidgetType);
    if( pAction != OP_NULL && pWidgetType == MSF_ACTION)
    {
        pAction->actionType = (MsfActionType)actionType;
        pAction->priority = priority;
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetActionGetAttr

  DESCRIPTION:
      Gets the attributes from the specified MsfAction.


  ARGUMENTS PASSED:
      action        The MsfAction identifier
      actionType    Specifies the MsfActionType of the MsfAction
      priority      Specifies the priority of the MsfAction
      
  RETURN VALUE:
      The MsfAction priority if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetActionGetAttr 
(
    MsfActionHandle action, 
    int* actionType, 
    int* priority
)
{
    MsfAction* pAction;
    MsfWidgetType pWidgetType;
    int ret = TPI_WIDGET_OK;
    
    pAction = (MsfAction*)seekWidget(action, &pWidgetType);
    if( pAction != OP_NULL && 
        pWidgetType == MSF_ACTION &&
        actionType != OP_NULL &&
        priority != OP_NULL)
    {
        *actionType = pAction->actionType;
        *priority = pAction->priority;
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    return ret;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetActionSetProperties

  DESCRIPTION:
      Turns on/off the different attributes and properties of the specified MsfAction.


  ARGUMENTS PASSED:
      action        The MsfAction identifier.
      propertyMask  MSF_ACTION_PROPERTY_ENABLED (0x0001).
                    Enables or disables the MsfAction, 1 means enable
                    (default).
                    
                    MSF_ACTION_PROPERTY_SINGLEACTION
                    (0x0002). Turns on or off the MsfAction blocking
                    functionality, 1 means On.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetActionSetProperties 
(
    MsfActionHandle action, 
    int propertyMask
)
{
    MsfAction* pAction;
    MsfWidgetType pWidgetType;
    int ret = TPI_WIDGET_OK;
    
    pAction = (MsfAction*)seekWidget(action, &pWidgetType);
    if( pAction != OP_NULL && pWidgetType == MSF_ACTION)
    {
        pAction->propertyMask = propertyMask;
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    return ret;
}

/****************************************************************
 GADGET 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetGadgetSetProperties

  DESCRIPTION:
      Turns on or off the different attributes and properties of the specified MsfGadget.


  ARGUMENTS PASSED:
      gadget        The MsfGadget identifier
      propertyMask  MSF_GADGET_PROPERTY_SCROLLBARVER
                    (0x0001), shows or hides the vertical scrollbar if
                    needed, 1 means show.
                    
                    MSF_GADGET_PROPERTY_SCROLLBARHOR
                    (0x0002), shows or hides the horizontal scrollbar if
                    needed, 1 means show.
                    
                    MSF_GADGET_PROPERTY_LABEL (0x0004),
                    shows or hides the label, 1 means show.
                    
                    MSF_GADGET_PROPERTY_BORDER (0x0008),
                    shows or hides the border, 1 means show.
                    
                    MSF_GADGET_PROPERTY_TICKERMODE
                    (0x0010), turns on or off the ticker mode, 1 means On.
                    
                    MSF_GADGET_PROPERTY_FOCUS (0x0020), tells
                    if the MsfGadget is focusable (1) or not (0).
                    
                    MSF_GADGET_PROPERTY_NOTIFY (0x0040),
                    turns on or off the notification functionality, 1 means
                    On.
                    
                    MSF_GADGET_PROPERTY_ALWAYSONTOP
                    (0x0080), turns on or off the AlwaysOnTop
                    functionality, 1 means On.
                    
                    MsfVerticalPos (0x0300), these two bits specify the
                    vertical alignment of the MsfGadget according to the
                    MsfVerticalPos enumeration. Note that this property
                    pertains to the contents inside the MsfGadget and not
                    the MsfGadget itself. For example, the MsfVerticalPos
                    property of an MsfStringGadget pertains to the
                    alignment of the MsfString inside the MsfStringGadget.
                    
                    MsfHorizontalPos (0x0C00), these two bits specify the
                    horizontal alignment of the MsfGadget according to the
                    MsfHorizontalPos enumeration. Note that this property
                    pertains to the contents inside the MsfGadget and not
                    the MsfGadget itself. For example, the
                    MsfHorizontalPos property of an MsfStringGadget
                    pertains to the alignment of the MsfString inside the
                    MsfStringGadget.
                    
                    MSF_GADGET_PROPERTY_MOVERESIZE
                    (0x1000), specifies if the integration is allowed to move
                    or resize the MsfGadget due to user events, 1 means that
                    the integration is allowed to move or resize the
                    MsfGadget.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetGadgetSetProperties 
(
    MsfGadgetHandle gadget, 
    int propertyMask
)
{
 
    MsfGadget* pGadget;
    MsfWidgetType widgetType;
    int ret = TPI_WIDGET_OK;

    pGadget = ( MsfGadget *)seekWidget(gadget, &widgetType);
    if( pGadget != OP_NULL && IS_GADGET(widgetType))
    {
    
        if( widgetType == MSF_TEXTINPUT )
        {
#ifndef _KEYPAD
            if( pGadget->parent 
            && getWidgetType(pGadget->parent->windowHandle) == MSF_EDITOR )
            {
                if( propertyMask == 0x8000 )
                {
                    propertyMask = MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_FOCUS |MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM;
                }
                else
                {
                    propertyMask &= ~MSF_GADGET_PROPERTY_PEN_CLICK;
                    propertyMask |= MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM;
                }
                pGadget->propertyMask = propertyMask;
            }
            else
            {
                if( propertyMask != 0x8000 )
                {
                    propertyMask |= MSF_GADGET_PROPERTY_PEN_CLICK;
                }
                pGadget->propertyMask = propertyMask;
            }	  	
#else        
            if( propertyMask != 0x8000 )
            {
                propertyMask |= MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM;
            }
            pGadget->propertyMask = propertyMask;


#endif
        }
        else
        {
            pGadget->propertyMask=propertyMask;
        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;

}

/*==================================================================================================
  FUNCTION:  TPIa_widgetGadgetSetAlignment

  DESCRIPTION:
      


  ARGUMENTS PASSED:
      
      
  RETURN VALUE:
      
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetGadgetSetAlignment
(
    MsfGadgetHandle gadget, 
    MsfAlignment* alignment
)
{
    MsfGadget* pGadget;
    MsfWidgetType widgetType;
    int ret = TPI_WIDGET_OK;

    pGadget = ( MsfGadget *)seekWidget(gadget, &widgetType);
    if( pGadget != OP_NULL && 
        IS_GADGET(widgetType)&&
        alignment != OP_NULL )
    {
        op_memcpy(&pGadget->alignment, alignment, sizeof(MsfAlignment));
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}



/****************************************************************
 STRING GADGET 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetStringGadgetCreateEx

  DESCRIPTION:
      Creates a MsfStringGadget with the specified MsfSize and its own state change callback funcion and returns the
      MsfGadgetHandle. The MsfStringGadget contains the specified MsfString.

  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      text          Specifies the text to be contained within the
                    MsfStringGadget
      size          Specifies the MsfSize of the MsfStringGadget. 0 means
                    that the MsfStringGadget will be created with the default
                    MsfSize, i.e. with the MsfSize of the MsfString.
      singleLine    Specifies if the entered text is to be shown on one single
                    line (1) or on multiple lines (0), if the content size is
                    bigger than the MsfSize of the MsfTextInput. I.e. if the
                    content should be horisontally scrollable or vertically
                    scrollable.
      propertyMask  See TPIa_widgetGadgetSetProperties().If this parameter
                    is set to 0x8000, the integration will use the device
                    default properties for the MsfStringGadget.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetStringGadgetCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle text, 
    MsfSize* size, 
    int singleLine, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget *pGadget;
    MsfStringGadget* pStringGadget;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    
    if (!MSF_IS_MODULE_ID_VALID(modId) ||
        !ISVALIDHANDLE(text)||
        (defaultStyle != 0 && isValidHandleOfType(defaultStyle, MSF_STYLE)== OP_FALSE))
    {
        return INVALID_HANDLE;
    }

    pStringGadget = WIDGET_NEW(MsfStringGadget);
    if( pStringGadget != OP_NULL)
    {
        op_memset(pStringGadget, 0, sizeof(MsfStringGadget));
        newGadgetHandle = getNewHandle(modId, MSF_STRINGGADGET);
        if(newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pStringGadget);
        }
        else
        {
            pGadget = (MsfGadget *)pStringGadget;
            pGadget->gadgetHandle = newGadgetHandle;
            pGadget->modId = modId;

            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_STRINGGADGET].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter = 1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;

#ifdef  _KEYPAD
            {     /* Set Gadget SoftKey  */
                MsfWidgetType  wt;
                OP_INT16          ind;
                
                wt = MSF_STRINGGADGET - MSF_WINDOW_END - 1;

                for( ind = 0; ind < 3; ind++)
                {
                    if( gadgetSoftKeyIDs[ wt][ind] != 0 )
                    {
                        pGadget->softKeyList[ind] = widgetGetPredefinedString( gadgetSoftKeyIDs[wt][ind]);
                    }
                }
            }
#endif            
            
            if( size )
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }
            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_STRINGGADGET].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_STRINGGADGET].pos, sizeof(MsfPosition));
            
            pStringGadget->singleLine = singleLine;
            replaceHandle((OP_UINT32 *)&pStringGadget->text, text);
            
            pStringGadget->bNeedAdjust = OP_TRUE;
            pStringGadget->pPageStarIndexList = pStringGadget->pageStarIndexListBuf;
            
            SP_list_insert_to_head(&widgetQueue[MSF_STRINGGADGET], pStringGadget);
        }
    }

    return newGadgetHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetStringGadgetSet

  DESCRIPTION:
      Sets the MsfString in the specified MsfStringGadget.


  ARGUMENTS PASSED:
      stringGadget          The MsfStringGadget identifier.
                            text Specifies the text to be contained within the
                            MsfStringGadget.
      singleLine            Specifies if the entered text is to be shown on one line (1)
                            or on multiple lines (0), if the content size is bigger than
                            the MsfSize of the MsfTextInput. This determines
                            whether the content should be horizontally or vertically
                            scrollable.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetStringGadgetSet 
(
    MsfGadgetHandle stringGadget, 
    MsfStringHandle text, 
    int singleLine
)
{
    MsfStringGadget* pStrGadget;
    MsfWidgetType  widgetType;
    int ret = TPI_WIDGET_ERROR_INVALID_PARAM;

    pStrGadget = (MsfStringGadget*)seekWidget(stringGadget, &widgetType);
    if (pStrGadget != OP_NULL &&
        widgetType == MSF_STRINGGADGET)
    {
        replaceHandle((OP_UINT32 *)&pStrGadget->text, text);
        
        /*
         * if singleLine == RESERVE_ORIGINAL_SETTING , don't change the singleline property 
         */
        if( singleLine != RESERVE_ORIGINAL_SETTING) 
        {
            pStrGadget->singleLine = singleLine;
        }
        
        widgetRedraw((void*) pStrGadget, widgetType, stringGadget, OP_TRUE );
        
        pStrGadget->bNeedAdjust = OP_TRUE;
        ret = TPI_WIDGET_OK;
    }
  
    return ret;
}


 
MsfGadgetHandle TPIa_widgetTextInputCreate
(
    MSF_UINT8 modId, 
    MsfStringHandle initialString, 
    MsfStringHandle inputString, 
    MsfTextType type, 
    int maxSize, 
    int singleLine, 
    MsfSize* size, 
    int propertyMask, 
    MsfStyleHandle defaultStyle
)
{
  
    if(propertyMask != 0x8000)
    {
#ifndef _KEYPAD
        propertyMask |=MSF_GADGET_PROPERTY_PEN_CLICK;
#else
        propertyMask|=MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM;
#endif
    }
    return (TPIa_widgetTextInputCreateEx(modId,initialString,inputString,type,maxSize,singleLine,&(*size),propertyMask,defaultStyle,MSF_NULL));
}



/****************************************************************
 TEXT INPUT GADGET 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetTextInputCreateEx

  DESCRIPTION:
      Creates an MsfTextInput with the specified MsfSize and initial MsfString and its own state change callback funtion and returns
      the MsfGadgetHandle. The TextInput has the specified maximum size and
      MsfTextType constraint. The contents including the MsfStyle values of the MsfString,
      specified by the inputString parameter in this function call, are copied into the string
      contained in the MsfTextInput.


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      initialString Specifies the text to be shown initially in the
                    MsfTextInput
      inputString   The MsfString identifier, containing the text to be set
                    as input string.
      type          Specifies the MsfTextType of the MsfTextInput
      maxSize       Specifies the maximum number of characters that can
                    be entered into the MsfTextInput
      singleLine    Specifies if the entered text is to be shown on one
                    single line (1) or on multiple lines (0), if the content
                    size is bigger than the MsfSize of the MsfTextInput.
                    i.e. if the content should be horisontally scrollable or
                    vertically scrollable.
      size          Pointer to an MsfSize struct that specifies the size of
                    the MsfTextInput
      propertyMask  See TPIa_widgetGadgetSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfTextInput.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the
                    default style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetTextInputCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle initialString, 
    MsfStringHandle inputString, 
    MsfTextType type, 
    int maxSize, 
    int singleLine, 
    MsfSize* size, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget *pGadget;
    MsfTextInput *pTextInput;
    MsfInput* pInput;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
#if 0
    MsfGadgetHandle hBar;
    MsfBar               *pBar;
    MsfSize      sizeScrollbar;
#endif
    
    if (!MSF_IS_MODULE_ID_VALID(modId) ||
        (defaultStyle != 0 && isValidHandleOfType(defaultStyle, MSF_STYLE)== OP_FALSE))
    {
        return INVALID_HANDLE;
    }

    pTextInput = WIDGET_NEW(MsfTextInput);
    if( pTextInput != OP_NULL)
    {
        op_memset(pTextInput, 0, sizeof(MsfTextInput));
        newGadgetHandle = getNewHandle(modId, MSF_TEXTINPUT);
        if(newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pTextInput);
        }
        else
        {
            int iRet;
//            OP_UINT16 iTabLineCnt;

            pGadget = (MsfGadget *)pTextInput;
            pInput = &(pTextInput->input);

            pGadget->modId = modId;
            pGadget->gadgetHandle = newGadgetHandle;
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter =1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;

#ifdef  _KEYPAD
            {     /* Set Gadget SoftKey  */
                MsfWidgetType  wt;
                OP_INT16          ind;
                
                wt = MSF_TEXTINPUT - MSF_WINDOW_END - 1;

                for( ind = 0; ind < 3; ind++)
                {
                    if( gadgetSoftKeyIDs[ wt][ind] != 0 )
                    {
                        pGadget->softKeyList[ind] = widgetGetPredefinedString( gadgetSoftKeyIDs[wt][ind]);
                    }
                }
            }
#endif
                
            pInput->textProp.alignment.verticalPos = MsfVerticalPosDefault;
            pInput->textProp.alignment.horisontalPos  = MsfHorizontalPosDefault;        
            
            /* initialString*/
            replaceHandle((OP_UINT32 *)&pInput->initString, initialString);
            
            /*inputString,maxSize*/
            replaceHandle((OP_UINT32 *)&pInput->inputString, inputString);

            if( maxSize <= 0 )
            {
                maxSize = 100;      /*  default max number of input chars   */
            }
            
/*            if(ISVALIDHANDLE(pGadget->title))
            {
                iTabLineCnt = 1;
            }
            else
            {
                iTabLineCnt = 0;
            }*/

            replaceHandle((OP_UINT32 *)&pInput->inputString, inputString);          /* for widget copy*/
            
            /*type*/
            pInput->textType = type;
            
            
            /*singleLine*/
            pInput->singleLine = singleLine;
            
            /*size*/
            if( size )
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }                
            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_TEXTINPUT].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_TEXTINPUT].pos, sizeof(MsfPosition));
                
            /*propertyMask*/
            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_TEXTINPUT].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }           

            pInput->font = DEFAULT_TEXTINPUT_STYLE_FONT;
            pInput->color.r = 0;
            pInput->color.g = 0;
            pInput->color.b = 0;
            pInput->textProp = DEFAULT_STYLE_TEXTPROPERTY;

            /*defaultStyle**/
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);

            SP_list_insert_to_head(&widgetQueue[MSF_TEXTINPUT], pTextInput);
            iRet = TBM_CreateFromStrHandle((OP_INT16)maxSize,inputString,newGadgetHandle);
            if(iRet != TPI_WIDGET_OK)
            {
               return INVALID_HANDLE;
            }
            TBM_AdjustInitCursorPos(&(pInput->txtInputBufMng));            
#if 0
            if( pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYS_SHOW_SCROLL_BAR )
            {
                sizeScrollbar.height = pGadget->size.height;
                sizeScrollbar.width = SCROLLBAR_WIDTH;
                hBar = TPIa_widgetBarCreate(
                                        modId,
                                        MsfVerticalScrollBar,
                                        pInput->txtInputBufMng.TotalRows-1,
                                        pInput->txtInputBufMng.top_line,
                                        &sizeScrollbar,
                                        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_ALWAYSONTOP,
                                        0);
                
                if( ISVALIDHANDLE(hBar))
                {
                    pGadget->size.width -= SCROLLBAR_WIDTH;
                    pBar = (MsfBar*)seekWidget(hBar, OP_NULL );
                    if(pInput->txtInputBufMng.TotalRows <= pInput->txtInputBufMng.rows)
                    {
                        pBar->pageStep = 0;
                    }
                    else
                    {
                        pBar->pageStep = pInput->txtInputBufMng.rows;
                    }
                    
                    pBar->lineStep = 1;
                    pBar->hRelatedWidget = pGadget->gadgetHandle;

                    pGadget->hVScrollBar = hBar;  
                }
            }
#endif            
        }
    }

    return newGadgetHandle;
}


MsfGadgetHandle TPIa_widgetTextInputCreateCss (MSF_UINT8 modId, MsfStringHandle initialString, 
                                              MsfStringHandle inputString, MsfTextType type, 
                                              const char* formatString, int inputRequired, 
                                              int maxSize, int singleLine, 
                                              MsfSize* size, int propertyMask, MsfStyleHandle defaultStyle)
{
    MsfSize defaultSize ;
    
    if( size == MSF_NULL )
    {
        defaultSize.width = 100;
        defaultSize.height = 20;
        return TPIa_widgetTextInputCreate( modId, initialString, inputString, type, maxSize, singleLine, &defaultSize, propertyMask, defaultStyle);
    }
    else
    {
        return TPIa_widgetTextInputCreate( modId, initialString, inputString, type, maxSize, singleLine, size, propertyMask, defaultStyle);
    }
}


/****************************************************************
 SELECT GROUP GADGET 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetSelectgroupCreateEx

  DESCRIPTION:
      Creates an empty SelectGroup with the specified MsfSize and MsfChoiceType and its own state change callback function and
      returns the MsfGadgetHandle


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      type          Specifies the MsfChoiceType of the MsfSelectGroup
      size          Pointer to an MsfSize struct that specifies the size of the
                    MsfSelectGroup
      elementPos    Specifies the order of the element attributes. If this
                    parameter is 0, the integration will ignore it. The
                    integration may also ignore this parameter if it exceeds
                    the capacity of the device to alter the order in which the
                    element attributes that are shown.
      bitmask       Specifies which element attributes that are to be shown.
                    If this parameter is 0, the integration will ignore it.
      propertyMask  See TPIa_widgetGadgetSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfSelectGroup.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetSelectgroupCreateEx
(
    MSF_UINT8 modId, 
    MsfChoiceType type, 
    MsfSize* size, 
    MsfElementPosition* elementPos, 
    int bitmask, 
    int propertyMask, 
    MSF_UINT16 sgShowStyle,
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget *pGadget;
    MsfSelectgroup* pSelectgroup;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    
    pSelectgroup = WIDGET_NEW(MsfSelectgroup);
    if( pSelectgroup != OP_NULL)
    {
        op_memset(pSelectgroup, 0, sizeof(MsfSelectgroup));
        newGadgetHandle = getNewHandle(modId, MSF_SELECTGROUP);
        if (newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pSelectgroup );
        }
        else
        {
            pGadget = (MsfGadget *)pSelectgroup;
            pGadget->modId = modId;
            pGadget->gadgetHandle = newGadgetHandle;
            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_SELECTGROUP].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter =1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;
            
            if( size )
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }
            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_SELECTGROUP].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_SELECTGROUP].pos, sizeof(MsfPosition));
            
#ifdef  _KEYPAD
            {     /* Set Gadget SoftKey  */
                MsfWidgetType  wt;
                OP_INT16          ind;
                
                wt = MSF_SELECTGROUP - MSF_WINDOW_END - 1;

                for( ind = 0; ind < 3; ind++)
                {
                    if( gadgetSoftKeyIDs[ wt][ind] != 0 )
                    {
                        pGadget->softKeyList[ind] = widgetGetPredefinedString( gadgetSoftKeyIDs[wt][ind]);
                    }
                }
                if( propertyMask & MSF_GADGET_PROPERTY_SG_DO_NOT_HANDLE_OK )
                {
                    pGadget->softKeyList[1] = OP_NULL;
                }
            }
#endif                
            pSelectgroup->showStyle = sgShowStyle;
            pSelectgroup->choice.bitmask = bitmask;
            if (elementPos != OP_NULL)
            {
                op_memcpy(&pSelectgroup->choice.elementPos, elementPos, sizeof(MsfElementPosition));
            }
            pSelectgroup->choice.type = type;
            
            /* calc the elementsPerPage */
            pSelectgroup->choice.elementsPerPage = pGadget->size.height / CHOICE_LINE_HEIGHT;
            if( ISVALIDHANDLE(pSelectgroup->gadgetData.title) )
            {
                pSelectgroup->choice.elementsPerPage --;
            }
            
            pSelectgroup->choice.scrollCircularly = OP_FALSE;
            
            SP_list_insert_to_head(&widgetQueue[MSF_SELECTGROUP], pSelectgroup);
        }
    }

    return newGadgetHandle;
}


/****************************************************************
 IMAGE GADGET
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetImageGadgetCreateEx

  DESCRIPTION:
      Creates an MsfImageGadget with the specified MsfSize containing the specified
      MsfImage and its own state change callback function and returns the MsfGadgetHandle


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      image         The MsfImage identifier
      size          Specifies the MsfSize of the MsfImageGadget. 0 means
                    that the MsfImageGadget will be created with the
                    default MsfSize, i.e. with the MsfSize of the MsfImage.
      imageZoom     Specifies the MsfImageZoom the MsfImage should be
                    drawn with. The drawn MsfImage is still bounded by
                    the specified maxSize and/or the MsfSize of the
                    MsfImageGadget.
      propertyMask  See TPIa_widgetGadgetSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfImageGadget.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetImageGadgetCreateEx
(
    MSF_UINT8 modId, 
    MsfImageHandle image, 
    MsfSize* size, 
    MsfImageZoom imageZoom, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget *pGadget;
    MsfImageGadget* pImageGadget;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    
    pImageGadget = WIDGET_NEW(MsfImageGadget);
    if(pImageGadget != OP_NULL)
    {
        op_memset(pImageGadget, 0, sizeof(MsfImageGadget));
        newGadgetHandle = getNewHandle(modId, MSF_IMAGEGADGET);
        if (newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pImageGadget );
        }
        else
        {
            pGadget = (MsfGadget *)pImageGadget;
            pGadget->modId = modId;
            pGadget->gadgetHandle = newGadgetHandle;
            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_IMAGEGADGET].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter =1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;
            
            if( size )
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }
/*            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_IMAGEGADGET].size, sizeof(MsfSize));
            }
*/            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_IMAGEGADGET].pos, sizeof(MsfPosition));
            
#ifdef  _KEYPAD
            {     /* Set Gadget SoftKey  */
                MsfWidgetType  wt;
                OP_INT16          ind;
                
                wt = MSF_IMAGEGADGET - MSF_WINDOW_END - 1;

                for( ind = 0; ind < 3; ind++)
                {
                    if( gadgetSoftKeyIDs[ wt][ind] != 0 )
                    {
                        pGadget->softKeyList[ind] = widgetGetPredefinedString( gadgetSoftKeyIDs[wt][ind]);
                    }
                }
            }
#endif                
            
            replaceHandle((OP_UINT32 *)&pImageGadget->image, image);
            pImageGadget->imageZoom = imageZoom;
/*            pImageGadget->isHoldDraw = OP_FALSE;       */
            
            SP_list_insert_to_head(&widgetQueue[MSF_IMAGEGADGET], pImageGadget);
        }
        
    }

    return newGadgetHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetImageGadgetSet

  DESCRIPTION:
      Stores the specified MsfImage in the specified MsfImageGadget.


  ARGUMENTS PASSED:
      imageGadget   The MsfImageGadget identifier.
      image         The MsfImage identifier.
      imageZoom     Specifies the MsfImageZoom the MsfImage should be
                    drawn with. The drawn MsfImage is still bounded by the
                    specified maxSize and/or the MsfSize of the
                    MsfImageGadget.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetImageGadgetSet
(
    MsfGadgetHandle imageGadget, 
    MsfImageHandle image, 
    MsfImageZoom imageZoom
)
{
    MsfImageGadget* pImageGadget;
    MsfWidgetType widgetType;
    int ret = TPI_WIDGET_OK;
    
    pImageGadget = (MsfImageGadget*)seekWidget(imageGadget, &widgetType);
    if( !pImageGadget || widgetType != MSF_IMAGEGADGET)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else if(pImageGadget->image == image )
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        replaceHandle((OP_UINT32 *)&pImageGadget->image, image);
        pImageGadget->imageZoom = imageZoom;
        
    }
    
    return ret;
}
  

/****************************************************************
 DATE TIME GADGET
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetDateTimeCreateEx

  DESCRIPTION:
      Creates an MsfDateTime with the specified DateTimeType and MsfSize and its own state change callback function and returns
      the MsfGadgetHandle


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      mode          Specifies the MsfDateTimeType of the MsfDateTime
      time          Specifies the MsfTime of the MsfDateTime. If this
                    parameter is set to 0, the integration will use the default
                    MsfTime of the device.
      date          Specifies the MsfDate of the MsfDateTime. If this
                    parameter is set to 0, the integration will use the default
                    MsfDate of the device.
      size          Specifies the MsfSize of the MsfDateTime.
      propertyMask  See TPIa_widgetGadgetSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfDateTime.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange    The state change callback function
      
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetDateTimeCreateEx 
(
    MSF_UINT8 modId, 
    MsfDateTimeType mode, 
    MsfTime* time, 
    MsfDate* date, 
    MsfSize* size, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget *pGadget;
    MsfDateTimeStruct* pDateTime;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    TIME_DATE  td;
    
    pDateTime = WIDGET_NEW(MsfDateTimeStruct);
    if(pDateTime != OP_NULL)
    {
        op_memset(pDateTime, 0, sizeof(MsfDateTimeStruct));
        newGadgetHandle = getNewHandle(modId, MSF_DATETIME);
        if (newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pDateTime );
        }
        else
        {
            pGadget = (MsfGadget *)pDateTime;
            pGadget->modId = modId;
            pGadget->gadgetHandle = newGadgetHandle;
            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_DATETIME].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            if (size != OP_NULL)
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }
            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_DATETIME].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_DATETIME].pos, sizeof(MsfPosition));

            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter =1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;
            
#ifdef  _KEYPAD
            {     /* Set Gadget SoftKey  */
                MsfWidgetType  wt;
                OP_INT16          ind;
                
                wt = MSF_DATETIME - MSF_WINDOW_END - 1;

                for( ind = 0; ind < 3; ind++)
                {
                    if( gadgetSoftKeyIDs[ wt][ind] != 0 )
                    {
                        pGadget->softKeyList[ind] = widgetGetPredefinedString( gadgetSoftKeyIDs[wt][ind]);
                    }
                }
            }
#endif                
            
            pDateTime->mode = mode;
            switch( mode )
            {
            case MsfTimeType:
                {
                    if( time )
                    {
                        op_memcpy(&pDateTime->time, time, sizeof(MsfTime));
                    }
                    else
                    {
                        op_get_cur_timedate( &td );
                        pDateTime->time.hours = td.Time.Hours;
                        pDateTime->time.minutes = td.Time.Minutes;
                        pDateTime->time.seconds = td.Time.Seconds;
                    }
                    
                    break;
                }
            case MsfDateType:
                {
                    if( date )
                    {
                        op_memcpy(&pDateTime->date, date, sizeof(MsfDate));
                    }
                    else
                    {
                        op_get_cur_timedate( &td );
                        pDateTime->date.day = td.Date.Day;
                        pDateTime->date.month = td.Date.Month;
                        pDateTime->date.year = td.Date.Year;
                    }
                    break;
                }
            case MsfDateTime:
                {
                    op_get_cur_timedate( &td );
                    
                    if( date )
                    {
                        op_memcpy(&pDateTime->date, date, sizeof(MsfDate));
                    }
                    else
                    {
                        pDateTime->date.day = td.Date.Day;
                        pDateTime->date.month = td.Date.Month;
                        pDateTime->date.year = td.Date.Year;
                    }
                    
                    if( time )
                    {
                        op_memcpy(&pDateTime->time, time, sizeof(MsfTime));
                    }
                    else
                    {
                        pDateTime->time.hours = td.Time.Hours;
                        pDateTime->time.minutes = td.Time.Minutes;
                        pDateTime->time.seconds = td.Time.Seconds;
                    }
                    
                    break;
                }
            default:
                {
                    break;
                }
            }
            
            SP_list_insert_to_head(&widgetQueue[MSF_DATETIME], pDateTime);
        }
        
    }

    return newGadgetHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetDateTimeSetValues

  DESCRIPTION:
      Sets the specified values in the specified MsfDateTime.

  ARGUMENTS PASSED:
      datetimeGadget    The MsfDateTime identifier.
      mode              Specifies the MsfDateTimeType of the
                        MsfDateTime.
      time              Specifies the MsfTime of the MsfDateTime. If this
                        parameter is set to 0, the integration will use the
                        default MsfTime of the device.
      date              Specifies the MsfDate of the MsfDateTime. If this
                        parameter is set to 0, the integration will use the
                        default MsfDate of the device.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetDateTimeSetValues 
(
    MsfGadgetHandle datetimeGadget, 
    MsfDateTimeType mode, 
    MsfTime* time, 
    MsfDate* date
)
{
    MsfDateTimeStruct* pDateTime;
    TIME_DATE  td;
    MsfWidgetType  widgetType;

    pDateTime = (MsfDateTimeStruct*)seekWidget(datetimeGadget, &widgetType);
    if( pDateTime && widgetType == MSF_DATETIME)
    {
        pDateTime->mode = mode;
        switch( mode )
        {
        case MsfTimeType:
            {
                if( time )
                {
                    op_memcpy(&pDateTime->time, time, sizeof(MsfTime));
                }
                else
                {
                    op_get_cur_timedate( &td );
                    pDateTime->time.hours = td.Time.Hours;
                    pDateTime->time.minutes = td.Time.Minutes;
                    pDateTime->time.seconds = td.Time.Seconds;
                }
                
                break;
            }
        case MsfDateType:
            {
                if( date )
                {
                    op_memcpy(&pDateTime->date, date, sizeof(MsfDate));
                }
                else
                {
                    op_get_cur_timedate( &td );
                    pDateTime->date.day = td.Date.Day;
                    pDateTime->date.month = td.Date.Month;
                    pDateTime->date.year = td.Date.Year;
                }
                break;
            }
        case MsfDateTime:
            {
                op_get_cur_timedate( &td );
                
                if( date )
                {
                    op_memcpy(&pDateTime->date, date, sizeof(MsfDate));
                }
                else
                {
                    pDateTime->date.day = td.Date.Day;
                    pDateTime->date.month = td.Date.Month;
                    pDateTime->date.year = td.Date.Year;
                }
                
                if( time )
                {
                    op_memcpy(&pDateTime->time, time, sizeof(MsfTime));
                }
                else
                {
                    pDateTime->time.hours = td.Time.Hours;
                    pDateTime->time.minutes = td.Time.Minutes;
                    pDateTime->time.seconds = td.Time.Seconds;
                }
                
                break;
            }
        default:
            {
                break;
            }
        }
        
        return TPI_WIDGET_OK;
    }
    
    return TPI_WIDGET_ERROR_BAD_HANDLE;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetDateTimeGetValues

  DESCRIPTION:
      Gets the specified values in the specified MsfDateTime.

  ARGUMENTS PASSED:
      datetimeGadget    The MsfDateTime identifier.
      time              Specifies the MsfTime of the MsfDateTime.
      date              Specifies the MsfDate of the MsfDateTime.
      
  RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/

int TPIa_widgetDateTimeGetValues 
(
    MsfGadgetHandle datetimeGadget, 
    MsfTime* time, 
    MsfDate* date
)
{
    MsfDateTimeStruct* pDateTime;
    MsfWidgetType  widgetType;

    pDateTime = (MsfDateTimeStruct*)seekWidget(datetimeGadget, &widgetType);
    if( pDateTime && widgetType == MSF_DATETIME)
    {
        switch( pDateTime->mode )
        {
        case MsfTimeType:
            {
                if( time )
                {
                    op_memcpy(time, &pDateTime->time, sizeof( MsfTime));
                }

                if( date )
                {
                    op_memset( date, 0, sizeof( MsfDate ));
                }
                break;
            }
        case MsfDateType:
            {
                if( date )
                {
                    op_memcpy(date, &pDateTime->date, sizeof(MsfDate));
                    
                }

                if( time )
                {
                    op_memset( time, 0, sizeof( MsfTime ));
                }
                break;
            }
        case MsfDateTime:
            {
                if( date )
                {
                    op_memcpy(date, &pDateTime->date, sizeof(MsfDate));
                }
                
                if( time )
                {
                    op_memcpy(time, &pDateTime->time, sizeof( MsfTime));
                }
                
                break;
            }
        default:
            {
                break;
            }
        }

        return TPI_WIDGET_OK;
    }
  
    return TPI_WIDGET_ERROR_BAD_HANDLE;
}

/****************************************************************
 BUTTON GADGET 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetButtonCreateEx

  DESCRIPTION:
      Creates a MsfButton with the specified MsfSize and its own state change callback funcion and returns the
      MsfGadgetHandle.

  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      label          Specifies the label of the MsfButton
      size          Specifies the MsfSize of the MsfButton. 
      labelPos     Specifies the alignment of the label of MsfButton
      pNormalRes  Specifies the resource IDs of normal background of MsfButton.
                           And the number of resource IDs MUST be COLOR_THEME_COUNT.
                           If is NULL, use the default values definied in Int_widget_custom.c
      pNormalRes  Specifies the resource IDs of focused background of MsfButton
                           And the number of resource IDs MUST be COLOR_THEME_COUNT.
                           If is NULL, use the default values definied in Int_widget_custom.c
      propertyMask  See TPIa_widgetGadgetSetProperties().If this parameter
                    is set to 0x8000, the integration will use the device
                    default properties for the MsfButton.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetButtonCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle label, 
    MsfSize* size, 
    MsfHorizontalPos        labelAlignment,
    MSF_UINT32      iNormalRes,
    MSF_UINT32      iFocusRes,
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget *pGadget;
    MsfButtonGadget* pButton;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    
    if (!MSF_IS_MODULE_ID_VALID(modId) ||
        (defaultStyle != 0 && isValidHandleOfType(defaultStyle, MSF_STYLE)== OP_FALSE))
    {
        return INVALID_HANDLE;
    }

    pButton = WIDGET_NEW(MsfButtonGadget);
    if( pButton != OP_NULL)
    {
        op_memset(pButton, 0, sizeof(MsfButtonGadget));
        newGadgetHandle = getNewHandle(modId, MSF_BUTTON);
        if(newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pButton);
        }
        else
        {
            pGadget = (MsfGadget *)pButton;
            pGadget->gadgetHandle = newGadgetHandle;
            pGadget->modId = modId;

            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_BUTTON].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            
            replaceHandle((OP_UINT32 *)&pGadget->title, label);
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter = 1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;
            
            if( size )
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }
            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_BUTTON].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_BUTTON].pos, sizeof(MsfPosition));

            pButton->labelAlignment = labelAlignment;
            pButton->iFocusRes = iFocusRes;
            pButton->iNormalRes = iNormalRes;
            
            SP_list_insert_to_head(&widgetQueue[MSF_BUTTON], pButton);
        }
    }

    return newGadgetHandle;
}

/****************************************************************
 BOX GADGET 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetBoxCreateEx

  DESCRIPTION:
      Creates an empty box with the specified MsfSize and MsfChoiceType and its own state change callback function and
      returns the MsfGadgetHandle


  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      type          Specifies the MsfChoiceType of the MsfBox
      size          Pointer to an MsfSize struct that specifies the size of the
                    MsfBox
      elementPos    Specifies the order of the element attributes. If this
                    parameter is 0, the integration will ignore it. The
                    integration may also ignore this parameter if it exceeds
                    the capacity of the device to alter the order in which the
                    element attributes that are shown.
      bitmask       Specifies which element attributes that are to be shown.
                    If this parameter is 0, the integration will ignore it.
      propertyMask  See TPIa_widgetGadgetSetProperties().If this
                    parameter is set to 0x8000, the integration will use the
                    device default properties for the MsfBox.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetBoxCreateEx
(
    MSF_UINT8 modId, 
    MsfChoiceType type, 
    MsfSize* size, 
    int         nItemCount,
    int         curPos,
    MsfElementPosition* elementPos, 
    int         bitmask, 
    int         propertyMask, 
    MSF_UINT16 sgShowStyle,
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget      *pGadget;
    MsfSelectgroup  *pSelectgroup;
    MsfBox           *pBox;
    
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    
    pBox = WIDGET_NEW(MsfBox);
    if( pBox != OP_NULL)
    {
        op_memset(pBox, 0, sizeof(MsfBox));
        newGadgetHandle = getNewHandle(modId, MSF_BOX);
        if (newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pBox );
        }
        else
        {
            pGadget = (MsfGadget *)pBox;
            pGadget->modId = modId;
            pGadget->gadgetHandle = newGadgetHandle;
            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_BOX].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter =1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;
            
            if( size )
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }
            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_BOX].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_BOX].pos, sizeof(MsfPosition));
            
#ifdef  _KEYPAD
            {     /* Set Gadget SoftKey  */
                MsfWidgetType  wt;
                OP_INT16          ind;
                
                wt = MSF_BOX - MSF_WINDOW_END - 1;

                for( ind = 0; ind < 3; ind++)
                {
                    if( gadgetSoftKeyIDs[ wt][ind] != 0 )
                    {
                        pGadget->softKeyList[ind] = widgetGetPredefinedString( gadgetSoftKeyIDs[wt][ind]);
                    }
                }
            }
#endif                

            pSelectgroup = (MsfSelectgroup*)pBox;
            pSelectgroup->showStyle = sgShowStyle &(~SG_SHOW_STYLE_NO_SCROLLBAR);
            pSelectgroup->choice.bitmask = bitmask;
            if (elementPos != OP_NULL)
            {
                op_memcpy(&pSelectgroup->choice.elementPos, elementPos, sizeof(MsfElementPosition));
            }
            pSelectgroup->choice.type = type;
            
            /* calc the elementsPerPage */
            pSelectgroup->choice.elementsPerPage = pGadget->size.height / CHOICE_LINE_HEIGHT;
            if( ISVALIDHANDLE(pSelectgroup->gadgetData.title) )
            {
                pSelectgroup->choice.elementsPerPage --;
            }
            
            pSelectgroup->choice.scrollCircularly = OP_TRUE;

            pBox->nItemCount = nItemCount;
            pBox->iCurPos = curPos;
            
            SP_list_insert_to_head(&widgetQueue[MSF_BOX], pBox);

            widgetBoxAdjustScrollbar(pBox);
        }
    }

    return newGadgetHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetBoxSetValues

  DESCRIPTION:
      Sets the specified Box values .


  ARGUMENTS PASSED:
      hBox               The MsfBox identifier.
      iCurPos            Specifies the  position of current item in the MsfBox.
      nItemCount      Specifies the count of items in the MsfBox.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetBoxSetValues 
(
    MsfGadgetHandle hBox,
    int iCurPos, 
    int nItemCount
)
{
    MsfBox             *pBox;
    MsfWidgetType  widgetType;

    pBox = (MsfBox*)seekWidget(hBox, &widgetType);
    
    if( pBox && widgetType == MSF_BOX)
    {
        pBox->iCurPos = iCurPos;
        pBox->nItemCount = nItemCount ;
        
        widgetBoxAdjustScrollbar(pBox);
        
        return TPI_WIDGET_OK;
    }

    return TPI_WIDGET_ERROR_BAD_HANDLE;
}


/*==================================================================================================
  FUNCTION:  TPIa_widgetBoxGetValues

  DESCRIPTION:
      Sets the specified Box values .


  ARGUMENTS PASSED:
      hBox               The MsfBox identifier.
      pCurPos           Returns the  position of current item in the MsfBox.
      pFocusItemRelativeIndex   The focus intem's index relative to the top item
      pItemsPerPage  Returns the count of items in a page 
      pItemCount      Returns the count of items in the MsfBox.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetBoxGetValues 
(
    MsfGadgetHandle hBox,
    int    *pCurPos, 
    int    *pFocusItemRelativeIndex,
    int    *pItemsPerPage,
    int    *pItemCount
)
{
    MsfBox             *pBox;
    MsfWidgetType  widgetType;

    pBox = (MsfBox*)seekWidget(hBox, &widgetType);
    
    if( pBox && widgetType == MSF_BOX)
    {
        if(pCurPos )
        {
            *pCurPos = pBox->iCurPos;
        }
        if( pItemsPerPage )
        {
            *pItemsPerPage = pBox->selectgroup.choice.elementsPerPage;
        }

        if( pItemCount )
        {
            *pItemCount = pBox->nItemCount;
        }

        if( pFocusItemRelativeIndex )
        {
            *pFocusItemRelativeIndex = pBox->selectgroup.choice.cur_item;
        }
        return TPI_WIDGET_OK;
    }

    return TPI_WIDGET_ERROR_BAD_HANDLE;
}

/****************************************************************
 BAR GADGET
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetBarCreate

  DESCRIPTION:
      Creates an MsfBar with the specified maxValue, initialValue, interactivity,
      MsfSize and its own state change callback function, and returns the MsfGadgetHandle


  ARGUMENTS PASSED:
  modId         The MSF Module identifier
  barType       Specifies the MsfBarType of the MsfBar.
  maxValue      Specifies the maximum value of the MsfBar
  initialValue  Specifies the initial value of the MsfBar
  size          Specifies the MsfSize of the MsfBar
  propertyMask  See TPIa_widgetGadgetSetProperties().If this
                parameter is set to 0x8000, the integration will use the
                device default properties for the MsfBar.
  defaultStyle  The MsfStyle identifier. If this parameter is 0 the
                default style of the device will be used.
  cbStateChange   The state change callback function
  
  RETURN VALUE:
      The new MsfGadgetHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetBarCreateEx 
(
    MSF_UINT8 modId, 
    MsfBarType barType,
    int maxValue, 
    int initialValue, 
    MsfSize* size,
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfGadget *pGadget;
    MsfBar* pBar;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    
    pBar = WIDGET_NEW(MsfBar);
    if(pBar != OP_NULL)
    {
        op_memset(pBar, 0, sizeof(MsfBar));
        newGadgetHandle = getNewHandle(modId, MSF_BAR);
        if (newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pBar );
        }
        else
        {
            pGadget = (MsfGadget *)pBar;
            pGadget->modId = modId;
            pGadget->gadgetHandle = newGadgetHandle;
            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_BAR].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter =1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;
            
            if( size )
            {
                pGadget->size = *size;
            }
            else
            {
                op_memcpy(&pGadget->size, &DefaultPropTab[MSF_BAR].size, sizeof(MsfSize));
            }
            
            op_memcpy(&pGadget->position, &DefaultPropTab[MSF_BAR].pos, sizeof(MsfPosition));
            
#ifdef  _KEYPAD
            {     /* Set Gadget SoftKey  */
                MsfWidgetType  wt;
                OP_INT16          ind;
                
                wt = MSF_BAR - MSF_WINDOW_END - 1;

                for( ind = 0; ind < 3; ind++)
                {
                    if( gadgetSoftKeyIDs[ wt][ind] != 0 )
                    {
                        pGadget->softKeyList[ind] = widgetGetPredefinedString( gadgetSoftKeyIDs[wt][ind]);
                    }
                }
            }
#endif                
            
            pBar->barType = barType;
            pBar->value = initialValue;
            pBar->maxValue = maxValue;

            /* If the bar is scroll bar, the lineStep is set to line height(18) of paintbox temporarily.
              * if the bar is add to menu, or form, the menu or form will adjust the lineStep and pageStep values
              */
            if( barType == MsfVerticalScrollBar || barType == MsfHorizontalScrollBar )
            {
                pBar->lineStep = 18;
            }
            else
            {
                pBar->lineStep = 1;
            }
            
            widgetBarAdjustValue(pBar);
            
            if( barType == MsfVerticalScrollBar )
            {
                pBar->pageStep = pGadget->size.height - pBar->lineStep;
            }
            else if( barType == MsfHorizontalScrollBar )
            {
                pBar->pageStep = pGadget->size.width- pBar->lineStep;
            }
            
            SP_list_insert_to_head(&widgetQueue[MSF_BAR], pBar);
        }
        
    }

    return newGadgetHandle;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetBarSetValues

  DESCRIPTION:
      Sets the specified Bar values of the specified MsfBar.


  ARGUMENTS PASSED:
      bar           The MsfBar identifier.
      value         Specifies the value to be set in the MsfBar.
      maxValue      Specifies the maximum value of the MsfBar.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetBarSetValues 
(
    MsfGadgetHandle bar,
    int value, 
    int maxValue
)
{
    MsfBar* pBar;
    MsfWidgetType  widgetType;

    pBar = (MsfBar*)seekWidget(bar, &widgetType);
    
    if( pBar && widgetType == MSF_BAR)
    {
        pBar->value = value;
        pBar->maxValue = maxValue;
        
        widgetBarAdjustValue(pBar);  
        
        if( pBar->gadgetData.parent  )
        {
            return widgetRedraw((void *) pBar, widgetType,  bar, OP_TRUE );
        }

        return TPI_WIDGET_OK;
    }
    
    return TPI_WIDGET_ERROR_BAD_HANDLE;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetBarGetValues

  DESCRIPTION:
      Gets the specified bar values of the specified MsfBar.


  ARGUMENTS PASSED:
      bar       The MsfBar identifier.
      value     Specifies the value to be set in the MsfBar.
      maxValue  Specifies the maximum value of the MsfBar.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetBarGetValues 
(
    MsfGadgetHandle bar, 
    int* value, 
    int* maxValue
)
{
    MsfBar* pBar;
    MsfWidgetType  widgetType;

    pBar = (MsfBar*)seekWidget(bar, &widgetType);
    
    if( pBar && widgetType == MSF_BAR)
    {
        if( value )
        {
            *value = pBar->value;
        }

        if( maxValue )
        {
            *maxValue = pBar->maxValue;
        }

        return TPI_WIDGET_OK;
    }
    
    return TPI_WIDGET_ERROR_BAD_HANDLE;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetUpdateBegin

  DESCRIPTION:
      The TPIa_widgetUpdateBegin and TPIa_widgetUpdateEnd functions provide the feature 
      of  updating a widget's contents in batches. i.e. when the widget is in batches update 
      mode ( the state after call TPIa_widgetUpdateBegin ), the widget's contents change 
      will not be seen until the TPIa_widgetUpdateBegin is called.
      TPIa_widgetUpdateBegin will make the widget to enter the mode of updating in batches.

  ARGUMENTS PASSED:
      handle       The MsfWidget identifier.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      The TPIa_widgetUpdateBegin and TPIa_widgetUpdateEnd must be pair-called.
==================================================================================================*/
int TPIa_widgetUpdateBegin( MSF_UINT32  handle )
{
    void                 *pWidget;
    MsfWidgetType   wt;
    MsfScreen         *pScreen;
    MsfWindow        *pWin;
    MsfGadget         *pGadget;

    pWidget = seekWidget( handle, &wt);
    if( !pWidget || wt > MSF_GADGET_END )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( IS_SCREEN(wt))
    {
        /*  set bInBatches flag of the screen  */
        pScreen = (MsfScreen*)pWidget;
        pScreen->bInBatches = OP_TRUE;
    }
    else if(IS_WINDOW(wt))
    {
        if( wt != MSF_PAINTBOX )
        {
            /*  set bInBatches flag of the window  */
            pWin = (MsfWindow*)pWidget;
            pWin->bInBatches = OP_TRUE;
        }
    }
    else if( IS_GADGET(wt))
    {
        /*  set bInBatches flag of the gadget  */
        pGadget = (MsfGadget*)pWidget;
        pGadget->bInBatches = OP_TRUE;
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetUpdateEnd

  DESCRIPTION:
      The TPIa_widgetUpdateBegin and TPIa_widgetUpdateEnd functions provide the feature 
      of  updating a widget's contents in batches. i.e. when the widget is in batches update 
      mode ( the state after call TPIa_widgetUpdateBegin ), the widget's contents change 
      will not be seen until the TPIa_widgetUpdateBegin is called.
      TPIa_widgetUpdateEnd will end the widget mode of updating in batches, and its contents
      will show in screen.

  ARGUMENTS PASSED:
      handle       The MsfWidget identifier.
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      The TPIa_widgetUpdateBegin and TPIa_widgetUpdateEnd must be pair-called.
==================================================================================================*/
int TPIa_widgetUpdateEnd( MSF_UINT32  handle )
{
    void                 *pWidget;
    MsfWidgetType   wt;
    MsfScreen         *pScreen;
    MsfWindow        *pWin;
    MsfGadget         *pGadget;
    OP_BOOLEAN     bRedraw = OP_FALSE;

    pWidget = seekWidget( handle, &wt);
    if( !pWidget || wt > MSF_GADGET_END )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( IS_SCREEN(wt))
    {
        /*  clear bInBatches flag of the screen  */
        pScreen = (MsfScreen*)pWidget;
        bRedraw = pScreen->bInBatches;
        pScreen->bInBatches = OP_FALSE;
    }
    else if(IS_WINDOW(wt))
    {
        /*  clear bInBatches flag of the window  */
        pWin = (MsfWindow*)pWidget;
        bRedraw = pWin->bInBatches;
        pWin->bInBatches = OP_FALSE;
    }
    else if( IS_GADGET(wt))
    {
        /*  clear bInBatches flag of the gadget  */
        pGadget = (MsfGadget*)pWidget;
        bRedraw = pGadget->bInBatches;
        pGadget->bInBatches = OP_FALSE;
    }

    if( bRedraw && wt != MSF_PAINTBOX )
    {
        widgetRedraw( pWidget, wt, handle, OP_TRUE );
    }
    
    return TPI_WIDGET_OK;
}



MsfGadgetHandle TPIa_widgetBmpAniGadgetCreatEx
(
 MSF_UINT8 modId, 
 MsfSize* size, 
 MSF_UINT32 resID,
 MSF_UINT32   lDelay,
 MSF_BOOL bPeriod,
 int propertyMask, 
 MsfStyleHandle defaultStyle,
 widget_state_change_callback_t   cbStateChange
 )
{
    MsfGadget *pGadget;
    MsfBmpAnimation *pBmpAniGadget;
    MsfGadgetHandle newGadgetHandle = INVALID_HANDLE;
    RM_INT_ANIDATA_T    *pAniData = OP_NULL;

    pAniData = util_get_animation_from_res(resID);
    if( !pAniData )
    {
        return INVALID_HANDLE;
    }
    
    pBmpAniGadget = WIDGET_NEW(MsfBmpAnimation);
    if(pBmpAniGadget != OP_NULL)
    {
        op_memset(pBmpAniGadget, 0, sizeof(MsfBmpAnimation));
        newGadgetHandle = getNewHandle(modId, MSF_BMPANIMATION);
        if (newGadgetHandle == INVALID_HANDLE)
        {
            WIDGET_FREE(pBmpAniGadget );
        }
        else
        {
            pGadget = (MsfGadget *)pBmpAniGadget;
            pGadget->modId = modId;
            pGadget->gadgetHandle = newGadgetHandle;
            if( propertyMask == 0x8000 )
            {
                pGadget->propertyMask = DefaultPropTab[MSF_BMPANIMATION].propertyMask;
            }
            else
            {
                pGadget->propertyMask = propertyMask;
            }
            replaceHandle((OP_UINT32 *)&pGadget->style, defaultStyle);
            pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
            pGadget->alignment.verticalPos = MsfVerticalPosDefault;
            pGadget->referenceCounter =1;
            pGadget->isReleased = OP_FALSE;
            pGadget->cbStateChange = cbStateChange;
            
            if( OP_NULL != size )
            {
                op_memcpy(&pGadget->size, size, sizeof(MsfSize));
            }
                          
            pBmpAniGadget->pAniData = pAniData;
            pBmpAniGadget->interval = lDelay;
            pBmpAniGadget->bLoop = bPeriod;
            SP_list_insert_to_head(&widgetQueue[MSF_BMPANIMATION], pBmpAniGadget);
        }
        
    }

    return newGadgetHandle;
}



/*==================================================================================================
    FUNCTION:  TPIa_widgetBmpAnimationPlay

    DESCRIPTION:
        play bitmap or icon animation
        
    ARGUMENTS PASSED:
    
        gadgetHandle: Bitmap gadget handle 
        bUsePeriodicTimer : specify if starting timer by periodic mode
        
    RETURN VALUE:
         OP_TRUE: successfully play animation
         OP_FALSE: exist invalid variable 

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetBmpAnimationPlay(MsfGadgetHandle gadgetHandle, int bUsePeriodicTimer)
{
     MsfBmpAnimation  *pBmpAni = OP_NULL;
     MsfWidgetType   gadgetType;
     MsfGadget       *pGadget;
     if(!(ISVALIDHANDLE(gadgetHandle)))
     {
         return TPI_WIDGET_ERROR_INVALID_PARAM ;
     }
     pBmpAni = (MsfBmpAnimation *)seekWidget(gadgetHandle,&gadgetType);
     if( !pBmpAni || gadgetType != MSF_BMPANIMATION || pBmpAni->timer_id != WIDGET_TIMER_INVALID_ID )
     {
        return TPI_WIDGET_ERROR_UNEXPECTED;
     }
     
     pGadget = (MsfGadget *)pBmpAni;
     if( bUsePeriodicTimer )
     {
        pBmpAni->bPeriodicTimer = OP_TRUE;
     }
     else
     {
        pBmpAni->bPeriodicTimer = OP_FALSE;
     }
     
     pBmpAni->timer_id = TPIa_widgetStartTimer(pBmpAni->interval, 
                                              bUsePeriodicTimer, /* period timer */
                                              pGadget->gadgetHandle,
                                              (widget_timer_callback_t)fnBmpAniTimerCallBack);
    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  TPIa_widgetBmpAnimationStop

    DESCRIPTION:
        stop bitmap or icon animation
        
    ARGUMENTS PASSED:
    
        gadgetHandle: Bitmap gadget handle 
        
    RETURN VALUE:
         OP_TRUE: successfully stop animation
         OP_FALSE: exist invalid variable 

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetBmpAnimationStop(MsfGadgetHandle gadgetHandle)
{
     MsfBmpAnimation  *pBmpAni = OP_NULL;
     MsfWidgetType   gadgetType;
     if(!(ISVALIDHANDLE(gadgetHandle)))
     {
         return TPI_WIDGET_ERROR_INVALID_PARAM;
     }
     pBmpAni = (MsfBmpAnimation *)seekWidget(gadgetHandle,&gadgetType);
     if( !pBmpAni || gadgetType != MSF_BMPANIMATION )
     {
        return TPI_WIDGET_ERROR_UNEXPECTED;
     }
     
     if(WIDGET_TIMER_INVALID_ID != pBmpAni->timer_id)
     {
         TPIa_widgetStopTimer( gadgetHandle, pBmpAni->timer_id);
         pBmpAni->timer_id = WIDGET_TIMER_INVALID_ID;
     }
     return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  TPIa_widgetImagePlay

    DESCRIPTION:
        play gif or jpg image.
        
    ARGUMENTS PASSED:
    
        gadgetHandle: gif or jpg image gadget handle 
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetImagePlay(MsfGadgetHandle gadgetHandle)
{
    MsfImageGadget   *pImageGadget;
    MsfWidgetType  wt;
    MsfImage           *pImage;
    
    WIDGET_TRACE(("widgetPaintBoxEndDraw\n"));

    if(!(ISVALIDHANDLE(gadgetHandle)))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM ;
    }

    // TODO: if GIF, start the timer, or not do
    pImageGadget= (MsfImageGadget *)seekWidget(gadgetHandle,&wt);
    if( !pImageGadget || wt != MSF_IMAGEGADGET || pImageGadget->iTimerId!= WIDGET_TIMER_INVALID_ID )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pImage = (MsfImage*)seekWidget(pImageGadget->image, &wt);
    if( !pImage || wt != MSF_IMAGE )
    {   
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    if( (pImage->format == MsfGif) && (pImage->flag & GIF_TIMER_REQUEST ))
    {
        WIDGET_TRACE(("widgetPaintBoxEndDraw:    play animation\n"));
        pImageGadget->iTimerId = TPIa_widgetStartTimer(500, 
                                                              MSF_FALSE,  /* one shot timer */
                                                              gadgetHandle,
                                                              (widget_timer_callback_t)ImageGadgetOnTimer);
    }
    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  TPIa_widgetImageStop

    DESCRIPTION:
        stop bitmap or icon animation
        
    ARGUMENTS PASSED:
    
        gadgetHandle: Bitmap gadget handle 
        
    RETURN VALUE:
         OP_TRUE: successfully stop animation
         OP_FALSE: exist invalid variable 

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetImageStop(MsfGadgetHandle gadgetHandle)
{
     MsfImageGadget   *pImageGadget;
     MsfWidgetType  wt;
     
     if(!(ISVALIDHANDLE(gadgetHandle)))
     {
         return TPI_WIDGET_ERROR_INVALID_PARAM ;
     }
     pImageGadget= (MsfImageGadget *)seekWidget(gadgetHandle,&wt);
     if( !pImageGadget || wt != MSF_IMAGEGADGET )
     {
        return TPI_WIDGET_ERROR_UNEXPECTED;
     }

     if( pImageGadget->iTimerId != WIDGET_TIMER_INVALID_ID )
     {
         TPIa_widgetStopTimer( gadgetHandle, pImageGadget->iTimerId );
         pImageGadget->iTimerId = WIDGET_TIMER_INVALID_ID;
     }


     // TODO:/*free gif unused buffer */

     
     return TPI_WIDGET_OK;
}

static int ImageGadgetOnTimer(MsfGadgetHandle gadgetHandle, OP_UINT32 iTimerId )
{
    MsfImageGadget   *pImageGadget;
    MsfGadget        *pGadget;
    MsfWidgetType  wt;
    MsfImage           *pImage;
    int                   iRet;

    WIDGET_TRACE(("ImageGadgetOnTimer\n"));
    if(!(ISVALIDHANDLE(gadgetHandle)))
    {
         return TPI_WIDGET_ERROR_INVALID_PARAM ;
    }

    pGadget = (MsfGadget*)seekWidget(gadgetHandle,&wt);
    if( !pGadget || wt != MSF_IMAGEGADGET )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pImageGadget= (MsfImageGadget *)pGadget;
    pImageGadget->iTimerId = WIDGET_TIMER_INVALID_ID;

    if( pGadget->parent == OP_NULL || pGadget->parent->isFocused == OP_FALSE)
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    pImage = (MsfImage *)seekWidget(pImageGadget->image, &wt);
    if( !pImage || wt != MSF_IMAGE )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    iRet = widgetImageGadgetPlay( pImageGadget, OP_TRUE );
    if(iRet >= 0)
    {
        pImageGadget->iTimerId = TPIa_widgetStartTimer(500, 
                                                                  MSF_FALSE,/* period timer */
                                                                  gadgetHandle,
                                                                  (widget_timer_callback_t)ImageGadgetOnTimer);
    }

    return TPI_WIDGET_OK;
}

/****************************************************************
 Main menu 
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetMainMenuCreateEx

  DESCRIPTION:
      Creates an empty MsfMenu with the specified MsfSize and MsfChoiceType and its own state change callback function
      returns the MsfWindowHandle. If the MsfMenu is of type MsfImplicit an
      MsfActionHandle to a MsfSelect MsfAction must be passed as input to the function in
      the parameter implicitSelectAction. When an element in the MsfMenu is selected, the
      integration will set the label in this MsfAction to String1 of the selected element and
      send the MsfAction to the MsfAction handling functionality, which will result in a call
      to the TPIc_widgetAction() function. If the MsfMenu is of another type than
      MsfImplicit the implicitSelectAction parameter should be 0.

  ARGUMENTS PASSED:
      modId             The MSF Module identifier
      type              Specifies the MsfChoiceType of the MsfMenu
      size              Pointer to an MsfSize struct that specifies the size of the
                        MsfMenu
      implicitSelect    Specifies the MsfAction that is to be used in an
      Action            MsfImplicit MsfMenu
      elementPos        Specifies the order of the element attributes. If this
                        parameter is 0, the integration will ignore it. The
                        integration may also ignore this parameter if it exceeds
                        the capacity of the device to alter the order in which the
                        element attributes that are shown.
      bitmask           Specifies which element attributes that are to be shown. If
                        this parameter is 0, the integration will ignore it.
      propertyMask      See TPIa_widgetWindowSetProperties(). If this
                        parameter is set to 0x8000, the integration will use the
                        device default properties for the MsfMenu.
      defaultStyle      The MsfStyle identifier. If this parameter is 0 the default
                        style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfWindowHandle TPIa_widgetMainMenuCreateEx 
(
    MSF_UINT8 modId, 
    MsfChoiceType type, 
    MsfSize* size, 
    MsfActionHandle implicitSelectAction, 
    MsfElementPosition* elementPos, 
    int bitmask, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
#if 0
    MsfWindow *pWindow;
    MsfMainMenuWindow *pMenu;
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    MsfAction *pAction;
    MsfWidgetType widgetType;

    MsfGadgetHandle   hChoice;
    MsfPosition             pos;
    MsfSize                   selectgroupSize;
    MsfSize                tempSize;
    MsfGadgetHandle        hUpArrowBtn,hDownArrowBtn,hBmpAni;
    MSF_UINT16            iUpArrowYStart, iDownArrowYStart;
    
    pAction = (MsfAction *)seekWidget(implicitSelectAction, &widgetType);
    
    if (!MSF_IS_MODULE_ID_VALID(modId) ||
        /*size == OP_NULL || */
        /*pAction == OP_NULL ||
        widgetType != MSF_ACTION ||
        (pAction->actionType != MsfSelect && pAction->actionType != MsfOk) ||*/
         (defaultStyle != 0 && isValidHandleOfType(defaultStyle, MSF_STYLE) == OP_FALSE))
    {
        return INVALID_HANDLE;
    }

    pMenu = WIDGET_NEW(MsfMainMenuWindow);
    newWindowHandle = getNewHandle(modId, MSF_MAINMENU);
    if (pMenu == OP_NULL || newWindowHandle == INVALID_HANDLE)
    {
        WIDGET_FREE(pMenu);
        newWindowHandle = INVALID_HANDLE;
    }
    else
    {
        op_memset(pMenu, 0, sizeof(MsfMainMenuWindow));
        pWindow = (MsfWindow *)pMenu;
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        if (size != OP_NULL)
        {
            op_memcpy(&pWindow->size, size, sizeof(MsfSize)); 
        }
        else
        {
            op_memcpy(&pWindow->size, &DefaultPropTab[MSF_MAINMENU].size, sizeof(MsfSize));
        }
        
        op_memcpy(&pWindow->position, &DefaultPropTab[MSF_MAINMENU].pos, sizeof(MsfPosition));

        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_MAINMENU].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
        if (defaultStyle != 0)
        {
            replaceHandle((OP_UINT32 *)&pWindow->style, defaultStyle);
        }
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;     
        pWindow->cbStateChange = cbStateChange;

        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);
        
        widgetGetClientRect( pWindow, &pos, &selectgroupSize);
      
       selectgroupSize.height = TITLE_MAIN_SUBMENU_BASE_TOP;
        /*   create selectgroup  */
        hChoice = TPIa_widgetSelectgroupCreateEx(
                                            modId, 
                                            type, 
                                            &selectgroupSize, 
                                            elementPos,
                                            bitmask,
                                            DefaultPropTab[MSF_SELECTGROUP].propertyMask, 
                                            SG_SHOW_STYLE_MAIN_MENU
                                            |SG_SHOW_STYLE_NO_BORDER 
                                            |SG_SHOW_STYLE_SEQUENCE_LAYOUT
                                            |SG_SHOW_STYLE_FOCUS_DRAW_LINE
                                            |SG_SHOW_STYLE_NO_SCROLLBAR,
                                            defaultStyle,
                                            OP_NULL);

        if( !ISVALIDHANDLE( hChoice ) )
        {
            WIDGET_FREE(pMenu);
            return INVALID_HANDLE;
        }
        pMenu->pSelectgroup = (MsfSelectgroup*)seekWidget( hChoice, OP_NULL );

        /* calc the elementsPerPage */
        pMenu->pSelectgroup->choice.elementsPerPage = selectgroupSize.height / CHOICE_MAIN_MENU_LINE_HEIGHT;

        /* scroll circularly  */
        pMenu->pSelectgroup->choice.scrollCircularly  = OP_TRUE;
        
        /* not alow the item scroll-period show  */
        pMenu->pSelectgroup->choice.bTimerStarted = OP_FALSE;
        
        
        /*create down arrow */
        widgetGetImageSize(ICON_DOWN_BLUE,&tempSize);
        iDownArrowYStart = ((CHOICE_MAIN_MENU_LINE_HEIGHT - tempSize.height) >0)
                        ?((CHOICE_MAIN_MENU_LINE_HEIGHT - tempSize.height)/2)
                        :(CHOICE_MAIN_MENU_LINE_HEIGHT);
        hUpArrowBtn = TPIa_widgetButtonCreateEx(modId,
                                                    OP_NULL,/* no string*/
                                                    &tempSize,
                                                    OP_NULL,
                                                    (const MSF_UINT32 )DownArrowNormalBackgroundRes,
                                                    (const MSF_UINT32 )DownArrowFocusBackgroundRes,
                                                    MSF_GADGET_PROPERTY_FOCUS | MSF_GADGET_PROPERTY_NOTIFY |MSF_GADGET_PROPERTY_PEN_CLICK,
                                                    defaultStyle,
                                                    widget_button_down_arrow_callback);
       
       if(!ISVALIDHANDLE(hUpArrowBtn))
       {
          TPIa_widgetRelease(hChoice);
          WIDGET_FREE(pMenu);
          return INVALID_HANDLE;
       }
       pMenu->pUpArrowButton = (MsfButtonGadget *)seekWidget(hUpArrowBtn,OP_NULL);
       /*create up arrow */
       widgetGetImageSize(ICON_UP_BLUE, &tempSize);
       iUpArrowYStart = ((CHOICE_MAIN_MENU_LINE_HEIGHT - tempSize.height) >0)
                        ?((CHOICE_MAIN_MENU_LINE_HEIGHT - tempSize.height)/2)
                        :(CHOICE_MAIN_MENU_LINE_HEIGHT);
       
       hDownArrowBtn = TPIa_widgetButtonCreateEx(modId,
                                                    OP_NULL,/* no string*/
                                                    &tempSize,
                                                    OP_NULL,
                                                    (const MSF_UINT32 )UpArrowNormalBackgroundRes,
                                                    (const MSF_UINT32 )UpArrowFocusBackgroundRes,
                                                    MSF_GADGET_PROPERTY_FOCUS | MSF_GADGET_PROPERTY_NOTIFY |MSF_GADGET_PROPERTY_PEN_CLICK,
                                                    defaultStyle,
                                                    widget_button_up_arrow_callback);
       
       if(!ISVALIDHANDLE(hDownArrowBtn))
       {
          TPIa_widgetRelease(hUpArrowBtn);
          TPIa_widgetRelease(hChoice);
          WIDGET_FREE(pMenu);
          return INVALID_HANDLE;
       }
       pMenu->pDownArrowButton = (MsfButtonGadget *)seekWidget(hDownArrowBtn,OP_NULL);
       /* create bitmap animation */
       
       hBmpAni = TPIa_widgetBmpAniGadgetCreatEx(modId,
                                                  OP_NULL,
                                                      ANI_SUB_MAINMENU_TITLE_C1_10,
                                                  100,
                                                  OP_TRUE,
                                                  DefaultPropTab[MSF_MAINMENU].propertyMask,
                                                  defaultStyle,
                                                  OP_NULL);
       if(!ISVALIDHANDLE(hBmpAni))
       {
         TPIa_widgetRelease(hDownArrowBtn);
         TPIa_widgetRelease(hUpArrowBtn);
         TPIa_widgetRelease(hChoice);
         WIDGET_FREE(pMenu);
       }
       pMenu->pBmpAni = (MsfBmpAnimation *)seekWidget(hBmpAni,OP_NULL);                                          
   
        SP_list_insert_to_head(&widgetQueue[MSF_MAINMENU], pMenu);

        /*add selectgroup*/
        TPIa_widgetWindowAddGadget( newWindowHandle, hChoice, &pos, OP_NULL );
        TPIa_widgetRelease( hChoice );  
        TPIa_widgetAddAction(newWindowHandle, implicitSelectAction);        
        /* add up arrow button*/
        pos.x = SUBMENU_BTN_X;
        pos.y = TITLE_MAIN_SUBMENU_BASE_TOP + iUpArrowYStart;
        TPIa_widgetWindowAddGadget( newWindowHandle, hUpArrowBtn, &pos, OP_NULL );
        TPIa_widgetRelease( hUpArrowBtn );  
        /* add down arrow button */
        pos.x += tempSize.width; 
        pos.y = TITLE_MAIN_SUBMENU_BASE_TOP + iDownArrowYStart;
        TPIa_widgetWindowAddGadget( newWindowHandle, hDownArrowBtn, &pos, OP_NULL );
        TPIa_widgetRelease( hDownArrowBtn ); 
        /*add bitmap animation */
        pos.x = MENUTITLE_START_X;
        pos.y = TITLE_MAIN_SUBMENU_BASE_TOP;
        TPIa_widgetWindowAddGadget( newWindowHandle, hBmpAni, &pos, OP_NULL );
        TPIa_widgetRelease( hBmpAni );   


        
    }
    return newWindowHandle;
#else
    return INVALID_HANDLE;
#endif
}

/****************************************************************
 VIEWER WINDOW
 ***************************************************************/
/*==================================================================================================
  FUNCTION:  TPIa_widgetViewerCreateEx

  DESCRIPTION:
      Creates a MsfViewer with the specified MsfSize and its own state change callback funcion and returns the
      MsfWindowHandle. The MsfViewer shows the specified MsfString.

  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      title          Specifies the title of the MsfViewer
      text          Specifies the text to be contained within the
                    MsfViewer
      size          Specifies the MsfSize of the MsfViewer.
      propertyMask      See TPIa_widgetWindowSetProperties(). If this
                        parameter is set to 0x8000, the integration will use the
                        device default properties for the MsfMenu.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetViewerCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle title, 
    MsfStringHandle text, 
    MsfSize* size, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfWindow         *pWindow;
    MsfViewer          *pViewer;
    MsfGadgetHandle  hStringGadget;
    MsfSize               clientSize;
    MsfPosition          clientPos;
    
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    if( !MSF_IS_MODULE_ID_VALID(modId)
        ||!ISVALIDHANDLE(text))
    {
        return INVALID_HANDLE;
    }
    
    pViewer = WIDGET_NEW(MsfViewer);
    if (pViewer != OP_NULL)
    {
        op_memset(pViewer, 0, sizeof(MsfViewer));
        pWindow = (MsfWindow *)pViewer;
        newWindowHandle = getNewHandle(modId, MSF_VIEWER);
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        if (size != OP_NULL)
        {
            op_memcpy(&pWindow->size, size, sizeof(MsfSize));
        }
        else
        {
            op_memcpy(&pWindow->size, &DefaultPropTab[MSF_VIEWER].size, sizeof(MsfSize));
        }
        
        op_memcpy(&pWindow->position, &DefaultPropTab[MSF_VIEWER].pos, sizeof(MsfPosition));

        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_VIEWER].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
        
        replaceHandle((OP_UINT32 *)&pWindow->title, title );
        replaceHandle((OP_UINT32 *)&pWindow->style, defaultStyle);
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;
        pWindow->cbStateChange = cbStateChange;

        /*  create a stringgadget and add it to the window
         */
           
        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);

        widgetGetClientRect( pWindow, &clientPos, &clientSize);
        hStringGadget = TPIa_widgetStringGadgetCreate(
                                                modId,
                                                text,
                                                &clientSize,
                                                0,
                                                MSF_GADGET_PROPERTY_FOCUS | MSF_GADGET_PROPERTY_LABEL |MSF_GADGET_PROPERTY_SHOW_NO_FOCUS_BAR,
                                                defaultStyle);
                                                
        if( !ISVALIDHANDLE(hStringGadget) )
        {
            WIDGET_FREE( pViewer );
            return INVALID_HANDLE;
        }
        
        SP_list_insert_to_head(&widgetQueue[MSF_VIEWER], pViewer);
        
        TPIa_widgetWindowAddGadget( newWindowHandle, hStringGadget, &clientPos, OP_NULL );
        pViewer->pStringGadget = (MsfStringGadget*)seekWidget(hStringGadget, OP_NULL);
        TPIa_widgetRelease( hStringGadget );
        
    }
    return newWindowHandle;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetViewerSetText

    DESCRIPTION:
        Set the MsfViewer's content
        
    ARGUMENTS PASSED:
    
        hViewer:    The handle of MsfViewer
        hStr          The handle of new string
        
    RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int   TPIa_widgetViewerSetText(MsfWindowHandle  hViewer,  MsfStringHandle  hStr )
{
    MsfViewer    *pViewer;
    MsfWidgetType  wtWin;

    pViewer = (MsfViewer*)seekWidget( hViewer, &wtWin);
    if( !pViewer || wtWin != MSF_VIEWER 
       || !ISVALIDHANDLE(hStr))
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    if( pViewer->pStringGadget )
    {
        return TPIa_widgetStringGadgetSet( pViewer->pStringGadget->gadgetData.gadgetHandle, hStr, 0);
    }
    
    return TPI_WIDGET_ERROR_UNEXPECTED;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWaitingWinCreateEx

  DESCRIPTION:
      Creates a MsfWaitingWin with the specified MsfSize and its own state change callback funcion 
      and returns the MsfWindowHandle. The MsfWaitingWin shows waiting flag, it can play the animation,
      or show a waiting image, or show a progressbar.

  ARGUMENTS PASSED:
      modId         The MSF Module identifier
      bShowBg     Indicates if show the background. if it is false,  the bPromptIsUp, bProgressBar,
                       hPromptStr parameters will be ignored
      bProgressBar       Specifies if use the progressbar as a waiting flag
      bPromptIsUp       Specifies if the prompt string is up( TRUE ) or down( FALSE )
      resID                 the bitmap animation resource id.
      size          Specifies the MsfSize of the MsfViewer.
      propertyMask      See TPIa_widgetWindowSetProperties(). If this
                        parameter is set to 0x8000, the integration will use the
                        device default properties for the MsfMenu.
      defaultStyle  The MsfStyle identifier. If this parameter is 0 the default
                    style of the device will be used.
      cbStateChange   The state change callback function
      
  RETURN VALUE:
      The new MsfWindowHandle if the operation was
      successful, otherwise 0
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
MsfGadgetHandle TPIa_widgetWaitingWinCreateEx 
(
    MSF_UINT8 modId, 
    int             bShowBg,
    int             bProgressBar,
    int             bPromptIsUp,
    MsfStringHandle hPromptStr, 
    MSF_UINT32     resID,
    MsfSize*          size, 
    int                  propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
)
{
    MsfWindow        *pWindow;
    MsfWaitingWin    *pWaitingWin = OP_NULL;
    MsfSize          clientSize = {0};
    MsfPosition      clientPos = {0};
    MsfGadgetHandle  hPrompt = INVALID_HANDLE;
    MsfSize          promptSize = {0};
    MsfPosition      promptPos = {0};
    MsfGadgetHandle  hProgressBar = INVALID_HANDLE;
    MsfSize          barSize = {0};
    MsfPosition      barPos = {0};
    MsfGadgetHandle  hBmpAni = INVALID_HANDLE;
    MsfSize          bmpAniSize = {0};
    MsfPosition      bmpAniPos = {0};
    RM_ICON_T        *pWaitingIcon;
    
    MsfWindowHandle newWindowHandle = INVALID_HANDLE;
    if( !MSF_IS_MODULE_ID_VALID(modId) )
    {
        return INVALID_HANDLE;
    }
    
    pWaitingWin = WIDGET_NEW(MsfWaitingWin);
    if (pWaitingWin != OP_NULL)
    {
        op_memset(pWaitingWin, 0, sizeof(MsfWaitingWin));
        pWindow = (MsfWindow *)pWaitingWin;
        newWindowHandle = getNewHandle(modId, MSF_WAITINGWIN);
        pWindow->windowHandle = newWindowHandle;
        pWindow->modId = modId;
        if (size != OP_NULL)
        {
            op_memcpy(&pWindow->size, size, sizeof(MsfSize));
        }
        else
        {
            op_memcpy(&pWindow->size, &DefaultPropTab[MSF_WAITINGWIN].size, sizeof(MsfSize));
        }
        
        op_memcpy(&pWindow->position, &DefaultPropTab[MSF_WAITINGWIN].pos, sizeof(MsfPosition));

        if(  propertyMask == 0x8000 )
        {
            pWindow->propertyMask = DefaultPropTab[MSF_WAITINGWIN].propertyMask;
        }
        else
        {
            pWindow->propertyMask = propertyMask;
        }
        
        replaceHandle((OP_UINT32 *)&pWindow->style, defaultStyle);
        pWindow->referenceCounter = 1;
        pWindow->isReleased = OP_FALSE;
        if( cbStateChange == OP_NULL)
        {
            pWindow->cbStateChange = WaitingWinStateChangeCb;
        }
        else
        {
            pWindow->cbStateChange = cbStateChange;
        }

        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);
        widgetGetClientRect( pWindow, &clientPos, &clientSize);

        if( bShowBg )
        {
            if( ISVALIDHANDLE(hPromptStr))
            {
                promptSize.width = WW_GADGET_WIDTH;
                promptSize.height = WW_PROMPT_HEIGHT;  /* temp height, need to recalculate */
                hPrompt = TPIa_widgetStringGadgetCreateEx(
                                                modId,
                                                hPromptStr, 
                                                &promptSize, 
                                                MSF_TRUE, 
                                                0, 
                                                0, 
                                                OP_NULL);
                if( !ISVALIDHANDLE(hPrompt))
                {
                    goto TPIa_widgetWaitingWinCreateEx_ERROR;
                }
            }
            
            if( bProgressBar )     /* the progress mode */
            {
                barSize.width = WW_GADGET_WIDTH;
                barSize.height = WW_PROGRESS_BAR_HEIGHT;
                hProgressBar = TPIa_widgetBarCreateEx(
                                                    modId,
                                                    MsfProgressBar,
                                                    0,
                                                    0, 
                                                    &barSize, 
                                                    0, 
                                                    0, 
                                                    OP_NULL);
                if( !ISVALIDHANDLE(hProgressBar))
                {
                    goto TPIa_widgetWaitingWinCreateEx_ERROR;
                }
                
                barPos.x = WW_GADGET_POS_X;
                if( bPromptIsUp )        /* the prompt is up */
                {
                    promptPos.y = (pWindow->size.height - (WW_PROGRESS_BAR_HEIGHT + WW_PROMPT_HEIGHT + 4)) >> 1;
                    barPos.y = promptPos.y + (WW_PROMPT_HEIGHT + 4);
                }
                else   /* the prompt is down */
                {
                    barPos.y = (pWindow->size.height - (WW_PROGRESS_BAR_HEIGHT + WW_PROMPT_HEIGHT + 4)) >> 1;
                    promptPos.y = barPos.y + (WW_PROGRESS_BAR_HEIGHT + 4);
                }
            }
            else       /* bmp animation mode */
            {   
                if( RM_IsItAnimation(resID))
                {
                    /* create bmp animation  */
                    hBmpAni = TPIa_widgetBmpAniGadgetCreatEx( 
                                                                modId,
                                                                OP_NULL,
                                                                resID,
                                                                WIDGET_ANIMATION_DELAY,
                                                                MSF_TRUE,
                                                                0,
                                                                0, 
                                                                OP_NULL);
                     if( !ISVALIDHANDLE(hBmpAni))
                     {
                        goto TPIa_widgetWaitingWinCreateEx_ERROR;
                     }
                     
                     TPIa_widgetGetSize(hBmpAni, &bmpAniSize);        
                }
                else
                {
                    pWaitingIcon = util_get_icon_from_res( ICON_WAITING );
                    bmpAniSize.width = (MSF_INT16)pWaitingIcon->biWidth;
                    bmpAniSize.height = (MSF_INT16)pWaitingIcon->biHeight;
                }
                
                bmpAniPos.x = (clientSize.width - bmpAniSize.width ) >> 1;
                if( bPromptIsUp )  /* the prompt is up */   
                {
                    promptPos.y = (pWindow->size.height - (bmpAniSize.height + WW_PROMPT_HEIGHT + 4)) >> 1;
                    bmpAniPos.y = promptPos.y + WW_PROMPT_HEIGHT + 4 ;
                }
                else      /* the prompt is down */
                {
                    bmpAniPos.y = (pWindow->size.height - (bmpAniSize.height + WW_PROMPT_HEIGHT + 4)) >> 1;
                    promptPos.y = bmpAniPos.y + bmpAniSize.height + 4;
                }
            }            
        }
        else     /*  only show a bmp animation or one of its frame  */
        {
            if( RM_IsItAnimation(resID))
            {
                /* create bmp animation  */
                hBmpAni = TPIa_widgetBmpAniGadgetCreatEx( 
                                                            modId,
                                                            OP_NULL,
                                                            resID,
                                                            WIDGET_ANIMATION_DELAY,
                                                            MSF_TRUE,
                                                            0,
                                                            0, 
                                                            OP_NULL);
                 if( !ISVALIDHANDLE(hBmpAni))
                 {
                    goto TPIa_widgetWaitingWinCreateEx_ERROR;
                 }
                 
                 TPIa_widgetGetSize(hBmpAni, &bmpAniSize);
                 bmpAniPos.x = (clientSize.width - bmpAniSize.width ) >> 1;
                 bmpAniPos.y = (clientSize.height- bmpAniSize.height) >> 1;
             }
        }

        SP_list_insert_to_head(&widgetQueue[MSF_WAITINGWIN], pWaitingWin);

        if( bShowBg )
        {
            pWaitingWin->bShowBg = OP_TRUE;
        }
        else
        {
            pWaitingWin->bShowBg = OP_FALSE;
        }
        
        if( ISVALIDHANDLE(hPrompt) )
        {
            TPIa_widgetStringGetRectSize(hPromptStr, newWindowHandle, &promptSize);
            TPIa_widgetSetSize(hPrompt, &promptSize);
            promptPos.x = (pWindow->size.width - promptSize.width) >> 1;
        }
        
        if( bProgressBar )
        {
            pWaitingWin->bProgressBar = OP_TRUE;
        }
        else
        {
            pWaitingWin->bProgressBar = OP_FALSE;
        }
        if( bPromptIsUp )
        {
            if( ISVALIDHANDLE(hPrompt) )
            {
                if( bProgressBar )
                {
                    promptPos.y = barPos.y - (4 + promptSize.height);
                }
                else
                {
                    promptPos.y = bmpAniPos.y - (4 + promptSize.height);
                }
            }
            pWaitingWin->bPromptIsUp = OP_TRUE;
        }
        else
        {
            pWaitingWin->bPromptIsUp = OP_FALSE;
        }
        
        if( ISVALIDHANDLE(hPrompt))
        {
            TPIa_widgetWindowAddGadget( newWindowHandle, hPrompt, &promptPos, OP_NULL );
            pWaitingWin->hPrompt = hPrompt;
        }

        if( ISVALIDHANDLE(hBmpAni))
        {
            TPIa_widgetWindowAddGadget( newWindowHandle, hBmpAni, &bmpAniPos, OP_NULL );
            pWaitingWin->hBmpAni = hBmpAni;
        }
        
        if( ISVALIDHANDLE(hProgressBar))
        {
            TPIa_widgetWindowAddGadget( newWindowHandle, hProgressBar, &barPos, OP_NULL );
            pWaitingWin->hBar = hProgressBar;
        }
    }
    return newWindowHandle;    

TPIa_widgetWaitingWinCreateEx_ERROR:

    if( pWaitingWin )
    {
        WIDGET_FREE(pWaitingWin);
    }
    if( ISVALIDHANDLE(hPrompt))
    {
        TPIa_widgetRelease(hPrompt);
    }
    if( ISVALIDHANDLE(hProgressBar))
    {
        TPIa_widgetRelease(hProgressBar);
    }
    if( ISVALIDHANDLE(hBmpAni))
    {
        TPIa_widgetRelease(hBmpAni);
    }

    return INVALID_HANDLE;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWaitingWinPlayAnimation

  DESCRIPTION:
      Notify the waiting window to play animation.

  ARGUMENTS PASSED:
      hWaitingWin   The waiting window identifier
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int  TPIa_widgetWaitingWinPlayAnimation(MsfWindowHandle  hWaitingWin)
{
    MsfWaitingWin   *pWaitingWin;
    MsfWidgetType  wt;

    pWaitingWin = (MsfWaitingWin*)seekWidget(hWaitingWin, &wt);
    if( !pWaitingWin || wt != MSF_WAITINGWIN)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    if( ISVALIDHANDLE(pWaitingWin->hBmpAni))
    {
        return TPIa_widgetBmpAnimationPlay(pWaitingWin->hBmpAni, MSF_FALSE);
    }

    return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWaitingWinStopAnimation

  DESCRIPTION:
      Notify the waiting window to stop playing animation.

  ARGUMENTS PASSED:
      hWaitingWin   The waiting window identifier
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int  TPIa_widgetWaitingWinStopAnimation(MsfWindowHandle  hWaitingWin)
{
    MsfWaitingWin   *pWaitingWin;
    MsfWidgetType  wt;

    pWaitingWin = (MsfWaitingWin*)seekWidget(hWaitingWin, &wt);
    if( !pWaitingWin || wt != MSF_WAITINGWIN)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    if( ISVALIDHANDLE(pWaitingWin->hBmpAni))
    {
        return TPIa_widgetBmpAnimationStop(pWaitingWin->hBmpAni );
    }

    return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWaitingWinSetPrompt

  DESCRIPTION:
      Set the prompt string of the waiting window. Note: the waiting window must be created with background mode.
      
  ARGUMENTS PASSED:
      hWaitingWin   The waiting window identifier
      hPromptStr    The new prompt string
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int  TPIa_widgetWaitingWinSetPrompt(MsfWindowHandle  hWaitingWin, MsfStringHandle  hPromptStr )
{
    MsfWaitingWin   *pWaitingWin;
    MsfWidgetType   wt;
    MsfSize         size, oldSize;
    MsfPosition     pos;

    pWaitingWin = (MsfWaitingWin*)seekWidget(hWaitingWin, &wt);
    if( !pWaitingWin || wt != MSF_WAITINGWIN)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    if( pWaitingWin->bShowBg == OP_FALSE )
    {
        return TPI_WIDGET_ERROR_NOT_SUPPORTED;
    }

    if( ISVALIDHANDLE(pWaitingWin->hPrompt))
    {
        TPIa_widgetUpdateBegin(hWaitingWin);
        size.width = WW_GADGET_WIDTH;
        TPIa_widgetStringGetRectSize( hPromptStr, hWaitingWin, &size);
        TPIa_widgetGetSize( pWaitingWin->hPrompt, &oldSize);
        TPIa_widgetSetSize( pWaitingWin->hPrompt, &size);
        TPIa_widgetStringGadgetSet(pWaitingWin->hPrompt, hPromptStr, MSF_TRUE);
        TPIa_widgetGetPosition( pWaitingWin->hPrompt, &pos);

        pos.x = (pWaitingWin->windowData.size.width - size.width) >> 1;
        if( pWaitingWin->bPromptIsUp )
        {
            pos.y = ( pos.y + oldSize.height) - size.height;
            TPIa_widgetSetPosition(pWaitingWin->hPrompt, &pos, OP_NULL);
        }
        TPIa_widgetUpdateEnd(hWaitingWin);
        
        return TPI_WIDGET_OK;
    }

    return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

/*==================================================================================================
  FUNCTION:  TPIa_widgetWaitingWinSetProgress

  DESCRIPTION:
      Set the prompt string of the waiting window.
      
  ARGUMENTS PASSED:
      hWaitingWin   The waiting window identifier
      iCurPos          The current value of the progress
      iTotal            The total value of the whole progress
      
  RETURN VALUE:
      A positive integer if the operation was successful, otherwise the
      appropriate return code is given.
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int  TPIa_widgetWaitingWinSetProgress(MsfWindowHandle  hWaitingWin, int iCurPos, int iTotal )
{
    MsfWaitingWin   *pWaitingWin;
    MsfWidgetType  wt;

    pWaitingWin = (MsfWaitingWin*)seekWidget(hWaitingWin, &wt);
    if( !pWaitingWin || wt != MSF_WAITINGWIN)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    if( ISVALIDHANDLE(pWaitingWin->hBar))
    {
        return TPIa_widgetBarSetValues(pWaitingWin->hBar, iCurPos, iTotal);
    }

    return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}


/*==================================================================================================
  FUNCTION:  TPIa_widgetTextInputInsertString

  DESCRIPTION:
      insert string in the text input
      
  ARGUMENTS PASSED:
      hText:           handle of text input gadget
      hInsert:          handle of unicode string inserted, 
      bFirstInsert:     OP_TRUE--- the first inserting for the text input
                        OP_FALSE---after the first inserting
      
  RETURN VALUE:
      TPI_WIDGET_OK: insert string successfully 
      TPI_WIDGET_ERROR_INVLAIDE_PARAM: invalid parameter         
      TPI_WIDGET_ERROR_UNEXPECTED: unknown error
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
int TPIa_widgetTextInputInsertString(MSF_UINT32 hText, MsfStringHandle hInsert, MSF_BOOL bFirstInsert)
{
    MsfTextInput *pTextInput = OP_NULL;
    MsfWidgetType widgetType;
    OP_BOOLEAN    ret = OP_FALSE;
    MsfString     *pStr = OP_NULL;
    OP_UINT16     insertStrLen = 0;
    void          *ptr;
    OP_UINT8      *pChar= OP_NULL;

    if( !ISVALIDHANDLE(hText) || !ISVALIDHANDLE(hInsert))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM ;
    }

    ptr = seekWidget(hText, &widgetType);
    if( ptr )
    {
        if( widgetType == MSF_EDITOR )
        {
            MsfEditor      *pEditor;
            pEditor = (MsfEditor*)ptr;
            pTextInput = pEditor->input;
        }
        else if( widgetType == MSF_TEXTINPUT )
        {
            pTextInput = (MsfTextInput *)ptr;
        }
        else
        {
            return  TPI_WIDGET_ERROR_INVALID_PARAM;
        }
    }
    
    if( OP_NULL == pTextInput)
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    pStr = (MsfString *)seekWidget(hInsert, &widgetType);
    if( pStr ) 
    {
        if( widgetType == MSF_STRING_RES )
        {
            pChar = (OP_UINT8*)pStr;
        }
        else if( widgetType == MSF_STRING && pStr->data != OP_NULL )
        {
            pChar = pStr->data;
        }
    }
    
    if(OP_NULL == pChar)
    {
      return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    insertStrLen = Ustrlen(pChar);
    if( insertStrLen > 0 )
    {
        OP_BOOLEAN insertHasChinese = OP_FALSE;
        int i = 0;

        if(bFirstInsert)
        {
            /*Whether Chinese exist in the inserted string */
            while(pChar[i] != 0 || pChar[i+1] !=0)
            {
                if(!(pChar[i] <0x7f && pChar[i+1] ==0))
                {
                    insertHasChinese = OP_TRUE;
                    break;
                }
                i += 2;
            }          
            if(insertHasChinese)
            {
                if(insertStrLen > pTextInput->input.txtInputBufMng.maxCharCount)
                {
                    insertStrLen = pTextInput->input.txtInputBufMng.maxCharCount - pTextInput->input.txtInputBufMng.maxCharCount%2;
                }
            }
            else
            {
                if(insertStrLen > pTextInput->input.txtInputBufMng.maxCharCount * 2)
                {
                    insertStrLen = pTextInput->input.txtInputBufMng.maxCharCount * 2;
                }
            }
        }
        ret = TBM_EnterStringEx(&pTextInput->input.txtInputBufMng, pChar, insertStrLen, pTextInput->input.textType );
        if( ret == OP_FALSE )
        {
            return TPI_WIDGET_ERROR_CONTENT_TOO_LONG;
        }
    }
    if(ret)
    {
        return TPI_WIDGET_OK;
    }
    else
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
}

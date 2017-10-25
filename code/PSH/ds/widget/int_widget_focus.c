/*==================================================================================================

    MODULE NAME : int_widget_focus.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    07/26/2003   Steven Lai       Cxxxxxx    Create file
    08/14/2003   Steven Lai       P001256      Rearrange the widget show function calling code
    8/18/2003     Zhuxq             P001263    Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    08/20/2003   Steven Lai       P001310       Improve the method of checking whether a widget is in focus
    08/20/2003   Zhuxq             P001279      solve the problem that when gadget focus changes, the pre focus gadget not redraw 
    08/25/2003   Zhuxq             P001355      Fixed some warning according to PC-Lint check result
    08/29/2003    Zhuxq             P001396      Add scroll-contents feature to string gadget
    09/03/2003   Zhuxq             P001479      When set screen infocus, enhance the judge condition: if wap has focus
    09/25/2003   Zhuxq             P001696     Re-implement the dialog
    09/26/2003   linda wang        P001451      separate the gif play and show function.    
    10/17/2003   Zhuxq             C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq             P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq             P001883     Optimize the redraw strategy to reduce the redraw times
    01/05/2004   Zhuxq            CNXT00000072  Add callback function to each widget in the widget system
    01/15/2004   Zhuxq            P002352     All editing actions of MsfTextInput turns to MsfEditor
    02/10/2004   chenxiao         p002405     add form scrollbar  in the dolphin project
    02/16/2004   Zhuxq             P002469      Add button gadget to widget system
    02/16/2004   chenxiao           p002474     add bitmap animation function in the dolphin project    
    02/17/2004   Zhuxq             P002492      Add Viewer window to widget system
    02/19/2004   Zhuxq             P002504      Add box gadget to widget system
    02/20/2004   Zhuxq             P002517      Adjust the handle order of pen events in widget system
    02/24/2004   chenxiao          p002530      modify WAP & MSG main menu     
    03/27/2004   Zhuxq             P002760      Fix some bugs in widget system
    03/30/2004    zhuxq            P002700        Fix UMB bugs
    03/31/2004   Dingjianxin         P002754       Delete the imagegadgetGetFocus and imagegadgetLoseFocus functions, their function handled by gadgetGetFocus and gadgetLoseFocus.
    04/02/2004    zhuxq            P002789        Fix UMB bugs
    04/15/2004    zhuxq            P002977        Fix some bugs in UMB for Dolphin
    04/162004     Dingjianxin      P002961        
    05/24/2004    zhuxq            P005568        Adjust UI implementation in UMB and fix some bugs
    06/01/2004    zhuxq            P005819        Optimize UMB codes
    06/04/2004    zhuxq            P005925        Correct issues found by PC-lint and fix some bugs
    06/09/2004    zhuxq            P006048        forbid Menu to respond successive multiple actions
    06/18/2004    zhuxq            P006260        Make up WAP&UMB code to cnxt_int_wap branch
    07/15/2004    zhuxq            P007016        Fix the bug that paintbox can't be updated with imm 
    07/23/2004    zhuxq            P007215        Prevent freeze in wap
    08/03/2004    gaoyan           p007467        replace function textinputGetFocus
    08/30/2004    zhuxq            c007998        improve the implementation of predefined string to reduce the times of memory allocation
    09/03/2004    zhuxq            p008122        Fix the bug that when opening a page incompletely (such as 4/5), press "menu",  then back to the browser window , the hypertext in the page can't be opened any more

    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include  "app_include.h"

#ifdef WIN32
#include "portab_new.h"
#else
#include "portab.h"
#endif
#include "GSM_timer.h"
    
#include "msf_int.h"
    
#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "int_widget_focus.h"
#include "int_widget_show.h"
#include "int_widget_custom.h"
#include "int_widget_imm.h"
#include "int_widget_txt_input.h"
extern int widgetPaintBoxScroll
(
    MsfWindow    *pPaintBox,
    OP_INT16     iDeltaX,
    OP_INT16     iDeltaY
);

extern void HDIc_widgetNotify(MSF_UINT8 modId, MSF_UINT32 msfHandle, MsfNotificationType notificationType);


extern int widgetGadgetGetPosition( const MsfGadget* pGadget,  OP_INT16  *pX, OP_INT16  *pY);

extern int  widgetInputFocusChange
( 
    OP_UINT32       hInput, 
    OP_BOOLEAN    bInfocus
);

extern OP_BOOLEAN widgetFocusGadgetAdjustWindow
(
    MsfGadget  *pGadget,
    MsfWindow   *pWin
);

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/

/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/*
 * description of this macro, if needed. - Remember self documenting code
 */

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
#ifdef _NO_CLEAR_KEY
static  OP_UINT8   skeyStr[SKEY_MAX_CHAR*2+2];
#endif
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

WidFocusFunc msfWidFocusFuncs[MSF_MAX_WIDGETTYPE] =
{
    {screenGetFocus,  screenLoseFocus},             /* MSF_SCREEN */
    {windowGetFocus,  windowLoseFocus},          /* MSF_PAINTBOX */
    {windowGetFocus,  windowLoseFocus},          /* MSF_FORM */
    {windowGetFocus,  dialogLoseFocus},                           /* MSF_DIALOG */
    {windowGetFocus,  menuLoseFocus},               /* MSF_MENU */
    {windowGetFocus,  windowLoseFocus},              /* MSF_MAINMENU */
    {windowGetFocus,  windowLoseFocus},               /* MSF_VIEWER */
    {windowGetFocus,  windowLoseFocus},               /* MSF_WAITINGWIN */
    {windowGetFocus,  windowLoseFocus},               /* MSF_EDITOR */
    {gadgetGetFocus,  selectgroupLoseFocus},                 /* MSF_SELECTGROUP */
    {gadgetGetFocus,  gadgetLoseFocus},                         /* MSF_DATETIME */
    {textinputGetFocus,  textinputLostFocus},      /* MSF_TEXTINPUT */
    {gadgetGetFocus,  stringGadgetLoseFocus},            /* MSF_STRINGGADGET */
   /* delete for add HDIa_widgetImagePlay and HDIa_widgetImageStop */
    {gadgetGetFocus,  gadgetLoseFocus},       /* MSF_IMAGEGADGET */  /*{imagegadgetGetFocus,  imagegadgetLoseFocus},*/ 
    {gadgetGetFocus,    gadgetLoseFocus},  /* MSF_BUTTON */
    {gadgetGetFocus, gadgetLoseFocus},     /* MSF_BMPANIMATION */
    {gadgetGetFocus,    selectgroupLoseFocus},  /* MSF_BOX */
    {gadgetGetFocus,    gadgetLoseFocus},  /* MSF_BAR */
    {OP_NULL,  OP_NULL},  /* MSF_IMAGE */
    {OP_NULL,  OP_NULL},  /* MSF_STRING */
    {OP_NULL,  OP_NULL},  /* MSF_ICON */
    {OP_NULL,  OP_NULL},  /* MSF_SOUND */
    {OP_NULL,  OP_NULL},  /* MSF_STYLE */
    {OP_NULL,  OP_NULL},  /* MSF_COLORANIM */
    {OP_NULL,  OP_NULL},  /* MSF_MOVE */
    {OP_NULL,  OP_NULL},  /* MSF_ROTATION */
    {OP_NULL,  OP_NULL},  /* MSF_ACTION */
    {OP_NULL,  OP_NULL}  /* MSF_STRING_RES */
};

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/


/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
int screenGetFocus(void *pWidget)
{
    MsfScreen *pScreen;
    MsfWindow *pWindow;
    MsfWidgetType widgetType;
    pScreen = (MsfScreen *)pWidget;

    if (pScreen)
    {
        pScreen->isFocused = OP_TRUE;
        if  (pScreen->focusedChild != INVALID_HANDLE)
        {
            pWindow = (MsfWindow *)seekWidget(pScreen->focusedChild, &widgetType);
            if (pWindow && widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[widgetType].pWidGetFocus)
            {
                (void)msfWidFocusFuncs[widgetType].pWidGetFocus(pWindow);
            }
        }
        
        if( pScreen->notify )
        {
            if( pScreen->cbStateChange != OP_NULL )
            {
                (void)pScreen->cbStateChange(pScreen->modId, pScreen->screenHandle, MsfNotifyFocus, OP_NULL);
            }
            else
            {
                HDIc_widgetNotify( pScreen->modId, pScreen->screenHandle, MsfNotifyFocus);
            }
        }        
    }

    return HDI_WIDGET_OK;
}

void screenLoseFocus(void *pWidget)
{
    MsfScreen *pScreen;
    MsfWindow *pWindow;
    MsfWidgetType widgetType;
    pScreen = (MsfScreen *)pWidget;

    if (pScreen)
    {
        pScreen->isFocused = OP_FALSE;
        if (pScreen->focusedChild != INVALID_HANDLE)
        {
            pWindow = (MsfWindow *)seekWidget(pScreen->focusedChild, &widgetType);
            if (pWindow && widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[widgetType].pWidLoseFocus)
            {
                (void)msfWidFocusFuncs[widgetType].pWidLoseFocus(pWindow);
            }
       }
       
        if( pScreen->notify )
        {
            if( pScreen->cbStateChange != OP_NULL )
            {
                (void)pScreen->cbStateChange(pScreen->modId, pScreen->screenHandle, MsfNotifyLostFocus, OP_NULL);
            }
            else
            {
                HDIc_widgetNotify( pScreen->modId, pScreen->screenHandle, MsfNotifyLostFocus);
            }
        }        
    }
}

int  windowGetFocus(void *pWidget)
{
    MsfWindow *pWindow;
    MsfGadget  *pGadget;
    MsfWidgetType widgetType;
    int             iRet = HDI_WIDGET_OK;
    
    pWindow = (MsfWindow *)pWidget;

    if (pWindow && pWindow->parent && pWindow->parent->isFocused)
    {
        pWindow->isFocused = OP_TRUE;
        if( pWindow->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY )
        {
            if( pWindow->cbStateChange != OP_NULL )
            {
                (void)pWindow->cbStateChange(pWindow->modId, pWindow->windowHandle, MsfNotifyFocus, OP_NULL);
            }
            else
            {
                HDIc_widgetNotify( pWindow->modId, pWindow->windowHandle, MsfNotifyFocus );
            }
        } 
        
        if (pWindow->focusedChild != OP_NULL)
        {
            pGadget = pWindow->focusedChild->gadget;
            if (pGadget)
            {
                widgetType = getWidgetType(pGadget->gadgetHandle);
                if ( widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[widgetType].pWidGetFocus)
                {
                    iRet = msfWidFocusFuncs[widgetType].pWidGetFocus(pGadget);
                }
                
                if( pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
                {
                    if( pGadget->cbStateChange != OP_NULL )
                    {
                        (void)pGadget->cbStateChange(pGadget->modId, pGadget->gadgetHandle, MsfNotifyFocus, OP_NULL);
                    }
                    else
                    {
                        HDIc_widgetNotify( pGadget->modId, pGadget->gadgetHandle, MsfNotifyFocus);
                    }
                }        
            }
        }
    }
    return iRet;
}

void windowLoseFocus(void *pWidget)
{
    MsfWindow *pWindow;
    MsfGadget  *pGadget;
    MsfWidgetType widgetType;
    pWindow = (MsfWindow *)pWidget;

    if (pWindow && pWindow->isFocused)
    {
        pWindow->isFocused = OP_FALSE;
        
        if (pWindow->focusedChild != OP_NULL)
        {
            pGadget = pWindow->focusedChild->gadget;
            if (pGadget)
            {
                widgetType = getWidgetType(pGadget->gadgetHandle);
                if (widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[widgetType].pWidLoseFocus)
                {
                    (void)msfWidFocusFuncs[widgetType].pWidLoseFocus(pGadget);
                }
                
                if( pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
                {
                    if( pGadget->cbStateChange != OP_NULL )
                    {
                        (void)pGadget->cbStateChange(pGadget->modId, pGadget->gadgetHandle, MsfNotifyLostFocus, OP_NULL);
                    }
                    else
                    {
                        HDIc_widgetNotify( pGadget->modId, pGadget->gadgetHandle, MsfNotifyLostFocus);
                    }
                }        
            }
        }
       
        if( pWindow->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY )
        {
            if( pWindow->cbStateChange != OP_NULL )
            {
                (void)pWindow->cbStateChange(pWindow->modId, pWindow->windowHandle, MsfNotifyLostFocus, OP_NULL);
            }
            else
            {
                HDIc_widgetNotify( pWindow->modId, pWindow->windowHandle, MsfNotifyLostFocus);
            }
        }        
    }
    
     
}

void dialogLoseFocus(void * pWidget)
{
//    MsfDialog *pDialog;
//    pDialog = (MsfDialog *)pWidget;

    windowLoseFocus(pWidget);
}

void menuLoseFocus(void *pWidget)
{
    MsfWindow *pWindow;
    MsfGadget  *pGadget;
    MsfWidgetType widgetType;
    int              i;
    
    pWindow = (MsfWindow *)pWidget;

    if (pWindow && pWindow->isFocused)
    {
        pWindow->isFocused = OP_FALSE;
        
        if (pWindow->focusedChild != OP_NULL)
        {
            pGadget = pWindow->focusedChild->gadget;
            if (pGadget)
            {
                widgetType = getWidgetType(pGadget->gadgetHandle);
                if (widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[widgetType].pWidLoseFocus)
                {
                    (void)msfWidFocusFuncs[widgetType].pWidLoseFocus(pGadget);
                }
                
                if( pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
                {
                    if( pGadget->cbStateChange != OP_NULL )
                    {
                        (void)pGadget->cbStateChange(pGadget->modId, pGadget->gadgetHandle, MsfNotifyLostFocus, OP_NULL);
                    }
                    else
                    {
                        HDIc_widgetNotify( pGadget->modId, pGadget->gadgetHandle, MsfNotifyLostFocus);
                    }
                }        
            }
        }

        /* enable the action */
        for( i = 0; i < 8; i++ )
        {
            if( pWindow->actionlist[i] )
            {
                pWindow->actionlist[i]->propertyMask &= ~MSF_ACTION_PROPERTY_BLOCK;
            }
        }
        
        if( pWindow->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY )
        {
            if( pWindow->cbStateChange != OP_NULL )
            {
                (void)pWindow->cbStateChange(pWindow->modId, pWindow->windowHandle, MsfNotifyLostFocus, OP_NULL);
            }
            else
            {
                HDIc_widgetNotify( pWindow->modId, pWindow->windowHandle, MsfNotifyLostFocus);
            }
        }        
    }
    
     
}

#if 0
void editorGetFocus(void *pWidget)
{
    MsfWindow *pWindow;
    MsfEditor     *pEditor;
    
    pWindow = (MsfWindow *)pWidget;
    
    if (pWindow->parent && pWindow->parent == seekFocusedScreen())
    {
        pEditor = (MsfEditor *)pWindow;

        if( pEditor->input != OP_NULL && msfWidFocusFuncs[MSF_TEXTINPUT].pWidGetFocus )
        {
            (void)msfWidFocusFuncs[MSF_TEXTINPUT].pWidGetFocus( pEditor->input );
        }
    }
    
}

void editorLoseFocus(void *pWidget)
{
    MsfWindow *pWindow;
    MsfEditor     *pEditor;
    
    pWindow = (MsfWindow *)pWidget;
    
    if (pWindow->parent && pWindow->parent == seekFocusedScreen())
    {
        pEditor = (MsfEditor *)pWindow;

        if( pEditor->input != OP_NULL && msfWidFocusFuncs[MSF_TEXTINPUT].pWidLoseFocus )
        {
            (void)msfWidFocusFuncs[MSF_TEXTINPUT].pWidLoseFocus( pEditor->input );
        }
    }

}
#endif

int gadgetGetFocus(void *pWidget)
{
    MsfGadget  *pGadget;
    pGadget = (MsfGadget *)pWidget;

    if (pGadget && pGadget->parent && pGadget->parent->isFocused)
    {
        pGadget->isFocused = OP_TRUE;
    }
    return HDI_WIDGET_OK;
     
}

void gadgetLoseFocus(void *pWidget)
{
    MsfGadget  *pGadget;
    pGadget = (MsfGadget *)pWidget;

    if (pGadget && pGadget->isFocused)
    {
        pGadget->isFocused = OP_FALSE;
    }
     
}


void selectgroupLoseFocus(void *pWidget)
{
    MsfSelectgroup *pSelectgroup;
    pSelectgroup = (MsfSelectgroup *)pWidget;

    if (pSelectgroup && pSelectgroup->gadgetData.isFocused)
    {
        pSelectgroup->gadgetData.isFocused = OP_FALSE;

        if( pSelectgroup->choice.bTimerStarted == OP_TRUE )
        {
            OPUS_Stop_Timer(OPUS_TIMER_CHOICITEM_SCROLL);
            pSelectgroup->choice.bTimerStarted = OP_FALSE;
        }
    }
}

int textinputGetFocus(void *pWidget)
{
    MsfGadget *pGadget;
    MsfWindow *pWindow;
    int           iRet = HDI_WIDGET_OK;
    
    pGadget = (MsfGadget *)pWidget;
    pWindow = (MsfWindow *)pGadget->parent;
 
    /* If the parent window is in focus */
    if (pWindow &&
        pWindow->isFocused)
    {
        pGadget->isFocused = OP_TRUE; 
 
        if(pGadget->propertyMask & MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM  )
        {
            if( getWidgetType(pWindow->windowHandle) != MSF_PAINTBOX )
            {
                iRet=widgetInputFocusChange(pGadget->gadgetHandle, OP_TRUE );
            }
            
#ifdef   _NO_CLEAR_KEY
            if (((MsfTextInput *)pWidget)->input.txtInputBufMng.iValidCharCount > 0 )
            {
                util_get_text_from_res_w_max_len(PMPT_SKEY_DELETE, skeyStr, SKEY_MAX_CHAR*2 );
                pGadget->softKeyList[2] = skeyStr;
            }
#endif
        }                        
    }     
    return iRet;
}
    
     


void textinputLostFocus(void *pWidget)
{
    MsfGadget *pGadget;
    pGadget = (MsfGadget *)pWidget;

    if (pGadget->isFocused)
    {
        pGadget->isFocused = OP_FALSE;
        widgetInputFocusChange( pGadget->gadgetHandle, OP_FALSE );      
        OPUS_Stop_Timer(OPUS_TIMER_TXT_INPUT_CURSOR_BLINK);
    }
}

/*==================================================================================================
    FUNCTION: stringGadgetLoseFocus

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void stringGadgetLoseFocus(void *pWidget)
{
    MsfStringGadget  *pStringGadget = (MsfStringGadget*)pWidget;

    if( pStringGadget->gadgetData.isFocused )
    {
        pStringGadget->gadgetData.isFocused = OP_FALSE;
        pStringGadget->iCurPageIndex = 0;
    }
} 
/*==================================================================================================
    FUNCTION: removeScreenFocus

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int removeScreenFocus(MsfScreen *pScreen)
{
    ScreenInDisplay *pScreenInDisplay;
    MsfScreen      *pNextScreen;

    if( !pScreen )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pDisplay->hScreenSave == pScreen->screenHandle )   /* the focused screen is cached */
    {
        pScreenInDisplay = pDisplay->screenList;
        if (pScreenInDisplay->next && pScreenInDisplay->next->screen )  /* switch the first two nodes in this link */
        {
            /* adjust the screen list */
            pDisplay->screenList = pScreenInDisplay->next;
            pScreenInDisplay->next = pDisplay->screenList->next;
            pDisplay->screenList->next = pScreenInDisplay;
            
            pNextScreen = pDisplay->screenList->screen;
            if( widgetAppIsCurrentFocusApp(pNextScreen->modId ))  /* the next app has focus */
            {
                pDisplay->hScreenSave = INVALID_HANDLE;
                pDisplay->focusedScreen = pNextScreen;
                screenGetFocus(pNextScreen);
            }
            else
            {
                pDisplay->focusedScreen = OP_NULL;
                pDisplay->hScreenSave = pNextScreen->screenHandle;
            }
        }
    }
    else
    {
        if (pScreen->isFocused)  /* pScreen is the current focus screen */
        {
            screenLoseFocus(pScreen);
            pScreenInDisplay = pDisplay->screenList;
            if (pScreenInDisplay->next)  /* switch the first two nodes in this link */
            {
                pDisplay->screenList = pScreenInDisplay->next;
                pScreenInDisplay->next = pDisplay->screenList->next;
                pDisplay->screenList->next = pScreenInDisplay;

                pNextScreen = pDisplay->screenList->screen;
                if( widgetAppIsCurrentFocusApp(pNextScreen->modId ))  /* the next app has focus */
                {
                    pDisplay->focusedScreen = pNextScreen;
                    screenGetFocus(pNextScreen);
                }
                else
                {
                    pDisplay->focusedScreen = OP_NULL;
                    pDisplay->hScreenSave = pNextScreen->screenHandle;
                }
            }
            else
            {
                pDisplay->focusedScreen = OP_NULL;
            }
        }
        
        widgetRedraw( OP_NULL, MSF_SCREEN, getFocusedScreenHandle(), OP_TRUE);
    }
    return HDI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: removeWindowFocus

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int removeWindowFocus(MsfWindow *pWin)
{
    MsfScreen *pScreen;
    WindowsInScreen *pWinInScrn;
    MsfWidgetType   widgetType;

    if (pWin == OP_NULL)
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pScreen = pWin->parent;
    widgetType = getWidgetType(pWin->windowHandle);
    
    if (pScreen != OP_NULL && pScreen->children != OP_NULL)
    {
        pWinInScrn = pScreen->children;              
    }
    else
    {
        return HDI_WIDGET_ERROR_UNEXPECTED;
    }
    
    if(pScreen->focusedChild != pWin->windowHandle )
    {
        return HDI_WIDGET_OK;
    }

    pScreen->focusedChild = INVALID_HANDLE;

    if (widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[widgetType].pWidLoseFocus)
    {
        (void)msfWidFocusFuncs[widgetType].pWidLoseFocus(pWin);
    }

    if( !pWinInScrn->next)  /* Other windows exist in current screen */
    {
        /* 
         *Set previous window in focus and switch the first two elements of WindowsInScreen queue
         *First: disconnect the first element.
         */
        pScreen->children = pWinInScrn->next;
        pWinInScrn->next->prev = OP_NULL;
    
        pScreen->focusedChild = ((MsfWindow*)pScreen->children->window)->windowHandle;
    
        /*
         * Second: Insert the disconnected element after the first children window.
         */
        pWinInScrn->next = pScreen->children->next;
        pWinInScrn->prev = pScreen->children;
        if(pWinInScrn->next)
        {
            pWinInScrn->next->prev = pWinInScrn;
        }
        pWinInScrn->prev->next = pWinInScrn;
        if (widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[getWidgetType(pScreen->focusedChild)].pWidGetFocus)
        {
            (void)msfWidFocusFuncs[getWidgetType(pScreen->focusedChild)].pWidGetFocus(pWinInScrn->window);
        }
     }

    widgetRedraw( OP_NULL, MSF_SCREEN, getFocusedScreenHandle(), OP_TRUE);

     return HDI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: removeGadgetFocus

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int removeGadgetFocus(MsfGadget *pGadget)
{
    MsfWindow *pWin = OP_NULL;
    MsfWidgetType  widgetType;

    if (!pGadget)
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    if ( pGadget->isFocused )
    {
        pWin = (MsfWindow*)pGadget->parent;
        pWin->focusedChild = OP_NULL;
        widgetType = getWidgetType(pGadget->gadgetHandle);
        if (widgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[widgetType].pWidLoseFocus)
        {
           (void)msfWidFocusFuncs[widgetType].pWidLoseFocus(pGadget);
        }

        if( (pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
          /*&&(getWidgetType(pWin->windowHandle ) == MSF_PAINTBOX )*/)
        {
            if( pGadget->cbStateChange )
            {
                (void)pGadget->cbStateChange(pGadget->modId, pGadget->gadgetHandle, MsfNotifyLostFocus, OP_NULL);
            }
            else
            {
                HDIc_widgetNotify(pGadget->modId, pGadget->gadgetHandle, MsfNotifyLostFocus );
            }
        }
    }
    return HDI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: setScreenFocus

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int setScreenInFocus(MsfScreen  *pScreen)
{
    ScreenInDisplay *pScreenInDisplay, *pTemp;

    if (!pDisplay->screenList || !pScreen)
    {
        return HDI_WIDGET_ERROR_NOT_FOUND;
    }
    
    /* find the screen node to be set in focus */
    pScreenInDisplay = pDisplay->screenList;
    pTemp = OP_NULL;
    while (pScreenInDisplay)
    {
        if (pScreenInDisplay->screen == pScreen)
        {
            break;
        }
        pTemp = pScreenInDisplay;
        pScreenInDisplay = pScreenInDisplay->next;
    }

    /* not found, return error */
    if (!pScreenInDisplay)
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    /* the app related to the screen is the current focus app  */
    if( widgetAppIsCurrentFocusApp(pScreen->modId ) )
    {
        /* notify the old focus screen to lose focus */
        if( pDisplay->hScreenSave == INVALID_HANDLE && pDisplay->focusedScreen )
        {
            screenLoseFocus(pDisplay->focusedScreen);
        }

        /* swap the screen nodes position to let the focus screen as the first screen */
        if (pTemp)
        {
            /* the screen is not the first node in the link */
            pTemp->next = pScreenInDisplay->next;
            pScreenInDisplay->next = pDisplay->screenList;
            pDisplay->screenList = pScreenInDisplay;
        }

        /* record the focus screen, and reset the saved screen */
        pDisplay->hScreenSave = INVALID_HANDLE;
        pDisplay->focusedScreen = pScreen;

        if(pScreen->children)                /* Has child window*/
        {
             if(pScreen->hasBeenFocused == OP_FALSE) /*screen has not been in focus before*/
             {
                 pScreen->hasBeenFocused = 1;
                 pScreen->focusedChild = ((MsfWindow*)pScreen->children->window)->windowHandle;
             }
        }

        /* notify the screen to get focus */
        screenGetFocus(pScreen);

        /* redraw the screen */
        widgetRedraw( OP_NULL, MSF_SCREEN, pScreen->screenHandle, OP_TRUE);
    }
    else   /* the app related to the screen is not the current focus app */
    {
        if( pDisplay->hScreenSave != INVALID_HANDLE )
        {
            if( pDisplay->hScreenSave == pScreen->screenHandle )  /* already is the saved screen */
            {
                return HDI_WIDGET_OK;
            }
        }
        else if( pDisplay->focusedScreen )
        {
            screenLoseFocus(pDisplay->focusedScreen);
        }
        
        if (pTemp)
        {
            /* the screen is not the first node in the link */
            pTemp->next = pScreenInDisplay->next;
            pScreenInDisplay->next = pDisplay->screenList;
            pDisplay->screenList = pScreenInDisplay;
        }

        /* save the screen */
        pDisplay->hScreenSave = pScreen->screenHandle;
        pDisplay->focusedScreen = OP_NULL;            
    }

    return HDI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION: setWindowInFocus

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int setWindowInFocus(MsfWindow *pWin)
{
    MsfScreen *pScreen;
    MsfWindow *pPreFocusWin;
    MsfWidgetType preFocusWinType, wt;
    WindowsInScreen *pWinInScrn;
    int ret = HDI_WIDGET_OK;

    pScreen = pWin->parent;
    if (pScreen != OP_NULL && pScreen->children != OP_NULL)
    {
        pWinInScrn = pScreen->children;              
    }
    else
    {
        return HDI_WIDGET_ERROR_UNEXPECTED;
    }

    if(pScreen->focusedChild != pWin->windowHandle)       /*Set a new window focused */
    {
        while(  pWinInScrn && 
                (pWinInScrn->window != pWin) )
        {
            pWinInScrn = pWinInScrn->next;
        }

        if(!pWinInScrn)
        {
            ret = HDI_WIDGET_ERROR_NOT_FOUND;
        }
        else
        {
            pPreFocusWin = (MsfWindow *)seekWidget(pScreen->focusedChild, &preFocusWinType);
            if (pPreFocusWin && preFocusWinType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[preFocusWinType].pWidLoseFocus)
            {
                (void)msfWidFocusFuncs[preFocusWinType].pWidLoseFocus(pPreFocusWin);
            }
            
            pScreen->focusedChild = pWin->windowHandle;
            if(pWin->hasBeenFocused == OP_FALSE)   /*window has been in focus before*/
            {
                pWin->hasBeenFocused = OP_TRUE;
            }

            /*
             * place this WindowInScreen element at the head of WindowInScreen queue. 
             */
            if(pWinInScrn->prev != OP_NULL )    /* not the first element*/
            {
                /*
                 * First: disconnect this element.
                 */
                pWinInScrn->prev->next= pWinInScrn->next;
                if(pWinInScrn->next != OP_NULL)
                {
                    pWinInScrn->next->prev = pWinInScrn->prev;
                }

                /*
                 * Second: insert this element at the head of this queue.
                 */
                pScreen->children->prev = pWinInScrn;
                pWinInScrn->next = pScreen->children;
                pScreen->children = pWinInScrn;
                pWinInScrn->prev = OP_NULL;
            }

        }

        
        wt = getWidgetType(pScreen->focusedChild);
        if (wt < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[wt].pWidGetFocus)
        {
            /* disable gadget to refresh screen alone  */
            if( wt != MSF_PAINTBOX )
            {
                pWin->bDrawing = OP_TRUE;
            }
            ret = msfWidFocusFuncs[wt].pWidGetFocus(pWin);
        }
        if( ret != WIDGET_WINDOW_ALREADY_DRAWN )
        {
            widgetRedraw( pScreen, MSF_SCREEN, pScreen->screenHandle, OP_TRUE);
        }
    }
    else if (getWidgetType(pWin->windowHandle) != MSF_DIALOG)
    {
        widgetRedraw( pScreen, MSF_SCREEN, pScreen->screenHandle, OP_TRUE);
    }
    
    return ret;
}


/*==================================================================================================
    FUNCTION: setGadgetInFocus

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int setGadgetInFocus(MsfGadget *pGadget)
{
    MsfWindow *pWin = OP_NULL;
    MsfGadget  *pPreFocusGadget;
    MsfWidgetType preGadgetType;
    MsfWidgetType winType, gadgetType;
    GadgetsInWindow *pGadgetInWin;
    OP_BOOLEAN      bScrolled;
    int ret = HDI_WIDGET_OK;

    if( !pGadget )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if (pGadget)
    {
        pWin = (MsfWindow*)pGadget->parent;
    }
    
    if(!pWin)
    {
        ret = HDI_WIDGET_ERROR_UNEXPECTED;                  
    }
    else if ( pGadget->isFocused == OP_FALSE )
    {
        winType = getWidgetType(pWin->windowHandle);
        pGadgetInWin = pWin->children;
        while(pGadgetInWin && (pGadgetInWin->gadget != pGadget))
        {
            pGadgetInWin = pGadgetInWin->next;
        }
    
        if(pGadgetInWin)
        {
            pPreFocusGadget = OP_NULL;
            
            if (pWin->focusedChild)
            {
                pPreFocusGadget = pWin->focusedChild->gadget;
                preGadgetType = getWidgetType( pPreFocusGadget->gadgetHandle);
                if (preGadgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[preGadgetType].pWidLoseFocus)
                {
                    (void)msfWidFocusFuncs[preGadgetType].pWidLoseFocus(pPreFocusGadget);
                }

                if( (pPreFocusGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
                  /*&&(getWidgetType(pWin->windowHandle ) == MSF_PAINTBOX )*/)
                {
                    if( pPreFocusGadget->cbStateChange )
                    {
                        (void)pPreFocusGadget->cbStateChange(pPreFocusGadget->modId, pPreFocusGadget->gadgetHandle, MsfNotifyLostFocus, OP_NULL);
                    }
                    else
                    {
                        HDIc_widgetNotify(pPreFocusGadget->modId, pPreFocusGadget->gadgetHandle, MsfNotifyLostFocus );
                    }
                }
            }

            pWin->focusedChild = pGadgetInWin;

            bScrolled = OP_FALSE;
            if( winType == MSF_PAINTBOX )
            {
                /*  Adjust the focus position  */
                bScrolled = widgetFocusGadgetAdjustWindow( pGadget, pWin);
            }
            
            /* the window is not scrolled or the window is scrolled but it isn't MsfPaintbox,
              * need to notify the gadget to get focus */
            if(  bScrolled == OP_FALSE || winType != MSF_PAINTBOX )
            {
                gadgetType = getWidgetType(pGadget->gadgetHandle);
                if (IS_GADGET(gadgetType) &&  msfWidFocusFuncs[gadgetType].pWidGetFocus)
                {
                    ret = msfWidFocusFuncs[gadgetType].pWidGetFocus(pGadget);
                }
                
                if( (pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
                  /*&&(getWidgetType(pWin->windowHandle ) == MSF_PAINTBOX )*/)
                {
                    if( pGadget->cbStateChange )
                    {
                        (void)pGadget->cbStateChange( pGadget->modId, pGadget->gadgetHandle, MsfNotifyFocus, OP_NULL );
                    }
                    else
                    {
                        HDIc_widgetNotify(pGadget->modId, pGadget->gadgetHandle, MsfNotifyFocus );
                    }
                }
            }
            if( bScrolled == OP_TRUE ) /*  the window is crolled  */
            {  /* but the window is not MsfPaintbox, so need to redraw the whole window   */
                if ((winType != MSF_PAINTBOX) && (ret != WIDGET_WINDOW_ALREADY_DRAWN ))
                {
                    if(MSF_FORM == winType)
                    {
                        MsfForm * pForm = (MsfForm *)(pWin);
                        if(OP_NULL != pForm->pVScrollBar)
                        {
                           HDIa_widgetBarSetValues(pForm->pVScrollBar->gadgetData.gadgetHandle,pWin->scrollPos.y, pForm->iMaxScrollAreaY);
                        }
                        if(OP_NULL != pForm->pHScrollBar)
                        {
                            HDIa_widgetBarSetValues(pForm->pHScrollBar->gadgetData.gadgetHandle, pWin->scrollPos.x, pForm->iMaxScrollAreaX);
                        }
            
                    }
                    widgetRedraw((void*)pWin, winType, INVALID_HANDLE, OP_TRUE);
                    ret = WIDGET_WINDOW_ALREADY_DRAWN;
                }
            }
            else
            {
                if( pPreFocusGadget )
                {
                    widgetRedraw((void *)pPreFocusGadget, preGadgetType, 0, OP_TRUE );
                }
            }
        }
    }
    return ret;
}






















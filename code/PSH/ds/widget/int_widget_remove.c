/*==================================================================================================

    MODULE NAME : int_widget_remove.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   XXXXX            Cxxxxxx     Initial file creation.
    07/03/2003   Steven Lai       P000729     Fix the bug in removing a gadget or a window
    7/30/2003    Zhuxq            P001026     Modify the editor_deallocate
    07/26/2003   lindawang        P001074     Fix draw image problems.
    04/08/2003   Zhuxq            P001052     Fix some bugs
    08/05/2003   Steven Lai       P001140     Change the method of handling screen focus; fixed some bugs
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    8/11/2003    Chenjs           P001091     Update text input widget.
    08/20/2003   Zhuxq            P001279     solve the problem that when a gadget removed from a window, the window not redraw
    08/14/2003   lindawang        P001254     Fix draw image problem for wap.
    08/20/2003   Steven Lai       P001310     Improve the method of checking whether a widget is in focus
    08/22/2003   Zhuxq            P001290     Modify the MsfAction 
    08/23/2003   Zhuxq            P001347     Fix some bugs of the Input Method, and delete the invalid references
    08/25/2003   Tree Zhan        P001368     when remove gif image, do not forget to free some buffer
    08/29/2003   Zhuxq            P001396     Add scroll-contents feature to string gadget
    09/25/2003   Zhuxq            P001696     Re-implement the dialog
    09/26/2003   linda wang       P001451     separate the gif play and show function.    
    10/17/2003   Zhuxq            C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq            P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq            P001883     Optimize the redraw strategy to reduce the redraw times
    12/12/2003   Zhuxq            P002192     fix bug for wap memory leak
    01/15/2004   Zhuxq            P002352     All editing actions of MsfTextInput turns to MsfEditor
    02/10/2004   chenxiao         p002405     add form scrollbar  in the dolphin project
    02/16/2004   chenxiao         p002474     add bitmap animation function in the dolphin project    
    02/17/2004   MengJiangHong    p002481     add scroll bar to stinggadget dynamically 
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    02/19/2004   Zhuxq            P002504     Add box gadget to widget system
    02/24/2004   chenxiao         p002530     modify WAP & MSG main menu     
    03/01/2004   Zbzhang          P002564     Resolve the bug that when browse the given page    
    03/27/2004   Zhuxq            P002760     Fix some bugs in widget system
    04/02/2004   zhuxq            P002789     Fix UMB bugs
    04/15/2004   zhuxq            P002977     Fix some bugs in UMB for Dolphin
    04/24/2004   zhuxq            P003022     Fix some bugs in UMB
    04/14/2004   Dingjianxin      P002961     delete the conditional compiler
    05/24/2004   zhuxq            P005568     Adjust UI implementation in UMB and fix some bugs
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/09/2004   zhuxq            P006048     forbid Menu to respond successive multiple actions
    07/20/2004   zhuxq            P007108     Add playing GIF animation in some wap page with multi-frame GIF
    08/18/2004   zhuxq            P007793     Trace widget memory issues, and reduce allocation times of MsfString
    08/30/2004   zhuxq            c007998     improve the implementation of predefined string to reduce the times of memory allocation
    
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

#include "OPUS_typedef.h"
#include "gsm_timer.h"

#include "utility_string_func.h"
#include "SP_list.h"
#include "SP_sysutils.h"

#include "Keymap.h"

#include "msf_int.h"
#include "msf_lib.h"

#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "int_widget_custom.h"
#include "int_widget_focus.h"
#include "int_widget_remove.h"
#include "int_widget_txt_input.h"

#include "int_widget_show.h"
extern const OP_UINT16 action_map_array[8];

extern int widgetRedraw( void *pWidget, MsfWidgetType widgetType, OP_UINT32  handle, OP_BOOLEAN   bRefresh);

extern void GIF_Terminate(int handle);
/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static int screen_deallocate(void *ptr);

static int paintbox_deallocate(void *ptr);
static int form_deallocate(void *ptr);
static int dialog_deallocate(void *ptr);
static int menu_deallocate(void *ptr);
static int mainmenu_dellocate(void *ptr);
static int viewer_deallocate(void *ptr);
static int waitingwin_deallocate(void *ptr);
static int editor_deallocate(void *ptr);

static int selectgroup_deallocate(void *ptr);
static int datetime_deallocate(void *ptr);
static int textinput_deallocate(void *ptr);
static int stringgadget_deallocate(void *ptr);
static int imagegadget_deallocate(void *ptr);
static int button_deallocate(void *ptr);
static int bmpanimation_deallocate(void *ptr);
static int box_deallocate(void *ptr);
static int bar_deallocate(void *ptr);

static int image_deallocate(void *ptr);
static int string_deallocate(void *ptr);
static int icon_deallocate(void *ptr);
static int sound_deallocate(void *ptr);
static int style_deallocate(void *ptr);
static int coloranim_deallocate(void *ptr);
static int move_deallocate(void *ptr);
static int rotation_deallocate(void *ptr);

static int action_deallocate(void *ptr);

static int gadget_deallocate(MsfGadget * pGadget);
static int window_deallocate(MsfWindow * pWindow);


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
widget_deallocate_func_t widgetDeallocateFunc[MSF_MAX_WIDGETTYPE] =
{
    screen_deallocate,  /* Screen */
    
    paintbox_deallocate, /* Paintbox */
    form_deallocate, /* Form */
    dialog_deallocate, /* Dialog */
    menu_deallocate, /* Menu */
    mainmenu_dellocate, /* Main menu*/
    viewer_deallocate, /* Viewer */
    waitingwin_deallocate,  /* WaitingWin */
    editor_deallocate, /* Editor */
    
    selectgroup_deallocate, /* selectgroup */
    datetime_deallocate, /* datetime */
    textinput_deallocate, /* textinput */
    stringgadget_deallocate, /* stringgadget */
    imagegadget_deallocate, /* imagegadget */
    button_deallocate,       /* button */
    bmpanimation_deallocate, /* bitmapgadget*/
    box_deallocate,  /*box*/
    bar_deallocate, /* bar */
    
    image_deallocate, /* image */
    string_deallocate, /* string */
    icon_deallocate, /* icon */
    sound_deallocate, /* sound */
    style_deallocate, /* style */
    coloranim_deallocate, /* coloranim */
    move_deallocate, /* move */
    rotation_deallocate, /* rotation */

    action_deallocate,/* action */
    OP_NULL   /* predefined string */
};




/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

widget_release_func_t widgetReleaseFunc[MSF_MAX_WIDGETTYPE] =
{
    screen_release,  /* Screen */
    
    window_release, /* Paintbox */
    window_release, /* Form */
    window_release, /* Dialog */
    window_release, /* Menu */
    window_release, /* MainMenu*/
    window_release, /* Viewer */
    window_release, /* WaitingWin */
    window_release, /* Editor */
    
    gadget_release, /* selectgroup */
    gadget_release, /* datetime */
    gadget_release, /* textinput */
    gadget_release, /* stringgadget */
    gadget_release, /* imagegadget */
    gadget_release, /* button */
    gadget_release, /* bmpanimation */
    gadget_release, /* box */
    gadget_release, /* bar */
    
    image_release, /* image */
    string_release, /* string */
    icon_release, /* icon */
    sound_release, /* sound */
    style_release, /* style */
    coloranim_release, /* coloranim */
    move_release, /* move */
    rotation_release, /* rotation */

    action_release, /* action */
    OP_NULL     /* predefined string */
};

/*ChenJS . 2003.4.10*/

widget_remove_func_t widgetRemoveFunc[MSF_MAX_WIDGETTYPE] =
{
    OP_NULL,                    /* Screen */

    window_remove,    /*Window: paintBox*/
    window_remove,    /* Form */  
    window_remove,    /* Dialog */
    window_remove,    /* Menu */
    window_remove,   /* MainMenu*/
    window_remove,    /* Viewer */
    window_remove,    /* WaitingWin */
    window_remove,    /* Edit  */

    gadget_remove,        /*Gadget: selectgroup*/
    gadget_remove,        /*datetime*/    
    gadget_remove,        /*textinput*/
    gadget_remove,        /*stringgadget*/
    gadget_remove,        /*imagegadget*/
    gadget_remove,        /*button*/
    gadget_remove,        /*bmpanimation */
    gadget_remove,        /*box*/
    gadget_remove,        /*bar*/

    OP_NULL,
    OP_NULL,
    OP_NULL,
    OP_NULL,
    OP_NULL,
    coloranim_remove,   /* ColorAnimation */
    move_remove,      /* Move       */
    rotation_remove,     /* Rotation     */

    action_remove,
    OP_NULL        /* predefined string */
};


/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
static int screen_deallocate(void *ptr)
{
    MsfScreen       *pScreen;
    int             ret=TPI_WIDGET_OK;
    WindowsInScreen *pWindowInScreen, *pCurWindowInScreen;    
    MsfAccessKey    *pAccesskey;
    OP_UINT8       i;

    pScreen = (MsfScreen *)ptr;
    if (pScreen == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    /* 
     * remove all the children 
     */
    pWindowInScreen = pScreen->children;
    while (pWindowInScreen != OP_NULL)
    {
        ret=TPIa_widgetRemove(pScreen->screenHandle, ((MsfWindow *)pWindowInScreen->window)->windowHandle);
        if (ret < TPI_WIDGET_OK)
        {
            pCurWindowInScreen = pWindowInScreen;
            pScreen->children = pWindowInScreen->next;
            WIDGET_FREE(pCurWindowInScreen);
        }        
        pWindowInScreen = pScreen->children;
    }
    
    /*
     * remove all the actions 
     */
    for (i = 0; i < 8; i++)
    {
        if (pScreen->actionlist[i] != OP_NULL)
        {
            action_remove(pScreen, MSF_SCREEN, pScreen->actionlist[i], MSF_ACTION);
            pScreen->actionlist[i] = OP_NULL;
        }
    }

    /* remove all accesskeys */
    pAccesskey = pScreen->accessKey;
    while (pAccesskey != OP_NULL)
    {
        pScreen->accessKey = pAccesskey->next;
        WIDGET_FREE(pAccesskey->accessKeyDefinition);
        WIDGET_FREE(pAccesskey);
        pAccesskey = pScreen->accessKey;
    }
    
    if (isValidHandleOfType(pScreen->style, MSF_STYLE))
    {
        decrease_ref_count(pScreen->style);
    }
    
   /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_SCREEN], pScreen))
    {
        WIDGET_FREE(pScreen);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }

    return ret;
}

static int window_deallocate(MsfWindow *pWindow)
{
    int                    ret=TPI_WIDGET_OK;
    GadgetsInWindow        *pGadgetInWindow, *pCurGadgetInWindow;    
    MsfAccessKey           *pAccesskey;
    OP_UINT8              i;

    if (pWindow == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    /* remove all the children */
    pGadgetInWindow = pWindow->children;
    while (pGadgetInWindow != OP_NULL)
    {
        ret = TPIa_widgetRemove(pWindow->windowHandle, ((MsfGadget *)pGadgetInWindow->gadget)->gadgetHandle);
        if (ret < TPI_WIDGET_OK)
        {
            pCurGadgetInWindow = pGadgetInWindow;
            pWindow->children = pGadgetInWindow->next;
            WIDGET_FREE(pCurGadgetInWindow);
        }
        pGadgetInWindow = pWindow->children;
    }
    
    /* remove all the actions */
    for (i = 0; i < 8; i++)
    {
        if (pWindow->actionlist[i] != OP_NULL)
        {
            action_remove(pWindow, getWidgetType(pWindow->windowHandle), pWindow->actionlist[i], MSF_ACTION);
            pWindow->actionlist[i] = OP_NULL;
        }
    }
    
    /* remove all accesskeys */
    pAccesskey = pWindow->accessKey;
    while (pAccesskey != OP_NULL)
    {
        pWindow->accessKey = pAccesskey->next;
        WIDGET_FREE(pAccesskey->accessKeyDefinition);
        WIDGET_FREE(pAccesskey);
        pAccesskey = pWindow->accessKey;
    }
    
    if (pWindow->style != INVALID_HANDLE)
    {
        decrease_ref_count(pWindow->style);
    }
    
    if (pWindow->ticker != INVALID_HANDLE)
    {
        decrease_ref_count(pWindow->ticker);
    }
    if (pWindow->title != INVALID_HANDLE)
    {
        decrease_ref_count(pWindow->title);
    }

    return ret;
}

static int paintbox_deallocate(void *ptr)
{
    MsfWindow      *pWindow;
    int            ret = TPI_WIDGET_OK;
  
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pWindow = (MsfWindow *)ptr;
        if (ISVALIDHANDLE(((MsfPaintbox *)ptr)->brush))
        {
            TPIa_widgetRelease(((MsfPaintbox *)ptr)->brush);
        }
        window_deallocate(pWindow);
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_PAINTBOX], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return ret;
}

static int form_deallocate(void *ptr)
{
    MsfWindow      *pWindow;
    int            ret = TPI_WIDGET_OK;
  
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pWindow = (MsfWindow *)ptr;
        window_deallocate(pWindow);
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_FORM], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return ret;
}


static int dialog_deallocate(void *ptr)
{
    MsfDialog    *pDialog;
    MsfWindow    *pWindow;
    int          ret = TPI_WIDGET_OK;
    
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pDialog = (MsfDialog *)ptr;
        pWindow = (MsfWindow *)pDialog;

        if (ISVALIDHANDLE(pDialog->dialogText))
        {
            decrease_ref_count(pDialog->dialogText);
        }

        if( IS_WIDGET_TIMER_ID( pDialog->iTimerId ))
        {
            TPIa_widgetStopTimer( pWindow->windowHandle, pDialog->iTimerId );
        }
        
        window_deallocate(pWindow);
            
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_DIALOG], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
        
    return ret;
}


static int menu_deallocate(void *ptr)
{
    MsfWindow      *pWindow;
    int            ret = TPI_WIDGET_OK;
  
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pWindow = (MsfWindow *)ptr;
        window_deallocate(pWindow);
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_MENU], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return ret;
}

static int mainmenu_dellocate(void *ptr)
{
    MsfWindow      *pWindow;
    int            ret = TPI_WIDGET_OK;
  
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pWindow = (MsfWindow *)ptr;
        window_deallocate(pWindow);
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_MAINMENU], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return ret;
}

static int viewer_deallocate(void *ptr)
{
    MsfWindow      *pWindow;
    int            ret = TPI_WIDGET_OK;
  
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pWindow = (MsfWindow *)ptr;
        window_deallocate(pWindow);
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_VIEWER], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return ret;
}

static int waitingwin_deallocate(void *ptr)
{
    MsfWindow      *pWindow;
    MsfWaitingWin  *pWaitingWin;
    int            ret = TPI_WIDGET_OK;
  
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pWaitingWin = (MsfWaitingWin*)ptr;
        if( ISVALIDHANDLE(pWaitingWin->hBar ))
        {
            TPIa_widgetRelease(pWaitingWin->hBar);
        }
        if( ISVALIDHANDLE(pWaitingWin->hPrompt))
        {
            TPIa_widgetRelease(pWaitingWin->hPrompt);
        }
        if( ISVALIDHANDLE(pWaitingWin->hBmpAni))
        {
            TPIa_widgetRelease(pWaitingWin->hBmpAni);
        }
        
        pWindow = (MsfWindow *)ptr;
        window_deallocate(pWindow);
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_WAITINGWIN], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return ret;
}

static int editor_deallocate(void *ptr)
{
    MsfWindow      *pWindow;
    int            ret = TPI_WIDGET_OK;
  
    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pWindow = (MsfWindow *)ptr;
        window_deallocate(pWindow);
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_EDITOR], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return ret;
}

static int gadget_deallocate(MsfGadget *pGadget)
{
    MsfAccessKey           *pAccesskey;
    int ret = TPI_WIDGET_OK;

    if (pGadget == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if (pGadget->style != INVALID_HANDLE)
    {
        decrease_ref_count(pGadget->style);
    }
    
    if (pGadget->title != INVALID_HANDLE)
    {
        decrease_ref_count(pGadget->title);
    }

    /* release the related horizontal Scrollbar  */
    if( ISVALIDHANDLE(pGadget->hHScrollBar ))
    {
        TPIa_widgetRelease(pGadget->hHScrollBar);
    }
    /* release the related vertical Scrollbar  */
    if( ISVALIDHANDLE(pGadget->hVScrollBar ))
    {
        TPIa_widgetRelease(pGadget->hVScrollBar);
    }
    
    /* remove all accesskeys */
    pAccesskey = pGadget->accessKey;
    while (pAccesskey != OP_NULL)
    {
        pGadget->accessKey = pAccesskey->next;
        WIDGET_FREE(pAccesskey->accessKeyDefinition);
        WIDGET_FREE(pAccesskey);
        pAccesskey = pGadget->accessKey;
    }
    return ret;
}

static int selectgroup_deallocate(void *ptr)
{
    MsfSelectgroup *pSelectgroup;
    MsfGadget      *pGadget;
    MsfChoice      *pChoice = OP_NULL;
    choiceElement  *pChoiceElement = OP_NULL;
    int            ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pSelectgroup = (MsfSelectgroup *)ptr;
    pChoice = &pSelectgroup->choice;
    pChoiceElement = pChoice->firstElement;
    
    while (pChoiceElement != OP_NULL)
    {
        if (pChoiceElement->string1 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->string1);
        }
        if (pChoiceElement->string2 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->string2);
        }
        if (pChoiceElement->image1 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->image1);
        }
        if (pChoiceElement->image2 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->image2);
        }
        pChoice->firstElement = pChoiceElement->next;
        WIDGET_FREE(pChoiceElement);
        pChoiceElement = pChoice->firstElement;
    }
    
    pGadget = (MsfGadget *)pSelectgroup;
    gadget_deallocate(pGadget);
        
    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_SELECTGROUP], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    return ret;
}

static int datetime_deallocate(void *ptr)
{
    MsfDateTimeStruct *pDateTime;
    MsfGadget         *pGadget;
    int               ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
    	return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pDateTime = (MsfDateTimeStruct *)ptr;
    pGadget = (MsfGadget *)pDateTime;
    gadget_deallocate(pGadget);
    
    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_DATETIME], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
        
    return ret;
}

static int textinput_deallocate(void *ptr)
{
    MsfTextInput   *pTextInput;
    MsfGadget      *pGadget;
    int            ret = TPI_WIDGET_ERROR_INVALID_PARAM;

    if (ptr == OP_NULL)
    {
    	return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pTextInput = (MsfTextInput *)ptr;
    if (pTextInput->input.inputString != INVALID_HANDLE)
    {
        decrease_ref_count(pTextInput->input.inputString);
    }
    if (pTextInput->input.initString != INVALID_HANDLE)
    {
        decrease_ref_count(pTextInput->input.initString);
    }
    if (pTextInput->input.txtInputBufMng.hStyle != INVALID_HANDLE)
    {
        decrease_ref_count(pTextInput->input.txtInputBufMng.hStyle);
    }
    
    TBM_Destroy(&(pTextInput->input.txtInputBufMng) );
        
    pGadget = (MsfGadget *)pTextInput;
    gadget_deallocate(pGadget);
    
    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_TEXTINPUT], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
        
    return ret;
}

static int stringgadget_deallocate(void *ptr)
{
    MsfStringGadget *pStringGadget;
    MsfGadget       *pGadget;
    int             ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pStringGadget = (MsfStringGadget *)ptr;

    if( (pStringGadget->pPageStarIndexList != pStringGadget->pageStarIndexListBuf )
        && pStringGadget->pPageStarIndexList )
    {
        WIDGET_FREE(pStringGadget->pPageStarIndexList);
    }
    
    if (pStringGadget->text != INVALID_HANDLE)
    {
        decrease_ref_count(pStringGadget->text);
    }

    pGadget = (MsfGadget *)pStringGadget;
    gadget_deallocate(pGadget);

    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_STRINGGADGET], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    return ret;
}

static int imagegadget_deallocate(void *ptr)
{
    MsfImageGadget *pImageGadget;
    MsfGadget      *pGadget;
    int            ret = TPI_WIDGET_OK;

    WIDGET_TRACE("----imagegadget_deallocate-----!\n" );

    if (ptr == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pImageGadget = (MsfImageGadget *)ptr;
    if (pImageGadget->image != INVALID_HANDLE)
    {
        decrease_ref_count(pImageGadget->image);
    }

    pGadget = (MsfGadget *)pImageGadget;
    gadget_deallocate(pGadget);

    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_IMAGEGADGET], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    return ret;
}

static int button_deallocate(void *ptr)
{
    MsfGadget  *pGadget;
    int             iRet = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pGadget = (MsfGadget *)ptr;

    gadget_deallocate(pGadget);

    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_BUTTON], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        iRet = TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    return iRet;
}

static int box_deallocate(void *ptr)
{
    MsfSelectgroup *pSelectgroup;
    MsfGadget      *pGadget;
    MsfChoice      *pChoice = OP_NULL;
    choiceElement  *pChoiceElement = OP_NULL;
    int            ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pSelectgroup = (MsfSelectgroup *)ptr;
    pChoice = &pSelectgroup->choice;
    pChoiceElement = pChoice->firstElement;
    
    while (pChoiceElement != OP_NULL)
    {
        if (pChoiceElement->string1 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->string1);
        }
        if (pChoiceElement->string2 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->string2);
        }
        if (pChoiceElement->image1 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->image1);
        }
        if (pChoiceElement->image2 != INVALID_HANDLE)
        {
            decrease_ref_count(pChoiceElement->image2);
        }
        pChoice->firstElement = pChoiceElement->next;
        WIDGET_FREE(pChoiceElement);
        pChoiceElement = pChoice->firstElement;
    }
    
    pGadget = (MsfGadget *)pSelectgroup;
    gadget_deallocate(pGadget);
        
    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_BOX], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    return ret;

}

static int bar_deallocate(void *ptr)
{
    MsfBar         *pBar;
    MsfGadget      *pGadget;
    int            ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
    	return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pBar = (MsfBar *)ptr;
    pGadget = (MsfGadget *)pBar;
    gadget_deallocate(pGadget);
    
    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_BAR], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
        
    return ret;
}

static int image_deallocate(void *ptr)
{
    MsfImage *pImage;
    int      ret = TPI_WIDGET_OK;
    
    if (ptr == OP_NULL)
    {
        return  TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pImage = (MsfImage *)ptr;
#ifdef   WIDGET_DEBUG
        {
            op_printf( "\n[IMAGE] FREE:ptr=0x%08p ", (void*)pImage->handle);
        }
#endif    

        if (pImage->style != INVALID_HANDLE)
        {
            decrease_ref_count(pImage->style);
        }

        /* we have not implement brush yet */
        if (pImage->brush != INVALID_HANDLE)
        {
            decrease_ref_count(pImage->brush);
        }
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_IMAGE], ptr))
        {
            if(pImage->predefinedId == 0)
            {
                if (pImage->data)
                {
                    WIDGET_FREE(pImage->data);
                }

                if(pImage->format == MsfGif)
                {
                    GIF_Terminate(pImage->handle);
                    if(pImage->pGifBmpBuf)
                    {
                        WIDGET_FREE(pImage->pGifBmpBuf);
                    }
                    if(pImage->colorPallet)
                    {
                        WIDGET_FREE(pImage->colorPallet);
                    }
                }
            }
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }

        WIDGET_FREE(pImage);
        WIDGET_TRACE("----IMAGE release---WIDGET_FREE(pImage)----!\n" );
    }
    
    return ret;
}

static int string_deallocate(void *ptr)
{
    MsfString *pString;
    int       ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pString = (MsfString *)ptr;
#ifdef   WIDGET_DEBUG
        {
            op_printf( "\n[STRING] FREE:ptr=0x%08p ", (void*)pString->handle);
        }
#endif    
        if (pString->style != INVALID_HANDLE)
        {
            decrease_ref_count(pString->style);
        }

        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_STRING], pString))
        {
            /*
             * Only the nomal strings have allocated a data block.
             * If it is a predefined string, need not to free the string data 
             */
            WIDGET_FREE(pString);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    return ret;
}

static int sound_deallocate(void *ptr)
{
    MsfSound *pSound;
    int      ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pSound = (MsfSound *)ptr;
#ifdef   WIDGET_DEBUG
        {
            op_printf( "\n[SOUND] FREE:ptr=0x%08p ", (void*)pSound->handle);
        }
#endif    
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_SOUND], ptr))
        {
            if (pSound->data)
            {
                WIDGET_FREE(pSound->data);
            }
            WIDGET_FREE(pSound);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    return ret;
}

static int style_deallocate(void *ptr)
{
    int      ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_STYLE], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    return ret;
}

static int icon_deallocate(void *ptr)
{
    MsfIcon  *pIcon;
    int       ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pIcon = (MsfIcon *)ptr;

        if (pIcon->image != INVALID_HANDLE)
        {
            decrease_ref_count(pIcon->image);
        }

        if ( pIcon->style != INVALID_HANDLE)
        {
            decrease_ref_count(pIcon->style);
        }

        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_ICON], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    return ret;
}

static int coloranim_deallocate(void *ptr)
{
    return 0;
}

static int move_deallocate(void *ptr)
{
    return 0;
}

static int rotation_deallocate(void *ptr)
{
    return 0;
}

static int action_deallocate(void *ptr)
{
    MsfAction *pAction;
    int       ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pAction = (MsfAction *)ptr;
#ifdef   WIDGET_DEBUG
        {
            op_printf( "\n[ACTION] FREE:ptr=0x%08p ", (void*)pAction->actionHandle);
        }
#endif    
        if (pAction->label != INVALID_HANDLE)
        {
            decrease_ref_count(pAction->label);
        }
        
        /* remove the node from the corresponding queue */
        if (SP_list_delete_by_item(&widgetQueue[MSF_ACTION], ptr))
        {
            WIDGET_FREE(ptr);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    return ret;
}

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

int screen_release(OP_UINT32 handle)
{
    int             ret=TPI_WIDGET_OK;
    MsfScreen       *pScreen, *pNextScreen;
    MsfWidgetType   widgetType;
    ScreenInDisplay *pScreenInDisplay, *pPreScreen;
    
    pScreen = (MsfScreen *)seekWidget(handle,&widgetType);
    
    if ((pScreen == OP_NULL) || (MSF_SCREEN != widgetType))
    {
        ret = TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    else
    {
        if ( pDisplay->screenList->screen == pScreen)
        {
            pScreenInDisplay = pDisplay->screenList;
            pDisplay->screenList = pScreenInDisplay->next;
            WIDGET_FREE(pScreenInDisplay);
            if (pDisplay->focusedScreen == pScreen)
            {
                if (pDisplay->screenList)
                {
                    pDisplay->focusedScreen = OP_NULL;
                    pNextScreen = pDisplay->screenList->screen;
                    if(widgetAppIsCurrentFocusApp(pNextScreen->modId))
                    {
                        TPIa_widgetSetInFocus(pNextScreen->screenHandle, 1);
                    }
                    else
                    {
                        pDisplay->hScreenSave = pNextScreen->screenHandle;
                    }
                }
                else
                {
                    pDisplay->focusedScreen = OP_NULL;
                }
            }
            else if( pDisplay->hScreenSave == handle )
            {
                pDisplay->hScreenSave = INVALID_HANDLE;
                if(pDisplay->screenList!= OP_NULL
                    && pDisplay->screenList->screen != OP_NULL)
                {
                    pDisplay->hScreenSave = pDisplay->screenList->screen->screenHandle;
                }
            }
        }
        else
        {
            pPreScreen = pDisplay->screenList;
            pScreenInDisplay = OP_NULL;
            while (pPreScreen)
            {
                pScreenInDisplay = pPreScreen->next;
                if (pScreenInDisplay && pScreenInDisplay->screen == pScreen)
                {
                    break;
                }
                pPreScreen = pPreScreen->next;
            }
            if (pScreenInDisplay)
            {
                if( pPreScreen )
                {
                    pPreScreen->next = pScreenInDisplay->next;
                }
                WIDGET_FREE(pScreenInDisplay);
            }
        }
        ret = screen_deallocate(pScreen);
    }
    return ret;
}


int window_release(OP_UINT32 handle)
{
    int                    ret=TPI_WIDGET_OK;
    void                   *ptr;
    MsfWindow              *pWindow;
    MsfWidgetType          widgetType;

    ptr = seekWidget(handle, &widgetType);
    if (ptr == OP_NULL || widgetType <= MSF_SCREEN || widgetType > MSF_WINDOW_END)
    {
        ret = TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    else
    {
        pWindow = (MsfWindow *)ptr;
        /* to check whether this window has been released before */
        if (pWindow->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pWindow->isReleased = OP_TRUE;
            pWindow->referenceCounter--;
            if (pWindow->referenceCounter < 1) /* this window is of no use any more */
            {
                ret = widgetDeallocateFunc[widgetType](ptr);
            }
        }
        
    }

    return ret;
}

int gadget_release(OP_UINT32 handle)
{
    void                   *ptr;
    MsfGadget              *pGadget;
    int                    ret = TPI_WIDGET_OK;
    MsfWidgetType          widgetType;

    ptr = seekWidget(handle, &widgetType);
    if (ptr == OP_NULL || !IS_GADGET(widgetType))
    {
        ret = TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    else
    {
        pGadget = (MsfGadget *)ptr;
        /* to check whether this gadget has been released before */
        if (pGadget->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pGadget->isReleased = OP_TRUE;
            pGadget->referenceCounter--;
            if (pGadget->referenceCounter < 1)
            {
                ret = widgetDeallocateFunc[widgetType](ptr);
            }
        }
    }

    return ret;
}

int image_release(OP_UINT32 handle)
{
    MsfImage               *pImage;
    int                    ret = TPI_WIDGET_OK;
    MsfWidgetType          widgetType;
    
    pImage = (MsfImage *)seekWidget(handle, &widgetType);
  
    if (( pImage == OP_NULL)||(widgetType != MSF_IMAGE))
    {
        ret = TPI_WIDGET_ERROR_NOT_FOUND;
    }
    else
    {
        /* to check whether this widget has been released before */
        if (pImage->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pImage->isReleased = OP_TRUE;
            pImage->referenceCounter--;
            if (pImage->referenceCounter < 1)
            {
                ret = image_deallocate(pImage);
            }
        }
    }

    return ret;
}

int string_release(OP_UINT32 handle)
{
    MsfString             *pString;
    int                   ret = TPI_WIDGET_OK;
    MsfWidgetType         widgetType;
    
    pString = (MsfString *)seekWidget(handle, &widgetType);
  
    if ((pString == OP_NULL)||(widgetType != MSF_STRING))
    {
        ret = TPI_WIDGET_ERROR_NOT_FOUND;
    }
    else
    {
        /* to check whether this widget has been released before */
        if (pString->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pString->isReleased = OP_TRUE;
            pString->referenceCounter--;
            if (pString->referenceCounter < 1)
            {
                ret = string_deallocate(pString);
            }
        }
    }

    return ret;
}

int icon_release(OP_UINT32 handle)
{
    MsfIcon               *pIcon;
    int                   ret = TPI_WIDGET_OK;
    MsfWidgetType         widgetType;
    
    pIcon = (MsfIcon *)seekWidget(handle, &widgetType);
  
    if ((pIcon == OP_NULL)||(widgetType != MSF_ICON))
    {
        ret = TPI_WIDGET_ERROR_NOT_FOUND;
    }
    else
    {
        /* to check whether this widget has been released before */
        if (pIcon->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pIcon->isReleased = OP_TRUE;
            pIcon->referenceCounter--;
            if (pIcon->referenceCounter < 1)
            {
                ret = icon_deallocate(pIcon);
            }
        }
    }

    return ret;
}

int sound_release(OP_UINT32 handle)
{
    MsfSound       *pSound;
    int            ret = TPI_WIDGET_OK;
    MsfWidgetType  widgetType;
    
    pSound = (MsfSound *)seekWidget(handle, &widgetType);
  
    if ((pSound == OP_NULL)||(widgetType != MSF_SOUND))
    {
        ret = TPI_WIDGET_ERROR_NOT_FOUND;
    }
    else
    {
        /* to check whether this widget has been released before */
        if (pSound->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pSound->isReleased = OP_TRUE;
            pSound->referenceCounter--;
            if (pSound->referenceCounter < 1)
            {
                ret = sound_deallocate(pSound);
            }
        }
    }

    return ret;
}

int style_release(OP_UINT32 handle)
{
    MsfStyle              *pStyle;
    int                   ret = TPI_WIDGET_OK;
    MsfWidgetType         widgetType;
    
    pStyle = (MsfStyle *)seekWidget(handle, &widgetType);
  
    if ((pStyle == OP_NULL)||(widgetType != MSF_STYLE))
    {
        ret = TPI_WIDGET_ERROR_NOT_FOUND;
    }
    else
    {
        /* to check whether this widget has been released before */
        if (pStyle->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pStyle->isReleased = OP_TRUE;
            pStyle->referenceCounter--;
            if (pStyle->referenceCounter < 1)
            {
                ret = style_deallocate(pStyle);
            }
        }
    }

    return ret;
}

int coloranim_release(OP_UINT32 handle)
{
    return 0;
}

int move_release(OP_UINT32 handle)
{
    return 0;
}


int rotation_release(OP_UINT32 handle)
{
    return 0;
}

int action_release(OP_UINT32 handle)
{
    MsfAction       *pAction;
    int            ret = TPI_WIDGET_OK;
    MsfWidgetType  widgetType;
    
    pAction = (MsfAction *)seekWidget(handle, &widgetType);
  
    if ((pAction == OP_NULL)||(widgetType != MSF_ACTION))
    {
        ret = TPI_WIDGET_ERROR_NOT_FOUND;
    }
    else
    {
        /* to check whether this widget has been released before */
        if (pAction->isReleased == OP_TRUE)
        {
            ret = TPI_WIDGET_ERROR_IS_RELEASED;
        }
        else
        {
            pAction->isReleased = OP_TRUE;
            pAction->referenceCounter--;
            if (pAction->referenceCounter < 1)
            {
                ret = action_deallocate(pAction);
            }
        }
    }

    return ret;
}

int window_remove(  void *pParWidget,
                         MsfWidgetType iParWidgetType, 
                         void *pChldWidget,
                         MsfWidgetType iChldWidgetType)
{
    int ret = TPI_WIDGET_OK;
    MsfWindow         *pWindowWidget;
    MsfScreen          *pScreenWidget;
    WindowsInScreen   *pWindowsInScreen;
    WindowsInScreen   *pPreNode;
    WindowsInScreen   *pNextNode;
    MsfWindowHandle nextFocusedWin = INVALID_HANDLE;
    MsfWindowHandle hWin;

    if(IS_SCREEN(iParWidgetType) && 
       IS_WINDOW(iChldWidgetType) &&
       pParWidget != OP_NULL &&
       pChldWidget != OP_NULL)
    {
        pScreenWidget = (MsfScreen*)pParWidget;
        pWindowWidget = (MsfWindow*)pChldWidget;

        hWin = pWindowWidget->windowHandle;
        pWindowsInScreen = pScreenWidget->children;
        while(pWindowsInScreen)
        {
            if(pWindowsInScreen->window == pWindowWidget)
            {
                break;  /* skip out this loop because the window to be deleted has been found */
            }
            pWindowsInScreen = pWindowsInScreen->next;
        }

        if(pWindowsInScreen)
        {
            pPreNode = pWindowsInScreen->prev;
            pNextNode = pWindowsInScreen->next;
            
            if (pScreenWidget->focusedChild == pWindowWidget->windowHandle)
            {
                if (iChldWidgetType == MSF_DIALOG && ((MsfDialog *)pChldWidget)->nextWindow != INVALID_HANDLE)
                {
                    nextFocusedWin = ((MsfDialog *)pChldWidget)->nextWindow;
                }
                else if (pNextNode)
                {
                    nextFocusedWin = ((MsfWindow *)pNextNode->window)->windowHandle;
                }
                
                if (msfWidFocusFuncs[iChldWidgetType].pWidLoseFocus)
                {
                    (void)msfWidFocusFuncs[iChldWidgetType].pWidLoseFocus(pWindowsInScreen->window);
                }
                /* It is a focused window. Remove the focus here, so when remove the gadgets from it, there will be no screen refresh */
                pScreenWidget->focusedChild = INVALID_HANDLE;
            }
            
            /* delete the node in the windowsInScreen list */
            if ( pPreNode )
            {
                pPreNode->next = pNextNode;
            }
            else
            {
                pScreenWidget->children = pNextNode;
            }
            if ( pNextNode )
            {
                pNextNode->prev= pPreNode;
            }
            WIDGET_FREE(pWindowsInScreen);
            if( iChldWidgetType == MSF_PAINTBOX )
            {
                clearBackupDisplay();
            }

            pWindowWidget = (MsfWindow*)seekWidget( hWin, OP_NULL );
            if( pWindowWidget )
            {
                pWindowWidget->referenceCounter--;
                pWindowWidget->parent = OP_NULL;
                if(pWindowWidget->referenceCounter == 0)
                {
                    (void)widgetDeallocateFunc[iChldWidgetType](pWindowWidget);
                }

                /* set the next focused window */
                if (nextFocusedWin != INVALID_HANDLE) /* this window is current focused window and there is another window */
                {
                    TPIa_widgetSetInFocus(nextFocusedWin, 1);
                }
            }

        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}

int gadget_remove( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType)
{
    int ret = TPI_WIDGET_OK;
    GadgetsInWindow   *pGadgetInWindow;
    GadgetsInWindow   *pPreNode;
    GadgetsInWindow   *pNextNode;
    MsfGadget *pGadgetWidget;
    MsfWindow *pWindowWidget;

    if(IS_WINDOW(iParWidgetType) && 
       IS_GADGET(iChldWidgetType) &&
       pParWidget != OP_NULL &&
       pChldWidget != OP_NULL)
    {
        pWindowWidget = (MsfWindow*)pParWidget;
        pGadgetWidget = (MsfGadget*)pChldWidget;
        pGadgetInWindow = OP_NULL;
        
        pGadgetInWindow = pWindowWidget->children;
    
        while(pGadgetInWindow)
        {
            if(pGadgetInWindow->gadget == pGadgetWidget)
            {
                break;
            }
            pGadgetInWindow = pGadgetInWindow->next;
        }

        if(pGadgetInWindow)
        {
            pPreNode = pGadgetInWindow->prev;
            pNextNode = pGadgetInWindow->next;
            if(pPreNode == OP_NULL)        /* First child*/
            {
                pWindowWidget->children = pNextNode;
                if( pNextNode )
                {
                    pNextNode->prev = OP_NULL;
                }
            }
            else
            {
                pPreNode->next = pNextNode;
                if (pNextNode)
                {
                    pNextNode->prev = pPreNode;
                }
            }

            pGadgetWidget->parent = OP_NULL;
            
            if (pWindowWidget->focusedChild == pGadgetInWindow)
            {
                GadgetsInWindow *newFocusedGadget;
                MsfWidgetType newFocusGadType;
                
                /* gadet will lose focus if it is currently in focus */
                if (msfWidFocusFuncs[iChldWidgetType].pWidLoseFocus)
                {
                    (void)msfWidFocusFuncs[iChldWidgetType].pWidLoseFocus(pChldWidget);
                }

                if( iParWidgetType == MSF_PAINTBOX )
                {
                    pWindowWidget->focusedChild = OP_NULL;
                }
                else
                {
                    /* 
                      * set a new focused gadget. 
                      * only a gadget with MSF_GADGET_PROPERTY_FOCUS can be set in focus 
                      */
                    newFocusedGadget = pNextNode;
                    while (newFocusedGadget)
                    {
                        if (newFocusedGadget->gadget != OP_NULL
                            && newFocusedGadget->gadget->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                        {
                            break;
                        }
                        newFocusedGadget = newFocusedGadget->next;
                    }

                    if (newFocusedGadget == OP_NULL) /* have not found a gadget can be set in focus yet */
                    {
                        newFocusedGadget = pPreNode;
                        while (newFocusedGadget)
                        {
                            if (newFocusedGadget->gadget != OP_NULL
                                &&newFocusedGadget->gadget->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                            {
                                break;
                            }
                            newFocusedGadget = newFocusedGadget->prev;
                        }
                    }
                    
                    pWindowWidget->focusedChild = newFocusedGadget;

                    if (newFocusedGadget && newFocusedGadget->gadget != OP_NULL )
                    {
                        newFocusGadType = getWidgetType(newFocusedGadget->gadget->gadgetHandle);
                        if (msfWidFocusFuncs[newFocusGadType].pWidGetFocus)
                        {
                            (void)msfWidFocusFuncs[newFocusGadType].pWidGetFocus(newFocusedGadget->gadget);
                        }
                    }
                }

            }

            /* if imagegadget, stop timer  */
            if( MSF_IMAGEGADGET == iChldWidgetType )
            {
                MsfImageGadget   *pImageGadget = (MsfImageGadget*)pGadgetWidget;
                if( pImageGadget->iTimerId != WIDGET_TIMER_INVALID_ID )
                {
                    TPIa_widgetStopTimer( pGadgetWidget->gadgetHandle, pImageGadget->iTimerId);
                    pImageGadget->iTimerId = WIDGET_TIMER_INVALID_ID;
                }
            }
            
            /*  remove the gadget's related scrollbar  */
            if( ISVALIDHANDLE(pGadgetWidget->hHScrollBar ))
            {
                TPIa_widgetRemove(pWindowWidget->windowHandle, pGadgetWidget->hHScrollBar);
            }
            if( ISVALIDHANDLE(pGadgetWidget->hVScrollBar ))
            {
                TPIa_widgetRemove(pWindowWidget->windowHandle, pGadgetWidget->hVScrollBar);
            }
            
            WIDGET_FREE(pGadgetInWindow);
            pGadgetWidget->referenceCounter--;
            if(pGadgetWidget->referenceCounter == 0)
            {
                (void)widgetDeallocateFunc[iChldWidgetType](pGadgetWidget);
            }
            if(MSF_PAINTBOX != iParWidgetType)
            {
                if( iParWidgetType == MSF_FORM )
                {
                    MsfForm *pForm;
                    pForm = (MsfForm *)pParWidget;     
                   widgetFormAdjustScrollbar(pForm);
                }
                widgetRedraw((void*)pParWidget, iParWidgetType, pWindowWidget->windowHandle, OP_TRUE );           
            }
        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}

int action_remove( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType)
{
    int ret = TPI_WIDGET_OK;
    MsfScreen *pScreen;
    MsfWindow *pWindow;
    MsfAction *pActionWidget;
    MsfAction **pActionlist = OP_NULL;

    pActionWidget = (MsfAction*)pChldWidget;

    if (IS_SCREEN(iParWidgetType))
    {
        pScreen = (MsfScreen*)pParWidget;
        pActionlist = &pScreen->actionlist[pActionWidget->actionType];
    }
    else if (IS_WINDOW(iParWidgetType))
    {
        pWindow = (MsfWindow *)pParWidget;
        pActionlist = &(pWindow)->actionlist[pActionWidget->actionType];
    }
    else
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if (*pActionlist == pActionWidget)
    {
        switch( action_map_array[ pActionWidget->actionType] )
        {
            case KEY_SOFT_LEFT:
            {
                if (iParWidgetType == MSF_SCREEN)
                {
                    pScreen->softKeyList[0] = OP_NULL;
                }
                else
                {
                    pWindow->softKeyList[0] = OP_NULL;
                }
                
                break;
            }
            case KEY_OK:
            {
                if (iParWidgetType == MSF_SCREEN)
                {
                    pScreen->softKeyList[1] = OP_NULL;
                }
                else
                {
                    pWindow->softKeyList[1] = OP_NULL;
                }
                
                break;
            }
            case KEY_SOFT_RIGHT:
            {
                if (iParWidgetType == MSF_SCREEN)
                {
                    pScreen->softKeyList[2] = OP_NULL;
                }
                else
                {
                    pWindow->softKeyList[2] = OP_NULL;
                }
                
                break;
            }
            default:
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
                break;
            }        
        } 
        
        *pActionlist = OP_NULL;
        pActionWidget->referenceCounter--;
        if (pActionWidget->referenceCounter < 1)
        {
            (void)widgetDeallocateFunc[iChldWidgetType](pChldWidget);
        }

        if (iParWidgetType == MSF_SCREEN)
        {
            if( pScreen == seekFocusedScreen())
            {
                if( ISVALIDHANDLE( pScreen->focusedChild ) )
                {
                    pWindow = (MsfWindow*)pScreen->children->window;
                   return widgetWindowShowSoftKey( pWindow, OP_TRUE );
                }
                else
                {
                    return widgetScreenShowSoftKey( pScreen, OP_TRUE );
                }
            }
        }
        else
        {
            if (pWindow->isFocused)
            {
                return widgetWindowShowSoftKey( pWindow, OP_TRUE );
            }
        }
        
        pActionWidget->ppSoftKey = OP_NULL;
    }
    else
    {
        ret = TPI_WIDGET_ERROR_NOT_FOUND;
    }
    
    return ret;

}
int coloranim_remove ( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType)
{
    return 0;
}
                        
int move_remove ( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType)
{
    return 0;
}
                        
int rotation_remove( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType)
{
    return 0;
}


int decrease_ref_count(OP_UINT32 handle)
{
    void                   *ptr;
    MsfWidgetType          tmpWidgetType;
    MsfImage               *pImage;
    MsfString              *pString;
    MsfIcon                *pIcon;
    MsfSound               *pSound;
    MsfStyle               *pStyle;
    MsfAction              *pAction;
    int                    ret = TPI_WIDGET_OK;

    ptr = seekWidget(handle, &tmpWidgetType);

    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        switch (tmpWidgetType)
        {
            case MSF_IMAGE:
                pImage = (MsfImage *)ptr;
                pImage->referenceCounter--;
                if (pImage->referenceCounter == 0)
                {
                    ret = image_deallocate(pImage);
                }
            break;
            
            case MSF_STRING:
                pString = (MsfString *)ptr;
                pString->referenceCounter--;
                if (pString->referenceCounter == 0)
                {
                    ret = string_deallocate(pString);
                }
            break;
            
            case MSF_ICON:
                pIcon = (MsfIcon *)ptr;
                pIcon->referenceCounter--;
                if (pIcon->referenceCounter == 0)
                {
                    ret = icon_deallocate(pIcon);
                }
            break;
            
            case MSF_SOUND:
                pSound = (MsfSound *)ptr;
                pSound->referenceCounter--;
                if (pSound->referenceCounter == 0)
                {
                    ret = sound_deallocate(pSound);
                }
            break;
            
            case MSF_STYLE:
                pStyle = (MsfStyle *)ptr;
                pStyle->referenceCounter--;
                if (pStyle->referenceCounter == 0)
                {
                    ret = style_deallocate(pStyle);
                }
            break;
            
            case MSF_ACTION:
                pAction = (MsfAction *)ptr;
                pAction->referenceCounter--;
                if (pAction->referenceCounter == 0)
                {
                    ret = action_deallocate(pAction);
                }
            break;

            case MSF_STRING_RES:
                break;
                
            default:
                ret = TPI_WIDGET_ERROR_INVALID_PARAM;
            break;
        }
    }

    
    return ret;
}

int increase_ref_count(OP_UINT32 handle)
{
    void                   *ptr;
    MsfWidgetType          tmpWidgetType;
    MsfImage               *pImage;
    MsfString              *pString;
    MsfIcon                *pIcon;
    MsfSound               *pSound;
    MsfStyle               *pStyle;
    MsfAction              *pAction;
    int                    ret = TPI_WIDGET_OK;

    ptr = seekWidget(handle, &tmpWidgetType);

    if (ptr == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        switch (tmpWidgetType)
        {
            case MSF_IMAGE:
                pImage = (MsfImage *)ptr;
                pImage->referenceCounter++;
            break;
            
            case MSF_STRING:
                pString = (MsfString *)ptr;
                pString->referenceCounter++;
            break;
            
            case MSF_ICON:
                pIcon = (MsfIcon *)ptr;
                pIcon->referenceCounter++;
            break;
            
            case MSF_SOUND:
                pSound = (MsfSound *)ptr;
                pSound->referenceCounter++;
            break;
            
            case MSF_STYLE:
                pStyle = (MsfStyle *)ptr;
                pStyle->referenceCounter++;
            break;
            
            case MSF_ACTION:
                pAction = (MsfAction *)ptr;
                pAction->referenceCounter++;
            break;
            
            case MSF_STRING_RES:
                break;
            
            default:
                ret = TPI_WIDGET_ERROR_INVALID_PARAM;
            break;
        }
    }

    
    return ret;
}



static int bmpanimation_deallocate(void *ptr)
{
    MsfBmpAnimation *pBmpAni;
    MsfGadget         *pGadget;
    int               ret = TPI_WIDGET_OK;

    if (ptr == OP_NULL)
    {
    	return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pBmpAni = (MsfBmpAnimation *)ptr;
    pGadget = (MsfGadget *)pBmpAni;
    if(WIDGET_TIMER_INVALID_ID != pBmpAni->timer_id)
    {
       TPIa_widgetStopTimer( pGadget->gadgetHandle, pBmpAni->timer_id );
    }
    
    gadget_deallocate(pGadget);
    
    /* remove the node from the corresponding queue */
    if (SP_list_delete_by_item(&widgetQueue[MSF_BMPANIMATION], ptr))
    {
        WIDGET_FREE(ptr);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_UNEXPECTED;
    }
        
    return ret;
}




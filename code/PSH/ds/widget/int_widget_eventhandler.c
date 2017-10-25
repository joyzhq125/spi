/*==================================================================================================

    MODULE NAME : int_widget_eventhandler.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   XXXXX            Cxxxxxx     Initial file creation.
    07/03/2003   Steven Lai       P000729     Update choice event handler
    07/30/2003   Zhuxq            P001026     Apply the eventhandler to all window, not only the form and paintbox
    04/08/2003   Zhuxq            P001052     Modify the method of getting the start position of a gadget 
    08/05/2003   Steven Lai       P001140     Change the method of handling screen focus
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    08/14/2003   Steven Lai       P001256     If a dialog is closed, we need to notify the corresponding WAP module.
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    8/11/2003    Chenjs           P001091     Update text input widget.
    0815/2003    Steven Lai       P001268     Fix the bug of cannot access link in web page
    8/18/2003    Zhuxq            P001263     Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    08/20/2003   lindawang        P001308     Add right softkey handle for stop Gif timer.  
    08/20/2003   Steven Lai       P001310     Improve the method of checking whether a widget is in focus
    08/23/2003   lindawang        P001350     solove the MMS Gif stop problem.
    08/25/2003   Steven Lai       P001355     Fixed some warning according to PC-Lint check result
    08/29/2003   Zhuxq            P001396     Add scroll-contents feature to string gadget
    09/03/2003   Zhuxq            P001479     Add the support of enter char to fix some bugs
    09/25/2003   Zhuxq            P001696     Re-implement the dialog
    09/27/2003   Zhuxq            P001727     Fix the bug: in some wap page the dialog connot be closed
    09/26/2003   linda wang       P001451     separate the gif play and show function.    
    10/17/2003   Zhuxq            C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq            P001842     Optimize the paintbox and input method solution and fix some bug
    11/6/2003    Zhuxq            P001981     Fix some bugs: in a menu with a scroll bar, the contents will not be scrolled to the end, etc
    11/11/2003   Zhuxq            P001883     Optimize the redraw strategy to reduce the redraw times
    12/03/2003   Zhuxq            P002139     Enhance the sensitivity of scrollbar in WAP&MMS
    12/04/2003   Andrew           P002113     Add switching IME by # and * (Long Press)
    01/05/2004   Zhuxq            CNXT000072  Add callback function to each widget in the widget system
    01/15/2004   Zhuxq            P002352     All editing actions of MsfTextInput turns to MsfEditor
    01/15/2004   Zhuxq            P002353     Using timer callback mechanism to support non-focus image gadget to play animation
    02/02/2004   Chenxiao         p002404     add textinput scrollbar  in the dolphin project    
    02/16/2004   Zhuxq            P002469     Add button gadget to widget system
    02/16/2004   chenxiao         p002474     add bitmap animation function in the dolphin project    
    02/17/2004   Zhuxq            P002483     Adjust the display system of widget to show up the annunciator and fix some bugs
    02/17/2004   MengJiangHong    p002481     add scroll bar to stinggadget dynamically
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    02/19/2004   Zhuxq            P002504     Add box gadget to widget system
    02/20/2004   Zhuxq            P002517     Adjust the handle order of pen events in widget system
    02/24/2004   chenxiao         p002530     modify WAP & MSG main menu     
    02/28/2004   Zhuxq            P002556     Fix some widget bugs
    03/03/2004   MengJianghong    P002568     Selectgroup draw function adjust.
    03/08/2004   Andrew           P002598     Update WAP Editor for Dolphin project
    03/19/2004   Zhuxq            P002687     Fix some bugs in widget system
    03/19/2004   Chenxiao         p002688     change main menu  of wap and fix some bugs from NEC feedback         
    03/27/2004   Zhuxq            P002760     Fix some bugs in widget system
    03/30/2004   zhuxq            P002700     Fix UMB bugs
    03/31/2004   Dingjainxin      P002754     Delete event handle OPUS_TIMER_GIF_ANIMATION && OPUS_FOCUS_KEYPAD for the Timer start/stop by app
    04/03/2004   chenxiao         p002806     The display information is incorrect.        
    04/02/2004   zhuxq            P002789     Fix UMB bugs
    04/09/2004   Lhjie            p002799     include the opus_events.h
    04/15/2004   zhuxq            P002977     Fix some bugs in UMB for Dolphin
    04/14/2004   Dingjianxin      P002961     delete conditonal compiler about ALL_TEXT_EDIT_GOTO_EDIYOR
    04/22/2004   Andrew           P002659     Fix some bugs of projects that bases Dolphin

    04/24/2004   zhuxq            P003022     Fix some bugs in UMB
    04/29/2004   zhuxq            P005196     Fix some bugs in UMB
    05/11/2004   zhuxq            P005352     Optimize UMB and fix some bugs in UMB, includes CNXT00004775
    05/24/2004   zhuxq            P005568     Adjust UI implementation in UMB and fix some bugs
    06/01/2004   zhuxq            P005819     Optimize UMB codes
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/09/2004   Jianghb          P006036     Add WHALE1,WHALE2 and NEMO compilerswitch to some module
    
    06/09/2004   zhuxq            P006048     forbid Menu to respond successive multiple actions
    06/12/2004   zhuxq            P006085     Add lotus macro for lotus   
    06/16/2004   zhuxq            P006224     Modify the menu style for lotus
    06/18/2004   zhuxq            P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/01/2004   zhuxq            P006630     Fix bugs that the position of image can't be center
    07/09/2004   sunsongtao       p006716     sound form comeback again
    
    07/07/2004   Chenxiao         p006839     Sometimes the icon of input address is incorrect.     
    07/22/2004   chenxiao         p007189     the 1 shortcut is invalid
    07/23/2004   zhuxq            P007215     Prevent freeze in wap
    08/23/2004   Zhuxq            p007888     Fix the bug that the icon of 'abc' input method flashes when inputing in a wap page
    08/27/2004   chenxiao       p007990      The scrollbar is created and showed even if the page count is less than 1.    
    08/30/2004   zhuxq             c007998     improve the implementation of predefined string to reduce the times of memory allocation
    
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#ifdef WIN32
#include "portab_new.h"
#else
#include "wcdtypes.h"
#include "portab.h"
#endif

#include "Rm_typedef.h"
#include "Ds_drawing.h"
#include "Keymap.h"

#include "Uhapi.h"

#include "SP_list.h"
#include "GSM_timer.h"

#include "msf_env.h"

#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "int_widget_txt_input.h"
#include "int_widget_custom.h"
#include "int_widget_show.h"
#include "int_widget_focus.h"
#include "int_widget_imm.h"
#include "KEY_table.h"

#include "INTERFACE_types.h"

#include "OPUS_events.h"

#define REGISTERED_PEN_EVENT_MASK  ( 0x00000001 << MsfPointer )

#define POINT_IN_RECT( pt, posLeftTop, rectSize )  \
        (((pt).x >= (posLeftTop).x && (pt).x < (posLeftTop).x + (rectSize).width) \
        && ((pt).y >= (posLeftTop).y && (pt).y < (posLeftTop).y + (rectSize).height))



extern int  widgetPointScreenToClient
( 
    const MsfWindow   *pWin, 
    const OP_INT16  screen_x,  
    const OP_INT16  screen_y,  
    OP_INT16           *pClient_x,
    OP_INT16           *pClient_y
);

extern int widgetGadgetGetPosition( const MsfGadget* pGadget,  OP_INT16  *pX, OP_INT16  *pY);
extern int widgetImmEventHandler
(
    MsfWidgetImm*                   pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                      *pMess,
    OP_BOOLEAN                       *handle
);
extern int  widgetDrawTextInputCaret
( 
    const MsfTextInput      *pInput,
    OP_BOOLEAN            bRefresh
);

extern int widgetChoiceGetItemRect
( 
    MsfSelectgroup         *pSelectGroup,
    OP_INT16              index,
    MsfPosition           *pPos,
    MsfSize                *pSize
);

extern OP_BOOLEAN widgetFocusGadgetAdjustWindow
(
    MsfGadget  *pGadget,
    MsfWindow   *pWin
);

extern OP_BOOLEAN  widgetPaintBoxNeedHorizontalScroll( MsfWindow *pPaintBox );

extern int widgetHandleTimer( OPUS_EVENT_ENUM_TYPE   event, OP_BOOLEAN   *handle);

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static int screen_event_handler 
(
    OP_UINT32 screenHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
);

static int paintbox_event_handler 
(
    OP_UINT32 paintboxHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int form_event_handler 
(
    OP_UINT32 formHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int dialog_event_handler 
(
    OP_UINT32 dialogHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int menu_event_handler 
(
    OP_UINT32 menuHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int main_menu_event_handler 
(
 OP_UINT32 menuHandle, 
 OPUS_EVENT_ENUM_TYPE   event, 
 void                   *pMess,
 OP_BOOLEAN   *handle
 );

static int viewer_event_handler 
(
    OP_UINT32 hView, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int editor_event_handler 
(
    OP_UINT32 editorHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int selectgroup_event_handler 
(
    OP_UINT32 selectgroupHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int datetime_event_handler 
(
    OP_UINT32 datetimeHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int textinput_event_handler 
(
    OP_UINT32 textinputHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int stringgadget_event_handler 
(
    OP_UINT32 stringgadgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int imagegadget_event_handler 
(
    OP_UINT32 imagegadgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int button_event_handler 
(
    OP_UINT32 hButton, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int bmpanimation_event_handler 
(
 OP_UINT32 bmpanimationHandle, 
 OPUS_EVENT_ENUM_TYPE   event, 
 void                   *pMess,
 OP_BOOLEAN   *handle
 );

static int bar_event_handler 
(
    OP_UINT32 barHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
);

static int choice_event_handler
(
    MsfSelectgroup              *pSelectGroup, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN               *handle
);

static int choice_scroll_up(MsfSelectgroup  *pSelectGroup);
static int choice_scroll_down(MsfSelectgroup  *pSelectGroup);
static OP_BOOLEAN choice_select_focus(MsfSelectgroup  *pSelectGroup,OP_BOOLEAN bScrolled);
static void choice_reset_direct_number(void);
static OP_BOOLEAN is_digit_key(OP_UINT16 key);

static void gadget_userevent_handler 
(
    OP_UINT32 gadgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
);

static void window_userevent_handler 
(
    OP_UINT32 windowHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
);

static void screen_userevent_handler 
(
    OP_UINT32 screenHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
);

static void accesskey_handler
(
    OP_UINT32 widgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
);

static void action_handler
(
    OP_UINT32 widgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
);

static void registered_userevent_handler
(
    OP_UINT32 widgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
);

static int window_event_handler 
(
    MsfWindow           *pWin,
    MsfWidgetType    wt,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN       *handle
);

#if 0
static MsfEvent OpusEvent_to_MsfEvent
(
    OPUS_EVENT_ENUM_TYPE   event,
    void                   *pMess
);
#endif

static MsfAction* OpusEvent_to_MsfAction
(
    OPUS_EVENT_ENUM_TYPE   event,
    void                   *pMess,
    OP_UINT32             widget_handle
);

static char* OpusEvent_to_MsfAccessKey
(
    OPUS_EVENT_ENUM_TYPE   event,
    void                   *pMess
);

static void widgetEventHandler
(
    MsfScreen                         *pScreen,
    OPUS_EVENT_ENUM_TYPE   event,
    void                                *pMess,
    OP_BOOLEAN                    *handle
);

#ifdef _TOUCH_PANEL
static void widgetPenEventHandler
(
    MsfScreen                               *pScreen,
    TP_PEN_EVENT_STRUCTURE_T   *pPen
);

static void widgetPenDownEventHandler
(
    MsfScreen                                *pScreen,
    TP_PEN_EVENT_STRUCTURE_T    *pPen
);

void widgetPenUpMoveEventHandler
(
    MsfScreen                                *pScreen,
    TP_PEN_EVENT_STRUCTURE_T    *pPen
);

void widgetScreenHandlePenDownEvent
(
    MsfScreen                                *pScreen,
    TP_PEN_EVENT_STRUCTURE_T    *pPen
);

static void widgetWindowHandlePenDownEvent
(
    MsfScreen                                *pScreen,
    MsfWindow                               *pWin,
    TP_PEN_EVENT_STRUCTURE_T    *pPen,
    OP_BOOLEAN                           bHandled
);

static int widgetFGadgetHandlePenDownEvent
(
    MsfScreen                               *pScreen,
    MsfWindow                              *pWin,
    TP_PEN_EVENT_STRUCTURE_T   *pPen,
    OP_BOOLEAN                          bHandled
);

static int widgetNFGadgetsHandlePenDownEvent
(
    MsfScreen                               *pScreen,
    MsfWindow                              *pWin,
    TP_PEN_EVENT_STRUCTURE_T   *pPen,
    MsfGadgetScope                     gadgetScope
);


static OP_BOOLEAN  widgetSelectGroupPenEventHandler
(
    MsfSelectgroup                         *pSelectgroup,
    TP_PEN_EVENT_STRUCTURE_T  *pPen,
    OP_BOOLEAN                          handle
);

static OP_BOOLEAN  widgetStringGadgetPenEventHandler
(
    MsfStringGadget                       *pStringGadget,
    TP_PEN_EVENT_STRUCTURE_T  *pPen,
    OP_BOOLEAN                          handle
);

static int gadgetPenEventToKeyEventHandler
(
    const MsfWindow              *pWin,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess

);

static SCROLLBAR_NOTIFICATION_T get_scrollbar_handle_area
(
    MsfBar *hBar, 
    OP_INT16 x, 
    OP_INT16 y
);
#endif

static int widgetScrollBarNotify( MsfBar *pBar, SCROLLBAR_NOTIFICATION_T op );

static OP_BOOLEAN widgetScrollbarScroll
(
    MsfBar *pBar, 
    SCROLLBAR_NOTIFICATION_T op
);

static int widgetFormHandleKeyEventScroll
(
    MsfForm *pForm, 
    UINT16  iKeyCode,
    OP_BOOLEAN   *handle
);

static GadgetsInWindow* widgetWindowGetNextFocusGadget
(
    const MsfWindow  *pWin,
    OP_BOOLEAN        bBack
);

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
/*
typedef enum {
  MsfKey_0 = 0, MsfKey_1 = 1, MsfKey_2 = 2, MsfKey_3 = 3, MsfKey_4 = 4,
  MsfKey_5 = 5, MsfKey_6 = 6, MsfKey_7 = 7, MsfKey_8 = 8, MsfKey_9 = 9, 
  MsfKey_Star = 10, MsfKey_Pound = 11, MsfKey_Up = 12, MsfKey_Down = 13, 
  MsfKey_Left = 14, MsfKey_Right = 15, MsfKey_Select = 16,
  MsfKey_Clear = 17, MsfKey_Yes = 18, MsfKey_No = 19, MsfKey_Menu = 20,
  MsfKey_Soft1 = 21, MsfKey_Soft2 = 22, MsfKey_Soft3 = 23,
  MsfKey_Soft4 = 24, MsfKey_Function1 = 25, MsfKey_Function2 = 26,
  MsfKey_Function3 = 27, MsfKey_Function4 = 28, MsfPointer = 29,
  MsfKey_Undefined = 30
} MsfEventType;
*/

/* map keypad event to MsfEventType */
const OP_UINT16 key_map_array[31] =
{
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_STAR,
    KEY_POUND,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_OK,    /* MsfKey_Select */
    KEY_CLEAR,
    KEY_OK,    /* MsfKey_Yes */
    KEY_SOFT_RIGHT,    /* MsfKey_No */
    KEY_SOFT_LEFT,    /* MsfKey_Menu */
    0x00,    /* MsfKey_Soft1 */
    0x00,    /* MsfKey_Soft2 */
    0x00,    /* MsfKey_Soft3 */
    0x00,    /* MsfKey_Soft4 */
    KEY_SIDE_UP,    /* MsfKey_Function1 */
    KEY_SIDE_DOWN,    /* MsfKey_Function2 */
    KEY_SEND,    /* MsfKey_Function3 */
    0x00,    /* MsfKey_Function4 */
    0x00,    /* MsfPointer */
    0x00     /* MsfKey_Undefined */
};

/*
typedef enum {
  MsfBack = 0,
  MsfCancel = 1,
  MsfExit = 2,
  MsfHelp = 3,
  MsfOk = 4,
  MsfMenu = 5,
  MsfStop = 6,
  MsfSelect = 7
} MsfActionType;

*/

/* map keypad event to MsfActionType */
const OP_UINT16 action_map_array[8] =
{
    KEY_SOFT_RIGHT,    /* MsfBack */ 
    KEY_SOFT_RIGHT,    /* MsfCancel */ 
    KEY_SOFT_RIGHT,    /* MsfExit */ 
    KEY_SOFT_LEFT,     /* MsfHelp */ 
    KEY_OK,            /* MsfOk */ 
    KEY_SOFT_LEFT,     /* MsfMenu */ 
    KEY_SOFT_RIGHT,    /* MsfStop */ 
    KEY_OK             /* MsfSelect */ 
};

/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
#ifdef _NO_CLEAR_KEY
static  OP_UINT8   skeyStr[SKEY_MAX_CHAR*2+2];
#endif
/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/*
 * description of this macro, if needed. - Remember self documenting code
 */
 

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
/* direct number selection support */
typedef int (* widget_msfevent_handle_func_t)(OP_UINT32, 
                                                OPUS_EVENT_ENUM_TYPE, 
                                                void *,
                                                OP_BOOLEAN *);

/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

widget_msfevent_handle_func_t widgetMsfEventHandleFunc[MSF_MAX_WIDGETTYPE] =
{
    screen_event_handler,  /* Screen */
    
    paintbox_event_handler, /* Paintbox */
    form_event_handler, /* Form */
    dialog_event_handler, /* Dialog */
    menu_event_handler, /* Menu */
    main_menu_event_handler, /* Main menu */
    viewer_event_handler, /* Viewer */
    OP_NULL,                     /* Waiting window */
    editor_event_handler, /* Editor */
    
    selectgroup_event_handler, /* selectgroup */
    datetime_event_handler, /* datetime */
    textinput_event_handler, /* textinput */
    stringgadget_event_handler, /* stringgadget */
    imagegadget_event_handler, /* imagegadget */
    button_event_handler,   /* button  */
    bmpanimation_event_handler, /* bmpanimation */
    selectgroup_event_handler, /* box */
    bar_event_handler, /* bar */
    
    OP_NULL, /* image */
    OP_NULL, /* string */
    OP_NULL, /* icon */
    OP_NULL, /* sound */
    OP_NULL, /* style */
    OP_NULL, /* coloranim */
    OP_NULL, /* move */
    OP_NULL, /* rotation */

    OP_NULL, /* action */
    OP_NULL /* predefined string */
};



/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
static int screen_event_handler 
(
    OP_UINT32 screenHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    /* do nothing here */
    return TPI_WIDGET_OK;
}

static int paintbox_event_handler 
(
    OP_UINT32 paintboxHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    /* do nothing here */
    return TPI_WIDGET_OK;
}

static int window_event_handler 
(
    MsfWindow           *pWin,
    MsfWidgetType    wt,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    MsfWidgetType  gadgetType = MSF_MAX_WIDGETTYPE;
    OP_UINT32     hPreFocusedGadget = INVALID_HANDLE;   /* the handle of the focused gadget before this function */
    OP_UINT32     hNewFocusedGadget;   /* the handle of the new focused gadget after this function */
    MsfGadget      *pPreGadget= OP_NULL;
    MsfGadget      *pCurGadget= OP_NULL;
    OP_BOOLEAN       needToHandle;
    OP_BOOLEAN       bFocusChanged = OP_FALSE;
    GadgetsInWindow *pGadInWin, *pGadInWinTemp;
    KEYEVENT_STRUCT *pKey;
    int                iRet;
    OP_INT8        step = 1; /* -1 to scroll up;  1 to scoll down */

    hPreFocusedGadget = INVALID_HANDLE;
    hNewFocusedGadget = INVALID_HANDLE;
    needToHandle = OP_FALSE;
    
    pKey = (KEYEVENT_STRUCT *)pMess;
    if (event == OPUS_FOCUS_KEYPAD)
    {
        if(pKey->state != UHKEY_RELEASE)
        {
            needToHandle = OP_TRUE;
        }
    }
    else if (event == OPUS_TIMER_KEYPRESS_REPEAT)
    {
        needToHandle = OP_TRUE;
    }
    
    if (needToHandle == OP_TRUE)
    {
        *handle = OP_TRUE;
        switch (pKey->code)
        {
            case KEY_UP:
            case KEY_SIDE_UP:
            case KEY_LEFT:
                step = -1;
                break;
                
            case KEY_DOWN:
            case KEY_SIDE_DOWN:
            case KEY_RIGHT:
                step = 1;
                break;
        
            default:
                *handle = OP_FALSE;
                break;
        }

        if (*handle == OP_TRUE) /* need to handle this event */
        {
            pGadInWin = pWin->focusedChild;

            if (pWin->focusedChild != OP_NULL)
            {
                pPreGadget = pGadInWin->gadget;
                hPreFocusedGadget = pPreGadget->gadgetHandle;
                gadgetType = getWidgetType(hPreFocusedGadget);
            }

            if (step == -1)
            {   /* the focus will switch to the prior gadget */
                if (pWin->focusedChild != OP_NULL)
                {
                    pGadInWin = pWin->focusedChild->prev;
                    while (pGadInWin != OP_NULL)
                    {
                        if (((MsfGadget *)pGadInWin->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                        {
                            break;
                        }
                        pGadInWin = pGadInWin->prev;
                    }

                    /* find the last gadget which can get focus */
                    if(!pGadInWin &&  !(pWin->propertyMask & MSF_WINDOW_PROPERTY_NOT_KEEP_GADGET_FOCUS ))
                    {
                        pGadInWinTemp = pWin->focusedChild->next;
                        while ( pGadInWinTemp!= OP_NULL)
                        {
                            if (((MsfGadget *)pGadInWinTemp->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                            {
                                pGadInWin = pGadInWinTemp;
                            }
                            pGadInWinTemp = pGadInWinTemp->next;
                        }
                    }
                }
                else
                {
                    GadgetsInWindow *pTemp = OP_NULL;
                    /* the new focused gadget should be the last one*/
                    pGadInWin = pWin->children;

                    /* look for the last gadget that can be focused */
                    while (pGadInWin != OP_NULL)
                    {
                        if (((MsfGadget *)pGadInWin->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                        {
                            pTemp = pGadInWin;
                        }
                        pGadInWin = pGadInWin->next;
                    }
                    pGadInWin = pTemp;
                }
            }
            else
            {
                if (pWin->focusedChild != OP_NULL)
                {
                    pGadInWin = pWin->focusedChild->next;
                }
                else
                {
                    /* the new focused gadget should be the first one*/
                    pGadInWin = pWin->children;
                }
                while (pGadInWin != OP_NULL)
                {
                    if (((MsfGadget *)pGadInWin->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                    {
                        break;
                    }
                    pGadInWin = pGadInWin->next;
                }
                
                /* find the first gadget which can get focus */
                if(!pGadInWin && pWin->focusedChild != OP_NULL
                && !(pWin->propertyMask & MSF_WINDOW_PROPERTY_NOT_KEEP_GADGET_FOCUS ))
                {
                    pGadInWinTemp = pWin->focusedChild->prev;
                    while ( pGadInWinTemp!= OP_NULL)
                    {
                        if (((MsfGadget *)pGadInWinTemp->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                        {
                            pGadInWin = pGadInWinTemp;
                        }
                        pGadInWinTemp = pGadInWinTemp->prev;
                    }
                }
            }
            
            if (pGadInWin != OP_NULL)
            {
                pCurGadget = pGadInWin->gadget;
                hNewFocusedGadget = pCurGadget->gadgetHandle;
                iRet = setGadgetInFocus(pCurGadget);
                
                if( iRet != WIDGET_WINDOW_ALREADY_DRAWN )
                {
                    widgetRedraw( (void*)pCurGadget, getWidgetType(hNewFocusedGadget), INVALID_HANDLE, OP_TRUE );
                    bFocusChanged = OP_TRUE;
                }
                
                if(pCurGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
                {
                    if( pCurGadget->cbStateChange )
                    {
                        (void)pCurGadget->cbStateChange(pCurGadget->modId, hNewFocusedGadget, MsfNotifyFocus, OP_NULL );
                    }
                    else
                    {
                        TPIc_widgetNotify(pCurGadget->modId,
                                          hNewFocusedGadget, 
                                          MsfNotifyFocus);
                    }                  
                }
            }
            else if( hPreFocusedGadget     /* The window allow all gadget to lose focus  */
               && (pWin->propertyMask & MSF_WINDOW_PROPERTY_NOT_KEEP_GADGET_FOCUS ))
            {
                pWin->focusedChild = OP_NULL;
                
                if (gadgetType < MSF_MAX_WIDGETTYPE && msfWidFocusFuncs[gadgetType].pWidLoseFocus)
                {
                    (void)msfWidFocusFuncs[gadgetType].pWidLoseFocus(pPreGadget);
                }
                
                widgetRedraw( (void*)pPreGadget, gadgetType, INVALID_HANDLE, OP_TRUE);
                bFocusChanged = OP_TRUE;
                
                if (pPreGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
                {
                    if( pPreGadget->cbStateChange )
                    {
                        pPreGadget->cbStateChange(pPreGadget->modId, hPreFocusedGadget, MsfNotifyLostFocus, OP_NULL );
                    }
                    else
                    {
                        TPIc_widgetNotify(pPreGadget->modId,
                                          hPreFocusedGadget, 
                                          MsfNotifyLostFocus);
                    }                  
                }

            }
        }

    }
    
    if (bFocusChanged == OP_FALSE )
    {
        widgetWindowShowSoftKey( pWin,  OP_TRUE );
    }
    
    return TPI_WIDGET_OK;
}

/*
 * In this handler, the focus may be switched between gadgets according to the 
 * user event. 
 * Not all kinds of gadget can be set in focused, but the viewport should be
 * scrolled to display some non-focus gadget when in need. Such as, if there is  a 
 * stringGadget located below the last can-be-focused gadget, and the user event 
 * is KEY_DOWN, even though this string gadget cannot get focus, the viewport 
 * should be scrolled down to let it display to user.
 */
static int form_event_handler 
(
    OP_UINT32 formHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    KEYEVENT_STRUCT  *pKey;
    MsfWindow             *pWin;
    MsfForm                  *pForm;
    GadgetsInWindow       *pNextGadgetNode;
    MsfGadget              *pCurGadget, *pNextGadget;
    MsfWidgetType       windowType;
    OP_BOOLEAN          bBack = OP_FALSE;
    OP_BOOLEAN          bScrollWindow = OP_TRUE;
    MsfPosition             posClient;
    MsfSize                 sizeCurGadget, sizeNextGadget, sizeClient;
    OP_INT16              iLeftNextGadget, iTopNextGadget, iRightNextGadget, iBottomNextGadget;
    OP_INT16              iLeftCurGadget, iTopCurGadget, iRightCurGadget, iBottomCurGadget;
    OP_INT16              left, top, right, bottom;
    OP_INT16              leftNext, topNext, rightNext, bottomNext;
    
    /* till now, *handle == OP_FALSE */
    pWin = (MsfWindow*)seekWidget(formHandle, &windowType);
    if (pWin == OP_NULL || 
        windowType != MSF_FORM || 
        pWin->isFocused == OP_FALSE || /* this window is not in focus */
        pWin->children == OP_NULL || /* there is no gadget attached to this form */
        (event != OPUS_FOCUS_KEYPAD && 
         event != OPUS_TIMER_KEYPRESS_REPEAT ))
    {
        return TPI_WIDGET_OK;
    }
    
    pKey = (KEYEVENT_STRUCT *)pMess;
    if (event == OPUS_FOCUS_KEYPAD)
    {
        if(pKey->state == UHKEY_RELEASE)
        {
            return TPI_WIDGET_OK;
        }
    }
    else if (event != OPUS_TIMER_KEYPRESS_REPEAT)
    {
        return TPI_WIDGET_OK;
    }

    switch (pKey->code)
    {
        case KEY_UP:
        case KEY_SIDE_UP:
        case KEY_LEFT:
            bBack = OP_TRUE;
            break;
            
        case KEY_DOWN:
        case KEY_SIDE_DOWN:
        case KEY_RIGHT:
            bBack = OP_FALSE;
            break;
    
        default:
            return TPI_WIDGET_OK;
    }

    pForm = (MsfForm*)pWin;
    
    /* get the next focus gadget */
    pNextGadget = OP_NULL;
    pNextGadgetNode = widgetWindowGetNextFocusGadget( pWin, bBack);
    if( pNextGadgetNode )
    {
        pNextGadget = pNextGadgetNode->gadget;
    }
    
    if( pNextGadget )
    {
        widgetGetClientRect( pWin, &posClient, &sizeClient);
        
        widgetGadgetGetPosition( pNextGadget, &iLeftNextGadget, &iTopNextGadget);
        widgetGadgetGetSize( pNextGadget, &sizeNextGadget);
        iRightNextGadget = iLeftNextGadget + sizeNextGadget.width;
        iBottomNextGadget = iTopNextGadget + sizeNextGadget.height;

        leftNext = iLeftNextGadget;
        topNext = iTopNextGadget;
        rightNext = iRightNextGadget;
        bottomNext = iBottomNextGadget;

        /*
          * if the next focus gadget is invisible,
          *          scroll the window;
          * else if the current gadget is invisible, 
          *          switch focus
          * else according to the key event and the two gadget's positions to switch focus or scroll window
          */
        if( widgetRectGetVisible(
                                    posClient.x, posClient.y,
                                    (OP_INT16)(posClient.x + sizeClient.width),
                                    (OP_INT16)(posClient.y + sizeClient.height),
                                    &leftNext, &topNext, &rightNext, &bottomNext))
        {
            pCurGadget = OP_NULL;
            if( pWin->focusedChild != OP_NULL )
            {
                pCurGadget = pWin->focusedChild->gadget;
            }
            
            if( !pCurGadget )
            {
                /* switch focus */
                bScrollWindow = OP_FALSE;
            }
            else
            {
                widgetGadgetGetPosition( pCurGadget, &iLeftCurGadget, &iTopCurGadget);
                widgetGadgetGetSize( pCurGadget, &sizeCurGadget);
                iRightCurGadget = iLeftCurGadget + sizeCurGadget.width;
                iBottomCurGadget = iTopCurGadget + sizeCurGadget.height;
                
                left = iLeftCurGadget;
                top = iTopCurGadget;
                right = iRightCurGadget;
                bottom = iBottomCurGadget;

                /* the current focused gadget is invisible, switch focus */
                if( OP_FALSE == widgetRectGetVisible(
                                            posClient.x, posClient.y,
                                            (OP_INT16)(posClient.x + sizeClient.width),
                                            (OP_INT16)(posClient.y + sizeClient.height),
                                            &left, &top, &right, &bottom))
                {
                    /* switch focus */
                    bScrollWindow = OP_FALSE;
                }
                /* the next gadget is full visible  */
                else if( leftNext == iLeftNextGadget 
                   && topNext == iTopNextGadget
                   && rightNext == iRightNextGadget
                   && bottomNext == iBottomNextGadget )
                {
                    /* the current gadget is not full visible  */
                     if( left != iLeftCurGadget 
                       || top != iTopCurGadget
                       || right != iRightCurGadget
                       || bottom != iBottomCurGadget )
                    {   
                        /* switch focus */
                        bScrollWindow = OP_FALSE;
                    }
                    else
                    {
                        if( bBack )
                        {
                            if( pKey->code == KEY_LEFT )
                            {
                                if( pWin->scrollPos.x > 0 )
                                {
                                    /* the next gadget is left to current gadget*/
                                    if( iLeftNextGadget < iLeftCurGadget ) 
                                    {
                                        /* switch focus */
                                        bScrollWindow = OP_FALSE;
                                    }
                                }
                                else
                                {
                                    /* switch focus */
                                    bScrollWindow = OP_FALSE;
                                }
                            }
                            else
                            {
                                if( pWin->scrollPos.y > 0 )
                                {
                                    /* the next gadget is top to current gadget*/
                                    if( iTopNextGadget < iTopCurGadget )
                                    {
                                        /* switch focus */
                                        bScrollWindow = OP_FALSE;
                                    }
                                }
                                else
                                {
                                    /* switch focus */
                                    bScrollWindow = OP_FALSE;
                                }
                            }    
                        }
                        else 
                        {
                            if(  pKey->code == KEY_RIGHT )
                            {
                                if( posClient.x + sizeClient.width <= pForm->iMaxScrollAreaX )
                                {
                                    /* the next gadget is right to current gadget*/
                                    if( iRightNextGadget > iRightCurGadget )
                                    {
                                        /* switch focus */
                                        bScrollWindow = OP_FALSE;
                                    }
                                }
                                else
                                {
                                    /* switch focus */
                                    bScrollWindow = OP_FALSE;
                                }
                            }
                            else
                            {
								if( sizeClient.height >= pForm->iMaxScrollAreaY )
								{
									/* switch focus */
									bScrollWindow = OP_FALSE;
								}
								else if( posClient.y + sizeClient.height <= pForm->iMaxScrollAreaY )
                                {
                                    /* the next gadget is bottom to current gadget*/
                                    if( iBottomNextGadget > iBottomCurGadget )
                                    {
                                        /* switch focus */
                                        bScrollWindow = OP_FALSE;
                                    }
                                }
                                else
                                {
                                    /* switch focus */
                                    bScrollWindow = OP_FALSE;
                                }
                            }
                        }
                    }
                }
                /* the current gadget is not full visible  */
                else if(left != iLeftCurGadget 
                       || top != iTopCurGadget
                       || right != iRightCurGadget
                       || bottom != iBottomCurGadget )
                {
                    if( bBack )
                    {
                        /* the next gadget is left to current gadget*/
                        if( pKey->code == KEY_LEFT )
                        {
                            if(  iLeftNextGadget < iLeftCurGadget) 
                            {
                                /* switch focus */
                                bScrollWindow = OP_FALSE;
                            }
                        }
                        /* the next gadget is top to current gadget*/
                        else if( iTopNextGadget < iTopCurGadget )
                        {
                            /* switch focus */
                            bScrollWindow = OP_FALSE;
                        }
                    }
                    else 
                    {
                        if(  pKey->code == KEY_RIGHT )
                        {
                            /* the next gadget is right to current gadget*/
                            if( iRightNextGadget > iRightCurGadget )
                            {
                                /* switch focus */
                                bScrollWindow = OP_FALSE;
                            }
                        }
                        /* the next gadget is bottom to current gadget*/
                        else if( iBottomNextGadget > iBottomCurGadget )
                        {
                            /* switch focus */
                            bScrollWindow = OP_FALSE;
                        }
                    }
                }
            }
        }
    }

    if( bScrollWindow == OP_TRUE )
    {
        return widgetFormHandleKeyEventScroll( pForm, pKey->code, handle);
    }
    else
    {
        return window_event_handler( pWin, MSF_FORM, event, pMess, handle);
    }
}

static int dialog_event_handler 
(
    OP_UINT32 dialogHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    MsfWindow   *pWin;
    MsfDialog     *pDialog;
    MsfWidgetType widgetType;

    /* till now, *handle == OP_FALSE */
    pDialog= (MsfDialog *)seekWidget(dialogHandle, &widgetType);
    if (pDialog == OP_NULL 
        || widgetType != MSF_DIALOG 
        || pDialog->windowData.isFocused == OP_FALSE )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin= (MsfWindow *)pDialog;
    
    /*  handle the focus switch between textinput  */  
    window_event_handler( pWin, MSF_FORM, event, pMess, handle);

    return TPI_WIDGET_OK;
}

static int menu_event_handler 
(
    OP_UINT32 menuHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
#if 0
    MsfMenuWindow     *pMenu;
    MsfChoice         *pChoice;
    MsfWidgetType     widgetType;

    /* till now, *handle == OP_FALSE */
    
    pMenu= (MsfMenuWindow *)seekWidget(menuHandle, &widgetType);
    if (pMenu == OP_NULL || widgetType != MSF_MENU)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pChoice = &pMenu->pSelectgroup->choice;
#endif

    return TPI_WIDGET_OK;
}

static int main_menu_event_handler 
(
    OP_UINT32 menuHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    return TPI_WIDGET_OK;
}


static int viewer_event_handler 
(
    OP_UINT32 editorHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    return TPI_WIDGET_OK;
}

static int editor_event_handler 
(
    OP_UINT32 editorHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    return TPI_WIDGET_OK;
}

static int selectgroup_event_handler 
(
    OP_UINT32 selectgroupHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    MsfGadget     *pGadget;
    MsfSelectgroup *pSelectgroup;
    MsfBox          *pBox;
    MsfChoice       *pChoice;
    MsfWidgetType  widgetType, wtWin;
    MsfWindow    *pWin;
    OP_BOOLEAN      bNeedNotify;
    int                    iTopItem;
    int                    iRet;
    MsfNotificationType  notiType;
    int                    iExtaData = SB_NOTIFY_INVALID;
    
    /* till now, *handle == OP_FALSE */
    pGadget = (MsfGadget *)seekWidget(selectgroupHandle, &widgetType);
    if ((pGadget == OP_NULL) || 
        (widgetType != MSF_SELECTGROUP && widgetType != MSF_BOX )||
        (pGadget->isFocused == OP_FALSE))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pSelectgroup = (MsfSelectgroup*)pGadget;
    pWin = pGadget->parent;
    wtWin = getWidgetType( pWin->windowHandle );
    
    pChoice = &pSelectgroup->choice;
#ifdef _TOUCH_PANEL
    if( event == OPUS_FOCUS_PEN )
    {
        bNeedNotify = widgetSelectGroupPenEventHandler(
                                                pSelectgroup, 
                                                (TP_PEN_EVENT_STRUCTURE_T*)pMess,
                                                *handle);
        /*  all pen events passed to selectgroup will be consumed  */                                                
        *handle = OP_TRUE;
        if (bNeedNotify == OP_TRUE)
        {
            if(( wtWin == MSF_MENU ) ||
               (wtWin == MSF_MAINMENU))
            {
                KEYEVENT_STRUCT   mess;
                
                /* Menu must use the ok event to trigger */
                mess.code = KEY_OK;
                mess.state = UHKEY_PRESS;
                
                gadgetPenEventToKeyEventHandler( pWin,  OPUS_FOCUS_KEYPAD, (void*)&mess);
            }
            else  if(pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
            {
                if( pGadget->cbStateChange )
                {
                    if( widgetType == MSF_BOX )
                    {
                        iExtaData = ((MsfBox*)pSelectgroup)->iCurPos + pChoice->cur_item;
                    }
                    else
                    {
                        iExtaData = pChoice->cur_item;
                    }
                    
                    (void)pGadget->cbStateChange(pGadget->modId, selectgroupHandle, MsfNotifyStateChange, (void*)iExtaData);
                }
                else
                {
                    TPIc_widgetNotify(pGadget->modId,
                                      selectgroupHandle, 
                                      MsfNotifyStateChange);
                }
            }
        }
    }
    else
#endif
    {
        iTopItem = pChoice->top_item;
        iRet = choice_event_handler( pSelectgroup, event, pMess, handle);

        /* Adjust the related scrollbar position */
        if( iTopItem != pChoice->top_item )
        {
            if( ISVALIDHANDLE(pGadget->hVScrollBar))
            {
                TPIa_widgetBarSetValues(
                            pGadget->hVScrollBar,
                            pChoice->top_item,
                            pChoice->count -1 );
            }
        }

        /* need to redraw the selectgroup  */
        if( iRet & WIDGET_EVENT_HANDLED_NEED_PAINT )
        {
            widgetRedraw( (void *) pSelectgroup, widgetType, selectgroupHandle, OP_TRUE);
        }

        bNeedNotify = OP_FALSE;
        
        /* need to notify the module  */
        if (iRet & WIDGET_EVENT_HANDLED_NEED_NOTIFY)
        {
            notiType = MsfNotifyStateChange;
            iExtaData = pChoice->cur_item;
            bNeedNotify = OP_TRUE;
        }
        else if( pGadget->propertyMask & MSF_GADGET_PROPERTY_REQUEST_CONTENT )
        {
            if( iRet & WIDGET_EVENT_HANDLED_REQ_PREVIOUS_NOTIFY )
            {
                notiType = MsfNotifyRequestNewContent;
                iExtaData = (int)SB_NOTIFY_LINE_UP;
                
                if( MSF_BOX == widgetType )
                {
                    pBox = (MsfBox*)pSelectgroup;
                    if( pBox->nItemCount > pChoice->elementsPerPage )
                    {
                        if( pBox->iCurPos > 0 )  /* not reach the first item of the box */
                        {
                            pBox->iCurPos --;
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                        else if( pSelectgroup->choice.scrollCircularly )
                        {
                            pBox->iCurPos = pBox->nItemCount - pSelectgroup->choice.elementsPerPage;
                            pSelectgroup->choice.cur_item = pSelectgroup->choice.elementsPerPage - 1;
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                    }
                }
                else
                {
                    bNeedNotify = OP_TRUE;
                }
            }
            else if( iRet & WIDGET_EVENT_HANDLED_REQ_NEXT_NOTIFY )
            {
                notiType = MsfNotifyRequestNewContent;
                iExtaData = (int)SB_NOTIFY_LINE_DOWN;
                
                if( MSF_BOX == widgetType )
                {
                    pBox = (MsfBox*)pSelectgroup;
                    if( pBox->nItemCount > pChoice->elementsPerPage )
                    {
                        if( pBox->iCurPos + pChoice->elementsPerPage < pBox->nItemCount )  /* not reach the last item of the box */
                        {
                            pBox->iCurPos ++;
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                        else if( pSelectgroup->choice.scrollCircularly )
                        {
                            pBox->iCurPos = 0;
                            pSelectgroup->choice.cur_item = 0;
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                    }
                }
                else
                {
                    bNeedNotify = OP_TRUE;
                }
            }
            /* page up */
            else if( iRet & WIDGET_EVENT_HANDLED_REQ_PRE_PAGE_NOTIFY)
            {
                notiType = MsfNotifyRequestNewContent;
                iExtaData = (int)SB_NOTIFY_PAGE_UP;
                
                if( MSF_BOX == widgetType )
                {
                    pBox = (MsfBox*)pSelectgroup;
                    if( pBox->nItemCount > pChoice->elementsPerPage )
                    {
                        if( pBox->iCurPos > 0 )  /* not reach the first item of the box */
                        {
                            pBox->iCurPos -= pSelectgroup->choice.elementsPerPage;
                            if( pBox->iCurPos < 0 )
                            {
                                pBox->iCurPos = 0;
                            }
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                        else if( pSelectgroup->choice.scrollCircularly )
                        {
                            pBox->iCurPos = pBox->nItemCount - pSelectgroup->choice.elementsPerPage;
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                    }
                }
                else
                {
                    bNeedNotify = OP_TRUE;
                }
            }
            /* page down */
            else if( iRet & WIDGET_EVENT_HANDLED_REQ_NEXT_PAGE_NOTIFY)
            {
                notiType = MsfNotifyRequestNewContent;
                iExtaData = (int)SB_NOTIFY_PAGE_DOWN;
                
                if( MSF_BOX == widgetType )
                {
                    pBox = (MsfBox*)pSelectgroup;
                    if( pBox->nItemCount > pChoice->elementsPerPage )
                    {
                        if( pBox->iCurPos + pChoice->elementsPerPage < pBox->nItemCount )  /* not reach the last item of the box */
                        {
                            pBox->iCurPos += pChoice->elementsPerPage;
                            if( pBox->iCurPos > pBox->nItemCount - pChoice->elementsPerPage )
                            {
                                pBox->iCurPos = pBox->nItemCount - pChoice->elementsPerPage;
                            }
                            
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                        else if( pSelectgroup->choice.scrollCircularly )
                        {
                            pBox->iCurPos = 0;
                            TPIa_widgetBarSetValues( pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1 ));
                            bNeedNotify = OP_TRUE;
                        }
                    }
                }
                else
                {
                    bNeedNotify = OP_TRUE;
                }
            }
        }

        if( bNeedNotify == OP_TRUE )
        {
            if(( wtWin == MSF_MENU ) ||
                (MSF_MAINMENU == wtWin))
            {
                if( pWin->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY )
                {
                    if( pWin->cbStateChange )
                    {
                        (void)pWin->cbStateChange(pWin->modId, pWin->windowHandle, notiType, (void*)iExtaData);
                    }
                    else
                    {
                        TPIc_widgetNotify(pWin->modId,
                                          pWin->windowHandle, 
                                          notiType);
                    }
                }
                
                *handle = OP_TRUE;
            }
            else  if(pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
            {
                *handle = OP_TRUE;
                if( pGadget->cbStateChange )
                {
                    iRet = pGadget->cbStateChange(pGadget->modId, selectgroupHandle, notiType, (void*)iExtaData);
                    if(TPI_WIDGET_NOT_HANDLED == iRet  && MSF_BOX != widgetType)
                    {
                        *handle = OP_FALSE;
                    }
                }
                else
                {
                    TPIc_widgetNotify(pGadget->modId,
                                      selectgroupHandle, 
                                      notiType);
                }
            }
            else if( event == OPUS_FOCUS_KEYPAD && ((KEYEVENT_STRUCT *)pMess)->code == KEY_OK )
            {
                *handle = OP_FALSE;
            }
        }
    }

    return TPI_WIDGET_OK;
}

static int datetime_event_handler 
(
    OP_UINT32 datetimeHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    return TPI_WIDGET_OK;
}

static int textinput_event_handler 
(
    OP_UINT32 textinputHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    KEYEVENT_STRUCT *pKey = OP_NULL;
    MsfTextInput *pTextInput;
    MsfInput* pInput ;
    static OP_UINT16 iLastKeyPressedCode = 0;
    OP_BOOLEAN bRet = OP_FALSE;
    OP_BOOLEAN bTextChanged = OP_FALSE;
    int               iRet;
    MsfWidgetType  wt, winWt;
    int               key = '0';
#ifdef _NO_CLEAR_KEY
    int               iLength = 0;
#endif

    pTextInput = (MsfTextInput*)seekWidget(textinputHandle, &wt);
    if(!pTextInput || wt != MSF_TEXTINPUT || pTextInput->gadgetData.parent == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    pInput = &pTextInput->input;
    *handle = OP_FALSE;
    if(event == OPUS_FOCUS_KEYPAD)
    {
        pKey = (KEYEVENT_STRUCT *)pMess;
    
        if(pKey->state == UHKEY_RELEASE 
#ifdef IMM_HANDLE_KEY_POUND_RELEASE        
            && pKey->code != KEY_POUND 
#endif
#ifdef IMM_HANDLE_KEY_STAR_RELEASE
            && pKey->code != KEY_STAR
#endif
#ifdef IMM_HANDLE_KEY_0_RELEASE
            && pKey->code != KEY_0
#endif            
            ) 
        {
            if(iLastKeyPressedCode == pKey->code)
            {
                *handle = OP_TRUE;
                return TPI_WIDGET_OK;
            }
        }
    }
    
    if( (event != OPUS_TIMER_TXT_INPUT_CURSOR_BLINK)
#ifdef _TOUCH_PANEL
        && (event != OPUS_FOCUS_PEN )
#endif        
        && pInput->pImm )
    {
         iRet = widgetImmEventHandler(pInput->pImm, event, pMess,handle);
         if( iRet < 0 )
         {
            return iRet;
         }
         
         if(*handle)
         {
            if(pKey &&  pKey->state != UHKEY_RELEASE) /* event == OPUS_FOCUS_KEYPAD*/
            {
                iLastKeyPressedCode = pKey->code;
            }
            
            return TPI_WIDGET_OK;
         }
    }    

    winWt = getWidgetType(pTextInput->gadgetData.parent->windowHandle);
    
    switch(event)
    {
       case OPUS_TIMER_KEYPRESS_REPEAT:
            pKey = (KEYEVENT_STRUCT *)pMess;
            switch(pKey->code )
            {
#ifdef _NO_CLEAR_KEY
               case KEY_SOFT_RIGHT:
                    if( pInput->pImm )
                    {
                        iLength = TBM_GetContentLen(&(pInput->txtInputBufMng), OP_TRUE);
                        if (iLength > 0)
                        {     
                              bTextChanged = TBM_DelString(&(pInput->txtInputBufMng));
                              if (bTextChanged == OP_FALSE)
                              {
                                  *handle = OP_TRUE;
                                  break;
                              }
                              
                              iLength = TBM_GetContentLen(&(pInput->txtInputBufMng), OP_TRUE);
                              if (iLength <= 0)
                              {
                                  pTextInput->gadgetData.softKeyList[2] = OP_NULL;
                              }
                        }
                    }
                    break;
#endif                    
                case KEY_CLEAR:
                    if( pInput->pImm )
                    {
                        bTextChanged = TBM_DelString(&(pInput->txtInputBufMng));
                    }
                    break;
                   
                case KEY_POUND:            
                    /*
                     *  Comment and move it to int_widget_imm.c
                     *  for managing the all # handling in same function
                     *  Andrew P002113
                    */       
                    /*be handled in multi line textinput only. */
                    /*
                    if(pInput->txtInputBufMng.rows > 1)
                    {
                       bTextChanged = TBM_EnterString(&(pInput->txtInputBufMng), (OP_UINT8*)"\xa\x0\x0\x0");
                    }
                    */
                    break;
                    
                case KEY_LEFT:
                    if( TVM_CanScrollCursor(pTextInput))
                    {
                        bRet = TBM_CursorHead(&(pInput->txtInputBufMng));
                        if( bRet == OP_FALSE )
                        {
                            if( winWt == MSF_PAINTBOX
                              && OP_FALSE == widgetPaintBoxNeedHorizontalScroll(pTextInput->gadgetData.parent))
                            {  
                                *handle = OP_TRUE;
                            }
                        }
                    }
                    break;
                case KEY_RIGHT:
                    if( TVM_CanScrollCursor(pTextInput))
                    {
                        bRet = TBM_CursorTail(&(pInput->txtInputBufMng) );
                        if( bRet == OP_FALSE )
                        {
                            if( winWt == MSF_PAINTBOX
                              && OP_FALSE == widgetPaintBoxNeedHorizontalScroll(pTextInput->gadgetData.parent))
                            {  
                                *handle = OP_TRUE;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            break;    
        case OPUS_FOCUS_KEYPAD:
            pKey = (KEYEVENT_STRUCT *)pMess;   
            if(pKey->state == UHKEY_RELEASE) 
                break;     
                
            switch(pKey->code)
            { 
                case KEY_SOFT_RIGHT:
#ifdef _NO_CLEAR_KEY            
                if( pInput->pImm )
                {
                    iLength = TBM_GetContentLen(&(pInput->txtInputBufMng), OP_TRUE);
                    if (iLength > 0)
                    {
                        bTextChanged = TBM_DelCursorChar(&(pInput->txtInputBufMng));
                        if (bTextChanged == OP_FALSE)
                        {
                            *handle = OP_TRUE;
                            break;
                        }

                        iLength = TBM_GetContentLen(&(pInput->txtInputBufMng), OP_TRUE);
                        if (iLength <= 0)
                        {
                            pTextInput->gadgetData.softKeyList[2] = OP_NULL;
                        }
                    }
                }
                break;
#endif                    
                case KEY_SOFT_LEFT:
                case KEY_OK:
                case KEY_SEND:
                   break;
                case KEY_CLEAR:
                    if( pInput->pImm )
                    {
                        bTextChanged = TBM_DelCursorChar(&(pInput->txtInputBufMng));
                    }
                    else if( !(pTextInput->gadgetData.propertyMask & MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM)
                        && pTextInput->gadgetData.cbStateChange )
                    {
                        (void)pTextInput->gadgetData.cbStateChange(pTextInput->gadgetData.modId, textinputHandle, MsfNotifyClick, OP_NULL);
                    }
                    break;                
                case KEY_LEFT:
                    if( TVM_CanScrollCursor(pTextInput))
                    {
                        bRet = TBM_CursorBackward(&(pInput->txtInputBufMng));
                        if( bRet == OP_FALSE )
                        {
                            if( winWt == MSF_PAINTBOX
                              && OP_FALSE == widgetPaintBoxNeedHorizontalScroll(pTextInput->gadgetData.parent))
                            {  
                                *handle = OP_TRUE;
                            }
                        }
                    }
                    break;
                case KEY_RIGHT:
                    if( TVM_CanScrollCursor(pTextInput))
                    {
                        if(0 == TBM_CursorForward(&(pInput->txtInputBufMng)))
                        {
                            bRet = OP_FALSE;
                        }
                        else
                        {
                            bRet = OP_TRUE;
                        }
                        if( bRet == OP_FALSE )
                        {
                            if( winWt == MSF_PAINTBOX
                              && OP_FALSE == widgetPaintBoxNeedHorizontalScroll(pTextInput->gadgetData.parent))
                            {  
                                *handle = OP_TRUE;
                            }
                        }
                    }
                    break;
                case KEY_UP:
                case KEY_SIDE_UP:
                    bRet = TBM_CursorUp(&(pInput->txtInputBufMng));
                    break;
                case KEY_DOWN: 
                case KEY_SIDE_DOWN:
                    bRet = TBM_CursorDown(&(pInput->txtInputBufMng));
                    break;
                    /*
                case KEY_STAR:
                     *  Comment it and handling * event in int_widget_imm.c
                     *  Andrew P002113
                    // bRet = OP_TRUE;
                    break;
                case KEY_POUND:
                    bRet = OP_TRUE;        
                    break;
                     */
                case KEY_9:
                    key++;
                case KEY_8:
                    key++;
                case KEY_7:
                    key++;
                case KEY_6:
                    key++;
                case KEY_5:
                    key++;
                case KEY_4:
                    key++;
                case KEY_3:
                    key++;
                case KEY_2:
                    key++;
                case KEY_1:
                    key++;
                case KEY_0:
                    if( !(pTextInput->gadgetData.propertyMask & MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM)
                        && pTextInput->gadgetData.cbStateChange )
                    {
                        (void)pTextInput->gadgetData.cbStateChange(pTextInput->gadgetData.modId, textinputHandle, MsfNotifyClick, (void*)key);
                    }
                    break;
                default: 
                    *handle = OP_TRUE;
                    break;
                }
            break;
        case OPUS_TIMER_TXT_INPUT_CURSOR_BLINK:   
#ifdef _TOUCH_PANEL 
            widgetImmChangeCursorState();
#endif            
            widgetDrawTextInputCaret(pTextInput, OP_TRUE);
            *handle = OP_TRUE;
            break;

#ifdef _TOUCH_PANEL
        case OPUS_FOCUS_PEN:
        {
            MsfGadget   *pGadget = (MsfGadget*)pTextInput;
            TP_PEN_EVENT_STRUCTURE_T *pPen = (TP_PEN_EVENT_STRUCTURE_T *)pMess;
            
            /* only handle pendown event */
            /*
            *  Handle if the user wanna locate cursor or handwriting.
            */
          
            if ( pGadget->propertyMask & MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM )
            {
                if(pPen->state == TP_PEN_STATE_PENDN)
                {
                    LCD_POINT_T pt_start;
                    LCD_POINT_T pt_clicking;
                    DS_FONTATTR font_attr;

                    {
                        MsfPosition pos;
                        MsfSize size;
                        widgetTextInputGetEditArea(pTextInput, &pos, &size);
                        pt_start.x = pos.x;
                        pt_start.y = pos.y;
                    }
                    pt_clicking.x = pPen->x;
                    pt_clicking.y = pPen->y;

                    widgetGetDrawingInfo(pTextInput, MSF_TEXTINPUT, textinputHandle, &font_attr, OP_NULL, OP_NULL);
                    
                    bRet = TBM_SetCursor(&(pInput->txtInputBufMng), pt_start, pt_clicking, font_attr);
                }
            }           
            else if (MSF_GADGET_PROPERTY_PEN_CLICK & pGadget->propertyMask )
            {
                if( (pPen->state == TP_PEN_STATE_PENUP)
                    && (getWidgetType(pGadget->parent->windowHandle) != MSF_EDITOR ))
                {
                    return WIDGET_PEN_EVENT_TO_OK;
                }
            }    
            break;
        }
#endif        
        default:
            break;
    }
 
    if(bRet || bTextChanged)
    {
        widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, textinputHandle, OP_TRUE);
        *handle = OP_TRUE;
    }

    if( bTextChanged 
      && (pTextInput->gadgetData.propertyMask & MSF_GADGET_PROPERTY_NOTIFY))
    {
        if( pTextInput->gadgetData.cbStateChange )
        {
            (void)pTextInput->gadgetData.cbStateChange(pTextInput->gadgetData.modId, textinputHandle, MsfNotifyStateChange, OP_NULL );
        }
        else
        {
            TPIc_widgetNotify( pTextInput->gadgetData.modId, textinputHandle, MsfNotifyStateChange );
        }
    }
    
    if(event == OPUS_FOCUS_KEYPAD && ((KEYEVENT_STRUCT *)pMess)->state != UHKEY_RELEASE) 
    {
        if(*handle == OP_FALSE)
        {
            iLastKeyPressedCode = 0;
        }
        else
        {
            iLastKeyPressedCode = pKey->code;
        }
    }
    
    return TPI_WIDGET_OK;
}

static int stringgadget_event_handler 
(
    OP_UINT32 stringgadgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    MsfStringGadget       *pStringGadget;
    MsfWidgetType      wt;
    KEYEVENT_STRUCT *pKey = OP_NULL;
    OP_BOOLEAN         bRedraw;
    int                       iRet = TPI_WIDGET_OK;
 
    pStringGadget = (MsfStringGadget*)seekWidget(stringgadgetHandle, &wt);
    if(!pStringGadget || wt != MSF_STRINGGADGET)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    *handle = OP_FALSE;
    bRedraw = OP_FALSE;
    
    switch(event)
    {
        case OPUS_FOCUS_KEYPAD:
            pKey = (KEYEVENT_STRUCT *)pMess;   
            if(pKey->state == UHKEY_RELEASE) 
                break;     
                
            switch(pKey->code)
            {   
                case KEY_LEFT:
                    if( pStringGadget->singleLine )
                    {
                        if(pStringGadget->iCurPageIndex > 0 )
                        {
                            pStringGadget->iCurPageIndex --;
                            *handle = OP_TRUE;
                            bRedraw = OP_TRUE;
                        }
                    }
                    break;
                case KEY_RIGHT:
                    if( pStringGadget->singleLine )
                    {
                        if(pStringGadget->iCurPageIndex < (pStringGadget->nPageCount -1))
                        {
                            pStringGadget->iCurPageIndex ++;
                            *handle = OP_TRUE;
                            bRedraw = OP_TRUE;
                        }
                    }
                    break;
                case KEY_UP:
                case KEY_SIDE_UP:
                
                    if( pStringGadget->singleLine == 0 )
                    {
                        if(pStringGadget->iCurPageIndex > 0 )
                        {
                            pStringGadget->iCurPageIndex --;
                            *handle = OP_TRUE;
                            bRedraw = OP_TRUE;
                        }
                    }
                    break;
                case KEY_DOWN: 
                case KEY_SIDE_DOWN: 
                    if( pStringGadget->singleLine == 0 )
                    {
                        if(pStringGadget->iCurPageIndex < (pStringGadget->nPageCount -1))
                        {
                            pStringGadget->iCurPageIndex ++;
                            *handle = OP_TRUE;
                            bRedraw = OP_TRUE;
                        }
                    }
                    break;
                default:
                    break;
                }
            break;
#ifdef _TOUCH_PANEL
        case OPUS_FOCUS_PEN:
            bRedraw = widgetStringGadgetPenEventHandler( pStringGadget, (TP_PEN_EVENT_STRUCTURE_T*)pMess, *handle);
            if( bRedraw == OP_FALSE )
            {
                iRet = WIDGET_PEN_EVENT_TO_OK;
            }
            
            *handle = OP_TRUE;
            break;
#endif            
        default:
            break;
    }
 
    if( bRedraw == OP_TRUE )
    {
        widgetRedraw( (void*)pStringGadget, MSF_STRINGGADGET, 0, OP_TRUE);
    }

    return iRet;
}

static int imagegadget_event_handler 
(
    OP_UINT32 imagegadgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
#if 0
    /* if it is a multi-line string, handle the "up", "down" events 
     * if the image width is larger than the screen width, handle "left", "right" events
     */
    KEYEVENT_STRUCT *pKey;
    MsfImageGadget  *pImageGadget;
    MsfWidgetType  wt;
    MsfGadget       *pGadget;
    MsfImage        *pImage;
    TP_PEN_EVENT_STRUCTURE_T   *pPen;
    MsfSize           size;

    *handle  = OP_FALSE;
    pImageGadget = (MsfImageGadget*)seekWidget( imagegadgetHandle, &wt);
    if( !pImageGadget || wt != MSF_IMAGEGADGET )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pGadget = (MsfGadget *)pImageGadget; 
    pImage = (MsfImage*)seekWidget( pImageGadget->image, &wt);
    if( !pImage  || wt != MSF_IMAGE )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    widgetGadgetGetSize(pGadget, &size);

    switch(event)
    {
         /* here also add handle left or right key envent in future*/
        /*se OPUS_TIMER_GIF_ANIMATION:
        {
            widgetGadgetGetPosition( pGadget, &pos.x, &pos.y);
            
            playGifAnimation(pGadget->parent,
                            &pos,
                            &size,
                            pImage, 
                    MsfImageZoom100); 
        
            WAP_REFRESH
            startGifTimer(pImage->handle, 500);
            pImage->gifFlag |= GIF_TIMER_SET;
            *handle  = OP_TRUE;
        }
        break;*/

        case OPUS_FOCUS_KEYPAD:
        {
            pKey = (KEYEVENT_STRUCT *)pMess;   
            if(pKey->state == UHKEY_RELEASE)
            {
                break;
            } 
        }
        break;
        
        case OPUS_FOCUS_PEN:
        {
            pPen = (TP_PEN_EVENT_STRUCTURE_T *)pMess;
            /* only handle pendown event */
            if(pPen->state == TP_PEN_STATE_PENDN)
            {
                /* when gadget size is larger than image or is predefine icon,
                  * needn't scroll
                  */
                if((size.width < pImage->size.width)
                    ||(size.height < pImage->size.height))
                {
                    image_scroll(pGadget, pImage, pPen->x, pPen->y);
                }
                
                *handle = OP_TRUE;
            }
        }

        break;

        default:
            break;
    }
#endif
    return TPI_WIDGET_OK;
}

static int button_event_handler 
(
    OP_UINT32 hButton, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
    MsfGadget            *pGadget;
    MsfWidgetType      wt;
 
    pGadget = (MsfGadget*)seekWidget(hButton, &wt);
    if(!pGadget || wt != MSF_BUTTON)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    *handle = OP_FALSE;
    
#ifdef _KEYPAD
    if( event == OPUS_FOCUS_KEYPAD )
    {
        /*  handle the OK press event  */
        KEYEVENT_STRUCT *pKey = (KEYEVENT_STRUCT *)pMess;   
        if(pKey->state == UHKEY_RELEASE || pKey->code != KEY_OK ) 
            return TPI_WIDGET_OK; 

        if( pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
        {
            if(pGadget->cbStateChange )
            {
                pGadget->cbStateChange( pGadget->modId, hButton, MsfNotifyClick, OP_NULL );
            }
            else
            {
                TPIc_widgetNotify( pGadget->modId, hButton, MsfNotifyClick );
            }
            
            *handle = OP_TRUE;
        }
    }
    else
#endif

#ifdef _TOUCH_PANEL
    if( event == OPUS_FOCUS_PEN )
    {
        TP_PEN_EVENT_STRUCTURE_T  *pPen = (TP_PEN_EVENT_STRUCTURE_T*)pMess;
        *handle = OP_TRUE;
        
        if((pPen->state == TP_PEN_STATE_PENUP )
           && ( pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY ))
        {
            if( POINT_IN_RECT(*pPen, pGadget->position, pGadget->size))
            {
                return WIDGET_PEN_EVENT_TO_OK;
            } 
            else    /* the point of pen up not in button area, notify the app with MsfNotifyPenMoveOutUp */
            {
                if( pGadget->cbStateChange )
                {
                    (void)pGadget->cbStateChange(pGadget->modId, pGadget->gadgetHandle, MsfNotifyPenMoveOutUp, OP_NULL);
                }
#if 0    /* avoid the app not handling the MsfNotifyPenMoveOutUp notification */       
                else
                {
                    TPIc_widgetNotify( pGadget->modId, pGadget->gadgetHandle, MsfNotifyPenMoveOutUp );
                }
#endif                
            }
        }
    }
#else
    ;
#endif

    return TPI_WIDGET_OK;
}


static int bar_event_handler 
(
    OP_UINT32 barHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN   *handle
)
{
#ifdef _TOUCH_PANEL
    MsfBar         *pBar;
    MsfWidgetType   widgetType;
    TP_PEN_EVENT_STRUCTURE_T   *pPen;
    SCROLLBAR_NOTIFICATION_T     op;

    if(!barHandle || !pMess)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    /* till now, *handle == OP_FALSE */
    pBar = (MsfBar *)seekWidget(barHandle, &widgetType);
    if (pBar == OP_NULL || widgetType != MSF_BAR
      ||(pBar->barType != MsfVerticalScrollBar && pBar->barType != MsfHorizontalScrollBar)
      || pBar->maxValue <= 0 )
    {
        return TPI_WIDGET_OK;
    }
    
    if(event == OPUS_FOCUS_PEN)
    {
        pPen = (TP_PEN_EVENT_STRUCTURE_T *)pMess;
        /* only handle pendown event */
        if(pPen->state == TP_PEN_STATE_PENDN)
        {
            if( pBar->value > pBar->maxValue )
            {
                pBar->value = pBar->maxValue;
            }
            
            op = get_scrollbar_handle_area(pBar, pPen->x, pPen->y);
             widgetScrollbarScroll(pBar, op);

            *handle = OP_TRUE;
        }
    }
#endif

    return TPI_WIDGET_OK;
}

/* the return value indicates whether the display need to be refreshed */
static KEYEVENT_STRUCT key_press;
/* direct number selection support */
static OP_BOOLEAN        direct_select_timer_on = OP_FALSE;
static OP_UINT16          pressed_key_value = 0;

static int choice_event_handler
(
    MsfSelectgroup              *pSelectGroup, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN               *handle
)
{
    int                         cur_item;
    OP_UINT8               key_val;     
    OP_BOOLEAN            map_key_to_index = OP_FALSE;
    KEYEVENT_STRUCT   *pKey;
    OP_BOOLEAN           bScrolled = OP_FALSE;
    MsfWidgetType        wtWin;
    int iRet = WIDGET_EVENT_NOT_HANDLE;
    MsfWidgetType gadgetType;

    if( !pSelectGroup || handle == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    wtWin = getWidgetType(pSelectGroup->gadgetData.parent->windowHandle);
    gadgetType = getWidgetType(pSelectGroup->gadgetData.gadgetHandle);

    if (pSelectGroup->choice.count <= 0)
    {
        return iRet;
    }

        
    /* till now, *handle == OP_FALSE */
    *handle = OP_TRUE;
    switch (event)
    {
        case OPUS_TIMER_KEYPRESS_REPEAT:
            if(key_press.state == UHKEY_RELEASE)
                break;
            if (is_digit_key(((KEYEVENT_STRUCT *)pMess)->code) == OP_FALSE)
            {
                break;
            }
            /*********************************************************
            Important Notice:
            
            To reduce code size, we just re-create a pMess here and do
            not place 'break' in this case. then the code will fall into
            next event: case OPUS_UH_KEYPAD, and the code in that message
            is used to process the repeat key message.

            p_repeat_key will be freed in processing of OPUS_UH_KEYPAD,
            .
            **********************************************************/
            ///
            //DON'T PLACE 'break;' HERE, DON'T INSERT OTHER EVENTS HERE!!!
            ///
        case OPUS_FOCUS_KEYPAD:
            pKey = (KEYEVENT_STRUCT *)pMess;
            if (event == OPUS_FOCUS_KEYPAD)
            {
                op_memcpy(&key_press, pMess, sizeof(KEYEVENT_STRUCT));
                if (pKey->state == UHKEY_RELEASE)
                {
                    *handle = OP_FALSE;
                    break;
                }
            }
            
            /* if the diect select timer was started but the key after that is not a digit key, clear the timer and variables */
            /* the digit key codes are not consecutive, they need to checked */
            if ( (direct_select_timer_on) &&
                 (is_digit_key(key_press.code) == OP_FALSE))
            {
                choice_reset_direct_number();
            }

            switch (pKey->code)
            {
                case KEY_0:  /* fall through */
                case KEY_1:  /* fall through */
                case KEY_2:  /* fall through */
                case KEY_3:  /* fall through */
                case KEY_4:  /* fall through */
                case KEY_5:  /* fall through */
                case KEY_6:  /* fall through */
                case KEY_7:  /* fall through */
                case KEY_8:  /* fall through */
                case KEY_9:  /* fall through */
                    /* direct selecting */
                    if(gadgetType == MSF_BOX)
                    {
                       break;
                    }
                    key_val = KeyToDigit(key_press.code);
                    
                    /* is this first digit key press ? */
                    if ( direct_select_timer_on == OP_FALSE ) 
                    {
                        /* do not allow 0 as the first key press */
                        if ( key_val != 0)
                        {
                            /* start timer only when the the number of items is larger than or equal to 10
                               and the first pressed key is smaller or equal to the 10's digit of the total item number */
                            if ((pSelectGroup->choice.count >= 10) && 
                                (key_val <= (pSelectGroup->choice.count /10)))
                            {
                                OPUS_Start_Timer(OPUS_TIMER_DIRECT_NUM_SELECT, 750, 0,  ONE_SHOT);
                                direct_select_timer_on = OP_TRUE;
                                pressed_key_value = key_val;
                            }
                            else /* no need to start timer. Do direct number selection right away. */
                            {
                                map_key_to_index = OP_TRUE;
                                pressed_key_value = key_val;
                            }
                        } 
                    } 
                    else  /* timer is already on */
                    {
                        OPUS_Stop_Timer(OPUS_TIMER_DIRECT_NUM_SELECT);
                        direct_select_timer_on = OP_FALSE;
                        pressed_key_value = pressed_key_value * 10 + key_val ;                   
                        map_key_to_index = OP_TRUE;
                    }

                    if (map_key_to_index)
                    {
                        /* make key_val index ranged 0 ~ (the number of item is less than 10 -1) */
                        pressed_key_value --;

                        if ( pressed_key_value < pSelectGroup->choice.count)
                        {
                            if( pSelectGroup->choice.cur_item != (int)pressed_key_value )
                            {
                                pSelectGroup->choice.cur_item    = pressed_key_value;    
                                if (pSelectGroup->choice.cur_item < pSelectGroup->choice.top_item)
                                {
                                    pSelectGroup->choice.top_item = pSelectGroup->choice.cur_item;
                                    bScrolled = OP_TRUE;
                                }
                                else if (pSelectGroup->choice.cur_item >= pSelectGroup->choice.top_item + pSelectGroup->choice.elementsPerPage)
                                {
                                    bScrolled = OP_TRUE;
                                    pSelectGroup->choice.top_item = MIN(pSelectGroup->choice.cur_item, pSelectGroup->choice.count - pSelectGroup->choice.elementsPerPage);
                                }
                            }
                            /*
                            * If the choice is menu, or it is MsfExclusiveChoice type selectgroup, this event should be 
                            * mapped to a MsfSelect action, so pass it to the window
                            */
                            if ( wtWin == MSF_MENU
                              &&(pSelectGroup->choice.type == MsfImplicitChoice || 
                              pSelectGroup->choice.type == MsfExclusiveChoice))
                            {
                                pKey->code = KEY_OK;                    
                            }    
                            /* !!!!! don't break here. Fall through intentionally !!!!! */    
                        }
                        else
                        {
                            pressed_key_value = 0;
                            break;
                        }
                    } 
                    else
                    {
                        break;
                    }

                case KEY_OK:
                    if( pKey->code == KEY_OK
                         && ( pSelectGroup->gadgetData.propertyMask & MSF_GADGET_PROPERTY_SG_DO_NOT_HANDLE_OK ))
                    {
                        *handle = OP_FALSE;
                        break;
                    }
                    
                    /*
                     * Update the state of current element. 
                     * If the choice type is MsfImplicitChoice or MsfExclusiveChoice,
                     * there may be an select action mapped to this event,
                     * so this event should be passed to the action_handler
                     */
                    if( choice_select_focus(pSelectGroup, bScrolled) == OP_TRUE )
                    {
                        if( wtWin == MSF_MENU )
                        {
                            *handle = OP_FALSE;
                        }
                        else
                        {
                            /* need to notify  */
                            iRet = WIDGET_EVENT_HANDLED_NEED_NOTIFY;
                        }
                    }
                    else if( wtWin != MSF_MENU )
                    {
                        *handle = OP_FALSE;
                    }
                    break;

                case KEY_LEFT:
                    if( wtWin != MSF_MENU && wtWin != MSF_MAINMENU )
                    {
                        /* need to support flip-page */
                        if(pSelectGroup->gadgetData.propertyMask & MSF_GADGET_PROPERTY_FLIP_PAGE_BY_LR_KEY )
                        {
                            iRet = WIDGET_EVENT_HANDLED_REQ_PRE_PAGE_NOTIFY;
                        }
                        else
                        {
                            *handle = OP_FALSE;
                        }
                        break;
                    }
                case KEY_UP:
                case KEY_SIDE_UP:
                    cur_item = pSelectGroup->choice.cur_item;
                    iRet = choice_scroll_up(pSelectGroup);
                    if(!(iRet & WIDGET_EVENT_HANDLED) )
                    {
                        *handle = OP_FALSE;
                    }

                    /* need handle focus change between items */
                    if( cur_item != pSelectGroup->choice.cur_item 
                    && (MSF_GADGET_PROPERTY_NOTIFY_ITEM_FOCUS_CHANGE & pSelectGroup->gadgetData.propertyMask) )
                    {
                        iRet |= WIDGET_EVENT_HANDLED_NEED_NOTIFY;
                    }

                    break;
                
                case KEY_RIGHT:
                    if( wtWin != MSF_MENU && wtWin != MSF_MAINMENU )
                    {
                        /* need to support flip-page */
                        if( pSelectGroup->gadgetData.propertyMask & MSF_GADGET_PROPERTY_FLIP_PAGE_BY_LR_KEY )
                        {
                            iRet = WIDGET_EVENT_HANDLED_REQ_NEXT_PAGE_NOTIFY;
                        }
                        else
                        {
                            *handle = OP_FALSE;
                        }
                        break;
                    }
                case KEY_DOWN:
                case KEY_SIDE_DOWN:
                    cur_item = pSelectGroup->choice.cur_item;
                    iRet = choice_scroll_down(pSelectGroup);                    
                    if(!(iRet & WIDGET_EVENT_HANDLED) )
                    {
                        *handle = OP_FALSE;
                    }

                    /* need handle focus change between items */
                    if( cur_item != pSelectGroup->choice.cur_item 
                    && (MSF_GADGET_PROPERTY_NOTIFY_ITEM_FOCUS_CHANGE & pSelectGroup->gadgetData.propertyMask) )
                    {
                        iRet |= WIDGET_EVENT_HANDLED_NEED_NOTIFY;
                    }

                    break;
                
                default: 
                    *handle = OP_FALSE;
                    break;
            }
            break;

        case OPUS_TIMER_DIRECT_NUM_SELECT: /* receive this event if direct number selection timer was started */
            if (direct_select_timer_on)
            {
                if ( --pressed_key_value < pSelectGroup->choice.count)
                {
                    if( pSelectGroup->choice.cur_item != (int)pressed_key_value )
                    {
                        int           iOldFocusIndex = pSelectGroup->choice.cur_item;
                        choiceElement *pOldFocusItem, *pItem;
                        pOldFocusItem = getChoiceElementByIndex(&pSelectGroup->choice, pSelectGroup->choice.cur_item);
                        if( pOldFocusItem )
                        {
                            pOldFocusItem->state &= ~MSF_CHOICE_ELEMENT_FOCUSED;
                        }
                        
                        pItem = getChoiceElementByIndex(&pSelectGroup->choice, (int)pressed_key_value);
                        if( pItem )
                        {
                            pItem->state |= MSF_CHOICE_ELEMENT_FOCUSED;
                        }
                        
                        pSelectGroup->choice.cur_item = pressed_key_value;
                        if (pSelectGroup->choice.cur_item < pSelectGroup->choice.top_item)
                        {
                            pSelectGroup->choice.top_item = pSelectGroup->choice.cur_item;
                            bScrolled = OP_TRUE;
                        }
                        else if (pSelectGroup->choice.cur_item >= pSelectGroup->choice.top_item + pSelectGroup->choice.elementsPerPage)
                        {
                            bScrolled = OP_TRUE;
                            pSelectGroup->choice.top_item = MIN(pSelectGroup->choice.cur_item, pSelectGroup->choice.count - pSelectGroup->choice.elementsPerPage);
                        }

                        if( bScrolled == OP_FALSE )
                        {
                            if( pOldFocusItem )
                            {
                                widgetChoiceDrawItemByIndex( pSelectGroup, pOldFocusItem, (OP_INT16)iOldFocusIndex, OP_TRUE);
                            }

                            if( pItem )
                            {
                                widgetChoiceDrawItemByIndex( pSelectGroup, pItem, (OP_INT16)pressed_key_value, OP_TRUE);
                            }
                        }
                        else
                        {
                            widgetRedraw((void*)pSelectGroup, gadgetType, INVALID_HANDLE, OP_TRUE);
                        }
                    }
                }  
                choice_reset_direct_number(); 
                /*
                * If the choice is menu, or it is MsfExclusiveChoice type selectgroup, this event should be 
                * mapped to a MsfSelect action, so pass it to the window
                */
                if ( wtWin == MSF_MENU
                  &&(pSelectGroup->choice.type == MsfImplicitChoice || 
                  pSelectGroup->choice.type == MsfExclusiveChoice))
                {
                   KEYEVENT_STRUCT  keyMap;
                   MsfScreen *pScreen;
                   OP_BOOLEAN bHandle = OP_FALSE;
                   pScreen = (MsfScreen *)seekFocusedScreen();
                   if(OP_NULL != pScreen)
                   {
                       keyMap.code = KEY_OK;
                       keyMap.state = UHKEY_PRESS;
                       widgetEventHandler(pScreen, OPUS_FOCUS_KEYPAD, (void*)&keyMap, &bHandle);
                       keyMap.state = UHKEY_RELEASE;
                       widgetEventHandler(pScreen, OPUS_FOCUS_KEYPAD, (void*)&keyMap, &bHandle);                    
                   }
                 
                }                
            }
                break;


        case OPUS_TIMER_CHOICITEM_SCROLL: /* the item string scorll horizontally */
            if (pSelectGroup->choice.iItemVisibleLen < pSelectGroup->choice.iCurItemStringLen)
            {
                pSelectGroup->choice.iCurItemStringPos += 1;
                if (pSelectGroup->choice.iCurItemStringPos > pSelectGroup->choice.iCurItemStringLen + pSelectGroup->choice.iItemVisibleLen)
                {
                    pSelectGroup->choice.iCurItemStringPos  = 1;
                }

                widgetChoiceDrawItemByIndex( pSelectGroup, OP_NULL, (OP_INT16)pSelectGroup->choice.cur_item, OP_TRUE);
            }
            break;

        default:
            *handle = OP_FALSE;
            break;
    }    
    
    return iRet;
}

static int choice_scroll_up(MsfSelectgroup  *pSelectGroup)
{
    int                  iRet = WIDGET_EVENT_HANDLED;
    choiceElement    *pItem;
    choiceElement    *pOldFocusItem = OP_NULL;
    int                  iOldFocusItemIndex;
    OP_INT16        items_in_page;
    OP_BOOLEAN    bDrawAll = OP_TRUE;
    MsfChoice         *pChoice;
    MsfPosition       pos, clientStart;
    MsfSize            size, clientSize;
    OP_INT16         left, top, right, bottom;

    pChoice = &pSelectGroup->choice;
    
    items_in_page = MIN(pChoice->elementsPerPage, pChoice->count);

    /* set the state of previous focused item */
    pItem = getChoiceElementByIndex( pChoice,  pChoice->cur_item);
    if( pItem )
    {
        pItem->state &= (~MSF_CHOICE_ELEMENT_FOCUSED);
        pOldFocusItem = pItem;
        iOldFocusItemIndex = pChoice->cur_item;
    }
    else
    {
        pChoice->cur_item = 0;
    }

    /* focus is in visible */
    if ( (pChoice->cur_item >= pChoice->top_item) &&
         (pChoice->cur_item < pChoice->top_item + pChoice->elementsPerPage) )
    {
        /* if the previously focused item is at the top and there is a upper item, 
           the upper item will be the first item of the screen with focus. */
        if (pChoice->top_item > 0 && pChoice->top_item == pChoice->cur_item) 
        {
            pChoice->top_item--;
            pChoice->cur_item--;
        }
        else if (pChoice->cur_item == 0) 
        {
            if( items_in_page > 1 )
            {
                if (pChoice->scrollCircularly )
                {
                    if( getWidgetType(pSelectGroup->gadgetData.gadgetHandle) != MSF_BOX )
                    {
                        /* if the previously focused line is the first item,  
                           display the last page of the list and move the focus to the last item.*/
                        pChoice->top_item = pChoice->count - items_in_page;
                        pChoice->cur_item = pChoice->count - 1;
                        if(pChoice->elementsPerPage >= pChoice->count )  /* only one page */
                        {
                            bDrawAll = OP_FALSE;
                        }
                    }
                    else
                    {   
                        MsfBox  *pBox = (MsfBox*)pSelectGroup;
                        /* reach the first item, and back continue */
                        if( pBox->iCurPos <= 0 && pBox->nItemCount <= pChoice->elementsPerPage )
                        {
                            /* if the previously focused line is the first item,  
                               display the last page of the list and move the focus to the last item.*/
                            pChoice->top_item = pChoice->count - items_in_page;
                            pChoice->cur_item = pChoice->count - 1;
                            bDrawAll = OP_FALSE;
                        }
                        else
                        {
                            iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_PREVIOUS_NOTIFY;
                        }
                    }
                }
                else if( pChoice->count >= pChoice->elementsPerPage )
                {
                    iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_PREVIOUS_NOTIFY;
                }
                else
                {
                    iRet = WIDGET_EVENT_NOT_HANDLE ;
                }
            }
            else if( items_in_page == pChoice->elementsPerPage && 1 == items_in_page )
            {
                iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_PREVIOUS_NOTIFY;
            }
            else
            {
                iRet = WIDGET_EVENT_NOT_HANDLE ;
            }
        }
        else /* case of - if (pChoice->cur_item > 0) */
        { /* if the previously focused line is not in the top of the screen, 
             just move the focus to the upper line */
            pChoice->cur_item--;

            if( pChoice->cur_item  < pChoice->top_item )
            {
                pChoice->top_item = pChoice->cur_item;
            }
            else if( pChoice->cur_item >= (pChoice->top_item + pChoice->elementsPerPage ))
            {
                pChoice->top_item = pChoice->cur_item;
                if( (pChoice->cur_item + pChoice->elementsPerPage) > pChoice->count )
                {
                    pChoice->top_item = pChoice->count - pChoice->elementsPerPage;
                }
            }
            bDrawAll = OP_FALSE;
        } 
    }
    else    /* visible is not visible */
    {
         if (pChoice->cur_item == 0) 
         {
            if (pChoice->scrollCircularly )
            {
              /* if the previously focused line is the first item,  
                 display the last page of the list and move the focus to the last item.*/
                if( pChoice->top_item + pChoice->elementsPerPage == pChoice->count )  /* last page */
                {
                    bDrawAll = OP_FALSE;
                }
                else
                {
                    pChoice->top_item = pChoice->count - pChoice->elementsPerPage;
                }
                
                pChoice->cur_item = pChoice->count - 1;
            }
            else
            {
                iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_PREVIOUS_NOTIFY;
            }
         }
         else
         {   
             pChoice->cur_item--;

             if (pChoice->top_item+ pChoice->elementsPerPage == pChoice->cur_item + 1 )
             {
                bDrawAll = OP_FALSE;
             }
             else if( pChoice->cur_item  < pChoice->top_item )
             {
                 pChoice->top_item = pChoice->cur_item;
             }
             else if( pChoice->cur_item >= (pChoice->top_item + pChoice->elementsPerPage ))
             {
                 pChoice->top_item = pChoice->cur_item;
                 if( (pChoice->cur_item + pChoice->elementsPerPage) > pChoice->count )
                 {
                     pChoice->top_item = pChoice->count - pChoice->elementsPerPage;
                 }
             }
         }
    }
    /* set the state of the new focused item */
    pItem = getChoiceElementByIndex( pChoice,  pChoice->cur_item);
    if ( !pItem )
    {
        return WIDGET_EVENT_NOT_HANDLE;
    }
    pItem->state |= MSF_CHOICE_ELEMENT_FOCUSED;

    if (iRet &WIDGET_EVENT_HANDLED)
    {
        if( pChoice->bTimerStarted == OP_TRUE )
        {
            OPUS_Stop_Timer(OPUS_TIMER_CHOICITEM_SCROLL);
            pChoice->bTimerStarted = OP_FALSE;
        }
        
        if( pSelectGroup->gadgetData.parent != OP_NULL )
        {
            widgetGetClientRect( pSelectGroup->gadgetData.parent, &clientStart, &clientSize);
            if( 0 > widgetChoiceGetItemRect( pSelectGroup, (OP_INT16)pChoice->cur_item, &pos, &size))
            {
                return iRet;
            }
            
            left = pos.x;
            top = pos.y;
            right = left + size.width;
            bottom = top + size.height;
            if( OP_TRUE == widgetRectGetVisible(
                                            clientStart.x,
                                            clientStart.y,
                                            (OP_INT16)(clientStart.x + clientSize.width), 
                                            (OP_INT16)(clientStart.y + clientSize.height), 
                                            &left, 
                                            &top,
                                            &right,
                                            &bottom))
            {
                if( left != pos.x || top != pos.y || right != (left + size.width) || bottom != (top + size.height))
                {
                    /* need scroll the window */
                    return TPI_WIDGET_OK;
                }
            }
            else
            {
                /* need scroll the window */
                return TPI_WIDGET_OK;
            }
            
            if( bDrawAll == OP_TRUE)
            {
                widgetRedraw( &pSelectGroup->gadgetData, MSF_SELECTGROUP, INVALID_HANDLE, OP_TRUE );
            }
            else if( CHOICE_REDRAW_ALL_ITEMS != widgetChoiceDrawItemByIndex(
                      pSelectGroup, pItem, (OP_INT16)pSelectGroup->choice.cur_item, OP_TRUE) 
               && pOldFocusItem )
            {
                widgetChoiceDrawItemByIndex(
                            pSelectGroup, 
                            pOldFocusItem, 
                            (OP_INT16)iOldFocusItemIndex, 
                            OP_TRUE);
            }
        }        
    }
    return iRet;
}

static int choice_scroll_down(MsfSelectgroup  *pSelectGroup)
{
    int                    iRet = WIDGET_EVENT_HANDLED;
    OP_INT16          items_in_page;
    choiceElement     *pItem;
    choiceElement     *pOldFocusItem = OP_NULL;
    int                   iOldFocusItemIndex;
    OP_BOOLEAN     bDrawAll = OP_TRUE;
    MsfChoice          *pChoice;
    MsfPosition       pos, clientStart;
    MsfSize            size, clientSize;
    OP_INT16         left, top, right, bottom;

    pChoice = &pSelectGroup->choice;
    
    items_in_page = MIN(pChoice->elementsPerPage, pChoice->count);
    
    /* set the state of previous focused item */
    pItem = getChoiceElementByIndex( pChoice,  pChoice->cur_item);
    if( pItem )
    {
        pItem->state &= (~MSF_CHOICE_ELEMENT_FOCUSED);
        pOldFocusItem = pItem;
        iOldFocusItemIndex = pChoice->cur_item;
    }
    else
    {
        pChoice->cur_item = 0;
    }
    
    if ( (pChoice->cur_item >= pChoice->top_item) &&
         (pChoice->cur_item < pChoice->top_item + pChoice->elementsPerPage) )
    {
        /* if the previously focused item was at the bottom of the screen and more item below, 
           the next item will be bottom item of the screen with focus. */
        if ((pChoice->top_item < pChoice->count - items_in_page) &&
            (pChoice->cur_item == pChoice->top_item + items_in_page-1) ) 
        {
            pChoice->top_item++;
            pChoice->cur_item++;
        }
        else if (pChoice->cur_item == (pChoice->count -1)) 
        {
            if( items_in_page > 1 )
            {
                if (pChoice->scrollCircularly)
                {
                    if( getWidgetType(pSelectGroup->gadgetData.gadgetHandle) != MSF_BOX )
                    {
                        /* if the previously focused line is the last item,  
                        display the first page of the list and move the focus to the first item.*/
                        pChoice->top_item = 0;
                        pChoice->cur_item = 0;
                        if(pChoice->elementsPerPage >= pChoice->count)  /* only one page */
                        {
                            bDrawAll = OP_FALSE;
                        }
                    }
                    else
                    {
                        MsfBox  *pBox = (MsfBox*)pSelectGroup;
                        /* reach the last item, and back continue */
                        if( pBox->iCurPos <= 0 && pBox->nItemCount <= pChoice->elementsPerPage )
                        {
                            /* if the previously focused line is the last item,  
                            display the first page of the list and move the focus to the first item.*/
                            pChoice->top_item = 0;
                            pChoice->cur_item = 0;
                            bDrawAll = OP_FALSE;
                        }
                        else
                        {
                            iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_NEXT_NOTIFY;
                        }
                    }
                }
                else if( pChoice->count >= pChoice->elementsPerPage )
                {
                    iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_NEXT_NOTIFY;
                }
                else
                {
                    iRet = WIDGET_EVENT_NOT_HANDLE ;
                }
            }
            else if( items_in_page == pChoice->elementsPerPage && 1 == items_in_page )
            {
                iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_NEXT_NOTIFY;
            }
            else
            {
                iRet = WIDGET_EVENT_NOT_HANDLE ;
            }
        }
        else /* case of - if (pChoice->cur_item < (pChoice->count-1)) */
        { /* if the previously focused line is not in the bottom line of the screen, 
             just move the focus to the next line */
            pChoice->cur_item++;

            bDrawAll = OP_FALSE;

            if( pChoice->cur_item  < pChoice->top_item )
            {
                pChoice->top_item = pChoice->cur_item;
            }
            else if( pChoice->cur_item >= (pChoice->top_item + pChoice->elementsPerPage ))
            {
                pChoice->top_item = pChoice->cur_item;
                if( (pChoice->cur_item + pChoice->elementsPerPage) > pChoice->count )
                {
                    pChoice->top_item = pChoice->count - pChoice->elementsPerPage;
                }
            }
        }
    }
    else
    {
        if (pChoice->cur_item == (pChoice->count -1))
        {
            if (pChoice->scrollCircularly )
            {
              /* if the previously focused line is the first item,  
                 display the last page of the list and move the focus to the last item.*/
                if( pChoice->top_item  == 0 )  /* first page */
                {
                    bDrawAll = OP_FALSE;
                }
                else
                {
                    pChoice->top_item = 0;
                }
                pChoice->cur_item = 0;
            }
            else
            {
                iRet = WIDGET_EVENT_NOT_HANDLE | WIDGET_EVENT_HANDLED_REQ_PREVIOUS_NOTIFY;
            }
         }
         else
         {
             pChoice->cur_item++;
             if( pChoice->cur_item == pChoice->top_item )
             {
                bDrawAll = OP_FALSE;
             }
             else if( pChoice->cur_item  < pChoice->top_item )
             {
                 pChoice->top_item = pChoice->cur_item;
             }
             else if( pChoice->cur_item >= (pChoice->top_item + pChoice->elementsPerPage ))
             {
                 pChoice->top_item = pChoice->cur_item;
                 if( (pChoice->cur_item + pChoice->elementsPerPage) > pChoice->count )
                 {
                     pChoice->top_item = pChoice->count - pChoice->elementsPerPage;
                 }
             }
         }
    }
    /* set the state of the new focused item */
    pItem = getChoiceElementByIndex( pChoice,  pChoice->cur_item);
    if ( !pItem )
    {
        return WIDGET_EVENT_NOT_HANDLE;
    }    
    pItem->state |= MSF_CHOICE_ELEMENT_FOCUSED;

    if (iRet &WIDGET_EVENT_HANDLED)
    {
        if( pChoice->bTimerStarted == OP_TRUE )
        {
            OPUS_Stop_Timer(OPUS_TIMER_CHOICITEM_SCROLL);
            pChoice->bTimerStarted = OP_FALSE;
        }

        if( pSelectGroup->gadgetData.parent != OP_NULL )
        {
            widgetGetClientRect( pSelectGroup->gadgetData.parent, &clientStart, &clientSize);
            if( 0 > widgetChoiceGetItemRect( pSelectGroup, (OP_INT16)pChoice->cur_item, &pos, &size))
            {
                return iRet;
            }
            
            left = pos.x;
            top = pos.y;
            right = left + size.width;
            bottom = top + size.height;
            if( OP_TRUE == widgetRectGetVisible(
                                            clientStart.x,
                                            clientStart.y,
                                            (OP_INT16)(clientStart.x + clientSize.width), 
                                            (OP_INT16)(clientStart.y + clientSize.height), 
                                            &left, 
                                            &top,
                                            &right,
                                            &bottom))
            {
                if( left != pos.x || top != pos.y || right != (left + size.width) || bottom != (top + size.height))
                {
                    /* need scroll the window */
                    return TPI_WIDGET_OK;
                }
            }
            else
            {
                /* need scroll the window */
                return TPI_WIDGET_OK;
            }
            
            if( bDrawAll == OP_TRUE)
            {
                widgetRedraw( &pSelectGroup->gadgetData, MSF_SELECTGROUP, INVALID_HANDLE, OP_TRUE );
            }
            else if( CHOICE_REDRAW_ALL_ITEMS != widgetChoiceDrawItemByIndex(
                      pSelectGroup, pItem, (OP_INT16)pSelectGroup->choice.cur_item, OP_TRUE) 
               && pOldFocusItem )
            {
                widgetChoiceDrawItemByIndex(
                            pSelectGroup, 
                            pOldFocusItem, 
                            (OP_INT16)iOldFocusItemIndex, 
                            OP_TRUE);
            }
        }
    }
    return iRet;
}


static OP_BOOLEAN choice_select_focus(MsfSelectgroup  *pSelectGroup, OP_BOOLEAN bScrolled)
{
    choiceElement     *pItem;
    choiceElement     *pOldFocusItem;
    choiceElement     *pNewFocusItem = OP_NULL;
    OP_INT16          iOldFocusItemIndex;
    OP_INT16         index;
    OP_BOOLEAN     bNeedDrawAllItems = bScrolled;
    OP_BOOLEAN     bStateChange = OP_TRUE;
    MsfChoice          *pChoice;

    pChoice = &pSelectGroup->choice;
    
    if (pSelectGroup->choice.count == 0)
    {
        return OP_FALSE;
    }
    
    pItem = pChoice->firstElement;
    index = 0;
    pOldFocusItem = OP_NULL;
    
    while (pItem && index < pChoice->count)
    {
        if (index != pChoice->cur_item)
        {
            /* clear previous focused item state */
            if (pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            {
                pItem->state &= (~MSF_CHOICE_ELEMENT_FOCUSED);
                pOldFocusItem = pItem;
                iOldFocusItemIndex = index;
            }
            
            /* clear previous selected item state */
            if ((pChoice->type != MsfMultipleChoice) &&
                (pItem->state & MSF_CHOICE_ELEMENT_SELECTED))
            {
                pItem->state &= (~MSF_CHOICE_ELEMENT_SELECTED);
                if( pOldFocusItem != pItem )
                {
                    bNeedDrawAllItems = OP_TRUE;
                }
            }
        }
        else
        {
            pItem->state |= MSF_CHOICE_ELEMENT_FOCUSED;
            if (pChoice->type == MsfMultipleChoice)
            {
                if (pItem->state & MSF_CHOICE_ELEMENT_DISABLED)
                {
                    /* need not do anything here */
                    bStateChange = OP_FALSE;
                }
                else if (pItem->state & MSF_CHOICE_ELEMENT_SELECTED)
                {
                    pItem->state &= (~MSF_CHOICE_ELEMENT_SELECTED);
                }
                else
                {
                    pItem->state |= MSF_CHOICE_ELEMENT_SELECTED;
                }
            }
            else  /* pChoice->type != MsfMultipleChoice */
            {
                if (pItem->state & MSF_CHOICE_ELEMENT_DISABLED)
                {
                    bStateChange = OP_FALSE;
                }
                else
                {
                    if( pItem->state & MSF_CHOICE_ELEMENT_SELECTED )
                    {
                        if( pChoice->type == MsfExclusiveChoice )
                        {
                            bStateChange = OP_FALSE;
                        }
                    }
                    else
                    {
                        pItem->state |= MSF_CHOICE_ELEMENT_SELECTED;
                    }
                }
            }

            pNewFocusItem = pItem;
        }
        pItem = pItem->next;
        index++;
    }


    if( bNeedDrawAllItems == OP_TRUE)
    {
        widgetRedraw( &pSelectGroup->gadgetData, MSF_SELECTGROUP, INVALID_HANDLE, OP_TRUE );
    }
    else
    {
        if( pOldFocusItem ) /* use digit to select */
        {
            if ( pChoice->bTimerStarted == OP_TRUE)
            {
                OPUS_Stop_Timer(OPUS_TIMER_CHOICITEM_SCROLL);
                pChoice->bTimerStarted = OP_FALSE;
            }
            
            if( CHOICE_REDRAW_ALL_ITEMS != widgetChoiceDrawItemByIndex(
                  pSelectGroup, pNewFocusItem, (OP_INT16)pSelectGroup->choice.cur_item, OP_TRUE) )
            {
                widgetChoiceDrawItemByIndex(
                            pSelectGroup, 
                            pOldFocusItem, 
                            (OP_INT16)iOldFocusItemIndex, 
                            OP_TRUE);
            }
        } 
        else if( bStateChange == OP_TRUE ) /* press OK to select */
        {
            widgetChoiceDrawItemByIndex(
                          pSelectGroup,
                          pNewFocusItem,
                          (OP_INT16)pSelectGroup->choice.cur_item,
                          OP_TRUE);
        }
    }
    return bStateChange;
}

static void choice_reset_direct_number
(
    void
)
{
    direct_select_timer_on = OP_FALSE;
    OPUS_Stop_Timer(OPUS_TIMER_DIRECT_NUM_SELECT);
    pressed_key_value = 0;
}

static OP_BOOLEAN is_digit_key
(
    OP_UINT16 key
)
{
    OP_BOOLEAN ret_state = OP_FALSE;

    /* digit key codes are not consecutive each other. Range checking does not work here.  */
    if ( ( key == KEY_0) ||
          ( key == KEY_1) ||
          ( key == KEY_2) ||
          ( key == KEY_3) ||
          ( key == KEY_4) ||
          ( key == KEY_5) ||
          ( key == KEY_6) ||
          ( key == KEY_7) ||
          ( key == KEY_8) ||
          ( key == KEY_9))
    {
        ret_state = OP_TRUE;
    }

    return (ret_state);
}

static OP_BOOLEAN widgetScrollbarScroll
(
    MsfBar *pBar, 
    SCROLLBAR_NOTIFICATION_T op
)
{
    int  oldValue;
    
    if( !pBar
    ||(pBar->barType != MsfVerticalScrollBar && pBar->barType != MsfHorizontalScrollBar )
    ||(pBar->maxValue <= 0)
    ||(op > SB_NOTIFY_PAGE_DOWN || op < SB_NOTIFY_LINE_UP ))
    {
        return SB_NOTIFY_INVALID;
    }

    oldValue = pBar->value;
    
    switch( op )
    {
        case SB_NOTIFY_LINE_UP:
            pBar->value -= pBar->lineStep ;
            break;
        case SB_NOTIFY_LINE_DOWN:
            pBar->value += pBar->lineStep ;
            break;
        case SB_NOTIFY_PAGE_UP:
            pBar->value -= pBar->pageStep;
            break;
        case SB_NOTIFY_PAGE_DOWN:
            pBar->value += pBar->pageStep;
            break;
        default:
            break;
    }

    widgetBarAdjustValue(pBar);
    
    if( oldValue != pBar->value )
    {
        if( TPI_WIDGET_PAINTED != widgetScrollBarNotify( pBar, op ) )
        {
            widgetRedraw( (void*)pBar, MSF_BAR, INVALID_HANDLE, OP_TRUE);
        }
    
        return OP_TRUE;
    }
    
    return OP_FALSE;
}


/* scrollbar handling area map 
   |^|-------|---|-------|^|
   |A|---C---|-E-|---D---|B|
   A~ area for line up;                               LINEUP_AREA
   B~ area for line down;                             LINEDOWN_AREA,
   C~ area for page up;                               PAGEUP_AREA,
   D~ area for page down;                             PAGEDOWN_AREA,
   E~ area for spit, no response when was touched.    SPLIT_AREA

*/
static SCROLLBAR_NOTIFICATION_T get_scrollbar_handle_area(MsfBar *pBar, OP_INT16 x, OP_INT16 y)
{
    OP_INT16     start_x, start_y;
    MsfGadget   *pGadget;
    OP_INT16     iValue;
    OP_INT16     iMaxValue;
    OP_INT16     sb_start;
    OP_INT16     sb_end;
    OP_INT16     sb_A_start;
    OP_INT16     sb_B_start;
    OP_INT16     sb_C_start;
    OP_INT16     sb_D_start;
    OP_INT16     sb_E_start;
    SCROLLBAR_NOTIFICATION_T   op;

    pGadget = (MsfGadget*)pBar;

    if( !pBar 
      || (pBar->maxValue == 0 && pBar->maxValue + 1 <= pBar->pageStep))
    {
        return SB_NOTIFY_INVALID;
    }

    widgetGadgetGetPosition( pGadget, &start_x, &start_y);
    
    if( pBar->barType == MsfVerticalScrollBar )   
    {
        sb_start = start_y;
        sb_end = (OP_INT16)(start_y + pGadget->size.height-1);    
        iValue = y;
        iMaxValue = pGadget->size.height;
    }
    else
    {
        sb_start = start_x;
        sb_end = (OP_INT16)(start_x + pGadget->size.width -1);
        iValue = x;
        iMaxValue = pGadget->size.width;
    }
    
    sb_A_start = sb_start;
    sb_C_start = sb_A_start + SCROLLBAR_ARROW_HEIGHT;
    sb_E_start = sb_C_start +  
                 pBar->value * (iMaxValue - SCROLLBAR_ARROW_HEIGHT*2 - SCROLLBAR_SPIN_HEIGHT) / (pBar->maxValue + 1 - pBar->pageStep );
    sb_D_start = sb_E_start + SCROLLBAR_SPIN_HEIGHT;
    sb_B_start = sb_end - SCROLLBAR_ARROW_HEIGHT ;

    op = SB_NOTIFY_INVALID;

    if((iValue >= sb_start ) && ( iValue <= sb_C_start ))
    {
        op = SB_NOTIFY_LINE_UP;
    }else if((iValue > sb_C_start) && (iValue <= sb_E_start))
    {
        op = SB_NOTIFY_PAGE_UP;
    }else if((iValue >= sb_D_start) && (iValue < sb_B_start))
    {
        op = SB_NOTIFY_PAGE_DOWN;
    }else if((iValue >= sb_B_start) && (iValue <= sb_end))
    {
        op = SB_NOTIFY_LINE_DOWN;
    } 

    return op;

}


/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
void  widgetHandleUserEvent
(
    OPUS_EVENT_ENUM_TYPE   event,
    void                   *pMess,
    OP_BOOLEAN             *handle
)
{
    MsfScreen     *pScreen;
    
    *handle = OP_FALSE;

    pScreen = seekFocusedScreen();
    if( !pScreen  )
    {
        return;
    }
#ifdef _TOUCH_PANEL
    if( event == OPUS_FOCUS_PEN )
    {
        if( handle != OP_NULL )
        {
            *handle = OP_TRUE;
        }

        if( !pMess )
        {
            return;
        }
        
        widgetPenEventHandler( pScreen, (TP_PEN_EVENT_STRUCTURE_T*)pMess);
    }
    else
#endif    
    {
        widgetHandleTimer( event, handle);
        if( *handle == OP_FALSE)
        {
            widgetEventHandler(pScreen, event, pMess, handle );
        }    
    }
}

static void widgetEventHandler
(
    MsfScreen                         *pScreen,
    OPUS_EVENT_ENUM_TYPE   event,
    void                                *pMess,
    OP_BOOLEAN                    *handle
)
{
    static MsfScreenHandle hPressEventHandleScreen = INVALID_HANDLE;
    MsfWindow             *pWin;
    MsfGadget             *pGadget;
    void                     *pWidget;
    MsfWidgetType       wt;
    KEYEVENT_STRUCT *pKey;
    OP_UINT32             hHandler;

    pKey = (KEYEVENT_STRUCT*)pMess;
    
    if((( event == OPUS_FOCUS_KEYPAD && pKey->state ==UHKEY_RELEASE )
       || (event == OPUS_TIMER_KEYPRESS_REPEAT ))
       && (ISVALIDHANDLE(hPressEventHandleScreen )))
    {
        if( (hPressEventHandleScreen == pScreen->screenHandle )
          && (ISVALIDHANDLE(pScreen->hKeyPressEventHandler)))
        {
            pWidget = seekWidget( pScreen->hKeyPressEventHandler, &wt);
            if(pWidget)
            {
                if(IS_SCREEN(wt))
                {
                    screen_userevent_handler(pScreen->screenHandle, event, pMess, handle);
                }
                else if(IS_GADGET(wt))
                {
                    gadget_userevent_handler(pScreen->hKeyPressEventHandler, event, pMess, handle);
                }
                else if( IS_WINDOW(wt))
                {
                    window_userevent_handler( pScreen->hKeyPressEventHandler, event, pMess, handle);
                }
            }
        }

        if( event == OPUS_FOCUS_KEYPAD && pKey->state ==UHKEY_RELEASE )
        {
            hPressEventHandleScreen = INVALID_HANDLE;
            pScreen->hKeyPressEventHandler = INVALID_HANDLE;
            pScreen->iKeyPressCode = 0;
        }
        
        *handle = OP_TRUE;
        return ;
    }
    
    pWin = OP_NULL;

    hHandler = INVALID_HANDLE;
    
    if( pScreen->focusedChild != 0 )
    {
        pWin = (MsfWindow *)seekWidget(pScreen->focusedChild, &wt);
        if( pWin && IS_WINDOW(wt))/* window obj in focus */
        {
            /* pass event to MsfGadget handler */
            if( pWin->focusedChild != OP_NULL )
            {
                pGadget = (MsfGadget *)pWin->focusedChild->gadget;
                gadget_userevent_handler(pGadget->gadgetHandle, event, pMess, handle);
                if( *handle == OP_TRUE )
                {
                    hHandler = pGadget->gadgetHandle;
                }
            }

            if( *handle == OP_FALSE )
            {
                window_userevent_handler( pWin->windowHandle, event, pMess, handle);
                if( *handle == OP_TRUE )
                {
                    hHandler = pWin->windowHandle;
                }
            }
        }    
    }
    
    if( *handle == OP_FALSE )
    {
        screen_userevent_handler(pScreen->screenHandle, event, pMess, handle);
        if( *handle == OP_TRUE )
        {
            hHandler = pScreen->screenHandle;
        }
    }    

    if( *handle == OP_TRUE && event == OPUS_FOCUS_KEYPAD && pKey->state ==UHKEY_PRESS )
    {
        hPressEventHandleScreen = pScreen->screenHandle;
        pScreen->hKeyPressEventHandler = hHandler;
        pScreen->iKeyPressCode = pKey->code;
    }
}


static void gadget_userevent_handler 
(
    OP_UINT32 gadgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
)
{
    MsfWidgetType    widgetType;
    OP_BOOLEAN         bHandle = OP_FALSE;

    *handle = OP_FALSE;
    if( ISVALIDHANDLE(gadgetHandle ) )
    {
        widgetType = getWidgetType(gadgetHandle);
        if( IS_GADGET(widgetType) )
        {
           if( widgetMsfEventHandleFunc[widgetType] != OP_NULL )
           {
                (void)widgetMsfEventHandleFunc[widgetType]( gadgetHandle, event, pMess, &bHandle);
                if( bHandle == OP_TRUE)
                {
                    *handle = OP_TRUE;
                }    
           }

           if( *handle == OP_FALSE && event == OPUS_FOCUS_KEYPAD )
           {
                accesskey_handler( gadgetHandle, event, pMess, handle);
           }
        }
    }
}

static void window_userevent_handler 
(
    OP_UINT32 windowHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
)
{
    MsfWidgetType    widgetType;

    *handle = OP_FALSE;
    if( ISVALIDHANDLE(windowHandle ) )
    {
        widgetType = getWidgetType(windowHandle);
        if( IS_WINDOW(widgetType) )
        {
            if( widgetMsfEventHandleFunc[widgetType] != OP_NULL )
            {
                (void)widgetMsfEventHandleFunc[widgetType]( windowHandle, event, pMess, (OP_BOOLEAN *)handle);
            }

            if( *handle == OP_FALSE && event == OPUS_FOCUS_KEYPAD)
            {
                accesskey_handler( windowHandle, event, pMess, handle);

                if( *handle == OP_FALSE )
                {
                    action_handler( windowHandle, event, pMess, handle);
                }
            }

            if( *handle == OP_FALSE )
            {
                registered_userevent_handler( windowHandle,  event,  pMess,  handle);
            }

            /*  if the window is MsfDialog or waiting window, filter all OPUS_FOCUS_KEYPAD and OPUS_TIMER_KEYPRESS_REPEAT events */
            if( widgetType == MSF_DIALOG || widgetType == MSF_WAITINGWIN 
                && ( event == OPUS_FOCUS_KEYPAD ||  event == OPUS_TIMER_KEYPRESS_REPEAT))
            {
                *handle = OP_TRUE;
            }
        }
    }

}

static void screen_userevent_handler 
(
    OP_UINT32 screenHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
)
{
    MsfWidgetType    widgetType;
    OP_BOOLEAN           bHandle = OP_FALSE;

    *handle = OP_FALSE;
    if( ISVALIDHANDLE(screenHandle ) )
    {
        widgetType = getWidgetType(screenHandle);
        if( IS_SCREEN(widgetType) )
        {
            if( widgetMsfEventHandleFunc[widgetType] != OP_NULL )
            {
                (void)widgetMsfEventHandleFunc[widgetType]( screenHandle, event, pMess, &bHandle);
                if( bHandle == OP_TRUE)
                {
                    *handle = OP_TRUE;
                }    
            }

            if( *handle == OP_FALSE && event == OPUS_FOCUS_KEYPAD )
            {
                accesskey_handler( screenHandle, event, pMess, handle);

                if( *handle == OP_FALSE )
                {
                    action_handler( screenHandle, event, pMess, handle);
                }
            }

            if( *handle == OP_FALSE )
            {
                registered_userevent_handler( screenHandle,  event,  pMess,  handle);
            }

        }
    }

}

static void accesskey_handler
(
    OP_UINT32 widgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
)
{
    void               *pWidget;
    MsfAccessKey       *pAccessKey;
    MsfWidgetType      widgetType;
    char               *pAccessKeyDefinition;

    *handle = OP_FALSE;
    
    pAccessKeyDefinition = OpusEvent_to_MsfAccessKey( event, pMess);
    if( !pAccessKeyDefinition )
    {
        return;
    }
    
    if( ISVALIDHANDLE(widgetHandle))
    {
        pWidget = seekWidget(widgetHandle, &widgetType );
        if( pWidget )
        {
            pAccessKey = OP_NULL;
            
            if( IS_SCREEN(widgetType))
            {
                pAccessKey = ((MsfScreen*)pWidget)->accessKey;
            }
            else if( IS_WINDOW(widgetType))
            {
                pAccessKey = ((MsfWindow*)pWidget)->accessKey;
            }
            else if( IS_GADGET(widgetType))
            {
                pAccessKey = ((MsfGadget*)pWidget)->accessKey;
            }
            
            if( pAccessKey )
            {
                while( pAccessKey )
                {
                    if( Ustrcmp((OP_UINT8 *)pAccessKey->accessKeyDefinition, (OP_UINT8 *)pAccessKeyDefinition) == 0 )
                    {
                        break;
                    }

                    pAccessKey = pAccessKey->next;
                }

                if( pAccessKey )        /* found the accessKey */
                {
                    TPIc_widgetAccessKey(getModID(widgetHandle), widgetHandle, pAccessKey->handle);
                }
            }
        }
    }
}

static void action_handler
(
    OP_UINT32 widgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
)
{
    MsfAction*  pAction;

    *handle = OP_FALSE;
    
    pAction = OpusEvent_to_MsfAction( event, pMess, widgetHandle );
    if( pAction )
    {
        if( (pAction->propertyMask & MSF_ACTION_PROPERTY_ENABLED )
           && !(pAction->propertyMask & MSF_ACTION_PROPERTY_BLOCK))
        {           
            if( getWidgetType(widgetHandle) == MSF_MENU)
            {
                pAction->propertyMask |= MSF_ACTION_PROPERTY_BLOCK;
            }
            
            if( pAction->cbActionNotify )
            {
                (void)pAction->cbActionNotify( pAction->modId, widgetHandle, pAction->actionHandle);
            }
            else
            {
                TPIc_widgetAction( pAction->modId, widgetHandle , pAction->actionHandle );
            }

        }
        
        *handle = OP_TRUE;
    }
}

static void registered_userevent_handler
(
    OP_UINT32 widgetHandle, 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess,
    OP_BOOLEAN             *handle
)
{
    MsfEvent              msfEvent;
    MsfWidgetType widgetType;
    void *ptr = OP_NULL;
    MsfScreen *pScreen;
    MsfWindow *pWindow;
    OP_UINT32 regEvents;
    KEYEVENT_STRUCT *pKey;
    OP_UINT8 i;
    OP_UINT8 modId;
    widget_register_event_callback_t  cbRegEventNotify;

    *handle = OP_FALSE;
    ptr = seekWidget(widgetHandle, &widgetType);

    if (ptr == OP_NULL ||
        widgetType > MSF_WINDOW_END ) /* not a screen or window */
    {
        return;
    }

    if (widgetType == MSF_SCREEN)
    {
        pScreen = (MsfScreen *)ptr;
        regEvents = pScreen->registedEvents;
        cbRegEventNotify = pScreen->cbRegEventNotify;
        modId = pScreen->modId;
    }
    else /* handle is a window handle */
    {
        pWindow = (MsfWindow *)ptr;
        regEvents = pWindow->registedEvents;
        cbRegEventNotify = pWindow->cbRegEventNotify;
        modId = pWindow->modId;
    }
    
#ifdef _TOUCH_PANEL
    if( event == OPUS_FOCUS_PEN )
    {
        if( regEvents & REGISTERED_PEN_EVENT_MASK )
        {
            if( cbRegEventNotify )
            {
                (void)cbRegEventNotify( modId, widgetHandle, event, pMess );
            }
            else
            {
                TP_PEN_EVENT_STRUCTURE_T *pPen;
                OP_BOOLEAN         bNotify;
                
                pPen =(TP_PEN_EVENT_STRUCTURE_T*) pMess;
                bNotify = OP_TRUE;
                switch( pPen->state)
                {
                 case TP_PEN_STATE_PENDN:
                    msfEvent.eventClass = MsfPress;
                    break;
                 case TP_PEN_STATE_PENUP:
                    msfEvent.eventClass = MsfRelease;
                    break;
                 case TP_PEN_STATE_PENMV:
                    msfEvent.eventClass = MsfRepeat;
                    break;
                 default:   
                    bNotify = OP_FALSE;
                    break;
                }
                
                if( bNotify == OP_TRUE )
                {
                    *handle = OP_TRUE;
                    msfEvent.eventType = MsfPointer;
                    msfEvent.eventMode = 0;
                    msfEvent.position.x = (MSF_INT16)pPen->x;
                    msfEvent.position.y = (MSF_INT16)pPen->y;
                    TPIc_widgetUserEvent( modId, widgetHandle, &msfEvent);
                }
            }
        }
    }
    else
#endif    
    if (event == OPUS_FOCUS_KEYPAD ||event == OPUS_TIMER_KEYPRESS_REPEAT)
    {
        pKey = (KEYEVENT_STRUCT *)pMess;
        if( pKey->code == KEY_SIDE_UP )
        {
            pKey->code = KEY_UP;
        }
        else if( pKey->code == KEY_SIDE_DOWN)
        {
            pKey->code = KEY_DOWN;
        }
        	
        for (i = (OP_UINT8)MsfKey_0; i < (OP_UINT8)MsfKey_Undefined; i++)
        {
            if (pKey->code == key_map_array[i] &&
                ((0x00000001 << i) & regEvents)) /* this event is registed */
            {
                if( cbRegEventNotify )
                {
                    (void)cbRegEventNotify( modId, widgetHandle, event, pMess );
                }
                else
                {
                    if (event == OPUS_TIMER_KEYPRESS_REPEAT)
                    {
                        msfEvent.eventClass = MsfRepeat;
                    }
                    else if (pKey->state == UHKEY_RELEASE)
                    {
                        msfEvent.eventClass = MsfRelease;
                    }
                    else
                    {
                        msfEvent.eventClass = MsfPress;
                    }
                    msfEvent.eventType = (MsfEventType)i;
                    msfEvent.eventMode = 0;

                    /* Since there is some bug in AUs code in handling MsfEvent, now only pass MsfRelease class */
                    TPIc_widgetUserEvent( modId, widgetHandle, &msfEvent);
                }
                
                *handle = OP_TRUE;
                break;
            }
        }
    }
    return;
}

#if 0
static MsfEvent OpusEvent_to_MsfEvent
(
    OPUS_EVENT_ENUM_TYPE   event,
    void                   *pMess
)
{
    MsfEvent ret;
    KEYEVENT_STRUCT *pKey;
    OP_UINT16 i;

    if (event == OPUS_FOCUS_KEYPAD ||event == OPUS_TIMER_KEYPRESS_REPEAT)
    {
        pKey = (KEYEVENT_STRUCT *)pMess;
        if (event == OPUS_FOCUS_KEYPAD)
        {
            if (pKey->state == UHKEY_RELEASE)
            {
                ret.eventClass = MsfRelease;
            }
            else
            {
                ret.eventClass = MsfPress;
            }
        }
        else
        {
            ret.eventClass = MsfRepeat;
        }
        for (i = (OP_UINT16)MsfKey_0; i < (OP_UINT16)MsfKey_Undefined; i++)
        {
            if (pKey->code == key_map_array[i])
                break;
        }
        ret.eventType = (MsfEventType)i;
    }
    else
    {
        ret.eventClass = (MsfEventClass)0xFFFF;
        ret.eventType = (MsfEventType)0xFFFF;
        ret.eventMode = 0xFF;
    }

    return ret;
}
#endif

/*
 * The function return a MsfActionHandle which is corresponding to
 * the keypad event and has been attached to the MsfWindow/MsfScreen
 * before.
 */
static MsfAction* OpusEvent_to_MsfAction
(
    OPUS_EVENT_ENUM_TYPE   event,
    void                   *pMess,
    OP_UINT32             widget_handle
)
{
    MsfAction*   pAction;
    KEYEVENT_STRUCT *pKey;
    MsfWidgetType widgetType;
    //ActionsInWidget *pActionInWidget;
    MsfScreen       *pScreen;
    MsfWindow       *pWindow;
    void            *ptr = OP_NULL;
    MsfAction       **pActionlist;
    OP_UINT16      keyCode;
    OP_UINT8       i;
    
    pAction = OP_NULL;
    switch (event)
    {
        case OPUS_FOCUS_KEYPAD:
            pKey = (KEYEVENT_STRUCT *)pMess;
            if (pKey->state == UHKEY_RELEASE)
            {
                break;
            }
            ptr = seekWidget(widget_handle, &widgetType);
            if ((widgetType > MSF_WINDOW_END) ||
                (ptr == OP_NULL))
            {
                break;
            }
            
            keyCode = pKey->code;
            
            if (widgetType == MSF_SCREEN)
            {
                pScreen = (MsfScreen *)ptr;
                pActionlist = pScreen->actionlist;
            }
            else /* msf_window */
            {
                pWindow = (MsfWindow *)ptr;
                if (pWindow->propertyMask & MSF_WINDOW_PROPERTY_DISABLED)
                {
                    return OP_NULL;
                }
                pActionlist = pWindow->actionlist;
            }
            
            for (i =0; i < 8; i++) /* there are 8 kinds of MsfAction */
            {
                if (*pActionlist != OP_NULL)
                {
                    if (keyCode == action_map_array[(**pActionlist).actionType])
                    {
                        pAction = *pActionlist;

                        if ( widgetType == MSF_DIALOG)
                        {
                            MsfDialog   *pDlg = (MsfDialog *)ptr;
                            if( IS_WIDGET_TIMER_ID(pDlg->iTimerId)  )
                            {
                                /* stop the dialog  timer */ 
                                TPIa_widgetStopTimer( widget_handle, pDlg->iTimerId);
                                pDlg->iTimerId = WIDGET_TIMER_INVALID_ID;
                            }
                        }
                        
                        break;
                    }
                }
                pActionlist++;
            }

            break;
            
        default:
            break;
    }

    return pAction;
}

static char* OpusEvent_to_MsfAccessKey
(
    OPUS_EVENT_ENUM_TYPE   event,
    void                   *pMess
)
{
    KEYEVENT_STRUCT   *pKey;
    static  char                str[12];            
    
    if( event == OPUS_FOCUS_KEYPAD ) 
    {
        pKey = (KEYEVENT_STRUCT*)pMess;

        if(pKey->state == UHKEY_PRESS)
        {
            op_memset( str, 0x00, 12);
            
            switch(pKey->code)
            {
            case KEY_0:
                str[0] = 0x30;
                break;
            case KEY_1:
                str[0] = 0x31;
                break;
            case KEY_2:
                str[0] = 0x32;
                break;
            case KEY_3:
                str[0] = 0x33;
                break;
            case KEY_4:
                str[0] = 0x34;
                break;
            case KEY_5:
                str[0] = 0x35;
                break;
            case KEY_6:
                str[0] = 0x36;
                break;
            case KEY_7:
                str[0] = 0x37;
                break;
            case KEY_8:
                str[0] = 0x38;
                break;
            case KEY_9:
                str[0] = 0x39;
                break;
            default:
                break;
            }

            if( str[0] != 0x00 )    /* There is an Accesskey definition */
            {
                return str;
            }
        }

    }   
    return OP_NULL;
}

/*==================================================================================================
    FUNCTION:  widgetFormHandleKeyEventScroll

    DESCRIPTION:
        handle the pen events
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetFormHandleKeyEventScroll
(
    MsfForm *pForm, 
    UINT16  iKeyCode,
    OP_BOOLEAN   *handle
)
{
    SCROLLBAR_NOTIFICATION_T op;
    MsfBar                  *pBar;
    
    if( !pForm || handle == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( !pForm->pVScrollBar  && !pForm->pHScrollBar )
    {
        return TPI_WIDGET_OK;
    }

    op = SB_NOTIFY_INVALID;
    pBar = OP_NULL;
    switch(iKeyCode)
    {
        case KEY_UP:
            pBar = pForm->pVScrollBar;
            op = SB_NOTIFY_LINE_UP;
            break;
        case KEY_DOWN:
            pBar = pForm->pVScrollBar;
            op = SB_NOTIFY_LINE_DOWN;
            break;
        case KEY_SIDE_UP:
            pBar = pForm->pVScrollBar;
            op = SB_NOTIFY_PAGE_UP;
            break;
        case KEY_SIDE_DOWN:
            pBar = pForm->pVScrollBar;
            op = SB_NOTIFY_PAGE_DOWN;
            break;
        case KEY_LEFT:
            pBar = pForm->pHScrollBar;
            op = SB_NOTIFY_LINE_UP;
            break;
        case KEY_RIGHT:
            pBar = pForm->pHScrollBar;
            op = SB_NOTIFY_LINE_DOWN;
            break;
        default:
            break;
    }

    if( pBar )
    {
        *handle = widgetScrollbarScroll( pBar, op);
    }

    return TPI_WIDGET_OK;
}


static GadgetsInWindow* widgetWindowGetNextFocusGadget
(
    const MsfWindow  *pWin,
    OP_BOOLEAN        bBack
)
{
    GadgetsInWindow  *pGadInWin = OP_NULL;
    GadgetsInWindow  *pTemp;

    if( !pWin )
    {
        return OP_NULL;
    }

    if ( bBack )
    {   /* the focus will switch to the prior gadget */
        if (pWin->focusedChild != OP_NULL)
        {
            pGadInWin = pWin->focusedChild->prev;
            while (pGadInWin != OP_NULL)
            {
                if (((MsfGadget *)pGadInWin->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                {
                    break;
                }
                pGadInWin = pGadInWin->prev;
            }

            /* find the last gadget which can get focus */
            if(!pGadInWin &&  !(pWin->propertyMask & MSF_WINDOW_PROPERTY_NOT_KEEP_GADGET_FOCUS ))
            {
                pTemp = pWin->focusedChild->next;
                while ( pTemp!= OP_NULL)
                {
                    if (((MsfGadget *)pTemp->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                    {
                        pGadInWin = pTemp;
                    }
                    pTemp = pTemp->next;
                }
            }
        }
        else
        {
            pTemp = OP_NULL;
            
            /* the new focused gadget should be the last one*/
            pGadInWin = pWin->children;

            /* look for the last gadget that can be focused */
            while (pGadInWin != OP_NULL)
            {
                if (((MsfGadget *)pGadInWin->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                {
                    pTemp = pGadInWin;
                }
                pGadInWin = pGadInWin->next;
            }
            pGadInWin = pTemp;
        }
    }
    else  /* forward search focus gadget */
    {
        if (pWin->focusedChild != OP_NULL)
        {
            pGadInWin = pWin->focusedChild->next;
        }
        else
        {
            /* the new focused gadget should be the first one*/
            pGadInWin = pWin->children;
        }
        while (pGadInWin != OP_NULL)
        {
            if (((MsfGadget *)pGadInWin->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
            {
                break;
            }
            pGadInWin = pGadInWin->next;
        }
        
        /* find the first gadget which can get focus */
        if(!pGadInWin && pWin->focusedChild != OP_NULL
        && !(pWin->propertyMask & MSF_WINDOW_PROPERTY_NOT_KEEP_GADGET_FOCUS ))
        {
            pTemp = pWin->focusedChild->prev;
            while ( pTemp!= OP_NULL)
            {
                if (((MsfGadget *)pTemp->gadget)->propertyMask & MSF_GADGET_PROPERTY_FOCUS)
                {
                    pGadInWin = pTemp;
                }
                pTemp = pTemp->prev;
            }
        }
    }

    return pGadInWin;
}

#ifdef _TOUCH_PANEL
static int gadgetPenEventToKeyEventHandler
(
    const MsfWindow              *pWin,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess

)
{
    OP_BOOLEAN             bHandle;

    if(! pWin )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    window_userevent_handler( pWin->windowHandle, event, pMess, &bHandle);
    
    if( bHandle == OP_FALSE && pWin->parent != OP_NULL )
    {
        screen_userevent_handler(pWin->parent->screenHandle, event, pMess, &bHandle);
    }    

    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  widgetPenEventHandler

    DESCRIPTION:
        handle the pen events
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static void widgetPenEventHandler
(
    MsfScreen                               *pScreen,
    TP_PEN_EVENT_STRUCTURE_T   *pPen
)
{
    if( pPen->state == TP_PEN_STATE_PENDN )
    {
        pScreen->penHandlerType = MSF_NO_HANDLER;
        widgetPenDownEventHandler( pScreen, pPen);
    }
    else
    {
        widgetPenUpMoveEventHandler( pScreen, pPen);
    }
}

/*==================================================================================================
    FUNCTION:  widgetPenDownEventHandler

    DESCRIPTION:
        handle the pen down event, the general process as following:
            1. if the pen down point in the soft key area, then tick down the handler type with MSF_SKEY_HANDLER
            2. if the input method is open, the pass the event to the input method event handler,
            3. if the input method is close or not created, or the input method event handler can not handle the 
                pen down event, pass the event to the screen event handler to handle
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static void widgetPenDownEventHandler
(
    MsfScreen                                *pScreen,
    TP_PEN_EVENT_STRUCTURE_T    *pPen
)
{
    OP_BOOLEAN   bHandled;
    MsfWidgetImm *pImm;
    
    if( pPen->state != TP_PEN_STATE_PENDN || pPen->x >= DS_SCRN_MAX_X ||pPen->y >= DS_SCRN_MAX_Y)
    {
        return;
    }
    
    bHandled = OP_FALSE;
    pImm = pScreen->pImm;
    
    /* pt in soft key area */
    if( pPen->y >= DS_SCRN_EXT_TEXT_HIGH  )
    {
        pScreen->penDownPosInSKey.x = (MSF_INT16)pPen->x;
        pScreen->penDownPosInSKey.y = (MSF_INT16)pPen->y;
        pScreen->penHandlerType = MSF_SKEY_HANDLER;
        
        bHandled = OP_TRUE;
    }
    
    /* input method is open  */
    else if( pImm &&( pImm->bOpen == OP_TRUE ))
    {
        if( pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING ) /* hand-written input  */
        {
            if (widgetImmEventHandler( pImm, OPUS_FOCUS_PEN, (void*)pPen, &bHandled ) 
                == TPI_WIDGET_HW_SCROLLING)
            {
                if( ds_get_disp_annun())
                {
                    /* point in the annunciator area */
                    if( pPen->y <  LCD_ANNUN_HIGH )
                    {
                        return;
                    }
                    /* point in the text area  */
                    else if( pPen->y < LCD_SKEY_FRAME_START )
                    {
                        pPen->y -= LCD_ANNUN_HIGH;
                    }
                }
                
                widgetScreenHandlePenDownEvent(pScreen, pPen);
                bHandled = OP_TRUE;
            }
            else
            {
                if( bHandled == OP_TRUE )
                {
                    pScreen->penHandlerType = MSF_IMM_HANDLER;
                }
                else   /* not give widget chance to handle the pen down event  */
                {
                    bHandled = OP_TRUE;
                }
            }
        }
        else
        {
            OP_INT16    iImmStart;
            if( ds_get_disp_annun())
            {
                iImmStart = pImm->start_y + LCD_ANNUN_HIGH;
            }
            else
            {
                iImmStart = pImm->start_y;
            }
            
            if( pPen->y > iImmStart  ) /*  pt in imm area  */
            {
                widgetImmEventHandler( pImm, OPUS_FOCUS_PEN, (void*)pPen, &bHandled );
                if( bHandled == OP_TRUE )
                {
                    pScreen->penHandlerType = MSF_IMM_HANDLER;
                }
                else   /* not give widget chance to handle the pen down event  */
                {
                    bHandled = OP_TRUE;
                }
            }
        }
    }

    /* 
      * hand-written input method may be no handle, or closed,
      * pass to window or gadget handle
      */
    if( OP_FALSE == bHandled )
    {
        if( ds_get_disp_annun())
        {
            /* point in the annunciator area */
            if( pPen->y <  LCD_ANNUN_HIGH )
            {
                return;
            }
            /* point in the text area  */
            else if( pPen->y < LCD_SKEY_FRAME_START )
            {
                pPen->y -= LCD_ANNUN_HIGH;
            }
        }
        
        widgetScreenHandlePenDownEvent(pScreen, pPen);
    }
}

/*==================================================================================================
    FUNCTION:  widgetPenUpMoveEventHandler

    DESCRIPTION:
        handle the pen move and pen up event, the general process as following:
        if the pen down event handler type is 
            1. MSF_SKEY_HANDLER, then pass the event to soft key area to handle,
            2. MSF_IMM_HANDLER,  then pass the event to input method event handler to handle
            3. MSF_WIDGET_HANDLER, then pass the event to the ticked widget to handle
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
void widgetPenUpMoveEventHandler
(
    MsfScreen                                *pScreen,
    TP_PEN_EVENT_STRUCTURE_T    *pPen
)
{
    OP_BOOLEAN              bHandled;
    KEYEVENT_STRUCT      mess;
    int                            iRet = TPI_WIDGET_OK;
 
    if((!pScreen ||!pPen ) 
    ||((pPen->state != TP_PEN_STATE_PENUP) && (pPen->state != TP_PEN_STATE_PENMV)))
    {
        return;
    }
    
    bHandled = OP_FALSE;
    
    switch( pScreen->penHandlerType )
    {
    case MSF_SKEY_HANDLER:
        {
            if( pPen->state == TP_PEN_STATE_PENMV)
            {
                return ;
            }
            
            if( pPen->y >= WAP_SCRN_TEXT_HIGH && pPen->y < DS_SCRN_MAX_Y )
            {
                if( pPen->x >= DS_SCRN_MAX_X )
                {
                    pScreen->penHandlerType = MSF_NO_HANDLER;
                    return ;
                }

                mess.state = UHKEY_PRESS;
                if( pScreen->penDownPosInSKey.x < THREEKEY_S_SKEY_WIDTH ) /* left skey */
                {
                    if(pPen->x < THREEKEY_S_SKEY_WIDTH)
                    {
                        mess.code = KEY_SOFT_LEFT;
                    }
                    else
                    {
                        pScreen->penHandlerType = MSF_NO_HANDLER;
                        return ;
                    }
                }
                 /* right skey */
                else if( pScreen->penDownPosInSKey.x >= THREEKEY_S_SKEY_WIDTH + THREEKEY_C_SKEY_WIDTH )
                {
                    if( pPen->x >= THREEKEY_S_SKEY_WIDTH + THREEKEY_C_SKEY_WIDTH )
                    {
                        mess.code = KEY_SOFT_RIGHT;
                    }
                    else
                    {
                        pScreen->penHandlerType = MSF_NO_HANDLER;
                        return ;
                    }
                }
                 /* center skey */
                else
                {
                    if((pPen->x >= THREEKEY_S_SKEY_WIDTH ) 
                     && (pPen->x < THREEKEY_S_SKEY_WIDTH + THREEKEY_C_SKEY_WIDTH ))
                    {
                        mess.code = KEY_OK;
                    }
                    else
                    {
                        pScreen->penHandlerType = MSF_NO_HANDLER;
                        return ;
                    }
                }

                widgetEventHandler(pScreen, OPUS_FOCUS_KEYPAD, (void*)&mess, &bHandled);

                mess.state = UHKEY_RELEASE;
                widgetEventHandler(pScreen, OPUS_FOCUS_KEYPAD, (void*)&mess, &bHandled);

            }
            break;
        }    
    case MSF_IMM_HANDLER:
        {
            if (widgetImmEventHandler( pScreen->pImm, OPUS_FOCUS_PEN, (void*)pPen, &bHandled )
                == TPI_WIDGET_HW_CURSOR_LOCATING)
            {
                if( ds_get_disp_annun())
                {
                    /* point in the annunciator area */
                    if( pPen->y <  LCD_ANNUN_HIGH )
                    {
                        return;
                    }
                    /* point in the text area  */
                    else if( pPen->y < LCD_SKEY_FRAME_START )
                    {
                        pPen->y -= LCD_ANNUN_HIGH;
                    }
                }
                pPen->state = TP_PEN_STATE_PENDN;
                widgetScreenHandlePenDownEvent(pScreen, pPen);
                pPen->state = TP_PEN_STATE_PENUP;
                widgetPenUpMoveEventHandler(pScreen, pPen);

            }
            break;
        }    
    case MSF_WIDGET_HANDLER:
        {
            MsfWidgetType wt;
            MsfPosition       pos;
            MsfGadget       *pGadget;
            MsfWindow        *pWin;
            
            pGadget = (MsfGadget*)seekWidget( pScreen->hPenHandleWidget, &wt);
            if(pGadget && wt <= MSF_GADGET_END )
            {
                if( ds_get_disp_annun())
                {
                    /* point in the annunciator area */
                    if( pPen->y <  LCD_ANNUN_HIGH )
                    {
                        return;
                    }
                    /* point in the text area  */
                    else if( pPen->y < LCD_SKEY_FRAME_START )
                    {
                        pPen->y -= LCD_ANNUN_HIGH;
                    }
                }
                
                if( IS_SCREEN(wt))
                {
                    /* registered events in screen  handle */
                    registered_userevent_handler(pScreen->hPenHandleWidget, OPUS_FOCUS_PEN, (void*)pPen, &bHandled);
                }
                else
                {  
                    if(  IS_GADGET(wt) )
                    {
                        pWin = pGadget->parent;
                    }
                    else 
                    {
                        pWin = (MsfWindow*)pGadget;
                    }
                    
                    if( pWin && pWin->isFocused == OP_TRUE )
                    {
                        widgetPointScreenToClient(
                                            pWin, 
                                            (OP_INT16)pPen->x, 
                                            (OP_INT16)pPen->y, 
                                            &pos.x, 
                                            &pos.y);

                        pPen->x = pos.x;
                        pPen->y = pos.y;

                        if( IS_WINDOW(wt))
                        {
                            /* registered events in screen  handle */
                            registered_userevent_handler(pScreen->hPenHandleWidget, OPUS_FOCUS_PEN, (void*)pPen, &bHandled);
                        }
                        else
                        {
                            if( widgetMsfEventHandleFunc[wt] )
                            {
                                iRet = widgetMsfEventHandleFunc[wt](pScreen->hPenHandleWidget,OPUS_FOCUS_PEN, (void*)pPen, &bHandled );
                            }

                            /*  If the event is pen up and the gadget demand to translate the event to ok event  */
                            if( iRet == WIDGET_PEN_EVENT_TO_OK
                                && pPen->state == TP_PEN_STATE_PENUP
                                && (pGadget->propertyMask & MSF_GADGET_PROPERTY_PEN_CLICK ))
                            {
                                if( pGadget->cbStateChange )
                                {
                                    (void)pGadget->cbStateChange(pGadget->modId, pGadget->gadgetHandle, MsfNotifyClick, OP_NULL);
                                }                     
                                else if( wt == MSF_TEXTINPUT && (pGadget->propertyMask&(~MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM)))
                                {
                                    TPIa_widgetTextInputDefaultStateChangeCb(pGadget->modId, pGadget->gadgetHandle, MsfNotifyClick, OP_NULL);
                                }

#ifdef _KEYPAD 
                                else
                                {
                                    mess.code = KEY_OK;
                                    mess.state = UHKEY_PRESS;
                                    gadgetPenEventToKeyEventHandler( pWin,  OPUS_FOCUS_KEYPAD, (void*)&mess);

                                    mess.state = UHKEY_RELEASE;
                                    gadgetPenEventToKeyEventHandler( pWin,  OPUS_FOCUS_KEYPAD, (void*)&mess);
                                }
#endif                                
                            }
                        }
                    }
                }
            }
            break;
        }    
    default :
        {
            break;
        } 
    }

    if(pPen->state == TP_PEN_STATE_PENUP)
    {
        pScreen->penHandlerType = MSF_NO_HANDLER;
    }
}


/*==================================================================================================
    FUNCTION:  widgetScreenHandlePenDownEvent

    DESCRIPTION:
        the focus screen handles the pen down event, the general process as following:
        1. if the current window is always-on-top, the pass the event to it to hanlde, if it does not handle,
                then discard the event
        2. if the current window is not always-on-top, then search the always-on-top windows 
            in the focus screen, if the pen down point in a always-on-top window, switch the windows focus,
            and pass the event to the new focus window to handle
        3. if the event is not handled, then pass the event to the current window to handle,
            if the pen down point not in the window, goto 4.
        4.  search the normal windows in the focus screen, if the pen down point in a window, switch the 
            windows focus, and pass the event to the new focus window to handle
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
void widgetScreenHandlePenDownEvent
(
    MsfScreen                                *pScreen,
    TP_PEN_EVENT_STRUCTURE_T    *pPen
)
{
    MsfWindow         *pWin;
    MsfWindow         *pCurWin;
    MsfWidgetType   wt;
    OP_BOOLEAN      bHandled = OP_FALSE;
    WindowsInScreen  *pWindowNode;
    WindowsInScreen  *pOnTopWindowNode;

    if( !pScreen || !pPen )
    {
        return ;
    }

    pWindowNode = pScreen->children;
    pCurWin = OP_NULL;
    
    if( ISVALIDHANDLE(pScreen->focusedChild) )
    {
        pCurWin = (MsfWindow*)seekWidget(pScreen->focusedChild, &wt );
        if( !pCurWin || !IS_WINDOW(wt))
        {
            return;
        }
        
        if( pCurWin->propertyMask & MSF_WINDOW_PROPERTY_ALWAYSONTOP ) 
        {
            /* the pt in focus window */
            if( POINT_IN_RECT( *pPen, pCurWin->position, pCurWin->size ) )
            {
                widgetWindowHandlePenDownEvent(pScreen,pCurWin, pPen, bHandled );
            }
            /*
              *  The pt not in focus window
              *  but the focus window is ON_TOP_ALWAYS, discard the pen event
              */
            else
            {
                pScreen->penHandlerType = MSF_NO_HANDLER;
            }
            
            return;
        }
        
        pWindowNode = pWindowNode->next;
    }    
    
    /* ontop window handle */
    pOnTopWindowNode = pWindowNode;
    while( pOnTopWindowNode )
    {
        pWin = (MsfWindow*)pOnTopWindowNode->window;
        if( pWin  && (pWin->propertyMask & MSF_WINDOW_PROPERTY_ALWAYSONTOP)
            && POINT_IN_RECT( *pPen, pWin->position, pWin->size ))
        {
            bHandled = OP_TRUE;
            if( pWin->propertyMask & MSF_WINDOW_PROPERTY_DISABLE_PEN_EVENT )
            {
                pScreen->penHandlerType = MSF_NO_HANDLER;
            }
            else
            {
                TPIa_widgetSetInFocus( pWin->windowHandle, 1);

                if( pWin->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY )
                {
                    if( pWin->cbStateChange )
                    {
                        (void)pWin->cbStateChange(pWin->modId, pWin->windowHandle, MsfNotifyFocus, OP_NULL);
                    }
                    else
                    {
                        TPIc_widgetNotify(pWin->modId, pWin->windowHandle, MsfNotifyFocus);
                    }
                }
                
                /*  notify the old focus window lose focus */
                if( pCurWin && (pCurWin->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY ))
                {
                    if( pCurWin->cbStateChange )
                    {
                        (void)pCurWin->cbStateChange(pCurWin->modId, pCurWin->windowHandle, MsfNotifyLostFocus, OP_NULL);
                    }
                    else
                    {
                        TPIc_widgetNotify( pCurWin->modId, pCurWin->windowHandle, MsfNotifyLostFocus);
                    }
                }
                
                widgetWindowHandlePenDownEvent(pScreen, pWin, pPen, bHandled );
            }
            return;
        }
        
        pOnTopWindowNode = pOnTopWindowNode->next;
    }
    
    /* focus window handle */
    if( pCurWin )
    {
        /* the pt in focus window */
        if( POINT_IN_RECT( *pPen, pCurWin->position, pCurWin->size ) )
        {
            widgetWindowHandlePenDownEvent(pScreen,pCurWin, pPen, bHandled );
            return;
        }
    }

    /* non-focus normal window handle */
    while( pWindowNode )
    {
        pWin = (MsfWindow*)pWindowNode->window;
        if( pWin  && !(pWin->propertyMask & MSF_WINDOW_PROPERTY_ALWAYSONTOP)
            && POINT_IN_RECT( *pPen, pWin->position, pWin->size ))
        {
            bHandled = OP_TRUE;
            if( pWin->propertyMask & MSF_WINDOW_PROPERTY_DISABLE_PEN_EVENT )
            {
                pScreen->penHandlerType = MSF_NO_HANDLER;
            }
            else
            {            
                TPIa_widgetSetInFocus( pWin->windowHandle, 1);
                
                if( pWin->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY )
                {
                    if( pWin->cbStateChange )
                    {
                        (void)pWin->cbStateChange(pWin->modId, pWin->windowHandle, MsfNotifyFocus, OP_NULL);
                    }
                    else
                    {
                        TPIc_widgetNotify( pWin->modId, pWin->windowHandle, MsfNotifyFocus);
                    }
                }
                
                /*  notify the old focus window lose focus */
                if( pCurWin && (pCurWin->propertyMask & MSF_WINDOW_PROPERTY_NOTIFY ))
                {
                    if( pCurWin->cbStateChange )
                    {
                        (void)pCurWin->cbStateChange(pCurWin->modId, pCurWin->windowHandle, MsfNotifyLostFocus, OP_NULL);
                    }
                    else
                    {
                        TPIc_widgetNotify( pCurWin->modId, pCurWin->windowHandle, MsfNotifyLostFocus);
                    }
                }
                
                widgetWindowHandlePenDownEvent(pScreen, pWin, pPen, bHandled );
            }
            return;
        }
        
        pWindowNode = pWindowNode->next;
    }

    /* registered events in screen  handle */
    registered_userevent_handler(
                                pScreen->screenHandle, 
                                OPUS_FOCUS_PEN, 
                                (void*)pPen, 
                                &bHandled);
                                
    if( bHandled == OP_TRUE )
    {
        pScreen->penHandlerType = MSF_WIDGET_HANDLER;
        pScreen->hPenHandleWidget = pScreen->screenHandle;
    }
    
}

/*==================================================================================================
    FUNCTION:  widgetWindowHandlePenDownEvent

    DESCRIPTION:
        the focus window handles the pen down event, the general process as following:
        1. if the current gadget is always-on-top, the pass the event to it to hanlde, if it does not handle,
        2. if the current gadget is not always-on-top, then search the always-on-top gadgets 
            in the focus window, if the pen down point in a always-on-top gadget, switch the gadgets focus,
            and pass the event to the new focus gadget to handle
        3. if the event is not handled, then pass the event to the current gadget to handle,
            if the pen down point not in the gadget, goto 4.
        4.  search the normal gadgets in the focus window, if the pen down point in a gadget, switch the 
            gadgets focus, and pass the event to the new focus gadget to handle
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static void widgetWindowHandlePenDownEvent
(
    MsfScreen                                *pScreen,
    MsfWindow                               *pWin,
    TP_PEN_EVENT_STRUCTURE_T    *pPen,
    OP_BOOLEAN                           bHandled
)
{
    OP_BOOLEAN     bCurGadgetHandled;
    MsfPosition        clientPos;
    MsfSize             clientSize;
    TP_PEN_EVENT_STRUCTURE_T   penPt;
    int                   iRet;

    if( !pScreen || !pWin || !pPen )
    {
        return ;
    }
    
    widgetGetClientRect( pWin, &clientPos, &clientSize);

    clientPos.x = pWin->position.x + pWin->clientOrigin.x;
    clientPos.y = pWin->position.y + pWin->clientOrigin.y;
    
    if( !POINT_IN_RECT( *pPen, clientPos, clientSize ))
    {
        return;
    }
    penPt.state = pPen->state;
    
    widgetPointScreenToClient(
                            pWin, 
                            (OP_INT16)pPen->x, 
                            (OP_INT16)pPen->y, 
                            (OP_INT16*)&penPt.x, 
                            (OP_INT16*)&penPt.y);
                            
    bCurGadgetHandled = OP_TRUE;
    if( pWin->focusedChild && pWin->focusedChild->gadget)
    {
        if (pWin->focusedChild->gadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP)
        {
            /* focus gadget handle */
            iRet = widgetFGadgetHandlePenDownEvent(
                                                    pScreen, 
                                                    pWin, 
                                                    &penPt, 
                                                    bHandled);
            if( iRet != WIDGET_EVENT_NOT_HANDLE )
            {
                return;
            }
        }
        else
        {
            bCurGadgetHandled = OP_FALSE;
        }
    }
    
    /* non-focus ontop gadget handle */
    iRet = widgetNFGadgetsHandlePenDownEvent(
                                            pScreen, 
                                            pWin, 
                                            &penPt, 
                                            MSF_ONLY_ONTOP_GADGETS);

    if( iRet != WIDGET_EVENT_NOT_HANDLE )
    {
        return;
    }

    if( bCurGadgetHandled == OP_FALSE )
    {
        /* focus gadget handle */
        iRet = widgetFGadgetHandlePenDownEvent(
                                                pScreen, 
                                                pWin, 
                                                &penPt, 
                                                bHandled);

        if( iRet != WIDGET_EVENT_NOT_HANDLE )
        {
            return;
        }
    }
    
    /* non-focus normal gadget handle */
    iRet = widgetNFGadgetsHandlePenDownEvent(
                                            pScreen, 
                                            pWin, 
                                            &penPt, 
                                            MSF_ONLY_NON_ONTOP_GADGETS);
                                                
    if( iRet != WIDGET_EVENT_NOT_HANDLE )
    {
        return;
    }

    bHandled = OP_FALSE;
    
    /* registered events in window  handle */
    registered_userevent_handler(
                                pWin->windowHandle, 
                                OPUS_FOCUS_PEN,
                                &penPt, 
                                &bHandled);
                            
    if( bHandled == OP_TRUE )
    {
        pScreen->penHandlerType = MSF_WIDGET_HANDLER;
        pScreen->hPenHandleWidget = pWin->windowHandle;
    }
}


/*==================================================================================================
    FUNCTION:  widgetFGadgetHandlePenDownEvent

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetFGadgetHandlePenDownEvent
(
    MsfScreen                               *pScreen,
    MsfWindow                              *pWin,
    TP_PEN_EVENT_STRUCTURE_T   *pPen,
    OP_BOOLEAN                          bHandled
)
{
    MsfGadget          *pGadget;
    MsfWidgetType    wt;
    MsfBarType         barType;
    MsfPosition          pos;
    MsfSize              size;

    if(!pScreen || !pWin || !pPen )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    if( pWin->focusedChild == OP_NULL )
    {
        return  WIDGET_EVENT_NOT_HANDLE;
    }
    
    pGadget = (MsfGadget*)pWin->focusedChild->gadget;
    if( !pGadget )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    wt = getWidgetType(pGadget->gadgetHandle);
    if( !IS_GADGET(wt))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y );
    widgetGadgetGetSize(pGadget, &size);
    if( wt == MSF_BAR )
    {
        barType = ((MsfBar*)pGadget)->barType;
        if( barType == MsfVerticalScrollBar )
        {
            pos.x -= PEN_SENSITIVITY;
            size.width += 2 * PEN_SENSITIVITY;
        }
        else if( barType == MsfHorizontalScrollBar )
        {
            pos.y -= PEN_SENSITIVITY;
            size.width += 2 * PEN_SENSITIVITY;
        }
    }
    
    if( POINT_IN_RECT( *pPen, pos, size ))
    {
        if( bHandled == OP_TRUE )
        {
            if( wt == MSF_BAR )
            {
                barType = ((MsfBar*)pGadget)->barType;
                
                if( barType == MsfVerticalScrollBar || barType == MsfHorizontalScrollBar )
                {
                    bHandled = OP_FALSE;
                }
            }
            else
            {
                /* the screen already switched the window focus, so discard the pen event
                  */
                  
                /* maybe the window will support to be moved by pen in future 
                pScreen->penHandlerType = MSF_WIDGET_HANDLER;
                pScreen->hPenHandleWidget = pWin->windowHandle;*/
                
                /* discard the succedent pen move or up events */
                pScreen->penHandlerType = MSF_NO_HANDLER;
            }
        }

        if( bHandled == OP_FALSE && widgetMsfEventHandleFunc[wt] )
        {
            (void)widgetMsfEventHandleFunc[wt](
                            pGadget->gadgetHandle,
                            OPUS_FOCUS_PEN, 
                            (void*)pPen, 
                            &bHandled );

            if( bHandled == OP_TRUE 
                ||(pGadget->propertyMask & MSF_GADGET_PROPERTY_PEN_CLICK))
            {
                pScreen->penHandlerType = MSF_WIDGET_HANDLER;
                pScreen->hPenHandleWidget = pGadget->gadgetHandle;
            }
        }
        
        return WIDGET_EVENT_HANDLED;
    }

    return WIDGET_EVENT_NOT_HANDLE;
}


/*==================================================================================================
    FUNCTION:  widgetNFGadgetsHandlePenDownEvent

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetNFGadgetsHandlePenDownEvent
(
    MsfScreen                               *pScreen,
    MsfWindow                              *pWin,
    TP_PEN_EVENT_STRUCTURE_T   *pPen,
    MsfGadgetScope                     gadgetScope
)
{
    MsfGadget         *pGadget;
    MsfWidgetType   wtWin, wtGadget;
    GadgetsInWindow  *pGadgetNode;
    GadgetsInWindow  *pCurFocusGadgetNode;
    MsfBarType       barType;
    OP_BOOLEAN     bHandled;
    MsfPosition        pos;
    MsfSize             size;

    if(!pScreen || !pWin || !pPen )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    if((gadgetScope != MSF_ONLY_NON_ONTOP_GADGETS )
        && (gadgetScope != MSF_ONLY_ONTOP_GADGETS))
    {
        return WIDGET_EVENT_NOT_HANDLE;
    }
    
    pCurFocusGadgetNode = pWin->focusedChild;
    pGadgetNode = pWin->children;
    wtWin = getWidgetType(pWin->windowHandle );
    
    /* search the gadget list of the window */
    while( pGadgetNode )
    {
        if( pGadgetNode != pCurFocusGadgetNode )
        {
            pGadget= (MsfGadget*)pGadgetNode->gadget;
            if( pGadget )
            {
                if( ((MSF_ONLY_ONTOP_GADGETS == gadgetScope) && !(pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP))
                    || ((MSF_ONLY_NON_ONTOP_GADGETS == gadgetScope) && (pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP)))
                {
                    pGadgetNode = pGadgetNode->next;
                    continue;
                }

                wtGadget = getWidgetType(pGadget->gadgetHandle);
                widgetGadgetGetPosition( pGadget, &pos.x, &pos.y );
                widgetGadgetGetSize(pGadget, &size);
                if( wtGadget == MSF_BAR )
                {
                    barType = ((MsfBar*)pGadget)->barType;
                    if( barType == MsfVerticalScrollBar )
                    {
                        pos.x -= PEN_SENSITIVITY;
                        size.width += 2 * PEN_SENSITIVITY;
                    }
                    else if( barType == MsfHorizontalScrollBar )
                    {
                        pos.y -= PEN_SENSITIVITY;
                        size.height += 2 * PEN_SENSITIVITY;
                    }
                }
                
                if( POINT_IN_RECT( *pPen, pos, size ))
                {
                    bHandled = OP_FALSE;

                    /*  
                      *  if the pen point in scroll bar, the scroll bar should not get the focus, 
                      *  but the scroll bar must handle the pen event
                      */ 
                    if( wtGadget == MSF_BAR )
                    {
                        barType = ((MsfBar*)pGadget)->barType;
                        
                        if( barType == MsfVerticalScrollBar || barType == MsfHorizontalScrollBar )
                        {
                            if( widgetMsfEventHandleFunc[MSF_BAR] )
                            {
                                (void)widgetMsfEventHandleFunc[MSF_BAR](
                                                pGadget->gadgetHandle,
                                                OPUS_FOCUS_PEN, 
                                                (void*)pPen, 
                                                &bHandled );
                            }
                            
                            bHandled = OP_TRUE;
                        }
                    }

                    if((bHandled == OP_FALSE) 
                        && (pGadget->propertyMask & MSF_GADGET_PROPERTY_FOCUS))
                    {
                        if( wtWin == MSF_PAINTBOX )
                        {
                            if( widgetFocusGadgetAdjustWindow( pGadget, pWin ) == OP_FALSE )
                            {
                                TPIc_widgetNotify(pWin->modId, 
                                                             pGadget->gadgetHandle,
                                                             MsfNotifyFocus);
#if 0                                                             
                                TPIc_widgetNotify( pWin->modId, pWin->windowHandle, MsfNotifyMoveResize );
#else
                                TPIc_widgetNotify( pWin->modId, pWin->windowHandle, MsfNotifyFocus);
#endif
                            }                                     
                        }
                        else
                        {
                            TPIa_widgetSetInFocus( pGadget->gadgetHandle, 1);
                        }

                        if( MSF_GADGET_PROPERTY_PEN_EVENT_RESPOND_DIRECT & pGadget->propertyMask )
                        {
                            (void)widgetMsfEventHandleFunc[wtGadget](
                                            pGadget->gadgetHandle,
                                            OPUS_FOCUS_PEN, 
                                            (void*)pPen, 
                                            &bHandled );
                            if( bHandled == OP_TRUE )
                            {
                                pScreen->penHandlerType = MSF_WIDGET_HANDLER;
                                pScreen->hPenHandleWidget = pGadget->gadgetHandle;
                            }
                        }

                        /* the button must trigger the state change event by only one-time click   */
                        else if( (wtGadget == MSF_BUTTON) && (pGadget->propertyMask & MSF_GADGET_PROPERTY_PEN_CLICK))
                        {
                            pScreen->penHandlerType = MSF_WIDGET_HANDLER;
                            pScreen->hPenHandleWidget = pGadget->gadgetHandle;
                        }
                        else
                        {
                            /* discard the succedent pen move or up events */
                            pScreen->penHandlerType = MSF_NO_HANDLER;
                        }
                    }
                    
                    return WIDGET_EVENT_HANDLED;
                }
            }
        }
        
        pGadgetNode = pGadgetNode->next;
    }
    
    return WIDGET_EVENT_NOT_HANDLE;
}


/*==================================================================================================
    FUNCTION:  widgetSelectGroupPenEventHandler

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static OP_BOOLEAN  widgetSelectGroupPenEventHandler
(
    MsfSelectgroup                         *pSelectgroup,
    TP_PEN_EVENT_STRUCTURE_T  *pPen,
    OP_BOOLEAN                          handle
)
{
    int             index;
    int             iBottomItem;
    MsfPosition  pos;
    MsfSize       size;
    choiceElement *pItem;
    choiceElement *pItemTemp, *pOldFocusItem = OP_NULL;
    OP_INT16 oldFocusIndex = 0, count = 0;
    
    if( !pSelectgroup || !pPen )
    {
        return OP_FALSE;
    }

    /*  if is pen move or pen down event but the event is handled, discard the event  */
    if( (pSelectgroup->choice.count <= 0 )
        ||(pPen->state == TP_PEN_STATE_PENMV )
        ||((pPen->state == TP_PEN_STATE_PENDN) && ( handle == OP_TRUE )))
    {
        return OP_FALSE;
    }

    if( pPen->state == TP_PEN_STATE_PENDN )
    {
        iBottomItem = pSelectgroup->choice.top_item + pSelectgroup->choice.elementsPerPage - 1;
        if( iBottomItem >= pSelectgroup->choice.count )
        {
            iBottomItem = pSelectgroup->choice.count - 1;
        }
        
        for( index = pSelectgroup->choice.top_item; index <= iBottomItem; index++  )
        {
            widgetChoiceGetItemRect( pSelectgroup, (OP_INT16)index, &pos, &size);
            if( POINT_IN_RECT(*pPen, pos, size) )
            {
                break;
            }
        }

        /*  the point in a item, switch the focus between the two items  */
        if( index <= iBottomItem )
        {
            if( index != pSelectgroup->choice.cur_item )
            {
                pItem = getChoiceElementByIndex( &pSelectgroup->choice, pSelectgroup->choice.cur_item );
                if( pItem )
                {
                    pItem->state &= ~ MSF_CHOICE_ELEMENT_FOCUSED;
                    
                    pItem = getChoiceElementByIndex( &pSelectgroup->choice, index );
                    pItem->state |= MSF_CHOICE_ELEMENT_FOCUSED;

                    pSelectgroup->choice.cur_item = index;

                    if( pSelectgroup->choice.bTimerStarted == OP_TRUE )
                    {
                        /* stop the cycle-scrolled timer  */
                        OPUS_Stop_Timer(OPUS_TIMER_CHOICITEM_SCROLL);
                        pSelectgroup->choice.bTimerStarted = OP_FALSE;
                    }
                    
                    widgetRedraw((void*)pSelectgroup, MSF_SELECTGROUP, pSelectgroup->gadgetData.gadgetHandle, OP_TRUE);
                }
            }
        }
    }
    else if( pPen->state == TP_PEN_STATE_PENUP )
    {
        widgetChoiceGetItemRect( pSelectgroup, (OP_INT16)pSelectgroup->choice.cur_item, &pos, &size);
        if( POINT_IN_RECT(*pPen, pos, size) )
        {
            pItem = getChoiceElementByIndex( &pSelectgroup->choice, pSelectgroup->choice.cur_item );
            if(pItem &&  !(pItem->state & MSF_CHOICE_ELEMENT_DISABLED ) )
            {
                if (pSelectgroup->choice.type == MsfMultipleChoice) 
                {
                    if (pItem->state & MSF_CHOICE_ELEMENT_SELECTED)
                    {
                        pItem->state &= (~MSF_CHOICE_ELEMENT_SELECTED);
                    }
                    else
                    {
                        pItem->state |= MSF_CHOICE_ELEMENT_SELECTED;
                    }
                }
                else  /* pChoice->type != MsfMultipleChoice */
                {
                    if( pItem->state & MSF_CHOICE_ELEMENT_SELECTED )
                    {
                        if( pSelectgroup->choice.type == MsfExclusiveChoice )
                        {
                            return OP_FALSE;
                        }
                        else if( pSelectgroup->choice.type == MsfImplicitChoice )
                        {
                            return OP_TRUE;
                        }
                    }
                    
                    /* clear the old selected state of the old selected item  */
                    pItemTemp = pSelectgroup->choice.firstElement;
                    oldFocusIndex = 0;
                    count = 0;
                    while( pItemTemp )
                    {
                        if(pItemTemp->state & MSF_CHOICE_ELEMENT_SELECTED)
                        {
                          pOldFocusItem = pItemTemp;
                          oldFocusIndex = count;
                        }
                        pItemTemp->state &= ~MSF_CHOICE_ELEMENT_SELECTED;
                        pItemTemp = pItemTemp->next;
                        count++;
                    }
                    
                    pItem->state |= MSF_CHOICE_ELEMENT_SELECTED;
                    if(OP_NULL != pOldFocusItem)
                    {
                        widgetChoiceDrawItemByIndex(pSelectgroup, pOldFocusItem, oldFocusIndex, OP_TRUE);
                    }     
                }
                widgetChoiceDrawItemByIndex(pSelectgroup, pItem, pSelectgroup->choice.cur_item, OP_TRUE);
                return OP_TRUE;
            }
        }
    }

    return OP_FALSE;
}

/*==================================================================================================
    FUNCTION:  widgetStringGadgetPenEventHandler

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static OP_BOOLEAN  widgetStringGadgetPenEventHandler
(
    MsfStringGadget                       *pStringGadget,
    TP_PEN_EVENT_STRUCTURE_T  *pPen,
    OP_BOOLEAN                          handle
)
{
    MsfPosition   pos;
    MsfSize        size;
    OP_BOOLEAN bRet = OP_FALSE;
    
    if( !pStringGadget || !pPen )
    {
        return bRet;
    }
    
    if( (pStringGadget->nPageCount <= 1 )
        ||(pPen->state == TP_PEN_STATE_PENMV )
        ||(pPen->state == TP_PEN_STATE_PENDN) && ( handle == OP_TRUE ))
    {
        return bRet;
    }

    widgetGadgetGetPosition((MsfGadget*)pStringGadget, &pos.x, &pos.y);
    size = pStringGadget->gadgetData.size;
    if(!POINT_IN_RECT(*pPen, pos, size ))
    {
        if( pPen->state == TP_PEN_STATE_PENUP )
        {
            pStringGadget->pageOperation = MSF_PAGE_NO_OPERATION;
        }
        
        return bRet;
    }
    
    if( pPen->state == TP_PEN_STATE_PENDN )
    {
        if( pStringGadget->singleLine )
        {
            size.width >>= 1;

            if( POINT_IN_RECT(*pPen, pos, size ) )
            {
                pStringGadget->pageOperation = MSF_PAGE_LEFT;
            }
            else
            {
                pStringGadget->pageOperation = MSF_PAGE_RIGHT;
            }
        }
        else
        {
            size.height >>= 1;
            
            if( POINT_IN_RECT(*pPen, pos, size ) )
            {
                pStringGadget->pageOperation = MSF_PAGE_UP;
            }
            else
            {
                pStringGadget->pageOperation = MSF_PAGE_DOWN;
            }
        }
    }
    else if( pPen->state == TP_PEN_STATE_PENUP )
    {
        if( pStringGadget->singleLine )
        {
            size.width >>= 1;

            if( POINT_IN_RECT(*pPen, pos, size ) )
            {
                if( pStringGadget->pageOperation == MSF_PAGE_LEFT )
                {
                    if(pStringGadget->iCurPageIndex > 0 )
                    {
                        pStringGadget->iCurPageIndex --;
                        bRet = OP_TRUE;
                    }
                }
            }
            else
            {
                if( pStringGadget->pageOperation == MSF_PAGE_RIGHT )
                {
                    if(pStringGadget->iCurPageIndex < (pStringGadget->nPageCount -1))
                    {
                        pStringGadget->iCurPageIndex ++;
                        bRet = OP_TRUE;
                    }
                }
            }
        }
        else
        {
            size.height >>= 1;
            
            if( POINT_IN_RECT(*pPen, pos, size ) )
            {
                if( pStringGadget->pageOperation == MSF_PAGE_UP )
                {
                    if(pStringGadget->iCurPageIndex > 0 )
                    {
                        pStringGadget->iCurPageIndex --;
                        bRet = OP_TRUE;
                    }                    
                }
            }
            else
            {
                if( pStringGadget->pageOperation == MSF_PAGE_DOWN )
                {
                    if(pStringGadget->iCurPageIndex < (pStringGadget->nPageCount -1))
                    {
                        pStringGadget->iCurPageIndex ++;
                        bRet = OP_TRUE;
                    }
                }
            }
        }
        pStringGadget->pageOperation = MSF_PAGE_NO_OPERATION;
    }

    return bRet;
}
#endif

/*==================================================================================================
    FUNCTION:  widgetScrollBarNotify

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetScrollBarNotify( MsfBar *pBar, SCROLLBAR_NOTIFICATION_T op )
{
    MsfGadget         *pGadget;
    MsfWindow        *pWin;
    MsfWidgetType   wt;
    int                    iFocusItemPos;
    
    if( !pBar || (pBar->barType != MsfVerticalScrollBar && pBar->barType != MsfHorizontalScrollBar) )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pGadget = (MsfGadget*)pBar;
    if( pGadget->cbStateChange )
    {
        (void)pGadget->cbStateChange(pGadget->modId, pGadget->gadgetHandle, MsfNotifyStateChange, (void*)(int)op );
        return TPI_WIDGET_OK;
    }
    
    if( !ISVALIDHANDLE(pBar->hRelatedWidget) )
    {
        if( pBar->gadgetData.parent != OP_NULL  
            && getWidgetType(pBar->gadgetData.parent->windowHandle) == MSF_PAINTBOX
            && pBar->gadgetData.parent->isFocused )
        {
            TPIc_widgetNotify(pBar->gadgetData.modId, pBar->gadgetData.gadgetHandle, MsfNotifyStateChange);
        }
    }
    else
    {
        pWin = (MsfWindow*)seekWidget(pBar->hRelatedWidget, &wt);
        if(!pWin)
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;  
        }
        if( IS_WINDOW(wt))
        {
            if( pBar->barType == MsfVerticalScrollBar )
            {
                pWin->scrollPos.y = pBar->value;
            }
            else
            {
                pWin->scrollPos.x = pBar->value;
            }

            widgetRedraw( (void*)pWin, wt, pBar->hRelatedWidget, OP_TRUE );

            return TPI_WIDGET_PAINTED;
        }
        else if(IS_GADGET(wt))
        {
            pGadget = (MsfGadget*)pWin;
            if( wt == MSF_SELECTGROUP )
            {// TODO: consider again
                MsfSelectgroup *pSelectgroup = (MsfSelectgroup*)pGadget;
                iFocusItemPos = pSelectgroup->choice.cur_item - pSelectgroup->choice.top_item;
                pSelectgroup->choice.top_item = pBar->value;
                iFocusItemPos += pBar->value;
                TPIa_widgetChoiceSetElemState(pBar->hRelatedWidget, iFocusItemPos, MSF_CHOICE_ELEMENT_FOCUSED);
            }
            else if(MSF_TEXTINPUT == wt)
            {
                 MsfTextInput *pTextInput = (MsfTextInput *)pGadget;
                 TBM_Scroll(&(pTextInput->input.txtInputBufMng), op);
                 /*switch (op)
                 {
                     case SB_NOTIFY_LINE_UP:
                        TBM_ScrollLineUp(&(pTextInput->input.txtInputBufMng)); 
                        break;
                     case SB_NOTIFY_LINE_DOWN:
                        TBM_ScrollLineDown(&(pTextInput->input.txtInputBufMng));
                        break;
                     case SB_NOTIFY_PAGE_UP:
                        TBM_ScrollPageUp(&(pTextInput->input.txtInputBufMng));
                        break;
                     case SB_NOTIFY_PAGE_DOWN:
	   	        TBM_ScrollPageDown(&(pTextInput->input.txtInputBufMng));
                        break;
                     default:
                        break;
                            
                 }   */        
            }
            else if(MSF_STRINGGADGET == wt)
            {
                if( pBar->barType == MsfVerticalScrollBar )
                {
                    MsfStringGadget *pStringGadget = (MsfStringGadget *)pGadget;
                    if( pStringGadget->singleLine == 0 )
                    {
                        pStringGadget->iCurPageIndex = pBar->value;
                    }
                }
            }
            
            if( pGadget->isFocused )
            {
                widgetRedraw( (void*)pGadget, wt, pBar->hRelatedWidget, OP_TRUE);
            }
            else if( pGadget->propertyMask & MSF_GADGET_PROPERTY_FOCUS )
            {
                TPIa_widgetSetInFocus( pBar->hRelatedWidget, MSF_TRUE );
            }
            /*
              *  If used the scrollbar to other gadgets, such as imagegadget, textinput,
              *  please add handle code here as selectgroup above
              */
        }
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  bmpanimation_event_handler

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int bmpanimation_event_handler 
(
  OP_UINT32 bmpanimationHandle, 
  OPUS_EVENT_ENUM_TYPE   event, 
  void                   *pMess,
  OP_BOOLEAN   *handle)
{
    *handle = OP_FALSE;
    return TPI_WIDGET_OK;
}


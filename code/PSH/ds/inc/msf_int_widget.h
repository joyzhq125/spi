#ifndef MSF_INT_WIDGET_H
#define MSF_INT_WIDGET_H
/*==================================================================================================

    HEADER NAME : msf_int_widget.h

    GENERAL DESCRIPTION
        General description of the contents of this header file.

    SEF Telecom Confidential Proprietary
    (c) Copyright 2003 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   Steven Lai        Cxxxxxx     Initial file creation. 
    07/30/2003   Zhuxq             P001026     Modify the type of MsfEditor's input member ( MsfInput ->  MsfTextInput  )
    07/26/2003   lindawang         P001074     add size and parser flag in image structure.
    04/08/2003   Zhuxq             P001052     Delete the bTimerStarted member from the structure MsfBar
    08/05/2003   Steven Lai        P001140     Change the method of handling screen focus
    08/13/2003   Zhuxq             P001147     Change the selectgroup show and menu show
    08/11/2003   Chenjs            P001091     Update text input widget.
    08/07/2003   Yuan Hui          P001169     modify the content of MsfSound struct.
    08/14/2003   Zhuxq             P001254     Fix draw image problem for wap.
    08/20/2003   Steven Lai        P001310     Improve the method of checking whether a widget is in focus
    08/22/2003   Zhuxq             P001290     Modify the MsfAction 
    08/23/2003   lindawang         P001350     solove the MMS Gif stop problem.
    08/28/2003   Tree Zhan         P001404     Modify low level widget about  create-image and create-sound
    08/29/2003   Zhuxq             P001396     Add scroll-contents feature to string gadget
    09/03/2003   Zhuxq             P001479     Move the hScreen from APP_wap_data to MsfDisplay, and rename it with  hScreenSave
    09/25/2003   Zhuxq             P001696     Re-implement the dialog
    09/27/2003   Zhuxq             P001727     Fix the bug: in some wap page the dialog connot be closed
    09/26/2003   linda wang        P001451     separate the gif play and show function.    
    10/17/2003   Zhuxq             C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq             P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq             P001883     Optimize the redraw strategy to reduce the redraw times
    12/12/2003   Zhuxq             P002192     fix bug for wap memory leak
    01/05/2004   Zhuxq             CNXT072     Add callback function to each widget in the widget system
    01/05/2004   Zhuxq             P002351     Update widget contents in batches    
    01/15/2004   Zhuxq             P002352     All editing actions of MsfTextInput turns to MsfEditor
    01/15/2004   Zhuxq             P002353     Using timer callback mechanism to support non-focus image gadget to play animation
    02/02/2004   Chenxiao          p002404     add textinput scrollbar  in the dolphin project
    02/16/2004   Zhuxq             P002469     Add button gadget to widget system and add modId parameter to event callback functions
    02/16/2004   chenxiao          p002474     add bitmap animation function in the dolphin project
    02/17/2004   Zhuxq             P002483     Adjust the display system of widget to show up the annunciator and fix some bugs
    02/17/2004   Zhuxq             P002492     Add Viewer window to widget system
    02/19/2004   Zhuxq             P002504     Add box gadget to widget system
    02/24/2004   chenxiao          p002530     modify WAP & MSG main menu  
    02/28/2004   Zhuxq             P002556     Fix some widget bugs
    03/19/2004   Zhuxq             P002687     Fix some bugs in widget system
    03/31/2004   Dingjianxin       P002754     Delete MsfImage gifFlag's value 0x02 for Timer symbol, start/stop Timer by app decided
    04/02/2004   zhuxq             P002789     Fix UMB bugs
    04/14/2004   Dingjianxin       P002961     replace the macro ALL_TEXT_EDIT_GOTO_EDITOR with DOLPHIN_VERSION
    04/22/2004   Andrew            P002659     Fix some bugs of projects that bases Dolphin
    04/24/2004   zhuxq             P003022     Fix some bugs in UMB
    06/04/2004   zhuxq             P005925     Correct issues found by PC-lint and fix some bugs
    06/18/2004   zhuxq             P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/05/2004   zhuxq             P006708     Fix  bugs: CNXT00007777, CNXT00004950, CNXT00009472, CNXT00008770, CNXT00008391, CNXT00009745
    07/20/2004   zhuxq             P007108     Add playing GIF animation in some wap page with multi-frame GIF
    07/30/2004   chenxiao          p007418     The cell phone will reset when line count in editor is over 255    
    08/07/2004   Hugh zhang        p007279     Add progress bar when sending or downloading mms.
    08/23/2004   Zhuxq             p007888     Fix the bug that the icon of 'abc' input method flashes when inputing in a wap page
    08/30/2004   zhuxq             c007998     improve the implementation of predefined string to reduce the times of memory allocation
    
    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include "msf_int.h"
#include "Rm_typedef.h"
#include "ds_int_def.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
//#define WIDGET_TRACE(_debugInfo_)  op_printf("[WIDGET] ");op_printf _debugInfo_;

#ifndef  WIDGET_TRACE
#define WIDGET_TRACE
#endif

#define MSF_ACCESSKEY                 (MSF_MAX_WIDGETTYPE)

/* the default size of page index buffer in stringgadget  */
#define STRINGGADGET_DEFAULT_BUF_PAGES            20

#define WIDGET_EVENT_HANDLED                                1
#define WIDGET_EVENT_NOT_HANDLE                           0

/* the result of widget event handling need repaint, used to selectgroup, etc  */
#define WIDGET_EVENT_HANDLED_NEED_PAINT                0x0004
/* the result of widget event handling need notify the module, used to selectgroup, etc   */
#define WIDGET_EVENT_HANDLED_NEED_NOTIFY              0x0008
/* the result of widget event handling
  * means when the choice reached the up-border and user needs next or previous contents.
  * used to selectgroup, etc
  */
#define WIDGET_EVENT_HANDLED_REQ_PREVIOUS_NOTIFY      0x0010
/* the result of widget event handling
  * means when the choice reached the bottom-border and user needs next or previous contents.
  * used to selectgroup, etc
  */
#define WIDGET_EVENT_HANDLED_REQ_NEXT_NOTIFY      0x0020

/* the result of widget event handling
  * means the box need get the previous page contents.
  * used to box
  */
#define WIDGET_EVENT_HANDLED_REQ_PRE_PAGE_NOTIFY      0x0040
/* the result of widget event handling
  * means the box need get the next page contents.
  * used to box
  */
#define WIDGET_EVENT_HANDLED_REQ_NEXT_PAGE_NOTIFY      0x0080


/* flag for Gif image */
#define GIF_FIRST_PARSE                             0x01   /* be the first parse */
#define GIF_TIMER_SET                               0x02   /* timer is being set. */
#define GIF_TIMER_REQUEST                           0x04   /* only for multi-frame, timer is requied */


/*  Define the widget timer range  */
#define  WIDGET_TIMER_START    MAX_OPUS_TIMER_EVENT
#define  WIDGET_TIMER_END       ( OPUS_EVENT_CUST_START - 1)

#define  WIDGET_TIMER_INVALID_ID    0

#define IS_WIDGET_TIMER_ID(iTimerId)  \
                 ((iTimerId) >= WIDGET_TIMER_START && (iTimerId) <= WIDGET_TIMER_END )
/*==================================================================================================
    MACROS
==================================================================================================*/

/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/
typedef enum 
{
    MSF_SCREEN = 0,
    
    MSF_PAINTBOX,
    MSF_FORM,
    MSF_DIALOG,
    MSF_MENU,
    MSF_MAINMENU,
    MSF_VIEWER,
    MSF_WAITINGWIN,
    MSF_EDITOR,

    MSF_WINDOW_END = MSF_EDITOR,
    
    MSF_SELECTGROUP,
    MSF_DATETIME,
    MSF_TEXTINPUT,
    MSF_STRINGGADGET,
    MSF_IMAGEGADGET,
    MSF_BUTTON,
    MSF_BMPANIMATION,
    MSF_BOX,
    MSF_BAR,

    MSF_GADGET_END = MSF_BAR,
    
    MSF_IMAGE,
    MSF_STRING,
    MSF_ICON,
    MSF_SOUND,
    MSF_STYLE,
    MSF_COLORANIM,
    MSF_MOVE,
    MSF_ROTATION,

    MSF_LOWLEVEL_WIDGET_END = MSF_ROTATION,

    MSF_ACTION,
    
    MSF_STRING_RES,
    
    MSF_MAX_WIDGETTYPE
}MsfWidgetType;


typedef enum 
{
    MSF_NO_HANDLER = 0,
    MSF_SKEY_HANDLER,
    MSF_IMM_HANDLER,
    MSF_WIDGET_HANDLER
}MsfPenHandlerType;

typedef enum 
{
    MSF_ALL_GADGETS =1,
    MSF_ALL_NF_GADGETS,
    MSF_ONLY_ONTOP_NF_GADGETS,
    MSF_ONLY_NON_ONTOP_NF_GADGETS,
    MSF_ONLY_ONTOP_GADGETS,
    MSF_ONLY_NON_ONTOP_GADGETS
}MsfGadgetScope;

typedef enum 
{
    MSF_PAGE_NO_OPERATION = 0,
    MSF_PAGE_LEFT,
    MSF_PAGE_RIGHT,
    MSF_PAGE_UP,
    MSF_PAGE_DOWN
}MsfStringGadgetOperation;


/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef int (* widget_release_func_t) (OP_UINT32);
typedef int (* widget_show_func_t)( void *, MsfWidgetType );
typedef int ( * widget_remove_func_t)(    void *,
                                        MsfWidgetType , 
                                        void *,
                                        MsfWidgetType);

typedef int (* widget_deallocate_func_t) (void *);

struct  tagMsfWidgetImm;
struct tagMsfBar;

/*brush*/
typedef struct tagMSFBRUSH
{
   MsfStyleHandle  style;
} MsfBrush; 

/*MsfTextInput*/
typedef struct 
{
    int maxSize;                                       /* max size (char)*/
    int maxCharCount;                             /* max Count of char this buffer can contain*/
    OP_UINT16    iValidCharCount;          /*current char count in buffer*/
    OP_UINT16    iLeftTopCharPos;      /* position(char index) of the first char displayed */
    OP_UINT16    iCursorPos;                 /*current cursor pos in buffer(char index)*/

    MsfStringFormat format;
    MsfStyleHandle  hStyle;
    
    OP_UINT8*        pTxtInputBuf;    

    /*for multi line textinput */
    OP_UINT16       cols;           /*count of col*/
    OP_UINT16		rows;          /*count of row*/

    OP_UINT16       *rowStartList;         /*first char index in text input buffer each row.Important note:
                                                              rowStartList is used like a ring buffer. The index of left top char 
                                                              in textinput view area is rowStartList[(pTBM->index + 1)%pTBM->cnt].
                                                             */
                  
    OP_UINT8		index;                      /*base index of rowStartList*/
    OP_UINT16       cnt;                         /*current element count of rowStartList: */

    OP_INT16		cursor_x;    /*cursor position x*/
    OP_INT16		cursor_y;    /*cursor position y*/
    OP_UINT16           TotalRows;  /* total rows */
    OP_UINT16           cur_line;     /* current line index in total rows*/
    OP_UINT16           top_line;    /* the top line index in the total rows*/
    OP_UINT16           iColsHasScrollbar;   /*count of column after scrollbar is added*/
    OP_BOOLEAN          bNeedScrollbar; /* wether scrollbar is needed */
    
}TBM_STRUCTURE_NAME_T;

typedef struct 
{
    MsfStringHandle initString;
    MsfStringHandle inputString;        /* Only used in text input copy */
    
    TBM_STRUCTURE_NAME_T txtInputBufMng;
    MsfTextType textType;
    int singleLine;
    MsfColor  color;
    MsfFont font;
    MsfTextProperty textProp;

    struct  tagMsfWidgetImm *pImm;       /* point to imm object  */
    OP_INT16              iCurImeIndex;  /* current ime index  */
}MsfInput;


typedef struct MsfString 
{
    OP_UINT8       modId;
    MsfStringHandle handle;
    MsfStringFormat format;
    OP_UINT8       *data;
    int             length;        /* byes of data , not include the MSF_NULL*/
    MsfStyleHandle  style;
    int             referenceCounter;
    OP_BOOLEAN        isReleased;     /* if true, cannot be made use again */
    // a parents list
}MsfString;

typedef struct MsfImage 
{
    MsfImageHandle   handle;
    OP_UINT8         modId;
    MsfImageFormat   format;
    OP_UINT8         *data;
    OP_UINT16        *colorPallet;
    OP_UINT8         *pGifBmpBuf;
    OP_UINT8         flag;        /* 0x01: first parse;  */
    MsfPosition      srcPos;           /* start position opposite the image (0,0) */
    MsfSize          size;
    OP_BOOLEAN       bOriginal;        /* if true, it's not parser data */
    int              bufferSize;       /* This is the total size of the image data */
    int              dataSize;
    OP_BOOLEAN       moreData;
    OP_BOOLEAN       isHoldDraw;
    MsfBrushHandle   brush;
    MsfStyleHandle   style;
    OP_UINT32        predefinedId;    /* the corresponding srcID if it is predefined string */
    int              referenceCounter;
    OP_BOOLEAN       isReleased;     /* if true, cannot be made use again */
}MsfImage;

typedef struct MsfIcon 
{
    MsfIconHandle  handle;
    OP_UINT8      modId;
    MsfIconType    type;
    MsfImageHandle image;
    MsfStyleHandle style;
    int            referenceCounter;
    OP_BOOLEAN       isReleased;     /* if true, cannot be made use again */
}MsfIcon;

typedef struct MsfSound
{
    MsfSoundHandle handle;
    OP_UINT8      modId;
    MsfSoundFormat format;
    char           *data;
    int            size;
    int             bufferSize;
    int            moreData;
    int            referenceCounter;
    OP_BOOLEAN       isReleased;     /* if true, cannot be made use again */
}MsfSound;

typedef struct MsfMove
{
    MsfMoveHandle handle;
    OP_UINT8     modId;
    MsfPosition       origin; 
    MsfPosition       destination; 
    int               duration; 
    int               nrOfSteps; 
    MsfMoveProperty   property; 
    int               repeat;
    int               referenceCounter;
    OP_BOOLEAN          isReleased;     /* if true, cannot be made use again */
}MsfMove;

typedef struct MsfRotation
{
    MsfRotationHandle handle;
    OP_UINT8         modId;
    int               startAngle; 
    int               endAngle; 
    int               duration; 
    int               nrOfSteps; 
    MsfMoveProperty   property; 
    int               repeat;
    int               referenceCounter;
    OP_BOOLEAN          isReleased;     /* if true, cannot be made use again */
}MsfRotation;

typedef struct MsfColorAnim
{
    MsfColorAnimHandle handle;
    OP_UINT8          modId;
    MsfColor           startColor; 
    MsfColor           endColor;
    int                duration; 
    int                nrOfSteps; 
    MsfMoveProperty    property; 
    int                repeat;
    int                referenceCounter;
    OP_BOOLEAN           isReleased;     /* if true, cannot be made use again */
}MsfColorAnim;

typedef struct MsfStyle
{
    MsfStyleHandle     handle;
    OP_UINT8          modId;
    MsfColor           color;
    MsfColor           backgroundColor;
    MsfPattern         foreground;
    MsfPattern         background;
    MsfLineStyle       lineStyle;
    MsfFont            font;
    MsfTextProperty    textProperty;
    int                referenceCounter;
    OP_BOOLEAN           isReleased;     /* if true, cannot be made use again */
}MsfStyle;

typedef struct tagMsfAccessKey
{
    MsfAccessKeyHandle  handle;
    OP_UINT8           modId;
    char*               accessKeyDefinition;
    int                 override;
    int                 index;
    int                 visualise;
    struct tagMsfAccessKey     *next;
}MsfAccessKey;

/* MsfAction */
typedef struct tagMsfAction
{
    MsfActionHandle actionHandle;
    OP_UINT8       modId;
    MsfStringHandle label;
    MsfActionType   actionType;
    int             priority;
    int             propertyMask;
    int                referenceCounter;
    OP_BOOLEAN        isReleased;     /* if true, cannot be made use again */
    OP_UINT8        **ppSoftKey;
    
    widget_action_callback_t    cbActionNotify;    /*  action notify callback function  */
}MsfAction;


/* MsfGadget */
typedef struct tagMsfGadget
{
    MsfGadgetHandle gadgetHandle;
    OP_UINT8       modId;
    MsfStringHandle title;
    MsfSize         size;
    int             propertyMask;
    MsfLineStyle    borderStyle;
    MsfStyleHandle  style;
    MsfPosition     position;
    MsfAlignment    alignment;
    int             referenceCounter;
    OP_BOOLEAN        isReleased;     /* if true, cannot be made use again */
    OP_BOOLEAN        isFocused;
    struct tagMsfWindow       *parent;
    //void            *data;
    MsfAccessKey    *accessKey;

    MsfGadgetHandle  hVScrollBar;
    MsfGadgetHandle  hHScrollBar;
    
    OP_UINT8 *softKeyList[3];                   /*  use for setting SoftKey  */

    OP_BOOLEAN    bInBatches;                 /*  use for update contents in bacthes, 
                                                              *  if the flag is set, the updated contents will not 
                                                              *  be drawn to screen
                                                              */
    widget_state_change_callback_t   cbStateChange;    /* state change callback function    */
}MsfGadget;

/* MsfStringGadget */
typedef struct MsfStringGadget
{
    MsfGadget       gadgetData;
    
    /* NOTE: add other properties below */
    MsfStringHandle text;
    int             singleLine;

    OP_BOOLEAN bNeedAdjust;                   /* indicate if need re calc the positions of every page */
    OP_UINT16  pageStarIndexListBuf[STRINGGADGET_DEFAULT_BUF_PAGES];  /* store the start position of every page  */
    OP_UINT16  *pPageStarIndexList;         /* if the total pages is beyond 20, will realloc memory to store the start position of every page  */
    OP_INT16    nPageCount;                      /* the page count*/
    OP_UINT16  iCurPageIndex;                  /* the current page start index */
    MsfStringGadgetOperation  pageOperation;
}MsfStringGadget;

/* choiceElement */
typedef struct choiceElement
{
    MsfIconType     icon;
    MsfStringHandle string1;
    MsfStringHandle string2;
    MsfImageHandle  image1;
    MsfImageHandle  image2;
    int             state;     // selected, isDisable, or isFocused
    struct choiceElement   *next;
}choiceElement;

/* MsfChoice */
typedef struct MsfChoice
{
    MsfChoiceType      type; // implicit, exclusive, multiple
    MsfElementPosition elementPos; 
    int                bitmask; //Specifies which element attributes that are to be shown.
    int                count;
    int                cur_item; /* the index of focused element */
    int                top_item; /* the current top element in screen */
    int                elementsPerPage;  /* maximum line numbers can be displayed concurrently */
    OP_BOOLEAN           scrollCircularly; /* indicates whether or not the list should be scroll circularly */
    choiceElement*     firstElement;

    /* use for item string  scroll-period-display  */
    OP_INT16   iItemVisibleLen;       /*  the number of eng-char in a item visible area  */
    OP_INT16   iCurItemStringLen;   /* the number of eng-char in current  item string  */
    OP_INT16   iCurItemStringPos;   /* the move cursor  */
    OP_BOOLEAN  bTimerStarted;          /* if MSF_TRUE, means the timer already started   */
}MsfChoice;

/* MsfSelectgroup */
typedef struct MsfSelectgroup
{
    MsfGadget       gadgetData;
    
    /* NOTE: add other properties below */
    MsfChoice       choice;
    
    OP_UINT16         showStyle;          /*  style for drawing  */

    OP_BOOLEAN     bItemPosCalculated;   /*  indicate if need to recalculate the item position info  */
    MsfPosition         clientPos;                     /*  the start position of the client area in selectgroup  */
    /* 
      * the height of the client area in selectgroup, 
      * the width of the client area is itemSize.width 
      */
    OP_INT16           iClientHeight;              
    MsfSize               itemSize;                     /*  the size of a item area  */
    OP_INT16           iSpaceY;                      /*  the space between two item area  */
    OP_INT16           iItemDeltaY;               /*   the content start position relative to a item  area  */
    
}MsfSelectgroup;

/* MsfTextInput */
typedef struct MsfTextInput
{
    MsfGadget       gadgetData;
    
    /* NOTE: add other properties below */
    MsfInput        input;
    MsfWindowHandle   hEditor;  
}MsfTextInput;

/* MsfImageGadget */
typedef struct MsfImageGadget
{
    MsfGadget       gadgetData;
    
    /* NOTE: add other properties below */
    MsfImageHandle  image;
    MsfImageZoom    imageZoom;
    OP_UINT32         iTimerId;
}MsfImageGadget;

/* MsfDateTime */
typedef struct MsfDateTimeStruct
{
    MsfGadget       gadgetData;
    
    /* NOTE: add other properties below */
    MsfDateTimeType mode;
    MsfTime         time;
    MsfDate         date;
}MsfDateTimeStruct;

/* MsfBar */
typedef struct tagMsfBar
{
    MsfGadget  gadgetData;
    
    /* NOTE: add other properties below */
    MsfBarType barType;
    int        maxValue;
    int        value;                   /* increase  by step  */
    
    int        lineStep;         
    int        pageStep;              
    MSF_UINT32  hRelatedWidget;
}MsfBar;

/* MsfButton */
typedef struct tagMsfButtonGadget
{
    MsfGadget  gadgetData;
    
    /* NOTE: add other properties below */
    MsfHorizontalPos            labelAlignment;           /* the label's position  */
    MSF_UINT32         iNormalRes;     /* the resource ids of normal background  */
    MSF_UINT32         iFocusRes;       /* the resource ids of focused background  */
}MsfButtonGadget;

/* bitmap animation gadget */
typedef struct tagMsfBmpAnimation
{
    MsfGadget         gadgetData;

    /* NOTE: add other properties below */
    RM_INT_ANIDATA_T  *pAniData;  /*point to animation structure*/
    OP_UINT32         timer_id;   /*timer id related to the bitmap animation*/
    OP_UINT32         interval;     /*interval between two frame of animation*/
    OP_BOOLEAN        bPeriodicTimer; /* period timer or one short timer */ 
    OP_BOOLEAN        bLoop;    /* indicates if playing animation periodically */       
    OP_UINT16         iCurFrame; /*current frame playing */
}MsfBmpAnimation; 

/*MsfBox*/
typedef struct tagMsfBox
{
    MsfSelectgroup  selectgroup;

    int     nItemCount;   /* the count of box items */
    int     iCurPos;       /* the index of current item of box */
}MsfBox;

/* window */
typedef struct GadgetsInWindow
{
    MsfGadget              *gadget;
    struct GadgetsInWindow *prev;
    struct GadgetsInWindow *next;
}GadgetsInWindow;

/*
 * If there are some gadgets attached to a window. The nodes in "children" list should
 * be arranged according to the order in which the gadgets got focus.
 *
 * NOTE: THE PROPERTY "focusedChild" IS DEFERENT FROM THAT OF MsfScreen
 *
 * If there is a focused gadget, "focusedChild" should point to the corresponding node
 * of this gadget. If there is no gadget attached to this window, it should be MSF_NULL.
 */
typedef struct tagMsfWindow
{
    MsfWindowHandle   windowHandle;
    OP_UINT8         modId;
    MsfStringHandle   title;
    MsfPosition       position;                 
    MsfPosition       clientOrigin;             /*  the client area origin, not include the title, it is the SCREEN coordinary   */
    MsfPosition       scrollPos;                /*  the scrolled position of  client view  */
    MsfAlignment      alignment;
    MsfSize           size;
    int               propertyMask;
    MsfLineStyle      borderStyle;
    MsfStyleHandle    style;
    MsfStringHandle   ticker;
    int               referenceCounter;
    OP_BOOLEAN          isReleased;     /* if true, cannot be made use again */
    OP_BOOLEAN        isFocused;
    MsfAction         *actionlist[8];   /* For each MsfActionType, there can be only one such action attached to a window  */
    struct tagMsfScreen  *parent;
    OP_BOOLEAN          hasBeenFocused;     /* 1: has been focused; 0: has not been focused */
    GadgetsInWindow   *focusedChild; /* current focus gadget node */
    GadgetsInWindow   *children;
    MsfAccessKey      *accessKey;
    OP_UINT32        registedEvents;  /* refer to TPIa_widgetHandleMsfEvt() */

    OP_UINT8 *softKeyList[3];                   /*  use for setting SoftKey  */
    
    OP_BOOLEAN    bInBatches;                 /*  use for update contents in bacthes, 
                                                              *  if the flag is set, the updated contents will not 
                                                              *  be drawn to screen
                                                              */
    OP_BOOLEAN    bDrawing;               /* indicates if the window is drawing. used to reduce times of refreshing screen */
    
    widget_state_change_callback_t   cbStateChange;    /* state change callback function    */
    widget_register_event_callback_t  cbRegEventNotify;  /* registered event callback function */
}MsfWindow;

/* Paintbox */
typedef struct MsfPaintbox
{
    MsfWindow       windowData;
    
    /* NOTE: add other properties below */
    MsfSize         redrawArea;
    OP_BOOLEAN        isHoldDraw;
    MsfBrushHandle  brush;
}MsfPaintbox;

/* Form */
typedef struct MsfForm
{
    MsfWindow       windowData;
    
    /* NOTE: add other properties below */
   struct tagMsfBar  *pVScrollBar;
   struct tagMsfBar  *pHScrollBar;
   OP_INT16        iMaxScrollAreaX;/* max X area of horizontal scrollbar */
   OP_INT16        iMaxScrollAreaY;/* max Y area of vertical scrollbar */
   
   
    
}MsfForm; 

/* MsfEditor */
typedef struct MsfEditor
{
    MsfWindow       windowData;
    
    /* NOTE: add other properties below */
    MsfTextInput      *input;
    MSF_UINT32    hRelatedTextInput;
}MsfEditor;

/* MsfMenu */
typedef struct MsfMenuWindow
{
    MsfWindow       windowData;
    
    /* NOTE: add other properties below */
    MsfSelectgroup        *pSelectgroup;
    OP_BOOLEAN        isAppMenu;
}MsfMenuWindow;

/* MsfDialog */
typedef struct MsfDialog
{
    MsfWindow        windowData;
    
    /* NOTE: add other properties below */
    MsfStringHandle  dialogText;
    MsfDialogType    type;
    OP_UINT32       timeoutTime;
    OP_UINT32       iTimerId;
    OP_INT16            inputCount;
    OP_BOOLEAN        bNeedAdjustInputPos;
    MsfScreenHandle  nextScreen; 
    MsfWindowHandle  nextWindow;
}MsfDialog;


/* main menu window */
typedef struct MsfMainMenuWindow
{
  MsfWindow   windowData;

  /* NOTE: add other properties below */
  MsfSelectgroup *pSelectgroup;
  MsfButtonGadget *pUpArrowButton;    
  MsfButtonGadget *pDownArrowButton;
  MsfBmpAnimation *pBmpAni;
}MsfMainMenuWindow;

/* Viewer */
typedef struct tagMsfViewer
{
    MsfWindow        windowData;
    
    /* NOTE: add other properties below */
    MsfStringGadget   *pStringGadget;
}MsfViewer;

/* WaitingWin */
typedef struct tagMsfWaitingWin
{
    MsfWindow        windowData;

    /* NOTE: add other properties below */
    OP_BOOLEAN       bShowBg;         /* if be true, will fill the background */
    OP_BOOLEAN       bProgressBar;   /* if be true, means the hBar is valid, and the hBmpAni is invalid. */
    OP_BOOLEAN       bPromptIsUp;   /* if be true, means the prompt string is up  */
    MsfGadgetHandle  hPrompt;         /* the prompt string gadget  */
    MsfGadgetHandle  hBmpAni;         /* the bitmap animation gadget */
    MsfGadgetHandle  hBar;               /* the progresss bar  */

    MSF_UINT32        hRelatedWin;   /* the related window */
}MsfWaitingWin;

/* screen */
typedef struct WindowsInScreen
{
    MsfWindow         *window;
    OP_BOOLEAN     bVisible;
    struct WindowsInScreen *prev;
    struct WindowsInScreen *next;
}WindowsInScreen;

/*
 * If there are some windows attached to a screen. The nodes in "children" list should
 * be arranged according to the order in which the windows got focus.
 *
 * NOTE: THE PROPERTY "focusedChild" IS DEFERENT FROM THAT OF MsfScreen
 *
 * If there is a focused window, "focusedChild" should be the handle of this window. If
 * there is no window attached to this screen or no windows has focus currently, this
 * property should be INVALID_HANDLE.
 * When a non-focus window is set focused, its corresponding node in this list should be
 * move to the first place.
 */
typedef struct tagMsfScreen
{
    MsfScreenHandle screenHandle;
    OP_UINT8       modId;
    int             notify;
    MsfStyleHandle  style;
    OP_BOOLEAN        isFocused;
    OP_BOOLEAN        hasBeenFocused;     /* 1: has been focused; 0: has not been focused */
    MsfWindowHandle focusedChild; /* for screen, focusedChild should be the same as the first child's handle */
    WindowsInScreen *children;
    MsfAction       *actionlist[8];   /* For each MsfActionType, there can be only one such action attached to a screen  */
    MsfAccessKey    *accessKey;
    OP_UINT32      registedEvents;  /* refer to TPIa_widgetHandleMsfEvt() */

    OP_UINT8       *softKeyList[3];      /*  use for setting SoftKey  */
    OP_UINT32     hKeyPressEventHandler; /*record the key press event handler */
    OP_UINT16     iKeyPressCode;

    MsfPenHandlerType   penHandlerType;
    MSF_UINT32            hPenHandleWidget;
    MsfPosition              penDownPosInSKey;
    struct  tagMsfWidgetImm *pImm;
    
    OP_BOOLEAN    bInBatches;                 /*  use for update contents in bacthes, 
                                                              *  if the flag is set, the updated contents will not 
                                                              *  be drawn to screen
                                                              */
    widget_state_change_callback_t   cbStateChange;    /* state change callback function */
    widget_register_event_callback_t  cbRegEventNotify;  /* registered event callback function */
}MsfScreen;

typedef struct ScreenInDisplay
{
    MsfScreen *screen;
    struct ScreenInDisplay *next;
}ScreenInDisplay;

typedef struct MsfDisplay
{
    MsfScreen          *focusedScreen;   /* Since currently not all application use screen, there may be no screen in focus, so we need this item */
    ScreenInDisplay *screenList;
    MsfScreenHandle  hScreenSave;
}MsfDisplay;

/*================================================================================================*/
#endif  /* MSF_INT_WIDGET_H */


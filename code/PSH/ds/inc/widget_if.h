#ifndef _WIDGET_IF_H
#define _WIDGET_IF_H
/*====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/18/2004   zhuxq          P001345      Optimize code for modularization
    07/23/2004   zhuxq          P007215      Prevent freeze in wap
    08/06/2004   zhuxq          P007577      adjust the layout of MMS/SMS for B1,  and fix some bugs
    08/07/2004   Hugh zhang     p007279      Add progress bar when sending or downloading mms.
    08/18/2004   zhuxq          P007793      Trace widget memory issues, and reduce allocation times of MsfString
    08/18/2004   Hugh zhang     p007815      Support vmd media for some projects    
    08/23/2004   Zhuxq          p007888      Fix the bug that the icon of 'abc' input method flashes when inputing in a wap page
    08/30/2004   zhuxq          c007998      improve the implementation of predefined string to reduce the times of memory allocation
    
====================================================================================================*/
#include "msf_def.h"
#include "widget_defs.h"


#ifndef CHECK_MEMORY_LEAK
//#define  WIDGET_DEBUG
#endif

#ifdef   WIDGET_DEBUG
void *  WIDGET_DEBUG_ALLOC(MSF_UINT32 size, const char *filename, int lineno);
void  WIDGET_DEBUG_FREE( void  *p, const char *filename, int lineno );

/*
Alloc and free macro
*/
#define WIDGET_ALLOC(_size_)      WIDGET_DEBUG_ALLOC(_size_, __FILE__, __LINE__)
#define WIDGET_FREE(_pointer_)    WIDGET_DEBUG_FREE((_pointer_), __FILE__, __LINE__); ( _pointer_)= OP_NULL

#define WIDGET_NEW(type)   ((type *)WIDGET_ALLOC(sizeof(type)))

#else
/*
Alloc and free macro
*/
#define WIDGET_ALLOC   op_alloc
#define WIDGET_FREE     op_free
#define WIDGET_NEW( type)      op_new(type)

#endif

/**********************************************************************
 * Widget
 **********************************************************************/
#define MsfScreenHandle     MSF_UINT32
#define MsfWindowHandle     MSF_UINT32
#define MsfGadgetHandle     MSF_UINT32
#define MsfActionHandle     MSF_UINT32
#define MsfImageHandle      MSF_UINT32
#define MsfStringHandle     MSF_UINT32
#define MsfFontFamilyNumber MSF_UINT32

#define MsfIconHandle       MSF_UINT32
#define MsfSoundHandle      MSF_UINT32
#define MsfMoveHandle       MSF_UINT32
#define MsfRotationHandle   MSF_UINT32
#define MsfColorAnimHandle  MSF_UINT32
#define MsfStyleHandle      MSF_UINT32
#define MsfBrushHandle      MSF_UINT32
#define MsfAccessKeyHandle  MSF_UINT32


#define TPI_WIDGET_OK                              0
#define TPI_WIDGET_PAINTED                     3
#define TPI_WIDGET_HW_SCROLLING       4
#define TPI_WIDGET_HW_CURSOR_LOCATING   5

#define  TPI_WIDGET_NOT_HANDLED            100

/* Error codes */
#define TPI_WIDGET_ERROR_UNEXPECTED                 -1
#define TPI_WIDGET_ERROR_NOT_SUPPORTED              -2
#define TPI_WIDGET_ERROR_BAD_HANDLE                 -3
#define TPI_WIDGET_ERROR_INVALID_PARAM              -4
#define TPI_WIDGET_ERROR_NON_FOCUS                  -5
#define TPI_WIDGET_ERROR_NOT_FOUND                  -6
#define TPI_WIDGET_ERROR_RESOURCE_LIMIT             -7
#define TPI_WIDGET_ERROR_IS_RELEASED                -8
#define TPI_WIDGET_ERROR_CONTENT_TOO_LONG     -9

typedef struct {
  MSF_INT16 x;
  MSF_INT16 y;
}MsfPosition;


typedef struct {
  MSF_INT16 height;
  MSF_INT16 width;
} MsfSize;

typedef enum {
  MsfHigh = 0,
  MsfMiddle = 1,
  MsfLow = 2,
  MsfVerticalPosDefault = 3
} MsfVerticalPos;

typedef enum {
  MsfLeft = 0,
  MsfCenter = 1,
  MsfRight = 2,
  MsfHorizontalPosDefault = 3
} MsfHorizontalPos;

typedef struct {
  MsfVerticalPos verticalPos;
  MsfHorizontalPos horisontalPos;
}MsfAlignment;



typedef struct {
  MsfSize displaySize;/**/
  int isTouchScreen;
  int hasDragAbility;
  int isColorDisplay;
  int numberOfColors;
  MSF_INT32 supportedMsfEvents; /*Deprecated*/
} MsfDeviceProperties;

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

typedef enum {
  MsfImplicitChoice = 0,
  MsfExclusiveChoice = 1,
  MsfMultipleChoice = 2,
  MsfExclusiveChoiceDropDown = 3
} MsfChoiceType;


#define MSF_CHOICE_ELEMENT_SELECTED   0x01
#define MSF_CHOICE_ELEMENT_DISABLED   0x02
#define MSF_CHOICE_ELEMENT_FOCUSED    0x04 


typedef struct {
  int iconPos;
  int indexPos;
  int string1Pos;
  int string2Pos;
  int image1Pos;
  int image2Pos;
} MsfElementPosition;

typedef enum {
  MsfAlert    = 0,
  MsfConfirmation = 1,
  MsfWarning    = 2,
  MsfInfo     = 3,
  MsfError    = 4,
  MsfPrompt   = 5
} MsfDialogType;

typedef enum {
MsfInteractiveBar = 0,
MsfProgressBar = 1,
MsfVerticalScrollBar = 2,
MsfHorizontalScrollBar = 3
} MsfBarType ;

typedef enum {
   MsfTimeType = 0,
   MsfDateType = 1,
   MsfDateTime = 2
} MsfDateTimeType;

typedef enum {
  MsfText = 0,
  MsfName = 1,
  MsfIpAddress = 2,
  MsfUrl = 3,
  MsfEmailAddress = 4,
  MsfPhoneNumber = 5,
  MsfNumeric = 6,
  MsfNumber = 7,
  MsfInteger = 8,
  MsfPasswordText = 9,
  MsfPasswordNumber = 10,
  MsfPredictive = 11,
  MsfFile = 12,
  MsfFilePath = 13,
  MsfSmsEms = 14,
  MsfNoTextType = 15
} MsfTextType;

typedef struct {
  int day;
  int month;
  int year;
} MsfDate;

typedef struct {
  int hours;
  int minutes;
  int seconds;
} MsfTime;


typedef enum {
  MsfButton = 0,
  MsfButtonPressed = 1,
  MsfRadioButton = 2,
  MsfRadioButtonPressed = 3,
  MsfCheckBox = 4, 
  MsfCheckBoxChecked = 5, 
  MsfKey = 6,
  MsfBullet = 7,
  MsfTick = 8,
  MsfSquare = 9,
  MsfCircle = 10,
  MsfDisk = 11,
  MsfArrowHead = 12,
  MsfIcon1 = 13,
  MsfIcon2 = 14,
  MsfIcon3 = 15,
  MsfIcon4 = 16,
  MsfIconNone = 100
} MsfIconType;

typedef enum {
  MsfPress = 0,
  MsfRelease = 1,
  MsfRepeat = 2
}MsfEventClass;

typedef enum {
  MsfKey_0 = 0, MsfKey_1 = 1, MsfKey_2 = 2, MsfKey_3 = 3, MsfKey_4 = 4,
  MsfKey_5 = 5, MsfKey_6 = 6, MsfKey_7 = 7, MsfKey_8 = 8, MsfKey_9 = 9, 
  MsfKey_Star = 10, MsfKey_Pound = 11, MsfKey_Up = 12, MsfKey_Down = 13, 
  MsfKey_Left = 14, MsfKey_Right = 15, MsfKey_Select = 16,
  MsfKey_Clear = 17, MsfKey_Yes = 18, MsfKey_No = 19, MsfKey_Menu = 20,
  MsfKey_Soft1 = 21, MsfKey_Soft2 = 22, MsfKey_Soft3 = 23,
  MsfKey_Soft4 = 24, MsfKey_Function1 = 25, MsfKey_Function2 = 26,
  MsfKey_Function3 = 27, MsfKey_Function4 = 28, MsfPointer = 29,
  MsfKey_Undefined = 30, MsfKey_PageUp = 31, MsfKey_PageDown = 32 
} MsfEventType;

#define MSF_EVENT_MODE_SHIFT      0x01
#define MSF_EVENT_MODE_ALT	      0x02 
#define MSF_EVENT_MODE_CTRL       0x04
#define MSF_EVENT_MODE_MULTITAP   0x08

typedef enum {
  MsfNotifyLostFocus = 0,
  MsfNotifyFocus = 1,
  MsfNotifyStateChange = 2,
  MsfNotifyMoveResize = 3,
  MsfNotifyResourceLoaded = 4,
  MsfNotifyResourceFailed = 5,
  MsfNotifyMarqueeDone = 6,
  MsfNotifyClick = 7,
  MsfNotifyRequestNewContent = 8,
  MsfNotifyPenMoveOut = 9,
  MsfNotifyPenMoveOutUp = 10
} MsfNotificationType;

typedef struct {
  MSF_UINT8  eventMode;
  MsfEventClass eventClass;
  MsfEventType eventType;
  MsfPosition position;
} MsfEvent;


typedef struct {
  int r;
  int g;
  int b;
} MsfColor;

typedef enum {
  MsfFontNormal = 0,
  MsfFontOblique = 1,
  MsfFontItalic = 2
} MsfFontStyle;

typedef enum {
  MsfGenericFontSerif = 0,
  MsfGenericFontSansSerif = 1,
  MsfGenericFontCursive = 2,
  MsfGenericFontFantasy = 3,
  MsfGenericFontMonospace = 4
}MsfGenericFont;

typedef struct {
  MsfFontStyle fontStyle;
  int size;
  int weight;
  int stretch;
  int variant;
  MsfFontFamilyNumber fontFamily;   
} MsfFont;

typedef enum {
  MsfNone = 0,
  MsfDotted = 1,
  MsfDashed = 2,
  MsfSemiDotted = 3,
  MsfSolid = 4,
  MsfDouble = 5,
  MsfGroove = 6,
  MsfRidge = 7,
  MsfInset = 8,
  MsfOutset = 9
} MsfLine;

typedef struct {
  int thickness;
  MsfLine style;
} MsfLineStyle;

#define MSF_TEXT_DECORATION_UNDERLINE       0x01
#define MSF_TEXT_DECORATION_OVERLINE        0x02
#define MSF_TEXT_DECORATION_STRIKETHROUGH   0x04
#define MSF_TEXT_DECORATION_SHADOW          0x08
#define MSF_TEXT_DECORATION_BLINK           0x10


typedef struct {
  int decoration;
  int letterSpacing;
  int wordSpacing;
  int lineSpacing;
  int capitalization; 
  MsfAlignment alignment;	
} MsfTextProperty;



typedef enum {
  MsfNoPadding = 0,
  MsfHorisontal = 1,
  MsfVertical = 2,
  MsfTile = 3,
  MsfFullPadding = 4
}MsfPadding;


typedef struct {
  MsfImageHandle image;
  MsfAlignment alignment;
  MsfPadding padding;
}MsfPattern;


typedef enum {
  MsfImageZoom100 = 0,
  MsfImageZoom10 = 1,
  MsfImageZoom25 = 2,
  MsfImageZoom50 = 3,
  MsfImageZoom75 = 4,
  MsfImageZoom125 = 5,
  MsfImageZoom150 = 6,
  MsfImageZoom200 = 7,
  MsfImageZoom400 = 8,
  MsfImageZoom800 = 9,
  MsfImageZoomAutofit = 10
}MsfImageZoom;


typedef enum {
  MsfBmp = 1,
  MsfPictogram = 2,
  MsfGif = 0x1d,
  MsfJpeg = 0x1e,
  MsfPng = 0x20,
  MsfWbmp = 0x21
} MsfImageFormat;

typedef enum {
  MsfUtf8 = 106,
  MsfUnicode = 107 /* add by Tree for test*/
} MsfStringFormat;

typedef enum {
  MsfAmr = 0,
  MsfMp3 = 1,
  MsfMidi = 2,
  MsfWav = 3,
  MsfMmf = 4,
  MsfVmd = 5
} MsfSoundFormat;

typedef enum {
  MsfClick = 0, MsfTone = 1, MsfSound_1 = 2, MsfSound_2 = 3, MsfSound_3 = 4, 
  MsfSound_4 = 5, MsfSound_5 = 6, MsfSound_6 = 7, MsfSound_7 = 8, MsfSound_8 = 9, 
  MsfSignal_1 = 10, MsfSignal_2 = 11, MsfSignal_3 = 12, MsfSignal_4 = 13, 
  MsfSignal_5 = 14, MsfSignal_6 = 15, MsfSignal_7 = 16, MsfSignal_8 = 17, 
  MsfMelody_1 = 18, MsfMelody_2 = 19, MsfMelody_3 = 20, MsfMelody_4 = 21, 
  MsfMelody_5 = 22, MsfMelody_6 = 23, MsfMelody_7 = 24, MsfMelody_8 = 25
} MsfPredefinedSound;

typedef enum {
  MsfMoveNormal = 0,
  MsfMoveAccelerate = 1,
  MsfMoveDescelerate = 2
} MsfMoveProperty;

typedef enum {
  MsfNoDecoration = 0,
  MsfDecorationBlink = 1,
  MsfDecorationLasVegas = 2,
  MsfDecorationAnts = 3,
  MsfDecorationShimmer = 4,
  MsfDecorationSparkle = 5
} MsfDecoration;

typedef enum {
  MsfMarqueeNone = 0,
  MsfMarqueeScroll = 1, 
  MsfMarqueeSlide = 2, 
  MsfMarqueeAlternate = 3,
  MsfMarqueeInsideScroll = 4
} MsfMarqueeType;


typedef enum {
  MsfResourceFile = 0,
  MsfResourcePipe = 1,
  MsfResourceBuffer = 2
} MsfResourceType;

/* Add by Zhuxq, the enum is used to choice and scrollbar  */
typedef enum 
{
    SB_NOTIFY_LINE_UP = 1,
    SB_NOTIFY_LINE_DOWN,
    SB_NOTIFY_PAGE_UP,
    SB_NOTIFY_PAGE_DOWN,
    SB_NOTIFY_INVALID
}SCROLLBAR_NOTIFICATION_T;

typedef struct {
  const char* data;
  int dataSize;
  int moreData;
} MsfDirectData;

typedef union {
  const char* resource;
  MsfDirectData* directData;
} MsfCreateData;

/*  Add by Zhuxq  */
/*  widget state change callback function definition, applyed to screen, window and gadget objects  */
typedef int (* widget_state_change_callback_t)( MSF_UINT8, /* modId */
                                                MSF_UINT32,             /*  widget handle */
                                                MsfNotificationType,    /*  state notification  */
                                                void * );   /*  extra data. it is used  ONLY the MsfNotificationType is MsfNotifyStateChange 
                                                                *  and MsfNotifyRequestNewContent. The meanings as following: 
                                                                *  as to   MsfNotificationType:
                                                                *                   focus item indes                                    -----    selectgroup   
                                                                *                   page flip ( SCROLLBAR_NOTIFICATION_T )  -----    scrollbar
                                                                *
                                                                *  as to MsfNotifyRequestNewContent:                           -----    selectgroup
                                                                *                  only the  page flip  ( in SCROLLBAR_NOTIFICATION_T ) 
                                                                */

/*  widget action callback function definition, only applyed to window and screeen objects  */
typedef int (* widget_action_callback_t)(  MSF_UINT8, /*  modId */
                                                MSF_UINT32,                   /*  widget handle */
                                                MSF_UINT32 );                 /*  action handle  */

/*  widget action callback function definition, only applyed to window and screeen objects  */
typedef int (* widget_register_event_callback_t)( MSF_UINT8, /* modId */
                                                MSF_UINT32,                   /*  widget handle */
                                                MSF_UINT32  ,                 /* the OPUS event  */
                                                void  *);                         /*  the OPUS event parameter  */
                                                
/*  widget timer callback function definition, applyed to screen, window and gadget objects */
typedef int (* widget_timer_callback_t)( MSF_UINT32,   /*  widget handle  */
                                                MSF_UINT32  ); /*  timer id   */


/****************************************************************
  Widget General
 ***************************************************************/
int TPIa_widgetDeviceGetProperties(MsfDeviceProperties* theDeviceProperties);

int TPIa_widgetSetInFocus (MSF_UINT32 handle, int focus);

int TPIa_widgetHasFocus (MSF_UINT32 handle);

int TPIa_widgetRemoveAllScreenFocus (void);

int TPIa_widgetRelease (MSF_UINT32 handle);

int TPIa_widgetReleaseAll (MSF_UINT8 modId);

MSF_UINT32 TPIa_widgetCopy (MSF_UINT8 modId, MSF_UINT32 handle);

int TPIa_widgetRemove(MSF_UINT32 handle, MSF_UINT32 handleToBeRemoved);

int TPIa_widgetAddAction (MSF_UINT32 handle, MsfActionHandle action);

#define TPIa_widgetHandleMsfEvt(handle, msfEventType, override, unsubscribe)\
     TPIa_widgetRegisterMsfEvent(handle, msfEventType, override, unsubscribe, MSF_NULL)

int TPIa_widgetRegisterMsfEvent
(
    MSF_UINT32 handle, 
    int msfEventType, 
    int override, 
    int unsubscribe,
    widget_register_event_callback_t  cbRegEventNotify
);


int TPIa_widgetSetTitle (MSF_UINT32 handle, MsfStringHandle title);

int TPIa_widgetSetBorder (MSF_UINT32 handle, MsfLineStyle borderStyle);

int TPIa_widgetSetPosition(MSF_UINT32 handle, MsfPosition* position, MsfAlignment* alignment);

int TPIa_widgetSetSize (MSF_UINT32 handle, MsfSize* size);

int TPIa_widgetGetPosition (MSF_UINT32 handle, MsfPosition* position);

int TPIa_widgetGetSize (MSF_UINT32 handle, MsfSize* size);

int TPIa_widgetAddAnimation(MSF_UINT32 handle, MSF_UINT32 animation);

int TPIa_widgetSetDecoration(MSF_UINT32 handle, MsfDecoration decoration);

int TPIa_widgetMsfEvent2Utf8(MsfEvent *event, int multitap, char* buffer);

/****************************************************************
 SCREEN 
 ***************************************************************/
#define TPIa_widgetScreenCreate( modId, notify, defaultStyle) \
       TPIa_widgetScreenCreateEx( modId, notify, defaultStyle, MSF_NULL)

MsfScreenHandle TPIa_widgetScreenCreateEx 
(
    MSF_UINT8 modId, 
    int notify, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
);
        
int TPIa_widgetScreenSetNotify (MsfScreenHandle screen, int notify);
  
int TPIa_widgetScreenAddWindow (MsfScreenHandle screen, MsfWindowHandle window, MsfPosition* position, MsfScreenHandle nextScreen, MsfWindowHandle nextWindow);

MsfWindowHandle TPIa_widgetScreenActiveWindow (MsfScreenHandle screen);
  

/****************************************************************
 WINDOW 
 ***************************************************************/

int TPIa_widgetWindowAddGadget(MsfWindowHandle window, MsfGadgetHandle gadget, 
                               MsfPosition* position, MsfAlignment* alignment);
  
int TPIa_widgetWindowSetTicker (MsfWindowHandle window, MsfStringHandle tickerText);

#define MSF_WINDOW_PROPERTY_SCROLLBARVER  0x0001
#define MSF_WINDOW_PROPERTY_SCROLLBARHOR  0x0002
#define MSF_WINDOW_PROPERTY_TITLE         0x0004
#define MSF_WINDOW_PROPERTY_BORDER        0x0008
#define MSF_WINDOW_PROPERTY_SECURE        0x0010
#define MSF_WINDOW_PROPERTY_BUSY          0x0020
#define MSF_WINDOW_PROPERTY_TICKER_MODE   0x0040
#define MSF_WINDOW_PROPERTY_TICKER        0x0080
#define MSF_WINDOW_PROPERTY_NOTIFY        0x0100
#define MSF_WINDOW_PROPERTY_SINGLEACTION  0x0200
#define MSF_WINDOW_PROPERTY_ALWAYSONTOP   0x0400
#define MSF_WINDOW_PROPERTY_DISABLED      0x0800
#define MSF_WINDOW_PROPERTY_MOVERESIZE    0x1000

/*  Add by zhuxq,   not allow window to handle pen event  */
#define MSF_WINDOW_PROPERTY_DISABLE_PEN_EVENT    0x2000
/* Allow all gadgets in a window to focus */
#define MSF_WINDOW_PROPERTY_NOT_KEEP_GADGET_FOCUS    0x4000

  
int TPIa_widgetWindowSetProperties (MsfWindowHandle window, int propertyMask);

int TPIa_widgetWindowSpecificSize (MsfWindowHandle window, MsfSize* size, int minimalSize);


/****************************************************************
 PAINTBOX 
 ***************************************************************/
MsfWindowHandle TPIa_widgetPaintboxCreate (MSF_UINT8 modId, MsfSize* size, 
                                           int propertyMask, MsfStyleHandle defaultStyle);

int TPIa_widgetPaintboxRedrawArea (MsfWindowHandle paintbox, MsfSize* size, MsfPosition* pos);
  

/****************************************************************
 FORM 
 ***************************************************************/
#define TPIa_widgetFormCreate(modId, size, propertyMask, defaultStyle) \
       TPIa_widgetFormCreateEx(modId, size, propertyMask, defaultStyle, MSF_NULL)

MsfWindowHandle TPIa_widgetFormCreateEx 
(
    MSF_UINT8 modId, 
    MsfSize* size, 
    int propertyMask,
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
);

int TPIa_widgetFormResetViewPort( MsfWindowHandle hForm );

/****************************************************************
 TEXT 
 ***************************************************************/
int TPIa_widgetTextSetText (MSF_UINT32 text, MsfStringHandle initialString, MsfTextType type, int maxSize, MsfStringHandle inputString, int singleLine);
    
int TPIa_widgetTextMaxSize (MSF_UINT32 text);
  
int TPIa_widgetTextClear (MSF_UINT32 text);

int TPIa_widgetTextSetProperty (MSF_UINT32 handle, MsfColor* color, MsfFont* font, MsfTextProperty* textProperty);

int TPIa_widgetTextGetProperty(MSF_UINT32 handle, MsfColor* color, MsfFont* font, MsfTextProperty* textProperty, MsfPosition* baseline);

int TPIa_widgetTextCurrentSize(MSF_UINT32 text);

int TPIa_widgetTextGetData(MSF_UINT32 handle, int index, char *buffer, MsfStringFormat format);

/****************************************************************
 EDITOR 
 ***************************************************************/
#define TPIa_widgetEditorCreate( modId, initialString, inputString, type, maxSize, singleLine, size, propertyMask, defaultStyle) \
        TPIa_widgetEditorCreateEx( modId, initialString, inputString, type, maxSize, singleLine, size, propertyMask, defaultStyle, MSF_NULL)
        
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
);

MsfWindowHandle TPIa_widgetEditorCreateCss (MSF_UINT8 modId, MsfStringHandle initialString, 
                                           MsfStringHandle inputString, MsfTextType type, 
                                           const char* formatString, int inputRequired, 
                                           int maxSize, int singleLine, 
                                           MsfSize* size, int propertyMask, MsfStyleHandle defaultStyle);

/****************************************************************
 CHOICE 
 ***************************************************************/
int TPIa_widgetChoiceSetElement (MSF_UINT32 choice, int index, MsfStringHandle string1, MsfStringHandle string2, MsfImageHandle image1, MsfImageHandle image2, int insert);

#ifdef MSF_CONFIG_TOOLTIP
int TPIa_widgetChoiceSetElementTT (MSF_UINT32 choice, int index, 
                                   MsfStringHandle string1, MsfStringHandle string2, 
                                   MsfImageHandle image1, MsfImageHandle image2, 
                                   MsfStringHandle tooltip, 
                                   int insert);
#endif
int TPIa_widgetChoiceRemoveElement (MSF_UINT32 choice, int index);  
    
int TPIa_widgetChoiceGetElemState (MSF_UINT32 choice, int index);

int TPIa_widgetChoiceSetElemState (MSF_UINT32 choice, int index, int state);
  
#define MSF_CHOICE_ELEMENT_ICON     0x01 
#define MSF_CHOICE_ELEMENT_INDEX    0x02 
#define MSF_CHOICE_ELEMENT_STRING_1 0x04 
#define MSF_CHOICE_ELEMENT_STRING_2 0x08 
#define MSF_CHOICE_ELEMENT_IMAGE_1  0x10 
#define MSF_CHOICE_ELEMENT_IMAGE_2  0x20 

/*
 *  Add by Zhuxq 
 *  the Selectgroup showing styles
 */
/*  SG_SHOW_STYLE_MENU: choice with no border, and item hight is 20   */
/* draw the selectgroup in menu style */
#define  SG_SHOW_STYLE_MENU                          0x0001
/* draw the selectgroup without border */
#define  SG_SHOW_STYLE_NO_BORDER                 0x0002
/* draw items in sequence layout and there is no space between items */
#define  SG_SHOW_STYLE_SEQUENCE_LAYOUT      0x0004
/* draw focus item with full focus rect or top and bottom lines */
#define  SG_SHOW_STYLE_FOCUS_DRAW_LINE      0x0008
/* always showing the hilight element  */
#define SG_SHOW_STYLE_HILIGHT_ALWAYS          0x0010
/* not showing the scrollbar  */
#define SG_SHOW_STYLE_NO_SCROLLBAR             0x0020
    
/* draw the selectgroup in main menu style */
#define  SG_SHOW_STYLE_MAIN_MENU               0x0040
    
int TPIa_widgetChoiceAttrProperty (MSF_UINT32 choice, MsfElementPosition* elementPos, int bitmask);

int TPIa_widgetChoiceElemProperty(MSF_UINT32 choice, int index, MsfElementPosition* elementPos, int bitmask);

int TPIa_widgetChoiceSize (MSF_UINT32 choice);

int TPIa_widgetChoiceGetCurrentItemState
(
	MSF_UINT32 choice,
	int              *pCurItemIndex,
	int              *pState
);

/****************************************************************
 MENU 
 ***************************************************************/
#define TPIa_widgetMenuCreate( modId, type, size, implicitSelectAction, elementPos, bitmask, propertyMask, defaultStyle) \
      TPIa_widgetMenuCreateEx( modId, type, size, implicitSelectAction, elementPos, bitmask, propertyMask, defaultStyle, MSF_NULL)

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
);
      
#define TPIa_widgetMenuAppCreate(modId, implicitSelectAction, elementPos, bitmask, propertyMask, defaultStyle) \
       TPIa_widgetMenuAppCreateEx(modId, implicitSelectAction, elementPos, bitmask, propertyMask, defaultStyle, MSF_NULL)

MsfWindowHandle TPIa_widgetMenuAppCreateEx 
(
    MSF_UINT8 modId, 
    MsfActionHandle implicitSelectAction,
    MsfElementPosition* elementPos, 
    int bitmask,
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
);

/****************************************************************
* MAIN MENU
*****************************************************************/
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
);


/****************************************************************
 VIEWER 
 ***************************************************************/
MsfGadgetHandle TPIa_widgetViewerCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle title, 
    MsfStringHandle text, 
    MsfSize* size, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
);

int   TPIa_widgetViewerSetText(MsfWindowHandle  hViewer,  MsfStringHandle  hStr );

/****************************************************************
   WAITING WINDOW
 ***************************************************************/
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
);

int  TPIa_widgetWaitingWinPlayAnimation(MsfWindowHandle  hWaitingWin);

int  TPIa_widgetWaitingWinStopAnimation(MsfWindowHandle  hWaitingWin);

int  TPIa_widgetWaitingWinSetPrompt(MsfWindowHandle  hWaitingWin, MsfStringHandle  hPromptStr );

int  TPIa_widgetWaitingWinSetProgress(MsfWindowHandle  hWaitingWin, int iCurPos, int iTotal );


/****************************************************************
 DIALOG 
 ***************************************************************/

#define TPIa_widgetDialogCreate(modId, dialogText, type, timeoutTime, propertyMask, defaultStyle)\
       TPIa_widgetDialogCreateEx(modId, dialogText, type, timeoutTime, propertyMask, defaultStyle, MSF_NULL)

MsfWindowHandle TPIa_widgetDialogCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle dialogText, 
    MsfDialogType type, 
    MSF_UINT32 timeoutTime, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
);

int TPIa_widgetDialogSetAttr (MsfWindowHandle dialog, MsfStringHandle dialogText, MSF_UINT32 timeoutTime);
  
int TPIa_widgetDialogAddInput (MsfWindowHandle dialog, MsfStringHandle label, MsfStringHandle inputText, MsfTextType inputMode, int maxSize);

int TPIa_widgetDialogRemoveInput (MsfWindowHandle dialog, int index);

int TPIa_widgetDialogSetInputAttr (MsfWindowHandle dialog, int index, MsfStringHandle label, MsfStringHandle inputText, MsfTextType inputMode, int maxSize);

/****************************************************************
 ACTION 
 ***************************************************************/
#ifndef   WIDGET_DEBUG
#define TPIa_widgetActionCreate(modId, label, actionType, priority, propertyMask) \
             TPIa_widgetActionCreateEx(modId, label, actionType, priority, propertyMask, MSF_NULL)
             
MsfActionHandle TPIa_widgetActionCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle label, 
    int actionType, 
    int priority, 
    int propertyMask,
    widget_action_callback_t   cbActionNotify
);
#else
#define TPIa_widgetActionCreate(modId, label, actionType, priority, propertyMask) \
             WIDGET_DEBUG_ACTION_CREATE(modId, label, actionType, priority, propertyMask, MSF_NULL, __FILE__, __LINE__)

#define TPIa_widgetActionCreateEx(modId, label, actionType, priority, propertyMask, cbActionNotify) \
        WIDGET_DEBUG_ACTION_CREATE(modId, label, actionType, priority, propertyMask, cbActionNotify, __FILE__, __LINE__)

MsfActionHandle WIDGET_DEBUG_ACTION_CREATE 
(
    MSF_UINT8 modId, 
    MsfStringHandle label, 
    int actionType, 
    int priority, 
    int propertyMask,
    widget_action_callback_t   cbActionNotify,
    const char *filename,
    int lineno
);

#endif
             
int TPIa_widgetActionSetAttr (MsfActionHandle action, int actionType, int priority);

int TPIa_widgetActionGetAttr (MsfActionHandle action, int* actionType, int* priority);

#define MSF_ACTION_PROPERTY_ENABLED       0x0001
#define MSF_ACTION_PROPERTY_SINGLEACTION  0x0002

/* Block the action -- add by zhuxq */
#define MSF_ACTION_PROPERTY_BLOCK              0x1000

int TPIa_widgetActionSetProperties (MsfActionHandle action, int propertyMask);

/****************************************************************
 GADGET 
 ***************************************************************/
#define MSF_GADGET_PROPERTY_SCROLLBARVER  0x01
#define MSF_GADGET_PROPERTY_SCROLLBARHOR  0x02
#define MSF_GADGET_PROPERTY_LABEL         0x04
#define MSF_GADGET_PROPERTY_BORDER        0x08
#define MSF_GADGET_PROPERTY_TICKERMODE    0x10
#define MSF_GADGET_PROPERTY_FOCUS         0x20
#define MSF_GADGET_PROPERTY_NOTIFY        0x40
#define MSF_GADGET_PROPERTY_ALWAYSONTOP   0x80
#define MSF_GADGET_PROPERTY_MOVERESIZE    0x1000

/*  Add by Zhuxq, if pen hit the gadget, then select the gadget, and trigger the ok event  */
#define MSF_GADGET_PROPERTY_PEN_CLICK    0x2000
/*  draw a gadget without background color  */
#define MSF_GADGET_PROPERTY_SHOW_NO_BACKGROUND  0x4000
/* draw a focused gadget without focused bar   */
#define MSF_GADGET_PROPERTY_SHOW_NO_FOCUS_BAR    0x10000
 /* if the property is set, when the focus is at the top or bottom border of the choice,
   * and user continue scroll up or down, the choice need to notify the module or app 
   * with MsfNotifyRequestNewContent and parameter 
   */
#define MSF_GADGET_PROPERTY_REQUEST_CONTENT    0x00020000

/* whether scrollbar is showed */
#define MSF_GADGET_PROPERTY_ALWAYS_SHOW_SCROLL_BAR    0x00040000

/* differeniate widgetTextInput need imm imdiately or not */
#define MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM     0x00080000

 /* if the property is set, when the focus among the choice items is changed
   * notify the app or module
   */
#define MSF_GADGET_PROPERTY_NOTIFY_ITEM_FOCUS_CHANGE    0x00100000

 /*  the property  is only applied to Selectgroup.
   * if the property is set, the selectgroup will not handle the ok event
   */
#define MSF_GADGET_PROPERTY_SG_DO_NOT_HANDLE_OK          0x00200000

 /*  the property  is set, indicates that when pen down event comes, the gadget will get focus
   * and it need handle the event further.
   */
#define MSF_GADGET_PROPERTY_PEN_EVENT_RESPOND_DIRECT       0x00400000

 /*  the property  is set, indicates that Left and Right key will filp page.
   * Only used for Box
   */
#define MSF_GADGET_PROPERTY_FLIP_PAGE_BY_LR_KEY           0x00800000


int TPIa_widgetGadgetSetProperties (MsfGadgetHandle gadget, int propertyMask);



/****************************************************************
 STRING GADGET 
 ***************************************************************/
#define TPIa_widgetStringGadgetCreate( modId, text, size, singleLine, propertyMask, defaultStyle) \
  TPIa_widgetStringGadgetCreateEx( modId, text, size, singleLine, propertyMask, defaultStyle, MSF_NULL)

MsfGadgetHandle TPIa_widgetStringGadgetCreateEx 
(
    MSF_UINT8 modId, 
    MsfStringHandle text, 
    MsfSize* size, 
    int singleLine, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
);
  
int TPIa_widgetStringGadgetSet (MsfGadgetHandle stringGadget, MsfStringHandle text, int singleLine);
    

/****************************************************************
 TEXT INPUT GADGET 
 ***************************************************************/

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
);
   
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
);
        
MsfGadgetHandle TPIa_widgetTextInputCreateCss (MSF_UINT8 modId, MsfStringHandle initialString, 
                                              MsfStringHandle inputString, MsfTextType type, 
                                              const char* formatString, int inputRequired, 
                                              int maxSize, int singleLine, 
                                              MsfSize* size, int propertyMask, MsfStyleHandle defaultStyle);


int TPIa_widgetTextInputInsertString(MSF_UINT32 hText, MsfStringHandle hInsert, MSF_BOOL bFirstInsert);

/****************************************************************
 SELECT GROUP GADGET 
 ***************************************************************/
#define TPIa_widgetSelectgroupCreate( modId, type, size, elementPos, bitmask, propertyMask, defaultStyle) \
        TPIa_widgetSelectgroupCreateEx( modId, type, size, elementPos, bitmask, propertyMask, 0,  defaultStyle, MSF_NULL)
        
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
);


/****************************************************************
 IMAGE GADGET
 ***************************************************************/
#define TPIa_widgetImageGadgetCreate(modId, image, size, imageZoom, propertyMask, defaultStyle) \
       TPIa_widgetImageGadgetCreateEx(modId, image, size, imageZoom, propertyMask, defaultStyle, MSF_NULL)

MsfGadgetHandle TPIa_widgetImageGadgetCreateEx
(
    MSF_UINT8 modId, 
    MsfImageHandle image, 
    MsfSize* size, 
    MsfImageZoom imageZoom, 
    int propertyMask, 
    MsfStyleHandle defaultStyle,
    widget_state_change_callback_t   cbStateChange
);
       
int TPIa_widgetImageGadgetSet(MsfGadgetHandle imageGadget, MsfImageHandle image, 
                              MsfImageZoom imageZoom);
  
int TPIa_widgetImagePlay(MsfGadgetHandle gadgetHandle);
int TPIa_widgetImageStop(MsfGadgetHandle gadgetHandle);

/****************************************************************
 DATE TIME GADGET
 ***************************************************************/
#define TPIa_widgetDateTimeCreate(modId, mode, time, date, size, propertyMask, defaultStyle) \
       TPIa_widgetDateTimeCreateEx(modId, mode, time, date, size, propertyMask, defaultStyle, MSF_NULL)

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
);

       
int TPIa_widgetDateTimeSetValues (MsfGadgetHandle datetimeGadget, MsfDateTimeType mode, MsfTime* time, MsfDate* date);
  
int TPIa_widgetDateTimeGetValues (MsfGadgetHandle datetimeGadget, MsfTime* time, MsfDate* date);

/***************************************************************
 BITMAP ANIMATION GADGET
****************************************************************/
int TPIa_widgetBmpAnimationStop(MsfGadgetHandle gadgetHandle);
int TPIa_widgetBmpAnimationPlay(MsfGadgetHandle gadgetHandle, int bUsePeriodicTimer);
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
);
/****************************************************************
 BAR GADGET
 ***************************************************************/
#define TPIa_widgetBarCreate(modId, barType, maxValue, initialValue, size, propertyMask, defaultStyle) \
       TPIa_widgetBarCreateEx(modId, barType, maxValue, initialValue, size, propertyMask, defaultStyle, MSF_NULL) 

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
);
           
int TPIa_widgetBarSetValues (MsfGadgetHandle bar, int value, int maxValue);

int TPIa_widgetBarGetValues (MsfGadgetHandle bar, int* value, int* maxValue);


/****************************************************************
 BUTTON GADGET
 ***************************************************************/
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
);

/****************************************************************
 BOX GADGET
 ***************************************************************/
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
);

int TPIa_widgetBoxSetValues 
(
    MsfGadgetHandle hBox,
    int iCurPos, 
    int nItemCount
);

int TPIa_widgetBoxGetValues 
(
    MsfGadgetHandle hBox,
    int    *pCurPos, 
    int    *pFocusItemRelativeIndex,
    int    *pItemsPerPage,
    int    *pItemCount
);

/****************************************************************
 LOW LEVEL
 ***************************************************************/
int TPIa_widgetHoldDraw(MSF_UINT32 msfHandle);

int TPIa_widgetPerformDraw(MSF_UINT32 msfHandle);

int TPIa_widgetDrawLine (MSF_UINT32 msfHandle, 
                         MsfPosition* start, 
                         MsfPosition* end);

int TPIa_widgetDrawRect (MSF_UINT32 msfHandle, MsfPosition* position, 
                         MsfSize* size, int fill);

int TPIa_widgetDrawIcon (MSF_UINT32 msfHandle, MsfIconHandle icon, 
                         MsfPosition* position);

int TPIa_widgetDrawArc (MSF_UINT32 msfHandle, MsfPosition* position, 
                        MsfSize* size, int startAngle,
                        int angleExtent, int fill);

int TPIa_widgetDrawString(MSF_UINT32 msfHandle, MsfStringHandle string, 
                          MsfPosition* position, MsfSize* maxSize, 
                          int index, int nbrOfChars, int baseline, 
                          int useBrushStyle);

int TPIa_widgetDrawPolygon (MSF_UINT32 msfHandle, int nrOfCorners, 
                            MSF_INT16* corners, int fill);

int TPIa_widgetDrawImage(MSF_UINT32 msfHandle, MsfImageHandle image, 
                         MsfPosition* position, MsfSize* maxSize, 
                         MsfImageZoom imageZoom);

int TPIa_widgetDrawAccessKey(MSF_UINT32 msfHandle, 
                             MsfAccessKeyHandle accessKey, 
                             MsfPosition* position);


/****************************************************************
 STYLE
 ***************************************************************/
MsfStyleHandle TPIa_widgetStyleCreate(MSF_UINT8 modId, MsfColor* color, 
                                      MsfColor* backgroundColor, 
                                      MsfPattern* foreground, 
                                      MsfPattern* background, 
                                      MsfLineStyle* lineStyle, 
                                      MsfFont* font, 
                                      MsfTextProperty* textProperty);

MsfBrushHandle TPIa_widgetGetBrush(MSF_UINT32 handle);

int TPIa_widgetSetColor (MSF_UINT32 msfHandle, MsfColor* color,
                         int background);

int TPIa_widgetSetLineStyle (MSF_UINT32 msfHandle,
                             MsfLineStyle* style);

int TPIa_widgetSetTextProperty (MSF_UINT32 msfHandle, 
                                MsfTextProperty* textProperty);

int TPIa_widgetSetPattern (MSF_UINT32 msfHandle, MsfPattern* pattern,
                           int background);

int TPIa_widgetSetFont (MSF_UINT32 msfHandle, MsfFont* font);

int TPIa_widgetFontGetFamily (int nrOfFonts, char* fontNames, 
                              MsfGenericFont genericFont, 
                              MsfFontFamilyNumber * fontFamilyNumber);

int TPIa_widgetGetColor (MSF_UINT32 msfHandle, MsfColor* color, 
                         int background);

int TPIa_widgetGetFont (MSF_UINT32 msfHandle, MsfFont* font);

int TPIa_widgetFontGetValues(MsfFont* font, int* ascent, 
                             int* height, int* xHeight);

int TPIa_widgetGetLineStyle (MSF_UINT32 msfHandle, 
                             MsfLineStyle* msfLineStyle);

int TPIa_widgetGetTextProperty (MSF_UINT32 msfHandle, 
                                MsfTextProperty* textProperty);

int TPIa_widgetGetPattern(MSF_UINT32 msfHandle, int background, 
                          MsfPattern* pattern);

int TPIa_widgetSetMarquee(MSF_UINT32 handle, MsfMarqueeType marqueeType, 
                          int direction, int repeat, int scrollAmount, 
                          int scrollDelay);



/****************************************************************
 STRING
 ***************************************************************/
#ifndef WIDGET_DEBUG
MsfStringHandle TPIa_widgetStringCreate (MSF_UINT8 modId, 
                                         const char* stringData, 
                                         MsfStringFormat stringFormat, 
                                         int length, 
                                         MsfStyleHandle defaultStyle);
                                         
MsfStringHandle TPIa_widgetStringCreateText (MSF_UINT8 modId, 
                                             MSF_UINT32 handle, 
                                             int index);
#else
#define TPIa_widgetStringCreate(modId, stringData, stringFormat, length, defaultStyle) \
           WIDGET_DEBUG_STRING_CREATE(modId, stringData, stringFormat, length, defaultStyle, __FILE__, __LINE__)
           
MsfStringHandle WIDGET_DEBUG_STRING_CREATE (MSF_UINT8 modId, 
                                         const char* stringData, 
                                         MsfStringFormat stringFormat, 
                                         int length, 
                                         MsfStyleHandle defaultStyle,
                                         const char *filename, int lineno );
                                         
#define  TPIa_widgetStringCreateText(modId, handle, index)  \
         WIDGET_DEBUG_STRING_CREATE_TEXT(modId, handle, index, __FILE__, __LINE__)
         
MsfStringHandle WIDGET_DEBUG_STRING_CREATE_TEXT (MSF_UINT8 modId, 
                                             MSF_UINT32 handle, 
                                             int index,
                                             const char *filename, int lineno );
                                             
#endif                                         

MsfStringHandle TPIa_widgetStringGetPredefined (MSF_UINT32 resId);

int TPIa_widgetStringGetLength(MsfStringHandle string,
                               int lengthInBytes, 
                               MsfStringFormat format);
  
int TPIa_widgetStringGetData(MsfStringHandle string, 
                             char* buffer, 
                             MsfStringFormat format);

int TPIa_widgetStringCompare(MsfStringHandle string1, 
                             MsfStringHandle string2);

int TPIa_widgetStringGetVisible(MsfStringHandle string, 
                                MSF_UINT32 handle, MsfSize* size,
                                int startIndex, 
                                int includeInitialWhiteSpaces, 
                                int* nbrOfCharacters, 
                                int* nbrOfEndingWhiteSpaces, 
                                int* nbrOfInitialWhiteSpaces);

int TPIa_widgetStringGetSubSize(MsfStringHandle string, 
                                MSF_UINT32 handle, int index, 
                                int nbrOfChars, int subwidth,
                                MsfSize* size);

int TPIa_widgetStringGetRectSize
(
    MsfStringHandle string, 
    MSF_UINT32 handle, 
    MsfSize* size
);

/****************************************************************
 ICON
 ***************************************************************/
MsfIconHandle TPIa_widgetIconCreate (MSF_UINT8 modId, MsfIconType iconType, MsfStyleHandle defaultStyle);


/****************************************************************
 IMAGE
 ***************************************************************/
#ifndef WIDGET_DEBUG
MsfImageHandle TPIa_widgetImageCreate (MSF_UINT8 modId, const char* imageData,
                                       int imageDataSize, MsfImageFormat imageFormat,
                                       int moreData, MsfStyleHandle defaultStyle);

MsfImageHandle TPIa_widgetImageCreateEx
(
    MSF_UINT8       modId, 
    const char*     imageData,
    int             imageDataSize, 
    int             totalSize,
    MsfImageFormat  imageFormat,
    int             moreData, 
    MsfStyleHandle  defaultStyle
);

MsfImageHandle TPIa_widgetImageCreateResource(MSF_UINT8 modId, char* resource, 
                                              MsfResourceType resourceType, 
                                              MsfImageFormat imageFormat);
  
MsfImageHandle TPIa_widgetImageCreateEmpty (MSF_UINT8 modId, MsfSize* size,
                                            MsfStyleHandle defaultStyle);

#else

#define TPIa_widgetImageCreate( modId, imageData, imageDataSize, imageFormat, moreData, defaultStyle)\
        WIDGET_DEBUG_IMAGE_CREATE( modId, imageData, imageDataSize, imageFormat, moreData, defaultStyle, __FILE__, __LINE__)

MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE (MSF_UINT8 modId, const char* imageData,
                                       int imageDataSize, MsfImageFormat imageFormat,
                                       int moreData, MsfStyleHandle defaultStyle, const char *filename, int lineno  );

#define TPIa_widgetImageCreateEx(modId, imageData, imageDataSize, totalSize, imageFormat, moreData, defaultStyle) \
        WIDGET_DEBUG_IMAGE_CREATE_EX(modId, imageData, imageDataSize, totalSize, imageFormat, moreData, defaultStyle, __FILE__, __LINE__) \

MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE_EX
(
    MSF_UINT8       modId, 
    const char*     imageData,
    int             imageDataSize, 
    int             totalSize,
    MsfImageFormat  imageFormat,
    int             moreData, 
    MsfStyleHandle  defaultStyle,
    const char *filename, int lineno 
);

#define TPIa_widgetImageCreateResource(modId, resource, resourceType, imageFormat) \
        WIDGET_DEBUG_IMAGE_CREATE_RESOURCE(modId, resource, resourceType, imageFormat, __FILE__, __LINE__ )

MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE_RESOURCE(MSF_UINT8 modId, char* resource, 
                                              MsfResourceType resourceType, 
                                              MsfImageFormat imageFormat,
                                              const char *filename, int lineno );

#define TPIa_widgetImageCreateEmpty( modId, size, defaultStyle) \
        WIDGET_DEBUG_IMAGE_CREATE_EMPTY( modId, size, defaultStyle, __FILE__, __LINE__ )
        
MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE_EMPTY (MSF_UINT8 modId, MsfSize* size,
                                            MsfStyleHandle defaultStyle, const char *filename, int lineno );
#endif


int TPIa_widgetImageNextPart (MsfImageHandle image, char* imageData,
                              int imageDataSize, int moreData);
 
MsfImageHandle TPIa_widgetImageCreateIcon(MSF_UINT8 modId, MsfIconHandle icon,
                                          MsfStyleHandle defaultStyle);

MsfImageHandle TPIa_widgetImageCreateFrame(MSF_UINT8 modId, MsfSize* size, MsfPosition* pos,
                                           MsfStyleHandle defaultStyle);

MsfImageHandle TPIa_widgetImageCreateFormat(MSF_UINT8 modId, MsfCreateData* imageData, 
                                            const char* imageFormat, MsfResourceType resourceType, 
                                            MsfStyleHandle defaultStyle);

MsfImageHandle TPIa_widgetImageGetPredefined(MSF_UINT32 resId, const char* resString);


/****************************************************************
 SOUND
 ***************************************************************/
#ifndef WIDGET_DEBUG
MsfSoundHandle TPIa_widgetSoundCreate (MSF_UINT8 modId, char* soundData, 
                                       MsfSoundFormat soundFormat, 
                                       int size, int moreData);

MsfSoundHandle TPIa_widgetSoundCreateEx
(
    MSF_UINT8      modId, 
    char*          soundData,
    MsfSoundFormat soundFormat, 
    int            size, 
    int            totalSize,
    int            moreData
);

MsfSoundHandle TPIa_widgetSoundCreateResource(MSF_UINT8 modId, 
                                              char* resource, 
                                              MsfResourceType resourceType, 
                                              MsfSoundFormat soundFormat);

MsfSoundHandle TPIa_widgetSoundCreateFormat(MSF_UINT8 modId, MsfCreateData* soundData, 
                                            const char* soundFormat, MsfResourceType resourceType);
#else

#define TPIa_widgetSoundCreate(modId, soundData, soundFormat, size, moreData) \
        WIDGET_DEBUG_SOUND_CREATE(modId, soundData, soundFormat, size, moreData, __FILE__, __LINE__ )

MsfSoundHandle WIDGET_DEBUG_SOUND_CREATE (MSF_UINT8 modId, char* soundData, 
                                       MsfSoundFormat soundFormat, 
                                       int size, int moreData,
                                       const char *filename, int lineno );

#define TPIa_widgetSoundCreateEx(modId, soundData, soundFormat, size, totalSize, moreData) \
        WIDGET_DEBUG_SOUND_CREATE_EX(modId, soundData, soundFormat, size, totalSize, moreData, __FILE__, __LINE__ )

MsfSoundHandle WIDGET_DEBUG_SOUND_CREATE_EX
(
    MSF_UINT8      modId, 
    char*          soundData,
    MsfSoundFormat soundFormat, 
    int            size, 
    int            totalSize,
    int            moreData,
    const char *filename, int lineno 
);

#define TPIa_widgetSoundCreateResource(modId, resource, resourceType, soundFormat) \
        WIDGET_DEBUG_SOUND_CREATE_RESOURCE(modId, resource, resourceType, soundFormat, __FILE__, __LINE__ )

MsfSoundHandle WIDGET_DEBUG_SOUND_CREATE_RESOURCE(MSF_UINT8 modId, 
                                              char* resource, 
                                              MsfResourceType resourceType, 
                                              MsfSoundFormat soundFormat,
                                              const char *filename, int lineno );

//#define TPIa_widgetSoundCreateFormat( modId, soundData, soundFormat, resourceType ) \
//        WIDGET_DEBUG_SOUND_CREATE_FORMAT( modId, soundData, soundFormat, resourceType, __FILE__, __LINE__ )

MsfSoundHandle TPIa_widgetSoundCreateFormat(MSF_UINT8 modId, MsfCreateData* soundData, 
                                            const char* soundFormat, MsfResourceType resourceType);
#endif

int TPIa_widgetSoundNextPart (MsfSoundHandle sound, char* soundData, int size, int moreData);

int TPIa_widgetSoundPlay (MsfSoundHandle sound);

int TPIa_widgetSoundPlayPredefined (MsfPredefinedSound sound);

int TPIa_widgetSoundStop (MsfSoundHandle sound);

MsfSoundHandle TPIa_widgetSoundGetPredefined(MSF_UINT32 resId);


/****************************************************************
 ACCESS KEYS
 ***************************************************************/
MsfAccessKeyHandle TPIa_widgetSetAccessKey(MSF_UINT32 handle,
                                           char* accessKeyDefinition,
                                           int override, int index,
                                           int visualise);

int TPIa_widgetRemoveAccessKey(MSF_UINT32 handle, 
                               MsfAccessKeyHandle accessKey);

/****************************************************************
 ANIMATION
 ***************************************************************/
MsfMoveHandle TPIa_widgetMoveCreate (MSF_UINT8 modId, MsfPosition* origin,
                                     MsfPosition* destination, int duration, 
                                     int nrOfSteps, MsfMoveProperty property, int repeat);

MsfRotationHandle TPIa_widgetRotationCreate (MSF_UINT8 modId, int startAngle, 
                                             int endAngle, int duration, int nrOfSteps, 
                                             MsfMoveProperty property, int repeat);

MsfColorAnimHandle TPIa_widgetColorAnimCreate (MSF_UINT8 modId, MsfColor* startColor, 
                                              MsfColor* endColor, int duration, 
                                              int nrOfSteps, MsfMoveProperty property, int repeat);



/**********************************************************************
 * Object Actions
 **********************************************************************/
void TPIa_objectAction (const char *action_cmd, const char *mime_type, 
                        MsfResourceType data_type, const unsigned char *data, 
                        MSF_INT32 data_len, const char *src_path, 
                        const char *default_name);



/*================================================================================================
        Extended functions' definition, which are used to support the callback notification 
================================================================================================*/
int TPIa_widgetUpdateBegin( MSF_UINT32  handle );

int TPIa_widgetUpdateEnd( MSF_UINT32  handle );

MSF_UINT32 TPIa_widgetStartTimer( MSF_INT32 delay, MSF_BOOL bPeriodic, MSF_UINT32 hWidget, widget_timer_callback_t fnOnTimerCallback);

int TPIa_widgetStopTimer(  MSF_UINT32 hWidget, MSF_UINT32 iTimerId );

void TPIa_widgetShowWaitingFlag(  int bClearAll );

int TPIa_widgetTextInputDefaultStateChangeCb( MSF_UINT8 modId, MSF_UINT32 hTextInput, MsfNotificationType notType, void *pData );

#endif

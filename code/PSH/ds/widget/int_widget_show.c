/*==================================================================================================

    MODULE NAME : int_widget_show.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   Zhuxq          Cxxxxxx      Initial file creation.
    7/30/2003    Zhuxq          P001026      Add the MsfEidtor show function
    07/26/2003   lindawang      P001074      Fix draw image problems.
    04/08/2003   Zhuxq          P001052      Modify the method of getting the start position of a gadget
    08/05/2003   Steven Lai     P001140      Change the method of handling screen focus
    08/13/2003   Zhuxq          P001147      Change the selectgroup show and menu show
    8/11/2003    Chenjs         P001091      Update text input widget.
    08/14/2003   lindawang      P001254      Fix draw image problem for wap.
    8/18/2003    Zhuxq          P001263      Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    08/20/2003   lindawang      P001308      Change draw rectangle function.  
    08/20/2003   Zhuxq          P001279      modify the access of the size of imagegadget
    08/22/2003   Zhuxq          P001290      Modify the MsfAction 
    08/23/2003   Zhuxq          P001347      Fix some bugs of the Input Method, and delete the invalid references
    08/27/2003   Zhuxq          P001383      fixed some bugs
    08/29/2003   Zhuxq          P001396      Add scroll-contents feature to string gadget
    08/29/2003   Zhuxq          P001429      Change the widgetStringCalcPageEndPos
    09/03/2003   Zhuxq          P001479      Add the support of enter char to fix some bugs
    09/25/2003   Zhuxq          P001696      Re-implement the dialog
    09/26/2003   linda wang     P001451      related P001714.    
    10/17/2003   Zhuxq          C001603      Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq          P001842      Optimize the paintbox and input method solution and fix some bug
    11/06/2003   Zhuxq          P001981      Fix some bugs: in a menu with a scroll bar, the contents will not be scrolled to the end, etc
    11/11/2003   Zhuxq          P001883      Optimize the redraw strategy to reduce the redraw times
    12/03/2003   Zhuxq          P002139      Enhance the sensitivity of scrollbar in WAP&MMS
    12/17/2003   Andrew         P002210      Show IME window when change focus in WAP.    
    12/30/2003   linda wang     P002286      Modify the macro define for wap module menu title and item margine.
    12/31/2003   Zhuxq          P002296      Fix the bugs that the textinput shows on the top of scrollbar, and the paintbox update error
    01/05/2004   Zhuxq          P002351      Update widget contents in batches    
    01/15/2004   Zhuxq          P002352      All editing actions of MsfTextInput turns to MsfEditor
    02/02/2004   Chenxiao       p002404      add textinput scrollbar  in the dolphin project    
    02/16/2004   Zhuxq          P002469      Add button gadget to widget system
    02/16/2004   chenxiao       p002474      add bitmap animation function in the dolphin project    
    02/17/2004   Zhuxq          P002483      Adjust the display system of widget to show up the annunciator and fix some bugs
    02/17/2004   MengJiangHong  p002481      add scroll bar to stinggadget dynamically
    02/17/2004   Zhuxq          P002492      Add Viewer window to widget system
    02/19/2004   Zhuxq          P002504      Add box gadget to widget system
    02/20/2004   Zhuxq          P002517      Adjust the handle order of pen events in widget system
    02/24/2004   chenxiao       p002530      modify WAP & MSG main menu     
    02/28/2004   Zhuxq          P002556      Modify the default the title of MainMenu and Viewer
    03/03/2004   MengJianghong  P002568      Selectgroup draw function adjust.

    03/08/2004   Andrew         P002598      Update WAP Editor for Dolphin project
    03/19/2004   Zhuxq          P002687      Fix some bugs in widget system
    03/19/2004   Chenxiao       p002688      change main menu  of wap and fix some bugs from NEC feedback     
    03/27/2004   Zhuxq          P002760      Fix some bugs in widget system
    03/30/2004   Chenxiao       p002758      fix bugs for bookmark and color scheme in the wap module
    03/27/2004   Zhuxq          P002760      Fix some bugs in widget system
    04/03/2004   chenxiao       p002806      The display information is incorrect.        
    04/02/2004   zhuxq          P002789      Fix UMB bugs
    04/10/2004   chenxiao       p002847      fix some bugs in the scrollbar and setting of umb module    
    04/09/2004   zhuxq          P002858      Fix UMB bugs
    04/14/2004   Chenxiao       p002916      fix some bugs from NEC's response    
    04/15/2004   zhuxq          P002977      Fix some bugs in UMB for Dolphin
    04/14/2004   Dingjianxin    p002961      Delete the conditional compiler
    04/19/2004   chenxiao       p003028      fix umb setting bugs and widget bugs
    04/22/2004   Andrew         P002659      Fix some bugs of projects that bases Dolphin
    04/24/2004   zhuxq          P003022      Fix some bugs in UMB
    04/29/2004   lindawang      P005196      Fix bug for draw softkey.    
    04/30/2004   Andrew         P005156      Update symbol IME.
    05/13/2004   zhuxq          c005363      Add WAP&UMB to B1 project
    05/11/2004   zhuxq          P005514      Adjust layout for B1 and synchronize B1 and Dolpin
    05/20/2004   zhuxq          p005567      Fix bugs in WAP&UMB and synchronize B1 and Dolphin (TF_DLP_VER_01.20)
    06/04/2004   zhuxq          P005925      Correct issues found by PC-lint and fix some bugs
    06/08/2004   zhuxq          P005978      Synchronize B1 with Dolphin( TF_DLP_VER_01.23 )
    06/09/2004   Jianghb        P006036      Add WHALE1,WHALE2 and NEMO compilerswitch to some module
    
    06/09/2004   zhuxq          P006048      forbid Menu to respond successive multiple actions
    06/10/2004   chenxiao       p006076      The last word homepage content is lost  
    06/12/2004   majingtao      P006119      fix bugs about incomplete dialog information 
    06/12/2004   zhuxq          P006085      Add lotus macro for lotus   
    06/18/2004   zhuxq          P006260      Make up WAP&UMB code to cnxt_int_wap branch
    06/30/2004   majingtao      P006598      cursor ahead in editor is error; no receiptor can't save mms
    07/01/2004   Zhang Youkun   P006589      adjust menu font color and key-right function for knight
    
    06/30/2004   chenxiao       p006470      menu font color will be changed with different display. 
    07/01/2004   zhuxq          P006630      Fix bugs that the position of image can't be center
    07/02/2004   gaoyan         P006677      adjust wap layout of pegasus
    07/05/2004   zhuxq          P006708      Fix  bugs: CNXT00007777, CNXT00004950, CNXT00009472, CNXT00008770, CNXT00008391, CNXT00009745
    07/09/2004   Hover          P006867      change umb menu display and srollbar scale
    07/16/2004   Hover          P007022      change dragon_s umb interface and wap MEM setting                                                 (stop the switch:
    07/12/2004   Jianghb        C006917      Create Nemo baseline
    07/20/2004   zhuxq          P007108      Add playing GIF animation in some wap page with multi-frame GIF
    07/21/2004   Hover          P007134      change index picture  position setting
    07/22/2004   chenxiao       p007189      adjust drawing string position.  
    07/23/2004   zhuxq          P007215      Prevent freeze in wap
    07/27/2004   zhuxq          P007300      Add "no SIM" prompt when enter browser without SIM    Self-documenting Code
    07/26/2004   Zhang Youkun   P007251      Modify UMB prompt and color for knight
    07/29/2004   Hover          P006349      change whale1/2 draw image2 position and change short backgroud bitmap .
    08/03/2004   gaoyan         p007467      add blank before the title if show ime name on titel
    08/06/2004   zhuxq          P007577      adjust the layout of MMS/SMS for B1,  and fix some bugs
    08/07/2004   Hugh zhang     p007279      Add progress bar when sending or downloading mms.
    08/09/2004   zhuxq          P007608      Fix the bug that can't input  in some wap pages with textinput for b1
    08/13/2004   zhuxq          P007751      Fix the bug that TextInput in WAP page can't be inputed after pressed OK
    08/18/2004   Hover          P007830      change dragon_m umb and wap UI
    08/23/2004   Zhuxq          p007888      Fix the bug that the icon of 'abc' input method flashes when inputing in a wap page
    08/27/2004   chenxiao       p007990      The scrollbar is created and showed even if the page count is less than 1.
    08/30/2004   zhuxq          c007998      improve the implementation of predefined string to reduce the times of memory allocation
    09/01/2004   Hover          p008070      change dragon_m and whale UI definition
    09/02/2004   zhuxq          p008091      Disable to save .png in WAP
    09/03/2004   Hover          p007977      change dragon_s,violet,b1 UI. 
    09/03/2004   Hugh zhang     p001333      Modify menu display of widget in lotus project.
    09/08/2004   Hover          p008227      change dragon_m labels and change menu font color 
    09/13/2004   Hover          p008294      change CF810 index position and change violet menu 
    09/14/2004   Hover          p008309      change whale1 and nemo dra backgroud and change violet dialog size
    09/15/2004   Hover          p008350      change label and edit bookmark softkey and menu UI 
    09/13/2004   zhuxq          c008273      Add feature of inserting PB entry into SMS content for UMB
    09/17/2004   Hover          p008400      change odin/odin_a focus bar value and dragon_m index over 40 will display asterisk and when nemo 's select picture will display empty it display string position error
    09/22/2004   Hover          P008478      change the violet project image1 and string position
    
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

#include "Rm_typedef.h"
#include "ds_def.h"
#include "Ds_drawing.h"
#include "LIST_display.h"
#include "GSM_timer.h"

#include "msf_env.h"
#include "msf_rc.h"
#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "Int_widget_custom.h"
#include "int_widget_show.h"
#include "product.h"
#ifdef _TOUCH_PANEL
#include "TP_Operator.h"
#endif
#include "int_widget_imm.h"
#include "bra_rc.h"

/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/*
 * description of this macro, if needed. - Remember self documenting code
 */
// #define  DEBUG_SHOW
#ifdef   DEBUG_SHOW
#define SHOW    ds_refresh();
#else
#define SHOW    
#endif

#define  COLOR_SHADOW_LIGHT_GRAY     _RGB( 212, 208, 200)
#define  COLOR_SHADOW_GRAY                  _RGB( 128, 128, 128)
#define  COLOR_SHADOW_DARK_GRAY       _RGB( 64, 64, 64)

#define   MAX_SCREEN_X    (DS_SCRN_MAX_X -1)
#define   MAX_SCREEN_Y   (WAP_SCRN_TEXT_HIGH -1)


#define  CHOICE_ITEM_SCROLL_INTERVAL  500


#define TEXT_INPUT_SIZE  ((DS_SCRN_MAX_X / 6) * (LCD_SKEY_FRAME_START / GADGET_LINE_HEIGHT ) * 2 + 2)

#define IF_SHOW_TITLE( pWin )  (MSF_WINDOW_PROPERTY_TITLE  & ((pWin)->propertyMask ))

#define IF_SHOW_BORDER( pWin )  (MSF_WINDOW_PROPERTY_BORDER & ((pWin)->propertyMask ))

//#define IF_SHOW_HSCROLLBAR( pWin )  (MSF_WINDOW_PROPERTY_SCROLLBARHOR & ((pWin)->propertyMask ))

//#define IF_SHOW_VSCROLLBAR( pWin )  (MSF_WINDOW_PROPERTY_SCROLLBARVER & ((pWin)->propertyMask ))


#ifndef MENU_BG_RES_WITH_COLOR_SCHEME
#define MENU_BG_RES_WITH_COLOR_SCHEME    (RM_RESOURCE_ID_T)(BMP_SUB_SECONDEMENU_BG1+ ds_get_color_theme())
#endif
#ifndef SECONDMENU_LINE_RES_WITH_NUM_ICON
#define SECONDMENU_LINE_RES_WITH_NUM_ICON        BMP_SECONDMENULINE_NUM_ICON
#endif
#ifndef MENU_SHORT_BG_RES_WITH_COLOR_SCHEME
#define MENU_SHORT_BG_RES_WITH_COLOR_SCHEME    (RM_RESOURCE_ID_T)(BMP_SUB_SECONDEMENU_SHORT_BG1+ ds_get_color_theme())
#endif

#ifndef BLANK_NUMBER_BEFORE_TITLE
#define  BLANK_NUMBER_BEFORE_TITLE  1
#endif

/* the underline position in a text with underline */
#ifndef GADGET_UNDERLINE_POS
#define GADGET_UNDERLINE_POS   ( MSF_WIDGET_FONT_HEIGHT - 2 )
#endif
/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
typedef struct{
    OP_INT16        nCount;                    /* the number of sections */
    OP_INT16        iStringWidth;           /* the width and start position of item string area   */
    OP_INT16        iStringPos;          
    OP_INT16        iImage1Width;        /* the width and start position of front image area   */
    OP_INT16        iImage1Pos;
    OP_INT16        iImage2Width;        /* the width and start position of back image area   */
    OP_INT16        iImage2Pos;
}MsfChoiceItemPartition;

typedef struct{
    choiceElement    *pItem;                  /*  the item to be draw    */
    MsfPosition       pos;                        /* the item start position  */
    MsfSize             size;                       /* the item size                 */
    DS_COLOR        fontColor;              /* font color  */
    DS_COLOR        bgColor;                /* background color   */
    DS_FONTATTR   fontAttr;                /* font attribute  */
    OP_BOOLEAN    bChoiceFocus;      /* indicate if the selectgroup is in focus */
    OP_BOOLEAN    bFullFocusRect;    /* if the item is focus, draw focus area with full rect if true, or draw two line at bottom and top  */
    OP_INT16         iEngCharWidth;     /* the width of english char  */
    OP_INT16         index;                     /* the item index relative to the top item in the selectgroup  */
    MsfChoiceItemPartition  part;         /* the partition sections of item area */
}MsfWidgetDrawChoiceItem;


extern const MsfColor  DEFAULT_BG_COLORS;

extern void lcd_drv_draw_rectangle
(
    OP_UINT8 col1, 
    OP_UINT8 row1, 
    OP_UINT8 col2, 
    OP_UINT8 row2, 
    void * color, 
    OP_UINT8 width, 
    OP_UINT8 start_row, 
    OP_UINT8 start_col
);

extern RM_RESOURCE_ID_T widgetGetIconResID
(
    MsfIconType    MsfIconType
);

extern int  widgetInputFocusChange
( 
    OP_UINT32       hInput, 
    OP_BOOLEAN    bInfocus
);

extern RM_RESOURCE_ID_T widgetGetIconResID
(
    MsfIconType    MsfIconType
);

extern void ds_draw_partial_icon_rm 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_RESOURCE_ID_T   icon_res_id
);

extern void ds_draw_partial_icon 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_ICON_T         *icon
);

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static int  widgetScreenShow( void *pWidget, MsfWidgetType wt );
static int  widgetPaintBoxShow( void *pWidget, MsfWidgetType wt );
static int  widgetFormShow(void *pWidget, MsfWidgetType wt);
static int  widgetDialogShow(void *pWidget, MsfWidgetType wt);
static int  widgetMenuShow( void *pWidget, MsfWidgetType wt );
static int  widgetMainMenuShow( void *pWidget, MsfWidgetType wt );
static int  widgetViewerShow( void *pWidget, MsfWidgetType wt );
static int  widgetWaitingWinShow( void *pWidget, MsfWidgetType wt );
static int  widgetEditorShow(void *pWidget, MsfWidgetType wt);
static int  widgetSelectgroupShow(void *pWidget, MsfWidgetType wt);
static int  widgetDatetimeShow(void *pWidget, MsfWidgetType wt);
static int  widgetTextInputShow(void *pWidget, MsfWidgetType wt);
static int  widgetStringGadgetShow(void *pWidget, MsfWidgetType wt);
static int  widgetImageGadgetShow(void *pWidget, MsfWidgetType wt);
static int  widgetButtonShow(void *pWidget, MsfWidgetType wt);
static int  widgetBmpGadgetShow(void *pWidget, MsfWidgetType wt);
static int  widgetBarShow(void *pWidget, MsfWidgetType wt);

static int  widgetBox_scrollbarStateChangeCb( MSF_UINT8 modId, MSF_UINT32 hBar, MsfNotificationType noteType, void* pData);

int widgetPaintBoxRedraw( MsfWindow    *pPaintBox, OP_BOOLEAN bRefresh);

OP_INT16 widgetGetWindowTitleHeight( const MsfWindow  *pWin );

//OP_BOOLEAN widgetGetHilightColor( DS_COLOR *pBorderColor, DS_COLOR *pFillColor );
int widgetDrawHilight
(  
    MsfWindow *pWin, 
    MsfPosition  *pPos,  
    MsfSize *pSize, 
    OP_BOOLEAN bFullRect , 
    OP_BOOLEAN bOnlyBorder
);


int  widgetProgressBarShow( const MsfWindow *pWin, MsfBar *pBar );
int  widgetScrollBarShow( const MsfWindow *pWin, const MsfBar *pBar );

DS_SCRNMODE_ENUM_T  widgetScreenSetMode( void );

int widgetGadgetGetPosition(const MsfGadget* pGadget,  OP_INT16  *pX, OP_INT16  *pY);
static int  widgetWindowShow(MsfWindow* pWin, MsfWidgetType widgetType, OP_BOOLEAN bClearBg, OP_UINT8 *pTitle );
int widgetTextInputGetEditArea( const MsfTextInput *pInput, MsfPosition *pPos, MsfSize  *pSize );
int widgetStringGadgetBuildPageIndexList( MsfStringGadget *pStringGadget );

int widgetWindowDrawStringByChar
(
    MsfWindow    *pWin,
    OP_UINT8    *pStr,
    MsfPosition   *pPos,
    MsfSize        *pSize
);

int widgetDialogAdjustInputPos
(
    MsfDialog               *pDlg,
    const MsfPosition   *pStartPos
);

int  widgetSelectgroupAdjustScrollbar( MsfSelectgroup  *pSelectgroup);

void widgetBarAdjustValue( MsfBar *pBar );

static int widgetWindowRedraw
( 
    MsfWindow         *pWin, 
    MsfWidgetType   wt
);

static int widgetGadgetRedraw
( 
    MsfGadget           *pGadget, 
    MsfWidgetType    widgetType
);

static int widgetChoiceDrawItemString
(
    MsfSelectgroup                      *pSelectGroup,
    MsfWidgetDrawChoiceItem *pDrawChoiceItem
);

static int widgetChoiceDrawItemImages
(
    MsfSelectgroup                      *pSelectGroup,
    MsfWidgetDrawChoiceItem *pDrawChoiceItem
);

static int  widgetChoiceCalcItemArea
(
    MsfSelectgroup     *pSelectGroup
);

static int widgetChoicePartitionItemArea
(
    MsfSelectgroup                *pSelectGroup,
    MsfChoiceItemPartition *pPart
);
int  widgetTextInputAdjustScrollbar(MsfTextInput *pWidget);

static int WidgetGetGadgetPositionRange(MsfWindow * pWindow,
                                        OP_INT16  *pGadgetXMaxArea,
                                        OP_INT16  *pGadgetYMaxArea);

static int  widgetStringGadgetAdjustScrollbar
(
    MsfStringGadget *pWidget
);

static int widgetDialogOnTimer( MSF_UINT32 hDlg, MSF_UINT32 iTimerId);

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/


static widget_show_func_t  widgetShowFunc[MSF_MAX_WIDGETTYPE]=
{
    widgetScreenShow,                   /* Screen */
    widgetPaintBoxShow,               /* PaintBox*/   
    widgetFormShow,         /* Form */  
    widgetDialogShow,               /* Dialog */    
    widgetMenuShow,         /* Menu */
    widgetMainMenuShow,     /* Main menu */
    widgetViewerShow,       /* Viewer  */
    widgetWaitingWinShow,       /* WaitingWin  */
    widgetEditorShow,          /* Editor */    
    widgetSelectgroupShow,  /* Selectgroup */
    widgetDatetimeShow,     /* Datetime */
    widgetTextInputShow,        /* TextInput */
    widgetStringGadgetShow, /* StringGadget */
    widgetImageGadgetShow,  /* ImageGadget */
    widgetButtonShow,      /* Button */
    widgetBmpGadgetShow,      /* bitmapGadget*/
    widgetSelectgroupShow,  /* Box */
    widgetBarShow,          /* Bar  */
    OP_NULL,           /* Image */
    OP_NULL,           /* String */
    OP_NULL,           /* Icon */
    OP_NULL,           /* Sound */
    OP_NULL,           /* Style */
    OP_NULL,           /* ColorAnim */
    OP_NULL,           /* Move  */
    OP_NULL,           /* Rotation */
    OP_NULL,           /* Action */
    OP_NULL,           /* predefined string */
};



/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/

static MSF_BOOL   bAnnuUpdate = MSF_FALSE;  /* whether update annunciate area */
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
extern OP_BOOLEAN widgetGetComposedColor( DS_COLOREDCOMPOSER_T  *pCompose);
/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================
    FUNCTION:  widgetGetCharType

    DESCRIPTION:
        get the unicode char type
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         1  --  white space
         2  --  ASCII  char
         3  --  non-ASCII char

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetGetCharType( const OP_UINT8    *pChar )
{
    if( *(pChar+1) == 0x00 )
    {
        if( (*pChar > 0x40 && *pChar < 0x5B )  /*  A ~ Z  */
         ||(*pChar > 0x60 && *pChar < 0x7B )) /*  a ~ z  */
        {
            return 2;
        }
        else if( *pChar == 0x20 )
        {
            return 1;
        }
    }

    return 3;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static void  widgetGetShodowColors
(
    DS_COLOR *pWhiteColor,
    DS_COLOR *pLightColor,
    DS_COLOR *pMidColor,
    DS_COLOR *pDarkColor,
    DS_COLOR  baseColor
)
{
    MsfWidgetColor rgb, hsl;
    
    rgb.colorRGB.r = (baseColor & 0xF800) >> 8;
    rgb.colorRGB.g = (baseColor & 0x07E0) >> 3;
    rgb.colorRGB.b = (baseColor & 0x001F) << 3;

    hsl.color = widgetRGBToHSL( rgb.color );

    if( pWhiteColor )
    {
        hsl.colorHSL.l = 242;
        rgb.color = widgetHSLToRGB( hsl.color );
        *pWhiteColor = (DS_COLOR)_RGB(rgb.colorRGB.r, rgb.colorRGB.g, rgb.colorRGB.b);
    }

    if( pLightColor )
    {
        hsl.colorHSL.l = 210;
        rgb.color = widgetHSLToRGB( hsl.color );
        *pLightColor = (DS_COLOR)_RGB(rgb.colorRGB.r, rgb.colorRGB.g, rgb.colorRGB.b);
    }
    
    if( pMidColor )
    {
        hsl.colorHSL.l = 128;
        rgb.color = widgetHSLToRGB( hsl.color );
        *pMidColor = (DS_COLOR)_RGB(rgb.colorRGB.r, rgb.colorRGB.g, rgb.colorRGB.b);
    }
    
    if( pDarkColor )
    {
        *pDarkColor = (DS_COLOR)0;
    }
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetPointClientToScreen
( 
    const MsfWindow   *pWin, 
    const OP_INT16  client_x,  
    const OP_INT16  client_y,  
    OP_INT16           *pScreen_x,
    OP_INT16           *pScreen_y
)
{
    if( !pWin || !pScreen_x || !pScreen_y )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    *pScreen_x = pWin->position.x + pWin->clientOrigin.x + client_x - pWin->scrollPos.x;
    *pScreen_y = pWin->position.y + pWin->clientOrigin.y + client_y - pWin->scrollPos.y;

    /*   Note: If the window need to show Ticker, 
      *             then must consider the Ticker height.
      */ 
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetPointScreenToClient
( 
    const MsfWindow   *pWin, 
    const OP_INT16  screen_x,  
    const OP_INT16  screen_y,  
    OP_INT16           *pClient_x,
    OP_INT16           *pClient_y
)
{
    if( !pWin || !pClient_x || !pClient_y )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    *pClient_x = screen_x -(pWin->position.x + pWin->clientOrigin.x) + pWin->scrollPos.x;
    *pClient_y = screen_y -(pWin->position.y + pWin->clientOrigin.y) + pWin->scrollPos.y;
    
    /*   Note: If the window need to show Ticker, 
      *             then must consider the Ticker height.
      */ 

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetRectClientToScreen
( 
    const MsfWindow    * pWin, 
    const MsfPosition  *pClientStart,
    const MsfSize        *pClientSize,  
    MsfPosition           *pScreenStart,
    MsfSize                 *pScreenSize
)
{
    if( !pWin || !pScreenStart || !pScreenSize||!pClientStart ||!pClientSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pScreenStart->x = pWin->position.x + pWin->clientOrigin.x + pClientStart->x - pWin->scrollPos.x;
    pScreenStart->y = pWin->position.y + pWin->clientOrigin.y + pClientStart->y - pWin->scrollPos.y;
    
    pScreenSize->width = pClientSize->width;
    pScreenSize->height = pClientSize->height;
    
    /*   Note: If the window need to show Ticker, 
      *             then must consider the Ticker height.
      */ 
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetRectScreenToClient
( 
    const MsfWindow    * pWin, 
    const MsfPosition  *pScreenStart,
    const MsfSize        *pScreenSize,  
    MsfPosition            *pClientStart,
    MsfSize                  *pClientSize
)
{
    if( !pWin || !pScreenStart || !pScreenSize||!pClientStart ||!pClientSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pClientStart->x = pScreenStart->x -(pWin->position.x + pWin->clientOrigin.x) + pWin->scrollPos.x;
    pClientStart->y = pScreenStart->y -(pWin->position.y + pWin->clientOrigin.y) + pWin->scrollPos.y;

    pClientSize->width = pScreenSize->width;
    pClientSize->height = pScreenSize->height;
    
    /*   Note: If the window need to show Ticker, 
      *             then must consider the Ticker height.
      */ 

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         The  returned coordinary is client coordinary( logic coordinary ), not the screen coordinary

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetGetClientRect(const MsfWindow* pWin, MsfPosition  *pStart, MsfSize *pSize)
{
    MsfWidgetType   wt;
    
    if( !pWin ||!pStart ||!pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pStart->x = pWin->scrollPos.x;
    pStart->y = pWin->scrollPos.y;
        
    wt = getWidgetType(pWin->windowHandle );
    if( wt == MSF_DIALOG )
    {
        pSize->width = pWin->size.width - DIALOG_LEFT_BORDER - DIALOG_RIGHT_BORDER;
        pSize->height = pWin->size.height - DIALOG_TOP_BORDER - DIALOG_BOTTOM_BORDER;
    }
    else
    {
        pSize->width = pWin->size.width ;
        pSize->height = pWin->size.height -pWin->clientOrigin.y ;
    
#if 0
        if( IF_SHOW_HSCROLLBAR( pWin ) )
        {
            pSize->height -= SCROLLBAR_WIDTH;
        }

        if( IF_SHOW_VSCROLLBAR( pWin ) )
        {
            pSize->width -= SCROLLBAR_WIDTH;
        }
#endif
        
        if(IF_SHOW_BORDER(pWin))
        {
            pSize->width -= 4;
            pSize->height -= 2;
        }
    }

    /*   Note: If the window need to show Ticker, 
      *             then must consider the Ticker height.
      */ 

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetStringGetVisibleInSize

    DESCRIPTION:
        It calculates the number of characters of the whole words, 
        counted from the index parameter of the MsfString, that 
        would be visible in a bounding rectangle of the specified 
        MsfSize when it is drawn according to its current style settings.

    ARGUMENTS PASSED:
        ustr         :Specifies the unicode string.
        size         :Specifies the MsfSize of the rectangle.
        fontAttr   :Specifies the font attributes.
        bIncludeInitialWhiteSpaces : Specifies if the calculation will include any whitespace characters 
                in the beginning of the partial MsfString (the partial MsfString     means that the 
                calculation always starts from the index given in the startIndex parameter). If this 
                parameter is OP_TRUE the function will include any whitespaces in the beginning of the
                partial MsfString when the nbrOfCharacters parameter is calculated and the 
                nbrOfInitialWhiteSpaces parameter will be ignored, that is, the integration will not 
                calculate the number of whitespaces in the beginning of the MsfString, since they 
                are included in the nbrOfCharacters parameter. If this parameter is OP_FALSE the function 
                will disregard any whitespaces in the beginning of the partial MsfString when the 
                nbrOfCharacters parameter is calculated and the nbrOfInitialWhiteSpaces parameter
                will return the number of whitespaces that were disregarded in the beginning of 
                the MsfString.
        bWordMode        :  
        nbrOfCharacters : Returns the number of characters of the whole words, counted from the 
                specified index of the MsfString, that would be visible in a bounding rectangle of the 
                specified size when it is drawn according to the current style settings of the 
                specified object.
        nbrOfEndingWhiteSpaces :Returns the number of whitespace characters in the end of the 
                partial string specified by the nbrOfCharacters parameter.
        nbrOfInitialWhiteSpaces  :Returns the number of whitespace characters in the beginning of
                the partial string specified by the nbrOfCharacters parameter. This parameter is
                ignored if the includeInitialWhiteSpaces parameter is set to 1.            
            
    RETURN VALUE:
        The width of characters which can be visible  if the operation was successful, 
        otherwise the appropriate error code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/

int  widgetStringGetVisibleByWord
(
    const OP_UINT8 *ustr,
    MsfSize               *size,
    int                       iEngCharWidth,
    OP_BOOLEAN     bIncludeInitialWhiteSpaces, 
    int                      *nbrOfCharacters, 
    int                      *nbrOfEndingWhiteSpaces,
    int                      *nbrOfInitialWhiteSpaces
)
{
//    int                 iEngCharWidth;              /*  width of a ASCII char */    /* NOTE: The width of a CHINESE char is double of a ASCII char*/
    OP_UINT8    *pNextChar;
    OP_UINT8    *pLastWord;                   /* point to the last word which can be shown */
    int                 nWordVisible;                /* number of words which can be shown   */
    int                 nWsBeforeNextWord;   /* number of white spaces before the word handled currently   */
    int                 iWidthLeft;                     /* remainder  width */
    int                 iCharWidth;                    /* width of next char    */
    int                 iCurWordType;               /* the type of word in which the current char is, values:  
                                                                   *       1 -- not a word   
                                                                   *       2 -- ASCII word
                                                                   *       3 -- non ASCII word
                                                                   */
    int                 iCharType;

    if(  ustr == OP_NULL 
    || size == OP_NULL 
    || nbrOfCharacters == OP_NULL 
    || nbrOfEndingWhiteSpaces == OP_NULL
    || (bIncludeInitialWhiteSpaces == OP_FALSE && nbrOfInitialWhiteSpaces == OP_NULL ))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( nbrOfInitialWhiteSpaces != OP_NULL)
    {
        *nbrOfInitialWhiteSpaces = 0;
    }
    
    if( size->height < 2 * iEngCharWidth )      /* height is too small */
    {
        *nbrOfCharacters = 0;
        *nbrOfEndingWhiteSpaces = 0;
        if( bIncludeInitialWhiteSpaces == OP_FALSE )
        {
            *nbrOfInitialWhiteSpaces = 0;
        }
        
        return 0;
    }
    
    iWidthLeft = size->width;
    nWordVisible = 0;
    nWsBeforeNextWord = 0;
    iCurWordType = 1;                /*  not a word  */

    if( bIncludeInitialWhiteSpaces == OP_FALSE )    
    {
        /*  skip the whitespaces  */
        while( *ustr == 0x20 && *(ustr+1) == 0x00 )
        {
            nWsBeforeNextWord++;
            ustr += 2;
        }
        
        *nbrOfInitialWhiteSpaces = nWsBeforeNextWord;
    }
    
    pLastWord = pNextChar = (OP_UINT8*)ustr;
    while(*pNextChar != 0x00 ||*(pNextChar+1) != 0x00 ) 
    {
        if( *pNextChar <= 0x7F && *(pNextChar+1) == 0x00 )       /*  ASCII char  */
        {
           iCharWidth = iEngCharWidth;
        }
        else
        {
           iCharWidth = iEngCharWidth * 2;
        }

        iCharType = widgetGetCharType( pNextChar ); 
        
        if( iWidthLeft < iCharWidth )   /*  no  room for the next  char  */
        {
            if(  iCurWordType != 2 )     /*  The last whole word can be shown, i.e.  the current char is not in a ASCII word  */
            {
                *nbrOfCharacters = (pNextChar -ustr ) / 2;
                *nbrOfEndingWhiteSpaces = nWsBeforeNextWord;
            }

            /*  The current word is ASCII word   */
            else if(  iCharType == 2  )           /* not  at  the edget of the current  word  */
            {
                if( nWordVisible == 0 )    /* The current word is the first word  */
                {
                    if( pNextChar == ustr )
                    {
                        *nbrOfCharacters = 0;
                    }
                    else 
                    {
                        *nbrOfCharacters = (pNextChar -ustr ) / 2 - 1;
                    }
                    
                    *nbrOfEndingWhiteSpaces = 0;                        
                }
                else        /* The current word is not the first word  */
                {
                    *nbrOfCharacters = ( pLastWord -ustr ) / 2;
                    *nbrOfEndingWhiteSpaces = nWsBeforeNextWord;
                }
            }
            else            /* at  the edget of the current  word  */
            {
                *nbrOfCharacters = (pNextChar -ustr ) / 2;
                *nbrOfEndingWhiteSpaces = 0;
            }
        
            break;
        }
        else        /*  enough room for the next char  */
        {
            if( iCharType == 3 )        /* next char is  not a ASCII char  or not a white space  */
            {
                iCurWordType = 3;          /* 3 -- non ASCII word */
                nWordVisible ++;
                nWsBeforeNextWord = 0;
                pLastWord = pNextChar;
            }
            else if( iCharType == 1 )  /* next char is   a white space  */
            {
                if( iCurWordType == 1 ) /* current char is not in a word   */
                {
                   nWsBeforeNextWord++;
                }
                else                        /* current char is  in a word   */
                {
                    iCurWordType = 1;
                    nWsBeforeNextWord = 1;
                    nWordVisible ++;
                }
            }
            else            /* next char is  a ASCII char, A~Z or a~z */
            {
                if( iCurWordType == 1 )  /* current char is not  in a word   */
                {
                    iCurWordType = 2;
                    pLastWord = pNextChar;
                }
                else if( iCurWordType == 3 )  /* current char is  in a non-ASCII word   */
                {
                    iCurWordType = 2;
                    pLastWord = pNextChar;
                    nWordVisible ++;
                    nWsBeforeNextWord = 0;
                }
            }
            
            iWidthLeft -= iCharWidth;
            pNextChar += 2;
        }        
    }

    if( *pNextChar == 0x00 && *(pNextChar+1) == 0x00 )      /*  at the end of the unicode string  */
    {
        if( iCurWordType != 1 )  /* current char is  in a word   */
        {
            nWsBeforeNextWord = 0;
        } 
        
        if( pNextChar == ustr )                     /* the ustr is a empty string */
        {
            *nbrOfCharacters = 0;
            *nbrOfEndingWhiteSpaces = 0;            
        }
        else
        {
            *nbrOfCharacters = (pNextChar -ustr ) / 2;
            *nbrOfEndingWhiteSpaces = nWsBeforeNextWord;
        }
    }

    return (int)( size->width -iWidthLeft );

}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetStringGetVisibleByChar
(
    const OP_UINT8 *ustr,
    MsfSize               *size,
    OP_INT16            iEngCharWidth,
    OP_BOOLEAN      bIncludeInitialWhiteSpaces, 
    int                       *nbrOfCharacters, 
    int                       *nbrOfInitialWhiteSpaces
)
{
//    int                 iEngCharWidth;              /*  width of a ASCII char */    /* NOTE: The width of a CHINESE char is double of a ASCII char*/
    OP_UINT8       *pNextChar;
    int                 nEngCharCount;              /* available chars room by english char  */
    int                 iCharWidthAsEngChar;   /* width of next char    */

    if(  ustr == OP_NULL 
    || size == OP_NULL 
    || nbrOfCharacters == OP_NULL 
    || (bIncludeInitialWhiteSpaces == OP_FALSE && nbrOfInitialWhiteSpaces == OP_NULL ))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( nbrOfInitialWhiteSpaces != OP_NULL )
    {
        *nbrOfInitialWhiteSpaces = 0;
    }
    
    if( size->height < 2 * iEngCharWidth )      /* height is too small */
    {
        *nbrOfCharacters = 0;
        
        return 0;
    }
    
    nEngCharCount = size->width / iEngCharWidth ;

    if( bIncludeInitialWhiteSpaces == OP_FALSE )    
    {
        /*  skip the whitespaces  */
        while( *ustr == 0x20 && *(ustr+1) == 0x00 )
        {
            (*nbrOfInitialWhiteSpaces)++;
            ustr += 2;
        }
    }

    pNextChar = (OP_UINT8*)ustr;
    
    /* handle the head enter chars */
    if( *pNextChar == 0x0A && *(pNextChar+1) == 0x00 ) /* enter char */
    {
        *nbrOfCharacters = 1;
        pNextChar += 2;

        return 0; /* length */
    }

    /* ending while loop condition: reaching the end of string or encountering the enter char  */
    while((*pNextChar != 0x00 && *pNextChar != 0x0A) ||*(pNextChar+1) != 0x00 )  
    {
        if( *pNextChar <= 0x7F && *(pNextChar+1) == 0x00 )       /*  ASCII char  */
        {
           iCharWidthAsEngChar = 1;
        }
        else
        {
           iCharWidthAsEngChar =  2;
        }

        if( nEngCharCount >=  iCharWidthAsEngChar )  /*  enough room for the next char  */
        {
            nEngCharCount -= iCharWidthAsEngChar;
            pNextChar += 2;
        }
        else          /*  no  room for the next  char  */
        {
            if( pNextChar == ustr )
            {
                *nbrOfCharacters = 0;
            }
            else 
            {
                *nbrOfCharacters = (pNextChar -ustr ) / 2 ;
            }

            break;
        }
    }

    if(*(pNextChar+1) == 0x00)
    {
        if( *pNextChar == 0x00 )      /*  at the end of the unicode string  */
        {
            if( pNextChar == ustr )                     /* the ustr is a empty string */
            {
                *nbrOfCharacters = 0;
            }
            else
            {
                *nbrOfCharacters = (pNextChar -ustr ) / 2;
            }
        }
        else if( *pNextChar == 0x0A ) /* enter char */
        {
            *nbrOfCharacters = (pNextChar -ustr ) / 2 + 1;
        }
    }
    return (int)( size->width -nEngCharCount * iEngCharWidth );

}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetStringGetSubSize
(
    const OP_UINT8 *ustr,
    OP_INT16            iEngCharWidth,
    int                        nbrOfChars, 
    int                        subwidth, 
    MsfSize                *pSize
)
{
//    int                 iEngCharWidth;              /*  width of a ASCII char */    /* NOTE: The width of a CHINESE char is double of a ASCII char*/
    OP_UINT8    *pChar;
    int                 iCharLenAsEngChar;                    /* width of next char    */
    int                 iCurWordType;               /* the type of word in which the current char is, values:  
                                                                   *       1 -- not a word   
                                                                   *       2 -- ASCII word
                                                                   *       3 -- non ASCII word
                                                                   */
    int                 iWordMaxLen;
    int                 iCurWordLen;
    int                 iSubStrLen;
    int                 index;
    int                 iCharType;

    if(  ustr == OP_NULL || pSize == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    iSubStrLen = 0;
    iWordMaxLen = 0;
    iCurWordLen = 0;
    iCurWordType = 1;                /*  not a word  */
    index = 0;
    pChar = (OP_UINT8*)ustr;
    while((*pChar != 0x00 ||*(pChar+1) != 0x00 ) )
    {
        if( nbrOfChars != 0 && index >= nbrOfChars )
        {
            break;
        }

        if( *pChar <= 0x7F && *(pChar+1) == 0x00 )       /*  ASCII char  */
        {
           iCharLenAsEngChar = 1;
        }
        else
        {
           iCharLenAsEngChar = 2;
        }

        iCharType = widgetGetCharType( pChar ); 
        
        if( iCharType == 3 )        /* next char is  not a ASCII char  or not a white space  */
        {
            iCurWordType = 3;          /* 3 -- non ASCII word */
            
            if( iCurWordLen > iWordMaxLen )
            {
                iWordMaxLen = iCurWordLen;
            }
            
            iCurWordLen = iCharLenAsEngChar;
        }
        else if( iCharType == 1 )  /* next char is  a white space  */
        {
            iCurWordType = 1;
            if( iCurWordLen > iWordMaxLen )
            {
                iWordMaxLen = iCurWordLen;
            }
            
            iCurWordLen = 0;
        }
        else            /* next char is  a ASCII char, A~Z or a~z */
        {
            if( iCurWordType == 1 )  /* current char is not  in a word   */
            {
                iCurWordType = 2;
                iCurWordLen = iCharLenAsEngChar;
            }
            else if( iCurWordType == 3 )  /* current char is  in a non-ASCII word   */
            {
                iCurWordType = 2;
                if( iCurWordLen > iWordMaxLen )
                {
                    iWordMaxLen = iCurWordLen;
                }
                
                iCurWordLen = iCharLenAsEngChar;
            }
            else
            {
                iCurWordLen += iCharLenAsEngChar;
            }
        }

        iSubStrLen += iCharLenAsEngChar;
        pChar += 2;
        index ++;
    }

    if( iCurWordLen > iWordMaxLen )
    {
        iWordMaxLen = iCurWordLen;
    }

    pSize->height = GADGET_LINE_HEIGHT;
    if( subwidth == 1 )
    {
        pSize->width = iEngCharWidth * iWordMaxLen;
    }
    else
    {
        pSize->width = iEngCharWidth * iSubStrLen;
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetDrawStringByChar
(
    MsfWindow *pWin, 
    MsfWidgetDrawString     *pDrawString
)
{
    OP_UINT8        strTemp[202];
    const OP_UINT8  *pBuf;
    OP_UINT8        *pStr;
    MsfSize         size;      
    OP_INT16        iYPos;
    OP_INT16        iXPos;
    int             nbrOfCharacters; 
    int             nbrOfInitialWhiteSpaces;
    int             nShownChars;        /*  the chars already shown  */
    int             iLen;
    OP_INT16        iEngCharWidth, iEngCharHeight;
    OP_BOOLEAN      bHasUnderline = OP_FALSE;
    int             iRet;
    int             iLineLen;

    if( !pWin ||  !pDrawString )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pBuf = pDrawString->pBuf;

    if( !pBuf ||(*pBuf == 0x00 && *(pBuf+1) == 0x00 ))
    {                                                 
        return TPI_WIDGET_OK;
    }
    
    iXPos = (OP_INT16)pDrawString->pos.x;
    iYPos = (OP_INT16)pDrawString->pos.y;
    size = pDrawString->size;
    
    nbrOfCharacters = 1;
    nShownChars = 0;
    
    if( 0 > widgetGetFontSizeInEngChar(pDrawString->fontAttr , &iEngCharWidth, &iEngCharHeight))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    if( pDrawString->fontAttr & FONT_UNDERLINE )
    {
        bHasUnderline = OP_TRUE;
        pDrawString->fontAttr &= ~FONT_UNDERLINE;
    }
    
    /*adjust to the ceter of a line */
    iYPos = (OP_INT16)(iYPos +(GADGET_LINE_HEIGHT /2 -iEngCharWidth) );
    
    iLen = size.width / iEngCharWidth *2 +2 ;
    if( iLen > 202 )
    {
        pStr = WIDGET_ALLOC(iLen + 4);
        if( pStr == OP_NULL )
        {
            return TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        }
    }
    else
    {
        pStr = strTemp;
    }
    
    while( nbrOfCharacters != 0 && size.height >= iEngCharHeight  )
    {
        iLineLen = widgetStringGetVisibleByChar(
                                            pBuf , 
                                            &size, 
                                            iEngCharWidth, 
                                            pDrawString->bShowInitialWhiteSpaces, 
                                            &nbrOfCharacters, 
                                            &nbrOfInitialWhiteSpaces);
        if( nbrOfCharacters != 0 )
        {
            nShownChars += nbrOfCharacters + nbrOfInitialWhiteSpaces;

            pBuf += nbrOfInitialWhiteSpaces * 2;
            iLen = nbrOfCharacters * 2;
            
            op_memcpy( pStr,  pBuf, iLen );
            pBuf += iLen;            
            pStr[iLen]  = pStr[iLen +1]  = 0x00;
            
            iRet = widgetScreenDrawTextLine( 
                                    pWin,
                                    (OP_INT16)( iXPos ),
                                    iYPos, 
                                    size.width,
                                    pStr, 
                                    pDrawString->fontAttr, 
                                    pDrawString->fontcolor, 
                                    pDrawString->backcolor);
                                    
            if( bHasUnderline && iRet == TPI_WIDGET_PAINTED )
            {
                widgetScreenDrawLine(
                                    pWin,
                                    (OP_INT16)(iXPos),
                                    (OP_INT16)(iYPos + GADGET_UNDERLINE_POS),
                                    (OP_INT16)(iXPos + iLineLen - 1),
                                    (OP_INT16)(iYPos + GADGET_UNDERLINE_POS),
                                    pDrawString->fontcolor);
            }

            size.height -= GADGET_LINE_HEIGHT;
            iYPos += GADGET_LINE_HEIGHT;

        }
    }

    if( pStr != strTemp )
    {
        WIDGET_FREE(pStr);
    }
    
    return nShownChars;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetDrawReliefRect
(
    const MsfWindow       *pWin,  
    MsfPosition     *pPos, 
    MsfSize           *pSize, 
    OP_BOOLEAN  bHollow,
    OP_BOOLEAN  bThin
)
{
    OP_INT16       left;
    OP_INT16       top;
    OP_INT16       right;
    OP_INT16       bottom;
    DS_COLOR     whiteColor, lightColor, midColor, darkColor;

    if( !pPos || !pSize ||!pWin)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    left = (OP_INT16)pPos->x;
    top = (OP_INT16)pPos->y;
    right = (OP_INT16)pPos->x + pSize->width - 1;
    bottom = (OP_INT16)pPos->y + pSize->height - 1;

#if 0
    widgetGetShodowColors( 
                            &whiteColor, 
                            &lightColor, 
                            &midColor, 
                            &darkColor, 
                            (DS_COLOR)_RGB(DEFAULT_BG_COLORS.r, DEFAULT_BG_COLORS.g, DEFAULT_BG_COLORS.b));
    if( bHollow == OP_TRUE )
    {
        widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)midColor);
                                
        widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)midColor);
                                
        widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)whiteColor);
        
        widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)whiteColor);

        if( bThin == OP_FALSE )
        {
            left++;
            top++;
            right--;
            bottom--;
            
            widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)darkColor);
                                    
            widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)darkColor);
                                    
            widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)lightColor);
            
            widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)lightColor);
        }
    }
    else
    {
        widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)whiteColor);
                                
        widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)whiteColor);
                                
        widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)midColor);
        
        widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)midColor);

        if( bThin == OP_FALSE )
        {
            left++;
            top++;
            right--;
            bottom--;
            
            widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)lightColor);
                                    
            widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)lightColor);
                                    
            widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)darkColor);
            
            widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)darkColor);
        }
    }
#else

    darkColor = COLOR_SHADOW_DARK_GRAY;
    midColor = COLOR_SHADOW_GRAY;
    lightColor = COLOR_SHADOW_LIGHT_GRAY;
    whiteColor = (DS_COLOR)COLOR_WHITE;
    
    if( bHollow == OP_TRUE )
    {
        widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)darkColor);
                                
        widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)darkColor);
                                
        widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)lightColor);
        
        widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)lightColor);

        if( bThin == OP_FALSE )
        {
            left++;
            top++;
            right--;
            bottom--;
            
            widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)midColor);
                                    
            widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)midColor);
                                    
            widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)whiteColor);
            
            widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)whiteColor);
        }
    }
    else
    {
        widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)lightColor);
                                
        widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)lightColor);
                                
        widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)darkColor);
        
        widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)darkColor);

        if( bThin == OP_FALSE )
        {
            left++;
            top++;
            right--;
            bottom--;
            
            widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)whiteColor);
                                    
            widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom -1) , (DS_COLOR)whiteColor);
                                    
            widgetScreenDrawLine( pWin, left, bottom, right, bottom, (DS_COLOR)midColor);
            
            widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)midColor);
        }
    }
    
#endif

    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetDrawSelectGroupBorder
( 
    const MsfSelectgroup *pSelectgroup,
    MsfPosition  *pPos, 
    MsfSize *pSize, 
    OP_INT16 labelLen
)
{
    MsfWindow       *pWin;
    OP_INT16       left;
    OP_INT16       top;
    OP_INT16       right;
    OP_INT16       bottom;

    DS_COLOR     whiteColor, midColor;
    
    if(!pSelectgroup || !pPos || !pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin = pSelectgroup->gadgetData.parent;
    if( !pWin )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    if( labelLen + 16 > pSize->width )
    {
        labelLen = pSize->width - 16;
    }
    
    right = (OP_INT16)pPos->x + pSize->width - 1;
    bottom = (OP_INT16)pPos->y + pSize->height - 1;

#if 0
    widgetGetShodowColors( 
                    &whiteColor, 
                    OP_NULL, 
                    &midColor, 
                    OP_NULL,
                    (DS_COLOR)_RGB(DEFAULT_BG_COLORS.r, DEFAULT_BG_COLORS.g, DEFAULT_BG_COLORS.b));
#else

    midColor = COLOR_SHADOW_DARK_GRAY;
    whiteColor = (DS_COLOR)COLOR_SHADOW_LIGHT_GRAY;

#endif

    if( labelLen != 0 )
    {
        left = (OP_INT16)pPos->x;
        top = (OP_INT16)pPos->y + GADGET_LINE_HEIGHT /2 - 1;
        /* top Line 1  */
        widgetScreenDrawLine( pWin,
                                                 left, 
                                                 top, 
                                                 6 , 
                                                 top, 
                                                 (DS_COLOR)midColor);
        widgetScreenDrawLine( pWin,
                                                 left, 
                                                 (OP_INT16)(top+1), 
                                                 6 , 
                                                 (OP_INT16)(top+1), 
                                                 (DS_COLOR)whiteColor);

        /* top Line 2  */
        widgetScreenDrawLine( pWin,
                                                 (OP_INT16)(left + 10 + labelLen),
                                                 top, 
                                                 (OP_INT16)(right -1) , 
                                                 top,
                                                 (DS_COLOR)midColor);
        widgetScreenDrawLine( pWin,
                                                 (OP_INT16)(left + 10 + labelLen), 
                                                 (OP_INT16)(top+1), 
                                                 (OP_INT16)(right -2) ,
                                                 (OP_INT16)(top+1), 
                                                 (DS_COLOR)whiteColor);
    }
    else
    {
        left = (OP_INT16)pPos->x;
        top = (OP_INT16)pPos->y;
        
        /* top Line  */
        widgetScreenDrawLine( pWin,
                                                 left, 
                                                 top, 
                                                 (OP_INT16)(right -1), 
                                                 top, 
                                                 (DS_COLOR)midColor);
        widgetScreenDrawLine( pWin,
                                                 left, 
                                                 (OP_INT16)(top+1), 
                                                 (OP_INT16)(right -2), 
                                                 (OP_INT16)(top+1), 
                                                 (DS_COLOR)whiteColor);
    }

    /*  left Line */
    widgetScreenDrawLine( pWin,
                                             left, 
                                             (OP_INT16)(top+1), 
                                             left , 
                                             (OP_INT16)(bottom-1), 
                                             (DS_COLOR)midColor);
    widgetScreenDrawLine( pWin,
                                             (OP_INT16)(left+1), 
                                             (OP_INT16)(top+1), 
                                             (OP_INT16)(left+1), 
                                             (OP_INT16)(bottom-2), 
                                             (DS_COLOR)whiteColor);

    /*  bottom Line */
    widgetScreenDrawLine( pWin,
                                             left, 
                                             (OP_INT16)(bottom-1), 
                                             (OP_INT16)(right -1) ,
                                             (OP_INT16)(bottom-1), 
                                             (DS_COLOR)midColor);
    widgetScreenDrawLine( pWin,
                                             left, 
                                             bottom, 
                                             right, 
                                             bottom, 
                                             (DS_COLOR)whiteColor);

    /*  right Line */
    widgetScreenDrawLine( pWin,
                                             (OP_INT16)(right -1), 
                                             (OP_INT16)(top + 1), 
                                             (OP_INT16)(right -1) ,
                                             (OP_INT16)(bottom-1), 
                                             (DS_COLOR)midColor);
    widgetScreenDrawLine( pWin,
                                             right, 
                                             top, 
                                             right, 
                                             bottom, 
                                             (DS_COLOR)whiteColor);

    return labelLen;    
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static OP_BOOLEAN  widgetChoiceAdjustItemScrollPos
(
    MsfSelectgroup                      *pSelectGroup,
    MsfWidgetDrawChoiceItem *pDrawChoiceItem,
    MsfWidgetDrawString          *pDrawString
)
{
    OP_INT16           nDeltaEngChar;
    OP_INT16           iDeltaX;
    
    if( pSelectGroup->choice.bTimerStarted == OP_FALSE)  /*  the first time display the item as focus item   */
    {
#ifndef  MENU_ITEM_STRING_SCROLL_PERIODICALLY
        if (!(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU ))
#endif        
        {
            pSelectGroup->choice.iCurItemStringLen = UstrCharCount( pDrawString->pBuf );
            if( pSelectGroup->choice.iCurItemStringLen > pSelectGroup->choice.iItemVisibleLen )  /* need scroll-period display  */
            {
                pSelectGroup->choice.iCurItemStringPos = pSelectGroup->choice.iItemVisibleLen;    /* set the start position  */

                /*  start up the scroll-period display timer  */
                OPUS_Start_Timer( OPUS_TIMER_CHOICITEM_SCROLL, CHOICE_ITEM_SCROLL_INTERVAL,  0, PERIODIC );
                pSelectGroup->choice.bTimerStarted = OP_TRUE;
            }
        }
    }
    else 
#ifndef  MENU_ITEM_STRING_SCROLL_PERIODICALLY
    if( !(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU ))
#endif    
    {           /* the scroll-period display  already start   */
        /*             -------------------
          *             |            |string  data  | 
          *             -------------------
          *                                 ^  iCurItemStringPos 
          */
        if( pSelectGroup->choice.iCurItemStringPos >  pSelectGroup->choice.iItemVisibleLen )
        {
            nDeltaEngChar = pSelectGroup->choice.iCurItemStringPos - pSelectGroup->choice.iItemVisibleLen;
            while( nDeltaEngChar > 0 && (pDrawString->pBuf[0] != 0x00 ||pDrawString->pBuf[1] != 0x00 ))
            {
                if( pDrawString->pBuf[0] < 0x7F &&  pDrawString->pBuf[1] == 0x00 )  /* english char  */
                {
                    nDeltaEngChar--;
                }
                else
                {
                    nDeltaEngChar -= 2;
                }

                pDrawString->pBuf += 2;
            }

            if( nDeltaEngChar < 0 )       /* avoid  uneven  scrolling   */
            {
/*                pDrawString->pBuf -= 2;
                pDrawString->pos.x -= pDrawChoiceItem->iEngCharWidth;
                pDrawString->size.width += pDrawChoiceItem->iEngCharWidth;   */
                
                pDrawString->pos.x += pDrawChoiceItem->iEngCharWidth;
                pDrawString->size.width -= pDrawChoiceItem->iEngCharWidth;
            }
        }
        
        /*             -------------------
          *             |            |string  data  | 
          *             -------------------
          *                    ^  iCurItemStringPos 
          */
        else if( pSelectGroup->choice.iCurItemStringPos <  pSelectGroup->choice.iItemVisibleLen )
        {
            nDeltaEngChar = pSelectGroup->choice.iItemVisibleLen - pSelectGroup->choice.iCurItemStringPos;
            iDeltaX = nDeltaEngChar * pDrawChoiceItem->iEngCharWidth;
            pDrawString->pos.x += iDeltaX;
            pDrawString->size.width -= iDeltaX;                
        }
    }

    return OP_TRUE;
}

/*==================================================================================================
    FUNCTION:  widgetDrawChoiceItem

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetChoiceDrawItem
(
    MsfSelectgroup                      *pSelectGroup,
    MsfWidgetDrawChoiceItem *pDrawChoiceItem
)
{
    MsfWindow           *pWin;
    int                       iRet;
    
    if( !pSelectGroup ||!pDrawChoiceItem )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    /* not add to window  */
    pWin = pSelectGroup->gadgetData.parent;
    if( !pWin )
    {
        return TPI_WIDGET_OK;
    }

    /* no room to draw the item */
    if( pSelectGroup->iItemDeltaY >= pDrawChoiceItem->size.height )
    {
        return TPI_WIDGET_OK;
    }
    
    /* focused item or always showing the hilight bar, draw highlight bar */
    if( !(pSelectGroup->showStyle & SG_SHOW_STYLE_MAIN_MENU ))
    {
        if ( ((pDrawChoiceItem->bChoiceFocus == OP_TRUE )
            || (pSelectGroup->showStyle & SG_SHOW_STYLE_HILIGHT_ALWAYS ))
            && ( pDrawChoiceItem->pItem->state & MSF_CHOICE_ELEMENT_FOCUSED ))
        {
            MsfSize focusSize;
            RM_RESOURCE_ID_T focusID;
            if(COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
            {
                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU) 
                {               
                    if(pSelectGroup->gadgetData.hVScrollBar)
                    {
                        focusID = MENU_ITEM_FOCUS_PIC_VS_SB;
                    }
                    else
                    {
                        focusID = MENU_ITEM_FOCUS_PIC;
                    }
                    widgetGetImageSize(focusID, &focusSize);
                    
                    if( RM_IsItBitmap(focusID) )
                    {
                        widgetScreenDrawBitmapRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y, 
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);
                    }
                    else if( RM_IsItIcon(focusID) )
                    {
                        widgetScreenDrawIconRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y,
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);            
                        
                    }                 
                    
                } 
                else
                {
                    
                    widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);
                }
             
            }
            else
            {
                /* draw the focus bar  */
                //pDrawChoiceItem->size = focusSize;
#if (PROJECT_ID == B1_PROJECT) 
                MsfPosition       TempChiceItemPos;       
                MsfSize            TempChiceItemSize;  
                /*the item size:
                    the item hieght is 21-7=14 ,the left border and right border are 15*2=30,
                    per item pos y is start 3 from per item top
                */
                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
                {
                    TempChiceItemSize.height = CHOICE_MENU_LINE_HEIGHT-6;
                    TempChiceItemSize.width = pDrawChoiceItem->size.width-MENU_LEFT_BORDER*2-2;
                    TempChiceItemPos.x=pDrawChoiceItem->pos.x+MENU_LEFT_BORDER+1;
                    TempChiceItemPos.y=pDrawChoiceItem->pos.y+3;               
                    widgetDrawHilight( pWin, &TempChiceItemPos, &TempChiceItemSize, pDrawChoiceItem->bFullFocusRect, OP_FALSE);
                }
                else
                {
                     pDrawChoiceItem->size.height -= 1;
                     widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);               
                }
#elif((defined MENU_ITEM_DRAW_INDEX_BY_OWNER_WITH_BG_PIC)||(PROJECT_ID == WHALE1_PROJECT)||(PROJECT_ID == WHALE2_PROJECT)||(PROJECT_ID == NEMO_PROJECT))

                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
                {
                     if(pSelectGroup->gadgetData.hVScrollBar)
                     {
                         focusID = MENU_ITEM_FOCUS_PIC_VS_SB_BY_USER;
                     }
                     else
                     {
                         focusID = MENU_ITEM_FOCUS_PIC_BY_USER;
                     }
                     widgetGetImageSize(focusID, &focusSize);
                     
                     if( RM_IsItBitmap(focusID) )
                     {
                         widgetScreenDrawBitmapRm((MsfWindow *)pWin, 
                             pDrawChoiceItem->pos.x,
                             pDrawChoiceItem->pos.y, 
                             (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                             (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                             focusID);
                     }
                     else if( RM_IsItIcon(focusID) )
                     {
                         widgetScreenDrawIconRm((MsfWindow *)pWin, 
                             pDrawChoiceItem->pos.x,
                             pDrawChoiceItem->pos.y,
                             (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                             (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                             focusID);            
                         
                     }
                }
                else
                {
                     pDrawChoiceItem->size.height -= 1;
                     widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);
                }
#elif (PROJECT_ID == DRAGON_S_PROJECT)||(PROJECT_ID == ODIN_PROJECT)
            if(COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
            {
                MsfSize focusSize;
                RM_RESOURCE_ID_T focusID;
                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU) 
                {               
                    if(pSelectGroup->gadgetData.hVScrollBar)
                    {
                        focusID = MENU_ITEM_FOCUS_PIC_VS_SB;
                    }
                    else
                    {
                        focusID = MENU_ITEM_FOCUS_PIC;
                    }
                    widgetGetImageSize(focusID, &focusSize);
                    
                    if( RM_IsItBitmap(focusID) )
                    {
                        widgetScreenDrawBitmapRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y, 
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);
                    }
                    else if( RM_IsItIcon(focusID) )
                    {
                        widgetScreenDrawIconRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y,
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);            
                        
                    }                 
                    
                  }
              }
              else
              {                              
                    widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);                                        
                    pDrawChoiceItem->size.height -= 1;                              
              }                
#elif (PROJECT_ID == VIOLET1_PROJECT)
            if(COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
            {
                MsfSize focusSize;
                RM_RESOURCE_ID_T focusID;
                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU) 
                {               
                    if(pSelectGroup->gadgetData.hVScrollBar)
                    {
                        focusID = MENU_ITEM_FOCUS_PIC_VS_SB;
                    }
                    else
                    {
                        focusID = MENU_ITEM_FOCUS_PIC;
                    }
                    widgetGetImageSize(focusID, &focusSize);
                    
                    if( RM_IsItBitmap(focusID) )
                    {
                        widgetScreenDrawBitmapRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y, 
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);
                    }
                    else if( RM_IsItIcon(focusID) )
                    {
                        widgetScreenDrawIconRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y,
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);            
                        
                    }                 
                    
                  }
              }
              else
              {
               if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU) 
                {                

                    if(!pSelectGroup->gadgetData.hVScrollBar)
                    {
                        pDrawChoiceItem->size.width-=11;
                    }else
                    {
                        pDrawChoiceItem->size.width-=6;
                    }
                    pDrawChoiceItem->pos.x+=6; 
                   
                    widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);                                        
                    widgetScreenDrawRect(pWin,(pDrawChoiceItem->pos.x-1),pDrawChoiceItem->pos.y,(pDrawChoiceItem->pos.x-1+pDrawChoiceItem->size.width),pDrawChoiceItem->pos.y+pDrawChoiceItem->size.height,COLOR_BLACK);                    
                    pDrawChoiceItem->pos.x-=6;
                    if(!pSelectGroup->gadgetData.hVScrollBar)
                    {
                        pDrawChoiceItem->size.width+=11;
                    }else
                    {
                        pDrawChoiceItem->size.width+=6;
                    } 
                    pDrawChoiceItem->pos.y+=2;                     
               } 
               else
               {                   
                   widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);                                        
                   widgetScreenDrawRect(pWin,(pDrawChoiceItem->pos.x-1),pDrawChoiceItem->pos.y,(pDrawChoiceItem->pos.x-1+pDrawChoiceItem->size.width),pDrawChoiceItem->pos.y+pDrawChoiceItem->size.height,COLOR_BLACK);                                       
               }                    
              }
#elif  (PROJECT_ID == DRAGON_M_PROJECT)
            if(COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
            {
                MsfSize focusSize;
                RM_RESOURCE_ID_T focusID;
                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU) 
                {               
                    if(pSelectGroup->gadgetData.hVScrollBar)
                    {
                        focusID = MENU_ITEM_FOCUS_PIC_VS_SB;
                    }
                    else
                    {
                        focusID = MENU_ITEM_FOCUS_PIC;
                    }
                    widgetGetImageSize(focusID, &focusSize);
                    
                    if( RM_IsItBitmap(focusID) )
                    {
                        widgetScreenDrawBitmapRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y, 
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);
                    }
                    else if( RM_IsItIcon(focusID) )
                    {
                        widgetScreenDrawIconRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y,
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);            
                        
                    }                 
                    
                  }
              }
              else
              {     
                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU) 
                {          
                    pDrawChoiceItem->size.height -= 2;
                    if(!pSelectGroup->gadgetData.hVScrollBar)
                    {
                        pDrawChoiceItem->size.width-=28;
                    }else
                    {
                        pDrawChoiceItem->size.width-=23;
                    }
                    pDrawChoiceItem->pos.x+=19; 
                   
                    widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);                                        
                    widgetScreenDrawRect(pWin,(pDrawChoiceItem->pos.x-1),pDrawChoiceItem->pos.y,(pDrawChoiceItem->pos.x-1+pDrawChoiceItem->size.width),pDrawChoiceItem->pos.y+pDrawChoiceItem->size.height,COLOR_BLACK);                    
                    pDrawChoiceItem->pos.x-=19;
                    if(!pSelectGroup->gadgetData.hVScrollBar)
                    {
                        pDrawChoiceItem->size.width+=28;
                    }else
                    {
                        pDrawChoiceItem->size.width+=23;
                    }                 
                    
                }else
                {
                    pDrawChoiceItem->size.height -= 1;
                    widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);
                    widgetScreenDrawRect(pWin,(pDrawChoiceItem->pos.x-1),pDrawChoiceItem->pos.y,(pDrawChoiceItem->pos.x-1+pDrawChoiceItem->size.width),pDrawChoiceItem->pos.y+pDrawChoiceItem->size.height,COLOR_BLACK);                    
                }                    
              }
                pDrawChoiceItem->size.height -= 1;              

#else
                pDrawChoiceItem->size.height -= 1;
                widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);
#endif               
            }
        }
    }

    /* draw the item string  */
    iRet = widgetChoiceDrawItemString( pSelectGroup, pDrawChoiceItem );
    if( iRet < 0 )
    {
        return iRet;
    }
    
    /*  draw the item image(s)   */
    widgetChoiceDrawItemImages( pSelectGroup, pDrawChoiceItem );

    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  widgetChoiceDrawItemString

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetChoiceDrawItemString
(
    MsfSelectgroup                      *pSelectGroup,
    MsfWidgetDrawChoiceItem *pDrawChoiceItem
)
{
    MsfWindow           *pWin;
    MsfStringHandle     hStr;
    OP_UINT8            *pChar;
    MsfWidgetType       widgetType;
    MsfWidgetDrawString drawString;
    DS_COLOR            whiteColor, midColor = MENU_ITEM_DISABLE_COLOR;
    choiceElement       *pItem;
    
    if( !pSelectGroup ||!pDrawChoiceItem )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    /* not add to window  */
    pWin = pSelectGroup->gadgetData.parent;
    if( !pWin )
    {
        return TPI_WIDGET_OK;
    }

    hStr = pDrawChoiceItem->pItem->string1;
    if( !ISVALIDHANDLE( hStr ))
    {
        hStr = pDrawChoiceItem->pItem->string2;
        if( !ISVALIDHANDLE( hStr ))  /* no item string to be draw */
        {
            return TPI_WIDGET_OK;
        }
    }

    WIDGET_GET_STRING_DATA(pChar, hStr);
        
    if( !pChar )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pItem = pDrawChoiceItem->pItem;
    if(pSelectGroup->showStyle & SG_SHOW_STYLE_MAIN_MENU)
    {
#if 0
        int                        nbrOfCharacters;
        int                        nbrOfInitialWhiteSpaces; 
        OP_INT16                  iLenByPixel;         
        MsfSize                    choiceItemSize;
       
        choiceItemSize.height = CHOICE_MAIN_MENU_LINE_HEIGHT;
        choiceItemSize.width = pDrawChoiceItem->part.iStringWidth;
        iLenByPixel = widgetStringGetVisibleByChar(pChar,
                                                &choiceItemSize,
                                                pDrawChoiceItem->iEngCharWidth,
                                                OP_FALSE,
                                                &nbrOfCharacters,
                                                &nbrOfInitialWhiteSpaces);
       if( pDrawChoiceItem->part.iStringWidth > iLenByPixel)
       {
            drawString.pos.x =  pDrawChoiceItem->pos.x +pDrawChoiceItem->part.iStringPos +  (choiceItemSize.width - iLenByPixel)/2;
            drawString.size.width= iLenByPixel;
       }
       else
       {
            drawString.pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iStringPos;
            drawString.size.width= pDrawChoiceItem->part.iStringWidth;
       }
#endif       
    }
#if  (PROJECT_ID == B1_PROJECT) 
    else if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
    {
        int                        nbrOfCharacters;
        int                        nbrOfInitialWhiteSpaces; 
        OP_INT16                  iLenByPixel;         
        MsfSize                    choiceItemSize;
       
        choiceItemSize.height = CHOICE_MENU_LINE_HEIGHT;
        choiceItemSize.width = pDrawChoiceItem->part.iStringWidth;
        iLenByPixel = widgetStringGetVisibleByChar(pChar,
                                                &choiceItemSize,
                                                pDrawChoiceItem->iEngCharWidth,
                                                OP_FALSE,
                                                &nbrOfCharacters,
                                                &nbrOfInitialWhiteSpaces);
       if( pDrawChoiceItem->part.iStringWidth > iLenByPixel)
       {
            drawString.pos.x =  pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iStringPos + (pDrawChoiceItem->part.iStringWidth - iLenByPixel)/2;
            drawString.size.width= iLenByPixel;
       }
       else
       {
            drawString.pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iStringPos;
            drawString.size.width= pDrawChoiceItem->part.iStringWidth;
       }
    }
#endif    
    else
    {
        drawString.pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iStringPos;
        drawString.size.width= pDrawChoiceItem->part.iStringWidth;
    }
    drawString.pos.y = pDrawChoiceItem->pos.y + pSelectGroup->iItemDeltaY;
    drawString.size.height = pDrawChoiceItem->size.height - pSelectGroup->iItemDeltaY;
    
    drawString.pBuf = pChar;
    drawString.backcolor = pDrawChoiceItem->bgColor;
    drawString.fontAttr = pDrawChoiceItem->fontAttr;
    drawString.bShowInitialWhiteSpaces = OP_FALSE;
#if(ODIN_PROJECT == PROJECT_ID)
     /* focused line */
    if ( (pDrawChoiceItem->bChoiceFocus == OP_TRUE)
        && ( pItem->state & MSF_CHOICE_ELEMENT_FOCUSED ))
    {
        /* 
          * maybe need to adjust the x position , width and pBuf position 
          */
        widgetChoiceAdjustItemScrollPos( pSelectGroup, pDrawChoiceItem, &drawString ) ;
        pDrawChoiceItem->fontColor = ds.cComposer.focusbarFontColor;
    }
    else
    {
       pDrawChoiceItem->fontColor = ds.cComposer.menuFontColor;
    }
#else
     /* focused line */
    if ( (pDrawChoiceItem->bChoiceFocus == OP_TRUE)
        && ( pItem->state & MSF_CHOICE_ELEMENT_FOCUSED ))
    {
        /* 
          * maybe need to adjust the x position , width and pBuf position 
          */
        widgetChoiceAdjustItemScrollPos( pSelectGroup, pDrawChoiceItem, &drawString ) ;

    }
#endif    
    
    /*  draw  disable item first time with light color */
    if( pItem->state & MSF_CHOICE_ELEMENT_DISABLED )
    {
#ifdef MENU_ITEM_DISABLED_WITH_SHADOW    
        widgetGetShodowColors( &whiteColor, OP_NULL, OP_NULL, OP_NULL, pDrawChoiceItem->fontColor );
        
        drawString.fontcolor = whiteColor;
        drawString.pos.y ++;
        drawString.size.height --;
        drawString.pos.x ++ ;
#else
        drawString.fontcolor = midColor;
#endif    
    }
    else
    {
#if  (PROJECT_ID == B1_PROJECT)
        if(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
        {
            drawString.fontcolor = ds.cComposer.focusbarFontColor;   /* focus color is back  */
        }
        else
        {
            drawString.fontcolor = pDrawChoiceItem->fontColor;
        }
#elif (PROJECT_ID == LOTUS_PROJECT)
        if( (pSelectGroup->showStyle & SG_SHOW_STYLE_MENU)
           && (pItem->state & MSF_CHOICE_ELEMENT_FOCUSED))
        {
            drawString.fontcolor = COLOR_WHITE;   /* focus color is back  */
        }
        else
        {
            drawString.fontcolor = pDrawChoiceItem->fontColor;
        }
#elif((PROJECT_ID == KNIGHT_PROJECT)||(PROJECT_ID == WHALE1_PROJECT)||(PROJECT_ID == WHALE2_PROJECT)||(PROJECT_ID == NEMO_PROJECT))
        if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
        {
            if(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            {
                drawString.fontcolor = ds.cComposer.focusbarFontColor;
            }
            else
            {
                drawString.fontcolor = ds.cComposer.menuFontColor;
            }
        }
        else
        {
            drawString.fontcolor = pDrawChoiceItem->fontColor;
        }
#elif (PROJECT_ID == DRAGON_M_PROJECT)
  if(COLOR_THEME_USER_DEFINE == CUR_COLOR_THEME )
  {
   if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
        {
          if(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            {
                 drawString.fontcolor=~(ds.cComposer.focusbarColor);
             }
           else
            {
                 drawString.fontcolor=COLOR_WHITE;
            }
       }else
       {
          if(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            {
                 drawString.fontcolor=~(ds.cComposer.focusbarColor);
             }
           else
            {
                 drawString.fontcolor=COLOR_BLACK;
            }       
       }
   }else
   {
       if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
        {
          if(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            {
                 drawString.fontcolor = COLOR_BLACK;
            }
             else
            {
                 drawString.fontcolor = COLOR_WHITE;
            }
          }
          else
          {
           if((pSelectGroup->gadgetData.isFocused)&&(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED))
            {
                 drawString.fontcolor = COLOR_WHITE;
            }
             else
            {
                 drawString.fontcolor = COLOR_BLACK;
            }
          }
   }
#elif (PROJECT_ID == VIOLET1_PROJECT)
  if(COLOR_THEME_USER_DEFINE == CUR_COLOR_THEME )
  {
          if(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            {
                 drawString.fontcolor=~(ds.cComposer.focusbarColor);
             }
           else
            {
                 drawString.fontcolor=COLOR_BLACK;
            }     
   }
   else
   {
                 drawString.fontcolor = COLOR_BLACK;           
   } 
#elif (PROJECT_ID == DRAGON_S_PROJECT)||(PROJECT_ID == DOLPHIN_PROJECT)||(PROJECT_ID == TULIP_PROJECT)
  if(COLOR_THEME_USER_DEFINE == CUR_COLOR_THEME )
  {  
          if(pItem->state & MSF_CHOICE_ELEMENT_FOCUSED)
            {
                 drawString.fontcolor=~(ds.cComposer.focusbarColor);
            }
            else
            {
                 drawString.fontcolor = COLOR_BLACK;            
            }             
 }
 else
 {
                drawString.fontcolor = COLOR_BLACK;                
 }
#else
        drawString.fontcolor = pDrawChoiceItem->fontColor;
#endif
    }

    widgetDrawStringByChar( pWin, &drawString );

#ifdef MENU_ITEM_DISABLED_WITH_SHADOW
    /*  draw  disable item second time with middle level color  */
    if( pItem->state & MSF_CHOICE_ELEMENT_DISABLED )
    {
        drawString.pos.y --;
        drawString.size.height ++;
        drawString.pos.x -- ;
        drawString.fontcolor = midColor;
        widgetDrawStringByChar( pWin, &drawString );

        /* There is a char in COLOR_WHITE   which is drawn on the right hilight border line   */
        if( (pDrawChoiceItem->bChoiceFocus == OP_TRUE)          /*the item in focus  */
            && ( pDrawChoiceItem->bFullFocusRect == OP_TRUE ) 
            && ( pItem->state & MSF_CHOICE_ELEMENT_FOCUSED )
            && ( pDrawChoiceItem->part.nCount < 3 )  /* there is no item image at the right of the item string  */
            && ( drawString.size.width % pDrawChoiceItem->iEngCharWidth == 0 )) 
        {
            
            MsfSize focusSize;
            RM_RESOURCE_ID_T focusID;
            if(COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
            {
                if(pSelectGroup->gadgetData.hVScrollBar)
                {
                    focusID = MENU_ITEM_FOCUS_PIC_VS_SB;
                }
                else
                {
                    focusID = MENU_ITEM_FOCUS_PIC;
                }
                widgetGetImageSize(focusID, &focusSize);
                if(pSelectGroup->showStyle & SG_SHOW_STYLE_MENU) 
                {
                    if( RM_IsItBitmap(focusID) )
                    {
                        widgetScreenDrawBitmapRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y, 
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);
                    }
                    else if( RM_IsItIcon(focusID) )
                    {
                        widgetScreenDrawIconRm((MsfWindow *)pWin, 
                            pDrawChoiceItem->pos.x,
                            pDrawChoiceItem->pos.y,
                            (OP_INT16)((focusSize.width < pDrawChoiceItem->size.width)?(focusSize.width):(pDrawChoiceItem->size.width)),
                            (OP_INT16)((focusSize.height< pDrawChoiceItem->size.height)?(focusSize.height):(pDrawChoiceItem->size.height)),
                            focusID);            
                        
                    }
                   
                } 
                else
                {
                    
                    widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);
                }
                
            }
            else
            {
                /* draw the focus bar  */
                //pDrawChoiceItem->size = focusSize;
                pDrawChoiceItem->size.height -= 1;
                widgetDrawHilight( pWin, &pDrawChoiceItem->pos, &pDrawChoiceItem->size, pDrawChoiceItem->bFullFocusRect, OP_FALSE);
            }  
        }
    }
#endif        

    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  widgetChoiceDrawItemImage

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetChoiceDrawItemImages
(
    MsfSelectgroup                      *pSelectGroup,
    MsfWidgetDrawChoiceItem *pDrawChoiceItem
)
    {
    MsfWindow                   *pWin;
    choiceElement              *pItem;
    RM_RESOURCE_ID_T  rm_icon;
    MsfIconType               iconType;
    MsfPosition                 pos;
    MsfSize                       size;
    MsfImageHandle        hImage;
    MsfImage                    *pImage;
    MsfWidgetType           widgetType;

    int                              iRet;
        
    if( !pSelectGroup ||!pDrawChoiceItem )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
        
    /* not add to window  */
    pWin = pSelectGroup->gadgetData.parent;
    if( !pWin )
    {
        return TPI_WIDGET_OK;
    }
    
    /* No image to draw  */
    if( pDrawChoiceItem->part.nCount < 2 )
    {
        return TPI_WIDGET_OK;
    }
    
    pItem = pDrawChoiceItem->pItem;

    /*  need show index  */
    if( pSelectGroup->choice.bitmask & MSF_CHOICE_ELEMENT_INDEX )
    {
#if(PROJECT_ID == DRAGON_S_PROJECT )        
        iRet = widgetScreenDrawIconRm( 
                                    pWin, 
                                    (OP_INT16)(pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage1Pos ), 
                                    pDrawChoiceItem->pos.y ,
                                    pDrawChoiceItem->part.iImage1Width,
                                    pDrawChoiceItem->size.height,
                                    ICON_SECONDMENU_ITEM_BALL);  
#endif                                    
#ifdef MENU_ITEM_DRAW_INDEX_BY_OWNER_WITH_BG_PIC
        {
            MsfWidgetDrawString  drawString;     
            OP_UINT8 num[42];
            RM_RESOURCE_ID_T  itemBgId;
            DS_COLOREDCOMPOSER_T choiceCompColor;
            
            widgetGetComposedColor(&choiceCompColor);
            if( COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
            {
                itemBgId = choiceCompColor.color_theme + BMP_SECONDMENULINE_NUM_ICON;
            }
            else
            {
                itemBgId = BMP_SECONDMENULINE_NUM_ICON;
            }

            if( !(( pDrawChoiceItem->pItem->state & MSF_CHOICE_ELEMENT_FOCUSED) && pDrawChoiceItem->bChoiceFocus) )
            {
                iRet=widgetScreenDrawBitmapRm(
                                                        pWin,
                                                        (OP_INT16)(pDrawChoiceItem->pos.x +pDrawChoiceItem->part.iImage1Pos),
                                                        pDrawChoiceItem->pos.y,
                                                        pDrawChoiceItem->part.iImage1Width,
                                                        pDrawChoiceItem->size.height,
                                                        itemBgId);
            }
            else
            {
                iRet = TPI_WIDGET_OK;
            }

            if( pSelectGroup->choice.top_item + pDrawChoiceItem->index >= 9 )
            {
                drawString.pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage1Pos + 6;
                drawString.size.width = pDrawChoiceItem->part.iImage1Width - 6;
            }
            else
            {
                drawString.pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage1Pos + 10;
                drawString.size.width = pDrawChoiceItem->part.iImage1Width - 10;
            }
            drawString.pos.y = pDrawChoiceItem->pos.y + 6;
            drawString.size.height = CHOICE_MENU_LINE_HEIGHT;
            drawString.fontcolor = COLOR_BLACK;
            drawString.fontAttr =  FONT_DEFAULT | FONT_OVERLAP;
            drawString.backcolor = pDrawChoiceItem->bgColor;
            drawString.bShowInitialWhiteSpaces = OP_FALSE;

            NumtoU(num, pSelectGroup->choice.top_item + pDrawChoiceItem->index + 1);
            drawString.pBuf=num;
            widgetDrawStringByChar( pWin, &drawString );
            drawString.pos.x++;
            widgetDrawStringByChar( pWin, &drawString );            
        }
#elif( (PROJECT_ID == WHALE1_PROJECT )||(PROJECT_ID == WHALE2_PROJECT))
            
        if( !(( pDrawChoiceItem->pItem->state & MSF_CHOICE_ELEMENT_FOCUSED) && pDrawChoiceItem->bChoiceFocus) )
        {   
           if((pSelectGroup->choice.top_item + pDrawChoiceItem->index)>39)
            {
            iRet=widgetScreenDrawIconRm(
                pWin,
                (OP_INT16)(pDrawChoiceItem->pos.x +pDrawChoiceItem->part.iImage1Pos),
                pDrawChoiceItem->pos.y,
                pDrawChoiceItem->part.iImage1Width,
                pDrawChoiceItem->size.height,
                ICON_LINE_NUM_C1_ASTERISK);                
            }else
            {
                iRet=widgetScreenDrawIconRm(
                    pWin,
                    (OP_INT16)(pDrawChoiceItem->pos.x +pDrawChoiceItem->part.iImage1Pos),
                    pDrawChoiceItem->pos.y,
                    pDrawChoiceItem->part.iImage1Width,
                    pDrawChoiceItem->size.height,
                    SECONDMENU_LINE_RES_WITH_NUM_ICON);
             }
        }
        else
        {
            iRet = TPI_WIDGET_OK;
        }
        if((pSelectGroup->choice.top_item + pDrawChoiceItem->index)>39)
        {
            iRet = widgetScreenDrawIconRm( 
                pWin, 
                (OP_INT16)(pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage1Pos ), 
                pDrawChoiceItem->pos.y + 2 ,
                pDrawChoiceItem->part.iImage1Width,
                pDrawChoiceItem->size.height,
                ICON_LINE_NUM_C1_ASTERISK);
        
        }else
        {
            iRet = widgetScreenDrawIconRm( 
                pWin, 
                (OP_INT16)(pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage1Pos ), 
                pDrawChoiceItem->pos.y + 2 ,
                pDrawChoiceItem->part.iImage1Width,
                pDrawChoiceItem->size.height,
                MENU_ITEM_INDEX_ICON_START+pDrawChoiceItem->index+ pSelectGroup->choice.top_item);
         }
                                    
#elif(PROJECT_ID == NEMO_PROJECT )
#else   
         /*
          *  draw index icon, if the item is focused, use focus index icon, or user normal index icon
          */
        if( pItem->state & MSF_CHOICE_ELEMENT_FOCUSED )
        {
#if  (PROJECT_ID == DRAGON_M_PROJECT )       
            if((pSelectGroup->choice.top_item + pDrawChoiceItem->index)>40)
            {
                rm_icon = ICON_SUBMENU_LINE_NUMBER_ASTERISK;
            }
            else
            {
            rm_icon = pSelectGroup->choice.top_item + pDrawChoiceItem->index
                                 + MENU_ITEM_INDEX_FOCUS_ICON_START;
            }
#else
            rm_icon = pSelectGroup->choice.top_item + pDrawChoiceItem->index
                                 + MENU_ITEM_INDEX_FOCUS_ICON_START;
#endif                                 
        }
        else 
        {
#if  (PROJECT_ID == DRAGON_M_PROJECT ) 
        if((pSelectGroup->choice.top_item + pDrawChoiceItem->index)>40)
        {
            rm_icon = ICON_SUBMENU_LINE_NUMBER_ASTERISK;
        }
        else
        {
            rm_icon = pSelectGroup->choice.top_item + pDrawChoiceItem->index
                                 + MENU_ITEM_INDEX_ICON_START;
        }
#else
            rm_icon = pSelectGroup->choice.top_item + pDrawChoiceItem->index
                                 + MENU_ITEM_INDEX_ICON_START;
#endif                                 
        }

        /* draw the index icon  */
        iRet = widgetScreenDrawIconRm( 
                                    pWin, 
                                    (OP_INT16)(pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage1Pos ), 
                                    pDrawChoiceItem->pos.y ,
                                    pDrawChoiceItem->part.iImage1Width,
                                    pDrawChoiceItem->size.height,
                                    rm_icon);

#endif        
        if( iRet < 0)
        {
            return iRet;
        }
    
        /*
          *  only draw index icon may be has two image, i.e.  pDrawChoiceItem->part.nCount is 3
          */

        if( pDrawChoiceItem->part.nCount != 3 )
        {
            return TPI_WIDGET_OK;
        }
    }
    
    pos.y = pDrawChoiceItem->pos.y;
    size.height= pDrawChoiceItem->size.height;
    
    if( pDrawChoiceItem->part.nCount == 3 )
    {
        pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage2Pos;
        size.width= pDrawChoiceItem->part.iImage2Width;
    }
    else
    {
        pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iImage1Pos;
        size.width= pDrawChoiceItem->part.iImage1Width;
    }

    iRet = TPI_WIDGET_OK;
    
    /* draw image with  our default icon  */
    if( pSelectGroup->choice.bitmask & MSF_CHOICE_ELEMENT_ICON )
    {
#if  (PROJECT_ID == B1_PROJECT)
        if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
        {
            if( pItem->state & MSF_CHOICE_ELEMENT_SELECTED )
            {
                pos.y++;
                iconType = MsfRadioButtonPressed;
                /* get the icon  */
                rm_icon = widgetGetIconResID( iconType );

                /* draw the icon  */
                iRet = widgetScreenDrawIconRm( 
                                            pWin, 
                                            pos.x, 
                                            pos.y, 
                                            size.width,
                                            size.height,
                                            rm_icon);
                    pos.x = pDrawChoiceItem->pos.x + pDrawChoiceItem->part.iStringPos + pDrawChoiceItem->part.iStringWidth;

                    iRet = widgetScreenDrawIconRm( 
                                                pWin, 
                                                pos.x, 
                                                pos.y, 
                                                size.width,
                                                size.height,
                                                rm_icon);
            }                            
        }
        else
        {
            /* calc the icon type  */
            if( pSelectGroup->choice.type == MsfMultipleChoice )
            {
                if( pItem->state & MSF_CHOICE_ELEMENT_SELECTED )
                {
                    iconType = MsfCheckBoxChecked;
                }
                /*  the front has the index icon, so not draw the icon  */
                else if( pDrawChoiceItem->part.nCount == 3 )  
                {
                    iconType = MsfIconNone;
                }
                else
                {
                    iconType = MsfCheckBox;
                }
            }
            else 
            {
                if( pItem->state & MSF_CHOICE_ELEMENT_SELECTED )
                {
                    iconType = MsfRadioButtonPressed;
                }
                /*  the front has the index icon, so not draw the icon  */
                else if( pDrawChoiceItem->part.nCount == 3 )  
                {
                    iconType = MsfIconNone;
                }
                else
                {
                    iconType = MsfRadioButton;
                }
            }

            if( MsfIconNone != iconType )
            {
                /* get the icon  */
                rm_icon = widgetGetIconResID( iconType );

                /* draw the icon  */
                iRet = widgetScreenDrawIconRm( 
                                            pWin, 
                                            pos.x, 
                                            pos.y, 
                                            size.width,
                                            size.height,
                                            rm_icon);
            } 
        }
#else
        /* calc the icon type  */
        if( pSelectGroup->choice.type == MsfMultipleChoice )
        {
            if( pItem->state & MSF_CHOICE_ELEMENT_SELECTED )
            {
                iconType = MsfCheckBoxChecked;
            }
            /*  the front has the index icon, so not draw the icon  */
            else if( pDrawChoiceItem->part.nCount == 3 )  
            {
                iconType = MsfIconNone;
            }
            else
            {
                iconType = MsfCheckBox;
            }
        }
        else 
        {
            if( pItem->state & MSF_CHOICE_ELEMENT_SELECTED )
            {
                iconType = MsfRadioButtonPressed;
            }
            /*  the front has the index icon, so not draw the icon  */
            else if( pDrawChoiceItem->part.nCount == 3 )  
            {
                iconType = MsfIconNone;
            }
            else
            {
                iconType = MsfRadioButton;
            }
        }

        if( MsfIconNone != iconType )
        {
            /* get the icon  */
            rm_icon = widgetGetIconResID( iconType );

            /* draw the icon  */
            iRet = widgetScreenDrawIconRm( 
                                        pWin, 
                                        pos.x, 
                                        pos.y, 
                                        size.width,
                                        size.height,
                                        rm_icon);
        }                                    
#endif
    }
    /* draw the destined image  */
    else if(pSelectGroup->choice.bitmask & (MSF_CHOICE_ELEMENT_IMAGE_1 | MSF_CHOICE_ELEMENT_IMAGE_2))
    {
        OP_BOOLEAN   bUserDefaultIcon = OP_TRUE;
        
        hImage = pItem->image1;
        if( !ISVALIDHANDLE(hImage))
        {
            hImage = pItem->image2;
        }

        if( ISVALIDHANDLE(hImage))
        {
            pImage = (MsfImage*)seekWidget( hImage, &widgetType );
            if( pImage && widgetType == MSF_IMAGE  && pImage->predefinedId != 0 )
            {
                iRet = widgetScreenDrawIcon( pWin, pos.x, pos.y, size.width, size.height,
                                             (RM_ICON_T *)(pImage->data));
                if( iRet >= 0 )
                {
                    bUserDefaultIcon = OP_FALSE;
                }
            }
        }

        /* draw the default icon  */
        if( bUserDefaultIcon == OP_TRUE )
        {
            /* draw the icon  */
            iRet = widgetScreenDrawIconRm( 
                                        pWin, 
                                        pos.x, 
                                        pos.y, 
                                        size.width,
                                        size.height,
                                        widgetGetIconResID( MsfRadioButton ));
        }
    }

    return iRet;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetChoiceDrawEmptyItem
(
    const MsfSelectgroup     *pSelectGroup, 
    const MsfPosition         *pPos, 
    const MsfSize               *pSize
)
{
    MsfWindow         *pWin;
    OP_UINT8         strTmp[30];
    OP_INT16         iLen;
    MsfWidgetDrawString     drawString;
    DS_COLOR        whiteColor, midColor;
    
    if( !pSelectGroup || !pPos || !pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pWin = pSelectGroup->gadgetData.parent;
    if( !pWin )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    /*  form the strTmp with contents '<��>'  */
    strTmp[0] = 0x3C;   /*  '<'  */
    strTmp[1] = 0x00;
    strTmp[2] = 0x00;
    strTmp[3] = 0x00;
    
    util_get_text_from_res_w_max_len(PMPT_TEXT_EMPTY, strTmp+2, sizeof(strTmp) -6);
    iLen = Ustrlen(strTmp);
    strTmp[iLen] = 0x3E;           /* '>'   */
    strTmp[iLen + 1 ] = 0x00;
    strTmp[iLen + 2 ] = 0x00;    /*  OP_NULL char  */
    strTmp[iLen + 3 ] = 0x00;
    
    drawString.pBuf = strTmp;

    /*  get font attribute  */
    widgetGetDrawingInfo( (void *) pSelectGroup, 
                                            getWidgetType(pSelectGroup->gadgetData.gadgetHandle),   
                                            0, 
                                            &drawString.fontAttr, 
                                            &drawString.fontcolor, 
                                            OP_NULL);
                                            
    /*  Menu style, use large size font  */                                            
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
    {
        drawString.fontAttr &=~ FONT_SIZE_SMALL;
        drawString.fontAttr |= FONT_SIZE_MED;
    }
    
    /*  set the target  area  */
#if (PROJECT_ID == NEMO_PROJECT)    
    drawString.pos.x = pPos->x + GADGET_COLUMN_WIDTH+14;
#else
    drawString.pos.x = pPos->x + GADGET_COLUMN_WIDTH;
#endif    
    drawString.pos.y = pPos->y + pSelectGroup->iItemDeltaY;
                                   
    drawString.size.width = pSize->width -GADGET_COLUMN_WIDTH;
    drawString.size.height= pSize->height -pSelectGroup->iItemDeltaY;

    /* no room to draw */
    if( drawString.size.height <= 0 )
    {
        return TPI_WIDGET_OK;
    }

    /*  get the light color and middle level color  */
    widgetGetShodowColors( &whiteColor, OP_NULL, OP_NULL, OP_NULL, drawString.fontcolor );
    midColor = MENU_ITEM_DISABLE_COLOR;
    
    /* draw the item with light color  */
    drawString.fontAttr |= FONT_OVERLAP;
    drawString.backcolor = COLOR_FONTBG;
    drawString.bShowInitialWhiteSpaces = OP_FALSE;
    
#ifdef MENU_ITEM_DISABLED_WITH_SHADOW    
    drawString.pos.x++;
    drawString.pos.y++;
    drawString.size.width-- ;
    drawString.size.height--;
    
    drawString.fontcolor = whiteColor;
    widgetDrawStringByChar( pWin, &drawString );
    /*  
      * adjust the position, and draw the item with middle level color, 
      * to achieve the shadow effect  
      */
    drawString.pos.x --;
    drawString.pos.y--;
    drawString.size.width++ ;
    drawString.size.height++;
#endif

    drawString.fontcolor = midColor;
    
    widgetDrawStringByChar( pWin, &drawString );

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetChoiceDrawItems

    DESCRIPTION:
        draw the items of selectgroup
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetChoiceDrawItems
( 
    MsfSelectgroup     *pSelectGroup
)
{
    MsfGadget         *pGadget;
    int                      nShowItemPerPage;
    MsfSize              itemSize;
    MsfPosition        pos;
    choiceElement     *pItem;
    int                      iItems;
    int                      iRet;
    OP_INT16          iLimitHeight;
    OP_INT16          iPosY;
    OP_INT16          iItemHeight;
    OP_INT16          iIntervalY;
    OP_INT16          iEngCharHeight;

    MsfWidgetDrawChoiceItem drawChoiceItem;


    if( !pSelectGroup )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pGadget = (MsfGadget*)pSelectGroup;
    
    /* get the selectgroup position and size */
    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y );

    pos.x += pSelectGroup->clientPos.x;
    pos.y += pSelectGroup->clientPos.y;
    
    iLimitHeight = pSelectGroup->iClientHeight;
    /* 
      *  If the choice has no item,  then
      *   draw a non-focus empty item with content '<None>' 
      */
    if( pSelectGroup->choice.count  <= 0 )  
    {
        itemSize.width = pSelectGroup->itemSize.width;
        itemSize.height = MIN(pSelectGroup->itemSize.height, iLimitHeight );
        
        widgetChoiceDrawEmptyItem( pSelectGroup, &pos, &itemSize);
        
        return TPI_WIDGET_OK;
    }

    /* partition the item area into several part, according to the bitmask of selectgroup  */
    iRet = widgetChoicePartitionItemArea( pSelectGroup, &drawChoiceItem.part );
    if( iRet < 0 )
    {
        return iRet;
    }
    
    /* 
      * skip the previous items before the top item
      */
    pItem = pSelectGroup->choice.firstElement;
    iItems = 0;
    while( iItems < pSelectGroup->choice.top_item && pItem )
    {
        pItem = pItem->next;
        iItems++;
    }

    /* calc if  the selectgroup has focus */
    if (pGadget->isFocused)
    {
        drawChoiceItem.bChoiceFocus = OP_TRUE;
    }
    else
    {
        drawChoiceItem.bChoiceFocus = OP_FALSE;
    }

    /*  calc the actual items per page of the selectgroup  */
    nShowItemPerPage = pSelectGroup->choice.count;
    if( nShowItemPerPage > pSelectGroup->choice.elementsPerPage )
    {
        nShowItemPerPage = pSelectGroup->choice.elementsPerPage;
    }

    /* get the font attribute and font color of the select group */
    widgetGetDrawingInfo( (void*) pSelectGroup, 
                                            getWidgetType(pSelectGroup->gadgetData.gadgetHandle), 
                                            0, 
                                            &drawChoiceItem.fontAttr, 
                                            &drawChoiceItem.fontColor, 
                                            OP_NULL );

#ifdef MENU_ITEM_STRING_USE_LARGE_FONT
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU)
    {
        /* menu style:  use large size  font  */
        drawChoiceItem.fontAttr &= ~FONT_SIZE_SMALL;
        drawChoiceItem.fontAttr |= FONT_SIZE_MED;
    }
#endif
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU)
    {
#if(PROJECT_ID == ODIN_PROJECT)
        drawChoiceItem.fontColor = ds.cComposer.menuFontColor;
#endif

    }
    /* calc the english char width  */
    if( 0 > widgetGetFontSizeInEngChar(drawChoiceItem.fontAttr , &drawChoiceItem.iEngCharWidth, &iEngCharHeight))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    /*  calc the width of scroll-period show visible area by english char   */
    pSelectGroup->choice.iItemVisibleLen = drawChoiceItem.part.iStringWidth / drawChoiceItem.iEngCharWidth;

    drawChoiceItem.bgColor = COLOR_FONTBG;
    drawChoiceItem.fontAttr |= FONT_OVERLAP;    

    /* calc if draw focus item with full focus rect or top and bottom lines */
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_FOCUS_DRAW_LINE )
    {
        drawChoiceItem.bFullFocusRect = OP_FALSE;
    }
    else
    {
        drawChoiceItem.bFullFocusRect = OP_TRUE;
    }
    
    /* set the drawing information of item   */
    drawChoiceItem.pos.x = pos.x;
    drawChoiceItem.size.width= pSelectGroup->itemSize.width;
    iItemHeight = pSelectGroup->itemSize.height;
    iPosY = pos.y + pSelectGroup->iSpaceY;
    iLimitHeight -= pSelectGroup->iSpaceY;
    iIntervalY = iItemHeight + pSelectGroup->iSpaceY;
    
    /* 
      * draw the items of  choicegroup 
      */
    iItems = 0;
    while( pItem && iLimitHeight >= CHOICE_LINE_HEIGHT  && iItems < nShowItemPerPage )
    {
        drawChoiceItem.pItem = pItem;
        drawChoiceItem.index = iItems;

        /* adjust the item y position and height  */
        drawChoiceItem.pos.y = iPosY;
        
        /* in the case of the bottom item: maybe arise this situation */
        if( iItemHeight > iLimitHeight )
        {
            iItemHeight = iLimitHeight;
        }
        
        drawChoiceItem.size.height = iItemHeight;

        /* draw the item  */
        if( widgetChoiceDrawItem( pSelectGroup, &drawChoiceItem ) < 0 )
        {
            break;
        }

        /* adjust the y coordinate and the rear height */
        iPosY += iIntervalY;
        iLimitHeight -= iIntervalY;
        
        pItem = pItem->next;
        iItems ++;            
    }
    
    return TPI_WIDGET_OK;

}


/*==================================================================================================
    FUNCTION:  widgetChoiceCalcItemArea

    DESCRIPTION:
        According to the show style of selectgroup and label , calculate the item position 
        information of selectgroup 
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetChoiceCalcItemArea
(
    MsfSelectgroup     *pSelectGroup
)
{
    MsfGadget         *pGadget;
    MsfPosition       pos;
    MsfSize           size;
    OP_UINT8          *pChar;
    MsfWidgetType     wt;
    int               nShowItemPerPage;
    
    if( !pSelectGroup )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pSelectGroup ->bItemPosCalculated == OP_TRUE )
    {
        return TPI_WIDGET_OK;
    }
    
    pGadget = (MsfGadget*)pSelectGroup;

    pos.x = 0;
    pos.y = 0;
    
    size.width = pGadget->size.width;
    size.height= pGadget->size.height;

    /*  
      * calc the position and size info of items area
      */
    if(pSelectGroup->showStyle & SG_SHOW_STYLE_MAIN_MENU)
    {
        pSelectGroup->itemSize.width  = size.width;
        pSelectGroup->itemSize.height = CHOICE_MAIN_MENU_LINE_HEIGHT;
        pSelectGroup->iItemDeltaY = ( CHOICE_MAIN_MENU_LINE_HEIGHT - CHOICE_LINE_HEIGHT ) >> 1;      
        pSelectGroup->iSpaceY = 0;
    }
    else if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )  /* menu style  */
    {
        pSelectGroup->itemSize.width  = size.width;
        pSelectGroup->itemSize.height = CHOICE_MENU_LINE_HEIGHT;
#if (PROJECT_ID == DRAGON_S_PROJECT)||(PROJECT_ID == DRAGON_M_PROJECT)||(PROJECT_ID == VIOLET1_PROJECT)  
        pSelectGroup->iItemDeltaY = (( CHOICE_MENU_LINE_HEIGHT - CHOICE_LINE_HEIGHT ) >> 1)+1;
#else
        pSelectGroup->iItemDeltaY = ( CHOICE_MENU_LINE_HEIGHT - CHOICE_LINE_HEIGHT ) >> 1;
#endif         
        pSelectGroup->iSpaceY = 0;
    }
    else
    {
        /* If  have label, calc the label lenght by pixel */
        pChar = OP_NULL;                                     
        if( ISVALIDHANDLE( pGadget->title)) 
        {
            WIDGET_GET_STRING_DATA(pChar, pGadget->title);
                
            if( !pChar )
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }
        }

        if( pSelectGroup->showStyle & SG_SHOW_STYLE_NO_BORDER  ) /* no border  */
        {
            if( pChar )  /*  have label  */
            {
                pos.y += CHOICE_LINE_HEIGHT;
                size.height -= CHOICE_LINE_HEIGHT;
            }
        }
        else    /* have  border  */
        {
            if( pChar )  /*  have label  */
            {
                pos.y += CHOICE_LINE_HEIGHT;
                size.height -= (CHOICE_LINE_HEIGHT + 2);
            }
            else
            {
                pos.y += 2;
                size.height -= 4;
            }
            
            pos.x += 2;
            size.width -= 4;
        }

        pSelectGroup->itemSize.width  = size.width;
        pSelectGroup->itemSize.height = CHOICE_LINE_HEIGHT;
        pSelectGroup->iItemDeltaY = 0;
        
        /*  calc the space between two item area  */
        if( pSelectGroup->showStyle & SG_SHOW_STYLE_SEQUENCE_LAYOUT ) /* sequence layout  */
        {
            pSelectGroup->iSpaceY = 0;
        }
        else             /*  even layout  */
        {
            nShowItemPerPage = pSelectGroup->choice.count;
            if( nShowItemPerPage > pSelectGroup->choice.elementsPerPage )
            {
                nShowItemPerPage = pSelectGroup->choice.elementsPerPage;
            }

            if( nShowItemPerPage == 0 )
            {
                pSelectGroup->iSpaceY = 2;
            }
            else
            {
                pSelectGroup->iSpaceY = ( size.height - CHOICE_LINE_HEIGHT * nShowItemPerPage ) / ( nShowItemPerPage + 1 );
            }
        }
    }

    /*  the start position of client area  */
    pSelectGroup->clientPos = pos;
    
    /*  the height of client area  */
    pSelectGroup->iClientHeight = size.height;
    
    pSelectGroup ->bItemPosCalculated = OP_TRUE;
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        Partition the item area into several section: front image area, string area, back image area
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetChoicePartitionItemArea
(
    MsfSelectgroup                *pSelectGroup,
    MsfChoiceItemPartition *pPart
)
{
    OP_INT16    iWidth;
    int                iRet;
    
    if( !pSelectGroup || !pPart )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pSelectGroup->bItemPosCalculated == OP_FALSE )
    {
        iRet = widgetChoiceCalcItemArea(pSelectGroup);
        if( iRet < 0 )
        {
            return iRet;
        }
    }
    
    pPart->nCount = 1;
    iWidth = pSelectGroup->itemSize.width;
    if( iWidth <  CHOICE_LINE_HEIGHT ) 
    {
        /* 
          * width not enough to show a image, so only show item string  
          */
        pPart->iStringWidth = iWidth;
        pPart->iStringPos = 0;
    }

    /*  need show index icon  */
    else if( pSelectGroup->choice.bitmask & MSF_CHOICE_ELEMENT_INDEX )
    {
#if(PROJECT_ID == ODIN_PROJECT)
        pPart->iImage1Width = 28;
        pPart->iImage1Pos = 0;
        iWidth -= 28;
        pPart->iStringPos = 28;
        pPart->nCount ++;
#elif( (PROJECT_ID == WHALE1_PROJECT )||(PROJECT_ID == WHALE2_PROJECT)||(PROJECT_ID == NEMO_PROJECT))
        pPart->iImage1Width = 30;
        pPart->iImage1Pos = 0;
        iWidth -= 30;
        pPart->iStringPos = 30;
        pPart->nCount ++;
#elif (PROJECT_ID == NEMO_PROJECT)
        pPart->iImage1Width = 38;
        pPart->iImage1Pos = 0;
        iWidth -= 38;
        pPart->iStringPos = 38;
        pPart->nCount ++;
#elif (PROJECT_ID == DRAGON_S_PROJECT)        
        pPart->iImage1Width = 28;
        pPart->iImage1Pos = 1;
        iWidth -= 29;
        pPart->iStringPos = 29;
        pPart->nCount ++;
#elif  (PROJECT_ID == DRAGON_M_PROJECT)           
        pPart->iImage1Width = 15;
        pPart->iImage1Pos = -1;
        iWidth -= 23;
        pPart->iStringPos = 23;
        pPart->nCount ++;
#elif (PROJECT_ID == KNIGHT_PROJECT)        
        pPart->iImage1Width = 15;
        pPart->iImage1Pos = 2;
        iWidth -= 18;
        pPart->iStringPos = 18;
        pPart->nCount ++;         
#elif (PROJECT_ID == LOTUS_PROJECT)        
        pPart->iImage1Width = 15;
        pPart->iImage1Pos = 9;
        iWidth -= 31;
        pPart->iStringPos = 31;
        pPart->nCount ++;       
#elif (PROJECT_ID == CF810_PROJECT)
        pPart->iImage1Width = 15;
        pPart->iImage1Pos = 2;
        iWidth -= 23;
        pPart->iStringPos = 23;
        pPart->nCount ++; 
#elif (PROJECT_ID == VIOLET1_PROJECT)  
        pPart->iImage1Width = 15;
        pPart->iImage1Pos = 7;
        iWidth -= 25;
        pPart->iStringPos = 25;
        pPart->nCount ++; 
#else        
        pPart->iImage1Width = CHOICE_LINE_HEIGHT;
        pPart->iImage1Pos = 0;
        iWidth -= CHOICE_LINE_HEIGHT;
        pPart->iStringPos = CHOICE_LINE_HEIGHT;
        pPart->nCount ++;
#endif

        /* There has a icon , image1 or image2  to draw  */
        if( pSelectGroup->choice.bitmask & (MSF_CHOICE_ELEMENT_ICON |MSF_CHOICE_ELEMENT_IMAGE_1 | MSF_CHOICE_ELEMENT_IMAGE_2 ) )
        {
            if( iWidth <  CHOICE_LINE_HEIGHT ) 
            {
                /* 
                  * width not enough to show a image or icon, so only show item string  
                  */
                pPart->iStringWidth = iWidth;
            }
            else
            {
                pPart->iImage2Width = CHOICE_LINE_HEIGHT - 2;
#if ( (PROJECT_ID == DRAGON_M_PROJECT)||(PROJECT_ID == WHALE1_PROJECT )||(PROJECT_ID == WHALE2_PROJECT)||(PROJECT_ID == NEMO_PROJECT)||(PROJECT_ID == LOTUS_PROJECT))
                pPart->iImage2Pos = pPart->iStringPos + iWidth - pPart->iImage2Width - 18;
                pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT - 15;
#elif (PROJECT_ID == VIOLET1_PROJECT)  
                pPart->iImage2Pos = pPart->iStringPos + iWidth - pPart->iImage2Width - 13;
                pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT - 10;
#else
                pPart->iImage2Pos = pPart->iStringPos + iWidth - pPart->iImage2Width + 2;
                pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT + 2;
#endif                
                pPart->nCount++;
            }
        }
        else
        {
#if (PROJECT_ID == DRAGON_M_PROJECT)||(PROJECT_ID == VIOLET1_PROJECT)      
            pPart->iStringWidth = iWidth-14 ;
#else
            pPart->iStringWidth = iWidth ;
#endif            
        }
    }

    /* There has a icon to draw  */
    else if( pSelectGroup->choice.bitmask &( MSF_CHOICE_ELEMENT_ICON))
    {
#if (PROJECT_ID == B1_PROJECT)
        if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
        {
            pPart->iImage1Width = GADGET_COLUMN_WIDTH;
            pPart->iImage1Pos = 0;
            pPart->iStringWidth = iWidth - GADGET_COLUMN_WIDTH * 2;
            pPart->iStringPos = CHOICE_LINE_HEIGHT;
            pPart->nCount ++;
        }
        else
        {
            pPart->iImage1Width = CHOICE_LINE_HEIGHT;
            pPart->iImage1Pos = 0;
            pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT -2 ;
            pPart->iStringPos = CHOICE_LINE_HEIGHT + 2;
            pPart->nCount ++;
        }
#else
        pPart->iImage1Width = CHOICE_LINE_HEIGHT;
        pPart->iImage1Pos = 0;
        pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT -2 ;
        pPart->iStringPos = CHOICE_LINE_HEIGHT + 2;
        pPart->nCount ++;
#endif        
    }
    /*There is a image to draw*/
    else if(pSelectGroup->choice.bitmask &( MSF_CHOICE_ELEMENT_ICON |MSF_CHOICE_ELEMENT_IMAGE_1 | MSF_CHOICE_ELEMENT_IMAGE_2 ) ) 
    {
#if( PROJECT_ID == B1_PROJECT) 
                pPart->iImage1Width = CHOICE_LINE_HEIGHT;
                pPart->iImage1Pos = 0;
                pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT -2 ;
                pPart->iStringPos = CHOICE_LINE_HEIGHT + 2;
                pPart->nCount ++;
#elif( PROJECT_ID == PEGASUS_PROJECT) 
                if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
                {
                    pPart->iImage1Width = CHOICE_MENU_LINE_HEIGHT - 4;
                    pPart->iImage1Pos = 0;
                    pPart->iStringWidth = iWidth - CHOICE_MENU_LINE_HEIGHT + 2;
                    pPart->iStringPos = CHOICE_MENU_LINE_HEIGHT - 2;
                    pPart->nCount ++;
                }
                else
                {
                    pPart->iImage1Width = CHOICE_LINE_HEIGHT + 3;
                    pPart->iImage1Pos = 0;
                    pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT -2 - 3 ;
                    pPart->iStringPos = CHOICE_LINE_HEIGHT + 2 +3;
                    pPart->nCount ++;
                }
#else
                if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
                {
                    pPart->iImage1Width = CHOICE_MENU_LINE_HEIGHT - 4;
                    pPart->iImage1Pos = 0;
                    pPart->iStringWidth = iWidth - CHOICE_MENU_LINE_HEIGHT + 2;
                    pPart->iStringPos = CHOICE_MENU_LINE_HEIGHT - 2;
                    pPart->nCount ++;
                }
                else
                {
                    pPart->iImage1Width = CHOICE_LINE_HEIGHT;
                    pPart->iImage1Pos = 0;
                    pPart->iStringWidth = iWidth - CHOICE_LINE_HEIGHT -2 ;
                    pPart->iStringPos = CHOICE_LINE_HEIGHT + 2;
                    pPart->nCount ++;
                }
#endif
    }
    /* There has only one string to draw   */
    else
    {
#if  (PROJECT_ID == B1_PROJECT)
        if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
        {
            pPart->iStringPos = MENU_LEFT_BORDER;
            pPart->iStringWidth = iWidth - MENU_LEFT_BORDER * 2;
        }
        else

#endif        
        {
            pPart->iStringWidth = iWidth;
            pPart->iStringPos = 0;
        }
    }
    
    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetDrawInputOutline( MsfWindow *pWin, MsfPosition *pPos,  MsfSize *pSize, DS_COLOR bgColor  )
{
    if( !pWin||!pSize ||!pPos )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    widgetDrawReliefRect( pWin, pPos , pSize, OP_TRUE, OP_TRUE );
    
#if 0
    pPos->x += 2;
    pPos->y += 2;
    pSize->height -= 4;
    pSize->width -= 4;
#else
    pPos->x += 1;
    pPos->y += 1;
    pSize->height -= 2;
    pSize->width -= 2;
#endif
    
    widgetScreenFillRect( pWin,
                                          pPos->x,
                                          pPos->y, 
                                          (OP_INT16)(pPos->x + pSize->width-1), 
                                          (OP_INT16)(pPos->y + pSize->height -1), 
                                          bgColor);      
    pPos->x += 1;
    pPos->y += 1;
    pSize->height -= 2;
    pSize->width -= 2;

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetDrawTextInputCaret
( 
    const MsfTextInput      *pInput,
    OP_BOOLEAN            bRefresh
)
{
    OP_INT16         iXCaret;
    OP_INT16         iYCaret;
    OP_INT16         iCaretHeight;
    OP_INT16         iEngCharWidth, iEngCharHeight;

    DS_FONTATTR   fontAttr;
    MsfPosition       pos;
    MsfSize             size;
    MsfWindow         *pWin;

    if( !pInput )
    {
        return  TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin = pInput->gadgetData.parent ;
    if( !pWin )
    {
        return  TPI_WIDGET_ERROR_UNEXPECTED;
    }

    
    if (pInput->gadgetData.isFocused == OP_FALSE)
    {
        OPUS_Stop_Timer(OPUS_TIMER_TXT_INPUT_CURSOR_BLINK);
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    if (pInput->input.txtInputBufMng.cursor_y >= pInput->input.txtInputBufMng.rows )
    {
        if( pInput->input.txtInputBufMng.rows != 0 || pInput->input.txtInputBufMng.cursor_y != pInput->input.txtInputBufMng.rows )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    if( pInput->input.txtInputBufMng.cursor_y < 0)
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    widgetGetDrawingInfo( 
                        (void*) pInput, 
                        getWidgetType( pInput->gadgetData.gadgetHandle), 
                        pInput->gadgetData.gadgetHandle, 
                        &fontAttr, 
                        OP_NULL, 
                        OP_NULL );

    if( 0 > widgetGetFontSizeInEngChar(fontAttr , &iEngCharWidth, &iEngCharHeight))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    widgetTextInputGetEditArea( pInput, &pos, &size );

    if( pInput->input.txtInputBufMng.cursor_x  == 0 )
    {
        iXCaret = pos.x-1;
    }
    else
    {
        iXCaret = pos.x + pInput->input.txtInputBufMng.cursor_x * iEngCharWidth -1;
    }

    iYCaret = pos.y + pInput->input.txtInputBufMng.cursor_y * GADGET_LINE_HEIGHT;

    if( (iXCaret < (pos.x-1) || iXCaret >= pos.x + size.width)
      || (iYCaret < pos.y || iYCaret >= pos.y + size.height))
    {
        return TPI_WIDGET_OK;
    }
    
    /*  draw Caret  */
    if( iYCaret + GADGET_LINE_HEIGHT  > pos.y + size.height )
    {
        iCaretHeight = pos.y + size.height -iYCaret ;
    }
    else
    {
        iCaretHeight = GADGET_LINE_HEIGHT;
    }
    
    widgetScreenDrawCaret( pWin, iXCaret, iYCaret, iCaretHeight);

    if( bRefresh == OP_TRUE)
    {
        WAP_REFRESH
    }

    return TPI_WIDGET_OK;
}

#if 0
OP_BOOLEAN widgetGetHilightColor( DS_COLOR *pBorderColor, DS_COLOR *pFillColor )
{
    OP_BOOLEAN                         draw_focus_bar = OP_TRUE;
    DS_COLORTHEME_ENUM_T    color_theme = ds.cComposer.titleColor;
    
    switch(color_theme)
    {
        case COLOR_THEME_1:
        {
            if( pBorderColor )
            {
                *pBorderColor = COLOR_MANGANESE_BLUE;
            }

            if( pFillColor )
            {
                *pFillColor = COLOR_WHITE_MANGANESE_BLUE;
            }
            
            break;
        }    
        case COLOR_THEME_2:
        {
            if( pBorderColor )
            {
                *pBorderColor = COLOR_VIOLET_PINK;
            }

            if( pFillColor )
            {
                *pFillColor = COLOR_WHITE_PINK;
            }
            
            break;
        }    
        case COLOR_THEME_3:
        {
            if( pBorderColor )
            {
                *pBorderColor = COLOR_DARK_VIOLET;
            }

            if( pFillColor )
            {
                *pFillColor = COLOR_COBALT_VIOLET_TINT;
            }

            break;
        }    
        default :
        {
            draw_focus_bar = OP_FALSE;
            break;
        }    
    }

    return draw_focus_bar;
}
#endif

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetDrawHilight
(  
    MsfWindow *pWin, 
    MsfPosition  *pPos,  
    MsfSize *pSize, 
    OP_BOOLEAN bFullRect , 
    OP_BOOLEAN bOnlyBorder
)
{
//    DS_COLOR     border_color;
//    DS_COLOR     fill_color;
    OP_INT16       left;
    OP_INT16       top;
    OP_INT16       right;
    OP_INT16       bottom;
//    DS_COLOR focusBar =0;
    DS_COLOREDCOMPOSER_T choiceCompColor;

    if( !pWin ||!pPos || !pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    op_memset(&choiceCompColor, 0, sizeof(choiceCompColor));
    widgetGetComposedColor(&choiceCompColor);
    {
        left = (OP_INT16)pPos->x;
        top = (OP_INT16)pPos ->y;
        right = (OP_INT16)(pPos->x + pSize->width -1);
        bottom = (OP_INT16)(pPos->y + pSize->height-1);
#if 0        
        /* top line */                                          
        widgetScreenDrawLine( pWin, left, top , right, top , focusBar );
        /* bottom line   */
        widgetScreenDrawLine( pWin, right, bottom, left, bottom, focusBar );

        if( bFullRect == OP_TRUE )
        {
            /* right line  */
            widgetScreenDrawLine( pWin, right, top , right, bottom, focusBar );
            /* left line  */
            widgetScreenDrawLine( pWin, left, bottom, left, top , focusBar );

            left ++;
            right --;
        }

        if( bOnlyBorder == OP_FALSE )
        {
            top ++;
            bottom --;  
        }
#else        
        /********* temp use ***********/
        //choiceCompColor.focusbarColor = ds_get_focus_bar_color();
        /****************************/
        widgetScreenFillRect( pWin, left, top, right, bottom,  ds_get_focus_bar_color());
#endif            
    }  
   

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetInputGetEngCharSize
( 
    OP_UINT32       hText,
    const MsfSize   *pInputSize, 
    OP_INT16        *pEngCharsPerLine,
    OP_INT16        *pAdjustEngCharsPerLine,
    OP_INT16        *pLines 
)
{
    MsfTextInput  *pTextInput;
    MsfWidgetType wt;
    OP_INT16    iEngCharWidth, iEngCharHeight;
    DS_FONTATTR  fontAttr;
    int         iRet;
    
    if(  !pInputSize || !pEngCharsPerLine || !pLines )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pTextInput = (MsfTextInput*)seekWidget(hText, &wt);
    if( !pTextInput || wt!= MSF_TEXTINPUT )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    iRet = widgetGetDrawingInfo((void*)pTextInput,
                                                MSF_TEXTINPUT,
                                                hText,
                                                &fontAttr,
                                                OP_NULL,
                                                OP_NULL);
    if( iRet < 0 )
    {
        return iRet;
    }
    
    /* get font size */
    if( 0> widgetGetFontSizeInEngChar(fontAttr, &iEngCharWidth, &iEngCharHeight ))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    *pAdjustEngCharsPerLine = (OP_INT16)(( pInputSize->width - 4 - SCROLLBAR_WIDTH )/ iEngCharWidth); 
    if(pTextInput->gadgetData.propertyMask & MSF_GADGET_PROPERTY_ALWAYS_SHOW_SCROLL_BAR)
    {
         *pEngCharsPerLine = *pAdjustEngCharsPerLine;
    }
    else
    {
        *pEngCharsPerLine = (OP_INT16)(( pInputSize->width -4 )/ iEngCharWidth);
    }
    *pLines = (OP_INT16)(( pInputSize->height -4 )/ GADGET_LINE_HEIGHT );

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  drawWindowTitle

    DESCRIPTION:
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetWindowDrawTitle(MsfWindow *pWin, OP_UINT8* pDefaultTitle, OP_BOOLEAN bRefresh )
{
    OP_UINT8      *pChar;
    
    if( !pWin )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;        
    }
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pWin->windowHandle, OP_TRUE);
    }

    if( pWin->title == 0 )
    {
        if( pDefaultTitle )
        {
            ds_draw_title( pDefaultTitle , widgetWindowGetTitleType( pWin ) );
        }
    }
    else
    {
        WIDGET_GET_STRING_DATA(pChar, pWin->title);

        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        if( pChar[0] != 0x00 ||pChar[1] != 0x00 ) /*  not empty string  */
        {
#ifdef IMM_SHOW_IME_NAME_ON_TITLE
            if( pWin->parent && pWin->parent->pImm )
            {
                OP_UINT8       strData[80] = {0};
                int i,j;

                /*add blank before the title*/
                for(i=0;i < BLANK_NUMBER_BEFORE_TITLE; i++)
                {
                    j = 2*i;
                    strData[j] = 0x20;
                    strData[j +1] = 0x00;
                }
                
                /*copy the real title*/
                Ustrcat(strData, pChar );
                
                /*draw title*/
                ds_draw_title( strData, widgetWindowGetTitleType(pWin) );
            }
            else
            {
                ds_draw_title( pChar, widgetWindowGetTitleType(pWin) );
            }
#elif (PROJECT_ID == DRAGON_M_PROJECT)

            ds_draw_title( pChar, TITLE_SECOND_MENU );
#else
            ds_draw_title( pChar, widgetWindowGetTitleType(pWin) );
#endif
        }
        else if(pDefaultTitle )
        {
            ds_draw_title( pDefaultTitle , widgetWindowGetTitleType( pWin ) );
        }
    }
#ifdef IMM_SHOW_IME_NAME_ON_TITLE
    if( pWin->parent && pWin->parent->pImm )
    {
        IMM_draw_ime_icon(pWin->parent->pImm->hImm);
    }
#endif

    if( bRefresh == OP_TRUE )
    {
        LCD_RECT_T rect;
        rect.left = 0;
        rect.right = DS_SCRN_MAX_X-1;
        rect.top = 0;
        rect.bottom = WINDOW_TITLE_HEIGHT;
        ds_refresh_text_rectangle(rect);
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetWindowDrawBorder

    DESCRIPTION:
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetWindowDrawBorder(MsfWindow *pWin, OP_BOOLEAN bRefresh )
{
    OP_INT16    pt1_x;
    OP_INT16    pt1_y;
    OP_INT16    pt2_x;
    OP_INT16    pt2_y;
    DS_COLOR   whiteColor, lightColor, midColor, darkColor;
    
    if( !pWin )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;        
    }

    if(  !IF_SHOW_BORDER(pWin))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pt1_x = pWin->position.x ;
    pt1_y = pWin->position.y ;
    pt2_x = pt1_x + pWin->size.width -1;
    pt2_y = pt1_y + pWin->size.height - 1;

    widgetGetShodowColors( 
                &whiteColor, 
                &lightColor, 
                &midColor, 
                &darkColor,
                (DS_COLOR)_RGB(DEFAULT_BG_COLORS.r, DEFAULT_BG_COLORS.g, DEFAULT_BG_COLORS.b));

    /* left line   */
    widgetScreenNCDrawLine(pWin, 
                                                 pt1_x, 
                                                 pt1_y, 
                                                 pt1_x, 
                                                 pt2_y, 
                                                 lightColor);
                                                 
    widgetScreenNCDrawLine(pWin, 
                                                 (OP_UINT16)(pt1_x +1 ), 
                                                 pt1_y, 
                                                 (OP_UINT16)(pt1_x +1 ), 
                                                 pt2_y, 
                                                 whiteColor );

    /* right  line   */
    widgetScreenNCDrawLine(pWin, 
                                                 pt2_x, 
                                                 pt1_y, 
                                                 pt2_x, 
                                                 pt2_y, 
                                                 darkColor);
                                                 
    widgetScreenNCDrawLine(pWin, 
                                                 (OP_UINT16)(pt2_x -1 ), 
                                                 pt1_y, 
                                                 (OP_UINT16)(pt2_x -1 ), 
                                                 pt2_y, 
                                                 midColor );

    /* top line  */                                                 
    widgetScreenNCDrawLine(pWin, 
                                                 pt1_x, 
                                                 pt1_y, 
                                                 (OP_UINT16)(pt2_x-1), 
                                                 pt1_y, 
                                                 lightColor);
    widgetScreenNCDrawLine(pWin, 
                                                 (OP_UINT16)(pt1_x +1 ), 
                                                 (OP_UINT16)(pt1_y +1 ), 
                                                 (OP_UINT16)(pt2_x-2), 
                                                 (OP_UINT16)(pt1_y +1 ), 
                                                 whiteColor );

    /* bottom  line   */
    widgetScreenNCDrawLine(pWin, 
                                                 pt1_x, 
                                                 pt2_y, 
                                                 pt2_x, 
                                                 pt2_y, 
                                                 darkColor);
                                                 
    widgetScreenNCDrawLine(pWin, 
                                                 (OP_UINT16)(pt1_x + 1 ), 
                                                 (OP_UINT16)(pt2_y -1), 
                                                 (OP_UINT16)( pt2_x -1 ), 
                                                 (OP_UINT16)(pt2_y -1), 
                                                 midColor );
    if( bRefresh == OP_TRUE )
    {
        WAP_REFRESH
    }

    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  drawWindowAllGadgets

    DESCRIPTION:
        Draw all gadgets of the window specified by pWin.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetWindowDrawAllGadgets(MsfWindow *pWin)
{
    MsfWidgetType       widgetType;
    MsfGadget             *pGadget;
    GadgetsInWindow      *pGadgetNode;
    widget_show_func_t  drawFunc;
    
    if( !pWin )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;        
    }

    pGadgetNode = pWin->children;
    if( !pGadgetNode )    /*no gadget in the window */
    {
        return TPI_WIDGET_OK;
    }

    /*begin to  draw from the last gadget, i.e. lowest Z-order */
    while( pGadgetNode->next )
    {
        pGadgetNode = pGadgetNode->next;
    }
    
    /* Draw gadgets without ontop property  */
    while( pGadgetNode )
    {
        if( pGadgetNode->gadget )
        {
            pGadget = (MsfGadget*)pGadgetNode->gadget;
            if( !(pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP ) )
            {
                widgetType = (int)getWidgetType(pGadget->gadgetHandle);
                if( widgetType >= 0 && widgetType < MSF_MAX_WIDGETTYPE )
                {
                    drawFunc = widgetShowFunc[widgetType];

                    if( drawFunc )
                    {
                        (void)(drawFunc)( (void*)pGadget, widgetType );
                    }
                }
            }
        }

        pGadgetNode = pGadgetNode->prev;
    }
    
    /* Draw ontop gadgets, seek to the last gadget  */
    pGadgetNode = pWin->children;
    while( pGadgetNode->next )
    {
        pGadgetNode = pGadgetNode->next;
    }
    while( pGadgetNode )
    {
        if( pGadgetNode->gadget )
        {
            pGadget = (MsfGadget*)pGadgetNode->gadget;
            if( pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP )
            {
                widgetType = (int)getWidgetType(pGadget->gadgetHandle);
                if( widgetType >= 0 && widgetType < MSF_MAX_WIDGETTYPE )
                {
                    drawFunc = widgetShowFunc[widgetType];

                    if( drawFunc )
                    {
                        (void)(drawFunc)( (void*)pGadget, widgetType );
                    }
                }
            }
        }
        
        pGadgetNode = pGadgetNode->prev;
    }

    return TPI_WIDGET_OK;

}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetGetDateDesc( MsfDate* pDate, OP_UINT8* pBuf)
{
    int                iTmp;
    OP_UINT8 *pStr;
    RM_LANGUAGE_T   lang;

    if( !pDate || !pBuf )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pStr = pBuf;
    op_memset( pStr, 0, 22 );
    
    lang = RM_GetCurrentLanguage();
    
    /* handle year  */
    iTmp = pDate->year % 10000;
    *pStr = (OP_UINT8) (iTmp / 1000 + 0x30);
    pStr += 2;
    
    iTmp %= 1000;
    *pStr =(OP_UINT8) (iTmp / 100 + 0x30);
    pStr += 2;

    iTmp %= 100;
    *pStr =(OP_UINT8) (iTmp / 10 + 0x30);
    pStr += 2;

    *pStr =(OP_UINT8) (iTmp % 10 + 0x30);
    pStr += 2;

    /*add year char */
    if( lang == RM_LANG_ENG )
    {
        *pStr = 0x79;     /* 'y ' */
        pStr += 2;
    }
    else        /*regard as chinese */
    {
        *pStr = 0x74;     /* '��' */
        pStr++;
        *pStr = 0x5E;
        pStr++;
    }

    /* handle month  */
    iTmp = pDate->month% 12;
    *pStr =(OP_UINT8) (iTmp / 10 + 0x30);
    pStr += 2;

    *pStr =(OP_UINT8) (iTmp % 10 + 0x30);
    pStr += 2;

    /*add month  char */
    if( lang == RM_LANG_ENG )
    {
        *pStr = 0x6D;     /* 'm' */
        pStr += 2;
    }
    else        /*regard as chinese */
    {
        *pStr = 0x08;     /* '��' */
        pStr++;
        *pStr = 0x67;
        pStr++;
    }
    
    /* handle day  */
    iTmp = pDate->day % 31;
    *pStr =(OP_UINT8) (iTmp / 10 + 0x30);
    pStr += 2;

    *pStr =(OP_UINT8) (iTmp % 10 + 0x30);
    pStr += 2;

    /*add day  char */
    if( lang == RM_LANG_ENG )
    {
        *pStr = 0x64;     /* 'd' */
        pStr += 2;
    }
    else        /*regard as chinese */
    {
        *pStr = 0xE5;     /* '��' */
        pStr++;
        *pStr = 0x65;
        pStr++;
    }

    return  (pStr - pBuf );
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widetGetTimeDesc(MsfTime *pTime, OP_UINT8 *pBuf )
{
    int                iTmp;
    OP_UINT8 *pStr;
    
    if( !pTime || !pBuf )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pStr = pBuf;

    op_memset( pStr, 0, 18 );
    
    /* handle hours   */
    iTmp = pTime->hours% 24;
    
    *pStr = (OP_UINT8) (iTmp / 10 + 0x30);  /* ten base */
    pStr += 2;

    *pStr = (OP_UINT8) (iTmp % 10 + 0x30); 
    pStr += 2;
    
    *pStr = 0x3A;       /* symbol ":"  */
    pStr += 2;

    /* handle minutes   */
    iTmp = pTime->minutes % 60;
    
    *pStr = (OP_UINT8) (iTmp / 10 + 0x30);  /* ten base */
    pStr += 2;

    *pStr = (OP_UINT8) (iTmp % 10 + 0x30); 
    pStr += 2;
    
    *pStr = 0x3A;       /* symbol ":"  */
    pStr += 2;

    /* handle seconds   */
    iTmp = pTime->seconds % 60;
    
    *pStr = (OP_UINT8) (iTmp / 10 + 0x30);  /* ten base */
    pStr += 2;

    *pStr = (OP_UINT8) (iTmp % 10 + 0x30); 
    pStr += 2;

    return (pStr-pBuf);
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetDateTimeGetDesc( MsfDateTimeStruct *pDatetime, OP_UINT8 *pBuf )
{
    int                iLen;
    OP_UINT8 *pStr;

    if( !pDatetime || !pBuf )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pStr = pBuf;
    
    switch( pDatetime->mode )
    {
        case MsfTimeType:
           {
               iLen = widetGetTimeDesc( &pDatetime->time,  pStr);
               break;
           }
        case MsfDateType:
           {
               iLen = widgetGetDateDesc( &pDatetime->date,  pStr);            
               break;
           }
        case MsfDateTime:
           {
               iLen = widgetGetDateDesc( &pDatetime->date,  pStr);      
               pStr += iLen;
   
               /* insert symbol ' '   */
               *pStr = 0x20;
               pStr++;
               *pStr = 0x00;
               pStr++;
               
               iLen = widetGetTimeDesc( &pDatetime->time, pStr );
               pStr += iLen;
               
               break;
           }
        default :
           {
               break;
           }
    }

    *pStr = *(pStr+1) = 0x00;
    
    return pStr - pBuf;  /* string length */
}



/*==================================================================================================
    FUNCTION:  getInputTextAttr

    DESCRIPTION:
        Get the attributes of the Specified MsfInput, include the input string, font attribute, foregroundcolor and backgroundcolor.
        
        Notes: Before enter  this funcion,  the pBuf must be allocated enough memory, if pBuf is not OP_NULL.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetInputGetTextAttr
( 
    MsfInput *pInput, 
    OP_UINT8 *pBuf, 
    OP_INT16  iBufSize,
    OP_INT16  *pInitStringLen,
    DS_FONTATTR *pFontattr, 
    DS_COLOR  *pFontcolor, 
    DS_COLOR *pBackcolor
)
{
    if( !pInput )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pBuf )
    {
        getInputTextStringToBuffer(pInput,OP_TRUE, OP_FALSE, pBuf, iBufSize, (OP_UINT16*)pInitStringLen );
        
        if( MsfPasswordText ==  pInput->textType || MsfPasswordNumber == pInput->textType)
        {
            /* fill pBuf with "*"   */
            pBuf += *pInitStringLen;
            while( *pBuf != 0x00 || *(pBuf+1) != 0x00 )
            {
                *pBuf= 0x2A;
                *(pBuf+1) = 0x00;

                pBuf += 2;
            }
        }
    }
    
    if( pFontattr ) 
    {  
        /* only consider the bold style and  font size */
       *pFontattr = FONT_NORMAL;
        if( pInput->font.weight > 5)
        {
            *pFontattr |= FONT_BOLD;
        }

        if( pInput->font.size < FONT_SIZE_SEPERATOR )
        {
            *pFontattr |= FONT_SIZE_SMALL;
        }
        else
        {
            *pFontattr |= FONT_SIZE_MED;
        }

        if( pInput->textProp.decoration & MSF_TEXT_DECORATION_UNDERLINE )
        {
            *pFontattr |= FONT_UNDERLINE;
        }

        if( pInput->textProp.decoration & MSF_TEXT_DECORATION_SHADOW )
        {
            *pFontattr |= FONT_SHADOW;
        }
    }

    if( pFontcolor )
    {
        *pFontcolor =(DS_COLOR) _RGB(pInput->color.r, pInput->color.g, pInput->color.b);
    }

    if( pBackcolor )
    {
        *pBackcolor = COLOR_FONTBG;
    }
    
    /* If the forecolor and backcolor is identical,  uses the default color */
    if( pFontcolor && pBackcolor && *pFontcolor == *pBackcolor )    
    {
        *pFontcolor = COLOR_FONTFG;
        *pBackcolor = COLOR_FONTBG;
    }        

    return TPI_WIDGET_OK;
}    

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetInputFillInitStringByChar
(
    MsfWindow *pWin, 
    MsfWidgetDrawString     *pDrawString,
    OP_INT16                     iInitStringLen
)
{
    const OP_UINT8  *pBuf;
    MsfSize             size;      
    OP_INT16         iYPos;
    OP_INT16         iXPos;
    int                     nbrOfCharacters; 
    int                     nbrOfInitialWhiteSpaces;
    int                     nShownChars;        /*  the chars already shown  */
    int                     iWidth;
    OP_INT16         iEngCharWidth, iEngCharHeight;

    if( !pWin || !pDrawString )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pBuf = pDrawString->pBuf;

    if( !pBuf ||(*pBuf == 0x00 && *(pBuf+1) == 0x00 ))
    {                                                 
        return TPI_WIDGET_OK;
    }

    iInitStringLen >>= 1;
    
    iXPos = (OP_INT16)pDrawString->pos.x;
    iYPos = (OP_INT16)pDrawString->pos.y;
    op_memcpy( &size, &pDrawString->size, sizeof(MsfSize));
    
    nbrOfCharacters = 1;
    nShownChars = 0;
    
    if( 0 > widgetGetFontSizeInEngChar(pDrawString->fontAttr, &iEngCharWidth, &iEngCharHeight))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    while( nbrOfCharacters != 0 && size.height >= iEngCharHeight  )
    {
        widgetStringGetVisibleByChar(
                                            pBuf , 
                                            &size, 
                                            iEngCharWidth, 
                                            pDrawString->bShowInitialWhiteSpaces, 
                                            &nbrOfCharacters, 
                                            &nbrOfInitialWhiteSpaces);
        if( nbrOfCharacters != 0 )
        {
            nShownChars += nbrOfCharacters + nbrOfInitialWhiteSpaces;
            pBuf += nbrOfInitialWhiteSpaces * 2;
            
            if( nShownChars  < iInitStringLen )  /*  initString need to occupy  multiline  */   
            {
                iWidth = size.width;
            }
            else
            {
                iWidth = iEngCharWidth * UstrCharCount( pBuf );
            }
            
            widgetScreenFillRect( 
                                             pWin, 
                                             iXPos, 
                                             iYPos, 
                                             (OP_INT16)( iXPos + iWidth -1 ), 
                                             (OP_INT16)( iYPos + GADGET_LINE_HEIGHT -1 ), 
                                             pDrawString->backcolor);
            
            pBuf += nbrOfCharacters * 2;   
            
            size.height -= GADGET_LINE_HEIGHT;
            iYPos += GADGET_LINE_HEIGHT;
        }
    }
    
    return nShownChars;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetGadgetDrawLabel( MsfGadget* pGadget , MsfPosition *pPos,  MsfSize*pSize)
{
    OP_UINT8            *pChar;
    MsfWidgetDrawString drawString;
    
    if( !pGadget )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( ISVALIDHANDLE( pGadget->title))
    {
        WIDGET_GET_STRING_DATA(pChar, pGadget->title);

        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        widgetGetDrawingInfo( (void*) pGadget, 
                              getWidgetType(pGadget->gadgetHandle), 
                              0, 
                              &drawString.fontAttr, 
                              &drawString.fontcolor, 
                              OP_NULL );
                                                
        drawString.pos.x = pPos->x;
        drawString.pos.y = pPos->y;
        drawString.size.width = pSize->width;
        drawString.size.height= pSize->height;
        drawString.pBuf = pChar;

        /* draw label with small  size 
        drawString.fontAttr &= ~FONT_SIZE_MED;
        drawString.fontAttr |= FONT_SIZE_SMALL;*/
        drawString.fontAttr |= FONT_OVERLAP;
        drawString.backcolor = COLOR_FONTBG;
        drawString.bShowInitialWhiteSpaces = OP_FALSE;
        
        widgetDrawStringByChar(pGadget->parent, &drawString);

        pPos->y += GADGET_LINE_HEIGHT;
        pSize->height -= GADGET_LINE_HEIGHT;
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetSelectGroupDrawFrame

    DESCRIPTION:
        Fill the gadget area, draw the label and border
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetSelectGroupDrawFrame
( 
    const MsfSelectgroup      *pSelectGroup
)
{
    MsfWindow      *pWin;
    MsfGadget      *pGadget;
    MsfSize        labelSize;
    OP_INT16       labelLen;
    int            nbrOfCharacters;
    int            nbrOfInitialWhiteSpaces;
    DS_FONTATTR    fontAttr;
    DS_COLOR       fontColor;
    DS_COLOR       bgColor;
    OP_UINT8       *pChar;
    int            iRet;
    int            i;   
    MsfPosition    pos;
    MsfSize        size,bgSize;

    OP_INT16       iEngCharWidth, iEngCharHeight;
    OP_INT16       yPos = 0;
    
    if( !pSelectGroup)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pGadget = (MsfGadget*)pSelectGroup;
    pWin = pGadget->parent;

    if ( !pWin )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y );
    size.width = pGadget->size.width;
    size.height= pGadget->size.height;
    
    /*  get the  background color of  selectgroup */
    widgetGetDrawingInfo((void*) pSelectGroup, 
                                     getWidgetType(pSelectGroup->gadgetData.gadgetHandle), 
                                     0, 
                                     &fontAttr, 
                                     &fontColor, 
                                     &bgColor);

#if 0
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MAIN_MENU )
    {
        /* fill background  */
        widgetGetImageSize(BMP_SUBMENULINE_BG, &bgSize);
        yPos = pos.y;
        for( i = 0 ; i < pSelectGroup->choice.elementsPerPage; i++ )
        {
            if( RM_IsItBitmap(BMP_SUBMENULINE_BG) )
            {
                widgetScreenDrawBitmapRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                                       pos.x,
                                                       yPos, 
                                                       bgSize.width,
                                                       bgSize.height,
                                                       BMP_SUBMENULINE_BG);
            }
            else if( RM_IsItIcon(BMP_SUB_MAINMENU_TITLE_BG_C1) )
            {
                widgetScreenDrawIconRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                                       pos.x,
                                                       yPos, 
                                                       bgSize.width,
                                                       bgSize.height,
                                                       BMP_SUBMENULINE_BG);            
            }

            yPos += CHOICE_MENU_LINE_HEIGHT;
            //widgetScreenDrawIcon( pWin, OP_INT16 x, OP_INT16 y, OP_INT16 width, OP_INT16 height, RM_ICON_T * pIcon)
        }
        return TPI_WIDGET_OK;
    }
#endif    
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
    {
#if(ODIN_PROJECT == PROJECT_ID)
        /*  fill the area of selectgroup with background color  */                                         
        widgetScreenFillRect( pWin, 
            pos.x, 
            pos.y, 
            (OP_INT16)(pos.x + size.width -1), 
            (OP_INT16)(pos.y + size.height-1), 
            bgColor);
#elif ((PROJECT_ID == WHALE1_PROJECT )||(PROJECT_ID == WHALE2_PROJECT) )
        /* fill background  */
        if( !pSelectGroup->gadgetData.hVScrollBar )
        {
        widgetScreenDrawBitmapRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                   pos.x,
                                   yPos, 
                                   size.width,
                                   size.height,
                                   MENU_BG_RES_WITH_COLOR_SCHEME);
        }else
        {
        widgetScreenDrawBitmapRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                   pos.x,
                                   yPos, 
                                   size.width,
                                   size.height,
                                   MENU_SHORT_BG_RES_WITH_COLOR_SCHEME);  
        }
        
        SHOW
#elif (PROJECT_ID == KNIGHT_PROJECT)||(PROJECT_ID == NEMO_PROJECT)
            widgetScreenDrawBitmapRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                   pos.x,
                                   yPos, 
                                   size.width,
                                   size.height,
                                   MENU_BG_RES_WITH_COLOR_SCHEME);
        
#else    
        RM_RESOURCE_ID_T  itemBgId;
        if( COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
        {
            if( pSelectGroup->gadgetData.hVScrollBar )
            {
                itemBgId = MENU_ITEM_BG_PIC_VS_SB;
            }
            else
            {
                itemBgId = MENU_ITEM_BG_PIC;
            }
        }
        else
        {
            if( pSelectGroup->gadgetData.hVScrollBar )
            {
                itemBgId = MENU_ITEM_BG_PIC_VS_SB_BY_USER;
            }
            else
            {
                itemBgId = MENU_ITEM_BG_PIC_BY_USER;
            }
        }
        
        /* fill background  */
        widgetGetImageSize(itemBgId, &bgSize);
        yPos = pos.y;
        for( i = 0 ; i < pSelectGroup->choice.elementsPerPage; i++ )
        {
            if( RM_IsItBitmap(itemBgId) )
            {
                widgetScreenDrawBitmapRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                                       pos.x,
                                                       yPos, 
                                                       (OP_INT16)((bgSize.width < pSelectGroup->itemSize.width)?(bgSize.width):(pSelectGroup->itemSize.width)),
                                                       (OP_INT16)((bgSize.height< pSelectGroup->itemSize.height)?(bgSize.height):(pSelectGroup->itemSize.height)),                                                      
                                                       itemBgId);
            }
            else if( RM_IsItIcon(itemBgId) )
            {
                widgetScreenDrawIconRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                                       pos.x,
                                                       yPos, 
                                                       (OP_INT16)((bgSize.width < pSelectGroup->itemSize.width)?(bgSize.width):(pSelectGroup->itemSize.width)),
                                                       (OP_INT16)((bgSize.height< pSelectGroup->itemSize.height)?(bgSize.height):(pSelectGroup->itemSize.height)),                                                      
                                                       itemBgId);            
            }

            yPos += CHOICE_MENU_LINE_HEIGHT;
        }      
#endif        
    }
    else
    {
        /*  fill the area of selectgroup with background color  */                                         
        widgetScreenFillRect( pWin, 
                                         pos.x, 
                                         pos.y, 
                                         (OP_INT16)(pos.x + size.width -1), 
                                         (OP_INT16)(pos.y + size.height-1), 
                                         bgColor);
    }

    /* if the showStyle is menu,  need not  draw the label and border  */
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU )
    {
        return TPI_WIDGET_OK;
    }
    
    /* If  have label, calc the label lenght by pixel */
    pChar = OP_NULL;                                     
    if( ISVALIDHANDLE( pSelectGroup->gadgetData.title))
    {
        WIDGET_GET_STRING_DATA(pChar, pSelectGroup->gadgetData.title);
            
        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        labelSize.width = (OP_INT16)(size.width - 16 );
        labelSize.height = (OP_INT16)(CHOICE_LINE_HEIGHT);
       
        if( 0 > widgetGetFontSizeInEngChar( fontAttr, &iEngCharWidth, &iEngCharHeight))
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        
        labelLen = (OP_INT16)widgetStringGetVisibleByChar( 
                                                pChar, 
                                                &labelSize, 
                                                iEngCharWidth, 
                                                OP_FALSE, 
                                                &nbrOfCharacters, 
                                                &nbrOfInitialWhiteSpaces);
    }
    else
    {
        labelLen = 0;
    }

    /*  need draw border  */
    if( !(pSelectGroup->showStyle & SG_SHOW_STYLE_NO_BORDER ))
    {
        /* widgetDrawSelectGroupBorder will adjust the labelLen if the label is too long  */
        iRet = widgetDrawSelectGroupBorder( pSelectGroup, &pos, &size, labelLen);
        if( iRet  < 0 )
        {
            return iRet;
        }
    
        labelLen = (OP_INT16) iRet;
    }

    /* draw  label  */
    if( pChar )    /* have label to draw */
    {
        MsfWidgetDrawString drawString;
    
        drawString.pBuf =  pChar;
        drawString.pos.x = (OP_INT16)(pos.x + 8);
        drawString.pos.y = (OP_INT16)(pos.y );
        drawString.size.width = (OP_INT16)labelLen;
        drawString.size.height = CHOICE_LINE_HEIGHT;
        
        drawString.fontcolor = fontColor;
        drawString.backcolor = COLOR_FONTBG;
        drawString.fontAttr = FONT_OVERLAP |fontAttr;
        drawString.bShowInitialWhiteSpaces = OP_FALSE;
        
        widgetDrawStringByChar( pWin, &drawString ); 
    }
        
    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:


    IMPORTANT NOTES:
        None
==================================================================================================*/
void  widgetAdjustWindowNodeVisible
( 
    WindowsInScreen         *pWindowNode,  
    MsfWindow                  *pWin, 
    OP_INT16                *pMinX,
    OP_INT16                *pMinY,
    OP_INT16                *pMaxX,
    OP_INT16                *pMaxY,
    OP_INT16                *pScreenFillState
)
{
    OP_INT16 iRight, iBottom;
    OP_BOOLEAN  bVisible ;

    bVisible = OP_FALSE;
    
    if( *pMinX > pWin->position.x )
    {
        if( *pMinX > 0 )
        {
            *pMinX = pWin->position.x;
            if( *pMinX < 0 )
            {
                *pMinX = 0;
            }
            
            bVisible = OP_TRUE;
        }
    }
    
    if( *pMinY > pWin->position.y )
    {
        if( *pMinY > 0 )
        {
            *pMinY = pWin->position.y;
            if( *pMinY < 0 )
            {
                *pMinY = 0;
            }
            
            bVisible = OP_TRUE;
        }
    }

    iRight = pWin->position.x + pWin->size.width -1;
    iBottom = pWin->position.y + pWin->size.height-1;
    
    if( *pMaxX < iRight )
    {
        if( *pMaxX < MAX_SCREEN_X )
        {
            *pMaxX = iRight;
            if( *pMaxX > MAX_SCREEN_X )
            {
                *pMaxX = MAX_SCREEN_X;
            }

            bVisible = OP_TRUE;
        }
    }
    
    if( *pMaxY < iBottom )
    {
        if( *pMaxY < MAX_SCREEN_Y )
        {
            *pMaxY = iBottom;
            if( *pMaxY > MAX_SCREEN_Y )
            {
                *pMaxY = MAX_SCREEN_Y;
            }

            bVisible = OP_TRUE;
        }
    }

    pWindowNode->bVisible = bVisible;
    
    if( *pMinX == 0 && *pMinY == 0 )
    {
        *pScreenFillState |= 0x0001;
    }

    if( (*pMaxX == MAX_SCREEN_X) && (*pMaxY == MAX_SCREEN_Y ))
    {
        *pScreenFillState |= 0x0002;
    }

}
/*==================================================================================================
    Show widget funcion definition
==================================================================================================*/

/*==================================================================================================
    FUNCTION:  widgetScreenShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetScreenShow( void *pWidget, MsfWidgetType wt )
 {
    MsfScreen           *pScreen;
    MsfWindow          *pWin;
    MsfWidgetType    widgetType;
    WindowsInScreen   *pWinNode;
    WindowsInScreen   *pPreNode;
    OP_INT16           minX, minY, maxX, maxY;
    OP_INT16           iScreenFillState;
    
    if( !pWidget || wt != MSF_SCREEN )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pScreen = (MsfScreen*)pWidget;

    if( pScreen->children == OP_NULL )
    {
        return TPI_WIDGET_OK;
    }
    
    if( pScreen->pImm && pScreen->pImm->bOpen == OP_TRUE )
    {
        minX = 0;
        minY = pScreen->pImm->start_y;
        maxX = MAX_SCREEN_X;
        maxY = MAX_SCREEN_Y;
        iScreenFillState = 0;
    }
    else
    {
        minX = MAX_SCREEN_X;
        minY = MAX_SCREEN_Y;
        maxX = 0;
        maxY = 0;
        iScreenFillState = 0;
    }
    
    pWinNode = pScreen->children;
    pPreNode = pWinNode;
    while( pWinNode && iScreenFillState != 0x0003 )
    {
        pWin = (MsfWindow*)pWinNode->window;
        if( !pWin )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        widgetAdjustWindowNodeVisible(
                                        pWinNode, 
                                        pWin, 
                                        &minX, 
                                        &minY, 
                                        &maxX,  
                                        &maxY, 
                                        &iScreenFillState);
        pPreNode = pWinNode;
        pWinNode = pWinNode->next;
    }

    while ( pPreNode)
    {
        if( pPreNode->bVisible == OP_TRUE )
        {
            pWin = (MsfWindow*)pPreNode->window;
            widgetType = getWidgetType( pWin->windowHandle);

            if( widgetType == MSF_PAINTBOX )
            {
                widgetPaintBoxRedraw(pWin, OP_FALSE );
            }
            else if( widgetShowFunc[widgetType] != OP_NULL )
            {
                pWin->bDrawing = OP_TRUE;
                (void)widgetShowFunc[widgetType]((void*)pWin, widgetType);
                pWin->bDrawing = OP_FALSE;
            }            
        }

        pPreNode = pPreNode->prev;
    }
    
    return TPI_WIDGET_PAINTED;
 }


/*==================================================================================================
    FUNCTION:  widgetPaintBoxShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetPaintBoxShow(void *pWidget, MsfWidgetType wt)
{
    MsfWindow             *pWin;
    MsfPaintbox           *pb;

    if( !pWidget ||wt != MSF_PAINTBOX)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    pWin = (MsfWindow*)pWidget;

    if( pWin->parent == OP_NULL || pWin->parent->isFocused == OP_FALSE)
    {
        return TPI_WIDGET_OK;
    }

    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        widgetScreenSetMode();
    }
    
    SHOW;

    widgetWindowShowSoftKey( pWin, OP_FALSE );
    SHOW

    pb = (MsfPaintbox*)pWin;
    if( pb->isHoldDraw == OP_FALSE )
    {
        WAP_REFRESH
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetFormShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetFormShow(void *pWidget, MsfWidgetType wt)
{
    if( !pWidget || wt != MSF_FORM)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    return widgetWindowShow(
                                (MsfWindow*)pWidget, 
                                MSF_FORM,
                                OP_TRUE,
                                widgetGetPredefinedString(MSF_STR_ID_WINDOW) ); /* Window */
}

/*==================================================================================================
    FUNCTION:  widgetDialogShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetDialogShow(void *pWidget, MsfWidgetType wt)
{
    MsfWindow        *pWin;
    MsfDialog        *pDlg;
    OP_UINT8         *pChar;
    MsfPosition      pos;
    MsfSize          size,strSize;
    int              iRet;
#ifndef    DIALOG_BG_ONLY_BACKGROUND_PICTURE
    RM_BMP_ID_T      dlgBmp;
#endif    
    
    if( !pWidget || wt != MSF_DIALOG)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pWin = (MsfWindow*)pWidget;
    pDlg = (MsfDialog*)pWin;

#ifndef    DIALOG_BG_ONLY_BACKGROUND_PICTURE
    /* draw the backgroud  */
    switch(pDlg->type)
    {
        case MsfAlert:
            dlgBmp = BMP_POPUP_WARNING;
            break;
        case MsfConfirmation:
            dlgBmp = BMP_POPUP_CONFIRM;
            break;
        case MsfWarning:
            dlgBmp = BMP_POPUP_WARNING;            
            break;
        case MsfInfo:
            dlgBmp = BMP_POPUP_INFORMATION;
            break;
        case MsfError:
            dlgBmp = BMP_POPUP_ERROR;
            break;
        case MsfPrompt:
            dlgBmp = BMP_POPUP_WARNING;
            break; 
        default:
            dlgBmp = BMP_POPUP_WARNING;
            break;             
    }
#endif    
    
#ifdef DIALOG_BG_ONLY_TYPE_PICTURE
    ds_draw_bitmap_image_rm_forward(
        (OP_INT16)(pWin->position.x ),
        (OP_INT16)(pWin->position.y ),
        dlgBmp);
#elif (defined DIALOG_BG_ONLY_BACKGROUND_PICTURE )
    ds_draw_bitmap_image_rm_forward(
                            (OP_INT16)pWin->position.x,
                            (OP_INT16)pWin->position.y,
                            BMP_POPUP_BACKGROUND);
#else
    ds_draw_bitmap_image_rm_forward(
                            (OP_INT16)pWin->position.x,
                            (OP_INT16)pWin->position.y,
                            BMP_POPUP_BACKGROUND);
#if  (PROJECT_ID == DRAGON_M_PROJECT)                                         
    ds_draw_bitmap_image_rm_forward(
        (OP_INT16)(pWin->position.x +14),
        (OP_INT16)(pWin->position.y +4),
         dlgBmp);
#else
 ds_draw_bitmap_image_rm_forward(
        (OP_INT16)(pWin->position.x +2),
        (OP_INT16)(pWin->position.y +2),
         dlgBmp);
#endif               
#endif
    /* get window client area  */
    widgetGetClientRect( pWin, &pos, &size);

    /*  draw the dialog prompt text  */
    if(ISVALIDHANDLE(pDlg->dialogText))
    {
        WIDGET_GET_STRING_DATA(pChar, pDlg->dialogText);

        if( pChar )
        {
            if( pDlg->type == MsfPrompt )
            {
                size.height = GADGET_LINE_HEIGHT;
            }
            else
            {
                strSize.width = size.width;
                TPIa_widgetStringGetRectSize(pDlg->dialogText, pDlg->windowData.windowHandle,&strSize);
                if(size.height > strSize.height)
                {
                    pos.y += (size.height - strSize.height)/2; 
                    /* adjust string at screen position according to string's length*/
                    if(size.width>strSize.width)
                        pos.x += (size.width - strSize.width)/2;
                }
            }
            
            widgetWindowDrawStringByChar(
                                        pWin, 
                                        pChar, 
                                        &pos, 
                                        &size );
        }  

        pos.y += (size.height + 2);
    }

    /* if Prompt Dialog, need adjust the textinput position  */
    if( (pDlg->type == MsfPrompt)
        && (pDlg->bNeedAdjustInputPos == OP_TRUE ))
    {
        widgetDialogAdjustInputPos( pDlg, &pos);
    }
    
    iRet = widgetWindowShow(
                                pWin, 
                                MSF_DIALOG, 
                                OP_FALSE,
                                OP_NULL ); /* Edit */

    if( pDlg->timeoutTime > 0 )
    {
        if(IS_WIDGET_TIMER_ID( pDlg->iTimerId ))
        {
            TPIa_widgetStopTimer( pWin->windowHandle, pDlg->iTimerId );
        }
        
        pDlg->iTimerId = TPIa_widgetStartTimer((MSF_INT32)pDlg->timeoutTime, MSF_FALSE, pWin->windowHandle, widgetDialogOnTimer);
    }
    
    return iRet;                                    
}


/*==================================================================================================
    FUNCTION:  WidgetMenuShow

    DESCRIPTION:
        Show the menu window specified by the hWin

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetMenuShow( void *pWidget, MsfWidgetType wt )
{
    if( !pWidget ||wt != MSF_MENU )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    #if  (PROJECT_ID == DRAGON_M_PROJECT)             
    widgetWindowShow(
                                (MsfWindow*)pWidget, 
                                MSF_MENU, 
                                OP_TRUE,
                                widgetGetPredefinedString(MSF_STR_ID_MENU) ); /* Menu */
    widgetScreenDrawBitmapRm( (MsfWindow*)pWidget, 0,0,128,4,BMP_SUBMENU_LINE_PADDING_NO_SCROLL);
    widgetScreenDrawBitmapRm( (MsfWindow*)pWidget, 0,79,128,4,BMP_SUBMENU_BOTTOM_PADDING_BG);                                
    return TPI_WIDGET_OK;
    #else
    return widgetWindowShow(
                                (MsfWindow*)pWidget, 
                                MSF_MENU, 
                                OP_TRUE,
                                widgetGetPredefinedString(MSF_STR_ID_MENU) ); /* Menu */
#endif                                
}

static int  widgetMainMenuShow( void *pWidget, MsfWidgetType wt )
{
    return 0;
#if 0

    MsfSize  size;
    MsfPosition pos;
    DS_FONTATTR        fontAttr;
    OP_INT16              iEngCharWidth, iEngCharHeight;
    int                        nbrOfCharacters;
    int                        nbrOfInitialWhiteSpaces; 
    OP_INT16              iLenByPixel;  
  
    if( !pWidget ||wt != MSF_MAINMENU )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    /* draw the background of the two buttons and the title  */
    widgetGetImageSize(MAIN_MENU_ITEM_BG_PIC, &size);
    if( RM_IsItBitmap(MAIN_MENU_ITEM_BG_PIC) )
    {
        widgetScreenDrawBitmapRm((MsfWindow *)pWidget, 
                                               0,
                                               TITLE_MAIN_SUBMENU_BASE_TOP, 
                                               size.width,
                                               size.height,
                                               MAIN_MENU_ITEM_BG_PIC);
    }
    else if( RM_IsItIcon(MAIN_MENU_ITEM_BG_PIC) )
    {
        widgetScreenDrawIconRm((MsfWindow *)pWidget, 
                                 0, 
                                 TITLE_MAIN_SUBMENU_BASE_TOP, 
                                 size.width,
                                 size.height,
                                 MAIN_MENU_ITEM_BG_PIC);
    }
    /* middle align of title */
    widgetGetDrawingInfo( (void*) pWidget, 
                                MSF_MAINMENU, 
                                0, 
                                &fontAttr, 
                                OP_NULL, 
                                OP_NULL );
    if( 0 > widgetGetFontSizeInEngChar(fontAttr, &iEngCharWidth, &iEngCharHeight))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    
    iLenByPixel = widgetStringGetVisibleByChar(widgetGetPredefinedString(BRA_STR_ID_START_TITLE),
                                               &size,
                                               iEngCharWidth,
                                               OP_FALSE,
                                               &nbrOfCharacters,
                                               &nbrOfInitialWhiteSpaces);
     if(size.width > iLenByPixel)
     {
         pos.x = (size.width - iLenByPixel)/2;
         pos.y = TITLE_MAIN_SUBMENU_BASE_TOP + CHOICE_MAIN_MENU_LINE_HEIGHT +4;
     }
     /* draw title */
     widgetWindowDrawStringByChar((MsfWindow *)pWidget,
                                 widgetGetPredefinedString(BRA_STR_ID_START_TITLE),
                                 &pos,
                                 &size);
                                 
                                 
    /* draw other gadgets */
    return widgetWindowShow(
                                (MsfWindow*)pWidget, 
                                MSF_MAINMENU, 
                                OP_FALSE,
                                widgetGetPredefinedString(MSF_STR_ID_MAIN_MENU) ); /* Main Menu */
#endif
}

/*==================================================================================================
    FUNCTION:  widgetViewerShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetViewerShow(void *pWidget, MsfWidgetType wt)
{
    if( !pWidget || wt != MSF_VIEWER )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    return widgetWindowShow(
                                (MsfWindow*)pWidget, 
                                MSF_VIEWER, 
                                OP_FALSE,
                                widgetGetPredefinedString(MSF_STR_ID_VIEWER) ); /* Viewer */
}

/*==================================================================================================
    FUNCTION:  widgetWaitingWinShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetWaitingWinShow( void *pWidget, MsfWidgetType wt )
{
    MsfWaitingWin   *pWaitingWin;
    RM_ICON_T       *pWaitingIcon;
    
    MsfSize         clientSize, promptSize;
    MsfPosition     clientPos, promptPos;
    OP_INT16        x, y;
    OP_INT16        w,h;
    int             iRet;
    
    if( !pWidget || wt != MSF_WAITINGWIN )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pWaitingWin = (MsfWaitingWin*)pWidget;
    iRet = widgetWindowShow(
                            (MsfWindow*)pWidget, 
                            MSF_WAITINGWIN, 
                            pWaitingWin->bShowBg,
                            OP_NULL );
                                
    /* need show the waiting flag */
    if( pWaitingWin->bProgressBar == OP_FALSE  && !ISVALIDHANDLE(pWaitingWin->hBmpAni ))
    {
        widgetGetClientRect((MsfWindow*)pWaitingWin, &clientPos, &clientSize);
        
        pWaitingIcon = util_get_icon_from_res( ICON_WAITING );
        if( pWaitingIcon )
        {
            w = pWaitingIcon->biWidth;
            h = pWaitingIcon->biHeight;
            if( pWaitingWin->bShowBg == OP_FALSE )
            {
                x = ( DS_SCRN_MAX_X - w ) >> 1;
                y = ( WAP_SCRN_TEXT_HIGH - h ) >> 1;
            }
            else
            {
                x = ( DS_SCRN_MAX_X - w ) >> 1;
                y = ( WAP_SCRN_TEXT_HIGH - h ) >> 1;
                if( ISVALIDHANDLE(pWaitingWin->hPrompt) )
                {
                    TPIa_widgetGetPosition(pWaitingWin->hPrompt, &promptPos);
                    if( pWaitingWin->bPromptIsUp )     /* the prompt string is up */
                    {
                        TPIa_widgetGetSize( pWaitingWin->hPrompt, &promptSize);
                        y = promptPos.y + promptSize.height + 4;
                    }
                    else          /* the prompt string is down */
                    {
                        y = promptPos.y - 4 - h;
                    }
                }
            }
            ds_draw_partial_icon( x, y, w, h, 0, 0, pWaitingIcon);
        }
    }

    return iRet;
}

/*==================================================================================================
    FUNCTION:  widgetEditorShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetEditorShow(void *pWidget, MsfWidgetType wt)
{
    if( !pWidget || wt != MSF_EDITOR )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    return widgetWindowShow(
                                (MsfWindow*)pWidget, 
                                MSF_EDITOR, 
                                OP_FALSE,
                                widgetGetPredefinedString(MSF_STR_ID_EDIT) ); /* Edit */
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetWindowShow
(
    MsfWindow*      pWin, 
    MsfWidgetType  widgetType,
    OP_BOOLEAN     bClearBg,
    OP_UINT8         *pTitle
)
{
    DS_COLOR        bgColor;

    if( !pWin || !IS_WINDOW(widgetType) )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pWin->parent == OP_NULL || pWin->parent->isFocused == OP_FALSE)
    {
        return TPI_WIDGET_ERROR_NON_FOCUS;
    }

    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        widgetScreenSetMode();
    }
    
    SHOW;
    
    if( bClearBg)
    {
        widgetGetDrawingInfo(
                                (void*) pWin, 
                                widgetType, 
                                0, 
                                OP_NULL, 
                                OP_NULL, 
                                &bgColor);

        widgetScreenFillWindowClientRect( pWin, bgColor);
        SHOW
    }
    
    if( widgetType != MSF_DIALOG  && widgetType != MSF_MAINMENU && widgetType != MSF_WAITINGWIN)
    {
        if( IF_SHOW_TITLE(pWin) )
        {
            widgetWindowDrawTitle( pWin, pTitle, OP_FALSE );
            SHOW
        }
        
        /* draw window's border */ 
        if( IF_SHOW_BORDER( pWin ))
        {
            widgetWindowDrawBorder(pWin, OP_FALSE );
            SHOW
        }
    }
    
    widgetWindowShowSoftKey( pWin, OP_FALSE );
    widgetWindowDrawAllGadgets (pWin);
    SHOW
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetSelectgroupShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetSelectgroupShow(void *pWidget, MsfWidgetType wt)
{
    MsfSelectgroup       *pSelectGroup;
    int                        iRet;
    
    if( !pWidget  || (wt != MSF_SELECTGROUP && wt != MSF_BOX))
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pSelectGroup = (MsfSelectgroup*)pWidget;
    
    /*Judge if the gadget parent  has focus  */
    if( pSelectGroup->gadgetData.parent == OP_NULL  )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }

    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pSelectGroup->gadgetData.parent->windowHandle, OP_TRUE);
    }
    

    /*  If not calc the item position info, the calc it  */
    if( pSelectGroup ->bItemPosCalculated == OP_FALSE )
    {
        iRet = widgetChoiceCalcItemArea(pSelectGroup);
        if( iRet < 0 )
        {
            return iRet;
        }
    }
    
    /* draw the frame of selectgroup  */
    widgetSelectGroupDrawFrame(pSelectGroup);
    
    /* draw all  items of the selectgroup   */
    return  widgetChoiceDrawItems( pSelectGroup );
}

/*==================================================================================================
    FUNCTION:  widgetDatetimeShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetDatetimeShow(void *pWidget, MsfWidgetType wt)
{
    MsfGadget           *pGadget;
    MsfDateTimeStruct  *pDatetime;
    OP_UINT8           datetime[50];
    MsfWidgetDrawString  drawString;
    
    if( !pWidget  || wt != MSF_DATETIME )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pGadget = (MsfGadget*)pWidget;

    /*Judge if the gadget parent  has focus  */
    if( pGadget->parent == OP_NULL )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pGadget->parent->windowHandle, OP_TRUE);
    }
    
    pDatetime = (MsfDateTimeStruct*)pGadget;

    widgetGadgetGetPosition( pGadget, &drawString.pos.x, &drawString.pos.y );
    drawString.size.width = pGadget->size.width;
    drawString.size.height= pGadget->size.height;
                                                  
    widgetGetDrawingInfo( (void*) pGadget, 
                                            MSF_DATETIME, 
                                            0, 
                                            &drawString.fontAttr, 
                                            &drawString.fontcolor, 
                                            OP_NULL );

    widgetDateTimeGetDesc( pDatetime, datetime );
    
    drawString.fontAttr |= FONT_OVERLAP;
    drawString.backcolor = COLOR_FONTBG;
    drawString.pBuf = (OP_UINT8*)datetime;
    drawString.bShowInitialWhiteSpaces = OP_FALSE;
    
    widgetDrawStringByChar( pGadget->parent, &drawString );
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetTextInputShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetTextInputShow(void *pWidget, MsfWidgetType wt)
{
    MsfTextInput        *pTextInput;
    MsfWindow         *pWin;
    MsfGadget        *pGadget;
    MsfInput           *pInput;
    OP_UINT8         *pBuf;
    MsfWidgetDrawString  drawString;
    OP_UINT8         buf[TEXT_INPUT_SIZE];
    OP_INT16         nBuf;
    OP_INT16         iInitStringLen;
    OP_BOOLEAN   bInFocus;   
    MsfPosition       pos;
    MsfSize             size;
    int                     iRet;
    
    if( !pWidget  || wt != MSF_TEXTINPUT )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    pGadget = (MsfGadget*)pWidget;

    /*Judge if the gadget parent  has focus  */
    if( pGadget->parent == OP_NULL )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }

    pWin = pGadget->parent;
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pWin->windowHandle, OP_TRUE);
    }

    pTextInput = (MsfTextInput*)pGadget;
    pInput = &pTextInput->input;
    
    iRet = TPI_WIDGET_OK;
    bInFocus = pGadget->isFocused;
    if( bInFocus )
    {
        OPUS_Stop_Timer( OPUS_TIMER_TXT_INPUT_CURSOR_BLINK );
    }    

    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y );
    size.width = pGadget->size.width;
    size.height= pGadget->size.height;

    widgetGadgetDrawLabel( pGadget, &pos, &size);
    SHOW
    
    if ( bInFocus )  /*   in focus  */
    {
        /* 
          * The  widgetDrawInputOutline will adjust the value of pos 
          * and size
          */
        widgetDrawInputOutline(
                                    pWin,
                                    &pos,
                                    &size,
                                    (DS_COLOR) DEFAULT_INPUT_FOCUS_BG_COLORS );

        SHOW
    }
    else
    {
        /* 
          * The  widgetDrawInputOutline will adjust the value of pos 
          * and size
          */
        widgetDrawInputOutline(
                                    pWin,  
                                    &pos, 
                                    &size, 
                                    (DS_COLOR)_RGB( DEFAULT_INPUT_BG_COLORS.r, 
                                                              DEFAULT_INPUT_BG_COLORS.g, 
                                                              DEFAULT_INPUT_BG_COLORS.b));
        SHOW
    }

    if( pInput->txtInputBufMng.rows <= 1 )
    {
        /* 
          *  Calc the buffer size : bufSize = charsPerLine * lines * 2  + 2  -- +2 for OP_NULL  
          *  If the buf not enough room , alloc momery 
          */
        nBuf = (size.width / 6) * ( size.height / GADGET_LINE_HEIGHT ) * 2 + 2;
        if(  nBuf  > TEXT_INPUT_SIZE  )
        {
            pBuf =(OP_UINT8 *)WIDGET_ALLOC( nBuf );
            if( !pBuf )
            {
                return TPI_WIDGET_ERROR_RESOURCE_LIMIT;
            }        
        }
        else
        {
            pBuf = buf;
        }
        
        widgetInputGetTextAttr(
                                pInput,
                                pBuf,
                                nBuf,
                                &iInitStringLen,
                                &drawString.fontAttr,
                                &drawString.fontcolor,
                                OP_NULL );

        drawString.pBuf = pBuf;
        drawString.pos.x = pos.x;
        drawString.pos.y = pos.y;
        drawString.size.width = size.width;
        drawString.size.height= size.height;
        drawString.bShowInitialWhiteSpaces = OP_TRUE;
        
#if 0
        if( iInitStringLen > 0 )
        {
            /*  save the InputString first char   */
            inputStringFirstChar[0] = pBuf[iInitStringLen];
            inputStringFirstChar[1] = pBuf[iInitStringLen+1];

            /*  add OP_NULL to the InitString  end  */
            pBuf[iInitStringLen] = 0x00;
            pBuf[iInitStringLen+1] = 0x00;

            /*  fill the InitString with light-gray  background   */
            drawString.fontAttr &= ~FONT_OVERLAP;
            drawString.backcolor = DEFAULT_INPUT_INITSTRING_BG_COLORS;
            
            widgetInputFillInitStringByChar(pWin, &drawString, iInitStringLen);
            SHOW
            
            /*  restore the InputString first char  */
            pBuf[iInitStringLen] = inputStringFirstChar[0];
            pBuf[iInitStringLen+1] = inputStringFirstChar[1];
        }
#endif

        drawString.fontAttr |= FONT_OVERLAP;
        drawString.backcolor = COLOR_FONTBG;
        
        widgetDrawStringByChar( pWin, &drawString );
        
        if(pBuf != buf )
        {
            WIDGET_FREE(pBuf );
        }
    }
    else
    {
        OP_INT16         iYPos;
        OP_INT16         iXPos;
        OP_INT16         iRow;
        OP_INT16         iLen;
        OP_UINT8        *pChar;
        OP_UINT8        idx, next;

        iXPos = pos.x;
        iYPos = pos.y;
        
        widgetInputGetTextAttr( pInput, OP_NULL, 0, OP_NULL,  &drawString.fontAttr, &drawString.fontcolor, OP_NULL );
        drawString.fontAttr |= FONT_OVERLAP;
        drawString.backcolor = COLOR_FONTBG;

        idx = pInput->txtInputBufMng.index;
        pChar = pInput->txtInputBufMng.pTxtInputBuf + (pInput->txtInputBufMng.rowStartList[idx]  << 1 ) ;
        for( iRow = 0 ; iRow < pInput->txtInputBufMng.cnt - 1; iRow ++ )
        {   
            next = (idx+1) %pInput->txtInputBufMng.cnt;
            
            iLen = (pInput->txtInputBufMng.rowStartList[next] -
                            pInput->txtInputBufMng.rowStartList[idx]) << 1;
            
            if( iLen > 0  )
            {
                if( MsfPasswordText ==  pInput->textType || MsfPasswordNumber == pInput->textType)
                {
                    /* fill pBuf with "*"   */
                    int i;
                    for(i=0; i<iLen; i++)
                    {
                        if(i%2)
                        {
                            buf[i] = 0x00;
                        }
                        else
                        {
                            buf[i] = 0x2a;
                        }
                    }
                    
                }
                else
                {
                    op_memcpy( buf,  pChar,  iLen );
                }
                buf[iLen] = buf[iLen +1] = 0x00;
                pChar += iLen;
            }
            else
            {
                break;
            }
            
            widgetScreenDrawTextLine( pWin,
                                                             iXPos,
                                                              iYPos, 
                                                              DS_SCRN_MAX_X,
                                                             buf, 
                                                             drawString.fontAttr, 
                                                             drawString.fontcolor, 
                                                             drawString.backcolor);

            iYPos += GADGET_LINE_HEIGHT;
            idx = next;
        }

        if( pTextInput->input.singleLine == 0
           || getWidgetType(pWin->windowHandle )== MSF_EDITOR )
        {
            /* draw vertical scrollbar */
            widgetTextInputAdjustScrollbar(pTextInput);
        }
    }
    
    SHOW

    if( bInFocus )
    {
        widgetDrawTextInputCaret(pTextInput, OP_FALSE);
        
#ifdef  _TOUCH_PANEL
        widgetImmSetCursorState(OP_TRUE);
#endif 

        OPUS_Start_Timer(OPUS_TIMER_TXT_INPUT_CURSOR_BLINK, 500, 0, PERIODIC);
    }
      
    return iRet;
}
/*==================================================================================================
    FUNCTION:  widgetStringGadgetShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetStringGadgetShow(void *pWidget, MsfWidgetType wt)
{
    MsfGadget            *pGadget;
    MsfStringGadget      *pStringGadget;
    OP_UINT8             *pChar;
    OP_UINT8             *pTitleChar;
    MsfWidgetDrawString  drawString;
    int                  iRet;
    
    if( !pWidget  || wt != MSF_STRINGGADGET  )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pGadget = (MsfGadget*)pWidget;
 
    /*Judge if the gadget parent  has focus  */
    if( pGadget->parent == OP_NULL )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pGadget->parent->windowHandle, OP_TRUE);
    }

    pStringGadget = (MsfStringGadget*)pGadget;
    
    WIDGET_GET_STRING_DATA(pChar, pStringGadget->text);
        
    if( !pChar )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pTitleChar = OP_NULL;
    if( ISVALIDHANDLE(pGadget->title ) )
    {
        WIDGET_GET_STRING_DATA(pTitleChar, pGadget->title);
            
        if( !pTitleChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    if( pStringGadget->bNeedAdjust == OP_TRUE )
    {
        widgetStringGadgetBuildPageIndexList(pStringGadget);
    }

    if( pStringGadget->singleLine == 0 )
    {
        iRet = widgetStringGadgetAdjustScrollbar(pWidget);
        if( iRet == TPI_WIDGET_PAINTED )
        {
            return TPI_WIDGET_OK;
        }
    }
    
    widgetGadgetGetPosition( pGadget, &drawString.pos.x, &drawString.pos.y );
    drawString.size = pGadget->size;
    
    widgetGetDrawingInfo( (void*) pGadget, 
                                        MSF_STRINGGADGET, 
                                        0, 
                                        &drawString.fontAttr, 
                                        &drawString.fontcolor, 
                                        &drawString.backcolor);
                                        
    if ( pGadget->isFocused   /*  selectgroup is focused and need to show focus bar */
       &&! (MSF_GADGET_PROPERTY_SHOW_NO_FOCUS_BAR & pGadget->propertyMask ))
    {
        widgetDrawHilight( pGadget->parent, 
                           &drawString.pos,
                           &drawString.size, 
                           OP_TRUE, 
                           OP_FALSE  );
#if (PROJECT_ID == ODIN_PROJECT)                           
        drawString.fontcolor = ds.cComposer.focusbarFontColor;
#endif        
    }
    else
    {
        widgetScreenFillRect( pGadget->parent, 
                              (OP_INT16) drawString.pos.x, 
                              (OP_INT16) drawString.pos.y, 
                              (OP_INT16) (drawString.pos.x + drawString.size.width -1 ), 
                              (OP_INT16) (drawString.pos.y + drawString.size.height -1 ), 
                              drawString.backcolor);
    }

    drawString.fontAttr |= FONT_OVERLAP;
    drawString.backcolor = COLOR_FONTBG;    
    
    if( pTitleChar && pStringGadget->singleLine == OP_FALSE )
    {
        if( drawString.size.height > DEFAULT_GADGET_HEIGHT)
        {
            drawString.size.height = DEFAULT_GADGET_HEIGHT;
        }
        
        drawString.pBuf = pTitleChar;
        drawString.fontAttr |= FONT_BOLD;
        drawString.bShowInitialWhiteSpaces = OP_FALSE;

        widgetDrawStringByChar( pGadget->parent, &drawString );
        
        drawString.fontAttr&= ~FONT_BOLD;
        drawString.pos.y += DEFAULT_GADGET_HEIGHT;
        drawString.size.height = pGadget->size.height -DEFAULT_GADGET_HEIGHT;
        if( drawString.size.height <= 0 )
        {
            return TPI_WIDGET_OK;
        }
    }

    if( pStringGadget->nPageCount > 0 )
    {
        drawString.bShowInitialWhiteSpaces = OP_TRUE;
        drawString.pBuf = (OP_UINT8*)(pChar + pStringGadget->pPageStarIndexList[pStringGadget->iCurPageIndex ]* 2);
        widgetDrawStringByChar( pGadget->parent, &drawString );
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetImageGadgetShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetImageGadgetShow(void *pWidget, MsfWidgetType wt)
{
    MsfGadget           *pGadget;
    MsfImageGadget    *pImageGadget;
    MsfSize              size;
    MsfImage              *pImage; 
    MsfPosition          pos;
    DS_COLOR          bgColor;
    int                       iRet;

    if( !pWidget || wt != MSF_IMAGEGADGET)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    pGadget = (MsfGadget*)pWidget;
    
    /*Judge if the gadget parent  has focus  */
    if( pGadget->parent == OP_NULL )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pGadget->parent->windowHandle, OP_TRUE);
    }

    pImageGadget = (MsfImageGadget*)pGadget;

    pImage = (MsfImage*)seekWidget( pImageGadget->image, &wt);
    if( !pImage ||wt != MSF_IMAGE )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    if(pImage->moreData == OP_TRUE) /* till we can get all image data */
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    widgetGadgetGetSize(pGadget, &size);
    widgetGetDrawingInfo( (void*) pGadget, 
                          MSF_IMAGEGADGET, 
                          0, 
                          OP_NULL, 
                          OP_NULL, 
                          &bgColor);

    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y );
    
    widgetScreenFillRect( pGadget->parent, 
                         pos.x, 
                         pos.y, 
                         (OP_INT16)(pos.x + size.width - 1 ), 
                         (OP_INT16)(pos.y + size.height - 1 ), 
                         bgColor);

    iRet = widgetScreenDrawImage(pGadget->parent,
                            &pos,
                            &size,
                            pImage,
                            pImageGadget->imageZoom);

    if(pGadget->isFocused)
    {
         widget_draw_focus_rect(iRet, pGadget->parent, &pos, &size);
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetButtonShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetButtonShow(void *pWidget, MsfWidgetType wt)
{
    MsfWindow            *pWin;
    MsfGadget            *pGadget;
    MsfButtonGadget      *pButton;
    OP_UINT8             *pChar;
    OP_INT16             iEngCharWidth, iEngCharHeight;
    OP_INT16             iLenByPixel;
    int                  nbrOfCharacters;
    int                  nbrOfInitialWhiteSpaces;      
    MsfWidgetDrawString  drawString;
    RM_RESOURCE_ID_T     bmp_res_id;
    
    if( !pWidget  || wt != MSF_BUTTON )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pButton = (MsfButtonGadget*)pWidget;
    pGadget = (MsfGadget*)pWidget;

    pWin = pGadget->parent;
 
    /*Judge if the gadget parent exists  */
    if( pWin == OP_NULL )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pGadget->parent->windowHandle, OP_TRUE);
    }

    /* get the MsfString of button's label  */
    pChar = OP_NULL;
    if( ISVALIDHANDLE(pGadget->title ) )
    {
        WIDGET_GET_STRING_DATA(pChar, pGadget->title);

        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    widgetGadgetGetPosition( pGadget, &drawString.pos.x, &drawString.pos.y );
    drawString.size = pGadget->size;
    
    /* get the background resource */ 
    if ( pGadget->isFocused ) /*  button is focused */
    {
        /* if the focus resource is provided, use it; or use the default resource  */
        if( pButton->iFocusRes )
        {
            bmp_res_id = pButton->iFocusRes;
        }
        else
        {
            bmp_res_id = ButtonFocusBackgroundRes;
        }
    }
    else
    {
        /* if the norma resource is provided, use it; or use the default resource  */
        if( pButton->iNormalRes )
        {
            bmp_res_id = pButton->iNormalRes;
        }
        else
        {
            bmp_res_id = ButtonNormalBackgroundRes;
        }
    }

    /* draw the background */
    if( RM_IsItBitmap(bmp_res_id) )
    {
        widgetScreenDrawBitmapRm(pWin, 
                                 drawString.pos.x, 
                                 drawString.pos.y, 
                                 drawString.size.width,
                                 drawString.size.height,
                                 bmp_res_id);
    }
    else if( RM_IsItIcon(bmp_res_id) )
    {
        widgetScreenDrawIconRm(pWin, 
                               drawString.pos.x, 
                               drawString.pos.y, 
                               drawString.size.width,
                               drawString.size.height,
                               bmp_res_id);
    }
    SHOW

    /* draw the label */
    if( pChar )
    {
        widgetGetDrawingInfo( (void*) pGadget, 
                              MSF_BUTTON, 
                              0, 
                              &drawString.fontAttr, 
                              &drawString.fontcolor, 
                              OP_NULL );
                                            
        drawString.pBuf = pChar;

        if( 0 > widgetGetFontSizeInEngChar( drawString.fontAttr, &iEngCharWidth, &iEngCharHeight))
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        /* calc the x-coordinate according to the labelPos */
        /* the label is aligned at center or right  */
        if( pButton->labelAlignment != MsfLeft )
        {

            iLenByPixel = widgetStringGetVisibleByChar(
                                    drawString.pBuf,
                                    &drawString.size,
                                    iEngCharWidth,
                                    OP_FALSE,
                                    &nbrOfCharacters,
                                    &nbrOfInitialWhiteSpaces);

            /* the label is aligned at right  */
            if( pButton->labelAlignment == MsfRight )
            {
                drawString.pos.x += drawString.size.width - iLenByPixel;
            }
            /* the label is aligned at center  */
            else
            {
                drawString.pos.x += ((drawString.size.width - iLenByPixel) >> 1);
            }
        }

        /* adjust the y-coordinate to the middle, 
          * and allow the label to be showed in only one line
          */
        if(drawString.size.height > GADGET_LINE_HEIGHT )
        {
            drawString.pos.y += (drawString.size.height - GADGET_LINE_HEIGHT )>>1;
            drawString.size.height = GADGET_LINE_HEIGHT;
        }

        /* draw the label in transparence, and ignore the front whitespaces  */
        drawString.bShowInitialWhiteSpaces = OP_FALSE;
        drawString.fontAttr |= FONT_OVERLAP;
        drawString.backcolor = COLOR_TRANS;    
        widgetDrawStringByChar( pWin, &drawString );
        SHOW
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetBarShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetBarShow(void *pWidget, MsfWidgetType wt)
{
    MsfWindow           *pWin;
    MsfGadget          *pGadget;
    MsfBar                 *pBar; 

    if( !pWidget || wt != MSF_BAR  )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    pGadget = (MsfGadget*)pWidget;
    
    /*Judge if the gadget parent  has focus  */
    if( pGadget->parent == OP_NULL )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }

    pWin = pGadget->parent;
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pWin->windowHandle, OP_TRUE);
    }
    
    pBar = (MsfBar*)pGadget;

    if( pBar->barType == MsfProgressBar )
    {
        return widgetProgressBarShow( pWin, pBar);
    }
    else if( pBar->barType == MsfVerticalScrollBar || pBar->barType == MsfHorizontalScrollBar )
    {
        return widgetScrollBarShow( pWin, pBar );
    }
    
    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetProgressBarShow( const MsfWindow *pWin, MsfBar *pBar )
{
    MsfGadget         *pGadget;
    DS_COLOR         bgColor;

    OP_INT16           left;
    OP_INT16           top;
    OP_INT16           right;
    OP_INT16           bottom;
    MsfWidgetColor    color;
    MsfWidgetColor    fillColor;
    MsfWidgetColor  hsl;
    int                       i;
    OP_INT16           rectLeft;
    OP_INT16           rectRight;
    DS_COLOR         whiteColor, midColor;

    if( !pBar || !pWin ||pBar->barType != MsfProgressBar )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pBar->maxValue <= 0 )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pGadget = (MsfGadget*)pBar;
    
    widgetGadgetGetPosition( pGadget, &left, &top );
    right = (OP_INT16)( left + pGadget->size.width -1);
    bottom = (OP_INT16)( top + pGadget->size.height-1);    
        

    color.colorRGB.r = DEFAULT_BAR_FG_COLORS.r;
    color.colorRGB.g = DEFAULT_BAR_FG_COLORS.g;
    color.colorRGB.b = DEFAULT_BAR_FG_COLORS.b;

    hsl.color= widgetRGBToHSL(color.color);
    hsl.colorHSL.s = 255;
    hsl.colorHSL.l = 180;

    widgetGetShodowColors(
                    &whiteColor, 
                    OP_NULL, 
                    &midColor, 
                    OP_NULL,
                    (DS_COLOR)_RGB(DEFAULT_BG_COLORS.r, DEFAULT_BG_COLORS.g, DEFAULT_BG_COLORS.b));
    
    widgetScreenDrawLine( pWin, left, top, right, top, (DS_COLOR)midColor);
    widgetScreenDrawLine( pWin, left, top, left, (OP_INT16)(bottom) , (DS_COLOR)midColor);
    widgetScreenDrawLine( pWin, (OP_INT16)(left+1), bottom, right, bottom, (DS_COLOR)whiteColor);
    widgetScreenDrawLine( pWin, right, (OP_INT16)(top + 1) , right, bottom, (DS_COLOR)whiteColor);

    left ++;
    top ++;
    right --;
    bottom --;
    
    rectLeft = rectRight = left;
    i = 0;
    color.colorHSL.h = hsl.colorHSL.h;
    color.colorHSL.s = hsl.colorHSL.s;
    
    while( i < pBar->value  )
    {
         i += pBar->lineStep;
         if( i > pBar->value )
         {
            i = pBar->value;
         }
         
        rectRight = left + i * (pGadget->size.width - 3) / pBar->maxValue;
        
        /* calc color  */
        color.colorHSL.l = 180  + ( 70 - 180 ) *i / pBar->maxValue;

        fillColor.color= widgetHSLToRGB( color.color);
        widgetScreenFillRect( pWin,
                                              rectLeft , 
                                              top ,
                                              rectRight, 
                                              bottom, 
                                              (DS_COLOR)_RGB(fillColor.colorRGB.r, fillColor.colorRGB.g, fillColor.colorRGB.b));

        rectLeft = rectRight;
    }        

    if( rectRight < right )
    {
        widgetGetDrawingInfo( (void*) pWin, getWidgetType(pWin->windowHandle), 0, OP_NULL, OP_NULL, &bgColor);
        widgetScreenFillRect( pWin, 
                                             rectRight, 
                                             top, 
                                             right, 
                                             bottom, 
                                             bgColor);
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetScrollBarShow( const MsfWindow *pWin, const MsfBar *pBar )
{
    MsfGadget         *pGadget;

    MsfPosition        pos;

    OP_INT16           left;
    OP_INT16           top;
    OP_INT16           right;
    OP_INT16           bottom;
    

    if( !pBar || !pWin 
    ||(pBar->barType != MsfVerticalScrollBar && pBar->barType != MsfHorizontalScrollBar ))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pGadget = (MsfGadget*)pBar;
    
    widgetGadgetGetPosition( pGadget, &left, &top );
    right = (OP_INT16)(left + pGadget->size.width -1);
    bottom = (OP_INT16)(top + pGadget->size.height-1);    

    if(pBar->barType == MsfVerticalScrollBar )
    {   
        if( pGadget->size.height > 2 * SCROLLBAR_ARROW_HEIGHT )
        {
            /* TOP ARROW  */
            pos.x = left;
            pos.y = top;
            widgetScreenDrawIconRm( pWin, 
                                    pos.x, 
                                    pos.y,
                                    SCROLLBAR_WIDTH,
                                    SCROLLBAR_ARROW_HEIGHT,
                                    SB_V_ARROW_UP ); 
            
            /* BOTTOM  ARROW  */ 
            pos.y = bottom - SCROLLBAR_ARROW_HEIGHT + 1;
            widgetScreenDrawIconRm( pWin, 
                                    pos.x, 
                                    pos.y,
                                    SCROLLBAR_WIDTH,
                                    SCROLLBAR_ARROW_HEIGHT,
                                    SB_V_ARROW_DOWN);
            
            /* SCOLL BAR */
            pos.y = top + SCROLLBAR_ARROW_HEIGHT;
            widgetScreenDrawIconRm( pWin, 
                                    pos.x, 
                                    pos.y,
                                    pGadget->size.width,
                                    (OP_INT16)(pGadget->size.height - SCROLLBAR_ARROW_HEIGHT*2),
                                    SB_V_BAR); 
             if(pBar->pageStep != 0)
             {
                /* SPIN ICON */
                if( (pGadget->size.height > (3 * SCROLLBAR_ARROW_HEIGHT + 1)) 
                    && (pBar->maxValue > 0 )
                    && (pBar->maxValue + 1 > pBar->pageStep))
                {
                    top += SCROLLBAR_ARROW_HEIGHT +  
                           pBar->value * (pGadget->size.height - SCROLLBAR_ARROW_HEIGHT * 2 - SCROLLBAR_SPIN_HEIGHT ) / (pBar->maxValue + 1 - pBar->pageStep );
                    bottom = top + SCROLLBAR_SPIN_HEIGHT - 1;
                    /* if((bottom + 1) >= (pGadget->size.height - SCROLLBAR_ARROW_HEIGHT))
                    {
                        top = pGadget->size.height - SCROLLBAR_ARROW_HEIGHT - SCROLLBAR_SPIN_HEIGHT - 1;
                    }*/
                    pos.y = top;

                    widgetScreenDrawIconRm( pWin, 
                                            (OP_INT16)(pos.x), 
                                            (OP_INT16)(pos.y),
                                            SCROLLBAR_WIDTH,
                                            SCROLLBAR_SPIN_HEIGHT,
                                            SB_V_SPIN ); 
                }
            }
        }
    }
    else
    {
        if( pGadget->size.width > 2 * SCROLLBAR_WIDTH )
        {
            /* LEFT ARROW */
            pos.x = left;
            pos.y = top;
            widgetScreenDrawIconRm( pWin, 
                                    pos.x, 
                                    pos.y,
                                    SCROLLBAR_ARROW_HEIGHT,
                                    SCROLLBAR_WIDTH,
                                    SB_H_ARROW_LEFT); 

            /* RIGHT ARROW */
            pos.x = right - SCROLLBAR_ARROW_HEIGHT + 1;
            widgetScreenDrawIconRm( pWin, 
                                    pos.x, 
                                    pos.y,
                                    SCROLLBAR_ARROW_HEIGHT,
                                    SCROLLBAR_WIDTH,
                                    SB_H_ARROW_RIGHT); 
            /* SCROLL BAR */
            pos.x = left + SCROLLBAR_ARROW_HEIGHT;
            widgetScreenDrawIconRm( pWin, 
                                    pos.x, 
                                    pos.y,
                                    (OP_INT16)(pGadget->size.width - SCROLLBAR_ARROW_HEIGHT*2),
                                    pGadget->size.height,
                                    SB_H_BAR); 
            if(pBar->pageStep != 0)
            {

                /* SPIN ICON */
                if( (pGadget->size.width > (3 * SCROLLBAR_ARROW_HEIGHT + 1))
                    && (pBar->maxValue > 0 )
                    && (pBar->maxValue + 1 > pBar->pageStep))
                {
                    left += SCROLLBAR_ARROW_HEIGHT + 
                            pBar->value  * (pGadget->size.width - SCROLLBAR_ARROW_HEIGHT * 2 - SCROLLBAR_SPIN_HEIGHT ) / (pBar->maxValue + 1 - pBar->pageStep );
                    right = left + SCROLLBAR_SPIN_HEIGHT - 1;
                    /*if((right + 1) >= (pGadget->size.width - SCROLLBAR_ARROW_HEIGHT))
                {
                        left = pGadget->size.width - SCROLLBAR_ARROW_HEIGHT - SCROLLBAR_SPIN_HEIGHT - 1;
                    }*/
                    pos.x = left;
                    widgetScreenDrawIconRm( pWin, 
                                            (OP_INT16)(pos.x), 
                                            (OP_INT16)(pos.y),
                                            SCROLLBAR_SPIN_HEIGHT,
                                            SCROLLBAR_WIDTH,
                                            SB_H_SPIN); 
                }
            }
        }
    }
    
    SHOW;
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    Widget  Show  Functions  End
==================================================================================================*/

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
OP_INT16 widgetGetWindowTitleHeight( const MsfWindow  *pWin )
{
    if( !pWin )
    {
        return 0;
    }
    
    switch( widgetWindowGetTitleType(pWin) )
    {
    case TITLE_NULL:
    case TITLE_NORMAL:
    case TITLE_ARROW:
    case TITLE_MSG_DETAIL:
    case TITLE_OWNERDRAW:
        return (OP_INT16)ds_get_image_height( WINDOW_TITLE_BG );
        break;
        
    case TITLE_MAIN_SUBMENU:
        return (OP_INT16)ds_get_image_height( MAIN_MENU_TITLE_BG );
        break;
#if (PROJECT_ID == LOTUS_PROJECT)
    case TITLE_WIDGET_MENU:
        return (OP_INT16)ds_get_image_height( WINDOW_TITLE_BG );
        break;
#endif
    default:
        return 0;
        break;
    }
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetWindowAdjustClientArea(MsfWindow *pWin)
{
    MsfWidgetType   wt;
    
    if( !pWin )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    wt = getWidgetType(pWin->windowHandle );
    if( wt == MSF_DIALOG )
    {
        pWin->clientOrigin.x = DIALOG_LEFT_BORDER;
        pWin->clientOrigin.y = DIALOG_TOP_BORDER;
        
        return TPI_WIDGET_OK;
    }
    
    /*  get  the origin of window client  -- add by zhuxq  */
    if( IF_SHOW_TITLE(pWin))
    {
        if( pWin->clientOrigin.y  <= 2 )
        {
            if(MSF_MAINMENU != wt)
            {
                pWin->clientOrigin.y = widgetGetWindowTitleHeight( pWin );
            }
        }
    }
    else
    {
        if( IF_SHOW_BORDER(pWin) )
        {
            pWin->clientOrigin.y = 2;
        }
        else
        {
            pWin->clientOrigin.y = 0;
        }
    }
    
    if( IF_SHOW_BORDER(pWin) )
    {
        pWin->clientOrigin.x = 2;
    }
    else
    {
        pWin->clientOrigin.x = 0;
    }

    switch(wt)
    {
    case MSF_EDITOR:
        {
            MsfEditor  *pEditor = ( MsfEditor*) pWin;

            if( pEditor->input != OP_NULL)
            {
                widgetGetClientRect( pWin, &pEditor->input->gadgetData.position, &pEditor->input->gadgetData.size );
            }
            break;
        }
    case MSF_MENU:
        {
            MsfWidgetType wtGadget;
            MsfGadget        *pScrollBar;
            MsfMenuWindow  *pMenu = (MsfMenuWindow*)pWin;
            MsfSelectgroup    *pSelectgroup = pMenu->pSelectgroup;
            MsfGadget        *pChoice = (MsfGadget*)pSelectgroup;

            if( pChoice )
            {
                widgetGetClientRect( pWin, &pChoice->position, &pChoice->size );
                
                pChoice->position.y += MENU_TITLE_AND_ITEM_OFFSET;
                pChoice->size.height -= MENU_TITLE_AND_ITEM_OFFSET;

#if (PROJECT_ID == DRAGON_M_PROJECT)
                pChoice->size.height -= MENU_TITLE_AND_ITEM_OFFSET;
#endif
                
                /* calc the elementsPerPage */
                pSelectgroup->choice.elementsPerPage = pChoice->size.height / CHOICE_MENU_LINE_HEIGHT;

                if( ISVALIDHANDLE(pChoice->hVScrollBar))
                {
                    pScrollBar = (MsfGadget*)seekWidget(pChoice->hVScrollBar, &wtGadget);
                    if( pScrollBar && wtGadget== MSF_BAR )
                    {
                        pChoice->size.width -= SCROLLBAR_WIDTH;

                        pScrollBar->size.width = SCROLLBAR_WIDTH;
                        pScrollBar->size.height = pChoice->size.height;

                        pScrollBar->position.x = pChoice->position.x + pChoice->size.width;
                        pScrollBar->position.y = pChoice->position.y;
                        ((MsfBar*)pScrollBar)->pageStep = pSelectgroup->choice.elementsPerPage;
                    }
                }
                
                pMenu->pSelectgroup->bItemPosCalculated = OP_FALSE;

                widgetSelectgroupAdjustScrollbar( pSelectgroup );
            }
            break;
        }
    case MSF_FORM:
        {
            MsfForm *pWidgetForm = (MsfForm *) pWin;
            widgetFormAdjustScrollbar(pWidgetForm);
            break;
        }
    case MSF_VIEWER:
        {
            MsfGadget     *pScrollBar;
            MsfViewer      *pViewer = (MsfViewer*)pWin;
            MsfGadget     *pGadget = (MsfGadget*)pViewer->pStringGadget;
            MsfWidgetType wtGadget;

            if( pGadget )
            {
                widgetGetClientRect( pWin, &pGadget->position, &pGadget->size );
                
                /* adjust the size of stringGadget and its scrollbar */
                if( ISVALIDHANDLE(pGadget->hVScrollBar))
                {
                    pScrollBar = (MsfGadget*)seekWidget(pGadget->hVScrollBar, &wtGadget);
                    if( pScrollBar && wtGadget== MSF_BAR )
                    {
                        pGadget->size.width -= SCROLLBAR_WIDTH;

                        pScrollBar->size.width = SCROLLBAR_WIDTH;
                        pScrollBar->size.height = pGadget->size.height;

                        pScrollBar->position.x = pGadget->position.x + pGadget->size.width;
                        pScrollBar->position.y = pGadget->position.y;
                    }
                }
                
                pViewer->pStringGadget->bNeedAdjust = OP_TRUE;
            }
            break;
        }
    default:
        break;
    }
    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetRedraw( void *pWidget, MsfWidgetType widgetType, OP_UINT32  handle, OP_BOOLEAN   bRefresh)
{
    void                    *pObj;
    MsfWidgetType   objType;
    MsfWidgetImm   *pImm;
    int                    iRet = TPI_WIDGET_ERROR_NOT_SUPPORTED;
    
    if( !pWidget )
    {
        if( ISVALIDHANDLE(handle))
        {
            pObj = seekWidget( handle, &objType );
            if( !pObj )
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }
        }    
        else
        {
            return TPI_WIDGET_ERROR_INVALID_PARAM;
        }
    }
    else
    {
        pObj = pWidget;
        objType = widgetType;
    }

    if( IS_GADGET(objType) )
    {
        MsfSize             size;
        OP_INT16          left, top, right, bottom;
        OP_BOOLEAN      bIntersect;
        MsfGadget         *pGadget = (MsfGadget*)pObj;

        iRet = widgetGadgetRedraw( pGadget, objType);
        if( iRet == TPI_WIDGET_PAINTED  && bRefresh == OP_TRUE)
        {
            pImm = pGadget->parent->parent->pImm;
            
            /*  if the imm is open and it intersects with the gadget, redraw the imm  */
            if( pImm && pImm->bOpen == OP_TRUE )
            {
                widgetGadgetGetPosition( pGadget, &left, &top);
                widgetGadgetGetSize(pGadget, &size);
                widgetPointClientToScreen( 
                                    pGadget->parent,
                                    left,
                                    top,
                                    &left, 
                                    &top);

                right = left + size.width -1;
                bottom = top + size.height - 1;

                bIntersect = widgetRectGetVisible(
                                            pImm->start_x,
                                            pImm->start_y,
                                            (OP_INT16)(DS_SCRN_MAX_X - 1),
                                            (OP_INT16)(WAP_SCRN_TEXT_HIGH - 1),
                                            &left, 
                                            &top,
                                            &right, 
                                            &bottom);

                if(( bIntersect == OP_TRUE ) 
#ifdef _KEYPAD
                    /*
                     *  Show the IMM when the gadget has focus and its type is Text Input
                     *  Andrew Add for P002210
                     */
                    || ((MSF_TEXTINPUT == objType) && ( OP_TRUE == pGadget->isFocused))
#endif
                )
                {
                    widgetImmShow( pImm, OP_TRUE);
                }
            }

            if( pGadget->parent->bDrawing == OP_FALSE )
            {
                WAP_REFRESH
            }
        }
    }   
    else if( IS_WINDOW(objType))
    {
        MsfWindow  *pWin = (MsfWindow*)pObj;

        pWin->bDrawing = OP_TRUE;
        
        iRet = widgetWindowRedraw( pWin, objType);
        if( iRet == TPI_WIDGET_PAINTED )
        {
            pImm = pWin->parent->pImm;
            
            if( pWin->isFocused == OP_TRUE && pImm && pImm->bOpen == OP_TRUE )
            {
                widgetImmShow( pImm, OP_TRUE );
            }
            
            WAP_REFRESH
        }
        
        pWin->bDrawing = OP_FALSE;
        
    }
    else if( objType == MSF_SCREEN )
    {
        MsfScreen   *pScreen = (MsfScreen*)pObj;

        /* the screen is non-focus or in batches update mode  */
        if( (pScreen->isFocused == OP_FALSE) ||pScreen->bInBatches )
        {
            return TPI_WIDGET_OK;
        }
        
        if( widgetShowFunc[MSF_SCREEN] != OP_NULL)
        {
            pImm = pScreen->pImm;
            
            iRet = widgetShowFunc[objType](pObj, MSF_SCREEN);

            if( pImm && pImm->bOpen == OP_TRUE )
            {
                widgetImmShow( pImm, OP_TRUE  );
            }

            if( iRet == TPI_WIDGET_PAINTED )
            {
                WAP_REFRESH
            }
        }
    }

    return iRet;    
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetGadgetRedraw
( 
    MsfGadget           *pGadget, 
    MsfWidgetType    widgetType
)
{
    MsfWidgetType     wt;
    MsfWindow           *pWin;
    int                       iRet;
    
    GadgetsInWindow     *pGadgetNode;
    GadgetsInWindow     *pOntopNode;
    MsfGadget              *pGt;
    OP_INT16               iGadgetLeft, iGadgetTop, iGadgetRight, iGadgetBottom;
    OP_INT16               left, top, right, bottom;
    OP_BOOLEAN         bVisible;   
    MsfSize                 sizeCurGadget, size;

    if( !pGadget )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
        
    pWin = pGadget->parent;
    if( !pWin || pGadget->bInBatches )   /* not add to window or in batches update mode, does not draw  */
    {
        return TPI_WIDGET_OK;
    }

    /*  parent window has no focus ,
      * or parent window in batches update mode,
      * or parent screen in batches update mode
      */
    // TODO: think this again. A gadget in a nonfocused window may be visible too
    if( (pWin->isFocused == OP_FALSE)||pWin->bInBatches || pWin->parent->bInBatches)
    {
        return TPI_WIDGET_OK;
    }

    if( widgetShowFunc[widgetType] == OP_NULL)   /* no show function  */
    {
        return TPI_WIDGET_OK;
    }
        
    /* draw the gadget  */
    iRet = widgetShowFunc[widgetType]( (void*)pGadget, widgetType );
    
    if( iRet < 0 )
    {
        return iRet;
    }

    /*  if the gadget is focused, then drawing the softkey area  */
#ifdef  _KEYPAD    
    if( pGadget->isFocused )
    {
        widgetWindowShowSoftKey( pWin, OP_FALSE );
    }
#endif    
    
    if( pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP )
    {
        return  TPI_WIDGET_PAINTED;
    }
    
    /*
      *  The gadget is not a ontop gadget, and there are some ontop gadgets overlapping with it,
      *  redraw these ontop gadgets
      */
    
    pGadgetNode = pWin->children;
    pOntopNode = OP_NULL;
    while(pGadgetNode )
    {
        pGt = (MsfGadget*)pGadgetNode->gadget;
        if( !pGt )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        
        if( pGt->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP )
        {
            pOntopNode = pGadgetNode;
        }

        pGadgetNode = pGadgetNode->next;
    }

    if( pOntopNode )
    {
        widgetGadgetGetPosition( pGadget, &iGadgetLeft, &iGadgetTop );
        widgetGadgetGetSize(pGadget, &sizeCurGadget);
        iGadgetRight = iGadgetLeft + sizeCurGadget.width - 1;
        iGadgetBottom = iGadgetTop + sizeCurGadget.height - 1;
        
        while( pOntopNode )
        {
            pGt = (MsfGadget*)pOntopNode->gadget;
            if( pGt && ( pGt->propertyMask & MSF_GADGET_PROPERTY_ALWAYSONTOP ))
            {
               wt = getWidgetType( pGt->gadgetHandle );
               widgetGadgetGetPosition( pGt, &left, &top );
                widgetGadgetGetSize(pGt, &size);
                right = left + size.width - 1;
                bottom = top + size.height -1;

                /*  If the ontop gadget  overlaps this gadget, redraw the ontop gadget  */
                bVisible = widgetRectGetVisible(
                                                        iGadgetLeft, 
                                                        iGadgetTop, 
                                                        iGadgetRight, 
                                                        iGadgetBottom, 
                                                        &left, 
                                                        &top, 
                                                        &right, 
                                                        &bottom);
                                                        
                if( bVisible == OP_TRUE )
                {
                    if( widgetShowFunc[wt] != OP_NULL)
                    {
                        iRet = widgetShowFunc[wt]( (void*)pGt, wt);
                        if ( iRet < 0 )
                        {
                            return iRet;
                        }
                    }
                }
            }
            
            pOntopNode = pOntopNode->prev;
        }
    }    

    return  TPI_WIDGET_PAINTED;
}   

/*==================================================================================================
    FUNCTION:  widgetWindowRedraw

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetWindowRedraw
( 
    MsfWindow         *pWin, 
    MsfWidgetType   wt
)
{
    int  iRet = TPI_WIDGET_OK;
    
    if( !pWin || !IS_WINDOW(wt))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    /*  window has no focus, 
      * or it is in batches update mode, or the parent screen is in batches update mode
      */
    if( (pWin->isFocused == OP_FALSE) || pWin->bInBatches || pWin->parent->bInBatches )
    {
        iRet = TPI_WIDGET_OK;
    }
    else if( widgetShowFunc[wt] != OP_NULL)
    {
        iRet = widgetShowFunc[wt]( (void*)pWin, wt );

        if( iRet >= 0 )
        {
            iRet = TPI_WIDGET_PAINTED;
        }
    }

    return iRet;
}
        
/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:

    RETURN VALUE:
    

    IMPORTANT NOTES:
        None
==================================================================================================*/
DS_SCRNMODE_ENUM_T  widgetScreenSetMode( void )
{
    DS_SCRNMODE_ENUM_T  pre_mode;

    pre_mode = ds.scrnMode;
    ds_stop_animation(&ds.ani);
    
#ifdef _SHOW_ANNUNCIATOR_IN_ALL_WINDOW
    bAnnuUpdate = MSF_TRUE;
#endif

    if( pre_mode != SCREEN_OWNERDRAW )
    {
        ds.scrnMode = SCREEN_OWNERDRAW;
#ifdef _SHOW_ANNUNCIATOR_IN_ALL_WINDOW
        ds_set_disp_annun();
        ds_init_softkeys();
#ifdef DS_NEED_EXTEND_SOFTKEY
        ds_set_extend_softkey(OP_FALSE);
#endif        
        util_set_rectangle(&ds.rcWin, 0,LCD_TEXT_FRAME_START, (OP_INT16)(LCD_MAX_X-1), (OP_INT16)(WAP_SCRN_TEXT_HIGH -1)); 
#else
        ds_clear_disp_annun();
        ds_clear_disp_annun();
        ds_init_softkeys();
        util_set_rectangle(&ds.rcWin, 0,0, (OP_INT16)(LCD_MAX_X-1), (OP_INT16)(WAP_SCRN_TEXT_HIGH-1)); 
#endif
      
        /* initialize window variables */
        ds.focused_line = CURSOR_OFF;
        ds.refresh_func = OP_NULL;
        
#ifdef _TOUCH_PANEL
        TPO_SetCurrent_RPU(TPO_RUI_NONE);  
#endif        
    }

    return pre_mode;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetGadgetGetPosition( const MsfGadget* pGadget,  OP_INT16  *pX, OP_INT16  *pY)
{
    if( !pGadget || !pX || !pY )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pGadget->parent == OP_NULL)
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    if( pGadget->alignment.verticalPos == MsfVerticalPosDefault 
        &&pGadget->alignment.horisontalPos== MsfHorizontalPosDefault )
    {
        *pX = pGadget->position.x;
        *pY = pGadget->position.y;
    }
    else
    {    
        MsfPosition   clientStart;
        MsfSize        clientSize;
        MsfSize        size;
        
        widgetGadgetGetSize((MsfGadget *)pGadget, &size);
        widgetGetClientRect( pGadget->parent, &clientStart, &clientSize);

        switch( pGadget->alignment.verticalPos )
        {
            case MsfHigh:
            {
                *pY = clientStart.y;
                break;    
            }
            case MsfMiddle:
            {
                *pY = clientStart.y + ((clientSize.height -size.height) >> 1 );
                break;    
            }
            case MsfLow:
            {
                *pY = clientStart.y + clientSize.height -size.height;
                break;    
            }
            default:
            {
                *pY = pGadget->position.y;
                break;    
            }
        }
        
        switch( pGadget->alignment.horisontalPos)
        {
            case MsfLeft:
            {
                *pX = clientStart.x;
                break;    
            }
            case MsfCenter:
            {
                *pX = clientStart.x + ((clientSize.width-size.width) >> 1 );
                break;    
            }
            case MsfRight:
            {
                *pX = clientStart.x + clientSize.width-size.width ;
                break;    
            }
            default:
            {
                *pX = pGadget->position.x;
                break;    
            }
        }
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         < 0 : some error
         TPI_WIDGET_OK : work normal
         TEXTINPUT_IMM_INFO_NOT_IN_WINDOW:    textinput already removed from  window

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetInputGetImmInfo
(
    OP_UINT32          hInput, 
    MsfTextType         *pTextType, 
    OP_INT16            **ppCurImeIndex,
    DS_FONTATTR      *pFontAttr,
    OP_UINT32          *phWin,
    OP_INT16            *pCurWinBottom,
    struct tagMsfWidgetImm ***pppImmObj,
    struct tagMsfWidgetImm ***pppImmObjInScreen
)
{
    void                   *pWidget;
    MsfWindow           *pWin;
    MsfTextInput          *pTextInput;
    MsfInput             *pInput;
    MsfWidgetType     wt;
    int                      iRet;

    pWidget = (void*)seekWidget( hInput, &wt);
    if( !pWidget || wt != MSF_TEXTINPUT)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    iRet = TPI_WIDGET_OK;
    
    pTextInput = (MsfTextInput*)pWidget;
    pWin = (MsfWindow*)pTextInput->gadgetData.parent;
    pInput = &pTextInput->input;

    if( pTextType )
    {
        *pTextType = pInput->textType;
    }

    if( phWin )
    {
        if( pWin )
        {
            *phWin = pWin->windowHandle;
        }
        else
        {
            *phWin = INVALID_HANDLE;
        }
    }
    
    if( pCurWinBottom )
    {
        if( pWin )
        {
            *pCurWinBottom = pWin->position.y + pWin->size.height - 1;
        }
        else
        {
            iRet = TEXTINPUT_IMM_INFO_NOT_IN_WINDOW;   /*  textinput already removed from  window  */
        }        
    }
    
    if( ppCurImeIndex )
    {
        *ppCurImeIndex = &pInput->iCurImeIndex;
    }

    if( pFontAttr )
    {
        widgetGetDrawingInfo(pWidget, wt, hInput, pFontAttr, OP_NULL, OP_NULL );
    }

    if( pppImmObj )
    {
        *pppImmObj = &pInput->pImm;
    }
    
    if( pppImmObjInScreen )
    {
        if( pWin && pWin->parent != OP_NULL )
        {
            *pppImmObjInScreen = &pWin->parent->pImm;
        }
        else
        {
            *pppImmObjInScreen = OP_NULL;
        }
    }

    return iRet;
}


/*==================================================================================================
    FUNCTION:  widgetPaintBoxScroll

    DESCRIPTION:
        Notify the paintbox to scroll its contents
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetPaintBoxScroll
(
    MsfWindow    *pPaintBox,
    OP_INT16     iDeltaX,
    OP_INT16     iDeltaY
)
{
    OP_UINT8          modId;
    MsfBar              *pVScrollBar;
    MsfBar              *pHScrollBar;
    GadgetsInWindow  *pNode;
    MsfWidgetType   wt;
    
    if( !pPaintBox || getWidgetType(pPaintBox->windowHandle) != MSF_PAINTBOX 
    ||( iDeltaX == 0 && iDeltaY == 0))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    modId = pPaintBox->modId;
    pNode = pPaintBox->children;
    pVScrollBar = OP_NULL;
    pHScrollBar = OP_NULL;

    while( pNode )
    {
        if( pNode->gadget == OP_NULL )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        
        wt = getWidgetType( pNode->gadget->gadgetHandle );
        if( wt == MSF_BAR 
            && ((MsfBar*)pNode->gadget)->hRelatedWidget == INVALID_HANDLE )
        {
            if( ((MsfBar*)pNode->gadget)->barType == MsfVerticalScrollBar )
            {
                pVScrollBar = (MsfBar*)pNode->gadget;
            }
            else if( ((MsfBar*)pNode->gadget)->barType == MsfHorizontalScrollBar )
            {
                pHScrollBar = (MsfBar*)pNode->gadget;
            }
        }
        
        pNode = pNode->next;
    }

    if( iDeltaY != 0 && pVScrollBar )
    {
        pVScrollBar->value += iDeltaY;
        
        widgetBarAdjustValue( pVScrollBar );

        /* Notify scrollbar to scroll contents  */
        TPIc_widgetNotify( modId, 
                                      pVScrollBar->gadgetData.gadgetHandle,
                                      MsfNotifyStateChange );
    }      

    if( iDeltaX != 0 && pHScrollBar )
    {
        pHScrollBar->value += iDeltaX;
        
        widgetBarAdjustValue( pHScrollBar );

        /* Notify scrollbar to scroll contents  */
        TPIc_widgetNotify( modId, 
                                      pHScrollBar->gadgetData.gadgetHandle,
                                      MsfNotifyStateChange );
    }      
    
    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  widgetPaintBoxRedraw

    DESCRIPTION:

        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetPaintBoxRedraw( MsfWindow    *pPaintBox, OP_BOOLEAN bRefresh)
{
    if( !pPaintBox || getWidgetType(pPaintBox->windowHandle) != MSF_PAINTBOX || pPaintBox->isFocused == OP_FALSE)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
#if 1   
  #if 0
    TPIc_widgetNotify( pPaintBox->modId, pPaintBox->windowHandle, MsfNotifyMoveResize );
  #else
    TPIc_widgetNotify( pPaintBox->modId, pPaintBox->windowHandle, MsfNotifyFocus);  
  #endif
    /* purely a patch, because the Paintbox maybe not draw the content,
     * and the back window can't draw the softkey,
     * so need to draw the softkey of the paintbox forcedly 
     */
    widgetWindowShowSoftKey( pPaintBox, bRefresh);
#else
    if( restoreDisplay( OP_FALSE,  pPaintBox) == OP_FALSE )
    {
        TPIc_widgetNotify( pPaintBox->modId, pPaintBox->windowHandle, MsfNotifyMoveResize );
    }
    else 
    {
        GadgetsInWindow  *pGadgetNode;
        MsfWidgetImm  *pImm;
        MsfWidgetType wt;
        MsfGadget       *pTextInput, *pGadget;
        
        widgetWindowDrawAllGadgets(pPaintBox);
        
        /* play image (gif animation)  */
        pGadgetNode = pPaintBox->children;
        while( pGadgetNode )
        {
            pGadget = pGadgetNode->gadget;
            if(pGadget && getWidgetType(pGadget->gadgetHandle) == MSF_IMAGEGADGET )
            {
                TPIa_widgetImagePlay(pGadget->gadgetHandle);
            }

            pGadgetNode = pGadgetNode->next;
        }
        
        pImm = pPaintBox->parent ->pImm;
        if( pImm )
        {
            pTextInput = (MsfGadget*)seekWidget(pImm->hInput, &wt);
//            widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, INVALID_HANDLE, OP_FALSE );

            if( pTextInput->isFocused == OP_TRUE && pImm->bOpen == OP_TRUE)
            {
                widgetImmShow( pImm, OP_TRUE );
            }
        }

    }

    {  /* draw the title of the form below the paintbox */
        MsfWindow  *pWin;
        MsfScreen   *pScreen = pPaintBox->parent;
        
        if( pScreen && pScreen->children && pScreen->children->next )
        {
            /* get the form */
            pWin = pScreen->children->next->window;

            widgetWindowDrawTitle( pWin, OP_NULL, OP_FALSE );
        }
    }
    
    widgetWindowShowSoftKey( pPaintBox, bRefresh);
    
    if( bRefresh== OP_TRUE )
    {
        WAP_REFRESH
    }
#endif    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetTextInputGetEditArea( const MsfTextInput *pInput, MsfPosition *pPos, MsfSize  *pSize )
{
    if( !pInput || !pPos || !pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    widgetGadgetGetPosition((const MsfGadget*)pInput, &pPos->x, &pPos->y);
    pSize->width = pInput->gadgetData.size.width;
    pSize->height= pInput->gadgetData.size.height;
    
    if( ISVALIDHANDLE(pInput->gadgetData.title))
    {
        pPos->y += GADGET_LINE_HEIGHT;
        pSize->height -= GADGET_LINE_HEIGHT;        
    }

    /*  minus the border width  */
    pPos->x += 2;
    pPos->y += 2;
    pSize->width -= 4;
    pSize->height -= 4;

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static OP_INT16 widgetStringCalcPageEndPos
(
    const OP_UINT8   *pStr,
    MsfSize               *pSize, 
    OP_INT16             iEngCharWidth,
    OP_BOOLEAN        bSingleLine,
    OP_INT16            *pNextPagePos
)
{
    const OP_UINT8   *pChar;
    int                     nbrOfInitialWhiteSpaces;
    int                     nVisibleChars;        /*  the visible chars */
    int                     nVisibleLineChars;  
    int                     nBeforeLastLine;
    int                     nLines;
    int                     iWidth;
    MsfSize              size;

    if( !pStr ||!pSize )
    {
        return 0;
    }

    nVisibleChars = 0;
    *pNextPagePos = 0;
    
    pChar = pStr;
    if( bSingleLine == OP_TRUE )       /* only one line */
    {
        widgetStringGetVisibleByChar( 
                                    pChar, 
                                    pSize, 
                                    iEngCharWidth, 
                                    OP_TRUE, 
                                    &nVisibleChars, 
                                    &nbrOfInitialWhiteSpaces);
                                    
        if( nVisibleChars > 1 )
        {
            *pNextPagePos = (OP_INT16)(nVisibleChars -1);
        }
        else
        {
            *pNextPagePos = (OP_INT16)nVisibleChars;
        }
    }
    else
    {
        size = *pSize;
        nLines = 0;
        nBeforeLastLine = 0;
        
        while( size.height >= GADGET_LINE_HEIGHT && (*pChar != 0x00 || *(pChar+ 1) != 0x00) )
        {
            iWidth = widgetStringGetVisibleByChar( 
                                        pChar, 
                                        &size, 
                                        iEngCharWidth, 
                                        OP_TRUE, 
                                        &nVisibleLineChars, 
                                        &nbrOfInitialWhiteSpaces);
                                        
            nLines++;                                        
            nBeforeLastLine = nVisibleChars;
            nVisibleChars += nVisibleLineChars;
            pChar += ( nVisibleLineChars << 1);
            size.height -= GADGET_LINE_HEIGHT;
        }

        if( nLines > 1 &&  iWidth > 0 )
        {
            *pNextPagePos = (OP_INT16)nBeforeLastLine;
        }
        else
        {
            *pNextPagePos = (OP_INT16)nVisibleChars;
        }
    }

    return (OP_INT16)nVisibleChars;
}


/*==================================================================================================
    FUNCTION:  widgetStringGadgetBuildList

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetStringGadgetBuildList
(
    MsfStringGadget  *pStringGadget,
    MsfSize            *pSize,
    OP_INT16         iEngCharWidth,
    OP_BOOLEAN    bSingleLine
)
{
    OP_UINT8         *pChar;
    OP_UINT8         *pStrPage;         /* the start position of the current page */
    OP_UINT8         *pStrNextPage;  /* the start position of the next page if the page exists */
    OP_INT16         nPageChars;       /* the number of chars of current page */
    OP_INT16         iNextPagePos;    /* the next page start position relative to current page */
    OP_INT16         iPage;                /* the next page index */
    MsfSize          size;
    OP_INT16         nCharCount;         /* the total char count up to the current page  */  
    OP_BOOLEAN       bCompleted;
    
    /* the calc result of the 20th page, need save it if exceed 20 pages  */
    OP_UINT8         *pStrPageSave = OP_NULL;         
    OP_UINT8         *pStrNextPageSave = OP_NULL;   
    OP_INT16         nCharCountSave = 0;  
    OP_INT16         iNextPagePosSave = 0; 

    OP_UINT16        *pList;
    
    if( !pStringGadget || !pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pStringGadget->nPageCount = 0;
    pStringGadget->pPageStarIndexList[0] = 0;
    
    if( !ISVALIDHANDLE(pStringGadget->text))
    {
        return TPI_WIDGET_OK;
    }

    WIDGET_GET_STRING_DATA(pChar, pStringGadget->text);
        
    if( !pChar )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    size = *pSize;
    pStrPage = pChar;
    pStrNextPage = pStrPage;
    
    iPage = 0;
    nCharCount = 0;
    nPageChars = 0; 
    iNextPagePos = 0;
    bCompleted = OP_FALSE;
    pList = pStringGadget->pPageStarIndexList;
    
    while( (bCompleted == OP_FALSE) && (*pStrPage != 0x00 || *(pStrPage+1) != 0x00 ) )
    {
        if( iPage == STRINGGADGET_DEFAULT_BUF_PAGES )  /* the buf size */
        {
            /* save the calc result of the 20th page */
            iNextPagePosSave = iNextPagePos;
            nCharCountSave = nCharCount;
            pStrPageSave = pStrPage;
            pStrNextPageSave = pStrNextPage;
        }        
        
        pStrPage = pStrNextPage;        
        nCharCount += iNextPagePos;
        
        if( iPage < STRINGGADGET_DEFAULT_BUF_PAGES )
        {
            pList[iPage] = nCharCount;
        }
        
        nPageChars = widgetStringCalcPageEndPos(
                                                pStrPage, 
                                                &size,
                                                iEngCharWidth, 
                                                bSingleLine, 
                                                &iNextPagePos);
        if( nPageChars > 0 )
        {
            iPage++;
            pStrPage += (nPageChars << 1);
            pStrNextPage += (iNextPagePos << 1);
        }
        else
        {
            bCompleted = OP_TRUE;
        }
    }

    pStringGadget->nPageCount = iPage;

    if( iPage > STRINGGADGET_DEFAULT_BUF_PAGES )  /* need to alloc momery to save the page index list  */
    {
        pList= OP_NULL;
        pList = (OP_UINT16*)WIDGET_ALLOC( iPage * sizeof(OP_UINT16) );
        if( !pList )
        {
            return TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        }

        op_memcpy(pList, pStringGadget->pPageStarIndexList, 20 * sizeof(OP_UINT16) );

        pStringGadget->pPageStarIndexList = pList;

        /* restore the calc result of the 20th page */
        iNextPagePos = iNextPagePosSave;
        nCharCount = nCharCountSave;
        pStrPage = pStrPageSave;
        pStrNextPage = pStrNextPageSave;
        iPage = STRINGGADGET_DEFAULT_BUF_PAGES;
        bCompleted = OP_FALSE;

        while( (bCompleted == OP_FALSE) && (*pStrPage != 0x00 || *(pStrPage+1) != 0x00 ) )
        {
            pStrPage = pStrNextPage;        
            nCharCount += iNextPagePos;
            pList[iPage] = nCharCount;
            
            nPageChars = widgetStringCalcPageEndPos( pStrPage, &size, iEngCharWidth, bSingleLine, &iNextPagePos);
            if( nPageChars > 0 )
            {
                iPage++;
                pStrPage += (nPageChars << 1);
                pStrNextPage += (iNextPagePos << 1);
            }
            else
            {
                bCompleted = OP_TRUE;
            }
        }
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetStringGadgetBuildPageIndexList

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetStringGadgetBuildPageIndexList( MsfStringGadget *pStringGadget )
{
    DS_FONTATTR   fontAttr;
    OP_INT16         iEngCharWidth, iEngCharHeight;
    MsfSize            size;
    
    if( !pStringGadget )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pStringGadget->bNeedAdjust == OP_FALSE )
    {
        return TPI_WIDGET_OK;
    }

    pStringGadget->bNeedAdjust = OP_FALSE;

    /* if alloc momery, free it */
    if( pStringGadget->pPageStarIndexList != pStringGadget->pageStarIndexListBuf )
    {
        if( pStringGadget->pPageStarIndexList )
        {
            WIDGET_FREE( pStringGadget->pPageStarIndexList );
        }

        pStringGadget->pPageStarIndexList = pStringGadget->pageStarIndexListBuf;
    }
    
    pStringGadget->iCurPageIndex = 0;

    widgetGetDrawingInfo( (void*) pStringGadget, 
                                            MSF_STRINGGADGET, 
                                            0, 
                                            &fontAttr, 
                                            OP_NULL, 
                                            OP_NULL);
                                            
    if( 0 > widgetGetFontSizeInEngChar( fontAttr, &iEngCharWidth, &iEngCharHeight))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    if( pStringGadget->singleLine )
    {
        return widgetStringGadgetBuildList( 
                                    pStringGadget, 
                                    &pStringGadget->gadgetData.size,
                                    iEngCharWidth,
                                    OP_TRUE);
    }
    else
    {
        size = pStringGadget->gadgetData.size;
        if( ISVALIDHANDLE(pStringGadget->gadgetData.title ) )
        {
            size.height -= DEFAULT_GADGET_HEIGHT;
        }
        
        return widgetStringGadgetBuildList( 
                                    pStringGadget, 
                                    &size,
                                    iEngCharWidth,
                                    OP_FALSE);
    }
}

/*==================================================================================================
    FUNCTION:  widgetWindowDrawStringByChar

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetWindowDrawStringByChar
(
    MsfWindow    *pWin,
    OP_UINT8    *pStr,
    MsfPosition   *pPos,
    MsfSize        *pSize
)
{
    MsfWidgetDrawString drawString;
    
    if( !pWin || !pStr ||!pPos ||!pSize)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    widgetGetDrawingInfo( (void*) pWin, 
                                        getWidgetType(pWin->windowHandle), 
                                        0, 
                                        &drawString.fontAttr, 
                                        &drawString.fontcolor, 
                                        OP_NULL );
                                            
    drawString.pos = *pPos;
    drawString.size = *pSize;
    drawString.pBuf = (const OP_UINT8*)pStr;

    drawString.fontAttr |= FONT_OVERLAP;
    drawString.backcolor = COLOR_FONTBG;
    drawString.bShowInitialWhiteSpaces = OP_FALSE;
    
    widgetDrawStringByChar(pWin, &drawString);

    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  widgetDialogAdjustInputPos

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetDialogAdjustInputPos
(
    MsfDialog               *pDlg,
    const MsfPosition   *pStartPos
)
{
    GadgetsInWindow    *pGadgetNode;
    MsfPosition          pos;
    MsfGadget           *pGadget;
    
    if( !pDlg || !pStartPos)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pDlg->type != MsfPrompt 
        || pDlg->bNeedAdjustInputPos == OP_FALSE
        || pDlg->inputCount < 1 )
    {
        return TPI_WIDGET_OK;
    }

    pDlg->bNeedAdjustInputPos = OP_FALSE;

    pos = *pStartPos;
    
    pGadgetNode = pDlg->windowData.children;

    while( pGadgetNode )
    {
        pGadget = pGadgetNode->gadget;
        if( pGadget)
        {
            pGadget->position = pos;
            pos.y += (pGadget->size.height + 4);
        }
        
        pGadgetNode = pGadgetNode->next;
    }
    
    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  widgetChoiceGetItemRect

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetChoiceGetItemRect
( 
    MsfSelectgroup         *pSelectGroup,
    OP_INT16              index,
    MsfPosition           *pPos,
    MsfSize                *pSize
)
{
    MsfPosition           pos;
    OP_INT16             iBottomItem;
    
    if( !pSelectGroup || !pPos || !pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    } 

    /* calc the bottom item index */
    iBottomItem = pSelectGroup->choice.count -1;
    if( pSelectGroup->choice.top_item + pSelectGroup->choice.elementsPerPage < pSelectGroup->choice.count )
    {
        iBottomItem = pSelectGroup->choice.top_item + pSelectGroup->choice.elementsPerPage -1;
    }
    
    if( index < pSelectGroup->choice.top_item 
     || index > iBottomItem )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    /* get the selectgroup position and size */
    widgetGadgetGetPosition( &pSelectGroup->gadgetData, &pos.x, &pos.y );

    pPos->x =  pos.x + pSelectGroup->clientPos.x;
    pPos->y = pos.y + pSelectGroup->clientPos.y + pSelectGroup->iSpaceY
                    + (index - pSelectGroup->choice.top_item) * (pSelectGroup->itemSize.height + pSelectGroup->iSpaceY );

    *pSize = pSelectGroup->itemSize;

    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:  widgetBarAdjustValue

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
void widgetBarAdjustValue( MsfBar *pBar )
{
    if( !pBar)
    {
        return;
    }

    if( pBar->maxValue <= 0 )
    {
        pBar->maxValue = 0;
        pBar->value = 0;
    }
    else if( pBar->value < 0 )
    {
        pBar->value = 0;
    }
    else if( pBar->value > pBar->maxValue )
    {
        pBar->value = pBar->maxValue;
    }
    
    if( pBar->barType == MsfVerticalScrollBar || pBar->barType == MsfHorizontalScrollBar)
    {
        if( (pBar->maxValue + 1 ) > pBar->pageStep ) 
        {
            if( pBar->value > (pBar->maxValue + 1  - pBar->pageStep ) )
            {
                pBar->value = pBar->maxValue + 1  - pBar->pageStep;
            }
        }
        else
        {
            pBar->value = 0;
        }
    }
}

/*==================================================================================================
    FUNCTION:  widgetPaintBoxEndDraw

    DESCRIPTION:
        Notify the paintbox to cache its contents into bitmap area
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetPaintBoxEndDraw(MSF_UINT32  hPaintBox)
{
    MsfWindow         *pWin;
    MsfWidgetType  wtWin;
    MsfTextInput        *pInput;
    MsfGadget         *pGadget;
    GadgetsInWindow  *pGadgetNode;

    WIDGET_TRACE(("widgetPaintBoxEndDraw\n"));
    pWin = (MsfWindow*)seekWidget(hPaintBox, &wtWin);
    if( !pWin || wtWin != MSF_PAINTBOX )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( (pWin->isFocused == OP_FALSE) || (((MsfPaintbox*)pWin)->isHoldDraw == OP_TRUE ))
    {
        return TPI_WIDGET_OK;
    }
    
    /* 
     * back up the paintbox
     */
    backupDisplay();

    /* play image (gif animation)  */
    pGadgetNode = pWin->children;
    while( pGadgetNode )
    {
        pGadget = pGadgetNode->gadget;
        if(pGadget && getWidgetType(pGadget->gadgetHandle) == MSF_IMAGEGADGET )
        {
            TPIa_widgetImagePlay(pGadget->gadgetHandle);
        }

        pGadgetNode = pGadgetNode->next;
    }

    /* draw the imm */
    if( pWin->focusedChild 
        && pWin->focusedChild->gadget
        && getWidgetType(pWin->focusedChild->gadget->gadgetHandle) == MSF_TEXTINPUT
        && (pWin->focusedChild->gadget->propertyMask & MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM))
    {
        pInput = (MsfTextInput*)pWin->focusedChild->gadget;
        widgetInputFocusChange(pInput->gadgetData.gadgetHandle, OP_TRUE );
        if( pInput->input.pImm && pInput->input.pImm->bOpen == OP_TRUE )
        {
            widgetImmShow(pInput->input.pImm, OP_TRUE );
        }
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetPaintBoxNeedHorizontalScroll

    DESCRIPTION:
        Notify the paintbox to scroll its contents
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
OP_BOOLEAN  widgetPaintBoxNeedHorizontalScroll( MsfWindow *pPaintBox )
{
    MsfWidgetType  wtGadget;
    GadgetsInWindow  *pNode;

    if( !pPaintBox || getWidgetType(pPaintBox->windowHandle) != MSF_PAINTBOX )
    {
        return OP_FALSE;
    }

    pNode = pPaintBox->children;

    while( pNode )
    {
        if( pNode->gadget != OP_NULL )
        {
            wtGadget = getWidgetType( pNode->gadget->gadgetHandle );
            if( wtGadget == MSF_BAR 
                && ((MsfBar*)pNode->gadget)->hRelatedWidget == INVALID_HANDLE 
                && ((MsfBar*)pNode->gadget)->barType == MsfHorizontalScrollBar)
            {
                return OP_TRUE;
            }
        }
        
        pNode = pNode->next;
    }

    return OP_FALSE;
}

/*==================================================================================================
    FUNCTION:  widgetSelectgroupAdjustScrollbar

    DESCRIPTION:
        Adjust the scroll bar related to the selectgroup.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetSelectgroupAdjustScrollbar( MsfSelectgroup  *pSelectgroup)
{
    MsfGadgetHandle    hBar;
    MsfGadget    *pGadget;
    MsfChoice      *pChoice;
    MsfBar         *pBar;
    MsfSize        size;
    MsfPosition   pos;
    
    if( !pSelectgroup )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pGadget = (MsfGadget*)pSelectgroup;
    /* not show the scrollbar */
    if( pSelectgroup->showStyle & SG_SHOW_STYLE_NO_SCROLLBAR )
    {
        /* remove scrollbar */
        if(ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            if( pGadget->parent )
            {
                TPIa_widgetRemove(pGadget->parent->windowHandle, pGadget->hVScrollBar);
            }
            TPIa_widgetRelease(pGadget->hVScrollBar);
            
            pGadget->hVScrollBar = INVALID_HANDLE;
            
            /*after add scrollbar, the related gadget size should be adjust*/
            size.width =  pGadget->size.width + SCROLLBAR_WIDTH;
            size.height = pGadget->size.height;
            TPIa_widgetSetSize(pGadget->gadgetHandle, &size);
        }
        return TPI_WIDGET_OK;
    }
    
    pChoice = &pSelectgroup->choice;

    if( pChoice->count <= pChoice->elementsPerPage ) /*  remove the scrollbar  */
    {
        /* remove scrollbar */
        if(ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            if( pGadget->parent )
            {
                TPIa_widgetRemove(pGadget->parent->windowHandle, pGadget->hVScrollBar);
            }
            TPIa_widgetRelease(pGadget->hVScrollBar);
            
            pGadget->hVScrollBar = INVALID_HANDLE;
            
            /*after add scrollbar, the related gadget size should be adjust*/
            size.width =  pGadget->size.width + SCROLLBAR_WIDTH;
            size.height = pGadget->size.height;
            TPIa_widgetSetSize(pGadget->gadgetHandle, &size);
        }
    }
    else  if(!ISVALIDHANDLE(pGadget->hVScrollBar))  /* add scrollbar into menu gadget */
    {
        /*first we should caculate the gadget size*/
        size.height = pGadget->size.height;
        size.width = SCROLLBAR_WIDTH;
        hBar = TPIa_widgetBarCreate(
                                        pGadget->modId,
                                        MsfVerticalScrollBar,
                                        (pChoice->count - 1),
                                        pChoice->top_item,
                                        &size,
                                        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_ALWAYSONTOP,
                                        0);

        if( ISVALIDHANDLE(hBar))
        {
            pBar = (MsfBar*)seekWidget(hBar, OP_NULL );

            pBar->pageStep = pChoice->elementsPerPage;
            pBar->lineStep = 1;
            pBar->hRelatedWidget = pGadget->gadgetHandle;

            /*before add scrollbar, the related gadget size should be adjust*/
            size.width =  pGadget->size.width - SCROLLBAR_WIDTH;
            TPIa_widgetSetSize(pGadget->gadgetHandle, &size);

            /* add to window */
            if( pGadget->parent )
            {
                widgetGadgetGetPosition( pGadget, &pos.x, &pos.y);
                pos.x += pGadget->size.width;
                
                TPIa_widgetWindowAddGadget(
                                             pGadget->parent->windowHandle, 
                                             hBar, 
                                             &pos, 
                                             0);
            }

            pGadget->hVScrollBar = hBar;
            
        }
    }
    else  /*  adjust the scroll bar value  */
    {
       TPIa_widgetBarSetValues(pGadget->hVScrollBar, pChoice->top_item, (pChoice->count - 1));
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetSelectgroupAdjustScrollbar

    DESCRIPTION:
        Adjust the scroll bar related to the MsfBox.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetBoxAdjustScrollbar( MsfBox  *pBox)
{
    MsfSelectgroup   *pSelectgroup;
    MsfGadgetHandle    hBar;
    MsfGadget    *pGadget;
    MsfChoice      *pChoice;
    MsfBar         *pBar;
    MsfSize        size;
    MsfPosition   pos;
    
    if( !pBox )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pSelectgroup = (MsfSelectgroup*)pBox;
    pGadget = (MsfGadget*)pBox;
    pChoice = &pSelectgroup->choice;

    /* not show the scrollbar */
    if( pSelectgroup->showStyle & SG_SHOW_STYLE_NO_SCROLLBAR )
    {
        /* remove scrollbar */
        if(ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            if( pGadget->parent )
            {
                TPIa_widgetRemove(pGadget->parent->windowHandle, pGadget->hVScrollBar);
            }
            TPIa_widgetRelease(pGadget->hVScrollBar);
            
            pGadget->hVScrollBar = INVALID_HANDLE;
            
            /*after add scrollbar, the related gadget size should be adjust*/
            size.width =  pGadget->size.width + SCROLLBAR_WIDTH;
            size.height = pGadget->size.height;
            TPIa_widgetSetSize(pGadget->gadgetHandle, &size);
        }

        if(pBox->nItemCount  <= pChoice->elementsPerPage)
        {
            pBox->iCurPos = 0;
        }
        else if( pBox->iCurPos + pChoice->elementsPerPage > pBox->nItemCount )
        {
            pBox->iCurPos = pBox->nItemCount - pChoice->elementsPerPage;
        }
        
        return TPI_WIDGET_OK;
    }

    if( pBox->nItemCount  <= pChoice->elementsPerPage ) /*  remove the scrollbar  */
    {
        /* remove scrollbar */
        if(ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            if( pGadget->parent )
            {
                TPIa_widgetRemove(pGadget->parent->windowHandle, pGadget->hVScrollBar);
            }
            TPIa_widgetRelease(pGadget->hVScrollBar);
            
            pGadget->hVScrollBar = INVALID_HANDLE;
            
            /*after add scrollbar, the related gadget size should be adjust*/
            size.width =  pGadget->size.width + SCROLLBAR_WIDTH;
            size.height = pGadget->size.height;
            TPIa_widgetSetSize(pGadget->gadgetHandle, &size);
        }

        pBox->iCurPos = 0;
    }
    else  if(!ISVALIDHANDLE(pGadget->hVScrollBar))  /* add scrollbar into menu gadget */
    {
        /*first we should caculate the gadget size*/
        size.height = pGadget->size.height;
        size.width = SCROLLBAR_WIDTH;
        hBar = TPIa_widgetBarCreateEx(
                                        pGadget->modId,
                                        MsfVerticalScrollBar,
                                        (pBox->nItemCount - 1),
                                        pBox->iCurPos,
                                        &size,
                                        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_ALWAYSONTOP|MSF_GADGET_PROPERTY_NOTIFY,
                                        0,
                                        widgetBox_scrollbarStateChangeCb);

        if( ISVALIDHANDLE(hBar))
        {
            pBar = (MsfBar*)seekWidget(hBar, OP_NULL );

            pBar->pageStep = pChoice->elementsPerPage;
            pBar->lineStep = 1;
            pBar->hRelatedWidget = pGadget->gadgetHandle;

            /*before add scrollbar, the related gadget size should be adjust*/
            size.width =  pGadget->size.width - SCROLLBAR_WIDTH;
            TPIa_widgetSetSize(pGadget->gadgetHandle, &size);

            /* add to window */
            if( pGadget->parent )
            {
                widgetGadgetGetPosition( pGadget, &pos.x, &pos.y);
                pos.x += pGadget->size.width;
                
                TPIa_widgetWindowAddGadget(
                                             pGadget->parent->windowHandle, 
                                             hBar, 
                                             &pos, 
                                             0);
            }

            pGadget->hVScrollBar = hBar;
            
        }
    }
    else  /*  adjust the scroll bar value  */
    {
       TPIa_widgetBarSetValues(pGadget->hVScrollBar, pBox->iCurPos, (pBox->nItemCount - 1));
       TPIa_widgetBarGetValues(pGadget->hVScrollBar, &pBox->iCurPos, OP_NULL );
    }

    return TPI_WIDGET_OK;    
}

/*==================================================================================================
    FUNCTION:  widgetTextInputAdjustScrollbar

    DESCRIPTION:
        Adjust the scroll bar related to the selectgroup.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetTextInputAdjustScrollbar(MsfTextInput *pWidget)
{
    MsfWidgetType  wt;
    MsfGadget        *pGadget = OP_NULL; 
//    MsfWindow       *pWin = OP_NULL; 
    MsfInput          *pInput = OP_NULL;
//    OP_UINT16       TempCnt = 0;
    MsfBar             *pBar =  OP_NULL;
    MsfSize            size;
    MsfPosition       pos;
    MsfGadgetHandle    hBar = OP_NULL;
    
    if( !pWidget )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    pGadget = (MsfGadget*)pWidget;
  
    pInput = &((MsfTextInput*)pWidget)->input;

  /* less one screen */
    if(pInput->txtInputBufMng.TotalRows <= pInput->txtInputBufMng.rows
        && !(pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYS_SHOW_SCROLL_BAR))
    {
        /* remove scrollbar */
        if(ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            if( pGadget->parent )
            {
                TPIa_widgetRemove(pGadget->parent->windowHandle, pGadget->hVScrollBar);
            }
            TPIa_widgetRelease(pGadget->hVScrollBar);
            
            pGadget->hVScrollBar = INVALID_HANDLE;
             //pGadget->size.width += SCROLLBAR_WIDTH;
             
        }
    }
    else
    {
        if(!ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            size.height = pGadget->size.height;
            size.width = SCROLLBAR_WIDTH;
            hBar = TPIa_widgetBarCreate(
                                    pGadget->modId,
                                    MsfVerticalScrollBar,
                                    pInput->txtInputBufMng.TotalRows-1,
                                    pInput->txtInputBufMng.top_line,
                                    &size,
                                    MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_ALWAYSONTOP,
                                    0);
            
            if( ISVALIDHANDLE(hBar))
            {
                pBar = (MsfBar*)seekWidget(hBar, OP_NULL );
                if((pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYS_SHOW_SCROLL_BAR) &&
                   (pInput->txtInputBufMng.TotalRows <= pInput->txtInputBufMng.rows))
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
                size.width =  pGadget->size.width - SCROLLBAR_WIDTH;
                
                /* add to window  */
                if( pGadget->parent )
                {
                    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y);              
                    pos.x += size.width;
                    
                    TPIa_widgetWindowAddGadget(
                                                 pGadget->parent->windowHandle, 
                                                 hBar, 
                                                 &pos, 
                                                 0);
                }
            }
        }
        else
        {
            pBar = (MsfBar*)seekWidget( pGadget->hVScrollBar, &wt);
            if( pBar && wt == MSF_BAR )
            {
                pBar->gadgetData.size.height = pGadget->size.height;
                if((pGadget->propertyMask & MSF_GADGET_PROPERTY_ALWAYS_SHOW_SCROLL_BAR) &&
                   (pInput->txtInputBufMng.TotalRows <= pInput->txtInputBufMng.rows))
                {
                    pBar->pageStep = 0;
                }
                else
                {
                    pBar->pageStep = pInput->txtInputBufMng.rows;
                }
                
#if 0
                pBar->value = pInput->txtInputBufMng.top_line;
                pBar->maxValue = pInput->txtInputBufMng.TotalRows-1;
                
                widgetBarAdjustValue(pBar);  
#endif                
                // pBar->pageStep =  pInput->txtInputBufMng.rows;
                TPIa_widgetBarSetValues(
                                            pGadget->hVScrollBar,
                                            pInput->txtInputBufMng.top_line,
                                            pInput->txtInputBufMng.TotalRows-1);
            }
        }
    }
    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  widgetStringGadgetAddScrollbar

    DESCRIPTION:
        Adjust the scroll bar related to the StringGadget.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetStringGadgetAdjustScrollbar(MsfStringGadget *pWidget)
{
    static OP_BOOLEAN bEntered = OP_FALSE;
    MsfGadget        *pGadget; 
    MsfBar           *pBar =  OP_NULL;
    MsfSize          size;
    MsfPosition      pos;
    MsfGadgetHandle  hBar = OP_NULL;
  
    OP_UINT16        nPageCount;
    OP_UINT16        iCurPageIndex;
    int              iRet = TPI_WIDGET_OK;
    
    if( !pWidget )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( bEntered == OP_TRUE )
    {
        return TPI_WIDGET_OK;
    }
    bEntered = OP_TRUE;
    
    pGadget = (MsfGadget*)pWidget;
    
    nPageCount = ((MsfStringGadget *)pWidget)->nPageCount;
    iCurPageIndex = ((MsfStringGadget *)pWidget)->iCurPageIndex;

    if(nPageCount <= 1)                      /* less one screen */
    {
        /* remove scrollbar */
        if(ISVALIDHANDLE(pGadget->hVScrollBar))
        {
            if( pGadget->parent )
            {
                TPIa_widgetRemove(pGadget->parent->windowHandle, pGadget->hVScrollBar);
            }
            TPIa_widgetRelease(pGadget->hVScrollBar);
            
            pGadget->hVScrollBar = INVALID_HANDLE;

            /*
             *after del scrollbar, the related gadget size should be adjust    
             */
            size.width =  pGadget->size.width + SCROLLBAR_WIDTH;
            size.height = pGadget->size.height;
            TPIa_widgetSetSize(pGadget->gadgetHandle, &size);
            
            iRet = TPI_WIDGET_PAINTED;
        }
    }
    else                                  
    {
        if(!ISVALIDHANDLE(pGadget->hVScrollBar))   /*create a scroll bar*/
        {
            size.height = pGadget->size.height;
            size.width = SCROLLBAR_WIDTH;
            hBar = TPIa_widgetBarCreate(pGadget->modId,
                                        MsfVerticalScrollBar,
                                        nPageCount - 1,
                                        iCurPageIndex,
                                        &size,
                                        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_ALWAYSONTOP,
                                        0);
            
            if( ISVALIDHANDLE(hBar) )
            {
                pBar = (MsfBar*)seekWidget(hBar, OP_NULL );
                
                pBar->pageStep = 1;
                pBar->lineStep = 1;
                pBar->hRelatedWidget = pGadget->gadgetHandle;
                
                pGadget->hVScrollBar = hBar;  
                size.width =  pGadget->size.width - SCROLLBAR_WIDTH;

                /* before add scroll bar,the related gadget size should be ajusted*/
                size.height = pGadget->size.height;
                TPIa_widgetSetSize(pGadget->gadgetHandle, &size);

                if( pGadget->parent )
                {
                    /* get position   */
                    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y);              
                    pos.x += size.width;
                    
                    /* add to window  */
                    TPIa_widgetWindowAddGadget(pGadget->parent->windowHandle, 
                                               hBar, 
                                               &pos, 
                                               0);
                }     

                iRet = TPI_WIDGET_PAINTED;
            }                
        }
        else                                   /*set the old scroll bar value*/
        {
            TPIa_widgetBarSetValues(pGadget->hVScrollBar, iCurPageIndex,nPageCount - 1);            
        }
    }

    bEntered = OP_FALSE;
    return iRet;
}

/*==================================================================================================
    FUNCTION:  widgetFormAdjustScrollbar

    DESCRIPTION:
        Adjust the scroll bar related to the selectgroup.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  widgetFormAdjustScrollbar(MsfForm *pWidgetForm)
{
    MsfBar         *pBar =  OP_NULL;
    MsfSize        size;
    MsfGadgetHandle    hBar;
    MsfWindow     *pWindow;
    MsfPosition   clientPosition;
    MsfSize       clientSize;
    OP_UINT16     iClientYMaxValue, iClientXMaxValue;
    OP_BOOLEAN   hScrollBarFlag = OP_FALSE, vScrollBarFlag = OP_FALSE;
    int                 value, maxValue;

    if( !pWidgetForm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    pWindow = (MsfWindow *)pWidgetForm;
    WidgetGetGadgetPositionRange(pWindow, &pWidgetForm->iMaxScrollAreaX, &pWidgetForm->iMaxScrollAreaY);
    widgetGetClientRect(pWindow, &clientPosition, &clientSize);
    iClientYMaxValue = clientSize.height;
    iClientXMaxValue = clientSize.width;
    if(pWidgetForm->iMaxScrollAreaY > iClientYMaxValue)
    {
       vScrollBarFlag = OP_TRUE;
       if(pWidgetForm->iMaxScrollAreaX + SCROLLBAR_WIDTH > iClientXMaxValue)
        {
           hScrollBarFlag = OP_TRUE;
        }
    }

    if(pWidgetForm->iMaxScrollAreaX > iClientXMaxValue)
    {
        hScrollBarFlag = OP_TRUE;
        if(pWidgetForm->iMaxScrollAreaY + SCROLLBAR_WIDTH > iClientYMaxValue)
        {
            vScrollBarFlag = OP_TRUE;
        }
    }
    /* vertical scroll bar */
    if(!vScrollBarFlag)
    {
        pWindow->scrollPos.y = 0;
        
        /* remove scrollbar */
        if(OP_NULL != pWidgetForm->pVScrollBar)
        {
            TPIa_widgetRemove(pWidgetForm->windowData.windowHandle, 
            pWidgetForm->pVScrollBar->gadgetData.gadgetHandle);

            pWidgetForm->pVScrollBar = OP_NULL;
        }
    }
    else
    {
        if(OP_NULL == pWidgetForm->pVScrollBar)
        {
            
            MsfAlignment alignmentWay;
            size.height = clientSize.height ;
            size.width = SCROLLBAR_WIDTH;
            hBar = TPIa_widgetBarCreate(
                                        pWidgetForm->windowData.modId,
                                        MsfVerticalScrollBar,
                                        pWidgetForm->iMaxScrollAreaY,
                                        pWindow->scrollPos.y,
                                        &size,
                                        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_ALWAYSONTOP,
                                        0);
            
            if( ISVALIDHANDLE(hBar))
            {
                pBar = (MsfBar*)seekWidget(hBar, OP_NULL );
                pBar->pageStep = clientSize.height - GADGET_LINE_HEIGHT; 
                if(hScrollBarFlag)
                {
                    pBar->pageStep -= SCROLLBAR_WIDTH;
                }
                
                if( pBar->pageStep < GADGET_LINE_HEIGHT )
                {
                    pBar->pageStep = GADGET_LINE_HEIGHT;
                }
                
                pBar->lineStep = GADGET_LINE_HEIGHT;
                pBar->hRelatedWidget = pWidgetForm->windowData.windowHandle;
                
                pWidgetForm->pVScrollBar = pBar;  
                
                /* add to window  */
                alignmentWay.verticalPos = MsfMiddle;
                alignmentWay.horisontalPos = MsfRight;
                TPIa_widgetWindowAddGadget(
                    pWidgetForm->windowData.windowHandle, 
                    hBar, 
                    OP_NULL, 
                    &alignmentWay);
                 TPIa_widgetRelease(hBar);
            }
        }
        else
        {
            pBar = pWidgetForm->pVScrollBar;
            pBar->pageStep = clientSize.height - GADGET_LINE_HEIGHT;
            if(hScrollBarFlag)
            {
                 pBar->pageStep -= SCROLLBAR_WIDTH;
            }
            
            if( pBar->pageStep < GADGET_LINE_HEIGHT )
            {
                pBar->pageStep = GADGET_LINE_HEIGHT;
            }
            
            TPIa_widgetBarSetValues( pBar->gadgetData.gadgetHandle,pWindow->scrollPos.y, pWidgetForm->iMaxScrollAreaY);
            TPIa_widgetBarGetValues( pBar->gadgetData.gadgetHandle, &value, &maxValue);
            pWindow->scrollPos.y = value;
        }
    }

    /* horizontal scroll bar */
    
    if(!hScrollBarFlag)
    {
        pWindow->scrollPos.x = 0;
        
        /* remove scrollbar */
        if(OP_NULL != pWidgetForm->pHScrollBar)
        {
            
            TPIa_widgetRemove(pWidgetForm->windowData.windowHandle, 
                pWidgetForm->pHScrollBar->gadgetData.gadgetHandle);
            
            pWidgetForm->pHScrollBar = OP_NULL;
        }
    }
    else
    {
        if(OP_NULL == pWidgetForm->pHScrollBar)
        {

            MsfAlignment alignmentWay;
            size.height = SCROLLBAR_WIDTH;
            size.width = clientSize.width;
            if(vScrollBarFlag)
            {
                size.width -= SCROLLBAR_WIDTH;
            }
            hBar = TPIa_widgetBarCreate(
                pWidgetForm->windowData.modId,
                MsfHorizontalScrollBar,
                pWidgetForm->iMaxScrollAreaX,
                pWindow->scrollPos.x,
                &size,
                MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_ALWAYSONTOP,
                0);
            
            if( ISVALIDHANDLE(hBar))
            {
                pBar = (MsfBar*)seekWidget(hBar, OP_NULL );
                pBar->pageStep = clientSize.width - GADGET_LINE_HEIGHT;
                if(vScrollBarFlag)
                {
                    pBar->pageStep -= SCROLLBAR_WIDTH;
                }
                
                if( pBar->pageStep < GADGET_LINE_HEIGHT )
                {
                    pBar->pageStep = GADGET_LINE_HEIGHT;
                }
                
                pBar->lineStep = GADGET_COLUMN_WIDTH;
                pBar->hRelatedWidget = pWidgetForm->windowData.windowHandle;

                pWidgetForm->pHScrollBar = pBar;  

                /* add to window  */
                alignmentWay.verticalPos = MsfLow;
                alignmentWay.horisontalPos = MsfLeft;
                TPIa_widgetWindowAddGadget(
                                        pWidgetForm->windowData.windowHandle, 
                                        hBar, 
                                        OP_NULL, 
                                        &alignmentWay);
                TPIa_widgetRelease(hBar);
            }
        }
        else
        {
            pBar = pWidgetForm->pHScrollBar;
            pBar->pageStep = clientSize.width - GADGET_LINE_HEIGHT;
            if(vScrollBarFlag)
            {
                pBar->pageStep -= SCROLLBAR_WIDTH;
            }
            
            if( pBar->pageStep < GADGET_LINE_HEIGHT )
            {
                pBar->pageStep = GADGET_LINE_HEIGHT;
            }
            
            TPIa_widgetBarSetValues( pBar->gadgetData.gadgetHandle, pWindow->scrollPos.x, pWidgetForm->iMaxScrollAreaX);
            TPIa_widgetBarGetValues( pBar->gadgetData.gadgetHandle, &value, &maxValue);
            pWindow->scrollPos.x = value;
        }
    }
    return TPI_WIDGET_OK;
}


/*==================================================================================================
  FUNCTION:  WidgetGetGadgetPositionRange

  DESCRIPTION:
      Search the bottom gadget position and size, right gadget position and size,

  ARGUMENTS PASSED:
      pWindow:                            point to the window structure
      pGadgetXMaxArea:                    point to the max horizontal value 
      pGadgetYMaxArea:                    point to the max vertical value
     
  RETURN VALUE:
      
  
  IMPORTANT NOTES:
      None
==================================================================================================*/
static int WidgetGetGadgetPositionRange
(MsfWindow * pWindow,
 OP_INT16  *pGadgetXMaxArea,
 OP_INT16  *pGadgetYMaxArea)
{
   
        int     iRet = TPI_WIDGET_OK; 
        if((OP_NULL == pWindow) ||
            (OP_NULL == pWindow->children))
        {
            iRet = TPI_WIDGET_ERROR_UNEXPECTED;
        }
        else
        {
            MsfSize tempGadgetSize;
            OP_UINT16 iTempXMax, iTempYMax;
            GadgetsInWindow *pTemp;
            pTemp = pWindow->children;
            iTempXMax = 0;
            iTempYMax = 0;
            while(pTemp)
            {
                if((MsfHorizontalPosDefault ==pTemp->gadget->alignment.horisontalPos ) &&
                    (MsfVerticalPosDefault == pTemp->gadget->alignment.verticalPos))
                {
                    widgetGadgetGetSize(pTemp->gadget, &tempGadgetSize);
                    if((pTemp->gadget->position.x+ tempGadgetSize.width) > iTempXMax)
                    {
                        iTempXMax = pTemp->gadget->position.x + tempGadgetSize.width;
                    }
                    if((pTemp->gadget->position.y +tempGadgetSize.height) > iTempYMax)
                    {
                        iTempYMax = pTemp->gadget->position.y + tempGadgetSize.height;
                    }
                      
                }

                pTemp = pTemp->next;               
                
            }

            if(OP_NULL != pGadgetXMaxArea)
            {
                *pGadgetXMaxArea = iTempXMax ;
            }
            if(OP_NULL != pGadgetYMaxArea)
            {
                *pGadgetYMaxArea = iTempYMax ;
            }
                      
           
        }
        return iRet;
        
        
}





/*==================================================================================================
    FUNCTION:  widgetBitmapGadgetShow

    DESCRIPTION:

    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetBmpGadgetShow(void *pWidget, MsfWidgetType wt)
{
    MsfGadget           *pGadget;
    MsfBmpAnimation    *pBmpAniGadget;
    MsfSize              size;
    MsfPosition          pos;
    DS_COLOR          bgColor;
    RM_INT_ANIDATA_T   *pBmpAniData = OP_NULL;
    

    if( !pWidget || wt != MSF_BMPANIMATION)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    pGadget = (MsfGadget*)pWidget;
    
    /*Judge if the gadget parent  has focus  */
    if( pGadget->parent == OP_NULL )  
    {
        /* need not to draw */
        return TPI_WIDGET_OK;
    }
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        return widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pGadget->parent->windowHandle, OP_TRUE);
    }

    pBmpAniGadget = (MsfBmpAnimation *)pGadget;
    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y );
    widgetGadgetGetSize(pGadget, &size); 
    if(!(MSF_GADGET_PROPERTY_SHOW_NO_BACKGROUND & pGadget->propertyMask))
    {
        widgetGetDrawingInfo( (void*) pGadget, 
            MSF_BMPANIMATION, 
            0, 
            OP_NULL, 
            OP_NULL, 
            &bgColor);
        /*  fill the area of selectgroup with background color  */                                         
        widgetScreenFillRect( pGadget->parent, 
                              pos.x, 
                              pos.y, 
                              (OP_INT16)(pos.x + size.width -1), 
                              (OP_INT16)(pos.y + size.height-1), 
                              bgColor);
    }    
    pBmpAniData = pBmpAniGadget->pAniData;
    if(OP_NULL == pBmpAniData)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        if(ANI_ICON == pBmpAniData->image_type)/* icon */
        {
            widgetScreenDrawIcon(pGadget->parent,
                                  pos.x,
                                  pos.y,
                                  size.width,
                                  size.height,
                                  (RM_ICON_T*)(((RM_ICON_T *)pBmpAniData->scene) + pBmpAniGadget->iCurFrame));
                        
        }
        else if(ANI_BITMAP == pBmpAniData->image_type)
        {
            widgetScreenDrawBitmap(pGadget->parent,
                                  pos.x,
                                  pos.y,
                                  size.width,
                                  size.height,
                                  (RM_BITMAP_T *)(((RM_BITMAP_T *)pBmpAniData->scene) + pBmpAniGadget->iCurFrame));
        }
    }/* pAniData */
    /*                      
    if(pGadget->isFocused)
    {
        widget_draw_focus_rect(iRet, pGadget->parent, &pos, &size);
    }
    */
    return TPI_WIDGET_OK;
}

int widgetMainMenuPlayAnimation( int iPlay )
{
    MsfWindow *pWin;
    MsfMainMenuWindow *pMainMenu;

    pWin = seekFocusedWindow();
    if( pWin && getWidgetType(pWin->windowHandle) == MSF_MAINMENU)
    {
        pMainMenu = (MsfMainMenuWindow*)pWin;
        if( pMainMenu->pBmpAni )
        {
            if( iPlay)
            {
                TPIa_widgetBmpAnimationPlay(pMainMenu->pBmpAni->gadgetData.gadgetHandle, MSF_TRUE);
            }
            else
            {
                TPIa_widgetBmpAnimationStop(pMainMenu->pBmpAni->gadgetData.gadgetHandle);
            }
        }
    }

    return TPI_WIDGET_OK;
}


static int  widgetBox_scrollbarStateChangeCb
(
    MSF_UINT8 modId,
    MSF_UINT32 hBar,
    MsfNotificationType noteType,
    void* pData
)
{
    MsfWidgetType   wt;
    MsfBar              *pBar;
    MsfBox              *pBox;
    MsfGadget         *pGadget;

    if( noteType != MsfNotifyStateChange )
    {
        return TPI_WIDGET_OK;
    }
    
    pBar = (MsfBar*)seekWidget( hBar, &wt);
    if( !pBar || wt != MSF_BAR )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    if( !ISVALIDHANDLE(pBar->hRelatedWidget ))
    {
        return TPI_WIDGET_OK;
    }

    pBox = (MsfBox*)seekWidget(pBar->hRelatedWidget, &wt);
    if( !pBox || wt != MSF_BOX )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pBox->iCurPos = pBar->value;
    pGadget = (MsfGadget*)pBox;

    if( pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
    {
        if( pGadget->cbStateChange )
        {
            (void)pGadget->cbStateChange(
                                        modId,
                                        pGadget->gadgetHandle, 
                                        MsfNotifyRequestNewContent,
                                        pData );
        }
        else
        {
            TPIc_widgetNotify( modId,
                                        pGadget->gadgetHandle,
                                        MsfNotifyRequestNewContent);
        }
    }
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  widgetChoiceDrawItemByIndex

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetChoiceDrawItemByIndex
(
    MsfSelectgroup         *pSelectGroup,
    choiceElement          *pItem,
    OP_INT16              index,
    OP_BOOLEAN         bRefresh
)
{
    MsfWindow         *pWin;
    int               iRet;
    MsfWidgetType     wt;
    MsfWidgetDrawChoiceItem drawChoiceItem;
    OP_INT16          iEngCharHeight;
    if( !pSelectGroup )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( pSelectGroup->gadgetData.isFocused == OP_FALSE )
    {
        return TPI_WIDGET_OK;
    }
    
    pWin = pSelectGroup->gadgetData.parent;
    
    if( ds_get_screen_mode() != SCREEN_OWNERDRAW )
    {
        widgetRedraw(OP_NULL, MSF_MAX_WIDGETTYPE, pWin->windowHandle, OP_TRUE);
        return CHOICE_REDRAW_ALL_ITEMS;
    }

    wt = getWidgetType(pSelectGroup->gadgetData.gadgetHandle);
    
    /* partition the item area into several part, according to the bitmask of selectgroup  */
    iRet = widgetChoicePartitionItemArea( pSelectGroup, &drawChoiceItem.part );
    if( iRet < 0 )
    {
        return iRet;
    }

    /* get the item rect */
    iRet = widgetChoiceGetItemRect( pSelectGroup, index, &drawChoiceItem.pos, &drawChoiceItem.size);
    if( iRet < 0 )
    {
        return iRet;
    }

    /* calc the item  */
    if( !pItem )
    {
        drawChoiceItem.pItem = getChoiceElementByIndex( &pSelectGroup->choice, (int)index);
        if( !drawChoiceItem.pItem )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    else
    {
        drawChoiceItem.pItem = pItem;
    }

    /* get the font attribute and font color of the select group */
    widgetGetDrawingInfo( (void*) pSelectGroup, 
                                        wt, 
                                        0, 
                                        &drawChoiceItem.fontAttr, 
                                        &drawChoiceItem.fontColor, 
                                        &drawChoiceItem.bgColor);
                                        
#ifdef MENU_ITEM_STRING_USE_LARGE_FONT
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU)
    {
        /* menu style:  use large size  font  */
        drawChoiceItem.fontAttr &= ~FONT_SIZE_SMALL;
        drawChoiceItem.fontAttr |= FONT_SIZE_MED;
    }
#endif
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU)
    {
#if(PROJECT_ID == ODIN_PROJECT)
        drawChoiceItem.fontColor = ds.cComposer.menuFontColor;
#endif
    }
    drawChoiceItem.fontAttr |= FONT_OVERLAP;    
    
    /* calc the english char width  */
    if( 0 > widgetGetFontSizeInEngChar(drawChoiceItem.fontAttr , &drawChoiceItem.iEngCharWidth, &iEngCharHeight))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    drawChoiceItem.index = index - pSelectGroup->choice.top_item;
    drawChoiceItem.bChoiceFocus = OP_TRUE;

    /*  fill the area of selectgroup with background color  */ 
#if 0    
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MAIN_MENU )
    {
        if( RM_IsItBitmap(BMP_SUBMENULINE_BG) )
        {
            widgetScreenDrawBitmapRm(
                                pWin, 
                                drawChoiceItem.pos.x, 
                                drawChoiceItem.pos.y, 
                                drawChoiceItem.size.width,
                                drawChoiceItem.size.height,
                                BMP_SUBMENULINE_BG);            
        }
        else if( RM_IsItIcon(BMP_SUBMENULINE_BG) )
        {
            widgetScreenDrawIconRm(
                                pWin, 
                                drawChoiceItem.pos.x, 
                                drawChoiceItem.pos.y, 
                                drawChoiceItem.size.width,
                                drawChoiceItem.size.height,
                                BMP_SUBMENULINE_BG);            
        }
    }
#endif
    if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU)
    {
#if(ODIN_PROJECT == PROJECT_ID)
        /* calc if draw focus item with full focus rect or top and bottom lines */
        if( pSelectGroup->showStyle & SG_SHOW_STYLE_FOCUS_DRAW_LINE )
        {
            drawChoiceItem.bFullFocusRect = OP_FALSE;
        }
        else
        {
            drawChoiceItem.bFullFocusRect = OP_TRUE;
        }
        
        widgetScreenFillRect( pWin, 
            drawChoiceItem.pos.x, 
            drawChoiceItem.pos.y, 
            (OP_INT16)(drawChoiceItem.pos.x + drawChoiceItem.size.width -1), 
            (OP_INT16)(drawChoiceItem.pos.y + drawChoiceItem.size.height-1), 
            drawChoiceItem.bgColor);
#elif( (PROJECT_ID == WHALE1_PROJECT )\
      ||(PROJECT_ID == WHALE2_PROJECT)\
      ||(PROJECT_ID == NEMO_PROJECT)\
      ||(PROJECT_ID == KNIGHT_PROJECT)\
      ||(PROJECT_ID == DRAGON_S_PROJECT) )
        /* fill background  */
        widgetScreenDrawBitmapRm((MsfWindow *)pSelectGroup->gadgetData.parent, 
                                drawChoiceItem.pos.x, 
                                drawChoiceItem.pos.y, 
                                drawChoiceItem.size.width,
                                drawChoiceItem.size.height,
                                MENU_BG_RES_WITH_COLOR_SCHEME );
#else
        MsfSize bgSize;
        RM_RESOURCE_ID_T  itemBgId;

        if( COLOR_THEME_USER_DEFINE != CUR_COLOR_THEME )
        {
            if( pSelectGroup->gadgetData.hVScrollBar )
            {
                itemBgId = MENU_ITEM_BG_PIC_VS_SB;
            }
            else
            {
                itemBgId = MENU_ITEM_BG_PIC;
            }
        }
        else
        {
            if( pSelectGroup->gadgetData.hVScrollBar )
            {
                itemBgId = MENU_ITEM_BG_PIC_VS_SB_BY_USER;
            }
            else
            {
                itemBgId = MENU_ITEM_BG_PIC_BY_USER;
            }
        }
        
        widgetGetImageSize(itemBgId, &bgSize);
        if( RM_IsItBitmap(itemBgId) )
        {
            widgetScreenDrawBitmapRm(
                                pWin, 
                                drawChoiceItem.pos.x, 
                                drawChoiceItem.pos.y, 
                                (OP_INT16)((bgSize.width < drawChoiceItem.size.width)?(bgSize.width):(drawChoiceItem.size.width)),
                                (OP_INT16)((bgSize.height < drawChoiceItem.size.height)?(bgSize.height):(drawChoiceItem.size.height)),
                                itemBgId);            
        }
        else if( RM_IsItIcon(itemBgId) )
        {
            widgetScreenDrawIconRm(
                                pWin, 
                                drawChoiceItem.pos.x, 
                                drawChoiceItem.pos.y, 
                                (OP_INT16)((bgSize.width < drawChoiceItem.size.width)?(bgSize.width):(drawChoiceItem.size.width)),
                                (OP_INT16)((bgSize.height < drawChoiceItem.size.height)?(bgSize.height):(drawChoiceItem.size.height)),
                                itemBgId);            
        }
#endif
    }
    else
    {
        /* calc if draw focus item with full focus rect or top and bottom lines */
        if( pSelectGroup->showStyle & SG_SHOW_STYLE_FOCUS_DRAW_LINE )
        {
            drawChoiceItem.bFullFocusRect = OP_FALSE;
        }
        else
        {
            drawChoiceItem.bFullFocusRect = OP_TRUE;
        }
#if  (PROJECT_ID == VIOLET1_PROJECT)  
 if( pSelectGroup->showStyle & SG_SHOW_STYLE_MENU)
     {
        widgetScreenFillRect( pWin, 
                              drawChoiceItem.pos.x+8, 
                              drawChoiceItem.pos.y, 
                              (OP_INT16)(drawChoiceItem.pos.x + drawChoiceItem.size.width -1-8), 
                              (OP_INT16)(drawChoiceItem.pos.y + drawChoiceItem.size.height-1), 
                              drawChoiceItem.bgColor);
      }else
      {
        widgetScreenFillRect( pWin, 
                              drawChoiceItem.pos.x, 
                              drawChoiceItem.pos.y, 
                              (OP_INT16)(drawChoiceItem.pos.x + drawChoiceItem.size.width -1), 
                              (OP_INT16)(drawChoiceItem.pos.y + drawChoiceItem.size.height-1), 
                              drawChoiceItem.bgColor);      
      }
#else
       widgetScreenFillRect( pWin, 
                              drawChoiceItem.pos.x, 
                              drawChoiceItem.pos.y, 
                              (OP_INT16)(drawChoiceItem.pos.x + drawChoiceItem.size.width -1), 
                              (OP_INT16)(drawChoiceItem.pos.y + drawChoiceItem.size.height-1), 
                              drawChoiceItem.bgColor);
#endif                              
    }
    
    /* draw item */
    widgetChoiceDrawItem( pSelectGroup, &drawChoiceItem);

    if( bRefresh == OP_TRUE )
    {
        WAP_REFRESH
    }

    return TPI_WIDGET_OK;
}

OP_BOOLEAN widgetGetClientVisibleRect(const MsfWindow* pWin, MsfPosition  *pStart, MsfSize *pSize)
{
    MsfPosition    clientPos;
    MsfSize         clientSize;
    OP_INT16      vRectLeft;
    OP_INT16      vRectTop;
    OP_INT16      vRectRight;
    OP_INT16      vRectBottom;
    
    OP_INT16      clientLeft;
    OP_INT16      clientTop;
    OP_INT16      clientRight;
    OP_INT16      clientBottom;
    OP_BOOLEAN  bVisible;
    
    if( !pWin ||!pStart ||!pSize )
    {
        return OP_FALSE;
    }
    
    widgetPointScreenToClient( pWin, 0, 0, &vRectLeft, &vRectTop );
    vRectRight = vRectLeft + DS_SCRN_MAX_X -1;
    vRectBottom = vRectTop + WAP_SCRN_TEXT_HIGH - 1;
    
    widgetGetClientRect(pWin, &clientPos, &clientSize);
    clientLeft = clientPos.x;
    clientTop = clientPos.y;
    clientRight = clientLeft + clientSize.width -1;
    clientBottom = clientTop + clientSize.height -1;
    
    bVisible = widgetRectGetVisible( 
                                vRectLeft,
                                vRectTop,
                                vRectRight, 
                                vRectBottom, 
                                &clientLeft, 
                                &clientTop, 
                                &clientRight, 
                                &clientBottom);
    if( bVisible == OP_TRUE )
    {
        pStart->x = clientLeft;
        pStart->y = clientTop;
        pSize->width = clientRight - clientLeft + 1;
        pSize->height= clientBottom - clientTop + 1;
    }

    return bVisible;
}


int WidgetRefresh()
{   
    ds_refresh_for_wap_mms();
    if(bAnnuUpdate)
    {
       widgetDrawAnnunciator();
       bAnnuUpdate = MSF_FALSE;
    }
    return TPI_WIDGET_OK;
}

static int widgetDialogOnTimer( MSF_UINT32 hDlg, MSF_UINT32 iTimerId)
{
    widget_state_change_callback_t  fnStateChange;
    MSF_UINT8      modId;
    MsfWindow      *pWin;
    MsfDialog          *pDlg;
    MsfWidgetType wt;

    if( IS_WIDGET_TIMER_ID(iTimerId))
    {
        TPIa_widgetStopTimer( hDlg, iTimerId);
    }
    
    pWin = (MsfWindow*)seekWidget(hDlg, &wt);
    if( !pWin || wt != MSF_DIALOG )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pDlg = (MsfDialog*)pWin;
    
    modId = pWin->modId;
    fnStateChange = pWin->cbStateChange;
    pDlg->iTimerId = WIDGET_TIMER_INVALID_ID;

    if( pWin->parent != OP_NULL )
    {
        TPIa_widgetRemove(pWin->parent->screenHandle, hDlg );
    }

    if( fnStateChange )
    {
        (void)fnStateChange( modId, hDlg, MsfNotifyLostFocus, OP_NULL);
    }
    else
    {
        /*  inform AU that dialog is closed  */ 
        TPIc_widgetNotify( modId, hDlg, MsfNotifyLostFocus);
    }

    return TPI_WIDGET_OK;
}    


/*==================================================================================================
    FUNCTION:  widgetPaintBoxRedrawForImm

    DESCRIPTION:

        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetPaintBoxRedrawForImm( MsfWindow    *pPaintBox, OP_BOOLEAN bRefresh)
{
    if( !pPaintBox || getWidgetType(pPaintBox->windowHandle) != MSF_PAINTBOX || pPaintBox->isFocused == OP_FALSE)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( restoreDisplay( OP_FALSE,  pPaintBox) == OP_FALSE )
    {
#if 0    
        TPIc_widgetNotify( pPaintBox->modId, pPaintBox->windowHandle, MsfNotifyMoveResize );
#else
        TPIc_widgetNotify( pPaintBox->modId, pPaintBox->windowHandle, MsfNotifyFocus);
#endif
    }
    else 
    {
        GadgetsInWindow  *pGadgetNode;
        MsfWidgetImm  *pImm;
        MsfWidgetType wt;
        MsfGadget       *pTextInput, *pGadget;
        
        widgetWindowDrawAllGadgets(pPaintBox);
        
        /* play image (gif animation)  */
        pGadgetNode = pPaintBox->children;
        while( pGadgetNode )
        {
            pGadget = pGadgetNode->gadget;
            if(pGadget && getWidgetType(pGadget->gadgetHandle) == MSF_IMAGEGADGET )
            {
                TPIa_widgetImagePlay(pGadget->gadgetHandle);
            }

            pGadgetNode = pGadgetNode->next;
        }
        
        pImm = pPaintBox->parent ->pImm;
        if( pImm )
        {
            pTextInput = (MsfGadget*)seekWidget(pImm->hInput, &wt);
//            widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, INVALID_HANDLE, OP_FALSE );

            if( pTextInput->isFocused == OP_TRUE && pImm->bOpen == OP_TRUE)
            {
                widgetImmShow( pImm, OP_TRUE );
            }
        }

    }

    {  /* draw the title of the form below the paintbox */
        MsfWindow  *pWin;
        MsfScreen   *pScreen = pPaintBox->parent;
        
        if( pScreen && pScreen->children && pScreen->children->next )
        {
            /* get the form */
            pWin = pScreen->children->next->window;

            widgetWindowDrawTitle( pWin, OP_NULL, OP_FALSE );
        }
    }
    
    widgetWindowShowSoftKey( pPaintBox, bRefresh);
    
    if( bRefresh== OP_TRUE )
    {
        WAP_REFRESH
    }
    return TPI_WIDGET_OK;
}


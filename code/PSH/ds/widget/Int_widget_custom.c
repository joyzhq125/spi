/*==================================================================================================

    MODULE NAME : int_widget_custom.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    23/05/2003   zhuxq             Cxxxxxx     Initial file creation.
    7/30/2003    Zhuxq             P001026     Modify the default window size 
    07/26/2003   lindawang         P001074     Fix draw image problems.
    04/08/2003   Zhuxq             P001052     Add the default propertyMask
    08/13/2003   Zhuxq             P001147     Change the selectgroup show and menu show
    8/11/2003    Chenjs            P001091     Update text input widget.
    08/20/2003   Zhuxq             P001279     modify the position of selectgroup item
    8/18/2003    Zhuxq             P001263     Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    08/14/2003   lindawang         P001254     Fix draw image problem for wap.
    08/20/2003   lindawang         P001308     Add stopGifTimer() and startGifTimer()  
    08/23/2003   lindawang         P001350     solove the MMS Gif stop problem.
    08/25/2003   Steven Lai        P001355     Fixed some warning according to PC-Lint check result
    08/27/2003   Zhuxq             P001383     Fixed some bugs
    08/28/2003   Tree Zhan         P001419     Fix gif not release problem
    08/30/2003   Zhuxq             P001439     Fix the bug: when opens the MMS View Message Window, the center softkey shows incorrect.
    09/25/2003   Zhuxq             P001696     Re-implement the dialog
    09/26/2003   linda wang        P001451     separate the gif play and show function.    
    10/14/2003   linda wang        C001806     Add function of setting download image to desktop
    10/17/2003   Zhuxq             C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq             P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq             P001883     Optimize the redraw strategy to reduce the redraw times
    12/03/2003   Zhuxq             P002139     Enhance the sensitivity of scrollbar in WAP&MMS
    01/05/2004   Zhuxq             C000072     Add callback function to each widget in the widget system
    01/15/2004   Zhuxq             P002352     All editing actions of MsfTextInput turns to MsfEditor
    02/16/2004   Zhuxq             P002469     Add button gadget to widget system
    02/16/2004   chenxiao          p002474     add bitmap animation function in the dolphin project    
    02/17/2004   Zhuxq             P002483     Adjust the display system of widget to show up the annunciator and fix some bugs
    02/17/2004   Zhuxq             P002492     Add Viewer window to widget system
    02/19/2004   Zhuxq             P002504     Add box gadget to widget system
    02/20/2004   Zhuxq             P002517     Adjust the handle order of pen events in widget system
    02/24/2004   chenxiao          p002530     modify WAP & MSG main menu     
    02/28/2004   Zhuxq             P002556     Fix some widget bugs
    03/19/2004   Zhuxq             P002687     Fix some bugs in widget system
    03/19/2004   Chenxiao          p002688     change main menu  of wap and fix some bugs from NEC feedback     
    03/26/2004   dingjianxin       P002754     delete setStopGifTimerFlag and getStopGifTimerFlag for optimize the image play
    03/30/2004   Chenxiao          p002758     fix bugs for bookmark and color scheme in the wap module
    04/02/2004   zhuxq             P002789     Fix UMB bugs
    04/09/2004   zhuxq             P002858     Fix UMB bugs
    04/15/2004   zhuxq             P002977     Fix some bugs in UMB for Dolphin
    04/14/2004   Dingjianxin       p002961     add propertyMask value to Msf_TextInput delete the conditional compiler about ALL_TEXT_EDIT_GOTO_EDITOR
    04/24/2004   zhuxq             P003022     Fix some bugs in UMB
    04/29/2004   zhuxq             P005196     Fix some bugs in UMB
    04/29/2004   chenxiao          p005177     fix some umb and widget bugs  
    05/25/2004   chenxiao          p005637     change image interface with DS     
    05/24/2004   zhuxq             P005568     Adjust UI implementation in UMB and fix some bugs
    06/04/2004   zhuxq             P005925     Correct issues found by PC-lint and fix some bugs
    06/09/2004   Jianghb           P006036     Add WHALE1,WHALE2 and NEMO compilerswitch to some module
    06/09/2004   zhuxq             P006048     forbid Menu to respond successive multiple actions
    06/09/2004   zhuxq             P006106     Fix bugs in UMB and WAP found in pretest for TF_DLP_VER_01.24    
    06/18/2004   zhuxq             P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/01/2004   zhuxq             P006630     Fix bugs that the position of image can't be center
    07/05/2004   zhuxq             P006708     Fix  bugs: CNXT00007777, CNXT00004950, CNXT00009472, CNXT00008770, CNXT00008391, CNXT00009745
    07/15/2004   zhuxq             P007016     Fix the bug that paintbox can't be updated with imm 
    07/20/2004   zhuxq             P007108     Add playing GIF animation in some wap page with multi-frame GIF
    08/06/2004   zhuxq             P007577     adjust the layout of MMS/SMS for B1,  and fix some bugs
    08/07/2004   Hugh zhang        p007279     Add progress bar when sending or downloading mms.
    08/09/2004   Hugh zhang        p007607     Change font of widget dialog in lotus project.
    08/18/2004   zhuxq             P007793     Trace widget memory issues, and reduce allocation times of MsfString
    08/18/2004   zhuxq             P007965     Fix the bug that browser can't open any page after enter some wap page
    08/30/2004   zhuxq             c007998     improve the implementation of predefined string to reduce the times of memory allocation
    09/03/2004   Hugh zhang        p001333     Modify menu display of widget in lotus project.
    09/07/2004   Hover             P008175     change project switch for dragon_m check obx icon
    
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
#include "gsm_timer.h"
#include "msf_int.h"
#include "msf_int_widget.h"
#include "Ds_drawing.h"
#include "Int_widget_common.h"
#include "Int_widget_custom.h"
#include "int_widget_imm.h"
#include "int_widget_show.h"

#include "gif_to_bmp.h"
#include "Msf_rc.h"

 /*  the number of  color themes    */

#define   DEFAULT_WINDOW_WIDTH   (DS_SCRN_MAX_X)
#define   DEFAULT_WINDOW_HEIGHT  (WAP_SCRN_TEXT_HIGH)
#ifndef DEFAULT_DIALOG_FONT_SIZE
    #define DEFAULT_DIALOG_FONT_SIZE DEFAULT_FONT_SIZE
#endif


const MsfLineStyle DEFAULT_STYLE_LINESTYLE = {1, MsfNone};
const MsfFont DEFAULT_STYLE_FONT = {MsfFontNormal,DEFAULT_FONT_SIZE, 0 ,0,0,0};
const MsfFont DEFAULT_MENU_STYLE_FONT = {MsfFontNormal,DEFAULT_MENU_FONT_SIZE, 0 ,0,0,0};
const MsfFont DEFAULT_TEXTINPUT_STYLE_FONT = {MsfFontNormal, DEFAULT_TEXT_INPUT_FONT_SIZE, 0 ,0,0,0};
const MsfFont DEFAULT_DIALOG_STYLE_FONT = {MsfFontNormal,DEFAULT_DIALOG_FONT_SIZE, 0 ,0,0,0};
const MsfTextProperty  DEFAULT_STYLE_TEXTPROPERTY = {0,0,1,1,0,{(MsfVerticalPos)0,(MsfHorizontalPos)0}};
const MsfPattern DEFAULT_STYLE_FOREGROUND = 
{
    (MsfImageHandle)0,
    {(MsfVerticalPos)0, (MsfHorizontalPos)0},
    MsfNoPadding
};
const MsfPattern DEFAULT_STYLE_BACKGROUND = 
{
    0,
    {(MsfVerticalPos)0, (MsfHorizontalPos)0},
    MsfNoPadding
};
static OP_BOOLEAN  bDisplayBackup = OP_FALSE; /*  indicate if the display is backup */
/*
  *  NOTE:  
  *      COLOR_THEME_1         --  Blue
  *      COLOR_THEME_2         --  Pink
  *      COLOR_THEME_3         --  Voilet
  */

const  DS_COLOR  DEFAULT_INPUT_INITSTRING_BG_COLORS = _RGB(164, 224, 253);

const  DS_COLOR  DEFAULT_INPUT_FOCUS_BG_COLORS = _RGB( 255, 255, 255 );

const  DS_COLOR  DEFAULT_FOCUS_BORDER_COLORS = COLOR_MANGANESE_BLUE;

const  DS_COLOR  DEFAULT_FOCUS_BG_COLORS = COLOR_WHITE_MANGANESE_BLUE;

const  DS_COLOR  DEFAULT_SCROLLBAR_FG_COLORS = _RGB( 164, 224, 253);

const  DS_COLOR  DEFAULT_SCROLLBAR_BG_COLORS = _RGB( 205, 238, 254);

const MsfColor  DEFAULT_FG_COLORS = { 0, 0, 0 };

const MsfColor  DEFAULT_BG_COLORS = { 255, 255, 255 };

const MsfColor  DEFAULT_STRING_BG_COLORS = { 255, 255, 255 };

const  MsfColor  DEFAULT_BAR_FG_COLORS = { 164, 224, 253 };

const MsfColor  DEFAULT_INPUT_BG_COLORS = { 255, 255, 255 };

const MsfColor  DEFAULT_PAINTBOX_BG_COLORS = { 255, 255, 255 };

const MsfColor  DEFAULT_MENU_BG_COLORS = { 255, 255, 255 };

const MsfColor  DEFAULT_DIALOG_FG_COLORS ={ 10, 80, 139 };

const MsfDefaultProperties  DefaultPropTab[] =
{
    /* MSF_SCREEN   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, OP_NULL, OP_NULL, 0 },
    /* MSF_PAINTBOX   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_PAINTBOX_BG_COLORS,
        MSF_WINDOW_PROPERTY_BORDER |MSF_WINDOW_PROPERTY_SINGLEACTION
    },
    /* MSF_FORM   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_WINDOW_PROPERTY_BORDER |MSF_WINDOW_PROPERTY_SINGLEACTION
    },
    /* MSF_DIALOG   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_DIALOG_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_WINDOW_PROPERTY_BORDER |MSF_WINDOW_PROPERTY_SINGLEACTION
    },
#if  (PROJECT_ID == B1_PROJECT )
    /* MSF_MENU   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_MENU_BG_COLORS, &DEFAULT_MENU_BG_COLORS,
        MSF_WINDOW_PROPERTY_SINGLEACTION |MSF_WINDOW_PROPERTY_TITLE
    },
    /* MSF_MAINMENU   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_MENU_BG_COLORS, &DEFAULT_MENU_BG_COLORS,
        0
    },
#else    
    /* MSF_MENU   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_MENU_BG_COLORS,
        MSF_WINDOW_PROPERTY_SINGLEACTION |MSF_WINDOW_PROPERTY_TITLE
    },
    /* MSF_MAINMENU   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_MENU_BG_COLORS,
        0
    },
#endif
    /* MSF_VIEWER  */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_INPUT_BG_COLORS,
        MSF_WINDOW_PROPERTY_SINGLEACTION |MSF_WINDOW_PROPERTY_TITLE
    },
    
    /* MSF_WAITINGWIN  */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_INPUT_BG_COLORS,
        0
    },
    
    /* MSF_EDITOR   */
    { {0,0}, {DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_INPUT_BG_COLORS,
        MSF_WINDOW_PROPERTY_BORDER |MSF_WINDOW_PROPERTY_SINGLEACTION |MSF_WINDOW_PROPERTY_TITLE
    },
    /* MSF_SELECTGROUP   */
    { {2,2}, {DEFAULT_CHOICE_HEIGHT, DEFAULT_GADGET_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_FOCUS 
    },
    /* MSF_DATETIME   */
    { {2,2}, {DEFAULT_DATETIMEGADGET_HEIGHT, DEFAULT_GADGET_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_FOCUS 
    },
#ifndef _KEYPAD
    /* MSF_TEXTINPUT   */
    { {2,2}, {DEFAULT_GADGET_HEIGHT, DEFAULT_GADGET_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_INPUT_BG_COLORS,
        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_FOCUS |MSF_GADGET_PROPERTY_PEN_CLICK|MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM
    },
#else
    /* MSF_TEXTINPUT   */
    { {2,2}, {DEFAULT_GADGET_HEIGHT, DEFAULT_GADGET_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_INPUT_BG_COLORS,
        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_FOCUS |MSF_GADGET_PROPERTY_TEXTINPUT_NEED_IMM
    },
#endif 
    /* MSF_STRINGGADGET   */
    { {2,2}, {DEFAULT_GADGET_HEIGHT, DEFAULT_GADGET_WIDTH - 6}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_GADGET_PROPERTY_LABEL
    },
    /* MSF_IMAGEGADGET   */
    { {2,2}, {DEFAULT_IMAGEGADGET_HEIGHT, DEFAULT_IMAGEGADGET_HEIGHT}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_GADGET_PROPERTY_FOCUS 
    },
    /* MSF_BUTTON   */
    { {2,2}, {DEFAULT_GADGET_HEIGHT, DEFAULT_BUTTON_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_GADGET_PROPERTY_FOCUS | MSF_GADGET_PROPERTY_NOTIFY | MSF_GADGET_PROPERTY_PEN_CLICK
    },
    /* MSF_BMPANIMATION   */
    { {2,2}, {DEFAULT_BMPANIMATION_HEIGHT, DEFAULT_BMPANIMATION_HEIGHT}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        0 
    },
    /* MSF_BOX   */
    { {2,2}, {DEFAULT_CHOICE_HEIGHT, DEFAULT_GADGET_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_GADGET_PROPERTY_BORDER | MSF_GADGET_PROPERTY_FOCUS 
    },
    /* MSF_BAR   */
    { {2,2}, {DEFAULT_GADGET_HEIGHT, DEFAULT_GADGET_WIDTH}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS,
        MSF_GADGET_PROPERTY_BORDER 
    },
    /* MSF_IMAGE   */
    { {0,0}, {DEFAULT_IMAGEGADGET_HEIGHT, DEFAULT_IMAGEGADGET_HEIGHT}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS, 0 },
    /* MSF_STRING   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_STRING_BG_COLORS, 0 },
    /* MSF_ICON   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS, 0 },
    /* MSF_SOUND   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS, 0 },
    /* MSF_STYLE   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_STRING_BG_COLORS, 0 },
    /* MSF_COLORANIM   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS, 0 },
    /* MSF_MOVE   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS, 0 },
    /* MSF_ROTATION   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS, 0 },
    /* MSF_ACTION   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_BG_COLORS, MSF_ACTION_PROPERTY_ENABLED },
    
    /* MSF_STRING_RES   */
    { {0,0}, {0, 0}, &DEFAULT_FG_COLORS, &DEFAULT_STRING_BG_COLORS, 0 },
    /* MSF_MAX_WIDGETTYPE   */
    { {0,0}, {0, 0},OP_NULL, OP_NULL, 0 },
};

#ifdef  _KEYPAD
const OP_UINT32 gadgetSoftKeyIDs[][3]=
{
    /*
      *  left yey, center key, right key
      */
      
    /* MSF_SELECTGROUP    */
    { 0, MSF_STR_ID_SELECT, 0 },
    /* MSF_DATETIME          */
    { 0, 0, 0 },
    /* MSF_TEXTINPUT         */
    { 0, 0, 0 },   
    /* MSF_STRINGGADGET  */
    { 0, 0, 0 },
    /* MSF_IMAGEGADGET    */
    { 0, 0, 0 },
    /* MSF_BUTTON            */
    { 0, 0, 0 },
    /* MSF_BMPANIMATION   */
    { 0, 0, 0 },
    /* MSF_BOX   */
    { 0, 0, 0 },
    /* MSF_BAR                  */
    { 0, 0, 0 }
    
};
#endif


/* Button's Normal background  */
const RM_RESOURCE_ID_T ButtonNormalBackgroundRes = BMP_UMB_BUTTON_IDLE;

/* Button's Focus background */
const RM_RESOURCE_ID_T ButtonFocusBackgroundRes = BMP_UMB_BUTTON_SELECTED;

typedef struct{
    OP_INT16    left;
    OP_INT16    top;
    OP_INT16    width;
    OP_INT16    height;
    OP_INT16    bmpStartX;
    OP_INT16    bmpStartY;
}MsfWidgetDrawPictureInfo;



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

extern int widgetGetClientRect(const MsfWindow* pWin, MsfPosition  *pStart, MsfSize *pSize);

extern OP_BOOLEAN widgetLineGetVisible
(
    OP_INT16           rectleft,
    OP_INT16           recttop,
    OP_INT16           rectright,
    OP_INT16           rectbottom,
    OP_INT16           *left,
    OP_INT16           *top,
    OP_INT16           *right,
    OP_INT16           *bottom
);

extern DS_COLOR get_pixel
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           pos_x,
    OP_INT16           pos_y
);

extern int  widgetPointScreenToClient
( 
    const MsfWindow   *pWin, 
    const OP_INT16  screen_x,  
    const OP_INT16  screen_y,  
    OP_INT16           *pClient_x,
    OP_INT16           *pClient_y
);

extern void widgetBarAdjustValue( MsfBar *pBar );

extern int widgetGadgetGetPosition(const MsfGadget* pGadget,  OP_INT16  *pX, OP_INT16  *pY);

extern int widgetPaintBoxScroll
(
    MsfWindow    *pPaintBox,
    OP_INT16     iDeltaX,
    OP_INT16     iDeltaY
);

extern void HDIc_widgetNotify(MSF_UINT8 modId, MSF_UINT32 msfHandle, MsfNotificationType notificationType);

static int widget_draw_bitmap_image
(
     RM_BITMAP_T*    bitmap,
     MsfPosition*      dest_position,
     MsfSize*         dest_size,
     MsfPosition*      src_position,
     MsfImageZoom    imageZoom
);

static OP_UINT8 *malloc_for_image_parsedata
(
    MsfImageFormat    format, 
    MsfSize            size
);

static int ParserImageData
(
    RM_BITMAP_T *pBitmap,
    MsfImage    *pImage
);


int fnTimerCallBack(MsfGadgetHandle handle, OP_UINT32 TimerId);
/*==================================================================================================
    FUNCTION:  widgetPictureAdaptPositionSize

    DESCRIPTION:
        Adjust the image or icon layout , make it to be shown at center
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetPictureAdaptPositionSize
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                      width,
    OP_INT16                      height,
    OP_INT16                      bmpWidth,
    OP_INT16                      bmpHeight,
    MsfWidgetDrawPictureInfo  *pPicInfo
)
{
    OP_INT16       screen_x;
    OP_INT16       screen_y;
    OP_INT16       left;
    OP_INT16       top;
    OP_INT16       right;
    OP_INT16       bottom;

    MsfPosition     pos;
    MsfSize           size;
    OP_BOOLEAN bVisible;
    
    OP_INT16       vRectLeft;
    OP_INT16       vRectTop;
    OP_INT16       vRectRight;
    OP_INT16       vRectBottom;
    
    if( !pWin || !pPicInfo )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    /* get the client area  */
    widgetGetClientRect( pWin, &pos, &size );
    
    /* translate the coordinates into screen coordinates */
    widgetPointClientToScreen( pWin, pos.x, pos.y, &vRectLeft, &vRectTop );
    vRectRight = vRectLeft + size.width -1;
    vRectBottom = vRectTop + size.height -1;

    /* calc the intersection of client area and screen visible area  */
    bVisible = widgetRectGetVisible( (OP_INT16) 0, 
                                                          (OP_INT16) 0, 
                                                          (OP_INT16) ( DS_SCRN_MAX_X-1), 
                                                          (OP_INT16) ( WAP_SCRN_TEXT_HIGH -1), 
                                                          &vRectLeft,
                                                          &vRectTop,
                                                          &vRectRight,
                                                          &vRectBottom);
    /*  There exists a visible part of image    */                                                          
    if( bVisible == OP_TRUE )
    {
        /*  if width is 0, means to use image size */
        if( width == 0 )
        {
            width = bmpWidth;
        }
        /* the size of destined area bigger than image size  */
        else if( width > bmpWidth )  
        {
            if( bmpWidth > 0 )
            {
                x += (width - bmpWidth) / 2;
                width = bmpWidth;
            }
        }

        /*  if height is 0, means to use image size */
        if( height == 0 )
        {
            height = bmpHeight;
        }
        /* the size of destined area bigger than image size  */
        else if( height > bmpHeight )
        {
            if( bmpHeight > 0 )
            {
                y += (height - bmpHeight) / 2;
                height = bmpHeight;
            }
        }
        /* the size of destined area smaller than image size  */

        widgetPointClientToScreen( pWin, x, y, &screen_x, &screen_y);
        
        left = screen_x;
        top = screen_y;
        right = left + width -1;
        bottom = top + height -1;

        /* Calc the visible part of the image  */
        bVisible = widgetRectGetVisible( 
                                      vRectLeft,
                                      vRectTop,
                                      vRectRight,
                                      vRectBottom,
                                      &left,
                                      &top,
                                      &right,
                                      &bottom);

        if( bVisible == OP_TRUE )
        {
            pPicInfo->bmpStartX = left - screen_x;
            pPicInfo->bmpStartY = top - screen_y;
            pPicInfo->left = left;
            pPicInfo->top = top;
            pPicInfo->width = (right - left) + 1;
            pPicInfo->height = (bottom - top) + 1;

            return 1;
        }
    }

    return 0;
}

int widgetScreenDrawLine
(
    const MsfWindow   *pWin,
    OP_INT16 start_x,
    OP_INT16 start_y,
    OP_INT16 end_x,
    OP_INT16 end_y,
    DS_COLOR color
)
{
    MsfPosition  clientStart;
    MsfSize        winSize;
    OP_BOOLEAN bVisible;
    OP_INT16    pt1_x;
    OP_INT16    pt1_y;
    OP_INT16    pt2_x;
    OP_INT16    pt2_y;

    if( !pWin  )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    /*  
      *  translate the coordinates of the whole screen visible area into client coordinates 
      */
    widgetPointScreenToClient( pWin, 
                                                   0, 
                                                   0, 
                                                   &pt1_x, 
                                                   &pt1_y );

    pt2_x = pt1_x + DS_SCRN_MAX_X-1; 
    pt2_y = pt1_y + WAP_SCRN_TEXT_HIGH  -1;

    /* get window client area  */
    widgetGetClientRect( pWin, &clientStart , &winSize);

    /* calc the intersection of client area and screen visible area */
    bVisible = widgetRectGetVisible(
                                                   clientStart.x,
                                                   clientStart.y,
                                                   (OP_INT16)(clientStart.x + winSize.width - 1),
                                                   (OP_INT16)(clientStart.y + winSize.height- 1),
                                                   &pt1_x,
                                                   &pt1_y,
                                                   &pt2_x,
                                                   &pt2_y);

    if( bVisible )
    {
        /* calc the visible part of the line   */
        bVisible = widgetLineGetVisible( pt1_x, 
                                                              pt1_y, 
                                                              pt2_x, 
                                                              pt2_y, 
                                                          &start_x, 
                                                          &start_y, 
                                                          &end_x, 
                                                          &end_y);
        if( bVisible == OP_TRUE )
        {
                /*  translate the coordinates of the visible line into screen coordinates  */
            widgetPointClientToScreen(pWin, start_x, start_y, &pt1_x, &pt1_y);
            widgetPointClientToScreen(pWin, end_x, end_y,  &pt2_x,  &pt2_y);
        
            /* draw the line  */
            ds_draw_line_forward(pt1_x, pt1_y, pt2_x, pt2_y, color);
        }
    }
    
    return HDI_WIDGET_OK;
}


int widgetScreenNCDrawLine
(
    const MsfWindow   *pWin,
    OP_INT16 start_x,
    OP_INT16 start_y,
    OP_INT16 end_x,
    OP_INT16 end_y,
    DS_COLOR color
)
{
    OP_BOOLEAN bVisible;

    if( !pWin)
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    bVisible = widgetLineGetVisible( 0, 
                                                          0, 
                                                          (OP_INT16)( DS_SCRN_MAX_X -1 ), 
                                                          (OP_INT16)( WAP_SCRN_TEXT_HIGH -1 ), 
                                                          &start_x, 
                                                          &start_y, 
                                                          &end_x, 
                                                          &end_y);
    if( bVisible )
    {
        ds_draw_line_forward(start_x, start_y, end_x, end_y, color);
    }
    
    return HDI_WIDGET_OK;
}

int widgetScreenDrawRect
(
    const MsfWindow   *pWin,
    OP_INT16 left,
    OP_INT16 top,
    OP_INT16 right,
    OP_INT16 bottom,
    DS_COLOR color
)
{

    if( !pWin )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    widgetScreenDrawLine( pWin, left, top , right, top , color );
    widgetScreenDrawLine( pWin, right, top , right, bottom, color );
    widgetScreenDrawLine( pWin, right, bottom, left, bottom, color );
    widgetScreenDrawLine( pWin, left, bottom, left, top , color );

    return HDI_WIDGET_OK;
}

int widgetScreenFillWindowClientRect(const MsfWindow   *pWin, DS_COLOR color )
{
    MsfPosition     clientStart;
    MsfSize           winSize;
    OP_INT16       left;
    OP_INT16       top;
    OP_INT16       right;
    OP_INT16       bottom;
    OP_BOOLEAN bVisible;
    
    if( !pWin )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    widgetGetClientRect( pWin, &clientStart , &winSize);
    
    widgetPointClientToScreen( pWin, 
                                                   clientStart.x,
                                                   clientStart.y, 
                                                   &left, 
                                                   &top);
                                                   
    widgetPointClientToScreen( pWin, 
                                                   (OP_INT16)(clientStart.x + winSize.width - 1), 
                                                   (OP_INT16)(clientStart.y + winSize.height- 1),  
                                                   &right, 
                                                   &bottom);
                                                   
    bVisible = widgetRectGetVisible( (OP_INT16) 0, 
                                                          (OP_INT16) 0, 
                                                          (OP_INT16) ( DS_SCRN_MAX_X-1), 
                                                          (OP_INT16) ( (WAP_SCRN_TEXT_HIGH) -1), 
                                                          &left, 
                                                          &top, 
                                                          &right, 
                                                          &bottom); 
    if( bVisible == OP_TRUE )
    {
        ds_fill_rect_forward( left, top, right, bottom,  color);
    }

    return HDI_WIDGET_OK;
}

int widgetScreenFillRect
(
    const MsfWindow   *pWin,
    OP_INT16 left,
    OP_INT16 top,
    OP_INT16 right,
    OP_INT16 bottom,
    DS_COLOR color
)
{
    MsfPosition     clientStart;
    MsfSize           winSize;
    OP_BOOLEAN bVisible;
    OP_INT16       pt1_x;
    OP_INT16       pt1_y;
    OP_INT16       pt2_x;
    OP_INT16       pt2_y;
    
    if( !pWin )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    widgetGetClientRect( pWin, &clientStart , &winSize);
    bVisible = widgetRectGetVisible( (OP_INT16) clientStart.x, 
                                                          (OP_INT16) clientStart.y, 
                                                          (OP_INT16) (clientStart.x + winSize.width -1), 
                                                          (OP_INT16) (clientStart.y + winSize.height -1), 
                                                          &left, 
                                                          &top, 
                                                          &right, 
                                                          &bottom); 
    if( bVisible == OP_TRUE )
    {
        widgetPointClientToScreen(pWin, left, top, &pt1_x, &pt1_y);
        widgetPointClientToScreen(pWin, right, bottom,  &pt2_x,  &pt2_y);
        
        bVisible = widgetRectGetVisible( (OP_INT16) 0, 
                                                              (OP_INT16) 0, 
                                                              (OP_INT16) ( DS_SCRN_MAX_X-1), 
                                                              (OP_INT16) ( WAP_SCRN_TEXT_HIGH -1), 
                                                              &pt1_x, 
                                                              &pt1_y, 
                                                              &pt2_x, 
                                                              &pt2_y); 
        if( bVisible == OP_TRUE )
        {
            ds_fill_rect_forward( pt1_x, pt1_y, pt2_x, pt2_y,  color);
        }
    }

    return HDI_WIDGET_OK;
}

int widgetScreenDrawTextLine
(
    const MsfWindow   *pWin,
    OP_INT16       x,
    OP_INT16       y,
    OP_INT16       xlimit,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
)
{
    OP_INT16       screen_x;
    OP_INT16       screen_y;
    MsfPosition     pos;
    MsfSize           size;
    OP_BOOLEAN bVisible;
    
    OP_INT16       vRectLeft;
    OP_INT16       vRectTop;
    OP_INT16       vRectRight;
    OP_INT16       vRectBottom;

    if( !pWin ||!text )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    widgetPointClientToScreen( pWin, x, y, &screen_x, &screen_y);
    widgetGetClientRect( pWin, &pos, &size );
    widgetPointClientToScreen( pWin, pos.x, pos.y, &vRectLeft, &vRectTop );
    vRectRight = vRectLeft + size.width -1;
    vRectBottom = vRectTop + size.height -1;
    
    bVisible = widgetRectGetVisible( (OP_INT16) 0, 
                                                          (OP_INT16) 0, 
                                                          (OP_INT16) ( DS_SCRN_MAX_X-1), 
                                                          (OP_INT16) ( WAP_SCRN_TEXT_HIGH -1), 
                                                          &vRectLeft,
                                                          &vRectTop,
                                                          &vRectRight,
                                                          &vRectBottom);
    if( bVisible == OP_TRUE )
    {
        /*
          * NOTE:  the range is not all right, need to adjust the text pointer.
          *             If the screen_x < 0 and the position of window is ( 20, xx ), 
          *             then the text will be drawn out of window.
          */    
        if( screen_y >= vRectTop  && screen_y <= vRectBottom  
        && screen_x <= vRectRight )
        {
            ds_draw_text_line(screen_x, 
                                            screen_y, 
                                            DS_SCRN_MAX_X, 
                                            text, 
                                            fontattr, 
                                            fontcolor, 
                                            backcolor);

            return HDI_WIDGET_PAINTED;           /* drawing executed  */                                        
        }                                    
    }
    return HDI_WIDGET_OK;
}

int  widgetScreenDrawIconRm
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_RESOURCE_ID_T     icon_res_id
)
{
    RM_ICON_T   *pIcon;
    pIcon = (RM_ICON_T *)util_get_icon_from_res(icon_res_id);
    
    return widgetScreenDrawIcon( pWin, x, y, width, height, pIcon );
}

int  widgetScreenDrawIcon
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_ICON_T                  *pIcon
)
{
    int     iRet;
    MsfWidgetDrawPictureInfo  iconInfo;
    
    if( !pWin || !pIcon )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    iRet = widgetPictureAdaptPositionSize( 
                                            pWin,  
                                            x,  
                                            y, 
                                            width, 
                                            height, 
                                            pIcon->biWidth,
                                            pIcon->biHeight, 
                                            &iconInfo );
    if( iRet == 1 )
    {
            ds_draw_partial_icon( 
                                      iconInfo.left, 
                                      iconInfo.top, 
                                      iconInfo.width, 
                                      iconInfo.height,
                                      iconInfo.bmpStartX, 
                                      iconInfo.bmpStartY,
                                      pIcon);
    }
    
    return iRet;
}

int  widgetScreenDrawBitmapRm
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_RESOURCE_ID_T     icon_res_id
)
{
    RM_BITMAP_T   *pBitmap;
    pBitmap = (RM_BITMAP_T *)util_get_bitmap_from_res(icon_res_id);
    
    return widgetScreenDrawBitmap( pWin, x, y, width, height, pBitmap );
}

int  widgetScreenDrawBitmap
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_BITMAP_T               *pBitmap
)
{
    int     iRet;
    MsfWidgetDrawPictureInfo  bmpInfo;
    
    if( !pWin || !pBitmap )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    iRet = widgetPictureAdaptPositionSize( 
                                            pWin,  
                                            x,  
                                            y, 
                                            width, 
                                            height, 
                                            pBitmap->biWidth,
                                            pBitmap->biHeight, 
                                            &bmpInfo );
    if( iRet == 1 )
    {
            ds_draw_partial_bitmap_image_forward( 
                                      bmpInfo.left, 
                                      bmpInfo.top, 
                                      bmpInfo.width, 
                                      bmpInfo.height,
                                      bmpInfo.bmpStartX, 
                                      bmpInfo.bmpStartY,
                                      pBitmap);
    }
    
    return iRet;
}

/*==================================================================================================
    FUNCTION:  widgetScreenDrawImage

    DESCRIPTION: fit to phisical screen, parser every type to bitmap image, then draw bitmap image.
        
    ARGUMENTS PASSED:
    MsfWindow         *pWin:  
    MsfPosition         *pStart :   start position in msfWindow. 
    MsfSize            *pSize:    width and height in dest msfWindow.
    MsfImage          *pImage:   image structure.
    MsfImageZoom      imageZoom: zoom rate.
        
    RETURN VALUE:
    HDI_WIDGET_ERROR_INVALID_PARAM: failed
    HDI_WIDGET_OK : success
    IMPORTANT NOTES:
        pStart and pSize should be convert to fit screen size.
==================================================================================================*/
int  widgetScreenDrawImage
(
    MsfWindow         *pWin,
    MsfPosition         *pStart, 
    MsfSize            *pSize,
    MsfImage          *pImage,
    MsfImageZoom      imageZoom
)
{
    MsfPosition     SrnStart;/* start position of image in screen */
    MsfSize         SrnSize;
    MsfSize          imgSize;
    MsfPosition     SrcStart;/* drawing start position in image*/
    MsfWidgetDrawPictureInfo  imageInfo;
    RM_BITMAP_T      bitmap;
    OP_BOOLEAN       bDefaultImage = OP_FALSE;
    int                     iRet;
    
    if( !pWin || !pStart || !pSize ||!pImage || pImage->data == OP_NULL || pImage->moreData )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    /* if it's pre-define image, draw icon directly */
    if(pImage->predefinedId != 0)
    {
        iRet = widgetScreenDrawIcon( pWin, 
                                      pStart->x, pStart->y,
                                      pSize->width, pSize->height,
                                      (RM_ICON_T *)(pImage->data) );
        return iRet;
    }
    /* the ds_get_file_image_size wants parameters Width and height are 0  */
    if(pImage->bOriginal == OP_TRUE)
    {
        if( pImage->size.height <= 0 || pImage->size.width <= 0 )
        {
            if(OP_FALSE == ds_get_file_image_size(convertImageFormat( pImage->format ), 
                                          pImage->data,
                                          pImage->dataSize,
                                          (OP_UINT16*) &pImage->size.width,
                                          (OP_UINT16*) &pImage->size.height))
            {                               
                op_memset( &pImage->size, 0, sizeof(MsfSize));
                imgSize = DefaultPropTab[MSF_IMAGE].size;
                bDefaultImage = OP_TRUE;
            }
            else
            {
                imgSize = pImage->size;
            }
        }
        else
        {
            imgSize = pImage->size;
        }
    }
    else
    {
        imgSize = pImage->size;
    }
    
    iRet = widgetPictureAdaptPositionSize( 
                                      pWin,  
                                      pStart->x,  
                                      pStart->y, 
                                      pSize->width,
                                      pSize->height,
                                      imgSize.width, 
                                      imgSize.height,
                                      &imageInfo );
    if( iRet < 1 )
    {
        return iRet; /* return 0, means there is no focus, the image cannot be shown */
    }

    iRet = HDI_WIDGET_OK;
    /* set coopration*/
    SrnStart.x = imageInfo.left;
    SrnStart.y = imageInfo.top;
    SrnSize.width = imageInfo.width;
    SrnSize.height = imageInfo.height;
    SrcStart.x = imageInfo.bmpStartX;
    SrcStart.y = imageInfo.bmpStartY;

    /* backup the image drawing start position, the relative position */
    pImage->srcPos.x =  SrcStart.x;
    pImage->srcPos.y =  SrcStart.y;   
    
    /* draw image */
    if(!IsValidImageRegion(imgSize.width, imgSize.height))
    {
        iRet = HDI_WIDGET_ERROR_NOT_SUPPORTED;
    }
    else
    {
        op_memset(&bitmap, 0x00, sizeof(RM_BITMAP_T));
        bitmap.biWidth  = (OP_UINT16)imgSize.width;
        bitmap.biHeight = (OP_UINT16)imgSize.height;

        /* if is first parse */
        if(!(pImage->flag & GIF_FIRST_PARSE))
        {
            iRet = ParserImageData( &bitmap, pImage);
            if( iRet >= 0 )
            {
                pImage->flag |= GIF_FIRST_PARSE;
            }
        }
        else
        {
            if(pImage->format == MsfGif)
            {
                bitmap.biBitCount = 8;
                bitmap.ColorPallet = pImage->colorPallet;

                if( pImage->bOriginal )
                {
                    bitmap.data = pImage->pGifBmpBuf;
                }
                else
                {
                    bitmap.data = pImage->data;
                }
            }
            else
            {
                /* as for jpg, bmp, wbmp */
                bitmap.biBitCount = 16;
                bitmap.data = pImage->data;
            }
        }
    }
    
    /* draw bitmap image */
    if( iRet == HDI_WIDGET_OK && bDefaultImage == OP_FALSE )
    {
    switch(pImage->format)
    {
        case MsfBmp:
        case MsfJpeg:
        case MsfWbmp:
                widget_draw_bitmap_image((RM_BITMAP_T *)&bitmap, 
                                           &SrnStart,
                                           &SrnSize,
                                           &SrcStart,
                                           imageZoom);
                break;
        case MsfGif:
            widget_draw_bitmap_image((RM_BITMAP_T *)&bitmap, 
                                   &SrnStart,
                                   &SrnSize,
                                   &SrcStart,
                                   imageZoom);
            break;
#if 0
            case MsfWbmp:
                /* for saving memory we draw wbmp image directly instead of malloc parse-data buffer*/
                ds_draw_wbmp_image(AREA_TEXT,
                                      SrnStart.x,
                                      SrnStart.y,
                                      SrnSize.width,
                                      SrnSize.height,
                                      SrcStart.x,
                                      SrcStart.y,
                                      pImage->data);
                break;
#endif
        default:
                iRet = widget_draw_default_image(pWin, pStart, pSize);
                break;
        }
    }
    else
    {
        iRet = widget_draw_default_image(pWin, pStart, pSize);
    }
    return iRet;
} 
int widgetScreenDrawCaret
(
    const MsfWindow      *pWin, 
    OP_INT16                x, 
    OP_INT16                y,  
    OP_INT16                iCaretHeight
)
{
    MsfPosition  clientStart;
    MsfSize        winSize;
    OP_BOOLEAN bVisible;
    OP_INT16    pt1_x;
    OP_INT16    pt1_y;
    OP_INT16    pt2_x;
    OP_INT16    pt2_y;
    OP_INT16    x1;
    OP_INT16    y1;
    OP_INT16    x2;
    OP_INT16    y2;    

    DS_COLOR   pixel;
    OP_INT16    i, j;

    if( !pWin ||iCaretHeight  < 1   )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    y1 = y;
    x1 = x;
    y2 = y + iCaretHeight -1;
    x2 = x + 1;
    
    widgetGetClientRect( pWin, &clientStart , &winSize);
    
    bVisible = widgetRectGetVisible( clientStart.x, 
                                                          clientStart.y, 
                                                          (OP_INT16)( clientStart.x + winSize.width -1 ), 
                                                          (OP_INT16)( clientStart.y + winSize.height -1 ), 
                                                          &x1, 
                                                          &y1, 
                                                          &x2, 
                                                          &y2);

    if( bVisible )
    {
        widgetPointClientToScreen(pWin, x1, y1, &pt1_x, &pt1_y);
        widgetPointClientToScreen(pWin, x2, y2,  &pt2_x,  &pt2_y);
        
        bVisible = widgetRectGetVisible( (OP_INT16) 0, 
                                                              (OP_INT16) 0, 
                                                              (OP_INT16) ( DS_SCRN_MAX_X-1), 
                                                              (OP_INT16) ( WAP_SCRN_TEXT_HIGH -2), 
                                                              &pt1_x, 
                                                              &pt1_y, 
                                                              &pt2_x, 
                                                              &pt2_y); 
        if( bVisible == OP_TRUE )
        {
            for( i = pt1_x; i <= pt2_x; i++)
                for( j = pt1_y; j <= pt2_y; j++ )
                {
                    pixel = ~ get_pixel( AREA_TEXT, i, j );
                    ds_put_pixel_forward( i, j , pixel );
                }
        }    
    }

    return HDI_WIDGET_OK;

}

int  widgetScreenShowSoftKey( const MsfScreen* pScreen, OP_BOOLEAN bRefresh )
{
    if( !pScreen )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pScreen->isFocused == OP_FALSE || pScreen->bInBatches )
    {
        return HDI_WIDGET_OK;
    }
    
    if( ISVALIDHANDLE(pScreen->focusedChild) )
    {
        MsfWindow  *pWin;
        MsfWidgetType widgetType;
        
        pWin =(MsfWindow*)seekWidget( pScreen->focusedChild,  &widgetType);
        if( !pWin ||!IS_WINDOW(widgetType) )
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }

        return  widgetWindowShowSoftKey( pWin, bRefresh );
    }
    else
    {
        ds_set_softkeys_char( pScreen->softKeyList[0], pScreen->softKeyList[1], pScreen->softKeyList[2] );

        if( bRefresh == OP_TRUE )
        {
#if 0
            WAP_REFRESH
#else
            ds_refresh_softkey_rectangle();
#endif
        }
    }

    return HDI_WIDGET_OK;
}

int  widgetWindowShowSoftKey( const MsfWindow  *pWin, OP_BOOLEAN bRefresh )
{
    OP_UINT8 *pLeft;
    OP_UINT8 *pCenter;
    OP_UINT8 *pRight;
    
    MsfScreen          *pScreen;
    MsfWidgetType   wtWin;
    
    if( !pWin )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pWin->isFocused == OP_FALSE || pWin->bInBatches || pWin->parent->bInBatches )
    {
        return HDI_WIDGET_OK;
    }

    if( pWin->focusedChild != OP_NULL && getWidgetType(pWin->windowHandle) != MSF_MENU )
    {
        MsfGadget        *pGadget;
        
        pGadget = (MsfGadget*)(pWin->focusedChild->gadget);
        if( !pGadget )
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        
        pLeft = pGadget->softKeyList[0];
        /* don't handle OK key */
        if( pGadget->propertyMask & MSF_GADGET_PROPERTY_SG_DO_NOT_HANDLE_OK )
        {
            pCenter = OP_NULL;
        }
        else
        {
            pCenter = pGadget->softKeyList[1];
        }
        
        pRight = pGadget->softKeyList[2];
    }
    else
    {
        pLeft = OP_NULL;
        pCenter = OP_NULL;
        pRight = OP_NULL;
    }

    /* Check whether the actions of a window is disable before setting the softkeys */
    if (!(pWin->propertyMask & MSF_WINDOW_PROPERTY_DISABLED))
    {
        if( pLeft == OP_NULL )
        {
            pLeft = pWin->softKeyList[0];
        }
        
        if( pCenter== OP_NULL )
        {
            pCenter = pWin->softKeyList[1];
        }
        if( pRight == OP_NULL )
        {
            pRight = pWin->softKeyList[2];
        }
    }

    wtWin = getWidgetType(pWin->windowHandle);
    if(  wtWin != MSF_DIALOG  && wtWin != MSF_WAITINGWIN)
    {
        pScreen = pWin->parent;
        
        if( pLeft == OP_NULL )
        {
            pLeft = pScreen->softKeyList[0];
        }
        
        if( pCenter== OP_NULL )
        {
            pCenter = pScreen->softKeyList[1];
        }
        if( pRight == OP_NULL )
        {
            pRight = pScreen->softKeyList[2];
        }
    }
    
    ds_set_softkeys_char( pLeft, pCenter, pRight );
    
    if( bRefresh == OP_TRUE && pWin->bDrawing == OP_FALSE )
    {
#if 0
        WAP_REFRESH
#else    
        /*  Only update the title area  */
        ds_refresh_softkey_rectangle();
#endif                                
    }

    return HDI_WIDGET_OK;
}

DS_SCRN_TITLE_ENUM_T  widgetWindowGetTitleType( const MsfWindow  *pWin )
{
    DS_SCRN_TITLE_ENUM_T ret = TITLE_NULL;
    if( pWin )
    {
        switch( getWidgetType( pWin->windowHandle ) )
        {
           case MSF_WAITINGWIN:
           case MSF_DIALOG:
               ret = TITLE_NULL;
               break;
#if (PROJECT_ID == LOTUS_PROJECT)
           case MSF_MENU:
           case MSF_MAINMENU:
               ret = TITLE_WIDGET_MENU;
               break;
#endif
           default:
               ret = TITLE_NORMAL;
               break;   
        }
        
    }

    return ret;
}

/*==================================================================================================
    FUNCTION:  widget_draw_default_image

    DESCRIPTION: Draw a default image(20*20) when we don't know how to parse it or no enough memory to show.
        
    ARGUMENTS PASSED:
    MsfWindow         *pWin:  
    MsfPosition         *pStart :   start position in msfWindow. 
    MsfSize            *pSize:    width and height in dest msfWindow.
        
    RETURN VALUE:
        none
    IMPORTANT NOTES:
        we can draw line or set a image.
==================================================================================================*/
int widget_draw_default_image
(
    MsfWindow       *pWin,
    MsfPosition     *pStart, 
    MsfSize         *pSize
)
{
    OP_INT16   left, top, right, bottom, width, height;

    width = height = DEFAULT_IMAGEGADGET_HEIGHT;

    if( width > pSize->width )
    {
        width = pSize->width;
    }
    if( height > pSize->height )
    {
        height = pSize->height;
    }
    
    left = pStart->x + ((pSize->width - width) / 2 );
    top = pStart->y + ((pSize->height- height) / 2 );
    right = left + width -1;
    bottom = top + height -1;
    
    widgetScreenDrawRect(
                        pWin, 
                        left,  
                        top, 
                        right, 
                        bottom, 
                        (DS_COLOR) COLOR_BLACK );

    widgetScreenDrawLine(
                        pWin, 
                        left,  
                        top, 
                        right, 
                        bottom, 
                        (DS_COLOR) COLOR_RED);
                            
    widgetScreenDrawLine(
                        pWin, 
                        left,  
                        bottom, 
                        right, 
                        top, 
                        (DS_COLOR) COLOR_RED);

    return IMAGE_DRAW_DEFAULT_IMAGE;
}

/*==================================================================================================
    FUNCTION:  widget_draw_focus_rect

    DESCRIPTION: Draw a focus rect according image gadget size.
        
    ARGUMENTS PASSED:
    MsfWindow         *pWin:  
    MsfPosition         *pStart :   start position in msfWindow. 
    MsfSize            *pSize:    width and height in dest msfWindow.
        
    RETURN VALUE:
        none
    IMPORTANT NOTES:
        we can draw line or set a image.
==================================================================================================*/
void widget_draw_focus_rect
(
    int             iValue,
    MsfWindow       *pWin,
    MsfPosition     *pStart, 
    MsfSize         *pSize
)
{
    OP_INT16   left, top, right, bottom, width, height;

    if( !pWin || !pStart || !pSize || (pSize->height == 0 && pSize->width == 0))
    {
        return ;
    }
    
    if( iValue == IMAGE_DRAW_DEFAULT_IMAGE )    /* Only draw a 16*16 rect to replace the real image  */
    {
        width = height = DEFAULT_IMAGEGADGET_HEIGHT;

        if( width > pSize->width )
        {
            width = pSize->width;
        }
        if( height > pSize->height )
        {
            height = pSize->height;
        }
        
        left = pStart->x + ((pSize->width - width) / 2 );
        top = pStart->y + ((pSize->height- height) / 2 );
        right = left + width -1;
        bottom = top + height -1;
    }            
    else
    {
        left = pStart->x;
        top = pStart->y;
        right = left + pSize->width - 1;
        bottom = top + pSize->height- 1;
    }

    widgetScreenDrawRect(pWin, 
                         left, 
                         top, 
                         right, 
                         bottom, 
                         DEFAULT_FOCUS_BORDER_COLORS);

}
/*==================================================================================================
    FUNCTION:  convertImageFormat

    DESCRIPTION: convert the msf image format to opus image format .
        
    ARGUMENTS PASSED:
        MsfImageFormat imageformat:  msf image format.
        
    RETURN VALUE:
        return RM_IMAGE_FORMAT type.
        
    IMPORTANT NOTES:
==================================================================================================*/
RM_IMAGE_FORMAT convertImageFormat
(
    MsfImageFormat imageformat
)
{
    RM_IMAGE_FORMAT format;
    switch(imageformat)
    {  
        case MsfBmp:
            format = IMAGE_BMP;
            break;
        case MsfPictogram:
            format = IMAGE_PICTOGRAM;
            break;
        case MsfGif:
            format = IMAGE_GIF;
            break;
        case MsfJpeg:
            format = IMAGE_JPEG;
            break;
        case MsfPng:
            format = IMAGE_PNG;
            break;
        case MsfWbmp:
            format = IMAGE_WBMP;
            break;
        default:
            format = IMAGE_WBMP;
            break;
    }
    
        return format;
}

/*==================================================================================================
    FUNCTION:  IsValidImageRegion

    DESCRIPTION: check the image size.
        
    ARGUMENTS PASSED:
        OP_INT16 width, 
        OP_INT16 height

    RETURN VALUE:
        return OP_TRUE when in Screen size.
    
    IMPORTANT NOTES:
==================================================================================================*/
OP_BOOLEAN IsValidImageRegion
(
    OP_INT16 width, 
    OP_INT16 height
)
{
    if( (width <= 0) 
     ||(height <= 0)
     ||(width > MAX_WIDTH_OF_VISIBLE_IMAGE )
     ||(height > MAX_HEIGHT_OF_VISIBLE_IMAGE ) )
    {
        return OP_FALSE;
    }
    else
    {
        return OP_TRUE;
    }
}

static OP_UINT8 *malloc_for_image_parsedata
(
    MsfImageFormat    format, 
    MsfSize            size
)
{
    OP_INT32       imagesize;
    OP_UINT8       *pData = OP_NULL;

    if (size.width > 0 && size.height > 0)
    {
        if(format == MsfGif)
        {
            imagesize = size.width * size.height * sizeof(OP_UINT8);
        }
        else
        {
            imagesize = size.width * size.height * sizeof(OP_UINT16) ;
        }
        
        pData = WIDGET_ALLOC(imagesize );
        if(pData != OP_NULL)
        {
            op_memset(pData, 0x00, imagesize);
        }
    }

    return pData;  
}

/* when the image is showing, set the parser data into pImage->data */
static int ParserImageData
(
    RM_BITMAP_T *pBitmap,
    MsfImage    *pImage
)
{
    int       iRet = HDI_WIDGET_OK;

    switch(pImage->format)
    {
        case MsfBmp:
        case MsfJpeg:
        case MsfWbmp:
        {
#if 0        
            if(pImage->bOriginal == OP_FALSE)
            {   /* if it's parsed data, organize the RM_BITMAP_T structure directly */
                pBitmap->biBitCount = 16;
                pBitmap->data = pImage->data;
            }
            else
#endif            
            {   /* if it's original data, need parse, then replace the pImage->data with new Parser data */
                pBitmap->data = malloc_for_image_parsedata(pImage->format, pImage->size);
                if(pBitmap->data == OP_NULL)
                {
                    return HDI_WIDGET_ERROR_RESOURCE_LIMIT;
                }
                if( ds_parse_image(pBitmap, pImage->data, pImage->dataSize,convertImageFormat(pImage->format))
                   == OP_TRUE )
                {
                    WIDGET_FREE(pImage->data);
                    pImage->data = (OP_UINT8 *)pBitmap->data;
                    pImage->dataSize = pBitmap->biHeight * pBitmap->biWidth * sizeof(OP_UINT16) ;
                    pImage->bOriginal = OP_FALSE;
                }
                else
                {   /* if parse failed, should free bitmap->data */
                    WIDGET_FREE(pBitmap->data);
                    return HDI_WIDGET_ERROR_UNEXPECTED;
                }
            }
            break;
        }         

        case MsfGif:
        {
            OP_UINT16   delay = 500;
            int    iGifRet = 0;
#if 0
            if((pImage->bOriginal == OP_FALSE)||(!(pImage->flag & GIF_FIRST_PARSE)))
            {
                pBitmap->biBitCount = 8;
                pBitmap->ColorPallet = pImage->colorPallet;
                if(!pImage->bOriginal)
                {
                    pBitmap->data = pImage->data;
                }
                else 
                {   /* as for gif, we use a temp buf for record current parse data,
                       once AU call the image_show, we can draw the temp buf data 
                       without parse next page */
                    pBitmap->data = pImage->pGifBmpBuf;
                }
            }
            else
 #endif
       {   /* when image create, set GIF_FIRST_PARSE, here reverse it */
                pBitmap->data = malloc_for_image_parsedata(pImage->format, pImage->size);
                if(pBitmap->data == OP_NULL)
                {
                    return HDI_WIDGET_ERROR_RESOURCE_LIMIT;
                }
                /* save color pallet, release when image remove */
                
                pBitmap->ColorPallet = WIDGET_ALLOC(256*sizeof(OP_UINT16));
                if(pBitmap->ColorPallet == OP_NULL)
                {
                    WIDGET_FREE(pBitmap->data);
                    return HDI_WIDGET_ERROR_RESOURCE_LIMIT;
                }
                /* analize file header and get global color pallet*/
                GIF_AnalizeFileHeader(pImage->handle, pImage->data, (OP_UINT32)pImage->dataSize,
                                      pBitmap->ColorPallet);

                iGifRet = GIF_LoadData(pImage->handle, pBitmap, &delay);
                /* return 2, means one frame gif image*/
                /* return 1, means multi-frame gif image, but only in SMA and SLS, we play animation*/
                if(iGifRet == 2 )
               //    ((iGifRet>0)&&(pImage->modId != MSF_MODID_SMA)&&(pImage->modId != MSF_MODID_SLS)))
                {
                    WIDGET_FREE(pImage->data);
                    pImage->data = (OP_UINT8 *)pBitmap->data;
                    pImage->colorPallet =(OP_UINT16 *)pBitmap->ColorPallet;
                    pImage->dataSize = pBitmap->biHeight * pBitmap->biWidth * sizeof(OP_UINT16) ;
                    pImage->bOriginal = OP_FALSE;
                    GIF_Terminate(pImage->handle);
                }
                else if(iGifRet > 0)
                {   /* pImage->data keep original data, and pImage->pGifBmpBuf keeps parse data */
                    pImage->pGifBmpBuf = (OP_UINT8 *)pBitmap->data;
                    pImage->colorPallet = (OP_UINT16 *)pBitmap->ColorPallet; 
                    pImage->flag |= GIF_TIMER_REQUEST;
                }
                else 
                {   /* if parse failed, should free bitmap->data */
                    if(pBitmap->data)
                    {
                        WIDGET_FREE(pBitmap->data);
                    }
                    if(pImage->colorPallet)
                    {
                        WIDGET_FREE(pImage->colorPallet);
                    }
                    return HDI_WIDGET_ERROR_UNEXPECTED;
                }
            }
            break;
        }
        default:
            break;
    }
    return iRet;
}

int widgetImageGadgetPlay( MsfImageGadget *pImageGadget, OP_BOOLEAN bRefresh )
{
    MsfImage            *pImage;
    MsfWidgetType   wt;
    RM_BITMAP_T    bitmap;
    OP_UINT16        delay = 500;
    int                   iRet;
    
    WIDGET_TRACE(("widgetImageGadgetPlay\n"));
    
    if( !pImageGadget || pImageGadget->gadgetData.parent == OP_NULL )
    {
        return HDI_WIDGET_ERROR_INVALID_PARAM;
    }

    pImage = (MsfImage*)seekWidget(pImageGadget->image, &wt );
    if(!pImage || pImage->data == OP_NULL)
    {
        return HDI_WIDGET_ERROR_UNEXPECTED;
    }
    
    /* draw image */
    op_memset(&bitmap, 0x00, sizeof(RM_BITMAP_T));
    if(!IsValidImageRegion(pImage->size.width, pImage->size.height))
    {
        iRet = HDI_WIDGET_ERROR_NOT_SUPPORTED;
    }
    else
    {
        bitmap.biBitCount = 8;
        bitmap.data = pImage->pGifBmpBuf;
        bitmap.ColorPallet = pImage->colorPallet;
        bitmap.biWidth = (OP_UINT16)pImage->size.width;
        bitmap.biHeight = (OP_UINT16)pImage->size.height;
        if(bitmap.data == OP_NULL)
        {
            return HDI_WIDGET_ERROR_RESOURCE_LIMIT;
        }
        
        iRet = GIF_LoadData(pImage->handle, &bitmap, &delay);

        if( iRet >= 0 )
        {
            WIDGET_TRACE(("widgetImageGadgetPlay:   draw next frame\n"));
            
            widgetRedraw((void*)pImageGadget, MSF_IMAGEGADGET, 0, bRefresh );
            iRet = HDI_WIDGET_OK;
        }
    }

    return iRet;
}
/*==================================================================================================
    FUNCTION: HDIa_widgetDrawImage

    DESCRIPTION: draw bitmap image according different zoom rate.

    ARGUMENTS PASSED:
        bitmap      : The identifier of the bitmap Image to be drawn.
        dest_position : Specifies the MsfPosition of the bounding rectangle
                      of the MsfImage.
        dest_size    : Specifies the maximum MsfSize of the bounding
                      rectangle of the MsfImage.
        src_position :  Specifies the MsfPosition of the source bitmap.
        imageZoom  : Specifies the MsfImageZoom the MsfImage should be
                      drawn with. The drawn MsfImage is still bounded by
                      the specified dest_size.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        The only interface between ds and msf;
        Called by HDIa_widgetDrawImage() or widgetScreenDrawImage().
==================================================================================================*/
static int widget_draw_bitmap_image
(
     RM_BITMAP_T*    bitmap,
     MsfPosition*      dest_position,
     MsfSize*         dest_size,
     MsfPosition*      src_position,
     MsfImageZoom    imageZoom
)
{
    if(bitmap == OP_NULL)
    {
        return 0;
    }
    switch(imageZoom)
    {
    case MsfImageZoom100:
    case MsfImageZoomAutofit:
        draw_partial_bitmap_image(AREA_TEXT, 
            dest_position->x,
            dest_position->y,
            dest_size->width,
            dest_size->height,
            src_position->x, 
            src_position->y,
            (RM_BITMAP_T *)bitmap);
        break;
    case MsfImageZoom10:
        break;
        
    case MsfImageZoom25:
        ds_draw_resized_bitmap_image(dest_position->x,
            dest_position->y,
            (OP_UINT8)((OP_UINT16)dest_size->width>>2),
            (OP_UINT8)((OP_UINT16)dest_size->height>>2),
            src_position->x, 
            src_position->y,
            (RM_BITMAP_T *)bitmap);
        break;
    case MsfImageZoom50:
        ds_draw_resized_bitmap_image(dest_position->x,
            dest_position->y,
            (OP_UINT8)((OP_UINT16)dest_size->width>>1),
            (OP_UINT8)((OP_UINT16)dest_size->height>>1),
            src_position->x, 
            src_position->y,
            (RM_BITMAP_T *)bitmap);
        break;
    case MsfImageZoom75:
        break;
    case MsfImageZoom125:
        break;
    case MsfImageZoom150:
        break;
    case MsfImageZoom200:
        break;
    case MsfImageZoom400:
        break;
    case MsfImageZoom800:
        break;
    default:
        break;
    }
    
   return 1;
}


/*==================================================================================================
    FUNCTION: widgetGetIconResID

    DESCRIPTION: get icon resID according to MsfIconType.

    ARGUMENTS PASSED:
         MsfIconType    MsfIconType.      
         
    RETURN VALUE:
        Resource ID. Otherwise return RM_ICON_LAST_ID.

    IMPORTANT NOTES:
==================================================================================================*/
RM_RESOURCE_ID_T widgetGetIconResID
(
    MsfIconType    msfIconType
)
{
    RM_RESOURCE_ID_T  resID;

    resID = RM_ICON_LAST_ID;
    switch(msfIconType)
    {
    case MsfButton:
        resID = ICON_RADIOBTN_OFF;
        break;
    case MsfButtonPressed:
#if (PROJECT_ID == DRAGON_M_PROJECT)   
        resID = ICON_RADIOBTN_ON_BIG + ds_get_color_theme();
#else
        resID = ICON_RADIOBTN_ON;
#endif
        break;
    case MsfRadioButton:
        resID = ICON_RADIOBTN_OFF;
        break;
    case MsfRadioButtonPressed:
#if (PROJECT_ID == DRAGON_M_PROJECT)      
        resID = ICON_RADIOBTN_ON_BIG + +ds_get_color_theme();
#else
        resID = ICON_RADIOBTN_ON;
#endif        
        break;
    case MsfCheckBox:
        resID = ICON_CHECKBOX_OFF;
        break;
    case MsfCheckBoxChecked:
        resID = ICON_CHECKBOX_ON;
        break;
    case MsfKey:
        break;
    case MsfBullet:
        break;
    case MsfTick:
        break;
    case MsfSquare:
        resID = ICON_CHECKBOX_OFF;
        break;
    case MsfCircle:
        resID = ICON_RADIOBTN_OFF;
        break;
    case MsfDisk:
        break;
    case MsfArrowHead:
        break;
    case MsfIcon1:
        break;
    case MsfIcon2:
        break;
    case MsfIcon3:
        break;
    case MsfIcon4:
        break;
    default:
        break;
    }
    return resID;
}


#if 0
/*
       
    ___x1___x2__
    |    up    |
  y1|--|----|--|
    |l |    |r |
    |  |    |  |
  y2|--|----|--|
    |___down___|

*/
static IMAGEGADGET_HANDLE_AREA get_image_handle_area
(
    MsfSize     *imageSize, 
    MsfPosition *rectStart,
    MsfSize     *rectSize, 
    OP_INT16     tp_x, 
    OP_INT16     tp_y
)
{
    MsfPosition    mid;
    OP_INT16       x1, x2, y1, y2;
    IMAGEGADGET_HANDLE_AREA    area;
        
    if(!imageSize || !rectStart || !rectSize)
    {
        return IMG_INVALID;
    }
        
    mid.x = rectStart->x + (rectSize->width >> 1);
    mid.y = rectStart->y + (rectSize->height >> 1);

    x1 = rectStart->x +  (rectSize->width >> 2);
    x2 = mid.x + (rectSize->width >> 2);
    y1 = rectStart->y + (rectSize->height >> 2);
    y2 = mid.y + (rectSize->height >> 2);

    area = IMG_INVALID;
    if(tp_y <= y1)
    {
        /* move up the image */
        area = IMG_UP;
    }
    else if(tp_y >= y2)
    {
        /* move down the image */
        area = IMG_DOWN;
    }
    else if((tp_y > y1) && (tp_y < y2))
    {
        if(tp_x <= x1 )
        {
            /* move left the image */
            area = IMG_LEFT;
        }
        else if(tp_x >= x2)
        {
            /* move right the image */
            area = IMG_RIGHT;
        }
    }

    return area;    
}

void get_image_src_postion
(
    MsfPosition    *src_start,  /* dest resource start of image */
    MsfPosition    *cur_srcstart, /* start position opositive the image(0,0)*/
    MsfPosition    *gadgetPos,
    MsfSize        *gadgetSize,
    MsfSize        *imageSize,
    OP_INT16       tp_x,
    OP_INT16       tp_y
)
{
    IMAGEGADGET_HANDLE_AREA    area;
    OP_INT16    offset;
    
    /* give the original data */
    src_start->x = cur_srcstart->x;
    src_start->y = cur_srcstart->y;
    
    area = get_image_handle_area(imageSize, 
                                 gadgetPos, 
                                 gadgetSize,
                                 tp_x, 
                                 tp_y);
    switch(area)
    {
        case IMG_UP:
        {
            if(cur_srcstart->y >0)
            {
                offset = cur_srcstart->y - (gadgetSize->height >> 1);
                if(offset <= 0)
                {
                    src_start->y = 0;
                }
                else
                {
                    src_start->y = offset;
                }
                cur_srcstart->y = src_start->y;
            }
        }
        break;
        case IMG_DOWN:
            if(cur_srcstart->y >= 0 && cur_srcstart->y < imageSize->height)
            {
                offset = cur_srcstart->y + (gadgetSize->height >> 1);
                if((imageSize->height - offset) >= gadgetSize->height)
                {
                    src_start->y = offset;
                }
                else
                {
                    src_start->y = imageSize->height - gadgetSize->height;
                }
                cur_srcstart->y = src_start->y;
            }
            break;

        case IMG_LEFT:
            if(cur_srcstart->x > 0)
            {
                offset = cur_srcstart->x - (gadgetSize->width >> 1);
                if(offset <= 0)
                {
                    src_start->x = 0;
                }
                else
                {
                    src_start->x = offset;
                }
                cur_srcstart->x = src_start->x;
            }
            break;

        case IMG_RIGHT:
            if(cur_srcstart->x >= 0 && cur_srcstart->x < imageSize->width)
            {
                offset = cur_srcstart->x + (gadgetSize->width >> 1);
                if((imageSize->width - offset) >= gadgetSize->width)
                {
                    src_start->x = offset;
                }
                else
                {
                    src_start->x = imageSize->width - gadgetSize->width;
                }
                cur_srcstart->x = src_start->x;
            }

            break;
        default:
            break;
    }
}

void image_scroll
(
    MsfGadget    *pGadget,
    MsfImage     *pImage,
    OP_INT16     tp_x,
    OP_INT16     tp_y
)
{
    MsfPosition        pos;
    MsfSize             size;
    RM_BITMAP_T    bitmap;
    MsfPosition    SrcStart;
    MsfPosition    SrnStart;
    MsfSize        SrnSize;
    MsfWidgetDrawPictureInfo  imageInfo;


    if(pImage->format == MsfGif)
    {
        bitmap.biBitCount = 8;
        bitmap.ColorPallet = pImage->colorPallet;
        //bitmap.data = pImage->pGifBmpBuf;
        if(pImage->bOriginal)
        {
            bitmap.data = pImage->pGifBmpBuf;        
        }
        else
        {
            bitmap.data = pImage->data;
        }
    }
    else 
    {
        bitmap.biBitCount = 16;
        bitmap.ColorPallet = OP_NULL;
        bitmap.data = pImage->data ;
    }
    bitmap.biHeight = pImage->size.height;
    bitmap.biWidth = pImage->size.width;

    widgetGadgetGetPosition( pGadget, &pos.x, &pos.y);
    widgetGadgetGetSize(pGadget, &size);
    
    widgetPictureAdaptPositionSize( pGadget->parent,
                              pos.x,  
                              pos.y, 
                              size.width,
                              size.height,
                              pImage->size.width, 
                              pImage->size.height,
                              &imageInfo );
    /* set coopration*/
    SrnStart.x = imageInfo.left;
    SrnStart.y = imageInfo.top;
    SrnSize.width = imageInfo.width;
    SrnSize.height = imageInfo.height;

    get_image_src_postion(&SrcStart, 
                          &pImage->srcPos,
                          &pos,
                          &size, 
                          &pImage->size,
                          tp_x, 
                          tp_y);
    widget_draw_bitmap_image((RM_BITMAP_T *)&bitmap, 
                             &SrnStart,
                             &SrnSize,
                             &SrcStart,
                             MsfImageZoom100);

    widget_draw_focus_rect( 0, pGadget->parent, &pos, &size);
    WAP_REFRESH
} 
#endif

/*back up screen into bitmap_screen */
void backupDisplay( void )
{
    bDisplayBackup = OP_TRUE;
    op_memcpy(bitmap_screen[0], 
                        txt_screen[0], 
                       (DS_SCRN_MAX_X * WAP_SCRN_TEXT_HIGH * sizeof(OP_UINT16)));
                       
}

OP_BOOLEAN restoreDisplay( OP_BOOLEAN   bOnlyPaintBox , MsfWindow  *pPaintBox)
{
    if( bDisplayBackup == OP_FALSE )
    {
        return OP_FALSE;
    }
    
    if( bOnlyPaintBox == OP_FALSE )
    {
        op_memcpy(txt_screen[0], 
                            bitmap_screen[0], 
                           (DS_SCRN_MAX_X * WAP_SCRN_TEXT_HIGH * sizeof(OP_UINT16)));

        UPDATESET(AREA_TEXT);
        
        set_update_region(0, 0, DS_SCRN_MAX_X - 1,  WAP_SCRN_TEXT_HIGH - 1);
    }
    else if( pPaintBox )
    {
        op_memcpy(&txt_screen[pPaintBox->position.y][0], 
                            &bitmap_screen[pPaintBox->position.y][0], 
                           (DS_SCRN_MAX_X * (pPaintBox->size.height) * sizeof(OP_UINT16)));
                           
        UPDATESET(AREA_TEXT);
        
        set_update_region(0, 0, DS_SCRN_MAX_X - 1,  WAP_SCRN_TEXT_HIGH - 1);
    }
    else 
    {
        return OP_FALSE;
    }

    return OP_TRUE;
}

void  clearBackupDisplay(void)
{
    bDisplayBackup = OP_FALSE;
}

/*==================================================================================================
    FUNCTION:  widgetFocusGadgetAdjustWindow

    DESCRIPTION:
        Notify the paintbox to scroll its contents
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         

    IMPORTANT NOTES:
        None
==================================================================================================*/
OP_BOOLEAN widgetFocusGadgetAdjustWindow
(
    MsfGadget  *pGadget,
    MsfWindow   *pWin
)
{
    MsfWidgetType  gadgetType;
    MsfWidgetType  winType;
    MsfSize             size;
    OP_BOOLEAN     bNeedScrolled = OP_FALSE;
    OP_INT16  x, y;
    OP_INT16  iBottom;
    OP_INT16  iDeltaY = 0;
    
    if( !pGadget ||!pWin )
    {   
        return OP_FALSE;
    }
    
    winType = getWidgetType(pWin->windowHandle);
    if( winType == MSF_EDITOR)
    {   
        return OP_FALSE;
    }
    
    widgetGadgetGetPosition( pGadget, &x, &y);
    widgetPointClientToScreen(pWin, x, y, &x, &y);

    gadgetType = getWidgetType(pGadget->gadgetHandle);
    if( gadgetType == MSF_TEXTINPUT )
    {
        if( winType == MSF_PAINTBOX )
        {
            iBottom = WAP_SCRN_TEXT_HIGH - WIDGET_IMM_MAX_HEIGHT;
        }
        else
        {
            MsfWidgetImm   *pImm = ((MsfTextInput*)pGadget)->input.pImm;
            if( pImm && pImm->bOpen == OP_TRUE )
            {
                iBottom = pImm->start_y - 4;
            }
            else
            {
                iBottom = WAP_SCRN_TEXT_HIGH;
            }
        }
    }
    else
    {
        iBottom = pWin->position.y + pWin->size.height ;
    }
    widgetGadgetGetSize(pGadget, &size);
    
    /* the gadget beyond the client area */
    if( y < pWin->position.y + pWin->clientOrigin.y )
    {
        iDeltaY = y - (pWin->position.y + pWin->clientOrigin.y + 4);
    }
    /* the gadget below the the client area  */
    else if( y + size.height > iBottom )
    {
        iDeltaY = y + size.height - iBottom;
    }

    if( iDeltaY != 0 )
    {
        if( winType == MSF_PAINTBOX )
        {
            widgetPaintBoxScroll( pWin, 0, iDeltaY );
            HDIc_widgetNotify(pGadget->modId, pGadget->gadgetHandle, MsfNotifyFocus);
        }
        else
        {
            pWin->scrollPos.y += iDeltaY;
        }
        
        bNeedScrolled = OP_TRUE;
    }

    return bNeedScrolled;
}




/*==================================================================================================
    FUNCTION:  fnBmpAniTimerCallBack

    DESCRIPTION:
        call back function for timer
        
    ARGUMENTS PASSED:
         handle:            gadget handle related to the timer
         TimerId:           current timer id.
        
    RETURN VALUE:
         HDI_WIDGET_ERROR_INVALID_PARAM:            invalid parameters
         HDI_WIDGET_OK:                             normal return

    IMPORTANT NOTES:
        None
==================================================================================================*/
int fnBmpAniTimerCallBack(MsfGadgetHandle handle, OP_UINT32 TimerId)
{
    MsfBmpAnimation *pBmpAnimation;
    RM_INT_ANIDATA_T  *pAni;
    MsfWidgetType   gadgetType;
    OP_BOOLEAN      bStartTimer = OP_FALSE;

    if(!(ISVALIDHANDLE(handle)) || (WIDGET_TIMER_INVALID_ID == TimerId))
    {
       return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pBmpAnimation = (MsfBmpAnimation *)seekWidget(handle, &gadgetType);
        if(OP_NULL == pBmpAnimation)
        {
           return HDI_WIDGET_ERROR_UNEXPECTED;
        }

        if( pBmpAnimation->bPeriodicTimer == OP_FALSE )
        {
            pBmpAnimation->timer_id = WIDGET_TIMER_INVALID_ID;
        }
        
        pAni = (RM_INT_ANIDATA_T *)pBmpAnimation->pAniData;
        pBmpAnimation->iCurFrame++;
        
        if(pBmpAnimation->iCurFrame >= (pAni->frames - 1))
        {
            if(!pBmpAnimation->bLoop )/* one short */
            {
                if( pBmpAnimation->bPeriodicTimer )
                {
                    HDIa_widgetStopTimer( handle, pBmpAnimation->timer_id);
                    pBmpAnimation->timer_id = WIDGET_TIMER_INVALID_ID;
                }
            }
            else if( pBmpAnimation->bPeriodicTimer == OP_FALSE )
            {
                bStartTimer = OP_TRUE;
            }
            
            if(pBmpAnimation->iCurFrame >= pAni->frames)
            {
                pBmpAnimation->iCurFrame = 0;
            }
        }
        else if( pBmpAnimation->bPeriodicTimer == OP_FALSE )
        {
            bStartTimer = OP_TRUE;
        }

        if( bStartTimer )
        {
             pBmpAnimation->timer_id = HDIa_widgetStartTimer(pBmpAnimation->interval, 
                                                      MSF_FALSE, /* period timer */
                                                      handle,
                                                      (widget_timer_callback_t)fnBmpAniTimerCallBack);
        }

        widgetRedraw((void *)pBmpAnimation, gadgetType, handle, OP_TRUE);
    }
    return HDI_WIDGET_OK;
}


OP_BOOLEAN  widgetGetImageSize(RM_RESOURCE_ID_T res_id, MsfSize *pSize)
{
     OP_BOOLEAN bRet = OP_TRUE;
     bRet = ds_get_image_size(res_id, (OP_UINT16 *)(&pSize->width), (OP_UINT16 *)(&pSize->height));
     if((!bRet) ||(0 == pSize->width) || (0 == pSize->height))
     {
         bRet = OP_FALSE;
     }
     return bRet;
}


int widget_button_up_arrow_callback(MSF_UINT8 modID, /* modId */
                                      MSF_UINT32 handle ,           /*  widget handle */
                                      MsfNotificationType notifyType,  /*  state notification  */
                                      void *pExtraData )   /*  extra data. it is used  ONLY the MsfNotificationType is MsfNotifyStateChange. The meanings as following: */
{
    MsfButtonGadget *pButton;
    MsfWidgetType  widgetType;
    MsfMainMenuWindow *pWin;
    MsfSelectgroup *pSelectGroup;

    if(!ISVALIDHANDLE(handle))
    {
      return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    if(MsfNotifyClick == notifyType)
    {
        pButton = (MsfButtonGadget *)seekWidget(handle, &widgetType);
        if(OP_NULL == pButton)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }      
        pWin = (MsfMainMenuWindow *)pButton->gadgetData.parent;
        if(OP_NULL == pWin)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        pSelectGroup = pWin->pSelectgroup;
        if(OP_NULL == pSelectGroup)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        if(pSelectGroup->choice.top_item > pSelectGroup->choice.elementsPerPage )
        {
            pSelectGroup->choice.top_item -= pSelectGroup->choice.elementsPerPage;
        }
        else
        {
           pSelectGroup->choice.top_item = 0;
        }
        HDIa_widgetSetInFocus(((MsfGadget *)pSelectGroup)->gadgetHandle, MSF_TRUE);      
    }
    else if( MsfNotifyPenMoveOutUp == notifyType )
    {
        pButton = (MsfButtonGadget *)seekWidget(handle, &widgetType);
        if(OP_NULL == pButton)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        pWin = (MsfMainMenuWindow *)pButton->gadgetData.parent;
        if(OP_NULL == pWin)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        pSelectGroup = pWin->pSelectgroup;
        if(OP_NULL == pSelectGroup)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        
        HDIa_widgetSetInFocus(((MsfGadget *)pSelectGroup)->gadgetHandle, MSF_TRUE);
    }
   return HDI_WIDGET_OK;
}



int widget_button_down_arrow_callback(MSF_UINT8 modID, /* modId */
                                      MSF_UINT32 handle ,           /*  widget handle */
                                      MsfNotificationType notifyType,  /*  state notification  */
                                      void *pExtraData )   /*  extra data. it is used  ONLY the MsfNotificationType is MsfNotifyStateChange. The meanings as following: */
{
    MsfButtonGadget *pButton;
    MsfWidgetType  widgetType;
    MsfMainMenuWindow *pWin;
    MsfSelectgroup *pSelectGroup;
    
    if(!ISVALIDHANDLE(handle))
    {
      return HDI_WIDGET_ERROR_INVALID_PARAM;
    }
    if(MsfNotifyClick == notifyType)
    {
        pButton = (MsfButtonGadget *)seekWidget(handle, &widgetType);
        if(OP_NULL == pButton)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        pWin = (MsfMainMenuWindow *)pButton->gadgetData.parent;
        if(OP_NULL == pWin)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        pSelectGroup = pWin->pSelectgroup;
        if(OP_NULL == pSelectGroup)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        if(pSelectGroup->choice.top_item +pSelectGroup->choice.elementsPerPage < pSelectGroup->choice.count - pSelectGroup->choice.elementsPerPage)
        {
            pSelectGroup->choice.top_item += pSelectGroup->choice.elementsPerPage;
        }
        else
        {
           pSelectGroup->choice.top_item = pSelectGroup->choice.count - pSelectGroup->choice.elementsPerPage;
        }
        HDIa_widgetSetInFocus(((MsfGadget *)pSelectGroup)->gadgetHandle, MSF_TRUE);
        
    }
    else if( MsfNotifyPenMoveOutUp == notifyType )
    {
        pButton = (MsfButtonGadget *)seekWidget(handle, &widgetType);
        if(OP_NULL == pButton)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        pWin = (MsfMainMenuWindow *)pButton->gadgetData.parent;
        if(OP_NULL == pWin)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        pSelectGroup = pWin->pSelectgroup;
        if(OP_NULL == pSelectGroup)
        {
            return HDI_WIDGET_ERROR_UNEXPECTED;
        }
        
        HDIa_widgetSetInFocus(((MsfGadget *)pSelectGroup)->gadgetHandle, MSF_TRUE);
    }
    return HDI_WIDGET_OK;
}


int widgetDrawAnnunciator()
{
  refresh_annuciator();
  return HDI_WIDGET_OK;
}

OP_BOOLEAN widgetGetComposedColor( DS_COLOREDCOMPOSER_T  *pCompose)
{
    return ds_get_composed_color(pCompose);
    
}








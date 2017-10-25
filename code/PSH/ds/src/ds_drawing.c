#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================
 
    MODULE NAME : ds_drawing.c

    GENERAL DESCRIPTION
        This file includes drawing modles using drawing primitives.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial creation

    01/13/2003   Steven Lai        PR-PP00005     fixed the bug that sms icon can't blink when sim is full
    01/21/2003   Steven Lai        PP00068         add a judgement in ds_set_screen_mode() to reduce some needless operation 
    02/12/2003   Steven Lai        PP00069         Update the type of some parameters; remove some useless statements
    02/24/2002   wangwei          PP00117     Change draw bitmap to draw icon when play animation.
    03/06/2003   wangwei          PP00130     Set mew emeishan image .
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    04/30/2003   linda wang       P000202     Fix bug 1026
    05/15/2003   linda wang       P000289     Extend the width and height defination from UINT8 to UINT16.
                                              set alarm and roam announciator in same position. 
    05/23/2003   linda wang       P000403     set mute announciator.
    06/09/2003   Chenyu       P000548     
                                    -Separate sublcd display from idle


    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    07/02/2003   lindawang        P000763     Fix draw bitmap file image problems.
    07/07/2003   linda wang       P000814     use UstrNullcpy() instead of Ustrcpy() when copy ustr.
    07/14/2003   linda wang       P000909     fix bug: when change color scheme, softkey bg is changed too.
    07/21/2003   Chenyu           P000878     -Add new main menu item for pacific
    07/25/2003   Chenyu           P001015     -Modify the display of annuciation
    07/26/2003   lindawang        P001074     Fix some wap draw image problem.
    08/04/2003   Chenyu           P001129     Add MMS icon
    08/14/2003   lindawang        P001241     Fix some ds and gif decode problems.
    08/20/2003   linda wang       P001293     Remove the number icon define into ds_drawing.h.    
    08/23/2003   Yuan Hui         P001353     add interface for stop annuciator blink timer.
    09/26/2003   linda wang       P001714     Provide WAP_REFRESH function for speeden refresh MMS&WAP module. 
    10/14/2003   linda wang       C001806     Add function of setting download image to desktop
    11/24/2003   Zhangxiaodong    P002103     Fix bug for 4 bit bitmap image.
    12/17/2003          linda wang       P002201     Add gif decode error handler and fix bug for download image display.
    12/04/2003   linda wang       p002147     Add qq and push image
    12/18/2003   Zhangxiaodong    P002218     improve the display rate    
    07/01/2003   Steven Li        p002325     Change ds_set_title() ds_clear_lines() to support ower draw title, line 
    02/06/2004   Steven Lai       p002440     Sometimes the result of ds_play_animation() is wrong
    02/09/2004   Liyingjun         p002449     Set display annunonce at all of screnn mode.
    02/16/2004   Zhuxq             P002469     replace XXX_forward functions with macro definitions
    02/17/2004   Zhuxq             P002483      Adjust the display system of widget to show up the annunciator and fix some bugs
    02/24/2004   chenxiao          p002530      modify WAP & MSG main menu     
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/11/2004   steven li        P002617     Delete init_scrollbar()at ds_set_screenmode
    03/11/2004    xcy               c002631         Add Xdict phone processing.
    03/12/2004   steven li        p002638     Change sub menu style same as second menu
    03/19/2004   Chenxiao         p002688      change main menu  of wap and fix some bugs from NEC feedback     
    03/20/2004   quzongqin        P002652     Add user define color theme.
    03/24/2004   chouwangyun   P002734     change resource and make new mainmenu back button and welcome style
    03/24/2004   linda wang       p002744     fix drawing scroll bar problem.
    03/30/2004   Chenxiao         p002758      fix bugs for bookmark and color scheme in the wap module
    03/30/2004   chouwangyun      p002788     fix bug for accessory, switch to focus in calendar list
    04/07/2004   lindawang        P002824     Fix bug for parser bitmap image.
    04/13/2004   chouwangyun   C002941   remove unnecessary resources
    04/13/2004   lindawang        p002976     Add deactive gprs annunciator.
    04/19/2004   lindawang       P003037      Fix problem of display missed call and connected annunciator.    
                                              - add extend softkey area function.
    04/23/2004   gongxiaofeng     p005058     add ds_set_scrollbar() function and ds_set_reverse(), 
                                             fixed a bug for sms in annuciator.  
    04/27/2004   penghaibo        p005111     fix wbmp file display error
    04/28/2004   gongxiaofeng     c005177     modify ds_set_reverse(), 
                                             
    04/24/2004   linda wang       c005088     Add jpg idle image and power on/off animation.
    04/29/2004   lindawang        P005196     Fix bug for draw softkey.    
    05/08/2004   linda wang       P005288     Fix bug for refresh softkey problem, cnxt00003633, cnxt00005165.
                                              - modify the draw text rectangle check condition.
    05/11/2004   lindawang        P005334     Fix bug for call connected annunciator display.    
    05/13/2004   linda wang       P005431     Use hardware decode photo image in display_graphic app..
    05/18/2004   linda wang       P005543     Fix menu display blink problem.
                                              Fix annunciator refresh problem in open window.
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    05/24/2004   Andrew           P005598     Don't allow user to input Chinese character in the password text input
    
    05/24/2004   penghaibo        p005629     fix jpg decode bug (add parameter to decode ds_* function)
    05/31/2004   linda            c005786     fix bug for display annunciator when phone lock in power up.
    06/1/2004    liren           P005827       adjust the position when drawing line & filling rect
    06/05/2004   gongxiaofeng     c005966       adjust selected icon display of group setting in pbook
    06/05/2004   penghaibo        p005951       fix write bitmap bug and add fatal error handle
    06/07/2004   linda wang       P005612     Fix bug for switch open window refresh and main lcd refresh.

    06/11/2004  liren              p006020    modify the ds_set_highlight function
    06/15/2004   liuqi           c006194       adjust the display of annunciator 
    06/21/2004   liren            p006345      add menufontcolor change to colortheme
    06/28/2004   penghaibo        p006396     fix bmp file parse bug
    07/08/2004   liren            c006711     change submenu style for Odin
    07/12/2004   liren          p006927    add openwindow animation
    07/16/2003   chenyong    C006702       modify answering icon.
    
    07/19/2004   Yun XiaoJun     P007048     Modify the RSSI annunciator.
    07/19/2004   liren          p007060    adjust openwindow animation
    07/29/2004   penghaibo        p007384     fix one byte bmp file parse bug    
    07/30/2004   gongxiaofeng     p007403    clear annunciator on power on status and remove functions.
    08/04/2004   liren          p007506     change openwindow display to reduce deep_sleep current
    08/12/2004   Yun XiaoJun     P007708     Modify ds_set_rssi().
    08/18/2004   liren          p007822     display the title in center
    09/07/2004   gongxiaofeng     p007403    resolve a problem that text can't be displayed when focus color is changed
    09/14.2004   liren            p008315    change the menu font color
    09/20/2004   liren            c008246     change mainmenu to four sheets style
This file includes drawing modles using drawing primitives.
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#ifdef WIN32
#include    "windows.h"
#include    "portab_new.h"
#else
#include    "portab.h"
#endif

#include    "rm_include.h"
#include    "SP_sysutils.h"

#include    "ds_lcd.h"
#include    "ds_def.h"
#include    "ds_int_def.h"
#include    "ds_drawing.h"
#include    "ds_font.h"
#include    "ds_util.h"
#include    "gsm_timer.h"
#include    "em_main.h"
#include    "gif_to_bmp.h"
#include    "jpg_to_bmp.h"
#include    "scrollbar.h"
#include    "SP_display.h"
#include    "CAM_interface.h"

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
#define WIN_LEFT                       ( ds.rcWin.left    )
#define WIN_RIGHT                      ( ds.rcWin.right   )

#define TITLE_TOP                      ds.title.y.top
#define TITLE_BOTTOM                   ds.title.y.bottom
 #define WIN_TOP                       ds.rcWin.top
/* BITMAPFILEHEADER+BITMAPINFOHEADER+RGBQUAD array+Corlor Index array*/
#define BYTELENGTH    8
#define OP_UINT8_SIZE             sizeof(OP_UINT8)
#define OP_UINT16_SIZE            sizeof(OP_UINT16)
#define OP_UINT32_SIZE            sizeof(OP_UINT32)

#define BMP_FILE_HEADER_SIZE     14*OP_UINT8_SIZE //sizeof(DS_BITMAPFILEHEADER)
#define BMP_INFO_HEADER_SIZE     40*OP_UINT8_SIZE //sizeof(DS_BITMAPINFOHEADER)
#define IMAGE_BMP_HEADERSIZE     (BMP_FILE_HEADER_SIZE+BMP_INFO_HEADER_SIZE)
#define IMAGE_GIF_HEADERSIZE              13 //GIFHEADER_SIZE
#define IMAGE_JPEG_HEADERSIZE             500
#define IMAGE_WBMP_HEADERSIZE            4
#define IMAGE_PNG_HEADERSIZE              10 // unknown
#define IMAGE_PICTOGRAM_HEADERSIZE       10 // unknown

#define IMAGE_1_BIT               1
#define IMAGE_4_BIT               4
#define IMAGE_8_BIT               8
#define IMAGE_16_BIT              16
#define IMAGE_24_BIT              24

#ifndef _WIN32
#define MAKEWORD(a, b)      ((OP_UINT16)(((OP_UINT8)(a)) | ((OP_UINT16)((OP_UINT8)(b))) << 8))
#endif

#define RLE_COMMAND      0
#define RLE_ENDOFLINE    0
#define RLE_ENDOFBITMAP  1
#define RLE_DELTA        2

#define TITLE_XOFFSET                  8
#define TITLE_ICON_XOFFSET             30
#define TITLE_ICON_XOFF                3
#define TITLE_BOTTOM_MARGINE           9 /* due to title image shape, this value shall be extraced from the 
                                              title bottom to get the center y positon */
#define TITLE_NORMAL_BASE_TOP          1    /* top y base for text writing in case of normal title */ 
#define TITLE_NORMAL_BASE_BOTTOM       19  /* bottom y base for text writing in case of normal title */    
#define TITLE_SUBMM_BASE_TOP           6    /* top y base for text writing in case of submenu of mainmenu title */ 
#define TITLE_SUBMM_BASE_BOTTOM        21  /* bottom y base for text writing in case of submenu of mainmenu title */  
#define TITLE_MSGDETAIL_BASE_TOP       31    /* top y base for text writing in case of message detail title */ 
#define TITLE_MSGDETAIL_BASE_BOTTOM    46  /* bottom y base for text writing in case of message detail title */ 

#define SUB_MENU_ITEMS                 4
#define SECOND_MENU_ITEMS              6
#define TITLE_NORMAL_ICON_TEXT_MARGINE   3
#define TITLE_ARROW_ICON_TEXT_MARGINE   2
#define TITLE_MSGDETAIL_ICON_TEXT_MARGINE   3

#define LINE_TEXT_OFFSET               3
#define LINE_ICON_HEIGHT               12
#define LINE_ICON_WIDTH                17
#define LINE_ICON_TEXT_MARGINE         3
#define LINE_BACK_ICON_X_MARGINE       13
#define LINE_BACK_ICON_XOFFSET         150
#define LINE_NARROW_SIDEBUTTON_WIDTH   (5 * C_WIDTH)
/* maximum number of RSSI bar from 0 to 6 */
#define MAX_RSSI_LEVEL                 7

#define    SUBMENU_TITLE_FONT_COLOR      COLOR_NAVY_BLUE    

#define TITLE_NUMBER_STARTX          10
#define TITLE_NUMBER_STARTX_BIG   6
#define ICON_MARGIN_FORNEWBG    30
#define REFRESH_WORD_Y                 39
#define OFFSET_Y                               2


/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/* Get Annuciator Maskbit */
#define GET_ANN_MASK(AMASK)            ( ds.annun_mask & AMASK )

/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
static  OP_BOOLEAN            isNeedStopBlinkTimer = OP_FALSE;
static  OP_BOOLEAN            bExtendSoftkey = OP_FALSE;
static  OP_BOOLEAN            bAnnunDisplayed = OP_TRUE;
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
SCRN_VAR_T         ds;               /* Disply system screen state variables */

extern OP_UINT8 Draw_SecondMenu(OP_UINT8 p8, OP_UINT32 p32); 
extern OP_UINT8 draw_SecondMenuLien(OP_UINT8 line, OP_UINT32 data);
extern int widgetMainMenuPlayAnimation( int iPlay );
/*
 *To speed the wap and mms module display, we provide the following function,
 *donot suggest other modules use these functions.
*/
extern void lcd_drv_draw_rectangle
(
    OP_UINT8 col1, 
    OP_UINT8 row1, 
    OP_UINT8 col2, 
    OP_UINT8 row2, 
    void    *color, 
    OP_UINT8 width, 
    OP_UINT8 start_row, 
    OP_UINT8 start_col
);


/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static  SCRNAREA_ENUM_T get_drawing_area
(
    DS_SCRNMODE_ENUM_T    scrn
);

static  SCRNAREA_ENUM_T get_text_icon_area
(
    DS_SCRNMODE_ENUM_T    scrn
);

static void load_default_image_ids
(
    RM_FEATURE_ID_T      feature_id,
    RM_RESOURCE_ID_T     default_res_id
);

static void register_ds_event_handler
(
    OPUS_EVENT_ENUM_TYPE event
);

static void deregister_ds_event_handler
(
    OPUS_EVENT_ENUM_TYPE event
);

static OP_INT16 get_line_text_x_pos
(
    OP_UINT8 line
);

static void refresh_cursor
(
    void 
);

static void ds_pass_event_to_ds
( 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess, 
    OP_BOOLEAN             *handle 
);

static void draw_animation_image 
(
    DS_ANI_T *ani
);

static OP_BOOLEAN GetBitmapFileHeader
(
    DS_BITMAPFILEHEADER  *pBmpFileHeader,
    DS_BITMAPINFOHEADER  *pBmpInfoHeader,
    OP_UINT8              *pSrcData
);

static OP_BOOLEAN ConvertBmp
(
    RM_BITMAP_T*  pBitmap,
    OP_UINT8 *    src, 
    DS_BITMAPINFOHEADER*  bmpinfo, 
    OP_UINT16     src_w_Start, 
    OP_UINT16     src_w_End,
    OP_UINT32     *bfMask
);
static void convert_1bit_image_to_bmp
(
    OP_UINT16   *dest, 
    OP_UINT8    *src, 
    OP_UINT16   width, 
    OP_UINT16   height,
    OP_BOOLEAN  bColor
);

static OP_BOOLEAN ds_parse_bitmap_image
(
    RM_BITMAP_T *pBitmap, 
    OP_UINT8    *pSrcData  
);
static void ds_parse_gif_image
(
    RM_BITMAP_T *pBitmap, 
    OP_UINT8    *pSrcData  
);
static OP_BOOLEAN ds_parse_jpg_image
(
    RM_BITMAP_T *pBitmap, 
    OP_UINT8    *pSrcData, 
    OP_UINT32   datasize
);
static void ds_parse_wbmp_image
(
    RM_BITMAP_T *pBitmap, 
    OP_UINT8    *pSrcData  
);

static OP_BOOLEAN draw_jpg_image
(
    OP_INT16 x,
    OP_INT16 y,
    RM_IMAGE_T *pImage
);

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: get_drawng_area 

    DESCRIPTION:
       Returns either TEXT or BITMAP screen area where graphical drawing is drawn according to the 
       screen mode.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Graphical drawing is one of drawin pixel, line, rect, button rect.
        Text and icons are drawn only to txt_screen or popup screen.
        Bitmap images are drawn only to bitmap screen or popup screen.
        Popup screen is a canvas where text, bitmap and graphical drawing is drawn.
        Popup window is handled separated by using pre-defined popup templated. And owner drawing
        style of popup is not allowed.
        All Editor is drawn as a popup window.
==================================================================================================*/
static SCRNAREA_ENUM_T get_drawing_area
(
    DS_SCRNMODE_ENUM_T    scrn
)
{
    
    SCRNAREA_ENUM_T    drawing_area;

    switch(scrn)
    {
        case SCREEN_GENERAL_BG:
        case SCREEN_FUNC_BG:
        case SCREEN_IDLE:
        case SCREEN_OWNERDRAW:
        case SCREEN_FULL_IMAGE:
        case SCREEN_MAINMENU:
           drawing_area = AREA_BITMAP;
           break;
        case SCREEN_MENU:
        case SCREEN_LIST:
        case SCREEN_FUNC:
        case SCREEN_GENERAL:
        case SCREEN_PLAIN_TEXT:
           drawing_area = AREA_TEXT;
           break;
        case SCREEN_OPENWINDOW:
           drawing_area = AREA_OPEN_WINDOW;
           break;
        default :
            drawing_area = AREA_BITMAP;
           break;
    }
    return drawing_area;
}


/*==================================================================================================
    FUNCTION: get_text_icon_area 

    DESCRIPTION:
       Returns either TEXT or BITMAP screen area where icon or text is drawn according to the 
       screen mode.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Text and icons are drawn only to txt_screen, bitmap or popup screen.
==================================================================================================*/
static  SCRNAREA_ENUM_T get_text_icon_area
(
    DS_SCRNMODE_ENUM_T    scrn
)
{
    
    SCRNAREA_ENUM_T    drawing_area;

    switch(scrn)
    {
        case SCREEN_FULL_IMAGE:
        case SCREEN_MAINMENU:
           drawing_area = AREA_BITMAP;
           break;
        case SCREEN_GENERAL_BG:
        case SCREEN_FUNC_BG:
        case SCREEN_IDLE:
        case SCREEN_OWNERDRAW:
        case SCREEN_MENU:
        case SCREEN_LIST:
        case SCREEN_FUNC:
        case SCREEN_GENERAL:
        case SCREEN_PLAIN_TEXT:
           drawing_area = AREA_TEXT;
           break;
        case SCREEN_OPENWINDOW:
           drawing_area = AREA_OPEN_WINDOW;
           break;
        default :
            drawing_area = AREA_TEXT;
            break;
    }
    return drawing_area;
}

/*==================================================================================================
    FUNCTION: draw_maksed_annuciators

    DESCRIPTION:
        When switched back from non-displaying annuciator mode to displaying annuciator mode,
        draw the currently masked on annuciators.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_all_maksed_annuciators
(
    void
)
{
    ANNUN_MASK    amask;
    OP_UINT8      ann;
    OP_BOOLEAN    blinking;

    /* draw RSSI */
    ds.rssi_level = (ds.rssi_level > 6)?6:ds.rssi_level;
    ds.rssi_level = (ds.rssi_level < 0)?0:ds.rssi_level;
    blinking = ((ds.annun_blink_mask & AMASK_RSSI) > 0)?OP_TRUE: OP_FALSE;
    ds_set_annuciator((DS_ANNUN_ENUM_T)(ANNUN_RSSI0+ds.rssi_level), OP_TRUE, blinking);
    /* draw Battery level */
    ds.batt_level = (ds.batt_level > 4)?4:ds.batt_level ;
    ds.batt_level = (ds.batt_level < 0)?0:ds.batt_level ;
    blinking = ((ds.annun_blink_mask & AMASK_BATT) > 0 )?OP_TRUE:OP_FALSE;
    ds_set_annuciator((DS_ANNUN_ENUM_T)(ANNUN_BATT0+ds.batt_level), OP_TRUE, blinking);

    amask = AMASK_MSG;
    for (ann = ANNUN_MSG; ann<= ANNUN_GPRS; ann++)
    {
        if ( ds.annun_mask & amask)
        {
            ds_set_annuciator((DS_ANNUN_ENUM_T)ann, OP_TRUE, (OP_BOOLEAN)((ds.annun_blink_mask & amask)?OP_TRUE:OP_FALSE));
        }
        /* chnage to next annuciator mask bit */
        amask = amask << 1;
    }
}


/*==================================================================================================
    FUNCTION: load_default_image_ids

    DESCRIPTION:
        Load default event animations and idle screen bitmaps resouce ids if the feature value
        is null resouce id.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static void load_default_image_ids
(
    RM_FEATURE_ID_T      feature_id,
    RM_RESOURCE_ID_T     default_res_id
)
{
    RM_RESOURCE_ID_T   res_id;
    /* Power on/off animations*/
    RM_GetFeature(feature_id, &res_id);
    if (res_id == NULL_RESOURCE)
    {
        res_id = default_res_id;
        RM_SetFeature(feature_id, &res_id);
    } 
}

/*==================================================================================================
    FUNCTION: register_ds_event_handler

    DESCRIPTION:
        Register DS specific event handler to the EM to get the specified event.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static void register_ds_event_handler
(
    OPUS_EVENT_ENUM_TYPE event
)
{
    EM_register_sys_handler(OPUS_SYS_DS_HANDLER, (void *)ds_pass_event_to_ds, event);
}

/*==================================================================================================
    FUNCTION: deregister_ds_event_handler

    DESCRIPTION:
        De-register DS specific event handler from the EM.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static void deregister_ds_event_handler
(
    OPUS_EVENT_ENUM_TYPE event
)
{
    EM_deregister_sys_handler(OPUS_SYS_DS_HANDLER);
}


/*==================================================================================================
    FUNCTION: get_line_text_x_pos

    DESCRIPTION:
        Return x coordinate of a line accoding to the line attributes.
        The calculation is based on ds_refresh_lines_items().
        It is only for SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static OP_INT16 get_line_text_x_pos
(
    OP_UINT8 line
)
{
    OP_UINT16       icon_width;
    OP_UINT8       font_width = FONTX_NORMAL;
    OP_INT16       x;
    OP_INT16       x_text_limit;
    OP_INT16       y_text;
    OP_INT16       string_width;
    DS_LINEATTR    lineattr;

    if ( (ds.scrnMode != SCREEN_LIST) && (ds.scrnMode != SCREEN_MENU) &&
          (ds.scrnMode != SCREEN_FUNC) &&(ds.scrnMode != SCREEN_FUNC_BG))
    {
       return 0;
    }

    x        = WIN_LEFT;    
    y_text   = ds.line[line].base.top+1;

    lineattr = ds.line[line].attr;
    x_text_limit = WIN_RIGHT;  

    /* Draw bg bitmap */               
    if (lineattr & LA_BG_BITMAP) 
    {  
        if (ds.line[line].bitmap) 
        {
             x_text_limit = MIN(LCD_MAX_X_COOR, (OP_INT16)(x + ds.line[line].bitmap->biWidth -1));
        } 
    }

    if (lineattr & LA_SUBITEM) 
    {  
        x += C_WIDTH; /* give one charcter space */
    }
                 
    if ( (lineattr & LA_RADIOBUTTON) || (lineattr & LA_CHECKBOX)  || (lineattr & LA_LINENUMBER) )
    { 
        x += LINE_ICON_WIDTH + LINE_ICON_TEXT_MARGINE; 
    }
    else if (lineattr & LA_ICON) 
    {  
        icon_width = LINE_ICON_WIDTH;
        if (ds.line[line].icon) 
        {
            if (ds.line[line].icon->data)
            { 
                icon_width = ds.line[line].icon->biWidth;
                icon_width = (icon_width > LINE_ICON_WIDTH) ? icon_width : LINE_ICON_WIDTH;
            } 
        }

        x += (OP_INT16)(icon_width + LINE_ICON_TEXT_MARGINE); 
   }

    /* line back attributes */
   if ( (lineattr & LA_BACK_CHECKMARK) ||  (lineattr & LA_BACK_CHECKBOX) || (lineattr & LA_BACK_ICON))
   { 
       x_text_limit = (OP_INT16)(LINE_BACK_ICON_XOFFSET-1);;
   }

    /* line arrows */
    if (lineattr & LA_SIDEBUTTON)  /* Draw Left and Right arrow at both end of the line. */
    {  
        x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_RIGHT) - 2);
        x += (ds_get_image_width(ICON_ARROW_LEFT) + 1); 
    }
    else if (lineattr & LA_NARROW_SIDEBUTTON)  
    {  
        x_text_limit = (OP_INT16)(x+font_width+LINE_NARROW_SIDEBUTTON_WIDTH -2);
        x += (ds_get_image_width(ICON_ARROW_LEFT) + 1); 
    }
    else if (lineattr & LA_RIGHTBUTTON) 
    {
        x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_RIGHT - 1));
    }
    else if (lineattr & LA_LEFTBUTTON) 
    {
        x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_LEFT - 1));
    }
    else if (lineattr & LA_UPBUTTON) 
    {
        x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_UP - 1));
    }
    else if (lineattr & LA_DOWNBUTTON) 
    {
        x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_DOWN - 1));
    }

    if ( (lineattr & LA_BUTTONSTYLE)  || (lineattr & LA_EDITLINE))
    {
        x +=2;
        x_text_limit -=2;
    }
    
         
    /* in case of small font, write the text below in one pixel */
    if (lineattr & LA_SMALLFONT)
    {
        y_text ++;
        font_width = FONTX_SMALL;
    } 
    
    if (lineattr & LA_CENTER_TEXT)  /* center text */
    { 
        if (IS_NOT_NULL_STRING(ds.line[line].txt)) 
        {
            string_width = ds.line[line].leng*font_width;
            if ( string_width < (x_text_limit-x+1))
            {
                x = (OP_INT16)(GET_CENTER(x, x_text_limit, string_width));
            }
        }
    }
    else if (lineattr & LA_RIGHT_TEXT)  /* right alignment */
    {  
        if (IS_NOT_NULL_STRING(ds.line[line].txt)) 
        {
            x = (OP_INT16)(x_text_limit - ds.line[line].leng*font_width-1); 
        }
    } 
    
    return x;

}   

/*==================================================================================================
    FUNCTION: refresh_cursor

    DESCRIPTION:
        Refresh the specified cursor when cursor blinking timer is timed-out, or character cursor is 
        changed.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static void refresh_cursor
(
    void
)
{
    OP_UINT8       font_width = FONTX_NORMAL;
    OP_UINT8       font_height = FONTY_NORMAL;
    DS_FONTATTR    fontattr = ds.cCursor.attr;
    DS_FONTATTR    space_fontattr = ds.cCursor.attr;
    DS_LINEATTR    lineattr = ds.line[ds.cCursor.line].attr;
    DS_COLOR       font_bg = COLOR_WINBG;
    OP_BOOLEAN     is_full_screen_image = OP_FALSE;
    OP_INT16       x = 0;
    OP_INT16       y;
    LCD_RECT_T     rect;
    OP_UINT8       space[2] = {0x20, 0x00};

    if ( IS_NOT_NULL_STRING(ds.cCursor.cursor) && (ds.cCursor.line != CURSOR_OFF) && (ds.cCursor.offset < DS_SCRN_CWIDE))
    {
        if (ds.cCursor.blink_status == OP_FALSE)
        {
            fontattr |= FONT_OVERLAP;
        } 
        else
        {
            fontattr &= ~FONT_OVERLAP;
        } 
    
        if (fontattr & LA_SMALLFONT)
        {
            font_width = FONTX_SMALL;
            font_height = FONTY_SMALL;
        } 

        if ( (ds.scrnMode == SCREEN_LIST) || (ds.scrnMode == SCREEN_MENU) ||
              (ds.scrnMode == SCREEN_FUNC) ||(ds.scrnMode == SCREEN_FUNC_BG))
        {
            x = ( get_line_text_x_pos(ds.cCursor.line) + font_width*ds.cCursor.offset);
            y = ds.line[ds.cCursor.line].base.top+1;
            /* assume that in these screen mode, only highlited line set the char cursor */
            if (lineattr & LA_EDITLINE)
            {
                font_bg = COLOR_EDIT_BTNFILL;
            } 
            else if (lineattr & LA_BUTTONSTYLE)
            { 
                font_bg = COLOR_FILL_BTNFILL;
            }
            else
            { 
                font_bg = COLOR_CURSORBG;
            }
        }
        else
        {
            x = (OP_INT16)(x+font_width*ds.cCursor.offset);
            y = (OP_INT16)(TITLE_BOTTOM +1 + ds.cCursor.line * LINE_HEIGHT);   
            font_bg = COLOR_WINBG;     
        }

        if (fontattr & LA_SMALLFONT)
        {
            y ++;
        } 

        /* clear the character region */   
        space_fontattr  &= ~FONT_OVERLAP;         
        
        draw_char(AREA_TEXT, 
                    x, 
                    y, 
                    space, 
                    space_fontattr , 
                    ds.cCursor.fontcolor, 
                    font_bg);

                    
        draw_char(AREA_TEXT, 
                    x, 
                    y, 
                    (OP_UINT8 *)ds.cCursor.cursor, 
                    fontattr, 
                    ds.cCursor.fontcolor, 
                    ds.cCursor.backcolor);
    
    
        rect.left = x;
        rect.top  = y;
        rect.right = (OP_INT16)(x + font_width -1);
        rect.bottom = (OP_INT16)(y+font_height-1);
              
        if (ds.scrnMode == SCREEN_IDLE)
        {
            is_full_screen_image = OP_TRUE;
        }
        lcd_refresh(AREA_TEXT, is_full_screen_image, ds_get_disp_annun(), &rect, OP_NULL, scramble_color[ds_get_color_theme()]); 
        UPDATECLEAR(AREA_TEXT);
    }
}

/*==================================================================================================
    FUNCTION: refresh_annuciator

    DESCRIPTION:
        Refresh annuciator area immediatedly.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
 void refresh_annuciator
(
    void 
)
{
    OP_BOOLEAN is_full_screen_image = OP_FALSE;
    LCD_RECT_T    rect;

    /* when in power on status, donot display annunciator */
    if(SP_get_power_on_status() == POWER_ON_STATUS_INIT)
    {
        return;
    }
    if (ds_get_disp_annun())
    {
        /* once cover close, donot refresh main lcd */
        if((SP_status_get_openwindow_display())&&
            (SUBLCD_DISPLAY_ANIMATION_MODE != SP_get_sublcd_screen_mode())&&
            (POWER_ON_STATUS_IDLE_IDLE == SP_get_power_on_status())&&
            (!SP_is_in_deep_sleep())
           )
        {
            rect.left = 0;
            rect.top = LCD_TIMEDISPLAY_START_Y - 3*OPENWINDOW_STATUS_ICON_HEIGHT/2;
            rect.right = LCD_MAX_X_COOR;
            rect.bottom = LCD_TIMEDISPLAY_START_Y + 3*OPENWINDOW_STATUS_ICON_HEIGHT/2+ds_get_image_height(ICON_IDLE_TIMENUM_0);
            
            lcd_openwindow_refresh(rect);

          /*  op_memcpy(&lcd_screen[LCD_TIMEDISPLAY_START_Y - 3*OPENWINDOW_STATUS_ICON_HEIGHT/2][0],
                      &open_window_screen[LCD_TIMEDISPLAY_START_Y - 3*OPENWINDOW_STATUS_ICON_HEIGHT/2][0],
                      LCD_MAX_X*(ds_get_image_height(ICON_IDLE_TIMENUM_0) + 3*OPENWINDOW_STATUS_ICON_HEIGHT)*sizeof(DS_COLOR)
                      );
            lcd_drv_draw_rectangle(0, 
                                   (OP_UINT8)(LCD_TIMEDISPLAY_START_Y - 3*OPENWINDOW_STATUS_ICON_HEIGHT/2), 
                                   LCD_MAX_X_COOR, 
                                   (OP_UINT8)(LCD_TIMEDISPLAY_START_Y + 3*OPENWINDOW_STATUS_ICON_HEIGHT/2+ds_get_image_height(ICON_IDLE_TIMENUM_0) ), 
                                   lcd_screen, 
                                   LCD_MAX_X_COOR, 
                                   0, 
                                   0);
            */
            UPDATECLEAR(AREAMASK_OPENWINDOW);
            return;
        }   
   
        if (ds.scrnMode == SCREEN_IDLE)
        {
            is_full_screen_image = OP_TRUE;
        }
        //lcd_refresh(AREA_ANNUN, is_full_screen_image, OP_TRUE, OP_NULL, OP_NULL, scramble_color[ds_get_color_theme()]); 
        op_memcpy(lcd_screen, annun_screen, LCD_MAX_X*LCD_ANNUN_HIGH*sizeof(DS_COLOR)); 
        ds_refresh_lcd_rect (0,0, LCD_MAX_X-1, LCD_ANNUN_HIGH-1);

        UPDATECLEAR(AREA_ANNUN);
    }   
}

/*==================================================================================================
    FUNCTION: draw_animation_iamge 

    DESCRIPTION:
       Draw the active animaiton. 

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static void draw_animation_image 
(
    DS_ANI_T *ani
)
{
    if (ani->type == ANI_NORMAL)
    {
        if(ani->data->encode == JPGANI)
	    {
            draw_jpg_image(ani->dx, ani->dy,((RM_IMAGE_T *)(ani->data->scene))+ani->frame);
	    }
        else
        {
	        if(ani->data->image_type == ANI_ICON)
	        {
	            draw_icon(ani->area, ani->dx, ani->dy, 
	                (((RM_ICON_T *)ani->data->scene)+ani->frame));
	        }
	        else 
	        {
	            draw_bitmap_image(ani->area, ani->dx, ani->dy, 
	                    (((RM_BITMAP_T *)ani->data->scene)+ani->frame));
	        }
        }
    }
    else  /* ANI_RESIZED */
    {
        if(ani->data->image_type == ANI_ICON)
        {
            draw_resized_icon(ani->area, ani->dx, ani->dy, ani->dw, ani->dh, 
                ani->sx, ani->sy, (((RM_ICON_T *)ani->data->scene)+ani->frame));
        }
        else
        {
            draw_resized_bitmap_image(ani->area, ani->dx, ani->dy, ani->dw, ani->dh,
                ani->sx, ani->sy, (((RM_BITMAP_T *)ani->data->scene)+ani->frame));
        }
    }
}

/*==================================================================================================
    FUNCTION: GetBitmapFileHeader 

    DESCRIPTION:
        parse bitmap file header and convert data.

    ARGUMENTS PASSED:
        DS_BITMAPFILEHEADER  *pBmpFileHeader: bitmap file header pointer.
        DS_BITMAPINFOHEADER  *pBmpInfoHeader: bitmap file info header pointer.
        OP_UINT8              *pSrcData: resource data pointer.
    RETURN VALUE:
        return true when success.
    IMPORTANT NOTES:
        None.
==================================================================================================*/
OP_BOOLEAN GetBitmapFileHeader
(
    DS_BITMAPFILEHEADER  *pBmpFileHeader,
    DS_BITMAPINFOHEADER  *pBmpInfoHeader,
    OP_UINT8              *pSrcData
)
{
    OP_UINT8*  pData;

    pData = pSrcData;
    op_memcpy(&(pBmpFileHeader->bfType), pData, OP_UINT16_SIZE);
    /* bfType */
    pData = pSrcData + OP_UINT16_SIZE;
    op_memcpy(&(pBmpFileHeader->bfSize), pData, OP_UINT32_SIZE);
    /* (bfSize + bfReserved1 + bfReserved2) */
    pData = pData + OP_UINT32_SIZE*2;
    op_memcpy(&(pBmpFileHeader->bfOffBits), pData, OP_UINT32_SIZE);
    
    /* check if BITMAP file, the first WORD is "BM"*/
    if (pBmpFileHeader->bfType != (('M'<<BYTELENGTH) | 'B'))     
    { 
        return OP_FALSE; 
    } 
     
    /* point to file info header */
    pData += OP_UINT32_SIZE;
    op_memcpy(&(pBmpInfoHeader->biSize), pData, OP_UINT32_SIZE);
    /* biSize */
    pData += OP_UINT32_SIZE;
    op_memcpy(&(pBmpInfoHeader->biWidth), pData, OP_UINT32_SIZE);
    /* biWidth */
    pData += OP_UINT32_SIZE;
    op_memcpy(&(pBmpInfoHeader->biHeight), pData, OP_UINT32_SIZE);
    /* biHeight + biPlanes */
    pData += OP_UINT32_SIZE + OP_UINT16_SIZE; 
    op_memcpy(&(pBmpInfoHeader->biBitCount), pData, OP_UINT16_SIZE);
    /* biBitCount */
    pData += OP_UINT16_SIZE; 
    op_memcpy(&(pBmpInfoHeader->biCompression), pData, OP_UINT32_SIZE);
    /* biCompression */
    pData += OP_UINT32_SIZE; 
    op_memcpy(&(pBmpInfoHeader->biSizeImage), pData, OP_UINT32_SIZE);
    /*biSizeImage*/
    pData += OP_UINT32_SIZE;
    op_memcpy(&(pBmpInfoHeader->biXPelsPerMeter),pData,OP_UINT32_SIZE);
    /*biXPelsPerMeter*/
    pData += OP_UINT32_SIZE;
    op_memcpy(&(pBmpInfoHeader->biYPelsPerMeter),pData,OP_UINT32_SIZE);
    /*biYPelsPerMeter*/
    pData += OP_UINT32_SIZE;
    op_memcpy(&(pBmpInfoHeader->biClrUsed),pData,OP_UINT32_SIZE);
    /*biClrUsed*/
    pData += OP_UINT32_SIZE;
    op_memcpy(&(pBmpInfoHeader->biClrImportant),pData,OP_UINT32_SIZE);
    /*biClrImportant*/

      /* check if MS bitmap */
    if (pBmpInfoHeader->biSize < BMP_INFO_HEADER_SIZE)                // not MS bitmap
    { 
        return OP_FALSE;
    }
    
    /* check if valid bitcount */
    if ( (pBmpInfoHeader->biBitCount != IMAGE_1_BIT)
         && (pBmpInfoHeader->biBitCount != IMAGE_4_BIT)
         && (pBmpInfoHeader->biBitCount != IMAGE_8_BIT)
         && (pBmpInfoHeader->biBitCount != IMAGE_16_BIT)
         && (pBmpInfoHeader->biBitCount != IMAGE_24_BIT) )
    {
        return OP_FALSE;
    }
    return OP_TRUE;
}

/*==================================================================================================
    FUNCTION: ConvertBmp 

    DESCRIPTION:
       convert bitmap file data and put into dest buffer. 
       
    ARGUMENTS PASSED:
    RETURN VALUE:
    IMPORTANT NOTES:
        None.
==================================================================================================*/
OP_BOOLEAN ConvertBmp
(
    RM_BITMAP_T*  pBitmap,
    OP_UINT8 *    src, 
    DS_BITMAPINFOHEADER*  bmpinfo, 
    OP_UINT16     src_w_Start, 
    OP_UINT16     src_w_End,
    OP_UINT32    *bfMask 
)
{
    OP_UINT8     biBitCnt; 
    OP_UINT16    Height;
    OP_UINT16    Width;
    OP_UINT32    Index;
    OP_UINT16    wPixcel;
    DS_RGBQUAD  *bmpColors;
    OP_UINT8     i;
    OP_UINT8     first = 0;
    OP_UINT16 *  dest;
    OP_UINT16 src_h_Start;
    if(pBitmap->data == OP_NULL || src == OP_NULL)
        return OP_FALSE;
    biBitCnt = pBitmap->biBitCount;
    src_h_Start = (OP_UINT16)bmpinfo->biHeight;
    dest = (OP_UINT16 *)pBitmap->data; 
        bmpColors = (DS_RGBQUAD *)pBitmap->ColorPallet;
    /* 
     * first, I should caculate the index for our bitmap
     * for Windows OS, the bitmap will start to show from left-bottom
     * but for our display format, the bitmap will start to show from left-up
     * the following code are to convert 24,16,8,4,1 bits color to 16-bits color
     */
    if(biBitCnt == IMAGE_24_BIT)
    {
        for (Height = src_h_Start; Height > 0 ;Height --)
        {
            for (Width = src_w_Start ; Width > src_w_End ; Width -= 3)
            {
                Index = (Height * src_w_Start) - Width;

                wPixcel = _RGB(src[Index+2],src[Index + 1],src[Index]);

                /* put the wPixcel into dest buffer*/
                *dest = (DS_COLOR)wPixcel;
                dest ++;
            }
        }
    } /* end of the process of 24-bits color bitmap */
    else if(biBitCnt == IMAGE_16_BIT)/* process 16-bits color bitmap */
    {
    	OP_UINT32 ns[3]={0,0,0};
        OP_UINT32 i;
        OP_UINT32 bluemask,greenmask,redmask;
        OP_UINT8  R,G,B;
		// compute the number of shift for each mask
        redmask= bfMask[0];
        greenmask = bfMask[1];
        bluemask = bfMask[2];
		for ( i=0;i<16;i++){
			if ((redmask>>i)&0x01) ns[0]++;
			if ((greenmask>>i)&0x01) ns[1]++;
			if ((bluemask>>i)&0x01) ns[2]++;
		}
		ns[1]+=ns[0]; ns[2]+=ns[1];	ns[0]=8-ns[0]; ns[1]-=8; ns[2]-=8;
        
        for (Height = src_h_Start; Height > 0 ;Height --)
        {
            for (Width = src_w_Start ; Width > src_w_End; Width -= 2)
            {
                Index = (Height * src_w_Start) - Width;
                wPixcel = (src[Index+1]<<8)|src[Index]; 
                B=(OP_UINT8)((wPixcel & bluemask)<<ns[0]);
				G=(OP_UINT8)((wPixcel & greenmask)>>ns[1]);
				R=(OP_UINT8)((wPixcel & redmask)>>ns[2]);

                wPixcel = _RGB(R,G,B);
                /* put the wPixcel into dest buffer*/
                *dest = (DS_COLOR)wPixcel;
                dest ++;
            }
        }
    }/*end of 16 bits image*/ 
    else if(biBitCnt == IMAGE_8_BIT)/* process 8-bits color bitmap */
    {
        if(DS_BI_RLE8 == bmpinfo->biCompression)
            {
                OP_UINT8 *status_byte;
                OP_UINT8 *second_byte;
                OP_UINT8  idx;
                BOOL bContinue;
                OP_UINT8 i;
                int scanline = 0;
                int bits = 0;
                dest += bmpinfo->biWidth * (bmpinfo->biHeight - 1);
                for (bContinue = TRUE; bContinue; ) {
                    status_byte = src++ ;
                    switch (*status_byte) {
                        case RLE_COMMAND :
                            status_byte = src++;
                            switch (*status_byte) {
                                case RLE_ENDOFLINE :
                                    bits = 0;
                                    scanline++;
                                    dest = (OP_UINT16 *)pBitmap->data + bmpinfo->biWidth * (bmpinfo->biHeight - scanline -1);
                                    break;
                                case RLE_ENDOFBITMAP :
                                    bContinue=FALSE;
                                    break;
                                case RLE_DELTA :
                                {
                                    // read the delta values
                                    OP_UINT8 *delta_x;
                                    OP_UINT8 *delta_y;
                                    delta_x = src++;
                                    delta_y = src++;
                                    // apply them
                                    bits     += *delta_x;
                                    scanline += *delta_y;
                                    dest = (OP_UINT16 *)pBitmap->data + bmpinfo->biWidth * bmpinfo->biHeight - (scanline-1) * bmpinfo->biWidth + bits; 
                                    break;
                                }
                                default :
                                    for(i=0;i<*status_byte;i++)
                                        {
                                            second_byte = src++;
                                            idx = * second_byte;
                                            *dest = _RGB(bmpColors[idx].rgbRed,bmpColors[idx].rgbGreen,bmpColors[idx].rgbBlue);
                                            dest++;
                                        }
                                    // align run length to even number of bytes 
                                    if ((*status_byte & 1) == 1)
                                        second_byte = src++;                                                
                                    bits += *status_byte;                                                    
                                    break;                                
                            };
                            break;
                        default :
                            second_byte = src++;
                            idx = * second_byte;
                            for (i = 0; i < *status_byte; i++) {
                                *dest = _RGB(bmpColors[idx].rgbRed,bmpColors[idx].rgbGreen,bmpColors[idx].rgbBlue);
                                dest++;
                                bits++;                    
                            }
                            break;
                    };
                }
            
                }
        else
            {
        for (Height = src_h_Start; Height > 0; Height --)
        {   
            for (Width = src_w_Start ; Width > src_w_End; Width -= 1)
            {
                Index = (Height * src_w_Start) - Width;

                /* convert 8 bits value into 16 bits */
                        wPixcel = _RGB(bmpColors[src[Index]].rgbRed,bmpColors[src[Index]].rgbGreen,bmpColors[src[Index]].rgbBlue);
                
                *dest = (DS_COLOR)wPixcel;
                dest ++;
            }
        }
    }
    }
    else if(biBitCnt == IMAGE_4_BIT)/* process 4-bits color bitmap */
    {
        if(DS_BI_RLE4 == bmpinfo->biCompression)
            {
            OP_UINT8 *status_byte = OP_NULL;
            OP_UINT8 *second_byte = OP_NULL;
            OP_UINT16 scanline = 0;
            OP_UINT16 bits = 0;
            OP_UINT8 i;
            OP_UINT8  idx;
            BOOL bContinue;
            BOOL low_nibble = FALSE;
            dest += bmpinfo->biWidth * (bmpinfo->biHeight - 1);
            for (bContinue = TRUE; bContinue;) 
                {
                status_byte = src++;
                switch (*status_byte) 
                    {
                    case RLE_COMMAND :
                        status_byte = src++;
                        switch (*status_byte) 
                            {
                            case RLE_ENDOFLINE :
                                bits = 0;
                                scanline++;
                                dest = (OP_UINT16 *)pBitmap->data + bmpinfo->biWidth * (bmpinfo->biHeight - scanline -1);
                                low_nibble = FALSE;
                                break;
                            case RLE_ENDOFBITMAP :
                                bContinue=FALSE;
                                break;
                            case RLE_DELTA :
                                {
                                // read the delta values
                                OP_UINT8 *delta_x;
                                OP_UINT8 *delta_y;
                                delta_x=src++;
                                delta_y=src++;
                                // apply them
                                bits       += *delta_x;
                                scanline   += *delta_y;
                                dest = (OP_UINT16 *)pBitmap->data + bmpinfo->biWidth * bmpinfo->biHeight - (scanline-1) * bmpinfo->biWidth + bits; 
                                break;
                                }
                            default :
                                second_byte=src++;
                                for (i = 0; i < *status_byte; i++) {
                                    if (low_nibble)
                                        {
                                        idx =  (OP_UINT8)(*second_byte & 0x0F);
                                        *dest = _RGB(bmpColors[idx].rgbRed,bmpColors[idx].rgbGreen,bmpColors[idx].rgbBlue);
                                        dest++;
                                        if (i != (*status_byte - 1))
                                            second_byte = src++;
                                        } 
                                    else 
                                        {
                                        idx = (OP_UINT8)((*second_byte>>4) & 0x0F);
                                        *dest = _RGB(bmpColors[idx].rgbRed,bmpColors[idx].rgbGreen,bmpColors[idx].rgbBlue);
                                        dest++;
                                        }
                                    low_nibble = !low_nibble;
                                    }
                                if ((((*status_byte+1) >> 1) & 1 )== 1)
                                    second_byte=src++;                                                
                                break;
                            };
                        break;
                            default :
                                {
                                second_byte=src++;
                                for ( i = 0; i < *status_byte; i++) 
                                    {
                                    if (low_nibble) 
                                        {
                                            idx= (OP_UINT8)(*second_byte & 0x0F);
                                            *dest = _RGB(bmpColors[idx].rgbRed,bmpColors[idx].rgbGreen,bmpColors[idx].rgbBlue);
                                            dest++;
                                        } 
                                    else 
                                        {
                                            idx = (OP_UINT8)((*second_byte)>>4 & 0x0F);
                                            *dest = _RGB(bmpColors[idx].rgbRed,bmpColors[idx].rgbGreen,bmpColors[idx].rgbBlue);
                                            dest++;
                                         }                
                                        low_nibble = !low_nibble;
                                    }
                                }
                                break;
                    };
                }
            }
        else
            {
        for (Height = src_h_Start; Height > 0 ;Height --)
        {
            for (Width = src_w_Start ; Width > src_w_End ; Width -= 1)
            {
                Index = (Height * src_w_Start) - Width;
                /* one byte represents two colors */
                for( i=0; i<2; i++)
                {
                    if(i == 0) /* the first half byte */
                    {
                        first = (src[Index]>>4) & 0x0F;
                    }
                    else if((pBitmap->biWidth%2)&&(Width == src_w_End+1)) /* the last half byte */
                    {
                        break;
                    }
                    else
                    {
                        first = src[Index]&0x0F;
                    }
                        wPixcel =  _RGB(bmpColors[first].rgbRed,bmpColors[first].rgbGreen,bmpColors[first].rgbBlue);
    
                    /* put the wPixcel into dest buffer*/
                    *dest = (DS_COLOR)wPixcel;
                    dest ++;
                }
            }
        }
    }
    }
    else if(biBitCnt == IMAGE_1_BIT) // process 1-bits color bitmap 
    {
        OP_UINT16 bitcount;
        for (Height = src_h_Start; Height > 0 ;Height --)
            {
            bitcount = 0;
            for (Width = src_w_Start ; Width > src_w_End ; Width -= 1)
                {
                Index = (Height * src_w_Start) - Width;
                // one byte represents 8 pixcel colors 
                for (i=0; i<BYTELENGTH; i++)
                    {
                    bitcount ++;
                    if ((src[Index] << i) & 0x80)
                        {
                           wPixcel = _RGB(bmpColors[1].rgbRed,bmpColors[1].rgbGreen,bmpColors[1].rgbBlue);;
                        }
                    else
                        {
                           wPixcel = _RGB(bmpColors[0].rgbRed,bmpColors[0].rgbGreen,bmpColors[0].rgbBlue);;
                        }
                    *dest = wPixcel;
                    dest++;
    
                    if (bitcount >= pBitmap->biWidth)
                        {
                        break;
                        }
                    }
                }
            }        
    }

    return OP_TRUE;
}

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: ds_init

    DESCRIPTION:
        Initialize display system state variables, buffer. Call this only dring phone initialization.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_init_ds
( 
    void 
)
{
    USERDEFINE_COLORSCHEME_T ret_nvm_data;
    OP_UINT8 color_scheme = COLOR_THEME_1;
    DS_COLOR color = COLOR_WHITE;

    /* DS LCD Init */
    lcd_init();
    sublcd_init();
//NVMGR_set_default();

    /* Font data init  */
    font_init_fontsets();

    /* DS variable initialization */    
    ds.bg_updated    = OP_TRUE;
    ds.update_area   = AREAMASK_NULL; /* ANNUN + TEXT + BITMAP + SKEY */
    ds.scrnMode      = SCREEN_NONE;
    ds.disp_annun    = OP_FALSE;

    ds_init_scrn_attr();
    ds_init_softkeys();

    ds_clear_lines();    /* Clear Text line data */
    ds_clear_items();    /* Clear graphical item data */

    ds.focused_line  = CURSOR_OFF;   
    ds.lines         = 0;          
    ds.items         = 0;            
    ds.max_lines     = 0;      
    ds.refresh_func  = OP_NULL;

    /*
    if (RM_GetFeature( FEATURE_COLOR_SCHEME_TYPE, &color_scheme) != RM_SUCCESS)
    {
        color_scheme = COLOR_THEME_1;        
    }        
    */
    
    /* title is not change */
	ds.cComposer.titleColor = color;
    /* get color scheme from flash */
    SP_Get_Current_ColorValue(&ret_nvm_data,&color_scheme);

    if(color_scheme == COLOR_THEME_USER_DEFINE)
    {
        /* get base color type */
        color=_RGB(ret_nvm_data.type[COMPOSED_BASE].r,
                   ret_nvm_data.type[COMPOSED_BASE].g,
                   ret_nvm_data.type[COMPOSED_BASE].b);

    }
	ds.cComposer.color_theme = color_scheme;
    ds.cComposer.annunColor = color;
    ds.cComposer.focusbarColor = color;
    ds.cComposer.softkeyColor = color;
#if ((PROJECT_ID == ODIN_PROJECT)||(PROJECT_ID == ODIN_A_PROJECT))
    ds.cComposer.titleFontColor  = COLOR_FRENCH_BLUE;
    ds.cComposer.menuFontColor = COLOR_BLACK;
    ds.cComposer.focusbarFontColor = COLOR_WHITE;
    if(COLOR_THEME_USER_DEFINE == color_scheme)
    {
        ds.cComposer.focusbarFontColor = ~ds.cComposer.focusbarColor;
    }
    if(color_scheme == COLOR_THEME_1)
    {
        ds.cComposer.focusbarColor = FOCUS_COLOR_1;
    }
    if(color_scheme == COLOR_THEME_2)
    {
        ds.cComposer.focusbarColor = FOCUS_COLOR_2;
    }
    if(color_scheme == COLOR_THEME_3)
    {
        ds.cComposer.focusbarColor = FOCUS_COLOR_3;
    }
#else
    if(color_scheme == COLOR_THEME_2)
    {
        ds.cComposer.titleFontColor  = COLOR_SOFTKEY;
        ds.cComposer.menuFontColor = COLOR_BLACK;
        ds.cComposer.focusbarFontColor = COLOR_WHITE;

    }
    else
    {
        ds.cComposer.titleFontColor  = COLOR_FRENCH_BLUE;  
        ds.cComposer.menuFontColor = COLOR_BLACK;
        ds.cComposer.focusbarFontColor = COLOR_BLACK;
    }
#endif 
    
    ds_clear_title();
    /* Initialize character cursor as space */
    ds.cCursor.blink = OP_FALSE;
    ds.cCursor.blink_status = OP_FALSE; 
    ds.cCursor.line = CURSOR_OFF;
    op_memset(ds.cCursor.cursor, 0x00, sizeof(ds.cCursor.cursor));

    ds.rssi_level    = 0;
    ds.batt_level    = 0xff;
    ds.annun_mask    = ANNUN_ALL_OFF;    
    ds.blink_annun   = OP_FALSE;
    ds.blink_annun_status = OP_FALSE; 

    ds.ani.status   = ANI_INACTIVE;

    ds.rcWin.left    = 0;
    ds.rcWin.top     = 0;
    ds.rcWin.right   = LCD_MAX_X-1;
    ds.rcWin.bottom  = LCD_TEXT_HIGH-1;

    /* initially updated region is full LCD screen */
    ds.rcUpdate     = ds.rcWin;
   

}

/*==================================================================================================
    FUNCTION: ds_init_scrn_attr 

    DESCRIPTION:
       Initialize screen attributes.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_init_scrn_attr 
( 
    void
)
{
    ds.scrnAttr.win.fontAttr            = FONT_DEFAULT;
    ds.scrnAttr.win.bgColor             = COLOR_WHITE;
    ds.scrnAttr.win.fontColor           = COLOR_BLACK;
    ds.scrnAttr.win.backColor           = COLOR_WHITE;
  
    ds.scrnAttr.annun.fontAttr          = FONT_DEFAULT ;
    ds.scrnAttr.annun.bgColor           = COLOR_ANNBG; 
    ds.scrnAttr.annun.fontColor         = COLOR_BLACK;
    ds.scrnAttr.annun.backColor         = COLOR_ANNBG;
    
    ds.scrnAttr.skey.skeyAttr.fontAttr  = FA_SKEY;
    ds.scrnAttr.skey.skeyAttr.bgColor   = COLOR_WHITE;
    ds.scrnAttr.skey.skeyAttr.fontColor = COLOR_BLUE;
    ds.scrnAttr.skey.skeyAttr.backColor = COLOR_WHITE;
    ds.scrnAttr.skey.skeyHLColor        = COLOR_WHITE; /* not used color attribute in Poseidon */
    ds.scrnAttr.skey.skeyShadowColor    = COLOR_WHITE; /* not used color attribute in Poseidon */
  
    ds.scrnAttr.pop.fontAttr            = FA_POPDEFAULT;
    ds.scrnAttr.pop.bgColor             = COLOR_PURL_BLUE; 
    ds.scrnAttr.pop.fontColor           = COLOR_DARK_BLUE;
    ds.scrnAttr.pop.backColor           = COLOR_WHITE; /* not used color attribute in Poseidon */
  
    ds.scrnAttr.title.bgColor           = COLOR_WHITE; /* not used color attribute in Poseidon */
    ds.scrnAttr.title.fgColor           = COLOR_BLACK;
    
    ds.scrnAttr.cursor.bgColor          = COLOR_FILLBG;
    ds.scrnAttr.cursor.fgColor          = COLOR_BLACK;

}


/**************************************************************************************************
    Refresh Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_set_update_region 

    DESCRIPTION:
        Set user specified region as updated region.  
        ds_refresh() refreshes this specified region only.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_set_update_region 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom
)
{
    set_update_region(left, top, right, bottom);
}


/*==================================================================================================
    FUNCTION: ds_refresh_region 

    DESCRIPTION:
        Refresh the specified region immediately. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
==================================================================================================*/
void ds_refresh_region 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom
)
{

    DS_RECT_T    rect;

    rect.left    = left;
    rect.top     = top;
    rect.right   = right;
    rect.bottom  = bottom;
  
    /*  user reuqest specific region is refreshed. If this area has ever been redrawn by 
        any drawing primitive (kept by ds.update_area), the requested region is refresed. */
    refresh_rectangle(rect);
}


/*==================================================================================================
    FUNCTION: ds_refresh 

    DESCRIPTION:
       Refresh according to the screen mode.
       Called periodically or by application.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_refresh
( 
    void 
)
{
    OP_BOOLEAN    refresh_scrn = OP_TRUE;

    /* when cover close, donot refresh main LCD, but in power on status
       we should refresh */
    if((hmi_get_flip_status() == FLIP_CLOSED)
     &&(SP_get_power_on_status() != POWER_ON_STATUS_INIT))
    {
        return;
    }
    op_debug(DEBUG_HIGH, "ds_refresh()\n");
    
    if ( ds_is_popup() == OP_FALSE )
    {
        switch (ds.scrnMode) 
        {
            case SCREEN_IDLE:
                 break;
            case SCREEN_OWNERDRAW:
                 if (ds.refresh_func) 
                 {
                     refresh_scrn = (ds.refresh_func)( (OP_UINT32) OP_NULL );
                 }
                 break;
            case SCREEN_MENU:
            case SCREEN_LIST:
            case SCREEN_FUNC:
            case SCREEN_FUNC_BG:
                /* draw lines and items and refresh */
                refresh_scrn = ds_refresh_lines_items(); 
                break;
            case SCREEN_MAINMENU:
            case SCREEN_GENERAL:
            case SCREEN_GENERAL_BG:
            case SCREEN_PLAIN_TEXT:
            case SCREEN_FULL_IMAGE:
            default : /* SCREEN_NONE */
                break;  
        }
    }
 
    if ( refresh_scrn || UPDATEAREA())
    {
        refresh_to_lcd();
    }

}


/*==================================================================================================
    FUNCTION: ds_set_refresh_func

    DESCRIPTION:
        Register own drawing and refreshment function.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_set_refresh_func
(
    OP_BOOLEAN (*refresh_func)(OP_UINT32) 
)
{
    ds.refresh_func = refresh_func;
}


/*==================================================================================================
    FUNCTION: ds_clear_refresh_func

    DESCRIPTION:
        Register own drawing and refreshment function.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_clear_refresh_func
( 
    void 
)
{
    ds.refresh_func = OP_NULL;
}

/*==================================================================================================
    FUNCTION: ds_set_disp_annun 

    DESCRIPTION:
        Indicate the annuciator area is not displayed. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        txt_screen buffer is dereferenced accordingly. 
 ==================================================================================================*/
void ds_set_disp_annun
(
    void
)
{

   ds.disp_annun = OP_TRUE;

   txt_screen    = ( OP_UINT16(*)[LCD_MAX_X])scrn_buffer.annunTextBuffer.txt_screen ; 
   /* display annuciator */
   clear_annuciator_buffer(); 
   if (ds.annun_blink_mask)
   {
       register_ds_event_handler(OPUS_TIMER_BLINK_ANNUN);
       OPUS_Start_Timer(OPUS_TIMER_BLINK_ANNUN, 1000, 0,  PERIODIC);
       ds.blink_annun = OP_TRUE;
       ds.blink_annun_status = OP_TRUE;
   }   

   ds_draw_all_maksed_annuciators();

}


/*==================================================================================================
    FUNCTION: ds_get_disp_annun 

    DESCRIPTION:
        Returns whether the annuciator shall be displayed or not.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
 ==================================================================================================*/
OP_BOOLEAN ds_get_disp_annun
(
    void
)
{
    return (ds.disp_annun);
}

/*==================================================================================================
    FUNCTION: ds_clear_disp_annun 

    DESCRIPTION:
        Indicate the annuciator area is not displayed. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        txt_screen buffer is dereferenced accordingly. 
 ==================================================================================================*/
void ds_clear_disp_annun
(
    void
)
{
    ds.disp_annun = OP_FALSE;
    ds.max_lines  = DS_SCRN_MAXLINES;
    txt_screen    = ( OP_UINT16(*)[LCD_MAX_X])scrn_buffer.ext_txt_screen ; 
    wmemset((OP_UINT16 *)annun_screen, ds.scrnAttr.win.bgColor, LCD_ANNUN_HIGH * LCD_MAX_X);
    UPDATECLEAR(AREAMASK_ANNUN);

   if (ds.blink_annun)
   {
        /* if battery and msg/MMS icons are set to blink, do not deregister blink timer
           for sub lcd update */
        if ( !(ds.annun_blink_mask & AMASK_BATT) && !(ds.annun_blink_mask & AMASK_MSG) &&
              !(ds.annun_blink_mask & AMASK_MMS))
        {
            deregister_ds_event_handler(OPUS_TIMER_BLINK_ANNUN);
            OPUS_Stop_Timer(OPUS_TIMER_BLINK_ANNUN);
            ds.blink_annun = OP_FALSE;
        }
   }   
}


/**************************************************************************************************
    Drawing Functions
**************************************************************************************************/

/*=================================================================================================
    FUNCTION: ds_get_string_width 

    DESCRIPTION:
        Measure the string widht in pixels with the given attribute.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        Text is unicoded.
==================================================================================================*/
OP_UINT32 ds_get_string_width 
(
    OP_UINT8       *text,
    DS_FONTATTR    fontattr
)
{
    OP_UINT8 font_width;
  
    font_width = (fontattr & FONT_SIZE_SMALL) ? FONTX_SMALL : FONTX_NORMAL;
    
    return ( font_width * UstrCharCount(text));
}

/*=================================================================================================
    FUNCTION: ds_draw_char 

    DESCRIPTION:
        Draw a character.It can be used to draw a cursor. 
        Text is 2-byte unicode.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
==================================================================================================*/
void ds_draw_char 
(
    OP_INT16       x,
    OP_INT16       y,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
)
{
    draw_char(get_text_icon_area(ds.scrnMode), x, y, text, fontattr, fontcolor, backcolor);
}

/*=================================================================================================
    FUNCTION: ds_draw_text 

    DESCRIPTION:
        Draw text in the specified area and position accroding to the given atttibutes. 
        If the text exceeds the end of a line, it wil be auto-wrpped to the next line of the area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw one line text, use ds_draw_text_line().
        Drawing text is done only on text screen.
==================================================================================================*/
void ds_draw_text 
(
    OP_INT16       x,
    OP_INT16       y,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
)
{
    draw_text(get_text_icon_area(ds.scrnMode), x, y, LCD_MAX_X-1, text, fontattr, fontcolor, backcolor);
}


/*=================================================================================================
    FUNCTION: ds_draw_text_rm 

    DESCRIPTION:
        Draw text with the given resource Id in the specified area and position accroding to the 
        given atttibutes. 
        If the text exceeds the end of a line, it wil be auto-wrpped to the next line of the area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw one line text, use ds_draw_text_line().
        Drawing text is done only on text screen.
        Maximum text lengh should not exceed the screen (except softkey area).
==================================================================================================*/
void ds_draw_text_rm 
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    res_id,
    DS_FONTATTR         fontattr,
    DS_COLOR            fontcolor,
    DS_COLOR            backcolor
)
{
    /* resouce string is uni-coded string and English alphabet in the string will be converted 
       into ASCII.*/
    OP_UINT8    text_str[DS_SCRN_MAX_UC_LEN*DS_SCRN_CHIGH+2];     

    if (util_cnvrt_res_into_text(res_id,text_str) != OP_NULL)
    {
        draw_text(get_text_icon_area(ds.scrnMode), x, y, LCD_MAX_X-1, text_str, fontattr, fontcolor, backcolor);
    }
}


/*==================================================================================================
    FUNCTION: ds_draw_text_line 

    DESCRIPTION:
        Draw text in the specified area and position accroding to the given atttibutes. 
        The text is drawn until it reaches the end of a line and the leftover text is not be drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw text longer than the a line from the staring position, use ds_draw_text().
        Drawing text is done only on text screen.
==================================================================================================*/
void ds_draw_text_line 
(
    OP_INT16       x,
    OP_INT16       y,
    OP_INT16       xlimit,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
)
{
    draw_text_line(get_text_icon_area(ds.scrnMode), x, y, xlimit, text, OP_TRUE, fontattr, fontcolor, backcolor);
}


/*==================================================================================================
    FUNCTION: ds_draw_text_line_rm 

    DESCRIPTION:
        Draw text with the given resource Id in the specified area and position accroding to the given
        atttibutes. 
        The text is drawn until it reaches the end of a line and the leftover text is not be drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw text longer than the a line from the staring position, use ds_draw_text().
        Drawing text is done only on text screen.
==================================================================================================*/
void ds_draw_text_line_rm 
(
    OP_INT16            x,
    OP_INT16            y,
    OP_INT16            xlimit,
    RM_RESOURCE_ID_T    res_id,
    DS_FONTATTR         fontattr,
    DS_COLOR            fontcolor,
    DS_COLOR            backcolor
)
{
    /* resouce string is uni-coded string and English alphabet in the string will be converted 
       into ASCII.*/
    OP_UINT8 text_str[DS_SCRN_MAX_UC_LEN+1];     

    if (util_cnvrt_res_into_text(res_id,text_str) != OP_NULL)
    {
        draw_text_line(get_text_icon_area(ds.scrnMode), x, y, xlimit, text_str, OP_TRUE, fontattr, fontcolor, backcolor);
    }
}

/*==================================================================================================
    FUNCTION: ds_clear_text 

    DESCRIPTION:
        Clear text buffer.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
void ds_clear_text 
( 
    void 
)
{
    DS_COLOR              text_bg_color = ds.scrnAttr.win.backColor;
    DS_SCRNMODE_ENUM_T    scrn_mode;

    scrn_mode = ds_get_screen_mode();
    
    if (get_drawing_area(scrn_mode) == AREA_BITMAP)
    {
        text_bg_color =    COLOR_TRANS;      
    }

    if (ds.disp_annun)
    {
        wmemset((OP_UINT16 *)txt_screen, text_bg_color, LCD_TEXT_HIGH*LCD_MAX_X );
        set_update_region(0,0, LCD_MAX_X-1, LCD_TEXT_HIGH-1);
    }
    else
    {
        wmemset((OP_UINT16 *)txt_screen, text_bg_color, LCD_EXT_TEXT_HIGH*LCD_MAX_X );
        set_update_region(0,0, LCD_MAX_X-1, LCD_EXT_TEXT_HIGH-1);
    }
    UPDATESET(AREAMASK_TEXT);
}


/*==================================================================================================
    FUNCTION: ds_fill_text 

    DESCRIPTION:
        Fill the specified region of the text buffer with the given value.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
void ds_fill_text 
(
    OP_INT16    startx,
    OP_INT16    starty,
    OP_INT16    endx,
    OP_INT16    endy,
    DS_COLOR    color
)
{  
    fill_rect(AREA_TEXT, startx, starty, endx, endy, color);
}


/*==================================================================================================
    FUNCTION: ds_draw_bitmap_image 

    DESCRIPTION:
        Draw bitmap image to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_bitmap_image 
(
    OP_INT16       x,
    OP_INT16       y,
    RM_BITMAP_T    *bitmap
)
{
    draw_bitmap_image(get_drawing_area(ds.scrnMode), x, y, bitmap);
}

/*==================================================================================================
    FUNCTION: ds_draw_bitmap_image_rm 

    DESCRIPTION:
        Draw bitmap image to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_bitmap_image_rm 
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    bitmap_res_id
)
{
    draw_bitmap_image(get_drawing_area(ds.scrnMode), x, y, (RM_BITMAP_T*)util_get_bitmap_from_res(bitmap_res_id));
}
 

/*==================================================================================================
    FUNCTION: ds_draw_partial_bitmap_image 

    DESCRIPTION:
        Draw bitmap image partially to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_partial_bitmap_image 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_BITMAP_T       *bitmap
)
{
    draw_partial_bitmap_image(get_drawing_area(ds.scrnMode), dsx, dsy, d_width, d_height, ssx, ssy, bitmap);
}

/*==================================================================================================
    FUNCTION: ds_draw_partial_bitmap_image 

    DESCRIPTION:
        Draw bitmap image partially to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_partial_bitmap_image_rm 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_RESOURCE_ID_T   bitmap_res_id
)
{
    draw_partial_bitmap_image(get_drawing_area(ds.scrnMode), dsx, dsy, d_width, d_height, ssx, ssy, 
                                 (RM_BITMAP_T*)util_get_bitmap_from_res(bitmap_res_id));
}

/*==================================================================================================
    FUNCTION: ds_draw_resized_bitmap_image 

    DESCRIPTION:
        Draw bitmap image to be fit into the destination size to the specified area and region. 
        The bitmap image is the internal formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_resized_bitmap_image 
(
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_BITMAP_T       *bitmap
)
{
    draw_resized_bitmap_image(get_drawing_area(ds.scrnMode), dx, dy, d_width, d_height, sx, sy, bitmap);
}

/*==================================================================================================
    FUNCTION: ds_draw_resized_bitmap_image_rm 

    DESCRIPTION:
        Draw bitmap image to be fit into the destination size to the specified area and region. 
        The bitmap image is the internal formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_resized_bitmap_image_rm 
(
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_RESOURCE_ID_T    bitmap_res_id
)
{
    draw_resized_bitmap_image(get_drawing_area(ds.scrnMode), dx, dy, d_width, d_height, sx, sy, 
                                  (RM_BITMAP_T*)util_get_bitmap_from_res(bitmap_res_id));
}

/*==================================================================================================
    FUNCTION: ds_draw_icon 

    DESCRIPTION:
        Draw bitmap icon image to the specified area and region. An icon image specifies a 
        transparent color and this color is not drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_icon 
(
    OP_INT16     x,
    OP_INT16     y,
    RM_ICON_T    *icon
)
{
    draw_icon(get_text_icon_area(ds.scrnMode), x, y, icon);
}


/*==================================================================================================
    FUNCTION: ds_draw_icon_rm 

    DESCRIPTION:
        Draw bitmap icon image to the specified area and region. An icon image specifies a 
        transparent color and this color is not drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_icon_rm 
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    icon_res_id
)
{
    draw_icon(get_text_icon_area(ds.scrnMode), x, y, (RM_ICON_T *)util_get_icon_from_res(icon_res_id));
}

/*==================================================================================================
    FUNCTION: ds_draw_partial_icon 

    DESCRIPTION:
        Draw icon partially to the specified area and region. The icon is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_partial_icon 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_ICON_T         *icon
)
{
    draw_partial_icon(get_text_icon_area(ds.scrnMode), dsx, dsy, d_width, d_height, ssx, ssy, icon);
}

/*==================================================================================================
    FUNCTION: ds_draw_partial_icon_rm 

    DESCRIPTION:
        Draw icon partially to the specified area and region. The icon is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_partial_icon_rm 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_RESOURCE_ID_T   icon_res_id
)
{
    draw_partial_icon(get_text_icon_area(ds.scrnMode), dsx, dsy, d_width, d_height, ssx, ssy, 
                         (RM_ICON_T*)util_get_icon_from_res(icon_res_id));
}

/*==================================================================================================
    FUNCTION: ds_draw_resized_icon 

    DESCRIPTION:
        Draw icon to be fit into the destination size to the specified area and region. 
        The bitmap image is the internal formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_resized_icon 
(
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_ICON_T       *icon
)
{
    draw_resized_icon(get_text_icon_area(ds.scrnMode), dx, dy, d_width, d_height, sx, sy, icon);
}

/*==================================================================================================
    FUNCTION: ds_draw_resized_icon_rm 

    DESCRIPTION:
        Draw icon to be fit into the destination size to the specified area and region. 
        The bitmap image is the internal formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers
    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_resized_icon_rm 
(
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_RESOURCE_ID_T    icon_res_id
)
{
    draw_resized_icon(get_text_icon_area(ds.scrnMode), dx, dy, d_width, d_height, sx, sy, 
                                  (RM_ICON_T*)util_get_icon_from_res(icon_res_id));
}

/*==================================================================================================
    FUNCTION: ds_clear_image 

    DESCRIPTION:
        Clear background.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is maly called to clear bg image. Clear it then draw window frame according to the 
        screen mode.
==================================================================================================*/
void ds_clear_image 
( 
    void 
)
{   
    OP_INT16 ylimit = LCD_EXT_TEXT_HIGH-1;
 
    switch (ds.scrnMode) 
    {
        case SCREEN_NONE:
            wmemset((OP_UINT16 *)bitmap_buffer, ds.scrnAttr.win.bgColor, sizeof(bitmap_buffer)/sizeof(OP_UINT16));
            break;
        case SCREEN_IDLE:
#ifdef NOT_YET_DECIDED
            ds.idle.updateBG = OP_TRUE;
#endif
            break;
        case SCREEN_MENU:
        case SCREEN_LIST:
        case SCREEN_FUNC:
        case SCREEN_PLAIN_TEXT:
            ds_draw_text_win_frame();
            break;
        case SCREEN_GENERAL:
        case SCREEN_MAINMENU:
            ds_draw_bg_win_frame();
            ds.bg_updated = OP_TRUE;
            break;
        case SCREEN_OPENWINDOW:
            wmemset((OP_UINT16 *)open_window_buffer, ds.scrnAttr.win.bgColor, sizeof(open_window_buffer)/sizeof(OP_UINT16));
            break;
        default:             
            wmemset((OP_UINT16 *)bitmap_buffer, ds.scrnAttr.win.bgColor, sizeof(bitmap_buffer)/sizeof(OP_UINT16));
            break;
    }
               
    if (ds_get_disp_annun())
    {
        ylimit = LCD_TEXT_HIGH-1;
    }
 
    set_update_region(0,0, LCD_MAX_X-1, ylimit);
    UPDATESET(AREAMASK_BITMAP);
}


/*==================================================================================================
    FUNCTION: ds_get_pixel 

    DESCRIPTION:
        Return the color of the specified point of the given area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
DS_COLOR ds_get_pixel
(
    OP_INT16    pos_x,
    OP_INT16    pos_y
)
{
    return get_pixel(get_drawing_area(ds.scrnMode), pos_x, pos_y);
}


/*==================================================================================================
    FUNCTION: ds_put_pixel 

    DESCRIPTION:
        Draw a pixel on the specified point of the given area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_put_pixel
(
    OP_INT16    pos_x,
    OP_INT16    pos_y,
    DS_COLOR    color
)
{
    put_pixel(get_drawing_area(ds.scrnMode), pos_x, pos_y, color);
}


/*==================================================================================================
    FUNCTION: ds_draw_line 

    DESCRIPTION:
        Draw a line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_line
(
    OP_INT16    startx,
    OP_INT16    starty,
    OP_INT16    endx,
    OP_INT16    endy,
    DS_COLOR    color
)
{
    draw_line(get_drawing_area(ds.scrnMode), startx, starty, endx, endy, color);
}


/*==================================================================================================
    FUNCTION: ds_draw_rect 

    DESCRIPTION:
        Draw a rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_rect 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom,
    DS_COLOR    color
)
{
    draw_rect(get_drawing_area(ds.scrnMode), left, top, right, bottom, color);
}

/*==================================================================================================
    FUNCTION: ds_fill_rect 

    DESCRIPTION:
        Fill a rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_fill_rect 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom,
    DS_COLOR    color
)
{
    fill_rect(get_drawing_area(ds.scrnMode), left, top, right, bottom, color);
}

/*==================================================================================================
    FUNCTION: ds_draw_button_rect 

    DESCRIPTION:
        Draw a button style rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_button_rect 
(
  OP_INT16    left,
  OP_INT16    top,
  OP_INT16    right,
  OP_INT16    bottom,
  DS_COLOR    left_top,
  DS_COLOR    right_bottom
)
{
    draw_button_rect (get_drawing_area(ds.scrnMode), left, top, right, bottom, left_top, right_bottom);
}

/*==================================================================================================
    FUNCTION: ds_draw_button 

    DESCRIPTION:
       Draw filled a button style rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_button 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom,
    DS_COLOR    top_left_color,
    DS_COLOR    right_bottom_color,
    DS_COLOR    fill_color
)
{
    draw_button (get_drawing_area(ds.scrnMode), left, top, right, bottom, 
                   top_left_color,right_bottom_color,fill_color);
}

/*==================================================================================================
    FUNCTION: ds_draw_title 

    DESCRIPTION:
       Draw title regardless screen mode.
       Do not use this in case of SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG.
       In those case, use ds_set_title.

       The title background image style is normal one.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_title 
(
    OP_UINT8                *title,
    DS_SCRN_TITLE_ENUM_T    title_type     
)
{
    OP_INT16            x;
    OP_INT16            y;
	RM_RESOURCE_ID_T    title_bg_res_id = BMP_SUB_SECONDMENU_TITLE_BG_C1;
    DS_LINE_Y_COOR_T    title_base_y = {TITLE_NORMAL_BASE_TOP, TITLE_NORMAL_BASE_BOTTOM};
    ds_set_title(title,title_type);
    /* do not support TITLE_MSG_DETAIL */
    if ( (title_type  > TITLE_NULL) && (title_type < TITLE_MSG_DETAIL))
    {
        if ( title_type == TITLE_MAIN_SUBMENU )
        {
           /*  title_base_y.top    = TITLE_SUBMM_BASE_TOP;
             title_base_y.bottom = TITLE_SUBMM_BASE_BOTTOM;   
             title_bg_res_id      = BMP_SUB_MAINMENU_TITLE_BG_C1;*/
        }
        
        draw_title(title, title_bg_res_id);
    
        /* draw side arrows */
        if ( title_type == TITLE_ARROW)
        {
            x = (OP_INT16)(WIN_LEFT + FONTX_NORMAL);
            x = 1;
    
            /* Center alignment */
            y = (OP_INT16)(GET_CENTER(title_base_y.top, title_base_y.bottom, ds_get_image_height(ICON_TITLE_ARROW_LEFT)));
    
            draw_icon (AREA_TEXT, 
                        x, 
                        y,
                        util_get_icon_from_res(ICON_TITLE_ARROW_LEFT));
    
            x = (OP_INT16)(LCD_MAX_X_COOR -ds_get_image_width(ICON_TITLE_ARROW_RIGHT));
            /* Center alignment */
            y = (OP_INT16)(GET_CENTER(title_base_y.top, title_base_y.bottom, ds_get_image_height(ICON_TITLE_ARROW_RIGHT)));
            draw_icon (AREA_TEXT, 
                        x, 
                        y,
                        util_get_icon_from_res(ICON_TITLE_ARROW_RIGHT));
        }
    }
}

/*==================================================================================================
    FUNCTION: ds_draw_title_rm 

    DESCRIPTION:
       Draw title regardless screen mode.
       Do not use this in case of SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG.
       In those case, use ds_set_title.
      
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_title_rm 
(
    RM_RESOURCE_ID_T        title_res_id,
    DS_SCRN_TITLE_ENUM_T    title_type     
)
{
    OP_UINT8    title_str[DS_SCRN_MAX_LINE_UC_LEN+2]; 
  
    if (util_cnvrt_res_into_text(title_res_id, title_str) != OP_NULL)
    {
        ds_draw_title(title_str, title_type);
    }
}


/*==================================================================================================
    FUNCTION: ds_get_image_size 

    DESCRIPTION:
       Return given image (bitmap or icon) size (witdh and height).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
OP_BOOLEAN ds_get_image_size 
(
    RM_RESOURCE_ID_T    res_id,
    OP_UINT16            *width,
    OP_UINT16            *height
)
{
    OP_BOOLEAN    ret_status  = OP_FALSE;
    RM_ICON_T     *icon_ptr   = OP_NULL;
    RM_BITMAP_T   *bitmap_ptr = OP_NULL;

    if ( RM_IsItIcon(res_id))
    {
        icon_ptr = (RM_ICON_T *)util_get_icon_from_res(res_id);
        if (icon_ptr != OP_NULL)
        {
            *width    = icon_ptr->biWidth;
            *height   = icon_ptr->biHeight;      
            ret_status = OP_TRUE; 
        }
    }
    else if ( RM_IsItBitmap(res_id))
    {
        bitmap_ptr = (RM_BITMAP_T *)util_get_bitmap_from_res(res_id);
        if (bitmap_ptr != OP_NULL)
        {
            *width    = bitmap_ptr->biWidth;
            *height   = bitmap_ptr->biHeight;      
            ret_status = OP_TRUE; 
        }
    }
    return ret_status;
}


/*==================================================================================================
    FUNCTION: ds_get_image_width 

    DESCRIPTION:
       Return given image (bitmap or icon) witdh.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
OP_UINT16 ds_get_image_width 
(
    RM_RESOURCE_ID_T    res_id
)
{    
    OP_UINT16            width = 0;
    RM_ICON_T          *icon_ptr   = OP_NULL;
    RM_BITMAP_T        *bitmap_ptr = OP_NULL;
    RM_INT_ANIDATA_T   *ani_ptr     = OP_NULL;

    if ( RM_IsItIcon(res_id))
    {
        icon_ptr = (RM_ICON_T *)util_get_icon_from_res(res_id);
        if (icon_ptr != OP_NULL)
        {
            width    = icon_ptr->biWidth;
        }
    }
    else if ( RM_IsItBitmap(res_id))
    {
        bitmap_ptr = (RM_BITMAP_T *)util_get_bitmap_from_res(res_id);
        if (bitmap_ptr != OP_NULL)
        {
            width    = bitmap_ptr->biWidth;
        }
    }
    else if ( RM_IsItAnimation(res_id))
    {
        ani_ptr = (RM_INT_ANIDATA_T *)util_get_animation_from_res(res_id);
        if (ani_ptr != OP_NULL)
        {
            if(ani_ptr->image_type == ANI_ICON)
            {
                icon_ptr = (RM_ICON_T *)ani_ptr->scene;
                width    = icon_ptr->biWidth;
            }
            else
            {
                bitmap_ptr = (RM_BITMAP_T *)ani_ptr->scene;
                width    = bitmap_ptr->biWidth;
            }
        }
    }
    return (width);
}


/*==================================================================================================
    FUNCTION: ds_get_image_height 

    DESCRIPTION:
       Return given image (bitmap or icon) height.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
OP_UINT16 ds_get_image_height 
(
    RM_RESOURCE_ID_T    res_id
)
{
    OP_UINT16           height;
    RM_ICON_T          *icon_ptr   = OP_NULL;
    RM_BITMAP_T        *bitmap_ptr = OP_NULL;
    RM_INT_ANIDATA_T   *ani_ptr     = OP_NULL;

    if ( RM_IsItIcon(res_id))
    {
        icon_ptr = (RM_ICON_T *)util_get_icon_from_res(res_id);
        if (icon_ptr != OP_NULL)
        {
            height    = icon_ptr->biHeight;
        }
    }
    else if ( RM_IsItBitmap(res_id))
    {
        bitmap_ptr = (RM_BITMAP_T *)util_get_bitmap_from_res(res_id);
        if (bitmap_ptr != OP_NULL)
        {
            height    = bitmap_ptr->biHeight;
        }
    }
    else if ( RM_IsItAnimation(res_id))
    {
        ani_ptr = (RM_INT_ANIDATA_T *)util_get_animation_from_res(res_id);
        if (ani_ptr != OP_NULL)
        {
            if(ani_ptr->image_type == ANI_ICON)
            {
                icon_ptr = (RM_ICON_T *)ani_ptr->scene;
                height    = icon_ptr->biHeight;
            }
            else
            {
                bitmap_ptr = (RM_BITMAP_T *)ani_ptr->scene;
                height    = bitmap_ptr->biHeight;
            }
        }
    }

    return (height);
}

/**************************************************************************************************
    Animation Control Functions
**************************************************************************************************/
/*==================================================================================================
    FUNCTION: ds_stop_animation 

    DESCRIPTION:
       Stop an active animaton.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_stop_animation 
(
    DS_ANI_T *ani
)
{
    if (!ani || ani->status == ANI_INACTIVE)
    {
      return;
    } 
    
    ani->frame      = 0;
    ani->max_repeat = 0;
    ani->repeat_cnt = 0;
    ani->status     = ANI_INACTIVE;
    ani->data      = OP_NULL;
    ani->bAniStart   = OP_FALSE;

    OPUS_Stop_Timer(OPUS_TIMER_ANIMATION);

}


/*==================================================================================================
    FUNCTION: ds_pause_animation 

    DESCRIPTION:
       Pause an active animiation.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_pause_animation 
(
    DS_ANI_T *ani
)
{
    if (!ani || !(ani->status == ANI_ACTIVE) )
    {
      return;
    } 
    
    ani->status = ANI_PAUSED;
    OPUS_Stop_Timer(OPUS_TIMER_ANIMATION);
}


/*==================================================================================================
    FUNCTION: ds_resume_animation 

    DESCRIPTION:
       Resume a paused animiation.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_resume_animation 
(
    DS_ANI_T *ani
)
{
    if (!ani || !(ani->status == ANI_PAUSED) )
    {
        return;
    } 
    
    ani->status = ANI_ACTIVE;  
    ds_play_animation(ani);
}

/*==================================================================================================
    FUNCTION: ds_start_animaton 

    DESCRIPTION:
       Start an icon animaton.

    ARGUMENTS PASSED:
       repeat_num - number of repeat. To repeat it until it is stopped, set DS_ANI_CONTINUOUSLY. 
                    Maximum repeat numer is 254.
                    If 255 is given, the animation will not be started.
       delay      - time delay in milisecond between frames.

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
 DS_ANI_T * ds_start_animation
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    res_id,
    OP_UINT8            repeat,
    OP_UINT32           delay 
)
{
    RM_INT_ANIDATA_T        *ani_data = OP_NULL;
    DS_ANI_T                *ani       = OP_NULL;
    RM_ICON_T               *ani_icon = OP_NULL;
        
    if (ds.ani.status != ANI_INACTIVE)
    {
        ds_stop_animation(&ds.ani);
    }

    if ((ani_data = util_get_animation_from_res(res_id)) != OP_NULL)
        
    {
        ds.ani.frame      = 0;
        ds.ani.max_repeat = repeat;
        ds.ani.repeat_cnt = 0;
        ds.ani.dx          = x;
        ds.ani.dy          = y;

         /* non-meaningful info for ANI_NORMAL type */
        ds.ani.dw          = 0;
        ds.ani.dh          = 0;
        ds.ani.sx          = 0;
        ds.ani.sy          = 0;

        ds.ani.type       = ANI_NORMAL;
        ds.ani.delay      = delay;
        ds.ani.status     = ANI_ACTIVE;
        ds.ani.data       = ani_data;
        ds.ani.area       = get_drawing_area(ds.scrnMode);
        /* backup the animation icon bg into animal_screen */
        if(ani_data->image_type == ANI_ICON)
        {
            ani_icon = (RM_ICON_T *)ani_data->scene;
            save_animation_icon_bg(ds.ani.area, x, y,
                                   (OP_INT16)(x+ani_icon->biWidth), 
                                   (OP_INT16)(y+ani_icon->biHeight)); 
            ds.ani.bAniStart = OP_TRUE;
        }

        if (ds_play_animation(&ds.ani) == ANI_ACTIVE)
        {
            ani = &ds.ani;
        }
      
    }

    return ani;   
}

/*==================================================================================================
    FUNCTION: ds_start_resized_animaton 

    DESCRIPTION:
       Start an icon animation fit to dest screen.

    ARGUMENTS PASSED:
       repeat_num - number of repeat. To repeat it until it is stopped, set DS_ANI_CONTINUOUSLY. 
                    Maximum repeat numer is 254.
                    If 255 is given, the animation will not be started.
       delay      - time delay in milisecond between frames.

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
DS_ANI_T * ds_start_resized_animaton
(
    OP_INT16            dx,
    OP_INT16            dy,
    OP_UINT8            d_width,
    OP_UINT8            d_height,
    OP_INT16            sx,
    OP_INT16            sy,
    RM_RESOURCE_ID_T    res_id,
    OP_UINT8            repeat,
    OP_UINT32           delay 
)
{

    RM_INT_ANIDATA_T        *ani_data = OP_NULL;
    DS_ANI_T                *ani       = OP_NULL;

    if (ds.ani.status == ANI_ACTIVE)
    {
        ds_stop_animation(&ds.ani);
    }

    if ((ani_data = util_get_animation_from_res(res_id)) != OP_NULL)
    {
        ds.ani.frame      = 0;
        ds.ani.max_repeat = repeat;
        ds.ani.repeat_cnt = 0;
        ds.ani.dx          = dx;
        ds.ani.dy          = dy;
        ds.ani.dw          = d_width;
        ds.ani.dh          = d_height;
        ds.ani.sx          = sx;
        ds.ani.sy          = sy;
        ds.ani.type       = ANI_RESIZED;
        ds.ani.delay      = delay;
        ds.ani.area       = get_drawing_area(ds.scrnMode);
        ds.ani.status     = ANI_ACTIVE;
        ds.ani.data       = ani_data;
        /* backup the animation icon bg into animal_screen */
        if(ani_data->image_type == ANI_ICON)
        {
            save_animation_icon_bg(ds.ani.area, dx, dy,
                                   (OP_INT16)(dx+d_width), 
                                   (OP_INT16)(dy+d_height)); 
            ds.ani.bAniStart = OP_TRUE;
        }
        if (ds_play_animation(&ds.ani) == ANI_ACTIVE)
        {
            ani = &ds.ani;
        }
    }

    return ani;   
}

/*==================================================================================================
    FUNCTION: ds_play_animation 

    DESCRIPTION:
       Draw next frame of the active animaiton. 

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
 DS_ANI_STATUS_ENUM_T ds_play_animation 
(
    DS_ANI_T *ani
)
{
    DS_ANI_STATUS_ENUM_T ani_status = ANI_INACTIVE;
    OP_UINT16    x_offset;
    OP_UINT16    y_offset;
    
    if (ani != OP_NULL)
    {
        ani_status = ani->status;
        if (ani->status == ANI_ACTIVE)
        {
            /* set the animation background*/
            if(ani->type == ANI_NORMAL)
            {
                if(ani->data->image_type == ANI_ICON)
                {
                    x_offset = ((RM_ICON_T *)(ani->data->scene))->biWidth;
                    y_offset = ((RM_ICON_T *)(ani->data->scene))->biHeight;
                }else
                {
                    x_offset = ((RM_BITMAP_T *)(ani->data->scene))->biWidth;
                    y_offset = ((RM_BITMAP_T *)(ani->data->scene))->biHeight;
                }
            }
            else /* resized bg*/
            {
                x_offset = ani->dw;
                y_offset = ani->dh;
            }
                    /* move to the next frame */
            if ((ani->frame) < (ani->data->frames))
            {
                /* redraw animation icon background*/
                if( ani->data->image_type == ANI_ICON )
                {
                    /*if it's the first, needn't set */
                    if(!ani->bAniStart)
                    {
                        set_animation_icon_bg(ani->area, ani->dx, ani->dy,
                            (OP_INT16)(ani->dx + x_offset) ,
                            (OP_INT16)(ani->dy + y_offset));
                    }
                    else
                    {
                        ani->bAniStart = OP_FALSE;
                    }
                }
                draw_animation_image(ani);
                ani->frame ++;
        
                OPUS_Start_Timer(OPUS_TIMER_ANIMATION, ani->delay, 0,  ONE_SHOT);
                if ( (ani->area == AREA_BITMAP) && (ds.scrnMode != SCREEN_FULL_IMAGE)&& (ds.scrnMode != SCREEN_MAINMENU))
                {        
                    UPDATESET(AREA_TEXT);
                }
                if(hmi_get_flip_status() == FLIP_OPENED)
                {
                    refresh_to_lcd();
                }
            }
            else if (ani->repeat_cnt < ani->max_repeat)
            {
                if( ani->data->image_type == ANI_ICON )
                {
                    set_animation_icon_bg(ani->area, ani->dx, ani->dy,
                        (OP_INT16)(ani->dx + x_offset), (OP_INT16)(ani->dy + y_offset));
                }
                if (ani->max_repeat != DS_ANI_CONTINUOUSLY)
                {
                    ani->repeat_cnt++;
                }
                ani->frame = 0;
                draw_animation_image(ani);    
                ani->frame ++;
        
                OPUS_Start_Timer(OPUS_TIMER_ANIMATION, ani->delay, 0,  PERIODIC);
                if ( (ani->area == AREA_BITMAP) && (ds.scrnMode != SCREEN_FULL_IMAGE)&& (ds.scrnMode != SCREEN_MAINMENU))
                {        
                    UPDATESET(AREA_TEXT);
                }
                if(hmi_get_flip_status() == FLIP_OPENED)
                {
                    refresh_to_lcd();
                }
            }
            else
            {
                ds_stop_animation(ani);
                ani_status = ANI_INACTIVE;
            }
        }
    }
    return ani_status;
}

/**************************************************************************************************
    Softkey Control Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_init_softkeys 

    DESCRIPTION:
       Initialize softkeys.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_init_softkeys 
( 
    void
)
{

    ds.skey.style          = SKEYSTYLE_DEFAULT;
   
    ds.skey.left.attr      = SKA_DEFAULT;
    Umemset(ds.skey.left.txt, 0x0000, 1);
    ds.skey.left.icon      = OP_NULL;
    ds.skey.left.fgcolor   = ds.scrnAttr.skey.skeyAttr.fontColor;
    ds.skey.left.bgcolor   = ds.scrnAttr.skey.skeyAttr.bgColor;
    ds.skey.left.update    = OP_TRUE;
    
    ds.skey.center.attr    = SKA_DEFAULT;
    Umemset(ds.skey.center.txt, 0x0000, 1);
    ds.skey.center.icon    = OP_NULL;
    ds.skey.center.fgcolor = ds.scrnAttr.skey.skeyAttr.fontColor;
    ds.skey.center.bgcolor = ds.scrnAttr.skey.skeyAttr.bgColor;
    ds.skey.center.update  = OP_TRUE;
    
    ds.skey.right.attr     = SKA_DEFAULT;
    Umemset(ds.skey.right.txt, 0x0000, 1);
    ds.skey.right.icon     = OP_NULL;
    ds.skey.right.fgcolor  = ds.scrnAttr.skey.skeyAttr.fontColor;
    ds.skey.right.bgcolor  = ds.scrnAttr.skey.skeyAttr.bgColor;
    ds.skey.right.update   = OP_TRUE;
}


/*==================================================================================================
    FUNCTION: ds_set_softkeys_rm 

    DESCRIPTION:
       Set predefined softkeys.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_softkeys_rm 
(
    RM_RESOURCE_ID_T    left_res_id,
    RM_RESOURCE_ID_T    center_res_id,
    RM_RESOURCE_ID_T    right_res_id
)
{
    OP_BOOLEAN check = OP_FALSE;

    OP_UINT8 tmp_skey_str[SKEY_MAX_CHAR*2+2];

    if ( IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(left_res_id,tmp_skey_str, SKEY_MAX_CHAR*2 )))
    {   
        check               = OP_TRUE;
        UstrNullCpy(ds.skey.left.txt, tmp_skey_str,(sizeof(ds.skey.left.txt)-2));
        ds.skey.left.attr  &= ~SKA_NOALIGN;
        ds.skey.left.length = MIN(SKEY_MAX_CHAR, UstrCharCount(ds.skey.left.txt));
        ds.skey.left.icon = OP_NULL;
        ds.skey.left.update = OP_TRUE;
    }
    else
    {
        if (ds.skey.left.txt != OP_NULL)
        {
            check = OP_TRUE;
            Umemset(ds.skey.left.txt, 0x0000, 1);
            ds.skey.left.attr   &= ~SKA_NOALIGN;
            ds.skey.left.length  = 0;
            ds.skey.left.icon = OP_NULL;
            ds.skey.left.update  = OP_TRUE;
        }
    }

  
    if ( IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(center_res_id,tmp_skey_str, SKEY_MAX_CHAR*2 )))
    {
        check                 = OP_TRUE;   
        ds.skey.center.icon   = OP_NULL;
        UstrNullCpy(ds.skey.center.txt, tmp_skey_str,(sizeof(ds.skey.center.txt)-2));
        ds.skey.center.attr   &= ~SKA_NOALIGN;
        ds.skey.center.length = MIN(SKEY_MAX_CHAR, UstrCharCount(ds.skey.center.txt));
        ds.skey.center.icon = OP_NULL;
        ds.skey.center.update = OP_TRUE;
    }
    else
    {
        if (ds.skey.center.txt != OP_NULL)
        {
            check = OP_TRUE;
            Umemset(ds.skey.center.txt, 0x0000, 1);
            ds.skey.center.attr   &= ~SKA_NOALIGN;
            ds.skey.center.length  = 0;
            ds.skey.center.icon = OP_NULL;
            ds.skey.center.update  = OP_TRUE;
        }
    }
    
   if ( IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(right_res_id,tmp_skey_str, SKEY_MAX_CHAR*2 )))
    {
        check                = OP_TRUE;
        UstrNullCpy(ds.skey.right.txt, tmp_skey_str,(sizeof(ds.skey.right.txt)-2));
        ds.skey.right.attr   &= ~SKA_NOALIGN;
        ds.skey.right.length = MIN(SKEY_MAX_CHAR, UstrCharCount(ds.skey.right.txt));
        ds.skey.right.icon = OP_NULL;
        ds.skey.right.update = OP_TRUE;
    }
    else
    {
        if (ds.skey.right.txt != OP_NULL)
        {
            check = OP_TRUE;
            Umemset(ds.skey.right.txt, 0x0000, 1);
            ds.skey.right.attr   &= ~SKA_NOALIGN;
            ds.skey.right.length  = 0;
            ds.skey.right.icon = OP_NULL;
            ds.skey.right.update  = OP_TRUE;
        }
    }
    
    if (ds.skey.style & SKEYSTYLE_DXCAPTION) /* Derect Caption */
    {
        ds.skey.style         &= ~SKEYSTYLE_DXCAPTION;
        check = OP_TRUE;
        ds.skey.left.update   = OP_TRUE;
        ds.skey.center.update = OP_TRUE;
        ds.skey.right.update  = OP_TRUE;
    }

    if (check)
    {
        draw_softkeys(&ds.skey, ds.scrnAttr.skey);
    }
}

/*==================================================================================================
    FUNCTION: ds_set_softkeys_char 

    DESCRIPTION:
       Set the given characters as softkey string.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Upto 6 English and 3 Chiness characters are allowed per a softkey.
==================================================================================================*/
void ds_set_softkeys_char 
(
    OP_UINT8    *left,
    OP_UINT8    *center,
    OP_UINT8    *right
)
{
    OP_BOOLEAN check        = OP_FALSE;

    ds.skey.left.icon     = NULL_ICON;
    ds.skey.center.icon   = NULL_ICON;
    ds.skey.right.icon    = NULL_ICON;

    if (left != OP_NULL)
    {
        /*if (Ustrcmp(left, ds.skey.left.txt)) */
        {
            check = OP_TRUE;
            UstrNullCpy(ds.skey.left.txt, left,(sizeof(ds.skey.left.txt)-2) );
            ds.skey.left.attr   &= ~SKA_NOALIGN;
            ds.skey.left.length  = MIN(SKEY_MAX_CHAR, UstrCharCount(left));
            ds.skey.left.update  = OP_TRUE;
        }
    }
    else
    {
        /*if (ds.skey.left.txt != OP_NULL)*/
        {
            check = OP_TRUE;
            Umemset(ds.skey.left.txt, 0x0000, 1);
            ds.skey.left.attr   &= ~SKA_NOALIGN;
            ds.skey.left.length  = 0;
            ds.skey.left.update  = OP_TRUE;
        }
    }

    if (center != OP_NULL)
    {    
        /*if (Ustrcmp(center, ds.skey.center.txt)) */
        {
            check = OP_TRUE;
            UstrNullCpy(ds.skey.center.txt, center,(sizeof(ds.skey.center.txt)-2));
            ds.skey.center.attr  &= ~SKA_NOALIGN;
            ds.skey.center.length = MIN(SKEY_MAX_CHAR, UstrCharCount(center));
            ds.skey.center.update = OP_TRUE;
        }
    }
    else
    {
        /*if (ds.skey.center.txt != OP_NULL)*/
        {
            check = OP_TRUE;
            Umemset(ds.skey.center.txt, 0x0000, 1);
            ds.skey.center.attr   &= ~SKA_NOALIGN;
            ds.skey.center.length  = 0;
            ds.skey.center.update  = OP_TRUE;
        }
    }
    
    if (right != OP_NULL)
    {
        /*if (Ustrcmp(right, ds.skey.right.txt)) */
        {
            check = OP_TRUE;
            UstrNullCpy(ds.skey.right.txt, right,(sizeof(ds.skey.right.txt)-2) );
            ds.skey.right.attr  &= ~SKA_NOALIGN;
            ds.skey.right.length = MIN(SKEY_MAX_CHAR, UstrCharCount(right));
            ds.skey.right.update = OP_TRUE;
        }
    }
    else
    {
        /*if (ds.skey.right.txt != OP_NULL)*/
        {
            check = OP_TRUE;
            Umemset(ds.skey.right.txt, 0x0000, 1);
            ds.skey.right.attr   &= ~SKA_NOALIGN;
            ds.skey.right.length  = 0;
            ds.skey.right.update  = OP_TRUE;
        }
    }
   
    
    /* If previously set as Direct caption skey, flag the flag */
    if (ds.skey.style & SKEYSTYLE_DXCAPTION) 
    {
        ds.skey.style &= ~SKEYSTYLE_DXCAPTION;
        check = OP_TRUE;
        ds.skey.left.update   = OP_TRUE;
        ds.skey.center.update = OP_TRUE;
        ds.skey.right.update  = OP_TRUE;
    }
    
    if (check)
    {
        draw_softkeys(&ds.skey, ds.scrnAttr.skey);
    }
}


/*==================================================================================================
    FUNCTION: ds_set_softkeys_icon 

    DESCRIPTION:
       Set the given icons as a softkey icon. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        This icon is aligned according to the skey alignment. This icon is placed in front
        of the the softkey string if string is set through ds_set_softkeys.
==================================================================================================*/
void ds_set_softkeys_icon 
(
    RM_ICON_T    *left,
    RM_ICON_T    *center,
    RM_ICON_T    *right
)
{
    ds.skey.left.icon     = left;
    ds.skey.left.update   = OP_TRUE;
    
    ds.skey.center.icon   = center;
    ds.skey.center.update = OP_TRUE;
    
    ds.skey.right.icon    = right;
    ds.skey.right.update  = OP_TRUE;

    Umemset(ds.skey.left.txt, 0x0000, 1);
    Umemset(ds.skey.center.txt, 0x0000, 1);
    Umemset(ds.skey.right.txt, 0x0000, 1);
    
    /* Clear Direct caption skey flag */
    ds.skey.style &= ~SKEYSTYLE_DXCAPTION;
    draw_softkeys(&ds.skey, ds.scrnAttr.skey);
}


/*==================================================================================================
    FUNCTION: ds_set_softkeys_icon_rm 

    DESCRIPTION:
       Set the given icons as a softkey icon. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        This icon is aligned according to the skey alignment. This icon is placed in front
        of the the softkey string if string is set through ds_set_softkeys.
==================================================================================================*/
void ds_set_softkeys_icon_rm 
(
    RM_RESOURCE_ID_T    left_res_id,
    RM_RESOURCE_ID_T    center_res_id,
    RM_RESOURCE_ID_T    right_res_id
)
{
    ds.skey.left.icon     = (RM_ICON_T*)util_get_icon_from_res(left_res_id);
    ds.skey.left.update   = OP_TRUE;

    ds.skey.center.icon   = (RM_ICON_T*)util_get_icon_from_res(center_res_id);
    ds.skey.center.update = OP_TRUE;

    ds.skey.right.icon    = (RM_ICON_T*)util_get_icon_from_res(right_res_id);
    ds.skey.right.update  = OP_TRUE;
          
    Umemset(ds.skey.left.txt, 0x0000, 1);
    Umemset(ds.skey.center.txt, 0x0000, 1);
    Umemset(ds.skey.right.txt, 0x0000, 1);

    /* Clear Direct caption skey flag */
    ds.skey.style         &= ~SKEYSTYLE_DXCAPTION;
    draw_softkeys(&ds.skey, ds.scrnAttr.skey);
}

void ds_set_softkeys_bg
(
    void
)
{
    ds.skey.left.update   = OP_TRUE;
    ds.skey.center.update = OP_TRUE;
    ds.skey.right.update  = OP_TRUE;
    draw_softkeys(&ds.skey, ds.scrnAttr.skey);
}
/*==================================================================================================
    FUNCTION: ds_set_softkey_caption 

    DESCRIPTION:
       Set the softkey canption. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_softkey_caption 
(
    OP_UINT8    *caption
)
{
    draw_softkey_caption ( caption );
    ds.skey.style |= SKEYSTYLE_DXCAPTION;  /* Set Direct Caption flag */
}


/*==================================================================================================
    FUNCTION: ds_set_softkey_caption_rm 

    DESCRIPTION:
       Set the softkey canption. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_softkey_caption_rm 
(
    RM_RESOURCE_ID_T    caption_res_id
)
{
    OP_UINT8    caption_skey_str[SKEY_MAX_CAP_CHAR+2]; 
    op_memset(caption_skey_str, 0x00, SKEY_MAX_CAP_CHAR+2);

    if (util_cnvrt_res_into_text(caption_res_id,caption_skey_str ) != OP_NULL)
    {
        draw_softkey_caption ( caption_skey_str );
        ds.skey.style |= SKEYSTYLE_DXCAPTION;  /* Set Direct Caption flag */
    }
}


/*==================================================================================================
    FUNCTION: ds_draw_any_softkey 

    DESCRIPTION:
        Draw a sofkkey according to the specified attribute. No pre-defined rule is assumed.

    ARGUMENTS PASSED:
        key - which softkey
        attr - softkey attribute
        x - x position to display text
        *text - text string
        length - text length: if -1, length is calculated automatically
        *icon - icon data pointer
        fgcolor - foreground(text) color
        bgcolor    - background color

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_any_softkey 
(
    DS_SKEYPOS_ENUM_T    key,    
    DS_SOFTKEYATTR       attr,    
    OP_INT16             x,                
    OP_UINT8             *text,    
    OP_INT8              length,    
    RM_ICON_T            *icon,        
    DS_COLOR             fgcolor,        
    DS_COLOR             bgcolor        
)
{
    switch (key) 
    {
        case SOFTKEY_LEFT: 
            ds.skey.left.attr    = attr;
            //op_strcpy((char *)ds.skey.left.txt,(char *)text);
            //Ustrcpy(ds.skey.left.txt,text);
            UstrNullCpy(ds.skey.left.txt, text,(sizeof(ds.skey.left.txt)-2));
            ds.skey.left.icon    = icon;
            ds.skey.left.x_pos   = x;
            if (length < 0)
            {
                //ds.skey.left.length = MIN(SKEY_MAX_CHAR, op_strlen((char *)text));
                ds.skey.left.length = MIN(SKEY_MAX_CHAR, UstrCharCount(text));
            }
            else
            {
                ds.skey.left.length = length;
            }
            ds.skey.left.fgcolor    = COLOR_MIDNIGHT_BLUE;
            ds.skey.left.bgcolor    = bgcolor;
            ds.skey.left.update     = OP_TRUE;
            break;
        case SOFTKEY_CENTER:
            ds.skey.center.attr  = attr;
            //op_strcpy((char *)ds.skey.center.txt, (char *)text);
            //Ustrcpy(ds.skey.center.txt, text);
            UstrNullCpy(ds.skey.center.txt, text,(sizeof(ds.skey.center.txt)-2) );
            ds.skey.center.icon  = icon;
            ds.skey.center.x_pos = x;
            if (length < 0)
            {
                //ds.skey.center.length = MIN(SKEY_MAX_CHAR, op_strlen((char *)text));
                ds.skey.center.length = MIN(SKEY_MAX_CHAR, UstrCharCount(text));
            }
            else
            {
                ds.skey.center.length = length;
            }
            ds.skey.center.fgcolor    = COLOR_MIDNIGHT_BLUE;
            ds.skey.center.bgcolor    = bgcolor;
            ds.skey.center.update     = OP_TRUE;
            break;
        case SOFTKEY_RIGHT:
            ds.skey.right.attr    = attr;
            //op_strcpy((char *)ds.skey.right.txt, (char *)text);
            //Ustrcpy(ds.skey.right.txt, text);
            UstrNullCpy(ds.skey.right.txt, text,(sizeof(ds.skey.right.txt)-2) );
            ds.skey.right.icon    = icon;
            ds.skey.right.x_pos   = x;
            if (length < 0)
            {
                /*ds.skey.right.length = MIN(SKEY_MAX_CHAR, op_strlen((char *)text));*/
                ds.skey.right.length = MIN(SKEY_MAX_CHAR, UstrCharCount(text));
            }
            else
            {
                ds.skey.right.length = length;
            }
            ds.skey.right.fgcolor    = COLOR_MIDNIGHT_BLUE;
            ds.skey.right.bgcolor    = bgcolor;
            ds.skey.right.update     = OP_TRUE;
            break;
    }
    
    draw_softkeys(&ds.skey, ds.scrnAttr.skey);
}



/*==================================================================================================
    FUNCTION: ds_draw_any_softkey_rm 

    DESCRIPTION:
        Draw a sofkkey according to the specified attribute. No pre-defined rule is assumed.

    ARGUMENTS PASSED:
        key - which softkey
        attr - softkey attribute
        x - x position to display text
        *text - text string
        length - text length: if -1, length is calculated automatically
        *icon - icon data pointer
        fgcolor - foreground(text) color
        bgcolor    - background color

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_any_softkey_rm 
(
    DS_SKEYPOS_ENUM_T    key,    
    DS_SOFTKEYATTR       attr,    
    OP_INT16             x,                
    RM_RESOURCE_ID_T     text_res_id,    
    OP_INT8              length,    
    RM_ICON_T            *icon,        
    DS_COLOR             fgcolor,        
    DS_COLOR             bgcolor        
)
{
    OP_UINT8 skey_str[SKEY_MAX_CHAR*2+2];  

    if ( IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(text_res_id,skey_str, SKEY_MAX_CHAR*2 )))

    {
        ds_draw_any_softkey(key, attr, x, (OP_UINT8 *)skey_str, length, icon, fgcolor, bgcolor);
    }
  
}


/**************************************************************************************************
    Small Drawing Widgets
**************************************************************************************************/

/*==================================================================================================
FUNCTION: ds_set_popup_scrollbar

DESCRIPTION:
Draw window frame to the txt screen for text mode.

ARGUMENTS PASSED:

RETURN VALUE:

IMPORTANT NOTES:
None.
===================================================================================================*/
void ds_set_popup_scrollbar( OP_UINT8    totalItems,    
                            OP_UINT8    firstItem,    
                            OP_UINT8    pageItems,
                            OP_BOOLEAN  bCal)
{
    OP_UINT8    offsetPosition = 0;  
    OP_UINT16   uiHeightScrollBar = 0;

    if(!ds_is_popup() ||
        totalItems <= pageItems)
    {
        return ;
    }
    if(bCal)
    {

        if((totalItems == 0) || (pageItems == 0) ||
            (pageItems > 255) ||(totalItems > 255) ||
            (firstItem > 255))
        {
            return ;
        }
        if(totalItems > pageItems)
        {
            //We will set the scroll bar.
            ds.popwin->bHasScrollBar = OP_TRUE;
            uiHeightScrollBar = POPUP_SCROLLBAR_BOTTOM - POPUP_SCROLLBAR_TOP -  4*POPUP_SCROLLBAR_SPILT_HIGHT ;
            if(firstItem == 0)
            {
                offsetPosition = POPUP_SCROLLBAR_TOP + 7 ;
            }
            else if(firstItem == totalItems -1)
            {
                offsetPosition = POPUP_SCROLLBAR_BOTTOM -  2*POPUP_SCROLLBAR_SPILT_HIGHT;
            }
            else
            {
                offsetPosition = POPUP_SCROLLBAR_TOP + 7 +(uiHeightScrollBar/(totalItems - 2))*firstItem;
            }
            ds.popwin->offsetScrollbar = offsetPosition;
        }
    }
    if (ds_get_disp_annun())
    {
        draw_bitmap_image(AREA_POPUP,
                        POPUP_SCROLLBAR_LEFT,
                        POPUP_SCROLLBAR_TOP - LCD_ANNUN_HIGH,
                        (RM_BITMAP_T*)util_get_bitmap_from_res(BMP_POPUP_SCROLL_BAR));
/*
        fill_rect(AREA_POPUP,
            (OP_UINT8)POPUP_SCROLLBAR_LEFT,
            (OP_UINT8)POPUP_SCROLLBAR_TOP - LCD_ANNUN_HIGH+7,
            (OP_UINT8)POPUP_SCROLLBAR_RIGHT,
            (OP_UINT8)POPUP_SCROLLBAR_BOTTOM - LCD_ANNUN_HIGH -7,
            COLOR_VIOLET_PINK); 
*/
        draw_bitmap_image(AREA_POPUP,
            POPUP_SCROLLBAR_LEFT,
            (OP_INT16)(ds.popwin->offsetScrollbar - LCD_ANNUN_HIGH),
            (RM_BITMAP_T*)util_get_bitmap_from_res(BMP_POPUP_SCROLL_SPIN));
/*

        fill_rect(AREA_POPUP,
            (OP_UINT8)POPUP_SCROLLBAR_LEFT,
            (OP_UINT8)(ds.popwin->offsetScrollbar - LCD_ANNUN_HIGH),
            (OP_UINT8)POPUP_SCROLLBAR_RIGHT,
            (OP_UINT8)(ds.popwin->offsetScrollbar + POPUP_SCROLLBAR_SPILT_HIGHT - LCD_ANNUN_HIGH),
            COLOR_BROWN_GRAY);    */

    }
    else
    {
        
        draw_bitmap_image(AREA_POPUP,
            POPUP_SCROLLBAR_LEFT,
            POPUP_SCROLLBAR_TOP ,
            (RM_BITMAP_T*)util_get_bitmap_from_res(BMP_POPUP_SCROLL_BAR));
        
/*

        fill_rect(AREA_POPUP,
            (OP_UINT8)POPUP_SCROLLBAR_LEFT,
            (OP_UINT8)POPUP_SCROLLBAR_TOP + 7,
            (OP_UINT8)POPUP_SCROLLBAR_RIGHT,
            (OP_UINT8)POPUP_SCROLLBAR_BOTTOM -7,
            COLOR_VIOLET_PINK);  */
        draw_bitmap_image(AREA_POPUP,
            POPUP_SCROLLBAR_LEFT,
            ds.popwin->offsetScrollbar,
            (RM_BITMAP_T*)util_get_bitmap_from_res(BMP_POPUP_SCROLL_SPIN));

   /*
        fill_rect(AREA_POPUP,
            (OP_UINT8)POPUP_SCROLLBAR_LEFT,
            (OP_UINT8)ds.popwin->offsetScrollbar ,
            (OP_UINT8)POPUP_SCROLLBAR_RIGHT,
            (OP_UINT8)(ds.popwin->offsetScrollbar + POPUP_SCROLLBAR_SPILT_HIGHT),
               COLOR_BROWN_GRAY);    */
   
    }
}


/*==================================================================================================
    FUNCTION: ds_draw_text_win_frame 

    DESCRIPTION:
        Draw window frame to the txt screen for text mode.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_text_win_frame 
( 
    void
)
{
    if (ds_get_disp_annun())
    {
        fill_rect(AREA_TEXT,0,0,LCD_MAX_X_COOR, LCD_TEXT_HIGH-1, COLOR_WINBG);    
    }
    else
    {
        fill_rect(AREA_TEXT,0,0,LCD_MAX_X_COOR, LCD_EXT_TEXT_HIGH-1, COLOR_WINBG);    
    }
}

/*==================================================================================================
    FUNCTION: ds_draw_bg_win_frame 

    DESCRIPTION:
        Draw window frame to the bitmap screen for bitmap mode.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_bg_win_frame 
( 
    void
)
{
    fill_rect(AREA_BITMAP, 0, 0, LCD_MAX_X-1, LCD_EXT_TEXT_HIGH-1, COLOR_WINBG);    
}


/*==================================================================================================
    FUNCTION: ds_draw_popup_win_frame 

    DESCRIPTION:
        Draw popup window frame to the popup screen for popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_draw_popup_win_frame 
( 
    DS_RECT_T    rect
)
{
   /* draw popup window frame */
}


/**************************************************************************************************
    Annuciator Control Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_annuciator 

    DESCRIPTION:
       Set handset annunciators on or off

    ARGUMENTS PASSED:
       mask - mask of annunciators
       value - annunciator values to mask in

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Currenlty blink annuciator will be implemented later (07/02/02).
==================================================================================================*/
void ds_set_annuciator
(
    DS_ANNUN_ENUM_T    annuciator,                    
    OP_BOOLEAN         on_off,
    OP_BOOLEAN         blink                     
)
{
    OP_BOOLEAN    update    = OP_TRUE;
    ANNUN_MASK    amask;
    
    /* draw lcd announciator */
    switch (annuciator)
    {
       case ANNUN_RSSI0:
       case ANNUN_RSSI1:
       case ANNUN_RSSI2:
       case ANNUN_RSSI3:
       case ANNUN_RSSI4:
       case ANNUN_RSSI5:
       case ANNUN_RSSI6:
            amask = AMASK_RSSI;
            break;       
       case ANNUN_BATT0:
       case ANNUN_BATT1:
       case ANNUN_BATT2:
       case ANNUN_BATT3:
       case ANNUN_BATT4:
            amask = AMASK_BATT;
            break;   
       case ANNUN_MSG:
            amask = AMASK_MSG;
            break;       
       case ANNUN_MMS:
            amask = AMASK_MMS;
            break;       
       case ANNUN_PUSH:
            amask = AMASK_PUSH;
            break;    
       case ANNUN_QQ:
            amask = AMASK_QQ;
            break; 
       case ANNUN_VMAIL:
            amask = AMASK_VMAIL;
            break; 

       case ANNUN_SMS_PHONE_FULL:
            amask = AMASK_SMS_PHONE_FULL;
            ds.annun_mask   &= ~AMASK_SMS_SIM_FULL;
            ds.annun_mask   &= ~AMASK_SMS_ALL_FULL;
            break;
       case ANNUN_SMS_SIM_FULL:
            amask = AMASK_SMS_SIM_FULL;
            ds.annun_mask   &= ~AMASK_SMS_PHONE_FULL;
            ds.annun_mask   &= ~AMASK_SMS_ALL_FULL;
            break;
       case ANNUN_SMS_ALL_FULL:
            amask = AMASK_SMS_ALL_FULL;
            ds.annun_mask   &= ~AMASK_SMS_SIM_FULL;
            ds.annun_mask   &= ~AMASK_SMS_PHONE_FULL;
            break;
       case ANNUN_CONNECTED:
            amask = AMASK_CONNECTED;
            if(on_off == OP_FALSE)
            {
	            
                if(GET_ANN_MASK(AMASK_CSD))
             	{
	             	ds.annun_mask  &= ~AMASK_CONNECTED;
                    annuciator     = ANNUN_CSD;
                    amask          = AMASK_CSD;
                    on_off         = OP_TRUE;  
             	}
                else if( GET_ANN_MASK(AMASK_MISSED_CALL))
                 {
                    ds.annun_mask  &= ~AMASK_CONNECTED;
                    annuciator     = ANNUN_MISSED_CALL;
                    amask          = AMASK_MISSED_CALL;
                    on_off         = OP_TRUE;            
                 }
                 else if(GET_ANN_MASK(AMASK_DIVERT))
                 {
                    ds.annun_mask  &= ~AMASK_CONNECTED;
                    annuciator     = ANNUN_DIVERT;
                    amask          = AMASK_DIVERT;
                    on_off         = OP_TRUE;            
                 }
                
            }

            break;       
       case ANNUN_MISSED_CALL:
            amask = AMASK_MISSED_CALL;

            /* when turns off  if lower priority icon mask is on, draw it */
            /* no service is higer priority than divert */
            if(on_off == OP_TRUE)
            {
                if(GET_ANN_MASK(AMASK_CONNECTED) || GET_ANN_MASK(AMASK_CSD))
                {
                    update = OP_FALSE;
                }
            }
            else
            {
                if(GET_ANN_MASK(AMASK_DIVERT))
                {
                    ds.annun_mask   &= ~AMASK_MISSED_CALL;
                    annuciator     = ANNUN_DIVERT;
                    amask           = AMASK_DIVERT;
                    on_off         = OP_TRUE;            
                }
            }
            break;       
       case ANNUN_DIVERT:
            amask = AMASK_DIVERT;
            if  ( (ds.annun_mask & AMASK_CONNECTED) || (ds.annun_mask & AMASK_MISSED_CALL) || (ds.annun_mask & AMASK_CSD))
            {
                update = OP_FALSE;
            } 
            break;    
       case ANNUN_CSD:
       	    amask = AMASK_CSD;
       	    if(on_off == OP_FALSE)
   	    	{
	       	    if(GET_ANN_MASK(AMASK_CONNECTED))
            	{
	            	ds.annun_mask   &= ~AMASK_CSD;
                    annuciator     = ANNUN_CONNECTED;
                    amask           = AMASK_CONNECTED;
                    on_off         = OP_TRUE;   
	            	 
            	}
	            else if(GET_ANN_MASK(AMASK_MISSED_CALL))
            	{
		            ds.annun_mask   &= ~AMASK_CSD;
                    annuciator     = ANNUN_MISSED_CALL;
                    amask           = AMASK_MISSED_CALL;
                    on_off         = OP_TRUE;    	    
            	}
                else if(GET_ANN_MASK(AMASK_DIVERT))
                {
                    ds.annun_mask   &= ~AMASK_CSD;
                    annuciator     = ANNUN_DIVERT;
                    amask           = AMASK_DIVERT;
                    on_off         = OP_TRUE;            
                }
   	    	}
       	    break;
   
       /* ANNUN_IB, ANNUN_MELODY, ANNUN_VIB_BELL and ANNUN_MUTE are exclusive each other */
       case ANNUN_VIB:
            amask = AMASK_VIB;
            ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_VIB_BELL;
            ds.annun_mask   &= ~AMASK_SILENCE;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_NORMAL;
            ds.annun_mask   &= ~AMASK_ALOUD;
            ds.annun_mask   &= ~AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_INMOBILE;
            break;       
       case ANNUN_MELODY:
            amask = AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_VIB;
            ds.annun_mask   &= ~AMASK_VIB_BELL;
            ds.annun_mask   &= ~AMASK_SILENCE;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_INMOBILE;
			ds.annun_mask   &= ~AMASK_NORMAL;
            ds.annun_mask   &= ~AMASK_ALOUD;
            break;       
       case ANNUN_VIB_BELL:
            amask = AMASK_VIB_BELL;
            ds.annun_mask   &= ~AMASK_VIB;
			ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_SILENCE;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_INMOBILE;
			ds.annun_mask   &= ~AMASK_NORMAL;
            ds.annun_mask   &= ~AMASK_ALOUD;
            break;       
       case ANNUN_MUTE:
		    amask = AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_VIB;
			ds.annun_mask   &= ~AMASK_SILENCE;
            ds.annun_mask   &= ~AMASK_VIB_BELL;
            ds.annun_mask   &= ~AMASK_NORMAL;
			ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_ALOUD;
            ds.annun_mask   &= ~AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_INMOBILE;
             break;
       case ANNUN_SILENCE:       
            amask = AMASK_SILENCE;
            ds.annun_mask   &= ~AMASK_VIB;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_VIB_BELL;
            ds.annun_mask   &= ~AMASK_NORMAL;
			ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_ALOUD;
            ds.annun_mask   &= ~AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_INMOBILE;
           break; 

       case ANNUN_NORMAL:
            amask = AMASK_NORMAL;
            ds.annun_mask   &= ~AMASK_VIB;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_VIB_BELL;
            ds.annun_mask   &= ~AMASK_SILENCE;
            ds.annun_mask   &= ~AMASK_ALOUD;
			ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_INMOBILE;
            break;  

       case ANNUN_ALOUD:
            amask = AMASK_ALOUD;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_VIB;
            ds.annun_mask   &= ~AMASK_VIB_BELL;
            ds.annun_mask   &= ~AMASK_SILENCE;
            ds.annun_mask   &= ~AMASK_NORMAL;
			ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_INMOBILE;
            break;       
       case ANNUN_EARPHONE:
            amask = AMASK_EARPHONE;
            ds.annun_mask   &= ~AMASK_VIB;
			ds.annun_mask   &= ~AMASK_VIB_BELL;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_SILENCE;
            ds.annun_mask   &= ~AMASK_NORMAL;
			ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_ALOUD;
            ds.annun_mask   &= ~AMASK_INMOBILE;
            break;       
       case ANNUN_INMOBILE:
            amask = AMASK_INMOBILE;
            ds.annun_mask   &= ~AMASK_VIB;
			ds.annun_mask   &= ~AMASK_VIB_BELL;
			ds.annun_mask   &= ~AMASK_MUTE;
            ds.annun_mask   &= ~AMASK_SILENCE;
            ds.annun_mask   &= ~AMASK_NORMAL;
			ds.annun_mask   &= ~AMASK_MELODY;
            ds.annun_mask   &= ~AMASK_ALOUD;
            ds.annun_mask   &= ~AMASK_EARPHONE;
	         break;
            

       case ANNUN_ALARM:
            amask = AMASK_ALARM;
            break;       
       case ANNUN_ROAM:
            amask = AMASK_ROAM;
            if(ds.annun_mask & AMASK_ALARM)
            {
                update = OP_FALSE;
            }
            break;

       case ANNUN_GPRS:
            amask = AMASK_GPRS;
            ds.annun_mask &= ~AMASK_GPRS_INACTIVE;
            break;    

       case ANNUN_GPRS_INACTIVE:
            amask = AMASK_GPRS_INACTIVE;
            ds.annun_mask &= ~AMASK_GPRS;
            break;    
            
       default:
           break;
    }  
   
    /* update annuciator maskbit */
    if ( on_off)
    { 
        ds.annun_mask |= amask;
        if  (blink) 
        {
            ds.annun_blink_mask |= amask;
        }
        else
        {
            ds.annun_blink_mask &= ~amask;
        }
    }
    else
    {
        ds.annun_mask       &= ~amask;
        ds.annun_blink_mask &= ~amask;
        
        if((annuciator == ANNUN_MSG) ||(annuciator == ANNUN_MMS) || (annuciator == ANNUN_PUSH) || 
    	(annuciator == ANNUN_QQ) ||(annuciator == ANNUN_VMAIL) )
		{
		/* do not refresh if one of them remain exist */
			if(ds.annun_mask & (AMASK_MSG | AMASK_MMS | AMASK_PUSH | AMASK_QQ | AMASK_VMAIL))
			{
				update = OP_FALSE;
			}
    }

    }
    

    /* display only when the displaying annuciator is allowed */
    if (ds.annun_blink_mask)
    {
        /* if the blinking timer is not running by any prior annuciator, run it */
        if (ds.blink_annun == OP_FALSE)
        {
            register_ds_event_handler(OPUS_TIMER_BLINK_ANNUN);
            OPUS_Start_Timer(OPUS_TIMER_BLINK_ANNUN, 1000, 0,  PERIODIC);
            ds.blink_annun = OP_TRUE;
            ds.blink_annun_status = OP_TRUE;
        }
    } 
    else if (ds.blink_annun)
    {
        deregister_ds_event_handler(OPUS_TIMER_BLINK_ANNUN);
        OPUS_Stop_Timer(OPUS_TIMER_BLINK_ANNUN);
        ds.blink_annun = OP_FALSE;
        /* refresh annuciator now since the timer is stopped */
        if  (update && ds_get_disp_annun())
        {
            if(hmi_get_flip_status() == FLIP_CLOSED)
            {
                draw_openwindow_annun(annuciator, on_off);
            }
            else
            {
                draw_annunciator(annuciator, on_off);
            }
        }
        refresh_annuciator();
    }   

    if(ds.annun_blink_mask & AMASK_BATT)
       {
            isNeedStopBlinkTimer = OP_FALSE;
       }
  
    if  (update && ds_get_disp_annun())
    {
        if(SP_status_get_openwindow_display()
            && (SUBLCD_DISPLAY_ANIMATION_MODE != SP_get_sublcd_screen_mode()))
        {
            draw_openwindow_annun(annuciator, on_off);
        }
        else
        {
            draw_annunciator(annuciator, on_off);
        }
    }                                                
}

/*==================================================================================================
    FUNCTION: ds_set_rssi 

    DESCRIPTION:
       Set RSSI annunciator bar. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_rssi
(
    OP_UINT8      value,
    OP_BOOLEAN    blink
)
{
    OP_INT8 new_value;
    OP_BOOLEAN blinking;
   
    blinking = ((ds.annun_blink_mask & AMASK_RSSI) > 0 )? OP_TRUE:OP_FALSE;

    new_value = value / 6;
    if ( new_value > 6 )
    {
        new_value = 6;
    }

#if (PROJECT_ID == ODIN_PROJECT)
    if ( value < 6 )
    {
        new_value = 0;
    }
    else if ( value < 16 )
    {
        new_value = 2;
    }
    else if ( value < 26 )
    {
        new_value = 4;
    }
    else
    {
        new_value = 6;
    }
#endif

    if ( (ds.rssi_level != new_value) || (blinking != blink))
    {
        ds_set_annuciator((DS_ANNUN_ENUM_T)(ANNUN_RSSI0+new_value), OP_TRUE, blink);
        ds.rssi_level = new_value;
    }
} 


/*==================================================================================================
    FUNCTION: ds_set_batt 

    DESCRIPTION:
       Set Battery annunciator bar. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_batt
(
    OP_UINT8      value,
    OP_BOOLEAN    blink
)
{
    /* Battery level
    */
    OP_BOOLEAN blinking;
   
    blinking = ((ds.annun_blink_mask & AMASK_BATT) > 0)? OP_TRUE:OP_FALSE;

    if (value < ANNUN_FULL_BATT)
    {
        if ( (ds.batt_level != value) || (blinking != blink) )
        {  
            ds_set_annuciator((DS_ANNUN_ENUM_T)(ANNUN_BATT0+value), OP_TRUE, blink);
            ds.batt_level = value;
        }
    }
}


/**************************************************************************************************
   General Title Control Functions - Title is used in Menu, List, and Text Lines and Graphic Items
                                     in SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_set_title 

    DESCRIPTION:
       Set the title of the current screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_title 
(
    OP_UINT8                *title,
    DS_SCRN_TITLE_ENUM_T    title_type     
)
{
    DS_COLORTHEME_ENUM_T color_theme = ds.cComposer.color_theme;
   
    /* set title */
    ds.title.flag = OP_TRUE;
    ds.title.sel  = OP_FALSE;
    ds.title.attr = LA_DEFAULT;
    ds.title.y.top = 0;
    ds.title.y.bottom = ds.title.y.top + ds_get_image_height(BMP_SUB_SECONDMENU_TITLE_BG_C1) - 1;
    
    /* title donot support user define style */
    if(color_theme == COLOR_THEME_USER_DEFINE)
    {
        color_theme = COLOR_THEME_1;
    }
 
       ds.cComposer.titleFontColor = NORMAL_TITLE_FONT_COLOR;    
      switch(title_type)
    {
        case TITLE_NORMAL:
             ds.title.bitmap = (RM_BITMAP_T *)util_get_bitmap_from_res(BMP_SUB_SECONDMENU_TITLE_BG_C1+color_theme);             
             ds.title.base.top = TITLE_NORMAL_BASE_TOP;
             ds.title.base.bottom = TITLE_NORMAL_BASE_BOTTOM;
             ds.title.it_margine = TITLE_NORMAL_ICON_TEXT_MARGINE;
             ds.title.attr |= LA_SHADOWFONT;
             break;
        case TITLE_ARROW:
             ds.title.bitmap = (RM_BITMAP_T *)util_get_bitmap_from_res(BMP_SUB_SECONDMENU_TITLE_BG_C1+color_theme);  
             ds.title.base.top = TITLE_NORMAL_BASE_TOP;
             ds.title.base.bottom = TITLE_NORMAL_BASE_BOTTOM;
             ds.title.it_margine = TITLE_ARROW_ICON_TEXT_MARGINE;
             ds.title.attr |= LA_SHADOWFONT|LA_SIDEBUTTON;           
            break;
        case TITLE_MAIN_SUBMENU:
		/* remove it for sub menu same as second menu*/
        case TITLE_SECOND_MENU:
            ds.title.bitmap = (RM_BITMAP_T *)util_get_bitmap_from_res(BMP_SUB_SECONDMENU_TITLE_BG_C1+color_theme);  
            ds.title.y.top = 0;
            ds.title.y.bottom = ds.title.y.top + ds_get_image_height(BMP_SUB_SECONDMENU_TITLE_BG_C1)-1;
            ds.title.base.top = ds.title.y.top + 1;
            ds.title.base.bottom = ds.title.y.bottom - 1;
            ds.title.it_margine = TITLE_NORMAL_ICON_TEXT_MARGINE;
            ds.title.attr &= ~LA_SHADOWFONT;
            ds.title.attr |= LA_TITLE_SECOND_SUBMENU;
            ds.max_lines = SECOND_MENU_ITEMS;
            ds_set_title_draw_func(Draw_SecondMenu);
            break;
        case TITLE_MSG_DETAIL:
             ds.title.bitmap = OP_NULL;  
             if (!(ds.title.attr & LA_TITLE_MSG_DETAIL))
             {
                 OP_INT16 y;
                 OP_UINT8 i;

                 ds.title.y.bottom =  (ds.title.y.bottom + LINE_HEIGHT);          

                 /* if the style of title is used, the maximum line is reduced by 1 */
                 y = ds.title.y.bottom + 1;
                 ds.max_lines = DS_SCRN_MAXLINES -1;

                 for  ( i = 0; i < ds.max_lines; i++)
                 {
                     ds.line[i].y.top = y;
                     ds.line[i].base.top = ds.line[i].y.top+LINE_TEXT_OFFSET;
                     y += (OP_INT16)LINE_HEIGHT;
                     ds.line[i].y.bottom = y -1;     
                     ds.line[i].base.bottom = ds.line[i].y.bottom -LINE_TEXT_OFFSET;                   
                 }
             }   
             ds.title.base.top = TITLE_MSGDETAIL_BASE_TOP;
             ds.title.base.bottom = TITLE_MSGDETAIL_BASE_BOTTOM;
             ds.title.it_margine = TITLE_MSGDETAIL_ICON_TEXT_MARGINE;

            /* no shadow */
            ds.title.attr &= ~ LA_SHADOWFONT;
            ds.title.attr |= LA_TITLE_MSG_DETAIL;      
            break;
        case TITLE_OWNERDRAW:
        default :
             ds.title.bitmap = OP_NULL;             
             break;             
    }

    ds.title.icon = OP_NULL;
  /*  ds.title.draw_func = OP_NULL;*/
    
    if (title) 
    {
    /*
       OP_UINT8 leng = Ustrlen( title )+2;
       leng = MIN (leng, DS_SCRN_MAX_LINE_UC_LEN);
       Ustrncpy( ds.title.txt, title, leng );*/
        UstrNullCpy(ds.title.txt, title,(sizeof(ds.title.txt)-2) );
    }
    else 
    {
        Umemset( ds.title.txt, 0x0000, 1);
    }
    
}

/*==================================================================================================
    FUNCTION: ds_set_title_rm 

    DESCRIPTION:
       Set the title of the current screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_title_rm 
(
    RM_RESOURCE_ID_T        title_res_id,
    DS_SCRN_TITLE_ENUM_T    title_type     
)
{
    OP_UINT8    title_str[DS_SCRN_MAX_LINE_UC_LEN+2]; 

   
    if (util_cnvrt_res_into_text(title_res_id, title_str) != OP_NULL)
    {
        ds_set_title(title_str, title_type);
    }
}

/*==================================================================================================
    FUNCTION: ds_set_title_bg 

    DESCRIPTION:
       Set the title background image. It is especially for TITLE_MSG_DETAIL.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_title_bg 
(
    RM_BITMAP_T    *title_bg     
)
{
    ds.title.bitmap = title_bg;                 
}

/*==================================================================================================
    FUNCTION: ds_set_title_bg_rm 

    DESCRIPTION:
       Set the title background image. It is especially for TITLE_MSG_DETAIL.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_title_bg_rm 
(
    RM_RESOURCE_ID_T    title_bg_res_id     
)
{
    ds.title.bitmap = (RM_BITMAP_T *)util_get_bitmap_from_res(title_bg_res_id);                          
}


/*==================================================================================================
    FUNCTION: ds_set_title_draw_func 

    DESCRIPTION:
       Set itle drawing function of Owner drawing type.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_title_draw_func 
(
    OP_UINT8    (*draw_func)(OP_UINT8, OP_UINT32) 
)
{   
    ds.title.flag     = OP_TRUE;
    Umemset(ds.title.txt, 0x0000, 1);
    ds.title.attr     |= LA_OWNERDRAW;
    ds.title.draw_func = draw_func;

}


/*==================================================================================================
    FUNCTION: ds_set_title_icon 

    DESCRIPTION:
       Set the title icon to be placed at the head of the title line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_title_icon 
( 
    RM_ICON_T    *icon 
)
{
    ds.title.flag      = OP_TRUE;
    if (icon) 
    {
        ds.title.attr  |= LA_ICON;
        ds.title.icon  = icon;
    }
    else 
    {
        ds.title.attr  &= ~LA_ICON;
        ds.title.icon  = OP_NULL;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_title_icon_rm 

    DESCRIPTION:
       Set the title icon to be placed at the head of the title line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_title_icon_rm 
( 
    RM_RESOURCE_ID_T    icon_res_id
)
{

    ds.title.flag      = OP_TRUE;
    ds.title.icon      = util_get_icon_from_res(icon_res_id);
    if ( ds.title.icon != OP_NULL)
    {
        ds.title.attr |= LA_ICON;
    }
    else
    {
        ds.title.attr &= ~LA_ICON;
    }

}

/* For SCREEN_OWNERDRAW*/
#if 0
void ui_set_refresh_func( OP_BOOLEAN (*refresh_func)(OP_UINT32) );
{
    /* Code */
    return OP_TRUE;
}
#endif


/*==================================================================================================
    FUNCTION: ds_clear_title 

    DESCRIPTION:
       Clear title data.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_clear_title 
( 
    void
)
{
    ds.title.flag = OP_FALSE;
    ds.title.attr = TITLE_NULL;
    ds.title.icon       = OP_NULL;
    ds.title.bitmap     = OP_NULL;
    ds.title.draw_func  = OP_NULL;
    Umemset(ds.title.txt, 0x0000, 1);
}



/**************************************************************************************************
   Text Line Control Functions :

   Using Text line control functions make an application easy to display text data
   in SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG.

**************************************************************************************************/


/*==================================================================================================
    FUNCTION: ds_get_line 

    DESCRIPTION:
       Returns the text of the requested line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is mainly called to update the text of the line.
==================================================================================================*/
OP_UINT8 *ds_get_line 
( 
    OP_UINT8     line 
)
{
#if 0 
   if (line > ds.lines - 1)
    {
        /* increment line counts if it is a new line */
        line = ds.lines - 1;
    }
#else
   if (line > ds.max_lines - 1)
    {
        /* increment line counts if it is a new line */
        line = ds.max_lines - 1;
    }

#endif
    
    /* Set the update flag since it is called to update the line. */
    ds.line[line].flag = OP_TRUE;
    return (OP_UINT8 *)ds.line[line].txt;
}

/*==================================================================================================
    FUNCTION: ds_set_line 

    DESCRIPTION:
       Add one logical text line.
     
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
       See the deference to ds_add_line. 
       It gets logical text line and make it physical lines if the length of the text is longer 
       than a line.
==================================================================================================*/
void ds_set_line 
( 
    OP_UINT8    line, 
    OP_UINT8    *data 
)
{
    //int         leng;
    
    if (!data) return;

    /* Not to exceed maximum line */
    if (line > ds.max_lines-1)
    { 
        line = ds.max_lines - 1;
    }   
      
#if 0
    leng = UstrCharCount(data);
    if (leng < DS_SCRN_MAX_LINE_UC_LEN) 
    {
        //op_strcpy( (char *)ds.line[line].txt, (char *)data );
        Ustrcpy( ds.line[line].txt, data );
        ds.line[line].leng = leng;
    }
    else 
    {
        op_memcpy( (char *)ds.line[line].txt, (char *)data , DS_SCRN_MAX_LINE_UC_LEN );
        ds.line[ds.lines-1].leng = DS_SCRN_LINE_CWIDE;
    }
#else
    UstrNullCpy(ds.line[line].txt, data,(sizeof(ds.line[line].txt)-2));
    ds.line[line].leng = MIN(DS_SCRN_LINE_CWIDE, UstrCharCount(data));

#endif
    
    ds.line[line].flag = OP_TRUE;    
}


/*==================================================================================================
    FUNCTION: ds_set_line_rm 

    DESCRIPTION:
       Add one logical text line.
     
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
       See the deference to ds_add_line. 
       It gets logical text line and make it physical lines if the length of the text is longer 
       than a line.
==================================================================================================*/
void ds_set_line_rm 
( 
    OP_UINT8            line, 
    RM_RESOURCE_ID_T    txt_res_id 
)
{
    OP_UINT8    text_str[DS_SCRN_MAX_LINE_UC_LEN+2]; 
    //OP_UINT8    *ret_ptr = OP_NULL;

   
    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(txt_res_id, text_str, DS_SCRN_MAX_LINE_UC_LEN) ))
    {
        ds_set_line(line, text_str);
    }
}

/*==================================================================================================
    FUNCTION: ds_add_line 

    DESCRIPTION:
       Add a line of text.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
       See the deference to ds_set_line. 
       Charcters exceeding a line width will be trucated.
==================================================================================================*/
void ds_add_line 
( 
    OP_UINT8    *data 
)
{
    //int    leng;
    
    if (!data) return;
    
    if (++ds.lines > ds.max_lines) 
    {
        ds.lines = ds.max_lines;
        return;
    }
    
#if 0
    leng = Ustrlen(data)+2;
    leng = MIN (leng, DS_SCRN_MAX_LINE_UC_LEN+2);

    //op_strcpy( (char *)ds.line[ds.lines-1].txt, (char *)data );
    Ustrncpy( ds.line[ds.lines-1].txt, data, leng );

    ds.line[ds.lines-1].leng = MIN(UstrCharCount(data), DS_SCRN_LINE_CWIDE);
#else
    UstrNullCpy(ds.line[ds.lines-1].txt, data,(sizeof(ds.line[ds.lines-1].txt)-2));
    ds.line[ds.lines-1].leng = MIN(UstrCharCount(data), DS_SCRN_LINE_CWIDE);    
#endif

    ds.line[ds.lines-1].flag = OP_TRUE;

}


/*==================================================================================================
    FUNCTION: ds_add_line_rm 

    DESCRIPTION:
       Add a line of text.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
       See the deference to ds_set_line. 
       Charcters exceeding a line width will be trucated.
==================================================================================================*/
void ds_add_line_rm 
( 
    RM_RESOURCE_ID_T    txt_res_id 
)
{
    OP_UINT8    text_str[DS_SCRN_MAX_LINE_UC_LEN+2]; 
   
    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(txt_res_id, text_str,DS_SCRN_MAX_LINE_UC_LEN )))
    {
        ds_add_line(text_str);
    }
}

/*==================================================================================================
    FUNCTION: ds_del_line 

    DESCRIPTION:
       Delete a text line data .

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_del_line 
( 
    OP_UINT8    line 
)
{
    if (line > ds.max_lines-1) 
    {
        return;
    }
    
    op_memset( (OP_UINT8 *)ds.line[line].txt, SPACE, DS_SCRN_MAX_LINE_UC_LEN ); 
    ds.line[line].flag      = OP_TRUE;
    ds.line[line].sel       = OP_FALSE;
    ds.line[line].check     = OP_FALSE;
    ds.line[line].icon      = OP_NULL;
    ds.line[line].leng      = 0;
    ds.line[line].attr      = LA_DEFAULT;
    ds.line[line].data      = 0;
    ds.line[line].draw_func = OP_NULL;

}


/*==================================================================================================
    FUNCTION: ds_clear_lines 

    DESCRIPTION:
       Clear all text line data.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called by ds_set_screen_type.
==================================================================================================*/
void ds_clear_lines 
( 
    void 
)
{
    int i;
    OP_INT16 y;
    OP_INT16  LineHeight = LINE_HEIGHT;
    RM_BITMAP_T   *bitmap =OP_NULL;
    OP_INT16  TextOffset = LINE_TEXT_OFFSET;
    DS_LINEATTR    attr   = LA_DEFAULT;
    OP_UINT8    (*draw_func)(OP_UINT8, OP_UINT32) = OP_NULL;

    if ( ( ds.scrnMode == SCREEN_GENERAL) || 
          ( ds.scrnMode == SCREEN_GENERAL_BG) || 
          ( ds.scrnMode == SCREEN_IDLE)||
          ( ds.scrnMode == SCREEN_MENU))
    {
        y = 1; 
    }
    else if (ds.scrnMode == SCREEN_LIST && (ds.title.attr & LA_TITLE_SECOND_SUBMENU) == LA_TITLE_SECOND_SUBMENU)
    {
        y = ds.title.y.bottom + 1;    
#if (PROJECT_ID == ODIN_PROJECT)
        LineHeight = ds_get_image_height(BMP_SUB_MAINMENU_FOCUS_BAR_C1) + 1;
#else
        LineHeight = ds_get_image_height(BMP_SUB_MAINMENU_FOCUS_BAR_C1);
#endif

        bitmap=(RM_BITMAP_T *)util_get_bitmap_from_res(BMP_SECONDMENULINE_BG);
        TextOffset = (LineHeight- 16)/2;
		attr = LA_OWNERDRAW;
        draw_func = draw_SecondMenuLien;
    }
    else
    {
        y = LINE_BASE_Y ; 
    }      
    
    ds.lines = 0;
    for (i = 0; i < ds.max_lines; i++, y+= (OP_INT16)LineHeight) 
    {
        Umemset(ds.line[i].txt, 0x0000, 1);
        ds.line[i].flag      = OP_TRUE;
        ds.line[i].sel       = OP_FALSE;
        ds.line[i].check     = OP_FALSE;
        ds.line[i].number    = INDEX_NONE;
        ds.line[i].y.top     = y;
        ds.line[i].base.top  = ds.line[i].y.top + TextOffset;
        ds.line[i].y.bottom  = (OP_INT16)( ds.line[i].y.top + LineHeight -1);
        ds.line[i].it_margine = LINE_ICON_TEXT_MARGINE; 
        ds.line[i].base.bottom  = ds.line[i].y.bottom - TextOffset ;
        ds.line[i].attr      = attr;
        Umemset(ds.line[i].txt, 0x0000, 1);
        ds.line[i].leng      = 0;
        ds.line[i].icon      = OP_NULL;
        ds.line[i].bitmap    = bitmap;
        ds.line[i].data      = 0;
        ds.line[i].draw_func = draw_func;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_line_icon 

    DESCRIPTION:
       Set icon for the text line. The icon is placed at the head of the line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_line_icon 
(
    OP_UINT8     line,
    RM_ICON_T    *icon
)
{
    if (line > ds.max_lines-1) 
    {
        return;
    }
    
    ds.line[line].flag = OP_TRUE;
    
    if (icon) 
    {
        ds.line[line].attr |= LA_ICON;
        ds.line[line].icon = icon;
    }
#if 0
    else 
    {
        ds.line[line].attr &= ~LA_ICON;
        ds.line[line].icon = OP_NULL;
    }
#endif
}

/*==================================================================================================
    FUNCTION: ds_set_line_icon_rm 

    DESCRIPTION:
       Set icon for the text line. The icon is placed at the head of the line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_line_icon_rm 
(
    OP_UINT8            line,
    RM_RESOURCE_ID_T    icon_res_id
)
{

    if (line > ds.max_lines-1) 
    {
        return;
    }

    ds.line[line].flag = OP_TRUE;
        
    ds.line[line].icon = util_get_icon_from_res(icon_res_id);

    if ( ds.line[line].icon != OP_NULL)
    {
        ds.line[line].attr |= LA_ICON;
    }  
#if 0  
    else
    {
        ds.line[line].attr &= ~LA_ICON;
    }   
#endif 
}


/*==================================================================================================
    FUNCTION: ds_set_line_bitmap 

    DESCRIPTION:
       Set bitmap for the text line. The bitmap is drawn in the region of the line.
       If the bitmap is larger than the line height, it will be clipped.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_line_bitmap 
(
    OP_UINT8     line,
    RM_BITMAP_T    *bitmap
)
{
    if (line > ds.max_lines-1) 
    {
        return;
    }
    
    ds.line[line].flag = OP_TRUE;
    
    if (bitmap) 
    {
        ds.line[line].attr |= LA_BG_BITMAP;
        ds.line[line].bitmap = bitmap;
    }
    else 
    {
        ds.line[line].attr &= ~LA_BG_BITMAP;
        ds.line[line].bitmap = OP_NULL;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_line_bitmap_rm 

    DESCRIPTION:
       Set bitmap for the text line. The bitmap is drawn in the region of the line.
       If the bitmap is larger than the line height, it will be clipped.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_line_bitmap_rm 
(
    OP_UINT8            line,
    RM_RESOURCE_ID_T    bitmap_res_id
)
{

    if (line > ds.max_lines-1) 
    {
        return;
    }

    ds.line[line].flag = OP_TRUE;
        
    ds.line[line].bitmap = util_get_bitmap_from_res(bitmap_res_id);

    if ( ds.line[line].bitmap != OP_NULL)
    {
        ds.line[line].attr |= LA_BG_BITMAP;
    }    
    else
    {
        ds.line[line].attr &= ~LA_BG_BITMAP;
    }    

}


/*==================================================================================================
    FUNCTION: ds_set_line_draw_func

    DESCRIPTION:
       Set the user drawing function for a logical line.
       To draw a logical line by itself, assign user's line drawing function. 
       The function will be called instead of being drawn by the Display System. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Line drawing function must return the height (in pixel unit) of the drawing to keep the 
        line height.
==================================================================================================*/
void ds_set_line_draw_func 
( 
    OP_UINT8    line, 
    OP_UINT8    (*draw_func)(OP_UINT8, OP_UINT32) 
)
{
    if (line >= ds.max_lines)
    {
        line = 0;
    }
    
    if (ds.lines < line+1)
    { 
        ds.lines = line+1;
    }
    
    ds.line[line].flag     = OP_TRUE;
    Umemset(ds.line[line].txt, 0x0000, 1);
    ds.line[line].attr     |= LA_OWNERDRAW;
    ds.line[line].draw_func = draw_func;

}


/*==================================================================================================
    FUNCTION: ds_get_line_data 

    DESCRIPTION:
       Returns the line data for the user's line drawing function.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
OP_UINT32 ds_get_line_data 
( 
    OP_UINT8     line 
)
{
    if (line >= ds.max_lines)
    {
        line = 0;
    }
     
    return ds.line[line].data;
}


/*==================================================================================================
    FUNCTION: ds_set_line_data 

    DESCRIPTION:
       Set the line data for the user's line drawing function.
       This data is passed to the drawing function.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_line_data 
( 
     OP_UINT8     line, 
     OP_UINT32    data 
)
{
    if (line >= ds.max_lines)
    {
        line = 0;
    }
    
    ds.line[line].flag = OP_TRUE;
    ds.line[line].data = data;
}


/*==================================================================================================
    FUNCTION: ds_get_lineattr 

    DESCRIPTION:
       Returns line attribites.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If the line number exceed allowed maximum line, returns the attribute of the last one.
==================================================================================================*/
DS_LINEATTR ds_get_lineattr 
( 
     OP_UINT8     line 
)
{
    if (line < ds.max_lines)
    {
        return ds.line[line].attr;
    }
    else
    {
        return ds.line[ds.max_lines-1].attr;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_lineattr 

    DESCRIPTION:
       Set line attributes.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_lineattr 
( 
     OP_UINT8       line, 
     DS_LINEATTR    attr 
)
{
    if (line < ds.max_lines) 
    {
        ds.line[line].flag = OP_TRUE;

        if(attr & LA_FILL)
        {
            ds.line[line].attr |= LA_FILL;   
        }
        else
        {
            ds.line[line].attr &= ~LA_FILL; 
        }
        
        if (attr & LA_BACK_CHECKMARK)
        {
            attr &= ~LA_BACK_CHECKBOX;
            attr &= ~LA_BACK_ICON;
            attr &= ~LA_CHECKBOX;
            attr &= ~LA_ICON;
        }
        else if (attr & LA_BACK_CHECKBOX)
        {
            attr &= ~LA_CHECKBOX;
            attr &= ~LA_BACK_ICON;
        }
        if (attr & LA_BACK_ICON)
        {
            attr &= ~LA_ICON;
        }

        ds.line[line].attr |= attr;        
        
    }
}


/*==================================================================================================
    FUNCTION: ds_update_line 

    DESCRIPTION:
       Indicate that the line is updated.
       It is the way to avoid whole lines are refreshed unnecessary.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To force the line is refrshed immediately, ds_refresh_lines_items shall be called.
==================================================================================================*/
void ds_update_line 
( 
     OP_UINT8     line 
)
{
    /* is it valid line number ? */
    if (line < ds.lines) 
    {
        ds.line[line].flag = OP_TRUE;
    }
}


/*==================================================================================================
    FUNCTION: ds_update_all_lines 

    DESCRIPTION:
       Set the update flag for all the line at once.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
       It is not recomended using this function frequently.
       Use ds_update_line instead to avoid unnecessary refreshment.
       To force the lines are refrshed immediately, ds_refresh_lines_items shall be called.
==================================================================================================*/
void ds_update_all_lines 
( 
     void
)
{
    int    i;
    for (i = 0; i < ds.lines; i++) 
    {
        ds.line[i].flag = OP_TRUE;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_selection 

    DESCRIPTION:
       Make the line is exclusively selected.
       Radio button is used to display the selection.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        The line is the line index of the current screen and starts from 0.
        ds.focused_line is 0-based also.
==================================================================================================*/
void ds_set_selection 
( 
     OP_UINT8     line 
)
{
    OP_UINT8    index;

    if (line < ds.max_lines) 
    {
        /* Toggle off the previously selected line */
        for (index = 0; index < ds.max_lines; index++)
        {
            if ( ds.line[index].sel)
            {
                ds.line[index].sel = OP_FALSE;
                ds.line[index].flag = OP_TRUE; 
                break;
            }
        }
          
        /* update newly selected line */
        ds.line[line].sel = OP_TRUE; 
        ds.line[line].flag = OP_TRUE; 
    }
}


/*==================================================================================================
    FUNCTION: ds_set_line_number 

    DESCRIPTION:
       Set the absolute line number to display line number icon. LA_LINENUMBER is set by this.
       Radio button is used to display the selection.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Index is not the line number. It is the index of the list that is displayed through 
        this line control functions.
==================================================================================================*/
void ds_set_line_number 
( 
     OP_UINT8     line,
     OP_UINT8    line_number 
)
{
  
    if (line != INDEX_NONE) 
    {
        if (/*line >= 0 && */line < ds.max_lines)
        {
            ds.line[line].flag = OP_TRUE; 
            ds.line[line].number = line_number; 
            ds.line[line].attr |= LA_LINENUMBER;
        }
    }
}

/*==================================================================================================
    FUNCTION: ds_set_check 

    DESCRIPTION:
       Make the line is selected. Multiple selection is allowed,
       Checkbox button is used to display the selection.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        The line is the line index of the current screen and starts from 0.
        ds.focused_line is 0-based also.
==================================================================================================*/
void ds_set_check 
( 
     OP_UINT8       line, 
     OP_BOOLEAN     check 
)
{
    
    if (line != INDEX_NONE) 
    {
        if (line < ds.max_lines)
        {
            ds.line[line].flag = OP_TRUE; 
            ds.line[line].check = check;
        }
    }
}


/*==================================================================================================
    FUNCTION: ds_get_highlight 

    DESCRIPTION:
       Returns the currently highlighted (focused) line number in the screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
OP_UINT8 ds_get_highlight 
( 
     void 
)
{
    if (ds.focused_line <= ds.max_lines)
    {
        return  (int)(ds.focused_line);
    }
    else
    {
        return CURSOR_OFF;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_highlight 

    DESCRIPTION:
       Set the highlighted (focused) line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        The line is the line index of the current screen and starts from 0.
        ds.focused_line is 0-based also.
==================================================================================================*/
void ds_set_highlight 
( 
     OP_UINT8       line, 
     OP_BOOLEAN     on 
)
{
    /* unfouce the previously focused line */
    if (ds.focused_line != CURSOR_OFF)
    {
        ds.line[ds.focused_line].flag = OP_TRUE;
    }
    
    /* set and update current focused line */
    if (on && line < ds.max_lines) 
    {
        ds.focused_line    = line;
        ds.line[line].flag = OP_TRUE;
    }
    else
    {
        ds.focused_line = CURSOR_OFF;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_char_cursor 

    DESCRIPTION:
       Set cursor characters used as partial highlight. Used to set charcter cursor.

    ARGUMENTS PASSED:
       offset   - index (x) of the charcter in the line to be highlighted.
       line     - indicate line number to be highlighted. Starts from 0. 
       fontcolor  - Font color of the highlighted text. 
       backcolor  - Background color of the highlighted text. 
       *cursor    - Charcters used as a cursor.

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_char_cursor 
(
     OP_UINT8    offset, 
     OP_UINT8    line, 
     DS_FONTATTR attr,
     OP_BOOLEAN  blink,
     DS_COLOR    fontcolor, 
     DS_COLOR    backcolor, 
     OP_UINT8    *cursor
)
{
    /*if (line != CURSOR_OFF) 
    { 
        ds.line[ds.cCursor.line].flag = OP_TRUE;
    }*/ //2002/08/27 mihyang
  
    if (line < ds.max_lines) 
    {
        /* if the cursor is changed, rewrite the previous cursor */
        if (IS_NOT_NULL_STRING(ds.cCursor.cursor) && Ustrcmp (ds.cCursor.cursor, cursor))
        {
            refresh_cursor();
        }

        ds.cCursor.offset            = offset;
        ds.cCursor.line              = line;
        ds.cCursor.attr              = attr;
        ds.cCursor.fontcolor         = fontcolor;
        ds.cCursor.backcolor         = backcolor;

        UstrNullCpy(ds.cCursor.cursor, cursor,(sizeof(ds.cCursor.cursor)-2) );

        ds.line[ds.cCursor.line].flag = OP_TRUE;
   
        if (blink)
        {    
            if (ds.cCursor.blink == OP_FALSE)
            {
                ds.cCursor.blink = OP_TRUE;
                register_ds_event_handler(OPUS_TIMER_BLINK_CURSOR);
                OPUS_Start_Timer(OPUS_TIMER_BLINK_CURSOR, 1000, 0,  PERIODIC);
            }
            ds.cCursor.blink_status = OP_FALSE;
        }

        /* if the current screen mode is not one of line screen mode, refresh the cursor immediately */
        if ( (ds.scrnMode != SCREEN_LIST) && (ds.scrnMode != SCREEN_MENU) &&
              (ds.scrnMode != SCREEN_FUNC) &&(ds.scrnMode != SCREEN_FUNC_BG))
        {
            refresh_cursor();
        }
    } 
    else 
    {
        ds.cCursor.line = (OP_UINT8)CURSOR_OFF;
    }
}


/*==================================================================================================
    FUNCTION: ds_deact_partial_highlight 

    DESCRIPTION:
       Deactivate character cursor.

    ARGUMENTS PASSED:
       None

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_deact_char_cursor
(
     void
)
{
    if (ds.cCursor.line != CURSOR_OFF) 
    {
        ds.cCursor.blink_status = OP_FALSE;
        /* if the current screen mode is not one of line screen mode, refresh the cursor immediately */
        if ( (ds.scrnMode != SCREEN_LIST) && (ds.scrnMode != SCREEN_MENU) &&
              (ds.scrnMode != SCREEN_FUNC) &&(ds.scrnMode != SCREEN_FUNC_BG))
        {
            refresh_cursor();
        }
        ds.line[ds.cCursor.line].flag = OP_TRUE;
        ds.cCursor.line = (OP_UINT8)CURSOR_OFF;
        ds.cCursor.blink = OP_FALSE;
        op_memset (ds.cCursor.cursor, 0x00, sizeof(ds.cCursor.cursor));
    }
}


/**************************************************************************************************
   Graphic Item Control Functions in SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG.
**************************************************************************************************/
/*==================================================================================================
    FUNCTION: ds_get_item_attr 

    DESCRIPTION:
       Get graphic item attributes.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
DS_GITEMATTR ds_get_item_attr 
( 
     OP_UINT8     item 
)
{
    if (item < DS_SCRN_MAXITEMS)
    {
        return ds.item[item].attr;
    }
    else
    {
        return GIA_DEFAULT;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_item_attr 

    DESCRIPTION:
       Set the graphic item attributes.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_item_attr 
( 
     OP_UINT8        item, 
     DS_GITEMATTR    attr 
)
{
    if (item < DS_SCRN_MAXITEMS)
    {
       ds.item[item].attr = attr;
    }
    
    ds.item[item].flag    = OP_TRUE;
}


/*==================================================================================================
    FUNCTION: ds_set_bitmap_item 

    DESCRIPTION:
       Set bitmap data as a new graphical item. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_bitmap_item 
( 
     OP_UINT8       item, 
     RM_BITMAP_T    *bitmap 
)
{
    /* do boundary check */
    if (item >= DS_SCRN_MAXITEMS || !bitmap)  
    {
        return;
    }
    
    /* Increase number of items */
    if (ds.items <= item)
    {
        ds.items = item+1;
    }
    
    ds.item[item].bitmap = bitmap;
    
    ds.item[item].flag   = OP_TRUE;
    ds.item[item].attr  |= GIA_BITMAP;

}


/*==================================================================================================
    FUNCTION: ds_set_bitmap_item_rm 

    DESCRIPTION:
       Set bitmap data as a new graphical item. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_bitmap_item_rm 
( 
     OP_UINT8            item, 
     RM_RESOURCE_ID_T    bitmap_res_id 
)
{

    /* do boundary check */
    if (item >= DS_SCRN_MAXITEMS)  
    {
        return;
    }

    /* Increase number of items */
    if (ds.items <= item)
    {
        ds.items = item+1;
    }
        
    ds.item[item].flag = OP_TRUE;
    ds.item[item].bitmap = util_get_bitmap_from_res(bitmap_res_id);
     
    if ( ds.item[item].bitmap != OP_NULL)
    {
        ds.item[item].attr |= GIA_BITMAP;
    }
    else
    {
        ds.item[item].attr &=  ~GIA_BITMAP;
    }

}

/*==================================================================================================
    FUNCTION: ds_set_icon_item 

    DESCRIPTION:
       Set icon as a new graphical item. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_icon_item 
( 
     OP_UINT8     item, 
     RM_ICON_T    *icon 
)
{
    if (item >= DS_SCRN_MAXITEMS || !icon) 
    {
        return;
    }
    
    /* Increase number of items */
    if (ds.items <= item)
    {
        ds.items = item+1;
    }
    
    ds.item[item].icon = icon;
    
    ds.item[item].flag = OP_TRUE;
    ds.item[item].attr |= GIA_ICON;

}


/*==================================================================================================
    FUNCTION: ds_set_icon_item_rm 

    DESCRIPTION:
       Set icon as a new graphical item. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_icon_item_rm 
( 
     OP_UINT8            item, 
     RM_RESOURCE_ID_T    icon_res_id 
)
{
    if (item >= DS_SCRN_MAXITEMS) 
    {
        return;
    }
    
    /* Increase number of items */
    if (ds.items <= item)
    {
        ds.items = item+1;
    }
    
    ds.item[item].flag = OP_TRUE;
    ds.item[item].icon = util_get_icon_from_res(icon_res_id);
        
    if (ds.item[item].icon != OP_NULL)
    {
        ds.item[item].attr |= GIA_ICON;
    }
    else
    {
        ds.item[item].attr &= ~GIA_ICON;       
    }

}

/*==================================================================================================
    FUNCTION: ds_set_func_item 

    DESCRIPTION:
       Set a function as a graphical item. 
       This function is called when the item is need to be drawn.
       It is owner drawn drawing function.

icon for the text line. The icon is placed at the head of the line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        The return value of this passed draw function tells whether the updates by the drawing 
        function is refreshed regionally or not.
        Make the drawing function return TRUE if it updates regionaly.
        And make it return FALSE if the whole screen shall be updated (for example changing 
        background image). 
==================================================================================================*/
void ds_set_func_item
( 
     OP_UINT8       item, 
     OP_BOOLEAN    (*draw_func)(OP_UINT8 index, OP_UINT32 data)
)
{
    if (item >= DS_SCRN_MAXITEMS || !draw_func)  
    {
        return;
    }
    
     if (ds.items <= item)
    {
        ds.items = item+1;
    }
    
    ds.item[item].draw_func = draw_func;
    
    ds.item[item].flag = OP_TRUE;
    ds.item[item].attr |= GIA_OWNERDRAW;
}


/*==================================================================================================
    FUNCTION: ds_set_item_region 

    DESCRIPTION:
       Set icon for the text line. The icon is placed at the head of the line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_item_region 
( 
    OP_UINT8    item, 
    OP_INT16    left, 
    OP_INT16    top, 
    OP_INT16    right, 
    OP_INT16    bottom 
)
{
    if (item >= DS_SCRN_MAXITEMS)
    {
        return;
    }
     
    if (ds.items <= item)
    {
        ds.items = item+1;
    }
    
    ds.item[item].rect.left   = MIN(left, LCD_MAX_X-1);
    ds.item[item].rect.top    = MIN(top, LCD_MAX_Y-1);
    ds.item[item].rect.right  = MIN(right, LCD_MAX_X-1);
    ds.item[item].rect.bottom = MIN(bottom, LCD_MAX_Y-1); 
    
    ds.item[item].flag = OP_TRUE;
}


/*==================================================================================================
    FUNCTION: ds_set_item_data 

    DESCRIPTION:
       Set item data to be passed its drawing function.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_item_data 
( 
    OP_UINT8     item, 
    OP_UINT32    data 
)
{
    if (item >= DS_SCRN_MAXITEMS) 
    {
        return;
    }
    
    if (ds.items <= item)
    {
        ds.items = item+1;
    }
    
    ds.item[item].data = data;
    
    ds.item[item].flag = OP_TRUE;
}


/*==================================================================================================
    FUNCTION: ds_clear_items 

    DESCRIPTION:
       Clear all graphical item data.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_clear_items 
( 
    void 
)
{
    OP_INT8    i;

    ds.items = 0;
    for (i = 0; i < DS_SCRN_MAXITEMS; i++) 
    {
        ds.item[i].flag        = OP_TRUE;
        ds.item[i].attr        = GIA_DEFAULT;
        ds.item[i].rect.left   = 0;
        ds.item[i].rect.top    = 0;
        ds.item[i].rect.right   = 0;
        ds.item[i].rect.bottom  = 0;
        ds.item[i].bitmap       = OP_NULL;
        ds.item[i].icon         = OP_NULL;
        ds.item[i].data         = 0;
        ds.item[i].draw_func    = OP_NULL;
    }
}


/*==================================================================================================
    FUNCTION: ds_clear_item 

    DESCRIPTION:
       Clear an specified graphical item data.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_clear_item 
( 
     OP_UINT8       item 
)
{
    ds.item[item].flag        = OP_TRUE;
    ds.item[item].attr        = GIA_DEFAULT;
    ds.item[item].rect.left   = 0;
    ds.item[item].rect.top    = 0;
    ds.item[item].rect.right   = 0;
    ds.item[item].rect.bottom  = 0;
    ds.item[item].bitmap       = OP_NULL;
    ds.item[item].icon         = OP_NULL;
    ds.item[item].data         = 0;
    ds.item[item].draw_func    = OP_NULL;
}

/*==================================================================================================
    FUNCTION: ds_update_item 

    DESCRIPTION:
       Indicate that the passed item is updated so that it is refreshed.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_update_item 
( 
    OP_UINT8    item 
)
{
    if (item >= DS_SCRN_MAXITEMS) 
    {
       return;
    }
    
    if (item < ds.items)
    {
        ds.item[item].flag = OP_TRUE;
    }
}


/*==================================================================================================
    FUNCTION: ds_update_item 

    DESCRIPTION:
       Draw an item and refresh it to the LCD.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_refresh_item 
( 
    OP_UINT8    item 
)
{
    DS_RECT_T    rect;

    if (item >= DS_SCRN_MAXITEMS)  
    {
        return;
    }
    
    rect = ds.item[item].rect;
  
    if (item < ds.items) 
    {
        if (ds.item[item].attr & GIA_BITMAP)
        {
            ds_draw_bitmap_image(rect.left, rect.top, ds.item[item].bitmap);
        }
        
        if (ds.item[item].attr & GIA_ICON)
        {
            ds_draw_icon(rect.left, rect.top, ds.item[item].icon);
        } 
        
        if (ds.item[item].attr & GIA_OWNERDRAW && ds.item[item].draw_func)
        {
            /* refresh only the drawing function requests the refreshment. */
            /* Important comments!
               The return value of this passed draw function tells whether the updates by the 
               drawing function is refreshed regionally or not.
               Make the drawing function return TRUE if it updates regionaly.
               And make it return FALSE if the whole screen shall be updated (for example changing 
               background image). */

            if ((ds.item[item].draw_func)( item, ds.item[item].data ))
            {
                ds_refresh_region(rect.top, rect.top, rect.right, rect.bottom);
            }
        }
        else
        {    
            /* Refresh the item regionlly */    
            ds_refresh_region(rect.top, rect.top, rect.right, rect.bottom);
        }

        ds.item[item].flag = OP_FALSE;
    }
}

/*==================================================================================================
    FUNCTION: ds_refresh_lines_items 

    DESCRIPTION:
       It draws and refreshes as pre-defined attributes for line and item concept.
       It is called if the screen mode is one of SCREEN_LIST, SCREEN_MENU, or SCREEN_FUNC.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If it refreshes partial screen, returns FALSE. 
        If whole screen refrshment is required, returns TRUE so that the screen is refreshed at 
        once.
==================================================================================================*/
static void drawTitleLine
(
    void 
)
{
    OP_UINT16     x;    /* ds.rcWin.left*/
    OP_UINT16     y;    /* ds.rcWin.top*/
    OP_UINT16     x_text_limit;    /* ds.rcWin.right*/
    OP_UINT16      icon_width ;
    OP_UINT16      icon_height ;
    OP_INT16      y_line;
    DS_COLOR      font_bg;
    OP_INT8       i;
    OP_INT16      font_width = FONTX_NORMAL;

    /* reset the x, y coordinates */
    x = WIN_LEFT;
    y = WIN_TOP;
    x_text_limit = WIN_RIGHT;

    /* get background color */
    if (ds.scrnMode == SCREEN_FUNC_BG)
    {
        font_bg  = COLOR_TRANS;
    }
    else
    {
        font_bg  = COLOR_FONTBG;
    }

        if (ds.title.attr & LA_OWNERDRAW) 
        {
            if (ds.title.draw_func) 
            {
                /* add y pixels that are processed by the owner-draw function. */
                ds.title.y.bottom = y + (ds.title.draw_func)(0,0) -1; /* unmeaningful parameter */

                 y_line = ds.title.y.bottom + 1;
                 /* if the title exceeds default title area, adjust line coordinates */
                 if ( ds.title.y.bottom >= LINE_BASE_Y)
                 { 
                     for  ( i = 0; i < ds.max_lines; i++)
                     {
                         ds.line[i].y.top = y_line;
                         ds.line[i].base.top = ds.line[i].y.top + LINE_TEXT_OFFSET;
                         y_line += (OP_INT16)LINE_HEIGHT;
                         ds.line[i].y.bottom = y_line -1;                        
                         ds.line[i].base.bottom = ds.line[i].y.bottom - LINE_TEXT_OFFSET;
                     }
                 } 
            }
        }
        else if (IS_NOT_NULL_STRING(ds.title.txt))
        { 
#if (PROJECT_ID == ODIN_PROJECT)
			x = (OP_INT16)(GET_CENTER(x, LCD_MAX_X_COOR, UstrCharCount(ds.title.txt)*FONTX_SMALL));
#endif
            if (ds.title.bitmap)
            {
                draw_bitmap_image(AREA_TEXT, 0, TITLE_TOP, ds.title.bitmap);
            }
            else if(ds.cComposer.color_theme == COLOR_THEME_USER_DEFINE)
            {    /* fill title background */
                 fill_rect(AREA_TEXT, 0, TITLE_TOP, LCD_MAX_X_COOR, TITLE_BOTTOM, ds.cComposer.titleColor);
            }
            

            /* draw title icon */
            if (ds.title.icon) 
            {
                icon_height = ds.title.icon->biHeight;

                /* Center alignment */
                y = (OP_INT16)(GET_CENTER(ds.title.base.top, ds.title.base.bottom, icon_height));

                if (ds.title.icon->data)
                {
                    /* title icon should be the same size */
                    draw_icon(AREA_TEXT, 
                              x,
                              y, 
                              (RM_ICON_T*)ds.title.icon); 
                    x += (ds.title.icon->biWidth + ds.title.it_margine);
                }
            }

            if (ds.title.attr & LA_SIDEBUTTON)
            {
                x = 1;
                icon_width = ds_get_image_width(ICON_TITLE_ARROW_LEFT);
                x_text_limit = (OP_INT16)(LCD_MAX_X_COOR - icon_width);

                /* Center alignment */
                y = (OP_INT16)(GET_CENTER(ds.title.base.top, ds.title.base.bottom, ds_get_image_height(ICON_TITLE_ARROW_LEFT)));

                draw_icon (AREA_TEXT, 
                            x, 
                            y,
                            util_get_icon_from_res(ICON_TITLE_ARROW_LEFT));
                draw_icon (AREA_TEXT, 
                            x_text_limit, 
                            y,
                            util_get_icon_from_res(ICON_TITLE_ARROW_RIGHT));
                x += icon_width; 
            }

           /* draw title */
#if (PROJECT_ID == ODIN_PROJECT)
            draw_text_line(AREA_TEXT, 
                           x,
                           (OP_UINT16)(ds.title.base.top + 3), 
                           x_text_limit, 
                           ds.title.txt, 
                           OP_TRUE,
                           FA_WINTITLE,            
                           ds.cComposer.titleFontColor, 
                           font_bg);
#else
            draw_text_line(AREA_TEXT, 
                           x,
                           ds.title.base.top, 
                           x_text_limit, 
                           ds.title.txt, 
                           OP_TRUE,
                           FA_WINTITLE,            
                           ds.cComposer.titleFontColor, 
                           font_bg);
#endif
       }
    
        ds.title.flag  = OP_FALSE;
}

static OP_BOOLEAN draw_graphical_items
(
    OP_BOOLEAN     refresh_screen
)
{
    OP_INT8        i;
    OP_BOOLEAN    bg_image_used = OP_FALSE;
    SCRNAREA_ENUM_T  bitmap_area;

    /* get background color */
    if (ds.scrnMode == SCREEN_FUNC_BG)
    {
        bitmap_area = AREA_BITMAP;
    }
    else
    {
        bitmap_area = AREA_TEXT;
    }

        for (i = 0; i < ds.items; i++) 
        {
            if (ds.item[i].flag) 
            {
                /* draw bitmap */
                if (ds.item[i].attr & GIA_BITMAP && ds.item[i].bitmap) 
                {
                    draw_bitmap_image(bitmap_area, ds.item[i].rect.left, ds.item[i].rect.top, ds.item[i].bitmap);
                    if (refresh_screen == OP_FALSE)
                    { 
                        ds_refresh_region(ds.item[i].rect.left, 
                                          ds.item[i].rect.top,
                                          (OP_INT16)(ds.item[i].rect.left+ds.item[i].bitmap->biWidth), 
                                          (OP_INT16)(ds.item[i].rect.top+ds.item[i].bitmap->biHeight));
                    }
                    if  ( (ds.item[i].attr & GIA_BG_BITMAP) && (ds.scrnMode == SCREEN_FUNC))
                    {
                        /* indicate background image is used for this screen.*/
                        bg_image_used = OP_TRUE;
                    } 
                }
                /* draw icon */
                if (ds.item[i].attr & GIA_ICON && ds.item[i].icon) 
                {
                    draw_icon(AREA_TEXT, ds.item[i].rect.left, ds.item[i].rect.top, ds.item[i].icon);
                    if (refresh_screen == OP_FALSE)
                    {
                        ds_refresh_region(ds.item[i].rect.left, 
                                          ds.item[i].rect.top,
                                          (OP_INT16)(ds.item[i].rect.left+ds.item[i].icon->biWidth), 
                                          (OP_INT16)(ds.item[i].rect.top+ds.item[i].icon->biHeight));
                    }
                }
                /* draw owner function*/
                if (ds.item[i].attr & GIA_OWNERDRAW && ds.item[i].draw_func)
                {
                    if ((ds.item[i].draw_func)( i, ds.item[i].data ) && refresh_screen == OP_FALSE)
                    { 
                        ds_refresh_region(ds.item[i].rect.left, 
                                          ds.item[i].rect.top,
                                          ds.item[i].rect.right, 
                                          ds.item[i].rect.bottom);       
                    }
                }
                ds.item[i].flag = OP_FALSE;
            }
        }
        return bg_image_used;
    }   

static void draw_text_lines
(
    OP_BOOLEAN     is_bg_image_used,
    OP_BOOLEAN     refresh_screen
)
{
    OP_INT8        i;
    OP_INT8        j;
    OP_UINT16       icon_width ;
    OP_UINT8       index;
    DS_FONTATTR   fontattr;
    DS_COLOR       fontcolor;
    DS_COLOR      font_bg;
    OP_INT16       x;
    OP_INT16       y=0;
    OP_INT16       x_text_limit;
    OP_INT16       y_bottom;
    OP_INT16       y_line;
    OP_INT16       y_text; 
    OP_INT16       x_fill_start;
    OP_INT16       font_width = FONTX_NORMAL;
    OP_INT16       string_width;
    DS_COLOR       btn_dark_color;
    DS_COLOR       btn_bright_color;
    DS_COLOR       btn_fill_color;
    SCRNAREA_ENUM_T  bitmap_area;
    DS_LINEATTR    lineattr;
    DS_LINEATTR    attr_no_highlight_bar = LA_DEFAULT;
    RM_RESOURCE_ID_T res_id;
    OP_UINT8    uiRightX = LCD_MAX_X_COOR;
    OP_UINT8    uiOffestX= LINE_BACK_ICON_XOFFSET;
    SCROLLBAR_INFO *pScrollbar = OP_NULL;

    /* get font background color */
    if (ds.scrnMode == SCREEN_FUNC_BG)
    {
        font_bg  = COLOR_TRANS;
        bitmap_area = AREA_BITMAP;
    }
    else
    {
        font_bg  = COLOR_FONTBG;
        bitmap_area = AREA_TEXT;
    }

    /*Get scrollbar*/
    pScrollbar = get_scrollbar();
    /* some line attributes display line highlight differently. */
    attr_no_highlight_bar = LA_SIDEBUTTON | LA_NARROW_SIDEBUTTON | LA_FOCUS_FILL |
                         LA_FILL | LA_BUTTONSTYLE| LA_EDITLINE| LA_BG_BITMAP;
    if(pScrollbar->IsEnableScrollbar && ds_get_screen_mode() == SCREEN_LIST)
    {
        uiRightX = LCD_MAX_X_COOR - get_scrollbar_width();
        uiOffestX= LINE_BACK_ICON_XOFFSET - get_scrollbar_width() + 11;
    }else
    {
        uiRightX = LCD_MAX_X_COOR;
        uiOffestX= LINE_BACK_ICON_XOFFSET;
    }
    /* draw line text */
    /* repeat until to meet the last line or bottom of the screen */
    for (i = 0; i < ds.max_lines && y <= (ds.rcWin.bottom-LINE_HEIGHT+1); i++) 
    {
        if (ds.line[i].flag)     //need to update the line
        {
            x        = WIN_LEFT;    
            y        = ds.line[i].y.top+1;
            y_bottom = ds.line[i].y.bottom;
            y_text   = ds.line[i].base.top+1;

            fontattr = FA_LISTDEFAULT | FONT_OVERLAP;
            lineattr = ds.line[i].attr;

            if(pScrollbar->IsEnableScrollbar)
             {
                   x_text_limit = uiRightX;
             }
            else
            {
              x_text_limit = WIN_RIGHT;
            }
            /* Owner draw by itself, call registerd drawing function */
            if (lineattr & LA_OWNERDRAW) 
            {
                if (ds.line[i].draw_func) 
                {
                    OP_INT16 height= (ds.line[i].draw_func)( i, ds.line[i].data );
                    if(height)
                    {
                        /* add y pixels that are processed by the owner-draw function. */
                        ds.line[i].y.bottom = (y-1) + height -1;
                        
                        /* if it is not the last line, update next lines's y coordinates */
                        y_line = ds.line[i].y.bottom + 1;
                        for  ( j =  i+1; j < ds.max_lines; j++)
                        {
                            ds.line[j].y.top = y_line;
                            ds.line[j].base.top = ds.line[j].y.top + LINE_TEXT_OFFSET;
                            y_line += (OP_INT16)LINE_HEIGHT;
                            ds.line[j].y.bottom = y_line -1;                        
                            ds.line[j].base.bottom = ds.line[j].y.bottom - LINE_TEXT_OFFSET;
                        } 
                    } 
                }
            }
            else
            {
                if  ( (i == ds.focused_line)  &&
                        (!( lineattr & attr_no_highlight_bar)))
                { 
                    /* clear the line delimeter */
                    ds_fill_text(0, (OP_INT16)(ds.line[i].y.top), uiRightX, (OP_INT16)(ds.line[i].y.top), COLOR_WINBG); 
                    /* in case of submenu of main menu, use different focus bar */
                    /*if (ds.title.attr & LA_TITLE_MAIN_SUBMENU)
                    {
                        draw_bitmap_image(AREA_TEXT, 
                                             0, 
                                             y, 
                                             (RM_BITMAP_T *)util_get_bitmap_from_res(BMP_SUB_MAINMENU_FOCUS_BAR_C1+ds.cComposer.focusbarColor));
                    }
                    else*/
                    {
                        ds_draw_focus_bar( (OP_INT16)(y-1), y_bottom);
                    }
                }
                else
                {
                    if  (is_bg_image_used == OP_FALSE) 
                    { 
                        /* clear text line region */
                        ds_fill_text(0, (OP_INT16)(ds.line[i].y.top), uiRightX, (OP_INT16)(ds.line[i].y.bottom), font_bg); 
                    }
                }

                /* Draw bg bitmap */               
                if (lineattr & LA_BG_BITMAP) 
                {  
                    if (ds.line[i].bitmap) 
                    {
                        if (ds.line[i].bitmap->data)
                        { 
                            draw_bitmap_image(bitmap_area, 
                                                x, 
                                                ds.line[i].y.top, 
                                                (RM_BITMAP_T *)ds.line[i].bitmap);
                            
                            /* in case of bg image is used, change bg of font */
                            font_bg = COLOR_TRANS;

                            /* save the x start coordinate to fill when it is focused */
                            x_fill_start = x;
                            //x_text_limit = MIN(LCD_MAX_X_COOR, (OP_INT16)(x + ds.line[i].bitmap->biWidth -1));
                            x_text_limit = MIN(uiRightX, (OP_INT16)(x + ds.line[i].bitmap->biWidth -1));
                        } 
                    }
                }

                if (lineattr & LA_SUBITEM) 
                {  
                    x += C_WIDTH; /* give one charcter space */
                }
                 
                /* Draw Icons */
                if (lineattr & LA_RADIOBUTTON)   /* radio button */
                { 
                    index = (ds.line[i].sel) ? 1:0;
                    res_id = (RM_RESOURCE_ID_T)(ICON_RADIOBTN_OFF + index);
                    draw_icon(AREA_TEXT, 
                             x, 
                             (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(res_id)), 
                             (RM_ICON_T *)util_get_icon_from_res(res_id));

                    x += LINE_ICON_WIDTH + LINE_ICON_TEXT_MARGINE; 
                }
                else if (lineattr & LA_CHECKBOX)  /*check box */
                {  
                    index = (ds.line[i].check) ? 1:0;
                    res_id = (RM_RESOURCE_ID_T)(ICON_CHECKBOX_OFF + index);
                    draw_icon(AREA_TEXT, 
                             x, 
                             (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(res_id)), 
                             (RM_ICON_T *)util_get_icon_from_res(res_id));

                     x += LINE_ICON_WIDTH +LINE_ICON_TEXT_MARGINE; 
                }
                else if (lineattr & LA_LINENUMBER) 
                {  
                    OP_UINT16            icon_y ;
                    RM_RESOURCE_ID_T    lni_base_id; /* line number icon base resouce id according to the selected color */
                    DS_COLORTHEME_ENUM_T    color_theme = ds.cComposer.color_theme;
                    if(color_theme == COLOR_THEME_USER_DEFINE)
                    {
                        color_theme = COLOR_THEME_1;
                    }

                    lni_base_id  = ICON_LINE_NUM_C1_1;
                    icon_y = (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(lni_base_id)) ;
                    /* draw different number icon depending on highlight */
                  
                         /* line number through 1~NUM_LINENUM_ICON_COUNT will have 1~NUM_LINENUM_ICON_COUNT number icon, after that * will be drawn */
                    if ( ds.line[i].number <= NUM_LINENUM_ICON_COUNT)                                                 
                    {
                        draw_icon(AREA_TEXT, 
                                    x, 
                                    icon_y, 
                                    (RM_ICON_T *)util_get_icon_from_res(lni_base_id + ds.line[i].number-1)); 
                    }
                    else /* line number over 11 will have * (asterisk) icon */
                    {
                        draw_icon(AREA_TEXT, 
                                    x, 
                                    icon_y, 
                                    (RM_ICON_T *)util_get_icon_from_res(lni_base_id + NUM_LINENUM_ICON-1));
                    }

                    x += LINE_ICON_WIDTH +LINE_ICON_TEXT_MARGINE; 
                }
                else if (lineattr & LA_ICON) 
                {  
                    icon_width = LINE_ICON_WIDTH;
                    if (ds.line[i].icon) 
                    {
                        if (ds.line[i].icon->data)
                        { 
                            icon_width = ds.line[i].icon->biWidth;
                            icon_width = (icon_width > LINE_ICON_WIDTH) ? icon_width : LINE_ICON_WIDTH;
                            draw_icon(AREA_TEXT, 
                                        x, 
                                        (OP_INT16)GET_CENTER(y, y_bottom, ds.line[i].icon->biHeight), 
                                        (RM_ICON_T *)ds.line[i].icon);
                        } 
                    }
                     /* if the line attribte is LA_ICON but no icon data, 
                       do not draw icon but leave the icon space empty */
                    x += (OP_INT16)(icon_width + LINE_ICON_TEXT_MARGINE); 
                }

                /* line back attributes */
                if (lineattr & LA_BACK_CHECKMARK)   /* check mark button at the back of the line as radio button */
                { 
                    /* only if it is selected line */
                    if (ds.line[i].sel)
                    { 
                        draw_icon(AREA_TEXT, 
                                  uiOffestX , 
                                 (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(ICON_LINE_CHECK)), 
                                 (RM_ICON_T *)util_get_icon_from_res(ICON_LINE_CHECK));
                    }
                    x_text_limit = (OP_INT16)(uiOffestX-1);;
                }
                else if (lineattr & LA_BACK_CHECKBOX)  /* back check box */
                {  
                    index = (ds.line[i].check) ? 1:0;
                    res_id = (RM_RESOURCE_ID_T)(ICON_CHECKBOX_OFF + index);
                    draw_icon(AREA_TEXT,  
                              uiOffestX, 
                             (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(res_id)), 
                             (RM_ICON_T *)util_get_icon_from_res(res_id));

                    x_text_limit = (OP_INT16)(uiOffestX-1);;
    
                }           
                else if (lineattr & LA_BACK_ICON)  /* back icon */
                {  
                    x_text_limit = (OP_INT16)(uiOffestX-1);;
                    if (ds.line[i].icon) 
                    {
                        if (ds.line[i].icon->data)
                        { 
                            draw_icon(AREA_TEXT, 
                                       uiOffestX, 
                                       (OP_INT16)GET_CENTER(y, y_bottom, ds.line[i].icon->biHeight), 
                                       (RM_ICON_T *)ds.line[i].icon);
                        } 
                    }
                }

                 /* draw line arrows */
                if (lineattr & LA_SIDEBUTTON)  /* Draw Left and Right arrow at both end of the line. */
                {  
                    x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_RIGHT) - 2);
                    res_id = ( i == ds.focused_line) ? ICON_ARROW_LEFT : ICON_ARROW_LEFT_OFF;
                    draw_icon(AREA_TEXT, 
                              x, 
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(res_id)), 
                              (RM_ICON_T *)util_get_icon_from_res(res_id));
                    res_id = ( i == ds.focused_line) ? ICON_ARROW_RIGHT : ICON_ARROW_RIGHT_OFF;
                    draw_icon(AREA_TEXT, 
                              (OP_INT16)(x_text_limit+2),
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(res_id)), 
                             (RM_ICON_T *)util_get_icon_from_res(res_id));
                     x += (ds_get_image_width(ICON_ARROW_LEFT) + 1); 

                     /* save the x start coordinate to fill when it is focused */
                     x_fill_start = x;

                    /* text inside of the side buttons shall be center aligned */
                    fontattr &= ~LA_RIGHT_TEXT;
                    fontattr |= LA_CENTER_TEXT;
                }
                else if (lineattr & LA_NARROW_SIDEBUTTON)  
                {  
                    x_text_limit = (OP_INT16)(x+font_width+LINE_NARROW_SIDEBUTTON_WIDTH -2);
                    res_id = ( i == ds.focused_line) ? ICON_ARROW_LEFT : ICON_ARROW_LEFT_OFF;
                     draw_icon(AREA_TEXT, 
                              x, 
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(res_id)), 
                              (RM_ICON_T *)util_get_icon_from_res(res_id));

                    res_id = ( i == ds.focused_line) ? ICON_ARROW_RIGHT : ICON_ARROW_RIGHT_OFF;
                    draw_icon(AREA_TEXT, 
                              (OP_INT16)(x_text_limit+2),
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(res_id)), 
                              (RM_ICON_T *)util_get_icon_from_res(res_id));
                     x += (ds_get_image_width(ICON_ARROW_LEFT) + 1); 

                    /* save the x start coordinate to fill when it is focused */
                    x_fill_start = x;

                    /* text inside of the side buttons shall be center aligned */
                    fontattr &= ~LA_RIGHT_TEXT;
                    fontattr |= LA_CENTER_TEXT;
                }
                else if (lineattr & LA_RIGHTBUTTON) 
                {
                    x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_RIGHT - 1));
                    draw_icon(AREA_TEXT, 
                              (OP_INT16)(WIN_RIGHT - ds_get_image_width(ICON_ARROW_RIGHT)), 
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(ICON_ARROW_RIGHT)), 
                              (RM_ICON_T *)util_get_icon_from_res(ICON_ARROW_RIGHT));
                }
                else if (lineattr & LA_LEFTBUTTON) 
                {
                    x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_LEFT - 1));
                    draw_icon(AREA_TEXT, 
                              (OP_INT16)(WIN_RIGHT - ds_get_image_width(ICON_ARROW_LEFT)), 
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(ICON_ARROW_LEFT)), 
                              (RM_ICON_T *)util_get_icon_from_res(ICON_ARROW_LEFT));

                }
                else if (lineattr & LA_UPBUTTON) 
                {
                    x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_UP - 1));
                    draw_icon(AREA_TEXT, 
                              (OP_INT16)(WIN_RIGHT - ds_get_image_width(ICON_ARROW_UP)), 
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(ICON_ARROW_UP)), 
                              (RM_ICON_T *)util_get_icon_from_res(ICON_ARROW_UP));
                }
                else if (lineattr & LA_DOWNBUTTON) 
                {
                    x_text_limit = (OP_INT16)(x_text_limit - ds_get_image_width(ICON_ARROW_DOWN - 1));
                    draw_icon(AREA_TEXT, 
                              (OP_INT16)(WIN_RIGHT - ds_get_image_width(ICON_ARROW_DOWN)), 
                              (OP_INT16)GET_CENTER(y, y_bottom, ds_get_image_height(ICON_ARROW_DOWN)), 
                              (RM_ICON_T *)util_get_icon_from_res(ICON_ARROW_DOWN));
                }

                 /* fill the line or draw line frame */
                if (lineattr & LA_FILL)
                {
                    /* save the x start coordinate to fill when it is focused */
                    x_fill_start = x;
                    ds_fill_text(x, y, x_text_limit, ds.line[i].y.bottom, COLOR_FILLBG); 
                    /* if the previous line is also filled, fill between the lines */
                    if ( (i > 0) && (ds.line[i-1].attr & LA_FILL))
                    {
                        ds_fill_text(x, ds.line[i].y.top, x_text_limit, ds.line[i].y.top, COLOR_FILLBG); 
                    }
                }
                else if (lineattr & LA_FOCUS_FILL)
                {
                    /* save the x start coordinate to fill when it is focused */
                    x_fill_start = x;
                }
                /* draw button style frame outside of the text */
                else if ( (lineattr & LA_BUTTONSTYLE) || (lineattr & LA_EDITLINE)) 
                {
                    if (lineattr & LA_BUTTONSTYLE)  
                    {
                       if (i == ds.focused_line)  /* Currently focused (highlighted) line */
                       {
                            btn_dark_color = COLOR_FILL_BTNDARK;
                            btn_bright_color = COLOR_FILL_BTNBRIGHT;
                            btn_fill_color = ds.cComposer.focusbarColor;
                       }
                       else
                       {
                            btn_dark_color = COLOR_BTNDARK;
                            btn_bright_color = COLOR_BTNBRIGHT;
                            btn_fill_color = COLOR_BTNFILL;
                       } 
                    }
                    else /* (lineattr & LA_EDITLINE) */
                    {
                       if (i == ds.focused_line)  /* Currently focused (highlighted) line */
                       {
                            btn_dark_color = COLOR_FILL_EDIT_BTNDARK;
                            btn_bright_color = COLOR_FILL_EDIT_BTNBRIGHT;
                            btn_fill_color = ds.cComposer.focusbarColor;
                       }
                       else
                       { 
                            btn_dark_color = COLOR_EDIT_BTNDARK;
                            btn_bright_color = COLOR_EDIT_BTNBRIGHT;
                            btn_fill_color = COLOR_EDIT_BTNFILL;
                       } 
                    } 

                    draw_button(AREA_TEXT, 
                                 x, 
                                 y, 
                                 x_text_limit, 
                                 (OP_INT16)(ds.line[i].y.bottom),
                                 btn_dark_color, 
                                 btn_bright_color,
                                 btn_fill_color);
                    /* save the x start coordinate of the button to fill inside when it is focused */
//                    x_fill_start = x+1;
                    x +=2;
                    x_text_limit -=2;                                             
                }
    
    
                /* Draw Text */
                if (lineattr & LA_SMALLFONT)    /* small-sized font */ 
                {
                    fontattr |= FONT_SIZE_SMALL; 
                }
    
#ifdef NOT_SUPPORTED
                if (lineattr & LA_LARGEFONT)   /* larger-sized font */
                {
                    fontattr &= ~FONT_SIZE_SMALL; 
                }
#endif
                     
                if (lineattr & LA_OVERLAP)  /* overlap */
                { 
                     fontattr |= FONT_OVERLAP;
                }                                                                              
         
                /* in case of small font, write the text below in one pixel */
                if (fontattr & FONT_SIZE_SMALL)
                {
                    y_text ++;
                    font_width = FONTX_SMALL;
                } 
    
                if (lineattr & LA_CENTER_TEXT)  /* center text */
                { 
                    if (IS_NOT_NULL_STRING(ds.line[i].txt)) 
                    {
                        string_width = ds.line[i].leng*font_width;
                        if ( string_width < (x_text_limit-x+1))
                        {
                            /* Center alignment */
                            x = (OP_INT16)(GET_CENTER(x, x_text_limit, string_width));
                        }
                    }
                }
                else if (lineattr & LA_RIGHT_TEXT)  /* right alignment */
                {  
                    if (IS_NOT_NULL_STRING(ds.line[i].txt)) 
                    {
                        x = (OP_INT16)(x_text_limit - ds.line[i].leng*font_width-1); 
                    }
                } 
    
                if (i == ds.focused_line)  /* Currently focused (highlighted) line */
                {   
                    /* if the hightlight bar was not drawn earlier, draw it now */                                
                     if (lineattr & attr_no_highlight_bar)
                     {
                         if ( !(lineattr & LA_BUTTONSTYLE) && !(lineattr & LA_EDITLINE))
                         { 
                             /* draw highlight bar */
                             ds_fill_text(x_fill_start, 
                                           y,
                                           x_text_limit, 
                                           ds.line[i].y.bottom, 
                                           ds.cComposer.focusbarColor);                                                 
                         }

                        if (IS_NOT_NULL_STRING(ds.line[i].txt)) 
                        {
                            /* draw focused text */
                            if (lineattr & LA_INACTIVE_TEXT)
                            {
                                fontcolor = COLOR_INACTIVE_FONTFG;
                            }
                            else
                            {
                                fontcolor = ds.cComposer.focusbarFontColor;
                            }
                            draw_text_line(AREA_TEXT, 
                                             x,
                                             y_text, 
                                             x_text_limit, 
                                             (OP_UINT8 *)ds.line[i].txt,  
                                             OP_TRUE,
                                             fontattr, 
                                             fontcolor, 
                                             font_bg);
                        }
                    }    
                    else
                    {
                        /* highlight bar was already drawn ealier, draw text here */
                         if (IS_NOT_NULL_STRING(ds.line[i].txt)) 
                        {
                            /* draw focused text */
                            draw_text_line(AREA_TEXT, 
                                             x,
                                             y_text, 
                                             x_text_limit, 
                                             (OP_UINT8 *)ds.line[i].txt,  
                                             OP_TRUE,
                                             fontattr, 
                                             ds.cComposer.focusbarFontColor, 
                                             font_bg);
                        }
                    }                 
                }
                else /* Unfocused line (unhighlighted ) */
                {  
                    DS_COLOR fontcolor = ds.cComposer.menuFontColor;
                     //ds_fill_text(x, y, WIN_RIGHT, (OP_INT16)(y+LINE_HEIGHT), backcolor); 
                      
                    /* inactive line. Only applied to non-focused line. */                    
                    if (lineattr & LA_INACTIVE_TEXT)
                    {
                        fontcolor = COLOR_INACTIVE_FONTFG;
                    }
    
                    if (IS_NOT_NULL_STRING(ds.line[i].txt)) 
                    {
                        draw_text_line(AREA_TEXT, 
                                         x, 
                                         y_text, 
                                         x_text_limit, 
                                         (OP_UINT8 *)ds.line[i].txt, 
                                         OP_TRUE,
                                         fontattr, 
                                         fontcolor, 
                                         font_bg);      
                     }             
                 }
            }
    
            /* Character cursor - Partially highlighted */
            if (i == ds.cCursor.line) 
            {
                fontattr &= ~FONT_OVERLAP;

                /* recalculate x position to allow LA_OWNERDRAW attributed line to utilize line attributes
                   to get the right position of the cursor accodring to the desing rules */                  
                x = get_line_text_x_pos(i);
                
                draw_text_line(AREA_TEXT, 
                               (OP_INT16)(x+font_width*ds.cCursor.offset), 
                               y_text, 
                               x_text_limit = WIN_RIGHT,
                               (OP_UINT8 *)ds.cCursor.cursor, 
                               OP_TRUE, 
                               fontattr, 
                               ds.cCursor.fontcolor, 
                               ds.cCursor.backcolor);
            } 
    
            if (refresh_screen == OP_FALSE)
            {
                /* refresh partially line by line */
                ds_refresh_region(0, (OP_INT16)(ds.line[i].y.top), uiRightX, (OP_INT16)(ds.line[i].y.bottom)) ;
            }
            ds.line[i].flag = OP_FALSE;
        }
    }
}

/*==================================================================================================
    FUNCTION: ds_refresh_lines_items 

    DESCRIPTION:
       It draws and refreshes as pre-defined attributes for line and item concept.
       It is called if the screen mode is one of SCREEN_LIST, SCREEN_MENU, or SCREEN_FUNC.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If it refreshes partial screen, returns FALSE. 
        If whole screen refrshment is required, returns TRUE so that the screen is refreshed at 
        once.
==================================================================================================*/
OP_BOOLEAN ds_refresh_lines_items
(
    void
)
{
    OP_BOOLEAN     refresh_screen = OP_FALSE;
    OP_BOOLEAN     is_bg_image_used = OP_FALSE;
    
    /* draw title */
    if (ds.title.flag) 
    {
       drawTitleLine();
       refresh_screen = OP_TRUE;
    }
    
    /* draw graphical items */
    if  ( (ds.scrnMode == SCREEN_FUNC) || (ds.scrnMode == SCREEN_FUNC_BG) )
    {
        is_bg_image_used = draw_graphical_items(refresh_screen);
    }
    
    /* draw text lines */
    draw_text_lines(is_bg_image_used, refresh_screen);
    
    return refresh_screen;
}


/**************************************************************************************************
      SCREEN MODE CONTROL FUNCTIONS
**************************************************************************************************/
/*==================================================================================================
    FUNCTION: ds_get_screen_mode 

    DESCRIPTION:
       Returns the current screen mode.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
DS_SCRNMODE_ENUM_T ds_get_screen_mode 
( 
    void 
)
{
    return ds.scrnMode;
}


/*==================================================================================================
    FUNCTION: ds_set_screen_mode 

    DESCRIPTION:
       Set screen mode to one of pre-defined screen mode.
       It is recommented to call this function to switch to another screen mode.
       It initialize and set screen mode dependant variables according to the screen mode
       characteristics.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Recommended screenformat sequences for line text and graphical items:
        1. Call this function
        2. set a title of a screen by calling ds_set_title()
        3. call ds_set_line() to set text data.
        4. call ds_set_lineattr() to apply the attributes of the line.
           For owner draw type, set drawing function, set data of the function. (in case of 
           graphical item, set region of the item)
        5. call ds_refresh_lines_items() to draw and refresh.
==================================================================================================*/
DS_SCRNMODE_ENUM_T ds_set_screen_mode 
( 
    DS_SCRNMODE_ENUM_T scrn 
)
{
    DS_SCRNMODE_ENUM_T    prev_scrn;
  
     prev_scrn = ds.scrnMode;
    clear_update_region();
    ds.scrnMode = scrn;
    ds_stop_animation(&ds.ani); 
    init_scrollbar();
    ds_set_extend_softkey(OP_FALSE);
    switch (scrn) 
    {
        case SCREEN_MAINMENU:
            ds.bg_updated = OP_TRUE;  
            ds_clear_disp_annun();
             /* background image is to be refreshed */

            /* clear drawing buffer - text buffer is used */
            ds_clear_text();
            ds_clear_image();
            break;

        case SCREEN_IDLE:
            ds.max_lines = DS_SCRN_MAXLINES;    
            ds.bg_updated = OP_TRUE;
            ds_set_disp_annun();
            /* clear drawing buffer - text buffer is used */
            ds_clear_text();
            ds_set_extend_softkey(OP_TRUE);
            break;

        case SCREEN_MENU:
        case SCREEN_LIST:
        case SCREEN_FUNC:
        case SCREEN_PLAIN_TEXT:
            ds.max_lines = DS_SCRN_MAXLINES;    
            ds.title.y.top = 0;
            ds.title.y.bottom = LINE_BASE_Y - 1;
            ds_set_disp_annun();
            ds_clear_lines();
            ds_clear_items();
            ds_draw_text_win_frame();

            break;

        case SCREEN_FUNC_BG:
            ds.max_lines = DS_SCRN_MAXLINES;    
            ds.title.y.top = 0;
            ds.title.y.bottom = LINE_BASE_Y - 1;
            ds_set_disp_annun();
            ds_clear_items();
            ds_draw_bg_win_frame();
            ds_clear_text();

            break;

        case SCREEN_GENERAL: /* Display annuciator and draw freely in text buffer */
            ds.max_lines = DS_SCRN_MAXLINES;    
            ds.bg_updated = OP_TRUE;
            ds_set_disp_annun();
            ds_draw_text_win_frame();
            break;

        case SCREEN_GENERAL_BG: /* Display annuciator and draw freely in text and bitmap buffers */
            ds.max_lines = DS_SCRN_MAXLINES;    
            ds.bg_updated = OP_TRUE;
            ds_set_disp_annun();
            ds_draw_bg_win_frame();
            break;

       case SCREEN_FULL_IMAGE: /* full screen size image - Only for Power on/off animation, Games, Worldtime - bitmap buffer is used.  */
            ds_clear_disp_annun();
            break;
            
       case SCREEN_OWNERDRAW: /* non-displaying annuciator and draw freely */
            ds_set_disp_annun();
            /*ds_draw_bg_win_frame();*/
            ds_clear_image();
            ds_clear_text();
            break;

       case SCREEN_OPENWINDOW: /* only for cover close */
            break;

        default: /* screen none */ 
             ds_set_disp_annun();
             ds_draw_bg_win_frame();
             ds_clear_text();
            break;

    }
  
    /* Reset the text screen starintg address. */
    if ( (ds.scrnMode == SCREEN_IDLE) || 
          (ds.scrnMode == SCREEN_GENERAL) || 
          (ds.scrnMode == SCREEN_PLAIN_TEXT) ||
          (ds.scrnMode == SCREEN_GENERAL_BG))
    {
        /* determine window rectangle */
        util_set_rectangle(&ds.rcWin, 0,0, LCD_MAX_X_COOR, LCD_MAX_Y_COOR); 
        ds_clear_lines();
    }
    else if ( (ds.scrnMode == SCREEN_LIST) || 
               (ds.scrnMode == SCREEN_MENU) || 
               (ds.scrnMode == SCREEN_FUNC) || 
               (ds.scrnMode == SCREEN_FUNC_BG))
    {
        util_set_rectangle(&ds.rcWin, DS_SCRN_DEFAULT_SMARGINE,0, (OP_INT16)(LCD_MAX_X-DS_SCRN_DEFAULT_SMARGINE-1), (OP_INT16)(LCD_EXT_TEXT_HIGH-1)); 
        /* clear window title & line data */
        ds_clear_title();
        ds_clear_lines();
    }
    else if (ds.scrnMode != SCREEN_FULL_IMAGE)
    {
        util_set_rectangle(&ds.rcWin, 0,0, (OP_INT16)(LCD_MAX_X-1), (OP_INT16)(LCD_EXT_TEXT_HIGH-1)); 
    }
      
    /* initialize window variables */
    ds.focused_line = CURSOR_OFF;
    ds.refresh_func = OP_NULL;

    return prev_scrn;
}


/*==================================================================================================
    FUNCTION: ds_get_max_lines 

    DESCRIPTION:
        Returns the maximum number of lines according to the current screen mode 
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If it refreshes partial screen, returns FALSE. 
        If whole screen refrshment is required, returns TRUE so that the screen is refreshed at 
        once.
==================================================================================================*/
OP_UINT8 ds_get_max_lines 
( 
    void
)
{
    return(ds.max_lines);
}

/*==================================================================================================
    FUNCTION: ds_get_line_height 

    DESCRIPTION:
        Returns the height of a line. 
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
OP_UINT8 ds_get_line_height 
( 
    void
)
{
    return(LINE_HEIGHT);
}


/*==================================================================================================
    FUNCTION: ds_get_line_y 

    DESCRIPTION:
        Returns the given line y coordinates. 
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_LINE_Y_COOR_T ds_get_line_y 
( 
    OP_UINT8 line
)
{
    return(ds.line[line].y);
}


/*==================================================================================================
    FUNCTION: ds_get_line_text_x 

    DESCRIPTION:
        Returns the given line x coordinate to write text in the line.
        It is desided to attribute of the line.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
OP_INT16 ds_get_line_text_x 
( 
    OP_UINT8 line
)
{
    
    return(get_line_text_x_pos(line));
}

/*==================================================================================================
    FUNCTION: ds_get_font_bg 

    DESCRIPTION:
        Returns the background color of the font. The font bg color is used in ds_draw_text.
        it depends on the screen mode. 
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
OP_UINT16 ds_get_font_bg 
( 
    void
)
{
    
    DS_COLOR    font_bg;

    switch(ds.scrnMode)
    {
        case SCREEN_MENU:
        case SCREEN_LIST:
        case SCREEN_FUNC:
        case SCREEN_GENERAL:
        case SCREEN_MAINMENU:
        case SCREEN_PLAIN_TEXT:
        case SCREEN_FULL_IMAGE: /* do not write any text with this mode. It is only for image */
           font_bg = COLOR_FONTBG;
           break;

        case SCREEN_GENERAL_BG:
        case SCREEN_FUNC_BG:
        case SCREEN_IDLE:
        case SCREEN_OWNERDRAW:
        default :
            font_bg = COLOR_TRANS;
            break;
    }
    return font_bg;
}

/*==================================================================================================
    FUNCTION: ds_save_region 

    DESCRIPTION:
        Copy a specified region of screen to the given buffer.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        It is very important that the caller should pass the enough sized buffer to hold the specified 
        region.
==================================================================================================*/
void ds_save_region 
( 
    OP_UINT16    *dest_buf,
    OP_INT16     left,
    OP_INT16     top,
    OP_INT16     right,
    OP_INT16     bottom
)
{
    copy_lcd_region(dest_buf, left, top, right, bottom);
}

/*==================================================================================================
    FUNCTION: ds_set_screen_side_margine 

    DESCRIPTION:
        Set screen side margine. It only applied to one of SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC, and
        SCREEN_FUNC_BG.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_set_screen_side_margines 
( 
    OP_INT16 left_margine,
    OP_INT16 right_margine
)
{
    
    switch(ds.scrnMode)
    {
        case SCREEN_MENU:
        case SCREEN_LIST:
        case SCREEN_FUNC:
        case SCREEN_FUNC_BG:
            ds.rcWin.left = left_margine;
            ds.rcWin.right = LCD_MAX_X -1 - right_margine;
            break;
        default :
            break;
    }
}


/*==================================================================================================
    FUNCTION: ds_get_color_theme 

    DESCRIPTION:
        Return screen color theme - called by display application.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_COLORTHEME_ENUM_T ds_get_color_theme
(
    void
)
{
    if((ds.cComposer.color_theme >= COLOR_THEME_1)
      &&(ds.cComposer.color_theme <= COLOR_THEME_3))
    {
        return ds.cComposer.color_theme;
    }
    else
    {
        return COLOR_THEME_1;
    }
}

/*==================================================================================================
    FUNCTION: ds_set_color_theme 

    DESCRIPTION:
        Set screen color theme - called by display application.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_set_color_theme
(
    DS_COLORTHEME_ENUM_T    color_theme
)
{
    /* set default color to title, annun, focusbar or softkey bar */
    ds.cComposer.titleColor = COLOR_WHITE;
    ds.cComposer.annunColor = COLOR_WHITE;
    ds.cComposer.focusbarColor = COLOR_WHITE;
    ds.cComposer.softkeyColor = COLOR_WHITE;
    /* only color theme is valid */
    ds.cComposer.color_theme = color_theme;
    ds.cComposer.titleFontColor = COLOR_FRENCH_BLUE;
}


/*==================================================================================================
    FUNCTION: ds_draw_focus_bar 

    DESCRIPTION:
        Draw foucs bar according to the current color theme.
        Focus bar height can be assigned.
        
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_draw_focus_bar
(
    OP_INT16                y_top,
    OP_INT16                y_bottom
)
{
    OP_BOOLEAN              draw_focus_bar = OP_FALSE;
    DS_COLOR                border_color;
    DS_COLOR                fill_color;
    DS_COLORTHEME_ENUM_T    color_theme = ds.cComposer.color_theme;
    OP_UINT8                uiRightX = LCD_MAX_X_COOR;
    SCROLLBAR_INFO          *pScrollbar = OP_NULL;

    switch(color_theme)
    {
        case COLOR_THEME_1:
            border_color = FOCUS_BORDER_COLOR_1;
            fill_color = FOCUS_COLOR_1;
            draw_focus_bar = OP_TRUE;
            break;
        case COLOR_THEME_2:
            border_color = FOCUS_BORDER_COLOR_2;
            fill_color = FOCUS_COLOR_2;
            draw_focus_bar = OP_TRUE;
            break;
        case COLOR_THEME_3:
            border_color = FOCUS_BORDER_COLOR_3;
            fill_color = FOCUS_COLOR_3;
            draw_focus_bar = OP_TRUE;
            break;
        case COLOR_THEME_USER_DEFINE:
            border_color = ds.cComposer.focusbarColor;
            fill_color = ds.cComposer.focusbarColor;
            draw_focus_bar = OP_TRUE;
            break;
        default :
            break;
    }

    /*Get scrollbar*/
    pScrollbar = get_scrollbar();
    if (draw_focus_bar)
    {
        if(pScrollbar->IsEnableScrollbar)
        {
            uiRightX = LCD_MAX_X_COOR - get_scrollbar_width();
        }
        /* donot draw border color */
        //fill_rect(AREA_TEXT, 0, y_top, uiRightX, y_top, border_color);
        fill_rect(AREA_TEXT, 0, (OP_INT16)(y_top), uiRightX, (OP_INT16)(y_bottom), fill_color);
        //fill_rect(AREA_TEXT,uiRightX,(OP_INT16)(y_top+1),uiRightX,y_bottom, border_color);
        //fill_rect(AREA_TEXT, 0, y_bottom, uiRightX, y_bottom, border_color);
    }  
}

/*==================================================================================================
    FUNCTION: ds_get_focus_bar_color
   
    DESCRIPTION:
        This just returns the fill_color of ds_draw_focus_bar() function.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_COLOR ds_get_focus_bar_color
(
    void
)
{
    DS_COLOR ret;
    switch( ds.cComposer.color_theme )
    {
        case COLOR_THEME_1:
            ret = FOCUS_COLOR_1;
            break;
        case COLOR_THEME_2:
            ret = FOCUS_COLOR_2;
            break;
        case COLOR_THEME_3:
            ret = FOCUS_COLOR_3;
            break;
        case COLOR_THEME_USER_DEFINE:
            ret = ds.cComposer.focusbarColor;
            break;
            
        default :
            ret = FOCUS_COLOR_1;
            break;
    }
    return ret;
} 

/*==================================================================================================
    FUNCTION: ds_pass_event_to_ds 

    DESCRIPTION:
        This function is called to pass DS specified event to DS.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_pass_event_to_ds
( 
    OPUS_EVENT_ENUM_TYPE   event, 
    void                   *pMess, 
    OP_BOOLEAN             *handle 
)
{
    *handle = OP_TRUE;

    switch(event)
    {
        case OPUS_TIMER_BLINK_CURSOR:
            {
                /* toggle the blinking status */
                ds.cCursor.blink_status = !ds.cCursor.blink_status;
                refresh_cursor();
                OPUS_Start_Timer(OPUS_TIMER_BLINK_CURSOR, 1000, 0,  PERIODIC);
            }
            break;
        case OPUS_TIMER_BLINK_ANNUN:
            if (ds.blink_annun)
            {
                /* toggle the blinking status */
                ds.blink_annun_status = !ds.blink_annun_status;

                if ((ds.annun_blink_mask & AMASK_BATT)&&(ds.disp_annun))
                {
                    if(hmi_get_flip_status() == FLIP_CLOSED&&(SUBLCD_DISPLAY_ANIMATION_MODE != SP_get_sublcd_screen_mode()))
                    {
                        draw_openwindow_annun((DS_ANNUN_ENUM_T)(ANNUN_BATT0+ds.batt_level), ds.blink_annun_status);
                    }
                    else
                    {
                        draw_annunciator((DS_ANNUN_ENUM_T)(ANNUN_BATT0+ds.batt_level), ds.blink_annun_status);                       
                    }
                }
                OPUS_Start_Timer(OPUS_TIMER_BLINK_ANNUN, 1000, 0,  PERIODIC);
                refresh_annuciator();                
            }
            break;
        default :
           *handle = OP_FALSE;
            break;

    }

}

/*==================================================================================================
    FUNCTION: ds_set_lcd_contrast 

    DESCRIPTION:
        Set lcd contrast.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_set_lcd_contrast
(
    OP_UINT8    contrast_index
)
{
#ifndef WIN32
     lcd_contrast_set(contrast_index);
#endif
}


/*==================================================================================================
    FUNCTION: ds_draw_clock_hand

    DESCRIPTION:
        Draw Analog clock hand.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_draw_analog_clock
( 
    OP_INT16              x_center,
    OP_INT16              y_center,
    OP_INT16              h_len,
    OP_INT16              m_len,
    DS_COLOR              h_color,
    DS_COLOR              s_color,
    OP_UINT8              hour,  
    OP_UINT8              minute  
)
{
    draw_analog_clock(x_center, y_center, h_len, m_len, h_color, s_color, hour, minute);
}


/*==================================================================================================
    FUNCTION: ds_pause_current_animation

    DESCRIPTION:
        Pause the current animation.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_pause_current_animation
( 
    void
)
{
 /*   if ( (ds.scrnMode == SCREEN_IDLE) || (ds.scrnMode == SCREEN_MAINMENU))*/
    {
        if (ds.ani.status == ANI_ACTIVE)
        {   
            ds_pause_animation(&ds.ani);
        }
    }
    widgetMainMenuPlayAnimation(OP_FALSE);
}


/*==================================================================================================
    FUNCTION: ds_resume_paused_animation

    DESCRIPTION:
        Resume the paused animation.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_resume_paused_animation
( 
    void
)
{
    if (ds.ani.status == ANI_PAUSED)
    {   
        ds_resume_animation(&ds.ani);
    }
    widgetMainMenuPlayAnimation(OP_TRUE);

}
/*==================================================================================================
    FUNCTION: ds_get_refresh_rect

    DESCRIPTION:
        get current refresh rect.

    ARGUMENTS PASSED:

    RETURN VALUE:
        DS_RECT_T : refresh rect.
    IMPORTANT NOTES:
==================================================================================================*/
/* Conexant Simulator test phone */
DS_RECT_T ds_get_refresh_rect(void)
{
    return(ds.rcUpdate);
}
/*==================================================================================================
    FUNCTION: ds_get_image_header_size

    DESCRIPTION:
        get image header size.

    ARGUMENTS PASSED:
        RM_IMAGE_FORMAT format :
    RETURN VALUE:
        DS_RECT_T : refresh rect.
    IMPORTANT NOTES:
==================================================================================================*/
OP_UINT16 ds_get_image_header_size
(
    RM_IMAGE_FORMAT format
)
{
    RM_IMAGE_FORMAT image_format;

    switch(format)
    {  
        case IMAGE_BMP:
            image_format = IMAGE_BMP_HEADERSIZE;
            break;
        case IMAGE_PICTOGRAM:
            image_format = IMAGE_PICTOGRAM_HEADERSIZE;
            break;
        case IMAGE_GIF:
            image_format = IMAGE_GIF_HEADERSIZE;
            break;
        case IMAGE_JPEG:
            image_format = IMAGE_JPEG_HEADERSIZE;
            break;
        case IMAGE_PNG:
            image_format = IMAGE_PNG_HEADERSIZE;
            break;
        case IMAGE_WBMP:
            image_format = IMAGE_WBMP_HEADERSIZE;
            break;
        default:
            image_format = 0;
            break;
    }
    return image_format;
}

/*==================================================================================================
    FUNCTION: ds_get_image_header_size

    DESCRIPTION:
        get image header size.

    ARGUMENTS PASSED:
        RM_IMAGE_FORMAT format: image format.
        OP_UINT8         *image: the original data of image. 
        OP_UINT16        *pWidth: return pointer of image width.
        OP_UINT16        *pHeight: return pointer of image height.
    RETURN VALUE:
        return true when success.
    IMPORTANT NOTES:
==================================================================================================*/
OP_BOOLEAN ds_get_file_image_size
(
    RM_IMAGE_FORMAT format,
    OP_UINT8         *image,
    OP_UINT32        data_size,
    OP_UINT16        *pWidth,
    OP_UINT16        *pHeight
)
{
    OP_UINT8        *pData;
    OP_BOOLEAN      bRet= OP_FALSE;

    switch(format)
    {  
        case IMAGE_BMP:
            {
                OP_UINT32 tmp;
                pData = image + BMP_FILE_HEADER_SIZE + OP_UINT32_SIZE;
                op_memcpy(&tmp, pData, OP_UINT32_SIZE);
                if(tmp <= 0xffff)
                {
                    *pWidth = (OP_UINT16)tmp;
                }
                else
                {
                    *pWidth = 0;
                }
                pData += OP_UINT32_SIZE;
                op_memcpy(&tmp, pData, OP_UINT32_SIZE);
                if(tmp <= 0xffff)
                {
                    *pHeight = (OP_UINT16)tmp;
                }
                else
                {
                    *pHeight = 0;
                }
                if(*pWidth && *pHeight)
                {
                    bRet = OP_TRUE;
                }
            }
            break;
        case IMAGE_GIF:
            {
                pData = image + 6;
                op_memcpy(pWidth, pData, 2);
                pData += 2;
                op_memcpy(pHeight, pData, 2);
                bRet = OP_TRUE;
            }
            break;
        case IMAGE_JPEG:
        case IMAGE_PHOTO:
           bRet = GetJpgImage_Width_Height(image,data_size, pWidth, pHeight);
            break;
        case IMAGE_WBMP:
            {
                *pWidth = 0;
                pData = image + 2;
	            while (*pData & 0x80){
	                *pWidth <<= 7;
	                *pWidth += *pData & 0x7f;
	                pData +=1;
	                }; 
	                *pWidth <<= 7;
	                *pWidth += *pData & 0x7f;

	            pData += 1;
                *pHeight = 0;
	            while (*pData & 0x80){
	                *pHeight <<= 7;
	                *pHeight += *pData & 0x7f;
	                pData +=1;
	                }; 
	                *pHeight <<= 7;
	                *pHeight += *pData & 0x7f;

                bRet = OP_TRUE;
            }
            break;
        case IMAGE_PNG:
        case IMAGE_PICTOGRAM:
        default:
            {
                *pWidth = 0;
                *pHeight = 0;
            }
            break;
    }
    return  bRet;
}

/*==================================================================================================
    FUNCTION: ds_parse_image

    DESCRIPTION:
        Malloc pBitmap->data before call these function,.

    ARGUMENTS PASSED:
        RM_BITMAP_T      *pBitmap:  pointer of bitmap structure, need by draw bitmap function.
        OP_UINT8          *pSrcData: the original data of resource from pipe or file.
        RM_IMAGE_FORMAT   format:   the format of ds image.
        
    RETURN VALUE:
        return true when success. 
    IMPORTANT NOTES:
==================================================================================================*/
OP_BOOLEAN ds_parse_image
(
    RM_BITMAP_T      *pBitmap, 
    OP_UINT8          *pSrcData,
    OP_UINT32         datasize,
    RM_IMAGE_FORMAT   format
)
{
    switch(format)
    {  
        case IMAGE_BMP:
            if(ds_parse_bitmap_image(pBitmap, pSrcData) == OP_FALSE)
                {
                return OP_FALSE;
                }
            break;
        case IMAGE_GIF:
            ds_parse_gif_image(pBitmap, pSrcData);
            break;
        case IMAGE_JPEG:
            if (ds_parse_jpg_image(pBitmap, pSrcData,datasize) == OP_FALSE)
            {
                return OP_FALSE;
            }
            break;
        case IMAGE_WBMP:
            ds_parse_wbmp_image(pBitmap, pSrcData);
            break;
        case IMAGE_PNG: /* not support */
            return  OP_FALSE;
            break;
        case IMAGE_PICTOGRAM:/* not support */
            return  OP_FALSE;
            break;
        default:
            return  OP_FALSE;
            break;
     }

    return OP_TRUE;
}

/*==================================================================================================
    FUNCTION: ds_parse_bitmap_image

    DESCRIPTION:
        Malloc pBitmap->data before call these function,.

    ARGUMENTS PASSED:
        RM_BITMAP_T      *pBitmap:  pointer of bitmap structure, need by draw bitmap function.
        OP_UINT8          *pSrcData: the original data of resource from pipe or file.
        
    RETURN VALUE:
        return true when success.   
    IMPORTANT NOTES:
==================================================================================================*/
static OP_BOOLEAN ds_parse_bitmap_image
(
    RM_BITMAP_T *pBitmap, 
    OP_UINT8    *pSrcData  
)
{
    /* analyse the bitmap header info */
    DS_BITMAPFILEHEADER    BmpFileHeader;
    DS_BITMAPINFOHEADER   BmpInfoHeader;
    OP_UINT16              nPitch;
    
    OP_UINT16              iTmp; /* represents the really bytes in one line should be copy into dest buffer */
    OP_UINT16              iCount = 0;
    OP_UINT8*              pData;
    OP_UINT32              bfMask[3];


    bfMask[0] = 0;
    bfMask[1] = 0;
    bfMask[2] = 0;
    if(GetBitmapFileHeader(&BmpFileHeader, &BmpInfoHeader, pSrcData) == OP_FALSE)
    {
        return OP_FALSE;
    }

    /* fill the bitmap header: bitcount, width, height */
    pBitmap->biWidth = (OP_UINT16)BmpInfoHeader.biWidth;
    pBitmap->biHeight = (OP_UINT16)BmpInfoHeader.biHeight;
    pBitmap->biBitCount = (OP_UINT8)BmpInfoHeader.biBitCount;
    /* pBitmap->data point to real bitmap data */
    
    /* copy color pallet to pBitmap->ColorPallet if exist */
    if((IMAGE_1_BIT == BmpInfoHeader.biBitCount) || (IMAGE_4_BIT == BmpInfoHeader.biBitCount) || (IMAGE_8_BIT == BmpInfoHeader.biBitCount))
    {  
        OP_UINT32  cpsize;
        if (BmpInfoHeader.biClrUsed == 0)
        {
            switch(BmpInfoHeader.biBitCount) {
            case IMAGE_1_BIT:
                cpsize = 2;
            	break;
            case IMAGE_4_BIT:
                cpsize = 16;
                break;
            case IMAGE_8_BIT:
                cpsize = 256;
                break;
            default:
                return OP_FALSE;
            }
        }
        else
        {
        cpsize = BmpInfoHeader.biClrUsed;
        }
        /* if with bmp color plane, put it into pBitmap->ColorPallet, */
        pData = pSrcData + BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE;
        pBitmap->ColorPallet = (DS_RGBQUAD*)op_alloc(cpsize *sizeof(DS_RGBQUAD));
        if(pBitmap->ColorPallet == OP_NULL)
        {
            op_debug(DEBUG_LOW,"%s\n","(pBitmap->ColorPallet == OP_NULL)\n");
            return OP_FALSE;
        }
        op_memcpy(pBitmap->ColorPallet, pData, cpsize*sizeof(DS_RGBQUAD));
    }
    else if(DS_BI_BITFIELDS == BmpInfoHeader.biCompression)
    {
        pData = pSrcData + BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE;
        op_memcpy(bfMask,pData,4);
        op_memcpy(bfMask+1,pData + 4,4);
        op_memcpy(bfMask+2,pData + 8,4);
    }
    else if(DS_BI_RGB == BmpInfoHeader.biCompression)
        {
        	bfMask[0]=0x7C00; bfMask[1]=0x3E0; bfMask[2]=0x1F; //RGB555
        }
    else
    {
        pBitmap->ColorPallet = OP_NULL;
    }

    /* caculate how many bytes in one line(bmp_width) */
    /* caculate the real width should be copied into dest buffer */
    if (IMAGE_1_BIT == BmpInfoHeader.biBitCount )
    {
        nPitch = (BmpInfoHeader.biWidth + 31) >> 5 << 2;
    }
    else
    {
        nPitch = (BmpFileHeader.bfSize - BmpFileHeader.bfOffBits)/BmpInfoHeader.biHeight;
    }

    if ( (BmpInfoHeader.biBitCount * BmpInfoHeader.biWidth) % BYTELENGTH )
    {
        iTmp = (BmpInfoHeader.biBitCount * BmpInfoHeader.biWidth)/BYTELENGTH + 1;
    }
    else 
    {
        iTmp = (BmpInfoHeader.biBitCount * BmpInfoHeader.biWidth)/BYTELENGTH;
    }
    pData = (OP_UINT8* )(pSrcData + BmpFileHeader.bfOffBits);
    ConvertBmp(pBitmap,
                pData,
                &BmpInfoHeader,  /* bmp info */
                nPitch,                 /* width */
                (OP_UINT16)(nPitch-iTmp), /* src x end */
                bfMask
                );
    if(pBitmap->ColorPallet != OP_NULL)
    {
       op_free(pBitmap->ColorPallet);
    }
    pBitmap->biBitCount = IMAGE_16_BIT;
    return OP_TRUE;
}

/*==================================================================================================
    FUNCTION: ds_parse_gif_image

    DESCRIPTION:
        parse gif image.

    ARGUMENTS PASSED:
        RM_BITMAP_T      *pBitmap:  pointer of bitmap structure, need by draw bitmap function.
        OP_UINT8          *pSrcData: the original data of resource from pipe or file.
        
    RETURN VALUE:
    IMPORTANT NOTES:
==================================================================================================*/
void ds_parse_gif_image
(
    RM_BITMAP_T *pBitmap, 
    OP_UINT8    *pSrcData  
)
{
//    GIF_LoadData( pSrcData, pBitmap);
    pBitmap->biBitCount = IMAGE_8_BIT;
}
/*==================================================================================================
    FUNCTION: ds_parse_jpg_image

    DESCRIPTION:
        parse jpg image.

    ARGUMENTS PASSED:
        RM_BITMAP_T      *pBitmap:  pointer of bitmap structure, need by draw bitmap function.
        OP_UINT8          *pSrcData: the original data of resource from pipe or file.
        
    RETURN VALUE:
    IMPORTANT NOTES:
==================================================================================================*/
OP_BOOLEAN ds_parse_jpg_image
(
    RM_BITMAP_T *pBitmap, 
    OP_UINT8    *pSrcData,
    OP_UINT32   data_size
)
{
    if(LoadJPGData(pSrcData,data_size, pBitmap) == OP_FALSE)
    {
        return OP_FALSE;
    }
    else
    {
        pBitmap->biBitCount = IMAGE_16_BIT;
        return OP_TRUE;
    }
}

/*==================================================================================================
    FUNCTION: ds_parse_wbmp_image

    DESCRIPTION:
        parse wbmp image.

    ARGUMENTS PASSED:
        RM_BITMAP_T      *pBitmap:  pointer of bitmap structure, need by draw bitmap function.
        OP_UINT8          *wbmp: the original data of resource from pipe or file.
        
    RETURN VALUE:
    IMPORTANT NOTES:
==================================================================================================*/
void ds_parse_wbmp_image 
(
    RM_BITMAP_T   *pBitmap,    
    OP_UINT8      *wbmp
)
{
    OP_UINT8     *src;
    OP_UINT16    *dest;
	OP_UINT16    biWidth = 0,biHeight = 0;
    OP_BOOLEAN   bColor = OP_FALSE;

    if((pBitmap == OP_NULL)||(pBitmap->data == OP_NULL)||(wbmp == OP_NULL))
    {
        return;
    }
	dest = pBitmap->data;
    src = wbmp;
	/* add for display yinbiao in POWER WORD  xcy ask linda to add at 2/17*/
    if((*src == 0xFF) && (*(src+1) == 0xFF))
	{
	    bColor = OP_TRUE;
	}
	src = wbmp + 2;     //skip firt two byte as TypeField and FixHeaderField
	
	while (*src & 0x80){
	    biWidth <<= 7;
	    biWidth += *src & 0x7f;
	    src +=1;
	    }; 
	    biWidth <<= 7;
	    biWidth += *src & 0x7f;

	pBitmap->biWidth = biWidth;
	src += 1;
	while (*src & 0x80){
	    biHeight <<= 7;
	    biHeight += *src & 0x7f;
	    src +=1;
	    }; 
	    biHeight <<= 7;
	    biHeight += *src & 0x7f;

	pBitmap->biHeight = biHeight;
    src +=1;
    pBitmap->biBitCount = IMAGE_16_BIT;

    convert_1bit_image_to_bmp(dest, src, pBitmap->biWidth, pBitmap->biHeight, bColor);
}

/*==================================================================================================
    FUNCTION: ds_draw_wbmp_image

    DESCRIPTION:
        draw wbmp image without parse-data buffer .

    ARGUMENTS PASSED:
        SCRNAREA_ENUM_T  area,
        OP_INT16           dsx,       
        OP_INT16           dsy,        
        OP_INT16           d_width, 
        OP_INT16           d_height,  
        OP_INT16           ssx,      
        OP_INT16           ssy,       
        OP_UINT8          *src_data
        
    RETURN VALUE:
    IMPORTANT NOTES:
==================================================================================================*/
void ds_draw_wbmp_image
(
    SCRNAREA_ENUM_T  area,
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    OP_UINT8          *src_data
)
{
    OP_UINT16   *pDest, *d_ptr;
    OP_UINT8    *pSrc;
    OP_INT16    i; 
    OP_INT16    w,h;
    OP_INT16    left, right, top, bottom, dx, dy;
    OP_UINT16   src_width, src_height;
    OP_INT16    linewidth, nPitch, Index;
    DS_COLOR    color = 0;          /*xcy ask linda to add at 2/17*/

    if ((src_data == OP_NULL) || ( area != AREA_TEXT ))
    {
        return;
    }

    pDest = (OP_UINT16 *)txt_screen;
    pSrc = src_data;
    
    left  = MAX(dsx, 0);
    top  = MAX(dsy, 0);
    ssx = MAX(ssx, 0);
    ssy = MAX(ssy, 0);
    
    right = MIN(dsx+d_width, LCD_MAX_X);
    if (ds_get_disp_annun())
    {
        bottom = MIN(dsy+d_height, LCD_TEXT_HIGH);
    }
    else
    {
        bottom = MIN(dsy+d_height, LCD_EXT_TEXT_HIGH);
    }
	/* add for display yinbiao in POWER WORD  xcy ask linda to add at 2/17*/
    if((*pSrc == 0xFF) && (*(pSrc+1) == 0xFF))
	{
	    color = COLOR_YELLOW;
	}

	/* skip the header */
    pSrc += 2;
	while (*pSrc & 0x80){
	    src_width <<= 7;
	    src_width += *pSrc & 0x7f;
	    pSrc +=1;
	    }; 
	    src_width <<= 7;
	    src_width += *pSrc & 0x7f;

	pSrc += 1;
	while (*pSrc & 0x80){
	    src_height <<= 7;
	    src_height += *pSrc & 0x7f;
	    pSrc +=1;
	    }; 
	    src_height <<= 7;
	    src_height += *pSrc & 0x7f;

    pSrc +=1;
    
    linewidth  = (src_width + 7) / 8;
	nPitch = 0;

	for (h=0, dy=top; (h<src_height) &&(dy<bottom) ; h++,dy++)
	{
		nPitch = 0;
        d_ptr = pDest + dy*LCD_MAX_X + dsx;   /* d_ptr points to start_x in line-dy */
        for(w=0, dx=left; (w<linewidth)&&(dx<right); w++,dx+=BYTELENGTH)
		{
			OP_UINT8  mask = 0x80;
 
			Index = (h*linewidth) + w;
			for(i=0; i<BYTELENGTH; i++, mask>>=1)
			{
				nPitch++;
				if(nPitch <= src_width)
				{
					if((pSrc[Index] & mask) == 0x00)
						*d_ptr++ = 0x0000;
					else if(color)
					{
					    *d_ptr++ = color;   /* displayed for power word */
					}
					else
					{
						*d_ptr++ = 0xffff;
					}
				}
			}
		}
	}

}

static void convert_1bit_image_to_bmp
(
    OP_UINT16   *dest, 
    OP_UINT8    *src, 
    OP_UINT16   width, 
    OP_UINT16   height,
    OP_BOOLEAN  bColor
)
{
    int           linewidth, ni;
    int           nPitch;
    int           Index = 0;
    int           yi, i;

    linewidth=(width + 7) / 8;
    nPitch = 0;
    for (yi=0; yi<height ; yi++)
    {
        nPitch = 0;
        for(ni=0; ni<linewidth; ni++)
        {
            OP_UINT8  mask = 0x80;

            Index = (yi*linewidth) +ni;

            for(i=0; i<8; i++, mask>>=1)
            {
                nPitch++;
                if(nPitch <= width)
                {
                    if((src[Index] & mask) == 0x00)
                    {
                        *dest++ = 0x0000;
                    }
                    else
                    {
                        if(bColor == OP_TRUE) /* color for display yinbiao for powerword */
                        {
                            *dest++ = COLOR_YELLOW;
                        }
                        else
                        {
                            *dest++ = 0xffff;
                        }
                    }
                }
            }
        }
    }
}

OP_BOOLEAN ds_need_stop_anun_blinktimer(void)
{
    return isNeedStopBlinkTimer;
}


/*==================================================================================================
    FUNCTION: ds_refresh_softkey_rectangle 

    DESCRIPTION:
        Only refresh the whole softkey rect. 
       
    ARGUMENTS PASSED:
        none.
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        Now this function is only used for wap&mms module refresh.
==================================================================================================*/
void ds_refresh_softkey_rectangle(void)
{
    OP_UINT8 skey_high;
    
    if(ds_get_extend_softkey() == OP_FALSE)
    {
        skey_high = LCD_SKEY_HIGH;
    }
    else
    {
        skey_high = LCD_EXT_SKEY_HIGH;
    }
    op_memcpy(lcd_screen[LCD_MAX_Y-skey_high], soft_screen[0], LCD_MAX_X*skey_high*sizeof(DS_COLOR)); 
    ds_refresh_lcd_rect( 0, 
                         (OP_UINT8)(LCD_MAX_Y-skey_high), 
                         LCD_MAX_X_COOR, 
                         LCD_MAX_Y_COOR);
}

/*==================================================================================================
    FUNCTION: ds_refresh_text_rectangle 

    DESCRIPTION:
        Only refresh the part of text screen. This text screen must start from (0,0). 
       
    ARGUMENTS PASSED:
        LCD_RECT_T u_rect: update rect.
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        Now this function is only used for wap&mms module refresh, especially for refresh title when 
        receive browser data.
==================================================================================================*/
void ds_refresh_text_rectangle(LCD_RECT_T u_rect)
{
    OP_INT16      y ;
    OP_INT16      cx_bytes ; /* number of x bytes to be refreshed */
    OP_UINT16     offset;

    if((u_rect.left > LCD_MAX_X-1) || (u_rect.top > LCD_MAX_Y-1)
       ||(u_rect.right < 0) || (u_rect.bottom < 0) )
    {
        return;
    }

    if ( u_rect.bottom > LCD_EXT_TEXT_HIGH)
    {
        u_rect.bottom = LCD_EXT_TEXT_HIGH -1;
    }

    offset = (ds.disp_annun) ? LCD_TEXT_FRAME_START : 0;

   cx_bytes = (u_rect.right - u_rect.left + 1) * sizeof(DS_COLOR);
   for (y=u_rect.top; y<=u_rect.bottom; y++) 
    {
        op_memcpy(&lcd_screen[y+offset][u_rect.left], 
                  &txt_screen[y][u_rect.left],
                  cx_bytes);
    }

    /* if the annunciator need to be shown up
      * the updated area must be moved down
    */
    if( ds.disp_annun )
    {
        u_rect.top += offset;
        u_rect.bottom += offset;
    }
    
    ds_refresh_lcd_rect((OP_UINT8)u_rect.left, 
                            (OP_UINT8)(u_rect.top), 
                            (OP_UINT8)(u_rect.right), 
                            (OP_UINT8)(u_rect.bottom));
    

}

/*==================================================================================================
    FUNCTION: ds_refresh_for_wap_mms 

    DESCRIPTION:
        Refresh for wap and mms module. 
       
    ARGUMENTS PASSED:
        none.
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        Now this function is only used for wap&mms module refresh.
        For speeden AU display, based on current structure, we have to provide this function.
==================================================================================================*/
void ds_refresh_for_wap_mms(void)
{
    LCD_RECT_T    u_rect;
    

    if((ds.scrnMode != SCREEN_OWNERDRAW)||(!UPDATEAREA()))
     {
         return;
     }
 
    if((ds.rcUpdate.left != LCD_MAX_X-1) && (ds.rcUpdate.top != LCD_MAX_Y-1)
       &&(ds.rcUpdate.right != 0) && (ds.rcUpdate.bottom != 0) )
    {
        u_rect.left = ds.rcUpdate.left;
        u_rect.top = ds.rcUpdate.top;
        u_rect.right = ds.rcUpdate.right;
        u_rect.bottom = ds.rcUpdate.bottom;
    }
        
    /* refresh either background or popup is updated */
    if ((&u_rect != OP_NULL) || UPDATEAREA())
    {
        /* text Area Update */
        if (ds.update_area & AREAMASK_TEXT)
        {

            ds_refresh_text_rectangle(u_rect);
        }
        /* SOFTKEY Area Update */
        if (ds.update_area & AREAMASK_SKEY) 
        {
            ds_refresh_softkey_rectangle();
        }       
        /* Clear the update region and area mask after refreshment */
        clear_update_region();    
    }
}
/*==================================================================================================
    FUNCTION: ds_refresh_text_area 

    DESCRIPTION:
        Refresh for text region. 
       
    ARGUMENTS PASSED:
        res_buf: the backup text buffer;
        top: the buffer start rows
        bottom:the end rows;
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        this function only refresh text region;
 =================================================================================================*/

void ds_refresh_text_area
(  OP_UINT16    *res_buf,
   OP_INT16     top,
   OP_INT16     bottom
 )
{
    if(ds.scrnMode != SCREEN_PLAIN_TEXT)
     {
         return;
     }
     if(res_buf == OP_NULL)
     {
         return;
     }
     op_memcpy(&txt_screen[top][0], res_buf, LCD_MAX_X*( bottom - top +1)*sizeof(DS_COLOR));

    clear_update_region(); 
}
 
int ds_refresh_lcd_rect
(
    OP_UINT8 x_start, 
    OP_UINT8 y_start, 
    OP_UINT8 x_end, 
    OP_UINT8 y_end
)
{
    /* add this condition for fix problem:
    when cover close, open window display overlap on main lcd;
    it's same to ds_refresh() */
    if((hmi_get_flip_status() == FLIP_CLOSED)
     &&(SP_get_power_on_status() != POWER_ON_STATUS_INIT))
    {
        return -1;
    }
    /* check the input parameter is valid */
    lcd_drv_draw_rectangle ( x_start, 
                             y_start, 
                             x_end, 
                             y_end, 
                             lcd_screen,
                             LCD_MAX_X,
                             0,
                             0); 
    return 1;
}
/*==================================================================================================
    FUNCTION: ds_draw_cursor 

    DESCRIPTION:
        Draw a cursor on the screen( in editor).
        Wrote by ZhangXiaoDong
       
         ARGUMENTS PASSED:
         OP_UINT8     cursor_x: The x position of the start point
         OP_UINT8     cursor_y: The y position of the start point
         OP_UINT8     cursor_hight: Cursor Height
         OP_UINT8     limit_y: The max Y value of cursor point.
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        
 =================================================================================================*/
void ds_draw_cursor
(
 OP_UINT8     cursor_x,
 OP_UINT8     cursor_y,
 OP_UINT8     cursor_hight,
 OP_UINT8     limit_y
 )
{
    OP_UINT8 i =0;
    OP_UINT16 * pBuffer = OP_NULL;
    
    if(cursor_x > LCD_MAX_X-1 || cursor_y + cursor_hight > limit_y-1)
    {
        return;
    }
	if(ds_get_disp_annun())
	{
		cursor_y += LCD_ANNUN_HIGH;
	}
    for(i = cursor_y; i <cursor_y + cursor_hight; i++)
    {
            pBuffer = (OP_UINT16 *)lcd_screen;
            lcd_screen[i][cursor_x] = ~lcd_screen[i][cursor_x];
            lcd_screen[i][cursor_x+1] = ~lcd_screen[i][cursor_x+1];
    }
    if(pBuffer == OP_NULL)
    {
        return;
    }
    
    ds_refresh_lcd_rect (cursor_x,
        cursor_y,
        (OP_UINT8)(cursor_x + 2 - 1),
        (OP_UINT8)(cursor_y + cursor_hight -1)
        );
    
    
}

/*==================================================================================================
    FUNCTION: ds_draw_region 

    DESCRIPTION:
        Draw a cursor on the screen( in editor).
        Wrote by ZhangXiaoDong
       
         ARGUMENTS PASSED:
         OP_UINT8     Left:The x position of the top
         OP_UINT8     Top:The y position of the top
         OP_UINT8     Right:The y position of the bottom
         OP_UINT8     Bottom:The y position of the start bottom
         
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        
 =================================================================================================*/
void ds_set_reverse
(
 OP_UINT16     Left,
 OP_UINT16     Top,
 OP_UINT16     Right,
 OP_UINT16     Bottom
 )
{
    OP_UINT16   i = 0;
    OP_UINT16   j = 0;
    OP_UINT16 * pBuffer = OP_NULL;
    
    if( (Right > (LCD_MAX_X - 1) ) || (Bottom > (LCD_MAX_Y - 1) ) )
    {
        return;
    }
	
    for(i = Top; i <= Bottom; i++)
    {
    	for(j = Left; j <= Right ; j++)
    		{
            	pBuffer = (OP_UINT16 *)lcd_screen;
            	lcd_screen[i][j] = ~lcd_screen[i][j];
    		}
    }
    
    if(pBuffer == OP_NULL)
    {
        return;
    }
    
    ds_refresh_lcd_rect ((OP_UINT8)Left,
        (OP_UINT8)Top,
        (OP_UINT8)Right,
        (OP_UINT8)Bottom
        );
    
    
}
/*==================================================================================================
    FUNCTION: ds_refresh_text_area 

    DESCRIPTION:
        save text buffer. 
       
    ARGUMENTS PASSED:
        dest_buf: the backup text buffer;
        top: the buffer start rows
        bottom:the end rows;
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        this function only backup text buffer;
 =================================================================================================*/

void ds_save_text_region
( 
 OP_UINT16    *dest_buf,
 OP_INT16     top,
 OP_INT16     bottom
 )
{
    OP_UINT16*  pDest = dest_buf;
    
    if(ds.scrnMode != SCREEN_PLAIN_TEXT)
    {
        return;
    }
    if(top <0 ||
        bottom >LCD_EXT_TEXT_HIGH ||
        top > bottom)        
    {
        return;
    }
   
    op_memcpy(pDest, &txt_screen[top][0], LCD_MAX_X *(bottom - top +1)*sizeof(DS_COLOR)); 
}
/*==================================================================================================
    FUNCTION: ds_update_text_region 
            
    DESCRIPTION:
        update text buffer to lcd buffer. 
  
    ARGUMENTS PASSED:
        top: the buffer start rows
        bottom:the end rows;
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        
 =================================================================================================*/
void ds_update_text_region
(
 OP_INT16     top,
 OP_INT16     bottom
 )
{
    if(ds.scrnMode != SCREEN_PLAIN_TEXT)
    {
        return;
    }
    if(top <0 ||
        bottom >LCD_EXT_TEXT_HIGH ||
        top > bottom)        
    {
        return;
    }
    /*
     * if the the annun  exist
     */
    if(ds_get_disp_annun())
    {
        op_memcpy(&lcd_screen[top+ LCD_ANNUN_HIGH][0], &txt_screen[top][0], LCD_MAX_X *(bottom - top +1)*sizeof(DS_COLOR)); 

        ds_refresh_lcd_rect(0,
            (OP_UINT8)(top+LCD_ANNUN_HIGH),
            (OP_UINT8)(LCD_MAX_X -1),
            (OP_UINT8)(bottom+LCD_ANNUN_HIGH));  
 
    }
    else
    {
    op_memcpy(&lcd_screen[top][0], &txt_screen[top][0], LCD_MAX_X *(bottom - top +1)*sizeof(DS_COLOR)); 

    ds_refresh_lcd_rect((OP_UINT8)0,
        (OP_UINT8)top,
        (OP_UINT8)(LCD_MAX_X -1),
        (OP_UINT8)bottom);
    }
}

/*==================================================================================================
    FUNCTION: ds_get_composed_color
   
    DESCRIPTION:
			get composed color structure.        
    ARGUMENTS PASSED:
        DS_COLOREDCOMPOSER_T  *pCompose : ds compose color structure.
 RETURN VALUE:
        OP_TRUE: get success.
        OP_FALSE: the param is invalid.
        
    IMPORTANT NOTES:
==================================================================================================*/
OP_BOOLEAN ds_get_composed_color
(
    DS_COLOREDCOMPOSER_T  *pCompose 
)
{
    if( pCompose == OP_NULL)
    {
        return OP_FALSE;
    }
    
    op_memcpy(pCompose, &ds.cComposer, sizeof(DS_COLOREDCOMPOSER_T));
    return OP_TRUE;
    
}
/*==================================================================================================
    FUNCTION: ds_put_jpeg_image_on_lcd_directly
   
    DESCRIPTION:
		draw jpg image on lcd directly without any temp buffer .        
    ARGUMENTS PASSED:
        DS_COLOREDCOMPOSER_T  *pCompose : ds compose color structure.
 RETURN VALUE:
        OP_TRUE: get success.
        OP_FALSE: the param is invalid.
        
    IMPORTANT NOTES:
==================================================================================================*/
OP_INT8 ds_put_jpeg_image_on_lcd_directly
(
    OP_INT16 x,
    OP_INT16 y,
    OP_UINT8 *pData,
    OP_UINT32 filesize,
    IMAGE_RESIZE zoom
)
{
    OP_INT8 iRet = 1;
    IMAGE_DISPLAY image;
    
    if(pData == OP_NULL)
    {
        return -1;
    }

    image.x_start = (OP_UINT8)x;
    image.y_start = (OP_UINT8)y;
    image.jpegdata = (OP_UINT16 *)pData;
    image.filesize = filesize;
    image.imageresize = zoom;

    if(iRet > 0)
    {
        mmi_CLC346S_HOLD(OP_FALSE);
        if(Corelogic_ImageDisplay(&image) == OP_FALSE)
        {
            iRet = -1;
        }
        mmi_CLC346S_HOLD(OP_TRUE);
    }
    
    return iRet;
}

/*==================================================================================================
    FUNCTION: ds_decode_jpg_by_hardware
   
    DESCRIPTION:
		decode jpg by hardware.        
    ARGUMENTS PASSED:
        DS_COLOREDCOMPOSER_T  *pCompose : ds compose color structure.
 RETURN VALUE:
        OP_TRUE: get success.
        OP_FALSE: the param is invalid.
        
    IMPORTANT NOTES:
==================================================================================================*/
OP_INT8 ds_decode_jpg_by_hardware
(
    OP_UINT8 *pData,
    OP_UINT32 filesize,
    RM_BITMAP_T *dest
)
{
    OP_INT8 iRet = 1;
    if((pData == OP_NULL)||
       (dest == OP_NULL)||(dest->data == OP_NULL))
    {
        return -1;
    }
#ifdef WIN32
    if(ds_parse_jpg_image(dest, pData,filesize) == OP_FALSE)
    {
        iRet = -1;
    }
#else
    mmi_CLC346S_HOLD(OP_FALSE);
    if(DecodeJPGData((OP_UINT16 *)pData, filesize, dest) == OP_FALSE)
    {
        iRet = -1;
    }
    mmi_CLC346S_HOLD(OP_TRUE);
#endif
    
    return iRet;
}

static OP_BOOLEAN draw_jpg_image
(
    OP_INT16 x,
    OP_INT16 y,
    RM_IMAGE_T *pImage
)
{
    OP_BOOLEAN bRet = OP_FALSE;
    /* use lcdc parser the power on/off animation */
    if(pImage != OP_NULL)
    {
#ifdef WIN32
      { 
          RM_BITMAP_T destBmp;
          op_memset(&destBmp, 0, sizeof(RM_BITMAP_T));
          destBmp.data = bitmap_buffer;
          bRet = ds_parse_image((RM_BITMAP_T *)&destBmp, (OP_UINT8 *)(pImage->data), pImage->size ,IMAGE_JPEG);
          UPDATESET(AREA_BITMAP);
          ds_set_update_region(0,0,(DS_SCRN_MAX_X-1), (DS_SCRN_MAX_Y-1));
          ds_refresh();
      }
#else
      {
          IMAGE_DISPLAY image;
          
          image.x_start = (OP_UINT8)x;
          image.y_start = (OP_UINT8)y;
          image.jpegdata = (OP_UINT16 *)pImage->data;
          image.filesize = pImage->size;
          image.imageresize = LEVEL_ONE;

          mmi_CLC346S_HOLD(OP_FALSE);
          bRet = Corelogic_ImageDisplay(&image);
          mmi_CLC346S_HOLD(OP_TRUE);
      }
#endif
    }
    return bRet;
}
/*==================================================================================================
FUNCTION: ds_set_extend_softkey
   
DESCRIPTION:
		set extend softkey area.        
ARGUMENTS PASSED:
    
RETURN VALUE:
        
IMPORTANT NOTES:
==================================================================================================*/
void ds_set_extend_softkey
(
    OP_BOOLEAN  bExtend
)
{
    bExtendSoftkey = bExtend;
}

/*==================================================================================================
FUNCTION: ds_get_extend_softkey
   
DESCRIPTION:
			get if display extend softkey area.        
ARGUMENTS PASSED:
RETURN VALUE:
        OP_TRUE: use the extend softkey area.
        OP_FALSE: without use extend softkey area.
        
IMPORTANT NOTES:
==================================================================================================*/
OP_BOOLEAN ds_get_extend_softkey
(
    void
)
{
    return bExtendSoftkey;
}
/*================================================================================================*/

#ifdef __cplusplus
}
#endif

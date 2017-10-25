#ifndef DS_LCD_H
#define DS_LCD_H
/*==================================================================================================

    HEADER NAME : ds_lcd.h

    GENERAL DESCRIPTION
        This file contains lcd refresh functions and required definition,

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial Creation
    02/24/2002   wangwei          PP00117     Change draw bitmap to draw icon when play animation.
                                              - add animal_screen[][]
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    05/07/2003   lindawang        P000222     - modify 3 popup window to 1.
    05/15/2003   linda wang       P000289      change the purl blue color.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    07/02/2003   lindawang        P000763     Fix draw bitmap file image problems.
                                              - set original blue color, same with EMS.
    07/26/2003   lindawang        P001074     remove animal_buffer.
    01/07/2003   Steven           P003225     adjust for dolphin          
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/18/2004   Chenyong        P002677       Modify dialpad and CC
    03/18/2004   Chenyong        P002677       Modify dialpad and CC

    03/24/2004   chouwangyun   P002734     change resource and make new mainmenu back button and welcome style
    03/30/2004   Steven Lai        P002795     Fix the bug that the softkey area will disappear if the idle image is animation
    04/20/2004   chouwangyun   c005037      change openwindow_annue
	04/16/2004   caoyanqing     c003008    set background color.
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    07/08/2004   liren          c006711    change submenu for ODIN
    07/12/2004   liren          p006927    add openwindow animation
    07/19/2004   liren          p007060    adjust openwindow animation

 This file contains lcd refresh functions and required definition,


====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "OPUS_typedef.h"
#include    "product.h"
/*==================================================================================================
    CONSTANTS
==================================================================================================*/
/* LCD Specific Constant */

#define LCD_MAX_X                      176
#define LCD_MAX_Y                      220

#define LCD_MAX_X_COOR                175
#define LCD_MAX_Y_COOR                219

#define LCD_ANNUN_HIGH                 20
#define LCD_SKEY_HIGH                  24
#define LCD_EXT_SKEY_HIGH                  24

#define LCD_TEXT_HIGH                  (LCD_MAX_Y-LCD_ANNUN_HIGH-LCD_SKEY_HIGH)
#define LCD_EXT_TEXT_HIGH              (LCD_MAX_Y-LCD_SKEY_HIGH)

#define LCD_ANNUN_FRAME_START          0x00       
#define LCD_TEXT_FRAME_START           (LCD_ANNUN_FRAME_START + LCD_ANNUN_HIGH)     
#define LCD_SKEY_FRAME_START           (LCD_TEXT_FRAME_START +  LCD_TEXT_HIGH)      

#define LCD_MAX_POPUP_WIN              1     /* maximum popup windows at once */

/* Maximum Popup window size */
#define LCD_POPUP_MAX_X                LCD_MAX_X 
#define LCD_POPUP_MAX_Y                LCD_EXT_TEXT_HIGH 

/* Maximum OPEN window size */
#define LCD_OPENWINDOW_WIDTH               176
#define LCD_OPENWINDOW_HIGH                  220

#define LCD_OPENWINDOW_MAX_X                LCD_MAX_X
#define LCD_OPENWINDOW_MAX_Y                LCD_OPENWINDOW_HIGH 

#define LCD_OPENWINDOW_START_X              (LCD_MAX_X-LCD_OPENWINDOW_WIDTH)/2
#define LCD_OPENWINDOW_START_Y              0
#define LCD_TIMEDISPLAY_START_X               26
#define LCD_TIMEDISPLAY_START_Y             60


#if (PROJECT_ID == WHALE1_PROJECT)
#define LCD_OPENWINDOW_ANNUN_HIGH          20 
#define OPEN_WINDOW_ANNUN_START_X           41
#else
#define LCD_OPENWINDOW_ANNUN_HIGH      18 
#define OPEN_WINDOW_ANNUN_START_X       41
#endif


/* update area mask */
/* User do not insist an area to be refreshed. Leave it to the Display system. */
#define AREAMASK_NULL                  0x00
#define AREAMASK_DEFAULT               0x0F    /* TEXT + BITMAP + ANNUN + SKEY*/

#define AREAMASK_ANNUN                 0x01
#define AREAMASK_SKEY                  0x02
#define AREAMASK_TEXT                  0x04    /* Update txt_screen. */
#define AREAMASK_BITMAP                0x08    /* Update bitmap_screen. */
#define AREAMASK_POPUP                 0x10    /* POPUP can be place on TEXT, BITMAP or both area */
#define AREAMASK_OPENWINDOW            0x20    /* Open window,use only when power down */
#define AREAMASK_ALL                   0x1F    /* ANNUN +  SKEY + POPUP + TEXT + BITMAP */


/*==================================================================================================
    MACROS
==================================================================================================*/
/* Conver 24-bit TRUE color, RGB into 16bpp 65k color */
#if 0
#define RGB(r,g,b)  \
   ((((((r)*125) >> 10)&0xffff)<< 11)|(((((g)*250) >> 10)&0xffff)<< 5) | ((((b)*125) >> 10)&0xffff))
#endif   

#define _RGB(r,g,b) (((((r) >> 3)&0xffff)<< 11)|((((g) >> 2)&0xffff)<< 5) | (((b) >> 3)&0xffff))

#define DS_OPEN_WINDOW    /* For dolphin open window */

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
#define COLOR_TRANS                     0x0010   /* Transparaent color used to draw a chracter. */
                                                  /* It should not be used any other purpose */
/* Pre-defined Color */
#define COLOR_BLACK                     0x0000                                                          
#define COLOR_WHITE                     0xffff     
#define COLOR_LIGHT_BEIGE               0xcfb9  // _RGB(203, 247, 205)  
#define COLOR_MIDNIGHT_BLUE             0x328f  // _RGB(53, 81, 126)    
#define COLOR_RED                       0xf800 // _RGB( 255,   0,   0 )                                                              
#define COLOR_CYAN                      0x07ff //_RGB(   0, 255, 255 )                                                            
#define COLOR_GREEN                     0x7581  // _RGB(119, 176, 9)                                     
#define COLOR_DARK_GREEN                0x0360 // _RGB(   0, 109,   0 )      
#define COLOR_GRASS_GREEN               _RGB(86,246,252)
#define COLOR_SOFTKEY                   _RGB(0, 220, 50)
#define COLOR_GRAY                      _RGB(145, 124, 145)
#define COLOR_EMERALD_GREEN             _RGB(198, 249, 241)
#define COLOR_NAVY_BLUE                 _RGB(  8,  55, 111)
#define COLOR_BLUE                      _RGB(  3, 86, 144)
#define COLOR_CERULEAN_BLUE             _RGB( 18, 113, 242)
#define COLOR_FRENCH_BLUE               _RGB( 34, 116, 185)
#define COLOR_YELLOW                    _RGB(249, 232,  48)
#define COLOR_DEEP_BLUE                 _RGB( 43,  92, 172)
#define COLOR_DARK_BLUE                 _RGB( 10,  80, 139)
#define COLOR_LIGHT_YELLOW              _RGB(247, 253, 184)
#define COLOR_LIGHT_BLUE                _RGB(171, 227, 245)
#define COLOR_MIDNIGHT                  _RGB( 30,  70, 122)
#define COLOR_SKY_BLUE                  _RGB(151, 211, 242)
#define COLOR_LIGHT_BLUE                _RGB(171, 227, 245)
#define COLOR_LIGHT_GRAY                _RGB(233, 233, 233)
#define COLOR_MIDDLE_GRAY               _RGB(169, 169, 169)
#define COLOR_MISTY_GRAY                _RGB(118, 118, 118)
#define COLOR_SMOKY_GRAY                _RGB(166, 166, 166)
#define COLOR_AQUA_BLUE                 _RGB( 58, 101, 207)
#define COLOR_RED_PINK                  _RGB(207,  58, 111)
#define COLOR_BLUE_VIOLET               _RGB(128, 143, 225)
#define COLOR_LIGHT_BLUE_VIOLET         _RGB(172, 184, 251)
#define COLOR_DARK_GRAY                 _RGB( 92,  92,  92)
#define COLOR_VERDITER_BLUE             _RGB(155, 192, 244)
#define COLOR_CORAL                     _RGB(249, 191, 191)
#define COLOR_BRIGHT_BLUE               _RGB(5, 92, 190)
#define COLOR_BLUE_MARINE               _RGB(34, 116, 185)
#define COLOR_COBALT_BLUE               _RGB(192, 212,235)
#define COLOR_MANAGANESE_BLUE           _RGB( 30,  77, 122)
#define COLOR_PRUSSIAN_BLUE             _RGB(3, 86, 144)
#define COLOR_DARK_SKY_BLUE             _RGB(1, 73, 171)
#define COLOR_LIGHT_CERULEAN_BLUE      _RGB(4, 161, 242)
#define COLOR_BRIGHT_SKY_BLUE          _RGB(168, 232, 250)
#define COLOR_DARK_CERULEAN_BLUE      _RGB(3,  86,  144)
#define COLOR_MIDDLE_BLUE               _RGB(1, 73,  171)
#define COLOR_BRIGHT_MARINE_BLUE      _RGB(185, 232,  254)
#define COLOR_BLUE_GREEN               _RGB(41, 149,  198) 
#define COLOR_WHITE_PINK                _RGB(254, 185,  231)
#define COLOR_VIOLET_PINK               _RGB(198, 41,  145)
#define COLOR_DARK_VIOLET               _RGB(150, 41,  198)
#define COLOR_VIOLET                    _RGB(233, 185, 254)
#define COLOR_BRIGHT_YELLOW            _RGB( 251, 245, 201 )
#define COLOR_RED_GRAY                  _RGB( 151, 150, 150 )
#define COLOR_BROWN_GRAY                _RGB( 76, 76, 76 )
#define COLOR_WHITE_BLUE                _RGB(118, 194, 248)
#define COLOR_WHITE_BLUE_BLUE          _RGB(101,200,252)
#define COLOR_SKY_BLUE_BLUE            _RGB(165,220,248)
#define COLOR_LIGHT_GRAY_VIOLET       _RGB(119,159,210)
#define COLOR_LIGHT_BLUE_GRAY         _RGB(255,242,248)
#define COLOR_MISTY_BLUE_GRAY         _RGB(68,103,136)
#define COLOR_RED_VIOLET_PINK         _RGB(210, 34, 141)
#define COLOR_BLUE_MINT                _RGB(2, 112, 206)
#define COLOR_VIOLET_SKY               _RGB(110,125,197)
#define COLOR_LIGHT_GREEN               _RGB(204,249,252)
#define COLOR_BRIGHT_VIOLET             _RGB(214,188,255)
#define COLOR_MARINE                      _RGB(30,148,210)
#define COLOR_LIGHT_LEMON_YELLOW        _RGB(255, 255, 218)
#define COLOR_PURL_BLUE                   _RGB(173, 225, 255)
#define COLOR_YELLOW_DEEP                _RGB(252, 202, 2)
#define COLOR_CYAN_BLUE                   _RGB(0,155,254)
#define COLOR_LILAC_PINK        _RGB(240, 205, 255)
#define COLOR_BRIGHT_CYAN_BLUE           _RGB(119, 238, 252)
#define COLOR_RED_VIOLET                  _RGB(157, 55, 210)
#define COLOR_BRIGHT_RED_VIOLET          _RGB(222, 177, 245)
#define COLOR_RED_WHITE                   _RGB(253, 249, 255)
#define COLOR_DARK_BLUE_MARINE           _RGB(1, 81, 149)  
#define COLOR_MANGANESE_BLUE             _RGB(39, 149, 225)
#define COLOR_WHITE_MANGANESE_BLUE      _RGB(184, 232, 254)
#define COLOR_COBALT_VIOLET_TINT        _RGB(231, 176, 254)
#define COLOR_SAP_BLUE          _RGB(37, 149, 253)
#define COLOR_LEMON_YELLOW        _RGB(255, 253, 213)
#define COLOR_VIOLET_PALE           _RGB(181, 5, 183)
#define COLOR_SAP_GREEN          _RGB(1, 166, 5)
#define COLOR_ORANGE          _RGB(243, 161, 4)
#define COLOR_DARK_YELLOW               _RGB(174, 114, 9)
#define COLOR_CARMINE               _RGB(161, 14, 3)
#define COLOR_WHITE_VIOLET      _RGB(202, 205, 253)
#define COLOR_RED_VIOLET                  _RGB(157, 55, 210)
#define COLOR_BRIGHT_RED_VIOLET          _RGB(222, 177, 245)
#define COLOR_RED_WHITE                   _RGB(253, 249, 255)
#define COLOR_DARK_BLUE_MARINE           _RGB(1, 81, 149)  
#define COLOR_MANGANESE_BLUE             _RGB(39, 149, 225)
#define COLOR_WHITE_MANGANESE_BLUE      _RGB(184, 232, 254)
#define COLOR_COBALT_VIOLET_TINT        _RGB(231, 176, 254)
#define COLOR_GRAY_BLUE                        _RGB(63,75,133)
#define COLOR_LIGHT_VERDURE             _RGB(179, 237, 17)
#define    SUBMENU_TITLE_FONT_COLOR      COLOR_NAVY_BLUE    
#define    NORMAL_TITLE_FONT_COLOR       COLOR_NAVY_BLUE
#if (PROJECT_ID == ODIN_PROJECT)
#define    FOCUS_COLOR_1                           _RGB(0, 53, 173)
#define    FOCUS_BORDER_COLOR_1            _RGB(198, 196, 182)
#define    FOCUS_COLOR_2                          _RGB(65, 70, 97)
#define    FOCUS_BORDER_COLOR_2           _RGB(164, 217, 254)
#define    FOCUS_COLOR_3                         _RGB(147, 16, 86)
#define    FOCUS_BORDER_COLOR_3           _RGB(178, 126, 208)
#define    FOCUS_SKY_LIGHT_BLUE            _RGB(236, 251, 254)
#else
#define    FOCUS_COLOR_1                           _RGB(198, 196, 182)
#define    FOCUS_BORDER_COLOR_1            _RGB(198, 196, 182)
#define    FOCUS_COLOR_2                          _RGB(164, 217, 254)
#define    FOCUS_BORDER_COLOR_2           _RGB(164, 217, 254)
#define    FOCUS_COLOR_3                         _RGB(178, 126, 208)
#define    FOCUS_BORDER_COLOR_3           _RGB(178, 126, 208)
#define    FOCUS_SKY_LIGHT_BLUE            _RGB(236, 251, 254)

#endif
/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/
/*==================================================================================================
    GLOBAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
/**************************************************************************************************  
    GRAPHIC REGION CONTROL DEFINITION
***************************************************************************************************/
/* All four points are inclued to the rectangle */
typedef struct 
{
    OP_INT16 left;
    OP_INT16 top;
    OP_INT16 right;
    OP_INT16 bottom;
} LCD_RECT_T;

typedef struct 
{
    OP_INT16 x;
    OP_INT16 y;
} LCD_POINT_T;


/* Screen Buffer */
typedef union 
{
    struct 
    {
        OP_UINT16    txt_screen[LCD_TEXT_HIGH][LCD_MAX_X];         /* MAIN FRAME(TEXT). Currently 176x204 */
        OP_UINT16    annun_screen [LCD_ANNUN_HIGH][LCD_MAX_X];   /* Display Annuciator icons currently 176x20 */
    } annunTextBuffer;  

    /* Extened Text buffer - Top 128x128 */
    OP_UINT16        ext_txt_screen[LCD_EXT_TEXT_HIGH][LCD_MAX_X];       

} SCRN_BUFFER_T;

/* Screen Buffer */
extern SCRN_BUFFER_T    scrn_buffer;

extern OP_UINT16        soft_screen  [LCD_EXT_SKEY_HIGH][LCD_MAX_X];        /* SOFTKEY FRAME. Currently 128x15 */

/* bitmap buffer for images and background process */
extern OP_UINT16        bitmap_buffer[LCD_MAX_Y][LCD_MAX_X];                   
/* animation buffer for all screen */
/* Popup Screen Buffer */
extern OP_UINT16        pop_buffer[LCD_MAX_POPUP_WIN][LCD_POPUP_MAX_Y][LCD_POPUP_MAX_X]; 
/* Open window Buffer */
extern OP_UINT16        open_window_buffer[LCD_OPENWINDOW_MAX_Y][LCD_OPENWINDOW_MAX_X]; 
/* LCD Mirror buffer */
extern OP_UINT16        lcd_screen[LCD_MAX_Y][LCD_MAX_X];                        

/* point the starting point of the txt_buffer - depends upon using annuciator area or not  */
extern OP_UINT16        (*txt_screen)[LCD_MAX_X];
extern OP_UINT16        (*annun_screen)[LCD_MAX_X];
extern OP_UINT16        (*bitmap_screen)[LCD_MAX_X];
extern OP_UINT16        (*open_window_screen)[LCD_OPENWINDOW_MAX_X];
extern OP_UINT16        (*pop_screen)[LCD_POPUP_MAX_X];

extern const OP_UINT8 LCD_CONTRAST_VALUES[];

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/
/*==================================================================================================
    FUNCTION: lcd_init

    DESCRIPTION:
        Initialize screen buffers. 

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
extern void lcd_init
(
    void
);


/*==================================================================================================
    FUNCTION: lcd_refresh

    DESCRIPTION:
        Refresh the screens to the lcd. 

    ARGUMENTS PASSED:
        update_area_f      : updated areas flag bits
        is_ann_displayed,  : indicate whether the annuciaotors are displayed or not
        *u_rect,           : updated rectangle 
        *pu_rect,          : popup window updated rectangle 
        scramble_color     : color used to scramble background in case popup window is updated 

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern void lcd_refresh
(
    OP_UINT8      update_area_f,
    OP_UINT8      is_full_screen_image,
    OP_BOOLEAN    is_ann_displayed,
    LCD_RECT_T    *u_rect,            
    LCD_RECT_T    *pu_rect,  
    OP_UINT16     scramble_color      
);

/*==================================================================================================
    FUNCTION: lcd_contrast...

    DESCRIPTION:
        LCD contranst management

    ARGUMENTS PASSED:

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
OP_BOOLEAN lcd_contrast_set(OP_UINT8 index);
OP_BOOLEAN lcd_contrast_get(OP_UINT8 *pIndex);
OP_BOOLEAN lcd_contrast_inc(void);
OP_BOOLEAN lcd_contrast_dec(void);


/*==================================================================================================
    FUNCTION: lcd_screen_init
    DESCRIPTION:
        initial the lcd_sreen[][] as color white. 
    ARGUMENTS PASSED:
    RETURN VALUE:
    IMPORTANT NOTES:
==================================================================================================*/
extern void lcd_screen_init
(
    void
);

/*==================================================================================================
    FUNCTION: lcd_openwindow_refresh
    DESCRIPTION:
        Refresh the open window screens to the lcd. 
    ARGUMENTS PASSED:
        *u_rect,           : updated rectangle 
    RETURN VALUE:
    IMPORTANT NOTES:
==================================================================================================*/
extern int lcd_openwindow_refresh
(
    LCD_RECT_T u_rect   /* updated rectangle */
);

/*================================================================================================*/
#endif  /* DS_LCD_H */

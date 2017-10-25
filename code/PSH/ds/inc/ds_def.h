#ifndef DS_DEF_H
#define DS_DEF_H
/*==================================================================================================

    HEADER NAME : ds_def.h

    GENERAL DESCRIPTION
        This file contains definition for display system.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial Creation
    03/06/2003   wangwei          PP00130     Set mew emeishan image .
                                              - add ANNUN_MUTE. 
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    08/04/2003   Chenyu           P001129     Add MMS icon
    10/14/2003   linda wang       C001806     Add function of setting download image to desktop
    12/04/2003   linda wang       p002147     Add qq and push image
    01/07/2003   Steven           P003225     adjust for dolphin menu   
    02/09/2004   Liyingjun        p002449     change max line value for dolphin
    02/17/2004   Zhuxq             P002492      Add Viewer window to widget system
    02/24/2004   chenxiao          p002530      modify WAP & MSG main menu 
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/20/2004   quzongqin        P002652     Add user define color theme.
    03/24/2004   chouwangyun   P002734     change resource and make new mainmenu back button and welcome style

    03/30/2004   Steven Lai        P002795     Fix the bug that the softkey area will disappear if the idle image is animation
    04/13/2004   lindawang         p002976     Add deactive gprs annunciator.
    04/24/2004   linda wang        c005088     Add jpg idle image and power on/off animation.
    05/10/2004   chouwangyun   p002882     fix bug about popup word can not be displayed fully problem
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    06/18/2004   liren          p006100    change the softkey position
    06/21/2004   liren          p006345    add member menuFontColor&focusbarFontColor to DS_COLOREDCOMPOSER_T
    06/23/2004   liren          c006348    change softkey font size for whale
    07/01/2004   liren          p006614    add a mask for 16*32 dial font 
    This file contains definition for display system.


====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "OPUS_typedef.h"
#include    "ds_lcd.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
/**********************************************
   LCD Screen Specific Constant Definitions
**********************************************/
 
/* Screen height in characters. Number of lines in the screen */
#define DS_SCRN_CHIGH                  5         /* Maximum line numbers in screen */   
                                                   /* 5 - case of no annuciator. */
/* Screen width in characters. Number of character in a line in the screen */   
#define DS_SCRN_CWIDE                  (LCD_MAX_X/8)       /* maximum number of character can be written within screen wide */

#define DS_SCRN_MAX_UC_LEN             (DS_SCRN_CWIDE * 2)  /* maximum unicode length in screen wide */

#define DS_SCRN_LINE_CWIDE             (LCD_MAX_X/8)      /* maximum number of characters in a line */      

/* Maximum Number of Unicode length in bytes in a line of screen - one unicode length is 2. */
#define DS_SCRN_MAX_LINE_UC_LEN        (DS_SCRN_LINE_CWIDE * 2)

#define DS_SCRN_MAXLINES               7         /* Maximum line numbers for text line data, exclude title. */
#define DS_TITLE_MAIN_SUBMENU_OFFSET   30 /* to display icon at the head in case of LA_TITLE_MAIN_SUBMENU */
#define DS_SCRN_MAXITEMS               10        /* Maximum line numbers for graphical item data, exclude title. 
                                                      It depends on design */

#define DS_SCRN_POPUP_MAXLINE          3        /* Maximum line of popup window exclude title. */
#define DS_SCRN_POPUP_CWIDE            30        /* Maximum chars whitin a  popup window line */
#define DS_SCRN_POPUP_MAX_UC_LEN       (DS_SCRN_POPUP_CWIDE *2)
#define DS_SCRN_POPUP_MAX_DIGIT_LEN    (DS_SCRN_POPUP_CWIDE *2)
#define DS_SCRN_POPUP_MAX_DIGIT_UC_LEN (DS_SCRN_POPUP_MAX_DIGIT_LEN *2)


#define DS_SCRN_TEXT_HIGH              LCD_TEXT_HIGH
#define DS_SCRN_EXT_TEXT_HIGH          LCD_EXT_TEXT_HIGH
#define DS_SCRN_ANNUN_HIGH             LCD_ANNUN_HIGH
#define DS_SCRN_SKEY_HIGH              LCD_SKEY_HIGH

/* Local Softkey related Definitions */
/* Softkey width */
/* Center key width */
#define THREEKEY_C_SKEY_WIDTH     60  /* desided by design */
/* Center side key (left and right) width */
#define THREEKEY_S_SKEY_WIDTH     58  /* desided by design */

#define THREEKEY_C_IDLE_SKEY_WIDTH     98 /* desided by design */
/* Center side key (left and right) width */
#define THREEKEY_S_IDLE_SKEY_WIDTH     46  /* desided by design */

#define DS_SCRN_MAX_X                  LCD_MAX_X
#define DS_SCRN_MAX_Y                  LCD_MAX_Y

#define DS_SCRN_DEFAULT_SMARGINE  2  /* default side margine */

#define NULL_UNICODE                   0x0000           /* indicate null unicode */
#define NULL_STRING                    {0x00, 0x00}           /* indicate null string */
#define NULL_PMPT                      (RM_RESOURCE_ID_T)0x00   /* indicate null prompt */
#define NULL_ICON                      (RM_RESOURCE_ID_T)0x00   /* indicate null icon */
#define NULL_BITMAP                    (RM_RESOURCE_ID_T)0x00   /* indicate null bitmap */
#define NULL_RESOURCE                  (RM_RESOURCE_ID_T)0x00   /* indicate null resource */

#define SUBMENU_BTN_X  18
#define SUBMENU_BTN_Y  144
#define SUBMENU_BTN_WIDTH  37
#define SUBMENU_BTN_HEIGHT 27
#define TITLE_MAIN_SUBMENU_BASE_TOP    124
#define MENUTITLE_START_X      111
#define MENUTITLE_START_Y      (ds.title.y.top)
#define SUBMAIN_ANIICON_WIDTH     65

/*==================================================================================================
    MACROS
==================================================================================================*/


/*==================================================================================================
    GLOBAL TYPE DEFINITIONS
==================================================================================================*/
typedef OP_UINT16                     DS_UNICODE;
typedef OP_UINT16                     DS_COLOR; /* 16bpp colors expressing 65k colors */
typedef OP_UINT8                      DS_MONO; /* 1bpp expressing black/white sublcd */
typedef OP_UINT16                     DS_FONTATTR;
typedef OP_UINT16                     DS_FONT_ID;
/* Do not use this mask unless there is need to force an area to be refreshed */
typedef OP_UINT8                      DS_AREAMASK;       

/*==================================================================================================
    SCREEN AREA AND ATTRIUTE CONTROL DEFINITION
==================================================================================================*/
typedef enum {
    SCREEN_NONE,        
    SCREEN_GENERAL,         /* Empty screen for general cases with annuciator, no background image */
    SCREEN_GENERAL_BG,      /* Empty screen for general cases with annuciator and background image */
    SCREEN_IDLE,            /* Compose Idle Screen */
    SCREEN_MAINMENU,        /* Compose Graphical Main Menu */
    SCREEN_MENU,            /* Compose Text-base Menu */
    SCREEN_LIST,            /* Compose Text-base List */
    SCREEN_FUNC,            /* Consisted line/item drawing functions without background image*/
    SCREEN_FUNC_BG,         /* Consisted line/item drawing functions on the background image*/
    SCREEN_PLAIN_TEXT,      /* Just Text display purpose with no annuciator, no background image, title is optional */
                            /* SMS display and Text editor can use it. Cursor is supported */
    SCREEN_FULL_IMAGE,      /* full screen size image - image is drawn bitmap buffer */
    SCREEN_OWNERDRAW,       /* Any style, under drawing owner responsibility */
    SCREEN_OPENWINDOW       /* Screen for display something when cover close, see from open window. */
} DS_SCRNMODE_ENUM_T;

typedef enum {
    COLOR_THEME_1 = 0,        
    COLOR_THEME_2,        
    COLOR_THEME_3,
    COLOR_THEME_USER_DEFINE = 0xff
} DS_COLORTHEME_ENUM_T;

/* the color define */
typedef struct
{
    OP_UINT8  r;
    OP_UINT8  g;
    OP_UINT8  b;
}DS_COLOR_RGB_T;

typedef enum
{
    COMPOSED_BASE,
    ANNUN_BAR, 
    TITLE_BAR,
    FOCUS_BAR,
    SOFTKEY_BAR,
    COMPOSED_MAX
}COMPOSED_AREA_ENUM_T;

typedef struct {
    DS_COLOR    annunColor;        
    DS_COLOR    titleColor;        
    DS_COLOR    focusbarColor;      
    DS_COLOR    softkeyColor;      
    DS_COLOR    titleFontColor;      
    DS_COLOR    menuFontColor;
    DS_COLOR    focusbarFontColor;
    DS_COLORTHEME_ENUM_T color_theme;  /* 0,1,2 is for pre-define , 0xff means user define */
} DS_COLOREDCOMPOSER_T;

typedef struct 
{
    DS_COLOR       bgColor;
    DS_COLOR       fontColor;
    DS_COLOR       backColor;
    DS_FONTATTR    fontAttr;
} DS_SCREENATTR_T;

typedef struct 
{
    DS_COLOR    bgColor;
    DS_COLOR    fgColor;
} DS_COLORATTR_T;

typedef struct 
{
    DS_SCREENATTR_T    skeyAttr;      /* SCREEN common attribute */
    DS_COLOR           skeyHLColor;   /* Skey specific color, Highlight line of the skey button. */
    DS_COLOR           skeyShadowColor; /* Skey specific color, Shadow line of the skey button. */
} DS_SKEYATTR_T;

typedef struct {
    DS_SCREENATTR_T    win;      /* area mainly display information. It can be preserented 
                                    text screen, bitmap screen or muxed  these two screens. */
    DS_SCREENATTR_T    annun;
    DS_SKEYATTR_T      skey;
    DS_SCREENATTR_T    pop;
    DS_COLORATTR_T     title;
    DS_COLORATTR_T     cursor;   /* Highlight bar or character cursor - partial cursor type highlight */
} DS_SCREENATTRS_T;



/*********************************************
    IMAGE TYPE DEFINITION  - for idle screen 
**********************************************/
typedef enum
{
    IMAGE_TYPE_ANI,
    IMAGE_TYPE_BITMAP,
    IMAGE_TYPE_JPEG,
    IMAGE_TYPE_ANALOG_CLOCK,
    IMAGE_TYPE_DOWNLOAD
}   DS_IMAGE_TYPE_ENUM_T;


/*******************************
    System Color defintion               
*******************************/
/* Use these color definiton for consistency and color theme changes */

/* Window default colors */
#define COLOR_WINBG         ( ds.scrnAttr.win.bgColor )        /* Background of window */
#define COLOR_FONTFG        ( ds.scrnAttr.win.fontColor )    /* Foreground(font) color of window */
#define COLOR_FONTBG        ( ds.scrnAttr.win.backColor )    /* Background color of font */
#define COLOR_TITLEFG       ( ds.scrnAttr.title.fgColor )    /* foreground(font)color of the title */
#define COLOR_TITLEBG       ( ds.scrnAttr.title.bgColor )   /* Background color of the title */
#define COLOR_TITLESHAOW    COLOR_DARK_BLUE_MARINE          /* title font shadow color */

/* Highlighting cursor color */
#define COLOR_CURSORFG      ds.scrnAttr.cursor.fgColor      /*  Highlighted font color */
#define COLOR_CURSORBG      ( ds.scrnAttr.cursor.bgColor )  /* Cursor color */

#define COLOR_FILLBG        COLOR_BRIGHT_SKY_BLUE        /* color of line of LA_FILL */

/* Button style rectangle colors */
#define COLOR_FILL_BTNDARK     COLOR_LIGHT_CERULEAN_BLUE        /* color of line of LA_FILL */
#define COLOR_FILL_BTNBRIGHT  COLOR_LIGHT_CERULEAN_BLUE        /* color of line of LA_FILL */
#define COLOR_FILL_BTNFILL    COLOR_BRIGHT_SKY_BLUE        /* color of line of LA_FILL */

#define COLOR_BTNDARK       COLOR_MISTY_BLUE_GRAY               /* Color of Dark sides :  Left and Top*/
#define COLOR_BTNBRIGHT     COLOR_MISTY_BLUE_GRAY                   /* Color of Bright sides : right and bottom */
#define COLOR_BTNFILL       COLOR_LIGHT_BLUE_GRAY                     /* Fill color of the button */

/* Editline - button style rectangle colors */
#define COLOR_FILL_EDIT_BTNDARK       COLOR_LIGHT_CERULEAN_BLUE               /* Color of Dark sides :  Left and Top*/
#define COLOR_FILL_EDIT_BTNBRIGHT     COLOR_BRIGHT_SKY_BLUE                   /* Color of Bright sides : right and bottom */
#define COLOR_FILL_EDIT_BTNFILL       COLOR_BRIGHT_SKY_BLUE                   /* Fill color of the button */

#define COLOR_EDIT_BTNDARK       COLOR_MISTY_BLUE_GRAY               /* Color of Dark sides :  Left and Top*/
#define COLOR_EDIT_BTNBRIGHT     COLOR_LIGHT_BLUE_GRAY                   /* Color of Bright sides : right and bottom */
#define COLOR_EDIT_BTNFILL       COLOR_LIGHT_BLUE_GRAY                   /* Fill color of the button */

#define COLOR_INACTIVE_FONTFG    COLOR_LIGHT_GRAY_VIOLET

/* Popup window default colors */
#define COLOR_POPUPBG       ( ds.scrnAttr.pop.bgColor )        /* Background color of popup */
#define COLOR_POPUPFG       ( ds.scrnAttr.pop.fontColor )    /* Foreground(font)color of popup */
#define COLOR_POPUPSHADOW   COLOR_WHITE

/* Softkey area default colors */
#define COLOR_SOFTKEYBG     ( ds.scrnAttr.skey.skeyAttr.bgColor )   /* Background color of softkey */
#define COLOR_SOFTKEYFG     ( ds.scrnAttr.skey.skeyAttr.fgColor )   /* Foreground(font)color of softkey */

/* annuciator area default color */
#define COLOR_ANNBG          COLOR_WHITE       /*background color of annuciator*/

#define COLOR_CHAR_CURSORBG           COLOR_WHITE_BLUE_BLUE
#define COLOR_POPUP_CHAR_CURSORBG    COLOR_SKY_BLUE_BLUE


/*==================================================================================================
    GRAPHIC REGION CONTROL DEFINITION
==================================================================================================*/
/* Drawing area - DS internally used */
typedef enum {
    AREA_ANNUN   = AREAMASK_ANNUN,       /* Annuciator area at the top to display the status of the handset. */
    AREA_SOFTKEY = AREAMASK_SKEY,        /* Softkey area at the bottom. */
    AREA_TEXT    = AREAMASK_TEXT,         /* Text screen */
    AREA_BITMAP  = AREAMASK_BITMAP,         /* Bitmap screen */
    AREA_POPUP   = AREAMASK_POPUP,       /* Poopup screen */
    AREA_OPEN_WINDOW = AREAMASK_OPENWINDOW,
    AREA_SUBLCD                         /* External LCD area */
} SCRNAREA_ENUM_T;

/* All four points are inclued to the rectangle */
typedef struct 
{
    OP_INT16 left;
    OP_INT16 top;
    OP_INT16 right;
    OP_INT16 bottom;
} DS_RECT_T;

typedef struct 
{
    OP_INT16 x;
    OP_INT16 y;
    OP_INT16 width;
    OP_INT16 height;
} DS_REGION_T;

typedef struct 
{
    OP_INT16 x;
    OP_INT16 y;
} DS_POINT_T;

/* Indicate top and bottom y coordinates of a line */
typedef struct 
{
    OP_INT16 top;
    OP_INT16 bottom;
} DS_LINE_Y_COOR_T;


/*==================================================================================================
    FONT CONTROL DEFINITIONS
==================================================================================================*/

  
/************************************************
   System Font Attributes definition
*************************************************/

/*************
 * FONT TYPE *
 *************                 7 6 5 4   3 2 1 0
                               x x x x   x x | |
                               | | | |   | | | +-> Bold or Normal (default: 0 - Normal)
                               | | | |   | | +---> Underline      (default: 0 - FALSE)
                               | | | |   | +-----> Overlap        (default: 0 - FALSE)
                               | | | |   +-------> Reverse        (default: 0 - FALSE) : SUB LCD Only
                               | | | |
                               | | | +------------> Small size font   (default: 0 - FLASE)           
                               | | +--------------> Medium size font  (default: 1 - TRUE)     
                               | +----------------> Large size font   (default: 0 - FALSE)
                               +------------------> Dialing Font      (default: 0 - FALSE)
Caution!!! 
  1. Font size bits (Bit4, 5 and 6) are exclusive each other. 
     And the rest of the bits can be combined.
  2. FONT_REVERSE is only supported in sub lcd.
     To have reverse effect in main lcd, assign background and foreground colors reversely.
*/
/* Attention for Dolphin, 
   As for Dolphin use T9 font, so there is only two fonts, 6*12, 13*14, 
   the real size is 8*16, 16*16
*/
#define FONT_DEFAULT               0x00   /* Normal, Medium size, non dialing font and non overlap(opaque) */ 

#define FONT_NORMAL                   0x00   /* Default Style */
#define FONT_BOLD                  0x01        

#define FONT_UNDERLINE          0x02   /* Draw underline at the bottom of a character */
#define FONT_OVERLAP            0x04   /* It set, do not fill the background with font bg color */
#define FONT_SHADOW             0x08    /* shadowing text */
#define FONT_REVERSE               0x10   /* Sub Lcd Only */

#define FONT_SIZE_SMALL         0x10   
#define FONT_SIZE_MED            0x20   /* Default size */
#define FONT_SIZE_LARGE              0x40   

#define FONT_DIAL                0x80   /* Dialing font, only support dialing digits (0~9, #, *, :, H, T, -) */
#define FONT_DIAL_L              0x0100

/* Font default attribute */
#define FA_DEFAULT      FONT_DEFAULT

/* Normal window font default attribute */
#define FA_WINDEFAULT   FONT_DEFAULT     /* Default font attribute in a window */
#define FA_WINTITLE     FONT_SIZE_SMALL|FONT_OVERLAP     /* Title default attribute in a window */
#define FA_WINSELECT    FONT_DEFAULT    /* Selected Font default attribute in a window */

/* Menu Font default attribute */
#define FA_MENUDEFAULT  FONT_DEFAULT    /* Defualt font attribute in a menu */
#define FA_MENUTITLE    FONT_DEFAULT|FONT_BOLD|FONT_OVERLAP    /* Defualt title attribute in a menu */
#define FA_MENUSELECT   FA_MENUDEFAULT    /* Defualt selected item default attribute in a menu */

/* List Font default attribute */
#define FA_LISTDEFAULT  FONT_DEFAULT    /* Defualt font attribute in a list */
#define FA_LISTTITLE    FONT_DEFAULT|FONT_BOLD    |FONT_OVERLAP/* Defualt title attribute in a list */
#define FA_LISTSELECT   FA_LISTDEFAULT    /* Defualt selected item default attribute in a list */

/* Drawing Function Default font attribute */
#define FA_FUNCDEFAULT  ( FONT_DEFAULT)    /* Defualt font attribute in a drawing function */
#define FA_FUNCTITLE    FONT_DEFAULT|FONT_BOLD|FONT_OVERLAP    /* Defualt title attribute a drawing function */
#define FA_FUNCSELECT   FA_DEFAULT      /* Defualt selected item default attribute in a list */

/* Popup Font default attribute */
#define FA_POPDEFAULT   ( FONT_DEFAULT|FONT_OVERLAP)     /* Defualt font attribute in a popup window */
#define FA_POPTITLE     FONT_DEFAULT                     /* Defualt title attribute in a popup window  */
#define FA_POPSELECT    FA_POPDEFAULT      /* Defualt selected item default */
                                         /* attribute in a popup window  */

#define FA_SKEY          ( FONT_DEFAULT|FONT_SIZE_SMALL|FONT_OVERLAP)     /* Defualt skey font attribute */


typedef struct 
{
    DS_FONT_ID    fontid;           /* Font Id */
    OP_UINT8      fontx;            /* font x size in pixel */
    OP_UINT8      fonty;            /* font y size in pixel */
    OP_BOOLEAN    underline;
    OP_BOOLEAN    overlap;          /* do not paint background color to make the bg is visible */
    OP_UINT8      buf_width;        /* buffer width where a character is drawn */
    DS_COLOR      fontcolor;
    DS_COLOR      backcolor;
    OP_UINT16     *dest;            /* destination (screen buffer) */
} DS_FONT_T;

/*==================================================================================================
    SOFT KEY CONTROL DEFINITION
==================================================================================================*/
/* softkey attributes - apply per sokfkey */
typedef enum 
{
    SKA_NOALIGN    = 0,         /* No alignment. User specifies the positon */
    SKA_LEFTALIGN  = 1,         /* Left alignment */
    SKA_CENTERLIGN = 2,         /* Left alignment */
    SKA_RIGHTALIGN = 3          /* Right alignment */
} DS_SKEYALIGN_ENUM_T;

#define SKA_NOUSE         0x10  /* Not use the skey */
#define SKA_SHADOWTEXT    0x20    /* Shadowing string */
#define SKA_DEFAULT       0x02    /* Default attr : Center aligned w/o shadow */

/* Can be combined multiply */
/* lower nibble presents alignment and higher nibble represents ther lest of the attributes. */
typedef OP_UINT8 DS_SOFTKEYATTR; 
/* softkey position */
typedef enum 
{
    SOFTKEY_CAPTION,
    SOFTKEY_LEFT = SOFTKEY_CAPTION,
    SOFTKEY_CENTER,
    SOFTKEY_RIGHT
} DS_SKEYPOS_ENUM_T;


#define SKEY_MAX_CHAR          7        /* Maximum number of charcters in the a softkey */
#define SKEY_MAX_CAP_CHAR      20        /* Maximum number of charcters in the a softkey */

#define SKEY_MAX_NUM           3        /* Maximum number of softkeys */

/* softkey display style - Appled commonly to all the softkeys */
#define SKEYSTYLE_BUTTON       0x01     /* Button Style */
#define SKEYSTYLE_NOBUTTON     0x00        /* Do not daraw button */
#define SKEYSTYLE_DXCAPTION    0x02        /* Direct Caption */

#define SKEYSTYLE_DEFAULT  SKEYSTYLE_BUTTON      /* softkey default style */

typedef OP_UINT8 DS_SOFTKEYSTYLE;

typedef struct 
{
    OP_UINT8          txt[SKEY_MAX_CHAR*2+2];   /* skey string */
    RM_ICON_T         *icon;
    OP_BOOLEAN        update;        /* update flag */
    DS_SOFTKEYATTR    attr;            /* softkey attributes */
    OP_INT16          x_pos;        /* user specified x-position in SKA_NOALIGN */
    OP_INT8           length;       /* text length : if -1, length is calculated automatically */
    DS_COLOR          fgcolor;      /* foreground(text) color */
    DS_COLOR          bgcolor;      /* background color */
} DS_SKEYDATA_T;

typedef struct 
{
    DS_SOFTKEYSTYLE       style;     /* softkey style */
    DS_SKEYDATA_T         left;
    DS_SKEYDATA_T         center;
    DS_SKEYDATA_T         right;
} DS_SKEYSET_T;


/*==================================================================================================
    ADVANCED LINE CONTROL DEFINITION  - TEXT DATA 
==================================================================================================*/
/* line attributes definition - applied to SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC, SCREEN_FUNC_BG */
#define LA_DEFAULT              0x00000000
#define LA_BG_BITMAP            0x00000001 /* bg bitmap in the line */   
#define LA_RADIOBUTTON          0x00000002    /* Radio Button. LA_RADIOBUTTON has higher priority than LA_CHECKBOX and LA_ICON. */
#define LA_CHECKBOX             0x00000004    /* Checkbox */
#define LA_BACK_CHECKBOX        0x00000008 /* display checkbox at the end of the line */
                                             /* LA_BACK_CHECKBOX is higher priority than LA_CHECKBOX */
#define LA_ICON                 0x00000010    /* Head icon in the line */
                                             /* LA_BACK_ICON is higher priority than LA_ICON */
#define LA_BACK_ICON            0x00000020  /* display icon at the end of the line */
#define LA_BACK_CHECKMARK       0x00000040  /* display check mark icon at the end of the line. */
                                             /*   It is exclusive selection as radiobutton */
                                             /* It has higher priority than LA_RADIOBUTTON, LA_CHECKBOX, */
                                             /* LA_ICON, LA_BACK_CHCEKBOX, LA_BACK_ICON */
#define LA_LINENUMBER           0x00000100    /* Head number in the line - If icon and linenumber co-exist 
                                               in a line, Icon then line number are displayed. */       

#ifdef NOT_SUPPORTED
#define LA_LARGEFONT            0x00000200    /* Use large font */
#endif
#define LA_NO_DEFAULT_SKEY     0x00000200    /* Used in popup numeditor. if it is set, no default skey is used. */
#define LA_SMALLFONT            0x00000400    /* Use small font */
#define LA_SHADOWFONT           0x00000800 /* shadowing text */
#define LA_OVERLAP              0x00001000  /* Overlapped text - backgound is visible */

#define LA_CENTER_TEXT          0x00002000    /* Center-alligned display */
#define LA_RIGHT_TEXT           0x00004000 
#define LA_INACTIVE_TEXT        0x00008000  /* Used to dispaly item can't be selcted. Can't be applied to the focused line. */

#define LA_TITLE_MAIN_SUBMENU   0x00010000  /* title attr - title of a submenu is diffently displayed from other title */
#define LA_TITLE_SECOND_SUBMENU 0x00030000  /* title attr - title of a second submenu is diffently displayed from other title */
#define LA_TITLE_MSG_DETAIL     0x00020000  /* title attr - title of a message detail */
#define LA_SUBITEM              0x00040000  /* Sub-item style - indented one character width */

#define LA_SIDEBUTTON           0x00080000  /*   Display left and right arrows in both side of the line. */
#define LA_NARROW_SIDEBUTTON    0x00100000  /*   Display left and right arrows in the line. 
                                                     Give 5-character spaces between arrows. */
#define LA_LEFTBUTTON           0x00200000  /* Displays left arrow button at the end of line. */
#define LA_RIGHTBUTTON          0x00400000  /* Displays right arrow button at the end of line. */
#define LA_UPBUTTON             0x01000000  /* Displays up arrow button at the end of line */
#define LA_DOWNBUTTON           0x02000000  /* Displays down arrow button at the end of line */


#define LA_BUTTONSTYLE          0x04000000  /* Disply button frame in the line */
#define LA_FILL                 0x08000000  /* Fill the line */
#define LA_FOCUS_FILL           0x10000000  /* Fill the line when the line is focused */

#define LA_EDITLINE             0x20000000  /* One line editor styled line - used for date input */
#define LA_OWNERDRAW            0x40000000  /* Owner-draw line - specified call back function is called */
#define LA_PASSWORD             0x80000000  /* asterisk will be displayed instead of digits - 
                                          currently applied only applied to POPUP_NUMEDITOR.
                                         If the line is not password stype, use LA_DEFAULT */


typedef OP_UINT32               DS_LINEATTR;

typedef struct 
{
    OP_BOOLEAN     flag;                    /* update flag */
    OP_BOOLEAN     sel;                     /* selection flag for radio button */
    OP_BOOLEAN     check;                   /* check flag for checkbox */
    OP_UINT8       number;                   /* absolute Line number. 0-based */
    DS_LINE_Y_COOR_T  y;                      /* y-size of line */    
    DS_LINE_Y_COOR_T  base;                  /* based y coordinate to write text in a line */    
    OP_INT16       it_margine;              /* icon and text margine */
    OP_UINT8       leng;                    /* text length */
    DS_LINEATTR    attr;                    /* line attributes */
    DS_RECT_T      rect;                    /* line rectangle */
    OP_UINT8  (*draw_func) ( OP_UINT8 line, OP_UINT32 data );    /* user-draw function pointer. Returned values
                                                   means y-pixel counts user function processed */
    OP_UINT8       txt[DS_SCRN_MAX_LINE_UC_LEN+2];    /* text string pointer */
    RM_ICON_T      *icon;                   /* icon for the line */
    RM_BITMAP_T    *bitmap;                 /* bitmap for the line */
    OP_UINT32      data;                    /* user-set line data to the user drawing function */
} DS_LINEDATA_T;

typedef enum
{
     TITLE_NULL,
     TITLE_NORMAL,           /* normal screen title */
     TITLE_ARROW,            /* title including arrows indicating next selection */
     TITLE_MAIN_SUBMENU,    /* titile of submenu under the main menu */
	 TITLE_SECOND_MENU,     /* titile of second submenu */
     TITLE_MSG_DETAIL,      /* titile of message detial - for message app - it has message type icons at the top of title */
                              /* the maximum line will be reduced by one */
     TITLE_OWNERDRAW        /* owner draw title, title drawing function shall be given */ 
} DS_SCRN_TITLE_ENUM_T;


/*==================================================================================================
    GRAPHIC ITEM DATA CONTROL - NON-TEXT DATA
==================================================================================================*/
/* graphic item data attributes definition - SCREEN_FUNC and SCREEN_FUNC_BG */
#define GIA_DEFAULT      0x0000
#define GIA_TEXT         0x0001
#define GIA_BITMAP       0x0002
#define GIA_ICON         0x0004  
#define GIA_BG_BITMAP    0x0008     /* bitmap which is used as background image of the screen. Used only with SCREEN_FUNG_BG */
#define GIA_OWNERDRAW    0x0010

typedef OP_UINT16  DS_GITEMATTR;

typedef struct 
{
    OP_BOOLEAN     flag;                    /* update flag */
    DS_GITEMATTR   attr;                    /* item attributes */
    DS_RECT_T      rect;                    /* item rectangle area */
    RM_BITMAP_T    *bitmap;                    /* bitmap image pointer : GIA_BITMAP */
    RM_ICON_T      *icon;
    OP_BOOLEAN     (*draw_func) ( OP_UINT8 index, OP_UINT32 data ); /* user-draw function pointer. Returned value
                                                           is TRUE when screen refresh required */
    OP_UINT32      data;                    /* user-set item data */
} DS_GITEMDATA_T;


/*==================================================================================================
    POPUP MESSGAE CONTROL DEFINITION  
==================================================================================================*/
#define DS_POPUP_DEFAULT_MSG_TIME    2000

/*==================================================================================================
    POPUP LIST CONTROL DEFINITION  
==================================================================================================*/
typedef struct 
{                                   
    OP_UINT8            txt[DS_SCRN_POPUP_MAX_UC_LEN+2];    /* text of choice - last is empty */
    RM_RESOURCE_ID_T    txt_res_id;              /* text resource id */
    RM_RESOURCE_ID_T    icon_res_id;
    DS_LINEATTR          attr;                     /* line attributes */
} DS_POPUP_LIST_ITEM;  

typedef struct 
{
    OP_UINT8              title[DS_SCRN_POPUP_MAX_UC_LEN+1]; /* list title */
    RM_RESOURCE_ID_T      title_res_id;           /* header title resource id */
    RM_RESOURCE_ID_T      title_icon_res_id;
    OP_UINT8              init_sel_item;         /* initially selected item */
    OP_UINT8              nitems;                 /* number of items */
    DS_POPUP_LIST_ITEM    *items;                 /* array of the list items */
} DS_POPUP_LIST_T;


/*==================================================================================================
    POPUP MENU CONTROL DEFINITION  
==================================================================================================*/
typedef struct
{
    OP_UINT8            title[DS_SCRN_POPUP_MAX_UC_LEN+1] ;  /* header text string (top line) */
    RM_RESOURCE_ID_T    title_res_id;             /* header title resource id */
    RM_RESOURCE_ID_T    title_icon_res_id;
    void                *menu_data_ptr;           /* pointer to the menu entries array, this is a  */
                                                  /* void but should be a DS_POPUP_MENU_ENTRY_T    */
    OP_UINT8            menu_entry_siz;           /* the number of menu selections for the tier    */
} DS_POPUP_MENU_TIER_T ;

typedef struct
{
    OP_UINT8                      txt[DS_SCRN_POPUP_MAX_UC_LEN+2];        /* entry text string for display */
    RM_RESOURCE_ID_T              txt_res_id;                  /* entry text resource id */
    OP_BOOLEAN                    sub_menu_flag;               /* indicates whether the union contains the next  */
                                                               /* menu or an action to invoke. TRUE = next_menu  */
    void                          (*action_func_ptr)(void) ;   /* function pointer to invoke */
                                                               /* at end of menu tree */
    DS_POPUP_MENU_TIER_T const    *next_menu_ptr ;             /* points to next menu down  */
} DS_POPUP_MENU_ENTRY_T ;

/*==================================================================================================
    POPUP NUM EDITOR CONTROL DEFINITION  
==================================================================================================*/
typedef struct 
{
    OP_UINT8              title[DS_SCRN_POPUP_MAX_UC_LEN+1] ;  /* header text string (top line) */
    RM_RESOURCE_ID_T      title_res_id;             /* header title resource id */
    OP_UINT8              msg[DS_SCRN_POPUP_MAX_UC_LEN+1]; /* list title */
    RM_RESOURCE_ID_T      msg_res_id;           /* header title resource id */
    OP_UINT8              digits[DS_SCRN_POPUP_MAX_DIGIT_UC_LEN+2]; /* numerid digit in POPUP_NUMEDITOR */
    OP_UINT8              max_leng;             /* maximum length of the digits */
    OP_INT16              pos;                   /* position of the cursor in the edited digits */
    DS_LINEATTR           attr;                  /* digit attribute */
} DS_POPUP_NUMEDITOR_T;



/*==================================================================================================
    POPUP WINDOW CONTROL DEFINITION  
==================================================================================================*/
#define DS_POPUP_MSG_DEFAULT_TIME    2000

typedef enum 
{
    DPU_INIT,            /* initial update */
    DPU_REDRAW,            /* update to redraw */
    DPU_LISTALL,        /* update all list items */
    DPU_LISTLINE        /* update certain list item */
} DS_POPUP_DRAW_ENUM_T;


typedef struct 
{
    DS_POPUP_DRAW_ENUM_T  type;
    OP_UINT8 line;
} DS_POPUP_DRAW_INFO_T;


/* Popup type */
typedef enum 
{
    POPUP_MSG,         /* Popup Message - for warning or general messages */
    POPUP_DIALOG,      /* Popup dialog - required user reactio to the popup - (YES and NO) */
    POPUP_MENU,        /* Popup menu */
    POPUP_LIST,        /* Popup list */
    POPUP_NUMEDITOR,  /* Popup Numeric digit editor */
    POPUP_OWNERDRAW    /* Owner-drawn popup */
} DS_POPUP_TYPE_ENUM_T;

/* User Data type in User List popup (PUA_USERLIST) 
   passed as the first parameter of userdata_handler function 
   Indicating type of user data. 
*/
typedef enum 
{
    PUUD_TITLE,        /* Title. */
    PUUD_LINE,         /* List line.Tthe 2nd param. of a popup handler func. is the line index index */
    PUUD_LINEATTR     /* Line attribute. 2nd param. of a popup handler func. is the line index index */
} DS_POPUP_UDATA_ENUM_T;

/* Popup Data */
typedef struct 
{
    DS_POPUP_TYPE_ENUM_T    type;                                          /* popup window type */
    OP_UINT8                title[DS_SCRN_POPUP_MAX_UC_LEN+1];                        /* popup window title */
    OP_UINT8                msg[DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE+1];    /* message to be displayed */
    RM_ICON_T               *icon;                                         /* icon */
    DS_POPUP_LIST_T         *list;                                         /* popup window list */
    DS_POPUP_MENU_TIER_T    *menu;                                         /* popup menu */
    DS_POPUP_NUMEDITOR_T    *numeditor;  /* data of POPUP_NUMEDITOR */
} DS_POPUP_DATA_T; 

/* Popup window state */
typedef enum 
{
    PS_NONE_S,
    PS_ENTER_S,
    PS_MAIN_S,
    PS_RETURN_S,       /* Re-entered state once it is endtered */
    PS_STO_S,          /* Stored key is pressed */
    PS_EXIT_S
} DS_POPUP_STATE_ENUM_T;


/*==================================================================================================
    ANIMATION CONTROL DEFINITION  
==================================================================================================*/
#define DS_ANI_CONTINUOUSLY    0xff

typedef enum
{
    ANI_INACTIVE,
    ANI_ACTIVE,
    ANI_PAUSED
} DS_ANI_STATUS_ENUM_T;

typedef enum
{
    ANI_NORMAL,
    ANI_RESIZED
} DS_ANI_TYPE_ENUM_T;

typedef struct 
{
    OP_UINT8                frame;          /* current frame */
    OP_UINT8                max_repeat;      /* how many this animation is repeate */
    OP_UINT8                repeat_cnt;   /* number of currently being repeated */
    OP_INT16                dx;           /* destination x position */
    OP_INT16                dy;           /* destination y position */
    OP_UINT8                dw;           /* destination width */
    OP_UINT8                dh;           /* destination height */
    OP_INT16                sx;           /* source x position */
    OP_INT16                sy;           /* source y position */
    OP_BOOLEAN             bAniStart;    /* the flag for ani start */
    DS_ANI_TYPE_ENUM_T      type;
    OP_UINT32               delay;        /* time delay between frames */
    SCRNAREA_ENUM_T         area;         /* buffer to be drawn */
    DS_ANI_STATUS_ENUM_T    status;          /* status of an animation */
    RM_INT_ANIDATA_T        *data;         /* animation data */
} DS_ANI_T;


/*==================================================================================================
    ANNICIATOR CONTROL DEFINITION  
==================================================================================================*/
typedef enum
{

/***********************************************************************
 * If you change the order of enum, you MUST change rm resource file also.
*************************************************************************/
    ANNUN_RSSI0 = 0, /* position 1 start */ 
    ANNUN_RSSI1,
    ANNUN_RSSI2,
    ANNUN_RSSI3,
    ANNUN_RSSI4,
    ANNUN_RSSI5,
    ANNUN_RSSI6,    /* position 1 end */

    ANNUN_MSG,       /* position 4 start */           
    ANNUN_MMS,      /* postion 4 end */
    ANNUN_QQ,
    ANNUN_PUSH,    /* position 6 start, one of message coming from wap */
    ANNUN_VMAIL,
    
    ANNUN_SMS_PHONE_FULL,
    ANNUN_SMS_SIM_FULL,
    ANNUN_SMS_ALL_FULL,
    
    ANNUN_CONNECTED,  /* position 3 start */ 
    ANNUN_MISSED_CALL,//ANNUN_NOSERVICE,
    ANNUN_DIVERT,       
    ANNUN_CSD,   /* position 3 end */ 
    
    ANNUN_VIB,     /* position 2 start */
    ANNUN_NORMAL,                             
    ANNUN_ALOUD,                         
    ANNUN_EARPHONE,                  
    ANNUN_INMOBILE,      
    
    ANNUN_MELODY,   
    ANNUN_VIB_BELL, 
    ANNUN_SILENCE, 
    ANNUN_MUTE,     /* postion 2 end */
    
    ANNUN_ALARM,    /* position 5 start */       /* 11 */
    ANNUN_ROAM,     /* position 5 end */
    
    ANNUN_GPRS_INACTIVE,    /* postion 6 end */
    ANNUN_GPRS,    /* postion 6 end */
    
    ANNUN_BATT0,   /* position 7 start */    
    ANNUN_BATT1,
    ANNUN_BATT2,
    ANNUN_BATT3,
    ANNUN_BATT4    /* position 7 end */
} DS_ANNUN_ENUM_T;


/*================================================================================================*/
#endif  /* DS_DEF_H */

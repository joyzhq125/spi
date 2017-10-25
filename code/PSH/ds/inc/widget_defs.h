#ifndef _WIDGET_DEFS_H
#define _WIDGET_DEFS_H
/*====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/14/2004   Zhuxq            P001345     Optimize code for modularization
    07/07/2004   Chenxiao         p006839     adjust layout according to customer's request 
    07/28/2004   ZhangYoukun      p007383     open  MENU_ITEM_STRING_SCROLL_PERIODICALLY
    09/17/2004   Hover            p008400     change odin/odin_a focus bar value 
   
====================================================================================================*/

/* current color scheme */
#define CUR_COLOR_THEME                          (ds.cComposer.color_theme)

/*=== window title === BEGIN ====*/
/* The height of window title, please comply with LINE_BASE_Y */ // TODO: comply with LINE_BASE_Y
#define  WINDOW_TITLE_HEIGHT                 18

/* define menu/window title picture */
#define  MAIN_MENU_TITLE_BG                  BMP_SUB_SECONDMENU_TITLE_BG_C1 
#define  WINDOW_TITLE_BG                     BMP_SUB_SECONDMENU_TITLE_BG_C1
/*=== window title === END ====*/


/*==== main menu style == BEGIN ==*/
/* choose one of the three: 
        MAIN_MENU_STYLE_IMAGE_TEXT,
        MAIN_MENU_STYLE_INDEX_TEXT,
        MAIN_MENU_STYLE_TEXT 
*/
/* app menu style definition:  a image and a text per menu item */
//#define MAIN_MENU_STYLE_IMAGE_TEXT

/* app menu style definition:  a index and a text per menu item */
#define MAIN_MENU_STYLE_INDEX_TEXT

/* app menu style definition:  only a text per menu item */
//#define MAIN_MENU_STYLE_TEXT
/*==== main menu style == END ==*/

/*==== menu style == BEGIN ==*/
/* choose one of the three: 
        MENU_STYLE_IMAGE_TEXT,
        MENU_STYLE_INDEX_TEXT,
        MENU_STYLE_TEXT 
*/
/* normal menu style definition:  a image and a text per menu item */
//#define MENU_STYLE_IMAGE_TEXT

/* normal menu style definition:  a index and a text per menu item */
#define MENU_STYLE_INDEX_TEXT

/* normal menu style definition:  only a text per menu item */
//#define MENU_STYLE_TEXT
/*==== menu style == END ==*/

/*=== define menu item background = BEGIN ==*/
/*background picture of  normal menu  */
#define  MENU_ITEM_BG_PIC                   (RM_RESOURCE_ID_T)(CUR_COLOR_THEME + BMP_SECONDMENULINE_BG)
/*background picture of  normal menu with a scrollbar  */
#define  MENU_ITEM_BG_PIC_VS_SB             (RM_RESOURCE_ID_T)(CUR_COLOR_THEME + BMP_SECONDMENULINE_BG)
/*background picture of  focused menu  */
#define  MENU_ITEM_FOCUS_PIC                (RM_RESOURCE_ID_T)(CUR_COLOR_THEME + BMP_SUB_MAINMENU_FOCUS_BAR_C1)
/*background picture of  focused menu with a scrollbar  */
#define  MENU_ITEM_FOCUS_PIC_VS_SB          (RM_RESOURCE_ID_T)(CUR_COLOR_THEME + BMP_SUB_MAINMENU_FOCUS_SHORT_BAR_C1)

/*background picture of  normal menu customed by user */
#define  MENU_ITEM_BG_PIC_BY_USER           (RM_RESOURCE_ID_T)(BMP_SECONDMENULINE_BG)
/*background picture of  normal menu with a scrollbar customed by user */
#define  MENU_ITEM_BG_PIC_VS_SB_BY_USER     (RM_RESOURCE_ID_T)(BMP_SECONDMENULINE_BG)
/*background picture of  focused menu customed by user */
#define  MENU_ITEM_FOCUS_PIC_BY_USER        (RM_RESOURCE_ID_T)(BMP_SUB_MAINMENU_FOCUS_BAR_C1)
/*background picture of  focused menu with a scrollbar customed by user  */
#define  MENU_ITEM_FOCUS_PIC_VS_SB_BY_USER  (RM_RESOURCE_ID_T)(BMP_SUB_MAINMENU_FOCUS_SHORT_BAR_C1)
/*=== define menu item background = END ==*/

/* interval between title and menu item */
#define   MENU_TITLE_AND_ITEM_OFFSET        0

/* indicates if show scrollbar in menu */
#define  MENU_SHOW_SCROLLBAR

/* define the font color of disable menu item */
#define  MENU_ITEM_DISABLE_COLOR            COLOR_RED_GRAY

/* specify if drawing disabled menu item without shadow */
#define  MENU_ITEM_DISABLED_WITH_SHADOW

/* indicates if draw the index of menu item with background picture by owner-draw mode, used in Odin etc. */
//#define MENU_ITEM_DRAW_INDEX_BY_OWNER_WITH_BG_PIC

/* define the focus index icon resource base in menu */
#define MENU_ITEM_INDEX_FOCUS_ICON_START    ICON_LINE_NUM_C1_1
/* define the normal index icon resource base in menu */
#define MENU_ITEM_INDEX_ICON_START          ICON_LINE_NUM_C1_1

/* indicates if menu item string scrolling periodically if it exceed the display area, used in B1 */
#define  MENU_ITEM_STRING_SCROLL_PERIODICALLY

/* indicates drawing the menu string in large font */
#define MENU_ITEM_STRING_USE_LARGE_FONT

/* the seperator between large font and small font.
 * if smaller than FONT_SIZE_SEPERATOR, the font is small size font,
 * or the font is big size font
 */
#define FONT_SIZE_SEPERATOR                 14 

/* Default menu font size */
#define DEFAULT_MENU_FONT_SIZE              ( FONT_SIZE_SEPERATOR + 2 )
/* Default font size */
#define DEFAULT_FONT_SIZE                   ( FONT_SIZE_SEPERATOR + 2 )
/* Default textinput font size */
#define DEFAULT_TEXT_INPUT_FONT_SIZE        ( FONT_SIZE_SEPERATOR + 2 )


/* The font heiht used in brs include 2 pixels space */
#define MSF_WIDGET_FONT_HEIGHT              18

/* height of an item in a menu selectgroup  */
#define   CHOICE_MENU_LINE_HEIGHT           26
/* height of an item in a main menu selectgroup  */
#define   CHOICE_MAIN_MENU_LINE_HEIGHT      27
/* border width of an item in a menu */
#define   MENU_LEFT_BORDER                  8
/* height of  a line text in a gadget,  invalved in font size */
#define   GADGET_LINE_HEIGHT                16
/* width of  a char in a gadget,  invalved in font size, and used to horizontal/vertical scollbar, etc. */
#define   GADGET_COLUMN_WIDTH               16
/* height of an item in a normal selectgroup */
#define   CHOICE_LINE_HEIGHT                17

/* default width or height of a image gadget */
#define   DEFAULT_IMAGEGADGET_HEIGHT        16
/* default width or height of a bitmap animation gadget */
#define   DEFAULT_BMPANIMATION_HEIGHT       16

/* default height of a datetime gadget */
#define   DEFAULT_DATETIMEGADGET_HEIGHT     ( DEFAULT_GADGET_HEIGHT * 2 )
/* default width of a button gadget */
#define   DEFAULT_BUTTON_WIDTH              68

/*===  Scrollbar == BEGIN ====*/
/* This scrollbar must be the same style with DS, LCD_HEIGHT_SB_ARROW
 * and the following size is gained from the scrollbar's resource bitmap/icon
 */
/* the width of a scrollbar */
#define   SCROLLBAR_WIDTH                   19
/* the height of a scrollbar's arrow */
#define   SCROLLBAR_ARROW_HEIGHT            26
/* the height of a scrollbar's spin */
#define   SCROLLBAR_SPIN_HEIGHT             18      

/* up arrow of vertical scrollbar */
#define   SB_V_ARROW_UP                     (RM_RESOURCE_ID_T)(ICON_WIN_SCROLL_ARROW_UP + ds_get_color_theme())
/* down arrow of vertical scrollbar */
#define   SB_V_ARROW_DOWN                   (RM_RESOURCE_ID_T)(ICON_WIN_SCROLL_ARROW_DOWN + ds_get_color_theme())
/* bar of vertical scrollbar */
#define   SB_V_BAR                          (RM_RESOURCE_ID_T)(ICON_WIN_SCROLL_BAR + ds_get_color_theme()) 
/* spin of vertical scrollbar */
#define   SB_V_SPIN                         (RM_RESOURCE_ID_T)(ICON_WIN_SCROLL_SPIN + ds_get_color_theme())

/* left arrow of horizontal scrollbar */
#define   SB_H_ARROW_LEFT                   (RM_RESOURCE_ID_T)(ICON_WIN_H_SCROLL_ARROW_LEFT + ds_get_color_theme()) 
/* right arrow of horizontal scrollbar */
#define   SB_H_ARROW_RIGHT                  (RM_RESOURCE_ID_T)(ICON_WIN_H_SCROLL_ARROW_RIGHT+ ds_get_color_theme())
/* bar of horizontal scrollbar */
#define   SB_H_BAR                          (RM_RESOURCE_ID_T)(ICON_WIN_H_SCROLL_BAR + ds_get_color_theme())
/* spin of horizontal scrollbar */
#define   SB_H_SPIN                         (RM_RESOURCE_ID_T)(ICON_WIN_H_SCROLL_SPIN + ds_get_color_theme()) 

/* Used for the pen event to enhance the pen sensitivity  */
#define PEN_SENSITIVITY                     4

/*===  Scrollbar == END ====*/

/*  The imm max height, mainly used to a focused textinput in paintbox  */
#define   WIDGET_IMM_MAX_HEIGHT             44

/*==== define the borders' width of dialog, it will determine the client area of the dialog ====*/
/* width of left border */
#define   DIALOG_LEFT_BORDER                26
/* width of right border */
#define   DIALOG_RIGHT_BORDER               8
/* width of top border */
#define   DIALOG_TOP_BORDER                 9
/* width of bottom border */
#define   DIALOG_BOTTOM_BORDER              9

/* only use the type picture as the dialog background picture */
//#define  DIALOG_BG_ONLY_TYPE_PICTURE

/* only use the background picture as the dialog background picture */
//#define  DIALOG_BG_ONLY_BACKGROUND_PICTURE

/* always show scrollbar in editor */
#define  ALWAYS_SHOW_SCROLLBAR_IN_EDITOR

/* The interval of the widget animation */
#define  WIDGET_ANIMATION_DELAY             500

/*==================================================================
  *       the waiting window layout information
  *==================================================================/
/* The x-coordinate of the start position of the gadgets in the waiting window  */  
#define WW_GADGET_POS_X                     4

/* The width of the gadgets in the waiting window */
#define WW_GADGET_WIDTH                     (DS_SCRN_MAX_X - WW_GADGET_POS_X * 2)

/*=== the layout information in using progressbar mode ===*/
/* the height of the prompt string  */
#define WW_PROGRESS_PROMPT_HEIGHT           32
/* the y-coordinate of the start position of the progressbar */
#define WW_PROGRESS_BAR_HEIGHT              20

          /***   the prompt string up    ***/
/* the y-coordinate of the start position of the prompt string */
#define WW_PROGRESS_PROMPT_UP_PROMPT_POS_Y  62
/* the y-coordinate of the start position of the progressbar */
#define WW_PROGRESS_PROMPT_UP_BAR_POS_Y     ( WW_PROGRESS_PROMPT_UP_PROMPT_POS_Y + WW_PROGRESS_PROMPT_HEIGHT + 4 )

          /***   the prompt string down    ***/
/* the y-coordinate of the start position of the progressbar */
#define WW_PROGRESS_PROMPT_DOWN_BAR_POS_Y   62
/* the y-coordinate of the start position of the prompt string */
#define WW_PROGRESS_PROMPT_DOWN_PROMPT_POS_Y  (WW_PROGRESS_PROMPT_DOWN_BAR_POS_Y + WW_PROGRESS_BAR_HEIGHT + 4 )

/*=== the layout information in using bitmap animation mode */
/* the height of the prompt string  */
#define WW_BMP_ANI_PROMPT_HEIGHT            20

          /***   the prompt string up    ***/
/* the y-coordinate of the start position of the prompt string */
#define WW_BMP_ANI_PROMPT_UP_PROMPT_POS_Y   20

          /***   the prompt string down    ***/
/* the y-coordinate of the start position of the prompt string */
#define WW_BMP_ANI_PROMPT_DOWN_PROMPT_POS_Y (WAP_SCRN_TEXT_HIGH - 40)

/* Indicates if ds define the ds_set_extend_softkey function. The extend softkey is used in idle,
 * if the idle support the extend softkey, the following macro must be defined
 */
#define DS_NEED_EXTEND_SOFTKEY

/*=== Input method definitions ===*/
/* specify if imm handles the '#' Key release event  */
//#define  IMM_HANDLE_KEY_POUND_RELEASE
/* specify if imm handles the '*' Key release event  */
//#define  IMM_HANDLE_KEY_STAR_RELEASE
/* specify if imm handles the '0' Key release event  */
//#define  IMM_HANDLE_KEY_0_RELEASE

/* the audio playing function, used in lowlevel.c and UBD_slideView.c */
#define AUDIO_PLAY                          SP_Audio_data_play_request


/*  max size of visible image  */
#define MAX_WIDTH_OF_VISIBLE_IMAGE              (LCD_MAX_X)
#define MAX_HEIGHT_OF_VISIBLE_IMAGE             (LCD_MAX_Y)

#endif

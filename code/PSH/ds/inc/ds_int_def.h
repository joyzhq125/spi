#ifndef DS_INT_DEF_H
#define DS_INT_DEF_H
/*==================================================================================================

    HEADER NAME : ds_int_def.h

    GENERAL DESCRIPTION
        This file contains Display system internal definitions.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/26/2002   Ashley Lee       crxxxxx     Initial Creation
    03/06/2003   wangwei          PP00130     Set mew emeishan image .
                                              - add AMASK_MUTE. 
   05/03/2003    linda wang       P000202     fix bug 1026.
                                              - set battary max item.
   06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
   08/04/2003   Chenyu           P001129     Add MMS icon
    12/04/2003   linda wang       p002147     Add qq and push image
	02/09/2004   Liyingjun        p002449     Adjust default line hieght
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/16/2004   lindawang        C002667     Mofify drawing dialing font.
    03/19/2004   chouwangyun      c002692     create new style popup
    04/13/2004   lindawang        p002976     Add deactive gprs annunciator.
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    06/15/2004   liuqi           c006194       add AMASK_CSD
    07/01/2004   liren           p006614      add macro for 16*32
    07/08/2004   liren            c006711     change submenu for ODIN
This file contains Display system internal definitions.


====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "OPUS_typedef.h"
#include    "ds_def.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
#define DEF_LINE_HEIGHT              (LCD_EXT_TEXT_HIGH / DS_SCRN_MAXLINES)     /* 128 /8 = 16 */
#define POPUP_LINE_HEIGHT            16 /* height of a popup line */
#define C_WIDTH                       8   /* width of a charcter */
#define FONTX_SMALL                  6
#define FONTX_NORMAL                 8
#define FONTY_SMALL                  12
#define FONTY_NORMAL                 16

/* define for dialing font, the font image is 8*16 */
#define FONT_DIAL_DISPLAY_WIDTH      13
#define FONT_DIAL_DISPLAY_HEIGHT     22
/* define for dialing font, the font image is 16*32 */

#define FONT_DIAL_L_DISPLAY_WIDTH    16
#define FONT_DIAL_L_DISPLAY_HEIGHT   32
#if (PROJECT_ID == ODIN_PROJECT)
#define LINE_HEIGHT                  22  /* height of a line */
#else 
#define LINE_HEIGHT                  23  /* height of a line */
#endif


#define SPACE                       ' '

#define CURSOR_OFF                    0xff
#define INDEX_NONE                    0xff

#define FONT_CHI_X_BYTES              2    /*  1<= widh <=16 bits */

#define RADIOBTN_ICON_X_OFFSET        (OP_INT16) 2
#define RADIOBTN_ICON_Y_OFFSET        (OP_INT16) 2
#define CHECKBOX_ICON_X_OFFSET         2
#define CHECKBOX_ICON_Y_OFFSET         2
#define LINE_ICON_X_OFFSET             1
#define LINE_ICON_Y_OFFSET             1


/*****************************************************
     ANNUCIATOR MASK - showing displaying status
*****************************************************/
/* The ANNUCIATOR masks and DS_ANNUN_ENUM_T are closely related to each other.
   The orders of these two definitions should be consistant and the mask bits are should be
   consistantly ordered */
#define    AMASK_RSSI                     0x00000001

#define    AMASK_MSG                      0x00000002
#define    AMASK_MMS                      0x00000004
#define    AMASK_QQ                       0x00000008
#define    AMASK_PUSH                     0x00000010
#define    AMASK_VMAIL                    0x00000020

#define    AMASK_SMS_PHONE_FULL           0x00000040
#define    AMASK_SMS_SIM_FULL             0x00000080
#define    AMASK_SMS_ALL_FULL             0x00000100

#define    AMASK_CONNECTED                0x00000200
#define    AMASK_MISSED_CALL              0x00000400
#define    AMASK_DIVERT                   0x00000800
#define    AMASK_CSD                       0x00001000


#define    AMASK_VIB                       0x00002000
#define    AMASK_NORMAL                   0x00004000                    
#define    AMASK_ALOUD                    0x00008000           
#define    AMASK_EARPHONE                 0x00010000          
#define    AMASK_INMOBILE                  0x00020000           

#define    AMASK_MELODY                    0x00040000
#define    AMASK_VIB_BELL                   0x00080000  
#define    AMASK_SILENCE                    0x00100000
#define    AMASK_MUTE                       0x00200000  /* useless */

#define    AMASK_ALARM                      0x00400000  
#define    AMASK_ROAM                       0x00800000   /* useless */

#define    AMASK_GPRS_INACTIVE             0x01000000      
#define    AMASK_GPRS                       0x02000000
#define    AMASK_BATT                       0x04000000


#define ANNUN_ALL_ON                   0xffffffff         
#define ANNUN_ALL_OFF                  0x00000000

#define ANNUN_FULL_RSSI                7
#define ANNUN_FULL_BATT                5

typedef OP_UINT32                      ANNUN_MASK;

typedef OP_INT8                        DS_POPUP_HANDLE;
typedef OP_UINT8                       POPUPATTR;


/*==================================================================================================
    MACROS
==================================================================================================*/


/*==================================================================================================
    TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/

/* To display partial highlight - like cursor */
typedef struct                       
{                    
    OP_UINT8    offset;                     /* index (x) of the charcter in the line to be highlighted. */
    OP_UINT8    line;                       /* Line number to be highlighted. 0-based */
    DS_FONTATTR attr;
    OP_BOOLEAN  blink;
    OP_BOOLEAN  blink_status;
    DS_COLOR    fontcolor;
    DS_COLOR    backcolor;
    OP_UINT8    cursor[DS_SCRN_MAX_LINE_UC_LEN+2];    /* Character cursor */
} CURSOR_T;

typedef enum
{
    POPUP_CONFIRM,
    POPUP_WARNING,
    POPUP_INFORMATION,
    POPUP_ERROR
}  DS_POPUP_ENUM_STYLE;

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/* Popup state variables */
typedef struct tagPOPUPWIN 
{
    DS_POPUP_ENUM_STYLE           style;
    DS_POPUP_TYPE_ENUM_T         type;            /* popup window type */
    DS_POPUP_HANDLE              handle;        /* popup window handle */
    DS_POPUP_HANDLE              prevPopup;     /* previous popup window handle */
    DS_POPUP_HANDLE              nextPopup;     /* next popup window handle */
    POPUPATTR                    attr;          /* popup window attributes */
    DS_POPUP_STATE_ENUM_T        state;         /* popup window minor state */
    OP_BOOLEAN                   update;        /* update flag */
    OP_BOOLEAN                   hsdGhostPopup; /* indicate popup that the previously closed 
                                                     but not cleared from the LCD exists */
    OP_UINT8                     nitems;        /* the number of items */
    OP_UINT8                     max_lines;     /* popup maximum lines except the title */
    OP_UINT8                     indexFirst;    /* first index (menu & list) */
    OP_UINT8                     indexFocused;  /* current index (cursor) */
    OP_UINT8                     indexSel;      /* selected index (radio button) */
    OP_UINT32                    indexCheck;    /* checked index (check box; item per bit; LSB is index 0) */
    OP_UINT8                     title[DS_SCRN_POPUP_MAX_UC_LEN+2];    /* popup window title */
    OP_UINT8                     msg[DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE+2]; /* message to be displayed */
    DS_POPUP_NUMEDITOR_T         *numeditor;  /* data of POPUP_NUMEDITOR */
    RM_ICON_T                    *icon;         /* icon */
    DS_POPUP_LIST_T              *list;         /* popup window list */
    DS_POPUP_MENU_TIER_T         *menu;         /* popup menu */
    OP_UINT16                    event;         /* event */
    OP_INT32                     timer;         /* popup window timer */
    DS_RECT_T                    pos;           /* popup window position */
    DS_RECT_T                    pos_ghostPopup;  /* position of the previous popup window - can be accumulated */
    DS_POINT_T                   point;         /* popup window start point */
    DS_RECT_T                    rcUpdate;      /* retangular region to be updated in popup_screen */
    OP_UINT32                    userData;      /* user-defined data */
    DS_SKEYSET_T                 skey;          /* popup window softkey set */
    OP_BOOLEAN                   bHasScrollBar; /* popup windwos has scrollbar*/
    OP_UINT8                     offsetScrollbar;/* popup windwos scrollbar offset*/
#ifdef RECOVER_AFTER_POPUP
    DS_SKEYSET_T                 skey_backup;  /* previous soft key, it will be recovered when the popup is closed */
#endif

    /* event handler function*/
    OP_BOOLEAN (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event );  
    /* user data hanlder funtion*/
    OP_UINT32 (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data ); 
    /* drawing function for owner-draw popup */
    void (*ownerdraw_func)( struct tagPOPUPWIN *pw, DS_POPUP_DRAW_INFO_T *dpu ); 
} DS_POPUPWIN_T;

/**************************************************************************************************
    DISPLY SYSTEM STATE VARIABLES DEFINITION 
**************************************************************************************************/

typedef struct 
{
    OP_BOOLEAN                   disp_annun;        /* indicate the annuciator icons should be displayed */
    OP_BOOLEAN                   bg_updated;        /* indicate the background image on bitmap_screen
                                                       is updated */
    OP_UINT8                     update_area;       /* Update area flag */
#ifdef RECOVER_AFTER_POPUP
    OP_UINT8                     last_update_area;  /* lastly refreshed background update area. 
                                                       These areas area recovered from the scrambling 
                                                        when the last popup is closed. */
#endif

    OP_BOOLEAN                   (*refresh_func)(OP_UINT32);  /*  own drawing function pointer */
                                                    /* TRUE when required to update LCD */

    OP_UINT8                     lines;             /* text line count currently displayed  */
    OP_UINT8                     items;             /* graphical item count currently displayed */
    OP_UINT8                     max_lines;         /* maximum line numbers can be displayed currently */
    DS_LINEDATA_T                title;             /* Title used in list, menu and drawing functions.*/ 
    DS_LINEDATA_T                line[DS_SCRN_MAXLINES];    /* text line data (text data) */
    DS_GITEMDATA_T               item[DS_SCRN_MAXITEMS];    /* graphical item data (non-text data) */
    
    OP_UINT8                     focused_line;      /* highlighted or partialy cursored  line. 0-based */
    
    OP_UINT8                     rssi_level;        /* indicating rssi bar level */
    OP_UINT8                     batt_level;        /* indicating battery bar level */
    ANNUN_MASK                   annun_mask;        /* annucitors mask indicating annun.displaying status */
    ANNUN_MASK                   annun_blink_mask;  /* blinking annuciator mask bit */
    OP_BOOLEAN                   blink_annun;       /* indicate whether at least one of annuciator is blinkging */
    OP_BOOLEAN                   blink_annun_status;  /* blinking status - ON/OFF */
    DS_SCRNMODE_ENUM_T           scrnMode;          /* Predefined Screen mode or ownerdarw mode */
    DS_SCREENATTRS_T             scrnAttr;          /* Attribute of each screen */

    DS_COLOREDCOMPOSER_T         cComposer;         /* user selected colors for each composer */

    CURSOR_T                     cCursor;           /* character cursor data - partial highlight */
  
    DS_SKEYSET_T                 skey;              /* sofk key data */
    DS_POPUPWIN_T                *popwin;           /* pop-up window pointer */
    OP_UINT8                     cnt_popup;         /* num. of popup win currently used -active or inactive */

    
    DS_RECT_T                    rcWin;             /* window rect */
    DS_RECT_T                    rcUpdate;          /* retangular region to be updated */

    DS_ANI_T                     ani;                /* animation play info. */
} SCRN_VAR_T;


extern SCRN_VAR_T ds;                               /* display system state variables */


/*================================================================================================*/
#endif  /* DS_INT_DEF_H */

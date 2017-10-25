#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    MODULE NAME : ds_popup.c

    GENERAL DESCRIPTION
        This file includes functions handling popup windows.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    07/03/2002   Ashley Lee       crxxxxx     Initial Creation
    01/14/2003   Steven Lai       PR-PP00007  Changed the method to count the line count for popup message. Correct the mistake in displaying a string containing '\n'
    01/17/2003   Steven Lai       PP00048     Fixed the bug of setting numeditor->pos in ds_popup_numeditor() and ds_popup_numeditor_rm()
    01/23/2003   Steven Lai       PP00046     Fixed the bug in draw_popup_window()
    02/08/2003   Steven Lai       PP00070     if the popup window is popup_numeditor and user hadnot input any digital, donot reponse to KEY_OK
    02/11/2003   Steven Lai       PP00071     Change the action of invoking callback function in case of Popup menu
    02/12/2003   Steven Lai       PP00072     Fix the bug that the string dosen't display correctly in popup numeditor when it is password and user press left arrow key
    03/03/2003   Steven Lai       PP00146     Fixed the bug that cannot press number key to access menu item in popup menu 
    03/21/2003   Steven Lai       P000005     Stop OPUS_TIMER_POPUP_MSG when popup_msg is closed
    05/15/2003   linda wang       P000289     modify the popup bg size.
    06/14/2003   linda wang       P000608     Fix some popup and smart list focus number problems. 
    07/07/2003   linda wang       P000814     use UstrNullcpy() instead of Ustrcpy() when copy ustr.
    09/28/2003   zhangxiaodong    P001738     not process the OPUS_FOCUS_OPERATION event to the popup_msg
    10/15/2003   Zhangxiaodong    P001801     process pen focus softkey left for popup
    10/31/2003   Zhangxiaodong    P001928     fix popup scrollbar page down bug.
    03/12/2004   Penghaibo        P002619     fix popup msg display position
    03/19/2004   chouwangyun      c002692     create new style popup
    03/23/2004   chouwangyun      p002716     settle static popup problem
    03/26/2004   chouwangyun      p002749     change popup softket
    04/13/2004   chouwangyun      C002941     revise popup function
    04/19/2004   lindawang        P003037     add extend softkey area function.
    04/21/2004   chouwangyun      p005039     fix bug about word problem in popup
    04/29/2004   lindawang        P005196     Fix bug for draw softkey.    
    03/26/2004   chouwangyun      p002749   change popup softket
    05/12/2004   chouwangyun      p002882   settle bug about popup word
    08/04/2004   liren            p007506     change popup softkey display in idle
    08/18/2004   liren            p007822     change the default height of popup window    
   This file includes functions handling popup windows.
   
        
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
#include    "uhapi.h"

#include "apitypes.h"
#include    "KEY_table.h"

#include    "keymap.h"
#include    "ds_popup.h"
#include    "gsm_timer.h"

#include    "TEXT_display.h"
#include    "TP_Operator.h"
/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
#define DS_NONE_EVENT         0xffff /* temporal DS event used in popup */

/* to convert 32bits OPUS_TIMER_KEYPRESS_REPEAT to ds intenal 16 bits event */ 
#define  DS_POPUP_KEYPRESS_REPEAT_EVENT       0xffff

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
/* Popup window maximum size */
#ifdef MEM_EFFECTIVE_POPUP_BUF 
#define POPUP_MAX_WIDTH                   117  /* determined by design */
#define POPUP_MAX_HEIGHT                  88  /* determined by design */
#endif

#ifdef POPUP_TITLE
#define POPUP_Y_ABOVE_TITLE_MARGIN        2
#define POPUP_Y_BELOW_TITLE_MARGIN        2      /* for title shadow */
#define POPUP_Y_ABOVE_FIRSTLINE_MARGIN    4
#define POPUP_Y_BELOW_LASTLINE_MARGIN     4
#endif
   

//#define POPUP_DEFAULT_WIDTH              122  /* determined by design */
#define POPUP_DEFAULT_HEIGHT             68  /* determined by design */
#define POPUP_DEFAULT_TOP_MARGINE       10    /* margine over the first line */
#define POPUP_DEFAULT_LEFT_MARGINE     10
#define POPUP_DEFAULT_RIGHT_MARGINE    10
#define POPUP_FOCUSBAR_WIDTH           100
#define POPUP_FOCUSBAR_HEIGHT          14
#define POPUP_SCROLL_TOP_MARGINE    5
#define COLOR_POPUP_FOCUSBAR              COLOR_LIGHT_LEMON_YELLOW



#define POPUP_DEFAULT_BOTTOM             (POPUP_DEFAULT_TOP+POPUP_DEFAULT_HEIGHT-1)    /* desided by design */
#define POPUP_DEFAULT_RIGHT              (POPUP_DEFAULT_LEFT+POPUP_DEFAULT_WIDTH-1)    /* desided by design */
#define POPUP_DEFAULT_ICON_MARGIN        2
#define POPUP_DEFAULT_ICON_WIDTH        26
#define POPUP_DEFAULT_TEXT_ADDLIMIT    18    
#define POPUP_MAXWORD_PER_LINE        6*2
#define POPUP_DEFAULT_CENTRE_MARGIN    24
#define POPUP_BACKGROUND_HEIGHT        68

#define POPUP_CENTRAL_MSGTEXT_VERTICAL    8

#define FONTX_POPUP                      12  /* desided by design */
#define POPUP_MAX_ITEM                   4  /* desided by design *, it was 3*/
#define POPUP_ICON_WIDTH                 12
#define POPUP_FONTY                       FONTY_SMALL

/* Maximum popup window at once */
#define POPUP_MAX_WIN                     LCD_MAX_POPUP_WIN 

/* Allowed popup window data */
static DS_POPUPWIN_T popWin[POPUP_MAX_WIN];

/* to remember lastly stored digit position */
static OP_UINT8    last_numeditor_pos = 0;
/* to clear all entered digits, remember last key was clear key in numeditor. */
static OP_BOOLEAN  clear_key_pressed = OP_FALSE;

/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/

/* check whether it is currenlty displayed in the popup window */
/* idx : index, sIdx : start index, mLns : max lines */
#define IS_IN_SCREEN(idx, sIdx, mLns) (((idx>=sIdx) && (idx<(sIdx+mLns))) ?  OP_TRUE : OP_FALSE)

/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void draw_popup_window_frame
(
    OP_INT16      left, 
    OP_INT16      top
);

static void draw_popup_window 
( 
    DS_POPUPWIN_T           *popwin, 
    DS_POPUP_DRAW_INFO_T    *dpu 
);

static DS_POPUP_HANDLE alloc_popup_window 
( 
    void 
);

static DS_POPUP_HANDLE close_popup_window 
( 
    DS_POPUP_HANDLE    handle 
);

static DS_POPUPWIN_T *create_popup 
(
    DS_POPUP_DATA_T    *p_data
);

static void open_popup 
( 
    DS_POPUPWIN_T *popwin 
);

static void close_popup_which 
( 
    DS_POPUPWIN_T    *popwin 
);

static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_msg 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
);

static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_menu 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
);

static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_list 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
);

static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_numeditor 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
);


static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_ownerdraw 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
);

static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_dialog 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
);

static DS_POPUP_PDEH_RET_ENUM_T pass_popup_event 
(   
    DS_POPUPWIN_T    *pw,
    OP_UINT16         event 
);


/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: draw_popup_window_frame

    DESCRIPTION:
        Draw system defined popup window frame. 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the previous popup window handle if exist, otherwise returns PHANDLE_NONE. 
        
    IMPORTANT NOTES:

==================================================================================================*/
static void draw_popup_window_frame
(
    OP_INT16      left, 
    OP_INT16      top
)
{ 
    draw_bitmap_image(AREA_POPUP,
                left, 
                top,
                util_get_bitmap_from_res(BMP_POPUP_BACKGROUND));
}

/*==================================================================================================
    FUNCTION: draw_popup_window

    DESCRIPTION:
        Draw popup window according to the popup type, popup data in the system defined style. 
        The size of the popup is fixed by the design.

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the previous popup window handle if exist, otherwise returns PHANDLE_NONE. 
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static void draw_popup_window 
( 
    DS_POPUPWIN_T           *popwin, 
    DS_POPUP_DRAW_INFO_T    *dpu 
)
{

    OP_BOOLEAN           is_shadow_text;
    OP_BOOLEAN           refresh_whole_popup = OP_TRUE;
    OP_UINT8             max_items;
    OP_INT16             i;
    OP_INT16             leng  = 0;
    OP_INT16             x_pos;
    OP_INT16             y_pos;
    OP_INT16             x_base; // x base coor of text and icon
    OP_INT16             y_base; // y base coor of the line
    OP_INT16             x_limit; // x limit coor of text 
    OP_INT16             x_focus; // x base coor of focus bar
    OP_INT16             x_focus_limit; // x limit coor of focus bar
    OP_INT16             font_width;
    DS_FONTATTR          fontattr;
    OP_UINT8             *item_txt = OP_NULL;
    RM_RESOURCE_ID_T     res_id;
    DS_POPUP_TYPE_ENUM_T popup_type = popwin->type;

    OP_UINT16           line_width[POPUP_MAX_ITEM]; /* the number of chars in each line */
  
    max_items = POPUP_MAX_ITEM;
    font_width =  font_get_font_width_w_fattr(ds.scrnAttr.pop.fontAttr);
    
    /* initial popup update, determine the popup window size according to the popup type  */
    if (dpu->type == DPU_INIT) 
    {
        /* determine window dimension */        
        /* user leaves the popup position to the system */
        if (!(popwin->attr & PUA_POSITION)) 
        {
            if (popwin->attr & PUA_POINT && popwin->point.x > 0)
            {
                popwin->pos.left  = popwin->point.x;
                popwin->pos.top = popwin->point.y;

            }
            else
            {
                popwin->pos.left  = POPUP_DEFAULT_LEFT;
                popwin->pos.top   = POPUP_DEFAULT_TOP;
            }

            popwin->pos.right = POPUP_DEFAULT_RIGHT;
            popwin->pos.bottom = POPUP_DEFAULT_BOTTOM;

            if (ds_get_disp_annun())
            {
                popwin->pos.top -= LCD_ANNUN_HIGH;
                popwin->pos.bottom -= LCD_ANNUN_HIGH;       
            }
        }

        if  ((popup_type == POPUP_MENU) || (popup_type == POPUP_LIST) )
        {
            /* get max lines of the popup */
            popwin->max_lines = MIN(max_items, popwin->nitems);
        }
        else if ((popup_type == POPUP_MSG)||(popup_type == POPUP_DIALOG))
        {
            OP_UINT8         nLines = 0;
            OP_UINT16       nFrom = 0, 
                                   nNext  = 0;
         x_pos = popwin->pos.left;
         popwin->pos.left= (LCD_MAX_X - POPUP_DEFAULT_WIDTH)/2  ;  
         x_pos = popwin->pos.left - x_pos;
         popwin->pos.right += x_pos;

         popwin->pos.top +=  POPUP_DEFAULT_CENTRE_MARGIN;
         popwin->pos.bottom += POPUP_DEFAULT_CENTRE_MARGIN;


            while(TEXT_get_nextpos(popwin->msg, nFrom, POPUP_MAXWORD_PER_LINE, &nNext) == OP_TRUE)
            {
                line_width[nLines]=nNext -nFrom;
                nFrom = nNext;
                nNext = 0;
                nLines++;
                if(nLines >= POPUP_MAX_ITEM)
                {
                    break;
                }
            }
            leng = UstrCharCount(popwin->msg);
            popwin->max_lines = MIN(max_items,nLines);
           
        }
        else if (popup_type != POPUP_OWNERDRAW) 
        {
           popwin->max_lines = max_items;
        }
        
    }


    is_shadow_text = (ds.scrnAttr.pop.fontAttr & FONT_SHADOW) ? OP_TRUE : OP_FALSE;
    fontattr = ds.scrnAttr.pop.fontAttr;
    fontattr |= FA_MENUDEFAULT;

    x_base = (OP_INT16)(popwin->pos.left);
    y_base = (OP_INT16)(popwin->pos.top + POPUP_DEFAULT_TOP_MARGINE);
    x_limit = (OP_INT16)(popwin->pos.right + POPUP_DEFAULT_TEXT_ADDLIMIT);
    x_focus = (OP_INT16)(x_base - 2);
    x_focus_limit = (OP_INT16)(x_focus + POPUP_FOCUSBAR_WIDTH-1);
     

    switch (popup_type) 
    {
         case POPUP_MSG:
         case POPUP_DIALOG:
         /* case POPUP_DIALOG: */
         {
             OP_BOOLEAN  is_first_line;
             OP_UINT8    i;
             OP_INT16  posleft, postop, width, height, iconwidth, iconheight;
             OP_INT16    current_from=0;
             OP_UINT8    msg_ptr[DS_SCRN_POPUP_MAX_UC_LEN+2];
       
             if (dpu->type != DPU_INIT && dpu->type != DPU_REDRAW)
             {
                 break;
             }
         
             /* draw new msg popup window frame */
             draw_popup_window_frame(popwin->pos.left, 
                                        popwin->pos.top);
             /* set softkey higher before draw it */
             ds_set_extend_softkey(OP_FALSE);

             posleft = popwin->pos.left+POPUP_DEFAULT_ICON_MARGIN;
             postop = popwin->pos.top+POPUP_DEFAULT_ICON_MARGIN;      
             switch (popwin->style)
             {
                 case POPUP_CONFIRM:
                     draw_bitmap_image(AREA_POPUP, posleft, postop, 
                         (RM_BITMAP_T *)util_get_bitmap_from_res((RM_RESOURCE_ID_T)BMP_POPUP_CONFIRM));
            /* arranges default popup softkeys */
                     ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_MSG_YES, PMPT_MSG_NO);

                 break;

                 case POPUP_WARNING:
                     draw_bitmap_image(AREA_POPUP, posleft, postop, 
                         (RM_BITMAP_T *)util_get_bitmap_from_res((RM_RESOURCE_ID_T)BMP_POPUP_WARNING));
                     if (popwin->timer <= 0)
                     {
                        ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_SKEY_EMPTY, PMPT_SKEY_BACK);
                     }
                     else
                     {
                        if(SCREEN_IDLE != ds_get_screen_mode())
                        {
                            ds_set_softkeys_rm(PMPT_SKEY_EMPTY, PMPT_SKEY_EMPTY, PMPT_SKEY_EMPTY);
                        }
                     }
                 break;

                 case POPUP_INFORMATION:
                     draw_bitmap_image(AREA_POPUP, posleft, postop, 
                         (RM_BITMAP_T *)util_get_bitmap_from_res((RM_RESOURCE_ID_T)BMP_POPUP_INFORMATION));
                     if (popwin->timer <= 0)
                     {
                        ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_SKEY_EMPTY, PMPT_SKEY_BACK);
                     }
                     else
                     {
                        ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_SKEY_EMPTY, PMPT_SKEY_EMPTY);
                     }
                 break;

                 case POPUP_ERROR:
                     draw_bitmap_image(AREA_POPUP, posleft, postop, 
                         (RM_BITMAP_T *)util_get_bitmap_from_res((RM_RESOURCE_ID_T)BMP_POPUP_ERROR));
                     if (popwin->timer <= 0)
                     {
                        ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_SKEY_EMPTY, PMPT_SKEY_BACK);
                     }
                     else
                     {
                        ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_SKEY_EMPTY, PMPT_SKEY_EMPTY);
                     }
                 break;

                 default:
                     draw_bitmap_image(AREA_POPUP, posleft, postop, 
                         (RM_BITMAP_T *)util_get_bitmap_from_res((RM_RESOURCE_ID_T)BMP_POPUP_INFORMATION));
                     if (popwin->timer <= 0)
                     {
                        ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_SKEY_EMPTY, PMPT_SKEY_BACK);
                     }
                     else
                     {
                        ds_set_softkeys_rm(PMPT_SKEY_EMPTY,     PMPT_SKEY_EMPTY, PMPT_SKEY_EMPTY);
                     }
                 break;
            }


#ifdef POPUP_TITLE
             /* draw title */
             if (IS_NOT_NULL_STRING(popwin->title)) 
             {
                 leng = MIN(DS_SCRN_POPUP_CWIDE, Ustrlen(popwin->title));
                 /* Steven Lai 01/23/2003  there is some bug in the following code, x_pos and y_pos are used without set their values firstly */
                 draw_text_line(AREA_POPUP, 
                                  (OP_INT16)(x_pos + (WIDTH(popwin->pos)-leng*FONTX_POPUP)/2), 
                                   y_pos, 
                                  (OP_INT16)(popwin->pos.right - POPUP_DEFAULT_RIGHT_MARGINE),
                                  (OP_UINT8 *)popwin->title, 
                                  OP_TRUE, 
                                  FA_POPTITLE, 
                                  COLOR_POPUPFG, 
                                  COLOR_POPUPBG);
                 y_pos += (OP_INT16)(POPUP_LINE_HEIGHT + POPUP_Y_ABOVE_FIRSTLINE_MARGIN);
             }
#endif

             y_pos = (OP_INT16)(y_base + 1);
             is_first_line = OP_TRUE;

             if(ds_get_image_size(BMP_POPUP_BACKGROUND, (OP_UINT16 *)&width, (OP_UINT16 *)&height)) 
             {
                    ;
             }
             else
             {
                    height = POPUP_BACKGROUND_HEIGHT;
             }
             y_pos = popwin->pos.top + height/(popwin->max_lines+1) - POPUP_CENTRAL_MSGTEXT_VERTICAL;

             if(ds_get_image_size(BMP_POPUP_INFORMATION, (OP_UINT16 *)&iconwidth
                                                , (OP_UINT16 *)&iconheight)) 
             {
                    ;
             }
             else
             {
                    iconwidth = POPUP_DEFAULT_ICON_WIDTH;
             }
             /* begin to write text, with icon already present, so x_base is rearranged */
             x_limit += POPUP_DEFAULT_TEXT_ADDLIMIT;
             for (i = 0; i < popwin->max_lines; i++)
             {                    
                 Ustrncpy(msg_ptr, &popwin->msg[current_from*2], (line_width[i])*2);
                 msg_ptr[line_width[i]*2+0]=0;
                 msg_ptr[line_width[i]*2+1]=0;
                 leng = UstrSize(msg_ptr);
                 if (leng < DS_SCRN_POPUP_CWIDE)
                 {
                     /* align center */
                      if ( popwin->max_lines == 1)
                      {
                          if(!line_width[i]%2)
                          {
                                x_pos = popwin->pos.left + (popwin->pos.right - 
                                             popwin->pos.left)/2 - font_width*leng/2 - font_width;
                          }
                          else
                          {
                                x_pos = popwin->pos.left + (popwin->pos.right - 
                                             popwin->pos.left)/2 - font_width*leng/2 ;    
                          }

                          x_pos = MAX(x_pos , popwin->pos.left + iconwidth + POPUP_DEFAULT_ICON_MARGIN);
                      }
                      else
                      {
                            x_pos = popwin->pos.left + iconwidth + POPUP_DEFAULT_ICON_MARGIN;
                      }
                 } 
                 else
                 {
                     x_pos = x_base;                           
                 } 

                 if (is_shadow_text)
                 {
                     /* draw text shadow */
                     draw_text_line(AREA_POPUP,
                                 (OP_INT16)(x_pos), 
                                y_pos, 
                                 (OP_INT16)(x_limit),
                                msg_ptr, 
                                is_first_line,
                                fontattr, 
                                COLOR_POPUPSHADOW, 
                                COLOR_POPUPBG);
                 }
    
                 draw_text_line(AREA_POPUP,
                                    x_pos, 
                                    y_pos, 
                                    x_limit,
                                    msg_ptr, 
                                    is_first_line,
                                    fontattr, 
                                    COLOR_POPUPFG, 
                                    COLOR_POPUPBG);
                                                
                current_from += line_width[i];
                 y_pos += height/(popwin->max_lines+1);  
                is_first_line = OP_FALSE;
             }              
         }

         break;
      
         case POPUP_MENU:                 
         case POPUP_LIST:
         {      
              OP_UINT16           index; 
              DS_LINEATTR         lineattr;
              DS_COLOR            fontcolor;
              DS_COLOR            backcolor = COLOR_TRANS ; /* font back color */
              RM_ICON_T           *icon=OP_NULL;
              OP_UINT8            temp_text[DS_SCRN_POPUP_MAX_UC_LEN+2];    
              DS_POPUP_MENU_ENTRY_T const *index_menu;
        
             
              /* requested to draw the whole popup list/menu */
              if (dpu->type == DPU_INIT || dpu->type == DPU_REDRAW) 
              {          
                  /* draw new msg popup window frame */
                  draw_popup_window_frame(popwin->pos.left, 
                                             popwin->pos.top);

                 if (popwin->type == POPUP_LIST) 
                 {
                     if ( (popwin->list) && 
                           (popwin->list->items[0].attr == LA_CHECKBOX))
                     {
                         ds_set_softkeys_rm(PMPT_SKEY_CHECK, PMPT_SKEY_OK, PMPT_SKEY_BACK);   
                     } 
                     else
                     {
                         ds_set_softkeys_rm(PMPT_SKEY_EMPTY, PMPT_SKEY_OK, PMPT_SKEY_BACK);   
                     } 
                 }
                 else /* POPUP_MENU */
                 {
                     ds_set_softkeys_rm(PMPT_SKEY_EMPTY, PMPT_SKEY_OK, PMPT_SKEY_BACK);   
                 }                      
#ifdef POPUP_TITLE
                  /* draw list title text */
                  if (popwin->attr & PUA_USERLIST) 
                  {  /*  User passed the popup list data */
                      if (popwin->userdata_handler)
                      {
                          Ustrcpy(popwin->title,(OP_UINT8 *)(popwin->userdata_handler)(PUUD_TITLE, 0));
                      }
                  }
                
                 /* draw title */
                 if (IS_NOT_NULL_STRING(popwin->title)) 
                 {
                     x_pos = x_base;
                     y_pos = y_base;
                     /* draw title icon */
                     if (popwin->icon != OP_NULL)
                     {
                          draw_icon(AREA_POPUP,  
                                     (OP_INT16)(x_pos+LINE_ICON_X_OFFSET),
                                     (OP_INT16)(y_pos+LINE_ICON_Y_OFFSET), 
                                     (RM_ICON_T *)popwin->icon);    
                          x_pos += (OP_INT16)(popwin->icon->biWidth + LINE_ICON_X_OFFSET*3);
                     }   

                     
                     /* draw title text */
                     draw_text_line(AREA_POPUP,
                                      x_pos, 
                                      y_pos, 
                                      x_limit,
                                      (OP_UINT8 *)popwin->title, 
                                      OP_TRUE, 
                                      FA_POPTITLE, 
                                      COLOR_TITLEFG, COLOR_TITLEBG);
                 }
#endif
              }
              
              if (dpu->type == DPU_LISTLINE) 
              {
                  refresh_whole_popup = OP_FALSE;
              }
              ds_set_popup_scrollbar(popwin->nitems, popwin->indexFocused, max_items,
                  OP_TRUE);


             
#ifdef POPUP_TITLE
              if (IS_NOT_NULL_STRING(popwin->title))
              {
                  y_pos = (OP_INT16)(popwin->pos.top + POPUP_Y_ABOVE_TITLE_MARGIN + POPUP_LINE_HEIGHT + 
                                     POPUP_Y_BELOW_TITLE_MARGIN + POPUP_Y_ABOVE_FIRSTLINE_MARGIN);
              }
              else
              {
                  y_pos =  (OP_INT16)(popwin->pos.top + POPUP_Y_ABOVE_FIRSTLINE_MARGIN);
              }
              y_off = y_pos;
#endif

              y_pos = (OP_INT16)(y_base+1);
              /* draw list and menu items */
              for (i = 0; i < popwin->max_lines; i++, y_pos += POPUP_LINE_HEIGHT) 
              {
                  /* If requested the whole list update redraw the entire lines.
                     If requested one line update, update the specified line only */ 
                  if (dpu->type < DPU_LISTLINE || (dpu->type == DPU_LISTLINE && dpu->line == i)) 
                  {
                      index = (OP_INT16)(popwin->indexFirst + i);
                      x_pos = x_base;

                      if (popup_type == POPUP_LIST)
                      {
                          if (popwin->attr & PUA_USERLIST && popwin->userdata_handler) 
                          {
                              lineattr = (DS_LINEATTR)(popwin->userdata_handler)(PUUD_LINEATTR, index);
                              item_txt = (OP_UINT8 *)(popwin->userdata_handler)(PUUD_LINE, index);
                          }
                          else if (popwin->list) 
                          { /* otherwise, retreive the line data */
                              lineattr    = popwin->list->items[index].attr;
                              item_txt    = (OP_UINT8 *)popwin->list->items[index].txt;
                              icon        = util_get_icon_from_res(
                                                     popwin->list->items[index].icon_res_id);
                          }
                      }
                      else /* POPUP_MENU */
                      {
                          index_menu = (DS_POPUP_MENU_ENTRY_T const *)popwin->menu->menu_data_ptr;
                          item_txt = (OP_UINT8 *)index_menu[index].txt;
                      }

                      /* in case of the item string is null or list LA_OWNERDRAW attr, move to the next line. */
                      if ( (IS_NOT_NULL_STRING(item_txt) == OP_FALSE) || 
                            ( (popup_type == POPUP_LIST) && (lineattr & LA_OWNERDRAW)))
                      {
                          if (refresh_whole_popup == OP_FALSE)
                          {
                              ds_refresh();
                          }
                          continue;
                      } 


                      /* fill line background color */
                      /* focused line, draw highlight bar */
                      if(popwin->nitems > max_items)
                      {
                          
                          if (index == popwin->indexFocused) 
                          {
                              fill_rect(AREA_POPUP,
                                         x_focus,
                                         (OP_INT16)(y_pos-1),
                                         (OP_INT16)(x_focus_limit - POPUP_SCROLLBAR_WIDTH),
                                         (OP_INT16)(y_pos+POPUP_FOCUSBAR_HEIGHT-2),
                                         COLOR_POPUP_FOCUSBAR);
                              fontcolor = COLOR_CURSORFG; 
                          }
                          else
                          {
                              fill_rect(AREA_POPUP,
                                        x_focus,
                                        (OP_INT16)(y_pos-1),
                                        (OP_INT16)(x_focus_limit - POPUP_SCROLLBAR_WIDTH),
                                        (OP_INT16)(y_pos+POPUP_FOCUSBAR_HEIGHT-2),
                                        COLOR_POPUPBG);
                              fontcolor = COLOR_POPUPFG;
                          }                            
                      }    
                      else
                      {
                          
                          if (index == popwin->indexFocused) 
                          {
                                 fill_rect(AREA_POPUP,
                                           x_focus,
                                           (OP_INT16)(y_pos-1),
                                           (OP_INT16)x_focus_limit,
                                           (OP_INT16)(y_pos+POPUP_FOCUSBAR_HEIGHT-2),
                                           COLOR_POPUP_FOCUSBAR);
                                fontcolor = COLOR_CURSORFG; 
                          }
                          else
                          {
                                 fill_rect(AREA_POPUP,
                                           x_focus,
                                           (OP_INT16)(y_pos-1),
                                           (OP_INT16)x_focus_limit,
                                           (OP_INT16)(y_pos+POPUP_FOCUSBAR_HEIGHT-2),
                                           COLOR_POPUPBG);
                                 fontcolor = COLOR_POPUPFG;
                          }                            
                      }                                       
                                                                 
                      /* apply line attributes only to popup list */ 
                      if (popup_type == POPUP_LIST)
                      {
                          /* if user passed the list data and its handler is not null, 
                             let the user process the line */

                          /* Radio button */
                          if ((lineattr & LA_RADIOBUTTON) &&  (index < popwin->nitems)) 
                          {
                              OP_UINT8 check = 0;
                              if (index == popwin->indexSel && popwin->indexSel != INDEX_NONE)
                              {
                                  check = 1;
                              }

                              res_id = (RM_RESOURCE_ID_T)(ICON_POPUP_RADIOBTN_OFF + check);
                              draw_icon(AREA_POPUP, 
                                         (OP_INT16)GET_CENTER(x_pos, (x_pos+ POPUP_ICON_WIDTH-1),\
                                                                ds_get_image_width(res_id)), 
                                         (OP_INT16)GET_CENTER(y_pos, (y_pos+ POPUP_FONTY-1),\
                                                                ds_get_image_height(res_id)), 
                                         (RM_ICON_T *)util_get_icon_from_res(res_id));
                              x_pos += POPUP_ICON_WIDTH;              
                          }
                          /* check box */
                          else if (lineattr & LA_CHECKBOX) 
                          { 
                              OP_UINT8 check = (popwin->indexCheck & (1<<index)) ? OP_TRUE:OP_FALSE;
                              res_id = (RM_RESOURCE_ID_T)(ICON_POPUP_CHECKBOX_OFF + check);
                              draw_icon(AREA_POPUP, 
                                         (OP_INT16)GET_CENTER(x_pos, (x_pos+ POPUP_ICON_WIDTH-1),\
                                                                ds_get_image_width(res_id)), 
                                         (OP_INT16)GET_CENTER(y_pos, (y_pos+ POPUP_FONTY-1),\
                                                                ds_get_image_height(res_id)), 
                                         (RM_ICON_T *)util_get_icon_from_res(res_id));
                              x_pos += POPUP_ICON_WIDTH;
                          }
                          
                          else if (lineattr & LA_LINENUMBER)
                          {
                              if (index == popwin->indexFocused) 
                              { 
                                  res_id = (RM_RESOURCE_ID_T)(ICON_POPUP_LINE_NUM_FOCUS_1+index);
                                  draw_icon(AREA_POPUP,
                                             x_pos, 
                                             (OP_INT16)GET_CENTER(y_pos, (y_pos+ POPUP_FONTY-2),\
                                                                    ds_get_image_height(res_id)), 
                                              util_get_icon_from_res(res_id)); 
                              }
                              else
                              { 
                                  res_id = (RM_RESOURCE_ID_T)(ICON_POPUP_LINE_NUM_1+index);
                                  draw_icon(AREA_POPUP,
                                             x_pos, 
                                             (OP_INT16)GET_CENTER(y_pos, (y_pos+ POPUP_FONTY-2),\
                                                                    ds_get_image_height(res_id)), 
                                              util_get_icon_from_res(res_id)); 
                              }
                              x_pos += POPUP_ICON_WIDTH;
                          } 
                          /* icon */
                          else if ((lineattr & LA_ICON) && ( icon != OP_NULL))
                          {                              
                              draw_icon(AREA_POPUP,  
                                         (OP_INT16)GET_CENTER(x_pos, (x_pos+ POPUP_ICON_WIDTH-1),\
                                                                icon->biWidth), 
                                         (OP_INT16)GET_CENTER(y_pos, (y_pos+ POPUP_FONTY-1),\
                                                                icon->biHeight), 
                                         (RM_ICON_T *)icon);  
  
                              x_pos += POPUP_ICON_WIDTH;
                          }
                                             
#ifdef NOT_SUPPORTED /* large font is not allowed in the popup */
                          if (lineattr & LA_LARGEFONT)
                          {
                              fontattr = FA_WINDEFAULT;  
                          }
#endif                                                                           
                          /* sub-item */
                          if (lineattr & LA_SUBITEM) 
                          { 
                              x_pos += FONTX_POPUP; /* indent one character width */
                          }
                          
                          /* draw list item text */
                            op_memset((void *) temp_text, 0x00,(DS_SCRN_POPUP_MAX_UC_LEN+2));
                          Ustrcpy(temp_text, (OP_UINT8 *)item_txt);        

                          /* shadowing text */
                          if (is_shadow_text)
                          {
                              draw_text_line(AREA_POPUP, 
                                             (OP_INT16)(x_pos+1),
                                             y_pos, 
                                             (OP_INT16)(x_limit +1),
                                             (OP_UINT8 *)temp_text, 
                                             OP_TRUE,                                              
                                             fontattr, 
                                             COLOR_POPUPSHADOW, 
                                             backcolor);
                          } 

                          draw_text_line(AREA_POPUP, 
                                         x_pos, 
                                         y_pos, 
                                         x_limit,
                                         (OP_UINT8 *)temp_text, 
                                          OP_TRUE,                                          
                                          fontattr, 
                                          fontcolor, 
                                          backcolor);

                      }                      
                      else if  (popup_type == POPUP_MENU)   /* Draw Menu item */
                      {
                          /* line number icon is displayed defaultly in popup menu */
                          if (index == popwin->indexFocused) 
                          { 
                              res_id = (RM_RESOURCE_ID_T)(ICON_POPUP_LINE_NUM_FOCUS_1+index);
                              draw_icon(AREA_POPUP,
                                         x_pos, 
                                         (OP_INT16)GET_CENTER(y_pos, (y_pos+ POPUP_FONTY-2),\
                                                                ds_get_image_height(res_id)), 
                                          util_get_icon_from_res(res_id)); 
                          }
                          else
                          { 
                              res_id = (RM_RESOURCE_ID_T)(ICON_POPUP_LINE_NUM_1+index);
                              draw_icon(AREA_POPUP,
                                         x_pos, 
                                         (OP_INT16)GET_CENTER(y_pos, (y_pos+ POPUP_FONTY-2),\
                                                                ds_get_image_height(res_id)), 
                                          util_get_icon_from_res(res_id)); 
                          }

                          x_pos += POPUP_ICON_WIDTH;

                          if (is_shadow_text)
                          {
                              /* shadowing text */
                              draw_text_line(AREA_POPUP, 
                                             (OP_INT16)(x_pos+1), 
                                             y_pos, 
                                             (OP_INT16)(x_limit+1),
                                             (OP_UINT8 *)index_menu[index].txt, 
                                             OP_TRUE,                                              
                                             fontattr, 
                                             COLOR_POPUPSHADOW, 
                                             backcolor);
                          }

                          draw_text_line(AREA_POPUP, 
                                         x_pos, 
                                         y_pos, 
                                         x_limit,
                                         (OP_UINT8 *)index_menu[index].txt, 
                                         OP_TRUE,                                          
                                         fontattr, 
                                         fontcolor, 
                                         backcolor);              
                      }
                   
                      
                      /* if it is not to refresh the popup entirely, update it line by line */
                      if (refresh_whole_popup == OP_FALSE)
                      {
                          /* DS keeps last update region. No need to call region update purposely */
                          ds_refresh();
                      }                      
                  }
              } // for loop
         }
             break;

        case POPUP_NUMEDITOR:      
        {
            OP_UINT8    digit_offset = 0;   /* digit offset start to be written */
            OP_UINT8    written_digits_leng = 0;   /* digit length to be written */
            OP_UINT8    cursor_pos     = 0;
            OP_UINT8    underline[2] = {0x5f, 0x00};
            OP_UINT8    asterisk[2] = {0x2a, 0x00};
            OP_UINT8    *cursor = OP_NULL;     /* point a charcter to be drawn with the cursor in POPUP_NUMEDIOR */
            OP_UINT8    asterisks[DS_SCRN_POPUP_MAX_UC_LEN+2];
            OP_UINT8    displayed_digits[DS_SCRN_POPUP_MAX_DIGIT_UC_LEN+2];
            OP_UINT8    displayed_len;

            op_memcpy(displayed_digits, popwin->numeditor->digits, sizeof(displayed_digits));

            if (dpu->type == DPU_INIT || dpu->type == DPU_REDRAW)
            {
                 /* draw popup window frame */
                 draw_popup_window_frame(popwin->pos.left, 
                                            popwin->pos.top);
    
                /* draw editor message */
                x_pos    = (OP_INT16)GET_CENTER(x_base, x_limit, UstrCharCount(popwin->numeditor->msg) * font_width);

                 y_pos = (OP_INT16)(y_base+1);
               
                 if (is_shadow_text)
                 {
                     /* draw text shadow */
                     draw_text(AREA_POPUP,
                                (OP_INT16)(x_pos+1), 
                                y_pos, 
                                (OP_INT16)(x_limit +1),
                                (OP_UINT8 *)popwin->numeditor->msg, 
                                fontattr, 
                                COLOR_POPUPSHADOW, 
                                COLOR_POPUPBG);
                 }
    
                 draw_text(AREA_POPUP,
                            x_pos, 
                            y_pos, 
                            x_limit,
                            (OP_UINT8 *)popwin->numeditor->msg, 
                            fontattr, 
                            COLOR_POPUPFG, 
                            COLOR_POPUPBG);
            }

             /* reset the softkey area */
            if(popwin->numeditor->attr & LA_LEFTBUTTON)
            {
                if (IS_NOT_NULL_STRING(popwin->numeditor->digits))
                {
                    ds_set_softkeys_rm(PMPT_SKEY_MENU, PMPT_SKEY_OK, PMPT_SKEY_BACK);    
                }
                else
                {
                    ds_set_softkeys_rm(PMPT_SKEY_MENU, PMPT_SKEY_EMPTY, PMPT_SKEY_BACK);    
                }
            }
            else if (!( popwin->numeditor->attr & LA_NO_DEFAULT_SKEY))
            {
                /* display default popup softkeys */
                /* added by Steven Lai. If user has not input any digital, donot display PMPT_SKEY_OK */
                if (IS_NOT_NULL_STRING(popwin->numeditor->digits))
                {
                    ds_set_softkeys_rm(PMPT_SKEY_EMPTY, PMPT_SKEY_OK, PMPT_SKEY_BACK);    
                }
                else
                {
                    ds_set_softkeys_rm(PMPT_SKEY_EMPTY, PMPT_SKEY_EMPTY, PMPT_SKEY_BACK);    
                }
            }

             /* clear scroll icon */
             y_pos = (OP_INT16)(y_base+POPUP_LINE_HEIGHT+POPUP_SCROLL_TOP_MARGINE);
        
             draw_partial_icon(AREA_POPUP, 
                       x_base,
                       y_pos,
                       (OP_INT16)ds_get_image_width(ICON_POPUP_LEFT_SCROLL),
                       (OP_INT16)ds_get_image_height(ICON_POPUP_LEFT_SCROLL),
                       POPUP_DEFAULT_LEFT_MARGINE,
                       POPUP_DEFAULT_TOP_MARGINE,
                       (RM_ICON_T*)util_get_icon_from_res(BMP_POPUP_BACKGROUND)
                       );

             /* draw background of the digits */
             x_pos = x_base;         
             y_pos = (OP_INT16)(y_base + POPUP_LINE_HEIGHT *2);
              fill_rect(AREA_POPUP,
                        x_focus,
                        y_pos,
                        x_focus_limit,
                        (OP_INT16)(y_pos+POPUP_FOCUSBAR_HEIGHT-1),
                        COLOR_POPUP_FOCUSBAR);

                        
            /* draw digits */
            if (IS_NOT_NULL_STRING(popwin->numeditor->digits))
            {
                leng = displayed_len = UstrCharCount(popwin->numeditor->digits);

                cursor_pos = (OP_UINT8)(popwin->numeditor->pos);
 
                /* if the currem positon is the same with the lenght of the already entered digits,
                   it means that the cursor is displayed at the end of the digits */
                if (popwin->numeditor->pos == leng)
                {
                    /* use underline as cursor in this case */
                    displayed_digits[popwin->numeditor->pos*2] = 0x5f;
                    displayed_digits[popwin->numeditor->pos*2+1] = 0x00;
                    displayed_len++;
                }

                written_digits_leng = displayed_len;

                /* get the number of digits to be displayed */
                /* if the number of displayed digits is longer than the maximum displayed digits*/
                if (displayed_len > DS_SCRN_POPUP_CWIDE)
                {
                    /* if the current cursor is positioned lear part of the digits, display 
                       the last maximum displayed digits, otherwise, display from the beginning  */
                    if (popwin->numeditor->pos > (DS_SCRN_POPUP_CWIDE-1))
                    { 
                        digit_offset = displayed_len - DS_SCRN_POPUP_CWIDE;
                    }
                    written_digits_leng = DS_SCRN_POPUP_CWIDE;
                }

                /* get the displayed cursor position */
                /* if the current cursor position is after the maximum displayed digits */
                if (popwin->numeditor->pos > (DS_SCRN_POPUP_CWIDE-1))
                {
                    /* if the cursor does not indicating next input position, 
                       - indicating one of already entered digits - 
                       the displayed cursor position will be the adjusted 
                       by the number of maximum displayed digits */
                    if (popwin->numeditor->pos < leng)
                    {
                        cursor_pos = DS_SCRN_POPUP_CWIDE - (leng-popwin->numeditor->pos);
                    }
                   
                    /* display left scroll icon */
                   draw_icon(AREA_POPUP,
                             x_base,
                             (OP_INT16)(y_base + POPUP_LINE_HEIGHT+POPUP_SCROLL_TOP_MARGINE),
                             util_get_icon_from_res(ICON_POPUP_LEFT_SCROLL)); 

                }

                /* get center position */
                x_pos = (OP_INT16)GET_CENTER( x_base, x_limit, (font_width*(written_digits_leng)) ); 

                if ( popwin->numeditor->attr & LA_PASSWORD)
                {
                    op_memset(asterisks, 0x00, sizeof(asterisks));
                    if (cursor_pos >= leng)
                    {
                        Umemset((asterisks+(written_digits_leng-1)*2), 0x005f,1); /* 0x005f is underline */
                        Umemset(asterisks, 0x002a, (OP_INT32)(written_digits_leng-1)); /* 0x002a is * */
                    }
                    else
                    {
                        Umemset(asterisks, 0x002a, (OP_INT32)(written_digits_leng)); /* 0x002a is * */
                    }
                    draw_text_line(AREA_POPUP,
                                x_pos, 
                                (OP_INT16)(y_pos+1), 
                                x_limit,
                                (OP_UINT8 *)asterisks, 
                                OP_TRUE,
                                fontattr, 
                                COLOR_POPUPFG, 
                                COLOR_POPUPBG);    
                }
                else
                { 
                    draw_text_line(AREA_POPUP,
                                x_pos, 
                                (OP_INT16)(y_pos+1), 
                                x_limit,
                                (OP_UINT8 *)(displayed_digits+digit_offset*2), 
                               OP_TRUE,
                               fontattr, 
                                COLOR_POPUPFG, 
                                COLOR_YELLOW);   

                }

                if (cursor_pos <  leng)
                {
                    /* cursor - one of the entered digits */
                    /* get center position */
                    x_pos += font_width*cursor_pos; 
                    fontattr &= ~FONT_OVERLAP;
                   if ( popwin->numeditor->attr & LA_PASSWORD)
                   {
                        cursor = asterisk;
                   }
                   else
                   {
                        cursor = (OP_UINT8 *)(popwin->numeditor->digits+popwin->numeditor->pos*2);
                   }
                }
            }
            else
            {
                /* cursor - a blank at the center of the input rect */
                cursor = underline;
                x_pos = (OP_INT16)GET_CENTER( x_pos, x_limit, font_width ); 
           }

           /* if the cursor is not underline, draw the cursor positioned digit */ 
           if (cursor != OP_NULL)
           {
               draw_char(AREA_POPUP,
                            x_pos, 
                             (OP_INT16)(y_pos+1), 
                             cursor, 
                             fontattr , 
                             COLOR_POPUPFG, 
                             COLOR_POPUP_CHAR_CURSORBG);    
           }
        }
            break;
           

        case POPUP_OWNERDRAW:
            if (popwin->ownerdraw_func) 
            {
                refresh_whole_popup = OP_FALSE;
                (popwin->ownerdraw_func)( popwin, dpu );
            }
            break;
    }
    
    if (refresh_whole_popup)
    {
        ds_refresh();      
    }
}

/*==================================================================================================
    FUNCTION: alloc_popup_window

    DESCRIPTION:
        Allocate a new popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static DS_POPUP_HANDLE alloc_popup_window 
( 
    void 
)
{

    OP_INT8         i;
    DS_POPUP_HANDLE handle = PHANDLE_NONE;
    
    /* only when available popup exists */    
    if (ds.cnt_popup < POPUP_MAX_WIN)
    {
        /* find an empty popup window */
        for (i = 0; i < POPUP_MAX_WIN; i++) 
        {
            if (popWin[i].handle == PHANDLE_NONE) 
            {
                handle = i;
                popWin[i].handle = handle;
                ds.cnt_popup ++;
                break;
            }
        }            
    }   
    return handle;
}


/*==================================================================================================
    FUNCTION: close_popup_window

    DESCRIPTION:
        Close the passed popup window. 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the previous popup window handle if exist, otherwise returns PHANDLE_NONE. 
        
    IMPORTANT NOTES:
        Keep in mind that the closed popup window is not the currently active one.       
==================================================================================================*/
static DS_POPUP_HANDLE close_popup_window 
( 
    DS_POPUP_HANDLE    handle 
)
{
    DS_POPUP_HANDLE prev_popup = PHANDLE_NONE;
    DS_POPUP_HANDLE next_popup = PHANDLE_NONE;
    DS_POPUP_HANDLE new_active_popup = PHANDLE_NONE;
   
     
    /* is it valid popup */
    if (PHANDLE_ISVALID(handle)) 
    {
        /* get linked popups */
        prev_popup = popWin[handle].prevPopup;
        next_popup = popWin[handle].nextPopup;

#ifdef MEM_EFFECTIVE_POPUP_BUF 
       /* clear the assigned popup buffer */
        wmemset(&pop_buffer[handle][0][0], COLOR_TRANS, POPUP_MAX_WIDTH * POPUP_MAX_HEIGHT);
#else
       /* clear the assigned popup buffer */
        wmemset(&pop_buffer[handle][0][0], COLOR_TRANS, LCD_MAX_X * LCD_EXT_TEXT_HIGH);
#endif
        
        /* If there is a next popup windows, link this next and prevous popup windows */
        if (next_popup != PHANDLE_NONE)
        {
            /* If previous popup is not PHANDLE_NONE, the previous popup handle is set as 
               the previous popup handle of the next popup. But if the prevous popup is PHANDLE_NONE,
               PHANDLE_NONE is set to previous popup handle of the next popup. */
            popWin[next_popup].prevPopup = prev_popup;
        }
        if ( prev_popup != PHANDLE_NONE)
        {
            /* If next popup is not PHANDLE_NONE, the next popup handle is set as 
               the next popup handle of the previous popup. But if the next popup is PHANDLE_NONE,
               PHANDLE_NONE is set to next popup handle of the previous popup. */
            popWin[prev_popup].nextPopup = next_popup;
        } 

        /* if just closed popup was not the active one */
        if (ds.popwin != &popWin[handle])
        {
            new_active_popup = ds.popwin->handle ;
        }
        else /* closed popup was the active one */
        {
            /* newly active one is the prevoius popup window */
            new_active_popup = prev_popup;
        }         

        /* reset popup handles */
        popWin[handle].handle = PHANDLE_NONE;
        popWin[handle].prevPopup = PHANDLE_NONE;
        popWin[handle].nextPopup = PHANDLE_NONE;

        popWin[handle].rcUpdate.left         = LCD_MAX_X-1;
        popWin[handle].rcUpdate.top          = LCD_EXT_TEXT_HIGH-1;
        popWin[handle].rcUpdate.right        = 0;
        popWin[handle].rcUpdate.bottom       = 0;

        if (ds.cnt_popup > 0 )
        {
            ds.cnt_popup --;
        }
    
    }
    
    return new_active_popup;
}

/*==================================================================================================
    FUNCTION: create_popup

    DESCRIPTION:
        Create a new empty popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        Keep in mind that the current popup window pointer will be changed to the newly created one
        after this ( inside of open_popup).
==================================================================================================*/
static DS_POPUPWIN_T *create_popup 
(
    DS_POPUP_DATA_T    *p_data
)
{
    DS_POPUP_HANDLE    handle;
 
    /* get new popup handle */   
    handle = alloc_popup_window();

    if (handle == PHANDLE_NONE)
    {
        return OP_NULL;
    }

    popWin[handle].handle     = handle;    
    
    if (p_data == OP_NULL)  /* set default popup data */
    { 
        popWin[handle].type     = POPUP_MSG;    
        Umemset(popWin[handle].title, 0x0000, 1);
        popWin[handle].icon     = OP_NULL;
        Umemset(popWin[handle].msg, 0x0000,1);
        popWin[handle].list     = OP_NULL;
        popWin[handle].menu     = OP_NULL;
        popWin[handle].numeditor     = OP_NULL;
    }
    else /* set the given popup data */
    {
        popWin[handle].type = p_data->type;
        //op_strcpy((char *)popWin[handle].title, (char *)p_data->title);
        Ustrcpy(popWin[handle].title, p_data->title);
        popWin[handle].icon = p_data->icon;
        //op_strcpy((char *)popWin[handle].msg, (char *)p_data->msg);
        Ustrcpy(popWin[handle].msg, p_data->msg);
        popWin[handle].list = p_data->list;
        popWin[handle].menu = p_data->menu;
        popWin[handle].numeditor = p_data->numeditor;
    }
    
    popWin[handle].attr  = PUA_DEFAULT;
    popWin[handle].state = PS_NONE_S;


    /* update popup chain */
    if (ds.popwin && ds.popwin->handle != PHANDLE_NONE)
    {
        popWin[ds.popwin->handle].nextPopup = handle;
        popWin[handle].prevPopup            = ds.popwin->handle;
    }
    else
    {
        popWin[handle].prevPopup = PHANDLE_NONE;
    }

    popWin[handle].max_lines    = 0;
    popWin[handle].nitems       = 0;
    popWin[handle].indexFirst   = 0;
    popWin[handle].indexFocused = 0;
    popWin[handle].indexSel     = INDEX_NONE;
    popWin[handle].indexCheck   = 0;
    popWin[handle].timer        = 0;
    
    popWin[handle].point.x      = 0;
    popWin[handle].point.y      = 0;
    
    popWin[handle].pos.left     = 0;
    popWin[handle].pos.top      = 0;
    popWin[handle].pos.right    = 0;
    popWin[handle].pos.bottom   = 0;

    /* positon of the previously existed popup */
    popWin[handle].pos_ghostPopup.left   = 0;
    popWin[handle].pos_ghostPopup.top    = 0;
    popWin[handle].pos_ghostPopup.right  = 0;
    popWin[handle].pos_ghostPopup.bottom = 0;
    popWin[handle].hsdGhostPopup         = 0;
  
    Umemset( popWin[handle].skey.left.txt, 0x0000,1);
    popWin[handle].skey.left.icon        = OP_NULL;
    Umemset( popWin[handle].skey.center.txt, 0x0000,1);
    popWin[handle].skey.center.icon      = OP_NULL;
    Umemset( popWin[handle].skey.right.txt, 0x0000,1);
    popWin[handle].skey.right.icon       = OP_NULL;

#ifdef MEM_EFFECTIVE_POPUP_BUF
    /* initialy set reversly */
    popWin[handle].rcUpdate.left         = POPUP_MAX_WIDTH-1;
    popWin[handle].rcUpdate.top          = POPUP_MAX_HEIGHT-1;
    popWin[handle].rcUpdate.right        = 0;
    popWin[handle].rcUpdate.bottom       = 0;
#else
    /* initialy set reversly */
    popWin[handle].rcUpdate.left         = LCD_MAX_X-1;
    popWin[handle].rcUpdate.top          = LCD_EXT_TEXT_HIGH-1;
    popWin[handle].rcUpdate.right        = 0;
    popWin[handle].rcUpdate.bottom       = 0;
#endif
    
    popWin[handle].event_handler         = OP_NULL;
    popWin[handle].ownerdraw_func        = OP_NULL;
    popWin[handle].bHasScrollBar         = OP_FALSE;
    popWin[handle].offsetScrollbar       = 0;
    return &popWin[handle];
    
}


/*==================================================================================================
    FUNCTION: open_popup

    DESCRIPTION:
        Open the allocated new popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
static void open_popup 
( 
    DS_POPUPWIN_T *popwin 
)
{
    DS_POPUP_DRAW_INFO_T    dpui;
    DS_POPUP_TYPE_ENUM_T    popup_type = popwin->type;

    
    if (!popwin || popwin->handle == PHANDLE_NONE) 
    {
        return;
    }

    
    /* if no current popup exists, this is the first popup. And make the background scrambled. */
    if (ds.popwin == OP_NULL)
    {
        /* update region to scramble the background. 
           In this case do not update AREA MASK, just update update region.
           By doing this, the scrambling background is done directly to lcd_screen without copying 
           txt_screen or bitmap_screen to lcd_screen. */
        /* keep the scrambled background update areas to recover them when the last popup window is 
           closed */

        if (ds_get_disp_annun())
        {
            set_update_region(0,0, LCD_MAX_X -1, LCD_TEXT_HIGH -1); 
        }
        else
        {
            set_update_region(0,0, LCD_MAX_X -1, LCD_EXT_TEXT_HIGH -1); 
        }
    }
    /*
    else
    {
       it means that the current popup exists and this is the new popup window.
       In this case, the background has been scrambled by the current popup window.
    }
    */
    
    /* update the current popup window */
    ds.popwin = popwin;
    /* re-derefrence the popup screen */
    pop_screen = ( OP_UINT16(*)[LCD_POPUP_MAX_X])pop_buffer[popwin->handle]; 

    popwin->state = PS_ENTER_S;

  
#ifdef RECOVER_AFTER_POPUP
    /* if it is the only popup at this time, 
       save the current softkey to recover it when the last popup is colsed */
    if (ds.cnt_popup == 1)
    {
        ds.popwin->skey_backup = ds.skey;
    }
#endif

    if ( popup_type == POPUP_MENU     || popup_type == POPUP_LIST ||
         popup_type == POPUP_OWNERDRAW || popup_type == POPUP_MSG ||
        (popup_type == POPUP_DIALOG )|| (popup_type == POPUP_NUMEDITOR)) 
    {
        pass_popup_event(ds.popwin, DS_NONE_EVENT);
    }
    else 
    {
        dpui.type = DPU_INIT;
        draw_popup_window( ds.popwin, &dpui );
    }
    /*
     *	set pen status
     */
    TPO_SetCurrent_RPU(TPO_RUI_DEFAULT);
}


/*==================================================================================================
    FUNCTION: close_popup_which

    DESCRIPTION:
        Close the sepcified popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
static void close_popup_which 
( 
    DS_POPUPWIN_T    *popwin 
)
{
    DS_POPUP_HANDLE         rpHandle = PHANDLE_NONE;    /* recovered poopup handle */
    DS_POPUP_HANDLE         cpHandle = PHANDLE_NONE;    /* closed popup handle */
    DS_POPUP_HANDLE         ppHandle = PHANDLE_NONE;    /* the previous popup handle of the closed popup */
    DS_POPUP_HANDLE         npHandle = PHANDLE_NONE;    /* the next popup handle of the closed popup */
    DS_POPUP_HANDLE         apHandle = PHANDLE_NONE;    /* currently active popup - ds.popwin */

    DS_RECT_T               cp_rect;  /* closed popup rectangle - position */
    DS_RECT_T               gp_rect;  /* position of the previous closed popups of the closed popup. */
    OP_UINT8                popup_frame_start = 0; /* depends on annuciator displaying */
    OP_BOOLEAN              has_ghost_popup = OP_FALSE;     /* indicate that some previous popups were
                                                        closed but not cleared from the LCD */

    apHandle = ds.popwin->handle;

    if ( ds_get_disp_annun())
    {
        popup_frame_start = LCD_ANNUN_HIGH;
    }
   
    if (popwin != OP_NULL)
    {
        cp_rect         = popwin->pos;
        has_ghost_popup = popwin->hsdGhostPopup;
        gp_rect         = popwin->pos_ghostPopup;             
        cpHandle        = popwin->handle;
        ppHandle        = popwin->prevPopup;
        npHandle        = popwin->nextPopup;
    }
       
    /* close the passed popup and get the newly active popup.
       Keep in mind that the closed one may not be not the active one.
       Be careful, close_popup_window changes popup info of the passed handle. */       
    rpHandle = close_popup_window(popwin->handle);

    if ( rpHandle == PHANDLE_NONE )
    {
#ifdef RECOVER_AFTER_POPUP
        /* No more popup window.
           When all popup windows are closed, Recover the scrambled background to unscrambled. */
        
        ds.last_update_area &= ~AREAMASK_POPUP;
        UPDATESET(ds.last_update_area);
        ds.last_update_area = AREAMASK_NULL;

        if (ds_get_disp_annun())
        {
            set_update_region(0,0, (OP_INT16)(LCD_MAX_X -1), (OP_INT16)(LCD_TEXT_HIGH -1)); 
        }
        else
        {
            set_update_region(0,0, (OP_INT16)(LCD_MAX_X -1), (OP_INT16)(LCD_EXT_TEXT_HIGH -1)); 
        }

        /* recover the previous soktkey when ds.cnt_popup == 0 */
        ds_set_softkeys_char(ds.popwin->skey_backup.left.txt, 
                               ds.popwin->skey_backup.center.txt,
                               ds.popwin->skey_backup.right.txt);       
#else
        /* when a popup is close but the next screen does not refresh the whole screen, 
           there can be poup screen left over (caused by scramble).
           Set the update region to the whole content area so that the all the scramble is gone */
        if (ds_get_disp_annun())
        {
            set_update_region(0,0, (OP_INT16)(LCD_MAX_X -1), (OP_INT16)(LCD_TEXT_HIGH -1)); 
        }
        else
        {
            set_update_region(0,0, (OP_INT16)(LCD_MAX_X -1), (OP_INT16)(LCD_EXT_TEXT_HIGH -1)); 
        }
#endif
        
        ds.popwin = OP_NULL;
    }
    else /* exist another popup window */
    {       
        /* if the closed popup and the currenty active popup are not same, 
           do not erase the closed popup from the LCD. Let it be cleared when the next popup of 
           the closed popup becomes the active popup and the active popup is closed.
           That means that when the active popup is closed the the LCD is refreshed accordingly.
           Otherwise, that just closed popup window is still displayed on the LCD.
           The only one case that the closed popup is not the active one is when the closed
           popup is a time-outed message popup. And displaying timed popup does not matter. 
           A popup keeps its prevous popup window position if the previous popup was closed but
           left as displayed on the LCD. When the popup is closed it checks the position of the
           previous popup positon and if the position is not null, recovered that positon too with
           its positon. The prevous popup positon can be accumulated becase several previous popups
           might be closed before the popup */
        
        if (cpHandle != apHandle)
        {
#ifdef MEM_EFFECTIVE_POPUP_BUF
            popwin[npHandle].pos_ghostPopup = 
                       *(util_add_rect_in_boundary((DS_RECT_T *)&popWin[npHandle].pos_ghostPopup,
                                               cp_rect,
                                               (OP_UINT8)(LCD_MAX_X -1),
                                               (popup_frame_start > 0) ? 
                                               (OP_UINT8)(POPUP_MAX_HEIGHT-LCD_ANNUN_HIGH-1): 
                                               (OP_UINT8)(POPUP_MAX_HEIGHT-1)));
#else
            OP_UINT8    ylimit;
            ylimit = (popup_frame_start >0) ? (LCD_TEXT_HIGH-1) : (LCD_EXT_TEXT_HIGH-1) ;
            popwin[npHandle].pos_ghostPopup = 
                       *(util_add_rect_in_boundary((DS_RECT_T *)&popWin[npHandle].pos_ghostPopup,
                                               cp_rect,
                                               (OP_UINT8)(LCD_MAX_X -1),
                                               ylimit));
#endif
        }
        else /* closed popup is the active one */
        {
            /* if there was a popup that was closed but displayed on the LCD yet. 
               Then clear the positon of the previous popup together with the 
               position of the currenlty closed popup 
               Keep it mind that the previous popups that shall be cleared out 
               together with the currenly closed popup might not be the actual previous
               popup of the currently closed popup. These previous popups to be cleared
               are some what ghost popups that were closed before but are still displayed
               on the LCD */
            if (has_ghost_popup)
            {
#ifdef MEM_EFFECTIVE_POPUP_BUF
               /* add the previous popup positon to the currently closed popup position */
                cp_rect = 
                    *(util_add_rect_in_boundary(  
                                                &cp_rect, 
                                                gp_rect, 
                                                LCD_MAX_X -1,
                                                (popup_frame_start > 0) ? 
                                                    (POPUP_MAX_HEIGHT-LCD_ANNUN_HIGH-1): 
                                                    (POPUP_MAX_HEIGHT-1)));
#else
                OP_UINT8 ylimit;
                ylimit = (popup_frame_start >0) ? (LCD_TEXT_HIGH-1) : (LCD_EXT_TEXT_HIGH-1) ;

               /* add the previous popup positon to the currently closed popup position */
                cp_rect =  *(util_add_rect_in_boundary( 
                                                       &cp_rect, 
                                                        gp_rect, 
                                                        LCD_MAX_X -1,
                                                        ylimit));

#endif
            }
            /* scramble the background of the regions of the closed popups */
            UPDATESET(AREAMASK_BITMAP|AREAMASK_TEXT);
            add_update_region(AREA_TEXT, 
                              cp_rect.left,
                              cp_rect.top, 
                              cp_rect.right, 
                              cp_rect.bottom);                

           /* update the active popup */
            ds.popwin = &popWin[rpHandle]; 

            /* re-derefrence the popup screen */
            pop_screen = ( OP_UINT16(*)[LCD_POPUP_MAX_X])(&pop_buffer[rpHandle][0][0]) ; 


            /* Refresh the recovered popup wbich was the previous popup of the closed popup. 
               Even though there are several popup behind of the currently cl0sed popup,
               we just refresh the right previous popup */
            UPDATESET(AREAMASK_POPUP);
#ifdef MEM_EFFECTIVE_POPUP_BUF
            set_popup_update_region(0, 0,  
                                    (OP_INT16)(ds.popwin->pos.right - ds.popwin->pos.left),
                                    (OP_INT16)(ds.popwin->pos.bottom - ds.popwin->pos.top));
#else
            set_popup_update_region(ds.popwin->pos.left, ds.popwin->pos.top,
                                    ds.popwin->pos.right, ds.popwin->pos.bottom );
#endif
            ds_set_softkeys_char(popwin[rpHandle].skey.left.txt, 
                                 popwin[rpHandle].skey.center.txt,
                                 popwin[rpHandle].skey.right.txt);       
        }
    }


#ifdef RECOVER_AFTER_POPUP
    ds_refresh();
#endif

    /* Pass the event to the newly active popup */
    /* Let the app pass the event to its popup widow. */
    // pass_popup_event(popwin->event);

}


/*==================================================================================================
    FUNCTION: PDEH_popup_msg

    DESCRIPTION:
        POPUP_MSG default event handler 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the event processing result.
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_msg 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        event 
)
{
    DS_POPUP_DRAW_INFO_T       dpui;
    DS_POPUP_STATE_ENUM_T      state;
    DS_POPUP_STATE_ENUM_T      next_state = PS_NONE_S;

    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_PROC_EVENT;
    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    
    do 
    {
        /*  only active popup can handle an event. if not exit.
            Exception occurs when the msg popup window expires. */
        if  ((ds_get_popup() != pw) && (event != MSG_TIMER_POPUP_MSG) && (event != MSG_FOCUS_CUST_APP_PAUSE) ) 
        {
            p_ret_state = POPUP_UNPROC_EVENT;
            break;
        }
        
        state = pw->state;
        switch (state) 
        {
        
            case PS_ENTER_S:
                if (pw->event_handler)
                { 
                    (pw->event_handler)( state, event );
                }
                  
                dpui.type = DPU_INIT;
                /* initial update */
                draw_popup_window(pw, &dpui); 
                if (pw->state == state)
                {
                    next_state = PS_MAIN_S;
                }
                break;
              
            case PS_MAIN_S:

                if( pw->event_handler)
                {
                    (pw->event_handler)( state, event );
                }                        
                   
                switch (event) 
                {
                    /* message popup expires.*/
                    case MSG_FOCUS_CUST_APP_PAUSE:
                    case MSG_TIMER_POPUP_MSG:
                    /* close the message popup window in any key press */
                    case KEY_1:  
                    case KEY_2:
                    case KEY_3:
                    case KEY_4:
                    case KEY_5:
                    case KEY_6:
                    case KEY_7:
                    case KEY_8:  
                    case KEY_9:
                    case KEY_STAR:
                    case KEY_0:
                    case KEY_POUND:
                    case KEY_SEND:  
                    case KEY_CLEAR:
                    case KEY_END:
                    case KEY_OK:
                    case KEY_SOFT_LEFT:
                    case KEY_SOFT_RIGHT:
                    case KEY_LEFT:
                    case KEY_RIGHT:  
                    case KEY_UP:
                    case KEY_DOWN:
                    case KEY_SIDE_UP:
                    case KEY_SIDE_DOWN:
                         pw->state = PS_EXIT_S;
                        break;
                    default:
                        p_ret_state = POPUP_UNPROC_EVENT;
                        break;
                }
          
                break;
      
            case PS_RETURN_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, event );
                }
                if (pw->state == state) 
                {
                    pw->state = PS_MAIN_S;
                }
                break;
              
             case PS_EXIT_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, event );
                }
                  
                return POPUP_CLOSE;
            default: /* invalid state */
                pw->state = PS_NONE_S;
                return POPUP_CLOSE;
        }
        
    } while (state != pw->state);

    if (next_state != PS_NONE_S)
    {
        pw->state = next_state;
    }
    
    return p_ret_state ;
}


/*==================================================================================================
    FUNCTION: PDEH_popup_menu

    DESCRIPTION:
        POPUP_MENU default event handler 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the event processing result.
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_menu 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
)
{
    OP_UINT8                   old_first_index;
    OP_UINT8                   old_focus_index;
    DS_POPUP_DRAW_INFO_T       dpui;
    DS_POPUP_STATE_ENUM_T      state;
    DS_POPUP_STATE_ENUM_T      next_state = PS_NONE_S;

    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_PROC_EVENT;

    
#define POPMENUTIMER  20000
  
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    
    pw->event = key;
    
    do 
    {
        /*  only active popup can handle an event. if not exit */
        if (ds_get_popup() != pw) 
        {
            p_ret_state = POPUP_UNPROC_EVENT;
            break;
        }
              
        state = pw->state;
          
        switch (state) 
        {
            case PS_ENTER_S:
            
              /* call private event handler if exists */
              if (pw->event_handler)
              {
                  (pw->event_handler)( state, pw->event );
              }

#ifdef NOT_YET_IMPLEMENTED                
              ds_menutimer_set( POPMENUTIMER );
#endif
              /*  initial update  */
              dpui.type = DPU_INIT;
              draw_popup_window(pw, &dpui);  

              if (pw->state == state)
              {
                  next_state = PS_MAIN_S;
              }

              break;
            
            case PS_MAIN_S:
                         
                switch (pw->event) 
                {
                    case KEY_UP:
                    case KEY_SIDE_UP:
                    case KEY_LEFT:
#ifdef NOT_YET_IMPLEMENTED                
                        ds_menutimer_set( POPMENUTIMER );
#endif
                        old_first_index = pw->indexFirst;
                        old_focus_index = pw->indexFocused;
                        /* move up the menu items */ 
                        if (pw->indexFirst > 0 && pw->indexFirst == pw->indexFocused) 
                        {
                            pw->indexFirst--;
                            pw->indexFocused--;
                        }
                        else if (pw->indexFocused > 0)  
                        {
                            pw->indexFocused--;
                        } 
                        else 
                        {
                            /* draw previous page of the item */ 
                            if (pw->nitems >= pw->max_lines) 
                            {
                                pw->indexFirst = pw->nitems - pw->max_lines;
                            }
                            pw->indexFocused = pw->nitems - 1;
                            dpui.type = DPU_LISTALL;
                            draw_popup_window(pw, &dpui);
                              break;
                        }
                        
                       /* re-draw menu from the previous item */ 
                       if (old_first_index != pw->indexFirst) 
                        {
                            dpui.type = DPU_LISTALL;
                            draw_popup_window(pw, &dpui);
                        }
                        else if (old_focus_index != pw->indexFocused) 
                        {  /* just move the focus */ 
                            dpui.type = DPU_LISTLINE;
                            dpui.line = pw->indexFocused - pw->indexFirst + 1;
                            draw_popup_window(pw, &dpui);
                            
                            dpui.line = pw->indexFocused - pw->indexFirst;
                            draw_popup_window(pw, &dpui);
                        }
                        break;
  
                    case KEY_DOWN:
                    case KEY_SIDE_DOWN:
                    case KEY_RIGHT:
  #ifdef NOT_YET_IMPLEMENTED                
                        ds_menutimer_set( POPMENUTIMER );
  #endif
                        old_first_index = pw->indexFirst;
                        old_focus_index = pw->indexFocused;
  
                        /* move down the menu items */ 
                        if ((pw->indexFirst < pw->nitems - pw->max_lines) &&
                            (pw->indexFocused == pw->indexFirst + pw->max_lines-1) ) 
                        {
                            pw->indexFirst++;
                            pw->indexFocused++;
                        }
                        else if (pw->indexFocused < pw->nitems-1) 
                        {
                          pw->indexFocused++;
                        } 
                        else 
                        {
                            /* draw the menu from the start */ 
                            pw->indexFirst = 0;
                            pw->indexFocused = 0;
                            dpui.type = DPU_LISTALL;
                            draw_popup_window(pw, &dpui);
                              break;
                          }
                        if (old_first_index != pw->indexFirst) 
                        {  /* re-draw menu by placing the focus at the last line */ 
                            dpui.type = DPU_LISTALL;
                            draw_popup_window(pw, &dpui);
                        }
                        else if (old_focus_index != pw->indexFocused) 
                        {  /* just move the focus */
                            dpui.type = DPU_LISTLINE;
                            dpui.line = pw->indexFocused - pw->indexFirst - 1;
                            draw_popup_window(pw, &dpui);
                            
                            dpui.line = pw->indexFocused - pw->indexFirst;
                            draw_popup_window(pw, &dpui);
                        }
                        break;
  
                    case KEY_0: 
                    case KEY_1: 
                    case KEY_2: 
                    case KEY_3: 
                    case KEY_4:                  
                    case KEY_5: 
                    case KEY_6: 
                    case KEY_7: 
                    case KEY_8: 
                    case KEY_9:
                    {
                        OP_UINT8 index = 0xff;
                        OP_UINT8 key_value;
  #ifdef NOT_YET_IMPLEMENTED                
                        ds_menutimer_set( POPMENUTIMER );
  #endif
                        key_value = (OP_UINT8)KeyToAscii(pw->event);
                        
                        if ( key_value == '0')
                        {
                            index = 9;
                        }
                        else if ( key_value <= '9')
                        {
                            index = (key_value - '1');
                        }
  
                        if (index < pw->nitems) 
                        {
                            pw->indexFocused = index;
                            pw->state = PS_STO_S;
                        }
                    }
                      break;
  
                    case KEY_OK: /* center navigation key - store key*/
                        /* selected */
                        pw->state = PS_STO_S;
                        break;
  
  #ifdef NOT_YET_IMPLEMENTED                
                    case UI_MENUTIMER_F:
  #endif
                    case KEY_SOFT_RIGHT: 
                    case KEY_END:
                    case MSG_FOCUS_CUST_APP_PAUSE:
#ifdef NOT_YET_IMPLEMENTED
                    case UI_TOIDLE_F: /* timer expired */
#endif
                        /* cancel the menu and exits it */
                        pw->state = PS_EXIT_S;
                        break;
                    default:
                        p_ret_state = POPUP_UNPROC_EVENT;
                        break;
                }

                /* first draw the changed menu, then calls event handler */
                if (pw->event_handler)
                {
                    (pw->event_handler)( state, pw->event );
                } 
                break;
            
            case PS_RETURN_S: /* reentered */
                if (pw->event_handler) 
                {
                      pw->state = PS_MAIN_S;
                }
                break;
              
            case PS_STO_S: /* storing state */
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, pw->event );
                }
                
#ifdef NOT_YET_IMPLEMENTED                
                ds_menutimer_set( 0 );
#endif
                pw->state = PS_EXIT_S;
                break;
              
            case PS_EXIT_S:
                if (pw->event_handler)
                {
                    (pw->event_handler)( state, pw->event );
                } 
              
#ifdef NOT_YET_IMPLEMENTED                
              ds_menutimer_set( 0 );
#endif
                p_ret_state = POPUP_CLOSE;
                break;
              
            default: /* invalid state */
                p_ret_state = POPUP_CLOSE;
                break;
        }
    } while (state != pw->state);

    if (next_state != PS_NONE_S)
    {
        pw->state = next_state;
    }
    
    return p_ret_state;
}


/*==================================================================================================
    FUNCTION: PDEH_popup_list

    DESCRIPTION:
        POPUP_LIST default event handler 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the event processing result.
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_list 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
)
{
    OP_UINT8                    old_first_index;
    OP_UINT8                    old_focus_index;
    OP_UINT8                    old_sel_index;
    DS_POPUP_DRAW_INFO_T        dpui;
    DS_POPUP_STATE_ENUM_T       state;
    DS_POPUP_STATE_ENUM_T       next_state = PS_NONE_S;
    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_PROC_EVENT;

    DS_LINEATTR                 lineattr = LA_DEFAULT;
    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    
    pw->event = key;
    
    do 
    {
        /*  only active popup can handle an event. if not exit */
        if (ds_get_popup() != pw) 
        {
            p_ret_state = POPUP_UNPROC_EVENT;
            break;
        }
        
        state = pw->state;
        switch (state) 
        {            
            case PS_ENTER_S:
              if (pw->event_handler)
              { 
                  (pw->event_handler)( state, pw->event );
              }
                
              if (pw->indexSel > 0 && pw->indexSel != INDEX_NONE)
              {
                  /* set focus to selected item initially */
                  pw->indexFocused = pw->indexSel; 
              }
              
              dpui.type = DPU_INIT;
              /* initial update of the popup */
              draw_popup_window(pw, &dpui);  

              if (pw->state == state)
              {
                  next_state = PS_MAIN_S;
              }
              break;
              
            case PS_MAIN_S:
                         
              switch (pw->event) {
              case KEY_UP:
              case KEY_SIDE_UP:
              case KEY_LEFT:

                  old_first_index = pw->indexFirst;
                  old_focus_index = pw->indexFocused;

                  if (pw->indexFirst > 0 && pw->indexFirst == pw->indexFocused) 
                  {
                      pw->indexFirst--;
                      pw->indexFocused--;
                  }
                  else if (pw->indexFocused == 0) 
                  {
                      pw->indexFirst = pw->nitems - pw->max_lines;
                      pw->indexFocused = pw->nitems - 1;
                      dpui.type = DPU_LISTALL;
                      draw_popup_window(pw, &dpui);
                      break;
                  }
                  else if (pw->indexFocused > 0) 
                  {
                      pw->indexFocused--;
                  }
          
                  if (old_first_index != pw->indexFirst) 
                  {
                      dpui.type = DPU_LISTALL;
                      draw_popup_window(pw, &dpui);
                  }
                  else if (old_focus_index != pw->indexFocused) 
                  {
                      dpui.type = DPU_LISTLINE;
                      dpui.line = pw->indexFocused - pw->indexFirst + 1;
                      draw_popup_window(pw, &dpui);
                      
                      dpui.line = pw->indexFocused - pw->indexFirst;
                      draw_popup_window(pw, &dpui);
                  }
                  else 
                  {  
                      /* pass the event when the focus index is changed */
                      pw->event = DS_NONE_EVENT;          
                  }
                  break;

              case KEY_DOWN:
              case KEY_SIDE_DOWN:
              case KEY_RIGHT:
                  old_first_index = pw->indexFirst;
                  old_focus_index = pw->indexFocused;

                  if ((pw->indexFirst < pw->nitems - pw->max_lines) &&
                      (pw->indexFocused == pw->indexFirst + pw->max_lines-1) ) 
                  {
                      pw->indexFirst++;
                      pw->indexFocused++;
                  }
                  else if (pw->indexFocused == pw->nitems-1) 
                  {
                      pw->indexFirst = 0;
                      pw->indexFocused = 0;
                      dpui.type = DPU_LISTALL;
                      draw_popup_window(pw, &dpui);
                      break;
                  }
                  else if (pw->indexFocused < pw->nitems-1) 
                  {
                      pw->indexFocused++;
                  }
                  
                  if (old_first_index != pw->indexFirst) 
                  {
                      dpui.type = DPU_LISTALL;
                      draw_popup_window(pw, &dpui);
                  }
                  else if (old_focus_index != pw->indexFocused) 
                  {
                      dpui.type = DPU_LISTLINE;
                      dpui.line = pw->indexFocused - pw->indexFirst - 1;
                      draw_popup_window(pw, &dpui);
                      
                      dpui.line = pw->indexFocused - pw->indexFirst;
                      draw_popup_window(pw, &dpui);
                  }
                  else 
                  {
                      /* pass the event when the focus index is changed */
                      pw->event = DS_NONE_EVENT;
                  }
                  break;

                case KEY_0: 
                case KEY_1: 
                case KEY_2: 
                case KEY_3: 
                case KEY_4:                  
                case KEY_5: 
                case KEY_6: 
                case KEY_7: 
                case KEY_8: 
                case KEY_9:
                {
                    OP_UINT8 index = 0xff;
                    OP_UINT8 key_value;
#ifdef NOT_YET_IMPLEMENTED                
                    ds_menutimer_set( POPMENUTIMER );
#endif
                    p_ret_state = POPUP_UNPROC_EVENT;

                    key_value = (OP_UINT8)KeyToAscii(pw->event);
                            
                    if ( key_value == '0')
                    {
                        index = 9;
                    }
                    else if ( key_value <= '9')
                    {
                        index = (key_value - '1');
                    }
      
                    if (index < pw->nitems) 
                    {
                        /* if it has LA_LINENUMBER attribute, then support direct selecting */
                        if (pw->list->items[index].attr & LA_LINENUMBER)
                        {
                            pw->indexFocused = index;
                            pw->indexSel = pw->indexFocused;
                            pw->state = PS_STO_S;
                            p_ret_state = POPUP_PROC_EVENT;
                        }
                    }
                 }
                 break;
        
              case KEY_OK: /* center navigation key - store key*/
                   /* set currently focused index as selected one */
                   pw->indexSel = pw->indexFocused;
                  if (pw->attr & PUA_USERLIST && pw->userdata_handler)
                  {
                      lineattr = (DS_LINEATTR)(pw->userdata_handler)(PUUD_LINEATTR, pw->indexFocused);
                  }
                  else if (pw->list)
                  {
                    lineattr = pw->list->items[pw->indexFocused].attr;
                  }
                   pw->state = PS_STO_S;
                  break;                  
        
               case KEY_SOFT_LEFT:
                  lineattr = pw->list->items[0].attr;

                  if (lineattr & LA_CHECKBOX) 
                  {
                      if (pw->indexCheck & 1<<(pw->indexFocused))
                      {
                          pw->indexCheck &= ~(1<<(pw->indexFocused));
                      }
                      else
                      {
                          pw->indexCheck |= 1<<(pw->indexFocused);
                      }
                                
                      dpui.line = pw->indexFocused - pw->indexFirst;
                      dpui.type = DPU_LISTLINE;
                      draw_popup_window(pw, &dpui);
                  }          
                  else if (lineattr & LA_RADIOBUTTON) 
                  {
                      dpui.type = DPU_LISTLINE;
                      old_sel_index = pw->indexSel;
                      pw->indexSel = pw->indexFocused;

                      /* toggle off the previously selected item if the item is currently displayed. */
                      if ( old_sel_index != INDEX_NONE)
                      {
                          if ( (old_sel_index >= pw->indexFirst) && 
                               (old_sel_index - pw->indexFirst) < pw->max_lines )
                          {
                              dpui.line = old_sel_index - pw->indexFirst ;
                              draw_popup_window(pw, &dpui);
                          }
                      }

                      /* trun on the newly selected item */
                      dpui.line = pw->indexSel - pw->indexFirst ;
                      draw_popup_window(pw, &dpui);
                  }
                  break;

              case KEY_SOFT_RIGHT: /* cancel */
              case KEY_END:
              case MSG_FOCUS_CUST_APP_PAUSE:
#ifdef NOT_YET_IMPLEMENTED
              case UI_TOIDLE_F: /* timer expired */
#endif
                  pw->state = PS_EXIT_S;
                  break;
              default :
                  p_ret_state = POPUP_UNPROC_EVENT;
                  break;
              }
              
              /* call popup event handler after updating list display */
              if (pw->event_handler) 
              {
                  (pw->event_handler)( state, pw->event );
              }
              break;
          
            case PS_RETURN_S:
                if (pw->event_handler)   
                {
                    (pw->event_handler)( state, pw->event );
                }
                if (pw->state == state)   
                {
                    pw->state = PS_MAIN_S;
                }
                break;
              
            case PS_STO_S:
                if (pw->event_handler)   
                {
                    (pw->event_handler)( state, pw->event );
                }
                if (pw->state == state)   
                {
                    pw->state = PS_EXIT_S;
                }
                break;
            
            case PS_EXIT_S:
                if (pw->event_handler)   
                {
                    (pw->event_handler)( state, pw->event );
                }
                p_ret_state = POPUP_CLOSE;
                break;
        
            default:
                p_ret_state = POPUP_CLOSE;
                break;
        }
        
    } while (state != pw->state);
    
    if (next_state != PS_NONE_S)
    {
        pw->state = next_state;
    }
 
    return p_ret_state;
}


/*==================================================================================================
    FUNCTION: PDEH_popup_ownerdraw

    DESCRIPTION:
        POPUP_OWNERDRAW default event handler 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the event processing result.
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_ownerdraw 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
)
{
    DS_POPUP_STATE_ENUM_T       state;
    OP_BOOLEAN                  do_refresh = OP_TRUE;
    DS_POPUP_STATE_ENUM_T       next_state = PS_NONE_S;
    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_PROC_EVENT;
    
    if (!pw || pw->handle == PHANDLE_NONE)
    {
        return POPUP_INVALID;
    }
    
    pw->event = key;
    
    do 
    {
        /*  only active popup can handle an event. if not exit */
        if (ds_get_popup() != pw) 
        {
            p_ret_state = POPUP_UNPROC_EVENT;
            break;
        }
           
        state = pw->state;
        switch (state) 
        {
            
            case PS_ENTER_S:
                if (pw->event_handler)
                {
                    do_refresh = (pw->event_handler)( state, pw->event );
                } 
                  
                if (pw->state == state)
                {
                    next_state = PS_MAIN_S;
                }
                break;
              
            case PS_MAIN_S:
                   
                if (pw->event_handler)
                {
                    do_refresh = (pw->event_handler)( state, pw->event );
                }
                
                switch (pw->event) 
                {
                    case KEY_OK: /* center navigation key - store key*/
                        pw->state = PS_STO_S;
                        break;
                    case KEY_SOFT_RIGHT: /* [Cancel] */
                    case KEY_END:
                    case MSG_FOCUS_CUST_APP_PAUSE:
#ifdef NOT_YET_IMPLEMENTED 
                    case UI_TOIDLE_F: /* timer expires */
#endif
                        pw->state = PS_EXIT_S;
                        break;
                    default:
                        p_ret_state = POPUP_UNPROC_EVENT;
                        break;
                }
                break;
          
            case PS_RETURN_S:
                if (pw->event_handler)
                {
                    do_refresh = (pw->event_handler)( state, pw->event );
                }
                  
                if (pw->state == state)
                {
                    pw->state = PS_MAIN_S;
                }
                break;
              
            case PS_STO_S:
                if (pw->event_handler)
                {
                    do_refresh = (pw->event_handler)( state, pw->event );
                }
                
                if (pw->state == state)
                {
                    pw->state = PS_EXIT_S;
                }
                break;
            
            case PS_EXIT_S:
                if (pw->event_handler)
                {
                  (pw->event_handler)( state, pw->event );
                } 
                
               p_ret_state = POPUP_CLOSE;
               break;
            default:
               p_ret_state = POPUP_CLOSE;
               break;
        }
        
        if (do_refresh)
        {
            ds_refresh();
        }        
    } while (state != pw->state);

    if (next_state != PS_NONE_S)
    {
        pw->state = next_state;
    }
    
    /* if OP_FALSE is returned, that means the popup window is no longer effective. */
    return p_ret_state;
}


/*==================================================================================================
    FUNCTION: PDEH_popup_dialog

    DESCRIPTION:
        POPUP_DIALOG default event handler 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the event processing result.
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_dialog 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
)
{
    DS_POPUP_DRAW_INFO_T       dpui;
    DS_POPUP_STATE_ENUM_T       state;
    DS_POPUP_STATE_ENUM_T       next_state = PS_NONE_S;
    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_PROC_EVENT;
    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    
    do 
    {
        /*  only active popup can handle an event. if not exit */
        if (ds_get_popup() != pw) 
        {
            p_ret_state = POPUP_UNPROC_EVENT;
            break;
        }
        
        state = pw->state;
        switch (state) 
        {
        
            case PS_ENTER_S:
                if (pw->event_handler)
                { 
                    (pw->event_handler)( state, key );
                }
                  
                dpui.type = DPU_INIT;
                /* initial update */
                draw_popup_window(pw, &dpui); 
                if (pw->state == state)
                {
                    next_state = PS_MAIN_S;
                }
                break;
              
            case PS_MAIN_S:
                   
                switch (key) 
                {
                   case KEY_OK: /* center navigation key - store key*/
                        pw->indexFocused = 0;
                        pw->state = PS_STO_S;
                        break;
                    case KEY_SOFT_RIGHT: /* [Cancel] */
                    case KEY_END:
                    case MSG_FOCUS_CUST_APP_PAUSE:
                        pw->indexFocused = 1;
                        pw->state = PS_STO_S;
                        break;
                    case KEY_UP:
                    case KEY_SIDE_UP:
                    case KEY_LEFT:
                    case KEY_DOWN:
                    case KEY_SIDE_DOWN:
                    case KEY_RIGHT:
                        break;

                    default:
                        p_ret_state = POPUP_UNPROC_EVENT;
                        break;
                }
          
                if (pw->event_handler)   
                {
                    (pw->event_handler)( state, key );
                }
                break;
      
            case PS_RETURN_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, key);
                }
                if (pw->state == state) 
                {
                    pw->state = PS_MAIN_S;
                }
                break;
              
            case PS_STO_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, key);
                }
                if (pw->state == state) 
                {
                    pw->state = PS_EXIT_S;
                }
                break;
            
            case PS_EXIT_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, key);
                }
                  
                p_ret_state = POPUP_CLOSE;
                break;
            default: /* invalid state */
                p_ret_state = POPUP_CLOSE;
                break;
        }
        
    } while (state != pw->state);
    
    if (next_state != PS_NONE_S)
    {
        pw->state = next_state;
    }

    /* if FALSE is returned, that means the popup window is no longer effective. */
    return p_ret_state;
}


/*==================================================================================================
    FUNCTION: PDEH_popup_numeditor

    DESCRIPTION:
        POPUP_NUMEDITOR default event handler 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the event processing result.
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static DS_POPUP_PDEH_RET_ENUM_T PDEH_popup_numeditor 
( 
    DS_POPUPWIN_T    *pw, 
    OP_UINT16        key 
)
{
    OP_BOOLEAN                  updated = OP_FALSE;
    OP_UINT8                    leng;
    DS_POPUP_DRAW_INFO_T       dpui;
    DS_POPUP_STATE_ENUM_T       state;
    DS_POPUP_STATE_ENUM_T       next_state = PS_NONE_S;
    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_PROC_EVENT;
    OP_UINT8                    *digits;
    OP_INT16                    *pos;
    OP_UINT8                    max_leng;

    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }

    digits = pw->numeditor->digits;
    pos    = &(pw->numeditor->pos);
    max_leng = pw->numeditor->max_leng;

    do 
    {
        /*  only active popup can handle an event. if not exit */
        if (ds_get_popup() != pw) 
        {
            p_ret_state = POPUP_UNPROC_EVENT;
            break;
        }
        
        state = pw->state;
        switch (state) 
        {
        
            case PS_ENTER_S:
                if (pw->event_handler)
                { 
                    (pw->event_handler)( state, key );
                }
                  
                dpui.type = DPU_INIT;
                /* initial update */
                draw_popup_window(pw, &dpui); 
                if (pw->state == state)
                {
                    next_state = PS_MAIN_S;
                }
                break;
              
            case PS_MAIN_S:
                /* if a key is not pressed and held, */
                if ((key != DS_POPUP_KEYPRESS_REPEAT_EVENT))
                {
                    clear_key_pressed = OP_FALSE;
                }
                switch (key) 
                {
                   case KEY_OK: /* center navigation key - store key*/
                        /* added by Steven Lai. If the digits is null, do nothing here */
                        if (*digits != OP_NULL)
                        {
                            pw->state = PS_STO_S;
                        }
                        break;
                    case KEY_SOFT_RIGHT: /* [Cancel] */
                        /* if no "Back" skey in the right skey, do not process this as cancel */
                        if (pw->numeditor->attr & LA_NO_DEFAULT_SKEY)
                        {
                            break;
                        }
                    case KEY_END:
                    case MSG_FOCUS_CUST_APP_PAUSE:
                        pw->state = PS_EXIT_S;
                        break;
                    case KEY_UP:
                    case KEY_SIDE_UP:
                    case KEY_LEFT:
                        /* move to the previous position */
                        if ( *pos >0)
                        {
                            (*pos)--; 
                            dpui.type = DPU_LISTLINE;
                            draw_popup_window(pw, &dpui);                            
                        }
                        break;
                        
                    case KEY_DOWN:
                    case KEY_SIDE_DOWN:
                    case KEY_RIGHT:
                        /* move to the next position */
                        if (*pos <(max_leng-1))
                        {
                            /* do not move the cursor if the current position is the next to the 
                               last digit - to prevent unmeaningful space into the digits */
                            if (*pos < UstrCharCount(digits))
                            {
                                (*pos)++;
                                updated = OP_TRUE;
                            }
                        }

                        if (updated)
                        {
                            dpui.type = DPU_LISTLINE;
                            draw_popup_window(pw, &dpui);                            
                        }
                        break;

                    /* long press event */
                    case DS_POPUP_KEYPRESS_REPEAT_EVENT:
                        /* only '0' and '*' are replaced if they are long pressed */
                        if (clear_key_pressed)
                        {
                            /* clear all digits */
                            op_memset(digits, 0x00, sizeof(pw->numeditor->digits));
                            *pos = 0;
                            last_numeditor_pos = 0;
                            dpui.type = DPU_LISTLINE;
                            draw_popup_window(pw, &dpui);                            
                            clear_key_pressed = OP_FALSE;
                        }
                        else if (digits[last_numeditor_pos] == '0')
                        {
                            digits[last_numeditor_pos] = '+';
                            dpui.type = DPU_LISTLINE;
                            draw_popup_window(pw, &dpui);                            
                        }
                        else if (digits[last_numeditor_pos] == '*')
                        {
                            digits[last_numeditor_pos] = 'p';
                            dpui.type = DPU_LISTLINE;
                            draw_popup_window(pw, &dpui);  
                        }
                        break;
                        
                    case KEY_STAR:
                    case KEY_POUND:
                    case KEY_0:
                    case KEY_1: 
                    case KEY_2:
                    case KEY_3:
                    case KEY_4:
                    case KEY_5:
                    case KEY_6:
                    case KEY_7:
                    case KEY_8:
                    case KEY_9:  
                        digits[(*pos)*2] = KeyToAscii(key);
                        digits[(*pos)*2+1] = 0x00;

                        last_numeditor_pos =(*pos)*2;

                        if (*pos <(max_leng-1))
                        {
                            (*pos)++;
                        }

                        dpui.type = DPU_LISTLINE;
                        draw_popup_window(pw, &dpui);                            

                        break;

                    case KEY_CLEAR:
                        /* clear the crrent cursor positioned digit */
                        leng = UstrCharCount(digits);
                        if (  leng > 0)
                        {
                            /* if the current cursor is positioned after the last digit or on the last digit, */
                            /* then, delete the last digit and move the cursor backward */
                            /* position is 0-based */
                            if (*pos >= leng)
                            {
                                (*pos)--;
                                Umemset(&(digits[(*pos)*2]), 0x0000,1);                        
                            }
                            else 
                            {
                                /* if the cursor is on one of entered digits, delete the digit */
                                /* and leave the cursor at the same position */
                                Ustrncpy(&(digits[(*pos)*2]), &(digits[(*pos)*2+2]), ((leng-(*pos))*2));                                                                            
                            }
                            /* set flag to indicate, clear key is pressed */
                            clear_key_pressed = OP_TRUE;

                            dpui.type = DPU_LISTLINE;
                            draw_popup_window(pw, &dpui);   

                        }
                        else /* if no digit to be deleted, regard this as cancel key is pressed */
                        {
                            /* if default numeditor right skey works */
                            if (!( pw->numeditor->attr & LA_NO_DEFAULT_SKEY))
                            {
                                pw->state = PS_EXIT_S;
                            }
                        }
                        break;

                    default:
                        p_ret_state = POPUP_UNPROC_EVENT;
                        break;
                }
          
                if (pw->event_handler)   
                {
                    (pw->event_handler)( state, key );
                }
                break;
      
            case PS_RETURN_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, key);
                }
                if (pw->state == state) 
                {
                    pw->state = PS_MAIN_S;
                }
                break;
              
            case PS_STO_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, key);
                }
                if (pw->state == state) 
                {
                    pw->state = PS_EXIT_S;
                }
                break;
            
            case PS_EXIT_S:
                if (pw->event_handler) 
                {
                    (pw->event_handler)( state, key);
                }
                  
                p_ret_state = POPUP_CLOSE;
                break;
            default: /* invalid state */
                p_ret_state = POPUP_CLOSE;
                break;
        }
        
    } while (state != pw->state);
    
    if (next_state != PS_NONE_S)
    {
        pw->state = next_state;
    }

    /* if FALSE is returned, that means the popup window is no longer effective. */
    return p_ret_state;
}

/*==================================================================================================
    FUNCTION: pass_popup_event

    DESCRIPTION:
        Popup event dispacher. 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns whether the passed event was processed or not. If the event was processed by the 
        popup event handler, the event was poped-up.

        The return value is one of DS_POPUP_PDEH_RET_ENUM_T and each has meaning in the below.

        POPUP_INVALID      : there is no active popup. 
        POPUP_CLOSE,       : The popup is no longer effective and is closed by the popup event handler.
        POPUP_PROC_EVENT   : The passed event was handled, so remove it after it returns.
        POPUP_UNPROC_EVENT : The passed event was not handled by the popup event handler,
                             process the event.

        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
static DS_POPUP_PDEH_RET_ENUM_T pass_popup_event 
(   
    DS_POPUPWIN_T    *pw,
    OP_UINT16         key 
)
{
    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_PROC_EVENT;
    void                          (*popup_action_func_ptr)(void) ;   

    popup_action_func_ptr = OP_NULL;
 
    /* except for POPUP_MSG and time out event, PDEH handles the event only when 
       pw is the currently active popup window - ds.popwin. */
    if ((ds.popwin == pw) || (pw->type == POPUP_MSG))
    {        
        switch (pw->type) 
        {
            case POPUP_MSG:
                p_ret_state = PDEH_popup_msg( pw, key );
                break;
            case POPUP_MENU:
                {
                    DS_POPUP_MENU_ENTRY_T      *index_menu;
                    index_menu = (DS_POPUP_MENU_ENTRY_T *)pw->menu->menu_data_ptr;
                    p_ret_state = PDEH_popup_menu( pw, key);

                    if (!index_menu[pw->indexFocused].sub_menu_flag &&
                        index_menu[pw->indexFocused].action_func_ptr) 
                    {
                        popup_action_func_ptr = index_menu[pw->indexFocused].action_func_ptr;
                    }
                }
                break;
            case POPUP_LIST:
                p_ret_state = PDEH_popup_list( pw, key );
                break;
            case POPUP_OWNERDRAW:
                p_ret_state = PDEH_popup_ownerdraw( pw, key );
                break;
            case POPUP_DIALOG:
                p_ret_state = PDEH_popup_dialog( pw, key );
                break;
            case POPUP_NUMEDITOR:
                p_ret_state = PDEH_popup_numeditor( pw, key );
                break;
            default:
                p_ret_state = POPUP_UNPROC_EVENT;
        }
        /* close the popup window if the default event handler returns FALSE */
        if ( p_ret_state == POPUP_CLOSE) 
        {
            if (ds.popwin->type == POPUP_MSG)
            {
                OPUS_Stop_Timer(OPUS_TIMER_POPUP_MSG);
            }
            close_popup_which(ds.popwin);
            if (popup_action_func_ptr)
            {
                (popup_action_func_ptr)();
            }
        }
    } 
   
    return p_ret_state;
    
}


/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: popup_init_popup_windows

    DESCRIPTION:
        Initialize popup related data.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
void popup_init_popup_windows 
( 
    void 
)
{
    OP_UINT8    i;
 
    /* clear popup handles */
    for (i = 0; i < POPUP_MAX_WIN; i++) 
    {
        popWin[i].handle = PHANDLE_NONE;
        popWin[i].prevPopup = PHANDLE_NONE;
        popWin[i].nextPopup = PHANDLE_NONE;
    }

    /* popup buffers */
    for (i=0;i<POPUP_MAX_WIN-1; i++)
    {
#ifdef MEM_EFFECTIVE_POPUP_BUF 
        wmemset ((OP_UINT16*)pop_buffer[i], COLOR_WHITE, POPUP_MAX_HEIGHT * POPUP_MAX_WIDTH);
#else
        wmemset(&pop_buffer[i][0][0], COLOR_TRANS, LCD_MAX_X * LCD_EXT_TEXT_HIGH);
#endif
    }
    ds.popwin = OP_NULL;
    ds.cnt_popup = 0;
}


/*==================================================================================================
    FUNCTION: ds_is_popup

    DESCRIPTION:
        Returns whether there is an active popup window or not.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
OP_BOOLEAN ds_is_popup 
( 
    void 
)
{
    if (ds.popwin && ds.popwin->handle != PHANDLE_NONE)
    {
        return OP_TRUE;
    }
    else
    {
        return OP_FALSE;
    }
}


/*==================================================================================================
    FUNCTION: ds_is_msg_popup_active

    DESCRIPTION:
        Returns whether an msg popup is currently active or not.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
OP_BOOLEAN ds_is_msg_popup_active 
( 
    void 
)
{
    if (ds.popwin && (ds.popwin->handle != PHANDLE_NONE) && (ds.popwin->type == POPUP_MSG))
    {
        return OP_TRUE;
    }
    else
    {
        return OP_FALSE;
    }
}

/*==================================================================================================
    FUNCTION: ds_is_popwin

    DESCRIPTION:
        Check the popup window is valid or not.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
OP_BOOLEAN ds_is_popwin 
( 
    DS_POPUPWIN_T    *pwin 
)
{
    if (pwin && pwin->handle >= 0 && pwin->handle < POPUP_MAX_WIN && pwin->handle != PHANDLE_NONE)
    {
        return OP_TRUE;
    }
    else
    {
        return OP_FALSE;
    }
}


/*==================================================================================================
    FUNCTION: ds_get_popup

    DESCRIPTION:
        Returns the current active popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
DS_POPUPWIN_T *ds_get_popup 
(
    void 
)
{
    if (ds.popwin && ds.popwin->handle != PHANDLE_NONE)
    {
        return ds.popwin;
    }
    else
    {
        return OP_NULL;
    }
}


/*==================================================================================================
    FUNCTION: ds_set_popup_pos

    DESCRIPTION:
        Set popup window position.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        use either ds_set_popup_pos() or ds_set_popup_point 
==================================================================================================*/
void ds_set_popup_pos 
(
    DS_POPUPWIN_T    *popwin,
    OP_INT16         left,
    OP_INT16         top,
    OP_INT16         right,
    OP_INT16         bottom
)
{
    if (!popwin || popwin->handle == PHANDLE_NONE)  
    {
        return;
    }

    popwin->attr      |= PUA_POSITION;
    popwin->pos.left   = left;
    popwin->pos.top    = top;
    popwin->pos.right  = right;
    popwin->pos.bottom = bottom;
  
}


/*==================================================================================================
    FUNCTION: ds_set_popup_point

    DESCRIPTION:
        Set popup placing point in the screen. Left top of the popup window is placed on the point.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        use either ds_set_popup_pos() or ds_set_popup_point 
==================================================================================================*/
void ds_set_popup_point 
(
    DS_POPUPWIN_T    *popwin,
    OP_INT16         x,
    OP_INT16          y
)
{
    if (!popwin || popwin->handle == PHANDLE_NONE) 
    {
        return;
    }
    popwin->attr    |= PUA_POINT;
    popwin->point.x = x;
    popwin->point.y = y;  

}


/*==================================================================================================
    FUNCTION: ds_set_popup_softkeys

    DESCRIPTION:
        Set popup softkeys.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
void ds_set_popup_softkeys 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         *left,
    OP_UINT8         *center,
    OP_UINT8         *right
)
{
    if (!popwin || popwin->handle == PHANDLE_NONE)  
    {
        return;
    }

    popwin->attr |= PUA_SOFTKEY;
    UstrNullCpy(popwin->skey.left.txt,   left,   SKEY_MAX_CHAR*2);
    UstrNullCpy(popwin->skey.center.txt, center, SKEY_MAX_CHAR*2);
    UstrNullCpy(popwin->skey.right.txt,  right,  SKEY_MAX_CHAR*2);
}


/*==================================================================================================
    FUNCTION: ds_set_popup_softkeys_rm

    DESCRIPTION:
        Set popup softkeys.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
void ds_set_popup_softkeys_rm 
(
    DS_POPUPWIN_T       *popwin,
    RM_RESOURCE_ID_T    left_res_id,
    RM_RESOURCE_ID_T    center_res_id,
    RM_RESOURCE_ID_T    right_res_id
)
{
    OP_UINT8 tmp_skey_str[SKEY_MAX_CHAR*2+2];

    if (!popwin || popwin->handle == PHANDLE_NONE)  
    {
        return;
    }

    popwin->attr |= PUA_SOFTKEY;

    if ( IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(left_res_id,tmp_skey_str, SKEY_MAX_CHAR*2 )))
    {
        //op_strncpy((char *)popwin->skey.left.txt,(char *)left_skey_str, SKEY_MAX_CHAR);
        Ustrcpy(popwin->skey.left.txt,tmp_skey_str);
    }

    if ( IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(center_res_id,tmp_skey_str, SKEY_MAX_CHAR*2 )))
    {
        //op_strncpy((char *)popwin->skey.center.txt,(char *)center_skey_str, SKEY_MAX_CHAR);
        Ustrcpy(popwin->skey.center.txt,tmp_skey_str);
    }

    if ( IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(right_res_id,tmp_skey_str, SKEY_MAX_CHAR*2 )))
    {
        //op_strncpy((char *)popwin->skey.right.txt, (char *)right_skey_str, SKEY_MAX_CHAR);
        Ustrcpy(popwin->skey.right.txt, tmp_skey_str);
    }

}

/*==================================================================================================
    FUNCTION: ds_get_popup_selection

    DESCRIPTION:
        Returns the currently selected item in POPUP_LIST, POPUP_MENU, POPUP_DIALOG.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
OP_UINT8 ds_get_popup_selection 
(
    DS_POPUPWIN_T    *popwin
)
{
    if (!popwin || popwin->handle == PHANDLE_NONE)
    {
        return INDEX_NONE;
    }
    if ( (popwin->type == POPUP_LIST) )
    {
        return popwin->indexSel;
    }
    else
    {
        return popwin->indexFocused;
    }
}

/*==================================================================================================
    FUNCTION: ds_get_popup_check

    DESCRIPTION:
        Returns the currently exclusively selected (checked) item in POPUP_LIST or POPUP_MENU.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
OP_UINT16 ds_get_popup_check 
(
    DS_POPUPWIN_T    *popwin
)
{
    if (!popwin || popwin->handle == PHANDLE_NONE) 
    {
        return INDEX_NONE;
    }
    if (popwin->type == POPUP_LIST)
    {
        return popwin->indexCheck;
    }
    else
    {
        return (OP_UINT16)popwin->indexFocused;
    }
}

/*==================================================================================================
    FUNCTION: ds_set_popup_selection

    DESCRIPTION:
        Select a radio buttoned item in the popup list and popup menu.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
void ds_set_popup_selection 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         index
)
{
    OP_UINT8                prev_sel;
    OP_UINT8                prev_focused;
    DS_POPUP_DRAW_INFO_T    dpui;

    if (!popwin || popwin->handle == PHANDLE_NONE) 
    {
        return;
    }
    if (popwin->type == POPUP_LIST) 
    {
        prev_sel = popwin->indexSel;
        prev_focused = popwin->indexFocused;
        popwin->indexSel = index;
        popwin->indexFocused = index;
        if (popwin->state <= PS_ENTER_S) 
        {
            OP_UINT8 lines = POPUP_MAX_ITEM;
            lines = MIN(lines, popwin->nitems);
            if (index >= lines) 
            {
                popwin->indexFirst = index;
                if (popwin->indexFirst > popwin->nitems - lines)
                { 
                    popwin->indexFirst = popwin->nitems - lines;
                }
            }
            /* do not refresh in PS_ENTER_S */
            return ;
        }

        /* refresh the new selection */    
        if (prev_sel != index)
        {
            /* newly selected item is in the cureent sceen, update selection in the screen */
            if (IS_IN_SCREEN(index, popwin->indexFirst, popwin->max_lines))
            {
                dpui.type = DPU_LISTLINE;

                /* unhighlit previouslu focused item. */
                if (IS_IN_SCREEN(prev_focused, popwin->indexFirst, popwin->max_lines))
                {
                    dpui.line = prev_focused;
                    draw_popup_window( ds.popwin, &dpui );
                }
     
                /* toggle off the previous selection */
                if (IS_IN_SCREEN(prev_sel, popwin->indexFirst, popwin->max_lines))
                {
                    dpui.line = prev_sel;
                    draw_popup_window( ds.popwin, &dpui );
                }

                /* toggle on the new selection with highlight */
                dpui.line = index;
                draw_popup_window( ds.popwin, &dpui );                 
            } 
            else /* newly selected item is not in the cureent sceen, show the new selected item in the screen */
            {
                /* newly selected item becomes the first item in the screen */
                if ((popwin->nitems - index) >= popwin->max_lines)
                {
                    popwin->indexFirst = index;
                }
                else /* compose the screen with the last items */
                {
                    popwin->indexFirst = popwin->nitems - popwin->max_lines;
                }
                dpui.type = DPU_LISTALL;
                draw_popup_window( ds.popwin, &dpui );                 
            }
        }
    }
    else if (popwin->type == POPUP_MENU)
    {
        if (popwin->indexFocused != index)
        {
            /* Move the focus to the new selection */
            if (IS_IN_SCREEN(index, popwin->indexFirst, popwin->max_lines))
            {
                popwin->indexFocused = index;
                dpui.type = DPU_LISTLINE; 
                dpui.line = index;
                draw_popup_window( ds.popwin, &dpui );
            }
        } 
        else 
        {
            popwin->indexFocused = index;
        }
    }
}


/*==================================================================================================
    FUNCTION: ds_set_popup_check

    DESCRIPTION:
        Toggle a checkbox icon (exclusively select) in the popup list.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
void ds_set_popup_check 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         index,
    OP_BOOLEAN       onoff
)
{
    DS_POPUP_DRAW_INFO_T    dpui;

    if (!popwin || popwin->handle == PHANDLE_NONE) 
    {
        return;
    }
  
    if (onoff)
    {
        popwin->indexCheck |= (1<<index);
        /* Refresh new check */
        if ( ( index >= popwin->indexFirst) &&
              (index <= (popwin->indexFirst + popwin->max_lines)))
        {
            dpui.type = DPU_LISTLINE; 
            dpui.line = index;
            draw_popup_window( ds.popwin, &dpui );
        }
    }
    else
    {
        popwin->indexCheck &= ~(1<<index);
        /* Refresh the uncheck */
        if (IS_IN_SCREEN(index, popwin->indexFirst, popwin->max_lines))
        {
            dpui.type = DPU_LISTLINE; 
            dpui.line = index;
            draw_popup_window( ds.popwin, &dpui );
        }

    }
}


/*==================================================================================================
    FUNCTION: ds_get_popup_highlight

    DESCRIPTION:
        Get the current highlight index of the passed popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
OP_UINT16 ds_get_popup_highlight 
(
    DS_POPUPWIN_T    *popwin
)
{
    if (!popwin || popwin->handle == PHANDLE_NONE) 
    {
        return INDEX_NONE;
    }
    return (OP_UINT16)popwin->indexFocused;
}

/*==================================================================================================
    FUNCTION: ds_set_popup_highlight

    DESCRIPTION:
        Set highlight of a popup item.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
void ds_set_popup_highlight 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         index
)
{
    OP_UINT8                prev_focused;
    DS_POPUP_DRAW_INFO_T    dpui;

    if (!popwin || popwin->handle == PHANDLE_NONE) 
    {
        return;
    }
  

    /* Refresh the focus change */
    if (IS_IN_SCREEN(index, popwin->indexFirst, popwin->max_lines))
    {
        prev_focused = popwin->indexFocused;

        /* update new focus */
        popwin->indexFocused = index;

        dpui.type = DPU_LISTLINE; 

        /* unhighlight the previous focus */
        dpui.line = prev_focused - popwin->indexFirst;
        draw_popup_window( ds.popwin, &dpui );

        /* highlight the new focus */
        dpui.line = popwin->indexFocused - popwin->indexFirst;
        draw_popup_window( ds.popwin, &dpui );
    }
    else
    {
        /* is this new focus can be first index */
        if ( (popwin->nitems - index) >= popwin->max_lines)
        {
            popwin->indexFirst = index;
            popwin->indexFocused = index; 
            dpui.type = DPU_LISTALL; 
            draw_popup_window( ds.popwin, &dpui );
        } 
        else
        {
            /* then, make the last item last line */
            OP_UINT8 last_index;
            popwin->indexFocused = index; 
            last_index = (popwin->nitems -1);
            popwin->indexFirst = last_index - (popwin->max_lines -1) ;
            dpui.type = DPU_LISTALL; 
            draw_popup_window( ds.popwin, &dpui );
        }      
    }
}


/*==================================================================================================
    FUNCTION: ds_get_popup_edited_number

    DESCRIPTION:
        Returns the edited digits through POPUP_NUMEDITOR. These digits are effective after the user 
        presses OK key (store key).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        The returned number shall be copied by the caller. 
==================================================================================================*/
OP_UINT8 *ds_get_popup_edited_digits 
(
    DS_POPUPWIN_T    *popwin
)
{
    if (!popwin || popwin->handle == PHANDLE_NONE || popwin->type != POPUP_NUMEDITOR) 
    {
        return OP_NULL;
    }

    return popwin->numeditor->digits;
}


/*==================================================================================================
    FUNCTION: ds_close_popup

    DESCRIPTION:
        Close the currently active popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
void ds_close_popup 
( 
    void 
)
{
    if (!ds.popwin || ds.popwin->handle == PHANDLE_NONE)
    {
        return;
    }
    if (ds.popwin->type == POPUP_MSG)
    {
        OPUS_Stop_Timer(OPUS_TIMER_POPUP_MSG);
    }
    close_popup_which (ds.popwin);
}


/*==================================================================================================
    FUNCTION: ds_popup_message

    DESCRIPTION:
        Create a message popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
       It is used to open a simple message Popup window such as for warning or temporal messages.
       If the timer is larger than "0", the popup window is automatically closed when the timer
       expires.

       If the timer is less than or equal to  "0", the caller shoud close te popup.

       If you want to open a popup which is continuously alive while you process, 
       use ds_popup_static().
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_message 
(
    OP_UINT8    *title,
    OP_UINT8    *msg,
    OP_UINT16   timer
    
)
{
    return (DS_POPUPWIN_T *)(ds_new_popup_message(title, msg, POPUP_WARNING, timer, OP_NULL));    
}

DS_POPUPWIN_T * ds_new_popup_message 
(
    OP_UINT8    *title,
    OP_UINT8    *msg,
    OP_UINT16     style,
    OP_UINT16   timer,
    OP_BOOLEAN          (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)    
)
{
    DS_POPUPWIN_T    *popwin;
    popwin = create_popup(OP_NULL);

    if (!popwin) 
    {
        return OP_NULL;
    }

    popwin->style = style;
    if(style == POPUP_CONFIRM)
    {
        popwin->type = POPUP_DIALOG;
    }
    if(style == POPUP_WARNING)
    {
        popwin->type = POPUP_MSG;
    }
    popwin->event_handler = event_handler;
    if (title)
    {
        UstrNullCpy(popwin->title, title, DS_SCRN_POPUP_MAX_UC_LEN);
    }
    if (msg)
    {
        UstrNullCpy(popwin->msg, msg, DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE);
    }
    if (timer > 0) 
    {
        popwin->timer = timer;
        open_popup ( popwin );
        /* start the message timer */
        if (popwin->style != POPUP_CONFIRM)
        {
            OPUS_Start_Timer(OPUS_TIMER_POPUP_MSG, timer, 0,  ONE_SHOT);
        }
    }
    else
    {
        open_popup ( popwin );
    }
       
    return popwin;
}

/*==================================================================================================
    FUNCTION: ds_popup_message_rm

    DESCRIPTION:
        Cleate a message popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
       It is used to open a simple message Popup window such as for warning or temporal messages.
       If the timer is larger than "0", the popup window is automatically closed when the timer
       expires.

       If the timer is less than or equal to  "0", the caller shoud close te popup.

       If you want to open a popup which is continuously alive while you process, 
       use ds_popup_static().
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_message_rm 
(
    RM_RESOURCE_ID_T    title_res_id,
    RM_RESOURCE_ID_T    msg_res_id,
    OP_UINT16           timer
)
{
    DS_POPUPWIN_T    *pw        = OP_NULL;
    OP_UINT8         *title_ptr = OP_NULL; 

    OP_UINT8 tmp_title[DS_SCRN_POPUP_MAX_UC_LEN+2];
    OP_UINT8 tmp_msg[DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE+2]; 

    title_ptr = (util_get_text_from_res_w_max_len(title_res_id, tmp_title,DS_SCRN_POPUP_MAX_UC_LEN ));

    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(msg_res_id,tmp_msg, 
                                              DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE )))
    {
        pw = ds_popup_message(title_ptr, tmp_msg, timer);
    }

    return pw;

}

/*==================================================================================================
    FUNCTION: ds_popup_static

    DESCRIPTION:
        Open a static popup window. It is alive until it is closed.

    ARGUMENTS PASSED:
        *itle       : popup title
        *sg,        : popup message 
        x           : popup x starting coordinate. If it is 0, it is centered.
        y           : popup y starting coordinate. If it is 0, it is centered.

    RETURN VALUE:

    IMPORTANT NOTES:
        Use ds_popup_message() for simple messages (warning...).
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_static 
(
    OP_UINT8    *title,
    OP_UINT8    *msg,
    OP_INT16    x,            
    OP_INT16    y             
)
{
    
#if 0    
    popwin = create_popup(OP_NULL);

    if (!popwin)
    { 
        return OP_NULL;
    }


    popwin->type = POPUP_MSG;
    if (title)
    {
        //op_strcpy((char *)popwin->title, (char *)title);
        Ustrcpy(popwin->title, title);
    }
    if (msg)
    {
        //op_strcpy((char *)popwin->msg, (char *)msg);
        Ustrcpy(popwin->msg, msg);
    }
#endif

    // popwin->attr |= PUA_STATIC;    
    // ds_set_popup_point(popwin, x, y);
    return (DS_POPUPWIN_T *)(ds_new_popup_message(title, msg, 
                                               POPUP_WARNING, 0, OP_NULL));    
}


/*==================================================================================================
    FUNCTION: ds_popup_static_rm

    DESCRIPTION:
        Open a static popup window. It is alive until it is closed.

    ARGUMENTS PASSED:
        *itle       : popup title
        *sg,        : popup message 
        x           : popup x starting coordinate. If it is 0, it is centered.
        y           : popup y starting coordinate. If it is 0, it is centered.

    RETURN VALUE:

    IMPORTANT NOTES:
        Use ds_popup_message() for simple messages (warning...).
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_static_rm 
(
    RM_RESOURCE_ID_T    title_res_id,
    RM_RESOURCE_ID_T    msg_res_id,
    OP_INT16            x,            
    OP_INT16            y             
)
{
    DS_POPUPWIN_T    *pw        = OP_NULL;
    OP_UINT8         *title_ptr = OP_NULL; 

    OP_UINT8 tmp_title[DS_SCRN_POPUP_MAX_UC_LEN+2];
    OP_UINT8 tmp_msg[DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE+2]; 

    title_ptr = util_get_text_from_res_w_max_len(title_res_id,tmp_title, DS_SCRN_POPUP_MAX_UC_LEN );

    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(msg_res_id,tmp_msg, DS_SCRN_POPUP_MAX_UC_LEN )))
    {
        pw = ds_popup_static(title_ptr, tmp_msg, x, y);
    }

    return pw;
}

/*==================================================================================================
    FUNCTION: ds_popup_static_redraw

    DESCRIPTION:
        Redraw an already opened static popup window with the new message.
        Use this function if the message of a static popup is changes.

    ARGUMENTS PASSED:
        *popwin    : the static popup window.
        *msg       : new message. If it is null, the old message remains.
    RETURN VALUE:

    IMPORTANT NOTES:
       None
==================================================================================================*/
void ds_popup_static_redraw 
( 
    DS_POPUPWIN_T    *popwin, 
    OP_UINT8         *msg 
)
{
    DS_POPUP_DRAW_INFO_T    dpui;
    
    if (!popwin) 
    {
        return;
    }

    if (msg)
    { 
        UstrNullCpy(popwin->msg, msg, DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE);
    }

    dpui.type = DPU_INIT;
    draw_popup_window(popwin, &dpui);

}


/*==================================================================================================
    FUNCTION: ds_popup_static_redraw_rm

    DESCRIPTION:
        Redraw an already opened static popup window with the new message.
        Use this function if the message of a static popup is changes.

    ARGUMENTS PASSED:
        *popwin    : the static popup window.
        *msg       : new message. If it is null, the old message remains.
    RETURN VALUE:

    IMPORTANT NOTES:
       None
==================================================================================================*/
void ds_popup_static_redraw_rm 
( 
    DS_POPUPWIN_T       *popwin, 
    RM_RESOURCE_ID_T    msg_res_id 
)
{
    OP_UINT8 tmp_msg[DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE+2]; 

    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(msg_res_id,tmp_msg, DS_SCRN_POPUP_MAX_UC_LEN )))
    {
        ds_popup_static_redraw(popwin, tmp_msg);
    }
}

/*==================================================================================================
    FUNCTION: ds_popup_dialog

    DESCRIPTION:
        Create and open an dialog type popup. (ask user's action, for example YES and NO).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_dialog 
(
    OP_UINT8      *title,
    OP_UINT8      *msg,
    OP_BOOLEAN    (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
)
{
    DS_POPUPWIN_T    *popwin;

   /*  popwin->event_handler = event_handler;
        if do this, will access the forbidden area    */
    popwin= ds_new_popup_message(title, msg, POPUP_CONFIRM, 0, event_handler);

    return popwin;
}


/*==================================================================================================
    FUNCTION: ds_popup_dialog_rm

    DESCRIPTION:
        Create and open an dialog type popup. (ask user's action, for example YES and NO).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_dialog_rm 
(
    RM_RESOURCE_ID_T    title_res_id,
    RM_RESOURCE_ID_T    msg_res_id,
    OP_BOOLEAN          (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)
)
{

    DS_POPUPWIN_T    *pw        = OP_NULL;
    OP_UINT8         *title_ptr = OP_NULL; 

    OP_UINT8 tmp_title[DS_SCRN_POPUP_MAX_UC_LEN+2];
    OP_UINT8 tmp_msg[DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE+2]; 


    title_ptr = util_get_text_from_res_w_max_len(title_res_id,tmp_title, DS_SCRN_POPUP_MAX_UC_LEN );

    if ( (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(msg_res_id,tmp_msg, DS_SCRN_POPUP_MAX_UC_LEN ))))
    {
        pw = ds_popup_dialog(title_ptr, tmp_msg, event_handler);
    }
    return pw;
}


/*==================================================================================================
    FUNCTION: ds_popup_menu

    DESCRIPTION:
        Create and open an menu popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
DS_POPUPWIN_T * ds_popup_menu 
(
    DS_POPUP_MENU_TIER_T    *menu,
    OP_BOOLEAN              (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
)
{
    DS_POPUPWIN_T    *popwin;
    
    popwin = create_popup(OP_NULL);
  
    if (!popwin) 
    {
        return OP_NULL;
    }
    popwin->type = POPUP_MENU;
    
    /* get title string */
    if (IS_NOT_NULL_STRING(menu->title))
    {
        UstrNullCpy(popwin->title, menu->title, DS_SCRN_POPUP_MAX_UC_LEN);
        /* get title icon */
        popwin->icon = util_get_icon_from_res(menu->title_icon_res_id);
    }

    popwin->menu = menu;

    popwin->nitems = menu->menu_entry_siz;

    popwin->event_handler = event_handler;
  
    open_popup(popwin);
  
    return popwin;
}

/*==================================================================================================
    FUNCTION: ds_popup_menu_rm

    DESCRIPTION:
        Create and open an dialog type popup. (ask user's action, for example YES and NO).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_menu_rm 
(
    DS_POPUP_MENU_TIER_T    *menu,
    OP_BOOLEAN              (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
)
{
    DS_POPUPWIN_T           *popwin;
    OP_UINT8                 menu_cnt;

    OP_UINT8                 tmp_txt[DS_SCRN_POPUP_MAX_UC_LEN+2];

    DS_POPUP_MENU_ENTRY_T    *index_menu;
   
    popwin = create_popup(OP_NULL);
  
    if (!popwin) 
    {
        return OP_NULL;
    }
    popwin->type = POPUP_MENU;
    
    /* get title string */
    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(menu->title_res_id,tmp_txt, DS_SCRN_POPUP_MAX_UC_LEN )))
    {
        //op_strcpy((char *)popwin->title, (char *)tmp_txt);
        Ustrcpy(popwin->title, tmp_txt);
        /* get title icon */
        popwin->icon = util_get_icon_from_res(menu->title_icon_res_id);
    }

    //popwin->nitems = menu->menu_entry_siz-1; 
    popwin->nitems = menu->menu_entry_siz;

    index_menu = (DS_POPUP_MENU_ENTRY_T *)menu->menu_data_ptr;
    //for (menu_cnt = 0; menu_cnt <= popwin->nitems; menu_cnt++) 
    for (menu_cnt = 0; menu_cnt < popwin->nitems; menu_cnt++)        
    {
        if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(index_menu[menu_cnt].txt_res_id,tmp_txt, DS_SCRN_POPUP_MAX_UC_LEN )))
        {
            //op_strcpy((char *)(index_menu[menu_cnt].txt), (char *)tmp_txt);
            Ustrcpy((index_menu[menu_cnt].txt), tmp_txt);
        }
        else
        {
            Umemset( index_menu[menu_cnt].txt, 0x0000,1);
        }
  }
    popwin->menu = menu;

    popwin->event_handler = event_handler;
  
    open_popup(popwin);
  
    return popwin;

}

/*==================================================================================================
    FUNCTION: ds_popup_list

    DESCRIPTION:
        Create and open an list popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
DS_POPUPWIN_T * ds_popup_list 
(
    DS_POPUP_LIST_T    *list,
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
)
{
    DS_POPUPWIN_T    *popwin;
    
    popwin = create_popup(OP_NULL);

    if (!popwin) 
    {
        return OP_NULL;
    }
    popwin->type = POPUP_LIST;

    /* get title string */
    if (IS_NOT_NULL_STRING(list->title))
    {
        UstrNullCpy(popwin->title, list->title, DS_SCRN_POPUP_MAX_UC_LEN);
        /* get title icon */
        popwin->icon = util_get_icon_from_res(list->title_icon_res_id);
    }

    popwin->list = list;
    popwin->nitems = list->nitems;
    popwin->event_handler = event_handler;

    /* set initial selection */
    if (list->init_sel_item < list->nitems)
    {
        ds_set_popup_selection(popwin, list->init_sel_item);
    }

    open_popup ( popwin );

    return popwin;
}


/*==================================================================================================
    FUNCTION: ds_popup_list_rm

    DESCRIPTION:
        Create and open an list popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
DS_POPUPWIN_T * ds_popup_list_rm 
(
    DS_POPUP_LIST_T    *list,
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
)
{
    DS_POPUPWIN_T    *popwin;
    OP_UINT8         list_cnt;

    OP_UINT8         tmp_txt[DS_SCRN_POPUP_MAX_UC_LEN+2];
   
    popwin = create_popup(OP_NULL);
  
    if (!popwin) 
    {
        return OP_NULL;
    }
    popwin->type = POPUP_LIST;
    
    /* get title string */
    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(list->title_res_id,tmp_txt, DS_SCRN_POPUP_MAX_UC_LEN )))
    {
        //op_strcpy((char *)popwin->title, (char *)tmp_txt);
        Ustrcpy(popwin->title, tmp_txt);
        /* get title icon */
        popwin->icon = util_get_icon_from_res(list->title_icon_res_id);
    }

    popwin->nitems = list->nitems;

    for (list_cnt = 0; list_cnt <= popwin->nitems; list_cnt++)
    {
        if (util_cnvrt_res_into_text(list->items[list_cnt].txt_res_id, tmp_txt ) != OP_NULL)
        {
            //op_strcpy((char *)list->items[list_cnt].txt, (char *)tmp_txt);
            Ustrcpy(list->items[list_cnt].txt, tmp_txt);
        }
        else
        {
            Umemset( list->items[list_cnt].txt, 0x0000,1);
        }
    }
    popwin->list = list;

    popwin->event_handler = event_handler;
  
    /* set initial selection */
    if (list->init_sel_item < list->nitems)
    {
        ds_set_popup_selection(popwin, list->init_sel_item);
    }

    open_popup(popwin);
  
    return popwin;
}

/*==================================================================================================
    FUNCTION: ds_popup_list_ex

    DESCRIPTION:
        Create and open an user data list popup.

    ARGUMENTS PASSED:
        *list         : list
        attr          : list attributes 
        nitems        : total number of items in the lust
        (*event_handler)( DS_POPUP_STATE_ENUM_T state, int event ) : event handler of the list.
        (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data ) : user data handler. First
                      parameter is a user data type and the second parameter is the line index.

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
DS_POPUPWIN_T * ds_popup_list_ex 
(
    DS_POPUP_LIST_T    *list,
    POPUPATTR          attr,
    OP_UINT8           nitems,     
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event ),
    OP_UINT32          (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data )
)
{
    DS_POPUPWIN_T    *popwin;
    
    popwin = create_popup(OP_NULL);

    if (!popwin) 
    {
        return OP_NULL;
    }

    popwin->type = POPUP_LIST;
    if (list) 
    {  /* general popup list */
        popwin->attr = PUA_DEFAULT|attr;

        /* get title string */
        if (IS_NOT_NULL_STRING(list->title))
        {
            UstrNullCpy(popwin->title, list->title, DS_SCRN_POPUP_MAX_UC_LEN);
            /* get title icon */
            popwin->icon = util_get_icon_from_res(list->title_icon_res_id);
        }

        popwin->list = list;
        popwin->nitems = list->nitems;
    }
    else 
    {   /* user defined popup list, the popup will be passed later. */
        popwin->attr = PUA_DEFAULT|PUA_USERLIST|attr;
        Umemset(popwin->title, 0x0000,1);
        popwin->list = OP_NULL;
        popwin->nitems = nitems;
        popwin->userdata_handler = userdata_handler;
    }
    
    popwin->event_handler = event_handler;
    open_popup ( popwin );
    return popwin;
}


/*==================================================================================================
    FUNCTION: ds_popup_list_ex_rm

    DESCRIPTION:
        Create and open an user data list popup.

    ARGUMENTS PASSED:
        *list         : list
        attr          : list attributes 
        nitems        " total number of items in the lust
        (*event_handler)( DS_POPUP_STATE_ENUM_T state, int event ) : event handler of the list.
        (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data ) : user data type and data.

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
DS_POPUPWIN_T * ds_popup_list_ex_rm 
(
    DS_POPUP_LIST_T    *list,
    POPUPATTR          attr,
    OP_UINT8           nitems,     
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event ),
    OP_UINT32          (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data )
)
{
    DS_POPUPWIN_T    *popwin;
    OP_UINT8         list_cnt;

    OP_UINT8         tmp_txt[DS_SCRN_POPUP_MAX_UC_LEN+2];
   
    popwin = create_popup(OP_NULL);

    if (!popwin) 
    {
        return OP_NULL;
    }

    popwin->type = POPUP_LIST;

    if (list) 
    {  /* general popup list */
        popwin->attr = PUA_DEFAULT|attr;
        if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(list->title_res_id,tmp_txt, DS_SCRN_POPUP_MAX_UC_LEN )))
        {
            //op_strcpy((char *)popwin->title, (char *)tmp_txt);
            Ustrcpy(popwin->title, tmp_txt);
            /* get title icon */
            popwin->icon = util_get_icon_from_res(list->title_icon_res_id);
        }

        popwin->nitems = list->nitems;
    
        for (list_cnt = 0; list_cnt <= popwin->nitems; list_cnt++)
        {
            if (util_cnvrt_res_into_text(list->items[list_cnt].txt_res_id,tmp_txt ) != OP_NULL)
            {
                //op_strcpy((char *)list->items[list_cnt].txt, (char *)tmp_txt);
                Ustrcpy(list->items[list_cnt].txt, tmp_txt);
            }
            else
            {
                Umemset(list->items[list_cnt].txt, 0x0000,1);
            }
        }

        popwin->list = list;
    }
    else 
    {   /* user defined popup list, the popup will be passed later. */
        popwin->attr = PUA_DEFAULT|PUA_USERLIST|attr;
        Umemset(popwin->title, 0x0000,1);
        popwin->list = OP_NULL;
        popwin->nitems = nitems;
        popwin->userdata_handler = userdata_handler;
    }
    
    popwin->event_handler = event_handler;
    open_popup ( popwin );
    return popwin;
}

/*==================================================================================================
    FUNCTION: ds_popup_ownerdraw

    DESCRIPTION:
        Create and open an owner draw popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_ownerdraw 
(
    OP_BOOLEAN    (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event ),
    void          (*ownerdraw_func)( DS_POPUPWIN_T *pw, DS_POPUP_DRAW_INFO_T *dpu)
)
{
    DS_POPUPWIN_T    *popwin;
    popwin = create_popup(OP_NULL);

    if (!popwin) 
    {
        return OP_NULL;
    }

    popwin->type = POPUP_OWNERDRAW;
    popwin->event_handler = event_handler;
    popwin->ownerdraw_func = ownerdraw_func;

    open_popup ( popwin );
    return popwin;
}


/*==================================================================================================
    FUNCTION: ds_popup_numeditor

    DESCRIPTION:
        Create and open an num editor type popup. (only accept digits).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_numeditor 
(
    DS_POPUP_NUMEDITOR_T      *numeditor,
    OP_BOOLEAN    (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
)
{
    DS_POPUPWIN_T    *popwin;
    
    popwin = create_popup(OP_NULL);

    if (!popwin) 
    {
        return OP_NULL;
    }
    popwin->type = POPUP_NUMEDITOR;

    /* get title string */
    if (IS_NOT_NULL_STRING(numeditor->title))
    {
        UstrNullCpy(popwin->title, numeditor->title, DS_SCRN_POPUP_MAX_UC_LEN);
    }

    if ( (numeditor->max_leng == 0) || (numeditor->max_leng >DS_SCRN_POPUP_MAX_DIGIT_LEN))
    {
        numeditor->max_leng = DS_SCRN_POPUP_MAX_DIGIT_LEN;
    }

    if (IS_NOT_NULL_STRING(numeditor->digits))
    {
        numeditor->pos = MIN(DS_SCRN_POPUP_MAX_DIGIT_LEN-1, UstrCharCount(numeditor->digits));
        numeditor->pos = MIN(numeditor->pos, numeditor->max_leng-1);
    }
    else
    {
        numeditor->pos = 0;
        op_memset(numeditor->digits, 0x00, DS_SCRN_POPUP_MAX_DIGIT_UC_LEN+2);
    }

    popwin->numeditor = numeditor;

    popwin->event_handler = event_handler;

    open_popup ( popwin );

    return popwin;
}


/*==================================================================================================
    FUNCTION: ds_popup_numeditor_rm

    DESCRIPTION:
        Create and open an num editor type popup. (only accept digits).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_numeditor_rm 
(
    DS_POPUP_NUMEDITOR_T    *numeditor,
    OP_BOOLEAN          (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)
)
{
    DS_POPUPWIN_T    *popwin;
    OP_UINT8         tmp_title[DS_SCRN_POPUP_MAX_UC_LEN+2];
    OP_UINT8         tmp_msg[DS_SCRN_POPUP_MAX_UC_LEN*DS_SCRN_POPUP_MAXLINE+2]; 
    
    popwin = create_popup(OP_NULL);

    if (!popwin) 
    {
        return OP_NULL;
    }
    popwin->type = POPUP_NUMEDITOR;

    if (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(numeditor->title_res_id,tmp_title, DS_SCRN_POPUP_MAX_UC_LEN )))
    {
        Ustrcpy(popwin->title, tmp_title);
    }

    if ( (IS_NOT_NULL_STRING(util_get_text_from_res_w_max_len(numeditor->msg_res_id,tmp_msg, DS_SCRN_POPUP_MAX_UC_LEN ))))
    {
        Ustrcpy(numeditor->msg, tmp_msg);
    }

    if ( (numeditor->max_leng == 0) || (numeditor->max_leng >DS_SCRN_POPUP_MAX_DIGIT_LEN))
    {
        numeditor->max_leng = DS_SCRN_POPUP_MAX_DIGIT_LEN;
    }

    if (IS_NOT_NULL_STRING(numeditor->digits))
    {
        numeditor->pos = MIN(DS_SCRN_POPUP_MAX_DIGIT_LEN-1, UstrCharCount(numeditor->digits));
        numeditor->pos = MIN(numeditor->pos, numeditor->max_leng-1);
    }
    else
    {
        numeditor->pos = 0;
        op_memset(numeditor->digits, 0x00, DS_SCRN_POPUP_MAX_DIGIT_UC_LEN+2);
    }

    popwin->numeditor = numeditor;

    popwin->event_handler = event_handler;

    open_popup ( popwin );

    return popwin;
}


/*==================================================================================================
    FUNCTION: ds_popup_by

    DESCRIPTION:
        Create and open a popup with the user passed popup data.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
DS_POPUPWIN_T * ds_popup_by 
(
  DS_POPUP_DATA_T    *popdata
)
{
    DS_POPUPWIN_T    *popwin;

    popwin = create_popup(popdata);

    if (!popwin) 
    {
        return OP_NULL;
    }

    open_popup ( popwin );
    return popwin;
}

/*==================================================================================================
    FUNCTION: ds_popup_scroll_select

    DESCRIPTION:
        the function for the pen select event,
        redraw the focused item and lost focused item.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
DS_POPUP_PDEH_RET_ENUM_T ds_popup_scroll_selected
(
    DS_POPUPWIN_T     *pw,
    void               *EvData 
)
{
    DS_POPUP_DRAW_INFO_T       dpui;
    TPO_OPERATION_PARAM_STRUCTURE_T * pOP;

    pOP = (TPO_OPERATION_PARAM_STRUCTURE_T *)EvData;

    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }


    if(pOP->id + pw->indexFirst == pw->indexFocused)
    {
        return POPUP_PROC_EVENT;
    }
    else
    {
        dpui.type = DPU_LISTLINE;
        dpui.line = pw->indexFocused - pw->indexFirst;

        pw->indexFocused = pOP->id + pw->indexFirst;
        draw_popup_window(pw, &dpui);

        dpui.line = pOP->id;
        draw_popup_window(pw, &dpui);
        
    }
    return POPUP_PROC_EVENT;
 
    
}

/*==================================================================================================
    FUNCTION: ds_popup_scroll_up

    DESCRIPTION:
        do with the popup scrollbar scroll up.
        the display item is moved up,

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
DS_POPUP_PDEH_RET_ENUM_T ds_popup_scroll_line_up
(
    DS_POPUPWIN_T     *pw
)
{
    DS_POPUP_DRAW_INFO_T       dpui;
     
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    /*
    *	if the current index first is 0,the screen do not move up.
    */
    if(0 == pw->indexFirst )
    {
        if( 0 < pw->indexFocused)
        {
            dpui.type = DPU_LISTLINE;
            pw->indexFocused--;
            dpui.line = pw->indexFocused +1;
            draw_popup_window(pw, &dpui);

            
            dpui.line = pw->indexFocused;
            draw_popup_window(pw, &dpui);
        }
        else
        {
            return POPUP_PROC_EVENT;
        }
        
    }
    else
    {
        pw->indexFirst--;
        pw->indexFocused--;
        dpui.type = DPU_LISTALL;
        draw_popup_window(pw, &dpui);
    }
    return POPUP_PROC_EVENT;
 
    
}

/*==================================================================================================
    FUNCTION: ds_popup_scroll_down

    DESCRIPTION:
        do with the popup scrollbar scroll down.
        the display item is moved down
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
DS_POPUP_PDEH_RET_ENUM_T ds_popup_scroll_line_down
(
    DS_POPUPWIN_T     *pw
)
{
    DS_POPUP_DRAW_INFO_T       dpui;
    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    /*
    *	if the current index first is the last screen,the screen do not move down.
    */
    if((pw->nitems - pw->max_lines) == pw->indexFirst ) 
       
    {
        if(pw->indexFocused < pw->nitems -1)
        {
            dpui.type = DPU_LISTLINE;
             pw->indexFocused++;

            dpui.line = pw->indexFocused - pw->indexFirst -1;
            draw_popup_window(pw, &dpui);

          
            dpui.line++;
            draw_popup_window(pw, &dpui);
        }
        else
        {
             return POPUP_PROC_EVENT;
        }
    }
    else
    {
        pw->indexFirst++;
        pw->indexFocused++;
        dpui.type = DPU_LISTALL;
        draw_popup_window(pw, &dpui);
    }
    return POPUP_PROC_EVENT;
 
    
}
/*==================================================================================================
    FUNCTION: ds_popup_scroll_page_up

    DESCRIPTION:
        do with the popup scrollbar page up.
        the display item is moved down
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
DS_POPUP_PDEH_RET_ENUM_T ds_popup_scroll_page_up
(
    DS_POPUPWIN_T     *pw
)
{
    DS_POPUP_DRAW_INFO_T       dpui;
    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    /*
    *	if the current index first is the last screen,the screen do not move down.
    */
    if(0 == pw->indexFirst)
    {
        if(pw->indexFocused != 0)
        {
            dpui.type = DPU_LISTLINE;
            
            dpui.line = pw->indexFocused ;
            pw->indexFocused=0;
            draw_popup_window(pw, &dpui);
            
           
            dpui.line=0;
            draw_popup_window(pw, &dpui);            
        }
        else
        {
           return POPUP_PROC_EVENT;
        }
    }
    else
    {
        /*
         *	
         */
        OP_UINT8    old_indexFirst = pw->indexFirst;
        if(pw->indexFirst <= pw->max_lines -1)
        {
            pw->indexFirst =0;
            pw->indexFocused -= old_indexFirst ; 
        }
        else
        {
            pw->indexFirst -= pw->max_lines;
            pw->indexFocused -=pw->max_lines ;
        }
        dpui.type = DPU_LISTALL;
        draw_popup_window(pw, &dpui);
    }
    return POPUP_PROC_EVENT;
 
    
}
/*==================================================================================================
    FUNCTION: ds_popup_page_down

    DESCRIPTION:
        do with the popup scrollbar page down.
        the display item is moved down
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
DS_POPUP_PDEH_RET_ENUM_T ds_popup_scroll_page_down
(
    DS_POPUPWIN_T     *pw
)
{
    DS_POPUP_DRAW_INFO_T       dpui;
    
    if (!pw || pw->handle == PHANDLE_NONE) 
    {
        return POPUP_INVALID;
    }
    /*
    *	if the current index first is the last screen,the screen do not move down.
    */
    if((pw->nitems - pw->max_lines) == pw->indexFirst)
    {
        if(pw->indexFocused != pw->nitems-1)
        {
            dpui.type = DPU_LISTLINE;
            dpui.line = pw->indexFocused - pw->indexFirst;
            pw->indexFocused = pw->nitems-1;
            draw_popup_window(pw, &dpui);
            
            
            dpui.line = pw->indexFocused - pw->indexFirst ;
            draw_popup_window(pw, &dpui);
        }
        else
        {
             return POPUP_PROC_EVENT;
        }
    }
    else
    {
        OP_UINT8    old_indexFirst = pw->indexFirst;
       /*
        *   the move screen is the last screen and the last can be enough to display 
        */
        if(pw->indexFirst + pw->max_lines >= pw->nitems - pw->max_lines-1 )
        {
            pw->indexFirst = pw->nitems - pw->max_lines;
            pw->indexFocused += (pw->indexFirst - old_indexFirst);
            
        }
        else
        {
            pw->indexFirst += pw->max_lines;
            pw->indexFocused += pw->max_lines;
        }
        dpui.type = DPU_LISTALL;
        draw_popup_window(pw, &dpui);
    }
    return POPUP_PROC_EVENT;
 

}
/*==================================================================================================
    FUNCTION: ds_popup_operation_event_handler

    DESCRIPTION:
      OPUS_FOCUS_OPERATION event dispacher. 
    
      *pw    : popup pointer
      *EvData : event data pointer
      
    RETURN VALUE:
        Returns whether the passed event was processed or not. If the event was processed by the 
        popup event handler, the event was poped-up.
        
        The return value is one of DS_POPUP_PDEH_RET_ENUM_T and each has meaning in the below.
          
           POPUP_INVALID      : there is no active popup. 
           POPUP_CLOSE,       : The popup is no longer effective and is closed by the popup event handler.
           POPUP_PROC_EVENT   : The passed event was handled, so remove it after it returns.
           POPUP_UNPROC_EVENT : The passed event was not handled by the popup event handler,
    
              
    IMPORTANT NOTES:
        None.          
==================================================================================================*/

DS_POPUP_PDEH_RET_ENUM_T ds_popup_operation_event_handler
(
  DS_POPUPWIN_T           *pw,
  void                    *EvData 
)
{
    TPO_OPERATION_PARAM_STRUCTURE_T * pOP;
    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_UNPROC_EVENT;
    if(POPUP_MSG == pw->type && 
       (pw->attr & PUA_STATIC) )
    {
        return p_ret_state;
    }
    pOP =  (TPO_OPERATION_PARAM_STRUCTURE_T *)EvData;
    switch(pOP->op)
    {
    case OPERATION_ID_PL_SELECT:
        p_ret_state = ds_popup_scroll_selected(pw,EvData);
        break;
    case OPERATION_ID_PL_CONFIRM:
        if(pw->indexFocused - pw->indexFirst  == pOP->id)
        {
            p_ret_state = pass_popup_event(pw, KEY_OK);
        }
        break;
    case OPERATION_ID_PL_LINEUP:
        p_ret_state = ds_popup_scroll_line_up(pw);
        break;
    case OPERATION_ID_PL_LINEDN:
        p_ret_state = ds_popup_scroll_line_down(pw);
        break;
    case OPERATION_ID_PL_PAGEUP: 
        p_ret_state = ds_popup_scroll_page_up(pw);
        break;
    case OPERATION_ID_PL_PAGEDN:
        p_ret_state = ds_popup_scroll_page_down(pw);
        break;
    case OPERATION_ID_SK_SELECT:
        break;
    case OPERATION_ID_SK_CONFIRM:
        {
            if(POPUP_MSG == pw->type && pOP->id != COMMAND_SOFTKEY_RIGHT)
            {
                return p_ret_state;
            }
            else if(POPUP_MSG == pw->type && pOP->id == COMMAND_SOFTKEY_RIGHT)
            {
                p_ret_state = pass_popup_event(pw, KEY_SOFT_RIGHT);
                return p_ret_state;
            }    
            switch(pOP->id)
            {
                
             case COMMAND_SOFTKEY_LEFT:
                 p_ret_state = pass_popup_event(pw, KEY_SOFT_LEFT);
                 break;
            /*
             *  the confirm softkey is selected
             */
            case COMMAND_SOFTKEY_OK:
                p_ret_state = pass_popup_event(pw, KEY_OK);
                break;
            /*
             *  the cancel softkey is selected
             */
            case COMMAND_SOFTKEY_RIGHT:
                p_ret_state = pass_popup_event(pw, KEY_SOFT_RIGHT);
                break;
            }
            
        }
        break;
    }
    return p_ret_state;
    
}
/*==================================================================================================
    FUNCTION: ds_pass_event_to_popup

    DESCRIPTION:
        Popup event dispacher. 

    ARGUMENTS PASSED:
        *pw    : popup pointer
        event  : event
        *EvData : event data pointer

    RETURN VALUE:
        Returns whether the passed event was processed or not. If the event was processed by the 
        popup event handler, the event was poped-up.

        The return value is one of DS_POPUP_PDEH_RET_ENUM_T and each has meaning in the below.

        POPUP_INVALID      : there is no active popup. 
        POPUP_CLOSE,       : The popup is no longer effective and is closed by the popup event handler.
        POPUP_PROC_EVENT   : The passed event was handled, so remove it after it returns.
        POPUP_UNPROC_EVENT : The passed event was not handled by the popup event handler,
                             process the event.

        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
DS_POPUP_PDEH_RET_ENUM_T ds_pass_event_to_popup 
(   
    DS_POPUPWIN_T           *pw,
    OPUS_EVENT_ENUM_TYPE    event, 
    void                    *EvData 
)
{
    DS_POPUP_PDEH_RET_ENUM_T    p_ret_state = POPUP_UNPROC_EVENT;
    KEYEVENT_STRUCT             key;

    if (pw != OP_NULL)
    {
        if (event == OPUS_FOCUS_KEYPAD)
        {
            op_memcpy(&key, EvData, sizeof(KEYEVENT_STRUCT));

            if (key.state != UHKEY_RELEASE) 
            {
                p_ret_state = pass_popup_event(pw, key.code);
            }       
        }
        else if  ( (event == OPUS_TIMER_KEYPRESS_REPEAT) && (pw->type == POPUP_NUMEDITOR))
        {   /* only POPUP_NUMEDITOR process long press key */
            p_ret_state = pass_popup_event(pw, DS_POPUP_KEYPRESS_REPEAT_EVENT);
        } 
        else if (event == OPUS_TIMER_POPUP_MSG)
        {
            p_ret_state = pass_popup_event(pw, (OP_UINT16)MSG_TIMER_POPUP_MSG);
        }
        else if (event == OPUS_FOCUS_CUST_APP_PAUSE)
        {
            p_ret_state = pass_popup_event(pw, (OP_UINT16)MSG_FOCUS_CUST_APP_PAUSE);
        }
        else if (event == OPUS_FOCUS_CUST_PAINT)
        {
             ds_refresh();
        }
        else if (event == OPUS_FOCUS_OPERATION)
        {
            p_ret_state = ds_popup_operation_event_handler(pw,EvData);
        }
    }
    return p_ret_state;
}


/*================================================================================================*/

#ifdef __cplusplus
}
#endif






















#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    MODULE NAME : ds_lcd.c

    GENERAL DESCRIPTION
        This file contains funcation required to refresh to the LCD.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial creation
    02/12/2003   Steven Lai        PP00069    Update the types of some parameter
    02/24/2002   wangwei          PP00117     Change draw bitmap to draw icon when play animation.
                                              - add animal_screen[][]
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    04/08/2003   lindawang        P000049     adjust the sublcd orientation.
                                              - remove warnings.
    05/15/2003   linda wang       P000289      Set transparent effect in idle screen softkey bar.
    05/23/2003   linda wang       P000403     Fix the UI bug.
                                              -move window system pallet into rm_bmp_scn_etc.rmd file.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    06/25/2003   lindawang        P000687     fix txt buffer has one bottom line problem.
    07/02/2003   lindawang        P000763     add error handler for over lcd_screen.
    07/26/2003   lindawang        P001074     remove animal_buffer.
    08/14/2003   lindawang        P001241     Fix some ds and gif decode problems.
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/30/2004   Steven Lai        P002795     Fix the bug that the softkey area will disappear if the idle image is animation
    04/19/2004   lindawang        P003037     Remove the popup scramble display.    
                                              - add extend softkey area function.
    04/29/2004   lindawang        P005196     Fix bug for draw softkey.    
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    06/04/2004   lindawang        P005612     Modify the refresh order.    
  
This file contains funcation required to refresh to the LCD.
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
/* for test on conexant simulator */
//#define CONEXANT_LCD_DRIVER  1

#ifdef WIN32
#include    "windows.h"
#include    "portab_new.h"
#include    "lcd_api.h"
#else
#include    "portab.h"
#endif

#include    "rm_include.h"
#include    "SP_sysutils.h"

#include    "ds_lcd_driver.h"
#include    "ds_lcd.h"
#include    "ds_util.h"
#include    "ds_sublcd.h"

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
/* scrambled area (A,B,C and D)  masks */
#define RECTMASK_A    0x01
#define RECTMASK_B    0x02
#define RECTMASK_C    0x04
#define RECTMASK_D    0x08

#define POPUP_ROUND_WIDTH     6
#define POPUP_ROUND_HEIGHT    5

/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/

SCRN_BUFFER_T scrn_buffer;

DS_COLOR    soft_screen  [LCD_EXT_SKEY_HIGH][LCD_MAX_X];        /* SOFTKEY FRAME. Currently 176x24. If it is in Idle, the height is 24, else the height is 20 */

/* bitmap buffer for images and background process */
DS_COLOR    bitmap_buffer[LCD_MAX_Y][LCD_MAX_X];                   
/* animation buffer , always forword*/
/* Popup Screeen Buffer */
DS_COLOR    pop_buffer[LCD_MAX_POPUP_WIN][LCD_POPUP_MAX_Y][LCD_POPUP_MAX_X]; 
            
/* Popup Screeen Buffer */
DS_COLOR    open_window_buffer[LCD_OPENWINDOW_MAX_Y][LCD_OPENWINDOW_MAX_X]; 

/* LCD Mirror buffer */
DS_COLOR    lcd_screen[LCD_MAX_Y][LCD_MAX_X];                        

/* point the starting point of the txt_buffer - depends upon using annuciator area or not  */
DS_COLOR    (*annun_screen)[LCD_MAX_X]    = ( DS_COLOR(*)[LCD_MAX_X])scrn_buffer.annunTextBuffer.annun_screen ; 
DS_COLOR    (*txt_screen)[LCD_MAX_X]       = ( DS_COLOR(*)[LCD_MAX_X])scrn_buffer.annunTextBuffer.txt_screen ; 
DS_COLOR    (*bitmap_screen)[LCD_MAX_X]    = ( DS_COLOR(*)[LCD_MAX_X])bitmap_buffer; 
DS_COLOR    (*open_window_screen)[LCD_OPENWINDOW_MAX_X]    = ( DS_COLOR(*)[LCD_OPENWINDOW_MAX_X])open_window_buffer; 
DS_COLOR    (*pop_screen)[LCD_POPUP_MAX_X] = ( DS_COLOR(*)[LCD_POPUP_MAX_X])pop_buffer[0] ; 



/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
#define SOFTKEY_TRANS_COLOR      0xF7FF    /* only for pacific softbar in idle. */
#define DEST_COLOR    COLOR_SMOKY_GRAY     
#define LCD_CONTRAST_CENTER      0x0004
#define LCD_CONTRAST_INDEX_NUM       8
#define LCD_CONTRAST_INDEX_MAX       LCD_CONTRAST_INDEX_NUM
static OP_UINT8 lcd_contrast_current_index = LCD_CONTRAST_INDEX_NUM/2;
const OP_UINT8 LCD_CONTRAST_VALUES[LCD_CONTRAST_INDEX_NUM] = 
{
    LCD_CONTRAST_CENTER+3,
    LCD_CONTRAST_CENTER+3,
    LCD_CONTRAST_CENTER+2,
    LCD_CONTRAST_CENTER+1,
    LCD_CONTRAST_CENTER,
    LCD_CONTRAST_CENTER-1,
    LCD_CONTRAST_CENTER-2,
    LCD_CONTRAST_CENTER-3,
};

/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
#ifdef WIN32
UINT8 byte_buf[36608];

const LCD_BITMAP dstest_buffer = 
{
    (UINT8*)byte_buf,
    LCD_MAX_Y,
    LCD_MAX_X,
    16,
};
#endif
/*==================================================================================================
    EXTERNAL FUNCTIONS
==================================================================================================*/
extern OP_BOOLEAN ds_get_extend_softkey(void);
/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
#ifdef WIN32

void cnv_screen_to_simtype()
{

    OP_INT32 i,j,k;

    k = 0;

    for (i=0;i<LCD_MAX_Y;i++)
    {
        for (j=0;j<LCD_MAX_X;j++)
        {
            byte_buf[k++] = (UINT8)(lcd_screen[i][j] & 0x00ff) ;
            byte_buf[k++] = (UINT8)((lcd_screen[i][j] & 0xff00) >> 8);
        }
    }
}

void refresh_to_sim(OP_BOOLEAN is_ann_displayed)
{

    LCD_POINT   point;
    LCD_BOX     box;
    OP_UINT8        yoffset;


    if (is_ann_displayed)
        yoffset = LCD_ANNUN_HIGH;
    else
        yoffset = 0;
    
    box.area.height = LCD_MAX_Y;
    box.area.width  = LCD_MAX_X;
    box.origin.x    = 0;
    box.origin.y    = 0;
    point.x             = 0;
    point.y             = 0;


    cnv_screen_to_simtype();

    lcd_PutBitmapExt(dstest_buffer,box,point,TRUE);
}
#endif

static DS_COLOR ChangeTransParent(DS_COLOR val)
{
    
    DS_COLOR  red, green, blue, R1,B1,G1, R2,B2,G2;
    
    R2 = DEST_COLOR & 0xF800;
    G2 = DEST_COLOR & 0x07E0;
    B2 = DEST_COLOR & 0x001F;
    
    R1 = val & 0xF800;
    G1 = val & 0x07E0;
    B1 = val & 0x001F;

    red = (R1 + R2)>>1 & 0xF800;
    green = (G1 + G2)>>1 & 0x07E0;
    blue = (B1 + B2)>>1 & 0x001F;

    return (red |green |blue);
}

/*==================================================================================================
    FUNCTION: get_overlapped_rectangles

    DESCRIPTION:
        Get overlapped retangles between two given rectangles. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Overlapped rectangles are as below.

       _________________________
      |           A             |
      |~~~~~~ ___________ ~~~~~~|
      |      |           |      |
      |   C  |   popup   |  D   |   
      |      |  window   |      |
      |      |           |      |
      |~~~~~~|___________|~~~~~~|
      |                         |
      |           B             | 
      |_________________________|

==================================================================================================*/
static OP_UINT8 get_overlapped_rectangles 
(
    LCD_RECT_T    *u_rect, /* background rectangle */
    LCD_RECT_T    *pu_rect,  /* popup rectangle */
    LCD_RECT_T    *a_rect, 
    LCD_RECT_T    *b_rect, 
    LCD_RECT_T    *c_rect, 
    LCD_RECT_T    *d_rect
)
{
    OP_BOOLEAN is_overlapped = OP_TRUE;
    OP_UINT8    scramble_area_f = 0x00;
    OP_BOOLEAN is_a_null_rect = OP_FALSE;
    OP_BOOLEAN is_b_null_rect  = OP_FALSE;
    LCD_RECT_T   bg_rect;
    LCD_RECT_T   p_rect;
   
    if ( ( u_rect != OP_NULL) && ( pu_rect != OP_NULL))
    {    
        bg_rect = *u_rect;
        p_rect = *pu_rect;
        /* check whether two given rectangle areas are overlapped */
        if ( (bg_rect.top >= p_rect.bottom) || (bg_rect.bottom <= p_rect.top) ||
             (bg_rect.left >= p_rect.right) || (bg_rect.right <= p_rect.left) )
        {
            is_overlapped = OP_FALSE;
        }
    }
    else
    {
        is_overlapped = OP_FALSE;
    }

    /* if they are ovelapped somewhere */
    if (is_overlapped)
    {
        /* get rectangle A */
        if (bg_rect.top < p_rect.top)
        {
            a_rect->top    = bg_rect.top;
            /* due to the round-side shape of the popup window, overlapped rect should concider its shape. */
            a_rect->bottom = (OP_INT16)(p_rect.top + POPUP_ROUND_HEIGHT-1);
            a_rect->left   = bg_rect.left;
            a_rect->right  = bg_rect.right;
            is_a_null_rect  = OP_TRUE;
            scramble_area_f |= RECTMASK_A;
        }
        /* get rectangle B */
        if (bg_rect.bottom > p_rect.bottom)
        {
            /* due to the unrectangle shape of the popup window, the unrectangle area should be considered */
            b_rect->top    = (OP_INT16)(p_rect.bottom -POPUP_ROUND_HEIGHT+1);
            b_rect->bottom = bg_rect.bottom;
            b_rect->left   = bg_rect.left;
            b_rect->right  = bg_rect.right;
            is_b_null_rect  = OP_TRUE;
            scramble_area_f |= RECTMASK_B;
        }
        /* get rectangle C */
        if (bg_rect.left < p_rect.left)
        {
            c_rect->left   = bg_rect.left;
            /* due to the unrectangle shape of the popup window, the unrectangle area should be considered */
            c_rect->right  = (OP_INT16)(p_rect.left+POPUP_ROUND_WIDTH-1);
            c_rect->top    = (is_a_null_rect) ? p_rect.top : (bg_rect.top+1);
            c_rect->bottom = (is_b_null_rect) ? p_rect.bottom : bg_rect.bottom;
            scramble_area_f |= RECTMASK_C;
        }
        /* get rectangle D */
        if (bg_rect.right > p_rect.right)
        {
            /* due to the unrectangle shape of the popup window, the unrectangle area should be considered */
            d_rect->left   = (OP_INT16)(p_rect.right-POPUP_ROUND_WIDTH+1);
            d_rect->right  = bg_rect.right;
            d_rect->top    = (is_a_null_rect) ? p_rect.top : (bg_rect.top+1);
            d_rect->bottom = (is_b_null_rect) ? p_rect.bottom : bg_rect.bottom;
            scramble_area_f |= RECTMASK_D;
        }
    }

    return scramble_area_f;
}

/*==================================================================================================
    FUNCTION: scramble_background

    DESCRIPTION:
        Scramble the assigned area with the given scramble color. 

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
static void scramble_background
(
    LCD_RECT_T    rect,
    DS_COLOR     s_color,   /* scramble_color */
    OP_BOOLEAN    is_it_upper_region, /* is it this region is upper of the popup */
    OP_UINT8      text_frame_start    
)
{
    OP_INT16 register    y ;
    OP_INT16 register    x ;
    OP_INT16 register    y_offset; /* y offset in lcd_screen */

    /* if it is 0, scramble color sets to even colume, 
       if it is 1, scramble color sets to add colume. */
    OP_INT16 register    col_off; /* colume offst */

    /* scramble auuciator area too */
    if ( (text_frame_start <= LCD_TEXT_FRAME_START) && (is_it_upper_region))
    {
         rect.bottom += LCD_ANNUN_HIGH;
         text_frame_start = 0;
    }

    for (y = rect.top; y <=rect.bottom; y++)
    {
       /* if even row, scramble color sets to even colume,
          if odd row, scramble color sets to odd colume */

       col_off = (y%2 ==0) ? 0 : 1; 
       
       /* add txt frame start y point since it it variable according to 
          whether annuciator is displayd or not */
       y_offset = text_frame_start+y;
       if(y_offset > LCD_MAX_Y-1)
       {  
           break;                /* Error handle*/ 
       }
       for (x = rect.left; x <= rect.right; x++)
       {
           if ( ( x + col_off) %2 == 0)
           {
               lcd_screen[y_offset][x] = s_color;
           }           
       }
    }
    lcd_drv_draw_rectangle ((OP_UINT8)rect.left, 
                            (OP_UINT8)(text_frame_start + rect.top), 
                            (OP_UINT8)rect.right, 
                            (OP_UINT8)(text_frame_start + rect.bottom),  
                            lcd_screen,
                            LCD_MAX_X,
                            (OP_UINT8)(text_frame_start + rect.top),
                            (OP_UINT8)rect.left); 
}

/*==================================================================================================
    GLOBAL FUNCTIONS
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
void lcd_init
(
    void
)
{
    OP_UINT8 i;

    wmemset((OP_UINT16 *)annun_screen, COLOR_WHITE, LCD_ANNUN_HIGH * LCD_MAX_X);
    /*wmemset((OP_UINT16 *)soft_screen, COLOR_WHITE, LCD_SKEY_HIGH * LCD_MAX_X); */
    wmemset((OP_UINT16 *)soft_screen, COLOR_WHITE, LCD_EXT_SKEY_HIGH * LCD_MAX_X);
    wmemset((OP_UINT16 *)txt_screen, COLOR_WHITE, LCD_TEXT_HIGH * LCD_MAX_X);
    wmemset((OP_UINT16 *)bitmap_screen, COLOR_WHITE, LCD_MAX_Y * LCD_MAX_X);
    
    for (i = 0; i< LCD_MAX_POPUP_WIN; i++)
    {
        wmemset((OP_UINT16 *)&pop_buffer[i][0][0], COLOR_WHITE, LCD_POPUP_MAX_Y * LCD_POPUP_MAX_X);
    }
}


/*==================================================================================================
    FUNCTION: lcd_refresh

    DESCRIPTION:
        Refresh the screens to the lcd. 

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

/**************************************************************************************************
  The below boxes are updated regions.
  Outer box is for normal screen.
  Inner box is for popup window.
       _________________________
      |           A             |
      |~~~~~~ ___________ ~~~~~~|
      |      |           |      |
      |   C  |   popup   |  D   |   
      |      |  window   |      |
      |      |           |      |
      |~~~~~~|___________|~~~~~~|
      |                         |
      |           B             | 
      |_________________________|

  Area A, B, C, and D are not overlapped with the popup window, so refresh these areas by muxing
  txt_screen and bitmap_screen accroding to the update_area_f.
  And then refresh popup window area.
**************************************************************************************************/
         
void lcd_refresh
(
    OP_UINT8         update_area_f,
    OP_BOOLEAN       is_full_screen_image, /* if it is true, bitmap buffer is not supported relative coor */
    OP_BOOLEAN       is_ann_displayed,
    LCD_RECT_T       *u_rect,       /* updated rectangle */
    LCD_RECT_T       *pu_rect,      /* popup window updated rectangle */ 
    DS_COLOR        scramble_color /* color used to scramble background in case popup window is updated */ 
)
{
    OP_BOOLEAN         is_new_text          = OP_FALSE;
    OP_BOOLEAN         is_new_bitmap     = OP_FALSE;
    OP_BOOLEAN         refresh_together  = OP_FALSE;

    OP_INT16 register  y ;
    OP_INT16 register  x ;
    OP_UINT16 register data;
    OP_INT16           y_offset = 0;
    OP_INT16           cx_bytes ; /* number of x bytes to be refreshed */
    OP_UINT8           text_frame_start = 0; /* where the text frame starts, 0 0r after annuciator area */
    OP_UINT8           skey_area_height = LCD_EXT_SKEY_HIGH;  /* where the text frame starts, LCD_EXT_SKEY_HIGH 0r LCD_SKEY_HIGH (in idle) */

    /* overlapped areas between popup area and its backgound area. See the above comment */
    LCD_RECT_T       bg_rect = {LCD_MAX_X_COOR, LCD_MAX_Y_COOR, 0, 0};     
    LCD_RECT_T       p_rect = {LCD_MAX_X_COOR, LCD_MAX_Y_COOR, 0, 0}; 
    LCD_RECT_T       refresh_rect = {LCD_MAX_X_COOR, LCD_MAX_Y_COOR, 0, 0}; 


    if (u_rect != OP_NULL)
    {
        bg_rect = * u_rect;
    }

    if (pu_rect != OP_NULL)
    {
        p_rect = *pu_rect;
    }

    text_frame_start = (is_ann_displayed) ? LCD_TEXT_FRAME_START : 0;       
   
    /* ANNUCIATOR area update */
    if (update_area_f & AREAMASK_ANNUN) 
    {
        /* in this case, annunciator area should be muxed with the full screen image */
        if (is_full_screen_image)
        {
            for (y=0; y<LCD_ANNUN_HIGH; y++) 
            {
                for (x=0; x<LCD_MAX_X; x++) 
                {
                    data = annun_screen[y][x];
                    if (data == COLOR_ANNBG)
                    {
                        lcd_screen[y][x] = bitmap_buffer[y][x];
                    }
                    else
                    {
                        lcd_screen[y][x] = data;
                    }
                }
            }
        }  
        else 
        {
            cx_bytes = LCD_MAX_X *2;   /* 2 means the color of each pixel is 2 bytes */
            for (y=0; y<LCD_ANNUN_HIGH; y++) 
            {
                op_memcpy(&lcd_screen[y][0], &annun_screen[y][0], cx_bytes); 
            }
        }  
    }
   
    /* SOFTKEY Area Update */
    if (update_area_f & AREAMASK_SKEY) 
    {
        /* in this case, softkey area should be muxed with the full screen image */
        if (is_full_screen_image)
        {
            if (ds_get_extend_softkey() == OP_TRUE) /* If it is screen_idle, the height of the softkey area is LCD_EXT_SKEY_HIGH*/
            {
                skey_area_height = LCD_EXT_SKEY_HIGH;
            }
            else
            {
                skey_area_height = LCD_SKEY_HIGH;
            }
            
            for (y=0; y<skey_area_height; y++) 
            {
                y_offset = LCD_MAX_Y -skey_area_height + y;
                for (x=0; x<LCD_MAX_X; x++) 
                {
                    data = soft_screen[y][x];
                    if (data == COLOR_TRANS)
                    {
                        lcd_screen[y_offset][x] = bitmap_buffer[y_offset][x];
                    }
                    else
                    {
                        lcd_screen[y_offset][x] = data;
                    }
                }
            }
            if (bg_rect.bottom >= LCD_MAX_Y -skey_area_height )
            {
                /* set this ture, unless, annuciator area, main content area, and softkey area are
                   refreshed separately and this is noticed to the user */
                refresh_together = OP_TRUE;
                /* adjust amount of the softkey and annuciator areas from the bg_rect since these
                   area are already moved to the lcd_screen buffer.  */
                bg_rect.bottom -= (LCD_ANNUN_HIGH + skey_area_height);
            }
        }  
        else
        {
            cx_bytes = LCD_MAX_X *2;
            for (y=0; y<LCD_SKEY_HIGH; y++) 
            {
                op_memcpy(&lcd_screen[LCD_SKEY_FRAME_START+y][0], &soft_screen[y][0], cx_bytes); 
            }
            if (bg_rect.bottom >= LCD_SKEY_FRAME_START )
            {
                bg_rect.bottom = LCD_SKEY_FRAME_START-1;
            }
        }       
    }  

    is_new_text = (update_area_f & AREAMASK_TEXT) ? OP_TRUE: OP_FALSE;
    is_new_bitmap = (update_area_f & AREAMASK_BITMAP) ? OP_TRUE: OP_FALSE;
    
    /* update rectangle adjust to the boundary */
    if  ((is_new_text == OP_TRUE) && (is_new_bitmap == OP_FALSE) && (u_rect != OP_NULL))
    {
        if (is_ann_displayed)
        {
            bg_rect.bottom = MIN(bg_rect.bottom, LCD_TEXT_HIGH-1);
        }
    }  

    if ((update_area_f & AREAMASK_POPUP) && (pu_rect != OP_NULL))
    {
        if (is_ann_displayed)
        {
            p_rect.bottom = MIN(p_rect.bottom, LCD_POPUP_MAX_Y-LCD_ANNUN_HIGH-1);
        }
        else
        {
            p_rect.bottom = MIN(p_rect.bottom, LCD_POPUP_MAX_Y-1);
        }
    }
  

    /* Due to procesing time in case of non-popup case - separate POPUP case and Non-popup case */

    /* POPUP AREA - can be on Main (Text on Bitmap), Text and Bitmap screen. */
    if  ( (update_area_f & AREAMASK_POPUP) && (pu_rect != OP_NULL) )
    {
        /* now update popup window */
        for ( y = p_rect.top; y <= p_rect.bottom; y++)
        {
            y_offset = text_frame_start + y;
            if(y_offset > LCD_MAX_Y-1) /* once y_offset is over screen, we can avoid the error */
            {   
                break;
            }
            for ( x = p_rect.left; x <=p_rect.right; x++)
            {
                data = pop_screen[y][x];
                if (data != COLOR_TRANS)
                {
                    lcd_screen[y_offset][x] = data;
                }
            }
        }         
        lcd_drv_draw_rectangle((OP_UINT8)p_rect.left, 
                                             (OP_UINT8)(text_frame_start + p_rect.top), 
                                             (OP_UINT8)p_rect.right, 
                                             (OP_UINT8)(text_frame_start + p_rect.bottom),  
                                             lcd_screen,
                                             LCD_MAX_X,
                                             (OP_UINT8)(text_frame_start + p_rect.top),
                                             (OP_UINT8)p_rect.left); 
        if (update_area_f & AREAMASK_SKEY) 
        {
            lcd_drv_draw_rectangle( 0, 
                                    LCD_SKEY_FRAME_START, 
                                    LCD_MAX_X-1, 
                                    LCD_SKEY_FRAME_START+LCD_SKEY_HIGH-1, 
                                    lcd_screen,
                                    LCD_MAX_X,
                                    LCD_SKEY_FRAME_START,
                                    0); 
       } 
    }
    else
    {
        if ( (is_new_text || is_new_bitmap) && (u_rect != OP_NULL)) /* no popup update */
        {
            refresh_rect.left = bg_rect.left;
            refresh_rect.right = bg_rect.right;
            if (is_new_text && is_new_bitmap) /* Text on bitmap */
            {
                for (y=bg_rect.top; y<=bg_rect.bottom; y++) 
                {
                    y_offset = text_frame_start+y;
                    if(y_offset > LCD_MAX_Y-1)
                    {   
                       /* Error handle, if set screen mode without annun from with annun,
                          but didn't refresh between them, the update area will be wrong.
                          such as: bg_rect.bottom=143, text_frame_start=20 (ems has no this problem)
                          y_offset will be over LCD_MAX_Y */
                        break;
                    }

                    for (x=bg_rect.left; x<=bg_rect.right; x++) 
                    {
                        data = txt_screen[y][x];
                        if ( (data !=COLOR_TRANS) && ( y_offset < LCD_EXT_TEXT_HIGH))
                        {
                            lcd_screen[y_offset][x] = data;
                        }
                        else 
                        {
                            if (is_full_screen_image)
                            {
                                  /* in case of bitmap buffer, no relative coordinate is not supported. 
                                     Only absolute y coordinate supported */
                                  lcd_screen[y_offset][x] = bitmap_screen[y_offset][x];
                             }
                             else
                             {
                     /*this bitmap screen (0,0) is same to text screen, 
                       which is mainly for user drawing text and bitmap at the same coordination*/
                                 lcd_screen[y_offset][x] = bitmap_screen[y][x];  
                             }
                        }
                    }
                }
                /* update refresh rect */
                refresh_rect.top = MIN((bg_rect.top + text_frame_start), LCD_MAX_Y_COOR);
                refresh_rect.bottom = MIN((bg_rect.bottom + text_frame_start), LCD_MAX_Y_COOR);
            }
            else if (is_new_text) /* Text only - no background image is refreshed */
            {
                if ( bg_rect.bottom > LCD_EXT_TEXT_HIGH)
                {
                   bg_rect.bottom = LCD_EXT_TEXT_HIGH -1;
                }

                for (y=bg_rect.top; y<=bg_rect.bottom; y++) 
                {
                    y_offset = text_frame_start+y; 
                    if(y_offset > LCD_MAX_Y-1)
                    {   
                        break;         /* error handler */
                    }
                    op_memcpy(&lcd_screen[y_offset][bg_rect.left], &txt_screen[y][bg_rect.left],
                           (bg_rect.right - bg_rect.left + 1) * 2);
                }
                /* update refresh rect */
                refresh_rect.top = MIN((bg_rect.top + text_frame_start), LCD_MAX_Y_COOR);
                refresh_rect.bottom = MIN((bg_rect.bottom + text_frame_start), LCD_MAX_Y_COOR);
            }
            else if (is_new_bitmap) /* Image only - no text is refreshed */ 
            {
                if (is_full_screen_image)
                {
                    /* in case of bitmap buffer, no relative coordinate is not supported. 
                       Only absolute y coordinate supported */
                    for (y=bg_rect.top; y<=bg_rect.bottom; y++) 
                    {
                        op_memcpy(&lcd_screen[y][bg_rect.left], &bitmap_screen[y][bg_rect.left],
                               (bg_rect.right - bg_rect.left + 1) * 2);
                    }
                    /* update refresh rect */
                    refresh_rect.top = bg_rect.top;
                    refresh_rect.bottom = bg_rect.bottom;
                }
                else
                {
                    for (y=bg_rect.top; y<=bg_rect.bottom; y++) 
                    {
                        y_offset = text_frame_start+y;
                        if(y_offset > LCD_MAX_Y-1)
                        {   
                            break;         /* error handler */
                        }
                        op_memcpy(&lcd_screen[y_offset][bg_rect.left], &bitmap_screen[y][bg_rect.left],
                        (bg_rect.right - bg_rect.left + 1) * 2);
                    }
                    /* update refresh rect */
                    refresh_rect.top = MIN((bg_rect.top + text_frame_start), LCD_MAX_Y_COOR);
                    refresh_rect.bottom = MIN((bg_rect.bottom + text_frame_start), LCD_MAX_Y_COOR);
                }

            }
        }
        
        if (update_area_f & AREAMASK_ANNUN) 
        {
            lcd_drv_draw_rectangle (0, 
                                     0, 
                                     LCD_MAX_X-1, 
                                     LCD_ANNUN_HIGH-1, 
                                     lcd_screen,
                                     LCD_MAX_X,
                                     0,
                                     0); 
        }
        if ( (is_new_text || is_new_bitmap) && (u_rect != OP_NULL))
        {
			lcd_drv_draw_rectangle((OP_UINT8)refresh_rect.left, 
                                   (OP_UINT8)refresh_rect.top, 
                                   (OP_UINT8)refresh_rect.right, 
                                   (OP_UINT8)refresh_rect.bottom,  
                                   lcd_screen,
                                   LCD_MAX_X,
                                   0,
                                   0); 
        }
        if (update_area_f & AREAMASK_SKEY) 
        {
            lcd_drv_draw_rectangle (0, 
                                     LCD_SKEY_FRAME_START, 
                                     LCD_MAX_X-1, 
                                     LCD_SKEY_FRAME_START+LCD_SKEY_HIGH-1, 
                                     lcd_screen,
                                     LCD_MAX_X,
                                     LCD_SKEY_FRAME_START,
                                     0); 

        }
    }
}

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
OP_BOOLEAN lcd_contrast_set(OP_UINT8 index)
{
    OP_BOOLEAN          rc = OP_FALSE;

    if(index>0 && index <= LCD_CONTRAST_INDEX_MAX)
    {
        lcd_contrast_current_index = index;
        lcd_drv_volume_set(LCD_CONTRAST_VALUES[lcd_contrast_current_index-1]);
        rc = OP_TRUE;
    }
    return rc;
    }

OP_BOOLEAN lcd_contrast_get(OP_UINT8 *pIndex)
{
    *pIndex = lcd_contrast_current_index;
    return OP_TRUE;
}

OP_BOOLEAN lcd_contrast_inc(void)
{
    OP_BOOLEAN          rc = OP_FALSE;
    
    if(lcd_contrast_current_index<LCD_CONTRAST_INDEX_MAX)
    {
        lcd_contrast_current_index++;
        lcd_drv_volume_set(LCD_CONTRAST_VALUES[lcd_contrast_current_index]);
        rc = OP_TRUE;
    }
    return rc;
}

OP_BOOLEAN lcd_contrast_dec(void)
{
    OP_BOOLEAN          rc = OP_FALSE;

    if(lcd_contrast_current_index>0)
    {
        lcd_contrast_current_index--;
        lcd_drv_volume_set(LCD_CONTRAST_VALUES[lcd_contrast_current_index]);
        rc = OP_TRUE;
    }
    return rc;
}

/*==================================================================================================
    FUNCTION: lcd_screen_init

    DESCRIPTION:
        Initialize screen buffers. 

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES: fill out the lcd screen as black color as ID request.
==================================================================================================*/
void lcd_screen_init
(
    void
)
{
    wmemset((OP_UINT16 *)lcd_screen, COLOR_BLACK, LCD_MAX_Y * LCD_MAX_X);

    lcd_drv_draw_rectangle ((OP_UINT8)0,
                            (OP_UINT8)0,
                            (OP_UINT8)LCD_MAX_X_COOR,
                            (OP_UINT8)LCD_MAX_Y_COOR,
                            lcd_screen,
                            LCD_MAX_X,
                            (OP_UINT8)0,
                            (OP_UINT8)0
                           ); 

}
/*==================================================================================================
    FUNCTION: lcd_openwindow_refresh

    DESCRIPTION:
        refresh openwindow buffer into lcd buffer and display. 

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int lcd_openwindow_refresh
(
    LCD_RECT_T u_rect   /* updated rectangle */
)
{
    OP_UINT16  i;
    OP_UINT16  j;
    OP_UINT16 base_x;
    OP_UINT16 base_y; 
    //OP_UINT16 x_offset = 0;
    
    /* clear lcd_screen buffer */
    base_x = LCD_OPENWINDOW_START_X;
    base_y = LCD_OPENWINDOW_START_Y;

    /* copy open window buffer to lcd_screen buffer */
    for (i = base_y; i < LCD_OPENWINDOW_HIGH; i++)
    {
        //x_offset = base_y + i;
        for(j = base_x; j < base_x + LCD_OPENWINDOW_WIDTH; j++)
        {
            lcd_screen[i][j] = open_window_screen[i][j];
        }
    }
    /* now still refresh the all open window buffer */
    
    /* refresh to lcd */
    if((SP_status_get_openwindow_display())&&
        (!SP_is_in_deep_sleep())
      )
    {
        lcd_drv_draw_rectangle ((OP_UINT8)u_rect.left,
                                (OP_UINT8)u_rect.top,
                                (OP_UINT8)u_rect.right,
                                (OP_UINT8)u_rect.bottom,
                                lcd_screen,
                                LCD_MAX_X,
                                (OP_UINT8)0,
                                (OP_UINT8)0
                               ); 
        return 1;     
    }
    else
    {        
        return -1;
    }

}

/*================================================================================================*/

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================

    MODULE NAME : ds_sublcd.c

    GENERAL DESCRIPTION
        This file contains funcation required to refresh to the LCD.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    09/23/2002   Paul Seo         crxxxxx     Initial Creation
    10/10/2002   Ashley Lee       crxxxxx     Add ds_sublcd_contrast
    02/10/2003   Steven Lai       PP00069     Update the types of some parameters and removed some useless statements

    02/27/2003   Chenyu           PP00150     -Decompress font data 
    03-03-31     Chenyu           P000023     - Add  symbol font library  of chinese 16 &12pixel 
    04/10/2003   Chenyu           P000072     -Sublcd font display wrong 
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    04/08/2003   lindawang        P000049     adjust the sublcd orientation.
                                              - remove warnings.
    03-04-04     Chenyu           P000041     - Fix sublcd display 
    03-05-07     Chenyu           P0000214    - Change the position of SUBLCD of pacific
    03-05-14     Chenyu           P000257     - Replace the new open image of pacific sublcd &cleanup the code by config
    03-05-27     Chenyu           P000436     Fix sublcd & game display

    03-05-29     Chenyu           P000466     Display  the picture of  game  cargoder animation   & fix sublcd   display
    03-05-31     Chenyu           P000489     - Add mute and melody icon in sublcd and idle display
    06/09/2003   Chenyu           P000548     -Separate sublcd display from idle
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    06/21/2003   Wang Peng        P000653     Fix bug: display wrong in sublcd when charging   
    06/26/2003   Chenyu           P000708     sublcd  display when powerup 
    07/01/2003   Chenyu           P000754     - Update sublcd  display because of changing size of sublcd 
    07/03/2003   Wang Peng        P000772     Adjust the text to align center when powerdown charging
    03-07-09     Chenyu           P000856     - Update sublcd display of pacific 
    03-07-25     Chenyu           P001015     - Update sublcd display of time
                                              - Update call animation
    08/04/2003   Chenyu           P001129     Add MMS icon
    08/29/2003   Chenyu           P001425     SUBLCD support display 20 char
    11/10/2003   Sunshuwei        P002011     Fixed a sublcd display bug:ascii-displaying not in the middle 
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/11/2004   xcy              c002631     Add Xdict phone processing.
    04/13/2004   lindawang        p002976     remove compressed fontdata, clear openwindow text region before write data.
    04/27/2004   linda wang       c005088     remove sublcd image for decrease resource.
    05/19/2004   chouwangyun      c005496     modify DS module on new odin base 
    06/04/2004   lindawang        P005612     remove some unused functions and modify open window display method.    
    07/12/2004   liren            p006927     add openwindow animation
    06/08/2004   liren            P005997     adjust text displaying area
    07/19/2004   liren            p007060     adjust openwindow animation
    07/24/2004   liren            p007234     change powerdown charging display 
    07/28/2004   Tree Zhan        p007383     do not update screen when deep sleep 
    08/04/2004   liren          p007506       change openwindow display when call text displayed

This file contains funcation required to refresh to the LCD.
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include    "APP_include.h"

#include    "rm_include.h"
#include    "SP_sysutils.h"
#include    "SP_status.h"

#include    "ds_lcd.h"
#include    "ds_util.h"
#include    "ds_sublcd.h"
#include    "ds_primitive.h"
#include    "ds_font.h"

#include    "SUBLCD_DISPLAY_main.h"
#include    "chinese_font_decompress.h"

#ifdef WIN32
#include "lcd_api.h"
#endif
/************************ Global Definitions  ****************/
#define      SUBLCD_START_COL            0

#define      SUBLCD_ANNUN_OFFSET         4
#define      SUBLCD_TIME_DATE_OFFSET     3
#define      SUBLCD_REMOVE_DATE_Y_BOTTOM    170
#define      SUBLCD_REMOVE_DATE_Y_TOP           130
#define      SUBLCD_TIME_LEFT_X_BELOW10        5
#define      SUBLCD_TIME_MARGIN_X                    1
#define      SUBLCD_DATE_X_POSITION      52
#define      SUBLCD_DATE_ICON_WIDTH      8
#define      SUBLCD_DATE_ICON_WIDTH_E      18
/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#ifdef WIN32
static OP_UINT8 sublcd_rev_bits
(
    OP_UINT8 value
);

static void sublcd_cnv_screen_to_simulator_type
( 
    OP_BOOLEAN is_txt 
);

static void sublcd_refresh_for_simulator
(
    OP_BOOLEAN is_txt
);
#endif

static void ds_sublcd_putpixel
( 
    OP_UINT8 x, 
    OP_UINT8 y, 
    OP_UINT8 data 
);

static void ds_sublcd_putpixel_graphic
( 
    OP_UINT8 x, 
    OP_UINT8 y, 
    OP_UINT8 data 
);


static OP_UINT8 ds_sublcd_pre_set_buffer
( 
    OP_UINT8 row, 
    OP_UINT8 start_x, 
    OP_UINT8 length 
);

static void ds_sublcd_ascii_out
( 
    OP_UINT8 row, 
    OP_UINT8 column, 
    OP_UINT8 x_offset, 
    OP_UINT8 data 
);

static void ds_sublcd_chinese_out
( 
    OP_UINT8 row, 
    OP_UINT8 column, 
    OP_UINT16 data 
);

static void ds_sublcd_line
( 
    OP_INT8 startx, 
    OP_INT8 starty, 
    OP_INT8 endx, 
    OP_INT8 endy, 
    OP_BOOLEAN b_or_w 
);

static void ds_sublcd_display_analog_clock
(
    OP_UINT8 hours,
    OP_UINT8 minutes,
    OP_UINT8 seconds
);

static  void ds_sublcd_refresh
(
    OP_BOOLEAN is_full_displayed
);


static OP_UINT8     sublcd_center_offset = 0;
/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
#define  abs(x) (((x) < 0) ? -(x) : (x))

#define SUBLCE_CENTER_OF_ANALOG_CLOCK_X 40
#define SUBLCE_CENTER_OF_ANALOG_CLOCK_Y 24


/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
//extern const OP_UINT8 ASCII_N_8_16[];
extern const OP_UINT8 GB2312_N_16_16[];
//extern const OP_UINT16 GB2312_unicode_table[];
/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
#ifdef WIN32
UINT8 sub_byte_buf[SUBLCD_MAX_PAGE][SUBLCD_X_SIZE];

const LCD_BITMAP sub_dstest_buffer = 
{
    (UINT8*)sub_byte_buf,
    S44000A01_Y_SIZE,
    S44000A01_X_SIZE,
    1,
};
#endif


/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
// LCD mirror buffer. Pls do not use this buffer for sublcd display
static OP_UINT8 sublcd_screen[SUBLCD_MAX_PAGE][SUBLCD_X_SIZE];
// For collect sublcd image data.(clock, game, clock+date)
static OP_UINT8 sublcd_graphic_screen[SUBLCD_MAX_PAGE][SUBLCD_X_SIZE];
// For collect sublcd txt data.(sms, cli etc)
static OP_UINT8 sublcd_txt_screen[SUBLCD_MAX_PAGE][SUBLCD_X_SIZE];
// For icon on top of sub lcd
static OP_UINT8 sublcd_icon_top[SUBLCD_ICON_MAX_PAGE][SUBLCD_X_SIZE];
// For icon on bottom of sub lcd
static OP_UINT8 sublcd_icon_bottom[SUBLCD_ICON_MAX_PAGE][SUBLCD_X_SIZE];

static SUBLCD_MARQUE_STRUCT_T sublcd_marque_txt[TXT_LINE_MAX-1];
static OP_UINT8 sublcd_marque_txt_buf[TXT_LINE_MAX-1][SUBLCD_MARQUE_MAX_CHAR*2];
static OP_UINT8 sublcd_marque_char[TXT_LINE_MAX-1]={0,0,0};
static OP_UINT8 sublcd_marque_column[TXT_LINE_MAX-1]={2,2,2};
static OP_BOOLEAN  sublcd_marque_first_loop[TXT_LINE_MAX-1] = {OP_TRUE,OP_TRUE,OP_TRUE};

extern OP_BOOLEAN first_time_event;

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
#ifdef WIN32
/*==================================================================================================
    FUNCTION: sublcd_rev_bits

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static OP_UINT8 sublcd_rev_bits
(
    OP_UINT8 value
)
{   
   OP_UINT8 i, j, ret_val=0;
   for ( i = 0 ; i < 8 ; i++ )
   {
        ret_val = ret_val << 1;
        j = value & 0x01;
        value = value >> 1 ;
        ret_val = ret_val | j ;
   }
  return (ret_val);
  
}

/*==================================================================================================
    FUNCTION: sublcd_cnv_screen_to_simulator_type

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static void sublcd_cnv_screen_to_simulator_type
( 
    OP_BOOLEAN is_txt
)
{
    OP_INT32 i,j,k,pos,bit;
    OP_UINT8 factor, srcdata,dstdata1,dstdata2;
    OP_UINT8 (*ptr)[SUBLCD_X_SIZE];

    if ( is_txt == OP_FALSE ) // graphic
    {
        ptr = sublcd_graphic_screen;
    }
    else
    {
        ptr = sublcd_txt_screen;
    }

        for (i=0;i<SUBLCD_MAX_PAGE;i++)
        {
            for (j=0;j<SUBLCD_X_SIZE;j++)
            {
            pos = j/8;
            bit = j%8;
            bit = 7-bit;
            factor = 1;
            srcdata = 0;
            dstdata1 = (1<<bit);
            dstdata2 = ~dstdata1;
            for(k=7; k>=0; k--)
    {
                srcdata = ptr[i][j]&factor;
                if(srcdata == 0)
        {
                    sub_byte_buf[i][pos+k*(SUBLCD_X_SIZE/8)] &= dstdata2;
                }
                else
            {
                    sub_byte_buf[i][pos+k*(SUBLCD_X_SIZE/8)] |= dstdata1;
                }
                factor <<= 1;
            }
        }
    }
}

/*==================================================================================================
    FUNCTION: sublcd_refresh_for_simulator

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static void sublcd_refresh_for_simulator
(
    OP_BOOLEAN is_txt
)
{

    LCD_POINT   point;
    LCD_BOX     box;

    sublcd_cnv_screen_to_simulator_type(is_txt);

    point.x             = 0;
    point.y             = 0;
    sublcd_PutBitmap(sub_dstest_buffer,point);


    box.area.height = SUBLCD_Y_SIZE;   
    box.area.width = SUBLCD_X_SIZE;
    box.origin.x = 0;
    box.origin.y = 0;
    sublcd_UpdateScreen( box );

}
#endif

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: sublcd_power_on

    DESCRIPTION: initial subLCD when open handset.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void sublcd_power_on
(
    void
)
{
    s44000a01_lcd_init();
}

/*==================================================================================================
    FUNCTION: sublcd_init

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void sublcd_init
(
    void
)
{
#ifdef DS_OPEN_WINDOW
    ds_openwindow_int();
#else
    op_memset((OP_UINT8 *)sublcd_screen, 0xFF, sizeof(sublcd_screen));
    op_memset((OP_UINT8 *)sublcd_graphic_screen, 0x00, sizeof(sublcd_graphic_screen));
    op_memset((OP_UINT8 *)sublcd_txt_screen, 0x00, sizeof(sublcd_txt_screen));    
    op_memset((OP_UINT8 *)sublcd_icon_top, 0x00, sizeof(sublcd_icon_top));
    op_memset((OP_UINT8 *)sublcd_icon_bottom, 0x00, sizeof(sublcd_icon_bottom));    
#endif 
}

/*==================================================================================================
    FUNCTION: ds_sublcd_screen_buffer_clear

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_screen_buffer_clear
(
    void
)
{
    ds_sublcd_graphic_screen_buffer_clear();
    ds_sublcd_txt_screen_buffer_clear();
}

/*==================================================================================================
    FUNCTION: ds_sublcd_graphic_screen_buffer_clear

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_graphic_screen_buffer_clear
(
    void
)
{
    op_memset((OP_UINT8 *)sublcd_graphic_screen, 0x00, sizeof(sublcd_graphic_screen));
}

/*==================================================================================================
    FUNCTION: ds_sublcd_txt_screen_buffer_clear

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_txt_screen_buffer_clear
(
    void
)
{
    op_memset((OP_UINT8 *)sublcd_txt_screen, 0x00, sizeof(sublcd_txt_screen));    
}

/*==================================================================================================
    FUNCTION: ds_sublcd_do_refresh

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_do_refresh
(
    void
)
{
#ifdef DS_OPEN_WINDOW
    ds_openwindow_refresh();
#else
        
    if (( SP_get_sublcd_screen_mode() == SUBLCD_DISPLAY_TEXT_MODE)
        || ((SP_manager_idle_initialized_get() == OP_TRUE)&&(first_time_event == OP_TRUE))) 
    {
        if (SP_get_sublcd_full_screen_mode())     
        {
            ds_sublcd_refresh(OP_TRUE);                
        }
        else
        {
            ds_sublcd_refresh(OP_FALSE);                    
        }
    }

#endif
}

/*==================================================================================================
    FUNCTION: ds_sublcd_refresh

    DESCRIPTION:
        Refresh the screens to the lcd. 

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static void ds_sublcd_refresh
(
    OP_BOOLEAN    is_full_displayed
)
{
#ifndef WIN32
    OP_UINT8 page;
#endif

    if ((SP_get_sublcd_screen_mode() == SUBLCD_DISPLAY_TIME_MODE)
        ||(SP_get_sublcd_screen_mode() == SUBLCD_DISPLAY_ANIMATION_MODE))
    {
        if ( SP_get_sublcd_full_screen_mode() == OP_FALSE )
        {
            op_memcpy(&sublcd_graphic_screen[0][0], &sublcd_icon_top[0][0], SUBLCD_X_SIZE);
            op_memcpy(&sublcd_graphic_screen[1][0], &sublcd_icon_top[1][0], SUBLCD_X_SIZE);
#ifndef PACIFIC_VERSION
            op_memcpy(&sublcd_graphic_screen[8][0], &sublcd_icon_bottom[0][0], SUBLCD_X_SIZE);
            op_memcpy(&sublcd_graphic_screen[9][0], &sublcd_icon_bottom[1][0], SUBLCD_X_SIZE);
#endif
        }

#ifndef WIN32
        for( page = 0; page < SUBLCD_MAX_PAGE; page++ )
        {
            if( op_memcmp(sublcd_screen[page], sublcd_graphic_screen[page], SUBLCD_X_SIZE) != 0 )
            {
                op_memcpy(sublcd_screen[page], sublcd_graphic_screen[page], SUBLCD_X_SIZE);
                s44000a01_write(page,0, SUBLCD_X_SIZE, (OP_UINT8*)&(sublcd_screen[page][0]));
            }
        }
#endif
#ifdef WIN32
        sublcd_refresh_for_simulator(OP_FALSE); // graphic
#endif

    }
    else // text display
    {
        op_memcpy(&sublcd_txt_screen[0][0], &sublcd_icon_top[0][0], SUBLCD_X_SIZE);
        op_memcpy(&sublcd_txt_screen[1][0], &sublcd_icon_top[1][0], SUBLCD_X_SIZE);
#ifndef PACIFIC_VERSION
        op_memcpy(&sublcd_txt_screen[8][0], &sublcd_icon_bottom[0][0], SUBLCD_X_SIZE);
        op_memcpy(&sublcd_txt_screen[9][0], &sublcd_icon_bottom[1][0], SUBLCD_X_SIZE);
#endif


#ifndef WIN32
        for( page = 0; page < SUBLCD_MAX_PAGE; page++ )
        {
            if( op_memcmp(sublcd_screen[page], sublcd_txt_screen[page], SUBLCD_X_SIZE) != 0 )
            {
                op_memcpy(sublcd_screen[page], sublcd_txt_screen[page], SUBLCD_X_SIZE);
                s44000a01_write(page,0, SUBLCD_X_SIZE, (OP_UINT8*)&(sublcd_screen[page][0]));
            }
        }
#endif
#ifdef WIN32
        sublcd_refresh_for_simulator(OP_TRUE); // txt
#endif

    }

}

/*==================================================================================================
    FUNCTION: ds_sublcd_image_out

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_image_out
( 
    OP_UINT8 x_size, 
    OP_UINT8 y_size, 
    OP_UINT8 x_offset, 
    OP_UINT8 y_offset, 
    const OP_UINT8 * image_ptr 
)
{
    OP_UINT8 page, x, temp;
    OP_UINT8 start_page = y_offset / BITS_EACH_BYTE;
    OP_UINT8 page_offset = y_offset % BITS_EACH_BYTE;
    OP_UINT8 img_offset;
    OP_UINT8 data;    

    temp = (y_size + y_offset)/BITS_EACH_BYTE;
    img_offset = BITS_EACH_BYTE-((temp + 1)*BITS_EACH_BYTE - (y_size+y_offset));
    
    for( page = start_page; (page <=  ( y_size + y_offset - 1 ) / BITS_EACH_BYTE ) && (page < SUBLCD_MAX_PAGE); page++ )  
    {
        for( x = x_offset; (x < ( x_size + x_offset )) && (x < SUBLCD_X_SIZE ); x++ )  
        {
            if( page == start_page )
            {
                data = ( *(image_ptr+(x-x_offset)) >> page_offset ) | ( sublcd_graphic_screen[page][x] & ( 0xff << (BITS_EACH_BYTE-page_offset) ) );
            }
            else if( page == ( (y_size+y_offset-1) / BITS_EACH_BYTE ) )
            {
                data = ( *(image_ptr+(x_size*(page-start_page-1))+(x-x_offset)) << (BITS_EACH_BYTE-page_offset) ) 
                            | ( *(image_ptr+(x_size*(page-start_page))+(x-x_offset)) >> page_offset );
                if( ( (y_size+y_offset) % BITS_EACH_BYTE ) != 0 )
                {
                    data = (data & (0xff<<(BITS_EACH_BYTE-img_offset))) | (sublcd_graphic_screen[page][x]&(0xff>>img_offset));
                }
            }
            else
            {
                data = ( *(image_ptr+(x_size*(page-start_page))+(x-x_offset)) >> page_offset ) 
                            | ( *(image_ptr+(x_size*(page-start_page-1))+(x-x_offset)) << (BITS_EACH_BYTE-page_offset) );
            }

            sublcd_graphic_screen[page][x] = data;      
        }
    }
}

/*==================================================================================================
    FUNCTION: ds_sublcd_annun_out

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_annun_out
( 
    DS_ANNUN_ENUM_T index, 
    OP_BOOLEAN     value 
)
{
#ifdef SUBLCD
     RM_RESOURCE_ID_T   res_id;
     RM_SUBLCD_IMAGE_T  *icon;
     OP_UINT8          *icon_data;

     OP_BOOLEAN is_top = OP_TRUE;
     OP_UINT8   start_point = 0;
     OP_BOOLEAN is_sublcd_icon = OP_TRUE;
    
    switch(index) {
        case ANNUN_RSSI0:
        case ANNUN_RSSI1:
        case ANNUN_RSSI2:
        case ANNUN_RSSI3:
        case ANNUN_RSSI4:
        case ANNUN_RSSI5:
        case ANNUN_RSSI6:       
            start_point = SUBLCD_ANNUN_OFFSET;
            is_top = OP_TRUE;
            break;

        case ANNUN_BATT0:
        case ANNUN_BATT1:
        case ANNUN_BATT2:
        case ANNUN_BATT3:
        case ANNUN_BATT4:
            start_point = SUBLCD_X_SIZE -SUBLCD_ANNUN_OFFSET-SUBLCD_BATTERY_WIDTH-2;
            is_top = OP_TRUE;
            break;

        case ANNUN_MSG: 
        case ANNUN_MMS: 
        case ANNUN_ROAM:
        case ANNUN_MELODY:
        case ANNUN_MUTE:
        case ANNUN_SILENCE:
        case ANNUN_NORMAL:
        case ANNUN_ALOUD:
        case ANNUN_EARPHONE:
        case ANNUN_INMOBILE:
            start_point = SUBLCD_RASSI_WIDTH +SUBLCD_ANNUN_OFFSET
                        +( SUBLCD_X_SIZE -2*SUBLCD_ANNUN_OFFSET-SUBLCD_BATTERY_WIDTH - SUBLCD_RASSI_WIDTH-SUBLCD_VIB_WIDTH)/2;
            is_top = OP_TRUE;
            break;

        default:
            is_sublcd_icon = OP_FALSE;
            break;
    }

    if ( is_sublcd_icon == OP_TRUE)
    {
        res_id = (RM_RESOURCE_ID_T)(SUBLCD_IMAGE_RSSI0 + index);
        icon = util_get_sublcd_image_from_res(res_id);
        if ( (icon != OP_NULL ) && (icon->data != OP_NULL) )
        {
            icon_data = (OP_UINT8 *)icon->data;
    
            if ( is_top == OP_TRUE )
            {
                if (value)  
                {    
                    op_memcpy(&sublcd_icon_top[0][start_point], icon_data, icon->biWidth);
                    op_memcpy(&sublcd_icon_top[1][start_point], icon_data + icon->biWidth, icon->biWidth);
                }
                else
                {    
                    op_memset(&sublcd_icon_top[0][start_point], 0x00, icon->biWidth);
                    op_memset(&sublcd_icon_top[1][start_point], 0x00, icon->biWidth);
                }
                
            }
            else
            {
                if (value)  
                {            
                    op_memcpy(&sublcd_icon_bottom[0][start_point], icon_data, icon->biWidth);
                    op_memcpy(&sublcd_icon_bottom[1][start_point], icon_data + icon->biWidth, icon->biWidth);
                }
                else
                {            
                    op_memset(&sublcd_icon_bottom[0][start_point], 0x00, icon->biWidth);
                    op_memset(&sublcd_icon_bottom[1][start_point], 0x00, icon->biWidth);
                }
    
            }
           ds_sublcd_do_refresh();

        }        
    }
#endif
}

/*===========================================================================

FUNCTION ds_sublcd_putpixel

DESCRIPTION
  one pixel in 80 x 101 Lcd Screen

  (0,0)  ------------------------ (79,0)
        |                           |
        |                           |
        |                           |        
        |            (x,y)             |
        |                           |
        |                           |
        |                           |
        |                           |
        |                           |
        |                           |
        |                           |        
  (0.100) ----------------------- (79,100)
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
static void ds_sublcd_putpixel
( 
    OP_UINT8 x, 
    OP_UINT8 y, 
    OP_UINT8 data 
)
{
  if( ( x >= SUBLCD_X_SIZE ) || ( y >= SUBLCD_Y_SIZE ) )      
      return;

  if( data )  //put BLACK
    sublcd_txt_screen[y/BITS_EACH_BYTE][x] |= ( 0x80 >> ( y % BITS_EACH_BYTE ) );
  else
    sublcd_txt_screen[y/BITS_EACH_BYTE][x] &= ~( 0x80 >> ( y % BITS_EACH_BYTE ) );
}


/*===========================================================================

FUNCTION ds_sublcd_putpixel_graphic

DESCRIPTION
  one pixel in 80 x 101 Lcd Screen

  (0,0)  ------------------------ (79,0)
        |                           |
        |                           |
        |                           |        
        |            (x,y)             |
        |                           |
        |                           |
        |                           |
        |                           |
        |                           |
        |                           |
        |                           |        
  (0.100) ----------------------- (79,100)
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
static void ds_sublcd_putpixel_graphic
( 
    OP_UINT8 x, 
    OP_UINT8 y, 
    OP_UINT8 data 
)
{
  if( ( x >= SUBLCD_X_SIZE ) || ( y >= SUBLCD_Y_SIZE ) )      
      return;

  if( data )  //put BLACK
    sublcd_graphic_screen[y/BITS_EACH_BYTE][x] |= ( 0x80 >> ( y % BITS_EACH_BYTE ) );
  else
    sublcd_graphic_screen[y/BITS_EACH_BYTE][x] &= ~( 0x80 >> ( y % BITS_EACH_BYTE ) );
}


/*==================================================================================================
    FUNCTION: ds_sublcd_pre_set_buffer

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static OP_UINT8 ds_sublcd_pre_set_buffer
(
    OP_UINT8 row, 
    OP_UINT8 start_x, 
    OP_UINT8 length 
)
{

  OP_UINT8 start_y = 0x00, j;

  switch( row )
  {
    case 0:
      for( j = 0; j < length; j++ )
      {
        sublcd_txt_screen[0][start_x+j] = 0x00;
        sublcd_txt_screen[1][start_x+j] = 0x00;
      }
      start_y = 0x00;
      break;
    case 1:
      for( j = 0; j < length; j++ )
      {
        sublcd_txt_screen[2][start_x+j] = 0x00;
        sublcd_txt_screen[3][start_x+j] = 0x00;
      }
      start_y = 0x10;
      break;
    case 2:
      for( j = 0; j < length; j++ )
      {
        sublcd_txt_screen[4][start_x+j] = 0x00;
        sublcd_txt_screen[5][start_x+j] = 0x00;
      }
      start_y = 0x20;
      break;

    case 3:
      for( j = 0; j < length; j++ )
      {
        sublcd_txt_screen[6][start_x+j] = 0x00;
        sublcd_txt_screen[7][start_x+j] = 0x00;
      }
      start_y = 0x30;
      break;
    case 4:
      for( j = 0; j < length; j++ )
      {
        sublcd_txt_screen[8][start_x+j] = 0x00;
        sublcd_txt_screen[9][start_x+j] = 0x00;
      }
      start_y = 0x40;
      break;
      
  } 
  return( start_y );


}

/*==================================================================================================
    FUNCTION: ds_sublcd_ascii_out

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static void ds_sublcd_ascii_out
( 
    OP_UINT8 row, 
    OP_UINT8 column, 
    OP_UINT8 x_offset, 
    OP_UINT8 data 
)
{
    const OP_UINT8 *HL;
    OP_UINT8 i, j, start_x, start_y;
    OP_UINT8 input = 0xff;

    HL = &ASCII_N_6_12[ data*12 ];    //Bold

    //pre-setting buffer and obtain start y value

    start_x = column *( BITS_EACH_BYTE)+sublcd_center_offset;
    start_y = ds_sublcd_pre_set_buffer( row, start_x, BITS_EACH_BYTE );
  
    //update buffer  
    for( i = 0; i < 12; i++, HL++ )
    {
        for( j = 0; j < BITS_EACH_BYTE; j++ )
        {
            if( ( 0x80 >> j ) & (*HL) )
            {
                ds_sublcd_putpixel( (OP_UINT8)(start_x + j), start_y, input );
            }
            else
            {
                ds_sublcd_putpixel( (OP_UINT8)(start_x + j), start_y, (OP_UINT8)(~input) );
            }
        }
        start_y++;
    }

}

/*==================================================================================================
    FUNCTION: ds_sublcd_chinese_out

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static void ds_sublcd_chinese_out
( 
    OP_UINT8 row, 
    OP_UINT8 column, 
    OP_UINT16 data 
)
{
    return ;
#if 0    
    OP_UINT8            i, j, start_x, start_y;
    OP_UINT8            input = 0xff;
    OP_UINT8           font_bmp[32];
    OP_UINT8           font_size;

    op_memset(&font_bmp[0],0x00,sizeof(font_bmp));
#ifdef PACIFIC_VERSION
     font_size = FONT12;
#else
    font_size = FONT16;
#endif
    
    if(Get_Chinese_Font( font_size,data,font_bmp))
    {
          start_x = column * (BITS_EACH_BYTE)+sublcd_center_offset;

         if ( column < SUBLCD_COL_MAX-1 )
        {
            start_y = ds_sublcd_pre_set_buffer( row, start_x, font_size );
            
            for(j=0; j< font_size; j++) 
            {
                for(i=0; i< BITS_EACH_BYTE; i++) 
                {
                    if( font_bmp[2*j]&(0x80>>i)) 
                    { 
                        ds_sublcd_putpixel( (OP_UINT8)(start_x +i), start_y, input );
                    }
                    else
                    {
                        ds_sublcd_putpixel( (OP_UINT8)(start_x +i), start_y, (OP_UINT8)(~input) );
                    } 
                }   
                
                for(i=0; i<BITS_EACH_BYTE; i++) 
                { 
                    if( font_bmp[2*j+1]&(0x80>>i)) 
                    { 
                        ds_sublcd_putpixel( (OP_UINT8)(start_x + i+BITS_EACH_BYTE), start_y, input );
                    }
                    else
                    {
                        ds_sublcd_putpixel( (OP_UINT8)(start_x + i+BITS_EACH_BYTE), start_y, (OP_UINT8)(~input) );
                    }
                } 
                start_y++;
            }
        }
        else
        { 
            /* no enough pixel for chinese so just delete remain pixel and do not write data */
            start_y = ds_sublcd_pre_set_buffer( row, start_x, BITS_EACH_BYTE );    
        }
    }
#endif
}


/*==================================================================================================
    FUNCTION: ds_sublcd_line

    DESCRIPTION:
        min : 0 ~ max : 79 , OP_TRUE : black
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static void ds_sublcd_line
( 
    OP_INT8 startx, 
    OP_INT8 starty, 
    OP_INT8 endx, 
    OP_INT8 endy, 
    OP_BOOLEAN b_or_w )
{
  OP_INT8  idx, idy;
  OP_INT8  inc_x, inc_y;
  OP_INT8  deltaP, deltaN;
  OP_INT8  G,i,j;

  if (startx < 0 || startx > (SUBLCD_X_SIZE -1) || 
      endx < 0 || endx > (SUBLCD_X_SIZE -1) ||
      starty < 0 || starty > (SUBLCD_Y_SIZE -1) ||
      endy < 0 || endy > (SUBLCD_Y_SIZE-1) ) return;

  idx = abs(endx - startx);
  idy = abs(endy - starty);

  if (endx > startx)      inc_x = 1;
  else if (endx < startx) inc_x = -1; 
  else                    inc_x = 0;

  if (endy > starty)      inc_y = 1;
  else if (endy < starty) inc_y = -1;
  else                    inc_y = 0;

  if (idx > idy) { 
    G = 2*idy - idx;
    deltaP = idy * 2;
    deltaN = (idy - idx) * 2;
    for (i = startx ; i != endx; i+=inc_x) {
      if (G > 0) {
        starty += inc_y;
        G += deltaN;
      } else {
        G += deltaP;
      }
      if ( b_or_w )
         ds_sublcd_putpixel(i, starty, 1); //black
      else
         ds_sublcd_putpixel(i, starty, 0); // white          
    }
  } else if (idy > idx) { 
    G = 2*idx - idy;
    deltaP = idx * 2;
    deltaN = (idx - idy) * 2;
    for (i = starty ; i != endy; i+=inc_y) {
      if (G > 0) {
        startx += inc_x;
        G += deltaN;
      } else {
        G += deltaP;
      }
      if ( b_or_w )
         ds_sublcd_putpixel(startx, i, 1); // black
      else
         ds_sublcd_putpixel(startx, i, 0); // white
    }
  } else { 
    for (i=startx, j=starty; i!=endx; i+=inc_x, j+=inc_y) {
        if ( b_or_w )
      ds_sublcd_putpixel(i, j, 1); // black
        else
         ds_sublcd_putpixel(i, j, 0); // white
        }
  }
  if ( b_or_w )
     ds_sublcd_putpixel(endx, endy, 1); // black
  else
     ds_sublcd_putpixel(endx, endy, 0); // white
}

/*==================================================================================================
    FUNCTION: 

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_text_write_line
(
    SUBLCD_TXT_LINE_ID    line_no,
    OP_UINT8    text_align_option,
    OP_UINT8  *text_data_ptr
)
{
/*
  * Assume, the number of characters sublcd displayed data string must be within 20 characters
 */    
    OP_UINT8 indent;
    OP_UINT8 char_num=0;
    OP_UINT8 text_buf[40];
    OP_UINT8 temp_buf[20];
    
    if( line_no == 0 )
        return;
    op_memset(text_buf , 0 , 40);
    op_memset(temp_buf, 0 , 20);
    sublcd_center_offset =(SUBLCD_X_SIZE -SUBLCD_COL_MAX*8)/2+SUBLCD_START_COL;
    char_num = UstrCharCount(text_data_ptr);
    
    if( (text_align_option == DISP_TXT_CENTER)||
        (text_align_option == DISP_TXT_RIGHT) )
    {
        if ( char_num <= SUBLCD_COL_MAX )
        {
            if( text_align_option == DISP_TXT_CENTER )
            {
                indent = (SUBLCD_COL_MAX - char_num)/2 ;
                ds_sublcd_text_write_rm( line_no, indent, text_data_ptr );                
            }
            else
            { /* DISP_TXT_RIGHT */
                indent = SUBLCD_COL_MAX - char_num;
                ds_sublcd_text_write_rm( line_no, indent, text_data_ptr );                
            }
        }
        else
        { /* marque */
            ds_sublcd_text_write_marque(line_no, text_data_ptr);                    
            OPUS_Stop_Timer(OPUS_TIMER_SUBLCD_MARQUE);
            OPUS_Start_Timer(OPUS_TIMER_SUBLCD_MARQUE, 1000,0, PERIODIC );
        }
    }
    else if( text_align_option == DISP_TXT_TWO_LINE )
    {        
        if( char_num > SUBLCD_COL_MAX )
        {
            if( line_no == 3 )
            {
                indent = 0;
                Ustrcpy( text_buf, text_data_ptr );
                AtoU( temp_buf, (OP_UINT8 *)"..." );
                Ustrcpy( text_buf+14, temp_buf );                    
                ds_sublcd_text_write_rm( line_no, indent, text_buf );
            }
            else
            {
                indent = 0;
                Ustrncpy( text_buf, text_data_ptr, SUBLCD_COL_MAX*2 );
                ds_sublcd_text_write_rm( line_no, indent, text_buf );
                if( char_num>(SUBLCD_COL_MAX*2) )
                {
                    Ustrncpy( text_buf, text_data_ptr,  SUBLCD_COL_MAX*2 );  
                    *(text_buf+SUBLCD_COL_MAX*2)=0;
                    *(text_buf+SUBLCD_COL_MAX*2+1)=0;
                }    
                else
                {
                    Ustrcpy( text_buf, text_data_ptr+SUBLCD_COL_MAX*2 );
                }
                ds_sublcd_text_write_rm( (SUBLCD_TXT_LINE_ID)(line_no+1), indent, text_buf );
            }
        }
        else if ( char_num == SUBLCD_COL_MAX )
        { /* DISP_TXT_LEFT */ 
            indent = 0;
            ds_sublcd_text_write_rm( line_no, indent, text_data_ptr );                 
        }
        else
        { /* DISP_TXT_CENTER */
            indent = (SUBLCD_COL_MAX - char_num)/2;
            ds_sublcd_text_write_rm( line_no, indent, text_data_ptr );                     
        }
    }
    else if( text_align_option == DISP_TXT_MARQUE )
    {        
        ds_sublcd_text_write_marque(line_no, text_data_ptr);                    
        OPUS_Stop_Timer(OPUS_TIMER_SUBLCD_MARQUE);
        OPUS_Start_Timer(OPUS_TIMER_SUBLCD_MARQUE, 1000,0, PERIODIC );
    }
    else 
    {
        indent = 0;
        ds_sublcd_text_write_rm( line_no, indent, text_data_ptr );                
    }
}

void ds_sublcd_text_write_marque
(
    SUBLCD_TXT_LINE_ID    marque_line_no, // 1 -3
    OP_UINT8    *text_data_ptr
)
{
    sublcd_marque_char[marque_line_no-1] = 0;    
    sublcd_marque_column[marque_line_no-1] = 2;

    sublcd_marque_txt[marque_line_no-1].marque_onoff = OP_TRUE;
    Ustrcpy( (sublcd_marque_txt[marque_line_no-1].sublcd_marque_txt), text_data_ptr );

    Umemset( sublcd_marque_txt_buf[marque_line_no-1], 0x0000, SUBLCD_COL_MAX );
    Ustrncpy( sublcd_marque_txt_buf[marque_line_no-1], text_data_ptr ,20);
    ds_sublcd_clear_text_line(marque_line_no);
    ds_sublcd_text_write_rm( marque_line_no, 0, sublcd_marque_txt_buf[marque_line_no-1] );        
 }

void ds_sublcd_marque_init
(
    void
)
{
    OP_UINT8    i;

    for( i=0; i<TXT_LINE_MAX-1; i++ )
    {
        sublcd_marque_char[i] = 0;    
        sublcd_marque_column[i] = 2;
        Umemset( sublcd_marque_txt[i].sublcd_marque_txt, 0x0000, SUBLCD_MARQUE_MAX_CHAR );
        Umemset( sublcd_marque_txt_buf[i], 0x0000, SUBLCD_MARQUE_MAX_CHAR );
        sublcd_marque_txt[i].marque_onoff = OP_FALSE;        
    }        
}

/* 
 * 1. Check characters in a buffer : if chinese character hang on the edge of buffer index 9(0-9)
 *    truncate character
 * 1.5. Display characters
 * 2. Scroll characters forward(left) : if chinese coming into buffer, scroll two numeric character space size
 * 3. Rescroll characters right to left 
 */
void ds_sublcd_marque_text_move
(
    void
)
{
    OP_UINT8 temp_byte;
    OP_UINT8 temp_column;
    OP_UINT8 marque_line_no;

    for ( marque_line_no = 0; marque_line_no < 3; marque_line_no++ )
    {
        if ( sublcd_marque_txt[marque_line_no].marque_onoff == OP_TRUE )
        {

            if ( sublcd_marque_first_loop[marque_line_no] )
            {
                Ustrncpy( sublcd_marque_txt_buf[marque_line_no], (sublcd_marque_txt[marque_line_no].sublcd_marque_txt +sublcd_marque_char[marque_line_no]), 20 );
                ds_sublcd_clear_text_line((SUBLCD_TXT_LINE_ID)(marque_line_no+1));
                ds_sublcd_text_write_rm( (SUBLCD_TXT_LINE_ID)(marque_line_no+1), 0, sublcd_marque_txt_buf[marque_line_no] );    

                sublcd_marque_char[marque_line_no] = sublcd_marque_char[marque_line_no] + 2;

                if (sublcd_marque_char[marque_line_no] >=20 )
                {
                    temp_byte = *((sublcd_marque_txt[marque_line_no].sublcd_marque_txt)+sublcd_marque_char[marque_line_no]);
                    if ( temp_byte == 0x00 )
                    {
                        temp_byte = *((sublcd_marque_txt[marque_line_no].sublcd_marque_txt)+sublcd_marque_char[marque_line_no]+1);
                        if ( temp_byte == 0x00 )
                        {
                            sublcd_marque_char[marque_line_no] = 0; /* Marque starting point init */
                            sublcd_marque_first_loop[marque_line_no] = OP_FALSE;
                        }
                    }
                }
            }
            else
            {
                Ustrncpy( sublcd_marque_txt_buf[marque_line_no], (sublcd_marque_txt[marque_line_no].sublcd_marque_txt +sublcd_marque_char[marque_line_no] ), 20 );    
                ds_sublcd_clear_text_line((SUBLCD_TXT_LINE_ID)(marque_line_no+1));
                if (sublcd_marque_column[marque_line_no] < SUBLCD_COL_MAX)
                {
                    temp_column = SUBLCD_COL_MAX-sublcd_marque_column[marque_line_no];
                }
                else
                {
                    temp_column = 0;
                }
                ds_sublcd_text_write_rm( (SUBLCD_TXT_LINE_ID)(marque_line_no+1), temp_column, sublcd_marque_txt_buf[marque_line_no] );        

                sublcd_marque_column[marque_line_no]++;
                if (sublcd_marque_column[marque_line_no] >= SUBLCD_COL_MAX)
                {
                    sublcd_marque_column[marque_line_no] = SUBLCD_COL_MAX;

                    sublcd_marque_char[marque_line_no] = sublcd_marque_char[marque_line_no] + 2;
                    if (sublcd_marque_char[marque_line_no] >=20 )
                    {
                        temp_byte = *((sublcd_marque_txt[marque_line_no].sublcd_marque_txt)+sublcd_marque_char[marque_line_no]);
                        if ( temp_byte == 0x00 )
                        {
                            temp_byte = *((sublcd_marque_txt[marque_line_no].sublcd_marque_txt)+sublcd_marque_char[marque_line_no]+1);
                            if ( temp_byte == 0x00 )
                            {
                                sublcd_marque_char[marque_line_no] = 0; /* Marque starting point init */
                                sublcd_marque_column[marque_line_no] = 2;
                            }
                        }
                    }
                }            
            }        

        }

    }
    
}

void ds_sublcd_clear_text_line
(
    SUBLCD_TXT_LINE_ID line_no
)
{
    OP_UINT8 txt_buf[SUBLCD_MARQUE_MAX_CHAR];
    Umemset( txt_buf, 0x0020, SUBLCD_MARQUE_MAX_CHAR/2 );
    ds_sublcd_text_write_rm( line_no, 0, txt_buf );
}
/*==================================================================================================
    FUNCTION: ds_sublcd_text_write

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_text_write
( 
    OP_UINT8 row_offset, 
    OP_UINT8 column_offset, 
    OP_UINT8 * text_ptr 
)
{
    OP_UINT8 row, column;

    SP_set_sublcd_screen_mode(SUBLCD_DISPLAY_TEXT_MODE);
    row = row_offset;
    for( column = column_offset; column < SUBLCD_COL_MAX; column++ )
    {
        /* ASCII */
        if( *text_ptr < 0x80 )
        {
            if( *text_ptr == 0x00 ) 
            {
                break;
            }
            ds_sublcd_ascii_out( row, column, (OP_UINT8)(column_offset*8), *text_ptr );
            text_ptr++;
        }
    }
}

/*==================================================================================================
    FUNCTION: ds_sublcd_text_write_rm

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_text_write_rm
( 
    SUBLCD_TXT_LINE_ID row_offset, 
    OP_UINT8 column_offset, 
    OP_UINT8 * text_ptr 
)
{
    OP_UINT8 row, column;
    OP_UINT8  ascii;    
    OP_UINT16 unicode_ptr;
    OP_UINT16 unicode_ptr2;    

    row = (OP_UINT8)row_offset;
    for( column = column_offset; column < SUBLCD_COL_MAX; column++ )
    {
        unicode_ptr = *text_ptr;        
        text_ptr++;
        unicode_ptr2 = *text_ptr;        
        text_ptr++;
        unicode_ptr2 = unicode_ptr2 << 8;
        unicode_ptr = unicode_ptr | unicode_ptr2;                
        
        if( unicode_ptr == 0x0000 ) 
        {
            break;
        }
        else if ( ( unicode_ptr >= 0x0080) && ( unicode_ptr <= 0xffe5))
        { 
            ds_sublcd_chinese_out( row, column,  unicode_ptr );
            column++;
        }
        else if( unicode_ptr < 0x0080 )
        {
            ascii = (OP_UINT8)((unicode_ptr) & 0x00ff);            
            ds_sublcd_ascii_out( row, column, (OP_UINT8)(column_offset*8), ascii );
        } 
    }
}

/*==================================================================================================
    FUNCTION: ds_sublcd_text_format

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_text_format
( 
    OP_UINT8 * input_str, 
    OP_UINT8 * out_buffer 
)
{
    AtoU (out_buffer, input_str );
}

/*==================================================================================================
    FUNCTION: ds_sublcd_text_format_rm

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_text_format_rm
( 
    RM_RESOURCE_ID_T res_id, 
    OP_UINT8 * out_buffer 
)
{
    OP_UINT8    sub_str[SUB_MAX_INPUT];
    OP_UINT16    res_len;    
    OP_UINT16   i;

    RM_RETURN_STATUS_T    rm_status;
    
    if (util_cnvrt_res_into_text(res_id,sub_str) != OP_NULL)
    {
        rm_status = RM_GetResourceLength(res_id, &res_len);    
        for ( i = 0; i < res_len; i++, out_buffer++ )
        {
            *out_buffer = sub_str[i];
        }
    }

}

/*==================================================================================================
    FUNCTION: ds_sublcd_time_display

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_time_display
(
    SP_SUBLCD_TIME_MODE_ENUM_T sublcd_time_mode,
    OP_TIMESTAMP cur_time 
)
{
#ifdef SUBLCD

    TIME_DATE   timedate;
    OP_UINT8    hour_10, hour_1, minute_10, minute_1;
    OP_UINT8    month_10, month_1, day_10, day_1, week_1;    
    OP_BOOLEAN  am_flag=OP_TRUE;
    OP_UINT8    x_start_offset = 0;
    OP_UINT8    image_width;
    OP_UINT8    image_high;
    OP_UINT8    image_y_offset;
    OP_UINT8    week_offset;
    RM_SUBLCD_IMAGE_T * char_resrc_id;

    ds_sublcd_graphic_screen_buffer_clear();
    op_secs_to_timedate( cur_time, &timedate);    
    if(sp_get_current_time_format() == TIME_FORMAT_12HOURS)  /* the timr mode is 12*/
    {
    if ( timedate.Time.Hours > 12 )
    {
        timedate.Time.Hours -= 12 ;
        am_flag = OP_FALSE;
    }
    else if ( timedate.Time.Hours == 12 )
    {
        am_flag = OP_FALSE;
    }
    else if ( timedate.Time.Hours == 0 )
    {
        timedate.Time.Hours  = 12 ;
    }
    }
    else if(sp_get_current_time_format() == TIME_FORMAT_24HOURS)   /*time is 24*/
    {
        x_start_offset = x_start_offset+SUBLCD_AP_WIDTH/2;
    }
    hour_10 = timedate.Time.Hours / 10;
    hour_1 = timedate.Time.Hours % 10;
    minute_10 = timedate.Time.Minutes / 10;
    minute_1 = timedate.Time.Minutes % 10;            
    switch ( sublcd_time_mode )
    { 
        case SP_SUBLCD_DIGITAL_TIME :
            SP_set_sublcd_full_screen_mode(OP_FALSE);   /*start col shpuld be */

            x_start_offset = x_start_offset +(SUBLCD_X_SIZE-SUBLCD_FONT_L_WIDTH_F-3*SUBLCD_FONT_L_WIDTH
                                        -SUBLCD_COLON_L_WIDTH - SUBLCD_AP_L_WIDTH)/2;
            image_y_offset = SUBLCD_ANNUN_HIGH;
            image_high  = SUBLCD_FONT_L_HEIGHT;
            image_width = SUBLCD_FONT_L_WIDTH;
            if(hour_10 == 0) 
            {
                image_width = SUBLCD_FONT_L_WIDTH_F;
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_FRONT_0_L);
                ds_sublcd_image_out( image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
                image_width =   image_width/2+1;
            }
            else if(hour_10 ==1)
            {
                image_width = SUBLCD_FONT_L_WIDTH_F;
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_FRONT_1_L);
                ds_sublcd_image_out( image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
                
            }
            else if(hour_10 ==2)
            {
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_2_L);
                ds_sublcd_image_out( image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
                
            }
            
            x_start_offset = x_start_offset + image_width;
            image_width = SUBLCD_FONT_L_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_0_L+hour_1);
            ds_sublcd_image_out(  image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
            
            x_start_offset = x_start_offset + image_width;
            image_width = SUBLCD_COLON_L_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_COLON_L);
            ds_sublcd_image_out(  image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
            
            x_start_offset = x_start_offset + image_width;
            image_width = SUBLCD_FONT_L_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_0_L+minute_10);
            ds_sublcd_image_out( image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
            
            x_start_offset = x_start_offset + image_width;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_0_L+minute_1);
            ds_sublcd_image_out(  image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
            
            x_start_offset = x_start_offset + image_width;
            image_width = SUBLCD_AP_L_WIDTH;
            
            if(sp_get_current_time_format() == TIME_FORMAT_12HOURS)
            {
                if( am_flag == OP_TRUE )
                {
                    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_AM_L);
                    ds_sublcd_image_out(  image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
                }
                else
                {
                    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_PM_L);
                    ds_sublcd_image_out(  image_width, image_high, x_start_offset, image_y_offset, char_resrc_id->data);
                }
            }
            break;
        case SP_SUBLCD_DIGITAL_TIME_DATE :
             SP_set_sublcd_full_screen_mode(OP_FALSE);

             x_start_offset = x_start_offset +(SUBLCD_X_SIZE-SUBLCD_TIME_WIDTH_F-3*SUBLCD_TIME_WIDTH
                                        -SUBLCD_COLON_WIDTH - SUBLCD_AP_WIDTH)/2;

             image_y_offset = SUBLCD_ANNUN_HIGH;
             image_high  = SUBLCD_TIME_HIGH;
             image_width = SUBLCD_TIME_WIDTH;

            /*draw the first font*/
            if(hour_10 == 0) 
            {
                image_width = SUBLCD_TIME_WIDTH_F;
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_FRONT_0_S);
                ds_sublcd_image_out(image_width,image_high,
                                    x_start_offset,image_y_offset, char_resrc_id->data);
                image_width = image_width/2+1;
            }
            else
            {
                if(hour_10 == 1)
                {
                    image_width = SUBLCD_TIME_WIDTH_F;
                    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_FRONT_1_S);
                    ds_sublcd_image_out( image_width,image_high,
                                    x_start_offset,image_y_offset, char_resrc_id->data);
                }
                if(hour_10 == 2)
                {
                    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_2_S);
                    ds_sublcd_image_out( image_width,image_high,
                                    x_start_offset,image_y_offset, char_resrc_id->data);
                }
                
            }
          
            /*draw the sec font*/
            x_start_offset= x_start_offset+image_width;     
            image_width = SUBLCD_TIME_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_0_S+hour_1);
            ds_sublcd_image_out( image_width,image_high,
                                 x_start_offset, image_y_offset, char_resrc_id->data);

            /*draw the sec colon*/
            x_start_offset= x_start_offset+image_width;
            image_width = SUBLCD_COLON_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_COLON_S);
            ds_sublcd_image_out(image_width,image_high,
                                x_start_offset, image_y_offset, char_resrc_id->data);

             /*draw the min first font*/
            x_start_offset= x_start_offset+image_width;
            image_width = SUBLCD_TIME_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_0_S+minute_10);
            ds_sublcd_image_out(image_width, image_high,
                               x_start_offset,image_y_offset, char_resrc_id->data);

             /*draw the min sec font*/
            x_start_offset= x_start_offset+image_width;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_0_S+minute_1);
            ds_sublcd_image_out(image_width,image_high,
                                x_start_offset,image_y_offset, char_resrc_id->data);

            /*begin to draw the AM &pm*/
            x_start_offset = x_start_offset+image_width;
            image_width = SUBLCD_AP_WIDTH;
            if(sp_get_current_time_format() == TIME_FORMAT_12HOURS)
            {
                if( am_flag == OP_TRUE )
                { 
                    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_AM_S);
                }  
                else
                {  
                    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_PM_S);
                }
                    ds_sublcd_image_out(image_width,image_high,
                                       x_start_offset,image_y_offset, char_resrc_id->data);                
            }  

            /*begin to draw the date and week TO SENCOND LINE*/
            month_10 = timedate.Date.Month/10;
            month_1  = timedate.Date.Month %10;
            day_10 = timedate.Date.Day/10;
            day_1  = timedate.Date.Day %10;
            week_1 = timedate.Date.DayOfWeek;
            x_start_offset = SUBLCD_TIME_DATE_OFFSET;
            week_offset =(SUBLCD_X_SIZE-SUBLCD_WEEK_WIDTH-2*SUBLCD_BR_WIDTH - x_start_offset);

            image_y_offset = image_y_offset+ SUBLCD_TIME_HIGH+1;
            image_high  = SUBLCD_DATE_HIGH;
            image_width = SUBLCD_DATE_WIDTH;
        
            if ( month_10 == 0 )
            {
                image_width = SUBLCD_DATE_WIDTH_F;
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_FRONT_0);
                ds_sublcd_image_out(image_width,image_high,
                                                    x_start_offset,image_y_offset, char_resrc_id->data);                
                image_width = 0;
            }
            else
            {
                image_width = SUBLCD_DATE_WIDTH_F;
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_FRONT_1);
                ds_sublcd_image_out(image_width,image_high,
                                    x_start_offset,image_y_offset, char_resrc_id->data);                
            }

            /*BEGIN TO DRAW THE sec font*/
            x_start_offset = x_start_offset+image_width;
            image_width = SUBLCD_DATE_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_0+month_1);
            ds_sublcd_image_out( image_width, image_high,
                                  x_start_offset,image_y_offset, char_resrc_id->data);                

            /*begin to draw the slash*/
            x_start_offset = x_start_offset+image_width;
            image_width = SUBLCD_SLASH_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_SLASH);
            ds_sublcd_image_out(image_width,image_high,
                               x_start_offset,image_y_offset, char_resrc_id->data);                
            
            /*draw forth font*/
            x_start_offset = x_start_offset+image_width;
            image_width = SUBLCD_DATE_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_0+day_10);
            ds_sublcd_image_out(image_width, image_high,
                                 x_start_offset, image_y_offset, char_resrc_id->data);                

            x_start_offset = x_start_offset+image_width;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_0+day_1);
            ds_sublcd_image_out(image_width, image_high,
                                 x_start_offset,image_y_offset, char_resrc_id->data);                

            /*draw "["*/
            x_start_offset = week_offset;
            image_width = SUBLCD_BR_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_OPEN_BR);
            ds_sublcd_image_out(image_width, image_high,
                                x_start_offset,image_y_offset, char_resrc_id->data);  

            x_start_offset = x_start_offset+SUBLCD_WEEK_WIDTH+SUBLCD_BR_WIDTH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_DAY_CLOSE_BR);
            ds_sublcd_image_out(image_width, image_high, 
                                 x_start_offset,image_y_offset, char_resrc_id->data);                


            /*draw week*/
            image_width = SUBLCD_WEEK_WIDTH;
            x_start_offset = x_start_offset-SUBLCD_WEEK_WIDTH;
            if ( RM_GetCurrentLanguage() == RM_LANG_ENG )
            {
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_MON+week_1*2);
            }
            else
            {
                char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_FONT_MON+week_1*2+1);
            }
            ds_sublcd_image_out(image_width, image_high,
                            x_start_offset,image_y_offset, char_resrc_id->data);                
            break;
        case SP_SUBLCD_ANALOG :
            SP_set_sublcd_full_screen_mode(OP_TRUE);
            image_width = SUBLCD_X_SIZE;
            image_high  = SUBLCD_POWER_ON_HIGH;
            char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_IMG_ANALOG_CLOCK);        
            ds_sublcd_image_out(image_width,image_high, 
                                0, 0, char_resrc_id->data );  
            ds_sublcd_display_analog_clock((OP_UINT8)(((timedate.Time.Hours > 11)?(timedate.Time.Hours-12):timedate.Time.Hours)),
                                           timedate.Time.Minutes,
                                           timedate.Time.Seconds);
            break;
    }
    ds_sublcd_do_refresh();
#endif
}

/*==================================================================================================
    FUNCTION: ds_sublcd_display_analog_clock

    DESCRIPTION:
        display analog clock.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        This function is a inline type for speed up.
==================================================================================================*/
static void ds_sublcd_display_analog_clock
(
    OP_UINT8 hours,
    OP_UINT8 minutes,
    OP_UINT8 seconds
)
{

    OP_INT16 theta_h;
    OP_INT16 theta_h1;    
    OP_INT16 theta_h2;        
    
    OP_INT16 theta_m;

    double cos_theta_h;
    double sin_theta_h;
    double cos_theta_m;
    double sin_theta_m;
    OP_INT16 i;
    OP_INT16 j;    
    OP_INT16 x;
    OP_INT16 y;
    
    theta_m = (OP_INT16)(6*minutes);
    cos_theta_m = (double)op_cos(theta_m);
    sin_theta_m = (double)op_sin(theta_m);

    if ( (minutes % 2) == 0 )
    {
        theta_h = (OP_INT16)(30*hours+theta_m/12);
        cos_theta_h = (double)op_cos(theta_h);
        sin_theta_h = (double)op_sin(theta_h);
    }
    else
    {
        theta_h1 = (OP_INT16)(30*hours+(theta_m-6)/12);
        theta_h2 = (OP_INT16)(30*hours+(theta_m+6)/12);
        /* interpolation */
        cos_theta_h = (double)((op_cos(theta_h1)+op_cos(theta_h2))/2.0);
        sin_theta_h = (double)((op_sin(theta_h1)+op_sin(theta_h2))/2.0);
    }

    /* hand of minutes */
    for ( i = (39-SUBLCE_CENTER_OF_ANALOG_CLOCK_X); i < (40-SUBLCE_CENTER_OF_ANALOG_CLOCK_X); i++ )
    {
        for ( j = (11-SUBLCE_CENTER_OF_ANALOG_CLOCK_Y); j < (24-SUBLCE_CENTER_OF_ANALOG_CLOCK_Y); j++ )
        {
            x = (OP_INT16)(i * cos_theta_m - j * sin_theta_m) + SUBLCE_CENTER_OF_ANALOG_CLOCK_X;
            y = (OP_INT16)(i * sin_theta_m + j * cos_theta_m) + SUBLCE_CENTER_OF_ANALOG_CLOCK_Y;
            ds_sublcd_putpixel_graphic( (OP_UINT8)x,
                                (OP_UINT8)y,
                                (OP_UINT8)1 );
        }
    }

    /* hand of hour */
    for ( i = (39-SUBLCE_CENTER_OF_ANALOG_CLOCK_X); i < (41-SUBLCE_CENTER_OF_ANALOG_CLOCK_X); i++ )
    {
        for ( j = (14-SUBLCE_CENTER_OF_ANALOG_CLOCK_Y); j < (24-SUBLCE_CENTER_OF_ANALOG_CLOCK_Y); j++ )
        {
            x = (OP_INT16)(i * cos_theta_h - j * sin_theta_h) + SUBLCE_CENTER_OF_ANALOG_CLOCK_X;
            y = (OP_INT16)(i * sin_theta_h + j * cos_theta_h) + SUBLCE_CENTER_OF_ANALOG_CLOCK_Y;
            ds_sublcd_putpixel_graphic( (OP_UINT8)x, 
                                (OP_UINT8)y, 
                                (OP_UINT8)1 );
        }
    }
}


/*==================================================================================================
    FUNCTION: ds_sublcd_contrast 

    DESCRIPTION:
        Set sub lcd contrast.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void ds_sublcd_contrast
(
    OP_UINT8    contrast_index
)
{
#ifndef WIN32
     if ( contrast_index>0 && contrast_index <= SUBLCD_CONTRAST_MAX_INDEX )
     {
         /* set new contrast value to the lcd */
         s44000a01_volume_set(SUBLCD_CONTRAST_VALUES[contrast_index-1]);                            
     }
#endif
}

/*==================================================================================================
    FUNCTION: sublcd_welcome 

    DESCRIPTION:
        Display welcome message on sub lcd.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void sublcd_welcome
(
    void
)
{
#ifdef DS_OPEN_WINDOW
    return; 
#else

    OP_UINT16 i,j;
    RM_SUBLCD_IMAGE_T  *char_resrc_id;
    OP_UINT8       uTempBuf[SUBLCD_MAX_PAGE][SUBLCD_X_SIZE];
    #ifndef WIN32
     OP_UINT8  page;
    #endif
    op_memset(uTempBuf,0x00,sizeof(uTempBuf));
    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_IMG_POWER_ON);

    for(i=0;i<SUBLCD_MAX_PAGE-1;i++)
    {
        for(j=0;j< SUBLCD_POWER_ON_WIDTH;j++)
       {
            uTempBuf[i][j] = char_resrc_id->data[i*SUBLCD_POWER_ON_WIDTH+j];
       }
    }
#ifndef WIN32
        for( page = 0; page < SUBLCD_MAX_PAGE; page++ )
        {
            s44000a01_write(page, 0, SUBLCD_X_SIZE, (OP_UINT8*)&(uTempBuf[page][0]));
        }
#endif
#endif
}

/*==================================================================================================
    FUNCTION: sublcd_goodbye 

    DESCRIPTION:
        Display welcome message on sub lcd.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/

void sublcd_goodbye
(
    void
)
{
#ifdef DS_OPEN_WINDOW
    return;
#else

    OP_UINT16 i,j;
    RM_SUBLCD_IMAGE_T  *char_resrc_id;
    OP_UINT8       uTempBuf[SUBLCD_MAX_PAGE][SUBLCD_X_SIZE];
 #ifndef WIN32
     OP_UINT8  page;
 #endif
    op_memset(uTempBuf,0x00,sizeof(uTempBuf));
    char_resrc_id = util_get_sublcd_image_from_res(SUBLCD_IMG_POWER_OFF);

    for(i=0;i<SUBLCD_MAX_PAGE-1;i++)
    {
        for(j=0;j< SUBLCD_POWER_ON_WIDTH;j++)
       {
            uTempBuf[i][j] = char_resrc_id->data[i*SUBLCD_POWER_ON_WIDTH+j];
       }
    }
#ifndef WIN32
        for( page = 0; page < SUBLCD_MAX_PAGE; page++ )
        {
            s44000a01_write(page, 0, SUBLCD_X_SIZE, (OP_UINT8*)&(uTempBuf[page][0]));
        }
#endif

#endif
}


#ifdef DS_OPEN_WINDOW

#define OPENWINDOW_LINE_UNI_MAX    14
#define OPENWINDOW_TEXT_OFFSET_X   1
#define OPENWINDOW_TEXT_OFFSET_Y   1

/*==================================================================================================
    FUNCTION: ds_openwindow_int 

    DESCRIPTION:
        init openwindow buffer .
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES: now initial as black color.
==================================================================================================*/
void ds_openwindow_int(void)
{
  //  RM_INT_ANIDATA_T    *ani_ptr = OP_NULL;

    wmemset((OP_UINT16 *)open_window_screen, COLOR_BLACK, LCD_OPENWINDOW_MAX_Y * LCD_OPENWINDOW_MAX_X);
  /*  ani_ptr = util_get_animation_from_res(ANI_OPENWINDOW);
    draw_bitmap_image(AREAMASK_OPENWINDOW,
                      (OP_UINT8)((LCD_MAX_X - (ds_get_image_width(ANI_OPENWINDOW)))/2), 
                      LCD_OPENWINDOW_START_Y, 
                      ani_ptr->scene);            
    */       
}

/*==================================================================================================
    FUNCTION: ds_openwindow_clear_time_region 

    DESCRIPTION:
        Clear time display area.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES: now fill as black color.
==================================================================================================*/
void ds_openwindow_clear_time_region
(
    void
)
{
 /*   RM_INT_ANIDATA_T    *ani_ptr = OP_NULL;
    ani_ptr = util_get_animation_from_res(ANI_OPENWINDOW);
*/
#if 0
    ds_draw_partial_bitmap_image_rm(LCD_OPENWINDOW_START_Y, 
                                    (OP_INT16)(LCD_OPENWINDOW_ANNUN_HIGH + 2), 
                                    LCD_OPENWINDOW_WIDTH,
                                    (OP_INT16)(LCD_OPENWINDOW_HIGH-LCD_OPENWINDOW_ANNUN_HIGH), 
                                    0, 
                                    (OP_INT16)(LCD_OPENWINDOW_ANNUN_HIGH + 2), 
                                    BMP_OPENWINDOW_BG);
#else

    fill_rect(AREA_OPEN_WINDOW,
              LCD_OPENWINDOW_START_X, 
              LCD_TIMEDISPLAY_START_Y, 
              (OP_INT16)(LCD_OPENWINDOW_START_X + LCD_OPENWINDOW_WIDTH),
              (OP_INT16)(LCD_TIMEDISPLAY_START_Y
                                + ds_get_image_height(ICON_IDLE_TIMENUM_0)), 
              COLOR_BLACK);
/*     fill_rect(AREA_OPEN_WINDOW,
                 LCD_OPENWINDOW_START_X, 
                 SUBLCD_REMOVE_DATE_Y_TOP, 
                 (OP_INT16)(LCD_OPENWINDOW_START_X + LCD_OPENWINDOW_WIDTH),
                 SUBLCD_REMOVE_DATE_Y_BOTTOM,
                 COLOR_BLACK);

     draw_partial_bitmap_image(AREA_BITMAP,
                               LCD_OPENWINDOW_START_Y, 
                               LCD_TIMEDISPLAY_START_Y, 
                               LCD_OPENWINDOW_WIDTH,
                               (OP_UINT16)ds_get_image_height(ICON_OPENWIN_NUM_1), 
                               LCD_OPENWINDOW_START_Y, 
                               LCD_TIMEDISPLAY_START_Y,  
                               ani_ptr->scene);
    */
#endif
}

/*==================================================================================================
    FUNCTION: ds_openwindow_time_display 

    DESCRIPTION:
        display time in open window.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int ds_openwindow_time_display
(
  void
)
{
    OP_UINT8         timestr[DS_SCRN_MAX_UC_LEN+2]; /* time string in Unicode */
    OP_INT16         x;
    OP_INT16         y;
    OP_BOOLEAN       need_update = OP_FALSE;
    OP_TIMESTAMP     cur_time;
    OP_UINT8         num_icon_width=0;
    TIME_DATE        timedate;
    OP_UINT8         hour_10, hour_1, minute_10, minute_1;
   // OP_UINT8         bg_start_x;
#if (PROJECT_ID == WHALE1_PROJECT)
    OP_UINT8         colon_icon_width=0;   
    OP_UINT8         month_10, month_1, day_10, day_1; 
    OP_UINT8         month_flag;
    OP_UINT8         day_flag;
    colon_icon_width = (OP_UINT8)ds_get_image_width(ICON_OPENWIN_NUM_COLON);  
    y = LCD_OPENWINDOW_START_Y + OPENWINDOW_STATUS_ICON_HEIGHT;  
#else
    y = LCD_OPENWINDOW_START_Y + OPENWINDOW_STATUS_ICON_HEIGHT
          + OPENWINDOW_STATUS_DIANCHI_HEIGHT;
#endif

    /* Draw time and date */           
    cur_time = op_get_cur_tod();
    op_secs_to_timedate(cur_time, &timedate);          
    op_get_timestring( cur_time, (OP_UINT8 *)&timestr, TIMESTR_ALL );
      
    x = LCD_TIMEDISPLAY_START_X;
    y = LCD_TIMEDISPLAY_START_Y;

    num_icon_width = (OP_UINT8)ds_get_image_width(ICON_OPENWIN_NUM_0);
   
    /* Draw time and date */           
 #if 0
    ds_openwindow_clear_time_region();
    x = (OP_INT16)GET_CENTER(0, LCD_OPENWINDOW_WIDTH,  \
                                  UstrCharCount(timestr) * GET_FONT_WIDTH(FONT_SIZE_LARGE));
    ds_draw_text((OP_INT16)(x+1),
                                 (OP_INT16)(y+1),
                                 (OP_UINT8 *)timestr,
                                 FONT_SIZE_SMALL|FONT_OVERLAP,
                                 COLOR_GREEN,
                                 COLOR_TRANS);
 #else
    if(sp_get_current_time_format() == TIME_FORMAT_12HOURS)  /* the timr mode is 12*/
    {
        if ( timedate.Time.Hours > 12 )
        {
            timedate.Time.Hours -= 12 ;
        }
        else if ( timedate.Time.Hours == 12 )
        {
        }
        else if ( timedate.Time.Hours == 0 )
        {
            timedate.Time.Hours  = 12 ;
        }
    }
#if 0
    ds_draw_partial_bitmap_image_rm(x, y, 
                                                  LCD_OPENWINDOW_WIDTH,
                                                  (OP_INT16)(LCD_OPENWINDOW_HIGH-LCD_OPENWINDOW_ANNUN_HIGH), 
                                                  0, y, BMP_OPENWINDOW_BG);
#else
    ds_openwindow_clear_time_region();
#endif
    /* there is only one time mode */
    /* 15:38 */
    // if( sublcd_time_mode == SP_SUBLCD_DIGITAL_TIME )
    {
        x += 26;
        
        hour_10 = timedate.Time.Hours / 10;
        hour_1 = timedate.Time.Hours % 10;
        minute_10 = timedate.Time.Minutes / 10;
        minute_1 = timedate.Time.Minutes % 10;     

        if ( 0 == hour_10 )
        {
            x -= SUBLCD_TIME_LEFT_X_BELOW10;
        }

        /* draw the hour font */
        if(hour_10 == 0) 
        {
            x += num_icon_width;
        }
        else if(hour_10 ==1)
        {
            draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_1));
            x += num_icon_width;
        //    x +=  SUBLCD_TIME_MARGIN_X;
        }
        else if(hour_10 ==2)
        {
            draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_2));
            x += num_icon_width;
        //    x +=  SUBLCD_TIME_MARGIN_X;
        }
        /* draw the sec second */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+hour_1));
        x += num_icon_width;
        //x +=  SUBLCD_TIME_MARGIN_X;

        /*draw the sec colon*/
            draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_COLON));
#if (PROJECT_ID == WHALE1_PROJECT)
        x += colon_icon_width;
#else
        x += num_icon_width;
#endif
        //x +=  SUBLCD_TIME_MARGIN_X;

        /*draw the min first font*/
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+minute_10));
        x += num_icon_width;
        //x +=  SUBLCD_TIME_MARGIN_X;

        /*draw the min sec font*/
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+minute_1));
        x += num_icon_width;
        //x +=  SUBLCD_TIME_MARGIN_X;

#ifdef DATA_SET
    OP_UINT8    year_10, year_1, month_10, month_1, day_10, day_1, week_1;    

        /* draw space */
        //draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_SLASH));
        x += num_icon_width+1;
        x +=  SUBLCD_TIME_MARGIN_X;

        /* draw date, 04-05-12 */
        year_10 = (timedate.Date.Year/10) % 10;
        year_1 = timedate.Date.Year%10;
        month_10 = timedate.Date.Month/10;
        month_1  = timedate.Date.Month %10;
        day_10 = timedate.Date.Day/10;
        day_1  = timedate.Date.Day %10;

        /* draw year */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+year_10));
        x += num_icon_width;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+year_1));
        x += num_icon_width;

        /* draw space */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_SLASH));
        x += num_icon_width;
        
        /* draw month */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+month_10));
        x += num_icon_width;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+month_1));
        x += num_icon_width;

        /* draw space */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_SLASH));
        x += num_icon_width;

        /* draw day */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+day_10));
        x += num_icon_width;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+day_1));
        x += num_icon_width;
#endif
    }
#endif
#if (PROJECT_ID == WHALE1_PROJECT)
    /*draw the date&week in English mode */

    month_10 = timedate.Date.Month/10;
    month_1  = timedate.Date.Month %10;
    day_10 = timedate.Date.Day/10;
    day_1  = timedate.Date.Day %10;
    month_flag = (month_10 > 0)? OP_TRUE:OP_FALSE;
    day_flag = (day_10 > 0)? OP_TRUE:OP_FALSE;
    x = SUBLCD_DATE_X_POSITION;
    y = LCD_OPENWINDOW_START_Y + 2*OPENWINDOW_STATUS_ICON_HEIGHT + 4;
    if ( RM_LANG_CHN == RM_GetCurrentLanguage() )
    {   
        x = (LCD_MAX_X - 8*SUBLCD_DATE_ICON_WIDTH -month_flag*SUBLCD_DATE_ICON_WIDTH)/2;
        /*draw month icon */
        if (month_flag)
        {
            draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+month_10));
            x += SUBLCD_DATE_ICON_WIDTH;
        }
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+month_1));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_MONTH));
        x += SUBLCD_DATE_ICON_WIDTH;
        /*draw day icon */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+day_10));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+day_1));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DAY));
        x += SUBLCD_DATE_ICON_WIDTH;
        /*draw WEEK icon */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_LEFT_BRACKET));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_WEEK_1 + timedate.Date.DayOfWeek));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_RIGHT_BRACKET));
        x += SUBLCD_DATE_ICON_WIDTH;      
             
    }
    else
    {    
        x = (LCD_MAX_X - 6*SUBLCD_DATE_ICON_WIDTH - SUBLCD_DATE_ICON_WIDTH_E - month_flag*SUBLCD_DATE_ICON_WIDTH)/2;
        /*draw month icon */
        if (month_flag)
        {
            draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+month_10));
            x += SUBLCD_DATE_ICON_WIDTH;
        }
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+month_1));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_SLASH));
        x += SUBLCD_DATE_ICON_WIDTH;
        /*draw day icon */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+day_10));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_DATE_0+day_1));
        x += SUBLCD_DATE_ICON_WIDTH;

        /*draw WEEK icon */
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_LEFT_BRACKET));
        x += SUBLCD_DATE_ICON_WIDTH;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_WEEK_1_E + timedate.Date.DayOfWeek));
        x += SUBLCD_DATE_ICON_WIDTH_E;
        draw_icon(AREA_OPEN_WINDOW,x, y, (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_RIGHT_BRACKET));
        x += SUBLCD_DATE_ICON_WIDTH;     

    }
#endif

        
    return 1;
}

/*==================================================================================================
    FUNCTION: ds_openwindow_draw_text 

    DESCRIPTION:
        display text in openwindow, here will start the timer if string is too long.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int ds_openwindow_draw_text
(
    SUBLCD_DISPLAY_TEXT_STRUCTURE_NAME_T *txt_sublcd

)
{
    OP_UINT8 txt_len[SUB_MAX_LINE];
    OP_INT16 x[SUB_MAX_LINE];
    OP_UINT8 i;
    OP_INT16 y=OPENWINDOW_TEXT_OFFSET_Y;
   
    for(i=0;i<SUB_MAX_LINE;i++)
    {
        txt_len[i] = UstrSize(txt_sublcd->subline[i]);
        switch(txt_sublcd->disp_type[i])
        {
            case DISP_TXT_LEFT:
                break;
            case DISP_TXT_CENTER:
                if(txt_len[i] <= OPENWINDOW_LINE_UNI_MAX)
                {
                    x[i] = GET_CENTER(OPENWINDOW_TEXT_OFFSET_X, LCD_OPENWINDOW_WIDTH,(txt_len[i]*FONTX_NORMAL));
                }
              
                break;
            case DISP_TXT_RIGHT:
                if(txt_len[i] <= OPENWINDOW_LINE_UNI_MAX)
                {
                    x[i] = LCD_OPENWINDOW_WIDTH - (txt_len[i]*FONTX_NORMAL);
                } 
                break;
            case DISP_TXT_TWO_LINE:
                break;
            case DISP_TXT_MARQUE:
                //OPUS_Start_Timer(opus_event, delay, 500, ONE_SHOT);
                break;
            default:
                break;
		}  
    }
   
    fill_rect(AREA_OPEN_WINDOW,
              LCD_OPENWINDOW_START_X, 
              (OP_INT16)(LCD_TIMEDISPLAY_START_Y - OPENWINDOW_STATUS_ICON_HEIGHT/2 + 1), 
              (OP_INT16)(LCD_OPENWINDOW_START_X + LCD_OPENWINDOW_WIDTH),
              (OP_INT16)(LCD_TIMEDISPLAY_START_Y
                                + ds_get_image_height(ICON_IDLE_TIMENUM_0) + 
                                OPENWINDOW_STATUS_ICON_HEIGHT/2 -1 ), 
              COLOR_BLACK);

    draw_text(AREA_OPEN_WINDOW,
              x[0], 
              (OP_INT16)(LCD_TIMEDISPLAY_START_Y - 5), 
              (OP_INT16)(LCD_OPENWINDOW_WIDTH+LCD_OPENWINDOW_START_X),  //LCD_OPENWINDOW_MAX_X,
              txt_sublcd->subline[0], 
              FONT_DEFAULT|FONT_OVERLAP, 
              COLOR_GREEN, 
              COLOR_TRANS);
    
    draw_text(AREA_OPEN_WINDOW,
              x[1], 
              (OP_INT16)(LCD_TIMEDISPLAY_START_Y - 5 + LINE_HEIGHT), 
              (OP_INT16)(LCD_OPENWINDOW_WIDTH+LCD_OPENWINDOW_START_X),  //LCD_OPENWINDOW_MAX_X,
              txt_sublcd->subline[1], 
              FONT_DEFAULT|FONT_OVERLAP, 
              COLOR_GREEN, 
              COLOR_TRANS);
    
    /*draw_text(AREA_OPEN_WINDOW,
              x3, 
              (OP_INT16)(LCD_TIMEDISPLAY_START_Y - 5 + 2*LINE_HEIGHT), 
              (OP_INT16)(LCD_OPENWINDOW_WIDTH+LCD_OPENWINDOW_START_X),  //LCD_OPENWINDOW_MAX_X,
              text_ptr3, 
              FONT_DEFAULT|FONT_OVERLAP, 
              COLOR_GREEN, 
              COLOR_TRANS);   */
    return 1;
}

/*==================================================================================================
    FUNCTION: ds_openwindow_text_move 

    DESCRIPTION:
        display text in openwindow, here will start the timer if string is too long.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int ds_openwindow_text_move()
{
    return 1;
}
/*==================================================================================================
    FUNCTION: ds_openwindow_missed_call_display 

    DESCRIPTION:
        display missed call with icon resource in openwindow.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES: now it's useless.
==================================================================================================*/
int ds_openwindow_missed_call_display
(
    OP_UINT8   missed_counts
)
{
    OP_INT16 x;
    OP_INT16 y;

    char str_buf[4];
    if((missed_counts <= 0)||(missed_counts > 200))
    {
        return -1;
    }
    op_memset(str_buf, 0, 4);
    ds_openwindow_clear_time_region();
    x = LCD_OPENWINDOW_START_Y + 2;
    y = LCD_OPENWINDOW_ANNUN_HIGH + 2;    
    draw_icon(AREA_OPEN_WINDOW,x, y,
              (RM_ICON_T *)util_get_icon_from_res(ICON_STATUS_OPENWIN_SMS ));
    x += 15;
    op_sprintf(str_buf, "%d", missed_counts);
    
    if((str_buf[0]-0x30) > 0 )
    {
        draw_icon(AREA_OPEN_WINDOW, (OP_INT16)(x+10), y,
              (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+(str_buf[0]-0x30)));
        x += 5;
    }
    else if((str_buf[1]-0x30) > 0)
    {
        draw_icon(AREA_OPEN_WINDOW, (OP_INT16)(x+10), y,
              (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+(str_buf[1]-0x30)));
        x += 5;
    }else 
    {
        draw_icon(AREA_OPEN_WINDOW, (OP_INT16)(x+10), y,
              (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0+(str_buf[2]-0x30)));
    }

    return 1;
}

/*==================================================================================================
    FUNCTION: ds_openwindow_incoming_call_display 

    DESCRIPTION:
        display incoming call with icon resource in openwindow.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:now it's useless.
==================================================================================================*/
int ds_openwindow_incoming_call_display
(
    OP_UINT8   *number
)
{
    OP_UINT16 len;
    OP_UINT16 i;
    OP_INT16 x;
    OP_INT16 y;
    
    OP_UINT8 *pNum;
    
    if(number == OP_NULL)
    {
        return -1;
    }

    SP_set_sublcd_screen_mode(SUBLCD_DISPLAY_OWNERDRAW_MODE);
    ds_openwindow_clear_time_region();
    len = op_strlen((char *)number);
    pNum = number;
    x = LCD_OPENWINDOW_START_Y + 2;
    y = LCD_OPENWINDOW_ANNUN_HIGH + 2;    
    
    for(i=0; i<len; i++)
    {
        /* if( *pNum <= 0x39 )*/
        draw_icon(AREA_OPEN_WINDOW,x, y,
                  (RM_ICON_T *)util_get_icon_from_res(ICON_OPENWIN_NUM_0 + (*pNum - 0x30)));
        pNum ++;
        x += 5;
    }
    return 1;
    /* refresh in cust paint */    
}

/*==================================================================================================
    FUNCTION: ds_openwindow_refresh 

    DESCRIPTION:
        refresh open window to lcd buffer .
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
int ds_openwindow_refresh(void)
{
    LCD_RECT_T    rect;  /* relative popup update region pointer*/

    /*could not use the ds update rect, here just for initial,
    consider if give a new variable.*/
    rect.left = LCD_OPENWINDOW_START_X;
    rect.top = LCD_OPENWINDOW_START_Y;
    rect.right = (LCD_OPENWINDOW_START_X+LCD_OPENWINDOW_WIDTH);
    rect.bottom = LCD_MAX_Y_COOR;
    
    lcd_openwindow_refresh(rect);
    clear_update_region();
    
    return 1;
}

#endif
///////////////////////////////////////////////////////////////////////
/*================================================================================================*/

#ifdef __cplusplus
}
#endif

#ifndef DS_SUBLCD_H
#define DS_SUBLCD_H
/*==================================================================================================

    HEADER NAME : ds_sublcd.h

    GENERAL DESCRIPTION
        This file contains lcd refresh functions and required definition,

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    09/23/2002   Paul Seo         crxxxxx     Initial Creation
    10/10/2002   Ashley Lee       crxxxxx     Add ds_sublcd_contrast
    02/12/2003   Steven Lai       PP00069     Update the type of some parameter
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    03-04-04   Chenyu             PR    ----      P000041 
                                  - Fix sublcd display 
    03-05-14   Chenyu             PR    ----      P000257 
                                  - Replace the new open image of pacific sublcd &cleanup the code by config
    03-05-14   Chenyu             PR    ----      P000361 
    03-07-01   Chenyu             P000754        
                                  - Update sublcd  display because of changing size of sublcd 
    07/03/2003   Wang Peng        P000772      Adjust the text to align center when powerdown charging
    03-07-09   Chenyu             P000856        
                                  - Update sublcd display of pacific 
    03-07-25     Chenyu           P001015        
                                  - Update sublcd display of time
                                  - Update call animation
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    06/04/2004   linda wang       p006612     remove unused functions.
    07/24/2004   liren           p007234     change openwindow display in earphone connected
    This file contains lcd refresh functions and required definition,


====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "OPUS_typedef.h"
#include    "ds_def.h"
#include "s44000a01.h"
#include "SP_sublcd_status.h"
#include "product.h"
#include "SUBLCD_DISPLAY_main.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/

#define SUBLCD_X_SIZE                    S44000A01_X_SIZE
#define SUBLCD_Y_SIZE                    S44000A01_Y_SIZE
#define SUBLCD_MAX_PAGE                  S44000A01_MAX_PAGE

#define SUBLCD_ICON_MAX_PAGE             S44000A01_ICON_MAX_PAGE

#define SUBLCD_MARQUE_MAX_CHAR 20


#define      BITS_EACH_BYTE          8
#define      SUBLCD_MAX_BITS         90
#define      SUBLCD_ANNUN_HIGH       16
#define      SUBLCD_RASSI_WIDTH      21
#define      SUBLCD_BATTERY_WIDTH    16
#define      SUBLCD_MSG_WIDTH        13
#define      SUBLCD_VIB_WIDTH        14

#define      SUBLCD_TIME_HIGH         24
#define      SUBLCD_DATE_HIGH         16
#define      SUBLCD_TIME_WIDTH        15
#define      SUBLCD_TIME_WIDTH_F      11

#define      SUBLCD_DATE_WIDTH_F      7
#define      SUBLCD_DATE_WIDTH        9
#define      SUBLCD_WEEK_WIDTH        16
#define      SUBLCD_BR_WIDTH          5
#define      SUBLCD_SLASH_WIDTH       7
#define      SUBLCD_COLON_WIDTH       6
#define      SUBLCD_AP_WIDTH          13
#define      SUBLCD_WEEK_HIGH         16
#define      SUBLCD_POWER_ON_WIDTH      80
#define      SUBLCD_POWER_ON_HIGH 48


#define     SUBLCD_FONT_L_WIDTH_F     10
#define     SUBLCD_FONT_L_WIDTH       12
#define     SUBLCD_COLON_L_WIDTH      5
#define     SUBLCD_AP_L_WIDTH         13
#define     SUBLCD_FONT_L_HEIGHT      32

#define     SUBLCD_GAME_CARD_WIDTH    19
#define     SUBLCD_GAME_CARDBG_WIDTH  80
#define     SUBLCD_GAME_ARROW_WIDTH   80

#define     SUBLCD_GAME_CARD_HEIGHT   32
#define     SUBLCD_GAME_ARROW_HEIGHT  80

#define     SUBLCD_ANALOGCLOCK_WIDTH    80
#define     SUBLCD_ANALOGCLOCK_HEIGHT    80

#define      SUBLCD_COL_MAX         SUBLCD_X_SIZE/(BITS_EACH_BYTE)




typedef enum
{
    TXT_LINE_BASE,    
    TXT_LINE_1,
    TXT_LINE_2,
    TXT_LINE_3,

    TXT_LINE_MAX    
}SUBLCD_TXT_LINE_ID;

typedef struct
{
    /* Sublcd marque text length maximum 20 characters(unicode) */
    OP_UINT8 sublcd_marque_txt[SUBLCD_MARQUE_MAX_CHAR*2];
    OP_BOOLEAN  marque_onoff;
}SUBLCD_MARQUE_STRUCT_T;
/*==================================================================================================
    MACROS
==================================================================================================*/

/*==================================================================================================
    CONSTANTS
==================================================================================================*/

/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

/*==================================================================================================
    GLOBAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/

/*==================================================================================================
    GLOBAL FUNCTION
==================================================================================================*/
extern void sublcd_init 
(
    void
);

extern void sublcd_power_on
(
    void
);

extern void ds_sublcd_screen_buffer_clear 
(
    void
);
extern void ds_sublcd_graphic_screen_buffer_clear 
(
    void
);
extern void ds_sublcd_txt_screen_buffer_clear 
(
    void
);
extern void ds_sublcd_do_refresh 
( 
    void 
);
extern void ds_sublcd_image_out
( 
    OP_UINT8 x_size, 
    OP_UINT8 y_size, 
    OP_UINT8 x_offset, 
    OP_UINT8 y_offset, 
    const OP_UINT8 * image_ptr 
);
extern void ds_sublcd_annun_out
( 
    DS_ANNUN_ENUM_T index, 
    OP_BOOLEAN value 
);
extern void ds_sublcd_text_write
( 
    OP_UINT8 row_offset, 
    OP_UINT8 column_offset, 
    OP_UINT8 * text_ptr 
);

extern void ds_sublcd_text_write_line
(
    SUBLCD_TXT_LINE_ID    line_no,
    OP_UINT8    text_align_option,
    OP_UINT8  *text_data_ptr
);
extern void ds_sublcd_clear_text_line
(
    SUBLCD_TXT_LINE_ID line_no
);

extern void ds_sublcd_text_write_rm
( 
    SUBLCD_TXT_LINE_ID row_offset, 
    OP_UINT8 column_offset, 
    OP_UINT8 * text_ptr 
);
extern void ds_sublcd_text_format
( 
    OP_UINT8 * input_str, 
    OP_UINT8 * out_buffer 
);
extern void ds_sublcd_text_format_rm
( 
    RM_RESOURCE_ID_T    res_id, 
    OP_UINT8 * out_buffer 
);
extern void ds_sublcd_time_display
( 
    SP_SUBLCD_TIME_MODE_ENUM_T sublcd_time_mode, 
    OP_TIMESTAMP cur_time 
);
extern void ds_sublcd_contrast
(
    OP_UINT8    contrast_index
);

extern void ds_sublcd_text_write_marque
(
    SUBLCD_TXT_LINE_ID    marque_line_no,
    OP_UINT8    *text_data_ptr
);

extern void ds_sublcd_marque_text_move
(
   void
);
extern void ds_sublcd_marque_init
(
    void
);

extern void sublcd_welcome
(
    void
);

extern void sublcd_goodbye
(
    void
);

extern void ds_openwindow_int
(
    void
);

extern int ds_openwindow_refresh
(
    void   /* updated rectangle */
);


extern void draw_openwindow_annun
(
    DS_ANNUN_ENUM_T    annun,
    OP_BOOLEAN         onoff
);

extern int ds_openwindow_time_display
(
  void
);

/* the following functions donot used here, it's for display special icon */
extern int ds_openwindow_incoming_call_display
(
    OP_UINT8   *number
);
extern int ds_openwindow_missed_call_display
(
    OP_UINT8   missed_counts
);

extern int ds_openwindow_draw_text
(
    SUBLCD_DISPLAY_TEXT_STRUCTURE_NAME_T *txt_sublcd

);

/*================================================================================================*/
#endif  /* DS_SUBLCD_H */

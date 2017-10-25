#ifndef DS_LCD_DRIVER_H
#define DS_LCD_DRIVER_H
/*==================================================================================================

    HEADER NAME : ds_lcd_driver.h

    GENERAL DESCRIPTION
        This file contains lcd refresh functions and required definition,

    SEF Telecom Confidential Proprietary
    (c) Copyright 2003 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    01/12/2003   Denver Fu       crxxxxx     Initial Creation
    05/23/2003   Yuan Hui        P000406     get lcd parameter interface.
    07/18/2003   Yuan Hui        P000968     add function define.
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    This file contains lcd refresh functions and required definition,


====================================================================================================
    INCLUDE FILES
==================================================================================================*/

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
/* LCD Specific Constant */

#define LCD_HEIGHT                      220
#define LCD_WIDTH                       176

#define LCD_MAX_X_COOR                175
#define LCD_MAX_Y_COOR                219

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/
extern void lcd_drv_power_on
(
    void
);

extern void lcd_drv_power_off
(
    void
);
extern OP_BOOLEAN Lcd_get_poweron_status
(
    void
);
extern void lcd_drv_draw_rectangle
(
    OP_UINT8 col1, 
    OP_UINT8 row1, 
    OP_UINT8 col2, 
    OP_UINT8 row2, 
    void * color, 
    OP_UINT8 width, 
    OP_UINT8 start_row, 
    OP_UINT8 start_col
);

extern void lcd_drv_welcome
(
    void
);

extern OP_BOOLEAN lcd_drv_volume_set
(
    OP_UINT8 volume
);
extern OP_BOOLEAN Lcd_get_poweron_status
(
    void
);
/*================================================================================================*/
#endif  /* DS_LCD_DRIVER_H */


#ifndef DS_LCD_DRIVER_H
#define DS_LCD_DRIVER_H
/*==================================================================================================

    HEADER NAME : ds_lcd_simulator.h

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
	01/07/2003   Steven           P003225     adjust for dolphin menu         
    This file contains lcd refresh functions and required definition,


====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include "opus_typedef.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
/* LCD Specific Constant */

#define LCD_WIDTH                       176
#define LCD_HEIGHT                      220

#define LCD_MAX_X_COOR                   175
#define LCD_MAX_Y_COOR                   219

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/
extern void lcd_simulator_draw_rectangle
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
/*================================================================================================*/
#endif  /* DS_LCD_DRIVER_H */



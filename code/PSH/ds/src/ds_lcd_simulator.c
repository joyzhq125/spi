#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    HEADER NAME : ds_lcd_simulator.c

    GENERAL DESCRIPTION
        This file contains lcd refresh functions and required definition,

    SEF Telecom Confidential Proprietary
    (c) Copyright 2003 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    01/12/2003   Denver Fu        crxxxxx     Initial Creation
    06/19/2004   Jianghb          C006298     Add pclint tools to source code view

    This file contains lcd refresh functions and required definition,

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#ifndef _lint
#include    "windows.h"
#endif

#include    "portab_new.h"
#include    "lcd_api.h"

#include    "ds_lcd_simulator.h"
/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/*
  * None
  */
  
/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
/*
  * None
  */
  
/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
/*
  * None
  */
  
/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/*
  * None
  */
  
/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
/*
  * None
  */
  
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
static UINT8 lcd_simulator_screen_buf[LCD_HEIGHT*LCD_WIDTH*2];
const static LCD_BITMAP lcd_simulator_screen_object =
{
    lcd_simulator_screen_buf,
    LCD_HEIGHT,
    LCD_WIDTH,
    16
};
/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================
    FUNCTION: lcd_simulator_draw_rectangle

    DESCRIPTION:
        Draw rectangle on simulator

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
==================================================================================================*/
void lcd_simulator_draw_rectangle
(
    OP_UINT8 col1, 
    OP_UINT8 row1, 
    OP_UINT8 col2, 
    OP_UINT8 row2, 
    void * color, 
    OP_UINT8 width, 
    OP_UINT8 start_row, 
    OP_UINT8 start_col
)
{
    int                   x, y,k;
    UINT16            *screen_ptr;
    LCD_POINT      point;
    LCD_BOX         box;
    
    if (col1 > LCD_MAX_X_COOR) col1 = LCD_MAX_X_COOR;
    if (col2 > LCD_MAX_X_COOR) col2 = LCD_MAX_X_COOR;
    if(col1>col2)
    {
        col2 ^= col1; 
        col1 ^= col2;
        col2 ^= col1;
    }

    if(row1 > LCD_MAX_Y_COOR) row1 = LCD_MAX_Y_COOR;
    if(row2 > LCD_MAX_Y_COOR) row2 = LCD_MAX_Y_COOR;
    if(row1>row2)
    {
        row2 ^= row1; 
        row1 ^= row2;
        row2 ^= row1;
    }
    
    box.area.height = row2-row1+1;
    box.area.width  = col2-col1+1;
    box.origin.x    = col1;
    box.origin.y    = row1;
    point.x             = col1;
    point.y             = row1;

    for (y = row1; y <= row2; y++)
    {
        screen_ptr = (OP_UINT16*)color+ y*width+col1;
        for (x=col1; x<=col2; x++)	
        {
            k = (y*LCD_WIDTH+x)*2;
            lcd_simulator_screen_buf[k+0] = (UINT8)(*screen_ptr & 0x00ff) ;
            lcd_simulator_screen_buf[k+1] = (UINT8)((*screen_ptr & 0xff00) >> 8);
            screen_ptr ++;
        }
    }

    lcd_PutBitmapExt(lcd_simulator_screen_object,box,point,TRUE);
}
/*================================================================================================*/

#ifdef __cplusplus
}
#endif



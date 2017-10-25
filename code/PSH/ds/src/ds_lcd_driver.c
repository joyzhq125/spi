#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    HEADER NAME : ds_lcd_driver.c

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
    05/12/2003   Yuan Hui        P000255     avoid dim operation again.
    05/15/2003   linda wang       P000289    support 256 color, attention the opening image.
    05/23/2003   Yuan Hui         P000406    add fuction for get lcd parameter.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    03/11/2004   wangrui          P002618    modify sensor power on and lcdc to save power
    04/07/2004   wangrui          P002848     add a timer to exit camera when no operation and modify flashlight
    04/24/2004   linda wang       c005088     Add jpg idle image and power on/off animation.
    05/14/2004  hugh zhang        p005439      When do mission of umb, not sleep
    05/24/2004   penghaibo       p005629       fix jpg decode bug (add parameter to decode ds_* function)
    05/26/2004   wangrui       P005672     fix some bugs by changing the way of cotroling lcdc accoring to hardware
   This file contains lcd refresh functions and required definition,

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#ifdef WIN32
#include    "windows.h"
#include    "portab_new.h"
#include    "ds_lcd_simulator.h"
#else
#include    "HD66766.h"
#endif

#include    "rm_include.h"
#include    "SP_sysutils.h"
#include    "ds_util.h"
#include    "CAM_interface.h"
#include    "ds_drawing.h"
/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
/*
*avoid dim operation again.
*/
static OP_UINT8 FirstPowerOn = 0;
static void clc346reset_delay_50ms(void)
{
    int       j,  i=0;
    for(j=0;j<300;j++)  ///500 means 60ms
    {
    	for(i=0; i<128; i++);
		
    	
   }	
}


/*==================================================================================================
    FUNCTION: lcd_power_...

    DESCRIPTION:
        LCD power management.

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
void lcd_drv_power_on
(
    void
)
{
#ifdef WIN32
#else
    if(FirstPowerOn == 0)
    {

//        mmi_CLC346S_CTRL(TRUE);//to test
        mmi_CtrlCLCOSC(TRUE);  
        clc346reset_delay_50ms();
        hmi_clc346Sreset_high();
        hmi_clc346Sreset_low();
        clc346reset_delay_50ms();
        hmi_clc346Sreset_high();
        mmi_CLC346S_HOLD(TRUE);
        HD66766_power_on();    
        mmi_CLC346S_HOLD(FALSE);	  
        CLC346MainLCD_Initialize_HITACHA();
        mmi_CLC346S_HOLD(TRUE);
        FirstPowerOn =1;
    }
    else
    {
        return;
    }
#endif
}

void lcd_drv_power_off
(
    void
)
{
#ifdef WIN32
#else
    if(FirstPowerOn == 1)
    {
       if(SP_status_phone_can_deep_sleep())
        {
            HD66766_power_off();   
            mmi_CtrlCLCOSC(FALSE);
 //           mmi_CLC346S_CTRL(FALSE);/*TRUE*///to test
        FirstPowerOn =0;
    }
    }
    else
    {
        return;
    }
#endif
}
OP_BOOLEAN Lcd_get_poweron_status(void)
{
    if(FirstPowerOn == 0)
    {
       return OP_FALSE;/*main lcd already off.*/
    }
    else
    {
        return OP_TRUE;
    }
} 
/*==================================================================================================
    FUNCTION: lcd_draw_rectangle

    DESCRIPTION:
        Write data to LCD

    ARGUMENTS PASSED:

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void lcd_drv_draw_rectangle
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
#ifdef WIN32
    lcd_simulator_draw_rectangle
#else
    HD66766_high_speed_write_region 
#endif
                                    (col1, 
                                     row1, 
                                     col2, 
                                     row2,  
                                     color,
                                     width,
                                     start_row,
                                     start_col); 
}

/*==================================================================================================
    FUNCTION: lcd_welcome

    DESCRIPTION:
        Display welcome message on lcd. 

    ARGUMENTS PASSED:

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void lcd_drv_welcome(void)
{
    RM_IMAGE_T  *pImage = OP_NULL;
    
    pImage = util_get_image_from_res(JPEG_POWER_UP_IMAGE);
    if(pImage != OP_NULL)
    {
#ifdef WIN32
      { 
        RM_BITMAP_T destBmp;
        op_memset(&destBmp, 0, sizeof(RM_BITMAP_T));
        destBmp.data = (OP_UINT16 *)lcd_screen;
        ds_parse_image((RM_BITMAP_T *)&destBmp, (OP_UINT8 *)pImage->data, pImage->size, IMAGE_JPEG);
        lcd_drv_draw_rectangle(0, 0, LCD_MAX_X_COOR, LCD_MAX_Y_COOR, 
        	                   lcd_screen, LCD_MAX_X, 0, 0);
      }
#else
      {
        IMAGE_DISPLAY image;
          
        image.x_start = 0;
        image.y_start = 0;
        image.jpegdata = (OP_UINT16 *)pImage->data;
        image.filesize = pImage->size;
        image.imageresize = LEVEL_ONE;

        mmi_CLC346S_HOLD(OP_FALSE);
        Corelogic_ImageDisplay(&image);
        mmi_CLC346S_HOLD(OP_TRUE);
      }
#endif
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
OP_BOOLEAN lcd_drv_volume_set(OP_UINT8 volume)
{
#ifdef WIN32
#else
    HD66766_volume_set(volume);
#endif
    return OP_TRUE;
}
/*================================================================================================*/

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    MODULE NAME : HD66789.c

    GENERAL DESCRIPTION
        All the declarations and definitions necessary for the small size
        color graphic display device (LCD) HITACHI HD66789 driver to interface 
        with the part.  This file is specific to the HITACHI HD66789 part, 
        please refer to the data sheets when examinig this module.
    
    REFERENCES
        HITACHI HD66789 LCD Data Sheet

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------
    07/18/2002   Ashley Lee       crxxxxx     Initially brought from Janus_K
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    04/30/2003   YuanHui          P000204     when open the handset fist times,the Lcd don't display normally.
    06/07/2003   YuanHui          P000542     avalid a disorder of the lcd.delay poweron timer.
    07/07/2003   DenverFu         P000813     Enable LCD/SubLCD volume Adjust.
    12/08/2003   Yuan Hui         P002163     adjust the main Lcd to truly
    01/05/2004   WangRui          P002305     Modify Main LCD driver
    04/27/2004  wangrui          cr005161     modify movie capture and decode jpeg,add delay to advoid white screen.
    05/20/2004  wangrui            P005610      add delay time for white screen in some cases
    05/28/2004  JasonWang   p005758     modify lcd  setting
    06/12/2004  JasonWang   p006143     fix the bug when handle the picture too big error, the screen will go blank
    06/22/2004  JasonWang   p006360     fix two bugs in odin camera
    06/29/2004  JasonWang   p006475     modify lcd contrast and fix bug when charging, the preview stop sometimes
    07/12/2004  JasonWang   C006915     modify lcd refresh ways at camera preview mode
    07/12/2004  JasonWang   c006930         modify lcd contrast for odin and whale
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include "APP_include.h"
#include "HD66766.h"
#include  "CAMI_driv.h"
#include  "mydata_picture_operator.h"
/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/*
  *  None
  */

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
/*
  *  None
  */

/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
/*
  *  None
  */

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
/*
  *  None
  */

/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/*
*   Form WORD
*/
#define MAKEWORD(msb, lsb) ( ((OP_UINT16)((((OP_UINT16)msb) &0x00ff) <<8)) | ((OP_UINT16)(((OP_UINT16)lsb)&0x00ff))) ;

/*
*   I/O Address
*/
#define    LCD_IDX_BASE               0xC00000     /* RS Index Address */
#define    LCD_DAT_BASE               0xC00002    /* Data I/O Address  */

/* Instruction Resister Index */
#define    DP_START_OSC                	0x0000      /* R00h */
#define    DP_OUTPUT_CTRL       	0x0001      /* R01h */
#define    DP_WAVEFORM_CTRL  		0x0002      /* R02h */
#define    DP_ENTRY_MODE         	0x0003      /* R03h */
#define    DP_COMP_RESISTER1         	0x0004      /* R04h */
#define    DP_COMP_RESISTER2         	0x0005      /* R05h */
//#define    DP_COMP_RESISTER    	0x0006      /* R06h */setting disable
#define    DP_DISPLAY_CTRL1       	0x0007      /* R07h */
#define    DP_DISPLAY_CTRL2       	0x0008      /* R08h */
#define    DP_DISPLAY_CTRL3      	0x0009      /* R09h */
#define    DP_FRM_CYCLE_CTRL  		0x000B       /* R0Bh */
#define    DP_EXTE_DISPLAY_CTRL   	0x000C       /* R0Ch */
//#define    DP_PWR_CTRL_5         	0x000E       /* R0Eh */  //setting disabled
#define    DP_PWR_CTRL_1         	0x0010       /* R10h */
#define    DP_PWR_CTRL_2               	0x0011           /* R11h */
#define    DP_PWR_CTRL_3               	0x0012          /* R12h */
#define    DP_PWR_CTRL_4               	0x0013           /* R13h */
//#define    DP_RAM_WRITE_DATA_MASK 	0x0020 /* R20h *///setting disabled
#define    DP_RAM_ADDRESS_SET    	0x0021   /* R21h */
#define    DP_WRITE_READ_GRAM    	0x0022  /* R22h */
#define    DP_RAM_WRITE_DATA_MASK1 	0x0023 /* R23h */
#define    DP_RAM_WRITE_DATA_MASK2 	0x0024 /* R24h */
#define    DP_Y_Control1                            0x0030  /* R30h */
#define    DP_Y_Control2                            0x0031 /* R31h */
#define    DP_Y_Control3                            0x0032  /* R32h */
#define    DP_Y_Control4                            0x0033  /* R33h */
#define    DP_Y_Control5                            0x0034  /* R34h */
#define    DP_Y_Control6                            0x0035  /* R35h */
#define    DP_Y_Control7                            0x0036  /* R36h */
#define    DP_Y_Control8                            0x0037  /* R37h */
#define    DP_Y_Control9                            0x0038  /* R38h */
#define    DP_Y_Control10                            0x0039  /* R39h */
//#define    DP_Y_Control11                           0x003a  /* R3ah *///setting disabled
#define    DP_GATE_SCAN_POS                      	0x0040       /* R40h */
#define    DP_V_SCROLL                                   0x0041       /* R41h */
#define    DP_1ST_DRV_POS                              	0x0042           /* R42h */
#define    DP_2ND_DRV_POS                              0x0043          /* R43h */
#define    DP_H_RAM_ADDR_POS                         	0x0044          /* R44h */
#define    DP_V_RAM_ADDR_POS                           0x0045           /* R45h */
/* LCD Size */
#define HD66766_START_X_ADDR            0x00
#define HD66766_START_Y_ADDR            0x00     
#define HD66766_END_X_ADDR            0xAf /* 175 */
#define HD66766_END_Y_ADDR            0xDb /* 219 */  //wandy

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
/*
  *  None
  */

/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
/*
  *  None
  */

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
static void HD66766_delay_1ms(void)
{
    int         i;
    for(i=0; i<14; i++);
}

static void HD66766_delay_10ms(void)
{
    int         i;
    for(i=0; i<128; i++);
}

static void HD66766_lcd_delay(unsigned int ms )
{
    unsigned int    i,n;
    
    n = ms/10;
    for( i = 0; i < n; i++ )
    {
        HD66766_delay_10ms();
    }

    n = ms%10;
    for( i = 0; i < n; i++ )
    {
        HD66766_delay_1ms();
    }
}

#define HD66766_index(index) *((volatile OP_UINT16 *)LCD_IDX_BASE)=(OP_UINT16)index;

#define HD66766_data(code)    *((volatile OP_UINT16 *)LCD_DAT_BASE)=(OP_UINT16)code;

static void HD66766_h_ram_addr_pos
(
    OP_UINT8 hea, 
    OP_UINT8 hsa
)
{
    OP_UINT16           word;
    
    HD66766_index(DP_H_RAM_ADDR_POS);   /* R16h. Horizontal RAM address position */
    word =MAKEWORD(hea, hsa);
    HD66766_data(word);
}

static void HD66766_v_ram_addr_pos
(
    OP_UINT8 vea, 
    OP_UINT8 vsa
)
{
    OP_UINT16           word;
    
    word =MAKEWORD(vea, vsa);
    HD66766_index(DP_V_RAM_ADDR_POS);   /* R16h. Horizontal RAM address position */
    HD66766_data(word);
}

/*==================================================================================================
    FUNCTION: HD66766_ram_address_set 

    DESCRIPTION:

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static void HD66766_ram_address_set 
(
    OP_UINT8 row, 
    OP_UINT8 column
)
{   
    OP_UINT16           word;
    
    word =MAKEWORD(row, column);
    HD66766_index (DP_RAM_ADDRESS_SET);
    HD66766_data(word);
}

/*==================================================================================================
    FUNCTION: HD66766_palette_init 

    DESCRIPTION:

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
static void HD66766_palette_init
(
    void
)
{
        HD66766_index(DP_Y_Control1);
   HD66766_data(0x0000);

    HD66766_index(DP_Y_Control2);
   HD66766_data(0x0000);
    
    HD66766_index(DP_Y_Control3);
   HD66766_data(0x0000);
    
    HD66766_index(DP_Y_Control4);
   HD66766_data(0x0103);
    
    HD66766_index(DP_Y_Control5);
   HD66766_data(0x0000);
    
    HD66766_index(DP_Y_Control6);
   HD66766_data(0x0000);
    
    HD66766_index(DP_Y_Control7);
  /*  HD66766_data(0x1c0e);*/
   HD66766_data(0x0000);
    HD66766_index(DP_Y_Control8);
   HD66766_data(0x0401);
    
    HD66766_index(DP_Y_Control9);
   HD66766_data(0x0009);
 
    HD66766_index(DP_Y_Control10);
   HD66766_data(0x0009);
}

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================
    FUNCTION: HD66766_high_speed_write_region 

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void HD66766_high_speed_write_region
(
    OP_UINT8    col1, 
    OP_UINT8    row1,
    OP_UINT8    col2, 
    OP_UINT8    row2, 
    void    *color, 
    OP_UINT8    width, 
    OP_UINT8    start_row, 
    OP_UINT8    start_col
) 
{
    int x, y;
    OP_UINT16  *screen_ptr;

    if (col1 > HD66766_END_X_ADDR) col1 = HD66766_END_X_ADDR;
    if (col2 > HD66766_END_X_ADDR) col2 = HD66766_END_X_ADDR;
    if(col1>col2)
    {
        col2 ^= col1; 
        col1 ^= col2;
        col2 ^= col1;
    }

    if(row1 > HD66766_END_Y_ADDR) row1 = HD66766_END_Y_ADDR;
    if(row2 > HD66766_END_Y_ADDR) row2 = HD66766_END_Y_ADDR;
    if(row1>row2)
    {
        row2 ^= row1; 
        row1 ^= row2;
        row2 ^= row1;
    }

    
    col1 -= (col1 % 4);
    col2 += (3 - col2 % 4);

    if(SP_status_get_cannot_sleep_maskbit(CANNOT_SLEEP_CAM)||Get_hardware_plot())
    {
        mmi_CLC346S_HOLD(FALSE);
        MainLcdLock(OP_TRUE);        
    }
    
    HD66766_h_ram_addr_pos(col2, col1);
    HD66766_v_ram_addr_pos(row2, row1);
    HD66766_ram_address_set(row1, col1);
    HD66766_index( DP_ENTRY_MODE );   
    HD66766_data(0x7230);
   HD66766_index(DP_WRITE_READ_GRAM);

    for (y = row1; y <= row2; y++)
    {
        screen_ptr = (OP_UINT16*)color+ y*width+col1;
        for (x=col1; x<=col2; x+=4)
        {
            HD66766_data(*screen_ptr);
            ++screen_ptr;
            HD66766_data(*screen_ptr);
            ++screen_ptr;
            HD66766_data(*screen_ptr);
            ++screen_ptr;
            HD66766_data(*screen_ptr);
            ++screen_ptr;
        }
    }
   
    if(SP_status_get_cannot_sleep_maskbit(CANNOT_SLEEP_CAM)||Get_hardware_plot())
    {
        MainLcdLock(OP_FALSE);
    }
    
}

/*==================================================================================================
    FUNCTION: HD66766_power_on 

    DESCRIPTION:
        Initialize HD66766 LCD.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void HD66766_power_on
(
    void
)
{ 


    op_debug(DEBUG_MED, "LCD Initialization start");


/*/////////////////////////////////////////////////////////////////////////
// 1. R00 start oscillation Start
/////////////////////////////////////////////////////////////////////////
*/
    HD66766_index(DP_START_OSC);             
    HD66766_data(0x0001);
    HD66766_lcd_delay(50);   /*add delay,for power vailid support.50ms*/
/*/////////////////////////////////////////////////////////////////////////
// 2. R01h, driver output control
// SM=0(non-Interlace), GS=0(G1->G224), SS=1(S528->S1), NL4-0:11011(224 Lines)
//EPL=0(Enable low valid),DPL=0(data read in synchronization with the rising edge of DOTCLK)
//VSPL=0(VSYNC Low active),HSPL=0(HSYNC low active)-----according RGB Interface
/////////////////////////////////////////////////////////////////////////
*/
       HD66766_index(0x10);
	HD66766_data(0x0000);
	HD66766_index(0x11);
	HD66766_data(0x0000);
       HD66766_lcd_delay(200);
	HD66766_index(0x12);
	HD66766_data(0x0000); 
	HD66766_index(0x13);
	HD66766_data(0x0000);
       HD66766_lcd_delay(200);
       
       HD66766_index(0x11);
	HD66766_data(0x0000);
	HD66766_index(0x13);
	HD66766_data(0x1417);
       HD66766_lcd_delay(200);
       HD66766_lcd_delay(100);/*debug*/
	HD66766_index(0x12);
	HD66766_data(0x0006);
       HD66766_lcd_delay(100);/*debug*/
	HD66766_index(0x10);
	HD66766_data(0x0004);
       HD66766_lcd_delay(100); /*debug*/
	HD66766_index(0x10);
	HD66766_data(0x0004);
	
	HD66766_index(0x11);
	HD66766_data(0x0004);
	HD66766_lcd_delay(100);/*debug*/
	HD66766_index(0x10);
	HD66766_data(0x0044);
	HD66766_index(0x12);
	HD66766_data(0x0016);
	
	HD66766_lcd_delay(200);   /*add delay,for power vailid support.50ms*/
	HD66766_index(0x10);
	HD66766_data(0x0140);
       HD66766_lcd_delay(100);   /*debug*/
	HD66766_index(0x13);
	HD66766_data(0x3417);
	HD66766_index(0x10);
	HD66766_data(0x0140);
	HD66766_lcd_delay(100);   /*debug*/
    
	HD66766_index(0x01);
	HD66766_data(0x011b);
	HD66766_index(0x02);
	HD66766_data(0x0700);
	HD66766_index(0x03);
	HD66766_data(0x7030);
	
	HD66766_index(0x04);
	HD66766_data(0x0000);
	
	HD66766_index(0x05);
	HD66766_data(0x0000);
	
	HD66766_index(0x08);
       HD66766_data(0x0706);
	HD66766_index(0x09);
	HD66766_data(0x0000);
	
	 HD66766_index(0x0b);
	HD66766_data(0x5006);
	 HD66766_index(0x0c);
	HD66766_data(0x0000);
    
	HD66766_palette_init();
	 HD66766_index(0x23);
	HD66766_data(0x0000);
	 HD66766_index(0x24);
	HD66766_data(0x0000);
	 HD66766_index(0x21);
	HD66766_data(0x0000);
	
	/////////R40 R41 R42 R43  R44   R45
	
	HD66766_index(DP_GATE_SCAN_POS);
	HD66766_data(0x0000);
	
	HD66766_index(DP_V_SCROLL);
	HD66766_data(0x0000);//provided 0x00DB
	HD66766_index(DP_1ST_DRV_POS);//maybe 0xDB00
	HD66766_data(0xE100);
	HD66766_lcd_delay(50);   /*add delay,for power vailid support.50ms*/
	HD66766_index(DP_2ND_DRV_POS);
	HD66766_data(0xFF00);     
	HD66766_lcd_delay(50);   /*add delay,for power vailid support.50ms*/
	HD66766_index(DP_H_RAM_ADDR_POS);
	HD66766_data(0xAF00);
	HD66766_index(DP_V_RAM_ADDR_POS);
	HD66766_data(0xDF00);
	HD66766_lcd_delay(500);   /*add delay,for power vailid support.50ms*/

	HD66766_index(0x10);
	HD66766_data(0x4140);
	HD66766_index(DP_DISPLAY_CTRL1);
	HD66766_data(0x0005);//provided 0x0137
	HD66766_lcd_delay(500);
    
	HD66766_index(DP_DISPLAY_CTRL1);
	HD66766_data(0x0025);
	HD66766_index(DP_DISPLAY_CTRL1);
	HD66766_data(0x0027);
	HD66766_lcd_delay(500);

	HD66766_index(DP_DISPLAY_CTRL1);
	HD66766_data(0x0037);

    	HD66766_index(DP_DISPLAY_CTRL1);
	HD66766_data(0x0137);
 	HD66766_lcd_delay(500);   

}

/*==================================================================================================
    FUNCTION: HD66766_POWER_OFF 

    DESCRIPTION:
        HD66766   LCD POWER OFF Sequence 

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void HD66766_power_off
(
    void
)
{
    HD66766_lcd_delay(10);
      
    /* display off sequence */
    HD66766_Display_on(OP_FALSE);
    HD66766_lcd_delay(10);
  
    /*power off lcd register  SAP2-0 =000, AP2-0=000*/
    HD66766_index( DP_PWR_CTRL_1 );  
    HD66766_data( 0x0400 );
    HD66766_lcd_delay(10);	
   /*power off lcd register  PON =0, VCOMG=0*/
    HD66766_index( DP_PWR_CTRL_3 );  
    HD66766_data( 0x0008 );
    HD66766_lcd_delay(10);	
/*power off lcd register  PON =0, VCOMG=0*/	
   HD66766_index( DP_PWR_CTRL_4 );  
   HD66766_data( 0x1519 );
   HD66766_lcd_delay(100);
}


/*==================================================================================================
    FUNCTION: HD66766_Display_on 

    DESCRIPTION:
            Parameter:  (TRUE=ON, FALSE=OFF)
                           OP_TRUE : display on sequence
                           OP_FALSE: display off sequence
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void HD66766_Display_on
(
   OP_BOOLEAN bOn
)
{
    if (bOn == OP_FALSE)
    {
        HD66766_index(DP_FRM_CYCLE_CTRL);
        /*NO1,0: 00   EQ=0; */
        HD66766_data(0x0000);
    }

    	HD66766_index(DP_DISPLAY_CTRL1);
    if (bOn == OP_TRUE)
    {
    //    HD66766_data(0x0005);
    HD66766_data(0x0105);
    }
    else
    {
    /*GON=1,DTE=1,CL=0,REV=1,D1=1,D0=0*/
    //    HD66766_data(0x0036);
   HD66766_data(0x0136);

    }
    /*wait more than 2 frame*/
    HD66766_lcd_delay(1);

    HD66766_index(DP_DISPLAY_CTRL1);
    if (bOn == OP_TRUE)
    {
        HD66766_data(0x0125);
    }
    else
    {
    /*GON=1,DTE=0,CL=0,REV=1,D1=1,D0=0*/
        HD66766_data(0x0126);
        HD66766_lcd_delay(1);
    }
   
    HD66766_index(DP_DISPLAY_CTRL1);
    if (bOn == OP_TRUE)
    {
        HD66766_data(0x0127);
    }
    else
    {
    /*GON=0,DTE=0,CL=0,REV=1,D1=0,D0=0*/
        HD66766_data(0x0104);
    }
    HD66766_lcd_delay(1);

    if (bOn == OP_TRUE)
    {
        HD66766_index(DP_DISPLAY_CTRL1);
        HD66766_data(0x0137);
    }
if (bOn == OP_FALSE)	
   {
    /*power off lcd register  SAP2-0 =000, AP2-0=000*/
    HD66766_index( DP_PWR_CTRL_1 );  ///it is nessesary to modified wandy
    HD66766_data( 0x0400 );
   HD66766_lcd_delay(10);	
   /*power off lcd register  PON =0, VCOMG=0*/
    HD66766_index( DP_PWR_CTRL_3 );  ///it is nessesary to modified wandy
    HD66766_data( 0x0008 );
   HD66766_lcd_delay(10);	
/*power off lcd register  PON =0, VCOMG=0*/	
   HD66766_index( DP_PWR_CTRL_4 );  ///it is nessesary to modified wandy
    HD66766_data( 0x1519 );
    HD66766_lcd_delay(100);	
    }
}

/*==================================================================================================
    FUNCTION: HD66766_standby_on 

    DESCRIPTION:
         HD66766 LCDÀÇ Standby On/Off ¼³Á¤      (TRUE=ON, FALSE=OFF)

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void HD66766_standby_on
(
    OP_BOOLEAN bOn
)
{
    if(bOn == OP_TRUE)
    {
        HD66766_Display_on(OP_FALSE);
        
        HD66766_index(DP_PWR_CTRL_1);
        HD66766_data( 0x0401 );
    }
    else
    {
        HD66766_index(DP_START_OSC);             
        HD66766_data(0x0001);
	HD66766_lcd_delay(50);	
      	HD66766_index(DP_PWR_CTRL_1);
        HD66766_data( 0x0400 );
        HD66766_Display_on(OP_TRUE);
    }
}

/*==================================================================================================
    FUNCTION: HD66766_Sleep_on 

    DESCRIPTION:
         HD66766 LCD Sleep On/Off       (TRUE=ON, FALSE=OFF)

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void HD66766_Sleep_on
(
    OP_BOOLEAN bOn
)
{
    if(bOn == OP_TRUE)
    {
        HD66766_Display_on(bOn);
        
        HD66766_index(DP_PWR_CTRL_1);
        HD66766_data( 0x0402 );
    }
    else
    {
        HD66766_index(DP_PWR_CTRL_1);
        HD66766_data(0x0400);

        HD66766_Display_on(OP_TRUE);
    }
}

/*==================================================================================================
    FUNCTION: HD66766_volume_set 

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void HD66766_volume_set
(
    OP_UINT8 volume
)
{
    OP_UINT16        contranst;
    if((volume>=1) && (volume<=6) )
    {
        HD66766_index(DP_PWR_CTRL_2);
        contranst = volume;
        HD66766_data(contranst);   
    }
}
/*==================================================================================================
    FUNCTION: CLC346MainLCD_Initialize_HITACHA 

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void	CLC346MainLCD_Initialize_HITACHA(void)
{

	CamMainLCDType1(0x3511);  //provide 0x3011
	CamMainLCDType2(0x0000); 	
	CamMainLCDWindowControl(0x080d);  
	CamMainLCDWindowXAddress(0x0044);
	CamMainLCDWindowYAddress(0x0045);
	CamMainLCDGRAMAccessControl(0x0389);
	CamMainLCDXYAddressCommand(0x0021);
	CamMainLCDXYAddressParameter(0x0000);
	CamMainLCDGRAMRWCommand(0x2222);

        CamUpperVideoBufferAddress(0x0003);
        CamLowerVideoBufferAddress(0x6800);
        CamUpperOSDAddress(0x0002);
        CamLowerOSDAddress(0xef00);
        CamUpperPIPAddress(0x0002);
        CamLowerPIPAddress(0xef00);

    CamSensorPowerSupply(0x00);
    CamInputPixelRatio(0x01);
    CamVsyncShapeRegister(0x00,0x00);
    CamInputClockControl(0x02,0x00);
    CamIICClockDirectionRegister(0x80);
    CamInterrupt_ResetMode(0x01,0x00,0x04,0x00,0x00,0x00);
    
}





/*================================================================================================*/
#ifdef __cplusplus
}
#endif


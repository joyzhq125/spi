/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      S e i k o     S - 4 4 0 0 0 A 0 1    L C D     D r i v e r

GENERAL DESCRIPTION

REFERENCES
  Seiko S-44000A01 LCD Data Sheet. Rev1.1

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when         who         what, where, why
----------   -------    -----------------------------------------------------
08/15/2002   ps         Initialization
    03/26/2003   lindawang       C000018     Set new pacific image for the first version.
    04/08/2003   lindawang       P000049     adjust the sublcd orientation.
    05/15/2003   Yuan Hui         P000288     modify subLcd power off and power on interface.
    05/23/2003   Yuan Hui         P000433     add power down interface.
    07/07/2003   DenverFu        P000813     Enable LCD/SubLCD volume Adjust.
    07/07/2003   DenverFu        P000813     Correct SubLCD volume Adjust in lcd_init,
                                                                Adjust SubLCD contrast center value.
    07/18/2003   Yuan Hui         P000986     sublcd turn into black after mast clear.
    08/06/2003   Yuan Hui         P001163     sublcd power on for supply power.
    08/16/2003   Yuan Hui         P001163     sublcd power on for supply power.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "s44000a01.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         SCREEN BUFFER                                  */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*==================================================================================================
    CONSTNAT
==================================================================================================*/
#define LCD_CS_BASE                               0x00800000   
#define S44000A01_COM_BASE            (LCD_CS_BASE + 0x000)
#define S44000A01_DAT_BASE            (LCD_CS_BASE + 0x002)

#define S44000A01_X_MAR                           0x1A  

#define SUBLCD_CONTRAST_CENTER     0x13

const OP_UINT8   SUBLCD_CONTRAST_VALUES [ SUBLCD_CONTRAST_MAX_INDEX ] =
{
    SUBLCD_CONTRAST_CENTER-18,
    SUBLCD_CONTRAST_CENTER-16,
    SUBLCD_CONTRAST_CENTER-14,
    SUBLCD_CONTRAST_CENTER-12,
    SUBLCD_CONTRAST_CENTER-9,
    SUBLCD_CONTRAST_CENTER-6,
    SUBLCD_CONTRAST_CENTER-3,
    SUBLCD_CONTRAST_CENTER,
    SUBLCD_CONTRAST_CENTER+4,
    SUBLCD_CONTRAST_CENTER+9,
    SUBLCD_CONTRAST_CENTER+14,
    SUBLCD_CONTRAST_CENTER+19,
    SUBLCD_CONTRAST_CENTER+22,
    SUBLCD_CONTRAST_CENTER+25,
    SUBLCD_CONTRAST_CENTER+28,
    SUBLCD_CONTRAST_CENTER+31
};

/*==================================================================================================
    MACRO
==================================================================================================*/
#define inp(port)        (*((volatile OP_UINT8 *) (port)))
#define outp(port, val)  (*((volatile OP_UINT8 *) (port)) = ((OP_UINT8) (val)))


#define S44000A01_COM_SET(command)    outp( S44000A01_COM_BASE, command )
#define S44000A01_DAT_SET(data)        outp( S44000A01_DAT_BASE, data )

#define S44000A01_COM_GET()        inp( S44000A01_COM_BASE )
#define S44000A01_DAT_GET()        inp( S44000A01_DAT_BASE )

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
static OP_BOOLEAN FirstPowerOn_sLcd = OP_FALSE;

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
/*===========================================================================

FUNCTION S44000A01_sublcd_delays

DESCRIPTION  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void S44000A01_delay_1ms(void)
{
    int         i;
    for(i=0; i<14; i++);
}

void S44000A01_delay_10ms(void)
{
    int         i;
    for(i=0; i<128; i++);
}

void S44000A01_delay(unsigned int ms )
{
    unsigned int    i,n;
    
    n = ms/10;
    for( i = 0; i < n; i++ )
    {
        S44000A01_delay_10ms();
    }

    n = ms%10;
    for( i = 0; i < n; i++ )
    {
        S44000A01_delay_1ms();
    }
}

/*===========================================================================

FUNCTION rev_bits

DESCRIPTION
  reverse bit

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
OP_UINT8 rev_bits(OP_UINT8 value)
{
    OP_UINT8         i, j, ret_val=0;
   
    for ( i = 0 ; i < 8 ; i++ )
    {
        ret_val = ret_val << 1;
        j = value & 0x01;
        value = value >> 1 ;
        ret_val = ret_val | j ;
    }
    return (ret_val);
}

/*===========================================================================

FUNCTION s44000a01_wait

DESCRIPTION
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void s44000a01_wait( void )
{
    OP_UINT8 status;
    int i=0;  
    do
    {
        status = S44000A01_COM_GET();
        status &= 0x80;
        i++;
        if( i > 50 )
        {
            break;
        }
    }while( status != 0x00 );
}

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
/*===========================================================================

FUNCTION s44000a01_set_pos

DESCRIPTION
  set the page and x on S44000A01 LCD 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void s44000a01_set_pos
( 
    OP_UINT8 iPage, 
    OP_UINT8 iCol
)
{
    OP_UINT8        Cmd_Page;
    OP_UINT8        Cmd_mCol;
    OP_UINT8        Cmd_lCol;
    
/*----------------------------------------------------------------
Check and correct parameters
----------------------------------------------------------------*/
    if( iPage >= S44000A01_MAX_PAGE || iCol >= S44000A01_X_SIZE )
    {
        return;
    }

/*----------------------------------------------------------------
Form command
----------------------------------------------------------------*/
    Cmd_Page = 0xB0 | ( iPage & 0x0f );

    iCol            += S44000A01_X_MAR;
    Cmd_lCol    =  iCol & 0x0f ;
    Cmd_mCol  = ( iCol & 0xf0 )>>4;
    Cmd_mCol  = Cmd_mCol |0x10;

/*----------------------------------------------------------------
Send start column command
----------------------------------------------------------------*/
  S44000A01_COM_SET( Cmd_mCol );     /* Set Column (X-add) */    
  S44000A01_delay(6);
  S44000A01_COM_SET( Cmd_lCol );       /* Set Column (X-add) */    
  S44000A01_delay(6);
 
/*----------------------------------------------------------------
Send start page command
----------------------------------------------------------------*/
  S44000A01_COM_SET( Cmd_Page);  /* Set Page (Y-add)  */    
  S44000A01_delay(6);

}

/*===========================================================================

FUNCTION s44000a01_write_data

DESCRIPTION
  set the page and x on S44000A01 LCD 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void s44000a01_write_data
( 
    OP_UINT8 iCol,
    OP_UINT8 nCols,
    OP_UINT8 *pData
)
{
    OP_UINT8        i,j;
    OP_UINT8        outdata, checker,maker;

/*----------------------------------------------------------------
Check and correct parameters
----------------------------------------------------------------*/
    if(iCol >= S44000A01_X_SIZE )
    {
        return;
    }
    if(nCols > S44000A01_X_SIZE)
    {
        return;
    }
    if(pData == OP_NULL)
    {
        return;
    }
    if(iCol + nCols > S44000A01_X_SIZE)
    {
        nCols = S44000A01_X_SIZE - iCol;
    }

/*----------------------------------------------------------------
Convert and Write display data to LCD
----------------------------------------------------------------*/
    for(i=0; i<S44000A01_X_SIZE; i++)
    {
        checker = 0x01;
        outdata = 0;
        maker = 0x80;
        for(j=0; j<8; j++)
        {
            if((pData[i]&checker) != 0)
            {
                outdata |= maker;
            }
            checker <<= 1;
            maker >>= 1;
        }
        S44000A01_DAT_SET( outdata ); /* Write data*/
    }
    S44000A01_delay(6);
}

/*===========================================================================

FUNCTION s44000a01_write

DESCRIPTION
  set the page and x on S44000A01 LCD 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void s44000a01_write
( 
    OP_UINT8 iPage, 
    OP_UINT8 iCol,
    OP_UINT8 nCols,
    OP_UINT8 *pData
)
{
/*----------------------------------------------------------------
Check and correct parameters
----------------------------------------------------------------*/
    if( iPage >= S44000A01_MAX_PAGE || iCol >= S44000A01_X_SIZE )
    {
        return;
    }
    if(nCols > S44000A01_X_SIZE)
    {
        return;
    }
    if(pData == OP_NULL)
    {
        return;
    }
    if(iCol + nCols > S44000A01_X_SIZE)
    {
        nCols = S44000A01_X_SIZE - iCol;
    }

/*----------------------------------------------------------------
Set start page and column
----------------------------------------------------------------*/
    s44000a01_set_pos(iPage, iCol);

/*----------------------------------------------------------------
Convert and Write display data to LCD
----------------------------------------------------------------*/
    s44000a01_write_data(iCol, nCols, pData);
}

/*===========================================================================

FUNCTION s44000a01_lcd_init

DESCRIPTION
  Initialize S44000A01 LCD  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void s44000a01_lcd_init( void )
{
//----------------------------------------------------------------
// Step 1 : Reset Command
//----------------------------------------------------------------
    S44000A01_COM_SET( 0xE2 ); // Reset command
    S44000A01_delay(6);
    S44000A01_delay(50);
//----------------------------------------------------------------
// Step 2 : wait time 10msec
//----------------------------------------------------------------
 //   S44000A01_sublcd_delay();
//----------------------------------------------------------------
// Step 3 : ADC select  (0xA0: normal, 0xA1: reverse)
//----------------------------------------------------------------
    S44000A01_COM_SET( 0xA0 ); 
    S44000A01_delay(10);
//----------------------------------------------------------------
// Step 4 : SHL select  (0xD0: COM0->COM63; 0xD8: COM63->COM0)
//----------------------------------------------------------------
    S44000A01_COM_SET( 0xC8 ); 

//----------------------------------------------------------------
// Step 5 :  LCD Bias Setting
//----------------------------------------------------------------
    S44000A01_COM_SET( 0xA3 );  
    S44000A01_delay(10);
//----------------------------------------------------------------
// Step 6 : power control (0010 1Vc Vr Vf) Vc On
//----------------------------------------------------------------
    S44000A01_COM_SET( 0x2C );  
    S44000A01_delay(6);
    S44000A01_delay(100);
//----------------------------------------------------------------
// Step 7 : power control (0010 1Vc Vr Vf) Vr On
//----------------------------------------------------------------
    S44000A01_COM_SET( 0x2E );  
    S44000A01_delay(6);
    S44000A01_delay(100);
//----------------------------------------------------------------
// Step 8 : power control (0010 1Vc Vr Vf) Vf On
//----------------------------------------------------------------
    S44000A01_COM_SET( 0x2F );  
    S44000A01_delay(40);
        S44000A01_delay(200);
//----------------------------------------------------------------
// Step 7 : Regulator resistor select (0010 0 R2 R1 R0) default :100
//----------------------------------------------------------------
    S44000A01_COM_SET( 0x24 );       

//----------------------------------------------------------------
// Step 8 : reference voltage mode select, then setting value: default 32
//----------------------------------------------------------------
    S44000A01_COM_SET( 0x81 );
    S44000A01_COM_SET( SUBLCD_CONTRAST_CENTER);
    S44000A01_delay(6);
    S44000A01_delay(50);

    s44000a01_clear_screen();

//Display ON/OFF
    S44000A01_COM_SET( 0xAF ); // display normal, 80 line

    /*set power on flag.*/
    FirstPowerOn_sLcd = OP_TRUE;

} /* end of 's44000a01_lcd_init()' */  

/*===========================================================================

FUNCTION s44000a01_clear_screen

DESCRIPTION
  Clear S44000A01 LCD screen

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void s44000a01_clear_screen( void )
{
#ifndef WIN32
    OP_UINT8 page, x;
    
    for( page = 0; page < S44000A01_MAX_PAGE; page++ )
    {
        s44000a01_set_pos(page,0);
        for( x = 0 ; x < S44000A01_X_SIZE; x++ )
        {
            S44000A01_DAT_SET(0x00);            
        }
    }
#endif
}
/*===========================================================================

FUNCTION s44000a01_power_on

DESCRIPTION
  turn on sub LCD, enter display mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/

void s44000a01_power_on(void)
{
#ifndef WIN32
    if( FirstPowerOn_sLcd ==OP_FALSE)
        {
                // Step 1 : static indicator on
              S44000A01_COM_SET( 0xA4 ); 
              S44000A01_delay(10);
            // Step 2 : display off
              S44000A01_COM_SET( 0xAD ); 
              S44000A01_COM_SET( 0x03 ); 
              S44000A01_delay(10);
            // should delay >= 50ms
            // Step 3 :  entire display on
              S44000A01_COM_SET( 0xAF ); 
            //delay more than 1ms
              S44000A01_delay(10);
            /*set power on flag.*/
              FirstPowerOn_sLcd = OP_TRUE;
            return;
        }
      else
        return;
#endif
}
/*===========================================================================

FUNCTION s44000a01_power_off

DESCRIPTION
  turn off sub LCD, enter Sleep mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
void s44000a01_power_off( void )
{
#ifndef WIN32
    if(FirstPowerOn_sLcd  == OP_TRUE)
    {
        // Step 1 : static indicator off
          S44000A01_COM_SET( 0xAC ); 
          S44000A01_delay(10);
        // Step 2 : display off
          S44000A01_COM_SET( 0xAE ); 
          S44000A01_delay(10);
        // should delay >= 50ms
        // Step 3 :  entire display on
          S44000A01_COM_SET( 0xA5 ); 
        //delay more than 1ms
          S44000A01_delay(10);
        /*clear flag.*/
         FirstPowerOn_sLcd = OP_FALSE;
        return;
    }
    else
    {
        return;
    }
#endif
}

/*===========================================================================

FUNCTION GetPowerSLcd

DESCRIPTION
  S44000A01 sub LCD enter sleep mode,get flag .  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
OP_BOOLEAN GetPowerSLcd( void )
{
    if(FirstPowerOn_sLcd== OP_TRUE)
        {
            return OP_TRUE;
        }
    else
        {
            return OP_FALSE;
        }
}
void s44000a01_power_down( void )
{
// Step 1 :  Display Off
  S44000A01_COM_SET( 0xA0 ); 
/*
 *control power circuit operation.
*/
// Step 2 :  Power Control : VR off
  S44000A01_COM_SET( 0x2D ); 
// should delay >= 50ms
  S44000A01_delay(50);

// Step 3 :  Power control : Vf off
  S44000A01_COM_SET( 0x2c ); 
//delay more than 1ms
  S44000A01_delay(50);

// Step 4: Power control : Vc off
  S44000A01_COM_SET( 0x28 ); 
//delay more than 1ms
  S44000A01_delay(50);

// Power down /turn off the sublcd.  
}
void s44000a01_volume_set( OP_UINT8 val )
{
  
#ifndef WIN32  
/* command 0x81; */
/* dat : SV5-0: default 0x20; */
    S44000A01_COM_SET( 0x81 );  
    S44000A01_COM_SET( val & 0x3F );  

#endif
  
}


/* end of file */

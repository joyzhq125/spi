#ifndef S44000A01_H
#define S44000A01_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      S e i k o     S - 4 4 0 0 0 A 0 1    L C D     D r i v e r

GENERAL DESCRIPTION

REFERENCES
  Seiko S-44000A01 LCD Data Sheet. Rev1.1

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

when         who         what, where, why
----------   -------    ---------------------------------------------_-------
    08/15/2002   ps         Initialization
    03/26/2003   lindawang      C000018     Set new pacific image for the first version.
    03-04-04   Chenyu           PR    ----      P000041 
    05/15/2003   Yuan Hui         P000288     modify subLcd power off and power on interface.
                                  - Fix sublcd display 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include    "OPUS_typedef.h"
#include    "rm_typedef.h"
#include    "rm_sublcd_image.h"
#include    "ds_util.h"
#include    "sp_include.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
 Define for S44000A01 LCD
*/
#define S44000A01_X_SIZE                0x50 //80 dots
#define S44000A01_Y_SIZE                0x38 //50 dots
#define S44000A01_MAX_PAGE            0x07 //max number of pages is 10 (0 ~ 9)

/* Icon
*/
#define S44000A01_ICON_MAX_PAGE            0x02

#define SUBLCD_CONTRAST_NUM_INDEX  16
#define SUBLCD_CONTRAST_MAX_INDEX  SUBLCD_CONTRAST_NUM_INDEX
extern const OP_UINT8 SUBLCD_CONTRAST_VALUES[];


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      FUNCTION DECLARATIONS                               */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

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
extern void s44000a01_write
( 
    OP_UINT8 iPage, 
    OP_UINT8 iCol,
    OP_UINT8 nCols,
    OP_UINT8 *pData
);

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
extern void s44000a01_lcd_init
( 
  void 
);

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
extern void s44000a01_clear_screen
( 
  void 
);
/*===========================================================================

FUNCTION s44000a01_power_on

DESCRIPTION
  make S44000A01 sub LCD enter display mode.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
extern void s44000a01_power_on
(
    void
);
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
extern OP_BOOLEAN GetPowerSLcd
(
    void
);
/*===========================================================================

FUNCTION s44000a01_power_off

DESCRIPTION
  make S44000A01 sub LCD enter sleep mode.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
extern void s44000a01_power_off
( 
  void 
);
extern void s44000a01_power_down
( 
    void 
);
/*===========================================================================

FUNCTION s44000a01_volume_set

DESCRIPTION  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

===========================================================================*/
extern void s44000a01_volume_set( OP_UINT8 val );

/* end of file */
#endif  /* S44000A01_H */

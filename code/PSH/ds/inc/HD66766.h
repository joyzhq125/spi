#ifndef HD66766_H
#define HD66766_H
/*==================================================================================================

    HEADER NAME : HD66766.h

    GENERAL DESCRIPTION
        All the declarations and definitions necessary for the small size
        color graphic display device (LCD) HITACHI HD66766 driver to interface 
        with the part.  This file    is specific to the CASIO HD66766 part, 
        please refer to the data sheets when examinig this module.

    REFERENCES
        HITACHI HD66766 LCD Data Sheet.
        This LCD driver is reused from Janus_K.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    07/18/2002   Ashley Lee       crxxxxx     Initially reodered from Janus_K LCD driver.
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.

====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "OPUS_typedef.h"

#include    "rm_typedef.h"
#include    "rm_bitmap.h"
#include    "ds_util.h"
#include    "sp_include.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
#define HD66766_MAX_ROW           LCD_MAX_Y
#define HD66766_MAX_COL           LCD_MAX_X
/*==================================================================================================
    MACROS
==================================================================================================*/
#define LCD_CONTRAST_CENTER         0x0004    
/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/
/*
*   None
*/

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/*
*   None
*/

/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/
/*
*   None
*/

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/
extern void HD66766_power_on
(
    void
);
extern void HD66766_power_off
(
    void
);

extern void HD66766_Display_on
(
    OP_BOOLEAN bOn
);

extern void HD66766_standby_on
(
    OP_BOOLEAN bOn
);

extern void HD66766_Sleep_on
(
    OP_BOOLEAN bOn
);

extern void HD66766_high_speed_write_region 
(
    OP_UINT8 col1, 
    OP_UINT8 row1, 
    OP_UINT8 col2, 
    OP_UINT8 row2, 
    void * color, 
    OP_UINT8 width, 
    OP_UINT8 start_row, 
    OP_UINT8 start_col
) ;

extern void HD66766_volume_set
(
    OP_UINT8 volume
);

extern void CLC346MainLCD_Initialize_HITACHA(void);
/*================================================================================================*/
#endif  /* HD66766_H */

/*
 * Copyright (C) Techfaith, 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */

#ifndef SIA_RC_H
#define SIA_RC_H

#ifndef WE_CFG_H
#include "We_Cfg.h"
#endif

/*--- Definitions/Declarations ---*/
#define SIA_NUMBER_OF_STRINGS  23

extern const char * const Sia_strings[][SIA_NUMBER_OF_STRINGS+1];

/*--- Types ---*/

/*--- Constants ---*/
/* " " */
#define SIA_STR_ID_EMPTY_STRING                             ((1<<8) | WE_MODID_SIA)
/* "Play" */
#define SIA_STR_ID_MENU_PLAY                                ((2<<8) | WE_MODID_SIA)
/* "Rewind" */
#define SIA_STR_ID_MENU_REWIND                              ((3<<8) | WE_MODID_SIA)
/* "Previous page" */
#define SIA_STR_ID_MENU_PREVIOUS                            ((4<<8) | WE_MODID_SIA)
/* "Next page" */
#define SIA_STR_ID_MENU_NEXT                                ((5<<8) | WE_MODID_SIA)
/* "Back" */
#define SIA_STR_ID_MENU_BACK                                ((6<<8) | WE_MODID_SIA)
/* "Close" */
#define SIA_STR_ID_MENU_EXIT                                ((7<<8) | WE_MODID_SIA)
/* "Ok" */
#define SIA_STR_ID_ACTION_OK                                ((8<<8) | WE_MODID_SIA)
/* "Cancel" */
#define SIA_STR_ID_ACTION_CANCEL                            ((9<<8) | WE_MODID_SIA)
/* "Play" */
#define SIA_STR_ID_ACTION_PLAY                              ((10<<8) | WE_MODID_SIA)
/* "Stop" */
#define SIA_STR_ID_ACTION_STOP                              ((11<<8) | WE_MODID_SIA)
/* "Back" */
#define SIA_STR_ID_ACTION_BACK                              ((12<<8) | WE_MODID_SIA)
/* "Menu" */
#define SIA_STR_ID_ACTION_MENU                              ((13<<8) | WE_MODID_SIA)
/* "Exit" */
#define SIA_STR_ID_ACTION_EXIT                              ((14<<8) | WE_MODID_SIA)
/* "The text could not be displayed." */
#define SIA_STR_ID_KNOWN_TEXT                               ((15<<8) | WE_MODID_SIA)
/* "Object list" */
#define SIA_STR_ID_OBJ_LIST                                 ((16<<8) | WE_MODID_SIA)
/* "Internal error. The application is going to close." */
#define SIA_STR_ID_PANIC                                    ((17<<8) | WE_MODID_SIA)
/* "Open link" */
#define SIA_STR_ID_OPEN_LINK                                ((18<<8) | WE_MODID_SIA)
/* "Call" */
#define SIA_STR_ID_MAKE_CALL                                ((19<<8) | WE_MODID_SIA)
/* "Called party is busy" */
#define SIA_STR_ID_CALL_BUSY                                ((20<<8) | WE_MODID_SIA)
/* "Call failed" */
#define SIA_STR_ID_CALL_FAILED                              ((21<<8) | WE_MODID_SIA)
/* "Unable to display all items" */
#define SIA_STR_DIA_TO_MANY_ITEMS                           ((22<<8) | WE_MODID_SIA)

#define SIA_STR_DIA_CORRUPT_CONTENT                         ((23<<8) | WE_MODID_SIA)

/*Predefined images*/
#define SIA_NUMBER_OF_IMAGES 4

extern const unsigned char * const Sia_images[SIA_NUMBER_OF_IMAGES + 1];

/*!< A play icon */
#define SIA_IMG_STOP            ((  1<<8) | WE_MODID_SIA)
/*!< A stop icon */
#define SIA_IMG_PLAY            ((  2<<8) | WE_MODID_SIA)
/*!< Not able to display the media object */
#define SIA_IMG_BROKEN_IMAGE    ((  3<<8) | WE_MODID_SIA)
/*!< An indication that an audio object is playing */
#define SIA_IMG_AUDIO           ((  4<<8) | WE_MODID_SIA)


/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Prototypes ---*/

#endif

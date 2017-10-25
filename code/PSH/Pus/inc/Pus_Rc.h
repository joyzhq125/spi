/*
 * Copyright (C) Techfaith 2002-2005.
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
 /*
 * Pus_Rc.h
 *
 * Description:
 *		This file contain defines for all strings used in the PUS
 *    module. All strings are predefined.
 */


#ifndef _Pus_Rc_h
#define _Pus_Rc_h

#ifndef _we_wid_h 
#include "we_wid.h"
#endif

#define PUS_NUMBER_OF_STRINGS 6

extern const char * const pus_strings[][PUS_NUMBER_OF_STRINGS+1];


#define PUS_STR_ID_OK               (WE_MODID_PUS + 0x0100) /*"Ok"*/
#define PUS_STR_ID_CANCEL           (WE_MODID_PUS + 0x0200) /*"Cancel"*/
#define PUS_STR_ID_SIA_CONFIRM_MSG  (WE_MODID_PUS + 0x0300) /*"Would you like to set up a new push connection?"*/
#define PUS_STR_ID_NO_WHITELIST_MSG (WE_MODID_PUS + 0x0400) /*"Push from unauthorised sender"*/
#define PUS_STR_ID_ACCEPT_MSG       (WE_MODID_PUS + 0x0500) /*"Accept push"*/
#define PUS_STR_ID_DECLINE_MSG      (WE_MODID_PUS + 0x0600) /*"Decline push"*/


#endif

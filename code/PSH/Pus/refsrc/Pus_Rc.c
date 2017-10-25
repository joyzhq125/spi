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
 * Description: This file implement the recources needed by the 
 *              PUS module.
 *    
 */

#include "Pus_Rc.h"

/* String Resources */

const char * const pus_strings[][PUS_NUMBER_OF_STRINGS+1] = 
{ /*English*/
  {
  "Ok",                                                              /*PUS_STR_ID_OK*/
  "Cancel",                                                          /*PUS_STR_ID_CANCEL*/
  "Would you like to set up a new push connection?",                 /*PUS_STR_ID_SIA_CONFIRM_MSG*/
  "Push from unauthorised sender",                                   /*PUS_STR_ID_NO_WHITELIST_MSG*/
  "Accept push",                                                     /*PUS_STR_ID_ACCEPT_MSG*/
  "Decline push",                                                     /*PUS_STR_ID_DECLINE_MSG*/
  NULL /*The table is NULL-terminated*/
  }
  /*Other language*/
 /*{
     Etc.
   }*/
};

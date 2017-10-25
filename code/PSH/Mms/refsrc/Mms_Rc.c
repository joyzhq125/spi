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

#include "Mms_Rc.h"

/*! \file mms_rc.c
 *  \brief Language definitions for the MMS service.
 */

/******************************************************************************
 * Constants
 *****************************************************************************/

/* String Resources */

const char * const mms_strings[][MMS_NUMBER_OF_STRINGS+1] = 
{ /*English*/
  {
    " ",                                                         /*MMS_STR_ID_EMPTY_STRING*/
    /*!
         * Text defines used for creating a Read Reply message.
     */
    "From: ",                                                    /*MMS_STR_ID_RR_TEXT_FROM*/
    "READ: ",                                                    /*MMS_STR_ID_RR_TEXT_READ*/
    "Your message\n\n",                                          /*MMS_STR_ID_RR_TEXT_YOUR_MSG*/
    "To:         ",                                              /*MMS_STR_ID_RR_TEXT_TO*/
    "Subject:    ",                                              /*MMS_STR_ID_RR_TEXT_SUBJECT*/
    "Message WID: ",                                              /*MMS_STR_ID_RR_TEXT_MSGID*/
    "Sent:       ",                                              /*MMS_STR_ID_RR_TEXT_SENT*/
    "\n\nwas read on ",                                          /*MMS_STR_ID_RR_TEXT_WAS_READ*/
    NULL /*The table is NULL-terminated*/
  }
  /*Other language*/
 /*{
     Etc.
   }*/
};


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

/*! \file Mms_Rc.h
 *  \brief Language definitions for the MMS service.
 */

#ifndef MMS_RC_H_
#define MMS_RC_H_

#include "We_Cfg.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! The total number of strings defined for the MMS service.
 *
 */
#define MMS_NUMBER_OF_STRINGS 9

/*! The strings table for the MMS service.
 *
 */
extern const char * const mms_strings[][MMS_NUMBER_OF_STRINGS+1];

/* Empty string, i.e., "" */
#define MMS_STR_EMPTY_STRING          ((   1<<8) | WE_MODID_MMS) /*" "*/
/*!
 * Text defines used for creating a Read Reply message.
 *
 * Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_FROM       ((   2<<8) | WE_MODID_MMS) /*"From: "*/
/*! Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_READ       ((   3<<8) | WE_MODID_MMS) /*"READ: "*/
/*! Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_YOUR_MSG   ((   4<<8) | WE_MODID_MMS) /*"Your message\n\n"*/
/*! Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_TO         ((   5<<8) | WE_MODID_MMS) /*"To:         "*/
/*! Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_SUBJECT    ((   6<<8) | WE_MODID_MMS) /*"Subject:    "*/
/*! Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_MSGID      ((   7<<8) | WE_MODID_MMS) /*"Message WID: "*/
/*! Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_SENT       ((   8<<8) | WE_MODID_MMS) /*"Sent:       "*/
/*! Part of Read Reply message. */
#define MMS_STR_ID_RR_TEXT_WAS_READ   ((   9<<8) | WE_MODID_MMS) /*"\n\nwas read on "*/


#endif

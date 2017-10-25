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
 * uba_text.h
 *
 * Created by Martin Andersson, 2003/08/22.
 *
 *
 * This file handles one complex text resource, the goal is to produce a
 * widget string handle. This gui string can be extracted from a string rc
 * or it can be dynamicaly created from context data
 *
 *
 * Revision history:
 *
 */
#ifndef UBA_TEXT_H
#define UBA_TEXT_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef _we_int_h
  #include "We_Int.h"
#endif

#ifndef UBA_RCE_H
 #include "Uba_Rce.h"
#endif

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/* Defines an uba text */
typedef struct  {
  uba_text_type_t type; /* Type of text */
  WeStringHandle text; /* resource handle owned by the object */
}uba_text_t;

/************************************************************************/
/* MACROs                                                                */
/************************************************************************/

/* Get the WeString handle from the text */
#define UBA_TEXT_GET_HANDLE(p) ((p)->text)


/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/* Init p with the data object data and rc */
int
uba_text_init (uba_text_t* p, uba_data_obj_t* data, const uba_rce_text_t* rc);

/* Release p and ps resources */
void
uba_text_release (uba_text_t* p);

#endif      /*UBA_TEXT_H*/


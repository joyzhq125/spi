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
 * We_Xmim.h
 * 
 * Additional Mime type support
 *
 */
#ifndef _we_xmim_h
#define _we_xmim_h

#ifndef _we_def_h 
#include "We_Def.h"
#endif

/*
 * Define this constant to be the number of additional mime types
 * that are supported, or 0 if none are supported.
 */
#define WE_NUMBER_OF_ADDITIONAL_MIME_TYPES   0

typedef struct {
  WE_INT32             value;
  const char            *mime;
  const char            *extension;
} we_xmim_entry_t;

#if WE_NUMBER_OF_ADDITIONAL_MIME_TYPES > 0
extern const we_xmim_entry_t we_xmim_table[WE_NUMBER_OF_ADDITIONAL_MIME_TYPES];
#endif

#endif


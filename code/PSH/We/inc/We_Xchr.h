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
 * We_Xchr.h
 * 
 * Additional Character Set Support
 *
 */
#ifndef _we_xchr_h
#define _we_xchr_h

#ifndef _we_def_h 
#include "We_Def.h"
#endif

#ifndef _we_chrs_h 
#include "We_Chrs.h"
#endif

/*
 * Define this constant to be the number of additional character sets
 * that are supported, or 0 if none are supported.
 */
#define WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS   2

typedef struct {
  int                    character_set;
  const char            *preferred_name;
  const char            *aliases;
  we_charset_convert_t *cvt_to_utf8;  
  we_charset_convert_t *cvt_from_utf8;  
} we_xchr_entry_t;

#if WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS > 0
extern const we_xchr_entry_t we_xchr_table[WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS];
#endif

#endif

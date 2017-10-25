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
 * Wml_Inpf.h
 *
 * Created by Henrik Olsson
 *
 * Revision history:
 *
 */
#ifndef _wml_inpf_h
#define _wml_inpf_h

#ifndef _we_def_h
#include "We_Def.h"
#endif

#ifndef _we_int_h
#include "We_Int.h"
#endif


int 
wml_input_make_valid(const char* inStr,
                     int startSel,
                     int endSel,
                     const char* newChars,
                     const char* formatStr,
                     int inputRequired,
                     char* outStr,
                     int* outStrSize);

#endif  /*_wml_inpf_h*/

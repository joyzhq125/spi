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
#ifndef _we_lib_h
#define _we_lib_h

/************************************************************
 * Includes for ANSI-C standard liwbaries
 ************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>

#ifndef _we_def_h
#include "We_Def.h"
#endif
#ifdef CFG_WE_HAS_FLOAT
#include <math.h>
#include <errno.h>
#endif

#endif

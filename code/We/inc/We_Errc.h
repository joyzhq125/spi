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
#ifndef _we_errc_h
#define _we_errc_h

#ifndef _we_int_h
#include "We_Int.h"
#endif

/* System errors */
#define WE_ERR_SYSTEM_REG_ABORTED                  TPI_ERR_SYSTEM_REG_ABORTED
#define WE_ERR_SYSTEM_REG_FILE_CORRUPT             TPI_ERR_SYSTEM_REG_FILE_CORRUPT
#define WE_ERR_SYSTEM_FATAL                        TPI_ERR_SYSTEM_FATAL
#define WE_ERR_SYSTEM_REG_TXT_FILE                 TPI_ERR_SYSTEM_REG_TXT_FILE
#define WE_ERR_SYSTEM_REG_TXT_PARSE                TPI_ERR_SYSTEM_REG_TXT_PARSE

/* Module errors */
#define WE_ERR_MODULE_OUT_OF_MEMORY                TPI_ERR_MODULE_OUT_OF_MEMORY


#define WE_ERROR                                   TPIa_error

#endif

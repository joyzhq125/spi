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










#ifndef _Pus_Hdr_h
#define _Pus_Hdr_h

#ifndef _Pus_Cfg_h
#include "Pus_Cfg.h"
#endif

#ifdef PUS_CONFIG_MULTIPART 

int 
Pus_Hdr_create_hdr_string_from_bin_hdrs (we_hdr_t *table, unsigned char *headers, int headers_len);

#endif

#endif

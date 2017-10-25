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










#ifndef _frw_reg_h
#define _frw_reg_h

#ifndef _we_core_h
#include "We_Core.h"
#endif





#define FRW_REG_PATHNAME                          "/frw"
#define FRW_REG_FILENAME                          "/frw/registry.dat"
#define FRW_REG_MAX_LENGTH_OF_STRING_VALUE        500
#define FRW_REG_MAX_LENGTH_OF_PATH_KEY            16
#define FRW_REG_MAX_LEVELS_OF_PATHS               8





#define FRW_REGISTRY_SIG_SET            FRW_SIG_DST_FRW_REGISTRY + 1
#define FRW_REGISTRY_SIG_DELETE         FRW_SIG_DST_FRW_REGISTRY + 2
#define FRW_REGISTRY_SIG_SUBSCRIBE      FRW_SIG_DST_FRW_REGISTRY + 3
#define FRW_REGISTRY_SIG_GET            FRW_SIG_DST_FRW_REGISTRY + 4
#define FRW_REGISTRY_SIG_RUN            FRW_SIG_DST_FRW_REGISTRY + 5
#define FRW_REGISTRY_SIG_MOD_TERMINATED FRW_SIG_DST_FRW_REGISTRY + 6


















void
frw_reg_init (const char* startup_file, const char* default_file);




void
frw_reg_terminate (void);









void
frw_reg_int_set_init (void);




void
frw_reg_int_set_path (char* path);




void
frw_reg_int_set_add_key_int (char* key, WE_INT32 value);




void
frw_reg_int_set_add_key_str (char* key, unsigned char* value, WE_UINT16 valueLength);




void
frw_reg_int_set_commit (void);




int
frw_reg_int_resp_get_next (WE_UINT8 wid, we_registry_param_t* param);




void
frw_reg_int_resp_free (WE_UINT8 wid);

#endif

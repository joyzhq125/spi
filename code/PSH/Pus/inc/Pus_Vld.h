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








 
#ifndef _phd_hvld_h
#define _phd_hvld_h

#ifndef _Pus_Cfg_h
#include "Pus_Cfg.h"
#endif
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES

#ifndef _we_prsr_h
#include "we_prsr.h"
#endif




typedef struct {
  int                  instanceId;
  int                  state;
  int                  doc_type;
  int                  charset;
  int                  content_type;
  int                  stream_handle;
} pus_hvld_user_data_t;















int
pus_hvld_init_parser_object (we_prsr_parserData_t*  p,
                             we_prsr_decoderData_t* d,
                             const char*             doctype_str,
                             int                     len,
                             int                     doctype_int,
                             int                     charset);


#endif
#endif

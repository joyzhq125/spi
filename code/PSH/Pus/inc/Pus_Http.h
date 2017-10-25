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








 
#ifndef _Pus_Http_h
#define _Pus_Http_h

#ifndef _Pus_Cfg_h
#include "Pus_Cfg.h"
#endif

#ifdef PUS_CONFIG_HTTP_PUSH

#ifndef _Pus_Sig_h
#include "Pus_Sig.h"
#endif




#define PUS_HTTP_SIG_NEW_MESSAGE                    PUS_MODULE_HTTP + 1









void
Pus_Http_init (void);

void
Pus_Http_main (Pus_Signal_t* signal);

#endif

#endif

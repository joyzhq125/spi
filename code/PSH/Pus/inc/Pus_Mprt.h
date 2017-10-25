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








 
#ifndef _Pus_Mprt_h
#define _Pus_Mprt_h

#ifndef _Pus_Cfg_h
#include "Pus_Cfg.h"
#endif

#ifdef PUS_CONFIG_MULTIPART 

#ifndef _Pus_Sig_h
#include "Pus_Sig.h"
#endif

#ifndef _Pus_Main_h
#include "Pus_Main.h"
#endif








#define PUS_MPRT_SIG_NEW_MESSAGE                    PUS_MODULE_MPRT + 1









void
Pus_Mprt_init (void);

void
Pus_Mprt_main (Pus_Signal_t* signal);

#endif

#endif

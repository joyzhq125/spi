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









#ifndef _Pus_Sia_h
#define _Pus_Sia_h

#include "Pus_Cfg.h"

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 

#ifndef _Pus_Sig_h
#include "Pus_Sig.h"
#endif









void
Pus_Sia_init (void);




void
Pus_Sia_terminate (void);

int
pus_handle_sia_push (stk_content_t *p);

void
Pus_Sia_main (Pus_Signal_t* signal);

#endif

#endif

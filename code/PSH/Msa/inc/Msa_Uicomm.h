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

/* \file mauicomm.h
 */

#ifndef _MAUICOMM_H
#define _MAUICOMM_H

WE_BOOL msaShowHomeRetrievalDialog(MsaStateMachine fsm, unsigned int signal, 
    unsigned int size);
WE_BOOL msaShowRoamingRetrievalDialog(MsaStateMachine fsm, 
    unsigned int signal, unsigned int size);
WE_BOOL msaShowRoamingSendDialog(MsaStateMachine fsm, unsigned int signal,
                                  unsigned int size);
WE_BOOL msaShowSettingsErrorDialog(MsaStateMachine fsm, int signal);

#endif /* _MAUICOMM_H */

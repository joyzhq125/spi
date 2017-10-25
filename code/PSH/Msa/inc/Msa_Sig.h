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

/* \file masig.h
 * \brief Handler of signals originating from the external signal queue.
 */

#ifndef _MASIG_H_
#define _MASIG_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! Timer identity for the progress meter view */
#define MSA_PM_TIMER_ID             0x01

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaSignalReceive(WE_UINT8 src_module, WE_UINT16 signal, void* p);

#endif

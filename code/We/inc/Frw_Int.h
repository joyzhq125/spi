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
/*
 * Frw_Int.h
 *
 * Created by Ingmar Persson
 *
 * Revision  history:
 * 
 *
 */

#ifndef _frw_int_h
#define _frw_int_h

#ifndef _we_core_h
#include "We_Core.h"
#endif

/****************************************
 * Module status
 ****************************************/
#define FRW_MODULE_STATUS_CREATED         MODULE_STATUS_CREATED
#define FRW_MODULE_STATUS_ACTIVE          MODULE_STATUS_ACTIVE
#define FRW_MODULE_STATUS_SHUTTING_DOWN   MODULE_STATUS_SHUTTING_DOWN
#define FRW_MODULE_STATUS_TERMINATED      MODULE_STATUS_TERMINATED


void
FRWa_createTask (WE_UINT8 uiModId);

void
FRWa_moduleStatus 
(
    WE_UINT8 uiModId, 
    int iStatus
);

void
FRWa_terminated (void);


/****************************************
 * Timers
 ****************************************/

void
FRWa_setTimer (WE_UINT32 uiTimeInterval);

void
FRWa_resetTimer (void);

int 
FRWa_setPersistentTimer (WE_UINT8 uiModId, WE_UINT32 uiTime);

int 
FRWa_resetPersistentTimer (void);


/****************************************
 * Registry
 ****************************************/

void
FRWa_registryResponse (WE_UINT8 wid);


/****************************************
 * External
 ****************************************/

void
FRWa_externalGet 
(
    WE_INT32 wid, 
    const char* pcPath, 
    const char* pcKey
);

WE_BOOL wap_modules_run(void);

#endif

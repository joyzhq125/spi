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
 * Frw_Env.h
 *
 * Created by Ingmar Persson
 *
 * Revision  history:
 *   021014  IPN  Added FRWc_kill.
 *   021014  IPN  Added FRWc_externalResponse.
 *
 */

#ifndef _frw_env_h
#define _frw_env_h

#ifndef _we_core_h
#include "We_Core.h"
#endif

/* When this function has returned, you can start 
   scheduling WE with the function WEc_run. */
void
FRWc_startMobileSuite 
(
    const char* pcStartOptions, 
    const int *piStaticModules, 
    int iNumberOfStaticModules
);

void
FRWc_terminateMobileSuite (void);

int
FRWc_wantsToRun (void);

void
FRWc_run (void);

void
FRWc_kill (void);

void
FRWc_startModule 
(
    WE_UINT8 uiModId, 
    char* pcStartOptions
);

void
FRWc_terminateModule (WE_UINT8 uiModId);

void
FRWc_timerExpiredEvent (void);

void 
FRWc_persistentTimerExpiredEvent (void);


/****************************************
 * Registry
 ****************************************/

void
FRWc_registrySetInit (void);

void
FRWc_registrySetPath (char* pcPath);

void
FRWc_registrySetAddKeyInt 
(
    char* pcKey, 
    WE_INT32 iValue
);

void
FRWc_registrySetAddKeyStr 
(
    char* pcKey, 
    unsigned char* pcValue, 
    WE_UINT16 uiValueLength
);

void
FRWc_registrySetCommit (void);

void
FRWc_registryGet 
(
    WE_UINT8 wid, 
    char* pcPath, 
    char* pcKey
);

void
FRWc_registryGet 
(
    WE_UINT8 wid, 
    char* pcPath, 
    char* pcKey
);

void
FRWc_registrySubscribe 
(
    WE_UINT8 wid, 
    const char* pcPath, 
    const char* pcKey, 
    WE_BOOL bAdd
);

void
FRWc_registrySubscribe 
(
    WE_UINT8 wid, 
    const char* pcPath, 
    const char* pcKey, 
    WE_BOOL bAdd
);

void
FRWc_registryRespFree (WE_UINT8 wid);


/****************************************
 * External
 ****************************************/

void
FRWc_externalResponse 
(
    WE_INT32 wid, 
    we_registry_param_t* pstParam
);

void
FRWc_objectAction 
(
    const we_act_content_t* pstContent,
    const char*              pcExclActStrings[],
    WE_INT32                iExclActStringsCnt
);

#endif

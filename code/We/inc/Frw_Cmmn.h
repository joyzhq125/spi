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



#ifndef _frw_cmmn_h
#define _frw_cmmn_h

#ifndef _we_core_h
#include "We_Core.h"
#endif


#define FRW_CMMN_SIG_OBJECT_ACTION        FRW_SIG_DST_FRW_CMMN + 1
#define FRW_CMMN_SIG_GET_ICONS                FRW_SIG_DST_FRW_CMMN + 2
#define FRW_CMMN_SIG_WIDGET_ACTION       FRW_SIG_DST_FRW_CMMN + 3
#define FRW_CMMN_SIG_ACT_FINISHED          FRW_SIG_DST_FRW_CMMN + 4


void
frw_cmmn_init (void);


void
frw_cmmn_terminate (void);




int
frw_cmmn_handle_package_signal
(
    WE_UINT16 uiSignal, 
    void *p
);




void
frw_content_send_ack 
(
    WE_UINT8 uiModId, 
    WE_UINT8 wid, 
    WE_UINT8 uiStatus,
    WE_BOOL bCallerOwnsPipe
);


#endif

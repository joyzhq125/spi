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










#ifndef _frw_time_h
#define _frw_time_h

#ifndef _we_def_h
#include "We_Def.h"
#endif






#define FRW_TIMER_SIG_SET                  FRW_SIG_DST_FRW_TIMER + 1
#define FRW_TIMER_SIG_RESET                FRW_SIG_DST_FRW_TIMER + 2
#define FRW_TIMER_SIG_EXPIRED              FRW_SIG_DST_FRW_TIMER + 3
#define FRW_TIMER_SIG_SET_PERSISTENT       FRW_SIG_DST_FRW_TIMER + 4
#define FRW_TIMER_SIG_RESET_PERSISTENT     FRW_SIG_DST_FRW_TIMER + 5
#define FRW_TIMER_SIG_PERSISTENT_EXPIRED   FRW_SIG_DST_FRW_TIMER + 6
#define FRW_TIMER_SIG_FILE_EXECUTE         FRW_SIG_DST_FRW_TIMER + 7
#define FRW_TIMER_SIG_TIMER_READ_RESP       FRW_SIG_DST_FRW_TIMER + 8




void
frw_timer_init (void);

void
frw_timer_terminate (void);

void
frw_timer_set (WE_UINT8 modId, WE_UINT32 timerID, WE_UINT32 msInterval);

void
frw_timer_reset (WE_UINT8 modId, WE_UINT32 timerID);

void
frw_timer_reset_all (WE_UINT8 modId);

void
frw_timer_expired (int do_reset);

void
frw_timer_set_persistent (WE_UINT8 modId, WE_UINT32 timerID, WE_UINT32 date_time);

void
frw_timer_reset_persistent (WE_UINT8 modId, WE_UINT32 timerID);

void
frw_timer_persistent_expired (void);

#endif

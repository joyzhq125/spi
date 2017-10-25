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
 * We_Time.h
 *
 * Created by Tomas Mandorf, Mon Jan 13 17:24:29 2004.
 *
 * Revision history:
 */

#ifndef _we_time_h
#define _we_time_h

#ifndef _we_cfg_h
#include "We_Cfg.h"
#endif

#ifndef _we_def_h
#include "We_Def.h"
#endif

/************************************************************ 
 * Constants
 ************************************************************/



/**************************************************
 * Type definitions 
 ***************************************************/

typedef struct 
{
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
} we_time_tm;


/************************************************************ 
 * Exported functions
 ************************************************************/


/*
 * Converts a 32 bit time value to a UTC date /time 
 * info struct.
 */
void
we_time_gmtime (WE_UINT32 time, we_time_tm *tm);


/*
 * Converts a UTC date / time struct to a 32 bit
 * time value returned in the time parameter. 
 * No timezone corrections are made.
 * Return True if successful, else False.
 */
int
we_time_mktime (we_time_tm *tm, WE_UINT32 *time);

/*
 * Converts a 32 bit time value to a date / time
 * info struct and corrects for the local time zone. 
 * Return True if successful, else False.
 */
int
we_time_localtime (WE_UINT32 time, we_time_tm *tm);

#endif /* _we_time_h */





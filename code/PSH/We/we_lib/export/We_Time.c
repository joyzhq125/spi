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
 * We_Time.c
 *
 * Created by Tomas Mandorf, Thu Jan 15 11:00:00 2004.
 *
 * Revision history:
 */


#include "We_Time.h"
#include "We_Cmmn.h"
#include "We_Core.h"

/************************************************************ 
 * Constants
 ************************************************************/


/**************************************************
 * Type definitions 
 ***************************************************/


/************************************************************ 
 * Exported functions
 ************************************************************/
static const char we_cmmn_m[12] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const char we_cmmn_m4[12] = {
  31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


#define WE_TIME_ONE_DAY_SEC   0x0015180
#define WE_TIME_ONE_HOUR_SEC  0x0000E10
#define WE_TIME_ONE_MIN_SEC   0x000003C

#define WE_TIME_CONST     719499 /*492-19+4+336+1+718685 Days until 1/1-1970*/
/*
 * Converts a 32 bit time value to
 * a UTC date /time info struct. Return True if 
 * successful, else False
 */
void
we_time_gmtime (WE_UINT32 time, we_time_tm *tm)
{
  int        i, nly;
  const char *days_in_month;

  /* First, divide into days, hours, minutes and seconds. */
  tm->tm_yday = (time / WE_TIME_ONE_DAY_SEC);
  time -= (tm->tm_yday * WE_TIME_ONE_DAY_SEC);

  
  tm->tm_wday = (tm->tm_yday + 4) % 7;

  tm->tm_hour = (time / WE_TIME_ONE_HOUR_SEC);
  time -= (tm->tm_hour * WE_TIME_ONE_HOUR_SEC);

  tm->tm_min = (time / WE_TIME_ONE_MIN_SEC);
  tm->tm_sec = (time - (tm->tm_min * WE_TIME_ONE_MIN_SEC));

  /* Now the number of tm->tm_mday has to be divided into years and months.
   * Start by approximating each year to be 365 days.
   * This approximation will be at most one off.
   * Compensate if necessary. */
  tm->tm_year = (tm->tm_yday / 365);
  tm->tm_yday = (tm->tm_yday - (tm->tm_year * 365));
  /* In this interval (1970 - 2038), every fourth year is a leap year,
   * without exceptions, starting with 1972. */
  nly = ((tm->tm_year + 1) / 4);
  if (tm->tm_year > 131 || (tm->tm_year == 131 && tm->tm_yday > 32) ){
    nly--;/*1970 + 130 = 2100 => no leapyear*/
  }

  if (tm->tm_yday < nly) {
    tm->tm_year--;
    tm->tm_yday = (tm->tm_yday + (365) - nly);
    if (((tm->tm_year + 2) & 0x3) == 0) {
      tm->tm_yday++;
    }

  }
  else {
    tm->tm_yday -= nly;
  }

  /* To determine the month we simply do a linear search through
   * an array holding the number of days of each month.
   * First we have to select the correct array though,
   * there is one for leap years and one for non-leap years. */
  if (((tm->tm_year + 2) % 4) == 0 && tm->tm_year != 130) /*1970 +130 = 2100 => no leapyear*/
    days_in_month = we_cmmn_m4;
  else
    days_in_month = we_cmmn_m;

  tm->tm_mday = tm->tm_yday;
  for (i = 0; i < 12; i++) {
    if (tm->tm_mday < days_in_month[i])
      break;
    tm->tm_mday -= days_in_month[i];
  }
  tm->tm_mday += 1;
  tm->tm_mon = i;

  /*The tm struct holds years since 1900 and therefor add 70*/
  tm->tm_year += 70;

  /*UTC time is never timezone corrected*/
  tm->tm_isdst = 0; 

}
/*
 * Converts a UTC date / time struct to a 32 bit
 * time value returned in the time parameter. 
 * No timezone corrections are made.
 * Return True if successful, else False.
 */
int
we_time_mktime (we_time_tm *tm, WE_UINT32 *time)
{
  WE_UINT32 days = 0;
  WE_UINT32 tm_year = 0;
  WE_UINT32 tm_mon = 0;
  WE_UINT32 tm_mday = 0;
  WE_UINT32 tm_hour = 0;
  WE_UINT32 tm_min = 0;
  WE_UINT32 tm_sec = 0;

  if (tm == NULL) {
    return FALSE;
  }
  tm_year = tm->tm_year;
  tm_mon = tm->tm_mon;
  tm_mday = tm->tm_mday;
  tm_hour = tm->tm_hour;
  tm_min = tm->tm_min;
  tm_sec = tm->tm_sec;

  *time = 0;

  /* Check the year value we retrieved */
  if (tm->tm_year < 0) {
    return FALSE;
  }

  /* If years has wrong format correct it */
  if (tm->tm_year < 1000) {
    tm_year += 1900;
  }
  /* Check year intevall*/
  if ((tm_year < 1970) || (tm_year > 2106)) {
    return FALSE;
  }

  /*Month is given from 0 to 11 add one*/
  tm_mon++;
  /*February has the leap day put it last */
  if (0 >= (int)(tm_mon -= 2)) {
    tm_mon += 12;
    tm_year -= 1;
  }
  /*Days since 1970*/
  days = (unsigned long)(tm_year/4 - tm_year/100 + tm_year/400 + 367*tm_mon/12 + tm_mday) + 
                         tm_year*365 - WE_TIME_CONST;

  /*Seconds since 1970*/
  *time = ((unsigned long)(days*24 + tm_hour)*60 + tm_min )*60 + tm_sec;
  
  tm->tm_yday = days-(unsigned long)((tm_year/4 - tm_year/100 + tm_year/400 + 367*11/12 + 1) + 
                                  tm_year*365 - WE_TIME_CONST);
  tm->tm_wday = (days + 4) % 7;

  if (tm->tm_year > 1900) {
    tm->tm_year -= 1900;
  }
  return TRUE;
}

/*
 * Converts a 32 bit time value to a date / time
 * info struct and corrects for the local time zone. 
 * Return True if successful, else False.
 */
int
we_time_localtime (WE_UINT32 time, we_time_tm *tm)
{
  WE_INT16 time_zone_diff_min = 0;
  time_zone_diff_min = WE_TIME_GET_TIME_ZONE();
  if(time_zone_diff_min == TPI_TIME_ZONE_UNKNOWN){
    return FALSE;
  }
  time += time_zone_diff_min*60;
  we_time_gmtime (time, tm);
  tm->tm_isdst = -1;
  return TRUE;
}




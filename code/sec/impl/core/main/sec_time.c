/*==================================================================================================
    HEADER NAME : sec_time.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the function for getting time.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None        Draft
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/

#include "we_def.h"
#include "aeestdlib.h"

#define TEN_YEAR_SECONDS 315964800L
/*==================================================================================================
FUNCTION: 
    Sec_TimeGetCurrent
CREATE DATE:
    2006-07-08
AUTHOR:
    Kevin Yang
DESCRIPTION:
    This function return the seconds elapsed since 1970-01-01 00.00.00 in local time. 
ARGUMENTS PASSED:
    None.
RETURN VALUE:
    the time value.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    OMIT.
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 Sec_TimeGetCurrent(void)
{
    WE_UINT32 uiTmpTime = 0;
 
    uiTmpTime = GETTIMESECONDS();
    uiTmpTime += TEN_YEAR_SECONDS;

    return uiTmpTime;
}


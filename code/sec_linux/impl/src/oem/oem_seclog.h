/*==================================================================================================
    HEADER NAME : oem_secLog.c
    MODULE NAME : WEA
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the  function prototype for log use.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        0.0.1         Draft
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/

#ifndef _OEM_SECLOG_H
#define _OEM_SECLOG_H


#define WE_LOG_MODULE

/* Logging levels */
#define WE_LOG_DETAIL_LOW              0
#define WE_LOG_DETAIL_MEDIUM           1
#define WE_LOG_DETAIL_HIGH             2

#ifdef WE_LOG_MODULE

void Sec_LogMsg(WE_INT32 iType, WE_UINT8 ucId, const WE_CHAR * pcFormat,...);

void Sec_LogData (WE_INT32 iType, WE_UINT8 ucId, const WE_UCHAR *pucData, WE_INT32 iDataLen);

#define WE_LOG_MSG(x)                              Sec_LogMsg x
#define WE_LOG_DATA(e, m, d, l)                    Sec_LogData(e, m, d, l)
#else
#define WE_LOG_MSG(x)
#define WE_LOG_DATA(level, m, d, l)
#endif


#endif 

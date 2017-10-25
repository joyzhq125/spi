/*=====================================================================================
    FILE NAME :
        oem_Seccsc.h
    MODULE NAME :
        sec
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    Modification Tracking
    Date       Author                      Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-10-19 Zhanghuanqing     none      Init
    
=====================================================================================*/

#ifndef _OEM_SECCSC_H
#define _OEM_SECCSC_H

#include "aeeappgen.h"
#include "we_csc.h"

WE_BOOL
Sec_Csc(IShell* pIShell,
            WE_INT32 iFromCharset,
            WE_INT32 iToCharset,
            WE_CHAR* pcSrc,
            WE_LONG lSrcLen,
            WE_CHAR* pcDst,
            WE_LONG* plDstLen);
#endif /*_OEM_SECCSC_H*/


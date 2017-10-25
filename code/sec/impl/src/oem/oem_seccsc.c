/*=====================================================================================
    FILE NAME :
        oem_Seccsc.c
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

#include "wecsc.bid" /*csc*/

#include "oem_seccsc.h"

/*==================================================================================================
FUNCTION: 
    Sec_Csc
CREATE DATE:
    2006-10-19
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    charset convert
ARGUMENTS PASSED:
    WE_INT32 iFromCharset[IN]:souce charset
    WE_INT32 iToCharset[IN]: dst charset
    WE_CHAR *pcSrc[IN]:source string.
    WE_LONG lSrcLen[IN]:source string length
    WE_CHAR *pcDst[OUT]:dst buffer
    WE_LONG *plDstLen[OUT]:dst length

RETURN VALUE:
    TRUE  : sucess
    FALSE : fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_BOOL
Sec_Csc(IShell* pIShell,
      WE_INT32 iFromCharset,
      WE_INT32 iToCharset,
      WE_CHAR* pcSrc,
      WE_LONG lSrcLen,
      WE_CHAR* pcDst,
      WE_LONG* plDstLen)
{
    ICSC *pICsc = NULL;
    Fn_We_Charset_Convert *pFn = NULL;     
    if(SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_WECSC, (void**)&pICsc))
    {
        return FALSE;
    }
    pFn = WE_FINDFUNCTIONOFCSC(pICsc,iFromCharset,iToCharset);
    if(NULL == pFn)
    {
        (void)ICSC_RELEASE(pICsc);
        return FALSE;
    }
    if(TRUE != pFn(pICsc,pcSrc,&lSrcLen, pcDst, plDstLen))
    {
        (void)ICSC_RELEASE(pICsc);
        return FALSE;
    }
    (void)ICSC_RELEASE(pICsc);
    return TRUE;

}


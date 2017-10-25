/*=====================================================================================
    FILE NAME :
        oem_seccb.h
    MODULE NAME :
        sec
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification                           Tracking
    Date            Author                 Number        Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-10-19      Zhanghuanqing          none          Init
    
=====================================================================================*/
/*--------------------------START-----------------------------*/


/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef _OEM_SECCB_H
#define _OEM_SECCB_H

/*******************************************************************************
*   Include File Section
*******************************************************************************/

#include "aeeshell.h"           

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define SEC_REGCALLBACK(shell,cb,pfn,data)\
{\
    CALLBACK_Init(&(cb),(pfn),(WE_HANDLE)(data));\
    ISHELL_Resume((shell),&(cb));\
}

/*Cancel Callback*/
#define SEC_CANCELCALLBACK(cb) CALLBACK_Cancel(&cb)

#endif

/*--------------------------END-----------------------------*/
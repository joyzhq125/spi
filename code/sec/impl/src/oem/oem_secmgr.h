/*==================================================================================================
    FILE NAME : oem_secmgr.h
    MODULE NAME : WEA
    
    GENERAL DESCRIPTION
        This file declare API of file manager.
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-01 Alan              none        Initialization
    
==================================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef __OEM_SECMGR__
#define __OEM_SECMGR__

/***************************************************************************************************
*   include File Section
*******************************************************************************/
#include "AEE.h"

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef void (*Fn_WeCallback)
(
    E_WE_EVENT weNotifyType, 
    void* pCbData, 
    WE_HANDLE hWeHandle, 
    WE_HANDLE hWeSubHandle
);    /* Fn_WeCallback prototype */

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
E_WE_ERROR WeMgr_Initialize
(
    IShell *pIShell, 
    IModule *pIModule, 
    WE_HANDLE *phWeHandle
);

E_WE_ERROR WeMgr_Terminate(WE_HANDLE hWeHandle);

E_WE_ERROR WeMgr_RegMsgProcFunc
(
    WE_HANDLE hWeHandle, 
    Fn_WeCallback Fn_cb, 
    void *pPrivData
);




#endif


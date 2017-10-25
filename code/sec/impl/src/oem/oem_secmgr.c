/*=====================================================================================
    FILE NAME : oem_secmgr.c
    MODULE NAME : WEA
    
    GENERAL DESCRIPTION
        This file implement the WEA management.
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-01 Alan              none        Initialization
    
=====================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "we_def.h"
#include "oem_secerr.h"
#include "oem_secevent.h"

#include "AEEShell.h"
#include "oem_secfile.h"
#include "oem_secmgr.h"
#include "oem_sechandle.h"


/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    WeMgr_Initialize
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Initialize IFileMgr Interface object.
ARGUMENTS PASSED:
    IShell  *pIShell: Pointer to the IShell interface
    IModule *pIModule: Pointer to the module
    WE_HANDLE *phWeHandle: Pointer to the IFileMgr Interface object
RETURN VALUE:
    Error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR WeMgr_Initialize(IShell *pIShell, IModule *pIModule, WE_HANDLE *phWeHandle)
{
    St_WeHandle *pstWeHandleData = NULL;

    /* Check the parameter first */
    if(NULL == pIShell || NULL == pIModule || NULL == phWeHandle)
    {
        return E_WE_ERROR_INVALID_PARAM;
    }

    /* Create memory for WeaHandle */
    pstWeHandleData = MALLOC(sizeof(St_WeHandle));
    if(NULL == pstWeHandleData)
    {
        return E_WE_ERROR_RESOURCE_LIMIT;
    }

    /* Initialize WeaHandle */
    (void)MEMSET(pstWeHandleData, 0 ,sizeof(St_WeHandle));

    pstWeHandleData->iMagicNum = WE_HANDLE_MAGIC_NUM;
    pstWeHandleData->pIModule = pIModule;
    pstWeHandleData->pIShell = pIShell;

    if(SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR,
                    (void **)&(pstWeHandleData->pIFileMgr)))
    {
        FREE(pstWeHandleData);
        return E_WE_ERROR_RESOURCE_LIMIT;
    }

    /* Return the pstWeaHandle as WEA_HANDLE */
    *phWeHandle = pstWeHandleData;
    
    return E_WE_OK;
}


/*==================================================================================================
FUNCTION: 
    WeMgr_Terminate
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Close all file and release IfileMgr object.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
RETURN VALUE:
    Error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR WeMgr_Terminate(WE_HANDLE hWeHandle)
{
    St_WeHandle *pstWeHandleData = hWeHandle;
	E_WE_ERROR  eResult = E_WE_OK;

    /* Check the parameter first */
    if(NULL == hWeHandle)
    {
        return E_WE_ERROR_INVALID_PARAM;
    }

    if(pstWeHandleData->iMagicNum != WE_HANDLE_MAGIC_NUM)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }

    /* Terminiate all sub-module */
    eResult = WeFile_CloseAll(hWeHandle);
    
    if(NULL != ((St_WeHandle*)hWeHandle)->pIFileMgr)
    {
        (void)IFILEMGR_Release(((St_WeHandle*)hWeHandle)->pIFileMgr);
        ((St_WeHandle*)hWeHandle)->pIFileMgr = NULL;
    }

    pstWeHandleData->iMagicNum = 0;
    /* Free Data */
    FREE(pstWeHandleData);
    pstWeHandleData = NULL;
    return eResult;
}


/*==================================================================================================
FUNCTION: 
    WeMgr_RegMsgProcFunc
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Regist call back.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    Fn_WeCallback Fn_cb: Call back function
    void *pPrivData: Data to be passed to Call back function
RETURN VALUE:
    Error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR WeMgr_RegMsgProcFunc
(
    WE_HANDLE hWeHandle, 
    Fn_WeCallback Fn_cb, 
    void *pPrivData
)
{
    St_WeHandle *pstWeaHandle = hWeHandle;

    /* Check the parameter first */
    if(NULL == pstWeaHandle)
    {
        return E_WE_ERROR_INVALID_PARAM;
    }

    if(pstWeaHandle->iMagicNum != WE_HANDLE_MAGIC_NUM)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }

    /* Register Handle */
    pstWeaHandle->cbWeCallback = Fn_cb;
    pstWeaHandle->pvPrivData = pPrivData;

    return E_WE_OK;
}



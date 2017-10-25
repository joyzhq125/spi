/*==================================================================================================
    HEADER NAME : we_idea.h
    MODULE NAME : WE
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file, define the initial function prototype for IDEA algorithm, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-11-27    Stone An         None         create we_idea.h
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef WE_IDEA_H
#define WE_IDEA_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define M_WE_IDEA_TYPE           0x135324
#define WE_IDEA_BLOCK_SIZE       8

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 We_LibIDEAEncryptNoPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibIDEAEncryptPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibIDEAEncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibIDEAEncryptInitPad
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibIDEAEncryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibIDEAEncryptUpdatePad
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibIDEAEncryptFinal(WE_HANDLE handle);

WE_INT32 We_LibIDEAEncryptFinalPad
(
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,  
    WE_HANDLE handle
);

WE_INT32 We_LibIDEADecryptNoPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibIDEADecryptPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibIDEADecryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibIDEADecryptInitPad
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibIDEADecryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibIDEADecryptUpdatePad
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibIDEADecryptFinal(WE_HANDLE handle);
WE_INT32 We_LibIDEADecryptFinalPad
( 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);
#endif

/*--------------------------END-----------------------------*/


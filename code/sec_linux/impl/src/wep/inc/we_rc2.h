/*==================================================================================================
    HEADER NAME : we_rc2.h
    MODULE NAME : WE
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file, define the initial function prototype for RC2 algorithm, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-01-14    Stone An         None         create we_rc2.h
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef WE_RC2_H
#define WE_RC2_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define M_WE_RC2_TYPE       0x13532453
#define WE_RC2_BLOCK_SIZE   8

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 We_LibRC2EncryptNoPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibRC2EncryptPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibRC2EncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibRC2EncryptInitPad
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibRC2EncryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibRC2EncryptUpdatePad
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibRC2EncryptFinal(WE_HANDLE handle);

WE_INT32 We_LibRC2EncryptFinalPad
( 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibRC2DecryptNoPadding
( 
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibRC2DecryptPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibRC2DecryptInit
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibRC2DecryptInitPad
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibRC2DecryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibRC2DecryptUpdatePad
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);

WE_INT32 We_LibRC2DecryptFinal(WE_HANDLE handle);

WE_INT32 We_LibRC2DecryptFinalPad
(
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
);
#endif

/*--------------------------END-----------------------------*/


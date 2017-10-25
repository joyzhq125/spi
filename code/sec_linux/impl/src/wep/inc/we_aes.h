/*==================================================================================================
    HEADER NAME : we_aes.h
    MODULE NAME : WE
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the function prototype 
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-11-10      Sam             None         Init
    
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/
#ifndef WE_AES_H
#define WE_AES_H


/***************************************************************************************************
* Macro Define Section
***************************************************************************************************/
/* direction */
#define AES_ENCRYPT 1
#define AES_DECRYPT 0

/* block and maximum NR */
#define AES_MAXNR       14
#define AES_BLOCK_SIZE  16

#define GETU32(pt) (((WE_UINT32)(pt)[0] << 24) ^ ((WE_UINT32)(pt)[1] << 16) ^ ((WE_UINT32)(pt)[2] <<  8) ^ ((WE_UINT32)(pt)[3]))
#define PUTU32(ct, st) { (ct)[0] = (WE_UINT8)((st) >> 24); (ct)[1] = (WE_UINT8)((st) >> 16); (ct)[2] = (WE_UINT8)((st) >>  8); (ct)[3] = (WE_UINT8)(st); }

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef struct tagSt_WeAesKey 
{
    WE_UINT32 auiRoundKey[4 *(AES_MAXNR + 1)];
    WE_INT32  iNRounds;
}St_WeAesKey;

typedef struct tagSt_WeAESParameter
{
    St_WeAesKey     stAESCipherKeyEx;
    WE_INT32        iType;
    WE_UCHAR        aucIv[16];
    WE_UCHAR        aucBuf[16];
    WE_UINT32       uiBufLen;
}St_WeAESParameter;


/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
WE_INT32 We_AESSetEncryptKey
(
    const WE_UINT8  *pucUserKey, 
    const WE_INT32  iNBits,
    St_WeAesKey     *pstAESKey
);

WE_INT32 We_AESSetDecryptKey
(
    const WE_UINT8  *pucUserKey, 
    const WE_INT32  iNBits,
    St_WeAesKey     *pstAESKey
);

WE_VOID We_AESEncrypt
(
    const WE_UINT8  *pucInData, 
    WE_UINT8        *pucOutData,
    const St_WeAesKey *pstAESKey
);


WE_VOID We_AESDecrypt
(
    const WE_UINT8  *pucInData, 
    WE_UINT8        *pucOutData,
    const St_WeAesKey *pstAESKey
);

/* encrypt and decrypt data with the same function, so we have the last param. */
WE_VOID We_AESCBCEncrypt
(
    const St_WeAesKey   *pstAESKey,
    WE_UINT8            *pucIVec, 
    const WE_INT32      enc, 
    const WE_UINT8      *pucInData, 
    const WE_UINT32     uiInDataLen, 
    WE_UINT8            *pucOutData,
    WE_UINT32           *puiOutDataLen
);

WE_VOID We_AESCBCDecrypt
(
    const St_WeAesKey   *pstAESKey,
    WE_UINT8            *pucIVec, 
    const WE_INT32      enc, 
    const WE_UINT8      *pucInData, 
    const WE_UINT32     uiInDataLen, 
    WE_UINT8            *pucOutData,
    WE_UINT32           *puiOutDataLen
);

/* interface */
WE_INT32 We_LibAESEncrypt
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR      *pucIn, 
    WE_UINT32           uiInLen, 
    WE_UCHAR            *pucOut, 
    WE_UINT32           *puiOutLen
);

WE_INT32 We_LibAESEncryptInit
(
    St_WeCipherCrptKey  stKey, 
    WE_HANDLE           *hAESHandle
);

WE_INT32 We_LibAESEncryptUpdate
(
    const WE_UCHAR  *pucIn, 
    WE_UINT32       uiInLen, 
    WE_UCHAR        *pucOut, 
    WE_UINT32       *puiOutLen, 
    WE_HANDLE       hAESHandle
);

WE_INT32 We_LibAESEncryptFinal
(
    WE_HANDLE hAESHandle
);

WE_INT32 We_LibAESEncrypt1
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibAESEncryptInit1
(
    St_WeCipherCrptKey  stKey, 
    WE_HANDLE *hAESHandle
);

WE_INT32 We_LibAESEncryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE hAESHandle
);

WE_INT32 We_LibAESEncryptFinal1
(
    WE_UCHAR    *pucOut, 
    WE_UINT32   *puiOutLen,
    WE_HANDLE   hAESHandle
);

WE_INT32 We_LibAESDecrypt
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibAESDecryptInit
(
    St_WeCipherCrptKey  stKey, 
    WE_HANDLE *hAESHandle
);

WE_INT32 We_LibAESDecryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE hAESHandle
);

WE_INT32 We_LibAESDecryptFinal
(
    WE_HANDLE hAESHandle
);

WE_INT32 We_LibAESDecrypt1
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibAESDecryptInit1
(
    St_WeCipherCrptKey  stKey, 
    WE_HANDLE *hAESHandle
);

WE_INT32 We_LibAESDecryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE hAESHandle
);

WE_INT32 We_LibAESDecryptFinal1
(
    WE_UCHAR* pucOutEndp, 
    WE_UINT32* puiPadLen, 
    WE_HANDLE hAESHandle
);

#endif /* endif WE_AES_H */

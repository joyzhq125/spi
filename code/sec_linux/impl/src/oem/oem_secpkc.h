/*==================================================================================================
    HEADER NAME : oem_secpkc.h
    MODULE NAME : SEC
    
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
    2006-12-5     tang            None         
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef _OEM_SEC_PKC_H 
#define _OEM_SEC_PKC_H

#ifdef WE_RSA_FOR_SEC
/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef enum tagE_SecRsaSigType
{
    E_SEC_RSA_PKCS1_NULL = 0,
    E_SEC_RSA_PKCS1_MD2,
    E_SEC_RSA_PKCS1_MD5,
    E_SEC_RSA_PKCS1_SHA1,
    E_SEC_RSA_PKCS1_PSS_SHA1_MGF1
}E_SecRsaSigType;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 Sec_PKCRsaGenerateKeyPair(WE_HANDLE hSecHandle,
                                   WE_INT32 iTargetID,  
                                   WE_INT32 iModBits);
WE_INT32 Sec_PKCRsaVerifySignature(WE_HANDLE hSecHandle,
                                   WE_INT32 iTargetID,
                                   St_SecRsaPublicKey stKey,
                                   WE_UCHAR* pucMsg, 
                                   WE_INT32 iMsgLen, 
                                   E_SecRsaSigType eType,
                                   const WE_UCHAR* pucSig, 
                                   WE_INT32 iSigLen);

WE_INT32 Sec_PKCRsaComputeHashedSignature(WE_HANDLE hSecHandle,
                                          WE_INT32 iTargetID,                                       
                                          St_SecCrptPrivKey stKey,
                                          const WE_UCHAR* pucData, 
                                          WE_INT32 iDataLen);

WE_INT32 Sec_PKCRsaComputeSignature(WE_HANDLE hSecHandle,
                                    WE_INT32 iTargetID, 
                                    St_SecCrptPrivKey stKey,
                                    E_SecRsaSigType eType,
                                    const WE_UCHAR* pucData, 
                                    WE_INT32 iDataLen);

WE_INT32 Sec_PKCRsaPublicKeyEncryption(WE_HANDLE hSecHandle,
                                       WE_INT32 iTargetID,
                                       St_SecRsaPublicKey stKey, 
                                       const WE_UCHAR* pucData, 
                                       WE_INT32 iDataLen);
WE_INT32 Sec_PKCConvertRSAPubKey(St_SecRsaPublicKey stKey, 
                                 WE_UCHAR** ppucKey, 
                                 WE_INT32* piKeyLen);
#endif /*WE_RSA_FOR_SEC*/

#endif /*_OEM_SEC_PKC_H*/


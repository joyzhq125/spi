/*==================================================================================================
    HEADER NAME : oem_secpkc.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be update later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   steven ding        None         Draft
==================================================================================================*/
/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef _OEM_SECPKC_H
#define _OEM_SECPKC_H

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
#define SEC_MIN_RSA_MODULUS_BITS            508
#define SEC_MAX_RSA_MODULUS_BITS            2048
#define SEC_MAX_RSA_MODULUS_LEN            ((SEC_MAX_RSA_MODULUS_BITS + 7) / 8)
#define SEC_MAX_RSA_PRIME_BITS             ((SEC_MAX_RSA_MODULUS_BITS + 1) / 2)
#define SEC_MAX_RSA_PRIME_LEN              ((SEC_MAX_RSA_PRIME_BITS + 7) / 8)
#define SEC_RSA_OUTPUT_BUF_SIZE            (SEC_MAX_RSA_MODULUS_BITS / 8)

typedef St_SecPubKeyRsa             St_SecRsaPublicKey;

typedef enum tagE_SecRsaSigType
{
    E_SEC_RSA_PKCS1_NULL = 0,
    E_SEC_RSA_PKCS1_MD2,
    E_SEC_RSA_PKCS1_MD5,
    E_SEC_RSA_PKCS1_SHA1,
    E_SEC_RSA_PKCS1_PSS_SHA1_MGF1
}E_SecRsaSigType;

/*get brew ssl root certificate*/
typedef struct tagSt_SeclibCertList       St_SeclibCertList;
struct tagSt_SeclibCertList
{
    WE_UCHAR*           pucCert;
    WE_UINT32           uiCertLen;
    St_SeclibCertList*  pstNext;
};
typedef struct tagSt_SecPKCOutput
{
    WE_HANDLE               hRsa;
    WE_HANDLE               hSecHandle;
    WE_UCHAR                aucIn[SEC_RSA_OUTPUT_BUF_SIZE];
    WE_UCHAR                aucOut[SEC_RSA_OUTPUT_BUF_SIZE];
    WE_INT32                iOutLen;
    WE_UINT32               uiResult;
    AEECallback             stFnc;
    WE_INT32                iTargetID;
    WE_UCHAR                aucHash[20];
    WE_INT32                iHashLen;
}St_SecPKCOutput;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

WE_INT32 Sec_PKCRsaPublicKeyEncryption(WE_HANDLE hSecHandle,
                                       WE_INT32 iTargetID,
                                       St_SecRsaPublicKey stKey, 
                                       const WE_UCHAR* pucData, 
                                       WE_INT32 iDataLen);

WE_INT32 Sec_PKCRsaPrivateKeyDecryption(WE_HANDLE hSecHandle,
                                        WE_INT32 iTargetID,
                                        St_SecCrptPrivKey stKey,
                                        const WE_UCHAR* pucData, 
                                        WE_INT32 iDataLen);

WE_INT32 Sec_PKCRsaComputeHashedSignature(WE_HANDLE hSecHandle,
                                          WE_INT32 iTargetID,                                       
                                          St_SecCrptPrivKey stKey,
                                          const WE_UCHAR* pucData, 
                                          WE_INT32 iDataLen);

WE_INT32 Sec_PKCRsaVerifySignature(WE_HANDLE hSecHandle,
                                   WE_INT32 iTargetID,
                                   St_SecRsaPublicKey stKey,
                                   WE_UCHAR* pucMsg, 
                                   WE_INT32 iMsgLen, 
                                   E_SecRsaSigType eType,
                                   const WE_UCHAR* pucSig, 
                                   WE_INT32 iSigLen);

WE_INT32 Sec_PKCRsaGenerateKeyPair(WE_HANDLE hSecHandle,
                                   WE_INT32 iTargetID,  
                                   WE_INT32 iModBits);

WE_INT32 Sec_PKCRsaGenerateKeyPairSyn(WE_HANDLE hSecHandle,
                                      WE_INT32 iTargetID, 
                                      WE_UCHAR** ppucPubkey, 
                                      WE_INT32* piPubkeyLen, 
                                      WE_UCHAR** ppucPrivkey, 
                                      WE_INT32* piPrivkeyLen,
                                      WE_INT32 iModBits);

WE_INT32 Sec_PKCRsaPrivateDecryptionBasic(WE_HANDLE hSecHandle,
                                          WE_INT32 iTargetID,
                                          WE_UCHAR* pucKey, 
                                          WE_INT32 iKeyLen,
                                          const WE_UCHAR *pucData, 
                                          WE_INT32 iDataLen);

WE_INT32 Sec_PKCRsaComputeSignatureBasic(WE_HANDLE hSecHandle,
                                         WE_INT32 iTargetID, 
                                         WE_UCHAR* pucKey, 
                                         WE_INT32 iKeyLen,
                                         E_SecRsaSigType eType,
                                         const WE_UCHAR* pucData, 
                                         WE_INT32 iDataLen);

WE_INT32 Sec_PKCRsaComputeSignature(WE_HANDLE hSecHandle,
                                    WE_INT32 iTargetID, 
                                    St_SecCrptPrivKey stKey,
                                    E_SecRsaSigType eType,
                                    const WE_UCHAR* pucData, 
                                    WE_INT32 iDataLen);

WE_INT32 Sec_TestBuildCertReqInfo(St_SecRsaPublicKey stKey,
                                  WE_UCHAR** ppucInfo,WE_INT32* piInfoLen);

WE_INT32 Sec_TestBuildCertReqMsg(WE_UCHAR* pucInfo, WE_INT32 iInfoLen,
                                 WE_UCHAR* pucShaSig, WE_INT32 iShaSigLen,
                                 WE_UCHAR** ppucMsg, WE_INT32* piMsgLen);

/*************************************************************************************************
get brew ssl root certificate
*************************************************************************************************/

WE_INT32 Sec_LibGetBrewSSLRootCerts(WE_HANDLE hSecHandle, 
                                   St_SeclibCertList* pstCertList,
                                   WE_HANDLE* pHandle);

WE_VOID Sec_LibFreeBrewSSLRootCerts(St_SeclibCertList* pstCertList, WE_HANDLE handle);


/*only for sec_lib.c and sec_pkc.c*/
WE_INT32 Sec_ConvertBrewErrToSecErr(WE_INT32 iError);  

WE_INT32 Sec_PKCConvertRSAPubKey(St_SecRsaPublicKey stKey, 
                                     WE_UCHAR** ppucKey, 
                                     WE_INT32* piKeyLen);
#endif























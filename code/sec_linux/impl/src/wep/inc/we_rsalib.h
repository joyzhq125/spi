/*==================================================================================================
    HEADER NAME : we_rsalib.h
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
    2006-12-5     tang              None         
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef RSA_LIB_H
#define RSA_LIB_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

/************************************************************************/
/* Error code                                                                     */
/************************************************************************/
#if 0
/*for WE_RSA_FOR_SEC*/
#define M_WE_LIB_ERR_OK                               0x00     
#define M_WE_LIB_ERR_BUFFER_TOO_SMALL                 0x02         
#define M_WE_LIB_ERR_UNSUPPORTED_METHOD               0x03       
#define M_WE_LIB_ERR_INSUFFICIENT_MEMORY              0x04   
#define M_WE_LIB_ERR_NOT_INITIALISED                  0x05
#define M_WE_LIB_ERR_NOT_IMPLEMENTED                  0x07          
#define M_WE_LIB_ERR_INVALID_PARAMETER                0x08        
#define M_WE_LIB_ERR_DATA_LENGTH                      0x09
#define M_WE_LIB_ERR_KEY_LENGTH                       0x0b 
#endif

#define M_WE_LIB_ERR_GENERAL_ERROR                    0x01
#define M_WE_LIB_ERR_INVALID_KEY                      0x10


/************************************************************************
*   interface                                                                     
************************************************************************/
#define SEC_RSA_ENCRYPTION                      SEClib_RsaPublicKeyEncryption
#define SEC_RSA_DECRYPTION                      SEClib_RsaPrivateKeyDecryption
#define SEC_RSA_COMPUTE_HASHED_SIGN             SEClib_ComputeRSAHashedSignature
#define SEC_RSA_COMPUTE_DIGITAL_SIGN            SEClib_computeRSADigitalSignature
#define SEC_RSA_VERIFY_SIGNATURE                SEClib_VerifyRSASignature
#define SEC_RSA_GENERATE_KEY_PAIR               SEClib_GenerateRSAKeyPair



/************************************************************************
*   type define section                                                                     
************************************************************************/
#ifndef WE_RSA_FOR_SEC
typedef struct tagSt_SecCrptPubKeyRsa
{
    WE_UINT16       usExpLen;
    WE_UINT8        *pucExponent;
    WE_UINT16       usModLen;
    WE_UINT8        *pucModulus;
} St_SecCrptPubKeyRsa;

typedef struct tagSt_SecCrptPrivKey
{
    WE_UINT16    usBufLen;
    WE_UCHAR    * pucBuf;
}St_SecCrptPrivKey;

typedef St_SecCrptPubKeyRsa             St_SecPubKeyRsa;
typedef St_SecPubKeyRsa                 St_SecRsaPublicKey;
#endif


/*the type of signature*/
#define G_WE_RSA_SIGNTYPE_PKCS1_NULL    0x00
#define G_WE_RSA_SIGNTYPE_PKCS1_MD2     0x01
#define G_WE_RSA_SIGNTYPE_PKCS1_MD5     0x02
#define G_WE_RSA_SIGNTYPE_PKCS1_SHA     0x03


/************************************************************************/
/*      Only used for Lib :Interface                                                                */
/************************************************************************/
WE_INT32 SEClib_RsaPublicKeyEncryption(WE_HANDLE hWeHandle,St_SecCrptPubKeyRsa stPubKey, 
                                       const WE_UINT8 *pucData, WE_INT32 iDataLen, 
                                       WE_UINT8 *pucOut, WE_INT32 *piOutLen);

WE_INT32 SEClib_RsaPrivateKeyDecryption(WE_HANDLE hWeHandle, St_SecCrptPrivKey stKey,
                                        const WE_UCHAR *pucData, 
                                        WE_INT32 iDataLen,
                                        WE_UINT8 *pucBuf, 
                                        WE_INT32 *piBufLen);

WE_INT32 SEClib_VerifyRSASignature(WE_HANDLE hWeHandle,St_SecCrptPubKeyRsa stPubKey, 
                                   WE_UINT8 *pucMsg, 
                                   WE_INT32 iMsgLen,
                                   E_WeRsaSigType eType,
                                   const WE_UINT8 *pucSign,
                                   WE_INT32 iSignLen);

WE_INT32 SEClib_computeRSADigitalSignature(WE_HANDLE hWeHandle,St_SecCrptPrivKey stKey, 
                                           E_WeRsaSigType eType,
                                           const WE_UINT8 *pucBuf, 
                                           WE_INT32 iBufLen, 
                                           WE_UINT8 *pucSign, 
                                           WE_INT32 *puiSignLen);

WE_INT32 SEClib_ComputeRSAHashedSignature(WE_HANDLE hWeHandle,St_SecCrptPrivKey stKey,
                                          const WE_UCHAR *pucData,
                                          WE_INT32 iDataLen,
                                          WE_UINT8 *pucSign, 
                                          WE_INT32 *puiSignLen);

WE_INT32 SEClib_GenerateRSAKeyPair(WE_HANDLE hWeHandle,WE_UCHAR **ppucPubkey, 
                                   WE_INT32 *piPubkeyLen, 
                                   WE_UCHAR **ppucPrivkey, 
                                   WE_INT32 *piPrivkeyLen,
                                   WE_INT32 iModBits);

/************************************************************************/
/*      only used for oem_secpkc.c and we_kpg.c                                                                */
/************************************************************************/
WE_INT32 Sec_GetHashType(E_WeRsaSigType eType,E_WeHashAlgType *peHashType,WE_INT32 *piDigLen);
WE_INT32 We_KpgConvertRSAPubKey(St_SecRsaPublicKey stKey, 
                                WE_UCHAR** ppucKey, WE_INT32* piKeyLen);
WE_INT32 We_KpgConvertRSAPrivKey(St_SecRsaPrivateKey stKey, 
                                 WE_UCHAR** ppucKey, 
                                 WE_INT32* piKeyLen);
WE_INT32 We_KpgGenerateRSAKeyPairBasic( WE_HANDLE hWeHandle,St_SecRsaBasicKeyPair* pstKey, WE_INT32 iModBits);




#endif /*RSA_LIB_H*/


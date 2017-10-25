/*=====================================================================================
    HEADER NAME : we_libalg.h
    MODULE NAME : WE

    GENERAL DESCRIPTION
        In this file,define the struct and function prototype. And also define 
        the interface called by extern module.

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification              Tracking
    Date            Author      Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-11-03       Tang         none      Init
  
=====================================================================================*/
/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef  WE_LIBALG_H
#define  WE_LIBALG_H


/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
/* error code */
#define M_WE_LIB_ERR_OK                               0x00      
#define M_WE_LIB_ERR_GENERAL_ERROR                    0x01
#define M_WE_LIB_ERR_BUFFER_TOO_SMALL                 0x02         
#define M_WE_LIB_ERR_UNSUPPORTED_METHOD               0x03       
#define M_WE_LIB_ERR_INSUFFICIENT_MEMORY              0x04   
#define M_WE_LIB_ERR_NOT_INITIALISED                  0x05
#define M_WE_LIB_ERR_NOT_IMPLEMENTED                  0x07          
#define M_WE_LIB_ERR_INVALID_PARAMETER                0x08        
#define M_WE_LIB_ERR_DATA_LENGTH                      0x09
#define M_WE_LIB_ERR_INVALID_KEY                      0x10
#define M_WE_LIB_ERR_KEY_LENGTH                       0x0b 

#define WE_LIB_FREEIF(p)            {if((p)) {WE_FREE((void*)(p));(p) = NULL;}}
#define WE_LIB_FREE(p)              {WE_FREE((void*)(p));(p) = NULL;}


/* If we run the program in C platform, we could use the global value, 
 * we will define this macro. But when we use BREW platform for integration 
 * test, we should not define this macro. */
//#define GLOBAL_VALUE

/*************************************************************************************************
cipher crypt Initialise:alloc resource, we should call this function 
before using we_alg functions;and when we don't use we_alg functions, 
cipher terminate should be called.
*************************************************************************************************/

#define WE_CRPT_INITIALISE                          We_LibCryptInitialise
#define WE_CRPT_TERMINATE                           We_LibCryptTerminate

/* interface of block cipher encryption/decryption. */
#define WE_CRPT_ENCRYPT_NO_PADDING                  We_LibCipherEncryptNoPadding
#define WE_CRPT_DECRYPT_NO_PADDING                  We_LibCipherDecryptNoPadding
#define WE_CRPT_ENCRYPT_INIT_NO_PADDING             We_LibCipherEncryptInitNoPadding
#define WE_CRPT_ENCRYPT_UPDATE_NO_PADDING           We_LibCipherEncryptUpdateNoPadding
#define WE_CRPT_ENCRYPT_FINAL_NO_PADDING            We_LibCipherEncryptFinalNoPadding
#define WE_CRPT_DECRYPT_INIT_NO_PADDING             We_LibCipherDecryptInitNoPadding
#define WE_CRPT_DECRYPT_UPDATE_NO_PADDING           We_LibCipherDecryptUpdateNoPadding
#define WE_CRPT_DECRYPT_FINAL_NO_PADDING            We_LibCipherDecryptFinalNoPadding
#define WE_CRPT_ENCRYPT_RFC2630_PADDING             We_LibCipherEncryptRFC2630Padding
#define WE_CRPT_DECRYPT_RFC2630_PADDING             We_LibCipherDecryptRFC2630Padding
#define WE_CRPT_ENCRYPT_INIT_RFC2630_PADDING        We_LibCipherEncryptInitRFC2630Padding
#define WE_CRPT_ENCRYPT_UPDATE_RFC2630_PADDING      We_LibCipherEncryptUpdateRFC2630Padding
#define WE_CRPT_ENCRYPT_FINAL_RFC2630_PADDING       We_LibCipherEncryptFinalRFC2630Padding
#define WE_CRPT_DECRYPT_INIT_RFC2630_PADDING        We_LibCipherDecryptInitRFC2630Padding
#define WE_CRPT_DECRYPT_UPDATE_RFC2630_PADDING      We_LibCipherDecryptUpdateRFC2630Padding
#define WE_CRPT_DECRYPT_FINAL_RFC2630_PADDING       We_LibCipherDecryptFinalRFC2630Padding
#define WE_CRPT_HASH                                We_LibHash
#define WE_CRPT_HASH_INIT                           We_LibHashInit
#define WE_CRPT_HASH_UPDATE                         We_LibHashUpdate
#define WE_CRPT_HASH_FINAL                          We_LibHashFinal
#define WE_CRPT_HMAC                                We_LibHmac
#define WE_CRPT_HMAC_INIT                           We_LibHmacInit
#define WE_CRPT_HMAC_UPDATE                         We_LibHmacUpdate
#define WE_CRPT_HMAC_FINAL                          We_LibHmacFinal
#define WE_CRPT_GENERATE_RANDOM                     We_LibGenerateRandom

/* interface of stream cipher encryption/decryption. */
//#define WE_CRPT_ENCRYPT_STREAM                      We_LibCipherEncryptStream
//#define WE_CRPT_DECRYPT_STREAM                      We_LibCipherDecryptStream
//#define WE_CRPT_ENCRYPT_INIT_STREAM                 We_LibCipherEncryptInitStream
//#define WE_CRPT_ENCRYPT_UPDATE_STREAM               We_LibCipherEncryptUpdateStream
//#define WE_CRPT_ENCRYPT_FINAL_STREAM                We_LibCipherEncryptFinalStream
//#define WE_CRPT_DECRYPT_INIT_STREAM                 We_LibCipherDecryptInitStream
//#define WE_CRPT_DECRYPT_UPDATE_STREAM               We_LibCipherDecryptUpdateStream
//#define WE_CRPT_DECRYPT_FINAL_STREAM                We_LibCipherDecryptFinalStream

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
/* Cipher Algorithm Type */
typedef enum tagE_WeCipherAlgType
{
    E_WE_ALG_CIPHER_NULL = 0x00,
    E_WE_ALG_CIPHER_RC5_CBC_40 = 0x01,
    E_WE_ALG_CIPHER_RC5_CBC_56  = 0x02,
    E_WE_ALG_CIPHER_RC5_CBC = 0x03,    
    E_WE_ALG_CIPHER_DES_CBC_40 = 0x04,
    E_WE_ALG_CIPHER_DES_CBC = 0x05,
    E_WE_ALG_CIPHER_3DES_CBC = 0x06,
    E_WE_ALG_CIPHER_IDEA_CBC_40 = 0x07,
    E_WE_ALG_CIPHER_IDEA_CBC_56 = 0x08,
    E_WE_ALG_CIPHER_IDEA_CBC = 0x09,
    E_WE_ALG_CIPHER_RC5_CBC_64 = 0x0a,
    E_WE_ALG_CIPHER_IDEA_CBC_64 = 0x0b,
    E_WE_ALG_CIPHER_RC2_CBC_40 = 0x14,
    E_WE_ALG_CIPHER_RC4_STR_40 = 0x15,
    E_WE_ALG_CIPHER_RC4_STR_128 = 0x16,
    E_WE_ALG_CIPHER_AES_CBC_128 = 0x1e
}E_WeCipherAlgType;

/* hash algorithm type. */
typedef enum tagE_WeHashAlgType
{
    E_WE_ALG_HASH_NULL          =  0x00,
    E_WE_ALG_HASH_MD5           = 0x01,   
    E_WE_ALG_HASH_SHA1          = 0x02,
    E_WE_ALG_HASH_MD2           = 0x03
}E_WeHashAlgType;

/* hmac algorithm type. */
typedef enum tagE_WeHmacAlgType
{
    E_WE_ALG_HMAC_NULL         = 0x00,
    E_WE_ALG_HMAC_MD5           = 0x01,
    E_WE_ALG_HMAC_SHA1          = 0x02,
    E_WE_ALG_HMAC_MD2           = 0x03
}E_WeHmacAlgType;

/* structure of cipher key */
typedef struct tagSt_WeCipherCrptKey
{ 
    WE_UCHAR*         pucKey; 
    WE_INT32          iKeyLen; 
    WE_UCHAR*         pucIv; 
    WE_INT32          iIvLen; 
} St_WeCipherCrptKey;

/* used to check for poionter. */
#define CHECK_FOR_NULL_ARG(arg,err) \
{ if (!(arg)) { \
    return err; \
} \
}
/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

/* initialise and terminate */
WE_INT32 We_LibCryptInitialise(WE_HANDLE hHandle,WE_HANDLE *phHandle);
WE_INT32 We_LibCryptTerminate(WE_HANDLE hHandle);

/*hash*/
WE_INT32 We_LibHash
(
    WE_HANDLE hWeHandle,
    E_WeHashAlgType eAlg,
    const WE_UCHAR  *pucData, 
    WE_INT32        iDataLen, 
    WE_UCHAR        *pucDigest, 
    WE_INT32        *piDigLen
);

WE_INT32 We_LibHashInit
(
    WE_HANDLE hWeHandle,
    E_WeHashAlgType eAlg, 
    WE_HANDLE       *hHashHandle
);

WE_INT32 We_LibHashUpdate
(
    WE_HANDLE hWeHandle,
    const WE_UINT8  *pucDataPart, 
    WE_INT32        iPartLen,
    WE_HANDLE       hHashHandle
);

WE_INT32 We_LibHashFinal
(
    WE_HANDLE hWeHandle,
    WE_UCHAR    *pucDigest, 
    WE_INT32    *piDigestLen,  
    WE_HANDLE   hHashHandle 
);

/*hmac*/
WE_INT32 We_LibHmac
(
    WE_HANDLE hWeHandle,
    E_WeHashAlgType eAlg,
    const WE_UCHAR  *pucKey, 
    WE_INT32        iKeyLen,
    const WE_UCHAR  *pucData, 
    WE_INT32        iDataLen, 
    WE_UCHAR        *pucDig, 
    WE_INT32        *piDigLen
 );

WE_INT32 We_LibHmacInit
(
    WE_HANDLE hWeHandle,
    E_WeHashAlgType eAlg,
    const WE_UINT8  *pucKey, 
    WE_INT32        iKeyLen,
    WE_HANDLE       *phPtrHandle
);

WE_INT32 We_LibHmacUpdate
(
    WE_HANDLE hWeHandle,
    const WE_UCHAR  *pucDataPart, 
    WE_INT32        iPartLen,
    WE_HANDLE       handle
);

WE_INT32 We_LibHmacFinal
(
    WE_HANDLE hWeHandle,
    WE_UCHAR    *pucDigest, 
    WE_INT32    *piDigestLen, 
    WE_HANDLE   handle
);

/************************************************************************/
/* crypt                                                                     */
/************************************************************************/
WE_INT32 We_LibCipherEncryptNoPadding
(
    WE_HANDLE hWeHandle,
    E_WeCipherAlgType   eAlg, 
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR      *pucIn, 
    WE_UINT32           uiInLen, 
    WE_UCHAR            *pucOut, 
    WE_UINT32           *puiOutLen
);

WE_INT32 We_LibCipherDecryptNoPadding
(
    WE_HANDLE hWeHandle,
    E_WeCipherAlgType   eAlg, 
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR      *pucIn, 
    WE_UINT32           uiInLen, 
    WE_UCHAR            *pucOut, 
    WE_UINT32           *puiOutLen
);


WE_INT32 We_LibCipherEncryptRFC2630Padding(WE_HANDLE hWeHandle,
                                           E_WeCipherAlgType eAlg, 
                                           St_WeCipherCrptKey stKey, 
                                           const WE_UCHAR* pucIn, 
                                           WE_UINT32 uiInLen, 
                                           WE_UCHAR* pucOut, 
                                           WE_UINT32* puiOutLen);

WE_INT32 We_LibCipherDecryptRFC2630Padding(WE_HANDLE hWeHandle,
                                           E_WeCipherAlgType eAlg, 
                                           St_WeCipherCrptKey stKey, 
                                           const WE_UCHAR* pucIn, 
                                           WE_UINT32 uiInLen, 
                                           WE_UCHAR* pucOut, 
                                           WE_UINT32* puiOutLen);

WE_INT32 We_LibCipherEncryptInitNoPadding(WE_HANDLE hWeHandle,
                                          E_WeCipherAlgType eAlg, 
                                          St_WeCipherCrptKey stKey,
                                          WE_HANDLE* pHandle);

WE_INT32 We_LibCipherEncryptUpdateNoPadding(WE_HANDLE hWeHandle,
                                            const WE_UCHAR* pucDataPartIn, 
                                            WE_UINT32 uiPartInLen,
                                            WE_UCHAR* pucOut, 
                                            WE_UINT32* puiOutLen, 
                                            WE_HANDLE handle);

WE_INT32 We_LibCipherEncryptFinalNoPadding(WE_HANDLE hWeHandle,WE_HANDLE handle);

WE_INT32 We_LibCipherDecryptInitNoPadding(WE_HANDLE hWeHandle,
                                          E_WeCipherAlgType eAlg, 
                                          St_WeCipherCrptKey stKey,
                                          WE_HANDLE* pHandle);

WE_INT32 We_LibCipherDecryptUpdateNoPadding(WE_HANDLE hWeHandle,
                                            const WE_UCHAR* pucDataPartIn, 
                                            WE_UINT32 uiPartInLen, 
                                            WE_UCHAR* pucOut, 
                                            WE_UINT32* puiOutLen, 
                                            WE_HANDLE handle);

WE_INT32 We_LibCipherDecryptFinalNoPadding(WE_HANDLE hWeHandle,WE_HANDLE handle);

WE_INT32 We_LibCipherEncryptInitRFC2630Padding(WE_HANDLE hWeHandle,
                                               E_WeCipherAlgType eAlg, 
                                               St_WeCipherCrptKey stKey, 
                                               WE_HANDLE* pHandle);

WE_INT32 We_LibCipherEncryptUpdateRFC2630Padding(WE_HANDLE hWeHandle,
                                                 const WE_UCHAR * pucDataPartIn, 
                                                 WE_UINT32 uiPartInLen, 
                                                 WE_UCHAR* pucOut, 
                                                 WE_UINT32* puiOutLen,
                                                 WE_HANDLE handle);


WE_INT32 We_LibCipherEncryptFinalRFC2630Padding(WE_HANDLE hWeHandle,WE_UINT8 *pucOut, WE_UINT32* puiOutLen, WE_HANDLE handle);


WE_INT32 We_LibCipherDecryptInitRFC2630Padding(WE_HANDLE hWeHandle,
                                               E_WeCipherAlgType eAlg, 
                                               St_WeCipherCrptKey stKey,
                                               WE_HANDLE* pHandle);

WE_INT32 We_LibCipherDecryptUpdateRFC2630Padding(WE_HANDLE hWeHandle,
                                                 const WE_UCHAR* pucDataPartIn, 
                                                 WE_UINT32 uiPartInLen, 
                                                 WE_UCHAR* pucOut, 
                                                 WE_UINT32* puiOutLen,
                                                 WE_HANDLE handle);

WE_INT32 We_LibCipherDecryptFinalRFC2630Padding(WE_HANDLE hWeHandle,WE_UCHAR* pucOut, 
                                                WE_UINT32* puiOutLen,WE_HANDLE handle);

WE_INT32 We_LibGenerateRandom(WE_HANDLE hHandle, WE_INT8 *pcRandomData, WE_INT32 iRandomLen);

#endif/* endif WE_LIBALG_H */



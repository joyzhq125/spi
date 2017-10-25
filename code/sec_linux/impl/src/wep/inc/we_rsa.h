/*==================================================================================================
    HEADER NAME : we_rsa.h
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
    2006-12-5    tang            None         
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef _WE_RSA_H
#define _WE_RSA_H

/*decide this rsa alg used for sec or others:"WE_RSA_FOR_SEC"for SEC,else for other*/
#define WE_RSA_FOR_SEC

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
#define M_WE_RSA_SELF_FREE(p)\
{\
    if(NULL != p)\
    {\
        WE_FREE(p);\
        (p) = NULL;\
    }\
}

#define SEC_MIN_RSA_MODULUS_BITS            508
#define SEC_MAX_RSA_MODULUS_BITS            2048
#define SEC_MAX_RSA_MODULUS_LEN             ((SEC_MAX_RSA_MODULUS_BITS + 7) / 8)
#define SEC_MAX_RSA_PRIME_BITS              ((SEC_MAX_RSA_MODULUS_BITS + 1) / 2)
#define SEC_MAX_RSA_PRIME_LEN               ((SEC_MAX_RSA_PRIME_BITS + 7) / 8)
#define SEC_RSA_OUTPUT_BUF_SIZE             (SEC_MAX_RSA_MODULUS_BITS / 8)

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_SecRsaPrivateKey
{
    WE_UCHAR        aucModulus[SEC_MAX_RSA_PRIME_LEN];  
    WE_UINT16       usModLen;
    WE_UCHAR        aucPubExp[3];
    WE_UINT16       usPubExpLen;
    WE_UCHAR        aucExponent[SEC_MAX_RSA_PRIME_LEN];
    WE_UINT16       usExpLen;    
    WE_UCHAR        aucPrime[2][SEC_MAX_RSA_PRIME_LEN]; 
    WE_UINT16       usPrimeLen;
    WE_UCHAR        aucPrimeExp[2][SEC_MAX_RSA_PRIME_LEN];
    WE_UINT16       usPrimeExpLen;
    WE_UCHAR        aucCoef[SEC_MAX_RSA_PRIME_LEN];
    WE_UINT16       usCoefLen;
}St_SecRsaPrivateKey;

typedef enum tagE_WeRsaSigType
{
    E_WE_RSA_PKCS1_NULL = 0x00,
    E_WE_RSA_PKCS1_MD2,
    E_WE_RSA_PKCS1_MD5,
    E_WE_RSA_PKCS1_SHA1
}E_WeRsaSigType;

/* pad_key_type */
typedef enum tagE_PadKeyType
{
    E_PAD_KEY_TYPE_DEFAULT, 
    E_PAD_PRIVATE, 
    E_PAD_PUBLIC
} E_PadKeyType;

typedef struct tagSt_PadParams 
{
    E_PadKeyType   eKeyType;
}St_PadParams;

/* pad_algn */
typedef enum tagE_RsaPadMethod
{
    E_RSA_PAD_NONE,
    E_RSA_PAD_BDES,
    E_RSA_PAD_BRSA,
    E_RSA_PAD_PKCS1,
    E_RSA_PAD_PKCS5,
    E_RSA_PAD_PKCS1NULL,
    E_RSA_PAD_ISO9796,
    E_RSA_PAD_BRSAORIG,
    E_RSA_PAD_AUTOD1PKCS,
    E_RSA_PAD_AUTOD1BRSA,
    E_RSA_PAD_OAEPPKCS1,
    E_RSA_PAD_OAEPSET1,
} E_RsaPadMethod;

typedef enum tagE_Result
{
    E_FALSE, 
    E_TRUE     
} E_Result;

typedef struct tagSt_UtMblk
{
    WE_UINT32   uiSize;
    WE_UINT32   uiUsed;
    WE_UINT8    *pucData;
} St_UtMblk;

typedef struct tagSt_UtMblkListS * pstUtMblkList;

typedef struct tagSt_UtMblkListS 
{
    pstUtMblkList     pstPrev;
    pstUtMblkList     pstNext;
    St_UtMblk         stBlock;
} st_UtMblkListS, *P_St_UtMblkLists;

typedef enum tagE_RsaKTag
{
    E_RSA_KTAG_SIMPLE_KEY, 
    E_RSA_KTAG_CRT_KEY     
} E_RsaKTag;

typedef struct tagSt_SimpleKey 
{        
    WE_VOID           *pvModulus;
    WE_VOID           *pvExpon;
    WE_UINT32         uiFermatNo;             
} St_SimpleKey;

typedef struct tagSt_CrtKey
{        
    WE_VOID        *pvPFactor;
    WE_VOID        *pvQFactor;
    WE_VOID        *pvPExpon;
    WE_VOID        *pvQExpon;
    WE_VOID        *pvConvConst;
    WE_VOID        *pvModulus;
    WE_VOID        *pvExpon;
    WE_VOID        *pub_expon; 
} St_CrtKey;

typedef union tagSt_KeyTypeUnion
{
    St_SimpleKey        stSimpleKey;        
    St_CrtKey           stCrtKey;
} St_KeyTypeUnion;

typedef struct tagSt_RsaKey
{
    WE_UINT32              uiBits;         
    E_RsaKTag              eKeyTypeTag; 
    St_KeyTypeUnion        stKeyTypeUnion;
} St_RsaKey;


#ifdef WE_RSA_FOR_SEC
/*modify linux by Bird 070313*/
//typedef St_SecCrptPubKeyRsa             St_SecPubKeyRsa;
typedef St_SecCrptPubKeyRsa                 St_SecRsaPublicKey;

#endif

typedef St_SecRsaPrivateKey             St_SecRsaBasicKeyPair;

typedef enum tagE_KsetDataType
{
    E_DATA_TYPE_DEFAULT, 
    E_DATA_TYPE_DATA, 
    E_DATA_TYPE_HASH
} E_KsetDataType;

typedef struct tagSt_PadAlg 
{
    E_RsaPadMethod  ePadMethod;
    WE_INT32        iNullByte;
} St_PadAlg;

typedef enum tagE_WeBsapiModeOp
{
    E_WE_RSA_ENCRYPT, 
    E_WE_RSA_DECRYPT, 
    E_WE_RSA_SIGN, 
    E_WE_RSA_VERIFY
} E_WeRsaModeOp; 

typedef enum tagE_WeRsaModeCBC
{
    E_WE_RSA_MODE_ECB, 
    E_WE_RSA_MODE_CBC
} E_WeRsaModeCBC;

typedef enum tagE_WeKSetKeyType
{
    E_RSA_KEY_TYPE_DEFAULT, 
    E_RSA_KEY_TYPE_SECRET, 
    E_RSA_KEY_TYPE_PUBLIC
} E_RsaKSetKeyType;

#define RSA_KEY_TYPE_PRIVATE E_RSA_KEY_TYPE_SECRET

typedef struct tagSt_RsaMode 
{
    E_WeRsaModeOp      eOp;
    E_WeRsaModeCBC     eMode;
} st_RsaMode ;

typedef enum tagE_WeHashType
{
    E_HASH_NUMERIC,
    E_HASH_BITS
}E_RsaHashType;

typedef struct tagSt_SigType 
{
    E_RsaHashType         eHashType;
    E_WeRsaSigType       eSigType;
} St_SigType ;

typedef struct tagSt_RsaHashValue
{
    WE_UINT32   uiSize;
    WE_UINT8    *pucData;
}St_RsaHashValue;

typedef struct tagSt_HashDigest 
{
    St_RsaHashValue  stHashValue;
    WE_VOID         *pvNum;
}St_RsaHashDigest;

typedef struct tagSt_RsaKeySet 
{
    St_UtMblk              *pstKey;
    St_PadAlg              stPadAlg;
    St_PadParams           *pstPadParam;
    St_SigType             stSigType;
    WE_UINT32              uiHashBits;
    E_KsetDataType         eDataType;  
    E_RsaKSetKeyType       eKeyType;  
} St_RsaKeySet;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_VOID   *We_RsaCrypt(St_RsaKey *pstRSAKey, WE_VOID * pvMessArc);
WE_INT32  We_RsaStart(WE_HANDLE hWeHandle,st_RsaMode *pstMode, St_RsaKeySet  *pstKeyRefPar,WE_HANDLE *pHandle);
WE_INT32  We_RsaAppend(WE_HANDLE hWeHandle,WE_UINT8 *pucData, WE_UINT32 uiLength, WE_VOID * pvHandle);
WE_INT32  We_RsaFinish(WE_HANDLE hWeHandle,WE_HANDLE Handle, P_St_UtMblkLists *pstData);
WE_VOID   We_RsaRelease(WE_VOID * pvHandle);
WE_VOID   We_RsaDisposeKey(St_RsaKey *pstRSAkey);
WE_VOID   We_RsaDisposeMblkList(pstUtMblkList pstList);

#endif /*_WE_RSA_H*/


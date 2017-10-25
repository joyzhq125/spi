/*=====================================================================================
    FILE   NAME : we_libalg.c
    MODULE NAME : WE

    GENERAL DESCRIPTION
        define the interface function, include hash, hmac, block and 
        stream cipher.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History

    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-11-03         Sam         None          Init

=====================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_hash.h"
#include "we_des.h"
#include "we_aes.h"
#include "we_rc4.h"
#include "we_rc2.h"
#include "we_idea.h"
#include "we_rnd.h"
#include "we_rc5.h"

#define TEST_FOR_RANDOM_SEED
#ifdef TEST_FOR_RANDOM_SEED
#include "time.h"
#endif

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
/* data handle. */
typedef struct tagSt_WeLibDataRecord
{
    WE_INT32 ucFlag;
    WE_HANDLE hDataHandle;
}St_WeLibDataRecord;


/*******************************************************************************
*   Global Variable Declare Section
*******************************************************************************/
#ifdef GLOBAL_VALUE
St_WeLibPrngInstS *pstLibPrng = NULL;
WE_INT32 iInitialise = 0;
static WE_INT32 We_LibPRNGInit(void);
#else
static WE_INT32 We_LibPRNGInit(WE_HANDLE *hPrng);
#endif


/* crypto: stream. */
static WE_INT32 We_LibCipherEncryptStream
(
    E_WeCipherAlgType iAlg,
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen
);

static WE_INT32 We_LibCipherDecryptStream
(
    E_WeCipherAlgType iAlg, 
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen
);

#if 0
static WE_INT32 We_LibCipherEncryptInitStream
(
    E_WeCipherAlgType eAlg,
    St_WeCipherCrptKey stKey,
    WE_HANDLE *pHandle
);


static WE_INT32 We_LibCipherDecryptInitStream
(
    E_WeCipherAlgType eAlg,
    St_WeCipherCrptKey stKey,
    WE_HANDLE *pHandle
);

static WE_INT32 We_LibCipherEncryptUpdateStream
(
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen, 
    WE_HANDLE handle
);

static WE_INT32 We_LibCipherDecryptUpdateStream
(
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen, 
    WE_HANDLE handle
);

static WE_INT32 We_LibCipherEncryptFinalStream(WE_HANDLE handle);

static WE_INT32 We_LibCipherDecryptFinalStream(WE_HANDLE handle);
#endif 

/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibGetSeed
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the seed value.
ARGUMENTS PASSED:
    pucSeed[IO]: the seed value.
    puiSeedLen[IO]: the seed length.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    if defined the macro TEST_FOR_RANDOM_SEED, get the random seed.
=====================================================================================*/
/* SECa_seed */
WE_INT32 We_LibGetSeed(WE_UCHAR *pucSeed, WE_INT32 *puiSeedLen)
{
    if((pucSeed==NULL)||(puiSeedLen==NULL))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
#ifndef TEST_FOR_RANDOM_SEED
    (WE_VOID)WE_MEMCPY( pucSeed, "12345abc3453dgsad", 17 );
    *puiSeedLen = 17;
#else
    {
        time_t iTimeValue=0; 
        iTimeValue = time(&iTimeValue);
        (WE_VOID)WE_MEMCPY( pucSeed, "12345abc3453dgsad", 17 );
        (WE_VOID)WE_MEMCPY( pucSeed+13, &iTimeValue, 4 );    
        *puiSeedLen = 17;     
    }
#endif
    return M_WE_LIB_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    We_LibPdfSeed
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the pdf seed.
ARGUMENTS PASSED:
    ppucSeed[IO]: the pdf seed value.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    used hash result to be seed value.
=====================================================================================*/
/* Sec_PdfSeed */
WE_INT32 We_LibPdfSeed(WE_UCHAR **ppucSeed)
{
    WE_UCHAR *pucTmpSeed = NULL;
    WE_UCHAR *pucPtr = NULL;
    WE_INT32 iTmpSeedLen = 0;
    WE_UCHAR aucDigest1[21] = {0};
    WE_UCHAR aucDigest2[21] = {0};
    WE_UCHAR aucDigest3[21] = {0};
    WE_INT32 iIndex = 0;
    WE_INT32 iResult = 0;
    WE_INT32 iDigestLen = 0;
    WE_HANDLE hWeHandle = NULL;

    
    pucTmpSeed = WE_MALLOC(21*sizeof(WE_UCHAR));
    if (pucTmpSeed == NULL)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pucPtr = pucTmpSeed;
    
    /*tmpseddlen = 17*/
    if ((iResult = We_LibGetSeed(pucTmpSeed, &iTmpSeedLen)) != M_WE_LIB_ERR_OK)	
    {
        WE_FREE(pucTmpSeed);
        return iResult;
    }
    if ((iTmpSeedLen < 16) || (iTmpSeedLen > 20))
    {
        WE_FREE(pucTmpSeed);
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    /* get digest1, digest2, digest3 */
    iResult = We_LibHash (hWeHandle, E_WE_ALG_HASH_SHA1, pucPtr, iTmpSeedLen, aucDigest1, &iDigestLen);
    if (iResult != M_WE_LIB_ERR_OK)
    {
        WE_FREE(pucTmpSeed);
        return iResult;
    }
    (*ppucSeed)[0]=0x00;
    (*ppucSeed)[1]=0x41;
    (*ppucSeed)[2]=0x01;
    (*ppucSeed)[3]=*aucDigest1;
    pucPtr += 4;
    iResult = We_LibHash (hWeHandle, E_WE_ALG_HASH_SHA1, pucPtr, (iTmpSeedLen-4), aucDigest2, &iDigestLen);
    if (iResult= M_WE_LIB_ERR_OK)
    {
        WE_FREE(pucTmpSeed);
        return iResult;
    }
    (*ppucSeed)[4]=*pucPtr;
    (*ppucSeed)[5]=*aucDigest2;  
    pucPtr += 4;
    iResult = We_LibHash (hWeHandle, E_WE_ALG_HASH_SHA1, pucPtr, (iTmpSeedLen-8), aucDigest3, &iDigestLen);
    if (iResult != M_WE_LIB_ERR_OK)
    {
        WE_FREE(pucTmpSeed);
        return iResult;
    }
    (*ppucSeed)[6]=*pucPtr;
    for (iIndex=0; iIndex<20; iIndex++)
    {
        (*ppucSeed)[iIndex+7]=aucDigest1[iIndex];
        (*ppucSeed)[iIndex+7+20]=aucDigest2[iIndex];
        (*ppucSeed)[iIndex+7+40]=aucDigest3[iIndex];
    }
    (*ppucSeed)[iIndex+7+40]=0;
    WE_FREE(pucTmpSeed);
    
    return M_WE_LIB_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    We_LibCryptInitialise
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Initialise.
ARGUMENTS PASSED:
    hPrng[IO]: the handle of pstLibPrng(only used in RSA cryptography).
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    iLibIsInitialised: 
        1: initialised.
        0: not initialised.
    pstLibPrng: 
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCryptInitialise(WE_HANDLE hHandle,WE_HANDLE *phHandle)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    hHandle = hHandle;
    if (!phHandle)
    {
        return 0x08;
    }
#ifdef GLOBAL_VALUE
    iResult = We_LibPRNGInit();
#else
    iResult = We_LibPRNGInit(phHandle);
#endif

    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_LibCryptTerminate
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Terminate.
ARGUMENTS PASSED:
    None
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    iLibIsInitialised: 
        1: initialised.
        0: not initialised.
    pstLibPrng: 
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCryptTerminate(WE_HANDLE hHandle)
{
    /* used for else */
    St_WeLibDataRecord *pstDR = NULL;
    St_WeLibPrngInstS *pstPrng = NULL;
    if (!hHandle)
    {
        return 0x08;
    }
#ifdef GLOBAL_VALUE
    if (pstLibPrng != NULL)
    {
        pstLibPrng->pvDestroyFn(pstLibPrng);
    }
    iInitialise = 0;
    pstDR = pstDR;
    pstPrng = pstPrng;
#else
    pstDR = (St_WeLibDataRecord*)hHandle;
    pstPrng = (St_WeLibPrngInstS*)pstDR->hDataHandle;
    if (pstPrng != NULL)
    {
        pstPrng->pvDestroyFn(pstPrng);
    }
    pstDR->ucFlag = 0;
    WE_LIB_FREE(pstDR);
#endif
    return M_WE_LIB_ERR_OK;
}


/*************************************************************
*   Hash
*************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibHash
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hash Algorithm.
ARGUMENTS PASSED:
    eAlg[IN]: algorithm type;
    pucData[IN]: data to be hashed;
    iDataLen[IN]: the length of data to be hashed;
    pucDigest[OUT]: hash result;
    piDigLen[OUT]: hashed data length(MD2,MD5:16;SHA:20).
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibHash
(
    WE_HANDLE hWeHandle,
    E_WeHashAlgType eAlg,
    const WE_UCHAR  *pucData, 
    WE_INT32        iDataLen, 
    WE_UCHAR        *pucDigest, 
    WE_INT32        *piDigLen
)
{
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;
    St_WeLibHashDigest stHashDigest = {0};
    WE_HANDLE hHandle = NULL;
    WE_UINT32 uiLen = 0;
    WE_UINT8 ucAlg = (WE_UINT8)eAlg;
    hWeHandle = hWeHandle;
    
    CHECK_FOR_NULL_ARG(pucDigest, 0x08);
    CHECK_FOR_NULL_ARG(piDigLen, 0x08);    
    
    switch (ucAlg)
    {
        case E_WE_ALG_HASH_SHA1:
            {
                uiRes = We_LibHashStartSha(&hHandle);
                if (M_WE_LIB_ERR_OK != uiRes)   /* Start a hash session */
                {
                    WE_LIB_FREEIF(hHandle);
                    return (WE_INT32)uiRes;        
                }
                uiRes = We_LibHashAppendSha((WE_UCHAR *)pucData, (WE_UINT32)iDataLen*8, hHandle);
                if (M_WE_LIB_ERR_OK != uiRes)   /* Pass data into the hash session */
                {
                    WE_LIB_FREE(hHandle);                                                    
                    return (WE_INT32)uiRes;                
                }
                uiRes = We_LibHashFinishSha(hHandle, &stHashDigest);/* Terminate a hash session and receive hashed data */

            }
            break;
            
        case E_WE_ALG_HASH_MD5:
            {
                uiRes = We_LibHashStartMd5(&hHandle);
                if (M_WE_LIB_ERR_OK != uiRes)   /* Start a hash session */
                {
                    WE_LIB_FREEIF(hHandle);
                    return (WE_INT32)uiRes;        
                }
                uiRes = We_LibHashAppendMd5((WE_UCHAR *)pucData, (WE_UINT32)iDataLen*8, hHandle);
                if (M_WE_LIB_ERR_OK != uiRes)   /* Pass data into the hash session */
                {
                    WE_LIB_FREE(hHandle);                                                    
                    return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;                
                }
                uiRes = We_LibHashFinishMd5(hHandle, &stHashDigest);/* Terminate a hash session and receive hashed data */

            }
            break;
            
        case E_WE_ALG_HASH_MD2:
            {
                uiRes = We_LibHashStartMd2(&hHandle);
                if (M_WE_LIB_ERR_OK != uiRes)   /* Start a hash session */
                {
                    WE_LIB_FREEIF(hHandle);
                    return (WE_INT32)uiRes;        
                }
                uiRes = We_LibHashAppendMd2((WE_UCHAR *)pucData, (WE_UINT32)iDataLen*8, hHandle);
                if (M_WE_LIB_ERR_OK != uiRes)   /* Pass data into the hash session */
                {
                    WE_LIB_FREE(hHandle);                                                    
                    return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;                
                }
                uiRes = We_LibHashFinishMd2(hHandle, &stHashDigest);/* Terminate a hash session and receive hashed data */

            }
            break;
            
        default:
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    
    uiLen = stHashDigest.uiHashSize;
    if (uiLen != 16 && uiLen != 20)
    {
        return M_WE_LIB_ERR_GENERAL_ERROR;                             /* len = output length in bytes */
    }
    (WE_VOID)WE_MEMCPY(pucDigest, stHashDigest.aucHashValue, uiLen);
    *piDigLen = (WE_INT32)uiLen;
    
    return (WE_INT32)uiRes;
}
/*=====================================================================================
FUNCTION: 
    We_LibHashInit
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hash init.
ARGUMENTS PASSED:
    eAlg[IN]: hash algorithm type;
    hHashHandle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibHashInit
(
    WE_HANDLE hWeHandle,
    E_WeHashAlgType eAlg, 
    WE_HANDLE       *hHashHandle
)
{
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDr = NULL;
    WE_UINT8 ucAlg = (WE_UINT8)eAlg;
    hWeHandle = hWeHandle;
    CHECK_FOR_NULL_ARG(hHashHandle, 0x08);
    
    pstDr = (St_WeLibDataRecord *)WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (!pstDr)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pstDr->ucFlag = ucAlg;
    
    switch (ucAlg)
    {
        case E_WE_ALG_HASH_SHA1:
            {
                uiRes = We_LibHashStartSha(&(pstDr->hDataHandle));
            }
            break;
            
        case E_WE_ALG_HASH_MD5:
            {
                uiRes = We_LibHashStartMd5(&(pstDr->hDataHandle));
            }
            break;
            
        case E_WE_ALG_HASH_MD2:
            {
                uiRes = We_LibHashStartMd2(&(pstDr->hDataHandle));
            }
            break;
            
        default:
            WE_LIB_FREEIF(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    if (M_WE_LIB_ERR_OK != uiRes) 
    {
        WE_LIB_FREEIF(pstDr->hDataHandle);
        WE_LIB_FREE(pstDr);
    }
    else
    {
        *hHashHandle = pstDr;
    }
    return (WE_INT32)uiRes;
}

/*=====================================================================================
FUNCTION: 
    We_LibHashUpdate
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Process the data with hash algorithm.
ARGUMENTS PASSED:
    pucPart[IN]: the data part to be hashed.
    iPartLen[IN]: the length of data part.
    hHashHandle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibHashUpdate
(
    WE_HANDLE hWeHandle,
    const WE_UINT8  *pucPart, 
    WE_INT32        iPartLen,
    WE_HANDLE       hHashHandle
)
{
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDr = NULL;
    hWeHandle = hWeHandle;

    CHECK_FOR_NULL_ARG(hHashHandle, M_WE_LIB_ERR_INVALID_PARAMETER);
    CHECK_FOR_NULL_ARG(pucPart, M_WE_LIB_ERR_INVALID_PARAMETER);
    
    pstDr = (St_WeLibDataRecord *)hHashHandle;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_HASH_SHA1:
            {
                uiRes = We_LibHashAppendSha((WE_UCHAR *)pucPart, 
                                            (WE_UINT32)iPartLen*8, 
                                            pstDr->hDataHandle);
            }
            break;
            
        case E_WE_ALG_HASH_MD5:
            {
                uiRes = We_LibHashAppendMd5((WE_UCHAR *)pucPart, 
                                            (WE_UINT32)iPartLen*8, 
                                            pstDr->hDataHandle);
            }
            break;
            
        case E_WE_ALG_HASH_MD2:
            {
                uiRes = We_LibHashAppendMd2((WE_UCHAR *)pucPart, 
                                            (WE_UINT32)iPartLen*8, 
                                            pstDr->hDataHandle);
            }
            break;
            
        default:
            WE_LIB_FREEIF(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }

    if (M_WE_LIB_ERR_OK != uiRes)  /* Pass data into the hash session */
    {
        WE_LIB_FREE(pstDr->hDataHandle);
        WE_LIB_FREE(pstDr);
    }
    
    return (WE_INT32)uiRes;                
}

/*=====================================================================================
FUNCTION: 
    We_LibHashFinal
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get hash result.
ARGUMENTS PASSED:
    pucDigest[OUT]: hash result.
    piDigestLen[OUT]: hashed data length.
    hHashHandle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibHashFinal
(
    WE_HANDLE hWeHandle,
    WE_UCHAR    *pucDigest, 
    WE_INT32    *piDigestLen,  
    WE_HANDLE   hHashHandle 
)
{
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;
    St_WeLibHashDigest stHashDigest = {0};
    WE_UINT32 uiLen = 0;
    St_WeLibDataRecord *pstDr = NULL;
    hWeHandle = hWeHandle;

    CHECK_FOR_NULL_ARG(hHashHandle, M_WE_LIB_ERR_INVALID_PARAMETER);
    CHECK_FOR_NULL_ARG(pucDigest, M_WE_LIB_ERR_INVALID_PARAMETER);
    CHECK_FOR_NULL_ARG(piDigestLen, M_WE_LIB_ERR_INVALID_PARAMETER);
    
    pstDr = (St_WeLibDataRecord *)hHashHandle;
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_HASH_SHA1:
            {
                uiRes = We_LibHashFinishSha(pstDr->hDataHandle, &stHashDigest);  /* Terminate a hash session and receive hashed data */
            }
            break;
            
        case E_WE_ALG_HASH_MD5:
            {
                uiRes = We_LibHashFinishMd5(pstDr->hDataHandle, &stHashDigest);  /* Terminate a hash session and receive hashed data */
            }
            break;
            
        case E_WE_ALG_HASH_MD2:
            {
                uiRes = We_LibHashFinishMd2(pstDr->hDataHandle, &stHashDigest);  /* Terminate a hash session and receive hashed data */
            }
            break;
            
        default:
            WE_LIB_FREEIF(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;    
    }
    WE_LIB_FREE(pstDr);
    uiLen = stHashDigest.uiHashSize;    /* Convert length from bits to bytes */
    if (uiLen != 16 && uiLen != 20)
    {
        return 0x01;                             /* len = output length in bytes */
    }
    (WE_VOID)WE_MEMCPY(pucDigest, stHashDigest.aucHashValue, uiLen);
    *piDigestLen = (WE_INT32)uiLen;

    return (WE_INT32)uiRes;
}


/*************************************************************
 * HMAC
 *************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibHmac
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hmac algorithm.
ARGUMENTS PASSED:
    eAlg[IN]: Hmac algorithm type;
    pucKey[IN]: cipher key used in Hmac;
    iKeyLen[IN]: key length;
    pucData[IN]: data to be hmac;
    iDataLen[IN]: data length;
    pucDig[OUT]: hmac result;
    piDigLen[OUT]: the length of hmac result.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
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
)
{
    WE_INT32  iRv   = M_WE_LIB_ERR_OK;
    WE_HANDLE hHmac = NULL;
    
    if( !pucKey || !pucData || !pucDig || !piDigLen || \
        (iDataLen <= 0) || (iKeyLen <= 0) || (*piDigLen <= 0))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    iRv = We_LibHmacInit( hWeHandle,eAlg, pucKey, iKeyLen, &hHmac);
    if(M_WE_LIB_ERR_OK != iRv)
    {
        return iRv;
    }
    iRv = We_LibHmacUpdate(hWeHandle, pucData, iDataLen, hHmac);
    if(M_WE_LIB_ERR_OK == iRv)
    {
        iRv = We_LibHmacFinal(hWeHandle, pucDig, piDigLen, hHmac);
    }
    return iRv;
}

/*=====================================================================================
FUNCTION: 
    We_LibHmacInit
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hmac init.
ARGUMENTS PASSED:
    eAlg[IN]: hmac algorithm type;
    pucKey[IN]: cipher key used in hmac;
    iKeyLen[IN]: key length;
    phPrtHandle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    iActiveHashAlg: flag of hmac algorithm type.
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibHmacInit
(
    WE_HANDLE hWeHandle,
    E_WeHashAlgType eAlg,
    const WE_UINT8  *pucKey, 
    WE_INT32        iKeyLen,
    WE_HANDLE       *phPtrHandle
)
{
    WE_UINT32 uiRes = 0x00;
    St_WeLibDataRecord *pstDr = NULL;
    WE_UINT8 ucAlg = (WE_UINT8)eAlg;
    hWeHandle = hWeHandle ;

    CHECK_FOR_NULL_ARG(pucKey, 0x08);
    CHECK_FOR_NULL_ARG(phPtrHandle, 0x08);
  
    pstDr = WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (!pstDr)
    {
        return 0x04;
    }
    
    pstDr->ucFlag = ucAlg;
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_HMAC_SHA1:
            {
                uiRes = We_LibHashStartHmacSha(pucKey, (WE_UINT32)iKeyLen, &(pstDr->hDataHandle));
            }
            break;            
            
        case E_WE_ALG_HMAC_MD5:
            {
                uiRes = We_LibHashStartHmacMd5(pucKey, (WE_UINT32)iKeyLen, &(pstDr->hDataHandle));
            }
            break;
            
        case E_WE_ALG_HMAC_MD2:
            {
                uiRes = We_LibHashStartHmacMd2(pucKey, (WE_UINT32)iKeyLen, &(pstDr->hDataHandle));
            }
            break;
            
        default:
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    if (0x00 != uiRes) 
    {
        We_LibRemoveHmac(pstDr->hDataHandle);
        WE_LIB_FREE(pstDr);
    }
    else
    {
        *phPtrHandle = (WE_HANDLE)pstDr;
    }
    return (WE_INT32)uiRes;
    
}

/*=====================================================================================
FUNCTION: 
    We_LibHmacUpdate
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Process the data with hmac algorithm.
ARGUMENTS PASSED:
    pucPart[IN]: the data part to be hmac;
    iPartLen[IN]: the length of data part;
    handle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    iActiveHashAlg: flag of hmac algorithm type.
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibHmacUpdate
(
    WE_HANDLE hWeHandle,
    const WE_UCHAR  *pucPart, 
    WE_INT32        iPartLen,
    WE_HANDLE       handle
)
{
    WE_UINT32 uiRes = 0x00;
    St_WeLibDataRecord *pstDr = NULL;
    hWeHandle = hWeHandle;
    
    CHECK_FOR_NULL_ARG(handle, 0x08);
    CHECK_FOR_NULL_ARG(pucPart, 0x08);

    pstDr = (St_WeLibDataRecord*)handle;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_HMAC_SHA1:
            {
                uiRes = We_LibHashAppendHmacSha((WE_UCHAR *)pucPart, (WE_UINT32)iPartLen*8, pstDr->hDataHandle);
            }
            break;
            
        case E_WE_ALG_HMAC_MD5:
            {
                uiRes = We_LibHashAppendHmacMd5((WE_UCHAR *)pucPart, (WE_UINT32)iPartLen*8, pstDr->hDataHandle);
            }
            break;
            
        case E_WE_ALG_HMAC_MD2:
            uiRes = We_LibHashAppendHmacMd2((WE_UCHAR *)pucPart, (WE_UINT32)iPartLen*8, pstDr->hDataHandle);
            break;
            
        default:
            We_LibRemoveHmac(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return 0x03;
    }
    if (0x00 != uiRes)
    {
        We_LibRemoveHmac(pstDr->hDataHandle);
        WE_LIB_FREE(pstDr);
    }
    return (WE_INT32)uiRes;
}

/*=====================================================================================
FUNCTION: 
    We_LibHmacFinal
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the hmac result.
ARGUMENTS PASSED:
    pucDigest[OUT]: the hmac result;
    piDigestLen[OUT]: the length of hmac result;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    iActiveHashAlg: flag of hmac algorithm type.
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibHmacFinal
(
    WE_HANDLE hWeHandle,
    WE_UCHAR    *pucDigest, 
    WE_INT32    *piDigestLen, 
    WE_HANDLE   handle
)
{
    WE_UINT32 uiRes = 0x00;
    St_WeLibHashDigest stHashDigest = {0};
    WE_UINT32 uiLen = 0;
    St_WeLibDataRecord *pstDr = NULL;
    hWeHandle = hWeHandle;

    CHECK_FOR_NULL_ARG(pucDigest, 0x08);
    CHECK_FOR_NULL_ARG(piDigestLen, 0x08);
    
    
    if (NULL == handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDr = (St_WeLibDataRecord*)handle;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_HMAC_SHA1:
            {
                uiRes = We_LibHashFinishHmacSha(pstDr->hDataHandle, &stHashDigest);  /* Terminate a hash session and receive hashed data */
            }
            break;
            
        case E_WE_ALG_HMAC_MD5:
            {
                uiRes = We_LibHashFinishHmacMd5(pstDr->hDataHandle, &stHashDigest);  /* Terminate a hash session and receive hashed data */
            }
            break;
            
        case E_WE_ALG_HMAC_MD2:
            {
                uiRes = We_LibHashFinishHmacMd2(pstDr->hDataHandle, &stHashDigest);  /* Terminate a hash session and receive hashed data */
            }
            break;
            
        default:
            We_LibRemoveHmac(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return 0x03;
    }
    WE_LIB_FREE(pstDr);
    uiLen = stHashDigest.uiHashSize;
    if ((uiRes != 0x00)||(uiLen != 16 && uiLen != 20))
    {
        return 0x01;
    }
    (WE_VOID)WE_MEMCPY(pucDigest, stHashDigest.aucHashValue, uiLen);
    *piDigestLen = (WE_INT32)uiLen;
    
    return (WE_INT32)uiRes;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Cipher encrypt with no padding.
ARGUMENTS PASSED:
    eAlg[IN]: Encryption algorithm type;
    stKey[IN]: cipher key;
    pucIn[IN]: data to be encrypt;
    uiInLen[IN]: the length of data to be encrypt;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: the length of encrypt data.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptNoPadding(WE_HANDLE hWeHandle,
                                      E_WeCipherAlgType eAlg, 
                                      St_WeCipherCrptKey  stKey, 
                                      const WE_UCHAR* pucIn, 
                                      WE_UINT32 uiInLen, 
                                      WE_UCHAR* pucOut, 
                                      WE_UINT32* puiOutLen)
{
    WE_INT32 iRes = M_WE_LIB_ERR_OK;
    WE_INT8 cAlg = (WE_INT8)eAlg;
    hWeHandle = hWeHandle;
    
    switch (cAlg)
    {
    case E_WE_ALG_CIPHER_DES_CBC_40:
    case E_WE_ALG_CIPHER_DES_CBC: 
        iRes = We_LibDesEncrypt(stKey,pucIn, uiInLen, pucOut, puiOutLen);
        break;
    case E_WE_ALG_CIPHER_3DES_CBC: 
        iRes = We_Lib3DesEncrypt(stKey,pucIn, uiInLen, pucOut, puiOutLen);
        break;
        
    case E_WE_ALG_CIPHER_AES_CBC_128:
        {
            iRes = We_LibAESEncrypt( stKey, pucIn,
                uiInLen, pucOut, puiOutLen);
        }break;
    case E_WE_ALG_CIPHER_RC2_CBC_40:
        {
            iRes = We_LibRC2EncryptNoPadding( stKey, pucIn,
                uiInLen, pucOut, puiOutLen);            
        }break;
        
    case E_WE_ALG_CIPHER_IDEA_CBC_40:
    case E_WE_ALG_CIPHER_IDEA_CBC_56:
    case E_WE_ALG_CIPHER_IDEA_CBC:
        {
            iRes = We_LibIDEAEncryptNoPadding( stKey, pucIn, uiInLen, 
                                                pucOut, puiOutLen);                    
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC_40:
    case E_WE_ALG_CIPHER_RC5_CBC_56:
    case E_WE_ALG_CIPHER_RC5_CBC_64:
        {
            iRes = We_LibRC5Encrypt(12, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC:
        {
            iRes = We_LibRC5Encrypt(16, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_NULL:
    case E_WE_ALG_CIPHER_RC4_STR_40:
    case E_WE_ALG_CIPHER_RC4_STR_128:
        {
            iRes = We_LibCipherEncryptStream((E_WeCipherAlgType)cAlg, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    default :
        return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    return iRes;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt with RFC2630 padding.
ARGUMENTS PASSED:
    eAlg[IN]: encryption algorithm type;
    stKey[IN]: cipher key;
    pucIn[IN]: data to be encrypt;
    uiInLen[IN]: length of data to be encrypt;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptRFC2630Padding(
                                           WE_HANDLE hWeHandle,
                                           E_WeCipherAlgType eAlg, 
                                           St_WeCipherCrptKey stKey, 
                                           const WE_UCHAR* pucIn, 
                                           WE_UINT32 uiInLen, 
                                           WE_UCHAR* pucOut, 
                                           WE_UINT32* puiOutLen)
{
    WE_INT32 iRes = M_WE_LIB_ERR_OK;
    WE_INT8  cAlg = (WE_INT8)eAlg;
    hWeHandle = hWeHandle;
    switch (cAlg)
    {
    case E_WE_ALG_CIPHER_DES_CBC_40  :
    case E_WE_ALG_CIPHER_DES_CBC     : 
        if((stKey.iIvLen != 8)||(stKey.iKeyLen != 8))
        {
            return M_WE_LIB_ERR_INVALID_KEY;
        }
        iRes = We_LibDesEncrypt1(stKey,pucIn, uiInLen, pucOut, puiOutLen);
        break;
    case E_WE_ALG_CIPHER_3DES_CBC: 
        if((stKey.iIvLen != 8)||(stKey.iKeyLen != 24))
        {
            return M_WE_LIB_ERR_INVALID_KEY;
        }
        iRes = We_Lib3DesEncrypt1(stKey,pucIn, uiInLen, pucOut, puiOutLen);
        break;
    case E_WE_ALG_CIPHER_AES_CBC_128:
        {
            iRes = We_LibAESEncrypt1( stKey, pucIn,uiInLen,pucOut,puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC2_CBC_40:
        {
            iRes = We_LibRC2EncryptPadding( stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_IDEA_CBC_40:
    case E_WE_ALG_CIPHER_IDEA_CBC_56:
    case E_WE_ALG_CIPHER_IDEA_CBC:
        {
            iRes = We_LibIDEAEncryptPadding( stKey, pucIn, uiInLen, 
                                                    pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC_40:
    case E_WE_ALG_CIPHER_RC5_CBC_56:
    case E_WE_ALG_CIPHER_RC5_CBC_64:
        {
            iRes = We_LibRC5Encrypt1(12, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC:
        {
            iRes = We_LibRC5Encrypt1(16, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    default :
        return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
        
    }
    return iRes;
    
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt with no padding.
ARGUMENTS PASSED:
    eAlg[OUT]: decrypt algorithm type;
    stKey[OUT]: cipher key;
    pucIn[IN]: data to be decrypt;
    uiInLen[IN]: length of data to be decrypt;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptNoPadding(
                                      WE_HANDLE hWeHandle,
                                      E_WeCipherAlgType eAlg, 
                                      St_WeCipherCrptKey  stKey, 
                                      const WE_UCHAR* pucIn, 
                                      WE_UINT32 uiInLen, 
                                      WE_UCHAR* pucOut, 
                                      WE_UINT32* puiOutLen)
{
    WE_INT32 iRes = M_WE_LIB_ERR_OK;
    WE_INT8 cAlg = (WE_INT8)eAlg;
    hWeHandle = hWeHandle;
    
    switch (cAlg)
    {
    case E_WE_ALG_CIPHER_DES_CBC_40  :
    case E_WE_ALG_CIPHER_DES_CBC     : 
        if((stKey.iIvLen != 8)||(stKey.iKeyLen != 8))
        {
            return M_WE_LIB_ERR_INVALID_KEY;
        }
        iRes = We_LibDesDecrypt(stKey,pucIn, uiInLen, pucOut, puiOutLen);
        break;
    case E_WE_ALG_CIPHER_3DES_CBC: 
        if((stKey.iIvLen != 8)||(stKey.iKeyLen != 24))
        {
            return M_WE_LIB_ERR_INVALID_KEY;
        }
        iRes = We_Lib3DesDecrypt(stKey,pucIn, uiInLen, pucOut, puiOutLen);        
        break;
        
    case E_WE_ALG_CIPHER_AES_CBC_128:
        {
            iRes = We_LibAESDecrypt(stKey, pucIn, 
                uiInLen, pucOut, puiOutLen);
        }break;
    case E_WE_ALG_CIPHER_RC2_CBC_40:
        {
            iRes = We_LibRC2DecryptNoPadding(stKey, pucIn,
                uiInLen, pucOut, puiOutLen);            
        }
        break;
    case E_WE_ALG_CIPHER_IDEA_CBC_40:
    case E_WE_ALG_CIPHER_IDEA_CBC_56:
    case E_WE_ALG_CIPHER_IDEA_CBC:
        {
            iRes = We_LibIDEADecryptNoPadding(stKey, pucIn, uiInLen, 
                                                    pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC_40:
    case E_WE_ALG_CIPHER_RC5_CBC_56:
    case E_WE_ALG_CIPHER_RC5_CBC_64:
        {
            iRes = We_LibRC5Decrypt(12, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC:
        {
            iRes = We_LibRC5Decrypt(16, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_NULL:
    case E_WE_ALG_CIPHER_RC4_STR_40:
    case E_WE_ALG_CIPHER_RC4_STR_128:
        {
            iRes = We_LibCipherDecryptStream((E_WeCipherAlgType)cAlg, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    default :
        return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    return iRes;
    
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt with RFC2630 padding.
ARGUMENTS PASSED:
    eAlg[IN]: decryption algorithm type;
    stKey[IN]: cipher key;
    pucIn[IN]: data to be decrypted;
    uiInLen[IN]: length of data to be decrypt;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptRFC2630Padding(WE_HANDLE hWeHandle,
                                           E_WeCipherAlgType eAlg, 
                                           St_WeCipherCrptKey  stKey, 
                                           const WE_UCHAR* pucIn, 
                                           WE_UINT32 uiInLen, 
                                           WE_UCHAR* pucOut, 
                                           WE_UINT32* puiOutLen)
{
    WE_INT32 iRes = M_WE_LIB_ERR_OK;
    WE_INT8  cAlg = (WE_INT8)eAlg;
    hWeHandle = hWeHandle;

    switch (cAlg)
    {
    case E_WE_ALG_CIPHER_DES_CBC_40  :
    case E_WE_ALG_CIPHER_DES_CBC     : 
        if((stKey.iIvLen != 8)||(stKey.iKeyLen != 8))
        {
            return M_WE_LIB_ERR_INVALID_KEY;
        }        
        iRes = We_LibDesDecrypt1(stKey,pucIn, uiInLen, pucOut, puiOutLen);
        break;
    case E_WE_ALG_CIPHER_3DES_CBC: 
        if((stKey.iIvLen != 8)||(stKey.iKeyLen != 24))
        {
            return M_WE_LIB_ERR_INVALID_KEY;
        }
        iRes = We_Lib3DesDecrypt1(stKey,pucIn, uiInLen, pucOut, puiOutLen);
        break;
    case E_WE_ALG_CIPHER_AES_CBC_128:
        {
            iRes = We_LibAESDecrypt1(stKey, pucIn,uiInLen,pucOut,puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC2_CBC_40:
        {
            iRes = We_LibRC2DecryptPadding(stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_IDEA_CBC_40:
    case E_WE_ALG_CIPHER_IDEA_CBC_56:
    case E_WE_ALG_CIPHER_IDEA_CBC:
        {
            iRes = We_LibIDEADecryptPadding(stKey, pucIn, uiInLen, 
                                                pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC_40:
    case E_WE_ALG_CIPHER_RC5_CBC_56:
    case E_WE_ALG_CIPHER_RC5_CBC_64:
        {
            iRes = We_LibRC5Decrypt1(12, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC:
        {
            iRes = We_LibRC5Decrypt1(16, stKey, pucIn, uiInLen, pucOut, puiOutLen);
        }
        break;
    default:
        return M_WE_LIB_ERR_UNSUPPORTED_METHOD;        
    }
    return iRes;    
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptInitNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt init with no padding.
ARGUMENTS PASSED:
    eAlg[IN]: encryption algorithm type;
    stKey[IN]: cipher key;
    pHandle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptInitNoPadding(WE_HANDLE hWeHandle,
                                          E_WeCipherAlgType eAlg, 
                                          St_WeCipherCrptKey stKey,
                                          WE_HANDLE* pHandle)

{
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;

    CHECK_FOR_NULL_ARG(pHandle, 0x08);

    pstDataRecord = WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (NULL == pstDataRecord)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pstDataRecord->ucFlag = (WE_UINT8)eAlg;
    switch (pstDataRecord->ucFlag)
    {
    case E_WE_ALG_CIPHER_NULL:
        iRes = M_WE_LIB_ERR_OK;
        break;
    case E_WE_ALG_CIPHER_DES_CBC_40  :
    case E_WE_ALG_CIPHER_DES_CBC     : 
        iRes = We_LibDesEncryptInit(stKey,&(pstDataRecord->hDataHandle));
        break;
    case E_WE_ALG_CIPHER_3DES_CBC: 
        iRes = We_Lib3DesEncryptInit(stKey, &(pstDataRecord->hDataHandle));
        break;
    case E_WE_ALG_CIPHER_AES_CBC_128:
        {
            iRes = We_LibAESEncryptInit(stKey, &(pstDataRecord->hDataHandle));
        }
        break;
    case E_WE_ALG_CIPHER_RC2_CBC_40:
        {
            iRes = We_LibRC2EncryptInit(stKey, 1024, &(pstDataRecord->hDataHandle));
        }
        break;
    case E_WE_ALG_CIPHER_IDEA_CBC_40:
    case E_WE_ALG_CIPHER_IDEA_CBC_56:
    case E_WE_ALG_CIPHER_IDEA_CBC:
        {
            iRes = We_LibIDEAEncryptInit(stKey, &(pstDataRecord->hDataHandle));
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC_40:
    case E_WE_ALG_CIPHER_RC5_CBC_56:
    case E_WE_ALG_CIPHER_RC5_CBC_64:
        {
            iRes = We_LibRC5EncryptInit(12, stKey, &(pstDataRecord->hDataHandle));
        }
        break;
    case E_WE_ALG_CIPHER_RC5_CBC:
        {
            iRes = We_LibRC5EncryptInit(16, stKey, &(pstDataRecord->hDataHandle));
        }
        break;
    case E_WE_ALG_CIPHER_RC4_STR_40:
    case E_WE_ALG_CIPHER_RC4_STR_128:
        {
            iRes = We_LibRC4EncryptInit(stKey, &(pstDataRecord->hDataHandle));
        }
        break;
    default :
        WE_LIB_FREE(pstDataRecord);
        return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    (*pHandle) = (WE_HANDLE)pstDataRecord;
    return iRes;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptUpdateNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt data with no padding.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be encrypt;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptUpdateNoPadding(WE_HANDLE hWeHandle,
                                            const WE_UCHAR* pucIn, 
                                            WE_UINT32 uiInLen,
                                            WE_UCHAR* pucOut, 
                                            WE_UINT32* puiOutLen, 
                                            WE_HANDLE handle)
{
    WE_INT32 iResult  = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDataRecord = (St_WeLibDataRecord *)handle;

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_NULL:
            {
                (WE_VOID)WE_MEMCPY(pucOut, pucIn, uiInLen);
                *puiOutLen = uiInLen;
                iResult = M_WE_LIB_ERR_OK;
            }
            break;
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesEnCryptUpdate(pucIn, uiInLen, 
                                        pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesEnCryptUpdate(pucIn, uiInLen,
                                      pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            {
                iResult = We_LibAESEncryptUpdate(pucIn, uiInLen, 
                                                pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            {
                iResult = We_LibRC2EncryptUpdate(pucIn, uiInLen, 
                                            pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEAEncryptUpdate(pucIn, uiInLen, 
                                            pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5EncryptUpdate(pucIn, uiInLen, pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5EncryptUpdate(pucIn, uiInLen, pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            {
                iResult = We_LibRC4EncryptUpdate(pucIn, uiInLen, pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            WE_LIB_FREE(pstDataRecord);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    return iResult;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptFinalNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the encrypted data and free the memory.
ARGUMENTS PASSED:
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptFinalNoPadding(WE_HANDLE hWeHandle,WE_HANDLE handle)
{
    WE_INT32 iResult=0;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;
    if( !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDataRecord = (St_WeLibDataRecord *)handle;

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_NULL:
            iResult = M_WE_LIB_ERR_OK;
            break;
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesEnCryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesEnCryptFinal(pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            iResult = We_LibAESEncryptFinal(pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            iResult = We_LibRC2EncryptFinal(pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEAEncryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5EncryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5EncryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            {
                iResult = We_LibRC4EncryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle != NULL)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            iResult = M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    WE_LIB_FREE(pstDataRecord);
    return iResult;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptInitNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt init with no padding.
ARGUMENTS PASSED:
    eAlg[IN]: decryption algorithm type;
    stKey[IN]: cipher key;
    pHandle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptInitNoPadding(WE_HANDLE hWeHandle,
                                          E_WeCipherAlgType eAlg, 
                                          St_WeCipherCrptKey stKey,
                                          WE_HANDLE* pHandle)
{
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;

    CHECK_FOR_NULL_ARG(pHandle, 0x08);

    pstDataRecord = WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (NULL == pstDataRecord)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pstDataRecord->ucFlag = (WE_UINT8)eAlg;
    switch (pstDataRecord->ucFlag)
    {
        case E_WE_ALG_CIPHER_NULL:
            iRes = M_WE_LIB_ERR_OK;
            break;
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC     :
            iRes = We_LibDesDeCryptInit(stKey,&(pstDataRecord->hDataHandle));
            break;
        case E_WE_ALG_CIPHER_3DES_CBC: 
            iRes = We_Lib3DesDecryptInit(stKey, &(pstDataRecord->hDataHandle));
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            iRes = We_LibAESDecryptInit(stKey, &(pstDataRecord->hDataHandle));
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            {
                iRes = We_LibRC2DecryptInit(stKey, 1024, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iRes = We_LibIDEADecryptInit(stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iRes = We_LibRC5DecryptInit(12, stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iRes = We_LibRC5DecryptInit(16, stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            {
                iRes = We_LibRC4DecryptInit(stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        default :
            WE_LIB_FREE(pstDataRecord);        
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    (*pHandle) = (WE_HANDLE)pstDataRecord;
    return iRes;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptUpdateNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt data with no padding.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be decrypt;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptUpdateNoPadding(WE_HANDLE hWeHandle,
                                            const WE_UCHAR* pucIn, 
                                            WE_UINT32 uiInLen, 
                                            WE_UCHAR* pucOut, 
                                            WE_UINT32* puiOutLen, 
                                            WE_HANDLE handle)
{
    WE_INT32              iResult  = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDataRecord = (St_WeLibDataRecord *)handle;
    

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_NULL:
            {
                (WE_VOID)WE_MEMCPY(pucOut, pucIn, uiInLen);
                *puiOutLen = uiInLen;
                iResult = M_WE_LIB_ERR_OK;
            }
            break;
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesDeCryptUpdate(pucIn, uiInLen, 
                                    pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesDeCryptUpdate(pucIn, uiInLen,
                                  pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            {
                iResult = We_LibAESDecryptUpdate(pucIn, uiInLen, 
                                    pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            {
                iResult = We_LibRC2DecryptUpdate(pucIn, uiInLen, 
                                    pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEADecryptUpdate( pucIn, uiInLen, 
                                                pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5DecryptUpdate(pucIn, uiInLen, pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5DecryptUpdate(pucIn, uiInLen, pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            {
                iResult = We_LibRC4DecryptUpdate(pucIn, uiInLen, pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            WE_LIB_FREE(pstDataRecord);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    return iResult;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptFinalNoPadding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the decrypted data and free memory.
ARGUMENTS PASSED:
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptFinalNoPadding(WE_HANDLE hWeHandle,WE_HANDLE handle)
{
    WE_INT32 iResult=0;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;
    
    if( !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDataRecord = (St_WeLibDataRecord *)handle;

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_NULL:
            iResult = M_WE_LIB_ERR_OK;
            break;
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesDeCryptFinal( pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesDeCryptFinal( pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            iResult = We_LibAESDecryptFinal( pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            iResult = We_LibRC2DecryptFinal( pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEADecryptFinal( pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5DecryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5DecryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            {
                iResult = We_LibRC4DecryptFinal(pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle != NULL)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            iResult = M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    WE_LIB_FREE(pstDataRecord);
    return iResult;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptInitRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt init with rfc2630 padding.
ARGUMENTS PASSED:
    eAlg[IN]: encryption algorithm type;
    stKey[IN]: cipher key;
    pHandle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptInitRFC2630Padding(WE_HANDLE hWeHandle,
                                               E_WeCipherAlgType eAlg, 
                                               St_WeCipherCrptKey stKey, 
                                               WE_HANDLE* pHandle)

{
    WE_INT32 iRv    = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;
    
    CHECK_FOR_NULL_ARG(pHandle, 0x08);

    pstDataRecord = WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (NULL == pstDataRecord)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pstDataRecord->ucFlag = (WE_UINT8)eAlg;
    switch (pstDataRecord->ucFlag)
    {
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC     :         
            iRv = We_LibDesEncryptInit1(stKey,&(pstDataRecord->hDataHandle));
            break;
        case E_WE_ALG_CIPHER_3DES_CBC: 
            if ((stKey.iIvLen != 8) || (stKey.iKeyLen != 24))
            {
                WE_LIB_FREE(pstDataRecord);
                return M_WE_LIB_ERR_INVALID_KEY;
            }
            iRv = We_Lib3DesEncryptInit1(stKey, &(pstDataRecord->hDataHandle));
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            {
                iRv = We_LibAESEncryptInit1(stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            {
                iRv = We_LibRC2EncryptInitPad(stKey, 1024, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iRv = We_LibIDEAEncryptInitPad(stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iRv = We_LibRC5EncryptInit1(12, stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iRv = We_LibRC5EncryptInit1(16, stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        default:
            WE_LIB_FREE(pstDataRecord);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    (*pHandle) = (WE_HANDLE)pstDataRecord;
    return iRv;
}


/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptUpdateRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt data part with rfc2630 padding.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be encrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptUpdateRFC2630Padding(WE_HANDLE hWeHandle,
                                                 const WE_UCHAR* pucIn, 
                                                 WE_UINT32 uiInLen, 
                                                 WE_UCHAR* pucOut, 
                                                 WE_UINT32* puiOutLen,
                                                 WE_HANDLE handle)
{
    WE_INT32              iResult       = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;
    
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDataRecord = (St_WeLibDataRecord *)handle;

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesEnCryptUpdate1(pucIn, uiInLen, 
                                        pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesEnCryptUpdate1(pucIn, uiInLen,
                                          pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            {
                iResult = We_LibAESEncryptUpdate1(pucIn, uiInLen, 
                                            pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            {
                iResult = We_LibRC2EncryptUpdatePad(pucIn, uiInLen, 
                                        pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEAEncryptUpdatePad(pucIn, uiInLen, 
                                        pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5EncryptUpdate1(pucIn, uiInLen, pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5EncryptUpdate1(pucIn, uiInLen, pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            WE_LIB_FREE(pstDataRecord);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    return iResult;    
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherEncryptFinalRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the encrypted data and free memory.
ARGUMENTS PASSED:
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherEncryptFinalRFC2630Padding(WE_HANDLE hWeHandle,WE_UINT8 *pucOut, WE_UINT32* puiOutLen, WE_HANDLE handle)
{
    WE_INT32 iResult=0;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;

    if ( !pucOut || !puiOutLen || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    pstDataRecord = (St_WeLibDataRecord *)handle;

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesEnCryptFinal1( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesEnCryptFinal1( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            iResult = We_LibAESEncryptFinal1( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            iResult = We_LibRC2EncryptFinalPad( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEAEncryptFinalPad( pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5EncryptFinal1(pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5EncryptFinal1(pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle != NULL)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            iResult = M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    WE_LIB_FREE(pstDataRecord);
    return iResult;
}

/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptInitRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt init with rfc2630 padding.
ARGUMENTS PASSED:
    eAlg[IN]: decryption algorithm type;
    stKey[IN]: cipher key;
    pHandle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptInitRFC2630Padding(WE_HANDLE hWeHandle,
                                               E_WeCipherAlgType eAlg, 
                                               St_WeCipherCrptKey stKey,
                                               WE_HANDLE* pHandle)
{
    WE_INT32 iRv    = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;

    CHECK_FOR_NULL_ARG(pHandle, 0x08);

    pstDataRecord = WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (NULL == pstDataRecord)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pstDataRecord->ucFlag = (WE_UINT8)eAlg;
    switch (pstDataRecord->ucFlag)
    {
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC     :         
            iRv = We_LibDesDecryptInit1(stKey,&(pstDataRecord->hDataHandle));
            break;
        case E_WE_ALG_CIPHER_3DES_CBC:         
            if ((stKey.iIvLen != 8) || (stKey.iKeyLen != 24))
            {
                WE_LIB_FREE(pstDataRecord);
                return M_WE_LIB_ERR_INVALID_KEY;
            }
            iRv = We_Lib3DesDeCryptInit1(stKey, &(pstDataRecord->hDataHandle));
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            {
                iRv = We_LibAESDecryptInit1(stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            {
                iRv = We_LibRC2DecryptInitPad(stKey, 1024, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iRv = We_LibIDEADecryptInitPad(stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iRv = We_LibRC5DecryptInit1(12, stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iRv = We_LibRC5DecryptInit1(16, stKey, &(pstDataRecord->hDataHandle));
            }
            break;
        default:
                WE_LIB_FREE(pstDataRecord);
                return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    (*pHandle) = (WE_HANDLE)pstDataRecord;
    return iRv;
}


/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptUpdateRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt data part with rfc2630 padding.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be decrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptUpdateRFC2630Padding(WE_HANDLE hWeHandle,
                                                 const WE_UCHAR * pucIn, 
                                                 WE_UINT32 uiInLen,
                                                 WE_UCHAR* pucOut, 
                                                 WE_UINT32* puiOutLen,
                                                 WE_HANDLE handle)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;
    
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDataRecord = (St_WeLibDataRecord *)handle;

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesDeCryptUpdate1(pucIn, uiInLen, 
                                        pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesDeCryptUpdate1(pucIn, uiInLen,
                                          pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            {
                iResult = We_LibAESDecryptUpdate1(pucIn, uiInLen, 
                                                pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            {
                iResult = We_LibRC2DecryptUpdatePad(pucIn, uiInLen, 
                                                pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEADecryptUpdatePad(pucIn, uiInLen, 
                                                pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5DecryptUpdate1(pucIn, uiInLen, pucOut, puiOutLen, 
                                                pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5DecryptUpdate1(pucIn, uiInLen, pucOut, puiOutLen, 
                                                    pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            WE_LIB_FREE(pstDataRecord);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_LibCipherDecryptFinalRFC2630Padding
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get decrypted data and free memory.
ARGUMENTS PASSED:
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibCipherDecryptFinalRFC2630Padding(WE_HANDLE hWeHandle,WE_UCHAR* pucOut, 
                                                WE_UINT32* puiOutLen,WE_HANDLE handle)
{  
    WE_INT32 iResult=0;
    St_WeLibDataRecord *pstDataRecord = NULL;
    hWeHandle = hWeHandle;

    if ( !pucOut || !puiOutLen || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstDataRecord = (St_WeLibDataRecord *)handle;

    switch(pstDataRecord->ucFlag) 
    {
        case E_WE_ALG_CIPHER_DES_CBC_40  :
        case E_WE_ALG_CIPHER_DES_CBC :
            {
                iResult = We_LibDesDeCryptFinal1( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_3DES_CBC  :
            iResult = We_Lib3DesDeCryptFinal1( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_AES_CBC_128:
            iResult = We_LibAESDecryptFinal1( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_RC2_CBC_40:
            iResult = We_LibRC2DecryptFinalPad( pucOut, puiOutLen, pstDataRecord->hDataHandle);
            break;
        case E_WE_ALG_CIPHER_IDEA_CBC_40:
        case E_WE_ALG_CIPHER_IDEA_CBC_56:
        case E_WE_ALG_CIPHER_IDEA_CBC:
            {
                iResult = We_LibIDEADecryptFinalPad( pucOut, puiOutLen, 
                                                            pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC_40:
        case E_WE_ALG_CIPHER_RC5_CBC_56:
        case E_WE_ALG_CIPHER_RC5_CBC_64:
            {
                iResult = We_LibRC5DecryptFinal1(pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        case E_WE_ALG_CIPHER_RC5_CBC:
            {
                iResult = We_LibRC5DecryptFinal1(pucOut, puiOutLen, pstDataRecord->hDataHandle);
            }
            break;
        default:
            if (pstDataRecord->hDataHandle != NULL)
            {
                WE_LIB_FREE(pstDataRecord->hDataHandle);
            }
            iResult = M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    WE_LIB_FREE(pstDataRecord);
    return iResult;
}


/*==================================================================================================
FUNCTION: 
    We_LibCipherEncryptStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    encrypt data by RC4 algorithm.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    WE_UINT16           usRounds[IN]: compute round of RC4 algorithm.
    St_WeCipherKey      stKey[IN]: the key using in RC4 algorithm.
    WE_INT32            iInLen[IN]: length of the data.
    WE_UCHAR*           pucOut[OUT]: Pointer to the encrypted data.
    WE_INT32*           piOutLen[IN/OUT]: length of the encrypted data.
    WE_HANDLE           handle[IN]: handler of RC4 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherEncryptStream
(
    E_WeCipherAlgType  eAlg, 
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen
)
{
    WE_INT32 iRes = M_WE_LIB_ERR_OK;
    WE_HANDLE hHandle = NULL;
    WE_INT8 cAlg = (WE_INT8)eAlg;
    
    if((0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    switch (cAlg)
    {
        case E_WE_ALG_CIPHER_NULL:
        {
            (WE_VOID)WE_MEMCPY(pucOut, pucIn, uiInLen);
            *puiOutLen = uiInLen;
            return M_WE_LIB_ERR_OK;
        }
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            iRes = We_LibRC4EncryptInit(stKey, &hHandle);
            break;
        default:
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRes = We_LibRC4EncryptUpdate( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRes == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibRC4EncryptFinal( hHandle);
        }
        else
        {
            iRes = We_LibRC4EncryptFinal( hHandle);
            return iRes;
        }
    }
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibCipherDecryptStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    decrypt the encypted data by RC4 algorithm.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    WE_UINT16           usRounds[IN]: compute round of RC4 algorithm.
    St_WeCipherKey      stKey[IN]: the key using in RC4 algorithm.
    const WE_UCHAR*     pucIn[IN]: Pointer to the encypted data.
    WE_INT32            iInLen[IN]: length of the encypted data.
    WE_UCHAR*           pucOut[OUT]: Pointer to the decrypted data.
    WE_INT32*           piOutLen[IN/OUT]: length of the decrypted data.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherDecryptStream
(
    E_WeCipherAlgType eAlg, 
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen
)
{
    WE_INT32     iRes    = M_WE_LIB_ERR_OK;
    WE_HANDLE    hHandle = NULL;
    WE_INT8 cAlg = (WE_INT8)eAlg;
    
    if((0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    switch (cAlg)
    {    
        case E_WE_ALG_CIPHER_NULL:
        {
            (WE_VOID)WE_MEMCPY(pucOut, pucIn, uiInLen);
            *puiOutLen = uiInLen;
            return M_WE_LIB_ERR_OK;
        }
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            iRes = We_LibRC4DecryptInit(stKey, &hHandle);
            break;
        default:
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRes = We_LibRC4DecryptUpdate( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRes == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibRC4EncryptFinal( hHandle);
        }
        else
        {
            iRes = We_LibRC4EncryptFinal( hHandle);
            return iRes;
        }
    }
    
    return iRes;
}

#if 0
/*==================================================================================================
FUNCTION: 
    We_LibCipherEncryptInitStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    encrypt init by RC4 algorithm.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    WE_UINT16           usRounds[IN]: compute round of RC4 algorithm.
    St_WeCipherKey      stKey[IN]: the key using in RC4 algorithm.
    WE_HANDLE           handle[IN]: handler of RC4 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherEncryptInitStream
(
    E_WeCipherAlgType eAlg, 
    St_WeCipherCrptKey stKey,
    WE_HANDLE *pHandle
)
{
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDr = NULL;

    pstDr = WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (NULL == pstDr)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pstDr->ucFlag = (WE_UINT8)eAlg;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_CIPHER_NULL:
            iRes = M_WE_LIB_ERR_OK;
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:    
            iRes = We_LibRC4EncryptInit(stKey, &(pstDr->hDataHandle));
            break;
        default :
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    *pHandle = (WE_HANDLE)pstDr;
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibCipherDecryptInitStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    Decrypt init by RC4 algorithm.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    WE_UINT16           usRounds[IN]: compute round of RC4 algorithm.
    St_WeCipherKey      stKey[IN]: the key using in RC4 algorithm.
    WE_HANDLE           handle[IN]: handler of RC4 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherDecryptInitStream
(
    E_WeCipherAlgType eAlg,
    St_WeCipherCrptKey stKey,
    WE_HANDLE *pHandle
)
{
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDr = NULL;

    pstDr = WE_MALLOC(sizeof(St_WeLibDataRecord));
    if (NULL == pstDr)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    pstDr->ucFlag = (WE_UINT8)eAlg;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_CIPHER_NULL:
            iRes = M_WE_LIB_ERR_OK;
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:    
            iRes = We_LibRC4DecryptInit(stKey, &(pstDr->hDataHandle));
            break;
        default :
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    if (iRes != M_WE_LIB_ERR_OK)
    {
        WE_LIB_FREEIF(pstDr->hDataHandle);
        WE_LIB_FREE(pstDr);
    }
    else
    {
        *pHandle = (WE_HANDLE)pstDr;
    }
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibCipherEncryptUpdateStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    Encrypt data by RC4 algorithm.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    const WE_UCHAR*     pucIn[IN]: data to be encrypted.
    WE_INT32            iInLen[IN]: length of the data.
    WE_UCHAR*           pucOut[OUT]: Pointer to the encrypted data.
    WE_INT32*           piOutLen[IN/OUT]: length of the encrypted data.
    WE_HANDLE           handle[IN]: handler of RC4 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherEncryptUpdateStream
(
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDr = NULL;
    
    if (NULL == handle)
    {
        return 0x08;
    }
    pstDr = (St_WeLibDataRecord*)handle;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_CIPHER_NULL:
            {
                (WE_VOID)WE_MEMCPY(pucOut, pucIn, uiInLen);
                *puiOutLen = uiInLen;
                iRes = M_WE_LIB_ERR_OK;
            }
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            iRes = We_LibRC4EncryptUpdate (pucIn, uiInLen, pucOut, puiOutLen, pstDr->hDataHandle);
            break;
        default :
            WE_LIB_FREEIF(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    if (iRes != M_WE_LIB_ERR_OK)
    {
        WE_LIB_FREEIF(pstDr->hDataHandle);
        WE_LIB_FREE(pstDr);
    }
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibCipherDecryptUpdateStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    Free memory.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    const WE_UCHAR*     pucIn[IN]: data to be decrypted.
    WE_INT32            iInLen[IN]: length of the data.
    WE_UCHAR*           pucOut[OUT]: Pointer to the encrypted data.
    WE_INT32*           piOutLen[IN/OUT]: length of the encrypted data.
    WE_HANDLE           handle[IN]: handler of RC4 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherDecryptUpdateStream
(
    const WE_UCHAR *pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR *pucOut, 
    WE_UINT32 *puiOutLen, 
    WE_HANDLE handle
)
{  
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDr = NULL;
    
    if (NULL == handle)
    {
        return 0x08;
    }
    pstDr = (St_WeLibDataRecord*)handle;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_CIPHER_NULL:
            {
                (WE_VOID)WE_MEMCPY(pucOut, pucIn, uiInLen);
                *puiOutLen = uiInLen;
                iRes = M_WE_LIB_ERR_OK;
            }
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            iRes = We_LibRC4DecryptUpdate (pucIn, uiInLen, pucOut, puiOutLen, pstDr->hDataHandle);
            break;
        default :
            WE_LIB_FREEIF(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    if (iRes != M_WE_LIB_ERR_OK)
    {
        WE_LIB_FREEIF(pstDr->hDataHandle);
        WE_LIB_FREE(pstDr);
    }
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibCipherEncryptFinalStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    encrypt data by RC4 algorithm.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    WE_HANDLE           handle[IN]: handler of RC4 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherEncryptFinalStream(WE_HANDLE handle)
{
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDr = NULL;

    if (NULL == handle)
    {
        return 0x08;
    }
    pstDr = (St_WeLibDataRecord*)handle;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_CIPHER_NULL:
            iRes = M_WE_LIB_ERR_OK;
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            iRes = We_LibRC4EncryptFinal(pstDr->hDataHandle);
            break;
        default :
            WE_LIB_FREEIF(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    WE_LIB_FREE(pstDr);
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibCipherDecryptFinalStream
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    Free memory.
ARGUMENTS PASSED:
    WE_INT32            iAlg[IN]: type of RC4 algorithm.
    WE_HANDLE           handle[IN]: handler of RC4 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK : success.
    other : fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_LibCipherDecryptFinalStream(WE_HANDLE handle)
{
    WE_INT32 iRes = 0;
    St_WeLibDataRecord *pstDr = NULL;

    if (NULL == handle)
    {
        return 0x08;
    }
    pstDr = (St_WeLibDataRecord*)handle;
    
    switch (pstDr->ucFlag)
    {
        case E_WE_ALG_CIPHER_NULL:
            iRes = M_WE_LIB_ERR_OK;
            break;
        case E_WE_ALG_CIPHER_RC4_STR_40:
        case E_WE_ALG_CIPHER_RC4_STR_128:
            iRes = We_LibRC4DecryptFinal(pstDr->hDataHandle);
            break;
        default :
            WE_LIB_FREEIF(pstDr->hDataHandle);
            WE_LIB_FREE(pstDr);
            return M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    WE_LIB_FREE(pstDr);
    return iRes;
}
#endif


/*=====================================================================================
FUNCTION: 
    We_LibGenerateRandom
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get random data.
ARGUMENTS PASSED:
    pucRandomData[OUT]: random data;
    iRandomLen[IO]: length of random data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    pstLibPrng:
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibGenerateRandom(WE_HANDLE hHandle, WE_INT8 *pcRandomData, WE_INT32 iRandomLen)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    WE_UINT8 *pucRandomData = (WE_UINT8*)pcRandomData;
    WE_UINT32 uiSize = (WE_UINT32)iRandomLen;
    St_WeLibDataRecord *pstDR = NULL;
    St_WeLibPrngInstS *pstPrng = NULL;
    if (hHandle == NULL)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
#ifdef GLOBAL_VALUE
    if (iInitialise != 1) 
    {
        return M_WE_LIB_ERR_NOT_INITIALISED;
    }
    iResult = pstLibPrng->piGetFn(pucRandomData, uiSize, pstLibPrng);
    pstDR = pstDR;
    pstPrng = pstPrng;
#else
    pstDR = (St_WeLibDataRecord*)hHandle;
    pstPrng = (St_WeLibPrngInstS*)pstDR->hDataHandle;
    if (pstDR->ucFlag != 1)
    {
        return M_WE_LIB_ERR_NOT_INITIALISED;
    }
    iResult = pstPrng->piGetFn(pucRandomData, uiSize, pstPrng);
#endif
    if (!pucRandomData)
    {
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    return iResult;
}


#ifdef GLOBAL_VALUE
static WE_INT32 We_LibPRNGInit(void)
{
    WE_UINT8 *pucBptr = NULL;
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    
    pucBptr= (WE_UCHAR *)WE_MALLOC(68*sizeof(WE_UCHAR));
    if (NULL == pucBptr)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    /* get seed. */
    iResult = We_LibPdfSeed(&pucBptr);
    if (iResult != M_WE_LIB_ERR_OK)
    {
        WE_LIB_FREE( pucBptr);
        return iResult;
    }
    pstLibPrng = We_LibPrngCreatBbs((WE_UINT8 *)pucBptr);
    if (!pstLibPrng)
    {
        WE_LIB_FREE( pucBptr);
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    iInitialise += 1;
    WE_LIB_FREE( pucBptr);
    
    return iResult;
}
#else
static WE_INT32 We_LibPRNGInit(WE_HANDLE *hPrng)
{
    WE_UINT8 *pucBptr = NULL;
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    St_WeLibDataRecord *pstDR = NULL;
    St_WeLibPrngInstS *pstLibPrng = NULL;
    WE_INT32 iInitialise = 0;

    pucBptr= (WE_UCHAR *)WE_MALLOC(68*sizeof(WE_UCHAR));
    if (NULL == pucBptr)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    /* get seed. */
    iResult = We_LibPdfSeed(&pucBptr);
    if (iResult != M_WE_LIB_ERR_OK)
    {
        WE_LIB_FREE( pucBptr);
        return iResult;
    }
    pstLibPrng = We_LibPrngCreatBbs((WE_UINT8 *)pucBptr);
    if (!pstLibPrng)
    {
        WE_LIB_FREE( pucBptr);
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    iInitialise += 1;
    WE_LIB_FREE( pucBptr);
    
    pstDR = (St_WeLibDataRecord*)WE_MALLOC(sizeof(St_WeLibDataRecord));
    if(!pstDR)
    {
        WE_LIB_FREE( pstLibPrng);
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    (WE_VOID)WE_MEMSET(pstDR, 0x00, sizeof(St_WeLibDataRecord));
    pstDR->ucFlag = (WE_UINT8)iInitialise;
    pstDR->hDataHandle = (WE_HANDLE)pstLibPrng;
    (*hPrng) = (WE_HANDLE)pstDR;

    return iResult;
}
#endif











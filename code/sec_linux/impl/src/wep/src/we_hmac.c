/*=====================================================================================
    FILE   NAME : we_libhmac.c
    MODULE NAME : WE

    GENERAL DESCRIPTION
        The functions in this file manage the certificate(x509 or wtls).

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History

    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-11-03         Tang         None          Init

=====================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_hash.h"   


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeHMAC 
{
    E_WeHashAlgType   eHashType;
    WE_UINT32         (*HashStart)(WE_HANDLE *hHandle);
    WE_UINT32         (*HashAppend)(WE_UINT8 *pucData, WE_UINT32 uiNBits, WE_VOID *pvHashID);
    WE_UINT32         (*HashFinish)(WE_VOID *pvHashID, St_WeLibHashDigest *pstHashDigest);
} St_WeHMAC;

typedef struct tagSt_WeLibHmacContext
{
    St_WeHMAC stHmac;
    WE_VOID *pvHid1;  
    WE_VOID *pvHid2;  
} St_HmacId;


/*******************************************************************************
*   Function prototype Section
*******************************************************************************/
static WE_UINT32  We_LibStartHmac
( 
    St_WeHMAC  *pstHmac,
    const WE_UINT8   *pucKey, 
    WE_UINT32  uiKeyLen, 
    WE_HANDLE  *hHMACHandle
);

static WE_UINT32 We_LibAppendHmac
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiNBits, 
    WE_VOID     *pvHid
);

static WE_UINT32 We_LibFinishHmac
(
    WE_VOID * pvHid, 
    St_WeLibHashDigest *pstHashDigest
);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/************************************************************************
*   hmac-md2
************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibHashStartHmacMd2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Start hmac with hash algorithm of MD2.
ARGUMENTS PASSED:
    pucKey[IN]: cipher key used in hmac;
    uiKeyLen[IN]: length of key;
    hHMACHandle[IO]: data handle.
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
WE_UINT32 We_LibHashStartHmacMd2
(
    const WE_UINT8    *pucKey, 
    WE_UINT32   uiKeyLen, 
    WE_HANDLE   *hHMACHandle
)
{
    St_WeHMAC stHmacMD2;
    
    stHmacMD2.eHashType = E_WE_ALG_HASH_MD2;
    stHmacMD2.HashStart = We_LibHashStartMd2;
    stHmacMD2.HashAppend = We_LibHashAppendMd2;
    stHmacMD2.HashFinish = We_LibHashFinishMd2;
    
    return We_LibStartHmac(&stHmacMD2, pucKey, uiKeyLen, hHMACHandle);
}

/*=====================================================================================
FUNCTION: 
    We_LibHashAppendHmacMd2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Append hmac with hash algorithm of MD2.
ARGUMENTS PASSED:
    pucData[IN]: data to be hmac;
    uiNBits[IN]: bit length of data;
    pvHid[IO]: data handle.
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
WE_UINT32 We_LibHashAppendHmacMd2
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiNBits, 
    WE_VOID     *pvHid
)
{
    return We_LibAppendHmac(pucData, uiNBits, pvHid);
}

/*=====================================================================================
FUNCTION: 
    We_LibHashFinishHmacMd2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get hmac result with hash algorithm of MD2.
ARGUMENTS PASSED:
    pvHid[IO]: data handle.
    pstHashDigest[OUT]: hmac result;
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
WE_UINT32 We_LibHashFinishHmacMd2( WE_VOID * pvHid, St_WeLibHashDigest *pstHashDigest)
{
    return We_LibFinishHmac(pvHid, pstHashDigest);
}
/************************************************************************
*   hmac md5
************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibHashStartHmacMd5
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Start hmac with hash algorithm of MD5.
ARGUMENTS PASSED:
    pucKey[IN]: cipher key used in hmac;
    uiKeyLen[IN]: length of key;
    hHMACHandle[IO]: data handle.
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
WE_UINT32 We_LibHashStartHmacMd5
(
    const WE_UINT8    *pucKey, 
    WE_UINT32   uiKeyLen, 
    WE_HANDLE   *hHMACHandle
)
{
    St_WeHMAC stHmacMD5;
    
    stHmacMD5.eHashType = E_WE_ALG_HASH_MD5;
    stHmacMD5.HashStart = We_LibHashStartMd5;
    stHmacMD5.HashAppend = We_LibHashAppendMd5;
    stHmacMD5.HashFinish = We_LibHashFinishMd5;
    
    return We_LibStartHmac(&stHmacMD5, pucKey, uiKeyLen, hHMACHandle);
}

/*=====================================================================================
FUNCTION: 
    We_LibHashAppendHmacMd5
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Append hmac with hash algorithm of MD5.
ARGUMENTS PASSED:
    pucData[IN]: data to be hmac;
    uiNBits[IN]: bit length of data;
    pvHid[IO]: data handle.
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
WE_UINT32 We_LibHashAppendHmacMd5
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiNBits, 
    WE_VOID     *pvHid
)
{
    return We_LibAppendHmac(pucData, uiNBits, pvHid);
}

/*=====================================================================================
FUNCTION: 
    We_LibHashFinishHmacMd5
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get hmac result with hash algorithm of MD5.
ARGUMENTS PASSED:
    pvHid[IO]: data handle.
    pstHashDigest[OUT]: hmac result;
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
WE_UINT32 We_LibHashFinishHmacMd5( WE_VOID * pvHid, St_WeLibHashDigest *pstHashDigest)
{
    return We_LibFinishHmac(pvHid, pstHashDigest);
}
/************************************************************************
*   hmac-sha
************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibHashStartHmacsha
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Start hmac with hash algorithm of sha.
ARGUMENTS PASSED:
    pucKey[IN]: cipher key used in hmac;
    uiKeyLen[IN]: length of key;
    hHMACHandle[IO]: data handle.
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
WE_UINT32  We_LibHashStartHmacSha
( 
    const WE_UINT8   *pucKey, 
    WE_UINT32  uiKeyLen, 
    WE_HANDLE  *hHMACHandle
)
{
    St_WeHMAC stHmacSHA;
    
    stHmacSHA.eHashType = E_WE_ALG_HASH_SHA1;
    stHmacSHA.HashStart = We_LibHashStartSha;
    stHmacSHA.HashAppend = We_LibHashAppendSha;
    stHmacSHA.HashFinish = We_LibHashFinishSha;
                           
    return We_LibStartHmac(&stHmacSHA, pucKey, uiKeyLen, hHMACHandle);
}


/*=====================================================================================
FUNCTION: 
    We_LibHashAppendHmacsha
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Append hmac with hash algorithm of sha.
ARGUMENTS PASSED:
    pucData[IN]: data to be hmac;
    uiNBits[IN]: bit length of data;
    pvHid[IO]: data handle.
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
WE_UINT32 We_LibHashAppendHmacSha
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiNBits, 
    WE_VOID     *pvHid
)
{
    return We_LibAppendHmac(pucData, uiNBits, pvHid);
}

/*=====================================================================================
FUNCTION: 
    We_LibHashFinishHmacsha
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get hmac result with hash algorithm of sha.
ARGUMENTS PASSED:
    pvHid[IO]: data handle.
    pstHashDigest[OUT]: hmac result;
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
WE_UINT32 We_LibHashFinishHmacSha( WE_VOID * pvHid, St_WeLibHashDigest *pstHashDigest )
{
    return We_LibFinishHmac(pvHid, pstHashDigest);
}


/*=====================================================================================
FUNCTION: 
    We_LibHashStartHmac
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Start hmac with reference hash algorithm.
ARGUMENTS PASSED:
    pstHmac[IN]: hash alg flag and reference function pointer;
    pucKey[IN]: cipher key used in hmac;
    uiKeyLen[IN]: length of key;
    hHMACHandle[IO]: data handle.
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
static WE_UINT32  We_LibStartHmac
( 
    St_WeHMAC  *pstHmac,
    const WE_UINT8   *pucKey, 
    WE_UINT32  uiKeyLen, 
    WE_HANDLE  *hHMACHandle
)
{
    WE_UINT32   uiRes = 0x00;
    St_HmacId   *pstNew = NULL;
    WE_UINT32   uiIndex = 0;
    WE_UINT8    aucKeyData[M_HASH_BLOCK_SIZE] = {0};
    WE_UINT8    *pucKeyData = NULL;
    WE_UINT8 ucAlg = (WE_UINT8)pstHmac->eHashType;
    
    if((NULL==pucKey)||(0==uiKeyLen)||!hHMACHandle||0==ucAlg||ucAlg>3)
    {
        return 0x08;
    }
    (WE_VOID)WE_MEMSET(aucKeyData, 0x36, M_HASH_BLOCK_SIZE);
    
    pucKeyData = WE_MALLOC(uiKeyLen*sizeof(WE_UINT8));
    if (NULL == pucKeyData)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMCPY(pucKeyData, pucKey, uiKeyLen);
    if(uiKeyLen > M_HASH_BLOCK_SIZE) 
    { 
        WE_VOID *pvHid = NULL;
        St_WeLibHashDigest stHdig = {0};

        uiRes = pstHmac->HashStart(&pvHid);
        if(0x00 != uiRes || !pvHid) 
        {
            WE_LIB_FREE(pucKeyData);
            return 0x01;
        }
        uiRes = pstHmac->HashAppend(pucKeyData, uiKeyLen * 8, pvHid);
        if(0x00 != uiRes) 
        {
            WE_LIB_FREE(pucKeyData);
            WE_LIB_FREE(pvHid);
            return uiRes;
        }
        uiRes = pstHmac->HashFinish(pvHid, &stHdig);
        if(uiRes != 0x00) 
        {
            WE_LIB_FREE(pucKeyData);
            WE_LIB_FREE(pvHid);
            return uiRes;
        }        
        for (uiIndex = 0; uiIndex < stHdig.uiHashSize; uiIndex ++)
        {
            aucKeyData[uiIndex] ^= stHdig.aucHashValue[uiIndex];
        }
    } 
    else 
    {
        for(uiIndex = 0; uiIndex < uiKeyLen; uiIndex++) 
        {
            aucKeyData[uiIndex] ^= pucKeyData[uiIndex];
        }
    }
    WE_LIB_FREE(pucKeyData);
    pstNew = (St_HmacId *)WE_MALLOC(sizeof(St_HmacId));
    if(!pstNew) 
    {
        return 0x04;
    }
    uiRes = pstHmac->HashStart(&(pstNew->pvHid1));
    if(!(pstNew->pvHid1) || 0x00 != uiRes)
    {
        WE_LIB_FREE(pstNew);
        return 0x01;
    }
    uiRes = pstHmac->HashAppend(aucKeyData, M_HASH_BLOCK_SIZE * 8, pstNew->pvHid1);
    if(0x00 != uiRes) 
    {
        WE_LIB_FREE(pstNew->pvHid1);
        WE_LIB_FREE(pstNew);
        return uiRes;
    }
    
    for(uiIndex = 0; uiIndex < M_HASH_BLOCK_SIZE; uiIndex++)
    {
        aucKeyData[uiIndex] ^= 0x6a; 
    }
    
    uiRes = pstHmac->HashStart(&(pstNew->pvHid2));
    if(0x00 != uiRes || !pstNew->pvHid2) 
    {
        WE_LIB_FREE(pstNew->pvHid1);
        WE_LIB_FREE(pstNew);
        return 0x01;
    }
    uiRes = pstHmac->HashAppend(aucKeyData, M_HASH_BLOCK_SIZE * 8, pstNew->pvHid2);
    if(0x00 != uiRes) 
    {
        WE_LIB_FREE(pstNew->pvHid1);
        WE_LIB_FREE(pstNew->pvHid2);
        WE_LIB_FREE(pstNew);
        return uiRes;
    }  
    pstNew->stHmac = (*pstHmac);
    (*hHMACHandle) = (WE_HANDLE)pstNew;
    return 0x00;
}

/*=====================================================================================
FUNCTION: 
    We_LibHashAppendHmac
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Append hmac with reference hash algorithm.
ARGUMENTS PASSED:
    pucData[IN]: data to be hmac;
    uiNBits[IN]: bit length of data;
    pvHid[IO]: data handle.
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
static WE_UINT32 We_LibAppendHmac
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiNBits, 
    WE_VOID     *pvHid
)
{
    St_HmacId *pstSession = (St_HmacId *)pvHid;
    
    CHECK_FOR_NULL_ARG(pstSession, 0x08);
    if (!pstSession->stHmac.HashAppend)
    {
        WE_LIB_FREEIF(pstSession->pvHid1);
        WE_LIB_FREEIF(pstSession->pvHid2);
        WE_LIB_FREE(pstSession);
        return 0x08;
    }
    CHECK_FOR_NULL_ARG(pstSession->pvHid1, 0x08);
    
    return(pstSession->stHmac.HashAppend(pucData, uiNBits, pstSession->pvHid1));
}

/*=====================================================================================
FUNCTION: 
    We_LibHashFinishHmac
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get hmac result with reference hash algorithm.
ARGUMENTS PASSED:
    pvHid[IO]: data handle.
    pstHashDigest[OUT]: hmac result;
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
static WE_UINT32 We_LibFinishHmac( WE_VOID * pvHid, St_WeLibHashDigest *pstHashDigest )
{
    WE_UINT32 uiRes = 0x00;
    St_HmacId *pstSession = (St_HmacId *)pvHid;
    St_WeLibHashDigest stRval={0};
    
    if (!(pstSession) || (!pstSession->stHmac.HashAppend)||(!pstSession->stHmac.HashFinish)) 
    {
        return 0x08;
    }
    if (!pstHashDigest)
    {
        WE_LIB_FREEIF(pstSession->pvHid1);
        WE_LIB_FREEIF(pstSession->pvHid2);
        WE_LIB_FREE(pstSession);
        return 0x08;
    }
    
    uiRes = pstSession->stHmac.HashFinish(pstSession->pvHid1, &stRval);
    if(uiRes != 0x00) 
    {
        WE_LIB_FREEIF(pstSession->pvHid2);
        WE_LIB_FREE(pstSession);
        return uiRes;
    }  
    uiRes = pstSession->stHmac.HashAppend(stRval.aucHashValue, stRval.uiHashSize * 8, pstSession->pvHid2);
    if(0x00 != uiRes)
    {
        WE_LIB_FREEIF(pstSession->pvHid2);
        WE_LIB_FREE(pstSession);
        return uiRes;
    }
    
    uiRes = pstSession->stHmac.HashFinish(pstSession->pvHid2, &stRval);
    if(0x00 != uiRes) 
    {
        WE_LIB_FREE(pstSession);
        return uiRes;
    }
    WE_LIB_FREE(pstSession);

    (*pstHashDigest) = stRval;
    return uiRes;
}

/*=====================================================================================
FUNCTION: 
    We_LibRemoveHmac
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Free the memory of data handle.
ARGUMENTS PASSED:
    pvHmacId[IN]: data handle.
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
void We_LibRemoveHmac( WE_VOID *pvHmacId )
{    
    St_HmacId *pstSession = (St_HmacId *)pvHmacId;
    if (!(pstSession)) 
    {
        return;
    }

    WE_LIB_FREEIF(pstSession->pvHid1);
    WE_LIB_FREEIF(pstSession->pvHid2);
    WE_LIB_FREE(pstSession);
}


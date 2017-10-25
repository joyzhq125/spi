/*=====================================================================================
    FILE   NAME : we_sha.c
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
    2006-11-03         Sam         None          Init

=====================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h" 
#include "we_hash.h"


/*******************************************************************************
*   Macro Define Section
*******************************************************************************/          
#define F(x,y,z) (((x) & (y)) | (~(x) & (z)))
#define G(x,y,z) ((x) ^ (y) ^ (z))
#define H(x,y,z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define I(x,y,z) ((x) ^ (y) ^ (z))

#define R(a,s) ((a << (s)) + (a >> (32 - (s))))


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeHashSHA1Context
{
    WE_UINT32   uiDigest[5];
    WE_UINT32   uiMessage[80];
    WE_UINT32   uiSofar;
    WE_UINT32   uiNBits;
} St_WeHashSHA1Ctx;


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_VOID We_LibHashOutputToSha( St_WeLibHashDigest *pstHashDigest, WE_VOID* pvHashID );

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibSHACompress
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Process the message stored in the message block array.
ARGUMENTS PASSED:
    puiDigest[IN]: state;
    puiMessage[IN]: message block;
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
#define NW(j) t = puiMessage[j - 3] ^ puiMessage[j - 8] ^ puiMessage[j - 14] ^ puiMessage[j - 16]; puiMessage[j] = R(t,1)

WE_VOID We_LibSHACompress( WE_UINT32 *puiDigest, WE_UINT32 *puiMessage )
{
    WE_UINT32 a, b, c, d, e;
    WE_UINT32 t;
    WE_INT32  i; 

    if (!(puiDigest))
    {
        return;
    }

    if (!(puiMessage)) 
    {
        return;
    }

    a = puiDigest[0];
    b = puiDigest[1];
    c = puiDigest[2];
    d = puiDigest[3];
    e = puiDigest[4];

    for(i = 0; i < 16; i++) 
    {
        t = R(a,5) + F(b,c,d) + e + puiMessage[i] + 0x5a827999;
        e = d;
        d = c;
        c = R(b,30);
        b = a;
        a = t;
    }

    for(i = 16; i < 20; i++) 
    {
        NW(i); 
        t = R(a,5) + F(b,c,d) + e + puiMessage[i] + 0x5a827999; 
        e = d;
        d = c; 
        c = R(b,30); 
        b = a; 
        a = t;
    }

    for(i = 20; i < 40; i++) 
    {
        NW(i); 
        t = R(a,5) + G(b,c,d) + e + puiMessage[i] + 0x6ed9eba1; 
        e = d; 
        d = c; 
        c = R(b,30); 
        b = a; 
        a = t;
    }

    for(i = 40; i < 60; i++) 
    {
        NW(i); 
        t = R(a,5) + H(b,c,d) + e + puiMessage[i] + 0x8f1bbcdc; 
        e = d; 
        d = c; 
        c = R(b,30); 
        b = a; 
        a = t;
    }

    for(i = 60; i < 80; i++) 
    {
        NW(i); 
        t = R(a,5) + I(b,c,d) + e + puiMessage[i] + 0xca62c1d6; 
        e = d; 
        d = c; 
        c = R(b,30); 
        b = a; 
        a = t;
    }

    puiDigest[0] += a;
    puiDigest[1] += b;
    puiDigest[2] += c;
    puiDigest[3] += d;
    puiDigest[4] += e;
}
#undef NW

/*=====================================================================================
FUNCTION: 
    We_LibHashStartSha
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set the init information.
ARGUMENTS PASSED:
    uiSize[IN]: ;
    pvModulus[IN]: ;
    pstKey[IN]: ;
    phHandle[IN]: data handle.
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
WE_UINT32 We_LibHashStartSha(WE_HANDLE *phHandle)
{
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;
    St_WeHashSHA1Ctx *pstHashSHA1Ctx = NULL;

    CHECK_FOR_NULL_ARG(phHandle, M_WE_LIB_ERR_INVALID_PARAMETER);
    
    pstHashSHA1Ctx = (St_WeHashSHA1Ctx *)WE_MALLOC(sizeof(St_WeHashSHA1Ctx));    
    if(!pstHashSHA1Ctx) 
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }

    pstHashSHA1Ctx->uiDigest[0] = 0x67452301UL;
    pstHashSHA1Ctx->uiDigest[1] = 0xefcdab89UL;
    pstHashSHA1Ctx->uiDigest[2] = 0x98badcfeUL;
    pstHashSHA1Ctx->uiDigest[3] = 0x10325476UL;
    pstHashSHA1Ctx->uiDigest[4] = 0xc3d2e1f0UL;
    pstHashSHA1Ctx->uiSofar = 0UL;
    pstHashSHA1Ctx->uiNBits = 0U;

    (*phHandle) = (WE_HANDLE)pstHashSHA1Ctx;
    return uiRes;
}
/*=====================================================================================
FUNCTION: 
    We_LibHashAppendSha
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hashed data with SHA1 algorithm type.
ARGUMENTS PASSED:
    pucMessage[IN]: message block to be hashed;
    uiNBits[IN]: length of message in bit(8*byte);
    pvHashID[IN]: data handle.
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
#define AP(id,index,byte) switch((index) & 3) {\
  case 0:\
    (id)->uiMessage[(index) >> 2] = (byte) << 24;\
    break;\
  case 1:\
    (id)->uiMessage[(index) >> 2] += (byte) << 16;\
    break;\
  case 2:\
    (id)->uiMessage[(index) >> 2] += (byte) << 8;\
    break;\
  case 3:\
    (id)->uiMessage[(index) >> 2] += (byte);\
    break;\
  default:\
    break;\
  }

WE_UINT32 We_LibHashAppendSha
(
    WE_UINT8    *pucMessage, 
    WE_UINT32   uiNBits, 
    WE_VOID*    pvHashID
)
{
    St_WeHashSHA1Ctx *pstHashSHA1Ctx = NULL;
    WE_UINT32   uiByteCount = 0;
    WE_UINT32   uiIndex;
    WE_UINT32   uiBytes = 0;/*/8*/  
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;

    if (!pvHashID||!pucMessage)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    if (uiNBits == 0)
    {
        return 0x00;
    }
    pstHashSHA1Ctx = (St_WeHashSHA1Ctx *)pvHashID;
    uiByteCount = pstHashSHA1Ctx->uiNBits >> 3;
    uiBytes = uiNBits >> 3;/* 8 */  
    
    for(uiIndex = 0; uiIndex < uiBytes; uiIndex++) 
    {
        AP(pstHashSHA1Ctx,uiByteCount,(WE_UINT32) pucMessage[(WE_UINT32) uiIndex])
        if(++uiByteCount == 64) 
        {
            We_LibSHACompress(pstHashSHA1Ctx->uiDigest, pstHashSHA1Ctx->uiMessage);
            pstHashSHA1Ctx->uiSofar += 512UL;
            uiByteCount = 0;
        }
    }
    pstHashSHA1Ctx->uiNBits = uiByteCount << 3;
    
    return uiRes; 
}
/*=====================================================================================
FUNCTION: 
    We_LibHashFinishSha
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the hash value with SHA1 algorithm type.
ARGUMENTS PASSED:
    pstHashDigest[OUT]: pointer to hash value;
    pvHashID[IN]: data handle.
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
WE_UINT32 We_LibHashFinishSha( WE_VOID* pvHashID, St_WeLibHashDigest *pstHashDigest)
{
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;
    St_WeLibHashDigest stOutHashDigest = {0};

    if (!(pvHashID)||(!pstHashDigest)) 
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    We_LibHashOutputToSha(&stOutHashDigest, pvHashID);
    
    (*pstHashDigest) = stOutHashDigest;
    WE_LIB_FREE(pvHashID);
    return uiRes;
}
/*=====================================================================================
FUNCTION: 
    We_LibHashOutputToSha
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encode and out put hash result.
ARGUMENTS PASSED:
    pucDigest[IN]: state;
    pvHashID[IO]: data handle.
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
static WE_VOID We_LibHashOutputToSha( St_WeLibHashDigest *pstHashDigest, WE_VOID* pvHashID )
{
    St_WeHashSHA1Ctx *pstHashSHA1Ctx = NULL;
    WE_UINT32   uiByteCount = 0;
    WE_UINT32   uiIndex = 0;

    if (!(pvHashID)||!pstHashDigest)
    {
        return;
    }
    pstHashSHA1Ctx = (St_WeHashSHA1Ctx *)pvHashID;
    uiByteCount = pstHashSHA1Ctx->uiNBits >> 3;

    pstHashSHA1Ctx->uiSofar += (WE_UINT32) pstHashSHA1Ctx->uiNBits;
    AP(pstHashSHA1Ctx,uiByteCount,128UL)
    uiByteCount++;

    if(uiByteCount > 56) 
    {
        if(uiByteCount <= 60)
        {
            pstHashSHA1Ctx->uiMessage[15] = 0UL;
        }
        We_LibSHACompress(pstHashSHA1Ctx->uiDigest, pstHashSHA1Ctx->uiMessage);
        
        for(uiIndex = 0; uiIndex < 14; uiIndex++)
        {
            pstHashSHA1Ctx->uiMessage[uiIndex] = 0UL;
        }
    } 
    else
    {
        for(uiIndex = (uiByteCount + 3) >> 2; uiIndex < 14; uiIndex++)
        {
            pstHashSHA1Ctx->uiMessage[uiIndex] = 0UL;
        }
    }

    pstHashSHA1Ctx->uiMessage[14] = 0UL;
    pstHashSHA1Ctx->uiMessage[15] = pstHashSHA1Ctx->uiSofar;
    We_LibSHACompress(pstHashSHA1Ctx->uiDigest, pstHashSHA1Ctx->uiMessage);

    for(uiIndex = 0; uiIndex < 5; uiIndex++) 
    {
        WE_UINT32 uiTmp = pstHashSHA1Ctx->uiDigest[uiIndex];
        WE_UINT32 dind  = uiIndex << 2;

        pstHashDigest->aucHashValue[dind+3] = (WE_UINT8) (uiTmp & 255);
        pstHashDigest->aucHashValue[dind+2] = (WE_UINT8) ((uiTmp >>= 8) & 255);
        pstHashDigest->aucHashValue[dind+1] = (WE_UINT8) ((uiTmp >>= 8) & 255);
        pstHashDigest->aucHashValue[dind+0] = (WE_UINT8) ((uiTmp >>= 8) & 255);
    }
    pstHashDigest->uiHashSize = uiIndex << 2;
    pstHashSHA1Ctx->uiDigest[0] = 0x67452301UL;
    pstHashSHA1Ctx->uiDigest[1] = 0xefcdab89UL;
    pstHashSHA1Ctx->uiDigest[2] = 0x98badcfeUL;
    pstHashSHA1Ctx->uiDigest[3] = 0x10325476UL;
    pstHashSHA1Ctx->uiDigest[4] = 0xc3d2e1f0UL;
    pstHashSHA1Ctx->uiSofar = 0UL;
    pstHashSHA1Ctx->uiNBits = 0U;

}


#undef F
#undef G
#undef I
#undef H
#undef R
#undef AP



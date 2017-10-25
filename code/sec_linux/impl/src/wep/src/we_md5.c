/*=====================================================================================
    FILE   NAME : we_algmd5.c
    MODULE NAME : WE

    GENERAL DESCRIPTION
        This file define several functions to realize the MD5 algorithm.

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
#define G(x,y,z) (((x) & (z)) | ((y) & ~(z)))
#define H(x,y,z) ((x) ^ (y) ^ (z))
#define I(x,y,z) ((y) ^ ((x) | ~(z)))

#define R(a,s) ((a << (s)) + (a >> (32 - (s))))

#define FF(a,b,c,d,m,s,t) a += F(b,c,d) + m + t; a = b + R(a,s);
#define GG(a,b,c,d,m,s,t) a += G(b,c,d) + m + t; a = b + R(a,s);
#define HH(a,b,c,d,m,s,t) a += H(b,c,d) + m + t; a = b + R(a,s);
#define II(a,b,c,d,m,s,t) a += I(b,c,d) + m + t; a = b + R(a,s);


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeLibMD5Context
{
    WE_UINT32   uiDigest[4];
    WE_UINT32   uiMessage[16];
    WE_UINT32   uiSofar;
    WE_UINT32   uiNBits;
} St_WeLibMD5Context;


/*******************************************************************************
*   Function prototype Section
*******************************************************************************/
WE_VOID We_LibHashOutputToMd5
(
    St_WeLibHashDigest *pstHashDigest, 
    WE_VOID* pvHashID 
);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibMD5Compress
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    MD5 basic transformation, Transforms state based on block.
ARGUMENTS PASSED:
    puiDigest[IN]: state;
    puiMessage[IN]: message block;
RETURN VALUE:
    None.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
static WE_VOID We_LibMD5Compress( WE_UINT32 *puiDigest, WE_UINT32 *puiMessage )
{
    WE_UINT32 a, b, c, d;

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

    FF(a,b,c,d,puiMessage[ 0], 7, 0xd76aa478)
    FF(d,a,b,c,puiMessage[ 1],12, 0xe8c7b756)
    FF(c,d,a,b,puiMessage[ 2],17, 0x242070db)
    FF(b,c,d,a,puiMessage[ 3],22, 0xc1bdceee)
    FF(a,b,c,d,puiMessage[ 4], 7, 0xf57c0faf)
    FF(d,a,b,c,puiMessage[ 5],12, 0x4787c62a)
    FF(c,d,a,b,puiMessage[ 6],17, 0xa8304613)
    FF(b,c,d,a,puiMessage[ 7],22, 0xfd469501)
    FF(a,b,c,d,puiMessage[ 8], 7, 0x698098d8)
    FF(d,a,b,c,puiMessage[ 9],12, 0x8b44f7af)
    FF(c,d,a,b,puiMessage[10],17, 0xffff5bb1)
    FF(b,c,d,a,puiMessage[11],22, 0x895cd7be)
    FF(a,b,c,d,puiMessage[12], 7, 0x6b901122)
    FF(d,a,b,c,puiMessage[13],12, 0xfd987193)
    FF(c,d,a,b,puiMessage[14],17, 0xa679438e)
    FF(b,c,d,a,puiMessage[15],22, 0x49b40821)

    GG(a,b,c,d,puiMessage[ 1], 5, 0xf61e2562)
    GG(d,a,b,c,puiMessage[ 6], 9, 0xc040b340)
    GG(c,d,a,b,puiMessage[11],14, 0x265e5a51)
    GG(b,c,d,a,puiMessage[ 0],20, 0xe9b6c7aa)
    GG(a,b,c,d,puiMessage[ 5], 5, 0xd62f105d)
    GG(d,a,b,c,puiMessage[10], 9, 0x02441453)
    GG(c,d,a,b,puiMessage[15],14, 0xd8a1e681)
    GG(b,c,d,a,puiMessage[ 4],20, 0xe7d3fbc8)
    GG(a,b,c,d,puiMessage[ 9], 5, 0x21e1cde6)
    GG(d,a,b,c,puiMessage[14], 9, 0xc33707d6)
    GG(c,d,a,b,puiMessage[ 3],14, 0xf4d50d87)
    GG(b,c,d,a,puiMessage[ 8],20, 0x455a14ed)
    GG(a,b,c,d,puiMessage[13], 5, 0xa9e3e905)
    GG(d,a,b,c,puiMessage[ 2], 9, 0xfcefa3f8)
    GG(c,d,a,b,puiMessage[ 7],14, 0x676f02d9)
    GG(b,c,d,a,puiMessage[12],20, 0x8d2a4c8a)

    HH(a,b,c,d,puiMessage[ 5], 4, 0xfffa3942)
    HH(d,a,b,c,puiMessage[ 8],11, 0x8771f681)
    HH(c,d,a,b,puiMessage[11],16, 0x6d9d6122)
    HH(b,c,d,a,puiMessage[14],23, 0xfde5380c)
    HH(a,b,c,d,puiMessage[ 1], 4, 0xa4beea44)
    HH(d,a,b,c,puiMessage[ 4],11, 0x4bdecfa9)
    HH(c,d,a,b,puiMessage[ 7],16, 0xf6bb4b60)
    HH(b,c,d,a,puiMessage[10],23, 0xbebfbc70)
    HH(a,b,c,d,puiMessage[13], 4, 0x289b7ec6)
    HH(d,a,b,c,puiMessage[ 0],11, 0xeaa127fa)
    HH(c,d,a,b,puiMessage[ 3],16, 0xd4ef3085)
    HH(b,c,d,a,puiMessage[ 6],23, 0x04881d05)
    HH(a,b,c,d,puiMessage[ 9], 4, 0xd9d4d039)
    HH(d,a,b,c,puiMessage[12],11, 0xe6db99e5)
    HH(c,d,a,b,puiMessage[15],16, 0x1fa27cf8)
    HH(b,c,d,a,puiMessage[ 2],23, 0xc4ac5665)

    II(a,b,c,d,puiMessage[ 0], 6, 0xf4292244)
    II(d,a,b,c,puiMessage[ 7],10, 0x432aff97)
    II(c,d,a,b,puiMessage[14],15, 0xab9423a7)
    II(b,c,d,a,puiMessage[ 5],21, 0xfc93a039)
    II(a,b,c,d,puiMessage[12], 6, 0x655b59c3)
    II(d,a,b,c,puiMessage[ 3],10, 0x8f0ccc92)
    II(c,d,a,b,puiMessage[10],15, 0xffeff47d)
    II(b,c,d,a,puiMessage[ 1],21, 0x85845dd1)
    II(a,b,c,d,puiMessage[ 8], 6, 0x6fa87e4f)
    II(d,a,b,c,puiMessage[15],10, 0xfe2ce6e0)
    II(c,d,a,b,puiMessage[ 6],15, 0xa3014314)
    II(b,c,d,a,puiMessage[13],21, 0x4e0811a1)
    II(a,b,c,d,puiMessage[ 4], 6, 0xf7537e82)
    II(d,a,b,c,puiMessage[11],10, 0xbd3af235)
    II(c,d,a,b,puiMessage[ 2],15, 0x2ad7d2bb)
    II(b,c,d,a,puiMessage[ 9],21, 0xeb86d391)

    puiDigest[0] += a;
    puiDigest[1] += b;
    puiDigest[2] += c;
    puiDigest[3] += d;
}



/*=====================================================================================
FUNCTION: 
    We_LibHashStartMd5
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hash init with MD5 algorithm type.
ARGUMENTS PASSED:
    uiSize[IN]: ;
    pvModulus[IN]: ;
    pstKey[IN]: ;
    hHandle[IN]: data handle.
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
WE_UINT32 We_LibHashStartMd5(WE_HANDLE *hHandle)
{
    WE_UINT32 uiRes = M_WE_LIB_ERR_OK;
    St_WeLibMD5Context *pstHashMD5Ctx = NULL;

    CHECK_FOR_NULL_ARG(hHandle, 0x08);

    pstHashMD5Ctx = (St_WeLibMD5Context *)WE_MALLOC(sizeof(St_WeLibMD5Context));
    if(!pstHashMD5Ctx)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    /* initialise */
    pstHashMD5Ctx->uiDigest[0] = 0x67452301UL;  /* MD buffer */
    pstHashMD5Ctx->uiDigest[1] = 0xefcdab89UL;
    pstHashMD5Ctx->uiDigest[2] = 0x98badcfeUL;
    pstHashMD5Ctx->uiDigest[3] = 0x10325476UL;
    pstHashMD5Ctx->uiSofar = 0UL;
    pstHashMD5Ctx->uiNBits = 0U;

    (*hHandle) = (WE_HANDLE)pstHashMD5Ctx;
    return uiRes; 
}
/*=====================================================================================
FUNCTION: 
    We_LibHashAppendMd5
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hashed data with MD5 algorithm type.
ARGUMENTS PASSED:
    pucMessage[IN]: data to be hashed;
    uiNBits[IN]: length of data in bit(8*bytes);
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
    (id)->uiMessage[(index) >> 2] = (byte);\
    break;\
  case 1:\
    (id)->uiMessage[(index) >> 2] += (byte) << 8;\
    break;\
  case 2:\
    (id)->uiMessage[(index) >> 2] += (byte) << 16;\
    break;\
  case 3:\
    (id)->uiMessage[(index) >> 2] += (byte) << 24;\
    break;\
  default:\
    break;\
  }
WE_UINT32 We_LibHashAppendMd5
(
    WE_UINT8    *pucMessage, 
    WE_UINT32   uiNBits, 
    WE_VOID     *pvHashID
)
{
    St_WeLibMD5Context *pstHashMD5Ctx = NULL;
    WE_UINT32   uiByteCount = 0;
    WE_UINT32   uiIndex = 0;
    WE_UINT32   uiBytes = 0;  

    if (!pvHashID||!pucMessage)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    if (uiNBits == 0)
    {
        return 0x00;
    }
    pstHashMD5Ctx = (St_WeLibMD5Context *)pvHashID;
    uiByteCount = pstHashMD5Ctx->uiNBits >> 3;
    uiBytes = uiNBits >> 3; /* transform to byte length. */

    for(uiIndex = 0; uiIndex < uiBytes; uiIndex++) 
    {
        AP(pstHashMD5Ctx,uiByteCount,(WE_UINT32) pucMessage[uiIndex])
        if(++uiByteCount == 64) 
        {
            /* compress the data with the unit of 64 bytes. */
            We_LibMD5Compress(pstHashMD5Ctx->uiDigest, pstHashMD5Ctx->uiMessage);
            pstHashMD5Ctx->uiSofar += 512UL;    /* flag of bit length. */
            uiByteCount = 0;
        }
    }
    /* record of data length. */
    pstHashMD5Ctx->uiNBits = uiByteCount << 3;
    return 0;
}
/*=====================================================================================
FUNCTION: 
    We_LibHashFinishMd5
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the hash value with MD5 algorithm type.
ARGUMENTS PASSED:
    pstHashDigest[IN]: hash value;
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
WE_UINT32 We_LibHashFinishMd5( WE_VOID* pvHashID, St_WeLibHashDigest *pstHashDigest)
{
    St_WeLibHashDigest  stOutHashDigest = {0};
    
    if (!(pvHashID) || (!pstHashDigest)) 
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    We_LibHashOutputToMd5(&stOutHashDigest, pvHashID);

    (*pstHashDigest) = stOutHashDigest;
    WE_LIB_FREE(pvHashID);
    return 0;
}
/*=====================================================================================
FUNCTION: 
    We_LibHashOutputToMd5
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encode and output.
ARGUMENTS PASSED:
    pucDigest[IN]: state;
    pvHashID[IN]: data handle.
RETURN VALUE:
    None.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
void We_LibHashOutputToMd5( St_WeLibHashDigest *pstHashDigest, WE_VOID* pvHashID )
{
    St_WeLibMD5Context *pstHashMD5Ctx = NULL;
    WE_UINT32   uiByteCount = 0;
    WE_UINT32   uiIndex;

    if (!(pvHashID)||!pstHashDigest) 
    {
        return;
    }
    pstHashMD5Ctx = (St_WeLibMD5Context *)pvHashID;
    uiByteCount = pstHashMD5Ctx->uiNBits >> 3;

    pstHashMD5Ctx->uiSofar += (WE_UINT32) pstHashMD5Ctx->uiNBits;
    AP(pstHashMD5Ctx,uiByteCount,128UL) /* a single "1" bit and seven "0" bits are padded to the message. */
    uiByteCount++;

    if(uiByteCount > 56) 
    {
        if(uiByteCount <= 60)
        {
            pstHashMD5Ctx->uiMessage[15] = 0UL;
        }
        We_LibMD5Compress(pstHashMD5Ctx->uiDigest, pstHashMD5Ctx->uiMessage);
        
        for(uiIndex = 0; uiIndex < 14; uiIndex++)
        {
            pstHashMD5Ctx->uiMessage[uiIndex] = 0UL;
        }
    } 
    else
    {
        for(uiIndex = (uiByteCount + 3) >> 2; uiIndex < 14; uiIndex++)
        {
            /* multiple "0" bits are appended so that the length in bits of
            * the padded message becomes congruent to 448, modulo 512.*/
            pstHashMD5Ctx->uiMessage[uiIndex] = 0UL;
        }
    }
    /* pad the last eight bytes. */
    pstHashMD5Ctx->uiMessage[14] = pstHashMD5Ctx->uiSofar;
    pstHashMD5Ctx->uiMessage[15] = 0UL;
    We_LibMD5Compress(pstHashMD5Ctx->uiDigest, pstHashMD5Ctx->uiMessage);

    for(uiIndex = 0; uiIndex < 4; uiIndex++) 
    {
        /* get the digest value. */
        WE_UINT32 uiTmp = pstHashMD5Ctx->uiDigest[uiIndex];
        WE_UINT32 dind = uiIndex << 2;

        pstHashDigest->aucHashValue[dind++] = (WE_UINT8) (uiTmp & 255);
        pstHashDigest->aucHashValue[dind++] = (WE_UINT8) ((uiTmp >>= 8) & 255);
        pstHashDigest->aucHashValue[dind++] = (WE_UINT8) ((uiTmp >>= 8) & 255);
        pstHashDigest->aucHashValue[dind] = (WE_UINT8) ((uiTmp >>= 8) & 255);
    }
    pstHashDigest->uiHashSize = uiIndex << 2;
    /* reset MD buffer. */
    pstHashMD5Ctx->uiDigest[0] = 0x67452301UL;
    pstHashMD5Ctx->uiDigest[1] = 0xefcdab89UL;
    pstHashMD5Ctx->uiDigest[2] = 0x98badcfeUL;
    pstHashMD5Ctx->uiDigest[3] = 0x10325476UL;
    pstHashMD5Ctx->uiSofar = 0UL;
    pstHashMD5Ctx->uiNBits = 0U;
}




#undef F
#undef G
#undef H
#undef I
#undef R
#undef FF
#undef GG
#undef HH
#undef II
#undef AP




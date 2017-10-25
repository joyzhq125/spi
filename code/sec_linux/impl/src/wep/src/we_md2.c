/*=====================================================================================
    FILE NAME   : we_md2.c
    MODULE NAME : WE

    GENERAL DESCRIPTION
        This file define several functions to realize the MD2 algorithm.

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
typedef struct tagSt_WeHashMD2Context
{
    WE_UINT8    aucCheckSum[16]; 
    WE_UINT8    aucState[16];    
    WE_UINT8    aucTrailing[16]; 
    WE_UINT8    ucTlen;         
    WE_UINT8    ucL;            
} St_WeHashMD2Context;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 We_LibHashUpdataMD2
(
    WE_UINT8            *pucData, 
    St_WeHashMD2Context *pstHashMD2Ctx
);

static WE_INT32 We_LibMD2UpdateChecksum
(
    WE_UINT8            *pucData, 
    St_WeHashMD2Context *pstHashMD2Ctx
);

static WE_INT32 We_LibMD2UpdateState
(
    WE_UINT8            *pucData, 
    St_WeHashMD2Context *pstHashMD2Ctx
);

static St_WeLibHashDigest We_LibHashOutputMD2( WE_VOID *pvHashID );

/*******************************************************************************
*   File Static Variable Define Section
*******************************************************************************/
static WE_UINT8 aucPerm[256] = 
{
    41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
    19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
    76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
    138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
    245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
    148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
    39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
    181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
    150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
    112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
    96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
    85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
    234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
    129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
    8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
    203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
    166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
    31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};
  
/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibHashStartMd2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hash init with MD2 algorithm type.
ARGUMENTS PASSED:
    uiSize[IN]: ;
    pvModulus[IN]: ;
    pstKey[IN]: ;
    hHandle[OUT]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
====================================================================================*/
WE_UINT32 We_LibHashStartMd2(WE_HANDLE *hHandle)
{
    St_WeHashMD2Context *pstNew = NULL;

    CHECK_FOR_NULL_ARG(hHandle, 0x08);
    
    pstNew = WE_MALLOC(sizeof(St_WeHashMD2Context));
    if(!pstNew)
    {
        return 0x04;
    }
    (WE_VOID)WE_MEMSET(pstNew->aucCheckSum, 0, 16);
    (WE_VOID)WE_MEMSET(pstNew->aucState,    0, 16);
    (WE_VOID)WE_MEMSET(pstNew->aucTrailing, 0, 16);
    pstNew->ucTlen = 0;
    pstNew->ucL    = 0;

    (*hHandle) = (WE_HANDLE)pstNew;
    return 0x00;
}

/*=====================================================================================
FUNCTION: 
    We_LibHashAppendMd2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Hashed data with MD2 algorithm type.
ARGUMENTS PASSED:
    pucData[IN]: data to be hashed;
    uiNbits[IN]: length in bit(8*bytes);
    pvEid[IN]: data handle.
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
WE_UINT32 We_LibHashAppendMd2
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiNbits, 
    WE_VOID     *pvEid
)
{
    St_WeHashMD2Context   *pstM = (St_WeHashMD2Context *)pvEid;
    WE_UINT32   uiLen = (WE_UINT32) (uiNbits >> 3);

    if ((NULL == pstM) || (NULL == pucData))
    {
        return 0x08;
    }
    
    if (uiLen + pstM->ucTlen < 16)
    {
        (WE_VOID)WE_MEMCPY (pstM->aucTrailing+pstM->ucTlen, pucData, uiLen);
        pstM->ucTlen = (WE_UINT8) (pstM->ucTlen + uiLen);  
        return 0x00;
    }
    (WE_VOID)WE_MEMCPY (pstM->aucTrailing+pstM->ucTlen, pucData, 16-pstM->ucTlen);
    pucData+=(16-pstM->ucTlen);
    uiLen -=(16-pstM->ucTlen);    
    
    (WE_VOID)We_LibHashUpdataMD2(pstM->aucTrailing, pstM);    
    
    while(uiLen > 15)
    {
        (WE_VOID)We_LibHashUpdataMD2(pucData, pstM);
        pucData += 16;
        uiLen -= 16;
    }
    
    (WE_VOID)WE_MEMCPY(pstM->aucTrailing, pucData, uiLen);
    pstM->ucTlen = (WE_UINT8) uiLen;
    return 0x00;
}

/*=====================================================================================
FUNCTION: 
    We_LibHashFinishMd2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get the hash value with MD2 algorithm type.
ARGUMENTS PASSED:
    pvEid[IN]: data handle.
    pstHashDigest[OUT]: hash value.
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
WE_UINT32 We_LibHashFinishMd2(WE_VOID *pvEid, St_WeLibHashDigest *pstHashDigest)
{
    St_WeLibHashDigest  stRval={0};

    CHECK_FOR_NULL_ARG(pvEid, 0x08);
    CHECK_FOR_NULL_ARG(pstHashDigest, 0x08);

    stRval = We_LibHashOutputMD2(pvEid);
    (*pstHashDigest) = stRval;
    
    WE_LIB_FREE(pvEid);
    return 0x00;
}

/*=====================================================================================
FUNCTION: 
    We_LibHashOutputMD2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Out put the hashed data and free memory.
ARGUMENTS PASSED:
    pvEid[IN]: data handle.
RETURN VALUE:
    Structure of hash value.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
static St_WeLibHashDigest We_LibHashOutputMD2(WE_VOID * pvEid)
{
    St_WeHashMD2Context *pstM = NULL;
    St_WeLibHashDigest stRval = {0};

    if (NULL == pvEid)
    {
        return stRval;
    }
    pstM = (St_WeHashMD2Context *)pvEid;
    
    stRval.uiHashSize = 16;    
    (WE_VOID)WE_MEMSET(pstM->aucTrailing+pstM->ucTlen, 16-pstM->ucTlen, 16-pstM->ucTlen);    
    (WE_VOID)We_LibHashUpdataMD2(pstM->aucTrailing, pstM);    
    (WE_VOID)We_LibMD2UpdateState(pstM->aucCheckSum, pstM);    
    (WE_VOID)WE_MEMCPY(stRval.aucHashValue, pstM->aucState, 16);    
    /* reset. */
    (WE_VOID)WE_MEMSET(pstM->aucCheckSum, 0, 16);
    (WE_VOID)WE_MEMSET(pstM->aucState, 0, 16);
    (WE_VOID)WE_MEMSET(pstM->aucTrailing, 0, 16);
    pstM->ucTlen = 0;
    pstM->ucL = 0;  
    
    return stRval;
}

/*=====================================================================================
FUNCTION: 
    We_LibHashUpdataMD2
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Update the checksum and state.
ARGUMENTS PASSED:
    pucIn[IN]: trailing data;
    pstM[IN]: MD2 context;
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
static WE_INT32 We_LibHashUpdataMD2(WE_UINT8 *pucIn, St_WeHashMD2Context *pstM)
{
    (WE_VOID)We_LibMD2UpdateChecksum(pucIn, pstM);
    (WE_VOID)We_LibMD2UpdateState(pucIn, pstM);
    return 0x00;
}


/*=====================================================================================
FUNCTION: 
    We_LibMD2UpdateChecksum
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Updata the checksum.
ARGUMENTS PASSED:
    pucIn[IN]: trailing data;
    pstM[IN]: MD2 context;
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
static WE_INT32 We_LibMD2UpdateChecksum(WE_UINT8 *pucin, St_WeHashMD2Context *pstM)
{
    WE_UINT32 iIndex=0;

    if (NULL == pstM||!pucin)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    for(iIndex = 0; iIndex < 16; iIndex++)
    {
        pstM->aucCheckSum[iIndex] ^= aucPerm[*(pucin+iIndex) ^ (pstM->ucL)];
        pstM->ucL = pstM->aucCheckSum[iIndex];
    }
    return 0x00;
}

/*=====================================================================================
FUNCTION: 
    We_LibMD2UpdateState
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Update the state.
ARGUMENTS PASSED:
    pucIn[IN]: trailing data;
    pstM[IN]: MD2 context;
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
static WE_INT32 We_LibMD2UpdateState(WE_UINT8 *pucIn, St_WeHashMD2Context *pstM)
{ 
    WE_UINT32   uiJ=0;     
    WE_UINT32   uiK=0;     
    WE_UINT8    ucT = 0; 
    WE_UINT8    aucX[48]; 

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    
    (WE_VOID)WE_MEMCPY(aucX, pstM->aucState, 16);
    (WE_VOID)WE_MEMCPY(aucX+16, pucIn, 16);
    for (uiJ=0; uiJ<16; uiJ++)
    {
        aucX[uiJ+32] = (WE_UINT8) (((pstM->aucState[uiJ]) ^ (pucIn[uiJ])));
    }
    
    for (uiJ = 0; uiJ < 18; uiJ++)
    {
        for (uiK = 0; uiK < 48; uiK++)
        {
            ucT = (WE_UINT8) (aucX[uiK] ^ aucPerm[ucT]);
            aucX[uiK] = ucT;
        }
        ucT = ((ucT+(WE_UINT8)uiJ) & 255);
    }
    
    (WE_VOID)WE_MEMCPY(pstM->aucState, aucX, 16);
    return 0x00;
}






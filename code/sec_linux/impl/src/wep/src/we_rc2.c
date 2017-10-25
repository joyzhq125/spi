/*=====================================================================================
    FILE NAME : we_rc2.c
    MODULE NAME : WE  
    
    GENERAL DESCRIPTION
        This File will handle the operations on Rc2, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
      
    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-11-14      Stone An       None           create we_rc2.c
        
=====================================================================================*/

/*******************************************************************************
*   Include File Section
*******************************************************************************/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_rc2.h"


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeRC2Parameter
{
    WE_UINT32       uiType;
    WE_UINT16       ausXKey[64];
    WE_UCHAR        aucIv[WE_RC2_BLOCK_SIZE];
    WE_UCHAR        aucBuf[WE_RC2_BLOCK_SIZE];
    WE_INT32        iBufLen;
}St_WeRC2Parameter;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

static WE_INT32 We_LibRC2EncryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

static WE_INT32 We_LibRC2Encrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

static WE_INT32 We_LibRC2DecryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

static WE_INT32 We_LibRC2Decrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptNoPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    RC2 encrypt with no padding. The length of input is divided exactly by WE_RC2_BLOCK_SIZE.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptNoPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_INT32     iRv      = M_WE_LIB_ERR_OK;
    WE_INT32     iRes     = M_WE_LIB_ERR_OK;
    WE_HANDLE    hHandle  = NULL;
    
    if( !stKey.pucKey || (0 >= stKey.iKeyLen) || 
        (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    iRes = We_LibRC2EncryptInit( stKey, 128, &hHandle);    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibRC2EncryptUpdate( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibRC2EncryptFinal( hHandle);
        }
        else
        {
            iRes = We_LibRC2EncryptFinal( hHandle);
            return iRv;
        }
    }
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    RC2 encrypt with padding. The length of input is not divided exactly by WE_RC2_BLOCK_SIZE.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_INT32     iRv      = M_WE_LIB_ERR_OK;
    WE_INT32     iRes     = M_WE_LIB_ERR_OK;
    WE_HANDLE    hHandle  = NULL;
    WE_UINT32    uiTmpLen = 0;
    
    if( !stKey.pucKey || (0 >= stKey.iKeyLen) || 
        (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    uiTmpLen = *puiOutLen;
    
    iRes = We_LibRC2EncryptInitPad( stKey, 128, &hHandle);    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibRC2EncryptUpdatePad( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        uiTmpLen -= (*puiOutLen);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibRC2EncryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
        }
        else
        {
            iRes = We_LibRC2EncryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
            return iRv;
        }
    }

    (*puiOutLen) += uiTmpLen;
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptInit
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize RC2 algorithm without padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_UINT32       uiBits[IN]: the available bits of secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
)
{
    St_WeRC2Parameter*   pstPara   = NULL;
    WE_INT32             iIndexI = 0; 
    WE_UCHAR             ucTmp = 0;  
    WE_UINT32            uiT8 = 0;
    WE_UINT8             ucTM = 0;
    WE_UINT8             *pucXKey = NULL;

    /* 256-entry permutation table, probably derived somehow from pi */    
    const WE_UCHAR pucPermute[256] = {
            217,120,249,196, 25,221,181,237, 40,233,253,121, 74,160,216,157,
            198,126, 55,131, 43,118, 83,142, 98, 76,100,136, 68,139,251,162,
            23,154, 89,245,135,179, 79, 19, 97, 69,109,141, 9,129,125, 50,
            189,143, 64,235,134,183,123, 11,240,149, 33, 34, 92,107, 78,130,
            84,214,101,147,206, 96,178, 28,115, 86,192, 20,167,140,241,220,
            18,117,202, 31, 59,190,228,209, 66, 61,212, 48,163, 60,182, 38,
            111,191, 14,218, 70,105, 7, 87, 39,242, 29,155,188,148, 67, 3,
            248, 17,199,246,144,239, 62,231, 6,195,213, 47,200,102, 30,215, 
            8,232,234,222,128, 82,238,247,132,170,114,172, 53, 77,106, 42,
            150, 26,210,113, 90, 21, 73,116, 75,159,208, 94, 4, 24,164,236,
            194,224, 65,110, 15, 81,203,204, 36,145,175, 80,161,244,112, 57,
            153,124, 58,133, 35,184,180,122,252, 2, 54, 91, 37, 85,151, 49,
            45, 93,250,152,227,138,146,174, 5,223, 41, 16,103,108,186,201,
            211, 0,230,207,225,158,168, 44, 99, 22, 1, 63, 88,226,137,169,
            13, 56, 52, 27,171, 51,255,176,187, 72, 12, 95,185,177,205, 46, 
            197,243,219, 71,229,165,156,119, 10,166, 32,104,254,127,193,173
    };
    
    if ((uiBits>1024) || (NULL==stKey.pucKey) || 
        (0>=stKey.iKeyLen) || (stKey.iKeyLen>128)||!pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    

    if(WE_RC2_BLOCK_SIZE != stKey.iIvLen)
    {
        return M_WE_LIB_ERR_KEY_LENGTH;
    }    
 
    if (0 == uiBits)
    {
        uiBits = 1024;
    }
    
    pstPara = (St_WeRC2Parameter *)WE_MALLOC(sizeof(St_WeRC2Parameter));
    pucXKey = (WE_UINT8 *)WE_MALLOC(128 * sizeof(WE_UINT8));
    if ((NULL == pucXKey)  || (NULL == pstPara))
    {
        if (NULL != pucXKey) 
        {
            WE_FREE(pucXKey);
        }
        if (NULL != pstPara)
        {
            WE_FREE(pstPara);
        }
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }

    (WE_VOID)WE_MEMSET(pstPara, 0, sizeof(St_WeRC2Parameter));
    
    for (iIndexI=0; iIndexI<stKey.iKeyLen; iIndexI++)
    {
        pucXKey[iIndexI] = stKey.pucKey[iIndexI];
    }
    /* Phase 1: Expand input key to 128 bytes */
    for (iIndexI=stKey.iKeyLen; iIndexI<128; iIndexI++)
    {
        ucTmp = (pucXKey[iIndexI-1] + pucXKey[iIndexI-stKey.iKeyLen]) % 256;
        pucXKey[iIndexI] = pucPermute[ucTmp]; 
    }
    /* Phase 2 - reduce effective key size to "uiBits" */ 
    uiT8 = (uiBits + 7) >> 3;
    ucTM = 255 % (1 << (8 + uiBits - 8*uiT8));
    
    ucTmp = (pucXKey)[128 - uiT8] & ucTM;   
    pucXKey[128 - uiT8] = pucPermute[ucTmp];
    
    if (127 >= uiT8)
    {
        for (iIndexI=127-(WE_INT32)uiT8; iIndexI>=0; iIndexI--)
        {
            ucTmp = pucXKey[iIndexI+1] ^ pucXKey[iIndexI+(WE_INT32)uiT8];
            pucXKey[iIndexI] = pucPermute[ucTmp];
        }
    }
    /* Phase 3 - copy to (pucXKey) in little-endian order */
    for (iIndexI = 0; iIndexI<64; iIndexI++)
    {
        (pstPara->ausXKey)[iIndexI] = pucXKey[2*iIndexI]
            + (pucXKey[2*iIndexI+1] << 8);
    }

    pstPara->uiType = M_WE_RC2_TYPE;
    (WE_VOID)WE_MEMCPY(pstPara->aucIv, stKey.pucIv, WE_RC2_BLOCK_SIZE);

    *pHandle = pstPara;
    WE_FREE(pucXKey);
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptInitPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize RC2 algorithm using with padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_UINT32       uiBits[IN]: the available bits of secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptInitPad
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
)
{
    return We_LibRC2EncryptInit( stKey, uiBits, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptUpdate
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    encrypt data with RC2 algorithm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    WE_UINT32             uiLen     = 0; 

    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if(*puiOutLen >= uiInLen )
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    uiLen = uiInLen % WE_RC2_BLOCK_SIZE;
    if(0 == uiLen)
    {        
        (WE_VOID)We_LibRC2EncryptCBC( pucIn, uiInLen, pucOut, handle);
    }
    else
    {
        return M_WE_LIB_ERR_DATA_LENGTH;
    }

    return M_WE_LIB_ERR_OK;    
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptUpdatePad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    encrypt data with RC2 algorithm using with padding.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptUpdatePad
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    WE_UINT32             uiRealInLen = 0;
    WE_UINT32             uiIndex    = 0;
    St_WeRC2Parameter*    pstPara   = NULL;

    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeRC2Parameter*)handle;
    uiIndex = (WE_UINT32)(pstPara->iBufLen);

    uiRealInLen = ((uiInLen + uiIndex) / WE_RC2_BLOCK_SIZE) * WE_RC2_BLOCK_SIZE;
    if (*puiOutLen >= uiRealInLen)
    {
        *puiOutLen = uiRealInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    pstPara->iBufLen = (WE_INT32)(uiInLen + uiIndex - uiRealInLen);
    (WE_VOID)WE_MEMCPY((pstPara->aucBuf + uiIndex), pucIn, (WE_UINT32)(WE_RC2_BLOCK_SIZE - uiIndex));
    (WE_VOID)We_LibRC2EncryptCBC( pstPara->aucBuf, WE_RC2_BLOCK_SIZE, pucOut, handle);
    if (WE_RC2_BLOCK_SIZE != *puiOutLen)
    {
        (WE_VOID)We_LibRC2EncryptCBC( (pucIn + (WE_RC2_BLOCK_SIZE - uiIndex)),            
                uiRealInLen-WE_RC2_BLOCK_SIZE, pucOut+WE_RC2_BLOCK_SIZE, handle);
    }
    uiIndex = uiInLen - (WE_UINT32)(pstPara->iBufLen);
    (WE_VOID)WE_MEMCPY(pstPara->aucBuf, pucIn + uiIndex, (WE_UINT32)(pstPara->iBufLen));
    
    return M_WE_LIB_ERR_OK;    
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptFinal
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle of RC2 algorithm, release the handle of RC2 algorithm.
ARGUMENTS PASSED:
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptFinal(WE_HANDLE handle)
{
    St_WeRC2Parameter*   pstPara   = NULL;

    if (!handle)
    {
        return 0x08;
    }
    
    pstPara = (St_WeRC2Parameter *)handle;
    WE_FREE(pstPara);    

    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptFinalPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle with pad. Encryt the left plain data and then release the handle of RC2 algorthm.
ARGUMENTS PASSED:
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2EncryptFinalPad
(
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,  
    WE_HANDLE handle
)
{
    St_WeRC2Parameter*   pstPara   = NULL;
    WE_UCHAR             aucData[WE_RC2_BLOCK_SIZE]    = {0};
    
    if((NULL == handle) || (NULL == pucOut) || (NULL == puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    

    pstPara = (St_WeRC2Parameter*)handle;
    
    if (pstPara->iBufLen != 0)
    {  
        if(*puiOutLen < WE_RC2_BLOCK_SIZE)
        {
            return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
        }
        *puiOutLen = WE_RC2_BLOCK_SIZE;
        (WE_VOID)WE_MEMSET(aucData, (WE_UCHAR)(WE_RC2_BLOCK_SIZE-pstPara->iBufLen), (WE_UINT32)WE_RC2_BLOCK_SIZE);
        (WE_VOID)WE_MEMCPY(aucData, pstPara->aucBuf, (WE_UINT32)pstPara->iBufLen);
        (WE_VOID)We_LibRC2EncryptCBC( aucData, WE_RC2_BLOCK_SIZE, pucOut, handle);
    }
    else
    {
        *puiOutLen = 0;
    }
    
    WE_FREE(pstPara); 
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2EncryptCBC
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Encryt the input data with RC2 algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
static WE_INT32 We_LibRC2EncryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{  
    
    WE_INT32  iLen = (WE_INT32)uiInLen;
    St_WeRC2Parameter*    pstPara = NULL;
    WE_INT32              iRes    = 0;
    WE_INT32              iIndex  = 0;
    WE_INT32              iIndexJ = 0; 
    WE_UCHAR              aucInTmp[WE_RC2_BLOCK_SIZE] = {0}; 
    
    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    pstPara = (St_WeRC2Parameter*)handle;

    
    for (iIndexJ=0; iIndexJ<iLen; iIndexJ+=WE_RC2_BLOCK_SIZE)
    {
        for (iIndex=0; iIndex<WE_RC2_BLOCK_SIZE; iIndex++)
        {
            aucInTmp[iIndex] = pucIn[iIndexJ+iIndex];
            aucInTmp[iIndex] ^= ((pstPara->aucIv)[iIndex]);
        }

        iRes = We_LibRC2Encrypt( aucInTmp, WE_RC2_BLOCK_SIZE, &pucOut[iIndexJ], handle);
        if (iRes != M_WE_LIB_ERR_OK)
        {
            return iRes;
        }

        (WE_VOID)WE_MEMCPY(pstPara->aucIv, &pucOut[iIndexJ], WE_RC2_BLOCK_SIZE);
    }

    return M_WE_LIB_ERR_OK;        
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2Encrypt
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Encryt the input data which is one WE_RC2_BLOCK_SIZE with RC2 algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data, which is must be WE_RC2_BLOCK_SIZE.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
static WE_INT32 We_LibRC2Encrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{
    St_WeRC2Parameter*   pstPara   = NULL;
    WE_INT32     iIndexI = 0;
    WE_UINT16    usIn76 = 0;
    WE_UINT16    usIn54 = 0;
    WE_UINT16    usIn32 = 0;
    WE_UINT16    usIn10 = 0;
    
    if ( (WE_RC2_BLOCK_SIZE != uiInLen) || (!pucIn) || (!pucOut) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }  
   
    pstPara = (St_WeRC2Parameter *)handle;
    usIn76 = (pucIn[7] << 8) + pucIn[6];
    usIn54 = (pucIn[5] << 8) + pucIn[4];  
    usIn32 = (pucIn[3] << 8) + pucIn[2];
    usIn10 = (pucIn[1] << 8) + pucIn[0];
    
    for (iIndexI=0; iIndexI<16; iIndexI++)
    {/* turn the N bits of head to the end. */
        usIn10 += (usIn32 & ~usIn76) + (usIn54 & usIn76) + (pstPara->ausXKey)[4*iIndexI+0];  
        usIn10 = (WE_UINT16)((usIn10 << 1) + (usIn10 >> 15 & 0x01)); 

        usIn32 += (usIn54 & ~usIn10) + (usIn76 & usIn10) + (pstPara->ausXKey)[4*iIndexI+1];
        usIn32 = (WE_UINT16)((usIn32 << 2) + (usIn32 >> 14 & 0x03));

        usIn54 += (usIn76 & ~usIn32) + (usIn10 & usIn32) + (pstPara->ausXKey)[4*iIndexI+2];
        usIn54 = (WE_UINT16)((usIn54 << 3) + (usIn54 >> 13 & 0x07));

        usIn76 += (usIn10 & ~usIn54) + (usIn32 & usIn54) + (pstPara->ausXKey)[4*iIndexI+3];
        usIn76 = (WE_UINT16)((usIn76 << 5) + (usIn76 >> 11 & 0x1f));
        
        if (iIndexI == 4 || iIndexI == 10)
        {
            usIn10 += (pstPara->ausXKey)[usIn76 & 0x3f];
            usIn32 += (pstPara->ausXKey)[usIn10 & 0x3f];
            usIn54 += (pstPara->ausXKey)[usIn32 & 0x3f];
            usIn76 += (pstPara->ausXKey)[usIn54 & 0x3f];
        }
    }
    
    pucOut[0] = (WE_UCHAR)usIn10;
    pucOut[1] = (WE_UCHAR)(usIn10 >> 8);
    pucOut[2] = (WE_UCHAR)usIn32;
    pucOut[3] = (WE_UCHAR)(usIn32 >> 8);
    pucOut[4] = (WE_UCHAR)usIn54;
    pucOut[5] = (WE_UCHAR)(usIn54 >> 8);
    pucOut[6] = (WE_UCHAR)usIn76;
    pucOut[7] = (WE_UCHAR)(usIn76 >> 8);

    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptNoPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    RC2 decrypt with no padding. The length of input is divided exactly by WE_RC2_BLOCK_SIZE.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of decrypted data when OUT.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptNoPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_INT32     iRv      = M_WE_LIB_ERR_OK;
    WE_INT32     iRes     = M_WE_LIB_ERR_OK;
    WE_HANDLE    hHandle  = NULL;
    
    if ( !stKey.pucKey || (0 >= stKey.iKeyLen) || 
        (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if (0 != uiInLen % WE_RC2_BLOCK_SIZE)
    {
        return M_WE_LIB_ERR_DATA_LENGTH;
    }
    
    iRes = We_LibRC2DecryptInit( stKey, 128, &hHandle);
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibRC2DecryptUpdate( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibRC2DecryptFinal( hHandle);
        }
        else
        {
            iRes = We_LibRC2DecryptFinal( hHandle);
            return iRv;
        }
    }
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    RC2 decrypt with padding. The length of input is not divided exactly by WE_RC2_BLOCK_SIZE.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of decrypted data when OUT.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptPadding
(
    St_WeCipherCrptKey  stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_INT32     iRv      = M_WE_LIB_ERR_OK;
    WE_INT32     iRes     = M_WE_LIB_ERR_OK;
    WE_HANDLE    hHandle  = NULL;
    WE_UINT32    uiTmpLen = 0;
    
    if( !stKey.pucKey || (0 >= stKey.iKeyLen) || 
        (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    iRes = We_LibRC2DecryptInitPad( stKey, 128, &hHandle);    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibRC2DecryptUpdatePad( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibRC2DecryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
        }
        else
        {
            iRes = We_LibRC2DecryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
            return iRv;
        }
    }

    (*puiOutLen) -= uiTmpLen;
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptInit
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize RC2 algorithm without padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_UINT32       uiBits[IN]: the available bits of secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptInit
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
)
{
    return We_LibRC2EncryptInit( stKey, uiBits, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptInitPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize RC2 algorithm using with padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_UINT32       uiBits[IN]: the available bits of secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptInitPad
(
    St_WeCipherCrptKey stKey,
    WE_UINT32 uiBits,
    WE_HANDLE* pHandle
)
{
    return We_LibRC2EncryptInit( stKey, uiBits, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptUpdate
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    decrypt data with RC2 algorithm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    WE_UINT32             uiLen     = 0; 

    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if(*puiOutLen >= uiInLen )
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    uiLen = uiInLen % WE_RC2_BLOCK_SIZE;
    if(0 == uiLen)
    {        
        (WE_VOID)We_LibRC2DecryptCBC( pucIn, uiInLen, pucOut, handle);
    }
    else
    {
        return M_WE_LIB_ERR_DATA_LENGTH;
    }

    return M_WE_LIB_ERR_OK;    
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptUpdatePad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    decrypt data with RC2 algorithm using with padding.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of RC2.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptUpdatePad
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    WE_UINT32             uiRealInLen = 0;
    WE_UINT32             uiIndex    = 0;
    St_WeRC2Parameter*    pstPara   = NULL;

    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeRC2Parameter*)handle;
    uiIndex = (WE_UINT32)(pstPara->iBufLen);

    uiRealInLen = ((uiInLen + uiIndex) / WE_RC2_BLOCK_SIZE) * WE_RC2_BLOCK_SIZE;
    if (*puiOutLen >= uiRealInLen)
    {
        *puiOutLen = uiRealInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    pstPara->iBufLen = (WE_INT32)(uiInLen + uiIndex - uiRealInLen);
    (WE_VOID)WE_MEMCPY((pstPara->aucBuf + uiIndex), pucIn, (WE_UINT32)(WE_RC2_BLOCK_SIZE - uiIndex));
    (WE_VOID)We_LibRC2DecryptCBC( pstPara->aucBuf, WE_RC2_BLOCK_SIZE, pucOut, handle);
    if (WE_RC2_BLOCK_SIZE != *puiOutLen)
    {
        (WE_VOID)We_LibRC2DecryptCBC( (pucIn + (WE_RC2_BLOCK_SIZE - uiIndex)),            
                uiRealInLen-WE_RC2_BLOCK_SIZE, pucOut+WE_RC2_BLOCK_SIZE, handle);
    }
    uiIndex = uiInLen - (WE_UINT32)(pstPara->iBufLen);
    (WE_VOID)WE_MEMCPY(pstPara->aucBuf, pucIn + uiIndex, (WE_UINT32)(pstPara->iBufLen));
    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptFinal
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle of RC2 algorithm, release the handle of RC2 algorithm.
ARGUMENTS PASSED:
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptFinal(WE_HANDLE handle)
{
    return We_LibRC2EncryptFinal( handle);
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptFinalPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle with pad. compute the length of padding data.
ARGUMENTS PASSED:
    WE_UCHAR*       pucOut[OUT]: the end of decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of padding data.
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
WE_INT32 We_LibRC2DecryptFinalPad
(
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    St_WeRC2Parameter*   pstPara   = NULL;
    WE_UINT32            uiLen     = 0;
    
    if((NULL == handle) || (NULL == pucOut) || (NULL == puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstPara = (St_WeRC2Parameter*)handle;
    
    *puiOutLen = 0;
    uiLen = *(pucOut - 1);
    if ((uiLen > 0) && (uiLen < WE_RC2_BLOCK_SIZE))
    {
        WE_UINT32   uiLoop = 0;
        for (; uiLoop < uiLen; uiLoop++)
        {
            if(uiLen != *(pucOut - 1 - uiLoop))
            {
                break;
            }
        }
        if (uiLoop == uiLen)
        {
            *puiOutLen = uiLen;
        }
    }
    
    WE_FREE(pstPara);    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2DecryptCBC
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Decryt the input data with RC2 algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
static WE_INT32 We_LibRC2DecryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{  
    
    WE_INT32  iLen = (WE_INT32)uiInLen;
    St_WeRC2Parameter*    pstPara = NULL;
    WE_INT32              iRes    = 0;
    WE_INT32              iIndex  = 0;
    WE_INT32              iIndexJ = 0; 
    WE_UCHAR              aucInTmp[WE_RC2_BLOCK_SIZE] = {0};  
    
    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeRC2Parameter*)handle;
    
    
    for (iIndexJ=0; iIndexJ<iLen; iIndexJ+=WE_RC2_BLOCK_SIZE)
    {
        for (iIndex=iIndexJ; iIndex<iIndexJ+WE_RC2_BLOCK_SIZE; iIndex++)
        {
            aucInTmp[iIndex-iIndexJ] = pucIn[iIndex];
        }
        iRes = We_LibRC2Decrypt( aucInTmp, WE_RC2_BLOCK_SIZE, &pucOut[iIndexJ], handle);
        if (iRes != M_WE_LIB_ERR_OK)
        {
            return iRes;
        }
        for (iIndex=0; iIndex<WE_RC2_BLOCK_SIZE; iIndex++)
        {
            pucOut[iIndexJ+iIndex] ^= ((pstPara->aucIv)[iIndex]);
        }
        
        (WE_VOID)WE_MEMCPY(pstPara->aucIv, aucInTmp, WE_RC2_BLOCK_SIZE);
    }
    
    return M_WE_LIB_ERR_OK;        
}

/*==================================================================================================
FUNCTION: 
    We_LibRC2Decrypt
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Decryt the input data which is one WE_RC2_BLOCK_SIZE with RC2 algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_HANDLE       Handle[IN]: the handle of RC2 algorithm.
RETURN VALUE:
    M_WE_LIB_ERR_OK: success.
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
static WE_INT32 We_LibRC2Decrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{
    St_WeRC2Parameter*   pstPara   = NULL;
    WE_INT32     iIndexI = 0;
    WE_UINT16    usIn76 = 0;
    WE_UINT16    usIn54 = 0;
    WE_UINT16    usIn32 = 0;
    WE_UINT16    usIn10 = 0;
    
    if ( (WE_RC2_BLOCK_SIZE != uiInLen) || (!pucIn) || (!pucOut) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeRC2Parameter *)handle;

    usIn76 = (pucIn[7] << 8) + pucIn[6];
    usIn54 = (pucIn[5] << 8) + pucIn[4];  
    usIn32 = (pucIn[3] << 8) + pucIn[2];
    usIn10 = (pucIn[1] << 8) + pucIn[0];
    
    for (iIndexI=15; iIndexI>=0; iIndexI--)
    {
        usIn76 = (WE_UINT16)((usIn76 << 11) + (usIn76 >> 5));
        usIn76 -= (usIn10 & ~usIn54) + (usIn32 & usIn54) + (pstPara->ausXKey)[4*iIndexI+3];

        usIn54 = (WE_UINT16)((usIn54 << 13) + (usIn54 >> 3));        
        usIn54 -= (usIn76 & ~usIn32) + (usIn10 & usIn32) + (pstPara->ausXKey)[4*iIndexI+2];

        usIn32 = (WE_UINT16)((usIn32 << 14) + (usIn32 >> 2));
        usIn32 -= (usIn54 & ~usIn10) + (usIn76 & usIn10) + (pstPara->ausXKey)[4*iIndexI+1];

        usIn10 = (WE_UINT16)((usIn10 << 15) + (usIn10 >> 1));         
        usIn10 -= (usIn32 & ~usIn76) + (usIn54 & usIn76) + (pstPara->ausXKey)[4*iIndexI+0];   
        
        if (iIndexI == 5 || iIndexI == 11)
        {
            usIn76 -= (pstPara->ausXKey)[usIn54 & 0x3f];
            usIn54 -= (pstPara->ausXKey)[usIn32 & 0x3f];
            usIn32 -= (pstPara->ausXKey)[usIn10 & 0x3f];
            usIn10 -= (pstPara->ausXKey)[usIn76 & 0x3f];
        }
    }
    
    pucOut[0] = (WE_UCHAR)usIn10;
    pucOut[1] = (WE_UCHAR)(usIn10 >> 8);
    pucOut[2] = (WE_UCHAR)usIn32;
    pucOut[3] = (WE_UCHAR)(usIn32 >> 8);
    pucOut[4] = (WE_UCHAR)usIn54;
    pucOut[5] = (WE_UCHAR)(usIn54 >> 8);
    pucOut[6] = (WE_UCHAR)usIn76;
    pucOut[7] = (WE_UCHAR)(usIn76 >> 8);
    
    return M_WE_LIB_ERR_OK;
}

/*************************************************************************************************
end
*************************************************************************************************/



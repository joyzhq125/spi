/*=====================================================================================
    FILE NAME :
        we_idea.c
    MODULE NAME :
        WE  
    GENERAL DESCRIPTION
        This File will handle the operations on Idea, and will 
        be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
    =======================================================================================
    Revision History
      
    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-11-14      Stone An       None           create we_idea.c
        
=====================================================================================*/

/*******************************************************************************
*   Include File Section
*******************************************************************************/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_idea.h"

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeIDEAParameter
{
    WE_UINT32       uiType;
    WE_UINT16       ausSecret[52];
    WE_UCHAR        aucIv[WE_IDEA_BLOCK_SIZE];
    WE_UCHAR        aucBuf[WE_IDEA_BLOCK_SIZE];
    WE_INT32        iBufLen;
}St_WeIDEAParameter;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

static WE_INT32 We_LibIDEAEncryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

static WE_INT32 We_LibIDEAEncrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

static WE_INT32 We_LibIDEADecryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

static WE_INT32 We_LibIDEADecrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
);

static WE_INT32 We_LibIDEAKeyRotLeftMove(WE_UCHAR *pucKey);
static WE_UINT16 We_LibIDEAMul(WE_UINT16 usA, WE_UINT16 usB);
static WE_INT32 We_LibIDEAHandle(WE_UINT16 *pusData1, WE_UINT16 *pusSecret);
static WE_UINT16 We_LibIDEAMulInv(WE_UINT16 usX);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptNoPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    IDEA encrypt with no padding. The length of input is divided exactly by WE_IDEA_BLOCK_SIZE.
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
WE_INT32 We_LibIDEAEncryptNoPadding
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
    
    iRes = We_LibIDEAEncryptInit( stKey, &hHandle);    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibIDEAEncryptUpdate( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibIDEAEncryptFinal( hHandle);
        }
        else
        {
            iRes = We_LibIDEAEncryptFinal( hHandle);
            return iRv;
        }
    }
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    IDEA encrypt with padding. The length of input is not divided exactly by WE_IDEA_BLOCK_SIZE.
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
WE_INT32 We_LibIDEAEncryptPadding
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
    
    iRes = We_LibIDEAEncryptInitPad( stKey, &hHandle);    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibIDEAEncryptUpdatePad( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        uiTmpLen -= (*puiOutLen);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibIDEAEncryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
        }
        else
        {
            iRes = We_LibIDEAEncryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
            return iRv;
        }
    }

    (*puiOutLen) += uiTmpLen;
    
    return iRes;
}


/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptInit
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize IDEA algorithm without padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of IDEA.
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
WE_INT32 We_LibIDEAEncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    St_WeIDEAParameter*   pstPara   = NULL;    
    WE_INT32 iIndexI = 0;
    WE_INT32 iIndexJ = 0;
    WE_UCHAR  *pucTmpSecret = NULL;
    WE_UINT16 *pusTmpKey = NULL;
    
    if ((stKey.iKeyLen != 16) || (stKey.pucKey == NULL)||!pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if(WE_IDEA_BLOCK_SIZE != stKey.iIvLen)
    {
        return M_WE_LIB_ERR_KEY_LENGTH;
    } 

    pusTmpKey = (WE_UINT16 *)WE_MALLOC((WE_UINT32)(stKey.iKeyLen / 2) * sizeof(WE_UINT16));
    if (NULL == pusTmpKey)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }    
    (WE_VOID)WE_MEMCPY((WE_UINT8 *)pusTmpKey, stKey.pucKey, (WE_UINT32)stKey.iKeyLen);
    pucTmpSecret = (WE_UINT8 *)pusTmpKey;
    pstPara = (St_WeIDEAParameter *)WE_MALLOC(sizeof(St_WeIDEAParameter));
    if (NULL == pstPara)
    {
        WE_FREE(pucTmpSecret);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMSET(pstPara, 0, sizeof(St_WeIDEAParameter));
    /* expand the private key to 52 bytes. */
    for (iIndexI=0; iIndexI<6; iIndexI++)
    {
        for (iIndexJ=0; iIndexJ<8; iIndexJ++)
        {
            (pstPara->ausSecret)[8*iIndexI + iIndexJ] = pusTmpKey[iIndexJ];
        }
        
        (WE_VOID)We_LibIDEAKeyRotLeftMove(pucTmpSecret); /* the 128 bits secret move left rotate 25 bits.*/ 
    }    
    
    for (iIndexJ=0; iIndexJ<4; iIndexJ++)
    {
        (pstPara->ausSecret)[48 + iIndexJ] = pusTmpKey[iIndexJ];
    }
    
    pstPara->uiType = M_WE_IDEA_TYPE;
    (WE_VOID)WE_MEMCPY(pstPara->aucIv, stKey.pucIv, WE_IDEA_BLOCK_SIZE);
    *pHandle = pstPara;
    WE_FREE(pucTmpSecret);
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptInitPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize IDEA algorithm using with padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of IDEA.
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
WE_INT32 We_LibIDEAEncryptInitPad
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    return We_LibIDEAEncryptInit( stKey, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptUpdate
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    encrypt data with IDEA algorithm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of IDEA.
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
WE_INT32 We_LibIDEAEncryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    WE_UINT32             uiLen     = 0; 

    if ( (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if (*puiOutLen >= uiInLen )
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    uiLen = uiInLen % WE_IDEA_BLOCK_SIZE;
    if(0 == uiLen)
    {        
        (WE_VOID)We_LibIDEAEncryptCBC( pucIn, uiInLen, pucOut, handle);
    }
    else
    {
        return M_WE_LIB_ERR_DATA_LENGTH;
    }

    return M_WE_LIB_ERR_OK;    
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptUpdatePad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    encrypt data with IDEA algorithm using with padding.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of IDEA.
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
WE_INT32 We_LibIDEAEncryptUpdatePad
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
    St_WeIDEAParameter*   pstPara   = NULL;

    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeIDEAParameter*)handle;
    uiIndex = (WE_UINT32)(pstPara->iBufLen);

    uiRealInLen = ((uiInLen + uiIndex) / WE_IDEA_BLOCK_SIZE) * WE_IDEA_BLOCK_SIZE;
    if (*puiOutLen >= uiRealInLen)
    {
        *puiOutLen = uiRealInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    pstPara->iBufLen = (WE_INT32)(uiInLen + uiIndex - uiRealInLen);
    (WE_VOID)WE_MEMCPY((pstPara->aucBuf + uiIndex), pucIn, (WE_UINT32)(WE_IDEA_BLOCK_SIZE - uiIndex));
    (WE_VOID)We_LibIDEAEncryptCBC( pstPara->aucBuf, WE_IDEA_BLOCK_SIZE, pucOut, handle);
    if (WE_IDEA_BLOCK_SIZE != *puiOutLen)
    {
        (WE_VOID)We_LibIDEAEncryptCBC( (pucIn + (WE_IDEA_BLOCK_SIZE - uiIndex)),            
                uiRealInLen-WE_IDEA_BLOCK_SIZE, pucOut+WE_IDEA_BLOCK_SIZE, handle);
    }
    uiIndex = uiInLen - (WE_UINT32)(pstPara->iBufLen);
    (WE_VOID)WE_MEMCPY(pstPara->aucBuf, pucIn + uiIndex, (WE_UINT32)(pstPara->iBufLen));
    
    return M_WE_LIB_ERR_OK;    
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptFinal
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle of IDEA algorithm, release the handle of IDEA algorithm.
ARGUMENTS PASSED:
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
WE_INT32 We_LibIDEAEncryptFinal(WE_HANDLE handle)
{
    St_WeIDEAParameter*   pstPara   = NULL;
    
    if(NULL == handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeIDEAParameter *)handle;
    WE_FREE(pstPara);    

    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncryptFinalPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle with pad. Encryt the left plain data and then release the handle of IDEA algorthm.
ARGUMENTS PASSED:
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
WE_INT32 We_LibIDEAEncryptFinalPad
(
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,  
    WE_HANDLE handle
)
{
    St_WeIDEAParameter*   pstPara   = NULL;
    WE_UCHAR             aucData[WE_IDEA_BLOCK_SIZE]    = {0};
    
    if((NULL == handle) || (NULL == pucOut) || (NULL == puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    

    pstPara = (St_WeIDEAParameter*)handle;
    
    if (pstPara->iBufLen != 0)
    {  
        if (*puiOutLen < WE_IDEA_BLOCK_SIZE)
        {
            return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
        }
        *puiOutLen = WE_IDEA_BLOCK_SIZE;
        (WE_VOID)WE_MEMSET(aucData, (WE_UCHAR)(WE_IDEA_BLOCK_SIZE-pstPara->iBufLen), (WE_UINT32)WE_IDEA_BLOCK_SIZE);
        (WE_VOID)WE_MEMCPY(aucData, pstPara->aucBuf, (WE_UINT32)pstPara->iBufLen);
        (WE_VOID)We_LibIDEAEncryptCBC( aucData, WE_IDEA_BLOCK_SIZE, pucOut, handle);
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
    We_LibIDEAEncryptCBC
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Encryt the input data with IDEA algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
static WE_INT32 We_LibIDEAEncryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{  
    
    WE_INT32  iLen = (WE_INT32)uiInLen;
    St_WeIDEAParameter*    pstPara = NULL;
    WE_INT32              iRes    = 0;
    WE_INT32              iIndex  = 0;
    WE_INT32              iIndexJ = 0; 
    WE_UCHAR              aucInTmp[WE_IDEA_BLOCK_SIZE] = {0}; 
    
    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    pstPara = (St_WeIDEAParameter*)handle;
    
    for (iIndexJ=0; iIndexJ<iLen; iIndexJ+=WE_IDEA_BLOCK_SIZE)
    {
        for (iIndex=0; iIndex<WE_IDEA_BLOCK_SIZE; iIndex++)
        {
            aucInTmp[iIndex] = pucIn[iIndex+iIndexJ];
            aucInTmp[iIndex] ^= ((pstPara->aucIv)[iIndex]);
        }
        iRes = We_LibIDEAEncrypt( aucInTmp, WE_IDEA_BLOCK_SIZE, &pucOut[iIndexJ], handle);
        if (iRes != M_WE_LIB_ERR_OK)
        {
            return iRes;
        }

        (WE_VOID)WE_MEMCPY(pstPara->aucIv, &pucOut[iIndexJ], WE_IDEA_BLOCK_SIZE);
    }

    return M_WE_LIB_ERR_OK;        
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAEncrypt
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Encryt the input data which is one WE_IDEA_BLOCK_SIZE with IDEA algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be encrypted.
    WE_UINT32       uiInLen[IN]: length of input data, which is must be WE_IDEA_BLOCK_SIZE.
    WE_UCHAR*       pucOut[OUT]: the encrypted data.
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
static WE_INT32 We_LibIDEAEncrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{
    St_WeIDEAParameter*   pstPara   = (St_WeIDEAParameter *)handle;
    WE_INT32              iIndexI = 0;
    WE_INT32              iRes = 0;
    WE_UINT16             usTmp = 0;
    WE_UINT16             *pusData = NULL;
    
    if ( (WE_IDEA_BLOCK_SIZE != uiInLen) || (!pucIn) || (!pucOut) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    (WE_VOID)WE_MEMCPY(pucOut, pucIn, WE_IDEA_BLOCK_SIZE);
    
    pusData = (WE_UINT16 *)WE_MALLOC(WE_IDEA_BLOCK_SIZE / 2 * sizeof(WE_UINT16));
    if (NULL == pusData)
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    //pusTmp = pusData;
    (WE_VOID)WE_MEMCPY((WE_UCHAR *)pusData, pucOut, WE_IDEA_BLOCK_SIZE);
    
    for (iIndexI=0; iIndexI<8; iIndexI++)
    {
        iRes = We_LibIDEAHandle(pusData, &(pstPara->ausSecret)[6*iIndexI]);
        if (iRes != M_WE_LIB_ERR_OK)
        {
            WE_FREE(pusData);
            return iRes;
        }
    }
    usTmp = pusData[1];
    pusData[1] = pusData[2];
    pusData[2] = usTmp;
    
    pusData[0] = We_LibIDEAMul(pusData[0], (pstPara->ausSecret)[48]);
    pusData[1] += pstPara->ausSecret[49];
    pusData[2] += pstPara->ausSecret[50];
    pusData[3] = We_LibIDEAMul(pusData[3], (pstPara->ausSecret)[51]);
    
    (WE_VOID)WE_MEMCPY(pucOut, (WE_UCHAR *)pusData, WE_IDEA_BLOCK_SIZE);
    WE_FREE(pusData);
    
    return M_WE_LIB_ERR_OK;    
}


/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptNoPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    IDEA decrypt with no padding. The length of input is divided exactly by WE_IDEA_BLOCK_SIZE.
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
WE_INT32 We_LibIDEADecryptNoPadding
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

    if (0 != uiInLen % WE_IDEA_BLOCK_SIZE)
    {
        return M_WE_LIB_ERR_DATA_LENGTH;
    }
    
    iRes = We_LibIDEADecryptInit( stKey, &hHandle);
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibIDEADecryptUpdate( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibIDEADecryptFinal( hHandle);
        }
        else
        {
            iRes = We_LibIDEADecryptFinal( hHandle);
            return iRv;
        }
    }
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptPadding
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    IDEA decrypt with padding. The length of input is not divided exactly by WE_IDEA_BLOCK_SIZE.
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
WE_INT32 We_LibIDEADecryptPadding
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
    
    iRes = We_LibIDEADecryptInitPad( stKey, &hHandle);    
    if (iRes == M_WE_LIB_ERR_OK)
    {
        iRv = We_LibIDEADecryptUpdatePad( pucIn, uiInLen, pucOut, puiOutLen, hHandle);
        if (iRv == M_WE_LIB_ERR_OK)
        {
            iRes = We_LibIDEADecryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
        }
        else
        {
            iRes = We_LibIDEADecryptFinalPad( pucOut+(*puiOutLen), &uiTmpLen, hHandle);
            return iRv;
        }
    }

    (*puiOutLen) -= uiTmpLen;
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptInit
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize IDEA algorithm without padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of IDEA.
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
WE_INT32 We_LibIDEADecryptInit
( 
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32  iIndexI = 0;
    WE_UINT8 aucKeyPosChange[52] = {48,49,50,51,46,47, 
        42,44,43,45,40,41,
        36,38,37,39,34,35,
        30,32,31,33,28,29,
        24,26,25,27,22,23,
        18,20,19,21,16,17,
        12,14,13,15,10,11,
        6, 8, 7, 9, 4, 5,
        0, 1, 2, 3 
        };
    WE_UINT8 aucMulRevPos[18] = {
        0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51}; 
    WE_UINT8 aucAddRevPos[18] ={
            1,2,7,8,13,14,19,20,25,26,31,32,37,38,43,44,49,50}; 
    St_WeIDEAParameter*    pstPara   = NULL;
    WE_UINT16              ausEncyptSecret[52] = {0};
    WE_INT32               iRes = M_WE_LIB_ERR_OK;    
            
    if (NULL == pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    iRes = We_LibIDEAEncryptInit( stKey, pHandle);
    if (iRes != M_WE_LIB_ERR_OK)
    {
        return iRes;
    }
    
    pstPara = (St_WeIDEAParameter *)(*pHandle);
    for (iIndexI=0 ; iIndexI<52 ; iIndexI++)
    { 
        ausEncyptSecret[iIndexI] = (pstPara->ausSecret)[iIndexI];
    }

    for (iIndexI=0 ; iIndexI<52 ; iIndexI++)
    { 
        (pstPara->ausSecret)[iIndexI] = ausEncyptSecret[aucKeyPosChange[iIndexI]];
    }
    
    for (iIndexI=0 ; iIndexI<18 ; iIndexI++) /* reverse of add mod 65536. */
    {
        (pstPara->ausSecret)[aucAddRevPos[iIndexI]] = (WE_UINT16)(65536 - (pstPara->ausSecret)[aucAddRevPos[iIndexI]]);
    }

    for (iIndexI=0 ; iIndexI<18 ; iIndexI++) /* reverse of multiply mod 65537. */
    {
        (pstPara->ausSecret)[aucMulRevPos[iIndexI]] = We_LibIDEAMulInv((pstPara->ausSecret)[aucMulRevPos[iIndexI]]);
    }
    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptInitPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    initialize IDEA algorithm using with padding.
ARGUMENTS PASSED:
    St_WeCipherKey  stKey[IN]: secret key.
    WE_HANDLE*      pHandle[OUT]: the handle of IDEA.
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
WE_INT32 We_LibIDEADecryptInitPad
( 
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    return We_LibIDEADecryptInit( stKey, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptUpdate
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    decrypt data with IDEA algorithm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of IDEA.
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
WE_INT32 We_LibIDEADecryptUpdate
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

    uiLen = uiInLen % WE_IDEA_BLOCK_SIZE;
    if(0 == uiLen)
    {        
        (WE_VOID)We_LibIDEADecryptCBC( pucIn, uiInLen, pucOut, handle);
    }
    else
    {
        return M_WE_LIB_ERR_DATA_LENGTH;
    }

    return M_WE_LIB_ERR_OK;    
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptUpdatePad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    decrypt data with IDEA algorithm using with padding.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of output buffer when IN, 
                                       the length of encrypted data when OUT.
    WE_HANDLE       Handle[IN]: the handle of IDEA.
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
WE_INT32 We_LibIDEADecryptUpdatePad
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    WE_UINT32             uiRealInLen = 0;
    WE_UINT32             uiIndex     = 0;
    St_WeIDEAParameter*   pstPara     = NULL;
    WE_INT32              iRes        = 0;

    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeIDEAParameter*)handle;
    uiIndex = (WE_UINT32)(pstPara->iBufLen);

    uiRealInLen = ((uiInLen + uiIndex) / WE_IDEA_BLOCK_SIZE) * WE_IDEA_BLOCK_SIZE;
    if (*puiOutLen >= uiRealInLen)
    {
        *puiOutLen = uiRealInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    pstPara->iBufLen = (WE_INT32)(uiInLen + uiIndex - uiRealInLen);
    (WE_VOID)WE_MEMCPY((pstPara->aucBuf + uiIndex), pucIn, (WE_UINT32)(WE_IDEA_BLOCK_SIZE - uiIndex));
    iRes = We_LibIDEADecryptCBC( pstPara->aucBuf, WE_IDEA_BLOCK_SIZE, pucOut, handle);
    if (WE_IDEA_BLOCK_SIZE != *puiOutLen)
    {
        iRes = We_LibIDEADecryptCBC( (pucIn + (WE_IDEA_BLOCK_SIZE - uiIndex)),            
                uiRealInLen-WE_IDEA_BLOCK_SIZE, pucOut+WE_IDEA_BLOCK_SIZE, handle);
    }
    uiIndex = uiInLen - (WE_UINT32)(pstPara->iBufLen);
    (WE_VOID)WE_MEMCPY(pstPara->aucBuf, pucIn + uiIndex, (WE_UINT32)(pstPara->iBufLen));
    
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptFinal
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle of IDEA algorithm, release the handle of IDEA algorithm.
ARGUMENTS PASSED:
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
WE_INT32 We_LibIDEADecryptFinal(WE_HANDLE handle)
{
    return We_LibIDEAEncryptFinal( handle);
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecryptFinalPad
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    the final handle with pad. compute the length of padding data.
ARGUMENTS PASSED:
    WE_UCHAR*       pucOut[OUT]: the end of decrypted data.
    WE_UINT32*      puiOutLen[IN/OUT]: the length of padding data.
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
WE_INT32 We_LibIDEADecryptFinalPad
(
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,  
    WE_HANDLE handle
)
{
    St_WeIDEAParameter*   pstPara = NULL;
    WE_UINT32             uiLen = 0;
    
    if((NULL == handle) || (NULL == pucOut) || (NULL == puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstPara = (St_WeIDEAParameter*)handle;
    
    *puiOutLen = 0;
    uiLen = *(pucOut - 1);
    if ((uiLen > 0) && (uiLen < WE_IDEA_BLOCK_SIZE))
    {
        WE_UINT32   uiLoop = 0;
        for (; uiLoop < uiLen; uiLoop++)
        {
            if (uiLen != *(pucOut - 1 - uiLoop))
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
    We_LibIDEADecryptCBC
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Decryt the input data with IDEA algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
static WE_INT32 We_LibIDEADecryptCBC
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{  
    
    WE_INT32  iLen = (WE_INT32)uiInLen;
    St_WeIDEAParameter*    pstPara = NULL;
    WE_INT32              iRes    = 0;
    WE_INT32              iIndex  = 0;
    WE_INT32              iIndexJ = 0; 
    WE_UCHAR              aucInTmp[WE_IDEA_BLOCK_SIZE] = {0};  
    
    if( (0 == uiInLen) || (!pucIn) || (!pucOut) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeIDEAParameter*)handle;
    
    
    for (iIndexJ=0; iIndexJ<iLen; iIndexJ+=WE_IDEA_BLOCK_SIZE)
    {
        for (iIndex=iIndexJ; iIndex<iIndexJ+WE_IDEA_BLOCK_SIZE; iIndex++)
        {
            aucInTmp[iIndex-iIndexJ] = pucIn[iIndex];
        }
        iRes = We_LibIDEADecrypt( aucInTmp, WE_IDEA_BLOCK_SIZE, &pucOut[iIndexJ], handle);
        if (iRes != M_WE_LIB_ERR_OK)
        {
            return iRes;
        }
        for (iIndex=0; iIndex<WE_IDEA_BLOCK_SIZE; iIndex++)
        {
            pucOut[iIndexJ+iIndex] ^= ((pstPara->aucIv)[iIndex]);
        }
        
        (WE_VOID)WE_MEMCPY(pstPara->aucIv, &pucIn[iIndexJ], WE_IDEA_BLOCK_SIZE);
    }
    
    return M_WE_LIB_ERR_OK;        
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEADecrypt
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    Decryt the input data which is one WE_IDEA_BLOCK_SIZE with IDEA algorthm.
ARGUMENTS PASSED:
    const WE_UCHAR* pucIn[IN]: the data need to be decrypted.
    WE_UINT32       uiInLen[IN]: length of input data.
    WE_UCHAR*       pucOut[OUT]: the decrypted data.
    WE_HANDLE       Handle[IN]: the handle of IDEA algorithm.
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
static WE_INT32 We_LibIDEADecrypt
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_HANDLE handle
)
{
    return We_LibIDEAEncrypt( pucIn, uiInLen, pucOut, handle);
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAKeyRotLeftMove
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    rotate left 25 bits.
ARGUMENTS PASSED:
    WE_UCHAR *pucKey[IN/OUT]: the secret need to rotate and the rotated result.
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
static WE_INT32 We_LibIDEAKeyRotLeftMove(WE_UCHAR *pucKey)
{ 
    WE_INT32 iIndexI = 0;
    WE_INT32 iIndexJ = 0;
    WE_UCHAR ucCy = 0;
    WE_UCHAR ucFirstCy = 0;
    WE_UCHAR ucTmp = 0;

    if (NULL == pucKey)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    ucFirstCy = (pucKey[0] >> 7) & 0x01;

    /* rotate one bit. */
    for (iIndexI=0; iIndexI<15; iIndexI++)
    {
        ucCy = (pucKey[iIndexI+1] >> 7) & 0x01;
        pucKey[iIndexI] = (WE_UINT8)((pucKey[iIndexI]<<1) | (ucCy));
    }
    pucKey[15] = (WE_UINT8)((pucKey[15]<<1) | (ucFirstCy));

    /* rotate 3*8 bits. */
    for (iIndexI=0; iIndexI<3; iIndexI++)
    {
        ucTmp = pucKey[0];
        for (iIndexJ=0; iIndexJ<15; iIndexJ++)
        {
            pucKey[iIndexJ] = pucKey[iIndexJ+1];
        }
        pucKey[15] = ucTmp;
    }
    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAHandle
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    encrypt or decrypt data using secret.
ARGUMENTS PASSED:
    WE_UINT16 *pusData[IN/OUT]: the data to be encrypted or decrypted when IN.
                                the encrypted or decrypted data when OUT.
    WE_UINT16 *pusSecret[IN]: the secret used for handling.
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
static WE_INT32 We_LibIDEAHandle(WE_UINT16 *pusData1, WE_UINT16 *pusSecret)
{
    WE_UINT16 usD1 = 0;
    WE_UINT16 usD2 = 0;
    WE_UINT16 usD3 = 0;
    WE_UINT16 usD4 = 0;
    
    WE_UINT16 usD57 = 0;
    WE_UINT16 usD68 = 0;
    WE_UINT16 *pusData = pusData1;
    
    if ((NULL == pusSecret) || (NULL == pusData))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 

    usD1 = pusData[0];
    usD2 = pusData[1];
    usD3 = pusData[2];
    usD4 = pusData[3];

    usD1 = We_LibIDEAMul(usD1, pusSecret[0]);
    usD2 += pusSecret[1];
    usD3 += pusSecret[2];
    usD4 = We_LibIDEAMul(usD4, pusSecret[3]);
    usD57 = usD1 ^ usD3;
    usD68 = usD2 ^ usD4;
    usD57 = We_LibIDEAMul(usD57, pusSecret[4]);
    usD68 += usD57;
    usD68 = We_LibIDEAMul(usD68, pusSecret[5]);
    usD57 += usD68;
    usD1 ^= usD68;
    usD3 ^= usD68;
    usD2 ^= usD57;
    usD4 ^= usD57;
    
    pusData[0] = usD1;
    pusData[1] = usD3; /* change 1 and 2. */
    pusData[2] = usD2;
    pusData[3] = usD4;
    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibIDEAMul
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    multiply mod 65537.
ARGUMENTS PASSED:
    WE_UINT16 usA: one factor.
    WE_UINT16 usB: another factor.
RETURN VALUE:
    result of multiply.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT16 We_LibIDEAMul(WE_UINT16 usA, WE_UINT16 usB)
{
    WE_UINT32 uiRes = 0;

    if ((0 == usA) && (0 == usB))
    {
        uiRes = 0;
    }
    else if (0 == usA)
    {
        uiRes = (65536) * (WE_UINT32)usB; 
    }
    else if (0 == usB)
    {
        uiRes = (65536) * (WE_UINT32)usA; 
    }
    else
    {
        uiRes = (WE_UINT32)usA * (WE_UINT32)usB;
    }

    return (WE_UINT16)(uiRes % (65537));
}

#define LOW16(x) ((x) & 0xffff)
/*==================================================================================================
FUNCTION: 
    We_LibIDEAMulInv
CREATE DATE:
    2006-11-23
AUTHOR:
    stone An
DESCRIPTION:
    reverse of multiply mod 65537.
ARGUMENTS PASSED:
    WE_UINT16 usX: one factor.
RETURN VALUE:
    result of reverse of multiply.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    omit.
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT16 We_LibIDEAMulInv(WE_UINT16 usX)
{
    WE_UINT16 usT0 = 0;
    WE_UINT16 usT1 = 0;    
    WE_UINT16 usQ = 0;
    WE_UINT16 usY = 0;
    
    if (usX <= 1)        
    {       
        return usX; 
    }
    
    usT1 = (WE_UINT16)(0x10001L / usX);    
    usY = (WE_UINT16)(0x10001L % usX);    
    if (usY == 1)        
    {        
        return LOW16(1-usT1);        
    }
    
    usT0 = 1 ;    
    do    
    {        
        usQ = usX / usY; 
        usX %= usY;        
        usT0 += (WE_UINT16)(usQ * usT1);        
        if( usX == 1)            
        {            
            return usT0;            
        }        
        usQ = usY / usX;        
        usY %= usX;        
        usT1 += (WE_UINT16)(usQ * usT0);        
    }while(usY != 1);
    
    return LOW16(1-usT1);    
}

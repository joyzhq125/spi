/*=====================================================================================
    FILE NAME : we_rc4.c
    MODULE NAME : WE
    
    GENERAL DESCRIPTION
        This File will handle the operations on Rc4, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
      
    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-11-14      Stone An       None           create we_rc4.c
        
=====================================================================================*/

/*******************************************************************************
*   Include File Section
*******************************************************************************/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_rc4.h"


/*******************************************************************************
*   Macro Define
*******************************************************************************/
#define M_RC4_EXCHANGE(x,y) {\
    WE_UINT8 ucTmp;\
    ucTmp = (x); \
    (x) = (y); \
    (y) = ucTmp;}


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeRC4Parameter
{
    WE_UINT16   usIndexI;
    WE_UINT16   usIndexJ;
    WE_INT32    iRounds;
    WE_UINT8    *pucArrayS; /* S-Box */
}St_WeRC4Parameter;


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    We_LibRC4EncryptInit
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an
DESCRIPTION:
    initialise the RC4 encrypt.
ARGUMENTS PASSED:
    WE_UINT16          usRounds[IN]: round of RC4 operation.
    St_WeCipherKey     stKey[IN]: the key using in RC4 algorithm.
    WE_HANDLE*         pHandle[OUT]: handler of RC4 algorithm.
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
WE_INT32 We_LibRC4EncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    St_WeRC4Parameter*   pstPara   = NULL;
    WE_INT32             iRv       = M_WE_LIB_ERR_OK;  
    WE_UINT16            usIndexI = 0;
    WE_UINT16            usIndexJ = 0;
 
    if ((NULL==stKey.pucKey) || (0>=stKey.iKeyLen) || (NULL==pHandle))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    pstPara = (St_WeRC4Parameter *)WE_MALLOC(sizeof(St_WeRC4Parameter));
    if (NULL == pstPara)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }    
    (WE_VOID)WE_MEMSET(pstPara, 0, sizeof(St_WeRC4Parameter));
    pstPara->iRounds = 256;
    pstPara->usIndexI = 0;
    pstPara->usIndexJ = 0;
    pstPara->pucArrayS = (WE_UINT8*)WE_MALLOC((WE_UINT32)pstPara->iRounds*sizeof(WE_UINT8));
    if (!pstPara->pucArrayS)
    {
        WE_LIB_FREE(pstPara);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    /* initialize the array S in RC4 parameter. */      
    for (usIndexI=0; usIndexI < pstPara->iRounds; usIndexI++)
    {
        pstPara->pucArrayS[usIndexI] = (WE_UINT8)usIndexI;
    }
    usIndexJ = 0;
    for (usIndexI=0; usIndexI < pstPara->iRounds; usIndexI++)
    {
        usIndexJ = (usIndexJ + pstPara->pucArrayS[usIndexI] + stKey.pucKey[usIndexI % stKey.iKeyLen]) % 256;
        M_RC4_EXCHANGE(pstPara->pucArrayS[usIndexI], pstPara->pucArrayS[usIndexJ]);
    }
    
    *pHandle = (WE_HANDLE)pstPara;    
    return iRv;
}


/*==================================================================================================
FUNCTION: 
    We_LibRC4EncryptUpdate
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    encrypt data using RC4 algorithm.
ARGUMENTS PASSED:
    const WE_UCHAR*     pucIn[IN]: Pointer to the data.
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
WE_INT32 We_LibRC4EncryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    St_WeRC4Parameter*   pstPara   = NULL;
    WE_UINT16    usIndexI = 0;
    WE_UINT16    usIndexJ = 0;
    WE_ULONG     ulIndexK = 0;
    WE_UINT16    usRounds = 0;

    if ( (0 == uiInLen) || (!pucIn) || (!pucOut) || (NULL == puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    pstPara = (St_WeRC4Parameter *)handle;
    usRounds = 256;
    usIndexI = pstPara->usIndexI;
    usIndexJ = pstPara->usIndexJ;
    for (ulIndexK=0; ulIndexK<uiInLen; ulIndexK++)
    {
        usIndexI = (usIndexI + 1) % usRounds;
        usIndexJ = (usIndexJ + pstPara->pucArrayS[usIndexI]) % usRounds;
        M_RC4_EXCHANGE(pstPara->pucArrayS[usIndexI], pstPara->pucArrayS[usIndexJ]);
        pucOut[ulIndexK] = pucIn[ulIndexK] ^ pstPara->pucArrayS  \
            [(pstPara->pucArrayS[usIndexI] + pstPara->pucArrayS[usIndexJ]) % usRounds];        
    }
    *puiOutLen = uiInLen;
    pstPara->usIndexI = usIndexI;
    pstPara->usIndexJ = usIndexJ;
    return M_WE_LIB_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    We_LibRC4EncryptFinal
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    release the handler of RC4 algorithm.
ARGUMENTS PASSED:
    WE_HANDLE*         pHandle[OUT]: handler of RC4 algorithm.
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
WE_INT32 We_LibRC4EncryptFinal(WE_HANDLE handle)
{
    St_WeRC4Parameter*   pstPara   = NULL;

    if(NULL == handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    pstPara = (St_WeRC4Parameter *)handle;
    WE_LIB_FREEIF(pstPara->pucArrayS);
    pstPara->usIndexI = 0;
    pstPara->usIndexJ = 0;
    WE_FREE(pstPara);    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC4DecryptInit
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an
DESCRIPTION:
    initialise the RC4 decrypt.
ARGUMENTS PASSED:
    WE_UINT16          usRounds[IN]: round of RC4 operation.
    St_WeCipherKey     stKey[IN]: the key using in RC4 algorithm.
    WE_HANDLE*         pHandle[OUT]: handler of RC4 algorithm.
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
WE_INT32 We_LibRC4DecryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    return We_LibRC4EncryptInit(stKey, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_LibRC4DecryptUpdate
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    decrypt the encypted data by RC4 algorithm.
ARGUMENTS PASSED:
    const WE_UCHAR*     pucIn[IN]: Pointer to the encypted data.
    WE_INT32            iInLen[IN]: length of the encypted data.
    WE_UCHAR*           pucOut[OUT]: Pointer to the decrypted data.
    WE_INT32*           piOutLen[IN/OUT]: length of the decrypted data.
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
WE_INT32 We_LibRC4DecryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen,
    WE_HANDLE handle
)
{
    return We_LibRC4EncryptUpdate( pucIn, uiInLen, pucOut, puiOutLen, handle);    
}

/*==================================================================================================
FUNCTION: 
    We_LibRC4DecryptFinal
CREATE DATE:
    2006-11-14
AUTHOR:
    stone an.
DESCRIPTION:
    release the handler of RC4 algorithm.
ARGUMENTS PASSED:
    WE_HANDLE*         pHandle[OUT]: handler of RC4 algorithm.
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
WE_INT32 We_LibRC4DecryptFinal(WE_HANDLE handle)
{
    return We_LibRC4EncryptFinal( handle);
}





/*=====================================================================================
    FILE   NAME : we_tripdes.c
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
#include "we_des.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/ 



/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibStartTripleDes
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set encrypt key and encrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    pstMode[IN]: mode param;
    pstKeys[IN]: cipher key;
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
WE_INT32 We_LibStartTripleDes
(
    St_WeLibBsapiMode *pstMode,
    St_WeCipherCrptKey stKey, 
    WE_HANDLE *pHandle
)
{
    St_WeLibBsapiMode newmode;
    WE_INT32 iResult=0;
    CHECK_FOR_NULL_ARG(pstMode, 0);
    (WE_VOID)WE_MEMCPY(&newmode, pstMode, sizeof(*pstMode)); 
    newmode.eTriple = BSAPI_TRIPLE;
    iResult = We_LibStartDesCore(&newmode,&stKey,pHandle);
    return iResult;
}
/*=====================================================================================
FUNCTION: 
    We_LibAppendTripleDes
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    pucData[IN]: data to be encrypted;
    uiLength[IN]: length of data to be encrypted;
    pucOut[OUT]: encrypted data;
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
WE_INT32 We_LibAppendTripleDes
(
    WE_UCHAR *pucData, 
    WE_UINT32 uiLength,
    WE_UCHAR* pucOut, 
    WE_VOID *pHandle
)
{
    return We_LibAppendBlock(pucData, uiLength, pucOut,pHandle);
}

/***************************************************************************************************
*   Tripartite DES Encrypt                                                                    
***************************************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEncrypt
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set encrypt key and encrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pucIn[IN]: data to be encrypted;
    uiInLen[IN]: length of data to be encrypted;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
/* No Padding */
WE_INT32 We_Lib3DesEncrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL;
    WE_INT32 iResult=0;
    WE_UINT32 uiLen=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);
    
    stMd.eOp = BSAPI_ENCRYPT;
    iResult= We_LibStartTripleDes(&stMd,stKey,&handle);
    
    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    
    uiLen = uiInLen % 8;
    if(uiLen && (*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else if(!uiLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    if(uiInLen-uiLen>0)
    {
        iResult=We_LibAppendTripleDes((WE_UCHAR*)pucIn, uiInLen-uiLen,pucOut,handle);
    }
    if(iResult!=0)
    {
        return iResult;
    }
    if(uiLen)
    {
        WE_UCHAR aucData[8] = {0};
        (WE_VOID)WE_MEMSET(aucData,0,8);
        (WE_VOID)WE_MEMCPY(aucData,(pucIn + uiInLen - uiLen),uiLen);   
        if(uiInLen==uiLen)
        {
            iResult=We_LibAppendTripleDes((WE_UCHAR*)aucData, 8,pucOut,handle);
        }
        else
        {
            iResult=We_LibAppendTripleDes((WE_UCHAR*)aucData, 8,(pucOut+ uiInLen - uiLen),handle);
        }
    } 
    We_LibKillDes(handle);
    return M_WE_LIB_ERR_OK;
} 
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEncryptInit
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt init with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IO]: cipher key
    pHandle[OUT]: data handle;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_Lib3DesEncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};
    
    if (NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_ENCRYPT;
    iResult= We_LibStartTripleDes(&stMd,stKey,pHandle);  

    return iResult;
}
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEnCryptUpdate
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be encrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_Lib3DesEnCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_UINT32 uiLen=0;
    WE_INT32 iResult=0;
    if(!pucIn||!uiInLen||!handle||!puiOutLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    uiLen = uiInLen % 8;
    if(*puiOutLen >= ((1 + uiInLen / 8) * 8))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }    
    if(0 == uiLen)
    {        
        iResult=We_LibAppendTripleDes((WE_UCHAR *)pucIn, uiInLen,pucOut, handle);  
    }
    else 
    {
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    return iResult;    
    
}
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEnCryptFinal
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Free the data handle.
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
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_Lib3DesEnCryptFinal(WE_HANDLE handle) 
{
    
    if(NULL==handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    We_LibKillDes(handle);
    return 0;    
}
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEncrypt1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set sub encrypt key and padding data, encrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pucIn[IN]: data to be encrypted;
    uiInLen[IN]: length of data to be encrypted;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
/* RFC2630 Padding */
WE_INT32 We_Lib3DesEncrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL;
    WE_INT32 iResult=0;
    WE_UINT32 uiLen=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);

    stMd.eOp = BSAPI_ENCRYPT;
    iResult= We_LibStartTripleDes(&stMd,stKey,&handle); 

    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    
    uiLen = uiInLen % 8;
    if(uiLen && (*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else if(!uiLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    if(uiInLen-uiLen>0)
    {
        iResult=We_LibAppendTripleDes((WE_UCHAR*)pucIn, uiInLen-uiLen,pucOut,handle);
    }
    if(iResult!=0)
    {
        return iResult;
    }
    if(uiLen)
    {
        WE_UCHAR aucData[8] = {0};
        (WE_VOID)WE_MEMSET(aucData,8-(WE_INT32)uiLen,8);
        (WE_VOID)WE_MEMCPY(aucData,(pucIn + uiInLen - uiLen),uiLen);   
        if(uiInLen==uiLen)
        {
            iResult=We_LibAppendTripleDes((WE_UCHAR*)aucData, 8,pucOut,handle);
        }
        else
        {
            iResult=We_LibAppendTripleDes((WE_UCHAR*)aucData, 8,(pucOut+ uiInLen - uiLen),handle);
        }
    }
    
    We_LibKillDes(handle);
    return M_WE_LIB_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEncryptInit1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt init with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key;
    pHandle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_Lib3DesEncryptInit1
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    if(NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_ENCRYPT;
    iResult= We_LibStartTripleDes(&stMd,stKey,pHandle);  

    return iResult;
}
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEnCryptUpdate1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set padding data and encrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be encrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_Lib3DesEnCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    St_WeBlockMAtchS *pstPara=NULL;
    WE_UINT32 iIndex=0;
    WE_INT32 iBufLen=0;
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen)||!puiOutLen||!pucOut)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }  
    
    iBufLen = (uiInLen % 8);
    if(iBufLen && (*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else if(!iBufLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    pstPara=(St_WeBlockMAtchS *)handle;
    
    iIndex = pstPara->iBufLen;
    if((uiInLen + (WE_UINT32)iIndex) == *puiOutLen)
    {
        pstPara->iBufLen = 0;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendTripleDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
        if(8 != *puiOutLen)
        {
            iResult=We_LibAppendTripleDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
        }
        
    }
    else if ((uiInLen + (WE_UINT32)iIndex) < *puiOutLen) 
    {
        *puiOutLen -= 8 ; 
        if((uiInLen + (WE_UINT32)iIndex) > 8)
        {
            pstPara->iBufLen = (iIndex + uiInLen) % 8;
            (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
            iResult=We_LibAppendTripleDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
            if((uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)))
            {
                iResult=We_LibAppendTripleDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                    (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
            }
            iIndex = uiInLen - pstPara->iBufLen;
            (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
        }
        else
        {
            (WE_VOID)WE_MEMCPY(pstPara->aucData8 + iIndex, pucIn, uiInLen);
            pstPara->iBufLen = uiInLen;
        }
    }
    else
    {
        pstPara->iBufLen = (iIndex + uiInLen) - *puiOutLen;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendTripleDes( pstPara->aucData8 ,8, pucOut, handle);
        if((uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)))
        {
            iResult=We_LibAppendTripleDes((WE_VOID*)(pucIn + (8 - iIndex)), \
                (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut + 8, handle);
        }
        iIndex = uiInLen - pstPara->iBufLen;
        (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
    }
    return iResult;
}
/*=====================================================================================
FUNCTION: 
    We_Lib3DesEnCryptFinal1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get encrypted data and free memory.
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
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_Lib3DesEnCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    St_WeBlockMAtchS *pstPara=NULL;
    WE_UCHAR aucData[8] = {0};
    
    if((NULL==handle)||(NULL==pucOut)||(0==puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 
    pstPara=(St_WeBlockMAtchS *)handle;
    if(*puiOutLen < 8)
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    *puiOutLen = 8;
    (WE_VOID)WE_MEMSET(aucData, (WE_UCHAR)(8-pstPara->iBufLen), (WE_UINT32)8);
    (WE_VOID)WE_MEMCPY(aucData, pstPara->aucData8, (WE_UINT32)pstPara->iBufLen);    
    iResult=We_LibAppendTripleDes( aucData ,8, pucOut, handle);
    
    We_LibKillDes(handle);   
    return iResult;
}
/***************************************************************************************************
*   Tripartite DES Decrypt                                                                    
***************************************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_Lib3DesDecrypt
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set sub decrypt key and decrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pucIn[IN]: data to be decrypted;
    uiInLen[IN]: length of data to be decrypted;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
/* No Padding */
WE_INT32 We_Lib3DesDecrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL;  
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);

    stMd.eOp = BSAPI_DECRYPT;
    iResult= We_LibStartTripleDes(&stMd,stKey,&handle);  

    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    
    if(!(uiInLen % 8) && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    iResult=We_LibAppendTripleDes((WE_UCHAR*)pucIn, uiInLen,pucOut,handle); 

    We_LibKillDes(handle);
    return iResult;
    
}
/*=====================================================================================
FUNCTION: 
    We_Lib3DesDecryptInit
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt init with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pHandle[OUT]: data handle;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_Lib3DesDecryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    if(NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_DECRYPT;
    iResult= We_LibStartTripleDes(&stMd,stKey,pHandle);

    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_Lib3DesDeCryptUpdate
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be decrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_Lib3DesDeCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{ 
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    WE_UINT32 uiLen = 0; 
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen)||!pucOut||!puiOutLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    uiLen = uiInLen % 8;
    if(!uiLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;        
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }    
    iResult=We_LibAppendTripleDes((WE_UCHAR *)pucIn, uiInLen,pucOut, handle);  
    
    return iResult; 
    
}


/*=====================================================================================
FUNCTION: 
    We_Lib3DesDeCryptFinal
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Free the data handle.
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
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_Lib3DesDeCryptFinal(WE_HANDLE handle)
{
    WE_INT32 iResult=0;    
    if(NULL==handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    We_LibKillDes(handle);
    
    return iResult;
}

/*=====================================================================================
FUNCTION: 
    We_Lib3DesDecrypt1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set sub decrypt key and decrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pucIn[IN]: data to be decrypted;
    uiInLen[IN]: length of data to be decrypted;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
/* RFC2630 Padding */
WE_INT32 We_Lib3DesDecrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL; 
    WE_INT32 iResult=0;
    WE_UINT32 uiLen=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);

    if(!(uiInLen % 8) && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }    
    stMd.eOp = BSAPI_DECRYPT;
    iResult = We_LibStartTripleDes(&stMd,stKey,&handle);  

    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iResult=We_LibAppendTripleDes((WE_UCHAR *)pucIn, uiInLen,pucOut, handle);    
    
    uiLen = *(pucOut + uiInLen - 1);
    if(uiLen&&(uiLen <= 8))
    {
        WE_UINT32   uiLoop = 0;
        for(; uiLoop < uiLen; uiLoop++)
        {
            if(uiLen != *(pucOut + uiInLen - 1 - uiLoop))
            {
                break;
            }
        }
        if(uiLoop == uiLen)
        {
            *puiOutLen -= uiLen;
        }
    }  
    We_LibKillDes(handle);
    return M_WE_LIB_ERR_OK;
    
}


/*=====================================================================================
FUNCTION: 
    We_Lib3DesDeCryptInit1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt init with 3DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    handle[OUT]: data handle;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_Lib3DesDeCryptInit1
( 
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    if(NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_DECRYPT;
    /* get subkey */
    iResult= We_LibStartTripleDes(&stMd,stKey,pHandle);  

    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_Lib3DesDeCryptUpdate1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt data with 3DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be decrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_Lib3DesDeCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    St_WeBlockMAtchS *pstPara=NULL;
    WE_UINT32 iIndex=0;
    WE_INT32 iBufLen=0;
    
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen)||!pucOut||!puiOutLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }  
    
    iBufLen = (uiInLen % 8);
    if(iBufLen && (*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else if(!iBufLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    pstPara=(St_WeBlockMAtchS *)handle;
    
    iIndex = pstPara->iBufLen;
    if((uiInLen + (WE_UINT32)iIndex) == *puiOutLen)
    {
        pstPara->iBufLen = 0;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendTripleDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
        if(8 != *puiOutLen)
        {
            iResult=We_LibAppendTripleDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
        }
        
    }
    else if ((uiInLen + (WE_UINT32)iIndex) < *puiOutLen) 
    {
        *puiOutLen -= 8 ; 
        if((uiInLen + (WE_UINT32)iIndex) > 8)
        {
            pstPara->iBufLen = (iIndex + uiInLen) % 8;
            (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
            iResult=We_LibAppendTripleDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
            if((uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)))
            {
                iResult=We_LibAppendTripleDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                    (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
            }
            iIndex = uiInLen - pstPara->iBufLen;
            (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
        }
        else
        {
            (WE_VOID)WE_MEMCPY(pstPara->aucData8 + iIndex, pucIn, uiInLen);
            pstPara->iBufLen = uiInLen;
        }
    }
    else
    {
        pstPara->iBufLen = (iIndex + uiInLen) - *puiOutLen;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendTripleDes( pstPara->aucData8 ,8, pucOut, handle);
        if((uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)))
        {
            iResult=We_LibAppendTripleDes((WE_VOID*)(pucIn + (8 - iIndex)), \
                (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut + 8, handle);
        }
        iIndex = uiInLen - pstPara->iBufLen;
        (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
    }
    return iResult;
    
}


/*=====================================================================================
FUNCTION: 
    We_Lib3DesDeCryptFinal1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Stripped the padding and free memory.
ARGUMENTS PASSED:
    pucOut[IN]: pointer to the end of decrypted data;
    puiOutLen[OUT]: length of padding;
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
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_Lib3DesDeCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
) 
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    WE_UINT32  uiLen = 0; 
    St_WeBlockMAtchS *pstPara=NULL;
    WE_UINT32 uiLoop=0;
    
    if((NULL==handle)||(NULL==pucOut)||(0==puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 
    pstPara=(St_WeBlockMAtchS *)handle;
    if(*puiOutLen < 8)
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    *puiOutLen = 8;  
    iResult=We_LibAppendTripleDes( pstPara->aucData8 ,8, pucOut, handle);
    
    uiLen = *(pucOut + 8 - 1);
    if(uiLen&&(uiLen <= 8))
    {
        for(; uiLoop < uiLen; uiLoop++)
        {
            if(uiLen != *(pucOut + 8 - 1 - uiLoop))
            {
                break;
            }
        }
        if(uiLoop == uiLen)
        {
            *puiOutLen -= uiLen;
        }
    }
    
    We_LibKillDes(handle);   
    return iResult;  
}


































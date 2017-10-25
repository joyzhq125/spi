/*==================================================================================================
    HEADER NAME : we_rsalib.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be update later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-11-27   tang            None         Draft
==================================================================================================*/
/***************************************************************************************
    *   Include File Section
 ****************************************************************************************/
#include "sec_comm.h"
/*#include "we_rsacomm.h"*/

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef enum tagE_MsgDataHash
{
    E_WE_RSA_MSGDATA_NOT_HASH,
    E_WE_RSA_MSGDATA_HASHED
}E_MsgDataHash;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 Sec_GetComputeRSASigType(E_WeRsaSigType eType,St_SigType *stSigType);
static WE_INT32 SEClib_RSAComputeSignatureBasic(WE_HANDLE hWeHandle,E_MsgDataHash eAlreadyHashed,
                                                WE_UINT8 *pucPrivKey, WE_INT32 iPrivKeyLen, 
                                                E_WeRsaSigType eType,
                                                const WE_UINT8 *pucBuf, WE_INT32 iBufLen, 
                                                WE_UINT8 *pucSign, WE_INT32 *puiSignLen);
static WE_INT32 SEClib_RsaDecryptionBasic(WE_HANDLE hWeHandle,St_SecCrptPubKeyRsa stPubKey, 
                                          const WE_UINT8 *pucData, WE_INT32 iDataLen, 
                                          WE_UINT8 *pucBuf, WE_INT32 *piBufLen);
static WE_INT32 SEClib_RecoverRSAPrivKey(const WE_UCHAR* pucKey, WE_INT32 iKeyLen, 
                                         St_SecRsaPrivateKey* pstKey);
static WE_INT32 SEClib_ConvertRSAKey(St_SecCrptPubKeyRsa stPubKey, St_UtMblk *pstRSAKey);

/*==================================================================================================
FUNCTION: 
    SEClib_ConvertRSAKey
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    convert rsa key
ARGUMENTS PASSED:
    St_SecCrptPubKeyRsa         stPubKey[IN]:public key
    St_UtMblk*                  pstRSAKey[OUT]:DER ENCODED BIT STRING
RETURN VALUE:
ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SEClib_ConvertRSAKey(St_SecCrptPubKeyRsa stPubKey, St_UtMblk *pstRSAKey)
{
    WE_UINT16  usMsgLen  = 0;
    WE_UINT16  usHdrLen = 0;
    WE_UINT16  usModLen = 0;
    WE_UINT16  usExpLen = 0;
    WE_UINT8   ucPreModZero = 0;
    WE_UINT8   ucPreExpZero = 0;
    WE_UINT16  usKeyModLen = stPubKey.usModLen;
    WE_UINT16  usKeyExpLen = stPubKey.usExpLen;
    WE_UINT8   *pucP=NULL;
    WE_INT32   iLoop=0;
    WE_UINT8   ucBigEndian = 0;
    WE_UINT16  usTemp = 0x0102;

    if(pstRSAKey==NULL)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    
    if (0x01 == *((WE_UINT8*)&(usTemp))) /* Check how bytes are positioned on the current platform */
    {
        ucBigEndian = 1;
    }
    else
    {
        ucBigEndian = 0;
    }
    /* Calculate length of DER representation of the public key */
    /* usModLen is the length of the modulus (integer) header */
    /* usExpLen is the length of the exponent (integer) header */
    /* usHdrLen is the length of the SEQUENCE (encapsulating) header */
    if (((WE_UINT8)*(stPubKey.pucModulus)) > 127)
    {
        ucPreModZero = 1;
        usKeyModLen++;
    }
    usModLen = 2;
    if (usKeyModLen > 127)
    {
        usModLen++;
    }
    if (usKeyModLen > 255)
    {
        usModLen++;
    }
    if (((WE_UINT8)*(stPubKey.pucExponent)) > 127)
    {
        ucPreExpZero = 1;
        usKeyExpLen++;
    }
    usExpLen = 2;
    if (usKeyExpLen > 127)
    {
        usExpLen++;
    }
    if (usKeyExpLen > 255)
    {
        usExpLen++;
    }
    usMsgLen = (WE_UINT16)(usModLen + usKeyModLen + usExpLen + usKeyExpLen);
    
    usHdrLen = 2;
    if (usMsgLen > 127)
    {
        usHdrLen++;
    }
    if (usMsgLen > 255)
    {
        usHdrLen++;
    }
    pstRSAKey->pucData=(WE_UINT8 *)WE_MALLOC(usHdrLen + usMsgLen);
    pstRSAKey->uiSize=(usHdrLen + usMsgLen);
    pstRSAKey->uiUsed=usHdrLen + usMsgLen;

    if (!((*pstRSAKey).pucData))
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMSET(pstRSAKey->pucData,0,(usHdrLen + usMsgLen));
    /* DER encode key (ASN.1) */
    pucP = (*pstRSAKey).pucData;
    *pucP++ = '\x30'; /* SEQUENCE */
    switch (usHdrLen)
    {
    case 2:
        if (ucBigEndian)
        {
            *pucP++ = *(((WE_UINT8*)&(usMsgLen))+1); /* write message length */
        }
        else
        {
            *pucP++ = *((WE_UINT8*)&(usMsgLen)); /* write message length (1 byte) */
        }
        break;
    case 3: 
        *pucP++ = 0x81; /* LENGTH SPECIFIER - 1 Byte */
        if (ucBigEndian)
        {
            *pucP++ = *(((WE_UINT8*)&(usMsgLen))+1); /* write message length */
        }
        else
        {
            *pucP++ = *((WE_UINT8*)&(usMsgLen)); /* write message length */
        }
        break;
    case 4:
        *pucP++ = 0x82; /* LENGTH SPECIFIER - 2 Bytes */
        if (ucBigEndian)
        {
            *pucP++ = *((WE_UINT8*)&(usMsgLen));
            *pucP++ = *(((WE_UINT8*)&(usMsgLen))+1); /* write message length */
        }
        else
        {
            *pucP++ = *(((WE_UINT8*)&(usMsgLen))+1); /* write message length */
            *pucP++ = *((WE_UINT8*)&(usMsgLen));
        }
        break;
     default:
        break;
    } /* This provides for message lengths up to 65535 bits */
    
    *pucP++ = 0x02; /* INTEGER */
    switch (usModLen)
    {
    case 2:
        if (ucBigEndian)
        {
            *pucP++ = *(((WE_UINT8*)&(usKeyModLen))+1); /* write modulus length (1 byte) */
        }
        else
        {
            *pucP++ = *((WE_UINT8*)&(usKeyModLen));
        }
        break;
    case 3:
        *pucP++ = 0x81; /* LENGTH SPECIFIER */
        if (ucBigEndian)
        {
            *pucP++ = *(((WE_UINT8*)&(usKeyModLen))+1); /* write modulus length (1 byte) */
        }
        else
        {
            *pucP++ = *((WE_UINT8*)&(usKeyModLen));
        }
        break;
    case 4:
        *pucP++ = 0x82; /* LENGTH SPECIFIER */
        if (ucBigEndian)
        {
            *pucP++ = *((WE_UINT8*)&(usKeyModLen));
            *pucP++ = *(((WE_UINT8*)&(usKeyModLen))+1); /* write modulus length (2 bytes) */
        }
        else
        {
            *pucP++ = *(((WE_UINT8*)&(usKeyModLen))+1); /* write modulus length (2 bytes) */
            *pucP++ = *((WE_UINT8*)&(usKeyModLen));
        }
        break;
    default:
        break;
    }
    if (ucPreModZero)
    {
        *pucP++ = '\x00';
    }
    for (iLoop=0; iLoop<stPubKey.usModLen; iLoop++) /* write modulus (original length  only) */
    {
        *pucP++ = stPubKey.pucModulus[iLoop];
    }
    *pucP++ = '\x02'; /* INTEGER */
    switch (usExpLen)
    {
    case 2:
        if (ucBigEndian)
        {
            *pucP++ = *(((WE_UINT8*)&(usKeyExpLen))+1); /* write exponent length (1 byte) */
        }
        else
        {
            *pucP++ = *((WE_UINT8*)&(usKeyExpLen));
        }
        break;
    case 3:
        *pucP++ = 0x81; /* LENGTH SPECIFIER */
        if (ucBigEndian)
        {
            *pucP++ = *(((WE_UINT8*)&(usKeyExpLen))+1); /* write exponent length (1 byte) */
        }
        else
        {
            *pucP++ = *((WE_UINT8*)&(usKeyExpLen));
        }
        break;
    case 4:
        *pucP++ = 0x82; /* LENGTH SPECIFIER */
        if (ucBigEndian)
        {
            *pucP++ = *((WE_UINT8*)&(usKeyExpLen));
            *pucP++ = *(((WE_UINT8*)&(usKeyExpLen))+1); /* write exponent length (2 bytes) */
        }
        else
        {
            *pucP++ = *(((WE_UINT8*)&(usKeyExpLen))+1); /* write exponent length (2 bytes) */
            *pucP++ = *((WE_UINT8*)&(usKeyExpLen));
        }
        break;
    default:
        break;
    }
    if (ucPreExpZero)
    {
        *pucP++ = '\x00';
    }
    for (iLoop=0; iLoop<stPubKey.usExpLen; iLoop++) /* write exponent (original length  only) */
    {
        *pucP++ = stPubKey.pucExponent[iLoop];
    }
    (*pstRSAKey).uiUsed = usHdrLen + usMsgLen;
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    SEClib_RsaPublicKeyEncryption
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    use a rsa public key to encrypt data
ARGUMENTS PASSED:
    St_SecCrptPubKeyRsa         stPubKey[IN]:public key
    WE_UINT8*                   pucData[IN]:the data need to been crypted
    WE_INT32	                iDataLen[IN]:length of pucdata
    WE_UINT8 *                  pucBuf[IN/OUT]:the encrypted data
    WE_INT32 *                  piBufLen[IN/OUT]:length of pucbuf
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SEClib_RsaPublicKeyEncryption(WE_HANDLE hWeHandle,St_SecCrptPubKeyRsa stPubKey, 
                                       const WE_UINT8 *pucData, WE_INT32 iDataLen, 
                                       WE_UINT8 *pucOut, WE_INT32 *piOutLen)
{   
    WE_HANDLE           handle=NULL;
    st_RsaMode          stMode = {E_WE_RSA_ENCRYPT,E_WE_RSA_MODE_CBC}; /* only first field matters */
    St_RsaKeySet        stKeyRefParam = {0};
    P_St_UtMblkLists    pstCypDataHandle = NULL;
    St_UtMblk           stRSAKey = {0};
    WE_INT32            iRes=0;
    
    if(!piOutLen||!pucOut||!pucData||!iDataLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }  
    if (*piOutLen == 0) /* Supply adequate buffer length if necessary */
    {
        /* 
        Approximate required buffer size We suggest that dataLen should be rounded up to the nearest
        multiple of the key length */
        *piOutLen = iDataLen / stPubKey.usModLen;
        if ((*piOutLen) * stPubKey.usModLen != iDataLen)
        {
            *piOutLen += 1;
        }
        *piOutLen *= stPubKey.usModLen;
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    iRes = SEClib_ConvertRSAKey(stPubKey, &stRSAKey); /* Convert RSA key to Baltimore format (DER) */
    if (iRes != M_WE_LIB_ERR_OK)
    {
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return iRes;
    }
    
    stKeyRefParam.pstKey = &stRSAKey;
    stKeyRefParam.stPadAlg.ePadMethod=E_RSA_PAD_PKCS1; 
    stKeyRefParam.stPadAlg.iNullByte=1;
    
    iRes=We_RsaStart( hWeHandle,&stMode, &stKeyRefParam,&handle);
    if(iRes!=M_WE_LIB_ERR_OK)
    {
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    
    iRes=We_RsaAppend( hWeHandle,(WE_UINT8 *)pucData, (WE_UINT32)iDataLen, handle);
    if(iRes!=M_WE_LIB_ERR_OK) 
    {
        We_RsaRelease(handle);
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iRes=We_RsaFinish(hWeHandle,handle,&pstCypDataHandle);    
    if(iRes!=M_WE_LIB_ERR_OK) 
    {
        We_RsaDisposeMblkList(pstCypDataHandle);
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_INVALID_KEY;
    }
    
    if (pstCypDataHandle->stBlock.uiUsed > (WE_UINT32)(*piOutLen)) /* If too little buffer space... */
    {
        *piOutLen =(WE_INT32)(pstCypDataHandle->stBlock.uiUsed);
        We_RsaDisposeMblkList(pstCypDataHandle);
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL; /* Return exact required buffer size */
    }    
    /* Copy encrypted data to buffer */
    (WE_VOID)WE_MEMCPY(pucOut,pstCypDataHandle->stBlock.pucData,pstCypDataHandle->stBlock.uiUsed);
    *piOutLen = (WE_INT32)(pstCypDataHandle->stBlock.uiUsed);
    
    We_RsaDisposeMblkList(pstCypDataHandle); 
    if(stRSAKey.pucData)
    {
        WE_FREE(stRSAKey.pucData);
        stRSAKey.pucData=NULL;
    }
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    SEClib_RsaPrivateKeyDecryption
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
     use a rsa private key struct to decrypt data
ARGUMENTS PASSED:
    St_SecCrptPrivKey           stKey[IN]:rsa private key
    WE_UCHAR*                   pucData[IN]:the data to be decryption
    WE_INT32	                iDataLen[IN]: length of pucdata
    WE_UINT8 *                  pucBuf[OUT]: the decrypted data
    WE_INT32 *                  piBufLen[OUT]:length of pucbuf
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SEClib_RsaPrivateKeyDecryption(WE_HANDLE hWeHandle,St_SecCrptPrivKey stKey,
                                        const WE_UCHAR *pucData, 
                                        WE_INT32 iDataLen,WE_UINT8 *pucBuf, 
                                        WE_INT32 *piBufLen)
{
    WE_INT32                iResult=0;
    St_SecRsaPrivateKey     pstKey={0};
    St_SecCrptPubKeyRsa     stPrikey={0};
    
    if(!pucData||!pucBuf||!piBufLen)
    {
       return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    iResult=SEClib_RecoverRSAPrivKey(stKey.pucBuf, stKey.usBufLen, 
        & pstKey);
    
    stPrikey.usModLen=pstKey.usModLen;
    stPrikey.pucModulus=pstKey.aucModulus;
    stPrikey.usExpLen =pstKey.usExpLen;    
    stPrikey.pucExponent =pstKey.aucExponent;    

    iResult=SEClib_RsaDecryptionBasic(hWeHandle,stPrikey, pucData, iDataLen, pucBuf, piBufLen);
    return iResult;
}
/*==================================================================================================
FUNCTION: 
    SEClib_RsaDecryptionBasic
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    use a rsa public key to encrypt data
ARGUMENTS PASSED:
    St_SecCrptPubKeyRsa         stPubKey[IN]:public key
    WE_UINT8*                   pucData[IN]:the data need to be encrypted
    WE_INT32	                iDataLen[IN]:length of pucdata
    WE_UINT8 *                  pucBuf[OUT]:the encrypted data
    WE_INT32 *                  piBufLen[IN/OUT]:length of pucbuf
RETURN VALUE:
ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SEClib_RsaDecryptionBasic(WE_HANDLE hWeHandle,St_SecCrptPubKeyRsa stPubKey, 
                                          const WE_UINT8 *pucData, WE_INT32 iDataLen, 
                                          WE_UINT8 *pucBuf, WE_INT32 *piBufLen)
{ 
    WE_HANDLE        hMachHandle=NULL;
    st_RsaMode     stMode = {E_WE_RSA_DECRYPT,E_WE_RSA_MODE_CBC}; /* only first field matters */
    St_RsaKeySet     stKeyRefParam = {0};
    P_St_UtMblkLists pstCypDataHandle=NULL;
    St_UtMblk        stRSAKey = {0};
    WE_INT32         iRes=0;
    if(!pucBuf||!piBufLen||!pucData||!iDataLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    

    if (*piBufLen == 0) /* Supply adequate buffer length if necessary */
    {
    /* Approximate required buffer size We suggest that dataLen should be  rounded up to the nearest 
      multiple of the key length */
        *piBufLen = iDataLen / stPubKey.usModLen;
        if ((*piBufLen) * stPubKey.usModLen != iDataLen)
        {
            *piBufLen += 1;
        }
        *piBufLen *= stPubKey.usModLen;
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    iRes = SEClib_ConvertRSAKey(stPubKey, &stRSAKey); /* Convert RSA key to Baltimore format (DER) */
    if (iRes != M_WE_LIB_ERR_OK)
    {
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return iRes;
    }
    
    stKeyRefParam.pstKey = &stRSAKey;
    stKeyRefParam.stPadAlg.ePadMethod=E_RSA_PAD_PKCS1;/*&pad_oaep_pkcs1;*/
    stKeyRefParam.stPadAlg.iNullByte=1;
         

    iRes=We_RsaStart( hWeHandle,&stMode,  &stKeyRefParam,&hMachHandle);
    
    if(M_WE_LIB_ERR_OK!=iRes) 
    {
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iRes=We_RsaAppend( hWeHandle,(WE_UINT8 *)pucData, (WE_UINT32)iDataLen, hMachHandle);
    
    if(M_WE_LIB_ERR_OK!=iRes) 
    {
        We_RsaRelease(hMachHandle);
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iRes=We_RsaFinish(hWeHandle,hMachHandle,&pstCypDataHandle);
    
    if(M_WE_LIB_ERR_OK!=iRes) 
    {
        We_RsaDisposeMblkList(pstCypDataHandle);
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_INVALID_KEY;
    }
    
    if (pstCypDataHandle->stBlock.uiUsed > (WE_UINT32)(*piBufLen)) /* If too little buffer space... */
    {
        *piBufLen = (WE_INT32)(pstCypDataHandle->stBlock.uiUsed);
        We_RsaDisposeMblkList(pstCypDataHandle);
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL; /* Return exact required buffer size */
    }
   
    (WE_VOID)WE_MEMCPY(pucBuf,pstCypDataHandle->stBlock.pucData,pstCypDataHandle->stBlock.uiUsed);
    *piBufLen =(WE_INT32)(pstCypDataHandle->stBlock.uiUsed);
    
    We_RsaDisposeMblkList(pstCypDataHandle); 
    if(stRSAKey.pucData)
    {
        WE_FREE(stRSAKey.pucData);
        stRSAKey.pucData=NULL;
    }
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    SEClib_computeRSADigitalSignature
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    signature data
ARGUMENTS PASSED:
    St_SecCrptPrivKey  stKey[IN]:rsa private key
    E_WeRsaSigType    eType[IN]:signature type
    WE_UINT8 *pucBuf[IN]:the data need to be signature
    WE_INT32 iBufLen[IN]:length of pucbuf
    WE_UINT8 *pucSign[OUT]:signature data
    WE_INT32 *piSignLen[OUT]:length of pucsign
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SEClib_computeRSADigitalSignature(WE_HANDLE hWeHandle,St_SecCrptPrivKey stKey, 
                                           E_WeRsaSigType eType, const WE_UINT8 *pucBuf, 
                                           WE_INT32 iBufLen, WE_UINT8 *pucSign, WE_INT32 *piSignLen)
{
    WE_INT32            iDigLen=0;
    WE_INT32            iResult=0; 
    E_WeHashAlgType     eHashType=E_WE_ALG_HASH_NULL;
    WE_UINT8            aucDigest[20]={0};

    if(!pucBuf||!pucSign||!piSignLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
 
    iResult=Sec_GetHashType(eType,&eHashType,&iDigLen);
    if(iResult)
    {
        return iResult;
    }
    /*iResult=We_LibHash(eHashType,pucBuf,iBufLen,aucDigest,&iDigLen);*/
    iResult= We_LibHash( hWeHandle,eHashType,  pucBuf, 
                        iBufLen, 
                        aucDigest, 
                        &iDigLen);
    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return iResult;
    }
    iResult= SEClib_RSAComputeSignatureBasic(hWeHandle,E_WE_RSA_MSGDATA_NOT_HASH, stKey.pucBuf, stKey.usBufLen, eType, 
        aucDigest, iDigLen, pucSign, piSignLen);

    return iResult;
}
/*==================================================================================================
FUNCTION: 
    SEClib_ComputeRSAHashedSignature
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    signature data which has been hashed
ARGUMENTS PASSED:
    St_SecCrptPrivKey  stKey[IN]:rsa private key
    WE_UCHAR *pucData[IN]:the data need to be signature
    WE_INT32 iDataLen[IN]:length of pucdata
    WE_UINT8 *pucSign[OUT]:signature data
    WE_INT32 *piSignLen[OUT]:length of pucsign
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SEClib_ComputeRSAHashedSignature(WE_HANDLE hWeHandle,St_SecCrptPrivKey stKey,const WE_UCHAR *pucData, 
                                    WE_INT32 iDataLen,WE_UINT8 *pucSign, WE_INT32 *piSignLen)
{
    return SEClib_RSAComputeSignatureBasic(hWeHandle,E_WE_RSA_MSGDATA_HASHED, stKey.pucBuf, stKey.usBufLen,E_WE_RSA_PKCS1_NULL, 
        pucData, iDataLen, pucSign, piSignLen);
}

/*==================================================================================================
FUNCTION: 
    SEClib_RSAComputeSignatureBasic
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    signature data
ARGUMENTS PASSED:
    E_MsgDataHash eAlreadyHashed[IN]:the type of data :hashed or not hashed
    WE_UINT8 *pucPrivKey[IN]:private key
    WE_INT32 iPrivKeyLen[IN]:length of private key
    E_WeRsaSigType eType[INT:signature type
    const WE_UINT8 *pucBuf[IN]:the data need to be signature
    WE_INT32 iBufLen[IN]:length of pucbuf
    WE_UINT8 *pucSign[OUT]:signature data
    WE_INT32 *piSignLen[OUT]:length of pucsign
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SEClib_RSAComputeSignatureBasic(WE_HANDLE hWeHandle,E_MsgDataHash eAlreadyHashed,
                                         WE_UINT8 *pucPrivKey, WE_INT32 iPrivKeyLen, 
                                         E_WeRsaSigType eType,
                                         const WE_UINT8 *pucBuf, WE_INT32 iBufLen, 
                                         WE_UINT8 *pucSign, WE_INT32 *piSignLen)
{
    st_RsaMode stMode = {E_WE_RSA_SIGN,E_WE_RSA_MODE_CBC}; /* only first field matter */
    St_RsaKeySet      stKeyRefParam = {0};
    P_St_UtMblkLists  pstCypDataHandle=NULL;   
    St_UtMblk         stRSAKey={0};
    WE_HANDLE         hMachHandle=NULL;
    WE_INT32          iRes=0;

    if(!pucPrivKey||!pucBuf||!pucSign||!piSignLen)
    {
       return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stRSAKey.pucData=(WE_UINT8 *)WE_MALLOC((WE_UINT32)iPrivKeyLen);
    if(!stRSAKey.pucData)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }   
    (WE_VOID)WE_MEMSET(stRSAKey.pucData,0,(WE_UINT32)iPrivKeyLen);
    (WE_VOID)WE_MEMCPY(stRSAKey.pucData,pucPrivKey,(WE_UINT32)iPrivKeyLen);
    stRSAKey.uiUsed=(WE_UINT32)iPrivKeyLen;
    stRSAKey.uiSize=(WE_UINT32)iPrivKeyLen;
    
    stKeyRefParam.pstKey = &stRSAKey;
   // stKeyRefParam.pstPadAlg = &stPadPkcs1 ;
    stKeyRefParam.stPadAlg.ePadMethod=E_RSA_PAD_PKCS1;/*&pad_oaep_pkcs1;*/
    stKeyRefParam.stPadAlg.iNullByte=1;

    if (eAlreadyHashed == E_WE_RSA_MSGDATA_NOT_HASH)
    {        
        iRes=Sec_GetComputeRSASigType(eType,&(stKeyRefParam.stSigType));
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            WE_FREE(stRSAKey.pucData);
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
    }
    else
    {
        stKeyRefParam.eDataType = E_DATA_TYPE_HASH;
    }
    iRes=We_RsaStart( hWeHandle,&stMode, &stKeyRefParam,&hMachHandle);
    
    if(M_WE_LIB_ERR_OK!=iRes)
    {
        WE_FREE(stRSAKey.pucData);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iRes=We_RsaAppend(hWeHandle,(WE_UINT8 *)pucBuf, (WE_UINT32)iBufLen, hMachHandle);
    if(M_WE_LIB_ERR_OK!=iRes)
    {
        WE_FREE(stRSAKey.pucData);
        We_RsaRelease(hMachHandle);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iRes=We_RsaFinish(hWeHandle,hMachHandle,&pstCypDataHandle);
    if(M_WE_LIB_ERR_OK!=iRes)
    {
        WE_FREE(stRSAKey.pucData);
        return M_WE_LIB_ERR_INVALID_KEY;
    }

    /* Copy encrypted data to sig */
    (WE_VOID)WE_MEMCPY(pucSign,pstCypDataHandle->stBlock.pucData,pstCypDataHandle->stBlock.uiUsed);
    *piSignLen = (WE_INT32)(pstCypDataHandle->stBlock.uiUsed);
    
    We_RsaDisposeMblkList(pstCypDataHandle);
    if(stRSAKey.pucData)
    {
        WE_FREE(stRSAKey.pucData);
        stRSAKey.pucData=NULL;
    }
    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    SEClib_VerifyRSASignature
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    verify rsa signature
ARGUMENTS PASSED:
    St_SecCrptPubKeyRsa stPubKey[IN]:rsa public key
    WE_UINT8 *pucMsg[IN]:Msg need to be verified
    WE_INT32 iMsgLen[IN]:length of pucMsg
    E_WeRsaSigType eType[IN]:signature type
    WE_UINT8 *pucSign[IN]:signature need to be verified
    WE_INT32 iSignLen[IN]:length of pucsign
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SEClib_VerifyRSASignature(WE_HANDLE hWeHandle,St_SecCrptPubKeyRsa stPubKey, 
                                   WE_UINT8 *pucMsg, WE_INT32 iMsgLen, 
                                   E_WeRsaSigType eType, const	WE_UINT8 *pucSign, 
                                   WE_INT32 iSignLen)
{
    st_RsaMode	        stMode   = {E_WE_RSA_VERIFY,E_WE_RSA_MODE_CBC}; 
    St_RsaKeySet        stKeyRefParam = {0};
    P_St_UtMblkLists    pstCypDataHandle=NULL;
    St_UtMblk           stRSAKey={0};
    St_UtMblk           stSign={0};
    WE_HANDLE           handle=NULL;
    WE_INT32            iRes=0;
    
    if(!pucMsg||!pucSign||!iSignLen||!iMsgLen)
    {
       return M_WE_LIB_ERR_INVALID_PARAMETER;
    }  

    pstCypDataHandle = WE_MALLOC(sizeof(*pstCypDataHandle)); /* Allocate pstCypDataHandle */
    if (pstCypDataHandle == NULL)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMSET(pstCypDataHandle,0,sizeof(*pstCypDataHandle));
    pstCypDataHandle->pstNext = 0;
    pstCypDataHandle->pstPrev = pstCypDataHandle;
    
    iRes = SEClib_ConvertRSAKey(stPubKey, &stRSAKey); /* Convert Public RSA key */
    if (iRes != M_WE_LIB_ERR_OK)
    {
        We_RsaDisposeMblkList(pstCypDataHandle);
        return iRes;
    }
    
    stSign.pucData=(WE_UINT8 *)WE_MALLOC((WE_UINT32)iSignLen);
    if(stSign.pucData==NULL)
    {
        WE_FREE(pstCypDataHandle);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMSET(stSign.pucData,0,(WE_UINT32)iSignLen);
    (WE_VOID)WE_MEMCPY(stSign.pucData,pucSign,(WE_UINT32)iSignLen);
    stSign.uiSize = (WE_UINT32)iSignLen;
    stSign.uiUsed = (WE_UINT32)iSignLen;
    pstCypDataHandle->stBlock = stSign;
    
    stKeyRefParam.pstKey = &stRSAKey;
   // stKeyRefParam.pstPadAlg  = &stPadPkcs1;
    stKeyRefParam.stPadAlg.ePadMethod=E_RSA_PAD_PKCS1;/*&pad_oaep_pkcs1;*/
    stKeyRefParam.stPadAlg.iNullByte=1;

    if ((WE_INT32)eType == (WE_INT32)E_WE_RSA_PKCS1_NULL)
    {
        stKeyRefParam.eDataType = E_DATA_TYPE_HASH;
    }
    else
    {
        iRes=Sec_GetComputeRSASigType(eType,&(stKeyRefParam.stSigType));
    }  
    if(iRes!=M_WE_LIB_ERR_OK)
    {
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        We_RsaDisposeMblkList(pstCypDataHandle);
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    
    iRes=We_RsaStart( hWeHandle,&stMode, &stKeyRefParam,&handle);
    if(M_WE_LIB_ERR_OK!=iRes)
    {
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        We_RsaDisposeMblkList(pstCypDataHandle);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iRes=We_RsaAppend( hWeHandle,(WE_UINT8 *)pucMsg,(WE_UINT32)iMsgLen, handle);
    if(M_WE_LIB_ERR_OK!=iRes)
    {
        if(stRSAKey.pucData)
        {
            WE_FREE(stRSAKey.pucData);
            stRSAKey.pucData=NULL;
        }
        We_RsaDisposeMblkList(pstCypDataHandle);
        We_RsaRelease(handle);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iRes=We_RsaFinish(hWeHandle,handle,&pstCypDataHandle);

    if(stRSAKey.pucData)
    {
        WE_FREE(stRSAKey.pucData);
        stRSAKey.pucData=NULL;
    }
    We_RsaDisposeMblkList(pstCypDataHandle);
    return iRes;
}
/*
FUNCTION: 
    SEClib_RecoverRSAPrivKey
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    recover asn der encode bit string to rsa private key struct 
ARGUMENTS PASSED:
    const WE_UCHAR* pucKey[IN]:rsa private key
    WE_INT32 iKeyLen[IN]:length of puckey
    St_SecRsaPrivateKey* pstPrivKey[OUT]:
RETURN VALUE:
    error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SEClib_RecoverRSAPrivKey(const WE_UCHAR* pucKey, WE_INT32 iKeyLen, 
                                  St_SecRsaPrivateKey* pstPrivKey)
{
    WE_UINT16           usV = 0;
    WE_UINT16           usM = 0;
    WE_UINT16           usE = 0;
    WE_UINT16           usD = 0;
    const WE_UCHAR*     pucP = pucKey;
    WE_INT32            iIndex = 0;  
    WE_INT32            iLoop = 0;

    if ((!pucKey) || !pstPrivKey || (!pucP))
    {
        return M_WE_LIB_ERR_NOT_IMPLEMENTED;
    }
    iKeyLen = iKeyLen;
    /*GET HEAD*/
    iIndex ++ ;/*0x30*/
    /*GET MSGLEN*/
    switch(pucP[iIndex])//
    {
        case 0x81:
        {
            iIndex ++ ;            
            break;
        }
        case 0x82:
        {
            iIndex ++ ;           
            iIndex ++ ;
            break;
        }

        default:
        	break;
    }
    /*GET VERSION*/
    iIndex ++ ;/*0x02*/
    {
        iIndex ++ ;/*version len*/
        switch(pucP[iIndex])
        {
            case 0x81:
            {
                iIndex ++ ;            
                usV = pucP[iIndex];
                break;
            }

            case 0x82:
            {
                iIndex ++ ;           
                usV = (pucP[iIndex])<<8;
                iIndex ++ ;
                usV += (pucP[iIndex]);            
                break;
            }

            default:
            {
                usV = pucP[iIndex] ;            
            }
        }
        iIndex += usV ;
    }

    /*GET MODULUS*/
    iIndex ++ ;/*0x2*/
    iIndex ++ ;
    switch (pucP[iIndex])/*modulus*/
    {
        case 0x81:
        {
            iIndex ++ ;            
            usM = pucP[iIndex];            
            break;
        }
        case 0x82:
        {
            iIndex ++ ;           
            usM = (pucP[iIndex]) << 8;
            iIndex ++ ;
            usM += pucP[iIndex] ;           
            break;
        }

        default:
        {
            usM = pucP[iIndex];            
            break;
        }
    }
    iIndex ++ ;
    if((0x00 == pucP[iIndex])&&(pucP[iIndex + 1] > 127))
    {
        usM -= 1;
        iIndex ++ ;
    }
    pstPrivKey->usModLen = usM;

    for (iLoop = 0; iLoop < usM; iLoop++)
    {
        pstPrivKey->aucModulus[iLoop] = pucP[iIndex];
        iIndex ++;
    }

    /*GET PUBLIC EXP*/
    iIndex ++ ;/*0x02*/
    switch (pucP[iIndex])
    {
        case 0x81:
        {
            iIndex ++ ;            
            usE = pucP[iIndex];            
            break;
        }
        case 0x82:
        {
            iIndex ++ ;           
            usE = (pucP[iIndex]) << 8;
            iIndex ++ ;
            usE += pucP[iIndex] ;           
            break;
        }

        default:
        {
            /*iIndex ++ ;*/
            usE = pucP[iIndex];            
            break;
        }
    }
    {
        iIndex += usE ;
    }

    /*GET PRIVATE EXP*/
    iIndex ++;/*0x02*/
    iIndex ++;
    switch (pucP[iIndex])/*d*/
    {
        case 0x81:
        {
            iIndex ++ ;            
            usD = pucP[iIndex];            
            break;
        }
        case 0x82:
        {
            iIndex ++ ;           
            usD = (pucP[iIndex]) << 8;
            iIndex ++ ;
            usD += pucP[iIndex] ;           
            break;
        }

        default:
        {
            usD = pucP[iIndex];            
            break;
        }
    }

    iIndex ++ ;
    if((0x00 == pucP[iIndex])&&(pucP[iIndex + 1] > 127))
    {
        usD -= 1;
        iIndex ++ ;
    }
    pstPrivKey->usExpLen = usD;

    for (iLoop = 0; iLoop < usD; iLoop++)
    {
        pstPrivKey->aucExponent[iLoop] = pucP[iIndex];
        iIndex ++;
    }

    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetComputeRSASigType
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get rsa signature type
ARGUMENTS PASSED:
    E_WeRsaSigType eType[IN]: rsa signature type
    St_SigType *stSigType[OUT]:rsa signature type
RETURN VALUE:
    error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 Sec_GetComputeRSASigType(E_WeRsaSigType eType,St_SigType *stSigType)
{
    if(NULL==stSigType)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    switch((WE_INT32)eType) 
    {
    case E_WE_RSA_PKCS1_MD2:
        stSigType->eHashType=E_HASH_BITS;
        stSigType->eSigType=E_WE_RSA_PKCS1_MD2;
        break;
    case E_WE_RSA_PKCS1_MD5:
        stSigType->eHashType=E_HASH_BITS;
        stSigType->eSigType=E_WE_RSA_PKCS1_MD5;
        break;
    case E_WE_RSA_PKCS1_SHA1:
        stSigType->eHashType=E_HASH_BITS;
        stSigType->eSigType=E_WE_RSA_PKCS1_SHA1;
        break;
    default:
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    return M_WE_LIB_ERR_OK;    
}
/*==================================================================================================
FUNCTION: 
    Sec_GetHashType
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get hash type
ARGUMENTS PASSED:
    E_WeRsaSigType eType[IN]: rsa sig type
    E_WeHashAlgType *peHashType[IN]:hash type
    WE_INT32 *piDigLen[OUT]:length of hash
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_GetHashType(E_WeRsaSigType eType,E_WeHashAlgType *peHashType,WE_INT32 *piDigLen)
{
    if((NULL==peHashType)||(NULL==piDigLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    switch(eType) 
    {
    case E_WE_RSA_PKCS1_NULL:
        *peHashType=E_WE_ALG_HASH_NULL;
        break;
    case E_WE_RSA_PKCS1_MD2:
        *peHashType=E_WE_ALG_HASH_MD2;
        *piDigLen=16;
        break;
    case E_WE_RSA_PKCS1_MD5:
        *peHashType=E_WE_ALG_HASH_MD5;
        *piDigLen=16;
        break;
    case E_WE_RSA_PKCS1_SHA1:
        *peHashType=E_WE_ALG_HASH_SHA1;
        *piDigLen=20;
        break;
    default:
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    return M_WE_LIB_ERR_OK;    
}


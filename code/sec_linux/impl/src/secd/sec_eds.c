/*==================================================================================================
    FILE NAME : Sec_eds.c
    MODULE NAME : Sec_SIC
    
    
    GENERAL DESCRIPTION
       Encode/Decode the message.
   
      
    Techfaith Software Confidential Proprietary
    (c) Copyright 2007 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History
        
    Modification                    Tracking
    Date            Author            Number       Description of changes
    ----------   --------------     ---------   ----------------------------
    2007-03-12      Stone an          None             draft
    
    Self-documenting Code
    
==================================================================================================*/ 

/***************************************************************************************************
Include File Section
***************************************************************************************************/
#include "sec_comm.h"
#include "sec_ecdr.h"
#include "sec_eds.h"

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static St_SecEcdrEncoder *Sec_SetMsgDataEncode( WE_UINT8 ucMsgId, WE_INT32 iDataLength );
static St_SecEcdrDecoder* Sec_GetMsgDecode(WE_VOID* pvBuf);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_GetMsgType
CREATE DATE:
    2007-03-15
AUTHOR:
    Stone An
DESCRIPTION:
    get the type of message.
ARGUMENTS PASSED:
    WE_VOID *pvBuf[IN]: Pointer to the encoded data .
RETURN VALUE:
    the type of message: success.
    0: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT8 Sec_GetMsgType(WE_VOID *pvBuf)
{
    WE_UINT8 ucType = 0;
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (!pvBuf)
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgType: pvBuf == NULL\n"));
        return 0;
    }

    pstDecoder = SecEcdr_CreateDecoder( (WE_UCHAR *)pvBuf, SEC_ED_MSG_TYPE_LENGTH );
    if (!pstDecoder)
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgType: pstDe == NULL\n"));
        return 0;
    }
    if (-1 == SecEcdr_DecodeUint8( pstDecoder, &ucType ))
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgType: pvBuf == NULL\n"));
        SecEcdr_DestroyDecoder( pstDecoder );
        return 0;
    }
    SecEcdr_DestroyDecoder( pstDecoder );

    return ucType;
}

#if 0
static St_SecEcdrDecoder *Sec_GetMsgDataDecode( WE_VOID *pvBuf )
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iDataLength = 0;

    WE_LOG_MSG((0,0,"Into Sec_GetMsgDataDecode\n"));

    if (!pvBuf)
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgDataDecode: pvBuf == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_GetMsgDataDecode\n"));
        return NULL;
    }

    pstDecoder = SecEcdr_CreateDecoder( (WE_UCHAR *)pvBuf, SEC_ED_MSG_HEADER_LENGTH );
    if (!pstDecoder)
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgDataDecode: pstDe == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_GetMsgDataDecode\n"));
        return NULL;
    }
      
    SecEcdr_ChangeDecodePosition( pstDecoder, SEC_ED_MSG_TYPE_LENGTH );
    SecEcdr_DecodeInt32( pstDecoder, &iDataLength );
    SecEcdr_DestroyDecoder( pstDecoder );

    pstDecoder = SecEcdr_CreateDecoder( (WE_UCHAR *)pvBuf + pstDecoder->iPosition, iDataLength );
    if (!pstDecoder)
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgDataDecode: En == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_GetMsgDataDecode\n"));
        return NULL;
    }

    WE_LOG_MSG((0,0,"Leave Sec_GetMsgDataDecode\n"));
    return pstDecoder;
}
#endif
/*==================================================================================================
FUNCTION: 
    Sec_SetMsgDataEncode
CREATE DATE:
    2007-03-15
AUTHOR:
    Stone An
DESCRIPTION:
    add message type to the encode data.
ARGUMENTS PASSED:
    WE_UINT8 ucMsgId[IN]: the type of message .
    WE_INT32 iDataLength[IN]:the length of data.
RETURN VALUE:
    Encoder data: success.
    NULL: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static St_SecEcdrEncoder *Sec_SetMsgDataEncode(WE_UINT8 ucMsgId, WE_INT32 iDataLength )
{
    St_SecEcdrEncoder *pstEncoder = NULL;

    pstEncoder = SecEcdr_CreateEncoder( SEC_ED_MSG_HEADER_LENGTH + iDataLength);
    if (!pstEncoder)
    {
        WE_LOG_MSG((0,0,"Sec_SetMsgDataEncode: En == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_SetMsgDataEncode\n"));
        return NULL;
    }

    if (-1 == SecEcdr_EncodeUint8( pstEncoder, &ucMsgId )
        || -1 == SecEcdr_EncodeInt32( pstEncoder, &iDataLength ))
    {
        WE_LOG_MSG((0,0,"Sec_SetMsgDataEncode: En == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_SetMsgDataEncode\n"));
        SecEcdr_DestroyEncoder( pstEncoder );
        return NULL;
    }

    return pstEncoder;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetMsgLen
CREATE DATE:
    2007-03-15
AUTHOR:
    Stone An
DESCRIPTION:
    get the length of encoded data.
ARGUMENTS PASSED:
    WE_VOID *pvBuf[IN]: pointer to the encoded data .
RETURN VALUE:
    the length of encoded data.: success.
    0: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_GetMsgLen(WE_VOID *pvBuf)
{
    WE_UINT8 ucType = 0;
    WE_INT32 iLen = 0;
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (!pvBuf)
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgLen: De == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_GetMsgLen\n"));
        return 0;
    }

    pstDecoder = SecEcdr_CreateDecoder( (WE_UCHAR *)pvBuf, SEC_ED_MSG_HEADER_LENGTH );
    if (!pstDecoder)
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgLen: De == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_GetMsgLen\n"));
        return 0;
    }
    if (-1 == SecEcdr_DecodeUint8( pstDecoder, &ucType ))
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgLen: De == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_GetMsgLen\n"));
        SecEcdr_DestroyDecoder( pstDecoder );
        return 0;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &iLen))
    {
        WE_LOG_MSG((0,0,"Sec_GetMsgLen: De == NULL\n"));
        WE_LOG_MSG((0,0,"Leave Sec_GetMsgLen\n"));
        SecEcdr_DestroyDecoder( pstDecoder );
        return 0;
    }
    SecEcdr_DestroyDecoder( pstDecoder );

    return iLen + SEC_ED_MSG_HEADER_LENGTH;
}


#ifdef M_SEC_CFG_CAN_SIGN_TEXT
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSignText
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    encode the result of signtext and return the result to the stk..
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to ISigntext.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iSignId[IN]:ID of the sign input from the invoker.
    WE_INT32 iAlgorithm[IN]:the value of the algorithm.
    WE_CHAR * pucSignature[IN]:Pointer to the digital sign nature.
    WE_INT32 iSigLen[IN]:The length of the sign
    WE_CHAR * pucHashedKey[IN]:Pointer to the hashed key.
    WE_INT32 iHashedKeyLen[IN]:Length of the hashed key.
    WE_CHAR * pucCertificate[IN]:Pointer to certificate.
    WE_INT32 iCertificateLen[IN]:The length of certificate.
    WE_INT32 iCertificateType[IN]:The type of the certificate.
    WE_INT32 iErr[IN]:The value of the err.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSignText(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID,
                        WE_INT32 iSignId, WE_INT32 iAlgorithm,
                      const WE_CHAR * pcSignature, WE_INT32 iSigLen,
                      const WE_CHAR * pcHashedKey, WE_INT32 iHashedKeyLen,
                      const WE_CHAR * pcCertificate, WE_INT32 iCertificateLen,
                      WE_INT32 iCertificateType, WE_INT32 iErr)
{    
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;

    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeSignText\n"));
    iLength = sizeof(iTargetID) + sizeof(iSignId) + sizeof(iAlgorithm) + 
                     iSigLen + sizeof(iSigLen) + sizeof(iHashedKeyLen) + iHashedKeyLen
                     + sizeof(iCertificateLen) + iCertificateLen
                     + sizeof(iCertificateType) + sizeof(iErr);
    
    pstEcdr = Sec_SetMsgDataEncode(G_SIGNTEXT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeInt32( pstEcdr, &iSignId )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iAlgorithm )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iSigLen )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iHashedKeyLen )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iCertificateLen )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iCertificateType )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iErr ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    if ( iSigLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pcSignature, iSigLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    if ( iHashedKeyLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pcHashedKey, iHashedKeyLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }  

    if ( iCertificateLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pcCertificate, iCertificateLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
    	return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);

    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
#endif 

/*==================================================================================================
FUNCTION: 
    Sec_EncodeWtlsGetPrfResult
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    encode prf result and return the result to the stk.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucBuf[IN]:Pointer to the data buffer of prf result.
    WE_INT32 iBufLen[IN]:The length of the buffer.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWtlsGetPrfResult(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                              const WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{       
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWtlsGetPrfResult\n"));  
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(iBufLen) + iBufLen;
    pstEcdr = Sec_SetMsgDataEncode(G_GETPRF_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iBufLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( iBufLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucBuf, iBufLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
    	return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeWtlsVerifySvrCert
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    verify gateway certificate chain and return the result.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWtlsVerifySvrCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{             
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWtlsVerifySvrCert\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_VERIFYSVRCERT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeWtlsGetUsrCert
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    get the user certificate and give the operation result.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucKeyId[IN]:A byte-encoded Identity which is used to 
                        recognize the client private key related with the cert 
                        in the buffer pucCert.
    WE_INT32 iKeyIdLen[IN]:The length of the keyID.
    WE_UCHAR * pucCert[IN]:Pointer to the cert content.
    WE_INT32 iCertLen[IN]:The length of the cert.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWtlsGetUsrCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                            const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                            const WE_UCHAR * pucCert, WE_INT32 iCertLen)
{           
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWtlsGetUsrCert\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(iKeyIdLen) + iKeyIdLen 
                + sizeof(iCertLen) + iCertLen;
    pstEcdr = Sec_SetMsgDataEncode(G_GETUSERCERT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iKeyIdLen )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iCertLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( iKeyIdLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucKeyId, iKeyIdLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    if ( iCertLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucCert, iCertLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
       return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeTlsGetUsrCert
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    get the user certificate and give the operation result.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucKeyId[IN]:A byte-encoded Identity which is used to 
                        recognize the client private key related with the cert 
                        in the buffer pucCert.
    WE_INT32 iKeyIdLen[IN]:The length of the keyID.
    WE_UCHAR * pucCert[IN]:Pointer to the cert content.
    WE_INT32 iCertLen[IN]:The length of the cert.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeTlsGetUsrCert(WE_UINT8 **ppucBuffer,  WE_INT32 iTargetID, WE_INT32 iResult,
                        const WE_UCHAR * pucPubkeyHash, WE_INT32 usPubKeyHashLen,
                        St_SecTlsAsn1Certificate * pstCert, WE_INT32 iNbrCerts)
{
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    WE_INT32          iLoop = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeTlsGetUsrCert\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(usPubKeyHashLen) + usPubKeyHashLen 
                + iNbrCerts*sizeof(St_SecTlsAsn1Certificate) + sizeof(iNbrCerts);
    pstEcdr = Sec_SetMsgDataEncode(G_TLS_GETUSERCERT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
    || -1 == SecEcdr_EncodeInt16( pstEcdr, &usPubKeyHashLen )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iNbrCerts ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }  
    
    if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucPubkeyHash, usPubKeyHashLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( iNbrCerts > 0 )
    {
        for (iLoop = 0;iLoop < iNbrCerts;iLoop ++)
        {
            if (-1 == SecEcdr_EncodeInt8( pstEcdr, &(pstCert[iLoop].ucFormat) ))
            {
                SecEcdr_DestroyEncoder( pstEcdr );
                return -1;
            }
            if(-1 == SecEcdr_EncodeInt32( pstEcdr, &pstCert[iLoop].uiCertLen) )
            {
                SecEcdr_DestroyEncoder( pstEcdr );
                return -1;
            }
            if (-1 == SecEcdr_EncodeOctets( pstEcdr, pstCert->pucCert, pstCert[iLoop].uiCertLen) )
            {
                SecEcdr_DestroyEncoder( pstEcdr );
                return -1;
            }
        }
    }
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeWtlsGetCipherSuite
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    Get cipher suite, and return the result to the caller
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucCipherMethods[IN]:a sequence of elements of cipher suite.
    WE_INT32 iCipherMethodsLen[IN]:The length of the cipher method.
    WE_UCHAR * pucKeyExchangeIds[IN]:pointer including key exchange methods.
    WE_INT32 iKeyExchangeIdsLen[IN]:The length of the key exchange id.
    WE_UCHAR * pucTrustedKeyIds[IN]:Pointer to the trusted CA DN.
    WE_INT32 iTrustedKeyIdsLen[IN]:The length of the trusted key id.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWtlsGetCipherSuite(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_UINT16 usResult,
                                const WE_UCHAR * pucCipherMethods, WE_UINT16 usCipherMethodsLen,
                                const WE_UCHAR * pucKeyExchangeIds, WE_UINT16 usKeyExchangeIdsLen,
                                const WE_UCHAR * pucTrustedKeyIds, WE_UINT16 usTrustedKeyIdsLen)
{ 
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWtlsGetCipherSuite\n"));    
    iLength = sizeof(iTargetID) + sizeof(usResult) + sizeof(usCipherMethodsLen) + usCipherMethodsLen 
                + sizeof(usKeyExchangeIdsLen) + usKeyExchangeIdsLen 
                + sizeof(usTrustedKeyIdsLen) + usTrustedKeyIdsLen;
    pstEcdr = Sec_SetMsgDataEncode(G_GETCIPHERSUITE_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeUint16( pstEcdr, &usResult )
    || -1 == SecEcdr_EncodeUint16( pstEcdr, &usCipherMethodsLen )
    || -1 == SecEcdr_EncodeUint16( pstEcdr, &usKeyExchangeIdsLen )
    || -1 == SecEcdr_EncodeUint16( pstEcdr, &usTrustedKeyIdsLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( usCipherMethodsLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucCipherMethods, usCipherMethodsLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    if ( usKeyExchangeIdsLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucKeyExchangeIds, usKeyExchangeIdsLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    if ( usTrustedKeyIdsLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucTrustedKeyIds, usTrustedKeyIdsLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
       return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeTlsGetCipherSuite
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    Get cipher suite, and return the result to the caller
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucCipherSuites[IN]:a sequence of elements of cipher suite.
    WE_INT32 iCipherSuitesLen[IN]:The length of the cipher method.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeTlsGetCipherSuite(WE_UINT8 **ppucBuffer , WE_INT32 iTargetID, WE_UINT16 usResult,
                               const WE_UCHAR * pucCipherSuites, WE_UINT16 usCipherSuitesLen)
{
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeTlsGetCipherSuite\n"));  
    
    iLength = sizeof(iTargetID) + sizeof(usResult) + sizeof(usCipherSuitesLen) + usCipherSuitesLen ;

    pstEcdr = Sec_SetMsgDataEncode(G_TLS_GETCIPHERSUITE_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeUint16( pstEcdr, &usResult )
    || -1 == SecEcdr_EncodeUint16( pstEcdr, &usCipherSuitesLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( usCipherSuitesLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucCipherSuites, usCipherSuitesLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeWtlsKeyExchange
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    get key exchange, ang return the result to the stk
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
    WE_CHAR * pucPublicValue[IN]:A public value computed by the key exchange 
                                method to be sent to the server side.
    WE_INT32 iPublicValueLen[IN]:The length of the public value.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWtlsKeyExchange(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                             WE_INT32 iMasterSecretId, const WE_UCHAR * pucPublicValue,
                             WE_INT32 iPublicValueLen)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWtlsKeyExchange\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(iMasterSecretId) + iPublicValueLen 
                + sizeof(iPublicValueLen);
    pstEcdr = Sec_SetMsgDataEncode(G_KEYEXCH_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iMasterSecretId )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iPublicValueLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( iPublicValueLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucPublicValue, iPublicValueLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
       return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeWtlsCompSig
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    compute the signature and give the result.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucSignature[IN]:Pointer to the digital signature.
    WE_INT32 iSignatureLen[IN]:The length of the sign nature.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWtlsCompSig(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR * pucSignature, WE_INT32 iSignatureLen)
{    
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWtlsCompSig\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(iSignatureLen) + iSignatureLen;
    pstEcdr = Sec_SetMsgDataEncode(G_COMPUTESIGN_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iSignatureLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( iSignatureLen > 0 )
    {
        if ( -1 == SecEcdr_EncodeOctets( pstEcdr, pucSignature, iSignatureLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
       return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
 }
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSSLGetMasterSecret
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    encode MasterSecret for ssl hash updata..
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucMasterSecret[IN]:Pointer to MasterSecret.
    WE_INT32 iMasterSecretLen[IN]:The length of the MasterSecret.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSSLGetMasterSecret(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR * pucMasterSecret, WE_INT32 iMasterSecretLen)
{    
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeSSLGetMasterSecret\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(iMasterSecretLen) + iMasterSecretLen;
    pstEcdr = Sec_SetMsgDataEncode(G_TLS_GETMASTERSECRET_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iMasterSecretLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( iMasterSecretLen > 0 )
    {
        if ( -1 == SecEcdr_EncodeOctets( pstEcdr, pucMasterSecret, iMasterSecretLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
 }

/*==================================================================================================
FUNCTION: 
    Sec_EncodeSearchPeer
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    response for search peer operation.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UINT8 ucConnection_type[IN]:the connection type.
    WE_INT32 iMasterSecretID[IN]:ID of the master secret.
    WE_INT32 iSecurityID[IN]:ID of the security.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSearchPeer(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                        WE_UINT8 ucConnectionType,WE_INT32 iMasterSecretID,
                        WE_INT32 iSecurityID)
{
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeSearchPeer\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(ucConnectionType)
                + sizeof(iMasterSecretID) + sizeof(iSecurityID);
    pstEcdr = Sec_SetMsgDataEncode(G_SEARCHPEER_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
         || -1 == SecEcdr_EncodeUint8( pstEcdr, &ucConnectionType)
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iMasterSecretID )
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iSecurityID ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSessionGet
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    response for get session information operation.
ARGUMENTS PASSED:
   WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Wap.
   WE_INT32 iTargetID[IN]:The identity of the invoker.
   WE_INT32 iResult[IN]:The value of the result.
   WE_UINT8 ucSessionOptions[IN]:The value of the session option.
   WE_UCHAR * pucSessionId[IN]:Pointer to the ID of the session.
   WE_UINT8 ucSessionIdLen[IN]: Length of the session id.
   WE_UINT8 ucCipherSuite[2][IN]:array for cipher suite.
   WE_UINT8 ucCompressionAlg[IN]: The value of th alg.
   WE_UCHAR * pucPrivateKeyId[IN]:Pointer to the Id of the private key.
   WE_UINT32 uiCreationTime[IN]:The value of the creation time.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSessionGet(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_UINT16 usResult,
                        WE_UINT8 ucSessionOptions, const WE_UCHAR * pucSessionId,
                        WE_UINT16 usSessionIdLen, WE_UINT8 *pucCipherSuite,
                        WE_UINT8 ucCompressionAlg, const WE_UCHAR * pucPrivateKeyId,
                        WE_UINT32 uiCreationTime)
{        
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    WE_UINT8          aucCipherSuite[2] = {0};
    WE_INT32          iPrivateKeyIdLen = 0;
    WE_INT32          iCipherSuiteLen = 2;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeSessionGet\n"));
    if (NULL == pucPrivateKeyId)
    {
        iPrivateKeyIdLen = 0;
    }
    else
    {
        iPrivateKeyIdLen = SEC_STRLEN(pucPrivateKeyId) + 1;
    }
    
    iLength = sizeof(iTargetID) + sizeof(usResult) + sizeof(ucSessionOptions) 
             + sizeof(usSessionIdLen) + usSessionIdLen + iCipherSuiteLen + sizeof(ucCompressionAlg)
             + sizeof(iPrivateKeyIdLen) + iPrivateKeyIdLen + sizeof(uiCreationTime);
    pstEcdr = Sec_SetMsgDataEncode(G_SESSIONGET_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
         || -1 == SecEcdr_EncodeUint16( pstEcdr, &usResult )
         || -1 == SecEcdr_EncodeUint8( pstEcdr, &ucSessionOptions )
         || -1 == SecEcdr_EncodeUint16( pstEcdr, &usSessionIdLen )
         || -1 == SecEcdr_EncodeUint8( pstEcdr, &ucCompressionAlg )
         || -1 == SecEcdr_EncodeUint32( pstEcdr, &uiCreationTime )
         || -1 == SecEcdr_EncodeInt32( pstEcdr, &iPrivateKeyIdLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( usSessionIdLen > 0 )
    {
        if ( -1 == SecEcdr_EncodeOctets( pstEcdr, pucSessionId, usSessionIdLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }   

    if ( NULL == pucCipherSuite )
    {
        aucCipherSuite[0] = 0xff;
        aucCipherSuite[1] = 0xff;
    }
    else
    {
        aucCipherSuite[0] = pucCipherSuite[0];
        aucCipherSuite[1] = pucCipherSuite[1];
    }
    if ( -1 == SecEcdr_EncodeOctets( pstEcdr, (WE_UINT8 *)aucCipherSuite, 2) )
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( iPrivateKeyIdLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucPrivateKeyId, iPrivateKeyIdLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetCertNameList
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    response for view name list of certificate.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iWid[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetCertNameList(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{                  
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeGetCertNameList\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_GETCERTNAME_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;    
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeDeleteCert
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    response for delete certificate.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iCertId[IN]:The certificate id.
    WE_INT32 iResult[IN]:The value of the result.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeDeleteCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, 
                            WE_INT32 iCertId, WE_INT32 iResult)
{
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeDeleteCert\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iCertId) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_DELCERT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iCertId )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
       return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;

}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeKeyPairGen
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    response for generate key pair.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
    WE_UINT8 ucKeyType[IN]: The value of the key type
    WE_UCHAR * pucPublicKey[IN]:Pointer to the public key.
    WE_INT32 iPublicKeyLen[IN]:The length of the public key.
    WE_UCHAR * pucSig[IN]:Pointer to the value of the sign
    WE_INT32 iSigLen[IN]:Length of the sign.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeKeyPairGen(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID,
                        WE_INT32 iResult, WE_UINT8 ucKeyType, 
                        const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen, 
                        const WE_UCHAR * pucSig, WE_INT32 iSigLen)
{  
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeKeyPairGen\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(ucKeyType) + 
             sizeof(iPublicKeyLen) + iPublicKeyLen + sizeof(iSigLen) + iSigLen;
    pstEcdr = Sec_SetMsgDataEncode(G_GENKEYPAIR_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
    || -1 == SecEcdr_EncodeUint8( pstEcdr, &ucKeyType )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iPublicKeyLen )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iSigLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( iPublicKeyLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucPublicKey, iPublicKeyLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    if ( iSigLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucSig, iSigLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetPubKey
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    get user public key response
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
    WE_UCHAR * pucPublicKey[IN]:Pointer to the public key.
    WE_INT32 iPublicKeyLen[IN]:The length of the public key.
    WE_UCHAR * pucSig[IN]:Pointer to the value of the sign
    WE_INT32 iSigLen[IN]:Length of the sign.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetPubKey(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult, 
                      const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen,
                      const WE_UCHAR * pucSig, WE_INT32 iSigLen)
{  
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeGetPubKey\n"));    
    iLength = sizeof(iTargetID) + sizeof(iResult) + 
             sizeof(iPublicKeyLen) + iPublicKeyLen + sizeof(iSigLen) + iSigLen;
    pstEcdr = Sec_SetMsgDataEncode(G_GETPUBKEY_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iPublicKeyLen )
    || -1 == SecEcdr_EncodeInt32( pstEcdr, &iSigLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( iPublicKeyLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucPublicKey, iPublicKeyLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    if ( iSigLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucSig, iSigLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeModifyPin
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    modify pin response.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeModifyPin(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeModifyPin\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_MODIFYPIN_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;   
}
 
/* #endif */

/*==================================================================================================
FUNCTION: 
    Sec_EncodeViewAndGetCert
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    response for view certificate information and get get certificate content
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
    WE_INT32 certId[IN]:ID of the cert.
    WE_UCHAR * pucCert[IN]:Pointer to the cert.
    WE_INT32 iCertLen[IN]:Length of the cert.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeViewAndGetCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult, 
                            WE_INT32 iCertId, WE_UCHAR * pucCert, WE_INT32 iCertLen)
{  
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeViewAndGetCert\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(iCertId) + sizeof(iCertLen) + iCertLen;;
    pstEcdr = Sec_SetMsgDataEncode(G_VIEWGETCERT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iCertId )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iCertLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( iCertLen > 0 )
    {
        if ( -1 == SecEcdr_EncodeOctets( pstEcdr, pucCert, iCertLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetCurSvrCert
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    view current used certificate information 
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetCurSvrCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{            
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeGetCurSvrCert\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_GETCURSVRCERT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetSessionInfo
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    view the current session information
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetSessionInfo(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{       
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeGetSessionInfo\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_GETSESSIONINFO_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeWtlsCurClass
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    get current sec class response
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iSecClass[IN]:The value of the sec class.
    WE_INT32 iInfoAvailable[IN]:The flg of the info available.
    WE_INT32 iCertAvailable[IN]:The flg of the cert available.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWtlsCurClass(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iSecClass,
                          WE_INT32 iInfoAvailable, WE_INT32 iCertAvailable)
{    
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWtlsCurClass\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iSecClass) + sizeof(iInfoAvailable) + sizeof(iCertAvailable);
    pstEcdr = Sec_SetMsgDataEncode(G_WTLSCURCLASS_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iSecClass )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iInfoAvailable )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iCertAvailable ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeClrSession
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    clear session response
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeClrSession(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeClrSession\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_SESSIONCLEAR_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetContractsList
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    view contract content response
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetContractsList(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{  
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeGetContractsList\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_GETCONTRACT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0; 
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeRemoveContract
CREATE DATE:
    2007-03-12
AUTHOR:
    Stone An
DESCRIPTION:
    remove contract response
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeRemoveContract(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeRemoveContract\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult);
    pstEcdr = Sec_SetMsgDataEncode(G_DELCONTRACT_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;     
}
#endif 

/*==================================================================================================
FUNCTION: 
    Sec_EncodeUserCertReq
CREATE DATE:
    2006-11-08
AUTHOR:
    stone an
DESCRIPTION:
    handle the request message of user certificate.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UCHAR* pucCertReqMsg: request message.
    WE_UINT32 uiCertReqMsgLen: length of request message.    
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeUserCertReq(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult, 
                                   WE_UCHAR* pucCertReqMsg, WE_UINT32 uiCertReqMsgLen)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeUserCertReq\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(uiCertReqMsgLen) + uiCertReqMsgLen;
    pstEcdr = Sec_SetMsgDataEncode(G_USERCERTREQ_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
    ||-1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
    || -1 == SecEcdr_EncodeUint32( pstEcdr, &uiCertReqMsgLen ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }    

    if ( uiCertReqMsgLen > 0 )
    {
        if (-1 == SecEcdr_EncodeOctets( pstEcdr, pucCertReqMsg, uiCertReqMsgLen) )
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}
    

/*==================================================================================================
FUNCTION: 
    Sec_EncodeChangeWTLSCertAble
CREATE DATE:
    2006-11-08
AUTHOR:
    stone an
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeChangeWTLSCertAble(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, 
                                        WE_INT32 iResult, WE_UINT8 ucState)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeChangeWTLSCertAble\n"));
    
    iLength = sizeof(iTargetID) + sizeof(iResult) + sizeof(ucState);
    pstEcdr = Sec_SetMsgDataEncode(G_CHANGECERTSTATE_RESP, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &iTargetID )
        || -1 == SecEcdr_EncodeInt32( pstEcdr, &iResult )
        || -1 == SecEcdr_EncodeUint8( pstEcdr, &ucState ))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*UE Encode*/
/*==================================================================================================
FUNCTION: 
    Sec_EncodeConfirm
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the ue event and return the result to browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeConfirm(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_Confirm* pstTemp = (St_Confirm*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeConfirm\n"));
    
    iLength = sizeof(pstTemp->iConfirmStrId);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iConfirmStrId )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeChangePin
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode change pin and return the result to broswer.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeChangePin(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_ChangePin* pstTemp = (St_ChangePin*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeChangePin\n"));
    
    iLength = sizeof(pstTemp->iNewPinId) + sizeof(pstTemp->iNewPinIdAfter);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNewPinId )) ||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNewPinIdAfter )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeSelCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the cert name and return to browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSelCert(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_SelectCert* pstTemp = (St_SelectCert*)pvData; 
    WE_INT32        iNumOfCert  = 0;
    WE_INT32       iIndex = 0;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeSelCert\n"));
    
    iNumOfCert = pstTemp->nbrOfCerts;
    /*cal size*/
    for(iIndex = 0;iIndex < iNumOfCert; iIndex ++)
    {
        iLength += SEC_STRLEN(pstTemp->ppcCertName[iIndex]) + 1;
    }    
    iLength += sizeof(pstTemp->nbrOfCerts);
    
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->nbrOfCerts)))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    for(iIndex = 0;iIndex < iNumOfCert; iIndex ++)
    {
        if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->ppcCertName[iIndex])))
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }    
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeNameConfirm
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the name confirm event and return to browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeNameConfirm(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_NameConfirm* pstTemp = (St_NameConfirm*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeNameConfirm\n"));
    
    iLength = SEC_STRLEN(pstTemp->pcCertName) + 1;
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcCertName)))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}


/*==================================================================================================
FUNCTION: 
    Sec_EncodeSigntextConfirm
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the Signtext confirm event and return to browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSigntextConfirm(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_SignTextConfirm* pstTemp = (St_SignTextConfirm*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeSigntextConfirm\n"));
    
    iLength = sizeof(pstTemp->iInfoId) + SEC_STRLEN(pstTemp->pcText) +1;
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iInfoId )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcText )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeCreatePin
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the creat pin event and return to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeCreatePin(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_CreatePin* pstTemp = (St_CreatePin*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeCreatePin\n"));
    
    iLength = sizeof(pstTemp->iInfoId) + sizeof(pstTemp->iInfoIdAfter);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iInfoId ))||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iInfoIdAfter )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeHash
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the value of hash and event,then return to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeHash(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_Hash* pstTemp = (St_Hash*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeHash\n"));
    
    iLength = sizeof(pstTemp->iInfoId) + SEC_STRLEN(pstTemp->pcDisplayName) + 1;
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iInfoId )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcDisplayName )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodePin
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the value of pin and event, then return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodePin(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_Pin* pstTemp = (St_Pin*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodePin\n"));
    
    iLength = sizeof(pstTemp->iInfoId) + sizeof(pstTemp->iInfoIdAfer);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iInfoId ))||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iInfoIdAfer )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeShowCertContent
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the certificate information and return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeShowCertContent(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_ShowCertContent* pstTemp = (St_ShowCertContent*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeShowCertContent\n"));
    
    iLength = SEC_STRLEN(pstTemp->pcIssuerStr) +1 + 
                  SEC_STRLEN(pstTemp->pcNotAftStr) +1 + 
                  SEC_STRLEN(pstTemp->pcNotBefStr) +1 + 
                  SEC_STRLEN(pstTemp->pcSubjectStr) +1 +
                  SEC_STRLEN(pstTemp->pucAlg) +1 +
                  SEC_STRLEN(pstTemp->pucPubKey) +1 +
                  SEC_STRLEN(pstTemp->pucSerialNumber) +1 +
                  sizeof(pstTemp->iNotAftMonth) +
                  sizeof(pstTemp->iNotBefMonth);

    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNotAftMonth )) ||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNotBefMonth )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcIssuerStr )) ||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcNotAftStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcNotBefStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcSubjectStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pucAlg ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pucPubKey ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pucSerialNumber )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeShowCertList
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode  list of the ids and the friendly names of the stored certificates,
    return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeShowCertList(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_ShowCertList* pstTemp = (St_ShowCertList*)pvData; 
    WE_INT32          iNumOfCert = 0;
    WE_INT32          iIndex = 0;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeShowCertList\n"));
    
    iNumOfCert = pstTemp->nbrOfCerts;
    /*cal size*/
    for(iIndex = 0;iIndex < iNumOfCert; iIndex ++)
    {
        iLength += SEC_STRLEN(pstTemp->ppcCertName[iIndex]) + 1;
    }    
    iLength += sizeof(pstTemp->nbrOfCerts) + iNumOfCert * sizeof(WE_UINT8) +  iNumOfCert * sizeof(E_CertType);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->nbrOfCerts )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    for(iIndex = 0;iIndex < iNumOfCert; iIndex ++)
    {
        if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->ppcCertName[iIndex])))
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }    
    if ( -1 == SecEcdr_EncodeOctets( pstEcdr, (pstTemp->pucAble ),iNumOfCert))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    if ( -1 == SecEcdr_EncodeOctets( pstEcdr, (pstTemp->pueCertType ),iNumOfCert * sizeof(E_CertType)))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeShowSessionContent
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the session info of the current session,and return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeShowSessionContent(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_ShowSessionContent* pstTemp = (St_ShowSessionContent*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeShowSessionContent\n"));
    
    iLength = sizeof(pstTemp->iConnTypeId) + sizeof(pstTemp->iEncralgId) + 
                  sizeof(pstTemp->iHmacId)  + sizeof(pstTemp->iKeyExchId) +
                  SEC_STRLEN(pstTemp->pcEncrKeyLenStr) +1 +
                  SEC_STRLEN(pstTemp->pcHmacKeyLenStr) +1 +
                  SEC_STRLEN(pstTemp->pcKeyExchKeyLenStr) +1;
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iConnTypeId )) ||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iEncralgId ))||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iHmacId ))||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iKeyExchId )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcEncrKeyLenStr)) ||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcHmacKeyLenStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcKeyExchKeyLenStr )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeShowContractList
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the list of contracts information,and return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeShowContractList(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_ShowContractsList* pstTemp = (St_ShowContractsList*)pvData; 
    WE_INT32          iNumOfContract = 0;
    WE_INT32          iIndex = 0;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeShowContractList\n"));
    
    iNumOfContract = pstTemp->nbrOfContract;
    /*cal size*/
    for(iIndex = 0;iIndex < iNumOfContract; iIndex ++)
    {
        iLength += SEC_STRLEN(pstTemp->ppcTime[iIndex]) + 1;
    }    
    iLength += sizeof(pstTemp->nbrOfContract) + iNumOfContract * sizeof(WE_INT32);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->nbrOfContract )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    for(iIndex = 0;iIndex < iNumOfContract; iIndex ++)
    {
        if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->ppcTime[iIndex])))
        {
            SecEcdr_DestroyEncoder( pstEcdr );
            return -1;
        }
    }    
    if ( -1 == SecEcdr_EncodeOctets( pstEcdr, (pstTemp->piMonthID),iNumOfContract * sizeof(WE_INT32)))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeStoreCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:    
    encode the event of asking if the end user wants to store a certificate,
    and return the result to the browser.    
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeStoreCert(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_StoreCert* pstTemp = (St_StoreCert*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeStoreCert\n"));
    iLength = SEC_STRLEN(pstTemp->pcIssuerStr) +1 + 
                  SEC_STRLEN(pstTemp->pcNotAftStr) +1 + 
                  SEC_STRLEN(pstTemp->pcNotBefStr) +1 + 
                  SEC_STRLEN(pstTemp->pcSubjectStr) +1 +
                  sizeof(pstTemp->iNotAftMonth) +
                  sizeof(pstTemp->iNotBefMonth);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNotAftMonth )) ||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNotBefMonth )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcIssuerStr )) ||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcNotAftStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcNotBefStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcSubjectStr )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeWarning
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the warning and return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeWarning(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_Warning* pstTemp = (St_Warning*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeWarning\n"));
    
    iLength = sizeof(pstTemp->iWarningContent);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iWarningContent )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeStoreContract
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode contract content and return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeStoreContract(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_ShowContractContent* pstTemp = (St_ShowContractContent*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeStoreContract\n"));
    
    iLength = sizeof(pstTemp->iMonthID) + SEC_STRLEN(pstTemp ->pcContractStr) + 1 +
                  SEC_STRLEN(pstTemp ->pcSignatureStr) + 1 +
                  SEC_STRLEN(pstTemp ->pcTimeStr) + 1;
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iMonthID )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcContractStr )) ||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcSignatureStr )) ||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcTimeStr )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_EncodeCurSvrCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    encode the information of current server certificate,and return the result to the browser.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to browser.
    WE_INT32 iEvent[IN]:event.
    WE_VOID* pvData[IN]:Pointer to the data.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeCurSvrCert(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData)
{   
    St_SecEcdrEncoder *pstEcdr = NULL;
    WE_INT32          iLength = 0;
    WE_INT32          iRes = 0;
    St_ShowCertContent* pstTemp = (St_ShowCertContent*)pvData; 
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_EncodeCurSvrCert\n"));
    
    iLength = SEC_STRLEN(pstTemp->pcIssuerStr) +1 + 
                  SEC_STRLEN(pstTemp->pcNotAftStr) +1 + 
                  SEC_STRLEN(pstTemp->pcNotBefStr) +1 + 
                  SEC_STRLEN(pstTemp->pcSubjectStr) +1 +
                  SEC_STRLEN(pstTemp->pucAlg) +1 +
                  SEC_STRLEN(pstTemp->pucPubKey) +1 +
                  SEC_STRLEN(pstTemp->pucSerialNumber) +1 +
                  sizeof(pstTemp->iNotAftMonth) +
                  sizeof(pstTemp->iNotBefMonth);
    /*encode event & length*/
    pstEcdr = Sec_SetMsgDataEncode(iEvent, iLength);                                      
    if (NULL == pstEcdr)
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }

    if ( -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNotAftMonth )) ||
        -1 == SecEcdr_EncodeInt32( pstEcdr, &(pstTemp->iNotBefMonth )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    if ( -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcIssuerStr )) ||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcNotAftStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcNotBefStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pcSubjectStr ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pucAlg ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pucPubKey ))||
        -1 == SecEcdr_EncodeString( pstEcdr, (pstTemp->pucSerialNumber )))
    {
        SecEcdr_DestroyEncoder( pstEcdr );
        return -1;
    }
    
    *ppucBuffer = (WE_UINT8 *)WE_MALLOC((WE_ULONG)(pstEcdr->iLength) * sizeof(WE_UINT8));
    if (NULL == *ppucBuffer)
    {
        return -1;
    }
    (WE_VOID)WE_MEMCPY(*ppucBuffer, pstEcdr->pucData, pstEcdr->iLength);
    
    SecEcdr_DestroyEncoder( pstEcdr );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeConnType
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the type of connection.
ARGUMENTS PASSED:
    WE_VOID *pvMsg[IN]:encoded data.
    WE_INT32 *piType[OUT]:type.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeConnType
(
    WE_VOID *pvMsg,
    WE_INT32 *piType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }    

    iRet = SecEcdr_DecodeInt32(pstDecoder, piType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstDecoder);
        return -1;
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetCipherSuite
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the type of connection and target id.
ARGUMENTS PASSED:
    WE_VOID *pvMsg[IN]:encoded data.
    WE_INT32* piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piType[OUT]:type.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeGetCipherSuite
(
    WE_VOID *pvMsg,
    WE_INT32* piTargetID,
    WE_UINT8* pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint8(pstDecoder,pucConnType);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeSetUpConn
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_SetupConnection.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32  *piTargetID[OUT]:the identity of the invoker.
    WE_INT32  *piMasterSecretId[OUT]:ID of the master secret.
    WE_INT32  *piSecId[OUT]:ID of the security.
    WE_INT32  *piFullHandshake[OUT]:whether is full hand shake.
    St_SecSessionInformation    *pstSessionInfo[OUT]:Information of the session.
    WE_UINT8   *pucConnType[OUT]:the type of connection.

RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeSetUpConn
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piMasterSecretId, 
    WE_INT32 *piSecId, 
    WE_INT32 *piFullHandshake,
    St_SecSessionInformation *pstSessionInfo,
    WE_UINT8   *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piMasterSecretId);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piFullHandshake);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    

     iRet = SecEcdr_DecodeUint32(pstDecoder, &pstSessionInfo->uiCertLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    if (pstSessionInfo->uiCertLen > 0)
    {
        pstSessionInfo->pucCert = (WE_UCHAR*)WE_MALLOC(pstSessionInfo->uiCertLen);
        if (NULL == pstSessionInfo->pucCert)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;            
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder, pstSessionInfo->pucCert,pstSessionInfo->uiCertLen);
        if (iRet < 0)
        {
            WE_FREE(pstSessionInfo->pucCert);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }        
    }    
    iRet = SecEcdr_EncodeUint16(pstDecoder, &(pstSessionInfo->usKeyExchangeKeyLen));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint16(pstDecoder, &(pstSessionInfo->usHmacLen));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint16(pstDecoder, &(pstSessionInfo->usEncryptionKeyLen));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstDecoder, &(pstSessionInfo->ucSecurityClass));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstDecoder, &(pstSessionInfo->ucKeyExchangeAlg));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstDecoder, &(pstSessionInfo->ucHmacAlg));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstDecoder, &(pstSessionInfo->ucEncryptionAlg));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstDecoder, &(pstSessionInfo->ucNumCerts));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstDecoder, &(pstSessionInfo->ucCertFormat));
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstDecoder, pucConnType);
    if (iRet < 0)
    {
        WE_FREE(pstSessionInfo->pucCert);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeStopConn
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_StopConnection.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32  *piTargetID[OUT]:the identity of the invoker.
    WE_UINT8   *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeStopConn
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piSecId, 
    WE_UINT8   *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucConnType);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeRemovePeer
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_RemovePeer.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32  *piMasterSecretId[OUT]:ID of the master secret.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeRemovePeer
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg);    
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeAttachPeerToSession
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_AttachPeerToSession.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_UCHAR  **ppucAddress[OUT]:Pointer to the address.
    WE_INT32  *piAddressLen[OUT]:Length of the address.
    WE_UINT16 *pusPortnum[OUT]:Number of the port.
    WE_INT32  *piMasterSecretId[OUT]:ID of the master secret.
    WE_UINT8  *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeAttachPeerToSession
(
    WE_VOID *pvMsg,
    WE_UCHAR **ppucAddress, 
    WE_INT32 *piAddressLen,
    WE_UINT16 *pusPortnum, 
    WE_INT32 *piMasterSecretId,
    WE_UINT8   *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg);    
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piAddressLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    *ppucAddress = (WE_UCHAR*)WE_MALLOC(*piAddressLen);
    if (NULL == *ppucAddress)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppucAddress,*piAddressLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucAddress);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint16(pstDecoder,pusPortnum);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucAddress);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder,piMasterSecretId);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucAddress);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucConnType);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucAddress);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeSearchPeer
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_SearchPeer.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_UCHAR  **ppucAddress[OUT]:Pointer to the address.
    WE_INT32  *piAddressLen[OUT]:Length of the address.
    WE_UINT16 *pusPortnum[OUT]:Number of the port.
    WE_UINT8  *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeSearchPeer
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_UCHAR **ppucAddress, 
    WE_INT32  *piAddressLen,
    WE_UINT16 *pusPortNum, 
    WE_UINT8  *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg);    
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piAddressLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    *ppucAddress = (WE_UCHAR*)WE_MALLOC(*piAddressLen);
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppucAddress,*piAddressLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucAddress);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint16(pstDecoder,pusPortNum);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucAddress);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucConnType);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucAddress);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
    
}  
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEnableSession
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_EnableSession.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32  *piMasterSecretId[OUT]:ID of the master secret.
    WE_UINT8 *pucIsActive[OUT]:option.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEnableSession
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId,
    WE_UINT8 *pucIsActive
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg); 
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint8(pstDecoder,pucIsActive);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeDisableSession
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_DisableSession.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32  *piMasterSecretId[OUT]:ID of the master secret.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeDisableSession
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg); 
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeSessionGet
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_SessionGet.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32  *piMasterSecretId[OUT]:ID of the master secret.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeSessionGet
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piMasterSecID
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg); 
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piMasterSecID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeSessionRenew
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_SessionRenew.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piMasterSecID[OUT]:ID of the master secret.
    WE_UINT8 *pucSessionOpts[OUT]:The option of the session.
    WE_UCHAR **ppucSessionId[OUT]:Pointer to the session ID.
    WE_UINT8 *pucSessionIdLen[OUT]:Length of the session id.
    WE_UINT8 **ppucCipherSuite[OUT]:Array for cipher suite.
    WE_UINT8 *pucCompressionAlg[OUT]:The value of the compression Algorithm.
    WE_UCHAR **ppucPrivateKeyId[OUT]:ID of the private key.
    WE_UINT32 *puiCreationTime[OUT]:the time of creation.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeSessionRenew
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecID,
    WE_UINT8 *pucSessionOpts,
    WE_UCHAR **ppucSessionId,
    WE_UINT8 *pucSessionIdLen,
    WE_UINT8 **ppucCipherSuite,
    WE_UINT8 *pucCompressionAlg,
    WE_UCHAR **ppucPrivateKeyId,
    WE_UINT32 *puiCreationTime
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iPrivateKeyIdLen = 0;
    WE_INT32 iDataLenTmp = 0;

    pstDecoder= Sec_GetMsgDecode(pvMsg);
    
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piMasterSecID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucSessionOpts);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucSessionIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    *ppucSessionId = (WE_UCHAR*)WE_MALLOC(*pucSessionIdLen);
    if (NULL == *ppucSessionId)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppucSessionId, *pucSessionIdLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSessionId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32( pstDecoder,&iDataLenTmp);// iDataLenTmp = 2
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSessionId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    **ppucCipherSuite = (WE_UCHAR*)WE_MALLOC(iDataLenTmp);
    if (NULL == *ppucCipherSuite)
    {
        WE_FREE(*ppucSessionId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppucCipherSuite, iDataLenTmp);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSessionId);
        WE_FREE(*ppucCipherSuite);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint8( pstDecoder, pucCompressionAlg);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSessionId);
        WE_FREE(*ppucCipherSuite);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder,&iPrivateKeyIdLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSessionId);
        WE_FREE(*ppucCipherSuite);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    *ppucPrivateKeyId = (WE_UCHAR*)WE_MALLOC(iPrivateKeyIdLen);
    if ( NULL ==  *ppucPrivateKeyId)
    {
        WE_FREE(*ppucSessionId);
        WE_FREE(*ppucCipherSuite);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    iRet = SecEcdr_DecodeOctets(pstDecoder, *ppucPrivateKeyId, iPrivateKeyIdLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSessionId);
        WE_FREE(*ppucCipherSuite);
        WE_FREE(*ppucPrivateKeyId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint32(pstDecoder, puiCreationTime);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSessionId);
        WE_FREE(*ppucCipherSuite);
        WE_FREE(*ppucPrivateKeyId);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetPrfResult
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_GetPrfResult.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_UINT8 *pucAlg[OUT]:The value of the Algorithm.
    WE_INT32 *piMasterSecretId[OUT]: ID of master secret.
    WE_UCHAR **ppucSecret[OUT]:Pointer to the secret.
    WE_INT32 *piSecretLen[OUT]:Length of the seed.
    WE_CHAR  **ppcLabel[OUT]:Pointer to lable.
    WE_UCHAR **ppucSeed[OUT]:Pointer to seed.
    WE_INT32 *piSeedLen[OUT]:Length of the secret.
    WE_INT32 *piOutputLen[OUT]:Length of the output.
    WE_UINT8 *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeGetPrfResult
(
    WE_VOID  *pvMsg,
    WE_INT32 *piTargetID, 
    WE_UINT8 *pucAlg,
    WE_INT32 *piMasterSecretId, 
    WE_UCHAR **ppucSecret, 
    WE_INT32 *piSecretLen, 
    WE_CHAR  **ppcLabel, 
    WE_UCHAR **ppucSeed,
    WE_INT32 *piSeedLen,
    WE_INT32 *piOutputLen,
    WE_UINT8 *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iLableLen = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32( pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucAlg);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder,piMasterSecretId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder, piSecretLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    *ppucSecret = (WE_UCHAR*)WE_MALLOC(*piSecretLen);
    if ( NULL == *ppucSecret)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }       
    iRet = SecEcdr_DecodeOctets(pstDecoder, *ppucSecret, *piSecretLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSecret);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder, &iLableLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSecret);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    if (iLableLen > 1)
    {
        *ppcLabel = (WE_CHAR*)WE_MALLOC(iLableLen);
        if (NULL == *ppcLabel)
        {
            WE_FREE(*ppucSecret);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder, *ppcLabel, iLableLen);
        if ( -1 == iRet)
        {
            WE_FREE(*ppucSecret);
            WE_FREE(*ppcLabel);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piSeedLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSecret);
        WE_FREE(*ppcLabel);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    *ppucSeed = (WE_UCHAR*)WE_MALLOC(*piSeedLen);
    if (NULL == *ppucSeed)
    {
        WE_FREE(*ppucSecret);;
        WE_FREE(*ppcLabel);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder, *ppucSeed, *piSeedLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSecret);
        WE_FREE(*ppcLabel); 
        WE_FREE(*ppucSeed);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32( pstDecoder,piOutputLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSecret);
        WE_FREE(*ppcLabel); 
        WE_FREE(*ppucSeed);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucConnType); 
    if ( -1 == iRet)
    {
        WE_FREE(*ppucSecret);
        WE_FREE(*ppcLabel); 
        WE_FREE(*ppucSeed);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeVerifySvrCertChain
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_VerifySvrCertChain.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_UINT8 **ppvBuf[OUT]:Pointer of buffer.
    WE_INT32 *piBufLen[OUT]:Length of buffer.
    WE_UINT8 *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeVerifySvrCertChain
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvBuf,
    WE_INT32 *piBufLen,
    WE_UINT8 *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;    

    if ((NULL == piTargetId) || (NULL == ppvBuf) || (NULL == piBufLen) || (NULL == pucConnType))
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder, piBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    } 
    *ppvBuf = WE_MALLOC(*piBufLen);
    if (NULL == *ppvBuf)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppvBuf,*piBufLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppvBuf);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    } 
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucConnType);
    if ( -1 == iRet)
    {
        WE_FREE(*ppvBuf);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeKeyExchange
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_VerifySvrCertChain.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_UINT8 **ppvParam[OUT]:Pointer of parameter.
    WE_INT32 *pucAlg[OUT]:alg.
    WE_UCHAR **ppucRandVal[OUT]:random.
    WE_UINT8 *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeKeyExchange
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvParam,
    WE_UINT8 *pucAlg,
    WE_UCHAR **ppucRandVal,
    WE_UINT8 *pucConnType
)
    {
        St_SecEcdrDecoder *pstDecoder = NULL;
        WE_INT32 iRet = -1;  
        WE_INT32 iRandValLen = 0;
        St_SecTlsKeyExchangeParams *pstTlsParams = NULL;
        St_SecWtlsKeyExchParams *pstWtlsParams = NULL;
        WE_LOG_MSG((0,0,"Sec_eds.c:.....into Sec_DecodeKeyExchange\n"));

        if (NULL == ppvParam)
        {
            return -1;
        }
        pstDecoder = Sec_GetMsgDecode(pvMsg);
        if (NULL == pstDecoder)
        {
            return -1;
        }    
        iRet = SecEcdr_DecodeUint8(pstDecoder,pucConnType);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }      

       iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetId);
       if ( -1 == iRet)
       {
           SecEcdr_DestroyDecoder(pstDecoder);
           return -1;
       }   
       iRet = SecEcdr_DecodeUint8(pstDecoder, pucAlg);
       if ( -1 == iRet)
       {
           SecEcdr_DestroyDecoder(pstDecoder);
           return -1;
       }  
       iRet = SecEcdr_DecodeInt32(pstDecoder,&iRandValLen);    
       if ( -1 == iRet)
       {
           SecEcdr_DestroyDecoder(pstDecoder);
           return -1;
       }   
       if (iRandValLen > 0)
       {        
           *ppucRandVal = (WE_UCHAR*)WE_MALLOC(iRandValLen);
           if (NULL == *ppucRandVal)
           {
               SecEcdr_DestroyDecoder(pstDecoder);
               return -1;            
           }
           iRet = SecEcdr_DecodeOctets(pstDecoder,*ppucRandVal,iRandValLen);
           if ( -1 == iRet)
           {
               WE_FREE(*ppucRandVal);
               SecEcdr_DestroyDecoder(pstDecoder);
               return -1;
           }    
       }
       if (*pucConnType == G_SEC_TLS_CONNECTION_TYPE)     
       {
            pstTlsParams = (St_SecTlsKeyExchangeParams*)WE_MALLOC(sizeof(St_SecTlsKeyExchangeParams));
            /*stRSAParam*/
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stRSAParam.stTlsRSAParam.usModLen);
           if ( -1 == iRet)
           {
               WE_FREE(*ppucRandVal);
               SecEcdr_DestroyDecoder(pstDecoder);
               return -1;
           }             
            if (pstTlsParams->stRSAParam.stTlsRSAParam.usModLen > 0)
            {
                pstTlsParams->stRSAParam.stTlsRSAParam.pucModulus = (WE_UCHAR*)WE_MALLOC(pstTlsParams->stRSAParam.stTlsRSAParam.usModLen);
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstTlsParams->stRSAParam.stTlsRSAParam.pucModulus,
                                            pstTlsParams->stRSAParam.stTlsRSAParam.usModLen);   
                if ( -1 == iRet)
                {
                    WE_FREE(*ppucRandVal);
                    WE_FREE(pstTlsParams->stRSAParam.stTlsRSAParam.pucModulus );
                    SecEcdr_DestroyDecoder(pstDecoder);
                    return -1;
                }                 
            }
    
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen);
            if (pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen > 0)
            {
                pstTlsParams->stRSAParam.stTlsRSAParam.pucExponent= (WE_UCHAR*)WE_MALLOC(pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen);
                if( pstTlsParams->stRSAParam.stTlsRSAParam.pucExponent == NULL)
                {
                    return -1;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstTlsParams->stRSAParam.stTlsRSAParam.pucExponent,
                                            pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen);            
            }    
    
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stRSAParam.stTlsSignature.usSignLen);
            if (pstTlsParams->stRSAParam.stTlsSignature.usSignLen > 0)
            {
                pstTlsParams->stRSAParam.stTlsRSAParam.pucExponent = (WE_UCHAR*)WE_MALLOC(pstTlsParams->stRSAParam.stTlsSignature.usSignLen);
                if( pstTlsParams->stRSAParam.stTlsRSAParam.pucExponent == NULL)
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstTlsParams->stRSAParam.stTlsSignature.pucSignature,
                                            pstTlsParams->stRSAParam.stTlsSignature.usSignLen);            
            }
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stRSAParam.stTlsSignature.usReserved);

            /*stDHParam*/
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stDHParam.stTlsDHParam.usDHGLen);
            if (pstTlsParams->stDHParam.stTlsDHParam.usDHGLen > 0)
            {
                pstTlsParams->stDHParam.stTlsDHParam.pucDHG = (WE_UCHAR*)WE_MALLOC(pstTlsParams->stDHParam.stTlsDHParam.usDHGLen);
                if (pstTlsParams->stDHParam.stTlsDHParam.pucDHG == NULL)
                {
                    return -1;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstTlsParams->stDHParam.stTlsDHParam.pucDHG,
                                            pstTlsParams->stDHParam.stTlsDHParam.usDHGLen);            
            }
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stDHParam.stTlsDHParam.usDHPLen);
            if (pstTlsParams->stDHParam.stTlsDHParam.usDHPLen > 0)
            {
                pstTlsParams->stDHParam.stTlsDHParam.pucDHP = (WE_UCHAR *)WE_MALLOC(pstTlsParams->stDHParam.stTlsDHParam.usDHPLen);
                if(pstTlsParams->stDHParam.stTlsDHParam.pucDHP == NULL)
                {
                    return -1;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstTlsParams->stDHParam.stTlsDHParam.pucDHP,
                                            pstTlsParams->stDHParam.stTlsDHParam.usDHPLen);            
            }
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen);
            if (pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen> 0)
            {
                pstTlsParams->stDHParam.stTlsDHParam.pucDHYs = (WE_UCHAR*)WE_MALLOC(pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen);
                if (pstTlsParams->stDHParam.stTlsDHParam.pucDHYs == NULL)
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstTlsParams->stDHParam.stTlsDHParam.pucDHYs,
                                            pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen);            
            }
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stDHParam.stTlsDHParam.usReserved1);

            /*stSecCert*/
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstTlsParams->stSecCert.usBufLen);
            if (pstTlsParams->stSecCert.usBufLen > 0)
            {
                pstTlsParams->stSecCert.pucBuf = (WE_UCHAR*)WE_MALLOC(pstTlsParams->stSecCert.usBufLen);
                if (pstTlsParams->stSecCert.pucBuf == NULL)
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstTlsParams->stSecCert.pucBuf,
                                            pstTlsParams->stSecCert.usBufLen);            
            }
            /*iOptions*/
            iRet = SecEcdr_DecodeInt32(pstDecoder,&pstTlsParams->iOptions);
            *ppvParam = (WE_VOID*)pstTlsParams;
       }
       else if ( *pucConnType == G_SEC_WTLS_CONNECTION_TYPE)
       {
            pstWtlsParams = (St_SecWtlsKeyExchParams *)WE_MALLOC(sizeof(St_SecWtlsKeyExchParams));
            if (!pstWtlsParams)
            {
                return -1 ;
            }
            /*stParams*/
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstWtlsParams->stParams.stCertificates.usBufLen);
            if (pstWtlsParams->stParams.stCertificates.usBufLen > 0)
            {
                pstWtlsParams->stParams.stCertificates.pucBuf = (WE_UCHAR*)WE_MALLOC(pstWtlsParams->stParams.stCertificates.usBufLen);
                if (pstWtlsParams->stParams.stCertificates.pucBuf ==NULL)
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstWtlsParams->stParams.stCertificates.pucBuf,
                                            pstWtlsParams->stParams.stCertificates.usBufLen);            
            }    
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen);
            if (pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen > 0)
            {
                pstWtlsParams->stParams.stKeyParam.stParamSpecifier.pucParams = (WE_UCHAR*)WE_MALLOC(pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen);   
                if (pstWtlsParams->stParams.stKeyParam.stParamSpecifier.pucParams == NULL)
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstWtlsParams->stParams.stKeyParam.stParamSpecifier.pucParams,
                                            pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen);            
            }           
            iRet = SecEcdr_DecodeUint8(pstDecoder,&pstWtlsParams->stParams.stKeyParam.stParamSpecifier.ucParameterIndex);
    
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen);
            if (pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen > 0)
            {
                pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent = (WE_UCHAR*)WE_MALLOC(pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen);
                if ( pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent == NULL )
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent,
                                            pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen );             
            }
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen);
            if (pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen > 0)
            {
                pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus = (WE_UCHAR*)WE_MALLOC(pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen);
                if (pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus == NULL)
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus,
                                            pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen );             
            }   
            /*stParams.stSecretKey*/
            iRet = SecEcdr_DecodeUint16(pstDecoder,&pstWtlsParams->stParams.stSecretKey.usIdLen);
            if (pstWtlsParams->stParams.stSecretKey.usIdLen > 0)
            {
                pstWtlsParams->stParams.stSecretKey.pucIdentifier = (WE_UCHAR*)WE_MALLOC(pstWtlsParams->stParams.stSecretKey.usIdLen);
                if ( pstWtlsParams->stParams.stSecretKey.pucIdentifier == NULL)
                {
                    return -1 ;
                }
                iRet = SecEcdr_DecodeOctets(pstDecoder,
                                            pstWtlsParams->stParams.stSecretKey.pucIdentifier,
                                            pstWtlsParams->stParams.stSecretKey.usIdLen );               
            }   
            
            /*ucKeyExchangeSuite*/
            iRet = SecEcdr_DecodeUint8(pstDecoder,&(pstWtlsParams->ucKeyExchangeSuite));
            *ppvParam = (WE_VOID*)pstWtlsParams;
       }
       else
       {
           if(*ppucRandVal)
           {
               WE_FREE(*ppucRandVal);
           }
           return -1 ;
       }
       SecEcdr_DestroyEncoder(pstDecoder);
       
       WE_LOG_MSG((0,0,"Sec_eds.c:.....exit Sec_DecodeKeyExchange\n"));
       return 0; 
    }

/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetUserCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_GetUserCert.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_UINT8 **ppvBuf[OUT]:Pointer of parameter.
    WE_INT32 *piBufLen[OUT]:length of buffer.
    WE_UINT8 *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeGetUserCert
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvBuf,
    WE_INT32 *piBufLen,
    WE_UINT8 *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;    

    if ((NULL == piTargetId) || (NULL == ppvBuf) || (NULL == piBufLen) || (NULL == pucConnType))
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder, piBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    *ppvBuf = WE_MALLOC(*piBufLen);
    if ( NULL == *ppvBuf )
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    } 

    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppvBuf,*piBufLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppvBuf);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    } 
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucConnType);
    if ( -1 == iRet)
    {
         WE_FREE(*ppvBuf);
         SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeCompSign
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_CompSign.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_UCHAR **ppucKeyId[OUT]:Pointer to the key id.
    WE_INT32 *piKeyIdLen[OUT]:Length of the key id.
    WE_UCHAR **ppucBuf[OUT]:Pointer to the buffer.
    WE_INT32 *piBufLen[OUT]:Length of the buffer.
    WE_UINT8 *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeCompSign
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_UCHAR **ppucKeyId,
    WE_INT32 *piKeyIdLen,
    WE_UCHAR **ppucBuf,
    WE_INT32 *piBufLen,
    WE_UINT8 *pucConnType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;    

    if ((NULL == piTargetId) || (NULL == ppucKeyId) || (NULL == piKeyIdLen) || (NULL == ppucBuf)
         || (NULL == piBufLen) || (NULL == pucConnType))
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeInt32(pstDecoder, piKeyIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    *ppucKeyId = (WE_UCHAR*)WE_MALLOC(*piKeyIdLen);
    if (NULL == *ppucKeyId)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppucKeyId,*piKeyIdLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucKeyId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    iRet = SecEcdr_DecodeInt32(pstDecoder,piBufLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucKeyId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    } 
    *ppucBuf = (WE_UCHAR*)WE_MALLOC(*piBufLen);
    if (NULL == *ppucBuf)
    {
        WE_FREE(*ppucKeyId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppucBuf,*piBufLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucKeyId);
        WE_FREE(*ppucBuf);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucConnType);
    if ( -1 == iRet)
    {
        WE_FREE(*ppucKeyId);
        WE_FREE(*ppucBuf);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeCompSign
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_CompSign.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_UCHAR **ppucKeyId[OUT]:Pointer to the key id.
    WE_INT32 *piKeyIdLen[OUT]:Length of the key id.
    WE_UCHAR **ppucBuf[OUT]:Pointer to the buffer.
    WE_INT32 *piBufLen[OUT]:Length of the buffer.
    WE_UINT8 *pucConnType[OUT]:the type of connection.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeSSLGetWMasterSec
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1; 

    if (NULL == piMasterSecId)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtShowDlgAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecW_EvtShowDlgAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_VOID **ppvData[OUT]:Pointer of data.
    WE_UINT32 *puiLen[OUT]:length.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtShowDlgAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvData,
    WE_UINT32 *puiLen
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint32(pstDecoder,puiLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    *ppvData = (WE_VOID*)WE_MALLOC(*puiLen);
    if (NULL == *ppvData)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder, (WE_CHAR*)*ppvData,*puiLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppvData);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;       
}
/*browser*/
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetCertNameList
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GetCertNameList.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piCertType[OUT]:the type of certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeGetCertNameList
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piCertType
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piCertType);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGenKeyPair
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GenKeyPair.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piCertType[OUT]:the type of certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeGenKeyPair
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_UINT8 *pucKeyType    
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    
    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucKeyType);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    Sec_DecodePinModify
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_PinModify.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piCertType[OUT]:the type of key.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodePinModify
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_UINT8 *pucKeyType    
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint8(pstDecoder, pucKeyType);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;

}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtChangePinAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtChangePinAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ChangePinAction *pstChangePin[OUT]:Pointer to the change pin.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtChangePinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    St_ChangePinAction *pstChangePin
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iLenOld = 0;
    WE_INT32 iLenNew = 0;
    WE_INT32 iLenAgain = 0;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, &iLenOld);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    if (iLenOld > 1)
    {   
        pstChangePin->pcOldPinValue = (WE_CHAR*)WE_MALLOC(iLenOld);
        if (NULL == pstChangePin->pcOldPinValue)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;            
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder, pstChangePin->pcOldPinValue, iLenOld);
        if (iRet < 0)
        {
            WE_FREE(pstChangePin->pcOldPinValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }        
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, &iLenNew);
    if (iRet < 0)
    {
        WE_FREE(pstChangePin->pcOldPinValue);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    if (iLenNew > 1)
    {
        pstChangePin->pcNewPinValue = (WE_CHAR*)WE_MALLOC(iLenNew);
        if (NULL == pstChangePin->pcNewPinValue)
        {
            WE_FREE(pstChangePin->pcOldPinValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;            
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder, pstChangePin->pcNewPinValue, iLenNew);
        if (iRet < 0)
        {
            WE_FREE(pstChangePin->pcOldPinValue);
            WE_FREE(pstChangePin->pcNewPinValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1; 

        }
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder,&iLenAgain);
    if (iRet < 0)
    {
        WE_FREE(pstChangePin->pcOldPinValue);
        WE_FREE(pstChangePin->pcNewPinValue);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1; 

    }    
    if (iLenAgain > 1)
    {
        pstChangePin->pcNewPinAgainValue = (WE_CHAR*)WE_MALLOC(iLenAgain);
        if (NULL == pstChangePin->pcNewPinAgainValue)
        {
            WE_FREE(pstChangePin->pcOldPinValue);
            WE_FREE(pstChangePin->pcNewPinValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;             
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder, pstChangePin->pcNewPinAgainValue, iLenAgain);
        if (iRet < 0)
        {
            WE_FREE(pstChangePin->pcOldPinValue);
            WE_FREE(pstChangePin->pcNewPinValue);
            WE_FREE(pstChangePin->pcNewPinAgainValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;    
        }        
    }
    iRet = SecEcdr_DecodeInt8(pstDecoder,pstChangePin->bIsOk);
    if (iRet < 0)
    {
        WE_FREE(pstChangePin->pcOldPinValue);
        WE_FREE(pstChangePin->pcNewPinValue);
        WE_FREE(pstChangePin->pcNewPinAgainValue);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1; 

    }
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}  
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtCreatePinAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtChangePinAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ChangePinAction *pstChangePin[OUT]:Pointer to the gen pin.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtCreatePinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    St_CreatePinAction *pstGenPin
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iPinValueLen = 0;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    
    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, &iPinValueLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    if (iPinValueLen > 1)
    {
        pstGenPin->pcPinValue = (WE_CHAR*)WE_MALLOC(iPinValueLen);
        if (NULL == pstGenPin->pcPinValue)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,pstGenPin->pcPinValue,iPinValueLen);
        if (iRet < 0)
        {
            WE_FREE(pstGenPin->pcPinValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
    }
    iRet = SecEcdr_DecodeInt8(pstDecoder, &pstGenPin->bIsOk);
    if (iRet < 0)
    {
        WE_FREE(pstGenPin->pcPinValue);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
} 
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtPinAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtPinAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_PinAction *pstPin[OUT]:Pointer to structure of pin action.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtPinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    St_PinAction *pstPin
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iPinValueLen = 0;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }
    
    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, &iPinValueLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    if (iPinValueLen > 1)
    {
        pstPin->pcPinValue = (WE_CHAR*)WE_MALLOC(iPinValueLen);
        if (NULL == pstPin->pcPinValue)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,pstPin->pcPinValue,iPinValueLen);
        if (iRet < 0)
        {
            WE_FREE(pstPin->pcPinValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
    }
    iRet = SecEcdr_DecodeInt8(pstDecoder, &pstPin->bIsOk);
    if (iRet < 0)
    {
        WE_FREE(pstPin->pcPinValue);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}  
#endif
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetCurSvrCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GetCurSvrCert.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piSecId[OUT]:ID of the security.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeGetCurSvrCert
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    WE_INT32 *piSecId
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetSessionInfo
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GetSessionInfo.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piSecId[OUT]:ID of the security.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeGetSessionInfo
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    WE_INT32 *piSecId
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetWtlsCurClasss
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GetWtlsCurClasss.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piSecId[OUT]:ID of the security.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeGetWtlsCurClasss
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piSecId
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeInt32(pstDecoder, piSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetContractsList
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GetContractsList.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeGetContractsList
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID
)
{
     St_SecEcdrDecoder *pstDecoder = NULL;
     WE_INT32 iRet = -1;
     
     pstDecoder = Sec_GetMsgDecode(pvMsg);
     if (NULL == pstDecoder)
     {
         return -1;
     }
    
     iRet = SecEcdr_DecodeInt32(pstDecoder, piTargetID);
     if (iRet < 0)
     {
         SecEcdr_DestroyDecoder(pstDecoder);
         return -1;
     }
     
     SecEcdr_DestroyDecoder(pstDecoder);
     return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeTransferCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GetContractsList.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    St_SecCertContent *pstCertContent[OUT]:pointer to the certificate content.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeTransferCert
(    
    WE_VOID *pvMsg, 
    St_SecCertContent *pstCertContent
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iMimeLen = 0;
    WE_INT32 iRet = -1;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeUint32(pstDecoder, &pstCertContent->uiCertLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    if (pstCertContent->uiCertLen > 0 )
    {
        pstCertContent->pucCertData = (WE_UCHAR*)WE_MALLOC(pstCertContent->uiCertLen);
        if (NULL == pstCertContent->pucCertData)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;            
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,pstCertContent->pucCertData,pstCertContent->uiCertLen);
        if (iRet < 0)
        {
            WE_FREE(pstCertContent->pucCertData);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
    }
    iRet = SecEcdr_DecodeUint32(pstDecoder, &iMimeLen);
    if (iRet < 0)
    {
        WE_FREE(pstCertContent->pucCertData);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    if (iMimeLen > 1)
    {
        pstCertContent->pcMime = (WE_CHAR*)WE_MALLOC(iMimeLen);
        if (NULL == pstCertContent->pcMime)
        {
            WE_FREE(pstCertContent->pucCertData);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;            
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,pstCertContent->pcMime,iMimeLen);
        if (iRet < 0)
        {
            WE_FREE(pstCertContent->pucCertData);
            WE_FREE(pstCertContent->pcMime);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
    }    
    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetRequestUserCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_GetRequestUserCert.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    St_SecUserCertReqInfo *pstShowContractContent[OUT]:pointer to the contract content.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeGetRequestUserCert
(   
    WE_VOID *pvMsg, 
    St_SecUserCertReqInfo *pstShowContractContent
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder, &pstShowContractContent->iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint16(pstDecoder, &pstShowContractContent->usCountryLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }

    pstShowContractContent->pucCountry = (WE_UCHAR*)WE_MALLOC(pstShowContractContent->usCountryLen);
    if (NULL == pstShowContractContent->pucCountry)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder, pstShowContractContent->pucCountry, pstShowContractContent->usCountryLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }
    iRet = SecEcdr_DecodeUint16(pstDecoder, &pstShowContractContent->usProvinceLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    pstShowContractContent->pucProvince = (WE_UCHAR*)WE_MALLOC(pstShowContractContent->usProvinceLen);
    if (NULL == pstShowContractContent->pucProvince)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeOctets(pstDecoder, pstShowContractContent->pucProvince, pstShowContractContent->usProvinceLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }    
    iRet = SecEcdr_DecodeUint16(pstDecoder, &pstShowContractContent->usCityLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  

    pstShowContractContent->pucCity = (WE_UCHAR*)WE_MALLOC(pstShowContractContent->usCityLen);
    if (NULL == pstShowContractContent->pucCity)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeOctets(pstDecoder, pstShowContractContent->pucCity, pstShowContractContent->usCityLen); 
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeUint16(pstDecoder, &pstShowContractContent->usCompanyLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    pstShowContractContent->pucCompany = (WE_UCHAR*)WE_MALLOC(pstShowContractContent->usCompanyLen);
    if (NULL == pstShowContractContent->pucCompany)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeOctets(pstDecoder, pstShowContractContent->pucCompany, pstShowContractContent->usCompanyLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeUint16(pstDecoder, &pstShowContractContent->usDepartmentLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    pstShowContractContent->pucDepartment = (WE_UCHAR*)WE_MALLOC(pstShowContractContent->usDepartmentLen);
    if (NULL == pstShowContractContent->pucDepartment)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeOctets(pstDecoder, pstShowContractContent->pucDepartment, pstShowContractContent->usDepartmentLen); 
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeUint16(pstDecoder, &pstShowContractContent->usNameLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    pstShowContractContent->pucName = (WE_UCHAR*)WE_MALLOC(pstShowContractContent->usNameLen);
    if (NULL == pstShowContractContent->pucName)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeOctets(pstDecoder, pstShowContractContent->pucName, pstShowContractContent->usNameLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        WE_FREE(pstShowContractContent->pucName);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeUint16(pstDecoder, &pstShowContractContent->usEMailLen);
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        WE_FREE(pstShowContractContent->pucName);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    pstShowContractContent->pucEMail = (WE_UCHAR*)WE_MALLOC(pstShowContractContent->usEMailLen);
    if (NULL == pstShowContractContent->pucEMail)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        WE_FREE(pstShowContractContent->pucName);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeOctets(pstDecoder, pstShowContractContent->pucEMail, pstShowContractContent->usEMailLen); 
    if (iRet < 0)
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        WE_FREE(pstShowContractContent->pucName); 
        WE_FREE(pstShowContractContent->pucEMail);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    } 
    if ( (SecEcdr_DecodeUint8(pstDecoder,&pstShowContractContent->ucCertUsage) < 0)
        || (SecEcdr_DecodeUint8(pstDecoder,&pstShowContractContent->ucKeyUsage) < 0)
        || (SecEcdr_DecodeUint8(pstDecoder,&pstShowContractContent->ucSignType)))
    {
        WE_FREE(pstShowContractContent->pucCountry);
        WE_FREE(pstShowContractContent->pucProvince);
        WE_FREE(pstShowContractContent->pucCity);
        WE_FREE(pstShowContractContent->pucCompany);
        WE_FREE(pstShowContractContent->pucDepartment);
        WE_FREE(pstShowContractContent->pucName); 
        WE_FREE(pstShowContractContent->pucEMail);
        SecEcdr_DestroyDecoder(pstDecoder);    
        return -1;
    }

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtConfirmAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtConfirmAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ConfirmAction *pstConfirm[OUT]:Pointer to structure of the confirm action.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtConfirmAction
(
   WE_VOID *pvMsg,
   WE_INT32 *piTargetID,
   St_ConfirmAction *pstConfirm
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstConfirm->bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtNameConfirmAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtNameConfirmAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ConfirmAction *pstConfirm[OUT]:Pointer to structure of the Name confirm .
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtNameConfirmAction
(   
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_NameConfirmAction *pstNameConfirm
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeUint8(pstDecoder,&pstNameConfirm->bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtHashAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtHashAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_HashAction *pstHash[OUT]:Pointer to structure of hash action .
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtHashAction
(   
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_HashAction *pstHash
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iTextLen = 0;
    WE_INT32 iRet = 0;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeInt32(pstDecoder,&iTextLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    if (iTextLen > 1)
    {
        pstHash->pcHashValue = (WE_CHAR*)WE_MALLOC(iTextLen);
        if (NULL == pstHash->pcHashValue)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,pstHash->pcHashValue,iTextLen);
        if (iRet < 0)
        {
            WE_FREE(pstHash->pcHashValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
    }
    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstHash->bIsOk);
    if ( -1 == iRet)
    {
        WE_FREE(pstHash->pcHashValue);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtShowCertContentAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtShowCertContentAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ShowCertContentAction *pstShowCertContent[OUT]:Pointer to structure of Show Cert Content Action .
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtShowCertContentAction
(    
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_ShowCertContentAction *pstShowCertContent
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstShowCertContent->bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtShowCertListAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtShowCertListAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ShowCertListAction *pstShowCertList[OUT]:Pointer to structure of Show Cert list Action .
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtShowCertListAction
(    
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_ShowCertListAction *pstShowCertList
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,&pstShowCertList->iSelId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeInt32(pstDecoder,&(pstShowCertList->eRKeyType));
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    } 
    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstShowCertList->bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtShowContractsListAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtShowContractsListAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ShowContractsListAction *pstShowContractsList[OUT]:Pointer to structure of Show contract list Action .
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtShowContractsListAction
(    
    WE_VOID *pvMsg,
    WE_INT32 piTargetID,
    St_ShowContractsListAction *pstShowContractsList
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;
    
    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,&pstShowContractsList->iSelId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     

    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstShowContractsList->bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtStoreCertAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtStoreCertAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_StoreCertAction *pstStoreCert[OUT]:Pointer to structure of store certificate Action .
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtStoreCertAction
(    
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_StoreCertAction *pstStoreCert
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iTextLen = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    iRet = SecEcdr_DecodeInt32(pstDecoder,&iTextLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }   
    if (iTextLen > 1)
    {
        pstStoreCert->pcText = (WE_CHAR*)WE_MALLOC(iTextLen);
        if (NULL == pstStoreCert->pcText)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,pstStoreCert->pcText,iTextLen);
        if (iRet < 0)
        {
            WE_FREE(pstStoreCert->pcText);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
    }
    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstStoreCert->bIsOk);
    if ( -1 == iRet)
    {
        WE_FREE(pstStoreCert->pcText);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DeocdeEvtShowContractContentAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SecB_EvtShowContractContentAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_ShowContractContentAction *pstShowContractContent[OUT]:Pointer to structure of show contract content Action .
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DeocdeEvtShowContractContentAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_ShowContractContentAction *pstShowContractContent
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstShowContractContent->bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeHandle
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SignText_Handle.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piSignId[OUT]:ID of the sign.
    WE_CHAR  **ppcText[OUT]:Pointer to the text.
    WE_INT32 *piKeyIdType[OUT]:The type of the key id.
    WE_CHAR **ppcKeyId[OUT]:Pointer to the key id.
    WE_INT32 *piKeyIdLen[OUT]:Length of key id.
    WE_CHAR **ppcStringToSign[OUT]: Pointer to the data
    WE_INT32 *piStringToSignLen[OUT]:Length of data.
    WE_INT32 *piOptions[OUT]:The value of option.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeHandle
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piSignId, 
    WE_CHAR  **ppcText,
    WE_INT32 *piKeyIdType,
    WE_CHAR **ppcKeyId,
    WE_INT32 *piKeyIdLen,
    WE_CHAR **ppcStringToSign, 
    WE_INT32 *piStringToSignLen, 
    WE_INT32 *piOptions
 )
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iTextLen = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,piSignId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,&iTextLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    if (iTextLen > 1)
    {
        *ppcText = (WE_CHAR*)WE_MALLOC(iTextLen);
        if (NULL == *ppcText)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;            
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,*ppcText,iTextLen);
        if ( -1 == iRet)
        {
            WE_FREE(*ppcText);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }     
    }
 
    iRet = SecEcdr_DecodeInt32(pstDecoder,piKeyIdType);
    if ( -1 == iRet)
    {
        WE_FREE(*ppcText);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,piKeyIdLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppcText);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    *ppcKeyId = (WE_CHAR*)WE_MALLOC(*piKeyIdLen);
    if (NULL == *ppcKeyId)
    {
        WE_FREE(*ppcText);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;        
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppcKeyId,*piKeyIdLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppcText);
        WE_FREE(*ppcKeyId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,piStringToSignLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppcText);
        WE_FREE(*ppcKeyId);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }  
    *ppcStringToSign = (WE_CHAR*)WE_MALLOC(*piStringToSignLen);
    if (NULL == *ppcStringToSign)
    {
        WE_FREE(*ppcText);
        WE_FREE(*ppcKeyId);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;         
    }
    iRet = SecEcdr_DecodeOctets(pstDecoder,*ppcStringToSign,*piStringToSignLen);
    if ( -1 == iRet)
    {
        WE_FREE(*ppcText);
        WE_FREE(*ppcKeyId);
        WE_FREE(*ppcStringToSign);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder, piOptions);
    if ( -1 == iRet)
    {
        WE_FREE(*ppcText);
        WE_FREE(*ppcKeyId);
        WE_FREE(*ppcStringToSign);        
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;
    
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeEvtSelectCertAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of Sec_DecodeEvtSelectCertAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_SelectCertAction *pstSelectCert[OUT]:Pointer to the structure of select certificate action.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_DecodeEvtSelectCertAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    St_SelectCertAction *pstSelectCert
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,&pstSelectCert->iSelId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     

    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstSelectCert->bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    SecS_DecodeEvtPinAction
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data,get the parameter of SignText_EvtPinAction.
ARGUMENTS PASSED:
    WE_VOID   *pvMsg[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    St_PinAction *pstPin[OUT]:Pointer to the structure of pin.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 SecS_DecodeEvtPinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    St_PinAction *pstPin
)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32 iPinValueLen = 0;
    WE_INT32 iRet = 0;

    pstDecoder = Sec_GetMsgDecode(pvMsg);
    if (NULL == pstDecoder)
    {
        return -1;
    }

    iRet = SecEcdr_DecodeInt32(pstDecoder,piTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      
    iRet = SecEcdr_DecodeInt32(pstDecoder,&iPinValueLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }     
    if (iPinValueLen > 1)
    {
        pstPin->pcPinValue = (WE_CHAR*)WE_MALLOC(iPinValueLen);
        if (NULL == pstPin->pcPinValue)
        {
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }
        iRet = SecEcdr_DecodeOctets(pstDecoder,pstPin->pcPinValue,iPinValueLen);
        if ( -1 == iRet)
        {
            WE_FREE(pstPin->pcPinValue);
            SecEcdr_DestroyDecoder(pstDecoder);
            return -1;
        }    
    }
    iRet = SecEcdr_DecodeUint8(pstDecoder,&pstPin->bIsOk);
    if ( -1 == iRet)
    {
        WE_FREE(pstPin->pcPinValue);
        SecEcdr_DestroyDecoder(pstDecoder);
        return -1;
    }      

    SecEcdr_DestroyDecoder(pstDecoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_GetMsgDecode
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    decode the encoded data.
ARGUMENTS PASSED:
    WE_VOID   *pvBuf[IN]:encoded data.
RETURN VALUE:
    pstDecodr: success.
    NULL: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

static St_SecEcdrDecoder* Sec_GetMsgDecode(WE_VOID* pvBuf)
{
    St_SecEcdrDecoder* pstDecodrTmp = NULL;
    St_SecEcdrDecoder *pstDecodr = NULL;
    WE_INT32 iMsgLen = 0;
    WE_INT32 iRet = 0;

    if (NULL == pvBuf)
    {
        return NULL;
    }

    pstDecodrTmp = SecEcdr_CreateDecoder((WE_UCHAR*)pvBuf, SEC_ED_MSG_HEADER_LENGTH);
    if (NULL == pstDecodrTmp)
    {
        return NULL;
    }
    iRet = SecEcdr_ChangeDecodePosition(pstDecodrTmp,SEC_ED_MSG_TYPE_LENGTH);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecodrTmp);
        return NULL;
    }
    iRet = SecEcdr_DecodeInt32(pstDecodrTmp,&iMsgLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecodrTmp);
        return NULL;
    }    
    SecEcdr_DestroyDecoder(pstDecodrTmp);

    pstDecodr = SecEcdr_CreateDecoder((WE_UCHAR*)pvBuf + SEC_ED_MSG_HEADER_LENGTH, iMsgLen);
    if (NULL == pstDecodr)
    {
        return NULL;
    }
    return pstDecodr;
}

/*************************************************************************************************
end
*************************************************************************************************/



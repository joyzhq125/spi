/*==================================================================================================
    FILE NAME : sec_wtlsparser.c
    MODULE NAME : SEC


    GENERAL DESCRIPTION
        The functions in this file process the wtls certificate.
        You can get the field, such as issuer, subject and so on, from a wtls certificate
        by using the supporting functions in this file.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History

    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-07-08        wuxl         None         Init

==================================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "sec_comm.h"


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
void Sec_WtlsGetIssuerAttributesType(const WE_UINT8 *pucAttribute, WE_UINT32 *puiAttributeType, 
                                          WE_UINT16 usAttributeLen);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_WtlsGetIssuerAttributesType
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the issuer attribute type.
ARGUMENTS PASSED:
    const WE_UINT8 *pucAttribute [IN]: Pointer to attribute in buffer.
    WE_UINT32 *puiAttributeType  [OUT]: Place to attribute type.
    WE_UINT16 usAttributeLen     [IN]: Length of Attribute. The Value must 1 or 2, others no sense.
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WtlsGetIssuerAttributesType(const WE_UINT8 *pucAttribute, WE_UINT32 *puiAttributeType, 
                                     WE_UINT16 usAttributeLen)
{
    if (NULL == pucAttribute)
    {
        return;
    }

    if (NULL == puiAttributeType)
    {
        return;
    }
    
    *puiAttributeType = M_SEC_UNKNOWN_ATTRIBUTE_TYPE;
    
    if (1 == usAttributeLen)
    {
        if ('L' == pucAttribute[0])
        {
            *puiAttributeType = M_SEC_LOCALITY_NAME;
        }
        if ('T' == pucAttribute[0])
        {
            *puiAttributeType = M_SEC_TITLE;
        }
        if ('S' == pucAttribute[0])
        {
            *puiAttributeType = M_SEC_STATE_OR_PROVINCE_NAME;
        }
    }
    
    if (2 == usAttributeLen)
    {
        if (('S' == pucAttribute[0]) && ('T' == pucAttribute[1]))
        {
            *puiAttributeType = M_SEC_STATE_OR_PROVINCE_NAME;
        }
        if (('S' == pucAttribute[0]) && ('N' == pucAttribute[1]))
        {
            *puiAttributeType = M_SEC_SERIAL_NUMBER;
        }
        if (('D' == pucAttribute[0]) && ('C' == pucAttribute[1]))
        {
            *puiAttributeType = M_SEC_DOMAIN_COMPONENT;
        }
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_WtlsGetCertFieldLength
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Check the certificate format whether wtls or not. When the value of *pusCertLen not zero, and 
    then will check the certificate length. If the the calculate length is not equal to *pusCertLen,
    the return is M_SEC_ERR_DATA_LENGTH.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle        [IN]: Global data handle.
    const WE_UCHAR *pucWtlsCert [IN]: Pointer to wtls certificate.
    WE_UINT16 *pusCertLen       [IN/OUT]: Place to length of certificate. If want to check, give a value
        to *pusCertLen(must not zero).
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE: Certificate not surpport.
    M_SEC_ERR_DATA_LENGTH: TThe data length not matching.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_WtlsGetCertFieldLength(WE_HANDLE hSecHandle, 
                                const WE_UCHAR *pucWtlsCert, WE_UINT16 *pusCertLen)
{
    WE_UCHAR *pucPtr         = NULL;
    WE_UINT8  ucIssuerLen    = 0;
    WE_UINT8  ucSubjectLen   = 0;
    WE_UINT16 usExponentLen  = 0;
    WE_UINT16 usModLen       = 0;
    WE_UINT16 usSignatureLen = 0;
    WE_UINT16 usLen          = 0;

    if (!hSecHandle || !pucWtlsCert)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (NULL == pusCertLen)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    /* Version of the certificate. 
       For WAP-261-WTLS-20010406 specification, the version is 1. 
    */
    pucPtr = (WE_UCHAR *)pucWtlsCert;
    if (1 != *pucPtr)
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }
    pucPtr++;
    
    /*signature algorithm:
      0--anonymous
      1--ecdsa_sha
      2--rsa_sha
    */
    if (2 != *pucPtr)
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }
    pucPtr++;

    /*issuer identifier type:
       0--null
       1--text
       2--binary
       254--key_hash_sha
       255--x509_name
    */
    if (1 == *pucPtr)
    {
        pucPtr += 3;
        ucIssuerLen = *pucPtr;
        pucPtr++;
        usLen += 6;
    }
    else
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }
    pucPtr += ucIssuerLen;
    usLen = usLen + ucIssuerLen;
    pucPtr += 8;
    usLen += 8;

     /*subject identifier type:
       0--null
       1--text
       2--binary
       254--key_hash_sha
       255--x509_name
    */
    if (1 == *pucPtr)
    {
        pucPtr += 3;
        ucSubjectLen = *pucPtr;
        pucPtr++;
        usLen += 4;
    }
    else
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }
    pucPtr += ucSubjectLen;
    usLen = usLen + ucSubjectLen;

    /*public key type:
      2--rsa
      3--ecdh
      4--ecdsa
    */
    if (2 == *pucPtr)
    {
        pucPtr++;
        usLen++;
    }
    else
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }

    /*parameter index
      0 = not applicable, or specified elsewhere.
      1...254 = assigned number of a parameter set.
      255 = explicit parameters are present in the next field.
    */
    if (0 == *pucPtr)
    {
        pucPtr++;
        usLen++;
    }
    else
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }

    Sec_StoreStrUint8to16(pucPtr, &usExponentLen);
    pucPtr += 2 + usExponentLen;
    usLen = usLen + 2 + usExponentLen;
    Sec_StoreStrUint8to16(pucPtr, &usModLen);
    pucPtr += 2 + usModLen;
    usLen = usLen + 2 + usModLen;
    Sec_StoreStrUint8to16(pucPtr, &usSignatureLen);
    pucPtr += 2 + usSignatureLen;
    usLen = usLen + 2 + usSignatureLen;
    
    if (0 == *pusCertLen)
    {
        *pusCertLen = usLen;
    }
    else if (*pusCertLen != usLen)
    {
        *pusCertLen = usLen;
        return M_SEC_ERR_DATA_LENGTH;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_WtlsGetIssuerInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the attributes of the issuer.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle      [IN]: Global data handle.
    const WE_UCHAR *pucIssuer [IN]: Pointer to the DER encoded issuer in buffer.
    WE_UINT16 usIssuerLen     [IN]: Length of issuer.
    WE_UINT8 *pucNbrIssuerAttributes [OUT]: Place to number of issuer attributes.
    St_SecNameAttribute *pstIssuerA  [OUT]: Place to attribute information of issuer.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_WtlsGetIssuerInfo(WE_HANDLE hSecHandle, 
                                const WE_UCHAR *pucIssuer, WE_UINT16 usIssuerLen, 
                                WE_UINT8 *pucNbrIssuerAttributes, St_SecNameAttribute *pstIssuerA)
{
    WE_UCHAR  ucAttribute[2]  = {0};
    WE_UINT8  ucIsReady       = 0;
    WE_UINT16 usPos           = 0;
    WE_UINT16 usStartPos      = 0;
    WE_UINT16 usSize          = 0;
    WE_UINT16 usTmpSize       = 0;
    WE_INT32  iLoop           = 0;
    WE_INT32  iNbr            = 0;
    WE_UINT32 uiAttributeType = 0;
    WE_INT32  usIndex = 0;

    if (!hSecHandle || !pucIssuer || !pucNbrIssuerAttributes)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    *pucNbrIssuerAttributes = 0;

    for (iLoop = 0; iLoop < (usIssuerLen - 2); iLoop++)
    {
        if((';' == pucIssuer[iLoop]) && (' ' == pucIssuer[iLoop+1]) )
        {
            (*pucNbrIssuerAttributes)++;
        }
    }
    (*pucNbrIssuerAttributes)++;

    if (NULL == pstIssuerA)
    {
        return M_SEC_ERR_OK;
    }

    usStartPos = usPos;
    ucIsReady  = 0;
    while ((usPos < usIssuerLen) && (!ucIsReady))
    {
        if (((';' == pucIssuer[usPos]) && (' ' == pucIssuer[usPos+1])) || (usPos == usIssuerLen - 1))
        {
            if (usPos == usIssuerLen - 1)
            {
                usSize++;
            }
            pstIssuerA[iNbr].pucAttributeValue = (WE_UCHAR *)WE_MALLOC((usSize + 1) * sizeof(WE_UCHAR));
            if (NULL == pstIssuerA[iNbr].pucAttributeValue)
            {
                for (usIndex = 0; usIndex < iNbr; usIndex++)
                {
                    WE_FREE(pstIssuerA[usIndex].pucAttributeValue);
                    pstIssuerA[usIndex].pucAttributeValue = NULL;
                }
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            (WE_VOID)WE_MEMCPY(pstIssuerA[iNbr].pucAttributeValue, &pucIssuer[usStartPos], usSize);
            pstIssuerA[iNbr].pucAttributeValue[usSize] = '\0';
            pstIssuerA[iNbr].usAttributeValueLen = usSize;
            pstIssuerA[iNbr].sAttributeCharSet = 0;
            pstIssuerA[iNbr].uiAttributeType = M_SEC_ORGANISATIONAL_UNIT_NAME;
            iNbr++;
            ucIsReady = 1;
            usPos++;
        }
        usPos++;
        usSize++;
    }

    if (ucIsReady)
    {
        usStartPos = usPos;
        usSize     = 0;
        ucIsReady  = 0;
        while ((usPos < usIssuerLen) && (!ucIsReady))
        {
            if (((';' == pucIssuer[usPos]) && (' ' == pucIssuer[usPos+1])) || (usPos == usIssuerLen - 1))
            {
                if (usPos == usIssuerLen - 1)
                {
                    usSize++;
                }
                
                pstIssuerA[iNbr].pucAttributeValue = (WE_UCHAR *)WE_MALLOC((usSize + 1) * sizeof(WE_UCHAR));
                if (NULL == pstIssuerA[iNbr].pucAttributeValue)
                {
                    for (usIndex = 0; usIndex < iNbr; usIndex++)
                    {
                        WE_FREE(pstIssuerA[usIndex].pucAttributeValue);
                        pstIssuerA[usIndex].pucAttributeValue = NULL;
                    }
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (WE_VOID)WE_MEMCPY(pstIssuerA[iNbr].pucAttributeValue, &pucIssuer[usStartPos], usSize);
                pstIssuerA[iNbr].pucAttributeValue[usSize] = '\0';
                pstIssuerA[iNbr].usAttributeValueLen = usSize;
                pstIssuerA[iNbr].sAttributeCharSet = 0;
                pstIssuerA[iNbr].uiAttributeType = M_SEC_ORGANISATION_NAME;
                iNbr++;
                ucIsReady = 1;
                usPos++;
            }
            
            usPos++;
            usSize++;
        }
    }

    if (ucIsReady)
    {
        usStartPos = usPos;
        usSize     = 0;
        ucIsReady  = 0;
        while ((usPos < usIssuerLen) && (!ucIsReady))
        {
            if (((';' == pucIssuer[usPos]) && (' ' == pucIssuer[usPos+1])) || (usPos == usIssuerLen - 1))
            {
                if (usPos == usIssuerLen - 1)
                {
                    usSize++;
                }
                
                pstIssuerA[iNbr].pucAttributeValue = (WE_UCHAR *)WE_MALLOC((usSize + 1) * sizeof(WE_UCHAR));
                if (NULL == pstIssuerA[iNbr].pucAttributeValue)
                {
                    for (usIndex = 0; usIndex < iNbr; usIndex++)
                    {
                        WE_FREE(pstIssuerA[usIndex].pucAttributeValue);
                        pstIssuerA[usIndex].pucAttributeValue = NULL;
                    }
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (WE_VOID)WE_MEMCPY(pstIssuerA[iNbr].pucAttributeValue, &pucIssuer[usStartPos], usSize);
                pstIssuerA[iNbr].pucAttributeValue[usSize] = '\0';
                pstIssuerA[iNbr].usAttributeValueLen = usSize;
                pstIssuerA[iNbr].sAttributeCharSet = 0;
                pstIssuerA[iNbr].uiAttributeType = M_SEC_COUNTRY_NAME;
                iNbr++;
                ucIsReady = 1;
                usPos++;
            }
            usPos++;
            usSize++;
        }
    }

    if (ucIsReady)
    {
        usStartPos = usPos;
        usSize = 0;
        ucIsReady = 0;
        if (('=' == pucIssuer[usPos+1]) || ('=' == pucIssuer[usPos+2]))
        {
            if ('=' == pucIssuer[usPos+1])
            {
                usTmpSize = 1;
            }            
            if ('=' == pucIssuer[usPos+2])
            {
                usTmpSize = 2;
            }
            
            (WE_VOID)WE_MEMCPY(ucAttribute, &pucIssuer[usStartPos], usTmpSize);
    
            Sec_WtlsGetIssuerAttributesType(ucAttribute, &uiAttributeType, usTmpSize);
        }
        else
        {
            uiAttributeType = M_SEC_COMMON_NAME;
        }
        
        while (usPos < usIssuerLen)
        {
            if (((';' == pucIssuer[usPos]) && (' ' == pucIssuer[usPos+1])) || (usPos == usIssuerLen-1))
            { 
                if (usPos == usIssuerLen - 1)
                {
                    usSize++;
                }
                
                pstIssuerA[iNbr].pucAttributeValue = (WE_UCHAR *)WE_MALLOC((usSize + 1) * sizeof(WE_UCHAR));
                if (NULL == pstIssuerA[iNbr].pucAttributeValue)
                {
                    for (usIndex = 0; usIndex < iNbr; usIndex++)
                    {
                        WE_FREE(pstIssuerA[usIndex].pucAttributeValue);
                        pstIssuerA[usIndex].pucAttributeValue = NULL;
                    }
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (WE_VOID)WE_MEMCPY(pstIssuerA[iNbr].pucAttributeValue, &pucIssuer[usStartPos], usSize);
                pstIssuerA[iNbr].pucAttributeValue[usSize] = '\0';
                pstIssuerA[iNbr].usAttributeValueLen = usSize;
                pstIssuerA[iNbr].sAttributeCharSet = 0;
                pstIssuerA[iNbr].uiAttributeType = uiAttributeType;
                iNbr++;
                usPos++;
                ucIsReady = 1;
            }
            
            usPos++;
            if(ucIsReady)
            {
                usSize     = 0;
                usStartPos = usPos;
                ucIsReady  = 0;
                if (('=' == pucIssuer[usPos+1]) || ('=' == pucIssuer[usPos+2]))
                {
                    if ('=' == pucIssuer[usPos+1])
                    {
                        usTmpSize = 1;
                    }
                    if ('=' == pucIssuer[usPos+2])
                    {
                        usTmpSize = 2;
                    }
                    
                    (WE_VOID)WE_MEMCPY(ucAttribute, &pucIssuer[usStartPos], usTmpSize);
              
                    Sec_WtlsGetIssuerAttributesType(ucAttribute, &uiAttributeType, usTmpSize);
                }
                else
                {
                    uiAttributeType = M_SEC_COMMON_NAME;
                }
            }
            else
            {
                usSize++;
            }
        }
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_WtlsGetSubjectInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the subject of a wtls certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle            [IN]: Global data handle.
    const WE_UCHAR *pucWtlsCert     [IN]: Pointer to wtls certificate.
    WE_UINT8 *pucSubjectIdentifierType[OUT]: Place to identifier of subject.
    WE_UINT16 *pusSubjectCharacterSet [OUT]: Place to character set of subject.
    WE_UINT8 *pucSubjectLen           [OUT]: Place to length of subject.
    WE_UCHAR **ppucSubject            [OUT]: Place to put pointer to subject value.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_WtlsGetSubjectInfo(WE_HANDLE hSecHandle, const WE_UCHAR *pucWtlsCert,
                                WE_UINT8 *pucSubjectIdentifierType, WE_UINT16 *pusSubjectCharacterSet,
                                WE_UINT8 *pucSubjectLen, WE_UCHAR **ppucSubject)
{
    WE_UINT8  ucIssuerLen = 0;
    WE_UCHAR *pucPtr      = NULL;

    if (!hSecHandle || !pucWtlsCert || !pucSubjectIdentifierType || !pusSubjectCharacterSet || !pucSubjectLen || !ppucSubject)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucPtr = (WE_UCHAR *)pucWtlsCert;
    pucPtr += 5;
    ucIssuerLen = *pucPtr;
    pucPtr++;
    pucPtr += ucIssuerLen + 8;
    *pucSubjectIdentifierType = *pucPtr;
    pucPtr++;
    Sec_StoreStrUint8to16(pucPtr, pusSubjectCharacterSet);
    pucPtr += 2;
    *pucSubjectLen = *pucPtr;
    pucPtr++;
    *ppucSubject = pucPtr;
    
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_WtlsCreateCertificate
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Compose a wtls cetificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle  [IN]: Global data handle.
    WE_UCHAR **ppucCert   [OUT]: Place to put pointer to certificate.
    WE_UINT16 *pusCertLen [OUT]: Place to length of certificate.
    WE_UCHAR *pucPubkey   [IN]: Pointer to public key.
    WE_UINT16 usPubkeyLen [IN]: length of public key.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_WtlsCreateCertificate(WE_HANDLE hSecHandle, WE_UCHAR *pucCert, WE_UINT16 *pusCertLen, 
                                WE_UCHAR *pucPubkey, WE_UINT16 usPubkeyLen)
{
    WE_INT32  iLoop                 = 0;
    WE_UINT16 usIndex               = 0;
    WE_UINT16 usExpLen              = 0;
    WE_UINT16 usModLen              = 0;
    WE_UINT16 usSignatureLen        = 128;
    WE_UINT16 usJumpStep            = 0;
    WE_UCHAR  ucIssuerAndSubject[7] = "\x0\x0\x0\x0\x0\x0";
    WE_UINT8  ucIssuerAndSubjectLen = 6;
    WE_UCHAR *pucPubMod             = NULL;
    WE_UCHAR  ucExpLenStr[2]        = {0};
    WE_UCHAR  ucModLenStr[2]        = {0};
    WE_UCHAR *pucTmpPubkey          = NULL;

    if (!pucCert || !pusCertLen)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if ((usPubkeyLen < 1) || (NULL == pucPubkey))
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    pucTmpPubkey = pucPubkey;
   
    pucCert[usIndex] = 0x01;
    usIndex++;
    pucCert[usIndex] = 0x02;
    usIndex++;
    pucCert[usIndex] = 0x01;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x04;
    usIndex++;
    pucCert[usIndex] = ucIssuerAndSubjectLen;
    usIndex++;
    for (iLoop = 0; iLoop < ucIssuerAndSubjectLen; iLoop++)
    {
        pucCert[usIndex+iLoop] = ucIssuerAndSubject[iLoop];
    }
    
    usIndex = usIndex + ucIssuerAndSubjectLen;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x01;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x04;
    usIndex++;
    pucCert[usIndex] = ucIssuerAndSubjectLen;
    usIndex++;
    for (iLoop = 0; iLoop < ucIssuerAndSubjectLen; iLoop++)
    {
        pucCert[usIndex+iLoop] = ucIssuerAndSubject[iLoop];
    }
    
    usIndex = usIndex + ucIssuerAndSubjectLen;
    pucCert[usIndex] = 0x02;
    usIndex++;
    pucCert[usIndex] = 0x00;
    usIndex++;

    pucTmpPubkey += 1;
    Sec_X509DerToSize(hSecHandle, pucTmpPubkey, &usJumpStep, &usModLen);
    pucTmpPubkey += usJumpStep;
    pucTmpPubkey += 1;
    Sec_X509DerToSize(hSecHandle, pucTmpPubkey, &usJumpStep, &usModLen);
    pucTmpPubkey += usJumpStep;
    if (0x00 == *pucTmpPubkey)
    {
        usModLen--;
        pucTmpPubkey++;
    }

    Sec_ExportStrUint16to8(&usModLen, ucModLenStr);
    pucPubMod = pucTmpPubkey;
    for (iLoop = 0; iLoop < usModLen; iLoop++)
    {
        pucTmpPubkey++;
    }
    pucTmpPubkey++;
    Sec_X509DerToSize(hSecHandle, pucTmpPubkey, &usJumpStep, &usExpLen);
    pucTmpPubkey += usJumpStep;
    Sec_ExportStrUint16to8(&usExpLen, ucExpLenStr);
    pucCert[usIndex] = ucExpLenStr[0];
    usIndex++;
    pucCert[usIndex] = ucExpLenStr[1];
    usIndex++;
    for (iLoop = 0; iLoop < usExpLen; iLoop++)
    {
        pucCert[usIndex+iLoop] = *pucTmpPubkey;
        pucTmpPubkey++;
    }
    
    usIndex = usIndex + usExpLen;
    pucCert[usIndex] = ucModLenStr[0];
    usIndex++;
    pucCert[usIndex] = ucModLenStr[1];
    usIndex++;
    for (iLoop = 0; iLoop < usModLen; iLoop++)
    {
        pucCert[usIndex+iLoop] = pucPubMod[iLoop];
    }
    
    usIndex = usIndex + usModLen;
    pucCert[usIndex] = 0x00;
    usIndex++;
    pucCert[usIndex] = 0x80;
    usIndex++;
    for (iLoop = 0; iLoop < usSignatureLen; iLoop++)
    {
        pucCert[usIndex+iLoop] = 0;
    }
    
    usIndex = usIndex + usSignatureLen;
    *pusCertLen = usIndex;
  
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_WtlsCertParse
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Parse a wtls certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle        [IN]: Global data handle.
    const WE_UCHAR *pucWtlsCert [IN]: Pointer to wtls certificate.
    WE_UINT16 *pusCertLen       [OUT]: Place to length of certificate.
    St_SecCertificate *pstCert  [OUT]: Place to parse information of certification.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_WtlsCertParse(WE_HANDLE hSecHandle, const WE_UCHAR *pucWtlsCert, 
                           WE_UINT16 *pusCertLen, St_SecCertificate *pstCert)
{
    WE_UCHAR *pucPtr = NULL;
    WE_INT32 iRes    = M_SEC_ERR_OK;

    if (NULL == pucWtlsCert)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    if (NULL == pusCertLen)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    if (NULL == pstCert)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRes = Sec_WtlsGetCertFieldLength(hSecHandle, pucWtlsCert, pusCertLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    pucPtr = (WE_UCHAR *)pucWtlsCert;
    
    /* version */
    (*pstCert).ucCertificateVersion = *pucPtr;
    pucPtr++;

    /* signature algorithm */
    (*pstCert).stCertUnion.stWtls.ucSignatureAlgorithm = *pucPtr;
    pucPtr++;

    /* issuer identifier type */
    (*pstCert).stCertUnion.stWtls.ucIssuerIdentifierType = *pucPtr;
    pucPtr++;

    /* issuer characterset */
    Sec_StoreStrUint8to16(pucPtr, &(*pstCert).stCertUnion.stWtls.usIssuercharacterSet);
    pucPtr += 2;

    /* issuer length */
    (*pstCert).stCertUnion.stWtls.ucIssuerLen = *pucPtr;
    pucPtr++;

    /* issuer content */
    (*pstCert).pucIssuer = pucPtr;
    pucPtr += pstCert->stCertUnion.stWtls.ucIssuerLen;

    /* Beginning of the validity period of the certificate.
       End of the validity period of the certificate
    */
    Sec_StoreStrUint8to32(pucPtr , &(*pstCert).iValidNotBefore);
    pucPtr += 4;
    Sec_StoreStrUint8to32(pucPtr , &(*pstCert).iValidNotAfter);
    pucPtr += 4;

    /* subject identifier type */
    (*pstCert).stCertUnion.stWtls.ucSubjectIdentifierType = *pucPtr;
    pucPtr++;

    /* subject characterset */
    Sec_StoreStrUint8to16(pucPtr, &(*pstCert).stCertUnion.stWtls.usSubjectcharacterSet);
    pucPtr += 2;

    /* subject length */
    (*pstCert).stCertUnion.stWtls.ucSubjectLen = *pucPtr;
    pucPtr++;

    /* subject content */
    (*pstCert).pucSubject = pucPtr;
    pucPtr += pstCert->stCertUnion.stWtls.ucSubjectLen;

    /* public key type */
    (*pstCert).stCertUnion.stWtls.ucPublicKeyType = *pucPtr;
    pucPtr++;

    /* parameter index */
    (*pstCert).stCertUnion.stWtls.ucParameter_index = *pucPtr;
    pucPtr++;

    /* RSA exponent length */
    Sec_StoreStrUint8to16(pucPtr, &(*pstCert).stCertUnion.stWtls.usExpLen);
    pucPtr += 2;

    /* RSA exponent content */
    (*pstCert).stCertUnion.stWtls.pucRsaExponent = pucPtr;
    pucPtr += pstCert->stCertUnion.stWtls.usExpLen;

    /* RSA modulus length */
    Sec_StoreStrUint8to16(pucPtr, &(*pstCert).stCertUnion.stWtls.usModLen);
    pucPtr += 2;

    /* RSA modulus content */
    (*pstCert).stCertUnion.stWtls.pucRsaModulus = pucPtr;
    pucPtr += pstCert->stCertUnion.stWtls.usModLen;

    /* signature length */
    Sec_StoreStrUint8to16(pucPtr, &(*pstCert).usSignatureLen);
    pucPtr += 2;

    /* signature content */
    (*pstCert).pucSignature = pucPtr;
    
    return M_SEC_ERR_OK;
}



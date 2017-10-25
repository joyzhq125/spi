/*=====================================================================================
    MODULE NAME : oem_secx509parser.c
    MODULE NAME : SEC


    GENERAL DESCRIPTION
        The functions in this file process the x.509 certificate.
        You can get the field, such as issuer, subject and so on, from a x.509 certificate
        by using the supporting functions in this file.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History

    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2007-02-03        tang         None         draf

=====================================================================================*/
/***************************************************************************************************
*   Include File Section
***************************************************************************************************/
#include "sec_comm.h"

/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_X509GetRsaModAndExp
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get the modulus and exponent of RSA key.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle           [IN]: Global data handle.
    const WE_UCHAR *pucPublicKeyVal[IN]: Pointer to the DER encoded public key value in buffer.
    WE_UCHAR **ppucRsaModulus      [OUT]: Place to put pointer to modulus of the RSA key
    WE_UINT16 *pusModLen           [OUT]: Place to length of the modulus
    WE_UCHAR **ppucRsaExponent     [OUT]: Place to put pointer to exponent of the RSA key.
    WE_UINT16 *pusExponentLen      [OUT]: Place to length of the exponent.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetRsaModAndExp(WE_HANDLE hSecHandle, const WE_UCHAR *pucPublicKeyVal, 
                                 WE_UCHAR **ppucRsaModulus, WE_UINT16 *pusModLen, 
                                 WE_UCHAR **ppucRsaExponent, WE_UINT16 *pusExponentLen)
{
    hSecHandle = hSecHandle;
    return We_X509GetRSAModAndExp(pucPublicKeyVal,ppucRsaModulus,pusModLen, \
        ppucRsaExponent,pusExponentLen);
}
/*==================================================================================================
FUNCTION: 
    Sec_X509CertParse
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Parse the x509 ASN.1 DER encode certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle        [IN]: Global data handle.
    const WE_UCHAR *pucX509Cert [IN]:  Pointer to the DER encoded certificate in buffer.
    WE_UINT16 *pusCertLength    [OUT]: Place to length of certificate.
    St_SecCertificate *pstCert  [OUT]: Place to parse information of certification.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509CertParse(WE_HANDLE hSecHandle, const WE_UCHAR *pucX509Cert, 
                           WE_UINT16 *pusCertLength, St_SecCertificate *pstCert)
{
    hSecHandle = hSecHandle;
    return We_X509Parse(pucX509Cert,pusCertLength,pstCert);
}
/*==================================================================================================
FUNCTION: 
    Sec_X509GetCertFieldLength
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get the length of certificate.When the value of *pusCertLen not zero, and then will check the certificate length. 
    If the the calculate length is not equal to *pusCertLen, the return is M_SEC_ERR_DATA_LENGTH.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle       [IN]: Global data handle.
    const WE_UCHAR *pucX509Cert[IN]: Pointer to the DER encoded certificate in buffer.
    WE_UINT16 *pusCertLen      [OUT]: Place to length of certificate.
    WE_UINT16 *pusJumpStep     [OUT]: Place to step of jump.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_DATA_LENGTH: The data length not matching.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetCertFieldLength(WE_HANDLE hSecHandle, const WE_UCHAR *pucX509Cert, 
                                    WE_UINT16 *pusCertLen, WE_UINT16 *pusJumpStep)
{
    hSecHandle = hSecHandle;
    return We_X509GetAndCheckTBSCertLength(pucX509Cert, pusCertLen, 0,pusJumpStep);
}
/*==================================================================================================
FUNCTION: 
    Sec_X509CertParseResponse
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Response for x509 certificate parse.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle        [IN]: Global data handle.
    const WE_UCHAR *pucCertResp [IN]: Pointer to the certificate response in buffer.
    WE_INT16 *psCharacterSet    [OUT]: Place to character set of display name.
    WE_VOID **ppvDisplayName    [OUT]: Place to put pointer to display name.
    WE_INT32 *piDisplayNameLen  [OUT]: Place to length of display name.
    WE_UCHAR **ppucCaKeyHash    [OUT]: Place to put pointer to ca key hash.
    WE_UCHAR **ppucSubjectKeyHash[OUT]: Place to put pointer to subject key hash.
    WE_UCHAR **ppucCert         [OUT]: Place to put pointer to certificate.
    WE_UINT16 *pusCertLen       [OUT]: Place to length of certificate.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_NOT_IMPLEMENTED: Not implemented.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509CertParseResponse(WE_HANDLE hSecHandle, 
                                   const WE_UCHAR *pucCertResp, WE_INT16 *psCharacterSet, 
                                   WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen, 
                                   WE_UCHAR **ppucCaKeyHash, WE_UCHAR **ppucSubjectKeyHash, 
                                   WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen)
{
    hSecHandle = hSecHandle;
    return  We_X509ParseCertResponse(pucCertResp,psCharacterSet, ppvDisplayName,\
        piDisplayNameLen,ppucCaKeyHash, ppucSubjectKeyHash, ppucCert, pusCertLen);
}
/*==================================================================================================
FUNCTION: 
    Sec_X509GetIssuerInfo
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get issuer attributes.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle            [IN]: Global data handle.
    const WE_UCHAR *pucIssuer       [IN]: Pointer to the DER encoded issuer in buffer.
    WE_UINT16 usIssuerLen           [IN]: Length of issuer.
    WE_UINT8 *pucNbrIssuerAttributes[OUT]: Place to number of issuer attributes.
    St_SecNameAttribute *pstIssuerA [OUT]: Place to attribute information of issuer.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetIssuerInfo(WE_HANDLE hSecHandle, 
                               const WE_UCHAR *pucIssuer, WE_UINT16 usIssuerLen, 
                               WE_UINT8 *pucNbrIssuerAttributes, St_SecNameAttribute *pstIssuerA)
{
    
    WE_UCHAR *pucPtr              = NULL;
    WE_UINT8 *pucAttributeValue   = NULL;
    WE_UINT8  ucNbrIssuerStrings  = 0;
    WE_UINT16 usSize              = 0;
    WE_UINT16 usTmpStep           = 0;
    WE_UINT16 usJumpStep          = 0;
    WE_UINT16 usAttributeValueLen = 0;
    WE_INT16  sAttributeCharSet   = 0;
    WE_UINT32 uiAttributeType     = 0;
    WE_INT32  iRes                = M_SEC_ERR_OK;
    WE_INT32  iLoop               = 0;
    WE_INT32  iLen                = 0;

    if ((NULL == pucNbrIssuerAttributes) || (NULL == pucIssuer) || (NULL == pstIssuerA))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRes = We_X509GetNbrIssuerStrings( pucIssuer, usIssuerLen, &ucNbrIssuerStrings);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    *pucNbrIssuerAttributes = ucNbrIssuerStrings;
    pucPtr = (WE_UCHAR *)pucIssuer;  
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pucPtr += usTmpStep;
    for (iLoop = 0; iLoop < ucNbrIssuerStrings; iLoop++)
    {
        iRes = We_X509GetAttributeString(pucPtr,(WE_VOID **)&pucAttributeValue, &usAttributeValueLen, 
                                &sAttributeCharSet, &uiAttributeType, &usJumpStep);
        if (M_SEC_ERR_OK != iRes)
        {
            for (iLen = 0; iLen < iLoop; iLen++)
            {
                WE_FREE(pstIssuerA[iLen].pucAttributeValue);
            }
            *pucNbrIssuerAttributes = 0;
            return iRes;
        }
        pucPtr += usJumpStep;
        pstIssuerA[iLoop].sAttributeCharSet = sAttributeCharSet;
        pstIssuerA[iLoop].uiAttributeType = uiAttributeType;
        pstIssuerA[iLoop].usAttributeValueLen = usAttributeValueLen;
        pstIssuerA[iLoop].pucAttributeValue = (WE_UCHAR *)WE_MALLOC((usAttributeValueLen + 2) * sizeof(WE_UCHAR));
        if(NULL==pstIssuerA[iLoop].pucAttributeValue)
        {
            for (iLen = 0; iLen < iLoop; iLen++)
            {
                WE_FREE(pstIssuerA[iLen].pucAttributeValue);
            }
            *pucNbrIssuerAttributes = 0;
            return M_SEC_ERR_INSUFFICIENT_MEMORY;            
        }
        (WE_VOID)WE_MEMCPY(pstIssuerA[iLoop].pucAttributeValue,pucAttributeValue,usAttributeValueLen);
        pstIssuerA[iLoop].pucAttributeValue[usAttributeValueLen] = '\0';
    }
    
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    Sec_X509DerToSize
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get the identifier size and the length of value.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle  [IN]: Global data handle.
    WE_UCHAR *pucBuf      [IN]: Pointer to the DER encoded data in buffer.
    WE_UINT16 *pusTmpStep [OUT]: Place to step of jump.
    WE_UINT16 *pusSize    [OUT]: Place to size of identifier.
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID Sec_X509DerToSize(WE_HANDLE hSecHandle, WE_UCHAR *pucCert, 
                          WE_UINT16 *pusTmpStep, WE_UINT16 *pusSize)
{
    hSecHandle = hSecHandle;
    We_X509DERtoSize(pucCert, pusTmpStep, pusSize);    
}
/*==================================================================================================
FUNCTION: 
    Sec_X509GetBasicConstraint
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get basic constraints
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle          [IN]: Global data handle.
    const WE_UCHAR *pucExtnValue  [IN]: Pointer to the DER encoded extension value in buffer.
    WE_INT32 *piCa                [OUT]: Place to whether a ca certificat.
    WE_INT32 *piHasPathLenConstraint[OUT]: Place to whether has the path length.
    WE_UINT8 *pucPathLenConstraint[OUT]: Place to the length of path.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetBasicConstraint(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtnValue, 
                                    WE_INT32 *piCa, WE_INT32 *piHasPathLenConstraint, 
                                    WE_UINT8 *pucPathLenConstraint)
{
    hSecHandle = hSecHandle;
    return We_X509ExtnBasicConstraints(pucExtnValue, piCa,piHasPathLenConstraint,\
        pucPathLenConstraint);
}
/*==================================================================================================
FUNCTION: 
    Sec_X509GetKeyUsage
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get key usage.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle         [IN]: Global data handle.
    const WE_UCHAR *pucExtnValue [IN]: Pointer to the DER encoded extension value in buffer.
    WE_UINT8 *pucKeyUsageBit0To7 [OUT]: Place to low bit of key usage.
    WE_UINT8 *pucKeyUsageBit8To15[OUT]: Place to high bit of key usage.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetKeyUsage(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtnValue, 
                             WE_UINT8 *pucKeyUsageBit0To7, WE_UINT8 *pucKeyUsageBit8To15)
{
    hSecHandle = hSecHandle;
    return  We_X509ExtnKeyUsage(pucExtnValue, pucKeyUsageBit0To7, pucKeyUsageBit8To15) ; 
}
/*==================================================================================================
FUNCTION: 
    Sec_X509GetKeyUsageEx
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get extkey usage.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle           [IN]: Global data handle.
    const WE_UCHAR *pucExtnValue   [IN]: Pointer to the DER encoded extension value in buffer.
    WE_INT32 *piAnyExtendedKeyUsage[OUT]: Place to .
    WE_INT32 *piServerAuth         [OUT]: Place to TLS Web server authentication.
    WE_INT32 *piClientAuth         [OUT]: Place to TLS Web client authentication.
    WE_INT32 *piCodeSigning        [OUT]: Place to Signing of downloadable executable code.
    WE_INT32 *piEmailProtection    [OUT]: Place to E-mail protection.
    WE_INT32 *piTimeStamping       [OUT]: Place to Binding the hash of an object to a time from an agreed-upon time source.
    WE_INT32 *piOcspSigning        [OUT]: Place to .
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetKeyUsageEx(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtnValue, 
                               WE_INT32 *piAnyExtendedKeyUsage, WE_INT32 *piServerAuth, 
                               WE_INT32 *piClientAuth, WE_INT32 *piCodeSigning, 
                               WE_INT32 *piEmailProtection, WE_INT32 *piTimeStamping,
                               WE_INT32 *piOcspSigning)
{
    hSecHandle = hSecHandle;
    return We_X509ExtnExtKeyUsage(pucExtnValue, piAnyExtendedKeyUsage,
        piServerAuth, piClientAuth, piCodeSigning,
        piEmailProtection, piTimeStamping, piOcspSigning) ;
}
/*==================================================================================================
FUNCTION: 
    Sec_X509GetCertExtension
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get extension information in the certificate's extension field.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle      [IN]: Global data handle.
    WE_UCHAR *pucExtensions   [IN]: Pointer to the DER encoded extension in buffer.
    WE_INT32 *piExtnId        [OUT]: ID of the extension.
    WE_INT32 *piCritical      [OUT]: Place to critical of extension.
    WE_UCHAR **ppucExtnValue  [OUT]: Place to put pointer to extension value.
    WE_UINT16 *pusExtnValueLen[OUT]: Place to length of extension value.
    WE_UINT16 *pusJumpStep    [OUT]: Place to step of jump.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetCertExtension(WE_HANDLE hSecHandle, WE_UCHAR *pucExtensions, 
                                  WE_INT32 *piExtnId, WE_INT32 *piCritical, 
                                  WE_UCHAR **ppucExtnValue, WE_UINT16 *pusExtnValueLen,
                                  WE_UINT16 *pusJumpStep)
{
    hSecHandle = hSecHandle;
    return We_X509GetExtension(pucExtensions, piExtnId, piCritical, ppucExtnValue, pusExtnValueLen,
        pusJumpStep);
}
/*==================================================================================================
FUNCTION: 
    M_SEC_ERR_INVALID_PARAMETER
CREATE DATE:
    2007-02-05
AUTHOR:
    Tang
DESCRIPTION:
    Get the number of issuer strings.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle         [IN]: Global data handle.
    const WE_UCHAR *pucIssuer    [IN]: Pointer to the DER encoded issuer in buffer.
    WE_UINT16 usIssuerLen        [IN]: Length of issuer.
    WE_UINT8 *pucNbrIssuerStrings[OUT]: Place to number of issuer strings
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509GetNbrIssuerStr(WE_HANDLE hSecHandle, const WE_UCHAR *pucIssuer, 
                                 WE_UINT16 usIssuerLen, WE_UINT8 *pucNbrIssuerStrings)
{    
    WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usSize    = 0;
    WE_UINT16 usLength  = 0;
    WE_UINT16 usTmpStep = 0;
    
    if ((NULL == hSecHandle) || (NULL == pucIssuer) || (NULL == pucNbrIssuerStrings))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucPtr = (WE_UCHAR *)pucIssuer;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);                
    usLength = usIssuerLen - (usTmpStep + 1); 
    pucPtr += usTmpStep;
    
    *pucNbrIssuerStrings = 0;
    while (usLength > 0)
    {
        if (WE_ASN1_SET_TYPE != *pucPtr)
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
        
        pucPtr++;
        Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);            
        pucPtr += usTmpStep + usSize;
        usLength = usLength - (usSize + usTmpStep + 1);    
        *pucNbrIssuerStrings += 1;
    }
    
    return M_SEC_ERR_OK;
}


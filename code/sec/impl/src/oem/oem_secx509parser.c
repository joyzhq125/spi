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
    2006-07-08        wuxl         None         Init

=====================================================================================*/

/***************************************************************************************
*   Include File Section
****************************************************************************************/
#include "sec_comm.h"
#include "AEEX509Chain.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

/*Define Constant Macro start*/
#define PUBLIC_KEY_OID      1
#define ALG_ID_OID          2
#define NO_OPTIONAL         150


/*Define Constant Macro end*/
#define GLOBAL_IX509CHAIN_HANLDE  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->hIX509Chain)
#define GLOBAL_ISHELL_HANDLE      (((ISec*)hSecHandle)->m_pIShell)


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
#if 1

WE_INT32 X509_CheckAlgOid(const WE_UCHAR *pucOid, WE_UINT16 usOidLen, WE_UINT8 ucOidType);

WE_INT32 X509_GetIssuerOidType(const WE_UCHAR *pucOid, WE_UINT16 usOidLen, WE_UINT32 *puiOidType);

WE_INT32 X509_GetAlgIdInfo(WE_HANDLE hSecHandle, 
                           const WE_UCHAR *pucSign, WE_UCHAR **ppucAlgId, 
                           WE_UINT16 *pusAlgIdLen, WE_UINT16 *pusJumpStep);

WE_INT32 X509_GetSubjectPubKeyInfo(WE_HANDLE hSecHandle, const WE_UCHAR *pucSubjectPublicKeyInfo, 
                                   WE_UCHAR **ppucPublicKeyAlgId, WE_UINT16 *pusPublicKeyAlgIdLen, 
                                   WE_UCHAR **ppucPublicKeyVal, WE_UINT16 *pusPublicKeyValLen, 
                                   WE_UINT16 *pusJumpStep);

WE_INT32 X509_GetExtensions(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtension, 
                            WE_UCHAR **ppucExtensionVal, WE_UINT16 *pusExtensionValLen, 
                            WE_UINT16 *pusJumpStep);

WE_INT32 X509_GetAttributeInfo(WE_HANDLE hSecHandle, const WE_UCHAR *pucIssuer, 
                               WE_VOID **ppvAttributeValue, WE_UINT16 *pusAttributeValueLen,
                               WE_INT16 *psAttributeCharSet, WE_UINT32 *puiAttributeType, WE_UINT16 *pusJumpStep);

#endif


/*******************************************************************************
*   Function Define Section
*******************************************************************************/

#if 1
/*==================================================================================================
FUNCTION: 
    X509_CheckAlgOid
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Check the algorithm OID.
ARGUMENTS PASSED:
    const WE_UCHAR *pucOid [IN]: Pointer to the DER encoded OID in buffer.
    WE_UINT16 usOidLen     [IN]: Length of OID.
    WE_UINT8 ucTypeOid     [IN]: Type of OID.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_UNSUPPORTED_METHOD: Unsurpported method.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
#define RSA_PUBLIC_KEY    1
#define ALG_ID_MD2        2
#define ALG_ID_MD5        4
#define ALG_ID_SHA1       5

WE_INT32 X509_CheckAlgOid(const WE_UCHAR *pucOid, WE_UINT16 usOidLen, WE_UINT8 ucOidType)
{
    const WE_UCHAR ucRsaOid[] = "\x2A\x86\x48\x86\xF7\x0D\x01\x01"; 
    const WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usLoop  = 0;
    WE_INT32  iRes    = M_SEC_ERR_OK;

    if (NULL == pucOid)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = pucOid;
    
    for (usLoop = 0; usLoop < (usOidLen - 1); usLoop++)
    {
        if(*pucPtr == ucRsaOid[usLoop])
        {
            pucPtr++;
        }
        else
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
    }
    
    if (ALG_ID_OID == ucOidType)
    { 
        if (ALG_ID_SHA1 == *pucPtr)
        {
            return iRes;
        }
        else if (ALG_ID_MD5 == *pucPtr)
        {
            return iRes;
        }
#ifdef M_SEC_CFG_MD2
        else if (ALG_ID_MD2 == *pucPtr)
        {
            return iRes;
        }
#endif
        else
        {
            return M_SEC_ERR_UNSUPPORTED_METHOD;
        }
    }

    if (PUBLIC_KEY_OID == ucOidType)
    {
        if (RSA_PUBLIC_KEY == *pucPtr)
        {
            return iRes;
        }
        else
        {
            return M_SEC_ERR_UNSUPPORTED_METHOD;
        }
    }

    return iRes;
}


/*==================================================================================================
FUNCTION: 
    X509_GetIssuerOidType
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the OID type in the issuer field.
ARGUMENTS PASSED:
    const WE_UCHAR *pucOid [IN]: Pointer to the DER encoded OID in buffer.
    WE_UINT16 usOidLen     [IN]: Length of OID.
    WE_UINT32 *puiOidType  [OUT]: Place to type of OID.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 X509_GetIssuerOidType(const WE_UCHAR *pucOid, WE_UINT16 usOidLen, WE_UINT32 *puiOidType)
{
    WE_UCHAR ucOidAddress[2] = {0x55, 0x04};
    WE_UCHAR ucOidEmail[9]   = {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01};
    WE_UCHAR ucOidDomain[10] = {0x09, 0x92, 0x26, 0x89, 0x93, 0xf2, 0x2c, 0x64, 0x01, 0x03};
    const WE_UCHAR *pucPtr   = NULL;

    if ((NULL == pucOid) || (NULL == puiOidType))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = pucOid;

    if ((3 == usOidLen) && (0 == WE_MEMCMP(pucPtr, ucOidAddress, 2))) 
    {
        switch (*(pucPtr + 2))
        {
            case 3:
                *puiOidType = M_SEC_COMMON_NAME;
                break;
            case 4:
                *puiOidType = M_SEC_SURNAME;
                break;
            case 5: 
                *puiOidType = M_SEC_SERIAL_NUMBER;
                break;
            case 6:
                *puiOidType = M_SEC_COUNTRY_NAME;
                break;
            case 7:
                *puiOidType = M_SEC_LOCALITY_NAME;
                break;
            case 8:
                *puiOidType = M_SEC_STATE_OR_PROVINCE_NAME;
                break;
            case 10:
                *puiOidType = M_SEC_ORGANISATION_NAME;
                break;
            case 11:
                *puiOidType = M_SEC_ORGANISATIONAL_UNIT_NAME;
                break;
            case 12:
                *puiOidType = M_SEC_TITLE;
                break;
            case 41:
                *puiOidType = M_SEC_NAME;
                break;
            case 42:
                *puiOidType = M_SEC_GIVENNAME;
                break;
            case 43:
                *puiOidType = M_SEC_INITIALS;
                break;
            case 44:
                *puiOidType = M_SEC_GENERATION_QUALIFIER;
                break;
            case 46:
                *puiOidType = M_SEC_DN_QUALIFIER;
                break;
            default:
                *puiOidType = M_SEC_UNKNOWN_ATTRIBUTE_TYPE;
                break;
        }
    }
    else if ((9 == usOidLen) && (0 == WE_MEMCMP(pucPtr, ucOidEmail, 9))) 
    {
        *puiOidType = M_SEC_EMAIL_ADDRESS;
    }
    else if ((10 == usOidLen) && (0 == WE_MEMCMP(pucPtr, ucOidDomain, 10)))
    {
        *puiOidType = M_SEC_DOMAIN_COMPONENT;
    }
    else
    {
        *puiOidType = M_SEC_UNKNOWN_ATTRIBUTE_TYPE;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    X509_GetAlgIdInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the algorithm identifier information..
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle    [IN]: Global data handle.
    const WE_UCHAR *pucSign [IN]: Pointer to the DER encoded signature in buffer.
    WE_UCHAR **ppucAlgId    [OUT]: Place to put pointer to algorithm identifier.
    WE_UINT16 *pusAlgIdLen  [OUT]: Place to length of algorithm identifier.
    WE_UINT16 *pusJumpStep  [OUT]: Place to step of jump.
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
WE_INT32 X509_GetAlgIdInfo(WE_HANDLE hSecHandle, 
                           const WE_UCHAR *pucSign, WE_UCHAR **ppucAlgId, 
                           WE_UINT16 *pusAlgIdLen, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usSize    = 0;
    WE_UINT16 usTmpStep = 0;
    WE_INT32  iRes      = M_SEC_ERR_OK;

    if ((!pucSign) || (!ppucAlgId) || !pusAlgIdLen || !pusJumpStep)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = (WE_UCHAR *)pucSign;

    pusJumpStep[0] = 0;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    pusJumpStep[0]++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep;
    pucPtr += usTmpStep;
    if (WE_ASN1_OID != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    pusJumpStep[0]++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    *pusAlgIdLen = usSize;
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep;
    pucPtr += usTmpStep;

    iRes = X509_CheckAlgOid(pucPtr, *pusAlgIdLen, ALG_ID_OID);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    *ppucAlgId = pucPtr;
    pucPtr += *pusAlgIdLen;
    pusJumpStep[0] = pusJumpStep[0] + *pusAlgIdLen;

    if (ALG_ID_SHA1 == ppucAlgId[0][(*pusAlgIdLen)-1])
    {
        if ((5 == (*pucPtr)) && (0 == *(pucPtr + 1)))
        {
            pucPtr += 2;
            pusJumpStep[0] += 2;
        }
        else
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    X509_GetSubjectPubKeyInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the subject public key information.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle                   [IN]: Global data handle.
    const WE_UCHAR *pucSubjectPublicKeyInfo[IN]: Pointer to the DER encoded subject public key info in buffer.
    WE_UCHAR **ppucPublicKeyAlgId          [OUT]: Place to put pointer to public key algorithm identifier.
    WE_UINT16 *pusPublicKeyAlgIdLen        [OUT]: Place to length of public key algorithm identifier.
    WE_UCHAR **ppucPublicKeyVal            [OUT]: Place to put pointer to public key value.
    WE_UINT16 *pusPublicKeyValLen          [OUT]: Place to length of public key value.
    WE_UINT16 *pusJumpStep                 [OUT]: Place to step of jump.
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
WE_INT32 X509_GetSubjectPubKeyInfo(WE_HANDLE hSecHandle, const WE_UCHAR *pucSubjectPublicKeyInfo, 
                                   WE_UCHAR **ppucPublicKeyAlgId, WE_UINT16 *pusPublicKeyAlgIdLen, 
                                   WE_UCHAR **ppucPublicKeyVal, WE_UINT16 *pusPublicKeyValLen, 
                                   WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucPtr   = NULL;
    WE_UINT16 usSize    = 0;
    WE_UINT16 usTmpStep = 0;
    WE_INT32  iRes      = M_SEC_ERR_OK;

    if ((NULL == pucSubjectPublicKeyInfo) || (NULL == ppucPublicKeyAlgId) || (NULL == pusPublicKeyAlgIdLen) 
        || (NULL == ppucPublicKeyVal) || (NULL == pusPublicKeyValLen) || (NULL == pusJumpStep))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = (WE_UCHAR *)pucSubjectPublicKeyInfo;
        
    pusJumpStep[0] = 0;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);        
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + 1;
    pucPtr += usTmpStep;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + 1;
    pucPtr += usTmpStep;
    if (WE_ASN1_OID != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    *pusPublicKeyAlgIdLen = usSize;
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + 1;
    pucPtr += usTmpStep;
    iRes = X509_CheckAlgOid(pucPtr, *pusPublicKeyAlgIdLen, PUBLIC_KEY_OID);    
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    *ppucPublicKeyAlgId = pucPtr;
    pucPtr += *pusPublicKeyAlgIdLen;
    pusJumpStep[0] = pusJumpStep[0] + *pusPublicKeyAlgIdLen;
    if (RSA_PUBLIC_KEY == (ppucPublicKeyAlgId[0][(*pusPublicKeyAlgIdLen) - 1]))    
    {
        if ((5 == (*pucPtr)) && (0 == *(pucPtr + 1)))    
        {
            pucPtr += 2;
            pusJumpStep[0] += 2;
        }
        else
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
        
        if (WE_ASN1_BITSTRING != *pucPtr)
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
        
        pucPtr++;
        Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
        pusJumpStep[0] = pusJumpStep[0] + usTmpStep + usSize + 1;
        pucPtr += usTmpStep;
        *pusPublicKeyValLen = usSize;
        *ppucPublicKeyVal = pucPtr;
    }
    else
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    X509_GetExtensions
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get extension information.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle          [IN]: Global data handle.
    const WE_UCHAR *pucExtension  [IN]: Pointer to the DER encoded extension in buffer.
    WE_UCHAR **ppucExtensionVal   [OUT]: Place to put pointer to extension value.
    WE_UINT16 *pusExtensionValLen [OUT]: Place to length of extension value.
    WE_UINT16 *pusJumpStep        [OUT]: Place to step of jump.
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
WE_INT32 X509_GetExtensions(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtension, 
                            WE_UCHAR **ppucExtensionVal, WE_UINT16 *pusExtensionValLen, 
                            WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usSize    = 0;
    WE_UINT16 usTmpStep = 0;

    if ((NULL == pucExtension) || (NULL == ppucExtensionVal) || (NULL == pusExtensionValLen) || (NULL == pusJumpStep))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucPtr = (WE_UCHAR *)pucExtension;
    pusJumpStep[0] = 0;
    if (WE_ASN1_IMPLICIT_TAG != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }

    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);   
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + 1;
    pucPtr += usTmpStep;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }

    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);      
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + usSize + 1;
    pucPtr += usTmpStep;
    *ppucExtensionVal   = pucPtr; 
    *pusExtensionValLen = usSize;

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    X509_GetAttributeInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get issuer's attribute information.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle           [IN]: Global data handle.
    const WE_UCHAR *pucIssuer      [IN]: Pointer to the DER encoded issuer in buffer.
    WE_VOID **ppvAttributeValue    [OUT]: Place to put pointer to attribute value.
    WE_UINT16 *pusAttributeValueLen[OUT]: Place to length of attribute value.
    WE_INT16 *psAttributeCharSet   [OUT]: Place to character set of attribute.
    WE_UINT32 *puiAttributeType    [OUT]: Place to type of attribute.
    WE_UINT16 *pusJumpStep         [OUT]: Place to step of jump.
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
WE_INT32 X509_GetAttributeInfo(WE_HANDLE hSecHandle, const WE_UCHAR *pucIssuer, 
                        WE_VOID **ppvAttributeValue, WE_UINT16 *pusAttributeValueLen,
                        WE_INT16 *psAttributeCharSet, WE_UINT32 *puiAttributeType, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usSize    = 0;
    WE_UINT16 usTmpStep = 0;
    WE_INT32  iRes      = M_SEC_ERR_OK;

    if ((NULL == pucIssuer) || (NULL == ppvAttributeValue) || (NULL == pusAttributeValueLen) 
        || (NULL == psAttributeCharSet) || (NULL == puiAttributeType) || (NULL == pusJumpStep))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    pusJumpStep[0] = 0;
    *pusAttributeValueLen = 0;

    pucPtr = (WE_UCHAR *)pucIssuer;
    if (WE_ASN1_SET_TYPE != *pucPtr) 
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);        
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + 1;
    pucPtr += usTmpStep;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);        
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + 1;
    pucPtr += usTmpStep;
    if (WE_ASN1_OID != *pucPtr) 
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + usSize + 1;
    pucPtr += usTmpStep;
    iRes = X509_GetIssuerOidType(pucPtr, usSize, puiAttributeType);
    pucPtr += usSize;
    *psAttributeCharSet = *pucPtr;
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pusJumpStep[0]        = pusJumpStep[0] + usTmpStep + usSize + 1;
    pucPtr               += usTmpStep;
    *pusAttributeValueLen = usSize;
    *ppvAttributeValue    = pucPtr;
    pucPtr               += usSize;

    iRes = iRes;
    return M_SEC_ERR_OK;
}      
#endif

/*==================================================================================================
FUNCTION: 
    Sec_X509DerToSize
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
WE_VOID Sec_X509DerToSize(WE_HANDLE hSecHandle, WE_UCHAR *pucBuf, 
                                WE_UINT16 *pusTmpStep, WE_UINT16 *pusSize)
{
    WE_UINT8 ucTmpLen = 0;

    if ((NULL == pucBuf) || (NULL == pusTmpStep) || (NULL == pusSize))
    {
        return;
    }
    hSecHandle = hSecHandle;

    pusTmpStep[0] = 1;
    ucTmpLen = *pucBuf;
    pucBuf++;
    
    if (ucTmpLen < 128)
    {
        *pusSize = ucTmpLen;
    }
    else
    {
        ucTmpLen -= 128;
        if (2 == ucTmpLen)
        {
            Sec_StoreStrUint8to16(pucBuf, pusSize);
        }        
        else
        {
            *pusSize = *pucBuf;
        }
        
        pusTmpStep[0] = pusTmpStep[0] + ucTmpLen;
        pucBuf += ucTmpLen;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_X509GetCertFieldLength
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
    WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usTmpStep = 0;
    WE_UINT16 usLen    = 0;

    if ((NULL == pucX509Cert) || (NULL == pusCertLen) || (NULL == pusJumpStep))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = (WE_UCHAR *)pucX509Cert;
    
    pusJumpStep[0] = 0;
    
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usLen);                        
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + 1;
    pucPtr += usTmpStep;
    
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
    M_SEC_ERR_INVALID_PARAMETER
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
   

/*==================================================================================================
FUNCTION: 
    Sec_X509GetIssuerInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
    
    iRes = Sec_X509GetNbrIssuerStr(hSecHandle, pucIssuer, usIssuerLen, &ucNbrIssuerStrings);
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
        iRes = X509_GetAttributeInfo(hSecHandle,pucPtr, 
                                (WE_VOID **)&pucAttributeValue, &usAttributeValueLen, 
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
    Sec_X509CertParseResponse
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
    WE_UCHAR *pucPtr         = NULL;
    WE_UINT8  ucRespType     = 0;
    WE_UINT16 usJumpStep     = 0;
    WE_UINT16 usCharacterSet = 0;
    WE_INT32  iRes           = M_SEC_ERR_OK;

    if ((NULL == pucCertResp) || (NULL == psCharacterSet) || (NULL == ppvDisplayName) 
        || (NULL == piDisplayNameLen) || (NULL == ppucCaKeyHash) || (NULL == ppucSubjectKeyHash)
        || (NULL == ppucCert) || (NULL == pusCertLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = (WE_UCHAR *)pucCertResp;

    if (1 != *pucPtr)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucPtr++;
    ucRespType = *pucPtr;
    pucPtr++;
    switch (ucRespType)
    {
        case 0:
            return M_SEC_ERR_NOT_IMPLEMENTED; 
        case 1:
            Sec_StoreStrUint8to16(pucPtr, &usCharacterSet);
            *psCharacterSet = (WE_INT16)usCharacterSet;
            pucPtr += 2;
            *piDisplayNameLen = *pucPtr;
            pucPtr++;
            *ppvDisplayName = pucPtr;
            pucPtr += *piDisplayNameLen;
            if (254 != *pucPtr)
            {
                return M_SEC_ERR_INVALID_PARAMETER;
            }
            
            pucPtr++;
            *ppucCaKeyHash = pucPtr;
            pucPtr += 20;
            if (254 != *pucPtr)
            {
                return M_SEC_ERR_INVALID_PARAMETER;
            }
            
            pucPtr++;
            *ppucSubjectKeyHash = pucPtr;
            pucPtr += 20;
            if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
            {
                pucPtr += 2;
            }
            
            iRes = Sec_X509GetCertFieldLength(hSecHandle, pucPtr, pusCertLen, &usJumpStep);
            if (M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            
            *ppucCert = pucPtr;
            *pusCertLen = *pusCertLen + usJumpStep /*+ 1*/;
            break;
        case 2:
            return M_SEC_ERR_NOT_IMPLEMENTED;
        default:
            break;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_X509GetRsaModAndExp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
    WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usTmpStep = 0;
    WE_UINT16 usSize    = 0;

    if ((NULL == pucPublicKeyVal) || (NULL == ppucRsaModulus) || (NULL == pusModLen) 
        || (NULL == ppucRsaExponent) || (NULL == pusExponentLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    pucPtr = (WE_UCHAR *)pucPublicKeyVal;
    if (0 == *pucPtr)
    {
        pucPtr++;
    }    

    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
 
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pucPtr += usTmpStep;
    if (WE_ASN1_INTEGER != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    *pusModLen = usSize;
    pucPtr += usTmpStep;
    if (0 == *pucPtr)
    {
        pucPtr++;
        *pusModLen -= 1;
    }
    
    *ppucRsaModulus = pucPtr;
    pucPtr += *pusModLen;
    if (WE_ASN1_INTEGER != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    *pusExponentLen = usSize;
    pucPtr += usTmpStep;
    if (0 == *pucPtr)
    {
        pucPtr++;
        *pusExponentLen -= 1;
    }
    
    *ppucRsaExponent = pucPtr;
    pucPtr += *pusExponentLen;

    return M_SEC_ERR_OK;
}



/*==================================================================================================
FUNCTION: 
    Sec_X509GetCertExtension
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
                                WE_UCHAR **ppucExtnValue, WE_UINT16 *pusExtnValueLen, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucPtr       = NULL;
    WE_UINT8  ucOidIdCe[2] = {0x55, 0x1d};
    WE_UINT8  ucOidWapCe[3]= {0x67, 0x2b, 0x03};
    WE_UINT16 usSize       = 0;
    WE_UINT16 usSize2      = 0;
    WE_UINT16 usTmpStep    = 0;
    WE_UINT16 usOidLen     = 0;

    if ((NULL == pucExtensions) || (NULL == piExtnId) || (NULL == piCritical) 
        || (NULL == ppucExtnValue) || (NULL == pusExtnValueLen) || (NULL == pusJumpStep))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucPtr = (WE_UCHAR *)pucExtensions;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pusJumpStep[0] = 1;
    pusJumpStep[0] = pusJumpStep[0] + usTmpStep + usSize;
    pucPtr += usTmpStep;
    if (WE_ASN1_OID != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usOidLen);
    pucPtr += usTmpStep;
    *piExtnId = 0;
    if ((3 == usOidLen) && (0 == WE_MEMCMP(pucPtr, ucOidIdCe, 2))) 
    {
        switch (*(pucPtr + 2))
        {
            case M_SEC_CERT_EXT_KEY_USAGE:
            case M_SEC_CERT_EXT_EXTENDED_KEY_USAGE:
            case M_SEC_CERT_EXT_CERTIFICATE_POLICIES:
            case M_SEC_CERT_EXT_SUBJECT_ALT_NAME:
            case M_SEC_CERT_EXT_BASIC_CONSTRAINTS:
            case M_SEC_CERT_EXT_NAME_CONSTRAINTS:
            case M_SEC_CERT_EXT_POLICY_CONSTRAINTS:      
            case M_SEC_CERT_EXT_AUTHORITY_KEY_IDENTIFIER:  
            case M_SEC_CERT_EXT_SUBJECT_KEY_IDENTIFIER:
                *piExtnId = *(pucPtr + 2);
                break;
            default:
                break;
        }
    } 
    else if ( (4 == usOidLen) && (0 == WE_MEMCMP(pucPtr, ucOidWapCe, 3))) 
    {
        if (M_SEC_CERT_EXT_DOMAIN_INFORMATION == *(pucPtr + 3))
        {
            *piExtnId = *(pucPtr + 3);
        }
    }

    pucPtr += usOidLen;
    if (WE_ASN1_BOOLEAN != *pucPtr)
    {
        *piCritical = 0;
    } 
    else
    { 
        *piCritical = (*(pucPtr + 1) == 1) && (*(pucPtr + 2) != 0);
        pucPtr += 3;
    }
    
    if (WE_ASN1_OCTETSTRING != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize2);    
    pucPtr += usTmpStep;

    *ppucExtnValue = pucPtr;
    *pusExtnValueLen = usSize2;

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_X509GetKeyUsage
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
    WE_UCHAR *pucPtr       = NULL;
    WE_UINT8  ucLastByte   = 0;
    WE_UINT16 usTmpStep    = 0;
    WE_UINT16 usSize       = 0;
    WE_UINT16 usUnusedBits = 0;

    if ((NULL == pucExtnValue) || (NULL == pucKeyUsageBit0To7) || (NULL == pucKeyUsageBit8To15))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    *pucKeyUsageBit0To7 = 0; 
    *pucKeyUsageBit8To15 = 0;
    
    pucPtr = (WE_UCHAR *)pucExtnValue;
    if (WE_ASN1_BITSTRING != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);
    pucPtr += usTmpStep;
    if (usSize > 1)
    {
        usUnusedBits  = *pucPtr;
        ucLastByte = 0xFF;
        ucLastByte <<= usUnusedBits;
        ucLastByte &= *(pucPtr + usSize - 1);
        if (2 == usSize)
        {
            *pucKeyUsageBit0To7 = ucLastByte;
        }
        else
        {
            *pucKeyUsageBit0To7 = *(pucPtr + 1);
            *pucKeyUsageBit8To15 = ucLastByte;
        }
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_X509GetKeyUsageEx
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
                                WE_INT32 *piEmailProtection, WE_INT32 *piTimeStamping, WE_INT32 *piOcspSigning)
{
    WE_UCHAR *pucPtr       = NULL;
    WE_UINT8  ucOidIdCe[2] = {0x55, 0x1d};
    WE_UINT8  ucOidIdKp[7] = {0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03};
    WE_UINT16 usTmpStep    = 0;
    WE_UINT16 usSize       = 0;
    WE_UINT16 usOidLen     = 0;
    WE_UINT16 usLength     = 0;

    if ((NULL == pucExtnValue) || (NULL == piAnyExtendedKeyUsage) || (NULL == piServerAuth) 
        || (NULL == piClientAuth) || (NULL == piCodeSigning) || (NULL == piEmailProtection)
        || (NULL == piTimeStamping) || (NULL == piOcspSigning))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
       
    *piAnyExtendedKeyUsage = 0;
    *piServerAuth          = 0;
    *piClientAuth          = 0;
    *piCodeSigning         = 0;
    *piEmailProtection     = 0;
    *piTimeStamping        = 0;
    *piOcspSigning         = 0; 

    pucPtr = (WE_UCHAR *)pucExtnValue;
    if(WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize); 
    pucPtr += usTmpStep;
    usLength = usSize;

    while (usLength > 0)
    {
        if(WE_ASN1_OID != *pucPtr)
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
        
        pucPtr++;
        usLength--;
        Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usOidLen);
        pucPtr += usTmpStep;
        usLength = usLength - usTmpStep;
        if ((4 == usOidLen) && (0 == WE_MEMCMP(pucPtr, ucOidIdCe, 2))) 
        {
            if ((37 == *(pucPtr + 2)) && (0 == *(pucPtr + 3)))
            {
                *piAnyExtendedKeyUsage = 1;
            }
        }
        else if ((8 == usOidLen) && (0 == WE_MEMCMP(pucPtr, ucOidIdKp, 7)))  
        {
            switch (*(pucPtr + 7))
            {
                case 1:
                    *piServerAuth = 1;
                    break;
                case 2:
                    *piClientAuth = 1;
                    break;
                case 3:
                    *piCodeSigning = 1;
                    break;
                case 4:
                    *piEmailProtection = 1;
                    break;
                case 8:
                    *piTimeStamping = 1;
                    break;
                case 9:
                    *piOcspSigning = 1;
                    break;
                default:
                    break;
            }
        } 
        
        pucPtr += usOidLen;
        usLength = usLength - usOidLen;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_X509GetBasicConstraint
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
                                WE_INT32 *piCa, WE_INT32 *piHasPathLenConstraint, WE_UINT8 *pucPathLenConstraint)
{
    WE_UCHAR *pucPtr    = NULL;
    WE_UINT16 usTmpStep = 0;
    WE_UINT16 usSize    = 0;

    if ((NULL == pucExtnValue) || (NULL == piCa) || (NULL == piHasPathLenConstraint) 
        || (NULL == pucPathLenConstraint))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
        
    *piCa = 0;
    *piHasPathLenConstraint = 0;
    
    pucPtr = (WE_UCHAR *)pucExtnValue;
    if (WE_ASN1_SEQUENCE_TYPE != *pucPtr)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pucPtr++;
    Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);    
    pucPtr += usTmpStep;
    if ((0 == usSize) || (WE_ASN1_BOOLEAN != *pucPtr))  
    {
        return M_SEC_ERR_OK;
    }
    
    *piCa = (*(pucPtr + 1) == 1) && (*(pucPtr + 2) != 0);
    pucPtr += 3;
    usSize -= 3;
    if (usSize > 0)
    {
        *piHasPathLenConstraint = 1;
        if (WE_ASN1_INTEGER != *pucPtr)
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
        
        pucPtr++;
        Sec_X509DerToSize(hSecHandle, pucPtr, &usTmpStep, &usSize);    
        pucPtr += usTmpStep;
        if (usSize > 1)
        {
            *pucPathLenConstraint = 127;
        }
        else
        {
            *pucPathLenConstraint = *pucPtr;
        }
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_X509ChainInit
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize the x509chain instance.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
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
WE_INT32 Sec_X509ChainInit(WE_HANDLE hSecHandle)
{
    WE_HANDLE hIX509Chain = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    hIX509Chain = GLOBAL_IX509CHAIN_HANLDE;

    if (NULL == hIX509Chain)
    {
        if (M_SEC_ERR_OK == ISHELL_CreateInstance(GLOBAL_ISHELL_HANDLE, AEECLSID_X509CHAIN, &hIX509Chain))
        {
            GLOBAL_IX509CHAIN_HANLDE = hIX509Chain;
        }
        else
        {
            return M_SEC_ERR_NOT_IMPLEMENTED;
        }
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_X509ChainTerminate
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Terminate the x509chain instance.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_X509ChainTerminate(WE_HANDLE hSecHandle)
{
    WE_HANDLE hIX509Chain = NULL;
    
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    hIX509Chain = GLOBAL_IX509CHAIN_HANLDE;

    if (hIX509Chain)
    {
        (WE_VOID)IX509CHAIN_Release(hIX509Chain);
        GLOBAL_IX509CHAIN_HANLDE = NULL;
    }

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_X509CertParse
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
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
                                WE_UINT16 *pusCertLen, St_SecCertificate *pstCert)
{
    WE_UINT16 usCertLen   = 0;
    WE_UINT16 usTbsLen    = 0;
    WE_UINT16 usStepLen   = 0;
    WE_UINT16 usStepLen1  = 0;
    WE_UINT16 usFieldLen  = 0;
    WE_INT32  iCertIndex  = 0;
    WE_INT32  iRes        = M_SEC_ERR_OK;
    WE_INT32  iCertWebOpt = WEBOPT_X509_ROOT_CERTS;
    WE_UINT   uiFieldLen  = 0;
    
    WE_UCHAR      *pucField       = NULL;
    IX509Chain    *pstIX509Chain  = NULL;
    X509BasicCert  stX509BasicCert;
    X509CertPart   stX509CertPart;
    X509PartRequest stX509PartRequest;

    if ((NULL == hSecHandle) || (NULL == pucX509Cert) || (NULL ==pusCertLen) || (NULL == pstCert))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    /* get cert length */
    iRes += Sec_X509GetCertFieldLength(hSecHandle, pucX509Cert, &usCertLen, &usStepLen);
   
    /* get Tbscert length*/
    iRes += Sec_X509GetCertFieldLength(hSecHandle, pucX509Cert+usStepLen, &usTbsLen, &usStepLen1);    
    if ((M_SEC_ERR_OK != iRes) || (usTbsLen >= usCertLen))
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    
    pstIX509Chain = (IX509Chain *)GLOBAL_IX509CHAIN_HANLDE;

    /* add cert */
    iRes = IX509CHAIN_AddCert(pstIX509Chain, iCertWebOpt, pucX509Cert, usCertLen+usStepLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }

    /* get the certificate length */
    /*   delete by stone
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_RAW_CERT, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }*/
    *pusCertLen = usCertLen;

    /* get the tbs certificate */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_SIGNED, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    pstCert->stCertUnion.stX509.pucTbsCert = pucField;
    pstCert->stCertUnion.stX509.uiTbsCertLen = uiFieldLen;
    
    /* get the serial number */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_SERIAL, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    pstCert->stCertUnion.stX509.pucSerialNumber = pucField;
    pstCert->stCertUnion.stX509.usSerialNumberLen = (WE_UINT16)uiFieldLen;

    /* get the signature */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_SIG_TYPE, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    iRes = X509_GetAlgIdInfo(hSecHandle, pucField,
                             &(*pstCert).stCertUnion.stX509.pucSignatureAlgId,
                             &(*pstCert).stCertUnion.stX509.usSignatureAlgIdLen,
                             &usFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    /* get the issuer */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_ISSUER, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    pstCert->pucIssuer = pucField;
    pstCert->stCertUnion.stX509.usIssuerLen = (WE_UINT16)uiFieldLen;
    
    /* get the subject */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_SUBJECT, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    pstCert->pucSubject = pucField;
    pstCert->stCertUnion.stX509.usSubjectLen = (WE_UINT16)uiFieldLen;
    
    /* get the public key info */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_PUBKEY, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    iRes= X509_GetSubjectPubKeyInfo(hSecHandle, pucField, 
                                    &(*pstCert).stCertUnion.stX509.pucPublicKeyAlgId, 
                                    &(*pstCert).stCertUnion.stX509.usPublicKeyAlgIdLen,
                                    &(*pstCert).stCertUnion.stX509.pucPublicKeyVal, 
                                    &(*pstCert).stCertUnion.stX509.usPublicKeyValLen, 
                                    &usFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    /* get the extensions */
    pstCert->stCertUnion.stX509.pucExtensions = NULL;
    pstCert->stCertUnion.stX509.usExtensionsLen = 0;
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_EXTENSIONS, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK == iRes)
    {
        iRes = X509_GetExtensions(hSecHandle, pucField, 
                           &(*pstCert).stCertUnion.stX509.pucExtensions, 
                           &(*pstCert).stCertUnion.stX509.usExtensionsLen, &usFieldLen);
    }
    
    /* get the sub-field */
    stX509PartRequest.nCertWebOpt = iCertWebOpt;
    stX509PartRequest.nCertIndex  = iCertIndex;
    stX509PartRequest.uCertPart   = X509CHAIN_FIELD_EXTENSIONS;
    stX509PartRequest.pcRawOID    = NULL;
    stX509PartRequest.nInstance   = 0;
    stX509PartRequest.uASNOID     = ASNOID_CERTEXT_KEY_USE;
    iRes = IX509CHAIN_GetFieldPart(pstIX509Chain, &stX509PartRequest, &stX509CertPart);
    if (M_SEC_ERR_OK == iRes)
    {
        pstCert->stCertUnion.stX509.uiExtKeyUsageLen = stX509CertPart.uLength;
        pstCert->stCertUnion.stX509.pucExtKeyUsage = (WE_UCHAR *)stX509CertPart.pcValue;
    }
    else
    {
        pstCert->stCertUnion.stX509.uiExtKeyUsageLen = 0;
        pstCert->stCertUnion.stX509.pucExtKeyUsage = NULL;
    }

    stX509PartRequest.uASNOID = ASNOID_CERTEXT_EXT_KEY_USE;
    iRes = IX509CHAIN_GetFieldPart(pstIX509Chain, &stX509PartRequest, &stX509CertPart);
    if (M_SEC_ERR_OK == iRes)
    {
        pstCert->stCertUnion.stX509.uiExtExtKeyUsageLen = stX509CertPart.uLength;
        pstCert->stCertUnion.stX509.pucExtExtKeyUsage = (WE_UCHAR *)stX509CertPart.pcValue;
    }
    else
    {
        pstCert->stCertUnion.stX509.uiExtExtKeyUsageLen = 0;
        pstCert->stCertUnion.stX509.pucExtExtKeyUsage = NULL;
    }

    stX509PartRequest.uASNOID = ASNOID_CERTEXT_SUBJALTNAME;
    iRes = IX509CHAIN_GetFieldPart(pstIX509Chain, &stX509PartRequest, &stX509CertPart);
    if (M_SEC_ERR_OK == iRes)
    {
        pstCert->stCertUnion.stX509.uiExtSubjectAltNameLen = stX509CertPart.uLength;
        pstCert->stCertUnion.stX509.pucExtSubjectAltName = (WE_UCHAR *)stX509CertPart.pcValue;
    }
    else
    {
        pstCert->stCertUnion.stX509.uiExtSubjectAltNameLen = 0;
        pstCert->stCertUnion.stX509.pucExtSubjectAltName = NULL;
    }

    stX509PartRequest.uASNOID = ASNOID_CERTEXT_ISSALTNAME;
    iRes = IX509CHAIN_GetFieldPart(pstIX509Chain, &stX509PartRequest, &stX509CertPart);
    if (M_SEC_ERR_OK == iRes)
    {
        pstCert->stCertUnion.stX509.uiExtIssuerAltNameLen = stX509CertPart.uLength;
        pstCert->stCertUnion.stX509.pucExtIssuerAltName = (WE_UCHAR *)stX509CertPart.pcValue;
    }
    else
    {
        pstCert->stCertUnion.stX509.uiExtIssuerAltNameLen = 0;
        pstCert->stCertUnion.stX509.pucExtIssuerAltName = NULL;
    }

    stX509PartRequest.uASNOID = ASNOID_CERTEXT_AUTHKEY_ID;
    iRes = IX509CHAIN_GetFieldPart(pstIX509Chain, &stX509PartRequest, &stX509CertPart);
    if (M_SEC_ERR_OK == iRes)
    {
        pstCert->stCertUnion.stX509.uiExtAuthKeyIdLen = stX509CertPart.uLength;
        pstCert->stCertUnion.stX509.pucExtAuthKeyId = (WE_UCHAR *)stX509CertPart.pcValue;
    }
    else
    {
        pstCert->stCertUnion.stX509.uiExtAuthKeyIdLen = 0;
        pstCert->stCertUnion.stX509.pucExtAuthKeyId = NULL;
    }

    stX509PartRequest.uASNOID = ASNOID_CERTEXT_B_CONSTRAIN;
    iRes = IX509CHAIN_GetFieldPart(pstIX509Chain, &stX509PartRequest, &stX509CertPart);
    if (M_SEC_ERR_OK == iRes)
    {
        pstCert->stCertUnion.stX509.uiExtBasicConstraintLen = stX509CertPart.uLength;
        pstCert->stCertUnion.stX509.pucExtBasicConstraint = (WE_UCHAR *)stX509CertPart.pcValue;
    }
    else
    {
        pstCert->stCertUnion.stX509.uiExtBasicConstraintLen = 0;
        pstCert->stCertUnion.stX509.pucExtBasicConstraint = NULL;
    }

    stX509PartRequest.uASNOID = ASNOID_CERTEXT_SUBJKEY_ID;
    iRes = IX509CHAIN_GetFieldPart(pstIX509Chain, &stX509PartRequest, &stX509CertPart);
    if (M_SEC_ERR_OK == iRes)
    {
        pstCert->stCertUnion.stX509.uiExtSubjectKeyIdLen = stX509CertPart.uLength;
        pstCert->stCertUnion.stX509.pucExtSubjectKeyId = (WE_UCHAR *)stX509CertPart.pcValue;
    }
    else
    {
        pstCert->stCertUnion.stX509.uiExtSubjectKeyIdLen = 0;
        pstCert->stCertUnion.stX509.pucExtSubjectKeyId = NULL;
    }

    /* get the signatureAlgorithm */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_SIGN_ALG, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    iRes = X509_GetAlgIdInfo(hSecHandle, pucField,
                             &(*pstCert).stCertUnion.stX509.pucSignatureAlgorithm,
                             &(*pstCert).stCertUnion.stX509.usSignatureAlgorithmLen,
                             &usFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    
    if((0 != WE_MEMCMP(pstCert->stCertUnion.stX509.pucSignatureAlgId,
              pstCert->stCertUnion.stX509.pucSignatureAlgorithm,
              pstCert->stCertUnion.stX509.usSignatureAlgIdLen))
       ||(0 != WE_MEMCMP(pstCert->stCertUnion.stX509.pucSignatureAlgId,
              pstCert->stCertUnion.stX509.pucSignatureAlgorithm,
              pstCert->stCertUnion.stX509.usSignatureAlgorithmLen)))
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }

    /* get the signature field  */
    iRes = IX509CHAIN_GetField(pstIX509Chain, iCertWebOpt, iCertIndex, X509CHAIN_FIELD_SIGNATURE, 
                                (const WE_UCHAR **)&pucField, &uiFieldLen);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    pucField++;
    Sec_X509DerToSize(hSecHandle, pucField, &usStepLen, &usFieldLen);
    if ( 0 == *(pucField + usStepLen))
    {
        usStepLen++;
        usFieldLen--;
    }
    pstCert->pucSignature = pucField + usStepLen;
    pstCert->usSignatureLen = usFieldLen;

    /* get the basic information */
    iRes = IX509CHAIN_GetBasic(pstIX509Chain, iCertWebOpt, iCertIndex, &stX509BasicCert);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }
    pstCert->ucCertificateVersion = stX509BasicCert.uVersion + 1;
    /* because the time get from brew start 1980, but the use in actual from 1970.
       so must add the value between, 3657(day) * 24 * 3600 =  315964800(seconds).
    */
    pstCert->iValidNotBefore = stX509BasicCert.uStartValidity + 315964800;
    pstCert->iValidNotAfter = stX509BasicCert.uEndValidity + 315964800;
    pstCert->stCertUnion.stX509.pucModulus = (WE_UCHAR *)stX509BasicCert.Key.puModulus;
    pstCert->stCertUnion.stX509.usModulusLen = stX509BasicCert.Key.uModulusLen;
    pstCert->stCertUnion.stX509.pucExponent = (WE_UCHAR *)stX509BasicCert.Key.puExponent;
    pstCert->stCertUnion.stX509.usExponentLen = stX509BasicCert.Key.uExponentLen;
    
    return iRes;
}



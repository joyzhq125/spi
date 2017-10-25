/*==================================================================================================
    FILE NAME : x509.c
    MODULE NAME : SEC

    GENERAL DESCRIPTION:
        The functions in this file process  certificate's storage,encryption
        and decryption..
  
    Techfaith Software Confidential Proprietary(c)
    Copyright 2002-2006 by Techfaith Software. All Rights Reserved.
================================================================================================
    Revision History
    
    Modification              Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-12-07 tang           None      Init
      
==================================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "sec_comm.h"
/***************************************************************************************************
*   Macro Define Section
**************************************************************************************************/
#define PUBLIC_KEY_OID                  1
#define ALG_ID_OID                      2
#define NO_OPTIONAL                      150

#define M_X509_CHECK_FOR_NULL_ARG(Arg,Err) \
{ if (!(Arg)) { \
    return Err; \
} \
}

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
WE_INT32 We_X509GetVersion(const WE_UCHAR *pucX509Cert, 
                           WE_UINT8 *pucVersion, WE_UINT16 *pusJumpStep);
WE_INT32 We_X509GetSerialNumber(const WE_UCHAR *pucX509Cert, 
                                WE_UCHAR **ppucSerialNumber, 
                                WE_UINT16 *pusSerialNumberLen, 
                                WE_UINT16 *pusJumpStep);
WE_INT32 We_X509CheckOID(const WE_UCHAR *pucX509Cert,
                         WE_UINT16 usOidLen, WE_UINT8 ucTypeOid);
WE_INT32 We_X509GetSignatureAlgId(const WE_UCHAR *pucX509Cert, 
                                  WE_UCHAR **ppucSignatureAlgId, 
                                  WE_UINT16 *pusSignatureAlgIdLen, 
                                  WE_UINT16 *pusJumpStep);
WE_INT32 We_X509GetIssuer(const WE_UCHAR *pucX509Cert, 
                          WE_UCHAR **ppucIssuer, WE_UINT16 *pusIssuerLen, 
                          WE_UINT16 *pusJumpStep);
WE_UINT16 We_X509DerToTime(const WE_UCHAR *pucX509Cert);
WE_INT32 We_X509GetSeconds(const WE_UCHAR *pucX509Cert, WE_UINT32 *puiNbrOfSeconds, 
                           WE_UINT16 usUtcTimeLen);
WE_INT32 We_X509GetValidity(const WE_UCHAR *pucX509Cert, 
                            WE_UINT32 *puiValidNotBefore, 
                            WE_UINT32 *puiValidNotAfter, 
                            WE_UINT16 *pusJumpStep);

WE_INT32 We_X509GetSubject(const WE_UCHAR *pucX509Cert, 
                           WE_UCHAR **ppucSubject, WE_UINT16 *pusSubjectLen, 
                           WE_UINT16 *pusJumpStep);
WE_INT32 We_X509GetSubjectPublicKeyInfo(const WE_UCHAR *pucX509Cert, 
                                        WE_UCHAR **ppucPublicKeyAlgId, 
                                        WE_UINT16 *pusPublicKeyAlgIdLen, 
                                        WE_UCHAR **ppucPublicKeyVal, 
                                        WE_UINT16 *pusPublicKeyValLen,
                                        WE_UINT16 *pusJumpStep);
WE_INT32 We_X509CheckUniqueIdentifier(const WE_UCHAR *pucX509Cert, 
                                      WE_UINT16 *pusJumpStep);

WE_INT32 We_X509GetExtensions1(const WE_UCHAR *pucX509Cert, 
                               WE_UCHAR **ppucExtensions, 
                               WE_UINT16 *pusExtensionsLen, WE_UINT16 *pusJumpStep);


WE_INT32 We_X509GetSignature(const WE_UCHAR *pucX509Cert, 
                             WE_UCHAR **ppucSignature, WE_UINT16 *pusSignatureLen, 
                             WE_UINT16 *pusJumpStep);

static WE_INT32 We_X509CheckIssuerOID(const WE_UCHAR *pucX509Cert, WE_UCHAR **ppucStr, WE_UINT16 usOidLen);

#if 0
static WE_INT32 We_X509GetExtnKeysUsage(const WE_UCHAR *pucExtnValue, 
                                        WE_UCHAR** ppucExKeyusage,
                                        WE_UINT16* pusExKeyUsageLen);
#endif



/***************************************************************************************************
*                           Function Define Section
***************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    We_X509DERtoSize
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
     Get the identifier size and the length of value.
ARGUMENTS PASSED:
    WE_UCHAR *pucCert[IN]:Pointer to the DER encoded data in buffer..
    WE_UINT16 *pusTmpStep[OUT]:Place to step of jump.
    WE_UINT16 *pusSize[OUT]:Place to size of identifier.
RETURN VALUE:
    none
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID We_X509DERtoSize(WE_UCHAR *pucCert, WE_UINT16 *pusTmpStep, WE_UINT16 *pusSize)
{
    WE_UINT8 ucTmpLen=0;
    
    if (!pucCert||!pusTmpStep||!pusSize)
    {
        return;
    }
    pusTmpStep[0]=0;
    ucTmpLen = *pucCert;
    pucCert++;
    pusTmpStep[0]++;
    if (ucTmpLen < 128)                                
    {
        *pusSize = (WE_UINT16)ucTmpLen;                        
    }
    else
    {
        ucTmpLen -= 128;                    
        if (ucTmpLen==2)
        {
            We_UTILSGetUINT16(pucCert,pusSize);
        }
        else
        {
            *pusSize=*pucCert; 
        }
        pusTmpStep[0] = (WE_UINT16)(pusTmpStep[0] + ucTmpLen);
        pucCert+=ucTmpLen;        
    }
}
/*==================================================================================================
FUNCTION: 
    We_X509GetAndCheckLength
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get and check length.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate
    WE_UINT16 *pusCertLen[OUT]:length of cert.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetAndCheckLength(const WE_UCHAR *pucX509Cert, WE_UINT16 *pusCertLen, 
                                   WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusCertLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)                
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    pucCert ++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);                        
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if (*pusCertLen == 0)
    {
        *pusCertLen = usSize;
    }
    else if (*pusCertLen != usSize)
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetAndCheckTBSCertLength
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get and check tbs certificate length.
ARGUMENTS PASSED:
    const WE_UCHAR *pucTbsCert[IN/OUT]:tbs certificate
    WE_UINT16 *pusTbsCertLen[OUT]:length of tbs certificate
    WE_UINT16 usCertLen[IN]:length of cert.
    WE_UINT16 *pusJumpStep[OUT]:step of jump
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
WE_INT32 
We_X509GetAndCheckTBSCertLength(const WE_UCHAR *pucTbsCert, 
                                 WE_UINT16 *pusTbsCertLen, WE_UINT16 usCertLen, 
                                 WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    usCertLen=usCertLen;
    
    M_X509_CHECK_FOR_NULL_ARG(pucTbsCert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusTbsCertLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucTbsCert;
    pusJumpStep[0] = 0;    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)    
        return SEC_ERR_INVALID_PARAMETER;    
    pucCert ++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);        
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if (*pusTbsCertLen == 0)
    {
        *pusTbsCertLen = usSize;
    }
    else if (*pusTbsCertLen != usSize)
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    /*
    if (!(*pusTbsCertLen < usCertLen))
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    */
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetVersion
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get certificate version.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate
    WE_UINT8 *pucVersion[OUT]:the version of certificate.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetVersion(const WE_UCHAR *pucX509Cert, WE_UINT8 *pucVersion, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pucVersion, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;  
    
    if (*pucCert == SEC_ASN_1_INTEGER)
    {                              
        *pucVersion = 1;               
        return SEC_ERR_OK;              
    }                      
    
    if(*pucCert != SEC_ASN_1_EXPLICIT_TAG)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    if(*pucCert != 3)                        
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    if(*pucCert != SEC_ASN_1_INTEGER)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    if(*pucCert != 1)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++; 
    
    if(!((*pucCert == 2)    || (*pucCert == 1)))    
    {
        return SEC_ERR_BAD_CERTIFICATE;            
    }
    *pucVersion= (WE_UINT8)((*pucCert) +1);  
    pucCert++;
    pusJumpStep[0]++;    
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetSerialNumber
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get serial number.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate
    WE_UCHAR **ppucSerialNumber[OUT]:serial number.
    WE_UINT16 *pusSerialNumberLen[OUT]:the length of ppucSerialNumber.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetSerialNumber(const WE_UCHAR *pucX509Cert, 
                                     WE_UCHAR **ppucSerialNumber, 
                                     WE_UINT16 *pusSerialNumberLen, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = (WE_UCHAR *)pucX509Cert;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucSerialNumber, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusSerialNumberLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;    
    
    if(*pucCert != SEC_ASN_1_INTEGER)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);    
    *pusSerialNumberLen=usSize;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    *ppucSerialNumber =pucCert;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + (*pusSerialNumberLen));
    pucCert +=*pusSerialNumberLen;
    
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_X509CheckOID
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    check OID.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UINT16 usOidLen[IN]:length of oid.
    WE_UINT8 ucTypeOid[IN]:length of oid type.
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
WE_INT32 We_X509CheckOID(const WE_UCHAR *pucX509Cert,WE_UINT16 usOidLen, WE_UINT8 ucTypeOid)
{
    const WE_UCHAR oid_rsa[]="\x2A\x86\x48\x86\xF7\x0D\x01\x01"; 
    //#ifdef SEC_CFG_DHE_DSS
    const WE_UCHAR oid_dsa[]="\x2A\x86\x48\xCE\x38\x04\x03"; 
    //#endif
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usIndex=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;    
    for (usIndex=0;usIndex<(usOidLen-1);usIndex++)
    {
        if(*pucCert == oid_rsa[usIndex])            
        {
            pucCert++;
        }
        //#ifdef SEC_CFG_DHE_DSS
        else if (*pucCert == oid_dsa[usIndex])
        {
            pucCert++;
        }
        //#endif
        else
        {
            return SEC_ERR_BAD_CERTIFICATE;
        }
    }
    if (ucTypeOid == ALG_ID_OID)
    { 
        if (*pucCert == 5)                
        {
            return iRes;
        }
        else if (*pucCert == 4)    
        {
            return iRes;
        }
        //#ifdef SEC_CFG_MD2
        else if (*pucCert == 2)    
        {
            return iRes;
        }
        //#endif
        //#ifdef SEC_CFG_DHE_DSS
        else if (*pucCert == 3)    
        {
            return iRes;
        }
        //#endif
        else
        {
            return SEC_ERR_UNSUPPORTED_METHOD;
        }
    }
    
    if (ucTypeOid == PUBLIC_KEY_OID)
    {
        if (*pucCert == 1)                    
        {
            return iRes;
        }
        //#ifdef SEC_CFG_DHE_DSS
        else if (*pucCert == 1)    
        {
            return iRes;        
        }
        //#endif
        else
        {
            return SEC_ERR_UNSUPPORTED_METHOD;
        }
    }
    
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetSignatureAlgId
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    check OID.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucSignatureAlgId[OUT]:the id of signature alg.
    WE_UINT16 *pusSignatureAlgIdLen[OUT]:the length of signature alg id
    WE_UINT16 *pusJumpStep[OUT]:jump step
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
WE_INT32 We_X509GetSignatureAlgId(const WE_UCHAR *pucX509Cert, 
                                       WE_UCHAR **ppucSignatureAlgId, 
                                       WE_UINT16 *pusSignatureAlgIdLen, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucSignatureAlgId, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusSignatureAlgIdLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);        
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if(*pucCert != SEC_ASN_1_OBJECT_IDENTIFIER)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);      
    *pusSignatureAlgIdLen = usSize;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert += usTmpStep;
    
    iRes = We_X509CheckOID(pucCert,*pusSignatureAlgIdLen, ALG_ID_OID);        
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    *ppucSignatureAlgId = pucCert;
    pucCert += *pusSignatureAlgIdLen;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + (*pusSignatureAlgIdLen));
    
    if ((ppucSignatureAlgId[0][(*pusSignatureAlgIdLen)-1]) == 5)
    {
        if (((*pucCert) == 5 ) && ((*(pucCert+1) == 0)))     
        {
            pucCert += 2;
            pusJumpStep[0] += 2;
        }
        else
        {
            return SEC_ERR_BAD_CERTIFICATE;
        }
    }        
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetIssuer
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the issuer of cerificate. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucIssuer[OUT]:the issuer.
    WE_UINT16 *pusIssuerLen[OUT]:the length of issuer.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetIssuer(const WE_UCHAR *pucX509Cert, 
                               WE_UCHAR **ppucIssuer, WE_UINT16 *pusIssuerLen, 
                               WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT16      usSize=0;
    WE_UINT16      usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucIssuer, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusIssuerLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    We_X509DERtoSize(pucCert+1, &usTmpStep, &usSize);            
    *pusIssuerLen = (WE_UINT16)(usSize + usTmpStep + 1);
    
    *ppucIssuer =pucCert;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + (*pusIssuerLen));
    
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509DerToTime
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get time. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
RETURN VALUE:
    1)the value of time.
    2)error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT16 We_X509DerToTime(const WE_UCHAR *pucX509Cert)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT8 ucZeroInAscii=0;                              
    WE_UINT8 ucTen = 0;
    WE_UINT8 ucNol = 0;
    WE_UINT16 usResult =0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    ucZeroInAscii=48;                              
    ucTen = (WE_UINT8)((*pucCert)-ucZeroInAscii);
    ucNol = (WE_UINT8)((*(pucCert+1))-ucZeroInAscii);
    usResult = (WE_UINT16)((10*ucTen)+ucNol);
    
    return usResult;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetSeconds
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get second. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UINT32 *puiNbrOfSeconds[OUT]:the number of second.
    WE_UINT16 usUtcTimeLen[IN]:the length of utc time.
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
WE_INT32 We_X509GetSeconds(const WE_UCHAR *pucX509Cert, WE_UINT32 *puiNbrOfSeconds, WE_UINT16 usUtcTimeLen)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT16 usTmpYear=0;
    WE_UINT32 uiSeconds=0;
    WE_UINT32 uiSecondsInYear=365*24*60*60;
    WE_UINT32 uiSecondsInLeapYear=366*24*60*60;
    WE_UINT32 uiSecondsIn28days=28*24*60*60;
    WE_UINT32 uiSecondsIn29days=29*24*60*60;
    WE_UINT32 uiSecondsIn30Days=30*24*60*60;
    WE_UINT32 uiSecondsIn31Days=31*24*60*60;
    WE_UINT32 uiNbrOfLeapYear=0;
    WE_UINT32 uiNbrOfYear=0;
    WE_UINT16 usYear=0;
    WE_UINT16 usMonth=0;
    WE_UINT16 usDay=0;
    WE_UINT16 usHour=0;
    WE_UINT16 usMinute=0;
    WE_UINT16 usSecond=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(puiNbrOfSeconds, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    usYear=We_X509DerToTime((const WE_UCHAR *) pucCert);
    pucCert+=2;
    usMonth=We_X509DerToTime((const WE_UCHAR *) pucCert);
    pucCert+=2;
    if ((usMonth<1) || (usMonth>12))
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    usDay=We_X509DerToTime((const WE_UCHAR *) pucCert);
    pucCert+=2;
    if ((usDay<1) || (usDay>31))
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    usHour=We_X509DerToTime((const WE_UCHAR *) pucCert);
    pucCert+=2;
    if ((usHour>23))
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    usMinute=We_X509DerToTime((const WE_UCHAR *) pucCert);
    pucCert+=2;
    if ( (usMinute>59))
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    usSecond=We_X509DerToTime((const WE_UCHAR *) pucCert);
    pucCert+=2;
    if ( (usSecond>59))
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    if (usUtcTimeLen==13)
    {
        if (usYear<=49)          
        {
            usYear+=2000;
        }
        else
        {
            usYear+=1900;
        }
        usTmpYear = (WE_UINT16)(usYear - 1970);
        if (usYear>=1972)
        {
            uiNbrOfLeapYear=((usYear-1972)/4)+1;        
            if ((usYear%4)==0)
            {
                uiNbrOfLeapYear--;
            }
        }
        else
        {
            uiNbrOfLeapYear=0;
        }
        uiNbrOfYear=usTmpYear-uiNbrOfLeapYear;
        
        uiSeconds=uiNbrOfYear*uiSecondsInYear;
        uiSeconds+=uiNbrOfLeapYear*uiSecondsInLeapYear;
        
        if (usMonth>1)
        {
            uiSeconds+=uiSecondsIn31Days;
        }
        if (usMonth>2)
        {
            if ((usYear%4)==0)
            {
                uiSeconds+=uiSecondsIn29days;
            }
            else
            {
                uiSeconds+=uiSecondsIn28days;
            }
        }
        if (usMonth>3)
        {
            uiSeconds+=uiSecondsIn31Days;
        }
        if (usMonth>4)
            uiSeconds+=uiSecondsIn30Days;
        if (usMonth>5)
            uiSeconds+=uiSecondsIn31Days;
        if (usMonth>6)
            uiSeconds+=uiSecondsIn30Days;
        if (usMonth>7)
            uiSeconds+=uiSecondsIn31Days;
        if (usMonth>8)
            uiSeconds+=uiSecondsIn31Days;
        if (usMonth>9)
            uiSeconds+=uiSecondsIn30Days;
        if (usMonth>10)
            uiSeconds+=uiSecondsIn31Days;
        if (usMonth>11)
            uiSeconds+=uiSecondsIn30Days;
        
        uiSeconds+=(usDay-1)*24*60*60;
        uiSeconds+=usHour*60*60;
        uiSeconds+=usMinute*60;
        uiSeconds+=usSecond;
        *puiNbrOfSeconds=uiSeconds;
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetValidity
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get validity. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UINT32 *puiValidNotBefore[OUT]:the validity of not before.
    WE_UINT32 *puiValidNotAfter[OUT]:the validity of not after.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetValidity(const WE_UCHAR *pucX509Cert, WE_UINT32 *puiValidNotBefore, 
                                 WE_UINT32 *puiValidNotAfter, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 tmpLen = 0;
    WE_UINT16 usUtcTimeLen = 0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(puiValidNotBefore, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(puiValidNotAfter, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    tmpLen=*pucCert;
    pucCert++;
    pusJumpStep[0]++;
    
    if(*pucCert != SEC_ASN_1_UTCTIME)        
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;  
    
    usUtcTimeLen=*pucCert;                                
    pucCert++;
    pusJumpStep[0]++;
    
    iRes= We_X509GetSeconds((const WE_UCHAR *)pucCert, puiValidNotBefore, usUtcTimeLen);
    
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usUtcTimeLen);
    pucCert+= usUtcTimeLen;
    
    if(*pucCert != SEC_ASN_1_UTCTIME)        
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;  
    
    usUtcTimeLen=*pucCert;                                
    pucCert++;
    pusJumpStep[0]++;
    
    iRes= We_X509GetSeconds((const WE_UCHAR *)pucCert, puiValidNotAfter, usUtcTimeLen);
    
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usUtcTimeLen);
    pucCert+= usUtcTimeLen;

    tmpLen = tmpLen;
    return iRes;    
}
/*==================================================================================================
FUNCTION: 
    We_X509GetSubject
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get certificate subject. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucSubject[OUT]:subject.
    WE_UINT16 *pusSubjectLen[OUT]:length of subject.
    WE_UINT16 *pusJumpStep[OUT]:jump step
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
WE_INT32 We_X509GetSubject(const WE_UCHAR *pucX509Cert, WE_UCHAR **ppucSubject, 
                                WE_UINT16 *pusSubjectLen, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucSubject, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusSubjectLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    We_X509DERtoSize(pucCert+1, &usTmpStep, &usSize);
    *pusSubjectLen = (WE_UINT16)(usSize + usTmpStep + 1);
    
    *ppucSubject =pucCert;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + (*pusSubjectLen));
    
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetRSAModAndExp
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get x509 public rsa modules and exponent. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucPublicKeyVal[IN]:public key.
    WE_UCHAR **ppucRsaModulus[OUT]:rsa modulus.
    WE_UINT16 *pusModLen[OUT]: the length of modulus
    WE_UCHAR **ppucRsaExponent[OUT]:rsa expoent.
    WE_UINT16 *pusExpLen[OUT]:length of expoent.
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
WE_INT32 We_X509GetRSAModAndExp(const WE_UCHAR *pucPublicKeyVal,
                                 WE_UCHAR **ppucRsaModulus, WE_UINT16 *pusModLen, 
                                 WE_UCHAR **ppucRsaExponent, WE_UINT16 *pusExpLen)
{    
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucPublicKeyVal, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucRsaModulus, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusModLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucRsaExponent, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusExpLen, 0x08);
    
    if (*pucPublicKeyVal == 0)                                        
    {
        pucPublicKeyVal++;
    }
    if(*pucPublicKeyVal != SEC_ASN_1_CONSTRUCTED_SEQUENCE)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPublicKeyVal++;
    
    We_X509DERtoSize((WE_UCHAR *)pucPublicKeyVal, &usTmpStep, &usSize);
    pucPublicKeyVal += usTmpStep;
    
    if(*pucPublicKeyVal != SEC_ASN_1_INTEGER)      
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPublicKeyVal++;
    
    We_X509DERtoSize((WE_UCHAR *)pucPublicKeyVal, &usTmpStep, &usSize);
    *pusModLen=usSize;
    pucPublicKeyVal += usTmpStep;    
    if (*pucPublicKeyVal == 0)                                        
    {
        pucPublicKeyVal++;
        *pusModLen-=1;
    }
    *ppucRsaModulus = (WE_UCHAR *)pucPublicKeyVal;
    pucPublicKeyVal += *pusModLen;    
    if(*pucPublicKeyVal != SEC_ASN_1_INTEGER)        
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPublicKeyVal++;
    
    We_X509DERtoSize((WE_UCHAR *)pucPublicKeyVal, &usTmpStep, &usSize);
    *pusExpLen=usSize;
    pucPublicKeyVal += usTmpStep;    
    if (*pucPublicKeyVal == 0)                                        
    {
        pucPublicKeyVal++;
        *pusExpLen-=1;
    }
    *ppucRsaExponent = (WE_UCHAR *)pucPublicKeyVal;
    pucPublicKeyVal += *pusExpLen;
    
    return SEC_ERR_OK;
}

#if 0	
/*==================================================================================================
FUNCTION: 
    We_X509GetDSAParams
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get DSA parameter. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucPublicKeyVal[IN]:
     WE_UCHAR **ppucPublicKey[OUT]:
     WE_UINT16 *pusPublicKeyLen[OUT]:
     WE_UCHAR **ppucDsaParams[OUT]:
     WE_UINT16 *pusDsaParamsLen[OUT]:
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
WE_INT32 
We_X509GetDSAParams(const WE_UCHAR *pucPublicKeyVal, 
                     WE_UCHAR **ppucPublicKey, WE_UINT16 *pusPublicKeyLen,
                     WE_UCHAR **ppucDsaParams, WE_UINT16 *pusDsaParamsLen)
{
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    WE_UCHAR *pucPtr = NULL;
    
    M_X509_CHECK_FOR_NULL_ARG(pucPublicKeyVal, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucPublicKey, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusPublicKeyLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucDsaParams, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusDsaParamsLen, 0x08);
    
    pucPtr = (WE_UCHAR *)pucPublicKeyVal;
    if(*pucPtr != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    *ppucDsaParams = pucPtr;                                    
    *pusDsaParamsLen += 1;
    pucPtr++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);    
    *pusDsaParamsLen = (WE_UINT16)((*pusDsaParamsLen) + usSize + usTmpStep);    
    pucPtr += usTmpStep;
    
    if(*pucPtr != SEC_ASN_1_INTEGER)            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPtr++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);    
    pucPtr += usTmpStep;                                        
    pucPtr += usSize;
    
    if(*pucPtr != SEC_ASN_1_INTEGER)            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPtr++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);    
    pucPtr += usTmpStep;                                        
    pucPtr += usSize;
    
    if(*pucPtr != SEC_ASN_1_INTEGER)            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPtr++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);    
    pucPtr += usTmpStep;                                        
    pucPtr += usSize;
    
    if(*pucPtr != SEC_ASN_1_BIT_STRING)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPtr++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);    
    pucPtr += usTmpStep;
    
    if (*pucPtr == 0)
    {
        pucPtr++;
        usSize--;
    }
    *pusPublicKeyLen = usSize;                            
    *ppucPublicKey = (WE_UCHAR *)pucPtr;
    
    return SEC_ERR_OK;
}  
#endif                      
/*==================================================================================================
FUNCTION: 
    We_X509GetSubjectPublicKeyInfo
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the infomation of public key. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucPublicKeyAlgId[OUT]:the id of alg 
    WE_UINT16 *pusPublicKeyAlgIdLen[OUT]:length of ppucPublicKeyAlgId.
    WE_UCHAR **publicKeyVal[OUT]:public key value.
    WE_UINT16 *pusPublicKeyValLen[OUT]: lenght of public key.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetSubjectPublicKeyInfo(const WE_UCHAR *pucX509Cert, 
                                             WE_UCHAR **ppucPublicKeyAlgId, WE_UINT16 *pusPublicKeyAlgIdLen, 
                                             WE_UCHAR **publicKeyVal, WE_UINT16 *pusPublicKeyValLen,
                                             WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    //#ifdef SEC_CFG_DHE_DSS
    WE_UCHAR *pucDsaParams=NULL;
    //#endif
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucPublicKeyAlgId, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusPublicKeyAlgIdLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(publicKeyVal, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusPublicKeyValLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);        
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE) 
        return SEC_ERR_BAD_CERTIFICATE;
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);        
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if(*pucCert != SEC_ASN_1_OBJECT_IDENTIFIER)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);
    *pusPublicKeyAlgIdLen=usSize;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    iRes = We_X509CheckOID(pucCert,*pusPublicKeyAlgIdLen, PUBLIC_KEY_OID);    
    if ( iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    *ppucPublicKeyAlgId = pucCert;
    pucCert += *pusPublicKeyAlgIdLen;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + (*pusPublicKeyAlgIdLen));
    
    if ((ppucPublicKeyAlgId[0][(*pusPublicKeyAlgIdLen)-2]) == 1)    
    {
        if (((*pucCert) == 5 ) && ((*(pucCert+1) == 0)))    
        {
            pucCert+=2;
            pusJumpStep[0]+=2;
        }
        else
        {
            return SEC_ERR_BAD_CERTIFICATE;  
        }
        if(*pucCert != SEC_ASN_1_BIT_STRING)                
        {
            return SEC_ERR_BAD_CERTIFICATE;
        }
        pucCert++;
        pusJumpStep[0]++;
        
        We_X509DERtoSize(pucCert, &usTmpStep, &usSize);
        pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
        pucCert += usTmpStep;
        pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
        if(0 == *pucCert)
        {
            pucCert++;
            usSize--;
        }
        *pusPublicKeyValLen = usSize;                                
        *publicKeyVal = pucCert;                                        
    }
    //#ifdef SEC_CFG_DHE_DSS	
    else if ((ppucPublicKeyAlgId[0][(*pusPublicKeyAlgIdLen)-2]) == 4)    
    {
        pucDsaParams = pucCert;
        (*pusPublicKeyValLen) = 0;
        if(*pucDsaParams != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
        {
            return SEC_ERR_BAD_CERTIFICATE;
        }
        pucDsaParams++;
        pusJumpStep[0]++;
        (*pusPublicKeyValLen)++;
        
        We_X509DERtoSize(pucDsaParams, &usTmpStep, &usSize);    
        pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
        pucDsaParams += usTmpStep;
        (*pusPublicKeyValLen) = (WE_UINT16)((*pusPublicKeyValLen) + usTmpStep);    
        pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
        pucDsaParams += usSize;
        (*pusPublicKeyValLen) = (WE_UINT16)((*pusPublicKeyValLen) + usSize);    
        
        if(*pucDsaParams != SEC_ASN_1_BIT_STRING)    
        {    
            return SEC_ERR_BAD_CERTIFICATE;
        }
        pucDsaParams++;
        pusJumpStep[0]++;
        (*pusPublicKeyValLen)++;    
        
        We_X509DERtoSize(pucDsaParams, &usTmpStep, &usSize);    
        (*pusPublicKeyValLen) = (WE_UINT16)((*pusPublicKeyValLen) + usSize + usTmpStep);    
        pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize + usTmpStep);
        pucDsaParams += usSize;
        pucDsaParams += usTmpStep;
        *publicKeyVal = pucCert;                                        
    }
    //#endif
    else
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509CheckUniqueIdentifier
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    check unique identifier.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509CheckUniqueIdentifier(const WE_UCHAR *pucX509Cert, WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;
    if((*pucCert != SEC_ASN_1_IMPLICIT_TAG1) && (*pucCert != SEC_ASN_1_IMPLICIT_TAG2))
    {
        return NO_OPTIONAL;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);                
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    pucCert +=usSize;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
    
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetExtensions1
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get extension.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucExtensions[OUT]:the extension of certificate.
    WE_UINT16 *pusExtensionsLen[OUT]: lenght of ppucExtensions.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetExtensions1(const WE_UCHAR *pucX509Cert, 
                                   WE_UCHAR **ppucExtensions, WE_UINT16 *pusExtensionsLen,
                                   WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usLength=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucExtensions, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusExtensionsLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;   
    if(*pucCert != SEC_ASN_1_IMPLICIT_TAG)            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);   
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);      
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    usLength=usSize;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
    *ppucExtensions = pucCert; 
    *pusExtensionsLen = usSize;

    usLength = usLength;
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetExtensions
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get extension.
ARGUMENTS PASSED:
    WE_UCHAR *pucExtensions[IN]:Pointer to the DER encoded extension in buffer
    WE_INT32 *piExtnId[OUT]:the id of extension.
    WE_INT32 *piCritical[OUT]:critical.
    WE_UCHAR **ppucExtnValue[OUT]:Place to put pointer to extension value.
    WE_UINT16 *pusExtnValueLen[OUT]:Place to length of extension value.
    WE_UINT16 *pusJumpStep[OUT]:the step of jump.
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
WE_INT32 
We_X509GetExtension(WE_UCHAR *pucExtensions, WE_INT32 *piExtnId, WE_INT32 *piCritical,
                     WE_UCHAR **ppucExtnValue, WE_UINT16 *pusExtnValueLen,
                     WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;    
    WE_UINT16 usSize=0;
    WE_UINT16 size2=0;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usOidLen=0;
    
    WE_UCHAR aucOidIdCe[2] = {85,29};          
    WE_UCHAR aucOidWapCe[3] = {103,43,3};      
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtensions, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piExtnId, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piCritical, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusExtnValueLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucExtensions;
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
        return SEC_ERR_BAD_CERTIFICATE;
    pucCert++;
    pusJumpStep[0] = 1;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);    
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
    pucCert +=usTmpStep;
    
    if(*pucCert != SEC_ASN_1_OBJECT_IDENTIFIER)    
        return SEC_ERR_BAD_CERTIFICATE;
    pucCert++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usOidLen);
    pucCert +=usTmpStep;
    *piExtnId = 0; 
    if ((usOidLen == 3) && (WE_MEMCMP(pucCert,aucOidIdCe,2) == 0)) 
    {
        switch (*(pucCert+2))
        {
        case 15:        
            *piExtnId = SEC_CERT_EXT_KEY_USAGE;
            break;
        case 37:  
            *piExtnId = SEC_CERT_EXT_EXTENDED_KEY_USAGE;
            break;           
        case 32:  
            *piExtnId = SEC_CERT_EXT_CERTIFICATE_POLICIES;
            break;    
        case 17:  
            *piExtnId = SEC_CERT_EXT_SUBJECT_ALT_NAME;
            break;        
        case 19:  
            *piExtnId = SEC_CERT_EXT_BASIC_CONSTRAINTS;
            break;       
        case 30:  
            *piExtnId = SEC_CERT_EXT_NAME_CONSTRAINTS;
            break;        
        case 36:  
            *piExtnId = SEC_CERT_EXT_POLICY_CONSTRAINTS;
            break;      
        case 35:  
            *piExtnId = SEC_CERT_EXT_AUTHORITY_KEY_IDENTIFIER;
            break;
        case 14:  
            *piExtnId = SEC_CERT_EXT_SUBJECT_KEY_IDENTIFIER;
            break;  
        default: 
            break;
        }
    } 
    else if ( (usOidLen == 4) && (WE_MEMCMP(pucCert,aucOidWapCe,3) == 0)) 
    {
        if (*(pucCert+3) == 1) 
        {
            *piExtnId = SEC_CERT_EXT_DOMAIN_INFORMATION;
        }
    }
    
    pucCert+=usOidLen;
    if (*pucCert != SEC_ASN_1_BOOLEAN) 
    {
        *piCritical = 0; 
    } 
    else 
    { 
        *piCritical = ((*(pucCert+1)) == 1) && ((*(pucCert+2)) != 0);
        pucCert+=3;
    }
    if(*pucCert != SEC_ASN_1_OCTET_STRING)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &size2);    
    pucCert +=usTmpStep;
    
    *ppucExtnValue = pucCert; 
    *pusExtnValueLen = size2;
    
    return SEC_ERR_OK; 
}
/*==================================================================================================
FUNCTION: 
    We_X509ExtnKeyUsage
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get extension.
ARGUMENTS PASSED:
    const WE_UCHAR *pucExtnValue[IN]:the value of exten.
    WE_UINT8 *pucKeyUsageBit0To7[OUT]:
    WE_UINT8 *pucKeyUsageBit8To15[OUT]:
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
WE_INT32 
We_X509ExtnKeyUsage(const WE_UCHAR *pucExtnValue, 
                     WE_UINT8 *pucKeyUsageBit0To7, 
                     WE_UINT8 *pucKeyUsageBit8To15) 
{
    WE_UCHAR *pucExtension = NULL;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    WE_UINT16 usUnUsedBits = 0;
    WE_UINT8 ucLastByte = 0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pucKeyUsageBit0To7, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pucKeyUsageBit8To15, 0x08);
    
    pucExtension = (WE_UCHAR *)pucExtnValue;
    *pucKeyUsageBit0To7 = 0; 
    *pucKeyUsageBit8To15 = 0;
    
    if(*pucExtension != SEC_ASN_1_BIT_STRING)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucExtension++;
    
    We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);
    pucExtension +=usTmpStep;
    if (usSize > 1)
    {
        usUnUsedBits  = *pucExtension;
        ucLastByte = 0xFF;                   
        ucLastByte <<= usUnUsedBits;                       
        ucLastByte &= *(pucExtension +usSize -1); 
        if (usSize == 2)
        {
            *pucKeyUsageBit0To7 = ucLastByte;
        }
        else
        {
            *pucKeyUsageBit0To7 = *(pucExtension + 1);
            *pucKeyUsageBit8To15 = ucLastByte;
        }
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509ExtnExtKeyUsage
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get extension.
ARGUMENTS PASSED:
    const WE_UCHAR *pucExtnValue[IN]: the value of exten.
    WE_INT32 *piAnyExtendedKeyUsage[OUT]:the usage of enten key.
    WE_INT32 *piServerAuth[OUT]:the server auth.
    WE_INT32 *piClientAuth[out]:the client auth.
    WE_INT32 *piCodeSigning[OUT]:the code signing.
    WE_INT32 *piEmailProtection[OUT]:the Email protection.
    WE_INT32 *piTimeStamping[OUT]:the time stamping.
    WE_INT32 *piOcspSigning[OUT]:the ocsp signing.
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
WE_INT32
We_X509ExtnExtKeyUsage(const WE_UCHAR *pucExtnValue, WE_INT32 *piAnyExtendedKeyUsage,
                        WE_INT32 *piServerAuth, WE_INT32 *piClientAuth, WE_INT32 *piCodeSigning,
                        WE_INT32 *piEmailProtection, WE_INT32 *piTimeStamping, WE_INT32 *piOcspSigning) 
{
    WE_UCHAR *pucExtension = NULL;
    
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    WE_UINT16 usOidLen=0;
    WE_UINT16 usLength=0;
    
    WE_UCHAR aucOidIdCe[2] = {85,29};          
    WE_UCHAR aucOidIdKp[7] = {43,6,1,5,5,7,3}; 
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piAnyExtendedKeyUsage, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piServerAuth, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piClientAuth, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piCodeSigning, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piEmailProtection, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piTimeStamping, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piOcspSigning, 0x08);
    
    pucExtension = (WE_UCHAR *)pucExtnValue;
    *piAnyExtendedKeyUsage = 0;
    *piServerAuth = 0;
    *piClientAuth = 0;
    *piCodeSigning = 0;
    *piEmailProtection = 0;
    *piTimeStamping = 0;
    *piOcspSigning = 0; 
    
    if(*pucExtension != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
        return SEC_ERR_BAD_CERTIFICATE;
    pucExtension++;
    We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);    
    pucExtension +=usTmpStep;
    usLength = usSize;
    
    while (usLength > 0)
    {
        if(*pucExtension != SEC_ASN_1_OBJECT_IDENTIFIER)
            return SEC_ERR_BAD_CERTIFICATE;
        pucExtension++;
        usLength--;
        
        We_X509DERtoSize(pucExtension, &usTmpStep, &usOidLen);
        pucExtension +=usTmpStep;
        usLength = (WE_UINT16)(usLength - usTmpStep);
        if ((usOidLen == 4) && (WE_MEMCMP(pucExtension,aucOidIdCe,2) == 0)) 
        {
            if ((*(pucExtension+2) == 37) &&(*(pucExtension+3) == 0))        
            {
                *piAnyExtendedKeyUsage = 1;
            }
        } 
        else if ((usOidLen == 8) && (WE_MEMCMP(pucExtension,aucOidIdKp,7) == 0))  
        {
            switch (*(pucExtension+7))
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
        pucExtension += usOidLen;
        usLength = (WE_UINT16)(usLength - usOidLen);
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509ExtnAuthorityKeyIdentifier
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the ifentifier of extension author key. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucExtnValue[IN]: the value of exten.
    WE_INT32 *piAnyExtendedKeyUsage[OUT]:the usage of enten key.
    WE_INT32 *piServerAuth[OUT]:the server auth.
    WE_INT32 *piClientAuth[out]:the client auth.
    WE_INT32 *piCodeSigning[OUT]:the code signing.
    WE_INT32 *piEmailProtection[OUT]:the Email protection.
    WE_INT32 *piTimeStamping[OUT]:the time stamping.
    WE_INT32 *piOcspSigning[OUT]:the ocsp signing.
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
WE_INT32 We_X509ExtnAuthorityKeyIdentifier(const WE_UCHAR *pucExtnValue, 
                                            WE_UCHAR** ppucKeyIdentifier,
                                            WE_UINT32* pusKeyIdentifierLen)
{
    WE_UCHAR *pucExtension = NULL;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucKeyIdentifier, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusKeyIdentifierLen, 0x08);
    
    pucExtension = (WE_UCHAR *)pucExtnValue;
    if(*pucExtension != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucExtension++;
    We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);  
    pucExtension +=usTmpStep;
    
    if (*pucExtension == SEC_ASN_1_EXPLICIT_TAG)       
    {
        pucExtension++;
        We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);
        pucExtension +=usTmpStep;
        
        if(*pucExtension != SEC_ASN_1_OCTET_STRING)            
        {
            return SEC_ERR_BAD_CERTIFICATE;
        }
        pucExtension++;
        
        We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);
        pucExtension +=usTmpStep;
        *ppucKeyIdentifier = pucExtension;
        *pusKeyIdentifierLen = usSize;
    }
    
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509ExtnSubjectKeyIdentifier
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the ifentifier . 
ARGUMENTS PASSED:
    const WE_UCHAR *pucExtnValue[IN]:the extent value.
    WE_UCHAR** ppucKeyIdentifier[OUT]:Identifier.
    WE_UINT32* pusKeyIdentifierLen[OUT]:Length of ppucKeyIdentifier.
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
WE_INT32 We_X509ExtnSubjectKeyIdentifier(const WE_UCHAR *pucExtnValue, 
                                          WE_UCHAR** ppucKeyIdentifier,
                                          WE_UINT32* pusKeyIdentifierLen)
{
    WE_UCHAR *pucExtension = NULL;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucKeyIdentifier, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusKeyIdentifierLen, 0x08);
    
    pucExtension = (WE_UCHAR *)pucExtnValue;
    if(*pucExtension != SEC_ASN_1_OCTET_STRING)            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucExtension++;
    
    We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);
    pucExtension +=usTmpStep;
    *ppucKeyIdentifier = pucExtension;
    *pusKeyIdentifierLen = usSize;
    
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509ExtnSubjectAltName
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get DNS and IP address . 
ARGUMENTS PASSED:
    const WE_UCHAR *pucExtnValue[IN]:the extent value.
    WE_UCHAR **ppucDnsName[OUT]:the name of DNS.
    WE_UINT32 *pusDnsNameLen[OUT]:the length of ppucDnsName.
    WE_UCHAR **ppucIpAddress[OUT]:the address of ip.
    WE_UINT16 *pusIpAddressLen[OUT]:the length of ppucIpAddress.
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
WE_INT32 We_X509ExtnSubjectAltName(const WE_UCHAR *pucExtnValue, 
                                    WE_UCHAR **ppucDnsName,WE_UINT32 *pusDnsNameLen,
                                    WE_UCHAR **ppucIpAddress,WE_UINT16 *pusIpAddressLen)
{
    WE_UCHAR *pucExtension = NULL;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    WE_UINT16 usSize2=0;
    WE_UINT16 usLength = 0;
    WE_UCHAR ucTag = 0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucDnsName, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusDnsNameLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucIpAddress, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusIpAddressLen, 0x08);
    
    pucExtension = (WE_UCHAR *)pucExtnValue;
    *ppucDnsName = 0;
    *pusDnsNameLen = 0;
    *ppucIpAddress = 0;
    *pusIpAddressLen = 0;
    
    if(*pucExtension != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucExtension++;
    We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);  
    pucExtension +=usTmpStep;
    usLength = usSize;
    while (usLength > 0)
    {
        ucTag = (WE_UCHAR)((*pucExtension) & 31);
        pucExtension++; 
        usLength--;
        We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);
        pucExtension +=usTmpStep;
        usLength = (WE_UINT16)(usLength - usTmpStep);
        switch (ucTag)
        {  
        case 2:  
            {
                *ppucDnsName = pucExtension;
                *pusDnsNameLen = usSize;
            }
            break;
        case 7:  
            {
                if(*pucExtension != SEC_ASN_1_OCTET_STRING)
                {
                    return SEC_ERR_BAD_CERTIFICATE;
                }
                pucExtension++;
                We_X509DERtoSize(pucExtension, &usTmpStep, &usSize2);    
                pucExtension +=usTmpStep;
                *ppucIpAddress = pucExtension;
                *pusIpAddressLen = usSize2;
            }
            break;
        default :
            break;  
        }
        pucExtension += usSize;
        usLength = (WE_UINT16)(usLength - usSize);
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509ExtnBasicConstraints
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    Get basic constraints 
ARGUMENTS PASSED:
    const WE_UCHAR *pucExtnValue  [IN]: Pointer to the DER encoded extension value in buffer.
    WE_INT32 *piCa                [OUT]: Place to whether a ca certificat.
    WE_INT32 *piHasPathLenConstraint[OUT]: Place to whether has the path length.
    WE_UINT8 *pucPathLenConstraint[OUT]: Place to the length of path.
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
WE_INT32 We_X509ExtnBasicConstraints(const WE_UCHAR *pucExtnValue, WE_INT32 *piCa,
                                      WE_INT32 *piHasPathLenConstraint,
                                      WE_UINT8 *pucPathLenConstraint)
{
    WE_UCHAR *pucExtension = NULL;
    WE_UINT16 usTmpStep = 0;
    WE_UINT16 usSize = 0;
    WE_UINT16 usLength = 0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piCa, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piHasPathLenConstraint, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pucPathLenConstraint, 0x08);
    
    pucExtension = (WE_UCHAR *)pucExtnValue;
    *piCa = 0; 
    *piHasPathLenConstraint = 0;  
    
    if(*pucExtension != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucExtension++;
    We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);    
    pucExtension +=usTmpStep;
    usLength = usSize;
    
    if ((usLength == 0) || (*pucExtension != SEC_ASN_1_BOOLEAN))  
    {
        
        return SEC_ERR_OK;
    }
    *piCa = ((*(pucExtension+1)) == 1) && ((*(pucExtension+2)) != 0);
    pucExtension+=3;
    usLength -=3;
    
    if ((usLength > 0))                       
    {
        *piHasPathLenConstraint = 1;
        if(*pucExtension != SEC_ASN_1_INTEGER)        
        {
            return SEC_ERR_BAD_CERTIFICATE;
        }
        pucExtension++;
        We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);    
        pucExtension += usTmpStep;
        if (usSize > 1) 
        {            
            *pucPathLenConstraint = 127;
        }
        else 
        {
            *pucPathLenConstraint = *pucExtension;
        }
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetSignature
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    Get signature
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucSignature[OUT]:signature.
    WE_UINT16 *pusSignatureLen[OUT]:length of  ppucSignature.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetSignature(const WE_UCHAR *pucX509Cert, 
                                  WE_UCHAR **ppucSignature, WE_UINT16 *pusSignatureLen, 
                                  WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucSignature, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusSignatureLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;       
    if(*pucCert != SEC_ASN_1_BIT_STRING)    
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);    
    *pusSignatureLen = usSize;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert += usTmpStep;
    
    if (*pucCert == 0)                                        
    {
        pucCert++;
        pusJumpStep[0]++;
        *pusSignatureLen -= 1;
    }
    
    *ppucSignature = pucCert;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + (*pusSignatureLen));
    
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509CheckIssuerOID
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    check issuer OID.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucStr[OUT]:
    WE_UINT16 usOidLen[IN]:
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
static WE_INT32 We_X509CheckIssuerOID(const WE_UCHAR *pucX509Cert, WE_UCHAR **ppucStr, WE_UINT16 usOidLen)
{
    const WE_UCHAR aucOid2[]="\x55\x4";
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usIndex=0;     
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucStr, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    if (usOidLen==3)
    {
        for (usIndex=0;usIndex<(usOidLen-1);usIndex++)
        {
            if(*pucCert==aucOid2[usIndex])            
            {
                pucCert++;
            }
            else
            {
                return SEC_ERR_BAD_CERTIFICATE;
            }
        }
        if (*pucCert==3)
        {    
            *ppucStr=(WE_UCHAR *)"CN=\0";
            return iRes;
        }
        if (*pucCert==6)    
        {              
            *ppucStr=(WE_UCHAR *)"C=\0";
            return iRes;
        }
        if (*pucCert==7)
        {
            *ppucStr=(WE_UCHAR *)"L=\0";
            return iRes;
        }
        if (*pucCert==8)                  
        {
            *ppucStr=(WE_UCHAR *)"S=\0";
            return iRes;
        }
        if (*pucCert==10)    
        {
            *ppucStr=(WE_UCHAR *)"O=\0";
            return iRes;
        }
        if (*pucCert==11)          
        {
            *ppucStr=(WE_UCHAR *)"OU=\0";
            return iRes;
        }
    }
    
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetIssuerString
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get issuer string.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR **ppucIssuerString[OUT]:issuer.
    WE_UINT16 *pusIssuerStringLen[OUT]:length of  ppucIssuerString.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetIssuerString(const WE_UCHAR *pucX509Cert, 
                                 WE_UCHAR **ppucIssuerString, 
                                 WE_UINT16 *pusIssuerStringLen, 
                                 WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    WE_UCHAR *tmpStr=NULL;
    WE_UCHAR tmpStr2[65]={0};
    WE_UCHAR tmpStr1[170]={0};
    WE_INT16 sIndex=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucIssuerString, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusIssuerStringLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    pusJumpStep[0] = 0;
    *pusIssuerStringLen=0;    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE_OF)        
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);            
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)  
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);            
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    if(*pucCert != SEC_ASN_1_OBJECT_IDENTIFIER)  
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);    
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    iRes=We_X509CheckIssuerOID((const WE_UCHAR *)pucCert, &tmpStr, usSize);
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
    pucCert += usSize;    
    
    if(!((*pucCert == SEC_ASN_1_PRINTTABLESTRING) || (*pucCert == SEC_ASN_1_UTF8STRING) \
        ||(*pucCert == SEC_ASN_1_NUMERICSTRING)))            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucCert +=usTmpStep;
    
    for (sIndex=0;sIndex<usSize;sIndex++)
    {    
        tmpStr2[sIndex]=*pucCert;
        pucCert++;
    }
    tmpStr2[sIndex]='\0';
    (WE_VOID)WE_STRCPY((char *)tmpStr1,(const char *)tmpStr);
    (WE_VOID)WE_STRCAT((char *)tmpStr1,(const char *)tmpStr2);
//    *ppucIssuerString = tmpStr1;
    *pusIssuerStringLen = (WE_UINT16)SEC_STRLEN((const char *)tmpStr1);
    (WE_VOID)WE_MEMCPY(*ppucIssuerString, tmpStr1, *pusIssuerStringLen);
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
    pucCert += usSize;
    
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_X509CheckAndGetIssuerOID
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get issuer oid.
ARGUMENTS PASSED:
    const WE_UCHAR *oidString[IN]:string of OID.
    WE_UINT32 *oid[OUT]:oid.
    WE_UINT16 usOidLen[OUT]:length of oid.
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
WE_INT32 We_X509CheckAndGetIssuerOID(const WE_UCHAR *oidString, 
                              WE_UINT32 *oid, WE_UINT16 usOidLen)
{
    WE_UCHAR aucOidIdAt[2] = {85, 4};         
    WE_UCHAR aucOidEmail[9] = {42, 134, 72, 134, 247, 13, 1, 9, 1};
    WE_UCHAR aucOidDomainc[10] = {9, 146, 38, 137, 147, 242, 44, 100, 1, 3};
    WE_UCHAR *pucPtr = NULL;    
    
    M_X509_CHECK_FOR_NULL_ARG(oidString, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(oid, 0x08);
    
    pucPtr = (WE_UCHAR *)oidString;    
    if ((usOidLen == 3) && (WE_MEMCMP(pucPtr,aucOidIdAt,2) == 0)) 
    {
        switch (*(pucPtr+2))
        {
        case 3: 
            *oid = SEC_COMMON_NAME;
            break;
        case 4: 
            *oid = SEC_SURNAME;
            break;
        case 5: 
            *oid = SEC_SERIAL_NUMBER;
            break;
        case 6: 
            *oid = SEC_COUNTRY_NAME;
            break;
        case 7: 
            *oid = SEC_LOCALITY_NAME;
            break;
        case 8: 
            *oid = SEC_STATE_OR_PROVINCE_NAME;
            break;
        case 10: 
            *oid = SEC_ORGANISATION_NAME;
            break;
        case 11: 
            *oid = SEC_ORGANISATIONAL_UNIT_NAME;
            break;
        case 12: 
            *oid = SEC_TITLE;
            break;
        case 41:  
            *oid = SEC_NAME;
            break;
        case 42: 
            *oid = SEC_GIVENNAME;
            break;
        case 43: 
            *oid = SEC_INITIALS;
            break;
        case 44: 
            *oid = SEC_GENERATION_QUALIFIER;
            break;
        case 46:  
            *oid = SEC_DN_QUALIFIER;
            break;
        default: 
            *oid = SEC_UNKNOWN_ATTRIBUTE_TYPE;
            break;
        }
    }
    else if ((usOidLen == 9) && (WE_MEMCMP(pucPtr,aucOidEmail,9) == 0))  
    {
        *oid = SEC_EMAIL_ADDRESS;
    }
    else if ((usOidLen == 10) && (WE_MEMCMP(pucPtr,aucOidDomainc,10) == 0)) 
    {
        *oid = SEC_DOMAIN_COMPONENT;
    }
    else
    {
        *oid = SEC_UNKNOWN_ATTRIBUTE_TYPE;
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetNbrIssuerStrings
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get issuer string.
ARGUMENTS PASSED:
    const WE_UCHAR *pucIssuer[IN]:issuer.
    WE_UINT16 usIssuerLen[IN]:length of pucIssuer.
    WE_UINT8 *pucNbrIssuerStrings[OUT]:the string of issuer.
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
WE_INT32 
We_X509GetNbrIssuerStrings(const WE_UCHAR *pucIssuer, WE_UINT16 usIssuerLen, 
                            WE_UINT8 *pucNbrIssuerStrings)
{
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usSize=0;
    WE_UINT16 usLength=0;
    WE_UINT16 usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucIssuer, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pucNbrIssuerStrings, 0x08);
    
    pucCert = (WE_UCHAR *)pucIssuer;
    *pucNbrIssuerStrings=0;    
    if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)          
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucCert++;
    
    We_X509DERtoSize(pucCert, &usTmpStep, &usSize);                
    usLength = (WE_UINT16)(usIssuerLen - (usTmpStep+1)); 
    pucCert +=usTmpStep;
    
    while(usLength>0)
    {
        if(*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE_OF)
        {
            return SEC_ERR_BAD_CERTIFICATE;
        }
        pucCert++;        
        We_X509DERtoSize(pucCert, &usTmpStep, &usSize);            
        pucCert +=usTmpStep;        
        pucCert += usSize;
        usLength = (WE_UINT16)(usLength - (usSize+usTmpStep+1));    
        *pucNbrIssuerStrings += 1;
    }
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_X509GetAttributeString                         
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the string of attribute.
ARGUMENTS PASSED:
    const WE_UCHAR *pucIssuerPart[IN]:the Issuer part.
    WE_VOID **ppvAttributeValue[OUT]:the value of attribute.
    WE_UINT16 *pusAttributeValueLen[OUT]:lenght of ppvAttributeValue.
    WE_INT16 *psAttrCharSet[OUT]:charset.
    WE_UINT32 *puiAttributeType[OUT]:the type of attribute.
    WE_UINT16 *pusJumpStep[OUT]:jump step.
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
WE_INT32 We_X509GetAttributeString(const WE_UCHAR *pucIssuerPart, 
                                        WE_VOID **ppvAttributeValue, 
                                        WE_UINT16 *pusAttributeValueLen,
                                        WE_INT16 *psAttrCharSet, 
                                        WE_UINT32 *puiAttributeType,
                                        WE_UINT16 *pusJumpStep)
{
    WE_UCHAR *pucPtr = NULL;
    WE_INT32 iRes = SEC_ERR_OK;
    WE_UINT16 usSize=0;
    WE_UINT16 usTmpStep=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucIssuerPart, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppvAttributeValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusAttributeValueLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(psAttrCharSet, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(puiAttributeType, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusJumpStep, 0x08);
    
    pucPtr = (WE_UCHAR *)pucIssuerPart;
    pusJumpStep[0] = 0;
    *pusAttributeValueLen=0;
    
    if(*pucPtr != SEC_ASN_1_CONSTRUCTED_SEQUENCE_OF) 
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPtr++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);        
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucPtr +=usTmpStep;
    
    if(*pucPtr != SEC_ASN_1_CONSTRUCTED_SEQUENCE)  
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPtr++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);        
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucPtr +=usTmpStep;
    
    if(*pucPtr != SEC_ASN_1_OBJECT_IDENTIFIER) 
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucPtr++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucPtr +=usTmpStep;
    
    iRes = We_X509CheckAndGetIssuerOID((const WE_UCHAR *)pucPtr, puiAttributeType, usSize);
    
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
    pucPtr += usSize; 
    *psAttrCharSet = *pucPtr;
    pucPtr++;
    pusJumpStep[0]++;
    
    We_X509DERtoSize(pucPtr, &usTmpStep, &usSize);
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usTmpStep);
    pucPtr +=usTmpStep;
    *pusAttributeValueLen = usSize;
    *ppvAttributeValue = pucPtr;
    pucPtr += usSize;
    pusJumpStep[0] = (WE_UINT16)(pusJumpStep[0] + usSize);
    
    return iRes;
}            
/*==================================================================================================
FUNCTION: 
    We_X509GetIssuerAttributes                         
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the attribute of Issuer.
ARGUMENTS PASSED:
    const WE_UCHAR *pucIssuer[IN]:Issuer.
    WE_UINT16 usIssuerLen[IN]: length of Issuer.
    WE_UINT8 *nbrIssuerAttributes[OUT]:the attribute of Issuer.
    st_SecNameAttribute *issuerA[OUT]: the name of issuer.
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
/*
WE_INT32
We_X509GetIssuerAttributes(const WE_UCHAR *pucIssuer, 
                            WE_UINT16 usIssuerLen, 
                            WE_UINT8 *nbrIssuerAttributes,                                
                            st_SecNameAttribute *issuerA)
{
    
    WE_UINT8 nbrIssuerStr = 0;
    WE_INT32 iResult = SEC_ERR_OK;
    WE_INT32 iIndex = 0;
    WE_INT32 k = 0;
    WE_UCHAR *tmp_issuer = NULL;
    WE_UINT16 usSize = 0;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usJumpStep = 0;
    
    WE_UCHAR *tmp_attributeV = NULL;
    WE_UINT16 tmp_attributeVLen = 0;
    WE_INT16 tmp_charSet = 0;
    WE_UINT32 tmp_attributeType = 0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucIssuer, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(nbrIssuerAttributes, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(issuerA, 0x08);
    
    tmp_issuer = (WE_UCHAR *)pucIssuer;
    iResult = We_X509GetNbrIssuerStrings(pucIssuer, usIssuerLen, &nbrIssuerStr);
    *nbrIssuerAttributes = nbrIssuerStr;
    
    if (iResult != SEC_ERR_OK)
    {
        return iResult;
    }
    if(*tmp_issuer != SEC_ASN_1_CONSTRUCTED_SEQUENCE)   
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    tmp_issuer++;    
    We_X509DERtoSize(tmp_issuer, &usTmpStep, &usSize);            
    tmp_issuer +=usTmpStep;
    
    for (iIndex=0; iIndex<nbrIssuerStr; iIndex++)
    {
        iResult = We_X509GetAttributeString((const WE_UCHAR *)tmp_issuer, \
            (WE_VOID**)&tmp_attributeV, &tmp_attributeVLen,\
            &tmp_charSet, &tmp_attributeType,&usJumpStep);
        if (iResult != SEC_ERR_OK)
        {
            for (k=0; k<iIndex; k++)
            {
                WE_FREE( issuerA[k].attributeValue);
            }
            *nbrIssuerAttributes = 0;
            return iResult;
        }          
        tmp_issuer += usJumpStep;
        issuerA[iIndex].attributeCharSet = tmp_charSet;
        issuerA[iIndex].attributeType = tmp_attributeType;
        issuerA[iIndex].attributeValueLen = tmp_attributeVLen;
        issuerA[iIndex].attributeValue = (WE_UCHAR *) 
            WE_MALLOC ((tmp_attributeVLen+2)*sizeof(WE_UCHAR)); 
        
        (WE_VOID)WE_MEMCPY(issuerA[iIndex].attributeValue,tmp_attributeV,tmp_attributeVLen);
        issuerA[iIndex].attributeValue[tmp_attributeVLen] = '\0';
    }
    return iResult;
}*/
/*==================================================================================================
FUNCTION: 
    We_X509GetTBSCert                         
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the attribute of Issuer.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UCHAR *pucTbsCert[OUT]:tbs certifcate.
    WE_UINT16 *pusTbsCertLen[OUT]: length of pucTbsCert.
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
WE_INT32 We_X509GetTBSCert(const WE_UCHAR *pucX509Cert,
                            WE_UCHAR *pucTbsCert, WE_UINT16 *pusTbsCertLen)
{
    WE_INT32 iRes;
    WE_UINT16 usJumpStep = 0;
    WE_UINT16 certLen = 0;
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iIndex;    
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pucTbsCert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusTbsCertLen, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    iRes = We_X509GetAndCheckLength((const WE_UCHAR *)pucCert, &certLen, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    
    
    iRes = We_X509GetAndCheckTBSCertLength((const WE_UCHAR *)pucCert, pusTbsCertLen, \
        certLen, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    (*pusTbsCertLen) = (WE_UINT16)((*pusTbsCertLen) + usJumpStep);
    for (iIndex=0; iIndex<(*pusTbsCertLen); iIndex++)
    {
        pucTbsCert[iIndex] = pucCert[iIndex];
    }
    return SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_X509Parse                         
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    parse the x509 certificate.
ARGUMENTS PASSED:
    const WE_UCHAR *pucX509Cert[IN]:x509 certificate.
    WE_UINT16 *pusCertLength[OUT]:Place to length of certificate.
    St_SecCertificate *pstCert[OUT]:Place to parse information of certification.
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
WE_INT32 We_X509Parse(const WE_UCHAR *pucX509Cert, WE_UINT16 *pusCertLength,
                       St_SecCertificate *pstCert)
{
    WE_UCHAR *pucCert = NULL;
    WE_INT32 iRes=0;
    WE_UINT16 usJumpStep = 0;
    WE_UINT16 usTbsCertLen = 0;
    WE_INT32 iIndex=0;
    WE_INT32 iMatch = 0;
    WE_UCHAR *pucTmp1=NULL;
    WE_UCHAR *pucTmp2=NULL;
    WE_UCHAR *pucTmpExt=NULL;
    WE_UINT16 usTmpExLen=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucX509Cert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusCertLength, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pstCert, 0x08);
    
    pucCert = (WE_UCHAR *)pucX509Cert;
    iRes = We_X509GetAndCheckLength((const WE_UCHAR *)pucCert, pusCertLength, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;    
    
    iRes = We_X509GetAndCheckTBSCertLength((const WE_UCHAR *)pucCert, &usTbsCertLen, 
        *pusCertLength, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    /*get TBS*/
    pstCert->stCertUnion.stX509.pucTbsCert=pucCert;
    pstCert->stCertUnion.stX509.uiTbsCertLen = (WE_UINT16)((usTbsCertLen) + usJumpStep);    
    
    pucCert += usJumpStep;   
    /*get version*/
    iRes= We_X509GetVersion((const WE_UCHAR *)pucCert, &(*pstCert).ucCertificateVersion, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;    
    /*get serialNumber*/
    iRes= We_X509GetSerialNumber((const WE_UCHAR *)pucCert, &(*pstCert).stCertUnion.stX509.pucSerialNumber, 
        &(*pstCert).stCertUnion.stX509.usSerialNumberLen, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    
    
    iRes= We_X509GetSignatureAlgId((const WE_UCHAR *)pucCert, &(*pstCert).stCertUnion.stX509.pucSignatureAlgId, 
        &(*pstCert).stCertUnion.stX509.usSignatureAlgIdLen, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    
    if ((*pucCert == 5) && (*(pucCert+1) == 0))
    {
        pucCert += 2;    
    }
    iRes= We_X509GetIssuer((const WE_UCHAR *)pucCert, &(*pstCert).pucIssuer, &(*pstCert).stCertUnion.stX509.usIssuerLen, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    
    
    iRes= We_X509GetValidity((const WE_UCHAR *)pucCert, &(*pstCert).iValidNotBefore, &(*pstCert).iValidNotAfter, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    
    
    iRes= We_X509GetSubject((const WE_UCHAR *)pucCert, &(*pstCert).pucSubject, &(*pstCert).stCertUnion.stX509.usSubjectLen, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    
    
    iRes= We_X509GetSubjectPublicKeyInfo((const WE_UCHAR *)pucCert, 
        &(*pstCert).stCertUnion.stX509.pucPublicKeyAlgId, &(*pstCert).stCertUnion.stX509.usPublicKeyAlgIdLen,        
        &(*pstCert).stCertUnion.stX509.pucPublicKeyVal, &(*pstCert).stCertUnion.stX509.usPublicKeyValLen, &usJumpStep);
    
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;    
    
    
    if (*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        iRes=We_X509CheckUniqueIdentifier((const WE_UCHAR *)pucCert, &usJumpStep);
        if (iRes != NO_OPTIONAL)
        {
            pucCert += usJumpStep;
        }
    }
    
    if (*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        iRes=We_X509CheckUniqueIdentifier((const WE_UCHAR *)pucCert, &usJumpStep);
        if (iRes != NO_OPTIONAL)
        {
            pucCert += usJumpStep;
        }
    }
    
    (*pstCert).stCertUnion.stX509.usExtensionsLen = 0;
    (*pstCert).stCertUnion.stX509.pucExtensions = NULL;    
    
    
    if (*pucCert != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
    {
        iRes= We_X509GetExtensions1((const WE_UCHAR *)pucCert, \
            &(*pstCert).stCertUnion.stX509.pucExtensions, \
            &(*pstCert).stCertUnion.stX509.usExtensionsLen, &usJumpStep);
        if (iRes != SEC_ERR_OK)
        {
            return iRes;
        }
        pucCert += usJumpStep;
    }
    
    
    iRes= We_X509GetSignatureAlgId((const WE_UCHAR *)pucCert,\
        &(*pstCert).stCertUnion.stX509.pucSignatureAlgorithm, \
        &(*pstCert).stCertUnion.stX509.usSignatureAlgorithmLen,&usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    
    if ((*pucCert == 5) && (*(pucCert+1) == 0))
    {
        pucCert += 2;
    }
    
    if ((pstCert->stCertUnion.stX509.usSignatureAlgIdLen)== (pstCert->stCertUnion.stX509.usSignatureAlgorithmLen))
    { 
        iMatch = 1;
        pucTmp1 = pstCert->stCertUnion.stX509.pucSignatureAlgId;
        pucTmp2 = pstCert->stCertUnion.stX509.pucSignatureAlgorithm;
        for (iIndex=0; iIndex<(pstCert->stCertUnion.stX509.usSignatureAlgIdLen); iIndex++)
        {  
            if ((*pucTmp1) != (*pucTmp2))                         
            {
                iMatch = 0;
                break;
            } 
            pucTmp1++;
            pucTmp2++;
        }    
    }
    if (iMatch == 0)
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    
    iRes= We_X509GetSignature((const WE_UCHAR *)pucCert, &(*pstCert).pucSignature, &(*pstCert).usSignatureLen, &usJumpStep);
    if (iRes != SEC_ERR_OK)
    {
        return iRes;
    }
    pucCert += usJumpStep;
    /*get rsa*/
    iRes= We_X509GetRSAModAndExp(pstCert->stCertUnion.stX509.pucPublicKeyVal,
        &(*pstCert).stCertUnion.stX509.pucModulus, \
        &(*pstCert).stCertUnion.stX509.usModulusLen,\
        &(*pstCert).stCertUnion.stX509.pucExponent,\
        &(*pstCert).stCertUnion.stX509.usExponentLen);
    
    pucTmpExt=pstCert->stCertUnion.stX509.pucExtensions;
    usTmpExLen=pstCert->stCertUnion.stX509.usExtensionsLen; 
    
    /*get extension*/
    if(pstCert->ucCertificateVersion==3)
    {
        WE_INT32        iExtnId=0;
        WE_INT32        iCritical=0;
        WE_UCHAR*       pucExtnValue=NULL;
        WE_UINT16       usExtnValueLen = 0;
        WE_UCHAR *      pucIpAddress=NULL;
        WE_UINT16       usIpAddressLen=0;  
        
        while (usTmpExLen > 0)
        {
            iRes = We_X509GetExtension(pucTmpExt,\
                &iExtnId, &iCritical,&pucExtnValue,&usExtnValueLen, &usJumpStep);
            if (iRes != SEC_ERR_OK)
            {
                return iRes;
            }
            switch (iExtnId)
            {  
            case SEC_CERT_EXT_KEY_USAGE:/*get EX_key usage*/
                {  
                    (*pstCert).stCertUnion.stX509.pucExtKeyUsage=pucExtnValue;
                    (*pstCert).stCertUnion.stX509.uiExtKeyUsageLen=usExtnValueLen;                    
                }       
                break;    
            case SEC_CERT_EXT_EXTENDED_KEY_USAGE:    
                {    
                    (*pstCert).stCertUnion.stX509.pucExtExtKeyUsage=pucExtnValue;
                    (*pstCert).stCertUnion.stX509.uiExtExtKeyUsageLen=usExtnValueLen; 
                }    
                break;
            case SEC_CERT_EXT_BASIC_CONSTRAINTS:  
                {
                    (*pstCert).stCertUnion.stX509.pucExtBasicConstraint=pucExtnValue;
                    (*pstCert).stCertUnion.stX509.uiExtBasicConstraintLen=usExtnValueLen; 
                }
                break;
            case SEC_CERT_EXT_DOMAIN_INFORMATION:
            case SEC_CERT_EXT_CERTIFICATE_POLICIES:   
                break;
            case SEC_CERT_EXT_SUBJECT_ALT_NAME:                  
                {
                    iRes=We_X509ExtnSubjectAltName(pucExtnValue, \
                        &(*pstCert).stCertUnion.stX509.pucExtSubjectAltName,\
                        &(*pstCert).stCertUnion.stX509.uiExtSubjectAltNameLen,\
                        &pucIpAddress,&usIpAddressLen);
                    if (iRes != SEC_ERR_OK)  
                    {  
                        return iRes;  
                    }
                }
                break;
            case SEC_CERT_EXT_NAME_CONSTRAINTS:        
            case SEC_CERT_EXT_POLICY_CONSTRAINTS: 
                break;
            case SEC_CERT_EXT_AUTHORITY_KEY_IDENTIFIER:
                iRes= We_X509ExtnAuthorityKeyIdentifier(pucExtnValue, \
                    &(*pstCert).stCertUnion.stX509.pucExtAuthKeyId,\
                    &(*pstCert).stCertUnion.stX509.uiExtAuthKeyIdLen);
                if (iRes != SEC_ERR_OK)  
                {      
                    return iRes;  
                }
                break;
            case SEC_CERT_EXT_SUBJECT_KEY_IDENTIFIER:  
                iRes= We_X509ExtnSubjectKeyIdentifier(pucExtnValue, \
                    &(*pstCert).stCertUnion.stX509.pucExtSubjectKeyId,\
                    &(*pstCert).stCertUnion.stX509.uiExtSubjectKeyIdLen);
                if (iRes != SEC_ERR_OK)  
                { 
                    return iRes;  
                }
                break;    
            default: 
                if (iCritical)    
                { 
                    return SEC_ERR_BAD_CERTIFICATE;  
                }   
            }     
            pucTmpExt += usJumpStep;
            usTmpExLen = (WE_UINT16)(usTmpExLen - usJumpStep);        
        }    
    }
    
    return SEC_ERR_OK;
}

#if 0
WE_INT32 
We_X509ParseTBHTrustedCAInfo(const WE_UCHAR *tBHTrusedCAInfo,
                              WE_INT16 *psCharacterSet, 
                              WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen,
                              WE_UCHAR **pucCert, WE_UINT16 *certLen)
{
    WE_UCHAR *pucTmpPtr = (WE_UCHAR *)tBHTrusedCAInfo;
    WE_INT32 iRes;
    WE_UINT16 usJumpStep = 0;
    WE_UINT16 usCharacterSet = 0;
    WE_UINT8 cLen = 0;
    
    
    if(*pucTmpPtr != 1)
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    pucTmpPtr++;
    
    
    We_UTILSGetUINT16((const WE_UCHAR *)pucTmpPtr, &usCharacterSet);
    *psCharacterSet = (WE_INT16) usCharacterSet;
    pucTmpPtr += 2;
    
    *piDisplayNameLen = *pucTmpPtr;
    pucTmpPtr++;
    
    *ppvDisplayName = (WE_VOID*) pucTmpPtr;
    pucTmpPtr += *piDisplayNameLen;
    
    *pucCert = pucTmpPtr;   
    
    switch (*pucTmpPtr)
    {
    case 1: 
        iRes= SEC_wtlsCheckLengthAndFormat(pucTmpPtr+1, certLen);
        (*certLen) +=1;
        pucTmpPtr += *certLen;    
        break;
    case 2:
        iRes= We_X509GetAndCheckLength(pucTmpPtr+1, certLen, &usJumpStep);
        (*certLen) = (WE_UINT16)((*certLen) + (usJumpStep+1));
        pucTmpPtr += *certLen;
        break;
    default:
        return SEC_ERR_BAD_CERTIFICATE;
    }
    
    if ((*pucTmpPtr) != 0)
    {        
        cLen = *pucTmpPtr;
        pucTmpPtr += cLen+1;
    }
    
    
    if (*pucTmpPtr != 0)
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    return SEC_ERR_OK;
}


WE_INT32 We_X509ParseSignedTrustedCAInfo(const WE_UCHAR *pucSignedTrustedCAInfo,
                                          WE_INT16 *psCharacterSet, 
                                          WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen,
                                          WE_UCHAR **ppucTrustedCAcert, 
                                          WE_UINT16 *pusTrustedCAcertLen,
                                          WE_UCHAR **ppucSignerCert, WE_UINT16 *pusSignerCertLen,
                                          WE_UINT8 *pucSignatureAlg, WE_UCHAR **ppucSignature, 
                                          WE_UINT16 *pusSignatureLen)
{
    
    WE_UCHAR *pucTmpPtr = (WE_UCHAR *)pucSignedTrustedCAInfo;
    WE_INT32 iRes;
    WE_UINT16 usJumpStep = 0;
    WE_UINT16 usCharacterSet = 0;
    WE_UINT8 cLen = 0;    
    
    if(*pucTmpPtr != 1)
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    pucTmpPtr++;
    
    
    We_UTILSGetUINT16((const WE_UCHAR *)pucTmpPtr, &usCharacterSet);
    *psCharacterSet = (WE_INT16) usCharacterSet;
    pucTmpPtr += 2;
    
    *piDisplayNameLen = *pucTmpPtr;
    pucTmpPtr++;
    
    *ppvDisplayName = (WE_VOID*) pucTmpPtr;
    pucTmpPtr += *piDisplayNameLen;
    
    *ppucTrustedCAcert = pucTmpPtr;
    
    
    switch (*pucTmpPtr)
    {
    case 1: 
        iRes= SEC_wtlsCheckLengthAndFormat(pucTmpPtr+1, pusTrustedCAcertLen);
        (*pusTrustedCAcertLen) +=1;
        pucTmpPtr += *pusTrustedCAcertLen;
        break;
    case 2:
        iRes= We_X509GetAndCheckLength(pucTmpPtr+1, pusTrustedCAcertLen, &usJumpStep);
        (*pusTrustedCAcertLen) = (WE_UINT16)((*pusTrustedCAcertLen) + (usJumpStep+1));
        pucTmpPtr += *pusTrustedCAcertLen;
        break;
    default:
        return SEC_ERR_BAD_CERTIFICATE;
    }
    
    
    if ((*pucTmpPtr) != 0)
    {
        cLen = *pucTmpPtr;
        pucTmpPtr += cLen +1;
    }
    
    *ppucSignerCert = pucTmpPtr;
    
    switch (*pucTmpPtr)
    {
    case 1: 
        iRes= SEC_wtlsCheckLengthAndFormat(pucTmpPtr+1, pusSignerCertLen);
        (*pusSignerCertLen) +=1;
        pucTmpPtr += *pusSignerCertLen;
        break;
    case 2:
        iRes= We_X509GetAndCheckLength(pucTmpPtr+1, pusSignerCertLen, &usJumpStep);
        (*pusSignerCertLen) = (WE_UINT16)((*pusSignerCertLen) + (usJumpStep+1));
        pucTmpPtr += *pusSignerCertLen;
        break;
    default:
        return SEC_ERR_BAD_CERTIFICATE;
    }
    
    *pucSignatureAlg = *pucTmpPtr;
    
    pucTmpPtr++;
    We_UTILSGetUINT16(pucTmpPtr, pusSignatureLen); 
    pucTmpPtr += 2; 
    *ppucSignature = pucTmpPtr;
    
    return SEC_ERR_OK;
}

#endif

/*==================================================================================================
FUNCTION: 
    We_X509ParseCertResponse
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the information of parse certificate . 
ARGUMENTS PASSED:
    const WE_UCHAR *pucCertResponse[IN]:the information of cert.
    WE_INT16 *psCharacterSet[OUT]:charset.
    WE_VOID **ppvDisplayName[OUT]:the dispayed name.
    WE_INT32 *piDisplayNameLen[OUT]:the length of ppvDisplayName.
    WE_UCHAR **ppucCaKeyHash[OUT]:the hash of ca key.
    WE_UCHAR **ppucSubjectKeyHash[OUT]:the hash of subject key.
    WE_UCHAR **pucCert[OUT]:cert.
    WE_UINT16 *pusCertLen[OUT]:the length of cert.
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
WE_INT32 We_X509ParseCertResponse(const WE_UCHAR *pucCertResponse,
                                  WE_INT16 *psCharacterSet, 
                                  WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen,
                                  WE_UCHAR **ppucCaKeyHash, WE_UCHAR **ppucSubjectKeyHash,
                                  WE_UCHAR **pucCert, WE_UINT16 *pusCertLen)
{
    WE_UCHAR *pucTmpPtr = NULL;
    WE_INT32 iRes;
    WE_UINT16 usJumpStep = 0;
    WE_UINT16 usCharacterSet = 0;
    WE_UINT8 respType= 0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucCertResponse, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(psCharacterSet, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppvDisplayName, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(piDisplayNameLen, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucCaKeyHash, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucSubjectKeyHash, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pucCert, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusCertLen, 0x08);
    
    pucTmpPtr = (WE_UCHAR *)pucCertResponse;
    if(*pucTmpPtr != 1)
    {
        return SEC_ERR_INVALID_PARAMETER;
    }
    pucTmpPtr++;    
    respType = *pucTmpPtr;
    pucTmpPtr++;
    
    switch (respType)
    {
    case 0:            
        return SEC_ERR_NOT_IMPLEMENTED; 
    case 1:            
        
        We_UTILSGetUINT16((const WE_UCHAR *)pucTmpPtr, &usCharacterSet);
        *psCharacterSet = (WE_INT16) usCharacterSet;
        pucTmpPtr += 2;
        *piDisplayNameLen = *pucTmpPtr;
        pucTmpPtr++;
        *ppvDisplayName = (WE_VOID*) pucTmpPtr;
        pucTmpPtr += *piDisplayNameLen;
        
        if (*pucTmpPtr != 254)
        {
            return SEC_ERR_INVALID_PARAMETER;
        }
        pucTmpPtr++;
        *ppucCaKeyHash = pucTmpPtr;
        pucTmpPtr += 20;
        
        if (*pucTmpPtr != 254)
        {
            return SEC_ERR_INVALID_PARAMETER;
        }
        pucTmpPtr++;
        *ppucSubjectKeyHash = pucTmpPtr;
        pucTmpPtr += 20;
        if(*pucTmpPtr != SEC_ASN_1_CONSTRUCTED_SEQUENCE)
        {
            pucTmpPtr += 2;
        }
        iRes = We_X509GetAndCheckLength(pucTmpPtr, pusCertLen, &usJumpStep);
        if (iRes != SEC_ERR_OK)
        {
            return iRes;
        }
        *pucCert = pucTmpPtr;
        (*pusCertLen) = (WE_UINT16)((*pusCertLen) + (usJumpStep+1));
        break;
    case 2:        
        return SEC_ERR_NOT_IMPLEMENTED;
    default:    
        break;
    }
    return SEC_ERR_OK;
}
#if 0
/*==================================================================================================
FUNCTION: 
    We_X509GetExtnKeysUsage
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get the usage of extent key. 
ARGUMENTS PASSED:
    const WE_UCHAR *pucExtnValue[IN]:Pointer to the DER encoded extension value in buffer. 
    WE_UCHAR** ppucExKeyusage[OUT]:the usage of extent key.
    WE_UINT16* pusExKeyUsageLen[OUT]: length of ppucExKeyusage.
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
static WE_INT32 We_X509GetExtnKeysUsage(const WE_UCHAR *pucExtnValue, 
                                        WE_UCHAR** ppucExKeyusage,
                                        WE_UINT16* pusExKeyUsageLen)
{
    WE_UCHAR *pucExtension = NULL;
    WE_UINT16 usTmpStep=0;
    WE_UINT16 usSize=0;
    
    M_X509_CHECK_FOR_NULL_ARG(pucExtnValue, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(ppucExKeyusage, 0x08);
    M_X509_CHECK_FOR_NULL_ARG(pusExKeyUsageLen, 0x08);
    
    pucExtension = (WE_UCHAR *)pucExtnValue;
    if(*pucExtension != SEC_ASN_1_BIT_STRING)            
    {
        return SEC_ERR_BAD_CERTIFICATE;
    }
    pucExtension++;
    
    We_X509DERtoSize(pucExtension, &usTmpStep, &usSize);
    pucExtension +=usTmpStep;
    *ppucExKeyusage = pucExtension;
    *pusExKeyUsageLen = usSize;
    
    return SEC_ERR_OK;
}
#endif
/*==================================================================================================
FUNCTION: 
    We_UTILSGetUINT16
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:

ARGUMENTS PASSED:
    const WE_UINT8 *pucStr[IN]:
    WE_UINT16 *pusUint[OUT]:
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
WE_VOID We_UTILSGetUINT16(const WE_UINT8 *pucStr, WE_UINT16 *pusUint)
{
    WE_UINT16 usU = 0x0102; 
    
    if (!pucStr||!pusUint)
    {
        return;
    }
    if (0x01 == *((WE_UINT8*)&(usU))) 
    {
        *((WE_UINT8*)pusUint) = pucStr[0];
        *(((WE_UINT8*)pusUint)+1) = pucStr[1];
    }
    else                        
    {
        *((WE_UINT8*)pusUint) = pucStr[1];
        *(((WE_UINT8*)pusUint)+1) = pucStr[0];
    }
}
/*==================================================================================================
FUNCTION: 
    We_UTILSGetUINT32
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:

ARGUMENTS PASSED:
    const WE_UINT8 *pucStr[IN]:
    WE_UINT16 *pusUint[OUT]:
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
WE_VOID We_UTILSGetUINT32(const WE_UINT8 *pucStr, WE_UINT32 *puiUint)
{
    WE_UINT16 usU = 0x0102; 
    
    if (!pucStr||!puiUint)
    {
        return;
    }
    
    if (0x01 == *((WE_UINT8*)&(usU))) 
    {
        *((WE_UINT8*)puiUint) = pucStr[0];
        *(((WE_UINT8*)puiUint)+1) = pucStr[1];
        *(((WE_UINT8*)puiUint)+2) = pucStr[2];
        *(((WE_UINT8*)puiUint)+3) = pucStr[3];
    }
    else                        
    {
        *((WE_UINT8*)puiUint) = pucStr[3];
        *(((WE_UINT8*)puiUint)+1) = pucStr[2];
        *(((WE_UINT8*)puiUint)+2) = pucStr[1];
        *(((WE_UINT8*)puiUint)+3) = pucStr[0];
    }
}
/*==================================================================================================
FUNCTION: 
    We_UTILSPutUINT16
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:

ARGUMENTS PASSED:
    const WE_UINT8 *pucStr[IN]:
    WE_UINT16 *pusUint[OUT]:
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
WE_VOID We_UTILSPutUINT16(WE_UINT16 *pusUint, WE_UINT8 *pucStr)
{
    WE_UINT16 usU = 0x0102; 
    
    if (!pucStr||!pusUint)
    {
        return;
    }
    
    if (0x01 == *((WE_UINT8*)&(usU))) 
    {
        pucStr[0] = *((WE_UINT8*)pusUint);
        pucStr[1] = *(((WE_UINT8*)pusUint)+1);
    }
    else                        
    {
        pucStr[1] = *((WE_UINT8*)pusUint);
        pucStr[0] = *(((WE_UINT8*)pusUint)+1);
    }
}
/*==================================================================================================
FUNCTION: 
    We_UTILSPutUINT32
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:

ARGUMENTS PASSED:
    const WE_UINT8 *pucStr[IN]:
    WE_UINT16 *pusUint[OUT]:
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
WE_VOID We_UTILSPutUINT32(WE_UINT32 *puiUint, WE_UINT8 *pucStr)
{
    WE_UINT16 usU = 0x0102; 
    
    if (!pucStr||!puiUint)
    {
        return;
    }
    
    if (0x01 == *((WE_UINT8*)&(usU))) 
    {
        pucStr[0] = *((WE_UINT8*)puiUint);
        pucStr[1] = *(((WE_UINT8*)puiUint)+1);
        pucStr[2] = *(((WE_UINT8*)puiUint)+2);
        pucStr[3] = *(((WE_UINT8*)puiUint)+3);
    }
    else                        
    {
        pucStr[3] = *((WE_UINT8*)puiUint);
        pucStr[2] = *(((WE_UINT8*)puiUint)+1);
        pucStr[1] = *(((WE_UINT8*)puiUint)+2);
        pucStr[0] = *(((WE_UINT8*)puiUint)+3);
    }
}


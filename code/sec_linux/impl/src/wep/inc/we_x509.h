/*==================================================================================================
    FILE NAME : x509.h
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
/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef _SEC_X509_H
#define _SEC_X509_H

/***************************************************************************************************
*   Macro Define Section
***************************************************************************************************/
#define SEC_ASN_1_BOOLEAN                 1
#define	SEC_ASN_1_INTEGER                 2
#define SEC_ASN_1_BIT_STRING              3
#define SEC_ASN_1_OCTET_STRING            4
#define SEC_ASN_1_NULL                    5
#define SEC_ASN_1_OBJECT_IDENTIFIER       6
#define SEC_ASN_1_UTF8STRING              12
#define SEC_ASN_1_SEQUENCE_OF             16
#define SEC_ASN_1_SEQUENCE                16
#define SEC_ASN_1_SET                     17
#define SEC_ASN_1_SET_OF                  17
#define SEC_ASN_1_NUMERICSTRING           18
#define SEC_ASN_1_PRINTTABLESTRING        19
#define SEC_ASN_1_T61STRING               20
#define SEC_ASN_1_EMAIL_IA5STRING         22
#define SEC_ASN_1_UTCTIME                 23
#define SEC_ASN_1_UNIVERSALSTRING         28
#define SEC_ASN_1_BMPSTRING               30
#define SEC_ASN_1_CONSTRUCTED_SEQUENCE    48
#define SEC_ASN_1_CONSTRUCTED_SEQUENCE_OF 49
#define SEC_ASN_1_UNKNOWN                 100

#define SEC_ASN_1_IMPLICIT_TAG1           129
#define SEC_ASN_1_IMPLICIT_TAG2           130


#define SEC_ASN_1_EXPLICIT_TAG            160
#define SEC_ASN_1_IMPLICIT_TAG            163


#define SEC_CERT_USAGE_SERVER_AUTH  1
#define SEC_CERT_USAGE_CLIENT_AUTH  2
#define SEC_CERT_USAGE_CA           3
#define SEC_CERT_USAGE_ROOT         4


/*Extensions*/
#define SEC_CERT_EXT_UNKNOWN                   0
#define SEC_CERT_EXT_DOMAIN_INFORMATION        1
#define SEC_CERT_EXT_KEY_USAGE                15 
#define SEC_CERT_EXT_EXTENDED_KEY_USAGE       37
#define SEC_CERT_EXT_CERTIFICATE_POLICIES     32
#define SEC_CERT_EXT_SUBJECT_ALT_NAME         17
#define SEC_CERT_EXT_BASIC_CONSTRAINTS        19
#define SEC_CERT_EXT_NAME_CONSTRAINTS         30
#define SEC_CERT_EXT_POLICY_CONSTRAINTS       36
#define SEC_CERT_EXT_AUTHORITY_KEY_IDENTIFIER 35
#define SEC_CERT_EXT_SUBJECT_KEY_IDENTIFIER   14

#define SEC_ERR_OK                                0
#define SEC_ERR_GENERAL_ERROR                     1
#define SEC_ERR_BUFFER_TOO_SMALL                  2
#define SEC_ERR_UNSUPPORTED_METHOD                3
#define SEC_ERR_INSUFFICIENT_MEMORY               4
#define SEC_ERR_CRYPTOLIB_NOT_INITIALISED         5
#define SEC_ERR_KEY_TOO_LONG                      6
#define SEC_ERR_NOT_IMPLEMENTED                   7
#define SEC_ERR_INVALID_PARAMETER                 8
#define SEC_ERR_DATA_LENGTH                       9
#define SEC_ERR_INVALID_KEY                      10
#define SEC_ERR_INVALID_HANDLE                   11
#define SEC_ERR_KEY_LENGTH                       12
#define SEC_ERR_MISSING_KEY                      13
#define SEC_ERR_UNKNOWN_CERTIFICATE_TYPE         14
#define SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE     15
#define SEC_ERR_BAD_CERTIFICATE                  16
#define SEC_ERR_CERTIFICATE_EXPIRED              17
#define SEC_ERR_MISSING_CERTIFICATE              18
#define SEC_ERR_NOT_FOUND                        19
#define SEC_ERR_INVALID_COMMON_NAME              20

#define SEC_ERR_USER_NOT_VERIFIED                21
#define SEC_ERR_MISSING_AUT_KEY_HANDSHAKE        22
#define SEC_ERR_REACHED_USER_CERT_LIMIT          23
#define SEC_ERR_PIN_TOO_SHORT                    24  /* The PIN was shorter than SEC_PIN_MIN_SIZE */
#define SEC_ERR_PIN_TOO_LONG                     25  /* The PIN was longer than SEC_PIN_MAX_SIZE */
#define SEC_ERR_PIN_FALSE                        26
#define SEC_ERR_PIN_LOCKED                       27
#define SEC_ERR_MISSING_PUK                      28  /* No PUK is stored on the WIM                    */
#define SEC_ERR_WRONG_PUK                        29  /* The wrong PUK was entered                      */
#define SEC_ERR_PUK_PIN_UNCHANGED                30  /* The PUK has been verified, but the PIN has not */
/* been changed due to user cancel or error in TI */  
#define SEC_ERR_HASH_NOT_VERIFIED                31
#define SEC_ERR_MISSING_AUT_KEY_OPEN_WIM         32
#define SEC_ERR_MISSING_NR_KEY_SIGN_TEXT         33

#define SEC_ERR_MISSING_KEY_CONNECT              34
#define SEC_ERR_MISSING_KEY_STORE_CERT           35
#define SEC_ERR_MISSING_KEY_DELETE_CERT          36
#define SEC_ERR_MISSING_KEY_CHANGE_AUTH          37
#define SEC_ERR_MISSING_KEY_CHANGE_NONREP        38

#define SEC_ERR_PRIV_KEY_NOT_FOUND               39
#define SEC_ERR_COULD_NOT_STORE_CONTRACT         40
#define SEC_ERR_CANCEL                           41

/***OID***/
#define SEC_COUNTRY_NAME              0x00000001   /*2 5 4 6*/   
#define SEC_ORGANISATION_NAME         0x00000002   /*2 5 4 10*/
#define SEC_ORGANISATIONAL_UNIT_NAME  0x00000004   /*2 5 4 11*/
#define SEC_STATE_OR_PROVINCE_NAME    0x00000008   /*2 5 4 8*/ 
#define SEC_COMMON_NAME               0x00000010   /*2 5 4 3*/  
#define SEC_DOMAIN_COMPONENT          0x00000020   /*0 9 2342 19200300 100 1 25*/
#define SEC_SERIAL_NUMBER             0x00000040   /*2 5 4 5*/

#define SEC_DN_QUALIFIER              0x00000080   /*2 5 4 46*/
#define SEC_LOCALITY_NAME             0x00000100   /*2 5 4 7*/                         
#define SEC_TITLE                     0x00000200   /*2 5 4 12*/
#define SEC_SURNAME                   0x00000400   /*2 5 4 4*/
#define SEC_GIVENNAME                 0x00000800   /*2 5 4 42*/
#define SEC_INITIALS                  0x00001000   /*2 5 4 43*/
#define SEC_GENERATION_QUALIFIER      0x00002000   /*2 5 4 44*/

#define SEC_NAME                      0x00004000   /*2 5 4 41*/
#define SEC_EMAIL_ADDRESS             0x00008000   /*1 2 840 113549 1 9 1*/

#define SEC_UNKNOWN_ATTRIBUTE_TYPE    0x00010000



#ifndef WE_RSA_FOR_SEC




/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tgSt_Wtls/* WTLS*/
{    
    WE_UINT8    ucSignatureAlgorithm;
    WE_UINT8    ucIssuerIdentifierType;
    WE_UINT16   usIssuercharacterSet;
    WE_UINT8    ucIssuerLen;
    WE_UINT8    ucSubjectIdentifierType;
    WE_UINT16   usSubjectcharacterSet;
    WE_UINT8    ucSubjectLen;
    WE_UINT8    ucPublicKeyType;
    WE_UINT8    ucParameter_index;
    WE_UINT16   usExpLen;
    WE_UCHAR*   pucRsaExponent;
    WE_UINT16   usModLen;
    WE_UCHAR*   pucRsaModulus;    
} St_SecWtlsCertificate;

typedef struct tagSt_X509 /* X509*/
{   
    WE_UCHAR*           pucSerialNumber;    
    WE_UINT16           usSerialNumberLen;
    WE_UCHAR*           pucSignatureAlgId;
    WE_UINT16           usSignatureAlgIdLen;
    WE_UINT16           usIssuerLen;
    WE_UINT16           usSubjectLen;
    WE_UCHAR*           pucPublicKeyAlgId;
    WE_UINT16           usPublicKeyAlgIdLen;
    WE_UCHAR*           pucPublicKeyVal; 
    WE_UINT16           usPublicKeyValLen;
    WE_UCHAR*           pucExtensions; 
    WE_UINT16           usExtensionsLen;
    WE_UCHAR*           pucSignatureAlgorithm;
    WE_UINT16           usSignatureAlgorithmLen;    
    WE_UCHAR*           pucTbsCert;
    WE_UINT32           uiTbsCertLen;
    /* Extension */
    WE_UCHAR*           pucExtKeyUsage;
    WE_UINT32           uiExtKeyUsageLen;
    WE_UCHAR*           pucExtExtKeyUsage;
    WE_UINT32           uiExtExtKeyUsageLen;
    WE_UCHAR*           pucExtSubjectAltName;
    WE_UINT32           uiExtSubjectAltNameLen;
    WE_UCHAR*           pucExtIssuerAltName;
    WE_UINT32           uiExtIssuerAltNameLen;
    WE_UCHAR*           pucExtAuthKeyId;
    WE_UINT32           uiExtAuthKeyIdLen;
    WE_UCHAR*           pucExtBasicConstraint;
    WE_UINT32           uiExtBasicConstraintLen;
    WE_UCHAR*           pucExtSubjectKeyId;
    WE_UINT32           uiExtSubjectKeyIdLen;
    /* RSA */
    WE_UCHAR*           pucModulus;
    WE_UINT16           usModulusLen;
    WE_UCHAR*           pucExponent;
    WE_UINT16           usExponentLen;
} St_SecX509Certificate;

typedef union tagSt_SecCertificateUnion
{
    St_SecWtlsCertificate      stWtls;
    St_SecX509Certificate      stX509;
}St_SecCertificateUnion;


typedef struct tagSt_SecCertificate
{
    WE_UINT8 ucCertificateVersion;
    WE_UINT32 iValidNotBefore;
    WE_UINT32 iValidNotAfter;
    WE_UCHAR *pucIssuer;
    WE_UCHAR *pucSubject;
    WE_UCHAR *pucSignature;
    WE_UINT16 usSignatureLen;
    St_SecCertificateUnion     stCertUnion;
} St_SecCertificate;

typedef struct tagSt_SecNameAttribute
{
    WE_UINT32  attributeType;
    WE_INT16   attributeCharSet;
    WE_UCHAR * attributeValue;
    WE_UINT16  attributeValueLen;
} st_SecNameAttribute;

typedef struct tagSt_SecX509RsaModAndExp 
{
    WE_UCHAR *rsa_modulus;
    WE_UINT16 modLen;
    WE_UCHAR *rsa_exponent;
    WE_UINT16 expLen;
} St_SecX509RsaModAndExp;

typedef struct tagSt_SecX509DsaParams
{
    WE_UCHAR *publicKey;
    WE_UINT16 publicKeyLen;
    WE_UCHAR *dsaParams;
    WE_UINT16 dsaParamsLen;
} St_SecX509DsaParams;

#endif /*WE_RSA_FOR_SEC*/
/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_VOID We_UTILSGetUINT16(const WE_UINT8 *str, WE_UINT16 *uint);
WE_VOID We_UTILSGetUINT32(const WE_UINT8 *str, WE_UINT32 *uint);
WE_VOID We_UTILSPutUINT16(WE_UINT16 *uint, WE_UINT8 *str);
WE_VOID We_UTILSPutUINT32(WE_UINT32 *uint, WE_UINT8 *str);

WE_INT32 
We_X509GetAndCheckLength(const WE_UCHAR *pucX509Cert, 
                         WE_UINT16 *pusCertLen, WE_UINT16 *pusJumpStep);

WE_VOID 
We_X509DERtoSize(WE_UCHAR *pucCert, WE_UINT16 *pusTmpStep, WE_UINT16 *pusSize);

WE_INT32 
We_X509GetTBSCert(const WE_UCHAR *pucX509Cert,
                  WE_UCHAR *pucTbsCert, WE_UINT16 *pusTbsCertLen);

WE_INT32 
We_X509Parse(const WE_UCHAR *pucX509Cert, WE_UINT16 *pusCertLength,
             St_SecCertificate *pstCert);

WE_INT32 
We_X509GetNbrIssuerStrings(const WE_UCHAR *pucIssuer, WE_UINT16 usIssuerLen, 
                           WE_UINT8 *pucNbrIssuerStrings);
/*
WE_INT32 
We_X509GetIssuerAttributes(const WE_UCHAR *pucIssuer, 
                           WE_UINT16 usIssuerLen, 
                           WE_UINT8 *pucNbrIssuerAttributes,                                
                           st_SecNameAttribute *pstIssuerA);*/


WE_INT32 
We_X509GetIssuerString(const WE_UCHAR *pucX509Cert, 
                       WE_UCHAR **ppucIssuerString, WE_UINT16 *pusIssuerStringLen, 
                       WE_UINT16 *pusJumpStep);

WE_INT32 
We_X509GetAndCheckTBSCertLength(const WE_UCHAR *pucTBSCert, WE_UINT16 *pusTBSCertLen, 
                                WE_UINT16 usCertLen, WE_UINT16 *pusJumpStep);

WE_INT32 
We_X509ParseTBHTrustedCAInfo(const WE_UCHAR *pucTBHTrusedCAInfo, 
                             WE_INT16 *psCharacterSet, 
                             WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen,
                             WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen);

WE_INT32 
We_X509ParseSignedTrustedCAInfo(const WE_UCHAR *pucSignedTrustedCAInfo, 
                                WE_INT16 *psCharacterSet, 
                                WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen,
                                WE_UCHAR **ppucTrustedCAcert, 
                                WE_UINT16 *pusTrustedCAcertLen,
                                WE_UCHAR **ppucSignerCert, WE_UINT16 *pusSignerCertLen,
                                WE_UINT8 *pucSignatureAlg, WE_UCHAR **ppucSignature, 
                                WE_UINT16 *pusSignatureLen);

WE_INT32 
We_X509ParseCertResponse(const WE_UCHAR *pucCertResponse, 
                         WE_INT16 *psCharacterSet, 
                         WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen,
                         WE_UCHAR **ppucCaKeyHash, WE_UCHAR **ppucSubjectKeyHash,
                         WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen);

WE_INT32 
We_X509GetRSAModAndExp(const WE_UCHAR *pucPublicKeyVal,
                       WE_UCHAR **ppucRsaModulus, WE_UINT16 *pusModLen, 
                       WE_UCHAR **ppucRsaExponent, WE_UINT16 *pusExpLen);

WE_INT32 
We_X509GetDSAParams(const WE_UCHAR *pucPublicKeyVal,
                    WE_UCHAR **ppucPublicKey, WE_UINT16 *pusPublicKeyLen,
                    WE_UCHAR **ppucDsaParams, WE_UINT16 *pusDsaParamsLen);

WE_INT32 
We_X509GetExtension(WE_UCHAR *pucExtensions, WE_INT32 *piExtnId, WE_INT32 *piCritical,
                    WE_UCHAR **ppucExtnValue, WE_UINT16 *pusExtnValueLen,
                    WE_UINT16 *pusJumpStep);

WE_INT32 
We_X509ExtnKeyUsage(const WE_UCHAR *pucExtnValue, 
                    WE_UINT8 *pucKeyUsageBit0To7, 
                    WE_UINT8 *pucKeyUsageBit8To15); 

WE_INT32 
We_X509ExtnExtKeyUsage(const WE_UCHAR *pucExtnValue, 
                       WE_INT32 *piAnyExtendedKeyUsage, WE_INT32 *piServerAuth, 
                       WE_INT32 *piClientAuth, WE_INT32 *piCodeSigning, WE_INT32 *piEmailProtection, 
                       WE_INT32 *piTimeStamping, WE_INT32 *piOCSPSigning);

WE_INT32 
We_X509ExtnAuthorityKeyIdentifier(const WE_UCHAR *pucExtnValue, 
                                  WE_UCHAR** ppucKeyIdentifier,
                                  WE_UINT32* pusKeyIdentifierLen);

WE_INT32 
We_X509ExtnSubjectKeyIdentifier(const WE_UCHAR *pucExtnValue, 
                                WE_UCHAR** ppucKeyIdentifier,
                                WE_UINT32* pusKeyIdentifierLen);


WE_INT32 
We_X509ExtnSubjectAltName(const WE_UCHAR *pucExtnValue, 
                          WE_UCHAR **ppucDnsName,WE_UINT32 *pusDnsNameLen,
                          WE_UCHAR **ppucIpAddress,WE_UINT16 *pusIpAddressLen);

WE_INT32 
We_X509ExtnBasicConstraints(const WE_UCHAR *pucExtnValue, WE_INT32 *piCa,
                            WE_INT32 *piHasPathLenConstraint,
                            WE_UINT8 *pucPathLenConstraint);

/*************************** for intenal use*/
WE_INT32 We_X509CheckAndGetIssuerOID(const WE_UCHAR *pucOidString, 
                                     WE_UINT32 *puiOid, WE_UINT16 usOidLen);
WE_INT32 We_X509GetAttributeString(const WE_UCHAR *pucIssuerPart, 
                                   WE_VOID **ppvAttributeValue, 
                                   WE_UINT16 *pusAttributeValueLen,
                                   WE_INT16 *pusAttrCharSet, 
                                   WE_UINT32 *puiAttributeType,
                                   WE_UINT16 *pusJumpStep);





#endif

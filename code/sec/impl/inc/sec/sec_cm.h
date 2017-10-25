/*=====================================================================================

    HEADER NAME : sec_cm.h
    MODULE NAME : SEC

    PRE-INCLUDE FILES DESCRIPTION

    GENERAL DESCRIPTION
        This file be included by all cert manage Programmer.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
   
    Modification                 Tracking
    Date           Author         Number        Description of changes
    ----------   ------------    ---------   --------------------------------------------
    2006-07-03   wuxl             None          Init

=====================================================================================*/


/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef SEC_CM_H
#define SEC_CM_H


/*******************************************************************************
*   macro Define Section
*******************************************************************************/
/*Attributes for issuer and subject*/
                                               /**OID**/
#define M_SEC_COUNTRY_NAME                     0x00000001   /*2 5 4 6*/   
#define M_SEC_ORGANISATION_NAME                0x00000002   /*2 5 4 10*/
#define M_SEC_ORGANISATIONAL_UNIT_NAME         0x00000004   /*2 5 4 11*/
#define M_SEC_STATE_OR_PROVINCE_NAME           0x00000008   /*2 5 4 8*/ 
#define M_SEC_COMMON_NAME                      0x00000010   /*2 5 4 3*/  
#define M_SEC_DOMAIN_COMPONENT                 0x00000020   /*0 9 2342 19200300 100 1 25*/
#define M_SEC_SERIAL_NUMBER                    0x00000040   /*2 5 4 5*/

#define M_SEC_DN_QUALIFIER                     0x00000080   /*2 5 4 46*/
#define M_SEC_LOCALITY_NAME                    0x00000100   /*2 5 4 7*/                         
#define M_SEC_TITLE                            0x00000200   /*2 5 4 12*/
#define M_SEC_SURNAME                          0x00000400   /*2 5 4 4*/
#define M_SEC_GIVENNAME                        0x00000800   /*2 5 4 42*/
#define M_SEC_INITIALS                         0x00001000   /*2 5 4 43*/
#define M_SEC_GENERATION_QUALIFIER             0x00002000   /*2 5 4 44*/

#define M_SEC_NAME                             0x00004000   /*2 5 4 41*/
#define M_SEC_EMAIL_ADDRESS                    0x00008000   /*1 2 840 113549 1 9 1*/

#define M_SEC_UNKNOWN_ATTRIBUTE_TYPE           0x00010000

/*Extensions*/
#define M_SEC_CERT_EXT_UNKNOWN                  0
#define M_SEC_CERT_EXT_DOMAIN_INFORMATION       1
#define M_SEC_CERT_EXT_KEY_USAGE                15 
#define M_SEC_CERT_EXT_EXTENDED_KEY_USAGE       37
#define M_SEC_CERT_EXT_CERTIFICATE_POLICIES     32
#define M_SEC_CERT_EXT_SUBJECT_ALT_NAME         17
#define M_SEC_CERT_EXT_BASIC_CONSTRAINTS        19
#define M_SEC_CERT_EXT_NAME_CONSTRAINTS         30
#define M_SEC_CERT_EXT_POLICY_CONSTRAINTS       36
#define M_SEC_CERT_EXT_AUTHORITY_KEY_IDENTIFIER 35
#define M_SEC_CERT_EXT_SUBJECT_KEY_IDENTIFIER   14

/*certificate usage */
#define M_SEC_CERT_USAGE_SERVER_AUTH            1
#define M_SEC_CERT_USAGE_CLIENT_AUTH            2
#define M_SEC_CERT_USAGE_CA                     3
#define M_SEC_CERT_USAGE_ROOT                   4


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_SecWtlsCertificate
{   /* WTLS*/
    WE_UINT8                   ucSignatureAlgorithm;
    WE_UINT8                   ucIssuerIdentifierType;
    WE_UINT16                  usIssuercharacterSet;
    WE_UINT8                   ucIssuerLen;
    WE_UINT8                   ucSubjectIdentifierType;
    WE_UINT16                  usSubjectcharacterSet;
    WE_UINT8                   ucSubjectLen;
    WE_UINT8                   ucPublicKeyType;
    WE_UINT8                   ucParameter_index;
    WE_UINT16                  usExpLen;
    WE_UCHAR*                  pucRsaExponent;
    WE_UINT16                  usModLen;
    WE_UCHAR*                  pucRsaModulus;  
}St_SecWtlsCertificate;

typedef struct tagSt_SecX509Certificate
{   /* X509*/
    WE_UCHAR*                  pucSerialNumber;    
    WE_UINT16                  usSerialNumberLen;
    WE_UCHAR*                  pucSignatureAlgId;
    WE_UINT16                  usSignatureAlgIdLen;
    WE_UINT16                  usIssuerLen;
    WE_UINT16                  usSubjectLen;
    WE_UCHAR*                  pucPublicKeyAlgId;
    WE_UINT16                  usPublicKeyAlgIdLen;
    WE_UCHAR*                  pucPublicKeyVal; 
    WE_UINT16                  usPublicKeyValLen;
    WE_UCHAR*                  pucExtensions; 
    WE_UINT16                  usExtensionsLen;
    WE_UCHAR*                  pucSignatureAlgorithm;
    WE_UINT16                  usSignatureAlgorithmLen;
    WE_UCHAR*                  pucTbsCert;
    WE_UINT32                  uiTbsCertLen;
    /* Extension */
    WE_UCHAR*                  pucExtKeyUsage;
    WE_UINT32                  uiExtKeyUsageLen;
    WE_UCHAR*                  pucExtExtKeyUsage;
    WE_UINT32                  uiExtExtKeyUsageLen;
    WE_UCHAR*                  pucExtSubjectAltName;
    WE_UINT32                  uiExtSubjectAltNameLen;
    WE_UCHAR*                  pucExtIssuerAltName;
    WE_UINT32                  uiExtIssuerAltNameLen;
    WE_UCHAR*                  pucExtAuthKeyId;
    WE_UINT32                  uiExtAuthKeyIdLen;
    WE_UCHAR*                  pucExtBasicConstraint;
    WE_UINT32                  uiExtBasicConstraintLen;
    WE_UCHAR*                  pucExtSubjectKeyId;
    WE_UINT32                  uiExtSubjectKeyIdLen;
    /* RSA */
    WE_UCHAR*                  pucModulus;
    WE_UINT16                  usModulusLen;
    WE_UCHAR*                  pucExponent;
    WE_UINT16                  usExponentLen;
}St_SecX509Certificate;

typedef union tagSt_SecCertificateUnion
{
    St_SecWtlsCertificate      stWtls;
    St_SecX509Certificate      stX509;
  }St_SecCertificateUnion;

typedef struct tagSt_SecCertificate
{
    WE_UINT8                   ucCertificateVersion;
    WE_UINT32                  iValidNotBefore;
    WE_UINT32                  iValidNotAfter;
    WE_UCHAR*                  pucIssuer;
    WE_UCHAR*                  pucSubject;
    WE_UCHAR*                  pucSignature;
    WE_UINT16                  usSignatureLen;
    St_SecCertificateUnion     stCertUnion;
} St_SecCertificate;

typedef struct tagSt_SecNameAttribute 
{
    WE_UINT32                  uiAttributeType;
    WE_INT16                   sAttributeCharSet;
    WE_UCHAR*                  pucAttributeValue;
    WE_UINT16                  usAttributeValueLen;
} St_SecNameAttribute;


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

WE_VOID Sec_CertGlobalVarInit(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertGetPubKeyRsa(WE_HANDLE hSecHandle, WE_UCHAR *pucCert, St_SecPubKeyRsa *pstPubKey);

WE_INT32 Sec_CertGetItemsForUe(WE_HANDLE hSecHandle, WE_UCHAR *pucCert, WE_INT16 *psIssuercharSet, 
                            WE_VOID **ppvInIssuer, WE_INT32 *piInIssuerLen, 
                            WE_INT16 *psSubjectcharSet, 
                            WE_VOID **ppvInSubject, WE_INT32 *piInSubjectLen, 
                            WE_UINT32 *puiValidnotBefore, WE_UINT32 *puiValidnotAfter, 
                            WE_UCHAR **ppucSerialNumber,WE_UINT16* pusSerialNumberLen,
                            WE_UCHAR **ppucAlg, WE_UCHAR **ppucPubKey, 
                            WE_UINT16* pusPubKeyLen);

WE_INT32 Sec_CertGetSignedTrustedCaInfo(WE_HANDLE hSecHandle, 
                                const WE_UCHAR *pucSignedTrustedCAInfo, WE_INT16 *psCharacterSet, 
                                WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen, 
                                WE_UCHAR **ppucTrustedCAcert, WE_UINT16 *pusTrustedCAcertLen, 
                                WE_UCHAR **ppucSignerCert, WE_UINT16 *pusSignerCertLen, 
                                WE_UINT8 *pucSignatureAlg, WE_UCHAR **ppucSignature, 
                                WE_UINT16 *pusSignatureLen);

WE_VOID Sec_HandleStoreCaCert(WE_HANDLE hSecHandle, WE_INT32 iWid, 
                            WE_UCHAR *pucCert, WE_UINT16 usCertLen);

WE_VOID Sec_HandleStoreUsrCert(WE_HANDLE hSecHandle, WE_INT32 iWid, 
                             WE_UCHAR *pucCert, WE_UINT16 usCertLen, const WE_UCHAR *pucURL);

WE_VOID Sec_HandleHashVerify(WE_HANDLE hSecHandle, WE_INT32 iWid, 
                            WE_UCHAR *pucTrustedCAInfoStruct, WE_UINT16 usTrustedCAInfoStructLen);

WE_VOID Sec_HandleSignVerify(WE_HANDLE hSecHandle, WE_INT32 iWid, 
                            WE_UCHAR *pucTrustedCAInfoStruct, WE_UINT16 usTrustedCAInfoStructLen);

WE_VOID Sec_HandleCertDelivery(WE_HANDLE hSecHandle, WE_INT32 iWid, WE_UCHAR *pucCertResponse);

WE_VOID Sec_HandleGetCertNames(WE_HANDLE hSecHandle, WE_INT32 iWid, WE_INT32 iCertOptions);

WE_VOID Sec_HandleGetCert(WE_HANDLE hSecHandle, WE_INT32 iWid, WE_INT32 iCertId, WE_INT32 iOptions);

WE_VOID Sec_HandleDeleteCert(WE_HANDLE hSecHandle, WE_INT32 iWid, WE_INT32 iCertId);

WE_INT32 Sec_HandleOldRootCert(WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_INT32 iTooOldRootCertId);

WE_INT32 Sec_CertStateFormInit(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertStateFormTerminate(WE_HANDLE hSecHandle);


#endif /* endif SEC_CM_H */

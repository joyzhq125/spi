/*=====================================================================================

    HEADER NAME : oem_secx509parser.h
    MODULE NAME : SEC

    PRE-INCLUDE FILES DESCRIPTION

    GENERAL DESCRIPTION
        This file be included by all x509 cert Programmer.
        
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
#ifndef _OEM_SECX509PARSER_H
#define _OEM_SECX509PARSER_H


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

WE_INT32 Sec_X509ChainInit(WE_HANDLE hSecHandle);

WE_INT32 Sec_X509ChainTerminate(WE_HANDLE hSecHandle);

WE_INT32 Sec_X509CertParse(WE_HANDLE hSecHandle, const WE_UCHAR *pucX509Cert, 
                           WE_UINT16 *pusCertLength, St_SecCertificate *pstCert);

WE_INT32 Sec_X509CertParseResponse(WE_HANDLE hSecHandle, 
                                   const WE_UCHAR *pucCertResponse, WE_INT16 *psCharacterSet, 
                                   WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen, 
                                   WE_UCHAR **ppucCaKeyHash, WE_UCHAR **ppucSubjectKeyHash, 
                                   WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen);

WE_INT32 Sec_X509GetCertFieldLength(WE_HANDLE hSecHandle, const WE_UCHAR *pucX509Cert, 
                                    WE_UINT16 *pusCertLen, WE_UINT16 *pusJumpStep);

WE_INT32 Sec_X509GetIssuerInfo(WE_HANDLE hSecHandle, 
                               const WE_UCHAR *pucIssuer, WE_UINT16 usIssuerLen, 
                               WE_UINT8 *pucNbrIssuerAttributes, St_SecNameAttribute *pstIssuerA);

WE_INT32 Sec_X509GetNbrIssuerStr(WE_HANDLE hSecHandle, const WE_UCHAR *pucIssuer, 
                                 WE_UINT16 usIssuerLen, WE_UINT8 *pucNbrIssuerStrings);

WE_VOID Sec_X509DerToSize(WE_HANDLE hSecHandle, WE_UCHAR *pucCert, 
                          WE_UINT16 *pusTmpStep, WE_UINT16 *pusSize);

WE_INT32 Sec_X509GetCertExtension(WE_HANDLE hSecHandle, WE_UCHAR *pucExtensions, 
                                  WE_INT32 *piExtnId, WE_INT32 *piCritical, 
                                  WE_UCHAR **ppucExtnValue, WE_UINT16 *pusExtnValueLen, 
                                  WE_UINT16 *pusJumpStep);

WE_INT32 Sec_X509GetKeyUsage(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtnValue, 
                             WE_UINT8 *pucKeyUsageBit0To7, WE_UINT8 *pucKeyUsageBit8To15); 

WE_INT32 Sec_X509GetKeyUsageEx(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtnValue, 
                               WE_INT32 *piAnyExtendedKeyUsage, WE_INT32 *piServerAuth, 
                               WE_INT32 *piClientAuth, WE_INT32 *piCodeSigning, 
                               WE_INT32 *piEmailProtection, WE_INT32 *piTimeStamping, 
                               WE_INT32 *piOcspSigning);

WE_INT32 Sec_X509GetBasicConstraint(WE_HANDLE hSecHandle, const WE_UCHAR *pucExtnValue, 
                                    WE_INT32 *piCa, WE_INT32 *piHasPathLenConstraint, 
                                    WE_UINT8 *pucPathLenConstraint);

WE_INT32 Sec_X509GetRsaModAndExp(WE_HANDLE hSecHandle, const WE_UCHAR *pucPublicKeyVal, 
                                 WE_UCHAR **ppucRsaModulus, WE_UINT16 *pusModLen, 
                                 WE_UCHAR **ppucRsaExponent, WE_UINT16 *pusExponentLen);


#endif /* endif SEC_X509PARSER_H */ 

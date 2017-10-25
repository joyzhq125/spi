/*=====================================================================================
    HEADER NAME : sec_wtlsparser.h
    MODULE NAME : SEC

    PRE-INCLUDE FILES DESCRIPTION

    GENERAL DESCRIPTION
        This file be included by all wtls cert Programmer.

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
#ifndef SEC_WTLSPARSER_H
#define SEC_WTLSPARSER_H


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 Sec_WtlsGetCertFieldLength(WE_HANDLE hSecHandle, 
                                    const WE_UCHAR *pucWtlsCert, WE_UINT16 *pusCertLen);

WE_INT32 Sec_WtlsCertParse(WE_HANDLE hSecHandle, const WE_UCHAR *pucWtlsCert, 
                           WE_UINT16 *pusCertLen, St_SecCertificate *pstCert);

WE_INT32 Sec_WtlsGetIssuerInfo( WE_HANDLE hSecHandle,
                                const WE_UCHAR *pucIssuer, WE_UINT16 usIssuerLen, 
                                WE_UINT8 *pucNbrIssuerAttributes, St_SecNameAttribute *pstIssuerA );

WE_INT32 Sec_WtlsGetSubjectInfo(WE_HANDLE hSecHandle, const WE_UCHAR *pucWtlsCert,
                                WE_UINT8 *pucSubjectIdentifierType, WE_UINT16 *pusSubjectCharacterSet,
                                WE_UINT8 *pucSubjectLen, WE_UCHAR **ppucSubject);

WE_INT32 Sec_WtlsGetPubKey( WE_HANDLE hSecHandle, const WE_UCHAR *pucWtlsCert,
                            WE_UINT16 *pusExponentLen, WE_UCHAR** ppucRsaExponent,
                            WE_UINT16 *pusModLen, WE_UCHAR** ppucRsaModulus );

WE_INT32 Sec_WtlsCreateCertificate( WE_HANDLE hSecHandle, WE_UCHAR *pucCert, WE_UINT16 *pusCertLen, 
                                    WE_UCHAR *pucPubkey, WE_UINT16 usPubkeyLen );


#endif /* endif SEC_WTLSPARSER_H */

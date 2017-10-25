/*==================================================================================================
    HEADER NAME : isigntext.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the interface function prototype ,and the related structure 
        used in these interfaces.This is a public head file of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/
#ifndef _ISIGNTEXT_H_
#define _ISIGNTEXT_H_

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
typedef struct _ISignText  ISignText;

/************************************************************************/
/* Response Event                                                                                                    */
/************************************************************************/
#define G_SIGNTEXT_RESP                     0x10


/***************************************************************************
* The result value from response
***************************************************************************/
#define G_SEC_NOTSTORE_CONTRACT             0x11

/*************************************************************************
* Macro used in signText
*************************************************************************/
#define G_SEC_SIGN_NOKEY                    0x00
#define G_SEC_SIGN_SHAKEY                   0x01
#define G_SEC_SIGN_SHACAKEY                 0x02

#define G_SEC_SIGN_RETURN_HASHEDKEY         0x02
#define G_SEC_SIGN_RETURN_CERTIFICATE       0x04


#define G_SEC_SIGN_WTLS_CERTIFICATE         0x02
#define G_SEC_SIGN_X509_CERTIFICATE         0x03
#define G_SEC_SIGN_URL_CERTIFICATE          0x05

#define G_SEC_SIGN_NOERROR                  0x00
#define G_SEC_SIGN_MISSINGCERTIFICATE       0x01
#define G_SEC_SIGN_USERCANCELED             0x02
#define G_SEC_SIGN_OTHERERROR               0x03

/*about dialog */
/*added by Bird 070123 ,comply with signtext*/
#ifndef G_Evt_SELECTCERT
#define    G_Evt_SELECTCERT     0x30

typedef struct tagSt_SelectCert
{
    WE_INT32 nbrOfCerts; 
    WE_CHAR** ppcCertName;  
}St_SelectCert;

typedef struct tagSt_SelectCertAction
{
    WE_INT32  iSelId;
    WE_BOOL   bIsOk;
}St_SelectCertAction;
#endif

/*******************************************************************************
*   Type Define Section
*******************************************************************************/

/************************************************************************/
/* Response Structure                                                                                              */
/************************************************************************/

typedef struct tagSt_SecSignTextResp
{
   WE_INT32  iTargetID;    
   WE_INT32  iSignId;
   WE_CHAR * pcSignature;
   WE_CHAR * pcHashedKey;
   WE_CHAR * pcCertificate;
   WE_UINT16 usSigLen;
   WE_UINT16 usHashedKeyLen;
   WE_UINT16 usCertificateLen;
   WE_UINT8  ucAlgorithm;
   WE_UINT8  ucCertificateType;
   WE_UINT8  ucErr;
}St_SecSignTextResp;


/*The following is ISigntext interface and struct definition. */

WE_UINT32 ISignText_Handle
(
    ISignText * pISec,
    WE_INT32 iTargetID,
    WE_INT32 iSignId,  
    WE_CHAR * pcText,
    WE_INT32 iKeyIdType, 
    WE_CHAR * pcKeyId,
    WE_INT32 iKeyIdLen,
    WE_CHAR * pcStringToSign,
    WE_INT32 iStringToSignLen,
    WE_INT32 iOptions
);
WE_UINT32 ISignText_EvtSelectCertAction
(
    ISignText * pISec,
    WE_INT32 iTargetID,
    St_SelectCertAction stSelectCert
);
#ifdef G_SEC_CFG_SHOW_PIN
WE_UINT32 ISignText_EvtPinAction
(
    ISignText * pISec,
    WE_INT32 iTargetID,
    St_PinAction stPin
);
#endif

#endif/*_ISIGNTEXT_H_*/




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
typedef struct ISignText ISignText;

AEEINTERFACE(ISignText) 
{
   INHERIT_IBase(ISignText);

   /*register callback function*/
   WE_UINT32 (*RegSecClientEvtCB)(ISignText * pMe,WE_HANDLE hPrivData,Fn_ISecEventHandle pcbSecEvtFunc);

   WE_UINT32 (*Handle)(ISignText * pMe, WE_INT32 iTargetID, WE_INT32 iSignId,  WE_CHAR * pcText,
                   WE_INT32 iKeyIdType, WE_CHAR * pcKeyId, WE_INT32 iKeyIdLen,
                   WE_CHAR * pcStringToSign, WE_INT32 iStringToSignLen, WE_INT32 iOptions);
    
    /*added by Bird 070123*/
    WE_UINT32 (*EvtSelCertAction)(ISignText* pMe,WE_INT32 iTrargetId,St_SelectCertAction stSelectCert);

};
/*-------------------------------------------------------------------
Extension structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
struct ISignText
{
   AEEVTBL(ISignText) *pvt;
   WE_UINT32   m_nRefs;           
   IShell      *m_pIShell;        
   IModule     *m_pIModule;
   /*Add variable here*/
   WE_HANDLE   hPrivateData;
   WE_HANDLE   hSecPrivDataCB;
   WE_HANDLE   hcbSecEvtFunc;   
   /*added by Bird 061122*/
   WE_INT32    iIFType;     
   WE_HANDLE   hUserCertKeyPair;    
};

/*Public I Sec interface opened to external*/
#define ISignText_AddRef(p)                             AEEGETPVTBL((p),ISignText)->AddRef((p))                            
#define ISignText_Release(p)                            AEEGETPVTBL((p),ISignText)->Release((p))                            

#define ISignText_RegSecClientEvtCB(p,v,c)              AEEGETPVTBL((p),ISignText)->RegSecClientEvtCB((p),(v),(c))

#define ISignText_Handle(p,i,s,t,d,l,k,e,n,o)           AEEGETPVTBL((p),ISignText)->Handle((p),(i),(s),(t),(d),(l),(k),(e),(n),(o))

/*added by Bird 070123*/
#define ISignText_EvtSelCertAction(p,t,s)         AEEGETPVTBL((p),ISignText)->EvtSelCertAction((p),(t),(s))


#endif/*_ISIGNTEXT_H_*/




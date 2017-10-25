/*==================================================================================================
    HEADER NAME : sec_pubdata.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the global data structure , and all the member is used as global 
        data, and is initialized when sec module is created.
    
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
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef _SECPUBDATA_H_
#define _SECPUBDATA_H_

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef struct tagSt_PublicData
{
    WE_HANDLE                   hWeHandle;

    WE_VOID*                   pvRespCBData;
    WE_VOID*                   pvUeCBData;
     /*modified  by bird 060118,for malloc failed improve*/
    WE_INT32                    iRespEvent;
    WE_INT32                    iUeEvent;
    St_SecMessage *             pstMsgQueueHead;    
    St_SecMessage *             pstMsgQueueTail;    
    St_SecMessage *             pstMsgFreeList;     
    WE_INT32                    iMsgFreeListLen;                          
    Fn_SecMessageFunc *         apfnSecMsgFunc[M_SEC_MSG_MAX_REGS_FUNC_POINTER];
    WE_INT32                    iTooOldCertId;
    WE_INT32                    iUserReq;
    WE_INT32                    iUeShown;
    WE_INT32                    iNocertUeShown;

    WE_INT32                    iOldRootCertCount;
    
    WE_INT32                    iNbrPinTries;    
    WE_CHAR                     cCertType;
    WE_INT32                    iNbrofCerts; 
    WE_INT32                    iMaxNbrCerts;
    WE_INT32 *                  piCertIds;
    WE_UINT8                    ucKeyType;
    E_SecPinReason              ePinCause;
    WE_UINT16                   usCharSet;
    WE_CHAR *                   pcDisplayName;
    WE_INT32                    iDisplayNameLen;
    St_SecCertName *            pstCertNames;
    WE_INT32                    iCertId;
    WE_INT32                    iIndex;
    WE_INT32                    iContractIndex;
    WE_INT32                    iNbrContracts;
    WE_INT32                    iMaxNbrContracts;
  
    St_SecContractInfo *        pstContractVec;
    St_SecInfo                  *pstInfo;
    St_SecSessionRec            *pstSessionCache;    
    St_SecPeerRec               *pstPeerCache;  
    WE_CHAR                     *pcHaveLoad;  
    
    WE_UINT32                   uiCertId;          
    WE_UINT32                   uiCertCount;    
    WE_UINT32                   uiKeyId;            
    WE_UINT32                   uiKeyCount; 
    WE_UINT32                   uiUserFileId;  
    WE_UINT32                   uiFileId;
    WE_INT32                    iNameCount;
    WE_INT32                    iURLCount;
    WE_INT32                    iVerifiedOk;
    WE_INT32                    iTmpTooOldCertId;
    WE_INT32                    iAskedUser;
    WE_INT32                    iHashFalse;
    WE_UINT16                   usFriendlyNameLen;    
    WE_INT16                    sCharSet;
    WE_UINT16                   usTmpUserCertLen;                
    WE_INT8                     cNameType;  
    WE_UINT8                    ucKeyUsage;        
    WE_UINT8                    ucAlg;                            
    WE_UCHAR                    aucPublicKeyHash[M_SEC_KEY_HASH_SIZE];        
    WE_UCHAR                    aucCAPublicKeyHash[M_SEC_KEY_HASH_SIZE];   
    WE_UCHAR *                  pucFriendlyName;              
    WE_UCHAR *                  pucTmpUserCert;
    WE_INT32                    iStepLocalRoot;
    WE_INT32                    iSepLocalHash;
    WE_HANDLE                   hIX509Chain;
    WE_HANDLE                   hStoreCaStateForm;
    WE_HANDLE                   hStoreUserStateForm;
    WE_HANDLE                   hHashVerifyStateForm;
    WE_HANDLE                   hSignVerifyStateForm;
    WE_HANDLE                   hDeliveryStateForm;
    
    WE_INT32                    iNbrStoredFiles;
    WE_UINT8                    ucInitialised;
    
    St_SecCertTransferInfo *    pstCertTransferInfo;    
    St_SecWimElement *            pstHeadElement;                                
    
    WE_VOID*                    pvCurUeResp;
    WE_INT32                    iWaitingForUe;
    WE_VOID*                    pvCurSecResp;
    WE_INT32                    iWaitingForSec;
    WE_VOID*                    pvCurWimResp;
    WE_INT32                    iWaitingForWim;
    WE_INT32                    iStates;
    WE_INT32                    iWantsToRun;
    E_SecDpFncId                eFuncId;
    WE_VOID*                    pvCurFnc;
    St_SecDpParameter           stQueueHead; 
    St_SecDpParameter           stQueueTail; 
    WE_INT32                    iSecInitialised;
    WE_INT32                    iRwiStates;
    WE_INT32                    iRvuStates;
    WE_INT32                    iRvuVerifiedOnWIM;
    WE_INT32                    iRvutiVerifiedOnWIM;
    WE_INT32                    iRvutiStates;
    WE_INT32                    iRcpVerifiedOnWIM;
/*for tls,add by tang 070108*/
    WE_HANDLE                 pvAlgHandle;
    St_SecTlsAsn1Certificate *pstAsn1Cert;
    WE_UINT32                auiCertChainIds[3];
    WE_UCHAR                 *pucKeyId;
    WE_INT32                 iSignAlg;
    WE_UINT8                  ucChainDepth;

    /*modify linux by Bird 070313*/
    WE_INT32                    iFd;

}St_PublicData;

#endif

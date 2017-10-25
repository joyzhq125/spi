/*==================================================================================================
    HEADER NAME : isec.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the interface function prototype of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date              Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2007-03-14  BirdZhang       None         Draft
==================================================================================================*/


/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/

/*common*/
WE_INT32 Sec_New( WE_INT32 iType, WE_INT32 iFd,WE_HANDLE* phHandle);
WE_UINT32 Sec_Release (WE_HANDLE pMe);

/*isecw*/
WE_INT32 SecW_SslGetWMasterSec(WE_HANDLE pMe, WE_INT32 iMasterSecretID);
WE_UINT32 SecW_CompSign(WE_HANDLE pMe, WE_INT32 iTargetID,
                        WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                        WE_UCHAR * pucBuf, WE_INT32 iBufLen,
                        WE_UINT8 ucConnType);
WE_UINT32 SecW_GetUserCert(WE_HANDLE pMe, WE_INT32 iTargetID, 
                           WE_VOID *  pvBuf, WE_INT32 iBufLen,
                           WE_UINT8   ucConnType);
WE_UINT32 SecW_KeyExchange(WE_HANDLE pMe, WE_INT32 iTargetID, WE_VOID *pvParam,
                           WE_UINT8 ucAlg,WE_UCHAR * pucRandval,WE_UINT8 ucConnType);
WE_UINT32 SecW_StopConnection(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecurityId,WE_UINT8   ucConnType);

WE_UINT32 SecW_GetCipherSuite(WE_HANDLE pMe, WE_INT32 iTargetID,WE_UINT8   ucConnType);
WE_UINT32 SecW_GetPrfResult(WE_HANDLE pMe, WE_INT32 iTargetID, WE_UINT8 ucAlg, 
                            WE_INT32 iMasterSecretId,  WE_UCHAR * pucSecret, 
                            WE_INT32 iSecretLen,  WE_CHAR * pcLabel, 
                            WE_UCHAR * pucSeed, WE_INT32 iSeedLen,
                            WE_INT32 iOutputLen,WE_UINT8 ucConnType);

WE_UINT32 SecW_VerifySvrCertChain(WE_HANDLE pMe, WE_INT32 iTargetID,
                                  WE_VOID *pvBuf,  WE_INT32 iBufLen,
                                  WE_UINT8 ucConnType);

WE_UINT32 SecW_SetupConnection(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId, 
                               WE_INT32 iSecurityId, WE_INT32 iFullHandshake,
                               St_SecSessionInformation stSessionInfo,WE_UINT8   ucConnType);

WE_UINT32 SecW_StopConnection(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecurityId,WE_UINT8   ucConnType);

WE_UINT32 SecW_RemovePeer(WE_HANDLE pMe, WE_INT32 iMasterSecretId);

WE_UINT32 SecW_AttachPeerToSession(WE_HANDLE pMe,  WE_UCHAR * pucAddress,
                                   WE_INT32 iAddressLen, WE_UINT16 usPortnum, 
                                   WE_INT32 iMasterSecretId,WE_UINT8   ucConnType);

WE_UINT32 SecW_SearchPeer(WE_HANDLE pMe, WE_INT32 iTargetID,  WE_UCHAR * pucAddress, 
                          WE_INT32 iAddressLen, WE_UINT16 usPortnum,WE_UINT8   ucConnType);
WE_UINT32 SecW_EnableSession(WE_HANDLE pMe, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive);

WE_UINT32 SecW_DisableSession(WE_HANDLE pMe, WE_INT32 iMasterSecretId);

WE_UINT32 SecW_SessionGet(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId);

WE_UINT32 SecW_SessionRenew(WE_HANDLE pMe, WE_INT32 iMasterSecretId, 
                            WE_UINT8 ucSessionOptions,  WE_UCHAR * pucSessionId, 
                            WE_UINT8 ucSessionIdLen,  WE_UINT8 *aucCipherSuite,
                            WE_UINT8 ucCompressionAlg,  WE_UCHAR * pucPrivateKeyId,
                            WE_UINT32 uiCreationTime);
WE_UINT32 SecW_EvtSelectCertAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_SelectCertAction stChooseCerByName);

WE_UINT32 SecW_EvtConfirmAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ConfirmAction stConfirm);

WE_UINT32 SecW_EvtShowDlgAction(WE_HANDLE pMe,WE_INT32 iTargetId,
                                WE_VOID* pvSrcData,WE_UINT32 uiLength);

/*isecb*/
WE_UINT32 SecB_GetCertNameList(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iCertType);

WE_UINT32 SecB_GenKeyPair(WE_HANDLE pMe, WE_INT32 iTargetID, WE_UINT8 ucKeyType);
WE_UINT32 SecB_PinModify(WE_HANDLE pMe, WE_INT32 iTargetID, WE_UINT8 ucKeyType);
WE_UINT32 SecB_GetCurSvrCert(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecID);
WE_UINT32 SecB_GetSessionInfo(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecID);
WE_UINT32 SecB_GetWtlsCurClasss(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecID);
WE_UINT32 SecB_GetContractsList(WE_HANDLE pMe, WE_INT32 iTargetID);
WE_UINT32 SecB_TransferCert(WE_HANDLE pMe,St_SecCertContent stCertContent);
WE_UINT32 SecB_GetRequestUserCert(WE_HANDLE pMe, St_SecUserCertReqInfo stUserCertReqInfo);
WE_UINT32 SecB_EvtChangePinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ChangePinAction stChangePin);
WE_UINT32 SecB_EvtCreatePinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_CreatePinAction stGenPin);
WE_UINT32 SecB_EvtHashAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_HashAction stHash);
WE_UINT32 SecB_EvtShowCertContentAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ShowCertContentAction stShowCer );
WE_UINT32 SecB_EvtShowCertListAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ShowCertListAction stShowCertName);
WE_UINT32 SecB_EvtShowContractsListAction(WE_HANDLE pMe,WE_INT32 iTargetId,St_ShowContractsListAction stStoredContracts);
WE_UINT32 SecB_EvtStoreCertAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_StoreCertAction stStoreCert);
WE_UINT32 SecB_EvtShowContractContentAction(WE_HANDLE pMe,WE_INT32 iTargetID,
                                                   St_ShowContractContentAction stShowContract);
WE_UINT32 SecB_RegClientEvtCB(WE_HANDLE pMe,WE_HANDLE hPrivData,Fn_ISecEventHandle pcbSecEvtFunc);
WE_UINT32 SecB_EvtConfirmAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ConfirmAction stConfirm);
WE_UINT32 SecB_EvtNameConfirmAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_NameConfirmAction stConfName);
WE_UINT32 SecB_EvtPinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_PinAction stPin);

/*isigntext*/
WE_UINT32 SignText_Handle(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSignId,  WE_CHAR * pcText,
                          WE_INT32 iKeyIdType, WE_CHAR * pcKeyId, WE_INT32 iKeyIdLen,
                          WE_CHAR * pcStringToSign, WE_INT32 iStringToSignLen, WE_INT32 iOptions);
WE_UINT32 SignText_EvtSelectCertAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_SelectCertAction stSelectCert);
WE_UINT32 SignText_EvtPinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_PinAction stPin);















































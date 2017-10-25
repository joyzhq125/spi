/*==================================================================================================
    HEADER NAME : sec_dp.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be update later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   steven ding        None         Draft
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef SEC_DP_H
#define SEC_DP_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define M_SEC_SELF_FREE(p)\
{\
    if(NULL != p)\
    {\
        WE_FREE(p);\
        (p) = NULL;\
    }\
}

/*******************************************************************************
*   Type Define Section
*******************************************************************************/

typedef enum tagE_SecDpMsgType
{             
    M_SEC_DP_MSG_GET_CIPHERSUITE = M_SEC_MSG_DST_HANDLE_MAIN + 2,  
    M_SEC_DP_MSG_EXCHANGE_WTLS_KEY,                            
    M_SEC_DP_MSG_WTLS_PRF,                                     
    M_SEC_DP_MSG_VERIFY_WTLS_CERT_CHAIN,                   
    M_SEC_DP_MSG_GET_WTLS_CLIENT_CERT,                    
    M_SEC_DP_MSG_COMPUTE_WTLS_SIGN,                          
    M_SEC_DP_MSG_SIGN_TEXT ,                            
    M_SEC_DP_MSG_ESTABLISH_CONN,                            
    M_SEC_DP_MSG_TERMINATE_CONN,                           
    M_SEC_DP_MSG_RM_PEER_LINKS,                               
    M_SEC_DP_MSG_LINK_PEER_SESSION,                          
    M_SEC_DP_MSG_LOOKUP_PEER,                                  
    M_SEC_DP_MSG_ACTIVE_SESSION,                               
    M_SEC_DP_MSG_INVALID_SESSION,                             
    M_SEC_DP_MSG_FETCH_SESSION,                                
    M_SEC_DP_MSG_UPDATE_SESSION,                             
    M_SEC_DP_MSG_STORE_CERT,                                  
    M_SEC_DP_MSG_TRUSTED_CA_INFO,                             
    M_SEC_DP_MSG_DELIVERY_CERT,                              
    M_SEC_DP_MSG_VIEW_CERT_NAMES,                              
    M_SEC_DP_MSG_GEN_KEY_PAIR,                                  
    M_SEC_DP_MSG_GET_USER_PUB_KEY,                             
    M_SEC_DP_MSG_CHANGE_PIN,                                   
    M_SEC_DP_MSG_VIEW_CUR_CERT,                                
    M_SEC_DP_MSG_VIEW_SESSION,                                 
    M_SEC_DP_MSG_GET_CUR_SEC_CLASS,                             
    M_SEC_DP_MSG_CLEANUP_SESSION,                              
    M_SEC_DP_MSG_VIEW_CONTRACTS,                                
    M_SEC_DP_MSG_GET_CERT,                                      
    M_SEC_DP_MSG_CHANGE_WTLS_CERT_ABLE,
    M_SEC_DP_MSG_RM_CERT,                                      
    M_SEC_DP_MSG_RM_CONTRACT,                                  
    M_SEC_DP_MSG_EXECUTE_COMMAND,         
    M_SEC_DP_MSG_REQ_USER_CERT,  
    M_SEC_DP_MSG_STORE_CERT_RESP,                             
    M_SEC_DP_MSG_TRUSTED_CA_INFO_RESP,                         
    M_SEC_DP_MSG_DELVERY_CERT_RESP,   
    M_SEC_DP_MSG_ENCRYPT_PKC_RESP,/*E_SEC_MSG_ENCRYPT_PKC_RESP*/
    M_SEC_DP_MSG_DECRYPT_PKC_RESP,/*E_SEC_MSG_DECRYPT_PKC_RESP*/
    M_SEC_DP_MSG_VERIFY_SIGN_RESP,/*E_SEC_MSG_VERIFY_SIGN_RESP*/
    M_SEC_DP_MSG_COMPUTE_SIGN_RESP,  /*E_SEC_MSG_COMPUTE_SIGN_RESP*/
    M_SEC_DP_MSG_GEN_KEYPAIR_RESP,/* E_SEC_MSG_GEN_KEYPAIR_RESP*/
    /*action function asynchronism*/
    M_SEC_DP_MSG_SHOWCONTRACTLIST,
    M_SEC_DP_MSG_SHOWCONTRACTCONTENT,
    M_SEC_DP_MSG_SHOWCERTLIST,
    M_SEC_DP_MSG_SHOWCERTCONTENT,
    /*added by Bird 070130*/
    M_SEC_DP_MSG_HASH,
    M_SEC_DP_MSG_STORECERT,
    M_SEC_DP_MSG_CONFIRM,
    M_SEC_DP_MSG_NAMECONFIRM,
    M_SEC_DP_MSG_SELECTCERT


}E_SecDpMsgType;

typedef enum tagE_SecDpFncId
{
    E_SEC_DP_NONE,                            
    E_SEC_DP_INIT,                             
    E_SEC_DP_ESTABLISHED_CONN,                   
    E_SEC_DP_TERMINATED_CONN,                    
    E_SEC_DP_GET_CIPHERSUITE,                     
    E_SEC_DP_XCH_WTLS_KEY,                         
    E_SEC_DP_WTLS_PRF,                            
    E_SEC_DP_VERIFY_WTLS_CERT_CHAIN,               
    E_SEC_DP_GET_WTLS_CLIENT_CERT,                 
    E_SEC_DP_COMPUTE_WTLS_SIGN,                  
    E_SEC_DP_XCH_KEY,                             
    E_SEC_DP_PRF,                                 
    E_SEC_DP_COMPUTE_SIGN,                       
    E_SEC_DP_VERIFY_CERT_CHAIN,                
    E_SEC_DP_VERIRY_USER_CERT_CHAIN,         
    E_SEC_DP_VERIRY_ROOT_CERT,                     
    E_SEC_DP_LOOKUP_PRIV_KEY,                      
    E_SEC_DP_RM_PEER_LINKS,                       
    E_SEC_DP_LINK_PEER_SESSION,                   
    E_SEC_DP_LOOKUP_PEER,                          
    E_SEC_DP_ACTIVE_SESSION,                      
    E_SEC_DP_INVALID_SESSION,                   
    E_SEC_DP_CLEANUP_SESSION,                     
    E_SEC_DP_FETCH_SESSION,                  
    E_SEC_DP_UPDATE_SESSION,                        
    E_SEC_DP_STORE_CA_CERT,                        
    E_SEC_DP_STORE_USER_CERT ,                   
    E_SEC_DP_STROE_CERT,                           
    E_SEC_DP_HASH_VERIFICATION,                    
    E_SEC_DP_VRRIFY_SIGN,                         
    E_SEC_DP_VERIFY_CERT,                           
    E_SEC_DP_VIEW_CERT_NAMES,                      
    E_SEC_DP_GET_NBR_CERTS,                      
    E_SEC_DP_GET_CERT_NAMES,                      
    E_SEC_DP_GET_CONN_CERT_NAMES,                   
    E_SEC_DP_GET_CERT_CHAIN,                       
    E_SEC_DP_GET_CERT_IDS,                         
    E_SEC_DP_GET_CERT,                            
    E_SEC_DP_GETCERT_RESP,                       
    E_SEC_DP_CHANGE_WTLS_CERT_ABLE,
    E_SEC_DP_CHANGE_WTLS_CERT_ABLE_RESP,                     
    E_SEC_DP_RM_CERT,                           
    E_SEC_DP_RMCERT_RESP,                         
    E_SEC_DP_CHOOSE_CERT,                       
    E_SEC_DP_SELF_SIGNED_CERT,                      
    E_SEC_DP_CERT_IS_EXIST,                        
    E_SEC_DP_STORE_USER_CERT_DATA,                  
    E_SEC_DP_STORE_CERT_DLG,                       
    E_SEC_DP_CONFIRM_DLG,                          
    E_SEC_DP_VIEW_CUR_CERT,                        
    E_SEC_DP_VIEW_SESSION,                          
    E_SEC_DP_GET_CUR_SEC_CLASS,                     
    E_SEC_DP_GET_UCERT_AND_SKEY,                    
    E_SEC_DP_GET_USER_CERT,                        
    E_SEC_DP_GEN_KEY_PAIR,                         
    E_SEC_DP_RM_KEYS_AND_CERTS,                     
    E_SEC_DP_GET_USER_PUB_KEY ,                    
    E_SEC_DP_PIN_DLG,                            
    E_SEC_DP_PIN_INFO,                              
    E_SEC_DP_VERIFY_PIN,                           
    E_SEC_DP_CHANGE_PIN,                           
    E_SEC_DP_CHANGE_PIN_DLG,                        
    E_SEC_DP_GEN_PIN,                              
    E_SEC_DP_UI_SIGN_TEXT,                         
    E_SEC_DP_GET_SIGN_MATCHES,                     
    E_SEC_DP_VIEW_CONTRACTS,                        
    E_SEC_DP_GET_CONTRACTS_RESP,                   
    E_SEC_DP_GET_CONTRACTS,                        
    E_SEC_DP_RM_CONTRACT,                           
    E_SEC_DP_RM_CNTRACT_RESP,                       
    E_SEC_DP_STORE_CONTRACT_RESP,                   
    E_SEC_DP_MIME_RESP,                             
    E_SEC_DP_READ_VIEWS_CERTS,                      
    E_SEC_DP_HASH_DLG,                             
    E_SEC_DP_VERIFY_HASH, 
    E_SEC_DP_GET_REQ_USER_CERT,     
    E_SEC_DP_GET_REQ_USER_CERT_RESP,
    /*action function asynchronism by bird 061221 */   
    E_SEC_DP_SHOWCONTRACTLIST,
    E_SEC_DP_SHOWCONTRACTCONTENT,
    E_SEC_DP_SHOWCERTTLIST,
    E_SEC_DP_SHOWCERTCONTENT
}E_SecDpFncId;


typedef struct tagSt_SecDpParameter 
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    struct tagSt_SecDpParameter*            pstPrev;
    struct tagSt_SecDpParameter*            pstNext;
}St_SecDpParameter;


typedef struct tagSt_SecDpGetWtlsCipherSuite
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    St_SecDpParameter*                      pstPrev;
    St_SecDpParameter*                      pstNext;
    St_SecWtlsGetCipherSuite*               pstStrct;
    
}St_SecDpGetWtlsCipherSuite;
        
typedef struct tagSt_SecDpWtlsKeyXch 
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    St_SecDpParameter*                      pstPrev;
    St_SecDpParameter*                      pstNext;
    St_SecWtlsKeyExchange*                  pstStrct;
}St_SecDpWtlsKeyXch ;

typedef struct tagSt_SecDpWtlsPrf 
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    St_SecDpParameter*                      pstPrev;
    St_SecDpParameter*                      pstNext;
    St_SecWtlsGetPrfResult*                 pstStrct;
}St_SecDpWtlsPrf ;

typedef struct tagSt_SecDpVerifyWtlsCertChain 
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    St_SecDpParameter*                      pstPrev;
    St_SecDpParameter*                      pstNext;
    St_SecWtlsVerifySvrCert*                pstStrct;
}St_SecDpVerifyWtlsCertChain;

typedef struct tagSt_SecDpGetWtlsClientCert
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    St_SecDpParameter*                      pstPrev;
    St_SecDpParameter*                      pstNext;
    St_SecWtlsGetUsrCert*                   pstStrct;
}St_SecDpGetWtlsClientCert;

typedef struct tagSt_SecDpComputeWtlsSign
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    St_SecDpParameter*                      pstPrev;
    St_SecDpParameter*                      pstNext;
    St_SecWtlsCompSign*                     pstStrct;
}St_SecDpComputeWtlsSign;


typedef struct tagSt_SecDpSignText
{
    E_SecDpFncId                            eType;
    WE_HANDLE hItype;
    St_SecDpParameter*                      pstPrev;
    St_SecDpParameter*                      pstNext;
    St_SecSignText*                         pstStrct;
  }St_SecDpSignText;

typedef struct tagSt_SecDpConnEst 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecSetupConnection*                    pstStrct;
}St_SecDpConnEst;

typedef struct tagSt_SecDpConnTmt
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecStopConnection*                     pstStrct;
}St_SecDpConnTmt ;

typedef struct tagSt_SecDpRmPeerLinks 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecRemovePeer*                         pstStrct;
}St_SecDpRmPeerLinks ;

typedef struct tagSt_SecDpPeerLinkSession  
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecAttachPeer*                         pstStrct;
}St_SecDpPeerLinkSession;

typedef struct tagSt_SecDpLookupPeer
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecSearchPeer*                         pstStrct;
}St_SecDpLookupPeer;

typedef struct tagSt_SecDpActiveSession 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecEnableSession*                      pstStrct;
}St_SecDpActiveSession;

typedef struct tagSt_SecDpInvalidSession
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecDisableSession*                     pstStrct;
}St_SecDpInvalidSession;

typedef struct tagSt_SecDpfetchSession
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecFetchSessionInfo*                   pstStrct;
}St_SecDpfetchSession;

typedef struct tagSt_SecDpUpdateSession 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecUpdateSession*                      pstStrct;
}St_SecDpUpdateSession;

typedef struct tagSt_SecDpStoreCert 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecSaveCert*                           pstStrct;
}St_SecDpStoreCert;

typedef struct tagSt_SecDpCaInfo 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecTrustedCaInfo*                      pstStrct;
}St_SecDpCaInfo;

typedef struct tagSt_SecDpDeliveryCert 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecCertDelivery*                       pstStrct;
}St_SecDpDeliveryCert;

typedef struct tagSt_SecDpMimeResp
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecSaveCertResp*                       pstStrct;
}St_SecDpMimeResp ;

typedef struct tagSt_SecDpViewCertNames 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGetCertNameList*                    pstStrct;
}St_SecDpViewCertNames;

typedef struct tagSt_SecDpVerifyCertChain 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecVerifyCertChain*                    pstStrct;
  WE_UCHAR*                                 pucBuf;
  WE_UINT32                                 uiBufLen;
}St_SecDpVerifyCertChain;

typedef struct tagSt_SecDpGenKeyPair 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGenKeyPair*                         pstStrct;
}St_SecDpGenKeyPair;

typedef struct tagSt_SecDpRmKeysAndUserCerts 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecDelKeysUserCerts*                   pstStrct;
}St_SecDpRmKeysAndUserCerts;

typedef struct tagSt_SecDpGetUserPubKey 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGetPublicKey*                       pstStrct;
}St_SecDpGetUserPubKey;

typedef struct tagSt_SecDpChangePin 
{
  E_SecDpFncId                              eType;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  WE_HANDLE hItype;
  St_SecModifyPriKeyPin*                    pstStrct;
}St_SecDpChangePin;

typedef struct tagSt_SecDpViewCurCert 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecViewCurCert*                        pstStrct;
  WE_INT32                                  iOptions;
}St_SecDpViewCurCert;

typedef struct tagSt_SecDpViewSessionInfo 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecViewSessionInfo*                    pstStrct;
  WE_INT32                                  iOptions;
}St_SecDpViewSessionInfo;

typedef struct tagSt_SecDpGetCurSecClass 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGetSecCurClass*                     pstStrct;
}St_SecDpGetCurSecClass;

typedef struct tagSt_SecDpCleanUpSession
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecClrSession*                         pstStrct;
}St_SecDpCleanUpSession;

typedef struct tagSt_SecDpGetCert 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGetCertContent*                     pstStrct;
  WE_INT32                                  iOptions;
}St_SecDpGetCert ;

typedef struct tagSt_SecDpChangeWtlsCertAble 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecChangeWtlsCertAble*                 pstStrct;
}St_SecDpChangeWtlsCertAble;

typedef struct tagSt_SecDpGetCertResp 
{
  E_SecDpFncId                              eType;  
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGetCertContentResp*                 pstStrct;
}St_SecDpGetCertResp;

typedef struct tagSt_SecDpRmCert 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecRemoveCert*                         pstStrct;
}St_SecDpRmCert;

typedef struct tagSt_SecDpRmCertResp 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecRemCertResp*                        pstStrct;
}St_SecDpRmCertResp;


typedef struct tagSt_SecDpViewContracts 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGetContractInfo*                    pstStrct;
}St_SecDpViewContracts;

typedef struct tagSt_SecDpGetContracts 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecGetContracts*                       pstStrct;
}St_SecDpGetContracts;

typedef struct tagSt_SecDpRmContract 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecRemoveContract*                     pstStrct;
}St_SecDpRmContract;

typedef struct tagSt_SecDpRmContractResp 
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecDelContractResp*                    pstStrct;
}St_SecDpRmContractResp;


typedef struct tagSt_SecDpUserCertReq
{
  E_SecDpFncId                              eType;
  WE_HANDLE hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecUserCertReqInfo*                    pstStrct;
}St_SecDpUserCertReq;
/*action function asynchronism*/
typedef struct tagSt_SecDpShowContractList
{
  E_SecDpFncId                              eType;
  WE_HANDLE                                 hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecShowContractListAction*             pstStrct;
}St_SecDpShowContractList;

typedef struct tagSt_SecDpShowContractContent
{
  E_SecDpFncId                              eType;
  WE_HANDLE                                 hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecShowContractContAction*             pstStrct;
}St_SecDpShowContractContent;

typedef struct tagSt_SecDpShowCertList
{
  E_SecDpFncId                              eType;
  WE_HANDLE                                 hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecShowCertListAction*             pstStrct;
}St_SecDpShowCertList;

typedef struct tagSt_SecDpShowCertContent
{
  E_SecDpFncId                              eType;
  WE_HANDLE                                 hItype;
  St_SecDpParameter*                        pstPrev;
  St_SecDpParameter*                        pstNext;
  St_SecShowCertContAction*             pstStrct;
}St_SecDpShowCertContent;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

WE_INT32 Sec_DpHandleTerminate (WE_HANDLE hSecHandle);
WE_VOID Sec_DpHandleInternal(WE_HANDLE hSecHandle);
WE_INT32 Sec_DpHandleInit (WE_HANDLE hSecHandle);
WE_INT32 Sec_DpHandleIsInitialised(WE_HANDLE hSecHandle);
WE_VOID Sec_DpHandleCleanUp(WE_HANDLE hSecHandle);
WE_INT32 Sec_DpHandleWimInit(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iPuk);
WE_INT32 Sec_DpHandleUserVerify(WE_HANDLE hSecHandle, WE_INT32 iTargetID, E_SecPinReason ePinCause);
WE_VOID Sec_DpHandleQueueDelById(WE_HANDLE hSecHandle);
WE_VOID Sec_DpResetVariable(WE_HANDLE hSecHandle);

#endif


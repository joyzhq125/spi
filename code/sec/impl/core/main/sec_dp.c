/*==================================================================================================
    HEADER NAME : sec_dp.c
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

/***************************************************************************************
*   Include File Section
****************************************************************************************/

#include "sec_comm.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define SEC_STATES_STOP                                         0
#define SEC_STATES_START                                        1

typedef St_PublicData                        St_SecGlobalData;

#define SEC_E_INITIALISED                   (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->ucInitialised)
#define SEC_CUR_UE_RESP                     (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurUeResp)
#define SEC_WAITING_FOR_UE_RESULT           (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForUe)
#define SEC_CUR_SEC_RESP                    (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurSecResp)
#define SEC_WAITING_FOR_SEC_RESULT          (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForSec)
#define SEC_CUR_WIM_RESP                    (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurWimResp)
#define SEC_WAITING_FOR_WIM_RESULT          (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForWim)
#define SEC_STATES                          (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iStates)
#define SEC_WANTS_TO_RUN                    (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iWantsToRun)
#define SEC_FUNCID                          (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->eFuncId)
#define SEC_CUR_FUNCTION                    (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurFnc)
#define SEC_QUEUE_HEAD                      (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->stQueueHead)
#define SEC_QUEUE_TAIL                      (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->stQueueTail)
#define SEC_INITIALISED                     (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iSecInitialised)
#define SEC_WIM_INITIALISED                 (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->ucInitialised)
#define SEC_STATES_RWI                      (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iRwiStates)
#define SEC_STATES_RVU                      (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iRvuStates)
#define SEC_VERIFIED_ON_WIM_RVU             (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iRvuVerifiedOnWIM)
#define SEC_VERIFIED_ON_WIM_RVUTI           (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iRvutiVerifiedOnWIM)
#define SEC_STATES_RVUTI                    (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iRvutiStates)
#define SEC_VERIFIED_ON_WIM_RCP             (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iRcpVerifiedOnWIM)

#define SEC_WE_HANDLE                       (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->hWeHandle) 
#define SEC_CONTRACTVEC                     (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->pstContractVec)

/*******************************************************************************
*   Function Define Section
*******************************************************************************/

/************************************************************************/
/*                 internal function declare                                                     */
/************************************************************************/

static WE_INT32 Sec_DpHandleUserVerifyInit(WE_HANDLE hSecHandle,
                                           WE_INT32 iTargetID);

static WE_VOID Sec_DpHandleQueueInit(WE_HANDLE hSecHandle);


static WE_VOID Sec_DpHandleQueueNodeRemove(St_SecDpParameter* pstNode);


static WE_VOID Sec_DpHandleQueueCleanUp(WE_HANDLE hSecHandle);


static St_SecDpParameter* Sec_DpHandleQueueNextNodeGet(WE_HANDLE hSecHandle);


static WE_VOID Sec_DpHandleGlobalVarialbeInit(WE_HANDLE hSecHandle);


static WE_VOID Sec_DpHandleGlobalVariableCleanUp(WE_HANDLE hSecHandle);


static WE_VOID Sec_DpHandlePinChange(WE_HANDLE hSecHandle,
                                     WE_INT32 iTargetID, 
                                     WE_UINT8 ucKeyType);


static WE_VOID Sec_DpHandleUserPubKeyGet(WE_HANDLE hSecHandle,
                                         WE_INT32 iTargetID, 
                                         WE_UINT8 ucKeyType);


static WE_VOID Sec_DpHandleKeysAndUserCertsRemove(WE_HANDLE hSecHandle,
                                                  WE_INT32 iTargetID, 
                                                  WE_UINT8 ucKeyType);


static WE_VOID Sec_DpHandleKeyPairGenerate(WE_HANDLE hSecHandle,
                                           WE_INT32 iTargetID, 
                                           WE_UINT8 ucKeyType, 
                                           WE_UINT8 ucAlg);


static WE_INT32 Sec_DpHandleInterVariableInit (WE_HANDLE hSecHandle);


static WE_INT32 Sec_DpHandleInterVariableCleanUp (WE_HANDLE hSecHandle);


static WE_VOID Sec_DpHandleSecInfoLog (WE_VOID); 


static WE_INT32 Sec_DpHandleRsaPubKeyConvert(St_SecPubKeyRsa stKey,
                                             WE_UCHAR** ppucPubKey, 
                                             WE_INT32* piOutPubKeyLen);


static WE_VOID Sec_DpHandleCurrentCertGet(WE_HANDLE hSecHandle,
                                          WE_INT32 iTargetID, 
                                          WE_INT32 iSecurityId,
                                          WE_INT32 iOptions);


static WE_VOID Sec_DpHandleSessionInfoView(WE_HANDLE hSecHandle,
                                           WE_INT32 iTargetID, 
                                           WE_INT32 iSecurityId ,
                                           WE_INT32 iOptions);


static WE_VOID Sec_DpHandleCurrentSecClassGet(WE_HANDLE hSecHandle, 
                                              WE_INT32 iTargetID, 
                                              WE_INT32 iSecurityId);


static WE_VOID Sec_DpHandleSessionCleanUp(WE_HANDLE hSecHandle, WE_INT32 iTargetID);

/* add by Sam [070119] */
#ifdef M_SEC_CFG_CAN_SIGN_TEXT
static WE_VOID Sec_DpHandleTextSign (WE_HANDLE hSecHandle,
                                     WE_INT32 iTargetID, WE_INT32 iSignId, 
                                     const WE_CHAR* pcText, const WE_CHAR* pcData, 
                                     WE_INT32 iDataLen, WE_INT32 iKeyIdType,
                                     const WE_CHAR* pcKeyId, WE_INT32 iKeyIdLen,
                                     WE_INT32 iOptions);
#endif
/* add by Sam [070119] */
#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))            
static WE_VOID Sec_DpHandleContractsView(WE_HANDLE hSecHandle, WE_INT32 iTargetID);


static WE_VOID Sec_DpHandleContractRemove(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                          WE_INT32 iContractId);
#endif

static WE_VOID Sec_DpHandleMimeResp(WE_HANDLE hSecHandle,WE_INT32 iTargetId, WE_INT32 iResult);


static WE_VOID Sec_DpHandleQueueNodeAdd(WE_HANDLE hSecHandle, St_SecDpParameter* pstNode);


static WE_INT32 Sec_DpHandleQueueIsEmpty(WE_HANDLE hSecHandle);


static WE_VOID Sec_DpHandleCurrentCertDeliver (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                               WE_INT32 iResult, WE_INT32 iOptions, 
                                               WE_UCHAR* pucCert);


static WE_VOID Sec_DpHandleSessionInfoDeliver (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                               WE_INT32 iResult, WE_INT32 iOptions, 
                                               St_SecSessionInfo* pstSessInf);


static WE_VOID Sec_DpHandleMain (WE_HANDLE hSecHandle, St_SecMessage* pstSig);

static WE_VOID Sec_DpHandleUserCertReq(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
            WE_UCHAR ucCertUsage, WE_UCHAR *pucCountry,WE_UINT16 usCountryLen, 
            WE_UCHAR *pucProvince, WE_UINT16 usProvinceLen, WE_UCHAR *pucCity, 
            WE_UINT16 usCityLen, WE_UCHAR *pucCompany, WE_UINT16 usCompanyLen, 
            WE_UCHAR *pucDepartment, WE_UINT16 usDepartmentLen,WE_UCHAR *pucName, 
            WE_UINT16 usNameLen,WE_UCHAR *pucEMail, WE_UINT16 usEMailLen, 
            WE_UCHAR ucKeyUsage, WE_UCHAR ucSignType);

static WE_VOID Sec_DpHandleChangeWtlsCertAble(WE_HANDLE hSecHandle, 
                                    WE_INT32 iTargetID, WE_UINT32 uiCertId);

/*************************************************************************************************
external function
*************************************************************************************************/

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleInternal
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle message in queue
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_VOID Sec_DpHandleInternal(WE_HANDLE hSecHandle)
{
    WE_INT32 iResult = -1;
    if(!hSecHandle)
    {
        return ;
    }

    if( (SEC_STATES_START == SEC_STATES) && (!SEC_CUR_FUNCTION) )
    {
        SEC_CUR_FUNCTION = Sec_DpHandleQueueNextNodeGet(hSecHandle);  
    }
    if(!SEC_CUR_FUNCTION)
    {
        return;
    }
    hSecHandle = ((St_SecDpParameter*)SEC_CUR_FUNCTION)->hItype; /*added by bird 061116*/

    switch(((St_SecDpParameter*)SEC_CUR_FUNCTION)->eType)                     
    {
        case E_SEC_DP_NONE:
        {    
            break;
        }
            
        case E_SEC_DP_GET_CIPHERSUITE:    
        {
            St_SecDpGetWtlsCipherSuite*             pstData = NULL;
            
            pstData = (St_SecDpGetWtlsCipherSuite*)SEC_CUR_FUNCTION;
            Sec_GetCipherSuiteIm (hSecHandle, pstData->pstStrct->iTargetID);
            break;
        }    
            
        case E_SEC_DP_XCH_WTLS_KEY:    
        {
            St_SecDpWtlsKeyXch*                pstData = NULL;

            pstData = (St_SecDpWtlsKeyXch *)SEC_CUR_FUNCTION;
            Sec_KeyExchangeIm (hSecHandle, 
                                    pstData->pstStrct->iTargetID, 
                                    pstData->pstStrct->stParam, 
                                    pstData->pstStrct->ucHashAlg, 
                                    pstData->pstStrct->pucRandval);
            break;
        }    
            
        case E_SEC_DP_WTLS_PRF:   
        {
            St_SecDpWtlsPrf*                    pstData = NULL;
            
            pstData = (St_SecDpWtlsPrf *)SEC_CUR_FUNCTION;
            Sec_GetPrfResultIm (hSecHandle, 
                            pstData->pstStrct->iTargetID, 
                            pstData->pstStrct->ucAlg, 
                            (WE_INT32)pstData->pstStrct->ucMasterSecretId,
                            pstData->pstStrct->pucSecret, 
                            (WE_INT32)pstData->pstStrct->usSecretLen,
                            pstData->pstStrct->pcLabel, 
                            pstData->pstStrct->pucSeed, 
                            (WE_INT32)pstData->pstStrct->usSeedLen, 
                            (WE_INT32)pstData->pstStrct->usOutputLen);
            break;
        }    
            
        case E_SEC_DP_VERIFY_WTLS_CERT_CHAIN: 
        {
            St_SecDpVerifyWtlsCertChain*        pstData = NULL;

            pstData =  \
                    (St_SecDpVerifyWtlsCertChain *)SEC_CUR_FUNCTION;
            Sec_VerifySvrCertIm (hSecHandle,
                                pstData->pstStrct->iTargetID,
                                pstData->pstStrct->pucBuf,
                                (WE_INT32)pstData->pstStrct->usBufLen,
                                pstData->pstStrct->pucAddr, 
                                (WE_INT32)pstData->pstStrct->usAddrLen,
                                (const WE_CHAR*)pstData->pstStrct->pucDomain,
                                (WE_INT32)pstData->pstStrct->usDomainLen);
            break;
        }    
#ifdef M_SEC_CFG_WTLS_CLASS_3       
        case E_SEC_DP_GET_WTLS_CLIENT_CERT:
        {
            St_SecDpGetWtlsClientCert*          pstData = NULL;

            pstData = \
                (St_SecDpGetWtlsClientCert *)SEC_CUR_FUNCTION;
            Sec_GetUsrCertIm (hSecHandle,  
                                      pstData->pstStrct->iTargetID,  
                                      pstData->pstStrct->pucBuf,  
                                      (WE_INT32)pstData->pstStrct->usBufLen);
            break;
        }    
            
        case E_SEC_DP_COMPUTE_WTLS_SIGN:   
        {
            St_SecDpComputeWtlsSign*             pstData = NULL;

            pstData = (St_SecDpComputeWtlsSign *)SEC_CUR_FUNCTION;
            Sec_CompSignatureIm (hSecHandle,  
                                         pstData->pstStrct->iTargetID, 
                                         pstData->pstStrct->pucKeyId,  
                                         (WE_INT32)pstData->pstStrct->usKeyIdLen, 
                                         pstData->pstStrct->pucBuf,  
                                         (WE_INT32)pstData->pstStrct->usBufLen);
            break;
        }    
#endif  
        case E_SEC_DP_ESTABLISHED_CONN:   
        {
            St_SecDpConnEst*            pstData = NULL;

            pstData = (St_SecDpConnEst *)SEC_CUR_FUNCTION;
            Sec_SetupConIm (hSecHandle, pstData->pstStrct);
            break;
        }
        
        case E_SEC_DP_TERMINATED_CONN: 
        {
            St_SecDpConnTmt*             pstData = NULL;

            pstData = (St_SecDpConnTmt *)SEC_CUR_FUNCTION;
            Sec_StopCon (hSecHandle,  
                                         pstData->pstStrct->iTargetID,  
                                         pstData->pstStrct->iSecurityId, 
                                         pstData->pstStrct->ucConnectionType);    
            break;
        }    
            
        case E_SEC_DP_RM_PEER_LINKS:  
        {
            St_SecDpRmPeerLinks*            pstData = NULL;

            pstData = (St_SecDpRmPeerLinks *)SEC_CUR_FUNCTION;
            Sec_RemovePeer (hSecHandle, 
                                    pstData->pstStrct->iTargetID,  
                                    (WE_INT32)pstData->pstStrct->ucMasterSecretId);
            break;
        }    
            
        case E_SEC_DP_LINK_PEER_SESSION:    
        {
            St_SecDpPeerLinkSession*          pstData = NULL;

            pstData = \
                (St_SecDpPeerLinkSession *)SEC_CUR_FUNCTION;
            Sec_AttachPeerToSessionIm (hSecHandle,  
                                      pstData->pstStrct->ucConnectionType, 
                                      pstData->pstStrct->pucAddress,  
                                      (WE_INT32)pstData->pstStrct->usAddressLen, 
                                      pstData->pstStrct->usPortnum,  
                                      (WE_INT32)pstData->pstStrct->ucMasterSecretId);
            break;
        }    
            
        case E_SEC_DP_LOOKUP_PEER:  
        {
            St_SecDpLookupPeer*                 pstData = NULL;

            pstData = (St_SecDpLookupPeer *)SEC_CUR_FUNCTION;
            Sec_SearchPeerIm (hSecHandle, 
                               pstData->pstStrct->iTargetID,  
                               pstData->pstStrct->ucConnectionType, 
                               pstData->pstStrct->pucAddress, 
                               (WE_INT32)pstData->pstStrct->usAddressLen, 
                               pstData->pstStrct->usPortnum);
            break;
        }    
            
        case E_SEC_DP_ACTIVE_SESSION:    
        {
            St_SecDpActiveSession*              pstData = NULL;

            pstData = (St_SecDpActiveSession *)SEC_CUR_FUNCTION;
            Sec_EnableSessionIm (hSecHandle,  
                                  (WE_INT32)pstData->pstStrct->ucMasterSecretId,  
                                  pstData->pstStrct->ucIsActive);
            break;
        }    
            
        case E_SEC_DP_INVALID_SESSION: 
        {
            St_SecDpInvalidSession*          pstData = NULL;

            pstData = \
                (St_SecDpInvalidSession *)SEC_CUR_FUNCTION;
            Sec_DisableSessionIm (hSecHandle,  
                                      (WE_INT32)pstData->pstStrct->ucMasterSecretId);
            break;
        }    
            
        case E_SEC_DP_FETCH_SESSION:   
        {
            St_SecDpfetchSession*               pstData = NULL;
            
            pstData = (St_SecDpfetchSession *)SEC_CUR_FUNCTION;
            Sec_FetchSessionInfoIm (hSecHandle, 
                                 pstData->pstStrct->iTargetID,  
                                 (WE_INT32)pstData->pstStrct->ucMasterSecretId);
            break;
        }    
            
        case E_SEC_DP_UPDATE_SESSION:  
        {
            St_SecDpUpdateSession*              pstData = NULL;
            
            pstData = (St_SecDpUpdateSession *)SEC_CUR_FUNCTION;
            Sec_UpdateSessionIm (hSecHandle,  
                                  (WE_INT32)pstData->pstStrct->ucMasterSecretId,  
                                  pstData->pstStrct->ucSessionOptions, 
                                  pstData->pstStrct->pucSessionId,  
                                  pstData->pstStrct->ucSessionIdLen, 
                                  pstData->pstStrct->ucCipherSuite, 
                                  pstData->pstStrct->ucCompressionAlg, 
                                  pstData->pstStrct->pucPrivateKeyId, 
                                  pstData->pstStrct->uiCreationTime);
            break;
        }    
 #ifdef M_SEC_CFG_CAN_SIGN_TEXT           
        case E_SEC_DP_UI_SIGN_TEXT: 
        {
            St_SecDpSignText*                   pstData = NULL;

            pstData = (St_SecDpSignText *)SEC_CUR_FUNCTION;
            Sec_DpHandleTextSign ( hSecHandle, 
                                 pstData->pstStrct->iTargetID,  
                                 pstData->pstStrct->iSignId,  
                                 pstData->pstStrct->pcText, 
                                 pstData->pstStrct->pcData,  
                                 (WE_INT32)pstData->pstStrct->usDataLen, 
                                 (WE_INT32)pstData->pstStrct->ucKeyIdType,  
                                 pstData->pstStrct->pcKeyId,  
                                 (WE_INT32)pstData->pstStrct->usKeyIdLen,  
                                 (WE_INT32)pstData->pstStrct->ucOptions);
            break;
        }    
#endif
        case E_SEC_DP_STORE_CA_CERT: 
        {
            St_SecDpStoreCert*              pstData = NULL;

            pstData = (St_SecDpStoreCert *)SEC_CUR_FUNCTION;
            Sec_HandleStoreCaCert(hSecHandle, 
                               pstData->pstStrct->iTargetID,  
                               pstData->pstStrct->pucCert,  
                               pstData->pstStrct->usCertLen);
            break;
        }    
            
        case E_SEC_DP_STORE_USER_CERT: 
        {
            St_SecDpStoreCert*              pstData = NULL;

            pstData = (St_SecDpStoreCert *)SEC_CUR_FUNCTION;
            Sec_HandleStoreUsrCert(hSecHandle, 
                                 pstData->pstStrct->iTargetID, 
                                 pstData->pstStrct->pucCert,  
                                 pstData->pstStrct->usCertLen,  
                                 pstData->pstStrct->pucCertURL);
            break;
        }    
            
        case E_SEC_DP_HASH_VERIFICATION: 
        {
            St_SecDpCaInfo*          pstData = NULL;

            pstData = (St_SecDpCaInfo *)SEC_CUR_FUNCTION;
            Sec_HandleHashVerify(hSecHandle, 
                                    pstData->pstStrct->iTargetID,  
                                    pstData->pstStrct->pucTrustedCAInfoStruct, 
                                    pstData->pstStrct->usTrustedCAInfoStructLen);
            break;
        }    
            
        case E_SEC_DP_VRRIFY_SIGN:  
        {
            St_SecDpCaInfo*          pstData = NULL;

            pstData = (St_SecDpCaInfo *)SEC_CUR_FUNCTION;
            Sec_HandleSignVerify(hSecHandle, 
                                         pstData->pstStrct->iTargetID,  
                                         pstData->pstStrct->pucTrustedCAInfoStruct, 
                                         pstData->pstStrct->usTrustedCAInfoStructLen);
            break;
        }    
            
        case E_SEC_DP_VERIFY_CERT:
        {
            St_SecDpDeliveryCert*           pstData = NULL;

            pstData = (St_SecDpDeliveryCert *)SEC_CUR_FUNCTION;
            Sec_HandleCertDelivery(hSecHandle, 
                                pstData->pstStrct->iTargetID,  
                                pstData->pstStrct->pucCertResponse);
            break;
        }    
            
        case E_SEC_DP_MIME_RESP:
        {
            St_SecDpMimeResp*               pstData = NULL;

            pstData = (St_SecDpMimeResp *) SEC_CUR_FUNCTION;
            Sec_DpHandleMimeResp(hSecHandle, pstData->pstStrct->iTargetID,(WE_INT32)pstData->pstStrct->usResult);
            break;
        }    
            
        case E_SEC_DP_VIEW_CERT_NAMES: 
        {
            St_SecDpViewCertNames*          pstData = NULL;
            
            pstData = (St_SecDpViewCertNames *)SEC_CUR_FUNCTION;
            Sec_HandleGetCertNames(hSecHandle, 
                                pstData->pstStrct->iTargetID,  
                                (WE_INT32)pstData->pstStrct->ucCertOptions);
            break;
        }                
            
        case E_SEC_DP_GET_CERT: 
        {
            St_SecDpGetCert*                pstData = NULL;

            pstData = (St_SecDpGetCert *)SEC_CUR_FUNCTION;
            Sec_HandleGetCert(hSecHandle, 
                           pstData->pstStrct->iTargetID,  
                           (WE_INT32)pstData->pstStrct->ucCertId, 
                           pstData->iOptions);
            break;
        }    
                     
        case E_SEC_DP_RM_CERT: 
        {
            St_SecDpRmCert*             pstData = NULL;

            pstData = (St_SecDpRmCert *)SEC_CUR_FUNCTION;
            Sec_HandleDeleteCert(hSecHandle,
                              pstData->pstStrct->iTargetID, 
                              (WE_INT32)pstData->pstStrct->ucCertId);
            break;
        }    

        case E_SEC_DP_GEN_KEY_PAIR: 
        {
            St_SecDpGenKeyPair*        pstData = NULL;
            
            pstData = \
                (St_SecDpGenKeyPair *)SEC_CUR_FUNCTION;
            Sec_DpHandleKeyPairGenerate(hSecHandle, 
                                      pstData->pstStrct->iTargetID,  
                                      pstData->pstStrct->ucKeyType,  
                                      pstData->pstStrct->ucAlg);
            break;
        }    
            
        case E_SEC_DP_RM_KEYS_AND_CERTS: 
        {
            St_SecDpRmKeysAndUserCerts*    pstData = NULL;
            
            pstData =  
                    (St_SecDpRmKeysAndUserCerts *)SEC_CUR_FUNCTION;
            Sec_DpHandleKeysAndUserCertsRemove(hSecHandle, 
                                             pstData->pstStrct->iTargetID,  
                                             pstData->pstStrct->ucKeyType);
            break;
        }    
            
        case E_SEC_DP_GET_USER_PUB_KEY: 
        {
            St_SecDpGetUserPubKey*          pstData = NULL;

            pstData = (St_SecDpGetUserPubKey *)SEC_CUR_FUNCTION;
            Sec_DpHandleUserPubKeyGet(hSecHandle, 
                                    pstData->pstStrct->iTargetID,  
                                    pstData->pstStrct->ucKeyType);
            break;
        }    
            
        case E_SEC_DP_CHANGE_PIN: 
        {
            St_SecDpChangePin*              pstData = NULL;

            pstData = (St_SecDpChangePin *)SEC_CUR_FUNCTION;
            Sec_DpHandlePinChange(hSecHandle, 
                                pstData->pstStrct->iTargetID,  
                                pstData->pstStrct->ucKeyType);
            break;
        }    

        case E_SEC_DP_VIEW_CUR_CERT: 
        {
            St_SecDpViewCurCert*        pstData = NULL;

            pstData = \
                (St_SecDpViewCurCert *)SEC_CUR_FUNCTION;
            Sec_DpHandleCurrentCertGet(hSecHandle, 
                                     pstData->pstStrct->iTargetID, 
                                     pstData->pstStrct->iSecurityId, 
                                     pstData->iOptions);
            break;
        }    
            
        case E_SEC_DP_VIEW_SESSION:
        {
            St_SecDpViewSessionInfo*        pstData = NULL;

            pstData = \
                (St_SecDpViewSessionInfo *)SEC_CUR_FUNCTION;
            Sec_DpHandleSessionInfoView(hSecHandle, 
                                      pstData->pstStrct->iTargetID, 
                                      pstData->pstStrct->iSecurityId, 
                                      pstData->iOptions);
            break;
        }    
            
        case E_SEC_DP_GET_CUR_SEC_CLASS: 
        {
            St_SecDpGetCurSecClass*     pstData = NULL;

            pstData =  
                (St_SecDpGetCurSecClass *)SEC_CUR_FUNCTION;
            Sec_DpHandleCurrentSecClassGet(hSecHandle,  
                                         pstData->pstStrct->iTargetID, 
                                         pstData->pstStrct->iSecurityId);
            break;
        }    
            
        case E_SEC_DP_CLEANUP_SESSION: 
        {
            St_SecDpCleanUpSession*           pstData = NULL;

            pstData = (St_SecDpCleanUpSession *)SEC_CUR_FUNCTION;
            Sec_DpHandleSessionCleanUp(hSecHandle, pstData->pstStrct->iTargetID);
            break;
        }    
#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))            
        case E_SEC_DP_VIEW_CONTRACTS: 
        {
            St_SecDpViewContracts*          pstData = NULL;

            pstData = (St_SecDpViewContracts *)SEC_CUR_FUNCTION;
            Sec_DpHandleContractsView(hSecHandle, pstData->pstStrct->iTargetID);
            break;
        }     
            
        case E_SEC_DP_RM_CONTRACT:
        {
            St_SecDpRmContract*         pstData = NULL;
            
            pstData = (St_SecDpRmContract *)SEC_CUR_FUNCTION;
            Sec_DpHandleContractRemove(hSecHandle, 
                                     pstData->pstStrct->iTargetID, 
                                     (WE_INT32)pstData->pstStrct->ucContractId);
            break;
        }
#endif

        case E_SEC_DP_GET_REQ_USER_CERT:
        {
            St_SecDpUserCertReq*         pstData = NULL;
            
            pstData = (St_SecDpUserCertReq *)SEC_CUR_FUNCTION;
            Sec_DpHandleUserCertReq(hSecHandle, 
                pstData->pstStrct->iTargetID, pstData->pstStrct->ucCertUsage,
                pstData->pstStrct->pucCountry, pstData->pstStrct->usCountryLen, 
                pstData->pstStrct->pucProvince, pstData->pstStrct->usProvinceLen, 
                pstData->pstStrct->pucCity, pstData->pstStrct->usCityLen, 
                pstData->pstStrct->pucCompany, pstData->pstStrct->usCompanyLen, 
                pstData->pstStrct->pucDepartment, pstData->pstStrct->usDepartmentLen,                           
                pstData->pstStrct->pucName, pstData->pstStrct->usNameLen,
                pstData->pstStrct->pucEMail, pstData->pstStrct->usEMailLen,
                pstData->pstStrct->ucKeyUsage, pstData->pstStrct->ucSignType);
            break;
        }

        case E_SEC_DP_CHANGE_WTLS_CERT_ABLE:
        {
            St_SecDpChangeWtlsCertAble*         pstData = NULL;
            
            pstData = (St_SecDpChangeWtlsCertAble *)SEC_CUR_FUNCTION;
            Sec_DpHandleChangeWtlsCertAble(hSecHandle, 
                                     pstData->pstStrct->iTargetID, 
                                     pstData->pstStrct->uiCertId);
            break;
        }
        /*action function asynchronism*/
        case E_SEC_DP_SHOWCONTRACTLIST:
        {
            St_SecDpShowContractList* pstData = NULL;
            pstData = (St_SecDpShowContractList *)SEC_CUR_FUNCTION;
            iResult = Sec_UeShowContractsListAction(hSecHandle,pstData->pstStrct->iTargetID,
                                                    pstData->pstStrct->stShowContractList);
            Sec_DpHandleCleanUp(hSecHandle);
        }
        break;
        case E_SEC_DP_SHOWCONTRACTCONTENT:
        {
            St_SecDpShowContractContent* pstData = NULL;
            pstData = (St_SecDpShowContractContent *)SEC_CUR_FUNCTION;
            iResult = Sec_UeShowContractContentAction(hSecHandle,pstData->pstStrct->iTargetID,
                                                pstData->pstStrct->stShowContractCont);
            Sec_DpHandleCleanUp(hSecHandle);
        }        
        break;
        case E_SEC_DP_SHOWCERTTLIST:
        {
            St_SecDpShowCertList* pstData = NULL;
            pstData = (St_SecDpShowCertList *)SEC_CUR_FUNCTION;
            iResult = Sec_UeShowCerListAction(hSecHandle,pstData->pstStrct->iTargetID,
                                                       pstData->pstStrct->stShowCertList);
            Sec_DpHandleCleanUp(hSecHandle);
        }
        break;
        case E_SEC_DP_SHOWCERTCONTENT:
        {
            St_SecDpShowCertContent* pstData = NULL;
            pstData = (St_SecDpShowCertContent *)SEC_CUR_FUNCTION;
            iResult = Sec_UeShowCertContentAction(hSecHandle,pstData->pstStrct->iTargetID,
                                                    pstData->pstStrct->stShowCertCont);
            Sec_DpHandleCleanUp(hSecHandle);
        }
        
        break;
        default:
        {
            break;
        }     
    }
    iResult = iResult;
    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    init sec module
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DpHandleInit (WE_HANDLE hSecHandle)
{
    WE_INT32 iRes = 0;
    
    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    Sec_MsgInit (hSecHandle);
    Sec_MsgRegister (hSecHandle,M_SEC_MSG_DST_HANDLE_MAIN, (Fn_SecMessageFunc*)Sec_DpHandleMain);
    iRes = Sec_DpHandleInterVariableInit(hSecHandle);    
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_MsgTerminate(hSecHandle);                                
        return iRes; 
    }
    
    Sec_DpHandleSecInfoLog();
    
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleIsInitialised
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get sec init state
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DpHandleIsInitialised(WE_HANDLE hSecHandle)
{
    if(!hSecHandle)
    {
        return FALSE;
    }
    
    return SEC_INITIALISED;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleCleanUp
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    when sec terminate, cleanup
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID Sec_DpHandleCleanUp(WE_HANDLE hSecHandle)
{
    if(!hSecHandle)
    {
        return ;
    }
    
    if (Sec_DpHandleQueueIsEmpty(hSecHandle))
    {
        SEC_STATES = SEC_STATES_STOP;
        SEC_WANTS_TO_RUN = FALSE;
    }
    else
    {
        SEC_STATES = SEC_STATES_START;
        SEC_WANTS_TO_RUN = TRUE;
    }
    Sec_TlHandleDpStructFree(SEC_CUR_FUNCTION);
    SEC_CUR_FUNCTION = NULL;
    
    return;
} 

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleWimInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    init wim
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN/OUT]:sec global data
    WE_INT32 iTargetID[IN]:TARGET ID
    WE_INT32 iPuk[IN]:FLAG
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_RWI_STATES_INIT_WIM                                 0
#define SEC_RWI_STATES_GET_VIEW_AND_CERTS                       1
#define SEC_RWI_STATES_INIT_INFO                                2
#define SEC_RWI_STATES_VERIFY_PUK                               3

WE_INT32 Sec_DpHandleWimInit(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iPuk)
{   
    WE_INT32 iRv = M_SEC_ERR_OK;
    
    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    switch(SEC_STATES_RWI)
    {
        case SEC_RWI_STATES_INIT_WIM: 
        {
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimInit(hSecHandle, iTargetID);
            SEC_STATES_RWI = SEC_RWI_STATES_GET_VIEW_AND_CERTS;
            iRv = M_SEC_ERR_IN_PROGRESS;
            break;
        }    
            
        case SEC_RWI_STATES_GET_VIEW_AND_CERTS: 
        {
            St_SecTlInitWimResp*            pstWimInitResp = NULL;

            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_iWimEnd(hSecHandle); 
                SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM;
                iRv = M_SEC_ERR_INSUFFICIENT_MEMORY;
                break;
            }
            pstWimInitResp = (St_SecTlInitWimResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            iRv = pstWimInitResp->iResult;
            if(M_SEC_ERR_OK != iRv)
            {
                SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM; 
            }
            else
            {
                SEC_WANTS_TO_RUN = FALSE;
                SEC_WAITING_FOR_WIM_RESULT = TRUE;
                Sec_iWimReadView(hSecHandle);
                iRv = M_SEC_ERR_IN_PROGRESS;
                SEC_STATES_RWI = SEC_RWI_STATES_INIT_INFO;
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimInitResp);
            break;
        }
    
        case SEC_RWI_STATES_INIT_INFO:
        {
            St_SecTlReadViewsCertsResp*      pstWimReadViewCertsResp = NULL;

            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }

            if (!SEC_CUR_WIM_RESP)
            {
                Sec_iWimEnd(hSecHandle); 
                SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM; 
                iRv = M_SEC_ERR_INSUFFICIENT_MEMORY;
                break;
            }
            pstWimReadViewCertsResp =  \
                    (St_SecTlReadViewsCertsResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            switch (pstWimReadViewCertsResp->iResult)
            {
                case M_SEC_ERR_NOT_FOUND: 
                {
                    iRv = M_SEC_ERR_OK;
                    break;
                }
                
                case M_SEC_ERR_OK:
                {
                    iRv = Sec_WimStartInfo(hSecHandle, 
                                       pstWimReadViewCertsResp->pucData,  
                                       pstWimReadViewCertsResp->iLen);
                    if (M_SEC_ERR_OK != iRv)
                    {
                        iRv = Sec_WimStopInfo(hSecHandle);
                        iRv = M_SEC_ERR_OK;
                    }
                    break;
                }
                
                default : 
                {
                    iRv = pstWimReadViewCertsResp->iResult;
                    break;
                }
            }
            
            if ((M_SEC_ERR_OK == iRv) && (!iPuk))
            {
                /*deleted by Bird 061121,for do not verify user when wim open*/
                /*
                iRv = Sec_DpHandleUserVerifyInit(hSecHandle, iTargetID);
                SEC_STATES_RWI = SEC_RWI_STATES_VERIFY_PUK;
                */
            }
            else if ((M_SEC_ERR_OK == iRv) && (iPuk))
            {
                SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM;                  
            }
            else                              
            {
                Sec_iWimEnd(hSecHandle); 
                SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM;                
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimReadViewCertsResp);            
            break;
        }
        
        case SEC_RWI_STATES_VERIFY_PUK:
        {
            if (iPuk)
            {
                SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM;
                iRv = M_SEC_ERR_OK;
                break;
            }
            iRv = Sec_DpHandleUserVerifyInit(hSecHandle, iTargetID);

            switch(iRv)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                
                case M_SEC_ERR_MISSING_KEY:
                case M_SEC_ERR_OK:
                {
                    SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM;            
                    break;
                }
                
                default:
                {
                    iRv = Sec_WimStopInfo(hSecHandle);
                    Sec_iWimEnd(hSecHandle); 
                    SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM; 
                    break;
                }
            }
            break;
        }

        default:
        {
            SEC_STATES_RWI = SEC_RWI_STATES_INIT_WIM; 
            iRv = M_SEC_ERR_GENERAL_ERROR;
            break;
        }
    }

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleUserVerify
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle user verify
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    E_SecPinReason   ePinCause[IN]:PIN TYPE
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_RVU_STATES_GET_PIN_INFO                             0
#define SEC_RVU_STATES_HANDLE_PIN_INFO_RESP                     1
#define SEC_RVU_STATES_INIT_WIM                                 2
#define SEC_RVU_STATES_HANDLE_UE_RESP                           3
#define SEC_RVU_STATES_VERIFY_PIN_RESP                          4

WE_INT32 Sec_DpHandleUserVerify(WE_HANDLE hSecHandle, WE_INT32 iTargetID, E_SecPinReason ePinCause)
{
    WE_INT32    iRv         = M_SEC_ERR_OK;
    WE_UINT8    ucIndex     = 0;
    WE_UINT8    ucKeyType   = 0;

    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    switch(SEC_STATES_RVU)
    {
        case SEC_RVU_STATES_GET_PIN_INFO: 
        {
            switch(ePinCause)
            {
                case E_SEC_OPEN_WIM:
                case E_SEC_STORE_CERT:
                case E_SEC_DELETE_CERT:
                case E_SEC_CREATE_AUTH_KEY:
                {
                    ucKeyType = M_SEC_PRIVATE_AUTHKEY;
                    break;
                }
                
                case E_SEC_SIGN_TEXT:
                case E_SEC_CREATE_NONREP_KEY:
                {
                    ucKeyType = M_SEC_PRIVATE_NONREPKEY;
                    break;
                }
                
                default:
                {
                    ucKeyType = 0;
                    break;
                }
            }
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimViewPrivKeyPin(hSecHandle, iTargetID, ucKeyType);
            SEC_STATES_RVU = SEC_RVU_STATES_HANDLE_PIN_INFO_RESP;
            iRv = M_SEC_ERR_IN_PROGRESS;
            break;
        }

        case SEC_RVU_STATES_HANDLE_PIN_INFO_RESP: 
        {
            St_SecTlPinResp*             pstWimPinResp = NULL;

            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }

            if (!SEC_CUR_WIM_RESP)
            {
                SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
                iRv = M_SEC_ERR_INSUFFICIENT_MEMORY;
                break;
            }

            pstWimPinResp = (St_SecTlPinResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            iRv = pstWimPinResp->iResult;
            switch(iRv)
            {
                case M_SEC_ERR_WIM_NOT_INITIALISED:
                {
                    SEC_STATES_RVU = SEC_RVU_STATES_INIT_WIM;
                    iRv = M_SEC_ERR_IN_PROGRESS;
                    break;
                }
                
                case M_SEC_ERR_MISSING_KEY:
                {
                    switch(ePinCause)
                    {
                        case E_SEC_SIGN_TEXT:
                        {
    #ifdef M_SEC_CFG_SHOW_WARNINGS
                            Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_MISSING_NR_KEY_SIGN_TEXT);
    #endif
                            iRv = M_SEC_ERR_MISSING_KEY;
                            break;
                        }

                        case E_SEC_STORE_CERT:
                        case E_SEC_DELETE_CERT:
                        {
                            iRv = M_SEC_ERR_OK;
                            break;
                        }

                        case E_SEC_CREATE_AUTH_KEY:
                        case E_SEC_CREATE_NONREP_KEY:
                        {
                            iRv = M_SEC_ERR_MISSING_KEY;
                            break;
                        }
                        
                        default:
                        {
                            break;
                        }
                    }
                    SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
                    break;
                }
                
                case M_SEC_ERR_OK:
                {
 #ifdef  G_SEC_CFG_SHOW_PIN /*modified by Bird 061121*/
                    SEC_VERIFIED_ON_WIM_RVU = FALSE;       
                    SEC_WAITING_FOR_UE_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iUePin(hSecHandle,iTargetID, ePinCause);    
                    SEC_STATES_RVU = SEC_RVU_STATES_HANDLE_UE_RESP;
                    iRv = M_SEC_ERR_IN_PROGRESS;
#else
                    SEC_STATES_RVU = NULL;
                    iRv = M_SEC_ERR_OK;
#endif
                    break;
                }
                
                default:
                {
                    SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
                    break;
                }
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimPinResp);
            break;
        }

        case SEC_RVU_STATES_INIT_WIM:
        {
            iRv = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
            if(M_SEC_ERR_IN_PROGRESS == iRv)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }
            if( (M_SEC_ERR_OK != iRv ) && (M_SEC_ERR_MISSING_KEY != iRv) )
            {
                SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
               break;
            }
            switch(ePinCause)
            {
                case E_SEC_OPEN_WIM:
                case E_SEC_STORE_CERT:
                case E_SEC_DELETE_CERT:
                {
                    iRv = M_SEC_ERR_OK;
                    SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
                    break;
                }
                
                case E_SEC_CREATE_AUTH_KEY:
                {
    #ifdef M_SEC_CFG_SHOW_WARNINGS
                    if (M_SEC_ERR_MISSING_KEY == iRv)
                    {
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
                    }
    #endif
                    SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO;
                    break;
                }
                
                case E_SEC_SIGN_TEXT:
                case E_SEC_CREATE_NONREP_KEY:
                {
    #ifdef M_SEC_CFG_SHOW_WARNINGS
                    if (M_SEC_ERR_MISSING_KEY == iRv)
                    {
                        Sec_iUeWarning(hSecHandle,iTargetID,  M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
                    }
    #endif
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimViewPrivKeyPin(hSecHandle, iTargetID, 
                                (WE_UINT8)M_SEC_PRIVATE_NONREPKEY);    
                    SEC_STATES_RVU = SEC_RVU_STATES_HANDLE_PIN_INFO_RESP;
                    iRv = M_SEC_ERR_IN_PROGRESS;
                    break;
                }
                
                default:
                {
                    break;
                }
            }
            break;
        }

        case SEC_RVU_STATES_HANDLE_UE_RESP: /*3*/
        {
            if(SEC_WAITING_FOR_UE_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }
            if (!SEC_CUR_UE_RESP)
            {
                SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
                iRv = M_SEC_ERR_INSUFFICIENT_MEMORY;
                break;
            }
            switch(((St_SecTlUeFncId *)SEC_CUR_UE_RESP)->eType)
            {
                case E_SEC_DP_VERIFY_PIN:
                {
                    St_SecTlVerifyPin*            pstUeVerifyPin = NULL;
                    
                    pstUeVerifyPin = (St_SecTlVerifyPin *)SEC_CUR_UE_RESP;
                    SEC_CUR_UE_RESP = NULL;
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimVerifyPrivKeyPin(hSecHandle, iTargetID,  
                                      pstUeVerifyPin->ucKeyType, pstUeVerifyPin->pcPin);
                    SEC_STATES_RVU = SEC_RVU_STATES_VERIFY_PIN_RESP;

                    ucIndex = 0; 
                    if(NULL != pstUeVerifyPin->pcPin)
                    {
                        while(0 != pstUeVerifyPin->pcPin[ucIndex])
                        {
                            pstUeVerifyPin->pcPin[ucIndex++] = 0;
                        }
                    }
                    iRv = M_SEC_ERR_IN_PROGRESS;
                    Sec_TlHandleUeStructFree(pstUeVerifyPin);
                    break;
                }
                
                case E_SEC_DP_PIN_DLG:
                {
                    St_SecTlPinDlgResult*      pstUePinResult = NULL;

                    pstUePinResult = (St_SecTlPinDlgResult *)SEC_CUR_UE_RESP;
                    SEC_CUR_UE_RESP = NULL;
                    switch(pstUePinResult->eResult)
                    {
                        case E_SEC_UE_OK:
                        {
                            if(SEC_VERIFIED_ON_WIM_RVU)
                            {
                                iRv = M_SEC_ERR_OK;
                            }
                            else
                            {
                                iRv = M_SEC_ERR_USER_NOT_VERIFIED;
                            }
                            break; 
                        } 
                        
                        case E_SEC_UE_CANCEL:
                        {
                            iRv = M_SEC_ERR_CANCEL;
                            break;    
                        }
                            
                        case E_SEC_UE_PIN_LOCKED:
                        {
                            iRv = M_SEC_ERR_PIN_LOCKED;
                            break;
                        }
                            
                        case E_SEC_UE_PIN_FALSE:
                        case E_SEC_UE_OTHER_ERROR:
                        {
                            iRv = M_SEC_ERR_USER_NOT_VERIFIED;
                            break;
                        }
                        
                        default:
                        {
                            iRv = M_SEC_ERR_USER_NOT_VERIFIED;
                            break;
                        }
                    }
                    SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
                    Sec_TlHandleUeStructFree(pstUePinResult);
                    break;
                }
                
                default:
                {
                    SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
                    iRv = M_SEC_ERR_GENERAL_ERROR;
                    break;
                }
            }

            break;
        }

        case SEC_RVU_STATES_VERIFY_PIN_RESP: 
        {         
            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                iRv = (WE_INT32)E_SEC_UE_OTHER_ERROR;
            }
            else
            {  
                St_SecTlPinResp*             pstWimPinResp = NULL;

                pstWimPinResp = (St_SecTlPinResp *)SEC_CUR_WIM_RESP;
                SEC_CUR_WIM_RESP = NULL;
                switch(pstWimPinResp->iResult)
                {
                    case M_SEC_ERR_OK:
                    {
                        SEC_VERIFIED_ON_WIM_RVU = TRUE;
                        iRv = (WE_INT32)E_SEC_UE_OK;
                        break;
                    }
                    case M_SEC_ERR_PIN_FALSE:
                    {
                        iRv = (WE_INT32)E_SEC_UE_PIN_FALSE;
                        break;
                    }
                    default:
                    {
                        iRv = (WE_INT32)E_SEC_UE_OTHER_ERROR;
                        break;
                    }
                }
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimPinResp);
            }
            SEC_WAITING_FOR_UE_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iUeVerifyPinResp(hSecHandle,iTargetID, (E_SecUeReturnVal)iRv);
            SEC_STATES_RVU = SEC_RVU_STATES_HANDLE_UE_RESP;
            iRv = M_SEC_ERR_IN_PROGRESS;
            break;
        }
        
        default:
        {
            SEC_STATES_RVU = SEC_RVU_STATES_GET_PIN_INFO; 
            iRv = M_SEC_ERR_GENERAL_ERROR;
            break;
        }
    }
    return iRv;
}
    
/*************************************************************************************************
internal function
*************************************************************************************************/

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleUserVerifyInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    init user verify init
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_RVUTI_STATES_GET_PIN_INFO                           0
#define SEC_RVUTI_STATES_HANDLE_PIN_INFO_RESP                   1
#define SEC_RVUTI_STATES_HANDLE_UE_RESP                         2
#define SEC_RVUTI_STATES_HANDLE_VERIFY_PIN_RESP                 3

static WE_INT32 Sec_DpHandleUserVerifyInit(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{  
    E_SecPinReason  ePinCause   = E_SEC_OPEN_WIM;
    WE_INT32        iIndex      = 0;
    WE_UINT8        ucKeyType   = M_SEC_PRIVATE_AUTHKEY;
    WE_INT32        iRv         = M_SEC_ERR_OK;
    

    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    switch(SEC_STATES_RVUTI)
    {
        case SEC_RVUTI_STATES_GET_PIN_INFO:
        {
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimViewPrivKeyPin(hSecHandle, iTargetID, ucKeyType);
            SEC_STATES_RVUTI = SEC_RVUTI_STATES_HANDLE_PIN_INFO_RESP;
            iRv = M_SEC_ERR_IN_PROGRESS;
            break;
        }
        
        case SEC_RVUTI_STATES_HANDLE_PIN_INFO_RESP: 
        {
            St_SecTlPinResp*             pstWimPinResp = NULL;

            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }

            if (!SEC_CUR_WIM_RESP)
            {
                SEC_STATES_RVUTI = SEC_RVUTI_STATES_GET_PIN_INFO;
                iRv = M_SEC_ERR_INSUFFICIENT_MEMORY;
                break;
            }

            pstWimPinResp = (St_SecTlPinResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            switch(iRv = pstWimPinResp->iResult)
            {
                case M_SEC_ERR_MISSING_KEY:
                {
                    SEC_STATES_RVUTI = SEC_RVUTI_STATES_GET_PIN_INFO; 
                    break;
                }
                
                case M_SEC_ERR_OK:
                {
                    SEC_VERIFIED_ON_WIM_RVUTI = FALSE;        
                    SEC_WAITING_FOR_UE_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iUePin(hSecHandle,iTargetID, ePinCause);
                    SEC_STATES_RVUTI = SEC_RVUTI_STATES_HANDLE_UE_RESP;
                    iRv = M_SEC_ERR_IN_PROGRESS;
                    break;
                }
                
                default:
                {
                    SEC_STATES_RVUTI = SEC_RVUTI_STATES_GET_PIN_INFO; 
                    break;
                }
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimPinResp);
            break;
        }
    
        case SEC_RVUTI_STATES_HANDLE_UE_RESP: 
        {
            if(SEC_WAITING_FOR_UE_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }

            if (!SEC_CUR_UE_RESP)
            {
                SEC_STATES_RVUTI = SEC_RVUTI_STATES_GET_PIN_INFO; 
                iRv = M_SEC_ERR_INSUFFICIENT_MEMORY;
                break;
            }
            switch(((St_SecTlUeFncId *)SEC_CUR_UE_RESP)->eType)
            {
                case E_SEC_DP_VERIFY_PIN:
                {
                    St_SecTlVerifyPin*            pstUeVerifyPin = NULL;
                    
                    pstUeVerifyPin = (St_SecTlVerifyPin *)SEC_CUR_UE_RESP;
                    SEC_CUR_UE_RESP = NULL;
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimVerifyPrivKeyPin(hSecHandle, iTargetID,  
                                      pstUeVerifyPin->ucKeyType, pstUeVerifyPin->pcPin);
                    SEC_STATES_RVUTI = SEC_RVUTI_STATES_HANDLE_VERIFY_PIN_RESP;
                    iIndex = 0;
                    if(NULL != pstUeVerifyPin->pcPin)
                    {
                        while(0 != pstUeVerifyPin->pcPin[iIndex])
                        {
                            pstUeVerifyPin->pcPin[iIndex++] = 0;
                        }
                    }
                    iRv = M_SEC_ERR_IN_PROGRESS;
                    Sec_TlHandleUeStructFree(pstUeVerifyPin);
                    break;
                }

                case E_SEC_DP_PIN_DLG:
                {
                    St_SecTlPinDlgResult*      pstUePinResult = NULL;

                    pstUePinResult = (St_SecTlPinDlgResult *)SEC_CUR_UE_RESP;
                    SEC_CUR_UE_RESP = NULL;
                    switch(pstUePinResult->eResult)
                    {
                        case E_SEC_UE_OK:
                        {
                            if(SEC_VERIFIED_ON_WIM_RVUTI)
                            {
                                iRv = M_SEC_ERR_OK;
                            }
                            else
                            {
                                iRv = M_SEC_ERR_USER_NOT_VERIFIED;
                            }
                            break;
                        }
                        
                        case E_SEC_UE_CANCEL:
                        {
                            iRv = M_SEC_ERR_CANCEL;
                            break;
                        }
                        
                        case E_SEC_UE_PIN_LOCKED:
                        {
                            iRv = M_SEC_ERR_PIN_LOCKED;
                            break;
                        }
                        
                        case E_SEC_UE_PIN_FALSE:
                        case E_SEC_UE_OTHER_ERROR:
                        {
                            iRv = M_SEC_ERR_USER_NOT_VERIFIED;
                            break;
                        }
                        
                        default:
                        {
                            iRv = M_SEC_ERR_USER_NOT_VERIFIED;
                            break;
                        }
                    }
                    SEC_STATES_RVUTI = SEC_RVUTI_STATES_GET_PIN_INFO;
                    Sec_TlHandleUeStructFree(pstUePinResult);
                    break;
                }
                
                default:
                {
                    SEC_STATES_RVUTI = SEC_RVUTI_STATES_GET_PIN_INFO;
                    iRv = M_SEC_ERR_GENERAL_ERROR;
                    break;
                }
            }

            break;
        }
        
        case SEC_RVUTI_STATES_HANDLE_VERIFY_PIN_RESP: 
        {
            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                iRv = M_SEC_ERR_IN_PROGRESS;
                break;
            }

            if (!SEC_CUR_WIM_RESP)
            {
                iRv = (WE_INT32)E_SEC_UE_OTHER_ERROR;
            }
            else
            {         
                St_SecTlPinResp*             pstWimPinResp = NULL;
                
                pstWimPinResp = (St_SecTlPinResp *)SEC_CUR_WIM_RESP;
                SEC_CUR_WIM_RESP = NULL;
                switch(pstWimPinResp->iResult)
                {
                    case M_SEC_ERR_OK:
                    {
                        SEC_VERIFIED_ON_WIM_RVUTI = TRUE;
                        iRv = (WE_INT32)E_SEC_UE_OK;
                        break;
                    }
                    case M_SEC_ERR_PIN_FALSE:
                    {
                        iRv = (WE_INT32)E_SEC_UE_PIN_FALSE;
                        break;
                    }
                    default:
                    {
                        iRv = (WE_INT32)E_SEC_UE_OTHER_ERROR;
                        break;
                    }
                }
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimPinResp);
            }
            SEC_WAITING_FOR_UE_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iUeVerifyPinResp(hSecHandle,iTargetID, (E_SecUeReturnVal)iRv);
            SEC_STATES_RVUTI = SEC_RVUTI_STATES_HANDLE_UE_RESP;
            iRv = M_SEC_ERR_IN_PROGRESS;
            break;
        }

        default:
        {
            SEC_STATES_RVUTI = SEC_RVUTI_STATES_GET_PIN_INFO; 
            iRv = M_SEC_ERR_GENERAL_ERROR;
            break;
        }
    }

    return iRv;
}
 
/*==================================================================================================
FUNCTION: 
    Sec_DpHandleQueueInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle dp queue init
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_DpHandleQueueInit(WE_HANDLE hSecHandle)
{
    if(!hSecHandle)
    {
        return ;
    }

    SEC_QUEUE_HEAD.eType = E_SEC_DP_NONE;
    SEC_QUEUE_HEAD.pstNext = &SEC_QUEUE_TAIL;
    SEC_QUEUE_HEAD.pstPrev = &SEC_QUEUE_HEAD;
    SEC_QUEUE_TAIL.eType = E_SEC_DP_NONE;
    SEC_QUEUE_TAIL.pstNext = NULL;
    SEC_QUEUE_TAIL.pstPrev = &SEC_QUEUE_HEAD;

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleQueueNodeRemove
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    remove a node from dp queue
ARGUMENTS PASSED:
    St_SecDpParameter*        pstNode[IN]:STRUCT OF DP QUEUE NODE
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_DpHandleQueueNodeRemove(St_SecDpParameter* pstNode)
{
    if (!pstNode)
    {
        return;
    }

    pstNode->pstPrev->pstNext = pstNode->pstNext;
    pstNode->pstNext->pstPrev = pstNode->pstPrev;
    pstNode->pstNext = NULL;
    pstNode->pstPrev = NULL;

    return;
}
/*==================================================================================================
FUNCTION: 
    Sec_DpHandleQueueDelById
CREATE DATE:
    2006-11-13
AUTHOR:
    brid
DESCRIPTION:
    delete node for queue and free it
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
    hItype [IN]: I interface type
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_VOID Sec_DpHandleQueueDelById(WE_HANDLE hSecHandle)
{
    St_SecDpParameter*        pstPara = NULL;
    St_SecDpParameter*        pstPrev = NULL;
    
    if(!hSecHandle)
    {
        return ;
    }

    pstPara = SEC_QUEUE_HEAD.pstNext;
    while (pstPara != &SEC_QUEUE_TAIL)
    {
        if(hSecHandle == pstPara->hItype)
        {
            pstPrev = pstPara->pstPrev;
            Sec_DpHandleQueueNodeRemove(pstPara);
            Sec_TlHandleDpStructFree((void*)pstPara);
            pstPara = pstPrev;

        }
        pstPara = pstPara->pstNext;
        
    }

    return;
}
/*==================================================================================================
FUNCTION: 
    Sec_DpHandleQueueCleanUp
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cleanup dp queue
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_DpHandleQueueCleanUp(WE_HANDLE hSecHandle)
{
    St_SecDpParameter*        pstPara = NULL;
    
    if(!hSecHandle)
    {
        return ;
    }

    while (!Sec_DpHandleQueueIsEmpty(hSecHandle))
    {
        pstPara = SEC_QUEUE_HEAD.pstNext;
        Sec_DpHandleQueueNodeRemove(pstPara);
        /*add by bird for free memory 061113*/
        Sec_TlHandleDpStructFree((void*)pstPara);
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleQueueNextNodeGet
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get a node from dp queue
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    St_SecDpParameter*
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static St_SecDpParameter* Sec_DpHandleQueueNextNodeGet(WE_HANDLE hSecHandle)
{
    St_SecDpParameter*        pstLastNode = NULL;

    if(!hSecHandle)
    {
        return NULL;
    }

    if (Sec_DpHandleQueueIsEmpty(hSecHandle))
    {
        return NULL;
    }

    pstLastNode = SEC_QUEUE_TAIL.pstPrev;
    Sec_DpHandleQueueNodeRemove(pstLastNode); 
    return pstLastNode;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleGlobalVarialbeInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    init global variable
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_DpHandleGlobalVarialbeInit(WE_HANDLE hSecHandle)
{
    if(!hSecHandle)
    {
        return;
    }

    SEC_CUR_UE_RESP = NULL;
    SEC_WAITING_FOR_UE_RESULT = FALSE;
    SEC_CUR_SEC_RESP = NULL;
    SEC_WAITING_FOR_SEC_RESULT = FALSE;
    SEC_CUR_WIM_RESP = NULL;
    SEC_WAITING_FOR_WIM_RESULT = FALSE;
    SEC_STATES = SEC_STATES_STOP;
    SEC_WANTS_TO_RUN = FALSE;
    SEC_FUNCID = E_SEC_DP_NONE;
    SEC_CUR_FUNCTION = NULL;
    SEC_INITIALISED = FALSE;

    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleGlobalVariableCleanUp
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cleanup global variable
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_DpHandleGlobalVariableCleanUp(WE_HANDLE hSecHandle)
{
    if(!hSecHandle)
    {
        return ;
    }

    SEC_INITIALISED = FALSE;

    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandlePinChange
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle pin change
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_UINT8        ucKeyType[IN]:KEY TYPE
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HPC_STATES_GET_PIN_INFO                             1
#define SEC_HPC_STATES_HANDLE_PIN_INFO_RESP                     2
#define SEC_HPC_STATES_INIT_WIM                                 3
#define SEC_HPC_STATES_HANDLE_CHANGE_PIN                        4
#define SEC_HPC_STATES_HANDLE_CHANGE_PIN_RESP                   5
static WE_VOID Sec_DpHandlePinChange(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                     WE_UINT8 ucKeyType)
{
    WE_INT32 iIndex     = 0;
    WE_INT32 iStatus    = 0;
    WE_INT32 iPinLen    = 0;

    if(!hSecHandle)
    {
        return;
    }
    
    switch(SEC_STATES)
    {
        case SEC_STATES_STOP :
        {
            break;
        }

        case SEC_HPC_STATES_GET_PIN_INFO:
        {
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimViewPrivKeyPin(hSecHandle, iTargetID, ucKeyType);
            SEC_STATES = SEC_HPC_STATES_HANDLE_PIN_INFO_RESP;
            break;
        }    

        case SEC_HPC_STATES_HANDLE_PIN_INFO_RESP:
        {
            St_SecTlPinResp*         pstWimPinResp = NULL;

            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }

            if (!SEC_CUR_WIM_RESP)
            {
                Sec_ModifyPinResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }

            pstWimPinResp = (St_SecTlPinResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            switch(pstWimPinResp->iResult)
            {
                case M_SEC_ERR_WIM_NOT_INITIALISED:
                {
                    SEC_STATES = SEC_HPC_STATES_INIT_WIM;
                    break;
                }
                    
                case M_SEC_ERR_MISSING_KEY:
                {
                    Sec_ModifyPinResp(hSecHandle, iTargetID, M_SEC_ERR_MISSING_KEY);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
                
                case M_SEC_ERR_OK:
                {
                    SEC_VERIFIED_ON_WIM_RCP = FALSE;
                    SEC_WAITING_FOR_UE_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iUeChangePin(hSecHandle,iTargetID,  ucKeyType);
                    SEC_STATES = SEC_HPC_STATES_HANDLE_CHANGE_PIN;
                    break;
                }
                
                default:
                {
                    Sec_ModifyPinResp(hSecHandle, pstWimPinResp->iTargetID, 
                                          pstWimPinResp->iResult);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimPinResp);
            break;
        }

        case SEC_HPC_STATES_INIT_WIM: 
        {
            iStatus = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
            if(M_SEC_ERR_IN_PROGRESS == iStatus)
            {
                break;
            }

            if( (M_SEC_ERR_OK != iStatus) && (M_SEC_ERR_MISSING_KEY != iStatus) )
            {
                Sec_ModifyPinResp(hSecHandle, iTargetID, M_SEC_ERR_USER_NOT_VERIFIED);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }

            switch(ucKeyType)
            {
                case M_SEC_PRIVATE_NONREPKEY:
                {
    #ifdef M_SEC_CFG_SHOW_WARNINGS
                    if(M_SEC_ERR_MISSING_KEY == iStatus)
                    {
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
                    }
    #endif
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimViewPrivKeyPin(hSecHandle, iTargetID, ucKeyType);
                    SEC_STATES = SEC_HPC_STATES_HANDLE_PIN_INFO_RESP;
                    break;
                }
                
                case M_SEC_PRIVATE_AUTHKEY:
                {
                    if(iStatus == M_SEC_ERR_MISSING_KEY)
                    {
    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
    #endif
                        Sec_ModifyPinResp(hSecHandle, iTargetID, M_SEC_ERR_MISSING_KEY);
                        Sec_DpHandleCleanUp(hSecHandle);
                    }
                    else
                    {
                        SEC_VERIFIED_ON_WIM_RCP = FALSE;
                        SEC_WAITING_FOR_UE_RESULT = TRUE;
                        SEC_WANTS_TO_RUN = FALSE;
                        Sec_iUeChangePin(hSecHandle,iTargetID, ucKeyType);
                        SEC_STATES = SEC_HPC_STATES_HANDLE_CHANGE_PIN;
                    }
                    break;
                }
                
                default:
                {
                    break;
                }
            }
            break;
        }

        case SEC_HPC_STATES_HANDLE_CHANGE_PIN:
        {
            if(SEC_WAITING_FOR_UE_RESULT)
            {
                break;
            }
            
            if (!SEC_CUR_UE_RESP)
            {
                Sec_ModifyPinResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }

            switch(((St_SecTlUeFncId *)SEC_CUR_UE_RESP)->eType)
            {
                case E_SEC_DP_CHANGE_PIN:
                {
                    St_SecTlChangePin*        pstUeChangePin = NULL;
                    
                    pstUeChangePin = (St_SecTlChangePin *)SEC_CUR_UE_RESP;
                    SEC_CUR_UE_RESP = NULL;
                    iPinLen = 0;
                    while (0 != pstUeChangePin->pcNewPin[iPinLen])
                    {
                        iPinLen++;
                    }
                    if ((iPinLen < M_SEC_PIN_MIN_SIZE) || (iPinLen > M_SEC_PIN_MAX_SIZE))
                    {
                        SEC_WAITING_FOR_UE_RESULT = TRUE;
                        SEC_WANTS_TO_RUN = FALSE;
                        if (iPinLen < M_SEC_PIN_MIN_SIZE)
                        {
                            Sec_iUeChangePinResp(hSecHandle, iTargetID, 
                                                    (E_SecUeReturnVal) E_SEC_UE_PIN_TOO_SHORT);
                        }
                        if (iPinLen > M_SEC_PIN_MAX_SIZE)
                        {
                            Sec_iUeChangePinResp(hSecHandle,iTargetID, 
                                                    (E_SecUeReturnVal) E_SEC_UE_PIN_TOO_LONG);
                        }
                        iIndex = 0;

                        while(0 != pstUeChangePin->pcOldPin[iIndex])
                        {
                            pstUeChangePin->pcOldPin[iIndex++] = 0;
                        }
                        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstUeChangePin);
                        break;
                    }
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimModifyPrivKeyPin(hSecHandle, iTargetID,  
                                      pstUeChangePin->ucKeyType,  
                                      pstUeChangePin->pcOldPin,  
                                      pstUeChangePin->pcNewPin);
                    SEC_STATES = SEC_HPC_STATES_HANDLE_CHANGE_PIN_RESP;
                    iIndex = 0;
                    while(0 != pstUeChangePin->pcOldPin[iIndex])
                    {
                        pstUeChangePin->pcOldPin[iIndex++] = 0;
                    }

                    iIndex = 0;
                    
                    while(0 != pstUeChangePin->pcNewPin[iIndex])
                    {
                        pstUeChangePin->pcNewPin[iIndex++] = 0;
                    }
                    Sec_TlHandleUeStructFree(pstUeChangePin);
                    break;
                }
                
                case E_SEC_DP_CHANGE_PIN_DLG:
                {
                    St_SecTlPinDlgResult*  pstUePinResult = NULL;

                    pstUePinResult = (St_SecTlPinDlgResult *)SEC_CUR_UE_RESP;
                    SEC_CUR_UE_RESP = NULL;
                    switch(pstUePinResult->eResult)
                    {
                        case E_SEC_UE_OK:
                        {
                            if(SEC_VERIFIED_ON_WIM_RCP)
                            {
                                iStatus = M_SEC_ERR_OK;
                            }
                            else
                            {
                                iStatus = M_SEC_ERR_USER_NOT_VERIFIED;
                            }
                            break;
                        }
                            
                        case E_SEC_UE_PIN_LOCKED:
                        {
                            iStatus = M_SEC_ERR_PIN_LOCKED;
                            break;
                        }
                        
                        case E_SEC_UE_CANCEL:
                        {
                            iStatus = M_SEC_ERR_CANCEL;
                            break;
                        }
                        
                        case E_SEC_UE_PIN_FALSE:
                        case E_SEC_UE_OTHER_ERROR:
                        {
                            iStatus = M_SEC_ERR_USER_NOT_VERIFIED;
                            break;
                        }
                        
                        default:
                        {
                            iStatus = M_SEC_ERR_USER_NOT_VERIFIED;
                            break;
                        }
                    }
                    Sec_ModifyPinResp(hSecHandle, iTargetID, iStatus);
                    Sec_DpHandleCleanUp(hSecHandle);
                    Sec_TlHandleUeStructFree(pstUePinResult);
                    break;
                }
                
                default:
                {
                    Sec_ModifyPinResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }

            break;
        }

        case SEC_HPC_STATES_HANDLE_CHANGE_PIN_RESP :
        {
            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            
            if (!SEC_CUR_WIM_RESP)
            {
                iStatus = (WE_INT32)E_SEC_UE_OTHER_ERROR;
            }
            else
            {
                St_SecTlPinResp*         pstWimPinResp = NULL;

                pstWimPinResp = (St_SecTlPinResp *)SEC_CUR_WIM_RESP;
                SEC_CUR_WIM_RESP = NULL;
                switch(pstWimPinResp->iResult)
                {
                    case M_SEC_ERR_OK:
                    {
                        SEC_VERIFIED_ON_WIM_RCP = TRUE;
                        iStatus = (WE_INT32)E_SEC_UE_OK;
                        break;
                    }
                    case M_SEC_ERR_PIN_FALSE:
                    {
                        iStatus = (WE_INT32)E_SEC_UE_PIN_FALSE;
                        break;
                    }
                    default:
                    {
                        iStatus = (WE_INT32)E_SEC_UE_OTHER_ERROR;
                        break;
                    }
                }
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimPinResp);
            }
            SEC_WAITING_FOR_UE_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iUeChangePinResp(hSecHandle,iTargetID, (E_SecUeReturnVal) iStatus);
            SEC_STATES = SEC_HPC_STATES_HANDLE_CHANGE_PIN;
            break;
        }

        default:
        {
            break;
        }
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleUserPubKeyGet
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get user public key
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_UINT8        ucKeyType[IN]:KEY TYPE
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HGK_STATES_GET_USER_CERT_NAME                       1
#define SEC_HGK_STATES_HANDLE_USER_CERT_NAME_RESP               2
#define SEC_HGK_STATES_INIT_WIM                                 3
#define SEC_HGK_STATES_HANDLE_CHOOSE_CERT_RESP                  4
#define SEC_HGK_HANDLE_GET_UCERT_SKEY_RESP                      5
static WE_VOID Sec_DpHandleUserPubKeyGet(WE_HANDLE hSecHandle, 
                                         WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
    WE_UCHAR*           pucOutPubKey    = NULL;    
    St_SecPubKeyRsa     stPubKey        = {0};
    WE_INT32            iResult         = M_SEC_ERR_OK;    
    WE_INT32            iOutPubKeyLen   = 0;

    if(!hSecHandle)
    {
        return;
    }
    switch(SEC_STATES)
    {
        case SEC_STATES_STOP: 
        {
            break;
        }
        
        case SEC_HGK_STATES_GET_USER_CERT_NAME: 
        {
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimViewNameOfUserCertByUsage(hSecHandle, ucKeyType);
            SEC_STATES = SEC_HGK_STATES_HANDLE_USER_CERT_NAME_RESP;
            break;
        }
        
        case SEC_HGK_STATES_HANDLE_USER_CERT_NAME_RESP:    
        {
            St_SecTlCertNamesResp*                   pstWimCertNamesResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT) 
            {
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimCertNamesResp =  \
                    (St_SecTlCertNamesResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_GET_CERT_NAMES != pstWimCertNamesResp->eType)
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,  
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimCertNamesResp);
                break;
            }
            
            if(M_SEC_ERR_WIM_NOT_INITIALISED == pstWimCertNamesResp->iResult)
            {
                SEC_STATES = SEC_HGK_STATES_INIT_WIM;
            }
            else if ((M_SEC_ERR_OK != pstWimCertNamesResp->iResult) ||  
                      (0 == pstWimCertNamesResp->iNbrOfCerts))
            {
#ifdef M_SEC_CFG_SHOW_WARNINGS
                if ((M_SEC_PRIVATE_NONREPKEY == ucKeyType) &&  \
                    (0 == pstWimCertNamesResp->iNbrOfCerts))
                {
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_NR_KEY_SIGN_TEXT);
                }
#endif
                Sec_GetPubKeyResp(hSecHandle, iTargetID,  
                                          pstWimCertNamesResp->iResult, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            else
            {
                SEC_WAITING_FOR_UE_RESULT = TRUE;
                SEC_WANTS_TO_RUN = FALSE;
                Sec_iUeSelectCert(hSecHandle, iTargetID, 
                                        M_SEC_ID_TITLE_CHOOSE_CERT_PUB_KEY,  
                                        M_SEC_ID_VIEW,  
                                        pstWimCertNamesResp->iNbrOfCerts, 
                                        pstWimCertNamesResp->pstCertNames);
                SEC_STATES = SEC_HGK_STATES_HANDLE_CHOOSE_CERT_RESP;
            }
            Sec_TlHandleWimStructFree(pstWimCertNamesResp);            
            break;
        }    
        
        case SEC_HGK_STATES_INIT_WIM: 
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                    
                case M_SEC_ERR_MISSING_KEY:
                {
          #ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
          #endif
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimViewNameOfUserCertByUsage(hSecHandle, ucKeyType);
                    SEC_STATES = SEC_HGK_STATES_HANDLE_USER_CERT_NAME_RESP;
                    break;
                }
                    
                case M_SEC_ERR_OK: 
                {                   
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimViewNameOfUserCertByUsage(hSecHandle, ucKeyType);
                    SEC_STATES = SEC_HGK_STATES_HANDLE_USER_CERT_NAME_RESP;
                    break;
                }
                    
                case M_SEC_ERR_USER_NOT_VERIFIED:
                default: 
                {                   
                    Sec_GetPubKeyResp(hSecHandle, iTargetID, iStatus, NULL, 0, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            } 
            break;
        }
               
        case SEC_HGK_STATES_HANDLE_CHOOSE_CERT_RESP:    
        {
            St_SecTlChooseCertResp* pstUeCCFNResp = NULL;

            if (SEC_WAITING_FOR_UE_RESULT) 
            {           
                break;
            }

            if (!SEC_CUR_UE_RESP)
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,  
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }

            pstUeCCFNResp =  \
                (St_SecTlChooseCertResp *)SEC_CUR_UE_RESP;
            SEC_CUR_UE_RESP = NULL;
            if(E_SEC_DP_CHOOSE_CERT != pstUeCCFNResp->eType)
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCCFNResp);

                break;
            }
            if (E_SEC_UE_CANCEL == pstUeCCFNResp->eResult)
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_OK,  
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCCFNResp);

                break;
            }
            else if ((E_SEC_UE_OK != pstUeCCFNResp->eResult) || (pstUeCCFNResp->iCertId < 1))
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,  
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCCFNResp);
                break;
            }
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimGetUserCertAndPrivKey(hSecHandle, pstUeCCFNResp->iCertId);
            Sec_TlHandleUeStructFree(pstUeCCFNResp);
            SEC_STATES = SEC_HGK_HANDLE_GET_UCERT_SKEY_RESP;
            break;
        } 
           
        case SEC_HGK_HANDLE_GET_UCERT_SKEY_RESP:
        {
            St_SecTlGetUcertSignedkeyResp*           pstGUSKResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT) 
            {
                break;
            }

            if (!SEC_CUR_WIM_RESP)
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,  
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            
            pstGUSKResp =  \
                (St_SecTlGetUcertSignedkeyResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_GET_UCERT_AND_SKEY != pstGUSKResp->eType)
            {
                Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,  
                                          NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstGUSKResp);

                break;
            }
            
            if ((NULL == pstGUSKResp->pucCert) || (0 == pstGUSKResp->usCertLen) ||  \
                (NULL == pstGUSKResp->pucSig) || (0 == pstGUSKResp->iSigLen))
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_GetPubKeyResp(hSecHandle, iTargetID,  
                                          M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, NULL, 0);
                break;
            }

            switch (pstGUSKResp->ucAlg)
            {
                case M_SEC_WTLS_RSA_SIGN_CERT:
                {
                    iResult = Sec_CertGetPubKeyRsa(hSecHandle, \
                                                  pstGUSKResp->pucCert, &stPubKey);
                    if (iResult != M_SEC_ERR_OK)
                    {
                        Sec_GetPubKeyResp(hSecHandle, iTargetID,  
                                                  M_SEC_ERR_INSUFFICIENT_MEMORY,  
                                                  NULL, 0, NULL, 0);
                        break;
                    }
                    iResult = Sec_DpHandleRsaPubKeyConvert(stPubKey,  
                                                         &pucOutPubKey, &iOutPubKeyLen);
                    if (M_SEC_ERR_OK != iResult)
                    {
                        Sec_GetPubKeyResp(hSecHandle, iTargetID, 
                                                  M_SEC_ERR_INSUFFICIENT_MEMORY,  
                                                  NULL, 0, NULL, 0);
                        break;
                    }
                    Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_OK,  
                                              pucOutPubKey, iOutPubKeyLen,  
                                              pstGUSKResp->pucSig, pstGUSKResp->iSigLen);
                    M_SEC_SELF_FREE(pucOutPubKey);
                    break;
                }

                default:
                {
                    Sec_GetPubKeyResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,  
                                              NULL, 0, NULL, 0);
                    break;
                }
            }
            Sec_TlHandleWimStructFree(pstGUSKResp);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
        } 
        
        default:
        {
            break;
        }
    }

    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleKeysAndUserCertsRemove
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    remove a user cert and an associated key
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_UINT8        ucKeyType[IN]:KEY TYPE
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HRKC_STATES_HANDLE_ALLOW_REMOVE_KEY                 1
#define SEC_HRKC_STATES_HANDLE_REMOVE_CERT                      2
#define SEC_HRKC_STATES_INIT_WIM                                3
#define SEC_HRKC_STATES_HANDLE_WIM_INIT_RESP                    4
#define SEC_HRKC_STATES_HANDLE_REMOVE_CERT_RESP                 5

static WE_VOID Sec_DpHandleKeysAndUserCertsRemove(WE_HANDLE hSecHandle,
                                                  WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
    WE_INT32 iRes = 0;
    
    if(!hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES)
    {
        case SEC_STATES_STOP: 
        {
            break;
        }
        
        case SEC_HRKC_STATES_HANDLE_ALLOW_REMOVE_KEY: 
        {
            SEC_WAITING_FOR_UE_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iUeDelUserKeys(hSecHandle,iTargetID, ucKeyType);
            SEC_STATES = SEC_HRKC_STATES_HANDLE_REMOVE_CERT;
            break;
        }
        
        case 2:
        {
            St_SecTlPinDlgResult*              pstUePin = NULL;

            if(SEC_WAITING_FOR_UE_RESULT)
            {
                break;
            }

            if(!SEC_CUR_UE_RESP)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, 
                                                   M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }

            pstUePin = (St_SecTlPinDlgResult *)SEC_CUR_UE_RESP;
            SEC_CUR_UE_RESP = NULL;
            if(E_SEC_DP_RM_KEYS_AND_CERTS != pstUePin->eType)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUePin);
                break;
            }

            if(E_SEC_UE_PIN_LOCKED == pstUePin->eResult)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, M_SEC_ERR_PIN_LOCKED);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            else if(E_SEC_UE_OK != pstUePin->eResult)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, \
                                                   M_SEC_ERR_USER_NOT_VERIFIED);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            else
            {
                SEC_WAITING_FOR_WIM_RESULT = TRUE;
                SEC_WANTS_TO_RUN = FALSE;
                Sec_iWimDelPrivKeysAndUCertsByUsage(hSecHandle, iTargetID, ucKeyType);
                SEC_STATES = SEC_HRKC_STATES_INIT_WIM;
            }
            Sec_TlHandleUeStructFree((St_SecTlUeFncId *)pstUePin);
            break;
        }
        
        case SEC_HRKC_STATES_INIT_WIM:
        {
            St_SecTlRmKeysAndUcertsResp*     pstWimDKUBTResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT) 
            {
                break;
            }
            
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, 
                                                   M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            
            pstWimDKUBTResp =  \
                (St_SecTlRmKeysAndUcertsResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(pstWimDKUBTResp->eType != E_SEC_DP_RM_KEYS_AND_CERTS)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimDKUBTResp);
                break;
            }
            if(M_SEC_ERR_WIM_NOT_INITIALISED == pstWimDKUBTResp->iResult)
            {
                SEC_WAITING_FOR_WIM_RESULT = TRUE;
                SEC_WANTS_TO_RUN = FALSE;
                Sec_iWimInit(hSecHandle, iTargetID);
                SEC_STATES = SEC_HRKC_STATES_HANDLE_WIM_INIT_RESP;
            }
            else
            {
                if ((ucKeyType == M_SEC_PRIVATE_AUTHKEY) &&  \
                    (M_SEC_ERR_OK == pstWimDKUBTResp->iResult))
                {
                    iRes = Sec_WimStopInfo(hSecHandle);
                    if (iRes != M_SEC_ERR_OK)
                    {
                        return ;
                    }
                }

                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, 
                                                   pstWimDKUBTResp->iResult);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimDKUBTResp);
            break;
        }
        
        case SEC_HRKC_STATES_HANDLE_WIM_INIT_RESP:
        {
            St_SecTlInitWimResp*                pstWimInitResp = NULL;

            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_iWimEnd(hSecHandle); 
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, 
                                                   M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimInitResp = (St_SecTlInitWimResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_INIT != pstWimInitResp->eType)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID,  
                                                   M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimInitResp);

                break;
            }
            
            if(M_SEC_ERR_OK != pstWimInitResp->iResult)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, 
                                                   pstWimInitResp->iResult);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            else
            {
                SEC_WAITING_FOR_WIM_RESULT = TRUE;
                SEC_WANTS_TO_RUN = FALSE;
                Sec_iWimDelPrivKeysAndUCertsByUsage(hSecHandle, iTargetID, ucKeyType);
                SEC_STATES = SEC_HRKC_STATES_HANDLE_REMOVE_CERT_RESP;
            }
            Sec_TlHandleUeStructFree((St_SecTlUeFncId *)pstWimInitResp);
            break;
        }
        
        case SEC_HRKC_STATES_HANDLE_REMOVE_CERT_RESP:
        {
            St_SecTlRmKeysAndUcertsResp*     pstWimDKUBTResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT) 
            {
                break;
            }
            Sec_iWimEnd(hSecHandle); 
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID,  
                                                   M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimDKUBTResp =  
                (St_SecTlRmKeysAndUcertsResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_RM_KEYS_AND_CERTS != pstWimDKUBTResp->eType)
            {
                Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimDKUBTResp);

                break;
            }
            
            Sec_DelKeysUserCertsByTypeResponse(hSecHandle, iTargetID, pstWimDKUBTResp->iResult);
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)pstWimDKUBTResp);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
        }
        
        default:
        {
            break;
        }
    }

    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleKeyPairGenerate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    geneate a key pair
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_UINT8        ucKeyType[IN]: KEY TYPE
    WE_UINT8        ucAlg[IN]:ALG TYPE
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HPG_STATES_VERIFY_USER                              1
#define SEC_HPG_STATES_HANDLE_GEN_PIN_RESP                      2
#define SEC_HPG_STATES_GEN_KEYPAIR_RESP                         3

static WE_VOID Sec_DpHandleKeyPairGenerate(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                           WE_UINT8 ucKeyType, WE_UINT8 ucAlg)
{  
    E_SecPinReason   ePinCause   = E_SEC_OPEN_WIM;
    WE_INT32        iIndex      = 0;
    WE_INT32        iPinLen     = 0;
    WE_INT32        iStatus     = 0;    
    
    if(!hSecHandle)
    {
        return ;
    }

    switch(SEC_STATES)
    {
        case SEC_STATES_STOP:
        {
            break;
        }
        
        case SEC_HPG_STATES_VERIFY_USER:
        {
            if(M_SEC_PRIVATE_NONREPKEY == ucKeyType)
            {
                ePinCause = E_SEC_CREATE_NONREP_KEY;
            }
            else
            {
                ePinCause = E_SEC_CREATE_AUTH_KEY;
            }

            iStatus = Sec_DpHandleUserVerify(hSecHandle, iTargetID, ePinCause);
            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                
                case M_SEC_ERR_MISSING_KEY:
                {
 #ifdef  G_SEC_CFG_SHOW_PIN /*modified by Bird 061121*/
                    SEC_WAITING_FOR_UE_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iUeCreatePin(hSecHandle,iTargetID, ucKeyType);
                    SEC_STATES = SEC_HPG_STATES_HANDLE_GEN_PIN_RESP;
                    break;
 
 #endif
                }      /* fall through */
                //lint -fallthrough
                
                case M_SEC_ERR_OK:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimKeyPairGen(hSecHandle, iTargetID, ucKeyType, NULL, ucAlg);
                    SEC_STATES = SEC_HPG_STATES_GEN_KEYPAIR_RESP;
                    break;
                } 
                
                case M_SEC_ERR_USER_NOT_VERIFIED:
                    //lint -fallthrough
                default:
                {
                    Sec_KeyPairGenResp(hSecHandle, iTargetID, iStatus, 
                                                ucKeyType, NULL, 0, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        
        case SEC_HPG_STATES_HANDLE_GEN_PIN_RESP:
        {
            St_SecTlGenPinResp*      pstUeGenPinResp = NULL;

            if(SEC_WAITING_FOR_UE_RESULT)
            {
                break;
            }
            if(!SEC_CUR_UE_RESP)
            {
                Sec_KeyPairGenResp(hSecHandle, iTargetID,M_SEC_ERR_INSUFFICIENT_MEMORY,  
                                            ucKeyType, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstUeGenPinResp =  \
                (St_SecTlGenPinResp *)SEC_CUR_UE_RESP;
            SEC_CUR_UE_RESP = NULL;
            if ((E_SEC_DP_GEN_PIN != pstUeGenPinResp->eType) || (NULL == pstUeGenPinResp->pcPin))
            {
                Sec_KeyPairGenResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,  
                                            ucKeyType, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeGenPinResp);

                break;
            }

            if(E_SEC_UE_OK != pstUeGenPinResp->eResult)
            {
                Sec_KeyPairGenResp(hSecHandle, iTargetID, M_SEC_ERR_OK, ucKeyType,  
                                            NULL, 0, NULL, 0);
                Sec_TlHandleUeStructFree((St_SecTlUeFncId *)pstUeGenPinResp);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            iPinLen = 0;
            while (0 != pstUeGenPinResp->pcPin[iPinLen])
            {
                iPinLen++;
            }
            if ((iPinLen < M_SEC_PIN_MIN_SIZE) || (iPinLen > M_SEC_PIN_MAX_SIZE))
            {
                if (iPinLen < M_SEC_PIN_MIN_SIZE)
                {
                    Sec_KeyPairGenResp(hSecHandle, iTargetID, 
                                                M_SEC_ERR_PIN_TOO_SHORT,  
                                                ucKeyType, NULL, 0, NULL, 0);
                }
                if  (iPinLen > M_SEC_PIN_MAX_SIZE)
                {
                    Sec_KeyPairGenResp(hSecHandle, iTargetID, 
                                                M_SEC_ERR_PIN_TOO_LONG,  
                                                ucKeyType, NULL, 0, NULL, 0);
                }
                if(NULL != pstUeGenPinResp->pcPin)
                {
                    iIndex=0;
                    while(0 != pstUeGenPinResp->pcPin[iIndex])
                    {
                        pstUeGenPinResp->pcPin[iIndex] = 0;
                        iIndex++;
                    }
                }
                Sec_TlHandleUeStructFree((St_SecTlUeFncId *)pstUeGenPinResp);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimKeyPairGen(hSecHandle,iTargetID, ucKeyType,  
                                    pstUeGenPinResp->pcPin, ucAlg);
            SEC_STATES = SEC_HPG_STATES_GEN_KEYPAIR_RESP;

            if(NULL != pstUeGenPinResp->pcPin)
            {
                iIndex=0;
                while(0 != pstUeGenPinResp->pcPin[iIndex])
                {
                    pstUeGenPinResp->pcPin[iIndex] = 0;
                    iIndex++;
                }
            }
            Sec_TlHandleUeStructFree((St_SecTlUeFncId *)pstUeGenPinResp);
            break;
        }
        
        case SEC_HPG_STATES_GEN_KEYPAIR_RESP:
        {
            St_SecTlGenRsaKeypairResp*       pstWimGRKPResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_KeyPairGenResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,  
                                            ucKeyType, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimGRKPResp =  
                (St_SecTlGenRsaKeypairResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_GEN_KEY_PAIR != pstWimGRKPResp->eType)
            {
                Sec_KeyPairGenResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,  
                                            ucKeyType, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimGRKPResp);

                break;
            }
            
            if (M_SEC_ERR_REACHED_USER_CERT_LIMIT == pstWimGRKPResp->iResult)
            {
                Sec_KeyPairGenResp(hSecHandle, iTargetID,  
                                            M_SEC_ERR_REACHED_USER_CERT_LIMIT,  
                                            ucKeyType, NULL, 0, NULL, 0);
                Sec_TlHandleWimStructFree(pstWimGRKPResp);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            else if (M_SEC_ERR_OK != pstWimGRKPResp->iResult)
            {
                Sec_KeyPairGenResp(hSecHandle, iTargetID,  
                                            pstWimGRKPResp->iResult,  
                                            ucKeyType, NULL, 0, NULL, 0);
                Sec_TlHandleWimStructFree(pstWimGRKPResp);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            else
            {
                Sec_KeyPairGenResp(hSecHandle, pstWimGRKPResp->iTargetID, 
                                            pstWimGRKPResp->iResult,  
                                            pstWimGRKPResp->ucKeyType,  
                                            pstWimGRKPResp->pucPublicKey, 
                                            pstWimGRKPResp->iPublicKeyLen, 
                                            pstWimGRKPResp->pucSig, pstWimGRKPResp->iSigLen);
            }
            Sec_TlHandleWimStructFree(pstWimGRKPResp);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
        }

        default:
        {
            break;
        }
    }

    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleTerminate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    terminate sec module
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DpHandleTerminate (WE_HANDLE hSecHandle)
{  
    WE_INT32 iRes = 0;
    
    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    Sec_MsgTerminate(hSecHandle);  
    iRes = Sec_DpHandleInterVariableCleanUp(hSecHandle);
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleInterVariableInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    init sec inter variable
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 Sec_DpHandleInterVariableInit (WE_HANDLE hSecHandle)
{
    WE_INT32 iRes = 0;
    WE_INT32 iRet = 0;
    
    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRes = Sec_LibCryptInitialise(hSecHandle);    
    if (iRes != M_SEC_ERR_OK)
    {
        return iRes;
    }
    Sec_DpHandleGlobalVarialbeInit(hSecHandle);
    Sec_CertGlobalVarInit(hSecHandle);
    iRes = Sec_CertStateFormInit(hSecHandle);
    if (iRes != M_SEC_ERR_OK)
    {
        return iRes;
    }
    iRes = Sec_X509ChainInit(hSecHandle);
    if (iRes != M_SEC_ERR_OK)
    {
        iRet = Sec_CertStateFormTerminate(hSecHandle);
        if(iRet)
        {
        }
        return iRes;
    }
    Sec_DpHandleQueueInit(hSecHandle);
    iRes = Sec_WimStartInfo(hSecHandle, NULL, 0);
    if (iRes != M_SEC_ERR_OK)
    {
        iRet = Sec_CertStateFormTerminate(hSecHandle);
        if(iRet)
        {
        }
        return iRes;
    }
    Sec_iUeStart(hSecHandle);
    SEC_INITIALISED = TRUE;
    
    return M_SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    Sec_DpHandleInterVariableCleanUp
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cleanup sec inter variable
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 Sec_DpHandleInterVariableCleanUp (WE_HANDLE hSecHandle)
{
    WE_UCHAR*   pucData     = NULL;
    WE_INT32    iLen        = 0;
    WE_INT32    iRes        = M_SEC_ERR_OK;

    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRes += Sec_CertStateFormTerminate(hSecHandle);
    iRes += Sec_X509ChainTerminate(hSecHandle);
    /*move from sec_iwim.c by bird 060808*/
    if (Sec_WimIsInited(hSecHandle))
    {
        /* only clear the M_SEC_USER_CERT_KEYPAIRS array. */
        Sec_WimClearKeyPairInfo(hSecHandle);
        Sec_WimSessionStop (hSecHandle);

    }

    iRes += Sec_WimGetDataInfo(hSecHandle, &pucData, &iLen);
    Sec_iWimStoreView(hSecHandle, pucData, iLen);
    
    M_SEC_SELF_FREE(pucData);
    iRes += Sec_WimStopInfo(hSecHandle);

    SEC_E_INITIALISED = FALSE;

    /*modified by bird 061113 for change free order,can memory leak possibly*/
    Sec_WimRemoveElement(hSecHandle);
    Sec_DpHandleCleanUp(hSecHandle);
    Sec_DpHandleQueueCleanUp(hSecHandle);
    /*
    Sec_DpHandleCleanUp(hSecHandle);
    */
    SEC_STATES = SEC_STATES_STOP;
    SEC_WANTS_TO_RUN = FALSE;
    Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    Sec_DpHandleGlobalVariableCleanUp(hSecHandle);
    SEC_INITIALISED = FALSE;
    
    if (iRes != M_SEC_ERR_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleSecInfoLog
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    log sec info
ARGUMENTS PASSED:
    NONE
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_DpHandleSecInfoLog (WE_VOID)
{
    WE_LOG_MSG((0, (WE_UINT8)0,"Sec_DpHandleSecInfoLog() n"));
#ifdef M_SEC_CFG_WTLS_CLASS_2
    WE_LOG_MSG((0, (WE_UINT8)0,"WTLS class 2 is used. n"));
#endif
#ifdef M_SEC_CFG_WTLS_CLASS_3
    WE_LOG_MSG((0, (WE_UINT8)0,"WTLS class 3 is used. n"));
#endif
    WE_LOG_MSG((0, (WE_UINT8)0,"RSA is used with keysize = %d. n", 1024));

    WE_LOG_MSG((0, (WE_UINT8)0,"RC5 is used. n"));

    WE_LOG_MSG((0, (WE_UINT8)0,"3DES is used. n"));

    WE_LOG_MSG((0, (WE_UINT8)0,"Can signText. n"));

    WE_LOG_MSG((0, (WE_UINT8)0,"Contracts will be stored. n"));

    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleRsaPubKeyConvert
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    convert a rsa public key struct to a der encoded bit string
ARGUMENTS PASSED:
    St_SecPubKeyRsa         stKey[IN]:RSA PUBLIC KEY STRUCT
    WE_UCHAR**              ppucPubKey[OUT]: DER ENCODED RSA PUBLIC KEY STRING
    WE_INT32*               piOutPubKeyLen[OUT]:DER ENCODED STRING LENGTH
RETURN VALUE:
    Counter number
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 Sec_DpHandleRsaPubKeyConvert(St_SecPubKeyRsa stKey,
                                             WE_UCHAR** ppucPubKey, 
                                             WE_INT32* piOutPubKeyLen)
{
    WE_UINT16   usMsgLen            = 0;
    WE_UINT16   usHdrLen            = 0;
    WE_UINT16   usMLen              = 0;
    WE_UINT16   usELen              = 0;
    WE_UINT8    ucPrependModZero    = 0;
    WE_UINT8    ucPrependExpZero    = 0;
    WE_UINT16   usM                 = stKey.usModLen ;
    WE_UINT16   usE                 = stKey.usExpLen;
    WE_UINT8*   puc                 = NULL;
    WE_INT32    iIndex              = 0;
    WE_UINT8    ucBigEndian         = 0;
    WE_UINT16   usU                 = 0x0102;

    if(!ppucPubKey || !piOutPubKeyLen || !stKey.pucExponent || !stKey.pucModulus || \
        (0 == stKey.usExpLen) || (0 == stKey.usModLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    if (0x01 == *((WE_UINT8*)&(usU)))
    { 
        ucBigEndian = 1;
    }
    else
    {
        ucBigEndian = 0;
    }
    if (((WE_UINT8)*(stKey.pucModulus)) > 127)
    {
        ucPrependModZero = 1;
        usM ++;
    }
    usMLen = 2;
    if (usM > 127)
    {
        usMLen++;
    }
    if (usM > 255)
    {
        usMLen++;
    }

    if (((WE_UINT8)*(stKey.pucExponent)) > 127)
    {
        ucPrependExpZero = 1;
        usE ++;
    }
    usELen = 2;
    if (usE > 127)
    {
        usELen ++;
    }
    if (usE > 255)
    {
        usELen ++;
    }
    usMsgLen = (WE_UINT16)(usMLen + usM + usELen + usE);
    usHdrLen = 2;
    if (usMsgLen > 127)
    {
        usHdrLen ++;
    }
    if (usMsgLen > 255)
    {
        usHdrLen ++;
    }

    *ppucPubKey = (WE_UCHAR *)WE_MALLOC((WE_ULONG)((usHdrLen + usMsgLen)*sizeof(WE_UCHAR))); 
    if (!(*ppucPubKey))
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    puc = (*ppucPubKey);
    *puc++ = (WE_UINT8)0X30; 
    switch (usHdrLen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *puc++ = *(((WE_UCHAR*)&(usMsgLen))+1); 
            }
            else
            {
                *puc++ = *((WE_UCHAR*)&(usMsgLen)); 
            }
            break;
        }
        
        case 3: 
        {
            *puc++ = (WE_UCHAR)0X81; 
            if (ucBigEndian)
            {
                *puc++ = *(((WE_UCHAR*)&(usMsgLen))+1); 
            }
            else
            {
                *puc++ = *((WE_UCHAR*)&(usMsgLen)); 
            }
            break;
        }
        
        case 4:
        {
            *puc++ = (WE_UCHAR)0X82; 
            if (ucBigEndian)
            {
                *puc++ = *((WE_UCHAR*)&(usMsgLen));
                *puc++ = *(((WE_UCHAR*)&(usMsgLen))+1); 
            }
            else
            {
                *puc++ = *(((WE_UCHAR*)&(usMsgLen))+1); 
                *puc++ = *((WE_UCHAR*)&(usMsgLen));
            }
            break;
        }
        
        default:
        {
            break;
        }
    } 

    *puc++ = (WE_UINT8)0X02; 
    switch (usMLen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *puc++ = *(((WE_UCHAR*)&(usM))+1); 
            }
            else
            {
                *puc++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }
        
        case 3:
        {
            *puc++ = (WE_UCHAR)0X81; 
            if (ucBigEndian)
            {
                *puc++ = *(((WE_UCHAR*)&(usM))+1); 
            }
            else
            {
                *puc++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }
        
        case 4:
        {
            *puc++ = (WE_UCHAR)0X82; 
            if (ucBigEndian)
            {
                *puc++ = *((WE_UCHAR*)&(usM));
                *puc++ = *(((WE_UCHAR*)&(usM))+1); 
            }
            else
            {
                *puc++ = *(((WE_UCHAR*)&(usM))+1); 
                *puc++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }

        default:
        {
            break;
        }
    }
    if (ucPrependModZero)
    {
        *puc++ = (WE_UINT8)0X00;
    }
    for (iIndex = 0; iIndex < stKey.usModLen; iIndex ++) 
    {
        *puc++ = stKey.pucModulus[iIndex];
    }

    *puc++ = (WE_UINT8)0X02; 
    switch (usELen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *puc++ = *(((WE_UCHAR*)&(usE))+1); 
            }
            else
            {
                *puc++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        
        case 3:
        {
            *puc++ = (WE_UCHAR)0X81; 
            if (ucBigEndian)
            {
                *puc++ = *(((WE_UCHAR*)&(usE))+1); 
            }
            else
            {
                *puc++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        
        case 4:
        {
            *puc++ = (WE_UCHAR)0X82; 
            if (ucBigEndian)
            {
                *puc++ = *((WE_UCHAR*)&(usE));
                *puc++ = *(((WE_UCHAR*)&(usE))+1); 
            }
            else
            {
                *puc++ = *(((WE_UCHAR*)&(usE))+1); 
                *puc++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }

        default:
        {
            break;
        }
    }
    if (ucPrependExpZero)
    {
        *puc++ = (WE_UINT8)0X00;
    }
    for (iIndex = 0; iIndex < stKey.usExpLen; iIndex ++) 
    {
        *puc++ = stKey.pucExponent[iIndex];
    }

    (*piOutPubKeyLen) = usHdrLen + usMsgLen;
    
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleCurrentCertGet
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get current cert
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_INT32        iSecurityId[IN]:SECURITY ID
    WE_INT32        iOptions[IN]:OPTIONS
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_DpHandleCurrentCertGet(WE_HANDLE hSecHandle,
                                          WE_INT32 iTargetID, WE_INT32 iSecurityId,
                                          WE_INT32 iOptions)
{
    WE_UINT32   iValidNotBefore     = 0;
    WE_UINT32   iValidNotAfter      = 0;
    WE_INT16    sIssuercharSet      = 0;
    WE_VOID*    pvIssuer            = NULL;
    WE_INT32    iIssuerLen          = 0;
    WE_INT16    sSubjectcharSet     = 0;
    WE_VOID*    pvSubject           = NULL;
    WE_INT32    iSubjectLen         = 0;

    if(!hSecHandle)
    {
        return;
    }
    
    switch (SEC_STATES) 
    {
        case SEC_STATES_STOP: 
        {
            break;
        }

        case SEC_STATES_START: 
        {
            St_SecSessionInfo* pstInfo = NULL;

            WE_INT32 iResult = Sec_WimGetInfo(hSecHandle, iSecurityId, &pstInfo);

            if (1 == iOptions) 
            {
                /*add by bird 061107 for new UI spec*/
                WE_UCHAR* pucSerialNumber = NULL;/*serial number*/
                WE_UCHAR* pucAlg = NULL;/*signature algorthm*/
                WE_UCHAR* pucPubKey = NULL;/*public key*/   

                WE_UINT16 usSerialNumberLen= 0;
                WE_UINT16 usPubKeyLen = 0;  
                /*modified by Bird 070130*/
                if ((M_SEC_ERR_OK == iResult) && (pstInfo->ucNumCerts != 0)) 
                {
                    iResult = Sec_CertGetItemsForUe(hSecHandle,  
                                                    pstInfo->pstCerts[0].pucCert, 
                                                    &sIssuercharSet,  
                                                    &pvIssuer,  
                                                    &iIssuerLen,  
                                                    &sSubjectcharSet,  
                                                    &pvSubject,  
                                                    &iSubjectLen,  
                                                    &iValidNotBefore,  
                                                    &iValidNotAfter, 
                                                    &pucSerialNumber,  
                                                    &usSerialNumberLen,
                                                    &pucAlg,
                                                    &pucPubKey,
                                                    &usPubKeyLen);

                    if (M_SEC_ERR_OK == iResult) 
                    { 
                        Sec_iUeShowCertContent(hSecHandle,iTargetID, 0, sIssuercharSet,  
                                        pvIssuer, iIssuerLen,  
                                        iValidNotBefore, iValidNotAfter,  
                                        sSubjectcharSet, pvSubject,  
                                        iSubjectLen, pucSerialNumber, usSerialNumberLen, 
                                        pucAlg, pucPubKey,usPubKeyLen,M_SEC_ID_NONE);
                    }
                    /*Free memory*/
                    if(NULL != pucSerialNumber)
                    {
                        WE_FREE(pucSerialNumber);
                    }
                    if(NULL != pucAlg)
                    {
                        WE_FREE(pucAlg);
                    }
                                        
                    if(NULL != pucPubKey)
                    {
                        WE_FREE(pucPubKey);
                    }
                }
                else
                {
                    iResult = M_SEC_ERR_GENERAL_ERROR;
                }
                Sec_DpHandleCurrentCertDeliver(hSecHandle, iTargetID, iResult, iOptions, NULL);
            }
            else
            {
                Sec_DpHandleCurrentCertDeliver(hSecHandle, iTargetID, iResult, iOptions,  
                                             pstInfo->pstCerts[0].pucCert);
            }
            Sec_DpHandleCleanUp(hSecHandle);
            break;
        }

        default:
        {
            break;
        }
    }
    
    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleSessionInfoView
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get view of session info
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_INT32        iSecurityId[IN]:SECURITY ID
    WE_INT32        iOptions[IN]:OPTIONS
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_DpHandleSessionInfoView(WE_HANDLE hSecHandle, 
                                           WE_INT32 iTargetID, WE_INT32 iSecurityId ,
                                           WE_INT32 iOptions)
{
    if(!hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES) 
    {
    case SEC_STATES_STOP: 
        {
            break;
        }
        
    case SEC_STATES_START: 
        {
            St_SecSessionInfo*  pstInfo = NULL;
            WE_INT32            iResult = Sec_WimGetInfo(hSecHandle, iSecurityId, &pstInfo);
            
            if ((1 == iOptions) && (M_SEC_ERR_OK == iResult))
            {
                Sec_iUeShowSessionContent(hSecHandle,iTargetID, *pstInfo);
            }
            Sec_DpHandleSessionInfoDeliver(hSecHandle, iTargetID, iResult, iOptions, pstInfo);
            
            Sec_DpHandleCleanUp(hSecHandle);
            break;
        }
        
    default:
        {
            break;
        }
    }
    
    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleCurrentSecClassGet
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get current security class
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]: sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_INT32        iSecurityId[IN]:SECURITY ID
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HGSC_STATES_INIT_WIM                                2

static WE_VOID Sec_DpHandleCurrentSecClassGet(WE_HANDLE hSecHandle, 
                                              WE_INT32 iTargetID, WE_INT32 iSecurityId)
{
    if(!hSecHandle)
    {
        return ;
    }
    switch (SEC_STATES) 
    {
        case SEC_STATES_STOP:
        {
            break;
        }

        case SEC_STATES_START: 
        {
            /* delete by Sam [070130] */
            /*
            if (SEC_WIM_INITIALISED) 
            {*/
            St_SecSessionInfo*  pstInfo         = NULL;
            WE_INT32            iSecClass       = 0;
            WE_INT32            iInfoAvailable  = 0;
            WE_INT32            iCertAvailable  = 0;
            WE_INT32            iResult         = M_SEC_ERR_OK;
            
            iResult = Sec_WimGetInfo(hSecHandle, iSecurityId, &pstInfo);

            if (M_SEC_ERR_OK == iResult) 
            {
                iSecClass = pstInfo->ucSecurityClass;
                iInfoAvailable = 1;
                iCertAvailable = (pstInfo->pstCerts != NULL);
            }
            else 
            {
                iSecClass = (iSecurityId >= 0);
                iInfoAvailable = 0;
                iCertAvailable = 0;
            }

            Sec_WtlsCurClassResp(hSecHandle, iTargetID, iSecClass, 
                                        iInfoAvailable, iCertAvailable);

            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, (WE_UINT8)0,
                        "security_class = %d n"
                        "info_available = %d n"
                        "cert_available = %d n",
                        iSecClass, iInfoAvailable, iCertAvailable));

            Sec_DpHandleCleanUp(hSecHandle);
            /*
            }
            else
            {
                SEC_STATES = SEC_HGSC_STATES_INIT_WIM;
            }*/
            break;
        }

        case SEC_HGSC_STATES_INIT_WIM: 
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);

            switch(iStatus) 
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }

                case M_SEC_ERR_MISSING_KEY:
                {
#ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
                    SEC_STATES = SEC_STATES_START;
                    break;
                }

                case M_SEC_ERR_OK:
                {
                    SEC_STATES = SEC_STATES_START;
                    break;
                }

                case M_SEC_ERR_USER_NOT_VERIFIED:
                default:
                {
                    Sec_WtlsCurClassResp(hSecHandle, iTargetID, 0, 0, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }        

        default:
        {
            break;
        }
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleSessionCleanUp
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cleanup session
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HCUS_STATES_CLEAR_SESSION                           1
#define SEC_HCUS_STATES_HANDLE_CLEAR_SESSION_RESP               2
#define SEC_HCUS_STATES_INIT_WIM                                3

static WE_VOID Sec_DpHandleSessionCleanUp(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{   
    WE_INT32 iRes = 0;
    
    if(!hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES)
    {
    case SEC_STATES_STOP: 
        {
            break;
        }
        
    case SEC_HCUS_STATES_CLEAR_SESSION: 
        {
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_WimClrSession(hSecHandle);
            SEC_STATES = SEC_HCUS_STATES_HANDLE_CLEAR_SESSION_RESP;
            break;
        }
        
    case SEC_HCUS_STATES_HANDLE_CLEAR_SESSION_RESP:
        {
            St_SecTlCleanUpSessionResp*    pstWimSCResp = NULL;
            
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_ClrSessionResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            
            pstWimSCResp = (St_SecTlCleanUpSessionResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_CLEANUP_SESSION != pstWimSCResp->eType)
            {
                Sec_ClrSessionResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimSCResp);                
                break;
            }
            
            if(M_SEC_ERR_WIM_NOT_INITIALISED == pstWimSCResp->iResult)
            {
                SEC_STATES = SEC_HCUS_STATES_INIT_WIM;
            }
            else
            {
                Sec_ClrSessionResp(hSecHandle, iTargetID, pstWimSCResp->iResult);
                iRes = Sec_WimStopInfo(hSecHandle);
                if (iRes != M_SEC_ERR_OK)
                {
                    return ;
                }
                Sec_DpHandleCleanUp(hSecHandle);
            }
            Sec_TlHandleWimStructFree(pstWimSCResp);
            
            break;
        }
        
    case SEC_HCUS_STATES_INIT_WIM: 
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);
            
            switch(iStatus)
            {
            case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
            case M_SEC_ERR_MISSING_KEY:
                {
#ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_WimClrSession(hSecHandle);
                    SEC_STATES = SEC_HCUS_STATES_HANDLE_CLEAR_SESSION_RESP;
                    break;
                }
                
            case M_SEC_ERR_OK:  
                {        
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_WimClrSession(hSecHandle);
                    SEC_STATES = SEC_HCUS_STATES_HANDLE_CLEAR_SESSION_RESP;
                    break;
                }
                
            case M_SEC_ERR_USER_NOT_VERIFIED:
            default:
                {
                    Sec_ClrSessionResp(hSecHandle, iTargetID, iStatus);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        
    default:
        {
            break;
        }
    }
    
    return;
}


#ifdef M_SEC_CFG_CAN_SIGN_TEXT
/*==================================================================================================
FUNCTION: 
    Sec_DpHandleTextSign
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    compute a text digital signature
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_INT32            iTargetID[IN]:TARGET ID
    WE_INT32            iSignId[IN]:SIGN ID
    const WE_CHAR*      pcText[IN]:TEXT DATA
    const WE_CHAR*      pcData[IN]:DATA
    WE_INT32            iDataLen[IN]:DATA LEN
    WE_INT32            iKeyIdType[IN]:KEY ID TYPE
    const WE_CHAR*      pcKeyId[IN]:KEY ID
    WE_INT32            iKeyIdLen[IN]:KEY ID LENGTH
    WE_INT32            iOptions[IN]:OPTIONS
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HTS_STATES_HANDLE_SIGN_COMFIRM                      1
#define SEC_HTS_STATES_HANDLE_VERIFY_USER                       2
#define SEC_HTS_STATES_GET_SIGN_TEXT_MATCHES                    3
#define SEC_HTS_STATES_SIGN_TEXT_MATCHES_RESP                   4
#define SEC_HTS_STATES_INIT_WIM                                 5
#define SEC_HTS_STATES_CHOOSE_CERT_RESP                         6
#define SEC_HTS_STATES_SIGN_TEXT_RESP                           7
#define SEC_HTS_STATES_STORE_CONTRACTS_RESP                     8
static WE_VOID Sec_DpHandleTextSign (WE_HANDLE hSecHandle,
                                     WE_INT32 iTargetID, WE_INT32 iSignId, 
                                     const WE_CHAR *pcText, const WE_CHAR *pcData, 
                                     WE_INT32 iDataLen, WE_INT32 iKeyIdType,
                                     const WE_CHAR *pcKeyId, WE_INT32 iKeyIdLen,
                                     WE_INT32 iOptions)
{
    if(!hSecHandle)
    {
        return;
    }
    
    switch(SEC_STATES)
    {
        case SEC_STATES_STOP: 
        {
            break;
        }
            
        case SEC_HTS_STATES_HANDLE_SIGN_COMFIRM: 
        {
            /*modified by Bird 061121 ,for signtext not need confirm dialog*/
            #if 0
            SEC_WAITING_FOR_UE_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iUeSignTextConfirm(hSecHandle, M_SEC_ID_CONFIRM_15, pcText, 
                                    M_SEC_ID_OK, M_SEC_ID_CANCEL);
            SEC_STATES = SEC_HTS_STATES_HANDLE_VERIFY_USER;
            #else
            SEC_STATES = SEC_HTS_STATES_GET_SIGN_TEXT_MATCHES;
            #endif
            break;
        }
            
        case SEC_HTS_STATES_HANDLE_VERIFY_USER: 
        {
            St_SecTlConfirmDlgResp*                pstUeCDResp = NULL;

            if (SEC_WAITING_FOR_UE_RESULT) 
            {
                break;
            }
            if (!SEC_CUR_UE_RESP)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
                                     NULL,0,0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }       
            pstUeCDResp =  \
                (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
            SEC_CUR_UE_RESP = NULL;
            if(E_SEC_DP_CONFIRM_DLG != pstUeCDResp->eType)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                     0,NULL,0,0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCDResp);

                break;
            }
            if (!(pstUeCDResp->ucAnswer))                                 
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                     0,NULL,0,0, M_SEC_SIGNTEXT_ABORT);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCDResp);

                break;
            }
            Sec_TlHandleUeStructFree(pstUeCDResp);
            SEC_STATES = SEC_HTS_STATES_GET_SIGN_TEXT_MATCHES;
            break;
        }
        
        case SEC_HTS_STATES_GET_SIGN_TEXT_MATCHES:
        {
            WE_INT32 iStatus = Sec_DpHandleUserVerify(hSecHandle, iTargetID, E_SEC_SIGN_TEXT);
            
            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                    
                case M_SEC_ERR_OK:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimFindMatchedSignText(hSecHandle,iTargetID, iSignId, iKeyIdType,  
                                               (const WE_CHAR *)pcKeyId, iKeyIdLen,  
                                               iOptions);
                    SEC_STATES = SEC_HTS_STATES_SIGN_TEXT_MATCHES_RESP;
                    break;
                }
                    
                case M_SEC_ERR_USER_NOT_VERIFIED:
                {
                    Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 
                                         0, NULL, 0, 0, M_SEC_SIGNTEXT_ABORT);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
                    
                case M_SEC_ERR_GENERAL_ERROR:
                {
                    Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 
                                         0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
                    
                case M_SEC_ERR_MISSING_KEY:
                default:
                {
                    Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 
                                         0, NULL, 0, 0, M_SEC_SIGNTEXT_MISSCERT);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        
        case SEC_HTS_STATES_SIGN_TEXT_MATCHES_RESP: 
        {
            St_SecTlGetSignMatchesResp*              pstWimGSMResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                     0,NULL,0,0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimGSMResp =  \
                (St_SecTlGetSignMatchesResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_GET_SIGN_MATCHES != pstWimGSMResp->eType)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                     0,NULL,0,0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimGSMResp);
                
                break;
            }

            if(M_SEC_ERR_WIM_NOT_INITIALISED == pstWimGSMResp->iResult)
            {
                SEC_STATES = SEC_HTS_STATES_INIT_WIM;
            }
            else if (0 == pstWimGSMResp->iNbrMatches)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                     0,NULL,0,0, M_SEC_SIGNTEXT_MISSCERT);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimGSMResp);
                break;
            }
            else
            {
                SEC_WAITING_FOR_UE_RESULT = TRUE;
                SEC_WANTS_TO_RUN = FALSE;
                Sec_iUeSelectCert (hSecHandle, iTargetID, 
                                         M_SEC_ID_TITLE_CHOOSE_CERT_PUB_KEY,  
                                         M_SEC_ID_VIEW, 
                                         pstWimGSMResp->iNbrMatches,  
                                         pstWimGSMResp->astCertNames);
                SEC_STATES = SEC_HTS_STATES_CHOOSE_CERT_RESP;
            }
            Sec_TlHandleWimStructFree(pstWimGSMResp);
            break;
        }
        
        case SEC_HTS_STATES_INIT_WIM:
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iSignId, FALSE);

            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                    
                case M_SEC_ERR_MISSING_KEY:
                case M_SEC_ERR_OK:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimFindMatchedSignText(hSecHandle, iTargetID, iSignId,  
                                               iKeyIdType,(const WE_CHAR*)pcKeyId,  
                                               iKeyIdLen, iOptions);
                    SEC_STATES = SEC_HTS_STATES_SIGN_TEXT_MATCHES_RESP;
                    break;
                }
                    
                case M_SEC_ERR_USER_NOT_VERIFIED:
                {
                    Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                         0,NULL,0,0, M_SEC_SIGNTEXT_ABORT);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
                    
                default:
                {
                    Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0, NULL, 0,  
                                         NULL, 0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        
        
        case SEC_HTS_STATES_CHOOSE_CERT_RESP:
        {
            St_SecTlChooseCertResp*     pstUeCCBFNResp = NULL;

            if (SEC_WAITING_FOR_UE_RESULT)      
            {      
                break;
            }
            if (!SEC_CUR_UE_RESP)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL,  
                                     0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstUeCCBFNResp =  \
                (St_SecTlChooseCertResp *)SEC_CUR_UE_RESP;
            SEC_CUR_UE_RESP = NULL;
            if(E_SEC_DP_CHOOSE_CERT != pstUeCCBFNResp->eType)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL,  
                                     0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCCBFNResp);

                break;
            }

            if (E_SEC_UE_CANCEL == pstUeCCBFNResp->eResult)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0,  
                                     NULL, 0, NULL, 0, NULL, 0, 0,  
                                     M_SEC_SIGNTEXT_ABORT);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCCBFNResp);

                break;
            }
            else if ((E_SEC_UE_OK != pstUeCCBFNResp->eResult) || (pstUeCCBFNResp->iCertId < 1))
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId, 0,  
                                     NULL, 0, NULL, 0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleUeStructFree(pstUeCCBFNResp);

                break;
            }
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimSignText(hSecHandle, iTargetID, iSignId, (const WE_CHAR *)pcData,  
                             iDataLen, pstUeCCBFNResp->iCertId, iOptions);
            Sec_TlHandleUeStructFree(pstUeCCBFNResp);
            SEC_STATES = SEC_HTS_STATES_SIGN_TEXT_RESP;
            break;
        }
        
        case SEC_HTS_STATES_SIGN_TEXT_RESP: 
        {
            St_SecTlSignText*                          pstWimTextSigned = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                     0,NULL,0,0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimTextSigned = (St_SecTlSignText *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_UI_SIGN_TEXT != pstWimTextSigned->eType)
            {
                Sec_SignTextResp(hSecHandle, iTargetID, iSignId,0,NULL,0,NULL, 
                                     0,NULL,0,0, M_SEC_SIGNTEXT_GENERALERROR);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimTextSigned);
                pstWimTextSigned = NULL;
                break;
            }            
            Sec_SignTextResp(hSecHandle, iTargetID, pstWimTextSigned->iSignId,  
                                 pstWimTextSigned->iAlgorithm, 
                                 (const WE_CHAR *)pstWimTextSigned->pcSignature,  
                                 pstWimTextSigned->iSigLen, 
                                 (const WE_CHAR *)pstWimTextSigned->pcHashedKey,  
                                 pstWimTextSigned->iHashedKeyLen, 
                                 (const WE_CHAR *)pstWimTextSigned->pcCertificate,  
                                 pstWimTextSigned->iCertificateLen, 
                                 pstWimTextSigned->iCertificateType,  
                                 pstWimTextSigned->iErr);

            if (M_SEC_SIGNTEXT_OK == pstWimTextSigned->iErr)
            {
                WE_UINT32   uiTime          = 0;
                
                uiTime = SEC_CURRENT_TIME();
                SEC_WAITING_FOR_WIM_RESULT = TRUE;
                SEC_WANTS_TO_RUN = FALSE;
                Sec_iWimStoreContract(hSecHandle, iTargetID, pcText,  
                                      (const WE_CHAR *)pstWimTextSigned->pcSignature,  
                                      (WE_UINT16)pstWimTextSigned->iSigLen, uiTime);
                SEC_STATES = SEC_HTS_STATES_STORE_CONTRACTS_RESP;                
                Sec_TlHandleWimStructFree(pstWimTextSigned);                
            }
            else
            {
                Sec_TlHandleWimStructFree(pstWimTextSigned);
                Sec_DpHandleCleanUp(hSecHandle);
            }             
            break;
        }        

        case SEC_HTS_STATES_STORE_CONTRACTS_RESP: 
        {
            St_SecTlStoreContractsResp*                  pstWimSCResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID,M_SEC_ERR_COULD_NOT_STORE_CONTRACT);
            #endif
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimSCResp =  \
                (St_SecTlStoreContractsResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if(E_SEC_DP_STORE_CONTRACT_RESP != pstWimSCResp->eType)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID,M_SEC_ERR_COULD_NOT_STORE_CONTRACT);
            #endif
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(pstWimSCResp);

                break;
            }
            
        #ifdef M_SEC_CFG_SHOW_WARNINGS
            if (M_SEC_ERR_OK != pstWimSCResp->iResult)
            {
                Sec_iUeWarning(hSecHandle,iTargetID,M_SEC_ERR_COULD_NOT_STORE_CONTRACT);
            }
        #endif
            Sec_TlHandleWimStructFree(pstWimSCResp);            
            Sec_DpHandleCleanUp(hSecHandle);
            break;
        }

        default:
        {

            break;
        }
    }

    return;
}
#endif

#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))
/*==================================================================================================
FUNCTION: 
    Sec_DpHandleContractsView
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get view of contracts
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_INT32            iTargetID[IN]:TARGET ID
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HCV_STATES_GET_CONTRACTS                            1
#define SEC_HCV_STATES_HANDLE_GET_CONTRACTS_RESP                2
#define SEC_HCV_STATES_INIT_WIM                                 3

static WE_VOID Sec_DpHandleContractsView(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    if(!hSecHandle)
    {
        return;
    }

    switch(SEC_STATES)
    {
        case SEC_STATES_STOP: 
        {
            break;
        }
            
        case SEC_HCV_STATES_GET_CONTRACTS: 
        {   
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimViewContracts(hSecHandle, iTargetID);
            SEC_STATES = SEC_HCV_STATES_HANDLE_GET_CONTRACTS_RESP;
            break;
        }
            
        case SEC_HCV_STATES_HANDLE_GET_CONTRACTS_RESP: 
        {
            St_SecTlGetContractsResp *pstWimGCResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
               break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_GetContractsListResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimGCResp = (St_SecTlGetContractsResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if (E_SEC_DP_GET_CONTRACTS_RESP != pstWimGCResp->eType)
            {
                Sec_GetContractsListResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            
            if (M_SEC_ERR_WIM_NOT_INITIALISED == pstWimGCResp->iResult)
            {
                SEC_STATES = SEC_HCV_STATES_INIT_WIM;
            }
            else
            {
                if (NULL == SEC_CONTRACTVEC)
                {
                    Sec_iUeShowContractsList(hSecHandle,iTargetID,pstWimGCResp->iNbrContracts,  
                                           pstWimGCResp->pstContracts);
                    Sec_GetContractsListResp(hSecHandle, iTargetID, pstWimGCResp->iResult);
                }
                else
                {
                    Sec_GetContractsListResp(hSecHandle, iTargetID, M_SEC_ERR_BUSY);
                }
                Sec_TlHandleWimStructFree(pstWimGCResp);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            Sec_TlHandleWimStructFree(pstWimGCResp);
            
            break;
        }
        
        case SEC_HCV_STATES_INIT_WIM: 
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);

            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                
                case M_SEC_ERR_MISSING_KEY:
                {
                #ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
                #endif
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimViewContracts(hSecHandle, iTargetID);
                    SEC_STATES = SEC_HCV_STATES_HANDLE_GET_CONTRACTS_RESP;
                    break;
                }    
                
                case M_SEC_ERR_OK:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimViewContracts(hSecHandle, iTargetID);
                    SEC_STATES = SEC_HCV_STATES_HANDLE_GET_CONTRACTS_RESP;
                    break;
                }
                
                case M_SEC_ERR_USER_NOT_VERIFIED:
                default:
                {
                    Sec_GetContractsListResp(hSecHandle, iTargetID, iStatus);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        
        default:
        {
            break;
        }
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleContractRemove
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    remove a contract
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_INT32        iContractId[IN]:CONTRACT ID
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HRC_STATES_REMOVE_CONTRACTS                         1
#define SEC_HRC_STATES_HANDLE_REMOVE_CONTRACTS_RESP             2
#define SEC_HRC_STATES_INIT_WIM                                 3

static WE_VOID Sec_DpHandleContractRemove(WE_HANDLE hSecHandle,
                                          WE_INT32 iTargetID, WE_INT32 iContractId)
{
    if(!hSecHandle)
    {
        return;
    }

    switch(SEC_STATES)
    {
        case SEC_STATES_STOP: 
        {
            break;
        }
            
        case SEC_HRC_STATES_REMOVE_CONTRACTS: 
        {
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimDelContract(hSecHandle, iContractId);
            SEC_STATES = SEC_HRC_STATES_HANDLE_REMOVE_CONTRACTS_RESP;
            break;
        }
            
        case SEC_HRC_STATES_HANDLE_REMOVE_CONTRACTS_RESP: 
        {
            St_SecTlRmContractsResp*     pstWimDCResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_RemoveContractResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimDCResp =  
                (St_SecTlRmContractsResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if (E_SEC_DP_RM_CNTRACT_RESP != pstWimDCResp->eType)
            {
                Sec_RemoveContractResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
            }
            else if (M_SEC_ERR_WIM_NOT_INITIALISED == pstWimDCResp->iResult)
            {
                SEC_STATES = SEC_HRC_STATES_INIT_WIM;
                Sec_TlHandleWimStructFree(pstWimDCResp);
                
                break;
            }
            else if (M_SEC_ERR_OK != pstWimDCResp->iResult)
            {
                Sec_RemoveContractResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
            }
            else
            {
                Sec_RemoveContractResp(hSecHandle, iTargetID, M_SEC_ERR_OK);
            }
            Sec_TlHandleWimStructFree(pstWimDCResp);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
        }
        
        case SEC_HRC_STATES_INIT_WIM: 
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);

            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                    
                case M_SEC_ERR_MISSING_KEY:
                {
                #ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
                #endif
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimDelContract(hSecHandle, iContractId);
                    SEC_STATES = SEC_HRC_STATES_HANDLE_REMOVE_CONTRACTS_RESP;
                    break;
                }
                
                case M_SEC_ERR_OK:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimDelContract(hSecHandle, iContractId);
                    SEC_STATES = SEC_HRC_STATES_HANDLE_REMOVE_CONTRACTS_RESP;
                    break;
                }
                
                case M_SEC_ERR_USER_NOT_VERIFIED:
                default:
                {
                    Sec_RemoveContractResp(hSecHandle, iTargetID, iStatus);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        
        default:
        {
            break;
        }
    }

    return;
}

#endif
/*==================================================================================================
FUNCTION: 
    Sec_DpHandleMimeResp
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle mime response
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_UINT32       iResult[IN]:
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HMR_STATES_INIT_WIM                                 2

static WE_VOID Sec_DpHandleMimeResp(WE_HANDLE hSecHandle,WE_INT32 iTargetId, WE_INT32 iResult)
{
    if(!hSecHandle)
    {
        return;
    }
    switch(SEC_STATES)                    
    {                                            
        case SEC_STATES_STOP:
        {
            break;
        }
        
        case SEC_STATES_START: 
        {
            if (M_SEC_ERR_OK == iResult)
            {
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            SEC_WAITING_FOR_UE_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iUeConfirm(hSecHandle,iTargetId, M_SEC_ID_CONFIRM_9, M_SEC_ID_OK,  
                                 M_SEC_ID_NONE);
            SEC_STATES = SEC_HMR_STATES_INIT_WIM;
            break;
        }
        
        case SEC_HMR_STATES_INIT_WIM:    
        {
            if (SEC_WAITING_FOR_UE_RESULT) 
            {       
                break;
            }
            if (!SEC_CUR_UE_RESP)
            {
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            Sec_DpHandleCleanUp(hSecHandle);                                    
            break;
        }
        
        default:
        {
            break;
        }
    }    
    
    return;
}


/*==================================================================================================
FUNCTION: 
    Sec_DpHandleQueueNodeAdd
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    add a node to dp queue
ARGUMENTS PASSED:
    WE_HANDLE                 hSecHandle[IN/OUT]:sec global data
    St_SecDpParameter*        pstNode[IN]:NODE STRUCT OF DP QUEUE
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_DpHandleQueueNodeAdd(WE_HANDLE hSecHandle, St_SecDpParameter* pstNode)
{
    if (!hSecHandle || !pstNode)
    {
        return;
    }
    pstNode->pstNext = SEC_QUEUE_HEAD.pstNext;
    pstNode->pstPrev = &SEC_QUEUE_HEAD;
    SEC_QUEUE_HEAD.pstNext = pstNode;
    pstNode->pstNext->pstPrev = pstNode;

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleQueueIsEmpty
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    get dp queue state
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 Sec_DpHandleQueueIsEmpty(WE_HANDLE hSecHandle)
{ 
    if(!hSecHandle)
    {
        return 0;
    }
    return (SEC_QUEUE_HEAD.pstNext == &SEC_QUEUE_TAIL);
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleCurrentCertDeliver
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    deliver current cert
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_INT32            iTargetID[IN]:TARGET ID
    WE_INT32            iResult[IN]:RESULT
    WE_INT32            iOptions[IN]:OPTION
    WE_UCHAR*           pucCert[IN]:DER ENCODED CERT STRING
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_DpHandleCurrentCertDeliver (WE_HANDLE hSecHandle,
                                               WE_INT32 iTargetID, WE_INT32 iResult, 
                                               WE_INT32 iOptions, WE_UCHAR* pucCert)
{ 
    if(!hSecHandle)
    {
        return;
    }
    pucCert = pucCert;
    if (1 == iOptions) 
    {
        Sec_GetCurSvrCertResp (hSecHandle, iTargetID, iResult);
    } 
    
    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleSessionInfoDeliver
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    deliver session info
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    WE_INT32                iTargetID[IN]:TARGET ID 
    WE_INT32                iResult[IN]: RESULT
    WE_INT32                iOptions[IN]:OPTION
    St_SecSessionInfo*      pstSessInf[IN]:SESSION INFO
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_DpHandleSessionInfoDeliver (WE_HANDLE hSecHandle,
                                               WE_INT32 iTargetID, WE_INT32 iResult, 
                                               WE_INT32 iOptions, 
                                               St_SecSessionInfo* pstSessInf)
{ 
    if(!hSecHandle)
    {
        return;
    }
    pstSessInf = pstSessInf;
    if (1 == iOptions) 
    {
        Sec_GetSessionInfoResp(hSecHandle, iTargetID, iResult);
    } 

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleUserCertReq
CREATE DATE:
    2006-07-07
AUTHOR:
    stone an
DESCRIPTION:
    handle to get the request of user cert.
ARGUMENTS PASSED:
    hSecHandle[IN]: Global data handle.
    iTargetID[IN]: ID of the object.
    ucCertUsage[IN]: anticipant usage of the user certificate.
    pucCountry[IN]: string of country.
    usCountryLen[IN]: length of country.
    pucProvince[IN]: string of province.
    usProvinceLen[IN]: length of province.
    pucCity[IN]: string of city.           
    usCityLen[IN]: length of city.
    pucCompany[IN]: string of company.
    usCompanyLen[IN]: length of company.
    pucDepartment[IN]: string of department.
    usDepartmentLen[IN]: length of department.
    pucName[IN]: string of name.
    usNameLen[IN]: length of name.
    pucEMail[IN]: string of e-mail.
    usEMailLen[IN]: length of e-mail.
    ucKeyUsage[IN]: type of key.
    ucSignType[IN]: type of signature.
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_UCR_STATES_USER_CERT_REQ                            0x01
#define SEC_UCR_STATES_HANDLE_USER_CERT_REQ_RESP                0x02
#define SEC_UCR_STATES_INIT_WIM                                 0x03
static WE_VOID Sec_DpHandleUserCertReq(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
            WE_UCHAR ucCertUsage, WE_UCHAR *pucCountry,WE_UINT16 usCountryLen, 
            WE_UCHAR *pucProvince, WE_UINT16 usProvinceLen, WE_UCHAR *pucCity, 
            WE_UINT16 usCityLen, WE_UCHAR *pucCompany, WE_UINT16 usCompanyLen, 
            WE_UCHAR *pucDepartment, WE_UINT16 usDepartmentLen,WE_UCHAR *pucName, 
            WE_UINT16 usNameLen,WE_UCHAR *pucEMail, WE_UINT16 usEMailLen, 
            WE_UCHAR ucKeyUsage, WE_UCHAR ucSignType)
{ 
    if (NULL == hSecHandle)
    {
        return;
    }

    switch(SEC_STATES)
    {
        case SEC_STATES_STOP: 
        {
            break;
        }
            
        case SEC_UCR_STATES_USER_CERT_REQ: 
        {   
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimUserCertReq(hSecHandle, iTargetID, ucCertUsage, pucCountry,
                                usCountryLen, pucProvince, usProvinceLen, pucCity,
                                usCityLen, pucCompany, usCompanyLen, pucDepartment, 
                                usDepartmentLen, pucName, usNameLen, pucEMail,
                                usEMailLen, ucKeyUsage, ucSignType);
            SEC_STATES = SEC_UCR_STATES_HANDLE_USER_CERT_REQ_RESP;
            break;
        }
            
        case SEC_UCR_STATES_HANDLE_USER_CERT_REQ_RESP: 
        {
            St_SecTlUserCertReqResp *pstWimUCRResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
               break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_UserCertReqResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            pstWimUCRResp = (St_SecTlUserCertReqResp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if (E_SEC_DP_GET_REQ_USER_CERT_RESP != pstWimUCRResp->eType)
            {
                Sec_UserCertReqResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
                Sec_TlHandleWimStructFree((WE_VOID *)pstWimUCRResp);  
                Sec_DpHandleCleanUp(hSecHandle);  
                break;
            }
            
            if (M_SEC_ERR_WIM_NOT_INITIALISED == pstWimUCRResp->iResult)
            {
                SEC_STATES = SEC_UCR_STATES_INIT_WIM;
            }
            else
            {
                Sec_UserCertReqResp(hSecHandle, iTargetID, pstWimUCRResp->iResult, 
                    pstWimUCRResp->pucCertReqMsg, pstWimUCRResp->uiCertReqMsgLen);
                Sec_TlHandleWimStructFree((WE_VOID *)pstWimUCRResp);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
            Sec_TlHandleWimStructFree((WE_VOID *)pstWimUCRResp);            
            break;
        }
        
        case SEC_UCR_STATES_INIT_WIM: 
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);

            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                
                case M_SEC_ERR_MISSING_KEY:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;        
                    Sec_iWimUserCertReq(hSecHandle, iTargetID, ucCertUsage, pucCountry,
                                usCountryLen, pucProvince, usProvinceLen, pucCity,
                                usCityLen, pucCompany, usCompanyLen, pucDepartment, 
                                usDepartmentLen, pucName, usNameLen, pucEMail,
                                usEMailLen, ucKeyUsage, ucSignType);
                    SEC_STATES = SEC_UCR_STATES_HANDLE_USER_CERT_REQ_RESP;
                    break;
                }    
                
                case M_SEC_ERR_OK:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;       
                    Sec_iWimUserCertReq(hSecHandle, iTargetID, ucCertUsage, pucCountry,
                                usCountryLen, pucProvince, usProvinceLen, pucCity,
                                usCityLen, pucCompany, usCompanyLen, pucDepartment, 
                                usDepartmentLen, pucName, usNameLen, pucEMail,
                                usEMailLen, ucKeyUsage, ucSignType);
                    SEC_STATES = SEC_UCR_STATES_HANDLE_USER_CERT_REQ_RESP;
                    break;
                }
                
                case M_SEC_ERR_USER_NOT_VERIFIED:
                default:
                {
                    Sec_UserCertReqResp(hSecHandle, iTargetID, iStatus, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        
        default:
        {
            break;
        }
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleChangeWtlsCertAble
CREATE DATE:
    2006-11-13
AUTHOR:
    stone an
DESCRIPTION:
    handle change the able attribute of the wtls CA certificate.
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_INT32            iTargetID[IN]:TARGET ID
    WE_UINT32           uiCertId[IN]:ID of certificate.
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
#define SEC_HCWCA_STATES_CHANGE_WTLS_CERT_ABLE                    1
#define SEC_HCWCA_STATES_CHANGE_WTLS_CERT_ABLE_RESP               2
#define SEC_HCWCA_STATES_INIT_WIM                                 3

static WE_VOID Sec_DpHandleChangeWtlsCertAble(WE_HANDLE hSecHandle, 
                                    WE_INT32 iTargetID, WE_UINT32 uiCertId)
{
    if(!hSecHandle)
    {
        return;
    }

    switch(SEC_STATES)
    {
        case SEC_STATES_STOP: 
        {
            break;
        }
            
        case SEC_HCWCA_STATES_CHANGE_WTLS_CERT_ABLE: 
        {   
            SEC_WAITING_FOR_WIM_RESULT = TRUE;
            SEC_WANTS_TO_RUN = FALSE;
            Sec_iWimChangeWTLSCertAble(hSecHandle, iTargetID, uiCertId);
            SEC_STATES = SEC_HCWCA_STATES_CHANGE_WTLS_CERT_ABLE_RESP;
            break;
        }
            
        case SEC_HCWCA_STATES_CHANGE_WTLS_CERT_ABLE_RESP: 
        {
            St_SecTlChangeWtlsCertAbleresp *pstWimGWCAResp = NULL;

            if (SEC_WAITING_FOR_WIM_RESULT)
            {
               break;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_ChangeWTLSCertAbleResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0);
                break;
            }
            pstWimGWCAResp = (St_SecTlChangeWtlsCertAbleresp *)SEC_CUR_WIM_RESP;
            SEC_CUR_WIM_RESP = NULL;
            if (E_SEC_DP_CHANGE_WTLS_CERT_ABLE_RESP != pstWimGWCAResp->eType)
            {
                Sec_TlHandleWimStructFree(pstWimGWCAResp);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_ChangeWTLSCertAbleResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0);
                break;
                
            }
            
            if (M_SEC_ERR_WIM_NOT_INITIALISED == pstWimGWCAResp->iResult)
            {
                SEC_STATES = SEC_HCWCA_STATES_INIT_WIM;
            }
            else
            {
                Sec_ChangeWTLSCertAbleResp(hSecHandle, iTargetID, M_SEC_ERR_OK, pstWimGWCAResp->ucState);
                Sec_TlHandleWimStructFree(pstWimGWCAResp);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
        }        /* fall through */
        //lint -fallthrough
        
        case SEC_HCWCA_STATES_INIT_WIM: 
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);

            switch(iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                {
                    break;
                }
                
                case M_SEC_ERR_MISSING_KEY:
                {
                #ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
                #endif
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimChangeWTLSCertAble(hSecHandle, iTargetID, uiCertId);
                    SEC_STATES = SEC_HCWCA_STATES_CHANGE_WTLS_CERT_ABLE_RESP;
                    break;
                }    
                
                case M_SEC_ERR_OK:
                {
                    SEC_WAITING_FOR_WIM_RESULT = TRUE;
                    SEC_WANTS_TO_RUN = FALSE;
                    Sec_iWimChangeWTLSCertAble(hSecHandle, iTargetID, uiCertId);
                    SEC_STATES = SEC_HCWCA_STATES_CHANGE_WTLS_CERT_ABLE_RESP;
                    break;
                }
                
                case M_SEC_ERR_USER_NOT_VERIFIED:
                default:
                {
                    Sec_ChangeWTLSCertAbleResp(hSecHandle, iTargetID, iStatus, 0);                
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        }
        //lint -fallthrough
        
        default:
        {
            break;
        }
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_DpHandleMain
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    dp main handler
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    St_SecMessage*      pstSig[IN]:MESSAGE
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_DpHandleMain (WE_HANDLE hSecHandle, St_SecMessage* pstMsg)
{
    WE_VOID*        pvMsg = NULL;

    if(!hSecHandle || !pstMsg || !pstMsg->pvParam)
    {
        return;
    }
    switch (pstMsg->iType)
    {
        case M_SEC_DP_MSG_GET_CIPHERSUITE:
        {
            St_SecDpGetWtlsCipherSuite*                 pstData = NULL;
            
            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_GET_CIPHERSUITE n"));
            pstData = \
                (St_SecDpGetWtlsCipherSuite* )WE_MALLOC((WE_ULONG)sizeof(St_SecDpGetWtlsCipherSuite));
            if(!pstData)
            {
               Sec_WtlsGetCipherSuiteResp(pstMsg->hItype, ((St_SecWtlsGetCipherSuite *)(pstMsg->pvParam))->iTargetID,  
                                      M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0,NULL,0,NULL,0);
                break;
            }

            pstData->eType = E_SEC_DP_GET_CIPHERSUITE;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_EXCHANGE_WTLS_KEY:
        {
            St_SecDpWtlsKeyXch*                    pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_EXCHANGE_WTLS_KEY n"));
            pstData = \
                (St_SecDpWtlsKeyXch*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpWtlsKeyXch));
            if(!pstData)
            {
                Sec_WtlsKeyExchangeResp(pstMsg->hItype, ((St_SecWtlsKeyExchange *)(pstMsg->pvParam))->iTargetID,  
                                      M_SEC_ERR_INSUFFICIENT_MEMORY,0,NULL,0);
                break;
            }
            pstData->eType = E_SEC_DP_XCH_WTLS_KEY;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;    
        }
        
        case M_SEC_DP_MSG_WTLS_PRF:
        {
            St_SecDpWtlsPrf*                        pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal SEC_MAIN_SIG_E_SEC_DP_WTLS_PRF n"));
            pstData = \
                (St_SecDpWtlsPrf*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpWtlsPrf));
            if(!pstData)
            {
                Sec_WtlsGetPrfResultResp(pstMsg->hItype, ((St_SecWtlsGetPrfResult *)(pstMsg->pvParam))->iTargetID,  
                                      M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0);
                break;
            }

            pstData->eType = E_SEC_DP_WTLS_PRF;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_VERIFY_WTLS_CERT_CHAIN:
        {
            St_SecDpVerifyWtlsCertChain*            pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal SEC_MAIN_SIG_E_SEC_DP_WTLS_VERIFY_CERT_CHAIN n"));
            pstData = \
                (St_SecDpVerifyWtlsCertChain*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpVerifyWtlsCertChain));
            if(!pstData)
            {
                Sec_WtlsVerifySvrCertResp(pstMsg->hItype, ((St_SecWtlsVerifySvrCert *)(pstMsg->pvParam))->iTargetID,  
                               M_SEC_ERR_INSUFFICIENT_MEMORY);
                break;
            }
            pstData->eType = E_SEC_DP_VERIFY_WTLS_CERT_CHAIN;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_GET_WTLS_CLIENT_CERT:
#ifdef M_SEC_CFG_WTLS_CLASS_3
        {

            St_SecDpGetWtlsClientCert*              pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_GET_WTLS_CLIENT_CERT n"));
            pstData = \
                (St_SecDpGetWtlsClientCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpGetWtlsClientCert));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_WtlsGetUsrCertResp(pstMsg->hItype, ((St_SecWtlsGetUsrCert *)(pstMsg->pvParam))->iTargetID,  
                               M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0,NULL,0);
                }
                break;
            }
            pstData->eType = E_SEC_DP_GET_WTLS_CLIENT_CERT;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
#else
        {
            St_SecWtlsGetUsrCert*    pstData = pstMsg->pvParam;

            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_WtlsGetUsrCertResp(pstMsg->hItype, ((St_SecWtlsGetUsrCert *)(pstMsg->pvParam))->iTargetID,  
                           M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0,NULL,0);
                }
                break;
            }

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal SEC_MAIN_SIG_WTLS_GET_CLIENT_CERT n"));
            Sec_WtlsGetUsrCertResp(pstMsg->hItype, pstData->iTargetID,  
                                          M_SEC_ERR_OK, NULL, 0, NULL, 0);
            break;
        }
#endif    
    
        case M_SEC_DP_MSG_COMPUTE_WTLS_SIGN:
#ifdef M_SEC_CFG_WTLS_CLASS_3
        {
            St_SecDpComputeWtlsSign*                 pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0, 
                         "SEC: received signal SEC_MAIN_SIG_E_SEC_DP_COMPUTE_WTLS_SIGN n"));
            pstData = \
                (St_SecDpComputeWtlsSign*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpComputeWtlsSign));
            if(!pstData)
            {
               if(pstMsg->pvParam)
               {
                    Sec_WtlsCompSigResp(pstMsg->hItype, ((St_SecWtlsCompSign *)(pstMsg->pvParam))->iTargetID,  
                               M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0);
                }
               break;
            }
            pstData->eType = E_SEC_DP_COMPUTE_WTLS_SIGN;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
#else
        {
            St_SecWtlsCompSign*     pstData = pstMsg->pvParam;

            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_WtlsCompSigResp(pstMsg->hItype, ((St_SecWtlsCompSign *)(pstMsg->pvParam))->iTargetID,  
                               M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0);
                }
                break;
            }
            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal SEC_MAIN_SIG_WTLS_COMPUTE_SIGNATURE n"));
            Sec_WtlsCompSigResp(pstMsg->hItype, pstData->iTargetID,  
                                             M_SEC_ERR_NOT_IMPLEMENTED, NULL, 0);
            break;
        }
#endif 
#ifdef M_SEC_CFG_CAN_SIGN_TEXT
        case M_SEC_DP_MSG_SIGN_TEXT:    
        {
            St_SecDpSignText*                       pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_SIGN_TEXT n"));
            pstData = (St_SecDpSignText*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpSignText));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_SignTextResp(pstMsg->hItype, ((St_SecSignText *)(pstMsg->pvParam))->iTargetID, 0,0, 
                               NULL,0,NULL,0,NULL,0,0,M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }

            pstData->eType = E_SEC_DP_UI_SIGN_TEXT;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
#endif

        case M_SEC_DP_MSG_ESTABLISH_CONN:
        {
            St_SecDpConnEst*                pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal SEC_MAIN_SIG_E_SEC_DP_ESTABLISH_CONN n"));
            pstData = \
                (St_SecDpConnEst*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpConnEst));
            if(!pstData)
            {
                break;
            }

            pstData->eType = E_SEC_DP_ESTABLISHED_CONN;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_TERMINATE_CONN:
        {
            St_SecDpConnTmt*                 pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_TERMINATE_CONN n"));
            pstData = \
                (St_SecDpConnTmt*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpConnTmt));
            if(!pstData)
            {
                break;
            }

            pstData->eType = E_SEC_DP_TERMINATED_CONN;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;    
        }
        
        case M_SEC_DP_MSG_RM_PEER_LINKS:
        {
            St_SecDpRmPeerLinks*                pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_RM_PEER_LINKS n"));
            pstData = \
                (St_SecDpRmPeerLinks*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpRmPeerLinks));
            if(!pstData)
            {
                break;
            }

            pstData->eType = E_SEC_DP_RM_PEER_LINKS;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;  
        }
        
        case M_SEC_DP_MSG_LINK_PEER_SESSION:
        {
            St_SecDpPeerLinkSession*              pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_LINK_PEER_SESSION n"));
            pstData = \
                (St_SecDpPeerLinkSession*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpPeerLinkSession));
            if(!pstData)
            {
                break;
            }

            pstData->eType = E_SEC_DP_LINK_PEER_SESSION;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;  
        }
        
        case M_SEC_DP_MSG_LOOKUP_PEER:
        {
            St_SecDpLookupPeer*                     pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_LOOKUP_PEER n"));
            pstData = \
                (St_SecDpLookupPeer*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpLookupPeer));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_SearchPeerResp(pstMsg->hItype, ((St_SecSearchPeer *)(pstMsg->pvParam))->iTargetID,
                                 M_SEC_ERR_INSUFFICIENT_MEMORY, 0,0,0);
                }
                break;
            }

            pstData->eType = E_SEC_DP_LOOKUP_PEER;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_ACTIVE_SESSION:
        {
            St_SecDpActiveSession*                  pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_ACTIVE_SESSION n"));
            pstData = \
                (St_SecDpActiveSession*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpActiveSession));
            if(!pstData)
            {
                break;
            }
            pstData->eType = E_SEC_DP_ACTIVE_SESSION;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_INVALID_SESSION:
        {
            St_SecDpInvalidSession*              pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                        "SEC: received signal M_SEC_DP_MSG_INVALID_SESSION n"));
            pstData = \
                (St_SecDpInvalidSession*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpInvalidSession));
            if(!pstData)
            {
                break;
            }

            pstData->eType = E_SEC_DP_INVALID_SESSION;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_FETCH_SESSION:
        {
            St_SecDpfetchSession*                   pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_FETCH_SESSION n"));
            pstData = \
                (St_SecDpfetchSession*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpfetchSession));
            if(!pstData)
            {               
                Sec_SessionGetResp(pstMsg->hItype, ((St_SecFetchSessionInfo *)(pstMsg->pvParam))->iTargetID,
                                  M_SEC_ERR_INSUFFICIENT_MEMORY, 0,NULL,0,NULL,0,NULL,0);
                    
                break;
            }

            pstData->eType = E_SEC_DP_FETCH_SESSION;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_UPDATE_SESSION:
        {
            St_SecDpUpdateSession*                  pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_UPDATE_SESSION n"));
            pstData = \
                (St_SecDpUpdateSession*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpUpdateSession));
            if(!pstData)
            {
                break;
            }

            pstData->eType = E_SEC_DP_UPDATE_SESSION;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_STORE_CERT:
        {
            St_SecDpStoreCert*          pstData         = NULL;
            WE_UCHAR                    ucFormat        = 0; 

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_STORE_CERT n"));
            pstData = \
                (St_SecDpStoreCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpStoreCert));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_SaveCertResp(pstMsg->hItype, ((St_SecSaveCert *)(pstMsg->pvParam))->iTargetID,
                                  M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }
            pstData->pstStrct = pstMsg->pvParam;
            if(!pstData->pstStrct)
            {
                WE_FREE(pstData);
                break;
            }
            if (0 == WeSCL_StrCmpNc(pstData->pstStrct->pcContentType, 
                                "application/vnd.wap.wtls-ca-certificate")) 
            {
                pstData->eType = E_SEC_DP_STORE_CA_CERT;
                ucFormat = M_SEC_CERTIFICATE_WTLS_TYPE; 
            }
            else if(0 == WeSCL_StrCmpNc(pstData->pstStrct->pcContentType, 
                                    "application/x-x509-ca-cert")) 
            {
                pstData->eType = E_SEC_DP_STORE_CA_CERT;
                ucFormat = M_SEC_CERTIFICATE_X509_TYPE; 
            }
            else if (0 == WeSCL_StrCmpNc(pstData->pstStrct->pcContentType, 
                                     "application/vnd.wap.wtls-user-certificate")) 
            {
                pstData->eType = E_SEC_DP_STORE_USER_CERT;
                ucFormat = M_SEC_CERTIFICATE_WTLS_TYPE; 
            }
            else if (0 == WeSCL_StrCmpNc(pstData->pstStrct->pcContentType, 
                                     "application/x-x509-user-cert")) 
            {
                pstData->eType = E_SEC_DP_STORE_USER_CERT;
                ucFormat = M_SEC_CERTIFICATE_X509_TYPE; 
            }
            else 
            {  
                Sec_SaveCertResp(pstMsg->hItype, pstData->pstStrct->iTargetID,  
                                      M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE);
                Sec_TlHandleDpStructFree((St_SecDpParameter*)pstData);
                break;
            }
            pstData->pstStrct->pucCert[0] = ucFormat;
            pstData->pstStrct->usCertLen += 1;
            pvMsg = pstData;
            break;       
        }
        
        case M_SEC_DP_MSG_TRUSTED_CA_INFO:
        {
            St_SecDpCaInfo*              pstData = NULL;
            
            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_TRUSTED_CA_INFO n"));
            pstData = \
                (St_SecDpCaInfo*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpCaInfo));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_TrustedCAInfoResp(pstMsg->hItype, ((St_SecTrustedCaInfo *)(pstMsg->pvParam))->iTargetID,
                                  M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }

            pstData->pstStrct = pstMsg->pvParam;
            if(!pstData->pstStrct)
            {
                WE_FREE(pstData);
                break;
            }
            
            if (!WeSCL_StrCmpNc(pstData->pstStrct->pcContentType, 
                                "application/vnd.wap.hashed-certificate"))
            {
                pstData->eType = E_SEC_DP_HASH_VERIFICATION;
            }
            else if (!WeSCL_StrCmpNc(pstData->pstStrct->pcContentType, 
                                     "application/vnd.wap.signed-certificate"))
            {
                pstData->eType = E_SEC_DP_VRRIFY_SIGN;
            }
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_DELIVERY_CERT:
        {
            St_SecDpDeliveryCert*               pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_DELIVERY_CERT n"));
            pstData = \
                (St_SecDpDeliveryCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpDeliveryCert));
            if(!pstData)
            {
                break;
            }
            pstData->eType = E_SEC_DP_VERIFY_CERT;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_VIEW_CERT_NAMES:
        {
            St_SecDpViewCertNames*              pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_VIEW_CERT_NAMES n"));
            pstData = \
                (St_SecDpViewCertNames*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpViewCertNames));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_GetCertNameListResp(pstMsg->hItype, ((St_SecGetCertNameList *)(pstMsg->pvParam))->iTargetID,
                                  M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }
            pstData->eType = E_SEC_DP_VIEW_CERT_NAMES;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }

        case M_SEC_DP_MSG_GEN_KEY_PAIR:
        {
            St_SecDpGenKeyPair*            pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_GEN_KEY_PAIR n"));
            pstData = \
                (St_SecDpGenKeyPair*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpGenKeyPair));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_KeyPairGenResp(pstMsg->hItype, ((St_SecGenKeyPair *)(pstMsg->pvParam))->iTargetID,
                                  M_SEC_ERR_INSUFFICIENT_MEMORY,0,NULL,0,NULL,0);
                }
                break;
            }
            pstData->eType = E_SEC_DP_GEN_KEY_PAIR;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }

        case M_SEC_DP_MSG_GET_USER_PUB_KEY:
        {
            St_SecDpGetUserPubKey*              pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_GET_USER_PUB_KEY n"));
            pstData = \
                (St_SecDpGetUserPubKey*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpGetUserPubKey));
            if(!pstData)
            {   
                if(pstMsg->pvParam)
                {
                    Sec_GetPubKeyResp(pstMsg->hItype, ((St_SecGetPublicKey *)(pstMsg->pvParam))->iTargetID,
                                 M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0,NULL,0);
                }
                break;
            }
            pstData->eType = E_SEC_DP_GET_USER_PUB_KEY;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_CHANGE_PIN:
        {
            St_SecDpChangePin*                  pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_CHANGE_PIN n"));
            pstData = \
                (St_SecDpChangePin*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpChangePin));
            if(!pstData)
            {
                break;
            }

            pstData->eType = E_SEC_DP_CHANGE_PIN;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }

        case M_SEC_DP_MSG_VIEW_CUR_CERT:
        {
            St_SecDpViewCurCert*            pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_VIEW_CUR_CERT n"));
            pstData = \
                (St_SecDpViewCurCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpViewCurCert));
            if(!pstData)
            {               
                break;
            }
            pstData->eType = E_SEC_DP_VIEW_CUR_CERT;
            pstData->pstStrct = pstMsg->pvParam;
            pstData->iOptions = pstMsg->iParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_VIEW_SESSION:
        {
            St_SecDpViewSessionInfo*            pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_VIEW_SESSION n"));
            pstData = \
                (St_SecDpViewSessionInfo*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpViewSessionInfo));
            if(!pstData)
            {
                break;
            } 
            pstData->eType = E_SEC_DP_VIEW_SESSION;
            pstData->pstStrct = pstMsg->pvParam;
            pstData->iOptions = pstMsg->iParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_GET_CUR_SEC_CLASS:
        {
            St_SecDpGetCurSecClass*         pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_GET_CUR_SEC_CLASS n"));
            pstData = \
                (St_SecDpGetCurSecClass*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpGetCurSecClass));
            if(!pstData)
            {
                break;
            }
            pstData->eType = E_SEC_DP_GET_CUR_SEC_CLASS;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
        
        case M_SEC_DP_MSG_CLEANUP_SESSION:
        {
            St_SecDpCleanUpSession*               pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_CLEANUP_SESSION n"));
            pstData = \
                (St_SecDpCleanUpSession*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpCleanUpSession));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_ClrSessionResp(pstMsg->hItype, ((St_SecClrSession *)(pstMsg->pvParam))->iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }
            pstData->eType = E_SEC_DP_CLEANUP_SESSION;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))        
        case M_SEC_DP_MSG_VIEW_CONTRACTS:
        {
            St_SecDpViewContracts*              pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_VIEW_CONTRACTS n"));
            pstData = \
                (St_SecDpViewContracts*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpViewContracts));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_GetContractsListResp(pstMsg->hItype, ((St_SecGetContractInfo *)(pstMsg->pvParam))->iTargetID, 
                                M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }
            pstData->eType = E_SEC_DP_VIEW_CONTRACTS;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
#endif
        case M_SEC_DP_MSG_GET_CERT:
        {
            St_SecDpGetCert*                    pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_GET_CERT n"));
            pstData = \
                (St_SecDpGetCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpGetCert));
            if(!pstData)
            { 
                if(pstMsg->pvParam)
                {
                    Sec_ViewAndGetCertResp(pstMsg->hItype, ((St_SecGetCertContent *)(pstMsg->pvParam))->iTargetID, 
                                   M_SEC_ERR_INSUFFICIENT_MEMORY,0,NULL,0);
                }
                break;
            }
            pstData->eType = E_SEC_DP_GET_CERT;
            pstData->pstStrct = pstMsg->pvParam;
            pstData->iOptions = pstMsg->iParam;
            pvMsg = pstData;
            break;
        }

        case M_SEC_DP_MSG_CHANGE_WTLS_CERT_ABLE:
        {
            St_SecDpChangeWtlsCertAble*   pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_CHANGE_WTLS_CERT_ABLE n"));
            pstData = \
                (St_SecDpChangeWtlsCertAble*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpChangeWtlsCertAble));
            if (NULL == pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_ChangeWTLSCertAbleResp(pstMsg->hItype, ((St_SecChangeWtlsCertAble *)(pstMsg->pvParam))->iTargetID, 
                                   M_SEC_ERR_INSUFFICIENT_MEMORY,0);
                }
                break;
            }
            pstData->eType = E_SEC_DP_CHANGE_WTLS_CERT_ABLE;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
     
        case M_SEC_DP_MSG_RM_CERT:
        {
            St_SecDpRmCert*                 pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_RM_CERT n"));
            pstData = \
                (St_SecDpRmCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpRmCert));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_DeleteCertResponse(pstMsg->hItype, ((St_SecRemoveCert *)(pstMsg->pvParam))->iTargetID, 
                                  0,M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }
            pstData->eType = E_SEC_DP_RM_CERT;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))  
        case M_SEC_DP_MSG_RM_CONTRACT:
        {
            St_SecDpRmContract*             pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_RM_CONTRACT n"));
            pstData = \
                (St_SecDpRmContract*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpRmContract));
            if(!pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_RemoveContractResp(pstMsg->hItype, ((St_SecRemoveContract *)(pstMsg->pvParam))->iTargetID, 
                                  M_SEC_ERR_INSUFFICIENT_MEMORY);
                }
                break;
            }
            pstData->eType = E_SEC_DP_RM_CONTRACT;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
#endif
        case M_SEC_DP_MSG_EXECUTE_COMMAND:
        {
            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_EXECUTE_COMMAND n"));
            Sec_GetTransferCertInfo(pstMsg->hItype, pstMsg->pvParam);
            break;
        }

        case M_SEC_DP_MSG_REQ_USER_CERT:
        {
            St_SecDpUserCertReq*   pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                        "SEC: received signal M_SEC_DP_MSG_REQ_USER_CERT n"));
            pstData = (St_SecDpUserCertReq*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpUserCertReq));
            if(NULL == pstData)
            {
                if(pstMsg->pvParam)
                {
                    Sec_UserCertReqResp(pstMsg->hItype, ((St_SecUserCertReqInfo *)(pstMsg->pvParam))->iTargetID, 
                                  M_SEC_ERR_INSUFFICIENT_MEMORY,NULL,0);
                }
                break;
            }

            pstData->eType = E_SEC_DP_GET_REQ_USER_CERT;
            pstData->pstStrct = pstMsg->pvParam;
            pvMsg = pstData;
            break;
        }
     
        case M_SEC_DP_MSG_STORE_CERT_RESP:
        {
            St_SecDpMimeResp*                   pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0, 
                         "SEC: received signal M_SEC_DP_MSG_STORE_CERT_RESP n"));  
            pstData = \
                (St_SecDpMimeResp*)WE_MALLOC((WE_ULONG)sizeof( St_SecDpMimeResp));
            if(!pstData)
            {
                Sec_DpHandleMimeResp(pstMsg->hItype, ((St_SecSaveCertResp *)(pstMsg->pvParam))->iTargetID, 
                                  M_SEC_ERR_INSUFFICIENT_MEMORY);
                break;
            }
            pstData->pstStrct = pstMsg->pvParam;
            if(!pstData->pstStrct)
            {
                Sec_DpHandleMimeResp(pstMsg->hItype, ((St_SecSaveCertResp *)(pstMsg->pvParam))->iTargetID, 
                                  G_SEC_INVALID_PARAMETER);
                WE_FREE(pstData);
                break;
            }
            pstData->eType = E_SEC_DP_MIME_RESP;
            if (M_SEC_ERR_OK != pstData->pstStrct->usResult)
            {
                pvMsg = pstData;
            }
            else
            {
                M_SEC_SELF_FREE(pstMsg->pvParam);
                WE_FREE(pstData);
            }
            break;
        }
        
        case M_SEC_DP_MSG_TRUSTED_CA_INFO_RESP:
        {
            St_SecDpMimeResp*                   pstData = NULL;
            
            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_TRUSTED_CA_INFO_RESP n"));  
            pstData = \
                (St_SecDpMimeResp*)WE_MALLOC((WE_ULONG)sizeof( St_SecDpMimeResp));
            if(!pstData)
            {
                Sec_DpHandleMimeResp(pstMsg->hItype, ((St_SecSaveCertResp *)(pstMsg->pvParam))->iTargetID, 
                                  M_SEC_ERR_INSUFFICIENT_MEMORY);
                break;
            }
            pstData->pstStrct = pstMsg->pvParam;
            if(!pstData->pstStrct)
            {
                Sec_DpHandleMimeResp(pstMsg->hItype, ((St_SecSaveCertResp *)(pstMsg->pvParam))->iTargetID, 
                                  M_SEC_ERR_INVALID_PARAMETER);
                WE_FREE(pstData);
                break;
            }
            pstData->eType = E_SEC_DP_MIME_RESP;
            if (M_SEC_ERR_OK != pstData->pstStrct->usResult)
            {
                pvMsg = pstData;
            }
            else
            {
                M_SEC_SELF_FREE(pstMsg->pvParam);
                WE_FREE(pstData);
            }
            break; 
        }    
        
        case M_SEC_DP_MSG_DELVERY_CERT_RESP:
        {
            St_SecDpMimeResp*                   pstData = NULL;

            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received signal M_SEC_DP_MSG_DELVERY_CERT_RESP n"));  
            pstData = \
                (St_SecDpMimeResp*)WE_MALLOC((WE_ULONG)sizeof( St_SecDpMimeResp));
            if(!pstData)
            {
                Sec_DpHandleMimeResp(pstMsg->hItype, ((St_SecSaveCertResp *)(pstMsg->pvParam))->iTargetID, 
                                  M_SEC_ERR_INSUFFICIENT_MEMORY);
                break;
            }
            pstData->pstStrct = pstMsg->pvParam;
            if(!pstData->pstStrct)
            {
                Sec_DpHandleMimeResp(pstMsg->hItype, ((St_SecSaveCertResp *)(pstMsg->pvParam))->iTargetID, 
                                  M_SEC_ERR_INVALID_PARAMETER);
                WE_FREE(pstData);
                break;
            }
            pstData->eType = E_SEC_DP_MIME_RESP;
            if (M_SEC_ERR_OK != pstData->pstStrct->usResult)
            {
                pvMsg = pstData;
            }
            else
            {
                WE_FREE(pstMsg->pvParam);
                WE_FREE(pstData);
            }
            break;
        }
        /*action function asynchronism*/
        case M_SEC_DP_MSG_SHOWCONTRACTLIST:
            {
                St_SecDpShowContractList*             pstData = NULL;

                WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                             "SEC: received signal M_SEC_DP_MSG_SHOWCONTRACTLIST n"));
                pstData = \
                    (St_SecDpShowContractList*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpShowContractList));
                if(!pstData)
                {
                    break;
                }
                pstData->eType = E_SEC_DP_SHOWCONTRACTLIST;
                pstData->pstStrct = pstMsg->pvParam;
                pvMsg = pstData;
            }
            break;
        case M_SEC_DP_MSG_SHOWCONTRACTCONTENT:
            {
                St_SecDpShowContractContent*             pstData = NULL;

                WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                             "SEC: received signal M_SEC_DP_MSG_SHOWCONTRACTCONTENT n"));
                pstData = \
                    (St_SecDpShowContractContent*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpShowContractContent));
                if(!pstData)
                {
                    break;
                }
                pstData->eType = E_SEC_DP_SHOWCONTRACTCONTENT;
                pstData->pstStrct = pstMsg->pvParam;
                pvMsg = pstData;
            }
            break;
        case M_SEC_DP_MSG_SHOWCERTLIST:
            {
                St_SecDpShowCertList*             pstData = NULL;

                WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                             "SEC: received signal M_SEC_DP_MSG_SHOWCERTLIST n"));
                pstData = \
                    (St_SecDpShowCertList*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpShowCertList));
                if(!pstData)
                {
                    break;
                }
                pstData->eType = E_SEC_DP_SHOWCERTTLIST;
                pstData->pstStrct = pstMsg->pvParam;
                pvMsg = pstData;
            }
            break;
        case M_SEC_DP_MSG_SHOWCERTCONTENT:
            {
                St_SecDpShowCertContent*             pstData = NULL;

                WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                             "SEC: received signal M_SEC_DP_MSG_SHOWCONTRACTCONTENT n"));
                pstData = \
                    (St_SecDpShowCertContent*)WE_MALLOC((WE_ULONG)sizeof(St_SecDpShowCertContent));
                if(!pstData)
                {
                    break;
                }
                pstData->eType = E_SEC_DP_SHOWCERTCONTENT;
                pstData->pstStrct = pstMsg->pvParam;
                pvMsg = pstData;
            }
            break;
        /*modified by Bird 070130*/
        case M_SEC_DP_MSG_HASH:
            {
                St_HashAction stHash = {0};
                WE_INT32 iTargetID = 0;
                WE_INT32 iRet = 0;
                iTargetID = ((St_SecHashAction*)(pstMsg->pvParam))->iTargetID; 
                stHash = ((St_SecHashAction*)(pstMsg->pvParam))->stHash;
                iRet = Sec_UeHashAction(hSecHandle,iTargetID,stHash);
                if(iRet != 0)
                {
                }
                if(NULL != pstMsg->pvParam)
                {
                    WE_FREE(pstMsg->pvParam);
                }
            }
            break;
        case M_SEC_DP_MSG_STORECERT:
            {
                St_StoreCertAction stStoreCert = {0};
                WE_INT32 iTargetID = 0;
                WE_INT32 iRet = 0;
                iTargetID = ((St_SecStoreAction*)(pstMsg->pvParam))->iTargetID; 
                stStoreCert = ((St_SecStoreAction*)(pstMsg->pvParam))->stStoreCert;
                iRet = Sec_UeStoreCertAction(hSecHandle,iTargetID,stStoreCert);
                if(iRet != 0)
                {
                }
                if(NULL != pstMsg->pvParam)
                {
                    WE_FREE(pstMsg->pvParam);
                }
            }
            break;
        case M_SEC_DP_MSG_CONFIRM:
            {
                St_ConfirmAction stConfirm = {0};
                WE_INT32 iTargetID = 0;
                WE_INT32 iRet = 0;
                iTargetID = ((St_SecConfirmAction*)(pstMsg->pvParam))->iTargetID; 
                stConfirm = ((St_SecConfirmAction*)(pstMsg->pvParam))->stConfirm;
                iRet = Sec_UeConfirmAction(hSecHandle,iTargetID,stConfirm);
                if(iRet != 0)
                {
                }
                if(NULL != pstMsg->pvParam)
                {
                    WE_FREE(pstMsg->pvParam);
                }
                
            }
            break;
        case M_SEC_DP_MSG_NAMECONFIRM:
            {
                St_NameConfirmAction stNameConfirm = {0};
                WE_INT32 iTargetID = 0;
                WE_INT32 iRet = 0;
                iTargetID = ((St_SecNameConfirmAction*)(pstMsg->pvParam))->iTargetID; 
                stNameConfirm = ((St_SecNameConfirmAction*)(pstMsg->pvParam))->stNameConfirm;
                iRet = Sec_UeNameConfirmAction(hSecHandle,iTargetID,stNameConfirm);
                if(iRet != 0)
                {
                }
                if(NULL != pstMsg->pvParam)
                {
                    WE_FREE(pstMsg->pvParam);
                }
            }
            break;
        case M_SEC_DP_MSG_SELECTCERT:
            {
                St_SelectCertAction stSelCert = {0};
                WE_INT32 iTargetID = 0;
                WE_INT32 iRet = 0;
                iTargetID = ((St_SecSelCertAction*)(pstMsg->pvParam))->iTargetID; 
                stSelCert = ((St_SecSelCertAction*)(pstMsg->pvParam))->stSelCert;
                iRet = Sec_UeSelectAction(hSecHandle,iTargetID,stSelCert);
                if(iRet != 0)
                {
                }
                if(NULL != pstMsg->pvParam)
                {
                    WE_FREE(pstMsg->pvParam);
                }
            }
            break;
        default: 
        {
            WE_LOG_MSG ((WE_LOG_DETAIL_LOW, (WE_UINT8)0,  
                         "SEC: received unknown signal: %d n", pstMsg->iType));
            break;
        }    
    }
    
    if(pvMsg)
    {
        if (Sec_DpHandleQueueIsEmpty(hSecHandle))
        {
            SEC_WANTS_TO_RUN = TRUE;
        }
        ((St_SecDpParameter*)pvMsg)->hItype = pstMsg->hItype; /*added by bird 061113*/
        Sec_DpHandleQueueNodeAdd(hSecHandle, (St_SecDpParameter*)pvMsg);
        if(SEC_STATES_STOP == SEC_STATES)    
        {           
            SEC_STATES = SEC_STATES_START;               
        }
    }
    /**
    pstMsg->pvParam = NULL;
    */
    Sec_MsgRemove (hSecHandle, pstMsg);
    pstMsg->pvParam = NULL;
    return;
}
#define SEC_NBROFCERTS   (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> iNbrofCerts)
#define SEC_CERTNAMES     (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstCertNames)
#define SEC_NBRCONTRACTS    (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> iNbrContracts)
#define SEC_CERTIDS          (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->piCertIds)
#define SEC_FRIENDLY_NAME            (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pucFriendlyName)
#define SEC_USER_CERT                (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pucTmpUserCert)
#define SEC_DISPLAYNAME (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> pcDisplayName)
#define SEC_PKCOUT       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstPKCOut)
#define SEC_MSG_QUEUE_HEAD                  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstMsgQueueHead)
#define SEC_BREW_IRSA               (((St_PublicData *)(((ISecB*)hSecHandle)->hPrivateData))->m_pIRsa) 
#define SEC_UECALLBACK       (((St_PublicData *)(((ISecW*)hSecHandle)->hPrivateData))->m_SecUeCB)
#define SEC_RESPCALLBACK       (((St_PublicData *)(((ISecW*)hSecHandle)->hPrivateData))->m_SecRespCB)
#define SEC_UECALLBACKDATA  (((St_PublicData *)(((ISecW*)hSecHandle)->hPrivateData))->pvUeCBData)
#define SEC_RESPCALLBACKDATA  (((St_PublicData *)(((ISecW*)hSecHandle)->hPrivateData))->pvRespCBData)

WE_VOID Sec_DpResetVariable(WE_HANDLE hSecHandle)
{
    WE_INT32 iIndex = 0;
    /*modify by Bird 061205 for each interface can run independently ,and do not influence other interfaces*/
#if 0   
    ISec * pISec = NULL;
    
    /*if current function is not the release interface*/
    if(NULL != SEC_CUR_FUNCTION)
    {
        if(hSecHandle != (((St_SecDpParameter*)SEC_CUR_FUNCTION)->hItype))
        {
            /*added by bird 061117*/
            pISec = (ISec*)(((St_SecDpParameter*)SEC_CUR_FUNCTION)->hItype);
            SEC_REGCALLBACK(pISec->m_pIShell,((St_PublicData*)(pISec->hPrivateData))->m_cb,
                Sec_RunMainFlow,pISec);
            return;
        }
    }
 #endif
    /*added by bird 061219*/
    SEC_CANCELCALLBACK(SEC_UECALLBACK);
    SEC_CANCELCALLBACK(SEC_RESPCALLBACK);

    /*1.Free global pointer*/
    /*certificates list*/
    if(NULL != SEC_UECALLBACKDATA)
    {
        Sec_ReleaseUeCB(hSecHandle);
        SEC_UECALLBACKDATA = NULL;
    }
    if(NULL != SEC_RESPCALLBACKDATA)
    {
        Sec_ReleaseRespCB(hSecHandle);
        SEC_RESPCALLBACKDATA = NULL;
    }
    
    if(NULL != SEC_CERTNAMES)
    {
        for (iIndex = 0; iIndex <SEC_NBROFCERTS; iIndex++) 
        {
            WE_FREE (SEC_CERTNAMES[iIndex].pvFriendlyName);
        }
        WE_FREE (SEC_CERTNAMES);
        SEC_CERTNAMES = NULL;
    }
    /*contracts list*/
    if(NULL != SEC_CONTRACTVEC)
    {
        for (iIndex = 0;iIndex < SEC_NBRCONTRACTS; iIndex++) 
        {
            WE_FREE (SEC_CONTRACTVEC[iIndex].pcContract);
            WE_FREE (SEC_CONTRACTVEC[iIndex].pcSignature);
        }
        WE_FREE (SEC_CONTRACTVEC); 
        SEC_CONTRACTVEC = NULL;
    }

    if(NULL != SEC_CERTIDS)
    {
        WE_FREE (SEC_CERTIDS);
        SEC_CERTIDS = NULL;
    }

    if(NULL != SEC_DISPLAYNAME)
    {
        WE_FREE (SEC_DISPLAYNAME);
        SEC_DISPLAYNAME = NULL;
    }
    
    if (NULL != SEC_FRIENDLY_NAME)
    {
        WE_FREE(SEC_FRIENDLY_NAME);
        SEC_FRIENDLY_NAME = NULL;
    }

    if(NULL != SEC_USER_CERT)
    {
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
    }
    /*Free IRSA*/
    if(NULL != SEC_BREW_IRSA)
    {
        (void)IRSA_Release(SEC_BREW_IRSA);
        SEC_BREW_IRSA = NULL;
    }
    /*PKC out*/
    if(NULL != SEC_PKCOUT)
    {
        WE_FREE(SEC_PKCOUT);
        SEC_PKCOUT = NULL;
    }
#if  0   
    /*2.manage state initial*/
    Sec_TlHandleDpStructFree(SEC_CUR_FUNCTION);
    SEC_CUR_FUNCTION = NULL;
    Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    
    
    SEC_WAITING_FOR_UE_RESULT = FALSE;
    SEC_WAITING_FOR_SEC_RESULT = FALSE;
    SEC_WAITING_FOR_WIM_RESULT = FALSE;
    
    SEC_STATES = SEC_STATES_START;
    SEC_WANTS_TO_RUN = TRUE;/*set to true*/


    /*resume call back*/

    /*1.find the second queue*/
    if (!Sec_DpHandleQueueIsEmpty(hSecHandle))
    {
        pISec = (ISec*)(SEC_QUEUE_TAIL.pstPrev->hItype);
        SEC_REGCALLBACK(pISec->m_pIShell,((St_PublicData*)(pISec->hPrivateData))->m_cb,
            Sec_RunMainFlow,pISec);
    }
    /*2.search the first queue*/

    else if (NULL != SEC_MSG_QUEUE_HEAD) 
    {
        pISec  = (ISec*)(SEC_MSG_QUEUE_HEAD->hItype);
        SEC_REGCALLBACK(pISec->m_pIShell,((St_PublicData*)(pISec->hPrivateData))->m_cb,
            Sec_RunMainFlow,pISec);
    }
#endif
}






















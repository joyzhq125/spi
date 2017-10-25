/*==================================================================================================
    HEADER NAME : sec_str.c
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
    2006-07-07   steven ding        0.0.1         Draft
==================================================================================================*/

#include "sec_comm.h"


/*************************************************************************************************
external function
*************************************************************************************************/

/*==================================================================================================
FUNCTION: 
    Sec_TlHandleTiStructFree
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    free ue struct
ARGUMENTS PASSED:
    WE_VOID*                pvFunctionId[IN]:ue struct need be free
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
WE_VOID Sec_TlHandleUeStructFree(WE_VOID* pvFncId)

{
    if (!pvFncId)
    {
        return;
    }
    
    switch (((St_SecTlUeFncId*)pvFncId)->eType)
    {        
        case E_SEC_DP_STORE_CERT_DLG:
        {
            St_SecTlStoreCertDlgResp* pstData = NULL;

            pstData = (St_SecTlStoreCertDlgResp*)pvFncId;
            if (pstData->pucCertFriendlyName)
            {
                M_SEC_SELF_FREE(pstData->pucCertFriendlyName);
            }
            break;
        }    
        
        case E_SEC_DP_VERIFY_PIN:
        {
            St_SecTlVerifyPin* pstData = NULL;

            pstData = (St_SecTlVerifyPin*)pvFncId;
            if(pstData->pcPin)
            {
                M_SEC_SELF_FREE(pstData->pcPin);
            }    
            break;
        }
        
        case E_SEC_DP_VERIFY_HASH:
        {
            St_SecTlVerifyHash* pstData = NULL;

            pstData = (St_SecTlVerifyHash*)pvFncId;
            if(pstData->pucHash)
            {
                M_SEC_SELF_FREE(pstData->pucHash);
            }    
            break;
        }
        
        case E_SEC_DP_CHANGE_PIN:
        {
            St_SecTlChangePin* pstData = NULL;

            pstData = (St_SecTlChangePin*)pvFncId;
            if(pstData->pcOldPin)
            {
                M_SEC_SELF_FREE(pstData->pcOldPin);
            }    
            if(pstData->pcNewPin)
            {
                M_SEC_SELF_FREE(pstData->pcNewPin);
            }    
            break;
        }

        case E_SEC_DP_GEN_PIN:
        {
            St_SecTlGenPinResp* pstData = NULL;

            pstData = (St_SecTlGenPinResp*)pvFncId;
            if (pstData->pcPin)
            {
                M_SEC_SELF_FREE(pstData->pcPin);
            }    
            break;
        }    

        default:
        {
            break;
        }    
    }
    M_SEC_SELF_FREE(pvFncId);

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_TlHandleWimStructFree
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    free wim struct
ARGUMENTS PASSED:
    WE_VOID*                pvFunctionId[IN]:wim struct need to be free
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

WE_VOID Sec_TlHandleWimStructFree(WE_VOID*            pvFncId)
{
    WE_INT32 iIndex = 0;
    
    if (!pvFncId)
    {
        return;
    }
    
    switch (((St_SecTlWimFncId*)pvFncId)->eType)
    {
        case E_SEC_DP_GET_CIPHERSUITE:   
        {
            St_SecTlGetWtlsMethodsResp* pstData = NULL;

            pstData = (St_SecTlGetWtlsMethodsResp*)pvFncId;
            if(pstData->pucCipherMethods)
            {
                M_SEC_SELF_FREE(pstData->pucCipherMethods);
            }    
            if(pstData->pucKeyExchangeIds)
            {
                M_SEC_SELF_FREE(pstData->pucKeyExchangeIds);
            }    
            if (pstData->pucTrustedKeyIds)
            {
                M_SEC_SELF_FREE(pstData->pucTrustedKeyIds);
            }    
            break;
        }    
        case E_SEC_DP_GET_TLS_CIPHERSUITE:   
        {
            St_SecTlGetTlsCipherSuitesResp* pstData = NULL;

            pstData = (St_SecTlGetTlsCipherSuitesResp*)pvFncId;
            if(pstData->pucCipherSuites)
            {
                M_SEC_SELF_FREE(pstData->pucCipherSuites);
            }   
 
            break;
        } 

        case E_SEC_DP_XCH_KEY:   
        {
            St_SecTlKeyXchResp* pstData = NULL;

            pstData = (St_SecTlKeyXchResp*)pvFncId;
            if (pstData->pucPublicValue)
            {
                M_SEC_SELF_FREE(pstData->pucPublicValue);
            }        
            break;
        }
         case E_SEC_DP_XCH_TLS_KEY:   
        {
            St_SecTlsTlKeyXchResp* pstData = NULL;

            pstData = (St_SecTlsTlKeyXchResp*)pvFncId;
            if (pstData->pucPublicValue)
            {
                M_SEC_SELF_FREE(pstData->pucPublicValue);
            }        
            break;
        }
        case E_SEC_DP_PRF:  
        {
            St_SecTlPrfResp* pstData = NULL;

            pstData = (St_SecTlPrfResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->pucBuf)
                {
                    M_SEC_SELF_FREE(pstData->pucBuf);
                }
            }    
            break;
        }
        case E_SEC_DP_TLS_PRF:  
        {
            St_SecTlPrfResp* pstData = NULL;

            pstData = (St_SecTlPrfResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->pucBuf)
                {
                    M_SEC_SELF_FREE(pstData->pucBuf);
                }
            }    
            break;
        }
        case E_SEC_DP_VERIRY_USER_CERT_CHAIN:  
        {
            St_SecTlVerifyUserCertResp* pstData = NULL;

            pstData = (St_SecTlVerifyUserCertResp*)pvFncId;
            if (pstData->pucCaCert)
            {
                M_SEC_SELF_FREE(pstData->pucCaCert); 
            }    
            break;
        }

        case E_SEC_DP_COMPUTE_SIGN: 
        {
            St_SecTlComputeSignResp* pstData = NULL;

            pstData = (St_SecTlComputeSignResp*)pvFncId;
            if(pstData->pucSig)
            {
                M_SEC_SELF_FREE(pstData->pucSig);
            }    
            break;    
        }
        
        case E_SEC_DP_FETCH_SESSION: 
        {
            St_SecTlFetchSessionResp* pstData = NULL;

            pstData = (St_SecTlFetchSessionResp*)pvFncId;
            if(pstData->pucSessionId)
            {
                M_SEC_SELF_FREE(pstData->pucSessionId);
            }    
            if(pstData->pucPrivateKeyId)
            {
                M_SEC_SELF_FREE(pstData->pucPrivateKeyId);
            }    
            break;
        }

        case E_SEC_DP_GET_CERT_NAMES:  
        {
            St_SecTlCertNamesResp* pstData = NULL;

            pstData = (St_SecTlCertNamesResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->pstCertNames)
                {
                    for (iIndex = 0; iIndex < pstData->iNbrOfCerts; iIndex++)
                    {
                        if (pstData->pstCertNames[iIndex].pvFriendlyName)
                        {
                            M_SEC_SELF_FREE(pstData->pstCertNames[iIndex].pvFriendlyName);
                        }    
                    }
                    M_SEC_SELF_FREE(pstData->pstCertNames);
                }
            }
            break;
        }
        
        case E_SEC_DP_GET_CONN_CERT_NAMES:
        {
            St_SecTlConnCertNamesResp* pstData = NULL;

            pstData = (St_SecTlConnCertNamesResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->pstCertNames)
                {
                    for (iIndex = 0; iIndex < pstData->iNbrOfCerts; iIndex++)
                    {
                        if (pstData->pstCertNames[iIndex].pvFriendlyName)
                        {
                            M_SEC_SELF_FREE(pstData->pstCertNames[iIndex].pvFriendlyName);
                        }
                        if (pstData->pcChainDepth)
                        {
                            if (0 != pstData->pcChainDepth[iIndex])
                            {
                                if ((pstData->ppuiCertChainIds)&&
                                    (pstData->ppuiCertChainIds[iIndex]))
                                {
                                    M_SEC_SELF_FREE(pstData->ppuiCertChainIds[iIndex]);
                                }    
                            }
                        }
                    }
                    M_SEC_SELF_FREE(pstData->pstCertNames);
                }

                if (pstData->pcChainDepth)
                {
                    M_SEC_SELF_FREE(pstData->pcChainDepth); 
                }    
                if (pstData->ppuiCertChainIds)
                {
                    M_SEC_SELF_FREE(pstData->ppuiCertChainIds); 
                }    
            }
            break;
        }
        
        case E_SEC_DP_GET_CERT_CHAIN:
        {
            St_SecTlGetChainOfCertsResp* pstData = NULL;

            pstData = (St_SecTlGetChainOfCertsResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->ppucCerts)
                {
                    for (iIndex = 0; iIndex < pstData->iNbrOfCerts; iIndex++)
                    {
                        if (pstData->ppucCerts[iIndex])
                        {
                            M_SEC_SELF_FREE(pstData->ppucCerts[iIndex]);
                        }
                    }
                }
                if (pstData->pusCertLen)
                {
                    M_SEC_SELF_FREE(pstData->pusCertLen);
                }    
                if (pstData->ppucCerts)
                {
                    M_SEC_SELF_FREE(pstData->ppucCerts); 
                }    
            }    
            break;
        }
        
        case E_SEC_DP_GET_CERT_IDS:
        {
            St_SecTlGetCertIdsResp* pstData = NULL;

            pstData = (St_SecTlGetCertIdsResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->pucCertificateIds)
                {
                    M_SEC_SELF_FREE(pstData->pucCertificateIds);
                }    
            }
            break;    
        }
        
        case E_SEC_DP_GET_CERT: 
        {
            St_SecTlGetCertResp* pstData = NULL;

            pstData = (St_SecTlGetCertResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if(pstData->pucCert)
                {
                    M_SEC_SELF_FREE(pstData->pucCert);
                }    
                if(pstData->pucTrustedUsage)
                {
                    M_SEC_SELF_FREE(pstData->pucTrustedUsage);
                }    
            }
            break;
        }
               

        case E_SEC_DP_GET_UCERT_AND_SKEY:
        {
            St_SecTlGetUcertSignedkeyResp* pstData = NULL;

            pstData = (St_SecTlGetUcertSignedkeyResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->pucCert)
                {
                    M_SEC_SELF_FREE(pstData->pucCert);
                }    
                if (pstData->pucSig)
                {
                    M_SEC_SELF_FREE(pstData->pucSig);
                }    
            }
            break;
        }    

        case E_SEC_DP_GET_USER_CERT: 
        {
            St_SecTlGetUcertKeyIdResp* pstData = NULL;

            pstData = (St_SecTlGetUcertKeyIdResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if(pstData->pucCert)
                {
                    M_SEC_SELF_FREE(pstData->pucCert);
                }
                if(pstData->pucKeyId)
                {
                    M_SEC_SELF_FREE(pstData->pucKeyId);
                }    
            }
            break;     
        }    

        case E_SEC_DP_READ_VIEWS_CERTS: 
        {
            St_SecTlReadViewsCertsResp* pstData = NULL;

            pstData = (St_SecTlReadViewsCertsResp*)pvFncId;
            if (pstData->pucData)
            {
                M_SEC_SELF_FREE(pstData->pucData);
            }    
            break;
        }
        
        case E_SEC_DP_UI_SIGN_TEXT: 
        {
            St_SecTlSignText* pstData = NULL;

            pstData = (St_SecTlSignText*)pvFncId;
            if (pstData->iErr != M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                if (pstData->pcSignature)
                {
                    M_SEC_SELF_FREE(pstData->pcSignature);
                }    
                if (pstData->pcHashedKey)
                {
                    M_SEC_SELF_FREE(pstData->pcHashedKey);
                }    
                if (pstData->pcCertificate)
                {
                    M_SEC_SELF_FREE(pstData->pcCertificate);
                }    
            }
            break;
        }
        
        case E_SEC_DP_GET_SIGN_MATCHES: 
        {
            St_SecTlGetSignMatchesResp* pstData = NULL;

            pstData = (St_SecTlGetSignMatchesResp*)pvFncId;
            if (pstData->iResult != M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                for (iIndex = 0; iIndex < pstData->iNbrMatches; iIndex++)
                {
                    if (pstData->astCertNames[iIndex].pvFriendlyName)
                    {
                        M_SEC_SELF_FREE(pstData->astCertNames[iIndex].pvFriendlyName);
                    }
                }                      
            }
            break;
        }    

        case E_SEC_DP_GET_CONTRACTS_RESP:
        {
            St_SecTlGetContractsResp* pstData = NULL;

            pstData = (St_SecTlGetContractsResp*)pvFncId;
            if (M_SEC_ERR_WIM_NOT_INITIALISED != pstData->iResult)
            {
                if (pstData->pstContracts)
                {
                    for (iIndex = 0; iIndex < pstData->iNbrContracts; iIndex++)
                    {
                        if (pstData->pstContracts[iIndex].pcContract)
                        {
                            M_SEC_SELF_FREE(pstData->pstContracts[iIndex].pcContract);
                        }    
                        if (pstData->pstContracts[iIndex].pcSignature)
                        {
                            M_SEC_SELF_FREE(pstData->pstContracts[iIndex].pcSignature);
                        }    
                    }
                    M_SEC_SELF_FREE(pstData->pstContracts);
                }
                if (pstData->piContractsLen)
                {
                    M_SEC_SELF_FREE(pstData->piContractsLen);
                }    
            }
            break;
        }
 

        case E_SEC_DP_GEN_KEY_PAIR: 
        {
            St_SecTlGenRsaKeypairResp* pstData = NULL;

            pstData = (St_SecTlGenRsaKeypairResp*)pvFncId;
            if(pstData->pucPublicKey)
            {
                M_SEC_SELF_FREE(pstData->pucPublicKey);
            }    
            if(pstData->pucSig)
            {
                M_SEC_SELF_FREE(pstData->pucSig);
            }    
            break;
        }

        case E_SEC_DP_GET_REQ_USER_CERT_RESP: 
        {
            St_SecTlUserCertReqResp* pstData = NULL;

            pstData = (St_SecTlUserCertReqResp *)pvFncId;
            if(pstData->pucCertReqMsg)
            {
                M_SEC_SELF_FREE(pstData->pucCertReqMsg);
            }  
            break;
        }
        
        case E_SEC_DP_CHANGE_WTLS_CERT_ABLE_RESP: 
        {
        }   
            break;
        default:
        {
            break;
        }    
    }
    M_SEC_SELF_FREE(pvFncId);

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_TlHandleDpStructFree
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    free dp struct 
ARGUMENTS PASSED:
    St_SecDpParameter*        pstPara[IN]:dp struct need to free
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

WE_VOID Sec_TlHandleDpStructFree(WE_VOID* pvPara)
{
    if (!pvPara)
    {
        return;
    }    

    switch (((St_SecDpParameter*)pvPara)->eType)
    {
        case E_SEC_DP_ESTABLISHED_CONN: 
        {
            St_SecDpConnEst* pstData = NULL;

            pstData = (St_SecDpConnEst*)pvPara;
            if(pstData->pstStrct)
            {
                if(NULL != pstData->pstStrct->stSessionInfo.pstCerts)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->stSessionInfo.pstCerts->pucCert);
                    M_SEC_SELF_FREE(pstData->pstStrct->stSessionInfo.pstCerts);
                }
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        
        case E_SEC_DP_TERMINATED_CONN:
        {
            St_SecDpConnTmt* pstData = NULL;

            pstData = (St_SecDpConnTmt*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }

        case E_SEC_DP_GET_CIPHERSUITE: 
        {
            St_SecDpGetWtlsCipherSuite* pstData = NULL;

            pstData = (St_SecDpGetWtlsCipherSuite*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        case E_SEC_DP_GET_TLS_CIPHERSUITE: 
        {
            St_SecDpGetTlsCipherSuite* pstData = NULL;

            pstData = (St_SecDpGetTlsCipherSuite*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        case E_SEC_DP_XCH_WTLS_KEY: 
        {
            St_SecDpWtlsKeyXch* pstData = NULL;

            pstData = (St_SecDpWtlsKeyXch*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucRandval)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucRandval);
                }
                if(pstData->pstStrct->pstParam)
                {
                    if ((E_SEC_WTLS_KEYEXCHANGE_RSA_ANON == pstData->pstStrct->pstParam->ucKeyExchangeSuite) ||
                        (E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512 == pstData->pstStrct->pstParam->ucKeyExchangeSuite) ||
                        (E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768 == pstData->pstStrct->pstParam->ucKeyExchangeSuite))
                    {
                        if ((pstData->pstStrct->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen) > 0)
                        {
                            M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
                        }    
                        if ((pstData->pstStrct->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen) > 0)
                        {
                            M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus);
                        }    
                        if (255 == pstData->pstStrct->pstParam->stParams.stKeyParam.stParamSpecifier.ucParameterIndex)                    
                        {
                            if ((pstData->pstStrct->pstParam->stParams.stKeyParam.stParamSpecifier.usParamLen) > 0)
                            {
                                M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stParams.stKeyParam.stParamSpecifier.pucParams);
                            }    
                        }
                  }
                  else if ((E_SEC_WTLS_KEYEXCHANGE_RSA == pstData->pstStrct->pstParam->ucKeyExchangeSuite) ||
                        (E_SEC_WTLS_KEYEXCHANGE_RSA_512 == pstData->pstStrct->pstParam->ucKeyExchangeSuite) ||
                        (E_SEC_WTLS_KEYEXCHANGE_RSA_768 == pstData->pstStrct->pstParam->ucKeyExchangeSuite))
                    {
                        if ((pstData->pstStrct->pstParam->stParams.stCertificates.usBufLen) > 0)
                        {
                            M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stParams.stCertificates.pucBuf);
                         }    
                    }                  
                  WE_FREE(pstData->pstStrct->pstParam);
                  pstData->pstStrct->pstParam =NULL;
                }
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_XCH_TLS_KEY: 
        {
            St_SecDpTlsKeyXch* pstData = NULL;

            pstData = (St_SecDpTlsKeyXch*)pvPara;
            if(pstData->pstStrct)
            {
            switch(pstData->pstStrct->ucHashAlg)
            {
            case E_SEC_TLS_KEYEXCH_DHE_DSS: /* sec_tls_server_dh_params */
            case E_SEC_TLS_KEYEXCH_DHE_DSS_EXPORT:
            case E_SEC_TLS_KEYEXCH_DHE_RSA:
            case E_SEC_TLS_KEYEXCH_DHE_RSA_EXPORT: 
                if(pstData->pstStrct->pstParam)
                {
                    if(NULL != pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHG )
                    {                   
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHG);
                    }
                    
                    if(NULL != pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHYs)                   
                    {
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHYs);             
                    }
                    if(NULL != pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHP)
                    {
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHP);              
                    }
                    if(NULL != pstData->pstStrct->pstParam->stDHParam.stTlsSignature.pucSignature)
                    {
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stDHParam.stTlsSignature.pucSignature);
                    }
                }
                break;
            case E_SEC_TLS_KEYEXCH_RSA:/* When RSA is used */
            case E_SEC_TLS_KEYEXCH_DH_DSS_EXPORT:
            case E_SEC_TLS_KEYEXCH_DH_DSS:
            case E_SEC_TLS_KEYEXCH_DH_RSA_EXPORT:
            case E_SEC_TLS_KEYEXCH_DH_RSA:
                break;
            case E_SEC_TLS_KEYEXCH_RSA_EXPORT:
                if(pstData->pstStrct->pstParam)
                 {
                    if(NULL != pstData->pstStrct->pstParam->stRSAParam.stTlsRSAParam.pucExponent)
                    {             
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stRSAParam.stTlsRSAParam.pucExponent);
                    }                
                    if(NULL != pstData->pstStrct->pstParam->stRSAParam.stTlsRSAParam.pucModulus )
                    {
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stRSAParam.stTlsRSAParam.pucModulus);
                    }
                    if(NULL != pstData->pstStrct->pstParam->stRSAParam.stTlsSignature.pucSignature )
                    {             
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stRSAParam.stTlsSignature.pucSignature );
                    }
                }
                break;
            case E_SEC_TLS_KEYEXCH_DH_ANON:
            case E_SEC_TLS_KEYEXCH_DH_ANON_EXPORT:             
                if(pstData->pstStrct->pstParam)
                {
                    if(NULL != pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHG )
                    {             
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHG);
                    }                
                    if(NULL != pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHYs)
                    {
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHYs);                
                    }
                    if(NULL != pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHP
                        && pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.usDHPLen > 0)
                    {              
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stDHParam.stTlsDHParam.pucDHP);                
                    }
                }
                break;
            default:
                break;
                }
               M_SEC_SELF_FREE(pstData->pstStrct->pucRandval);
              if(pstData->pstStrct->pstParam)
            {
                M_SEC_SELF_FREE(pstData->pstStrct->pstParam->stSecCert.pucBuf);
            }
            M_SEC_SELF_FREE(pstData->pstStrct->pstParam);
            M_SEC_SELF_FREE(pstData->pstStrct); 
            }

           
            break;
        }
        
        case E_SEC_DP_WTLS_PRF: 
        {
            St_SecDpWtlsPrf* pstData = NULL;

            pstData = (St_SecDpWtlsPrf*)pvPara;
            if(pstData->pstStrct)
            {
                if (((pstData->pstStrct->usSecretLen) > 0) && 
                    pstData->pstStrct->pucSecret)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucSecret);
                }    
                if(pstData->pstStrct->pcLabel)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pcLabel);
                }
                if(pstData->pstStrct->pucSeed)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucSeed);
                }
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        case E_SEC_DP_TLS_PRF: 
        {
            St_SecDpTlsPrf* pstData = NULL;

            pstData = (St_SecDpTlsPrf*)pvPara;
            if(pstData->pstStrct)
            {
                if (((pstData->pstStrct->usSecretLen) > 0) && 
                    pstData->pstStrct->pucSecret)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucSecret);
                }    
                if(NULL!=(pstData->pstStrct->pcLabel))
                {
                    WE_FREE(pstData->pstStrct->pcLabel);
                    pstData->pstStrct->pcLabel=NULL;
                }
                if(pstData->pstStrct->pucSeed)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucSeed);
                }
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        case E_SEC_DP_VERIFY_WTLS_CERT_CHAIN: 
        {
            St_SecDpVerifyWtlsCertChain* pstData = NULL;

            pstData = (St_SecDpVerifyWtlsCertChain*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucBuf)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucBuf);
                }    
                if (pstData->pstStrct->pucAddr)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucAddr);
                }    
                if (pstData->pstStrct->pucDomain)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucDomain);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        case E_SEC_DP_VERIFY_TLS_CERT_CHAIN: 
        {
            WE_INT32 iLoop=0;
            St_SecDpVerifyTlsCertChain* pstData = NULL;

            pstData = (St_SecDpVerifyTlsCertChain*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucAddr)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucAddr);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct->pucDomain);
                if(pstData->pstStrct->pstCerts)
                {
                    for(iLoop=0;iLoop<pstData->pstStrct->ucNumCerts;iLoop++)
                    {                        
                         M_SEC_SELF_FREE(pstData->pstStrct->pstCerts[iLoop].pucCert);
                    }
                   WE_FREE(pstData->pstStrct->pstCerts);
                   pstData->pstStrct->pstCerts =NULL;
                }
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_COMPUTE_WTLS_SIGN: 
        {
            St_SecDpComputeWtlsSign* pstData = NULL;

            pstData = (St_SecDpComputeWtlsSign*)pvPara;
            if(pstData->pstStrct)
            {
                if(pstData->pstStrct->pucKeyId)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucKeyId);
                }    
                if(pstData->pstStrct->pucBuf)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucBuf);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        case E_SEC_DP_COMPUTE_TLS_SIGN: 
        {
            St_SecDpComputeTlsSign* pstData = NULL;

            pstData = (St_SecDpComputeTlsSign*)pvPara;
            if(pstData->pstStrct)
            {
                if(pstData->pstStrct->pucPubKeyHash)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucPubKeyHash);
                }    
                if(pstData->pstStrct->pucData)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucData);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_GET_WTLS_CLIENT_CERT: 
        {
            St_SecDpGetWtlsClientCert* pstData = NULL;

            pstData = (St_SecDpGetWtlsClientCert*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucBuf)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucBuf);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;      
        }  
        case E_SEC_DP_GET_TLS_CLIENT_CERT: 
        {
            WE_INT32 iLoop=0;
            St_SecDpGetTlsClientCert* pstData = NULL;

            pstData = (St_SecDpGetTlsClientCert*)pvPara;
            if(pstData)
            {
                if(pstData->pstStrct)
                {
                    if (pstData->pstStrct->pstParam)
                    {
                        if(pstData->pstStrct->pstParam->pstCertAuthorities)
                        {
                            for(iLoop=0;iLoop<pstData->pstStrct->pstParam->ucNumCertAuthorities;iLoop++)
                            {
                                M_SEC_SELF_FREE(pstData->pstStrct->pstParam->pstCertAuthorities[iLoop].pucName);
                            }
                        }
                        
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->pucCertTypes);
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam->pstCertAuthorities);
                        M_SEC_SELF_FREE(pstData->pstStrct->pstParam);
                    }    
                    M_SEC_SELF_FREE(pstData->pstStrct);
                }
            }
            break;      
        } 
        
        case E_SEC_DP_RM_PEER_LINKS:
        {
            St_SecDpRmPeerLinks* pstData = NULL;

            pstData = (St_SecDpRmPeerLinks*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_LINK_PEER_SESSION: 
        {
            St_SecDpPeerLinkSession* pstData = NULL;

            pstData = (St_SecDpPeerLinkSession*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucAddress)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucAddress);
                }    
                if (pstData->pstStrct->pucDomain)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucDomain);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_LOOKUP_PEER: 
        {
            St_SecDpLookupPeer* pstData = NULL;

            pstData = (St_SecDpLookupPeer*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucAddress)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucAddress);
                }    
                if (pstData->pstStrct->pucDomain)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucDomain);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_ACTIVE_SESSION: 
        {
            St_SecDpActiveSession* pstData = NULL;

            pstData = (St_SecDpActiveSession*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }    
        
        case E_SEC_DP_INVALID_SESSION: 
        {
            St_SecDpInvalidSession* pstData = NULL;

            pstData = (St_SecDpInvalidSession*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        
        case E_SEC_DP_FETCH_SESSION: 
        {
            St_SecDpfetchSession* pstData = NULL;

            pstData = (St_SecDpfetchSession*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        
        case E_SEC_DP_UPDATE_SESSION: 
        {
            St_SecDpUpdateSession* pstData = NULL;

            pstData = (St_SecDpUpdateSession*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucSessionId)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucSessionId);
                }    
                if (pstData->pstStrct->pucPrivateKeyId)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucPrivateKeyId);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_STORE_CA_CERT:
        case E_SEC_DP_STORE_USER_CERT: 
        {
            St_SecDpStoreCert* pstData = NULL;

            pstData = (St_SecDpStoreCert*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucCertURL)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucCertURL);
                }    
                if (pstData->pstStrct->pcContentType)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pcContentType);
                }    
                if (pstData->pstStrct->pucCert)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucCert);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_VRRIFY_SIGN:
        case E_SEC_DP_HASH_VERIFICATION:   
        {
            St_SecDpCaInfo* pstData = NULL;

            pstData = (St_SecDpCaInfo*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucTrustedCAInfoStruct)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucTrustedCAInfoStruct);
                }    
                if (pstData->pstStrct->pcContentType)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pcContentType);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;   
        }
        
        case E_SEC_DP_VERIFY_CERT: 
        {
            St_SecDpDeliveryCert* pstData = NULL;

            pstData = (St_SecDpDeliveryCert*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucCertResponse)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucCertResponse);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_VIEW_CERT_NAMES: 
        {
            St_SecDpViewCertNames* pstData = NULL;

            pstData = (St_SecDpViewCertNames*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;   
        }

        case E_SEC_DP_GEN_KEY_PAIR: 
        {
            St_SecDpGenKeyPair* pstData = NULL;

            pstData = (St_SecDpGenKeyPair*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        
        case E_SEC_DP_RM_KEYS_AND_CERTS: 
        {
            St_SecDpRmKeysAndUserCerts* pstData = NULL;

            pstData = (St_SecDpRmKeysAndUserCerts*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }
        
        case E_SEC_DP_GET_USER_PUB_KEY: 
        {
            St_SecDpGetUserPubKey* pstData = NULL;

            pstData = (St_SecDpGetUserPubKey*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;    
        }
        
        case E_SEC_DP_CHANGE_PIN: 
        {
            St_SecDpChangePin* pstData = NULL;

            pstData = (St_SecDpChangePin*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        } 

        case E_SEC_DP_VIEW_CUR_CERT: 
        {
            St_SecDpViewCurCert* pstData = NULL;

            pstData = (St_SecDpViewCurCert*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        
        case E_SEC_DP_VIEW_SESSION: 
        {
            St_SecDpViewSessionInfo* pstData = NULL;

            pstData = (St_SecDpViewSessionInfo*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        
        case E_SEC_DP_GET_CUR_SEC_CLASS: 
        {
            St_SecDpGetCurSecClass* pstData = NULL;

            pstData = (St_SecDpGetCurSecClass*)pvPara;
            
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }

        
        case E_SEC_DP_CLEANUP_SESSION: 
        {
            St_SecDpCleanUpSession* pstData = NULL;

            pstData = (St_SecDpCleanUpSession*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break; 
        }
        
        case E_SEC_DP_VIEW_CONTRACTS: 
        {
            St_SecDpViewContracts* pstData = NULL;

            pstData = (St_SecDpViewContracts*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break; 
        }
        
        case E_SEC_DP_GET_CONTRACTS: 
        {
            St_SecDpGetContracts* pstData = NULL;

            pstData = (St_SecDpGetContracts*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;
        }
        
        case E_SEC_DP_RM_CONTRACT: 
        {
            St_SecDpRmContract* pstData = NULL;

            pstData = (St_SecDpRmContract*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break;  
        }
        
        case E_SEC_DP_RM_CNTRACT_RESP: 
        {
            St_SecDpRmContract* pstData = NULL;

            pstData = (St_SecDpRmContract*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }    

        case E_SEC_DP_GET_CERT: 
        {
            St_SecDpGetCert* pstData = NULL;

            pstData = (St_SecDpGetCert*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break; 
        }
        
        case E_SEC_DP_GETCERT_RESP: 
        {
            St_SecDpGetCertResp* pstData = NULL;

            pstData = (St_SecDpGetCertResp*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucCert)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucCert);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }   
        
        case E_SEC_DP_RM_CERT: 
        {
            St_SecDpRmCert* pstData = NULL;

            pstData = (St_SecDpRmCert*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break; 
        }
        
        case E_SEC_DP_RMCERT_RESP: 
        {
            St_SecDpRmCertResp* pstData = NULL;

            pstData = (St_SecDpRmCertResp*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break; 
        }    

        case E_SEC_DP_MIME_RESP: 
        {
            St_SecDpMimeResp* pstData = NULL;

            pstData = (St_SecDpMimeResp*)pvPara;
            if(pstData->pstStrct)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break; 
        }    

        case E_SEC_DP_UI_SIGN_TEXT : 
        {
            St_SecDpSignText* pstData = NULL;

            pstData = (St_SecDpSignText*)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pcText)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pcText);
                }    
                if (pstData->pstStrct->pcData)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pcData);
                }    
                if (pstData->pstStrct->pcKeyId)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pcKeyId);
                }    
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }    

        case E_SEC_DP_GET_REQ_USER_CERT : 
        {
            St_SecDpUserCertReq* pstData = NULL;

            pstData = (St_SecDpUserCertReq *)pvPara;
            if(pstData->pstStrct)
            {
                if (pstData->pstStrct->pucCountry)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucCountry);
                }    
                if (pstData->pstStrct->pucProvince)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucProvince);
                }    
                if (pstData->pstStrct->pucCity)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucCity);
                }      
                if (pstData->pstStrct->pucCompany)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucCompany);
                }     
                if (pstData->pstStrct->pucDepartment)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucDepartment);
                }   
                if (pstData->pstStrct->pucName)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucName);
                }   
                if (pstData->pstStrct->pucEMail)
                {
                    M_SEC_SELF_FREE(pstData->pstStrct->pucEMail);
                }   
                M_SEC_SELF_FREE(pstData->pstStrct);
            }
            break;
        }  
        
        case E_SEC_DP_CHANGE_WTLS_CERT_ABLE: 
        {
            St_SecDpChangeWtlsCertAble* pstData = NULL;

            pstData = (St_SecDpChangeWtlsCertAble*)pvPara;
            if (pstData->pstStrct != NULL)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
            break; 
        }    
        case E_SEC_DP_SHOWCONTRACTLIST:
        {
            St_SecDpShowContractList* pstData = NULL;
            pstData = (St_SecDpShowContractList*)pvPara;
            if (pstData->pstStrct != NULL)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
        }
        break;
        case E_SEC_DP_SHOWCONTRACTCONTENT:
        {
            St_SecDpShowContractContent* pstData = NULL;
            pstData = (St_SecDpShowContractContent*)pvPara;
            if (pstData->pstStrct != NULL)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
        }
        break;
        case E_SEC_DP_SHOWCERTTLIST:
        {
            St_SecDpShowCertList* pstData = NULL;
            pstData = (St_SecDpShowCertList*)pvPara;
            if (pstData->pstStrct != NULL)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
        }
        break;
        case E_SEC_DP_SHOWCERTCONTENT:
        {
            St_SecDpShowCertContent* pstData = NULL;
            pstData = (St_SecDpShowCertContent*)pvPara;
            if (pstData->pstStrct != NULL)
            {
                M_SEC_SELF_FREE(pstData->pstStrct);
            }    
        }
        break;
        default:
        {
            break;
        }    
    }
    M_SEC_SELF_FREE(pvPara);
    
    return;
}


/*==================================================================================================
FUNCTION: 
    Sec_TlHandleConvertStr4ToUint16
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge whether the big endian or little endian, evaluate and get 16 bit value.
ARGUMENTS PASSED: 
    pucStr : 8-bit pointer 
    pusUint : 16-bit pointer 
RETURN VALUE:
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_TlHandleConvertStr4ToUint16( const WE_UINT8* pucStr, WE_UINT16* pusUint)
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr) || (NULL == pusUint))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {
        *((WE_UINT8 *)pusUint) = pucStr[0];
        *(((WE_UINT8 *)pusUint)+1) = pucStr[1];
    }
    else                        
    {
        *((WE_UINT8 *)pusUint) = pucStr[1];
        *(((WE_UINT8 *)pusUint)+1) = pucStr[0];
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_TlHandleConvertStr4ToUint32
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge whether the big endian or little endian, evaluate and get 32 bit value.
ARGUMENTS PASSED: 
    pucStr : 8-bit pointer 
    puiUint : 32-bit pointer 
RETURN VALUE:
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_TlHandleConvertStr4ToUint32( const WE_UINT8* pucStr, WE_UINT32* puiUint )
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr) || (NULL == puiUint))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {
        *((WE_UINT8 *)puiUint) = pucStr[0];
        *(((WE_UINT8 *)puiUint)+1) = pucStr[1];
        *(((WE_UINT8 *)puiUint)+2) = pucStr[2];
        *(((WE_UINT8 *)puiUint)+3) = pucStr[3];
    }
    else                        
    {
        *((WE_UINT8 *)puiUint) = pucStr[3];
        *(((WE_UINT8 *)puiUint)+1) = pucStr[2];
        *(((WE_UINT8 *)puiUint)+2) = pucStr[1];
        *(((WE_UINT8 *)puiUint)+3) = pucStr[0];
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_TlHandleConvertUint16ToStr4
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge whether the big endian or little endian, put the 16 bit value to 8 bit.
ARGUMENTS PASSED: 
    pusUint : 16-bit pointer 
    pucStr : 8-bit pointer 
RETURN VALUE:
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_TlHandleConvertUint16ToStr4( WE_UINT16* pusUint, WE_UINT8* pucStr )
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr) || (NULL == pusUint))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {
        pucStr[0] = *((WE_UINT8 *)pusUint);
        pucStr[1] = *(((WE_UINT8 *)pusUint)+1);
    }
    else                        
    {
        pucStr[1] = *((WE_UINT8 *)pusUint);
        pucStr[0] = *(((WE_UINT8 *)pusUint)+1);
    }

    return;
}

/*==================================================================================================
FUNCTION: 
    Sec_TlHandleConvertUint32ToStr4
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge whether the big endian or little endian, put the 32 bit value to 8 bit.
ARGUMENTS PASSED: 
    puiUint : 32-bit pointer 
    pucStr : 8-bit pointer 
RETURN VALUE:
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_TlHandleConvertUint32ToStr4( WE_UINT32* puiUint, WE_UINT8* pucStr )
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr) || (NULL == puiUint))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {
        pucStr[0] = *((WE_UINT8 *)puiUint);
        pucStr[1] = *(((WE_UINT8 *)puiUint)+1);
        pucStr[2] = *(((WE_UINT8 *)puiUint)+2);
        pucStr[3] = *(((WE_UINT8 *)puiUint)+3);
    }
    else                        
    {
        pucStr[3] = *((WE_UINT8 *)puiUint);
        pucStr[2] = *(((WE_UINT8 *)puiUint)+1);
        pucStr[1] = *(((WE_UINT8 *)puiUint)+2);
        pucStr[0] = *(((WE_UINT8 *)puiUint)+3);
    }

    return;
}

/*=====================================================================================
FUNCTION: 
	  Sec_FreeMsg
CREATE DATE: 
	  2006-07-04
AUTHOR: 
	  Sam
DESCRIPTION:
      the function is responsible for memory free after pkc response message is used.
ARGUMENTS PASSED:
	   ucID[IN]:the value of the id,no use.
	   usMsg[IN]:Value of the usMsg.
       pDatap[IN]:Pointer of data.
RETURN VALUE:
	  None
USED GLOBAL VARIABLES:
      None
USED STATIC VARIABLES:
      None
CALL BY:
    
IMPORTANT NOTES:
	  Describe anything that help understanding the function and important aspect of
	  using the function i.e side effect..etc
=====================================================================================*/
void Sec_FreeMsg (WE_UINT8 ucID, WE_INT32 iMsg, void* pvData)
{
    if (pvData == NULL)
    {
        return;
    }
    ucID = ucID;

    switch (iMsg)
    {
        case M_SEC_DP_MSG_ENCRYPT_PKC_RESP:
        {
            St_SecCrptEncryptPkcResp  *pstEncryptPkcResp = (St_SecCrptEncryptPkcResp *)pvData;
            if (pstEncryptPkcResp->pucBuf != NULL)
            {
                WE_FREE(pstEncryptPkcResp->pucBuf);
                pstEncryptPkcResp->pucBuf = NULL;
            }
            break;
        }

        case M_SEC_DP_MSG_DECRYPT_PKC_RESP:
        {
            St_SecCrptDecryptPkcResp* pst = (St_SecCrptDecryptPkcResp*)pvData;
            if (pst->pucBuf != NULL)
            {
                WE_FREE(pst->pucBuf);
                pst->pucBuf = NULL;
            }
        }
        break;
        case M_SEC_DP_MSG_VERIFY_SIGN_RESP:
        {
            ucID = ucID;
            break;
        }

        case M_SEC_DP_MSG_COMPUTE_SIGN_RESP:
        {
            St_SecCrptComputeSignatureResp* pstComputeSignResp = (St_SecCrptComputeSignatureResp *)pvData;
            if (pstComputeSignResp->pucSig != NULL)
            {
                WE_FREE(pstComputeSignResp->pucSig);
                pstComputeSignResp->pucSig = NULL;
            }
            break;
        }

        case M_SEC_DP_MSG_GEN_KEYPAIR_RESP:
        {
            St_SecCrptGenKeypairResp* pstGenerateKeypairResp = (St_SecCrptGenKeypairResp *)pvData;
            if (pstGenerateKeypairResp->stPrivKey.pucBuf != NULL)
            {
                WE_FREE(pstGenerateKeypairResp->stPrivKey.pucBuf);
                pstGenerateKeypairResp->stPrivKey.pucBuf = NULL;
            }
            if (pstGenerateKeypairResp->stPubKey.pucExponent != NULL)
            {
                WE_FREE(pstGenerateKeypairResp->stPubKey.pucExponent);
                pstGenerateKeypairResp->stPubKey.pucExponent = NULL;
            }
            if (pstGenerateKeypairResp->stPubKey.pucModulus != NULL)
            {
                WE_FREE(pstGenerateKeypairResp->stPubKey.pucModulus);
                pstGenerateKeypairResp->stPubKey.pucModulus = NULL;
            }
            break;
        }
        
        case M_SEC_DP_MSG_EXECUTE_COMMAND:
        {
            St_SecCertContent *pst = (St_SecCertContent*)pvData;
            if(NULL != pst->pcMime)
            {
                WE_FREE(pst->pcMime);
                pst->pcMime = NULL;  
            }
            if(NULL != pst->pucCertData)
            {
                WE_FREE(pst->pucCertData);
                pst->pucCertData = NULL;
            }
        }
        break;
        case M_SEC_DP_MSG_REQ_USER_CERT:
        {
            St_SecUserCertReqInfo *pstUserCertReqInfo = (St_SecUserCertReqInfo*)pvData;
           if (pstUserCertReqInfo->pucName != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucName);
               pstUserCertReqInfo->pucName = NULL;
           }
           if (pstUserCertReqInfo->pucEMail != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucEMail);
               pstUserCertReqInfo->pucEMail = NULL;
           }
           if (pstUserCertReqInfo->pucDepartment != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucDepartment);
               pstUserCertReqInfo->pucDepartment = NULL;
           }
           if (pstUserCertReqInfo->pucCompany != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCompany);
           }
           if (pstUserCertReqInfo->pucCity != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCity);
               pstUserCertReqInfo->pucCity = NULL;
           }
           if (pstUserCertReqInfo->pucProvince != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucProvince);
               pstUserCertReqInfo->pucProvince = NULL;
           }
           if(pstUserCertReqInfo->pucCountry != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCountry);
               pstUserCertReqInfo->pucCountry = NULL;
           }
        }
        break;
        case M_SEC_DP_MSG_STORE_CERT:
        {
            St_SecSaveCert * pstStoreCert = (St_SecSaveCert*)pvData;
            if(NULL != pstStoreCert->pcContentType)
            {
                WE_FREE(pstStoreCert->pcContentType);
                pstStoreCert->pcContentType  = NULL;
            }
            if(NULL != pstStoreCert->pucCert)
            {
                WE_FREE(pstStoreCert->pucCert);
                pstStoreCert->pucCert = NULL;
            }
            if(NULL != pstStoreCert->pucCertURL)
            {
                WE_FREE(pstStoreCert->pucCertURL);
                pstStoreCert->pucCertURL = NULL;
            }
        }
        break;
        case M_SEC_DP_MSG_STORE_CERT_RESP:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_TRUSTED_CA_INFO:
        {
            St_SecTrustedCaInfo* pstTrustedCaInfo = (St_SecTrustedCaInfo*)pvData;
            if(NULL != pstTrustedCaInfo->pcContentType)
            {
                WE_FREE(pstTrustedCaInfo->pcContentType);
                pstTrustedCaInfo->pcContentType = NULL;
            }
            if(NULL != pstTrustedCaInfo->pucTrustedCAInfoStruct)
            {
                WE_FREE(pstTrustedCaInfo->pucTrustedCAInfoStruct);
                pstTrustedCaInfo->pucTrustedCAInfoStruct = NULL;
            }
        }
        break;
        case M_SEC_DP_MSG_TRUSTED_CA_INFO_RESP:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_DELIVERY_CERT:
        {
            St_SecCertDelivery* pstCertDelivery = (St_SecCertDelivery*)pvData;
            if(NULL != pstCertDelivery->pucCertResponse)
            {
                WE_FREE(pstCertDelivery->pucCertResponse);
                pstCertDelivery->pucCertResponse = NULL;
            }
        }
        break;
        case M_SEC_DP_MSG_DELVERY_CERT_RESP:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_ESTABLISH_CONN:
        {
            St_SecSetupConnection* pst = (St_SecSetupConnection*)pvData;
            if(NULL != pst->stSessionInfo.pstCerts)
            {
                M_SEC_SELF_FREE(pst->stSessionInfo.pstCerts->pucCert);
                M_SEC_SELF_FREE(pst->stSessionInfo.pstCerts);
            }
        }
        break;
        case M_SEC_DP_MSG_TERMINATE_CONN:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_RM_PEER_LINKS:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_LINK_PEER_SESSION:
        {
            St_SecAttachPeer* pst = (St_SecAttachPeer*)pvData;
            M_SEC_SELF_FREE(pst->pucAddress);
        }
        break;
        case M_SEC_DP_MSG_LOOKUP_PEER:
        {
            St_SecSearchPeer* pst = (St_SecSearchPeer*)pvData;
            M_SEC_SELF_FREE(pst->pucAddress);
        }
        break;
        case M_SEC_DP_MSG_ACTIVE_SESSION:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_INVALID_SESSION:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_FETCH_SESSION:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_UPDATE_SESSION:
        {
            St_SecUpdateSession* pst = (St_SecUpdateSession*)pvData;
            M_SEC_SELF_FREE(pst->pucPrivateKeyId);
            M_SEC_SELF_FREE(pst->pucSessionId);
        }
        break;
        
        case M_SEC_DP_MSG_VIEW_CERT_NAMES:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_VIEW_CUR_CERT:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_VIEW_SESSION:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_GET_CUR_SEC_CLASS:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_CLEANUP_SESSION:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_GET_CERT:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_RM_CERT:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_GET_CIPHERSUITE:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_GET_TLS_CIPHERSUITE:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_EXCHANGE_WTLS_KEY:
         {
            St_SecWtlsKeyExchange * pst = (St_SecWtlsKeyExchange*)pvData;
            if (pst->pucRandval)
            {
                M_SEC_SELF_FREE(pst->pucRandval);
            }
            if(pst->pstParam)
            {
                if ((E_SEC_WTLS_KEYEXCHANGE_RSA_ANON == pst->pstParam->ucKeyExchangeSuite) ||
                    (E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512 == pst->pstParam->ucKeyExchangeSuite) ||
                    (E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768 == pst->pstParam->ucKeyExchangeSuite))
                {
                    if ((pst->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen) > 0)
                    {
                        M_SEC_SELF_FREE(pst->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
                    }    
                    if ((pst->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen) > 0)
                    {
                        M_SEC_SELF_FREE(pst->pstParam->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus);
                    }    
                    if (255 == pst->pstParam->stParams.stKeyParam.stParamSpecifier.ucParameterIndex)                    
                    {
                        if ((pst->pstParam->stParams.stKeyParam.stParamSpecifier.usParamLen) > 0)
                        {
                            M_SEC_SELF_FREE(pst->pstParam->stParams.stKeyParam.stParamSpecifier.pucParams);
                        }    
                    }
                }
                else if ((E_SEC_WTLS_KEYEXCHANGE_RSA == pst->pstParam->ucKeyExchangeSuite) ||
                    (E_SEC_WTLS_KEYEXCHANGE_RSA_512 == pst->pstParam->ucKeyExchangeSuite) ||
                    (E_SEC_WTLS_KEYEXCHANGE_RSA_768 == pst->pstParam->ucKeyExchangeSuite))
                {
                    if ((pst->pstParam->stParams.stCertificates.usBufLen) > 0)
                    {
                        M_SEC_SELF_FREE(pst->pstParam->stParams.stCertificates.pucBuf);
                    }    
                }
            }
            M_SEC_SELF_FREE(pst->pstParam);
        }
        break;
        case M_SEC_DP_MSG_EXCHANGE_TLS_KEY:
        {
            St_SecTlsKeyExchange     * pst = (St_SecTlsKeyExchange*)pvData;
            if(pst->pucRandval)
            {
                M_SEC_SELF_FREE(pst->pucRandval);
            }
            switch(pst->ucHashAlg)
            {
            case E_SEC_TLS_KEYEXCH_DHE_DSS: /* sec_tls_server_dh_params */
            case E_SEC_TLS_KEYEXCH_DHE_DSS_EXPORT:
            case E_SEC_TLS_KEYEXCH_DHE_RSA:
            case E_SEC_TLS_KEYEXCH_DHE_RSA_EXPORT:                
                if(pst->pstParam)
                 {              
                        if(NULL != pst->pstParam->stDHParam.stTlsDHParam.pucDHG )
                        {                   
                            M_SEC_SELF_FREE(pst->pstParam->stDHParam.stTlsDHParam.pucDHG);
                        }
                        
                        if(NULL != pst->pstParam->stDHParam.stTlsDHParam.pucDHYs)                   
                        {
                            M_SEC_SELF_FREE(pst->pstParam->stDHParam.stTlsDHParam.pucDHYs);             
                        }
                        if(NULL != pst->pstParam->stDHParam.stTlsDHParam.pucDHP)
                        {
                            M_SEC_SELF_FREE(pst->pstParam->stDHParam.stTlsDHParam.pucDHP);              
                        }
                        if(NULL != pst->pstParam->stDHParam.stTlsSignature.pucSignature)
                        {
                            M_SEC_SELF_FREE(pst->pstParam->stDHParam.stTlsSignature.pucSignature);
                        }
                }
                break;
            case E_SEC_TLS_KEYEXCH_RSA:/* When RSA is used */
            case E_SEC_TLS_KEYEXCH_DH_DSS_EXPORT:
            case E_SEC_TLS_KEYEXCH_DH_DSS:
            case E_SEC_TLS_KEYEXCH_DH_RSA_EXPORT:
            case E_SEC_TLS_KEYEXCH_DH_RSA:
                break;
            case E_SEC_TLS_KEYEXCH_RSA_EXPORT:
                if(pst->pstParam)
                {
                    if(NULL != pst->pstParam->stRSAParam.stTlsRSAParam.pucExponent)
                    {             
                        M_SEC_SELF_FREE(pst->pstParam->stRSAParam.stTlsRSAParam.pucExponent);
                    }                
                    if(NULL != pst->pstParam->stRSAParam.stTlsRSAParam.pucModulus )
                    {
                        M_SEC_SELF_FREE(pst->pstParam->stRSAParam.stTlsRSAParam.pucModulus);
                    }
                    if(NULL != pst->pstParam->stRSAParam.stTlsSignature.pucSignature )
                    {             
                        M_SEC_SELF_FREE(pst->pstParam->stRSAParam.stTlsSignature.pucSignature );
                    }
                }
                break;
            case E_SEC_TLS_KEYEXCH_DH_ANON:
            case E_SEC_TLS_KEYEXCH_DH_ANON_EXPORT:     
                if(pst->pstParam)
                {
                    if(NULL != pst->pstParam->stDHParam.stTlsDHParam.pucDHG )
                    {             
                        M_SEC_SELF_FREE(pst->pstParam->stDHParam.stTlsDHParam.pucDHG);
                    }                
                    if(NULL != pst->pstParam->stDHParam.stTlsDHParam.pucDHYs)
                    {
                        M_SEC_SELF_FREE(pst->pstParam->stDHParam.stTlsDHParam.pucDHYs);                
                    }
                    if(NULL != pst->pstParam->stDHParam.stTlsDHParam.pucDHP
                        && pst->pstParam->stDHParam.stTlsDHParam.usDHPLen > 0)
                    {              
                        M_SEC_SELF_FREE(pst->pstParam->stDHParam.stTlsDHParam.pucDHP);                
                    }
                }
                break;
            default:
                break;
                }
        }
        break;
        case M_SEC_DP_MSG_WTLS_PRF:
        {
            St_SecGetPrfResult* pst = (St_SecGetPrfResult*)pvData;
            M_SEC_SELF_FREE(pst->pucSecret);
            M_SEC_SELF_FREE(pst->pcLabel);
            M_SEC_SELF_FREE(pst->pucSeed);
        }
        break;
        case M_SEC_DP_MSG_TLS_PRF:
        {
            St_SecGetPrfResult* pst = (St_SecGetPrfResult*)pvData;
            M_SEC_SELF_FREE(pst->pucSecret);
            M_SEC_SELF_FREE(pst->pcLabel);
            M_SEC_SELF_FREE(pst->pucSeed);
        }
        break;
        case M_SEC_DP_MSG_VERIFY_WTLS_CERT_CHAIN:
        {
            St_SecWtlsVerifySvrCert* pst = (St_SecWtlsVerifySvrCert*)pvData;
            M_SEC_SELF_FREE(pst->pucBuf);
            M_SEC_SELF_FREE(pst->pucAddr);
            
        }
        break;
        case M_SEC_DP_MSG_VERIFY_TLS_CERT_CHAIN:
        {
            WE_INT32 iIndex=0;
            St_SecTlsVerifySvrCert* pst = (St_SecTlsVerifySvrCert*)pvData;
            M_SEC_SELF_FREE(pst->pucDomain);
            M_SEC_SELF_FREE(pst->pucAddr);
            for(iIndex=0;iIndex<pst->ucNumCerts;iIndex++)
            {
                if(pst->pstCerts)
                {
                    M_SEC_SELF_FREE(pst->pstCerts[iIndex].pucCert);
                }
            }              
            M_SEC_SELF_FREE(pst->pstCerts);
        }
        break;
        case M_SEC_DP_MSG_GET_WTLS_CLIENT_CERT:
        {
            St_SecWtlsGetUsrCert* pst = (St_SecWtlsGetUsrCert*)pvData;
            M_SEC_SELF_FREE(pst->pucBuf);
        }
        break;
        case M_SEC_DP_MSG_GET_TLS_CLIENT_CERT:
        {
            WE_INT32 iLoop=0;
            St_SecTlsGetUsrCert* pst = (St_SecTlsGetUsrCert*)pvData;
            if(pst)
            {	
                if(pst->pstParam)
                {
                    M_SEC_SELF_FREE( pst->pstParam->pucCertTypes );  
                    if(pst->pstParam->pstCertAuthorities)
                    {
                        for(iLoop=0;iLoop<pst->pstParam->ucNumCertAuthorities;iLoop++)
                        {
                            M_SEC_SELF_FREE(pst->pstParam->pstCertAuthorities[iLoop].pucName);
                        }            
                        M_SEC_SELF_FREE(pst->pstParam->pstCertAuthorities);            
                    }
                    M_SEC_SELF_FREE(pst->pstParam);
                }
            }
        }
        break;
        case M_SEC_DP_MSG_COMPUTE_WTLS_SIGN:
        {
            St_SecWtlsCompSign* pst = (St_SecWtlsCompSign*)pvData;
            M_SEC_SELF_FREE(pst->pucKeyId);
            M_SEC_SELF_FREE(pst->pucBuf);
            
        }
        case M_SEC_DP_MSG_COMPUTE_TLS_SIGN:
        {
            St_SecTlsComputeSign* pst = (St_SecTlsComputeSign*)pvData;
            M_SEC_SELF_FREE(pst->pucPubKeyHash);
            M_SEC_SELF_FREE(pst->pucData);            
        }
        break;

        case M_SEC_DP_MSG_SIGN_TEXT:
        {
            St_SecSignText* pst = (St_SecSignText*)pvData;
            M_SEC_SELF_FREE(pst->pcText);
            M_SEC_SELF_FREE(pst->pcData);
            M_SEC_SELF_FREE(pst->pcKeyId);
        }
        break;
        case M_SEC_DP_MSG_VIEW_CONTRACTS:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_RM_CONTRACT:
        {
            ucID = ucID;
        }
        break;
        
        case M_SEC_DP_MSG_GEN_KEY_PAIR:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_GET_USER_PUB_KEY:
        {
            ucID = ucID;
        }
        break;
        case M_SEC_DP_MSG_CHANGE_PIN:
        {
            ucID = ucID;
        }
        break;
        /*action function asynchronism*/
        case M_SEC_DP_MSG_SHOWCONTRACTLIST:
        case M_SEC_DP_MSG_SHOWCONTRACTCONTENT:
        case M_SEC_DP_MSG_SHOWCERTLIST:
        case M_SEC_DP_MSG_SHOWCERTCONTENT:
        case M_SEC_DP_MSG_CHANGE_WTLS_CERT_ABLE:
        {
            ucID = ucID;
        }
            
        break;
        default:
            break;
    }
    WE_FREE(pvData);

}





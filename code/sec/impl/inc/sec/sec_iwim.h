/*==================================================================================================
    HEADER NAME : sec_iwim.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file, define the initial function prototype for WIM module, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-19    Stone An         None         create sec_iwim.h
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef SEC_IWIM_H
#define SEC_IWIM_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define M_SEC_KEY_HASH_SIZE       20
#define M_SEC_SP_RSA              M_SEC_WTLS_RSA_SIGN_CERT

#define M_SEC_ERR_WIM_NOT_INITIALISED      101
#define M_SEC_ERR_IN_PROGRESS              102
#define M_SEC_ERR_OLD_ROOTCERT_FOUND       121

/*added by Bird 070209*/
#define M_SEC_HAVEREAD_KEYPAIRINFO    0x0001
#define M_SEC_HAVEREAD_SESSIONPEER    0x0002
#define M_SEC_HAVEREAD_VIEWINFO         0x0004

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
void Sec_iWimInit(WE_HANDLE hSecHandle, WE_INT32 iTargetID);
void Sec_iWimEnd(WE_HANDLE hSecHandle);
void Sec_iWimWtlsGetPrfResult(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                              WE_INT32 iMasterSecretId, const WE_UCHAR *pucSecret, 
                              WE_INT32 iSecretLen, const WE_CHAR *pcLabel,
                              const WE_UCHAR *pucSeed, WE_INT32 iSeedLen,
                              WE_INT32 iOutputLen, WE_UINT8 ucConnectionType,WE_UINT8 ucAlg);
void Sec_iWimGetFitedPrivKeyResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement,
                                 void *pvParam);
void Sec_iWimGetFitedPrivKey(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                             St_SecPubKeyRsa stWimUserPublicKey);
void Sec_iWimWtlsCalSign(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR *pucKeyId,
                         WE_INT32 iKeyIdLen, const WE_UCHAR *pucBuf, WE_INT32 iBufLen, 
                         WE_UINT8 ucAlg);
void Sec_iWimWtlsCalSignResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                             St_SecCrptComputeSignatureResp *pstParam);
void Sec_iWimVerifyCertInterResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                                 St_SecCrptVerifySignatureResp *pstParam);
void Sec_iWimVerifyCertRootResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement,
                                St_SecCrptVerifySignatureResp *pstParam);
void  Sec_iWimWtlsVerifySvrCertChain(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                     const WE_UCHAR *pucBuf, WE_INT32 iBufLen);
void Sec_iWimReadView(WE_HANDLE hSecHandle);
void Sec_iWimStoreView(WE_HANDLE hSecHandle, const WE_UCHAR *pucData, 
                       WE_INT32 iDataLen);
void Sec_iWimWtlsGetCipherSuite(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucServerAuth);
void Sec_iWimWtlsKeyExchange(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                             St_SecWtlsKeyExchParams stParam, const WE_UCHAR *pucRandVal,WE_UINT8 ucHashAlg);
void Sec_iWimWtlsKeyExRsaResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                              St_SecCrptEncryptPkcResp *pstParam);
void Sec_iWimDelPrivKeysAndUCertsByUsage(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                         WE_UINT8 ucKeyType);
void Sec_iWimKeyPairGen(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType,
                        const WE_CHAR *pcPin, WE_UINT8 ucAlg);
void Sec_iWimKeyPairGenResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                            St_SecCrptGenKeypairResp *pstParam);
void Sec_iWimKeyPairGenSignResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                                St_SecCrptComputeSignatureResp *pstParam);
void Sec_iWimViewPrivKeyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType);
void Sec_iWimVerifyPrivKeyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType,
                              const WE_CHAR *pcPin);
void Sec_iWimModifyPrivKeyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType, 
                              const WE_CHAR *pcOldPin, const WE_CHAR *pcNewPin);
void Sec_iWimViewNameOfUserCert(WE_HANDLE hSecHandle, const WE_UCHAR *pucBuf,
                                WE_INT32 iBufLen);
void Sec_iWimViewNameOfUserCertByUsage(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType);
void Sec_iWimGetKeyIdAndUCert(WE_HANDLE hSecHandle, WE_INT32 iCertId, 
                              WE_UINT8 ucConnectionType);
void Sec_iWimGetUserCertAndPrivKey(WE_HANDLE hSecHandle, WE_INT32 iCertId);
void Sec_iWimGetUserCertAndPrivKeyResp (WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                                        St_SecCrptComputeSignatureResp *pstParam);
void Sec_iWimFindMatchedSignText(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                 WE_INT32 iSignId, WE_INT32 iPubKeyType, const WE_CHAR *pcPubKeyHash,
                                 WE_INT32 iPubKeyHashLen, WE_INT32 iOptions);
void Sec_iWimSignText(WE_HANDLE hSecHandle, WE_INT32 iObjectId, WE_INT32 iSignId, 
                      const WE_CHAR *pcData, WE_INT32 iDataLen, WE_INT32 iCertId, 
                      WE_INT32 iOptions);
void Sec_iWimSignTextResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                          St_SecCrptComputeSignatureResp *pstParam);
void Sec_iWimViewContracts(WE_HANDLE hSecHandle, WE_INT32 iTargetID);
void Sec_iWimDelContract(WE_HANDLE hSecHandle, WE_INT32 iContractId);
void Sec_iWimStoreContract(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_CHAR *pcContract,
                           const WE_CHAR *pcSig, WE_UINT16 usSigLen, WE_UINT32 uiTime);
void Sec_iWimWtlsChkUCertChain(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                               const WE_UCHAR *pucBuf, WE_INT32 iBufLen);
void Sec_iWimChkCACert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR *pucCert,
                       WE_UINT16 usCertLen);
void Sec_iWimChkCACertResp (WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                            St_SecCrptVerifySignatureResp *pstParam);
void Sec_iWimSelfSignedCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR *pucCert);
void Sec_iWimSaveCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR *pucCert, 
                      WE_UINT16 usCertLen, WE_CHAR cCertType, void *pvCertFriendlyName,
                      WE_UINT16 usFriendlyNameLen, WE_INT16 sCharSet,
                      const WE_UCHAR *pucCertURL);
void Sec_iWimCertExists(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR *pucCert,
                        WE_UINT16 usCertLen, WE_CHAR cCertType);
void Sec_iWimGetCert(WE_HANDLE hSecHandle, WE_INT32 iCertId);
void Sec_iWimDelCert(WE_HANDLE hSecHandle, WE_INT32 iCertId);
void Sec_iWimViewNameOfCert(WE_HANDLE hSecHandle, WE_INT32 iCertOptions);
void Sec_iWimViewIdOfCert(WE_HANDLE hSecHandle, WE_INT32 iCertOptions);
void Sec_iWimStoreUCertKeyPairInfo(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iUserCertId,
                                   WE_INT32 iUserCertCount, WE_INT32 iPrivKeyId, WE_INT32 iPrivKeyCount, 
                                   WE_UINT8 ucPrivKeyUsage, WE_UINT8 ucAlg, const WE_UCHAR *pucPublicKeyHash, 
                                   const WE_UCHAR *pucCaPublicKeyHash, WE_INT32 iURLCount, 
                                   WE_UINT8 ucPublicKeyCert, WE_INT32 iNameCount, WE_INT8 cNameType);
void Sec_iWimVerifySign(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                        const WE_UCHAR *pucTrustedCAInfo, 
                        WE_UINT16 usTrustedCAInfoLen);
void Sec_iWimVerifySignResp (WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                             St_SecCrptVerifySignatureResp *pstParam);
void Sec_iWimUserCertReq(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
            WE_UCHAR ucCertUsage, WE_UCHAR *pucCountry,WE_UINT16 usCountryLen, 
            WE_UCHAR *pucProvince, WE_UINT16 usProvinceLen, WE_UCHAR *pucCity, 
            WE_UINT16 usCityLen, WE_UCHAR *pucCompany, WE_UINT16 usCompanyLen, 
            WE_UCHAR *pucDepartment, WE_UINT16 usDepartmentLen,WE_UCHAR *pucName, 
            WE_UINT16 usNameLen,WE_UCHAR *pucEMail, WE_UINT16 usEMailLen, 
            WE_UCHAR ucKeyUsage, WE_UCHAR ucSignType);
WE_VOID Sec_iWimUserCertReqResp(WE_HANDLE hSecHandle, 
            St_SecWimElement *pstElement, St_SecCrptComputeSignatureResp *pstParam);
WE_VOID Sec_iWimChangeWTLSCertAble(WE_HANDLE hSecHandle, 
                                    WE_INT32 iTargetID, WE_UINT32 uiCertId);

void Sec_WimFreeRsaElement(WE_HANDLE hSecHandle, St_SecWimElement* pstElement);

#endif/*endif SEC_IWIM_H*/


/*--------------------------END-----------------------------*/

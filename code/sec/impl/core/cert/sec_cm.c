/*=====================================================================================
    MODULE NAME : sec_cm.c
    MODULE NAME : SEC


    GENERAL DESCRIPTION
        The functions in this file manage the certificate(x509 or wtls).

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History

    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-07-08        wuxl         None          Init

=====================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "sec_comm.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
/*Define Constant Macro start*/
#define USER_CANCEL                200
#define MD2_WITH_RSA_ENCRYP        2
#define MD5_WITH_RSA_ENCRYP        4
#define SHA1_WITH_RSA_ENCRYP       5
#define STORE_CA_CERT_STATE_MAX    18
#define STORE_USER_CERT_STATE_MAX  16
#define HASH_VERIFY_STATE_MAX      13
#define SIGN_VERIFY_STATE_MAX      18
#define DELIVERY_STATE_MAX         17
/*Define Constant Macro end*/

/*Define Macro Function start*/
#define ST_PUBLICDATA                      St_PublicData

#define SEC_CUR_UE_RESP              (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->pvCurUeResp)

#define SEC_CUR_WIM_RESP             (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->pvCurWimResp)

#define SEC_WAITING_FOR_UE           (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForUe)

#define SEC_WAITING_FOR_WIM          (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForWim)

#define SEC_WANTS_TO_RUN             (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iWantsToRun)

#define SEC_CERT_ID                  (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->uiCertId)

#define SEC_CERT_COUNT               (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->uiCertCount)

#define SEC_KEY_ID                   (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->uiKeyId)

#define SEC_KEY_COUNT                (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->uiKeyCount)

#define SEC_USER_FILE_ID             (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->uiUserFileId)

#define SEC_FILE_ID                  (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->uiFileId)

#define SEC_NAME_COUNT               (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iNameCount)

#define SEC_URL_COUNT                (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iURLCount)

#define SEC_VERIFIED_OK              (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iVerifiedOk)

#define SEC_TOO_OLD_CERT_ID          (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iTmpTooOldCertId)

#define SEC_ASKED_USER               (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iAskedUser)

#define SEC_HASH_FALSE               (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iHashFalse)

#define SEC_FRIENDLY_NAME_LEN        (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->usFriendlyNameLen)

#define SEC_CHAR_SET                 (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->sCharSet)

#define SEC_USER_CERT_LEN            (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->usTmpUserCertLen)

#define SEC_NAME_TYPE                (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->cNameType)

#define SEC_KEY_USAGE                (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->ucKeyUsage)

#define SEC_KEY_ALG                  (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->ucAlg)

#define SEC_PUBLIC_KEY_HASH          (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->aucPublicKeyHash)

#define SEC_CA_PUBLIC_KEY_HASH       (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->aucCAPublicKeyHash)

#define SEC_FRIENDLY_NAME            (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->pucFriendlyName)

#define SEC_USER_CERT                (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->pucTmpUserCert)

#define SEC_STEP_LOCAL_ROOT          (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iStepLocalRoot)

#define SEC_STEP_LOCAL_HASH          (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iSepLocalHash)

#define SEC_STATES                     (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->iStates)
#define SEC_STORECA_STATE_FORM         (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->hStoreCaStateForm)
#define SEC_STOREUSER_STATE_FORM       (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->hStoreUserStateForm)
#define SEC_HASHVERIFY_STATE_FORM      (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->hHashVerifyStateForm)
#define SEC_SIGNVERIFY_STATE_FORM      (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->hSignVerifyStateForm)
#define SEC_DELIVERY_STATE_FORM        (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->hDeliveryStateForm)
#define SEC_CERTNAMES                  (((ST_PUBLICDATA *)(((ISec*)hSecHandle)->hPrivateData))->pstCertNames)
/*Define Macro Function end*/

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_SecCertStateParam
{
    WE_INT32  iTargetID;
    WE_UCHAR  *pucCert;
    WE_UINT16 usCertLen;
    const WE_UCHAR *pucUrl;
}St_SecCertStateParam;

typedef void (*Fn_SecCertStateFunc)(WE_HANDLE, St_SecCertStateParam);

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 Sec_CertGetIssuer(WE_HANDLE hSecHandle, 
                           WE_UCHAR **ppucCertFriendlyName, WE_UCHAR *pucCert, 
                           WE_UINT16 *pusCertFriendlyNameLen, WE_INT16 *psCharSet);

WE_INT32 Sec_CertHashChecksum(WE_UINT8 *pucHashFromUser);

WE_INT32 Sec_CertHashCompare(WE_HANDLE hSecHandle, WE_UINT8 *pucHashFromUser, 
                             WE_UCHAR *pucTrustedCAInfo, WE_UINT16 usTrustedCAInfoLen);

WE_INT32 Sec_CertHashCompares(WE_UCHAR *pucCAPublicKeyHash, WE_UCHAR *pucCAKeyHash, 
                              WE_UCHAR *pucPublicKeyHash, WE_UCHAR *pucSubjectKeyHash);

WE_INT32 Sec_CertHashVerify(WE_HANDLE hSecHandle,WE_INT32 iTargetId,  WE_INT16 sCharacterSet, 
                            WE_VOID *pvDisplayName, WE_INT32 iDisplayNameLen, 
                            WE_UCHAR *pucTrustedCAInfo, WE_UINT16 usTrustedCAInfoLen);

WE_INT32 Sec_CertStoreDlgResp(WE_HANDLE hSecHandle, WE_UCHAR *pucCert);

WE_INT32 Sec_CertVerifyUserResp(WE_HANDLE hSecHandle, E_SecHashAlgType eHashAlg);

WE_INT32 Sec_CertFindPrivKeyResp(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertAlreadExistsResp(WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_INT32 *piExists, WE_CHAR cCertType);

WE_INT32 Sec_CertDelete(WE_HANDLE hSecHandle, WE_CHAR cCertType);

WE_INT32 Sec_CertDeleteResp(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertStoreResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID);

WE_INT32 Sec_CertStoreDataResp(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertConfirmDlgResp(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertDeleteResult(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertVerifyRsakey(WE_HANDLE hSecHandle, 
                              WE_INT32 iTargetID, E_SecHashAlgType eHashAlg, WE_UCHAR *pucCert);

WE_INT32 Sec_CertConfirmDlg(WE_HANDLE hSecHandle,WE_INT32 iTargetId);

WE_INT32 Sec_CertSelfSignResp(WE_HANDLE hSecHandle);

WE_INT32 Sec_CertVerifyChainResp(WE_HANDLE hSecHandle,WE_INT32 iTargetId);

WE_INT32 Sec_CertGetHashedTrustedCaInfo(WE_HANDLE hSecHandle, 
                                const WE_UCHAR *pucHashedTrusedCAInfo, WE_INT16 *psCharacterSet, 
                                WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen, 
                                WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen);
                                
static WE_VOID Sec_CertGetResp (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                WE_INT32 iResult, WE_INT32 iCertId, 
                                WE_UINT8 *pucCert, WE_INT32 iCertLen, 
                                WE_INT32 iOptions);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_CertGlobalVarInit
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize the variables which are used in the certificate module. 
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
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
WE_VOID Sec_CertGlobalVarInit(WE_HANDLE hSecHandle)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    
    SEC_USER_FILE_ID       = 0;
    SEC_FRIENDLY_NAME_LEN  = 0;
    SEC_CHAR_SET           = 0;
    SEC_NAME_TYPE          = 0;                    
    SEC_FILE_ID            = 0;
    SEC_VERIFIED_OK        = 0;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertGetPubKeyRsa
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the public key of Rsa algorithm.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle       [IN]: Global data handle.
    WE_UCHAR *pucCert          [IN]: Pointer to DER encoded certificate in buffer.
    St_SecPubKeyRsa *pstPubKey [OUT]: Pointer to public struct which to return.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    SEC_ERR_UNKNOWN_CERTIFICATE_TYPE: Unknown certificate type.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertGetPubKeyRsa(WE_HANDLE hSecHandle, WE_UCHAR *pucCert, St_SecPubKeyRsa *pstPubKey)
{
    WE_UINT16 usCertLen = 0;
    WE_INT32  iRes      = M_SEC_ERR_OK;
    WE_UCHAR  ucAlgId   = 0;
    St_SecCertificate stCert = {0};

    if (NULL == pucCert)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    if (NULL == pstPubKey)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (M_SEC_CERTIFICATE_WTLS_TYPE == (*pucCert))
    {
        iRes = Sec_WtlsCertParse(hSecHandle, pucCert + 1, &usCertLen, &stCert);
        if (M_SEC_ERR_OK != iRes)
        {
            return iRes;
        }
        pstPubKey->usExpLen = stCert.stCertUnion.stWtls.usExpLen;
        pstPubKey->pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
        pstPubKey->usModLen = stCert.stCertUnion.stWtls.usModLen;
        pstPubKey->pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
    }
    else if (M_SEC_CERTIFICATE_X509_TYPE == (*pucCert))
    {
        iRes = Sec_X509CertParse(hSecHandle, pucCert + 1, &usCertLen, &stCert);
        if (M_SEC_ERR_OK != iRes)
        {
            return iRes;
        }

        ucAlgId = stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1];
        if ((SHA1_WITH_RSA_ENCRYP == ucAlgId) || (MD5_WITH_RSA_ENCRYP == ucAlgId) || (MD2_WITH_RSA_ENCRYP == ucAlgId))
        {
            iRes = Sec_X509GetRsaModAndExp(hSecHandle, stCert.stCertUnion.stX509.pucPublicKeyVal,
                            &(pstPubKey->pucModulus), &(pstPubKey->usModLen),
                            &(pstPubKey->pucExponent), &(pstPubKey->usExpLen));
        }
        else
        {
            return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
        }
    }
    else
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }
    
    return iRes;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertGetItemsForUe
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
    modify by bird 061118
DESCRIPTION:
    Get the information from the input DER encoded certificate, and return it to Ue.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_UCHAR *pucCert    [IN]: Pointer to the DER encoded certificate in buffer.
    WE_INT16 *psIssuercharSet  [OUT]: Place to issuer character set.
    WE_VOID **ppvInIssuer      [OUT]: Place to put pointer to issuer value.
    WE_INT32 *piInIssuerLen    [OUT]: Place to length of issuer.
    WE_INT16 *psSubjectcharSet [OUT]: Place to subject character set.
    WE_VOID **ppvInSubject     [OUT]: Place to put pointer to subject value.
    WE_INT32 *piInSubjectLen   [OUT]: Place to length of subject.
    WE_UINT32 *puiValidnotBefore[OUT]: Place to the beginning of the validity period of the certificate.
    WE_UINT32 *puiValidnotAfter[OUT]: Place to the end of the validity period of the certificate.
    WE_UCHAR *ppucSerialNumber   [OUT]: Place to serial number.
    WE_INT32 *pusSerialNumberLen [OUT]: Place to length of serial number.
    WE_UCHAR **ppucAlg[OUT]:Place to algorithm
    WE_UCHAR **ppucPubKey[OUT]:place to public key
    WE_UINT16* pusPubKeyLen[OUT]:place to publick key length
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE: Certificate not surpport.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertGetItemsForUe(WE_HANDLE hSecHandle, WE_UCHAR *pucCert, WE_INT16 *psIssuercharSet, 
                            WE_VOID **ppvInIssuer, WE_INT32 *piInIssuerLen, 
                            WE_INT16 *psSubjectcharSet, 
                            WE_VOID **ppvInSubject, WE_INT32 *piInSubjectLen, 
                            WE_UINT32 *puiValidnotBefore, WE_UINT32 *puiValidnotAfter, 
                            WE_UCHAR **ppucSerialNumber,WE_UINT16* pusSerialNumberLen,
                            WE_UCHAR **ppucAlg, WE_UCHAR **ppucPubKey, 
                            WE_UINT16* pusPubKeyLen)
{
    WE_UCHAR  ucAlgId         = 0;
    WE_UCHAR  *pucPtr          = NULL;
    WE_UINT16 usCertLen       = 0;
    WE_INT32  iRes            = M_SEC_ERR_OK;
    St_SecCertificate stCert = {0};

//    WE_UCHAR* pucTmp = NULL;
    if (!pucCert || !psIssuercharSet || !ppvInIssuer || !piInIssuerLen || !psSubjectcharSet 
        || !ppvInSubject || !piInSubjectLen || !puiValidnotBefore || !puiValidnotAfter) 
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    pucPtr = pucCert;
    switch (*pucPtr)
    {
        case M_SEC_CERTIFICATE_WTLS_TYPE:
            iRes = Sec_WtlsCertParse(hSecHandle, pucPtr + 1, &usCertLen, &stCert);
            if (M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            /*add by bird 061107*/
            if((NULL != ppucSerialNumber)&&(NULL != pusSerialNumberLen)&&
                (NULL != ppucAlg)&&
                (NULL != ppucPubKey)&&(NULL != pusPubKeyLen))
            {
                WE_UCHAR aucAlg[3][16] = 
                {
                    "anonymous",/*0*/
                    "ecdsa_sha", /*1*/
                    "rsa_sha"      /*2*/
                };
                St_SecCrptPubKeyRsa stPubKey = {0};
                /*get serial number*/
                *ppucSerialNumber = NULL;
                *pusSerialNumberLen = 0;

                /*get signature algorithm*/
                *ppucAlg = (WE_UCHAR*)WE_SCL_STRNDUP((WE_CHAR*)aucAlg[stCert.stCertUnion.stWtls.ucSignatureAlgorithm],\
                    (WE_INT32)SEC_STRLEN((WE_CHAR*)aucAlg[stCert.stCertUnion.stWtls.ucSignatureAlgorithm]));
                if(NULL == *ppucAlg)
                {
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }                
                /* *pusAlgLen = stCert.stCertUnion.stX509.usSignatureAlgIdLen;*/

                /*get public key RSA*/
                stPubKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
                stPubKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
                stPubKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
                stPubKey.usModLen = stCert.stCertUnion.stWtls.usModLen;
                
                iRes = Sec_WimConvPubKey(stPubKey,ppucPubKey,pusPubKeyLen);  
                if((NULL == ppucPubKey) || (M_SEC_ERR_OK != iRes))
                {
                    WE_FREE(*ppucAlg);
                    **ppucAlg = NULL;
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
            }
            
            *piInIssuerLen     = stCert.stCertUnion.stWtls.ucIssuerLen;
            *piInSubjectLen    = stCert.stCertUnion.stWtls.ucSubjectLen;
            *psIssuercharSet   = (WE_INT16)stCert.stCertUnion.stWtls.usIssuercharacterSet;
            *psSubjectcharSet  = (WE_INT16)stCert.stCertUnion.stWtls.usSubjectcharacterSet;
            
            break;
        case M_SEC_CERTIFICATE_X509_TYPE:
            iRes = Sec_X509CertParse(hSecHandle, pucPtr + 1, &usCertLen, &stCert);
            if (M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            /*add by bird 061107*/
            if((NULL != ppucSerialNumber) && (NULL != pusSerialNumberLen)&&
                (NULL != ppucAlg)&&
                (NULL != ppucPubKey)&&(NULL != pusPubKeyLen))
            {
                WE_CHAR *pcAlg = NULL;  
                /*get serial number*/
                *ppucSerialNumber = (WE_UCHAR*)WE_MALLOC(stCert.stCertUnion.stX509.usSerialNumberLen);
                if(NULL == *ppucSerialNumber)
                {
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (void)WE_MEMCPY(*ppucSerialNumber,stCert.stCertUnion.stX509.pucSerialNumber,
                            stCert.stCertUnion.stX509.usSerialNumberLen);
                *pusSerialNumberLen = stCert.stCertUnion.stX509.usSerialNumberLen;

                /*get signature x509 algorithm???*/
                
                ucAlgId = stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1];
                switch (ucAlgId)
                {
                    case SHA1_WITH_RSA_ENCRYP:
                        pcAlg = (WE_CHAR*)"sha1WithRSAEncrypt";
                        break;
                    case MD5_WITH_RSA_ENCRYP:
                        pcAlg = (WE_CHAR*)"md5WithRSAEncrypt";
                        break;
                    case MD2_WITH_RSA_ENCRYP:
                        pcAlg = (WE_CHAR*)"md2WithRSAEncrypt";
                        break;
                    default:
                        /* add by sam [070308] */
                        WE_FREE(*ppucSerialNumber);
                        *ppucSerialNumber = NULL;
                        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
                }
                *ppucAlg  =  (WE_UCHAR*)WE_SCL_STRNDUP(pcAlg,(WE_INT32)SEC_STRLEN(pcAlg));
                if(NULL == *ppucAlg)
                {
                    WE_FREE(*ppucSerialNumber);
                    *ppucSerialNumber = NULL;
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }


                /*get public key RSA*/
/*
                stPubKey.pucExponent = stCert.stCertUnion.stX509.pucExponent;
                stPubKey.pucModulus = stCert.stCertUnion.stX509.pucModulus;
                stPubKey.usExpLen = stCert.stCertUnion.stX509.usExponentLen;
                stPubKey.usModLen = stCert.stCertUnion.stX509.usModulusLen;

                Sec_WimConvPubKey(stPubKey,ppucPubKey,pusPubKeyLen); 
*/
                *ppucPubKey = (WE_UCHAR*)WE_MALLOC(stCert.stCertUnion.stX509.usPublicKeyValLen);
                if(NULL == *ppucPubKey)
                {
                    WE_FREE(*ppucSerialNumber);
                    WE_FREE(*ppucAlg);
                    *ppucSerialNumber = NULL;
                    *ppucAlg = NULL;
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (void)WE_MEMCPY(*ppucPubKey,stCert.stCertUnion.stX509.pucPublicKeyVal,
                    stCert.stCertUnion.stX509.usPublicKeyValLen);
                *pusPubKeyLen = stCert.stCertUnion.stX509.usPublicKeyValLen;
            }
            
            *psIssuercharSet   = -1;
            *piInIssuerLen     = stCert.stCertUnion.stX509.usIssuerLen;
            *psSubjectcharSet  = -1;
            *piInSubjectLen    = stCert.stCertUnion.stX509.usSubjectLen;
            
            break;
        case WE_ASN1_SEQUENCE_TYPE:
            iRes = Sec_X509CertParse(hSecHandle, pucPtr, &usCertLen, &stCert);
            if (M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            /*add by bird 061107*/
            if((NULL != ppucSerialNumber) && (NULL != pusSerialNumberLen)&&
                (NULL != ppucAlg)&&
                (NULL != ppucPubKey)&&(NULL != pusPubKeyLen))
            {
                WE_CHAR *pcAlg = NULL;  
                /*get serial number*/
                *ppucSerialNumber = (WE_UCHAR*)WE_MALLOC(stCert.stCertUnion.stX509.usSerialNumberLen);
                if(NULL == *ppucSerialNumber)
                {
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (void)WE_MEMCPY(*ppucSerialNumber,stCert.stCertUnion.stX509.pucSerialNumber,
                            stCert.stCertUnion.stX509.usSerialNumberLen);
                *pusSerialNumberLen = stCert.stCertUnion.stX509.usSerialNumberLen;

                /*get signature x509 algorithm???*/
                
                ucAlgId = stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1];
                switch (ucAlgId)
                {
                    case SHA1_WITH_RSA_ENCRYP:
                        pcAlg = (WE_CHAR*)"sha1WithRSAEncrypt";
                        break;
                    case MD5_WITH_RSA_ENCRYP:
                        pcAlg = (WE_CHAR*)"md5WithRSAEncrypt";
                        break;
                    case MD2_WITH_RSA_ENCRYP:
                        pcAlg = (WE_CHAR*)"md2WithRSAEncrypt";
                        break;
                    default:
                        /* add by sam [070308] */
                        WE_FREE(*ppucSerialNumber);
                        *ppucSerialNumber = NULL;
                        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
                }
                *ppucAlg  =  (WE_UCHAR*)WE_SCL_STRNDUP(pcAlg,(WE_LONG)SEC_STRLEN(pcAlg));
                if(NULL == *ppucAlg)
                {
                    WE_FREE(*ppucSerialNumber);
                    *ppucSerialNumber = NULL;
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }


                /*get public key RSA*/
/*
                stPubKey.pucExponent = stCert.stCertUnion.stX509.pucExponent;
                stPubKey.pucModulus = stCert.stCertUnion.stX509.pucModulus;
                stPubKey.usExpLen = stCert.stCertUnion.stX509.usExponentLen;
                stPubKey.usModLen = stCert.stCertUnion.stX509.usModulusLen;

                Sec_WimConvPubKey(stPubKey,ppucPubKey,pusPubKeyLen); 
*/
                *ppucPubKey = (WE_UCHAR*)WE_MALLOC(stCert.stCertUnion.stX509.usPublicKeyValLen);
                if(NULL == *ppucPubKey)
                {
                    WE_FREE(*ppucSerialNumber);
                    WE_FREE(*ppucAlg);
                    *ppucSerialNumber = NULL;
                    *ppucAlg = NULL;
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (void)WE_MEMCPY(*ppucPubKey,stCert.stCertUnion.stX509.pucPublicKeyVal,
                    stCert.stCertUnion.stX509.usPublicKeyValLen);
                *pusPubKeyLen = stCert.stCertUnion.stX509.usPublicKeyValLen;

            }
            
            *psIssuercharSet   = -1;
            *piInIssuerLen     = stCert.stCertUnion.stX509.usIssuerLen;
            *psSubjectcharSet  = -1;
            *piInSubjectLen    = stCert.stCertUnion.stX509.usSubjectLen;
            
            break;
        default:
            return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }
    
    *puiValidnotBefore = stCert.iValidNotBefore;
    *puiValidnotAfter  = stCert.iValidNotAfter;
    *ppvInIssuer       = stCert.pucIssuer;
    *ppvInSubject      = stCert.pucSubject;
    
    /*Do not need fingerprint for UI Spec*/
#if 0 
    if (M_SEC_CERTIFICATE_WTLS_TYPE == (*pucCert))
    {
        eHashAlg =  E_SEC_ALG_HASH_SHA1;
        *piFingerprintLen = 20;
    }
    else
    {
        ucAlgId = stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1];
        switch (ucAlgId)
        {
            case SHA1_WITH_RSA_ENCRYP:
                eHashAlg =  E_SEC_ALG_HASH_SHA1;
                *piFingerprintLen = 20;
                break;
            case MD5_WITH_RSA_ENCRYP:
                eHashAlg = E_SEC_ALG_HASH_MD5;
                *piFingerprintLen = 16;
                break;
            case MD2_WITH_RSA_ENCRYP:
                eHashAlg = E_SEC_ALG_HASH_MD2;
                *piFingerprintLen = 16;
                break;
            default:
                return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
        }
    }
    
    iRes = Sec_LibHash(hSecHandle, eHashAlg, pucCertToHash, iCertToHashLen, pucFingerprint, piFingerprintLen);
#endif        
     return iRes;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertGetSignedTrustedCaInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the trusted ca certificate and signer certificate information from the input buffer.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    const WE_UCHAR *pucSignedTrustedCAInfo [IN]: Pointer to the DER encoded info in buffer.
    WE_INT16 *psCharacterSet     [OUT]: Place to character set.
    WE_VOID **ppvDisplayName     [OUT]: Place to put pointer to display name value.
    WE_INT32 *piDisplayNameLen   [OUT]: Place to length of display name.
    WE_UCHAR **ppucTrustedCAcert [OUT]: Place to put pointer to trusted ca certificate value.
    WE_UINT16 *pusTrustedCAcertLen[OUT]: Place to length of trusted ca certificate.
    WE_UCHAR **ppucSignerCert    [OUT]: Place to put pointer to signer certificate value.
    WE_UINT16 *pusSignerCertLen  [OUT]: Place to the length of signer certificate.
    WE_UINT8 *pucSignatureAlg    [OUT]: Place to the algorithm of signature.
    WE_UCHAR **ppucSignature     [OUT]: Place to put pointer to signature value.
    WE_UINT16 *pusSignatureLen   [OUT]: Place to length of signature.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertGetSignedTrustedCaInfo(WE_HANDLE hSecHandle, 
                                const WE_UCHAR *pucSignedTrustedCAInfo, WE_INT16 *psCharacterSet, 
                                WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen, 
                                WE_UCHAR **ppucTrustedCAcert, WE_UINT16 *pusTrustedCAcertLen, 
                                WE_UCHAR **ppucSignerCert, WE_UINT16 *pusSignerCertLen, 
                                WE_UINT8 *pucSignatureAlg, WE_UCHAR **ppucSignature, 
                                WE_UINT16 *pusSignatureLen)
{
    WE_UCHAR *pucPtr        = NULL;
    WE_UINT8  ucLen         = 0;
    WE_UINT16 usJumpStep    = 0;
    WE_UINT16 usCharacterSet= 0;
    WE_INT32  iRes          = M_SEC_ERR_OK;

    if ((NULL == pucSignedTrustedCAInfo) || (NULL == psCharacterSet) || (NULL == ppvDisplayName) 
        || (NULL == piDisplayNameLen) || (NULL == ppucTrustedCAcert) || (NULL == pusTrustedCAcertLen)
        || (NULL == ppucSignerCert) || (NULL == pusSignerCertLen) || (NULL == pucSignatureAlg)
        || (NULL == ppucSignature) || (NULL == pusSignatureLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = (WE_UCHAR *)pucSignedTrustedCAInfo;

    if(1 != *pucPtr)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucPtr++;
    Sec_StoreStrUint8to16(pucPtr, &usCharacterSet);
    *psCharacterSet = (WE_INT16)usCharacterSet;
    pucPtr += 2;
    *piDisplayNameLen = *pucPtr;
    pucPtr++;
    *ppvDisplayName = pucPtr;
    pucPtr += *piDisplayNameLen;
    *ppucTrustedCAcert = pucPtr;
    
    switch (*pucPtr)
    {
        case M_SEC_CERTIFICATE_WTLS_TYPE:
            iRes= Sec_WtlsGetCertFieldLength(hSecHandle, pucPtr + 1, pusTrustedCAcertLen);
            if(M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            (*pusTrustedCAcertLen) += 1;
            pucPtr += *pusTrustedCAcertLen;
            break;
        case M_SEC_CERTIFICATE_X509_TYPE:
            iRes= Sec_X509GetCertFieldLength(hSecHandle, pucPtr + 1, pusTrustedCAcertLen, &usJumpStep);
            if(M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            *pusTrustedCAcertLen = *pusTrustedCAcertLen + usJumpStep + 1;
            pucPtr += *pusTrustedCAcertLen;
            break;
        default:
            return M_SEC_ERR_BAD_CERTIFICATE;
    }

    if (0 != *pucPtr)
    {
        ucLen = *pucPtr;
        pucPtr += ucLen + 1;
    }

    *ppucSignerCert = pucPtr;
    switch (*pucPtr)
    {
        case M_SEC_CERTIFICATE_WTLS_TYPE:
            iRes= Sec_WtlsGetCertFieldLength(hSecHandle, pucPtr + 1, pusSignerCertLen);
            if(M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            (*pusSignerCertLen) += 1;
            pucPtr += *pusSignerCertLen;
            break;
        case M_SEC_CERTIFICATE_X509_TYPE:
            iRes= Sec_X509GetCertFieldLength(hSecHandle, pucPtr + 1, pusSignerCertLen, &usJumpStep);
            if(M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            *pusSignerCertLen = *pusSignerCertLen + usJumpStep + 1;
            pucPtr += *pusSignerCertLen;
            break;
        default:
            return M_SEC_ERR_BAD_CERTIFICATE;
    }

    *pucSignatureAlg = *pucPtr;
    pucPtr++;
    Sec_StoreStrUint8to16(pucPtr, pusSignatureLen);
    pucPtr += 2;
    *ppucSignature = pucPtr;

    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_CertGetIssuer
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the issuer from a DER encoded certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle            [IN/OUT]: Global data handle.
    WE_UCHAR **ppucCertFriendlyName [OUT]: Place to put pointer to friend name.
    WE_UCHAR *pucCert               [IN]: Pointer to the DER encoded certificate in buffer.
    WE_UINT16 *pusCertFriendlyNameLen[OUT]: Place to length of friend name.
    WE_INT16 *psCharSet              [OUT]: Place to character set.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE: Certificate not surpport.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertGetIssuer(WE_HANDLE hSecHandle, 
                            WE_UCHAR **ppucCertFriendlyName, WE_UCHAR *pucCert, 
                            WE_UINT16 *pusCertFriendlyNameLen, WE_INT16 *psCharSet)
{
    WE_UINT16  usCertLen   = 0;
    WE_UINT16  usIssuerLen = 0;
    WE_INT32   iLoop       = 0;
    WE_INT32   iRes        = M_SEC_ERR_OK;
    St_SecCertificate stCert = {0};

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (NULL == pucCert)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    if (!ppucCertFriendlyName || !pusCertFriendlyNameLen || !psCharSet)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (M_SEC_CERTIFICATE_WTLS_TYPE == (*pucCert))
    {
        iRes = Sec_WtlsCertParse(hSecHandle, pucCert + 1, &usCertLen, &stCert);
        if (M_SEC_ERR_OK != iRes)
        {
            return iRes;
        }
        usIssuerLen = stCert.stCertUnion.stWtls.ucIssuerLen;
        psCharSet[0] = (WE_INT16)stCert.stCertUnion.stWtls.usIssuercharacterSet;
        SEC_NAME_TYPE = M_SEC_CERT_NAME_WTLS;
    }
    else if (M_SEC_CERTIFICATE_X509_TYPE == (*pucCert))
    {
        iRes = Sec_X509CertParse(hSecHandle, pucCert + 1, &usCertLen, &stCert);
        if (M_SEC_ERR_OK != iRes)
        {
            return iRes;
        }
        usIssuerLen = stCert.stCertUnion.stX509.usIssuerLen;
        psCharSet[0] = -1;
        SEC_NAME_TYPE = M_SEC_CERT_NAME_X509;
    }
    else
    {
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }

    ppucCertFriendlyName[0] = (WE_UCHAR *)WE_MALLOC(usIssuerLen * sizeof(WE_UCHAR) + 2);
    if (!ppucCertFriendlyName[0])
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    for (iLoop = 0; iLoop < usIssuerLen; iLoop++)
    {
        ppucCertFriendlyName[0][iLoop] = stCert.pucIssuer[iLoop];
    }
    
    pusCertFriendlyNameLen[0] = usIssuerLen;
    ppucCertFriendlyName[0][iLoop] = '\0';
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashChecksum
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Verify the hash checksum.
ARGUMENTS PASSED:
    WE_UINT8 *pucHashFromUser [IN]: Pointer to the hash value.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertHashChecksum(WE_UINT8 *pucHashFromUser)
{
    WE_INT32 iLoop  = 0;
    WE_INT32 iSum   = 0;
    WE_INT32 iDigit = 0;

    if (NULL == pucHashFromUser)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    for (iLoop = 0; iLoop < 5; iLoop++)
    {
        iDigit = pucHashFromUser[iLoop * 6] * 2;
        if (1 == (iDigit / 10))
        {
            iSum += (1 + (iDigit % 10));
        }
        else
        {
            iSum += iDigit;
        }
        
        iSum  += pucHashFromUser[iLoop * 6 + 1];
        iDigit = pucHashFromUser[iLoop * 6 + 2] * 2;
        if (1 == (iDigit / 10))
        {
            iSum += (1 + (iDigit % 10));
        }
        else
        {
            iSum += iDigit;
        }
        
        iSum  += pucHashFromUser[iLoop * 6 + 3];
        iDigit = pucHashFromUser[iLoop * 6 + 4] * 2;
        
        if (1 == (iDigit / 10))
        {
            iSum += (1 + (iDigit % 10));
        }
        else
        {
            iSum += iDigit;
        }
        
        iSum += pucHashFromUser[iLoop * 6 + 5];
        if (0 != (iSum % 10))
        {
            return M_SEC_ERR_INVALID_PARAMETER;
        }
        iSum = 0;
    }
    
    return M_SEC_ERR_OK;    
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashCompare
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Compare the hash value.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle      [IN]: Global data handle.
    WE_UINT8 *pucHashFromUser [IN]: Pointer to the hash value.
    WE_UCHAR *pucTrustedCAInfo[IN]: Pointer to ca info in buffer.
    WE_UINT16 usTrustedCAInfoLen[IN]: Length of ca info.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertHashCompare(WE_HANDLE hSecHandle, WE_UINT8 *pucHashFromUser, 
                            WE_UCHAR *pucTrustedCAInfo, WE_UINT16 usTrustedCAInfoLen)
{
    WE_UINT8 *pucNbr       = NULL;
    WE_UINT8  aucDigest[20]= {0};
    WE_UINT8 *pucTmpDigest = NULL;
    WE_UINT16 usValue      = 0;
    WE_UINT16 usTmpValue   = 0;
    WE_INT32  iDigestLen   = 20;
    WE_INT32  iLoop        = 0;
    WE_INT32  iRes         = M_SEC_ERR_OK;

    if (!pucHashFromUser || !pucTrustedCAInfo)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRes = Sec_CertHashChecksum(pucHashFromUser);
    if (M_SEC_ERR_OK != iRes)
    {
        return iRes;
    }

    iRes=Sec_LibHash(hSecHandle, E_SEC_ALG_HASH_SHA1, pucTrustedCAInfo, 
                    usTrustedCAInfoLen, aucDigest, &iDigestLen);
    if (iRes == M_SEC_ERR_OK)
    {
        pucTmpDigest = aucDigest;
        pucNbr = pucHashFromUser;
        for (iLoop = 0; iLoop < 5; iLoop++)
        {
            usValue = (WE_UINT16)((10000 * (*pucNbr)) + (1000 * (*(pucNbr + 1))) + (100 * (*(pucNbr + 2))) 
                            + (10 * (*(pucNbr + 3))) + (*(pucNbr + 4)));
            Sec_StoreStrUint8to16(pucTmpDigest, &usTmpValue);
            if (usTmpValue == usValue)
            {
                pucNbr += 6;
                pucTmpDigest = pucTmpDigest + 2;
            }
            else
            {
                return M_SEC_ERR_INVALID_PARAMETER;
            }
        }
    }
    
    return iRes;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashCompares
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Compares the hash value. 
ARGUMENTS PASSED:
    WE_UCHAR *pucCAPublicKeyHash [IN]: Pointer to the hash value of ca public key .
    WE_UCHAR *pucCAKeyHash       [IN]: Pointer to the hash value of ca key.
    WE_UCHAR *pucPublicKeyHash   [IN]: Pointer to the hash value of public key .
    WE_UCHAR *pucSubjectKeyHash  [IN]: Pointer to the hash value of subject key .
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertHashCompares(WE_UCHAR *pucCAPublicKeyHash, WE_UCHAR *pucCAKeyHash, 
                            WE_UCHAR *pucPublicKeyHash, WE_UCHAR *pucSubjectKeyHash)
{
    WE_INT32 iLoop = 0;

    if (!pucCAPublicKeyHash || !pucCAKeyHash || !pucPublicKeyHash || !pucSubjectKeyHash)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    for(iLoop = 0; iLoop < M_SEC_KEY_HASH_SIZE; iLoop++)
    {
        if ((pucCAKeyHash[iLoop] != pucCAPublicKeyHash[iLoop]) 
            || (pucSubjectKeyHash[iLoop] != pucPublicKeyHash[iLoop]))
        {
            return M_SEC_ERR_INVALID_PARAMETER;
        }
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerify
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Verify the hash value.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle     [IN/OUT]: Global data handle.
    WE_INT16 sCharacterSet       [IN]: Character of display name.
    WE_VOID *pvDisplayName       [IN]: Pointer to display name.
    WE_INT32 iDisplayNameLen     [IN]: Length of display name.
    WE_UCHAR *pucTrustedCAInfo   [IN]: Pointer to trusted ca information.
    WE_UINT16 usTrustedCAInfoLen [IN]: Length of trusted ca information.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
    M_SEC_ERR_IN_PROGRESS: In progress.
    M_SEC_ERR_HASH_NOT_VERIFIED: Hash value not verified.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertHashVerify(WE_HANDLE hSecHandle,WE_INT32 iTargetId, WE_INT16 sCharacterSet, 
                            WE_VOID *pvDisplayName, WE_INT32 iDisplayNameLen, 
                            WE_UCHAR *pucTrustedCAInfo, WE_UINT16 usTrustedCAInfoLen)
{
    WE_INT32  iStatus = 0;
    WE_INT32  iRes    = M_SEC_ERR_OK; 
    St_SecTlVerifyHash   *pstTiStVerifyHash = NULL;
    St_SecTlPinDlgResult *pstTiStPinDiagRes = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    switch (SEC_STEP_LOCAL_HASH)
    {
        case 0:
            SEC_HASH_FALSE = 0;
            SEC_WAITING_FOR_UE = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iUeHash(hSecHandle, iTargetId,sCharacterSet, pvDisplayName, iDisplayNameLen);
            SEC_STEP_LOCAL_HASH += 1;
            iStatus = M_SEC_ERR_IN_PROGRESS;
            return iStatus;
            
        case 1:
            if (SEC_WAITING_FOR_UE)
            {
                return M_SEC_ERR_IN_PROGRESS;
            }
            
            if (!SEC_CUR_UE_RESP)
            {
                SEC_STEP_LOCAL_HASH = 0;
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
             
            switch (((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP)->eType)
            {
                case E_SEC_DP_VERIFY_HASH:
                    SEC_HASH_FALSE = 0;
                    pstTiStVerifyHash = (St_SecTlVerifyHash *)SEC_CUR_UE_RESP;
                    iRes = Sec_CertHashCompare(hSecHandle, pstTiStVerifyHash->pucHash, pucTrustedCAInfo, 
                                          usTrustedCAInfoLen);
                    switch (iRes)
                    {
                        case M_SEC_ERR_OK:
                            iStatus = (WE_INT32)E_SEC_UE_OK;
                            break;
                        case M_SEC_ERR_INVALID_PARAMETER:
                            SEC_HASH_FALSE = 1;
                            break;
                        default:
                            iStatus = (WE_INT32)E_SEC_UE_OTHER_ERROR;
                            break;
                    }
                    SEC_STEP_LOCAL_HASH++;
                    iStatus = M_SEC_ERR_IN_PROGRESS;
                    break;
                case E_SEC_DP_HASH_DLG:
                    pstTiStPinDiagRes = (St_SecTlPinDlgResult *)SEC_CUR_UE_RESP;
                    switch (pstTiStPinDiagRes->eResult)
                    {
                        case E_SEC_UE_OK:
                            iStatus = M_SEC_ERR_OK;
                            break;
                        case E_SEC_UE_CANCEL:
                            iStatus = M_SEC_ERR_CANCEL;
                            break;
                        case E_SEC_UE_PIN_LOCKED:
                        case E_SEC_UE_PIN_FALSE:
                        case E_SEC_UE_HASH_FALSE:
                        case E_SEC_UE_OTHER_ERROR:
                            iStatus = M_SEC_ERR_HASH_NOT_VERIFIED;
                            break;
                        default:
                            iStatus = M_SEC_ERR_HASH_NOT_VERIFIED;
                            break;
                    }
                    SEC_STEP_LOCAL_HASH = 0;
                    break;
                default:
                    SEC_STEP_LOCAL_HASH = 0;
                    iStatus = M_SEC_ERR_GENERAL_ERROR;
                    break;
            }
            Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            return iStatus;
    
        case 2:
            SEC_WAITING_FOR_UE = 1;
            SEC_WANTS_TO_RUN = 0;
            if (1 == SEC_HASH_FALSE)
            {
                Sec_iUeVerifyHashResp(hSecHandle,iTargetId, E_SEC_UE_HASH_FALSE);
            }
            else
            {
                Sec_iUeVerifyHashResp(hSecHandle,iTargetId, E_SEC_UE_OK);
            }
            
            SEC_STEP_LOCAL_HASH--;
            return M_SEC_ERR_IN_PROGRESS;
        default:
            SEC_STEP_LOCAL_HASH = 0;
            return M_SEC_ERR_GENERAL_ERROR;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreDlgResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the dialog response when store user certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_UCHAR *pucCert        [IN]: Pointer to the DER encoded certificate in buffer.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
    USER_CANCEL: User cancel.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertStoreDlgResp(WE_HANDLE hSecHandle, WE_UCHAR *pucCert)
{
    WE_INT32 iLoop      = 0;
    WE_INT32 iRes       = M_SEC_ERR_OK;
    St_SecTlStoreCertDlgResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlStoreCertDlgResp *)SEC_CUR_UE_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_STORE_CERT_DLG != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if (!(pstResp->ucAnswer))
    {
        return USER_CANCEL;
    }

    if (NULL == pstResp->pucCertFriendlyName)
    {            
        iRes = Sec_CertGetIssuer(hSecHandle, &SEC_FRIENDLY_NAME, pucCert, 
                                            &SEC_FRIENDLY_NAME_LEN, &SEC_CHAR_SET);
        if (M_SEC_ERR_OK != iRes)
        {
            return iRes;
        }
    }
    else
    {
        SEC_FRIENDLY_NAME_LEN  = pstResp->usFriendlyNameLen;
        SEC_FRIENDLY_NAME = (WE_UCHAR *)WE_MALLOC(SEC_FRIENDLY_NAME_LEN * sizeof(WE_UCHAR) + 1);
        if (!SEC_FRIENDLY_NAME)
        {
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        
        for (iLoop = 0; iLoop < SEC_FRIENDLY_NAME_LEN; iLoop++)
        {
            SEC_FRIENDLY_NAME[iLoop] = pstResp->pucCertFriendlyName[iLoop];
        }
        
        SEC_FRIENDLY_NAME[iLoop] = 0;
        SEC_CHAR_SET = pstResp->sCharSet;
        SEC_NAME_TYPE = M_SEC_WRITTEN_NAME;
    }
    
    return iRes;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertVerifyUserResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the verify response when store user certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    E_SecHashAlgType eHashAlg[IN]: Hash algorithm type.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE: Certificate not surpport.
    M_SEC_ERR_GENERAL_ERROR: General error.
    M_SEC_ERR_WIM_NOT_INITIALISED: Wim not initialized.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertVerifyUserResp(WE_HANDLE hSecHandle, E_SecHashAlgType eHashAlg)
{
    WE_INT32  iHashLen         = 20;
    WE_INT32  iRes             = M_SEC_ERR_OK;
    St_SecTlVerifyUserCertResp *pstResp = NULL;
    St_SecPubKeyRsa             stCaPublicKey;
    
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlVerifyUserCertResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_VERIRY_USER_CERT_CHAIN != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if (M_SEC_ERR_WIM_NOT_INITIALISED == pstResp->iResult)
    {
        return M_SEC_ERR_WIM_NOT_INITIALISED;
    }
    else if ((M_SEC_ERR_OLD_ROOTCERT_FOUND == pstResp->iResult) && (0 == SEC_ASKED_USER))
    {
        SEC_TOO_OLD_CERT_ID = pstResp->iRootCertTooOldId;
        SEC_ASKED_USER = 1;
    }
    else if (M_SEC_ERR_OK != pstResp->iResult)
    {
        return pstResp->iResult;
    }
    
    if (M_SEC_ERR_OK == pstResp->iResult)
    {
        SEC_KEY_ALG = pstResp->ucAlg;
        switch (SEC_KEY_ALG)
        {
            case M_SEC_SP_RSA:
                iRes = Sec_CertGetPubKeyRsa(hSecHandle, pstResp->pucCaCert, &stCaPublicKey);
                if (M_SEC_ERR_OK != iRes)
                {
                    return iRes;
                }
                
                iRes = Sec_LibHash(hSecHandle, eHashAlg, stCaPublicKey.pucModulus, stCaPublicKey.usModLen, 
                                    SEC_CA_PUBLIC_KEY_HASH, &iHashLen);
                break;
            default:
                iRes = M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
                break;
        }
        return iRes;
    }
    return pstResp->iResult;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertFindPrivKeyResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the find private key response when store user certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertFindPrivKeyResp(WE_HANDLE hSecHandle)
{
    St_SecTlFindPrivkeyResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlFindPrivkeyResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_LOOKUP_PRIV_KEY != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if ((M_SEC_ERR_OK != pstResp->iResult) 
        || (pstResp->uiKeyId < 1)
        || (0 == pstResp->iKeyCount) 
        || ((M_SEC_PRIVATE_NONREPKEY != pstResp->ucKeyUsage) 
        && (M_SEC_PRIVATE_AUTHKEY != pstResp->ucKeyUsage)))
    {
        if (M_SEC_ERR_OK == pstResp->iResult)
        {
            return M_SEC_ERR_INVALID_PARAMETER;
        }
        else
        {
            return pstResp->iResult;
        }
    }
    
    SEC_KEY_ID = pstResp->uiKeyId;
    SEC_KEY_COUNT = (WE_UINT32)pstResp->iKeyCount;
    SEC_KEY_USAGE = pstResp->ucKeyUsage;
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertAlreadExistsResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the exist response when store a certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 *piExists   [OUT]: Place to whether exist the certificate.
    WE_CHAR cCertType    [IN]: Certificate type.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertAlreadExistsResp(WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_INT32 *piExists, WE_CHAR cCertType)
{
    St_SecTlCertIsExistResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    if (NULL == piExists)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlCertIsExistResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    *piExists = pstResp->ucExists;
    if (E_SEC_DP_CERT_IS_EXIST != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if (M_SEC_ERR_OK != pstResp->iResult)
    {
        return pstResp->iResult;
    }
    
    if (pstResp->ucExists)
    {
        if (M_SEC_UE_USER_CERT == cCertType)
        {
            SEC_USER_FILE_ID = (WE_UINT32)pstResp->iFileId;
        }
        else if (M_SEC_UE_CA_CERT == cCertType)
        {
            SEC_FILE_ID = (WE_UINT32)pstResp->iFileId;
        }
        
        SEC_WAITING_FOR_UE = 1;
        SEC_WANTS_TO_RUN = 0;

        if((pstResp->uiNewValidNotBefore == pstResp->uiOldValidNotBefore) 
            && (pstResp->uiNewValidNotAfter == pstResp->uiOldValidNotAfter))
        {
            Sec_iUeConfirm(hSecHandle, iTargetId,M_SEC_ID_CONFIRM_4, M_SEC_ID_OVERWRITE, M_SEC_ID_CANCEL);
        }
        else if (pstResp->uiNewValidNotAfter > pstResp->uiOldValidNotAfter)
        {
            Sec_iUeConfirm(hSecHandle, iTargetId,M_SEC_ID_CONFIRM_5, M_SEC_ID_OVERWRITE, M_SEC_ID_CANCEL);
        }
        else if (pstResp->uiNewValidNotAfter < pstResp->uiOldValidNotAfter)
        {
            Sec_iUeConfirm(hSecHandle, iTargetId,M_SEC_ID_CONFIRM_6, M_SEC_ID_OVERWRITE, M_SEC_ID_CANCEL);
        }
        
    }
    
    return pstResp->iResult;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDelete
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with confirm response when delete a certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_CHAR cCertType    [IN]: certificate type.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertDelete(WE_HANDLE hSecHandle, WE_CHAR cCertType)
{
    St_SecTlConfirmDlgResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_CONFIRM_DLG != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if (!(pstResp->ucAnswer))
    {
        return USER_CANCEL;
    }    
                                                             
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    if (M_SEC_UE_USER_CERT == cCertType)
    {
        Sec_iWimDelCert(hSecHandle, (WE_INT32)SEC_USER_FILE_ID);
    }    
    else
    {
        Sec_iWimDelCert(hSecHandle, (WE_INT32)SEC_FILE_ID); 
    }
    
    return M_SEC_ERR_OK;
}                        


/*==================================================================================================
FUNCTION: 
    Sec_CertDeleteResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the remove response when delete a certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertDeleteResp(WE_HANDLE hSecHandle)
{
    St_SecTlRmCertResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlRmCertResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_RM_CERT != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    return pstResp->iResult;                         
} 


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the response of store a certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertStoreResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    St_SecTlStoreCertResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlStoreCertResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_STROE_CERT != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if (M_SEC_ERR_OK != pstResp->iResult)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    SEC_CERT_ID = (WE_UINT32)pstResp->iCertId;
    SEC_CERT_COUNT = pstResp->uiCertCount;
    SEC_URL_COUNT  = pstResp->iURLCount;
    SEC_NAME_COUNT = pstResp->iNameCount;
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimStoreUCertKeyPairInfo(hSecHandle, iTargetID, (WE_INT32)SEC_CERT_ID, (WE_INT32)SEC_CERT_COUNT, 
                    (WE_INT32)SEC_KEY_ID, (WE_INT32)SEC_KEY_COUNT, SEC_KEY_USAGE, 
                    SEC_KEY_ALG, SEC_PUBLIC_KEY_HASH, SEC_CA_PUBLIC_KEY_HASH, 
                    SEC_URL_COUNT, 0, SEC_NAME_COUNT, SEC_NAME_TYPE);
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreDataResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the response of store a certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertStoreDataResp(WE_HANDLE hSecHandle)
{
    St_SecTlStoreUcertResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlStoreUcertResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_STORE_USER_CERT_DATA != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    return pstResp->iResult;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertConfirmDlgResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the confirm dialog response.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertConfirmDlgResp(WE_HANDLE hSecHandle)
{
    St_SecTlConfirmDlgResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_CONFIRM_DLG != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeleteResult
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the delete result.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertDeleteResult(WE_HANDLE hSecHandle)
{
    St_SecTlRmCertResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlRmCertResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_RM_CERT != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }

    return pstResp->iResult;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertVerifyRsakey
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Verify the rsa key of the certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    E_SecHashAlgType eHashAlg[IN]: Hash algorithm type.
    WE_UCHAR *pucCert    [IN]: Pointer to the DER encoded certificate in buffer.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE: Certificate not surpport.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertVerifyRsakey(WE_HANDLE hSecHandle, 
                            WE_INT32 iTargetID, E_SecHashAlgType eHashAlg, WE_UCHAR *pucCert)
{
    WE_INT32  iHashLen   = 20;
    WE_INT32  iRes       = M_SEC_ERR_OK;
    St_SecPubKeyRsa      stUserPublicKey = {0};

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    switch (SEC_KEY_ALG)
    {
        case M_SEC_SP_RSA:
            iRes = Sec_CertGetPubKeyRsa(hSecHandle, pucCert, &stUserPublicKey);
            if (M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            
            iRes = Sec_LibHash(hSecHandle, eHashAlg, stUserPublicKey.pucModulus, stUserPublicKey.usModLen, 
                                SEC_PUBLIC_KEY_HASH, &iHashLen);
            if (iRes == M_SEC_ERR_OK)
            {
                SEC_WAITING_FOR_WIM = 1;
                SEC_WANTS_TO_RUN = 0;
                Sec_iWimGetFitedPrivKey(hSecHandle, iTargetID, stUserPublicKey);
            }
            break;
        default:
            iRes = M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
            break;
    }
    
    return iRes;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertConfirmDlg
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the response of store certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertConfirmDlg(WE_HANDLE hSecHandle,WE_INT32 iTargetId)
{
    St_SecTlStoreCertResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlStoreCertResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_STROE_CERT != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if (M_SEC_ERR_OK != pstResp->iResult)
    {
        return pstResp->iResult;
    }
    
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeConfirm(hSecHandle,iTargetId, M_SEC_ID_CONFIRM_7, M_SEC_ID_OK, M_SEC_ID_NONE);
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSelfSignResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the response of self-sign of certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertSelfSignResp(WE_HANDLE hSecHandle)
{
    St_SecTlSelfSignedCertResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlSelfSignedCertResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_SELF_SIGNED_CERT != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    if (M_SEC_ERR_WIM_NOT_INITIALISED == pstResp->iResult)
    {
        return pstResp->iResult;
    }    
    else if (!(pstResp->ucSelfsigned))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertVerifyChainResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Deal with the response of verify certificate chain.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_GENERAL_ERROR: General error.
    M_SEC_ERR_WIM_NOT_INITIALISED: Wim not initialized.
    M_SEC_ERR_OLD_ROOTCERT_FOUND: Not found old root certificate.
    M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE: Not mathing root certificate.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertVerifyChainResp(WE_HANDLE hSecHandle,WE_INT32 iTargetId)
{
    St_SecTlVerifyCertChainResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstResp = (St_SecTlVerifyCertChainResp *)SEC_CUR_WIM_RESP;
    if (NULL == pstResp)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (E_SEC_DP_VERIFY_CERT_CHAIN != pstResp->eType)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    SEC_VERIFIED_OK = pstResp->iResult;
    if (M_SEC_ERR_WIM_NOT_INITIALISED == pstResp->iResult)
    {
        return M_SEC_ERR_WIM_NOT_INITIALISED;
    }
    else if ((M_SEC_ERR_OLD_ROOTCERT_FOUND == SEC_VERIFIED_OK) && (0 == SEC_ASKED_USER))
    {
        SEC_TOO_OLD_CERT_ID = pstResp->iCertTooOldId;
        SEC_ASKED_USER = 1;
        return M_SEC_ERR_OLD_ROOTCERT_FOUND;
    }
    else if (M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE == SEC_VERIFIED_OK)
    {
        SEC_WAITING_FOR_UE = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iUeConfirm(hSecHandle,iTargetId, M_SEC_ID_CONFIRM_3, M_SEC_ID_STORE, M_SEC_ID_CANCEL);
        return M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE;
    }
    
    return pstResp->iResult;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertGetHashedTrustedCaInfo
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get the hashed-certificate information from the input buffer.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle      [IN]: Global data handle.
    const WE_UCHAR *pucHashedTrusedCAInfo [IN]: Pointer to the DER encoded info in buffer.
    WE_INT16 *psCharacterSet  [OUT]: Place to character set.
    WE_VOID **ppvDisplayName  [OUT]: Place to put pointer to display name value.
    WE_INT32 *piDisplayNameLen[OUT]: Place to length of display name.
    WE_UCHAR **ppucCert       [OUT]: Place to put pointer to certificate value.
    WE_UINT16 *pusCertLen     [OUT]: Place to length of certificate.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_BAD_CERTIFICATE: Certificate can't be parsed.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertGetHashedTrustedCaInfo(WE_HANDLE hSecHandle, 
                                const WE_UCHAR *pucHashedTrusedCAInfo, WE_INT16 *psCharacterSet, 
                                WE_VOID **ppvDisplayName, WE_INT32 *piDisplayNameLen, 
                                WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen)
{
    WE_UCHAR *pucPtr        = NULL;
    WE_UINT8  ucLen         = 0;
    WE_UINT16 usJumpStep    = 0;
    WE_UINT16 usCharacterSet= 0;    
    WE_INT32  iRes          = M_SEC_ERR_OK;

    if ((NULL == pucHashedTrusedCAInfo) || (NULL == psCharacterSet) || (NULL == ppvDisplayName) 
        || (NULL == piDisplayNameLen) || (NULL == ppucCert) || (NULL == pusCertLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucPtr = (WE_UCHAR *)pucHashedTrusedCAInfo;

    if(1 != *pucPtr)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucPtr++;
    Sec_StoreStrUint8to16(pucPtr, &usCharacterSet);
    *psCharacterSet = (WE_INT16)usCharacterSet;
    pucPtr += 2;

    *piDisplayNameLen = *pucPtr;
    pucPtr++;
    *ppvDisplayName = pucPtr;
    pucPtr += *piDisplayNameLen;
    *ppucCert = pucPtr;

    switch (*pucPtr)
    {
        case M_SEC_CERTIFICATE_WTLS_TYPE:
            iRes = Sec_WtlsGetCertFieldLength(hSecHandle, pucPtr + 1, pusCertLen);
            if (M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            (*pusCertLen) += 1;
            pucPtr += *pusCertLen;
            break;
        case M_SEC_CERTIFICATE_X509_TYPE:
            iRes = Sec_X509GetCertFieldLength(hSecHandle, pucPtr + 1, pusCertLen, &usJumpStep);
            if (M_SEC_ERR_OK != iRes)
            {
                return iRes;
            }
            *pusCertLen  = *pusCertLen + usJumpStep + 1;
            pucPtr   += *pusCertLen;
            break;
        default:
            return M_SEC_ERR_BAD_CERTIFICATE;
    }

    if (0 != *pucPtr)
    {
        ucLen = *pucPtr;
        pucPtr += ucLen + 1;
    }

    if (0 != *pucPtr)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertGetResp
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Response for get certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_INT32 iResult     [IN]: The result.
    WE_INT32 iCertId     [IN]: ID of certificate.
    WE_UINT8 *pucCert    [IN]: Pointer to 
    WE_INT32 iCertLen    [IN]: Length of certificate.
    WE_INT32 iOptions    [IN]: Options.
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
static WE_VOID Sec_CertGetResp (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                    WE_INT32 iResult, WE_INT32 iCertId, 
                                    WE_UINT8 *pucCert, WE_INT32 iCertLen, 
                                    WE_INT32 iOptions)
{
    if (iOptions)
    {/* iOptions is false. */
        /*Sec_GetCertResponse(hSecHandle,iTargetID, iResult, iCertId, pucCert, iCertLen, 
                             pucTrustedUsage, iTrustedUsageLen);*/
    } 
    else 
    {
        Sec_ViewAndGetCertResp(hSecHandle,iTargetID, iResult, iCertId, pucCert, iCertLen);
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState1
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
RETURN VALUE:
    None.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_CertStoreCaState1(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvIssuer         = NULL;
    WE_VOID  *pvSubject        = NULL;
    
    WE_INT16  sIssuerCharSet   = 0;
    WE_INT16  sSubjectCharSet  = 0;
    WE_INT32  iIssuerLen       = 0;
    WE_INT32  iSubjectLen      = 0;
    WE_INT32  iRes             = 0;
    WE_UINT32 uiValidNotBefore = 0;
    WE_UINT32 uiValidNotAfter  = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if ((NULL == stStateParam.pucCert) || (0 == stStateParam.usCertLen))
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_ASKED_USER = 0;
    
    if (M_SEC_ERR_OK != (iRes = Sec_CertGetItemsForUe(hSecHandle, stStateParam.pucCert, &sIssuerCharSet, 
                                                      &pvIssuer, &iIssuerLen, 
                                                      &sSubjectCharSet, &pvSubject, &iSubjectLen, 
                                                      &uiValidNotBefore, &uiValidNotAfter,
                                                      NULL, NULL,NULL,NULL,NULL)))
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeStoreCert(hSecHandle, stStateParam.iTargetID,sIssuerCharSet, pvIssuer, iIssuerLen,
                     uiValidNotBefore, uiValidNotAfter, 
                     sSubjectCharSet, 
                     pvSubject, iSubjectLen, M_SEC_UE_CA_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState2
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState2(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_UINT8   ucExitFlag   = 1;
    St_SecTlStoreCertDlgResp *pstTiCertDialogResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstTiCertDialogResp = (St_SecTlStoreCertDlgResp *)SEC_CUR_UE_RESP;
    if (E_SEC_DP_STORE_CERT_DLG != pstTiCertDialogResp->eType)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        ucExitFlag = 0;
    }
    
    if (ucExitFlag && !(pstTiCertDialogResp->ucAnswer))
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        ucExitFlag = 0;
    }
    
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    if (!ucExitFlag)
    {
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, (const WE_UCHAR *)stStateParam.pucCert, stStateParam.usCertLen);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState3
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState3(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes  = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertVerifyChainResp(hSecHandle,stStateParam.iTargetID);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    switch (iRes)
    {
        case M_SEC_ERR_WIM_NOT_INITIALISED:
            SEC_STATES++;
            break;
        case M_SEC_ERR_OLD_ROOTCERT_FOUND:
            SEC_STATES += 2;
            break;
        case M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE:
            SEC_STATES += 3;
            break;
        case M_SEC_ERR_OK:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimSelfSignedCert(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert);
            SEC_STATES += 5;
            break;
        default:
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
    
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState4
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState4(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iStatus = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleWimInit(hSecHandle, stStateParam.iTargetID, FALSE);
    switch (iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
            Sec_iUeWarning(hSecHandle, stStateParam.iTargetID,M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, (const WE_UCHAR *)stStateParam.pucCert, stStateParam.usCertLen);
            SEC_STATES--;
            break;
        case M_SEC_ERR_OK:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, (const WE_UCHAR *)stStateParam.pucCert, stStateParam.usCertLen);
            SEC_STATES--;
            break;
        default:
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iStatus);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState5
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState5(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iStatus = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_HandleOldRootCert(hSecHandle,stStateParam.iTargetID,SEC_TOO_OLD_CERT_ID);
    if (M_SEC_ERR_IN_PROGRESS == iStatus)
    {
    }
    else
    {
        SEC_WAITING_FOR_WIM = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen);
        SEC_STATES -= 2;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState6
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState6(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_UINT8   ucExitFlag   = 1;
    St_SecTlConfirmDlgResp   *pstTiConfirmDialogResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstTiConfirmDialogResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
    if (E_SEC_DP_CONFIRM_DLG != pstTiConfirmDialogResp->eType)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        ucExitFlag = 0;
    }
    
    if (ucExitFlag && !(pstTiConfirmDialogResp->ucAnswer))                                  
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        ucExitFlag = 0;
    }
    
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    if (!ucExitFlag)
    {
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimSelfSignedCert(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert);
    SEC_STATES += 2;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState8
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState8(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_UINT8   ucSelfsigned = 0;
    WE_UINT8   ucExitFlag   = 1;
    St_SecTlSelfSignedCertResp  *pstWimSignedCertResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return; 
    }
    
    pstWimSignedCertResp = (St_SecTlSelfSignedCertResp *)SEC_CUR_WIM_RESP;
    if (E_SEC_DP_SELF_SIGNED_CERT != pstWimSignedCertResp->eType)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        ucExitFlag = 0;
    }
    
    if (ucExitFlag && (M_SEC_ERR_OK != pstWimSignedCertResp->iResult))
    {                                                    
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, pstWimSignedCertResp->iResult);
        ucExitFlag = 0;
    }

    ucSelfsigned = pstWimSignedCertResp->ucSelfsigned;
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (!ucExitFlag)
    {
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    
    if(ucSelfsigned && (M_SEC_ERR_OK != SEC_VERIFIED_OK))
    {
        Sec_iWimChkCACert(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen);
        SEC_STATES += 9;
    }
    else
    {
        Sec_iWimCertExists(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen, M_SEC_UE_CA_CERT);
        SEC_STATES += 2;
    }
    
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState10
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState10(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes             = 0;
    WE_INT32  iExists          = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertAlreadExistsResp(hSecHandle,stStateParam.iTargetID, &iExists, M_SEC_UE_CA_CERT);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    if (!iExists)
    {
        SEC_STATES += 3;
    }
    else
    {
        SEC_STATES++;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState11
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState11(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY); 
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDelete(hSecHandle, M_SEC_UE_CA_CERT);
    SEC_FILE_ID = 0;
    if (M_SEC_ERR_OK != iRes)
    {
        if (USER_CANCEL == iRes)
        {
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        }
        else
        {
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        }
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_STATES++;
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState12
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState12(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDeleteResp(hSecHandle);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState13
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState13(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iStatus = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleUserVerify(hSecHandle, stStateParam.iTargetID, E_SEC_STORE_CERT);
    if (M_SEC_ERR_IN_PROGRESS == iStatus)
    {
    }
    else if (M_SEC_ERR_OK == iStatus)
    {
        SEC_WAITING_FOR_WIM = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimSaveCert(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen, M_SEC_UE_CA_CERT, NULL, 0, 0, NULL);
        SEC_STATES++;
    }
    else
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iStatus);
        Sec_DpHandleCleanUp(hSecHandle);
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState14
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState14(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    iRes = Sec_CertConfirmDlg(hSecHandle, stStateParam.iTargetID);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_STATES++;
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState15
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState15(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    Sec_DpHandleCleanUp(hSecHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreCaState17
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreCaState17(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_UINT8   ucExitFlag = 1;
    St_SecTlVerifyRootCertresp *pstWimRootCertResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return; 
    }
    
    pstWimRootCertResp = (St_SecTlVerifyRootCertresp *)SEC_CUR_WIM_RESP;
    if (E_SEC_DP_VERIRY_ROOT_CERT != pstWimRootCertResp->eType)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        ucExitFlag = 0;
    }
    
    if (ucExitFlag && (M_SEC_ERR_OK != pstWimRootCertResp->iResult))
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, pstWimRootCertResp->iResult);
        ucExitFlag = 0;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (!ucExitFlag)
    {
        Sec_DpHandleCleanUp(hSecHandle);
        return; 
    }
    SEC_STATES -= 4;
}


/*==================================================================================================
FUNCTION: 
    Sec_InitStoreCaStateForm
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize the state-form of store CA certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_InitStoreCaStateForm(WE_HANDLE hSecHandle)
{
    Fn_SecCertStateFunc *pfnStateForm = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pfnStateForm = (Fn_SecCertStateFunc *)WE_MALLOC(sizeof(Fn_SecCertStateFunc) * STORE_CA_CERT_STATE_MAX);
    if (NULL == pfnStateForm)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pfnStateForm[0] = NULL;
    pfnStateForm[1] = Sec_CertStoreCaState1;
    pfnStateForm[2] = Sec_CertStoreCaState2;
    pfnStateForm[3] = Sec_CertStoreCaState3;
    pfnStateForm[4] = Sec_CertStoreCaState4;
    pfnStateForm[5] = Sec_CertStoreCaState5;
    pfnStateForm[6] = Sec_CertStoreCaState6;
    pfnStateForm[7] = NULL;
    pfnStateForm[8] = Sec_CertStoreCaState8;
    pfnStateForm[9] = NULL;
    pfnStateForm[10] = Sec_CertStoreCaState10;
    pfnStateForm[11] = Sec_CertStoreCaState11;
    pfnStateForm[12] = Sec_CertStoreCaState12;
    pfnStateForm[13] = Sec_CertStoreCaState13;
    pfnStateForm[14] = Sec_CertStoreCaState14;
    pfnStateForm[15] = Sec_CertStoreCaState15;
    pfnStateForm[16] = NULL;
    pfnStateForm[17] = Sec_CertStoreCaState17;

    if (NULL != SEC_STORECA_STATE_FORM)
    {
        WE_FREE(SEC_STORECA_STATE_FORM);
    }
    SEC_STORECA_STATE_FORM = pfnStateForm;

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleStoreCaCert
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Store the ca certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]: Global data handle.
    WE_INT32 iTargetID  [IN]: ID of the target.
    WE_UCHAR *pucCert   [IN]: Pointer to the DER encoded certificate in buffer.
    WE_UINT16 usCertLen [IN]: length of certificate.
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
WE_VOID Sec_HandleStoreCaCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                            WE_UCHAR *pucCert, WE_UINT16 usCertLen)
{
    WE_INT32 iStates = 0;
    Fn_SecCertStateFunc *pfnStateForm = NULL;
        
    if (NULL == hSecHandle)
    {
        return;
    }

    pfnStateForm = (Fn_SecCertStateFunc *)SEC_STORECA_STATE_FORM;
    if (NULL == pfnStateForm)
    {
        return;
    }
    
    iStates = SEC_STATES;
    if ((iStates >= 1) && (iStates < STORE_CA_CERT_STATE_MAX))
    {
        St_SecCertStateParam stStateParam = {0};
        
        stStateParam.iTargetID = iTargetID;
        stStateParam.pucCert = pucCert;
        stStateParam.usCertLen = usCertLen;
        pfnStateForm[iStates](hSecHandle, stStateParam);
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState1
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState1(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvIssuer         = NULL;
    WE_VOID  *pvSubject        = NULL;
    
    WE_INT16  sIssuerCharSet   = 0;
    WE_INT16  sSubjectCharSet  = 0;
    WE_INT32  iIssuerLen       = 0;
    WE_INT32  iSubjectLen      = 0;
    WE_UINT32 uiValidNotBefore = 0;
    WE_UINT32 uiValidNotAfter  = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if ((NULL == stStateParam.pucCert) || (0 == stStateParam.usCertLen))
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_ASKED_USER  = 0;
    SEC_CERT_ID     = 0;
    SEC_CERT_COUNT  = 0;
    SEC_KEY_ID      = 0;
    SEC_KEY_COUNT   = 0;
    SEC_KEY_USAGE   = 0;
    SEC_KEY_ALG     = 0;
    SEC_URL_COUNT   = 0;
    SEC_CHAR_SET    = 0;
    SEC_NAME_TYPE   = 0;
    SEC_USER_FILE_ID          = 0;
    SEC_FRIENDLY_NAME         = NULL;
    SEC_FRIENDLY_NAME_LEN     = 0;
    SEC_PUBLIC_KEY_HASH[0]    = 0;
    SEC_CA_PUBLIC_KEY_HASH[0] = 0;
 
    if (M_SEC_ERR_OK != Sec_CertGetItemsForUe(hSecHandle, stStateParam.pucCert, &sIssuerCharSet,
                                              &pvIssuer, &iIssuerLen,
                                              &sSubjectCharSet,
                                              &pvSubject, &iSubjectLen,
                                              &uiValidNotBefore, &uiValidNotAfter,
                                              NULL, NULL,NULL,NULL,NULL))
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeStoreCert(hSecHandle, stStateParam.iTargetID,sIssuerCharSet, pvIssuer, iIssuerLen,
                     uiValidNotBefore, uiValidNotAfter, 
                     sSubjectCharSet, 
                     pvSubject, iSubjectLen, M_SEC_UE_USER_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState2
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState2(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertStoreDlgResp(hSecHandle, stStateParam.pucCert);
    if (M_SEC_ERR_OK != iRes)
    {
        if (USER_CANCEL == iRes)
        {
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        }
        else
        {
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        }
        
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_DpHandleCleanUp(hSecHandle);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        return;
    }
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, (const WE_UCHAR *)stStateParam.pucCert, stStateParam.usCertLen);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState3
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState3(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    
    E_SecHashAlgType eHashAlg = E_SEC_ALG_HASH_SHA1;
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertVerifyUserResp(hSecHandle, eHashAlg);
    switch (iRes)
    {
        case M_SEC_ERR_WIM_NOT_INITIALISED:
            SEC_STATES++;
            break;
        case M_SEC_ERR_OLD_ROOTCERT_FOUND:
            SEC_STATES += 2;
            break;
        case M_SEC_ERR_OK:
            SEC_STATES += 3;
            break;
        default:
            if (SEC_FRIENDLY_NAME)
            {
                WE_FREE(SEC_FRIENDLY_NAME);
                SEC_FRIENDLY_NAME = NULL;
            }
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState4
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState4(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iStatus = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleWimInit(hSecHandle, stStateParam.iTargetID, FALSE);
    switch (iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
            Sec_iUeWarning(hSecHandle,stStateParam.iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen);
            SEC_STATES--;
            break;
        case M_SEC_ERR_OK:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen);
            SEC_STATES--;
            break;
        default:
            if (SEC_FRIENDLY_NAME )
            {
                WE_FREE(SEC_FRIENDLY_NAME);
                SEC_FRIENDLY_NAME = NULL;
            }
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iStatus);            
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState5
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState5(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iStatus = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_HandleOldRootCert(hSecHandle,stStateParam.iTargetID, SEC_TOO_OLD_CERT_ID);
    if (M_SEC_ERR_IN_PROGRESS == iStatus)
    {
    }
    else
    {
        SEC_WAITING_FOR_WIM = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen);                          
        SEC_STATES -= 2;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState6
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState6(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;
    E_SecHashAlgType eHashAlg = E_SEC_ALG_HASH_SHA1;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iRes = Sec_CertVerifyRsakey(hSecHandle, stStateParam.iTargetID, eHashAlg, stStateParam.pucCert);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState7
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState7(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);  
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertFindPrivKeyResp(hSecHandle);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;        
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    Sec_iWimCertExists(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen, M_SEC_UE_USER_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState8
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState8(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;
    WE_INT32 iExists = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertAlreadExistsResp(hSecHandle,stStateParam.iTargetID, &iExists, M_SEC_UE_USER_CERT);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    
    if (!iExists)
    {
        SEC_STATES += 3;
    }
    else
    {
        SEC_STATES++;
    }   
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState9
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState9(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY); 
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDelete(hSecHandle, M_SEC_UE_USER_CERT);
    SEC_USER_FILE_ID = 0;
    if (M_SEC_ERR_OK != iRes)
    {
        if (USER_CANCEL == iRes)
        {
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        }
        else
        {
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        }
        
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState10
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState10(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDeleteResp(hSecHandle);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    SEC_STATES++;
}

/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState11
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState11(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    if (NULL == hSecHandle)
    {
        return;
    }

    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimSaveCert(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen, M_SEC_UE_USER_CERT, 
                    SEC_FRIENDLY_NAME, SEC_FRIENDLY_NAME_LEN , 
                    SEC_CHAR_SET, stStateParam.pucUrl);
    SEC_STATES++;
}

/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState12
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState12(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertStoreResp(hSecHandle, stStateParam.iTargetID);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    if (SEC_FRIENDLY_NAME)
    {
        WE_FREE(SEC_FRIENDLY_NAME);
        SEC_FRIENDLY_NAME = NULL;
    }
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState13
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState13(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertStoreDataResp(hSecHandle);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        SEC_WAITING_FOR_WIM = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimDelCert(hSecHandle, (WE_INT32)SEC_CERT_ID);
        SEC_STATES += 2;
        return;
    }
    
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeConfirm(hSecHandle,stStateParam.iTargetID, M_SEC_ID_CONFIRM_7, M_SEC_ID_OK, M_SEC_ID_NONE);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState14
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState14(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertConfirmDlgResp(hSecHandle);
    Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    Sec_DpHandleCleanUp(hSecHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStoreUserState15
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertStoreUserState15(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDeleteResult(hSecHandle);
    Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    Sec_DpHandleCleanUp(hSecHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_InitStoreUserStateForm
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize the state-form of store user certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_InitStoreUserStateForm(WE_HANDLE hSecHandle)
{
    Fn_SecCertStateFunc *pfnStateForm = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pfnStateForm = (Fn_SecCertStateFunc *)WE_MALLOC(sizeof(Fn_SecCertStateFunc) * STORE_USER_CERT_STATE_MAX);
    if (NULL == pfnStateForm)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pfnStateForm[0] = NULL;
    pfnStateForm[1] = Sec_CertStoreUserState1;
    pfnStateForm[2] = Sec_CertStoreUserState2;
    pfnStateForm[3] = Sec_CertStoreUserState3;
    pfnStateForm[4] = Sec_CertStoreUserState4;
    pfnStateForm[5] = Sec_CertStoreUserState5;
    pfnStateForm[6] = Sec_CertStoreUserState6;
    pfnStateForm[7] = Sec_CertStoreUserState7;
    pfnStateForm[8] = Sec_CertStoreUserState8;
    pfnStateForm[9] = Sec_CertStoreUserState9;
    pfnStateForm[10] = Sec_CertStoreUserState10;
    pfnStateForm[11] = Sec_CertStoreUserState11;
    pfnStateForm[12] = Sec_CertStoreUserState12;
    pfnStateForm[13] = Sec_CertStoreUserState13;
    pfnStateForm[14] = Sec_CertStoreUserState14;
    pfnStateForm[15] = Sec_CertStoreUserState15;

    if (NULL != SEC_STOREUSER_STATE_FORM)
    {
        WE_FREE(SEC_STOREUSER_STATE_FORM);
    }
    SEC_STOREUSER_STATE_FORM = pfnStateForm;

    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_HandleStoreUsrCert
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Store the user certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_UCHAR *pucCert    [IN]: Pointer to the DER encoded certificate in buffer.
    WE_UINT16 usCertLen  [IN]: Length of certificate.
    const WE_UCHAR *pucURL[IN]: Pointer to the URL.
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
WE_VOID Sec_HandleStoreUsrCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                            WE_UCHAR *pucCert, WE_UINT16 usCertLen, const WE_UCHAR *pucURL)
{
    WE_INT32 iStates = 0;
    Fn_SecCertStateFunc *pfnStateForm = NULL;
        
    if (NULL == hSecHandle)
    {
        return;
    }

    pfnStateForm = (Fn_SecCertStateFunc *)SEC_STOREUSER_STATE_FORM;
    if (NULL == pfnStateForm)
    {
        return;
    }
    
    iStates = SEC_STATES;
    if ((iStates >= 1) && (iStates < STORE_USER_CERT_STATE_MAX))
    {
        St_SecCertStateParam stStateParam = {0};

        stStateParam.iTargetID = iTargetID;
        stStateParam.pucCert = pucCert;
        stStateParam.usCertLen = usCertLen;
        stStateParam.pucUrl= pucURL;
        pfnStateForm[iStates](hSecHandle, stStateParam);
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState1
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState1(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iDisplayNameLen  = 0;
    WE_INT16  sCharacterSet    = 0;
    WE_UINT16 usCertLen        = 0;
    WE_INT32  iRes             = M_SEC_ERR_OK;
    
    WE_VOID  *pvDisplayName    = NULL;
    WE_UINT8 *pucCert          = NULL;
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iRes = Sec_CertGetHashedTrustedCaInfo(hSecHandle, stStateParam.pucCert, 
                                        &sCharacterSet, &pvDisplayName, 
                                        &iDisplayNameLen, &pucCert, &usCertLen);
    if ((NULL == pucCert) || (0 == usCertLen) || (M_SEC_ERR_OK != iRes))
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }    

    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimSelfSignedCert(hSecHandle, stStateParam.iTargetID, pucCert);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState2
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState2(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iDisplayNameLen  = 0;
    WE_INT16  sCharacterSet    = 0;
    WE_UINT16 usCertLen        = 0;
    WE_INT32  iRes             = M_SEC_ERR_OK;
    
    WE_VOID  *pvDisplayName    = NULL;
    WE_UINT8 *pucCert          = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return; 
    }
    
    iRes = Sec_CertSelfSignResp(hSecHandle);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    switch (iRes)
    {
        case M_SEC_ERR_WIM_NOT_INITIALISED:
            SEC_STATES++;
            break;
        case M_SEC_ERR_OK:
            iRes = Sec_CertGetHashedTrustedCaInfo(hSecHandle, stStateParam.pucCert,
                                    &sCharacterSet, &pvDisplayName, 
                                    &iDisplayNameLen, &pucCert, &usCertLen);    
            if (M_SEC_ERR_OK != iRes)
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER); 
                return;
            }
            SEC_WAITING_FOR_WIM = 1;
            Sec_iWimChkCACert(hSecHandle, stStateParam.iTargetID, pucCert, usCertLen);
            SEC_STATES += 2;
            break;
        default:
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState3
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState3(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iDisplayNameLen  = 0;
    WE_INT16  sCharacterSet    = 0;
    WE_UINT16 usCertLen        = 0;
    WE_INT32  iRes             = M_SEC_ERR_OK;
    WE_INT32  iStatus          = 0;
    WE_VOID  *pvDisplayName    = NULL;
    WE_UINT8 *pucCert          = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleWimInit(hSecHandle, stStateParam.iTargetID, FALSE);
    switch (iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
            Sec_iUeWarning(hSecHandle,stStateParam.iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
            iRes = Sec_CertGetHashedTrustedCaInfo(hSecHandle, stStateParam.pucCert,
                                    &sCharacterSet, &pvDisplayName, 
                                    &iDisplayNameLen, &pucCert, &usCertLen);
            if (M_SEC_ERR_OK != iRes)
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER); 
                return;
            }
            
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimSelfSignedCert(hSecHandle, stStateParam.iTargetID, pucCert);
            SEC_STATES--;
            break;
        case M_SEC_ERR_OK:
            iRes = Sec_CertGetHashedTrustedCaInfo(hSecHandle, stStateParam.pucCert,
                                    &sCharacterSet, &pvDisplayName, 
                                    &iDisplayNameLen, &pucCert, &usCertLen);
            if (M_SEC_ERR_OK != iRes)
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER); 
                return;
            }
            
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimSelfSignedCert(hSecHandle, stStateParam.iTargetID, pucCert);
            SEC_STATES--;
            break;
        default:
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iStatus); 
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    } 
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState4
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState4(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    St_SecTlVerifyRootCertresp *pstWimRootCertResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);    
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstWimRootCertResp = (St_SecTlVerifyRootCertresp *)SEC_CUR_WIM_RESP;
    if (E_SEC_DP_VERIRY_ROOT_CERT != pstWimRootCertResp->eType)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR); 
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    if (M_SEC_ERR_OK != pstWimRootCertResp->iResult)    
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, pstWimRootCertResp->iResult);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState5
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState5(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName    = NULL;
    WE_VOID  *pvIssuer         = NULL;
    WE_VOID  *pvSubject        = NULL;
    WE_UINT8 *pucCert          = NULL;
    
    WE_INT16  sCharacterSet    = 0;
    WE_INT32  iDisplayNameLen  = 0;
    WE_UINT16 usCertLen        = 0;
    WE_INT16  sIssuerCharSet   = 0;
    WE_INT32  iIssuerLen       = 0;
    WE_INT16  sSubjectCharSet  = 0;
    WE_INT32  iSubjectLen      = 0;
    WE_INT32  iRes             = M_SEC_ERR_OK;
    WE_INT32  iStatus          = 0;
    WE_UINT32 uiValidNotBefore = 0;
    WE_UINT32 uiValidNotAfter  = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iRes = Sec_CertGetHashedTrustedCaInfo(hSecHandle, stStateParam.pucCert,
                            &sCharacterSet, &pvDisplayName, 
                            &iDisplayNameLen, &pucCert, &usCertLen);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }    
    iStatus = Sec_CertHashVerify(hSecHandle, stStateParam.iTargetID,sCharacterSet, pvDisplayName, iDisplayNameLen, 
                            stStateParam.pucCert, stStateParam.usCertLen);
    switch (iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_OK:
            if ((iRes = Sec_CertGetItemsForUe(hSecHandle, pucCert, &sIssuerCharSet, &pvIssuer, &iIssuerLen, 
                            &sSubjectCharSet, &pvSubject, &iSubjectLen, 
                            &uiValidNotBefore, &uiValidNotAfter,
                            NULL, NULL,NULL,NULL,NULL)) != M_SEC_ERR_OK)
            {
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            SEC_WAITING_FOR_UE = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iUeStoreCert(hSecHandle,stStateParam.iTargetID, sIssuerCharSet, pvIssuer, iIssuerLen,
                            uiValidNotBefore, uiValidNotAfter, 
                            sSubjectCharSet, pvSubject, iSubjectLen, 
                            M_SEC_UE_CA_CERT);
            SEC_STATES++;
            break;  
        default:
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iStatus);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState6
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState6(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT16  sCharacterSet    = 0;
    WE_UINT16 usCertLen        = 0;
    WE_INT32  iRes             = M_SEC_ERR_OK;
    WE_INT32  iDisplayNameLen  = 0;
    
    WE_VOID  *pvDisplayName    = NULL;
    WE_UINT8 *pucCert          = NULL;
    St_SecTlStoreCertDlgResp *pstTiCertDialogResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstTiCertDialogResp = (St_SecTlStoreCertDlgResp *)SEC_CUR_UE_RESP;
    if (E_SEC_DP_STORE_CERT_DLG != pstTiCertDialogResp->eType)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    if (!(pstTiCertDialogResp->ucAnswer))                    
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    iRes = Sec_CertGetHashedTrustedCaInfo(hSecHandle, stStateParam.pucCert,
                            &sCharacterSet, &pvDisplayName, 
                            &iDisplayNameLen, &pucCert, &usCertLen);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimCertExists(hSecHandle, stStateParam.iTargetID, pucCert, usCertLen, M_SEC_UE_CA_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState7
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState7(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes    = M_SEC_ERR_OK;
    WE_INT32  iExists = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertAlreadExistsResp(hSecHandle,stStateParam.iTargetID, &iExists, M_SEC_UE_CA_CERT);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    if (!iExists)
    {
        SEC_STATES += 3;
    }
    else
    {
        SEC_STATES++;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState8
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState8(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY); 
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDelete(hSecHandle, M_SEC_UE_CA_CERT);
    SEC_FILE_ID = 0;
    if (M_SEC_ERR_OK != iRes)
    {
        if (USER_CANCEL == iRes)
        {
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        }                
        else
        {
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        }
        
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState9
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState9(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDeleteResp(hSecHandle);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState10
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState10(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT16  sCharacterSet    = 0;
    WE_UINT16 usCertLen        = 0;
    WE_INT32  iRes             = M_SEC_ERR_OK;
    WE_INT32  iDisplayNameLen  = 0;
    WE_INT32  iStatus          = 0;
    WE_VOID  *pvDisplayName    = NULL;
    WE_UINT8 *pucCert          = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleUserVerify(hSecHandle, stStateParam.iTargetID, E_SEC_STORE_CERT);
    switch(iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_OK:
            iRes = Sec_CertGetHashedTrustedCaInfo(hSecHandle, stStateParam.pucCert,
                                    &sCharacterSet, &pvDisplayName, 
                                    &iDisplayNameLen, &pucCert, &usCertLen);
            if (M_SEC_ERR_OK != iRes)
            {
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
                Sec_DpHandleCleanUp(hSecHandle); 
                return;
            }
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimSaveCert(hSecHandle, stStateParam.iTargetID, pucCert, usCertLen, M_SEC_UE_CA_CERT, NULL, 0, 0, NULL);
            SEC_STATES++;
            break;
        default:
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iStatus);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    } 
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState11
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertHashVerifyState11(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertConfirmDlg(hSecHandle,stStateParam.iTargetID);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertHashVerifyState12
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Omit
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_CertHashVerifyState12(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    Sec_DpHandleCleanUp(hSecHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_InitHashVerifyStateForm
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize the state-form of verify the hashed-certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_InitHashVerifyStateForm(WE_HANDLE hSecHandle)
{
    Fn_SecCertStateFunc *pfnStateForm = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pfnStateForm = (Fn_SecCertStateFunc *)WE_MALLOC(sizeof(Fn_SecCertStateFunc) * HASH_VERIFY_STATE_MAX);
    if (NULL == pfnStateForm)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pfnStateForm[0] = NULL;
    pfnStateForm[1] = Sec_CertHashVerifyState1;
    pfnStateForm[2] = Sec_CertHashVerifyState2;
    pfnStateForm[3] = Sec_CertHashVerifyState3;
    pfnStateForm[4] = Sec_CertHashVerifyState4;
    pfnStateForm[5] = Sec_CertHashVerifyState5;
    pfnStateForm[6] = Sec_CertHashVerifyState6;
    pfnStateForm[7] = Sec_CertHashVerifyState7;
    pfnStateForm[8] = Sec_CertHashVerifyState8;
    pfnStateForm[9] = Sec_CertHashVerifyState9;
    pfnStateForm[10] = Sec_CertHashVerifyState10;
    pfnStateForm[11] = Sec_CertHashVerifyState11;
    pfnStateForm[12] = Sec_CertHashVerifyState12;

    if (NULL != SEC_HASHVERIFY_STATE_FORM)
    {
        WE_FREE(SEC_HASHVERIFY_STATE_FORM);
    }
    SEC_HASHVERIFY_STATE_FORM = pfnStateForm;

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleHashVerify
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Verify the hashed-certificat.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_UCHAR *pucTrustedCAInfo  [IN]: Pointer to trusted ca information.
    WE_UINT16 usTrustedCAInfoLen[IN]: Length of trusted ca information.
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
WE_VOID Sec_HandleHashVerify(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                            WE_UCHAR *pucTrustedCAInfo, WE_UINT16 usTrustedCAInfoLen)
{
    WE_INT32 iStates = 0;
    Fn_SecCertStateFunc *pfnStateForm = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }

    pfnStateForm = (Fn_SecCertStateFunc *)SEC_HASHVERIFY_STATE_FORM;
    if (NULL == pfnStateForm)
    {
        return;
    }
    
    iStates = SEC_STATES;
    if ((iStates >= 1) && (iStates < HASH_VERIFY_STATE_MAX))
    {
        St_SecCertStateParam stStateParam = {0};

        stStateParam.iTargetID = iTargetID;
        stStateParam.pucCert = pucTrustedCAInfo;
        stStateParam.usCertLen = usTrustedCAInfoLen;
        pfnStateForm[iStates](hSecHandle, stStateParam);
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState1
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState1(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                        stStateParam.pucCert, &sCharacterSet, 
                        &pvDisplayName, &iDisplayNameLen,
                        &pucTrustedCAcert, &usTrustedCAcertLen,
                        &pucSignerCert, &usSignerCertLen,
                        &ucSignatureAlg, &pucSignature, &usSignatureLen);
    if ((NULL == pucTrustedCAcert) || (0 == usTrustedCAcertLen) || (M_SEC_ERR_OK != iRes))
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_ASKED_USER = 0;
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeNameConfirm(hSecHandle, stStateParam.iTargetID, sCharacterSet, pvDisplayName, 
                    iDisplayNameLen, M_SEC_ID_CONFIRM_14, 
                    M_SEC_ID_OK, M_SEC_ID_CANCEL);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState1
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState2(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;
    
    St_SecTlConfirmDlgResp *pstTiConfirmDialogResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstTiConfirmDialogResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
    if (E_SEC_DP_CONFIRM_DLG != pstTiConfirmDialogResp->eType)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    if (!(pstTiConfirmDialogResp->ucAnswer))
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                            stStateParam.pucCert, &sCharacterSet, 
                            &pvDisplayName, &iDisplayNameLen,
                            &pucTrustedCAcert, &usTrustedCAcertLen,
                            &pucSignerCert, &usSignerCertLen,
                            &ucSignatureAlg, &pucSignature, &usSignatureLen);
    if ((NULL == pucTrustedCAcert) || (0 == usTrustedCAcertLen) || (M_SEC_ERR_OK != iRes))
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, pucSignerCert, usSignerCertLen);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState3
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState3(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertVerifyChainResp(hSecHandle,stStateParam.iTargetID);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    switch (iRes)
    {
        case M_SEC_ERR_WIM_NOT_INITIALISED:
            SEC_STATES++;
            break;
        case M_SEC_ERR_OLD_ROOTCERT_FOUND:
            SEC_STATES += 2;
            break;
        case M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE:
            SEC_STATES += 3;
            break;
        case M_SEC_ERR_OK:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimVerifySign(hSecHandle, stStateParam.iTargetID, stStateParam.pucCert, stStateParam.usCertLen);
            SEC_STATES += 5;
            break;
        default:
            Sec_SaveCertResp(hSecHandle, stStateParam.iTargetID, iRes);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }  
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState4
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState4(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;
    WE_INT32  iStatus            = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleWimInit(hSecHandle, stStateParam.iTargetID, FALSE);
    switch (iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
            Sec_iUeWarning(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
            iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                                    stStateParam.pucCert, &sCharacterSet, 
                                    &pvDisplayName, &iDisplayNameLen,
                                    &pucTrustedCAcert, &usTrustedCAcertLen,
                                    &pucSignerCert, &usSignerCertLen,
                                    &ucSignatureAlg, &pucSignature, &usSignatureLen);
            if ((NULL == pucTrustedCAcert) || (0 == usTrustedCAcertLen) || (M_SEC_ERR_OK != iRes))
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER); 
                return;
            }
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, pucSignerCert, usSignerCertLen);
            SEC_STATES--;
            break;
        case M_SEC_ERR_OK:
            iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                                    stStateParam.pucCert, &sCharacterSet, 
                                    &pvDisplayName, &iDisplayNameLen,
                                    &pucTrustedCAcert, &usTrustedCAcertLen,
                                    &pucSignerCert, &usSignerCertLen,
                                    &ucSignatureAlg, &pucSignature, &usSignatureLen);
            if ((NULL == pucTrustedCAcert) || (0 == usTrustedCAcertLen) || (M_SEC_ERR_OK != iRes))
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER); 
                return;
            }
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, pucSignerCert, usSignerCertLen);
            SEC_STATES--;
            break;
        default:
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iStatus);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState5
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState5(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;
    WE_INT32  iStatus            = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_HandleOldRootCert(hSecHandle, stStateParam.iTargetID,SEC_TOO_OLD_CERT_ID);
    if (M_SEC_ERR_IN_PROGRESS == iStatus)
    {
    }
    else
    {
        iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                                stStateParam.pucCert, &sCharacterSet, 
                                &pvDisplayName, &iDisplayNameLen,
                                &pucTrustedCAcert, &usTrustedCAcertLen,
                                &pucSignerCert, &usSignerCertLen,
                                &ucSignatureAlg, &pucSignature, &usSignatureLen);
        if ((pucTrustedCAcert == NULL) || (usTrustedCAcertLen == 0) || (M_SEC_ERR_OK != iRes))
        {
            Sec_DpHandleCleanUp(hSecHandle);
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER); 
            return;                         
        }
        SEC_WAITING_FOR_WIM = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimWtlsVerifySvrCertChain(hSecHandle, stStateParam.iTargetID, pucSignerCert, usSignerCertLen);
        SEC_STATES -= 2;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState6
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState6(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    St_SecTlConfirmDlgResp   *pstTiConfirmDialogResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstTiConfirmDialogResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
    if(pstTiConfirmDialogResp->eType != E_SEC_DP_CONFIRM_DLG)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);    
        Sec_DpHandleCleanUp(hSecHandle);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        return;
    }
    if (!(pstTiConfirmDialogResp->ucAnswer))
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_DpHandleCleanUp(hSecHandle);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        return;
    }

    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState7
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState7(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimVerifySign( hSecHandle, 
                        stStateParam.iTargetID, 
                        stStateParam.pucCert, 
                        stStateParam.usCertLen );
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState8
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState8(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;
    St_SecTlVerifySignResp *pstWimSigVerifResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstWimSigVerifResp = (St_SecTlVerifySignResp *)SEC_CUR_WIM_RESP;
    if(pstWimSigVerifResp->eType != E_SEC_DP_VRRIFY_SIGN)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);    
        Sec_DpHandleCleanUp(hSecHandle);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        return;
    }
    if (pstWimSigVerifResp->iResult != M_SEC_ERR_OK)        
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, pstWimSigVerifResp->iResult);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;

    iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                            stStateParam.pucCert, &sCharacterSet, 
                            &pvDisplayName, &iDisplayNameLen, &pucTrustedCAcert, 
                            &usTrustedCAcertLen, &pucSignerCert, &usSignerCertLen, 
                            &ucSignatureAlg, &pucSignature, &usSignatureLen);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_DpHandleCleanUp(hSecHandle);
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimSelfSignedCert(hSecHandle, stStateParam.iTargetID, pucTrustedCAcert);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState9
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState9(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);    
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertSelfSignResp(hSecHandle);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    switch (iRes)
    {
        case M_SEC_ERR_WIM_NOT_INITIALISED:
            SEC_STATES++;
            break;
        case M_SEC_ERR_OK:
            iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                                    stStateParam.pucCert, &sCharacterSet, 
                                    &pvDisplayName, &iDisplayNameLen,
                                    &pucTrustedCAcert, &usTrustedCAcertLen,
                                    &pucSignerCert, &usSignerCertLen,
                                    &ucSignatureAlg, &pucSignature, &usSignatureLen);
            if (M_SEC_ERR_OK != iRes)
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER); 
                return;
            }
            
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimChkCACert(hSecHandle, stStateParam.iTargetID, pucTrustedCAcert, usTrustedCAcertLen);
            SEC_STATES++;
            break;
        default:
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState10
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState10(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_VOID  *pvIssuer           = NULL;
    WE_VOID  *pvSubject          = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_INT16  sIssuerCharSet     = 0;
    WE_INT16  sSubjectCharSet    = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iIssuerLen         = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iSubjectLen        = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;
    WE_UINT32 uiValidNotBefore   = 0;
    WE_UINT32 uiValidNotAfter    = 0;
    St_SecTlVerifyRootCertresp *pstWimRootCertResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);    
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstWimRootCertResp = (St_SecTlVerifyRootCertresp *)SEC_CUR_WIM_RESP;
    if (E_SEC_DP_VERIRY_ROOT_CERT != pstWimRootCertResp->eType)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    if (M_SEC_ERR_OK != pstWimRootCertResp->iResult)    
    {                                                    
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, pstWimRootCertResp->iResult);
        Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                            stStateParam.pucCert, &sCharacterSet, 
                            &pvDisplayName, &iDisplayNameLen,
                            &pucTrustedCAcert, &usTrustedCAcertLen,
                            &pucSignerCert, &usSignerCertLen,
                            &ucSignatureAlg, &pucSignature, &usSignatureLen);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;                         
    }
    
    if (M_SEC_ERR_OK != (iRes = Sec_CertGetItemsForUe(hSecHandle, pucTrustedCAcert, 
                            &sIssuerCharSet, &pvIssuer, &iIssuerLen, 
                            &sSubjectCharSet, &pvSubject, &iSubjectLen, 
                            &uiValidNotBefore, &uiValidNotAfter,
                            NULL, NULL,NULL,NULL,NULL)))
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeStoreCert(hSecHandle, stStateParam.iTargetID, sIssuerCharSet, pvIssuer, iIssuerLen,
                    uiValidNotBefore, uiValidNotAfter, 
                    sSubjectCharSet, 
                    pvSubject, iSubjectLen, M_SEC_UE_CA_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState11
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState11(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;
    St_SecTlStoreCertDlgResp *pstTiCertDialogResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstTiCertDialogResp = (St_SecTlStoreCertDlgResp *)SEC_CUR_UE_RESP;
    if (E_SEC_DP_STORE_CERT_DLG != pstTiCertDialogResp->eType)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    if (!(pstTiCertDialogResp->ucAnswer))
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle, 
                            stStateParam.pucCert, &sCharacterSet, 
                            &pvDisplayName, &iDisplayNameLen,
                            &pucTrustedCAcert, &usTrustedCAcertLen,
                            &pucSignerCert, &usSignerCertLen,
                            &ucSignatureAlg, &pucSignature, &usSignatureLen);
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimCertExists(hSecHandle, stStateParam.iTargetID, pucTrustedCAcert, usTrustedCAcertLen, M_SEC_UE_CA_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState12
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_CertSignVerifyState12(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes    = M_SEC_ERR_OK;
    WE_INT32  iExists = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertAlreadExistsResp(hSecHandle, stStateParam.iTargetID,&iExists, M_SEC_UE_CA_CERT);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    if (!iExists)
    {
        SEC_STATES += 3;
    }
    else
    {
        SEC_STATES++;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState13
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState13(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDelete(hSecHandle, M_SEC_UE_CA_CERT);
    SEC_FILE_ID = 0;
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        if (USER_CANCEL == iRes)
        {
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        }
        else
        {
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        }
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState14
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState14(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDeleteResp(hSecHandle);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState15
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_CertSignVerifyState15(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName      = NULL;
    WE_UINT8 *pucTrustedCAcert   = NULL;
    WE_UINT8 *pucSignerCert      = NULL;
    WE_UINT8 *pucSignature       = NULL;
    WE_UINT8  ucSignatureAlg     = 0;
    WE_INT16  sCharacterSet      = 0;
    WE_UINT16 usSignatureLen     = 0;
    WE_UINT16 usTrustedCAcertLen = 0;
    WE_UINT16 usSignerCertLen    = 0;
    WE_INT32  iDisplayNameLen    = 0;
    WE_INT32  iRes               = M_SEC_ERR_OK;
    WE_INT32  iStatus            = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleUserVerify(hSecHandle, stStateParam.iTargetID, E_SEC_STORE_CERT);
    switch(iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_OK:
            iRes = Sec_CertGetSignedTrustedCaInfo(hSecHandle,
                                    stStateParam.pucCert, &sCharacterSet, 
                                    &pvDisplayName, &iDisplayNameLen,
                                    &pucTrustedCAcert, &usTrustedCAcertLen,
                                    &pucSignerCert, &usSignerCertLen,
                                    &ucSignatureAlg, &pucSignature, &usSignatureLen);
            if (M_SEC_ERR_OK != iRes)
            {
                Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INVALID_PARAMETER);
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimSaveCert(hSecHandle, stStateParam.iTargetID, pucTrustedCAcert, usTrustedCAcertLen, M_SEC_UE_CA_CERT, NULL, 0, 0, NULL);
            SEC_STATES++;
            break;
        case M_SEC_ERR_USER_NOT_VERIFIED:
            
        default:
            Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iStatus);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState16
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState16(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32  iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertConfirmDlg(hSecHandle,stStateParam.iTargetID);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertSignVerifyState17
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertSignVerifyState17(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_TrustedCAInfoResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    Sec_DpHandleCleanUp(hSecHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_InitSignVerifyStateForm
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize the state-form of verify the signed-certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_InitSignVerifyStateForm(WE_HANDLE hSecHandle)
{
    Fn_SecCertStateFunc *pfnStateForm = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pfnStateForm = (Fn_SecCertStateFunc *)WE_MALLOC(sizeof(Fn_SecCertStateFunc) * SIGN_VERIFY_STATE_MAX);
    if (NULL == pfnStateForm)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pfnStateForm[0] = NULL;
    pfnStateForm[1] = Sec_CertSignVerifyState1;
    pfnStateForm[2] = Sec_CertSignVerifyState2;
    pfnStateForm[3] = Sec_CertSignVerifyState3;
    pfnStateForm[4] = Sec_CertSignVerifyState4;
    pfnStateForm[5] = Sec_CertSignVerifyState5;
    pfnStateForm[6] = Sec_CertSignVerifyState6;
    pfnStateForm[7] = Sec_CertSignVerifyState7;
    pfnStateForm[8] = Sec_CertSignVerifyState8;
    pfnStateForm[9] = Sec_CertSignVerifyState9;
    pfnStateForm[10] = Sec_CertSignVerifyState10;
    pfnStateForm[11] = Sec_CertSignVerifyState11;
    pfnStateForm[12] = Sec_CertSignVerifyState12;
    pfnStateForm[13] = Sec_CertSignVerifyState13;
    pfnStateForm[14] = Sec_CertSignVerifyState14;
    pfnStateForm[15] = Sec_CertSignVerifyState15;
    pfnStateForm[16] = Sec_CertSignVerifyState16;
    pfnStateForm[17] = Sec_CertSignVerifyState17;

    if (NULL != SEC_SIGNVERIFY_STATE_FORM)
    {
        WE_FREE(SEC_SIGNVERIFY_STATE_FORM);
    }
    SEC_SIGNVERIFY_STATE_FORM = pfnStateForm;

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleSignVerify
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Verify the signed-certificat.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_UCHAR *pucTrustedCAInfo  [IN]: Pointer to trusted ca information.
    WE_UINT16 usTrustedCAInfoLen[IN]: Length of trusted ca information.
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
WE_VOID Sec_HandleSignVerify(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                            WE_UCHAR *pucTrustedCAInfo, WE_UINT16 usTrustedCAInfoLen)
{
    WE_INT32 iStates = 0;
    Fn_SecCertStateFunc *pfnStateForm = NULL;
        
    if (NULL == hSecHandle)
    {
        return;
    }

    pfnStateForm = (Fn_SecCertStateFunc *)SEC_SIGNVERIFY_STATE_FORM;
    if (NULL == pfnStateForm)
    {
        return;
    }
    
    iStates = SEC_STATES;
    if ((iStates >= 1) && (iStates < SIGN_VERIFY_STATE_MAX))
    {
        St_SecCertStateParam stStateParam = {0};
        
        stStateParam.iTargetID = iTargetID;
        stStateParam.pucCert = pucTrustedCAInfo;
        stStateParam.usCertLen = usTrustedCAInfoLen;
        pfnStateForm[iStates](hSecHandle, stStateParam);
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState1
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState1(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName     = NULL;
    WE_UINT8 *pucCert           = NULL;
    WE_UINT8 *pucCaKeyHash      = NULL;
    WE_UINT8 *pucSubjectKeyHash = NULL;
    WE_INT16  sCharacterSet     = 0;
    WE_UINT16 usCertLen         = 0;
    WE_INT32  iDisplayNameLen   = 0;
    WE_INT32  iRes              = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iRes = Sec_X509CertParseResponse(hSecHandle, 
                    stStateParam.pucCert, &sCharacterSet, &pvDisplayName, 
                    &iDisplayNameLen, &pucCaKeyHash, &pucSubjectKeyHash,
                    &pucCert, &usCertLen);
    if ((pucCert == NULL) || (usCertLen == 0) || (M_SEC_ERR_OK != iRes))
    {
        Sec_DpHandleCleanUp(hSecHandle);
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes); 
        return;
    }
    
    SEC_ASKED_USER = 0;
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeNameConfirm(hSecHandle, stStateParam.iTargetID,sCharacterSet, pvDisplayName, 
                    iDisplayNameLen, M_SEC_ID_CONFIRM_3, 
                    M_SEC_ID_OK, M_SEC_ID_CANCEL);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState2
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState2(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvIssuer          = NULL;
    WE_VOID  *pvSubject         = NULL;
    WE_VOID  *pvDisplayName     = NULL;
    WE_UINT8 *pucCert           = NULL;
    WE_UINT8 *pucCaKeyHash      = NULL;
    WE_UINT8 *pucSubjectKeyHash = NULL;
    
    WE_INT16  sCharacterSet     = 0;
    WE_INT16  sIssuerCharSet    = 0;
    WE_INT16  sSubjectCharSet   = 0;
    WE_INT32  iIssuerLen        = 0;
    WE_UINT16 usCertLen         = 0;
    WE_INT32  iDisplayNameLen   = 0;
    WE_INT32  iSubjectLen       = 0;
    WE_INT32  iRes              = M_SEC_ERR_OK;
    WE_INT32  iLoop             = 0;
    WE_UINT32 uiValidNotBefore  = 0;
    WE_UINT32 uiValidNotAfter   = 0;
    
    St_SecTlConfirmDlgResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    pstResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
    if (E_SEC_DP_CONFIRM_DLG != pstResp->eType)
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    if (!(pstResp->ucAnswer))
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    iRes = Sec_X509CertParseResponse(hSecHandle, stStateParam.pucCert, 
                    &sCharacterSet, &pvDisplayName, 
                    &iDisplayNameLen, &pucCaKeyHash, &pucSubjectKeyHash,
                    &pucCert, &usCertLen);
    if ((NULL == pucCert) || (0 == usCertLen) || (M_SEC_ERR_OK != iRes))
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_USER_CERT_LEN = usCertLen + 1;
    SEC_USER_CERT = (WE_UCHAR *)WE_MALLOC((SEC_USER_CERT_LEN) * sizeof(WE_UCHAR));
    if (!SEC_USER_CERT)
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_USER_CERT[0] = '\x02';
    for (iLoop = 0; iLoop < usCertLen; iLoop++)
    {
        SEC_USER_CERT[iLoop+1] = pucCert[iLoop];
    }

    SEC_CERT_ID = 0;
    SEC_CERT_COUNT = 0;
    SEC_KEY_ID = 0;
    SEC_KEY_COUNT = 0;
    SEC_KEY_USAGE = 0;
    SEC_KEY_ALG = 0;
    SEC_PUBLIC_KEY_HASH[0] = 0;
    SEC_CA_PUBLIC_KEY_HASH[0] = 0;
    SEC_URL_COUNT = 0;

    SEC_USER_FILE_ID = 0;
    SEC_FRIENDLY_NAME = NULL;
    SEC_FRIENDLY_NAME_LEN  = 0;
    SEC_CHAR_SET = 0;
    SEC_NAME_TYPE = 0;
    
    if (Sec_CertGetItemsForUe(hSecHandle, SEC_USER_CERT, &sIssuerCharSet, 
                    &pvIssuer, &iIssuerLen, 
                    &sSubjectCharSet, &pvSubject, &iSubjectLen, 
                    &uiValidNotBefore, &uiValidNotAfter,
                    NULL, NULL,NULL,NULL,NULL) !=M_SEC_ERR_OK)
    {
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iUeStoreCert(hSecHandle,stStateParam.iTargetID,sIssuerCharSet, pvIssuer, iIssuerLen,
                    uiValidNotBefore, uiValidNotAfter, 
                    sSubjectCharSet, pvSubject, iSubjectLen, M_SEC_UE_USER_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState3
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState3(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName     = NULL;
    WE_UINT8 *pucCert           = NULL;
    WE_UINT8 *pucCaKeyHash      = NULL;
    WE_UINT8 *pucSubjectKeyHash = NULL;
    WE_INT16  sCharacterSet     = 0;
    WE_UINT16 usCertLen         = 0;
    WE_INT32  iDisplayNameLen   = 0;
    WE_INT32  iRes              = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    iRes = Sec_X509CertParseResponse(hSecHandle, stStateParam.pucCert, 
                    &sCharacterSet, &pvDisplayName, 
                    &iDisplayNameLen, &pucCaKeyHash, &pucSubjectKeyHash,
                    &pucCert, &usCertLen);
    if ((NULL == pucCert) || (0 == usCertLen) || (M_SEC_ERR_OK != iRes))
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertStoreDlgResp(hSecHandle, SEC_USER_CERT);
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        if (USER_CANCEL == iRes)
        {
            Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        }
        else
        {
            Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        }
        
        Sec_DpHandleCleanUp(hSecHandle);
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        return;
    }
    
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, SEC_USER_CERT, SEC_USER_CERT_LEN);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState4
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState4(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32     iRes     = M_SEC_ERR_OK;
    E_SecHashAlgType eHashAlg = E_SEC_ALG_HASH_SHA1;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertVerifyUserResp(hSecHandle, eHashAlg);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    switch (iRes)
    {
        case M_SEC_ERR_WIM_NOT_INITIALISED:
            SEC_STATES++;
            break;
        case M_SEC_ERR_OLD_ROOTCERT_FOUND:
            SEC_STATES += 2;
            break;
        case M_SEC_ERR_OK:
            SEC_STATES += 3;
            break;
        default:
            if (SEC_FRIENDLY_NAME)
            {
                WE_FREE(SEC_FRIENDLY_NAME);
                SEC_FRIENDLY_NAME = NULL;
            }
            WE_FREE(SEC_USER_CERT);
            SEC_USER_CERT = NULL;
            Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState5
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState5(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iStatus = 0;
        
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_DpHandleWimInit(hSecHandle, stStateParam.iTargetID, FALSE);
    switch(iStatus)
    {
        case M_SEC_ERR_IN_PROGRESS:
            break;
        case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
            Sec_iUeWarning(hSecHandle,stStateParam.iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, SEC_USER_CERT, SEC_USER_CERT_LEN);
            SEC_STATES--;
            break;
        case M_SEC_ERR_OK:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, SEC_USER_CERT, SEC_USER_CERT_LEN);
            SEC_STATES--;
            break;
        default:
            if (SEC_FRIENDLY_NAME)
            {
                WE_FREE(SEC_FRIENDLY_NAME);
                SEC_FRIENDLY_NAME = NULL;
            }
            WE_FREE(SEC_USER_CERT);
            SEC_USER_CERT = NULL;
            Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iStatus);
            Sec_DpHandleCleanUp(hSecHandle);
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState6
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState6(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iStatus = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    iStatus = Sec_HandleOldRootCert(hSecHandle,stStateParam.iTargetID, SEC_TOO_OLD_CERT_ID);

    if (M_SEC_ERR_IN_PROGRESS == iStatus)
    {
    }
    else
    {
        SEC_WAITING_FOR_WIM = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimWtlsChkUCertChain(hSecHandle, stStateParam.iTargetID, SEC_USER_CERT, SEC_USER_CERT_LEN);                          
        SEC_STATES -= 2;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState7
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState7(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_VOID  *pvDisplayName     = NULL;
    WE_UINT8 *pucCert           = NULL;
    WE_UINT8 *pucCaKeyHash      = NULL;
    WE_UINT8 *pucSubjectKeyHash = NULL;
    WE_INT16  sCharacterSet     = 0;
    WE_UINT16 usCertLen         = 0;
    WE_INT32  iDisplayNameLen   = 0;
    WE_INT32  iRes              = M_SEC_ERR_OK;
    E_SecHashAlgType   eHashAlg = E_SEC_ALG_HASH_SHA1;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    iRes = Sec_CertVerifyRsakey(hSecHandle, stStateParam.iTargetID, eHashAlg, SEC_USER_CERT);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    SEC_STATES++;
    iRes = Sec_X509CertParseResponse(hSecHandle, stStateParam.pucCert, 
                    &sCharacterSet, &pvDisplayName, 
                    &iDisplayNameLen, &pucCaKeyHash, &pucSubjectKeyHash,
                    &pucCert, &usCertLen);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
       
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        if (!SEC_WAITING_FOR_WIM)
        {
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
        }
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    if (M_SEC_ERR_OK != Sec_CertHashCompares(SEC_CA_PUBLIC_KEY_HASH, pucCaKeyHash, SEC_PUBLIC_KEY_HASH, pucSubjectKeyHash))
    {
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState8
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState8(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertFindPrivKeyResp(hSecHandle);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimCertExists(hSecHandle, stStateParam.iTargetID, SEC_USER_CERT, SEC_USER_CERT_LEN, M_SEC_UE_USER_CERT);
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState9
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState9(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes     = M_SEC_ERR_OK;
    WE_INT32  iExists = 0;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertAlreadExistsResp(hSecHandle,stStateParam.iTargetID, &iExists, M_SEC_UE_USER_CERT);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    if (!iExists)
    {
        SEC_STATES += 3;
    }
    else
    {
        SEC_STATES++;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState10
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState10(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDelete(hSecHandle, M_SEC_UE_USER_CERT);
    SEC_USER_FILE_ID = 0;
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        if (iRes == USER_CANCEL)
        {
            Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        }
        else
        {
            Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        }
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState11
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState11(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDeleteResp(hSecHandle);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        WE_FREE(SEC_USER_CERT);
        SEC_USER_CERT = NULL;
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    SEC_STATES++;
}

/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState12
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState12(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    if (NULL == hSecHandle)
    {
        return;
    }   

    SEC_WAITING_FOR_WIM = 1;
    SEC_WANTS_TO_RUN = 0;
    Sec_iWimSaveCert(hSecHandle, stStateParam.iTargetID, SEC_USER_CERT, SEC_USER_CERT_LEN, 
                            M_SEC_UE_USER_CERT, SEC_FRIENDLY_NAME, SEC_FRIENDLY_NAME_LEN , 
                            SEC_CHAR_SET, (WE_UCHAR *)"\x00");
    WE_FREE(SEC_USER_CERT);
    SEC_USER_CERT = NULL;
    SEC_STATES++;
}
/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState13
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState13(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;
    
    if (NULL == hSecHandle)
    {
        return;
    }   
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }    

    if (!SEC_CUR_WIM_RESP)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }    

    iRes = Sec_CertStoreResp(hSecHandle, stStateParam.iTargetID);
    if (M_SEC_ERR_OK != iRes)
    {
        if (SEC_FRIENDLY_NAME)
        {
            WE_FREE(SEC_FRIENDLY_NAME);
            SEC_FRIENDLY_NAME = NULL;
        }
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, iRes);
        if (!SEC_WAITING_FOR_WIM)
        {
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
        }
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    if (SEC_FRIENDLY_NAME)
    {
        WE_FREE(SEC_FRIENDLY_NAME);
        SEC_FRIENDLY_NAME = NULL;
    }
    SEC_STATES++;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState14
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState14(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertStoreDataResp(hSecHandle);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    if (M_SEC_ERR_OK != iRes)
    {
        SEC_WAITING_FOR_WIM = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimDelCert(hSecHandle, (WE_INT32)SEC_CERT_ID);
        SEC_STATES += 2;
        return;
    }
    SEC_WAITING_FOR_UE = 1;
    SEC_WANTS_TO_RUN = 0;

    Sec_iUeConfirm(hSecHandle, stStateParam.iTargetID,M_SEC_ID_CONFIRM_7, M_SEC_ID_OK, M_SEC_ID_NONE);
    SEC_STATES++;   
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState15
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState15(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_UE)
    {
        return;
    }
    
    if (!SEC_CUR_UE_RESP)
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }

    Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_OK);
    Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
    SEC_CUR_UE_RESP = NULL;
    Sec_DpHandleCleanUp(hSecHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryState16
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    St_SecCertStateParam stStateParam [IN]: Structure of parameter.
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
static WE_VOID Sec_CertDeliveryState16(WE_HANDLE hSecHandle, St_SecCertStateParam stStateParam)
{
    WE_INT32 iRes = M_SEC_ERR_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (SEC_WAITING_FOR_WIM)
    {
        return;
    }
    
    if (!SEC_CUR_WIM_RESP)
    {
        Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        Sec_DpHandleCleanUp(hSecHandle);
        return;
    }
    
    iRes = Sec_CertDeleteResult(hSecHandle);
    if(M_SEC_ERR_OK != iRes )
    {
    }
    /* modify by Sam [070126] */
    Sec_CertDeliveryResp(hSecHandle, stStateParam.iTargetID, M_SEC_ERR_GENERAL_ERROR);
    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
    SEC_CUR_WIM_RESP = NULL;
    Sec_DpHandleCleanUp(hSecHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_InitDeliveryStateForm
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize the state-form of delivery the certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_InitDeliveryStateForm(WE_HANDLE hSecHandle)
{
    Fn_SecCertStateFunc *pfnStateForm = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pfnStateForm = (Fn_SecCertStateFunc *)WE_MALLOC(sizeof(Fn_SecCertStateFunc) * DELIVERY_STATE_MAX);
    if (NULL == pfnStateForm)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pfnStateForm[0] = NULL;
    pfnStateForm[1] = Sec_CertDeliveryState1;
    pfnStateForm[2] = Sec_CertDeliveryState2;
    pfnStateForm[3] = Sec_CertDeliveryState3;
    pfnStateForm[4] = Sec_CertDeliveryState4;
    pfnStateForm[5] = Sec_CertDeliveryState5;
    pfnStateForm[6] = Sec_CertDeliveryState6;
    pfnStateForm[7] = Sec_CertDeliveryState7;
    pfnStateForm[8] = Sec_CertDeliveryState8;
    pfnStateForm[9] = Sec_CertDeliveryState9;
    pfnStateForm[10] = Sec_CertDeliveryState10;
    pfnStateForm[11] = Sec_CertDeliveryState11;
    pfnStateForm[12] = Sec_CertDeliveryState12;
    pfnStateForm[13] = Sec_CertDeliveryState13;
    pfnStateForm[14] = Sec_CertDeliveryState14;
    pfnStateForm[15] = Sec_CertDeliveryState15;
    pfnStateForm[16] = Sec_CertDeliveryState16;

    if (NULL != SEC_DELIVERY_STATE_FORM)
    {
        WE_FREE(SEC_DELIVERY_STATE_FORM);
    }
    SEC_DELIVERY_STATE_FORM = pfnStateForm;

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleCertDelivery
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Delivery the certificat.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_UCHAR *pucCertResp[IN]: Poniter to  the response value.
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
WE_VOID Sec_HandleCertDelivery(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UCHAR *pucCertResp)
{
    WE_INT32 iStates = 0;
    Fn_SecCertStateFunc *pfnStateForm = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }

    pfnStateForm = (Fn_SecCertStateFunc *)SEC_DELIVERY_STATE_FORM;
    if (NULL == pfnStateForm)
    {
        return;
    }
   
    iStates = SEC_STATES;
    if ((iStates >= 1) && (iStates < DELIVERY_STATE_MAX))
    {
        St_SecCertStateParam stStateParam = {0};

        stStateParam.iTargetID = iTargetID;
        stStateParam.pucCert = pucCertResp;
        pfnStateForm[iStates](hSecHandle, stStateParam);
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStateFormInit
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Initialize all state-form in the certificate manage.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_NOT_IMPLEMENTED: Not implemented.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertStateFormInit(WE_HANDLE hSecHandle)
{
    WE_INT32 iRes = M_SEC_ERR_OK;
    
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRes += Sec_InitStoreCaStateForm(hSecHandle);
    iRes += Sec_InitStoreUserStateForm(hSecHandle);
    iRes += Sec_InitHashVerifyStateForm(hSecHandle);
    iRes += Sec_InitSignVerifyStateForm(hSecHandle);
    iRes += Sec_InitDeliveryStateForm(hSecHandle);

    if(M_SEC_ERR_OK != iRes)
    {
        iRes = Sec_CertStateFormTerminate(hSecHandle);
        return M_SEC_ERR_NOT_IMPLEMENTED;
    }

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_CertStateFormTerminate
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Terminite the certificate manage, free the memory for the state-form.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_CertStateFormTerminate(WE_HANDLE hSecHandle)
{
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (NULL != SEC_STORECA_STATE_FORM)
    {
        WE_FREE(SEC_STORECA_STATE_FORM);
        SEC_STORECA_STATE_FORM = NULL;
    }
    
    if (NULL != SEC_STOREUSER_STATE_FORM)
    {
        WE_FREE(SEC_STOREUSER_STATE_FORM);
        SEC_STOREUSER_STATE_FORM = NULL;
    }

    if (NULL != SEC_HASHVERIFY_STATE_FORM)
    {
        WE_FREE(SEC_HASHVERIFY_STATE_FORM);
        SEC_HASHVERIFY_STATE_FORM = NULL;
    }

    if (NULL != SEC_SIGNVERIFY_STATE_FORM)
    {
        WE_FREE(SEC_SIGNVERIFY_STATE_FORM);
        SEC_SIGNVERIFY_STATE_FORM = NULL;
    }

    if (NULL != SEC_DELIVERY_STATE_FORM)
    {
        WE_FREE(SEC_DELIVERY_STATE_FORM);
        SEC_DELIVERY_STATE_FORM = NULL;
    }

    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleGetCertNames
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get name from a certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_INT32 iCertOptions[IN]: Options of certificate.
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
WE_VOID Sec_HandleGetCertNames(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertOptions)
{
    St_SecTlCertNamesResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    switch (SEC_STATES)
    {
        case 0:
            break;
        case 1:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimViewNameOfCert(hSecHandle, iCertOptions);
            SEC_STATES++;
            break;
        case 2:
            if (SEC_WAITING_FOR_WIM)
            {
                return;
            }
            
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_GetCertNameListResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            
            pstResp = (St_SecTlCertNamesResp *)SEC_CUR_WIM_RESP;
            if (pstResp->eType != E_SEC_DP_GET_CERT_NAMES)
            {
                Sec_GetCertNameListResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            if(pstResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++;
            }
            else
            {/* add by Sam [070126] */
                if (pstResp->iResult != M_SEC_ERR_OK)
                {
                    Sec_GetCertNameListResp(hSecHandle, iTargetID, pstResp->iResult);
                    Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                    SEC_CUR_WIM_RESP = NULL;
                    Sec_DpHandleCleanUp(hSecHandle);
                    return;
                }
                if (NULL == SEC_CERTNAMES)
                {         
                    Sec_iUeShowCertList(hSecHandle, iTargetID,pstResp->iNbrOfCerts, pstResp->pstCertNames);
                    Sec_GetCertNameListResp(hSecHandle, iTargetID, M_SEC_ERR_OK);
                }
                else
                {
                    Sec_GetCertNameListResp(hSecHandle, iTargetID, M_SEC_ERR_BUSY);
                }
                Sec_DpHandleCleanUp(hSecHandle);
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case 3:
            {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 0, FALSE);
            switch (iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                    break;
                case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
                    SEC_WAITING_FOR_WIM = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimViewNameOfCert(hSecHandle, iCertOptions);
                    SEC_STATES--;
                    break;
                case M_SEC_ERR_OK:
                    SEC_WAITING_FOR_WIM = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimViewNameOfCert(hSecHandle, iCertOptions);
                    SEC_STATES--;
                    break;
                default:
                    Sec_GetCertNameListResp(hSecHandle, iTargetID, iStatus);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
            }
            }
            break;
        default:
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleGetCert
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Get certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_INT32 iCertId     [IN]: ID of certificate.
    WE_INT32 iOptions    [IN]: Options.
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
WE_VOID Sec_HandleGetCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId, WE_INT32 iOptions)
{
    WE_VOID  *pvIssuer         = NULL;
    WE_VOID  *pvSubject        = NULL;
    WE_INT16  sIssuerCharSet   = 0;
    WE_INT16  sSubjectCharSet  = 0;
    WE_INT32  iIssuerLen       = 0;
    WE_INT32  iSubjectLen      = 0;
//    WE_INT32  iFingerprintLen  = 0;
    WE_UINT32 uiValidNotBefore = 0;
    WE_UINT32 uiValidNotAfter  = 0;

    St_SecTlGetCertResp *pstResp = NULL;
    /*add by bird 061107 for new UI spec*/
    WE_UCHAR* pucSerialNumber = NULL;/*serial number*/
    WE_UCHAR* pucAlg = NULL;/*signature algorthm*/
    WE_UCHAR* pucPubKey = NULL;/*public key*/   

     WE_UINT16 usSerialNumberLen= 0;
     WE_UINT16 usPubKeyLen = 0;  

    if (NULL == hSecHandle)
    {
        return;
    }
    
    switch (SEC_STATES)
    {
        case 0:
            break;
        case 1:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimGetCert(hSecHandle, iCertId);
            SEC_STATES++;
            break;
        case 2:
            if (SEC_WAITING_FOR_WIM)
            {
                return;
            }
            
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_CertGetResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                        iCertId, NULL, 0, iOptions);
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            
            pstResp = (St_SecTlGetCertResp *)SEC_CUR_WIM_RESP;
            if (E_SEC_DP_GET_CERT != pstResp->eType)
            {
                Sec_CertGetResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                                        iCertId, NULL, 0, iOptions);
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            
            if (M_SEC_ERR_WIM_NOT_INITIALISED == pstResp->iResult)
            {
                SEC_STATES++;
            }
            else
            {
                if ((NULL == pstResp->pucCert) || (0 == pstResp->usCertLen)) 
                {
                    Sec_CertGetResp(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, 
                                            iCertId, NULL, 0, iOptions);
                }                
                else if (M_SEC_ERR_OK != Sec_CertGetItemsForUe(hSecHandle, pstResp->pucCert, &sIssuerCharSet, 
                     &pvIssuer, &iIssuerLen, &sSubjectCharSet, 
                     &pvSubject, &iSubjectLen, 
                     &uiValidNotBefore, &uiValidNotAfter,
                     &pucSerialNumber, &usSerialNumberLen,
                     &pucAlg,
                     &pucPubKey,&usPubKeyLen))
                {
                    Sec_CertGetResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
                                            iCertId, NULL, 0, iOptions);
                }
                else
                {

                    if (!iOptions)
                    {
                        Sec_iUeShowCertContent(hSecHandle,iTargetID, iCertId, sIssuerCharSet, pvIssuer, iIssuerLen,
                                        uiValidNotBefore, uiValidNotAfter, 
                                        sSubjectCharSet, pvSubject, iSubjectLen,
                                        pucSerialNumber, usSerialNumberLen, 
                                        pucAlg,pucPubKey,usPubKeyLen,
                                        M_SEC_ID_DELETE);
                    }
                    Sec_CertGetResp(hSecHandle, iTargetID, M_SEC_ERR_OK, 
                                    iCertId, pstResp->pucCert, pstResp->usCertLen, iOptions);
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
                Sec_DpHandleCleanUp(hSecHandle);
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case 3:
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iCertId, FALSE);
            switch (iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                    break;
                case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
                    SEC_WAITING_FOR_WIM = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimGetCert(hSecHandle, iCertId);
                    SEC_STATES--;
                    break;
                case M_SEC_ERR_OK:
                    SEC_WAITING_FOR_WIM = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimGetCert(hSecHandle, iCertId);
                    SEC_STATES--;
                    break;
                default:
                    Sec_CertGetResp(hSecHandle, iTargetID, iStatus,
                                    iCertId, NULL, 0, iOptions);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
            }
        }
            break;
        default:
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleDeleteCert
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Delete the certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Global data handle.
    WE_INT32 iTargetID   [IN]: ID of target.
    WE_INT32 iCertId     [IN]: ID of certificate.
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
WE_VOID Sec_HandleDeleteCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId)
{
    St_SecTlRmCertResp *pstResp = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    switch (SEC_STATES)
    {
        case 0:
            break;
#ifdef M_SEC_CFG_PIN_DELETE_CERT
        case 1:
        {
            WE_INT32 iStatus = Sec_DpHandleUserVerify(hSecHandle, iCertId, E_SEC_DELETE_CERT);
            switch (iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                    break;
                case M_SEC_ERR_OK:
                    SEC_WAITING_FOR_WIM = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimDelCert(hSecHandle, iCertId);
                    SEC_STATES++;
                    break;
                case M_SEC_ERR_USER_NOT_VERIFIED:
                    
                default:
                    Sec_DeleteCertResponse(hSecHandle, iTargetID, iCertId, iStatus);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
            }
        }
            break;
#else
        case 1:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimDelCert(hSecHandle, iCertId);
            SEC_STATES++;
            break;
#endif
        case 2:
            if (SEC_WAITING_FOR_WIM)
            {
                return;
            }
            
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_DeleteCertResponse(hSecHandle, iTargetID, iCertId, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            
            pstResp = (St_SecTlRmCertResp *)SEC_CUR_WIM_RESP;
            if (E_SEC_DP_RM_CERT != pstResp->eType)
            {
                Sec_DeleteCertResponse(hSecHandle, iTargetID, iCertId, M_SEC_ERR_GENERAL_ERROR);
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            
            if (M_SEC_ERR_WIM_NOT_INITIALISED == pstResp->iResult)
            {
                SEC_STATES++;
            }
            else
            {
                Sec_DeleteCertResponse(hSecHandle, iTargetID, pstResp->iCertId, pstResp->iResult);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case 3:
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iCertId, FALSE);
            switch (iStatus)
            {
                case M_SEC_ERR_IN_PROGRESS:
                    break;
                case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM);
#endif
                    SEC_WAITING_FOR_WIM = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimDelCert(hSecHandle, iCertId);
                    SEC_STATES--;
                    break;
                case M_SEC_ERR_OK:
                    SEC_WAITING_FOR_WIM = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimDelCert(hSecHandle, iCertId);
                    SEC_STATES--;
                    break;
                default:
                    Sec_DeleteCertResponse(hSecHandle, iTargetID, iCertId, iStatus);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
            }
        }
            break;
        default:
            break;
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_HandleOldRootCert
CREATE DATE:
    2006-07-08
AUTHOR:
    Jabber Wu
DESCRIPTION:
    Process the old root certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle      [IN]: Global data handle.
    WE_INT32 iTooOldRootCertId[IN]: ID of old root certificate.
RETURN VALUE:
    M_SEC_ERR_OK: If task is successful.
    M_SEC_ERR_INVALID_PARAMETER: Invalid parameter.
    M_SEC_ERR_INSUFFICIENT_MEMORY: Insufficient memory.
    M_SEC_ERR_GENERAL_ERROR: General error.
    M_SEC_ERR_IN_PROGRESS: In progress.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_HandleOldRootCert(WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_INT32 iTooOldRootCertId)
{
    WE_VOID  *pvIssuer         = NULL;
    WE_VOID  *pvSubject        = NULL;
    WE_INT16  sSubjectCharSet  = 0;
    WE_INT16  sIssuerCharSet   = 0;
    WE_INT32  iIssuerLen       = 0;
    WE_INT32  iSubjectLen      = 0;
    WE_INT32  iStatus          = M_SEC_ERR_OK;
    WE_UINT32 uiValidNotAfter  = 0;
    WE_UINT32 uiValidNotBefore = 0;

    St_SecTlConfirmDlgResp *pstTiConfirmDialogResp = NULL;
    St_SecTlRmCertResp     *pstWimDeleCertResp     = NULL;
    St_SecTlGetCertResp    *pstWimGetCertResp      = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    switch (SEC_STEP_LOCAL_ROOT)
    {
        case 0:
            SEC_WAITING_FOR_WIM = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimGetCert(hSecHandle, iTooOldRootCertId);
            SEC_STEP_LOCAL_ROOT += 1;
            iStatus = M_SEC_ERR_IN_PROGRESS;    
            break;
        case 1:
            if (SEC_WAITING_FOR_WIM)
            {
                return M_SEC_ERR_IN_PROGRESS;
            }
            
            if (!SEC_CUR_WIM_RESP)
            {
                SEC_STEP_LOCAL_ROOT = 0;
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            pstWimGetCertResp = (St_SecTlGetCertResp *)SEC_CUR_WIM_RESP;
            if (E_SEC_DP_GET_CERT != pstWimGetCertResp->eType)
            {
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                SEC_STEP_LOCAL_ROOT = 0;
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            iStatus = pstWimGetCertResp->iResult;
            if (M_SEC_ERR_OK == pstWimGetCertResp->iResult)
            {
                if ((NULL == pstWimGetCertResp->pucCert) || (0 == pstWimGetCertResp->usCertLen))
                {
                    iStatus = M_SEC_ERR_INSUFFICIENT_MEMORY;
                    SEC_STEP_LOCAL_ROOT = 0;
                }
                else if (M_SEC_ERR_OK != Sec_CertGetItemsForUe(hSecHandle, pstWimGetCertResp->pucCert, &sIssuerCharSet, 
                    &pvIssuer, &iIssuerLen, &sSubjectCharSet, 
                    &pvSubject, &iSubjectLen, 
                    &uiValidNotBefore, &uiValidNotAfter,
                    NULL, NULL,NULL,NULL,NULL))
                {
                    iStatus = M_SEC_ERR_INSUFFICIENT_MEMORY;
                    SEC_STEP_LOCAL_ROOT = 0;
                }
                else
                {
                    SEC_WAITING_FOR_UE = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iUeNameConfirm(hSecHandle, iTargetId,sSubjectCharSet, pvSubject, iSubjectLen, 
                                        M_SEC_ID_CONFIRM_13, M_SEC_ID_OK, M_SEC_ID_CANCEL);    
                    iStatus = M_SEC_ERR_IN_PROGRESS;
                }
            }
            Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            SEC_STEP_LOCAL_ROOT += 1;
            break;
        case 2:
            if (SEC_WAITING_FOR_UE)
            {
                return M_SEC_ERR_IN_PROGRESS;
            }
            
            if (!SEC_CUR_UE_RESP)
            {
                SEC_STEP_LOCAL_ROOT = 0;
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            pstTiConfirmDialogResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
            if (E_SEC_DP_CONFIRM_DLG != pstTiConfirmDialogResp->eType)
            {
                Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
                SEC_CUR_UE_RESP = NULL;
                SEC_STEP_LOCAL_ROOT = 0;
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            if (pstTiConfirmDialogResp->ucAnswer)
            {
                SEC_WAITING_FOR_WIM = 1;
                SEC_WANTS_TO_RUN = 0;
                Sec_iWimDelCert(hSecHandle, iTooOldRootCertId);
                SEC_STEP_LOCAL_ROOT += 1;
                iStatus = M_SEC_ERR_IN_PROGRESS;
            }
            else
            {
                iStatus = M_SEC_ERR_GENERAL_ERROR;
                SEC_STEP_LOCAL_ROOT = 0;
            }
            Sec_TlHandleUeStructFree((St_SecTlUeFunctionId *)SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            break;
        case 3:
            if (SEC_WAITING_FOR_WIM)
            {
                return M_SEC_ERR_IN_PROGRESS;
            }

            SEC_STEP_LOCAL_ROOT = 0;
            if (!SEC_CUR_WIM_RESP)
            {
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            pstWimDeleCertResp = (St_SecTlRmCertResp *)SEC_CUR_WIM_RESP;
            if (E_SEC_DP_RM_CERT != pstWimDeleCertResp->eType)
            {
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            if (M_SEC_ERR_OK != pstWimDeleCertResp->iResult)
            {
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                iStatus = M_SEC_ERR_GENERAL_ERROR;
            }
            else
            {
                Sec_TlHandleWimStructFree((St_SecTlWimFunctionId *)SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                iStatus = M_SEC_ERR_OK;
            }
            break;
        default:
            SEC_STEP_LOCAL_ROOT = 0;
            iStatus = M_SEC_ERR_GENERAL_ERROR;
            break;
    }
    
    return iStatus;
}



/*==================================================================================================
    HEADER NAME : sec_wim.h
    MODULE NAME : SEC
        
    GENERAL DESCRIPTION
        In this file,define the function prototype 
    
    Techfaith Software Confidential Proprietary
    (c) Copyright 2006 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   --------------------------------------------------
    2006-07-16  tang               None         Init
     
 =================================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_WIM_H
#define SEC_WIM_H

/***************************************************************************************************
*   macro Define Section
***************************************************************************************************/
#define M_SEC_SESSION_ID_LEN                32
#define M_SEC_PRIVATE_KEY_ID_LEN            4
#define M_SEC_ADDRESS_LEN                   18
#define M_SEC_WTLS_MASTER_SECRET_LEN        20

#define M_SEC_CERT_ID_LEN                   4 
#define M_SEC_CERT_COUNT_LEN                4
#define M_SEC_PUB_HASH_LEN                  20
#define M_SEC_DN_HASH_LEN                   20

#define M_SEC_PRIVKEY_COUNT_LENGTH          4

/*Root Dir*/
#define M_SEC_FILE_PATH                      "/sec"
/*DIR*/
#define M_SEC_CACERT_DIR                        "/sec/ca_cert"
#define M_SEC_USER_CERT_DIR                     "/sec/usercert"
#define M_SEC_CONTRACT_DIR                      "/sec/contract"
#define M_SEC_NON_VERIFIED_CACERT_DIR           "/sec/nonverifiedcacert"
#define M_SEC_WTLS_NON_VERIFIED_CACERT_DIR      "/sec/nonverifiedcacert/wtls"
#define M_SEC_X509_NON_VERIFIED_CACERT_DIR      "/sec/nonverifiedcacert/x509"
/*PREFIX */
#define M_SEC_CA_CERT_FILENAME_PREFIX           "/sec/ca_cert/c_"
#define M_SEC_USERCERT_FILENAME_PREFIX          "/sec/usercert/u_"
#define M_SEC_CONTRACT_FILENAME_PREFIX          "/sec/contract/t_"

#define M_SEC_CA_CERT_DIR_PREFIX           "/sec/ca_cert/"
#define M_SEC_USERCERT_DIR_PREFIX          "/sec/usercert/"
#define M_SEC_CONTRACT_DIR_PREFIX          "/sec/contract/"

#define M_SEC_WTLS_NON_VERIFIED_CACERT_PREFIX   "/sec/nonverifiedcacert/wtls/"
#define M_SEC_X509_NON_VERIFIED_CACERT_PREFIX   "/sec/nonverifiedcacert/x509/"

/*Index File Name*/
#define M_SEC_USERCERT_INDEXFILE_NAME     "/sec/ucert_index"
#define M_SEC_CA_CERT_INDEXFILE_NAME      "/sec/cacert_index"
#define M_SEC_CONTRACT_INDEXFILE_NAME     "/sec/contract_index"
/*private key name*/
#define M_SEC_AUTH_PRIVKEY_NAME                "/sec/auth_privkey"
#define M_SEC_NON_REP_PRIVKEY_NAME             "/sec/nonrep_privkey"
/*User PrivateKey Index File Name*/
#define M_SEC_USERCERT_PRIVKEY_INDEX_NAME      "/sec/user_privkey"
/* session, peer and view information*/
#define M_SEC_SH_NAME                          "/sec/session"

/*File Type*/
#define M_SEC_CA_CERT_HEADER                   'C'
#define M_SEC_USER_CERT_HEADER                 'U'

#define M_SEC_WTLS_CA_CERT_DISABLE             0x00
#define M_SEC_WTLS_CA_CERT_ENABLE              0x01

#define M_SEC_X509_USER_CERT                   0x01
#define M_SEC_WTLS_USER_CERT                   0x02
#define M_SEC_X509_CA_CERT                     0x03
#define M_SEC_WTLS_CA_CERT                     0x04
#define M_SEC_PUBKEY_CERT                      0x05
#define M_SEC_CONTRACT                         0x06
#define M_SEC_NON_VERIFIED_WTLS_TYPE           0x07
#define M_SEC_NON_VERIFIED_X509_TYPE           0x08

/*file ID Scope*/
#define M_SEC_USER_CERT_ID_START                  1
#define M_SEC_CA_CERT_ID_START                   60
#define M_SEC_CONTRACT_CERT_ID_START            120
#define M_SEC_CERT_ID_END                       250


/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef struct tagSt_WimUCertKeyPairInfo 
{            
    WE_INT32     iUcertId;            
    WE_INT32     iUcertCount;
    WE_INT32     iPkeyCount; 
    WE_UINT8     ucKeyUsage; 
    WE_UINT8     aucPublicKeyHash[M_SEC_KEY_HASH_SIZE];
    WE_UINT8     aucCApublicKeyHash[M_SEC_KEY_HASH_SIZE];   
    WE_UINT8     ucPublicKeyCert;    
    WE_INT32     iNameCount;             
    WE_INT8      cNameType;                 
}St_WimUCertKeyPairInfo;

typedef struct tagSt_SecWimPSRCertInfo 
{
    WE_INT32    iFileSize;
    WE_CHAR     *pcFileName;
    WE_CHAR     cCertType;
} St_SecWimPSRCertInfo;
  
/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
WE_INT32 Sec_WimMakeFile(WE_HANDLE hSecHandle,WE_INT8 cFileType,WE_HANDLE *phFileHandle, WE_UINT32 *puiFileId);
WE_INT32 Sec_WimAcqFileName(WE_INT8 cFileType,WE_UINT32 uiFileId, WE_CHAR **ppcFileName);
WE_INT32 Sec_WimDelFile(WE_HANDLE hSecHandle,WE_INT8 cFileType,WE_INT32 iFileId);
WE_INT32 Sec_WimCheckFileExist(WE_HANDLE hSecHandle,WE_CHAR *pcFileName);
WE_INT32 Sec_WimSavePrivateKey(WE_HANDLE hSecHandle,WE_UINT8 *pucPrivkey,
                               WE_INT32 iPrivkeyLen,WE_UINT8 ucKeyType, const WE_INT8 *pcPin,WE_UINT32 *puiCountOut);
WE_INT32 Sec_WimStorePin(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType,const WE_INT8 *pcNewPin);
WE_INT32 Sec_WimEncryptDataToFile(WE_HANDLE hSecHandle,
                                  WE_HANDLE hFileHandle, 
                                  void *pvData, 
                                  WE_INT32 iPos,
                                  WE_INT32 iDataLen,
                                  WE_INT32 *piWriteLength);
WE_INT32 Sec_WimDecryptDataFromFile(WE_HANDLE hSecHandle,
                                    WE_HANDLE hFileHandle, 
                                    WE_UINT8 *pucDecryptedData, 
                                    WE_INT32 iPos, 
                                    WE_INT32 iDataLen,
                                    WE_INT32 *piReaderLength);  
WE_INT32 Sec_WimCheckPin(WE_HANDLE hSecHandle,WE_UINT8 ucKeyType, const WE_CHAR *pcPin);
WE_INT32 Sec_WimReadPin(WE_HANDLE hSecHandle,WE_UINT8 ucKeyType, WE_CHAR *pcStoredPin); 

WE_INT32 Sec_WimAddBlock(WE_HANDLE hSecHandle,WE_INT8 cFileType,WE_UINT32 uiCertId, 
                         WE_UINT32 uiCount,const WE_UINT8 *pucDNHash ,const WE_UINT8 * pucPubHash,
                         WE_INT8 cNameType,WE_INT16 sCharSet,WE_UINT16 usFriendlyNameLen,
                         const WE_UINT8 *pucFriendlyName, WE_UINT8 ucAble);
WE_INT32 Sec_WimDelBlock(WE_HANDLE hSecHandle,WE_INT32 iInFileId,WE_INT8 cFileType);
WE_INT32 Sec_WimGenPubKeyAndUCertInfo(WE_HANDLE hSecHandle,WE_UINT8 *pucPubkey, WE_INT32 iPubkeyLen, 
                                      WE_UINT8 ucKeyType,  WE_INT32 iCountK);
WE_INT32 Sec_WimUpdataUCertKeyPairInfo(WE_HANDLE hSecHandle,WE_INT32 iUserCertId, 
                                       WE_INT32 iUserCertCount,  
                                       WE_INT32 iPrivKeyCount, WE_UINT8 ucPrivKeyUsage,
                                       const WE_UINT8 *pucPublicKeyHash, 
                                       const WE_UINT8 *pucCaPublicKeyHash, 
                                       WE_UINT8 ucPublicKeyCert, 
                                       WE_INT32 iNameCount, WE_INT8 cNameType);
void Sec_WimCheckNonVerifiedCert (WE_HANDLE hSecHandle,WE_INT32 iTargetID, WE_INT8 cCertType);
void Sec_WimCheckNonVerifiedCertResp (WE_HANDLE hSecHandle,
                                      St_SecWimElement *pstE,
                                      St_SecCrptVerifySignatureResp *pstParam);
WE_INT32 Sec_WimViewCertName(WE_HANDLE hSecHandle,
           WE_CHAR *pcFileName, WE_UINT8 *pucCertType, WE_INT32 *piCertId, WE_UINT8 **ppucCertFriendlyName, 
           WE_UINT16 *pusFriendlyNameLen, WE_INT16 *psCharSet, WE_INT8 *pcNameType,
           WE_INT8 *pcCertNum, WE_UCHAR *pucAble);
void Sec_WimSaveKeyPairInfoToFile(WE_HANDLE hSecHandle);
void Sec_WimClearKeyPairInfo(WE_HANDLE hSecHandle);
WE_INT32 Sec_WimGetOrigiLen(WE_UINT8 *pucDecryptedData, WE_INT32 iDataLen);
WE_INT32 Sec_WimGetMultiLen(WE_INT32 iDataLen);
WE_INT32 Sec_WimChkWTLSCert(WE_HANDLE hSecHandle,
                            WE_INT32 iTargetID, 
                            St_SecPubKeyRsa stKey, 
                            const WE_UINT8 *pucCert);

WE_INT32 Sec_WimChkX509Cert(WE_HANDLE hSecHandle,WE_INT32 iTargetID, 
                               WE_UINT8 *pucPublicKeyVal, 
                               const WE_UINT8 *pucCert, 
                               WE_INT32 iConnectionType,
                               WE_INT32 iChainDepth,
                               WE_INT32 iCertUsage);
WE_INT32 Sec_WimVerifiCertChain(WE_HANDLE hSecHandle,
                                const WE_UINT8 *pucChain, 
                                WE_INT32 iChainLen, 
                                St_SecAsn1Certificate *pstCerts, 
                                WE_UINT8 *pucNumCerts);
WE_INT32 Sec_WimGetX509Sub(WE_HANDLE hSecHandle,WE_UINT8 *pucX509Cert, 
                           WE_UINT8 **ppucSubject, WE_UINT16 *pusSubjectLen);
WE_INT32 Sec_WimGetX509PubKey(WE_HANDLE hSecHandle,WE_UINT8 *pucX509Cert,
                              WE_UINT16 *pusExpLen, WE_UINT8** ppucExponent,
                              WE_UINT16 *pusModLen,WE_UINT8** ppucModulus);
WE_INT32 Sec_WimGetPubKeyHash(WE_HANDLE hSecHandle,WE_INT32 iCertificateId, 
                              WE_UINT8 **ppucKeyId, WE_INT32 *piKeyIdLen);
WE_UINT8 Sec_WimIsInited(WE_HANDLE hSecHandle);
WE_INT32 Sec_WimCalHashedSign (WE_HANDLE  hSecHandle,
                               WE_INT32 iTargetID, 
                               St_SecCrptPrivKey stPrivKey, 
                               const WE_UINT8 *pucBuf, 
                               WE_INT32 iBufLen);
WE_INT32 Sec_WimGetPubKey(WE_HANDLE hSecHandle,WE_UINT8 *pucCert, St_SecPubKeyRsa *pstPubKey);
WE_INT32 Sec_WimVerifyHashedSign (WE_HANDLE  hSecHandle,
                                  WE_INT32 iHashAlg,
                                  WE_INT32 iTargetID,
                                  St_SecCrptPubKey stPubKey, 
                                  const WE_UINT8 *pucMsg,
                                  WE_INT32 iMsgLen, 
                                  const WE_UINT8 *pucSig, 
                                  WE_INT32 iSigLen);
WE_INT32 Sec_WimConvPubKey(St_SecPubKeyRsa stKey,
                           WE_UINT8 **ppucPubKey, WE_UINT16 *pusOutPubKeyLen);
void Sec_WimGetKeyPairInfo (WE_HANDLE hSecHandle);
WE_INT32 Sec_WimCleanUCertData(WE_HANDLE hSecHandle,WE_INT32 iUserCertId);

WE_INT32 Sec_WimGetSubKeyId(WE_HANDLE hSecHandle,WE_UINT8 *pucCert, WE_UINT8 **ppucKeyId, 
                            WE_INT32 *piKeyIdLen);
WE_INT32 Sec_WimGetPrivateKey(WE_HANDLE hSecHandle,WE_UINT8 ucKeyUsage, WE_UINT8 **ppucKey,WE_INT32 *piCertCount);
WE_INT32 Sec_WimGetCACert(WE_HANDLE hSecHandle,WE_INT32 iIndex, WE_UINT8 **ppucCert,
                          WE_UINT16 *pusCertLen, WE_UINT32 *puiFileId,WE_UINT8 *pucCertType, WE_UCHAR *pucAble);
void Sec_WimGetKeyExgIds(WE_HANDLE hSecHandle,
                         WE_UINT8 **ppucKeyExchangeIds, WE_INT32 *piKeyExIdLen);
WE_INT32 Sec_WimGetWTLSMasterSecret( WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId,
                                    const WE_UINT8 *pucSecret, WE_INT32 iSecretLen, 
                                    const WE_CHAR *pcLabel, const WE_UINT8 *pucSeed,
                                    WE_INT32 iSeedLen,WE_INT32 iOutputLen, WE_UINT8 *pucBuf,WE_UINT8 ucHashAlg);
WE_INT32 Sec_WimGetCertFormatByID(WE_HANDLE hSecHandle,
                                  WE_INT32 iCertId, WE_UINT8 *pucCertFormat);
WE_INT32 Sec_WimGetBlockLength(WE_UINT8 *pucP, 
                               WE_INT8 cFileType, WE_INT32 *piBlockLen);
WE_INT32 Sec_WimGetNbrOfCACerts(WE_HANDLE hSecHandle, WE_UINT8 *pucCACertNum);
WE_INT32 Sec_WimGetNbrUserCerts(WE_HANDLE hSecHandle);
WE_INT32 Sec_WimGetNbrOfWTLSDisableCACerts(WE_HANDLE hSecHandle,
                                    WE_INT32 *piNumWTLSDisableRootCerts);
void Sec_WimClearOneUCertKeyInfo(St_WimUCertKeyPairInfo *pstRec);

#endif /* end of SEC_WIM_H */




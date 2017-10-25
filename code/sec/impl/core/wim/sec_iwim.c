/*=====================================================================================
    FILE NAME :
        sec_iwim.c
    MODULE NAME :
        sec  
    GENERAL DESCRIPTION
    This File will handle the operations on the WIM, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
    =======================================================================================
    Revision History
      
    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-07-18      Stone An       None           create sec_iwim.c
        
=====================================================================================*/

/*******************************************************************************
*   Include File Section
*******************************************************************************/
#include "sec_comm.h"
#include "oem_sechandle.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define    SEC_GET_USER_CERT     0x01
#define    SEC_GET_ROOT_CERT     0x02

#define    M_SEC_CRPT_PKC_RSA    0x01
#define    MASTER_SECRET         "master secret"

#define    M_SEC_SIG_RSA_SHA     0x01
#define    M_SEC_SIG_RSA_MD5     0x02
#define    M_SEC_SIG_RSA_MD2     0x03

#define    M_SEC_CERT_USAGE_AUTH_SERVER        0x01
#define    M_SEC_CERT_USAGE_AUTH_CLIENT        0x02
#define    M_SEC_CERT_USAGE_CODE_SIGN          0x04
#define    M_SEC_CERT_USAGE_EMAIL_PROTECT      0x08
#define    M_SEC_CERT_USAGE_IPSEC_ENDSYSTEM    0x10
#define    M_SEC_CERT_USAGE_IPSEC_TUNNEL       0x20
#define    M_SEC_CERT_USAGE_IPSEC_USER         0x40
#define    M_SEC_CERT_USAGE_TIME_STAMPING      0x80

#define    SEC_WE_HANDLE                 (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->hWeHandle) 
#define    SEC_INITIALISE                (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->ucInitialised)
#define    SEC_USER_CERT_KEYPAIRS        (((ISec*)hSecHandle)->pstUserCertKeyPair)
#define    SEC_OLD_ROOT_CERT_COUNT       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iOldRootCertCount)
#define    SEC_BREW_ISHELL_HANDLE        (((ISec*)hSecHandle)->m_pIShell)
#define    SEC_BREW_CALLBACK_FNC         (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->m_cb)
#define    SEC_CALLER_TYPE               (((ISec*)hSecHandle)->iIFType) 
#define    SEC_NAME_TYPE                 (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->cNameType)
#define M_SEC_WIM_HAVELOADUSERPRIV        (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pcHaveLoad)

/*******************************************************************************
*   Type Define Section
*******************************************************************************/

typedef struct tagSt_SecWimWtlsKeyExRsa
{
    WE_INT32    iTargetID;
    WE_UCHAR    *pucMasterSecret;
} St_SecWimWtlsKeyExRsa;

typedef struct tagSt_SecWimGetMatchedPrivKey
{
    WE_INT32    iIndexI;
    St_SecRsaPublicKey stPubKey;
} St_SecWimGetMatchedPrivKey;

typedef struct tagSt_SecWimKeyPairGen
{
    WE_INT32     iTargetID; 
    WE_UINT8     ucKeyType;
    WE_CHAR      *pcPin;
    WE_UINT8     ucAlg;
    WE_UINT32    uiCountK;
    WE_UCHAR     *pucPubKey;
    WE_UINT16    usPubKeyLen;
}St_SecWimKeyPairGen;

typedef struct tagSt_SecWimSigntext
{
    WE_INT32     iTargetID; 
    WE_INT32     iSignId;
    WE_UINT8     ucAlgorithm;
    WE_CHAR      *pcHashedKey;
    WE_INT32     iHashedKeyLen;
    WE_UCHAR     *pucCert;
    WE_INT32     iCertLen;
    WE_INT32     iCertType;
    WE_INT32     iErr;
}St_SecWimSigntext;

typedef struct tagSt_SecWimGetUserCertAndPrivKey
{
    WE_INT32     iCertId;
    WE_UCHAR     *pucCert;
    WE_UINT16    usCertLen;
}St_SecWimGetUserCertAndPrivKey;

typedef struct tagSt_SecWimVerifyCertChain
{
    WE_INT32     iTargetID;
    WE_INT32     iConnType;
    WE_INT32     iUsage; 
    WE_INT32     iNumCerts;    
    WE_UINT8     ucAlg;
    WE_UINT16    uskeyExchKeyLen;   
    WE_UCHAR     *pucRootCert;
    WE_UINT16    usRootCertLen;
    WE_INT32     iCertIsOld;
    WE_INT32     iRootCertId;  
    WE_UINT16    usIssuerLen;
    WE_INT32     iIndex;
    St_SecAsn1Certificate   *pstCerts;
    St_SecCertificate       stCert;
} St_SecWimVerifyCertChain;

typedef struct tagSt_SecUserCertReq
{
    WE_UCHAR*           pucCertReqInfo;
    WE_UINT32           uiCertReqInfoLen;    
    E_SecRsaSigType     eType;
}St_SecUserCertReq;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 Sec_WimCheckDirExist(WE_HANDLE hSecHandle,const WE_INT8 *pcDirName);
static WE_VOID Sec_WimCheckDir(WE_HANDLE hSecHandle);
static WE_INT32 Sec_WimCheckAllIndexFile(WE_HANDLE hSecHandle);
static WE_INT32 Sec_WimCheckIndexFile(WE_HANDLE hSecHandle, 
                                     const WE_CHAR *pcIndexFileName);
static WE_VOID Sec_WimWtlsKeyExRsa(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                St_SecPubKeyRsa stPubKey, const WE_UCHAR *pucRandVal,WE_UINT8 ucAlg);
static WE_VOID Sec_WimVerifyCertChainStart(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                        WE_INT32 iNumCerts,
                                        St_SecAsn1Certificate *pstCerts, 
                                        const WE_UCHAR *pucBuf, WE_INT32 iBufLen,
                                        WE_INT32 iConnType, WE_INT32 iUsage);
static WE_VOID Sec_WimVerifyCertChainInter(WE_HANDLE hSecHandle, 
                                        St_SecWimVerifyCertChain *pstP);
static WE_VOID Sec_WimVerifyCertChainRoot(WE_HANDLE hSecHandle, 
                                       St_SecWimVerifyCertChain *pstP);
static WE_VOID Sec_WimVerifyCertChainDeliverResp(WE_HANDLE hSecHandle,
                                              St_SecWimVerifyCertChain *pstP,
                                              WE_INT32 iResult, WE_UCHAR *pucCaCert, 
                                              WE_UINT16 usCaCertLen,  WE_INT32 iRootCertIsOld, 
                                              WE_INT32 iRootCertIsOldId);
static WE_INT32 Sec_WimGetCertByID(WE_HANDLE hSecHandle, WE_INT32 iCertId, 
                                   WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen);
static WE_INT32 Sec_WimGetCertNameByID(WE_HANDLE hSecHandle,
                                       WE_INT32 iCertId, WE_UINT8 **ppucCertNames, 
                                       WE_UINT16 *pusNameLen, WE_INT16 *psCharSet,
                                       WE_INT8 *pcNameType);
static WE_INT32 Sec_WimStoreContract(WE_HANDLE hSecHandle, const WE_CHAR *pcText, 
                                     const WE_CHAR *pcSig, WE_UINT16 usSigLen, 
                                     WE_UINT32 uiTime);
WE_VOID Sec_WimDelPrivKey(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType);
static WE_VOID Sec_WimFreePubKey(St_SecRsaPublicKey stKey);
static WE_INT32 Sec_WimGetNumUserAndPubKeyCerts(WE_HANDLE hSecHandle);
static WE_VOID  Sec_WimWtlsKeyExNULL(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                                            const WE_UCHAR *pucRandVal,WE_UINT8 ucHashAlg);
static WE_INT32 Sec_WimBuildCertReqInfo(WE_HANDLE hSecHandle,
            WE_UCHAR ucExtKeyPurpose, WE_UCHAR *pucCountry,WE_UINT16 usCountryLen, 
            WE_UCHAR *pucProvince, WE_UINT16 usProvinceLen, WE_UCHAR *pucCity, 
            WE_UINT16 usCityLen, WE_UCHAR *pucCompany, WE_UINT16 usCompanyLen, 
            WE_UCHAR *pucDepartment, WE_UINT16 usDepartmentLen,WE_UCHAR *pucName, 
            WE_UINT16 usNameLen,WE_UCHAR *pucEMail, WE_UINT16 usEMailLen,
            WE_UCHAR ucKeyUsage, WE_UCHAR** ppucInfo,WE_UINT32* puiInfoLen);
static WE_UINT32 Sec_WimCountNameAttrDERLen(WE_UINT16 usAttrValueLen, WE_UINT16 usAttrTypeDERLen);
static WE_INT32 Sec_WimComposeUserCertReqMsg(WE_HANDLE hSecHandle, 
                                   WE_UCHAR *pucCertReqInfo, WE_UINT32 uiCertReqInfoLen, 
                                   E_SecRsaSigType eType, WE_UCHAR *pucSig, WE_UINT16 usSigLen,
                                   WE_UCHAR **ppucCertReqMsg, WE_UINT32 *puiCertReqMsgLen);
static WE_INT32 Sec_WimAddNameAttr(
                     WE_UCHAR *pucAttrValue, WE_UINT16 usAttrValueLen, 
                     WE_UCHAR *pucAttrTypeDer, WE_UINT16 usAttrTypeDERLen,
                     WE_UCHAR** ppucInfo, WE_UCHAR ucAttrValueType);
static WE_VOID Sec_WimAddDerTag(WE_UINT32 uiTag,
                            WE_UINT32 uiDerLen, WE_UCHAR** ppucInfo, WE_UCHAR ucTagType);
/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
        Sec_WimCheckDirExist
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Check whether directories exists or not.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.    
        pcDirName[IN]: name of directory.  
RETURN VALUE:
        M_SEC_ERR_OK: exist.
        other: not exist.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        Omit.
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimCheckDirExist(WE_HANDLE hSecHandle,const WE_INT8 *pcDirName)
{ 
    if((NULL == hSecHandle)||(NULL == pcDirName))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    return Sec_WimCheckFileExist(hSecHandle, (WE_CHAR*)pcDirName);
}

/*=====================================================================================
FUNCTION: 
        Sec_WimCheckDir
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Check whether all directories exist or not.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle. 
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        Omit.
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_VOID Sec_WimCheckDir(WE_HANDLE hSecHandle)
{
    WE_INT32       iResult = 0; 
    E_WE_ERROR     eRes = E_WE_OK; 
    
    if (NULL == hSecHandle)
    {
        return;
    }    

    iResult = Sec_WimCheckDirExist(hSecHandle, (const WE_INT8 *)M_SEC_FILE_PATH);
    if (iResult != M_SEC_ERR_OK)
    {
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_FILE_PATH);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_NON_VERIFIED_CACERT_DIR);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_WTLS_NON_VERIFIED_CACERT_DIR);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_X509_NON_VERIFIED_CACERT_DIR);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_CACERT_DIR);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_USER_CERT_DIR);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_CONTRACT_DIR);
        return;
    }
    iResult = Sec_WimCheckDirExist(hSecHandle, (const WE_INT8 *)M_SEC_NON_VERIFIED_CACERT_DIR);
    if (iResult != M_SEC_ERR_OK)
    {
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_NON_VERIFIED_CACERT_DIR);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_WTLS_NON_VERIFIED_CACERT_DIR);
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_X509_NON_VERIFIED_CACERT_DIR);
    }
    else
    {
        iResult = Sec_WimCheckDirExist(hSecHandle, (const WE_INT8 *)M_SEC_WTLS_NON_VERIFIED_CACERT_DIR);
        if (iResult != M_SEC_ERR_OK)
        {
            eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_WTLS_NON_VERIFIED_CACERT_DIR);
        }
        iResult = Sec_WimCheckDirExist(hSecHandle, (const WE_INT8 *)M_SEC_X509_NON_VERIFIED_CACERT_DIR);
        if (iResult != M_SEC_ERR_OK)
        {
            eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_X509_NON_VERIFIED_CACERT_DIR);
        }
    }
    iResult = Sec_WimCheckDirExist(hSecHandle, (const WE_INT8 *)M_SEC_CACERT_DIR);
    if (iResult != M_SEC_ERR_OK)
    {
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_CACERT_DIR);
    }
    iResult = Sec_WimCheckDirExist(hSecHandle, (const WE_INT8 *)M_SEC_USER_CERT_DIR);
    if (iResult != M_SEC_ERR_OK)
    {
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_USER_CERT_DIR);
    }
    iResult = Sec_WimCheckDirExist(hSecHandle, (const WE_INT8 *)M_SEC_CONTRACT_DIR);
    if (iResult != M_SEC_ERR_OK)
    {
        eRes = WE_FILE_MKDIR(SEC_WE_HANDLE, M_SEC_CONTRACT_DIR);
    }
 
    if (eRes != E_WE_OK)
    {
        return;
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_WimCheckAllIndexFile
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Check all index files.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle. 
RETURN VALUE:
        M_SEC_ERR_OK: success.
        other: fail.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        Omit.
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimCheckAllIndexFile(WE_HANDLE hSecHandle)
{
    WE_INT32 iResult = 0; 
    
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iResult = Sec_WimCheckIndexFile(hSecHandle, (const WE_CHAR *)M_SEC_USERCERT_INDEXFILE_NAME);
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    
    if ((WE_INT32)E_SEC_SIGNTEXT != SEC_CALLER_TYPE) /* for ISigntext, not initialize the CA index file.*/
    {
        iResult = Sec_WimCheckIndexFile(hSecHandle, (const WE_CHAR *)M_SEC_CA_CERT_INDEXFILE_NAME);
        if (iResult != M_SEC_ERR_OK)
        {
            return iResult;
        }
    }
    
    if ((WE_INT32)E_SEC_WTLS != SEC_CALLER_TYPE) /* for ISecW, not initialize the contract index file.*/
    {
        iResult = Sec_WimCheckIndexFile(hSecHandle, (const WE_CHAR *)M_SEC_CONTRACT_INDEXFILE_NAME);
        if (iResult != M_SEC_ERR_OK)
        {
            return iResult;
        }
    }
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimCheckIndexFile
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Check the index files.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.     
        pucIndexFileName[IN]: name of index file.  
RETURN VALUE:
        M_SEC_ERR_OK: success.
        other: fail.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        Omit.
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimCheckIndexFile(WE_HANDLE hSecHandle, 
                                     const WE_CHAR* pcIndexFileName)
{
    WE_HANDLE      hIndexFileHandle = NULL;
    E_WE_ERROR     eResult = E_WE_OK;  
    WE_INT32       iResult = 0;   
    E_WE_ERROR     eRes = E_WE_OK ;    
    WE_INT32       iIndexI = 0;    
    WE_INT32       iIndexJ = 0;    
    WE_CHAR        *pcFilePath = NULL;
    WE_INT32       iIndexFileSize = 0;
    WE_INT32       iRealIndexFileSize = 0;
    WE_INT32       iPastLen = 0;
    WE_INT32       iNumInIndexFile = 0;
    WE_INT32       iNumInFolder = 0;
    WE_INT32       iType = 0;
    WE_LONG        lSize = 0L;
    WE_CHAR        acNameBuf[20] = {0};
    WE_CHAR        *pcFileName = NULL;
    WE_CHAR        cFileType = 0;
    WE_INT32        iLeftLength = 0;
    WE_UINT32      uiIndexFileId = 0;
    WE_UINT32      uiCount = 0;
    WE_INT32       iNewPos = 0;
    WE_INT32       iBlockLen = 0;
    WE_INT32       iPos = 0;
    WE_UCHAR       *pucIndexFile = NULL;
    WE_UCHAR       *pucP = NULL;
    WE_UCHAR       *pucTemp = NULL;
    WE_CHAR        *pcPrefix = NULL;
    WE_CHAR        *pcCertPrefix = NULL;
    WE_CHAR        *pcDirFileName = NULL;
    
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    /* open or creat the indexfile.*/
    eResult = WE_FILE_GETSIZE(SEC_WE_HANDLE, pcIndexFileName, &(iIndexFileSize));
    if (E_WE_OK == eResult)
    {
        eResult = WE_FILE_OPEN(SEC_WE_HANDLE, (WE_CHAR *)pcIndexFileName, 
            WE_FILE_SET_RDWR, &hIndexFileHandle);
    }
    else 
    {
        eResult = WE_FILE_OPEN(SEC_WE_HANDLE, (WE_CHAR *)pcIndexFileName,
            WE_FILE_SET_CREATE, &hIndexFileHandle);
        iIndexFileSize = 0;
    }
    if(eResult != E_WE_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    /* get the path and type of indexfile. */
    if (SEC_STRCMP(M_SEC_USERCERT_INDEXFILE_NAME, pcIndexFileName) == 0)
    {
        pcFilePath = M_SEC_USER_CERT_DIR;    
        cFileType = M_SEC_USER_CERT_HEADER;
        pcPrefix = M_SEC_USERCERT_DIR_PREFIX;
        pcCertPrefix = "u_";
    }
    else if(SEC_STRCMP(M_SEC_CA_CERT_INDEXFILE_NAME, pcIndexFileName) == 0)
    {
        pcFilePath = M_SEC_CACERT_DIR;
        cFileType = M_SEC_CA_CERT_HEADER;
        pcPrefix = M_SEC_CA_CERT_DIR_PREFIX;
        pcCertPrefix = "c_";
    }
    else
    {
        pcFilePath = M_SEC_CONTRACT_DIR;
        cFileType = M_SEC_CONTRACT;
        pcPrefix = M_SEC_CONTRACT_DIR_PREFIX;
        pcCertPrefix = "t_";
    }
    /* when the length of IndexFile is zero.*/
    if ((0 == iIndexFileSize) || (1 == iIndexFileSize))
    {
        WE_CHAR cTemp = 0;
        eRes = WE_FILE_WRITE(hIndexFileHandle, &(cTemp), 1, &(iResult));
        if ((eRes!=E_WE_OK) || (iResult<1))
        {
            return M_SEC_ERR_GENERAL_ERROR;
        }
        iIndexFileSize = 1; 
    }    
    
    pucIndexFile = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iIndexFileSize * sizeof(WE_UCHAR));  
    if (NULL == pucIndexFile)
    {
        eRes = WE_FILE_CLOSE(hIndexFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    /* read out indexfile.*/
    eRes = WE_FILE_SEEK(hIndexFileHandle, 0, WE_FILE_SEEK_SET, &(iNewPos));
    eRes = WE_FILE_READ(hIndexFileHandle, pucIndexFile, iIndexFileSize, &(iResult));
    if ((eRes!=E_WE_OK) || (iResult<iIndexFileSize))
    {
        WE_FREE(pucIndexFile);
        eRes = WE_FILE_CLOSE(hIndexFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }    
    pucP = pucIndexFile;
    iPos = 0;
    iNumInIndexFile = *pucP;
    pucP++;
    iPos++;
    iRealIndexFileSize = iIndexFileSize;
    iPastLen = 1;
    for (iIndexI=0; ((iIndexI<iNumInIndexFile) && (iPastLen<iIndexFileSize)); iIndexI++) 
    {
        pucTemp = pucP;
        if ((SEC_STRCMP(M_SEC_USERCERT_INDEXFILE_NAME, pcIndexFileName) == 0)
            || (SEC_STRCMP(M_SEC_CA_CERT_INDEXFILE_NAME, pcIndexFileName) == 0))
        {
            pucTemp++;
        }
        Sec_StoreStrUint8to32(pucTemp, &uiIndexFileId);
        pucTemp += 4;
        Sec_StoreStrUint8to32(pucTemp, &uiCount);
        pucTemp += 4;
        iResult = Sec_WimAcqFileName(cFileType, uiIndexFileId, &pcFileName);        
        if (iResult != M_SEC_ERR_OK)
        {  
            if(NULL != pcFileName)
            {
                WE_FREE(pcFileName);
                pcFileName = NULL;
            }
            WE_FREE(pucIndexFile);
            eRes = WE_FILE_CLOSE(hIndexFileHandle);
            return M_SEC_ERR_GENERAL_ERROR;
        }    
        /* check if the file exists.*/
        iResult = Sec_WimCheckFileExist(hSecHandle, pcFileName);
        WE_FREE(pcFileName);
        if (iResult != M_SEC_ERR_OK) /* change the indexfile.*/
        {    
            iResult = Sec_WimGetBlockLength(pucP, cFileType, &iBlockLen);              
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucIndexFile);
                eRes = WE_FILE_CLOSE(hIndexFileHandle);
                return M_SEC_ERR_GENERAL_ERROR;
            }    
            /* length data which need to be moved. */
            iLeftLength = iRealIndexFileSize - iBlockLen - iPos;
            iRealIndexFileSize = iRealIndexFileSize - iBlockLen;
            /* for checking every block in index file.*/
            iNumInIndexFile--;
            iIndexI--;            
            if (0 != iLeftLength) /* the block at the end of indexfile.*/
            {
                (WE_VOID)WE_MEMMOVE(pucP, pucP+iBlockLen, (WE_UINT32)iLeftLength);
            }
        }    
        else /* the file exists.*/
        {
            iResult = Sec_WimGetBlockLength(pucP, cFileType, &iBlockLen);
            pucP += iBlockLen;            
            iPos += iBlockLen;
        }
        iPastLen += iBlockLen;
    }   
    /* update the index file. */
    *pucIndexFile = (WE_UCHAR)iIndexI;
    eRes = WE_FILE_SETSIZE(hIndexFileHandle, iRealIndexFileSize);
    eRes = WE_FILE_SEEK(hIndexFileHandle, 0, WE_FILE_SEEK_SET, &(iNewPos));
    eRes = WE_FILE_WRITE(hIndexFileHandle, pucIndexFile, iRealIndexFileSize, &(iResult));
    eResult = WE_FILE_CLOSE(hIndexFileHandle); 
    if ((eRes!=E_WE_OK) || (iResult<iRealIndexFileSize))
    {        
        WE_FREE(pucIndexFile);         
        return M_SEC_ERR_GENERAL_ERROR;
    }   
    
    eRes = WE_FILE_GETSIZE_DIR(SEC_WE_HANDLE, pcFilePath, &(iNumInFolder)); 
    if (eRes != E_WE_OK)
    {    
        WE_FREE(pucIndexFile);
        return M_SEC_ERR_GENERAL_ERROR;
    }     
    
    pucP = pucIndexFile;        
    *pucP = (WE_UINT8)iNumInIndexFile;
    pucP++;
    /* set the file folder is consistent with Index File.*/
    if (iNumInFolder != iNumInIndexFile)/* there are files in file folder, but can not be found in IndexFile. */
    {
        WE_INT32 iFound = 0;
        for (iIndexI=0; iIndexI<iNumInFolder; iIndexI++)
        {    
            iFound = 0;
            eRes = WE_FILE_READ_DIR(SEC_WE_HANDLE, pcFilePath, iIndexI, 
                acNameBuf, 20, (E_WE_FILE_TYPE *)(&iType), &lSize);
            if (eRes != E_WE_OK)
            {
                WE_FREE(pucIndexFile);
                return M_SEC_ERR_GENERAL_ERROR;
            }
            pcDirFileName = WE_SCL_STRCAT(pcPrefix, acNameBuf); 
            if (WE_MEMCMP(pcCertPrefix, acNameBuf, 2) == 0)
            {
                pucP = pucIndexFile+1;
                for (iIndexJ=0; iIndexJ<iNumInIndexFile; iIndexJ++) /* search in index file.*/
                {
                    pucTemp = pucP;
                    if ((SEC_STRCMP(M_SEC_USERCERT_INDEXFILE_NAME, pcIndexFileName) == 0)
                        || (SEC_STRCMP(M_SEC_CA_CERT_INDEXFILE_NAME, pcIndexFileName) == 0))
                    {
                        pucTemp++;
                    }
                    Sec_StoreStrUint8to32(pucTemp, &uiIndexFileId);
                    iResult= Sec_WimAcqFileName(cFileType, uiIndexFileId, &pcFileName);
                    if (iResult != M_SEC_ERR_OK)
                    {  
                        if(NULL != pcFileName)
                        {
                            WE_FREE(pcFileName);
                            pcFileName = NULL;
                        }                  
                        WE_FREE(pucIndexFile);
                        WE_FREE(pcDirFileName);
                        return M_SEC_ERR_GENERAL_ERROR;
                    }            
                    if ((WE_MEMCMP(pcFileName, pcDirFileName, SEC_STRLEN(pcFileName)) == 0)
                        && (SEC_STRLEN(pcFileName) == SEC_STRLEN(pcDirFileName)))
                    {
                        iFound = 1;
                        WE_FREE(pcFileName);   
                        pcFileName = NULL;             
                        break;
                    }
                    iResult = Sec_WimGetBlockLength(pucP, cFileType, &iBlockLen);
                    if (iResult != M_SEC_ERR_OK)
                    {
                        WE_FREE(pcFileName);
                        WE_FREE(pucIndexFile);
                        WE_FREE(pcDirFileName);
                        return M_SEC_ERR_GENERAL_ERROR;
                    }            
                    pucP += iBlockLen;
                    WE_FREE(pcFileName);
                    pcFileName = NULL;             
                }
            }
            
            if (0 == iFound) /* if the file can not be found in IndexFile, delete it.*/
            {
                eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcDirFileName);
                if (E_WE_OK == eRes)
                {
                    iIndexI--;
                    iNumInFolder--;
                }
            }
            WE_FREE(pcDirFileName);
            pcDirFileName = NULL;
        }            
    }   
    WE_FREE(pucIndexFile);         
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimInit
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:        
        Initialse WIM.        
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.    
        iTargetID[IN/OUT]: ID of the object.   
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimInit(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    WE_INT32  iResult = 0; 
    
    if (NULL == hSecHandle)
    {
        return;
    }    
    SEC_INITIALISE = 0;
    
    Sec_WimCheckDir(hSecHandle);
    
    iResult = Sec_WimCheckAllIndexFile(hSecHandle);
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimInitializeResponse(hSecHandle, iTargetID, iResult);
        return;
    }
    /*modified by Bird 070210*/
    if (((WE_INT32)E_SEC_WTLS == SEC_CALLER_TYPE) &&
        (M_SEC_HAVEREAD_SESSIONPEER & *(M_SEC_WIM_HAVELOADUSERPRIV)) != M_SEC_HAVEREAD_SESSIONPEER) 
      /* for ISigntext, not load session and peer.*/
    {
        Sec_WimSessionStart(hSecHandle);
        *(M_SEC_WIM_HAVELOADUSERPRIV) |= M_SEC_HAVEREAD_SESSIONPEER;
    }
    /* load M_SEC_USERCERT_PRIVKEY_INDEX file. */
    Sec_WimGetKeyPairInfo(hSecHandle);    
    
    Sec_WimInit(hSecHandle);
    SEC_INITIALISE = 1; 
    
    /* check not verified certificates.*/
    Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, M_SEC_NON_VERIFIED_WTLS_TYPE);  
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimEnd
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Terminate WIM.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimEnd(WE_HANDLE hSecHandle)
{
    if (NULL == hSecHandle)
    {
        return;
    }    
    if (!Sec_WimIsInited(hSecHandle))
    {
        return;
    }
    Sec_WimClearKeyPairInfo(hSecHandle);
    Sec_WimSessionStop(hSecHandle);
    SEC_INITIALISE = 0;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsGetCipherSuite
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Initialise parameters.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        ucServerAuth[IN]: indication of server authentication or not.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimWtlsGetCipherSuite(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucServerAuth)
{ 
    WE_UINT16      usTooManyCerts = 0;
#if (defined M_SEC_CFG_RC5_CBC) && (!(defined M_SEC_CFG_3DES_CBC_EDE))
    WE_UCHAR       aucCipherMethods[16] = {0};    
#endif
#if (defined M_SEC_CFG_RC5_CBC) && (defined M_SEC_CFG_3DES_CBC_EDE)
    WE_UCHAR       aucCipherMethods[20] = {0};    
#endif
#if (!(defined M_SEC_CFG_RC5_CBC)) && (defined M_SEC_CFG_3DES_CBC_EDE)
    WE_UCHAR       aucCipherMethods[8] = {0};    
#endif    
    WE_INT32       iCipherMethodsLen = 0;    
    WE_UCHAR       *pucTrustedKeyIds = NULL; /* for CA certificate.*/
    WE_INT32       iNumRootCerts = 0;        /* number of CA certificate.*/
    WE_INT32       iIndex = 0;
    WE_UINT16      usAllocLen = 0;
    WE_UINT8       *pucSubjectLen = NULL;
    WE_UCHAR       **ppucSubject = NULL;
    WE_UCHAR       *pucTmpSubject = NULL;
    WE_UINT8       *pucSubjectType = NULL;
    const WE_UINT8 ucX509Name = 1;
    const WE_UINT8 ucWtlsName = 2;
    WE_UINT8       iIndexI = 0;
    WE_INT32       iIndexJ = 0;
    WE_INT32       iIndexK = 0;
    WE_INT32       iRes = 0;
    WE_INT32       iKeyExIdLen = 0;         /* for user certificate.*/
    WE_UCHAR       *pucKeyExIds = NULL;
    WE_UCHAR       *pucRootCert = NULL;
    WE_UINT16      usRootCertLen = 0;
    WE_UINT32      uiCertId = 0;        
    WE_INT32       iResult = 0;
    WE_UINT8       ucSubjectIdType = 0;
    WE_UINT16      usSubjectcharSet = 0;
    WE_UINT8       ucCertType = 0;
    WE_UINT16      usTemp = 0;
    WE_INT32       iNumWTLSDisableRootCerts = 0;
    WE_INT32       iNumAbleRootCerts = 0;
    WE_UCHAR       ucAble = 0;

#ifdef M_ALGORITHM_TEST       

    WE_LOG_MSG((0,(WE_UINT8)0,"+++++++++++++#define M_ALGORITHM_TEST+++++++++++++ \n"));
    
    {
        IFile  * pFile = NULL ;
        WE_UINT8 aucAlgorithm[3] = { 0, };
        St_WeHandle *pstWeHandleData = NULL;
        ISecW  *pstSecHandle = (ISecW *)hSecHandle;
        if ( NULL == pstSecHandle )
        {
            DBGPRINTF("%s\n","+++error++,NULL == pstSecHandle");
            Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED,
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
            return ;
        }

        pucKeyExIds = (WE_UCHAR *)WE_MALLOC(3 * sizeof(WE_UCHAR));
        
        if ( NULL == pucKeyExIds )
        {
            DBGPRINTF("%s\n","+++error++,NULL == pstSecHandle");
            Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED,
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
            return ;
        }
        pstWeHandleData = (St_WeHandle*)(((St_PublicData*)(pstSecHandle->hPrivateData))->hWeHandle);

        WE_LOG_MSG((0,(WE_UINT8)0,"++++++++open fs:/shared/SecAlg/alg.txt++++++++ \n"));
        
        pFile = IFILEMGR_OpenFile( pstWeHandleData->pIFileMgr, "fs:/shared/algorithm/algorithm", _OFM_READ );
        
        
        WE_LOG_MSG((0,(WE_UINT8)0,"++++++++fs:/shared/SecAlg/alg.txt++++++++ read \n"));
        IFILE_Read(pFile, aucAlgorithm, 3);   
        WE_LOG_MSG((0,(WE_UINT8)0,"++++++++fs:/shared/SecAlg/alg.txt++++++++ read over \n"));
    
        if( pFile )
        {
            IFILE_Release(pFile);
        }
        WE_LOG_MSG((0,(WE_UINT8)0,"+++++++++++++get cipher suites ++++++++++++\n"));
        
        pucKeyExIds[iKeyExIdLen++] = aucAlgorithm[0];
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        aucCipherMethods[iCipherMethodsLen++] = aucAlgorithm[1];
        aucCipherMethods[iCipherMethodsLen++] = aucAlgorithm[2];
        
        WE_LOG_MSG((0,(WE_UINT8)0,"+++++++++++++get cipher suites ++++++++++++\n"));
        
    }       
        
#else /* else #ifdef M_ALGORITHM_TEST  */

    if (NULL == hSecHandle)
    {
        return;
    }    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED,
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
        return;
    }
#if defined(M_SEC_CFG_WTLS_CLASS_2)
    pucKeyExIds = (WE_UCHAR *)WE_MALLOC(21 * sizeof(WE_UCHAR));
#elif defined(M_SEC_CFG_WTLS_CLASS_3) 
    pucKeyExIds = (WE_UCHAR *)WE_MALLOC(800 * sizeof(WE_UCHAR)); 
#endif    
    if (NULL == pucKeyExIds)
    {
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
        return;
    }

#ifdef M_SEC_CFG_RC5_CBC
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_RC5_CBC;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_SHA1;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_RC5_CBC_56;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_SHA1;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_RC5_CBC_40;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_SHA1;

    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_RC5_CBC;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_MD5;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_RC5_CBC_56;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_MD5;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_RC5_CBC_40;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_MD5;
#endif
#ifdef M_SEC_CFG_3DES_CBC_EDE
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_3DES_CBC_EDE;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_SHA1;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_3DES_CBC_EDE;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_MD5;
#endif
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_NULL;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_SHA1;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_BULK_NULL;
    aucCipherMethods[iCipherMethodsLen++] = (WE_UCHAR)E_SEC_ALG_HASH_MD5;
    
#if defined(M_SEC_CFG_WTLS_CLASS_3)    
    Sec_WimGetKeyExgIds(hSecHandle, &pucKeyExIds, &iKeyExIdLen);
#endif
    if (iKeyExIdLen == 0)
    {
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_NULL;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_768;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_512;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
        pucKeyExIds[iKeyExIdLen++] = (WE_UCHAR)0;
    }
    if (ucServerAuth == 1)
    {
        WE_UINT8  iTemp = 0;   
        pucKeyExIds[iTemp++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_NULL;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        pucKeyExIds[iTemp++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        pucKeyExIds[iTemp++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        pucKeyExIds[iTemp++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        pucKeyExIds[iTemp++] = (WE_UCHAR)0;
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, 
            M_SEC_ERR_OK, (WE_UCHAR *)aucCipherMethods, 
            iCipherMethodsLen, pucKeyExIds, iTemp * sizeof(WE_UCHAR),
            NULL, 0, usTooManyCerts);
        WE_FREE(pucKeyExIds);
        return;
    }
    
#endif /* end if #ifdef M_ALGORITHM_TEST */

    /* get the number of CA certificate. */
    iRes = Sec_WimGetNbrOfCACerts(hSecHandle, (WE_UINT8 *)&(iNumRootCerts));
    if (iRes != M_SEC_ERR_OK)
    {    
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, iRes, 
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
        WE_FREE(pucKeyExIds);
        
        return;
    }

    iRes = Sec_WimGetNbrOfWTLSDisableCACerts(hSecHandle, &iNumWTLSDisableRootCerts);
    if (iRes != M_SEC_ERR_OK)
    {    
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, iRes, 
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
        WE_FREE(pucKeyExIds);

        return;
    }
    iNumAbleRootCerts = iNumRootCerts - iNumWTLSDisableRootCerts;    
    if (0 == iNumAbleRootCerts) /* should not be executed.*/
    {            
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, 
            (WE_UCHAR *)aucCipherMethods,iCipherMethodsLen, pucKeyExIds, iKeyExIdLen * (WE_INT32)sizeof(WE_UCHAR),
             pucTrustedKeyIds, (iIndex * (WE_INT32)sizeof(WE_UCHAR)), (WE_INT32)usTooManyCerts);        
        WE_FREE(pucKeyExIds);
        return;
    }
    pucSubjectLen = (WE_UINT8*)WE_MALLOC(sizeof(WE_UINT8) * (WE_UINT32)iNumAbleRootCerts);
    pucSubjectType = (WE_UINT8*)WE_MALLOC(sizeof(WE_UINT8) * (WE_UINT32)iNumAbleRootCerts);
    ppucSubject = (WE_UCHAR**)WE_MALLOC(sizeof(WE_UCHAR*) * (WE_UINT32)iNumAbleRootCerts);
    if ((NULL==pucSubjectLen) || (NULL==pucSubjectType) ||  (NULL==ppucSubject))
    {
        WE_FREE(pucKeyExIds);
        if (pucSubjectLen != NULL )
        {
            WE_FREE(pucSubjectLen);
        }
        if (pucSubjectType != NULL )
        {
            WE_FREE(pucSubjectType);
        }
        if (NULL != ppucSubject)
        { 
            WE_FREE(ppucSubject);
        }
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
        return;
    }      
    /* get subjects of all CA certificates.*/    
    for (iIndexK=0; iIndexK<iNumRootCerts; iIndexK++, iIndexI++)
    {
        iResult = Sec_WimGetCACert(hSecHandle, iIndexK+1, &pucRootCert,
            &usRootCertLen, &uiCertId, &ucCertType, &ucAble);
        if (iResult == M_SEC_ERR_OK) 
        {
            if (M_SEC_WTLS_CA_CERT == ucCertType) /* wtls format */
            {
                if (M_SEC_WTLS_CA_CERT_DISABLE == ucAble)
                {
                    WE_FREE(pucRootCert);
                    iIndexI--;
                    continue;
                }
                iResult = Sec_WtlsGetSubjectInfo(hSecHandle, pucRootCert, &ucSubjectIdType,
                    &usSubjectcharSet, &(pucSubjectLen[iIndexI]), &pucTmpSubject);  
                if (iResult != M_SEC_ERR_OK) 
                {
                    WE_FREE(pucRootCert);
                    pucSubjectLen[iIndexI] = 0;
                    ppucSubject[iIndexI] = NULL;
                    continue;
                }
                pucSubjectType[iIndexI] = ucWtlsName;
                usAllocLen = (WE_UINT16)(usAllocLen + 6 + pucSubjectLen[iIndexI]);   
            }
            else if (M_SEC_X509_CA_CERT == ucCertType)  /* x509 formate */
            {
                iResult = Sec_WimGetX509Sub(hSecHandle, pucRootCert, 
                    &pucTmpSubject, &(usTemp));  
                if (iResult != M_SEC_ERR_OK) 
                {
                    WE_FREE(pucRootCert);
                    pucSubjectLen[iIndexI] = 0;
                    ppucSubject[iIndexI] = NULL;
                    continue;
                }
                
                if(usTemp > 255)
                {
                    pucSubjectLen[iIndexI] = 255;
                }
                else
                {
                    pucSubjectLen[iIndexI] = (WE_UINT8)usTemp;
                }
                pucSubjectType[iIndexI] = ucX509Name;
                usAllocLen = (WE_UINT16)(usAllocLen + 4 + pucSubjectLen[iIndexI]); 
            } 
            else /* not wtls or x509 format. */
            {
                WE_FREE(pucRootCert);
                for (iIndexJ=0; iIndexJ<iIndexI; iIndexJ++)
                {
                    if (NULL != ppucSubject[iIndexJ])
                    {
                        WE_FREE(ppucSubject[iIndexJ]); 
                    }
                }            
                WE_FREE(ppucSubject);
                WE_FREE(pucSubjectLen);
                WE_FREE(pucSubjectType);
                WE_FREE(pucKeyExIds);
                Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, 
                    iTargetID, M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, 
                    NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
                return;
            }              
            
            ppucSubject[iIndexI] = (WE_UCHAR *)WE_MALLOC(pucSubjectLen[iIndexI] * sizeof(WE_UCHAR));
            if ((NULL==ppucSubject[iIndexI]) && (0!=pucSubjectLen[iIndexI]))
            {  
                WE_FREE(pucRootCert);
                for (iIndexJ=0; iIndexJ<iIndexI; iIndexJ++)
                {
                    if (NULL != ppucSubject[iIndexJ])
                    {
                        WE_FREE(ppucSubject[iIndexJ]); 
                    }
                }            
                WE_FREE(ppucSubject);
                WE_FREE(pucSubjectLen);
                WE_FREE(pucSubjectType);
                WE_FREE(pucKeyExIds);
                Sec_WimWtlsGetCipherSuiteResponse(hSecHandle,
                    iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                    NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
                return;
            }
            for(iIndexJ=0; iIndexJ<pucSubjectLen[iIndexI]; iIndexJ++)
            {
                ppucSubject[iIndexI][iIndexJ] = pucTmpSubject[iIndexJ];
            }  
            WE_FREE(pucRootCert);
        }
        else  /* failure to get the root certificate. */
        {
            for (iIndexJ=0; iIndexJ<iIndexI; iIndexJ++)
            {
                if (NULL != ppucSubject[iIndexJ])
                {
                    WE_FREE(ppucSubject[iIndexJ]); 
                }
            }    
            WE_FREE(ppucSubject);
            WE_FREE(pucSubjectLen);
            WE_FREE(pucSubjectType);
            WE_FREE(pucKeyExIds);
            WE_FREE(pucRootCert);
            Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, 
                iTargetID, M_SEC_ERR_BAD_CERTIFICATE, 
                NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
            return;
        }
    }
    
    pucTrustedKeyIds = (WE_UCHAR *)WE_MALLOC(sizeof(WE_UCHAR) * usAllocLen); 
    if (NULL == pucTrustedKeyIds)
    {
        for (iIndexJ=0; iIndexJ<iNumAbleRootCerts; iIndexJ++) 
        {
            if (NULL != ppucSubject[iIndexJ])
            {
                WE_FREE(ppucSubject[iIndexJ]); 
            }
        }
        WE_FREE(ppucSubject);
        WE_FREE(pucSubjectLen);
        WE_FREE(pucSubjectType);
        WE_FREE(pucKeyExIds);
        Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
            NULL, 0, NULL, 0, NULL, 0, usTooManyCerts);
        return;
    }
    
    if (iNumAbleRootCerts <= M_SEC_MAX_NBR_OF_CERTS_IN_TRUSTED_KEY_IDS)
    { 
        iIndex = 0;
        for (iIndexI=0; iIndexI<iNumAbleRootCerts; iIndexI++)
        {
            if (ppucSubject[iIndexI] != NULL)
            {
                if (pucSubjectType[iIndexI] == ucWtlsName)
                {
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)0;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)1;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)0;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)4;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)pucSubjectLen[iIndexI];
                    for (iIndexJ=0; iIndexJ<pucSubjectLen[iIndexI]; iIndexJ++)
                    {
                        pucTrustedKeyIds[iIndex++] = (ppucSubject[iIndexI])[iIndexJ];
                    }
                }
                if (pucSubjectType[iIndexI] == ucX509Name)
                {
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)E_SEC_WTLS_KEYEXCHANGE_RSA;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)0;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)255;
                    pucTrustedKeyIds[iIndex++] = (WE_UCHAR)pucSubjectLen[iIndexI];
                    for (iIndexJ=0; iIndexJ<pucSubjectLen[iIndexI]; iIndexJ++)
                    {
                        pucTrustedKeyIds[iIndex++] = (ppucSubject[iIndexI])[iIndexJ];
                    }
                }
            }
        }
    }
    else
    {
        usTooManyCerts = 1;
    }
    
    Sec_WimWtlsGetCipherSuiteResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, (WE_UCHAR *)aucCipherMethods,
        iCipherMethodsLen, pucKeyExIds, iKeyExIdLen * (WE_INT32)sizeof(WE_UCHAR),
        pucTrustedKeyIds, (iIndex * (WE_INT32)sizeof(WE_UCHAR)), (WE_INT32)usTooManyCerts);
    WE_FREE(pucTrustedKeyIds);
    for (iIndexJ=0; iIndexJ<iNumAbleRootCerts; iIndexJ++)
    {
        if (ppucSubject[iIndexJ] != NULL)
        {
            WE_FREE(ppucSubject[iIndexJ]);
        }
    }
    WE_FREE(ppucSubject);
    WE_FREE(pucSubjectLen);
    WE_FREE(pucSubjectType);
    WE_FREE(pucKeyExIds);
    

}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsGetPrfResult
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        compute master secret.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]:  ID of the object.
        iMasterSecretId[IN]: ID of the master secret.
        pucSecret[IN]:  pointer to the secret.
        iSecretLen[IN]: the length of the secret.
        pcLabel[IN]:  pointer to the label.
        pucSeed[IN]:  pointer to the seed.
        iSeedLen[IN]: the length of the seed.
        iOutputLen[IN]: the length of the output data.
        ucConnectionType[IN]: M_SEC_WTLS_CONNECTION_MODE.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimWtlsGetPrfResult(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                              WE_INT32 iMasterSecretId, const WE_UCHAR *pucSecret, 
                              WE_INT32 iSecretLen, const WE_CHAR *pcLabel,
                              const WE_UCHAR *pucSeed, WE_INT32 iSeedLen,
                              WE_INT32 iOutputLen, WE_UINT8 ucConnectionType,WE_UINT8 ucAlg)
{
    WE_INT32     iRes = 0;
    WE_UCHAR     *pucBuf = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimWtlsGetPrfResultResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED, NULL, 0);
        return;
    }
    /* ioutputLen maybe 0. */
    if (0 == iOutputLen)
    {
        Sec_WimWtlsGetPrfResultResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, NULL, 0);
        return;
    }
    pucBuf = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iOutputLen * sizeof(WE_UCHAR));
    if (NULL == pucBuf) 
    {
        Sec_WimWtlsGetPrfResultResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
        return;
    }
    
    if (ucConnectionType == M_SEC_WTLS_CONNECTION_MODE)
    {
        iRes = Sec_WimGetWTLSMasterSecret(hSecHandle, iMasterSecretId, pucSecret, iSecretLen, 
            pcLabel, pucSeed, iSeedLen, iOutputLen, pucBuf,ucAlg);
    }
    else 
    {
        Sec_WimWtlsGetPrfResultResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
    }
    
    Sec_WimWtlsGetPrfResultResponse(hSecHandle, iTargetID, iRes, pucBuf, iOutputLen);
    WE_FREE(pucBuf);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimSaveCert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Store user certificate and CA certificate in files and in IndexFile.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]:  ID of the object.
        pucCert[IN]: certificate to be stored.
        usCertLen[IN]: length of certificate.
        cCertType[IN]: type of certificate.
        pvCertFriendlyName[IN]: name of certificate.
        usFriendlyNameLen[IN]: length of name.
        sCharSet[IN]: character set of certificate.
        pucCertURL[IN]: URL of certificate.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimSaveCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR *pucCert, 
                       WE_UINT16 usCertLen, WE_CHAR cCertType, WE_VOID *pvCertFriendlyName,
                       WE_UINT16 usFriendlyNameLen, WE_INT16 sCharSet,
                       const WE_UCHAR *pucCertURL)
{
    WE_HANDLE        hFileHandle = NULL;
    WE_CHAR          *pcFileName = NULL;
    WE_UINT32        uiFileId = 0; 
    WE_UINT8         usURLLen = 0;  
    WE_INT32         iRes = 0;
    E_WE_ERROR       eRes = E_WE_OK;
    WE_INT32         iNewPos = 0;    
    WE_INT8          cFileType = 0;  
    WE_UINT16        usRealCertLen = 0;
    WE_INT8          cNameType = 0;    
    St_SecCertificate  stCert = {0};
    E_SecHashAlgType eHashAlg = E_SEC_ALG_HASH_SHA1;
    WE_UCHAR         aucDN[M_SEC_KEY_HASH_SIZE] = {0};
    WE_INT32         iDNLen = M_SEC_KEY_HASH_SIZE;
    WE_UINT8         aucPubKeyHash[M_SEC_KEY_HASH_SIZE] = {0};    
    WE_INT32         iPubKeyHashLen = M_SEC_KEY_HASH_SIZE;
    WE_UINT8         aucSubjectHash[M_SEC_KEY_HASH_SIZE] = {0};    
    WE_INT32         iSubjectHashLen = M_SEC_KEY_HASH_SIZE;
    St_SecPubKeyRsa  stPubKey = {0};
    WE_UINT16        usSubjectLen = 0;
    WE_UINT16        usIssuerLen = 0;
    WE_UCHAR         ucAble = 0;
    
    if ((NULL == hSecHandle) || (NULL == pucCert))
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimStoreCertResponse(hSecHandle, iTargetID,
            M_SEC_ERR_WIM_NOT_INITIALISED, 0, 0, 0, 0);
        return;
    }
    
    usRealCertLen = 0;
    /* parse the certificate, get public key and length of DN. */
    if (*pucCert == M_SEC_CERTIFICATE_WTLS_TYPE)
    {
        iRes = Sec_WtlsCertParse(hSecHandle, pucCert+1, &usRealCertLen, &stCert);
        if (M_SEC_ERR_OK != iRes)
        {
            Sec_WimStoreCertResponse(hSecHandle, iTargetID, iRes, 0, 0, 0, 0);
            return;      
        }
        usIssuerLen = stCert.stCertUnion.stWtls.ucIssuerLen;
        usSubjectLen = stCert.stCertUnion.stWtls.ucSubjectLen;
    }
    else if (*pucCert == M_SEC_CERTIFICATE_X509_TYPE)
    {
        iRes = Sec_X509CertParse(hSecHandle, pucCert+1, &usRealCertLen, &stCert); 
        if (M_SEC_ERR_OK != iRes)
        {
            Sec_WimStoreCertResponse(hSecHandle, iTargetID, iRes, 0, 0, 0, 0);
            return;      
        }
        usIssuerLen = stCert.stCertUnion.stX509.usIssuerLen;
        usSubjectLen = stCert.stCertUnion.stX509.usSubjectLen;
    }
    else
    {
        Sec_WimStoreCertResponse(hSecHandle, iTargetID, 
            M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, 0, 0, 0, 0);
        return;      
    }    
    /* hash the issuer. */
    iRes = Sec_LibHash(hSecHandle, eHashAlg, stCert.pucIssuer, usIssuerLen, aucDN, &(iDNLen));
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimStoreCertResponse(hSecHandle, iTargetID, iRes, 0, 0, 0, 0);
        return; 
    }    
    
    /* user certificate.*/
    if (cCertType == M_SEC_USER_CERT_HEADER) 
    {
        cNameType = SEC_NAME_TYPE;
        /* parse the certificate, get public key and length of DN. */
        if (*pucCert == M_SEC_CERTIFICATE_WTLS_TYPE)
        {
            cFileType = M_SEC_WTLS_USER_CERT;            
            stPubKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
            stPubKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
            stPubKey.usModLen = stCert.stCertUnion.stWtls.usModLen;            
            stPubKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
        }
        else if (*pucCert == M_SEC_CERTIFICATE_X509_TYPE)
        {
            cFileType = M_SEC_X509_USER_CERT;
            iRes = Sec_CertGetPubKeyRsa(hSecHandle, (WE_UCHAR *)pucCert, &stPubKey);
            if (iRes != M_SEC_ERR_OK)
            {
                Sec_WimStoreCertResponse(hSecHandle, iTargetID, iRes, 0, 0, 0, 0);
                return;  
            }
        }
        else
        {
            Sec_WimStoreCertResponse(hSecHandle, iTargetID, 
                M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, 0, 0, 0, 0);
            return;      
        }    
        /* hash the public key.*/
        iRes = Sec_LibHash(hSecHandle, eHashAlg, stPubKey.pucModulus, stPubKey.usModLen, 
            aucPubKeyHash, &iPubKeyHashLen);
        if (iRes != M_SEC_ERR_OK)
        {
            Sec_WimStoreCertResponse(hSecHandle, iTargetID, iRes, 0, 0, 0, 0);
            return;  
        }            
    }
    else if (cCertType == M_SEC_CA_CERT_HEADER) /* CA certificate. */
    {
        /*get public key and length of DN. */
        if (*pucCert == M_SEC_CERTIFICATE_WTLS_TYPE)
        {
            cFileType = M_SEC_WTLS_CA_CERT;
            cNameType = M_SEC_CERT_NAME_WTLS;            
            ucAble = M_SEC_WTLS_CA_CERT_ENABLE;
            sCharSet =(WE_INT16)(stCert.stCertUnion.stWtls.usSubjectcharacterSet);
        }
        else if (*pucCert == M_SEC_CERTIFICATE_X509_TYPE)
        {
            cFileType = M_SEC_X509_CA_CERT;
            cNameType = M_SEC_CERT_NAME_X509;
            ucAble = M_SEC_WTLS_CA_CERT_DISABLE;
            sCharSet = -1;
        }
        else
        {
            Sec_WimStoreCertResponse(hSecHandle, iTargetID,
                M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, 0, 0, 0, 0);
            return;      
        }      
        /* hash subject. */
        iRes = Sec_LibHash(hSecHandle, eHashAlg, stCert.pucSubject, 
            usSubjectLen, aucSubjectHash, &iSubjectHashLen);
        if (iRes != M_SEC_ERR_OK)
        {
            Sec_WimStoreCertResponse(hSecHandle, iTargetID, iRes, 0, 0, 0, 0);
            return; 
        }
    }
    else
    {
        Sec_WimStoreCertResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, 0, 0, 0, 0);
        return;      
    }
    /* creat a certificate file. */
    iRes = Sec_WimMakeFile(hSecHandle, cFileType, &hFileHandle, &uiFileId);
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimStoreCertResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0, 0, 0, 0);
        return;
    }
    iRes = Sec_WimAcqFileName(cFileType, uiFileId, &pcFileName);
    usCertLen--; /* exclude the type of certificate.*/
    eRes = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET, &(iNewPos));
    if (eRes != E_WE_OK)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        WE_FREE(pcFileName);        
        Sec_WimStoreCertResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0, 0, 0, 0);
        return;
    }
    /* write the certificate. */
    eRes = WE_FILE_WRITE(hFileHandle, (WE_VOID *)(pucCert+1), (WE_LONG)usCertLen, &(iRes));
    if ((iRes < (WE_INT32)usCertLen) || (eRes != E_WE_OK))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        WE_FREE(pcFileName);
        Sec_WimStoreCertResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0, 0, 0, 0);
        return;
    }
    /* add in certificate Indexfile.*/
    if (cCertType == M_SEC_USER_CERT_HEADER) 
    {        
        iRes = Sec_WimAddBlock(hSecHandle, cFileType, uiFileId, usCertLen,
            aucDN, aucPubKeyHash, cNameType, sCharSet,            
            usFriendlyNameLen, pvCertFriendlyName, 0); 
    }
    else
    {
        iRes = Sec_WimAddBlock(hSecHandle, cFileType, uiFileId, usCertLen,
            aucSubjectHash, aucDN, cNameType, sCharSet,            
            usSubjectLen, stCert.pucSubject, ucAble);
    }
    if (iRes != M_SEC_ERR_OK)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        WE_FREE(pcFileName);
        Sec_WimStoreCertResponse(hSecHandle, iTargetID, (WE_INT32)eRes, 0, 0, 0, 0);
        return; 
    }
    
    if (cCertType == M_SEC_USER_CERT_HEADER)        
    {
        if (NULL == pucCertURL)
        {
            return;
        }
        usURLLen = pucCertURL[0] + 1;   
    }
    else
    {
        usURLLen = 0;
    }
    Sec_WimStoreCertResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, 
        (WE_INT32)uiFileId, usCertLen, usURLLen, usFriendlyNameLen);
    eRes = WE_FILE_CLOSE(hFileHandle);
    WE_FREE(pcFileName);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimDelCert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Delete certificate and relevant index in IndexFile by 'CertId'.
        Clear all Session and peer information.
        If the certificate is user certificate, also need clear relevant user_cert_keypairs, 
        and modify the file whose header is M_SEC_USERCERT_PRIVKEY_INDEX_NAME.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iCertId[IN]:The ID of the certificate.      
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.                           
=====================================================================================*/
WE_VOID Sec_iWimDelCert(WE_HANDLE hSecHandle, WE_INT32 iCertId)
{
    WE_INT8      cFileType = 0;
    WE_CHAR      *pcFileName = NULL;
    WE_INT32     iRes = 0;
    E_WE_ERROR   eResult = E_WE_OK;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimDelCertResponse(hSecHandle, iCertId, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    
    if ((iCertId>=M_SEC_USER_CERT_ID_START) && (iCertId<M_SEC_CA_CERT_ID_START))
    {
        cFileType = M_SEC_USER_CERT_HEADER;
        iRes = Sec_WimCleanUCertData(hSecHandle, iCertId);
        if (iRes != M_SEC_ERR_OK)
        {  
            Sec_WimDelCertResponse(hSecHandle, iCertId, iRes);
            return;
        }
        /* update M_SEC_USERCERT_PRIVKEY_INDEX_NAME FILE. */
        Sec_WimSaveKeyPairInfoToFile(hSecHandle);
    }
    else if (iCertId>=M_SEC_CA_CERT_ID_START && iCertId<M_SEC_CONTRACT_CERT_ID_START)
    {
        cFileType = M_SEC_CA_CERT_HEADER;
    }
    else
    {
        Sec_WimDelCertResponse(hSecHandle, iCertId, M_SEC_ERR_MISSING_CERTIFICATE);
    }
    
    /* delete the block in certificate Indexfile. */
    iRes = Sec_WimDelBlock(hSecHandle, iCertId, cFileType); 
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimDelCertResponse(hSecHandle, iCertId, iRes);
        return;
    }
    
    iRes = Sec_WimAcqFileName(cFileType, (WE_UINT32)iCertId, &pcFileName);
    if (iRes != M_SEC_ERR_OK)
    {
        if(NULL != pcFileName)
        {
            WE_FREE(pcFileName);
            pcFileName = NULL;
        }
        Sec_WimDelCertResponse(hSecHandle, iCertId, iRes);
        return;            
    }
    
    /*added by Bird 061205,perhaps we delete a not exist file*/
    /* check if the certificate exists.*/
    iRes = Sec_WimCheckFileExist(hSecHandle, pcFileName);
    if (iRes != M_SEC_ERR_OK)
    {
        if(NULL != pcFileName)
        {
            WE_FREE(pcFileName);
            pcFileName = NULL;
        }
        Sec_WimDelCertResponse(hSecHandle, iCertId, iRes);
        return ;            
    }
    
    /* delete the certificate file. */
    eResult = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
    WE_FREE(pcFileName);
    Sec_WimDelCertResponse(hSecHandle, iCertId, M_SEC_ERR_OK);  
    /* modify 20070208
    for (iIndexI=0; iIndexI<M_SEC_SESSION_CACHE_SIZE; iIndexI++)
    {      
        Sec_WimClrSessionInfo(hSecHandle, iIndexI);     
        Sec_WimClrPeerInfo(hSecHandle, iIndexI);  
    }*/
    eResult = eResult;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimGetCert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Read out the certificate.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iCertId[IN]: Id of certificate.    
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimGetCert(WE_HANDLE hSecHandle, WE_INT32 iCertId)
{
    WE_UCHAR   *pucCert = NULL;
    WE_UCHAR   *pucTempCert = NULL;
    WE_UINT16  usCertLen = 0;
    WE_UINT16  usTempCertLen = 0;
    WE_INT32   iRes = 0;
    WE_UINT8   ucCertFormat = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimGetCertResponse(hSecHandle, M_SEC_ERR_WIM_NOT_INITIALISED, NULL, 0, NULL, 0);
        return;
    }
    
    iRes = Sec_WimGetCertByID(hSecHandle, iCertId, &pucCert, &usCertLen);                
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimGetCertResponse(hSecHandle, iRes, NULL, 0, NULL, 0);
        return;
    }    
    iRes = Sec_WimGetCertFormatByID(hSecHandle, iCertId, &ucCertFormat);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pucCert);
        Sec_WimGetCertResponse(hSecHandle, iRes, NULL, 0, NULL, 0);
        return;
    }

    pucTempCert = (WE_UCHAR *)WE_MALLOC((usCertLen+1) * sizeof(WE_UCHAR));  
    if (NULL == pucTempCert)
    {
        WE_FREE(pucCert);
        Sec_WimGetCertResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, NULL, 0);
        return;
    }
    pucTempCert[0] = ucCertFormat;
    (WE_VOID)WE_MEMCPY(pucTempCert+1, pucCert, usCertLen);
    usTempCertLen = usCertLen + 1;
    WE_FREE(pucCert);    
    Sec_WimGetCertResponse(hSecHandle, M_SEC_ERR_OK, pucTempCert, usTempCertLen, NULL, 0);
    WE_FREE(pucTempCert);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimCertExists
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Judge whether certificate whose type is cCertType exists or not.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucCert[IN]: pointer to the certificate which need to be verified.
        usCertLen[IN]: length of certificate.
        cCertType[IN]: type of certificate.    
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        omit¡£
=====================================================================================*/
WE_VOID Sec_iWimCertExists(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR *pucCert,
                        WE_UINT16 usCertLen, WE_CHAR cCertType)
{
    WE_UINT8              ucExist = 0;
    WE_INT32              iIndexFileSize = 0;
    WE_INT32              iTryNext = 0;  
    WE_UINT16             usTmpCertLen = 0;
    WE_HANDLE             hFileHandle = NULL;
    WE_CHAR               *pcFileName = NULL;
    WE_INT32              iResult = 0;  
    WE_INT32              iRes = 0;
    E_WE_ERROR            eRes = E_WE_OK;
    WE_INT32              iIndexI = 0;
    WE_INT32              iIndexL= 0;
    St_SecCertificate     stNewCert = {0};
    St_SecCertificate     stOldCert = {0};
    WE_INT32              iFileId = 0;                 
    WE_UINT32             uiCount = 0; 
    WE_CHAR               *pcIndexFileName = NULL;
    WE_UCHAR              *pucIndexFile = NULL;
    WE_UCHAR              *pucFile = NULL;
    WE_UCHAR              *pucP = NULL;
    WE_INT32              iNumInIndexFile = 0;
    WE_INT32              iBlockLen = 0;
    WE_CHAR               cFileType = 0;
    
    if ((NULL == hSecHandle) || (NULL == pucCert))
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED, 
            ucExist, 0, 0, 0, 0, iFileId);
        return;
    }    

    /* store original certificate in 'stNewCert' */
    if (M_SEC_CERTIFICATE_WTLS_TYPE == (*pucCert)) 
    {
        iRes = Sec_WtlsCertParse(hSecHandle, pucCert+1, &usTmpCertLen, &stNewCert); 
    }
    else if (M_SEC_CERTIFICATE_X509_TYPE == (*pucCert))          
    {
        iRes = Sec_X509CertParse(hSecHandle, pucCert+1, &usTmpCertLen, &stNewCert); 
    }
    else
    {
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE,
            ucExist, 0, 0, 0, 0, iFileId);
        return;
    }    
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, iRes, ucExist, 0, 0, 
            0, 0, iFileId);
        return;
    }
        
    if (M_SEC_USER_CERT_HEADER == cCertType)
    {
        pcIndexFileName = (WE_CHAR *)M_SEC_USERCERT_INDEXFILE_NAME;    
    }
    else if(M_SEC_CA_CERT_HEADER == cCertType)
    {
        pcIndexFileName = (WE_CHAR *)M_SEC_CA_CERT_INDEXFILE_NAME;
    }
    else
    {
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE,
            ucExist, 0, 0, 0, 0, iFileId);
        return;
    }
    
    /* get size of indexfile.*/
    eRes = WE_FILE_GETSIZE(SEC_WE_HANDLE, (const WE_CHAR *)pcIndexFileName, &(iIndexFileSize));
    if (E_WE_OK != eRes)
    {
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
            ucExist, 0, 0, 0, 0, iFileId);
        return;
    }
    if (1 >= iIndexFileSize)
    {
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_OK,
            ucExist, 0, 0, 0, 0, iFileId);
        return;
    }
    pucIndexFile = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iIndexFileSize * sizeof(WE_UCHAR));  
    if (NULL == pucIndexFile)
    {
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
            ucExist, 0, 0, 0, 0, iFileId);
        return;
    }
    /* open indexfile and read all.*/
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, (WE_CHAR *)pcIndexFileName, WE_FILE_SET_RDWR, &(hFileHandle));
    if (eRes != E_WE_OK)
    {
        WE_FREE(pucIndexFile);
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
            ucExist, 0, 0, 0, 0, iFileId);
        return;
    }     
    eRes = WE_FILE_READ(hFileHandle, pucIndexFile, iIndexFileSize, &(iResult));    
    if ((eRes!=E_WE_OK) || (iResult<iIndexFileSize))
    {
        WE_FREE(pucIndexFile);
        Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
            ucExist, 0, 0, 0, 0, iFileId);
        eRes = WE_FILE_CLOSE(hFileHandle);
        return;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);  
    pucP = pucIndexFile;
    iNumInIndexFile = *pucP;
    pucP++;
    for (iIndexI=0; iIndexI<iNumInIndexFile; iIndexI++) 
    {
        /* judge whether the format of certificate is matched or not. */
        cFileType = (WE_CHAR)(*pucP);            
        if (((cFileType == M_SEC_WTLS_USER_CERT) || (cFileType == M_SEC_WTLS_CA_CERT)) 
            && ((*pucCert) == M_SEC_CERTIFICATE_WTLS_TYPE)) 
        {            
        }
        else if (((cFileType == M_SEC_X509_USER_CERT) || (cFileType == M_SEC_X509_CA_CERT)) 
            && ((*pucCert) == M_SEC_CERTIFICATE_X509_TYPE)) 
        {
        }
        else /* Format is not matched, read next index.*/
        {
            iRes = Sec_WimGetBlockLength(pucP, cCertType, &iBlockLen);
            if (iRes != M_SEC_ERR_OK)
            {
                WE_FREE(pucIndexFile);
                Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
                    ucExist, 0, 0, 0, 0, iFileId);
                return;                
            }
            pucP += iBlockLen;
            continue;
        }        
        Sec_StoreStrUint8to32(pucP+1, (WE_UINT32 *)(&iFileId));
        Sec_StoreStrUint8to32(pucP+1+4, &uiCount);
        iRes = Sec_WimAcqFileName(cFileType, (WE_UINT32)iFileId, &pcFileName);
        if (iRes != M_SEC_ERR_OK)
        {
            WE_FREE(pucIndexFile);
            Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
                ucExist, 0, 0, 0, 0, iFileId);
            return;
        }
        /* open the certificate and read out it.*/
        eRes = WE_FILE_OPEN(SEC_WE_HANDLE, pcFileName, WE_FILE_SET_RDWR, &(hFileHandle));
        WE_FREE(pcFileName);
        if (eRes != E_WE_OK)
        {
            WE_FREE(pucIndexFile);
            Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
                ucExist, 0, 0, 0, 0, iFileId);
            return;
        }   
        pucFile = (WE_UCHAR *)WE_MALLOC(uiCount * sizeof(WE_UCHAR));  
        if (NULL == pucFile)
        {
            WE_FREE(pucIndexFile);
            eRes = WE_FILE_CLOSE(hFileHandle);
            Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
                ucExist, 0, 0, 0, 0, iFileId);
            return;
        }
        eRes = WE_FILE_READ(hFileHandle, pucFile, (WE_LONG)uiCount, &(iResult));  
        if (eRes != E_WE_OK)
        {
            WE_FREE(pucIndexFile);
            WE_FREE(pucFile);  
            eRes = WE_FILE_CLOSE(hFileHandle);
            Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
                ucExist, 0, 0, 0, 0, iFileId);
            return;
        }   
        eRes = WE_FILE_CLOSE(hFileHandle);

        usTmpCertLen = 0;
        if ((cFileType == M_SEC_WTLS_USER_CERT) || (cFileType == M_SEC_WTLS_CA_CERT)) 
        {
            iResult = Sec_WtlsCertParse(hSecHandle, pucFile, &usTmpCertLen, &stOldCert);             
        }
        else
        {
            iResult = Sec_X509CertParse(hSecHandle, pucFile, &usTmpCertLen, &stOldCert); 
        }
        if (iResult != M_SEC_ERR_OK)
        {            
            WE_FREE(pucIndexFile);
            WE_FREE(pucFile);
            Sec_WimCertExistsResponse(hSecHandle, iTargetID, iResult, 
                ucExist, 0, 0, 0, 0, iFileId);            
            return;
        }
        /* judge if two certificates are matched.*/
        if (SEC_STRCMP((WE_CHAR *)stNewCert.pucSubject, (WE_CHAR *)stOldCert.pucSubject) == 0)
        {
            if (SEC_STRCMP((WE_CHAR *)stNewCert.pucIssuer, (WE_CHAR *)stOldCert.pucIssuer) == 0)
            {
                ucExist = 1; 
                iTryNext = 0;
                if ((*pucCert) == M_SEC_CERTIFICATE_WTLS_TYPE) 
                {
                    if (stOldCert.stCertUnion.stWtls.usModLen != stNewCert.stCertUnion.stWtls.usModLen)
                    { 
                        ucExist = 0; 
                        iTryNext = 1;
                    } 
                    if (iTryNext == 0)
                    {
                        for (iIndexL=0; iIndexL<(stNewCert.stCertUnion.stWtls.usModLen); iIndexL++)
                        {
                            if (stOldCert.stCertUnion.stWtls.pucRsaModulus[iIndexL] != \
                                stNewCert.stCertUnion.stWtls.pucRsaModulus[iIndexL])
                            {
                                ucExist = 0;
                                break;
                            }
                        }
                    }
                }
                else if ((*pucCert) == M_SEC_CERTIFICATE_X509_TYPE)
                {
                    if (stOldCert.stCertUnion.stX509.usPublicKeyValLen != \
                        stNewCert.stCertUnion.stX509.usPublicKeyValLen)
                    {
                        ucExist = 0;
                        iTryNext = 1;
                    }
                    if (iTryNext == 0)
                    {
                        for (iIndexL=0; iIndexL<stNewCert.stCertUnion.stX509.usPublicKeyValLen; iIndexL++)
                        {
                            if (stOldCert.stCertUnion.stX509.pucPublicKeyVal[iIndexL] != \
                                stNewCert.stCertUnion.stX509.pucPublicKeyVal[iIndexL])
                            { 
                                ucExist = 0; 
                                break;
                            }
                        }
                    }
                }   
            }
        }  
        
        if (1 == ucExist)
        {            
            WE_FREE(pucIndexFile);
            Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, ucExist,
                stNewCert.iValidNotBefore, stNewCert.iValidNotAfter, 
                stOldCert.iValidNotBefore, stOldCert.iValidNotAfter, iFileId);
            /* add by Sam [070203] */
            WE_FREE(pucFile);
            return;
        }
        else /* move point to next block in indexfile.*/
        {
            iRes = Sec_WimGetBlockLength(pucP, cCertType, &iBlockLen);
            if (iRes != M_SEC_ERR_OK)
            {
                WE_FREE(pucIndexFile);
                Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
                    ucExist, 0, 0, 0, 0, iFileId);
                return;                
            }
            pucP += iBlockLen;
            /* add by Sam [070203] */
            WE_FREE(pucFile);
        }
    }        
    WE_FREE(pucIndexFile);
    Sec_WimCertExistsResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, 0, 0, 0, 0, 0, 0);
    usCertLen = usCertLen;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimViewNameOfCert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Search all files by the IndexFile, and record the Names in an array by CertOptions.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iCertOptions[IN]:The value of the cert option.     
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimViewNameOfCert(WE_HANDLE hSecHandle, WE_INT32 iCertOptions)
{
    WE_INT32             iNumOfCerts = 0;
    WE_UCHAR             **ppucCertNames = NULL;
    WE_UINT16            *pusNameLen = NULL;
    WE_INT16             *psCharSet = NULL;
    WE_INT8              *pcNameType = NULL;
    WE_INT32             *piCertIds = NULL;
    WE_UINT8             ucUserCertNum = 0;
    WE_UINT8             ucCACertNum = 0;
    WE_UINT8             ucCertNum = 0;
    WE_INT32             iRes = 0;
    WE_INT32             iResult = 0;
    WE_INT32             iIndexI = 0;
    WE_UINT8             *pucAble = NULL;    
    WE_UINT8             *pucCertType = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    } 
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimViewNameOfCertResponse(hSecHandle, M_SEC_ERR_WIM_NOT_INITIALISED, 0, NULL, NULL, NULL, 
            NULL, NULL, NULL, NULL);
        return;
    }
    /* get number of user certificates. */
    if ((iCertOptions == SEC_GET_USER_CERT) || (iCertOptions == (SEC_GET_ROOT_CERT | SEC_GET_USER_CERT)))            
    {
        ucUserCertNum = (WE_UINT8)Sec_WimGetNbrUserCerts(hSecHandle);
    }
    else if (iCertOptions != SEC_GET_ROOT_CERT)
    {
        Sec_WimViewNameOfCertResponse(hSecHandle, M_SEC_ERR_INVALID_PARAMETER, 0, NULL, NULL, NULL, 
            NULL, NULL, NULL, NULL);
        return;
    }
    
    /* get the number of CA certificates.*/
    if ((iCertOptions == SEC_GET_ROOT_CERT) || (iCertOptions == (SEC_GET_ROOT_CERT | SEC_GET_USER_CERT)))
    {
        iRes = Sec_WimGetNbrOfCACerts(hSecHandle, &ucCACertNum);
        if (iRes != M_SEC_ERR_OK)
        {    
            Sec_WimViewNameOfCertResponse(hSecHandle, iRes, 0, NULL, NULL, NULL, 
                NULL, NULL, NULL, NULL);
            return;
        }
    }
    ucCertNum = ucUserCertNum + ucCACertNum;
    if (0 == ucCertNum)
    {
        Sec_WimViewNameOfCertResponse(hSecHandle, M_SEC_ERR_OK, 0, NULL, NULL, NULL, 
            NULL, NULL, NULL, NULL);
        return;
    }
    ppucCertNames = (WE_UCHAR **)WE_MALLOC((WE_UINT32)(ucCertNum) * sizeof(WE_UCHAR *));
    pusNameLen = (WE_UINT16 *)WE_MALLOC((WE_UINT32)(ucCertNum) * sizeof(WE_UINT16));
    psCharSet = (WE_INT16 *)WE_MALLOC((WE_UINT32)(ucCertNum) * sizeof(WE_INT16));
    pcNameType = (WE_INT8 *)WE_MALLOC((WE_UINT32)(ucCertNum) * sizeof(WE_INT8));
    piCertIds = (WE_INT32 *)WE_MALLOC((WE_UINT32)(ucCertNum) * sizeof(WE_INT32));
    pucAble = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(ucCertNum) * sizeof(WE_UINT8));
    pucCertType = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(ucCertNum) * sizeof(WE_UINT8));
    if ((piCertIds == NULL) || (psCharSet == NULL) || (pusNameLen == NULL) 
        || (pcNameType == NULL) || (NULL == ppucCertNames) || (pucAble == NULL) || (pucCertType == NULL))
    {
        Sec_WimViewNameOfCertResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY, 0, NULL, 
            NULL, NULL, NULL, NULL, NULL, NULL);
        if (psCharSet != NULL)
        {
            WE_FREE(psCharSet);
        }
        if (pusNameLen != NULL)
        {
            WE_FREE(pusNameLen);
        }
        if (pcNameType != NULL)
        {
            WE_FREE(pcNameType);
        }
        if (piCertIds != NULL)
        {
            WE_FREE(piCertIds);
        }
        if (ppucCertNames != NULL) 
        {
            WE_FREE(ppucCertNames);
        }
        if (pucAble != NULL) 
        {
            WE_FREE(pucAble);
        }
        if (pucCertType != NULL)
        {
            WE_FREE(pucCertType);
        }
        return;
    }
    
    if ((iCertOptions == SEC_GET_USER_CERT) 
        || (iCertOptions == (SEC_GET_ROOT_CERT | SEC_GET_USER_CERT)))
    {
        iNumOfCerts = 0;
        ucUserCertNum = 0;
        /* get name of user certificates. */
        iResult = Sec_WimViewCertName(hSecHandle, (WE_CHAR *)M_SEC_USERCERT_INDEXFILE_NAME, 
            &(pucCertType[iNumOfCerts]),&(piCertIds[iNumOfCerts]), &(ppucCertNames[iNumOfCerts]), 
            &(pusNameLen[iNumOfCerts]), &(psCharSet[iNumOfCerts]),
            &(pcNameType[iNumOfCerts]), (WE_INT8 *)(&ucUserCertNum), &(pucAble[iNumOfCerts]));        
        if (iResult != M_SEC_ERR_OK)
        { 
            WE_FREE(psCharSet);
            WE_FREE(pusNameLen);
            WE_FREE(pcNameType);        
            WE_FREE(piCertIds);
            WE_FREE(pucAble);
            WE_FREE(pucCertType);
            WE_FREE(ppucCertNames);    
            Sec_WimViewNameOfCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, NULL, NULL, NULL, 
                NULL, NULL, NULL, NULL);
            return;
        }
    }
    if ((iCertOptions == SEC_GET_ROOT_CERT) 
        || (iCertOptions == (SEC_GET_ROOT_CERT | SEC_GET_USER_CERT)))
    {
        iNumOfCerts = ucUserCertNum;
        ucCACertNum = 0;
        /* get name of CA certificates. */
        iResult = Sec_WimViewCertName(hSecHandle, (WE_CHAR *)M_SEC_CA_CERT_INDEXFILE_NAME, 
            &(pucCertType[iNumOfCerts]),&(piCertIds[iNumOfCerts]), &(ppucCertNames[iNumOfCerts]),
            &(pusNameLen[iNumOfCerts]), &(psCharSet[iNumOfCerts]),
            &(pcNameType[iNumOfCerts]), (WE_INT8 *)(&ucCACertNum), &(pucAble[iNumOfCerts]));        
        if (iResult != M_SEC_ERR_OK)
        { 
            WE_FREE(psCharSet);
            WE_FREE(pusNameLen);
            WE_FREE(pcNameType);        
            WE_FREE(piCertIds);
            WE_FREE(pucAble);
            WE_FREE(pucCertType); 
            for (iIndexI=0; iIndexI<(WE_INT32)(ucUserCertNum); iIndexI++)
            {
                WE_FREE(ppucCertNames[iIndexI]); 
            }
            WE_FREE(ppucCertNames);    
            Sec_WimViewNameOfCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, NULL, NULL, NULL, 
                NULL, NULL, NULL, NULL);
            return;
        }
    }
    
    ucCertNum = ucUserCertNum + ucCACertNum;
    Sec_WimViewNameOfCertResponse(hSecHandle, M_SEC_ERR_OK, ucCertNum, pucCertType,
        (const WE_VOID *const *)ppucCertNames,
        pusNameLen, psCharSet, pcNameType,
        (const WE_INT32 *)piCertIds, pucAble);
    WE_FREE(pusNameLen);
    WE_FREE(pcNameType);
    WE_FREE(pucCertType); 
    WE_FREE(psCharSet);
    WE_FREE(piCertIds);
    WE_FREE(pucAble);
    for (iIndexI=0; iIndexI<ucCertNum; iIndexI++)
    {
        WE_FREE(ppucCertNames[iIndexI]);
    }
    WE_FREE(ppucCertNames);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimViewNameOfUserCertByUsage
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Search user cert by KeyType. 
        If find out, get 'name' part of user cert ,parse it, and store it in an array.        
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]:Global data handle.
        ucKeyType[IN]:key type.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.        
=====================================================================================*/
WE_VOID Sec_iWimViewNameOfUserCertByUsage(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType)
{
    WE_UCHAR             **ppucCertNames = NULL;
    WE_INT32             iNumOfCerts = 0;
    WE_UINT16            *pusNameLen = NULL;
    WE_INT16             *psCharSet = NULL;
    WE_INT8              *pcNameType = NULL;
    WE_INT32             *piCertIds = NULL;
    WE_INT32             iRes = 0; 
    WE_UINT32            uiNumOfUserCerts = 0;
    WE_INT32             iIndexJ = 0;
    WE_INT32             iIndexK = 0;
    WE_UINT8             *pucAble = NULL;   
    WE_UINT8             *pucCertType = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimViewNameOfUserCertByTypeResponse(hSecHandle, M_SEC_ERR_WIM_NOT_INITIALISED, 
            0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return; 
    }
    /* get the number of user certificates which satisfy ucKeyType, including Public key certificate.*/
    uiNumOfUserCerts = 0;
    for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
    {
        if ((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) 
            && (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == ucKeyType))
        {
            uiNumOfUserCerts++;
        }
    }    
    if (0 == uiNumOfUserCerts)
    {
        Sec_WimViewNameOfUserCertByTypeResponse(hSecHandle, M_SEC_ERR_OK, 
            0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        return;
    }   
    
    ppucCertNames = (WE_UCHAR **)WE_MALLOC(uiNumOfUserCerts * sizeof(WE_UCHAR *));
    pusNameLen = (WE_UINT16 *)WE_MALLOC(uiNumOfUserCerts * sizeof(WE_UINT16));
    psCharSet = (WE_INT16 *)WE_MALLOC(uiNumOfUserCerts * sizeof(WE_INT16));
    pcNameType = (WE_INT8 *)WE_MALLOC(uiNumOfUserCerts * sizeof(WE_INT8));
    piCertIds = (WE_INT32 *)WE_MALLOC(uiNumOfUserCerts * sizeof(WE_INT32));
    pucAble = (WE_UCHAR *)WE_MALLOC(uiNumOfUserCerts * sizeof(WE_UCHAR));
    pucCertType = (WE_UINT8 *)WE_MALLOC(uiNumOfUserCerts * sizeof(WE_UINT8));
    if ((piCertIds == NULL) || (psCharSet == NULL) || (pucAble == NULL) || (NULL == pucCertType) ||
        (pusNameLen == NULL) || (pcNameType == NULL) || (ppucCertNames == NULL))
    {
        Sec_WimViewNameOfUserCertByTypeResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY, 
            0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (NULL != psCharSet)
        {
            WE_FREE(psCharSet);
        }
        if (NULL != pcNameType)
        {
            WE_FREE(pcNameType);
        }
        if (NULL != pusNameLen)
        {
            WE_FREE(pusNameLen);
        }
        if (NULL != piCertIds)
        {
            WE_FREE(piCertIds);
        }
        if (NULL != ppucCertNames)
        {
            WE_FREE(ppucCertNames);
        }
        if (NULL != pucAble)
        {
            WE_FREE(pucAble);
        }
        if (pucCertType != NULL)
        {
            WE_FREE(pucCertType);
        }
        return;
    }
    (WE_VOID)WE_MEMSET(pucAble, 0, uiNumOfUserCerts*sizeof(WE_UCHAR));
    (WE_VOID)WE_MEMSET(pucCertType, 0, uiNumOfUserCerts*sizeof(WE_UCHAR));

    iNumOfCerts = 0;
    for (iIndexJ=0; ((iIndexJ<M_SEC_USER_CERT_MAX_SIZE) && ((WE_UINT32)iNumOfCerts<uiNumOfUserCerts)); iIndexJ++)
    {
        /* search user certificate by KeyType*/
        if ((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) 
            && (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == ucKeyType))
        {
            piCertIds[iNumOfCerts] = SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId;
            iRes =  Sec_WimGetCertNameByID(hSecHandle, piCertIds[iNumOfCerts],
                &(ppucCertNames[iNumOfCerts]), &(pusNameLen[iNumOfCerts]), 
                &(psCharSet[iNumOfCerts]), &(pcNameType[iNumOfCerts]));    
            if (iRes != M_SEC_ERR_OK)
            { 
                WE_FREE(psCharSet);
                WE_FREE(pusNameLen);
                WE_FREE(pcNameType);        
                WE_FREE(piCertIds);
                WE_FREE(pucAble);
                WE_FREE(pucCertType);
                for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                {
                    WE_FREE(ppucCertNames[iIndexK]);
                }
                WE_FREE(ppucCertNames);    
                Sec_WimViewNameOfUserCertByTypeResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                    0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                return;
            }    
            iNumOfCerts++;
        }
    }
    Sec_WimViewNameOfUserCertByTypeResponse(hSecHandle, M_SEC_ERR_OK, iNumOfCerts, pucCertType,
        (const WE_VOID *const *)ppucCertNames, pusNameLen, psCharSet, pcNameType,
        (const WE_INT32 *)piCertIds, pucAble);
    WE_FREE(pcNameType);
    WE_FREE(psCharSet);
    WE_FREE(pusNameLen);
    WE_FREE(piCertIds);
    WE_FREE(pucAble);
    WE_FREE(pucCertType);
    for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
    {
        WE_FREE(ppucCertNames[iIndexK]);
    }
    WE_FREE(ppucCertNames);
}  

/*=====================================================================================
FUNCTION: 
        Sec_iWimViewNameOfUserCert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Get information(issuer or PublicKeyHash) from the buffer, then search in user certificates.
        Store names of certificates which satisfy requires in an array. 
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucBuf[IN]: pointer to the data buf.
        iBufLen[IN]: the length of the data buf.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        if buffer is NULL, get all.
=====================================================================================*/
WE_VOID Sec_iWimViewNameOfUserCert(WE_HANDLE hSecHandle, const WE_UCHAR *pucBuf,
                                WE_INT32 iBufLen)
{
    WE_UCHAR    **ppucCertNames = NULL;
    WE_INT32    iNumOfCerts = 0;
    WE_UINT16   *pusNameLen = NULL;
    WE_INT16    *psCharSet = NULL;
    WE_INT8     *pcNameType = NULL;
    WE_INT32    *piCertIds = NULL;
    WE_UINT8    ucKeyExchangeSuite = 0; 
    WE_UINT8    ucParameter = 0;   
    WE_UCHAR    *pucBuffer = (WE_UCHAR *)pucBuf;
    WE_UCHAR    *pucIIssuer = NULL;
    WE_UINT8    ucIIssuerLen = 0;
    WE_UINT8    ucDoNotMatch = 0;
    WE_UINT8    ucAllUserCertName = 0;
    WE_UINT8    ucStop = 0;
    WE_UCHAR    aucPublicKeyHash[M_SEC_KEY_HASH_SIZE] = {0};
    WE_UINT8    ucCheckPublicKeyHash = 0;
    St_SecCertificate       stCert = {0};
    WE_INT32    iResult = 0; 
    WE_INT32    iTmpResult = 0; 
    WE_INT32    iRes = 0;
    WE_INT32    iIndexI = 0;
    WE_INT32    iIndexK = 0;
    WE_INT32    iIndexJ = 0;
    WE_INT32    iTmpCertId = 0;
    WE_UINT16   usTmpCertLen = 0;
    WE_UCHAR    *pucTmpName = NULL;
    WE_UINT16   usTmpNameLen = 0;
    WE_INT16    sTmpCharSet = 0;
    WE_INT8     cTmpNameType = 0;
    WE_UCHAR    *pucTmpCert = NULL;
    WE_UINT8    ucCertFormat = 0;    
    WE_UINT32   uiMayNumOfCert = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_WIM_NOT_INITIALISED, 0, NULL, 
            NULL, NULL, NULL, NULL, NULL, NULL);
        return; 
    }    
    
    for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
    {
        if ((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) && 
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == M_SEC_PRIVATE_AUTHKEY) && 
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucPublicKeyCert == 0))
        { 
            uiMayNumOfCert++;
        }
    }
    if (0 == uiMayNumOfCert)
    {
        Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_OK, 0, NULL, 
            NULL, NULL, NULL, NULL, NULL, NULL);
        return; 
    }
    ppucCertNames = (WE_UCHAR **)WE_MALLOC(uiMayNumOfCert * sizeof(WE_UCHAR *));
    pusNameLen = (WE_UINT16 *)WE_MALLOC(uiMayNumOfCert * sizeof(WE_UINT16));
    psCharSet = (WE_INT16 *)WE_MALLOC(uiMayNumOfCert * sizeof(WE_INT16));
    pcNameType = (WE_INT8 *)WE_MALLOC(uiMayNumOfCert * sizeof(WE_INT8));
    piCertIds = (WE_INT32 *)WE_MALLOC(uiMayNumOfCert * sizeof(WE_INT32));
    if ((piCertIds == NULL) || (psCharSet == NULL) || 
        (pusNameLen == NULL) || (pcNameType == NULL) || (NULL == ppucCertNames))
    {
        Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (NULL != psCharSet)
        {
            WE_FREE(psCharSet);
        }
        if (NULL != pcNameType)
        {
            WE_FREE(pcNameType);
        }
        if (NULL != pusNameLen)
        {
            WE_FREE(pusNameLen);
        }
        if (NULL != piCertIds)
        {
            WE_FREE(piCertIds);
        }
        if (NULL != ppucCertNames)
        {
            WE_FREE(ppucCertNames);
        }
        return;
    }
    
    if (iBufLen == 0)  
    {
        ucAllUserCertName = 1;  
    }   
    while (ucStop == 0)
    {
        /* get one block from buffer each time. buffer is the KeyExchangeId of CA certificate.*/
        if (iBufLen > 0)
        { 
            if (NULL == pucBuffer)
            {            
                Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_INVALID_PARAMETER, 0, 
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                WE_FREE(piCertIds);
                WE_FREE(pcNameType);
                WE_FREE(psCharSet);
                WE_FREE(pusNameLen);    
                for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                {
                    WE_FREE(ppucCertNames[iIndexK]);
                }
                WE_FREE(ppucCertNames);
                return;
            }
            ucKeyExchangeSuite = *pucBuffer; /* the first Byte */
            if ((ucKeyExchangeSuite == (WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA) || 
                (ucKeyExchangeSuite == (WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA_512) || 
                (ucKeyExchangeSuite == (WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA_768))
            {
                pucBuffer++;
                iBufLen--;
                ucParameter = *pucBuffer; /* the second Byte.*/
                if(ucParameter != 0)   
                {
                    Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, 
                        (const WE_VOID *const *)ppucCertNames,
                        NULL, NULL, NULL, NULL, NULL, NULL);
                    WE_FREE(piCertIds);
                    WE_FREE(pcNameType);
                    WE_FREE(psCharSet);
                    WE_FREE(pusNameLen);    
                    for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                    {
                        WE_FREE(ppucCertNames[iIndexK]);
                    }
                    WE_FREE(ppucCertNames);
                    return;
                }
                pucBuffer++;
                iBufLen--;
                switch (*pucBuffer) /* the third byte.*/
                {
                case 1  :                     
                    pucBuffer += 3; 
                    ucIIssuerLen = *pucBuffer; 
                    pucBuffer++; 
                    pucIIssuer = (WE_UCHAR *)WE_MALLOC((WE_UINT32)ucIIssuerLen * sizeof(WE_UCHAR));
                    if (NULL == pucIIssuer)
                    {  
                        Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, 
                            NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        WE_FREE(piCertIds);
                        WE_FREE(pcNameType);
                        WE_FREE(psCharSet);
                        WE_FREE(pusNameLen);  
                        for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                        {
                            WE_FREE(ppucCertNames[iIndexK]);
                        }
                        WE_FREE(ppucCertNames);
                        return;
                    }
                    for (iIndexI=0; iIndexI<ucIIssuerLen; iIndexI++)
                    { 
                        pucIIssuer[iIndexI] = *pucBuffer;
                        pucBuffer++;
                    }
                    iBufLen -= 4; 
                    iBufLen -= ucIIssuerLen;
                    break;
                case 255: 
                    pucBuffer++;
                    ucIIssuerLen = *pucBuffer; 
                    pucBuffer++;
                    pucIIssuer = (WE_UCHAR *)WE_MALLOC((WE_UINT32)ucIIssuerLen * sizeof(WE_UCHAR));
                    if (NULL == pucIIssuer)
                    {  
                        Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, 
                            NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        WE_FREE(piCertIds);
                        WE_FREE(pcNameType);
                        WE_FREE(psCharSet);
                        WE_FREE(pusNameLen); 
                        for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                        {
                            WE_FREE(ppucCertNames[iIndexK]);
                        }
                        WE_FREE(ppucCertNames);
                        return;
                    }                    
                    for (iIndexI=0; iIndexI<ucIIssuerLen; iIndexI++)
                    { 
                        pucIIssuer[iIndexI] = *pucBuffer;
                        pucBuffer++;
                    }
                    iBufLen -= 2;
                    iBufLen -= ucIIssuerLen; 
                    break;
                case 254: 
                    pucBuffer++;
                    for (iIndexI=0; iIndexI<M_SEC_KEY_HASH_SIZE; iIndexI++)
                    {
                        aucPublicKeyHash[iIndexI] = *pucBuffer;
                        pucBuffer++;
                    }
                    iBufLen -= 1; 
                    iBufLen -= M_SEC_KEY_HASH_SIZE;
                    ucCheckPublicKeyHash = 1;
                    break;
                case 0  : 
                case 2  :
                default : 
                    Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, 
                        NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                    WE_FREE(piCertIds);
                    WE_FREE(pcNameType);
                    WE_FREE(psCharSet);
                    WE_FREE(pusNameLen);
                    for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                    {
                        WE_FREE(ppucCertNames[iIndexK]);
                    }
                    WE_FREE(ppucCertNames);
                    return;
                }
            }
            else 
            {
                Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, 
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                WE_FREE(piCertIds);
                WE_FREE(pcNameType);
                WE_FREE(psCharSet);
                WE_FREE(pusNameLen);
                for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                {
                    WE_FREE(ppucCertNames[iIndexK]);
                }
                WE_FREE(ppucCertNames);
                return;
            }
        }         
        /* if length of Buffer equals zero, quit next time.*/
        if (iBufLen == 0)
        {
            ucStop = 1;   
        }
        /* buffer or buflen is a wrong parameter*/
        if (iBufLen < 0)
        {
            Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_INVALID_PARAMETER, 0, 
                NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            WE_FREE(piCertIds);
            WE_FREE(pcNameType);
            WE_FREE(psCharSet);
            WE_FREE(pusNameLen);
            for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
            {
                WE_FREE(ppucCertNames[iIndexK]);
            }
            WE_FREE(ppucCertNames);
            return;
        }
        
        /* search in user certificate. */
        for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
        {
            ucDoNotMatch = 0;
            if ((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) && 
                (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == M_SEC_PRIVATE_AUTHKEY) && 
                (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucPublicKeyCert == 0))
            { 
                iTmpCertId = SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId;
                iRes = Sec_WimGetCertNameByID(hSecHandle, iTmpCertId,
                    &(pucTmpName), &(usTmpNameLen), &(sTmpCharSet), &(cTmpNameType));                    
                iResult = Sec_WimGetCertByID(hSecHandle, iTmpCertId, &pucTmpCert, &usTmpCertLen);                
                iTmpResult = Sec_WimGetCertFormatByID(hSecHandle, iTmpCertId, &ucCertFormat);
                if ((iRes!=M_SEC_ERR_OK) || (iResult!=M_SEC_ERR_OK) || (iTmpResult!=M_SEC_ERR_OK))
                { 
                    if (iRes == M_SEC_ERR_OK)
                    {
                        WE_FREE(pucTmpName);
                    }
                    if (iResult == M_SEC_ERR_OK)
                    {
                        WE_FREE(pucTmpCert);
                    }
                    WE_FREE(psCharSet);
                    WE_FREE(pusNameLen);
                    WE_FREE(pcNameType);        
                    WE_FREE(piCertIds);
                    for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                    {
                        WE_FREE(ppucCertNames[iIndexK]);
                    }
                    WE_FREE(ppucCertNames);    
                    if (NULL != pucIIssuer)
                    {
                        WE_FREE(pucIIssuer);
                    }
                    Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 0, 
                        NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                    return;
                }
                if (0 == ucAllUserCertName)
                {
                    if (ucCheckPublicKeyHash == 1)
                    {
                        /* compared hashed CA public key. */
                        for (iIndexI=0; iIndexI<M_SEC_KEY_HASH_SIZE; iIndexI++) 
                        { 
                            if (aucPublicKeyHash[iIndexI] !=
                                SEC_USER_CERT_KEYPAIRS[iIndexJ].aucCApublicKeyHash[iIndexI])
                            {
                                ucDoNotMatch = 1;
                                break;
                            }
                        }
                    }
                    else
                    {  
                        /* compare issuer of user certificate with subject of CA certificate(read out from buffer.). */
                        if (ucCertFormat == M_SEC_CERTIFICATE_WTLS_TYPE)  /* wtls */
                        {
                            iRes = Sec_WtlsCertParse(hSecHandle, pucTmpCert, &(usTmpCertLen), &stCert);
                        }                        
                        else if (ucCertFormat == M_SEC_CERTIFICATE_X509_TYPE)   /* x509 */                                                     
                        {
                            iRes = Sec_X509CertParse(hSecHandle, pucTmpCert, &(usTmpCertLen), &stCert);  
                        }
                        else 
                        {
                            Sec_WimViewNameOfUserCertResponse(hSecHandle,
                                M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, 0, 
                                NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                            WE_FREE(pucTmpCert);
                            WE_FREE(pucTmpName);                            
                            WE_FREE(piCertIds);
                            WE_FREE(pcNameType);
                            WE_FREE(psCharSet);
                            WE_FREE(pusNameLen);
                            for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                            {
                                WE_FREE(ppucCertNames[iIndexK]);
                            }
                            WE_FREE(ppucCertNames);
                            if (NULL != pucIIssuer)
                            {
                                WE_FREE(pucIIssuer);
                            } 
                            return;
                        }
                        
                        if (iRes != M_SEC_ERR_OK)
                        {
                            Sec_WimViewNameOfUserCertResponse(hSecHandle,
                                M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, 0, 
                                NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                            WE_FREE(pucTmpCert);
                            WE_FREE(pucTmpName);                            
                            WE_FREE(piCertIds);
                            WE_FREE(pcNameType);
                            WE_FREE(psCharSet);
                            WE_FREE(pusNameLen);
                            for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                            {
                                WE_FREE(ppucCertNames[iIndexK]);
                            }
                            WE_FREE(ppucCertNames);
                            if (NULL != pucIIssuer)
                            {
                                WE_FREE(pucIIssuer);
                            } 
                            return;
                        }     
                        
                        for (iIndexI=0; iIndexI<ucIIssuerLen; iIndexI++) 
                        {  
                            if (NULL != pucIIssuer)
                            {
                                if (pucIIssuer[iIndexI] != stCert.pucIssuer[iIndexI])
                                {
                                    ucDoNotMatch = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
                /* if matched, store the name of user certificate.*/
                if (ucDoNotMatch == 0)    
                {
                    psCharSet[iNumOfCerts] = sTmpCharSet;
                    ppucCertNames[iNumOfCerts] = pucTmpName;
                    pusNameLen[iNumOfCerts] = usTmpNameLen;
                    pcNameType[iNumOfCerts] = cTmpNameType;
                    piCertIds[iNumOfCerts] = iTmpCertId;
                    iNumOfCerts++;
                }
                else
                {
                    WE_FREE(pucTmpName); 
                }
            }   
            if (NULL != pucTmpCert)
            {
                WE_FREE(pucTmpCert);
                pucTmpCert = NULL;
            }      
        }         
        if (NULL != pucIIssuer)
        {
            WE_FREE(pucIIssuer);
            pucIIssuer = NULL;
        }             
    }   
    Sec_WimViewNameOfUserCertResponse(hSecHandle, M_SEC_ERR_OK, iNumOfCerts, 
        (const WE_VOID *const *)ppucCertNames,  
        pusNameLen, psCharSet, pcNameType,
        (const WE_INT32 *)piCertIds, NULL, NULL);
    WE_FREE(piCertIds);
    WE_FREE(pcNameType);
    WE_FREE(psCharSet);
    WE_FREE(pusNameLen);
    for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
    {
        WE_FREE(ppucCertNames[iIndexK]);
    }
    WE_FREE(ppucCertNames);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimGetKeyIdAndUCert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Search user cert body or certURL by certID. If find out, get public key by certID or cert.
        If certificate type is x509 and connection type is WTLS, insert 'length of Realbody' 
        between 'type' and 'Realbody'.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]:Global data handle.
        iCertId[IN]: Id of certificate. 
        ucConnectionType[IN]: type of connection.    
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Sec_GetUsrCertIm.           
IMPORTANT NOTES:
        omit.        
=====================================================================================*/
WE_VOID Sec_iWimGetKeyIdAndUCert(WE_HANDLE hSecHandle, WE_INT32 iCertId, 
                              WE_UINT8 ucConnectionType)
{
    WE_UCHAR                *pucCert = NULL;
    WE_UCHAR                *pucTempCert = NULL;
    WE_UINT16               usTempCertLen = 0;      
    WE_UCHAR                *pucKeyExId = NULL;
    WE_INT32                iKeyExIdLen = 0;
    WE_UINT16               usCertLen = 0; 
    WE_UINT8                ucCertFormat = 0;
    WE_UINT8                ucFound = 0;
    WE_UCHAR                *pucRealCert = NULL;
    WE_UINT16               usRealCertLen = 0; 
    WE_INT32                iResult = 0;
    WE_INT32                iRes = 0;
    WE_INT32                iIndexJ = 0; 
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_WIM_NOT_INITIALISED, 
            NULL, 0, NULL, 0);
        return; 
    }   
    
    if (ucConnectionType != M_SEC_WTLS_CONNECTION_MODE)
    {
        Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_INVALID_PARAMETER, 
            NULL, 0, NULL, 0);
        return;
    }    
    /* search in user certificates */
    for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
    {
        if ((iCertId == SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId) &&
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucPublicKeyCert == 0) && (ucFound==0))
        {
            iResult = Sec_WimGetCertByID(hSecHandle, iCertId, &pucCert, &usCertLen);        
            if (iResult != M_SEC_ERR_OK)
            {
                Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                    NULL, 0, NULL, 0);
                return;
            } 
            iResult = Sec_WimGetCertFormatByID(hSecHandle, iCertId, &ucCertFormat);
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucCert);
                Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                    NULL, 0, NULL, 0);
                return;
            } 
            pucTempCert = (WE_UCHAR*)WE_MALLOC((usCertLen + 1) * sizeof(WE_UCHAR)); 
            if (NULL == pucTempCert)
            {
                WE_FREE(pucCert);
                Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                    NULL, 0, NULL, 0);
                return;
            } 
            pucTempCert[0] = ucCertFormat;            
            (WE_VOID)WE_MEMCPY(pucTempCert+1, pucCert, usCertLen);
            usTempCertLen = usCertLen + 1;
            WE_FREE(pucCert);
            ucFound = 1;
            break;
        }
    }
    
    if ((1 == ucFound) && (pucTempCert != NULL)) /* find out the certificate.*/
    {
        WE_UCHAR  *pucP = NULL;
        
        pucKeyExId = (WE_UCHAR *)WE_MALLOC(256 * sizeof(WE_UCHAR));
        if (NULL == pucKeyExId)
        {
            WE_FREE(pucTempCert);
            Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY,
                NULL, 0, NULL, 0);
            return; 
        }        
        /* get the hashed public key or subject.*/
        iRes = Sec_WimGetPubKeyHash(hSecHandle, iCertId, &pucKeyExId, &iKeyExIdLen);               
        if (iRes != M_SEC_ERR_OK)
        {
            WE_FREE(pucKeyExId);
            WE_FREE(pucTempCert);
            Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                NULL, 0, NULL, 0);
            return; 
        }         
        
        if (pucTempCert[0] == M_SEC_CERTIFICATE_X509_TYPE) /* X509 certificate */
        { 
            pucRealCert = (WE_UCHAR*)WE_MALLOC((usTempCertLen + 2) * sizeof(WE_UCHAR)); 
            if (NULL == pucRealCert)
            {
                WE_FREE(pucKeyExId);
                WE_FREE(pucTempCert);
                Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR,
                    NULL, 0, NULL, 0);
                return;            
            }
            pucP = pucRealCert;
            *pucP = M_SEC_CERTIFICATE_X509_TYPE;  
            pucP++;
            /* length of pure certificate. */
            usRealCertLen = usTempCertLen - 1;  
            Sec_ExportStrUint16to8(&usRealCertLen, pucP);
            pucP += 2;
            for (iIndexJ=0; iIndexJ<(WE_INT32)(usRealCertLen); iIndexJ++)
            {
                pucP[iIndexJ] = pucTempCert[iIndexJ+1];
            }                
        }
        else  /* wtls certificate */
        {
            Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_OK, pucTempCert,
                usTempCertLen, pucKeyExId, iKeyExIdLen);
            WE_FREE(pucKeyExId);
            WE_FREE(pucTempCert);
            return;
        }        
    }
    else /* not find the certificate.*/
    {
        Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR,
            NULL, 0, NULL, 0);
        return; 
    }
    
    Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_OK, pucRealCert,
        (usRealCertLen + 3), pucKeyExId, iKeyExIdLen);    
    WE_FREE(pucRealCert);
    WE_FREE(pucKeyExId);
    WE_FREE(pucTempCert);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimGetUserCertAndPrivKey
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        get user certificate by Id and take out public key. Get private key from private file.
        compute hash signature using private and public key.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iCertId[IN]: Id of certificate.     
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimGetUserCertAndPrivKey(WE_HANDLE hSecHandle, WE_INT32 iCertId)
{
    WE_UCHAR        *pucCert = NULL;
    WE_UINT16       usCertLen = 0;
    WE_UCHAR        *pucOutPubKey = NULL;
    WE_UINT16       usOutPubKeyLen = 0;
    WE_INT32        iIndexJ = 0; 
    St_SecPubKeyRsa       stPubRsaKey = {0}; 
    St_SecCrptPrivKey     stPrivKey = {0};
    St_SecWimGetUserCertAndPrivKey *pstP = NULL; 
    WE_INT32        iResult = 0;
    WE_UCHAR        *pucTempCert = NULL;
    WE_UINT8        ucCertFormat = 0;
    WE_UCHAR        *pucPrivKey = NULL;
    WE_INT32        iPrivKeyLen = 0;
    WE_INT32        iKeyLen = 0;
    WE_UCHAR        ucKeyUsage = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, M_SEC_ERR_WIM_NOT_INITIALISED,
            NULL, 0, NULL, 0, 0);
        return; 
    }
    
    for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
    {
        if (iCertId == SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId)
        {
            /* get the certificate. */
            iResult = Sec_WimGetCertByID(hSecHandle, iCertId, &pucTempCert, &usCertLen);        
            if (iResult != M_SEC_ERR_OK)
            {
                Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                    NULL, 0, NULL, 0);
                return;
            }     
            /* get the format of certificate. wtls or x509. */
            iResult = Sec_WimGetCertFormatByID(hSecHandle, iCertId, &ucCertFormat);
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucTempCert);
                Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                    NULL, 0, NULL, 0);
                return;
            }  
            pucCert = (WE_UCHAR*)WE_MALLOC((usCertLen + 1) * sizeof(WE_UCHAR)); 
            if (NULL == pucCert)
            {
                WE_FREE(pucTempCert);
                Sec_WimGetUserCertAndKeyIdResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                    NULL, 0, NULL, 0);
                return;
            }
            pucCert[0] = ucCertFormat;            
            (WE_VOID)WE_MEMCPY(pucCert+1, pucTempCert, usCertLen);
            WE_FREE(pucTempCert);
            /* get public key */
            iResult = Sec_WimGetPubKey(hSecHandle, pucCert, &stPubRsaKey);
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucCert);
                Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                    NULL, 0, NULL, 0, 0);
                return;
            }  
            iResult = Sec_WimConvPubKey(stPubRsaKey, &pucOutPubKey, &usOutPubKeyLen);            
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucCert);
                Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, iResult, NULL, 0, NULL, 0, 0);
                return;
            }            
            /* get the private key.*/
            ucKeyUsage = SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage;
            iResult = Sec_WimGetPrivateKey(hSecHandle, ucKeyUsage, &pucPrivKey, &iPrivKeyLen);
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucCert);
                WE_FREE(pucOutPubKey);
                Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, iResult, NULL, 0, NULL, 0, 0);
                return;
            }    
            iKeyLen = Sec_WimGetOrigiLen(pucPrivKey, iPrivKeyLen);           
            stPrivKey.pucBuf = pucPrivKey;
            stPrivKey.usBufLen = (WE_UINT16)iKeyLen;            
            /* compute hash signature using private and public key */
            iResult = Sec_WimCalHashedSign(hSecHandle, iCertId, 
                                stPrivKey, pucOutPubKey, usOutPubKeyLen);                                
            WE_FREE(pucPrivKey);
            WE_FREE(pucOutPubKey);
            if (iResult == M_SEC_ERR_OK) 
            {
                pstP = (St_SecWimGetUserCertAndPrivKey *)WE_MALLOC\
                                (sizeof(St_SecWimGetUserCertAndPrivKey));
                if (NULL == pstP)
                {
                    WE_FREE(pucCert);
                    Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle,
                        M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, NULL, 0, 0);
                    return;
                }
                pstP->pucCert = pucCert;
                pstP->usCertLen = usCertLen;
                pstP->iCertId = iCertId;
                iResult = Sec_WimAddNewElement(hSecHandle, iCertId, 
                                 M_SEC_WIM_GET_USER_CERT_AND_SIGNED_KEY, pstP);
                if(iResult != M_SEC_ERR_OK)
                {
                    WE_FREE(pstP);
                    WE_FREE(pucCert);
                    Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle,
                        M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, NULL, 0, 0);
                    return;
                }
            }
            else
            {
                WE_FREE(pucCert);
                Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, 
                    NULL, 0, NULL, 0, 0);
            }
            return;
        }             
    }
    Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, M_SEC_ERR_NOT_FOUND,
        NULL, 0, NULL, 0, 0);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimGetUserCertAndPrivKeyResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        response of get user certificate and private key.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.    
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimGetUserCertAndPrivKeyResp (WE_HANDLE hSecHandle, 
                St_SecWimElement *pstElement, St_SecCrptComputeSignatureResp *pstParam)
{
    St_SecWimGetUserCertAndPrivKey *pstP = NULL;
    WE_INT32  iResult = 0;
    
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    pstP = pstElement->pvPointer;
    /*modified by Bird 070118*/
    if(NULL == pstParam)
    {
         Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY, 
            NULL, 0, NULL, 0, 0);
        
    }
    else
    {
        iResult = pstParam->sResult;
        if (iResult != M_SEC_ERR_OK) 
        {
            Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, M_SEC_ERR_INVALID_PARAMETER, 
                NULL, 0, NULL, 0, 0);
        } 
        else 
        {
            Sec_WimGetUserCertAndPrivKeyResponse(hSecHandle, iResult, pstP->pucCert, 
                pstP->usCertLen, pstParam->pucSig, pstParam->sSigLen, M_SEC_SP_RSA);
        }
    }
    WE_FREE(pstP->pucCert);
    WE_FREE(pstP);
    WE_FREE(pstElement);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimDelPrivKeysAndUCertsByUsage
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        delete certificate and relevant private key file, then update M_SEC_USER_CERT_KEYPAIRS 
        and M_SEC_USERCERT_PRIVKEY_INDEX_HEADER file.
        Search in IndexFile and delete other private key file whose usage is 'ucKeyType'.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        ucKeyType[IN]: type of private key.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimDelPrivKeysAndUCertsByUsage(WE_HANDLE hSecHandle, 
                                         WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
    WE_INT32             iIndexI = 0;
    WE_INT32             iResult = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimDelPrivKeysAndUserCertsByTypeResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }  

    if ((ucKeyType != M_SEC_PRIVATE_NONREPKEY) && (ucKeyType != M_SEC_PRIVATE_AUTHKEY))
    {
        Sec_WimDelPrivKeysAndUserCertsByTypeResponse(hSecHandle, iTargetID,
                    M_SEC_ERR_INVALID_PARAMETER);
        return;
    } 
    
    /* search in user certificate. */
    for (iIndexI=0; iIndexI<M_SEC_USER_CERT_MAX_SIZE; iIndexI++)
    {
        if (SEC_USER_CERT_KEYPAIRS[iIndexI].ucKeyUsage == ucKeyType)
        {   
            /* delete both user certificate and public key certificate. */
            iResult = Sec_WimDelFile(hSecHandle, M_SEC_USER_CERT_HEADER, SEC_USER_CERT_KEYPAIRS[iIndexI].iUcertId);            
            if (iResult != M_SEC_ERR_OK)
            {
                Sec_WimDelPrivKeysAndUserCertsByTypeResponse(hSecHandle, iTargetID, iResult);
                return; 
            }                                
            /* clear data in  SEC_USER_CERT_KEYPAIRS relevant to user certificate. */
            iResult = Sec_WimCleanUCertData(hSecHandle, SEC_USER_CERT_KEYPAIRS[iIndexI].iUcertId);
            if (iResult != M_SEC_ERR_OK)
            {
                Sec_WimDelPrivKeysAndUserCertsByTypeResponse(hSecHandle, iTargetID, iResult);
                return; 
            }           
        }
    }     
    /* update M_SEC_USERCERT_PRIVKEY_INDEX_HEADER file. */
    Sec_WimSaveKeyPairInfoToFile(hSecHandle);     
    /* delete private key file. */
    Sec_WimDelPrivKey(hSecHandle, ucKeyType);    
    
    /* delete 20070208
    if (ucKeyType == M_SEC_PRIVATE_AUTHKEY) 
    {
        for (iIndexI=0; iIndexI<M_SEC_SESSION_CACHE_SIZE; iIndexI++)
        {
            Sec_WimClrSessionInfo(hSecHandle, iIndexI);
            Sec_WimClrPeerInfo(hSecHandle, iIndexI);
        }
    }*/
    Sec_WimDelPrivKeysAndUserCertsByTypeResponse(hSecHandle, iTargetID, M_SEC_ERR_OK);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimSelfSignedCert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        judge if the certificate is self-signed. 
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucCert[IN]: pointer to the certificate which need to be verified.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        For self-signed certificate, 'issuer' and 'subjiect' are same.
=====================================================================================*/
WE_VOID Sec_iWimSelfSignedCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                            const WE_UCHAR *pucCert) 
{    
    St_SecCertificate  stCert = {0};
    WE_UINT8           ucSelfSigned = 0;
    WE_UINT16          usIssuerLen = 0;
    WE_UINT16          usSubjectLen = 0;
    WE_UINT16          usCertLen = 0;
    WE_INT32           iResult = 0;
    
    if ((NULL == hSecHandle) || (NULL == pucCert))
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimSelfSignedCertResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED, ucSelfSigned);
        return;
    }
    
    /* parse the certificate.*/
    if ((*pucCert) == M_SEC_CERTIFICATE_WTLS_TYPE)     /* wtls */
    { 
        iResult = Sec_WtlsCertParse(hSecHandle, pucCert+1, &usCertLen, &stCert);    
        usIssuerLen = stCert.stCertUnion.stWtls.ucIssuerLen;  
        usSubjectLen = stCert.stCertUnion.stWtls.ucSubjectLen;
    }
    else if ((*pucCert) == M_SEC_CERTIFICATE_X509_TYPE) /* x509 */
    {
        iResult = Sec_X509CertParse(hSecHandle, pucCert+1, &usCertLen, &stCert); 
        usIssuerLen = stCert.stCertUnion.stX509.usIssuerLen;  
        usSubjectLen = stCert.stCertUnion.stX509.usSubjectLen;
    }
    else
    {
        Sec_WimSelfSignedCertResponse(hSecHandle, iTargetID,
            M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, ucSelfSigned);
        return;
    }
    
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimSelfSignedCertResponse(hSecHandle, iTargetID, iResult, ucSelfSigned);
        return;
    }  
    
    if ((usIssuerLen == usSubjectLen) && 
        !WE_MEMCMP(stCert.pucIssuer, stCert.pucSubject, usSubjectLen)) 
    {
        ucSelfSigned = 1;
    }    
    
    Sec_WimSelfSignedCertResponse(hSecHandle, iTargetID, iResult, ucSelfSigned); 
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimChkCACert
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        verify if the certificate is root certificate. 
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucCert[IN]: pointer to the certificate which need to be verified.
        usCertLen[IN]: length of certificate.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        The number of root certificate chain is one.
        Verify root certificate with its own public key, because root certificate is self-signed.
=====================================================================================*/
WE_VOID Sec_iWimChkCACert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                            const WE_UCHAR *pucCert, WE_UINT16 usCertLen)
{
    St_SecCertificate    stCert = {0};
    St_SecPubKeyRsa      stPubKey = {0}; 
    WE_INT32             iRes = 0;
    WE_UINT8             ucCertsNum = 0;
    
    if ((NULL == hSecHandle) || (NULL == pucCert))
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimVerifyRootCertResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    
    /* take out the number of certificate chain.*/
    iRes = Sec_WimVerifiCertChain(hSecHandle, pucCert, usCertLen, NULL, &ucCertsNum); 
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimVerifyRootCertResponse(hSecHandle, iTargetID, iRes);
        return;
    }
    if (ucCertsNum != 1)
    {
        Sec_WimVerifyRootCertResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    
    usCertLen = 0;
    /* parse the root certificate ,get public key and verify itself. */
    if ((*pucCert) == M_SEC_CERTIFICATE_WTLS_TYPE) 
    {
        iRes = Sec_WtlsCertParse(hSecHandle, pucCert+1, &usCertLen, &stCert);   
        stPubKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
        stPubKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
        stPubKey.usModLen = stCert.stCertUnion.stWtls.usModLen;
        stPubKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
        if (iRes == M_SEC_ERR_OK)
        {
            iRes = Sec_WimChkWTLSCert(hSecHandle, iTargetID, stPubKey, pucCert+1);
        }
    }
    else if ((*pucCert) == M_SEC_CERTIFICATE_X509_TYPE)   
    {
        iRes = Sec_X509CertParse(hSecHandle, pucCert+1, &usCertLen, &stCert); 
        if (iRes == M_SEC_ERR_OK)
        {
            iRes = Sec_WimChkX509Cert(hSecHandle, iTargetID, 
                stCert.stCertUnion.stX509.pucPublicKeyVal, 
                pucCert+1, M_SEC_WTLS_CONNECTION_MODE, 1, M_SEC_CERT_USAGE_ROOT);
        }
    }
    else
    {
        Sec_WimVerifyRootCertResponse(hSecHandle, iTargetID, M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE);
        return;
    }
    
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimVerifyRootCertResponse(hSecHandle, iTargetID, iRes);
    }
    else
    {
        iRes = Sec_WimAddNewElement(hSecHandle, iTargetID, M_SEC_WIM_VERIFY_ROOT_CERTIFICATE,NULL);
        if(iRes != M_SEC_ERR_OK)
        {
            Sec_WimVerifyRootCertResponse(hSecHandle, iTargetID, iRes);
        }
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimChkCACertResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        response of verify root certificate. 
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimChkCACertResp (WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                            St_SecCrptVerifySignatureResp *pstParam)
{
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    if(NULL == pstParam)
    {
        Sec_WimVerifyRootCertResponse(hSecHandle, pstElement->iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        
    }
    else
    {
        Sec_WimVerifyRootCertResponse(hSecHandle, pstElement->iTargetID, pstParam->sResult);
    }
    WE_FREE(pstElement);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimStoreUCertKeyPairInfo
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Store user certificate data.
ARGUMENTS PASSED:
        Omit.        
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        none.
=====================================================================================*/
WE_VOID Sec_iWimStoreUCertKeyPairInfo(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                               WE_INT32 iUserCertId, WE_INT32 iUserCertCount,
                               WE_INT32 iPrivKeyId, WE_INT32 iPrivKeyCount, 
                               WE_UINT8 ucPrivKeyUsage, WE_UINT8 ucAlg, 
                               const WE_UCHAR *pucPublicKeyHash, 
                               const WE_UCHAR *pucCaPublicKeyHash, 
                               WE_INT32 iURLCount, WE_UINT8 ucPublicKeyCert,
                               WE_INT32 iNameCount, WE_INT8 cNameType)
{
    WE_INT32  iResult = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimStoreUserCertDataResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    /* Store the data about user certificate in User_PrivateKey Index file.*/
    iResult = Sec_WimUpdataUCertKeyPairInfo(hSecHandle, iUserCertId, iUserCertCount, 
        iPrivKeyCount, ucPrivKeyUsage, pucPublicKeyHash, pucCaPublicKeyHash, 
        ucPublicKeyCert, iNameCount, cNameType);  
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimStoreUserCertDataResponse(hSecHandle, iTargetID, iResult);
        return;        
    }
    
    Sec_WimStoreUserCertDataResponse(hSecHandle, iTargetID, M_SEC_ERR_OK);
    iPrivKeyId = iPrivKeyId;
    ucAlg = ucAlg;
    iURLCount = iURLCount;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimKeyPairGen
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Generate key Pair.       
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        iTargetID[IN]:The value of the wid.
        ucKeyType[IN]:The type of the key.
        pcPin[IN]: pin of private key.
        ucAlg[IN]:The value of the alg.   
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimKeyPairGen(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType,
                        const WE_CHAR *pcPin, WE_UINT8 ucAlg)
{
    WE_INT32         iResult = M_SEC_ERR_OK;
    St_SecWimKeyPairGen *pstP = NULL; 
    WE_INT32         iModBits = 1024;
    WE_INT8          cFound = 0;
    WE_INT32         iIndexI = 0;
    E_WE_ERROR       eRes = E_WE_OK;
    #ifndef G_SEC_CFG_SHOW_PIN 
    WE_CHAR* pcFileName = NULL;
    #endif
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (Sec_WimGetNumUserAndPubKeyCerts(hSecHandle) == M_SEC_USER_CERT_MAX_SIZE)
    {
        Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, M_SEC_ERR_REACHED_USER_CERT_LIMIT, 
            ucKeyType, NULL, 0, NULL, 0);
        return; 
    } 
    
    if (((ucKeyType != M_SEC_PRIVATE_AUTHKEY) && (ucKeyType != M_SEC_PRIVATE_NONREPKEY))
        || (ucAlg != M_SEC_SP_RSA))
    {
        Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, 
            ucKeyType, NULL, 0, NULL, 0);
        return;
    }
    
#ifndef G_SEC_CFG_SHOW_PIN /*added by Bird 061121*/
    if(M_SEC_PRIVATE_AUTHKEY==ucKeyType)
    {
        pcFileName = M_SEC_AUTH_PRIVKEY_NAME;
    }
    else
    {
        pcFileName = M_SEC_NON_REP_PRIVKEY_NAME;
    }
    iResult = Sec_WimCheckFileExist(hSecHandle, pcFileName);
    if (iResult != M_SEC_ERR_OK) 
    {
        pcPin = "12345";
        pcFileName = NULL;
        /* clean the key pairs file and delete the public key file. */
        for (iIndexI=0; iIndexI<M_SEC_USER_CERT_MAX_SIZE; iIndexI++)
        {
            if ((SEC_USER_CERT_KEYPAIRS[iIndexI].ucPublicKeyCert == 1)
                && (SEC_USER_CERT_KEYPAIRS[iIndexI].ucKeyUsage == ucKeyType))
            {
                iResult = Sec_WimDelBlock(hSecHandle, SEC_USER_CERT_KEYPAIRS[iIndexI].iUcertId, M_SEC_USER_CERT_HEADER); 
                iResult = Sec_WimAcqFileName(M_SEC_USER_CERT_HEADER, 
                                (WE_UINT32)(SEC_USER_CERT_KEYPAIRS[iIndexI].iUcertId), &pcFileName);
                eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
                if (pcFileName != NULL)
                {
                    WE_FREE(pcFileName);
                    pcFileName = NULL;
                }
                Sec_WimClearOneUCertKeyInfo(&(SEC_USER_CERT_KEYPAIRS[iIndexI]));
            } 
        }
        Sec_WimSaveKeyPairInfoToFile(hSecHandle);        
    }
    else
    {
        for (iIndexI=0; iIndexI<M_SEC_USER_CERT_MAX_SIZE; iIndexI++)
        {
            /* find the public key and don't store it. */
            if ((SEC_USER_CERT_KEYPAIRS[iIndexI].ucPublicKeyCert == 1)
                && (SEC_USER_CERT_KEYPAIRS[iIndexI].ucKeyUsage == ucKeyType))
            {
                cFound = 1;
                break;
            } 
        }
        if (1 == cFound)  /* public key and private key both exist. */
        {
            Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, 
                        M_SEC_ERR_OK, ucKeyType, NULL, 0, NULL, 0);
            return; 
        }
        else
        { 
            /* private key exists and public key doesn't exist. another call is using for generating the key pairs.*/           
            Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, 
                        M_SEC_ERR_BUSY, ucKeyType, NULL, 0, NULL, 0);
            return; 
        }
    }
#endif

    /* If the corresponding private key exists, don't make the key pairs. */
    if (NULL == pcPin)
    {
        Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, 
                        M_SEC_ERR_OK, ucKeyType, NULL, 0, NULL, 0);
        return;
    }    
    else /* generate the key pairs.*/
    {
        iResult = Sec_PKCRsaGenerateKeyPair(hSecHandle, iTargetID, iModBits);
        if (iResult != M_SEC_ERR_OK)
        {
            Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                ucKeyType, NULL, 0, NULL, 0);
            return; 
        }  
    }
    
    pstP = (St_SecWimKeyPairGen *)WE_MALLOC(sizeof(St_SecWimKeyPairGen));
    if (NULL == pstP)
    {
        Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
            ucKeyType, NULL, 0, NULL, 0);
        return;
    } 
    pstP->iTargetID = iTargetID;
    pstP->ucKeyType = ucKeyType;

    pstP->pcPin = SEC_STRDUP(pcPin);
    pstP->ucAlg = ucAlg; 
    iResult = Sec_WimAddNewElement(hSecHandle, iTargetID, M_SEC_WIM_GENERATE_KEYPAIR, pstP);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pstP->pcPin);
        WE_FREE(pstP);
        Sec_WimKeyPairGenResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
            ucKeyType, NULL, 0, NULL, 0);
        return;
    }
    eRes = eRes;    
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimKeyPairGenResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        If Pin is NULL, have read out key pairs, compute hashed result directly; 
        If Pin is not NULL, store the private key, and use it hashed public key.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN]: Global queue.
        pstParam[IN]: The parameter value.   
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID  Sec_iWimKeyPairGenResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                             St_SecCrptGenKeypairResp *pstParam)
{
    St_SecWimKeyPairGen   *pstP = NULL;
    WE_INT32              iResult = 0;
    WE_UCHAR              *pucPubkey = NULL;
    WE_UINT16             usPubkeyLen = 0;
    WE_UCHAR              *pucPrikey = NULL;
    WE_INT32              iPrikeyLen = 0;
    WE_UINT32             uiCountK = 0;
    WE_CHAR               *pcFileName=NULL;
    
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    pstP = pstElement->pvPointer;
    if (NULL == pstParam)
    {
        Sec_WimKeyPairGenResponse(hSecHandle, pstP->iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
            pstP->ucKeyType, NULL, 0,  NULL, 0);
        WE_FREE(pstP->pcPin);
        WE_FREE(pstP);
        WE_FREE(pstElement);
        return;
    }
    iResult = pstParam->sResult;
    if ((iResult != M_SEC_ERR_OK) || (NULL == pstP->pcPin))
    {
        Sec_WimKeyPairGenResponse(hSecHandle, pstP->iTargetID, iResult, 
            pstP->ucKeyType, NULL, 0,  NULL, 0);
        WE_FREE(pstP->pcPin);
        WE_FREE(pstP);
        WE_FREE(pstElement);
        return;
    }
    pucPrikey = pstParam->stPrivKey.pucBuf;
    iPrikeyLen = pstParam->stPrivKey.usBufLen;
    
    /* generate the keypairs.*/    
    if (M_SEC_PRIVATE_AUTHKEY == pstP->ucKeyType)
    {
        pcFileName = M_SEC_AUTH_PRIVKEY_NAME;
    }        
    else
    {
        pcFileName = M_SEC_NON_REP_PRIVKEY_NAME;
    }
    /* the private key has been generated by another call function which is using for generating the key pairs. */
    iResult= Sec_WimCheckFileExist(hSecHandle, pcFileName);  
    if (iResult == M_SEC_ERR_OK)  
    {
        Sec_WimKeyPairGenResponse(hSecHandle, pstP->iTargetID, 
            M_SEC_ERR_BUSY, pstP->ucKeyType, NULL, 0, NULL, 0);
        (WE_VOID)WE_MEMSET(pucPrikey, 0, (WE_UINT32)iPrikeyLen);
        WE_FREE(pstP->pcPin);
        WE_FREE(pstP);
        WE_FREE(pstElement);
        return;
    }
    
    iResult = Sec_WimSavePrivateKey(hSecHandle, pucPrikey, iPrikeyLen, pstP->ucKeyType, 
        (const WE_INT8 *)(pstP->pcPin), &uiCountK);            
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimDelPrivKey(hSecHandle, pstP->ucKeyType);
    } 
    else
    {
        /* get the public key. If fail, delete the private key file. */
        if (pstP->ucAlg == M_SEC_SP_RSA)
        {
            iResult = Sec_WimConvPubKey(pstParam->stPubKey, &pucPubkey, &usPubkeyLen);
        }
        if (iResult != M_SEC_ERR_OK)
        { 
            Sec_WimDelPrivKey(hSecHandle, pstP->ucKeyType);
        }
        else
        {
            iResult = Sec_WimCalHashedSign(hSecHandle, pstP->iTargetID,
                pstParam->stPrivKey, pucPubkey, usPubkeyLen);              
            if (iResult == M_SEC_ERR_OK)
            {
                (WE_VOID)WE_MEMSET(pucPrikey, 0, (WE_UINT32)iPrikeyLen);
                pstP->uiCountK = uiCountK;
                pstP->pucPubKey = pucPubkey;
                pstP->usPubKeyLen = usPubkeyLen;
                pstElement->iState = M_SEC_WIM_PRODUCE_PUBKEY_CERT;
                pstElement->hItype = hSecHandle;
                Sec_WimAddPreassignElement(hSecHandle, pstElement);      
                return;
            }
            Sec_WimDelPrivKey(hSecHandle, pstP->ucKeyType);
        }
    }                
    
    (WE_VOID)WE_MEMSET(pucPrikey, 0, (WE_UINT32)iPrikeyLen);
    Sec_WimKeyPairGenResponse(hSecHandle, pstP->iTargetID, iResult, 
        pstP->ucKeyType, NULL, 0,  NULL, 0);
    if (NULL != pucPubkey)
    {
        WE_FREE(pucPubkey);
    }
    WE_FREE(pstP->pcPin);
    WE_FREE(pstP);
    WE_FREE(pstElement);
    
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimKeyPairGenSignResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Store public key and change M_SEC_WIM_ASTUSERCERTKEYPAIR.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN]: Global queue.
        pstParam[IN]: The parameter value.    
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimKeyPairGenSignResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                                St_SecCrptComputeSignatureResp *pstParam)
{
    St_SecWimKeyPairGen *pstP = NULL;
    WE_INT32  iResult = 0;    
    
    if ((NULL==hSecHandle) || (NULL==pstElement) )
    {
        return;
    }
    pstP = pstElement->pvPointer;
    /*modified by Bird 070118*/
    if (NULL == pstParam)
    {
        Sec_WimKeyPairGenResponse(hSecHandle, pstP->iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
                                        pstP->ucKeyType,
                                        pstP->pucPubKey, 
                                        pstP->usPubKeyLen,
                                        NULL, 0);
        WE_FREE(pstP->pucPubKey);
        WE_FREE(pstP->pcPin);
        WE_FREE(pstP);
        WE_FREE(pstElement);    
        return;
    }
    iResult = pstParam->sResult;
    
    if (iResult != M_SEC_ERR_OK)    
    { 
        Sec_WimDelPrivKey(hSecHandle, pstP->ucKeyType);
    }
    else /* store public key.*/
    {    
        if (pstP->ucAlg == M_SEC_SP_RSA)
        {
            iResult = Sec_WimGenPubKeyAndUCertInfo(hSecHandle, pstP->pucPubKey, pstP->usPubKeyLen, 
                pstP->ucKeyType, (WE_INT32)(pstP->uiCountK));
        }           
        if (iResult != M_SEC_ERR_OK)
        {
            Sec_WimDelPrivKey(hSecHandle, pstP->ucKeyType);
        }
    }
    Sec_WimKeyPairGenResponse(hSecHandle, pstP->iTargetID, iResult, pstP->ucKeyType,
        pstP->pucPubKey, pstP->usPubKeyLen, pstParam->pucSig, pstParam->sSigLen);
    WE_FREE(pstP->pucPubKey);
    WE_FREE(pstP->pcPin);
    WE_FREE(pstP);
    WE_FREE(pstElement);    
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsCalSign
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        If find user certificate matching to KeyExId, that means the private key can be used compute signature.
        Read out the private key and then compute the signature.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucKeyId[IN]: Pointer to the key exchange Id.
        iKeyIdLen[IN]: length of the key exchange id.
        pucBuf[IN]: pointer to the data buffer.
        iBufLen[IN]: The length of the buffer.
        ucAlg[IN]: signature algorithm.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimWtlsCalSign(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                  const WE_UCHAR *pucKeyExId, WE_INT32 iKeyExIdLen,
                                  const WE_UCHAR *pucBuf, WE_INT32 iBufLen, WE_UINT8 ucAlg)
{
    WE_UINT8           ucDoNotMatch = 0;
    WE_UINT8           ucKeyFound = 0;
    WE_UCHAR           *pucPrivKey = NULL;
    WE_INT32           iPrivKeyLen = 0;
    WE_UCHAR           *pucIdentifier = NULL;
    WE_INT32           iIdentifierLen = 0;
    WE_INT32           iCheckPublicKeyHash = 0;
    WE_UCHAR           *pucCert = NULL;
    WE_UINT16          usCertLen = 0;
    St_SecCertificate  stSpCert = {0};
    WE_UINT8           ucSubjectLen = 0;  
    WE_UCHAR           *pucBuffer = (WE_UCHAR *)pucKeyExId;
    St_SecCrptPrivKey  stCrptPrivKey = {0};
    WE_INT32           iRes = 0;
    WE_INT32           iResult = 0;
    WE_INT32           iIndexI = 0;
    WE_INT32           iIndexJ = 0;
    WE_UCHAR           ucCertFormat = 0;
    WE_INT32           iCertId = 0;
    WE_UCHAR           ucKeyUsage = 0;
    WE_INT32           iKeyLen = 0;    
    
    if ((NULL == hSecHandle) || (NULL == pucBuffer))
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
            M_SEC_ERR_WIM_NOT_INITIALISED, NULL, 0);
        return;
    }
    if (ucAlg != M_SEC_SP_RSA)
    {
        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
            M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
        return;  
    }
    
    /* get 'Identifier' from Buffer.*/
    switch (*pucBuffer)   
    {
    case 1  :        
        pucBuffer += 3;
        iIdentifierLen = *pucBuffer;
        pucBuffer++; 
        pucIdentifier = (WE_UCHAR *)WE_MALLOC((WE_UINT32)(iIdentifierLen) * sizeof(WE_UCHAR));
        if (NULL == pucIdentifier)
        {
            Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
            return;
        }
        for (iIndexI=0; iIndexI<iIdentifierLen; iIndexI++)
        { 
            pucIdentifier[iIndexI] = *pucBuffer;
            pucBuffer++;
        }
        break;
    case 255: 
        pucBuffer++;
        iIdentifierLen = *pucBuffer; 
        pucBuffer++; 
        pucIdentifier = (WE_UCHAR *)WE_MALLOC((WE_UINT32)(iIdentifierLen) * sizeof(WE_UCHAR));
        if (NULL == pucIdentifier)
        {
            Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
            return;
        }
        for (iIndexI=0; iIndexI<iIdentifierLen; iIndexI++)
        { 
            pucIdentifier[iIndexI] = *pucBuffer;
            pucBuffer++;
        }
        break;
    case 254: 
        pucBuffer++;
        iIdentifierLen = M_SEC_KEY_HASH_SIZE;
        pucIdentifier=(WE_UCHAR *)WE_MALLOC((WE_UINT32)iIdentifierLen * sizeof(WE_UCHAR));
        if (NULL == pucIdentifier)
        {
            Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
            return;
        }
        for (iIndexI=0; iIndexI<iIdentifierLen; iIndexI++)
        {
            pucIdentifier[iIndexI] = *pucBuffer;
            pucBuffer++;
        }
        iCheckPublicKeyHash = 1;
        break;
    case 0  :
    case 2  : 
    default : 
        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
            M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
        return;
    }   
    
    /* search user certificate matched 'Identifier' */
    while ((!ucKeyFound) && (iIndexJ<M_SEC_USER_CERT_MAX_SIZE))
    {
        ucDoNotMatch = 0;
        if((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) && 
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == M_SEC_PRIVATE_AUTHKEY) && 
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucPublicKeyCert == 0))
        {
            if (iCheckPublicKeyHash == 1) /*type is 254, compare the hashed public key.*/
            {
                for (iIndexI=0; iIndexI<M_SEC_KEY_HASH_SIZE; iIndexI++)
                { 
                    if (pucIdentifier[iIndexI] !=
                        SEC_USER_CERT_KEYPAIRS[iIndexJ].aucPublicKeyHash[iIndexI])
                    {
                        ucDoNotMatch = 1;
                        break;
                    }
                }
            }
            else  /*type is not 254*/
            {
                iCertId = SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId;
                /* get the certificate. */
                iRes = Sec_WimGetCertByID(hSecHandle, iCertId, &pucCert, &usCertLen);
                if (iRes != M_SEC_ERR_OK)
                {                 
                    WE_FREE(pucIdentifier);
                    Sec_WimWtlsComputeSignatureResponse(hSecHandle,
                        iTargetID, iRes, NULL, 0);
                    return;
                }
                /* get the format of certificate.*/
                iRes = Sec_WimGetCertFormatByID(hSecHandle, iCertId, &ucCertFormat);
                if (iRes != M_SEC_ERR_OK)
                {                       
                    WE_FREE(pucCert);            
                    WE_FREE(pucIdentifier);
                    Sec_WimWtlsComputeSignatureResponse(hSecHandle,
                        iTargetID, iRes, NULL, 0);
                    return;
                }
                /* get the 'subject' of user certificate.*/
                if (ucCertFormat == M_SEC_CERTIFICATE_WTLS_TYPE)
                {
                    iRes = Sec_WtlsCertParse(hSecHandle, pucCert, &usCertLen, &stSpCert);
                    if (iRes != M_SEC_ERR_OK)
                    {
                        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
                            M_SEC_ERR_BAD_CERTIFICATE, NULL, 0);
                        WE_FREE(pucCert);
                        WE_FREE(pucIdentifier);
                        return;
                    }
                    ucSubjectLen = stSpCert.stCertUnion.stWtls.ucSubjectLen;
                }
                if (ucCertFormat == M_SEC_CERTIFICATE_X509_TYPE)
                {
                    iRes = Sec_X509CertParse(hSecHandle, pucCert, &usCertLen, &stSpCert);
                    if (iRes != M_SEC_ERR_OK)
                    {                        
                        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
                            M_SEC_ERR_BAD_CERTIFICATE, NULL, 0);
                        WE_FREE(pucCert);
                        WE_FREE(pucIdentifier);
                        return;
                    }               
                    
                    if (stSpCert.stCertUnion.stX509.usSubjectLen>255)
                    {
                        ucSubjectLen = 255;
                    }
                    else 
                    {
                        ucSubjectLen = (WE_UINT8)stSpCert.stCertUnion.stX509.usSubjectLen;
                    }
                }
                
                if (iIdentifierLen == ucSubjectLen)
                {
                    for (iIndexI=0; iIndexI<iIdentifierLen; iIndexI++)
                    { 
                        if (pucIdentifier[iIndexI] != stSpCert.pucSubject[iIndexI])
                        {
                            ucDoNotMatch = 1;
                            break;
                        }
                    }
                }
                else
                {
                    ucDoNotMatch = 1;
                }                
                WE_FREE(pucCert); 
            }  
            /* find a matched user certificate, take out the authentication private key.*/
            if (ucDoNotMatch == 0)
            {
                ucKeyUsage = SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage;
                iResult = Sec_WimGetPrivateKey(hSecHandle, ucKeyUsage, &pucPrivKey, &iKeyLen);
                if (iResult != M_SEC_ERR_OK) 
                {                    
                    Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, 
                        M_SEC_ERR_BAD_CERTIFICATE, NULL, 0);
                    WE_FREE(pucIdentifier);
                    return;
                } 
                iPrivKeyLen = Sec_WimGetOrigiLen(pucPrivKey, iKeyLen); 
                ucKeyFound = 1;
            }            
        }
        iIndexJ++;  
    }    
    
    if (!ucKeyFound)
    {  
        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, M_SEC_ERR_MISSING_KEY, NULL, 0);
        WE_FREE(pucIdentifier);
        return;
    }    
    
    stCrptPrivKey.pucBuf = pucPrivKey;
    stCrptPrivKey.usBufLen = (WE_UINT16)iPrivKeyLen;  
    /* compute signature.*/
    iRes = Sec_PKCRsaComputeHashedSignature(hSecHandle, iTargetID, stCrptPrivKey, pucBuf, iBufLen);    
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pucPrivKey);
        WE_FREE(pucIdentifier);
        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
        return;
    }      
    WE_FREE(pucPrivKey);    
    WE_FREE(pucIdentifier);
    iRes = Sec_WimAddNewElement (hSecHandle, iTargetID, M_SEC_WIM_COMPUTE_SIGNATURE, NULL);
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimWtlsComputeSignatureResponse(hSecHandle, iTargetID, iRes, NULL, 0);
        return;
    }      
    iKeyExIdLen = iKeyExIdLen;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsCalSignResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        response of compute signature.        
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimWtlsCalSignResp(WE_HANDLE hSecHandle, 
                St_SecWimElement *pstElement, St_SecCrptComputeSignatureResp *pstParam)
{
    WE_INT32   iResult = 0;
    
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    if(NULL == pstParam)
    {
            Sec_WimWtlsComputeSignatureResponse(hSecHandle, pstElement->iTargetID, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
    }
    else
    {
        iResult = pstParam->sResult;
        if (iResult != M_SEC_ERR_OK)
        {
            Sec_WimWtlsComputeSignatureResponse(hSecHandle, pstElement->iTargetID, 
                iResult, NULL, 0);
        } 
        else 
        {
            Sec_WimWtlsComputeSignatureResponse(hSecHandle, pstElement->iTargetID, M_SEC_ERR_OK, 
                pstParam->pucSig, pstParam->sSigLen);
        }
    }
    WE_FREE(pstElement);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimVerifySign
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Parse SignedTrustedCAInfo and get CA certificate, signature certificate, signature and so on.
        Get hash algorithm and signature public key from signature certificate.
        Verify whether the signature is right using hash algorithm, SignedTrustedCAInfo and signature public key .            
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucTrustedCAInfo[IN]:The structure containing the trusted CA Information.
        usTrustedCAInfoLen[IN]:The length of the structure.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Sec_HandleSignVerify.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimVerifySign(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                             const WE_UCHAR *pucTrustedCAInfo, WE_UINT16 usTrustedCAInfoLen) 
{
    WE_INT16        sCharSet = 0;
    WE_VOID            *pvDisplayName = NULL;
    WE_INT32        iDisplayNameLen = 0;
    WE_UCHAR        *pucTrustedCAcert = NULL;
    WE_UINT16       usTrustedCAcertLen = 0;
    WE_UCHAR        *pucSignCert = NULL;
    WE_UINT16       usSignCertLen = 0;
    WE_UINT8        ucSignAlg = 0;
    WE_UCHAR        *pucSign = NULL;
    WE_UINT16       usSignLen = 0;
    WE_UINT16       usTmpCertLen = 0;
    WE_INT32        iHashAlg = 0;
    St_SecCertificate  stCert = {0};
    St_SecCrptPubKey   stKey = {0};   
    WE_INT32        iResult = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimVerifySignatureResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    
    iResult = Sec_CertGetSignedTrustedCaInfo(hSecHandle, (const WE_UCHAR *)pucTrustedCAInfo,
        &sCharSet, &pvDisplayName, &iDisplayNameLen,
        &pucTrustedCAcert, &usTrustedCAcertLen, &pucSignCert,
        &usSignCertLen, &ucSignAlg, &pucSign,
        &usSignLen);
    if (iResult != M_SEC_ERR_OK)
    {         
        Sec_WimVerifySignatureResponse(hSecHandle, iTargetID, iResult);
        return; 
    }  
    
    /* wtls certificate */
    if ((*pucSignCert) == M_SEC_CERTIFICATE_WTLS_TYPE)
    {
        /* get public key */
        iResult = Sec_WtlsCertParse(hSecHandle, pucSignCert+1, &usTmpCertLen, &stCert);      
        stKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
        stKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
        stKey.usModLen = stCert.stCertUnion.stWtls.usModLen;
        stKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
        if (iResult == M_SEC_ERR_OK)
        {
            iResult = Sec_WimVerifyHashedSign(hSecHandle, (WE_INT32)E_SEC_ALG_HASH_SHA1, iTargetID, stKey,
                pucTrustedCAInfo, usTrustedCAInfoLen,
                pucSign, usSignLen);
        }
    }
    /* x509 certificate */
    else if ((*pucSignCert) == M_SEC_CERTIFICATE_X509_TYPE)
    {
        iResult = Sec_X509CertParse(hSecHandle, pucSignCert+1, &usTmpCertLen, &stCert); 
        if ((stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 5) || 
            (stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 4)
#ifdef M_SEC_CFG_MD2
            || (stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 2)
#endif      
            )
        { 
            /* get public key and hash algorithm */
            iResult = Sec_X509GetRsaModAndExp(hSecHandle, (const WE_UCHAR *)stCert.stCertUnion.stX509.pucPublicKeyVal,
                &(stKey.pucModulus), &(stKey.usModLen), 
                &(stKey.pucExponent), &(stKey.usExpLen));
            if (iResult == M_SEC_ERR_OK)
            {
                if (stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 5)
                {
                    iHashAlg = (WE_INT32)E_SEC_ALG_HASH_SHA1;   
                }
#ifdef M_SEC_CFG_MD2
                else if (stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 2)
                {
                    iHashAlg = (WE_INT32)E_SEC_ALG_HASH_MD2;
                }
#endif
                else
                {
                    iHashAlg = (WE_INT32)E_SEC_ALG_HASH_MD5;
                }
                iResult = Sec_WimVerifyHashedSign(hSecHandle, iHashAlg, iTargetID, stKey, 
                    pucTrustedCAInfo, usTrustedCAInfoLen-2-usSignLen,
                    pucSign, usSignLen);
            }
        }
    }
    else
    {
        Sec_WimVerifySignatureResponse(hSecHandle, iTargetID, M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE);
        return;
    }
    
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimVerifySignatureResponse(hSecHandle, iTargetID, iResult);
    }
    else
    {
        iResult = Sec_WimAddNewElement(hSecHandle, iTargetID, M_SEC_WIM_SIGNATURE_VERIFICATION,NULL);
        if (iResult != M_SEC_ERR_OK)
        {
            Sec_WimVerifySignatureResponse(hSecHandle, iTargetID, iResult);
        }
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimVerifySignResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        response of verify signature.            
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.   
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Sec_iWimVerifySignResp, SecWim_HandleVerifySignResponse.           
IMPORTANT NOTES:
        none.
=====================================================================================*/
WE_VOID Sec_iWimVerifySignResp (WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                                  St_SecCrptVerifySignatureResp *pstParam)
{
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    if(NULL==pstParam)
    {
         Sec_WimVerifySignatureResponse(hSecHandle, pstElement->iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);    
    }
    else
    {
         Sec_WimVerifySignatureResponse(hSecHandle, pstElement->iTargetID, pstParam->sResult);    
    }
    WE_FREE(pstElement);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimSignText
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Get private key and compute the hashed-RSA signature. Get hashed Public key or other 
        information accordding to iOptions.
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        iTargetID[IN]: ID of the object.
        iSignId[IN]: ID of the sign.
        pcData[IN]: Pointer to the data.
        iDataLen[IN]:The length of the data.
        iCertId[IN]: Id of user certificate.
        iOptions[IN]: The value of the options.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimSignText(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSignId, 
                      const WE_CHAR *pcData, WE_INT32 iDataLen, WE_INT32 iCertId, 
                      WE_INT32 iOptions)
{
    WE_UCHAR          *pucPrivKey = NULL;
    WE_INT32          iPrivKeyLen = 0;
    WE_INT32          iKeyLen = 0;
    WE_INT32          iFoundKey = 0;
    WE_UINT8          ucNumber = 0;
    WE_UINT8          ucAlgorithm = M_SEC_SIGNTEXT_RSAALG;
    WE_CHAR           *pcHashedKey = NULL;
    WE_INT32          iHashedKeyLen = 0;
    WE_UCHAR          *pucCert = NULL;
    WE_UINT16         usCertLen = 0;
    WE_INT32          iCertType = 0;
    WE_INT32          iErr = M_SEC_SIGNTEXT_OK;
    WE_INT32          iResult = 0;
    WE_INT32          iRes = 0;
    WE_INT32          iIndexJ = 0;
    St_SecWimSigntext      *pstP = NULL;
    St_SecCrptPrivKey      stCrptPrivKey = {0};
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
            NULL, 0, 0, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    } 
    /* check if the private key exists.*/
    iResult = Sec_WimCheckFileExist(hSecHandle, (WE_CHAR *)M_SEC_NON_REP_PRIVKEY_NAME);
    if(iResult != M_SEC_ERR_OK)     
    {
        Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
            NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
        return;
    }                      
    
    /* find matched private key file and read it. */
    while ((!iFoundKey) && (iIndexJ < M_SEC_USER_CERT_MAX_SIZE))
    { 
        if((iCertId == SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId) && 
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == M_SEC_PRIVATE_NONREPKEY))/* used for signature.*/
        {
            iResult = Sec_WimGetPrivateKey(hSecHandle, M_SEC_PRIVATE_NONREPKEY, &pucPrivKey, &iKeyLen);
            if (iResult != M_SEC_ERR_OK)
            {
                Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL,
                    0, NULL, 0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                return;
            }    
            iPrivKeyLen = Sec_WimGetOrigiLen(pucPrivKey, iKeyLen);  
            iFoundKey = 1;
            ucNumber = (WE_UINT8)iIndexJ;
        }
        iIndexJ++;
    }
    
    /* find the matched private key. */
    if (iFoundKey == 1)
    {
        if (iOptions == 0x00) /* no requirement.*/
        {
            stCrptPrivKey.pucBuf = pucPrivKey;
            stCrptPrivKey.usBufLen = (WE_UINT16)iPrivKeyLen;
            /* compute the hashed-RSA signature */
            iRes = Sec_PKCRsaComputeHashedSignature(hSecHandle, iSignId, stCrptPrivKey, 
                (const WE_UCHAR *)pcData, iDataLen);
            if (iRes != M_SEC_ERR_OK)
            {
                WE_FREE(pucPrivKey);
                Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL,
                    0, NULL, 0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                return;
            }
            WE_FREE(pucPrivKey);      
            pstP = (St_SecWimSigntext *)WE_MALLOC(sizeof(St_SecWimSigntext));
            if (NULL == pstP)
            {
                Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
                    NULL, 0, 0, M_SEC_ERR_INSUFFICIENT_MEMORY);
                return;
            }
            pstP->iTargetID = iTargetID; 
            pstP->iSignId = iSignId;
            pstP->ucAlgorithm = ucAlgorithm;
            pstP->pcHashedKey = NULL;
            pstP->iHashedKeyLen = 0;
            pstP->pucCert = NULL;
            pstP->iCertLen = 0;
            pstP->iCertType = 0;
            pstP->iErr = iErr;
            iRes = Sec_WimAddNewElement (hSecHandle, iSignId, M_SEC_WIM_SIGNTEXT,pstP);
            if (iRes != M_SEC_ERR_OK)
            {
                WE_FREE(pstP);
                Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
                    NULL, 0, 0, M_SEC_ERR_INSUFFICIENT_MEMORY);
            }
            return;
        }    
        if ((iOptions & M_SEC_SIGNTEXT_RETURNHASHEDKEY) == M_SEC_SIGNTEXT_RETURNHASHEDKEY)
        {
            pcHashedKey = (WE_CHAR *)WE_MALLOC((M_SEC_KEY_HASH_SIZE) * sizeof(WE_CHAR));
            if (NULL == pcHashedKey)
            {
                WE_FREE(pucPrivKey);
                Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, 
                    NULL, 0, NULL, 0, 0, M_SEC_ERR_INSUFFICIENT_MEMORY);
                return;
            }
            /* get hashed key.*/
            for (iIndexJ=0; iIndexJ<M_SEC_KEY_HASH_SIZE; iIndexJ++)
            {
                pcHashedKey[iIndexJ] = (WE_CHAR)(SEC_USER_CERT_KEYPAIRS[ucNumber]\
                    .aucPublicKeyHash[iIndexJ]);
            }
            iHashedKeyLen = M_SEC_KEY_HASH_SIZE;
        } 
        if ((iOptions & M_SEC_SIGNTEXT_RETURNCERT) == M_SEC_SIGNTEXT_RETURNCERT)
        {
            if (SEC_USER_CERT_KEYPAIRS[ucNumber].ucPublicKeyCert == 1) /* public key certificate. */
            {
                pucCert = NULL;
                usCertLen = 0;
                iErr = M_SEC_SIGNTEXT_MISSCERT;
            }
            else /* user certificate. */
            {
                WE_UCHAR ucCertFormat = 0;
                /* get the certificate. */
                iRes = Sec_WimGetCertByID(hSecHandle, iCertId, &pucCert, &usCertLen);
                if (iRes != M_SEC_ERR_OK)
                {      
                    WE_FREE(pucPrivKey);  
                    if (pcHashedKey != NULL)
                    {
                        WE_FREE(pcHashedKey);
                    }
                    Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
                        NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                    return;
                }
                /* get the format of certificate.*/
                iResult = Sec_WimGetCertFormatByID(hSecHandle, iCertId, &ucCertFormat);
                if (iResult != M_SEC_ERR_OK)
                {     
                    WE_FREE(pucPrivKey);  
                    WE_FREE(pucCert); 
                    if (pcHashedKey != NULL)
                    {
                        WE_FREE(pcHashedKey);
                    }
                    Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
                        NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
                    return;
                }
                if (ucCertFormat == M_SEC_CERTIFICATE_WTLS_TYPE)
                {
                    iCertType = M_SEC_SIGNTEXT_WTLSCERT;
                }
                if (ucCertFormat == M_SEC_CERTIFICATE_X509_TYPE)
                {
                    iCertType = M_SEC_SIGNTEXT_X509CERT;
                }
            } 
        } 
        /* other option */
        if ((iOptions < 0) || (iOptions > 7))
        {
            WE_FREE(pucPrivKey); 
            if (pcHashedKey != NULL)
            {
                WE_FREE(pcHashedKey);
            }
            if (pucCert != NULL)
            {
                WE_FREE(pucCert);
            }
            Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
                NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
            return;
        }   
        
        stCrptPrivKey.pucBuf = pucPrivKey;
        stCrptPrivKey.usBufLen = (WE_UINT16)iPrivKeyLen;
        /* compute the hashed-RSA signature */
        iRes = Sec_PKCRsaComputeHashedSignature(hSecHandle, iSignId, stCrptPrivKey,
            (const WE_UCHAR *) pcData, iDataLen);
        WE_FREE(pucPrivKey);
        if (iRes != M_SEC_ERR_OK)
        {
            if (pucCert != NULL)
            {
                WE_FREE(pucCert);
            }
            if (pcHashedKey != NULL)
            {
                WE_FREE(pcHashedKey);
            }
            Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 
                0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
            return;
        }        
        
        pstP = (St_SecWimSigntext *)WE_MALLOC(sizeof(St_SecWimSigntext));
        if (NULL == pstP)
        {
            if (pucCert != NULL)
            {
                WE_FREE(pucCert);
            }
            if (pcHashedKey != NULL)
            {
                WE_FREE(pcHashedKey);
            }
            Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL,
                0, NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
            return;
        }
        pstP->iTargetID = iTargetID; 
        pstP->iSignId = iSignId;
        pstP->ucAlgorithm = ucAlgorithm;
        pstP->pcHashedKey = pcHashedKey;
        pstP->iHashedKeyLen = iHashedKeyLen;
        pstP->pucCert = pucCert;
        pstP->iCertLen = (WE_INT32)usCertLen;
        pstP->iCertType = iCertType;
        pstP->iErr = iErr;
        iRes = Sec_WimAddNewElement (hSecHandle, iSignId, M_SEC_WIM_SIGNTEXT,pstP);
        if (iRes != M_SEC_ERR_OK)
        {
            if (pucCert != NULL)
            {
                WE_FREE(pucCert);
            }
            if (pcHashedKey != NULL)
            {
                WE_FREE(pcHashedKey);
            }
            WE_FREE(pstP);
            Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
                NULL, 0, 0, M_SEC_ERR_INSUFFICIENT_MEMORY);
        }
        return;
    } 
    else /* not find private key. */
    {
        Sec_WimSignTextResponse(hSecHandle, iTargetID, iSignId, 0, NULL, 0, NULL, 0, 
            NULL, 0, 0, M_SEC_SIGNTEXT_MISSCERT);
        return;
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimSignTextResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit. 
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.  
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimSignTextResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                          St_SecCrptComputeSignatureResp *pstParam)
{
    St_SecWimSigntext  *pstP = NULL;
    
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    /*modified by Bird 070118*/
    pstP = pstElement->pvPointer;

    if (NULL==pstParam)
    {
        Sec_WimSignTextResponse(hSecHandle, pstP->iTargetID, pstP->iSignId, 0, NULL, 0, NULL, 0,
            NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
    }
    else
    {
        if (pstParam->sResult != M_SEC_ERR_OK) 
        {
            Sec_WimSignTextResponse(hSecHandle, pstP->iTargetID, pstP->iSignId, 0, NULL, 0, NULL, 0,
                NULL, 0, 0, M_SEC_SIGNTEXT_GENERALERROR);
        } 
        else 
        {
            Sec_WimSignTextResponse(hSecHandle, pstP->iTargetID, pstP->iSignId, pstP->ucAlgorithm, 
                (const WE_CHAR *)pstParam->pucSig, pstParam->sSigLen, 
                (const WE_CHAR *)pstP->pcHashedKey, pstP->iHashedKeyLen, 
                (const WE_CHAR *)pstP->pucCert, pstP->iCertLen, 
                pstP->iCertType, pstP->iErr);
        }  
    }
    WE_FREE (pstP->pucCert);
    WE_FREE (pstP->pcHashedKey);
    WE_FREE (pstP);
    WE_FREE (pstElement);
    return;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimViewPrivKeyPin
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Verify if the pin exists.
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        iTargetID[IN]:ID of the object.
        ucKeyType[IN]:The type of the key.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Pin is stored in the private_key file.
=====================================================================================*/
WE_VOID Sec_iWimViewPrivKeyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
    WE_INT32   iResult = 0;  
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        iResult = M_SEC_ERR_WIM_NOT_INITIALISED;
    }
    else if ((ucKeyType != M_SEC_PRIVATE_AUTHKEY) && (ucKeyType != M_SEC_PRIVATE_NONREPKEY))
    {
        iResult = M_SEC_ERR_INVALID_PARAMETER;
    }
    else
    {
        iResult = Sec_WimReadPin(hSecHandle, ucKeyType, NULL);  
    }
    
    Sec_WimViewPrivKeyPinResponse(hSecHandle, iTargetID, iResult);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimVerifyPrivKeyPin
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Get pin from private key file, and judge if the input pin is right.
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        iTargetID[IN]:ID of the object.
        ucKeyType[IN]:The type of the key.   
        pcPin[IN]: The pin.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Pin is stored in the private_key file.
=====================================================================================*/
WE_VOID Sec_iWimVerifyPrivKeyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                    WE_UINT8 ucKeyType, const WE_CHAR *pcPin)
{
    WE_INT32   iResult = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimVerifyPrivKeyPinResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    } 
    
    iResult = Sec_WimCheckPin(hSecHandle, ucKeyType, pcPin);    
    Sec_WimVerifyPrivKeyPinResponse(hSecHandle, iTargetID, iResult);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimModifyPrivKeyPin
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Verify OldPin. And write the newPin in it.
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        iTargetID[IN]:The value of the wid.
        ucKeyType[IN]:The type of the key.   
        pcOldPin[IN]: The old pin.
        pcNewPin[IN]: The new pin.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Pin is stored in the private_key file.
=====================================================================================*/
WE_VOID Sec_iWimModifyPrivKeyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                    WE_UINT8 ucKeyType, const WE_CHAR *pcOldPin, const WE_CHAR *pcNewPin)
{
    WE_INT32  iResult = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimModifyPrivKeyPinResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }   
    
    iResult = Sec_WimCheckPin(hSecHandle, ucKeyType, pcOldPin);
    if(iResult != M_SEC_ERR_OK)
    {    
        Sec_WimModifyPrivKeyPinResponse(hSecHandle, iTargetID, iResult);
        return;
    }
    iResult = Sec_WimStorePin(hSecHandle, ucKeyType, (const WE_INT8 *)pcNewPin);
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimModifyPrivKeyPinResponse(hSecHandle, iTargetID, iResult);
        return;
    }
    Sec_WimModifyPrivKeyPinResponse(hSecHandle, iTargetID, M_SEC_ERR_OK);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimGetFitedPrivKey
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Copy out public key and start search matched private key.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        stWimPubKey[IN]: RSA public key.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.            
=====================================================================================*/
WE_VOID Sec_iWimGetFitedPrivKey(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                               St_SecPubKeyRsa stWimPubKey)
{
    St_SecWimElement            *pstElement = NULL;
    St_SecWimGetMatchedPrivKey  *pstP = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID,
            M_SEC_ERR_WIM_NOT_INITIALISED, 0, 0, 0);
        return; 
    }
    
    pstP = (St_SecWimGetMatchedPrivKey *)WE_MALLOC(sizeof(St_SecWimGetMatchedPrivKey));
    if (NULL == pstP)
    {
        Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID,
            M_SEC_ERR_INSUFFICIENT_MEMORY, 0, 0, 0);
        return;
    } 
    pstP->iIndexI= 0;
    /* copy RSA public key */
    pstP->stPubKey.pucExponent = (WE_UCHAR *)WE_MALLOC(stWimPubKey.usExpLen);
    if ((NULL == pstP->stPubKey.pucExponent) && (stWimPubKey.usExpLen != 0))
    {
        Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID,
            M_SEC_ERR_INSUFFICIENT_MEMORY, 0, 0, 0);
        WE_FREE (pstP);
        return;
    }
    (WE_VOID)WE_MEMCPY(pstP->stPubKey.pucExponent, stWimPubKey.pucExponent, 
        stWimPubKey.usExpLen);
    pstP->stPubKey.usExpLen = stWimPubKey.usExpLen;
    pstP->stPubKey.pucModulus = (WE_UCHAR *)WE_MALLOC(stWimPubKey.usModLen);
    if ((NULL == pstP->stPubKey.pucModulus) && (stWimPubKey.usModLen != 0))
    {
        Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID,
            M_SEC_ERR_INSUFFICIENT_MEMORY, 0, 0, 0);
        WE_FREE (pstP->stPubKey.pucExponent);
        WE_FREE (pstP);
        return;
    }
    (WE_VOID)WE_MEMCPY(pstP->stPubKey.pucModulus, stWimPubKey.pucModulus, 
        stWimPubKey.usModLen);
    pstP->stPubKey.usModLen = stWimPubKey.usModLen;
    
    pstElement = (St_SecWimElement *)WE_MALLOC(sizeof(St_SecWimElement));
    if (NULL == pstElement)
    {
        Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, 
            M_SEC_ERR_INSUFFICIENT_MEMORY, 0, 0, 0);
        WE_FREE (pstP->stPubKey.pucModulus);
        WE_FREE (pstP->stPubKey.pucExponent);
        WE_FREE (pstP);
        return;
    }
    pstElement->iTargetID = (WE_INT16) iTargetID;
    pstElement->pvPointer = pstP;
    pstElement->iState = M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_START;  
    Sec_iWimGetFitedPrivKeyResp (hSecHandle, pstElement, NULL);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimGetFitedPrivKeyResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Find the private key which matches public key, and verify it.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN]: Global queue.
        pstParam[IN]: The parameter value.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimGetFitedPrivKeyResp(WE_HANDLE hSecHandle, 
                                   St_SecWimElement *pstElement, WE_VOID *pvParam)
{
    St_SecWimGetMatchedPrivKey *pstP = NULL; 
    WE_INT32                 iTargetID = 0;
    WE_UINT32                uiKeyId = 0;
    WE_INT32                 iKeyCount = 0;
    WE_UINT8                 ucKeyUsage = 0;
    WE_UCHAR                 *pucTestData = (WE_UCHAR *)"just for test";
    WE_INT32                 iTestDataLen = 13;
    WE_UCHAR                 aucHash[20] = {0};
    WE_INT32                 iHashLen = 20;
    E_SecRsaSigType          eSigType = E_SEC_RSA_PKCS1_SHA1;   
    WE_INT32                 iIndexI = 0;  
    WE_INT32                 iIndexJ = 0;  
    WE_INT32                 iResult = 0;
    WE_UINT8                 aucUsageOfKey[2] = {M_SEC_PRIVATE_AUTHKEY, M_SEC_PRIVATE_NONREPKEY};
    
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }    
    pstP = pstElement->pvPointer;
    iTargetID = pstElement->iTargetID;
    iIndexI = pstP->iIndexI;  

    while (iIndexI < 2) 
    {
        switch (pstElement->iState)
        {
        case M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_START:
            {
                WE_UCHAR  *pucPrivKey = NULL;
                WE_INT32  iPrivKeyLen = 0;
                WE_INT32  iKeyLen = 0;
                St_SecCrptPrivKey  stPrivKey = {0};
                ucKeyUsage = aucUsageOfKey[iIndexI];
                iResult = Sec_WimGetPrivateKey(hSecHandle, ucKeyUsage, &pucPrivKey, &iPrivKeyLen);
                if (iResult != M_SEC_ERR_OK) 
                {
                    break; /* If not find private key, jump to find next private key. */
                }  
                iKeyLen = Sec_WimGetOrigiLen(pucPrivKey, iPrivKeyLen);                
                stPrivKey.pucBuf = pucPrivKey;
                stPrivKey.usBufLen = (WE_UINT16)iKeyLen;
                /* compute RSA signature. */
                iResult = Sec_PKCRsaComputeSignature(hSecHandle, pstElement->iTargetID, stPrivKey, eSigType,
                    pucTestData, iTestDataLen);
                if (iResult != M_SEC_ERR_OK)
                {
                    Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, iResult, 0, 0, 0);
                    WE_FREE(pucPrivKey);
                    Sec_WimFreePubKey(pstP->stPubKey);
                    WE_FREE(pstP);
                    WE_FREE(pstElement);
                    return;
                } 
                WE_FREE(pucPrivKey);
                pstElement->iState = M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_COMPUTE;
                pstElement->hItype = hSecHandle;
                Sec_WimAddPreassignElement(hSecHandle, pstElement);
                return;
            }
        case M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_COMPUTE:
            {  
                St_SecCrptComputeSignatureResp *pstSigResp = NULL;
                if (NULL == pvParam)
                {
                    Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, 
                        M_SEC_ERR_INVALID_PARAMETER, 0, 0, 0);
                    Sec_WimFreePubKey(pstP->stPubKey);
                    WE_FREE(pstP);
                    WE_FREE(pstElement);
                    return;
                }
                pstSigResp = pvParam;
                iResult = pstSigResp->sResult;
                if (iResult != M_SEC_ERR_OK)
                {
                    Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, 
                        iResult, 0, 0, 0);
                    Sec_WimFreePubKey(pstP->stPubKey);
                    WE_FREE (pstP);
                    WE_FREE (pstElement);
                    return;
                }
                /* verify RSA signature. */
                iResult = Sec_LibHash(hSecHandle, E_SEC_ALG_HASH_SHA1, pucTestData, \
                    iTestDataLen, aucHash, &iHashLen);
                if(M_SEC_ERR_OK != iResult)
                {
                    Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, 
                        iResult, 0, 0, 0);
                    Sec_WimFreePubKey(pstP->stPubKey);
                    WE_FREE (pstP);
                    WE_FREE (pstElement);
                    return;
                }
                iResult = Sec_PKCRsaVerifySignature(hSecHandle, iTargetID, 
                    (pstP->stPubKey), aucHash, iHashLen, E_SEC_RSA_PKCS1_NULL,
                    pstSigResp->pucSig, pstSigResp->sSigLen); 
                if (iResult != M_SEC_ERR_OK) 
                {
                    break;
                } 
                else 
                {
                    pstElement->iState = M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_VERIFY;
                    pstElement->hItype = hSecHandle;
                    Sec_WimAddPreassignElement(hSecHandle, pstElement);
                    return;
                }
            }
        case M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_VERIFY:
            {  
                St_SecCrptVerifySignatureResp *pstVsigResp = NULL;
                if (NULL == pvParam)
                {
                    Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, 
                        M_SEC_ERR_INVALID_PARAMETER, 0, 0, 0);
                    Sec_WimFreePubKey(pstP->stPubKey);
                    WE_FREE (pstP);
                    WE_FREE (pstElement);
                    return;
                }
                pstVsigResp = pvParam;
                iResult = pstVsigResp->sResult;
                if (iResult == M_SEC_ERR_OK)  
                {  
                    ucKeyUsage = aucUsageOfKey[iIndexI];
                    uiKeyId = 0; 
                    for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
                    {
                        if (ucKeyUsage == SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage)
                        {                    
                            iKeyCount = SEC_USER_CERT_KEYPAIRS[iIndexJ].iPkeyCount;
                            break;
                        }
                    }
                    Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, M_SEC_ERR_OK,
                        uiKeyId, iKeyCount, ucKeyUsage);
                    Sec_WimFreePubKey(pstP->stPubKey);
                    WE_FREE (pstP);
                    WE_FREE (pstElement);
                    return;
                }
                break;
            }
        default :
            {
                Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, 
                                    M_SEC_ERR_INVALID_PARAMETER, 0, 0, 0);
                Sec_WimFreePubKey(pstP->stPubKey);
                WE_FREE (pstP);
                WE_FREE (pstElement);
                return;
            }
        }
        pstElement->iState = M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_START;
        pstP->iIndexI = ++iIndexI; 
    }
    Sec_WimGetMatchedPrivKeyResponse(hSecHandle, iTargetID, M_SEC_ERR_PRIV_KEY_NOT_FOUND, 
                                uiKeyId, iKeyCount, ucKeyUsage);
    Sec_WimFreePubKey(pstP->stPubKey);
    WE_FREE (pstP);
    WE_FREE (pstElement);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimFindMatchedSignText
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Find the matched user certificate and get out its name. 
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]:Global data handle.
        iTargetID[IN]: The value of the wid.
        iSignId[IN]: ID of the sign.
        iPubKeyType[IN]: Type of the key id.
        pcPubKeyHash[IN]: Pointer to the hashed key.
        iPubKeyHashLen[IN]: Length of hashed key.
        iOptions[IN]: The value of the options.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimFindMatchedSignText(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                 WE_INT32 iSignId, WE_INT32 iPubKeyType, const WE_CHAR *pcPubKeyHash,
                                 WE_INT32 iPubKeyHashLen, WE_INT32 iOptions)
{
    WE_UCHAR            **ppucCertNames = NULL;
    WE_UINT16           *pusNameLen = NULL; 
    WE_INT16            *psCharSet = NULL;
    WE_INT8             *pcNameType = NULL;
    WE_INT32            *piCertIds = NULL;
    WE_INT32            iIndexJ = 0;
    WE_INT32            iIndexK = 0;
    WE_INT32            iRes = 0;    
    WE_INT32            iNumOfCerts = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimFindMatchedSignTextResponse(hSecHandle, iTargetID, iSignId, 
            M_SEC_ERR_WIM_NOT_INITIALISED, 
            0, NULL, NULL, NULL, NULL, NULL);
        return;
    }
    
    ppucCertNames = (WE_UCHAR **)WE_MALLOC(M_SEC_USER_CERT_MAX_SIZE * sizeof(WE_UCHAR *));
    pusNameLen = (WE_UINT16 *)WE_MALLOC(M_SEC_USER_CERT_MAX_SIZE * sizeof(WE_UINT16));
    psCharSet = (WE_INT16 *)WE_MALLOC(M_SEC_USER_CERT_MAX_SIZE * sizeof(WE_INT16));
    pcNameType = (WE_INT8 *)WE_MALLOC(M_SEC_USER_CERT_MAX_SIZE * sizeof(WE_INT8));
    piCertIds = (WE_INT32 *)WE_MALLOC(M_SEC_USER_CERT_MAX_SIZE * sizeof(WE_INT32));
    if ((piCertIds == NULL) || (psCharSet == NULL) || (pusNameLen == NULL) ||
        (pcNameType == NULL) || (ppucCertNames == NULL))
    {
        Sec_WimFindMatchedSignTextResponse(hSecHandle, iTargetID, iSignId, 
            M_SEC_ERR_INSUFFICIENT_MEMORY, 
            0, NULL, NULL, NULL, NULL, NULL);
        if (psCharSet != NULL)
        {
            WE_FREE(psCharSet);
        }
        if (pusNameLen != NULL)
        {
            WE_FREE(pusNameLen);
        }
        if (pcNameType != NULL)
        {
            WE_FREE(pcNameType);
        }
        if (piCertIds != NULL)
        {
            WE_FREE(piCertIds);
        }
        if (ppucCertNames != NULL)
        {
            WE_FREE(ppucCertNames);
        }
        return;
    }
    
    /* M_SEC_SIGN_NO_KEY: don't need check keyexchangeIds.*/
    if (iPubKeyType == M_SEC_SIGN_NO_KEY) 
    {
        for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
        { 
            if((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) && 
                (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == M_SEC_PRIVATE_NONREPKEY) &&
                (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucPublicKeyCert == 0))
            {
                piCertIds[iNumOfCerts] = SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId;
                iRes =  Sec_WimGetCertNameByID(hSecHandle, piCertIds[iNumOfCerts],
                    &(ppucCertNames[iNumOfCerts]), &(pusNameLen[iNumOfCerts]), 
                    &(psCharSet[iNumOfCerts]), &(pcNameType[iNumOfCerts]));        
                if (iRes != M_SEC_ERR_OK)
                { 
                    WE_FREE(psCharSet);
                    WE_FREE(pusNameLen);
                    WE_FREE(pcNameType);        
                    WE_FREE(piCertIds);
                    for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                    {
                        WE_FREE(ppucCertNames[iIndexK]);
                    }
                    WE_FREE(ppucCertNames);    
                    Sec_WimFindMatchedSignTextResponse(hSecHandle, iTargetID, iSignId, 
                        M_SEC_SIGNTEXT_GENERALERROR, 
                        0, NULL, NULL, NULL, NULL, NULL);
                    return;
                }
                iNumOfCerts++;  
            }
        } 
        
        Sec_WimFindMatchedSignTextResponse(hSecHandle, iTargetID, iSignId, M_SEC_ERR_OK, 
            iNumOfCerts, (const WE_VOID *const *)ppucCertNames, 
            pusNameLen, psCharSet, pcNameType, piCertIds);         
    }  
    /* need check KeyId.*/
    else if ((iPubKeyType == M_SEC_SIGN_SHA_KEY) || (iPubKeyType == M_SEC_SIGN_SHA_CA_KEY))
    {
        WE_INT32  iNotMatch = 0;
        /*search in user certificates */
        for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
        { 
            if((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) && 
                (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == M_SEC_PRIVATE_NONREPKEY) &&
                (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucPublicKeyCert == 0))
            {
                if ((iPubKeyHashLen == M_SEC_KEY_HASH_SIZE) && (pcPubKeyHash != NULL))
                { 
                    iNotMatch = 0;
                    if (iPubKeyType == M_SEC_SIGN_SHA_KEY)
                    {
                        for (iIndexK=0; iIndexK<iPubKeyHashLen; iIndexK++)
                        {
                            if ((WE_UCHAR)(pcPubKeyHash[iIndexK]) != \
                                SEC_USER_CERT_KEYPAIRS[iIndexJ].aucPublicKeyHash[iIndexK])
                            {
                                iNotMatch = 1;
                                break;
                            }
                        }
                    }                  
                    if (iPubKeyType == M_SEC_SIGN_SHA_CA_KEY)
                    {
                        for (iIndexK=0; iIndexK<iPubKeyHashLen; iIndexK++)
                        {
                            if ((WE_UCHAR)(pcPubKeyHash[iIndexK]) != \
                                SEC_USER_CERT_KEYPAIRS[iIndexJ].aucCApublicKeyHash[iIndexK])
                            {
                                iNotMatch = 1;
                                break;
                            }
                        }
                    }
                    
                    /* if find the matched user certificate. */
                    if (iNotMatch == 0)   
                    {                        
                        piCertIds[iNumOfCerts] = SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId;
                        iRes =  Sec_WimGetCertNameByID(hSecHandle, piCertIds[iNumOfCerts],
                            &(ppucCertNames[iNumOfCerts]), &(pusNameLen[iNumOfCerts]), 
                            &(psCharSet[iNumOfCerts]), &(pcNameType[iNumOfCerts]));        
                        if (iRes != M_SEC_ERR_OK)
                        { 
                            WE_FREE(psCharSet);
                            WE_FREE(pusNameLen);
                            WE_FREE(pcNameType);        
                            WE_FREE(piCertIds);
                            for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
                            {
                                WE_FREE(ppucCertNames[iIndexK]);
                            }
                            WE_FREE(ppucCertNames);    
                            Sec_WimFindMatchedSignTextResponse(hSecHandle, iTargetID, iSignId, 
                                M_SEC_SIGNTEXT_GENERALERROR, 
                                0, NULL, NULL, NULL, NULL, NULL);
                            return;
                        }
                        iNumOfCerts++;        
                    }
                }  
            }
        } 
        Sec_WimFindMatchedSignTextResponse(hSecHandle, iTargetID, iSignId, M_SEC_ERR_OK, 
            iNumOfCerts, (const WE_VOID *const *)ppucCertNames,
            pusNameLen, psCharSet, pcNameType, piCertIds);   
    }    
    else 
    {
        Sec_WimFindMatchedSignTextResponse(hSecHandle, iTargetID, iSignId, 
            M_SEC_SIGNTEXT_GENERALERROR, 0, NULL, NULL, NULL, NULL, NULL);
    }
    for (iIndexK=0; iIndexK<iNumOfCerts; iIndexK++)
    {
        WE_FREE(ppucCertNames[iIndexK]);
    }
    WE_FREE(ppucCertNames);
    WE_FREE(psCharSet);
    WE_FREE(pusNameLen);
    WE_FREE(pcNameType);
    WE_FREE(piCertIds);
    iOptions = iOptions;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsKeyExchange
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Generate master secret by calling Sec_WimWtlsKeyExRsa.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        stParam[IN] : The parameter value.
        pucRandVal[IN] : Pointer to the rand value.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimWtlsKeyExchange(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                             St_SecWtlsKeyExchParams stParam, const WE_UCHAR *pucRandVal,WE_UINT8 ucHashAlg)
{ 
    St_SecCertificate stCert = {0};
    St_SecPubKeyRsa   stPubKey = {0};
    WE_UCHAR          *pucCert = NULL;
    WE_UINT16         usCertLen = 0;
    WE_INT32          iRes = 0;  
    
    if (NULL == hSecHandle)
    {
        return;
    }    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED,
                                        0, NULL, 0);
        return;
    }
    
    switch (stParam.ucKeyExchangeSuite)
    {
        case E_SEC_WTLS_KEYEXCHANGE_NULL          : 
            Sec_WimWtlsKeyExNULL(hSecHandle, iTargetID, pucRandVal,ucHashAlg);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON      : 
        case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512  : 
        case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768  : 
            Sec_WimWtlsKeyExRsa(hSecHandle, iTargetID,
                    stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey, pucRandVal,ucHashAlg);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA           : 
        case E_SEC_WTLS_KEYEXCHANGE_RSA_512       : 
        case E_SEC_WTLS_KEYEXCHANGE_RSA_768       : 
            pucCert = stParam.stParams.stCertificates.pucBuf; 
            if (NULL == pucCert)/* add by Sam [070126] */
            {
                Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, 0, NULL, 0);
                break;
            }
            if ((*pucCert) == M_SEC_CERTIFICATE_WTLS_TYPE)       
            {            
                iRes = Sec_WtlsCertParse(hSecHandle, pucCert+1, &usCertLen, &stCert);    
                if (iRes != M_SEC_ERR_OK)
                {
                    Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, iRes, 0, NULL, 0);
                    break;
                }
                stPubKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
                stPubKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
                stPubKey.usModLen = stCert.stCertUnion.stWtls.usModLen;
                stPubKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
            }
            else if ((*pucCert) == M_SEC_CERTIFICATE_X509_TYPE)     
            {
                iRes = Sec_WimGetX509PubKey(hSecHandle, pucCert + 1, &(stPubKey.usExpLen),
                    &(stPubKey.pucExponent), &(stPubKey.usModLen), &(stPubKey.pucModulus));
                if (iRes != M_SEC_ERR_OK)
                {
                    Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, iRes, 0, NULL, 0);
                    break;
                }
            }
            else      
            { 
                Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID,
                                M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, 0, NULL, 0);
                break;
            }    
            
            Sec_WimWtlsKeyExRsa(hSecHandle, iTargetID, stPubKey, pucRandVal,ucHashAlg);  
            break;
        case E_SEC_WTLS_KEYEXCHANGE_SHARED_SECRET : 
        case E_SEC_WTLS_KEYEXCHANGE_DH_ANON       : 
        case E_SEC_WTLS_KEYEXCHANGE_DH_ANON_512   : 
        case E_SEC_WTLS_KEYEXCHANGE_DH_ANON_768   : 
        case E_SEC_WTLS_KEYEXCHANGE_ECDH_ANON     : 
        case E_SEC_WTLS_KEYEXCHANGE_ECDH_ANON_113 : 
        case E_SEC_WTLS_KEYEXCHANGE_ECDH_ANON_131 : 
        case E_SEC_WTLS_KEYEXCHANGE_ECDH_ECDSA    :
            Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_UNSUPPORTED_METHOD, 0, NULL, 0); 
            break;
        default   : 
            Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, 0, NULL, 0);
            break;
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_WimWtlsKeyExNULL
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Get master secret and store it when KeyExchangeSuite is NULL.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucRandVal[IN]: Pointer to the rand value.
        ucHashAlg[IN]: algorithm of hash.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.        
=====================================================================================*/
static WE_VOID  Sec_WimWtlsKeyExNULL(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                                const WE_UCHAR *pucRandVal, WE_UINT8 ucHashAlg)
{
    WE_UCHAR            *pucMasterSecret = NULL;
    WE_INT32            iMasterSecretId = 0;
    WE_INT32            iRes = 0;
    WE_INT32            iIndexI = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    pucMasterSecret = (WE_UCHAR *)WE_MALLOC(20 * sizeof(WE_UCHAR));  
    if (NULL == pucMasterSecret)
    {
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0, NULL, 0);
        return;
    }
    /* get the master secret. */
    iRes = Sec_WimGetWTLSMasterSecret(hSecHandle, 0, NULL,  0,
        MASTER_SECRET, pucRandVal, 32, 20, pucMasterSecret,ucHashAlg);
    if (iRes != M_SEC_ERR_OK)
    {
        for (iIndexI=0; iIndexI<20; iIndexI++)
        {
            pucMasterSecret[iIndexI] = 0;
        }
        WE_FREE(pucMasterSecret);
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, iRes, 0, NULL, 0);
        return;
    }  
    /* store master secret in session cache . iMasterSecretId equals session number.*/
    Sec_WimSaveMastSecret(hSecHandle, pucMasterSecret, M_SEC_WTLS_MASTER_SECRET_LEN, 
        &iMasterSecretId, &iRes);
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, iRes, 0, NULL, 0); 
    }  
    else
    {
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, iMasterSecretId, NULL, 0);
    }
    
    /*clear master secret and free it. */
    for (iIndexI=0; iIndexI<20; iIndexI++)
    {
        pucMasterSecret[iIndexI] = 0;
    }
    WE_FREE(pucMasterSecret);
}
/*=====================================================================================
FUNCTION: 
        Sec_WimWtlsKeyExRsa
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Get master secret.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        stPubKey[IN]: public key.
        pucRandVal[IN]: Pointer to the rand value.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.        
=====================================================================================*/
static WE_VOID  Sec_WimWtlsKeyExRsa (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                  St_SecPubKeyRsa stPubKey, const WE_UCHAR *pucRandVal,WE_UINT8 ucAlg)
{
    WE_UCHAR            *pucPreMasterSecret = NULL;
    WE_INT32            iPreMasterSecretLen = 0;
    WE_UCHAR            *pucMasterSecret = NULL;
    WE_UCHAR            *pucTemp = NULL;
    WE_UINT16           usExpLen = stPubKey.usExpLen;
    WE_UINT16           usModLen = stPubKey.usModLen;
    St_SecPubKeyRsa     stKey = {0};
    St_SecWimWtlsKeyExRsa  *pstP = NULL;
    WE_INT32            iRes = 0;
    WE_INT32            iIndexI = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    iPreMasterSecretLen = 20 + usExpLen + usModLen + 4;
    pucPreMasterSecret = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iPreMasterSecretLen * sizeof(WE_UCHAR));
    if (NULL == pucPreMasterSecret)
    {
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0, NULL, 0);
        return;
    }
    
    iRes = Sec_LibGenerateRandom(hSecHandle, (WE_CHAR *)pucPreMasterSecret, 20);
    pucPreMasterSecret[0] = 0x01;
    /* store public key in pre-master secret. */
    pucTemp = pucPreMasterSecret + 20;
    Sec_ExportStrUint16to8(&usExpLen, pucTemp);
    pucTemp += 2;
    for (iIndexI=0; iIndexI<usExpLen; iIndexI++) 
    {
        *pucTemp++ = stPubKey.pucExponent[iIndexI];
    }
    Sec_ExportStrUint16to8(&usModLen, pucTemp);
    pucTemp += 2;
    for (iIndexI=0; iIndexI<usModLen; iIndexI++) 
    {
        *pucTemp++ = stPubKey.pucModulus[iIndexI];   
    }   
    
    pucMasterSecret = (WE_UCHAR *)WE_MALLOC(20 * sizeof(WE_UCHAR));  
    if (NULL == pucMasterSecret)
    {
        WE_FREE(pucPreMasterSecret);
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0, NULL, 0);
        return;
    }
    /* get the master secret. */
    iRes = Sec_WimGetWTLSMasterSecret(hSecHandle, 0, pucPreMasterSecret,  iPreMasterSecretLen,
        MASTER_SECRET, pucRandVal, 32, 20, pucMasterSecret,ucAlg);
    if (iRes != M_SEC_ERR_OK)
    {
        for (iIndexI=0; iIndexI<20; iIndexI++)
        {
            pucMasterSecret[iIndexI] = 0;
        }
        WE_FREE(pucMasterSecret);
        WE_FREE(pucPreMasterSecret);
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, iRes, 0, NULL, 0);
        return;
    }  
    stKey = stPubKey;
    /* encrypt the random value(20 byte) used public key.*/
    iRes = Sec_PKCRsaPublicKeyEncryption(hSecHandle, iTargetID, stKey, pucPreMasterSecret, 20);  
    /* clear pre-master secret and free it. */
    for (iIndexI=0; iIndexI<iPreMasterSecretLen; iIndexI++)
    {
        pucPreMasterSecret[iIndexI] = 0;
    }
    WE_FREE(pucPreMasterSecret);     
    if (iRes != M_SEC_ERR_OK)
    {
        for (iIndexI=0; iIndexI<20; iIndexI++)
        {
            pucMasterSecret[iIndexI] = 0;
        }
        WE_FREE(pucMasterSecret);
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, iRes, 0, NULL, 0);
        return;
    } 
    pstP = (St_SecWimWtlsKeyExRsa *)WE_MALLOC(sizeof(St_SecWimWtlsKeyExRsa));
    if (NULL == pstP)
    {
        for (iIndexI=0; iIndexI<20; iIndexI++)
        {
            pucMasterSecret[iIndexI] = 0;
        }
        WE_FREE(pucMasterSecret);
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0, NULL, 0);
        return;
    } 
    pstP->iTargetID = iTargetID;
    pstP->pucMasterSecret = pucMasterSecret;    
    iRes = Sec_WimAddNewElement(hSecHandle, iTargetID, M_SEC_WIM_WTLS_KEYEXCHANGE_RSA, pstP);
    if(iRes!= M_SEC_ERR_OK)
    {
        for (iIndexI=0; iIndexI<20; iIndexI++)
        {
            pucMasterSecret[iIndexI] = 0;
        }
        WE_FREE(pucMasterSecret);
        WE_FREE(pstP);
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0, NULL, 0);
        return;
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsKeyExRsaResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        store master secret in session cache.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_iWimWtlsKeyExRsaResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                              St_SecCrptEncryptPkcResp *pstParam)
{
    St_SecWimWtlsKeyExRsa *pstP = NULL;
    WE_INT32    iRes = 0;
    WE_INT32    iMasterSecretId = 0;
    WE_INT32    iIndexI = 0; 
    
    if ((NULL==hSecHandle) || (NULL==pstElement)) 
    {
        return;
    }
    pstP = pstElement->pvPointer;
    if(NULL==pstParam)
    {
        Sec_WimWtlsKeyExchangeResponse(hSecHandle, pstP->iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
                                            0, NULL, 0);
    }
    else
    {
        iRes = pstParam->sResult;
        if (iRes != M_SEC_ERR_OK)
        {
            Sec_WimWtlsKeyExchangeResponse(hSecHandle, pstP->iTargetID, iRes, 0, NULL, 0);
        }
        else
        {
            /* store master secret in session cache . iMasterSecretId equals session number.*/
            Sec_WimSaveMastSecret(hSecHandle, pstP->pucMasterSecret, M_SEC_WTLS_MASTER_SECRET_LEN, 
                &iMasterSecretId, &iRes);
            if (iRes != M_SEC_ERR_OK)
            {
                Sec_WimWtlsKeyExchangeResponse(hSecHandle, pstP->iTargetID, iRes, 0, NULL, 0); 
            }  
            else
            {
                Sec_WimWtlsKeyExchangeResponse(hSecHandle, pstP->iTargetID, M_SEC_ERR_OK, iMasterSecretId, 
                    pstParam->pucBuf, pstParam->sBufLen);
            }
        }
    }
    /*clear master secret and free it. */
    for (iIndexI=0; iIndexI<20; iIndexI++)
    {
        pstP->pucMasterSecret[iIndexI] = 0;
    }
    WE_FREE(pstP->pucMasterSecret);
    WE_FREE(pstP);
    WE_FREE(pstElement);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsVerifySvrCertChain
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Interface of verifying server certificate chain. 
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]:  ID of the object.
        pucBuf[IN]:  pointer to the data buf.
        iBufLen[IN]: the length of the data buf.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID  Sec_iWimWtlsVerifySvrCertChain(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                     const WE_UCHAR *pucBuf, WE_INT32 iBufLen)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    
    Sec_WimVerifyCertChainStart(hSecHandle, iTargetID, 0, NULL, pucBuf, iBufLen,
        M_SEC_WTLS_CONNECTION_MODE, M_SEC_CERT_USAGE_SERVER_AUTH);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimWtlsChkUCertChain
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Interface of verifying user certificate chain. 
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucBuf[IN]: pointer to the data buf.
        iBufLen[IN]: the length of the data buf.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimWtlsChkUCertChain(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                     const WE_UCHAR *pucBuf, WE_INT32 iBufLen)
{
    if (NULL == hSecHandle)
    {
        return;
    }
    /* check if the number of user certificates has got the max. */
    if (Sec_WimGetNumUserAndPubKeyCerts(hSecHandle) == M_SEC_USER_CERT_MAX_SIZE)
    {
        Sec_WimWtlsVerifyUserCertChainResponse(hSecHandle, iTargetID, 
            M_SEC_ERR_REACHED_USER_CERT_LIMIT, 0, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;    
    }
    
    Sec_WimVerifyCertChainStart(hSecHandle, iTargetID, 0, NULL, pucBuf, iBufLen, 
        0, M_SEC_CERT_USAGE_CLIENT_AUTH);
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifyCertChainStart
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Get out certificate from 'certs' or 'buffer'.
        Parse the first certificate of the certificate chain, and decide what to do next .    
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]:  ID of the object.
        iNumCerts[IN]: the number of certificates in certificate chain.
        pstCerts[IN]: certificates maybe stores certificate chain.
        pucBuf[IN]: pointer to the data buf.
        iBufLen[IN]: the length of the data buf.
        iConnType[IN]: type of connect.
        iUsage[IN]: usage of certificate: server or client.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Next work is verifying the next certificate in certificate chain, which maybe is root certificate.
=====================================================================================*/
static WE_VOID Sec_WimVerifyCertChainStart (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                         WE_INT32 iNumCerts,  St_SecAsn1Certificate *pstCerts, 
                                         const WE_UCHAR *pucBuf, WE_INT32 iBufLen, WE_INT32 iConnType, 
                                         WE_INT32 iUsage)
{
    WE_INT32         iResult = 0;
    WE_INT32         iIndexI = 0;
    St_SecWimVerifyCertChain *pstP = NULL;
    WE_UCHAR         *pucCert = NULL;
    St_SecPubKeyRsa  stKey = {0};
    WE_UINT16        usFirstCertLen = 0;
    WE_UCHAR         ucSignatureAlgId = 0;
    WE_UCHAR         *pucTmpCert = NULL;
    WE_UINT8         ucTmpNumCerts = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    pstP = (St_SecWimVerifyCertChain *)WE_MALLOC(sizeof(St_SecWimVerifyCertChain));
    if (NULL == pstP)
    {
        if (M_SEC_CERT_USAGE_SERVER_AUTH == iUsage)
        {
            Sec_WimVerifyCertChainResponse(hSecHandle, iTargetID,
                M_SEC_ERR_INSUFFICIENT_MEMORY, 0, 0, 0);
            return;
        }
        else 
        {
            Sec_WimWtlsVerifyUserCertChainResponse(hSecHandle, iTargetID, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, 0, NULL, 0, 0, 0);
            return;
        }
    }        
    pstP->iTargetID = iTargetID;
    pstP->pstCerts = NULL;
    pstP->iNumCerts = 0;
    pstP->iConnType = iConnType;
    pstP->iUsage = iUsage;
    pstP->iIndex = 0;
    pstP->pucRootCert = NULL;
    pstP->usRootCertLen = 0;
    pstP->iCertIsOld = 0;
    pstP->iRootCertId = 0;    
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
            M_SEC_ERR_WIM_NOT_INITIALISED, NULL, 0, 0, 0); 
        SEC_OLD_ROOT_CERT_COUNT = 0;   
        return;
    }   
    
    /* copy out the certificates in certificate chain.*/
    if (pstCerts != NULL)
    {
        pstP->pstCerts = (St_SecAsn1Certificate *)WE_MALLOC((WE_UINT32)iNumCerts * sizeof(St_SecAsn1Certificate));
        if ((NULL == pstP->pstCerts) && (iNumCerts != 0))
        {
            Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, 0, 0); 
            SEC_OLD_ROOT_CERT_COUNT = 0;
            return;
        }  
        for (iIndexI=0; iIndexI<iNumCerts; iIndexI++)
        {
            pstP->pstCerts[iIndexI].ucFormat = pstCerts[iIndexI].ucFormat;
            pstP->pstCerts[iIndexI].pucCert = (WE_UCHAR * )WE_MALLOC \
                (pstCerts[iIndexI].uiCertLen * sizeof(WE_UCHAR));
            if ((NULL == (pstP->pstCerts[iIndexI].pucCert)) && (pstCerts[iIndexI].uiCertLen != 0))
            {
                Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
                    M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, 0, 0);
                SEC_OLD_ROOT_CERT_COUNT = 0;
                return;
            }    
            (WE_VOID)WE_MEMCPY (pstP->pstCerts[iIndexI].pucCert, pstCerts[iIndexI].pucCert,
                pstCerts[iIndexI].uiCertLen);
            pstP->pstCerts[iIndexI].uiCertLen = pstCerts[iIndexI].uiCertLen;           
            (pstP->iNumCerts)++;
        }
    } 
    else /* certificates stored in buffer haven't been picked out.*/
    {
        /* get the number of certificates.*/
        iResult = Sec_WimVerifiCertChain(hSecHandle, pucBuf, iBufLen, NULL, &ucTmpNumCerts);         
        if (iResult != M_SEC_ERR_OK)
        {
            Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iResult, NULL, 0, 0, 0);
            SEC_OLD_ROOT_CERT_COUNT = 0;
            return;      
        }
        
        pstP->pstCerts = (St_SecAsn1Certificate *)WE_MALLOC \
            (ucTmpNumCerts * sizeof(St_SecAsn1Certificate));
        if ((NULL == (pstP->pstCerts)) && (ucTmpNumCerts != 0))
        {
            Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, 0, 0);
            SEC_OLD_ROOT_CERT_COUNT = 0;
            return;                    
        }
        /*let 'pstP->pstCerts' points to the certificates stored in buffer.*/
        iResult = Sec_WimVerifiCertChain(hSecHandle, pucBuf, iBufLen, 
            pstP->pstCerts, &ucTmpNumCerts);         
        if (iResult != M_SEC_ERR_OK) 
        {
            Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iResult, NULL, 0, 0, 0);  
            SEC_OLD_ROOT_CERT_COUNT = 0;
            return;
        }        
        /* copy out the certificates in certificate chain.*/
        for (iIndexI=0; iIndexI<ucTmpNumCerts; iIndexI++) 
        {
            pucTmpCert = pstP->pstCerts[iIndexI].pucCert;
            pstP->pstCerts[iIndexI].pucCert =
                (WE_UCHAR * )WE_MALLOC(pstP->pstCerts[iIndexI].uiCertLen * sizeof(WE_UCHAR));
            if ((NULL == (pstP->pstCerts[iIndexI].pucCert))
                && (pstP->pstCerts[iIndexI].uiCertLen != 0))
            {
                Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
                    M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, 0, 0);
                SEC_OLD_ROOT_CERT_COUNT = 0;
                return;
            }       
            (WE_VOID)WE_MEMCPY(pstP->pstCerts[iIndexI].pucCert, pucTmpCert,
                pstP->pstCerts[iIndexI].uiCertLen);                     
            (pstP->iNumCerts)++;
        }
        pstCerts = pstP->pstCerts;
        iNumCerts = pstP->iNumCerts;
    }
    
    /* parse the first certificate .*/
    pucCert = pstCerts[0].pucCert;
    if (pstCerts[0].ucFormat == M_SEC_CERTIFICATE_WTLS_TYPE)  /* wtls */
    {
        iResult = Sec_WtlsCertParse(hSecHandle, pucCert, &usFirstCertLen, &(pstP->stCert)); 
        stKey.usExpLen = pstP->stCert.stCertUnion.stWtls.usExpLen;
        stKey.pucExponent = pstP->stCert.stCertUnion.stWtls.pucRsaExponent;
        stKey.usModLen = pstP->stCert.stCertUnion.stWtls.usModLen;
        stKey.pucModulus = pstP->stCert.stCertUnion.stWtls.pucRsaModulus;
        pstP->usIssuerLen = pstP->stCert.stCertUnion.stWtls.ucIssuerLen;
        pstP->ucAlg = M_SEC_SP_RSA;
        pstP->uskeyExchKeyLen  = (WE_UINT16)(stKey.usModLen * 8);
    }
    else if (pstCerts[0].ucFormat == M_SEC_CERTIFICATE_X509_TYPE)   /* x509 */
    {
        iResult = Sec_X509CertParse(hSecHandle, pucCert, &usFirstCertLen, &pstP->stCert); 
        pstP->usIssuerLen = pstP->stCert.stCertUnion.stX509.usIssuerLen;
        ucSignatureAlgId = pstP->stCert.stCertUnion.stX509.pucSignatureAlgId \
            [pstP->stCert.stCertUnion.stX509.usSignatureAlgIdLen - 1];
        if (iResult == M_SEC_ERR_OK)
        {
            if ((ucSignatureAlgId == 5) || (ucSignatureAlgId == 4)
#ifdef M_SEC_CFG_MD2
                || (ucSignatureAlgId == 2)
#endif 
                ) 
            {
                pstP->ucAlg = M_SEC_SP_RSA;
                iResult = Sec_X509GetRsaModAndExp(hSecHandle, 
                    pstP->stCert.stCertUnion.stX509.pucPublicKeyVal,
                    &(stKey.pucModulus), &(stKey.usModLen), 
                    &(stKey.pucExponent), &(stKey.usExpLen));
                pstP->uskeyExchKeyLen = (WE_UINT16)(stKey.usModLen * 8);
            }
            else
            {
                Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP,
                    M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, NULL, 0, 0, 0);
                SEC_OLD_ROOT_CERT_COUNT = 0;
                return;
            }
        }
    }
    else
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP,
            M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }
    
    if (iResult != M_SEC_ERR_OK) 
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iResult, NULL, 0, 0, 0);  
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }    
    
    if (iNumCerts > 1) 
    {
        Sec_WimVerifyCertChainInter(hSecHandle, pstP);
    }
    else
    {
        Sec_WimVerifyCertChainRoot(hSecHandle, pstP);
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifyCertInter
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Use public key which is taken out from the next certificate, to verify the former certificate.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        pstP[IN]: Variable stored parameters about certificate chain.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
static WE_VOID Sec_WimVerifyCertChainInter(WE_HANDLE hSecHandle, 
                                        St_SecWimVerifyCertChain *pstP)
{
    WE_INT32         iRes = 0;
    WE_INT32         iIndexI = 0;
    WE_INT32         iTargetID = 0;
    WE_UCHAR         *pucPrevCert = NULL;
    WE_UCHAR         *pucCert = NULL;   
    WE_INT32         iPrevFormat = 0;
    WE_INT32         iFormat = 0;
    WE_UINT16        usCertLen = 0;
    St_SecPubKeyRsa  stKey = {0};
    
    if ((NULL == hSecHandle) || (NULL == pstP))
    {
        return;
    }
    iIndexI = pstP->iIndex;
    iTargetID = pstP->iTargetID;
    pucPrevCert = pstP->pstCerts[iIndexI].pucCert;
    iPrevFormat = pstP->pstCerts[iIndexI].ucFormat;
    pucCert = pstP->pstCerts[iIndexI+1].pucCert;
    iFormat = pstP->pstCerts[iIndexI+1].ucFormat;     
    /* Parse the next certificate and get the public key. pstP->stCert point to the next certificate.*/
    if (iFormat == M_SEC_CERTIFICATE_WTLS_TYPE)
    {
        iRes = Sec_WtlsCertParse(hSecHandle, pucCert, &usCertLen, &pstP->stCert); 
        stKey.usExpLen = pstP->stCert.stCertUnion.stWtls.usExpLen;
        stKey.pucExponent = pstP->stCert.stCertUnion.stWtls.pucRsaExponent;
        stKey.usModLen = pstP->stCert.stCertUnion.stWtls.usModLen;
        stKey.pucModulus = pstP->stCert.stCertUnion.stWtls.pucRsaModulus;
        pstP->usIssuerLen = pstP->stCert.stCertUnion.stWtls.ucIssuerLen;
    }
    else if (iFormat == M_SEC_CERTIFICATE_X509_TYPE)  
    {
        iRes = Sec_X509CertParse(hSecHandle, pucCert, &usCertLen, &pstP->stCert); 
        stKey.usExpLen = 0; 
        pstP->usIssuerLen = pstP->stCert.stCertUnion.stX509.usIssuerLen;
    }
    else
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
            M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE, NULL, 0, 0, 0); 
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, NULL, 0, 0, 0); 
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }
    /* use the public key to verify the former certificate.*/
    if (iPrevFormat == M_SEC_CERTIFICATE_WTLS_TYPE)            
    {    
        iRes = Sec_WimChkWTLSCert(hSecHandle, iTargetID, stKey, pucPrevCert); 
    }
    else if (iPrevFormat == M_SEC_CERTIFICATE_X509_TYPE)   
    {    
        iRes = Sec_WimChkX509Cert(hSecHandle, iTargetID, 
            pstP->stCert.stCertUnion.stX509.pucPublicKeyVal, 
            pucPrevCert, pstP->iConnType, (iIndexI+1), pstP->iUsage);
    }
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }  
    
    iRes = Sec_WimAddNewElement(hSecHandle, iTargetID, M_SEC_WIM_VERIFY_CERT_CHAIN_INTERMEDIATE, pstP);
    if(iRes != M_SEC_ERR_OK)
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimVerifyCertInterResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Response of verify intermediate certificate.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimVerifyCertInterResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement, 
                                 St_SecCrptVerifySignatureResp *pstParam)
{
    St_SecWimVerifyCertChain    *pstP = NULL;
    WE_INT32    iResult = 0;  
    
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    pstP = pstElement->pvPointer;
    if (NULL==pstParam)
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        WE_FREE(pstElement); 
        return;
    }
    iResult = pstParam->sResult;
    WE_FREE(pstElement); 
    
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iResult, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }
    
    pstP->iIndex++;
    if (pstP->iIndex < (pstP->iNumCerts - 1)) 
    {
        Sec_WimVerifyCertChainInter(hSecHandle, pstP);
    }
    else  /* for root certificate: the 'index' equals to 'NumCerts-1'. */
    {
        Sec_WimVerifyCertChainRoot(hSecHandle, pstP);
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifyCertChainRoot
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Get root certificate and parse it. If it's valid, judge whether it is self-signed.
        If root certificate is not self-signed, verify it by its extension.
        At last, verify the former certificate use the root certificate. 
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        pstP[IN]: Variable stored parameters about certificate chain.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
static WE_VOID Sec_WimVerifyCertChainRoot (WE_HANDLE hSecHandle,
                                        St_SecWimVerifyCertChain *pstP)
{ 
    WE_INT32         iIndexI = 0;
    WE_INT32         iTargetID = 0;
    WE_INT32         iRes = 0;
    WE_INT32         iNumRootCerts = 0;
    WE_UCHAR         *pucPrevCert = NULL;  
    WE_INT32         iPrevFormat = 0;
    WE_INT32         iCertIndex = 0;
    WE_UCHAR         *pucRootCert = NULL;
    WE_UINT16        usRootCertLen = 0;
    WE_INT32         iCertIsOld = 0;
    WE_INT32         iMatch = 0;
    WE_UINT32        uiNow = 0;
    St_SecCertificate    stCert = {0};
    St_SecPubKeyRsa      stKey = {0};
    WE_UINT16        usSubjectLen = 0;
    WE_UINT32        uiFileId = 0;
    WE_UCHAR         *pucKeyIdentifier = NULL;
    WE_UINT16        usKeyIdentifierLen = 0;  
    WE_UINT16        usLen = 0; 
    WE_UINT8         ucCertType = 0; 
    WE_UINT8         ucCertFormat = 0; 
    WE_UCHAR         *pucTempRootCert = NULL;
    WE_UCHAR         ucAble = 0;
    
    if ((NULL == hSecHandle) || (NULL == pstP))
    {
        return;
    }
    iIndexI = pstP->iIndex;
    iTargetID = pstP->iTargetID;
    pucPrevCert = pstP->pstCerts[iIndexI].pucCert;
    iPrevFormat = pstP->pstCerts[iIndexI].ucFormat;
    
    iRes = Sec_WimGetNbrOfCACerts(hSecHandle, (WE_UINT8 *)(&iNumRootCerts));
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }
    for (iCertIndex=0; iCertIndex<iNumRootCerts; iCertIndex++)
    { 
        iCertIsOld = 0;
        usLen = 0;
        /* if it is root certificate, get out of it.*/
        iRes = Sec_WimGetCACert(hSecHandle, iCertIndex+1, &pucRootCert,
            &usRootCertLen, &uiFileId, &ucCertType, &ucAble);
        if (iRes == M_SEC_ERR_OK)
        {
            if (M_SEC_WTLS_CA_CERT == ucCertType) /* wtls format */
            {
                if (M_SEC_WTLS_CA_CERT_DISABLE == ucAble)
                { 
                    WE_FREE(pucRootCert);
                    continue;
                }
                iRes = Sec_WtlsCertParse(hSecHandle, pucRootCert, &usLen, &stCert);
                usSubjectLen = stCert.stCertUnion.stWtls.ucSubjectLen;
                stKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
                stKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
                stKey.usModLen = stCert.stCertUnion.stWtls.usModLen;
                stKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
                ucCertFormat = M_SEC_CERTIFICATE_WTLS_TYPE;
            }
            else if (M_SEC_X509_CA_CERT == ucCertType)  /* x509 formate */
            {
                iRes = Sec_X509CertParse(hSecHandle, pucRootCert, &usLen, &stCert);
                stKey.usExpLen = 0;
                usSubjectLen = stCert.stCertUnion.stX509.usSubjectLen;
                ucCertFormat= M_SEC_CERTIFICATE_X509_TYPE;
            }
            else
            {
                Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP,
                    M_SEC_ERR_GENERAL_ERROR, NULL, 0, 0, 0);
                SEC_OLD_ROOT_CERT_COUNT = 0;
                WE_FREE(pucRootCert);
                return;
            }            
            if (iRes != M_SEC_ERR_OK)
            {
                Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, NULL, 0, 0, 0);
                SEC_OLD_ROOT_CERT_COUNT = 0;
                WE_FREE(pucRootCert);
                return;
            }
            /* add format for root certificate.*/
            pucTempRootCert = (WE_UCHAR*)WE_MALLOC((usRootCertLen + 1) * sizeof(WE_UCHAR)); 
            if (NULL == pucTempRootCert)
            {
                Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, 0, 0);
                SEC_OLD_ROOT_CERT_COUNT = 0;
                WE_FREE(pucRootCert);
                return;
            }
            pucTempRootCert[0] = ucCertFormat;            
            (WE_VOID)WE_MEMCPY(pucTempRootCert+1, pucRootCert, usRootCertLen);
            usRootCertLen++;
            
            /*judge if the certificate is valid. */
            uiNow = SEC_CURRENT_TIME();
            if ((uiNow < stCert.iValidNotBefore) ||(stCert.iValidNotAfter < uiNow)) 
            {  
                /*modified by bird 061121,delete and omit it,do not show name confirm dialog*/
                /*
                iCertIsOld = 1;                
                if (SEC_OLD_ROOT_CERT_COUNT == 0)
                {
                    SEC_OLD_ROOT_CERT_COUNT = 1;
                    WE_FREE(pucRootCert);
                    WE_FREE(pucTempRootCert);
                    
                    Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
                        M_SEC_ERR_OLD_ROOTCERT_FOUND, NULL, 
                        0, iCertIsOld, (WE_INT32)uiFileId);
                    return;
                   
                }
                */
                WE_FREE(pucRootCert);
                WE_FREE(pucTempRootCert);
                pucRootCert = NULL;
                pucTempRootCert = NULL;
                continue;

            }
            
            if (!iCertIsOld)
            {
                /* judge if the subject of root certificate equals the issuer of last certificate.*/
                if (pstP->usIssuerLen == usSubjectLen &&
                    !WE_MEMCMP(pstP->stCert.pucIssuer, stCert.pucSubject, usSubjectLen))  
                {                   
                    iMatch = TRUE;
                }                
                /* if it not matched, find the subject from extension of root certificate.*/
                if (!iMatch && (ucCertType == M_SEC_CERTIFICATE_X509_TYPE) && 
                    ((iPrevFormat) == M_SEC_CERTIFICATE_WTLS_TYPE) &&   
                    (pstP->stCert.stCertUnion.stWtls.ucIssuerIdentifierType == 254) &&
                    (stCert.ucCertificateVersion == 3))   
                { 
                    usKeyIdentifierLen = (WE_UINT16)(stCert.stCertUnion.stX509.uiExtSubjectKeyIdLen);
                    pucKeyIdentifier = stCert.stCertUnion.stX509.pucExtSubjectKeyId;
                    if ((pstP->usIssuerLen == usKeyIdentifierLen) && 
                        !WE_MEMCMP(pstP->stCert.pucIssuer, pucKeyIdentifier, usKeyIdentifierLen)) 
                    {
                        iMatch = TRUE;
                    }
                }
            }    
            
            /* verify the former certificate using public key of root certificate. */
            if (TRUE == iMatch)
            {
                if (iPrevFormat == M_SEC_CERTIFICATE_WTLS_TYPE)
                {
                    iRes = Sec_WimChkWTLSCert(hSecHandle, iTargetID, stKey, pucPrevCert); 
                }
                else if (iPrevFormat == M_SEC_CERTIFICATE_X509_TYPE) 
                {
                    iRes = Sec_WimChkX509Cert(hSecHandle, iTargetID, 
                        stCert.stCertUnion.stX509.pucPublicKeyVal, 
                        pucPrevCert, pstP->iConnType, (iIndexI+1), pstP->iUsage);
                }      
                WE_FREE(pucRootCert);
                if (iRes == M_SEC_ERR_OK) 
                {          
                    pstP->pucRootCert = pucTempRootCert;                    
                    pstP->usRootCertLen = usRootCertLen;
                    pstP->iCertIsOld = iCertIsOld;
                    pstP->iRootCertId = (WE_INT32)uiFileId;
                    iRes = Sec_WimAddNewElement (hSecHandle, pstP->iTargetID, 
                        M_SEC_WIM_VERIFY_CERT_CHAIN_ROOT,pstP);
                    if(iRes != M_SEC_ERR_OK)
                    {
                        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, 
                            pucTempRootCert, usRootCertLen, iCertIsOld, (WE_INT32)uiFileId);
                        WE_FREE(pucTempRootCert);
                        SEC_OLD_ROOT_CERT_COUNT = 0;  
                    }
                } 
                else 
                {
                    Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, 
                        pucTempRootCert, usRootCertLen, iCertIsOld, (WE_INT32)uiFileId);
                    WE_FREE(pucTempRootCert);
                    SEC_OLD_ROOT_CERT_COUNT = 0;  
                }
                return;
            } 
            else 
            {        
                WE_FREE(pucRootCert);
                WE_FREE(pucTempRootCert);
                continue;
            }
        } 
        else /* failure of getting the CA certificate.*/
        {
            Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, iRes, 
                pucRootCert, usRootCertLen, iCertIsOld, (WE_INT32)uiFileId);
            SEC_OLD_ROOT_CERT_COUNT = 0;  
            return;
        }
    } 
    /* check if the former certificate is expired. */
    uiNow = SEC_CURRENT_TIME();
    if ((uiNow < pstP->stCert.iValidNotBefore) ||(pstP->stCert.iValidNotAfter < uiNow)) 
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
            M_SEC_ERR_CERTIFICATE_EXPIRED, NULL, 0, 0, 0);
        SEC_OLD_ROOT_CERT_COUNT = 0;
        return;
    }  
    
    Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, 
        M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE, NULL, 0, 0, 0);
    SEC_OLD_ROOT_CERT_COUNT = 0;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimVerifyCertRootResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Response of verify root certificate.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimVerifyCertRootResp(WE_HANDLE hSecHandle, St_SecWimElement *pstElement,
                                St_SecCrptVerifySignatureResp *pstParam)
{
    St_SecWimVerifyCertChain *pstP = NULL;
    
    if ((NULL==hSecHandle) || (NULL==pstElement))
    {
        return;
    }
    pstP = pstElement->pvPointer;
    WE_FREE(pstElement);
    if(NULL==pstParam)
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, M_SEC_ERR_INSUFFICIENT_MEMORY,
                    pstP->pucRootCert, 
                    pstP->usRootCertLen, 
                    pstP->iCertIsOld, 
                    pstP->iRootCertId);
    }
    else
    {
        Sec_WimVerifyCertChainDeliverResp(hSecHandle, pstP, pstParam->sResult, pstP->pucRootCert, 
                    pstP->usRootCertLen, pstP->iCertIsOld, pstP->iRootCertId);
    }
    SEC_OLD_ROOT_CERT_COUNT = 0;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifyCertChainDeliverResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Interface of response of verifying certificate chain. 
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        pstP[IN]: structure stored parameter.
        iResult[IN]: result of last operation.
        pucCaCert[IN]: CA certificate.
        usCaCertLen[IN]: length of CA certificate.
        iRootCertIsOld[IN]: sign of root certificate is old or not.
        iRootCertIsOldId[IN]: Id of root certificate which is old.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
static WE_VOID Sec_WimVerifyCertChainDeliverResp (WE_HANDLE hSecHandle,
                                               St_SecWimVerifyCertChain *pstP, WE_INT32 iResult,
                                               WE_UCHAR *pucCaCert, WE_UINT16 usCaCertLen,
                                               WE_INT32 iRootCertIsOld, WE_INT32 iRootCertIsOldId)
{
    WE_INT32         iIndexI = 0;
    
    if ((NULL == hSecHandle) || (NULL == pstP))
    {
        return;
    }
    if (pstP->iUsage == M_SEC_CERT_USAGE_SERVER_AUTH)
    {
        Sec_WimVerifyCertChainResponse(hSecHandle, pstP->iTargetID, iResult,            
            pstP->uskeyExchKeyLen, iRootCertIsOld, iRootCertIsOldId);
    }
    else
    {
        Sec_WimWtlsVerifyUserCertChainResponse(hSecHandle, pstP->iTargetID, iResult, pstP->ucAlg,
            pucCaCert, usCaCertLen, iRootCertIsOld, iRootCertIsOldId);
    }
    
    for (iIndexI=0; iIndexI<pstP->iNumCerts; iIndexI++)
    {
        WE_FREE(pstP->pstCerts[iIndexI].pucCert);
    }
    if (NULL != (pstP->pstCerts))
    {
        WE_FREE(pstP->pstCerts);
    }
    if (NULL != (pstP->pucRootCert)) 
    {
        WE_FREE(pstP->pucRootCert);
    }
    WE_FREE (pstP);
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetCertByID
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Read out the certificate by ID of it.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iCertId[IN]: Id of certificate. 
        ppucCert[OUT]: point to pointer of the certificate.
        pusCertLen[OUT]: point to the length of certificate.
RETURN VALUE:
        M_SEC_ERR_OK: success.
        other: fail.   
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimGetCertByID(WE_HANDLE hSecHandle, WE_INT32 iCertId, 
                            WE_UCHAR **ppucCert, WE_UINT16 *pusCertLen)
{
    WE_INT8      cFileType = 0;
    WE_INT32     iFileSize = 0; 
    WE_HANDLE    hFileHandle = NULL;
    WE_CHAR      *pcFileName = NULL;
    WE_INT32     iResult = 0;
    WE_INT32     iRes = 0;
    E_WE_ERROR   eRes = E_WE_OK;
    
    if ((NULL == hSecHandle) || (NULL == ppucCert) || (NULL == pusCertLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if ((iCertId>=M_SEC_USER_CERT_ID_START) && (iCertId<M_SEC_CA_CERT_ID_START))
    {
        cFileType = M_SEC_USER_CERT_HEADER;
    }
    else if (iCertId>=M_SEC_CA_CERT_ID_START && iCertId<M_SEC_CONTRACT_CERT_ID_START)
    {
        cFileType = M_SEC_CA_CERT_HEADER;
    }
    else
    {
        return M_SEC_ERR_MISSING_CERTIFICATE;
    }
    
    iRes = Sec_WimAcqFileName(cFileType, (WE_UINT32)iCertId, &pcFileName);
    if (iRes != M_SEC_ERR_OK)
    {
        if(NULL != pcFileName)
        {
            WE_FREE(pcFileName);
            pcFileName = NULL;
        }
        return iRes;            
    }
    /* check if the certificate exists.*/
    iRes = Sec_WimCheckFileExist(hSecHandle, pcFileName);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pcFileName);
        return M_SEC_ERR_MISSING_CERTIFICATE;            
    }
    /* get the size of certificate.*/
    eRes = WE_FILE_GETSIZE(SEC_WE_HANDLE, pcFileName, &(iFileSize));
    if (E_WE_OK != eRes)
    {
        WE_FREE(pcFileName);    
        return M_SEC_ERR_GENERAL_ERROR;            
    }
    (*ppucCert) = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iFileSize * sizeof(WE_UCHAR));
    if ((NULL == (*ppucCert)) && (iFileSize != 0))
    {
        WE_FREE(pcFileName);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;    
    }
    /* open and read out the certificate.*/
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, pcFileName, WE_FILE_SET_RDONLY, &hFileHandle);
    WE_FREE(pcFileName);
    if (eRes != E_WE_OK)
    {
        WE_FREE(*ppucCert);    
        return M_SEC_ERR_GENERAL_ERROR;            
    }    
    eRes = WE_FILE_READ(hFileHandle, (*ppucCert), (WE_LONG)iFileSize, &(iResult));
    if ((iResult < iFileSize) || (eRes != E_WE_OK))
    {        
        WE_FREE(*ppucCert);     
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);

    *pusCertLen = (WE_UINT16)iFileSize;
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetCertNameByID
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Read out the name of certificate by ID.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iCertId[IN]: Id of certificate. 
        ppucCertNames[OUT]: name of certificate.
        pusNameLen[OUT]: length of name.
        psCharSet[OUT]: character set.
        pcNameType[OUT]: type of name.
RETURN VALUE:
        M_SEC_ERR_OK: success.
        other: fail.   
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimGetCertNameByID(WE_HANDLE hSecHandle,
                                WE_INT32 iCertId, WE_UINT8 **ppucCertNames, 
                                WE_UINT16 *pusNameLen, WE_INT16 *psCharSet,
                                WE_INT8 *pcNameType)
{
    WE_HANDLE hFileHandle = NULL;
    WE_CHAR   *pcIndexFileName = NULL;
    WE_UCHAR  *pucIndexFile = NULL;
    WE_UCHAR  *pucP = NULL;
    E_WE_ERROR eRes = E_WE_OK;
    WE_INT32  iResult = 0;
    WE_INT32  iIndexFileSize = 0;
    WE_INT32  iNumInIndexFile = 0;
    WE_INT32  iTmpCertId = 0;
    WE_INT32  iIndexI = 0;
    WE_INT32  iBlockLen = 0;
    WE_CHAR   cFileType = 0;

    if ((NULL == hSecHandle) || (NULL == ppucCertNames) 
        || (NULL == pcNameType) || (NULL == pusNameLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }      
    if ((iCertId>=M_SEC_USER_CERT_ID_START) && (iCertId<M_SEC_CA_CERT_ID_START))
    {
        pcIndexFileName = M_SEC_USERCERT_INDEXFILE_NAME;
    }
    else if (iCertId>=M_SEC_CA_CERT_ID_START && iCertId<M_SEC_CONTRACT_CERT_ID_START)
    {
        pcIndexFileName = M_SEC_CA_CERT_INDEXFILE_NAME;
    }
    else
    {
        return M_SEC_ERR_MISSING_CERTIFICATE;
    }
    
    /* get size of index file.*/
    eRes = WE_FILE_GETSIZE(SEC_WE_HANDLE, pcIndexFileName, &(iIndexFileSize));
    if (E_WE_OK != eRes)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    pucIndexFile = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iIndexFileSize * sizeof(WE_UCHAR));  
    if (NULL == pucIndexFile)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    /* open indexfile and read all.*/
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, pcIndexFileName, WE_FILE_SET_RDONLY, &(hFileHandle));
    if (eRes != E_WE_OK)
    {
        WE_FREE(pucIndexFile);
        return M_SEC_ERR_GENERAL_ERROR;
    }     
    eRes = WE_FILE_READ(hFileHandle, pucIndexFile, iIndexFileSize, &(iResult));
    if ((eRes!=E_WE_OK) || (iResult<iIndexFileSize))
    {
        WE_FREE(pucIndexFile);
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }    
    eRes = WE_FILE_CLOSE(hFileHandle);

    pucP = pucIndexFile;
    iNumInIndexFile = *pucP;
    pucP++;
    for (iIndexI=0; iIndexI<iNumInIndexFile; iIndexI++) 
    {
        cFileType = (WE_CHAR)(*pucP);
        Sec_StoreStrUint8to32(pucP+1, (WE_UINT32 *)(&iTmpCertId));
        if (iTmpCertId == iCertId)
        {
            pucP += 1+4+4+M_SEC_DN_HASH_LEN+M_SEC_DN_HASH_LEN;  
            *pcNameType = (WE_CHAR)(*pucP);
            pucP++;
            Sec_StoreStrUint8to16(pucP, (WE_UINT16 *)psCharSet);
            pucP += 2;        
            if ((M_SEC_X509_CA_CERT == cFileType) || (M_SEC_WTLS_CA_CERT == cFileType))
            {
                pucP++;
            }
            Sec_StoreStrUint8to16(pucP, pusNameLen);
            pucP += 2;        
            *ppucCertNames = (WE_UCHAR *)WE_MALLOC((*pusNameLen) * sizeof(WE_UCHAR)); 
            if ((NULL==(*ppucCertNames)) && (0!=(*pusNameLen)))
            {
                WE_FREE(pucIndexFile);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            (WE_VOID)WE_MEMCPY(*ppucCertNames, pucP, *pusNameLen);        
            WE_FREE(pucIndexFile);
            return M_SEC_ERR_OK;
        }
        else
        {
            iResult = Sec_WimGetBlockLength(pucP, cFileType, &iBlockLen);
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucIndexFile);
                return M_SEC_ERR_GENERAL_ERROR;
            }
            pucP += iBlockLen;
        }
    }
    WE_FREE(pucIndexFile);    
    return M_SEC_ERR_NOT_FOUND;    
}

/*=====================================================================================
FUNCTION: 
        Sec_WimDelPrivKey
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Delete the private key file.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        ucKeyType[IN]: type of private key. 
RETURN VALUE:
        M_SEC_ERR_OK: success.
        other: fail.   
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
WE_VOID Sec_WimDelPrivKey(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType)
{
    WE_INT32 iResult = 0;
    E_WE_ERROR  eRes = E_WE_OK;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (M_SEC_PRIVATE_AUTHKEY == ucKeyType)
    {
        iResult = Sec_WimCheckFileExist(hSecHandle, (WE_CHAR*)M_SEC_AUTH_PRIVKEY_NAME);
        if(iResult == M_SEC_ERR_OK)
        {                    
            eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, M_SEC_AUTH_PRIVKEY_NAME);
        }
    }
    else if(M_SEC_PRIVATE_NONREPKEY == ucKeyType)
    {
        iResult = Sec_WimCheckFileExist(hSecHandle, (WE_CHAR *)M_SEC_NON_REP_PRIVKEY_NAME);
        if(iResult == M_SEC_ERR_OK)
        {                    
            eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, M_SEC_NON_REP_PRIVKEY_NAME);
        }
    }  
    eRes = eRes;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimViewContracts
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        View all contracts.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimViewContracts(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    WE_HANDLE          hIndexFileHandle = NULL;
    WE_HANDLE          hFileHandle = NULL;
    WE_UCHAR           *pucIndexFile = NULL;   
    WE_UINT32          uiIndexFileBlockLen = 4+4;     
    WE_UINT32          uiAllLength = 0; 
    WE_UCHAR           ucNumOfContracts= 0;
    WE_UINT32          uiFileId = 0;                   
    WE_UINT32          uiCount = 0; 
    WE_CHAR            **ppcContracts = NULL;
    WE_UINT16          *pusContractLen = NULL;
    WE_INT32           *piContractIds = NULL;
    WE_CHAR            **ppcSign = NULL; 
    WE_UINT16          *pusSignLen = NULL;
    WE_UINT32          *puiTime = NULL;
    WE_UCHAR           *pucAllData = NULL;
    WE_UCHAR           *pucP = NULL;
    WE_UCHAR           *pucTemp = NULL;
    WE_CHAR            *pcFileName = NULL;
    WE_INT32           iIndexI = 0;
    WE_INT32           iIndexJ = 0;    
    WE_INT32           iNewPos = 0;    
    WE_INT32           iResult = 0;
    WE_INT32           iRes = 0;
    E_WE_ERROR         eRes = E_WE_OK;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED,
            0, NULL, NULL, NULL, NULL, NULL, NULL);
        return;
    } 
    /* open the contract indexfile. */
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, M_SEC_CONTRACT_INDEXFILE_NAME, 
        WE_FILE_SET_RDONLY, &(hIndexFileHandle));
    if(eRes != E_WE_OK)
    {
        Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
            0, NULL, NULL, NULL, NULL, NULL, NULL);
        return;
    }
    /* read out the number of contracts. */
    eRes = WE_FILE_READ(hIndexFileHandle, &ucNumOfContracts, 1, &(iResult));
    if ((eRes!=E_WE_OK) || (iResult<1))
    {
        Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
            0, NULL, NULL, NULL, NULL, NULL, NULL);        
        eRes = WE_FILE_CLOSE(hIndexFileHandle);
        return;    
    }    
    if (0 == ucNumOfContracts)
    {
        Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, 
            0, NULL, NULL, NULL, NULL, NULL, NULL);
        eRes = WE_FILE_CLOSE(hIndexFileHandle);
        return;    
    }
    
    ppcContracts = (WE_CHAR **)WE_MALLOC((WE_UINT32)(ucNumOfContracts) * sizeof(WE_CHAR *));
    ppcSign = (WE_CHAR **)WE_MALLOC((WE_UINT32)(ucNumOfContracts) * sizeof(WE_CHAR *));
    pusSignLen = (WE_UINT16 *)WE_MALLOC((WE_UINT32)(ucNumOfContracts) * sizeof(WE_UINT16));
    puiTime = (WE_UINT32 *)WE_MALLOC((WE_UINT32)(ucNumOfContracts) * sizeof(WE_UINT32));
    piContractIds = (WE_INT32 *)WE_MALLOC((WE_UINT32)(ucNumOfContracts) * sizeof(WE_INT32));
    pusContractLen = (WE_UINT16 *)WE_MALLOC((WE_UINT32)(ucNumOfContracts) * sizeof(WE_UINT16));
    uiAllLength = uiIndexFileBlockLen * ucNumOfContracts;
    pucIndexFile = (WE_UCHAR *)WE_MALLOC(uiAllLength * sizeof(WE_UCHAR));  
    if ((ppcContracts == NULL) || (ppcSign == NULL) || (pusSignLen == NULL)
        || (puiTime == NULL) || (piContractIds == NULL) || (NULL == pucIndexFile))
    {
        Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
            ucNumOfContracts, NULL, NULL, NULL, NULL, NULL, NULL);
        if (ppcContracts != NULL)
        {
            WE_FREE(ppcContracts);
        }
        if (ppcSign != NULL)
        {
            WE_FREE(ppcSign);
        }
        if (pusSignLen != NULL)
        {
            WE_FREE(pusSignLen);
        }
        if (puiTime != NULL)
        {
            WE_FREE(puiTime);
        }
        if (piContractIds != NULL)
        {
            WE_FREE(piContractIds);
        }
        if (pucIndexFile != NULL)
        {
            WE_FREE(pucIndexFile);
        }
        if (pusContractLen != NULL)
        {
            WE_FREE(pusContractLen);
        }
        eRes = WE_FILE_CLOSE(hIndexFileHandle);
        return;
    }
    
    eRes = WE_FILE_SEEK(hIndexFileHandle, 1, WE_FILE_SEEK_SET, &(iNewPos));
    /* read out the information in indexFile. */
    eRes = WE_FILE_READ(hIndexFileHandle, pucIndexFile, (WE_LONG)uiAllLength, &(iResult));
    if (((WE_UINT32)iResult < uiAllLength) || (eRes != E_WE_OK))
    {            
        Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
            ucNumOfContracts, NULL, NULL, NULL, NULL, NULL, NULL);    
        eRes = WE_FILE_CLOSE(hIndexFileHandle);
        WE_FREE(ppcContracts);
        WE_FREE(pusContractLen);
        WE_FREE(ppcSign);
        WE_FREE(pusSignLen);
        WE_FREE(puiTime);
        WE_FREE(piContractIds);
        WE_FREE(pucIndexFile);
        return;
    }    
    eRes = WE_FILE_CLOSE(hIndexFileHandle);

    pucP = pucIndexFile;
    for (iIndexI=0; iIndexI<ucNumOfContracts; iIndexI++) 
    {
        Sec_StoreStrUint8to32(pucP, &uiFileId);
        pucP += 4;
        Sec_StoreStrUint8to32(pucP, &uiCount);
        pucP += 4;
        pucAllData = (WE_UCHAR *)WE_MALLOC(uiCount * sizeof(WE_UCHAR));
        /* open the contract.*/
        iRes = Sec_WimAcqFileName(M_SEC_CONTRACT, uiFileId, &pcFileName);
        eRes = WE_FILE_OPEN(SEC_WE_HANDLE, pcFileName, WE_FILE_SET_RDONLY, &(hFileHandle));
        WE_FREE(pcFileName);
        if ((eRes != E_WE_OK) || (NULL == pucAllData))
        {
            Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR,
                ucNumOfContracts, NULL, NULL, NULL, NULL, NULL, NULL);
            if (pucAllData != NULL)
            {
                WE_FREE(pucAllData);
            }
            if (E_WE_OK == eRes)     
            {
                eRes = WE_FILE_CLOSE(hFileHandle); 
            }
            for (iIndexJ=0; iIndexJ<iIndexI; iIndexJ++)
            {
                WE_FREE(ppcContracts[iIndexJ]);
                WE_FREE(ppcSign[iIndexJ]);
            }
            WE_FREE(ppcContracts);
            WE_FREE(pusContractLen);
            WE_FREE(ppcSign);
            WE_FREE(pusSignLen);
            WE_FREE(puiTime);
            WE_FREE(piContractIds);
            WE_FREE(pucIndexFile);
            return;
        }
        /* read out the contract.*/
        eRes = WE_FILE_READ(hFileHandle, pucAllData, (WE_LONG)uiCount, &(iResult));
        if ((eRes!=E_WE_OK) || ((WE_UINT32)iResult<uiCount))
        {
            Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                ucNumOfContracts, NULL, NULL, NULL, NULL, NULL, NULL);            
            eRes = WE_FILE_CLOSE(hFileHandle);
            WE_FREE(pucAllData);
            for (iIndexJ=0; iIndexJ<iIndexI; iIndexJ++)
            {
                WE_FREE(ppcContracts[iIndexJ]);
                WE_FREE(ppcSign[iIndexJ]);
            }
            WE_FREE(ppcContracts);
            WE_FREE(pusContractLen);
            WE_FREE(ppcSign);
            WE_FREE(pusSignLen);
            WE_FREE(puiTime);
            WE_FREE(piContractIds);
            WE_FREE(pucIndexFile);
            return;
        }             
        eRes = WE_FILE_CLOSE(hFileHandle);

        pucTemp = pucAllData;  
        /*get out contract.*/
        Sec_StoreStrUint8to16(pucTemp, &(pusContractLen[iIndexI]));
        pucTemp += 2;
        ppcContracts[iIndexI] = (WE_CHAR *)WE_MALLOC((pusContractLen[iIndexI]) * sizeof(WE_CHAR));
        if ((NULL == ppcContracts[iIndexI]) && ((pusContractLen[iIndexI]) != 0))
        {
            Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
                ucNumOfContracts, NULL, NULL, NULL, NULL, NULL, NULL);
            WE_FREE(pucAllData);
            for (iIndexJ=0; iIndexJ<iIndexI; iIndexJ++)
            {
                WE_FREE(ppcContracts[iIndexJ]);
                WE_FREE(ppcSign[iIndexJ]);
            }
            WE_FREE(ppcContracts);
            WE_FREE(pusContractLen);
            WE_FREE(ppcSign);
            WE_FREE(pusSignLen);
            WE_FREE(puiTime);
            WE_FREE(piContractIds);
            WE_FREE(pucIndexFile);
            return;
        }
        for (iIndexJ=0; iIndexJ<(pusContractLen[iIndexI]); iIndexJ++)
        {
            ppcContracts[iIndexI][iIndexJ] = (WE_CHAR)pucTemp[iIndexJ];
        }
        pucTemp += (pusContractLen[iIndexI]);     
        /* get out signature.*/
        Sec_StoreStrUint8to16(pucTemp, &(pusSignLen[iIndexI]));
        pucTemp += 2;
        ppcSign[iIndexI] = (WE_CHAR *)WE_MALLOC(pusSignLen[iIndexI] * sizeof(WE_CHAR));
        if ((NULL == ppcSign[iIndexI]) && (pusSignLen[iIndexI] != 0))
        {
            Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
                ucNumOfContracts, NULL, NULL, NULL, NULL, NULL, NULL);
            WE_FREE(pucAllData);
            for (iIndexJ=0; iIndexJ<iIndexI; iIndexJ++)
            {
                WE_FREE(ppcContracts[iIndexJ]);
                WE_FREE(ppcSign[iIndexJ]);
            }
            WE_FREE(ppcContracts[iIndexI]);
            WE_FREE(ppcContracts);
            WE_FREE(pusContractLen);
            WE_FREE(ppcSign);
            WE_FREE(pusSignLen);
            WE_FREE(puiTime);
            WE_FREE(piContractIds);
            WE_FREE(pucIndexFile);
            return;
        }
        for (iIndexJ=0; iIndexJ<pusSignLen[iIndexI]; iIndexJ++)
        {
            ppcSign[iIndexI][iIndexJ] = (WE_CHAR)pucTemp[iIndexJ];
        }
        pucTemp += pusSignLen[iIndexI]; 
        /*get out time.*/
        Sec_StoreStrUint8to32(pucTemp, &(puiTime[iIndexI]));           
        piContractIds[iIndexI] = (WE_INT32)uiFileId;
        WE_FREE(pucAllData);    
    }
    
    Sec_WimViewContractsResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, 
                             ucNumOfContracts, ppcContracts, pusContractLen, ppcSign,
                             pusSignLen, puiTime, piContractIds);
    for (iIndexJ=0; iIndexJ<ucNumOfContracts; iIndexJ++)
    {
        WE_FREE(ppcContracts[iIndexJ]);
        WE_FREE(ppcSign[iIndexJ]);
    }
    WE_FREE(piContractIds);
    WE_FREE(ppcContracts);
    WE_FREE(pusContractLen);
    WE_FREE(ppcSign);
    WE_FREE(pusSignLen);
    WE_FREE(puiTime);
    WE_FREE(pucIndexFile);
    iRes = iRes;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimStoreContract
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Interface to store Contract in files and in IndexFile.
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        iTargetID[IN]:ID of the object.
        pcContract[IN]: content to be stored.
        pcSig[IN]: pointer to signature.
        usSigLen[IN]: length of signature.
        uiTime[IN]: time of contract.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Contract: ContractLen(2)£¬Contract(textLen)£¬sigLen(2)£¬sig(sigLen)£¬time(4)
=====================================================================================*/
WE_VOID Sec_iWimStoreContract(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_CHAR *pcContract,
                           const WE_CHAR *pcSig, WE_UINT16 usSigLen, WE_UINT32 uiTime)
{
    WE_INT32   iResult = 0;   
    
    if (NULL == hSecHandle)
    {
        return;
    }    
    iResult = Sec_WimStoreContract(hSecHandle, pcContract, pcSig, usSigLen, uiTime);
    Sec_WimStoreContractResponse(hSecHandle, iTargetID, iResult);
}

/*=====================================================================================
FUNCTION: 
        Sec_WimStoreContract
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Store Contract in files and IndexFile.
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        pcData[IN]: content to be stored.
        pcSig[IN]: pointer to signature.
        usSigLen[IN]: length of signature.
        uiTime[IN]: time of contract.  
RETURN VALUE:
        M_SEC_ERR_OK : SUCCESS.
        OTHER : FAIL.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Sec_iWimStoreContract.           
IMPORTANT NOTES:
        Contract: ContractLen(2)£¬Contract(textLen)£¬sigLen(2)£¬sig(sigLen)£¬time(4)
=====================================================================================*/
static WE_INT32 Sec_WimStoreContract(WE_HANDLE hSecHandle, const WE_CHAR *pcData, 
                    const WE_CHAR *pcSig, WE_UINT16 usSigLen, WE_UINT32 uiTime)
{
    WE_HANDLE          hFileHandle = NULL;
    WE_CHAR            *pcFileName = NULL; 
    WE_UINT32          uiFileId = 0;
    WE_UINT16          usDataLen = 0;
    WE_CHAR            *pcContract = NULL;
    WE_CHAR            *pcTemp = NULL;
    WE_INT32           iContractLen = 0;
    WE_INT32           iNewPos = 0;
    WE_INT32           iResult = 0;
    WE_INT32           iRes = 0;
    WE_INT32           iIndexI = 0;
    E_WE_ERROR         eRes = E_WE_OK;
    
    if ((NULL == hSecHandle) || (NULL == pcData) || (NULL == pcSig))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }    
    if (!Sec_WimIsInited(hSecHandle))
    {
        return M_SEC_ERR_WIM_NOT_INITIALISED;  
    }
    /* creat a contract file and open it */
    iResult = Sec_WimMakeFile(hSecHandle, M_SEC_CONTRACT, &hFileHandle, &uiFileId);
    if (iResult != M_SEC_ERR_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;   
    }
    iResult = Sec_WimAcqFileName(M_SEC_CONTRACT, uiFileId, &pcFileName);
    if (iResult != M_SEC_ERR_OK)
    {
        if(NULL != pcFileName)
        {
            WE_FREE(pcFileName);
            pcFileName = NULL;
        }
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;   
    }
    
    usDataLen = (WE_UINT16)SEC_STRLEN(pcData);
    if ((usDataLen < 1) || (usSigLen < 1))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        WE_FREE(pcFileName);
        return M_SEC_ERR_INVALID_PARAMETER; 
    }
    
    eRes = WE_FILE_SETSIZE(hFileHandle, (WE_LONG)(usSigLen + usDataLen + 8));
    eRes = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET, &(iNewPos));
    pcContract = (WE_CHAR *)WE_MALLOC((usSigLen + usDataLen + 8) * sizeof(WE_CHAR));
    if ((eRes != E_WE_OK) || (NULL == pcContract))
    {
        if (NULL != pcContract)
        {
            WE_FREE(pcContract);
        }
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);    
        WE_FREE(pcFileName);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    pcTemp = pcContract;    
    /* write 'ContractLen' and 'Contract' in pcContract */
    Sec_ExportStrUint16to8(&usDataLen, (WE_UCHAR *)pcTemp);
    pcTemp = pcTemp + 2;
    for (iIndexI=0; iIndexI<usDataLen; iIndexI++)
    {
        pcTemp[iIndexI] = (WE_CHAR)pcData[iIndexI];
    }
    pcTemp += usDataLen;
    iContractLen = 2 + usDataLen;        
    /* write 'SigLen' and 'Sig' in pcContract */
    Sec_ExportStrUint16to8(&usSigLen, (WE_UCHAR *)pcTemp);
    pcTemp = pcTemp + 2;
    for (iIndexI=0; iIndexI<usSigLen; iIndexI++)
    {
        pcTemp[iIndexI] = (WE_CHAR)pcSig[iIndexI];
    }
    pcTemp += usSigLen;
    iContractLen += 2 + usSigLen;     
    /* write 'Time' in pcContract */
    Sec_ExportStrUint32to8(&uiTime, (WE_UCHAR *)pcTemp);
    iContractLen += 4;
    /* write pcContract in file. */  
    eRes = WE_FILE_WRITE(hFileHandle, (WE_VOID *)pcContract, iContractLen, &(iResult));
    WE_FREE(pcContract);
    if ((iResult < iContractLen) || (eRes != E_WE_OK))
    {
        eRes = WE_FILE_CLOSE(hFileHandle); 
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        WE_FREE(pcFileName);         
        return M_SEC_ERR_GENERAL_ERROR;
    }    
    eRes = WE_FILE_CLOSE(hFileHandle);  
    /* write information in corresponding IndexFile. */
    iRes = Sec_WimAddBlock(hSecHandle, M_SEC_CONTRACT, uiFileId, 
                            (WE_UINT32)iContractLen, NULL, NULL , 0, 0, 0, NULL, 0);  
    if (iRes != M_SEC_ERR_OK)       
    {         
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        WE_FREE(pcFileName);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    WE_FREE(pcFileName);
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimDelContract
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        delete contract and modify IndexFile.
ARGUMENTS PASSED:
        hSecHandle[IN]:Global data handle.
        iContractId[IN]:ID of the contract.     
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        Omit.           
IMPORTANT NOTES:
        Contract: ContractLen(2)£¬Contract(textLen)£¬sigLen(2)£¬sig(sigLen)£¬time(4)
=====================================================================================*/
WE_VOID Sec_iWimDelContract(WE_HANDLE hSecHandle, WE_INT32 iContractId)
{
    WE_CHAR    *pcFileName = NULL;
    WE_INT32   iRes = 0;
    E_WE_ERROR        eRes = E_WE_OK;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimDelContractResponse(hSecHandle, iContractId, M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    /* delete the block in corresponding Indexfile.*/
    iRes = Sec_WimDelBlock(hSecHandle, iContractId, M_SEC_CONTRACT);  
    if (iRes != M_SEC_ERR_OK)
    {
        Sec_WimDelContractResponse(hSecHandle, iContractId, iRes);
        return;
    }
    /* delete the contract file.*/
    iRes = Sec_WimAcqFileName(M_SEC_CONTRACT, (WE_UINT32)(iContractId), &pcFileName);
    if (iRes != M_SEC_ERR_OK)
    {
        if(NULL != pcFileName)
        {
            WE_FREE(pcFileName);
            pcFileName = NULL;
        }
        Sec_WimDelContractResponse(hSecHandle, iContractId, M_SEC_ERR_GENERAL_ERROR);
        return;   
    }
    eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
    eRes = eRes;
    WE_FREE(pcFileName);
    Sec_WimDelContractResponse(hSecHandle, iContractId, M_SEC_ERR_OK);
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimStoreView
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        If find a session file, store view in it.            
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]:Global data handle.  
        pucData: data to be stored in View file.
        iDataLen: length of data.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimStoreView(WE_HANDLE hSecHandle, const WE_UCHAR *pucData, 
                       WE_INT32 iDataLen)
{
    WE_HANDLE         hFileHandle = NULL;
    WE_CHAR           *pcFileName = M_SEC_SH_NAME;
    WE_UINT32         uiPos = 0;
    WE_INT32          iRes = 0;
    WE_INT32          iResult = 0;
    WE_INT32          iNewPos = 0;    
    WE_UINT32         uiSessionLen = 0;
    WE_UINT32         uiPeerLen = 0;
    WE_UINT32         uiViewLen = 0;
    E_WE_ERROR        eRes = E_WE_OK;
    
    if ((NULL == hSecHandle) || (NULL == pucData) || (0 == iDataLen))
    {
        return;
    }    
    if (!Sec_WimIsInited(hSecHandle))
    {
        return;  
    }    

    if ((WE_INT32)E_SEC_WTLS != SEC_CALLER_TYPE) /*is not wtls*/
    {
        return;
    }
    
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, pcFileName, WE_FILE_SET_RDWR, &hFileHandle);
    if(eRes != E_WE_OK)
    {
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    }
    /*read session ,peer length*/
    iResult = Sec_WimGetSessionLength(hSecHandle, hFileHandle, &uiSessionLen,
        &uiPeerLen, &uiViewLen, &iRes);
    if(iResult != 0)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    }    
    /* View file position */        
    uiPos = 3 * M_FILE_LENGTH_BITS + uiSessionLen + uiPeerLen;
    /*store 'view' data*/
    eRes = WE_FILE_SEEK(hFileHandle, (WE_LONG)uiPos, WE_FILE_SEEK_SET, &iNewPos);
    eRes = WE_FILE_WRITE(hFileHandle, (WE_VOID *)pucData, iDataLen, &(iResult));           
    if ((iResult < (WE_INT32)iDataLen) ||(eRes != E_WE_OK))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    }
    /*write View file length*/
    uiPos = 2 * M_FILE_LENGTH_BITS;
    eRes = WE_FILE_SEEK(hFileHandle, (WE_LONG)uiPos, WE_FILE_SEEK_SET, &iNewPos); 
    eRes = WE_FILE_WRITE(hFileHandle, &iDataLen, M_FILE_LENGTH_BITS, &iResult);
    if ((iResult < M_FILE_LENGTH_BITS) ||(eRes != E_WE_OK))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    } 
    eRes = WE_FILE_CLOSE(hFileHandle); 
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimReadView
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        If find a session file, take out 'View' from it. Then delete the file.          
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]:Global data handle.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimReadView(WE_HANDLE hSecHandle)
{
    WE_UCHAR          *pucData = NULL; 
    WE_UINT32         uiDataLen = 0;
    WE_CHAR           *pcFileName = M_SEC_SH_NAME;
    WE_HANDLE         hFileHandle = NULL;
    WE_INT32          iNewPos = 0; 
    WE_INT32          iRes = 0;   
    WE_INT32          iResult = 0;
    E_WE_ERROR        eRes = E_WE_OK;
    WE_UINT32         uiSessionLen =0;
    WE_UINT32         uiPeerLen = 0;
    WE_UINT32         uiViewLen = 0;
    WE_UINT32         uiPos = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    /* for ISigntext, not read view. */
    if (((WE_INT32)E_SEC_SIGNTEXT == SEC_CALLER_TYPE) || 
        ((M_SEC_HAVEREAD_VIEWINFO & *(M_SEC_WIM_HAVELOADUSERPRIV)) == M_SEC_HAVEREAD_VIEWINFO))
    {
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_OK, NULL, 0);
        return;
    }
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_WIM_NOT_INITIALISED, NULL, 0);
        return;
    } 
    
    iResult = Sec_WimCheckFileExist(hSecHandle, pcFileName);
    if(iResult != M_SEC_ERR_OK)
    {  
        /*the session file can not exist when wim run first time.*/
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_OK, NULL, 0);        
        return;
    }
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, pcFileName, WE_FILE_SET_RDWR, &(hFileHandle));
    if (eRes != E_WE_OK) 
    {
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    }     
    /*get session and peer data length*/
    iRes = Sec_WimGetSessionLength(hSecHandle, hFileHandle, &uiSessionLen,
                                &uiPeerLen, &uiViewLen, &iResult);
    if(iRes != M_SEC_ERR_OK)
    {
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    }
    
    if (0 == uiViewLen) /* length of view is zero.*/
    {
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_OK, NULL, 0);
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    }    
    pucData = (WE_UCHAR *)WE_MALLOC((uiViewLen) * sizeof(WE_UCHAR));
    if (NULL == pucData)
    {
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;
    }
    uiPos = 3*M_FILE_LENGTH_BITS + uiSessionLen + uiPeerLen;
    eRes = WE_FILE_SEEK(hFileHandle, (WE_LONG)uiPos , WE_FILE_SEEK_SET, &iNewPos); 
    /* read out 'View'. */            
    eRes = WE_FILE_READ(hFileHandle, pucData, (WE_LONG)uiViewLen, &(iResult));
    if(eRes != E_WE_OK || iResult < (WE_INT32)uiViewLen)
    {
        Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
        WE_FREE(pucData);
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE, pcFileName);
        return;        
    }
    eRes = WE_FILE_CLOSE(hFileHandle);
    uiDataLen = uiViewLen;
    Sec_WimReadViewResponse(hSecHandle, M_SEC_ERR_OK, pucData, (WE_INT32)uiDataLen);
    WE_FREE(pucData);
    /*added by Bird 070210*/
    *(M_SEC_WIM_HAVELOADUSERPRIV) |= M_SEC_HAVEREAD_VIEWINFO;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimFreePubKey
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Free public key.
ARGUMENTS PASSED:
        stKey[IN]: the public key.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_VOID Sec_WimFreePubKey(St_SecRsaPublicKey stKey)
{
    if (stKey.pucExponent != NULL)
    {
        WE_FREE(stKey.pucExponent);
    }
    if (stKey.pucModulus != NULL)
    {
        WE_FREE(stKey.pucModulus);
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetNumUserAndPubKeyCerts
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        get the number of user certificate and public key certificate.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimGetNumUserAndPubKeyCerts(WE_HANDLE hSecHandle)
{
    WE_INT32 iLoop = 0;
    WE_INT32 iNbrOfCerts = 0;
    
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    for (iLoop=0;iLoop<M_SEC_USER_CERT_MAX_SIZE;iLoop++)
    {
        if(SEC_USER_CERT_KEYPAIRS[iLoop].iUcertId != 0)
        {
            iNbrOfCerts++;
        }
    }
    return iNbrOfCerts;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimUserCertReq
CREATE DATE: 
        2006-11-8
AUTHOR: 
        Stone An
DESCRIPTION:
        get the request of user certificate.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        ucExtKeyPurpose[IN]: anticipant usage of the user certificate.
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
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
#define   M_SEC_COMPUTE_TAG(x, y) {(x) = 2; if ((y) > 127) { (x) = 3;} if ((y) > 255) { (x) = 4;}}

WE_VOID Sec_iWimUserCertReq(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
            WE_UCHAR ucExtKeyPurpose, WE_UCHAR *pucCountry,WE_UINT16 usCountryLen, 
            WE_UCHAR *pucProvince, WE_UINT16 usProvinceLen, WE_UCHAR *pucCity, 
            WE_UINT16 usCityLen, WE_UCHAR *pucCompany, WE_UINT16 usCompanyLen, 
            WE_UCHAR *pucDepartment, WE_UINT16 usDepartmentLen,WE_UCHAR *pucName, 
            WE_UINT16 usNameLen,WE_UCHAR *pucEMail, WE_UINT16 usEMailLen, 
            WE_UCHAR ucKeyUsage, WE_UCHAR ucSignType)
{
    St_SecCrptPrivKey   stKey = {0};
    St_SecUserCertReq   *pstP = NULL;
    WE_UCHAR*           pucCertReqInfo = NULL;
    WE_UINT32           uiCertReqInfoLen = 0; 
    WE_UCHAR            *pucPrivKey = NULL;
    WE_INT32            iPrivKeyLen = 0;
    WE_INT32            iKeyLen = 0;
    WE_CHAR*            pcFileName = NULL;
    WE_INT32            iResult = 0;   
    E_SecRsaSigType     eType = E_SEC_RSA_PKCS1_NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED, NULL, 0);
        return;
    } 

    switch(ucSignType)
    {
        case M_SEC_SIG_RSA_SHA:
            eType = E_SEC_RSA_PKCS1_SHA1;
            break;
        case M_SEC_SIG_RSA_MD5:
            eType = E_SEC_RSA_PKCS1_MD5;
            break;
        case M_SEC_SIG_RSA_MD2:
            eType = E_SEC_RSA_PKCS1_MD2;
            break;
        default:
            {
                Sec_WimUserCertReqResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
                return; 
            }            
    }
    
    if (M_SEC_PRIVATE_AUTHKEY == ucKeyUsage)
    {
        pcFileName = M_SEC_AUTH_PRIVKEY_NAME;
    }    
    else if (M_SEC_PRIVATE_NONREPKEY == ucKeyUsage)
    {
        pcFileName = M_SEC_NON_REP_PRIVKEY_NAME;
    }
    else 
    {
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
        return;
    }      
    
    iResult = Sec_WimCheckFileExist(hSecHandle, pcFileName);
    if(iResult != M_SEC_ERR_OK)      /*private key don't exist*/
    {
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
        return ;
    } 

    iResult = Sec_WimGetPrivateKey(hSecHandle, ucKeyUsage, &pucPrivKey, &iPrivKeyLen); 
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
        return; 
    }    
    iKeyLen = Sec_WimGetOrigiLen(pucPrivKey, iPrivKeyLen);                               
    stKey.pucBuf = pucPrivKey;
    stKey.usBufLen = (WE_UINT16)iKeyLen;
    /* build the request info of user cert. */
    iResult = Sec_WimBuildCertReqInfo(hSecHandle, ucExtKeyPurpose, pucCountry,
                                usCountryLen, pucProvince, usProvinceLen, pucCity,
                                usCityLen, pucCompany, usCompanyLen, pucDepartment, 
                                usDepartmentLen, pucName, usNameLen, pucEMail,
                                usEMailLen, ucKeyUsage,
                                &pucCertReqInfo, &uiCertReqInfoLen);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucPrivKey);
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, iResult, NULL, 0);
        return; 
    }
    /* compute signature using request info. */
    iResult = Sec_PKCRsaComputeSignature(hSecHandle, iTargetID, stKey, 
                                eType, pucCertReqInfo, (WE_INT32)uiCertReqInfoLen);  
    WE_FREE(pucPrivKey);
    if (iResult != M_SEC_ERR_OK)
    {        
        WE_FREE(pucCertReqInfo);
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, iResult, NULL, 0);
        return; 
    }
     
    pstP = (St_SecUserCertReq *)WE_MALLOC(sizeof(St_SecUserCertReq));
    if (NULL == pstP)
    {
        WE_FREE(pucCertReqInfo);
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
        return;
    }
    pstP->pucCertReqInfo = pucCertReqInfo;
    pstP->uiCertReqInfoLen = uiCertReqInfoLen;
    pstP->eType = eType;
    
    iResult = Sec_WimAddNewElement(hSecHandle, iTargetID, M_SEC_WIM_USER_CERT_REQ_COMPUTE_SIGNATURE, pstP);
    if(iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pstP);
        WE_FREE(pucCertReqInfo);
        Sec_WimUserCertReqResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
        return;
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_WimAddDerTag
CREATE DATE: 
        2006-11-8
AUTHOR: 
        Stone An
DESCRIPTION:
        insert the length of DER code in info accordding to tag, then move the pointer to buffer.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        uiTag[IN]: Tag of DER code.
        uiDerLen[IN]: length of DER code.
        ppucInfo[OUT]: the infomation which is added DerLen.
        ucTagType[IN]: type of tag.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_VOID Sec_WimAddDerTag(WE_UINT32 uiTag,
                            WE_UINT32 uiDerLen, WE_UCHAR** ppucInfo, WE_UCHAR ucTagType)
{
    WE_UCHAR    *pucInfo = *ppucInfo;
    WE_UINT16   usU = 0x0102;
    WE_UCHAR    ucBigEndian = 0; 

    *pucInfo++ = ucTagType;
    /* Check how bytes are positioned on the current platform */
    if (0x01 == *((WE_UINT8*)&(usU)))
    {
        ucBigEndian = 1;
    }
    else
    {
        ucBigEndian = 0;
    }
    
    if (2 == uiTag)
    {
        if (ucBigEndian)
        {
            *pucInfo++ = *(((WE_UCHAR*)&(uiDerLen))+1); 
        }
        else
        {
            *pucInfo++ = *((WE_UCHAR*)&(uiDerLen));
        }
    }
    else if (3 == uiTag)
    {
        *pucInfo++ = (WE_UCHAR)0x81; 
        if (ucBigEndian)
        {
            *pucInfo++ = *(((WE_UCHAR*)&(uiDerLen))+1); 
        }
        else
        {
            *pucInfo++ = *((WE_UCHAR*)&(uiDerLen));
        }
    }
    else
    {
        *pucInfo++ = (WE_UCHAR)0x82;
        if (ucBigEndian)
        {
            *pucInfo++ = *((WE_UCHAR*)&(uiDerLen));
            *pucInfo++ = *(((WE_UCHAR*)&(uiDerLen))+1); 
        }
        else
        {
            *pucInfo++ = *(((WE_UCHAR*)&(uiDerLen))+1); 
            *pucInfo++ = *((WE_UCHAR*)&(uiDerLen));
        }
    } 

    *ppucInfo = pucInfo;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimAddNameAttr
CREATE DATE: 
        2006-11-8
AUTHOR: 
        Stone An
DESCRIPTION:
        insert the length of attribute DER code in info accordding to tag of attribute.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        pucAttrValue[IN]: string of attribute value.
        usAttrValueLen[IN]: length of attribute value.
        pucAttrTypeDer[IN]: string of attribute type.
        usAttrTypeDERLen[IN]: length of attribute type.
        ucAttrValueType[IN]: Type of attribute value.
        ppucInfo[OUT]: the infomation which is added DerLen.
RETURN VALUE:
        M_SEC_ERR_OK : SUCCESS.
        OTHER : FAIL.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimAddNameAttr(
                    WE_UCHAR *pucAttrValue, WE_UINT16 usAttrValueLen, 
                    WE_UCHAR *pucAttrTypeDer, WE_UINT16 usAttrTypeDERLen, 
                    WE_UCHAR** ppucInfo, WE_UCHAR ucAttrValueType)
{
    WE_UINT16  usSeqLen = 0;
    WE_UINT16  usSetLen = 0;
    WE_UINT16  usAttrValueTag = 0;
    WE_UINT16  usSequenceTag = 0;
    WE_UINT16  usSetTag = 0;
    WE_UCHAR*  pucInfo = NULL;

    if (((usAttrValueLen != 0) && (NULL == pucAttrValue))
        || (NULL == pucAttrTypeDer) || (NULL == ppucInfo))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucInfo = *ppucInfo;    
    if (usAttrValueLen != 0)
    {         
        M_SEC_COMPUTE_TAG(usAttrValueTag, usAttrValueLen);
        
        usSeqLen = usAttrValueLen + usAttrValueTag + usAttrTypeDERLen;
        M_SEC_COMPUTE_TAG(usSequenceTag, usSeqLen);
        
        usSetLen = usSeqLen + usSequenceTag;
        M_SEC_COMPUTE_TAG(usSetTag, usSetLen);
        
        Sec_WimAddDerTag(usSetTag, (WE_UINT32)usSetLen, &pucInfo, WE_ASN1_SET_TYPE);
        Sec_WimAddDerTag(usSequenceTag, (WE_UINT32)usSeqLen, 
                         &pucInfo, WE_ASN1_SEQUENCE_TYPE);
        /* add attribute type. */
        (WE_VOID)WE_MEMCPY(pucInfo, pucAttrTypeDer, usAttrTypeDERLen);
        pucInfo += usAttrTypeDERLen;
        /* add attribute value. */
        Sec_WimAddDerTag(usAttrValueTag, (WE_UINT32)usAttrValueLen, &pucInfo, ucAttrValueType);
        (WE_VOID)WE_MEMCPY(pucInfo, pucAttrValue, usAttrValueLen);
        pucInfo += usAttrValueLen;        
    }

    *ppucInfo = pucInfo;
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimCountNameAttrDERLen
CREATE DATE: 
        2006-11-8
AUTHOR: 
        Stone An
DESCRIPTION:
        count the length of attribute DER code.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        usAttrValueLen[IN]: length of attribute value.
        usAttrTypeDERLen[IN]: length of attribute type in Der.
RETURN VALUE:
        M_SEC_ERR_OK : success.
        other : fail.  
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_UINT32 Sec_WimCountNameAttrDERLen(WE_UINT16 usAttrValueLen, WE_UINT16 usAttrTypeDERLen)
{
    WE_UINT16 usCount = 0;
    WE_UINT16 usTmp = 0;
    
    if (usAttrValueLen != 0)  
    {
        usCount += usAttrTypeDERLen;
        
        M_SEC_COMPUTE_TAG(usTmp, usAttrValueLen)
        usCount += usTmp + usAttrValueLen;        

        M_SEC_COMPUTE_TAG(usTmp, usCount)  /* sequence */
        usCount += usTmp;

        M_SEC_COMPUTE_TAG(usTmp, usCount)  /* set */
        usCount += usTmp;
    }

    return (WE_UINT32)usCount;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimBuildCertReqInfo
CREATE DATE: 
        2006-11-8
AUTHOR: 
        Stone An
DESCRIPTION:
        build the request info about user certificate.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        ucExtKeyPurpose[IN]: anticipant usage of the user certificate.
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
        ppucInfo[OUT]: pointer to the pointer of request info.
        puiInfoLen[OUT]: pointer to the length of request info.
RETURN VALUE:
        M_SEC_ERR_OK : success.
        other : fail.  
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimBuildCertReqInfo(WE_HANDLE hSecHandle,
            WE_UCHAR ucExtKeyPurpose, WE_UCHAR *pucCountry,WE_UINT16 usCountryLen, 
            WE_UCHAR *pucProvince, WE_UINT16 usProvinceLen, WE_UCHAR *pucCity, 
            WE_UINT16 usCityLen, WE_UCHAR *pucCompany, WE_UINT16 usCompanyLen, 
            WE_UCHAR *pucDepartment, WE_UINT16 usDepartmentLen,WE_UCHAR *pucName, 
            WE_UINT16 usNameLen,WE_UCHAR *pucEMail, WE_UINT16 usEMailLen,
            WE_UCHAR ucKeyUsage, WE_UCHAR** ppucInfo, WE_UINT32* puiInfoLen)
{
    
    WE_UCHAR            *pucInfo = NULL;
    St_SecPubKeyRsa     stPubRsaKey = {0}; 
    WE_UINT16           usCertLen = 0;
    WE_UCHAR            *pucCert = NULL;
    St_SecCertificate   stCert = {0};
    WE_INT32            iIndexJ = 0;
    WE_INT32            iResult = 0;
    WE_INT32            iFound = 0;  
    /* version part. */
    WE_UCHAR            aucVersion[] = {0x02, 0x01, 0x00};    
    WE_UINT32           uiVersionLen = 0;
    /* subject part. */
    WE_UINT32           uiSubjectLen = 0;
    WE_UINT32           uiAttrTag = 0; 
    WE_UINT32           uiAttrDerLen = 0; 
    WE_UCHAR            aucCountryType[] = {0x06, 0x03, 0x55, 0x04, 0x06};
    WE_UCHAR            aucProvinceType[] = {0x06, 0x03, 0x55, 0x04, 0x08};
    WE_UCHAR            aucCityType[] = {0x06, 0x03, 0x55, 0x04, 0x07};
    WE_UCHAR            aucCompanyType[] = {0x06, 0x03, 0x55, 0x04, 0x0A};
    WE_UCHAR            aucDepartmentType[] = {0x06, 0x03, 0x55, 0x04, 0x0B};
    WE_UCHAR            aucNameType[] = {0x06, 0x03, 0x55, 0x04, 0x03};
    WE_UCHAR            aucEMailType[] = {0x06, 0x09, 
                                 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x01};
    /* RSA algorithm and public key. */
    WE_UINT32           uiPublicKeyInfoLen = 0;
    WE_UINT32           uiSubjectPubKeyInfoTag = 0;
    WE_UINT32           uiSubjectPubKeyInfoDerLen = 0;
    WE_UCHAR            aucRSAPublicKeyAlg[] = {0x30, 0x0d, 
                               0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 
                               0x05, 0x00};  
    WE_UINT32           uiPubKeyTag = 0; 
    WE_UINT32           uiKeyDerLen = 0;     
    WE_UCHAR*           pucPubKey = NULL;
    /* attribute part. */
    /* add for future
    WE_UCHAR            aucKeyUsageType[] = {0x06, 0x03, 0x55, 0x1D, 0x0F};
    WE_UCHAR            aucKeyUsage[] = {0x04, 0x04, 0x03, 0x02, 0x04, 0xF0};
    */
    WE_UINT32           uiSubExtKeyPurposeSeqTag = 0;
    WE_UINT32           uiSubExtKeyPurposeDerLen = 0;
    WE_UINT32           uiExtKeyPurposeTag = 0;
    WE_UINT32           uiKeyUsageSeqTag = 0;
    WE_UINT32           uiKeyUsageSeqDerLen = 0;
    WE_UINT32           uiOneOIDSeqTag = 0;
    WE_UINT32           uiOneOIDSeqLen = 0;
    WE_UINT32           uiOneOIDSetTag = 0;
    WE_UINT32           uiOneOIDSetLen = 0;
    WE_UINT32           uiOneOIDAllDerTag = 0;
    WE_UINT32           uiOneOIDAllDerLen = 0;
    WE_UINT32           uiAllAttributeTag = 0;
    WE_UINT32           uiAllAttributeLen = 0;
    WE_UINT32           uiAttributeLen = 0;
    WE_UINT32           uiExtKeyPurposeDerLen = 0;
    WE_UINT32           uiExtKeyPurposeOctStringTag = 0;
    WE_UINT32           uiExtKeyUsageOctStringDerLen = 0;
    WE_UCHAR            aucExtKeyPurposeType[] = {0x06, 0x03, 0x55, 0x1d, 0x25};
    WE_UINT8            aucOIDIdKp[9] = {0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03};
    WE_UINT8            aucKeyPurpose[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    WE_UINT8            aucKeyPurposeType[8] = {M_SEC_CERT_USAGE_AUTH_SERVER, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

    WE_UCHAR            aucOneOIDHeaderDer[] = {0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x0E};
    /* total. */
    WE_UINT32           uiTotalReqTag = 0;
    WE_UINT32           uiTotalReqDerLen = 0;

    /* search */
    for (iIndexJ=0; iIndexJ<M_SEC_USER_CERT_MAX_SIZE; iIndexJ++)
    {
        if ((SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId != 0) && 
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucKeyUsage == ucKeyUsage) && 
            (SEC_USER_CERT_KEYPAIRS[iIndexJ].ucPublicKeyCert == 1))
        {          
            iResult = Sec_WimGetCertByID(hSecHandle, SEC_USER_CERT_KEYPAIRS[iIndexJ].iUcertId, 
                &pucCert, &usCertLen);   
            if (iResult != M_SEC_ERR_OK)
            {
                return iResult;
            }
            /* get public key */
            iResult = Sec_WtlsCertParse(hSecHandle, pucCert, &usCertLen, &stCert); 
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucCert);            
                return M_SEC_ERR_BAD_CERTIFICATE;
            }
            stPubRsaKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
            stPubRsaKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
            stPubRsaKey.usModLen = stCert.stCertUnion.stWtls.usModLen;
            stPubRsaKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
            /* convert public key to DER code.*/
            iResult = Sec_PKCConvertRSAPubKey(stPubRsaKey, &pucPubKey, (WE_INT32 *)(&uiKeyDerLen)); 
            WE_FREE(pucCert);
            if (iResult != M_SEC_ERR_OK)
            {
                return iResult;
            }  
            iFound = 1; /* find out the public key used to register user certificate.*/
            break;
        }
    }

    if (1 != iFound)
    {
        return M_SEC_ERR_MISSING_KEY;
    }

    /* version part */
    uiVersionLen = sizeof(aucVersion);

    /* subject part */ 
    uiAttrDerLen += Sec_WimCountNameAttrDERLen(usCountryLen, sizeof(aucCountryType));
    uiAttrDerLen += Sec_WimCountNameAttrDERLen(usProvinceLen, sizeof(aucProvinceType));
    uiAttrDerLen += Sec_WimCountNameAttrDERLen(usCityLen, sizeof(aucCityType));
    uiAttrDerLen += Sec_WimCountNameAttrDERLen(usCompanyLen, sizeof(aucCompanyType));
    uiAttrDerLen += Sec_WimCountNameAttrDERLen(usDepartmentLen, sizeof(aucDepartmentType));
    uiAttrDerLen += Sec_WimCountNameAttrDERLen(usNameLen, sizeof(aucNameType));
    uiAttrDerLen += Sec_WimCountNameAttrDERLen(usEMailLen, sizeof(aucEMailType));      
    M_SEC_COMPUTE_TAG(uiAttrTag, uiAttrDerLen);  /* sequence */
    uiSubjectLen = uiAttrTag + uiAttrDerLen;

    /* public key info part */
    uiPublicKeyInfoLen = 0;
    /* SubjectPublicKeyInfo ::= SEQUENCE{ algorithm AlgorithmIdentifier, subjectPublicKey BIT STRING} */
    M_SEC_COMPUTE_TAG(uiPubKeyTag, (uiKeyDerLen + 1));   /* add 1, for bit string patching byte. */
    uiSubjectPubKeyInfoDerLen = sizeof(aucRSAPublicKeyAlg) + uiPubKeyTag + uiKeyDerLen + 1;
    M_SEC_COMPUTE_TAG(uiSubjectPubKeyInfoTag, uiSubjectPubKeyInfoDerLen);  /* sequence */
    uiPublicKeyInfoLen = uiSubjectPubKeyInfoTag + uiSubjectPubKeyInfoDerLen;

    /* attribute part */
    uiAttributeLen = 0;    
    /* OID: 06 0A 2B 06 01 04 01 82 37 02 01 0E */
    /* sub-part I */
    for (iIndexJ=0; iIndexJ<8; iIndexJ++)
    {
        if ((ucExtKeyPurpose & aucKeyPurposeType[iIndexJ]) == aucKeyPurposeType[iIndexJ])
        {
            uiSubExtKeyPurposeDerLen += sizeof(aucOIDIdKp) + 1; /* add extension key usage length.*/
        }
    }
    M_SEC_COMPUTE_TAG(uiSubExtKeyPurposeSeqTag, uiSubExtKeyPurposeDerLen);  /* sequence */
    uiExtKeyUsageOctStringDerLen = uiSubExtKeyPurposeSeqTag + uiSubExtKeyPurposeDerLen;     
    M_SEC_COMPUTE_TAG(uiExtKeyPurposeOctStringTag, uiExtKeyUsageOctStringDerLen); /* octer string */
    uiExtKeyPurposeDerLen = sizeof(aucExtKeyPurposeType);
    uiExtKeyPurposeDerLen += uiExtKeyPurposeOctStringTag + uiExtKeyUsageOctStringDerLen;    
    M_SEC_COMPUTE_TAG(uiExtKeyPurposeTag, uiExtKeyPurposeDerLen);

    /* sub-part II */
    /* prepared for future.
    uiKeyUsageSeqDerLen = sizeof(aucKeyUsageType); 
    uiKeyUsageSeqDerLen += sizeof(aucKeyUsage); 
    M_SEC_COMPUTE_TAG(uiKeyUsageSeqTag, uiKeyUsageSeqDerLen);  *//* sequence */

    /* all sub-part */
    uiOneOIDSeqLen = uiExtKeyPurposeTag + uiExtKeyPurposeDerLen + uiKeyUsageSeqTag + uiKeyUsageSeqDerLen;
    M_SEC_COMPUTE_TAG(uiOneOIDSeqTag, uiOneOIDSeqLen);  /* sequence */
    uiOneOIDSetLen = uiOneOIDSeqTag + uiOneOIDSeqLen;
    M_SEC_COMPUTE_TAG(uiOneOIDSetTag, uiOneOIDSetLen);  /* set */

    uiOneOIDAllDerLen = sizeof(aucOneOIDHeaderDer); 
    uiOneOIDAllDerLen += uiOneOIDSetTag + uiOneOIDSetLen;
    M_SEC_COMPUTE_TAG(uiOneOIDAllDerTag, uiOneOIDAllDerLen);  /* sequence */

    /* other OID */
    /* none. */

    /* all attribute */
    uiAllAttributeLen = uiOneOIDAllDerTag + uiOneOIDAllDerLen;
    M_SEC_COMPUTE_TAG(uiAllAttributeTag, uiAllAttributeLen);   /* WE_ASN1_NEW_TAG1 0xA0 */
    uiAttributeLen = uiAllAttributeTag + uiAllAttributeLen;

    /* total request info */
    uiTotalReqDerLen = uiVersionLen + uiSubjectLen + uiPublicKeyInfoLen + uiAttributeLen;    
    M_SEC_COMPUTE_TAG(uiTotalReqTag, uiTotalReqDerLen); /* sequence */

    /* malloc memory. */
    pucInfo = (WE_UCHAR*)WE_MALLOC((uiTotalReqDerLen + uiTotalReqTag) * sizeof(WE_UCHAR));
    if (NULL == pucInfo)
    {
        WE_FREE(pucPubKey);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    *puiInfoLen = uiTotalReqDerLen + uiTotalReqTag;
    *ppucInfo = pucInfo;

    /* set content in memory. */
    
    /* 1. set all length */
    Sec_WimAddDerTag(uiTotalReqTag, uiTotalReqDerLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);
    
    /* 2. set version */
    (WE_VOID)WE_MEMCPY(pucInfo, aucVersion, uiVersionLen);
    pucInfo += uiVersionLen;

    /* 3. set subject part. */
    Sec_WimAddDerTag(uiAttrTag, uiAttrDerLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);    
    iResult = Sec_WimAddNameAttr(pucCountry, usCountryLen, aucCountryType, 
                                sizeof(aucCountryType), &pucInfo, WE_ASN1_PRINTABLESTRING);
    iResult += Sec_WimAddNameAttr(pucProvince, usProvinceLen, aucProvinceType, 
                                sizeof(aucProvinceType), &pucInfo, WE_ASN1_PRINTABLESTRING);
    iResult += Sec_WimAddNameAttr(pucCity, usCityLen, aucCityType, 
                                sizeof(aucCityType), &pucInfo, WE_ASN1_PRINTABLESTRING);
    iResult += Sec_WimAddNameAttr(pucCompany, usCompanyLen, aucCompanyType,
                                sizeof(aucCompanyType), &pucInfo, WE_ASN1_PRINTABLESTRING);
    iResult += Sec_WimAddNameAttr(pucDepartment, usDepartmentLen, aucDepartmentType, 
                                sizeof(aucDepartmentType), &pucInfo, WE_ASN1_PRINTABLESTRING);
    iResult += Sec_WimAddNameAttr(pucName, usNameLen, aucNameType,
                                sizeof(aucNameType), &pucInfo, WE_ASN1_PRINTABLESTRING);
    iResult += Sec_WimAddNameAttr(pucEMail, usEMailLen, aucEMailType,
                                sizeof(aucEMailType), &pucInfo, WE_ASN1_IA5STRING);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucPubKey);
        WE_FREE(*ppucInfo);
        *ppucInfo = NULL;
        *puiInfoLen = 0;
        return iResult;
    }
    
    /* 4. set public key info part */
    Sec_WimAddDerTag(uiSubjectPubKeyInfoTag, uiSubjectPubKeyInfoDerLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);    
    (WE_VOID)WE_MEMCPY(pucInfo, aucRSAPublicKeyAlg, sizeof(aucRSAPublicKeyAlg));
    pucInfo += sizeof(aucRSAPublicKeyAlg);
    Sec_WimAddDerTag(uiPubKeyTag, (uiKeyDerLen+1), &pucInfo, WE_ASN1_BITSTRING);
    *pucInfo++ = (WE_UCHAR)0x00;
    (WE_VOID)WE_MEMCPY(pucInfo, pucPubKey, uiKeyDerLen);
    pucInfo += uiKeyDerLen;

    /* 5. attribute part. */
    Sec_WimAddDerTag(uiAllAttributeTag, uiAllAttributeLen, &pucInfo, WE_ASN1_NEW_TAG1);  
    /* OID: 06 0A 2B 06 01 04 01 82 37 02 01 0E  */
    Sec_WimAddDerTag(uiOneOIDAllDerTag, uiOneOIDAllDerLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);    
    (WE_VOID)WE_MEMCPY(pucInfo, aucOneOIDHeaderDer, sizeof(aucOneOIDHeaderDer));
    pucInfo += sizeof(aucOneOIDHeaderDer);
    Sec_WimAddDerTag(uiOneOIDSetTag, uiOneOIDSetLen, &pucInfo, WE_ASN1_SET_TYPE);     
    Sec_WimAddDerTag(uiOneOIDSeqTag, uiOneOIDSeqLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);
    /* subset II */
    /* prepared for future.
    Sec_WimAddDerTag(uiKeyUsageSeqTag, uiKeyUsageSeqDerLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);   
    (WE_VOID)WE_MEMCPY(pucInfo, aucKeyUsageType, sizeof(aucKeyUsageType));
    pucInfo += sizeof(aucKeyUsageType);
    (WE_VOID)WE_MEMCPY(pucInfo, aucKeyUsage, sizeof(aucKeyUsage));
    pucInfo += sizeof(aucKeyUsage);
    */

    /* subset I */
    Sec_WimAddDerTag(uiExtKeyPurposeTag, uiExtKeyPurposeDerLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);
    (WE_VOID)WE_MEMCPY(pucInfo, aucExtKeyPurposeType, sizeof(aucExtKeyPurposeType));
    pucInfo += sizeof(aucExtKeyPurposeType);
    Sec_WimAddDerTag(uiExtKeyPurposeOctStringTag, uiExtKeyUsageOctStringDerLen, &pucInfo, WE_ASN1_OCTETSTRING);
    Sec_WimAddDerTag(uiSubExtKeyPurposeSeqTag, uiSubExtKeyPurposeDerLen, &pucInfo, WE_ASN1_SEQUENCE_TYPE);
    for (iIndexJ=0; iIndexJ<8; iIndexJ++)
    {
        if ((ucExtKeyPurpose & aucKeyPurposeType[iIndexJ]) == aucKeyPurposeType[iIndexJ])
        {
            (WE_VOID)WE_MEMCPY(pucInfo, aucOIDIdKp, sizeof(aucOIDIdKp));
            pucInfo += sizeof(aucOIDIdKp);
            *pucInfo++ = aucKeyPurpose[iIndexJ];
        }
    }

    WE_FREE(pucPubKey);
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
        Sec_iWimUserCertReqResp
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Response of get request of user certificate.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN] : Global queue.
        pstParam[IN] : The parameter value.  
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimUserCertReqResp(WE_HANDLE hSecHandle, 
                 St_SecWimElement *pstElement, St_SecCrptComputeSignatureResp *pstParam)        
{
    St_SecUserCertReq   *pstP = NULL;    
    WE_UCHAR            *pucCertReqMsg = NULL; 
    WE_UCHAR            *pucEncodeCertReqMsg = NULL;
    WE_UINT32           uiCertReqMsgLen = 0; 
    WE_INT32            iResult = 0; 
    WE_UINT32           uiTmpLen = 0;

    if ((NULL == hSecHandle) || (NULL == pstElement))
    {
        return;
    }
    
    pstP = pstElement->pvPointer;
    if(NULL == pstParam)
    {
        Sec_WimUserCertReqResponse(hSecHandle, pstElement->iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY,
                                                    NULL, 0);
        WE_FREE(pstP->pucCertReqInfo);
        WE_FREE(pstP);
        WE_FREE(pstElement);
        return;
        
    }
    iResult = pstParam->sResult;
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimUserCertReqResponse(hSecHandle, pstElement->iTargetID, iResult, NULL, 0);
        WE_FREE(pstP->pucCertReqInfo);
        WE_FREE(pstP);
        WE_FREE(pstElement);
        return;
    } 

    /* Algorithm is RSA-SHA1.*/
    iResult = Sec_WimComposeUserCertReqMsg(hSecHandle, pstP->pucCertReqInfo, 
            pstP->uiCertReqInfoLen, pstP->eType, pstParam->pucSig, (WE_UINT16)(pstParam->sSigLen),
            &pucCertReqMsg, &uiCertReqMsgLen);
    WE_FREE(pstP->pucCertReqInfo);
    WE_FREE(pstP);
    if (iResult != M_SEC_ERR_OK)
    {
        Sec_WimUserCertReqResponse(hSecHandle, pstElement->iTargetID, iResult, NULL, 0); 
        WE_FREE(pstElement);
        WE_FREE(pucCertReqMsg);
        return;
    } 
    
    uiTmpLen = uiCertReqMsgLen;
    iResult = Base64_Encode(&pucEncodeCertReqMsg, pucCertReqMsg, &uiTmpLen);
    WE_FREE(pucCertReqMsg);
    if (iResult != M_SEC_ERR_OK)
    {
        if (pucEncodeCertReqMsg != NULL)
        {
            WE_FREE(pucEncodeCertReqMsg);
            pucEncodeCertReqMsg = NULL;
        }
        Sec_WimUserCertReqResponse(hSecHandle, pstElement->iTargetID, iResult, NULL, 0);   
        WE_FREE(pstElement);
        return;
    }
    
    Sec_WimUserCertReqResponse(hSecHandle, pstElement->iTargetID, 
                            iResult, pucEncodeCertReqMsg, uiTmpLen);
    WE_FREE(pucEncodeCertReqMsg);
    WE_FREE(pstElement);
}

/*=====================================================================================
FUNCTION: 
        Sec_WimComposeUserCertReqMsg
CREATE DATE: 
        2006-11-8
AUTHOR: 
        Stone An
DESCRIPTION:
        build the request message about user certificate using request info and signature.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iTargetID[IN]: ID of the object.
        pucCertReqInfo[IN]: string of request info.
        uiCertReqInfoLen[IN]: length of request info.
        eType[IN]: type of sign.
        pucSig[IN]: string of signature. 
        usSigLen[IN]: length of signature.
        ppucCertReqMsg[OUT]: pointer to the pointer of request message.
        puiCertReqMsgLen[OUT]: pointer to the length of request message.
RETURN VALUE:
        M_SEC_ERR_OK : success.
        other : fail.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
static WE_INT32 Sec_WimComposeUserCertReqMsg(WE_HANDLE hSecHandle,
                                   WE_UCHAR *pucCertReqInfo, WE_UINT32 uiCertReqInfoLen, 
                                   E_SecRsaSigType eType, WE_UCHAR *pucSig, WE_UINT16 usSigLen,
                                   WE_UCHAR **ppucCertReqMsg, WE_UINT32 *puiCertReqMsgLen)
{ 
    WE_UCHAR*   pucMsg = NULL;
    WE_UCHAR    aucRSASHA1SigAlg[] = {0x30, 0x0d, 
        0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05,  
        0x05, 0x00};
    WE_UCHAR    aucRSAMD5SigAlg[] = {0x30, 0x0d, 
        0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x04,  
        0x05, 0x00};
    WE_UCHAR    aucRSAMD2SigAlg[] = {0x30, 0x0d, 
        0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x02,  
        0x05, 0x00};
    WE_UINT16   usSigTag = 0;
    WE_UINT32   uiMsgTag = 0; 
    WE_UINT32   uiMsgLen = 0;
    WE_UCHAR    *pucSignAlg = NULL;
    WE_UINT32   uiSignAlgLen = 0;

    if (NULL==hSecHandle || !pucCertReqInfo || !pucSig || !ppucCertReqMsg || !puiCertReqMsgLen)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    switch(eType)
    {
        case E_SEC_RSA_PKCS1_MD2:
            pucSignAlg = aucRSAMD2SigAlg;
            uiSignAlgLen = sizeof(aucRSAMD2SigAlg);
            break;
        case E_SEC_RSA_PKCS1_MD5:
            pucSignAlg = aucRSAMD5SigAlg;
            uiSignAlgLen = sizeof(aucRSAMD5SigAlg);
            break;
        case E_SEC_RSA_PKCS1_SHA1:
            pucSignAlg = aucRSASHA1SigAlg;
            uiSignAlgLen = sizeof(aucRSASHA1SigAlg);
            break;
        case E_SEC_RSA_PKCS1_NULL:
        case E_SEC_RSA_PKCS1_PSS_SHA1_MGF1:
        default:
            return M_SEC_ERR_INVALID_PARAMETER;
    }
    /* Because of bit string DER code, so add one byte for the patched number. in this comment, number is zero.*/
    M_SEC_COMPUTE_TAG(usSigTag, (usSigLen+1))
    uiMsgLen = uiCertReqInfoLen + uiSignAlgLen + usSigLen + 1 + usSigTag;
    
    M_SEC_COMPUTE_TAG(uiMsgTag, uiMsgLen)
    pucMsg = (WE_UCHAR *)WE_MALLOC((uiMsgLen + uiMsgTag) * sizeof(WE_UCHAR));
    if (NULL == pucMsg)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    *puiCertReqMsgLen = uiMsgLen + uiMsgTag;
    *ppucCertReqMsg = pucMsg;
    
    Sec_WimAddDerTag(uiMsgTag, uiMsgLen, &pucMsg, WE_ASN1_SEQUENCE_TYPE);     
    (WE_VOID)WE_MEMCPY(pucMsg, pucCertReqInfo, uiCertReqInfoLen);
    pucMsg += uiCertReqInfoLen;
    /* add algorithm in DER. */
    (WE_VOID)WE_MEMCPY(pucMsg, pucSignAlg, uiSignAlgLen);
    pucMsg += uiSignAlgLen;
    /* add signature. */
    Sec_WimAddDerTag(usSigTag, usSigLen+1, &pucMsg, WE_ASN1_BITSTRING);
    *pucMsg++ = 0x00; /* add the patched number. */
    (WE_VOID)WE_MEMCPY(pucMsg, pucSig, usSigLen);
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
        Sec_WimFreeRsaElement
CREATE DATE: 
        2006-11-14
AUTHOR: 
        Bird
DESCRIPTION:
        build the request message about user certificate using request info and signature.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        pstElement[IN]: pointer to the element structure.
RETURN VALUE:
        None
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        Omit.
=====================================================================================*/
void Sec_WimFreeRsaElement(WE_HANDLE hSecHandle, St_SecWimElement* pstElement)
{
    if((NULL == hSecHandle) || (NULL == pstElement))
    {
        return;
    }
    switch(pstElement->iState)
    {
        /*1*/
        case M_SEC_WIM_WTLS_KEYEXCHANGE_RSA:
        {
            St_SecWimWtlsKeyExRsa *pstP = NULL;
            pstP = pstElement->pvPointer;
            WE_FREE(pstP->pucMasterSecret);
            WE_FREE(pstP);
        }
        break;
        /*2*/
        case M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_VERIFY:
        {
            St_SecWimGetMatchedPrivKey *pstP = NULL; 

            pstP = pstElement->pvPointer;

            Sec_WimFreePubKey(pstP->stPubKey);
            WE_FREE (pstP);
        }
        break;
        case M_SEC_WIM_VERIFY_ROOT_CERTIFICATE:
        {
        }
        break;
        case M_SEC_WIM_SIGNATURE_VERIFICATION:
        {
        }
        break;
        case M_SEC_WIM_VERIFY_CERT_CHAIN_INTERMEDIATE:
        case M_SEC_WIM_VERIFY_CERT_CHAIN_ROOT:

        {
            St_SecWimVerifyCertChain    *pstP = NULL;
            WE_INT32 iIndexI = 0;
            pstP = pstElement->pvPointer;
            for (iIndexI=0; iIndexI<pstP->iNumCerts; iIndexI++)
            {
                WE_FREE(pstP->pstCerts[iIndexI].pucCert);
            }
            if (NULL != (pstP->pstCerts))
            {
                WE_FREE(pstP->pstCerts);
            }
            if (NULL != (pstP->pucRootCert)) 
            {
                WE_FREE(pstP->pucRootCert);
            }
            WE_FREE (pstP);
        }
        break;
        case M_SEC_WIM_CHECK_PRESTORED_ROOT_CERT:
        {
            St_SecWimPSRCertInfo *pstP=NULL;
            pstP= pstElement->pvPointer;
            WE_FREE(pstP->pcFileName);
            WE_FREE(pstP);
            
        }
        break;

        /*3*/
        case M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_COMPUTE:
        {
            St_SecWimGetMatchedPrivKey *pstP = NULL;
            pstP = pstElement->pvPointer;
            Sec_WimFreePubKey(pstP->stPubKey);
            WE_FREE (pstP);
        }
        break;
        case M_SEC_WIM_PRODUCE_PUBKEY_CERT:
        {
            St_SecWimKeyPairGen *pstP = NULL;
            pstP = pstElement->pvPointer;
            if (pstP->pcPin != NULL) 
            {
                Sec_WimDelPrivKey(hSecHandle, pstP->ucKeyType);
            }
            if(NULL != pstP->pcPin)
            {
                WE_FREE(pstP->pcPin);
            }
            WE_FREE(pstP->pucPubKey);
            WE_FREE(pstP);        
        }
        break;
        case M_SEC_WIM_SIGNTEXT:
        {
            St_SecWimSigntext  *pstP = NULL;
            pstP = pstElement->pvPointer;
            WE_FREE (pstP->pucCert);
            WE_FREE (pstP->pcHashedKey);
            WE_FREE (pstP);        
        }
        break;
        case M_SEC_WIM_COMPUTE_SIGNATURE:
        {
        }
        break;
        case M_SEC_WIM_GET_USER_CERT_AND_SIGNED_KEY:
        {
            St_SecWimGetUserCertAndPrivKey *pstP = NULL;
            pstP = pstElement->pvPointer;
            WE_FREE(pstP->pucCert);
            WE_FREE(pstP);        
        }
        break;
        case M_SEC_WIM_USER_CERT_REQ_COMPUTE_SIGNATURE:
        {
            St_SecUserCertReq   *pstP = NULL;    
            pstP = pstElement->pvPointer;
            WE_FREE(pstP->pucCertReqInfo);
            WE_FREE(pstP);        
        }
        break;

        /*4*/
        case M_SEC_WIM_GENERATE_KEYPAIR:
        {
            St_SecWimKeyPairGen   *pstP = NULL;
            pstP = pstElement->pvPointer;
           /* if (NULL != pstP->pucPubKey)
            {
                WE_FREE(pstP->pucPubKey);
            }*/
            WE_FREE(pstP->pcPin);
            WE_FREE(pstP);
        }
        break;
        
        default:
            break;
        
    }
    WE_FREE(pstElement);

}


/*=====================================================================================
FUNCTION: 
        Sec_iWimChangeWTLSCertAble
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Change the able attribute of WTLS CA certificate.          
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]:Global data handle.  
        iTargetID[IN]: ID of the object.
        uiCertId[IN]:ID of certificate.
RETURN VALUE:
        none.    
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.    
CALL BY:
        omit.           
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_iWimChangeWTLSCertAble(WE_HANDLE hSecHandle, 
                                    WE_INT32 iTargetID, WE_UINT32 uiCertId)
{  
    WE_UINT32   uiFileId = 0;
    WE_INT32    iResult = 0; 
    E_WE_ERROR  eResult = E_WE_OK; 
    WE_HANDLE   hFileHandle = NULL;
    WE_LONG     lReadNum = 0;
    WE_LONG     lWriteNum = 0;
    WE_UINT8    *pucIdxFileData = NULL;
    WE_UINT8    *pucTmp = NULL;
    WE_INT32    iLoop = 0;
    WE_INT32    iSizeOfIndexFile = 0;
    WE_UINT8    ucCertType = 0;
    WE_INT32    iCACertNum = 0;
    WE_INT32    iNewPos = 0;
    WE_INT32    iBlockLen = 0;
    WE_UINT8    ucState = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    } 
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimChangeWtlsCertAbleResponse(hSecHandle, iTargetID, M_SEC_ERR_WIM_NOT_INITIALISED, 0);
        return;
    } 

    eResult = WE_FILE_GETSIZE(SEC_WE_HANDLE, M_SEC_CA_CERT_INDEXFILE_NAME, &iSizeOfIndexFile);
    if (eResult != E_WE_OK)
    {        
        Sec_WimChangeWtlsCertAbleResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0);
        return;
    }
    eResult = WE_FILE_OPEN(SEC_WE_HANDLE, M_SEC_CA_CERT_INDEXFILE_NAME, WE_FILE_SET_RDWR, &hFileHandle);
    if (eResult != E_WE_OK)
    {        
        Sec_WimChangeWtlsCertAbleResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0);
        return;
    }
    
    pucIdxFileData = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(iSizeOfIndexFile)*sizeof(WE_UINT8));  
    if (NULL == pucIdxFileData)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);      
        Sec_WimChangeWtlsCertAbleResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0);
        return;
    }    
    /*read whole CA IndexFile*/
    eResult = WE_FILE_READ(hFileHandle, pucIdxFileData, iSizeOfIndexFile, &lReadNum);
    if (eResult != E_WE_OK)
    {
        WE_FREE(pucIdxFileData);
        eResult = WE_FILE_CLOSE(hFileHandle);    
        Sec_WimChangeWtlsCertAbleResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0);
        return;
    }
    pucTmp = pucIdxFileData;
    iCACertNum = (WE_INT32)(*pucTmp);
    pucTmp++;
    for(iLoop=0; iLoop<iCACertNum; iLoop++)
    {
        ucCertType = *pucTmp;
        if (M_SEC_WTLS_CA_CERT != ucCertType)
        {            
            iResult = Sec_WimGetBlockLength(pucTmp, (WE_CHAR)ucCertType, &iBlockLen);
            pucTmp += iBlockLen;
            continue;
        }
        Sec_StoreStrUint8to32(pucTmp+1, &uiFileId);   
        if (uiCertId != uiFileId)
        {           
            iResult = Sec_WimGetBlockLength(pucTmp, (WE_CHAR)ucCertType, &iBlockLen);
            pucTmp += iBlockLen;
            continue;
        }
        pucTmp += 1+4+4+M_SEC_DN_HASH_LEN+M_SEC_DN_HASH_LEN+1+2;  
        if (M_SEC_WTLS_CA_CERT_ENABLE == (*pucTmp))
        {
            (*pucTmp) = M_SEC_WTLS_CA_CERT_DISABLE;
        }
        else
        {
            (*pucTmp) = M_SEC_WTLS_CA_CERT_ENABLE;
        }
        ucState = (*pucTmp);
        break;
    }

    eResult = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET, &(iNewPos));
    eResult = WE_FILE_WRITE(hFileHandle, pucIdxFileData, iSizeOfIndexFile, &(lWriteNum));
    WE_FREE(pucIdxFileData);
    if ((eResult!=E_WE_OK) || (lWriteNum<iSizeOfIndexFile))
    {
        eResult = WE_FILE_CLOSE(hFileHandle);     
        Sec_WimChangeWtlsCertAbleResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, ucState);  
        return;
    }  
    eResult = WE_FILE_CLOSE(hFileHandle);  
    Sec_WimChangeWtlsCertAbleResponse(hSecHandle, iTargetID, M_SEC_ERR_OK, ucState);
    iResult = iResult;
}


/*************************************************************************************************
end
*************************************************************************************************/



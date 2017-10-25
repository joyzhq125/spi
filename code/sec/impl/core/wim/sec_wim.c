/*==================================================================================================
    FILE NAME : sec_wim.c
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION:
        The functions in this file process  certificate's storage,encryption
        and decryption..
  
    Techfaith Software Confidential Proprietary(c)
    Copyright 2002-2006 by Techfaith Software. All Rights Reserved.
    ================================================================================================
    Revision History
    
    Modification              Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-12 Tang           None      Init
      
==================================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "sec_comm.h"
#include "oem_sechandle.h"

/***************************************************************************************************
*   Macro Define Section
***************************************************************************************************/
#define SEC_CREATEFILE_ERROR_NUM                10

/*padded value*/
#define SEC_FIRST_PAD_VALUE                     1
#define SEC_SECOND_PAD_VALUE                    0
#define SEC_PIN_PADDING_CHARACTER               0

#define SEC_HMAC_B                              64
#define SEC_HMAC_MD5_L                          16
#define SEC_HMAC_SHA1_L                         20


typedef St_PublicData   St_SecGlobalData;

#define M_SEC_WIM_ASTUSERCERTKEYPAIR      (((ISec*)hSecHandle)->pstUserCertKeyPair)
#define M_SEC_WIM_HAVELOADUSERPRIV        (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->pcHaveLoad)
#define M_SEC_WIM_UCINITIALISED           (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->ucInitialised)
#define M_SEC_WIM_HWEHANDLE               (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->hWeHandle)

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
void Sec_WimPMD5(WE_HANDLE hSecHandle,WE_UCHAR *pucSecret, WE_INT32 iSecretLen, 
                  WE_UCHAR *pucSeed, WE_INT32 iSeedLen, WE_INT32 *iResult, 
                  WE_UCHAR *pucBuf, WE_INT32 iDesiredLen);
WE_INT32 Sec_WimGenCipherKey(WE_UINT8 *pucKey, WE_INT32 iKeyLen ,WE_UINT8 *pucIv ,WE_INT32 iIvLen );
WE_INT32 Sec_WimEncryptData(WE_HANDLE hSecHandle,
                            WE_UINT8 *pucData, 
                            WE_INT32 iDataLen, 
                            WE_UINT8 *pucEncryptedData,
                            WE_INT32 *piEncryptedDataLen);
WE_INT32 Sec_WimDecryptData(WE_HANDLE hSecHandle,
                            WE_UINT8 *pucData, 
                            WE_INT32 iDataLen,
                            WE_UINT8 *pucDecryptedData,
                            WE_INT32 *piDecDataLen);
WE_INT32 Sec_WimStorePubKeyCert(WE_HANDLE hSecHandle,WE_UINT8 *pucCert,
                                WE_UINT16 usCertLen, WE_INT32 *piUserCertId,
                                WE_INT32 *piUserCertCount, const WE_UINT8 *pucPublicKeyHash, 
                                WE_INT32 *piNameCount,WE_UINT8 ucAlg, WE_UINT8 ucKeyType);

void Sec_WimHMAC(WE_HANDLE hSecHandle,WE_UINT8 *pucK, 
                 WE_INT32 iKLen, WE_UINT8 *pcData, 
                 WE_INT32 iDataLen, WE_INT32 *piResult,WE_UINT8 *pucBuf);
void Sec_WimPHash(WE_HANDLE hSecHandle,WE_UINT8 *pucSecret, WE_INT32 iSecretLen, 
                  WE_UINT8 *pucSeed, WE_INT32 iSeedLen, WE_INT32 *piResult, 
                  WE_UINT8 *pucBuf, WE_INT32 iDesiredLen);

static WE_INT32 Sec_WimCheckPreStoredRootCertVerify (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                     WE_UINT8 *pucCert, WE_UINT16 usCertLen, WE_CHAR cCertType);


/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/

/*====================================================================================
FUNCTION:
    Sec_WimMakeFile    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Creat a new file by it's type,and get it's filehandle and FileID.     
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:  Global data handle.
    cFileType[IN]:       the type of file:
                         M_SEC_X509_USER_CERT,M_SEC_WTLS_USER_CERT,M_SEC_PUBKEY_CERT,
                         M_SEC_WTLS_CA_CERT,M_SEC_X509_CA_CERT,M_SEC_CONTRACT
    phFileHandle[OUT]:   file handle      
    puiFileId[OUT]:      FileID     
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimMakeFile(WE_HANDLE hSecHandle,WE_INT8 cFileType,
                         WE_HANDLE *phFileHandle, WE_UINT32 *puiFileId)
{
    WE_INT32   iResult=0;
    E_WE_ERROR eRes = E_WE_OK;
    WE_INT32   iFound = 0;
    WE_INT32   iErrStep=0;
    WE_UINT32  uiTmpFileId=0;
    WE_UINT32  uiFileIdEnd=0;
    WE_CHAR    *pcFileName=NULL;
    
    if ((NULL==hSecHandle)||(NULL==puiFileId))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if ((M_SEC_X509_USER_CERT==cFileType) || (M_SEC_WTLS_USER_CERT==cFileType)
        || (M_SEC_PUBKEY_CERT==cFileType) || (M_SEC_USER_CERT_HEADER==cFileType))
    {
        uiTmpFileId = M_SEC_USER_CERT_ID_START;
        uiFileIdEnd = M_SEC_CA_CERT_ID_START;
    } 
    else if((M_SEC_WTLS_CA_CERT==cFileType) || (M_SEC_X509_CA_CERT==cFileType)
            || (M_SEC_CA_CERT_HEADER==cFileType))
    {
        uiTmpFileId = M_SEC_CA_CERT_ID_START;
        uiFileIdEnd = M_SEC_CONTRACT_CERT_ID_START;
    }
    else if(M_SEC_CONTRACT==cFileType)
    {
        uiTmpFileId = M_SEC_CONTRACT_CERT_ID_START;
        uiFileIdEnd = M_SEC_CERT_ID_END;
    }
    else        /*other type */
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    while ((!iFound) && (iErrStep<SEC_CREATEFILE_ERROR_NUM) && (uiTmpFileId<uiFileIdEnd))
    {
        
        iResult = Sec_WimAcqFileName(cFileType, uiTmpFileId, &pcFileName);
        if(iResult!=M_SEC_ERR_OK)
        {
            if(NULL != pcFileName)
            {
                WE_FREE(pcFileName);
                pcFileName = NULL;
            }
            return M_SEC_ERR_GENERAL_ERROR;
        }
        iResult=Sec_WimCheckFileExist(hSecHandle,pcFileName);
        if(M_SEC_ERR_OK==iResult)                      /* file exist*/
        {
            uiTmpFileId = uiTmpFileId + 1;
        }           
        else                                     /* file not exist*/
        {
            eRes=WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName, WE_FILE_SET_CREATE , phFileHandle);
            if(E_WE_OK==eRes)                                   /*create success*/
            {  
                *puiFileId = uiTmpFileId;
                iFound = 1;
            }
            else                                                   /*create failure */
            {
                uiTmpFileId = uiTmpFileId + 1;
                iErrStep++;
                
            }
        }  
        WE_FREE(pcFileName);          
    }        
    
    return (WE_INT32)eRes;
}
/*====================================================================================
FUNCTION: 
    Sec_WimDelFile  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    delete file and  it's indexfile by it's type.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:          Global data handle.
    cFileType[IN]:               the type of file:
                                 M_SEC_X509_USER_CERT,M_SEC_WTLS_USER_CERT,M_SEC_PUBKEY_CERT,
                                 M_SEC_WTLS_CA_CERT,M_SEC_X509_CA_CERT,M_SEC_CONTRACT,
                                 M_SEC_USER_CERT_HEADER,M_SEC_CA_CERT_HEADER
    iFileId[IN]:                 FileId
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimDelFile(WE_HANDLE hSecHandle,WE_INT8 cFileType,WE_INT32 iFileId)
{
    WE_INT32         iResult=0;
    WE_CHAR          *pcFileName=NULL;
    E_WE_ERROR        eRes = E_WE_OK;
    
    if(!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if ((M_SEC_X509_USER_CERT==cFileType)||(M_SEC_WTLS_USER_CERT==cFileType)||\
        (M_SEC_PUBKEY_CERT==cFileType)|| (M_SEC_X509_CA_CERT==cFileType)||\
        (M_SEC_WTLS_CA_CERT==cFileType)||(M_SEC_CONTRACT==cFileType)||\
        (M_SEC_USER_CERT_HEADER==cFileType)||(M_SEC_CA_CERT_HEADER==cFileType))
    {
        iResult=Sec_WimAcqFileName(cFileType,(WE_UINT32)iFileId, &pcFileName);
        if(iResult!=M_SEC_ERR_OK)
        {
            if(NULL != pcFileName)
            {
                WE_FREE(pcFileName);
                pcFileName = NULL;
            }
            return M_SEC_ERR_GENERAL_ERROR; 
        }
        iResult=Sec_WimDelBlock(hSecHandle,iFileId,cFileType); 
        if(iResult!=M_SEC_ERR_OK)
        {
            WE_FREE(pcFileName);
            return M_SEC_ERR_MISSING_CERTIFICATE;
        }
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE,pcFileName);  
        if (eRes!= E_WE_OK)
        {
            WE_FREE(pcFileName);
            return M_SEC_ERR_MISSING_CERTIFICATE;
        }
        WE_FREE(pcFileName);
        return iResult;        
    }
    else 
    {
      return M_SEC_ERR_INVALID_PARAMETER;
    }   

} 
/*====================================================================================
FUNCTION: 
    Sec_WimAcqFileName    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get filename by by it's FileId and type.
ARGUMENTS PASSED:
    cFileType[IN]:    the type of file:
                      M_SEC_X509_USER_CERT,M_SEC_WTLS_USER_CERT,M_SEC_PUBKEY_CERT,
                      M_SEC_WTLS_CA_CERT,M_SEC_X509_CA_CERT,M_SEC_CONTRACT,
                      M_SEC_USER_CERT_HEADER,M_SEC_CA_CERT_HEADER
    uiFileId[IN]:     File ID.          
    ppcFileName[OUT]: file name.            
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
=====================================================================================*/
WE_INT32 Sec_WimAcqFileName(WE_INT8 cFileType,WE_UINT32 uiFileId, WE_CHAR **ppcFileName)
{
    WE_CHAR acStrFileId[9]={0}; 
    if(!ppcFileName)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    (void)SEC_SPRINTF(acStrFileId, "%lu",uiFileId);
    /*User Cert or HomeMade Cert*/
    if(((M_SEC_X509_USER_CERT==cFileType)||(M_SEC_WTLS_USER_CERT==cFileType)||\
        (M_SEC_PUBKEY_CERT==cFileType)|| (M_SEC_USER_CERT_HEADER==cFileType))&&\
        (uiFileId>=M_SEC_USER_CERT_ID_START)&&(uiFileId<M_SEC_CA_CERT_ID_START))
    {
        *ppcFileName = WE_SCL_STRCAT ((const WE_CHAR *)M_SEC_USERCERT_FILENAME_PREFIX, \
                                      (const WE_CHAR *)acStrFileId); 
    }
    /*CA Cert*/
    else if(((M_SEC_X509_CA_CERT==cFileType)||(M_SEC_WTLS_CA_CERT==cFileType)||\
        (M_SEC_CA_CERT_HEADER==cFileType))&&(uiFileId>=M_SEC_CA_CERT_ID_START)&&\
        (uiFileId<M_SEC_CONTRACT_CERT_ID_START))
    {
         *ppcFileName = WE_SCL_STRCAT ((const WE_CHAR *)M_SEC_CA_CERT_FILENAME_PREFIX,\
                                       (const WE_CHAR *)acStrFileId); 
    }
    /*CONTRACT*/
    else if ((M_SEC_CONTRACT==cFileType)&&(uiFileId>=M_SEC_CONTRACT_CERT_ID_START)&&(uiFileId<M_SEC_CERT_ID_END))
    {
        *ppcFileName = WE_SCL_STRCAT ((const WE_CHAR *)M_SEC_CONTRACT_FILENAME_PREFIX,\
            (const WE_CHAR *)acStrFileId); 
    } 
    else 
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if(*ppcFileName==NULL)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    else
    {
        return M_SEC_ERR_OK;      
    }
}

/*====================================================================================
FUNCTION:
    Sec_WimCheckFileExist    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    tests whether the specified file is exist.      
ARGUMENTS PASSED:  
    hSecHandle[IN/OUT]: Global data handle.
    pcFileName[IN]:     filename.       
RETURN VALUE:
    If file is exist, return Success.Else return EFAILED.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimCheckFileExist(WE_HANDLE hSecHandle,WE_CHAR *pcFileName)
{    
    return (WE_INT32)WE_FILE_CHECK_EXIST(M_SEC_WIM_HWEHANDLE,pcFileName);
}

/*====================================================================================
FUNCTION: 
    Sec_WimSavePrivateKey  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    store private key,the format is "keylen+PrivateKey",After store Private key,Must store Pin.
    pin and private key stored in one file
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    pucPrivkey[IN]:             Private key
    iPrivkeyLen[IN]:            Length of Privkey
    ucKeyType[IN]:              Privkey type:
                                M_SEC_PRIVATE_AUTHKEY,M_SEC_PRIVATE_NONREPKEY                                
    pcPin[IN]:                  pin
    puiCountOut[OUT]:           Length of wrote private key 
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimSavePrivateKey(WE_HANDLE hSecHandle,WE_UINT8 *pucPrivkey,WE_INT32 iPrivkeyLen,
                               WE_UINT8 ucKeyType, const WE_INT8 *pcPin,WE_UINT32 *puiCountOut)
{
    E_WE_ERROR    eResult = E_WE_OK;
    WE_INT32     iResult = -1;
    WE_UINT32   uiPos = 0;
    WE_UINT16   usStoredPinLen = M_SEC_PIN_MAX_SIZE;
    WE_UINT32   uiPaddedPrivKeyLen = 0;
    WE_UINT16   usPaddedPinLen = 0;
    WE_HANDLE   hFileHandle=NULL;
    WE_CHAR    *pcFileName=NULL;
    WE_INT32    iNewPos=0;
    WE_LONG     lWriteNum=0;
    WE_INT32    iWriteLength=0;
    WE_UINT8    aucTmpKeyLen[4]={0};
   
    if((!hSecHandle)||(!puiCountOut))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
   
    if(M_SEC_PRIVATE_AUTHKEY==ucKeyType)
    {
        pcFileName=M_SEC_AUTH_PRIVKEY_NAME;
    }

    else if(M_SEC_PRIVATE_NONREPKEY==ucKeyType)
    {
        pcFileName=M_SEC_NON_REP_PRIVKEY_NAME;
    }
    else 
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }  

    eResult=WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName, WE_FILE_SET_CREATE , &hFileHandle);    
    if(eResult!=E_WE_OK)
    {
        return (WE_INT32)eResult;
    }

    usPaddedPinLen = (WE_UINT16)Sec_WimGetMultiLen(usStoredPinLen);
    uiPaddedPrivKeyLen = (WE_UINT16)Sec_WimGetMultiLen(iPrivkeyLen);

    eResult = WE_FILE_SETSIZE(hFileHandle,(WE_LONG)(uiPaddedPrivKeyLen+usPaddedPinLen+4));  
    if(eResult!=E_WE_OK)
    {
        eResult = WE_FILE_CLOSE(hFileHandle); 
        return (WE_INT32)eResult;
    }
    eResult=WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET,&iNewPos);
    if(eResult!=E_WE_OK)
    {
        eResult=WE_FILE_CLOSE(hFileHandle);        
        eResult=WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE,pcFileName);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    Sec_ExportStrUint32to8(&uiPaddedPrivKeyLen, aucTmpKeyLen);
    /*write KeyLen*/
    eResult = WE_FILE_WRITE(hFileHandle, aucTmpKeyLen, 4,&lWriteNum);
    if((lWriteNum < 4) || (eResult !=E_WE_OK))
    {
        eResult=WE_FILE_CLOSE(hFileHandle);
        eResult=WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE,pcFileName);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    uiPos +=(WE_UINT32)lWriteNum; 

    iResult = Sec_WimEncryptDataToFile(hSecHandle,hFileHandle, (WE_UINT8 *)pucPrivkey,\
                                       (WE_INT32)uiPos, iPrivkeyLen,&iWriteLength);
    if ((iWriteLength < iPrivkeyLen) || (iResult!=M_SEC_ERR_OK))
    {
        eResult=WE_FILE_CLOSE(hFileHandle);
        eResult=WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE,pcFileName);        
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    uiPos += (WE_UINT32)iWriteLength; 
    uiPos -= 4;

    /*Length of wrote private key*/
    *puiCountOut = uiPos; 
    eResult = WE_FILE_CLOSE(hFileHandle);
    /*write pin*/
    iResult = Sec_WimStorePin(hSecHandle,ucKeyType, pcPin);   
    return iResult;

}
/*====================================================================================
FUNCTION: 
    Sec_WimStorePin  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Write pin .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    ucKeyType[IN]:              Privkey type:
                                M_SEC_PRIVATE_AUTHKEY,M_SEC_PRIVATE_NONREPKEY 
    pcNewPin[IN]:               New pin 
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimStorePin(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType,const WE_INT8 *pcNewPin)
{
    WE_INT32   usStoredLen = M_SEC_PIN_MAX_SIZE; 
    WE_HANDLE  hFileHandle = NULL;    
    WE_INT32   ikeyCount = 0;
    WE_INT32   iLoop=0;
    WE_INT32   iRes = 0;
    WE_INT8    *pcStoredPin = NULL;
    WE_UINT16  usPinLen = 0;
    WE_INT32   iWriteLength=0;
    WE_UINT8   aucTmpKeyLen[4]={0};
    E_WE_ERROR  eRes = E_WE_OK;
    
    if((NULL == hSecHandle) || (NULL == pcNewPin))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    /* get length of the new pin.*/
    usPinLen = (WE_UINT16)(SEC_STRLEN(pcNewPin) + 1); /* modify by stone.*/
    /*
    while(pcNewPin[usPinLen] != 0)
    {
        usPinLen++;
    }
    usPinLen++; */
    if (usPinLen > M_SEC_PIN_MAX_SIZE)
    {
        return M_SEC_ERR_PIN_TOO_LONG;
    }
    else if (usPinLen < M_SEC_PIN_MIN_SIZE)
    {
        return M_SEC_ERR_PIN_TOO_SHORT;
    }
    
    /* open the private key file.*/
    if (M_SEC_PRIVATE_AUTHKEY == ucKeyType)
    {
        iRes = Sec_WimCheckFileExist(hSecHandle, (WE_CHAR *)(M_SEC_AUTH_PRIVKEY_NAME));
        if (iRes != M_SEC_ERR_OK)
        {
            return M_SEC_ERR_MISSING_KEY;
        }
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_AUTH_PRIVKEY_NAME,WE_FILE_SET_RDWR, &(hFileHandle));
        
    }
    else if (M_SEC_PRIVATE_NONREPKEY == ucKeyType)
    {    
        iRes = Sec_WimCheckFileExist(hSecHandle, (WE_CHAR *)(M_SEC_NON_REP_PRIVKEY_NAME));
        if (iRes != M_SEC_ERR_OK)
        {
            return M_SEC_ERR_MISSING_KEY;
        }
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_NON_REP_PRIVKEY_NAME,
            WE_FILE_SET_RDWR, &(hFileHandle));
    }
    else
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (eRes != E_WE_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    /* get the pin to be stored.*/
    pcStoredPin = (WE_INT8 *)WE_MALLOC((WE_UINT32)usStoredLen * sizeof(WE_INT8));
    if(NULL == pcStoredPin)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    for (iLoop=0; iLoop<usPinLen; iLoop++)
    {
        pcStoredPin[iLoop] = pcNewPin[iLoop];
    }
    for (iLoop=usPinLen; iLoop<usStoredLen; iLoop++)
    {
        pcStoredPin[iLoop] = SEC_PIN_PADDING_CHARACTER;
    } 
    
    /* read out the length of private key.*/
    eRes = WE_FILE_READ(hFileHandle, aucTmpKeyLen, M_SEC_PRIVKEY_COUNT_LENGTH, &iWriteLength);
    if ((iWriteLength<M_SEC_PRIVKEY_COUNT_LENGTH) || (eRes!=E_WE_OK))
    {
        WE_FREE(pcStoredPin);    
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    Sec_StoreStrUint8to32( aucTmpKeyLen, (WE_UINT32 *)(&ikeyCount) );
    /* write the pin.*/
    iRes = Sec_WimEncryptDataToFile(hSecHandle, hFileHandle, (WE_UINT8 *)pcStoredPin, \
                         (M_SEC_PRIVKEY_COUNT_LENGTH+ikeyCount), usStoredLen, &iWriteLength);
    WE_FREE(pcStoredPin);    
    if ((iWriteLength < usStoredLen) || (iRes != M_SEC_ERR_OK))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);       
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);        
    
    return M_SEC_ERR_OK;
}
/*====================================================================================
FUNCTION: 
    Sec_WimReadPin  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Read pin.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    ucKeyType[IN]:              Privkey type:
                                M_SEC_PRIVATE_AUTHKEY,M_SEC_PRIVATE_NONREPKEY 
    pcStoredPin[OUT]:           pin 
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/  
WE_INT32 Sec_WimReadPin(WE_HANDLE hSecHandle,WE_UINT8 ucKeyType, WE_CHAR *pcStoredPin)
{
    WE_INT32   iPaddedPinLen = Sec_WimGetMultiLen(M_SEC_PIN_MAX_SIZE); 
    WE_HANDLE  hFileHandle = NULL;    
    WE_INT32   ikeyCount = 0;
    WE_INT32   iRes = 0;
    E_WE_ERROR eRes = E_WE_OK;
    WE_INT32   iReaderLength=0;
    WE_CHAR    *pcFileName=NULL;
    WE_UINT8   aucTmpKeyLen[4]={0};
    
    if(NULL == hSecHandle) 
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (M_SEC_PRIVATE_AUTHKEY == ucKeyType)
    {
        pcFileName=M_SEC_AUTH_PRIVKEY_NAME;    
        
    }
    else if (M_SEC_PRIVATE_NONREPKEY == ucKeyType)
    {    
        pcFileName=M_SEC_NON_REP_PRIVKEY_NAME;
    }
    else
    { 
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRes = Sec_WimCheckFileExist(hSecHandle, pcFileName);/*not exist*/
    if (iRes != M_SEC_ERR_OK)
    {
        return M_SEC_ERR_MISSING_KEY;
    }
    if(pcStoredPin != NULL)
    {
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName,WE_FILE_SET_RDWR, &(hFileHandle));    
        
        if (eRes != E_WE_OK)
        {
            return M_SEC_ERR_GENERAL_ERROR;
        }
        eRes = WE_FILE_READ(hFileHandle, aucTmpKeyLen, M_SEC_PRIVKEY_COUNT_LENGTH, &iReaderLength);
        if ((iReaderLength<M_SEC_PRIVKEY_COUNT_LENGTH) || (eRes!=E_WE_OK))
        {
            eRes = WE_FILE_CLOSE(hFileHandle);
            return M_SEC_ERR_GENERAL_ERROR;
        }
        Sec_StoreStrUint8to32( aucTmpKeyLen, (WE_UINT32 *)(&ikeyCount));           
        iRes = Sec_WimDecryptDataFromFile(hSecHandle, hFileHandle, (WE_UINT8 *)pcStoredPin, 
                            (M_SEC_PRIVKEY_COUNT_LENGTH+ikeyCount), iPaddedPinLen, &iReaderLength);
        if ((iReaderLength < iPaddedPinLen) || (iRes != M_SEC_ERR_OK))
        {    
            eRes = WE_FILE_CLOSE(hFileHandle);
            return M_SEC_ERR_GENERAL_ERROR;
        }
        eRes = WE_FILE_CLOSE(hFileHandle);
    }
    return M_SEC_ERR_OK;
}
/*====================================================================================
FUNCTION: 
    Sec_WimCheckPin  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Verify Pin 
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    ucKeyType[IN]:              Privkey type:
                                M_SEC_PRIVATE_AUTHKEY,M_SEC_PRIVATE_NONREPKEY 
    pcPin[IN]:                  pin
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimCheckPin(WE_HANDLE hSecHandle,WE_UINT8 ucKeyType, const WE_CHAR *pcPin)
{
    WE_INT32  iLoop = 0;
    WE_INT32  iResult = 0;
    WE_CHAR   *pcReferencePin = NULL;
    WE_INT32  iPaddedPinLen = Sec_WimGetMultiLen(M_SEC_PIN_MAX_SIZE);     
    
    if(NULL == pcPin)
    {
        return M_SEC_ERR_PIN_FALSE;
    }
    
    pcReferencePin = (WE_CHAR *)WE_MALLOC((WE_UINT32)iPaddedPinLen * sizeof(WE_INT8));
    if(NULL == pcReferencePin)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    iResult = Sec_WimReadPin(hSecHandle, ucKeyType, pcReferencePin);    
    if(M_SEC_ERR_OK == iResult)
    {
        iLoop = 0;
        /*compare pin*/
        while((pcPin[iLoop] == pcReferencePin[iLoop]) && (pcReferencePin[iLoop] != 0))
        {
            pcReferencePin[iLoop] = 0; 
            iLoop++;
        }
        
        if((pcReferencePin[iLoop] == 0) && (pcPin[iLoop] == 0))
        {
            iResult = M_SEC_ERR_OK;
        }
        else
        {
            iResult = M_SEC_ERR_PIN_FALSE;
        }

        while(pcReferencePin[iLoop] != 0)
        {
            pcReferencePin[iLoop] = 0;
            iLoop++;
        }    
        WE_FREE(pcReferencePin);
    }
    return iResult;
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetMultiLen  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get Padded Length.
ARGUMENTS PASSED:
    iDataLen[IN]:the value of length.   
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
=====================================================================================*/
WE_INT32 Sec_WimGetMultiLen(WE_INT32 iDataLen)
{
    WE_INT32 iLen = iDataLen;
    WE_INT32 iTmpLen=0;
    WE_INT32 iPaddedLen=0;
    
    iLen += 2;                    
    iTmpLen = iLen % 8;    
    if ((iLen-2) == 0)
    {
        iPaddedLen = iLen - 2;
    }
    else if (iTmpLen != 0)
    {
        iTmpLen = 8 - iTmpLen;
        iPaddedLen = iLen + iTmpLen;
    }
    else
    {
        iPaddedLen = iLen;
    }
    return iPaddedLen;
} 
/*====================================================================================
FUNCTION: 
    Sec_WimGetOrigiLen  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get Unpadded Length.
ARGUMENTS PASSED:
    pucDecryptedData[IN]:    Decrypted Data.
    iDataLen[IN]:             The length of pucDecryptedData.
RETURN VALUE:
    The length of unpadded pucDecryptedData.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetOrigiLen(WE_UINT8 *pucDecryptedData, WE_INT32 iDataLen)
{
    WE_INT32 iLen = 0;
    if ((0==iDataLen ) || (NULL==pucDecryptedData))
    {
        return 0;
    }
    iLen = iDataLen;
    while ((SEC_SECOND_PAD_VALUE==pucDecryptedData[iLen-1]) && (iLen > 1))
    {
        iLen--;
    }
    iLen--;
    return iLen;
}
/*====================================================================================
FUNCTION: 
    Sec_WimAddBlock    
CREATE DATE:  
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    add User Cert,homeMade Cert ,CA Cert,Contract to IndexFile.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:       Global data handle.
    cFileType[IN]:            the type of file:
                              M_SEC_X509_USER_CERT,M_SEC_WTLS_USER_CERT,M_SEC_PUBKEY_CERT,
                              M_SEC_WTLS_CA_CERT,M_SEC_X509_CA_CERT,M_SEC_CONTRACT,
    uiCertId[IN]:             certificate file id.
    uiCount[IN]:              Count of certificate.
    pucDNHash[IN]:            issuerhash of user certificate, or subjecthash of CA cert.
    pucPubkeyHash[IN]:        PubkeyHash of user certificate, or issuerhash of CA cert.
    cNameType[IN]:            NameType of certificate.    
    sCharSet[IN]:             Charser of certificate.
    usFriendlyNameLen[IN]:    Friendly Name Length of certificate
    pucFriendlyName[IN]:      FriendlyName
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimAddBlock(WE_HANDLE hSecHandle,WE_INT8 cFileType,WE_UINT32 uiCertId, 
                         WE_UINT32 uiCount,const WE_UINT8 *pucDNHash ,const WE_UINT8 * pucPubkeyHash,
                         WE_INT8 cNameType,WE_INT16 sCharSet,WE_UINT16 usFriendlyNameLen,
                         const WE_UINT8 *pucFriendlyName, WE_UINT8 ucAble)
{    
    E_WE_ERROR   eResult= E_WE_OK;
    WE_INT32    iBlockLen = 0 ; 
    WE_UINT8    *pucParamsStr=NULL;
    WE_UINT8    *pucP=NULL;
    WE_CHAR     *pcFileName=NULL;
    WE_INT32    iLoop=0;
    WE_HANDLE   hFileHandle=NULL;
    WE_LONG     lWriteNum=0;
    WE_LONG     lSize=0;
    WE_UINT8    ucFileNum=0;
    WE_LONG     lReadNum=0;
    WE_INT32    iNewPos=0;
    
    if (NULL==hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if ((M_SEC_X509_USER_CERT==cFileType)||(M_SEC_WTLS_USER_CERT==cFileType)||(M_SEC_PUBKEY_CERT==cFileType))    /*User Cert or HomeMade Cert*/
    {
        if ((NULL==pucDNHash) || (NULL==pucPubkeyHash) || (NULL==pucFriendlyName))
        {
            return M_SEC_ERR_INVALID_PARAMETER;
        }
        pcFileName=M_SEC_USERCERT_INDEXFILE_NAME;
        iBlockLen=1+4+4+20+20+1+2+2+usFriendlyNameLen;
    }
    else if ((M_SEC_X509_CA_CERT==cFileType)||(M_SEC_WTLS_CA_CERT==cFileType))                /*CA Cert*/
    {
        if ((NULL==pucDNHash) || (NULL==pucPubkeyHash) || (NULL==pucFriendlyName))
        {
            return M_SEC_ERR_INVALID_PARAMETER;
        }
        pcFileName=M_SEC_CA_CERT_INDEXFILE_NAME;            
        iBlockLen=1+4+4+20+20+1+2+1+2+usFriendlyNameLen;
    }
    else if (M_SEC_CONTRACT==cFileType)                            /*CONTRACT*/
    {
        pcFileName = M_SEC_CONTRACT_INDEXFILE_NAME;            
        iBlockLen = 4+4;
    }
    else
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    /*get the position*/
    eResult = WE_FILE_GETSIZE (M_SEC_WIM_HWEHANDLE,(const WE_CHAR *)pcFileName,&lSize);    
    if (eResult!=E_WE_OK)                
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    eResult = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName, WE_FILE_SET_RDWR, &hFileHandle);
    if(eResult!=E_WE_OK)
    {
        return (WE_INT32)eResult;
    }
    eResult = WE_FILE_SETSIZE(hFileHandle, (lSize+iBlockLen));
    if (eResult!=E_WE_OK)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    pucParamsStr= (WE_UINT8 *)WE_MALLOC((WE_UINT32)iBlockLen*sizeof(WE_UINT8));     
    if(!pucParamsStr)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    /*get the data written to indexfile*/
    pucP = pucParamsStr;
    if((M_SEC_X509_USER_CERT==cFileType)||(M_SEC_WTLS_USER_CERT==cFileType)||\
        (M_SEC_PUBKEY_CERT==cFileType)||(M_SEC_X509_CA_CERT==cFileType)||\
        (M_SEC_WTLS_CA_CERT==cFileType))        /*User Cert or HomeMade Cert, CA Cert*/
    {      
        *pucP++ = (WE_UINT8)cFileType;
        Sec_ExportStrUint32to8(&uiCertId, pucP);
        pucP += 4;
        Sec_ExportStrUint32to8(&uiCount, pucP);
        pucP+=4;
        (WE_VOID)WE_MEMCPY(pucP, pucDNHash, M_SEC_DN_HASH_LEN);
        pucP += M_SEC_DN_HASH_LEN;
        (WE_VOID)WE_MEMCPY(pucP, pucPubkeyHash, M_SEC_DN_HASH_LEN);
        pucP += M_SEC_DN_HASH_LEN;        
        *pucP++=(WE_UINT8)cNameType;
        Sec_ExportStrUint16to8((WE_UINT16 *)(&sCharSet), pucP);
        pucP+=2;
        if ((M_SEC_X509_CA_CERT==cFileType) || (M_SEC_WTLS_CA_CERT==cFileType))
        {
            *pucP++ = ucAble;
        }
        Sec_ExportStrUint16to8(&usFriendlyNameLen, pucP);
        pucP+=2;
        for(iLoop=0;iLoop<usFriendlyNameLen;iLoop++)
        {
            pucP[iLoop]=pucFriendlyName[iLoop];
        }
    }
    else                                /*CONTRACT*/
    {         
        Sec_ExportStrUint32to8(&uiCertId, pucP);
        pucP += 4;
        Sec_ExportStrUint32to8(&uiCount, pucP);                 
        
    }
    
    /*file seek,then write file*/ 
    eResult = WE_FILE_SEEK(hFileHandle, lSize, WE_FILE_SEEK_SET,&iNewPos);
    eResult = WE_FILE_WRITE(hFileHandle,pucParamsStr, iBlockLen,&lWriteNum);
    WE_FREE(pucParamsStr);
    if((lWriteNum<iBlockLen)||eResult!=E_WE_OK)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    /*updata file num*/
    eResult = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET,&iNewPos);
    eResult = WE_FILE_READ(hFileHandle, &ucFileNum, 1, &lReadNum);
    if(eResult!=E_WE_OK)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    ucFileNum+=1;
    eResult = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET,&iNewPos);
    eResult = WE_FILE_WRITE(hFileHandle,&ucFileNum, 1,&lWriteNum);
    if((lWriteNum<1)||eResult!=E_WE_OK)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    eResult = WE_FILE_CLOSE(hFileHandle);
    return M_SEC_ERR_OK;    
    
}
/*====================================================================================
FUNCTION: 
    Sec_WimStorePubKeyCert  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    store homemade certificate,first Create a file,get this fils's FileId and FileHandle
    ;then write Cert into this file,get CertCount and NameCount,last write indexfile    
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    pucCert[IN]:                Certificate
    usCertLen[IN]:              Length of Certificate
    piUserCertId[OUT]:           Certificate FileId
    piUserCertCount[OUT]:       actual length of certificate wrote to the file
    pucPublicKeyHash[IN]:       public key hash
    piNameCount[OUT]:           actual length of Name  wrote to the file
    ucAlg[IN]:                  Algorithm(not used).
    ucKeyType[IN]:              KeyType(M_SEC_PRIVATE_NONREPKEY/M_SEC_PRIVATE_AUTHKEY) 
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimStorePubKeyCert(WE_HANDLE hSecHandle,WE_UINT8 *pucCert, WE_UINT16 usCertLen, 
                                WE_INT32 *piUserCertId,WE_INT32 *piUserCertCount, 
                                const WE_UINT8 *pucPublicKeyHash,WE_INT32 *piNameCount, 
                                WE_UINT8 ucAlg, WE_UINT8 ucKeyType)
{
    WE_INT32     iResult = 0;  
    E_WE_ERROR   eRes = E_WE_OK;
    WE_UINT32    uiFileId = 0;
    WE_INT16     sCharSet=0;     
    WE_HANDLE    hFileHandle=NULL;
    WE_CHAR      *pcFileName=NULL;
    WE_INT32     iWriteLength=0;
    WE_INT32     iNewPos=0;
    WE_INT8      cFileType=0;
    WE_UINT8     aucTmpDnHash[20]={0};
    WE_UINT16    usNameLen=0;
    WE_UINT8     *pucTmpName = NULL;
    
    ucAlg=ucAlg;    
    if((!hSecHandle)||(!piUserCertId)||(!piNameCount)||(!piUserCertCount))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if (M_SEC_PRIVATE_NONREPKEY==ucKeyType )
    {
        usNameLen  = M_SEC_PUB_KEY_CERT_NAME_NP_LEN;
        sCharSet   = M_SEC_PUB_KEY_CERT_NAME_NP_CHARSET;
        pucTmpName = (WE_UINT8 *)M_SEC_PUB_KEY_CERT_NAME_NP;
    }
    else if (M_SEC_PRIVATE_AUTHKEY==ucKeyType )
    {
        usNameLen = M_SEC_PUB_KEY_CERT_NAME_AUT_LEN;
        sCharSet = M_SEC_PUB_KEY_CERT_NAME_AUT_CHARSET;
        pucTmpName = (WE_UINT8 *)M_SEC_PUB_KEY_CERT_NAME_AUT;
    }
    else
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    cFileType = M_SEC_PUBKEY_CERT; 
    /*Create file,and get "FileId","hFilehandle"*/    
    iResult = Sec_WimMakeFile(hSecHandle,cFileType,&hFileHandle, &uiFileId);
    if (iResult !=M_SEC_ERR_OK)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }   
    iResult = Sec_WimAcqFileName(cFileType, uiFileId, &pcFileName);
    eRes = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET, &iNewPos);
    if(eRes!=E_WE_OK)
    { 
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE,pcFileName);
        WE_FREE(pcFileName);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    /*write cert*/
    eRes = WE_FILE_WRITE(hFileHandle, (WE_UINT8 *)pucCert, usCertLen,&iWriteLength);
    if((eRes != E_WE_OK)||(iWriteLength< (WE_INT32)usCertLen))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE,pcFileName);
        WE_FREE(pcFileName);        
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);
    
    /*add block*/
    iResult = Sec_WimAddBlock(hSecHandle,cFileType, uiFileId, usCertLen,aucTmpDnHash,pucPublicKeyHash,
        M_SEC_CERT_NAME_WTLS, sCharSet , usNameLen, pucTmpName, 0);
    if (iResult != M_SEC_ERR_OK)
    {        
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE,pcFileName);
        WE_FREE(pcFileName);
        return iResult;     
    }
    WE_FREE(pcFileName);
    *piUserCertId = (WE_INT32)uiFileId; 
    *piUserCertCount = usCertLen;
    *piNameCount = usNameLen;
    return M_SEC_ERR_OK;
}


/*====================================================================================
FUNCTION: 
    Sec_WimDelBlock    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Delete the bolck from IndexFile ,which FileId is iInFileId.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]: Global data handle.
    iInFileId[IN]:      File ID.    
    cFileType[IN]:      the type of file:
                        M_SEC_X509_USER_CERT,M_SEC_WTLS_USER_CERT,M_SEC_PUBKEY_CERT,
                        M_SEC_WTLS_CA_CERT,M_SEC_X509_CA_CERT,M_SEC_CONTRACT,
                        M_SEC_USER_CERT_HEADER,M_SEC_CA_CERT_HEADER
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimDelBlock(WE_HANDLE hSecHandle,WE_INT32 iInFileId,WE_INT8 cFileType)
{
    WE_UINT32   uiFileId = 0;
    WE_INT32    iLoop=0; 
    E_WE_ERROR  eResult = E_WE_OK;
    WE_INT32    iResult = 0;
    WE_LONG     lReadNum=0;
    WE_HANDLE   hFileHandle=NULL;
    WE_INT32    iFileNum=0; 
    WE_UINT8    *pucIdxFileData=NULL;  
    WE_INT8     *pcIndexFileName=NULL;  
    WE_UINT8    *pucP = NULL;
    WE_INT32    iPos = 0;
    WE_INT32    iNewPos = 0;
    WE_INT32    iBlockLen = 0;
    WE_INT32    iLeftLength = 0;
    WE_INT32    iSizeOfIndexFile = 0;
    WE_CHAR     cFound = 0;    
    
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }    
    
    if((M_SEC_X509_CA_CERT==cFileType) || (M_SEC_WTLS_CA_CERT==cFileType) || (M_SEC_CA_CERT_HEADER==cFileType))
    {       
        pcIndexFileName = M_SEC_CA_CERT_INDEXFILE_NAME;
    }
    else if (M_SEC_CONTRACT == cFileType)
    {
        pcIndexFileName = M_SEC_CONTRACT_INDEXFILE_NAME;
    }
    else/*user Cert or HomeMade Cert*/
    {       
        pcIndexFileName = M_SEC_USERCERT_INDEXFILE_NAME;
    }
    eResult = WE_FILE_GETSIZE (M_SEC_WIM_HWEHANDLE, pcIndexFileName, &iSizeOfIndexFile);
    if (eResult != E_WE_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eResult = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcIndexFileName, WE_FILE_SET_RDWR, &hFileHandle);
    if (eResult != E_WE_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    pucIdxFileData = (WE_UINT8 *)WE_MALLOC((WE_UINT32)iSizeOfIndexFile * sizeof(WE_UINT8));
    if (NULL == pucIdxFileData)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    /*read the whole data of indexfile to pucIdxFileData*/        
    eResult = WE_FILE_READ(hFileHandle, pucIdxFileData, iSizeOfIndexFile, &lReadNum);
    if ((eResult!=E_WE_OK) || (lReadNum<iSizeOfIndexFile))
    {
        WE_FREE(pucIdxFileData);
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    pucP = pucIdxFileData;
    iFileNum = *pucP;      /*get file number*/
    pucP++;
    iPos = 1;      
    cFound = 0;  /* set not found */
    for(iLoop=0; iLoop<iFileNum; iLoop++)  
    {/* add by Sam [070203] */
        if (M_SEC_CONTRACT == cFileType)
        {
            Sec_StoreStrUint8to32(pucP, &uiFileId);
        }
        else
        {
            Sec_StoreStrUint8to32(pucP+1, &uiFileId);
        }
        if (uiFileId == (WE_UINT32)iInFileId)
        {  
            iResult = Sec_WimGetBlockLength(pucP, cFileType, &iBlockLen);
            if (iResult != M_SEC_ERR_OK)
            {               
                WE_FREE(pucIdxFileData);
                eResult = WE_FILE_CLOSE(hFileHandle);
                return M_SEC_ERR_GENERAL_ERROR;
            }
            /* length data which need to be moved. */
            iLeftLength = iSizeOfIndexFile - iBlockLen - iPos;
            iSizeOfIndexFile = iSizeOfIndexFile - iBlockLen;
            if (0 != iLeftLength) /* the block at the end of indexfile.*/
            {
                (WE_VOID)WE_MEMMOVE(pucP, pucP+iBlockLen, (WE_UINT32)iLeftLength);
            }
            iFileNum--;
            cFound = 1;
            break;
        }           
        else /* the file exists.*/
        {
            iResult = Sec_WimGetBlockLength(pucP, cFileType, &iBlockLen);
            if (iResult != M_SEC_ERR_OK)
            {               
                WE_FREE(pucIdxFileData);
                eResult = WE_FILE_CLOSE(hFileHandle);
                return M_SEC_ERR_GENERAL_ERROR;
            }
            pucP += iBlockLen;            
            iPos += iBlockLen;
        }
    }
    
    if (1 == cFound)
    {        
        *pucIdxFileData = (WE_UCHAR)iFileNum;
        eResult = WE_FILE_SETSIZE(hFileHandle, iSizeOfIndexFile);
        eResult = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET, &(iNewPos));
        eResult = WE_FILE_WRITE(hFileHandle, pucIdxFileData, iSizeOfIndexFile, &(iResult));
        if ((eResult!=E_WE_OK) || (iResult<iSizeOfIndexFile))
        { 
            WE_FREE(pucIdxFileData);
            eResult = WE_FILE_CLOSE(hFileHandle);            
            return M_SEC_ERR_GENERAL_ERROR;
        }
        iResult = M_SEC_ERR_OK;
    }
    else
    {
        iResult = M_SEC_ERR_NOT_FOUND;
    }
    WE_FREE(pucIdxFileData);
    eResult = WE_FILE_CLOSE(hFileHandle);   
    return iResult;    
}     
/*====================================================================================
FUNCTION: 
    Sec_WimGenPubKeyAndUCertInfo  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Store RsaPubKey
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    pucPubkey[IN]:              public key.
    iPubkeyLen[IN]:             the length of pucPubkey.
    ucKeyType[IN]:              key type.
    iCountK[IN]:                the count of key.   
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGenPubKeyAndUCertInfo(WE_HANDLE hSecHandle,WE_UINT8 *pucPubkey, WE_INT32 iPubkeyLen, 
                                      WE_UINT8 ucKeyType,  WE_INT32 iCountK)
{
    WE_UINT16  usCertLen = (WE_UINT16)(iPubkeyLen*4); 
    WE_UINT8   *pucUserCert=NULL;
    WE_INT32   iResult = 0;
    WE_INT32   iRes = 0;
    WE_UINT16  usModLen = 0;
    WE_UINT8   *pucTmpPubkey = pucPubkey;
    WE_UINT16  usJumpStep = 0;
    WE_INT32   iUserCertId = 0;
    WE_INT32   iUserCertCount = 0; 
    WE_UINT8   aucPubKeyHash[M_SEC_KEY_HASH_SIZE]={0};
    WE_UINT8   aucCAPubKeyHash[M_SEC_KEY_HASH_SIZE]={0};
    WE_UINT8   ucPubKeyCert = 1;
    WE_INT32   iNameCount = 0;
    WE_INT32   iDigLen = M_SEC_KEY_HASH_SIZE;
    WE_INT8    cFileType=0; 
    
    if ((NULL==hSecHandle) || (NULL==pucPubkey))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if ((M_SEC_PRIVATE_NONREPKEY==ucKeyType) || (M_SEC_PRIVATE_AUTHKEY==ucKeyType ))
    {        
        cFileType  = M_SEC_PUBKEY_CERT;      
    }    
    else
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucUserCert = (WE_UINT8 *)WE_MALLOC((usCertLen+1)*sizeof(WE_UINT8));
    if (NULL == pucUserCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    /*generate homemade certificate by pubkey */
    iResult = Sec_WtlsCreateCertificate(hSecHandle, pucUserCert, &usCertLen, pucPubkey, (WE_UINT16) iPubkeyLen);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucUserCert);
        return iResult;
    }
    pucTmpPubkey += 1;            
    Sec_X509DerToSize(hSecHandle,pucTmpPubkey, &usJumpStep, &usModLen);
    pucTmpPubkey += usJumpStep;
    pucTmpPubkey += 1;            
    Sec_X509DerToSize(hSecHandle,pucTmpPubkey, &usJumpStep, &usModLen);     
    pucTmpPubkey += usJumpStep;
    if (*pucTmpPubkey == 0x00)
    {
        usModLen--;
        pucTmpPubkey++;
    }
    /*get public key Hash*/
    iResult = Sec_LibHash (hSecHandle, E_SEC_ALG_HASH_SHA1, pucTmpPubkey, usModLen, aucPubKeyHash, &iDigLen);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucUserCert);
        return iResult;
    }
    
    iResult = Sec_WimStorePubKeyCert(hSecHandle, pucUserCert, usCertLen, &iUserCertId,
        &iUserCertCount, aucPubKeyHash, &iNameCount,  M_SEC_SP_RSA, ucKeyType);
    WE_FREE(pucUserCert);
    if (iResult != M_SEC_ERR_OK)
    { 
        return iResult;
    }
    
    iResult = Sec_WimUpdataUCertKeyPairInfo(hSecHandle,iUserCertId, iUserCertCount, 
                                       iCountK, ucKeyType, 
                                       (const WE_UINT8 *)aucPubKeyHash, 
                                       (const WE_UINT8 *)aucCAPubKeyHash, 
                                       ucPubKeyCert, iNameCount,
                                        M_SEC_CERT_NAME_WTLS);
    if (iResult != M_SEC_ERR_OK)        
    {       
        iRes = Sec_WimDelFile(hSecHandle, cFileType, iUserCertId); 
        if (iRes != M_SEC_ERR_OK)
        {
            return iRes;
        }
        return M_SEC_ERR_GENERAL_ERROR;
    } 
    return iResult;
}

/*====================================================================================
FUNCTION: 
    Sec_WimUpdataUCertKeyPairInfo  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    store user data in vector,then write to file and indexfile
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:     Global data handle.
    iUserCertId[IN]:        user certificate id.
    iUserCertCount[IN]:     length of user certificate.
    iPrivKeyCount[IN]:      length of private key.
    ucPrivKeyUsage[IN]:     usage of private key.
    pucPublicKeyHash[IN]:   public key hash.
    pucCaPublicKeyHash[IN]: CA public key hash.
    ucPublicKeyCert[IN]:    the certificate of public key .
    iNameCount[IN]:         length of name.
    iNameType[IN]:          the type of name .    
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimUpdataUCertKeyPairInfo(WE_HANDLE hSecHandle,WE_INT32 iUserCertId, 
                                       WE_INT32 iUserCertCount, 
                                       WE_INT32 iPrivKeyCount, WE_UINT8 ucPrivKeyUsage,
                                       const WE_UINT8 *pucPublicKeyHash, 
                                       const WE_UINT8 *pucCaPublicKeyHash, 
                                       WE_UINT8 ucPublicKeyCert, 
                                       WE_INT32 iNameCount, WE_INT8 cNameType)
{
    WE_INT32 iIndex = 0;
    WE_INT32 iLoop=0;
    WE_UINT8 ucNbrEmpty = 0;
    if((NULL==hSecHandle)||(NULL==pucPublicKeyHash)||(NULL==pucCaPublicKeyHash))       
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    for(iIndex=0;(!ucNbrEmpty) && (iIndex < M_SEC_USER_CERT_MAX_SIZE);iIndex++)
    {
        if (0 == M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].iUcertId)
        {
            ucNbrEmpty = 1;
            break;
        }    
    }
    
    if (!ucNbrEmpty)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].iUcertId = iUserCertId;
    M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].iUcertCount = iUserCertCount;
    M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].iPkeyCount = iPrivKeyCount;
    M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].ucKeyUsage = ucPrivKeyUsage;
    M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].ucPublicKeyCert = ucPublicKeyCert;
    M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].iNameCount = iNameCount;
    M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].cNameType = cNameType;
    
    for (iLoop=0; iLoop<M_SEC_KEY_HASH_SIZE; iLoop++)
    {
        M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].aucPublicKeyHash[iLoop] = pucPublicKeyHash[iLoop];
        M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].aucCApublicKeyHash[iLoop] = pucCaPublicKeyHash[iLoop];       
    }
    
    /*save M_SEC_WIM_ASTUSERCERTKEYPAIR[]*/
    Sec_WimSaveKeyPairInfoToFile(hSecHandle);
    return M_SEC_ERR_OK;
}

/*====================================================================================
FUNCTION: 
    Sec_WimSaveKeyPairInfoToFile    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    write M_SEC_WIM_ASTUSERCERTKEYPAIR to file.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:             Global data handle.
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
=====================================================================================*/    
void Sec_WimSaveKeyPairInfoToFile(WE_HANDLE hSecHandle)
{ 
    WE_UINT32 uiKeyPairRecSize=0;
    WE_INT32  iResult = 0;
    E_WE_ERROR eRes = E_WE_OK;
    WE_INT32  iNewPos=0;
    WE_LONG   lWriteNum=0;
    WE_HANDLE hFileHandle=NULL;
    
    if (NULL == hSecHandle)
    {
        return ;
    }
    uiKeyPairRecSize =  M_SEC_USER_CERT_MAX_SIZE * sizeof(St_WimUCertKeyPairInfo);   
    iResult = Sec_WimCheckFileExist(hSecHandle, M_SEC_USERCERT_PRIVKEY_INDEX_NAME);
    if (iResult != M_SEC_ERR_OK)      /*'I' file not exist*/
    {
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_USERCERT_PRIVKEY_INDEX_NAME, WE_FILE_SET_CREATE , &hFileHandle);
        if (eRes != E_WE_OK)
        {
            return ;
        }
    }                               /*'I' file exist*/
    else
    {
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_USERCERT_PRIVKEY_INDEX_NAME, WE_FILE_SET_RDWR, &hFileHandle);
        if(eRes != E_WE_OK)
        {
            return ;
        }
    }
    /*set file size*/
    eRes = WE_FILE_SETSIZE(hFileHandle, (WE_LONG)uiKeyPairRecSize);  
    if (eRes != E_WE_OK)    
    {        
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, M_SEC_USERCERT_PRIVKEY_INDEX_NAME);        
        return;    
    }    
    eRes = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET,&iNewPos);
    /*write 'I' File*/
    eRes = WE_FILE_WRITE(hFileHandle, M_SEC_WIM_ASTUSERCERTKEYPAIR, (WE_LONG)uiKeyPairRecSize,&lWriteNum);      
    if ((lWriteNum < (WE_INT32)uiKeyPairRecSize) || (eRes!=E_WE_OK))
    {        
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, M_SEC_USERCERT_PRIVKEY_INDEX_NAME);       
        return;
    }
}
/*====================================================================================
FUNCTION: 
    Sec_WimClearOneUCertKeyInfo    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    clear one user cert key pair record.
ARGUMENTS PASSED:
    pstRec[IN] : user cert key record.                
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
=====================================================================================*/
void Sec_WimClearOneUCertKeyInfo(St_WimUCertKeyPairInfo *pstRec)
{
   WE_INT32 iLoop=0;

   if (NULL == pstRec)
   {
       return;
   }
  
   (*pstRec).iUcertId = 0;
   (*pstRec).iUcertCount = 0;
   (*pstRec).iPkeyCount = 0;
   (*pstRec).ucKeyUsage = 0;
   for (iLoop=0; iLoop<M_SEC_KEY_HASH_SIZE; iLoop++)
   {
       (*pstRec).aucPublicKeyHash[iLoop] = 0;
       (*pstRec).aucCApublicKeyHash[iLoop] = 0;
   } 
   (*pstRec).ucPublicKeyCert = 0;
   (*pstRec).iNameCount = 0;
   (*pstRec).cNameType = 0;
}

/*====================================================================================
FUNCTION: 
    Sec_WimClearKeyPairInfo    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Clear all  User cert key pair Record :M_SEC_WIM_ASTUSERCERTKEYPAIR[M_SEC_USER_CERT_MAX_SIZE]
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:             Global data handle.
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
=====================================================================================*/
void Sec_WimClearKeyPairInfo(WE_HANDLE hSecHandle)
{
   WE_INT32 iLoop;
   if(NULL==hSecHandle)
   {
        return ;
   }   
   for (iLoop=0; iLoop<M_SEC_USER_CERT_MAX_SIZE; iLoop++)
   {
       Sec_WimClearOneUCertKeyInfo(&(M_SEC_WIM_ASTUSERCERTKEYPAIR[iLoop]));
   }
}

/*====================================================================================
FUNCTION: 
    Sec_WimCheckNonVerifiedCert  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
   check prestored root certificate.
ARGUMENTS PASSED: 
    hSecHandle[IN/OUT]:         Global data handle.
    iTargetID[IN]:              object id.
    cCertType[IN]:              the type of CA certificate:
                                M_SEC_NON_VERIFIED_WTLS_TYPE,M_SEC_NON_VERIFIED_X509_TYPE
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
=====================================================================================*/
void Sec_WimCheckNonVerifiedCert (WE_HANDLE hSecHandle,WE_INT32 iTargetID, WE_INT8 cCertType)
{
    WE_INT32  iResult = 0;
    WE_HANDLE hFileHandle=NULL;
    WE_CHAR   *pcFileName = NULL;
    WE_INT32  iNewPos=0;
    WE_LONG   lReadNum=0;    
    WE_INT32  iNotVerCertNum = 0;
    WE_INT32  iIndexI = 0;
    WE_INT32  iType = 0;
    WE_LONG   lSize = 0L;
    WE_CHAR   acNameBuf[20] = {0};
    E_WE_ERROR  eRes = E_WE_OK;
    E_WE_ERROR  eResult = E_WE_OK;
    WE_INT32  iFileSize = 0;
    WE_UINT8  *pucTempFile = NULL;
    WE_CHAR   *pcDir = NULL;
    WE_CHAR   *pcPrefix = NULL;
    
    if(NULL == hSecHandle)
    {
        return;
    }
    
    if (M_SEC_NON_VERIFIED_WTLS_TYPE == cCertType) 
    {
        pcDir = M_SEC_WTLS_NON_VERIFIED_CACERT_DIR;
        pcPrefix = M_SEC_WTLS_NON_VERIFIED_CACERT_PREFIX;
    }
    else if (M_SEC_NON_VERIFIED_X509_TYPE == cCertType)
    {
        pcDir = M_SEC_X509_NON_VERIFIED_CACERT_DIR;
        pcPrefix = M_SEC_X509_NON_VERIFIED_CACERT_PREFIX;
    }
    else 
    {
        Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        return ;
    }
    /* get the number of non-verified files.*/
    eRes = WE_FILE_GETSIZE_DIR(M_SEC_WIM_HWEHANDLE, pcDir, &(iNotVerCertNum)); 
    if ( eRes != E_WE_OK )
    {
        Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
        return;
    }

    if (0 == iNotVerCertNum)
    {        
        if (M_SEC_NON_VERIFIED_WTLS_TYPE == cCertType) 
        {
            Sec_WimCheckNonVerifiedCert(hSecHandle,iTargetID,M_SEC_NON_VERIFIED_X509_TYPE); 
            return;
        }
    }
    
    for (iIndexI=0; iIndexI<iNotVerCertNum; iIndexI++)
    {    
        eRes = WE_FILE_READ_DIR(M_SEC_WIM_HWEHANDLE, pcDir, iIndexI, acNameBuf, 20, (E_WE_FILE_TYPE*)(&iType), &lSize);
        if (eRes != E_WE_OK)
        {
            Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
            return;
        }
        /* get the name of non-verified file and open it.*/
        pcFileName = WE_SCL_STRCAT(pcPrefix, acNameBuf);  
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName, WE_FILE_SET_RDWR, &hFileHandle);
        if (eRes != E_WE_OK)
        {
            eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);
            if (E_WE_OK == eRes)
            {
                iIndexI--;
                iNotVerCertNum--;
            }
            WE_FREE(pcFileName);
            pcFileName = NULL;
            continue;
        }
        /* get size of non-verified file.*/
        eRes = WE_FILE_GETSIZE(M_SEC_WIM_HWEHANDLE, pcFileName, &(iFileSize));
        if (E_WE_OK != eRes)
        {
            eResult = WE_FILE_CLOSE(hFileHandle);
            WE_FREE(pcFileName);
            pcFileName = NULL;
            Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
            return;
        }
        if (0 == iFileSize) /*delete the file when it is none.*/
        {
            eResult = WE_FILE_CLOSE(hFileHandle);
            eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);
            if (E_WE_OK == eRes)
            {
                iIndexI--;
                iNotVerCertNum--;
            }
            WE_FREE(pcFileName);
            pcFileName = NULL;
            continue;
        }
        
        pucTempFile = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iFileSize * sizeof(WE_UCHAR));
        if (NULL == pucTempFile)
        {
            eResult = WE_FILE_CLOSE(hFileHandle);
            WE_FREE(pcFileName);
            pcFileName = NULL;
            Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
            return;
        }
        /* read out the file.*/        
        eRes = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET,&iNewPos);
        eRes = WE_FILE_READ(hFileHandle, pucTempFile, (WE_LONG)iFileSize, &lReadNum);
        eResult = WE_FILE_CLOSE(hFileHandle);
        if ((lReadNum < iFileSize) || (eRes != E_WE_OK))
        {
            WE_FREE(pucTempFile);
            WE_FREE(pcFileName);
            pcFileName = NULL;
            Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
            return;
        }
        
        iResult = Sec_WimCheckPreStoredRootCertVerify(hSecHandle, iTargetID,pucTempFile,(WE_UINT16)iFileSize, cCertType);
        WE_FREE(pucTempFile);
        if (M_SEC_ERR_OK == iResult )              /*Verify success*/
        {  
            St_SecWimPSRCertInfo *pstPSRCertInfo = NULL;
            pstPSRCertInfo = (St_SecWimPSRCertInfo *)WE_MALLOC(sizeof(St_SecWimPSRCertInfo));
            if (NULL == pstPSRCertInfo)
            {
                WE_FREE(pcFileName);
                pcFileName = NULL;
                Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                return;
            }
            pstPSRCertInfo->pcFileName = pcFileName;
            pstPSRCertInfo->iFileSize = iFileSize;
            pstPSRCertInfo->cCertType = cCertType;
            iResult =  Sec_WimAddNewElement(hSecHandle, iTargetID, M_SEC_WIM_CHECK_PRESTORED_ROOT_CERT, pstPSRCertInfo);
            if(iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pcFileName);
                WE_FREE(pstPSRCertInfo);
                Sec_WimInitializeResponse(hSecHandle, iTargetID, iResult);
            }
            return;
        }
        else        /*Verify failure,delete this file*/
        {
            eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);
            WE_FREE(pcFileName);
            pcFileName = NULL;
            if (E_WE_OK == eRes)
            {
                iIndexI--;
                iNotVerCertNum--;
            }
            if (iIndexI == (iNotVerCertNum-1))  /* check if all certs is verified. */
            {
                if (M_SEC_NON_VERIFIED_WTLS_TYPE == cCertType) 
                {
                    Sec_WimCheckNonVerifiedCert(hSecHandle,iTargetID,M_SEC_NON_VERIFIED_X509_TYPE); 
                    return;
                }
                else
                {
                    break;  /* finish */
                }
            }
            else
            {
                continue;
            }
        }   
    }
    
    if (M_SEC_NON_VERIFIED_WTLS_TYPE == cCertType) 
    {
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, M_SEC_NON_VERIFIED_X509_TYPE); 
        return;
    }
    else
    {         
        Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_OK);
    }     
    eResult = eResult;
}
/*====================================================================================
FUNCTION: 
    Sec_WimCheckPreStoredRootCertVerify  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Verify pre stored cert(puccert)
ARGUMENTS PASSED:  
    hSecHandle[IN/OUT]:         Global data handle.
    iTargetID[IN]:              object id.
    pucCert[IN]:                certificate.
    usCertLen[IN]:              length of certificate.  
    cCertType[IN]:              the type of CA certificate:
                                M_SEC_NON_VERIFIED_WTLS_TYPE,M_SEC_NON_VERIFIED_X509_TYPE             
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
static WE_INT32 Sec_WimCheckPreStoredRootCertVerify (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                                     WE_UINT8 *pucCert, WE_UINT16 usCertLen,
                                                     WE_CHAR cCertType)
{
    WE_INT32            iResult = 0;
    St_SecCertificate   stCert = {0};
    St_SecPubKeyRsa     stKey = {0};
    WE_UINT16           usRealCertLen = 0;
    WE_UINT8            *pucTmpCert=NULL;
    WE_INT32            iLoop=0;
    WE_UINT8            ucNumCerts=0;
    
    if((!hSecHandle) || (!pucCert))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }    
    
    pucTmpCert=(WE_UINT8 *)WE_MALLOC((usCertLen+1)*sizeof(WE_UINT8));
    if(!pucTmpCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    if (M_SEC_NON_VERIFIED_WTLS_TYPE == cCertType)     /*wtls:   add type*/
    {
        *pucTmpCert=M_SEC_CERTIFICATE_WTLS_TYPE;
    }
    else if(M_SEC_NON_VERIFIED_X509_TYPE == cCertType)  /*X509:    add type*/
    {
        *pucTmpCert=M_SEC_CERTIFICATE_X509_TYPE;        
    }
    else 
    {
        WE_FREE(pucTmpCert);
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;
    }
    
    for(iLoop=0;iLoop<usCertLen;iLoop++)
    {
        pucTmpCert[iLoop+1]=pucCert[iLoop];
    }
    
    iResult = Sec_WimVerifiCertChain(hSecHandle,pucTmpCert, usCertLen+1, NULL, &ucNumCerts);    
    WE_FREE(pucTmpCert);
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    if (ucNumCerts != 1)
    {        
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    usRealCertLen = 0;    
    if (M_SEC_NON_VERIFIED_WTLS_TYPE == cCertType)        /*wtls*/           
    {
        iResult = Sec_WtlsCertParse(hSecHandle,pucCert, &usRealCertLen, &stCert); 
        stKey.usExpLen = stCert.stCertUnion.stWtls.usExpLen;
        stKey.pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
        stKey.usModLen = stCert.stCertUnion.stWtls.usModLen;
        stKey.pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
        if (M_SEC_ERR_OK==iResult )
        {
            iResult = Sec_WimChkWTLSCert(hSecHandle,iTargetID, stKey, pucCert );
        }
    }
    else if (M_SEC_NON_VERIFIED_X509_TYPE == cCertType)     /*x509*/
    {
        iResult = Sec_X509CertParse(hSecHandle,pucCert, &usRealCertLen, &stCert);
        if (M_SEC_ERR_OK==iResult)         
        {  
            iResult= Sec_WimChkX509Cert(hSecHandle,iTargetID, stCert.stCertUnion.stX509.pucPublicKeyVal, \
                pucCert , M_SEC_WTLS_CONNECTION_MODE, 1,M_SEC_CERT_USAGE_ROOT);            
        }
    }   
    
    return iResult;  
   
}
/*====================================================================================
FUNCTION: 
    Sec_WimCheckNonVerifiedCertResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Check PreStored Root Cert.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:             Global data handle.
    pstE[IN]:                        certificate chain.
    pstParam[IN]:                    crpt verify signature.
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
=====================================================================================*/
void Sec_WimCheckNonVerifiedCertResp (WE_HANDLE hSecHandle,
                                      St_SecWimElement *pstE,
                                      St_SecCrptVerifySignatureResp *pstParam)
{      
    WE_INT32    iResult = 0;
    E_WE_ERROR  eRes = E_WE_OK;
    WE_INT32    iTargetID =0;
    WE_INT32    iNewPos =0;
    WE_UINT32   uiFileId =0;
    WE_HANDLE   hFileHandle =NULL;
    WE_CHAR     *pcFileName = NULL;
    WE_UINT32   uiFileSize =0;
    WE_INT8     cCertType = 0;
    WE_INT8     cCACertType = 0;
    WE_UINT8    *pucTempFile = NULL;
    WE_LONG     lWriteNum=0;
    WE_INT8     cNameType=0;
    WE_INT16    sCharSet=0;
    St_SecCertificate    stCert = {0};
    WE_LONG     lReadNum=0;
    St_SecWimPSRCertInfo *pstP=NULL;
    E_SecHashAlgType eHashAlg = E_SEC_ALG_HASH_SHA1;
    WE_UCHAR         aucDN[M_SEC_KEY_HASH_SIZE] = {0};
    WE_INT32         iDNLen = M_SEC_KEY_HASH_SIZE;
    WE_UINT8         aucSubjectHash[M_SEC_KEY_HASH_SIZE] = {0};    
    WE_INT32         iSubjectHashLen = M_SEC_KEY_HASH_SIZE;
    WE_UINT16        usIssuerLen = 0;
    WE_UINT16        usSubjectLen = 0;
    WE_UCHAR         ucAble = 0;
    
    if ((NULL == pstE) || (NULL == hSecHandle))
    {
        return;
    }
    pstP= pstE->pvPointer;
    pcFileName = pstP->pcFileName;
    uiFileSize = (WE_UINT32)(pstP->iFileSize);
    cCertType = pstP->cCertType;
    iTargetID = pstE->iTargetID;   
    WE_FREE(pstE); 
    WE_FREE(pstP);
    /*modified by Bird 070118 ,when malloc failed,do not continute to verify others certificate*/
    if(NULL == pstParam)
    {
        WE_FREE(pcFileName);
        Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
        return;
    }
    
    if ((M_SEC_NON_VERIFIED_WTLS_TYPE != cCertType)
        && (M_SEC_NON_VERIFIED_X509_TYPE != cCertType))
    {
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);       
        WE_FREE(pcFileName);
        Sec_WimInitializeResponse(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER);
        return;    
    }
    
    iResult = pstParam->sResult;    
    if (iResult != M_SEC_ERR_OK)
    {  
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);
        WE_FREE(pcFileName);
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName, WE_FILE_SET_RDWR, &hFileHandle);
    if (eRes != E_WE_OK)
    {
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);        
        WE_FREE(pcFileName);
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    pucTempFile = (WE_UCHAR *)WE_MALLOC(uiFileSize * sizeof(WE_UCHAR));
    if (NULL == pucTempFile)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);        
        WE_FREE(pcFileName);
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    /* read out the content and close and remove the non-verified file.*/
    eRes = WE_FILE_READ(hFileHandle, pucTempFile, (WE_LONG)uiFileSize, &lReadNum);
    if ((lReadNum < (WE_LONG)uiFileSize) || (eRes!= E_WE_OK))
    {
        WE_FREE(pucTempFile);     
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);         
        WE_FREE(pcFileName);         
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    /* remove the non verified file. */
    eRes = WE_FILE_CLOSE(hFileHandle);
    eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName); 
    WE_FREE(pcFileName);    
    pcFileName = NULL;
    
    /*Parse the certificate then stored into indexfile*/
    if (M_SEC_NON_VERIFIED_WTLS_TYPE ==  cCertType)            /*wtls*/
    {
        cCACertType = M_SEC_WTLS_CA_CERT;
        cNameType = M_SEC_CERT_NAME_WTLS;       
        ucAble = M_SEC_WTLS_CA_CERT_ENABLE;
        iResult = Sec_WtlsCertParse(hSecHandle,pucTempFile, (WE_UINT16 *)(&uiFileSize), &stCert);
        if (iResult != M_SEC_ERR_OK)
        {
            WE_FREE(pucTempFile);             
            Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
            return;
        }
        usSubjectLen = stCert.stCertUnion.stWtls.ucSubjectLen;
        usIssuerLen = stCert.stCertUnion.stWtls.ucIssuerLen;
        sCharSet = (WE_INT16)(stCert.stCertUnion.stWtls.usSubjectcharacterSet);
    }
    else   /*x509*/
    {
        WE_UINT16 usCertLen   = 0;
        WE_UINT16 usStepLen   = 0;
        
        cCACertType = M_SEC_X509_CA_CERT;
        sCharSet = -1;
        cNameType = M_SEC_CERT_NAME_X509;  
        ucAble = M_SEC_WTLS_CA_CERT_DISABLE;
        iResult = Sec_X509CertParse(hSecHandle, pucTempFile, (WE_UINT16 *)(&uiFileSize), &stCert); 
        if (iResult != M_SEC_ERR_OK)
        {   
            WE_FREE(pucTempFile);             
            Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
            return;
        }
        usCertLen = 0;
        iResult = Sec_X509GetCertFieldLength(hSecHandle, pucTempFile, &usCertLen, &usStepLen);
        if (iResult != M_SEC_ERR_OK)
        {   
            WE_FREE(pucTempFile);             
            Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
            return;
        }
        uiFileSize = usCertLen + usStepLen;
        usSubjectLen = stCert.stCertUnion.stX509.usSubjectLen;
        usIssuerLen = stCert.stCertUnion.stX509.usIssuerLen;
    }    
    /* hash the issuer. */
    iResult = Sec_LibHash(hSecHandle, eHashAlg, stCert.pucIssuer, usIssuerLen, aucDN, &(iDNLen));
    if (iResult != M_SEC_ERR_OK)
    { 
        WE_FREE(pucTempFile);           
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    /* hash subject. */
    iResult = Sec_LibHash(hSecHandle, eHashAlg, stCert.pucSubject, 
        usSubjectLen, aucSubjectHash, &iSubjectHashLen);
    if (iResult != M_SEC_ERR_OK)
    { 
        WE_FREE(pucTempFile);           
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }    
    /*creat a CA certificate and open it.*/
    iResult = Sec_WimMakeFile(hSecHandle, cCACertType, &hFileHandle, &uiFileId);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucTempFile);            
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    eRes = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET, &(iNewPos));
    /* write in CA file.*/
    eRes = WE_FILE_WRITE(hFileHandle, pucTempFile, (WE_LONG)uiFileSize, &lWriteNum);             
    if ((lWriteNum < (WE_LONG)uiFileSize) || (eRes!= E_WE_OK))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);  
        /* if not write in, delete the CA certificate.*/
        iResult = Sec_WimAcqFileName(cCACertType, uiFileId, &pcFileName);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);
        WE_FREE(pcFileName);
        WE_FREE(pucTempFile);            
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    eRes = WE_FILE_CLOSE(hFileHandle); 
    /* add in CA IndexFile.*/   
    iResult = Sec_WimAddBlock(hSecHandle, cCACertType, uiFileId, uiFileSize,
        aucSubjectHash, aucDN, cNameType, sCharSet,           
        usSubjectLen, stCert.pucSubject, ucAble);
    if (iResult!= M_SEC_ERR_OK)
    {
        iResult = Sec_WimAcqFileName(cCACertType, uiFileId, &pcFileName);
        eRes = WE_FILE_REMOVE(M_SEC_WIM_HWEHANDLE, pcFileName);
        WE_FREE(pcFileName);
        Sec_WimCheckNonVerifiedCert(hSecHandle, iTargetID, cCertType);
        return;
    }
    WE_FREE(pucTempFile);
    /* continue check. */    
    Sec_WimCheckNonVerifiedCert (hSecHandle,iTargetID,cCertType); 
}

/*====================================================================================
FUNCTION: 
    Sec_WimChkWTLSCert  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Verify WTLS Certificate .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    iWid[IN]: The value of the wid.
    stKey[IN]:      stkey        
    pucCert[IN]:  The Certificate which wanted to be verified.             
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimChkWTLSCert (WE_HANDLE hSecHandle,WE_INT32 iTargetID, St_SecPubKeyRsa stKey, 
                                const WE_UINT8 *pucCert)
{
   WE_UINT8    *pucWtlSCert=NULL;
   WE_UINT16   usCertLen=0;
   WE_INT32    iResult=0;
   WE_UINT32   uiNow=0;
   St_SecCertificate stCert={0};
   St_SecCrptPubKey stPubKey={0};
   if (!pucCert)
   {
       return M_SEC_ERR_INVALID_PARAMETER;
   }
   pucWtlSCert = (WE_UINT8*)pucCert;
   iResult = Sec_WtlsCertParse(hSecHandle,pucWtlSCert, &usCertLen, &stCert);
   if (iResult != M_SEC_ERR_OK)
   {
       return iResult;
   }
   uiNow = SEC_CURRENT_TIME();
   if ((uiNow < stCert.iValidNotBefore) || (stCert.iValidNotAfter < uiNow)) 
   {
       return M_SEC_ERR_CERTIFICATE_EXPIRED;
   }
   stPubKey = stKey;
   return Sec_WimVerifyHashedSign (hSecHandle,(WE_INT32)E_SEC_ALG_HASH_SHA1, iTargetID, stPubKey, pucWtlSCert, \
                                  (usCertLen - (stCert.usSignatureLen) - 2),\
                                  stCert.pucSignature, stCert.usSignatureLen);
}
/*====================================================================================
FUNCTION: 
    Sec_WimChkX509Cert  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Verify X509 Certificate .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    iTargetID[IN]:   The value of the iTargetID.            
    pucPublicKeyVal[IN]:   public key. 
    pucCert[IN]:           certificate.
    iConnectionType[IN]:   Connection type.
    iChainDepth[IN]:       chain depth.
    iCertUsage[IN]:        the usage of cert.   
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimChkX509Cert (WE_HANDLE hSecHandle,WE_INT32 iTargetID, WE_UINT8 *pucPublicKeyVal,
                             const WE_UINT8 *pucCert, WE_INT32 iConnectionType,
                             WE_INT32 iChainDepth, WE_INT32 iCertUsage)
{
    WE_UINT8        *pucX509cert=NULL;
    WE_UINT16       usCertLen=0;
    WE_INT32        iResult = M_SEC_ERR_OK;
    WE_UINT32       uiNow=0;
    St_SecCertificate stSpCert={0};
    St_SecPubKeyRsa  stKey={0};
    WE_UINT16       usTBSCertLen = 0;
    WE_UINT16       usJumpStep = 0; 
    WE_INT32        iTmpCertUsage=0;
    WE_INT32        iExtnId = 0;
    WE_INT32        iCritical = 0;
    WE_UINT8        *pucExtnValue = 0;
    WE_UINT16       usExtnValueLen = 0;
    WE_UINT8        ucKeyUsageBit0To7 = 0; 
    WE_UINT8        ucKeyUsageBit8To15 = 0;
    WE_INT32        iAnyExtendedKeyUsage = 0;
    WE_INT32        iServerAuth = 0;
    WE_INT32        iClientAuth = 0;
    WE_INT32        iCodeSigning = 0;
    WE_INT32        iEmailProtection = 0;
    WE_INT32        iTimeStamping = 0;
    WE_INT32        iOCSPSigning = 0; 
    WE_INT32        iCA=0;
    WE_INT32        iHasPathLenConstraint=0;
    WE_UINT8        ucPathLenConstraint=0; 
    WE_INT32        iBasicConstraintCa = 0;
    St_SecCrptPubKey stPubKey={0};
    WE_INT32        iHashAlg=0;
    
    iConnectionType=iConnectionType; 
    if (pucCert == NULL)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pucX509cert = (WE_UINT8*)pucCert;
    usCertLen = 0;
    
    iResult = Sec_X509CertParse(hSecHandle,pucX509cert, &usCertLen, &stSpCert); 
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    uiNow = SEC_CURRENT_TIME();
    if ((uiNow < stSpCert.iValidNotBefore) || (stSpCert.iValidNotAfter < uiNow)) 
    {
        return M_SEC_ERR_CERTIFICATE_EXPIRED;
    }
    usTBSCertLen = (WE_UINT16)stSpCert.stCertUnion.stX509.uiTbsCertLen;
    
    if (stSpCert.ucCertificateVersion == 3) 
    { 
        if (iChainDepth>1)
        {         
            iTmpCertUsage = M_SEC_CERT_USAGE_CA;    
        }     
        else
        {
            iTmpCertUsage = iCertUsage;
        }
        while (stSpCert.stCertUnion.stX509.usExtensionsLen > 0)
        {
            iResult = Sec_X509GetCertExtension(hSecHandle,stSpCert.stCertUnion.stX509.pucExtensions, \
                &iExtnId, &iCritical,&pucExtnValue,&usExtnValueLen, &usJumpStep);
            if (iResult != M_SEC_ERR_OK)
            {
                return iResult;
            }
            switch (iExtnId)
            {  
                case M_SEC_CERT_EXT_KEY_USAGE:
                    {  
                        iResult = Sec_X509GetKeyUsage(hSecHandle,pucExtnValue, &ucKeyUsageBit0To7, 
                            &ucKeyUsageBit8To15);                 
                        if (iResult != M_SEC_ERR_OK)              
                        {  
                            return iResult;  
                        }  
                        if (iTmpCertUsage == M_SEC_CERT_USAGE_CA ||iTmpCertUsage == M_SEC_CERT_USAGE_ROOT) 
                        {  
                            if ( !(ucKeyUsageBit0To7 & 0x4) )   
                            {     
                                return M_SEC_ERR_BAD_CERTIFICATE;   
                            } 
                        }
                    }       
                    break;    
                case M_SEC_CERT_EXT_EXTENDED_KEY_USAGE:    
                    if (M_SEC_CERT_USAGE_SERVER_AUTH == iTmpCertUsage) 
                    {    
                        iResult = Sec_X509GetKeyUsageEx(hSecHandle,pucExtnValue, &iAnyExtendedKeyUsage,\
                                            &iServerAuth, &iClientAuth, &iCodeSigning,&iEmailProtection, \
                                            &iTimeStamping,&iOCSPSigning);   
                        if (iResult != M_SEC_ERR_OK)  
                        {   
                            return iResult;   
                        }             
                        if (!(iServerAuth || iAnyExtendedKeyUsage)) 
                        {  
                            return M_SEC_ERR_BAD_CERTIFICATE; 
                        }    
                    }    
                    break;
                case M_SEC_CERT_EXT_BASIC_CONSTRAINTS: 
                    if (M_SEC_CERT_USAGE_CA == iTmpCertUsage||M_SEC_CERT_USAGE_ROOT == iTmpCertUsage) 
                    {  
                        iResult = Sec_X509GetBasicConstraint(hSecHandle,pucExtnValue, &iCA, 
                                                          &iHasPathLenConstraint,&ucPathLenConstraint); 
                        if (iResult != M_SEC_ERR_OK)  
                        {             
                            return iResult;  
                        }          
                        if (!iCA|| (iHasPathLenConstraint && (ucPathLenConstraint + 2 < iChainDepth)))  
                        {       
                            return M_SEC_ERR_BAD_CERTIFICATE;  
                        } 
                        else   
                        {     
                            iBasicConstraintCa = 1; 
                        }    
                    }      
                    break;
                case M_SEC_CERT_EXT_DOMAIN_INFORMATION:
                case M_SEC_CERT_EXT_CERTIFICATE_POLICIES:   
                case M_SEC_CERT_EXT_SUBJECT_ALT_NAME:        
                case M_SEC_CERT_EXT_NAME_CONSTRAINTS:        
                case M_SEC_CERT_EXT_POLICY_CONSTRAINTS:      
                case M_SEC_CERT_EXT_AUTHORITY_KEY_IDENTIFIER:
                case M_SEC_CERT_EXT_SUBJECT_KEY_IDENTIFIER:  
                    break;    
                default: 
                    if (iCritical)    
                    { 
                        return M_SEC_ERR_BAD_CERTIFICATE;  
                    }   
            }     
            stSpCert.stCertUnion.stX509.pucExtensions += usJumpStep;
            stSpCert.stCertUnion.stX509.usExtensionsLen = 
                (WE_UINT16)(stSpCert.stCertUnion.stX509.usExtensionsLen - usJumpStep);    
        }
        if ((M_SEC_CERT_USAGE_CA == iTmpCertUsage || M_SEC_CERT_USAGE_ROOT == iTmpCertUsage) && !iBasicConstraintCa) 
        {  
            return M_SEC_ERR_BAD_CERTIFICATE;  
        }  
    }   
    
    if ((stSpCert.stCertUnion.stX509.pucSignatureAlgId[stSpCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 5) || \
        (stSpCert.stCertUnion.stX509.pucSignatureAlgId[stSpCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 4) || \
        (stSpCert.stCertUnion.stX509.pucSignatureAlgId[stSpCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 2)  )     
    { 
        iResult = Sec_X509GetRsaModAndExp(hSecHandle,(const WE_UINT8 *)pucPublicKeyVal,
            &(stKey.pucModulus), &(stKey.usModLen), 
            &(stKey.pucExponent), &(stKey.usExpLen));
        if (M_SEC_ERR_OK == iResult)
        {     
            stPubKey = stKey;
            if (stSpCert.stCertUnion.stX509.pucSignatureAlgId[stSpCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 5)
            {
                iHashAlg = (WE_INT32)E_SEC_ALG_HASH_SHA1;
            }
            /*#ifdef M_SEC_CFG_MD2*/
            else if (stSpCert.stCertUnion.stX509.pucSignatureAlgId[stSpCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 2)
            {
                iHashAlg = (WE_INT32)E_SEC_ALG_HASH_MD2;
            }
            /* #endif*/
            else
            {
                iHashAlg = (WE_INT32)E_SEC_ALG_HASH_MD5;
            }            
            iResult = Sec_WimVerifyHashedSign (hSecHandle,iHashAlg, iTargetID, stPubKey, \
                     pucX509cert+4, usTBSCertLen, stSpCert.pucSignature, stSpCert.usSignatureLen);    
        }     
    }  
    else     
    {         
        return M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE;     
    }
    if (iResult != M_SEC_ERR_OK)
    {
        return M_SEC_ERR_BAD_CERTIFICATE;
    }
    else
    {
        return M_SEC_ERR_OK;
    }   
}

/*====================================================================================
FUNCTION: 
    Sec_WimViewCertName  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    get all CA or User certificate's name.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    pcFileName[IN]:       Filename:M_SEC_USERCERT_INDEXFILE_NAME,M_SEC_CA_CERT_INDEXFILE_NAME          
    piCertId[IN]:         certificate Id.
    ppucCertFriendlyName[IN]:   friendly name of certificate.        
    psCharSet[IN]:   char set of certificate.
    pcNameType[IN]:  name type of certificate.     
    pcCertNum[IN]:   Number of certificate.       
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimViewCertName(WE_HANDLE hSecHandle,
           WE_CHAR *pcFileName, WE_UINT8 *pucCertType, WE_INT32 *piCertId, WE_UINT8 **ppucCertFriendlyName, 
           WE_UINT16 *pusFriendlyNameLen, WE_INT16 *psCharSet, WE_INT8 *pcNameType,
           WE_INT8 *pcCertNum, WE_UCHAR *pucAble)
{
    WE_UINT8   *pucIndexFile = NULL;
    WE_INT32   iSizeOfIndexFile = 0;
    WE_UINT8   *pucP = NULL;
    WE_UINT8   *pucTmp = NULL;
    E_WE_ERROR  eResult = E_WE_OK;
    WE_INT32   iLoop = 0;    
    WE_LONG    lReadNum = 0;
    WE_UINT8   ucFileNum=0;
    WE_HANDLE  hFileHandle = NULL;
    WE_UINT8   ucCertType = 0;
    WE_INT32   iBlockLen = 0;
    WE_INT32   iResult = 0;
    WE_INT32   iIndexJ = 0;    
    
    if((!ppucCertFriendlyName)||(!hSecHandle)||(!psCharSet)|| (!pucCertType) ||
        (!pcNameType)||(!pusFriendlyNameLen)||(!piCertId)||(!pcCertNum))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    eResult = WE_FILE_GETSIZE (M_SEC_WIM_HWEHANDLE, pcFileName, &iSizeOfIndexFile);
    if ( eResult != E_WE_OK)
    { 
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eResult = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName, WE_FILE_SET_RDWR, &hFileHandle);
    if ( eResult != E_WE_OK )
    { 
        return M_SEC_ERR_GENERAL_ERROR;
    }
    pucIndexFile = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(iSizeOfIndexFile+1)*sizeof(WE_UINT8));
    if (NULL == pucIndexFile)
    {
        eResult = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    eResult = WE_FILE_READ(hFileHandle, pucIndexFile, iSizeOfIndexFile,&lReadNum);
    if ((lReadNum < (WE_INT32)iSizeOfIndexFile) || ( eResult != E_WE_OK ))
    {    
        eResult = WE_FILE_CLOSE(hFileHandle);
        WE_FREE(pucIndexFile);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    eResult = WE_FILE_CLOSE(hFileHandle);
    
    pucP = pucIndexFile;    
    ucFileNum = *pucP;
    pucP++;
    for (iLoop=0; iLoop<ucFileNum; iLoop++)
    {
        pucTmp = pucP;
        ucCertType = *pucTmp;
        pucTmp++;
        if (M_SEC_PUBKEY_CERT == ucCertType)
        {
            iResult = Sec_WimGetBlockLength(pucP, (WE_CHAR)ucCertType, &iBlockLen);
            if (iResult != M_SEC_ERR_OK)
            {  
                for (iIndexJ=0; iIndexJ<iLoop; iIndexJ++)
                {
                    WE_FREE(ppucCertFriendlyName[iIndexJ]);
                }
                WE_FREE(pucIndexFile);
                return M_SEC_ERR_GENERAL_ERROR;
            }
            pucP += iBlockLen;
            iLoop--;   /* for not increase the cert number. */
            ucFileNum--;
            continue;
        }
        pucCertType[iLoop] = ucCertType;
        /*get file id*/
        Sec_StoreStrUint8to32(pucTmp, (WE_UINT32 *)(&piCertId[iLoop]));
        pucTmp += 4;
        pucTmp += 4 + M_SEC_DN_HASH_LEN + M_SEC_DN_HASH_LEN;
        pcNameType[iLoop] = (WE_CHAR)(*pucTmp++);
        Sec_StoreStrUint8to16(pucTmp, (WE_UINT16 *)(&(psCharSet[iLoop])));
        pucTmp += 2;
        if (0 == SEC_STRCMP(pcFileName, M_SEC_CA_CERT_INDEXFILE_NAME))
        {
            pucAble[iLoop] = *pucTmp++;
        }
        else
        {
            pucAble[iLoop] = 0;
        }
        
        Sec_StoreStrUint8to16(pucTmp, &(pusFriendlyNameLen[iLoop]));
        pucTmp += 2;
        ppucCertFriendlyName[iLoop] = (WE_UINT8 *)WE_MALLOC(pusFriendlyNameLen[iLoop] * sizeof(WE_UINT8));
        if (NULL == ppucCertFriendlyName[iLoop])
        { 
            for (iIndexJ=0; iIndexJ<iLoop; iIndexJ++)
            {
                WE_FREE(ppucCertFriendlyName[iIndexJ]);
            }
            WE_FREE(pucIndexFile);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;            
        }
        (WE_VOID)MEMCPY(ppucCertFriendlyName[iLoop], pucTmp, pusFriendlyNameLen[iLoop]);
        iResult = Sec_WimGetBlockLength(pucP, (WE_CHAR)ucCertType, &iBlockLen);
        if (iResult != M_SEC_ERR_OK)
        {  
            for (iIndexJ=0; iIndexJ<iLoop; iIndexJ++)
            {
                WE_FREE(ppucCertFriendlyName[iIndexJ]);
            }
            WE_FREE(pucIndexFile);
            return M_SEC_ERR_GENERAL_ERROR;
        }
        pucP += iBlockLen;
    }
    
    *pcCertNum = (WE_CHAR)ucFileNum;
    WE_FREE(pucIndexFile);
    return M_SEC_ERR_OK;      
}
/*====================================================================================
FUNCTION: 
    Sec_WimVerifiCertChain  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    check certificate:include type,version,length,Algorithm.....   
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    pucChain[IN]:    certificate.
    iChainLen[IN] :  The length of  pucChain.
    pstCerts[IN]:    cert .
    pucNumCerts[OUT]:    number of cert. 
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimVerifiCertChain(WE_HANDLE hSecHandle,
                                const WE_UINT8 *pucChain, 
                                WE_INT32 iChainLen, 
                                St_SecAsn1Certificate *pstCerts, 
                                WE_UINT8 *pucNumCerts)
{
    WE_UINT8  *pucCert=NULL;
    WE_UINT16 usCertLen=0;
    WE_UINT16 usAccLen=0;
    WE_UINT8  ucFormat=0;
    WE_UINT8  ucLocalNumCerts = 0;
    WE_INT32  iResult=0;
    WE_UINT16 usJumpStep = 0;
    
    if ((NULL==pucChain )||(NULL==pucNumCerts))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (iChainLen < 3)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucCert = (WE_UINT8*)pucChain;
    do {     
        usCertLen = 0;
        ucFormat = (*pucCert);
        pucCert++;
        usAccLen++;
        if (M_SEC_CERTIFICATE_WTLS_TYPE==ucFormat)                    /*wtls*/
        {  
            /*check wtls certificate length and format */                                         
            iResult = Sec_WtlsGetCertFieldLength(hSecHandle,pucCert, &usCertLen);
        }
        else if (M_SEC_CERTIFICATE_X509_TYPE==ucFormat)             /*x509*/
        {            
            iResult = Sec_X509GetCertFieldLength(hSecHandle,pucCert, &usCertLen, &usJumpStep);
            usCertLen += usJumpStep;                
        }        
        else                                        
        {              
            return M_SEC_ERR_INVALID_PARAMETER;
        } 
        if (iResult != M_SEC_ERR_OK)         
        {                  
            return iResult;          
        }
        
        if (pstCerts != NULL) 
        {       
            if (ucLocalNumCerts >= *pucNumCerts)  
            {
                return M_SEC_ERR_INVALID_PARAMETER;
            }
            pstCerts[ucLocalNumCerts].ucFormat = ucFormat;
            pstCerts[ucLocalNumCerts].pucCert =  pucCert;
            pstCerts[ucLocalNumCerts].uiCertLen =  usCertLen;
        }
        ucLocalNumCerts++;
        usAccLen += usCertLen;
        pucCert += usCertLen;    
    }    
    while (usAccLen <iChainLen); 
    
    if (usAccLen != iChainLen)      
    {          
        return M_SEC_ERR_INVALID_PARAMETER;      
    }      
    *pucNumCerts = ucLocalNumCerts;      
    return M_SEC_ERR_OK;
}

/*====================================================================================
FUNCTION: 
    Sec_WimEncryptDataToFile  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    after Encrypt File Data,then write Encrypted data to file.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:       Global data handle.
    hFileHandle[IN]:          filehandle
    pvData[IN]:               the  data need to be Encrypted 
    iPos[IN]:                 offset
    iDataLen[IN]:             length of pvdata
    piWriteLength[OUT]:       the returned length writed to file
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimEncryptDataToFile(WE_HANDLE hSecHandle,WE_HANDLE hFileHandle, 
                                         void *pvData, WE_INT32 iPos, WE_INT32 iDataLen,
                                         WE_INT32 *piWriteLength)
{
    WE_INT32 iResult=0;
    E_WE_ERROR eRes = E_WE_OK;
    WE_UINT8 *pucEncryptedData=NULL;
    WE_INT32 iEncryptedDataLen = 0;
    WE_INT32 iNewPos = 0;
    if ((NULL == hSecHandle) || (NULL == piWriteLength))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }        
    pucEncryptedData = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(iDataLen+10)*sizeof(WE_UINT8));
    if (NULL == pucEncryptedData)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    iEncryptedDataLen = iDataLen+10;
    /*Encrypt data*/
    iResult = Sec_WimEncryptData(hSecHandle,pvData, iDataLen, pucEncryptedData, &iEncryptedDataLen);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucEncryptedData);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_SEEK(hFileHandle, iPos, WE_FILE_SEEK_SET,&iNewPos);
    /*write the Encrypted Data to file */
    eRes = WE_FILE_WRITE(hFileHandle, (void*)pucEncryptedData, iEncryptedDataLen,piWriteLength);
    if (((*piWriteLength) < (WE_INT32)iEncryptedDataLen) || ( eRes != E_WE_OK ))
    {
        WE_FREE(pucEncryptedData);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    WE_FREE(pucEncryptedData);
    return (WE_INT32)eRes;
}
/*====================================================================================
FUNCTION: 
    Sec_WimDecryptDataFromFile 
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Read data from file from the positon of offset ,then Decrypted the data.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:       Global data handle.
    hFileHandle[IN]:          filehandle.
    pucDecryptedData[OUT]:    the decrypted data with padded data.
    iPos[IN]:                 the offset position in hFileHandle.
    iDataLen[IN]:             length of data wanted to read from the file. 
    piReaderLength[OUT]:      length of  pucDecryptedData.
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimDecryptDataFromFile(WE_HANDLE hSecHandle,WE_HANDLE hFileHandle, 
                                        WE_UINT8 *pucDecryptedData, WE_INT32 iPos, 
                                        WE_INT32 iDataLen,WE_INT32 *piReaderLength)
{
    WE_INT32     iResult=0;
    E_WE_ERROR   eRes = E_WE_OK;
    WE_UINT8     *pucData=NULL;
    WE_INT32     iNewPos=0;
    WE_INT32     iDecDataLen=0;
    WE_LONG      lReadNum=0;
      
    if ((0 == iDataLen)||(NULL==piReaderLength))
    { 
        return M_SEC_ERR_INVALID_PARAMETER;
    }    
    pucData = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(iDataLen+1)*sizeof(WE_UINT8));    
    if(!pucData)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    eRes = WE_FILE_SEEK(hFileHandle, iPos, WE_FILE_SEEK_SET,&iNewPos);
    eRes = WE_FILE_READ(hFileHandle, pucData, iDataLen,&lReadNum);
    if((lReadNum<(WE_INT32)iDataLen)||(eRes !=E_WE_OK))
    {
        WE_FREE(pucData);
        return (WE_INT32)eRes;
    }
    pucData[iDataLen] = '\0';
    iDecDataLen=lReadNum;
    /*Decrypt Data which from file*/
    iResult = Sec_WimDecryptData(hSecHandle,pucData, iDataLen, pucDecryptedData,&iDecDataLen);
    *piReaderLength=iDecDataLen;
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucData);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    WE_FREE(pucData);
    return iResult;
}
/*====================================================================================
FUNCTION: 
    Sec_WimGenCipherKey  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    generate Cipher Key
ARGUMENTS PASSED:
     pucKey[OUT]:    Generated key used to  encrypt private key and pin.
     iKeyLen[IN]:    length of pucKey.
     pucIv[OUT]:     IV    
     iIvLen[IN]:     length of IV
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGenCipherKey(WE_UINT8 *pucKey, WE_INT32 iKeyLen ,WE_UINT8 *pucIv ,WE_INT32 iIvLen )
{
    WE_INT32    iLoop = 0;

    if(!pucKey || (iKeyLen < 2) || !pucIv || (iIvLen < 2))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if(iKeyLen >= iIvLen)
    {
        pucKey[0] = (WE_UCHAR)((iKeyLen - iIvLen)%256);
    }
    else
    {
        pucKey[0] = (WE_UCHAR)((iKeyLen - iIvLen)%256);
    }
    
    pucKey[1] = (WE_UCHAR)((iKeyLen + iIvLen)%256);

    for(iLoop = 2; iLoop < iKeyLen; iLoop++)
    {
        pucKey[iLoop] = (pucKey[iLoop-1] + pucKey[iLoop-2])%256;
    }
    pucIv[0] = pucKey[iKeyLen-2];
    pucIv[1] = pucKey[iKeyLen-1];
    for(iLoop = 2; iLoop < iIvLen; iLoop++)
    {
        pucIv[iLoop] = (pucIv[iLoop-1] + pucIv[iLoop-2])%256;
    }

    return M_SEC_ERR_OK;

}
/*====================================================================================
FUNCTION: 
    Sec_WimEncryptData  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Encrypt File Data
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:     Global data handle.
    pucData[IN]:            the data to be  encrypted.     
    iDataLen[IN]:           length of pucData.    
    pucEncryptedData[OUT]:  encrypted data  
    piEncryptedDataLen[OUT]:length of pucEncryptedData
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimEncryptData(WE_HANDLE hSecHandle,WE_UINT8 *pucData, WE_INT32 iDataLen,
                            WE_UINT8 *pucEncryptedData, WE_INT32 *piEncryptedDataLen)
{
    WE_INT32            iResult=0;
    St_SecCipherCrptKey stkeyObj={0};
    WE_UINT8            *pucKey=NULL;
    WE_INT32            iLoop=0;
    WE_INT32            iLenWithPad = 0;
    WE_UINT8            *pucPaddedData=NULL;

    stkeyObj.iKeyLen = 24;
    stkeyObj.iIvLen  = 8;
 
    if ((!pucData) || (!pucEncryptedData) || (!piEncryptedDataLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (0==iDataLen)
    {
        *piEncryptedDataLen = 0;
        return M_SEC_ERR_OK;
    }

    pucKey = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(stkeyObj.iKeyLen+stkeyObj.iIvLen)*sizeof(WE_UINT8));
    if(!pucKey)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    iResult=Sec_WimGenCipherKey(pucKey,stkeyObj.iKeyLen,&pucKey[stkeyObj.iKeyLen],stkeyObj.iIvLen);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucKey);
        return iResult;
    }
    stkeyObj.pucKey = pucKey;                
    stkeyObj.pucIv = &pucKey[stkeyObj.iKeyLen]; 

    iLenWithPad = iDataLen +2;            
    if( (iLenWithPad %8) != 0 )       
    {
        iLenWithPad += 8 -(iLenWithPad %8);
    }
    *piEncryptedDataLen = iLenWithPad;
  
    pucPaddedData = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(iLenWithPad)*sizeof(WE_UINT8));
    if (NULL == pucPaddedData)
    {       
        WE_FREE(pucKey);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    for(iLoop=0; iLoop<iDataLen; iLoop++)
    {
        pucPaddedData[iLoop] = pucData[iLoop];
    }
    /*Padded with 1 or 0*/
    pucPaddedData[iDataLen] = SEC_FIRST_PAD_VALUE;
    pucPaddedData[iDataLen +1] = SEC_SECOND_PAD_VALUE;

    for(iLoop=(iDataLen +2); iLoop<iLenWithPad; iLoop++)
    {
        pucPaddedData[iLoop] = SEC_SECOND_PAD_VALUE;
    }

    iResult = Sec_LibCipherEncrypt(hSecHandle, E_WE_ALG_CIPHER_3DES_CBC/*E_SEC_BULK_3DES_CBC_EDE*/, 
                                   stkeyObj, pucPaddedData, (WE_UINT32)iLenWithPad,
                                   pucEncryptedData,(WE_UINT32*)piEncryptedDataLen);

    WE_FREE(pucKey);
    WE_FREE(pucPaddedData);
    return iResult;
}
/*====================================================================================
FUNCTION: 
    Sec_WimDecryptData  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Decrypt File Data 
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    pucData[IN]:          Encrypted data
    iDataLen[IN]:         length of Encrypted data
    pucDecryptedData[OUT]:Decrypted data
    piDecDataLen[OUT]:    length of Decryped data
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimDecryptData(WE_HANDLE hSecHandle,WE_UINT8 *pucData, 
                            WE_INT32 iDataLen, WE_UINT8 *pucDecryptedData,WE_INT32 *piDecDataLen)
{
    WE_INT32            iResult=0;
    St_SecCipherCrptKey stKeyObj={0};
    WE_UINT8            *pucKey=NULL;

    stKeyObj.iKeyLen = 24;
    stKeyObj.iIvLen  = 8;

    if(iDataLen == 0)
    {
        return M_SEC_ERR_OK;
    }
    if(( NULL==pucData) || (NULL==pucDecryptedData ) )
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pucKey = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(stKeyObj.iKeyLen+stKeyObj.iIvLen)*sizeof(WE_UINT8));
    if(!pucKey )
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
 
    iResult=Sec_WimGenCipherKey(pucKey,stKeyObj.iKeyLen,&pucKey[stKeyObj.iKeyLen],stKeyObj.iIvLen);
    if (iResult != M_SEC_ERR_OK)
    {  
        WE_FREE(pucKey);
        return iResult;
    }
    stKeyObj.pucKey = pucKey;        
    stKeyObj.pucIv = &pucKey[stKeyObj.iKeyLen]; 

    /*Decrypt data*/
    iResult = Sec_LibCipherDecrypt(hSecHandle,E_WE_ALG_CIPHER_3DES_CBC/*E_SEC_BULK_3DES_CBC_EDE*/,
        stKeyObj, pucData, (WE_UINT32)iDataLen, pucDecryptedData,(WE_UINT32 *)piDecDataLen);

    WE_FREE(pucKey);
    return iResult;  
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetX509Sub  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Parse X509 certificate,get Subject.  
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    pucX509Cert[IN]:            X509 certificate.
    ppucSubject[OUT]:            Subject
    pusSubjectLen[OUT]:         the length of ppucSubject.
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code. 
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetX509Sub(WE_HANDLE hSecHandle,WE_UINT8 *pucX509Cert, 
                               WE_UINT8 **ppucSubject,WE_UINT16 *pusSubjectLen)
{
    WE_INT32 iResult=0;
    St_SecCertificate stCert={0};
    WE_UINT16 usCertLen = 0;
    if((NULL==hSecHandle)||(NULL==pucX509Cert)||(NULL==ppucSubject)||(NULL==pusSubjectLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iResult = Sec_X509CertParse(hSecHandle,pucX509Cert, &usCertLen, &stCert);    
    *ppucSubject = stCert.pucSubject;
    *pusSubjectLen = stCert.stCertUnion.stX509.usSubjectLen;
    return iResult;
}

/*====================================================================================
FUNCTION: 
    Sec_WimGetX509PubKey  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get X509 RSA publicKey
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    pucX509Cert[IN]:            certificate
    pusExpLen[OUT]:            the length of Exponent.
    ppucExponent[OUT}:        Exponent.
    pusModLen[OUT]:            the length of Modulus.
    ppucModulus[OUT]:        Modulus.
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code. 
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetX509PubKey(WE_HANDLE hSecHandle,WE_UINT8 *pucX509Cert, 
                              WE_UINT16 *pusExpLen, WE_UINT8** ppucExponent,
                              WE_UINT16 *pusModLen, WE_UINT8** ppucModulus)
{
    WE_INT32 iResult=0;
    WE_UINT16 usCertLen = 0;
    St_SecCertificate stCert={0};
    iResult = Sec_X509CertParse(hSecHandle,pucX509Cert, &usCertLen, &stCert);    
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    if (stCert.stCertUnion.stX509.pucPublicKeyAlgId[stCert.stCertUnion.stX509.usPublicKeyAlgIdLen-2] == 1)
    {
        iResult = Sec_X509GetRsaModAndExp (hSecHandle,stCert.stCertUnion.stX509.pucPublicKeyVal,
                                      ppucModulus, pusModLen, ppucExponent, pusExpLen);
    }
    else 
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    return iResult;
  
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetPubKeyHash  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    get public key hash.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    iCertificateId[IN]:         certificate FileId
    ppucKeyId[OUT]:              publicKeyHash
    piKeyIdLen[OUT]:            length of publicKeyHash
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetPubKeyHash(WE_HANDLE hSecHandle,WE_INT32 iCertificateId, 
                              WE_UINT8 **ppucKeyId, WE_INT32 *piKeyIdLen)
{
   WE_INT32 iJLoop=0;
   WE_INT32 iLoop=0;
   if((!hSecHandle)||(!ppucKeyId)||(!piKeyIdLen))
   {
       return M_SEC_ERR_INVALID_PARAMETER;
   }
   
   for (iJLoop=0; iJLoop<M_SEC_USER_CERT_MAX_SIZE; iJLoop++)
   {   
       if ((iCertificateId == M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].iUcertId)\
                 && (M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].ucPublicKeyCert == 0))
       {          
           (*ppucKeyId)[0] = 254; 
           for (iLoop=0; iLoop<20; iLoop++)                
           {
               (*ppucKeyId)[iLoop+1] = M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].aucPublicKeyHash[iLoop];
           }
           iJLoop=M_SEC_USER_CERT_MAX_SIZE;            
           *piKeyIdLen = 21;        
       }    
   }   
   
   if (*piKeyIdLen == 0)       
   {
       return M_SEC_ERR_INVALID_PARAMETER;
   }
   else        
   {
       return M_SEC_ERR_OK;
   }
}
/*====================================================================================
FUNCTION: 
    Sec_WimIsInited  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    wim initialise. 
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:             Global data handle.
RETURN VALUE:
    (St_SecGlobalData *)hSecHandle)->ucInitialised.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_UINT8 Sec_WimIsInited(WE_HANDLE hSecHandle)
{
    return M_SEC_WIM_UCINITIALISED;
}
/*====================================================================================
FUNCTION: 
    Sec_WimCalHashedSign  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Compute Hashed Signature.    
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    iTargetID[IN]:        TargetID. 
    stPrivKey[IN]:        privkey .
    pucBuf[IN]:           the data needed to hash.
    iBufLen[IN]:          lenght of pucBuf.
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimCalHashedSign (WE_HANDLE   hSecHandle, WE_INT32 iTargetID, 
                                   St_SecCrptPrivKey stPrivKey, 
                                   const WE_UINT8 *pucBuf, WE_INT32 iBufLen)
{
   WE_INT32 iResult=0;
   WE_UINT8 aucDigest[20]={0};
   WE_INT32 iDigLen=20;
 
   iResult = Sec_LibHash (hSecHandle,E_SEC_ALG_HASH_SHA1, pucBuf, iBufLen, aucDigest,&iDigLen);
   if (iResult != M_SEC_ERR_OK)
   {
       return iResult;
   }  
   return Sec_PKCRsaComputeHashedSignature (hSecHandle, iTargetID, stPrivKey, aucDigest, 20);
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetPubKey  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get Public Rsa Key   
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    pucCert[IN]:      certificate.
    pstPubKey[OUT]:   public RSA key.      
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetPubKey(WE_HANDLE hSecHandle,WE_UINT8 *pucCert, St_SecPubKeyRsa *pstPubKey)
{
    WE_INT32 iResult = M_SEC_ERR_OK;
    WE_UINT16 usCertLen = 0;
    St_SecCertificate stCert={0};
    if((!hSecHandle)||(!pucCert)||(!pstPubKey))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (M_SEC_CERTIFICATE_WTLS_TYPE==(*pucCert))         /*wtls*/  
    {
        iResult = Sec_WtlsCertParse(hSecHandle,pucCert+1, &usCertLen, &stCert); 
        if (iResult != M_SEC_ERR_OK)
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
        (*pstPubKey).usExpLen = stCert.stCertUnion.stWtls.usExpLen;
        (*pstPubKey).pucExponent = stCert.stCertUnion.stWtls.pucRsaExponent;
        (*pstPubKey).usModLen = stCert.stCertUnion.stWtls.usModLen;
        (*pstPubKey).pucModulus = stCert.stCertUnion.stWtls.pucRsaModulus;
    }
    else if (M_SEC_CERTIFICATE_X509_TYPE==(*pucCert))     /*x509*/
    {
        iResult = Sec_X509CertParse(hSecHandle,pucCert+1, &usCertLen, &stCert); 
        if (iResult != M_SEC_ERR_OK)
        {
            return M_SEC_ERR_BAD_CERTIFICATE;
        }
        if ((stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 5) || 
            (stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 4) || 
            (stCert.stCertUnion.stX509.pucSignatureAlgId[stCert.stCertUnion.stX509.usSignatureAlgIdLen-1] == 2) )
        {
            iResult = Sec_X509GetRsaModAndExp(hSecHandle,(const WE_UINT8 *)stCert.stCertUnion.stX509.pucPublicKeyVal,\
                &((*pstPubKey).pucModulus), &((*pstPubKey).usModLen), &((*pstPubKey).pucExponent),\
                &((*pstPubKey).usExpLen)); 
            if (iResult != M_SEC_ERR_OK)
            {
                return M_SEC_ERR_BAD_CERTIFICATE;
            }
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
    return iResult;
}
/*====================================================================================
FUNCTION: 
    Sec_WimVerifyHashedSign  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Verify Hashed Signature .  
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    iHashAlg[IN]:         Algorithm 
    iTargetID[IN]:        iTargetID     
    stPubKey[IN]:         public key.
    pucMsg[IN]:           certificate.
    iMsgLen[IN]:          the lenght of pucMsg.
    pucSig[IN]:           Signature
    iSigLen[IN]:          the lenght of Sig.
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimVerifyHashedSign (WE_HANDLE  hSecHandle,WE_INT32 iHashAlg,
                                  WE_INT32 iTargetID,  St_SecCrptPubKey stPubKey, 
                                  const WE_UINT8 *pucMsg, WE_INT32 iMsgLen, 
                                  const WE_UINT8 *pucSig, WE_INT32 iSigLen)
{
   WE_INT32 iResult=0;
   WE_UINT8 aucDigest[20]={0};
   WE_INT32 iDigestLen=20;

   if ((WE_INT32)E_SEC_ALG_HASH_SHA1==iHashAlg )
   {
       iDigestLen = 20;
   }
   else    
   {
       iDigestLen = 16;
   }
   
   iResult = Sec_LibHash(hSecHandle, (E_SecHashAlgType)iHashAlg, pucMsg, iMsgLen, aucDigest,&iDigestLen);
   if (iResult != M_SEC_ERR_OK)
   {
       return iResult;
   }
   return Sec_PKCRsaVerifySignature (hSecHandle,iTargetID, stPubKey,\
                aucDigest, iDigestLen, E_SEC_RSA_PKCS1_NULL,pucSig, iSigLen);
}
/*====================================================================================
FUNCTION: 
    Sec_WimConvPubKey  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Convert public Key   
ARGUMENTS PASSED:
    stKey[IN]:            
    ppucPubKey[OUT]: public key .          
    pusOutPubKeyLen[OUT]:   the length of public key.        
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimConvPubKey(St_SecPubKeyRsa stKey, WE_UINT8 **ppucPubKey, 
                                    WE_UINT16 *pusOutPubKeyLen)
{
    WE_UINT16   usMsgLen=0;
    WE_UINT16   usHdrLen=0;
    WE_UINT16   usMLen=0;
    WE_UINT16   usELen=0;
    WE_UINT8    ucPrePendMod = 0;
    WE_UINT8    ucPrependExp = 0;
    WE_UINT16   usM = stKey.usModLen;
    WE_UINT16   usE = stKey.usExpLen;
    WE_UINT8    *pucP=NULL;
    WE_INT32    iLoop=0;
    WE_UINT8    ucBigEndian=0;   
    WE_UINT16   usU = 0x0102;
 
    if (0x01 == *((WE_INT8*)&(usU))) 
    {
        ucBigEndian = 1;
    }
    else
    {
        ucBigEndian = 0;
    }

    if (*(stKey.pucModulus) > 127)
    {
        ucPrePendMod = 1;
        usM++;
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
    if (*(stKey.pucExponent) > 127)
    {
        ucPrependExp = 1;
        usE++;
    }
    usELen = 2;
    if (usE > 127)
    {
        usELen++;
    }
    if (usE > 255)
    {
        usELen++;
    }

    usMsgLen = (WE_UINT16)(usMLen + usM + usELen + usE);

    usHdrLen = 2;
    if (usMsgLen > 127)
    {
        usHdrLen++;
    }
    if (usMsgLen > 255)
    {
        usHdrLen++;
    }
    if((!ppucPubKey)||(!pusOutPubKeyLen))
    {
        return -1;
    }
    *ppucPubKey = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(usHdrLen + usMsgLen)*sizeof(WE_UINT8)); 
    if (!(*ppucPubKey))
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pucP = (*ppucPubKey);
    *pucP++ = 0x30; 
   
    switch (usHdrLen)
    {
        case 2: 
            if (ucBigEndian)
            {
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usMsgLen))+1));
            } 
            else
            {
                *pucP++ =(WE_UINT8)( *((WE_INT8*)&(usMsgLen)));
            }   
            break; 
        case 3:
            *pucP++ = 0x81; 
            if (ucBigEndian)
            {
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usMsgLen))+1)); 
            }
            else
            {
                *pucP++ =(WE_UINT8)( *((WE_INT8*)&(usMsgLen))); 
            }
            break;  
        case 4:
            *pucP++ = 0x82; 
            if (ucBigEndian)
            {
                *pucP++ = (WE_UINT8)(*((WE_INT8*)&(usMsgLen)));
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usMsgLen))+1)); 
            }
            else
            {
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usMsgLen))+1)); 
                *pucP++ =(WE_UINT8)( *((WE_INT8*)&(usMsgLen)));
            } 
            break;
        default:
            break;
    } 
   
    *pucP++ = '\x02'; 

    switch (usMLen)   
    {  
        case 2:      
            if (ucBigEndian)
            {
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usM))+1)); 
            }
            else
            {
                *pucP++ = (WE_UINT8)(*((WE_INT8*)&(usM)));
            }
            break;
        case 3:
            *pucP++ = 0x81; 
            if (ucBigEndian)
            {
                *pucP++ =(WE_UINT8)( *(((WE_INT8*)&(usM))+1)); 
            }
            else
            {
                *pucP++ =(WE_UINT8)(*((WE_INT8*)&(usM)));
            }
            break;
        case 4:
            *pucP++ = 0x82; 
            if (ucBigEndian)
            {
                *pucP++ =(WE_UINT8)(*((WE_INT8*)&(usM)));
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usM))+1)); 
            }
            else
            {
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usM))+1)); 
                *pucP++ = (WE_UINT8)(*((WE_INT8*)&(usM)));
            }
            break;
        default:
            break;
    }

    if (ucPrePendMod)
    {
        *pucP++ = '\x00';
    }
    for (iLoop=0; iLoop<stKey.usModLen; iLoop++) 
    {
        *pucP++ = stKey.pucModulus[iLoop];
    }
    *pucP++ = '\x02'; 
   
    switch (usELen)
    {
        case 2:
            if (ucBigEndian)
            {
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usE))+1)); 
            }
            else
            {
                *pucP++ = (WE_UINT8)(*((WE_INT8*)&(usE)));
            }
            break;
        case 3:
            *pucP++ = 0x81; 
            if (ucBigEndian)
            {
                *pucP++ =(WE_UINT8)(*(((WE_INT8*)&(usE))+1)); 
            }
            else
            {
                *pucP++ = (WE_UINT8)(*((WE_INT8*)&(usE)));
            }
            break;
        case 4:
            *pucP++ = 0x82; 
            if (ucBigEndian)
            {
                *pucP++ =(WE_UINT8)( *((WE_INT8*)&(usE)));
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usE))+1)); 
            }
            else
            {
                *pucP++ = (WE_UINT8)(*(((WE_INT8*)&(usE))+1)); 
                *pucP++ =(WE_UINT8)(*((WE_INT8*)&(usE)));
            }
            break;
        default:
            break;
    }

    if (ucPrependExp)
    {
        *pucP++ = '\x00';
    }
    for (iLoop=0; iLoop<stKey.usExpLen; iLoop++) 
    {
        *pucP++ = stKey.pucExponent[iLoop];
    }

    (*pusOutPubKeyLen) = (WE_UINT16) (usHdrLen + usMsgLen);
    return M_SEC_ERR_OK;
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetKeyPairInfo    
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Read user cert key pairs information file,then save to M_SEC_WIM_ASTUSERCERTKEYPAIR[]
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:             Global data handle.
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
=====================================================================================*/
void Sec_WimGetKeyPairInfo (WE_HANDLE hSecHandle)
{ 
    WE_INT32  iResult = 0;
    E_WE_ERROR  eRes = E_WE_OK;
    WE_HANDLE hFileHandle=NULL;
    WE_INT32  iReaderLength=0;
    WE_UINT32 uiKeyPairRecSize=0;
    
    if(NULL == hSecHandle)
    {
        return;
    }
    if ((M_SEC_HAVEREAD_KEYPAIRINFO & *(M_SEC_WIM_HAVELOADUSERPRIV)) != M_SEC_HAVEREAD_KEYPAIRINFO)
    {
        /*Clear User key Record*/ 
        Sec_WimClearKeyPairInfo(hSecHandle);    
        uiKeyPairRecSize = M_SEC_USER_CERT_MAX_SIZE * sizeof(St_WimUCertKeyPairInfo);   
        iResult = Sec_WimCheckFileExist(hSecHandle, M_SEC_USERCERT_PRIVKEY_INDEX_NAME);
        if (iResult != M_SEC_ERR_OK)      /*'user_priv' file not exist*/
        {       
            return;
        }     
        
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_USERCERT_PRIVKEY_INDEX_NAME, WE_FILE_SET_RDONLY, &hFileHandle);
        if (eRes != E_WE_OK)
        {
            return;
        }
        /*get user_private key file data*/
        eRes = WE_FILE_READ(hFileHandle, (WE_UINT8 *)M_SEC_WIM_ASTUSERCERTKEYPAIR,(WE_LONG)uiKeyPairRecSize, &iReaderLength);
        if ((iReaderLength < (WE_INT32)uiKeyPairRecSize) || (eRes != E_WE_OK))
        {
            eRes = WE_FILE_CLOSE(hFileHandle);
            return;
        }
        eRes = WE_FILE_CLOSE(hFileHandle);
        *(M_SEC_WIM_HAVELOADUSERPRIV) |= M_SEC_HAVEREAD_KEYPAIRINFO;
    }
}

/*====================================================================================
FUNCTION: 
    Sec_WimCleanUCertData  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Search M_SEC_WIM_ASTUSERCERTKEYPAIR,if someone's UserCertId is equi to input UserCertID,then 
    clear it's data.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:     Global data handle.
    iUserCertId[IN]:  certificate FileId.          
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimCleanUCertData(WE_HANDLE hSecHandle,WE_INT32 iUserCertId)
{
    WE_UINT8 ucFound = 0;
    WE_INT32 iIndex = 0;

    if(NULL==hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    for(iIndex=0;(!ucFound) && (iIndex < M_SEC_USER_CERT_MAX_SIZE);iIndex++)
    {
         if (M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex].iUcertId == iUserCertId)
         {
            ucFound = 1;
            break;
         }        
    }
    if (!ucFound)
    {
        return M_SEC_ERR_MISSING_CERTIFICATE;
    }
    /*clear data*/
    Sec_WimClearOneUCertKeyInfo(&M_SEC_WIM_ASTUSERCERTKEYPAIR[iIndex]);
    return M_SEC_ERR_OK;
}

/*====================================================================================
FUNCTION: 
    Sec_WimGetSubKeyId  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get Subject key Id 
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:     Global data handle.
    pucCert[IN]:        certificate
    ppucKeyId[OUT]:      keyid.
    piKeyIdLen[OUT]:    the length of ppucKeyId.
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetSubKeyId(WE_HANDLE hSecHandle,WE_UINT8 *pucCert, 
                                WE_UINT8 **ppucKeyId, WE_INT32 *piKeyIdLen)
{
    WE_INT32 iResult = 0;
    WE_UINT8 ucIdentifierLen=0;
    WE_INT32 iLoop=0;
    St_SecCertificate stCert={0};
    WE_UINT16 usCertLen = 0;
    WE_UINT16 usSubjectLen = 0;
    
    if((NULL==hSecHandle)||(NULL==pucCert)||(NULL==ppucKeyId)||(NULL==piKeyIdLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
 
    if (M_SEC_CERTIFICATE_WTLS_TYPE==(*pucCert))              /*wtls*/      
    {
        iResult= Sec_WtlsCertParse(hSecHandle,pucCert+1, &usCertLen, &stCert); 
        if (iResult!= M_SEC_ERR_OK)
        {
            return iResult;
        }
        usSubjectLen = stCert.stCertUnion.stWtls.ucSubjectLen;
        if (stCert.pucSubject != NULL)
        {
            (*ppucKeyId)[ucIdentifierLen++] = 1;
            (*ppucKeyId)[ucIdentifierLen++] = 0;
            (*ppucKeyId)[ucIdentifierLen++] = 4;
            (*ppucKeyId)[ucIdentifierLen++] = (WE_UINT8)usSubjectLen;
            
            for (iLoop=0; iLoop<usSubjectLen; iLoop++)
            {
                (*ppucKeyId)[ucIdentifierLen++] = stCert.pucSubject[iLoop];
            }
        }
        *piKeyIdLen=ucIdentifierLen;
        return iResult;                        
    }
    else if (M_SEC_CERTIFICATE_X509_TYPE==(*pucCert))     /*x509*/
    {    
        iResult= Sec_X509CertParse(hSecHandle,pucCert+1, &usCertLen, &stCert); 
        usSubjectLen = stCert.stCertUnion.stX509.usSubjectLen;
        if (iResult!= M_SEC_ERR_OK)
        {
            return iResult;
        }
        if (stCert.stCertUnion.stX509.usSubjectLen>255)
        {
            usSubjectLen=255;
        }
        if (stCert.pucSubject != NULL)
        {
            (*ppucKeyId)[ucIdentifierLen++] = 255;
            (*ppucKeyId)[ucIdentifierLen++] = (WE_UINT8) usSubjectLen;
            
            for (iLoop=0; iLoop<usSubjectLen; iLoop++)
            {
                (*ppucKeyId)[ucIdentifierLen++] = stCert.pucSubject[iLoop];
            }
        }
        *piKeyIdLen=ucIdentifierLen;                    
        return iResult;
    }
    else
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
} 
/*====================================================================================
FUNCTION: 
    Sec_WimGetPrivateKey  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    get privkey.  
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    ucKeyUsage[IN]:             usage of key:M_SEC_PRIVATE_AUTHKEY,M_SEC_PRIVATE_NONREPKEY.
    ppucKey[OUT]:               private key.
    piCertCount[OUT]:           the length of ppucKey.    
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetPrivateKey(WE_HANDLE hSecHandle,WE_UINT8 ucKeyUsage, WE_UINT8 **ppucKey,WE_INT32 *piCertCount)
{
    WE_INT32  iResult = 0;
    E_WE_ERROR eRes = E_WE_OK;
    WE_HANDLE hFileHandle=NULL;
    WE_INT32  iReaderLength=0;
    WE_INT32  iCount=0;
    WE_UINT8  aucIdxFileData[4]={0};
    if((!hSecHandle)||(!ppucKey)||(!piCertCount))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if(M_SEC_PRIVATE_AUTHKEY==ucKeyUsage)
    {
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_AUTH_PRIVKEY_NAME, WE_FILE_SET_RDWR, &hFileHandle);    
    }
    else if(M_SEC_PRIVATE_NONREPKEY==ucKeyUsage)
    {    
        eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_NON_REP_PRIVKEY_NAME, WE_FILE_SET_RDWR, &hFileHandle);
    }
    else 
        return M_SEC_ERR_PRIV_KEY_NOT_FOUND;
    
    if ( eRes!=E_WE_OK )
    {                
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }   
    
    eRes = WE_FILE_READ(hFileHandle, aucIdxFileData, 4,&iReaderLength);
    if ((iReaderLength < 4)|| (eRes!=E_WE_OK))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }   
    Sec_StoreStrUint8to32(aucIdxFileData, (WE_UINT32 *)(&iCount));
    /*get Private key */
    *ppucKey=(WE_UINT8 *)WE_MALLOC((WE_ULONG)iCount*sizeof(WE_UINT8));
    if(!(*ppucKey))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    iResult = Sec_WimDecryptDataFromFile(hSecHandle,hFileHandle, *ppucKey, 4,iCount,&iReaderLength);
    if ((iReaderLength < iCount) || (iResult!=M_SEC_ERR_OK))
    {
        WE_FREE(*ppucKey);
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }   
    *piCertCount=iReaderLength;
    
    eRes = WE_FILE_CLOSE(hFileHandle);        
    return M_SEC_ERR_OK;       
    
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetCACert  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    get root cert
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:     Global data handle.
    iIndex[IN]:             the offset positon in indexfile.    
    ppucCert[OUT]:          the content of certificate:(type+Certification)
    pusCertLen[OUT]:        the length of pucCert. 
    puiFileId[OUT]:         fileId   
    pucCertType[OUT]        certificate type
    pucAble[OUT]            able attribute of certificate
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/ 
WE_INT32 Sec_WimGetCACert(WE_HANDLE hSecHandle,WE_INT32 iIndex, WE_UINT8 **ppucCert,
             WE_UINT16 *pusCertLen, WE_UINT32 *puiFileId,WE_UINT8 *pucCertType, WE_UCHAR *pucAble)
{
    WE_UINT32   uiFileId = 0;
    WE_INT32    iResult=0;
    E_WE_ERROR   eRes = E_WE_OK;
    WE_UINT32   uiFileTypeLen = 1;    
    WE_HANDLE   hFileHandle=NULL;
    WE_CHAR     *pcFileName=NULL;
    WE_INT32    iNewPos=0;
    WE_LONG     lReadNum=0;
    WE_UINT8    *pucIdxFileData=NULL;
    WE_UINT8    *pucTmp=NULL;
    WE_INT32    iLoop=0;
    WE_INT32    iSizeOfIndexFile=0;
    WE_UINT32   uiCertCount=0;
    WE_INT8     cCertType=0;
    WE_UINT16   usFrinedlyNameLen=0;
    
    if((NULL==hSecHandle)||(NULL==ppucCert)||(NULL==puiFileId)||(NULL==pusCertLen)||(!pucCertType)||!iIndex)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    } 

    eRes = WE_FILE_GETSIZE (M_SEC_WIM_HWEHANDLE,M_SEC_CA_CERT_INDEXFILE_NAME,&iSizeOfIndexFile);
    if(eRes != E_WE_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_CA_CERT_INDEXFILE_NAME, WE_FILE_SET_RDWR, &hFileHandle);
    if(eRes != E_WE_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    pucIdxFileData = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(iSizeOfIndexFile+1)*sizeof(WE_UINT8));  
    if (NULL == pucIdxFileData)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    /*read whole CA IndexFile*/
    eRes = WE_FILE_READ(hFileHandle, pucIdxFileData, iSizeOfIndexFile,&lReadNum);
    if ((eRes != E_WE_OK) || (lReadNum < iSizeOfIndexFile))
    {
        WE_FREE(pucIdxFileData);
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);
    pucTmp = pucIdxFileData;
    pucTmp++;
    for(iLoop=0;iLoop<iIndex;iLoop++)
    {
        /*get cert type*/
        cCertType=(WE_INT8)(*pucTmp);
        pucTmp += uiFileTypeLen;   
        /*get file id*/
        Sec_StoreStrUint8to32(pucTmp, &uiFileId);   
        pucTmp += 4;
        /*get cert count*/
        Sec_StoreStrUint8to32(pucTmp, &uiCertCount); 
        pucTmp += 4;
        pucTmp += M_SEC_DN_HASH_LEN+M_SEC_DN_HASH_LEN+1+2;  
        *pucAble = *pucTmp;
        pucTmp++;
        /*get friendly name len*/
        Sec_StoreStrUint8to16(pucTmp, &usFrinedlyNameLen);
        pucTmp += 2 + usFrinedlyNameLen;
    }
    WE_FREE(pucIdxFileData);
    *puiFileId=uiFileId;
    iResult = Sec_WimAcqFileName(cCertType, uiFileId, &pcFileName);
    if (iResult != M_SEC_ERR_OK )
    {
        if (NULL != pcFileName )
        {
            WE_FREE(pcFileName);
            pcFileName = NULL;            
        }
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcFileName, WE_FILE_SET_RDONLY, &hFileHandle);
    WE_FREE(pcFileName);
    if (eRes != E_WE_OK)
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }    
    *ppucCert = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(uiCertCount)*sizeof(WE_UINT8));
    if (!(*ppucCert))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    eRes = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET,&iNewPos);    
    eRes = WE_FILE_READ(hFileHandle, *ppucCert, (WE_LONG)uiCertCount, &lReadNum);
    if ((lReadNum<(WE_INT32)uiCertCount) || (eRes!=E_WE_OK))
    {        
        WE_FREE(*ppucCert);
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);
    
    *pusCertLen=(WE_UINT16)lReadNum;
    *pucCertType=(WE_UINT8)cCertType;    
    return M_SEC_ERR_OK;    
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetKeyExgIds  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get Key Exchange Ids .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    ppucKeyExchangeIds[OUT]:    the value of Id.
    piKeyExIdLen[OUT]:          The length of ppucKeyExchangeIds. 
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
=====================================================================================*/
void Sec_WimGetKeyExgIds(WE_HANDLE hSecHandle,
                              WE_UINT8 **ppucKeyExchangeIds, WE_INT32 *piKeyExIdLen)
{
    WE_INT32 iIdIndex=0;
    WE_INT32 iJLoop=0;
    WE_INT32 ikLoop=0;
    WE_UINT8 ucNbrUCert=0;
    
    if((!ppucKeyExchangeIds)||(!piKeyExIdLen)||(!hSecHandle))
    {
        return ;
    }
   
    (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA;
    (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)0;
    (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)0; 
  
    for(iJLoop=0;iJLoop<M_SEC_USER_CERT_MAX_SIZE;iJLoop++)
    {
        if (ucNbrUCert==0)
        {
            if ((M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].iUcertId !=0) && 
                (M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].ucKeyUsage==M_SEC_PRIVATE_AUTHKEY) &&
                (M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].ucPublicKeyCert == 0))
            {
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA;
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)254;
                
                for (ikLoop=0; ikLoop<20; ikLoop++)
                {
                    (*ppucKeyExchangeIds)[iIdIndex++] = M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].aucPublicKeyHash[ikLoop];
                }
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA_768;
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)254;
                
                for (ikLoop=0; ikLoop<20; ikLoop++)
                {
                    (*ppucKeyExchangeIds)[iIdIndex++] = M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].aucPublicKeyHash[ikLoop];
                }
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA_512;
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++]=(WE_UINT8)254;
                
                for (ikLoop=0; ikLoop<20; ikLoop++)
                {
                    (*ppucKeyExchangeIds)[iIdIndex++] = M_SEC_WIM_ASTUSERCERTKEYPAIR[iJLoop].aucPublicKeyHash[ikLoop];
                }
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON;
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++]= (WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768;
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512;
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)0;
                (*ppucKeyExchangeIds)[iIdIndex++] = (WE_UINT8)0;
                ucNbrUCert=1;
                iJLoop=M_SEC_USER_CERT_MAX_SIZE;
                *piKeyExIdLen=iIdIndex;
            }
        }
    }

}          
/*====================================================================================
FUNCTION: 
    Sec_WimGetWTLSMasterSecret  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    get Master secret  
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    iMasterSecretId[IN]:        The id of the corresponding session slot.
    pucSecret[IN]:              PreMasterSecret.
    iSecretLen[IN]:             length of PreMasterSecret.
    pcLabel[IN]:                lable.
    pucSeed[IN]:                seed.
    iSeedLen[IN]:               length of seed.
    iOutputLen[IN]:             output length.
    pucBuf [OUT]:               MasterSecret
RETURN VALUE:
    If success, return M_SEC_ERR_OK.Else return error code.
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetWTLSMasterSecret(WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId,const WE_UINT8 *pucSecret, 
                                    WE_INT32 iSecretLen, const WE_CHAR *pcLabel,const WE_UINT8 *pucSeed, 
                                    WE_INT32 iSeedLen, WE_INT32 iOutputLen, WE_UINT8 *pucBuf,WE_UINT8 ucHashAlg)
{
    WE_INT32    iLabelLen=0;
    WE_UINT8    *pucLabAndSeed=NULL;
    WE_UINT8    aucMasterSecret[M_SEC_WTLS_MASTER_SECRET_LEN]={0};
    WE_INT32    iResult=0;
    WE_INT32    iLoop=0;
    WE_CHAR     *pcTmpLabel="master secret";   
  
    if ((NULL==pcLabel) || (NULL==pucSeed))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iLabelLen =(WE_INT32)SEC_STRLEN(pcLabel);
    pucLabAndSeed = (WE_UINT8 *)WE_MALLOC(((WE_UINT32)(iLabelLen + iSeedLen))*sizeof(WE_UINT8));
    if (NULL == pucLabAndSeed)
    { 
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }    
    /*get pucLabelAndSeed*/
    for (iLoop=0; iLoop<iLabelLen; iLoop++)
    {
        pucLabAndSeed[iLoop] = (WE_UINT8)(pcLabel[iLoop]);
    }
    for (iLoop=0; iLoop<iSeedLen; iLoop++)
    {
        pucLabAndSeed[iLabelLen + iLoop] = pucSeed[iLoop];
    }
  
    if ( NULL==pucSecret )
    {
        if ((0 == WE_MEMCMP(pcLabel, "client write IV",(WE_UINT32)SEC_STRLEN(pcLabel)))
            || (0 == WE_MEMCMP(pcLabel, "server write IV",(WE_UINT32)SEC_STRLEN(pcLabel))))
        { 
            if((WE_UINT8)E_SEC_ALG_HASH_SHA1 == ucHashAlg)
            {
                Sec_WimPHash(hSecHandle,(WE_UINT8 *)pucSecret, iSecretLen, 
                             pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                             &iResult, pucBuf, iOutputLen);
            }
            else if((WE_UINT8)E_SEC_ALG_HASH_MD5 == ucHashAlg)
            {
                Sec_WimPMD5(hSecHandle,(WE_UINT8 *)pucSecret, iSecretLen, 
                         pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                         &iResult, pucBuf, iOutputLen);
            }
        }
        else if(WE_MEMCMP(pcLabel,pcTmpLabel,(WE_UINT32)SEC_STRLEN(pcLabel)))
        { 
            /*if pre master secret is NULL,get aucMasterSecret by computer iMasterSecretId */
            Sec_WimGetMastSecret(hSecHandle,iMasterSecretId, aucMasterSecret, M_SEC_WTLS_MASTER_SECRET_LEN, &iResult);
            if (iResult != M_SEC_ERR_OK)
            {
                WE_FREE(pucLabAndSeed);
                return iResult;
            }
            if((WE_UINT8)E_SEC_ALG_HASH_SHA1 == ucHashAlg)
            {
                Sec_WimPHash(hSecHandle,aucMasterSecret, M_SEC_WTLS_MASTER_SECRET_LEN, 
                             pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                             &iResult, pucBuf, iOutputLen);
            }
            else if((WE_UINT8)E_SEC_ALG_HASH_MD5 == ucHashAlg)
            {
                Sec_WimPMD5(hSecHandle,aucMasterSecret, M_SEC_WTLS_MASTER_SECRET_LEN, 
                         pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                         &iResult, pucBuf, iOutputLen);
            }
        }
        else   /*master secret*/
        {
           if((WE_UINT8)E_SEC_ALG_HASH_SHA1 == ucHashAlg)
           {
                Sec_WimPHash(hSecHandle,(WE_UINT8 *)pucSecret, iSecretLen, 
                             pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                             &iResult, pucBuf, iOutputLen);
           }
            else if((WE_UINT8)E_SEC_ALG_HASH_MD5 == ucHashAlg)
            {
                Sec_WimPMD5(hSecHandle,(WE_UINT8 *)pucSecret, iSecretLen, 
                    pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                             &iResult, pucBuf, iOutputLen);
            }
        }   
    }
    else
    {
        if((WE_UINT8)E_SEC_ALG_HASH_SHA1 == ucHashAlg)
        {
            Sec_WimPHash(hSecHandle,(WE_UINT8 *)pucSecret, iSecretLen, 
                         pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                         &iResult, pucBuf, iOutputLen);
        }
        else if((WE_UINT8)E_SEC_ALG_HASH_MD5 == ucHashAlg)
        {
            Sec_WimPMD5(hSecHandle,(WE_UINT8 *)pucSecret, iSecretLen, 
                pucLabAndSeed, (WE_INT32)(iLabelLen + iSeedLen), 
                         &iResult, pucBuf, iOutputLen);
        }
    }

    WE_FREE(pucLabAndSeed);
    return iResult;
} 

void Sec_WimHMACMD5(WE_HANDLE hSecHandle,WE_UCHAR *pucK, WE_INT32 iKLen, 
                     WE_UCHAR *pucData, WE_INT32 iDataLen, WE_INT32 *piResult, 
                     WE_UCHAR *pucBuf)
{
    WE_UINT8 *pucInner = NULL;
    WE_UINT8 *pucOuter = NULL;
    WE_UCHAR *pucBptr = NULL;
    WE_INT32  iIndex = 0;
    WE_INT32 iDigLen = 16;
    WE_INT32 iResult=0;

    pucInner = (WE_UINT8*)WE_MALLOC((WE_UINT32)(SEC_HMAC_B + iDataLen)*sizeof(WE_UCHAR));
    if (pucInner == NULL)
    {
        *piResult = M_SEC_ERR_INVALID_PARAMETER;
        return;
    }
    pucOuter = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(SEC_HMAC_B + SEC_HMAC_MD5_L)*sizeof(WE_UCHAR));
    if (pucOuter == NULL)
    {
        *piResult = M_SEC_ERR_INSUFFICIENT_MEMORY;
         WE_FREE(pucInner);
        return;
    }
    if (iKLen > SEC_HMAC_B)
    {
        iResult = Sec_LibHash(hSecHandle,E_SEC_ALG_HASH_MD5, pucK, iKLen, pucInner,&iDigLen); 
        if(iResult !=M_SEC_ERR_OK)
        { 
            WE_FREE(pucOuter);
            WE_FREE(pucInner);
            *piResult =iResult;
            return;
        }

        iKLen = SEC_HMAC_MD5_L;
    }
    else 
    {
        for (iIndex = 0; iIndex < iKLen; iIndex++)
        {
            if ( pucK != NULL )
            {
                pucInner[iIndex] = pucK[iIndex];
            }
        }
    }
    
    for (iIndex = iKLen; iIndex < SEC_HMAC_B; iIndex++)
        pucInner[iIndex] = 0;
    for (iIndex = 0; iIndex < SEC_HMAC_B; iIndex++)  
        pucOuter[iIndex] = pucInner[iIndex];
    
    for (iIndex = 0; iIndex < SEC_HMAC_B; iIndex++)
        pucInner[iIndex] ^= 0x36;
    
    for (iIndex = 0; iIndex < SEC_HMAC_B; iIndex++)
        pucOuter[iIndex] ^= 0x5c;
    
    for (iIndex = 0; iIndex < iDataLen; iIndex++)
        pucInner[SEC_HMAC_B + iIndex] = pucData[iIndex];
    pucBptr = pucOuter;
    pucBptr += SEC_HMAC_B; 
    iResult = Sec_LibHash(hSecHandle,E_SEC_ALG_HASH_MD5, pucInner, (SEC_HMAC_B+iDataLen), pucBptr,&iDigLen); 
    if(iResult!=M_SEC_ERR_OK)
    {
        if (pucOuter != NULL)
        {
            WE_FREE(pucOuter);
        }
        if (pucInner != NULL)
        {
            WE_FREE(pucInner);    
        }
        *piResult =iResult;
        return;
    }
    iResult = Sec_LibHash(hSecHandle,E_SEC_ALG_HASH_MD5, pucOuter, (SEC_HMAC_B+SEC_HMAC_MD5_L), pucBuf,&iDigLen); 
    if(iResult !=M_SEC_ERR_OK)
    {          
        if (pucOuter != NULL)
        {
            WE_FREE(pucOuter);
        }
        if (pucInner != NULL)
        {
            WE_FREE(pucInner);    
        }
        *piResult =iResult;
        return;
    }
    *piResult = M_SEC_ERR_OK;
    
    if (pucOuter != NULL)
        WE_FREE(pucOuter);
    if (pucInner != NULL)
        WE_FREE(pucInner);
}

void Sec_WimPMD5(WE_HANDLE hSecHandle,WE_UCHAR *pucSecret, WE_INT32 iSecretLen, 
                  WE_UCHAR *pucSeed, WE_INT32 iSeedLen, WE_INT32 *iResult, 
                  WE_UCHAR *pucBuf, WE_INT32 iDesiredLen)
{
    WE_UCHAR *pucSecAndSeed = NULL; 
    WE_UCHAR aucPart[16] = {0};   
    WE_INT32 iRes = 0;
    WE_INT32 iIterations = 0;
    WE_INT32 iIndexI = 0;
    WE_INT32 iIndexJ = 0;
    WE_INT32 iNumCopiedChars = 0;;
    
    pucSecAndSeed = (WE_UCHAR *)WE_MALLOC((WE_UINT32)(SEC_HMAC_MD5_L + iSeedLen)*sizeof(WE_CHAR));
    if (NULL == pucSecAndSeed )
    {
        *iResult = M_SEC_ERR_INVALID_PARAMETER;
        return;
    }
    
    Sec_WimHMACMD5(hSecHandle,pucSecret, iSecretLen, pucSeed, iSeedLen, &iRes, pucSecAndSeed); 
    if (iRes != M_SEC_ERR_OK)
    {
        *iResult = iRes;
        WE_FREE(pucSecAndSeed);
        return;
    }
    for (iIndexI = 0; iIndexI < iSeedLen; iIndexI++)
    {
        pucSecAndSeed[SEC_HMAC_MD5_L + iIndexI] = pucSeed[iIndexI]; 
    }
    
    iIterations = iDesiredLen / SEC_HMAC_MD5_L; 
    if (iDesiredLen % SEC_HMAC_MD5_L != 0)
    {
        iIterations++;
    }
    
    iNumCopiedChars = 0;
    for (iIndexI = 0; iIndexI < iIterations; iIndexI++)
    {
        Sec_WimHMACMD5(hSecHandle,pucSecret, iSecretLen, pucSecAndSeed, (SEC_HMAC_MD5_L + iSeedLen), &iRes, aucPart);
        if (iRes != M_SEC_ERR_OK)
        {
            *iResult = iRes;
            if (NULL != pucSecAndSeed)
            {
                WE_FREE(pucSecAndSeed);
            }
            return;
        }
        
        for (iIndexJ  = 0; iIndexJ < SEC_HMAC_MD5_L; iIndexJ++)
        {
            pucBuf[(SEC_HMAC_MD5_L * iIndexI) + iIndexJ] = aucPart[iIndexJ];
            if (++iNumCopiedChars >= iDesiredLen)
                break;
        }
        if (iNumCopiedChars >= iDesiredLen)
            break;
        Sec_WimHMACMD5(hSecHandle,pucSecret, iSecretLen, pucSecAndSeed, SEC_HMAC_MD5_L, &iRes, pucSecAndSeed); 
        if (iRes != M_SEC_ERR_OK)
        {
            *iResult = iRes;
            if (pucSecAndSeed != NULL)
                WE_FREE(pucSecAndSeed);
            return;
        }
    }
    
    *iResult = M_SEC_ERR_OK;
    if (pucSecAndSeed != NULL)
        WE_FREE(pucSecAndSeed);
    return;
}

/*====================================================================================
FUNCTION: 
    Sec_WimPHash  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:    
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    pucSecret[IN]:              secret.
    iSecretLen[IN]:             length of pucSecret.
    pucSeed[IN]:                seed.
    iSeedLen[IN]:               length of pucSeed.
    piResult[OUT]:              result.
    pucBuf[OUT]:                buffer.
    iDesiredLen[IN]:            Desired Lenght.
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
=====================================================================================*/
void Sec_WimPHash(WE_HANDLE hSecHandle,WE_UINT8 *pucSecret, WE_INT32 iSecretLen, 
                  WE_UINT8 *pucSeed, WE_INT32 iSeedLen, WE_INT32 *piResult, 
                  WE_UINT8 *pucBuf, WE_INT32 iDesiredLen)
{
    WE_INT32 iRes=0;
    WE_INT32 iTimes=0;
    WE_INT32 iLoop=0;
    WE_INT32 iJLoop=0;
    WE_UINT8 *pucSecAndSeed=NULL; 
    WE_INT32 iNumCopiedchars=0;
    WE_UINT8 aucPart[20]={0};   
    if((!hSecHandle)||(!pucSeed))
    {
        *piResult = M_SEC_ERR_INVALID_PARAMETER;
         return ;
    }
    
    pucSecAndSeed = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(SEC_HMAC_SHA1_L + iSeedLen)*sizeof(WE_UINT8));
    if (!pucSecAndSeed)
    {
        *piResult = M_SEC_ERR_INSUFFICIENT_MEMORY;
        return;
    }
    
    Sec_WimHMAC(hSecHandle,pucSecret, iSecretLen, pucSeed, iSeedLen,  &iRes, pucSecAndSeed); 
    if (iRes != M_SEC_ERR_OK)
    {
        *piResult = iRes;      
         WE_FREE(pucSecAndSeed);  
         return;
    }
    for (iLoop=0; iLoop<iSeedLen; iLoop++)
    {
        pucSecAndSeed[SEC_HMAC_SHA1_L+iLoop] = pucSeed[iLoop]; 
    }
    
    iTimes = iDesiredLen / SEC_HMAC_SHA1_L; 
    if (iDesiredLen % SEC_HMAC_SHA1_L != 0)
    {
        iTimes++;
    }
    iNumCopiedchars = 0;
    for (iLoop=0; iLoop<iTimes; iLoop++)
    {
        Sec_WimHMAC(hSecHandle,pucSecret, iSecretLen, pucSecAndSeed, (SEC_HMAC_SHA1_L + iSeedLen), &iRes, aucPart);
        if (iRes != M_SEC_ERR_OK)
        {
            *piResult = iRes;
            if (pucSecAndSeed != NULL)
            {
                WE_FREE(pucSecAndSeed);
            }
            return;
        }
        
        for (iJLoop=0; iJLoop<SEC_HMAC_SHA1_L; iJLoop++)
        {
            if(!pucBuf)
            {
                *piResult = M_SEC_ERR_INVALID_PARAMETER;      
                WE_FREE(pucSecAndSeed);  
                return;
            }
            pucBuf[(SEC_HMAC_SHA1_L*iLoop) + iJLoop] = aucPart[iJLoop];
            if (++iNumCopiedchars >= iDesiredLen)
            {
                break;
            }
        }
        if (iNumCopiedchars >= iDesiredLen)
        {
            break;
        }
        Sec_WimHMAC(hSecHandle,pucSecret, iSecretLen, pucSecAndSeed, SEC_HMAC_SHA1_L, &iRes, pucSecAndSeed); 
        if (iRes != M_SEC_ERR_OK)
        {
            *piResult = iRes;
            if (pucSecAndSeed != NULL)
            {
                WE_FREE(pucSecAndSeed);
            }
            return;
        }
    }    
    *piResult = M_SEC_ERR_OK;
    if (pucSecAndSeed != NULL)
    {
        WE_FREE(pucSecAndSeed);
    }
    return;
}

/*====================================================================================
FUNCTION: 
    Sec_WimHMAC  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    HMAC
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:         Global data handle.
    pucK[IN]:       secret.        
    iKLen[IN]:      the length of pucK.        
    pucData[IN]:    seed.        
    iDataLen[IN]:   the length of pucData.      
    piResult[OUT]:  result.        
    pucBuf[OUT]:      pucbuf.        
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
=====================================================================================*/
void Sec_WimHMAC(WE_HANDLE hSecHandle,WE_UINT8 *pucK, WE_INT32 iKLen, WE_UINT8 *pucData, 
                 WE_INT32 iDataLen, WE_INT32 *piResult,WE_UINT8 *pucBuf)
{
    WE_UINT8 *pucIn=NULL;
    WE_UINT8 *pucOut=NULL;
    WE_UINT8 *pucBptr=NULL;
    WE_INT32 iLoop=0;
    WE_INT32 iDigLen=0;
    WE_INT32 iResult=0;
   
    if((!hSecHandle)||(!pucData))
    {
        *piResult = M_SEC_ERR_INVALID_PARAMETER;
         return ;
    }
    pucIn = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(SEC_HMAC_B + iDataLen)*sizeof(WE_UINT8));        
    if (NULL==pucIn)
    {
        *piResult = M_SEC_ERR_INSUFFICIENT_MEMORY;
        return;
    }
    pucOut = (WE_UINT8 *)WE_MALLOC((WE_UINT32)(SEC_HMAC_B + SEC_HMAC_SHA1_L)*sizeof(WE_UINT8));
    if (!pucOut)
    {
        *piResult = M_SEC_ERR_INSUFFICIENT_MEMORY;
        WE_FREE(pucIn);
        return;
    }
        
    if (iKLen > SEC_HMAC_B)
    {
        iDigLen=SEC_HMAC_B + iDataLen;
        iResult=Sec_LibHash(hSecHandle,E_SEC_ALG_HASH_SHA1, pucK, iKLen, pucIn,&iDigLen); 
        if(iResult!=M_SEC_ERR_OK)
        {          
             WE_FREE(pucOut);
             WE_FREE(pucIn);
             *piResult =iResult;
             return;
        }
        iKLen = SEC_HMAC_SHA1_L;
    }
    else 
    {
        for (iLoop=0; iLoop<iKLen; iLoop++)
        {
            if (pucK != NULL)
            {
                pucIn[iLoop] = pucK[iLoop];
            }
        }
    }
    
    for (iLoop=iKLen; iLoop<SEC_HMAC_B; iLoop++)
    {
        pucIn[iLoop] = 0;
    }
    for (iLoop=0; iLoop<SEC_HMAC_B; iLoop++)  
    {
        pucOut[iLoop] = pucIn[iLoop];
    }    
    for (iLoop=0; iLoop<SEC_HMAC_B; iLoop++)
    {
        pucIn[iLoop] ^= 0x36;
    }    
    for (iLoop=0; iLoop<SEC_HMAC_B; iLoop++)
    {
        pucOut[iLoop] ^= 0x5c;
    }    
    for (iLoop=0; iLoop<iDataLen; iLoop++)
    {
        pucIn[SEC_HMAC_B+iLoop] = pucData[iLoop];
    }
    
    pucBptr = pucOut;
    pucBptr += SEC_HMAC_B; 
    iDigLen=SEC_HMAC_SHA1_L;
    iResult=Sec_LibHash(hSecHandle,E_SEC_ALG_HASH_SHA1, pucIn, (SEC_HMAC_B+iDataLen), pucBptr,&iDigLen); 
    if(iResult!=M_SEC_ERR_OK)
    {
        if (pucOut != NULL)
        {
            WE_FREE(pucOut);
        }
        if (pucIn != NULL)
        {
            WE_FREE(pucIn);    
        }
        *piResult =iResult;
        return;
    }
    
    iDigLen=SEC_HMAC_SHA1_L;
    iResult=Sec_LibHash(hSecHandle,E_SEC_ALG_HASH_SHA1, pucOut, (SEC_HMAC_B+SEC_HMAC_SHA1_L), pucBuf,&iDigLen); 
    if(iResult!=M_SEC_ERR_OK)
    { 
        if (pucOut != NULL)
        {
            WE_FREE(pucOut);
        }
        if (pucIn != NULL)
        {
            WE_FREE(pucIn);
        }
        *piResult =iResult;
        return;
    }
    *piResult = M_SEC_ERR_OK;
    
    if (pucOut != NULL)
    {
        WE_FREE(pucOut);
    }
    if (pucIn != NULL)
    {
        WE_FREE(pucIn);
    }
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetCertFormatByID  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    get certificate format by id.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:      Global data handle.
    iCertId[IN]:             certificate id.
    pucCertFormat[OUT]:      certificate format:M_SEC_CERTIFICATE_X509_TYPE,M_SEC_CERTIFICATE_WTLS_TYPE
RETURN VALUE:
    converted result. 
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetCertFormatByID(WE_HANDLE hSecHandle,
                                        WE_INT32 iCertId, WE_UINT8 *pucCertFormat)
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

    if((!hSecHandle)||(!pucCertFormat))
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
    
    /* get size of indexfile.*/
    eRes = WE_FILE_GETSIZE(M_SEC_WIM_HWEHANDLE, pcIndexFileName, &(iIndexFileSize));
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
    eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, pcIndexFileName, WE_FILE_SET_RDONLY, &(hFileHandle));
    if (eRes != E_WE_OK)
    {
        WE_FREE(pucIndexFile);
        return M_SEC_ERR_GENERAL_ERROR;
    }     
    eRes = WE_FILE_READ(hFileHandle, pucIndexFile, iIndexFileSize, &(iResult));
    if ((eRes!=E_WE_OK) || (iResult<iIndexFileSize))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        WE_FREE(pucIndexFile);
        return M_SEC_ERR_GENERAL_ERROR;
    }   
    eRes = WE_FILE_CLOSE(hFileHandle); 
    pucP = pucIndexFile;
    iNumInIndexFile = *pucP;
    pucP++;
    for (iIndexI=0; iIndexI<iNumInIndexFile; iIndexI++) 
    {
        cFileType = (WE_INT8)(*pucP);
        Sec_StoreStrUint8to32(pucP+1, (WE_UINT32 *)(&iTmpCertId));
        if (iTmpCertId == iCertId)
        {            
            if ((M_SEC_X509_USER_CERT == cFileType)|| (M_SEC_X509_CA_CERT == cFileType))
            {
                *pucCertFormat = M_SEC_CERTIFICATE_X509_TYPE;
            }
            else                                      /*Homemade Cert is stored with wtls format*/
            {
                *pucCertFormat = M_SEC_CERTIFICATE_WTLS_TYPE;
            }
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
    return M_SEC_ERR_GENERAL_ERROR;    
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetNbrUserCerts  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get number of user certificate .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:             Global data handle.
RETURN VALUE:
    the number of user certificate.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetNbrUserCerts(WE_HANDLE hSecHandle)
{
    WE_INT32 iLoop=0;
    WE_INT32 iNbrOfCerts=0;
    
    if(NULL==hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    for (iLoop=0;iLoop<M_SEC_USER_CERT_MAX_SIZE;iLoop++)
    {
        if((M_SEC_WIM_ASTUSERCERTKEYPAIR[iLoop].iUcertId != 0) && (M_SEC_WIM_ASTUSERCERTKEYPAIR[iLoop].ucPublicKeyCert==0))
        {
            iNbrOfCerts++;
        }
    }
    return iNbrOfCerts;
}
/*====================================================================================
FUNCTION: 
    Sec_WimGetNbrOfCACerts  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get number of ca certificate .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:             Global data handle.
    pucCACertNum[OUT]:              Number of ca certificate.
RETURN VALUE:
    the number of user certificate.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetNbrOfCACerts(WE_HANDLE hSecHandle, WE_UINT8 *pucCACertNum)
{
    E_WE_ERROR   eRes = E_WE_OK; 
    WE_INT32     iResult = 0;
    WE_HANDLE    hFileHandle = NULL;
    
    if ((!hSecHandle)||(!pucCACertNum))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_CA_CERT_INDEXFILE_NAME, WE_FILE_SET_RDONLY, &(hFileHandle)); 
    if (eRes != E_WE_OK)
    {   
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_READ(hFileHandle, pucCACertNum, 1, &(iResult));
    if ((eRes != E_WE_OK) || (iResult < 1))
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);
    return M_SEC_ERR_OK;
}                 
/*====================================================================================
FUNCTION: 
    Sec_WimGetBlockLength  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get length of indexfile block .
ARGUMENTS PASSED:
    pucP[IN]:                    point to data.
    cFileType[IN]:               the type of file:
                                 M_SEC_X509_USER_CERT,M_SEC_WTLS_USER_CERT,M_SEC_PUBKEY_CERT,
                                 M_SEC_WTLS_CA_CERT,M_SEC_X509_CA_CERT,M_SEC_CONTRACT,
    piBlockLen[OUT}:             length of block.
RETURN VALUE:
    the number of user certificate.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetBlockLength(WE_UINT8 *pucP, 
                               WE_INT8 cFileType, WE_INT32 *piBlockLen)
{
    WE_UINT16 usNameLen = 0;
    
    if ((NULL == piBlockLen) || (NULL == pucP))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    *piBlockLen = 0;    
    if ((M_SEC_USER_CERT_HEADER == cFileType) || (M_SEC_CA_CERT_HEADER == cFileType)
        || (M_SEC_X509_USER_CERT == cFileType) || (M_SEC_X509_CA_CERT == cFileType)
        || (M_SEC_WTLS_USER_CERT == cFileType) || (M_SEC_WTLS_CA_CERT == cFileType)
        ||(M_SEC_PUBKEY_CERT == cFileType))
    {
        *piBlockLen += 1+4+4+M_SEC_DN_HASH_LEN+M_SEC_DN_HASH_LEN+1+2;
        if ((M_SEC_CA_CERT_HEADER == cFileType) || (M_SEC_X509_CA_CERT == cFileType)||
            (M_SEC_WTLS_CA_CERT == cFileType))
        {
            *piBlockLen += 1;
        }
        Sec_StoreStrUint8to16((pucP+*piBlockLen), &usNameLen);
        *piBlockLen += 2;
        *piBlockLen += usNameLen;
    }
    else
    {
        *piBlockLen = 8;    
    }
    return M_SEC_ERR_OK;
}

/*====================================================================================
FUNCTION: 
    Sec_WimGetNbrOfWTLSDisableCACerts  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Get the number of disable WTLS CA certificate .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:  Global data handle.
    piNumWTLSDisableRootCerts[OUT]: Pointer to the number of disable WTLS CA certificate.
RETURN VALUE:
    the number of user disable WTLS CA certificate.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
WE_INT32 Sec_WimGetNbrOfWTLSDisableCACerts(WE_HANDLE hSecHandle,
                                    WE_INT32 *piNumWTLSDisableRootCerts)
{
    E_WE_ERROR  eRes = E_WE_OK;
    WE_INT32    iResult = -1;
    WE_HANDLE  hFileHandle = NULL;
    WE_INT32   iIndexFileSize=0;
    WE_UCHAR   *pucIdxFileData = NULL;
    WE_UCHAR   *pucTmp = NULL;
    WE_LONG    lReadNum = 0;
    WE_INT32   iIndex = 0;
    WE_INT32   iLoop = 0;
    WE_INT32   iBlockLen = 0;
    
    if((NULL == hSecHandle) || (NULL == piNumWTLSDisableRootCerts))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    eRes = WE_FILE_GETSIZE(M_SEC_WIM_HWEHANDLE, M_SEC_CA_CERT_INDEXFILE_NAME, &(iIndexFileSize));
    if (eRes != E_WE_OK)
    {   
        return M_SEC_ERR_GENERAL_ERROR;
    }
    if (1 >= iIndexFileSize)
    {
        *piNumWTLSDisableRootCerts = 0;
        return M_SEC_ERR_OK;
    }
    
    eRes = WE_FILE_OPEN(M_SEC_WIM_HWEHANDLE, M_SEC_CA_CERT_INDEXFILE_NAME, WE_FILE_SET_RDONLY, &(hFileHandle)); 
    if (eRes != E_WE_OK)
    {   
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    pucIdxFileData=(WE_UINT8 *)WE_MALLOC((WE_UINT32)(iIndexFileSize) * sizeof(WE_UINT8));  
    if (NULL == pucIdxFileData)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    /*read whole CA IndexFile*/
    eRes = WE_FILE_READ(hFileHandle, pucIdxFileData, iIndexFileSize, &lReadNum);
    if (eRes != E_WE_OK)
    {  
        WE_FREE(pucIdxFileData);
        eRes = WE_FILE_CLOSE(hFileHandle);   
        return M_SEC_ERR_GENERAL_ERROR;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);    
    pucTmp = pucIdxFileData;
    iIndex = *pucTmp;
    pucTmp++;
    for(iLoop=0; iLoop<iIndex; iLoop++)
    {
        if (M_SEC_WTLS_CA_CERT == (*pucTmp))
        {
            if (M_SEC_WTLS_CA_CERT_DISABLE == *(pucTmp+1+4+4+M_SEC_DN_HASH_LEN+M_SEC_DN_HASH_LEN+1+2))
            {
                (*piNumWTLSDisableRootCerts)++;
            }
        }
        iResult = Sec_WimGetBlockLength(pucTmp, M_SEC_CA_CERT_HEADER, &iBlockLen);
        if (iResult != M_SEC_ERR_OK)
        {  
            WE_FREE(pucIdxFileData);
            return M_SEC_ERR_GENERAL_ERROR;
        }
        pucTmp += iBlockLen;
    }
    WE_FREE(pucIdxFileData);
    return M_SEC_ERR_OK;
} 


/*=====================================================================================
    FILE NAME :
        secapp_linux.c
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
    2007-03-22      Stone An       None           create  secapp_linux.c
        
=====================================================================================*/

/*******************************************************************************
*   Include File Section
*******************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "we_def.h"
#include "we_mem.h"
#include "sec.h"
#include "isecw.h"
#include "isecb.h"
#include "isigntext.h"

 /*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define ISIGNTESTREPORT                     "isignresult.txt"
#define ISECWAPTESTREPORT                "isecwresult.txt"
#define ISECERRORREPORT                     "isecerror.txt"
#define ISECBROWSERTESTREPORT        "isecbresult.txt"
#define ISECBROWSERERRORREPORT      "isecberror.txt"
#define USERCERTREQ                             "user_cert_req"

/* Values for keyIdType parameter */
#define M_SEC_SIGN_NO_KEY                     0x00
#define M_SEC_SIGN_SHA_KEY                   0x01
#define M_SEC_SIGN_SHA_CA_KEY             0x02
/* Values for options parameter */
#define M_SEC_SIGNTEXT_RETURNHASHEDKEY     0x02
#define M_SEC_SIGNTEXT_RETURNCERT                0x04
/* hash size */
#define M_SEC_KEY_HASH_SIZE                  20

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef enum tagE_IFType
{
    E_SEC_WTLS,
        E_SEC_BRS,
        E_SEC_SIGNTEXT
}E_IFType;

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

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 Test_ISecW_SearchPeer( ISEC *pISecW );
static WE_INT32 Test_ISecW_SessionGet( ISEC *pISecW );
static WE_INT32 Test_ISecW_GetCipherSuite( ISEC *pISecW );
static WE_INT32 Test_ISecW_KeyExchange( ISEC *pISecW );
static WE_INT32 Test_ISecW_GetUserCert( ISEC *pISecW );
static WE_INT32 Test_ISecW_CompSign( ISEC *pISecW );
static WE_INT32 Test_ISecW_GetPrfResult( ISEC *pISecW );
static WE_INT32 Test_ISecW_VerifySvrCertChain( ISEC *pISecW );
static WE_INT32 Test_ISecW_RemovePeer( ISEC *pISecW );
static WE_INT32 Test_ISecW_SessionRenew( ISEC *pISecW );
static WE_INT32 Test_ISecW_AttachPeerToSession( ISEC *pISecW );
static WE_INT32 Test_ISecW_EnableSession( ISEC *pISecW );
static WE_INT32 Test_ISecW_SetUpConnection( ISEC *pISecW );
static WE_INT32 Test_ISecW_StopConnection( ISEC *pISecW );
static WE_INT32 Test_ISecW_DisableSession( ISEC *pISecW );
static WE_INT32 Test_ISecB_GenKeyPair(ISEC *pISecB);
static WE_INT32 Test_ISecB_GetRequestUserCert( ISEC *pISecB );
static WE_INT32 Test_ISecB_TransferCert( ISEC *pISecB );
static WE_INT32 Test_ISecB_GetCertNameList( ISEC *pISecB );
static WE_INT32 Test_ISecB_GetContractList( ISEC *pISecB );
static WE_INT32 Test_ISecB_GetSessionInfo( ISEC *pISecB );
static WE_INT32 Test_ISecB_GetWtlsCurClass( ISEC *pISecB );
static WE_INT32 Test_ISecB_GetCurSvrCert( ISEC *pISecB );
static WE_INT32 Test_ISignTextHandle( ISEC *pISecS );
static WE_VOID thread_Wcallfunction(ISEC *pISec);
static WE_VOID thread_Bcallfunction(ISEC *pISecB);
static WE_VOID thread_Scallfunction(ISEC *pISecS);
static WE_UINT GetCurrentTime( WE_VOID );
static WE_VOID IntToString( char *pszString,WE_INT32 iConvet );
static WE_VOID WriteResult(char * pcFileName,const char *pcBuffer, WE_INT32 iBufferLength );
static WE_INT32 readfile(WE_CHAR *pcFileName, WE_UCHAR **ppucFile, WE_INT32 *piLen);
static WE_VOID NotifySignTextCallback(WE_INT32 iEvent,  WE_VOID *hSecAppEvent, WE_VOID *pvPrivData);
static WE_VOID NotifyBrowCallback(WE_INT32 iEvent, WE_VOID *hSecAppEvent, WE_VOID *pvPrivData);
static WE_VOID NotifyWapCallback(WE_INT32 iEvent, WE_VOID *hSecAppEvent, WE_VOID *pvPrivData);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
static WE_INT32 Test_ISecW_SearchPeer( ISEC *pISecW )
{
    WE_INT32 iTargetID = 0;                     
    WE_UINT8 aucAddress[6] = { 0, 4, 192, 168, 52, 241 };    
    WE_UINT8 aucAddress2[6] = { 0, 4, 10, 0, 0, 172 };      
    WE_UINT8 aucAddress3[6] = { 0, 4, 1, 1, 1, 1 };    
    WE_INT32 iResult = 0;    
    
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress, 6, 9203, G_SEC_WTLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    #if 1
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress2, 6, 9203, G_SEC_WTLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress, 6, 5555, G_SEC_WTLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress3, 6, 9000, G_SEC_WTLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress, 6, 9000, G_SEC_TLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress, 6, 443, G_SEC_TLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress, 6, 9000, G_SEC_SSL_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecW_SearchPeer( (ISecW *)pISecW, iTargetID++, aucAddress, 6, 443, G_SEC_SSL_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    #endif
    return G_SEC_OK; 
}

static WE_INT32 Test_ISecW_SessionGet( ISEC *pISecW )
{
    WE_INT32 iTargetID = 0; 
    WE_INT32 iMasterSecretId = 0;    
    WE_INT32 iResult  = 0;    
    
    iMasterSecretId = 0;
    iResult = ISecW_SessionGet( (ISecW *)pISecW, iTargetID++, iMasterSecretId );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iMasterSecretId = 2;
    iResult = ISecW_SessionGet( (ISecW *)pISecW, iTargetID++, iMasterSecretId );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iMasterSecretId = 11;
    iResult = ISecW_SessionGet( (ISecW *)pISecW, iTargetID++, iMasterSecretId );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    return G_SEC_OK;
}


static WE_INT32 Test_ISecW_GetCipherSuite( ISEC *pISecW )
{
    WE_INT32 iTargetID = 0;     
    WE_INT32 iResult  = 0;   
    
    iResult = ISecW_GetCipherSuite((ISecW *)pISecW, iTargetID++, G_SEC_WTLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    iResult = ISecW_GetCipherSuite((ISecW *)pISecW, iTargetID++, G_SEC_WTLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    iResult = ISecW_GetCipherSuite((ISecW *)pISecW, iTargetID++, G_SEC_SSL_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    iResult = ISecW_GetCipherSuite((ISecW *)pISecW, iTargetID++, G_SEC_TLS_CONNECTION_TYPE);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_KeyExchange( ISEC *pISecW )
{
    WE_INT32  iTargetID = 0;    
    WE_INT32  iResult  = 0;
    St_SecWtlsKeyExchParams stParam = {{{{{{0}}}}}, 0};
    WE_UCHAR  aucRandval[32] = {0};
    WE_UINT8  ucHashAlg = 1;
    
    {    
        stParam.ucKeyExchangeSuite = G_SEC_KEYEXCH_NULL;
        ucHashAlg = G_SEC_HASH_MD5;
        iResult = ISecW_KeyExchange((ISecW *)pISecW, iTargetID++, (WE_VOID *)(&stParam), ucHashAlg, 
            aucRandval, G_SEC_WTLS_CONNECTION_TYPE);
        if (iResult != G_SEC_OK)
        {
            return iResult;
        }
    }
    
    {
        WE_UINT8  *pucExp = NULL;
        WE_INT32  uiExpLen = 0;
        WE_UINT8  *pucMod = NULL;
        WE_INT32  uiModLen = 0;
        
        stParam.ucKeyExchangeSuite = G_SEC_KEYEXCH_RSA_ANON;
        ucHashAlg = G_SEC_HASH_SHA;
        iResult = readfile("exponent_241", &pucExp, &uiExpLen);
        if (iResult != G_SEC_OK)
        {
            return iResult;
        }
        iResult = readfile("mod_241", &pucMod, &uiModLen);
        if (iResult != G_SEC_OK)
        {
            WE_FREE(pucExp);
            return iResult;
        }
        stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen = (WE_UINT16)uiExpLen;            
        stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent = pucExp;                
        stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen = (WE_UINT16)uiModLen;
        stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus = pucMod; 
        iResult = ISecW_KeyExchange((ISecW *)pISecW, iTargetID++, (WE_VOID *)(&stParam), ucHashAlg, 
            aucRandval, G_SEC_WTLS_CONNECTION_TYPE);
        WE_FREE(pucExp);
        WE_FREE(pucMod);
        if (iResult != G_SEC_OK)
        {
            return iResult;
        }
    }
    {
        WE_UINT8  *pucCert = NULL;
        WE_UINT8  *pucTmpCert = NULL;
        WE_INT32  uiCertLen = 0;
        
        stParam.ucKeyExchangeSuite = G_SEC_KEYEXCH_RSA_512;
        ucHashAlg = G_SEC_HASH_MD5;
        iResult = readfile("95599", &pucCert, &uiCertLen);
        if (iResult != G_SEC_OK)
        {
            return iResult;
        }
        pucTmpCert = (WE_UINT8 *)WE_MALLOC((uiCertLen + 1) * sizeof(WE_UINT8));
        if (NULL == pucTmpCert)
        {
            WE_FREE(pucCert);
            return G_SEC_NOTENOUGH_MEMORY;
        }
        *pucTmpCert = 0x02;
        (WE_VOID)WE_MEMCPY(pucTmpCert+1, pucCert, uiCertLen);       
        WE_FREE(pucCert);               
        stParam.stParams.stCertificates.usBufLen = (WE_UINT16)(uiCertLen + 1);
        stParam.stParams.stCertificates.pucBuf = pucTmpCert;
        iResult = ISecW_KeyExchange((ISecW *)pISecW, iTargetID++, (WE_VOID *)(&stParam), ucHashAlg, 
            aucRandval, G_SEC_WTLS_CONNECTION_TYPE);
        WE_FREE(pucTmpCert);
        if (iResult != G_SEC_OK)
        {
            return iResult;
        }                    
    }
    
    {
        WE_UINT8  *pucCert = NULL;
        WE_UINT8  *pucTmpCert = NULL;
        WE_INT32  uiCertLen = 0;
        
        stParam.ucKeyExchangeSuite = G_SEC_KEYEXCH_RSA_512;
        ucHashAlg = G_SEC_HASH_MD5;
        iResult = readfile("wtlscacert", &pucCert, &uiCertLen);
        if (iResult != G_SEC_OK)
        {
            return iResult;
        }
        pucTmpCert = (WE_UINT8 *)WE_MALLOC((uiCertLen + 1) * sizeof(WE_UINT8));
        if (NULL == pucTmpCert)
        {
            WE_FREE(pucCert);
            return G_SEC_NOTENOUGH_MEMORY;
        }
        *pucTmpCert = 0x01;
        (WE_VOID)WE_MEMCPY(pucTmpCert+1, pucCert, uiCertLen);       
        WE_FREE(pucCert);               
        stParam.stParams.stCertificates.usBufLen = (WE_UINT16)(uiCertLen + 1);
        stParam.stParams.stCertificates.pucBuf = pucTmpCert;
        iResult = ISecW_KeyExchange((ISecW *)pISecW, iTargetID++, (WE_VOID *)&stParam, ucHashAlg, 
            aucRandval, G_SEC_WTLS_CONNECTION_TYPE);
        WE_FREE(pucTmpCert);
        if (iResult != G_SEC_OK)
        {
            return iResult;
        }                    
    }
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_GetUserCert( ISEC *pISecW )
{
    WE_INT32 iTargetID = 0; 
    WE_INT32 iResult  = 0;
    WE_UCHAR *pucBuf = NULL;
    WE_INT32 uiBufLen  = 0;
    
    iResult = readfile("issuer", &pucBuf, &uiBufLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    ISecW_GetUserCert((ISecW *)pISecW, iTargetID++, pucBuf, (WE_INT32)uiBufLen, G_SEC_WTLS_CONNECTION_TYPE );
    WE_FREE(pucBuf);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iResult = readfile("issuer2", &pucBuf, &uiBufLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    ISecW_GetUserCert((ISecW *)pISecW, iTargetID++, pucBuf, (WE_INT32)uiBufLen, G_SEC_WTLS_CONNECTION_TYPE );
    WE_FREE(pucBuf);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    ISecW_GetUserCert((ISecW *)pISecW, iTargetID++, NULL, 0, G_SEC_WTLS_CONNECTION_TYPE );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    return G_SEC_OK;
}


static WE_INT32 Test_ISecW_CompSign( ISEC *pISecW )
{
    WE_INT32 iTargetID = 0; 
    WE_INT32 iResult  = 0;
    WE_UCHAR aucBuf[8] = {"ttsefsd"};
    WE_UCHAR *pucKeyId = NULL;
    WE_INT32 iKeyIdLen = 0;
    
    iResult = readfile("keyExchangeId1", &pucKeyId, &iKeyIdLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iResult = ISecW_CompSign((ISecW *)pISecW, iTargetID++, pucKeyId, iKeyIdLen, 
        aucBuf, sizeof(aucBuf), G_SEC_WTLS_CONNECTION_TYPE );
    WE_FREE(pucKeyId);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iResult = readfile("keyExchangeId2", &pucKeyId, &iKeyIdLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iResult = ISecW_CompSign((ISecW *)pISecW, iTargetID++, pucKeyId, iKeyIdLen, 
        aucBuf, sizeof(aucBuf), G_SEC_WTLS_CONNECTION_TYPE );
    WE_FREE(pucKeyId);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iResult = readfile("keyExchangeId3", &pucKeyId, &iKeyIdLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iResult = ISecW_CompSign((ISecW *)pISecW, iTargetID++, pucKeyId, iKeyIdLen, 
        aucBuf, sizeof(aucBuf), G_SEC_TLS_CONNECTION_TYPE );
    WE_FREE(pucKeyId);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iResult = readfile("keyExchangeId4", &pucKeyId, &iKeyIdLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iResult = ISecW_CompSign((ISecW *)pISecW, iTargetID++, pucKeyId, iKeyIdLen, 
        aucBuf, sizeof(aucBuf), G_SEC_SSL_CONNECTION_TYPE );
    WE_FREE(pucKeyId);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iResult = ISecW_CompSign((ISecW *)pISecW, iTargetID++, NULL, 0, 
        aucBuf, sizeof(aucBuf), G_SEC_WTLS_CONNECTION_TYPE );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_GetPrfResult( ISEC *pISecW )
{
    WE_INT32 iTargetID = 0;    
    WE_INT32 iResult  = 0;
    WE_UINT8 ucAlg = 1; 
    WE_INT32 iMasterSecretId = -1;
    WE_UCHAR *pucSecret = NULL; 
    WE_INT32 iSecretLen = 0;
    WE_CHAR *pcLabel = NULL; 
    WE_UCHAR *pucSeed = NULL;
    WE_INT32 iSeedLen = 0;
    WE_INT32 iOutputLen = 0;        
    
    ucAlg = 1;
    iMasterSecretId = 2;
    pcLabel = "client finish";
    iOutputLen = 16;
    iResult = readfile("secret", &pucSecret, &iSecretLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = readfile("seed", &pucSeed, &iSeedLen);
    if (iResult != G_SEC_OK)
    {
        WE_FREE(pucSecret);
        return iResult;
    }
    iResult = ISecW_GetPrfResult( (ISecW *)pISecW, iTargetID++, ucAlg, iMasterSecretId,
        pucSecret, iSecretLen, pcLabel, pucSeed,
        iSeedLen, iOutputLen, G_SEC_WTLS_CONNECTION_TYPE);
    WE_FREE(pucSecret);
    WE_FREE(pucSeed);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    return G_SEC_OK;
}


static WE_INT32 Test_ISecW_VerifySvrCertChain( ISEC *pISecW )
{
    WE_INT32 iTargetID = 0;    
    WE_INT32 iResult  = 0;
    WE_UCHAR *pucBuf = NULL;
    WE_INT32 iBufLen  = 0;
    WE_UCHAR *pucTmpBuf = NULL;
    WE_INT32 iTmpBufLen  = 0;
    
    iResult = readfile("certchain", &pucBuf, &iBufLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecW_VerifySvrCertChain( (ISecW *)pISecW, iTargetID++, pucBuf, iBufLen, G_SEC_WTLS_CONNECTION_TYPE); 
    WE_FREE(pucBuf);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }   
    
    iResult = readfile("cibchain", &pucBuf, &iBufLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecW_VerifySvrCertChain( (ISecW *)pISecW, iTargetID++, pucBuf, iBufLen, G_SEC_WTLS_CONNECTION_TYPE); 
    WE_FREE(pucBuf);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }   
    
    
    iResult = readfile("wtlscacert", &pucBuf, &iBufLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iTmpBufLen = iBufLen + 1;
    pucTmpBuf = (WE_UCHAR *)WE_MALLOC(iTmpBufLen * sizeof(WE_UCHAR));
    if (NULL == pucTmpBuf)
    {
        WE_FREE(pucBuf);
        return G_SEC_NOTENOUGH_MEMORY;
    }
    *pucTmpBuf = '1';
    (WE_VOID)WE_MEMCPY(pucTmpBuf + 1, pucBuf, iBufLen);        
    iResult = ISecW_VerifySvrCertChain( (ISecW *)pISecW, iTargetID++, pucTmpBuf, iTmpBufLen, G_SEC_WTLS_CONNECTION_TYPE); 
    WE_FREE(pucBuf);    
    WE_FREE(pucTmpBuf);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }   
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_RemovePeer( ISEC *pISecW )
{
    WE_INT32 iMasterSecretID = 0;    
    WE_INT32 iResult  = 0;
    
    iResult = ISecW_RemovePeer( (ISecW *)pISecW, iMasterSecretID++ );    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iResult = ISecW_RemovePeer( (ISecW *)pISecW, iMasterSecretID++ );    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iResult = ISecW_RemovePeer( (ISecW *)pISecW, iMasterSecretID++ );    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    iMasterSecretID = 11;
    iResult = ISecW_RemovePeer( (ISecW *)pISecW, iMasterSecretID );    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_SessionRenew( ISEC *pISecW )
{
    
    WE_INT32 iResult = -1;    
    WE_INT32 iMasterSecretId = -1; 
    WE_UINT8 ucSessionOptions = 0;
    WE_UCHAR aucSessionId[18] = {0}; 
    WE_UINT8 ucSessionIdLen = 0;
    WE_UINT8 aucCipherSuite[2] = {0};
    WE_CHAR  *pcSessionName = NULL;
    
    WE_UINT uiCreationTime = GetCurrentTime();
    
    pcSessionName = "12345678";
    iMasterSecretId = 0;
    ucSessionOptions = 5;
    ucSessionIdLen = WE_STRLEN(pcSessionName) + 1;
    WE_MEMCPY(aucSessionId, pcSessionName, ucSessionIdLen);
    iResult = ISecW_SessionRenew( (ISecW *)pISecW, iMasterSecretId, ucSessionOptions, aucSessionId,
        ucSessionIdLen, aucCipherSuite, 0,
        ( WE_UINT8 *)"\x00\x00\x00\x00", uiCreationTime); 
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    pcSessionName = "1";
    iMasterSecretId = 1;
    ucSessionOptions = 1;
    ucSessionIdLen = WE_STRLEN(pcSessionName) + 1;
    WE_MEMCPY(aucSessionId, pcSessionName, ucSessionIdLen);
    iResult = ISecW_SessionRenew( (ISecW *)pISecW, iMasterSecretId, ucSessionOptions, aucSessionId,
        ucSessionIdLen, aucCipherSuite, 0,
        ( WE_UINT8 *)"\x00\x00\x00\x00", uiCreationTime); 
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    
    pcSessionName = "3333";
    iMasterSecretId = 2;
    ucSessionOptions = 4;
    ucSessionIdLen = WE_STRLEN(pcSessionName) + 1;
    WE_MEMCPY(aucSessionId, pcSessionName, ucSessionIdLen);
    iResult = ISecW_SessionRenew( (ISecW *)pISecW, iMasterSecretId, ucSessionOptions, aucSessionId,
        ucSessionIdLen, aucCipherSuite, 0,
        ( WE_UINT8 *)"\x00\x00\x00\x00", uiCreationTime); 
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_AttachPeerToSession( ISEC *pISecW )
{
    
    WE_INT32 iResult  = -1;    
    WE_UCHAR aucAddress1[6] = {0,4,192,168,51,25};
    WE_UCHAR aucAddress2[6] = {0,4,43,65,51,1};    
    WE_UINT16 usPortnum = 0;
    WE_INT32 iMasterSecretId = -1;
    
    usPortnum= 9002;
    iMasterSecretId = 0;
    iResult = ISecW_AttachPeerToSession( (ISecW *)pISecW, aucAddress1, 6, usPortnum, 
        iMasterSecretId, G_SEC_WTLS_CONNECTION_TYPE);    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    usPortnum= 3434;
    iMasterSecretId = 3;
    iResult = ISecW_AttachPeerToSession( (ISecW *)pISecW, aucAddress2, 6, usPortnum, 
        iMasterSecretId, G_SEC_WTLS_CONNECTION_TYPE);    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    
    usPortnum= 6666;
    iMasterSecretId = 10;
    iResult = ISecW_AttachPeerToSession( (ISecW *)pISecW, aucAddress2, 6, usPortnum, 
        iMasterSecretId, G_SEC_WTLS_CONNECTION_TYPE);    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    
    usPortnum= 0;
    iMasterSecretId = 1;
    iResult = ISecW_AttachPeerToSession( (ISecW *)pISecW, aucAddress2, 6, usPortnum, 
        iMasterSecretId, G_SEC_WTLS_CONNECTION_TYPE);    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_EnableSession( ISEC *pISecW )
{
    
    WE_INT32 iResult  = -1;
    WE_INT32 iMasterSecretId = 0;
    WE_UINT8 ucIsActive = 0;    
    
    ucIsActive = 0;
    iMasterSecretId = 0;
    iResult = ISecW_EnableSession( (ISecW *)pISecW, iMasterSecretId, ucIsActive );    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }    
    
    ucIsActive = 1;
    iMasterSecretId = 3;
    iResult = ISecW_EnableSession( (ISecW *)pISecW, iMasterSecretId, ucIsActive );    
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }   
    
    return G_SEC_OK;
}


static WE_INT32 Test_ISecW_SetUpConnection( ISEC *pISecW )
{
    
    WE_INT32 iResult  = -1;
    St_SecSessionInformation     stSessionInfo = {0};
    WE_INT32 iTargetID = 0;
    WE_INT32 iMasterSecretId = 0;    
    WE_INT32 iSecurityId = 0;
    WE_INT32 iFullHandshake = 0;
    
    iMasterSecretId = 0;    
    iSecurityId = 2;
    iFullHandshake = 1;
    iResult = ISecW_SetUpConnection( (ISecW *)pISecW, iTargetID++, iMasterSecretId, iSecurityId,
        iFullHandshake, stSessionInfo, G_SEC_WTLS_CONNECTION_TYPE);   
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }      
    
    iMasterSecretId = 2;    
    iSecurityId = 4;
    iFullHandshake = 0;
    iResult = ISecW_SetUpConnection( (ISecW *)pISecW, iTargetID++, iMasterSecretId, iSecurityId,
        iFullHandshake, stSessionInfo, G_SEC_WTLS_CONNECTION_TYPE);   
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }          
    
    iMasterSecretId = 5;    
    iSecurityId = 0;
    iFullHandshake = 0;
    iResult = ISecW_SetUpConnection( (ISecW *)pISecW, iTargetID++, iMasterSecretId, iSecurityId,
        iFullHandshake, stSessionInfo, G_SEC_WTLS_CONNECTION_TYPE);   
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }                      
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecW_StopConnection( ISEC *pISecW )
{        
    WE_INT32 iResult  = -1;    
    WE_INT32 iTargetID = 0;
    WE_INT32 iSecurityId = 0;
    
    iSecurityId = 1;
    iResult = ISecW_StopConnection( (ISecW *)pISecW, iTargetID++, iSecurityId , G_SEC_WTLS_CONNECTION_TYPE);  
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }  
    
    iSecurityId = 0;
    iResult = ISecW_StopConnection( (ISecW *)pISecW, iTargetID++, iSecurityId , G_SEC_WTLS_CONNECTION_TYPE);  
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }                       
    
    iSecurityId = 10;
    iResult = ISecW_StopConnection( (ISecW *)pISecW, iTargetID++, iSecurityId , G_SEC_WTLS_CONNECTION_TYPE);  
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }     
    
    return G_SEC_OK;    
}

static WE_INT32 Test_ISecW_DisableSession( ISEC *pISecW )
{
    
    WE_INT32 iResult  = -1;
    WE_INT32 iMasterSecretId = 0;
    
    iMasterSecretId = 0;
    iResult = ISecW_DisableSession( (ISecW *)pISecW, iMasterSecretId );     
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }     
    
    iMasterSecretId = 3;
    iResult = ISecW_DisableSession( (ISecW *)pISecW, iMasterSecretId );     
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }     
    
    iMasterSecretId = 13;
    iResult = ISecW_DisableSession( (ISecW *)pISecW, iMasterSecretId );     
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }     
    
    return G_SEC_OK;    
}

static WE_INT32 Test_ISecB_GenKeyPair(ISEC *pISecB)
{
    WE_INT32 iTargetID = 0; 
    WE_UINT8 ucKeyType = 0;
    
    WE_INT32 iResult  = -1;
    
    ucKeyType = G_SEC_NONREPUDIATION_KEY;
    iResult = ISecB_GenKeyPair( (ISecB *)pISecB, iTargetID++, ucKeyType );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }

    ucKeyType = G_SEC_AUTHENTICATION_KEY;
    iResult = ISecB_GenKeyPair( (ISecB *)pISecB, iTargetID++, ucKeyType );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
   #if 0     
    ucKeyType = 3;
    iResult = ISecB_GenKeyPair( (ISecB *)pISecB, iTargetID++, ucKeyType );
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    #endif
    return G_SEC_OK;
}

static WE_INT32 Test_ISecB_GetRequestUserCert( ISEC *pISecB )
{
    WE_INT32   iResult  = -1; 
    St_SecUserCertReqInfo stShowContractContent = {0};
    stShowContractContent.iTargetID = 0;
    stShowContractContent.pucCountry = (WE_UINT8 *)"CN";
    stShowContractContent.pucCity = (WE_UINT8 *)"hangzhou";
    stShowContractContent.pucCompany = (WE_UINT8 *)"techfaith";
    stShowContractContent.pucDepartment = (WE_UINT8 *)"sec";
    stShowContractContent.pucEMail = (WE_UINT8 *)"stone_an@163.com";
    stShowContractContent.pucName = (WE_UINT8 *)"techfaith";
    stShowContractContent.pucProvince = (WE_UINT8 *)"zhejiang";
    stShowContractContent.usCityLen = 8;
    stShowContractContent.usCompanyLen = 9;
    stShowContractContent.usCountryLen = 2;
    stShowContractContent.usDepartmentLen = 3;
    stShowContractContent.usEMailLen = 16;
    stShowContractContent.usNameLen = 5;
    stShowContractContent.usProvinceLen = 8;
    
    stShowContractContent.iTargetID++;
    stShowContractContent.ucCertUsage = G_SEC_CERT_USAGE_SERVER_AUTH;
    stShowContractContent.ucKeyUsage = G_SEC_NONREPUDIATION_KEY;
    stShowContractContent.ucSignType = G_SEC_SIG_RSA_SHA;
    iResult = ISecB_GetRequestUserCert( (ISecB *)pISecB, stShowContractContent);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    stShowContractContent.iTargetID++;
    stShowContractContent.ucCertUsage = G_SEC_CERT_USAGE_SERVER_AUTH | G_SEC_CERT_USAGE_CLIENT_AUTH;
    stShowContractContent.ucKeyUsage = G_SEC_AUTHENTICATION_KEY;
    stShowContractContent.ucSignType = G_SEC_SIG_RSA_MD5;
    iResult = ISecB_GetRequestUserCert( (ISecB *)pISecB, stShowContractContent);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    stShowContractContent.iTargetID++;
    stShowContractContent.ucCertUsage = G_SEC_CERT_USAGE_SERVER_AUTH | G_SEC_CERT_USAGE_CLIENT_AUTH
        | G_SEC_CERT_USAGE_CODE_SIGN | G_SEC_CERT_USAGE_EMAIL_PROTECT;
    stShowContractContent.ucKeyUsage = G_SEC_AUTHENTICATION_KEY;
    stShowContractContent.ucSignType = G_SEC_SIG_RSA_MD2;
    iResult = ISecB_GetRequestUserCert( (ISecB *)pISecB, stShowContractContent);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }  
    
    return G_SEC_OK;
}


static WE_INT32 Test_ISecB_TransferCert( ISEC *pISecB )
{
    WE_INT32   iResult  = 0;
    St_SecCertContent stCertContent ={0};
    
    stCertContent.pcMime = "application/x-x509-ca-cert";
    iResult = readfile("rootcert1", &(stCertContent.pucCertData), (WE_INT32 *)&(stCertContent.uiCertLen));
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecB_TransferCert( (ISecB *)pISecB, stCertContent);
    WE_FREE(stCertContent.pucCertData);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    stCertContent.pcMime = "application/x-x509-ca-cert";
    iResult = readfile("rootcert2", &(stCertContent.pucCertData), (WE_INT32 *)&(stCertContent.uiCertLen));
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecB_TransferCert( (ISecB *)pISecB, stCertContent);
    WE_FREE(stCertContent.pucCertData);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }  
    
    stCertContent.pcMime = "application/x-x509-ca-cert";
    iResult = readfile("cacert1", &(stCertContent.pucCertData), (WE_INT32 *)&(stCertContent.uiCertLen));
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecB_TransferCert( (ISecB *)pISecB, stCertContent);
    WE_FREE(stCertContent.pucCertData);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    stCertContent.pcMime = "application/x-x509-ca-cert";
    iResult = readfile("cacert2", &(stCertContent.pucCertData), (WE_INT32 *)&(stCertContent.uiCertLen));
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecB_TransferCert( (ISecB *)pISecB, stCertContent);
    WE_FREE(stCertContent.pucCertData);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    
    stCertContent.pcMime = "application/x-x509-user-cert";
    iResult = readfile("usercert1", &(stCertContent.pucCertData), (WE_INT32 *)&(stCertContent.uiCertLen));
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecB_TransferCert( (ISecB *)pISecB, stCertContent);
    WE_FREE(stCertContent.pucCertData);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    
    stCertContent.pcMime = "application/x-x509-user-cert";
    iResult = readfile("usercert2", &(stCertContent.pucCertData), (WE_INT32 *)&(stCertContent.uiCertLen));
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }
    iResult = ISecB_TransferCert( (ISecB *)pISecB, stCertContent);
    WE_FREE(stCertContent.pucCertData);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecB_GetCertNameList( ISEC *pISecB )
{
    WE_INT32    iTargetID = 0; 
    WE_INT32    iResult  = 0;
    WE_INT32    iCertOptions = 0;    
    
    iCertOptions = G_SEC_USER_CERT;
    iResult = ISecB_GetCertNameList((ISecB *)pISecB, iTargetID++, iCertOptions);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iCertOptions = G_SEC_ROOT_CERT;
    iResult = ISecB_GetCertNameList((ISecB *)pISecB, iTargetID++, iCertOptions);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iCertOptions = G_SEC_USER_CERT | G_SEC_ROOT_CERT;
    iResult = ISecB_GetCertNameList((ISecB *)pISecB, iTargetID++, iCertOptions);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iCertOptions = 4;
    iResult = ISecB_GetCertNameList((ISecB *)pISecB, iTargetID++, iCertOptions);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecB_GetContractList( ISEC *pISecB )
{
    WE_INT32    iTargetID = 0; 
    WE_INT32    iResult  = 0;
    
    iResult = ISecB_GetContractsList((ISecB *)pISecB, iTargetID++); 
    if (iResult != G_SEC_OK)
    {
        return iResult;
    }  
    iResult = ISecB_GetContractsList((ISecB *)pISecB, iTargetID++); 
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecB_GetSessionInfo( ISEC *pISecB )
{
    WE_INT32    iTargetID = 0; 
    WE_INT32    iResult  = 0;
    WE_INT32    iSecID = 0;
    
    iSecID = 0;
    iResult = ISecB_GetSessionInfo((ISecB *)pISecB, iTargetID++, iSecID++);   
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iResult = ISecB_GetSessionInfo((ISecB *)pISecB, iTargetID++, iSecID++);   
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    
    return G_SEC_OK;
}

static WE_INT32 Test_ISecB_GetWtlsCurClass( ISEC *pISecB )
{
    WE_INT32    iTargetID = 0; 
    WE_INT32    iResult  = 0;
    WE_INT32    iSecID = 0;    
    
    iResult = ISecB_GetWtlsCurClass((ISecB *)pISecB, iTargetID++, iSecID++);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iResult = ISecB_GetWtlsCurClass((ISecB *)pISecB, iTargetID++, iSecID++);   
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    return G_SEC_OK;
}

static WE_INT32 Test_ISecB_GetCurSvrCert( ISEC *pISecB )
{
    WE_INT32    iTargetID = 0; 
    WE_INT32    iResult  = 0;
    WE_INT32    iSecID = 0;    
    
    iResult = ISecB_GetCurSvrCert((ISecB *)pISecB, iTargetID++, iSecID++);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iResult = ISecB_GetCurSvrCert((ISecB *)pISecB, iTargetID++, iSecID++);   
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    return G_SEC_OK;
}

#define USER_PRIVATE_KEY    "./shared/sec/user_privkey"
#define DATA_TO_BE_SIGN     "http://www.taobao.com"
static WE_INT32 Test_ISignTextHandle( ISEC *pISecS )
{
    
    WE_INT32 iTargetID = 0;
    WE_INT32 iResult = 0;    
    WE_INT32 iSignId = 0;
    WE_INT32 iKeyIdType = 0;
    WE_CHAR  *pcKeyId = NULL;
    WE_INT32 iKeyIdLen = 0;
    WE_UINT  uiStringToSignLen = WE_STRLEN(DATA_TO_BE_SIGN) + 1;
    WE_CHAR  *pcStringToSign = NULL;
    WE_CHAR  *pcText = "hello";
    WE_UCHAR *pucFile = NULL;
    WE_INT32  uiLen = 0;
    
    pcStringToSign = (char *)WE_MALLOC(uiStringToSignLen * sizeof(char));
    if (NULL == pcStringToSign)
    {
        perror("WE_MALLOC failed.\n");
        return G_SEC_NOTENOUGH_MEMORY;
    }
    WE_MEMCPY(pcStringToSign, DATA_TO_BE_SIGN, uiStringToSignLen);
    
    iKeyIdType = 0;    
    pcKeyId = NULL;
    iKeyIdLen = 0;
    iResult = ISignText_Handle( (ISignText *)pISecS,
        iTargetID++, 
        iSignId++,
        pcText,
        M_SEC_SIGN_NO_KEY,
        pcKeyId,
        iKeyIdLen,                                
        pcStringToSign,
        uiStringToSignLen,
        0);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iKeyIdType = 1;  
    iResult = readfile(USER_PRIVATE_KEY, &pucFile, &uiLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    pcKeyId = pucFile + 2 * sizeof(St_WimUCertKeyPairInfo) + 13;
    iKeyIdLen = 20;
    iResult = ISignText_Handle( (ISignText *)pISecS,
        iTargetID++, 
        iSignId++,
        pcText,
        M_SEC_SIGN_SHA_KEY,
        pcKeyId,
        iKeyIdLen,                                
        pcStringToSign,
        uiStringToSignLen,
        M_SEC_SIGNTEXT_RETURNHASHEDKEY);
    WE_FREE(pucFile);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    iKeyIdType = 2;  
    iResult = readfile(USER_PRIVATE_KEY, &pucFile, &uiLen);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    pcKeyId = pucFile + 2 * sizeof(St_WimUCertKeyPairInfo) + 13 + 20;
    iKeyIdLen = 20;
    iResult = ISignText_Handle( (ISignText *)pISecS,
        iTargetID++, 
        iSignId++,
        pcText,
        M_SEC_SIGN_SHA_CA_KEY,
        pcKeyId,
        iKeyIdLen,                                
        pcStringToSign,
        uiStringToSignLen,
        M_SEC_SIGNTEXT_RETURNCERT);
    WE_FREE(pucFile);
    if (iResult != G_SEC_OK)
    {
        return iResult;
    } 
    return G_SEC_OK;    
}

static WE_VOID NotifySignTextCallback 
( 
 WE_INT32 iEvent,
 WE_VOID *hSecAppEvent,        
 WE_VOID *pvPrivData
 )
{
    WE_HANDLE hHandle = (WE_HANDLE)pvPrivData;
    char acResult[12] = {0};
    switch ( iEvent )
    {
    case G_SIGNTEXT_RESP :
        {
            St_SecSignTextResp *pstSignResp = (St_SecSignTextResp *)hSecAppEvent;
            printf("*************************************\n");
            printf("received event G_SIGNTEXT_RESP !\n");
            printf("Response value: Target ID = %d\n", (WE_INT)(pstSignResp->iTargetID));
            printf("Response value: Sign ID = %d\n", (WE_INT)(pstSignResp->iSignId));
            printf("Response value: Type of cert = %d\n", pstSignResp->ucCertificateType);
            printf("Response value: Length of cert = %d\n", pstSignResp->usCertificateLen);
            printf("Response value: Length of hash = %d\n", pstSignResp->usHashedKeyLen);
            printf("Response value: length of signature = %d\n", pstSignResp->usSigLen);
            printf("Response value: Result = %d\n", pstSignResp->ucErr);
            printf("and write result to file !\n");
            printf("*************************************\n");
            if ( NULL == pstSignResp )
            {
                WriteResult( 
                    (char *)ISIGNTESTREPORT,                                
                    "sign Resp --error-- NULL pointer;\n",
                    WE_STRLEN("sign Resp --error-- NULL pointer;\n")
                    );
                break;
            }
            WriteResult( 
                (char *)ISIGNTESTREPORT,                                
                "sign Resp result is;",
                WE_STRLEN("sign Resp result is;")
                );
            IntToString( acResult, pstSignResp->ucErr );
            WriteResult( 
                (char *)ISIGNTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISIGNTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            break;
        }
        
    case G_Evt_SELECTCERT:
        {
            /*create dialog*/
            /*WE_FREE mem*/
            /*dialog action and call sec IF */
            St_SelectCertAction stChooseCerByName = {0};
            stChooseCerByName.bIsOk = 1;
            stChooseCerByName.iSelId = 0;
            /*encode action*/
            ISignText_EvtSelectCertAction(hHandle, 0, stChooseCerByName);
            break;
        }
        
        
    default :
        WriteResult( 
            (char *)ISIGNTESTREPORT,                                
            "sign response is not expect event;\n",
            WE_STRLEN("sign response is not expect event;\n")
            );
    }
}


static WE_VOID NotifyBrowCallback
( 
 WE_INT32 iEvent,
 WE_VOID *hSecAppEvent,        
 WE_VOID *pvPrivData
 )
{
    ISecB *pISecB = (ISecB *)pvPrivData;
    WE_CHAR acResult[12] = {0};
    switch(iEvent)
    {
        /*Response*/
    case G_GENKEYPAIR_RESP:
        {
            printf( "receive sec G_GENKEYPAIR_RESP\n" );
            {
                St_SecGenKeyPairResp* pstData = (St_SecGenKeyPairResp*)hSecAppEvent;  
                if (NULL == pstData)
                {
                    WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                        "NotifyBrowCallback G_GENKEYPAIR_RESP--error-- NULL response pointer \n",
                        WE_STRLEN("NotifyBrowCallback G_GENKEYPAIR_RESP--error-- NULL response pointer \n")
                        );
                    printf("NotifyBrowCallback G_GENKEYPAIR_RESP--error-- NULL response pointer \n");
                    break ;
                }
                
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "receive sec G_GENKEYPAIR_RESP;", WE_STRLEN("receive sec G_GENKEYPAIR_RESP; "));                
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "serach result is:", WE_STRLEN("serach result is:"));
                IntToString( acResult, pstData->usResult );
                WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
                WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
                
                printf( "receive sec G_GENKEYPAIR_RESP result %d\n", pstData->usResult );                
            }
            break;
        }
        
    case G_USERCERTREQ_RESP:
        {
            printf( "receive sec G_USERCERTREQ_RESP\n" );
            {
                St_SecUserCertRequestResp*  pstData = (St_SecUserCertRequestResp*)hSecAppEvent;  
                if (NULL == pstData)
                {
                    WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                        "NotifyBrowCallback G_USERCERTREQ_RESP--error-- NULL response pointer \n",
                        WE_STRLEN("NotifyBrowCallback G_USERCERTREQ_RESP--error-- NULL response pointer \n")
                        );
                    printf("NotifyBrowCallback G_USERCERTREQ_RESP--error-- NULL response pointer \n");
                    break ;
                }
                
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "receive sec G_USERCERTREQ_RESP; ", WE_STRLEN("receive sec G_USERCERTREQ_RESP; "));                
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "serach result is:", WE_STRLEN("serach result is:"));
                IntToString( acResult, pstData->iResult );
                WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
                WriteResult(  (char *)ISECBROWSERTESTREPORT, ";", WE_STRLEN(";"));
                
                WriteResult(  USERCERTREQ, (char *)pstData->pucCertReqMsg, pstData->uiCertReqMsgLen);                
                WriteResult(  USERCERTREQ, "\n", WE_STRLEN("\n"));
                printf( "receive sec G_USERCERTREQ_RESP result %d\n", (WE_INT)(pstData->iResult ));                
            }
            break;
        }
        
    case G_Evt_HASH:
        {
            St_Hash* pstData = (St_Hash*)hSecAppEvent;
            St_HashAction stHash = {0};
            printf( "receive sec G_Evt_HASH\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_HASH--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_HASH--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_HASH--error-- NULL response pointer \n");
                break ;
            }
            
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "receive sec G_Evt_HASH; ", WE_STRLEN("receive sec G_Evt_HASH; ")); 
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "Display Name is:", WE_STRLEN("Display Name is:"));
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                pstData->pcDisplayName, WE_STRLEN(pstData->pcDisplayName));
            WriteResult(  USERCERTREQ, "\n", WE_STRLEN("\n"));
            printf("Display Name is: %s\n", pstData->pcDisplayName );
            
            stHash.pcHashValue = "613786052902159285544924458810";
            stHash.bIsOk = TRUE;
            printf(" Hash value: %s\n", stHash.pcHashValue );
            ISecB_EvtHashAction(pISecB, 0, stHash);  
            break;
        }
        
        
    case G_Evt_STORECERT:
        {
            St_StoreCert* pstEvt = (St_StoreCert*)hSecAppEvent;
            printf( "receive sec G_Evt_STORECERT\n" );
            if (NULL == pstEvt)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_STORECERT--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_STORECERT--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_STORECERT--error-- NULL response pointer \n");
                break;
            }
            
            if ('C' == pstEvt->cCertType)
            {
                St_StoreCertAction stStoreCert = {0};
                stStoreCert.pcText = NULL;
                stStoreCert.bIsOk = 1;       
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "store ca cert action is ok!\n", WE_STRLEN("store ca cert action is ok!\n"));                  
                ISecB_EvtStoreCertAction(pISecB,0,stStoreCert);
            }
            if ('U' == pstEvt->cCertType)
            { 
                St_StoreCertAction stStoreCert = {0};
                if (NULL != pstEvt->pcSubjectStr)
                {
                    stStoreCert.pcText = NULL;
                    stStoreCert.bIsOk = 1;   
                    WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                        "store user cert action is using subject as name!\n", WE_STRLEN("store cert action is using subject as name!\n"));                       
                }
                else
                {
                    stStoreCert.pcText = "hello";
                    stStoreCert.bIsOk = 1;   
                    WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                        "store user cert action is using hello as name!\n", WE_STRLEN("store cert action is using hello as name!\n"));                       
                    
                }                 
                ISecB_EvtStoreCertAction(pISecB, 0, stStoreCert);
            } 
            break;
        }
        
        
    case G_Evt_CONFIRM:
        {    
            St_Confirm* pstData = (St_Confirm*)hSecAppEvent; 
            St_ConfirmAction stConfirm = {0};
            printf( "receive sec G_Evt_CONFIRM\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_CONFIRM--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_CONFIRM--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_CONFIRM--error-- NULL response pointer \n");
                break ;
            } 
            
            stConfirm.bIsOk = 1;   
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "confirm action is ok!\n", WE_STRLEN("confirm action is ok!\n"));  
            ISecB_EvtConfirmAction(pISecB, 0, stConfirm);
            break;
        }
        
        
    case G_Evt_NAMECONFIRM:
        {   
            St_NameConfirm* pstData = (St_NameConfirm*)hSecAppEvent;
            St_NameConfirmAction stNameConfirm = {0};
            printf( "receive sec G_Evt_NAMECONFIRM\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_NAMECONFIRM--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_NAMECONFIRM--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_NAMECONFIRM--error-- NULL response pointer \n");
                break ;
            } 
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "name is:", WE_STRLEN("name is:"));
            WriteResult(  (char *)ISECBROWSERTESTREPORT, pstData->pcCertName, WE_STRLEN(pstData->pcCertName)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            stNameConfirm.bIsOk = 1;     
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "name confirm action is ok!\n", WE_STRLEN("name confirm action is ok!\n"));        
            ISecB_EvtNameConfirmAction(pISecB, 0, stNameConfirm); 
            break;
        }
        
        
    case G_Evt_SHOWCERTLIST:
        { 
            St_ShowCertList* pstData = (St_ShowCertList*)hSecAppEvent;
            St_ShowCertListAction stShowCertList = {0};
            static WE_INT32 i = 0;
            printf( "receive sec G_Evt_SHOWCERTLIST\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCERTLIST--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_SHOWCERTLIST--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_SHOWCERTLIST--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "receive sec G_Evt_SHOWCERTLIST; \n", WE_STRLEN("receive sec G_Evt_SHOWCERTLIST; \n"));
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "number of cert is:", WE_STRLEN("number of cert is:"));
            IntToString( acResult, pstData->nbrOfCerts );
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            if (0 == i % 3)
            {
                stShowCertList.iSelId = 0;
                stShowCertList.bIsOk = 1;    /* ok */                
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "show cert list action is ok!\n", WE_STRLEN("show cert list action is ok!\n"));
            }
            else if (1 == i % 3)
            {
                stShowCertList.iSelId = 0;
                stShowCertList.bIsOk = 0;
                stShowCertList.eRKeyType = 0; /* change wtls cert to diable. */
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "show cert list action is change wtls to disable!\n", WE_STRLEN("show cert list action is change wtls to disable!\n"));
            }
            else
            {
                stShowCertList.iSelId = 0;
                stShowCertList.bIsOk = 0;
                stShowCertList.eRKeyType = 2; /* cancel */
                WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                    "show cert list action is cancel!\n", WE_STRLEN("show cert list action is cancel!\n"));
            }
            ISecB_EvtShowCertListAction(pISecB, 0, stShowCertList);
            i++;
            break;
        }
        
        
    case G_Evt_SHOWCERTCONTENT: /* need change */
        {    
            St_ShowCertContent* pstData = (St_ShowCertContent*)hSecAppEvent;
            St_ShowCertContentAction stShowCert = {0};
            printf( "receive sec G_Evt_SHOWCERTCONTENT\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCERTCONTENT--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_SHOWCERTCONTENT--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_SHOWCERTCONTENT--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                (char *)"receive sec G_Evt_SHOWCERTCONTENT; ", WE_STRLEN("receive sec G_Evt_SHOWCERTCONTENT; "));
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "subject of cert is:", WE_STRLEN("subject of cert is:"));
            WriteResult(  (char *)ISECBROWSERTESTREPORT, pstData->pcSubjectStr, WE_STRLEN(pstData->pcSubjectStr)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            stShowCert.bIsOk = 0;  /* delete */
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "action of show cert content is delete\n", WE_STRLEN("action of show cert content is delete\n"));
            printf("delete the cert. \n");
            ISecB_EvtShowCertContentAction(pISecB, 0, stShowCert);
            break;
        }
        
        
    case G_DELCERT_RESP:
        {
            St_SecRemCertResp* pstData = (St_SecRemCertResp*)hSecAppEvent;
            printf( "receive sec G_DELCERT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_DELCERT_RESP--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_DELCERT_RESP--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_DELCERT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "receive sec G_DELCERT_RESP; ", WE_STRLEN("receive sec G_DELCERT_RESP; "));
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "serach result is:", WE_STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            WE_MEMSET(acResult, 0, sizeof(acResult));
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "ID of delete cert is:", WE_STRLEN("ID of delete cert is:"));
            IntToString( acResult, pstData->ucCertId);
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            printf("result is: %d\n", pstData->usResult); 
            break;
        }
        
        
    case G_VIEWGETCERT_RESP:
        {
            St_SecGetCertContentResp* pstData = (St_SecGetCertContentResp*)hSecAppEvent;
            printf( "receive sec G_VIEWGETCERT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_VIEWGETCERT_RESP--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_VIEWGETCERT_RESP--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_VIEWGETCERT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "receive sec G_VIEWGETCERT_RESP; ", WE_STRLEN("receive sec G_VIEWGETCERT_RESP; "));
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "serach result is:", WE_STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            WE_MEMSET(acResult, 0, sizeof(acResult));
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "ID of cert is:", WE_STRLEN("ID of cert is:"));
            IntToString( acResult, pstData->ucCertId);
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            printf("result is: %d\n", pstData->usResult);
            break;
        }
        
        
    case G_GETCONTRACT_RESP:
        {     
            St_SecGetContractsListResp* pstData = (St_SecGetContractsListResp *)hSecAppEvent;
            printf( "receive sec G_GETCONTRACT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_GETCONTRACT_RESP--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_GETCONTRACT_RESP--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_GETCONTRACT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "receive sec G_GETCONTRACT_RESP; ", WE_STRLEN("receive sec G_GETCONTRACT_RESP; "));
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "serach result is:", WE_STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            printf("result is: %d\n", pstData->usResult);
            break;
        }
        
        
    case G_DELCONTRACT_RESP:
        {     
            St_SecDelContractResp* pstData = (St_SecDelContractResp*)hSecAppEvent;
            printf( "receive sec G_DELCONTRACT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_DELCONTRACT_RESP--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_DELCONTRACT_RESP--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_DELCONTRACT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "receive sec G_DELCONTRACT_RESP; \n", WE_STRLEN("receive sec G_DELCONTRACT_RESP; \n"));
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "serach result is:", WE_STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            printf("result is: %d\n", pstData->usResult);
            break;
        }
        
        
    case G_Evt_SHOWCONTRACTLIST:
        {    
            St_ShowContractsList* pstData = (St_ShowContractsList*)hSecAppEvent;
            St_ShowContractsListAction  stContractList = {0};
            static WE_INT32 i = 0;            
            
            printf( "receive sec G_Evt_SHOWCONTRACTLIST\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCONTRACTLIST--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_SHOWCONTRACTLIST--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_SHOWCONTRACTLIST--error-- NULL response pointer \n");
                break ;
            } 
            WriteResult( (char *)ISECBROWSERTESTREPORT,                                
                "receive sec G_Evt_SHOWCONTRACTLIST; \n", WE_STRLEN("receive sec G_Evt_SHOWCONTRACTLIST; \n"));
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "number of contracts is:", WE_STRLEN("number of contracts is:"));
            IntToString( acResult, pstData->nbrOfContract);
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            printf("number of contracts is: %d\n", (WE_INT)(pstData->nbrOfContract));            
            if (0 == i % 2)
            {             
                stContractList.iSelId = 0;
                stContractList.bIsOk = 1;
            }
            else /* cancel */
            {
                stContractList.bIsOk = 0;
            }
            ISecB_EvtShowContractsListAction(pISecB, 0, stContractList);
            i++; 
            break;
        }
        
        
    case G_Evt_SHOWCONTRACTCONTENT:
        {       
            St_ShowContractContent* pstData = (St_ShowContractContent*)hSecAppEvent;
            St_ShowContractContentAction stShowContractContent = {0};
            printf( "receive sec G_Evt_SHOWCONTRACTCONTENT\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCONTRACTCONTENT--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_SHOWCONTRACTCONTENT--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_SHOWCONTRACTCONTENT--error-- NULL response pointer \n");
                break ;
            }
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "signature of contract is:", WE_STRLEN("signature of contract is:"));
            WriteResult(  (char *)ISECBROWSERTESTREPORT, pstData->pcSignatureStr, WE_STRLEN(pstData->pcSignatureStr)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            stShowContractContent.bIsOk = 0; /* delete */     
            WriteResult(  (char *)ISECBROWSERTESTREPORT, 
                "show contract action is delete.\n", WE_STRLEN("show contract action is delete.\n"));
            ISecB_EvtShowContractContentAction(pISecB, 0, stShowContractContent);
            break;
        }
        
        
    case G_Evt_SHOWSESSIONCONTENT:
        {      
            St_ShowSessionContent* pstData = (St_ShowSessionContent*)hSecAppEvent;
            printf( "receive sec G_Evt_SHOWSESSIONCONTENT\n" ); 
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWSESSIONCONTENT--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_Evt_SHOWSESSIONCONTENT--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_Evt_SHOWSESSIONCONTENT--error-- NULL response pointer \n");
                break ;
            }
            break;
        }
        
        
    case G_GETSESSIONINFO_RESP:
        {        
            St_SecGetSessionInfoResp* pstData = (St_SecGetSessionInfoResp*)hSecAppEvent;
            printf( "receive sec G_GETSESSIONINFO_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_GETSESSIONINFO_RESP--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_GETSESSIONINFO_RESP--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_GETSESSIONINFO_RESP--error-- NULL response pointer \n");
                break ;
            }
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "serach result is:", WE_STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            break;
        }
        
        
    case G_WTLSCURCLASS_RESP:
        {          
            St_SecGetWtlsCurClassResp* pstData = (St_SecGetWtlsCurClassResp*)hSecAppEvent;
            printf( "receive sec G_WTLSCURCLASS_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_WTLSCURCLASS_RESP--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_WTLSCURCLASS_RESP--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_WTLSCURCLASS_RESP--error-- NULL response pointer \n");
                break ;
            }
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "SEC class is:", WE_STRLEN("SEC class is:"));
            IntToString( acResult, pstData->ucSecClass);
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            WE_MEMSET(acResult, 0, sizeof(acResult));
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "session ready is:", WE_STRLEN("session ready is:"));
            IntToString( acResult, pstData->ucSessionInfoReady);
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            
            WE_MEMSET(acResult, 0, sizeof(acResult));
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "cert ready is:", WE_STRLEN("cert ready is:"));
            IntToString( acResult, pstData->ucCertReady);
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            break;
        }
        
        
    case G_GETCURSVRCERT_RESP:
        {      
            St_SecGetCurSvrCertResp* pstData = (St_SecGetCurSvrCertResp*)hSecAppEvent;
            printf( "receive sec G_GETCURSVRCERT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult( (char *)ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_GETCURSVRCERT_RESP--error-- NULL response pointer \n",
                    WE_STRLEN("NotifyBrowCallback G_GETCURSVRCERT_RESP--error-- NULL response pointer \n")
                    );
                printf("NotifyBrowCallback G_GETCURSVRCERT_RESP--error-- NULL response pointer \n");
                break ;
            }
            
            WriteResult(  (char *)ISECBROWSERTESTREPORT,                                
                "serach result is:", WE_STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult(  (char *)ISECBROWSERTESTREPORT, acResult, WE_STRLEN(acResult)); 
            WriteResult(  (char *)ISECBROWSERTESTREPORT, "\n", WE_STRLEN("\n"));
            break;
        }            
        
    default:
        break;
    }
}

static WE_VOID NotifyWapCallback
( 
 WE_INT32 iEvent,
 WE_VOID *hSecAppEvent,        
 WE_VOID *pvPrivData
 )
{
    char acResult[12] = {0,};
    switch ( iEvent  )
    {
    case G_SEARCHPEER_RESP :
        {
            
            {
                St_SecSearchPeerResp *pstSearchPeer = (St_SecSearchPeerResp *)hSecAppEvent;
                if (NULL == pstSearchPeer)
                {
                    WriteResult( 
                        (char *)ISECWAPTESTREPORT,                                
                        "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- NULL response pointer ",
                        WE_STRLEN("WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- NULL response pointer ")
                        );
                    
                    printf( "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- NULL response pointer");
                    break ;
                }
                
                WriteResult( 
                    (char *)ISECWAPTESTREPORT,                                
                    "receive sec G_SEARCHPEER_RESP;",
                    WE_STRLEN("receive sec G_SEARCHPEER_RESP;")
                    );
                
                WriteResult( 
                    (char *)ISECWAPTESTREPORT,                                
                    "serach result is ",
                    WE_STRLEN("serach result is ")
                    );
                IntToString( acResult,pstSearchPeer->usResult );                
                
                WriteResult( 
                    (char *)ISECWAPTESTREPORT,                                
                    acResult,
                    WE_STRLEN(acResult)
                    );
                WriteResult( 
                    (char *)ISECWAPTESTREPORT,                                
                    "\n",
                    WE_STRLEN("\n")
                    );
                printf( "receive sec G_SEARCHPEER_RESP result %d\n",pstSearchPeer->usResult );
                break;
            }
        }
        
    case G_SESSIONGET_RESP :            
        {
            St_SecSessionGetResp *pstSessionFetch = (St_SecSessionGetResp *)hSecAppEvent;
            if (NULL == pstSessionFetch)
            {
                WriteResult( 
                    (char *)ISECWAPTESTREPORT,                                
                    "G_SESSIONGET_RESP --error-- NULL response pointer",
                    WE_STRLEN("G_SESSIONGET_RESP --error-- NULL response pointer")
                    );
                printf("G_SESSIONGET_RESP --error-- NULL response pointer\n");
                break ;
            }
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_SESSIONGET_RESP;",
                WE_STRLEN("receive sec G_SESSIONGET_RESP;")
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "session get result is ",
                WE_STRLEN("session get result is ")
                );
            IntToString( acResult,pstSessionFetch->usResult );                
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            break;
        }
        
    case G_GETCIPHERSUITE_RESP :
        
        {
            St_SecGetCipherSuiteResp *pstGetCipherSuite = (St_SecGetCipherSuiteResp *)hSecAppEvent;
            
            if (NULL == pstGetCipherSuite)
            {
                
                printf( "WTLS:WapWtls_ProcSecResp GETCIPHERSUITE--error-- no memory space!\n");
                return ;
            }      
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_GETCIPHERSUITE_RESP;",
                WE_STRLEN("receive sec G_GETCIPHERSUITE_RESP;")
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "get cipher suites result is ",
                WE_STRLEN("get cipher suites result is ")
                );
            IntToString( acResult,pstGetCipherSuite->usResult );                
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            
            break;
        }
        
    case G_KEYEXCH_RESP :
        
        {
            St_SecKeyExchResp *pstKeyExch = (St_SecKeyExchResp*)hSecAppEvent;
            if (NULL == pstKeyExch)
            {
                printf( "WTLS:WapWtls_ProcSecResp G_KEYEXCH_RESP--error-- no memory space!\n");
                return ;
            }
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_KEYEXCH_RESP;",
                WE_STRLEN("receive sec G_KEYEXCH_RESP;")
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "key exchange result is ",
                WE_STRLEN("key exchange result is ")
                );
            IntToString( acResult,pstKeyExch->usResult );                
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            break;
        }
        
    case  G_VERIFYSVRCERT_RESP :
        
        {
            St_SecVerifySvrCertResp *pstVerSvrCert = (St_SecVerifySvrCertResp *)hSecAppEvent;
            if (NULL == pstVerSvrCert)
            {
                printf( "WTLS:WapWtls_ProcSecResp G_VERIFYSVRCERT_RESP--error-- no memory space!\n");
                return ;
            }
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_VERIFYSVRCERT_RESP;",
                WE_STRLEN("receive sec G_VERIFYSVRCERT_RESP;")                            
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "verify server certificate result is ",
                WE_STRLEN("verify server certificate result is ")
                );
            IntToString( acResult,pstVerSvrCert->usResult );                
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            break;
        }
    case G_GETPRF_RESP :
        
        {
            St_SecGetPrfResp *pstPrf = (St_SecGetPrfResp *)hSecAppEvent;
            
            if (NULL == pstPrf)
            {
                
                printf( "WTLS:WapWtls_ProcSecResp G_GETPRF_RESP--error-- no memory space!\n");
                return ;
            }
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_GETPRF_RESP;",
                WE_STRLEN("receive sec G_GETPRF_RESP;")
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "prf result is ",
                WE_STRLEN("prf result is ")
                );
            IntToString( acResult,pstPrf->usResult );                
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            break;
        }
        
    case G_GETUSERCERT_RESP :
        
        {
            St_SecGetUserCertResp *pstGetUserCert = (St_SecGetUserCertResp *)hSecAppEvent;
            if (NULL == pstGetUserCert)
            {
                printf( "WTLS:WapWtls_ProcSecResp G_GETUSERCERT_RESP--error-- no memory space!\n");
                return ;
            }
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_GETUSERCERT_RESP;",
                WE_STRLEN("receive sec G_GETUSERCERT_RESP;")
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "get user certificate result is ",
                WE_STRLEN("get user certificate result is ")
                );
            IntToString( acResult,pstGetUserCert->usResult );                
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            break;
        }
        
    case G_COMPUTESIGN_RESP :
        
        {
            St_SecCompSignResp *pstCompSig = (St_SecCompSignResp *)hSecAppEvent;
            if (NULL == pstCompSig)
            {
                
                printf( " G_COMPUTESIGN_RESP--error-- no memory space!\n");
                return ;
            }
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_COMPUTESIGN_RESP;",
                WE_STRLEN("receive sec G_COMPUTESIGN_RESP;")
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "compute signature result is ",
                WE_STRLEN("compute signature result is ")
                );
            IntToString( acResult,pstCompSig->usResult );                
            
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                acResult,
                WE_STRLEN(acResult)
                );
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "\n",
                WE_STRLEN("\n")
                );
            break;
        }
        
        
    case G_Evt_SHOW_DLG :
        
        {
            St_ShowDlg *pstShowDlg = (St_ShowDlg *)hSecAppEvent;
            if (NULL == pstShowDlg)
            {
                printf( "WTLS:WapWtls_ProcSecResp G_Evt_SHOW_DLG--error-- no memory space!\n");
                return ;
            }
            WriteResult( 
                (char *)ISECWAPTESTREPORT,                                
                "receive sec G_Evt_SHOW_DLG;\n",
                WE_STRLEN("receive sec G_Evt_SHOW_DLG;\n")
                ); 
            
            break;
        }
        
    default :
        WriteResult( 
            (char *)ISECERRORREPORT,                                
            "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- not expect event!\n",
            WE_STRLEN("WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- not expect event!\n")
            );
        printf( "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- not expect event!\n");
    }    
}

static WE_INT32 readfile(WE_CHAR *pcFileName, WE_UCHAR **ppucFile, WE_INT32 *piLen)
{
    WE_INT32         fd = 0;
    struct stat buf = {0};
    WE_INT32    iResult = 0;
    
    if ((NULL == ppucFile) || (NULL == piLen))
    {
        return G_SEC_INVALID_PARAMETER;
    }
    
    *ppucFile = NULL;
    *piLen = 0;    
    if ((fd = open(pcFileName, O_RDONLY, 0644)) < 0)
    {
        perror("can't open");
        return G_SEC_GENERAL_ERROR;
    }
    else
    {
        printf("open %s\n", pcFileName);
        printf("descriptor is %d\n", (WE_INT)(fd));
    }
    
    if ((fstat(fd, &buf)) < 0)
    {
        perror("fstat fail. \n");
        close(fd);
        return G_SEC_GENERAL_ERROR;
    }    
    
    *ppucFile = ( WE_UCHAR *)WE_MALLOC(buf.st_size * sizeof(WE_UCHAR));
    if (NULL == *ppucFile)
    {
        perror("WE_MALLOC fail.\n");
        close(fd);
        return G_SEC_NOTENOUGH_MEMORY;
    }
    
    iResult = read(fd, *ppucFile, buf.st_size);
    if (iResult < 0)
    {
        WE_FREE(*ppucFile);
        perror("read file fail.\n");
        close(fd);
        return G_SEC_GENERAL_ERROR;
    }    
    *piLen = buf.st_size;
    close(fd);
    return G_SEC_OK;
}

static WE_VOID WriteResult(char * pcFileName, const char *pcBuffer, WE_INT32 iBufferLength )
{    
    WE_INT  fd = 0;
    WE_INT32  iBytes = 0;
    WE_INT32  iRes = 0;
    
    /*if file exists, then append , otherwise create it. */
    if ((fd = open(pcFileName, O_CREAT | O_WRONLY | O_APPEND)) < 0)
    {
        perror("can't open\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("open %s\n", pcFileName);
        printf("descriptor is %d\n", fd);
    }
    
    /* move file pointer to file end. */
    iRes = lseek(fd, SEEK_END, 0);
    if (-1 == iRes)
    {
        perror("ERROR - seek error!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if ((pcBuffer != NULL) && (iBufferLength > 0))
    {
        iBytes = write( fd, pcBuffer, iBufferLength );
        printf( "%d\n", (WE_INT)(iBufferLength ));
        printf( "%s\n",pcBuffer );
        if( iBytes < 0 )
        {
            perror("ERROR - write buffer!\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }   
    
    close(fd);
}


static WE_VOID IntToString( char *pszString,WE_INT32 iConvet )
{
    char acResult[12] = {0,};
    WE_INT32 i = 0;
    WE_INT32 j = 0;
    
    if ( iConvet < 0 )
    {
        pszString[j++] = '-';
        iConvet = -iConvet;
        
    }
    if ( iConvet >= 0 && iConvet <= 9 )
    {
        pszString[j++] = iConvet + 0x30;
        
        pszString[j++] = 0;
        return;
    }
    while (1)
    {
        
        if ( iConvet < 10 )
        {
            acResult[i] = iConvet + 0x30;
            break;
        }
        
        acResult[i++] = iConvet%10 + 0x30;
        iConvet = iConvet/10;        
    }
    while ( i )
    {
        
        pszString[j++] = acResult[i--];
    }
    pszString[j++] = acResult[i];
    
    pszString[j] = 0;
    
}

static WE_UINT GetCurrentTime( WE_VOID )
{
    WE_UINT uiTmpTime = 0;  
    
    uiTmpTime = time((time_t *)NULL);
    return uiTmpTime;
}

static WE_VOID thread_callback(ISEC *pISec)
{
    WE_UINT uiRes = 0;
    
    printf("into secapp callback\n");
    while(1)
    {
        uiRes = ISec_ExcuteCallBack(pISec);   
        if (uiRes != G_SEC_OK)
        {
            return;
        }
    }
}

static WE_VOID thread_Wcallfunction(ISEC *pISec)
{
    WE_INT32  iResult = 0;
    
    iResult = Test_ISecW_SearchPeer( pISec );
    iResult = Test_ISecW_SessionGet( pISec );  
    iResult = Test_ISecW_GetCipherSuite( pISec );
    iResult = Test_ISecW_KeyExchange( pISec ); 
    iResult = Test_ISecW_GetUserCert( pISec );         
    iResult = Test_ISecW_GetPrfResult( pISec );
    iResult = Test_ISecW_VerifySvrCertChain( pISec );
    iResult = Test_ISecW_RemovePeer( pISec );
    iResult = Test_ISecW_SessionRenew( pISec );
    iResult = Test_ISecW_AttachPeerToSession( pISec );
    iResult = Test_ISecW_EnableSession( pISec );
    iResult = Test_ISecW_SetUpConnection( pISec );
    iResult = Test_ISecW_StopConnection( pISec );
    iResult = Test_ISecW_DisableSession( pISec );
    iResult = Test_ISecW_CompSign( pISec ); 
    iResult = Test_ISecW_RemovePeer( pISec );
}


static WE_VOID thread_Bcallfunction(ISEC *pISecB)
{
    WE_INT32  iResult = 0;
    
    iResult = Test_ISecB_GenKeyPair( pISecB );
    #if 0
    iResult = Test_ISecB_GetRequestUserCert( pISecB );
    iResult = Test_ISecB_TransferCert(pISecB);
    iResult = Test_ISecB_GetCertNameList(pISecB);
    iResult = Test_ISecB_GetContractList(pISecB);
    iResult = Test_ISecB_GetSessionInfo(pISecB);
    iResult = Test_ISecB_GetWtlsCurClass(pISecB);
    iResult = Test_ISecB_GetCurSvrCert(pISecB);
    #endif
}

static WE_VOID thread_Scallfunction(ISEC *pISecS)
{
    WE_INT32  iResult = 0;    
    
    iResult = Test_ISignTextHandle( pISecS );   
}

typedef WE_VOID *(* THREAD)(WE_VOID *);
#define MAX_THREAD_NUMBER	10

int main(int argc, char *argv[])
{
    ISEC        *pISecW = NULL;
    ISEC        *pISecB = NULL;
    ISEC        *pISecS = NULL;
    WE_UINT     uiRes = 0;
    WE_INT32    iType = 0;
    WE_INT32    iResult = 0;
    pthread_t   thread[MAX_THREAD_NUMBER];
    WE_INT32    i = 0;
    WE_INT32    max = 0;

    #if 0
    iType = E_SEC_WTLS;    
    uiRes = ISec_Create(iType, &pISecW);
    if (G_SEC_OK != uiRes)
    {
        return uiRes;
    }
   #endif
    iType = E_SEC_BRS;    
    uiRes = ISec_Create(iType, &pISecB);
    if (G_SEC_OK != uiRes)
    {
        ISec_Release(pISecW);
        return uiRes;
    }
    
    #if 0
    iType = E_SEC_SIGNTEXT;    
    uiRes = ISec_Create(iType, &pISecS);
    if (G_SEC_OK != uiRes)
    {
        ISec_Release(pISecW);
        ISec_Release(pISecB);
        return uiRes;
    } 
    #endif
    #if 0
    uiRes = ISec_RegClientEventCB(pISecW, pISecW, (Fn_ISecEventHandle)NotifyWapCallback);
    if (G_SEC_OK != uiRes)
    {
        ISec_Release(pISecW);
        ISec_Release(pISecB);
        ISec_Release(pISecS);
        return uiRes;
    }    
    #endif
    uiRes = ISec_RegClientEventCB(pISecB, pISecB, (Fn_ISecEventHandle)NotifyBrowCallback);
    if (G_SEC_OK != uiRes)
    {
        ISec_Release(pISecW);
        ISec_Release(pISecB);
        ISec_Release(pISecS);
        return uiRes;
    }   
    #if 0
    uiRes = ISec_RegClientEventCB(pISecS, pISecS, (Fn_ISecEventHandle)NotifySignTextCallback);
    if (G_SEC_OK != uiRes)
    {
        ISec_Release(pISecW);
        ISec_Release(pISecB);
        ISec_Release(pISecS);
        return uiRes;
    }   
    #endif
    i = 0;    
//    iResult = pthread_create(&(thread[i++]), NULL, (THREAD)thread_callback, (WE_VOID *)pISecW);
    iResult = pthread_create(&(thread[i++]), NULL, (THREAD)thread_callback, (WE_VOID *)pISecB);
//    iResult = pthread_create(&(thread[i++]), NULL, (THREAD)thread_callback, (WE_VOID *)pISecS);
 //   iResult = pthread_create(&(thread[i++]), NULL, (THREAD)thread_Wcallfunction, (WE_VOID *)pISecW);
//    iResult = pthread_create(&(thread[i++]), NULL, (THREAD)thread_Bcallfunction, (WE_VOID *)pISecB);
//    iResult = pthread_create(&(thread[i++]), NULL, (THREAD)thread_Scallfunction, (WE_VOID *)pISecS);
//    thread_Wcallfunction(pISecW);
    thread_Bcallfunction(pISecB);
//    thread_Scallfunction(pISecS);
    max = i;
    for (i = 0; i < max; i++)
    {
        printf("pthread_join\n");
        pthread_join(thread[i], NULL);
        
    }
    
    //ISec_DeRegClientEventCB(pISecW, pISecW, NotifyWapCallback);    
    //ISec_Release(pISecW);
    //ISec_Release(pISecB);
    //ISec_Release(pISecS); 
    return G_SEC_OK;
}

/*************************************************************************************************
end
*************************************************************************************************/


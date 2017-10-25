/*=====================================================================================
    HEADER NAME : we_des.h
    MODULE NAME : WE

    GENERAL DESCRIPTION
        In this file,define the struct and function prototype. 

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification              Tracking
    Date            Author      Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-11-03       Tang         none      Init
  
=====================================================================================*/
/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef  WE_DES_H
#define  WE_DES_H


/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define BLOCKSIZE   (8)
#define KEYSIZE     (8)
#define ROUND_KEY_SIZE  (128)

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeBlockMAtchS *pstBlockMach;
typedef WE_VOID (*blockfn)(pstBlockMach m, WE_UINT32 len, WE_UINT8 *in, WE_UINT8 *out);
typedef WE_VOID (*dcfn)(WE_UINT32 *data, WE_VOID *pvRoundKey[]);


typedef enum tagE_WeBsapiModeOp
{
    BSAPI_ENCRYPT, 
    BSAPI_DECRYPT
} E_WeBsapiModeOp; 


typedef enum tagE_WeBsapiModeCBC
{
    BSAPI_ECB, 
    BSAPI_CBC
} E_WeBsapiModeCBC;

typedef enum tagE_WeDesOr3Des
{
    BSAPI_SINGLE,
    BSAPI_TRIPLE
} E_WeDesOr3Des;

typedef enum tagE_WeDesKeyType
{
    BSAPI_KEY, 
    BSAPI_SCHEDULE
} E_WeDesKeyType;

typedef struct tagSt_BsapiMode 
{
    E_WeBsapiModeOp     eOp;
    E_WeBsapiModeCBC    eMode;
    E_WeDesOr3Des       eTriple;
    E_WeDesKeyType      eKeyType;
}St_WeLibBsapiMode ;
/*
typedef enum tagE_WeKSetKeyType
{
    BSAPI_KEY_TYPE_DEFAULT, 
    BSAPI_SECRET, 
    BSAPI_PUBLIC
}E_WeKSetKeyType;*/

typedef struct tagSt_WeBlockMAtchS 
{
    WE_VOID         *pvRoundKey[3];
    WE_UINT8        aucData8[BLOCKSIZE];
    WE_UINT32       iv[3][BLOCKSIZE / 4];
    blockfn         fn;
    dcfn            corefn;
    E_WeBsapiModeOp eOp;
    WE_UINT32       iBufLen;
    WE_UINT32       uiType; 
}St_WeBlockMAtchS;


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_VOID We_LibBlock3
(
    dcfn core,  
    WE_UINT32 *puiData, 
    WE_VOID *pavKs[]
);

WE_VOID We_LibBlockCBC
(
    pstBlockMach pstM, 
    WE_UINT32 uiLen, 
    WE_UINT8 *pucIn, 
    WE_UINT8 *pucOut
);

WE_INT32 We_LibAppendBlock
(
    WE_UINT8 *pucData, 
    WE_UINT32 uiLength, 
    WE_UCHAR *pucOut,
    WE_VOID *handle
);

WE_INT32 We_LibAppendDes
(
    WE_UINT8 *pucData, 
    WE_UINT32 uiLength,
    WE_UCHAR *pucOut,
    WE_VOID *pHandle
);

WE_VOID We_LibKillDes(WE_HANDLE hDESHandle);

WE_INT32 We_LibStartTripleDes
(
    St_WeLibBsapiMode *pstMode,  
    St_WeCipherCrptKey stKey, 
    WE_HANDLE *pHandle
);


WE_INT32 We_LibAppendTripleDes
(
    WE_UCHAR *pucData, 
    WE_UINT32 uiLength,
    WE_UCHAR *pucOut, 
    WE_VOID *pHandle
);


/***************************************************************************************************
*   DES Encrypt                                                                    
***************************************************************************************************/
/* No Padding */
WE_INT32 We_LibDesEncrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibDesEncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibDesEnCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_LibDesEnCryptFinal
(
    WE_HANDLE handle
);

/* RFC2630 Padding */
WE_INT32 We_LibDesEncrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibDesEncryptInit1
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibDesEnCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_LibDesEnCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);


/***************************************************************************************************
*   DES Decrypt                                                                    
***************************************************************************************************/
/* No Padding */
WE_INT32 We_LibDesDecrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibDesDeCryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibDesDeCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_LibDesDeCryptFinal
(
    WE_HANDLE handle
);


/* RFC2630 Padding */
WE_INT32 We_LibDesDecrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_LibDesDecryptInit1
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_LibDesDeCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_LibDesDeCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);



/***************************************************************************************************
*   Tripartite DES Encrypt                                                                    
***************************************************************************************************/
/* No Padding */
WE_INT32 We_Lib3DesEncrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_Lib3DesEncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_Lib3DesEnCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_Lib3DesEnCryptFinal
(
    WE_HANDLE handle
); 



/* RFC2630 Padding */
WE_INT32 We_Lib3DesEncrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);

WE_INT32 We_Lib3DesEncryptInit1
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);



WE_INT32 We_Lib3DesEnCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_Lib3DesEnCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);


/***************************************************************************************************
*   Tripartite DES Decrypt                                                                    
***************************************************************************************************/
/* No Padding */
WE_INT32 We_Lib3DesDecrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);


WE_INT32 We_Lib3DesDecryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_Lib3DesDeCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_Lib3DesDeCryptFinal
(
    WE_HANDLE handle
); 



/* RFC2630 Padding */
WE_INT32 We_Lib3DesDecrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
);


WE_INT32 We_Lib3DesDeCryptInit1
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
);

WE_INT32 We_Lib3DesDeCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen,  
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_Lib3DesDeCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
);

WE_INT32 We_LibStartDesCore
(
 St_WeLibBsapiMode *pstMode,  
 St_WeCipherCrptKey *pstKey, 
 WE_HANDLE *pHandle
 );  

// WE_VOID We_LibFreeOrigKey(St_WeLibBsapiKSet stKey);


#endif /* endif WE_DES_H */


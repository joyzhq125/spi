/*==================================================================================================
    HEADER NAME : sec_lib.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be 
        update later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   steven ding        0.0.1         Draft
==================================================================================================*/

/*******************************************************************************
*   multi-Include-Prevent Section
*******************************************************************************/
#ifndef SEC_LIB_H
#define SEC_LIB_H


/*******************************************************************************
*   macro Define Section
*******************************************************************************/


/*******************************************************************************
*   Type Define Section
*******************************************************************************/


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
/*************************************************************************************************
cipher crypt Initialise:alloc resource
*************************************************************************************************/
WE_INT32 Sec_LibCryptInitialise(WE_HANDLE hSecHandle);

/*************************************************************************************************
cipher crypt terminate:release resource
*************************************************************************************************/
WE_INT32 Sec_LibCryptTerminate(WE_HANDLE hSecHandle);

/*************************************************************************************************
for short char_string encrypt,every time is new,must be n*blocksize,no padding ,padded by yourself
DES,3DES,AES_128
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/
WE_INT32 Sec_LibCipherEncrypt(WE_HANDLE hSecHandle, 
                              E_SecCipherAlgType eAlg, 
                              St_SecCipherCrptKey stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen);
/*************************************************************************************************
for short char_string encrypt,every time is new,not padded, padding with RFC2630
DES,3DES,AES_128
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/
WE_INT32 Sec_LibCipherEncryptRFC2630Padding(WE_HANDLE hSecHandle, 
                              E_SecCipherAlgType eAlg, 
                              St_SecCipherCrptKey stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen);
/*************************************************************************************************
for short char_string decrypt,every time is new,must be n*blocksize
DES,3DES,AES_128
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/                            
WE_INT32 Sec_LibCipherDecrypt(WE_HANDLE hSecHandle,  
                              E_SecCipherAlgType eAlg, 
                              St_SecCipherCrptKey stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen);
/*************************************************************************************************
for short char_string decrypt,every time is new
DES,3DES,AES_128
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/                            
WE_INT32 Sec_LibCipherDecryptRFC2630Padding(WE_HANDLE hSecHandle,  
                              E_SecCipherAlgType eAlg, 
                              St_SecCipherCrptKey stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen);
/*************************************************************************************************
for long char_string encrypt,must init resource before every new start encrypt
*************************************************************************************************/
WE_INT32 Sec_LibCipherEncryptInit(WE_HANDLE hSecHandle,  
                                  E_SecCipherAlgType eAlg, 
                                  St_SecCipherCrptKey stKey, 
                                  WE_HANDLE* pHandle);

/*************************************************************************************************
for long char_string encrypt,after inited,can use this function n times,
and every time the input char_string must be n*blocksize.
input *piOutLen is pucOub buf size,and return actually byte written in this buf
no padding ,padded by yourself
*************************************************************************************************/
WE_INT32 Sec_LibCipherEncryptUpdate(WE_HANDLE hSecHandle,
                                    const WE_UCHAR* pucIn, 
                                    WE_UINT32 uiInLen, 
                                    WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen,
                                    WE_HANDLE handle);

/*************************************************************************************************
for long char_string encrypt,when input char_string is finished,use this functing
to release resource.
*************************************************************************************************/                                    
WE_INT32 Sec_LibCipherEncryptFinal(WE_HANDLE hSecHandle, WE_HANDLE handle);

/*************************************************************************************************
for long char_string decrypt, must init resouce before every new decrypt
*************************************************************************************************/
WE_INT32 Sec_LibCipherDecryptInit(WE_HANDLE hSecHandle,  
                                  E_SecCipherAlgType eAlg, 
                                  St_SecCipherCrptKey stKey,
                                  WE_HANDLE* pHandle);

/*************************************************************************************************
for long char_string decrypt,after inited,can use this function n times,
and every time the input char_string must be n*blocksize.
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/                        
WE_INT32 Sec_LibCipherDecryptUpdate(WE_HANDLE hSecHandle,
                                    const WE_UCHAR* pucIn, 
                                    WE_UINT32 uiInLen, 
                                    WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen,
                                    WE_HANDLE handle);

/*************************************************************************************************
for long char_string encrypt,when input char_string is finished,use this functing
to release resource.
*************************************************************************************************/                                    
WE_INT32 Sec_LibCipherDecryptFinal(WE_HANDLE hSecHandle, WE_HANDLE handle);



/*************************************************************************************************
for long char_string encrypt,must init resource before every new start encrypt
*************************************************************************************************/
WE_INT32 Sec_LibCipherEncryptInitRFC2630Padding(WE_HANDLE hSecHandle,  
                                   E_SecCipherAlgType eAlg, 
                                   St_SecCipherCrptKey stKey, 
                                   WE_HANDLE* pHandle);

/*************************************************************************************************
for long char_string encrypt,after inited,can use this function n times,
and every time the input char_string can be any value.
input *piOutLen is pucOub buf size,and return actually byte written in this buf
padded with RFC2630
*************************************************************************************************/
WE_INT32 Sec_LibCipherEncryptUpdateRFC2630Padding(WE_HANDLE hSecHandle,
                                     const WE_UCHAR* pucIn, 
                                     WE_UINT32 uiInLen, 
                                     WE_UCHAR* pucOut, 
                                     WE_UINT32* puiOutLen,
                                     WE_HANDLE handle);

/*************************************************************************************************
for long char_string encrypt,when input char_string is finished,use this functing
to release resource, and get the last encrypted char_string
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/                                    
WE_INT32 Sec_LibCipherEncryptFinalRFC2630Padding(WE_HANDLE hSecHandle, WE_UCHAR*pucOut, 
                                    WE_UINT32* puiOutLen, WE_HANDLE handle);

/*************************************************************************************************
for long char_string decrypt,must init resource before every new start decrypt
*************************************************************************************************/
WE_INT32 Sec_LibCipherDecryptInitRFC2630Padding(WE_HANDLE hSecHandle,  
                                  E_SecCipherAlgType eAlg, 
                                  St_SecCipherCrptKey stKey,
                                  WE_HANDLE* pHandle);

/*************************************************************************************************
for long char_string encrypt,after inited,can use this function n times,
and every time the input char_string can be any value.
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/
WE_INT32 Sec_LibCipherDecryptUpdateRFC2630Padding(WE_HANDLE hSecHandle,
                                    const WE_UCHAR* pucIn, 
                                    WE_UINT32 uiInLen, 
                                    WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen,
                                    WE_HANDLE handle);

/*************************************************************************************************
for long char_string decrypt,when input char_string is finished,use this functing
to release resource, and get the last decrypted char_string
*piOutLen is the pucOub buf size
input *piOutLen is pucOub buf size,and return actually byte written in this buf
*************************************************************************************************/                                    
WE_INT32 Sec_LibCipherDecryptFinalRFC2630Padding(WE_HANDLE hSecHandle, WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen, WE_HANDLE handle);

/*************************************************************************************************
get one char_string hash value.
MD2,MD5,SHA1
input *piDigLen is pucDig buf size,and return actually byte written in this buf
*************************************************************************************************/
WE_INT32 Sec_LibHash(WE_HANDLE hSecHandle,
                     E_SecHashAlgType eAlg,
                     const WE_UCHAR* pucData, 
                     WE_INT32 iDataLen, 
                     WE_UCHAR* pucDig, 
                     WE_INT32* piDigLen);

/*************************************************************************************************
*************************************************************************************************/
WE_INT32 Sec_LibHashInit(WE_HANDLE hSecHandle , 
                         E_SecHashAlgType eAlg,
                         WE_HANDLE* pHandle);

/*************************************************************************************************
*************************************************************************************************/
WE_INT32 Sec_LibHashUpdate(WE_HANDLE hSecHandle, 
                           const WE_UCHAR* pucData, 
                           WE_INT32 iDataLen,
                           WE_HANDLE handle);

/*************************************************************************************************
input *piDigLen is pucDig buf size,and return actually byte written in this buf
*************************************************************************************************/
WE_INT32 Sec_LibHashFinal(WE_HANDLE hSecHandle, 
                          WE_UCHAR* pucDig, 
                          WE_INT32* piDigLen,
                          WE_HANDLE handle);

/*************************************************************************************************
hmac with hash
HMAC_MD2,HMAC_MD5,HMAC_SHA1
input *piDigLen is pucDig buf size,and return actually byte written in this buf
*************************************************************************************************/
WE_INT32 Sec_LibHmac(WE_HANDLE hSecHandle, 
                     E_SecHmacAlgType eAlg,
                     const WE_UCHAR* pucKey, 
                     WE_INT32 iKeyLen,
                     const WE_UCHAR* pucData, 
                     WE_INT32 iDataLen, 
                     WE_UCHAR* pucDig, 
                     WE_INT32* piDigLen);

/*************************************************************************************************
*************************************************************************************************/
WE_INT32 Sec_LibHmacInit(WE_HANDLE hSecHandle, 
                         E_SecHmacAlgType eAlg,
                         const WE_UCHAR* pucKey, 
                         WE_INT32 iKeyLen,
                         WE_HANDLE* pHandle);

/*************************************************************************************************
*************************************************************************************************/
WE_INT32 Sec_LibHmacUpdate(WE_HANDLE hSecHandle, 
                           const WE_UCHAR* pucData, 
                           WE_INT32 iDataLen,
                           WE_HANDLE handle);

/*************************************************************************************************
input *piDigLen is pucDig buf size,and return actually byte written in this buf
*************************************************************************************************/
WE_INT32 Sec_LibHmacFinal(WE_HANDLE hSecHandle, 
                          WE_UCHAR* pucDig, 
                          WE_INT32* piDigLen,
                          WE_HANDLE handle);

/*************************************************************************************************
return iRandomLen byte random char_string
*************************************************************************************************/
WE_INT32 Sec_LibGenerateRandom(WE_HANDLE hSecHandle, 
                               WE_CHAR* pcRandomData, 
                               WE_INT32 iRandomLen) ;


                            
#endif /* end of SEC_LIB_H */




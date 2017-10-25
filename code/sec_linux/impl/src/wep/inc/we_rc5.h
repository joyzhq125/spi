/*==================================================================================================
    HEADER NAME : we_rc5.h
    MODULE NAME : WE
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,declare the algorithm function prototype ,and related variables.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date              Author               Number        Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-12-06   bird zhang        0.0.1            Draft
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef _WE_RC5_H
#define _WE_RC5_H


/*************************************************************************************************
function declare
*************************************************************************************************/
WE_INT32 We_LibRC5Encrypt(WE_INT32 iRounds,
                                  St_WeCipherCrptKey stKey, 
                                  const WE_UCHAR* pucIn, 
                                  WE_UINT32 uiInLen, 
                                  WE_UCHAR* pucOut, 
                                  WE_UINT32* puiOutLen);
                           
WE_INT32 We_LibRC5Decrypt(WE_INT32 iRounds,
                                  St_WeCipherCrptKey stKey, 
                                  const WE_UCHAR*pucIn, 
                                  WE_UINT32 uiInLen, 
                                  WE_UCHAR* pucOut, 
                                  WE_UINT32* puiOutLen);

WE_INT32 We_LibRC5Encrypt1(WE_INT32 iRounds,
                                  St_WeCipherCrptKey stKey, 
                                  const WE_UCHAR* pucIn, 
                                  WE_UINT32 uiInLen, 
                                  WE_UCHAR* pucOut, 
                                  WE_UINT32* puiOutLen);

WE_INT32 We_LibRC5Decrypt1(WE_INT32 iRounds,
                                  St_WeCipherCrptKey stKey, 
                                  const WE_UCHAR*pucIn, 
                                  WE_UINT32 uiInLen, 
                                  WE_UCHAR* pucOut, 
                                  WE_UINT32* puiOutLen);

WE_INT32 We_LibRC5CryptInit(WE_INT32 iRounds,
                                      St_WeCipherCrptKey stKey,
                                      WE_HANDLE* pHandle);

WE_INT32 We_LibRC5EncryptInit(WE_INT32 iRounds,
                                      St_WeCipherCrptKey stKey,
                                      WE_HANDLE* pHandle);

WE_INT32 We_LibRC5EncryptUpdate(const WE_UCHAR* pucIn, 
                                        WE_UINT32 uiInLen, 
                                        WE_UCHAR* pucOut, 
                                        WE_UINT32* puiOutLen,
                                        WE_HANDLE handle);

WE_INT32 We_LibRC5EncryptFinal(WE_HANDLE handle); 

WE_INT32 We_LibRC5EncryptInit1(WE_INT32 iRounds,
                                       St_WeCipherCrptKey stKey,
                                       WE_HANDLE* pHandle);

WE_INT32 We_LibRC5EncryptUpdate1(const WE_UCHAR* pucIn, 
                                         WE_UINT32 uiInLen, 
                                         WE_UCHAR* pucOut, 
                                         WE_UINT32* puiOutLen,
                                         WE_HANDLE handle);

WE_INT32 We_LibRC5EncryptFinal1(WE_UCHAR* pucOut, 
                                        WE_UINT32* puiOutLen, WE_HANDLE handle); 

WE_INT32 We_LibRC5DecryptInit(WE_INT32 iRounds,
                                      St_WeCipherCrptKey stKey,
                                      WE_HANDLE* pHandle);

WE_INT32 We_LibRC5DecryptUpdate(const WE_UCHAR* pucIn, 
                                         WE_UINT32 uiInLen, 
                                         WE_UCHAR* pucOut, 
                                         WE_UINT32* puiOutLen,
                                         WE_HANDLE handle);

WE_INT32 We_LibRC5DecryptFinal(WE_HANDLE handle); 

WE_INT32 We_LibRC5DecryptInit1(WE_INT32 iRounds,
                                       St_WeCipherCrptKey stKey,
                                       WE_HANDLE* pHandle);

WE_INT32 We_LibRC5DecryptUpdate1(const WE_UCHAR* pucIn, 
                                         WE_UINT32 uiInLen, 
                                         WE_UCHAR* pucOut, 
                                         WE_UINT32* puiOutLen,
                                         WE_HANDLE handle);

WE_INT32 We_LibRC5DecryptFinal1(WE_UCHAR* pucOut, 
                                        WE_UINT32* puiOutLen, WE_HANDLE handle);     




                            
#endif /* endif _WE_RC5_H */

/*==================================================================================================
    HEADER NAME : sec_lib.c
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

/*==================================================================================================
*   Include File Section
==================================================================================================*/
#include "sec_comm.h"

/***************************************************************************************************
*   Macro Define Section
***************************************************************************************************/
typedef St_PublicData               St_SecGlobalData;
#define M_SEC_BREW_ISHELL_HANDLE    (((ISec*)hSecHandle)->m_pIShell)
#define M_SEC_LIB_HANDLE            (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->hFactory)


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*************************************************************************************************
*   cipher
*************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_LibCryptInitialise
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    crypt init,create a factory for cipher crypt
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    ERROR CODE:
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCryptInitialise(WE_HANDLE hSecHandle)
{
    WE_INT32            iRv       = M_SEC_ERR_OK;

    if (!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if(!M_SEC_BREW_ISHELL_HANDLE)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    iRv = WE_CRPT_INITIALISE(M_SEC_BREW_ISHELL_HANDLE, &M_SEC_LIB_HANDLE);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCryptTerminate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    crypt terminate for cipher crypt to release factory
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCryptTerminate(WE_HANDLE hSecHandle)
{
    WE_INT32  iRv       = M_SEC_ERR_OK;

    if (!hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_TERMINATE(M_SEC_LIB_HANDLE);
    M_SEC_LIB_HANDLE = NULL;
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncrypt
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle cipher encrypt
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    E_SecCipherAlgType      eAlg[IN]: cipher encrypt alg type
    St_SecCipherCrptKey     stKey[IN]:cipher encrypt key
    const WE_UCHAR*         pucIn[IN]:src data need to be encrypted
    WE_INT32                iInLen[IN]: src data lenth(any lenth)
    WE_UCHAR*               pucOut[OUT]:encrypted data
    WE_INT32*               piOutLen[IN/OUT]:accept encrypted data len[IN]
                                             actual encrypted data len[OUT](always multiple of 8)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncrypt(WE_HANDLE hSecHandle, 
                              E_SecCipherAlgType  eAlg, 
                              St_SecCipherCrptKey  stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;

    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_ENCRYPT_NO_PADDING(M_SEC_LIB_HANDLE, eAlg, stKey, pucIn, \
            uiInLen, pucOut, puiOutLen);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecrypt
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle cipher decrypt
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]: sec global data
    E_SecCipherAlgType      eAlg[IN]:cipher decrypt alg typ
    St_SecCipherCrptKey     stKey[IN]:cipher decrypt key
    const WE_UCHAR*         pucIn[IN]:src data need to be decrypted
    WE_INT32                iInLen[IN]:src data length
    WE_UCHAR*               pucOut[OUT]:decrypted data
    WE_INT32*               piOutLen[IN/OUT]:accept decryped data length[IN]
                                             actual decryped data length[OUT](always multiple of 8)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecrypt(WE_HANDLE    hSecHandle,  
                              E_SecCipherAlgType eAlg, 
                              St_SecCipherCrptKey  stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;

    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_DECRYPT_NO_PADDING(M_SEC_LIB_HANDLE, eAlg, stKey, pucIn, \
        uiInLen, pucOut, puiOutLen);
    
    return iRv;
}



/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncryptRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle cipher encrypt
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    E_SecCipherAlgType      eAlg[IN]: cipher encrypt alg type
    St_SecCipherCrptKey     stKey[IN]:cipher encrypt key
    const WE_UCHAR*         pucIn[IN]:src data need to be encrypted
    WE_INT32                iInLen[IN]: src data lenth(any lenth)
    WE_UCHAR*               pucOut[OUT]:encrypted data
    WE_INT32*               piOutLen[IN/OUT]:accept encrypted data len[IN]
                                             actual encrypted data len[OUT](always multiple of 8)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncryptRFC2630Padding(WE_HANDLE hSecHandle, 
                              E_SecCipherAlgType  eAlg, 
                              St_SecCipherCrptKey  stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;

    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_ENCRYPT_RFC2630_PADDING(M_SEC_LIB_HANDLE, eAlg, stKey, pucIn, \
        uiInLen, pucOut, puiOutLen);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecryptRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle cipher decrypt
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]: sec global data
    E_SecCipherAlgType      eAlg[IN]:cipher decrypt alg typ
    St_SecCipherCrptKey     stKey[IN]:cipher decrypt key
    const WE_UCHAR*         pucIn[IN]:src data need to be decrypted
    WE_INT32                iInLen[IN]:src data length
    WE_UCHAR*               pucOut[OUT]:decrypted data
    WE_INT32*               piOutLen[IN/OUT]:accept decryped data length[IN]
                                             actual decryped data length[OUT](always multiple of 8)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecryptRFC2630Padding(WE_HANDLE   hSecHandle,  
                              E_SecCipherAlgType eAlg, 
                              St_SecCipherCrptKey  stKey, 
                              const WE_UCHAR* pucIn, 
                              WE_UINT32 uiInLen, 
                              WE_UCHAR* pucOut, 
                              WE_UINT32* puiOutLen)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;

    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_DECRYPT_RFC2630_PADDING(M_SEC_LIB_HANDLE, eAlg, stKey, pucIn, \
        uiInLen, pucOut, puiOutLen);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncryptInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher encrypt init step
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]: sec global data 
    E_SecCipherAlgType      eAlg[IN]:cipher encrypt alg typ
    St_SecCipherCrptKey     stKey[IN]:cipher encrypt key
    WE_HANDLE*              pHandle[OUT]:handler
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncryptInit(WE_HANDLE hSecHandle,  
                                  E_SecCipherAlgType eAlg, 
                                  St_SecCipherCrptKey stKey,
                                  WE_HANDLE* pHandle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;

    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1) )
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_ENCRYPT_INIT_NO_PADDING(M_SEC_LIB_HANDLE, eAlg, stKey, pHandle);

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncryptUpdate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher encrypt update step
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    const WE_UCHAR*         pucIn[IN]:src data need to be encrypted
    WE_INT32                iInLen[IN]:src data length(always multiple of 8)
    WE_UCHAR*               pucOut[OUT]:encrypted data
    WE_INT32*               piOutLen[IN/OUT]:accept encrypted data length[IN]
                                             actual encrypted data length[OUT](always multiple of 8)
    WE_HANDLE               handle[IN]:handle (create by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncryptUpdate(WE_HANDLE hSecHandle,
                                    const WE_UCHAR* pucIn, 
                                    WE_UINT32 uiInLen, 
                                    WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen,
                                    WE_HANDLE handle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if (!hSecHandle || !pucOut || !puiOutLen || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    
    iRv = WE_CRPT_ENCRYPT_UPDATE_NO_PADDING(M_SEC_LIB_HANDLE, pucIn, uiInLen, \
        pucOut, puiOutLen, handle);
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncryptFinal
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher encrypt final step
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_HANDLE           handle[IN]: handle (create by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncryptFinal(WE_HANDLE hSecHandle, WE_HANDLE handle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if(!hSecHandle || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_ENCRYPT_FINAL_NO_PADDING(M_SEC_LIB_HANDLE, handle);
    
    return iRv;
}


/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecryptInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher decrypt init step
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    E_SecCipherAlgType      eAlg[IN]:cipher decrypt alg type
    St_SecCipherCrptKey     stKey[IN]:cipher decrypt key
    WE_HANDLE*              pHandle[OUT]:handle(create by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecryptInit(WE_HANDLE hSecHandle,  
                                  E_SecCipherAlgType eAlg, 
                                  St_SecCipherCrptKey stKey,
                                  WE_HANDLE* pHandle)
{
    WE_INT32 iRv = M_SEC_ERR_OK;

    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_DECRYPT_INIT_NO_PADDING(M_SEC_LIB_HANDLE, eAlg, stKey, pHandle);

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecryptUpdate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher decrypt update step
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    const WE_UCHAR*         pucIn[IN]:src data need to be decrypted
    WE_INT32                iInLen[IN]:src data length(always mutiple of 8)
    WE_UCHAR*               pucOut[OUT]:encrypted data
    WE_INT32*               piOutLen[IN/OUT]:accept encrypted data length[IN]
                                             actual encrypted data length[OUT](always mutiple of 8)
    WE_HANDLE               handle[IN]:handle(create by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecryptUpdate(WE_HANDLE hSecHandle,
                                    const WE_UCHAR* pucIn, 
                                    WE_UINT32 uiInLen, 
                                    WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen,
                                    WE_HANDLE handle)
{
    WE_INT32 iRv = M_SEC_ERR_OK;
    
    if (!hSecHandle || !pucOut || !puiOutLen || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_DECRYPT_UPDATE_NO_PADDING(M_SEC_LIB_HANDLE, pucIn, \
        uiInLen, pucOut, puiOutLen, handle);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecryptFinal
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher decrypt final step
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    WE_HANDLE               handle[IN]:handle (created by init)
RETURN VALUE:
    Counter number
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecryptFinal(WE_HANDLE hSecHandle, WE_HANDLE handle)
{
    WE_INT32 iRv = M_SEC_ERR_OK;
    
    if(!hSecHandle || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_DECRYPT_FINAL_NO_PADDING(M_SEC_LIB_HANDLE, handle);

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncryptInitRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher encrypt init step for any length input data
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]: sec global data
    E_SecCipherAlgType      eAlg[IN]:cipher encrypt alg type
    St_SecCipherCrptKey     stKey[IN]:cipher encrypt key
    WE_HANDLE*              pHandle[OUT]:handler
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncryptInitRFC2630Padding(WE_HANDLE hSecHandle,  
                                   E_SecCipherAlgType eAlg, 
                                   St_SecCipherCrptKey stKey, 
                                   WE_HANDLE* pHandle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1) )
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_ENCRYPT_INIT_RFC2630_PADDING(M_SEC_LIB_HANDLE, eAlg, \
        stKey, pHandle);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncryptUpdateRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher encrypt update step for any lenth input data
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    const WE_UCHAR*     pucIn[IN]:src data need to be encrypted
    WE_INT32            iInLen[IN]:src data length
    WE_UCHAR*           pucOut[OUT]:encrypted data
    WE_INT32*           piOutLen[IN/OUT]:accept encrypted data length[IN]
                                         actual encrypted data length[OUT](always mutliple of 8)
    WE_HANDLE           handle[IN]:handle (create by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncryptUpdateRFC2630Padding(WE_HANDLE hSecHandle,
                                     const WE_UCHAR* pucIn, 
                                     WE_UINT32 uiInLen, 
                                     WE_UCHAR* pucOut, 
                                     WE_UINT32* puiOutLen,
                                     WE_HANDLE handle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if(!hSecHandle || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_ENCRYPT_UPDATE_RFC2630_PADDING(M_SEC_LIB_HANDLE, pucIn, \
        uiInLen, pucOut, puiOutLen, handle);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherEncryptFinalRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher encrypt final step for any lenth input data
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_UCHAR*           pucOut[OUT]:last encrypted data
    WE_INT32*           piOutLen[IN/OUT]:accepted last encrypted data length[IN]
                                         actual last encrypted data length[OUT](always mutliple of 8)
    WE_HANDLE           handle[IN]:
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherEncryptFinalRFC2630Padding(WE_HANDLE hSecHandle, WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen, WE_HANDLE handle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if (!hSecHandle || !pucOut || !puiOutLen || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_ENCRYPT_FINAL_RFC2630_PADDING(M_SEC_LIB_HANDLE, pucOut,\
        puiOutLen, handle);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecryptInitRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher decrypt init step for any length input data
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data  
    E_SecCipherAlgType      eAlg[IN]:cipher decrypt alg type
    St_SecCipherCrptKey     stKey[IN]:cipher decrypt key
    WE_HANDLE*              pHandle[OUT]:decrypt handler
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecryptInitRFC2630Padding(WE_HANDLE hSecHandle,  
                                   E_SecCipherAlgType eAlg, 
                                   St_SecCipherCrptKey stKey,
                                   WE_HANDLE* pHandle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if(!hSecHandle || !stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 1) || !pHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_DECRYPT_INIT_RFC2630_PADDING(M_SEC_LIB_HANDLE, eAlg, \
        stKey, pHandle);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecryptUpdateRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher decrypt update step for any lenth input data
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    const WE_UCHAR*         pucIn[IN]:src data need to be decrypted
    WE_INT32                iInLen[IN]:src data len
    WE_UCHAR*               pucOut[OUT]:decrypted data
    WE_INT32*               piOutLen[IN/OUT]:accepte decrypted data length[IN]
                                             actual decrypted data length[OUT](always multiple of 8)
    WE_HANDLE               handle[IN]:decrypt handler(created by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecryptUpdateRFC2630Padding(WE_HANDLE hSecHandle,
                                     const WE_UCHAR* pucIn, 
                                     WE_UINT32 uiInLen, 
                                     WE_UCHAR* pucOut, 
                                     WE_UINT32* puiOutLen,
                                     WE_HANDLE handle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if(!hSecHandle || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_DECRYPT_UPDATE_RFC2630_PADDING(M_SEC_LIB_HANDLE, pucIn,\
            uiInLen, pucOut, puiOutLen, handle);

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibCipherDecryptFinalRFC2630Padding
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    cipher decrypt final step for any lenth input data
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_UCHAR*           pucOut[OUT]:last decryped data
    WE_INT32*           piOutLen[IN/OUT]:accepted decryped data length[IN]
                                         actual decryped data length[OUT](always 8)
    WE_HANDLE           handle[IN]:
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibCipherDecryptFinalRFC2630Padding(WE_HANDLE hSecHandle,WE_UCHAR* pucOut, 
                                    WE_UINT32* puiOutLen,WE_HANDLE handle)
{
    WE_INT32 iRv    = M_SEC_ERR_OK;
    
    if (!hSecHandle || !pucOut || !puiOutLen || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_DECRYPT_FINAL_RFC2630_PADDING(M_SEC_LIB_HANDLE, \
        pucOut, puiOutLen, handle);
    
    return iRv;
}


/*************************************************************************************************
*   Hash
*************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_LibHash
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle hash function
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    E_SecHashAlgType        eAlg[IN]:hash alg type
    const WE_UCHAR*         pucData[IN]:src data need to be hashed
    WE_INT32                iDataLen[IN]:src data
    WE_UCHAR*               pucDig[OUT]:hashed data
    WE_INT32*               piDigLen[IN/OUT]:accept hashed data length[IN]
                                             actual hashed data length[OUT]
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHash(WE_HANDLE hSecHandle,
                     E_SecHashAlgType eAlg,
                     const WE_UCHAR* pucData, 
                     WE_INT32 iDataLen, 
                     WE_UCHAR* pucDig, 
                     WE_INT32* piDigLen)
{
    WE_INT32  iRv   = M_SEC_ERR_OK;

    if(!hSecHandle || !pucData || !pucDig || !piDigLen ||\
        (iDataLen <= 0) || (*piDigLen <= 0))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_HASH(M_SEC_LIB_HANDLE, eAlg, pucData, iDataLen, \
        pucDig, piDigLen);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibHashInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    hash init
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    E_SecHashAlgType        eAlg[IN]:hash alg type
    WE_HANDLE*              pHandle[OUT]: hash handler
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHashInit(WE_HANDLE hSecHandle ,
                         E_SecHashAlgType eAlg, WE_HANDLE* pHandle)
{
    WE_INT32  iRv   = M_SEC_ERR_OK;

    if (!hSecHandle || !pHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iRv = WE_CRPT_HASH_INIT(M_SEC_LIB_HANDLE, eAlg, pHandle);
  
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibHashUpdate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    hash updtate
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[in/out]: sec global data
    const WE_UCHAR*         pucData[in]:src data need to hashed
    WE_INT32                iDataLen[in]:src data length
    WE_HANDLE               handle[in]:hash handler(created by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHashUpdate(WE_HANDLE hSecHandle, 
                           const WE_UCHAR* pucData, 
                           WE_INT32 iDataLen,
                           WE_HANDLE handle)
{
    WE_INT32  iRv   = M_SEC_ERR_OK;
    
    if (!hSecHandle || !pucData || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_HASH_UPDATE(M_SEC_LIB_HANDLE, pucData, iDataLen, handle);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibHashFinal
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    hash final
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[in/out]:sec global data
    WE_UCHAR*           pucDig[out]:hashed data
    WE_INT32*           piDigLen[in/out]:accept hashed data[IN]
                                         actual hashed data[OUT]
    WE_HANDLE           handle[in]:
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHashFinal(WE_HANDLE hSecHandle, 
                          WE_UCHAR* pucDig, 
                          WE_INT32* piDigLen,
                          WE_HANDLE handle)
{ 
    WE_INT32 iRv = M_SEC_ERR_OK;

    if (!hSecHandle || !piDigLen || !pucDig || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_HASH_FINAL(M_SEC_LIB_HANDLE, pucDig, piDigLen, handle);
    
    return iRv;
}

/*************************************************************************************************
*   hmac
*************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_LibHmac
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    handle hmac function
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    E_SecHmacAlgType        eAlg[IN]:hmac alg type
    const WE_UCHAR*         pucKey[IN]:hmac key
    WE_INT32                iKeyLen[IN]:hmac key length
    const WE_UCHAR*         pucData[IN]:src data need to be hmaced
    WE_INT32                iDataLen[IN]:src data length
    WE_UCHAR*               pucDig[OUT]:hmaced data
    WE_INT32*               piDigLen[IN/OUT]:accept hmaced data length[IN]
                                             actual hmaced data length[OUT]
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHmac(WE_HANDLE hSecHandle, 
                     E_SecHmacAlgType eAlg,
                     const WE_UCHAR* pucKey, 
                     WE_INT32 iKeyLen,
                     const WE_UCHAR* pucData, 
                     WE_INT32 iDataLen, 
                     WE_UCHAR* pucDig, 
                     WE_INT32* piDigLen)
{
    WE_INT32  iRv   = M_SEC_ERR_OK;

    if(!hSecHandle || !pucKey || !pucData || !pucDig || !piDigLen || \
        (iDataLen <= 0) || (iKeyLen <= 0) || (*piDigLen <= 0))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_HMAC(M_SEC_LIB_HANDLE, eAlg, pucKey, iKeyLen, \
        pucData, iDataLen, pucDig, piDigLen);

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibHmacInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    hmac init
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]: sec global data
    E_SecHmacAlgType        eAlg[IN]:hmac alg type
    const WE_UCHAR*         pucKey[IN]:hmac key
    WE_INT32                iKeyLen[IN]:hmac key len
    WE_HANDLE*              pHandle[OUT]:hmac handler
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHmacInit(WE_HANDLE hSecHandle, 
                         E_SecHmacAlgType eAlg,
                         const WE_UCHAR* pucKey, 
                         WE_INT32 iKeyLen,
                         WE_HANDLE* pHandle)
{
    WE_INT32  iRv   = M_SEC_ERR_OK;

    if (!hSecHandle || !pHandle || !pucKey)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_HMAC_INIT(M_SEC_LIB_HANDLE, eAlg, pucKey, iKeyLen, pHandle);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibHmacUpdate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    hmac update
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]:sec global data
    const WE_UCHAR*         pucData[IN]:src data need to hmaced
    WE_INT32                iDataLen[IN]:src data length
    WE_HANDLE               handle[IN]:hmac handler(created by init)
RETURN VALUE:
    Counter number
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHmacUpdate(WE_HANDLE hSecHandle, 
                           const WE_UCHAR* pucData, 
                           WE_INT32 iDataLen,
                           WE_HANDLE handle)
{
    WE_INT32  iRv   = M_SEC_ERR_OK;

    if (!hSecHandle || !pucData || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

   iRv = WE_CRPT_HMAC_UPDATE(M_SEC_LIB_HANDLE, pucData, iDataLen, handle);

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    Sec_LibHmacFinal
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    hmac final
ARGUMENTS PASSED:
    WE_HANDLE           hSecHandle[IN/OUT]:sec global data
    WE_UCHAR*           pucDig[OUT]:hmaced data
    WE_INT32*           piDigLen[IN/OUT]:hmaced data len
    WE_HANDLE           handle[IN]:hmac handler(created by init)
RETURN VALUE:
    ERROR CODE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibHmacFinal(WE_HANDLE hSecHandle, 
                          WE_UCHAR* pucDig, 
                          WE_INT32* piDigLen,
                          WE_HANDLE handle)
{
    WE_INT32  iRv   = M_SEC_ERR_OK;

    if (!hSecHandle || !pucDig || !piDigLen || !handle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_HMAC_FINAL(M_SEC_LIB_HANDLE, pucDig, piDigLen, handle);
    
    return iRv;
}

/*************************************************************************************************
*   random
*************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_LibGenerateRandom
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    generate any lenth random str-buf
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_UCHAR*       pucRandomData[OUT]:random buf 
    WE_INT32        iRandomLen[IN]:random length
RETURN VALUE:
    Counter number
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_LibGenerateRandom(WE_HANDLE hSecHandle, 
                               WE_CHAR*  pcRandomData, 
                               WE_INT32 iRandomLen) 
{
    WE_INT32  iRv   = M_SEC_ERR_OK;

    if (!pcRandomData || !hSecHandle || (iRandomLen <= 0))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    iRv = WE_CRPT_GENERATE_RANDOM(M_SEC_LIB_HANDLE, pcRandomData, iRandomLen);
    
    return iRv;
}



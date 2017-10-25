/*==================================================================================================
    HEADER NAME : we_rsa.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be update later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-11-27   Sam            None         Draft
==================================================================================================*/
/***************************************************************************************
    *   Include File Section
 ****************************************************************************************/
#include "sec_comm.h"
//#include "we_rsacomm.h"


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_RSAMachs *P_St_RSAMachs;

typedef WE_INT32 (*piRsaFn)(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut);

typedef struct tagSt_RsaMachStateConf 
{ 
    WE_UINT8              *pucPending;    
    WE_UINT32             uiNPend;      
    piRsaFn                 pfnRsaFn;         
    P_St_UtMblkLists      pstBuf;        
} st_RsaMachStateConf;

typedef struct tagSt_RsaMachsig
{ 
    St_SigType              stSigType;  
    WE_UINT8                *pucBuf;   
    E_KsetDataType          eDateType;  
    WE_UINT32               uiBufLen;   
} st_RsaMachSig;

typedef union tagSt_RsaMachState
{
    st_RsaMachStateConf    stConf; 
    st_RsaMachSig          stMachSig; 
} St_RsaMachState;


struct tagSt_RSAMachs 
{
    E_WeRsaModeOp         eModOp;       
    St_PadAlg             stPadAlg;     
    St_PadParams          stPadParam;     
    WE_UINT32             uiPadSize;  
    WE_UINT32             uiIbs;      
    WE_UINT32             uiPbs;      
    WE_UINT32             uiObs;      
    WE_UINT32             uiFbs;      
    St_RsaKey             *pstRSAKey;      
    WE_UINT32             uiKeybits;  
    WE_VOID               *pvModulus;  
    WE_INT32              iForce;    
    St_RsaMachState       stRSAMachState;    
};

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/ 
static WE_INT32 We_RsaEncrypt(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut);
static WE_INT32 We_RsaDecrypt(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut);
static WE_INT32 We_RsaEncryptPad(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut);
static WE_INT32 We_RsaDecryptPad(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut);
static WE_INT32 We_RsaDecryptAutoUnPad(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut);
    
static WE_INT32 We_RsaEncryptFinish(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach,WE_VOID * pvHandle,P_St_UtMblkLists *pstData);
static WE_INT32 We_RsaDecryptFinish(P_St_RSAMachs pstMach,P_St_UtMblkLists *pstData);
static WE_INT32 We_RsaSignFinish(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach,P_St_UtMblkLists *pstData);
static WE_INT32 We_RsaVerifySigFinish(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach,P_St_UtMblkLists *pstData);   
    
static WE_VOID *We_RsaCryptCrt(WE_VOID *pvMid, St_RsaKey *pstRSAKey, WE_VOID * pvMessArc);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    We_RsaEncrypt
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    encrypt data
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN]:mach
    WE_UINT32 uiLen[IN]:length of pucIn.
    WE_UINT8 *pucIn[OUT]:the data putin.
    WE_UINT8 *pucOut[OUT]:the data output.
RETURN VALUE:
    error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaEncrypt(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, 
                              WE_UINT8 *pucIn, WE_UINT8 *pucOut)
{
    WE_UINT32       uiIbs=0;
    WE_UINT32       uiObs=0;
    WE_INT32        iRes=0;
    
    if(!pstMach||!pucIn||!pucOut)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    hWeHandle=hWeHandle;
    uiIbs = pstMach->uiPbs; 
    uiObs = pstMach->uiObs;
    
    while(uiLen > 0) 
    {
        WE_VOID     *pvPt=NULL;
        WE_VOID     *pvCt=NULL;
        E_MpmSign   eSign=E_MPM_NEG;
        WE_UINT16   usPrec=0;
        WE_UINT16   usOprec=0;
        WE_UINT32   uiTmpLen=0;
        
        if(uiLen < uiIbs)
        {
            uiTmpLen=uiLen;
        }
        else
        {
            uiTmpLen=uiIbs;
        }
        pvPt=We_MpmBloadArc(E_MPM_POS, (WE_UINT16)uiTmpLen,(WE_UINT8 *) pucIn);
        if(pvPt==NULL)
        {
            /*couldn't load plaintext for encryption*/
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        
        uiLen -= uiIbs;
        pucIn += uiIbs;
        pvCt = We_RsaCrypt(pstMach->pstRSAKey, pvPt);
        We_MpmKillArc(pvPt);
        if(!pvCt) 
        {
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }    
        
        iRes=We_MpmSizeOfArc(pvCt,&usPrec);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        usOprec = usPrec;
        while(usPrec++ < uiObs)
        {
            *pucOut++ = 0;
        }
        (WE_VOID)We_MpmBStoreArc(&eSign, &usPrec, (WE_UINT8 *) pucOut, (WE_UINT16) usOprec, pvCt);
        We_MpmKillArc(pvCt);
        if(!usPrec) 
        {
            /*couldn't store ciphertext output*/
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        pucOut += usPrec;
    }    
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_RsaDecrypt
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    decrypt data.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN]:mach
    WE_UINT32 uiLen[IN]:lenght of  pucin
    WE_UINT8 *pucIn[OUT]:the input data  
    WE_UINT8 *pucOut[OUT]:the output data
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaDecrypt(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, 
                              WE_UINT8 *pucIn, WE_UINT8 *pucOut)
{
    WE_UINT32       uiIbs=0;
    WE_UINT32       uiObs=0;
    WE_INT32        iRes=0;
    WE_VOID         *pvMod=NULL;
    if(!pstMach||!pucIn||!pucOut)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    hWeHandle=hWeHandle;
    uiIbs = pstMach->uiIbs;
    uiObs = pstMach->uiPbs;     
    pvMod = pstMach->pstRSAKey->eKeyTypeTag == E_RSA_KTAG_SIMPLE_KEY ?
        pstMach->pstRSAKey->stKeyTypeUnion.stSimpleKey.pvModulus : pstMach->pstRSAKey->stKeyTypeUnion.stCrtKey.pvModulus;
   
    while(uiLen) 
    {
        WE_VOID * pvPt=NULL;       
        WE_VOID *  pvCt=NULL;       
        E_MpmSign eSign=E_MPM_NEG;
        WE_UINT16 usPrec=0;
        WE_UINT16 usOprec=0;
        
        pvCt = We_MpmBloadArc(E_MPM_POS, (WE_UINT16) uiIbs, (WE_UINT8 *) pucIn);
        if(!pvCt) 
        {
            /*couldn't load ciphertext for decryption*/
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        iRes=We_MpmCmpArc(pvCt, pvMod);
        if(iRes != -1)
        {
            /*ciphertext incompatible with key*/
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        uiLen -= uiIbs;
        pucIn += uiIbs;
        pvPt = We_RsaCrypt(pstMach->pstRSAKey, pvCt);
        We_MpmKillArc(pvCt);
        if(!pvPt) 
        {
            /*RSA decrypt failed*/
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        iRes=We_MpmSizeOfArc(pvPt,&usPrec);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return M_WE_LIB_ERR_GENERAL_ERROR;
        } 
        usOprec = usPrec;
        if(usPrec > uiObs) 
        {
            /*ciphertext is incompatible with key*/
            We_MpmKillArc(pvPt);
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        while(usPrec++ < uiObs)
        {
            *pucOut++ = 0;
        }
        (WE_VOID)We_MpmBStoreArc(&eSign, &usPrec, (WE_UINT8 *) pucOut, usOprec, pvPt);
        We_MpmKillArc(pvPt);
        pucOut += usPrec;
    }
    return M_WE_LIB_ERR_OK;
    
}
/*==================================================================================================
FUNCTION: 
    We_RsaEncryptPad
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
   encrypt data with pad number.  
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN]:
    WE_UINT32 uiLen[IN]:
    WE_UINT8 *pucIn[OUT]:
    WE_UINT8 *pucOut[OUT]:
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaEncryptPad(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut)
{
    WE_UINT8     *pucPadded=NULL;    
    WE_UINT32    uiPadLen=0;    
    WE_UINT8     *pucPadTmp=NULL;    
    WE_UINT8     *pucPadEnd=NULL;    
    WE_UINT32    uiIbs=0;       
    WE_UINT32    uiPbs=0;       
    WE_UINT32    uiUbs=0;   
    WE_UINT8     *pucInend=NULL;     
    WE_INT32     iRes = 0; 
    
    if(!pstMach||!pucIn||!pucOut)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    uiIbs = pstMach->uiIbs;
    uiPbs = pstMach->uiPbs;
    uiPadLen = (((uiLen-1)/uiIbs)+1) * uiPbs;
    pucPadded = (WE_UINT8 *)WE_MALLOC(uiPadLen);
    if(!pucPadded)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }  
    (WE_VOID)WE_MEMSET(pucPadded,0,uiPadLen);
    pucPadTmp = pucPadded;
    pucPadEnd = pucPadded + uiPadLen;
    pucInend = pucIn + uiLen;
    
    while (pucPadTmp < pucPadEnd)
    {
        if((pucIn + uiIbs)<=pucInend)
        {
            uiUbs=uiIbs;
        }
        else
        {
            uiUbs= (WE_UINT32)(pucInend-pucIn);
        }
        (WE_VOID)WE_MEMCPY (pucPadTmp, pucIn, uiUbs);

        if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)
        {
            iRes=We_MbkPadPkcs1Pad(hWeHandle,pucPadTmp, uiUbs, uiPbs, &pstMach->stPadParam);
        }
        else if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)
        {
            iRes=We_MbkPadPkcs1NullPad(hWeHandle,pucPadTmp, uiUbs, uiPbs, &pstMach->stPadParam);
        }
        else
        {
            WE_FREE(pucPadded);
            pucPadded=NULL;
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        if (iRes) 
        {
            /*padding failed*/
            WE_FREE(pucPadded);
            pucPadded=NULL;
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        pucIn += uiIbs;
        pucPadTmp += uiPbs;
    }  
    
    iRes = We_RsaEncrypt(hWeHandle,pstMach, uiPadLen, pucPadded, pucOut);
    
    //return_fail:
    WE_FREE(pucPadded);
    pucPadded=NULL;
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_RsaDecryptPad
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    decrypt data with pad data.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN]:
    WE_UINT32 uiLen[IN]:
    WE_UINT8 *pucIn[OUT]:
    WE_UINT8 *pucOut[OUT]:
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaDecryptPad(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut)
{
    WE_UINT8      *pucPadded=NULL;     
    WE_UINT32     uiPadLen = 0;     
    WE_UINT8      *pucPadTmp=NULL;     
    WE_UINT8      *pucPadEnd=NULL;     
    WE_UINT8      *pucPadBlk=NULL;    
    WE_UINT32     uiObs = 0;        
    WE_UINT32     uiUbs = 0;    
    WE_UINT32     uiPbs = 0;        
    // modify by Sam 061128
    WE_UINT32     uiFbs = 0;
    WE_INT32      iRes=M_WE_LIB_ERR_OK;
    
    if(!pstMach||!pucIn||!pucOut)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    
    uiObs = pstMach->uiObs;
    uiPbs = pstMach->uiPbs;
    
    if (uiLen % pstMach->uiIbs) 
    {
        /*ciphertext is incompatible with key*/
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    uiPadLen = (uiLen/pstMach->uiIbs)*uiPbs;
    pucPadded = (WE_UINT8 *)WE_MALLOC(uiPadLen);
    if(!pucPadded)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMSET(pucPadded,0,uiPadLen);
    pucPadBlk = (WE_UINT8 *)WE_MALLOC(uiPbs);
    if(!pucPadBlk)
    {
        WE_FREE(pucPadded);
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }   
    (WE_VOID)WE_MEMSET(pucPadBlk,0,uiPbs);    
    pucPadTmp = pucPadded;
    pucPadEnd = pucPadded + uiPadLen;    
    
    iRes=We_RsaDecrypt( hWeHandle,pstMach, uiLen, pucIn, pucPadded);
    if (iRes!=M_WE_LIB_ERR_OK)
    {
        /*decryption failure*/
        WE_FREE(pucPadBlk);
        pucPadBlk=NULL;
        WE_FREE(pucPadded);
        pucPadded=NULL;
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }  
    while (pucPadTmp < pucPadEnd)
    {
        (WE_VOID)WE_MEMCPY(pucPadBlk, pucPadTmp, uiPbs);
        if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)
        {
            iRes=We_MbkPadPkcs1Unpad(pucPadBlk, &uiUbs, uiPbs, &pstMach->stPadParam);
        }
        else if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)
        {
            iRes=We_MbkPadPkcs1Unpad(pucPadBlk, &uiUbs, uiPbs, &pstMach->stPadParam);
        }
        else
        {
            WE_FREE(pucPadBlk);
            pucPadBlk=NULL;
            WE_FREE(pucPadded);
            pucPadded=NULL;
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            WE_FREE(pucPadBlk);
            pucPadBlk=NULL;
            WE_FREE(pucPadded);
            pucPadded=NULL;
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        if (uiUbs > uiObs)
        {
            /*unpadded block is wrong length*/
            WE_FREE(pucPadBlk);
            pucPadBlk=NULL;
            WE_FREE(pucPadded);
            pucPadded=NULL;
            //return iRval;
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        (WE_VOID)WE_MEMCPY(pucOut, pucPadBlk, uiUbs);
        pucOut += uiUbs;
        pucPadTmp += uiPbs;
        /* add, to get the unpadlen, so uiFbs is the original data length.*/
        uiFbs += uiUbs;
    }
    /*
    iRval = 0;
    pstMach->uiFbs = uiUbs;*/
    pstMach->uiFbs = uiFbs;  

    WE_FREE(pucPadBlk);
    pucPadBlk=NULL;
    WE_FREE(pucPadded);
    pucPadded=NULL;
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_RsaDecryptAutoUnPad
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    decrypt data with auto unpad data.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN]:mach
    WE_UINT32 uiLen[IN]:length of pucIn.
    WE_UINT8 *pucIn[OUT]:the input data.
    WE_UINT8 *pucOut[OUT]:the output data.
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaDecryptAutoUnPad(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach, WE_UINT32 uiLen, WE_UINT8 *pucIn, WE_UINT8 *pucOut)
{
    WE_UINT8      *pucPadded=NULL;     
    WE_UINT32     uiPadLen=0;     
    WE_UINT8      *pucPadTmp=NULL;     
    WE_UINT8      *pucPadEnd=NULL;     
    WE_UINT8      *pucUPadTmp=NULL;    
    WE_UINT8      *pucUnPadded=NULL;                 
    WE_UINT32     uiUbs=0;       
    WE_UINT32     uiPbs=0;   
    WE_INT32      iRes=0; 
    if(!pstMach||!pucIn||!pucOut)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }   

    uiPbs = pstMach->uiPbs;
    
    if (uiLen % pstMach->uiIbs) 
    {
        /*ciphertext is incompatible with key*/
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    uiPadLen = (uiLen/pstMach->uiIbs)*uiPbs;
    pucPadded = WE_MALLOC(uiPadLen);
    if(!pucPadded)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }   
    (WE_VOID)WE_MEMSET(pucPadded,0,uiPadLen);
    pucUnPadded = WE_MALLOC(uiPadLen);
    if(!pucUnPadded)
    {
        WE_FREE(pucPadded);
        pucPadded=NULL;
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMSET(pucUnPadded,0,uiPadLen);
    pucPadTmp = pucPadded;
    pucUPadTmp = pucUnPadded;
    pucPadEnd = pucPadded + uiPadLen;
    
    iRes=We_RsaDecrypt( hWeHandle,pstMach, uiLen, pucIn, pucPadded);
    if (iRes!=M_WE_LIB_ERR_OK)
    {
        /*decryption failure*/
        WE_FREE(pucUnPadded);
        pucUnPadded=NULL;
        WE_FREE(pucPadded);
        pucPadded=NULL;
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    
    //iRval = 0;
    
    while (pucPadTmp < pucPadEnd)
    {
        (WE_VOID)WE_MEMCPY(pucUPadTmp, pucPadTmp, uiPbs);
        if(!(We_MbkPadPkcs1Unpad(pucUPadTmp, &uiUbs, uiPbs, &pstMach->stPadParam)))
        { 
            pucUPadTmp += uiUbs;
            pucPadTmp += uiPbs;  
        }
    }
    
    pstMach->stPadAlg.ePadMethod=E_RSA_PAD_PKCS1;
    pstMach->stPadAlg.iNullByte=1;

    pstMach->uiObs = pstMach->uiFbs =  pstMach->uiPbs;
    pstMach->stRSAMachState.stConf.pfnRsaFn = We_RsaDecryptPad;
    
    pstMach->stRSAMachState.stConf.pstBuf->stBlock.uiSize -= uiPadLen;
    pstMach->stRSAMachState.stConf.pstBuf->stBlock.uiSize += (WE_UINT32)(pucUPadTmp - pucUnPadded);
    (WE_VOID)WE_MEMCPY(pucOut, pucUnPadded, (WE_UINT32)(pucUPadTmp-pucUnPadded));

    WE_FREE(pucUnPadded);
    pucUnPadded=NULL;
    WE_FREE(pucPadded);
    pucPadded=NULL;
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_RsaUnpackKey
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    unpack key.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN/OUT]:mach
    St_UtMblk *pstKey[IN]:key
    E_PadKeyType eKeyType[OUT]:the type of key.
    E_WeRsaModeOp eModOp[OUT]:mode.
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaUnpackKey (P_St_RSAMachs pstMach, St_UtMblk *pstKey, E_PadKeyType eKeyType, E_WeRsaModeOp eModOp)
{
    WE_VOID * pvResult=0;

    if(!pstKey||!pstMach)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    eModOp=eModOp;
    eKeyType=eKeyType;    

    pvResult = We_MbkRsaBerkey2key(pstKey->pucData, &(pstMach->pstRSAKey));
    if(!pvResult)
    {
        /*ASN.1 decoding problem*/
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    
    pstMach->uiIbs = (WE_UINT32) ((pstMach->pstRSAKey->uiBits + 7)>> 3);
    pstMach->uiKeybits = pstMach->pstRSAKey->uiBits;
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_RsaStart
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    rsa start to encryp ,decrypt,signature or verify signature.
ARGUMENTS PASSED:
    St_BsapiMode *pstMode[IN]:mode
    St_RsaKeySet  *pstKeyRefPar[IN]:key parameter.
    WE_HANDLE *pHandle[OUT]:the handle of mach.
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32  We_RsaStart(WE_HANDLE hWeHandle,st_RsaMode *pstMode,  St_RsaKeySet  *pstKeyRefPar,WE_HANDLE *pHandle)
{
    P_St_RSAMachs       pstNewMach=NULL;
    WE_UINT32           uiIbs=0;
    WE_UINT32           uiObs=0;
    WE_INT32            iResult=0;
    
    if(!pHandle||!pstKeyRefPar||!(pstKeyRefPar->pstKey)||!(pstKeyRefPar->pstKey->pucData)||!pstMode)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    hWeHandle=hWeHandle;
    pstNewMach=WE_MALLOC(sizeof(*pstNewMach));
    if(pstNewMach==NULL)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    else
    {
        (WE_VOID)WE_MEMSET(pstNewMach, 0, sizeof(*(pstNewMach)));
    }
    
    pstNewMach->eModOp = pstMode->eOp;
    
    if (pstKeyRefPar->pstPadParam) 
    {
        (WE_VOID)WE_MEMCPY (&(pstNewMach->stPadParam), pstKeyRefPar->pstPadParam, sizeof (St_PadParams));
    }
    
    switch(pstNewMach->eModOp)
    {
    case E_WE_RSA_SIGN:
        pstNewMach->stPadParam.eKeyType = E_PAD_PRIVATE;
        break;
    case E_WE_RSA_VERIFY:
        pstNewMach->stPadParam.eKeyType = E_PAD_PUBLIC;
        break;
    case E_WE_RSA_ENCRYPT:
        if (!(pstKeyRefPar->pstPadParam)) 
        {
            if(pstKeyRefPar->eKeyType == RSA_KEY_TYPE_PRIVATE)
            {
                pstNewMach->stPadParam.eKeyType =E_PAD_PRIVATE;
            }
            else
            {
                pstNewMach->stPadParam.eKeyType =E_PAD_PUBLIC;
            }
            break;
        }
        if(pstKeyRefPar->pstPadParam->eKeyType == E_PAD_PRIVATE)
        {
            pstNewMach->stPadParam.eKeyType =E_PAD_PRIVATE;
        }
        else
        {
            pstNewMach->stPadParam.eKeyType =E_PAD_PUBLIC;            
        }
        break;
    case E_WE_RSA_DECRYPT:
        if (!(pstKeyRefPar->pstPadParam)) 
        {
            pstNewMach->stPadParam.eKeyType = \
                pstKeyRefPar->eKeyType == E_RSA_KEY_TYPE_PUBLIC ?E_PAD_PUBLIC: E_PAD_PRIVATE;
            break;
        }
        pstNewMach->stPadParam.eKeyType = \
            (pstKeyRefPar->pstPadParam->eKeyType == E_PAD_PUBLIC ?E_PAD_PUBLIC: E_PAD_PRIVATE);
        break;
    default:
        break ;
    }   
    
    iResult=We_RsaUnpackKey(pstNewMach, pstKeyRefPar->pstKey, pstNewMach->stPadParam.eKeyType, pstMode->eOp);
    if(iResult!=M_WE_LIB_ERR_OK)
    {
        We_RsaRelease((WE_VOID *) pstNewMach);
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    uiIbs = uiObs = pstNewMach->uiIbs;    
    
    pstNewMach->stPadAlg.ePadMethod= pstKeyRefPar->stPadAlg.ePadMethod;
    pstNewMach->stPadAlg.iNullByte= pstKeyRefPar->stPadAlg.iNullByte;
    
    if ((pstKeyRefPar->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)||(pstKeyRefPar->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)) 
    {
        pstNewMach->uiPbs = uiIbs - (WE_UINT32)(pstKeyRefPar->stPadAlg.iNullByte);
       if(pstKeyRefPar->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)
       {
            pstNewMach->uiPadSize =We_MbkPadPkcs1PadSize(&(pstNewMach->stPadParam));
       }
       else if(pstKeyRefPar->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)
       {
           pstNewMach->uiPadSize =We_MbkPadPkcs1NullPadSize(&(pstNewMach->stPadParam));           
       }
       else
       {
           We_RsaRelease((WE_VOID *) pstNewMach);
           return M_WE_LIB_ERR_GENERAL_ERROR;
       }

        //pstNewMach->uiPadSize = pstKeyRefPar->pstPadAlg->puiPadSizeFn(&(pstNewMach->stPadParam));
       if (pstMode->eOp == E_WE_RSA_SIGN || pstMode->eOp == E_WE_RSA_ENCRYPT) 
       {
          uiIbs-=(WE_UINT32)(pstKeyRefPar->stPadAlg.iNullByte) + pstNewMach->uiPadSize;
       }
       else
       {  
            uiObs -= (WE_UINT32)(pstKeyRefPar->stPadAlg.iNullByte) + pstNewMach->uiPadSize;
       }
       pstNewMach->uiIbs = uiIbs;
       pstNewMach->uiObs = uiObs;
    } 
    else 
    {
        pstNewMach->uiPadSize = 0;
        pstNewMach->uiObs = pstNewMach->uiPbs = pstNewMach->uiIbs = uiIbs;
    }
    
    pstNewMach->uiFbs = 0;  
    
    if(pstMode->eOp == E_WE_RSA_DECRYPT || pstMode->eOp == E_WE_RSA_ENCRYPT) 
    {
        pstNewMach->stRSAMachState.stConf.pucPending = WE_MALLOC(uiIbs);
        if(!(pstNewMach->stRSAMachState.stConf.pucPending)) 
        {
            We_RsaRelease((WE_VOID *) pstNewMach);
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        (WE_VOID)WE_MEMSET(pstNewMach->stRSAMachState.stConf.pucPending,0, uiIbs);
        pstNewMach->stRSAMachState.stConf.uiNPend = 0;
        
        if(pstNewMach->uiPadSize)
        {
            if(pstMode->eOp == E_WE_RSA_ENCRYPT)
            {
                pstNewMach->stRSAMachState.stConf.pfnRsaFn=We_RsaEncryptPad;
            }
            else
            {
                pstNewMach->stRSAMachState.stConf.pfnRsaFn=We_RsaDecryptPad;
            }
        }
        else
        {
            if(pstMode->eOp == E_WE_RSA_ENCRYPT)
            {
                pstNewMach->stRSAMachState.stConf.pfnRsaFn=We_RsaEncrypt;
            }
            else
            {
                pstNewMach->stRSAMachState.stConf.pfnRsaFn=We_RsaDecrypt;
            }
        }
        
        if(pstMode->eOp == E_WE_RSA_DECRYPT &&(pstNewMach->stPadAlg.ePadMethod == E_RSA_PAD_AUTOD1PKCS ||\
            pstNewMach->stPadAlg.ePadMethod == E_RSA_PAD_AUTOD1BRSA))
        {
            pstNewMach->stRSAMachState.stConf.pfnRsaFn = We_RsaDecryptAutoUnPad;
        }
        pstNewMach->stRSAMachState.stConf.pstBuf = 0;
    }   
    else /*signature*/
    {    
        pstNewMach->stRSAMachState.stMachSig.stSigType= pstKeyRefPar->stSigType;
        if(pstKeyRefPar->eDataType != E_DATA_TYPE_HASH) /*the data not be hashed*/
        {
            pstNewMach->stRSAMachState.stMachSig.uiBufLen = 0;
            pstNewMach->stRSAMachState.stMachSig.pucBuf=NULL;
            pstNewMach->stRSAMachState.stMachSig.eDateType = E_DATA_TYPE_DATA;
        } 
        else /*the data has been hashed*/
        {
            pstNewMach->stRSAMachState.stMachSig.uiBufLen = 0;
            pstNewMach->stRSAMachState.stMachSig.pucBuf=NULL;
            pstNewMach->stRSAMachState.stMachSig.eDateType = E_DATA_TYPE_HASH;
        }
    } 
    *pHandle=(WE_VOID *)pstNewMach;
    return M_WE_LIB_ERR_OK;   
    
}
/*==================================================================================================
FUNCTION: 
    We_RsaAppend
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    rsa append
ARGUMENTS PASSED:
    WE_UINT8 *pucData[IN]:the input data.
    WE_UINT32 uiLength[IN]:the length of pucdata.
    WE_VOID * pvHandle[OUT]:the handle of data.
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 We_RsaAppend(WE_HANDLE hWeHandle,WE_UINT8 *pucData, WE_UINT32 uiLength, WE_VOID * pvHandle)
{
    P_St_RSAMachs pstMach = NULL;
    WE_INT32      iRes=M_WE_LIB_ERR_OK;
    
    if(!pvHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstMach = (P_St_RSAMachs) pvHandle;
    
    if (!pstMach->iForce) 
    {
        if (!uiLength)
        {
            return M_WE_LIB_ERR_OK;
        }
        if(pucData==NULL)
        {
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        } 
    }
    
    if(pstMach->eModOp == E_WE_RSA_ENCRYPT || pstMach->eModOp == E_WE_RSA_DECRYPT) 
    {
        P_St_UtMblkLists pstL=NULL;
        P_St_UtMblkLists pstN=NULL;
        WE_UINT32        uiRLen=0; 
        WE_UINT32        uiP=0;    
        WE_UINT32        uiSpare=0;
        uiP = pstMach->stRSAMachState.stConf.uiNPend;
        uiRLen = uiLength + uiP;
        if(pstMach->eModOp == E_WE_RSA_ENCRYPT)
        {
            if(pucData!=(pstMach->stRSAMachState.stConf.pucPending+uiP))
            {
                uiRLen +=pstMach->uiIbs;
            }
            else
            {
                uiRLen +=pstMach->uiIbs-1;
            }
        }
        uiRLen = (uiRLen / pstMach->uiIbs) * pstMach->uiObs;
        
        pstL = pstMach->stRSAMachState.stConf.pstBuf;
        if(uiRLen && (!pstL || (pstL = pstL->pstPrev,(uiSpare = pstL->stBlock.uiSize - pstL->stBlock.uiUsed) < uiRLen))) 
        {
            St_UtMblk stMt={0};
            WE_UINT32  uiAsz = uiRLen;
            
            if(pstL)
            {
                uiAsz -= uiSpare;
            }
            stMt.pucData=(WE_UINT8 *)WE_MALLOC(uiAsz);
            stMt.uiSize=uiAsz;
            stMt.uiUsed=uiAsz;

            pstN = WE_MALLOC(sizeof(*pstN));
            if(!stMt.pucData || !pstN ) 
            {
                if(stMt.pucData)
                {
                    WE_FREE(stMt.pucData);
                    stMt.pucData=NULL;
                }
                if(pstN)
                {
                    WE_FREE(pstN);
                }
                return M_WE_LIB_ERR_INVALID_PARAMETER;
            }
            (WE_VOID)WE_MEMSET(stMt.pucData,0,uiAsz);
            (WE_VOID)WE_MEMSET(pstN,0,sizeof(*pstN));
             stMt.uiUsed = 0;
            (WE_VOID)WE_MEMCPY(&(pstN->stBlock), &stMt, sizeof(stMt));
            if(!pstL)
            {
                pstMach->stRSAMachState.stConf.pstBuf = pstN->pstNext = pstN->pstPrev = pstL = pstN;
            }
            else 
            {
                pstN->pstPrev = pstL;
                pstN->pstNext = pstL->pstNext;
                pstN->pstNext->pstPrev = pstL->pstNext = pstN;
                if(!uiSpare)
                {
                    pstL = pstL->pstNext;
                }
            }
        }
        
        if(uiP) 
        {
            WE_UINT32 uiAd = pstMach->uiIbs - uiP;
            
            if(uiAd > uiLength)
            {
                uiAd = uiLength;
            }
            if(pucData != pstMach->stRSAMachState.stConf.pucPending + uiP)
            {         
                if(pucData!=NULL)
                {
                    (WE_VOID)WE_MEMCPY(pstMach->stRSAMachState.stConf.pucPending + uiP, pucData, uiAd);       
                }         
            }
            uiLength -= uiAd;
            if((uiP += uiAd) == pstMach->uiIbs) 
            {
                if(!pucData)
                {
                    return M_WE_LIB_ERR_INVALID_PARAMETER;
                }    
                pucData += uiAd;
                if(pstMach->stRSAMachState.stConf.pfnRsaFn(hWeHandle,pstMach, pstMach->uiIbs, pstMach->stRSAMachState.stConf.pucPending,
                    pstL->stBlock.pucData + pstL->stBlock.uiUsed)) 
                {
                    /*RSA encrypt/decrypt failed*/
                    return M_WE_LIB_ERR_INVALID_PARAMETER;
                }
                pstMach->stRSAMachState.stConf.uiNPend = 0;
                if((pstL->stBlock.uiUsed += pstMach->uiObs) == pstL->stBlock.uiSize)
                {
                    pstL = pstL->pstNext;
                }
            } 
            else
            {
                pstMach->stRSAMachState.stConf.uiNPend = uiP;
            }
        }
        if(pstL) 
        {
            uiRLen = ((pstL->stBlock.uiSize - pstL->stBlock.uiUsed) / pstMach->uiObs) * pstMach->uiIbs;
            if(uiLength > uiRLen) 
            {
                if(pucData==NULL)
                {
                    return M_WE_LIB_ERR_INVALID_PARAMETER;
                }
                iRes=pstMach->stRSAMachState.stConf.pfnRsaFn(hWeHandle,pstMach, uiRLen, 
                    pucData, pstL->stBlock.pucData + pstL->stBlock.uiUsed);
                if(iRes!=M_WE_LIB_ERR_OK)
                {
                    /*RSA encrypt/decrypt failed*/
                    return M_WE_LIB_ERR_GENERAL_ERROR;
                }
                pstL->stBlock.uiUsed = pstL->stBlock.uiSize;
                pucData += uiRLen;
                uiLength -= uiRLen;
                pstL = pstL->pstNext;
            }
        }
        if(uiLength >= pstMach->uiIbs) /*why???*/
        {
            if((pucData==NULL)||(pstL==NULL))
            {
                return M_WE_LIB_ERR_INVALID_PARAMETER;
            }
            uiRLen = uiLength - (uiLength % pstMach->uiIbs);
            /*call fn*/
            iRes=pstMach->stRSAMachState.stConf.pfnRsaFn( hWeHandle,pstMach, uiRLen, \
                pucData, pstL->stBlock.pucData + pstL->stBlock.uiUsed);
            if(iRes!=M_WE_LIB_ERR_OK) 
            {
                /*RSA encrypt/decrypt failed*/
                return M_WE_LIB_ERR_GENERAL_ERROR;
            }
            pstL->stBlock.uiUsed += (uiRLen / pstMach->uiIbs) * pstMach->uiObs;
            pucData += uiRLen;
            uiLength -= uiRLen;
        }
        
        if(pstMach->iForce) 
        {
            if(pstL==NULL)
            {
                return M_WE_LIB_ERR_INVALID_PARAMETER;
            }
            uiRLen = pstMach->stRSAMachState.stConf.uiNPend;
            /*call fn*/
            iRes=pstMach->stRSAMachState.stConf.pfnRsaFn(hWeHandle,pstMach, uiRLen, \
                pstMach->stRSAMachState.stConf.pucPending, pstL->stBlock.pucData + pstL->stBlock.uiUsed);
            if(iRes!=M_WE_LIB_ERR_OK) 
            {
                return M_WE_LIB_ERR_GENERAL_ERROR;
            }
            pstL->stBlock.uiUsed += (uiRLen > pstMach->uiIbs) ? (uiRLen / pstMach->uiIbs) * pstMach->uiObs : pstMach->uiObs;
            if(pucData!=NULL)
            {
                pucData += uiRLen;
            }            
            if (uiLength > uiRLen)
            {
                uiLength -= uiRLen;
            }
            else
            {
                uiLength = 0;
            }
        }
        if(uiLength) 
        {
            if(pucData==NULL)
            {
                return M_WE_LIB_ERR_INVALID_PARAMETER;
            }
            pstMach->stRSAMachState.stConf.uiNPend = uiLength;
            (WE_VOID)WE_MEMCPY(pstMach->stRSAMachState.stConf.pucPending, pucData, uiLength);
        }
    } 
    else /*signature*/
    { 
       if(pstMach->stRSAMachState.stMachSig.eDateType == E_DATA_TYPE_DATA) 
       {
           pstMach->stRSAMachState.stMachSig.pucBuf = (WE_UINT8 *)WE_MALLOC(uiLength);
           if(pstMach->stRSAMachState.stMachSig.pucBuf==NULL)
           {
                return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
           }
           (WE_VOID)WE_MEMSET(pstMach->stRSAMachState.stMachSig.pucBuf,0,uiLength);
           (WE_VOID)WE_MEMCPY(pstMach->stRSAMachState.stMachSig.pucBuf, pucData, uiLength);
           pstMach->stRSAMachState.stMachSig.uiBufLen = uiLength;
       }
       else
       {
           if ((pstMach->stRSAMachState.stMachSig.uiBufLen) ||(pucData==NULL))
           {
               /*Hash data already read into RSA pvSession*/
               return M_WE_LIB_ERR_GENERAL_ERROR;
           }
           pstMach->stRSAMachState.stMachSig.pucBuf = (WE_UINT8 *)WE_MALLOC(uiLength);
           if(pstMach->stRSAMachState.stMachSig.pucBuf==NULL)
           {
               return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
           }
           (WE_VOID)WE_MEMSET(pstMach->stRSAMachState.stMachSig.pucBuf,0,uiLength);
           (WE_VOID)WE_MEMCPY(pstMach->stRSAMachState.stMachSig.pucBuf, pucData, uiLength);           
           pstMach->stRSAMachState.stMachSig.uiBufLen = uiLength;
       }
    }   
    return iRes;   
}
/*==================================================================================================
FUNCTION: 
    We_RsaFinish
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    rsa finish
ARGUMENTS PASSED:
    WE_HANDLE Handle[IN]:the handle of data.
    P_St_UtMblkLists *pstData[OUT]:the data.
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 We_RsaFinish(WE_HANDLE hWeHandle,WE_HANDLE Handle, P_St_UtMblkLists *pstData)
{
    WE_INT32          iRes =0;
    P_St_RSAMachs     pstMach = NULL;
    
    if(!Handle||!pstData)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    pstMach = (P_St_RSAMachs) Handle;     
    
    if (pstMach->eModOp == E_WE_RSA_VERIFY) 
    {
        if(*pstData==0)
        {
            We_RsaRelease(Handle);
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        if((*pstData)->stBlock.pucData==0)
        {
            We_RsaRelease(Handle);
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }        
    }
    
    switch(pstMach->eModOp) 
    {
    case E_WE_RSA_ENCRYPT: 
        iRes=We_RsaEncryptFinish(hWeHandle,pstMach,Handle,pstData);         
        break;        
    case E_WE_RSA_DECRYPT:
        iRes=We_RsaDecryptFinish(pstMach,pstData);
        break;
    case E_WE_RSA_SIGN: 
        iRes= We_RsaSignFinish( hWeHandle,pstMach, pstData);          
        break;
    case E_WE_RSA_VERIFY: 
        iRes= We_RsaVerifySigFinish( hWeHandle,pstMach,pstData);        
        break;
    default:
        iRes=M_WE_LIB_ERR_UNSUPPORTED_METHOD;
    }
    
    We_RsaRelease(Handle);
    return iRes;
}

/*==================================================================================================
FUNCTION: 
    We_RsaRelease
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    release the data.
ARGUMENTS PASSED:
    WE_VOID * pvHandle[IN]:handle of data.
RETURN VALUE:
    none.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID We_RsaRelease(WE_VOID * pvHandle)
{
    P_St_RSAMachs pstMach = NULL;
    if(!pvHandle)
    {
        return ;
    }
    pstMach = (P_St_RSAMachs) pvHandle;
    if (pstMach->pstRSAKey) 
    {
        We_RsaDisposeKey(pstMach->pstRSAKey);
        M_WE_RSA_SELF_FREE(pstMach->pstRSAKey);
    }
    
    if(pstMach->pvModulus)
    {
        We_MpmKillArc(pstMach->pvModulus);
    }
    if(pstMach->eModOp == E_WE_RSA_ENCRYPT || pstMach->eModOp == E_WE_RSA_DECRYPT) 
    {
        if (pstMach->stRSAMachState.stConf.pucPending)
        {
            M_WE_RSA_SELF_FREE(pstMach->stRSAMachState.stConf.pucPending);
        }
        if(pstMach->stRSAMachState.stConf.pstBuf) 
        {
            pstMach->stRSAMachState.stConf.pstBuf->pstPrev->pstNext = 0;
            We_RsaDisposeMblkList(pstMach->stRSAMachState.stConf.pstBuf);
        }
    } 

    WE_FREE(pstMach);    
}

/*==================================================================================================
FUNCTION: 
    We_RsaDisposeKey
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    release key.
ARGUMENTS PASSED:
    St_RsaKey *pstRSAkey[IN]:rsa key.
RETURN VALUE:
    none.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
#define KILLIFTHERE(x) if(x) We_MpmKillArc(x);

WE_VOID We_RsaDisposeKey(St_RsaKey *pstRSAkey)
{
    if(pstRSAkey==NULL)
    {
        return ;
    }
    if(pstRSAkey->eKeyTypeTag == E_RSA_KTAG_SIMPLE_KEY) 
    {
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stSimpleKey.pvModulus);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stSimpleKey.pvExpon);
    } 
    else if(pstRSAkey->eKeyTypeTag == E_RSA_KTAG_CRT_KEY) 
    {
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pvPFactor);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pvQFactor);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pvPExpon);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pvQExpon);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pvConvConst);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pvModulus);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pvExpon);
        KILLIFTHERE(pstRSAkey->stKeyTypeUnion.stCrtKey.pub_expon);
    }
}
/*==================================================================================================
FUNCTION: 
    We_RsaCryptGeneral
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    crypt general.
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:data.
    WE_VOID * pvExponArc[OUT]:expon arc.
    WE_VOID * pvModArc[OUT]:mode arc.
    WE_VOID * pvMessArc[OUT]:message arc.
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID *We_RsaCryptGeneral(WE_VOID * pvMid, WE_VOID * pvExponArc, WE_VOID * pvModArc, WE_VOID * pvMessArc)
{
    const E_MpmReg eM = E_MPM_R0; 
    const E_MpmReg eE = E_MPM_R1; 
    const E_MpmReg eC = E_MPM_R2; 
    WE_INT32 iRval=0;
    
    We_MpmRestore(pvMid, E_MPM_RMOD, pvModArc, E_TRUE);
    We_MpmRestore(pvMid, eM, pvMessArc, E_TRUE);
    
    
    if(We_MpmCmp(pvMid, eM, E_MPM_RMOD), !We_MpmGe(pvMid,&iRval)&&iRval)
    {
        return NULL;
    }
    
    We_MpmRestore(pvMid, eE, pvExponArc, E_TRUE);
    We_MpmExpm(pvMid, eC, eM, eE);
    return We_MpmBackUp(pvMid, eC, E_FALSE);
}
/*==================================================================================================
FUNCTION: 
    We_RsaCryptFermat
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    crypt fermat.
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]: data.
    WE_UINT32 uiFermatNo[IN]: fermat type.
    WE_VOID * pvModArc[OUT] :mode arc.
    WE_VOID * pvMessArc[OUT]:message arc.
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID *We_RsaCryptFermat(WE_VOID * pvMid, WE_UINT32 uiFermatNo, WE_VOID * pvModArc, WE_VOID * pvMessArc)
{
    WE_UINT32           uiSqrs = 1 << (uiFermatNo - 1); 
    WE_UINT32           iLoop=0; 
    WE_INT32            iRval=0;
    const E_MpmReg      eM = E_MPM_R0; 
    const E_MpmReg      eC = E_MPM_R1; 
    
    We_MpmRestore(pvMid, E_MPM_RMOD, pvModArc, E_TRUE);
    We_MpmRestore(pvMid, eM, pvMessArc, E_TRUE);
    
    
    if(We_MpmCmp(pvMid, eM, E_MPM_RMOD), !We_MpmGe(pvMid,&iRval)&&iRval)
    {
        /*message too big*/
        return NULL;
    }    
    We_MpmMove(pvMid, eC, eM, E_TRUE); 
    for(iLoop = 0; iLoop < uiSqrs; iLoop++)
    {
        We_MpmMulm(pvMid, eC, eC, eC); 
    }
    We_MpmMulm(pvMid, eC, eC, eM); 
    return We_MpmBackUp(pvMid, eC, E_FALSE);
}
/*==================================================================================================
FUNCTION: 
    We_RsaCryptCrt
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    crypt crt.
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:data.
    St_RsaKey *pstRSAKey[OUT]:rsa key
    WE_VOID * pvMessArc[OUT]:
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID *We_RsaCryptCrt(WE_VOID * pvMid, St_RsaKey *pstRSAKey, WE_VOID * pvMessArc)
{
    WE_INT32              iRval=0;
    const E_MpmReg        eC  = E_MPM_R0; 
    const E_MpmReg        eMp = E_MPM_R1;
    const E_MpmReg        eMq = E_MPM_R2;
    const E_MpmReg        eQ  = E_MPM_R3;
    const E_MpmReg        eDp = E_MPM_R4;
    const E_MpmReg        eDq = E_MPM_R5;
    const E_MpmReg        eM  = E_MPM_R6;
    
    if((pstRSAKey==NULL)||(pvMid==NULL)||(pvMessArc==NULL))
    {
        return NULL;
    } 
    
    We_MpmRestore(pvMid, E_MPM_RMOD, pstRSAKey->stKeyTypeUnion.stCrtKey.pvModulus, E_TRUE);
    We_MpmRestore(pvMid, eC, pvMessArc, E_TRUE);
    
    if(We_MpmCmp(pvMid, eC, E_MPM_RMOD), !We_MpmGe(pvMid,&iRval)&&iRval)
    {
        /*bad parameter*/
        return NULL;
    } 
    
    We_MpmRestore(pvMid, E_MPM_RMOD, pstRSAKey->stKeyTypeUnion.stCrtKey.pvQFactor, E_TRUE);
    We_MpmRestore(pvMid, eDq, pstRSAKey->stKeyTypeUnion.stCrtKey.pvQExpon, E_TRUE);
    We_MpmMod(pvMid, eMq, eC);
    We_MpmExpm(pvMid, eMq, eMq, eDq);
    We_MpmMove(pvMid, eQ, E_MPM_RMOD, E_FALSE);
    We_MpmRestore(pvMid, E_MPM_RMOD, pstRSAKey->stKeyTypeUnion.stCrtKey.pvPFactor, E_TRUE);
    We_MpmRestore(pvMid, eDp, pstRSAKey->stKeyTypeUnion.stCrtKey.pvPExpon, E_TRUE);
    We_MpmMod(pvMid, eMp, eC);
    We_MpmExpm(pvMid, eMp, eMp, eDp);
    We_MpmRestore(pvMid, eM, pstRSAKey->stKeyTypeUnion.stCrtKey.pvConvConst, E_TRUE);
    We_MpmSub(pvMid, eMp, eMp, eMq);
    We_MpmMulm(pvMid, eM, eM, eMp);
    We_MpmMul(pvMid, eM, eM, eQ);
    We_MpmAdd(pvMid, eM, eM, eMq);
    return We_MpmBackUp(pvMid, eM, E_FALSE);
}
/*==================================================================================================
FUNCTION: 
    We_RsaCrypt
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    rsa crypt
ARGUMENTS PASSED:
    St_RsaKey *pstRSAKey[OUT]:rsa key.
    WE_VOID * pvMessArc[OUT]:message arc.
RETURN VALUE:

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID *We_RsaCrypt(St_RsaKey *pstRSAKey, WE_VOID * pvMessArc)
{
    WE_VOID         *pvMid = NULL;
    WE_VOID         *pvRval = NULL;
    
    if((pstRSAKey==NULL)||(pvMessArc==NULL))
    {    
        return NULL;
    }    
    
    pvMid = We_MpmNewMach();    
    if(pvMid) 
    {
        if(pstRSAKey->eKeyTypeTag != E_RSA_KTAG_SIMPLE_KEY)
        {
            pvRval = We_RsaCryptCrt(pvMid, pstRSAKey, pvMessArc);
        }
        else if(pstRSAKey->stKeyTypeUnion.stSimpleKey.uiFermatNo == 0)
        {
            pvRval = We_RsaCryptGeneral(pvMid, pstRSAKey->stKeyTypeUnion.stSimpleKey.pvExpon,\
                pstRSAKey->stKeyTypeUnion.stSimpleKey.pvModulus, pvMessArc);
        }
        else 
        {
            pvRval = We_RsaCryptFermat(pvMid, pstRSAKey->stKeyTypeUnion.stSimpleKey.uiFermatNo,\
                pstRSAKey->stKeyTypeUnion.stSimpleKey.pvModulus, pvMessArc);
        }
        We_MpmDisposeMach(pvMid);
    }
    return pvRval;
}
/*==================================================================================================
FUNCTION: 
    We_RsaEncryptFinish
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    finish encrypt.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN]:mach
    WE_HANDLE handle[IN][OUT]:handle.
    P_St_UtMblkLists *pstData[IN/OUT]:data.
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaEncryptFinish(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach,WE_HANDLE handle,
                                    P_St_UtMblkLists *pstData)
{    
    WE_UINT32            uiPad = 0;        
    WE_UINT32            uiOpad = 0;
    WE_INT32             iRes=0;
    P_St_UtMblkLists     pstL = NULL;
    
    if(!pstMach||!handle||!pstData)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    uiPad=pstMach->uiIbs - pstMach->stRSAMachState.stConf.uiNPend;
    uiOpad = uiPad;
    /*pucTemp = pstMach->stRSAMachState.stConf.pucPending + pstMach->stRSAMachState.stConf.uiNPend;*/
    if(!(pstMach->stRSAMachState.stConf.uiNPend)) 
    { 
        iRes = 0;
    } 
    else 
    {
        if(pstMach->uiPadSize) 
        {
            pstMach->iForce = 1;
            iRes = We_RsaAppend(hWeHandle,0, 0, handle);
        }
        else 
        {        
            if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)
            {
                iRes=We_MbkPadPkcs1Pad(hWeHandle,pstMach->stRSAMachState.stConf.pucPending, 
                    pstMach->stRSAMachState.stConf.uiNPend, pstMach->uiIbs,&pstMach->stPadParam);
            }
            else if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)
            {
                iRes=We_MbkPadPkcs1NullPad(hWeHandle,pstMach->stRSAMachState.stConf.pucPending, 
                    pstMach->stRSAMachState.stConf.uiNPend, pstMach->uiIbs,&pstMach->stPadParam);
            }
            else
            {
                return M_WE_LIB_ERR_INVALID_PARAMETER;
            }
            /* iRes = pstMach->pstPadAlg->piPadFn(pstMach->stRSAMachState.stConf.pucPending, 
                 pstMach->stRSAMachState.stConf.uiNPend, pstMach->uiIbs,&pstMach->stPadParam);*/
            if(iRes) 
            {
                /*padding failed*/
                return iRes;
            }            
            iRes = We_RsaAppend( hWeHandle,pstMach->stRSAMachState.stConf.pucPending + pstMach->stRSAMachState.stConf.uiNPend, 
                uiOpad, handle);
        }        
    }     
    
    if(!iRes)
    {
        pstL = pstMach->stRSAMachState.stConf.pstBuf;
        if(pstL)
        {
            pstL->pstPrev->pstNext = pstMach->stRSAMachState.stConf.pstBuf = 0;
        }
        else
        {
            pstL = WE_MALLOC(sizeof(*pstL));
            if (!pstL) 
            {
                iRes = M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
            }
            else
            {
                (WE_VOID)WE_MEMSET (pstL, 0, sizeof(*pstL));
            }    
        }
    } 
    *pstData =pstL;    
    return iRes;    
}
/*==================================================================================================
FUNCTION: 
    We_RsaDecryptFinish
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    decrypt finish.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN]:mach.
    P_St_UtMblkLists *pstData[IN/OUT]:data.
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaDecryptFinish(P_St_RSAMachs pstMach,P_St_UtMblkLists *pstData)
{      
    WE_INT32            iRes=0;
    P_St_UtMblkLists    pstL = NULL;

    if((pstMach==NULL)||(pstData==NULL))
    {
    	return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if(pstMach->stRSAMachState.stConf.uiNPend)
    {
        //ut_log1(BSAPI_EXTRAC, UT_ACONTINUE, "%d trailing ciphertext octets",pstMach->stRSAMachState.stConf.uiNPend);
        ;
    }     
    else 
    {
        pstL = pstMach->stRSAMachState.stConf.pstBuf; 
        
        if(!pstL) 
        {          
            iRes = 0;
        } 
        else if (pstMach->uiPadSize) 
        { 
            //pstL->prev->block.uiUsed-=(pstMach->uiObs - pstMach->uiFbs);
            pstL->pstPrev->stBlock.uiUsed= pstMach->uiFbs;
            iRes = 0;
        } 
        else 
        { 
            P_St_UtMblkLists     pstT = NULL;
            WE_UINT32            uiBody = 0;
            WE_UINT32            uiTail=0;

            pstT = pstL->pstPrev;
            uiBody = pstT->stBlock.uiUsed - pstMach->uiObs;
            if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)
            {
                iRes=We_MbkPadPkcs1Unpad(pstT->stBlock.pucData + uiBody, \
                    &uiTail, pstMach->uiObs, &pstMach->stPadParam);
            }
            else if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)
            {
                iRes=We_MbkPadPkcs1Unpad(pstT->stBlock.pucData + uiBody, \
                    &uiTail, pstMach->uiObs, &pstMach->stPadParam);
            }
            else
            {
                return M_WE_LIB_ERR_INVALID_PARAMETER;
            }
            
            if(iRes)
            {
                /*unpadding (decrypt?) failed)*/
                return iRes;
            }
            else 
            {
               // iRv = 0;
                pstT->stBlock.uiUsed = uiBody + uiTail;
                if(!pstT->stBlock.uiUsed) 
                {
                    pstL->pstPrev = pstT->pstPrev;
                    pstT->pstNext = 0;
                    if(pstT == pstL)
                    {
                        pstMach->stRSAMachState.stConf.pstBuf = pstL = 0;
                    }
                    We_RsaDisposeMblkList(pstT);
                }
            }
        }
        if(pstL)
        {
            pstL->pstPrev->pstNext = pstMach->stRSAMachState.stConf.pstBuf = 0;
        }
        else 
        {
            pstL = WE_MALLOC(sizeof(*pstL));
            
            if (!pstL) 
            {
                iRes = M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
            }
            else
            {
                (WE_VOID)WE_MEMSET (pstL, 0, sizeof(*pstL));
            }
        }
    }
    *pstData = pstL;
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_RsaSignFinish
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    finish signature.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN/OUT]:mach.
    P_St_UtMblkLists *pstData[OUT]:data.
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaSignFinish(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach,P_St_UtMblkLists *pstData)
{
    WE_INT32            iRes=0;
    P_St_UtMblkLists    pstL = NULL;
    WE_UINT32           uiMLen=0;    
    WE_UINT32           uiLen=0;    
    WE_UINT32           uiOLen=0;    
    St_RsaHashDigest    pstHashDig={0};
    E_RsaHashType       eHashType=E_HASH_NUMERIC;
    WE_UINT8            *pucBerHash=NULL; 
    WE_UINT8            *pucEncHash=NULL; 
    WE_UINT8            *pucDp=NULL;      
    St_UtMblk           stTmp={0};
    St_UtMblk           stEncoid={0};  
    WE_INT32            iTmpRes=0;    
    
    if((pstMach==NULL)||(pstData==NULL))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stEncoid.pucData = NULL;
    
    if(pstMach->stRSAMachState.stMachSig.eDateType == E_DATA_TYPE_DATA) 
    {
        pstHashDig.stHashValue.pucData=(WE_UCHAR *)WE_MALLOC(pstMach->stRSAMachState.stMachSig.uiBufLen);
        if(!(pstHashDig.stHashValue.pucData))
        {
            if(pstMach->stRSAMachState.stMachSig.pucBuf)
            {
                WE_FREE(pstMach->stRSAMachState.stMachSig.pucBuf);
            }
            return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
        }
        (WE_VOID)WE_MEMSET(pstHashDig.stHashValue.pucData,0,pstMach->stRSAMachState.stMachSig.uiBufLen);
        (WE_VOID)WE_MEMCPY(pstHashDig.stHashValue.pucData,\
            pstMach->stRSAMachState.stMachSig.pucBuf,pstMach->stRSAMachState.stMachSig.uiBufLen);
        eHashType = pstMach->stRSAMachState.stMachSig.stSigType.eHashType;
        pstHashDig.stHashValue.uiSize=(pstMach->stRSAMachState.stMachSig.uiBufLen)*8;
        if(pstMach->stRSAMachState.stMachSig.pucBuf)
        {
            WE_FREE(pstMach->stRSAMachState.stMachSig.pucBuf);
        }
    }
    else 
    {
        pstHashDig.stHashValue.pucData = pstMach->stRSAMachState.stMachSig.pucBuf;
        pstHashDig.stHashValue.uiSize = pstMach->stRSAMachState.stMachSig.uiBufLen << 3;
        eHashType = E_HASH_BITS;
    }
    if(eHashType == E_HASH_NUMERIC ? !pstHashDig.pvNum : !pstHashDig.stHashValue.pucData)
    {
        /*unable to finish hash pvSession*/
        iRes=M_WE_LIB_ERR_GENERAL_ERROR;
        return iRes;
        
    } 
    else if(eHashType == E_HASH_NUMERIC)
    {
        uiMLen = (WE_UINT32)We_MpmSizeOfArc2BerInteger(pstHashDig.pvNum);
    }
    else if ((pstMach->stPadAlg.ePadMethod ==E_RSA_PAD_PKCS1)&&(pstMach->stPadAlg.iNullByte ==1)&& pstMach->stRSAMachState.stMachSig.stSigType.eSigType)
    {
        stTmp.pucData = pstHashDig.stHashValue.pucData;
        stTmp.uiSize = stTmp.uiUsed = (WE_UINT32) (pstHashDig.stHashValue.uiSize >> 3);
        uiMLen = (WE_UINT32)We_MbkAsn1SizeOfMblk2BerOctetString(stTmp);
    } 
    else 
    {
        pucBerHash = pstHashDig.stHashValue.pucData;
        uiMLen = pstHashDig.stHashValue.uiSize >> 3;
    }   
    
    if ((pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)&&(pstMach->stPadAlg.iNullByte==1)) 
    {
        if (pstMach->stRSAMachState.stMachSig.stSigType.eSigType)
        {              
            switch ((WE_INT32)(pstMach->stRSAMachState.stMachSig.stSigType.eSigType) )
            {                 
            case E_WE_RSA_PKCS1_MD2:
                stEncoid.pucData = (WE_UINT8 *) "\x30\x20\x30\x0c\x06\x08\x2A\x86\x48\x86\xF7"
                    "\x0D\x02\x02\x05\x00";
                stEncoid.uiUsed = stEncoid.uiSize = 16;
                uiMLen += stEncoid.uiSize;
                break;
                
            case E_WE_RSA_PKCS1_MD5:
                stEncoid.pucData = (WE_UINT8 *) "\x30\x20\x30\x0c\x06\x08\x2A\x86\x48\x86\xF7"
                    "\x0D\x02\x05\x05\x00";
                stEncoid.uiUsed = stEncoid.uiSize = 16;
                uiMLen += stEncoid.uiSize;
                break;                  
                
            case E_WE_RSA_PKCS1_SHA1:
                stEncoid.pucData = (WE_UINT8 *) "\x30\x21\x30\x09\x06\x05\x2B\x0E\x03\x02\x1A"
                    "\x05\x00";
                stEncoid.uiUsed = stEncoid.uiSize = 13;
                uiMLen += stEncoid.uiSize;
                break;
            default:
                break;
            }
           
            pucBerHash = WE_MALLOC(uiMLen);
            if (!pucBerHash) 
            {
                iRes=M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
                
                if(eHashType == E_HASH_NUMERIC)
                {
                    We_MpmKillArc(pstHashDig.pvNum);
                }
                else
                {
                    M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
                }                
                return iRes;
            }
            (WE_VOID)WE_MEMSET(pucBerHash,0,uiMLen);
            pucDp = pucBerHash;
          
            if (stEncoid.pucData) 
            {                    
                (WE_VOID)WE_MEMCPY(pucDp, stEncoid.pucData, stEncoid.uiUsed);
                pucDp += stEncoid.uiUsed;
            }
            
            if(eHashType == E_HASH_NUMERIC)
            {
                pucDp = We_MpmArc2BerInteger(pstHashDig.pvNum, pucDp);
            }
            else
            {
                pucDp = We_MbkAsn1Mblk2BerOctetString(stTmp, pucDp); 
            }
  
            if(!pucDp) 
            {
                iRes=M_WE_LIB_ERR_GENERAL_ERROR;
                if(pucBerHash != pstHashDig.stHashValue.pucData)
                {
                    WE_FREE(pucBerHash);
                    pucBerHash=NULL;
                } 
                
                if(eHashType == E_HASH_NUMERIC)
                {
                    We_MpmKillArc(pstHashDig.pvNum);
                }
                else
                {
                    M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
                }              
                *pstData = NULL;               
                return iRes;
            }  
            uiMLen = (WE_UINT32)(pucDp - pucBerHash);
        }
    }   
    uiOLen = (( (uiMLen-1)/pstMach->uiIbs )+1) * pstMach->uiObs;
    pucEncHash = (WE_UINT8 *)WE_MALLOC(uiOLen);
    if(!pucEncHash )
    {  
        if(eHashType == E_HASH_NUMERIC)
        {
            We_MpmKillArc(pstHashDig.pvNum);
        }
        else
        {
            M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
        }       
        *pstData = NULL;
        
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    (WE_VOID)WE_MEMSET(pucEncHash,0,uiOLen);
    if (pstMach->uiPadSize) 
    {  
        iTmpRes=We_RsaEncryptPad(hWeHandle,pstMach, uiMLen, pucBerHash, pucEncHash);
        if(iTmpRes) 
        {
            /*encrypt failure*/
            WE_FREE(pucEncHash);
            pucEncHash=NULL; 
            if(pucBerHash != pstHashDig.stHashValue.pucData)
            {
                M_WE_RSA_SELF_FREE(pucBerHash);
            }
            if(eHashType == E_HASH_NUMERIC)
            {
                We_MpmKillArc(pstHashDig.pvNum);
            }
            else
            {
                M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
            }          
            *pstData = NULL;
            
            return M_WE_LIB_ERR_GENERAL_ERROR;
            
        }
     } 
     else
     {           
        WE_UINT8 *pucNewBerHash=NULL; 
        uiLen = (( (uiMLen-1)/pstMach->uiIbs )+1) * pstMach->uiIbs;
        
        if (pucBerHash == pstHashDig.stHashValue.pucData)
        {
            pstHashDig.stHashValue.pucData = 0;
        }
        pucNewBerHash = WE_MALLOC(uiLen);
        if(!pucNewBerHash) 
        {
            WE_FREE(pucEncHash);
            pucEncHash=NULL;  
            if(pucBerHash != pstHashDig.stHashValue.pucData)
            {
                M_WE_RSA_SELF_FREE(pucBerHash);
            }               
            if(eHashType == E_HASH_NUMERIC)
            {
                We_MpmKillArc(pstHashDig.pvNum);
            }
            else
            {
                M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
            }           
            
            *pstData = NULL;            
            return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
            
        }
        (WE_VOID)WE_MEMSET(pucNewBerHash,0,uiLen);
        if(pucBerHash)
        {
            (WE_VOID)WE_MEMCPY(pucNewBerHash, pucBerHash, uiMLen);
            WE_FREE(pucBerHash);
            pucBerHash=NULL;
        }
        else
        {
            WE_FREE(pucEncHash);
            pucEncHash=NULL; 
            WE_FREE(pucNewBerHash);
            pucNewBerHash=NULL;
            return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
        }
        pucBerHash = pucNewBerHash;
        
        if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)
        {
            iRes=We_MbkPadPkcs1Pad(hWeHandle,pucBerHash, uiMLen, uiLen, &pstMach->stPadParam);
        }
        else if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)
        {
            iRes=We_MbkPadPkcs1NullPad(hWeHandle,pucBerHash, uiMLen, uiLen, &pstMach->stPadParam);
        }
        else
        {
            /*padding failed*/
            WE_FREE(pucEncHash);
            pucEncHash=NULL; 
            if(pucBerHash != pstHashDig.stHashValue.pucData)
            {
                WE_FREE(pucBerHash);
                pucBerHash=NULL;                    
            }              
            if(eHashType == E_HASH_NUMERIC)
            {
                We_MpmKillArc(pstHashDig.pvNum);
            }
            else
            {
                M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
            }         
            *pstData = NULL;
            return M_WE_LIB_ERR_GENERAL_ERROR;  
        }
        if(iRes) 
        {
            /*padding failed*/
            WE_FREE(pucEncHash);
            pucEncHash=NULL; 
            if(pucBerHash != pstHashDig.stHashValue.pucData)
            {
                WE_FREE(pucBerHash);
                pucBerHash=NULL;                    
            }              
            if(eHashType == E_HASH_NUMERIC)
            {
                We_MpmKillArc(pstHashDig.pvNum);
            }
            else
            {
                M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
            }         
            *pstData = NULL;
            return M_WE_LIB_ERR_GENERAL_ERROR;                
        }
        iTmpRes=We_RsaEncrypt( hWeHandle,pstMach, uiLen, pucBerHash, pucEncHash);
        if (iTmpRes) 
        {
            /*RSA encrypt failed*/
            WE_FREE(pucEncHash);
            pucEncHash=NULL; 
            if(pucBerHash != pstHashDig.stHashValue.pucData)
            {
                WE_FREE(pucBerHash);
                pucBerHash=NULL; 
            }          
            if(eHashType == E_HASH_NUMERIC)
            {
                We_MpmKillArc(pstHashDig.pvNum);
            }
            else
            {
                M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
            }  
            
            *pstData = NULL;
            return M_WE_LIB_ERR_GENERAL_ERROR;            
        }
     }  
     pstL = WE_MALLOC(sizeof(*pstL));
     if(!pstL)
     {
         return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
     }
     (WE_VOID)WE_MEMSET(pstL,0,sizeof(*pstL));

     pstL->stBlock.pucData = (WE_UINT8 *)WE_MALLOC(uiOLen);     
     if(!(pstL->stBlock.pucData)) 
     {        
         WE_FREE(pucEncHash);
         pucEncHash=NULL;         
         if(pucBerHash != pstHashDig.stHashValue.pucData)
         {
             M_WE_RSA_SELF_FREE(pucBerHash);
         }
         WE_FREE(pstL);
         pstL =NULL;         
         if(eHashType == E_HASH_NUMERIC)
         {
             We_MpmKillArc(pstHashDig.pvNum);
         }
         else
         {
             M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
         }
         *pstData = NULL;
         
         return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
         
     }
     (WE_VOID)WE_MEMSET(pstL->stBlock.pucData,0,uiOLen);
     pstL->stBlock.uiSize = pstL->stBlock.uiUsed = uiOLen;
     pstL->pstNext = 0;
     pstL->pstPrev = pstL;
     (WE_VOID)WE_MEMCPY(pstL->stBlock.pucData, pucEncHash, uiOLen);
     //iRv = 0;
     *pstData = pstL;    

     WE_FREE(pucEncHash);
     pucEncHash=NULL;
     if(pucBerHash != pstHashDig.stHashValue.pucData)
     {
         WE_FREE(pucBerHash);
     }
     if(eHashType == E_HASH_NUMERIC)
     {
         We_MpmKillArc(pstHashDig.pvNum);
     }
     else
     {
         M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
     }
     M_WE_RSA_SELF_FREE(pstHashDig.stHashValue.pucData);
     return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_RsaVerifySigFinish
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    verify signature finish.
ARGUMENTS PASSED:
    P_St_RSAMachs pstMach[IN/OUT]:
    P_St_UtMblkLists *pstData[OUT]:
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 We_RsaVerifySigFinish(WE_HANDLE hWeHandle,P_St_RSAMachs pstMach,P_St_UtMblkLists *pstData)
{
    WE_INT32           iRes=0;
    St_RsaHashDigest   stHashDig={0};
    E_RsaHashType      eHashType=E_HASH_NUMERIC;
    St_UtMblk          stHash = {0}; 
    WE_UINT32          uiHashLen=0;           
    WE_UINT8           *pucHash=NULL;             
    WE_UINT32          uiUnPadLen=0;          
    WE_UINT32          uiPtl=0;
    WE_UINT8           *pucDe=NULL;
    WE_VOID            *pvHsh=NULL;   
    
    if((pstMach==NULL)||(pstData==NULL))
    {
         return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    
    if(pstMach->stRSAMachState.stMachSig.eDateType == E_DATA_TYPE_DATA) 
    {
        WE_UINT8            aucDigest[20]={0};
        WE_INT32            iDigLen=20; 
        E_WeHashAlgType     eHashType1=E_WE_ALG_HASH_NULL;

        iRes=Sec_GetHashType(pstMach->stRSAMachState.stMachSig.stSigType.eSigType,&eHashType1,&iDigLen);
        if(iRes)
        {
            return M_WE_LIB_ERR_INVALID_PARAMETER;
        }
        iRes= We_LibHash( hWeHandle,eHashType1, pstMach->stRSAMachState.stMachSig.pucBuf, \
            (WE_INT32)(pstMach->stRSAMachState.stMachSig.uiBufLen) , aucDigest, &iDigLen);
        
        stHashDig.stHashValue.pucData=(WE_UCHAR *)WE_MALLOC(pstMach->stRSAMachState.stMachSig.uiBufLen);
        if(!(stHashDig.stHashValue.pucData))
        {
            return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
        }
        (WE_VOID)WE_MEMSET(stHashDig.stHashValue.pucData,0,pstMach->stRSAMachState.stMachSig.uiBufLen);
        (WE_VOID)WE_MEMCPY(stHashDig.stHashValue.pucData,aucDigest,(WE_UINT32)iDigLen);
        stHashDig.stHashValue.uiSize=(WE_UINT32)(iDigLen*8);
        eHashType = pstMach->stRSAMachState.stMachSig.stSigType.eHashType;
        if(pstMach->stRSAMachState.stMachSig.pucBuf)
        {
            WE_FREE(pstMach->stRSAMachState.stMachSig.pucBuf);
            pstMach->stRSAMachState.stMachSig.pucBuf=NULL;
        }
    } 
    else 
    {
        stHashDig.stHashValue.pucData = pstMach->stRSAMachState.stMachSig.pucBuf;
        stHashDig.stHashValue.uiSize = pstMach->stRSAMachState.stMachSig.uiBufLen << 3;
        eHashType = E_HASH_BITS;
    }
    if(eHashType == E_HASH_NUMERIC ? !stHashDig.pvNum : !stHashDig.stHashValue.pucData) 
    {
        /*unable to finish hash pvSession*/
        return M_WE_LIB_ERR_GENERAL_ERROR;        
    }
    
    iRes=We_MbkCstSig2Sig(&((*pstData)->stBlock), &stHash);
    if(iRes!=M_WE_LIB_ERR_OK)
    {
        /*couldn't decode signature*/
        if(eHashType == E_HASH_NUMERIC)
        {
            We_MpmKillArc(stHashDig.pvNum);
        }
        else
        {
            M_WE_RSA_SELF_FREE(stHashDig.stHashValue.pucData);
        }
        return M_WE_LIB_ERR_GENERAL_ERROR;        
    }
    if(stHash.uiUsed % pstMach->uiIbs) 
    {
        /*signature incompatible with key*/
        if(stHash.pucData)
        {
            WE_FREE(stHash.pucData);
            stHash.pucData=NULL;
        }
        if(eHashType == E_HASH_NUMERIC)
        {
            We_MpmKillArc(stHashDig.pvNum);
        }
        else
        {
            M_WE_RSA_SELF_FREE(stHashDig.stHashValue.pucData);
        }
        return M_WE_LIB_ERR_INVALID_KEY;
        
    }
    uiHashLen = (stHash.uiUsed/pstMach->uiIbs) * pstMach->uiObs;
    pucHash =(WE_UINT8*) WE_MALLOC (uiHashLen);
    if(!pucHash)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }   
    (WE_VOID)WE_MEMSET(pucHash,0,uiHashLen);
    if(pstMach->uiPadSize) 
    { 
        if(We_RsaDecryptPad(hWeHandle,pstMach, stHash.uiUsed, stHash.pucData, pucHash))
        {
            iRes = M_WE_LIB_ERR_INVALID_KEY;
            /*couldn't decrypt signature*/
            WE_FREE(pucHash);
            if(stHash.pucData)
            {
                WE_FREE(stHash.pucData);
                stHash.pucData=NULL;
            }
            if(eHashType == E_HASH_NUMERIC)
            {
                We_MpmKillArc(stHashDig.pvNum);
            }
            else
            {
                M_WE_RSA_SELF_FREE(stHashDig.stHashValue.pucData);
            }
            return M_WE_LIB_ERR_INVALID_KEY;            
        }
        uiUnPadLen = pstMach->uiFbs;
    } 
    else 
    {         
        WE_INT32 iAutoSucceed = 0;      
        if (We_RsaDecrypt(hWeHandle,pstMach, stHash.uiUsed, stHash.pucData, pucHash))
        {
           // iRv = 2;
            /*couldn't decrypt signature*/
            WE_FREE(pucHash);
            if(stHash.pucData)
            {
                WE_FREE(stHash.pucData);
                stHash.pucData=NULL;
            }
            if(eHashType == E_HASH_NUMERIC)
            {
                We_MpmKillArc(stHashDig.pvNum);
            }
            else
            {
                M_WE_RSA_SELF_FREE(stHashDig.stHashValue.pucData);
            }
            return M_WE_LIB_ERR_INVALID_KEY;            
        }
        if ((pstMach->stPadAlg.ePadMethod == E_RSA_PAD_AUTOD1PKCS ||
            pstMach->stPadAlg.ePadMethod == E_RSA_PAD_AUTOD1BRSA ))
        { 
            if (uiHashLen > pstMach->uiObs) 
            {
                WE_UINT32    uiUbLen=0;                       
                WE_UINT8     *pucUHash = NULL; 
                WE_UINT8     *pucPTmp = NULL;   
                WE_UINT8     *pucTmp=NULL;   

                pucUHash = WE_MALLOC(uiHashLen);
                pucPTmp = WE_MALLOC(pstMach->uiPbs);
                if(!pucUHash||!pucPTmp)
                {
                    if(pucUHash)
                    {
                       WE_FREE(pucUHash);
                       pucUHash=NULL;
                    }
                    if(pucPTmp)
                    {
                       WE_FREE(pucPTmp);
                       pucPTmp=NULL;
                    }
                    return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
                }
                (WE_VOID)WE_MEMSET(pucUHash,0,uiHashLen);  
                (WE_VOID)WE_MEMSET(pucPTmp,0,pstMach->uiPbs);
                iAutoSucceed = 1;
                uiUnPadLen = 0;
                
                for (pucTmp = pucHash; pucTmp < pucHash+uiHashLen; pucTmp += pstMach->uiPbs) 
                {
                    (WE_VOID)WE_MEMCPY(pucPTmp, pucTmp, pstMach->uiPbs);
                    if (We_MbkPadPkcs1Unpad(pucPTmp, &uiUbLen, pstMach->uiPbs, &pstMach->stPadParam)) 
                    {
                        iAutoSucceed = 0;
                        break;
                    } 
                    else 
                    { 
                        (WE_VOID)WE_MEMCPY(pucUHash+uiUnPadLen, pucPTmp, uiUbLen);
                        uiUnPadLen += uiUbLen;
                    }
                }
                if (iAutoSucceed) 
                {  
                    (WE_VOID)WE_MEMCPY(pucHash, pucUHash, uiUnPadLen);
                }
                WE_FREE(pucUHash);
                pucUHash=NULL;
                WE_FREE(pucPTmp);
                pucPTmp=NULL;
            }
        } 
        if (!(iAutoSucceed)) 
        {
            if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1)
            {
                iRes=We_MbkPadPkcs1Unpad(pucHash+uiHashLen-pstMach->uiPbs, &uiUnPadLen, 
                    pstMach->uiPbs, &pstMach->stPadParam);
            }
            else if(pstMach->stPadAlg.ePadMethod==E_RSA_PAD_PKCS1NULL)
            {
                iRes=We_MbkPadPkcs1Unpad(pucHash+uiHashLen-pstMach->uiPbs, &uiUnPadLen, 
                    pstMach->uiPbs, &pstMach->stPadParam);
            }
            else
            {
                /*unpadding failed*/
                M_WE_RSA_SELF_FREE(pucHash);
                if(stHash.pucData)
                {
                    WE_FREE(stHash.pucData);
                    stHash.pucData=NULL;
                }
                if(eHashType == E_HASH_NUMERIC)
                {
                    We_MpmKillArc(stHashDig.pvNum);
                }
                else
                {
                    M_WE_RSA_SELF_FREE(stHashDig.stHashValue.pucData);
                }
                return M_WE_LIB_ERR_GENERAL_ERROR;      
            }
            if (iRes) 
            {
                /*unpadding failed*/
                M_WE_RSA_SELF_FREE(pucHash);
                if(stHash.pucData)
                {
                    WE_FREE(stHash.pucData);
                    stHash.pucData=NULL;
                }
                if(eHashType == E_HASH_NUMERIC)
                {
                    We_MpmKillArc(stHashDig.pvNum);
                }
                else
                {
                    M_WE_RSA_SELF_FREE(stHashDig.stHashValue.pucData);
                }
                return M_WE_LIB_ERR_GENERAL_ERROR;                
            }
        }    
    }  
    if(eHashType == E_HASH_NUMERIC) 
    { 
        pucDe = We_MpmBerInteger2Arc(pucHash, &pvHsh);                
        if(!pucDe) 
        {
            /*couldn't decode message digest (""integer)*/
            M_WE_RSA_SELF_FREE(pucHash);
            if(stHash.pucData)
            {
                WE_FREE(stHash.pucData);
                stHash.pucData=NULL;
            }
            We_MpmKillArc(stHashDig.pvNum);
            
            return M_WE_LIB_ERR_GENERAL_ERROR;            
        }
        iRes = We_MpmCmpArc(stHashDig.pvNum, pvHsh);
        We_MpmKillArc(pvHsh);
    } 
    else 
    {
        uiPtl = (WE_UINT32) (stHashDig.stHashValue.uiSize >> 3);      
              
        iRes = uiUnPadLen >= uiPtl ? WE_MEMCMP(stHashDig.stHashValue.pucData,pucHash+uiUnPadLen-uiPtl, uiPtl) : 1;
    }          
    M_WE_RSA_SELF_FREE(pucHash);
    if(stHash.pucData)
    {
        WE_FREE(stHash.pucData);
        stHash.pucData=NULL;
    }
    if(eHashType == E_HASH_NUMERIC)
    {
        We_MpmKillArc(stHashDig.pvNum);
    }
    else
    {
        M_WE_RSA_SELF_FREE(stHashDig.stHashValue.pucData);
    }
    return iRes;
}
/*==================================================================================================
FUNCTION: 
    We_RsaDisposeMblkList
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    verify signature finish.
ARGUMENTS PASSED:
    pstUtMblkList pstList[IN]:
RETURN VALUE:
    error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID We_RsaDisposeMblkList(pstUtMblkList pstList)
{
    pstUtMblkList pstCur=NULL;
    
    if (pstList && pstList->pstPrev)
    {
        pstList->pstPrev->pstNext = 0;
    }
    
    for(pstCur = pstList; pstCur; pstCur = pstList) 
    {
        pstList = pstCur->pstNext;
        if (pstCur->stBlock.pucData)
        {
            if(pstCur->stBlock.pucData)
            {
                WE_FREE(pstCur->stBlock.pucData);
                pstCur->stBlock.pucData=NULL;
            }
        }
        if(pstCur)
        {
            WE_FREE(pstCur);
            pstCur=NULL;
        }
    }
}



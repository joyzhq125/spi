/*==================================================================================================
    HEADER NAME : mbk2ostr.c
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
    2006-11-27   tang            None         Draft
==================================================================================================*/
/***************************************************************************************
*   Include File Section
****************************************************************************************/
#include "sec_comm.h"
//#include "we_rsacomm.h"

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

static WE_VOID  We_MbkPush(WE_UINT8 *pucDestEnd, WE_UINT8 *pucSrcEnd, WE_UINT32 uiLen);
static WE_UINT8 *We_MbkIsCstSig (WE_UINT8 *pucData, WE_UINT32 *puiDataLen);
static WE_UINT8 *We_MbkOldBerkey2key(WE_UINT8 *pucSrc, St_RsaKey **ppstKey);
static WE_UINT8 *We_MbkPkcs1Berkey2key(WE_UINT8 *pucSrc, St_RsaKey **ppstKey);
static WE_UINT8 *We_MbkGetRemainingFields(WE_UINT8 *pucSrc, St_RsaKey **ppstKey, 
                                          WE_UINT8 *pucEnd, WE_VOID **ppvMod,
                                          WE_VOID * *ppvExp, WE_VOID * *ppvPubExp);
static WE_UINT8 *We_MbkCheckForFermat(WE_UINT8 *pucSrc, St_RsaKey **ppstKey, 
                                      WE_UINT8 *pucEnd, WE_VOID * *ppvMod, 
                                      WE_VOID * *ppvExp);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1SizeOfMblk2BerOctetString
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_UtMblk stMem[IN]:
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
WE_LONG We_MbkAsn1SizeOfMblk2BerOctetString(St_UtMblk stMem)
{
    return We_MbkAsn1SizeOfType2BerTag(ASN1_OCTETSTRING)
        + We_MbkAsn1SizeOfSize2BerLen((WE_LONG)(stMem.uiUsed)) + (WE_LONG)(stMem.uiUsed);
}
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1Mblk2BerOctetString
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_UtMblk stMem[IN]:
    WE_UINT8 *pucDest[IN]:
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
WE_UINT8  *We_MbkAsn1Mblk2BerOctetString(St_UtMblk stMem, WE_UINT8 *pucDest)
{
    pucDest = We_MbkAsn1Type2BerTag(ASN1_OCTETSTRING, pucDest);
    pucDest = We_MbkAsn1Size2BerLen((WE_LONG)stMem.uiUsed, pucDest);
    (WE_VOID)WE_MEMCPY(pucDest, stMem.pucData, stMem.uiUsed);    
    return pucDest + stMem.uiUsed;   
}
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1SizeOfSize2BerLen
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_LONG iSize[IN]:
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
WE_LONG We_MbkAsn1SizeOfSize2BerLen(WE_LONG iSize)
{
    if (iSize <= 127)
    {
        return 1;
    }
    else if (iSize <= 255)
    {
        return 2;
    }
    else if (iSize <= 65535)
    {
        return 3;
    }
    else if (iSize <= 0xffffffL)
    {
        return 4;
    }
    else
    {
        return 5;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1Size2BerLen
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_LONG iSize[IN]:
    WE_UINT8 *pucDest[IN]:
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
WE_UINT8  *We_MbkAsn1Size2BerLen(WE_LONG iSize, WE_UINT8 *pucDest)
{
    WE_UINT32 uiSize = 0;
    if(!pucDest||iSize < 0)
    {
        return NULL;
    }
    uiSize = (WE_UINT32)iSize;
    
    if(iSize >(WE_LONG)0xffffffffL)
    {
        /*ASN.1 size argument (%u) too large*/
//        return NULL;
    }
    if(uiSize <= 127)
    {
        *pucDest++ = (WE_UINT8) uiSize;
    }
    else if(uiSize <= 255) 
    {
        *pucDest++ = 129;
        *pucDest++ = (WE_UINT8) uiSize;
    } 
    else if(uiSize <= 65535U) 
    {
        *pucDest++ = 130;
        *pucDest++ = (WE_UINT8) (uiSize >> 8);
        *pucDest++ = (WE_UINT8) uiSize;
    } 
    else if(uiSize <= 0xffffffUL) 
    {
        *pucDest++ = 131;
        *pucDest++ = (WE_UINT8) (uiSize >> 16);
        *pucDest++ = (WE_UINT8) (uiSize >> 8);
        *pucDest++ = (WE_UINT8) uiSize;
    } 
    else 
    {
        *pucDest++ = 132;
        *pucDest++ = (WE_UINT8) (uiSize >> 24);
        *pucDest++ = (WE_UINT8) (uiSize >> 16);
        *pucDest++ = (WE_UINT8) (uiSize >> 8);
        *pucDest++ = (WE_UINT8) uiSize;
    }
    
    return pucDest;
}
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1SizeOfType2BerTag
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32 uiT[IN]
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
WE_LONG We_MbkAsn1SizeOfType2BerTag(WE_UINT32 uiT)
{
    WE_INT32 iNum=0;
    uiT = ASN_TVAL(uiT);
    if(uiT < 0x1f)
    {
        return 1;
    }
    for(iNum = 0; uiT; uiT>>=1)
    {
        iNum++;
    }
    return ((iNum+6) / 7) + 1;
}
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1Type2BerTag
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32 uiT[IN]:
    WE_UINT8 *pucD[IN]:
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
WE_UINT8 * We_MbkAsn1Type2BerTag(WE_UINT32 uiT, WE_UINT8 *pucD)
{
    WE_LONG iNum = 1;
    WE_LONG iK = 2;
    
    if(!pucD)
    {
        return NULL;
    }    
    
    *pucD = (WE_UINT8) (uiT >> 24) & 0xe0;
    uiT = ASN_TVAL(uiT);
    
    if(uiT < 0x1f) 
    {
        *pucD |= (WE_UINT8)uiT;
    }
    else 
    {
        *pucD |= 0x1f;
        pucD++;
        iNum = We_MbkAsn1SizeOfType2BerTag(uiT) - 1;
        
        *(pucD + iNum - 1) = (WE_UINT8)uiT & 0x7f;
        uiT >>= 7;
        while(uiT) 
        {
            *(pucD + iNum - iK++) = (WE_UINT8) (uiT & 0x7f) | 0x80;
            uiT >>= 7;
        }
    }
    return pucD + iNum;
}
/*==================================================================================================
FUNCTION: 
    We_MbkPush
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucDestEnd[OUT]:
    WE_UINT8 *pucSrcEnd[IN]:
    WE_UINT32 uiLen[IN]:
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
static WE_VOID We_MbkPush(WE_UINT8 *pucDestEnd, WE_UINT8 *pucSrcEnd, WE_UINT32 uiLen)
{
    if(!pucDestEnd||!pucSrcEnd)
    {
        return ;
    }  
    while(uiLen--)
    {
        *--pucDestEnd = *--pucSrcEnd;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MbkPadPkcs1NullPad
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucData[IN]:
    WE_UINT32 uiUnpadded[IN]:
    WE_UINT32 uiPadded[IN]:
    St_PadParams *pstPpar[OUT]:
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
WE_INT32 We_MbkPadPkcs1NullPad(WE_HANDLE hWeHandle,WE_UINT8 *pucData, WE_UINT32 uiUnpadded, 
                               WE_UINT32 uiPadded, St_PadParams *pstPpar)
{
    WE_INT32           iSKey = 0;
    if(!pucData||!pstPpar)
    {
        return -1;
    }    
    iSKey = pstPpar->eKeyType == E_PAD_PUBLIC ? 0 : 1;
    if(uiPadded < We_MbkPadPkcs1NullPadSize(pstPpar) || 
        uiUnpadded > uiPadded - We_MbkPadPkcs1NullPadSize(pstPpar)) 
    {
        /* "no space in which to perform padding"*/
        return -1;
    }
    
    if(!iSKey)
    {
        return We_MbkPadPkcs1Pad(hWeHandle,pucData, uiUnpadded, uiPadded, pstPpar);
    }
    We_MbkPush(pucData + uiPadded, pucData + uiUnpadded, uiUnpadded);
    (WE_VOID)WE_MEMSET(pucData, 0, uiPadded - uiUnpadded);
    
    return 0;
}
/*==================================================================================================
FUNCTION: 
    We_MbkPadPkcs1Pad
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucData[IN]:
    WE_UINT32 uiUnpadded[IN]:
    WE_UINT32 uiPadded[IN]: 
    St_PadParams *pstPpar[OUT]:
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
WE_INT32 We_MbkPadPkcs1Pad(WE_HANDLE hWeHandle,WE_UINT8 *pucData, WE_UINT32 uiUnpadded, 
                           WE_UINT32 uiPadded, St_PadParams *pstPpar)
{
    WE_UINT32        uiPadding = 0;
    WE_INT32         iSKey = 0;
    WE_INT32         iResult=0;
    
    if(!pucData||!pstPpar)
    {
       return -1;
    }   

    uiPadding = uiPadded - uiUnpadded - 2;
    iSKey = pstPpar->eKeyType == E_PAD_PUBLIC ? 0 : 1;
    if(uiPadded < We_MbkPadPkcs1PadSize(pstPpar) || 
        uiUnpadded > uiPadded - We_MbkPadPkcs1PadSize(pstPpar)) 
    {
        /*no space in which to perform padding*/
        return -1;
    }
    
    We_MbkPush(pucData + uiPadded, pucData + uiUnpadded, uiUnpadded);
    if(iSKey)  
    {
        *pucData++ = 1;
        (WE_VOID)WE_MEMSET(pucData, 255, uiPadding);
        pucData[uiPadding] = 0;
    } 
    else 
    {
        *pucData++ = 2;
        /*modified by tang 1206*/
#ifdef WE_RSA_FOR_SEC
        iResult=Sec_LibGenerateRandom(hWeHandle,(WE_CHAR *)pucData,(WE_INT32)uiPadding);
#else
        iResult=We_LibGenerateRandom(hWeHandle,pucData,(WE_INT32)uiPadding);         
#endif
        if(iResult)
        {
            return -1;
        }
        while(uiPadding--) 
        {
            while(!*pucData)
            {
                #ifdef WE_RSA_FOR_SEC
                iResult=Sec_LibGenerateRandom(hWeHandle,(WE_CHAR *)pucData,1);
                #else
                iResult=We_LibGenerateRandom(hWeHandle,pucData,1);
                #endif                
                if(iResult)
                {
                    return -1;
                } 
            }
            pucData++;            
        }
        *pucData = 0;
    }    
    return 0;
}
/*==================================================================================================
FUNCTION: 
    We_MbkPadPkcs1Unpad
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucData[IN]:
    WE_UINT32 *puiUnpadded[OUT]:
    WE_UINT32 uiPadded[IN]:
    St_PadParams *pstPpar[OUT]:
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
WE_INT32 We_MbkPadPkcs1Unpad(WE_UINT8 *pucData, WE_UINT32 *puiUnpadded, WE_UINT32 uiPadded, St_PadParams *pstPpar)
{
    WE_INT32  iRval = -1;
    WE_UINT8  *pucStart = NULL;
    WE_INT32  iSKey =0;
    
    if(!pucData||!puiUnpadded||!pstPpar)
    {
       return -1;
    }    
    
    pucStart = pucData;
    iSKey = pstPpar->eKeyType == E_PAD_PUBLIC ? 0 : 1;
    uiPadded--;
    switch(*pucStart++) 
    {
    case 0:
        if(!iSKey) 
        {
            while(uiPadded && !*pucStart)
            {
                uiPadded--, pucStart++;
            }
            iRval = 0;
        }
        break;
        
    case 1:
        if(!iSKey)
        {
            while(uiPadded && *pucStart == 255)
            {
                uiPadded--, pucStart++;
            }
            if(!*pucStart++)
            {
                uiPadded--, iRval = 0;
            }
        }
        break;
        
    case 2:
        if(iSKey)
        {
            while(uiPadded && *pucStart)
            {
                uiPadded--, pucStart++;
            }
            if(!*pucStart++)
            {
                uiPadded--, iRval = 0;
            }
        }
        break;
     default:
        break ;
    }    
    if(!iRval) 
    { 
        *puiUnpadded = uiPadded;
        while(uiPadded--)
        {
            *pucData++ = *pucStart++;
        }
    }    
    return iRval;
}
/*==================================================================================================
FUNCTION: 
    We_MbkPadPkcs1PadSize
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get pad pkcs1 pad size.
ARGUMENTS PASSED:
    St_PadParams *pstDum[IN]:
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
WE_UINT32 We_MbkPadPkcs1PadSize(St_PadParams *pstDum)
{
    pstDum=pstDum;    
    return 10;
}
/*==================================================================================================
FUNCTION: 
    We_MbkPadPkcs1NullPadSize
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    get null pad pkcs1 pad size.
ARGUMENTS PASSED:
    St_PadParams *pstDum[IN]:
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
WE_UINT32 We_MbkPadPkcs1NullPadSize(St_PadParams *pstDum)
{
    pstDum=pstDum;    
    return 3;
}
/*==================================================================================================
FUNCTION: 
    We_MbkCstSig2Sig
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_UtMblk *pstSigIn[IN]:
    St_UtMblk *pstSigOut[OUT]:
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
WE_INT32 We_MbkCstSig2Sig (St_UtMblk *pstSigIn, St_UtMblk *pstSigOut)
{
    WE_UINT8    *pucTmp=NULL;
    
    if(!pstSigIn||!pstSigOut)
    {
        return -1;
    }    
    
    pucTmp = We_MbkIsCstSig(pstSigIn->pucData, &(pstSigIn->uiUsed));
    if(pucTmp) 
    {
        pstSigOut->uiUsed = pstSigOut->uiSize =(WE_UINT32)((pstSigIn->pucData + pstSigIn->uiUsed) - pucTmp);
        pstSigOut->pucData = (WE_UINT8 *)WE_MALLOC(pstSigOut->uiSize);
        if (! pstSigOut->pucData) 
        {
            pstSigOut->uiSize = pstSigOut->uiUsed = 0;
            return -1;
        }
        (WE_VOID)WE_MEMSET(pstSigOut->pucData,0,pstSigOut->uiSize);
        
        (WE_VOID)WE_MEMCPY (pstSigOut->pucData, pucTmp, pstSigOut->uiSize);
        return 0;
    }
    
    pstSigOut ->pucData = (WE_UINT8 *)WE_MALLOC(pstSigIn->uiUsed);
    if (!(pstSigOut ->pucData)) 
    {
        pstSigOut->uiSize = pstSigOut->uiUsed = 0;
        return -1;
    }
    (WE_VOID)WE_MEMSET(pstSigOut ->pucData,0,pstSigIn->uiUsed);
    pstSigOut->uiSize = pstSigOut->uiUsed = pstSigIn->uiUsed;
    (WE_VOID)WE_MEMCPY(pstSigOut->pucData, pstSigIn->pucData, pstSigIn->uiUsed);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    We_MbkIsCstSig
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucData[IN]:
    WE_UINT32 *puiDataLen[IN]:
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
static WE_UINT8 *We_MbkIsCstSig (WE_UINT8 *pucData, WE_UINT32 *puiDataLen)
{
    WE_UINT32  uiAsn1Type=0;
    WE_UINT8   *pucSrc = NULL;
    WE_LONG    uiTmpOff= 0;

    if(!pucData||!puiDataLen)
    {
    	return NULL;
    }    
    pucSrc = pucData;
    
    pucSrc = We_MbkAsn1BerTag2Type(pucSrc, &uiAsn1Type);
    if(!pucSrc)
    {
        return NULL;
    }
    if(uiAsn1Type != ASN1_BITSTRING)
    {
        return NULL;
    }
    pucSrc = WE_MBK_ASN1_BERLEN2SIZE(pucSrc, &uiTmpOff);
    if(!pucSrc)
    {
        return NULL;
    }
    if(*puiDataLen && (pucSrc + uiTmpOff != pucData + *puiDataLen))
    {
        return NULL;
    }
    
    if(*pucSrc++ != 0)
    {
        return NULL;
    }
    if(!(*puiDataLen))
    {
        *puiDataLen = (WE_UINT32)((pucSrc-pucData) + uiTmpOff - 1);
    }
    return pucSrc;
}
/*==================================================================================================
FUNCTION: 
    We_MbkRsaBerkey2key
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucSrc[IN]:
    St_RsaKey **ppstKey[IN]:
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
WE_UINT8 *We_MbkRsaBerkey2key(WE_UINT8 *pucSrc, St_RsaKey **ppstKey)
{
    WE_UINT32  uiType=0;
    
    pucSrc = We_MbkAsn1BerTag2Type(pucSrc, &uiType);
    if(!pucSrc)
    {
        return NULL;
    }
    if(uiType == ASN1_BITSTRING) 
    {
        return We_MbkOldBerkey2key(pucSrc, ppstKey);
    }
    else if (uiType == ASN1_SEQUENCE)
    {
        return We_MbkPkcs1Berkey2key(pucSrc, ppstKey);
    }
    else 
    {
        return NULL;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MbkOldBerkey2key
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucSrc[IN]:
    St_RsaKey **ppstKey[IN]:
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
static WE_UINT8 *We_MbkOldBerkey2key(WE_UINT8 *pucSrc, St_RsaKey **ppstKey)
{
    WE_UINT32    uiType=0;
    WE_LONG      iSize=0;
    WE_LONG      iSeqs=0;
    WE_VOID      *pvMod=NULL;
    WE_VOID      *pvExp=NULL;
    WE_VOID      *pvPubExp=NULL;
    WE_UINT8     *pucEnd=NULL;
    WE_UINT8     *pucSend=NULL;

    if(!pucSrc)
    {
        return NULL;
    }
    
    pucSrc = WE_MBK_ASN1_BERLEN2SIZE(pucSrc, &iSeqs);
    if(!pucSrc)
    {
        return NULL;
    }
    pucSend = pucSrc + iSeqs;
    if(*pucSrc++ != 0) 
    {
        /*bit string with non-full*/
        return NULL;
    }
    
    pucSrc = We_MbkAsn1BerTag2Type(pucSrc, &uiType);
    if(!pucSrc)
    {
        return NULL;
    }
    if(uiType != ASN1_SEQUENCE) 
    {
        /*expected sequence*/
        return NULL;
    }
    
    pucSrc = WE_MBK_ASN1_BERLEN2SIZE(pucSrc, &iSize);    
    if(!pucSrc)
    {
        return NULL;
    }
    pucEnd = pucSrc + iSize;
    if(pucEnd != pucSend) 
    {
        /*bitstring pucEnd and sequence pucEnd do not coincide*/
        return NULL;
    }
    pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvMod);
    if(!pucSrc)
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        return NULL;
    }
    pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvExp);
    if(!pucSrc)
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        We_MpmKillArc(pvMod);        
        return NULL;
    }
    
    if(pucSrc < pucEnd) 
    {  
        pvPubExp = We_MpmLoadArc(E_MPM_POS, 1, (WE_UINT8 *)"\0");
        return We_MbkGetRemainingFields(pucSrc, ppstKey, pucEnd, &pvMod, &pvExp, &pvPubExp);
    } 
    else
    {
        return We_MbkCheckForFermat(pucSrc, ppstKey, pucEnd, &pvMod, &pvExp);
    }
}
/*==================================================================================================
FUNCTION: 
    We_MbkPkcs1Berkey2key
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucSrc[IN]:
    St_RsaKey **ppstKey[IN]:
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
static WE_UINT8 *We_MbkPkcs1Berkey2key(WE_UINT8 *pucSrc, St_RsaKey **ppstKey)
{
    WE_LONG    iSize=0;
    WE_VOID    *pvMod=NULL;
    WE_VOID    *pvExp=NULL;
    WE_VOID    *pvPubExp=NULL;
    WE_UINT8   *pucEnd=NULL;

    if(!pucSrc)
    {
    	return NULL;
    }    
    pucSrc = WE_MBK_ASN1_BERLEN2SIZE(pucSrc, &iSize);
    if(!pucSrc)
    {
        return NULL;
    }
    pucEnd = pucSrc + iSize;
    
    if((*pucSrc) == 2 && (*(pucSrc+1) == 1) && (*(pucSrc+2) == 0)) 
    {  
        pucSrc += 3;
        pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvMod);
        if(!pucSrc)
        {
            return NULL;
        }
        pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvPubExp);
        if(!pucSrc)
        {
            We_MpmKillArc(pvMod);
            return NULL;
        }
        pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvExp);
        if(!pucSrc)
        {
            We_MpmKillArc(pvPubExp);
            We_MpmKillArc(pvMod);
            return NULL;
        }
        return We_MbkGetRemainingFields(pucSrc, ppstKey, pucEnd, &pvMod, &pvExp, &pvPubExp);        
    }     
    else
    { 
    	pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvMod);
        if(!pucSrc)
        {
            return NULL;
        }
        pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvExp);
        if(!pucSrc)
        {
            We_MpmKillArc(pvMod);            
            return NULL;
        }
        return We_MbkCheckForFermat(pucSrc, ppstKey, pucEnd, &pvMod, &pvExp);    
        
    }
}
/*==================================================================================================
FUNCTION: 
    We_MbkGetRemainingFields
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    E_UINT8 *pucSrc[IN]:
    St_RsaKey **ppstKey[IN]:
    WE_UINT8 *pucEnd[IN]:
    WE_VOID * *ppvMod[IN]:
    WE_VOID * *ppvExp[IN]:
    WE_VOID * *ppvPubExp[IN]:
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
static WE_UINT8 *We_MbkGetRemainingFields(WE_UINT8 *pucSrc, St_RsaKey **ppstKey, WE_UINT8 *pucEnd, WE_VOID * *ppvMod,
                                          WE_VOID * *ppvExp, WE_VOID * *ppvPubExp)
{
    WE_VOID        *pvP=NULL;
    WE_VOID        *pvQ=NULL;
    WE_VOID        *pvPex=NULL;
    WE_VOID        *pvQex=NULL;
    WE_VOID        *pvCc=NULL;
    St_RsaKey      *pstKey=NULL;
    struct tagSt_CrtKey *pstSk=NULL;
    
    if(!pucSrc||!ppstKey||!pucEnd||!ppvMod||!ppvExp||!ppvPubExp)
    {
        return NULL;
    }    
    
    pstKey = (St_RsaKey *)WE_MALLOC(sizeof(*pstKey));
    if(!pstKey)
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        We_MpmKillArc(*ppvPubExp);
        We_MpmKillArc(*ppvExp);
        We_MpmKillArc(*ppvMod);
        return NULL;
    }
    (WE_VOID)WE_MEMSET(pstKey,0,sizeof(*pstKey));
    pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvP);
    if(!pucSrc)
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        WE_FREE(pstKey);
        pstKey=NULL;
        We_MpmKillArc(*ppvPubExp);
        We_MpmKillArc(*ppvExp);
        We_MpmKillArc(*ppvMod);
        return NULL;
    }
    pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvQ);
    if(!pucSrc)
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        We_MpmKillArc(pvP);
        WE_FREE(pstKey);
        pstKey=NULL;
        We_MpmKillArc(*ppvPubExp);
        We_MpmKillArc(*ppvExp);
        We_MpmKillArc(*ppvMod);
        return NULL;
    }
    pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvPex);
    if(!pucSrc)
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        We_MpmKillArc(pvQ);
        We_MpmKillArc(pvP);
        WE_FREE(pstKey);
        pstKey=NULL;
        We_MpmKillArc(*ppvPubExp);
        We_MpmKillArc(*ppvExp);
        We_MpmKillArc(*ppvMod);
        return NULL;
    }
    pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvQex);
    if(!pucSrc )
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        We_MpmKillArc(pvPex);
        We_MpmKillArc(pvQ);
        We_MpmKillArc(pvP);
        WE_FREE(pstKey);
        pstKey=NULL;
        We_MpmKillArc(*ppvPubExp);
        We_MpmKillArc(*ppvExp);
        We_MpmKillArc(*ppvMod);
        return NULL;
    }
    pucSrc = We_MpmBerInteger2Arc(pucSrc, &pvCc);
    if(!pucSrc )
    {
        /*internal failure in We_MpmBerInteger2Arc*/
        We_MpmKillArc(pvQex);
        
        We_MpmKillArc(pvPex);
        We_MpmKillArc(pvQ);
        We_MpmKillArc(pvP);
        WE_FREE(pstKey);
        pstKey=NULL;
        We_MpmKillArc(*ppvPubExp);
        We_MpmKillArc(*ppvExp);
        We_MpmKillArc(*ppvMod);
        return NULL;
    }
    if(pucSrc != pucEnd) 
    {
        /*unexpected trailing pucData in key*/
        We_MpmKillArc(pvQex);
        
        We_MpmKillArc(pvPex);
        We_MpmKillArc(pvQ);
        We_MpmKillArc(pvP);
        WE_FREE(pstKey);
        pstKey=NULL;
        We_MpmKillArc(*ppvPubExp);
        We_MpmKillArc(*ppvExp);
        We_MpmKillArc(*ppvMod);
        return NULL;
    }
    
    pstKey->uiBits = (We_MpmBitLenArc(*ppvMod));
    pstKey->eKeyTypeTag = E_RSA_KTAG_CRT_KEY;
    pstSk = &(pstKey->stKeyTypeUnion.stCrtKey);
    pstSk->pvModulus = *ppvMod;
    pstSk->pvExpon = *ppvExp;
    pstSk->pub_expon = *ppvPubExp;
    pstSk->pvPFactor = pvP;
    pstSk->pvQFactor = pvQ;
    pstSk->pvPExpon = pvPex;
    pstSk->pvQExpon = pvQex;
    pstSk->pvConvConst = pvCc;
    *ppstKey = pstKey;    
    return pucEnd;    
}
/*==================================================================================================
FUNCTION: 
    We_MbkCheckForFermat
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucSrc[IN]:
    St_RsaKey **ppstKey[IN]:
    WE_UINT8 *pucEnd[IN]:
    WE_VOID * *ppvMod[IN]:
    WE_VOID * *ppvExp[IN]:
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
static WE_UINT8 *We_MbkCheckForFermat(WE_UINT8 *pucSrc, St_RsaKey **ppstKey, WE_UINT8 *pucEnd, 
                                      WE_VOID * *ppvMod, WE_VOID * *ppvExp)
{
    St_RsaKey     *pstKey=NULL;
    WE_UINT32     uiFermat = 0;
    WE_UINT32     uiEBits=0;

    if(!pucSrc||!ppstKey||!pucEnd||!ppvMod||!ppvExp)
    {
       return NULL;
    }    
    if(pucSrc != pucEnd) 
    {
        /* unexpected trailing pucData in pstKey*/
        We_MpmKillArc(*ppvMod);
        We_MpmKillArc(*ppvExp);
        return NULL;
    }
    pstKey = (St_RsaKey *)WE_MALLOC(sizeof(*pstKey));
    if(!pstKey)
    {
        /*out of memory*/
        We_MpmKillArc(*ppvMod);
        We_MpmKillArc(*ppvExp);
        return NULL;
    }
    (WE_VOID)WE_MEMSET(pstKey,0,sizeof(*pstKey));
    uiEBits = We_MpmBitLenArc(*ppvExp) - 1;
    if(!(uiEBits & (uiEBits - 1))) 
    {
        WE_VOID * pvMach = We_MpmNewMach();
        
        if(pvMach) 
        {
            WE_VOID * pvTmp = 0;            
            We_Mpm2Power(pvMach, E_MPM_R0, uiEBits);
            We_MpmLToMpm(pvMach, E_MPM_R1, 1L);
            if(!We_MpmFlagSet(pvMach, E_MPM_OVERFLOW)) 
            {
                We_MpmAdd(pvMach, E_MPM_R0, E_MPM_R0, E_MPM_R1);
                if(!We_MpmFlagSet(pvMach, E_MPM_OVERFLOW))
                {
                    pvTmp = We_MpmBackUp(pvMach, E_MPM_R0, E_FALSE);
                }
            }
            if(!pvTmp) 
            {
                pucEnd = NULL;
                /*format failed in mpm*/
                We_MpmDisposeMach(pvMach);
                WE_FREE(pstKey);
                pstKey=NULL;
                return NULL;
            }
            if(!We_MpmCmpArc(pvTmp, *ppvExp))
            {
                for(uiFermat = 1; uiEBits >> uiFermat; uiFermat++)
                {
                }
            }   
            We_MpmKillArc(pvTmp);          
            We_MpmDisposeMach(pvMach);
        } 
        else
        {
            pucEnd =NULL;
        }
    }
    
    if(pucEnd) 
    {
        struct tagSt_SimpleKey *pk = &(pstKey->stKeyTypeUnion.stSimpleKey);        
        pstKey->uiBits = (We_MpmBitLenArc(*ppvMod));
        pstKey->eKeyTypeTag = E_RSA_KTAG_SIMPLE_KEY;
        pk->pvModulus = *ppvMod;
        pk->pvExpon = *ppvExp;
        pk->uiFermatNo = uiFermat;
        *ppstKey = pstKey;
        return pucEnd;
    } 
    We_MpmKillArc(*ppvMod);
    We_MpmKillArc(*ppvExp);
    WE_FREE(pstKey);
    pstKey=NULL;
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1InnerBerLen2Size
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucSrc[IN]:
    WE_LONG *piSize[IN]:
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
WE_UINT8  *We_MbkAsn1InnerBerLen2Size(WE_UINT8 *pucSrc, WE_LONG *piSize)
{
    WE_UINT8 ucFirst = 0;   
    ucFirst = *pucSrc++;
    if(ucFirst < 128)
    {
        *piSize = (WE_LONG) ucFirst;
    }
    else 
    {
        WE_ULONG uiResult = 0;
        WE_ULONG lOld=0;        
        ucFirst -= 128;
        while(ucFirst--) 
        {
            lOld = uiResult;
            uiResult = (uiResult << 8) + (WE_ULONG) *pucSrc++;
            if(uiResult >> 8 != lOld)
            {            
                return NULL;
            }
        }
        *piSize = (WE_INT32)uiResult;
    }    
    return pucSrc;
}
/*==================================================================================================
FUNCTION: 
    We_MbkAsn1BerTag2Type
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucS[IN]:
    WE_UINT32 *puiT[IN]:
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
WE_UINT8 *We_MbkAsn1BerTag2Type(WE_UINT8 *pucS, WE_UINT32 *puiT)
{
    WE_UINT8 ucTyp=0;
    
    if(!pucS||!puiT)
    {
        return NULL;
    }    
    *puiT = 0;
    ucTyp = *pucS & 0xe0;
    if((*pucS & 0x1f) != 0x1f) 
    {
        *puiT = *pucS & 0x1f;
    }
    else 
    {
        do {
            pucS++;
            *puiT = (*puiT << 7) | (*pucS & 0x7f);
        } while(*pucS & 0x80);
    }
    *puiT |= ucTyp << 24;
    if(ASN_IS_UNIV(*puiT)
        && (!ASN_TVAL(*puiT)
        || ASN_TVAL(*puiT) > ASN1_BMPSTRING
        || !(ASN1_VALID & (1 << (ASN_TVAL(*puiT) - 1)))
        || (ASN_TVAL(*puiT) == ASN1_SEQUENCE_PR && ASN_IS_PRIM(*puiT))
        || (ASN_TVAL(*puiT) == ASN1_SET_PR && ASN_IS_PRIM(*puiT)))) 
    {
        return NULL;
    }
    return pucS + 1;
}
/*==================================================================================================
FUNCTION: 
    We_MbkRegNorm
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstReg[IN]:
    WE_UINT32 uiPrec[IN]:
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
WE_VOID We_MbkRegNorm(St_Reg * pstReg, WE_UINT32 uiPrec)
{
    if(!pstReg)
    {
        return ;
    }    
    We_MpaNorm(&pstReg->stNormVal, uiPrec);
}
/*==================================================================================================
FUNCTION: 
    We_MbkRegNorm
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstReg[OUT]:
    WE_UINT32  uiPrec[IN]:
    E_Result ePreserve[IN]:
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
E_Result We_MbkRegPrep(St_Reg * pstReg, WE_UINT32  uiPrec, E_Result ePreserve)
{
    if(!pstReg)
    {
    	return E_FALSE;
    }    
    if(pstReg->uiAllocPrec < uiPrec) 
    {
        WE_ULONG  *puiNewAlloc=NULL;
        WE_UINT32  uiBlocks=0;
        WE_UINT32  uiAllocPrec=0;  
        
        uiBlocks = (uiPrec / REG_BLOCK_SIZE) + 1;
        uiAllocPrec = uiBlocks * REG_BLOCK_SIZE;
        
        puiNewAlloc = (WE_ULONG *)WE_MALLOC(uiAllocPrec * sizeof( WE_ULONG));
        if(!puiNewAlloc) 
        { 
            return E_FALSE;
        }
        else 
        {
            (WE_VOID)WE_MEMSET(puiNewAlloc,0,uiAllocPrec * sizeof( WE_ULONG));
            if(ePreserve && REG_LOADED(*pstReg))
            {
                (WE_VOID)WE_MEMCPY(puiNewAlloc, pstReg->stNormVal.puiDig,pstReg->stNormVal.uiPrec * sizeof(WE_ULONG));
            }
            else 
            {
                pstReg->stNormVal.uiPrec = 0;
            }
            if(REG_ALLOCED(*pstReg))
            {
                M_WE_RSA_SELF_FREE(pstReg->stNormVal.puiDig);
            }
            pstReg->stNormVal.puiDig = puiNewAlloc;
            pstReg->uiAllocPrec = uiAllocPrec;
            return E_TRUE;
        }
    } 
    else
    {
        return E_TRUE;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MbkRegMpmPlaces
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstReg[IN]:
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
WE_UINT16 We_MbkRegMpmPlaces(St_Reg * pstReg)
{
    WE_UINT16 usPrec=0;
    WE_ULONG  uiMsPlace=0;     

    if(!pstReg)
    {
        return 0;
    }    
    if(REG_ZERO(*pstReg))
    {
        return 1;
    }
    else 
    {       
        usPrec = (WE_UINT16) ((pstReg->stNormVal.uiPrec - 1) * REP_RATIO + 1);
        if((WE_UINT16)(usPrec - 1) / REP_RATIO != (pstReg->stNormVal.uiPrec - 1)) 
        {
            /*Internal Function Failure*/
            return 0;
        }
        
        uiMsPlace = pstReg->stNormVal.puiDig[pstReg->stNormVal.uiPrec - 1];
        while(uiMsPlace >= BASE_MPM_DIG) 
        {
            if(++usPrec == 0) 
            {
                /*Memory Overflow in We_MbkRegMpmPlaces*/
                return 0;
            }
            uiMsPlace /= BASE_MPM_DIG;
        }
        return usPrec;
    }
}

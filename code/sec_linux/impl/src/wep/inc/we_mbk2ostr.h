/*==================================================================================================
    HEADER NAME : we_mbk2ostr.h
    MODULE NAME : WE
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file, define the initial function prototype for RC2 algorithm, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-12-5    tang            None         
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef _WE_MBK2OSTR_H 
#define _WE_MBK2OSTR_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define ASN1_UNIV       (0L)
#define ASN1_APPL       (0x40000000L)
#define ASN1_CONT       (0x80000000L)
#define ASN1_PRIV       (0xc0000000L)
#define ASN1_CONS       (0x20000000L)
  
#define ASN_TVAL(x)     ((x) & 0x1fffffffL)
#define ASN_CLASS(x)    ((x) & ASN1_PRIV)

#define ASN_IS_UNIV(x)  (!ASN_CLASS(x))
#define ASN_IS_APPL(x)  (ASN_CLASS(x) == ASN1_APPL)
#define ASN_IS_CONT(x)  (ASN_CLASS(x) == ASN1_CONT)
#define ASN_IS_PRIV(x)  (ASN_CLASS(x) == ASN1_PRIV)
#define ASN_IS_CONS(x)  ((x) & ASN1_CONS)
#define ASN_IS_PRIM(x)  (!ASN_IS_CONS(x))

#define ASN_UNIV(x)     ((x) & ~ASN1_PRIV)
#define ASN_APPL(x)     (((x) & ~ASN1_PRIV) | ASN1_APPL)
#define ASN_CONT(x)     (((x) & ~ASN1_PRIV) | ASN1_CONT)
#define ASN_PRIV(x)     ((x) | ASN1_PRIV)
#define ASN_CONS(x)     ((x) | ASN1_CONS)
#define ASN_PRIM(x)     ((x) & ~ASN1_CONS)

#define ASN1_UNKNOWN        (0x0UL)  
#define ASN1_BOOL           (0x1UL)  
#define ASN1_INTEGER        (0x2UL)  
#define ASN1_BITSTRING      (0x3UL)  
#define ASN1_OCTETSTRING    (0x4UL)  
#define ASN1_NULL           (0x5UL)  
#define ASN1_OBJECTID       (0x6UL)  
#define ASN1_OBJECTDESC     (0x7UL)  
#define ASN1_EXTERNAL       (0x8UL)    
#define ASN1_REAL           (0x9UL)  
#define ASN1_ENUMERATED     (0x0aUL)  
#define ASN1_UTF8STRING     (0x0cUL)  
#define ASN1_SEQUENCE_PR    (0x10UL)  
#define ASN1_SET_PR         (0x11UL)  
#define ASN1_NUMERIC        (0x12UL)  
#define ASN1_PRINTSTRING    (0x13UL)  
#define ASN1_T61STRING      (0x14UL)  
#define ASN1_VIDEOTEX       (0x15UL)  
#define ASN1_IA5            (0x16UL)  
#define ASN1_TIME           (0x17UL)  
 
#define ASN1_GRAPHIC        (0x19UL)  
#define ASN1_VISIBLE        (0x1aUL)  
#define ASN1_GENERALSTRING  (0x1bUL)  
#define ASN1_UNISTR         (0x1cUL)  
#define ASN1_BMPSTRING      (0x1eUL)  

#define ASN1_BAD_TAG        (~0L)      
#define ASN1_VALID          (0x2fff8bffL)     
  
#define ASN1_SEQUENCE       (ASN_CONS(ASN1_SEQUENCE_PR)) 
#define ASN1_SET            (ASN_CONS(ASN1_SET_PR))   


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
E_Result We_MbkRegPrep(St_Reg * pstReg, WE_UINT32  uiPrec, E_Result ePreserve);

WE_VOID   We_MbkRegNorm(St_Reg * pstReg, WE_UINT32 uiPrec);

WE_UINT16 We_MbkRegMpmPlaces(St_Reg * pstReg);

WE_UINT8  *We_MbkAsn1BerTag2Type(WE_UINT8 *pucS, WE_UINT32 *puiT);
WE_LONG   We_MbkAsn1SizeOfType2BerTag(WE_UINT32 uiT);
WE_UINT8  * We_MbkAsn1Type2BerTag(WE_UINT32 uiT, WE_UINT8 *pucD);

WE_LONG   We_MbkAsn1SizeOfSize2BerLen(WE_LONG iSize);

WE_UINT8  *We_MbkAsn1Size2BerLen(WE_LONG iSize, WE_UINT8 *pucDest);

#define WE_MBK_ASN1_BERLEN2SIZE(s, l) \
  ((*s == 0x80)?(*l = -1,s+1):    We_MbkAsn1InnerBerLen2Size(s, l))

WE_LONG   We_MbkAsn1SizeOfMblk2BerOctetString(St_UtMblk stMem);
WE_UINT8  *We_MbkAsn1Mblk2BerOctetString(St_UtMblk stMem, WE_UINT8 *pucDest);
WE_UINT8  *We_MbkAsn1InnerBerLen2Size(WE_UINT8 *pucSrc, WE_LONG *piSize);
WE_INT32  We_MbkCstSig2Sig (St_UtMblk *pstSigIn, St_UtMblk *pstSigOut);
WE_UINT8  *We_MbkRsaBerkey2key(WE_UINT8 *pucSrc, St_RsaKey **ppstKey);
WE_INT32 We_MbkPadPkcs1Pad(WE_HANDLE hWeHandle,WE_UINT8 *pucData, WE_UINT32 uiUnpadded, 
                           WE_UINT32 uiPadded, St_PadParams *pstPpar);

WE_INT32 We_MbkPadPkcs1Unpad(WE_UINT8 *pucData, WE_UINT32 *puiUnpadded, 
                             WE_UINT32 uiPadded, St_PadParams *pstPpar);
WE_UINT32 We_MbkPadPkcs1PadSize(St_PadParams *pstDum);

WE_INT32 We_MbkPadPkcs1NullPad(WE_HANDLE hWeHandle,WE_UINT8 *pucData, WE_UINT32 uiUnpadded, 
                               WE_UINT32 uiPadded, St_PadParams *pstPpar);

WE_UINT32 We_MbkPadPkcs1NullPadSize(St_PadParams *pstDum);


#endif /*_WE_MBK2OSTR_H*/


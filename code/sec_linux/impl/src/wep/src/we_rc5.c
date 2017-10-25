/*==================================================================================================
    HEADER NAME : we_rc5.c
    MODULE NAME : WE

    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be update later.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History

    Modification                   Tracking
    Date              Author               Number        Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-12-06   bird zhang        0.0.1            Draft
==================================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_rc5.h"


/*************************************************************************************************
*   Macro define
*************************************************************************************************/
#define M_WE_RC5_ENC               1
#define M_WE_RC5_DEC               0
#define M_WE_RC5_TYPE              0xf123f456
#define M_WE_RC5_ROUNDS            16


/*************************************************************************************************
   RC5 crypt
*************************************************************************************************/

#define M_WE_RC5_C2L(c,l)    {\
    l =  ((WE_UINT32)(*((c)++)))    ; \
    l |= ((WE_UINT32)(*((c)++))) << 8L; \
    l |= ((WE_UINT32)(*((c)++))) << 16L; \
l |= ((WE_UINT32)(*((c)++))) << 24L;}

#define M_WE_RC5_L2C(l,c)    {\
    *((c)++) = (WE_UCHAR)(((l)) & 0xff); \
    *((c)++) = (WE_UCHAR)(((l) >> 8L) & 0xff); \
    *((c)++) = (WE_UCHAR)(((l) >> 16L) & 0xff); \
    *((c)++) = (WE_UCHAR)(((l) >> 24L) & 0xff);}
             
#define M_WE_RC5_C2LN(c,l1,l2,n)    { \
            c += n; \
            l1 = 0;\
            l2 = 0; \
            switch (n) { \
                        case 8: l2 =  ((WE_UINT32)(*(--(c)))) << 24L;/*fall through*/\
                        case 7: l2 |= ((WE_UINT32)(*(--(c)))) << 16L;/*fall through*/\
                        case 6: l2 |= ((WE_UINT32)(*(--(c)))) << 8L;/*fall through*/\
                        case 5: l2 |= ((WE_UINT32)(*(--(c))));/*fall through*/     \
                        case 4: l1 =  ((WE_UINT32)(*(--(c)))) << 24L;/*fall through*/\
                        case 3: l1 |= ((WE_UINT32)(*(--(c)))) << 16L;/*fall through*/ \
                        case 2: l1 |= ((WE_UINT32)(*(--(c)))) << 8L; /*fall through*/\
                        case 1: l1 |= ((WE_UINT32)(*(--(c)))); /*fall through*/    \
                        default :/*fall through*/\
                        break;\
                        } \
            }

#define M_WE_RC5_L2CN(l1,l2,c,n)    { \
            c += n; \
            switch (n) { \
                            case 8: *(--(c)) = (WE_UCHAR)(((l2) >> 24L) & 0xff);/*fall through*/ \
                            case 7: *(--(c)) = (WE_UCHAR)(((l2) >> 16L) & 0xff);/*fall through*/ \
                            case 6: *(--(c)) = (WE_UCHAR)(((l2) >> 8L) & 0xff); /*fall through*/\
                            case 5: *(--(c)) = (WE_UCHAR)(((l2)) & 0xff); /*fall through*/\
                            case 4: *(--(c)) = (WE_UCHAR)(((l1) >> 24L) & 0xff); /*fall through*/\
                            case 3: *(--(c)) = (WE_UCHAR)(((l1) >>16L) & 0xff); /*fall through*/\
                            case 2: *(--(c)) = (WE_UCHAR)(((l1) >> 8L) & 0xff); /*fall through*/\
                            case 1: *(--(c)) = (WE_UCHAR)(((l1)) & 0xff); /*fall through*/\
                            default :/*fall through*/\
                            break;\
                        } \
            }

#define M_WE_RC5_N2LN(c,l1,l2,n)    { \
            c += n; \
            l1 = l2 = 0; \
            switch (n) { \
                            case 8: l2 =  ((WE_UINT32)(*(--(c))));/*fall through*/ \
                            case 7: l2 |= ((WE_UINT32)(*(--(c)))) << 8;/*fall through*/ \
                            case 6: l2 |= ((WE_UINT32)(*(--(c)))) << 16; /*fall through*/\
                            case 5: l2 |= ((WE_UINT32)(*(--(c)))) <<24; /*fall through*/\
                            case 4: l1 =  ((WE_UINT32)(*(--(c)))) ; /*fall through*/\
                            case 3: l1 |= ((WE_UINT32)(*(--(c)))) << 8;/*fall through*/ \
                            case 2: l1 |= ((WE_UINT32)(*(--(c)))) << 16;/*fall through*/ \
                            case 1: l1 |= ((WE_UINT32)(*(--(c)))) << 24;/*fall through*/ \
                            default :/*fall through*/\
                            break;\
                } \
            }

#define M_WE_RC5_L2NN(l1,l2,c,n)    { \
            c += n; \
            switch (n) { \
                            case 8: *(--(c))=(WE_UCHAR)(((l2) ) &0xff); /*fall through*/\
                            case 7: *(--(c))=(WE_UCHAR)(((l2) >> 8) & 0xff); /*fall through*/\
                            case 6: *(--(c))=(WE_UCHAR)(((l2) >> 16) & 0xff); /*fall through*/\
                            case 5: *(--(c))=(WE_UCHAR)(((l2) >> 24) & 0xff);/*fall through*/\
                            case 4: *(--(c))=(WE_UCHAR)(((l1)) & 0xff); /*fall through*/\
                            case 3: *(--(c))=(WE_UCHAR)(((l1) >> 8) & 0xff);/*fall through*/ \
                            case 2: *(--(c))=(WE_UCHAR)(((l1) >> 16) & 0xff);/*fall through*/ \
                            case 1: *(--(c))=(WE_UCHAR)(((l1) >> 24) & 0xff); /*fall through*/\
                            default :/*fall through*/\
                            break;\
                } \
            }

#define M_WE_ROTATE_L32(a,n)     \
(((a) << (n & 0x1f))|(((a) & 0xffffffff) >> (32 - (n & 0x1f))))

#define M_WE_ROTATE_R32(a,n)     \
(((a) << (32 - (n & 0x1f)))|(((a) & 0xffffffff) >> (n & 0x1f)))

#define E_RC5_32(a,b,s,n) {\
    a ^= b; \
    a = M_WE_ROTATE_L32(a,b); \
    a += s[n]; \
    a &= 0xffffffffL; \
    b ^= a; \
    b = M_WE_ROTATE_L32(b,a); \
    b += s[n+1]; \
b &= 0xffffffffL;}

#define D_RC5_32(a,b,s,n) {\
    b -= s[n+1]; \
    b &= 0xffffffffL; \
    b = M_WE_ROTATE_R32(b,a); \
    b ^= a; \
    a -= s[n]; \
    a &= 0xffffffffL; \
    a = M_WE_ROTATE_R32(a,b); \
a ^= b;}     


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeRC5Key
{
    WE_INT32    iRounds;
    WE_UINT32   auiData[3*(M_WE_RC5_ROUNDS + 1)];
}St_WeRC5Key;

typedef struct tagSt_WeRC5Parameter
{
    WE_UINT32       uiType;
    St_WeRC5Key     stKey;
    WE_UCHAR        aucIv[8];
    WE_UCHAR        aucBuf[8];
    WE_INT32        iBufLen;
}St_WeRC5Parameter;


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_VOID We_LibSetRC5Key(St_WeRC5Key* pstKey, St_WeCipherCrptKey stKey);

WE_VOID We_RC5Encrypt(WE_UINT32* puiData, St_WeRC5Key* pstKey);

WE_VOID We_RC5Decrypt(WE_UINT32* puiData, St_WeRC5Key* pstKey);

WE_VOID We_RC5CryptCBC(const WE_UCHAR* pucIn, WE_UCHAR* pucOut,
                       WE_UINT32 uiInLen, St_WeRC5Key* pstKey, WE_UCHAR* pucIv, WE_INT32 iEnc);



/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    We_LibSetRC5Key
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    convert cipher key to RC5 key
ARGUMENTS PASSED:
    St_WeRC5Key*               pstKey[OUT]:
    St_WeCipherCrptKey         stKey[IN]:
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

WE_VOID We_LibSetRC5Key(St_WeRC5Key* pstKey, St_WeCipherCrptKey stKey)
{
    WE_UINT32   auiL[64] = {0}, uiL = 0, uiLl = 0, uiA = 0, uiB = 0, 
        *puiS = NULL, uiK = 0;
    WE_INT32    iIndexI = 0, iIndexJ = 0, iIndexM= 0, iIndexC = 0, iIndexT = 0,
        iIndexII = 0, iIndexJJ = 0;
    WE_INT32    iLen = stKey.iKeyLen;
    WE_UCHAR*   pucData = stKey.pucKey;
    
    if (!pstKey)
    {
        return;
    }  
    puiS = &(pstKey->auiData[0]);
    iIndexJ = 0;
    for (iIndexI = 0; iIndexI <= (iLen-8); iIndexI += 8)
    {
        M_WE_RC5_C2L(pucData,uiL);
        auiL[iIndexJ++] = uiL;
        M_WE_RC5_C2L(pucData,uiL);
        auiL[iIndexJ++] = uiL;
    }
    iIndexII = iLen-iIndexI;
    if (iIndexII)
    {
        uiK = iLen & 0x07;
        M_WE_RC5_C2LN(pucData,uiL,uiLl,uiK);
        auiL[iIndexJ+0] = uiL;
        auiL[iIndexJ+1] = uiLl;
    }
    
    iIndexC = (iLen + 3) / 4;
    iIndexT = (pstKey->iRounds + 1) * 2;
    puiS[0] = 0xB7E15163L;
    for (iIndexI = 1; iIndexI < iIndexT; iIndexI ++)
    {
        puiS[iIndexI] = (puiS[iIndexI-1] + 0x9E3779B9L) & 0xffffffffL;
    }    
    
    iIndexJ = (iIndexT > iIndexC) ? iIndexT : iIndexC;
    iIndexJ *= 3;
    iIndexII = iIndexJJ = 0;
    uiA = uiB = 0;
    for (iIndexI = 0; iIndexI < iIndexJ; iIndexI ++)
    {
        uiK = (puiS[iIndexII] + uiA + uiB)&0xffffffffL;
        uiA = puiS[iIndexII] = M_WE_ROTATE_L32(uiK, 3);
        iIndexM = (WE_INT32)(uiA + uiB);
        uiK = (auiL[iIndexJJ] + uiA + uiB)&0xffffffffL;
        uiB = auiL[iIndexJJ] = M_WE_ROTATE_L32(uiK, iIndexM);
        if (++iIndexII >= iIndexT) 
        {
            iIndexII = 0;
        }    
        if (++iIndexJJ >= iIndexC) 
        {
            iIndexJJ = 0;
        }    
    }
    
    return ;
}

/*==================================================================================================
FUNCTION: 
    We_RC5Encrypt
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    RC5 encrypt function
ARGUMENTS PASSED:
    WE_UINT32*          puiData[IN/OUT]
    St_WeRC5Key*       pstKey[IN]
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

WE_VOID We_RC5Encrypt(WE_UINT32* puiData, St_WeRC5Key* pstKey)
{
    WE_UINT32 uiA = 0, uiB = 0,  *puiS = NULL;
    WE_INT32  iIndex = 0,iRounds = 0;
    
    if (!puiData || !pstKey)
    {
        return;
    }
    
    puiS = pstKey->auiData;
    iRounds = pstKey->iRounds;
    uiA = puiData[0] + puiS[0];
    uiB = puiData[1] + puiS[1];
    for(iIndex = 2; iIndex < (2 * (iRounds + 1)); iIndex += 2)
    {
        E_RC5_32(uiA, uiB, puiS, iIndex);
    } 
    
    puiData[0] = uiA;
    puiData[1] = uiB;
    
    return ;
}

/*==================================================================================================
FUNCTION: 
    We_RC5Decrypt
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    RC5 decrypt function
ARGUMENTS PASSED:
    WE_UINT32*          puiData[IN/OUT]:
    St_WeRC5Key*       pstKey[IN]:
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

WE_VOID We_RC5Decrypt(WE_UINT32* puiData, St_WeRC5Key* pstKey)
{
    WE_UINT32 uiA = 0, uiB = 0,  *puiS = NULL;
    WE_INT32  iIndex = 0,iRounds = 0;
    
    if (!puiData || !pstKey)
    {
        return;
    }
    
    puiS = pstKey->auiData;
    iRounds = pstKey->iRounds;
    uiA = puiData[0];
    uiB = puiData[1];
    for(iIndex = (2 * (iRounds )); iIndex > 0; iIndex -= 2)
    {
        D_RC5_32(uiA, uiB, puiS, iIndex);
    }
    puiData[0] = uiA - puiS[0];
    puiData[1] = uiB - puiS[1];
    
    return;
}

/*==================================================================================================
FUNCTION: 
    We_RC5CryptCBC
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    RC5 crypt function
ARGUMENTS PASSED:
    const WE_UCHAR*     pucIn[IN]:
    WE_UCHAR*           pucOut[OUT]:
    WE_INT32            iInLen[IN]:
    St_WeRC5Key*       pstKey[IN]:
    WE_UCHAR*           pucIv[IN]:
    WE_INT32            iEnc[IN]:
RETURN VALUE:
    NONE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_VOID We_RC5CryptCBC (const WE_UCHAR* pucIn, 
                        WE_UCHAR* pucOut, WE_UINT32 uiInLen, 
                        St_WeRC5Key* pstKey, WE_UCHAR* pucIv, WE_INT32 iEnc)
{
    WE_UINT32 uiTin0 = 0, uiTin1 = 0;
    WE_UINT32 uiTout0 = 0, uiTout1 = 0, uiXor0 = 0, uiXor1 = 0;
    WE_INT32  iLen = (WE_INT32)uiInLen;
    WE_UINT32 auiTin[2] = {0};
    
    if ((!pstKey) || (!pucIv) || (!pucIn) || (!pucOut))
    {
        return;
    }
    
    if (iEnc)
    {
        M_WE_RC5_C2L(pucIv,uiTout0);
        M_WE_RC5_C2L(pucIv,uiTout1);
        pucIv -= 8;
        for (iLen -= 8; iLen >= 0; iLen -= 8)
        {
            M_WE_RC5_C2L(pucIn,uiTin0);
            M_WE_RC5_C2L(pucIn,uiTin1);
            uiTin0^=uiTout0;
            uiTin1^=uiTout1;
            auiTin[0]=uiTin0;
            auiTin[1]=uiTin1;
            We_RC5Encrypt(auiTin,pstKey);
            uiTout0=auiTin[0]; M_WE_RC5_L2C(uiTout0,pucOut);
            uiTout1=auiTin[1]; M_WE_RC5_L2C(uiTout1,pucOut);
        }
        if (iLen != -8)
        {
            M_WE_RC5_C2LN(pucIn,uiTin0,uiTin1,(iLen+8));
            uiTin0^=uiTout0;
            uiTin1^=uiTout1;
            auiTin[0]=uiTin0;
            auiTin[1]=uiTin1;
            We_RC5Encrypt(auiTin,pstKey);
            uiTout0=auiTin[0]; M_WE_RC5_L2C(uiTout0,pucOut);
            uiTout1=auiTin[1]; M_WE_RC5_L2C(uiTout1,pucOut);
        }
        M_WE_RC5_L2C(uiTout0,pucIv);
        M_WE_RC5_L2C(uiTout1,pucIv);
    }
    else
    {
        M_WE_RC5_C2L(pucIv,uiXor0);
        M_WE_RC5_C2L(pucIv,uiXor1);
        pucIv-=8;
        for (iLen -= 8; iLen >= 0; iLen -= 8)
        {
            M_WE_RC5_C2L(pucIn,uiTin0); auiTin[0]=uiTin0;
            M_WE_RC5_C2L(pucIn,uiTin1); auiTin[1]=uiTin1;
            We_RC5Decrypt(auiTin,pstKey);
            uiTout0=auiTin[0]^uiXor0;
            uiTout1=auiTin[1]^uiXor1;
            M_WE_RC5_L2C(uiTout0,pucOut);
            M_WE_RC5_L2C(uiTout1,pucOut);
            uiXor0=uiTin0;
            uiXor1=uiTin1;
        }
        if (iLen != -8)
        {
            M_WE_RC5_C2L(pucIn,uiTin0); auiTin[0]=uiTin0;
            M_WE_RC5_C2L(pucIn,uiTin1); auiTin[1]=uiTin1;
            We_RC5Decrypt(auiTin,pstKey);
            uiTout0=auiTin[0]^uiXor0;
            uiTout1=auiTin[1]^uiXor1;
            M_WE_RC5_L2CN(uiTout0,uiTout1,pucOut,(iLen+8));
            uiXor0=uiTin0;
            uiXor1=uiTin1;
        }
        M_WE_RC5_L2C(uiXor0,pucIv);
        M_WE_RC5_L2C(uiXor1,pucIv);
    }
    
    return ;
}

/*==================================================================================================
FUNCTION: 
    We_LibRC5CryptInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:

ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    WE_HANDLE*              pHandle[OUT]:
    WE_INT32                iPadding[IN]:
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
  
WE_INT32 We_LibRC5CryptInit(WE_INT32 iRounds,
                              St_WeCipherCrptKey stKey,
                              WE_HANDLE* pHandle)
{
    St_WeRC5Parameter*   pstPara   = NULL;
    WE_INT32              iRv       = M_WE_LIB_ERR_OK;
    if(!stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
      (stKey.iIvLen < 8) || (!pHandle))
    {
      return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if((16 != stKey.iKeyLen)||(8 != stKey.iIvLen))
    {
      return M_WE_LIB_ERR_KEY_LENGTH;
    }

    pstPara = (St_WeRC5Parameter*)WE_MALLOC(sizeof(St_WeRC5Parameter));
    if (!pstPara)
    {
      return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }

    pstPara->uiType = M_WE_RC5_TYPE;
    //pstPara->iPadding = iPadding;
    pstPara->stKey.iRounds = iRounds;
    We_LibSetRC5Key(&(pstPara->stKey), stKey);
    (void)WE_MEMCPY(pstPara->aucIv, stKey.pucIv, 8);
    pstPara->iBufLen = 0;

    *pHandle = (WE_HANDLE)pstPara;

    return iRv;
}
/*==================================================================================================
FUNCTION: 
  We_libRC5EncryptInit
CREATE DATE:
  2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    WE_HANDLE*              pHandle[OUT]:
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

 WE_INT32 We_LibRC5EncryptInit (WE_INT32 iRounds,
                                       St_WeCipherCrptKey stKey,
                                       WE_HANDLE* pHandle)
{
    if(!stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 8) || (!pHandle))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if((16 != stKey.iKeyLen)||(8 != stKey.iIvLen))
    {
        return M_WE_LIB_ERR_KEY_LENGTH;
    }

    return We_LibRC5CryptInit(iRounds, stKey, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_libRC5EncryptUpdate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    const WE_UCHAR*     pucIn[IN]:
    WE_INT32            iInLen[IN]:
    WE_UCHAR*           pucOut[OUT]:
    WE_INT32*           piOutLen[IN/OUT]:
    WE_HANDLE           handle[IN]:
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

 WE_INT32 We_LibRC5EncryptUpdate (const WE_UCHAR* pucIn, 
                                         WE_UINT32 uiInLen, 
                                         WE_UCHAR* pucOut, 
                                         WE_UINT32* puiOutLen,
                                         WE_HANDLE handle)
{
    St_WeRC5Parameter*   pstPara   = NULL;
    WE_INT32              iRv       = M_WE_LIB_ERR_OK;
    WE_UINT32             uiLen     = 0; 

    if((0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    uiLen = uiInLen % 8;
    if(*puiOutLen >= ((1 + uiInLen / 8) * 8))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    pstPara = (St_WeRC5Parameter*)handle;

    /*if(CIPHER_PADDING_RFC2630 == pstPara->iPadding)
    {
        WE_UCHAR aucData[8] = {0};
        We_RC5CryptCBC(pucIn, pucOut, uiInLen - uiLen, &(pstPara->stKey),\
            pstPara->aucIv, M_WE_RC5_ENC);
        (WE_VOID)WE_MEMSET(aucData,(WE_UCHAR)(8-uiLen),8);
        (WE_VOID)WE_MEMCPY(aucData,(pucIn + uiInLen - uiLen),uiLen);
        We_RC5CryptCBC(aucData, (pucOut + uiInLen - uiLen), 8, \
            &(pstPara->stKey), pstPara->aucIv, M_WE_RC5_ENC);
    }
    else */if(0 == uiLen)
    {        
        We_RC5CryptCBC(pucIn, pucOut, uiInLen, &(pstPara->stKey),\
            pstPara->aucIv, M_WE_RC5_ENC);
    }
    else
    {
        iRv = M_WE_LIB_ERR_DATA_LENGTH;
    }

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    We_libRC5EncryptFinal
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE       handle[IN]:
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

 WE_INT32 We_LibRC5EncryptFinal(WE_HANDLE handle)
{
    St_WeRC5Parameter*   pstPara   = NULL;
    WE_INT32              iRv       = M_WE_LIB_ERR_OK;

    if((NULL == handle))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    pstPara = (St_WeRC5Parameter*)handle;
    WE_LIB_FREE(pstPara);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    We_libRC5DecryptInit
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    WE_HANDLE*              pHandle[OUT]:
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
 WE_INT32 We_LibRC5DecryptInit (WE_INT32 iRounds, St_WeCipherCrptKey stKey, WE_HANDLE* pHandle)
 {
     return We_LibRC5CryptInit(iRounds, stKey, pHandle);
 }

/*==================================================================================================
FUNCTION: 
    We_libRC5DecryptUpdate
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    const WE_UCHAR*     pucIn[IN]:
    WE_INT32            iInLen[IN]:
    WE_UCHAR*           pucOut[OUT]:
    WE_INT32*           piOutLen[IN/OUT]:
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
WE_INT32 We_LibRC5DecryptUpdate (const WE_UCHAR* pucIn, WE_UINT32 uiInLen, WE_UCHAR* pucOut, 
                                 WE_UINT32* puiOutLen, WE_HANDLE handle)
{
    St_WeRC5Parameter*   pstPara   = NULL;
    WE_INT32              iRv       = M_WE_LIB_ERR_OK;
    WE_UINT32             uiLen     = 0; 

    if((0 == uiInLen) || !pucIn || \
        !pucOut || !puiOutLen || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    uiLen = uiInLen % 8;
    if(!uiLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiLen;        
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    pstPara = (St_WeRC5Parameter*)handle;

    We_RC5CryptCBC(pucIn, pucOut, uiInLen, &(pstPara->stKey),\
                          pstPara->aucIv, M_WE_RC5_DEC);    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    We_libRC5DecryptFinal
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE       handle[IN]:
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
WE_INT32 We_LibRC5DecryptFinal(WE_HANDLE handle)
{
    return We_LibRC5EncryptFinal(handle);
}

/*==================================================================================================
FUNCTION: 
    We_libRC5EncryptInit1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    WE_HANDLE*              pHandle[OUT]:
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
WE_INT32 We_LibRC5EncryptInit1 (WE_INT32 iRounds, St_WeCipherCrptKey stKey, WE_HANDLE* pHandle)
{
    return We_LibRC5CryptInit(iRounds, stKey, pHandle);
}

/*==================================================================================================
FUNCTION: 
    We_libRC5EncryptUpdate1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    const WE_UCHAR*         pucIn[IN]:
    WE_INT32                iInLen[IN]:
    WE_UCHAR*               pucOut[OUT]:
    WE_INT32*               piOutLen[IN/OUT]:
    WE_HANDLE               handle[IN]:
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
WE_INT32 We_LibRC5EncryptUpdate1 (const WE_UCHAR* pucIn, WE_UINT32 uiInLen, WE_UCHAR* pucOut, 
                                  WE_UINT32* puiOutLen, WE_HANDLE handle)
{
    St_WeRC5Parameter*   pstPara   = NULL;
    WE_INT32              iRv       = M_WE_LIB_ERR_OK;
    WE_INT32              iBufLen   = 0;
    WE_INT32              iIndex    = 0;

    if((0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    iBufLen = (uiInLen % 8);
    if(iBufLen && (*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else if(!iBufLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    pstPara = (St_WeRC5Parameter*)handle;
    iIndex = pstPara->iBufLen;
    
    if((uiInLen + (WE_UINT32)iIndex) == *puiOutLen)
    {
        pstPara->iBufLen = 0;
        (void)WE_MEMCPY((pstPara->aucBuf + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        We_RC5CryptCBC(pstPara->aucBuf , pucOut, 8, &(pstPara->stKey),\
                          pstPara->aucIv, M_WE_RC5_ENC);
        if(8 != *puiOutLen)
        {
            We_RC5CryptCBC((pucIn + (8 - iIndex)), pucOut + 8, \
                (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)), &(pstPara->stKey), \
                pstPara->aucIv, M_WE_RC5_ENC);
        }            
    }
    else if((uiInLen + (WE_UINT32)iIndex) < *puiOutLen)
    {
        pstPara->iBufLen = (iIndex + (WE_INT32)uiInLen) % 8;
        *puiOutLen -= 8 ; 
        if((uiInLen + (WE_UINT32)iIndex) > 8)
        {    
            (void)WE_MEMCPY((pstPara->aucBuf + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
            We_RC5CryptCBC(pstPara->aucBuf , pucOut, 8, \
                &(pstPara->stKey), pstPara->aucIv, M_WE_RC5_ENC);
            We_RC5CryptCBC((pucIn + (8 - iIndex)), pucOut + 8, \
                (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)), &(pstPara->stKey), \
                pstPara->aucIv, M_WE_RC5_ENC);
            iIndex = (WE_INT32)uiInLen - pstPara->iBufLen;
            (void)WE_MEMCPY(pstPara->aucBuf, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
        }    
        else
        {
            (void)WE_MEMCPY(pstPara->aucBuf + iIndex, pucIn, uiInLen);
        }
    }
    else
    {
        pstPara->iBufLen = (iIndex + (WE_INT32)uiInLen) - (WE_INT32)*puiOutLen;
        (void)WE_MEMCPY((pstPara->aucBuf + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        We_RC5CryptCBC(pstPara->aucBuf , pucOut, 8, \
                &(pstPara->stKey), pstPara->aucIv, M_WE_RC5_ENC);
        We_RC5CryptCBC((pucIn + (8 - iIndex)), pucOut + 8, \
            (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)), &(pstPara->stKey), \
            pstPara->aucIv, M_WE_RC5_ENC);
        iIndex = (WE_INT32)uiInLen - pstPara->iBufLen;
        (void)WE_MEMCPY(pstPara->aucBuf, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
    }

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    We_libRC5EncryptFinal1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UCHAR*       pucOut[OUT]:
    WE_INT32*       piOutLen[IN/OUT]:
    WE_HANDLE       handle[IN]:
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

 WE_INT32 We_LibRC5EncryptFinal1(WE_UCHAR* pucOut, 
                                 WE_UINT32* puiOutLen, WE_HANDLE handle)
{
    St_WeRC5Parameter*   pstPara       = NULL;
    WE_UCHAR              aucData[8]    = {0};
    //WE_INT32             iRv = M_WE_LIB_ERR_OK;

    if(!pucOut || !puiOutLen || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    if(*puiOutLen < 8)
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    *puiOutLen = 8;
    pstPara = (St_WeRC5Parameter*)handle;
    {
        (WE_VOID)WE_MEMSET(aucData, (WE_UCHAR)(8-pstPara->iBufLen), (WE_UINT32)8);
        (WE_VOID)WE_MEMCPY(aucData, pstPara->aucBuf, (WE_UINT32)pstPara->iBufLen);
    }
    We_RC5CryptCBC(aucData , pucOut, (WE_UINT32)(8), \
            &(pstPara->stKey), pstPara->aucIv, M_WE_RC5_ENC);

    return We_LibRC5EncryptFinal(handle);
}

/*==================================================================================================
FUNCTION: 
    We_libRC5DecryptInit1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    WE_HANDLE*              pHandle[OUT]:
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
WE_INT32 We_LibRC5DecryptInit1 (WE_INT32 iRounds,
                                        St_WeCipherCrptKey stKey,
                                        WE_HANDLE* pHandle)
{
    return We_LibRC5CryptInit(iRounds, stKey, pHandle);
}
                                  
/*==================================================================================================
FUNCTION: 
    We_libRC5DecryptUpdate1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    const WE_UCHAR*     pucIn[IN]:
    WE_INT32            iInLen[IN]:
    WE_UCHAR*           pucOut[OUT]:
    WE_INT32*           piOutLen[IN/OUT]:
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

 WE_INT32 We_LibRC5DecryptUpdate1 (const WE_UCHAR* pucIn, 
                                          WE_UINT32 uiInLen, 
                                          WE_UCHAR* pucOut, 
                                          WE_UINT32* puiOutLen,
                                          WE_HANDLE handle)
{
    St_WeRC5Parameter*   pstPara   = NULL;
    WE_INT32              iRv       = M_WE_LIB_ERR_OK;
    WE_INT32              iBufLen   = 0;
    WE_INT32              iIndex    = 0;

    if((0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen) || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    iBufLen = (uiInLen % 8);
    if(iBufLen && (*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else if(!iBufLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }

    pstPara = (St_WeRC5Parameter*)handle;
    iIndex = pstPara->iBufLen;
    
    if((uiInLen + (WE_UINT32)iIndex) == *puiOutLen)
    {
        pstPara->iBufLen = 0;
        (void)WE_MEMCPY((pstPara->aucBuf + iIndex), pucIn, 8 - (WE_UINT32)iIndex);
        We_RC5CryptCBC(pstPara->aucBuf , pucOut, 8, &(pstPara->stKey),\
                          pstPara->aucIv, M_WE_RC5_DEC);
        if(8 != *puiOutLen)
        {
            We_RC5CryptCBC((pucIn + (8 - iIndex)), pucOut + 8, \
                (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)), &(pstPara->stKey), \
                pstPara->aucIv, M_WE_RC5_DEC);
        }            
    }
    else if((uiInLen + (WE_UINT32)iIndex) < *puiOutLen)
    {
        pstPara->iBufLen = (iIndex + (WE_INT32)uiInLen) % 8;
        *puiOutLen -= 8 ; 
        if((uiInLen + (WE_UINT32)iIndex) > 8)
        {    
            (void)WE_MEMCPY((pstPara->aucBuf + iIndex), pucIn, 8 - (WE_UINT32)iIndex);
            We_RC5CryptCBC(pstPara->aucBuf , pucOut, 8, \
                &(pstPara->stKey), pstPara->aucIv, M_WE_RC5_DEC);
            We_RC5CryptCBC((pucIn + (8 - iIndex)), pucOut + 8, \
                (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)), &(pstPara->stKey), \
                pstPara->aucIv, M_WE_RC5_DEC);
            iIndex = (WE_INT32)uiInLen - pstPara->iBufLen;
            (void)WE_MEMCPY(pstPara->aucBuf, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
        }    
        else
        {
            (void)WE_MEMCPY(pstPara->aucBuf + iIndex, pucIn, uiInLen);
        }
    }
    else
    {
        pstPara->iBufLen = (iIndex + (WE_INT32)uiInLen) - (WE_INT32)*puiOutLen;
        (void)WE_MEMCPY((pstPara->aucBuf + iIndex), pucIn, 8 - (WE_UINT32)iIndex);
        We_RC5CryptCBC(pstPara->aucBuf , pucOut, 8, \
                &(pstPara->stKey), pstPara->aucIv, M_WE_RC5_DEC);
        We_RC5CryptCBC((pucIn + (8 - iIndex)), pucOut + 8, \
            (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)), &(pstPara->stKey), \
            pstPara->aucIv, M_WE_RC5_DEC);
        iIndex = (WE_INT32)uiInLen - pstPara->iBufLen;
        (void)WE_MEMCPY(pstPara->aucBuf, pucIn + iIndex, (WE_UINT32)pstPara->iBufLen);
    }

    return iRv;
}

/*==================================================================================================
FUNCTION: 
    We_libRC5DecryptFinal1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UCHAR*           pucOut[OUT]:
    WE_INT32*           piOutLen[IN/OUT]:
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

 WE_INT32 We_LibRC5DecryptFinal1(WE_UCHAR* pucOut, 
                                        WE_UINT32* puiOutLen, WE_HANDLE handle)
{
    St_WeRC5Parameter*   pstPara = NULL;
    WE_UINT32             uiLen     = 0;

    if(!pucOut || !puiOutLen || !handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    if(*puiOutLen < 8)
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    *puiOutLen = 8;
    pstPara = (St_WeRC5Parameter*)handle;

    We_RC5CryptCBC(pstPara->aucBuf , pucOut, (WE_UINT32)8, \
                &(pstPara->stKey), pstPara->aucIv, M_WE_RC5_DEC);
    uiLen = *(pucOut + 8 - 1);
    if(uiLen&&(uiLen <= 8))
    {
        WE_UINT32   uiLoop = 0;
        for(; uiLoop < uiLen; uiLoop++)
        {
            if(uiLen != *(pucOut + 8 - 1 - uiLoop))
            {
                break;
            }
        }
        if(uiLoop == uiLen)
        {
            *puiOutLen -= uiLen;
        }
    }

    return We_LibRC5EncryptFinal(handle);
}


/*==================================================================================================
FUNCTION: 
    We_libRC5Encrypt
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    const WE_UCHAR*         pucIn[IN]:
    WE_INT32                iInLen[IN]:
    WE_UCHAR*               pucOut[OUT]:
    WE_INT32*               piOutLen[IN/OUT]:
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

WE_INT32 We_LibRC5Encrypt (WE_INT32 iRounds,
                                   St_WeCipherCrptKey stKey, 
                                   const WE_UCHAR* pucIn, 
                                   WE_UINT32 uiInLen, 
                                   WE_UCHAR* pucOut, 
                                   WE_UINT32* puiOutLen)
{
    St_WeRC5Key    stRc5Key    = {0};
    WE_INT32        iRv         = M_WE_LIB_ERR_OK;
    WE_UINT32       uiLen       = 0; 
    WE_UCHAR        aucIv[8]    = {0};

    if(!stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 8) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if((16 != stKey.iKeyLen)||(8 != stKey.iIvLen))
    {
        return M_WE_LIB_ERR_KEY_LENGTH;
    }

    uiLen = uiInLen % 8;
    if(uiLen && (*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else if(!uiLen && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    stRc5Key.iRounds = iRounds;
    We_LibSetRC5Key(&stRc5Key, stKey);

    (WE_VOID)WE_MEMCPY((WE_UCHAR*)aucIv, stKey.pucIv, 8);
    We_RC5CryptCBC(pucIn, pucOut, uiInLen, &(stRc5Key),\
        (WE_UCHAR*)aucIv, M_WE_RC5_ENC);
    
    return iRv;
}

/*==================================================================================================
FUNCTION: 
    We_libRC5Decrypt
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    const WE_UCHAR*         pucIn[IN]:
    WE_INT32                iInLen[IN]:
    WE_UCHAR*               pucOut[OUT]:
    WE_INT32*               piOutLen[IN/OUT]:
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

 WE_INT32 We_LibRC5Decrypt (WE_INT32 iRounds,
                                   St_WeCipherCrptKey stKey, 
                                   const WE_UCHAR* pucIn, 
                                   WE_UINT32 uiInLen, 
                                   WE_UCHAR* pucOut, 
                                   WE_UINT32* puiOutLen)
{
    St_WeRC5Key    stRc5Key    = {0};
    WE_INT32        iRv         = M_WE_LIB_ERR_OK;
    WE_UCHAR        aucIv[8]    = {0};

    if(!stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 8) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if((16 != stKey.iKeyLen)||(8 != stKey.iIvLen))
    {
        return M_WE_LIB_ERR_KEY_LENGTH;
    }

    if(!(uiInLen % 8) && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    stRc5Key.iRounds = iRounds;
    We_LibSetRC5Key(&stRc5Key, stKey);
    (WE_VOID)WE_MEMCPY(aucIv,stKey.pucIv,8);
    We_RC5CryptCBC(pucIn, pucOut, uiInLen, &stRc5Key, \
                    stKey.pucIv, M_WE_RC5_DEC);
    
    return iRv;
}
/*==================================================================================================
FUNCTION: 
    We_libRC5Encrypt1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    const WE_UCHAR*         pucIn[IN]:
    WE_INT32                iInLen[IN]:
    WE_UCHAR*               pucOut[OUT]:
    WE_INT32*               piOutLen[IN/OUT]:
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
WE_INT32 We_LibRC5Encrypt1 (WE_INT32 iRounds,
                                   St_WeCipherCrptKey stKey, 
                                   const WE_UCHAR* pucIn, 
                                   WE_UINT32 uiInLen, 
                                   WE_UCHAR* pucOut, 
                                   WE_UINT32* puiOutLen)
{
    St_WeRC5Key    stRc5Key    = {0};
    WE_INT32        iRv         = M_WE_LIB_ERR_OK;
    WE_UINT32       uiLen       = 0; 
    WE_UCHAR        aucIv[8]    = {0};

    if(!stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 8) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if((16 != stKey.iKeyLen)||(8 != stKey.iIvLen))
    {
        return M_WE_LIB_ERR_KEY_LENGTH;
    }

    uiLen = uiInLen % 8;
    if((*puiOutLen >= ((1 + uiInLen / 8) * 8)))
    {
        *puiOutLen = ((1 + uiInLen / 8) * 8);
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    stRc5Key.iRounds = iRounds;
    We_LibSetRC5Key(&stRc5Key, stKey);

    (WE_VOID)WE_MEMCPY((WE_UCHAR*)aucIv, stKey.pucIv, 8);

    We_RC5CryptCBC(pucIn, pucOut, uiInLen - uiLen, &(stRc5Key),\
                    (WE_UCHAR*)aucIv, M_WE_RC5_ENC);

    {
        WE_UCHAR aucData[8] = {0};
        (WE_VOID)WE_MEMSET(aucData,(WE_UCHAR)(8-uiLen),8);
        if(uiLen)
        {
            (WE_VOID)WE_MEMCPY(aucData,(pucIn + uiInLen - uiLen),uiLen);
        }
        
        We_RC5CryptCBC(aucData, (pucOut + uiInLen - uiLen), 8, \
            &(stRc5Key), (WE_UCHAR*)aucIv, M_WE_RC5_ENC);
    }
    
    return iRv;
}
/*==================================================================================================
FUNCTION: 
    We_libRC5Decrypt1
CREATE DATE:
    2006-07-07
AUTHOR:
    steven ding
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_WeCipherCrptKey     stKey[IN]:
    const WE_UCHAR*         pucIn[IN]:
    WE_INT32                iInLen[IN]:
    WE_UCHAR*               pucOut[OUT]:
    WE_INT32*               piOutLen[IN/OUT]:
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
WE_INT32 We_LibRC5Decrypt1 (WE_INT32 iRounds,
                                   St_WeCipherCrptKey stKey, 
                                   const WE_UCHAR* pucIn, 
                                   WE_UINT32 uiInLen, 
                                   WE_UCHAR* pucOut, 
                                   WE_UINT32* puiOutLen)
{
    St_WeRC5Key    stRc5Key    = {0};
    WE_INT32        iRv         = M_WE_LIB_ERR_OK;
    WE_UINT32       uiLen       = 0; 
    WE_UCHAR        aucIv[8]    = {0};

    if(!stKey.pucKey || !stKey.pucIv || (0 >= stKey.iKeyLen) || \
        (stKey.iIvLen < 8) || (0 == uiInLen) || (!pucIn) || (!pucOut) || (!puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if((16 != stKey.iKeyLen)||(8 != stKey.iIvLen))
    {
        return M_WE_LIB_ERR_KEY_LENGTH;
    }

    if(!(uiInLen % 8) && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    stRc5Key.iRounds = iRounds;
    We_LibSetRC5Key(&stRc5Key, stKey);
    (WE_VOID)WE_MEMCPY(aucIv,stKey.pucIv,8);
    We_RC5CryptCBC(pucIn, pucOut, uiInLen, &stRc5Key, \
                    stKey.pucIv, M_WE_RC5_DEC);
    uiLen = *(pucOut + uiInLen - 1);
    if(uiLen&&(uiLen <= 8))
    {
        WE_UINT32   uiLoop = 0;
        for(; uiLoop < uiLen; uiLoop++)
        {
            if(uiLen != *(pucOut + uiInLen - 1 - uiLoop))
            {
                break;
            }
        }
        if(uiLoop == uiLen)
        {
            *puiOutLen -= uiLen;
        }
    }
    
    return iRv;
}

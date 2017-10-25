/*==================================================================================================
    FILE   NAME : we_rnd.c
    MODULE NAME : WE

    GENERAL DESCRIPTION
        define several functions to realize generate random data.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History

    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-11-03         Sam         None          Init

==================================================================================================*/

/*==================================================================================================
*   Include File Section
*=================================================================================================*/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_rnd.h"


/***************************************************************************************************
*   Macro Define Section
***************************************************************************************************/
#ifndef MPA_TMP_PLACES
#define MPA_TMP_PLACES (256)
#endif 

#define BIT_CHAR 8
#define MPA_LONG_BIT (4 * BIT_CHAR)
#define MPA_SHORT_BIT (MPA_LONG_BIT >> 1)
#define BBS_COMB(uiHI,uiLO) (((uiHI) << MPA_SHORT_BIT) + (uiLO))
#define BBS_HI(x) ((x) >> MPA_SHORT_BIT)
#define BBS_LO(x) ((x) & (((WE_ULONG) 1 << MPA_SHORT_BIT) - 1))
#define MPA_DIALOG_ONE 1UL
#define WE_MPA_MOD(pstRem, pstNumer, pstDenom) We_LibMpaDiv(0, pstRem, pstNumer, pstDenom)

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeLibMpaNum
{
    WE_UINT32   uiPrec;     
    WE_UINT32   *puiDig; 
} St_WeLibMpaNum;

typedef struct tagSt_WeLibBbsMach
{
    St_WeLibMpaNum        stN;        
    WE_UINT32        uiNbits;    
    WE_UINT32        uiNoct;     
    St_WeLibMpaNum        stX;        
    WE_UINT32         uiPending;  
    WE_UINT32        *puiTmp;      
    WE_INT32         iSeeded;   
} St_WeLibBbsMach;


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 We_LibBbsNext(St_WeLibBbsMach *pstM);
static WE_INT32 We_LibBbsGetImpl(WE_VOID *pvDest, WE_UINT32 uiLen, St_WeLibPrngInstS *pstMach);
static WE_INT32 We_LibBbsGet(WE_VOID *pvDest, WE_UINT32 uiLen, St_WeLibPrngInstS *pstMach);
static WE_VOID  We_LibBbsDestroy(St_WeLibPrngInstS *pstMach);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
WE_VOID We_LibMpaMul(St_WeLibMpaNum *pstRes, St_WeLibMpaNum *pstOp1, St_WeLibMpaNum *pstOp2)
{
    WE_UINT32   uiJLoop=0;
    WE_ULONG    uiTemp=0; 
    WE_UINT32   uiOp1Prec =0;
    WE_UINT32   uiOp2Prec = 0;
    
    if(!pstRes||!pstOp1||!pstOp2)
    {
        return ;
    }    
    uiOp1Prec = pstOp1->uiPrec;
    uiOp2Prec = pstOp2->uiPrec;
    (WE_VOID)WE_MEMSET(pstRes->puiDig, 0, uiOp1Prec * sizeof( WE_ULONG));
    
    for(uiJLoop = 0; uiJLoop < uiOp2Prec; uiJLoop++) 
    {
        WE_ULONG *puiDst = pstRes->puiDig + uiJLoop;
        WE_ULONG *puiSrc = pstOp1->puiDig;
        WE_ULONG  uiCur = 0;
        WE_ULONG *puiEnd = puiSrc + uiOp1Prec;
        WE_ULONG  uiML = pstOp2->puiDig[uiJLoop];
        WE_ULONG  uiMH = BBS_HI(uiML);
        
        uiML = BBS_LO(uiML);
        while(puiSrc < puiEnd) 
        { 
            WE_ULONG uiL1 = *puiSrc++;
            WE_ULONG uiH1 = BBS_HI(uiL1);
            WE_ULONG uiNXT = uiCur;
            
            uiL1 = BBS_LO(uiL1);
            uiCur += uiL1 * uiML; 
            uiNXT = uiCur < uiNXT; 
            uiTemp = uiL1; uiL1 = uiTemp * uiMH;  
            uiNXT += uiMH * uiH1; 
            
            
            uiTemp = uiH1; uiH1 = uiTemp * uiML;  
            uiL1 += uiH1; 
            uiNXT += ( WE_ULONG) (uiL1 < uiH1) << MPA_SHORT_BIT; 
            uiH1 = uiCur + (uiL1 << MPA_SHORT_BIT); 
            if(uiH1 < uiCur)
            {
                uiNXT += (uiL1 >> MPA_SHORT_BIT) + 1;
            }
            else
            {
                uiNXT += (uiL1 >> MPA_SHORT_BIT) + 0;
            }
            //uiNXT += (uiL1 >> MPA_BIT_2) + (uiH1 < uiCur); 
            uiCur = uiH1 + *puiDst;
            *puiDst++ = uiCur;
            if(uiCur < uiH1)
            {
                uiCur = uiNXT + 1;
            }
            else
            {
                uiCur = uiNXT + 0;
            }    
            //uiCur = uiNXT + (uiCur < uiH1);
        }
        *puiDst = uiCur;
    }
}

WE_VOID We_LibMpaNorm(St_WeLibMpaNum *pstNum, WE_UINT32 uiPrec)
{
    WE_ULONG *pulLastPlaceP = NULL;
    
    if(!pstNum)
    {
        return ;
    }    
    pulLastPlaceP = &pstNum->puiDig[uiPrec];
    while(*(--pulLastPlaceP) == 0 && uiPrec > 1)
    {
        --uiPrec;
    }
    
    pstNum->uiPrec = uiPrec;
}


WE_INT32 We_LibMpaBitLen(St_WeLibMpaNum *pstNum,WE_INT32 *piRval)
{
    WE_UINT32       uiNbits=0;
    WE_UINT32       uiPrec=0;
    WE_ULONG        uiMsPlace=0; 
    
    if(!pstNum||!piRval)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }   
    
    uiPrec = pstNum->uiPrec - 1;
    
    if ((uiPrec * sizeof ( WE_ULONG) * BIT_CHAR + 1) > (WE_UINT32) ~1) 
    {
        /*"number too big");*/
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    uiNbits = uiPrec * sizeof( WE_ULONG) * BIT_CHAR + 1;
    uiMsPlace = pstNum->puiDig[uiPrec];
    
    if(uiMsPlace & 0xffff0000UL) uiMsPlace >>= 16, uiNbits += 16;
    if(uiMsPlace & 0xff00) uiMsPlace >>= 8, uiNbits += 8;
    if(uiMsPlace & 0xf0) uiMsPlace >>= 4, uiNbits += 4;
    if(uiMsPlace & 0xc) uiMsPlace >>= 2, uiNbits += 2;
    if(uiMsPlace & 0x2) uiNbits += 1;
    *piRval=(WE_INT32)uiNbits;
    
    return M_WE_LIB_ERR_OK;
}



WE_VOID We_LibMpaShiftL(St_WeLibMpaNum *pstDest, St_WeLibMpaNum *pstSrc, WE_UINT8 ucShift,WE_UINT32 *puiShifted)
{
    WE_UINT8        ucRShift=0;
    WE_ULONG        *puiSrc = NULL;
    WE_ULONG        *puiDst = NULL;
    WE_ULONG        *puiEnd = NULL;
    WE_ULONG        uiShifted=0;
    WE_ULONG        uiOShifted=0;
    
    if(!pstDest||!pstSrc||!puiShifted)
    {
        return ;
    }
    puiSrc = pstSrc->puiDig;
    puiDst = pstDest->puiDig;
    puiEnd = pstSrc->puiDig + pstSrc->uiPrec;
    
    ucRShift = (WE_UINT8) (MPA_LONG_BIT - (ucShift %= MPA_LONG_BIT));
    
    if(!ucShift) 
    {
        if(pstSrc->puiDig != pstDest->puiDig)
        {
            (WE_VOID)WE_MEMCPY(pstDest->puiDig, pstSrc->puiDig, pstSrc->uiPrec * sizeof( WE_ULONG));
        }
        *puiShifted=0;
        // return 0;
        return ;
    }
    
    uiShifted = *puiSrc >> ucRShift;
    *puiDst++ = *puiSrc++ << ucShift;
    while(puiSrc < puiEnd) 
    {
        uiOShifted = *puiSrc >> ucRShift;
        *puiDst++ = (*puiSrc++ << ucShift) + uiShifted;
        uiShifted = uiOShifted;
    }
    *puiShifted=uiShifted;
    //return uiShifted;
}


static WE_ULONG We_LibMpaSingleDiv( WE_ULONG uiTHI,  WE_ULONG uiTLO,  WE_ULONG uiBLO,  WE_ULONG *pulQuo)
{
    WE_ULONG uiRem=0;
    WE_ULONG uiQuo=0;
    
    if(uiBLO == 0)
    { 
        uiRem = uiBLO;
        uiQuo = 0;
    } 
    else if(uiBLO <= uiTHI) 
    { 
        uiRem = uiBLO;
        uiQuo = 1;
    }
    else if(uiTHI == 0) 
    { 
        uiRem = uiTLO % uiBLO;
        uiQuo = uiTLO / uiBLO;
    }
    else if(BBS_HI(uiBLO) == 0) 
    { 
        WE_ULONG uiTmp=0;
        uiQuo = BBS_COMB(uiTHI, BBS_HI(uiTLO));
        uiTmp = uiQuo / uiBLO;
        uiQuo %= uiBLO;
        uiQuo = BBS_COMB(uiQuo, BBS_LO(uiTLO));
        uiRem = uiQuo % uiBLO;
        uiQuo /= uiBLO;
        uiQuo = BBS_COMB(uiTmp, uiQuo);
    } 
    else
    { 
        uiQuo = 0;
        
        if(uiBLO & (MPA_DIALOG_ONE << (MPA_LONG_BIT - 1))) 
        {
#define BBS_STGC uiQuo <<= 1; uiRem = uiTHI + (uiTLO >> (MPA_LONG_BIT - 1)); \
    uiTLO <<= 1; if(uiTHI & (MPA_DIALOG_ONE << (MPA_LONG_BIT - 1))) { uiTHI += uiRem - uiBLO; uiQuo++; } \
            else if((uiTHI += uiRem) >= uiBLO) { uiTHI -= uiBLO; uiQuo++; }
            
            BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC
                BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC
                BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC
                BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC BBS_STGC
        }
        else 
        {
#define BBS_STGS uiQuo <<= 1; uiTHI = (uiTHI << 1) + (uiTLO >> (MPA_LONG_BIT - 1)); \
            uiTLO <<= 1; if(uiBLO <= uiTHI) { uiTHI -= uiBLO; uiQuo++; }
            
            BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS
                BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS
                BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS
                BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS BBS_STGS
        }
        
        uiRem = uiTHI;
    }
    
    if(pulQuo)
    {
        *pulQuo = uiQuo;
    }
    return uiRem;
}

static WE_INT32 We_LibMpaGuessQuo( WE_ULONG *puiMsNumer,  WE_ULONG *puiMsDenom,WE_UINT32 *puiQuo)
{
    WE_ULONG *puiU = NULL;
    WE_ULONG *puiV = NULL;
    WE_ULONG  uiRem=0;
    WE_ULONG  uiQuo=0;
    WE_ULONG  uiLO=0;
    WE_ULONG  uiHI=0;
    WE_ULONG  uiT0=0;
    WE_ULONG  uiT1=0;
    WE_ULONG  uiT2=0;
    WE_ULONG  uiTemp=0; 
    
    if(!puiMsDenom||!puiMsNumer||!puiQuo)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    puiU = puiMsNumer;
    puiV = puiMsDenom;
    if(puiU[2] >= puiV[1])
    {     
        uiQuo = ~ ( WE_ULONG) 0;  
        uiRem = puiU[1] + puiV[1];   
        if(uiRem < puiU[1])       
        {
            *puiQuo=uiQuo;
            return M_WE_LIB_ERR_OK;
        }
    } 
    else 
    {
        // ut_assert(puiV[1] != 0);                        
        uiRem = We_LibMpaSingleDiv(puiU[2], puiU[1], puiV[1], &uiQuo); 
        //ut_assert(uiRem != puiV[1]);                      
    }
    
    
    uiT0 = BBS_LO(uiQuo);
    uiT1 = BBS_HI(uiQuo);
    uiT2 = BBS_LO(puiV[0]);
    uiHI = BBS_HI(puiV[0]);
    uiLO = uiT0 * uiT2;
    uiTemp = uiT0; uiT0 = uiTemp * uiHI; 
    uiTemp = uiT2; uiT2 = uiTemp * uiT1; 
    uiTemp = uiHI; uiHI = uiTemp * uiT1; 
    uiT0 += uiT2;
    if(uiT0 < uiT2)
    {
        uiHI += MPA_DIALOG_ONE << MPA_SHORT_BIT;
    }
    uiLO += (uiT1 = uiT0 << MPA_SHORT_BIT);
    if(uiLO < uiT1)
    {
        uiHI += 1;
    }
    uiHI += uiT0 >> MPA_SHORT_BIT;
    while(uiHI > uiRem || (uiHI == uiRem && uiLO > puiU[0])) 
    { 
        --uiQuo;                  
        
        uiRem += puiV[1];            
        if(uiRem < puiV[1])          
        {
            *puiQuo=uiQuo;
            return M_WE_LIB_ERR_OK;
            
        }
        if(puiV[0] > uiLO)  
        {
            --uiHI;
        }
        uiLO -= puiV[0];
    }
    
    *puiQuo=uiQuo;
    return M_WE_LIB_ERR_OK;
    
}

WE_INT32 We_LibMpaAdd(St_WeLibMpaNum *pstRes, St_WeLibMpaNum *pstOp1, St_WeLibMpaNum *pstOp2,WE_INT32 *piRval)
{
    WE_ULONG   *puiRes = NULL;
    WE_ULONG   *puiOp1 = NULL;
    WE_ULONG   *puiOp2 = NULL;
    WE_ULONG   *puiEnd = NULL;
    WE_ULONG   uiOp = 0;
    WE_ULONG   uiSum = 0;
    WE_UINT32      uiCarry = 0;
    
    if(!pstOp1||!pstOp2||!pstRes||!piRval||!pstRes->puiDig||!pstOp1->puiDig||!pstOp2->puiDig)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 
    
    puiRes = pstRes->puiDig;
    puiOp1 = pstOp1->puiDig;
    puiOp2 = pstOp2->puiDig;
    puiEnd = puiOp2 + pstOp2->uiPrec;
    uiOp = *(puiOp1++);
    uiSum = *(puiRes++) = uiOp + *(puiOp2++);
    uiCarry = uiSum < uiOp;
    
    while(puiOp2 < puiEnd) 
    {
        *(puiRes++) = uiSum = (uiOp = *(puiOp1++)) + *(puiOp2++) + uiCarry;
        if(uiSum < uiOp)
        {
            uiCarry = 1;
        }
        else if(uiSum > uiOp)
        {
            uiCarry = 0;
        }
    }
    
    if(puiOp1 < (puiEnd = pstOp1->puiDig + pstOp1->uiPrec) && uiCarry) 
    {
        //        while(!(*(puiRes++) = *(puiOp1++) + 1) && puiOp1 < puiEnd)
        //        {
        //        }
        // modify by Sam, 
        *(puiRes++) = *(puiOp1++) + 1;
        if(puiRes[-1])
        {
            uiCarry = 0;
        }
    }
    
    if(!uiCarry) 
    {
        if(puiRes != puiOp1)
        {
            (WE_VOID)WE_MEMCPY(puiRes, puiOp1, (WE_UINT32) (puiEnd - puiOp1) * sizeof( WE_ULONG));
        }
        *piRval=0;
        return M_WE_LIB_ERR_OK;
    } 
    else
    {
        *piRval=1;
        return M_WE_LIB_ERR_OK;
    }
}

static WE_INT32 We_LibMpaMulSub(St_WeLibMpaNum *pstNumer, St_WeLibMpaNum *pstDenom,  WE_ULONG uiQuoGuess,WE_UINT32 *puiQuo)
{
    WE_ULONG *puiDst = NULL;
    WE_ULONG *puiSrc = NULL;
    WE_ULONG  uiMH = 0;
    WE_ULONG *puiEnd = NULL;
    WE_ULONG  uiML = 0;
    WE_ULONG  uiBorrow = 0;
    WE_ULONG  uiTemp=0; 
    WE_INT32  iRes=0;
    WE_INT32  iRval=0;
    
    if(!pstNumer||!pstDenom||!puiQuo)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    puiDst = pstNumer->puiDig;
    puiSrc = pstDenom->puiDig;
    uiMH = BBS_HI(uiQuoGuess);
    puiEnd = puiSrc + pstDenom->uiPrec;
    uiML = BBS_LO(uiQuoGuess);
    
    if(uiQuoGuess == 0)
    {
        *puiQuo=0;
        return M_WE_LIB_ERR_OK;
    }
    while(puiSrc < puiEnd) 
    {
        WE_ULONG uiL1 = *puiSrc++;
        WE_ULONG uiH1 = BBS_HI(uiL1);
        WE_ULONG uiNXT = uiBorrow;
        
        uiL1 = BBS_LO(uiL1);
        uiBorrow += uiL1 * uiML; 
        uiNXT = uiBorrow < uiNXT; 
        uiTemp = uiL1; uiL1 = uiTemp * uiMH;  
        uiNXT += uiMH * uiH1; 
        
        
        uiTemp = uiH1; uiH1 = uiTemp * uiML;  
        uiL1 += uiH1; 
        uiNXT += ( WE_ULONG) (uiL1 < uiH1) << MPA_SHORT_BIT; 
        uiH1 = uiBorrow + (uiL1 << MPA_SHORT_BIT); 
        if(uiH1 < uiBorrow)
        {
            uiNXT += (uiL1 >> MPA_SHORT_BIT) + 1;
        }
        else
        {
            uiNXT += (uiL1 >> MPA_SHORT_BIT) + 0;
        }    
        //uiNXT += (uiL1 >> MPA_BIT_2) + (uiH1 < uiBorrow); 
        uiBorrow = *puiDst;
        *puiDst++ = uiH1 = uiBorrow - uiH1; 
        if(uiH1 > uiBorrow)
        {
            uiBorrow = uiNXT + 1;
        }    
        else
        {
            uiBorrow = uiNXT + 0; 
        }    
    }
    if(uiBorrow) 
    {
        iRes=We_LibMpaAdd(pstNumer, pstNumer, pstDenom,&iRval);
        if(iRes)
        {
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        if(!iRval)
        {
            // ut_assert(UT_FALSE);            
            return M_WE_LIB_ERR_GENERAL_ERROR;            
        }
        *puiQuo=uiQuoGuess - 1;
        return M_WE_LIB_ERR_OK;
    }
    else
    {
        *puiQuo=uiQuoGuess ;
        return M_WE_LIB_ERR_OK;
    }
}

WE_INT32 We_LibMpaShiftR(St_WeLibMpaNum *pstDest, St_WeLibMpaNum *pstSrc, WE_UINT8 ucShift,WE_UINT32 *puiShifted)
{
    WE_UINT8        ucLShift=0;
    WE_ULONG        *puiEnd = NULL;
    WE_ULONG        *puiSrc = NULL;
    WE_ULONG        *puiDst = NULL;
    WE_ULONG        uiShifted=0;
    WE_ULONG        uiOShifted=0; 
    if(!pstDest||!pstSrc||!puiShifted)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    puiEnd = pstSrc->puiDig;
    puiSrc = puiEnd + pstSrc->uiPrec - 1;
    puiDst = pstDest->puiDig + pstSrc->uiPrec - 1;
    
    ucLShift = (WE_UINT8) (MPA_LONG_BIT - (ucShift %= MPA_LONG_BIT));
    
    if(!ucShift) 
    {
        if(pstSrc->puiDig != pstDest->puiDig)
        {
            (WE_VOID)WE_MEMCPY(pstDest->puiDig, pstSrc->puiDig, pstSrc->uiPrec * sizeof( WE_ULONG));
        }
        *puiShifted=0;
        return M_WE_LIB_ERR_OK;
    }
    
    uiShifted = *puiSrc << ucLShift;
    *puiDst-- = *puiSrc >> ucShift;
    while(puiSrc-- > puiEnd) 
    {
        uiOShifted = *puiSrc << ucLShift;
        *puiDst-- = (*puiSrc >> ucShift) + uiShifted;
        uiShifted = uiOShifted;
    }
    
    // return uiShifted;
    *puiShifted=uiShifted;
    return M_WE_LIB_ERR_OK;
}

static WE_UINT32 We_LibMpaLongDiv(St_WeLibMpaNum *pstQuo, St_WeLibMpaNum *pstRem, St_WeLibMpaNum *pstNumer, St_WeLibMpaNum *pstDenom)
{
    WE_ULONG        auiNormNumberHolder[MPA_TMP_PLACES]={0};
    WE_ULONG        auiNormDenomHolder[MPA_TMP_PLACES]={0};
    WE_ULONG        uiTmp=0;
    St_WeLibMpaNum       stNormNumber={0};
    St_WeLibMpaNum       stNormDenom={0};
    St_WeLibMpaNum       stUnTmp={0};
    St_WeLibMpaNum       stNormRem={0};
    WE_UINT32       uiShift=0;
    WE_UINT32       uiQuoPrec = 0;
    WE_UINT32       uiQuoPlace=0;
    WE_ULONG        *puiMsRem=NULL;   
    WE_ULONG        *puiMsDenom=NULL;   
    WE_INT32        iRes=0;
    WE_UINT32        uiShifted=0;
	WE_UINT32        *puiTmpBuf1 = NULL;
	WE_UINT32        *puiTmpBuf2 = NULL;
    
    if(!pstNumer||!pstDenom||!pstRem)
    {
        return 0;
    }
	puiTmpBuf1 = (WE_UINT32 *)WE_MALLOC((pstNumer->uiPrec + 1) * sizeof( WE_ULONG));	
	puiTmpBuf2 =  (WE_UINT32 *)WE_MALLOC((pstDenom->uiPrec + 1) * sizeof( WE_ULONG));
	if(!puiTmpBuf1||!puiTmpBuf2)
	{
		if(puiTmpBuf1)
        {
            WE_FREE(puiTmpBuf1);
        }
		if(puiTmpBuf2)
        {
            WE_FREE(puiTmpBuf2);
        }
		return 0;
	}
	
	WE_MEMSET(puiTmpBuf1,0,(pstNumer->uiPrec + 1) * sizeof( WE_ULONG));	
	WE_MEMSET(puiTmpBuf2,0,(pstDenom->uiPrec + 1) * sizeof( WE_ULONG));
    //eDiscardQuo = !pstQuo;
    uiQuoPrec = pstNumer->uiPrec - pstDenom->uiPrec + 1;
    if(pstNumer->uiPrec + 1 > MPA_TMP_PLACES) 
    {
        stNormNumber.puiDig =puiTmpBuf1;
    } 
    else 
    {
        stNormNumber.puiDig = auiNormNumberHolder;
    }
    
    if(pstDenom->uiPrec + 1 > MPA_TMP_PLACES) 
    {
        stNormDenom.puiDig = puiTmpBuf2;
    } 
    else 
    {
        stNormDenom.puiDig = auiNormDenomHolder;
    }
    uiTmp = pstDenom->puiDig[pstDenom->uiPrec - 1];
    if(!(uiTmp & 0xffff0000UL)) 
    {
        uiTmp <<= 16, uiShift = 16;
    }  
    else
    {
        uiShift = 0;
    }
    if(!(uiTmp & 0xff000000UL))
    {
        uiTmp <<= 8, uiShift += 8;
    }
    if(!(uiTmp & 0xf0000000UL))
    {
        uiTmp <<= 4, uiShift += 4;
    }
    if(!(uiTmp & 0xc0000000UL))
    {
        uiTmp <<= 2, uiShift += 2;
    }
    if(!(uiTmp & 0x80000000UL)) 
    {
        uiShift++;
    }
    stNormNumber.uiPrec = pstNumer->uiPrec + 1;
    We_LibMpaShiftL(&stNormNumber, pstNumer,(WE_UINT8) uiShift,&uiShifted);
    stNormNumber.puiDig[pstNumer->uiPrec] = uiShifted;
    stNormDenom.uiPrec = pstDenom->uiPrec + 1;
    We_LibMpaShiftL(&stNormDenom, pstDenom,(WE_UINT8) uiShift,&uiShifted);
    stNormDenom.puiDig[pstDenom->uiPrec] =uiShifted;
    stNormRem.uiPrec = stNormDenom.uiPrec;
    stNormRem.puiDig = &stNormNumber.puiDig[stNormNumber.uiPrec - stNormDenom.uiPrec];
    puiMsRem = &stNormRem.puiDig[stNormRem.uiPrec - 3];
    puiMsDenom = &stNormDenom.puiDig[stNormDenom.uiPrec - 3];
    uiQuoPlace = uiQuoPrec - 1;
    do {
        WE_ULONG uiQuoGuess=0;
        WE_ULONG uiQuo=0; 
        
        iRes = We_LibMpaGuessQuo(puiMsRem--, puiMsDenom,&uiQuoGuess);
        if(iRes)
        {
			WE_FREE(puiTmpBuf1);
			WE_FREE(puiTmpBuf2);
            return 0;
        }
        iRes = We_LibMpaMulSub(&stNormRem, &stNormDenom, uiQuoGuess,&uiQuo);
        if(iRes)
        {
            WE_FREE(puiTmpBuf1);
            WE_FREE(puiTmpBuf2);
            return 0;
        }
        if(pstQuo)
        {
            pstQuo->puiDig[uiQuoPlace] = uiQuo;
        }
    } while((--stNormRem.puiDig, uiQuoPlace--) > 0);
    
    ++stNormRem.puiDig; 
    stUnTmp.puiDig = stNormRem.puiDig;
    stUnTmp.uiPrec = stNormRem.uiPrec - 1;
    iRes= We_LibMpaShiftR(pstRem, &stUnTmp, (WE_UINT8) uiShift,&uiShifted); 
    if(iRes)
    {
        WE_FREE(puiTmpBuf1);
        WE_FREE(puiTmpBuf2);
        return 0;
    }    
    if(uiShift) 
    {
        WE_UINT32  uiIdx = stNormRem.uiPrec - 2;
        WE_ULONG  *puiPtr = stNormRem.puiDig + uiIdx;        
        pstRem->puiDig[uiIdx] = ((*puiPtr >> uiShift) | (puiPtr[1] << (MPA_LONG_BIT - uiShift)));
    }
    
    WE_FREE(puiTmpBuf1);
    WE_FREE(puiTmpBuf2);    
    (WE_VOID)WE_MEMSET(auiNormNumberHolder, 0, MPA_TMP_PLACES * sizeof(*auiNormNumberHolder));
    (WE_VOID)WE_MEMSET(auiNormNumberHolder,0xff, MPA_TMP_PLACES * sizeof(*auiNormNumberHolder));
    (WE_VOID)WE_MEMSET(auiNormDenomHolder, 0, MPA_TMP_PLACES * sizeof(*auiNormDenomHolder));
    (WE_VOID)WE_MEMSET(auiNormDenomHolder,0xff, MPA_TMP_PLACES * sizeof(*auiNormDenomHolder));
    
    return 1;
}

static WE_VOID We_LibMpaShortDiv(St_WeLibMpaNum *pstQuo,  WE_ULONG *puiRem, St_WeLibMpaNum *pstNumer,  WE_ULONG uiDenom)
{
    WE_UINT32 uiPrec=0;
    WE_ULONG  uiRemainder = 0;
    if(!puiRem||!pstNumer)
    {
        return ;
    }    
    
    for(uiPrec = pstNumer->uiPrec; uiPrec > 0; --uiPrec)
    {
        if(pstQuo)
        {
            uiRemainder = We_LibMpaSingleDiv(uiRemainder, pstNumer->puiDig[uiPrec - 1], \
                uiDenom,(pstQuo->puiDig + uiPrec - 1) );
        }
        else
        {    
            uiRemainder = We_LibMpaSingleDiv(uiRemainder, pstNumer->puiDig[uiPrec - 1], \
                uiDenom,NULL);
        }
    }
    *puiRem = uiRemainder;
}


WE_UINT32 We_LibMpaDiv(St_WeLibMpaNum *pstQuo, St_WeLibMpaNum *pstRem, St_WeLibMpaNum *pstNumer, St_WeLibMpaNum *pstDenom)
{    
    if(!pstRem||!pstNumer||!pstDenom)
    {
        return 0;
    }    
    
    if(pstDenom->uiPrec != 1)
    {
        return We_LibMpaLongDiv(pstQuo, pstRem, pstNumer, pstDenom);
    }
    else 
    {
        We_LibMpaShortDiv(pstQuo, &pstRem->puiDig[0], pstNumer, pstDenom->puiDig[0]);
        return 1;
    }
}


static WE_INT32 We_LibBbsGet(WE_VOID *pvDest, WE_UINT32 uiLen, St_WeLibPrngInstS *pstMach)
{
    WE_INT32 iRes=0;
    
    iRes = We_LibBbsGetImpl(pvDest, uiLen, pstMach);
    return iRes;
}

/*get_bbs_impl*/
static WE_INT32 We_LibBbsGetImpl(WE_VOID *pvDest, WE_UINT32 uiLen, St_WeLibPrngInstS *pstMach)
{
    St_WeLibBbsMach *pstM =NULL;
    WE_UINT32   uiLoaded =0;
    WE_UINT32   uiMask =0;
    WE_UINT8    *pucPtr =NULL;
    if(!pstMach||!pvDest)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    pstM = (St_WeLibBbsMach *)pstMach->pvState;
    uiLoaded = pstM->stX.puiDig[0];
    uiMask = (1U << pstM->uiNbits) - 1;
    pucPtr = (WE_UINT8 *)pvDest;
    
    while(uiLen--) 
    {
        while(pstM->uiPending < 8) 
        {
            if(We_LibBbsNext(pstM))
            {
                //ut_log0(PRNG_EITERF, UT_ACONTINUE, "iterate operation failed");
                return M_WE_LIB_ERR_GENERAL_ERROR;
            }
            uiLoaded = (uiLoaded << pstM->uiNbits) | (uiMask & pstM->stX.puiDig[0]);
            pstM->uiPending += pstM->uiNbits;
        }
        *pucPtr++ = (WE_UINT8) (uiLoaded >> (pstM->uiPending - 8));
        pstM->uiPending -= 8;
    }
    return M_WE_LIB_ERR_OK;
}

static WE_INT32 We_LibBbsNext(St_WeLibBbsMach *pstM)
{
    St_WeLibMpaNum stTmp={0};
    if(pstM==NULL)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    stTmp.puiDig = pstM->puiTmp;
    We_LibMpaMul(&stTmp, &pstM->stX, &pstM->stX);
    We_LibMpaNorm(&stTmp, pstM->stX.uiPrec << 1);
    if (stTmp.uiPrec < pstM->stN.uiPrec) 
    {
        (WE_VOID)WE_MEMCPY (pstM->stX.puiDig, stTmp.puiDig, stTmp.uiPrec * sizeof (WE_UINT32));
        pstM->stX.uiPrec = stTmp.uiPrec;
    } 
    else 
    {
        if (!WE_MPA_MOD(&pstM->stX, &stTmp, &pstM->stN)) 
        {
            // ut_log0(PRNG_EMODNF, UT_ACONTINUE, "modulo operation failed");
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        We_LibMpaNorm(&pstM->stX, pstM->stN.uiPrec);
    }
    return M_WE_LIB_ERR_OK;
}

/*prng_create_bbs_core*/
St_WeLibPrngInstS  * We_LibPrngCreatBbs(WE_UINT8 *pucParam)
{
    St_WeLibPrngInstS   *pstNewI=NULL;
    WE_UINT32     uiLen=0;
    WE_UINT32     uiSz=0;
    WE_UINT32     uiSzT=0; 
    WE_UINT32     uiAcc = 0;
    WE_UINT32     uiNbits=0;
    WE_INT32      iLogBits = 1;
    St_WeLibBbsMach    *pstNewD=NULL;
    WE_UINT32     uiNoct = 1;
    WE_INT32      iFreeParam = 0;
    WE_INT32      iRes=0;
    WE_INT32      iRval=0;
    
    if(!pucParam) 
    {
        pucParam = (WE_UINT8 *)WE_MALLOC(0x43);
        if (!pucParam) 
        {
            return 0;
        }
        (WE_VOID)WE_MEMCPY(pucParam, (WE_UINT8 *) 
            "\x00\x41\x01\x63\xe8\x66\x91\xce\xbb\x9b\x7c\xd4\x2a\x47\x7a"
            "\xf0\xf2\x21\xd9\xfa\x5d\xf1\x6b\x47\x4d\xd6\x85\xb2\xd1\x16"
            "\x9e\xee\x9a\x95\x4d\x5e\x8c\xe9\x69\xe8\xc2\x4c\x3a\xf2\x74"
            "\x46\x80\x44\x98\xc1\x0e\x11\x2b\x7d\x4d\xc6\xfb\x35\xbe\xdf"
            "\xe8\x10\x7d\x94\x81\x59\x9d", 0x43);
        iFreeParam = 1;
    } 
    
    uiLen = (pucParam[0] << 8) + pucParam[1];
    pucParam += 2;
    uiSz = uiLen + sizeof(WE_UINT32) - 1;
    uiSz -= uiSz % sizeof(WE_UINT32);
    
      //CHECK_FIT_IN_TYPE(uiSz / sizeof(WE_UINT32), WE_UINT32, mpa_prec)
    pstNewI = (St_WeLibPrngInstS *)WE_MALLOC(sizeof(*pstNewI));
    if(!pstNewI)
    {
        if(iFreeParam)
        {
            WE_LIB_FREE(pucParam);    
        }
        return NULL;
    }
    pstNewD = (St_WeLibBbsMach *)WE_MALLOC(sizeof(*pstNewD));
    if(!pstNewD)
    {
        WE_LIB_FREE(pstNewI);
        if(iFreeParam)
        {
            WE_LIB_FREE(pucParam);    
        }
        return NULL;
    }
    pstNewD->stN.puiDig = WE_MALLOC(uiSz);
    if(!(pstNewD->stN.puiDig))
    {
        WE_LIB_FREE(pstNewD);
        WE_LIB_FREE(pstNewI);
        if(iFreeParam)
        {
            WE_LIB_FREE(pucParam);    
        }
        return NULL;
    }
    pstNewD->stX.puiDig = WE_MALLOC(uiSz);
    
    if(!(pstNewD->stX.puiDig))
    {
        WE_LIB_FREEIF(pstNewD->stN.puiDig);
        WE_LIB_FREE(pstNewD);
        WE_LIB_FREE(pstNewI);
        if(iFreeParam)
        {
            WE_LIB_FREE(pucParam);    
        }
        return NULL;
    }

    uiSzT = uiSz * 2;
    pstNewD->puiTmp = WE_MALLOC(uiSzT);
    if(!(pstNewD->puiTmp))
    {
        WE_LIB_FREEIF(pstNewD->stX.puiDig);
        WE_LIB_FREEIF(pstNewD->stN.puiDig);
        WE_LIB_FREE(pstNewD);
        WE_LIB_FREE(pstNewI);
        if(iFreeParam)
        {
            WE_LIB_FREE(pucParam);    
        }
        return NULL;
    }
    while(uiLen--) 
    {
        if(uiNoct == 1 && *pucParam)
        {
            uiNoct = uiLen + 1;
        }
        uiAcc = (uiAcc << BIT_CHAR) + *pucParam++;
        if(!(uiLen % sizeof(WE_UINT32)))
        {
            pstNewD->stN.puiDig[uiLen / sizeof(WE_UINT32)] = uiAcc;
        }
    }
    We_LibMpaNorm(&pstNewD->stN, uiSz / sizeof(WE_UINT32));
    if(iFreeParam)
    {
        WE_UINT8 *pucTmp = pucParam - 0x43;
        WE_LIB_FREE(pucTmp);
    }
    iRes=We_LibMpaBitLen (&(pstNewD->stN),&iRval);
    if(iRes)
    {
        WE_LIB_FREE(pstNewD->stX.puiDig);
        WE_LIB_FREE(pstNewD->puiTmp);        
        WE_LIB_FREE(pstNewI);
        return NULL;
    }
    
    uiNbits = (WE_UINT32)iRval;
    while((2UL << iLogBits) <= uiNbits)
    {
        iLogBits++;
    }
    pstNewD->uiNbits = (WE_UINT32)iLogBits;
    pstNewD->uiNoct = uiNoct;
    (WE_VOID)WE_MEMSET(pstNewD->stX.puiDig, 85, sizeof(WE_UINT32) * pstNewD->stN.uiPrec);
    pstNewD->stX.puiDig[pstNewD->stN.uiPrec - 1] &= ~(~0UL << ((uiNoct % sizeof(WE_UINT32)) * BIT_CHAR));
    pstNewD->stX.uiPrec = pstNewD->stN.uiPrec;
    
    pstNewD->uiPending = 0;    
    pstNewD->iSeeded = 0;    
    /*set function point*/
//    pstNewI->piSeedFn = We_BbsSeed;
    pstNewI->piGetFn  = We_LibBbsGet;
    pstNewI->pvDestroyFn = We_LibBbsDestroy;
    pstNewI->pvState = pstNewD;
    
    return pstNewI;    

}


static WE_VOID We_LibBbsDestroy(St_WeLibPrngInstS *pstMach)
{
    St_WeLibBbsMach *pstM=NULL;
    
    if(!pstMach)
    {
        return ;
    }
    if(!(pstMach->pvState))
    {
        WE_FREE(pstMach);
        return;
    }
    pstM = (St_WeLibBbsMach *)pstMach->pvState;
    
    WE_LIB_FREEIF(pstM->stN.puiDig);
    WE_LIB_FREEIF(pstM->stX.puiDig);
    WE_LIB_FREEIF(pstM->puiTmp);
    WE_LIB_FREE(pstM);
    WE_LIB_FREE(pstMach);    
}

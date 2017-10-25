/*==================================================================================================
    HEADER NAME : we_mpa.c
    MODULE NAME : We
    
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
/*#include "we_rsacomm.h"*/

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define COMB(uiHI,uiLO) (((uiHI) << MPA_BIT_2) + (uiLO))
#define HI(x) ((x) >> MPA_BIT_2)
#define LO(x) ((x) & (((WE_ULONG) 1 << MPA_BIT_2) - 1))

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

static WE_ULONG We_MpaSingleDiv( WE_ULONG uiTHI,  WE_ULONG uiTLO,  WE_ULONG uiBLO,  WE_ULONG *puiQUO);
static WE_UINT32 We_MpalongDiv(st_MpaNum *pstQuo, st_MpaNum *pstRem, st_MpaNum *pstNumer, st_MpaNum *pstDenom);
static WE_VOID We_MpaShortDiv(st_MpaNum *pstQuo,  WE_ULONG *pstRem, st_MpaNum *pstNumer,  WE_ULONG uiDenom);
static WE_INT32 We_MpaMulSub(st_MpaNum *pstNumer, st_MpaNum *pstDenom,  WE_ULONG uiQuoGuess,WE_UINT32 *puiQuo);
/*******************************************************************************
*   Function Define Section
*******************************************************************************/ 
/*==================================================================================================
FUNCTION: 
    We_MpaSub
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstRes:
    st_MpaNum *pstOp1:
    st_MpaNum *pstOp2:
    WE_INT32 *piRval:
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
WE_INT32  We_MpaSub(st_MpaNum *pstRes, st_MpaNum *pstOp1, st_MpaNum *pstOp2,WE_INT32 *piRval)
{
    WE_ULONG       *puiRes = NULL;
    WE_ULONG       *puiOp1 = NULL;
    WE_ULONG       *puiOp2 = NULL;
    WE_ULONG       *puiEnd = NULL;
    WE_ULONG       uiOp = 0;
    WE_ULONG       uiDif = 0;
    WE_UINT32      uiBorrow = 0;
    
    if(!pstOp1||!pstOp2||!pstRes||!piRval||!pstRes->puiDig||!pstOp1->puiDig||!pstOp2->puiDig)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 
    puiRes = pstRes->puiDig;
    puiOp1 = pstOp1->puiDig;
    puiOp2 = pstOp2->puiDig;
    puiEnd = puiOp2 + pstOp2->uiPrec;
    uiOp = *(puiOp1++);
    uiDif = *(puiRes++) = uiOp - *(puiOp2++);
    uiBorrow = uiDif > uiOp;
    
    while(puiOp2 < puiEnd) 
    {
        *(puiRes++) = uiDif = (uiOp = *(puiOp1++)) - *(puiOp2++) - uiBorrow;
        if(uiDif > uiOp)
        {
            uiBorrow = 1;
        }
        else if(uiDif < uiOp)
        {
            uiBorrow = 0;
        }
    }
    if(puiOp1 < (puiEnd = pstOp1->puiDig + pstOp1->uiPrec) && uiBorrow) 
    {
        while((*(puiRes++) = (uiOp = *(puiOp1++)) - 1), !uiOp && puiOp1 < puiEnd)
        {
        }
        if(uiOp)
        {
            uiBorrow = 0;
        }
    }
    
    if(!uiBorrow) 
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
/*==================================================================================================
FUNCTION: 
    We_MpaAdd
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstRes:
    st_MpaNum *pstOp1:
    st_MpaNum *pstOp2:
    WE_INT32 *piRval:
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
WE_INT32 We_MpaAdd(st_MpaNum *pstRes, st_MpaNum *pstOp1, st_MpaNum *pstOp2,WE_INT32 *piRval)
{
    WE_ULONG     *puiRes = NULL;
    WE_ULONG     *puiOp1 = NULL;
    WE_ULONG     *puiOp2 = NULL;
    WE_ULONG     *puiEnd = NULL;
    WE_ULONG  	  uiOp = 0;
    WE_ULONG  	  uiSum = 0;
    WE_UINT32    uiCarry = 0;
    
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
        /*
        while(!(*(puiRes++) = *(puiOp1++) + 1) && puiOp1 < puiEnd)
        {
        } modify by Sam*/        
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
/*==================================================================================================
FUNCTION: 
    We_MpaMul
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstRes[OUT]:
    st_MpaNum *pstOp1[IN]:
    st_MpaNum *pstOp2[IN]:
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
WE_VOID We_MpaMul(st_MpaNum *pstRes, st_MpaNum *pstOp1, st_MpaNum *pstOp2)
{
    WE_UINT32           uiJLoop = 0;
    WE_ULONG            uiTemp = 0; 
    WE_UINT32           uiOp1Prec = 0;
    WE_UINT32           uiOp2Prec = 0;
    
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
        WE_ULONG  uiMH = HI(uiML);
        uiML = LO(uiML);
        while(puiSrc < puiEnd) 
        { 
            WE_ULONG uiL1 = *puiSrc++;
            WE_ULONG uiH1 = HI(uiL1);
            WE_ULONG uiNXT = uiCur;
            uiL1 = LO(uiL1);
            uiCur += uiL1 * uiML; 
            uiNXT = uiCur < uiNXT; 
            uiTemp = uiL1; uiL1 = uiTemp * uiMH;  
            uiNXT += uiMH * uiH1; 
            uiTemp = uiH1; uiH1 = uiTemp * uiML;  
            uiL1 += uiH1; 
            uiNXT += ( WE_ULONG) (uiL1 < uiH1) << MPA_BIT_2; 
            uiH1 = uiCur + (uiL1 << MPA_BIT_2); 
            if(uiH1 < uiCur)
            {
                uiNXT += (uiL1 >> MPA_BIT_2) + 1;
            }
            else
            {
                uiNXT += (uiL1 >> MPA_BIT_2) + 0;
            }
            /*uiNXT += (uiL1 >> MPA_BIT_2) + (uiH1 < uiCur); */
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
            /*uiCur = uiNXT + (uiCur < uiH1);*/
        }
        *puiDst = uiCur;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpaDiv
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstQuo[IN]:
    st_MpaNum *pstRem[IN]:
    st_MpaNum *pstNumer[IN]:
    st_MpaNum *pstDenom[IN]:
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
WE_UINT32 We_MpaDiv(st_MpaNum *pstQuo, st_MpaNum *pstRem, st_MpaNum *pstNumer, st_MpaNum *pstDenom)
{    
    if(!pstRem||!pstNumer||!pstDenom)
    {
        return 0;
    }    
    if(pstDenom->uiPrec != 1)
    {
        return We_MpalongDiv(pstQuo, pstRem, pstNumer, pstDenom);
    }
    else 
    {
        We_MpaShortDiv(pstQuo, &pstRem->puiDig[0], pstNumer, pstDenom->puiDig[0]);
        return 1;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpaSingleDiv
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_ULONG uiTHI[IN]:
    WE_ULONG uiTLO[IN]:
    WE_ULONG uiBLO[IN]:
    WE_ULONG *puiQuoP[IN]:
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
static WE_ULONG We_MpaSingleDiv( WE_ULONG uiTHI,  WE_ULONG uiTLO,  WE_ULONG uiBLO,  WE_ULONG *puiQuoP)
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
    else if(HI(uiBLO) == 0) 
    { 
        WE_ULONG uiTmp=0;
        uiQuo = COMB(uiTHI, HI(uiTLO));
        uiTmp = uiQuo / uiBLO;
        uiQuo %= uiBLO;
        uiQuo = COMB(uiQuo, LO(uiTLO));
        uiRem = uiQuo % uiBLO;
        uiQuo /= uiBLO;
        uiQuo = COMB(uiTmp, uiQuo);
    } 
    else
    { 
        uiQuo = 0;
        
        if(uiBLO & (MPA_DIG_ONE << (MPA_BIT - 1))) 
        {
#define STGC uiQuo <<= 1; uiRem = uiTHI + (uiTLO >> (MPA_BIT - 1)); \
    uiTLO <<= 1; if(uiTHI & (MPA_DIG_ONE << (MPA_BIT - 1))) { uiTHI += uiRem - uiBLO; uiQuo++; } \
            else if((uiTHI += uiRem) >= uiBLO) { uiTHI -= uiBLO; uiQuo++; }
            
            STGC STGC STGC STGC STGC STGC STGC STGC
            STGC STGC STGC STGC STGC STGC STGC STGC
            STGC STGC STGC STGC STGC STGC STGC STGC
            STGC STGC STGC STGC STGC STGC STGC STGC
        }
        else 
        {
#define STGS uiQuo <<= 1; uiTHI = (uiTHI << 1) + (uiTLO >> (MPA_BIT - 1)); \
            uiTLO <<= 1; if(uiBLO <= uiTHI) { uiTHI -= uiBLO; uiQuo++; }
            
            STGS STGS STGS STGS STGS STGS STGS STGS
            STGS STGS STGS STGS STGS STGS STGS STGS
            STGS STGS STGS STGS STGS STGS STGS STGS
            STGS STGS STGS STGS STGS STGS STGS STGS
        }        
        uiRem = uiTHI;
    }
    
    if(puiQuoP)
    {
        *puiQuoP = uiQuo;
    }
    return uiRem;
}
/*==================================================================================================
FUNCTION: 
    We_MpaGuessQuo
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_ULONG *puiMsNumer[IN]:
    WE_ULONG *puiMsDenom[IN]:
    WE_UINT32 *puiQuo[OUT]:
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
static WE_INT32 We_MpaGuessQuo( WE_ULONG *puiMsNumer,  WE_ULONG *puiMsDenom,WE_UINT32 *puiQuo)
{
    WE_ULONG     *puiU = NULL;
    WE_ULONG     *puiV = NULL;
    WE_ULONG     uiRem=0;
    WE_ULONG     uiQuo=0;
    WE_ULONG     uiLO=0;
    WE_ULONG     uiHI=0;
    WE_ULONG     uiT0=0;
    WE_ULONG     uiT1=0;
    WE_ULONG     uiT2=0;
    WE_ULONG     uiTemp=0; 
    
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
        uiRem = We_MpaSingleDiv(puiU[2], puiU[1], puiV[1], &uiQuo); 
    }
    
    uiT0 = LO(uiQuo);
    uiT1 = HI(uiQuo);
    uiT2 = LO(puiV[0]);
    uiHI = HI(puiV[0]);
    uiLO = uiT0 * uiT2;
    uiTemp = uiT0; uiT0 = uiTemp * uiHI; 
    uiTemp = uiT2; uiT2 = uiTemp * uiT1; 
    uiTemp = uiHI; uiHI = uiTemp * uiT1; 
    uiT0 += uiT2;
    if(uiT0 < uiT2)
    {
        uiHI += MPA_DIG_ONE << MPA_BIT_2;
    }
    uiLO += (uiT1 = uiT0 << MPA_BIT_2);
    if(uiLO < uiT1)
    {
        uiHI += 1;
    }
    uiHI += uiT0 >> MPA_BIT_2;
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
/*==================================================================================================
FUNCTION: 
    We_MpaMulSub
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstNumer[IN]:
    st_MpaNum *pstDenom[IN]:
    WE_ULONG uiQuoGuess[IN]:
    WE_UINT32 *puiQuo[OUT]:
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
static WE_INT32 We_MpaMulSub(st_MpaNum *pstNumer, st_MpaNum *pstDenom,  WE_ULONG uiQuoGuess,WE_UINT32 *puiQuo)
{
    WE_ULONG     *puiDst = NULL;
    WE_ULONG     *puiSrc = NULL;
    WE_ULONG     uiMH = 0;
    WE_ULONG     *puiEnd = NULL;
    WE_ULONG     uiML = 0;
    WE_ULONG     uiBorrow = 0;
    WE_ULONG     uiTemp=0; 
    WE_INT32     iRes=0;
    WE_INT32     iRval=0;

    if(!pstNumer||!pstDenom||!puiQuo)
    {
       return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    puiDst = pstNumer->puiDig;
    puiSrc = pstDenom->puiDig;
    uiMH = HI(uiQuoGuess);
    puiEnd = puiSrc + pstDenom->uiPrec;
    uiML = LO(uiQuoGuess);
    
    if(uiQuoGuess == 0)
    {
        *puiQuo=0;
        return M_WE_LIB_ERR_OK;
    }
    while(puiSrc < puiEnd) 
    {
        WE_ULONG uiL1 = *puiSrc++;
        WE_ULONG uiH1 = HI(uiL1);
        WE_ULONG uiNXT = uiBorrow;
        
        uiL1 = LO(uiL1);
        uiBorrow += uiL1 * uiML; 
        uiNXT = uiBorrow < uiNXT; 
        uiTemp = uiL1; uiL1 = uiTemp * uiMH;  
        uiNXT += uiMH * uiH1;      
        uiTemp = uiH1; uiH1 = uiTemp * uiML;  
        uiL1 += uiH1; 
        uiNXT += ( WE_ULONG) (uiL1 < uiH1) << MPA_BIT_2; 
        uiH1 = uiBorrow + (uiL1 << MPA_BIT_2); 
        if(uiH1 < uiBorrow)
        {
           uiNXT += (uiL1 >> MPA_BIT_2) + 1;
        }
        else
        {
           uiNXT += (uiL1 >> MPA_BIT_2) + 0;
        }    
        /*uiNXT += (uiL1 >> MPA_BIT_2) + (uiH1 < uiBorrow); */
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
        iRes=We_MpaAdd(pstNumer, pstNumer, pstDenom,&iRval);
        if(iRes)
        {
            ;
        }
        if(!iRval)
        {
            /*
            ut_assert(E_FALSE);            
            return M_WE_LIB_ERR_GENERAL_ERROR; */           
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
/*==================================================================================================
FUNCTION: 
    We_MpalongDiv
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstQuo[OUT]:
    st_MpaNum *pstRem[IN/OUT]:
    st_MpaNum *pstNumer[IN]:
    st_MpaNum *pstDenom[IN]:
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
static WE_UINT32 We_MpalongDiv(st_MpaNum *pstQuo, st_MpaNum *pstRem, st_MpaNum *pstNumer, st_MpaNum *pstDenom)
{
    WE_ULONG        auiNormNumberHolder[MPA_TEMP_PLACES]={0};
    WE_ULONG        auiNormDenomHolder[MPA_TEMP_PLACES]={0};
    WE_ULONG        uiTmp=0;
    st_MpaNum       stNormNumber={0};
    st_MpaNum       stNormDenom={0};
    st_MpaNum       stUnTmp={0};
    st_MpaNum       stNormRem={0};
    WE_UINT32       uiShift=0;
    WE_UINT32       uiQuoPrec = 0;
    WE_UINT32       uiQuoPlace=0;
    WE_ULONG        *puiMsRem=NULL;   
    WE_ULONG        *puiMsDenom=NULL;   
    WE_INT32        iRes=0;
    WE_UINT32       uiShifted=0;
	WE_UINT32       *puiTmpBuf1 = NULL;
	WE_UINT32       *puiTmpBuf2 = NULL;
    
    if(!pstNumer||!pstDenom||!pstRem)
    {
        return 0;
    }
	puiTmpBuf1 = (WE_UINT32*)WE_MALLOC((pstNumer->uiPrec + 1) * sizeof( WE_ULONG));
	puiTmpBuf2 = (WE_UINT32*)WE_MALLOC((pstDenom->uiPrec + 1) * sizeof( WE_ULONG));
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
	(WE_VOID)WE_MEMSET(puiTmpBuf1,0,(pstNumer->uiPrec + 1) * sizeof( WE_ULONG));
	(WE_VOID)WE_MEMSET(puiTmpBuf2,0,(pstDenom->uiPrec + 1) * sizeof( WE_ULONG));
    uiQuoPrec = pstNumer->uiPrec - pstDenom->uiPrec + 1;
    if(pstNumer->uiPrec + 1 > MPA_TEMP_PLACES) 
    {
        stNormNumber.puiDig = puiTmpBuf1;
    } 
    else 
    {
        stNormNumber.puiDig = auiNormNumberHolder;
    }
    
    if(pstDenom->uiPrec + 1 > MPA_TEMP_PLACES) 
    {
        stNormDenom.puiDig =puiTmpBuf2;
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
    We_MpaShiftL(&stNormNumber, pstNumer,(WE_UINT8) uiShift,&uiShifted);
    stNormNumber.puiDig[pstNumer->uiPrec] = uiShifted;
    stNormDenom.uiPrec = pstDenom->uiPrec + 1;
    We_MpaShiftL(&stNormDenom, pstDenom,(WE_UINT8) uiShift,&uiShifted);
    stNormDenom.puiDig[pstDenom->uiPrec] =uiShifted;
    stNormRem.uiPrec = stNormDenom.uiPrec;
    stNormRem.puiDig = &stNormNumber.puiDig[stNormNumber.uiPrec - stNormDenom.uiPrec];
    puiMsRem = &stNormRem.puiDig[stNormRem.uiPrec - 3];
    puiMsDenom = &stNormDenom.puiDig[stNormDenom.uiPrec - 3];
    uiQuoPlace = uiQuoPrec - 1;
    do {
        WE_ULONG uiQuoGuess=0;
        WE_ULONG uiQuo=0; 
        
        iRes = We_MpaGuessQuo(puiMsRem--, puiMsDenom,&uiQuoGuess);
        if(iRes)
        {
			WE_FREE(puiTmpBuf1);
			WE_FREE(puiTmpBuf2);
            return 0;
        }
        iRes = We_MpaMulSub(&stNormRem, &stNormDenom, uiQuoGuess,&uiQuo);
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
    iRes= We_MpaShiftR(pstRem, &stUnTmp, (WE_UINT8) uiShift,&uiShifted); 
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
        
        pstRem->puiDig[uiIdx] = ((*puiPtr >> uiShift) | (puiPtr[1] << (MPA_BIT - uiShift)));
    }
    WE_FREE(puiTmpBuf1);
    WE_FREE(puiTmpBuf2);
    
    (WE_VOID)WE_MEMSET(auiNormNumberHolder, 0, MPA_TEMP_PLACES * sizeof(*auiNormNumberHolder));
    (WE_VOID)WE_MEMSET(auiNormNumberHolder,0xff, MPA_TEMP_PLACES * sizeof(*auiNormNumberHolder));
    (WE_VOID)WE_MEMSET(auiNormDenomHolder, 0, MPA_TEMP_PLACES * sizeof(*auiNormDenomHolder));
    (WE_VOID)WE_MEMSET(auiNormDenomHolder,0xff, MPA_TEMP_PLACES * sizeof(*auiNormDenomHolder));
    
    return 1;
}
/*==================================================================================================
FUNCTION: 
    We_MpaShortDiv
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstQuo[IN]:
    WE_ULONG *puiRem[OUT]:
    st_MpaNum *pstNumer[IN]:
    WE_ULONG uiDenom[IN]:
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
static WE_VOID We_MpaShortDiv(st_MpaNum *pstQuo,  WE_ULONG *puiRem, st_MpaNum *pstNumer,  WE_ULONG uiDenom)
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
            uiRemainder = We_MpaSingleDiv(uiRemainder, pstNumer->puiDig[uiPrec - 1], \
            uiDenom,(pstQuo->puiDig + uiPrec - 1) );
        }
        else
        {    
            uiRemainder = We_MpaSingleDiv(uiRemainder, pstNumer->puiDig[uiPrec - 1], \
            uiDenom,NULL);
        }
    }
    *puiRem = uiRemainder;
}
/*==================================================================================================
FUNCTION: 
    We_MpaShiftL
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstDest[OUT]:
    st_MpaNum *pstSrc[IN]:
    WE_UINT8 ucShift[IN]:
    WE_UINT32 *puiShifted[OUT]:
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
WE_VOID We_MpaShiftL(st_MpaNum *pstDest, st_MpaNum *pstSrc, WE_UINT8 ucShift,WE_UINT32 *puiShifted)
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
    
    ucRShift = (WE_UINT8) (MPA_BIT - (ucShift %= MPA_BIT));
    
    if(!ucShift) 
    {
        if(pstSrc->puiDig != pstDest->puiDig)
        {
            (WE_VOID)WE_MEMCPY(pstDest->puiDig, pstSrc->puiDig, pstSrc->uiPrec * sizeof( WE_ULONG));
        }
        *puiShifted=0;
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
}
/*==================================================================================================
FUNCTION: 
    We_MpaShiftR
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstDest[OUT]:
    st_MpaNum *pstSrc[IN]:
    WE_UINT8 ucShift[IN]:
    WE_UINT32 *puiShifted[OUT]:
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
WE_INT32 We_MpaShiftR(st_MpaNum *pstDest, st_MpaNum *pstSrc, WE_UINT8 ucShift,WE_UINT32 *puiShifted)
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
    
    ucLShift = (WE_UINT8) (MPA_BIT - (ucShift %= MPA_BIT));
    
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
    
    *puiShifted=uiShifted;
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_MpaNorm
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstNum[IN/OUT]:
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
WE_VOID We_MpaNorm(st_MpaNum *pstNum, WE_UINT32 uiPrec)
{
    WE_ULONG *puiLastPlaceP = NULL;
    
    if(!pstNum)
    {
        return ;
    }    
    puiLastPlaceP = &pstNum->puiDig[uiPrec];
    while(*(--puiLastPlaceP) == 0 && uiPrec > 1)
    {
        --uiPrec;
    }
    
    pstNum->uiPrec = uiPrec;
}
/*==================================================================================================
FUNCTION: 
    We_MpaCmp
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    st_MpaNum *pstOp1[IN]:
    st_MpaNum *pstOp2[IN]:
    WE_INT32 *piRval[OUT]:
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
WE_VOID We_MpaCmp(st_MpaNum *pstOp1, st_MpaNum *pstOp2,WE_INT32 *piRval)
{
    if(!pstOp1||!pstOp2||!piRval)
    {
        return ;
    }    
    if(pstOp1->uiPrec != pstOp2->uiPrec)
    {
        if(pstOp1->uiPrec > pstOp2->uiPrec)
        {
            *piRval=1;
        }
        else
        {
            *piRval=-1;
        }    
        return ;
    }
    else 
    {
        WE_UINT32   uiPrec=0;
        WE_ULONG    *puiOp1Place=NULL;
        WE_ULONG    *puiOp2Place=NULL;
        
        uiPrec = pstOp1->uiPrec;
        puiOp1Place = &pstOp1->puiDig[uiPrec - 1];
        puiOp2Place = &pstOp2->puiDig[uiPrec - 1];
        while(uiPrec > 0 && *puiOp1Place == *puiOp2Place) 
        {
            --uiPrec;
            --puiOp1Place;
            --puiOp2Place;
        }
        if(uiPrec == 0)
        {
            *piRval=0;    
            return ;
        }
        else
        {
            if(*puiOp1Place > *puiOp2Place)
            {
                *piRval=1;
            }
            else
            {
                *piRval=-1;
            }
            return ;
        }
    }
}
#undef STGC
#undef STGS


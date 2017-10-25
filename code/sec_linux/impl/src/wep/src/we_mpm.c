/*==================================================================================================
    HEADER NAME : we_mpm.c
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
/*#include "we_rsacomm.h"*/

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#ifndef M 
#define M 8 
#endif 

#define POW_M (1 << M) 

#define REG_SIGN(reg_s) ((reg_s).stNormVal.uiPrec == 1 \
&& (reg_s).stNormVal.puiDig[0] == 0 ? E_MPM_POS : (reg_s).eSign)

#define REG_NEG(reg_s) (!((reg_s).stNormVal.uiPrec == 1 \
&& (reg_s).stNormVal.puiDig[0] == 0) && (reg_s).eSign == E_MPM_NEG)


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 We_MpmMulAbs(St_Reg * pstResP, St_Reg * pstOp1, St_Reg * pstOp2P,WE_INT32 *piRval);
static E_Result We_MpmModAbs(St_Reg * pstRem, St_Reg * pstOp, St_Reg * pstMod);
static WE_INT32 We_MpmIsNegArc(WE_VOID * pvArc,WE_INT32 *piRval);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/ 
/*==================================================================================================
FUNCTION: 
    We_MpmModAbs
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstRem[IN/OUT]:
    St_Reg * pstOp[IN]:
    St_Reg * pstMod[IN]:
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
static E_Result We_MpmModAbs(St_Reg * pstRem, St_Reg * pstOp, St_Reg * pstMod)
{
    WE_UINT32  uiRemPrec=0;
    WE_INT32   iRes=0;
    WE_INT32   iRval=0;
    E_Result   eRes = E_TRUE;

    if(!pstMod||!pstRem||!pstOp)
    {
       return E_FALSE;
    }
    
    iRes=We_MpmRegGrAbs(pstMod, pstOp,&iRval);
    if(iRes)
    {
        return E_FALSE;
    }
    if(iRval)
    {        
        iRes=We_MpmRegMoveAbs(pstRem, pstOp, E_TRUE,&iRval);
        if(iRes)
        {
            return E_FALSE;
        }    
        if(!iRval)
        {
            return E_FALSE;
        }
        return E_TRUE; 
    }
    
    uiRemPrec = pstMod->stNormVal.uiPrec;
    if(pstRem != pstMod) 
    {
        eRes=We_MbkRegPrep(pstRem, uiRemPrec, E_FALSE);
        if(!eRes)
        {
            if(pstRem->stNormVal.puiDig)
            {
               WE_FREE(pstRem->stNormVal.puiDig);
            }   
            return E_FALSE;
        }
    }
    iRes=(WE_INT32)MPA_MOD(&pstRem->stNormVal, &pstOp->stNormVal, &pstMod->stNormVal);
    if(!iRes)
    {
        return E_FALSE;
    }
    We_MbkRegNorm(pstRem, uiRemPrec);
    return E_TRUE;
}
/*==================================================================================================
FUNCTION: 
    We_MpmBackUp
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eOpRid[IN]:
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
WE_VOID *We_MpmBackUp(WE_VOID * pvMid, E_MpmReg eOpRid, E_Result ePreserve)
{
    St_Mach     *pstMach = NULL;
    St_Reg      *pstOpReg=NULL;
    St_Reg      *pstArcReg=NULL; 
    WE_INT32    iRes=M_WE_LIB_ERR_OK;
    WE_INT32    iRval=0;
    
    if (eOpRid >= E_MPM_NUM_REGS) 
    {
        return NULL;
    }
    if(!pvMid)
    {
        return NULL;
    }    
    pstMach = (St_Mach *)(pvMid);
    
    pstOpReg = &pstMach->stRegister[eOpRid];
    
    pstArcReg = WE_MALLOC(sizeof(St_Reg));
    if(pstArcReg!=NULL) 
    {
        (WE_VOID)WE_MEMSET(pstArcReg,0,sizeof(St_Reg));
        pstArcReg->uiAllocPrec = 0; 
        if(!REG_LOADED(*pstOpReg)) 
        { 
            pstArcReg->stNormVal.uiPrec = 0;
            return (WE_VOID *)pstArcReg;
        }
        else
        {
            E_MpmSign eArcSign = E_MPM_NEG;
            eArcSign = REG_SIGN(*pstOpReg);
            iRes=We_MpmRegMoveAbs(pstArcReg, pstOpReg, ePreserve,&iRval);
            if(iRes!=M_WE_LIB_ERR_OK)
            {
                if(pstArcReg->stNormVal.puiDig)
                {
                    WE_FREE(pstArcReg->stNormVal.puiDig);
                }
                WE_FREE(pstArcReg);
                pstArcReg=NULL;
                return NULL;
            }    
            if(!iRval)
            {
                if(pstArcReg->stNormVal.puiDig)
                {
                    WE_FREE(pstArcReg->stNormVal.puiDig);
                }
                WE_FREE(pstArcReg);
                pstArcReg=NULL;                
                return NULL; 
            } 
            else 
            {
                pstArcReg->eSign = eArcSign;
                return (WE_VOID *)pstArcReg;
            }
        }
    }
    else
    {
        return NULL; 
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpmGe
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
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
WE_INT32 We_MpmGe(WE_VOID * pvMid,WE_INT32 *piRval)
{
    St_Mach       *pstMach = NULL;
    if(!pvMid||!piRval)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    pstMach = (St_Mach  *)(pvMid);
    *piRval=pstMach->stFlag.uiZero || !pstMach->stFlag.uiNegative;
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_MpmCmp
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eOp1Rid[IN]:
    E_MpmReg eOp2Rid[IN]:
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

WE_VOID We_MpmCmp(WE_VOID * pvMid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid)
{
    St_Mach    *pstMach = NULL;
    St_Reg     *pstOp1Reg=NULL; 
    St_Reg     *pstOp2Reg=NULL; 
    WE_INT32   iRval=0;
    
    if(!pvMid)
    {
        return ;
    }
        
    pstMach = (St_Mach  *)(pvMid);
    if(eOp1Rid == eOp2Rid) 
    { 
        pstMach->stFlag.uiZero = (WE_UINT32)E_TRUE;
        pstMach->stFlag.uiNegative =(WE_UINT32) E_FALSE;
        return;
    }
    
    pstOp1Reg = &pstMach->stRegister[eOp1Rid];
    pstOp2Reg = &pstMach->stRegister[eOp2Rid];
    
    if(REG_SIGN(*pstOp1Reg) != REG_SIGN(*pstOp2Reg)) 
    {        
        pstMach->stFlag.uiZero = (WE_UINT32)E_FALSE;
        pstMach->stFlag.uiNegative = (REG_SIGN(*pstOp1Reg) == E_MPM_NEG);
        return;
    } 
    else 
    {        
        const E_MpmSign eSign = REG_SIGN(*pstOp1Reg);   
        We_MpaCmp(&pstOp1Reg->stNormVal, &pstOp2Reg->stNormVal,&iRval);
        switch(iRval) 
        {
        case 0: 
            pstMach->stFlag.uiZero = (WE_UINT32)E_TRUE;
            pstMach->stFlag.uiNegative =(WE_UINT32)E_FALSE;
            break;
        case 1: 
            pstMach->stFlag.uiZero = (WE_UINT32)E_FALSE;
            pstMach->stFlag.uiNegative = (eSign == E_MPM_NEG);
            break;
        case -1: 
            pstMach->stFlag.uiZero = (WE_UINT32)E_FALSE;
            pstMach->stFlag.uiNegative = (eSign == E_MPM_POS);
            break;
        default:
            break;
        }
        return;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpmRestore
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eOpRid[IN]:
    WE_VOID * pvArcReg[IN/OUT]:
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
WE_VOID We_MpmRestore(WE_VOID * pvMid, E_MpmReg eOpRid, WE_VOID * pvArcReg, E_Result ePreserve)
{
    St_Mach    *pstMach = NULL;
    St_Reg     *pstOpReg=NULL; 
    St_Reg     *pstArcReg=NULL; 
    WE_INT32   iRes=M_WE_LIB_ERR_OK;
    WE_INT32   iRval=0;
    
    if(!pvMid||!pvArcReg)
    {
        return;
    }
    pstMach = (St_Mach  *)(pvMid);
    pstOpReg = &pstMach->stRegister[eOpRid];
    pstArcReg = (St_Reg *)pvArcReg;
    
    if(!REG_LOADED(*pstArcReg)) 
    {
        pstOpReg->stNormVal.uiPrec = 0;
    }
    else 
    {
        E_MpmSign eOpSign = REG_SIGN(*pstArcReg);

        iRes=We_MpmRegMoveAbs(pstOpReg, pstArcReg, ePreserve,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return ;
        }    
        if(!iRval) 
        {
            pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
            return;
        }
        pstOpReg->eSign = eOpSign;
    }
    if(!ePreserve) 
    {
        if(REG_ALLOCED(*pstArcReg))
        {
            M_WE_RSA_SELF_FREE(pstArcReg->stNormVal.puiDig);
        }
        WE_FREE(pstArcReg);
        pstArcReg=NULL;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpmMulm
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eResRid[IN]:
    E_MpmReg eOp1Rid[IN]:
    E_MpmReg eOp2Rid[IN]:
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
WE_VOID We_MpmMulm(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid)
{
    St_Mach  * pstMach = NULL;

    if(!pvMid)
    {
        return ;
    }    
    pstMach = (St_Mach  *)(pvMid);   
    
    We_MpmMul(pvMid, eResRid, eOp1Rid, eOp2Rid);
    if(pstMach->stFlag.uiOverFlow)
    {
        return;
    }
    else
    {
        We_MpmMod(pvMid, eResRid, eResRid);
        We_MpmSetOn(pvMid, eResRid);
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpmMove
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eDestRid[IN]:
    E_MpmReg eSrcRid[IN]:
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
WE_VOID We_MpmMove(WE_VOID * pvMid, E_MpmReg eDestRid, E_MpmReg eSrcRid, E_Result ePreserve)
{
    St_Mach    *pstMach = NULL;
    St_Reg     *pstSrcReg=NULL;  
    St_Reg     *pstDestReg=NULL;  
    E_MpmSign  eDestSign=E_MPM_NEG;
    WE_INT32   iRes=M_WE_LIB_ERR_OK;
    WE_INT32   iRval=0;
    
    if(!pvMid)
    {
        return ;
    }    
    pstMach = (St_Mach  *)(pvMid);
    pstSrcReg = &pstMach->stRegister[eSrcRid];   
    
    if(eSrcRid == eDestRid)
    {
        return; 
    }
    pstDestReg = &pstMach->stRegister[eDestRid];
    
    eDestSign = REG_SIGN(*pstSrcReg);
    iRes=We_MpmRegMoveAbs(pstDestReg, pstSrcReg, ePreserve,&iRval);
    if(iRes!=M_WE_LIB_ERR_OK)
    {
        return ;
    }    
    if(!iRval) 
    {
        pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
        return;
    }
    pstDestReg->eSign = eDestSign;
}
/*==================================================================================================
FUNCTION: 
    We_MpmAdd
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eResRid[IN]:
    E_MpmReg eOp1Rid[IN]:
    E_MpmReg eOp2Rid[IN]:
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
WE_VOID We_MpmAdd(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid)
{
    St_Mach     *pstMach = NULL;
    St_Reg      *pstOp1Reg=NULL; 
    St_Reg      *pstOp2Reg=NULL; 
    St_Reg      *pstResReg=NULL; 
    WE_INT32    iRes=0;
    WE_INT32    iRval=0;
    
    if(!pvMid)
    {
        return;
    }
    pstMach = (St_Mach  *)(pvMid);
    pstOp1Reg = &pstMach->stRegister[eOp1Rid];
    pstOp2Reg = &pstMach->stRegister[eOp2Rid]; 
    
    pstResReg = &pstMach->stRegister[eResRid];
    if(REG_SIGN(*pstOp1Reg) == REG_SIGN(*pstOp2Reg)) 
    {
        pstResReg->eSign = REG_SIGN(*pstOp1Reg);
        if(!We_MpmAddAbs(pstResReg, pstOp1Reg, pstOp2Reg)) 
        {
            pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
            return;
        }
    } 
    else 
    {
        iRes=We_MpmRegGrAbs(pstOp1Reg, pstOp2Reg,&iRval);
        if(iRes)
        {
            return ;
        }    
        if(iRval) 
        {
            pstResReg->eSign = REG_SIGN(*pstOp1Reg);
            if(!We_MpmSubAds(pstResReg, pstOp1Reg, pstOp2Reg)) 
            {
                pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
                return;
            }
        } 
        else 
        {
            pstResReg->eSign = REG_SIGN(*pstOp2Reg);
            if(!We_MpmSubAds(pstResReg, pstOp2Reg, pstOp1Reg)) 
            {
                pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
                return;
            }
        }
    }
    pstMach->stFlag.uiZero = REG_ZERO(*pstResReg);
    pstMach->stFlag.uiNegative = REG_NEG(*pstResReg);
}
/*==================================================================================================
FUNCTION: 
    We_MpmMul
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eResRid[IN]:
    E_MpmReg eOp1Rid[IN]:
    E_MpmReg eOp2Rid[IN]:
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
WE_VOID We_MpmMul(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid)
{
    St_Mach          *pstMach = NULL;
    E_Result         eUseExtra = E_FALSE;
    St_Reg           *pstOp1Reg=NULL; 
    St_Reg           *pstOp2Reg=NULL; 
    St_Reg           *pstResReg=NULL; 
    E_MpmSign        eOp1Sign=E_MPM_NEG; 
    E_MpmSign        eOp2Sign=E_MPM_NEG; 
    WE_INT32         iRval=0;
    WE_INT32         iRes=0;
 
    if(!pvMid)
    {
        return ;
    }  
    if((eResRid == eOp1Rid)|| (eResRid == eOp2Rid))
    {
        eUseExtra=E_TRUE;
    }
    else
    {
        eUseExtra=E_FALSE;
    }
    /*eUseExtra = (eResRid == eOp1Rid || eResRid == eOp2Rid);*/
    pstMach = (St_Mach  *)(pvMid);
    pstOp1Reg = &pstMach->stRegister[eOp1Rid];
    pstOp2Reg = &pstMach->stRegister[eOp2Rid];
    
    pstResReg = eUseExtra ? &pstMach->stExtraReg : &pstMach->stRegister[eResRid];
    iRes=We_MpmMulAbs(pstResReg, pstOp1Reg, pstOp2Reg,&iRval);
    if(iRes)
    {
        return ;
    }    
    if(!iRval)
    {
        pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
        return;
    }
    
    if(eUseExtra)
    { 
        St_Reg * pstActualResReg = &pstMach->stRegister[eResRid];        
        iRes=We_MpmRegMoveAbs(pstActualResReg, pstResReg, E_FALSE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return;
        }    
        pstResReg = pstActualResReg;
    }
    
    eOp1Sign = REG_SIGN(*pstOp1Reg);
    eOp2Sign = REG_SIGN(*pstOp2Reg);
    if((eOp1Sign && eOp2Sign) || (!eOp1Sign && !eOp2Sign))
    {
        pstResReg->eSign =E_MPM_POS;
    }
    else
    {
        pstResReg->eSign =E_MPM_NEG;
    }    
    
    pstMach->stFlag.uiZero = REG_ZERO(*pstResReg);
    pstMach->stFlag.uiNegative = REG_NEG(*pstResReg);
}
/*==================================================================================================
FUNCTION: 
    We_MpmSub
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eResRid[IN]:
    E_MpmReg eOp1Rid[IN]:
    E_MpmReg eOp2Rid[IN]:
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
WE_VOID We_MpmSub(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid)
{
    St_Mach     *pstMach = NULL;
    St_Reg      *pstOp1Reg=NULL; 
    St_Reg      *pstOp2Reg=NULL; 
    St_Reg      *pstResReg=NULL; 
    WE_INT32    iRes=0;
    WE_INT32    iRval=0;    

    if(!pvMid)
    {
        return ;
    }    
    pstMach = (St_Mach *)(pvMid);
    pstOp1Reg = &pstMach->stRegister[eOp1Rid];
    pstOp2Reg = &pstMach->stRegister[eOp2Rid];   
    
    pstResReg = &pstMach->stRegister[eResRid];
    if(REG_SIGN(*pstOp1Reg) == REG_SIGN(*pstOp2Reg)) 
    {
        iRes=We_MpmRegGrAbs(pstOp1Reg, pstOp2Reg,&iRval);
        if(iRes)
        {
            return ;
        }
        if(iRval) 
        {
            pstResReg->eSign = REG_SIGN(*pstOp1Reg);
            if(!We_MpmSubAds(pstResReg, pstOp1Reg, pstOp2Reg)) 
            {
                pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
                return;
            }
        } 
        else 
        {
            if(REG_SIGN(*pstOp2Reg))
            {
               pstResReg->eSign =E_MPM_NEG;
            }
            else
            {
               pstResReg->eSign =E_MPM_POS;
            }
            //pstResReg->eSign = !REG_SIGN(*pstOp2Reg);
            if(!We_MpmSubAds(pstResReg, pstOp2Reg, pstOp1Reg)) 
            {
                pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
                return;
            }
        }
    } 
    else 
    {
        pstResReg->eSign = REG_SIGN(*pstOp1Reg);
        if(!We_MpmAddAbs(pstResReg, pstOp1Reg, pstOp2Reg)) 
        {
            pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
            return;
        }
    }
    pstMach->stFlag.uiZero = REG_ZERO(*pstResReg);
    pstMach->stFlag.uiNegative = REG_NEG(*pstResReg);
}
/*==================================================================================================
FUNCTION: 
    We_MpmMod
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eRemRid[IN]:
    E_MpmReg eOpRid[IN]:
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
WE_VOID We_MpmMod(WE_VOID * pvMid, E_MpmReg eRemRid, E_MpmReg eOpRid)
{
    St_Mach        *pstMach = NULL;
    E_Result       eUseExtra = E_FALSE;
    St_Reg         *pstOpReg=NULL; 
    St_Reg         *pstRemReg=NULL; 
    St_Reg         *pstModReg=NULL; 
    WE_INT32       iRes=0;
    WE_INT32       iRval=0;
    
    if(!pvMid)
    {
        return ;
    }  
    if((eRemRid == eOpRid) ||( eRemRid == E_MPM_RMOD))
    {
        eUseExtra=E_TRUE;
    }
    else
    {
        eUseExtra=E_FALSE;
    }
    pstMach = (St_Mach  *)(pvMid);
    pstOpReg = &pstMach->stRegister[eOpRid];
    pstModReg = &pstMach->stRegister[E_MPM_RMOD]; 
    
    pstRemReg = eUseExtra ? &pstMach->stExtraReg : &pstMach->stRegister[eRemRid];
    
    if (!(We_MbkRegPrep(pstRemReg, pstModReg->uiAllocPrec, E_FALSE))) 
    {        
        pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
        return;
    }
    
    if(REG_ZERO(*pstModReg)) 
    { 
        pstMach->stFlag.uiDivZero = (WE_UINT32)E_TRUE;
        return;
    } 
    else 
    {
        if(!We_MpmModAbs(pstRemReg, pstOpReg, pstModReg)) 
        {
            pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
            return;
        }
    }
    
    if(REG_NEG(*pstOpReg) && !REG_ZERO(*pstRemReg)) 
    {    
        iRes=We_MpaSub(&pstRemReg->stNormVal, &pstModReg->stNormVal, &pstRemReg->stNormVal,&iRval);
        if(iRes)
        {
            return ;
        }    
        We_MpaNorm(&pstRemReg->stNormVal, pstModReg->stNormVal.uiPrec);
    }
    
    if(eUseExtra) 
    { 
        St_Reg * pstActualRemRegP = &pstMach->stRegister[eRemRid];        
        iRes=We_MpmRegMoveAbs(pstActualRemRegP, pstRemReg, E_FALSE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return ;
        } 
        pstRemReg = pstActualRemRegP;
    }
    
    pstRemReg->eSign = E_MPM_POS;
    pstMach->stFlag.uiZero = REG_ZERO(*pstRemReg);
    pstMach->stFlag.uiNegative = (WE_UINT32)E_FALSE;
}
/*==================================================================================================
FUNCTION: 
    We_MpmAddAbs
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstRes[OUT]:
    St_Reg * pstOp1[IN]:
    St_Reg * pstOp2[IN]:
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
WE_INT32 We_MpmAddAbs(St_Reg * pstRes, St_Reg * pstOp1, St_Reg * pstOp2)
{
    st_MpaNum       *pstA=NULL;
    st_MpaNum       *pstB=NULL;
    E_Result        eCarry=E_FALSE;
    WE_INT32        iRes=0;
    WE_INT32        iRval=0;     
    
    if(!pstRes||!pstOp1||!pstOp2)
    {
        return (WE_INT32)E_FALSE;
    }
    if(REG_ZERO(*pstOp1))
    {         
        iRes=We_MpmRegMoveAbs(pstRes, pstOp2, E_TRUE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return (WE_INT32)E_FALSE;
        }
        return iRval;
    }
    else if(REG_ZERO(*pstOp2))
    {
        iRes= We_MpmRegMoveAbs(pstRes, pstOp1, E_TRUE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return (WE_INT32)E_FALSE;
        }
        return iRval;
    }
    
    if(pstOp1->stNormVal.uiPrec > pstOp2->stNormVal.uiPrec) 
    {
        pstA = &pstOp1->stNormVal;
        pstB = &pstOp2->stNormVal;
    } 
    else 
    {
        pstA = &pstOp2->stNormVal;
        pstB = &pstOp1->stNormVal;
    }
    
    if(!We_MbkRegPrep(pstRes, pstA->uiPrec + 1, ((pstRes == pstOp1) || (pstRes == pstOp2)) ? E_TRUE : E_FALSE))
    {
        return (WE_INT32)E_FALSE;
    }
    iRes = We_MpaAdd(&pstRes->stNormVal, pstA, pstB,&iRval);
    if(iRes)
    {
        return (WE_INT32)E_FALSE;
    }    
    if(iRval) 
    { 
        pstRes->stNormVal.uiPrec = pstA->uiPrec + 1;
        pstRes->stNormVal.puiDig[pstRes->stNormVal.uiPrec - 1] = (WE_UINT32)eCarry;
    }
    else
    {
        pstRes->stNormVal.uiPrec = pstA->uiPrec;
    }
    return (WE_INT32)E_TRUE;
}
/*==================================================================================================
FUNCTION: 
    We_MpmSubAds
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstRes[IN/OUT]:
    St_Reg * pstOp1[IN]:
    St_Reg * pstOp2[IN]:
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
WE_INT32 We_MpmSubAds(St_Reg * pstRes, St_Reg * pstOp1, St_Reg * pstOp2)
{
    //E_Result eBorrow=E_FALSE;
    WE_INT32 iRes=0;
    WE_INT32 iRval=0;    
  
    if(!pstRes||!pstOp1||!pstOp2)
    {
        return (WE_INT32)E_FALSE;
    }
    if(REG_ZERO(*pstOp2))
    {
        iRes=We_MpmRegMoveAbs(pstRes, pstOp1, E_TRUE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return (WE_INT32)E_FALSE;
        }   
        return (WE_INT32)iRval;
    }
    
    if(pstRes != pstOp1) 
    {
        if(!We_MbkRegPrep(pstRes, pstOp1->stNormVal.uiPrec, pstRes == pstOp2 ? E_TRUE : E_FALSE))
        {
            return (WE_INT32)E_FALSE;
        }
    }
    
    /*eBorrow = We_MpaSub(&pstRes->stNormVal, &pstOp1->stNormVal, &pstOp2->stNormVal);*/
    iRes=We_MpaSub(&pstRes->stNormVal, &pstOp1->stNormVal, &pstOp2->stNormVal,&iRval);
    if(iRes)
    {
        return (WE_INT32)E_FALSE;
    }    
    We_MbkRegNorm(pstRes, pstOp1->stNormVal.uiPrec);
    
    return (WE_INT32)E_TRUE;
    /*return eBorrow;*/
}
/*==================================================================================================
FUNCTION: 
    We_MpmMulAbs
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstRes[IN/OUT]:
    St_Reg * pstOp1[IN]:
    St_Reg * pstOp2[IN]:
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
static WE_INT32 We_MpmMulAbs(St_Reg * pstRes, St_Reg * pstOp1, St_Reg * pstOp2,WE_INT32 *piRval)
{
    WE_UINT32    uiResPrec=0;
    WE_INT32     iRes=0;
    WE_INT32     iRval=0;    

    if(!pstRes||!pstOp1||!pstOp2||!piRval)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    if(REG_ZERO(*pstOp1) || REG_ZERO(*pstOp2)) 
    {
        if(!We_MbkRegPrep(pstRes, 1, E_FALSE))
        {
            //return E_FALSE;
            *piRval=(WE_INT32)E_FALSE;
            return M_WE_LIB_ERR_OK;
        }
        pstRes->stNormVal.uiPrec = 1;
        pstRes->stNormVal.puiDig[0] = 0;
        //return E_TRUE;
        *piRval=(WE_INT32)E_TRUE;
        return M_WE_LIB_ERR_OK;
    } 
    else if(pstOp1->stNormVal.uiPrec == 1 && pstOp1->stNormVal.puiDig[0] == 1)
    {
        //return We_MpmRegMoveAbs(pstRes, pstOp2, E_TRUE);
        iRes=We_MpmRegMoveAbs(pstRes, pstOp2, E_TRUE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        *piRval=(WE_INT32)iRval;
        return M_WE_LIB_ERR_OK;
    }
    else if(pstOp2->stNormVal.uiPrec == 1 && pstOp2->stNormVal.puiDig[0] == 1)
    {
        iRes=We_MpmRegMoveAbs(pstRes, pstOp1, E_TRUE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        *piRval=iRval;
        return M_WE_LIB_ERR_OK;
    }
    
    uiResPrec = pstOp1->stNormVal.uiPrec + pstOp2->stNormVal.uiPrec;
    if(!We_MbkRegPrep(pstRes, uiResPrec, E_FALSE))
    {
        *piRval=(WE_INT32)E_FALSE;
        return M_WE_LIB_ERR_OK;
    }
    We_MpaMul(&pstRes->stNormVal, &pstOp1->stNormVal, &pstOp2->stNormVal);
    We_MbkRegNorm(pstRes, uiResPrec);
    *piRval=(WE_INT32)E_TRUE;
    return M_WE_LIB_ERR_OK ;
}
/*==================================================================================================
FUNCTION: 
    We_MpmBitLenArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvArc[IN]:
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
WE_UINT32 We_MpmBitLenArc(WE_VOID * pvArc)
{
    WE_UINT32      uiNbits=0;
    WE_UINT32      uiPrec=0;
    WE_ULONG         uiMsPlace=0;  
    
    uiPrec = ((St_Reg  *)pvArc)->stNormVal.uiPrec - 1;
    uiNbits = (WE_UINT32) uiPrec * sizeof(WE_ULONG) * CHAR_BIT + 1;
    uiMsPlace = ((St_Reg  *)pvArc)->stNormVal.puiDig[uiPrec];
    if(uiMsPlace & 0xffff0000UL) uiMsPlace >>= 16, uiNbits += 16;
    if(uiMsPlace & 0xff00) uiMsPlace >>= 8, uiNbits += 8;
    if(uiMsPlace & 0xf0) uiMsPlace >>= 4, uiNbits += 4;
    if(uiMsPlace & 0xc) uiMsPlace >>= 2, uiNbits += 2;
    if(uiMsPlace & 0x2) uiNbits += 1;
   
    return uiNbits;
}
/*==================================================================================================
FUNCTION: 
    We_MpmFlagSet
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmFlag eFid[IN]:
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
E_Result We_MpmFlagSet(WE_VOID * pvMid, E_MpmFlag eFid)
{
    WE_UINT32       uiRval=(WE_UINT32)E_FALSE;
    St_Mach         *pstMach = NULL;
    
    if(!pvMid)
    {
        return E_FALSE;
    } 
    pstMach = (St_Mach  *)(pvMid);
    switch((WE_INT32)eFid) 
    {
    case E_MPM_DIV_ZERO:
        uiRval = pstMach->stFlag.uiDivZero;
        break;
    case E_MPM_OVERFLOW:
        uiRval = pstMach->stFlag.uiOverFlow;
        break;
    case E_MPM_ZERO:
        uiRval = pstMach->stFlag.uiZero;
        break;
    case E_MPM_NEGATIVE:
        uiRval = pstMach->stFlag.uiNegative;
        break;
    default:
        /*out of range mpm_flag error*/      
        uiRval = (WE_UINT32)E_FALSE;
        break;
    }    
    return (E_Result)uiRval;
}
/*==================================================================================================
FUNCTION: 
    We_MpmLToMpm
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eDestRid[IN]:
    WE_LONG uiSrc:
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
WE_VOID We_MpmLToMpm(WE_VOID * pvMid, E_MpmReg eDestRid, WE_LONG uiSrc)
{
    St_Mach     *pstMach = NULL;
    St_Reg      *pstDestReg=NULL; 
  
    if(!pvMid)
    {
       return ;
    }    
    pstMach = (St_Mach  *)(pvMid);
    pstDestReg = &pstMach->stRegister[eDestRid];
    
    if(!We_MbkRegPrep(pstDestReg, (WE_UINT32 ) 1, E_FALSE)) 
    {
        pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
        return;
    }
    
    pstDestReg->stNormVal.uiPrec = (WE_UINT32 ) 1;
    pstDestReg->stNormVal.puiDig[0] = uiSrc < 0 ? (WE_ULONG) -uiSrc : (WE_ULONG) uiSrc;
    pstDestReg->eSign = uiSrc < 0 ? E_MPM_NEG : E_MPM_POS;
}
/*==================================================================================================
FUNCTION: 
    We_Mpm2Power
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eOpRid[IN]:
    WE_UINT32 uiPower2[IN]:
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
WE_VOID We_Mpm2Power(WE_VOID * pvMid, E_MpmReg eOpRid, WE_UINT32 uiPower2)
{
    St_Mach     *pstMach  = NULL;
    St_Reg      *pstReg=NULL;
    WE_ULONG    *puiPlace=NULL;
    WE_UINT32   uiWSize = CHAR_BIT * sizeof(WE_ULONG);
    WE_UINT32   uiPrec=0;
    WE_UINT32   uiIndex=0; 
   
    if(!pvMid)
    {
        return ;
    }    
    pstMach  = (St_Mach  *)(pvMid);
    pstReg = &pstMach->stRegister[eOpRid];    
    uiPrec = (WE_UINT32 ) (uiPower2 / uiWSize) + 1;
    
    if(!We_MbkRegPrep(pstReg, uiPrec, E_FALSE)) 
    {
        pstMach->stFlag.uiOverFlow = (WE_UINT32)E_TRUE;
        if(pstReg->stNormVal.puiDig)
        {
            WE_FREE(pstReg->stNormVal.puiDig);
        }    
        return;
    }
    
    puiPlace = pstReg->stNormVal.puiDig;
    for(uiIndex = uiPrec - 1; uiIndex > 0; uiIndex--)
    {
        *puiPlace++ = 0;
    }
    *puiPlace = (WE_ULONG) 1 << (uiPower2 % uiWSize);
    
    pstReg->eSign = E_MPM_POS;
    We_MbkRegNorm(pstReg, uiPrec);
}
/*==================================================================================================
FUNCTION: 
    We_MpmIs2PowArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvArc[IN]:
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
WE_INT32 We_MpmIs2PowArc(WE_VOID * pvArc,WE_INT32 *piRval)
{
    St_Reg      *pstReg = NULL;
    WE_UINT32   uiNDig = 0;
    WE_UINT32   uiInd=0;
    WE_ULONG    *puiPlace = NULL;
    WE_ULONG    uiMsd = 0;
    
    if(!pvArc||!piRval)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }    
    pstReg = (St_Reg  *)pvArc;
    uiNDig = pstReg->stNormVal.uiPrec - 1;
    puiPlace = pstReg->stNormVal.puiDig;
    uiMsd = puiPlace[uiNDig - 1];
    
    for(uiInd = 0; uiInd <= uiNDig && puiPlace[uiInd] == 0; uiInd++)
    {
        ;
    }
    
    if(uiInd == uiNDig && !(uiMsd & (uiMsd - 1))) 
    {
        //return E_TRUE;
        *piRval=(WE_UINT32)E_TRUE;
        return M_WE_LIB_ERR_OK;
    }
    //return E_FALSE;
    *piRval=(WE_UINT32)E_FALSE;
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_MpmIsNegArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvArc[IN]:
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
static WE_INT32 We_MpmIsNegArc(WE_VOID * pvArc,WE_INT32 *piRval)
{
    //E_Result eRes=E_FALSE;    
    if(!pvArc||!piRval)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 
    //eRes=E_FALSE;
    *piRval=(WE_UINT32)E_FALSE;
    
    //eRes=((St_Reg  *)pvArc)->eSign == E_MPM_NEGATIVE ? E_TRUE: E_FALSE;
    //return eRes;
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_MpmRegMoveAbs
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstDest[OUT]:
    St_Reg * pstSrc[IN]:
    E_Result ePreserve[IN]:
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
WE_INT32 We_MpmRegMoveAbs(St_Reg * pstDest, St_Reg * pstSrc, E_Result ePreserve,WE_INT32 *piRval)
{
    E_Result    eRes=E_FALSE;
 
    if(!pstDest||!pstSrc||!piRval)
    {
       return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if(ePreserve) 
    { 
       eRes=We_MbkRegPrep(pstDest, pstSrc->stNormVal.uiPrec, E_FALSE);
        if(!eRes)
        {
            //return E_FALSE;
            if(pstDest->stNormVal.puiDig)
            {
                WE_FREE(pstDest->stNormVal.puiDig);
                pstDest->stNormVal.puiDig=NULL;
            }
            *piRval=(WE_INT32)E_FALSE;
            return M_WE_LIB_ERR_OK;
        }
        if(!(pstDest->stNormVal.puiDig)||!(pstSrc->stNormVal.puiDig))
        {
            *piRval=(WE_INT32)E_FALSE;
            if(pstDest->stNormVal.puiDig)
            {
                WE_FREE(pstDest->stNormVal.puiDig);
            }
            return M_WE_LIB_ERR_OK;
        }
        pstDest->stNormVal.uiPrec = pstSrc->stNormVal.uiPrec;

        (WE_VOID)WE_MEMCPY(pstDest->stNormVal.puiDig, pstSrc->stNormVal.puiDig,\
            pstSrc->stNormVal.uiPrec * sizeof(WE_ULONG));
        //return E_TRUE;
        *piRval=(WE_INT32)E_TRUE;
        return M_WE_LIB_ERR_OK;
    }
    else
    {
        WE_UINT32   uiOldAllocPrec=0;
        WE_ULONG    *puiOldPlace = NULL;
        
        if(REG_ALLOCED(*pstDest))
        {
            uiOldAllocPrec = pstDest->uiAllocPrec;
            puiOldPlace = pstDest->stNormVal.puiDig;
        }
        else
        {
            uiOldAllocPrec = 0;
        }
        pstDest->uiAllocPrec = pstSrc->uiAllocPrec;
        pstDest->stNormVal.puiDig = pstSrc->stNormVal.puiDig;
        pstDest->stNormVal.uiPrec = pstSrc->stNormVal.uiPrec;
        
        if((pstSrc->uiAllocPrec = uiOldAllocPrec) != 0) 
        {
            pstSrc->stNormVal.puiDig = puiOldPlace;
            pstSrc->stNormVal.uiPrec = 0; 
        }
        //return E_TRUE;
        *piRval=(WE_INT32)E_TRUE;
        return M_WE_LIB_ERR_OK;
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpmSetOn
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eOpRid[IN]:
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
WE_VOID We_MpmSetOn(WE_VOID * pvMid, E_MpmReg eOpRid)
{
    St_Mach     *pstMach = NULL;
    if(!pvMid)
    {
        return ;
    }    
    pstMach = (St_Mach  *)(pvMid);
    pstMach->stFlag.uiNegative = REG_NEG(pstMach->stRegister[eOpRid]);
    pstMach->stFlag.uiZero = REG_ZERO(pstMach->stRegister[eOpRid]);
}
/*==================================================================================================
FUNCTION: 
    We_MpmBStoreArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    E_MpmSign *peDestSign[OUT]:
    WE_UINT16 *pusDestPrec[OUT]:
    WE_UCHAR *pucDestPlace[IN]:
    WE_UINT16 usMaxPrec[IN]:
    WE_VOID * pvSrc:
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
E_Result We_MpmBStoreArc(E_MpmSign *peDestSign, WE_UINT16 *pusDestPrec,
                         WE_UCHAR *pucDestPlace, WE_UINT16 usMaxPrec, WE_VOID * pvSrc)
{
    St_Reg        *pstSrcReg = NULL;
    WE_ULONG      *puiSrcPlace=NULL;
    WE_UINT16     usIIndex=0;        
    WE_UINT32     uiSrcPrec=0;    
    
    if(!pvSrc||!peDestSign||!pusDestPrec||!pucDestPlace)
    {
        return E_FALSE;
    }    
    pstSrcReg = (St_Reg  *)pvSrc;
    uiSrcPrec = pstSrcReg->stNormVal.uiPrec;
    
    *pusDestPrec = We_MbkRegMpmPlaces(pstSrcReg);
    if(*pusDestPrec == 0) 
    {
        return E_FALSE;
    }
    
    if(*pusDestPrec > usMaxPrec)
    {
        return E_FALSE; 
    }   
    
    puiSrcPlace = pstSrcReg->stNormVal.puiDig;
    pucDestPlace += *pusDestPrec;
    for(usIIndex = (WE_UINT16) uiSrcPrec; usIIndex > 0; usIIndex--) 
    {
        WE_UINT16     usBlkSz=0;
        WE_ULONG      uiHolder = *(puiSrcPlace++);
        
        usBlkSz = (WE_UINT16) (usIIndex != 1 ? REP_RATIO\
            : *pusDestPrec - ((uiSrcPrec - 1) * REP_RATIO));
        while(usBlkSz--) 
        {
            *--pucDestPlace = (WE_UINT8) uiHolder; 
            uiHolder /= BASE_MPM_DIG;
        }
    }    
    
    *peDestSign = REG_SIGN(*pstSrcReg);
    
    return E_TRUE;
}
/*==================================================================================================
FUNCTION: 
    We_MpmBloadArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    E_MpmSign eSrcSign[IN]:
    WE_UINT16 usSrcPrec[IN]:
    WE_UINT8 *pucSrcPlaces[IN]:
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
WE_VOID * We_MpmBloadArc(E_MpmSign eSrcSign, WE_UINT16 usSrcPrec, WE_UINT8 *pucSrcPlaces)
{
    WE_UINT32       uiDestPrec=0;
    WE_ULONG        *uiDestPlace=NULL;
    St_Reg          *pstDestReg = NULL;
    WE_UINT16       usIIndex=0; 

    if(!pucSrcPlaces)
    {
        return NULL;
    }    
    pstDestReg = WE_MALLOC(sizeof(St_Reg));
    if(!pstDestReg)
    {
        return NULL;
    }
    (WE_VOID)WE_MEMSET(pstDestReg,0,sizeof(St_Reg));
    pstDestReg->uiAllocPrec = 0;
    uiDestPrec = (usSrcPrec + (REP_RATIO - 1)) / REP_RATIO;
    
    if(!We_MbkRegPrep(pstDestReg, uiDestPrec, E_FALSE)) 
    {
        if(pstDestReg->stNormVal.puiDig)
        {
            WE_FREE(pstDestReg->stNormVal.puiDig);
        }    
        WE_FREE(pstDestReg);
        pstDestReg=NULL;
        /*Internal Function Failure*/
        return NULL;
    }
    
    uiDestPlace = pstDestReg->stNormVal.puiDig + uiDestPrec;
    for(usIIndex = (WE_UINT16) uiDestPrec; usIIndex > 0; usIIndex--) 
    {
        WE_UINT16     usBlkSz=0;
        WE_ULONG      uiHolder = 0;
        
        usBlkSz = (WE_UINT16) (usIIndex != uiDestPrec ? REP_RATIO
            : usSrcPrec - ((uiDestPrec - 1) * REP_RATIO));
        while(usBlkSz--)
        {
            uiHolder = uiHolder * BASE_MPM_DIG + *(pucSrcPlaces++);
        }
        *--uiDestPlace = uiHolder;
    }
    
    pstDestReg->eSign = eSrcSign;
    We_MbkRegNorm(pstDestReg, uiDestPrec);
    
    return (WE_VOID *)pstDestReg;
}
/*==================================================================================================
FUNCTION: 
    We_MpmSizeOfArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvArc[IN]:
    WE_UINT16 *pusPrec[OUT]:
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
WE_INT32 We_MpmSizeOfArc(WE_VOID * pvArc,WE_UINT16 *pusPrec)
{
    if(!pvArc||!pusPrec)
    {        
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    *pusPrec=We_MbkRegMpmPlaces((St_Reg  *)pvArc);
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_MpmCmpArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvOp1Arc[IN]:
    WE_VOID * pvOp2Arc[IN]:
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
WE_INT32 We_MpmCmpArc(WE_VOID * pvOp1Arc, WE_VOID * pvOp2Arc)
{
    St_Reg      *pstOp1Reg = NULL;
    St_Reg      *pstOp2Reg = NULL;
    E_MpmSign    eOp1Sign=E_MPM_NEG;
    WE_INT32     iRval=0;

    if(!pvOp1Arc||!pvOp2Arc)
    {
        return 0;
    }
    pstOp1Reg = (St_Reg  *)pvOp1Arc;
    pstOp2Reg = (St_Reg  *)pvOp2Arc;
    if(pstOp1Reg == pstOp2Reg) 
    {
        return 0;
    } 
    eOp1Sign = REG_SIGN(*pstOp1Reg);
    if(eOp1Sign != REG_SIGN(*pstOp2Reg))
    {
        return eOp1Sign == E_MPM_POS ? 1 : -1;
    }
    else
    {
        We_MpaCmp(&pstOp1Reg->stNormVal, &pstOp2Reg->stNormVal,&iRval);
        if(eOp1Sign == E_MPM_NEG)
        {
           return -iRval;
        }
        else
        {
           return iRval;
        }    
    }
}
/*==================================================================================================
FUNCTION: 
    We_MpmRegGrAbs
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstOp1[IN]:
    St_Reg * pstOp2[IN]:
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
WE_INT32 We_MpmRegGrAbs(St_Reg * pstOp1, St_Reg * pstOp2,WE_INT32 *piRval)
{
    WE_INT32    iRval=0;
    if(!pstOp1||!pstOp2||!piRval)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }   
    We_MpaCmp(&pstOp1->stNormVal, &pstOp2->stNormVal,&iRval);
    *piRval=(iRval == 1);
    return M_WE_LIB_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    We_MpmNewMach
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    None
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
WE_VOID *We_MpmNewMach(void)
{
    St_Mach     * pstMach=NULL;
    
    pstMach = WE_MALLOC(sizeof(St_Mach));
    if(pstMach) 
    {        
        E_MpmReg  eRid=E_MPM_R0;
        (WE_VOID)WE_MEMSET(pstMach,0,sizeof(St_Mach));        
        for(eRid = E_MPM_R0; eRid < E_MPM_NUM_REGS; eRid++)
        {
            pstMach->stRegister[eRid].uiAllocPrec = 0;
        }
        pstMach->stExtraReg.uiAllocPrec = 0;
        
        
        pstMach->stFlag.uiDivZero = (WE_UINT32)E_FALSE;
        pstMach->stFlag.uiOverFlow = (WE_UINT32)E_FALSE;
        pstMach->stFlag.uiZero = (WE_UINT32)E_FALSE;
        pstMach->stFlag.uiNegative = (WE_UINT32)E_FALSE;
    }
    
    return (WE_VOID *)(pstMach);
}
/*==================================================================================================
FUNCTION: 
    We_MpmDisposeMach
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN]:
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID We_MpmDisposeMach(WE_VOID * pvMid)
{
    St_Mach  * pstMach = NULL;
    E_MpmReg   eRid=E_MPM_R0;
    
    if(!pvMid)
    {
        return ;
    }    
    pstMach = (St_Mach  *)(pvMid);
    for(eRid = E_MPM_R0; eRid < E_MPM_NUM_REGS; eRid++)
    {
        if(REG_ALLOCED(pstMach->stRegister[eRid]))
        {
            M_WE_RSA_SELF_FREE(pstMach->stRegister[eRid].stNormVal.puiDig);
        }
    }
    if(REG_ALLOCED(pstMach->stExtraReg))
    {
        M_WE_RSA_SELF_FREE(pstMach->stExtraReg.stNormVal.puiDig);
    }
    
    M_WE_RSA_SELF_FREE(pstMach);

}
/*==================================================================================================
FUNCTION: 
    We_MpmSizeOfArc2BerInteger
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
   WE_VOID * pvNum[IN]:
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 We_MpmSizeOfArc2BerInteger(WE_VOID * pvNum)
{
    WE_UINT32        uiDigits=0;
    WE_INT32         iRes=0;
    WE_INT32         iRval=0;
   // uiDigits = ((We_MpmBitLenArc(pvNum) + (We_MpmIsNegArc(pvNum) && We_MpmIs2PowArc(pvNum) ? 7 : 8)) >> 3);
    iRes=We_MpmIsNegArc(pvNum,&iRval);
    if(iRes)
    {
       //return ;
    }
    if(iRval)
    {
        iRes=We_MpmIs2PowArc(pvNum,&iRval);
        if(iRes)
        {
        //return;
        }
        if(iRval)
        {
           uiDigits =(We_MpmBitLenArc(pvNum)+7)>>3;
        }    
    }
    else
    {
        uiDigits =(We_MpmBitLenArc(pvNum)+8)>>3;
    }    
    
    /*CHECK_FIT_IN_TYPE(digits, WE_UINT32, sec_off_t)*/
    return We_MbkAsn1SizeOfType2BerTag(ASN1_INTEGER)
        + We_MbkAsn1SizeOfSize2BerLen((WE_LONG)uiDigits) +(WE_LONG)uiDigits;

}
/*==================================================================================================
FUNCTION: 
    We_MpmBerInteger2Arc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT8 *pucSrc[IN]:
    WE_VOID * *ppvArc[IN]:
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT8  * We_MpmBerInteger2Arc(WE_UINT8 *pucSrc, WE_VOID * *ppvArc)
{
    WE_UINT32       uiType=0;
    WE_LONG         iSize=0;
    WE_UCHAR        *pucDigits=NULL;
    E_MpmSign       eSign = E_MPM_POS;
    if(!pucSrc||!ppvArc)
    {
        return NULL;
    }
    pucSrc = We_MbkAsn1BerTag2Type(pucSrc, &uiType);
    if(!pucSrc) 
    {
        /*Error parsing ASM.1 object*/
        return NULL;
    }
    if((uiType != ASN1_INTEGER) && (!(ASN_IS_CONT(uiType)))) 
    {
        /*"expected integer, got uiType #%d", uiType);*/
        return NULL;
    }
    pucSrc = WE_MBK_ASN1_BERLEN2SIZE(pucSrc, &iSize);
    if(!pucSrc) 
    {
        /*"Error parsing ASM.1 object");*/
        return NULL;
    }
    
    /*CHECK_FIT_IN_TYPE(iSize, sec_off_t, WE_UINT16);    
    CHECK_OFF_T_FITS_IN_SIZE_T(iSize);*/
    
    pucDigits = (WE_UINT8 *)WE_MALLOC((WE_UINT32)iSize);

    if(!pucDigits) 
    {
        /*"Error parsing ASM.1 object");*/
        return NULL;
    }
    (WE_VOID)WE_MEMSET(pucDigits,0,(WE_UINT32)iSize);
    (WE_VOID)WE_MEMCPY(pucDigits, pucSrc, (WE_UINT32)iSize);
    if(*pucDigits & 128) 
    { 
        WE_UINT8     ucCarry = 1;
        WE_UINT32    uiIndex = (WE_UINT32)iSize;
        while(uiIndex--)
        {
            pucDigits[uiIndex] = (WE_UINT8) (~pucDigits[uiIndex] + ucCarry);
            if(pucDigits[uiIndex]  == 0)
            {
               ucCarry &=1;
            }
            else
            {
               ucCarry &=0;
            }    
           /*ucCarry &= (pucDigits[uiIndex] = (WE_UINT8) (~pucDigits[uiIndex] + ucCarry)) == 0;*/
        }
        eSign = E_MPM_NEG;
    }
    *ppvArc = We_MpmBloadArc(eSign, (WE_UINT16) iSize, pucDigits);
    if(!(*ppvArc)) 
    {
        /*"internal failure in We_MpmBloadArc()");*/
        pucSrc = 0;
    } 
    else
    {
        pucSrc += iSize;
    }
    if(pucDigits)
    {
        WE_FREE(pucDigits);
        pucDigits=NULL;
    }
    return pucSrc;
    
}
/*==================================================================================================
FUNCTION: 
    We_MpmArc2BerInteger
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvNum[IN]:
    WE_UINT8 *pucDest[OUT]:
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT8 *We_MpmArc2BerInteger(WE_VOID * pvNum, WE_UINT8 *pucDest)
{
    WE_UINT32     uiBits=0;
    WE_UINT32     uiDigits=0;
    WE_UINT32     uiXtra=0;
    WE_UINT16     usPrec=0;
    E_MpmSign     eSign=E_MPM_NEG;
    WE_INT32      iRes=0;
    WE_INT32      iRval=0;
    
    uiBits = We_MpmBitLenArc(pvNum);
    iRes=We_MpmIsNegArc(pvNum,&iRval);
    if(iRes)
    {
        return NULL;
    }     
    if(iRval)
    {
        iRes=We_MpmIs2PowArc(pvNum,&iRval);
        if(iRes)
        {
            return NULL;
        }
        if(iRval)
        {    
            uiDigits=(uiBits+7)>>3;
        }    
    }
    else
    {
        uiDigits=(uiBits+8)>>3;
    }    
    uiXtra = (uiDigits - ((uiBits + 7) >> 3));
    
    pucDest = We_MbkAsn1Type2BerTag(ASN1_INTEGER, pucDest);
    pucDest = We_MbkAsn1Size2BerLen((WE_LONG)uiDigits, pucDest);
    if(!pucDest)
    {
        return NULL;
    }
    
    if(We_MpmBStoreArc(&eSign, &usPrec, pucDest + uiXtra, \
        (WE_UINT16) (uiDigits - uiXtra), pvNum)== E_FALSE) 
    {
        return NULL;
    }
    if(uiXtra)
    {
        *pucDest = 0;
    }
    if(eSign == E_MPM_NEG) 
    {
        WE_UINT8 ucCarry = 1;        
        for(usPrec = (WE_UINT16) uiDigits; usPrec--;)
        {
            pucDest[usPrec] = (WE_UINT8) (~pucDest[usPrec] + ucCarry);
            if(pucDest[usPrec] ==0)
            {
                ucCarry &=1;
            }
            else
            {
                ucCarry &=0;
            }
            //ucCarry &= (pucDest[usPrec] = (WE_UINT8) (~pucDest[usPrec] + ucCarry)) == 0;
        }
    }
    
    return pucDest + uiDigits;  
}

/*==================================================================================================
FUNCTION: 
    We_MpmKillArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvArcReg[IN]:
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID We_MpmKillArc(WE_VOID * pvArcReg)
{
    St_Reg      *pstArcReg=NULL; 
    
    if(!pvArcReg)
    {
        return ;
    }    
    
    pstArcReg = (St_Reg  *)pvArcReg;
    
    if(REG_ALLOCED(*pstArcReg))
    {
        M_WE_RSA_SELF_FREE(pstArcReg->stNormVal.puiDig);
    }
    WE_FREE(pstArcReg);
    pstArcReg=NULL;
}

/*==================================================================================================
FUNCTION: 
    We_MpmExpmAbs
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    St_Reg * pstRes[OUT]:
    St_Reg * pstOp[IN]:
    St_Reg * pstExp[IN]:
    St_Reg * pstMod[IN]:
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static E_Result We_MpmExpmAbs(St_Reg * pstRes, St_Reg * pstOp, St_Reg * pstExp, St_Reg * pstMod)
{
    st_MpaNum          astBk[POW_M]={0};      
    st_MpaNum          *pstIres=NULL; 
    st_MpaNum          *pstTmp=NULL;  
    WE_UINT16          usIIndex=0;          
    WE_ULONG           uiExpHolder=0;
    WE_UINT32          uiExpPlace=0;
    E_Result           eLastBitDone=E_FALSE; 
    WE_ULONG           uiBitMask=0;      
    WE_UINT32          uiModPrec = 0;
    WE_UINT32          uiOpPrec = 0; 
    WE_INT32           iRval=0;
    const WE_ULONG     uiTopBitMask = MPA_DIG_ONE << (MPA_BIT - 1);
    if(!pstRes||!pstOp||!pstExp||!pstMod)
    {
        return E_FALSE;
    }
    
    uiModPrec = pstMod->stNormVal.uiPrec;
    uiOpPrec = pstOp->stNormVal.uiPrec;
    if(!We_MbkRegPrep(pstRes, uiModPrec, E_FALSE)) 
    {
        return E_FALSE;
    }
    
    for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
    {
        astBk[usIIndex].puiDig = 0;
    }
    
    pstIres = &astBk[0];
    pstTmp = &astBk[1];
    
    pstTmp->puiDig =(WE_UINT32 *) WE_MALLOC((uiModPrec > uiOpPrec ? uiModPrec : uiOpPrec) * 2 * sizeof(WE_ULONG));
    if(pstTmp->puiDig == 0)
    {
        for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
        {
            if(astBk[usIIndex].puiDig != 0)
            {
                M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
            }
        }
       return E_FALSE;
    }
    (WE_VOID)WE_MEMSET( pstTmp->puiDig,0,(uiModPrec > uiOpPrec ? uiModPrec : uiOpPrec) * 2 * sizeof(WE_ULONG));
    pstIres->puiDig =(WE_UINT32 *) WE_MALLOC(uiModPrec * sizeof(WE_ULONG));
    if(pstIres->puiDig == 0)
    {
        for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
        {
            if(astBk[usIIndex].puiDig != 0)
            {
                M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
            }
        }
       return E_FALSE;
    }
    (WE_VOID)WE_MEMSET(pstIres->puiDig,0,uiModPrec * sizeof(WE_ULONG));
    pstIres->puiDig[0] = 1;
    pstIres->uiPrec = 1;
    

    astBk[2].puiDig = (WE_ULONG *)WE_MALLOC(uiModPrec * sizeof(WE_ULONG));
    if(astBk[2].puiDig == 0)
    {
        for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
        {
            if(astBk[usIIndex].puiDig != 0)
            {
                M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
            }
        }
        return E_FALSE;
    }
    (WE_VOID)WE_MEMSET(astBk[2].puiDig,0,uiModPrec * sizeof(WE_ULONG));
    We_MpaMul(pstTmp, &pstOp->stNormVal, &pstOp->stNormVal);
    We_MpaNorm(pstTmp, uiOpPrec * 2);
    We_MpaCmp(&pstMod->stNormVal, pstTmp,&iRval);
    if(iRval == 1) 
    {
        (WE_VOID)WE_MEMCPY(astBk[2].puiDig, pstTmp->puiDig, pstTmp->uiPrec * sizeof(WE_ULONG));
        astBk[2].uiPrec = pstTmp->uiPrec;
    }
    else 
    {
        if(!MPA_MOD(&astBk[2], pstTmp, &pstMod->stNormVal))
        {
            for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
            {
                if(astBk[usIIndex].puiDig != 0)
                {
                    M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
                }
            }
           return E_FALSE;
        }
        We_MpaNorm(&astBk[2], uiModPrec);
    }
    for(usIIndex = 3; usIIndex < POW_M; usIIndex++)
    {
        astBk[usIIndex].puiDig = (WE_ULONG *)WE_MALLOC(uiModPrec * sizeof(WE_ULONG));
        if(astBk[usIIndex].puiDig == 0)
        {
            for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
            {
                if(astBk[usIIndex].puiDig != 0)
                {
                    M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
                }
            }
            return E_FALSE;
        }
        (WE_VOID)WE_MEMSET(astBk[usIIndex].puiDig,0,uiModPrec * sizeof(WE_ULONG));
        We_MpaMul(pstTmp, &astBk[usIIndex - 1], &pstOp->stNormVal);
        We_MpaNorm(pstTmp, astBk[usIIndex - 1].uiPrec + uiOpPrec);
        We_MpaCmp(&pstMod->stNormVal, pstTmp,&iRval);
        if(iRval == 1) 
        {
            (WE_VOID)WE_MEMCPY(astBk[usIIndex].puiDig, pstTmp->puiDig, pstTmp->uiPrec * sizeof(WE_ULONG));
            astBk[usIIndex].uiPrec = pstTmp->uiPrec;
        } 
        else
        {
            if(!MPA_MOD(&astBk[usIIndex], pstTmp, &pstMod->stNormVal))
            {
                for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
                {
                    if(astBk[usIIndex].puiDig != 0)
                    {
                        M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
                    }
                }
              return E_FALSE;
            }
            We_MpaNorm(&astBk[usIIndex], uiModPrec);
        }
    }    
    uiExpPlace = pstExp->stNormVal.uiPrec - 1;
    uiExpHolder = pstExp->stNormVal.puiDig[uiExpPlace];
    uiBitMask = uiTopBitMask;
    eLastBitDone = E_FALSE;
    while(!eLastBitDone)
    {
        WE_UINT16 usZeroBits;
        WE_UINT16 usMBits = 0; 
        WE_UINT32 uiThisExp = 0;        
        usZeroBits = 0;
        while((uiBitMask & uiExpHolder) == 0)
        {
            usZeroBits++;
            if((uiBitMask >>= 1) == 0) 
            { 
                if(uiExpPlace == 0) 
                { 
                    eLastBitDone = E_TRUE;
                    break;
                } 
                else 
                {
                    uiExpPlace--;
                    uiExpHolder = pstExp->stNormVal.puiDig[uiExpPlace];
                    uiBitMask = uiTopBitMask;
                }
            }
        }        
        if(!eLastBitDone) 
        { 
            while(usMBits < M) 
            {
                usMBits++;
                uiThisExp <<= 1;
                if((uiBitMask & uiExpHolder) != 0) 
                {
                    uiThisExp++;
                }
                if((uiBitMask >>= 1) == 0) 
                { 
                    if(uiExpPlace == 0) 
                    { 
                        eLastBitDone = E_TRUE;
                        break;
                    }
                    else 
                    {
                        uiExpPlace--;
                        uiExpHolder = pstExp->stNormVal.puiDig[uiExpPlace];
                        uiBitMask = uiTopBitMask;
                    }
                }
            }
        }       
        
        for(usIIndex = usZeroBits + usMBits; usIIndex > 0; usIIndex--)
        {
            We_MpaMul(pstTmp, pstIres, pstIres);
            We_MpaNorm(pstTmp, pstIres->uiPrec * 2);
            We_MpaCmp(&pstMod->stNormVal, pstTmp,&iRval);
            if(iRval == 1) 
            {
                (WE_VOID)WE_MEMCPY(pstIres->puiDig, pstTmp->puiDig, pstTmp->uiPrec * sizeof(WE_ULONG));
                pstIres->uiPrec = pstTmp->uiPrec;
            } 
            else 
            {
                if(!MPA_MOD(pstIres, pstTmp, &pstMod->stNormVal))
                {
                    for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
                    {
                        if(astBk[usIIndex].puiDig != 0)
                        {
                            M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
                        }
                    }
                   return E_FALSE;
                }
                We_MpaNorm(pstIres, uiModPrec);
            }
        }
        
        if(usMBits > 0) 
        {
            if(uiThisExp == 1) 
            {
                We_MpaMul(pstTmp, pstIres, &pstOp->stNormVal); 
                We_MpaNorm(pstTmp, pstIres->uiPrec + uiOpPrec);
            } 
            else 
            { 
                We_MpaMul(pstTmp, pstIres, &astBk[(WE_UINT32) uiThisExp]);
                We_MpaNorm(pstTmp, pstIres->uiPrec + astBk[(WE_UINT32) uiThisExp].uiPrec);
            }
            We_MpaCmp(&pstMod->stNormVal, pstTmp,&iRval);
            if(iRval== 1) 
            {
                (WE_VOID)WE_MEMCPY(pstIres->puiDig, pstTmp->puiDig, pstTmp->uiPrec * sizeof(WE_ULONG));
                pstIres->uiPrec = pstTmp->uiPrec;
            } 
            else 
            {
                if(!MPA_MOD(pstIres, pstTmp, &pstMod->stNormVal))
                {
                    for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
                    {
                        if(astBk[usIIndex].puiDig != 0)
                        {
                            M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
                        }
                    }
                    return E_FALSE;
                }
                We_MpaNorm(pstIres, uiModPrec);
            }
        }
    }    
    
    (WE_VOID)WE_MEMCPY(pstRes->stNormVal.puiDig, pstIres->puiDig, pstIres->uiPrec * sizeof(WE_ULONG));
    We_MbkRegNorm(pstRes, pstIres->uiPrec);    
    
    for(usIIndex = 0; usIIndex < POW_M; usIIndex++)
    {
        if(astBk[usIIndex].puiDig != 0)
        {
            M_WE_RSA_SELF_FREE(astBk[usIIndex].puiDig);
        }
    }
    return E_TRUE; 
}

#undef M
#undef POW_M
/*==================================================================================================
FUNCTION: 
    We_MpmExpm
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_VOID * pvMid[IN/OUT]:
    E_MpmReg eResRid[IN]:
    E_MpmReg eOpRid[IN]:
    E_MpmReg eExpRid[IN]:
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_VOID We_MpmExpm(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOpRid, E_MpmReg eExpRid)
{
    St_Mach         *pstMach = NULL;
    WE_UINT32       eUseExtra = (eResRid == eOpRid || eResRid == eExpRid || eResRid == E_MPM_RMOD);
    St_Reg          *pstOpReg=NULL; 
    St_Reg          *pstExpReg=NULL; 
    St_Reg          *pstResReg=NULL; 
    St_Reg          *pstModReg=NULL; 
    E_MpmSign       eOpSign=E_MPM_NEG; 
    E_MpmSign       eModSign=E_MPM_NEG; 
    WE_INT32        iRes=0;
    WE_INT32        iRval=0;    
 
    if(!pvMid)
    {
        return ;
    }    
    pstMach = (St_Mach  *)(pvMid);
    
    pstOpReg = &pstMach->stRegister[eOpRid];
    pstExpReg = &pstMach->stRegister[eExpRid];
    pstModReg = &pstMach->stRegister[E_MPM_RMOD]; 
  
    pstResReg = eUseExtra ? &pstMach->stExtraReg : &pstMach->stRegister[eResRid];
    if(REG_ZERO(*pstModReg)) 
    { 
        pstMach->stFlag.uiDivZero = TRUE;
        return;
    } 
    else 
    {
        if(!We_MpmExpmAbs(pstResReg, pstOpReg, pstExpReg, pstModReg)) 
        {
            pstMach->stFlag.uiOverFlow = TRUE;
            return;
        }
    }
    
    if(eUseExtra) 
    { 
        St_Reg * pstActualResReg = &pstMach->stRegister[eResRid];
        
        iRes= We_MpmRegMoveAbs(pstActualResReg, pstResReg, E_FALSE,&iRval);
        if(iRes!=M_WE_LIB_ERR_OK)
        {
            return ;
        }    
        pstResReg = pstActualResReg;
    }
    
    eOpSign = REG_SIGN(*pstOpReg);
    eModSign = REG_SIGN(*pstModReg);
    pstResReg->eSign = (eOpSign && eModSign) || (!eOpSign && !eModSign) ? E_MPM_POS : E_MPM_NEG;
    
    pstMach->stFlag.uiZero = REG_ZERO(*pstResReg);
    pstMach->stFlag.uiNegative = REG_NEG(*pstResReg);
}
/*==================================================================================================
FUNCTION: 
    We_MpmLoadArc
CREATE DATE:
    2006-12-07
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    E_MpmSign eSrcSign[IN]:
    WE_UINT16 usSrcPrec[IN]:
    WE_UCHAR *pucSrcPlaces[OUT]:
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
WE_VOID *We_MpmLoadArc(E_MpmSign eSrcSign, WE_UINT16 usSrcPrec, WE_UCHAR *pucSrcPlaces)
{
    WE_UINT32    uiDestPrec=0;
    WE_UINT32    *uiDestPlace=NULL;
    St_Reg       *pstDestReg = NULL;
    WE_UINT16    usIIndex=0; 
    WE_UINT16    usJIndex=0;     
  
    if(!pucSrcPlaces)
    {
        return NULL;
    }    
    pstDestReg = WE_MALLOC(sizeof(St_Reg));
    if(!pstDestReg)
    {
        return NULL;
    }
    (WE_VOID)WE_MEMSET(pstDestReg,0,sizeof(St_Reg));   
    pstDestReg->uiAllocPrec = 0;
    uiDestPrec = (usSrcPrec + (REP_RATIO - 1)) / REP_RATIO;
    
    if(!We_MbkRegPrep(pstDestReg, uiDestPrec, E_FALSE)) 
    {
        WE_FREE(pstDestReg);
        pstDestReg=NULL;
        /*Internal Function Failure*/
        return 0;
    } 
    uiDestPlace = pstDestReg->stNormVal.puiDig;
    for(usIIndex = (WE_UINT16) uiDestPrec; usIIndex > 0; usIIndex--) 
    {
        WE_UINT16       usBlkSz=0;
        WE_UCHAR        *pucSrcBlk=NULL;
        WE_ULONG        uiHolder=0;
        
        usBlkSz = (WE_UINT16) (usIIndex != 1 ? REP_RATIO: usSrcPrec - ((uiDestPrec - 1) * REP_RATIO));
        pucSrcBlk = pucSrcPlaces += usBlkSz;
        uiHolder = 0;
        for(usJIndex = usBlkSz; usJIndex > 0; usJIndex--)
        {
            uiHolder = uiHolder * BASE_MPM_DIG + *(--pucSrcBlk);
        }
        *uiDestPlace++ = uiHolder;
    }   
    
    pstDestReg->eSign = eSrcSign;
    We_MbkRegNorm(pstDestReg, uiDestPrec);
    
    return (WE_VOID *)pstDestReg;
}


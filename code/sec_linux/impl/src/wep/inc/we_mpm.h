/*==================================================================================================
    HEADER NAME : we_mpm.h
    MODULE NAME : WE
    
    PRE-INCLUDE FILES DESCRIPTION
        we_def.h
        
    GENERAL DESCRIPTION
        In this file,define the function prototype 
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-11-28      tang             None         Init
    
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/

#ifndef _MPM_H 
#define _MPM_H  

/***************************************************************************************************
* Macro Define Section
***************************************************************************************************/
#define REG_BLOCK_SIZE 9
#define MPM_DIG_BITS 8  

#define REG_ALLOCED(stReg) ((stReg).uiAllocPrec != 0)

#define REG_LOADED(stReg) (((stReg).uiAllocPrec != 0) \
&& ((stReg).stNormVal.uiPrec != 0))

#define REG_ZERO(stReg) (((stReg).stNormVal.uiPrec == 1) \
&& ((stReg).stNormVal.puiDig[0] == 0))

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef enum tagE_MpmReg
{
    E_MPM_R0, 
    E_MPM_R1,
    E_MPM_R2, 
    E_MPM_R3, 
    E_MPM_R4, 
    E_MPM_R5, 
    E_MPM_R6,         
    E_MPM_R7, E_MPM_RMOD = E_MPM_R7,          
    E_MPM_NUM_REGS                    
} E_MpmReg;

typedef struct tagSt_MpaNum
{
    WE_UINT32   uiPrec;     
    WE_UINT32   *puiDig; 
} st_MpaNum;

typedef enum tagE_MpmFlag
{
    E_MPM_D, E_MPM_DIV_ZERO = E_MPM_D,     
    E_MPM_O, E_MPM_OVERFLOW = E_MPM_O,     
    E_MPM_Z, E_MPM_ZERO     = E_MPM_Z,     
    E_MPM_N, E_MPM_NEGATIVE = E_MPM_N,     
    E_MPM_NUM_FLAGS                
} E_MpmFlag;

typedef enum tagE_MpmSign
{
    E_MPM_NEG, 
    E_MPM_POS
} E_MpmSign;

typedef struct tagSt_Reg
{    
    E_MpmSign eSign;  
    st_MpaNum  stNormVal; 
    WE_UINT32 uiAllocPrec;  
} St_Reg;

typedef struct tagSt_Flag
{
    WE_UINT32 uiDivZero;
    WE_UINT32 uiOverFlow;
    WE_UINT32 uiZero;
    WE_UINT32 uiNegative;
} St_Flag;

typedef struct tagSt_Mach
{
    St_Reg  stRegister[E_MPM_NUM_REGS]; 
    St_Flag stFlag;                     
    St_Reg  stExtraReg;   
} St_Mach;                 

#define REP_RATIO ((unsigned) sizeof( unsigned long))
#define BASE_MPM_DIG (( unsigned long) (1 << MPM_DIG_BITS))

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
WE_VOID *We_MpmLoadArc(E_MpmSign eSrcSign, WE_UINT16 usSrcPrec, WE_UCHAR *pucSrcPlaces);

WE_VOID * We_MpmBloadArc(E_MpmSign eSrcSign, WE_UINT16 usSrcPrec, WE_UINT8 *pucSrcPlaces);

E_Result We_MpmBStoreArc(E_MpmSign *peDestSign, WE_UINT16 *pusDestPrec,
                           WE_UCHAR *pucDestPlace, WE_UINT16 usMaxPrec,  WE_VOID * pvSrc);

WE_VOID We_MpmLToMpm(WE_VOID * pvMid, E_MpmReg eDestRid, WE_LONG uiSrc);

WE_VOID We_MpmMove(WE_VOID * pvMid, E_MpmReg eDestRid, 
                   E_MpmReg eSrcRid, E_Result ePreserve);

WE_VOID *We_MpmBackUp(WE_VOID * pvMid, E_MpmReg eOpRid, E_Result ePreserve);

WE_VOID We_MpmRestore(WE_VOID * pvMid, E_MpmReg eOpRid,
                      WE_VOID * pvArcReg, E_Result ePreserve);
WE_VOID We_MpmKillArc(WE_VOID * pvArcReg);
E_Result We_MpmFlagSet(WE_VOID * pvMid, E_MpmFlag eFid);

WE_VOID We_MpmAdd(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid);
WE_VOID We_MpmSub(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid);
WE_VOID We_MpmMul(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid);


WE_VOID We_MpmMod(WE_VOID * pvMid, E_MpmReg eRemRid, E_MpmReg eOpRid);
WE_VOID We_MpmMulm(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid);
WE_VOID We_MpmExpm(WE_VOID * pvMid, E_MpmReg eResRid, E_MpmReg eOpRid, E_MpmReg eExpRid);

WE_VOID   We_MpmCmp(WE_VOID * pvMid, E_MpmReg eOp1Rid, E_MpmReg eOp2Rid);
WE_INT32  We_MpmCmpArc(WE_VOID * pvOp1Arc, WE_VOID * pvOp2Arc);
WE_VOID   We_MpmSetOn(WE_VOID * pvMid, E_MpmReg eOpRid);
WE_INT32  We_MpmGe(WE_VOID * pvMid,WE_INT32 *piRval);

WE_INT32  We_MpmSizeOfArc(WE_VOID * pvArc,WE_UINT16 *pusPrec);
WE_UINT32 We_MpmBitLenArc(WE_VOID * pvArc);
WE_INT32  We_MpmIs2PowArc(WE_VOID * pvArc,WE_INT32 *piRval);
WE_VOID   We_Mpm2Power(WE_VOID * pvMid, E_MpmReg eOpRid, WE_UINT32 uiPower2);
WE_VOID   We_MpmAbs(WE_VOID * pvMid, E_MpmReg eOpRid);
WE_INT32  We_MpmAddAbs(St_Reg * pstRes, St_Reg * pstOp1, St_Reg * pstOp2);
WE_INT32  We_MpmSubAds(St_Reg * pstRes, St_Reg * pstOp1, St_Reg * pstOp2);
WE_INT32  We_MpmSizeOfArc2BerInteger(WE_VOID * pvNum);

WE_UINT8  *We_MpmArc2BerInteger(WE_VOID * pvNum, WE_UINT8 *pucDest);
WE_UINT8  *We_MpmBerInteger2Arc(WE_UINT8 *pucSrc, WE_VOID * *ppvArc);
WE_VOID   *We_MpmNewMach(WE_VOID);   

WE_VOID   We_MpmDisposeMach(WE_VOID * pvMid) ;
WE_INT32  We_MpmRegMoveAbs(St_Reg * pstDest, St_Reg * pstSrc, E_Result ePreserve,WE_INT32 *piRval);
WE_INT32  We_MpmRegGrAbs(St_Reg * pstOp1, St_Reg * pstOp2,WE_INT32 *piRval);


#endif /*_MPM_H*/


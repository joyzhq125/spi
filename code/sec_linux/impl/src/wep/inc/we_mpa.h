/*==================================================================================================
    HEADER NAME : we_mpa.h
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
    2006-12-5     tang            None         
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef _MPA_H 
#define _MPA_H  

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#ifndef CHAR_BIT
#define CHAR_BIT    8
#endif

#define MPA_LOG_BIT 5
#define MPA_DIG_ONE 1UL
#define MPA_DIG_FMT "l"
#define MPA_DIG_FMT_WIDTH "08"

#define MPA_LOG_BIT_2 (MPA_LOG_BIT - 1)


#define MPA_BIT (sizeof(WE_UINT32) * CHAR_BIT)
#define MPA_BIT_2 (MPA_BIT >> 1)

#define MPA_MASK_BIT (MPA_BIT - 1)
#define MPA_MASK_BIT_2 (MPA_BIT_2 - 1)

#define MPA_TEMP_PLACES (256)

#define MPA_MOD(pstRem, pstNumer, pstDenom) We_MpaDiv(0, pstRem, pstNumer, pstDenom)

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_VOID We_MpaNorm(st_MpaNum *pstNum, WE_UINT32 uiPrec);

WE_VOID We_MpaCmp(st_MpaNum *pstOp1, st_MpaNum *pstOp2,WE_INT32 *piRval);

WE_INT32 We_MpaAdd(st_MpaNum *pstRes, st_MpaNum *pstOp1, 
                   st_MpaNum *pstOp2,WE_INT32 *piRval);

WE_INT32 We_MpaSub(st_MpaNum *pstRes, st_MpaNum *pstOp1, 
                   st_MpaNum *pstOp2,WE_INT32 *piRval);

WE_VOID We_MpaMul(st_MpaNum *pstRes, st_MpaNum *pstOp1, st_MpaNum *pstOp2);

WE_UINT32 We_MpaDiv(st_MpaNum *pstQuo, st_MpaNum *pstRem,
                    st_MpaNum *pstNumer, st_MpaNum *pstDenom);

WE_VOID We_MpaShiftL(st_MpaNum *pstDest, st_MpaNum *pstSrc,
                     WE_UINT8 ucShift,WE_UINT32 *puiShifted);

WE_INT32 We_MpaShiftR(st_MpaNum *pstDest, st_MpaNum *pstSrc, 
                      WE_UINT8 ucShift,WE_UINT32 *puiShifted);


#endif /*_MPA_H*/


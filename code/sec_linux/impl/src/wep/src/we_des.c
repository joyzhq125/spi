/*=====================================================================================
    FILE   NAME : we_descore.c
    MODULE NAME : WE
    
    GENERAL DESCRIPTION
        The functions in this file manage the certificate(x509 or wtls).

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History

    Modification                   Tracking
    Date              Author       Number       Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-11-03         Tang         None          Init

=====================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "we_def.h"
#include "we_libalg.h"
#include "we_mem.h"
#include "we_des.h"

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

static WE_UINT8 *We_LibDesSchedule(WE_UINT8 *pucKey, 
                                   E_WeBsapiModeOp eOp,  
                                   WE_UINT8 *pucSched);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibDesF
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Transform operation and get round key.
ARGUMENTS PASSED:
    uiK[IN]: ;
    pucSubKey[IN]: ;
RETURN VALUE:
    
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_UINT32 We_LibDesF(WE_UINT32 uiK, WE_UINT8 *pucSubKey)
{
  WE_UINT32 uiRval=0;
  static const WE_UINT32 auiSp[8][64] = {
    /*sp1*/
    {0x808200, 0x0, 0x8000, 0x808202, 0x808002,
     0x8202, 0x2, 0x8000, 0x200, 0x808200, 0x808202, 0x200, 0x800202, 0x808002,
     0x800000, 0x2, 0x202, 0x800200, 0x800200, 0x8200, 0x8200, 0x808000,
     0x808000, 0x800202, 0x8002, 0x800002, 0x800002, 0x8002, 0x0, 0x202,
     0x8202, 0x800000, 0x8000, 0x808202, 0x2, 0x808000, 0x808200, 0x800000,
     0x800000, 0x200, 0x808002, 0x8000, 0x8200, 0x800002, 0x200, 0x2, 0x800202,
     0x8202, 0x808202, 0x8002, 0x808000, 0x800202, 0x800002, 0x202, 0x8202,
     0x808200, 0x202, 0x800200, 0x800200, 0x0, 0x8002, 0x8200, 0x0, 0x808002},
   /*sp2*/
    {0x40084010, 0x40004000, 0x4000, 0x84010, 0x80000, 0x10, 0x40080010,
     0x40004010, 0x40000010, 0x40084010, 0x40084000, 0x40000000, 0x40004000,
     0x80000, 0x10, 0x40080010, 0x84000, 0x80010, 0x40004010, 0x0, 0x40000000,
     0x4000, 0x84010, 0x40080000, 0x80010, 0x40000010, 0x0, 0x84000, 0x4010,
     0x40084000, 0x40080000, 0x4010, 0x0, 0x84010, 0x40080010, 0x80000,
     0x40004010, 0x40080000, 0x40084000, 0x4000, 0x40080000, 0x40004000, 0x10,
     0x40084010, 0x84010, 0x10, 0x4000, 0x40000000, 0x4010, 0x40084000,
     0x80000, 0x40000010, 0x80010, 0x40004010, 0x40000010, 0x80010, 0x84000,
     0x0, 0x40004000, 0x4010, 0x40000000, 0x40080010, 0x40084010, 0x84000},
   /*sp3*/
    {0x104, 0x4010100, 0x0, 0x4010004, 0x4000100, 0x0, 0x10104, 0x4000100,
     0x10004, 0x4000004, 0x4000004, 0x10000, 0x4010104, 0x10004, 0x4010000,
     0x104, 0x4000000, 0x4, 0x4010100, 0x100, 0x10100, 0x4010000, 0x4010004,
     0x10104, 0x4000104, 0x10100, 0x10000, 0x4000104, 0x4, 0x4010104, 0x100,
     0x4000000, 0x4010100, 0x4000000, 0x10004, 0x104, 0x10000, 0x4010100,
     0x4000100, 0x0, 0x100, 0x10004, 0x4010104, 0x4000100, 0x4000004, 0x100,
     0x0, 0x4010004, 0x4000104, 0x10000, 0x4000000, 0x4010104, 0x4, 0x10104,
     0x10100, 0x4000004, 0x4010000, 0x4000104, 0x104, 0x4010000, 0x10104, 0x4,
     0x4010004, 0x10100},
    /*sp4*/
    {0x80401000, 0x80001040, 0x80001040, 0x40, 0x401040, 0x80400040,
     0x80400000, 0x80001000, 0x0, 0x401000, 0x401000, 0x80401040, 0x80000040,
     0x0, 0x400040, 0x80400000, 0x80000000, 0x1000, 0x400000, 0x80401000, 0x40,
     0x400000, 0x80001000, 0x1040, 0x80400040, 0x80000000, 0x1040, 0x400040,
     0x1000, 0x401040, 0x80401040, 0x80000040, 0x400040, 0x80400000, 0x401000,
     0x80401040, 0x80000040, 0x0, 0x0, 0x401000, 0x1040, 0x400040, 0x80400040,
     0x80000000, 0x80401000, 0x80001040, 0x80001040, 0x40, 0x80401040,
     0x80000040, 0x80000000, 0x1000, 0x80400000, 0x80001000, 0x401040,
     0x80400040, 0x80001000, 0x1040, 0x400000, 0x80401000, 0x40, 0x400000,
     0x1000, 0x401040},
    /*sp5*/
    {0x80, 0x1040080, 0x1040000, 0x21000080, 0x40000, 0x80, 0x20000000,
     0x1040000, 0x20040080, 0x40000, 0x1000080, 0x20040080, 0x21000080,
     0x21040000, 0x40080, 0x20000000, 0x1000000, 0x20040000, 0x20040000, 0x0,
     0x20000080, 0x21040080, 0x21040080, 0x1000080, 0x21040000, 0x20000080,
     0x0, 0x21000000, 0x1040080, 0x1000000, 0x21000000, 0x40080, 0x40000,
     0x21000080, 0x80, 0x1000000, 0x20000000, 0x1040000, 0x21000080,
     0x20040080, 0x1000080, 0x20000000, 0x21040000, 0x1040080, 0x20040080,
     0x80, 0x1000000, 0x21040000, 0x21040080, 0x40080, 0x21000000, 0x21040080,
     0x1040000, 0x0, 0x20040000, 0x21000000, 0x40080, 0x1000080, 0x20000080,
     0x40000, 0x0, 0x20040000, 0x1040080, 0x20000080},
    /*sp6*/ 
    {0x10000008, 0x10200000, 0x2000, 0x10202008, 0x10200000, 0x8, 0x10202008,
     0x200000, 0x10002000, 0x202008, 0x200000, 0x10000008, 0x200008,
     0x10002000, 0x10000000, 0x2008, 0x0, 0x200008, 0x10002008, 0x2000,
     0x202000, 0x10002008, 0x8, 0x10200008, 0x10200008, 0x0, 0x202008,
     0x10202000, 0x2008, 0x202000, 0x10202000, 0x10000000, 0x10002000, 0x8,
     0x10200008, 0x202000, 0x10202008, 0x200000, 0x2008, 0x10000008, 0x200000,
     0x10002000, 0x10000000, 0x2008, 0x10000008, 0x10202008, 0x202000,
     0x10200000, 0x202008, 0x10202000, 0x0, 0x10200008, 0x8, 0x2000,
     0x10200000, 0x202008, 0x2000, 0x200008, 0x10002008, 0x0, 0x10202000,
     0x10000000, 0x200008, 0x10002008},
    /*sp7*/
    {0x100000, 0x2100001, 0x2000401, 0x0, 0x400, 0x2000401, 0x100401,
     0x2100400, 0x2100401, 0x100000, 0x0, 0x2000001, 0x1, 0x2000000, 0x2100001,
     0x401, 0x2000400, 0x100401, 0x100001, 0x2000400, 0x2000001, 0x2100000,
     0x2100400, 0x100001, 0x2100000, 0x400, 0x401, 0x2100401, 0x100400, 0x1,
     0x2000000, 0x100400, 0x2000000, 0x100400, 0x100000, 0x2000401, 0x2000401,
     0x2100001, 0x2100001, 0x1, 0x100001, 0x2000000, 0x2000400, 0x100000,
     0x2100400, 0x401, 0x100401, 0x2100400, 0x401, 0x2000001, 0x2100401,
     0x2100000, 0x100400, 0x0, 0x1, 0x2100401, 0x0, 0x100401, 0x2100000, 0x400,
     0x2000001, 0x2000400, 0x400, 0x100001},
    /*sp8*/
    {0x8000820, 0x800, 0x20000, 0x8020820, 0x8000000, 0x8000820, 0x20,
     0x8000000, 0x20020, 0x8020000, 0x8020820, 0x20800, 0x8020800, 0x20820,
     0x800, 0x20, 0x8020000, 0x8000020, 0x8000800, 0x820, 0x20800, 0x20020,
     0x8020020, 0x8020800, 0x820, 0x0, 0x0, 0x8020020, 0x8000020, 0x8000800,
     0x20820, 0x20000, 0x20820, 0x20000, 0x8020800, 0x800, 0x20, 0x8020020,
     0x800, 0x20820, 0x8000800, 0x20, 0x8000020, 0x8020000, 0x8020020,
     0x8000000, 0x20000, 0x8000820, 0x0, 0x8020820, 0x20020, 0x8000020,
     0x8020000, 0x8000800, 0x8000820, 0x0, 0x8020820, 0x20800, 0x20800, 0x820,
     0x820, 0x20020, 0x8000000, 0x8020800}
    };
  
  if(!pucSubKey)
  {
      return M_WE_LIB_ERR_INVALID_PARAMETER;
  }
  uiRval = auiSp[0][(((uiK >> 27) | (uiK << 5)) & 63) ^ *pucSubKey++];
  uiRval |= auiSp[1][((uiK >> 23) & 63) ^ *pucSubKey++];
  uiRval |= auiSp[2][((uiK >> 19) & 63) ^ *pucSubKey++];
  uiRval |= auiSp[3][((uiK >> 15) & 63) ^ *pucSubKey++];
  uiRval |= auiSp[4][((uiK >> 11) & 63) ^ *pucSubKey++];
  uiRval |= auiSp[5][((uiK >> 7) & 63) ^ *pucSubKey++];
  uiRval |= auiSp[6][((uiK >> 3) & 63) ^ *pucSubKey++];
  uiRval |= auiSp[7][(((uiK << 1) | (uiK >> 31)) & 63) ^ *pucSubKey++];

  return uiRval;
}

/*=====================================================================================
FUNCTION: 
    We_LibDesFPerm
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    puiData[IN]: ;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibDesFPerm(WE_UINT32 *puiData)
{
    WE_UINT32  uiHi = 0;
    WE_UINT32  uiLo = 0;
    WE_UINT32 uiT=0;
    if(!puiData)
    {        
        return ;        
    }

    uiHi = puiData[0];
    uiLo = puiData[1];
    
    uiT = (uiLo ^ (uiHi >> 4)) & 0xf0f0f0f; uiLo ^= uiT; uiHi ^= uiT << 4;
    uiT = (uiLo ^ (uiLo >> 18)) & 0x3333; uiLo ^= uiT + (uiT << 18);
    uiT = (uiHi ^ (uiHi >> 18)) & 0x3333; uiHi ^= uiT + (uiT << 18);
    uiT = (uiLo ^ (uiLo >> 9)) & 0x550055; uiLo ^= uiT + (uiT << 9);
    uiT = (uiHi ^ (uiHi >> 9)) & 0x550055; uiHi ^= uiT + (uiT << 9);
    uiT = (uiLo ^ (uiLo >> 6)) & 0x3030303; uiLo ^= uiT + (uiT << 6);
    uiT = (uiHi ^ (uiHi >> 6)) & 0x3030303; uiHi ^= uiT + (uiT << 6);
    uiT = (uiLo ^ (uiLo >> 3)) & 0x11111111; uiLo ^= uiT + (uiT << 3);
    uiT = (uiHi ^ (uiHi >> 3)) & 0x11111111; uiHi ^= uiT + (uiT << 3);
    
    puiData[0] = uiHi;
    puiData[1] = uiLo;
}

/*=====================================================================================
FUNCTION: 
    We_LibDesIPerm
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    puiData[IN]: ;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibDesIPerm(WE_UINT32 *puiData)
{
  WE_UINT32  uiHi = 0;
  WE_UINT32  uiLo = 0;
  WE_UINT32 uiT=0;

  if (NULL == puiData)
  {
    return;
  }
  uiHi = puiData[0];
  uiLo = puiData[1];

  uiT = (uiLo ^ (uiHi >> 1)) & 0x55555555; uiLo ^= uiT; uiHi ^= uiT << 1;
  uiT = (uiLo ^ (uiLo >> 12)) & 0xf0f0; uiLo ^= uiT + (uiT << 12);
  uiT = (uiHi ^ (uiHi >> 12)) & 0xf0f0; uiHi ^= uiT + (uiT << 12);
  uiT = (uiLo ^ (uiLo >> 6)) & 0xcc00cc; uiLo ^= uiT + (uiT << 6);
  uiT = (uiHi ^ (uiHi >> 6)) & 0xcc00cc; uiHi ^= uiT + (uiT << 6);
  uiT = (uiLo ^ (uiLo >> 3)) & 0xa0a0a0a; uiLo ^= uiT + (uiT << 3);
  uiT = (uiHi ^ (uiHi >> 3)) & 0xa0a0a0a; uiHi ^= uiT + (uiT << 3);
  uiT = (uiLo ^ (uiLo >> 3)) & 0x11111111; uiLo ^= uiT + (uiT << 3);
  uiT = (uiHi ^ (uiHi >> 3)) & 0x11111111; uiHi ^= uiT + (uiT << 3);

  puiData[0] = uiHi;
  puiData[1] = uiLo;
}

/*=====================================================================================
FUNCTION: 
    We_LibDesSchedule
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    pucKey[IN]: data to be decrypt;
    eOp[IN]: length of data to be decrypt;
    eKeyType[IN]: decrypted data;
    pucSched[OUT]: length of decrypted data.
RETURN VALUE:
    
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
static WE_UINT8 *We_LibDesSchedule
(
    WE_UINT8 *pucKey, 
    E_WeBsapiModeOp eOp, 
    WE_UINT8 *pucSched
)
{
    WE_UINT32 uiKhi=0;
    WE_UINT32 uiKlo=0;
    WE_UINT32 uiC=0;
    WE_UINT32 uiD=0;
    WE_UINT32 iIndex = 16;
    
    static WE_INT32 aiShifts[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
    
    CHECK_FOR_NULL_ARG(pucKey, 0);

    if (!pucSched)
    {
        pucSched = (WE_UINT8 *)WE_MALLOC(128 * sizeof(WE_UINT8));
        if (!pucSched)
        {
            return 0;
        }
    }
    
    uiKlo = (WE_UINT32) pucKey[0] + (((WE_UINT32) pucKey[1]) << 8)\
            + (((WE_UINT32) pucKey[2]) << 16)+ (((WE_UINT32) pucKey[3]) << 24);
    uiKhi = (WE_UINT32) pucKey[4] + (((WE_UINT32) pucKey[5]) << 8) 
           + (((WE_UINT32) pucKey[6]) << 16)+ (((WE_UINT32) pucKey[7]) << 24);
    
    uiD   = (uiKhi & 0xf0f0f0f) + ((uiKlo & 0xf0f0f0f) << 4);
    uiKlo = (uiKlo & 0xf0f0f0f0) + ((uiKhi >> 4) & 0xf0f0f0f);
    uiKhi = (uiD >> 24) + ((uiD >> 8) & 0xff00) + ((uiD & 0xff00) << 8) + (uiD << 24);
    uiC   = (uiKlo ^ (uiKlo >> 9)) & 0x550055; uiKlo ^= uiC + (uiC << 9);
    uiD   = (uiKhi ^ (uiKhi >> 7)) & 0xaa00aa; uiKhi ^= uiD + (uiD << 7);
    uiC   = (uiKlo ^ (uiKlo >> 18)) & 0x3333; uiKlo ^= uiC + (uiC << 18);
    uiD   = (uiKhi ^ (uiKhi >> 14)) & 0xcccc; uiKhi ^= uiD + (uiD << 14);
    uiD   = (uiKhi >> 8) + ((uiKlo >> 4) & 0xf000000);
    uiC   = uiKlo & 0xfffffff;
    
    while(iIndex--)
    {
        WE_INT32  iL = aiShifts[iIndex];
        WE_INT32  iR = 28 - iL ;
        WE_UINT8  *p = pucSched + ((eOp == BSAPI_ENCRYPT ? iIndex : 15 - iIndex) << 3);
        
        *p++ = (WE_UINT8) (((uiC >> 8) & 32) | ((uiC >> 12) & 16) | ((uiC >> 7) & 8)\
               | ((uiC >> 21) & 4) | ((uiC << 1) & 2) | ((uiC >> 4) & 1));
        *p++ = (WE_UINT8) (((uiC << 3) & 32) | ((uiC >> 23) & 16) | ((uiC >> 11) & 8)\
               | ((uiC >> 3) & 4) | ((uiC >> 19) & 2) | ((uiC >> 9) & 1));
        *p++ = (WE_UINT8) (((uiC >> 17) & 32) | ((uiC >> 14) & 16) | ((uiC >> 8) & 8)\
               | ((uiC >> 1) & 4) | ((uiC >> 24) & 2) | ((uiC >> 7) & 1));
        *p++ = (WE_UINT8) (((uiC >> 10) & 32) | ((uiC >> 2) & 16) | ((uiC >> 23) & 8)\
               | ((uiC >> 17) & 4) | ((uiC >> 11) & 2) | ((uiC >> 1) & 1));
        *p++ = (WE_UINT8) (((uiD >> 7) & 32) | ((uiD >> 19) & 16) | ((uiD << 1) & 8)\
               | ((uiD >> 6) & 4) | ((uiD >> 17) & 2) | ((uiD >> 26) & 1));
        *p++ = (WE_UINT8) (((uiD << 4) & 32) | ((uiD >> 7) & 16) | ((uiD >> 19) & 8)\
               | ((uiD >> 14) & 4) | ((uiD >> 3) & 2) | ((uiD >> 19) & 1));
        *p++ = (WE_UINT8) (((uiD >> 10) & 32) | ((uiD >> 16) & 16) | ((uiD >> 7) & 8)\
               | ((uiD >> 25) & 4) | ((uiD >> 4) & 2) | ((uiD >> 24) & 1));
        *p   = (WE_UINT8) (((uiD >> 12) & 32) | ((uiD >> 9) & 16) | ((uiD >> 18) & 8)\
               | ((uiD >> 5) & 4) | ((uiD << 1) & 2) | ((uiD >> 3) & 1));
        uiC = (uiC << iL ) | (uiC >> iR);
        uiD = (uiD << iL ) | (uiD >> iR);
    }
    
    return pucSched;
}

/*=====================================================================================
FUNCTION: 
    We_LibDesDes1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    puiData[IN]: data to be decrypt;
    pavKs[IN]: length of data to be decrypt;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibDesDes1(WE_UINT32 *puiData, WE_VOID *pavKs[])
{
    WE_UINT8  *pucPtr=NULL;
    WE_UINT8  *pucEnd=NULL;
    WE_UINT32  uiH = 0;
    WE_UINT32  uiL = 0;

    if (NULL == puiData||NULL == pavKs)
    {
        return;
    }
    uiH = puiData[1];
    uiL = puiData[0];
    
    /*round=8*2*/
    for(pucPtr = pavKs[0], pucEnd = pucPtr + 128; pucPtr < pucEnd; pucPtr += 16)
    {
        uiL ^= We_LibDesF(uiH, pucPtr),uiH ^= We_LibDesF(uiL, pucPtr + 8);
    }
    
    puiData[0] = uiH;
    puiData[1] = uiL;
}

/*=====================================================================================
FUNCTION: 
    We_LibDesDes1P
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    puiData[IN]: data to be decrypt;
    pavKs[IN]: length of data to be decrypt;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibDesDes1P(WE_UINT32 *puiData, WE_VOID *pavKs[])
{
    We_LibDesIPerm(puiData);
    We_LibDesDes1(puiData, pavKs);
    We_LibDesFPerm(puiData);
}

/*=====================================================================================
FUNCTION: 
    We_LibDesDes3
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    puiData[IN]: data to be decrypt;
    pavKs[IN]: length of data to be decrypt;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibDesDes3(WE_UINT32 *puiData, WE_VOID *pavKs[])
{
    We_LibBlock3(We_LibDesDes1, puiData, pavKs);
}

/*=====================================================================================
FUNCTION: 
    We_LibDesDes3P
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    puiData[IN]: data to be decrypt;
    pavKs[IN]: length of data to be decrypt;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibDesDes3P(WE_UINT32 *puiData, WE_VOID *pavKs[])
{
    We_LibDesIPerm(puiData);
    We_LibDesDes3(puiData, pavKs);
    We_LibDesFPerm(puiData);
}

/*=====================================================================================
FUNCTION: 
    We_LibStartDesCore
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Start des encrypt.
ARGUMENTS PASSED:
    pstMode[IN]: mode type;
    pstKeys[IN]: key parameter;
    pHandle[OUT]: data handle;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibStartDesCore
(
    St_WeLibBsapiMode *pstMode, 
    St_WeCipherCrptKey *pstKey, 
    WE_HANDLE *phandle
)
{
    St_WeBlockMAtchS     *pstNew=NULL;
    WE_UINT8 aucKeyTmp[KEYSIZE] = {0};
    
    CHECK_FOR_NULL_ARG(pstMode, 0x08);
    CHECK_FOR_NULL_ARG(pstKey, 0x08);
    CHECK_FOR_NULL_ARG(pstKey->pucKey, 0x08);
    CHECK_FOR_NULL_ARG(pstKey->pucIv, 0x08);    
    CHECK_FOR_NULL_ARG(phandle, 0x08);
    
    pstNew = WE_MALLOC(sizeof(St_WeBlockMAtchS));
    if (!pstNew)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    (WE_VOID)WE_MEMSET(pstNew, 0, sizeof(St_WeBlockMAtchS));
    pstNew->eOp = pstMode->eOp;    
    
    /* key1 */
    (WE_VOID)WE_MEMCPY(aucKeyTmp, pstKey->pucKey, KEYSIZE);
    pstNew->pvRoundKey[0] = We_LibDesSchedule(aucKeyTmp, pstNew->eOp,0);
    if(! pstNew->pvRoundKey[0])
    {       
        WE_LIB_FREE(pstNew);
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    
    /*3des*/    
    if(pstMode->eTriple == BSAPI_TRIPLE && pstKey->iKeyLen!=KEYSIZE) 
    {
        E_WeBsapiModeOp eOp2;
        eOp2 =  (pstMode->eOp == BSAPI_ENCRYPT ? BSAPI_DECRYPT : BSAPI_ENCRYPT);
        /*key1 */
        (WE_VOID)WE_MEMCPY(aucKeyTmp, pstKey->pucKey + KEYSIZE, KEYSIZE);
        pstNew->pvRoundKey[1] = We_LibDesSchedule(aucKeyTmp, eOp2, 0);
        if(!pstNew->pvRoundKey[1])
        {            
            WE_LIB_FREEIF(pstNew->pvRoundKey[0]);
            WE_LIB_FREE(pstNew);
            return M_WE_LIB_ERR_GENERAL_ERROR;
        }
        
        /*key2 */
        if(pstKey->iKeyLen == 16)/*k1=k3*/
        {
            pstNew->pvRoundKey[2] = pstNew->pvRoundKey[0];
        }
        else if (pstKey->iKeyLen == 24)
        {
            (WE_VOID)WE_MEMCPY(aucKeyTmp, pstKey->pucKey + 2*KEYSIZE, KEYSIZE);
            pstNew->pvRoundKey[2] = We_LibDesSchedule(aucKeyTmp, pstMode->eOp, 0);
            if(!pstNew->pvRoundKey[2])
            {                
                WE_LIB_FREE(pstNew->pvRoundKey[1]);
                WE_LIB_FREE(pstNew->pvRoundKey[0]);
                WE_LIB_FREE(pstNew);
                return M_WE_LIB_ERR_GENERAL_ERROR;
            }            
            if(pstMode->eOp == BSAPI_DECRYPT) 
            {
                WE_UINT8 *tmp = pstNew->pvRoundKey[0];
                pstNew->pvRoundKey[0] = pstNew->pvRoundKey[2];
                pstNew->pvRoundKey[2] = tmp;
            }
        }
        else
        {
            WE_LIB_FREE(pstNew->pvRoundKey[1]);
            WE_LIB_FREE(pstNew->pvRoundKey[0]);
            WE_LIB_FREE(pstNew);
            return M_WE_LIB_ERR_KEY_LENGTH;
        }
        pstNew->uiType= (WE_UINT32)E_WE_ALG_CIPHER_3DES_CBC;
    }
    /*des*/
    else
    {
        pstNew->pvRoundKey[1] = pstNew->pvRoundKey[2] = 0;
        pstNew->uiType = (WE_UINT32)E_WE_ALG_CIPHER_DES_CBC;
    }
    if (pstKey->pucIv)
    {
        WE_UINT8 *iv = pstKey->pucIv;                    
        pstNew->iv[0][0] = (WE_UINT32) iv[3] + (((WE_UINT32) iv[2]) << 8)+ (((WE_UINT32) iv[1]) << 16) + (((WE_UINT32) iv[0]) << 24);
        pstNew->iv[0][1] = (WE_UINT32) iv[7] + (((WE_UINT32) iv[6]) << 8)+ (((WE_UINT32) iv[5]) << 16) + (((WE_UINT32) iv[4]) << 24);
    }    
    
    pstNew->fn = We_LibBlockCBC;   
    
    if(pstMode->eTriple == BSAPI_TRIPLE)
    {
        pstNew->corefn =We_LibDesDes3P;
    }
    else
    {
        pstNew->corefn =We_LibDesDes1P;
    }   
    
    pstNew->iBufLen = 0;
    (WE_VOID)WE_MEMSET(pstNew->aucData8, 0, sizeof(pstNew->aucData8));       
    *phandle=(WE_VOID *)pstNew;
    
    return M_WE_LIB_ERR_OK;    
}


/*=====================================================================================
FUNCTION: 
    We_LibAppendDes
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    pucData[IN]: data to be decrypt;
    uiLength[IN]: length of data to be decrypt;
    pucOut[OUT]: decrypted data;
    pHandle[IN]: length of decrypted data.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_INT32 We_LibAppendDes
(
    WE_UINT8 *pucData,
    WE_UINT32 uiLength, 
    WE_UCHAR* pucOut, 
    WE_VOID * pHandle
)
{
    return We_LibAppendBlock(pucData, uiLength,pucOut, pHandle);
}

/*=====================================================================================
FUNCTION: 
    We_LibKillDes
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Free memory.
ARGUMENTS PASSED:
    hDESHandle[IN]: data handle.
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibKillDes(WE_HANDLE hDESHandle)
{
    St_WeBlockMAtchS *pstMach = NULL;

    if (!hDESHandle)
    {
        return;
    }
    pstMach = (St_WeBlockMAtchS *)hDESHandle;

    if (pstMach->pvRoundKey[2] && pstMach->pvRoundKey[2] != pstMach->pvRoundKey[0])
    {
        WE_LIB_FREE(pstMach->pvRoundKey[2]);
    }
    if (pstMach->pvRoundKey[1])
    {
        WE_LIB_FREE(pstMach->pvRoundKey[1]);
    }
    WE_LIB_FREEIF(pstMach->pvRoundKey[0]);
    WE_LIB_FREE(pstMach);
}


/*=====================================================================================
FUNCTION: 
    We_LibAppendBlock
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    pucIn[IN]: data to be decrypt;
    uiLength[IN]: length of data to be decrypt;
    pucOut[OUT]: decrypted data;
    handle[IN]: length of decrypted data.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
/* from block.c */
WE_INT32 We_LibAppendBlock
(
    WE_UINT8 *pucIn, 
    WE_UINT32 uiLength, 
    WE_UCHAR* pucOut, 
    WE_VOID * handle
)
{
    WE_INT32 iResult=0;
    St_WeBlockMAtchS *pstM =NULL;   
    if(!pucIn||!uiLength||!pucOut||!handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    pstM=(St_WeBlockMAtchS *) handle; 
    We_LibBlockCBC(pstM, uiLength,pucIn,pucOut);
    return iResult;
}

/*=====================================================================================
FUNCTION: 
    We_LibBlockCBC
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    pstM[IN]: data to be decrypt;
    uiLen[IN]: length of data to be decrypt;
    pucIn[OUT]: length of decrypted data.
    pucOut[OUT]: decrypted data;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibBlockCBC
(
    St_WeBlockMAtchS * pstM, 
    WE_UINT32 uiLen, 
    WE_UINT8 *pucIn, 
    WE_UINT8 *pucOut
)
{
    
    WE_INT32      iEncryting =0; 
    WE_UINT32     uiTemLen = 0;

    if(!pstM||!uiLen||!pucIn||!pucOut)
    {
        return ;
    }

    iEncryting = pstM->eOp == BSAPI_ENCRYPT;
    uiTemLen = uiLen;
    
    while(uiTemLen >= 8) 
    {
        WE_UINT32 auiData[2]={0};
        WE_UINT32 auiTmp[2]={0};
        auiData[0] = ((((((WE_UINT32) pucIn[0] << 8) + pucIn[1]) << 8) + pucIn[2]) << 8) + pucIn[3];
        auiData[1] = ((((((WE_UINT32) pucIn[4] << 8) + pucIn[5]) << 8) + pucIn[6]) << 8) + pucIn[7];
        if(iEncryting)
        {
            auiData[0] ^= pstM->iv[0][0], auiData[1] ^= pstM->iv[0][1];
        }
        else
        {
            auiTmp[0] = pstM->iv[0][0], auiTmp[1] = pstM->iv[0][1];
            pstM->iv[0][0] = auiData[0], pstM->iv[0][1] = auiData[1];
        }
        pucIn += 8;
        pstM->corefn(auiData, pstM->pvRoundKey + 0);
        if(iEncryting)
        {
            pstM->iv[0][0] = auiData[0], pstM->iv[0][1] = auiData[1];
        }
        else
        {
            auiData[0] ^= auiTmp[0], auiData[1] ^= auiTmp[1];
        }
        pucOut[0] = (WE_UINT8) (auiData[0] >> 24);
        pucOut[1] = (WE_UINT8) (auiData[0] >> 16);
        pucOut[2] = (WE_UINT8) (auiData[0] >> 8);
        pucOut[3] = (WE_UINT8) auiData[0];
        pucOut[4] = (WE_UINT8) (auiData[1] >> 24);
        pucOut[5] = (WE_UINT8) (auiData[1] >> 16);
        pucOut[6] = (WE_UINT8) (auiData[1] >> 8);
        pucOut[7] = (WE_UINT8) auiData[1];
        pucOut += 8;
        uiTemLen -= 8;
    }
    iEncryting ^= 1;
    pucIn = (pucOut -= uiLen - uiTemLen);
    
}

/*=====================================================================================
FUNCTION: 
    We_LibBlock3
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    
ARGUMENTS PASSED:
    core[IN]: function pointer;
    puiData[IN]: length of data to be decrypt;
    pavKs[IN]: decrypted data;
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
=====================================================================================*/
WE_VOID We_LibBlock3(dcfn core, WE_UINT32 *puiData, WE_VOID *pavKs[])
{
    if(!core||!puiData||!pavKs)
    {
        return ;
    }
    core(puiData, pavKs++);
    core(puiData, pavKs++);
    core(puiData, pavKs);
}

/***************************************************************************************************
*   DES Encrypt                                                                    
***************************************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibDesEncrypt
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set sub encrypt key and encrypt data with DES algorithm.
ARGUMENTS PASSED:
    iRounds[IN]: total rounds.
    stKey[IN]: cipher key
    pucIn[IN]: data to be encrypted;
    uiInLen[IN]: length of data to be encrypted;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
/* No Padding */
WE_INT32 We_LibDesEncrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL; 
    WE_INT32 iResult=0;
    WE_UINT32 uiLen=0;   
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);
    if (!stKey.pucIv||!stKey.pucKey||stKey.iIvLen!= BLOCKSIZE
        ||stKey.iKeyLen!= KEYSIZE)
    {
        return M_WE_LIB_ERR_INVALID_KEY;
    }
    stMd.eOp = BSAPI_ENCRYPT;
    
    iResult= (WE_INT32)We_LibStartDesCore(&stMd,&stKey,&handle); 
    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
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
    if(uiInLen-uiLen>0)
    {
        iResult = We_LibAppendDes((WE_UCHAR*)pucIn, uiInLen-uiLen,pucOut,handle);
    }
    if(iResult!=0)
    {
        return iResult;
    }
    if(uiLen)
    {
        WE_UCHAR aucData[8] = {0};
        (WE_VOID)WE_MEMSET(aucData,0,8);
        (WE_VOID)WE_MEMCPY(aucData,(pucIn + uiInLen - uiLen),uiLen);   
        if(uiInLen==uiLen)
        {
            iResult=We_LibAppendDes((WE_UCHAR*)aucData, 8,pucOut,handle);
        }
        else
        {
            iResult=We_LibAppendDes((WE_UCHAR*)aucData, 8,(pucOut+ uiInLen - uiLen),handle);
        }
    }    
    We_LibKillDes(handle);
    return M_WE_LIB_ERR_OK;
}


/*=====================================================================================
FUNCTION: 
    We_LibDesEncryptInit
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt init with DES algorithm.
ARGUMENTS PASSED:
    stKey[IO]: cipher key
    pHandle[OUT]: data handle;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_LibDesEncryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};
    
    if(NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_ENCRYPT;
    iResult= (WE_INT32)We_LibStartDesCore(&stMd,&stKey,pHandle);

    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_LibDesEnCryptUpdate
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt data with DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be encrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_LibDesEnCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    WE_UINT32 uiLen = 0; 
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen)||!puiOutLen)
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
    if(0 == uiLen)
    {        
        iResult=We_LibAppendDes((WE_UCHAR *)pucIn, uiInLen,pucOut, handle);  
    }
    else 
    {
        return M_WE_LIB_ERR_GENERAL_ERROR;
    }
    
    return iResult;    
}


/*=====================================================================================
FUNCTION: 
    We_LibDesEnCryptFinal
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Free the data handle.
ARGUMENTS PASSED:
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_LibDesEnCryptFinal(WE_HANDLE handle) 
{
    WE_INT32 iResult=0;    
    if(NULL==handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    We_LibKillDes(handle);
    
    return iResult;    
}


/*=====================================================================================
FUNCTION: 
    We_LibDesEncrypt1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set sub encrypt key and padding data, encrypt data with DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pucIn[IN]: data to be encrypted;
    uiInLen[IN]: length of data to be encrypted;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
/* RFC2630 Padding */
WE_INT32 We_LibDesEncrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL;
    WE_INT32 iResult=0;
    WE_UINT32 uiLen=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};
    
    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);
    
    stMd.eOp = BSAPI_ENCRYPT;
    iResult= We_LibStartDesCore(&stMd,&stKey,&handle);
    
    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
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
    if(uiInLen-uiLen>0)
    {
        iResult=We_LibAppendDes((WE_UCHAR*)pucIn, uiInLen-uiLen,pucOut,handle);
    }
    if(iResult!=0)
    {
        return iResult;
    }
    if(uiLen)
    {
        WE_UCHAR aucData[8] = {0};
        (WE_VOID)WE_MEMSET(aucData,8-(WE_INT32)uiLen,8);
        (WE_VOID)WE_MEMCPY(aucData,(pucIn + uiInLen - uiLen),uiLen);   
        if(uiInLen==uiLen)
        {
            iResult=We_LibAppendDes((WE_UCHAR*)aucData, 8,pucOut,handle);
        }
        else
        {
            iResult=We_LibAppendDes((WE_UCHAR*)aucData, 8,(pucOut+ uiInLen - uiLen),handle);
        }
    }    
    We_LibKillDes(handle);
    return M_WE_LIB_ERR_OK;
    
}


/*=====================================================================================
FUNCTION: 
    We_LibDesEncryptInit1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Encrypt init with DES algorithm.
ARGUMENTS PASSED:
    iRounds[IN]: total rounds.
    stKey[IN]: cipher key;
    pHandle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_LibDesEncryptInit1
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    if(NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_ENCRYPT;
    iResult= We_LibStartDesCore(&stMd,&stKey,pHandle); 
    
    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_LibDesEnCryptUpdate1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set padding data and encrypt data with DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be encrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_LibDesEnCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK; 
    St_WeBlockMAtchS *pstPara=NULL;
    WE_UINT32 iIndex=0;
    WE_INT32 iBufLen=0;
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen)||!pucOut||!puiOutLen)
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
    
    pstPara=(St_WeBlockMAtchS *)handle;
    
    iIndex = pstPara->iBufLen;
    if((uiInLen + (WE_UINT32)iIndex) == *puiOutLen)
    {
        pstPara->iBufLen = 0;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
        if(8 != *puiOutLen)
        {
            iResult=We_LibAppendDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
        }
        
    }
    else if ((uiInLen + (WE_UINT32)iIndex) < *puiOutLen) 
    {
        *puiOutLen -= 8 ; 
        if((uiInLen + (WE_UINT32)iIndex) > 8)
        {
            pstPara->iBufLen = (iIndex + uiInLen) % 8;
            (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
            iResult=We_LibAppendDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
            if((uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)))
            {
                iResult=We_LibAppendDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                    (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
            }
            iIndex = uiInLen - pstPara->iBufLen;
            (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
        }
        else
        {
            (WE_VOID)WE_MEMCPY(pstPara->aucData8 + iIndex, pucIn, uiInLen);
            pstPara->iBufLen = uiInLen;
        }
    }
    else
    {
        pstPara->iBufLen = (iIndex + uiInLen) - *puiOutLen;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendDes( pstPara->aucData8 ,8, pucOut, handle);
        if((uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)))
        {
            iResult=We_LibAppendDes((WE_VOID*)(pucIn + (8 - iIndex)), \
                (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut + 8, handle);
        }
        iIndex = uiInLen - pstPara->iBufLen;
        (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
    }
    return iResult;
}   

/*=====================================================================================
FUNCTION: 
    We_LibDesEnCryptFinal1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Get encrypted data and free memory.
ARGUMENTS PASSED:
    pucOut[OUT]: encrypted data;
    puiOutLen[OUT]: length of encrypted data;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_LibDesEnCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    St_WeBlockMAtchS *pstPara = NULL;
    WE_UCHAR aucData[8] = {0};
    
    if((NULL==handle)||(NULL==pucOut)||(0==puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 
    pstPara=(St_WeBlockMAtchS *)handle;
    if(*puiOutLen < 8)
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    *puiOutLen = 8;
    (WE_VOID)WE_MEMSET(aucData, (WE_UCHAR)(8-pstPara->iBufLen), (WE_UINT32)8);
    (WE_VOID)WE_MEMCPY(aucData, pstPara->aucData8, (WE_UINT32)pstPara->iBufLen);    
    iResult=We_LibAppendDes( aucData ,8, pucOut, handle);
    
    We_LibKillDes(handle);   
    return iResult;
}



/***************************************************************************************************
*   DES Decrypt                                                                    
***************************************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_LibDesDecrypt
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set sub decrypt key and decrypt data with DES algorithm.
ARGUMENTS PASSED:
    iRounds[IN]: total rounds.
    stKey[IN]: cipher key
    pucIn[IN]: data to be decrypted;
    uiInLen[IN]: length of data to be decrypted;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
/* No Padding */
WE_INT32 We_LibDesDecrypt
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL;
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);
    
    stMd.eOp = BSAPI_DECRYPT;
    iResult= We_LibStartDesCore(&stMd,&stKey,&handle);    
    
    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    
    if(!(uiInLen % 8) && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    iResult=We_LibAppendDes((WE_UCHAR*)pucIn, uiInLen,pucOut,handle);   

    We_LibKillDes(handle);
    return M_WE_LIB_ERR_OK;
}


/*=====================================================================================
FUNCTION: 
    We_LibDesDeCryptInit
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt init with DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pHandle[OUT]: data handle;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_LibDesDeCryptInit
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    if(NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_DECRYPT;
    iResult= We_LibStartDesCore(&stMd,&stKey,pHandle); 
    
    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_LibDesDeCryptUpdate
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt data with DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be decrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_LibDesDeCryptUpdate
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32              iResult       = M_WE_LIB_ERR_OK;
    WE_UINT32             uiLen     = 0; 
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen)||!pucOut||!puiOutLen)
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
    iResult=We_LibAppendDes((WE_UCHAR *)pucIn, uiInLen,pucOut, handle);  
    return iResult;    
}


/*=====================================================================================
FUNCTION: 
    We_LibDesDeCryptFinal
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Free the data handle.
ARGUMENTS PASSED:
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    no padding: the input data had been padded, so the length of 
                input data is multiples of block size.Padding is 
                stripped in the extern module.
=====================================================================================*/
WE_INT32 We_LibDesDeCryptFinal(WE_HANDLE handle) 
{
    WE_INT32 iResult=0;    
    if(NULL==handle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    We_LibKillDes(handle);
    
    return iResult;    
}

/*=====================================================================================
FUNCTION: 
    We_LibDesDecrypt1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Set sub decrypt key and decrypt data with DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    pucIn[IN]: data to be decrypted;
    uiInLen[IN]: length of data to be decrypted;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
/* RFC2630 Padding */
WE_INT32 We_LibDesDecrypt1
(
    St_WeCipherCrptKey stKey, 
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen
)
{
    WE_HANDLE handle=NULL; 
    WE_INT32 iResult=0;
    WE_UINT32 uiLen=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    CHECK_FOR_NULL_ARG(pucIn, 0x08);
    CHECK_FOR_NULL_ARG(pucOut, 0x08);
    CHECK_FOR_NULL_ARG(puiOutLen, 0x08);
    
    if(!(uiInLen % 8) && (*puiOutLen >= uiInLen))
    {
        *puiOutLen = uiInLen;
    }
    else
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    
    stMd.eOp = BSAPI_DECRYPT;
    iResult = We_LibStartDesCore(&stMd,&stKey,&handle);   
    
    if(iResult!=M_WE_LIB_ERR_OK)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    iResult=We_LibAppendDes((WE_UCHAR *)pucIn, uiInLen,pucOut, handle);
    
    if(iResult!=M_WE_LIB_ERR_OK)         
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    
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
    We_LibKillDes(handle);
    return M_WE_LIB_ERR_OK;    
}


/*=====================================================================================
FUNCTION: 
    We_LibDesDecryptInit1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt init with DES algorithm.
ARGUMENTS PASSED:
    stKey[IN]: cipher key
    handle[OUT]: data handle;
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_LibDesDecryptInit1
(
    St_WeCipherCrptKey stKey,
    WE_HANDLE* pHandle
)
{
    WE_INT32 iResult=0;
    St_WeLibBsapiMode stMd = {BSAPI_DECRYPT, BSAPI_CBC, BSAPI_SINGLE, BSAPI_KEY};

    if(NULL==pHandle)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    stMd.eOp = BSAPI_DECRYPT;
    /* get subkey */
    iResult= We_LibStartDesCore(&stMd,&stKey,pHandle);  
    
    return iResult;
}


/*=====================================================================================
FUNCTION: 
    We_LibDesDeCryptUpdate1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Decrypt data with DES algorithm.
ARGUMENTS PASSED:
    pucIn[IN]: data part to be decrypted;
    uiInLen[IN]: length of data part;
    pucOut[OUT]: decrypted data;
    puiOutLen[OUT]: length of decrypted data;
    handle[IO]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_LibDesDeCryptUpdate1
(
    const WE_UCHAR* pucIn, 
    WE_UINT32 uiInLen, 
    WE_UCHAR* pucOut, 
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK; 
    St_WeBlockMAtchS *pstPara=NULL;
    WE_UINT32 iIndex=0;
    WE_INT32 iBufLen=0;
    if((NULL==handle)||(NULL==pucIn)||(0==uiInLen)||!pucOut||!puiOutLen)
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
    
    pstPara=(St_WeBlockMAtchS *)handle;
    
    iIndex = pstPara->iBufLen;
    if((uiInLen + (WE_UINT32)iIndex) == *puiOutLen)
    {
        pstPara->iBufLen = 0;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
        if(8 != *puiOutLen)
        {
            iResult=We_LibAppendDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
        }
        
    }
    else if ((uiInLen + (WE_UINT32)iIndex) < *puiOutLen) 
    {
        *puiOutLen -= 8 ; 
        if((uiInLen + (WE_UINT32)iIndex) > 8)
        {
            pstPara->iBufLen = (iIndex + uiInLen) % 8;
            (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
            iResult=We_LibAppendDes((WE_UCHAR *)(pstPara->aucData8), 8,pucOut, handle);
            if((uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)))
            {
                iResult=We_LibAppendDes((WE_VOID *)(pucIn + (8 - iIndex)), \
                    (uiInLen - (WE_UINT32)(8 - iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut+8, handle);
            }
            iIndex = uiInLen - pstPara->iBufLen;
            (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
        }
        else
        {
            (WE_VOID)WE_MEMCPY(pstPara->aucData8 + iIndex, pucIn, uiInLen);
            pstPara->iBufLen = uiInLen;
        }
    }
    else
    {
        pstPara->iBufLen = (iIndex + uiInLen) - *puiOutLen;
        (WE_VOID)WE_MEMCPY((pstPara->aucData8 + iIndex), pucIn, (WE_UINT32)(8 - iIndex));
        iResult=We_LibAppendDes( pstPara->aucData8 ,8, pucOut, handle);
        if((uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)))
        {
            iResult=We_LibAppendDes((WE_VOID*)(pucIn + (8 - iIndex)), \
                (uiInLen - (8 - (WE_UINT32)iIndex) - (WE_UINT32)(pstPara->iBufLen)),pucOut + 8, handle);
        }
        iIndex = uiInLen - pstPara->iBufLen;
        (WE_VOID)WE_MEMCPY(pstPara->aucData8, pucIn + iIndex, (WE_UINT32)(pstPara->iBufLen));
    }
    return iResult;
}   


/*=====================================================================================
FUNCTION: 
    We_LibDesDeCryptFinal1
CREATE DATE: 
    2006-12-04
AUTHOR: 
    Sam
DESCRIPTION:
    Stripped the padding and free memory.
ARGUMENTS PASSED:
    pucOut[IN]: pointer to the end of decrypted data;
    puiOutLen[OUT]: length of padding;
    handle[IN]: data handle.
RETURN VALUE:
    M_WE_LIB_ERR_OK if success,others error code.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    rfc2630 padding: pad interior, and pad each byte with # bytes 
                     padded(# is equal to the length of padding, 
                     at least 1 byte).Padding is stripped on decrypt.
=====================================================================================*/
WE_INT32 We_LibDesDeCryptFinal1
(
    WE_UCHAR* pucOut,
    WE_UINT32* puiOutLen, 
    WE_HANDLE handle
)
{
    WE_INT32 iResult = M_WE_LIB_ERR_OK;
    WE_UINT32 uiLen = 0; 
    St_WeBlockMAtchS *pstPara=NULL;
    WE_UINT32 uiLoop=0;
    
    if((NULL==handle)||(NULL==pucOut)||(0==puiOutLen))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    } 
    pstPara=(St_WeBlockMAtchS *)handle;
    if(*puiOutLen < 8)
    {
        return M_WE_LIB_ERR_BUFFER_TOO_SMALL;
    }
    *puiOutLen = 8; 
    iResult=We_LibAppendDes( pstPara->aucData8 ,8, pucOut, handle);
    
    uiLen = *(pucOut + 8 - 1);
    if(uiLen&&(uiLen <= 8))
    {
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
    
    We_LibKillDes(handle);   
    return iResult;
}
















































/*==================================================================================================
    HEADER NAME : we_kpg.c
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

#define M_WE_SELF_FREE(p)                          {WE_FREE(p); (p) = NULL;}

/*******************************************************************************
*   Function Define Section
*******************************************************************************/ 
/*==================================================================================================
FUNCTION: 
    We_KpgConvertRSAPubKey
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    CONVERT RSA PUBLIC KEY STRUCT TO ASN DER ENCODE BIT STRING
ARGUMENTS PASSED:
    St_SecRsaPublicKey      stKey[IN]:RSA PUBLIC KEY STRUCT
    WE_UCHAR**              ppucKey[OUT]:DER ENCODED BIT STRING
    WE_INT32*               piKeyLen[OUT]:DER ENCODED STRING LENGTH
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
WE_INT32 We_KpgConvertRSAPubKey(St_SecRsaPublicKey stKey, 
                                        WE_UCHAR** ppucKey, WE_INT32* piKeyLen)
{
    WE_UINT16   usMsgLen = 0;
    WE_UINT16   usHdrLen = 0;
    WE_UINT16   usMLen = 0;
    WE_UINT16   usELen = 0;
    WE_UINT8    ucPrependModZero = 0;
    WE_UINT8    ucPrependExpZero = 0;
    WE_UINT16   usM = stKey.usModLen;
    WE_UINT16   usE = stKey.usExpLen;
    WE_UCHAR*   pucP = NULL;
    WE_INT32    iIndex = 0;
    WE_UINT8    ucBigEndian = 0;
    WE_UINT16   usU = 0x0102;

    if(!ppucKey || !piKeyLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    /* Check how bytes are positioned on the current platform */
    if (0x01 == *((WE_UINT8*)&(usU)))
    {
        ucBigEndian = 1;
    }
    else
    {
        ucBigEndian = 0;
    }

    /* Calculate length of DER representation of the public stKey */
    /* usMLen is the length of the modulus (integer) header */
    /* usELen is the length of the exponent (integer) header */
    /* usHdrLen is the length of the SEQUENCE (encapsulating) header */
    if (((WE_UINT8)*(stKey.pucModulus)) > 127)
    {
        ucPrependModZero = 1;
        usM ++;
    }
    usMLen = 2;
    if (usM > 127)
    {
        usMLen ++;
    }
    if (usM > 255)
    {
        usMLen ++;
    }

    if (((WE_UINT8)*(stKey.pucExponent)) > 127)
    {
        ucPrependExpZero = 1;
        usE ++;
    }
    usELen = 2;
    if (usE > 127)
    {
        usELen ++;
    }
    if (usE > 255)
    {
        usELen ++;
    }

    usMsgLen = (WE_UINT16)(usMLen + usM + usELen + usE);

    usHdrLen = 2;
    if (usMsgLen > 127)
    {
        usHdrLen ++;
    }
    if (usMsgLen > 255)
    {
        usHdrLen ++;
    }
    /* DER encode stKey (ASN.1) */
    *piKeyLen = usHdrLen + usMsgLen;
    pucP = (WE_UCHAR*)WE_MALLOC(usHdrLen + usMsgLen);
    if(!pucP)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    *ppucKey = pucP;
    *pucP++ = '\x30'; /* SEQUENCE */
    switch (usHdrLen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usMsgLen)); /* write message length (1 byte) */
            }
            break;
        }
        
        case 3: 
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER - 1 Byte */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usMsgLen)); /* write message length */
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER - 2 Bytes */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usMsgLen));
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
                *pucP++ = *((WE_UCHAR*)&(usMsgLen));
            }
            break;
        }
        
        default:
            break;
    } /* This provides for message lengths up to 65535 bits */

    *pucP++ = 0x02; /* INTEGER */
    switch (usMLen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usM));
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }

        default:
            break;
    }
    if (ucPrependModZero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usModLen; iIndex ++) /* write modulus (original length  only) */
    {
        *pucP++ = stKey.pucModulus[iIndex];
    }
    *pucP++ = 0x02; /* INTEGER */
    switch (usELen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usE));
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependExpZero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex=0; iIndex < stKey.usExpLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.pucExponent[iIndex];
    }

    return M_WE_LIB_ERR_OK;
}

/****************************************************************************************
****************************************************************************************/
#define WE_NN_DIGIT_BITS                         32
#define WE_NN_HALF_DIGIT_BITS                    16
#define WE_NN_DIGIT_LEN                          (WE_NN_DIGIT_BITS / 8)
#define WE_MAX_NN_DIGITS \
            ((SEC_MAX_RSA_MODULUS_LEN + WE_NN_DIGIT_LEN - 1) / WE_NN_DIGIT_LEN + 1)
  
#define WE_MAX_NN_DIGIT                           0xffffffff
#define WE_MAX_NN_HALF_DIGIT                      0xffff
#define WE_NN_LOW_HALF(x)                         ((x) & WE_MAX_NN_HALF_DIGIT)
#define WE_NN_HIGH_HALF(x)\
            (((x) >> WE_NN_HALF_DIGIT_BITS) & WE_MAX_NN_HALF_DIGIT)
#define WE_NN_TO_HIGH_HALF(x)\
            (((WE_UINT32)(x)) << WE_NN_HALF_DIGIT_BITS)

#define WE_NN_ASSIGN_DIGIT(a, b, digits)\
            {We_NnAssignZero(a, digits); a[0] = b;}

/*==================================================================================================
FUNCTION: 
    We_NnAssignZero
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*          puiArr[IN/OUT]:
    WE_UINT32           uiDigits[IN]:
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

static WE_VOID We_NnAssignZero (WE_UINT32* puiArr,WE_UINT32 uiDigits)
{
    if (NULL == puiArr)
    {
        return;
    }
    
    if(uiDigits) 
    {
        do 
        {
            *puiArr++ = 0;
        }while(--uiDigits);
    }
    return;
}

/*==================================================================================================
FUNCTION: 
    We_NnAssign2Exp
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArr[IN/OUT]:
    WE_UINT32       uiIndex[IN]:
    WE_UINT32       uiDigits[IN]:
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
static WE_VOID We_NnAssign2Exp (WE_UINT32* puiArr,WE_UINT32 uiIndex, WE_UINT32 uiDigits)
{
    if (NULL == puiArr)
    {
        return;
    }
    We_NnAssignZero (puiArr, uiDigits);

    if (uiIndex >= uiDigits * WE_NN_DIGIT_BITS)
    {
        return;
    }
    puiArr[uiIndex / WE_NN_DIGIT_BITS] = (WE_UINT32)1 << (uiIndex % WE_NN_DIGIT_BITS);

    return;
}

/*==================================================================================================
FUNCTION: 
    We_NnAdd
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*          puiArra[IN]:
    WE_UINT32*          puiArrb[IN]:
    WE_UINT32*          puiArrc[IN]:
    WE_UINT32           uiDigits[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 We_NnAdd (WE_UINT32* puiArra,WE_UINT32* puiArrb, 
                            WE_UINT32* puiArrc,WE_UINT32 uiDigits)
{
    WE_UINT32 uiTemp = 0, uiCarry = 0;

    if (!puiArra || !puiArrb || !puiArrc)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if(uiDigits)
    {
        do 
        {
            if((uiTemp = (*puiArrb++) + uiCarry) < uiCarry)
            {
                uiTemp = *puiArrc++;
            }
            else
            {
                uiTemp += *puiArrc;
                if(uiTemp < *puiArrc++)
                {
                    uiCarry = 1;
                }
                else
                {
                    uiCarry = 0;
                }
            }
            *puiArra++ = uiTemp;
        }while(--uiDigits);
    }

    return (uiCarry);
}

/*==================================================================================================
FUNCTION: 
    We_NnSub
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*          puiArra[IN]:
    WE_UINT32*          puiArrb[IN]:
    WE_UINT32*          puiArrc[IN]:
    WE_UINT32           uiDigits[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_UINT32 We_NnSub (WE_UINT32* puiArra,WE_UINT32* puiArrb, 
                            WE_UINT32* puiArrc,WE_UINT32 uiDigits)
{
    WE_UINT32 uiTemp = 0, uiBorrow = 0;

    if (!puiArra || !puiArrb || !puiArrc)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if(uiDigits)
    {
        do {
            if((uiTemp = (*puiArrb++) - uiBorrow) == WE_MAX_NN_DIGIT)
            {
                uiTemp = WE_MAX_NN_DIGIT - *puiArrc++;
            }
            else
            {
                uiTemp -= *puiArrc;
                if(uiTemp > (WE_MAX_NN_DIGIT - *puiArrc++))
                {
                    uiBorrow = 1;
                }    
                else
                {
                    uiBorrow = 0;
                }
            }    
            *puiArra++ = uiTemp;
        }while(--uiDigits);
    }    

    return(uiBorrow);
}

/*==================================================================================================
FUNCTION: 
    We_NnCmp
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*          puiArra[IN]:
    WE_UINT32*          puiArrb[IN]:
    WE_UINT32           uiIndex[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 We_NnCmp (WE_UINT32* puiArra, WE_UINT32* puiArrb, WE_UINT32 uiIndex)
{
    if (!puiArra || !puiArrb)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if(uiIndex) 
    {
        do 
        {
            uiIndex--;
            if(*(puiArra + uiIndex) > *(puiArrb + uiIndex))
            {
                return(1);
            }
            if(*(puiArra + uiIndex) < *(puiArrb + uiIndex))
            {
                return(-1);
            }
        }while(uiIndex);
    }

    return (0);
}

/*==================================================================================================
FUNCTION: 
    We_NnAssign
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*          puiArra[IN]:
    WE_UINT32*          puiArrb[IN]:
    WE_UINT32           uiIndex[IN]:
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

static WE_VOID We_NnAssign (WE_UINT32* puiArra, WE_UINT32* puiArrb, WE_UINT32 uiIndex)
{
    if (!puiArra || !puiArrb)
    {
        return;
    }
    
    if(uiIndex) 
    {
        do 
        {
            *puiArra++ = *puiArrb++;
        }while(--uiIndex);
    }
    return;
}

/*==================================================================================================
FUNCTION: 
    We_NnDigits
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*          puiArra[IN]:
    WE_UINT32           uiDigits[IN]
RETURN VALUE:
    INDEX
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_UINT32 We_NnDigits (WE_UINT32* puiArra, WE_UINT32 uiDigits)
{
    if (!puiArra)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if(uiDigits) 
    {
        uiDigits--;

        do 
        {
            if(*(puiArra + uiDigits))
            {
                break;
            }
        }while(uiDigits--);

        return(uiDigits + 1);
    }

    return(uiDigits);
}

/*==================================================================================================
FUNCTION: 
    We_NnDmult
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32           uiA[IN]:
    WE_UINT32           uiB[IN]:
    WE_UINT32*          puiHigh[OUT]: 
    WE_UINT32*          puiLow[OUT]:
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

static WE_VOID We_NnDmult(WE_UINT32 uiA, WE_UINT32 uiB, WE_UINT32* puiHigh, 
                        WE_UINT32* puiLow)
{
    WE_UINT16 usAl = 0, usAh = 0, usBl = 0, usBh = 0;
    WE_UINT32 uiM1 = 0, uiM2 = 0, uiM = 0, uiMl = 0, iMh = 0, uiCarry = 0;

    if (!puiHigh || !puiLow)
    {
        return;
    }
    usAl = (WE_UINT16)WE_NN_LOW_HALF( uiA );
    usAh = (WE_UINT16)WE_NN_HIGH_HALF( uiA );
    usBl = (WE_UINT16)WE_NN_LOW_HALF( uiB );
    usBh = (WE_UINT16)WE_NN_HIGH_HALF( uiB );

    *puiLow = (WE_UINT32) usAl * usBl;
    *puiHigh = (WE_UINT32) usAh * usBh;

    uiM1 = (WE_UINT32) usAl * usBh;
    uiM2 = (WE_UINT32) usAh * usBl;
    uiM = uiM1 + uiM2;

    if(uiM < uiM1)
    {
        uiCarry = 1L << (WE_NN_DIGIT_BITS / 2);
    }

    uiMl = (uiM & WE_MAX_NN_HALF_DIGIT) << (WE_NN_DIGIT_BITS / 2);
    iMh = uiM >> (WE_NN_DIGIT_BITS / 2);

    *puiLow += uiMl;

    if(*puiLow < uiMl)
    {
        uiCarry++;
    }

    *puiHigh += uiCarry + iMh;

    return;
}


/*==================================================================================================
FUNCTION: 
    We_NnMult
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32*      puiArrc[IN]:
    WE_UINT32       uiDigits[IN]:
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

static WE_VOID We_NnMult (WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                           WE_UINT32* puiArrc, WE_UINT32 uiDigits)
{
    WE_UINT32 auiT[2*WE_MAX_NN_DIGITS] = {0};
    WE_UINT32 uiDhigh = 0, uiDlow = 0, uiCarry = 0;
    WE_UINT32 uiDigitsb = 0, uiDigitsc, uiIndex = 0, uiLoop = 0;

    if (!puiArra || !puiArrb || !puiArrc)
    {
        return;
    }
    We_NnAssignZero (auiT, 2 * uiDigits);

    uiDigitsb = We_NnDigits (puiArrb, uiDigits);
    uiDigitsc = We_NnDigits (puiArrc, uiDigits);

    for (uiIndex = 0; uiIndex < uiDigitsb; uiIndex++) 
    {
        uiCarry = 0;
        if(*(puiArrb+uiIndex) != 0) 
        {
            for(uiLoop = 0; uiLoop < uiDigitsc; uiLoop++) 
            {
                We_NnDmult(*(puiArrb+uiIndex), *(puiArrc+uiLoop), &uiDhigh, &uiDlow);
                if((*(auiT+(uiIndex+uiLoop)) = \
                    *(auiT+(uiIndex+uiLoop)) + uiCarry) < uiCarry)
                {
                    uiCarry = 1;
                }
                else
                {
                    uiCarry = 0;
                }
                if((*(auiT+(uiIndex+uiLoop)) += uiDlow) < uiDlow)
                    uiCarry++;
                uiCarry += uiDhigh;
            }
        }
        *(auiT+(uiIndex+uiDigitsc)) += uiCarry;
    }


    We_NnAssign(puiArra, auiT, 2 * uiDigits);

    return;
}

/*==================================================================================================
FUNCTION: 
    We_NnDigitBits
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32       uiDigits[IN]:
RETURN VALUE:
    NUM
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_UINT32 We_NnDigitBits (WE_UINT32 uiDigits)
{
    WE_UINT32 uiNum = 0;

    for (uiNum = 0; uiNum < WE_NN_DIGIT_BITS; uiNum++, uiDigits >>= 1)
    {
        if (uiDigits == 0)
        {
            break;
        }
    }

    return (uiNum);
}

/*==================================================================================================
FUNCTION: 
    We_NnLShift
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32       uiC[IN]:
    WE_UINT32       uiDigits[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_UINT32 We_NnLShift (WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                               WE_UINT32 uiC, WE_UINT32 uiDigits)
{
    WE_UINT32 uiTemp = 0, uiCarry = 0;
    WE_UINT32 uiT = 0;

    if (!puiArra || !puiArrb)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if(uiC < WE_NN_DIGIT_BITS)
        if(uiDigits) 
        {

            uiT = WE_NN_DIGIT_BITS - uiC;

            do 
            {
                uiTemp = *puiArrb++;
                *puiArra++ = (uiTemp << uiC) | uiCarry;
                uiCarry = uiC ? (uiTemp >> uiT) : 0;
            }while(--uiDigits);
        }

    return (uiCarry);
}


/*==================================================================================================
FUNCTION: 
    We_NnRShift
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32       uiC[IN]:
    WE_UINT32       uiDigits[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_UINT32 We_NnRShift (WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                               WE_UINT32 uiC, WE_UINT32 uiDigits)
{
    WE_UINT32 uiTemp= 0, uiCarry = 0;
    WE_UINT32 uiT;

    if (!puiArra || !puiArrb)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    if(uiC < WE_NN_DIGIT_BITS)
    {
        if(uiDigits) 
        {
            uiT = WE_NN_DIGIT_BITS - uiC;
            do 
            {
                uiDigits--;
                uiTemp = *(puiArrb+uiDigits);
                *(puiArra+uiDigits) = (uiTemp >> uiC) | uiCarry;
                uiCarry = uiC ? (uiTemp << uiT) : 0;
            }while(uiDigits);
        }
    }

    return (uiCarry);
}


/*==================================================================================================
FUNCTION: 
    We_NnSubdigitmult
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[IN/OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32       uiC[IN]:
    WE_UINT32*      puiArrd[IN]: 
    WE_UINT32       uiDigits[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_UINT32 We_NnSubdigitmult(WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                                    WE_UINT32 uiC, WE_UINT32* puiArrd, 
                                    WE_UINT32 uiDigits)
{
    WE_UINT32 uiBorrow = 0, uiThigh = 0, uiTlow = 0;
    WE_UINT32 uiIndex = 0;

    if (!puiArra || !puiArrb || !puiArrd)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }

    uiBorrow = 0;

    if(uiC != 0) 
    {
        for(uiIndex = 0; uiIndex < uiDigits; uiIndex++) 
        {
            We_NnDmult(uiC, puiArrd[uiIndex], &uiThigh, &uiTlow);
            if((puiArra[uiIndex] = puiArrb[uiIndex] - uiBorrow) > \
                (WE_MAX_NN_DIGIT - uiBorrow))
            {
                uiBorrow = 1;
            }
            else
            {
                uiBorrow = 0;
            }
            if((puiArra[uiIndex] -= uiTlow) > (WE_MAX_NN_DIGIT - uiTlow))
            {
                uiBorrow++;
            }
            uiBorrow += uiThigh;
        }
    }

    return (uiBorrow);
}


/*==================================================================================================
FUNCTION: 
    We_NnDiv
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[OUT]:
    WE_UINT32*      puiArrc[IN]:
    WE_UINT32       uiDigitsc[IN]:
    WE_UINT32*      puiArrd[IN]:
    WE_UINT32       uiDigitsd[IN]:
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

static WE_VOID We_NnDiv (WE_UINT32*puiArra, WE_UINT32* puiArrb, 
                       WE_UINT32* puiArrc,WE_UINT32 uiDigitsc, 
                       WE_UINT32* puiArrd, WE_UINT32 uiDigitsd)
{
    WE_UINT32 uiAi = 0, auiCc[2*WE_MAX_NN_DIGITS+1] = {0}, 
              auiDd[WE_MAX_NN_DIGITS] = {0}, uiS = 0;
    WE_UINT32 auiT[2] = {0}, uiU = 0, uiV = 0, *puiCcptr = NULL;
    WE_UINT16 usAhigh = 0, usAlow = 0, usChigh = 0, usClow = 0;
    WE_INT32  iIndex = 0;
    WE_UINT32 uiDdDigits = 0, uiShift = 0;

    if (!puiArra || !puiArrb || !puiArrc || !puiArrd)
    {
        return;
    }

    uiDdDigits = We_NnDigits (puiArrd, uiDigitsd);
    if(uiDdDigits == 0)
    {
        return;
    }

    uiShift = WE_NN_DIGIT_BITS - We_NnDigitBits (puiArrd[uiDdDigits-1]);
    We_NnAssignZero (auiCc, uiDdDigits);
    auiCc[uiDigitsc] = We_NnLShift (auiCc, puiArrc, uiShift, uiDigitsc);
    (WE_VOID)We_NnLShift (auiDd, puiArrd, uiShift, uiDdDigits);
    uiS = auiDd[uiDdDigits-1];

    We_NnAssignZero (puiArra, uiDigitsc);

    for (iIndex = (WE_INT32)(uiDigitsc-uiDdDigits); iIndex >= 0; iIndex--) 
    {
        if (uiS == WE_MAX_NN_DIGIT)
        {
            uiAi = auiCc[(WE_UINT32)iIndex+uiDdDigits];
        }
        else 
        {
            puiCcptr = &auiCc[(WE_UINT32)iIndex+uiDdDigits-1];

            uiS++;
            usChigh = (WE_UINT16)WE_NN_HIGH_HALF (uiS);
            usClow = (WE_UINT16)WE_NN_LOW_HALF (uiS);

            *auiT = *puiCcptr;
            *(auiT+1) = *(puiCcptr+1);

            if (usChigh == WE_MAX_NN_HALF_DIGIT)
            {
                usAhigh = (WE_UINT16)WE_NN_HIGH_HALF (*(auiT+1));
            }
            else
            {
                usAhigh = (WE_UINT16)(*(auiT+1) / (usChigh + 1));
            }
            uiU = (WE_UINT32)usAhigh * (WE_UINT32)usClow;
            uiV = (WE_UINT32)usAhigh * (WE_UINT32)usChigh;
            if ((*auiT -= WE_NN_TO_HIGH_HALF (uiU)) > \
                (WE_MAX_NN_DIGIT - WE_NN_TO_HIGH_HALF (uiU)))
            {
                auiT[1]--;
            }
            *(auiT+1) -= WE_NN_HIGH_HALF (uiU);
            *(auiT+1) -= uiV;

            while ((*(auiT+1) > usChigh) ||\
                         ((*(auiT+1) == usChigh) && \
                         (*auiT >= WE_NN_TO_HIGH_HALF (usClow))))
            {
                if ((*auiT -= WE_NN_TO_HIGH_HALF (usClow)) > \
                    WE_MAX_NN_DIGIT - WE_NN_TO_HIGH_HALF (usClow))
                {
                    auiT[1]--;
                }
                *(auiT+1) -= usChigh;
                usAhigh++;
            }

            if (usChigh == WE_MAX_NN_HALF_DIGIT)
            {
                usAlow = (WE_UINT16)WE_NN_LOW_HALF (*(auiT+1));
            }
            else
            {
                usAlow = (WE_UINT16)((WE_NN_TO_HIGH_HALF (*(auiT+1)) + \
                    WE_NN_HIGH_HALF (*auiT)) / (usChigh + 1));
            }
            uiU = (WE_UINT32)usAlow * (WE_UINT32)usClow;
            uiV = (WE_UINT32)usAlow * (WE_UINT32)usChigh;
            if ((*auiT -= uiU) > (WE_MAX_NN_DIGIT - uiU))
            {
                auiT[1]--;
            }
            if ((*auiT -= WE_NN_TO_HIGH_HALF (uiV)) > \
                (WE_MAX_NN_DIGIT - WE_NN_TO_HIGH_HALF (uiV)))
            {
                auiT[1]--;
            }
            *(auiT+1) -= WE_NN_HIGH_HALF (uiV);

            while ((*(auiT+1) > 0) || ((*(auiT+1) == 0) && *auiT >= uiS)) 
            {
                if ((*auiT -= uiS) > (WE_MAX_NN_DIGIT - uiS))
                {
                    auiT[1]--;
                }
                usAlow++;
            }

            uiAi = WE_NN_TO_HIGH_HALF (usAhigh) + usAlow;
            uiS--;
        }

        auiCc[(WE_UINT32)iIndex+uiDdDigits] -= \
            We_NnSubdigitmult(&auiCc[iIndex], &auiCc[iIndex], uiAi, auiDd, uiDdDigits);

        while (auiCc[(WE_UINT32)iIndex+uiDdDigits] || \
            (We_NnCmp (&auiCc[iIndex], auiDd, uiDdDigits) >= 0)) 
        {
            uiAi++;
            auiCc[(WE_UINT32)iIndex+uiDdDigits] -= \
                We_NnSub (&auiCc[iIndex], &auiCc[iIndex], auiDd, uiDdDigits);
        }

        puiArra[iIndex] = uiAi;
    }

    We_NnAssignZero (puiArrb, uiDigitsd);
    (WE_VOID)We_NnRShift (puiArrb, auiCc, uiShift, uiDdDigits);

    return;
}

/*==================================================================================================
FUNCTION: 
    We_NnMod
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32       uiDigitsb[IN]:
    WE_UINT32*      puiArrc[IN]:
    WE_UINT32       uiDigitsc[IN]:
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

static WE_VOID We_NnMod (WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                          WE_UINT32 uiDigitsb,WE_UINT32* puiArrc, WE_UINT32 uiDigitsc)
{
    WE_UINT32 auiT[2 * WE_MAX_NN_DIGITS] = {0};

    if (!puiArra || !puiArrb || !puiArrc)
    {
        return;
    }  
    We_NnDiv (auiT, puiArra, puiArrb, uiDigitsb, puiArrc, uiDigitsc);

    return;
}


/*==================================================================================================
FUNCTION: 
    We_NnZero
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[IN]:
    WE_UINT32       uiDigits[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 We_NnZero (WE_UINT32* puiArra, WE_UINT32 uiDigits)
{
    if (!puiArra)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if(uiDigits)
    {
        do 
        {
            if(*puiArra++)
            {
                return(0);
            }
        }while(--uiDigits);
    }

    return (1);
}


/*==================================================================================================
FUNCTION: 
    We_NnModInv
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32*      puiArrc[IN]:
    WE_UINT32       uiDigits[IN]:
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

static WE_VOID We_NnModInv (WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                             WE_UINT32* puiArrc, WE_UINT32 uiDigits)
{
    WE_UINT32 auiQ[WE_MAX_NN_DIGITS] = {0}, 
        auiT1[WE_MAX_NN_DIGITS] = {0}, auiT3[WE_MAX_NN_DIGITS] = {0},
        auiU1[WE_MAX_NN_DIGITS] = {0}, auiU3[WE_MAX_NN_DIGITS] = {0}, 
        auiV1[WE_MAX_NN_DIGITS] = {0}, auiV3[WE_MAX_NN_DIGITS] = {0}, 
        auiW[2*WE_MAX_NN_DIGITS] = {0};
    WE_INT32 iU1Sign = 0;

    if (!puiArra || !puiArrb || !puiArrc)
    {
        return;
    }

    /* Apply extended Euclidean algorithm, modified to avoid negative
       numbers.
    */
    WE_NN_ASSIGN_DIGIT (auiU1, 1, uiDigits);
    We_NnAssignZero (auiV1, uiDigits);
    We_NnAssign (auiU3, puiArrb, uiDigits);
    We_NnAssign (auiV3, puiArrc, uiDigits);
    iU1Sign = 1;

    while (! We_NnZero (auiV3, uiDigits)) 
    {
        We_NnDiv (auiQ, auiT3, auiU3, uiDigits, auiV3, uiDigits);
        We_NnMult (auiW, auiQ, auiV1, uiDigits);
        (WE_VOID)We_NnAdd (auiT1, auiU1, auiW, uiDigits);
        We_NnAssign (auiU1, auiV1, uiDigits);
        We_NnAssign (auiV1, auiT1, uiDigits);
        We_NnAssign (auiU3, auiV3, uiDigits);
        We_NnAssign (auiV3, auiT3, uiDigits);
        iU1Sign = -iU1Sign;
    }

    /* Negate result if sign is negative. */
    if (iU1Sign < 0)
    {
        (WE_VOID)We_NnSub (puiArra, puiArrc, auiU1, uiDigits);
    }
    else
    {
        We_NnAssign (puiArra, auiU1, uiDigits);
    }

    return;
}


/*==================================================================================================
FUNCTION: 
    We_NnEncode
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UCHAR*       pucArra[OUT]
    WE_UINT32       uiLen[IN]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32       uiDigits[IN]:
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

static WE_VOID We_NnEncode (WE_UCHAR* pucArra, WE_UINT32 uiLen, 
                             WE_UINT32* puiArrb,  WE_UINT32 uiDigits)
{
    WE_UINT32 uiT = 0;
    WE_INT32  iLoop = 0;
    WE_UINT32 uiIndex = 0, uiStep = 0;

    if (!pucArra || !puiArrb)
    {
        return;
    } 

    for (uiIndex = 0, iLoop = (WE_INT32)uiLen - 1; uiIndex < uiDigits && iLoop >= 0; uiIndex++) 
    {
        uiT = puiArrb[uiIndex];
        for (uiStep = 0; iLoop >= 0 && uiStep < WE_NN_DIGIT_BITS; iLoop--, uiStep += 8)
        {
            pucArra[iLoop] = (WE_UCHAR)(uiT >> uiStep);
        }
    }
    for (; iLoop >= 0; iLoop--)
    {
        pucArra[iLoop] = 0;           
    }

    return;        
}


#define WE_NN_EQUAL(a, b, digits)      (! We_NnCmp (a, b, digits))
#define WE_NN_DIGIT_2MSB(x)            (WE_UINT32)(((x) >> (WE_NN_DIGIT_BITS - 2)) & 3)
#define WE_NN_I_PLUS1                  ( iIndex==2 ? 0 : iIndex+1 )
#define WE_NN_I_MINUS1                 ( iIndex==0 ? 2 : iIndex-1 )
#define WE_NN_GET_T(i)                 ( &(aauiT[i][0]) )


/*==================================================================================================
FUNCTION: 
    We_NnGcd
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32*      puiArrc[IN]:
    WE_UINT32       uiDigits[IN]:
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

static WE_VOID We_NnGcd(WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                      WE_UINT32* puiArrc, WE_UINT32 uiDigits)
{
    WE_INT16 iIndex = 0;
    WE_UINT32 aauiT[3][WE_MAX_NN_DIGITS] = {0};

    if (!puiArra || !puiArrb || !puiArrc)
    {
        return;
    } 

    We_NnAssign(WE_NN_GET_T(0), puiArrc, uiDigits);
    We_NnAssign(WE_NN_GET_T(1), puiArrb, uiDigits);

    iIndex = 1;

    while(!We_NnZero(WE_NN_GET_T(iIndex),uiDigits))
    {
        We_NnMod(WE_NN_GET_T(WE_NN_I_PLUS1), \
            WE_NN_GET_T(WE_NN_I_MINUS1), uiDigits, WE_NN_GET_T(iIndex), uiDigits);
        iIndex = WE_NN_I_PLUS1;
    }

    We_NnAssign(puiArra , WE_NN_GET_T(WE_NN_I_MINUS1), uiDigits);

    return;
}


/*==================================================================================================
FUNCTION: 
    We_KpgRSAFilter
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[IN]:
    WE_UINT32       uiDigitsa[IN]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32       uiDigitsb[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 We_KpgRSAFilter(WE_UINT32*puiArra, WE_UINT32 uiDigitsa, 
                              WE_UINT32* puiArrb, WE_UINT32 uiDigitsb)
{
    WE_INT32 iStatus = 0;
    WE_UINT32 auiAminus1[WE_MAX_NN_DIGITS] = {0}, auiT[WE_MAX_NN_DIGITS] = {0};
    WE_UINT32 auiU[WE_MAX_NN_DIGITS] = {0};

    if (!puiArra || !puiArrb)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    uiDigitsb = uiDigitsb;

    WE_NN_ASSIGN_DIGIT(auiT, 1, uiDigitsa);
    (WE_VOID)We_NnSub(auiAminus1, puiArra, auiT, uiDigitsa);

    We_NnGcd(auiU, auiAminus1, puiArrb, uiDigitsa);

    iStatus = WE_NN_EQUAL(auiT, auiU, uiDigitsa);

    return(iStatus);
}


/*==================================================================================================
FUNCTION: 
    We_NnModMult
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra, 
    WE_UINT32*      puiArrb, 
    WE_UINT32*      puiArrc, 
    WE_UINT32*      puiArrd, 
    WE_UINT32       uiDigits
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

static WE_VOID We_NnModMult (WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                              WE_UINT32* puiArrc, WE_UINT32* puiArrd, 
                              WE_UINT32 uiDigits)
{
    WE_UINT32 auiT[2*WE_MAX_NN_DIGITS] = {0};

    if (!puiArra || !puiArrb || !puiArrc ||!puiArrd)
    {
        return;
    }
    We_NnMult (auiT, puiArrb, puiArrc, uiDigits);
    We_NnMod (puiArra, auiT, 2 * uiDigits, puiArrd, uiDigits);

    return;
}


/*==================================================================================================
FUNCTION: 
    We_NnModExp
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*          puiArra[OUT]:
    WE_UINT32*          puiArrb[IN]:
    WE_UINT32*          puiArrc[IN]:
    WE_UINT32           uiDigitsC[IN]:
    WE_UINT32*          puiArrd[IN]:
    WE_UINT32           uiDigitsd[IN]:
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

static WE_VOID We_NnModExp (WE_UINT32* puiArra, WE_UINT32* puiArrb, 
                             WE_UINT32* puiArrc,WE_UINT32 uiDigitsC, 
                             WE_UINT32* puiArrd,WE_UINT32 uiDigitsd)
{
    WE_UINT32 aauiBPower[3][WE_MAX_NN_DIGITS] = {0}, uiCi = 0, 
              auiT[WE_MAX_NN_DIGITS] = {0};
    WE_INT32 iIndex = 0;
    WE_UINT32 uiCiBits = 0, uiLoop = 0, uiIndex = 0;

    if (!puiArra || !puiArrb || !puiArrc || !puiArrd)
    {
        return;
    }

    /* Store puiArrb, puiArrb^2 mod puiArrd, and puiArrb^3 mod puiArrd.
     */
    We_NnAssign (aauiBPower[0], puiArrb, uiDigitsd);
    We_NnModMult (aauiBPower[1], aauiBPower[0], puiArrb, puiArrd, uiDigitsd);
    We_NnModMult (aauiBPower[2], aauiBPower[1], puiArrb, puiArrd, uiDigitsd);
  
    WE_NN_ASSIGN_DIGIT (auiT, 1, uiDigitsd);

    uiDigitsC = We_NnDigits (puiArrc, uiDigitsC);
    for (iIndex = (WE_INT32)uiDigitsC - 1; iIndex >= 0; iIndex--)
    {
        uiCi = puiArrc[iIndex];
        uiCiBits = WE_NN_DIGIT_BITS;

        /* Scan past leading zero bits of most significant digit.
         */
        if (iIndex == (int)(uiDigitsC - 1)) 
        {
            while (! WE_NN_DIGIT_2MSB (uiCi)) 
            {
                uiCi <<= 2;
                uiCiBits -= 2;
            }
        }

        for (uiLoop = 0; uiLoop < uiCiBits; uiLoop += 2, uiCi <<= 2) 
        {
        /* Compute auiT = auiT^4 * puiArrb^uiIndex mod puiArrd, where uiIndex = two MSB'uiIndex of uiCi. */
            We_NnModMult (auiT, auiT, auiT, puiArrd, uiDigitsd);
            We_NnModMult (auiT, auiT, auiT, puiArrd, uiDigitsd);
            if ((uiIndex = WE_NN_DIGIT_2MSB (uiCi)) != 0)
                We_NnModMult (auiT, auiT, aauiBPower[uiIndex-1], puiArrd, uiDigitsd);
        }
    }
  
    We_NnAssign (puiArra, auiT, uiDigitsd);

    return;
}


/*==================================================================================================
FUNCTION: 
    We_KpgProbablePrime
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[IN]:
    WE_UINT32       uiDigitsa[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 We_KpgProbablePrime(WE_UINT32* puiArra, WE_UINT32 uiDigitsa)
{
    WE_UINT32 uiSmallPrimes[] = 
    {
        3, 5, 7, 11, 13, 17, 19,
        23, 29, 31, 37, 41, 43, 47, 53,
        59, 61, 67, 71, 73, 79, 83, 89,
        97, 101, 103, 107, 109, 113, 127, 131,
        137, 139, 149, 151, 157, 163, 167, 173,
        179, 181, 191, 193, 197, 199, 211, 223,
        227, 229, 233, 239, 241, 251, 257, 263,
        269, 271, 277, 281, 283, 293, 307, 311,
        313, 317, 331, 337, 347, 349, 353, 359,
        367, 373, 379, 383, 389, 397, 401, 409,
        419, 421, 431, 433, 439, 443, 449, 457,
        461, 463, 467, 479, 487, 491, 499, 503,
        509, 521, 523, 541, 547, 557, 563, 569,
        571, 577, 587, 593, 599, 601, 607, 613,
        617, 619, 631, 641, 643, 647, 653, 659,
        661, 673, 677, 683, 691, 701, 709, 719,
        727, 733, 739, 743, 751, 757, 761, 769,
        773, 787, 797, 809, 811, 821, 823, 827,
        829, 839, 853, 857, 859, 863, 877, 881,
        883, 887, 907, 911, 919, 929, 937, 941,
        947, 953, 967, 971, 977, 983, 991, 997,
        1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049,
        1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097,
        1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163,
        1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223,
        1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283,
        1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321,
        1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423,
        1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459,
        1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
        1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571,
        1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619,
        1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693,
        1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747,
        1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811,
        1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877,
        1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949,
        1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003,
        2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069,
        2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129,
        2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203,
        2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267,
        2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311,
        2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377,
        2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423,
        2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503,
        2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579,
        2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657,
        2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693,
        2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741,
        2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801,
        2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861,
        2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939,
        2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011,
        3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079,
        3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167,
        3169, 3181, 3187, 3191, 3203, 3209, 3217, 3221,
        3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301,
        3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347,
        3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413,
        3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491,
        3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541,
        3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607,
        3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671,
        3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727,
        3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797,
        3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863,
        3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923,
        3929, 3931, 3943, 3947, 3967, 3989, 4001, 4003,
        4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057,
        4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129,
        4133, 4139, 4153, 4157, 4159, 4177, 4201, 4211,
        4217, 4219, 4229, 4231, 4241, 4243, 4253, 4259,
        4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337,
        4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409,
        4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481,
        4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547,
        4549, 4561, 4567, 4583, 4591, 4597, 4603, 4621,
        4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673,
        4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751,
        4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813,
        4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909,
        4919, 4931, 4933, 4937, 4943, 4951, 4957, 4967,
        4969, 4973, 4987, 4993, 4999, 5003, 5009, 5011,
        5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087,
        5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167,
        5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233,
        5237, 5261, 5273, 5279, 5281, 5297, 5303, 5309,
        5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399,
        5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443,
        5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507,
        5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573,
        5581, 5591, 5623, 5639, 5641, 5647, 5651, 5653,
        5657, 5659, 5669, 5683, 5689, 5693, 5701, 5711,
        5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791,
        5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849,
        5851, 5857, 5861, 5867, 5869, 5879, 5881, 5897,
        5903, 5923, 5927, 5939, 5953, 5981, 5987, 6007,
        6011, 6029, 6037, 6043, 6047, 6053, 6067, 6073,
        6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133,
        6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211,
        6217, 6221, 6229, 6247, 6257, 6263, 6269, 6271,
        6277, 6287, 6299, 6301, 6311, 6317, 6323, 6329,
        6337, 6343, 6353, 6359, 6361, 6367, 6373, 6379,
        6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473,
        6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563,
        6569, 6571, 6577, 6581, 6599, 6607, 6619, 6637,
        6653, 6659, 6661, 6673, 6679, 6689, 6691, 6701,
        6703, 6709, 6719, 6733, 6737, 6761, 6763, 6779,
        6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833,
        6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907,
        6911, 6917, 6947, 6949, 6959, 6961, 6967, 6971,
        6977, 6983, 6991, 6997, 7001, 7013, 7019, 7027,
        7039, 7043, 7057, 7069, 7079, 7103, 7109, 7121,
        7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207,
        7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253,
        7283, 7297, 7307, 7309, 7321, 7331, 7333, 7349,
        7351, 7369, 7393, 7411, 7417, 7433, 7451, 7457,
        7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517,
        7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561,
        7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621,
        7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691,
        7699, 7703, 7717, 7723, 7727, 7741, 7753, 7757,
        7759, 7789, 7793, 7817, 7823, 7829, 7841, 7853,
        7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919,
        7927, 7933, 7937, 7949, 7951, 7963, 7993, 8009,
        8011, 8017, 8039, 8053, 8059, 8069, 8081, 8087,
        8089, 8093, 8101, 8111, 8117, 8123, 8147, 8161,
        8167, 8171, 8179, 8191, 0
    };
    WE_INT32 iStatus = 0;
    WE_UINT32 uiT[WE_MAX_NN_DIGITS] = {0}, uiU[WE_MAX_NN_DIGITS] = {0};
    WE_UINT32 iIndex = 0;

    if (NULL == puiArra)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    iStatus = 1;

    We_NnAssignZero(uiT, uiDigitsa);

    /* Small Primes test, weed out junk numbers before slower Fermat'uiIndex */

    for(iIndex = 0; *(uiSmallPrimes+iIndex); iIndex++) 
    {
        *uiT = *(uiSmallPrimes+iIndex);
        if(uiDigitsa == 1)
        {
            if(We_NnCmp (puiArra, uiT, 1) == 0)
            {
                break;
            }
        }
        We_NnMod(uiT, puiArra, uiDigitsa, uiT, 1);
        if(We_NnZero (uiT, 1))
        {
            iStatus = 0;
            break;
        }
    }

    /* Clear sensitive information. */

    iIndex = 0;
    (WE_VOID)WE_MEMSET(uiT,0,sizeof(uiT));

    if(iStatus) 
    {
        WE_NN_ASSIGN_DIGIT(uiT, 2, uiDigitsa);
        We_NnModExp(uiU, uiT, puiArra, uiDigitsa, puiArra, uiDigitsa);

        iStatus = WE_NN_EQUAL(uiT, uiU, uiDigitsa);
    }

    return(iStatus ? TRUE : FALSE);
}


/*==================================================================================================
FUNCTION: 
    We_NnDecode
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32       uiDigits[IN]:
    WE_UCHAR*       pucArrb[IN]:
    WE_UINT32       uiLen[IN]:
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

static WE_VOID We_NnDecode (WE_UINT32* puiArra, WE_UINT32 uiDigits, 
                          WE_UCHAR* pucArrb, WE_UINT32 uiLen)
{
  WE_UINT32 uiT = 0;
  WE_INT32  iIndex = 0;
  WE_UINT32 uiLoop = 0, uiStep = 0;

    if (!puiArra || !pucArrb)
    {
        return;
    } 
  
  for (uiLoop = 0, iIndex = (WE_INT32)uiLen - 1; uiLoop < uiDigits && iIndex >= 0; uiLoop++) 
  {
    uiT = 0;
    for (uiStep = 0; iIndex >= 0 && uiStep < WE_NN_DIGIT_BITS; iIndex--, uiStep += 8)
    {
        uiT |= ((WE_UINT32)pucArrb[iIndex]) << uiStep;
    }
    puiArra[uiLoop] = uiT;
  }
  
  for (; uiLoop < uiDigits; uiLoop++)
  {
    puiArra[uiLoop] = 0;
  }

  return;
}


/*==================================================================================================
FUNCTION: 
    We_KpgGeneratePrime
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]: 
    WE_UINT32*      puiArra[OUT]:
    WE_UINT32*      puiArrb[IN]:
    WE_UINT32*      puiArrc[IN]:
    WE_UINT32*      puiArrd[IN]:
    WE_UINT32       uiDigits[IN]:
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 We_KpgGeneratePrime(WE_HANDLE hWeHandle,WE_UINT32* puiArra, 
                                  WE_UINT32* puiArrb, WE_UINT32* puiArrc, 
                                  WE_UINT32* puiArrd, WE_UINT32 uiDigits)
{
    WE_INT32  iStatus = 0;
    WE_UCHAR  aucBlock[WE_MAX_NN_DIGITS * WE_NN_DIGIT_LEN] = {0};
    WE_UINT32 uiT[WE_MAX_NN_DIGITS] = {0}, uiU[WE_MAX_NN_DIGITS] = {0};

    if (!puiArra || !puiArrb || !puiArrc ||!puiArrd)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    /* Generate random number between puiArrb and puiArrc. */

    /*get randome string*/
#ifdef WE_RSA_FOR_SEC
    iStatus= Sec_LibGenerateRandom( hWeHandle,  (WE_CHAR*)aucBlock,  (WE_INT32)uiDigits * WE_NN_DIGIT_LEN);
#else
    iStatus = We_LibGenerateRandom(hWeHandle,(WE_UCHAR*)aucBlock, (WE_INT32)uiDigits * WE_NN_DIGIT_LEN);
#endif
    if(M_WE_LIB_ERR_OK != iStatus)
    {
        return(iStatus);
    }

    We_NnDecode(puiArra, uiDigits, aucBlock, uiDigits * WE_NN_DIGIT_LEN);
    (WE_VOID)We_NnSub(uiT, puiArrc, puiArrb, uiDigits);
    WE_NN_ASSIGN_DIGIT(uiU, 1, uiDigits);
    (WE_VOID)We_NnAdd(uiT, uiT, uiU, uiDigits);
    We_NnMod(puiArra, puiArra, uiDigits, uiT, uiDigits);
    (WE_VOID)We_NnAdd(puiArra, puiArra, puiArrb, uiDigits);

    /* Adjust so that puiArra-1 is divisible by puiArrd. */

    We_NnMod(uiT, puiArra, uiDigits, puiArrd, uiDigits);
    (WE_VOID)We_NnSub(puiArra, puiArra, uiT, uiDigits);
    (WE_VOID)We_NnAdd(puiArra, puiArra, uiU, uiDigits);
    if(We_NnCmp(puiArra, puiArrb, uiDigits) < 0)
    {
        (WE_VOID)We_NnAdd(puiArra, puiArra, puiArrd, uiDigits);
    }
    if(We_NnCmp(puiArra, puiArrc, uiDigits) > 0)
    {
        (WE_VOID)We_NnSub(puiArra, puiArra, puiArrd, uiDigits);
    }

    /* Search to puiArrc in steps of puiArrd. */

    We_NnAssign(uiT, puiArrc, uiDigits);
    (WE_VOID)We_NnSub(uiT, uiT, puiArrd, uiDigits);

    while(!We_KpgProbablePrime (puiArra, uiDigits)) 
    {
        if(We_NnCmp (puiArra, uiT, uiDigits) > 0)
        {
            return(1);
        }
        (WE_VOID)We_NnAdd(puiArra, puiArra, puiArrd, uiDigits);
    }

    return(0);
}

#define WE_GET_CURRENT_MODULUS_LEN(modBits)        ((modBits + 7)/8)
#define WE_GET_CURRENT_PRIME_LEN(modBits)          (((modBits + 1)/2 + 7)/8)

/*==================================================================================================
FUNCTION: 
    We_KpgGenerateRSAKeyPairBasic
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    A MAIN FUNCTION TO GENERATE A RSA KEY PAIR
ARGUMENTS PASSED:
    WE_HANDLE               hSecHandle[IN/OUT]: sec global data
    St_SecRsaBasicKeyPair*  pstKey[OUT]:RSA KEY PAIR STRUCT
    WE_INT32                iModBits[IN]:RSA MODULUS BIT LENGTH
RETURN VALUE:
    RESULT
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    self
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 We_KpgGenerateRSAKeyPairBasic( WE_HANDLE hWeHandle,St_SecRsaBasicKeyPair* pstKey, WE_INT32 iModBits)
{
    WE_UINT32 auiD[WE_MAX_NN_DIGITS] = {0}, auiDP[WE_MAX_NN_DIGITS] = {0}, 
              auiDQ[WE_MAX_NN_DIGITS] = {0}, auiE[WE_MAX_NN_DIGITS] = {0}, 
              auiN[WE_MAX_NN_DIGITS] = {0}, auiP[WE_MAX_NN_DIGITS] = {0}, 
              auiPhiN[WE_MAX_NN_DIGITS] = {0}, auiPMinus1[WE_MAX_NN_DIGITS] = {0}, 
              auiQ[WE_MAX_NN_DIGITS] = {0}, auiQInv[WE_MAX_NN_DIGITS] = {0},
              auiQMinus1[WE_MAX_NN_DIGITS] = {0}, auiT[WE_MAX_NN_DIGITS] = {0}, 
              auiU[WE_MAX_NN_DIGITS] = {0}, auiV[WE_MAX_NN_DIGITS] = {0};

    WE_INT32 iStatus = 0;
    WE_UINT32 uiNDigits = 0, uiPBits = 0, uiPDigits = 0, uiQBits = 0;

    if (NULL == pstKey)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    uiNDigits = (WE_UINT32)((iModBits + WE_NN_DIGIT_BITS - 1) / WE_NN_DIGIT_BITS);
    uiPDigits = (uiNDigits + 1) / 2;
    uiPBits = ((WE_UINT32)iModBits + 1) / 2;
    uiQBits = (WE_UINT32)iModBits - uiPBits;

    auiE[0] = 65537;

    We_NnAssign2Exp(auiT, uiPBits-1, uiPDigits);
    We_NnAssign2Exp(auiU, uiPBits-2, uiPDigits);
    (WE_VOID)We_NnAdd(auiT, auiT, auiU, uiPDigits);
    WE_NN_ASSIGN_DIGIT(auiV, 1, uiPDigits);
    (WE_VOID)We_NnSub(auiV, auiT, auiV, uiPDigits);
    (WE_VOID)We_NnAdd(auiU, auiU, auiV, uiPDigits);
    WE_NN_ASSIGN_DIGIT(auiV, 2, uiPDigits);

    do 
    {
        iStatus = We_KpgGeneratePrime( hWeHandle,auiP, auiT, auiU, auiV, uiPDigits);
        if(iStatus)
        {
            return(iStatus);
        }
    }while(!We_KpgRSAFilter(auiP, uiPDigits, auiE, 1));

    /* Generate prime auiQ between 3*2^(uiQBits-2) and 2^uiQBits-1, searching
         in steps of 2, until one satisfies gcd (auiQ-1, auiE) = 1. */

    We_NnAssign2Exp(auiT, uiQBits-1, uiPDigits);
    We_NnAssign2Exp(auiU, uiQBits-2, uiPDigits);
    (WE_VOID)We_NnAdd(auiT, auiT, auiU, uiPDigits);
    WE_NN_ASSIGN_DIGIT(auiV, 1, uiPDigits);
    (WE_VOID)We_NnSub(auiV, auiT, auiV, uiPDigits);
    (WE_VOID)We_NnAdd(auiU, auiU, auiV, uiPDigits);
    WE_NN_ASSIGN_DIGIT(auiV, 2, uiPDigits);

    do 
    {
        iStatus = We_KpgGeneratePrime( hWeHandle,auiQ, auiT, auiU, auiV, uiPDigits);
        if(iStatus)
        {
            return(iStatus);
        }
    }while(!We_KpgRSAFilter(auiQ, uiPDigits, auiE, 1));

    /* Sort so that auiP > auiQ. (auiP = auiQ case is extremely unlikely. */

    if(We_NnCmp(auiP, auiQ, uiPDigits) < 0)
    {
        We_NnAssign(auiT, auiP, uiPDigits);
        We_NnAssign(auiP, auiQ, uiPDigits);
        We_NnAssign(auiQ, auiT, uiPDigits);
    }

    /* Compute auiN = pq, auiQInv = auiQ^{-1} mod auiP, auiD = auiE^{-1} mod (auiP-1)(auiQ-1),
         auiDP = auiD mod auiP-1, auiDQ = auiD mod auiQ-1. */

    We_NnMult(auiN, auiP, auiQ, uiPDigits);
    We_NnModInv(auiQInv, auiQ, auiP, uiPDigits);

    WE_NN_ASSIGN_DIGIT(auiT, 1, uiPDigits);
    (WE_VOID)We_NnSub(auiPMinus1, auiP, auiT, uiPDigits);
    (WE_VOID)We_NnSub(auiQMinus1, auiQ, auiT, uiPDigits);
    We_NnMult(auiPhiN, auiPMinus1, auiQMinus1, uiPDigits);

    We_NnModInv(auiD, auiE, auiPhiN, uiNDigits);
    We_NnMod(auiDP, auiD, uiNDigits, auiPMinus1, uiPDigits);
    We_NnMod(auiDQ, auiD, uiNDigits, auiQMinus1, uiPDigits);

    pstKey->usModLen = (WE_UINT16)WE_GET_CURRENT_MODULUS_LEN(iModBits);
    We_NnEncode(pstKey->aucModulus, pstKey->usModLen, auiN, uiNDigits);

    pstKey->usPubExpLen = 3;
    We_NnEncode(pstKey->aucPubExp, pstKey->usPubExpLen, auiE, 1);

    pstKey->usExpLen = (WE_UINT16)WE_GET_CURRENT_MODULUS_LEN(iModBits);
    We_NnEncode(pstKey->aucExponent, pstKey->usExpLen, auiD, uiNDigits);

    pstKey->usPrimeLen = (WE_UINT16)WE_GET_CURRENT_PRIME_LEN(iModBits);
    We_NnEncode(pstKey->aucPrime[0], pstKey->usPrimeLen, auiP, uiPDigits);
    We_NnEncode(pstKey->aucPrime[1], pstKey->usPrimeLen, auiQ, uiPDigits);

    pstKey->usPrimeExpLen = (WE_UINT16)WE_GET_CURRENT_PRIME_LEN(iModBits);
    We_NnEncode(pstKey->aucPrimeExp[0], pstKey->usPrimeExpLen, auiDP, uiPDigits);
    We_NnEncode(pstKey->aucPrimeExp[1], pstKey->usPrimeExpLen, auiDQ, uiPDigits);

    pstKey->usCoefLen = (WE_UINT16)WE_GET_CURRENT_PRIME_LEN(iModBits);
    We_NnEncode(pstKey->aucCoef, pstKey->usCoefLen, auiQInv, uiPDigits);

    return (0);
}



/*==================================================================================================
FUNCTION: 
    SEClib_GenerateRSAKeyPair
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
    TO GENERATE A RSA KEY PAIR IN SYNCHRONOUS
ARGUMENTS PASSED:
    WE_HANDLE       hSecHandle[IN/OUT]:sec global data
    WE_INT32        iTargetID[IN]:TARGET ID
    WE_UCHAR**      ppucPubkey[OUT]: RSA PUBLIC KEY DER ENCODED STRING
    WE_INT32*       piPubkeyLen[OUT]:RSA PUBLIC KEY STRING LENGTH
    WE_UCHAR**      ppucPrivkey[OUT]:RSA PRIATE KEY DER ENCODED STRING
    WE_INT32*       piPrivkeyLen[OUT]:RSA PRIVATE KEY STRING
    WE_INT32        iModBits[IN]:RSA MODULUS BIT LENGTH
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

WE_INT32 SEClib_GenerateRSAKeyPair(WE_HANDLE hWeHandle,WE_UCHAR **ppucPubkey, 
                                      WE_INT32 *piPubkeyLen, 
                                      WE_UCHAR **ppucPrivkey, 
                                      WE_INT32 *piPrivkeyLen,
                                      WE_INT32 iModBits)
{
    St_SecRsaBasicKeyPair   stKey = {0};
    St_SecRsaPublicKey      stPubKey = {0};  
    WE_INT32                iRv = M_WE_LIB_ERR_OK;

    if (!ppucPubkey || !piPubkeyLen || !ppucPrivkey ||!piPrivkeyLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    
    if((iModBits > 1024) || (iModBits < 508))
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    iRv = We_KpgGenerateRSAKeyPairBasic(hWeHandle,&stKey, iModBits);
    if(M_WE_LIB_ERR_OK != iRv)
    {
        return iRv;
    }
    stPubKey.pucExponent = stKey.aucPubExp;
    stPubKey.usExpLen = stKey.usPubExpLen;
    stPubKey.pucModulus = stKey.aucModulus;
    stPubKey.usModLen = stKey.usModLen;
    iRv = We_KpgConvertRSAPubKey(stPubKey, ppucPubkey, piPubkeyLen);
    if(M_WE_LIB_ERR_OK != iRv)
    {
        return iRv;
    }
    iRv = We_KpgConvertRSAPrivKey(stKey, ppucPrivkey, piPrivkeyLen);
    if(M_WE_LIB_ERR_OK != iRv)
    {
        M_WE_SELF_FREE(*ppucPubkey);
        return iRv;
    }
    
    return M_WE_LIB_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    We_KpgConvertRSAPrivKey
CREATE DATE:
    2006-12-15
AUTHOR:
    tang
DESCRIPTION:
     CONVERT RSA PRIVATE KEY STRUCT TO ASN DER CODE BIT STRING   
ARGUMENTS PASSED:
    St_SecRsaPrivateKey     stKey[IN]:RSA PRIVATE KEY
    WE_UCHAR**              ppucKey[OUT]:DER ENCODED BIT STRING
    WE_INT32*               piKeyLen[OUT]:DER ENCODED STRING LENGTH
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
WE_INT32 We_KpgConvertRSAPrivKey(St_SecRsaPrivateKey stKey, 
                                         WE_UCHAR** ppucKey, 
                                         WE_INT32* piKeyLen)
{
    WE_UINT16   usMsgLen = 0;
    WE_UINT16   usHdrLen = 0;
    WE_UINT16   usMLen = 0;
    WE_UINT16   usPELen = 0;
    WE_UINT16   usELen = 0;
    WE_UINT16   usPLen1 = 0;
    WE_UINT16   usPLen2 = 0;
    WE_UINT16   usPELen1 = 0;
    WE_UINT16   usPELen2 = 0;
    WE_UINT16   usCLen = 0;
    WE_UINT8    ucPrependModZero = 0;
    WE_UINT8    ucPrependPubExpZero = 0;
    WE_UINT8    ucPrependExpZero = 0;
    WE_UINT8    ucPrependPrime1Zero = 0;
    WE_UINT8    ucPrependPrime2Zero = 0;
    WE_UINT8    ucPrependPrimeExp1Zero = 0;
    WE_UINT8    ucPrependPrimeExp2Zero = 0;
    WE_UINT8    ucPrependCoefZero = 0;
    WE_UINT16   usM = stKey.usModLen;
    WE_UINT16   usPE = stKey.usPubExpLen;
    WE_UINT16   usE = stKey.usExpLen;
    WE_UINT16   usP1 = stKey.usPrimeLen;
    WE_UINT16   usP2 = stKey.usPrimeLen;
    WE_UINT16   usPE1 = stKey.usPrimeExpLen;
    WE_UINT16   usPE2 = stKey.usPrimeExpLen;
    WE_UINT16   usC = stKey.usCoefLen;
    WE_UCHAR*   pucP = NULL;
    WE_INT32    iIndex = 0;
    WE_UINT8    ucBigEndian = 0;
    WE_UINT16   usU = 0x0102;

    if(!ppucKey || !piKeyLen)
    {
        return M_WE_LIB_ERR_INVALID_PARAMETER;
    }
    /* Check how bytes are positioned on the current platform */
    if (0x01 == *((WE_UINT8*)&(usU)))
    {
        ucBigEndian = 1;
    }
    else
    {
        ucBigEndian = 0;
    }

    /* Calculate length of DER representation of the public stKey */
    /* usMLen is the length of the modulus (integer) header */
    /* usELen is the length of the exponent (integer) header */
    /* usHdrLen is the length of the SEQUENCE (encapsulating) header */
    /*version 0*/
    usMsgLen += 3;
    /*modulus*/    
    if (((WE_UINT8)*(stKey.aucModulus)) > 127)
    {
        ucPrependModZero = 1;
        usM ++;
    }
    usMLen = 2;
    if (usM > 127)
    {
        usMLen ++;
    }
    if (usM > 255)
    {
        usMLen ++;
    }
    usMsgLen += (usM + usMLen);
    
    /*pub exponent*/
    if (((WE_UINT8)*(stKey.aucPubExp)) > 127)
    {
        ucPrependPubExpZero = 1;
        usPE ++;
    }
    usPELen = 2;
    if (usPE > 127)
    {
        usPELen ++;
    }
    if (usPE > 255)
    {
        usPELen ++;
    }
    usMsgLen += (usPE + usPELen);
    
    /*pri exponent*/
    if (((WE_UINT8)*(stKey.aucExponent)) > 127)
    {
        ucPrependExpZero = 1;
        usE ++;
    }
    usELen = 2;
    if (usE > 127)
    {
        usELen ++;
    }
    if (usE > 255)
    {
        usELen ++;
    }
    usMsgLen += (usE + usELen);
    
    /*prime1*/
    if (((WE_UINT8)*(stKey.aucPrime[0])) > 127)
    {
        ucPrependPrime1Zero = 1;
        usP1 ++;
    }
    usPLen1 = 2;
    if (usP1 > 127)
    {
        usPLen1 ++;
    }
    if (usP1 > 255)
    {
        usPLen1 ++;
    }
    usMsgLen += (usP1 + usPLen1);
    
    /*prime2*/
    if (((WE_UINT8)*(stKey.aucPrime[1])) > 127)
    {
        ucPrependPrime2Zero = 1;
        usP2 ++;
    }
    usPLen2= 2;
    if (usP2 > 127)
    {
        usPLen2 ++;
    }
    if (usP2 > 255)
    {
        usPLen2 ++;
    }
    usMsgLen += (usP2 + usPLen2);
    
    /*prime exp1*/
    if (((WE_UINT8)*(stKey.aucPrimeExp[0])) > 127)
    {
        ucPrependPrimeExp1Zero = 1;
        usPE1 ++;
    }
    usPELen1 = 2;
    if (usPE1 > 127)
    {
        usPELen1 ++;
    }
    if (usPE1 > 255)
    {
        usPELen1 ++;
    }
    usMsgLen += (usPE1 + usPELen1);
    
    /*prime exp2*/
    if (((WE_UINT8)*(stKey.aucPrimeExp[1])) > 127)
    {
        ucPrependPrimeExp2Zero = 1;
        usPE2 ++;
    }
    usPELen2 = 2;
    if (usPE2 > 127)
    {
        usPELen2 ++;
    }
    if (usPE2 > 255)
    {
        usPELen2 ++;
    }
    usMsgLen += (usPE2 + usPELen2);
        
    /*ceof*/
    if (((WE_UINT8)*(stKey.aucCoef)) > 127)
    {
        ucPrependCoefZero = 1;
        usC ++;
    }
    usCLen = 2;
    if (usC > 127)
    {
        usCLen ++;
    }
    if (usC > 255)
    {
        usCLen ++;
    }
    usMsgLen += (usC + usCLen);

    usHdrLen = 2;
    if (usMsgLen > 127)
    {
        usHdrLen ++;
    }
    if (usMsgLen > 255)
    {
        usHdrLen ++;
    }
    /* DER encode stKey (ASN.1) */
    *piKeyLen = usHdrLen + usMsgLen;
    pucP = (WE_UCHAR*)WE_MALLOC(usHdrLen + usMsgLen);
    if(!pucP)
    {
        return M_WE_LIB_ERR_INSUFFICIENT_MEMORY;
    }
    *ppucKey = pucP;
    /*header*/
    *pucP++ = '\x30'; /* SEQUENCE */
    /*msglen*/
    switch (usHdrLen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usMsgLen)); /* write message length (1 byte) */
            }
            break;
        }
        
        case 3: 
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER - 1 Byte */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usMsgLen)); /* write message length */
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER - 2 Bytes */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usMsgLen));
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usMsgLen))+1); /* write message length */
                *pucP++ = *((WE_UCHAR*)&(usMsgLen));
            }
            break;
        }
        default:
            break;
    } /* This provides for message lengths up to 65535 bits */

    /*version*/
    *pucP++ = 0x02; /* INTEGER */
    *pucP++ = 0x01; /* len */
    *pucP++ = 0x00; /* version */

    /*modulus*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usMLen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usM));
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usM))+1); /* write modulus length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usM));
            }
            break;
        }
        default:
            break;
    }
    
    if (ucPrependModZero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usModLen; iIndex ++) /* write modulus (original length  only) */
    {
        *pucP++ = stKey.aucModulus[iIndex];
    }

    /*pub exp*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usPELen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usPE));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usPE));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usPE));
                *pucP++ = *(((WE_UCHAR*)&(usPE))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usPE));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependPubExpZero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usPubExpLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.aucPubExp[iIndex];
    }

    /*pri exp*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usELen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usE));
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usE))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usE));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependExpZero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usExpLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.aucExponent[iIndex];
    }

    /*prime1*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usPLen1)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usP1))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usP1));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usP1))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usP1));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usP1));
                *pucP++ = *(((WE_UCHAR*)&(usP1))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usP1))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usP1));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependPrime1Zero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usPrimeLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.aucPrime[0][iIndex];
    }

    /*prime2*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usPLen2)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usP2))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usP2));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usP2))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usP2));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usP2));
                *pucP++ = *(((WE_UCHAR*)&(usP2))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usP2))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usP2));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependPrime2Zero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usPrimeExpLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.aucPrime[1][iIndex];
    }

    /*prime exp1*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usPELen1)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE1))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usPE1));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE1))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usPE1));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usPE1));
                *pucP++ = *(((WE_UCHAR*)&(usPE1))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE1))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usPE1));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependPrimeExp1Zero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usPrimeExpLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.aucPrimeExp[0][iIndex];
    }

    /*prime exp2*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usPELen2)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE2))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usPE2));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE2))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usPE2));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usPE2));
                *pucP++ = *(((WE_UCHAR*)&(usPE2))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usPE2))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usPE2));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependPrimeExp2Zero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usPrimeExpLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.aucPrimeExp[1][iIndex];
    }

    /*coefficient*/
    *pucP++ = 0x02; /* INTEGER */
    switch (usCLen)
    {
        case 2:
        {
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usC))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usC));
            }
            break;
        }
        
        case 3:
        {
            *pucP++ = (WE_UCHAR)0x81; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *(((WE_UCHAR*)&(usC))+1); /* write exponent length (1 byte) */
            }
            else
            {
                *pucP++ = *((WE_UCHAR*)&(usC));
            }
            break;
        }
        
        case 4:
        {
            *pucP++ = (WE_UCHAR)0x82; /* LENGTH SPECIFIER */
            if (ucBigEndian)
            {
                *pucP++ = *((WE_UCHAR*)&(usC));
                *pucP++ = *(((WE_UCHAR*)&(usC))+1); /* write exponent length (2 bytes) */
            }
            else
            {
                *pucP++ = *(((WE_UCHAR*)&(usC))+1); /* write exponent length (2 bytes) */
                *pucP++ = *((WE_UCHAR*)&(usC));
            }
            break;
        }
        default:
            break;
    }
    if (ucPrependCoefZero)
    {
        *pucP++ = 0x00;
    }
    for (iIndex = 0; iIndex < stKey.usCoefLen; iIndex++) /* write exponent (original length  only) */
    {
        *pucP++ = stKey.aucCoef[iIndex];
    }

    return M_WE_LIB_ERR_OK;
}


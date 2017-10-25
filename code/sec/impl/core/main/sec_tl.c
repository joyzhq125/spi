/*=================================================================================================
    MODULE NAME : sec_tl.c
    MODULE NAME : SEC

    GENERAL DESCRIPTION    
        This file offers four function to verify that the platform if big-endian 
        or little-endian, and then convert, store or export between two different 
        types.
        
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                  Tracking
    Date          Author          Number      Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-07-11     Sam              None           Init  

==================================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "sec_comm.h"

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 Base64_nextBase64Char(WE_UINT8 *pucOut, WE_UINT8 *pucData, WE_UINT32 uiDataLength);

/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_StoreStrUint8to16
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge the platform is big endian or little endian, evaluate and get 16 bit value.
ARGUMENTS PASSED: 
    pucStr8[IN]: 8-bit pointer 
    pusAdd16[OUT]: 16-bit pointer 
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
void Sec_StoreStrUint8to16( const WE_UINT8 *pucStr8, WE_UINT16 *pusAdd16 )
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr8) || (NULL == pusAdd16))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {/* big endian */
        *((WE_UINT8 *)pusAdd16) = pucStr8[0];
        *(((WE_UINT8 *)pusAdd16)+1) = pucStr8[1];
    }
    else                        
    {/* little endian */
        *((WE_UINT8 *)pusAdd16) = pucStr8[1];
        *(((WE_UINT8 *)pusAdd16)+1) = pucStr8[0];
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_StoreStrUint8to32
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge the platform is big endian or little endian, evaluate and get 32 bit value.
ARGUMENTS PASSED: 
    pucStr8[IN]: 8-bit pointer 
    puiAdd32[OUT]: 32-bit pointer 
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
void Sec_StoreStrUint8to32( const WE_UINT8 *pucStr8, WE_UINT32 *puiAdd32 )
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr8) || (NULL == puiAdd32))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {/* big endian */
        *((WE_UINT8 *)puiAdd32) = pucStr8[0];
        *(((WE_UINT8 *)puiAdd32)+1) = pucStr8[1];
        *(((WE_UINT8 *)puiAdd32)+2) = pucStr8[2];
        *(((WE_UINT8 *)puiAdd32)+3) = pucStr8[3];
    }
    else                        
    {/* little endian */
        *((WE_UINT8 *)puiAdd32) = pucStr8[3];
        *(((WE_UINT8 *)puiAdd32)+1) = pucStr8[2];
        *(((WE_UINT8 *)puiAdd32)+2) = pucStr8[1];
        *(((WE_UINT8 *)puiAdd32)+3) = pucStr8[0];
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_ExportStrUint16to8
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge the platform is big endian or little endian, put the 16 bit value to 8 bit.
ARGUMENTS PASSED: 
    pusAdd16[IN] : 16-bit pointer 
    pucStr8[OUT]: 8-bit pointer 
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
void Sec_ExportStrUint16to8( WE_UINT16 *pusAdd16, WE_UINT8 *pucStr8 )
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr8) || (NULL == pusAdd16))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {/* big endian */
        pucStr8[0] = *((WE_UINT8 *)pusAdd16);
        pucStr8[1] = *(((WE_UINT8 *)pusAdd16)+1);
    }
    else                        
    {/* little endian */
        pucStr8[1] = *((WE_UINT8 *)pusAdd16);
        pucStr8[0] = *(((WE_UINT8 *)pusAdd16)+1);
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_ExportStrUint32to8
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Judge the platform is big endian or little endian, put the 32 bit value to 8 bit.
ARGUMENTS PASSED: 
    puiAdd32[IN]: 32-bit pointer 
    pucStr8[OUT]: 8-bit pointer 
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
void Sec_ExportStrUint32to8( WE_UINT32 *puiAdd32, WE_UINT8 *pucStr8 )
{
    WE_UINT16 usTemp = 0x0102; 

    if ((NULL == pucStr8) || (NULL == puiAdd32))
    {
        return ;
    }
    
    if (0x01 == *((WE_UINT8 *)&(usTemp))) 
    {/* big endian */
        pucStr8[0] = *((WE_UINT8 *)puiAdd32);
        pucStr8[1] = *(((WE_UINT8 *)puiAdd32)+1);
        pucStr8[2] = *(((WE_UINT8 *)puiAdd32)+2);
        pucStr8[3] = *(((WE_UINT8 *)puiAdd32)+3);
    }
    else                        
    {/* little endian */
        pucStr8[3] = *((WE_UINT8 *)puiAdd32);
        pucStr8[2] = *(((WE_UINT8 *)puiAdd32)+1);
        pucStr8[1] = *(((WE_UINT8 *)puiAdd32)+2);
        pucStr8[0] = *(((WE_UINT8 *)puiAdd32)+3);
    }
}

/*=====================================================================================
FUNCTION: 
    Base64_nextBase64Char
CREATE DATE: 
    2006-6-20
AUTHOR: 
    Evan lin
DESCRIPTION:
    return the corresponding character in table.
ARGUMENTS PASSED:
    pucOut[OUT]: pointer to the corresponding character in table.
    pucData[IN]: pointer to a encoded data block.
    uiDataLength[IN]: the size of the data block that are available in pucData.
RETURN VALUE:
    M_SEC_ERR_OK: success.
    others: fail.
USED GLOBAL VARIABLES:
    none.    
USED STATIC VARIABLES:
    none.    
CALL BY:
    omit.    
IMPORTANT NOTES:
    omit.
=====================================================================================*/
static WE_INT32 Base64_nextBase64Char(WE_UINT8 *pucOut, WE_UINT8 *pucData, WE_UINT32 uiDataLength)
{
    WE_UINT8  ucChar = 0;
    WE_INT32  iIndexI = 0;
    WE_UINT32 uiLen = uiDataLength;
    
    if ((0 == uiDataLength) || (NULL == pucData) ||  (NULL == pucOut))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    do 
    {
        ucChar = *pucData;
        if ((ucChar>='0')&&(ucChar<='9'))      
        {
            pucOut[iIndexI++] = ucChar + 4;   
        }
        else if ((ucChar>='A')&&(ucChar<='Z'))  
        {
            pucOut[iIndexI++] = ucChar - 65; 
        }
        else if ((ucChar>='a')&&(ucChar<='z'))  
        {
            pucOut[iIndexI++] = ucChar - 71;  
        }        
        else if (ucChar == '/')           
        {
            pucOut[iIndexI++] = 63;                     
        }
        else if (ucChar == '+')              
        {
            pucOut[iIndexI++] = 62;                   
        }
        else if (ucChar == '=')
        {
            pucOut[iIndexI++] = 64;   /*   padding character        */ 
        }        
        else
        {        
            return M_SEC_ERR_INVALID_PARAMETER;
        }        
        uiLen--;
        pucData++;        
    } while (uiLen > 0);
    
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    Base64_Encode
CREATE DATE: 
    2006-6-20
AUTHOR: 
    Evan lin
DESCRIPTION:
    Encodes a chunk of data according to the base64 encoding rules. 
ARGUMENTS PASSED:
    ppucTargetOut[OUT]: return the location of encoded data block.
    pucDataIn[IN]: pointer to a data block that contains the clear data that are to be encoded.
    puiDataLength[IN/OUT]: the size of the data block that are available in pucDataIn,
                           the size of the data block that are available in pucTargetOut.
RETURN VALUE:
    M_SEC_ERR_OK: success.
    others: fail.
USED GLOBAL VARIABLES:
    none.    
USED STATIC VARIABLES:
    none.    
CALL BY:
    omit.    
IMPORTANT NOTES:
    omit.
=====================================================================================*/
WE_INT32 Base64_Encode (WE_UINT8 **ppucTargetOut,
                                WE_UINT8 *pucDataIn, WE_UINT32 *puiDataLength)

{   
    WE_UINT8 *pucSourceData = NULL;
    WE_UINT8 *pucOut = NULL;    /*compute the size of an encoded document with the given size.*/
    WE_UINT32 uiCopySize = 0;           /*the size of the data block that has been encode*/
    WE_UINT32 uiDataProcessed = 0;  /* the size of the data block that left to be encode */
    WE_UINT16 usTemp0 = 0;
    WE_UINT16 usTemp1 = 0;
    WE_UINT16 usTemp2 = 0;
    WE_UINT16 usTemp3 = 0;
    WE_UINT16 usByte = 0;
    WE_UINT8  aucTab[] =  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";  
    WE_UINT32 uiEncodedSize = 0;
    
    /* Check for NULL data buffer*/
    if ((NULL == pucDataIn) || (NULL == puiDataLength) || 
        (NULL == ppucTargetOut))
    { 
        return M_SEC_ERR_INVALID_PARAMETER;
    }    
    
    pucSourceData = pucDataIn;
    uiEncodedSize = ((*puiDataLength) / 3) * 4;
    if ((*puiDataLength) % 3 != 0) 
    {
        uiEncodedSize += 4;
    }
    pucOut = (WE_UCHAR *)WE_MALLOC(uiEncodedSize * sizeof(WE_UCHAR));
    if (NULL == pucOut)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    *ppucTargetOut = pucOut;
    uiDataProcessed = *puiDataLength;
    
    while (uiDataProcessed > 0) 
    {
        usByte = (WE_UINT16)(*pucSourceData); 
        pucSourceData++; 
        uiDataProcessed--;
        usTemp0 =  usByte >> 2;
        usTemp1 =  (usByte & 0x0003) << 4;
        
        if (uiDataProcessed > 0)
        {            
            usByte = (WE_UINT16)(*pucSourceData); 
            pucSourceData++; 
            uiDataProcessed--;
            
            usTemp1 += (usByte >> 4);
            usTemp2 =  (usByte & 0x000F) << 2;
            
            if (uiDataProcessed > 0)
            {                               
                usByte = (WE_UINT16) *pucSourceData; 
                pucSourceData ++; 
                uiDataProcessed --;
                
                usTemp2 += (usByte & 0x00C0) >> 6;
                usTemp3 =  usByte & 0x003F;
            }
            else
            {
                usTemp3 = 64;   /* index to the padding WE_INT8 '=' */
            }
        }
        else
        {
            usTemp2 = usTemp3 = 64;  /* index to the padding WE_INT8 '=' */
        }
        pucOut[uiCopySize++] = aucTab[usTemp0];
        pucOut[uiCopySize++] = aucTab[usTemp1];
        pucOut[uiCopySize++] = aucTab[usTemp2];
        pucOut[uiCopySize++] = aucTab[usTemp3];
    }/* end while (uiDataProcessed >0) */    
    
    *puiDataLength = uiCopySize;    
    return M_SEC_ERR_OK;
}


/*=====================================================================================
FUNCTION: 
    Base64_Decode
CREATE DATE: 
    2006-6-20
AUTHOR: 
    Evan lin
DESCRIPTION:
    decode a base64- encoded block of data. 
ARGUMENTS PASSED:
    ppucTargetOut[OUT]: return the location of encoded data block.
    pucDataIn[IN]: pointer to a data block that contains the clear data that are to be decoded.
    puiDataLength[IN/OUT]: the size of the data block that are available in pucDataIn,
                           the size of the data block that are available in pucTargetOut.
RETURN VALUE:
    M_SEC_ERR_OK: success.
    others: fail.
USED GLOBAL VARIABLES:
    none.    
USED STATIC VARIABLES:
    none.    
CALL BY:
    omit.    
IMPORTANT NOTES:
    omit.
=====================================================================================*/
WE_INT32 Base64_Decode (WE_UINT8 **ppucTargetOut, 
                                WE_UINT8 *pucDataIn, WE_UINT32 *puiDataLength)       
{
    WE_UINT32 uiCount = 0; 
    WE_UINT32 uiDecodedSize = 0;
    WE_UINT8  *pucOut = NULL;
    WE_UINT8  *pucRealOut = NULL;    
    WE_INT32  iResult = 0;
    WE_INT32  iIndexI = 0;
    
    if ((NULL == pucDataIn) || (NULL == puiDataLength) || (NULL == ppucTargetOut) ||
        (*puiDataLength < 4) || ((*puiDataLength)%4 != 0))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    uiCount = *puiDataLength; 
    pucOut = (WE_UCHAR *)WE_MALLOC(uiCount * sizeof(WE_UCHAR));
    if (NULL == pucOut)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pucRealOut = (WE_UCHAR *)WE_MALLOC((uiCount/4*3)* sizeof(WE_UCHAR));
    if (NULL == pucRealOut)
    {
        WE_FREE(pucOut);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    *ppucTargetOut = pucRealOut;
    
    iResult = (WE_INT32)Base64_nextBase64Char(pucOut, pucDataIn, uiCount);
    if (iResult != M_SEC_ERR_OK)
    {
        WE_FREE(pucOut);
        WE_FREE(pucRealOut);
        return iResult;
    }
    
    while (uiCount > 0)
    {
        *pucRealOut = (WE_UINT8)(pucOut[iIndexI] << 2) + (pucOut[iIndexI+1] >> 4);         
        pucRealOut++; 
        uiDecodedSize++;
        if (pucOut[iIndexI+2] != 64)
        {
            *pucRealOut = ((pucOut[iIndexI+1] & 0x000f)<<4) + (pucOut[iIndexI+2]>>2);
            pucRealOut++; 
            uiDecodedSize++; 
            if (pucOut[iIndexI+3] != 64)
            {
                *pucRealOut = ((pucOut[iIndexI+2] & 0x0003) << 6) + pucOut[iIndexI+3];
                pucRealOut ++; 
                uiDecodedSize ++; 
            }            
        }    
        uiCount -= 4;
        iIndexI += 4;
    }        
    *puiDataLength = uiDecodedSize;    
    WE_FREE(pucOut);
    return M_SEC_ERR_OK;
}


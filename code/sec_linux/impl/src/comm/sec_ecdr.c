/*==================================================================================================
    FILE NAME :
        wap_ecdr.c
    MODULE NAME :
        WAP_ECDR
    GENERAL DESCRIPTION
        This file creates encoder and decoder,after converts between an external 
        and an internal representation,then destroy the encoder or decoder.
        Encode functions encode the internal to external representation.
        Decode functions decode the external to internal representation.
        GetEncodeSize functions calculate the buffer size required to do an encoding operation.
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-05-18 ChenZhifeng    none      Init
    2006-08-07 MaZhiqiang     None      Optimization
==================================================================================================*/

/***************************************************************************************************
*   Include File Section
***************************************************************************************************/

#include "sec_comm.h"
#include "sec_ecdr.h"

/**************************************************************************************
*   Macro Define Section
**************************************************************************************/

/**************************************************************************************
*   Type Define Section
**************************************************************************************/


/**************************************************************************************
*   Prototype Declare Section
**************************************************************************************/
/*==================================================================================================
FUNCTION: 
    WapEcdr_CreateEncoder
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Create a new Encoder structure
ARGUMENTS PASSED:
    WE_INT32 iLength : The length of the data to be encoded
RETURN VALUE:
    NULL : Error
    St_SecEcdrEncoder *pstEncoder : the new Encoder structure created
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
St_SecEcdrEncoder *SecEcdr_CreateEncoder
(
    WE_INT32     iLength
)
{
    St_SecEcdrEncoder   *pstEncoder = 
        (St_SecEcdrEncoder*)WE_MALLOC ( (WE_ULONG)sizeof(St_SecEcdrEncoder) );

    if( (NULL == pstEncoder) || (iLength < 1) )
    {
        if (pstEncoder != NULL)
        {
            WE_FREE(pstEncoder);
        }
        return NULL;
    }

    pstEncoder->pucData = (WE_UCHAR*)WE_MALLOC( (WE_ULONG)iLength );
    if( NULL == pstEncoder->pucData )
    {
        WE_FREE(pstEncoder);
        return NULL;
    }
    WE_MEMSET( (void *)(pstEncoder->pucData), 0, (WE_UINT32)iLength );
    pstEncoder->iLength = iLength;
    pstEncoder->iPosition = 0;
    
    return pstEncoder;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DestroyEncoder
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Destroy the encoder structure
ARGUMENTS PASSED:
    St_SecEcdrEncoder *pstEncoder : pointer to the encoder structure to be destroyed
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DestroyEncoder
(
    St_SecEcdrEncoder *pstEncoder
)
{
    if( NULL == pstEncoder )
    {
        return -1;
    }
    
    if( NULL != pstEncoder->pucData )
    {
        WE_FREE( pstEncoder->pucData );
    }
    WE_FREE( pstEncoder );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_ChangeEncodePosition
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Change the current buffer position of the conversion object
ARGUMENTS PASSED:
    St_SecEcdrEncoder  *pstEncoder  : pointer to the Encoder to change the current buffer position
    WE_INT32            iOffset     : the interval to move
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_ChangeEncodePosition
(
    St_SecEcdrEncoder  *pstEncoder,
    WE_INT32            iOffset
)
{
    WE_INT32 iCurrentPos = 0;
    
    if( NULL == pstEncoder )
    {
        return -1;
    }
    iCurrentPos = pstEncoder->iPosition + iOffset;
    if( iCurrentPos < 0 )
    {
        return -1;
    }
    pstEncoder->iPosition = iCurrentPos;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeUint8
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode 8 bit unsigned integer
ARGUMENTS PASSED:
    St_SecEcdrEncoder   *pstEncoder : pointer to a encoder
    WE_UINT8            *pucSrc     : pointer to a UINT8 that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeUint8
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT8            *pucSrc
)
{
    WE_INT32 iTmp = 0;
    
    if( (NULL == pstEncoder) || (NULL == pucSrc) )
    {
        return -1;
    }
    
    iTmp = pstEncoder->iPosition + 1;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }

    pstEncoder->pucData[pstEncoder->iPosition] = *pucSrc;
    pstEncoder->iPosition++;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeInt8
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode 8 bit signed integer
ARGUMENTS PASSED:
    St_SecEcdrEncoder   *pstEncoder : pointer to a encoder
    WE_INT8             *pcSrc     : pointer to a INT8 that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeInt8
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_INT8             *pcSrc
)
{
    WE_INT32 iTmp = 0;
    
    if( (NULL == pstEncoder) || (NULL == pcSrc) )
    {
        return -1;
    }
    
    iTmp = pstEncoder->iPosition + 1;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }

    pstEncoder->pucData[pstEncoder->iPosition] = (WE_UCHAR)*pcSrc;
    pstEncoder->iPosition++;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeUint16
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode 16 bit unsigned integer
ARGUMENTS PASSED:
    St_SecEcdrEncoder   *pstEncoder : pointer to a encoder
    WE_UINT16           *pusSrc     : pointer to a UINT16 that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeUint16
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT16           *pusSrc
)
{
    WE_INT32 iTmp = 0;
    
    if( (NULL == pstEncoder) || (NULL == pusSrc) )
    {
        return -1;
    }
    
    iTmp = pstEncoder->iPosition + 2;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }
        
    pstEncoder->pucData[pstEncoder->iPosition] = (WE_UCHAR)((*pusSrc>>8) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 1] = (WE_UCHAR)(*pusSrc & 0xFF);
    pstEncoder->iPosition += 2;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeInt16
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode 16 bit signed integer
ARGUMENTS PASSED:
    St_SecEcdrEncoder  *pstEncoder : pointer to a encoder
    WE_INT16           *psSrc      : pointer to a INT16 that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeInt16
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_INT16            *psSrc
)
{
    WE_INT32 iTmp = 0;
    if( (NULL == pstEncoder) || (NULL == psSrc) )
    {
        return -1;
    }

    iTmp = pstEncoder->iPosition + 2;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }
       
    pstEncoder->pucData[pstEncoder->iPosition]   = (WE_UCHAR)((*psSrc>>8) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 1] = (WE_UCHAR)(*psSrc & 0xFF);
    pstEncoder->iPosition += 2;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeUint32
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode 32 bit unsigned integer
ARGUMENTS PASSED:
    St_SecEcdrEncoder   *pstEncoder : pointer to a encoder
    WE_UINT32           *puiSrc     : pointer to a UNINT32 that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeUint32
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT32           *puiSrc
)
{
    WE_INT32 iTmp = 0;
    if( (NULL == pstEncoder) || (NULL == puiSrc) )
    {
        return -1;
    }

    iTmp = pstEncoder->iPosition + 4;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }

    pstEncoder->pucData[pstEncoder->iPosition]   = (WE_UCHAR)((*puiSrc>>24) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 1] = (WE_UCHAR)((*puiSrc>>16) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 2] = (WE_UCHAR)((*puiSrc>>8 ) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 3] = (WE_UCHAR)(*puiSrc & 0xFF);
    pstEncoder->iPosition += 4;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeInt32
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode 32 bit signed integer
ARGUMENTS PASSED:
    St_SecEcdrEncoder  *pstEncoder : pointer to a encoder
    WE_INT32           *piSrc      : pointer to a INT32 that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeInt32
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_INT32            *piSrc
)
{
    WE_INT32 iTmp = 0;
    if( (NULL == pstEncoder) || (NULL == piSrc) )
    {
        return -1;
    }

    iTmp = pstEncoder->iPosition + 4;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }
    pstEncoder->pucData[pstEncoder->iPosition]   = (WE_UCHAR)((*piSrc>>24) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 1] = (WE_UCHAR)((*piSrc>>16) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 2] = (WE_UCHAR)((*piSrc>>8 ) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 3] = (WE_UCHAR)(*piSrc & 0xFF);
    pstEncoder->iPosition += 4;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeUintVar
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode a unsigned int var
ARGUMENTS PASSED:
    St_SecEcdrEncoder  *pstEncoder : pointer to a encoder
    WE_UINT32          *puiSrc     : pointer to a UINT32 var that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeUintVar
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT32           *puiSrc
)
{
    WE_INT32 iEncodeVarSize = 0;
    WE_INT32 iCount = 0;
    WE_INT32 iTmp = 0;
    
    if( (NULL == pstEncoder) || (NULL == puiSrc) )
    {
        return -1;
    }
    
    iEncodeVarSize = SecEcdr_GetEncodeUintVarSize(puiSrc);
    iTmp = pstEncoder->iPosition + iEncodeVarSize;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }

    for( iCount = iEncodeVarSize; iCount > 0; iCount-- )
    {
        pstEncoder->pucData[pstEncoder->iPosition] 
                        = (WE_UCHAR)((*puiSrc >> ((iCount - 1)*7)) & 0x7F);
        if( iCount < iEncodeVarSize )
        {
            pstEncoder->pucData[pstEncoder->iPosition - 1] |= 0x80;
        }
        pstEncoder->iPosition++ ;
    }
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeOctets
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    encode byte
ARGUMENTS PASSED:
    St_SecEcdrEncoder  *pstEncoder  : pointer to a encoder
    WE_CHAR            *pcSrc       : pointer to the byte that will be encoded
    WE_INT32           iSrcLength   : the length of the encodeing byte
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_EncodeOctets
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_CHAR             *pcSrc,
    WE_INT32             iSrcLength
)
{
    WE_INT32 iTmp = 0;
    if( NULL == pstEncoder || (iSrcLength > 0 && NULL == pcSrc) )
    {
        return -1;
    }
    iTmp = pstEncoder->iPosition + iSrcLength;
    
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }
    WE_MEMCPY( (void *)(pstEncoder->pucData + pstEncoder->iPosition),(void *)pcSrc, (WE_UINT32)iSrcLength );
    pstEncoder->iPosition += iSrcLength;
    return 0;
}
/*==================================================================================================
FUNCTION: 
    SecEcdr_EncodeString
CREATE DATE:
    2007-03-15
AUTHOR:
    Bird Zhang
DESCRIPTION:
    encode string
ARGUMENTS PASSED:
    St_SecEcdrEncoder  *pstEncoder  : pointer to a encoder
    WE_CHAR            *pcSrc       : pointer to the byte that will be encoded
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 SecEcdr_EncodeString (St_SecEcdrEncoder   *pstEncoder, WE_CHAR *pcSrc)
{
    WE_INT32 iLength = 0;
    WE_INT32 iTmp = 0;
    iLength = ((pcSrc != NULL) ? SEC_STRLEN (pcSrc) : 0);
    iTmp = pstEncoder->iPosition + iLength +1;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }
    WE_MEMCPY (pstEncoder->pucData+ pstEncoder->iPosition,  pcSrc, iLength);
    (pstEncoder->pucData+ pstEncoder->iPosition)[iLength] = '\0';
    pstEncoder->iPosition += iLength + 1;

    return 0;
}
/*==================================================================================================
FUNCTION: 
    SecEcdr_CreateDecoder
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Create a new Decoder structure
ARGUMENTS PASSED:
    WE_UCHAR    *pucData         : the decode data
    WE_UINT32    uiDataLength    : the length of decode data
RETURN VALUE:
    NULL : Error
    St_SecEcdrDecoder *pstDecoder : the new Decoder structure created
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
St_SecEcdrDecoder *SecEcdr_CreateDecoder
(
    WE_UCHAR    *pucData,
    WE_UINT32    uiDataLength
)
{
    St_SecEcdrDecoder   *pstDecoder = NULL;

    if( NULL == pucData || uiDataLength < 1 )
    {
        return NULL;
    }
    pstDecoder = (St_SecEcdrDecoder*)WE_MALLOC( (WE_ULONG)sizeof(St_SecEcdrDecoder) );
    if( NULL == pstDecoder )
    {
        return NULL;
    }
    pstDecoder->pucData = (WE_UCHAR*)WE_MALLOC((WE_ULONG)uiDataLength);
    if( NULL == pstDecoder->pucData )
    {
        WE_FREE(pstDecoder);
        return NULL;
    }
    WE_MEMCPY( (void *)(pstDecoder->pucData),(void *)pucData, uiDataLength );
    pstDecoder->iLength = (WE_INT32)uiDataLength;
    pstDecoder->iPosition = 0;
    return pstDecoder;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DestroyDecoder
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Destroy the decoder structure
ARGUMENTS PASSED:
    St_SecEcdrDecoder   *pstDecoder : pointer to the decoder structure to be destroyed
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DestroyDecoder
(
    St_SecEcdrDecoder   *pstDecoder
)
{
    if( NULL == pstDecoder )
    {
        return -1;
    }
    if( NULL != pstDecoder->pucData )
    {
        WE_FREE(pstDecoder->pucData);
    }
    WE_FREE(pstDecoder);
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_ChangeDecodePosition
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Change the current buffer position of the conversion object
ARGUMENTS PASSED:
    St_SecEcdrDecoder  *pstDecoder  : pointer to the Decoder to change the current buffer position
    WE_INT32            iOffset     : the interval to move
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_ChangeDecodePosition
(
    St_SecEcdrDecoder  *pstDecoder,
    WE_INT32            iOffset
)
{
    WE_INT32 iCurrentPos = 0;
    
    if( NULL == pstDecoder )
    {
        return -1;
    }
    iCurrentPos = pstDecoder->iPosition + iOffset;
    if( (iCurrentPos < 0) || (iCurrentPos > pstDecoder->iLength) )
    {
        return -1;
    }
    pstDecoder->iPosition = iCurrentPos;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DecodeUint8
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode 8 bit unsigned integer
ARGUMENTS PASSED:
    St_SecEcdrDecoder   *pstDecoder :   pointer to a decoder structure
    WE_UINT8            *pucDst     :   pointer to a UINT8 that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DecodeUint8
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT8            *pucDst
)
{
    WE_INT32 iTmp = 0;
    
    if( NULL == pstDecoder || NULL == pstDecoder->pucData || NULL == pucDst )
    {
        return -1;
    }
    
    iTmp = pstDecoder->iPosition + 1;
    if( iTmp > pstDecoder->iLength)
    {
        return -1;
    }
    *pucDst = pstDecoder->pucData[pstDecoder->iPosition];
    pstDecoder->iPosition++;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DecodeInt8
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode 8 bit integer
ARGUMENTS PASSED:
    St_SecEcdrDecoder  *pstDecoder :   pointer to a decoder structure
    WE_INT8            *pcDst      :   pointer to a INT8 that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DecodeInt8
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_INT8             *pcDst
)
{
    WE_INT32 iTmp = 0;
    
    if( NULL == pstDecoder || NULL == pstDecoder->pucData || NULL == pcDst )
    {
        return -1;
    }
    
    iTmp = pstDecoder->iPosition + 1;
    if( iTmp > pstDecoder->iLength)
    {
        return -1;
    }

    *pcDst = (WE_INT8)pstDecoder->pucData[pstDecoder->iPosition];
    pstDecoder->iPosition++;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DecodeUint16
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode 16 bit unsigned integer
ARGUMENTS PASSED:
    St_SecEcdrDecoder    *pstDecoder :   pointer to a decoder structure
    WE_UINT16            *pucDst     :   pointer to a UINT16 that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DecodeUint16
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT16           *pusDst
)
{
    WE_UINT8 ucTmp1 = '\0';
    WE_UINT8 ucTmp2 = '\0';
    WE_INT32 iTmp = 0;
    
    if( NULL == pstDecoder || NULL == pstDecoder->pucData || NULL == pusDst )
    {
        return -1;
    }
    
    iTmp = pstDecoder->iPosition + 2;
    if( iTmp > pstDecoder->iLength)
    {
        return -1;
    }

    ucTmp1 = pstDecoder->pucData[pstDecoder->iPosition];
    ucTmp2 = pstDecoder->pucData[pstDecoder->iPosition + 1];
    pstDecoder->iPosition += 2;

    *pusDst = (WE_UINT16)(((WE_UINT16)(ucTmp1 << 8)) | ((WE_UINT16)ucTmp2));
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DecodeInt16
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode 16 bit integer
ARGUMENTS PASSED:
    St_SecEcdrDecoder  *pstDecoder :   pointer to a decoder structure
    WE_INT16           *psDst      :   pointer to a INT16 that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DecodeInt16
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_INT16            *psDst
)
{
    WE_UINT8 ucTmp1 = '\0';
    WE_UINT8 ucTmp2 = '\0';
    WE_INT32 iTmp = 0;
    
    if( NULL == pstDecoder || NULL == pstDecoder->pucData || NULL == psDst )
    {
        return -1;
    }
    
    iTmp = pstDecoder->iPosition + 2;
    if( iTmp > pstDecoder->iLength)
    {
        return -1;
    }
    ucTmp1 = pstDecoder->pucData[pstDecoder->iPosition];
    ucTmp2 = pstDecoder->pucData[pstDecoder->iPosition + 1];
    pstDecoder->iPosition += 2;

    *psDst = (WE_INT16)(((WE_INT16)(ucTmp1 << 8)) | ((WE_INT16)ucTmp2));
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DecodeUint32
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode 32 unsigned bit integer
ARGUMENTS PASSED:
    St_SecEcdrDecoder   *pstDecoder :   pointer to a decoder structure
    WE_UINT32           *puiDst     :   pointer to a UINT32 that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DecodeUint32
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT32           *puiDst
)
{
    WE_UINT8 ucTmp1 = '\0';
    WE_UINT8 ucTmp2 = '\0';
    WE_UINT8 ucTmp3 = '\0';
    WE_UINT8 ucTmp4 = '\0';
    WE_INT32 iTmp = 0;
    
    if( NULL == pstDecoder || NULL == pstDecoder->pucData || NULL == puiDst )
    {
        return -1;
    }
    
    iTmp = pstDecoder->iPosition + 4;
    if( iTmp > pstDecoder->iLength)
    {
        return -1;
    }
    ucTmp1 = pstDecoder->pucData[pstDecoder->iPosition];
    ucTmp2 = pstDecoder->pucData[pstDecoder->iPosition + 1];
    ucTmp3 = pstDecoder->pucData[pstDecoder->iPosition + 2];
    ucTmp4 = pstDecoder->pucData[pstDecoder->iPosition + 3];
    pstDecoder->iPosition += 4;

    *puiDst = (WE_UINT32)( ((WE_UINT32)(ucTmp1 << 24)) | ((WE_UINT32)(ucTmp2 << 16)) | \
                           ((WE_UINT32)(ucTmp3 << 8 )) | ((WE_UINT32)ucTmp4 ) );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DecodeInt32
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode 32 bit integer
ARGUMENTS PASSED:
    St_SecEcdrDecoder  *pstDecoder :   pointer to a decoder structure
    WE_INT32           *piDst      :   pointer to a INT32 that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DecodeInt32
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_INT32            *piDst
)
{
    WE_UINT8 ucTmp1 = '\0';
    WE_UINT8 ucTmp2 = '\0';
    WE_UINT8 ucTmp3 = '\0';
    WE_UINT8 ucTmp4 = '\0';
    WE_INT32 iTmp = 0;
    
    if( NULL == pstDecoder || NULL == pstDecoder->pucData || NULL == piDst )
    {
        return -1;
    }
    
    iTmp = pstDecoder->iPosition + 4;
    if( iTmp > pstDecoder->iLength)
    {
        return -1;
    }
    
    ucTmp1 = pstDecoder->pucData[pstDecoder->iPosition];
    ucTmp2 = pstDecoder->pucData[pstDecoder->iPosition + 1];
    ucTmp3 = pstDecoder->pucData[pstDecoder->iPosition + 2];
    ucTmp4 = pstDecoder->pucData[pstDecoder->iPosition + 3];
    pstDecoder->iPosition += 4;

    *piDst = (WE_INT32)(((WE_INT32)(ucTmp1 << 24)) | ((WE_INT32)(ucTmp2 << 16)) | \
                        ((WE_INT32)(ucTmp3 << 8 )) | ((WE_INT32)ucTmp4) );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_DecodeUintVar
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode a unsigned int var
ARGUMENTS PASSED:
    St_SecEcdrDecoder  *pstDecoder : pointer to a decoder
    WE_UINT32          *puiDst     : pointer to a UINT32 var that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_DecodeUintVar
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT32           *puiDst
)
{
    WE_UINT8 iCount = 0;
    WE_UCHAR ucTmp = '\0';
    
    if( NULL == pstDecoder || NULL == pstDecoder->pucData || NULL == puiDst )
    {
        return -1;
    }

    *puiDst = 0;
    for( iCount = 0; iCount < 5; iCount++ )
    {
        if( pstDecoder->iPosition + 1 > pstDecoder->iLength )
        {
            *puiDst = 0;
            return -1;
        }
        ucTmp = pstDecoder->pucData[pstDecoder->iPosition++];

        if( 0 != *puiDst )
        {
            *puiDst <<= 7;
        }
        *puiDst |= (WE_UINT32)(ucTmp & 0x7f);
        if( ucTmp < 0x80 )   /* The last 7bits */
        {
            return 0;
        }
        if( 4 == iCount )        /* The decoder exceed 5 byte, the data is error */
        {
            *puiDst = 0;
            return -1;
        }
    }
    
    return 0;
}

/*==================================================================================================
FUNCTION: 
    
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    decode
ARGUMENTS PASSED:
    St_SecEcdrDecoder  *pstDecoder : pointer to a Decoder
    WE_CHAR            *pcDst      : pointer to the byte that store the decoded data
    WE_INT32           iDstLength  : the length of the Decodeing byte
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    pcDst is Malloced by Invoker
==================================================================================================*/
WE_INT32 SecEcdr_DecodeOctets
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_CHAR             *pcDst,
    WE_INT32            iDstLength
)
{
    WE_INT32 iTmp;
    if( NULL == pstDecoder || NULL == pcDst || iDstLength < 0 )
    {
        return -1;
    }
    iTmp = pstDecoder->iPosition + iDstLength;
    if (iTmp > pstDecoder->iLength )
    {
    return -1;
    }

    WE_MEMCPY( pcDst, (pstDecoder->pucData) + (pstDecoder->iPosition), (WE_UINT32)iDstLength );
    pstDecoder->iPosition += iDstLength;
    
    return 0;
}
/*==================================================================================================
FUNCTION: 
   SecEcdr_DecodeString 
CREATE DATE:
    2006-05-18
AUTHOR:
    Bird zhang
DESCRIPTION:
    encode
ARGUMENTS PASSED:
    St_SecEcdrDecoder  *pstDecoder : pointer to a Decoder
    WE_CHAR            *pcDst      : pointer to the byte that store the decoded data
RETURN VALUE:
    -1  :   Error
    0   :   Success
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    pcDst is Malloced by Invoker
==================================================================================================*/

WE_INT32
SecEcdr_DecodeString (St_SecEcdrDecoder   *pstEncoder, WE_CHAR **ppcDst)
{
    WE_CHAR *pcBuf = NULL;
    WE_INT32        iLength = 0;;
    WE_INT32        iIndex = 0;
    iLength = pstEncoder->iLength - pstEncoder->iPosition;
    pcBuf = (WE_CHAR*)(pstEncoder->pucData + pstEncoder->iPosition);
    
    for (iIndex = 0; iIndex < iLength; iIndex++) 
    {
      if (pcBuf[iIndex] == '\0')
      {
        break;
      }
    }
    if (iIndex == iLength) 
    {
      *ppcDst = NULL;
      return -1;
    }
    iLength = iIndex;
    if (iLength == 0) 
    {
      *ppcDst = NULL;
    }
    else 
    {
      *ppcDst = WE_MALLOC (iLength + 1);
      WE_MEMCPY ((void *)*ppcDst, pstEncoder->pucData+ pstEncoder->iPosition, iLength);
      ((WE_CHAR *)*ppcDst)[iLength] = '\0';
    }
    pstEncoder->iPosition+= iLength + 1;

    return 0;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeUint8Size
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    calculate the buffer size required to do an encoding UINT8 operation
ARGUMENTS PASSED:
    void
RETURN VALUE:
    1
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeUint8Size( void )
{
    return 1;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeInt8Size
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    calculate the buffer size required to do an encoding INT8 operation
ARGUMENTS PASSED:
    void 
RETURN VALUE:
    1
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeInt8Size( void )
{
    return 1;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeUint16Size
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    calculate the buffer size required to do an encoding UINT16 operation
ARGUMENTS PASSED:
    void 
RETURN VALUE:
    2
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeUint16Size( void )
{
    return 2;
}
/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeInt16Size
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    calculate the buffer size required to do an encoding INT16 operation
ARGUMENTS PASSED:
    void 
RETURN VALUE:
    2
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeInt16Size( void )
{
    return 2;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeUint32Size
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    calculate the buffer size required to do an encoding UINT32 operation
ARGUMENTS PASSED:
    void 
RETURN VALUE:
    4
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeUint32Size( void )
{
    return 4;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeInt32Size
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    calculate the buffer size required to do an encoding INT32 operation
ARGUMENTS PASSED:
    void
RETURN VALUE:
    4
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeInt32Size( void )
{
    return 4;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeUintVarSize
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Get uint var type length after encode
ARGUMENTS PASSED:
    WE_INT32 *puiSrc    :   pointer to a Uint var have encoded
RETURN VALUE:
    -1          :    Error
    WE_INT32    :    Return the number of bytes required to store n as a Uintvar.
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeUintVarSize
(
    WE_UINT32 *puiSrc
)
{
    if( NULL == puiSrc )
    {
        return -1;
    }
    
    if( *puiSrc < (WE_UINT32)0x80 )              /* 1 0000000 */
    {
        return 1;
    }
    else if( *puiSrc < (WE_UINT32)0x4000 )      /* 1 0000000 0000000 */
    {
        return 2;
    }
    else if( *puiSrc < (WE_UINT32)0x200000 )     /* 1 0000000 0000000 0000000 */
    {
        return 3;
    }
    else if( *puiSrc < (WE_UINT32)0x10000000 )   /* 1 0000000 0000000 0000000 0000000 */
    {
        return 4;
    }
    else
    {
        return 5;
    }
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetEncodeOctetsSize
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    calculate the buffer size required to do an encoding octet operation
ARGUMENTS PASSED:
    WE_INT32    iSrcLength
RETURN VALUE:
    -1                  :   Error
    WE_INT32 iSrcLength :   the length of octet
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetEncodeOctetsSize
(
    WE_INT32    iSrcLength
)
{
    if( iSrcLength < 0 )
    {
        return -1;
    }
    
    return iSrcLength;
}

/*==================================================================================================
FUNCTION: 
    SecEcdr_GetRemainLength
CREATE DATE:
    2006-05-18
AUTHOR:
    ChenZhifeng
DESCRIPTION:
    Return the number of bytes remaining in the Encoder data
ARGUMENTS PASSED:

RETURN VALUE:
    -1      :   Error
    WE_INT32 iRemLenth   :   the number of bytes remaining in the Encoder data
USED GLOBAL VARIABLES:   
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecEcdr_GetRemainLength
(
    St_SecEcdrEncoder *pstEncoder
)
{
    WE_INT32 iRemLength = 0;
    if( NULL == pstEncoder || pstEncoder->iLength < pstEncoder->iPosition )
    {
        return -1;
    }
    iRemLength = pstEncoder->iLength - pstEncoder->iPosition;
    return iRemLength;
}


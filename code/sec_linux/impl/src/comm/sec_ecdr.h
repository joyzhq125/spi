/*=====================================================================================
    HEADER NAME : 
        wap_ecdr.h
    MODULE NAME : 
        WAP_ECDR
    PRE-INCLUDE FILES DESCRIPTION
        we_def.h
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c) 
    Copyright 2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    Modification Tracking
    Date       Author       Number    Description of changes
    ---------- ------------ --------- --------------------------------------------
    2006-05-18 ChenZhiFeng  none      Init
    2006-08-07 MaZhiqiang   None      Optimization
=====================================================================================*/

/**************************************************************************************
*   Multi-Include-Prevent Section
**************************************************************************************/
#ifndef WAP_ECDR_H
#define WAP_ECDR_H

/**************************************************************************************
*   include File Section
**************************************************************************************/
#include "we_def.h"

/**************************************************************************************
*   Marco Define Section
**************************************************************************************/

/**************************************************************************************
*   Type Define Section
**************************************************************************************/
/* Encoder, used for encode. */
typedef struct tagSt_SecEcdrEncoder
{
    WE_INT32        iLength;    /* The length of encoded data */
    WE_INT32        iPosition;  /* The current position */
    WE_UCHAR       *pucData;    /* The encoded data */
} St_SecEcdrEncoder;

/* Decoder, used for decode */
typedef struct tagSt_SecEcdrDecoder
{
    WE_INT32        iLength;    /* The length of decoded data */
    WE_INT32        iPosition;  /* The current position */
    WE_UCHAR       *pucData;    /* The decoded data */
} St_SecEcdrDecoder;

/***************************************************************************************
*   Prototype Declare Section
***************************************************************************************/
St_SecEcdrEncoder *SecEcdr_CreateEncoder
(
    WE_INT32     iLength
);

/* Destroy the encoder structure */
WE_INT32 SecEcdr_DestroyEncoder
(
    St_SecEcdrEncoder *pstEncoder
);

/* Change the current buffer position of the conversion object */
WE_INT32 SecEcdr_ChangeEncodePosition
(
    St_SecEcdrEncoder  *pstEncoder,
    WE_INT32            iOffset
);

/* encode fuctions */
WE_INT32 SecEcdr_EncodeUint8
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT8            *pucSrc
);

WE_INT32 SecEcdr_EncodeInt8
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_INT8             *pcSrc
);

WE_INT32 SecEcdr_EncodeUint16
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT16           *pusSrc
);

WE_INT32 SecEcdr_EncodeInt16
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_INT16            *psSrc
);

WE_INT32 SecEcdr_EncodeUint32
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT32           *puiSrc
);

WE_INT32 SecEcdr_EncodeInt32
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_INT32            *piSrc
);

WE_INT32 SecEcdr_EncodeUintVar
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_UINT32            *puiSrc
);

WE_INT32 SecEcdr_EncodeOctets
(
    St_SecEcdrEncoder   *pstEncoder,
    WE_CHAR             *pcSrc,
    WE_INT32             iSrcLength
);

/* Create a new Decoder structure */
St_SecEcdrDecoder *SecEcdr_CreateDecoder
(
    WE_UCHAR    *pucData,
    WE_UINT32    uiDataLength
);

WE_INT32 SecEcdr_DestroyDecoder
(
    St_SecEcdrDecoder   *pstDecoder
);

/* Change the current buffer position of the conversion object */
WE_INT32 SecEcdr_ChangeDecodePosition
(
    St_SecEcdrDecoder  *pstDecoder,
    WE_INT32            iOffset
);


/* decode fuctions */
WE_INT32 SecEcdr_DecodeUint8
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT8            *pucDst
);

WE_INT32 SecEcdr_DecodeInt8
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_INT8             *pcDst
);

WE_INT32 SecEcdr_DecodeUint16
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT16           *pusDst
);

WE_INT32 SecEcdr_DecodeInt16
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_INT16            *psDst
);

WE_INT32 SecEcdr_DecodeUint32
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT32           *puiDst
);

WE_INT32 SecEcdr_DecodeInt32
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_INT32            *piDst
);

WE_INT32 SecEcdr_DecodeUintVar
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_UINT32            *puiDst
);

/* pcDst is Malloced by Invoker */
WE_INT32 SecEcdr_DecodeOctets
(
    St_SecEcdrDecoder   *pstDecoder,
    WE_CHAR             *pcDst,
    WE_INT32            iDstLength
);

/* Get encode size */
WE_INT32 SecEcdr_GetEncodeUint8Size( void );

WE_INT32 SecEcdr_GetEncodeInt8Size( void );

WE_INT32 SecEcdr_GetEncodeUint16Size( void );

WE_INT32 SecEcdr_GetEncodeInt16Size( void );

WE_INT32 SecEcdr_GetEncodeUint32Size( void );

WE_INT32 SecEcdr_GetEncodeInt32Size( void );

/* Get uint var type length after encode */
WE_INT32 SecEcdr_GetEncodeUintVarSize
(
    WE_UINT32 *puiSrc
);

WE_INT32 SecEcdr_GetEncodeOctetsSize
(
    WE_INT32    iSrcLength
);

/* Return the number of bytes remaining in the Encoder data */
WE_INT32 SecEcdr_GetRemainLength
(
    St_SecEcdrEncoder *pstEncoder
);

WE_INT32
SecEcdr_EncodeString (St_SecEcdrEncoder   *pstEncoder, WE_CHAR *pcSrc);

WE_INT32
SecEcdr_DecodeString (St_SecEcdrDecoder   *pstEncoder, WE_CHAR **ppcDst);

#endif  /* end WAP_ECDR_H */


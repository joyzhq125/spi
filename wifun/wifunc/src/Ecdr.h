#ifndef ECDR_H
#define ECDR_H


/* Encoder, used for encode. */
typedef struct tagSt_EcdrEncoder
{
    int        iLength;    /* The length of encoded data */
    int        iPosition;  /* The current position */
    char       *pucData;    /* The encoded data */
} St_EcdrEncoder;

/* Decoder, used for decode */
typedef struct tagSt_EcdrDecoder
{
    int        iLength;    /* The length of decoded data */
    int        iPosition;  /* The current position */
    char       *pucData;    /* The decoded data */
} St_EcdrDecoder;

St_EcdrEncoder *Ecdr_CreateEncoder
(
    int     iLength
);

int Ecdr_DestroyEncoder
(
    St_EcdrEncoder *pstEncoder
);


St_EcdrDecoder *Ecdr_CreateDecoder
(
    char    *pucData,
    int    uiDataLength
);


int Ecdr_DestroyDecoder
(
    St_EcdrDecoder   *pstDecoder
);


int Ecdr_EncodeOneByte
(
    St_EcdrEncoder   *pstEncoder,
    char            *pucSrc
);


int Ecdr_Encode2Bytes
(
    St_EcdrEncoder   *pstEncoder,
    short           *pusSrc
);


int Ecdr_Encode4Bytes
(
    St_EcdrEncoder   *pstEncoder,
    int           *puiSrc
);


int Ecdr_DecodeOneByte
(
    St_EcdrDecoder   *pstDecoder,
    char            *pucDst
);


int Ecdr_Decode2Bytes
(
    St_EcdrDecoder   *pstDecoder,
    short           *pusDst
);


int Ecdr_Decode4Bytes
(
    St_EcdrDecoder   *pstDecoder,
    int           *puiDst
);


#endif //ECDR_H


/*encode decode*/

#include "Wifunc.h"


St_EcdrEncoder *Ecdr_CreateEncoder
(
    int     iLength
)
{
    St_EcdrEncoder   *pstEncoder = 
        (St_EcdrEncoder*)malloc ( sizeof(St_EcdrEncoder) );

    if( (NULL == pstEncoder) || (iLength < 1) )
    {
        if (pstEncoder != NULL)
        {
            free(pstEncoder);
        }
        return NULL;
    }

    pstEncoder->pucData = (char*)malloc( iLength );
    if( NULL == pstEncoder->pucData )
    {
        free(pstEncoder);
        return NULL;
    }
    memset( (void *)(pstEncoder->pucData), 0, iLength );
    pstEncoder->iLength = iLength;
    pstEncoder->iPosition = 0;
    
    return pstEncoder;
}



int Ecdr_DestroyEncoder
(
    St_EcdrEncoder *pstEncoder
)
{
    if( NULL == pstEncoder )
    {
        return -1;
    }
    
    if( NULL != pstEncoder->pucData )
    {
        free( pstEncoder->pucData );
    }
    free( pstEncoder );
    return 0;
}


//decode create
St_EcdrDecoder *Ecdr_CreateDecoder
(
    char    *pucData,
    int    uiDataLength
)
{
    St_EcdrDecoder   *pstDecoder = NULL;

    if( NULL == pucData || uiDataLength < 1 )
    {
        return NULL;
    }
    pstDecoder = (St_EcdrDecoder*)malloc( sizeof(St_EcdrDecoder) );
    if( NULL == pstDecoder )
    {
        return NULL;
    }
    pstDecoder->pucData = (char*)malloc(uiDataLength);
    if( NULL == pstDecoder->pucData )
    {
        free(pstDecoder);
        return NULL;
    }
    memcpy( (void *)(pstDecoder->pucData),(void *)pucData, uiDataLength );
    pstDecoder->iLength = (int)uiDataLength;
    pstDecoder->iPosition = 0;
    return pstDecoder;
}


int Ecdr_DestroyDecoder
(
    St_EcdrDecoder   *pstDecoder
)
{
    if( NULL == pstDecoder )
    {
        return -1;
    }
    if( NULL != pstDecoder->pucData )
    {
        free(pstDecoder->pucData);
    }
    free(pstDecoder);
    return 0;
}

//encode 8bits
int Ecdr_EncodeOneByte
(
    St_EcdrEncoder   *pstEncoder,
    char            *pucSrc
)
{
    int iTmp = 0;
    
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

//encode 16bits
int Ecdr_Encode2Bytes
(
    St_EcdrEncoder   *pstEncoder,
    short           *pusSrc
)
{
    int iTmp = 0;
    
    if( (NULL == pstEncoder) || (NULL == pusSrc) )
    {
        return -1;
    }
    
    iTmp = pstEncoder->iPosition + 2;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }
        
    pstEncoder->pucData[pstEncoder->iPosition] = (char)((*pusSrc>>8) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 1] = (char)(*pusSrc & 0xFF);
    pstEncoder->iPosition += 2;
    return 0;
}


//encode 32bits
int Ecdr_Encode4Bytes
(
    St_EcdrEncoder   *pstEncoder,
    int           *puiSrc
)
{
    int iTmp = 0;
    if( (NULL == pstEncoder) || (NULL == puiSrc) )
    {
        return -1;
    }

    iTmp = pstEncoder->iPosition + 4;
    if( iTmp > pstEncoder->iLength )
    {
        return -1;
    }

    pstEncoder->pucData[pstEncoder->iPosition]   = (char)((*puiSrc>>24) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 1] = (char)((*puiSrc>>16) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 2] = (char)((*puiSrc>>8 ) & 0xFF);
    pstEncoder->pucData[pstEncoder->iPosition + 3] = (char)(*puiSrc & 0xFF);
    pstEncoder->iPosition += 4;
    return 0;
}



//decode 8 bits
int Ecdr_DecodeOneByte
(
    St_EcdrDecoder   *pstDecoder,
    char            *pucDst
)
{
    int iTmp = 0;
    
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

//decode 16bits
int Ecdr_Decode2Bytes
(
    St_EcdrDecoder   *pstDecoder,
    short           *pusDst
)
{
    char ucTmp1 = '\0';
    char ucTmp2 = '\0';
    char iTmp = 0;
    
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

    *pusDst = (short)(((short)(ucTmp1 << 8)) | ((short)ucTmp2));
    return 0;
}

//decode 32bits
int Ecdr_Decode4Bytes
(
    St_EcdrDecoder   *pstDecoder,
    int           *puiDst
)
{
    char ucTmp1 = '\0';
    char ucTmp2 = '\0';
    char ucTmp3 = '\0';
    char ucTmp4 = '\0';
    char iTmp = 0;
    
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

    *puiDst = (int)( ((int)(ucTmp1 << 24)) | ((int)(ucTmp2 << 16)) | \
                           ((int)(ucTmp3 << 8 )) | ((int)ucTmp4 ) );
    return 0;
}


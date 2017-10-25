/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */








#ifndef MFIELDP_H
#define MFIELDP_H 







unsigned char *parseContentType( WE_UINT8 modId, MmsContentType **cType,
     const unsigned char *contentTypeData, WE_UINT32 length);
WE_BOOL parseFromAddress( WE_UINT8 modId, unsigned char *headerData, 
    const WE_UINT32 headerSize, MmsAddress *from);
unsigned char *parseEntryHeader( WE_UINT8 modId, unsigned char *headerStart, 
    WE_UINT32 bufSize, MmsBodyInfoList *bodyInfoList);
MmsAddressList* parseAddress( WE_UINT8 modId, MmsHeaderTag tag, 
    unsigned char *headerData, WE_UINT32 headerDataSize);
WE_BOOL parseHeaderTime( unsigned char *headerData, 
    WE_UINT32 headerDataSize, MmsGetHeader *mHeader,MmsHeaderTag tag);
WE_BOOL parseEncodedStringValue( WE_UINT8 modId, unsigned char *buf, 
    WE_UINT32 size, WE_UINT32 *lengthOfString, MmsEncodedText *eText);
MmsStatus checkStatus( WE_UINT8 status);
unsigned char *parseOneAddress( WE_UINT8 modId, unsigned char *buffer, 
    WE_UINT32 size, MmsAddress *address);
MmsKnownMediaType mmsConvertToMmsKnownMediaType( const WE_UINT32 mediaValue);


#endif 



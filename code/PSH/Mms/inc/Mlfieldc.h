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





#ifndef MFIELDC_H
#define MFIELDC_H






unsigned char *createContentType(WE_UINT8 modId, WE_UINT32 *size,
    const MmsContentType *contentInfo);
unsigned char *createMsgClass( WE_UINT8 modId, unsigned long *size,
    MmsMessageClass msgClass);
WE_BOOL createFromField( WE_UINT8 modId, const MmsAddress *from, 
    WE_UINT32 fromType, unsigned char **buf, unsigned long *bufLength);
WE_BOOL createDestinationField( WE_UINT8 modId, MmsAddressList *addrList, 
    unsigned char *pduHeader, unsigned long bufSize, unsigned long *actLength, 
    MmsHeaderValue *valueOfTag, MmsHeaderTag headerTag);
unsigned char *createTimeField( WE_UINT8 modId, unsigned long *size, WE_UINT32 theTime, MmsTimeType timeType);
WE_BOOL createEncodedStringValue( WE_UINT8 modId, const char *text, MmsCharset charset,
    unsigned long *size, unsigned char **retPtr);
void calculateSize( const MmsContentType *contentInfo, WE_UINT32 *size);
WE_UINT32 calculateEntryHeadersSize( MmsEntryHeader *entryHeader);
unsigned char *createEntryHeaders( MmsEntryHeader *entryHeader, 
    unsigned char *buffer);
unsigned long getSizeOfAddress( MmsAddressList *addrList);
unsigned long getSizeOfEncodedText(MmsEncodedText eText);
char *generateMmsTransactionId( WE_UINT8 modId, unsigned long *length);

#endif 

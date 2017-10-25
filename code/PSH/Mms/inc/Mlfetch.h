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





#ifndef MFETCH_H
#define MFETCH_H



typedef enum
{
    
    ENCODE_VALUE_GZIP       = 128,
    
    ENCODE_VALUE_COMPRESS   = 129,
    
    ENCODE_VALUE_DEFLATE    = 130,
    
    ENCODE_VALUE_BASE64     = 66, 
    
    ENCODE_VALUE_BINARY     = 0 
} MmsContentEncodingValue;


#define MMS_MESSAGE_INFO_SIZE  (POS_INT_MSG_NUMBER * sizeof(WE_UINT32))
#define MMS_PART_INFO_SIZE (POS_INT_PART_NUMBER * sizeof(WE_UINT32))










WE_UINT32 getNumberOfParts( const MmsMessage *msg);
WE_UINT32 getNumberOfSubParts( WE_UINT32 number, const MmsMessage *msg);
WE_UINT32 getMaxPartSize( const MmsMessage *msg);
WE_UINT32 getMaxEntrySize( const MmsMessage *msg);
WE_UINT32 getHeaderSize( const MmsMessage *msg);

WE_UINT32 getEntrySize( WE_UINT32 number, const MmsMessage *msg);
WE_UINT32 getEntryStart( WE_UINT32 number, const MmsMessage *msg);
WE_UINT32 getDataSize( WE_UINT32 number, const MmsMessage *msg); 
WE_UINT32 getDataStart( WE_UINT32 number, const MmsMessage *msg); 
WE_UINT32 getDrmStatus( WE_UINT32 number, const MmsMessage *mHandle);
MmsContentEncodingValue getEncodingType( WE_UINT32 number, 
    const MmsMessage *mHandle);

void setHeaderSize( unsigned char *buffer, WE_UINT32 value);
void setNumberOfParts( unsigned char *buffer, WE_UINT32 value);
void setNumberOfSubParts( unsigned char *buffer, WE_UINT32 number, 
    WE_UINT32 value);
void setDrmStatus( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value);
void setEncodingType( unsigned char *buffer, WE_UINT32 number, 
    MmsContentEncodingValue value);
void setHeaderStart( unsigned char *buffer);
void setEntrySize( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value);
void setEntryStart( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value);
void setDataSize( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value);
void setDataStart( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value);
MmsResult readDataFromFile( WE_UINT8 modId, unsigned char **buf, 
    WE_UINT32 *size, int fileHandle, WE_INT32 *filePos);
MmsResult getMessageInfo( WE_UINT8 modId,
              MmsGetHeader *header, MmsBodyInfoList *bodyInfoList,
              const MmsMessage *msg, MmsTransaction *tHandle);
MmsMessageInfo *parseMsgInfo( WE_UINT8 modId, unsigned char *buffer, 
    WE_UINT32 bufferSize, WE_UINT32 *msgPtr);
MmsResult loadMessageInfoFile( WE_UINT8 modId, MmsMessage *msg, 
    MmsTransaction *tHandle);
MmsResult getMmsHeader( WE_UINT8 modId, MmsGetHeader *header, 
    const MmsMessage *msg, MmsTransaction *tHandle);
MmsResult parseBodyHeader( WE_UINT8 modId, WE_BOOL prevIsMultipart, 
    MmsBodyInfoList *msgInfo, unsigned char *data, WE_UINT32 size);

#endif 


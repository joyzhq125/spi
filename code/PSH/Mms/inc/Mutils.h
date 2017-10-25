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





#ifndef MUTILS_H
#define MUTILS_H
















WE_BOOL copyMmsEncText( WE_UINT8 modId, MmsEncodedText *to, 
    const MmsEncodedText *from);
WE_BOOL  copyMmsAddress( WE_UINT8 modId, MmsAddress *to, const MmsAddress *from);
WE_BOOL  copyMmsAddressList( WE_UINT8 modId, MmsAddressList **toList, const MmsAddressList *fromList);
WE_BOOL  copyMmsContentType( WE_UINT8 modId, MmsContentType *toContent, const MmsContentType *fromContent);
WE_BOOL copyMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *toDrmInfo, const MmsDrmInfo *fromDrmInfo);
WE_BOOL  copyMmsEntryHeader( WE_UINT8 modId, MmsEntryHeader **to, const MmsEntryHeader *from);
MmsForward *copyMmsForward( WE_UINT8 modId, const MmsForward *mmsForward);
MmsBodyInfoList *copyMmsBodyInfoList( WE_UINT8 modId, MmsBodyInfoList *list);



void freeMmsAddress( WE_UINT8 modId, MmsAddress *address);
void freeMmsAddressList( WE_UINT8 modId, MmsAddressList *addressList);
void freeMmsBodyInfoList( WE_UINT8 modId, MmsBodyInfoList *bodyInfoList);
void freeMmsEncodedText( WE_UINT8 modId, MmsEncodedText *encodedText);
void freeMmsMessageClass( WE_UINT8 modId, MmsMessageClass *messageClass);
void freeMmsContentTypeParams( WE_UINT8 modId, MmsAllParams *params);
void freeMmsContentType( WE_UINT8 modId, MmsContentType *contentType);
void freeMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *drmInfo);
void freeMmsSetHeader( WE_UINT8 modId, MmsSetHeader *mHeader);
void freeMmsGetHeader( WE_UINT8 modId, MmsGetHeader *mHeader);
void freeMmsNotification( WE_UINT8 modId, MmsNotification *mmsNotification);
void freeMmsEntryHeader( WE_UINT8 modId, MmsEntryHeader *entryHeader);
void freeMmsForward( WE_UINT8 modId, MmsForward *mmsForward);
void freeMmsMessageInfo( WE_UINT8 modId, MmsMessageInfo *messageInfo);
void freeMmsReadOrigInd( WE_UINT8 modId, MmsReadOrigInd *mmsOrigInd);
void freeMmsPrevSentBy( WE_UINT8 modId, MmsPrevSentBy *mmsPrevSentBy);
void freeMmsPrevSentDate( WE_UINT8 modId, MmsPrevSentDate *mmsPrevSentDate);
void freeMmsBodyParams( WE_UINT8 modId, MmsBodyParams *bodyParams);
void freeMmsDeliveryInd( WE_UINT8 modId, MmsDeliveryInd *deliveryInd);

unsigned char *mmsCvtBuf( WE_UINT8 modId, int *charset, const unsigned char *srcData, 
    int *srcSize, int *dstSize);
unsigned long mmsStrnlen(const char *str, unsigned long length);
char *mmsStrtok(char *string, const char *control);


WE_BOOL  mmsValidateAddressList(  const MmsAddressList *addrList);
WE_BOOL  mmsValidateSubject(  const MmsEncodedText *subject);
WE_BOOL  mmsValidateMessageClass(  const MmsMessageClass *msgClass);
WE_BOOL  mmsValidateAddress(  const MmsAddress *fromAddress);
WE_BOOL  mmsValidateEntryHeaders(  WE_UINT8 modId, MmsEntryHeader *entryHeader, MmsResult *result);
WE_BOOL  mmsValidateCharSet( int charSet);


WE_BOOL  mmsIsMultiPart( MmsKnownMediaType value);
WE_BOOL  mmsIsText( MmsKnownMediaType value);


void mmsConvertDateToString( WE_UINT32 t, char *s);
WE_UINT32 mmsGetGMTtime(WE_BOOL ensureGMT);

#endif 


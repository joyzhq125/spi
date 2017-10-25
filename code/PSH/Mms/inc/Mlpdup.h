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









#ifndef MLPDUP_H
#define MLPDUP_H
 






MmsResult parseMmsNotification( WE_UINT8 modId, unsigned char *headerData, 
    WE_UINT32 headerDataSize, MmsNotification *mmsNotification, MmsVersion *version);
WE_BOOL parseDeliveryReport( WE_UINT8 modId, unsigned char *headerData, 
    WE_UINT32 headerDataSize, MmsDeliveryInd *mmsDelivery);
void parseForwardConf( unsigned char *headerData, WE_UINT32 headerDataSize, 
    MmsForwardConf *mmsForwardConf);    
MmsReadOrigInd *parseReadReport( WE_UINT8 modId, unsigned char *pdu, WE_UINT32 len);
MmsResult parseMmsHeader(  WE_UINT8 modId, unsigned char *headerData, 
    WE_UINT32 headerDataSize, MmsGetHeader *mHeader);

#endif 

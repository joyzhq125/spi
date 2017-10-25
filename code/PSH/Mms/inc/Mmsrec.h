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





#ifndef MMSREC_H
#define MMSREC_H





















void recContentRouting(const we_content_data_t* pContentData);


















void recCreateMessage(WE_UINT8 orderer, MmsFileType fileType);













void recCreateMessageDone(WE_UINT8 orderer, MmsMsgId msgId, MmsFolderType folder);











void recDeleteMessage(WE_UINT8 orderer, MmsMsgId msgId);

















void recForwardReq(WE_UINT8 source, const unsigned char *pdu, WE_UINT32 length);











void recGetNumberOfMessages(WE_UINT8 orderer, MmsFolderType folder);












void recGetMessageIdList(WE_UINT8 orderer, MmsFolderType folder, WE_UINT32 startPos, 
    WE_UINT32 maxIds);










void recGetMessageInfo(WE_UINT8 source, WE_UINT32 msgId);














void recMoveMessage(WE_UINT8 orderer, MmsMsgId msgId, MmsFolderType toFolder);












void recPush( WE_BOOL isSmsBearer, unsigned long size, void *pdu);








void recRegResponse(we_registry_response_t* p);












void recRetrieveCancel(MmsMsgId notifId);












void recRetrieveMessage(WE_UINT8 orderer, MmsMsgId notifId);














void recSendCancel(MmsMsgId msgId);











void recSendMessage(WE_UINT8 source, MmsMsgId msgId);















void recSendReadReport(WE_UINT8 source, MmsMsgId msgId, MmsReadStatus readStatus);











void recTerminate(void);










void recSetReadMark(MmsMsgId msgId, WE_UINT8 value); 









void recGetReport(MmsMsgId msgId);









void getFolderStatus(WE_UINT8 source, WE_UINT32 folderMask);

#endif  

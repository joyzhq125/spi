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











#ifndef MMSRPL_H
#define MMSRPL_H






          



























void replyCreateMessageResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId msgId);













void replyDeleteMessageResponse(WE_UINT8 reciever, MmsResult result);














void replyGetNumberOfMessagesResponse(WE_UINT8 reciever, MmsResult result, 
                                      WE_UINT32 number, MmsFolderType folder);













void replyMoveMessageResponse(WE_UINT8 reciever, MmsResult result);


















void replyNotificationReceived(MmsNotificationType type, MmsMsgId notifId,
                               MmsAddress *from, MmsEncodedText *subject, WE_UINT32 size,
                               MmsNotificationReason reason);













void replyReadReportIndication(MmsReadOrigInd *mmsOrigInd, WE_UINT32 msgId,
                               WE_UINT32 msgIdFile);













void notifyReadReportIndication(MmsReadOrigInd *mmsOrigInd, WE_UINT32 msgId,
                               WE_UINT32 msgIdFile);













void replyRetrieveMessageResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId msgId);













void replySendMessageResponse(WE_UINT8 reciever, MmsResult result);













void replySendReadReportResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId rrId, MmsMsgId msgId);




















void replyDeliveryReportReceived(char *serverMsgId, MmsAddress *to, 
                                MmsTimeSec date, MmsStatus status,
                                WE_UINT32 msgId, WE_UINT32 msgIdFile);






















void notifyDeliveryReportReceived(char *serverMsgId, MmsAddress *to, 
                                  MmsTimeSec date, MmsStatus status, WE_UINT32 msgId, WE_UINT32 msgIdFile);














void mmsNotifyError(MmsResult result);













void replyForwardReqResponse(WE_UINT8 reciever, MmsResult result);





void replyFolderContent(WE_UINT8 reciever, MmsListOfProperties *properties);





void replyMsgInfo(WE_UINT8 modId, MmsListOfProperties *ptr);






void replyProgressStatus(WE_UINT8 operation, WE_UINT8 state,
    MmsMsgId msgId,
    WE_UINT32 progress, WE_UINT32 totalLength,
    WE_UINT32 *lastTime);












void replyCreateMessageDoneResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId msgId);

#endif  


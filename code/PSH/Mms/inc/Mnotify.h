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





#ifndef MNOTIFY_H
#define MNOTIFY_H





typedef enum
{
    






    MMS_SIG_MSR_NOTIFICATION,

    MMS_SIG_MSR_NOTIFY_FILE_CREATED,
        
    


    MMS_SIG_MSR_NOTIFY_ASYNC_WRITE_FINISHED,

    

    MMS_SIG_MSR_NOTIFY_MMT_RSP
} MmsNotifySignalId;



typedef struct
{
    WE_BOOL isSmsBearer;
    MmsStatus msgStatus;
    MmsDeliveryReportAllowed deliveryReportFlag;
    char *transactionId;
    MmsVersion version;
} MmsNotifIndInfo;









void msrNotifyEmergencyAbort(void);
void msrNotifyTerminate(void);
void msrNotifyInit(void);
void sendNotifyIndRsp( WE_BOOL isSmsBearer, const char *trId, MmsStatus mgsStatus,
    MmsClassIdentifier msgClass, MmsVersion version);
MmsResult mmsPromoteNotifToDelayed(WE_UINT32 notifId);

#endif 

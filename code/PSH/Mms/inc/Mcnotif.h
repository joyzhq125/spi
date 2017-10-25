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





#ifndef MCNOTIF_H
#define MCNOTIF_H






typedef enum
{
    





    MMS_SIG_COH_PUSH,

    





    MMS_SIG_COH_NOTIFY_RESP,

    


    MMS_SIG_COH_NOTIFY_POST_RSP
} MmsCohPushSignalId;










void cohPushReceiveEmergencyAbort(void);
void cohPushReceiveInit(void);
void cohPushReceiveTerminate(void);

#endif 
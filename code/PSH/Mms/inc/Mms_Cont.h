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





#ifndef MMS_CONT_H
#define MMS_CONT_H












typedef enum
{
    





    MMS_SIG_CONT_START,

    





    MMS_SIG_CONT_NOTIF
} MmsContSignalId;










void contEmergencyAbort(void);
void contInit(void);
void contTerminate(void);
void contQueue(we_content_data_t* sig);
void contDeQueue(void);

#endif 

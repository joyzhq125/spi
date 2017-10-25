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





#ifndef MRECEIVE_H
#define MRECEIVE_H












typedef enum
{
    





    MMS_SIG_MSR_RECEIVE_CANCEL,

    





    MMS_SIG_MSR_RECEIVE_DELAYED,
    
    





     
    MMS_SIG_MSR_RECEIVE_DELAYED_GET_HEADER_RSP,
    
    



    MMS_SIG_MSR_RECEIVE_GET_RSP,
 
    


    MMS_SIG_MSR_RECEIVE_FILE_CREATED,
    
    


    MMS_SIG_MSR_MSG_DONE_RSP,

    


    MMS_SIG_MSR_RECEIVE_ACK_RSP,

    


    MMS_SIG_MSR_RECEIVE_FILE_DELETED,


    





    MMS_SIG_MSR_RECEIVE_NEXT_IMMEDIATE
} MmsMsrReceiveSignalId;










void msrReceiveEmergencyAbort(void);
void msrReceiveInit(void);
void msrReceiveTerminate(void);
void mmsImmediateRetrievalGet(void);

#endif 

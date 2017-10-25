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




#ifndef MSEND_H
#define MSEND_H











typedef enum
{
    





    MMS_SIG_MSR_SEND_CANCEL,

    





    MMS_SIG_MSR_SEND_MSG,

    





    MMS_SIG_MSR_SEND_RSP,   
   
    






    MMS_SIG_MSR_CREATE_RR,     

    
    





     MMS_SIG_MSR_CREATE_RR_RSP,

    





     MMS_SIG_MSR_SEND_RR_RSP,

     





     MMS_SIG_MSR_SEND_FORWARD, 

    





     MMS_SIG_MSR_SEND_PDU,

    




 
     MMS_SIG_MSR_SEND_FORWARD_RSP
     
} MmsMsrSendSignalId;












void msrSendEmergencyAbort(void);
void msrSendInit(void);
void msrSendTerminate(void);

#endif 


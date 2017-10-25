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





#ifndef MCWAP_H
#define MCWAP_H






typedef enum
{
    MMS_COMM_GET,   
    MMS_COMM_POST   
} MmsCommMethod;



typedef struct
{
    
    MmsRequestId wid;    
    
    
    MmsResult errorNo;
    
    
    WE_INT16 dataType;

    
    WE_INT32 dataLen;      
    
    
    union
    {
        char *fileName;
        char *pipeName;
        char *buffer;
    } data;
} MmsHttpContent;


typedef enum
{
    





    MMS_SIG_COH_WAP_START,

    





    MMS_SIG_COH_WAP_START_RESPONSE,

    





    MMS_SIG_COH_WAP_STOP,

    





    MMS_SIG_COH_WAP_STOP_RESPONSE

    







    

    







    
} MmsCohWapSignalId;










void mmsWapChannelDisconnect(void);
void mmsWapChannelIsDisconnected(void);
void mmsWapConnectionCheck(void);
void mmsWapEmergencyAbort(void);
MmsRequestId mmsWapGetRequestId(void);
long mmsWapGetTransactionId(void);
void mmsWapFreeContentParams(MmsHttpContent *cont);
void mmsWapHttpCancel(MmsRequestId requestId);
void mmsWapHttpGet(MmsRequestId requestId, const char *uri);
void mmsWapHttpPostFile(MmsRequestId requestId, const char *uri, char *fileName);
void mmsWapHttpPostPipe(MmsRequestId requestId, const char *uri, char *pipeName, 
    WE_INT32 dataLen);
void mmsWapHttpPostPdu(MmsRequestId requestId, const char *uri, void *pdu, 
    WE_INT32 size);
void mmsWapHttpResponse(MmsRequestId wid, WE_INT16 status, WE_INT16 dataType, 
    WE_INT32 dataLen, void *data);
void mmsWapInit(void);
void mmsWapTerminate(void);
MmsResult mmsWapTranslateError(WE_INT32 error);
MmsResult mmsWapTranslateResponseStatus(MmsResponseStatus status);
MmsResult mmsWapTranslateRetrieveStatus(MmsRetrieveStatus status);

#endif 

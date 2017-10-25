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




#ifndef MCWSP_H
#define MCWSP_H







typedef struct
{
    char *transactionId; 
    MmsStatus status;    
    MmsDeliveryReportAllowed allowed; 
} MmsNotifyRespInd; 
 


typedef struct
{
    char *transactionId;               
    MmsDeliveryReportAllowed allowed;  
} MmsAcknowledgeInd;










unsigned char *createWspAcknowledgeIndMsg(const MmsAcknowledgeInd *headerValues, 
    WE_UINT32 *length, MmsVersion version);

unsigned char *createWspNotifyRespIndMsg(const MmsNotifyRespInd *header,
    WE_UINT32 *length, MmsVersion version);

unsigned char *createWspReadReport(const MmsGetHeader *header, WE_UINT32 *length, 
    MmsResult *result, MmsReadStatus readStatus, MmsVersion version);

#endif 


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





#ifndef MREPORT_H
#define MREPORT_H








typedef struct
{
    MmsMsgId msgId;           
    MmsReadStatus readStatus; 
} MmsReadReportInfo;











void readReportInitData(void);
void startReadReportCreation(const MmsSignal *sig);
void createReadReport(const MmsSignal *sig);
void createInfoFileForRR (const MmsSignal *sig );
void readReportDone(const MmsSignal *sig);
void create10report(MmsSignal *sig);
void freeReportGlobalData(void);
void deleteNewReadReport(void);

#endif 


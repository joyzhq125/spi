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















#ifndef MSGREAD_H
#define MSGREAD_H

#ifndef WE_CFG_H
#include "We_Cfg.h"
#endif

#ifndef WE_DEF_H
#include "We_Def.h"
#endif

typedef struct
{
    WE_UINT8   modId;      
    void        *first;     
    WE_INT16   instId;     
} MmFSM;

extern MmFSM       mmsMRFSM;




void mmsMREmergencyAbort(void);
void mmsMRInit(void);
void mmsMRTerminate( MmsStateMachine fsm, WE_UINT32 signal, long instance);




void mmsMRgetMsgHeader(MmFSM *factory, WE_UINT8 orderer, WE_UINT32 msgId, WE_UINT32 userData);
void mmsMRreadGetMsgSkeleton(MmFSM *factory, WE_UINT8 orderer, WE_UINT32 msgId, WE_UINT32 userData);
void mmsMRgetMsgBodyPart(MmFSM *factory, WE_UINT8 orderer, WE_UINT32 msgId, WE_UINT32 bpId, WE_UINT32 userData);







WE_BOOL mmsMRhandleDcfProperties(const MmFSM *factory, void *sigData);


WE_BOOL mmsMRhandleOpenFileReply(const MmFSM *factory, void *sigData);


WE_BOOL mmsMRhandleReadFileReply(const MmFSM *factory, void *sigData);

#endif 

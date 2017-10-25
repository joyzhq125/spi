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










#ifndef MMS_UI_H
#define MMS_UI_H

#ifndef WE_CFG_H
#include "We_Cfg.h"
#endif

#ifndef WE_DEF_H
#include "We_Def.h"
#endif

#ifndef WE_DCVT_H
#include "We_Dcvt.h"
#endif

int mmsHandleExtUBSignal(WE_UINT8 module, WE_UINT16 signal, void *sigData);
void mmsHandleUBSStart(void);

void mmsNotifyUBSMsgNew(WE_UINT32 msgId);
void mmsNotifyUBSMsgDeleted(WE_UINT32 msgId);
void mmsNotifyUBSMsgChanged(WE_UINT32 msgId);
void mmsNotifyUBSUnreadMsg(WE_UINT16 nbrOfUnreadMsgs);

#endif 

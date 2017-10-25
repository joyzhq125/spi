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
/*
 * ubs_a.h
 *
 * Created by Klas Hermodsson, Thu May 22 15:47:35 2003.
 *
 * Revision  history:
 * 2003-09-22 (MADE) Removed UBSa_newMessage (int msgType)
 *                   Removed UBSa_noPendingMessages (int msgType)
 *                   Added UBSa_notifyUnreadMsgCount (int msgType, unsigned nbrOfUnreadMsgs);
 * 
 *
 */

#ifndef UBS_A_H
#define UBS_A_H

#include "Ubs_Cfg.h"
#include "Ubs_If.h"



/* ref src adapter function */
/* May be called at any time and several times with the same value.
 * nbrOfUnreadMsgs may be 0 which msans there are no unread messages.
 */
void
UBSa_notifyUnreadMsgCount (int msgType, unsigned nbrOfUnreadMsgs);

/*
 * ref src adapter function
 * UBS calls this function when needing to inform the integration of errors
 */
void
UBSa_error (ubs_result_code_t result_code);

#endif    /*UBS_A_H*/

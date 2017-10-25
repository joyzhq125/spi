/*
 * Copyright (C) Techfaith, 2002-2005.
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
 
#ifndef SAATTACH_H
#define SAATTACH_H

#ifndef SATYPES_H
#error Satypes.h must be included berfore Saattach.h!
#endif
#ifndef SIA_DEF_H
#error Sia_def.h must be included berfore Saattach.h!
#endif

/*--- Definitions/Declarations ---*/

/*--- Macros ---*/

/*--- Types ---*/
/*! \enum EmaMmnuSignalId Signals used with the Message List FSM */
typedef enum
{
	/*! Send data via content routing
     *  - Param I : -
     *  - Param U1: -
     *  - Param U2: - 
     *  - Param P : attachment info (#SisUnrefItemList)
     */
	SIA_SIG_ATTACHMENT_GET_ACTIONS_RSP,
    /*! Object action operation is complete 
     *  - Param I : -  
     *  - Param U1: -
     *  - Param U2: -
     *  - Param P : -
     */
	SIA_SIG_ATTACHMENT_OBJ_ACTION_DONE
} SiaAttachmentSignalId;
/*********************************************
 * Exported function
 *********************************************/
WE_BOOL smaCreateAttachmentMenu(SiaObjectInfoList *objInfoList);
void smaAttachmentTerminate(void);
void smaAttachmentInit(void);
#endif /* SAATTACH_H */



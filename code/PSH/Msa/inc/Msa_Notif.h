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

/*!\file manotif.h
 * \brief Reads a MMS notification. 
 */

#ifndef _MANOTIF_H_
#define _MANOTIF_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaReadNotifStates
 *  Message states when reding a message */
typedef enum
{
    MSA_MAL_OPEN_NOTIF,         /* !< Opening the notification */
    MSA_MAL_READ_NOTIF          /* !< Reading the notification */
}MsaReadNotifStates;

/*! \enum MsaReadNotifResult
 *  Possible return values when reading a message */
typedef enum
{
    MSA_READ_NOTIF_ERROR,     /* !< Error reading the notification */
    MSA_READ_NOTIF_DELAYED,   /* !< The operation is delayed */
    MSA_READ_NOTIF_NEW_STATE, /* !< The operation is split into several calls */
    MSA_READ_NOTIF_DONE       /* !< The message was successfully read */
}MsaReadNotifResult;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct MsaGetNotifInstance
 *  Data type used for notification parsing */
typedef struct
{
    MmsMsgHandle         msgHandle;
    MmsTransactionHandle tr;
    MmsMsgId             msgId;
    MmsFileType          msgType;
    MmsResult            mmsRes;
    MmsNotification      *notif;
    MsaReadNotifStates   state; 
    MsaStateMachine      notifyFsm;    /*!< FSM to report delayed file 
                                            operations to */
    int                  notifySignal; /*!< Signal sent when a delayed file 
                                            operation can be continued */
}MsaGetNotifInstance;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

MsaReadNotifResult msaGetNotifFSM(MsaGetNotifInstance *inst);
WE_BOOL msaCreateGetNotifInstance(MsaGetNotifInstance **inst, MmsMsgId msgId,
    MmsFileType msgType, MsaStateMachine notifyFsm, 
    int notifySignal);
void msaCleanupGetNotifInstance(MsaGetNotifInstance **inst);

#endif /* _MANOTIF_H_ */

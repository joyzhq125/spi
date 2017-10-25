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
 * Pus_If.h
 *
 * Created by Kent Olsson, Wed July 03 18:23:35 2001.
 *
 * Revision  history:
 *
 *
 */

#ifndef _Pus_If_h
#define _Pus_If_h

#ifndef _we_cfg_h
#include "we_cfg.h"
#endif

#ifndef _we_def_h
#include "we_def.h"
#endif

#ifndef _we_dcvt_h
#include "we_dcvt.h"
#endif


/**********************************************************************
 * Signals in the PUSH API:
 **********************************************************************/

/* Signals sent to the PUS Module */
#define PUS_SIG_OPEN_PUSH_CONNECTION             ((WE_MODID_PUS << 8) + 0x01)
#define PUS_SIG_CLOSE_PUSH_CONNECTION            ((WE_MODID_PUS << 8) + 0x02)

/* Signals sent from the PUS Module */
#define PUS_SIG_CONNECTION_OPENED                ((WE_MODID_PUS << 8) + 0x03)
#define PUS_SIG_CONNECTION_CLOSED                ((WE_MODID_PUS << 8) + 0x04)

/**********************************************************************
 * Error Codes:
 **********************************************************************/

#define PUS_ERR_OK                                   0x00
#define PUS_ERR_OK_ALREADY_OPENED_BY_OTHER_MODULE    0x01
#define PUS_ERR_FAILED_HANDLE_ALREADY_EXIST          0x02
#define PUS_ERR_FAILED_TOO_MANY_CONN                 0x03
#define PUS_ERR_FAILED_ALREADY_EXIST_WITH_OTHER_NWA  0x04
#define PUS_ERR_CONN_CLOSED_BY_STK                   0x05

 /**********************************************************************
 * Incoming Signals:
 **********************************************************************/

/* For the signal PSH_SIG_OPEN_PUSH_CONNECTION */
typedef struct 
{
  WE_INT16  handle;
  WE_UINT16 portNumber;
  WE_INT16  connectionType;
  WE_INT32  networkAccountId;
} pus_open_push_connection_t;

/* For the signal PSH_SIG_CLOSE_PUSH_CONNECTION */
typedef struct {
  WE_INT16 handle;
} pus_close_push_connection_t;

/**********************************************************************
 * Outgoing Signals:
 **********************************************************************/

/* For the signal PUS_SIG_CONNECTION_OPENED */
typedef struct {
  WE_INT16 handle;
  WE_INT16 errorCode;
} Pus_Connection_opened_t;

/* For the signal PUS_SIG_CONNECTION_CLOSED */
typedef struct {
  WE_INT16 handle;
  WE_INT16 errorCode;
} Pus_Connection_closed_t;

/*
 * Called by any other module that wants to use the PUS module.
 */
void
PUSif_startInterface (void);


/**********************************************************************
 * Signal-sending API
 * These functions should be used by any other module that wants to
 * send a signal to the PSH module.
 **********************************************************************/

/*
 * Signals sent from client to PUS Module
 */
void
PUSif_openPushConnection (WE_UINT8 modId, int handle,
                          WE_UINT16 portNumber, int connectionType,
                          WE_INT32 networkAccountId);

void
PUSif_closePushConnection (WE_UINT8 modId, int handle);


/**********************************************************************
 * Conversion routines for all signal owned by this module.
 * None of these functions should be called directly by another module.
 **********************************************************************/

int
pus_cvt_open_push_connection (we_dcvt_t *obj, pus_open_push_connection_t* p);

int
pus_cvt_close_push_connection (we_dcvt_t *obj, pus_close_push_connection_t* p);

int
pus_cvt_connection_opened (we_dcvt_t *obj, Pus_Connection_opened_t *p);

int
pus_cvt_connection_closed (we_dcvt_t *obj, Pus_Connection_closed_t *p);

/*
 * Convert from memory buffer to signal structs.
 */
void*
pus_convert (WE_UINT8 module, WE_UINT16 signal, void* buffer);

/*
 * Released a signal structure to a signal.
 */
void
pus_destruct (WE_UINT8 module, WE_UINT16 signal, void* p);

#endif

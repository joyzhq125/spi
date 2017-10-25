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
 * We_Env.h
 *
 * Created by Ingmar Persson
 *
 * Revision  history:
 * 
 *
 */

#ifndef _we_env_h
#define _we_env_h

#ifndef _we_def_h
#include "We_Def.h"
#endif
#ifndef _we_int_h
#include "We_Int.h"
#endif

/****************************************
 * Messaging
 ****************************************/

WE_UINT8
TPIs_signalGetDestination (void* signalData);

WE_UINT8
TPIs_signalGetSource (void* signalData);


/**********************************************************************
 * Named Pipes
 **********************************************************************/

void
TPIs_PipeNotify (WE_UINT8 modId, int handle, int eventType);


/**********************************************************************
 * Sockets
 **********************************************************************/

void
TPIs_socketConnectResponse (WE_UINT8 modId, int socketId, int result);

void
TPIs_socketNotify (WE_UINT8 modId, int socketId, int eventType);

void
TPIs_socketHostByName (WE_UINT8 modId, int requestId, int result,
                       unsigned char *addr, int addrLen);

/**********************************************************************
 * File
 **********************************************************************/
typedef struct {
  WE_INT32  space_used;
  WE_INT32  space_free;
  WE_INT32  slots_used;
  WE_INT32  slots_left;
} we_quota_t;

typedef struct {
  WE_INT32 max_path;
  WE_INT32 max_file;
  WE_INT16 max_ext;
  WE_INT8 is_external;
} we_path_property_t;

void
TPIs_fileNotify (WE_UINT8 modId, int fileHandle, int eventType);

void
TPIs_filePathPropertyResponse (WE_UINT8 modId, WE_INT32 wid, 
                               const we_path_property_t *param);

void
TPIs_fileQuotaResponse (WE_UINT8 modId, WE_INT32 wid, const we_quota_t *param);

/**********************************************************************
 * Telephone
 **********************************************************************/

void
TPIs_telMakeCallResponse (WE_UINT8 modId, WE_UINT16 telId, int result);

void
TPIs_telSendDtmfResponse (WE_UINT8 modId, WE_UINT16 telId, int result);


/**********************************************************************
 * Phonebook
 **********************************************************************/

void
TPIs_pbAddEntryResponse (WE_UINT8 modId, WE_UINT16 pbId, int result);

/**********************************************************************
 * Public Key Cryptography 
 **********************************************************************/

void
TPIs_crptEncryptPkcResponse (WE_UINT8 modId, int wid, int result,
                             const unsigned char *buf, int bufLen);

void
TPIs_crptDecryptPkcResponse (WE_UINT8 modId, int wid, int result,
                             unsigned char *buf, int bufLen);
void
TPIs_crptComputeSignatureResponse (WE_UINT8 modId, int wid, int result, 
                                   const unsigned char *sig, int sigLen);

void
TPIs_crptVerifySignatureResponse (WE_UINT8 modId, int wid, int result);


void
TPIs_crptGenerateKeyPairResponse (WE_UINT8 modId, int wid, int result,
                                  int pkcAlg, 
                                  const we_crpt_pub_key_t* pubKey,
                                  const we_crpt_priv_key_t* privKey);

void
TPIs_crptDhKeyExchangeResponse (WE_UINT8 modId, int wid, int result,
                                const unsigned char *publicValue, 
                                int publicValueLen,
                                const unsigned char *secretValue, 
                                int secretLen);

/**********************************************************************
 * Widgets
 **********************************************************************/

void
TPIs_widgetNotify(WE_UINT8 modId, WE_UINT32 weHandle, int notificationType);

void
TPIs_widgetAction(WE_UINT8 modId, WE_UINT32 weHandle, WE_UINT32 action);

void
TPIs_widgetUserEvent(WE_UINT8 modId, WE_UINT32 weHandle, WeEvent* weEvent);

void
TPIs_widgetAccessKey(WE_UINT8 modId, WE_UINT32 weHandle, WE_UINT32 accessKey);


#endif

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
 * We_Env.c
 *
 * Created by Ingmar Persson, 
 *
 * Revision history:
 *
 */
#include "We_Env.h"
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Lib.h"
#include "We_Log.h"

WE_UINT8
TPIs_signalGetDestination (void* signalData)
{
  WE_UINT8 retVal;

  WE_LOG_FC_BEGIN(TPIs_signalGetDestination)
  WE_LOG_FC_PTR(signalData, NULL)
  WE_LOG_FC_PRE_IMPL

  retVal = WE_SIGNAL_GET_DESTINATION (signalData);

  WE_LOG_FC_UINT8(retVal, NULL)
  WE_LOG_FC_END

  return retVal;
}

WE_UINT8
TPIs_signalGetSource (void* signalData)
{
  WE_UINT8 retVal;

  WE_LOG_FC_BEGIN(TPIs_signalGetSource)
  WE_LOG_FC_PTR(signalData, NULL)
  WE_LOG_FC_PRE_IMPL

  retVal = WE_SIGNAL_GET_SOURCE (signalData);

  WE_LOG_FC_UINT8(retVal, NULL)
  WE_LOG_FC_END
  return retVal; 
}

void
TPIc_PipeNotify (WE_UINT8 modId, int handle, int eventType)
{
  we_pipe_notify_t  p;
  we_dcvt_t         cvt_obj;
  WE_UINT16         length;
  void              *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIc_PipeNotify)
  WE_LOG_FC_INT(handle, NULL)
  WE_LOG_FC_INT(eventType, NULL)
  WE_LOG_FC_PRE_IMPL

  p.handle = handle;
  p.eventType = (WE_INT16)eventType;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_pipe_notify (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_PIPE_NOTIFY, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_pipe_notify (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }

  WE_LOG_FC_END
}

void
TPIs_socketConnectResponse (WE_UINT8 modId, int socketId, int result)
{
  we_socket_connect_response_t  p;
  we_dcvt_t                     cvt_obj;
  WE_UINT16                     length;
  void                          *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_socketConnectResponse)
  WE_LOG_FC_UINT8(modId, NULL) 
  WE_LOG_FC_INT(socketId, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_PRE_IMPL

  p.socketId = socketId;
  p.result = (WE_INT16)result;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_socket_connect_response (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_SOCKET_CONNECT_RESPONSE,
                                     WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_socket_connect_response (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }

  WE_LOG_FC_END
}

void
TPIs_socketNotify (WE_UINT8 modId, int socketId, int eventType)
{
  we_socket_notify_t  p;
  we_dcvt_t           cvt_obj;
  WE_UINT16           length;
  void                *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_socketNotify)
  WE_LOG_FC_UINT8(modId, NULL) 
  WE_LOG_FC_INT(socketId, NULL)
  WE_LOG_FC_INT(eventType, NULL)
  WE_LOG_FC_PRE_IMPL

  p.socketId = socketId;
  p.eventType = (WE_INT16)eventType;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_socket_notify (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_SOCKET_NOTIFY, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_socket_notify (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }

  WE_LOG_FC_END
}

/**********************************************************************
 * File
 **********************************************************************/

void
TPIs_fileNotify (WE_UINT8 modId, int fileHandle, int eventType)
{
  we_file_notify_t  p;
  we_dcvt_t           cvt_obj;
  WE_UINT16           length;
  void                *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_fileNotify)
  WE_LOG_FC_INT(fileHandle, NULL)
  WE_LOG_FC_INT(eventType, NULL)
  WE_LOG_FC_PRE_IMPL

  p.fileHandle = fileHandle;
  p.eventType = (WE_INT16)eventType;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_file_notify (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_FILE_NOTIFY, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_file_notify (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  } 
  WE_LOG_FC_END
}


void
TPIs_filePathPropertyResponse (WE_UINT8 modId, WE_INT32 wid, 
                               const we_path_property_t *param)
{
  we_dcvt_t                     cvt_obj;
  WE_UINT16                     length;
  void                          *signal_buffer; 
  void                          *user_data;
  we_file_path_property_resp_t  p;

  WE_LOG_FC_BEGIN(TPIs_filePathPropertyResponse)
  WE_LOG_FC_UINT8(modId, NULL)
  WE_LOG_FC_INT32(wid, NULL)
  WE_LOG_FC_PRE_IMPL
  
  p.wid = wid;
  p.param_exist = (WE_INT8)((param == NULL)? 0:1);
  p.p  = (we_path_property_t *)param;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_file_path_property_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_FILE_PATH_PROPERTY_RESP, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_file_path_property_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  } 

  WE_LOG_FC_END
}

void
TPIs_fileQuotaResponse (WE_UINT8 modId, WE_INT32 wid, const we_quota_t *param)
{
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer; 
  void                  *user_data;
  we_file_quota_resp_t  p;

  WE_LOG_FC_BEGIN(TPIs_fileQuotaResponse)
  WE_LOG_FC_UINT8(modId, NULL)
  WE_LOG_FC_INT32(wid, NULL)
  WE_LOG_FC_PRE_IMPL
  
  p.wid = wid;
  p.param_exist = (WE_INT8)((param == NULL)? 0:1);
  p.p  = (we_quota_t*)param;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_file_quota_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_FILE_QUOTA_RESP, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_file_quota_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  } 
  
  WE_LOG_FC_END
}



/**********************************************************************
 * Sockets
 **********************************************************************/

void
TPIs_socketHostByName (WE_UINT8 modId, int requestId, int result,
                       unsigned char *addr, int addrLen)
{
  we_socket_host_by_name_t  p;
  we_dcvt_t                 cvt_obj;
  WE_UINT16                 length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_socketHostByName)
  WE_LOG_FC_INT(requestId, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_INT(addrLen, NULL)
  WE_LOG_FC_BYTES(addr, addrLen, NULL) 
  WE_LOG_FC_PRE_IMPL

  p.requestId = requestId;
  p.result = (WE_INT16)result;
  p.addrLen = (WE_INT16)addrLen;
  memcpy (p.addr, addr, addrLen);

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_socket_host_by_name (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_SOCKET_HOST_BY_NAME, WE_MODID_FRW,
                                     modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_socket_host_by_name (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}

void
TPIs_telMakeCallResponse (WE_UINT8 modId, WE_UINT16 telId, int result)
{
  we_tel_make_call_resp_t  p;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_telMakeCallResponse)
  WE_LOG_FC_UINT16(telId, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_PRE_IMPL

  p.telId = telId;
  p.result = (WE_INT16)result;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_tel_make_call_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_TEL_MAKE_CALL_RESPONSE, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_tel_make_call_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  } 
  WE_LOG_FC_END
}

void
TPIs_telSendDtmfResponse (WE_UINT8 modId, WE_UINT16 telId, int result)
{
  we_tel_send_dtmf_resp_t  p;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_telSendDtmfResponse)
  WE_LOG_FC_UINT16(telId, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_PRE_IMPL

  p.telId = telId;
  p.result = (WE_INT16)result;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_tel_send_dtmf_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_TEL_SEND_DTMF_RESPONSE, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_tel_send_dtmf_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  } 
  WE_LOG_FC_END
}

void
TPIs_pbAddEntryResponse (WE_UINT8 modId, WE_UINT16 pbId, int result)
{
  we_pb_add_entry_resp_t   p;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_pbAddEntryResponse)
  WE_LOG_FC_UINT16(pbId, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_PRE_IMPL

  p.pbId = pbId;
  p.result = (WE_INT16)result;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_pb_add_entry_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_PB_ADD_ENTRY_RESPONSE, WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_pb_add_entry_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  } 
  WE_LOG_FC_END
}

/**********************************************************************
 * Public Key Cryptography 
 **********************************************************************/

void
TPIs_crptEncryptPkcResponse (WE_UINT8 modId, int wid, int result,
                             const unsigned char *buf, int bufLen)
{
  we_crpt_encrypt_pkc_resp_t  p;
  we_dcvt_t                   cvt_obj;
  WE_UINT16                   length;
  void                        *signal_buffer, *user_data;
  
  WE_LOG_FC_BEGIN(TPIs_crptEncryptPkcResponse)
  WE_LOG_FC_INT(wid, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_INT(bufLen, NULL)
  WE_LOG_FC_BYTES(buf, bufLen, NULL) 
  WE_LOG_FC_PRE_IMPL

  p.wid  = (WE_INT16)wid;
  p.result = (WE_INT16)result;
  p.buf = (unsigned char*) buf;
  p.bufLen = (WE_INT16)bufLen;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_crpt_encrypt_pkc_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_CRPT_ENCRYPT_PKC_RESPONSE,
                                     WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_crpt_encrypt_pkc_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}

void
TPIs_crptDecryptPkcResponse (WE_UINT8 modId, int wid, int result,
                             unsigned char *buf, int bufLen)
{
  we_crpt_decrypt_pkc_resp_t  p;
  we_dcvt_t                   cvt_obj;
  WE_UINT16                   length;
  void                        *signal_buffer, *user_data;
  
  WE_LOG_FC_BEGIN(TPIs_crptDecryptPkcResponse)
  WE_LOG_FC_INT(wid, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_INT(bufLen, NULL)
  WE_LOG_FC_BYTES(buf, bufLen, NULL) 
  WE_LOG_FC_PRE_IMPL

  p.wid  = (WE_INT16)wid;
  p.result = (WE_INT16)result;
  p.buf = (unsigned char*) buf;
  p.bufLen = (WE_INT16)bufLen;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_crpt_decrypt_pkc_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_CRPT_DECRYPT_PKC_RESPONSE,
                                     WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_crpt_decrypt_pkc_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}

void
TPIs_crptVerifySignatureResponse (WE_UINT8 modId, int wid, int result)
{
  we_crpt_verify_signature_resp_t  p;
  we_dcvt_t                        cvt_obj;
  WE_UINT16                        length;
  void                             *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_crptVerifySignatureResponse)
  WE_LOG_FC_INT(wid, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_PRE_IMPL

  p.wid  = (WE_INT16)wid;
  p.result = (WE_INT16)result;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_crpt_verify_signature_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_CRPT_VERIFY_SIGN_RESPONSE,
                                     WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_crpt_verify_signature_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}

void
TPIs_crptComputeSignatureResponse (WE_UINT8 modId, int wid, int result, 
                                   const unsigned char *sig, int sigLen)
{
  we_crpt_compute_signature_resp_t  p;
  we_dcvt_t                         cvt_obj;
  WE_UINT16                         length;
  void                              *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_crptComputeSignatureResponse)
  WE_LOG_FC_INT(wid, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_INT(sigLen, NULL)
  WE_LOG_FC_BYTES(sig, sigLen, NULL) 
  WE_LOG_FC_PRE_IMPL


  p.wid  = (WE_INT16)wid;
  p.result = (WE_INT16)result;
  p.sig = (unsigned char*) sig;
  p.sigLen = (WE_INT16)sigLen;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_crpt_compute_signature_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_CRPT_COMPUTE_SIGN_RESPONSE,
                                     WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_crpt_compute_signature_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}

void
TPIs_crptGenerateKeyPairResponse (WE_UINT8 modId, int wid, int result,
                                  int pkcAlg, const we_crpt_pub_key_t *pubKey,
                                  const we_crpt_priv_key_t *privKey)
{
  we_crpt_generate_keypair_resp_t  p;
  we_dcvt_t                        cvt_obj;
  WE_UINT16                        length;
  void                             *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_crptGenerateKeyPairResponse)
  WE_LOG_FC_INT(wid, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_INT(pkcAlg, NULL)
  WE_LOG_FC_PRE_IMPL

  p.wid  = (WE_INT16)wid;
  p.result = (WE_INT16)result;
  p.pkcAlg = (WE_UINT8)pkcAlg;
  p.pubKey = *pubKey;
  p.privKey = *privKey;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_crpt_generate_keypair_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_CRPT_GENERATE_KEY_RESPONSE, 
                                     WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_crpt_generate_keypair_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}

void
TPIs_crptDhKeyExchangeResponse (WE_UINT8 modId, int wid, int result,
                                const unsigned char *publicValue, 
                                int publicValueLen,
                                const unsigned char *secretValue, 
                                int secretLen)
{
  we_crpt_dh_keyexchange_resp_t  p;
  we_dcvt_t                      cvt_obj;
  WE_UINT16                      length;
  void                           *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(TPIs_crptDhKeyExchangeResponse)
  WE_LOG_FC_INT(wid, NULL)
  WE_LOG_FC_INT(result, NULL)
  WE_LOG_FC_PRE_IMPL

  p.wid = (WE_INT16)wid;
  p.result = (WE_INT16)result;
  p.publicValue = (unsigned char*)publicValue; 
  p.publicValueLen = (WE_INT16)publicValueLen;
  p.secretValue = (unsigned char*)secretValue;
  p.secretLen = (WE_INT16)secretLen;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_crpt_dh_keyexchange_resp (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (WE_SIG_CRPT_DH_KEYEXCHANGE_RESPONSE, 
                                     WE_MODID_FRW, modId, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    we_cvt_crpt_dh_keyexchange_resp (&cvt_obj, &p);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}

void
TPIs_widgetNotify(WE_UINT8 modId, WE_UINT32 weHandle, int notificationType)
{
  we_widget_notify_t p;
  we_dcvt_t          cvt_obj;
  void                *buffer, *user_data;
  WE_UINT16          length;

  WE_LOG_FC_BEGIN(TPIs_widgetNotify)
  WE_LOG_FC_UINT32(weHandle, NULL) 
  WE_LOG_FC_INT(notificationType, NULL)
  WE_LOG_FC_PRE_IMPL

  p.handle = weHandle;
  p.notificationType = (WeNotificationType) notificationType;
  buffer = WE_SIGNAL_CREATE (WE_SIG_WIDGET_NOTIFY, WE_MODID_FRW, modId, 5);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, WE_MODID_FRW);
    we_cvt_wid_notify (&cvt_obj, &p);
  }
  WE_SIGNAL_SEND (buffer);

  WE_LOG_FC_END
}

void
TPIs_widgetAction(WE_UINT8 modId, WE_UINT32 weHandle, WE_UINT32 action)
{
  we_widget_action_t p;
  we_dcvt_t          cvt_obj;
  void                *buffer, *user_data;
  WE_UINT16          length;

  WE_LOG_FC_BEGIN(TPIs_widgetAction)
  WE_LOG_FC_UINT32(weHandle, NULL) 
  WE_LOG_FC_INT(action, NULL)
  WE_LOG_FC_PRE_IMPL

  p.handle = weHandle;
  p.action = action;

  WE_LOG_SIG_BEGIN("WE_SIG_WIDGET_ACTION", LS_SEND, WE_MODID_FRW, modId);
  WE_LOG_SIG_UINT32("handle", p.handle);
  WE_LOG_SIG_UINT32("action", p.action);
  WE_LOG_SIG_END();

  buffer = WE_SIGNAL_CREATE (WE_SIG_WIDGET_ACTION, WE_MODID_FRW, modId, 8);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, WE_MODID_FRW);
    we_cvt_wid_action (&cvt_obj, &p);
  }
  WE_SIGNAL_SEND (buffer);

  WE_LOG_FC_END
}

void
TPIs_widgetUserEvent(WE_UINT8 modId, WE_UINT32 weHandle, WeEvent* weEvent)
{
  we_widget_userevt_t  p;
  we_dcvt_t            cvt_obj;
  void                  *buffer, *user_data;
  WE_UINT16            length;

  WE_LOG_FC_BEGIN(TPIs_widgetUserEvent)
  WE_LOG_FC_UINT32(weHandle, NULL)
  WE_LOG_FC_INT(weEvent->eventClass, NULL)
  WE_LOG_FC_UINT8(weEvent->eventMode, NULL)
  WE_LOG_FC_INT(weEvent->eventType, NULL)
#ifdef WE_LOG_FC
  if(weEvent->eventType == WePointer)
  {
    WE_LOG_FC_INT16(weEvent->position.x, NULL)
    WE_LOG_FC_INT16(weEvent->position.y, NULL)
  }
#endif /*WE_LOG_FC*/
  WE_LOG_FC_PRE_IMPL

  p.handle = weHandle;
  p.event.eventMode = weEvent->eventMode;
  p.event.eventClass = weEvent->eventClass;
  p.event.eventType = weEvent->eventType;
  p.event.position.x = weEvent->position.x;
  p.event.position.y = weEvent->position.y;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_wid_user_event (&cvt_obj, &p);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_WIDGET_USEREVT, WE_MODID_FRW, modId, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, WE_MODID_FRW);
    we_cvt_wid_user_event (&cvt_obj, &p);
  }
  WE_SIGNAL_SEND (buffer);

  WE_LOG_FC_END
}

void
TPIs_widgetAccessKey(WE_UINT8 modId, WE_UINT32 weHandle, WE_UINT32 accessKey)
{
  we_widget_access_key_t p;
  we_dcvt_t              cvt_obj;
  void                    *buffer, *user_data;
  WE_UINT16              length;

  WE_LOG_FC_BEGIN(TPIs_widgetAccessKey)
  WE_LOG_FC_UINT32(weHandle, NULL) 
  WE_LOG_FC_INT(accessKey, NULL)
  WE_LOG_FC_PRE_IMPL

  p.handle = weHandle;
  p.accessKey = accessKey;
  buffer = WE_SIGNAL_CREATE (WE_SIG_WIDGET_ACCESS_KEY, WE_MODID_FRW, modId, 8);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, WE_MODID_FRW);
    we_cvt_wid_access_key (&cvt_obj, &p);
  }
  WE_SIGNAL_SEND (buffer);

  WE_LOG_FC_END
}



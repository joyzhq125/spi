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
 * We_Core.c
 *
 * Created by Ingmar Persson
 *
 * Revision history:
 *   021011  IPN  Send a notfication when a parameter in the registry is deleted.
 *   021105  IPN  Changed the signal definition.
 *   021105  IPN  TPIa_signalRetrieve only returns a pointer.
 *
 */

#include "We_Core.h"
#include "We_Mem.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Cmmn.h"
#include "We_Errc.h"
#include "We_Ess.h"

/**********************************************************************
 * Macro definition.
 **********************************************************************/

#define WE_GET_SIGNAL_OWNER(s)     ((s & 0xFF00) >> 8)


/**********************************************************************
 * Global array that stores the modules different covertion functions,
 * that covert from memory buffer to signal struct.
 **********************************************************************/
static convert_function_t *we_convert_func[WE_NUMBER_OF_MODULES];

/**********************************************************************
 * Global array that stores the modules different destruct functions,
 * that destruct a Modules signal structure.
 **********************************************************************/
static destruct_function_t *we_destruct_func[WE_NUMBER_OF_MODULES];


/**********************************************************************
 * Conversion routines for signal structs
 **********************************************************************/
int
we_cvt_embedded_data (we_dcvt_t *obj, we_embedded_data_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->screenHandle)) ||
      !we_dcvt_uint32 (obj, &(p->embeddedWindowHandle))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_content_data (we_dcvt_t *obj, we_content_data_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->routingFormat)) ||
      !we_dcvt_string (obj, &(p->routingIdentifier)) ||
      !we_dcvt_string (obj, &(p->contentUrl)) ||
      !we_dcvt_string (obj, &(p->contentSource)) ||
      !we_dcvt_string (obj, &(p->contentParameters)) ||
      !we_dcvt_string (obj, &(p->contentHeaders)) ||
      !we_dcvt_int32 (obj, &(p->networkAccountId)) ||
      !we_dcvt_uint8 (obj, &(p->contentDataType)) ||
      !we_dcvt_uint32 (obj, &(p->contentDataLength))) {
    return FALSE;
  }
  
  if (p->contentDataType == WE_CONTENT_DATA_RAW){
    if (!we_dcvt_uchar_vector (obj, p->contentDataLength, &(p->contentData)))
      return FALSE;
  }
  else
  {
   /* The actual data to be routed is in contentData 
      only for WE_CONTENT_DATA_RAW */
    p->contentData = NULL;
  }
  
  if (!we_dcvt_string (obj, &(p->contentType)) ||
      !we_dcvt_uint8 (obj, &(p->embeddedObjectExists)) ||
      !we_dcvt_array (obj, sizeof (we_embedded_data_t), 
                       p->embeddedObjectExists,
                       (void **)&(p->embeddedObjectData), 
                       (we_dcvt_element_t *)we_cvt_embedded_data)) {
        return FALSE;
  }
  
  return TRUE;
}

int
we_cvt_act_content(we_dcvt_t *obj, we_act_content_t *p)
{
  if (!we_dcvt_string (obj, &(p->mime_type)) ||
      !we_dcvt_int32 (obj, (WE_INT32*) &(p->data_type)) ||
      !we_dcvt_uint32 (obj, &(p->data_len)) ||
      !we_dcvt_uchar_vector (obj, p->data_len, &(p->data)) ||
      !we_dcvt_string (obj, &(p->src_path)) ||
      !we_dcvt_string (obj, &(p->default_name)) ||
      !we_dcvt_string (obj, &(p->content_type))) {

    return FALSE;
  }
  return TRUE;
}

int
we_cvt_module_start (we_dcvt_t *obj, we_module_start_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->modId)) ||
      !we_dcvt_string (obj, &(p->startOptions)) ||
      !we_dcvt_uint8 (obj, &(p->contentDataExists)) ||
      !we_dcvt_array (obj, sizeof (we_content_data_t), p->contentDataExists,
                       (void **)&(p->contentData), (we_dcvt_element_t *)we_cvt_content_data)) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_module_terminate (we_dcvt_t *obj, we_module_terminate_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->modId))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_module_status (we_dcvt_t *obj, we_module_status_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->status)) ||
      !we_dcvt_uint8 (obj, &(p->modId)) ||
      !we_dcvt_string (obj, &(p->modVersion))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_module_execute_cmd (we_dcvt_t *obj, we_module_execute_cmd_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->srcModId)) ||
      !we_dcvt_string (obj, &(p->commandOptions)) ||
      !we_dcvt_uint8 (obj, &(p->contentDataExists)) ||
      !we_dcvt_array (obj, sizeof (we_content_data_t), p->contentDataExists,
                       (void **)&(p->contentData), (we_dcvt_element_t *)we_cvt_content_data)) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_timer_set (we_dcvt_t *obj, we_timer_set_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->timerID)) ||
      !we_dcvt_uint32 (obj, &(p->timerInterval))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_timer_reset (we_dcvt_t *obj, we_timer_reset_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->timerID))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_timer_expired (we_dcvt_t *obj, we_timer_expired_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->timerID))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_wid_notify (we_dcvt_t *obj, we_widget_notify_t *p)
{
  WE_UINT8 not_type = (WE_UINT8)(p->notificationType);
  if (!we_dcvt_uint32 (obj, &(p->handle)) ||
      !we_dcvt_uint8 (obj, &not_type)) {
    return FALSE;
  }
  p->notificationType = (WeNotificationType)not_type;
  return TRUE;
}

int
we_cvt_wid_action (we_dcvt_t *obj, we_widget_action_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->handle)) ||
      !we_dcvt_uint32 (obj, &(p->action))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_wid_user_event (we_dcvt_t *obj, we_widget_userevt_t *p)
{
  WE_UINT8  eventType = (WE_UINT8)(p->event.eventType);
  WE_UINT8  eventClass = (WE_UINT8)(p->event.eventClass);
  if (!we_dcvt_uint32 (obj, &(p->handle)) ||
      !we_dcvt_uint8 (obj, &(p->event.eventMode)) ||
      !we_dcvt_uint8 (obj, &eventClass) ||
      !we_dcvt_uint8 (obj, &eventType) ||
      !we_dcvt_int16 (obj, &(p->event.position.x)) ||
      !we_dcvt_int16 (obj, &(p->event.position.y))) {
    return FALSE;
  }
  p->event.eventType = (WeEventType)eventType;
  p->event.eventClass = (WeEventClass)eventClass;
  return TRUE;
}

int
we_cvt_wid_access_key (we_dcvt_t *obj, we_widget_access_key_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->handle)) ||
      !we_dcvt_uint32 (obj, &(p->accessKey))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_pipe_notify (we_dcvt_t *obj, we_pipe_notify_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->handle)) ||
      !we_dcvt_int16 (obj, &(p->eventType))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_file_notify (we_dcvt_t *obj, we_file_notify_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->fileHandle)) ||
      !we_dcvt_int16 (obj, &(p->eventType))) {
    return FALSE;
  }
  return TRUE;
}

int 
we_cvt_file_path_property(we_dcvt_t *obj, we_path_property_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->max_path)) ||
      !we_dcvt_int32 (obj, &(p->max_file)) ||
      !we_dcvt_int16 (obj, &(p->max_ext))  ||
      !we_dcvt_int8  (obj, &(p->is_external))) {
    return FALSE;
  }
  
  return TRUE;
}

int
we_cvt_file_path_property_resp (we_dcvt_t *obj, we_file_path_property_resp_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->wid)) ||
      !we_dcvt_uint8 (obj, &(p->param_exist)) ||
      !we_dcvt_array(obj, sizeof(we_path_property_t), 
                      p->param_exist,(void**)&(p->p), 
                      (we_dcvt_element_t *)we_cvt_file_path_property)){
    return FALSE;
  }

  if(!p->param_exist){
    p->p = NULL;
  }

  return TRUE;

}

static int
we_cvt_file_quota (we_dcvt_t *obj, we_quota_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->space_used)) ||
      !we_dcvt_int32 (obj, &(p->space_free)) ||
      !we_dcvt_int32 (obj, &(p->slots_used))  ||
      !we_dcvt_int32 (obj, &(p->slots_left))) {
    return FALSE;
  }
  
  return TRUE;
}

int
we_cvt_file_quota_resp (we_dcvt_t *obj, we_file_quota_resp_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->wid)) ||
      !we_dcvt_uint8 (obj, &(p->param_exist)) ||
      !we_dcvt_array(obj, sizeof(we_quota_t), 
                      p->param_exist,(void**)&(p->p), 
                      (we_dcvt_element_t *)we_cvt_file_quota)) {
    return FALSE;
  }

  if(!p->param_exist){
    p->p = NULL;
  }

  return TRUE;
}


int
we_cvt_socket_connect_response (we_dcvt_t *obj, we_socket_connect_response_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->socketId)) ||
      !we_dcvt_int16 (obj, &(p->result))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_socket_notify (we_dcvt_t *obj, we_socket_notify_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->socketId)) ||
      !we_dcvt_int16 (obj, &(p->eventType))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_socket_host_by_name (we_dcvt_t *obj, we_socket_host_by_name_t *p)
{
  if (!we_dcvt_int32 (obj, &(p->requestId)) ||
      !we_dcvt_int16 (obj, &(p->result)) ||
      !we_dcvt_int16 (obj, &(p->addrLen)) ||
      !we_dcvt_static_uchar_vector (obj, p->addrLen, p->addr)) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_registry_identifier (we_dcvt_t *obj, we_registry_identifier_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_string (obj, &(p->path)) ||
      !we_dcvt_string (obj, &(p->key))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_registry_subscription (we_dcvt_t *obj, we_registry_subscription_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_bool (obj, &(p->add)) ||
      !we_dcvt_string (obj, &(p->path)) ||
      !we_dcvt_string (obj, &(p->key)) ) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_content_send (we_dcvt_t *obj, we_content_send_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->destModId)) ||
      !we_dcvt_uint8 (obj, &(p->sendAck)) ||
      !we_dcvt_uint8 (obj, &(p->wid)) ||
      !we_dcvt_uint8 (obj, &(p->useObjectAction)) ||
      !we_dcvt_string (obj, &(p->startOptions)) ||
      !we_dcvt_uint8 (obj, &(p->contentDataExists)) ||
      !we_dcvt_array (obj, sizeof (we_content_data_t), p->contentDataExists,
                       (void **)&(p->contentData), (we_dcvt_element_t *)we_cvt_content_data)) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_content_send_ack (we_dcvt_t *obj, we_content_send_ack_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->wid)) ||
      !we_dcvt_uint8 (obj, &(p->status)) ||
      !we_dcvt_bool (obj, &(p->caller_owns_pipe))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_tel_make_call_resp (we_dcvt_t *obj, we_tel_make_call_resp_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->telId)) ||
      !we_dcvt_int16 (obj, &(p->result))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_tel_send_dtmf_resp (we_dcvt_t *obj, we_tel_send_dtmf_resp_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->telId)) ||
      !we_dcvt_int16 (obj, &(p->result))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_pb_add_entry_resp (we_dcvt_t *obj, we_pb_add_entry_resp_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->pbId)) ||
      !we_dcvt_int16 (obj, &(p->result))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_crpt_encrypt_pkc_resp (we_dcvt_t *obj, 
                               we_crpt_encrypt_pkc_resp_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_int16 (obj, &(p->result)) ||
      !we_dcvt_int16 (obj, &(p->bufLen)) ||
      !we_dcvt_uchar_vector (obj, p->bufLen, &(p->buf))) {
    return FALSE;
  }
  return TRUE;  
}

int
we_cvt_crpt_decrypt_pkc_resp (we_dcvt_t *obj, 
                               we_crpt_decrypt_pkc_resp_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_int16 (obj, &(p->result)) ||
      !we_dcvt_int16 (obj, &(p->bufLen)) ||
      !we_dcvt_uchar_vector (obj, p->bufLen, &(p->buf))) {
    return FALSE;
  }
  return TRUE;  
}

int
we_cvt_crpt_verify_signature_resp (we_dcvt_t *obj, 
                                    we_crpt_verify_signature_resp_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_int16 (obj, &(p->result))) {
    return FALSE;
  }
  return TRUE;
} 

int
we_cvt_crpt_compute_signature_resp (we_dcvt_t *obj, 
                                     we_crpt_compute_signature_resp_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_int16 (obj, &(p->result)) ||
      !we_dcvt_int16 (obj, &(p->sigLen)) ||
      !we_dcvt_uchar_vector (obj, p->sigLen, &(p->sig))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_crpt_generate_keypair_resp (we_dcvt_t *obj, 
                                    we_crpt_generate_keypair_resp_t *p)
{
  we_crpt_pub_key_rsa_t* rsa_key;
  we_crpt_pub_key_dsa_t* dsa_key;

  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_int16 (obj, &(p->result)) ||
      !we_dcvt_uint8 (obj, &(p->pkcAlg))) {
    return FALSE;
  }

  if (p->pkcAlg == TPI_CRPT_PKC_RSA) {  
    rsa_key = &(p->pubKey._u.rsa);
    if (!we_dcvt_uint16 (obj, &(rsa_key->expLen)) ||
        !we_dcvt_uchar_vector (obj, rsa_key->expLen, &(rsa_key->exponent)) ||
        !we_dcvt_uint16 (obj, &(rsa_key->modLen)) ||
        !we_dcvt_uchar_vector (obj, rsa_key->modLen, &(rsa_key->modulus))) {
      return FALSE;
    }
  } 
  else if (p->pkcAlg == TPI_CRPT_PKC_DSA) {
    dsa_key = &(p->pubKey._u.dsa);
    if (!we_dcvt_uint16 (obj, &(dsa_key->keyLen)) ||
        !we_dcvt_uchar_vector (obj, dsa_key->keyLen, &(dsa_key->key)) ||
        !we_dcvt_uint16 (obj, &(dsa_key->dsaPLen)) ||
        !we_dcvt_uchar_vector (obj, dsa_key->dsaPLen, &(dsa_key->dsaP)) ||
        !we_dcvt_uint16 (obj, &(dsa_key->dsaQLen)) ||
        !we_dcvt_uchar_vector (obj, dsa_key->dsaQLen, &(dsa_key->dsaQ)) ||
        !we_dcvt_uint16 (obj, &(dsa_key->dsaGLen)) ||
        !we_dcvt_uchar_vector (obj, dsa_key->dsaGLen, &(dsa_key->dsaG))) {
      return FALSE;
    }
  }
  else
    return FALSE;
  
  if (!we_dcvt_uint16 (obj, &(p->privKey.bufLen)) ||
      !we_dcvt_uchar_vector (obj, p->privKey.bufLen, &(p->privKey.buf))) {
    return FALSE;
  }
  
  return TRUE;
}

int
we_cvt_crpt_dh_keyexchange_resp (we_dcvt_t *obj, 
                                  we_crpt_dh_keyexchange_resp_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->wid)) ||
      !we_dcvt_int16 (obj, &(p->result)) ||
      !we_dcvt_int16 (obj, &(p->publicValueLen)) ||
      !we_dcvt_uchar_vector (obj, p->publicValueLen, &(p->publicValue)) ||
      !we_dcvt_int16 (obj, &(p->secretLen)) ||
      !we_dcvt_uchar_vector (obj, p->secretLen, &(p->secretValue))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_mime_list(we_dcvt_t *obj, we_mime_list_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->type)))
    return FALSE;

  if (p->type == WE_MIME_TYPE_STR){
    if (!we_dcvt_string (obj, &(p->_u.s_value)))
      return FALSE;
  }
  else {
    if(!we_dcvt_int32 (obj, &(p->_u.i_value)))
      return FALSE;
  }

  return TRUE;
}

int
we_cvt_icon_list(we_dcvt_t *obj, we_icon_list_t *p)
{

  if (!we_dcvt_uint32 (obj, &(p->mime_hash)) ||
      !we_dcvt_string (obj, &(p->s_mime)) ||
      !we_dcvt_int32 (obj, &(p->i_mime)) ||
      !we_dcvt_uint8 (obj, &(p->resource_type)) ||
      !we_dcvt_string (obj, &(p->resource_str_id)) ||
      !we_dcvt_uint32 (obj, &(p->resource_int_id))){
    return FALSE;
  }

  return TRUE;
}

int
we_cvt_get_icons (we_dcvt_t *obj, frw_get_icon_map_table_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->src)) ||
      !we_dcvt_uint32 (obj, &(p->wid)) ||
      !we_dcvt_uint16 (obj, &(p->n_items)) ||
      !we_dcvt_array (obj, sizeof (we_mime_list_t), p->n_items,
                       (void **)&(p->mime_list), (we_dcvt_element_t *)we_cvt_mime_list)) {
    return FALSE;
  }

  return TRUE;
}

int
we_cvt_get_icons_resp (we_dcvt_t *obj, frw_get_icons_resp_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->wid)) ||
      !we_dcvt_uint16 (obj, &(p->n_items)) ||
      !we_dcvt_array (obj, sizeof (we_icon_list_t), p->n_items,
                       (void **)&(p->icon_list), (we_dcvt_element_t *)we_cvt_icon_list)) {

    return FALSE;
  }
  return TRUE;
}

int
we_cvt_object_action(we_dcvt_t *obj, frw_object_action_t *p)
{
  if (!we_cvt_act_content (obj, &(p->content)) ||
      !we_dcvt_int32 (obj, &(p->excl_act_strings_cnt)) ||
      !we_dcvt_array (obj, sizeof (char*), p->excl_act_strings_cnt,
                       (void **)&(p->excl_act_strings), (we_dcvt_element_t *)we_dcvt_string)) {

    return FALSE;
  }
  return TRUE;
}

int
we_cvt_open_dlg_response (we_dcvt_t *obj, we_dlg_open_response_sig_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->wid)) ||
      !we_dcvt_string (obj, &(p->file_name)) ||
      !we_dcvt_int32  (obj, &(p->status))){
    return FALSE;
  }

  return TRUE;
}

int
we_cvt_save_dlg_response (we_dcvt_t *obj, we_dlg_save_response_sig_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->wid)) ||
      !we_dcvt_string (obj, &(p->file_name)) ||
      !we_dcvt_int32  (obj, &(p->status))){
    return FALSE;
  }

  return TRUE;
}

int
we_cvt_save_as_response (we_dcvt_t *obj, we_sas_save_as_response_sig_t *p)
{
  if (!we_dcvt_uint32 (obj, &(p->wid)) ||
      !we_dcvt_string (obj, &(p->file_name)) ||
      !we_dcvt_int32  (obj, &(p->status))){
    return FALSE;
  }

  return TRUE;
}

/**********************************************************************
 * Local functions
 **********************************************************************/
/*
 * Must be the same type as convert_function_t typedef.
 */
static void *
we_convert (WE_UINT8 module, WE_UINT16 signal, void *buffer)
{
  we_dcvt_t  cvt_obj;
  void       *user_data;
  WE_UINT16  length;

  if (buffer == NULL) {
    return NULL;
  }

  user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
  if ((user_data == NULL) || (length == 0)) {
    return NULL;
  }

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, user_data, length, module);

  switch (signal){
  case WE_SIG_MODULE_START:
    {
      we_module_start_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_module_start_t);
      if (!we_cvt_module_start (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_MODULE_TERMINATE:
    {
      we_module_terminate_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_module_terminate_t);
      if (!we_cvt_module_terminate (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_MODULE_STATUS:
    {
      we_module_status_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_module_status_t);
      if (!we_cvt_module_status (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_MODULE_STATUS_NOTIFY:
    {
      we_module_status_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_module_status_t);
      if (!we_cvt_module_status (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_MODULE_EXECUTE_COMMAND:
    {
      we_module_execute_cmd_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_module_execute_cmd_t);
      if (!we_cvt_module_execute_cmd (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_TIMER_SET:
    {
      we_timer_set_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_timer_set_t);
      if (!we_cvt_timer_set (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_TIMER_RESET:
    {
      we_timer_reset_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_timer_reset_t);
      if (!we_cvt_timer_reset (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_TIMER_EXPIRED:
    {
      we_timer_expired_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_timer_expired_t);
      if (!we_cvt_timer_expired (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_WIDGET_NOTIFY:
    {
      we_widget_notify_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_widget_notify_t);
      if (!we_cvt_wid_notify (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_WIDGET_ACTION:
    {
      we_widget_action_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_widget_action_t);
      if (!we_cvt_wid_action (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_WIDGET_USEREVT:
    {
      we_widget_userevt_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_widget_userevt_t);
      if (!we_cvt_wid_user_event (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_WIDGET_ACCESS_KEY:
    {
      we_widget_access_key_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_widget_access_key_t);
      if (!we_cvt_wid_access_key (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_PIPE_NOTIFY:
    {
      we_pipe_notify_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_pipe_notify_t);
      if (!we_cvt_pipe_notify (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_FILE_NOTIFY:
    {
      we_file_notify_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_file_notify_t);
      if (!we_cvt_file_notify (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
    
  case WE_SIG_FILE_PATH_PROPERTY_RESP:
    {
      we_file_path_property_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_file_path_property_resp_t);
      if (!we_cvt_file_path_property_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_FILE_QUOTA_RESP:
    {
      we_file_quota_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_file_quota_resp_t);
      if (!we_cvt_file_quota_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_SOCKET_CONNECT_RESPONSE:
    {
      we_socket_connect_response_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_socket_connect_response_t);
      if (!we_cvt_socket_connect_response (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_SOCKET_NOTIFY:
    {
      we_socket_notify_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_socket_notify_t);
      if (!we_cvt_socket_notify (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_SOCKET_HOST_BY_NAME:
    {
      we_socket_host_by_name_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_socket_host_by_name_t);
      if (!we_cvt_socket_host_by_name (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_REG_DELETE:
  case WE_SIG_REG_GET:
    {
      we_registry_identifier_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_registry_identifier_t);
      if (!we_cvt_registry_identifier (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_REG_SUBSCRIBE:
    {
      we_registry_subscription_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_registry_subscription_t);
      if (!we_cvt_registry_subscription (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_REG_SET:
  case WE_SIG_REG_RESPONSE:
    {
      we_registry_response_t *p;
      p = WE_MEM_ALLOCTYPE (module, we_registry_response_t);
      we_dcvt_int16 (&cvt_obj, &(p->wid));
      p->path = NULL;
      p->buffer_start = p->next_element = WE_MEM_ALLOC (module, length - 2);
      memcpy (p->buffer_start, ((unsigned char*)user_data + 2), length - 2);
      p->buffer_end = p->buffer_start + length - 3;
      return (void *)p;
    }

  case WE_SIG_CONTENT_SEND:
    {
      we_content_send_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_content_send_t);
      if (!we_cvt_content_send (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_CONTENT_SEND_ACK:
    {
      we_content_send_ack_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_content_send_ack_t);
      if (!we_cvt_content_send_ack (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_TEL_MAKE_CALL_RESPONSE:
    {
      we_tel_make_call_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_tel_make_call_resp_t);
      if (!we_cvt_tel_make_call_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_TEL_SEND_DTMF_RESPONSE:
    {
      we_tel_send_dtmf_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_tel_send_dtmf_resp_t);
      if (!we_cvt_tel_send_dtmf_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_PB_ADD_ENTRY_RESPONSE:
    {
      we_pb_add_entry_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_pb_add_entry_resp_t);
      if (!we_cvt_pb_add_entry_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_CRPT_ENCRYPT_PKC_RESPONSE:
    {
      we_crpt_encrypt_pkc_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_crpt_encrypt_pkc_resp_t);
      if (!we_cvt_crpt_encrypt_pkc_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_CRPT_DECRYPT_PKC_RESPONSE:
    {
      we_crpt_decrypt_pkc_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_crpt_decrypt_pkc_resp_t);
      if (!we_cvt_crpt_decrypt_pkc_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_CRPT_VERIFY_SIGN_RESPONSE:
    {
      we_crpt_verify_signature_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_crpt_verify_signature_resp_t);
      if (!we_cvt_crpt_verify_signature_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_CRPT_COMPUTE_SIGN_RESPONSE:
    {
      we_crpt_compute_signature_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_crpt_compute_signature_resp_t);
      if (!we_cvt_crpt_compute_signature_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_CRPT_GENERATE_KEY_RESPONSE:
    {
      we_crpt_generate_keypair_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_crpt_generate_keypair_resp_t);
      if (!we_cvt_crpt_generate_keypair_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_CRPT_DH_KEYEXCHANGE_RESPONSE:
    {
      we_crpt_dh_keyexchange_resp_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_crpt_dh_keyexchange_resp_t);
      if (!we_cvt_crpt_dh_keyexchange_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_GET_ICONS:
    {
      frw_get_icon_map_table_t *p;
      
      p = WE_MEM_ALLOCTYPE (module, frw_get_icon_map_table_t);
      if (!we_cvt_get_icons (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_GET_ICONS_RESPONSE:
    {
      frw_get_icons_resp_t *p;
      
      p = WE_MEM_ALLOCTYPE (module, frw_get_icons_resp_t);
      if (!we_cvt_get_icons_resp (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SIG_OBJECT_ACTION:
    {
      frw_object_action_t *p;
      
      p = WE_MEM_ALLOCTYPE (module, frw_object_action_t);
      if (!we_cvt_object_action (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_DLG_SAVE_RESPONSE_SIG:
    {
      we_dlg_save_response_sig_t *p;
      
      p = WE_MEM_ALLOCTYPE (module, we_dlg_save_response_sig_t);
      if (!we_cvt_save_dlg_response (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_DLG_OPEN_RESPONSE_SIG:
    {
      we_dlg_open_response_sig_t *p;
      
      p = WE_MEM_ALLOCTYPE (module, we_dlg_open_response_sig_t);
      if (!we_cvt_open_dlg_response (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  case WE_SAS_SAVE_AS_RESPONSE_SIG:
    {
      we_sas_save_as_response_sig_t *p;
      
      p = WE_MEM_ALLOCTYPE (module, we_sas_save_as_response_sig_t);
      if (!we_cvt_save_as_response (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_TIMER_SET_PERSISTENT:
    {
      we_timer_set_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_timer_set_t);
      if (!we_cvt_timer_set (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_TIMER_RESET_PERSISTENT:
    {
      we_timer_reset_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_timer_reset_t);
      if (!we_cvt_timer_reset (&cvt_obj, p)) {
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }

  case WE_SIG_TIMER_PERSISTENT_EXPIRED:
    {
      we_timer_expired_t *p;

      p = WE_MEM_ALLOCTYPE (module, we_timer_expired_t);
      if (!we_cvt_timer_expired (&cvt_obj, p)) { 
        WE_MEM_FREE (module, p);
        break;
      }
      return (void *)p;
    }
  default:
    break;
  }

  return we_ess_convert (module, signal, buffer);
}

/*
 * Must be the same type as destruct_function_t typedef.
 */
static void
we_destruct (WE_UINT8 module, WE_UINT16 signal, void* p)
{
  switch (signal){
  case WE_SIG_MODULE_START:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_module_start (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_MODULE_TERMINATE:
    WE_MEM_FREE (module, p);
    break;
  case WE_SIG_MODULE_STATUS:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_module_status (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_MODULE_STATUS_NOTIFY:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_module_status (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_MODULE_EXECUTE_COMMAND:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_module_execute_cmd (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_TIMER_SET:
  case WE_SIG_TIMER_RESET:
  case WE_SIG_TIMER_EXPIRED:
  case WE_SIG_WIDGET_NOTIFY:
  case WE_SIG_WIDGET_ACTION:
  case WE_SIG_WIDGET_USEREVT:
  case WE_SIG_WIDGET_ACCESS_KEY:
  case WE_SIG_PIPE_NOTIFY:
  case WE_SIG_FILE_NOTIFY:
  case WE_SIG_SOCKET_CONNECT_RESPONSE:
  case WE_SIG_SOCKET_NOTIFY:
  case WE_SIG_SOCKET_HOST_BY_NAME:
  case WE_SIG_CONTENT_SEND_ACK:
  case WE_SIG_TEL_MAKE_CALL_RESPONSE:
  case WE_SIG_TEL_SEND_DTMF_RESPONSE:
  case WE_SIG_PB_ADD_ENTRY_RESPONSE:
  case WE_SIG_GET_ICONS:
  case WE_SIG_OBJECT_ACTION:
  case WE_SIG_TIMER_SET_PERSISTENT:
  case WE_SIG_TIMER_RESET_PERSISTENT:
  case WE_SIG_TIMER_PERSISTENT_EXPIRED:

    WE_MEM_FREE (module, p);
    break;


  case WE_SIG_FILE_PATH_PROPERTY_RESP:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_file_path_property_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_FILE_QUOTA_RESP:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_file_quota_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_CRPT_ENCRYPT_PKC_RESPONSE:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_crpt_encrypt_pkc_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_CRPT_DECRYPT_PKC_RESPONSE:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_crpt_decrypt_pkc_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_CRPT_VERIFY_SIGN_RESPONSE:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_crpt_verify_signature_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_CRPT_COMPUTE_SIGN_RESPONSE:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_crpt_compute_signature_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_CRPT_GENERATE_KEY_RESPONSE:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_crpt_generate_keypair_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_CRPT_DH_KEYEXCHANGE_RESPONSE:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_crpt_dh_keyexchange_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
  case WE_SIG_REG_DELETE:
  case WE_SIG_REG_GET:
    {
      we_registry_identifier_t *q = (we_registry_identifier_t*)p;

      WE_MEM_FREE (module, q->path);
      WE_MEM_FREE (module, q->key);
      WE_MEM_FREE (module, q);
      break;
    }
  case WE_SIG_REG_SET:
  case WE_SIG_REG_RESPONSE:
    {
      we_registry_response_t *q = (we_registry_response_t*)p;

      WE_MEM_FREE (module, q->buffer_start);
      WE_MEM_FREE (module, q);
      break;
    }
  case WE_SIG_CONTENT_SEND:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_content_send (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
   case WE_SIG_GET_ICONS_RESPONSE:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_get_icons_resp (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;

    }
   case WE_DLG_SAVE_RESPONSE_SIG:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_save_dlg_response (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }

   case WE_DLG_OPEN_RESPONSE_SIG:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_open_dlg_response (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
   case WE_SAS_SAVE_AS_RESPONSE_SIG:
    {
      we_dcvt_t cvt_obj;
      if (p == NULL)
        return;
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
      we_cvt_save_as_response (&cvt_obj, p);
      WE_MEM_FREE (module, p);
      break;
    }
   default :
    {
      we_ess_destruct (module, signal, p);
      break;
    }
  }
}

static convert_function_t*
we_get_converter (int module)
{
  if (module == WE_MODID_FRW )
    return (convert_function_t*)we_convert;
  else
    return we_convert_func[module];
}

static destruct_function_t*
we_get_destructor (int module)
{
  if (module == WE_MODID_FRW )
    return (destruct_function_t*)we_destruct;
  else
    return we_destruct_func[module];
}


/**********************************************************************
 * Exported messaging functions:
 **********************************************************************/

void
we_signal_reg_functions (WE_UINT8 module, convert_function_t *cf,
                                            destruct_function_t *df)
{
  we_convert_func[module] = cf;
  we_destruct_func[module] = df;
}


void*
we_signal_create (WE_UINT16 sigUid, WE_UINT8 srcModId, WE_UINT8 destModId,
                    WE_UINT16 userDataLength)
{
  we_dcvt_t  cvt_obj;
  void*       p;
  WE_UINT16  length;

  length = (WE_UINT16)(WE_SIGNAL_HEADER_LENGTH + userDataLength);

  p = TPIa_SignalAllocMemory( length );

  if (p != NULL){
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, p, length, 0);
    we_dcvt_uint16 (&cvt_obj, &sigUid);
    we_dcvt_uint8 (&cvt_obj, &srcModId);
    we_dcvt_uint8 (&cvt_obj, &destModId);
    we_dcvt_uint16 (&cvt_obj, &userDataLength);

    return p;
  }
  WE_ERROR (srcModId, WE_ERR_SYSTEM_FATAL);
  return NULL;
}

void
we_signal_send (void* buffer)
{
  we_dcvt_t  cvt_obj;
  WE_UINT16  length;
  WE_UINT8   srcModId, destModId;

  /* Note: If changing in this function, you must also change
     in function frw_send_signal_to_external in file Frw.c. */
  if (buffer != NULL){
      /* buffer + 2: step over signal */
      /* 2 bytes -> UINT16 */
    we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, (char*)buffer + 2, 4, 0);
    we_dcvt_uint8 (&cvt_obj, &srcModId);
    we_dcvt_uint8 (&cvt_obj, &destModId);
    we_dcvt_uint16 (&cvt_obj, &length);
    if (TPIa_SignalSend (destModId, buffer, (WE_UINT16)(WE_SIGNAL_HEADER_LENGTH + length)) == TPI_SIGNAL_ERROR_QUEUE_FULL)
      WE_ERROR (srcModId, WE_ERR_SYSTEM_FATAL);
  }
}

int
we_signal_retrieve (WE_UINT8 modId, receive_function_t *rf)
{
  convert_function_t* converter;
  WE_UINT16          signal;
  void                *p, *sig_struct;

  p = TPIa_SignalRetrieve (modId);
  if (p != NULL){
    signal = WE_SIGNAL_GET_SIGNAL_ID (p);
    converter = we_get_converter (WE_GET_SIGNAL_OWNER (signal));
    sig_struct = converter (modId, signal, p);

    rf (WE_SIGNAL_GET_SOURCE (p), signal, sig_struct);
    TPIa_SignalFreeMemory (p);
    return 1;
  }
  return 0;
}

void
we_signal_destruct (WE_UINT8 module, WE_UINT16 signal, void* p)
{
  destruct_function_t* destructor;

  if (p != NULL){
    destructor = we_get_destructor (WE_GET_SIGNAL_OWNER (signal));
    destructor (module, signal, p);
  }
}

WE_UINT16
we_signal_get_signal_id (void* buffer)
{
  we_dcvt_t  cvt_obj;
  WE_UINT16  sig_id;

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, buffer, WE_SIGNAL_HEADER_LENGTH, 0);
  we_dcvt_uint16 (&cvt_obj, &sig_id);
  return sig_id;
}

WE_UINT8
we_signal_get_source (void* buffer)
{
  we_dcvt_t  cvt_obj;
  WE_UINT8  source;

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, buffer, WE_SIGNAL_HEADER_LENGTH, 0);
  we_dcvt_change_pos (&cvt_obj, 2); /* Step over signal. */
  we_dcvt_uint8 (&cvt_obj, &source);
  return source;
}

WE_UINT8
we_signal_get_destination (void* buffer)
{
  we_dcvt_t  cvt_obj;
  WE_UINT8   dest;

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, buffer, WE_SIGNAL_HEADER_LENGTH, 0);
  we_dcvt_change_pos (&cvt_obj, 3); /* Step over signal and source. */
  we_dcvt_uint8 (&cvt_obj, &dest);
  return dest;
}

void*
we_signal_get_user_data (void* buffer, WE_UINT16* userDataLength)
{
  we_dcvt_t  cvt_obj;

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, buffer, WE_SIGNAL_HEADER_LENGTH, 0);
  we_dcvt_change_pos (&cvt_obj, 4); /* Step over signal, source and destination. */
  we_dcvt_uint16 (&cvt_obj, userDataLength);
  return (char*)buffer + cvt_obj.pos;
}


/**********************************************************************
 * Exported timer functions:
 **********************************************************************/

void
we_timer_set (WE_UINT8 modId, int timerID, WE_UINT32 msInterval)
{
  we_timer_set_t timer;
  we_dcvt_t      cvt_obj;
  void            *buffer, *user_data;
  WE_UINT16      length;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId,
                "WE: WE_TIMER_SET timerID=%d, msInterval=%u\n",
                timerID, msInterval));

  timer.timerID = timerID;
  timer.timerInterval = msInterval;
  buffer = WE_SIGNAL_CREATE (WE_SIG_TIMER_SET, modId, WE_MODID_FRW, 8);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, modId);
    we_cvt_timer_set (&cvt_obj, &timer);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_timer_reset (WE_UINT8 modId, int timerID)
{
  we_timer_reset_t timer;
  we_dcvt_t        cvt_obj;
  void              *buffer, *user_data;
  WE_UINT16        length;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId,
                "WE: WE_TIMER_RESET timerID=%d\n", timerID));

  timer.timerID = timerID;
  buffer = WE_SIGNAL_CREATE (WE_SIG_TIMER_RESET, modId, WE_MODID_FRW, 4);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, modId);
    we_cvt_timer_reset (&cvt_obj, &timer);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_timer_set_persistent (WE_UINT8 modId, int timerID, WE_UINT32 date_time)
{
  we_timer_set_t timer;
  we_dcvt_t      cvt_obj;
  void            *buffer, *user_data;
  WE_UINT16      length;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId,
                "WE: WE_TIMER_SET_PERSISTENT timerID=%d, date_time=%u\n",
                timerID, date_time));

  timer.timerID = timerID;
  timer.timerInterval = date_time;
  buffer = WE_SIGNAL_CREATE (WE_SIG_TIMER_SET_PERSISTENT, modId, WE_MODID_FRW, 8);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, modId);
    we_cvt_timer_set (&cvt_obj, &timer);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_timer_reset_persistent (WE_UINT8 modId, int timerID)
{
  we_timer_reset_t timer;
  we_dcvt_t        cvt_obj;
  void              *buffer, *user_data;
  WE_UINT16        length;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId,
                "WE: WE_TIMER_RESET_PERSISTENT timerID=%d\n", timerID));

  timer.timerID = timerID;
  buffer = WE_SIGNAL_CREATE (WE_SIG_TIMER_RESET_PERSISTENT, modId, WE_MODID_FRW, 4);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, modId);
    we_cvt_timer_reset (&cvt_obj, &timer);
    WE_SIGNAL_SEND (buffer);
  }
}
/**********************************************************************
 * Exported module functions:
 **********************************************************************/

void
we_module_is_created (WE_UINT8 module_src, char* modVersion)
{
  we_module_status_t   status;
  we_dcvt_t            cvt_obj;
  void                  *buffer, *user_data;
  WE_UINT16            length;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, module_src, "WE: WE_MODULE_IS_CREATED\n"));

  status.modId = module_src;
  status.status = MODULE_STATUS_CREATED;
  status.modVersion = modVersion;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_module_status (&cvt_obj, &status);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_STATUS_NOTIFY, module_src, 
                                    WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, module_src);
    we_cvt_module_status (&cvt_obj, &status);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_module_is_active (WE_UINT8 module_src)
{
  we_module_status_t   status;
  we_dcvt_t            cvt_obj;
  void                  *buffer, *user_data;
  WE_UINT16            length;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, module_src, "WE: WE_MODULE_IS_ACTIVE\n"));

  status.modId = module_src;
  status.status = MODULE_STATUS_ACTIVE;
  status.modVersion = NULL;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_module_status (&cvt_obj, &status);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_STATUS_NOTIFY, module_src, 
                                    WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, module_src);
    we_cvt_module_status (&cvt_obj, &status);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_module_is_terminated (WE_UINT8 module_src)
{
  we_module_status_t   status;
  we_dcvt_t            cvt_obj;
  void                  *buffer, *user_data;
  WE_UINT16            length;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, module_src, "WE: WE_MODULE_IS_TERMINATED\n"));
  
  status.modId = module_src;
  status.status = MODULE_STATUS_TERMINATED;
  status.modVersion = NULL;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_module_status (&cvt_obj, &status);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_STATUS_NOTIFY, module_src, 
                                    WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, module_src);
    we_cvt_module_status (&cvt_obj, &status);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_module_start (WE_UINT8 module_src, WE_UINT8 module, char* startOptions,
                  we_content_data_t* contentData, void* contentArea)
{
  we_module_start_t  start;
  we_dcvt_t          cvt_obj;
  WE_UINT16          length;
  void                *buffer, *user_data;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, module_src,
                "WE: WE_MODULE_START module=%d startOptions=%s\n",
                module, ((startOptions != NULL) ? startOptions:"(null)")));

  start.modId = module;
  start.startOptions = startOptions;
  if (contentData == NULL) {
    start.contentDataExists = 0;
    start.contentData = NULL;
  }
  else {
    start.contentDataExists = 1;
    start.contentData = contentData;
  }
  contentArea = contentArea;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_module_start (&cvt_obj, &start);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_START, module_src, 
                                    WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, module_src);
    we_cvt_module_start (&cvt_obj, &start);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_module_terminate (WE_UINT8 module_src, WE_UINT8 module)
{
  we_module_terminate_t  term;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *buffer, *user_data;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, module_src,
                "WE: WE_MODULE_TERMINATE module=%d\n", module));

  term.modId = module;
  buffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_TERMINATE, module_src, 
                                    WE_MODID_FRW, 1);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, module_src);
    we_cvt_module_terminate (&cvt_obj, &term);
    WE_SIGNAL_SEND (buffer);
  }
}


/**********************************************************************
 * Registry type definitions used with the set command.
 **********************************************************************/

typedef struct we_reg_set_element_st {
  struct we_reg_set_element_st *next;
  WE_UINT8                     type;
  char*                         name;
  WE_INT32                     value_i;
  unsigned char*                value_bv;
} we_reg_set_element_t;

typedef struct {
  WE_UINT8             modId;
  int                   buff_len;
  we_reg_set_element_t *start_element;
  we_reg_set_element_t *end_element;
} we_reg_set_head_t;


/**********************************************************************
 * Local registry functions:
 **********************************************************************/
/* Add an element (path or key) to the set-structure. */
static void
we_registry_set_add_element (we_reg_set_head_t* head, WE_UINT8 type, char* name,
                              WE_INT32 value_i, unsigned char* value_bv)
{
  we_reg_set_element_t*  element;
    /* Ignore the element if the first element is a key,
       or if both the last element and the new element is a path.*/
  if ((head->start_element == NULL && type != WE_REG_SET_TYPE_PATH) ||
      (head->end_element != NULL && head->end_element->type == WE_REG_SET_TYPE_PATH
      && type == WE_REG_SET_TYPE_PATH))
    return;

  element = WE_MEM_ALLOCTYPE (head->modId, we_reg_set_element_t);
  if (element == NULL)
    return;
  element->type = type;
  element->name = we_cmmn_strdup (head->modId, name);
  element->value_i = value_i;
  if (type == WE_REG_SET_TYPE_STR && value_i > 0) {
    element->value_bv = WE_MEM_ALLOC (head->modId, value_i);
    if (element->value_bv != NULL)
      memcpy (element->value_bv, value_bv, value_i);
  }
  else
    element->value_bv = NULL;
  element->next = NULL;
    /* Add the new element last in the list. */
  if (head->start_element == NULL)
    head->start_element = head->end_element = element;
  else {
    head->end_element->next = element;
    head->end_element = element;
  }
    /* Increase the buffert counter. */
  head->buff_len += strlen (name) + 2; /* 2 = 1 byte for the type of element, and 1 byte
                                                for the null byte to the name */
  if (type == WE_REG_SET_TYPE_INT)
    head->buff_len += 4;               /* 4 = the number of bytes to hold a WE_INT32 value. */
  else if (type == WE_REG_SET_TYPE_STR)
    head->buff_len += 2 + value_i;     /* 2 = the number of bytes to hold the
                                              length (WE_UINT16) of the string. */
}

static void
we_registry_set_free_element (WE_UINT8 modId, we_reg_set_element_t* element)
{
  WE_MEM_FREE (modId, element->name);
  WE_MEM_FREE (modId, element->value_bv);
  WE_MEM_FREE (modId, element);
}

/* Go through the set-structure, create a buffer and send the buffer.
 * If the call to this function orignats from FRWc_registrySetCommit
 * from_frw_api is set TRUE. In that case the buffer is not send by
 * the external signal system, the buffer is returned to the calling
 * function.
 *
 * The signal structure of WE_SIG_REG_SET and WE_SIG_REG_RESPONSE is:
 * <wid> followed by a sequence of following fields: <type><name><value>
 *
 * The different type of fields:
 * Path:    <path_type><name><00>
 * Key_int: <key_int_type><name><00><4 bytes value>
 * Key_str: <key_str_type><name><00><string length><the string>
 */
static void*
we_registry_set_commit_core (void* handle, int from_frw_api)
{
  we_reg_set_head_t*     head = (we_reg_set_head_t*)handle;
  we_reg_set_element_t   *element, *tmp_element;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *buffer, *user_data;

  if (head == NULL)
    return NULL;

  if (head->buff_len == 0) {
    WE_MEM_FREE (head->modId, head);
    return NULL;
  }
    /* Ignore this set operation if only one element (path or key) is in the set-structure. */
  if (head->start_element != NULL && head->start_element == head->end_element) {
    we_registry_set_free_element (head->modId, head->start_element);
    WE_MEM_FREE (head->modId, head);
    return NULL;
  }
    /* Ignore the last element, if it is a path. Decrease the buffer counter. */
  if (head->end_element->type == WE_REG_SET_TYPE_PATH)
    head->buff_len -= strlen (head->end_element->name) + 2; /* 2 = 1 byte for the type of element,
                                                            and 1 byte for the null byte to the name */

  length = (WE_UINT16)(head->buff_len);
  element = head->start_element;

  buffer = user_data = NULL;
    /* Create an external signal if it is a module that is calling. */
  if (from_frw_api == FALSE)
    buffer = WE_SIGNAL_CREATE (WE_SIG_REG_SET, head->modId, WE_MODID_FRW, (WE_UINT16)(length + 2));
              /* 2 = wid parameter. WE_SIG_REG_SET and WE_SIG_REG_RESPONSE have the same signal structure,
                 the wid parameter in not used in this case when we send WE_SIG_REG_SET. */

  if (buffer != NULL || from_frw_api == TRUE){
    if (from_frw_api == FALSE)
      user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    else {
        /* If the call to this function orignats from FRWc_registrySetCommit, the user_data came from a normal malloc. */
      user_data = WE_MEM_ALLOC (WE_MODID_FRW, length);
      if (user_data == NULL)
        return NULL;
    }
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, head->modId);
    if (from_frw_api == FALSE)
      cvt_obj.pos += 2;   /* WE_SIG_REG_SET and WE_SIG_REG_RESPONSE have the same signal structure,
                             so we step over the wid parameter in this case when we send WE_SIG_REG_SET. */
    while (element != NULL) {
      we_dcvt_uint8 (&cvt_obj, &(element->type));
      we_dcvt_string (&cvt_obj, &(element->name));
      if (element->type == WE_REG_SET_TYPE_INT)
        we_dcvt_int32 (&cvt_obj, &(element->value_i));
      else if (element->type == WE_REG_SET_TYPE_STR) {
        WE_UINT16  tmp = (WE_UINT16)(element->value_i);
        we_dcvt_uint16 (&cvt_obj, &(tmp));
        we_dcvt_uchar_vector (&cvt_obj, element->value_i, &(element->value_bv));
      }
      tmp_element = element;
      element = element->next;
      we_registry_set_free_element (head->modId, tmp_element);
    }
  }
  WE_MEM_FREE (head->modId, head);
  if (from_frw_api == FALSE) {
      /* Send the external signal if it is a module that is calling. */
    if (buffer != NULL)
      WE_SIGNAL_SEND (buffer);
    return NULL;
  }
  else {
      /* Return a pointer if the call to this function orignats from FRWc_registrySetCommit. */
    we_registry_response_t *p = WE_MEM_ALLOCTYPE (WE_MODID_FRW, we_registry_response_t);

    if (p != NULL) {
      p->wid = 0;
      p->path = NULL;
      p->buffer_start = p->next_element = user_data;
      p->buffer_end = p->buffer_start + length - 1;
    }
    return (void*)p;
  }
}


/**********************************************************************
 * Exported registry functions:
 **********************************************************************/

void*
we_registry_set_init (WE_UINT8 modId)
{
  we_reg_set_head_t* head;

  head = WE_MEM_ALLOCTYPE (modId, we_reg_set_head_t);
  if (head == NULL)
    return NULL;
  head->modId = modId;
  head->buff_len = 0;
  head->start_element = NULL;
  head->end_element = NULL;

  return (void*)head;
}

void
we_registry_set_path (void* handle, char* path)
{
  if (handle == NULL || path == NULL)
    return;
  we_registry_set_add_element ((we_reg_set_head_t*)handle, WE_REG_SET_TYPE_PATH,
                                path, 0, NULL);
}

void
we_registry_set_add_key_int (void* handle, char* key, WE_INT32 value)
{
  if (handle == NULL || key == NULL)
    return;
  we_registry_set_add_element ((we_reg_set_head_t*)handle, WE_REG_SET_TYPE_INT,
                                key, value, NULL);
}

void
we_registry_set_add_key_str (void* handle, char* key, unsigned char* value,
                              WE_UINT16 valueLength)
{
  if (handle == NULL || key == NULL)
    return;
  we_registry_set_add_element ((we_reg_set_head_t*)handle, WE_REG_SET_TYPE_STR,
                                key, valueLength, value);
}

void
we_registry_set_commit (void* handle)
{
  we_registry_set_commit_core (handle, FALSE);
}

void
we_registry_get (WE_UINT8 modId, int wid, char* path, char* key)
{
  we_registry_identifier_t   reg;
  we_dcvt_t                  cvt_obj;
  WE_UINT16                  length;
  void                        *buffer, *user_data;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId,
    "WE: WE_SIG_REG_GET path=%s, key=%s\n", path, ((key != NULL) ? key:"(null)")));

  reg.wid = (WE_INT16)wid;
  reg.path = path;
  reg.key = key;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_registry_identifier (&cvt_obj, &reg);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_REG_GET, modId, WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, modId);
    we_cvt_registry_identifier (&cvt_obj, &reg);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_registry_delete (WE_UINT8 modId, char* path, char* key)
{
  we_registry_identifier_t   reg;
  we_dcvt_t                  cvt_obj;
  WE_UINT16                  length;
  void                        *buffer, *user_data;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId,
                "WE: WE_SIG_REG_DELETE path=%s, key=%s\n", path, key));

  reg.wid = 0;
  reg.path = path;
  reg.key = key;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_registry_identifier (&cvt_obj, &reg);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_REG_DELETE, modId, WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, modId);
    we_cvt_registry_identifier (&cvt_obj, &reg);
    WE_SIGNAL_SEND (buffer);
  }
}

void
we_registry_subscribe (WE_UINT8 modId, int wid, char* path, char* key, WE_BOOL add)
{
  we_registry_subscription_t reg;
  we_dcvt_t                  cvt_obj;
  WE_UINT16                  length;
  void                        *buffer, *user_data;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId,
                "WE: WE_SIG_REG_SUBSCRIBE modId=%d, wid=%d, path=%s, key=%s, add=%d \n",
                modId, wid, path?path:"null", key?key:"null", add));

  reg.wid = (WE_INT16) wid;
  reg.path = path;
  reg.key = key;
  reg.add = add;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_registry_subscription (&cvt_obj, &reg);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_REG_SUBSCRIBE, modId, WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, modId);
    we_cvt_registry_subscription (&cvt_obj, &reg);
    WE_SIGNAL_SEND (buffer);
  }
}

int
we_registry_response_get_next (we_registry_response_t* handle, we_registry_param_t* param)
{
  we_dcvt_t              cvt_obj;
  WE_UINT8               type;

  do {
    if (handle == NULL || handle->buffer_end == handle->next_element)
      return FALSE;

    we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, handle->next_element,
                  (handle->buffer_end - handle->next_element + 1), 0);
    we_dcvt_uint8 (&cvt_obj, &type);
    if (type == WE_REG_SET_TYPE_PATH) {
      handle->path = (char*)(handle->next_element + cvt_obj.pos);
      cvt_obj.pos += strlen (handle->path) + 1;
      we_dcvt_uint8 (&cvt_obj, &type);
    }
    if (type != WE_REG_SET_TYPE_INT && type != WE_REG_SET_TYPE_STR && type != WE_REG_SET_TYPE_DELETED)
      return FALSE;

    param->path = handle->path;
    param->key = (char*)(handle->next_element + cvt_obj.pos);
    cvt_obj.pos += strlen (param->key) + 1;

    if (type == WE_REG_SET_TYPE_INT) {
      param->type = WE_REGISTRY_TYPE_INT;
      we_dcvt_int32 (&cvt_obj, &(param->value_i));
      param->value_bv = NULL;
      param->value_bv_length = 0;
    }
    else if (type == WE_REG_SET_TYPE_STR){
      param->type = WE_REGISTRY_TYPE_STR;
      we_dcvt_uint16 (&cvt_obj, &(param->value_bv_length));
      if (param->value_bv_length != 0)
        param->value_bv = handle->next_element + cvt_obj.pos;
      else
        param->value_bv = NULL;
      cvt_obj.pos += param->value_bv_length;
      param->value_i = 0;
    }
    else {
      param->type = WE_REGISTRY_TYPE_DELETED;
      param->value_bv = NULL;
      param->value_bv_length = 0;
      param->value_i = 0;
    }

    handle->next_element = MIN ((handle->next_element + cvt_obj.pos), handle->buffer_end);
  } while (type & 0x80);
  return TRUE;
}

void
we_registry_response_free (WE_UINT8 modId, we_registry_response_t* handle)
{
  if (handle == NULL)
    return;

  WE_MEM_FREE (modId, handle->buffer_start);
  WE_MEM_FREE (modId, handle);
}


/**********************************************************************
 * Exported content routing functions:
 **********************************************************************/

void
we_content_send (WE_UINT8 src_modId, WE_UINT8 dest_modId, char* startOptions,
                  we_content_data_t* contentData, int sendAck, WE_UINT8 wid, int useObjectAction)
{
  we_content_send_t   send;
  we_dcvt_t           cvt_obj;
  WE_UINT16           length;
  void                 *buffer, *user_data;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, src_modId,
                "WE: WE_SIG_CONTENT_SEND dest_modId=%d, sendAck=%d, wid=%d, startOptions=%s, contentData=%s\n",
                dest_modId, sendAck, wid, startOptions, contentData ? "(non-null)" : "(null)"));

  send.destModId = dest_modId;
  send.sendAck = (WE_UINT8)(((sendAck >= 0) && (sendAck <= 2)) ? sendAck : 1);
  send.wid = wid;
  send.useObjectAction = (WE_UINT8) useObjectAction;
  send.startOptions = startOptions;
  if (contentData == NULL) {
    send.contentDataExists = 0;
    send.contentData = NULL;
  }
  else {
    send.contentDataExists = 1;
    send.contentData = contentData;

    if (send.contentData->embeddedObjectData != NULL)
      contentData->embeddedObjectExists = 1;
    else
      contentData->embeddedObjectExists = 0;
  }

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_content_send (&cvt_obj, &send);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_CONTENT_SEND, src_modId, WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, src_modId);
    we_cvt_content_send (&cvt_obj, &send);
    WE_SIGNAL_SEND (buffer);
  }
}


/****************************************
 * Only used internally:
 ****************************************/

void*
we_registry_set_int_commit (void* handle)
{
  return we_registry_set_commit_core (handle, TRUE);
}

void
we_registry_set_free_handle (WE_UINT8 modId, void* handle)
{
  we_reg_set_head_t*     head = (we_reg_set_head_t*)handle;
  we_reg_set_element_t   *element, *tmp_element;

  if (head == NULL)
    return;

  if (head->buff_len == 0) {
    WE_MEM_FREE (modId, head);
    return;
  }

  element = head->start_element;
  while (element != NULL) {
    tmp_element = element;
    element = element->next;
    we_registry_set_free_element (modId, tmp_element);
  }
  WE_MEM_FREE (modId, head);
}

/**********************************************************************
 * Exported functions:
 **********************************************************************/

void
we_get_icons (WE_UINT8 src_modId, WE_UINT32 wid, 
               WE_UINT16 n_items, we_mime_list_t mime_list[])
{
  frw_get_icon_map_table_t   *p;
  we_dcvt_t                 cvt_obj;
  WE_UINT16                 length;
  void                      *buffer, *user_data;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, src_modId,
                "WE: WE_SIG_GET_ICON_TABLE src_modId=%d, wid=%d", src_modId, wid));

  p = WE_MEM_ALLOC (src_modId, sizeof (frw_get_icon_map_table_t));
  p->src = src_modId;
  p->wid  = wid;
  p->n_items = n_items;
  p->mime_list = mime_list;


  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_get_icons (&cvt_obj, p);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_GET_ICONS, src_modId, WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, src_modId);
    we_cvt_get_icons (&cvt_obj, p);
    WE_SIGNAL_SEND (buffer);
  }
  WE_MEM_FREE (src_modId, p);
}

void
we_object_action(WE_UINT8                src_modId, 
                  const we_act_content_t* content,
                  const char*              excl_act_strings[],
                  WE_INT32                excl_act_strings_cnt)
{
  frw_object_action_t  msg;
  we_dcvt_t           cvt_obj;
  WE_UINT16           length;
  void                *buffer, *user_data;

  if (!content)
    return;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, src_modId,
                "WE: WE_SIG_OBJECT_ACTION mime_type=%s, data_type=%d, &data=%d, data_len=%d"
                "src_path=%s, default_name=%s, excl_act_strings_cnt=%d",
                content->mime_type?content->mime_type:"", content->data_type, 
                content->data, content->data_len,
                content->src_path?content->src_path:"",
                content->default_name?content->default_name:"",
                excl_act_strings_cnt));

  msg.content.mime_type = (char*) content->mime_type;
  msg.content.data_type = content->data_type;
  msg.content.data      = (unsigned char*) content->data;
  msg.content.data_len  = content->data_len;
  msg.content.src_path  = (char*) content->src_path;
  msg.content.default_name = (char*) content->default_name;
  msg.content.content_type = (char*) content->content_type;
  msg.excl_act_strings_cnt = excl_act_strings ? excl_act_strings_cnt : 0;
  msg.excl_act_strings = (char**) excl_act_strings;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_object_action (&cvt_obj, &msg);
  length = (WE_UINT16)cvt_obj.pos;

  buffer = WE_SIGNAL_CREATE (WE_SIG_OBJECT_ACTION, src_modId, WE_MODID_FRW, length);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, src_modId);
    we_cvt_object_action (&cvt_obj, &msg);
    WE_SIGNAL_SEND (buffer);
  }
}

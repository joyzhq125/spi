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








 
#include "we_core.h"
#include "we_cmmn.h"
#include "we_lib.h"
#include "we_url.h"
#include "we_chrs.h"
#include "we_log.h"
#include "we_hdr.h"

#include "stk_if.h"
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
#include "ubs_if.h"
#endif

#include "Pus_Main.h"
#include "Pus_Conn.h"
#include "Pus_Msg.h"
#include "Pus_Prh.h"
#include "Pus_Sig.h"
#include "Pus_Def.h"
#include "Pus_Cfg.h"
#include "Pus_Sia.h"
#ifdef PUS_CONFIG_HTTP_PUSH
#include "Pus_Http.h"
#endif
#include "Pus_Cmmn.h"










#define PUS_URI_IS_AUTHENTICATED                 0x01
#define PUS_CONTENT_IS_TRUSTED                   0x02
#define PUS_LAST_PUSH_FLAG                       0x04




#define PUS_ABORT_USER_REFUSE                 0xEB

  
#define PUS_ABORT_DESTINATION_REFUSE          0xEC


#define PUS_ABORT_RESOURCE_REFUSE             0xED


#define PUS_ABORT_CONTENT_TYPE_REFUSE         0xEE

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 

#define PUS_NUMBER_OF_PROP_FOR_UBS            9


#define PUS_FOLDER_NAME                      "i"
#endif










static void
pus_handle_push_message (int wid);

static int
pus_check_if_handled_by_pus (char *application_id, WE_UINT32 content_type_int);

static void
pus_send_push_to_content_router (stk_content_t* content, char* hdrs, int routing_type, 
                                 char* routing_identifier, int wid, int net_id);

static void
pus_get_push_headers (char *headers, char **application_id, char **content_type_string,
                      WE_UINT32 *content_type_int, char **content_uri, 
                      char **content_location, char **initiator_uri, 
                      WE_UINT8 *push_flag, int *charset);

static int
pus_check_push_security_level (int routing_type, WE_UINT8 push_flag);

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
static void
pus_handle_parsed_msg (Pus_Msg_list_t *msg);

static char*
Pus_Msg_create_random_id (void);

static void
Pus_Msg_store_msg_in_ubs (Pus_Msg_list_t *msg, int type);

static void
pus_handle_parsing_completed (int msg_id);
#endif

static void
Pus_Msg_handle_dialog_response (Pus_Dlg_t* dlg, int type);

static char*      
Pus_Msg_add_address_to_whitelist (char* list, const char *key, char* address);

static char*
Pus_Msg_cvt_smsc_address_to_string (we_sockaddr_t fromAddr);

static char*
Pus_Msg_cvt_ip_address_to_string (we_sockaddr_t fromAddr);




#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
static void
Pus_Msg_create_data_for_ubs (ubs_full_msg_t *data, Pus_Msg_list_t* msg, int type);

static ubs_msg_handle_t
Pus_Msg_create_ubs_handle (unsigned char* value);

static void 
Pus_Msg_set_ubs_UTF8_value (ubs_key_value_t *result, WE_UINT16 key, 
                            const unsigned char *value);

static void 
Pus_Msg_set_ubs_uint32_value(ubs_key_value_t *result,
                             WE_UINT16 key,
                             WE_UINT32 value);

static void 
Pus_Msg_set_ubs_bool_value(ubs_key_value_t *result,
                           WE_UINT16 key,
                           WE_BOOL value);

static WE_UINT32
Pus_Msg_get_properity_uint32 (ubs_key_value_t* propertyList, 
                              WE_UINT16 list_len,
                              WE_UINT16 key);

static WE_UINT32
Pus_Msg_get_properity_bool (ubs_key_value_t* propertyList, 
                            WE_UINT16 list_len,
                            WE_UINT16 key);

static void
Pus_Msg_free_key_value (ubs_key_value_t *p);

static void
Pus_Msg_free_key_value_array (ubs_key_value_t **p, int arraySize);

static void
Pus_Msg_free_data (ubs_full_msg_t data);


#endif











void
Pus_Msg_init (void)
{

  




  Pus_Signal_register_dst (PUS_MODULE_MSG, Pus_Msg_main);
}













 




void
Pus_Msg_main (Pus_Signal_t* sig)
{

  switch (sig->type) {

  case PUS_MSG_SIG_NEW_MESSAGE:
    pus_handle_push_message (sig->i_param1);
    break;

  


#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
  case PUS_MSG_SIG_UBS_MESSAGE_DATA:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,"received PUS_MSG_SIG_UBS_MESSAGE_DATA\n"));
    pus_handle_ubs_message_data ((ubs_reply_msg_t*)sig->p_param);
    break;
#endif

  


#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
  case PUS_PRH_SIG_PARSING_COMPLETED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,"received PUS_PRH_SIG_PARSING_COMPLETED\n"));
    pus_handle_parsing_completed (sig->i_param1);
    break;

  case PUS_PRH_SIG_PARSING_ABORTED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,"received PUS_PRH_SIG_PARSING_ABORTED\n"));
    pus_handle_parsing_aborted (sig->i_param1);
    break;
#endif
  


  case PUS_SIG_DIALOG_RESPONSE:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,"received PUS_SIG_DIALOG_RESPONSE\n"));
    Pus_Msg_handle_dialog_response ((Pus_Dlg_t*)sig->p_param, sig->type);
    break;

  }

  Pus_Signal_delete (sig);
  
}


#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES











static void
pus_handle_parsing_completed (int msg_id)
{
  Pus_Msg_list_t          *msg = pus_find_msg_by_id (msg_id);
  char                    *text;

  if (msg == NULL)
    return;

  if (msg->whitelist_result == PUS_WHITELIST_NO_MATCH) {
    
    
    if ((msg->content_type_int == WE_MIME_TYPE_TEXT_VND_WAP_SI) ||
        (msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_SIC) ||
        (msg->content_type_int == WE_MIME_TYPE_TEXT_VND_WAP_SL) ||
        (msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_SLC)) {
      if (msg->parsed_info.text == NULL)
        text = msg->parsed_info.href;
      else
        text = msg->parsed_info.text;
      msg->dialog_id = pus_disp_start_whitelist_dialog (PUS_MODULE_MSG, text);
      return;
    }
  }
  
  msg->content->_u.data = NULL;
  pus_handle_parsed_msg (msg);
}

static void
pus_handle_parsed_msg (Pus_Msg_list_t *msg)
{
  ubs_msg_handle_t        handle;
  pus_co_data_t           *tmp_co;
  char                    *url = NULL, *url2, *scheme;
  WE_UINT32               current_time;

  if (msg == NULL)
    return;


  
  if ((msg->content_type_int == WE_MIME_TYPE_TEXT_VND_WAP_SI) ||
      (msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_SIC)) {    

    
    current_time = WE_TIME_GET_CURRENT ();
    if (!((msg->parsed_info.expired != 0) &&
          (current_time > msg->parsed_info.expired))) {
      

      
      if (msg->parsed_info.si_id == NULL) {
        

        if (msg->parsed_info.href != NULL) {
          msg->parsed_info.si_id = we_cmmn_strdup (WE_MODID_PUS, msg->parsed_info.href);
        }
        else {
          msg->parsed_info.si_id = Pus_Msg_create_random_id ();
        }
      }
      
      if (msg->parsed_info.priority != PUS_PRIO_NONE) {
         
        handle = Pus_Msg_create_ubs_handle ((unsigned char*) msg->parsed_info.si_id);
        UBSif_getMsg (WE_MODID_PUS, (unsigned int)msg->wid, UBS_MSG_TYPE_PUSH_SI, &handle);
        PUS_MEM_FREE (handle.handle);
        return;               
      }
      
    }
         
  }

  
  else if ((msg->content_type_int == WE_MIME_TYPE_TEXT_VND_WAP_SL) ||
    (msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_SLC)) {
    













    





      if (!pus_support_SL) {
        
        pus_delete_msg_from_list (msg->wid);
        return;
      }

      if (msg->parsed_info.href == NULL) { 
        pus_delete_msg_from_list (msg->wid);
        return;
      }

      


      handle = Pus_Msg_create_ubs_handle ((unsigned char*) msg->parsed_info.href);
      UBSif_getMsg (WE_MODID_PUS, (unsigned int)msg->wid, UBS_MSG_TYPE_PUSH_SL, &handle);
      PUS_MEM_FREE (handle.handle);
      return;                
  }

  
  else if ((msg->content_type_int == WE_MIME_TYPE_TEXT_VND_WAP_CO) ||
           (msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_COC)) {
     
    tmp_co = msg->parsed_info.co_data;
    while (tmp_co != NULL) {
      
      if ((scheme = we_url_get_scheme (WE_MODID_PUS, tmp_co->text)) == NULL) {
        

        if (msg->content_location && msg->content_uri) {
          we_url_resolve (WE_MODID_PUS, msg->content_uri, msg->content_location, &url);
        }
        else if (msg->content_uri) {
          unsigned int len = strlen(msg->content_uri);
          url = (char*)PUS_MEM_ALLOC (len + 1);
          memcpy (url, msg->content_uri, len);
          url[len] = 0;
        }
        else if (msg->content_location) {
          unsigned int len = strlen(msg->content_location);
          url = (char*)PUS_MEM_ALLOC (len + 1);
          memcpy (url, msg->content_location, len);
          url[len] = 0;
        }
        else
          break;
        
        if ((scheme = we_url_get_scheme (WE_MODID_PUS, url)) != NULL) {
          we_url_resolve (WE_MODID_PUS, url, tmp_co->text, &url2);
          PUS_MEM_FREE (tmp_co->text);
          PUS_MEM_FREE (scheme);
          tmp_co->text = url2;
        }
        else 
          break; 
      }
      else {
        PUS_MEM_FREE (scheme);
        url2 = tmp_co->text;
      }
      if ( tmp_co->type == PUS_ELEMENT_INVALIDATE_OBJECT )
          STKif_clear (WE_MODID_PUS, STK_CLEAR_CACHE_URL_MATCH, url2);
        else if ( tmp_co->type == PUS_ELEMENT_INVALIDATE_SERVICE )
          STKif_clear (WE_MODID_PUS, STK_CLEAR_CACHE_URL_PREFIX, url2);
        tmp_co = tmp_co->next;
        PUS_MEM_FREE (url);
        url = NULL;  
    }
  }
  
  pus_delete_msg_from_list (msg->wid);
}




void
pus_handle_parsing_aborted (int msg_id)
{
  pus_delete_msg_from_list (msg_id);
}



















void
pus_handle_ubs_message_data (ubs_reply_msg_t *result)
{
  Pus_Msg_list_t          *msg = pus_find_msg_by_id ((int)result->transactionId); 
  WE_UINT32               created, priority;

  if (msg == NULL)
    
    goto done;

  if (result->msgType == UBS_MSG_TYPE_PUSH_SL) 
  {
  	
    if (result->result == UBS_RESULT_SUCCESS) 
   { 
      if (!Pus_Msg_get_properity_bool (result->msg.propertyList, 
                                       result->msg.propertyListSize,
                                       UBS_MSG_KEY_READ)) 
      {  
        priority = Pus_Msg_get_properity_uint32 (result->msg.propertyList, 
                                                 result->msg.propertyListSize,
                                                 UBS_MSG_KEY_PRIORITY);
        if ((msg->parsed_info.priority == PUS_PRIO_HIGH) &&
            (priority == PUS_PRIO_LOW)) 
        {
          


          UBSif_deleteMsg (WE_MODID_PUS, 0, UBS_MSG_TYPE_PUSH_SL, &(result->msg.handle));
          Pus_Msg_store_msg_in_ubs (msg, UBS_MSG_TYPE_PUSH_SL);
        }
      }
      else 
      {
        
        if (msg->parsed_info.priority != PUS_PRIO_CACHE)
          UBSif_deleteMsg (WE_MODID_PUS, 0, UBS_MSG_TYPE_PUSH_SL, &(result->msg.handle));
        Pus_Msg_store_msg_in_ubs (msg, UBS_MSG_TYPE_PUSH_SL);
      }
    }
    else 
    {
      
      Pus_Msg_store_msg_in_ubs (msg, UBS_MSG_TYPE_PUSH_SL);
    }
  }
  
  else if (result->msgType == UBS_MSG_TYPE_PUSH_SI) 
  { 
    if (result->result == UBS_RESULT_SUCCESS) 
    { 
      if (msg->parsed_info.created == 0) 
     {   
        PUS_MEM_FREE (msg->parsed_info.si_id);
        msg->parsed_info.created = WE_TIME_GET_CURRENT ();
        msg->parsed_info.si_id = Pus_Msg_create_random_id ();
        Pus_Msg_store_msg_in_ubs (msg, UBS_MSG_TYPE_PUSH_SI);
      }
      else 
      {
        created = Pus_Msg_get_properity_uint32 (result->msg.propertyList, 
                                                result->msg.propertyListSize,
                                                UBS_MSG_KEY_CREATED);
        if (msg->parsed_info.created < created) 
          
          goto done;    
        
        UBSif_deleteMsg (WE_MODID_PUS, 0, UBS_MSG_TYPE_PUSH_SI, &(result->msg.handle));
        Pus_Msg_store_msg_in_ubs (msg, UBS_MSG_TYPE_PUSH_SI);
      }
    }
    else 
    { 
      if (msg->parsed_info.created == 0) 
     {
         PUS_MEM_FREE (msg->parsed_info.si_id);
         msg->parsed_info.created = WE_TIME_GET_CURRENT ();
         msg->parsed_info.si_id = Pus_Msg_create_random_id ();
      }
      
       Pus_Msg_store_msg_in_ubs (msg, UBS_MSG_TYPE_PUSH_SI);
    }
  }
done:
  pus_delete_msg_from_list ((int)result->transactionId); 
  Pus_Msg_free_key_value_array (&(result->msg.propertyList), result->msg.propertyListSize);
  PUS_MEM_FREE (result->msg.handle.handle);
  PUS_MEM_FREE (result);
}

static void
Pus_Msg_store_msg_in_ubs (Pus_Msg_list_t *msg, int type) 
{
  ubs_full_msg_t           data;

  if (type == UBS_MSG_TYPE_PUSH_SI) {
  
    if ((msg->parsed_info.priority != PUS_PRIO_DELETE) &&
        (msg->parsed_info.priority != PUS_PRIO_NONE)) {
      Pus_Msg_create_data_for_ubs (&data, msg, UBS_MSG_TYPE_PUSH_SI);
      UBSif_createMsg (WE_MODID_PUS, 0, UBS_MSG_TYPE_PUSH_SI, &data);
      Pus_Msg_free_data (data);
    }
  }
  else if (type == UBS_MSG_TYPE_PUSH_SL) {
    if (msg->parsed_info.priority != PUS_PRIO_CACHE) {
      Pus_Msg_create_data_for_ubs (&data, msg, UBS_MSG_TYPE_PUSH_SL);
      UBSif_createMsg (WE_MODID_PUS, 0, UBS_MSG_TYPE_PUSH_SL, &data);
      Pus_Msg_free_data (data);
    }
    else {
      char          *tmp_url;
      tmp_url = we_url_make_complete (WE_MODID_PUS, msg->parsed_info.href);
      if (tmp_url)
        STKif_httpRequest (WE_MODID_PUS, STK_DEFAULT_CHANNEL, 1,
                           tmp_url, STK_METHOD_GET, 0,
                           "accept: */*\r\n", NULL, 0, 0, NULL);
      PUS_MEM_FREE (tmp_url);
    }
  }
}





static char*
Pus_Msg_create_random_id ()
{
  WE_UINT32               tmp2;
  WE_UINT32               random_num;
  char                     tmp_si_id[16];
  
  tmp2 = (WE_UINT32) (rand () & 0xffff);
  random_num = (WE_TIME_GET_CURRENT () ^ ((tmp2 << 16) | tmp2)) & 0x0fffffff;
  sprintf (tmp_si_id, "S%07lx.%s", random_num, "si_id"); 
  return we_cmmn_strdup (WE_MODID_PUS, tmp_si_id);
}

#endif




void
pus_handle_content_send_ack (int wid, int status)
{
  
  Pus_Msg_list_t      *push_msg = pus_find_msg_by_id (wid);

  if (push_msg == NULL) 
    return;
  
  
  if (status == WE_CONTENT_STATUS_SUCCEED)
    STKif_confirmRequest (WE_MODID_PUS, push_msg->conn_id, push_msg->req_id);
  else if (push_msg->routing_type == WE_ROUTING_TYPE_MIME)
    STKif_abortRequest (WE_MODID_PUS, push_msg->conn_id, push_msg->req_id, PUS_ABORT_CONTENT_TYPE_REFUSE);
  else if (push_msg->routing_type == WE_ROUTING_TYPE_PAPPID)
    STKif_abortRequest (WE_MODID_PUS, push_msg->conn_id, push_msg->req_id, PUS_ABORT_DESTINATION_REFUSE);

  pus_delete_msg_from_list (wid);
  
}

void 
Pus_Msg_delete_all_message_in_list (void)
{
  Pus_Msg_list_t   *tmp;
  
  for (tmp = Pus_Msg_list; tmp != NULL; tmp = Pus_Msg_list) {
    pus_delete_msg_from_list (tmp->wid);
  }
}







static void
pus_handle_push_message (int wid)
{
  Pus_Msg_list_t*   msg = pus_find_msg_by_id (wid);
  char             *application_id = NULL;
  char             *content_type_string = NULL;
  WE_UINT32        content_type_int;
  WE_UINT8         push_flag;
  int               charset;
  int               net_id = Pus_Conn_get_network_acc_id_type_by_connection_id (msg->conn_id);

  if (msg == NULL)
    return;

  pus_get_push_headers (msg->headers, &application_id, 
                        &content_type_string, &content_type_int,
                        &(msg->content_uri), &(msg->content_location), &(msg->initiator_uri), 
                        &push_flag, &charset);

  
  msg->content_type_int = content_type_int;
  msg->routing_type = pus_check_if_handled_by_pus (application_id, msg->content_type_int);

  if (pus_check_push_security_level (msg->routing_type, push_flag)) 
  {
    if (!we_cmmn_strcmp_nc (msg->url, "/wappush_confirmed"))
       STKif_abortRequest (WE_MODID_PUS, msg->conn_id, msg->req_id, PUS_ABORT_USER_REFUSE);
    pus_delete_msg_from_list (msg->wid);
    PUS_MEM_FREE (application_id);
    PUS_MEM_FREE (content_type_string);
    return;
  }

  if (msg->routing_type) 
  {
    if (msg->routing_type == WE_ROUTING_TYPE_MIME) 
   {
      if (content_type_string != NULL) 
     {
        char *p = strchr (content_type_string, ';');
        if (p == NULL) 
	{
          msg->routing_identifier = we_cmmn_strdup(WE_MODID_PUS, content_type_string);
        }
        else 
	 {
          int len = we_cmmn_skip_trailing_blanks (content_type_string, p - content_type_string);
          msg->routing_identifier = we_cmmn_strndup (WE_MODID_PUS, content_type_string, len);
        }
      }
    }
    else
      msg->routing_identifier = we_cmmn_strdup(WE_MODID_PUS, application_id);
    if (msg->whitelist_result == PUS_WHITELIST_NO_MATCH) 
    {
          msg->dialog_id = pus_disp_start_whitelist_dialog (PUS_MODULE_MSG, msg->routing_identifier);
    }
    else 
    {
      pus_send_push_to_content_router (msg->content, msg->headers, msg->routing_type, 
                                       msg->routing_identifier, msg->wid, net_id);
      pus_delete_msg_from_list (msg->wid);
    }
  }
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
  else 
  { 
      
      if ((msg->url != NULL) && (!we_cmmn_strcmp_nc (msg->url, "/wappush_confirmed"))) 
      {
        if ((msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_SIA) &&
            (sir_lockout_timer)) 
          STKif_abortRequest (WE_MODID_PUS, msg->conn_id, msg->req_id, PUS_ABORT_RESOURCE_REFUSE);
        else
          STKif_confirmRequest (WE_MODID_PUS, msg->conn_id, msg->req_id);
      }

    if (msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_SIA) 
    {
      if (!sir_lockout_timer) {
        int timer_value = PUS_CFG_SIR_LOCKOUT_TIMER; 
        if ((pus_handle_sia_push (msg->content)) && (timer_value > 0)) 
	{
          
          
          we_timer_set (WE_MODID_PUS, 1, PUS_CFG_SIR_LOCKOUT_TIMER);
          sir_lockout_timer = TRUE;
        }
        pus_delete_msg_from_list (msg->wid);  
      }
    }
    else 
    { 
      pus_start_parsing_t  *start_parsing = PUS_MEM_ALLOCTYPE (pus_start_parsing_t);
      start_parsing->content_type_int = msg->content_type_int;
      start_parsing->charset = charset;
      start_parsing->data = msg->content->_u.data;
      start_parsing->data_len = msg->content->dataLen;
      start_parsing->data_type = msg->content->dataType;
      start_parsing->instance_id = wid;  
      msg->parsed_info.co_data = NULL;
      msg->parsed_info.created = 0;
      msg->parsed_info.expired = 0;
      msg->parsed_info.href = NULL;
      msg->parsed_info.priority = 0;
      msg->parsed_info.si_id = NULL;
      msg->parsed_info.text = NULL;
      msg->state = PUS_STATE_PRS_STARTED;
      PUS_SIGNAL_SENDTO_P (PUS_MODULE_PARSER, PUS_PRH_SIG_START_PARSING, start_parsing);
    }
  }
#else
  pus_delete_msg_from_list (msg->wid);  
#endif
  PUS_MEM_FREE (application_id);
  PUS_MEM_FREE (content_type_string);
}









static void
pus_get_push_headers(char *headers, char **application_id,
                     char **content_type_string, WE_UINT32 *content_type_int,
                     char **content_uri, char **content_location, 
                     char **initiator_uri, WE_UINT8 *push_flag, int *charset)
{
  we_hdr_t  *hdr = we_hdr_create (WE_MODID_PUS);
  char       *push_flag_string;
  char       *chrs;


  *charset = WE_CHARSET_UNKNOWN;

  we_hdr_add_headers_from_text (hdr, headers);

  


  *application_id = (char *) we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string (hdr, WE_HDR_X_WAP_APPLICATION_ID));

  
  *content_type_string = (char *)we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string (hdr, WE_HDR_CONTENT_TYPE));
  if (!we_hdr_get_uint32 (hdr, WE_HDR_CONTENT_TYPE, content_type_int)) {
    *content_type_int = WE_MIME_TYPE_UNKNOWN;
  }

  
  chrs = we_hdr_get_parameter (hdr, WE_HDR_CONTENT_TYPE, "charset");
  if (chrs != NULL) {
    *charset = we_charset_str2int (chrs, strlen (chrs));
    if (*charset <= 0)
      *charset = WE_CHARSET_UNKNOWN;
    PUS_MEM_FREE (chrs);
  }
  
  
  *content_uri = (char *)we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string (hdr, WE_HDR_X_WAP_CONTENT_URI));

  
  *content_location = (char *)we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string (hdr, WE_HDR_CONTENT_LOCATION));

  
  if ((*initiator_uri = (char *)we_cmmn_strdup (WE_MODID_PUS, 
                                                 we_hdr_get_string (hdr, WE_HDR_X_WAP_INITIATOR_URI))) 
                                                 == NULL)
    *initiator_uri = we_cmmn_strdup (WE_MODID_PUS, *content_uri);

  
  push_flag_string = (char *)we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string (hdr, WE_HDR_PUSH_FLAG));
  if (push_flag_string != NULL)
    *push_flag = (WE_UINT8)atoi (push_flag_string);
  else
    *push_flag = 0;

  PUS_MEM_FREE (push_flag_string);
  we_hdr_delete (hdr);
}




static int
pus_check_if_handled_by_pus (char *application_id, WE_UINT32 content_type_int)
{
  
  if (application_id != NULL)
  {
    if (!((!we_cmmn_strcmp_nc (application_id, "x-wap-application:*")) ||
          (!we_cmmn_strcmp_nc (application_id, "x-wap-application:push.sia")) ||
          (!we_cmmn_strcmp_nc (application_id, "x-wap-application:wml.ua")) ||
          (!we_cmmn_strcmp_nc (application_id, "0")) ||
          (!we_cmmn_strcmp_nc (application_id, "1")) ||
          (!we_cmmn_strcmp_nc (application_id, "2"))))
    {
      

      return WE_ROUTING_TYPE_PAPPID;
    }
  }
  
  


  if (((content_type_int >= WE_MIME_TYPE_TEXT_VND_WAP_SI) && 
       (content_type_int <= WE_MIME_TYPE_APPLICATION_VND_WAP_COC)) || 
      (content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_SIA))
      return FALSE;
  else
    return WE_ROUTING_TYPE_MIME;
}














static int
pus_check_push_security_level (int routing_type, WE_UINT8 push_flag)
{
  if ((pus_security_level == 3) ||
      (pus_security_level == 2 && !routing_type) ||
      (pus_security_level == 1 && !((PUS_URI_IS_AUTHENTICATED + PUS_CONTENT_IS_TRUSTED) & push_flag))) {
    
    
    return TRUE;
  }
  return FALSE;
}




static void
pus_send_push_to_content_router (stk_content_t* content, char* hdrs, int routing_type, 
                                 char* routing_identifier, int wid, int net_id) 
{
  we_content_data_t    content_data;

  content_data.routingFormat = (WE_UINT8) routing_type;
  content_data.routingIdentifier = routing_identifier;
  content_data.contentParameters = NULL;
  content_data.contentUrl = NULL;
  content_data.contentData = NULL;
  content_data.networkAccountId = net_id;
  content_data.contentHeaders = hdrs;
  if ((content->dataLen == -1) || ((WE_UINT8)content->dataType != WE_CONTENT_DATA_RAW))
    content_data.contentDataLength = 0;
  else
    content_data.contentDataLength = content->dataLen;
  content_data.contentDataType = (WE_UINT8)content->dataType;
  content_data.contentSource = NULL;

  if (content_data.contentDataType == WE_CONTENT_DATA_RAW)
    content_data.contentData = content->_u.data;
  else if (content_data.contentDataType == WE_CONTENT_DATA_PIPE) {
    content_data.contentSource = content->_u.pipeName;
  }
  else if (content_data.contentDataType == WE_CONTENT_DATA_FILE) {
    content_data.contentSource = content->_u.pathName;
  }
  
  content_data.contentType = NULL;
  content_data.embeddedObjectExists = FALSE;
  content_data.embeddedObjectData = NULL;

  WE_CONTENT_SEND(WE_MODID_PUS, WE_MODID_FRW, NULL, &content_data, TRUE, (WE_UINT8) wid, FALSE);  
}




#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 

static void
Pus_Msg_create_data_for_ubs (ubs_full_msg_t *data, Pus_Msg_list_t* msg, int type)
{
  WE_UINT32 time_stamp = msg->parsed_info.created ? msg->parsed_info.created : WE_TIME_GET_CURRENT ();

  if (type == UBS_MSG_TYPE_PUSH_SL)
    data->handle = Pus_Msg_create_ubs_handle ((unsigned char*)msg->parsed_info.href);
  else
    data->handle = Pus_Msg_create_ubs_handle ((unsigned char*)msg->parsed_info.si_id);
  
  data->propertyListSize = 0;

  data->propertyList = PUS_MEM_ALLOC(sizeof(ubs_key_value_t)*PUS_NUMBER_OF_PROP_FOR_UBS);

  
  Pus_Msg_set_ubs_UTF8_value (&(data->propertyList[data->propertyListSize]), 
                              UBS_MSG_KEY_FOLDER, (const unsigned char*)PUS_FOLDER_NAME);
  data->propertyListSize++;
  
  
  Pus_Msg_set_ubs_UTF8_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_SUBJECT, 
                               (const unsigned char*) (msg->parsed_info.text));
  data->propertyListSize++;
  
  
  Pus_Msg_set_ubs_UTF8_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_FROM,
                              (const unsigned char*)(msg->initiator_uri));
  data->propertyListSize++;

  
  Pus_Msg_set_ubs_uint32_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_TIMESTAMP,
                                time_stamp);
  data->propertyListSize++;

  
  Pus_Msg_set_ubs_bool_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_READ,
                              FALSE);
  data->propertyListSize++;

  
  Pus_Msg_set_ubs_uint32_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_PRIORITY,
                                (WE_UINT32) (msg->parsed_info.priority));
  data->propertyListSize++;

  
  if (msg->parsed_info.created) {
    Pus_Msg_set_ubs_uint32_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_CREATED,
                                  msg->parsed_info.created);
    data->propertyListSize++;
  }
  
  
  if (msg->parsed_info.expired) {
    Pus_Msg_set_ubs_uint32_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_EXPIRES,
                                  msg->parsed_info.expired);
    data->propertyListSize++;
  }
  
  
  Pus_Msg_set_ubs_UTF8_value (&(data->propertyList[data->propertyListSize]), UBS_MSG_KEY_URL,
                              (const unsigned char*)(msg->parsed_info.href));
  data->propertyListSize++;

  data->hiddenPropertyListSize = 0;
  data->hiddenPropertyList = NULL;
  data->data = NULL;
  data->dataLen = 0;
}


static ubs_msg_handle_t
Pus_Msg_create_ubs_handle (unsigned char* value)
{
  ubs_msg_handle_t handle;
  handle.handle = (unsigned char*)we_cmmn_strdup (WE_MODID_PUS, (const char*)value);
  handle.handleLen = (WE_UINT16) strlen ((const char*) value);
  return handle;
}




static void 
Pus_Msg_set_ubs_UTF8_value (ubs_key_value_t *result, WE_UINT16 key, 
                            const unsigned char *value)
{
  WE_UINT16 len;

  if (value == NULL)
    len = 0;
  else
    len = (WE_UINT16) (strlen((const char*)value)+1);

  result->key = key;
  result->valueType = UBS_VALUE_TYPE_UTF8;
  result->value = (unsigned char*) we_cmmn_strdup (WE_MODID_PUS, (const char*)value);
  result->valueLen = len;
}




static void 
Pus_Msg_set_ubs_uint32_value(ubs_key_value_t *result,
                             WE_UINT16 key,
                             WE_UINT32 value)
{
    result->key = key;
    result->valueType = UBS_VALUE_TYPE_UINT32;
    result->value = WE_MEM_ALLOC(WE_MODID_PUS, sizeof(value));
    (*((WE_UINT32*)result->value)) = value;
    result->valueLen = sizeof(value);
}




static void 
Pus_Msg_set_ubs_bool_value(ubs_key_value_t *result,
                           WE_UINT16 key,
                           WE_BOOL value)
{
    result->key = key;
    result->valueType = UBS_VALUE_TYPE_BOOL;
    result->value = WE_MEM_ALLOC(WE_MODID_PUS, sizeof(value));
    (*((WE_BOOL*)result->value)) = value;
    result->valueLen = sizeof(value);
}

static WE_UINT32
Pus_Msg_get_properity_uint32 (ubs_key_value_t* propertyList, 
                              WE_UINT16 list_len,
                              WE_UINT16 key)
{
  WE_UINT32 value;
  int i;

  for (i=0; i < list_len; i++) {
    if(propertyList[i].key == key) {
      value = (*((WE_UINT32*)propertyList[i].value));
      return value;
    }
  }
  return 0;
}

static WE_UINT32
Pus_Msg_get_properity_bool (ubs_key_value_t* propertyList, 
                            WE_UINT16 list_len,
                            WE_UINT16 key)
{
  int value;
  int i;

  for (i=0; i < list_len; i++) {
    if(propertyList[i].key == key) {
      value = (*((WE_BOOL*)propertyList[i].value));
      return value;
    }
  }
  return 0;
}

static void
Pus_Msg_free_key_value (ubs_key_value_t *p)
{
  if (p && (p->value)) {
    PUS_MEM_FREE (p->value);
  }
}


static void
Pus_Msg_free_key_value_array (ubs_key_value_t **p, int arraySize)
{
  if (*p) 
  {
    int i;
    for (i = 0; i < arraySize; i++ ) {
      Pus_Msg_free_key_value (&((*p)[i]));
    }
    PUS_MEM_FREE (*p);
  }
}

static void
Pus_Msg_free_data (ubs_full_msg_t data)
{
  Pus_Msg_free_key_value_array (&(data.propertyList), data.propertyListSize);
  PUS_MEM_FREE (data.handle.handle);
  PUS_MEM_FREE (data.data);
}

#endif










static void
Pus_Msg_handle_dialog_response (Pus_Dlg_t* dlg, int type)
{
  int               dialog_id = Pus_DlgGetDialogHandle (dlg);
  int               answer, more_dialogs;
  Pus_Msg_list_t   *msg;
  char             *address;
  const char       *key;

  if (type == PUS_SIG_DIALOG_RESPONSE) { 
    
    for (msg = Pus_Msg_list; msg != NULL; msg = msg->next_msg) {
      if (msg->dialog_id == dialog_id);
        break;
    }

    if (msg == NULL)
      goto done;
  
    
    answer = Pus_DlgGetResponse (dlg);
    if (answer == PUS_DLG_RESPONSE_POSITIVE) { 
      
      if (msg->connection_type == STK_CONNECTION_TYPE_SMS_DATA) {
        key = Pus_Main_get_key_as_string (PUS_REG_KEY_WHITELIST_SMSC); 
        address = Pus_Msg_cvt_smsc_address_to_string (msg->fromAddr);
        pus_whitelist_smsc = Pus_Msg_add_address_to_whitelist (pus_whitelist_smsc, key, address);
      }
      else {
        key = Pus_Main_get_key_as_string (PUS_REG_KEY_WHITELIST_IP);
        address = Pus_Msg_cvt_ip_address_to_string (msg->fromAddr);
        pus_whitelist_ip = Pus_Msg_add_address_to_whitelist (pus_whitelist_ip, key, address);
      }
  
      if (msg->routing_type) {
        
        pus_send_push_to_content_router (msg->content, msg->headers, msg->routing_type, 
                                         msg->routing_identifier, msg->wid, 
                                         Pus_Conn_get_network_acc_id_type_by_connection_id (msg->conn_id));
        pus_delete_msg_from_list (msg->wid);
      }
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
      else {
        

        



        msg->content->_u.data = NULL;
        pus_handle_parsed_msg (msg);
      }
#endif
    }
    else if (answer == PUS_DLG_RESPONSE_CANCEL) {
      if (!msg->routing_type) { 
        msg->content->_u.data = NULL;
      }
      
      if (msg->connection_type == STK_CONNECTION_TYPE_SMS_DATA) {
        key = Pus_Main_get_key_as_string (PUS_REG_KEY_BLACKLIST_SMSC);
        address = Pus_Msg_cvt_smsc_address_to_string (msg->fromAddr);
        pus_blacklist_smsc = Pus_Msg_add_address_to_whitelist (pus_blacklist_smsc, key, address);
      }
      else {
        key = Pus_Main_get_key_as_string (PUS_REG_KEY_BLACKLIST_IP);
        address = Pus_Msg_cvt_ip_address_to_string (msg->fromAddr);
        pus_blacklist_ip = Pus_Msg_add_address_to_whitelist (pus_blacklist_ip, key, address);
      }
      pus_delete_msg_from_list (msg->wid);
    }
  }

done:
  more_dialogs = pus_deregister_dialog (dlg); 
  Pus_DlgDelete (dlg, more_dialogs);
}





static char*      
Pus_Msg_add_address_to_whitelist (char* list, const char *key, char* address)
{
  void  *reg_handle;
  char  *tmp_s, *tmp_list;

  if (list == NULL) {
    tmp_s = address;
  }
  else {
    tmp_list = PUS_MEM_ALLOC (strlen (list) + 2);
    memcpy (tmp_list, list, strlen (list));
    tmp_list[strlen (list)] = ';';
    tmp_list[strlen (list) + 1] = '\0';
    tmp_s = we_cmmn_strcat (WE_MODID_PUS, tmp_list, address);
    PUS_MEM_FREE (list);
    PUS_MEM_FREE (tmp_list);
    PUS_MEM_FREE (address);
  }

  reg_handle = WE_REGISTRY_SET_INIT (WE_MODID_PUS);
  WE_REGISTRY_SET_PATH (reg_handle, (char*)Pus_Main_get_path_as_string(PUS_REG_PATH_WHITELIST));

  WE_REGISTRY_SET_ADD_KEY_STR (reg_handle, (char*)key, (unsigned char*)tmp_s, (WE_UINT16)(strlen (tmp_s)+1));
  WE_REGISTRY_SET_COMMIT (reg_handle);
  return tmp_s;
}







static char*
Pus_Msg_cvt_smsc_address_to_string (we_sockaddr_t fromAddr)
{
  char *s;
  int   i, tmp1, tmp2, j = 0;
  int   len = fromAddr.addrLen * 2;

  if (fromAddr.addr [fromAddr.addrLen] > 0x0F) 
    len--;

  s = PUS_MEM_ALLOC (len + 1);

  for (i=0; i <= fromAddr.addrLen; i++) {
    tmp1 = (fromAddr.addr[i] & 0x0f); 
    tmp2 = (fromAddr.addr[i] >> 4);   
    if (tmp1 == 0x0a) 
      s[j] = '*';
    else if (tmp1 == 0x0b)
      s[j] = '#';
    else if (tmp1 == 0x0c)
      s[j] = 'a';
    else if (tmp1 == 0x0d)
      s[j] = 'b';
    else if (tmp1 == 0x0e)
      s[j] = 'c';
    else if (tmp1 == 0x0f)
      ; 
    else
      sprintf (&s[j], "%i", tmp1);
    if (tmp2 == 0x0a) 
      s[j+1] = '*';
    else if (tmp2 == 0x0b)
      s[j+1] = '#';
    else if (tmp2 == 0x0c)
      s[j+1] = 'a';
    else if (tmp2 == 0x0d)
      s[j+1] = 'b';
    else if (tmp2 == 0x0e)
      s[j+1] = 'c';
    else if (tmp2 == 0x0f)
      ; 
    else
      sprintf (&s[j+1], "%i", tmp2);
    j = j + 2;
  }
  return s;
}






static char*
Pus_Msg_cvt_ip_address_to_string (we_sockaddr_t fromAddr)
{
  char *s;
  char *tmp_s = PUS_MEM_ALLOC (4);
  int   i, len = 0;
  
  for (i=0; i < fromAddr.addrLen; i++) {
    len = len + sprintf (tmp_s, "%u", fromAddr.addr[i]);
  }
  tmp_s[3] = '\0';
  PUS_MEM_FREE (tmp_s);
  s = PUS_MEM_ALLOC (len + 4); 
  

  sprintf (s, "%u.%u.%u.%u", fromAddr.addr[0], fromAddr.addr[1], fromAddr.addr[2], fromAddr.addr[3]);
  return s;
}
      


     

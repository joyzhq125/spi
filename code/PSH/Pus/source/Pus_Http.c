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









#include "Pus_Cfg.h"

#ifdef PUS_CONFIG_HTTP_PUSH

#include "we_hdr.h"
#include "we_cmmn.h"
#include "we_neta.h"

#include "stk_if.h"

#include "Pus_Http.h"
#include "Pus_Conn.h"
#include "Pus_Msg.h"
#include "Pus_Sig.h"
#include "Pus_Cmmn.h"





#define PUS_HTTP_OPTIONS_CPI_MATCH                       "500"
#define PUS_HTTP_OPTIONS_CPI_MISIATCH_OR_NOT_INCLUDED    "501"

#define PUS_HTTP_POST_REQUEST_REJECTED_REQ               "234"
#define PUS_HTTP_POST_CPI_MISIATCH                       "256"
#define PUS_HTTP_POST_CPI_MATCH_OR_NOT_INCLUDED          "400"

#define PUS_HTTP_LINE_BUF_SIZE                            256




typedef struct push_hdr_data_st {
  unsigned char     *buf;
  int                buf_len;
  int                buf_pos;
  char               line_buf[PUS_HTTP_LINE_BUF_SIZE];
  int                line_pos;
} Pus_Hdr_data_t;





static void
Pus_Http_handle_new_http_request (int wid);

static void
Pus_Http_handle_stream_notification (int stream_handle, int msg_id);

static int
Pus_Http_handle_post (Pus_Msg_list_t* msg);

static void
Pus_Http_whole_message_in_buf(Pus_Msg_list_t* msg);

static int
Pus_Http_read_one_line (Pus_Hdr_data_t *hdr_data);

static void
Pus_Http_send_response (we_hdr_t *send_hdrs, int http_status_code,
                        int conn_id, int req_id);

static char*
Pus_Http_get_terminal_id_based_on_realm (char* authenticate);








void
Pus_Http_init (void)
{

  




  Pus_Signal_register_dst (PUS_MODULE_HTTP, Pus_Http_main);
}













 




void
Pus_Http_main (Pus_Signal_t* sig)
{

  if (sig->type == PUS_HTTP_SIG_NEW_MESSAGE) {
    Pus_Http_handle_new_http_request (sig->i_param1);
  } 
  else if (sig->type == PUS_SIG_NOTIFY) {
    Pus_Http_handle_stream_notification (sig->i_param1, sig->i_param2);   
  }

  Pus_Signal_delete (sig);
}




static void
Pus_Http_handle_new_http_request (int wid)
{
  
  Pus_Msg_list_t*           msg = pus_find_msg_by_id (wid);
  int                       network_account_id, bearer, http_status_code = STK_HTTP_NO_CONTENT;
  int                       leave = FALSE;
  char                     *version, *host = NULL;
  char                     *cpi_tag = NULL;
  char                     *authenticate = NULL;
  char                     *terminal_id = NULL;
  char                     *str_bearer = NULL;

  we_hdr_t  *send_hdrs = we_hdr_create (WE_MODID_PUS);

  we_hdr_t  *hdr = we_hdr_create (WE_MODID_PUS);
  we_hdr_add_headers_from_text (hdr, msg->headers);
  
  
  version = we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string 
                             (hdr, WE_HDR_X_WAP_PUSH_OTA_VERSION));
  if (version != NULL) {
    
    we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_OTA_VERSION, version);
  }

  
  if ((msg->method != STK_METHOD_POST) && (msg->method != STK_METHOD_OPTIONS)) {
    Pus_Http_send_response (send_hdrs, STK_HTTP_NOT_IMPLEMENTED,
                            msg->conn_id, msg->req_id);
    goto done;
  }

  host = we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string (hdr, WE_HDR_HOST));
  
  if ( (!strcmp (msg->url, "/wappush")) && (*host == '\0') ) {  
    
    


    cpi_tag = we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string (hdr, WE_HDR_X_WAP_CPITAG));
    if (cpi_tag == NULL) 
      cpi_tag = (char*)Pus_Conn_get_cpi_tag_by_connection_id  (msg->parent_id);

    








    authenticate = we_cmmn_strdup (WE_MODID_PUS, we_hdr_get_string 
                                    (hdr, WE_HDR_X_WAP_AUTHENTICATE));
    if (authenticate) {
      
      if (!we_cmmn_strncmp_nc (authenticate, "digest", 6)) {
        


        http_status_code = STK_HTTP_PRECONDITION_FAILED;
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_AUTHORIZATION, "failed_noretry");
      }
      else {
        
        terminal_id = Pus_Http_get_terminal_id_based_on_realm (authenticate);
        if (terminal_id != NULL) {  
          we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_AUTHORIZATION, 
                              we_cmmn_strcat (WE_MODID_PUS, "Basic ", terminal_id));
        }
        else {
          http_status_code = STK_HTTP_PRECONDITION_FAILED;
          we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_AUTHORIZATION, "failed_noretry");
        }
      }
    }
    else {
      
      terminal_id = we_cmmn_strdup (WE_MODID_PUS, pus_terminal_id);
      we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_TERMINAL_ID, terminal_id);
    }

    if (msg->method == STK_METHOD_OPTIONS) {
      
      network_account_id = Pus_Conn_get_network_acc_id_type_by_connection_id (msg->parent_id);
      bearer = WE_NETWORK_ACCOUNT_GET_BEARER (network_account_id);
      if (bearer != 0) {
        str_bearer = PUS_MEM_ALLOC (3);
        we_cmmn_byte2hex ((unsigned char) bearer, str_bearer);
        str_bearer[2]='\0';
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_BEARER_INDICATION, 
                            str_bearer);
      }
      if (cpi_tag == NULL || strcmp (cpi_tag, pus_clients_cpi_tag)) {
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_CPITAG, 
                            pus_clients_cpi_tag);
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_ACCEPT, 
                            pus_cpi_hdr_values.accept);
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_ACCEPT_APP_ID,
                            pus_cpi_hdr_values.accept_app_id);
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_ACCEPT_CHARSET, 
                            pus_cpi_hdr_values.accept_charset);
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_ACCEPT_LANGUAGE, 
                            pus_cpi_hdr_values.accept_language);
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_STATUS, 
                            PUS_HTTP_OPTIONS_CPI_MISIATCH_OR_NOT_INCLUDED);
      }
      else
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_STATUS, 
                            PUS_HTTP_OPTIONS_CPI_MATCH);
    }
    else if (msg->method == STK_METHOD_POST) {
      if ( (cpi_tag != NULL) && (strcmp (cpi_tag, pus_clients_cpi_tag))) {
        we_hdr_add_string (hdr, WE_HDR_X_WAP_CPITAG, 
                            pus_clients_cpi_tag);
        we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_STATUS, 
                            PUS_HTTP_POST_CPI_MISIATCH);
      }
      else {
        
        if (!Pus_Http_handle_post (msg)) {
          we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_STATUS, 
                              PUS_HTTP_POST_REQUEST_REJECTED_REQ);
        }
        else {
          we_hdr_add_string (send_hdrs, WE_HDR_X_WAP_PUSH_STATUS, 
                          PUS_HTTP_POST_CPI_MATCH_OR_NOT_INCLUDED);
          leave = TRUE;
        }
      }
    }
  }
  else
    http_status_code = STK_HTTP_NOT_IMPLEMENTED;
  
  Pus_Http_send_response (send_hdrs, http_status_code,
                          msg->conn_id, msg->req_id);

done:
  we_hdr_delete (hdr);
  we_hdr_delete (send_hdrs);

  if (!leave)
    pus_delete_msg_from_list (wid);
  PUS_MEM_FREE (str_bearer);
  PUS_MEM_FREE (terminal_id);
  PUS_MEM_FREE (authenticate);
  PUS_MEM_FREE (version);
  PUS_MEM_FREE (host);
  PUS_MEM_FREE (cpi_tag);

}


static void
Pus_Http_handle_stream_notification (int stream_handle, int msg_id)
{
  Pus_Msg_list_t     *msg = pus_find_msg_by_id (msg_id);

  if (Pus_Cmmn_read_data_from_stream_to_buf (stream_handle, msg) == FALSE) {
    

    Pus_Cmmn_build_buf_from_data_buf_list (msg);
    
    Pus_Http_whole_message_in_buf(msg);  
  }
  
}












static void
Pus_Http_send_response (we_hdr_t *send_hdrs, int http_status_code,
                        int conn_id, int req_id)
{
  char       *headers = NULL;

  if (we_hdr_make_string (send_hdrs, &headers))
    STKif_sendReply (WE_MODID_PUS, conn_id, req_id, http_status_code, 
                     headers, 0, NULL);
  PUS_MEM_FREE (headers);
}


static int
Pus_Http_handle_post (Pus_Msg_list_t* msg)
{
  int          stream_handle;

  if (msg->content->dataType == WE_CONTENT_DATA_RAW) {
    

    msg->msg_data = msg->content->_u.data;
    msg->msg_len = msg->content->dataLen;
    Pus_Http_whole_message_in_buf(msg);
    return TRUE;
  }
  else if (msg->content->dataType == WE_CONTENT_DATA_PIPE) {
    stream_handle = pus_stream_init (PUS_MODULE_HTTP, msg->wid, PUS_STREAM_PIPE_READ, 
                                     NULL, 0, msg->content->_u.pipeName);
  }
  else if (msg->content->dataType == WE_CONTENT_DATA_FILE) {
    stream_handle = pus_stream_init (PUS_MODULE_HTTP, msg->wid, PUS_STREAM_FILE_READ, 
                                     NULL, 0, msg->content->_u.pathName);
  }
  else {
    return FALSE;
  }
  if (stream_handle != -1) {
    
    if (Pus_Cmmn_read_data_from_stream_to_buf (stream_handle, msg) == FALSE) {
      

      Pus_Cmmn_build_buf_from_data_buf_list (msg);
      
      Pus_Http_whole_message_in_buf(msg);
    }
    return TRUE;
  }
  else 
    return FALSE;
}


static void
Pus_Http_whole_message_in_buf(Pus_Msg_list_t* msg)
{
  unsigned char  *tmp;
  we_hdr_t      *tmp_hdr = NULL;
  Pus_Hdr_data_t *hdr_data = PUS_MEM_ALLOCTYPE (Pus_Hdr_data_t);

  hdr_data->buf = msg->msg_data;
  hdr_data->buf_len = msg->msg_len;
  hdr_data->buf_pos = 0;
  hdr_data->line_pos = 0;
    
  while ( (msg->state != PUS_STATE_READING_BODY) && (Pus_Http_read_one_line (hdr_data)) ) {
    switch (msg->state) {
    case PUS_STATE_WHITELIST_DONE:
      
      tmp_hdr = we_hdr_create (WE_MODID_PUS);
      msg->state = PUS_STATE_READING_HEADER;
      break;

    case PUS_STATE_READING_HEADER:
      if (strlen (hdr_data->line_buf) > 0) {
        
        we_hdr_add_header_from_text_line (tmp_hdr, hdr_data->line_buf, 
                                           strlen (hdr_data->line_buf));
      }
      else 
        

        msg->state = PUS_STATE_READING_BODY;
      break;  
    }
  }
  

  PUS_MEM_FREE (msg->headers);  
  we_hdr_make_string (tmp_hdr, &(msg->headers));
  msg->content->dataLen = msg->msg_len - hdr_data->buf_pos;
  tmp = PUS_MEM_ALLOC (msg->content->dataLen);
  memcpy (tmp, hdr_data->buf + hdr_data->buf_pos, msg->content->dataLen);
  
  if (msg->content->dataType == WE_CONTENT_DATA_RAW)
    PUS_MEM_FREE (msg->content->_u.data);
  else if (msg->content->dataType == WE_CONTENT_DATA_PIPE)
    PUS_MEM_FREE (msg->content->_u.pipeName);
  else if (msg->content->dataType == WE_CONTENT_DATA_FILE)
    PUS_MEM_FREE (msg->content->_u.pathName);

  msg->content->dataType = WE_CONTENT_DATA_RAW;
  msg->content->_u.data = tmp;
  


  PUS_MEM_FREE (msg->msg_data);
  msg->msg_len = 0;
  PUS_SIGNAL_SENDTO_I (PUS_MODULE_MSG, PUS_MSG_SIG_NEW_MESSAGE, msg->wid);
  PUS_MEM_FREE (hdr_data);
  if (tmp_hdr != NULL)
    we_hdr_delete (tmp_hdr);
}

static int
Pus_Http_read_one_line (Pus_Hdr_data_t *hdr_data)
{
  int c;

  while (hdr_data->buf != NULL) {
    c = (int)hdr_data->buf[hdr_data->buf_pos++];

    if (hdr_data->buf_pos >= hdr_data->buf_len) {
      PUS_MEM_FREE (hdr_data->buf);
      hdr_data->buf = NULL;
      hdr_data->buf_len = hdr_data->buf_pos = 0;
    }

    if (c != '\n') {  
      if (hdr_data->line_pos + 1 < PUS_HTTP_LINE_BUF_SIZE)
        hdr_data->line_buf[hdr_data->line_pos++] = (char)c;
    }
    else {
      





      if ((hdr_data->line_pos > 0) &&
          (hdr_data->line_buf[hdr_data->line_pos - 1] == '\r'))
        hdr_data->line_pos--;
      hdr_data->line_buf[hdr_data->line_pos] = '\0';
      hdr_data->line_pos = 0;
      return 1;
    }
  }

  return 0;
}






static char*
Pus_Http_get_terminal_id_based_on_realm (char* authenticate)
{
  char       *realm, *terminal_id = NULL, *tmp_realm;
  char       *tmp1, *index_in_list = pus_prov_termid_list;
  char       *tmp = NULL;
  int         realm_len, len;

  
  tmp = we_cmmn_strnchr (authenticate, '"' ,strlen(authenticate));
  tmp++;
  tmp1 = we_cmmn_strnchr (tmp, '"' ,strlen(tmp));
  len = tmp1 - tmp;
  realm = we_cmmn_strndup (WE_MODID_PUS, tmp, len);

  
  tmp_realm = we_cmmn_skip_blanks (realm);
  realm_len = we_cmmn_skip_trailing_blanks (tmp_realm, strlen(tmp_realm));
  tmp_realm = we_cmmn_strndup (WE_MODID_PUS, tmp_realm, realm_len);
  PUS_MEM_FREE (realm);

  while (index_in_list != NULL) {
    
    index_in_list = we_cmmn_skip_blanks (index_in_list);
    
    tmp = we_cmmn_strnchr (index_in_list, ':' ,strlen (index_in_list));
    len = tmp - index_in_list;
    
    tmp = we_cmmn_strndup (WE_MODID_PUS, (const char*) index_in_list, len);
    len = we_cmmn_skip_trailing_blanks (tmp, strlen(tmp));
    realm = we_cmmn_strndup (WE_MODID_PUS, tmp, len);
    PUS_MEM_FREE (tmp);
    
    if ((int)(strlen (realm)) == realm_len) { 
      
      if (!we_cmmn_strncmp_nc (realm, tmp_realm, realm_len)) {
        
        
        index_in_list = we_cmmn_strnchr (index_in_list, ':' ,strlen (index_in_list));
        index_in_list++;
        tmp1 = we_cmmn_skip_blanks (index_in_list);
        
        len = strlen (tmp1);
        terminal_id = we_cmmn_strnchr (tmp1, ';' ,len);
        if (terminal_id != NULL)
          len = terminal_id - tmp1;
        else  
          len = strlen (tmp1);
        
        tmp = we_cmmn_strndup (WE_MODID_PUS, (const char*) tmp1, len);
        len = we_cmmn_skip_trailing_blanks (tmp, strlen(tmp));
        terminal_id = we_cmmn_strndup (WE_MODID_PUS, tmp, len);
        PUS_MEM_FREE (tmp);
        PUS_MEM_FREE (realm);
        break;
      }
    }
    else 
      index_in_list = we_cmmn_strnchr (index_in_list, ';' ,strlen (index_in_list));
    if (index_in_list != NULL)
      index_in_list++;
    PUS_MEM_FREE (realm);
  }
  PUS_MEM_FREE (tmp_realm);
  return terminal_id;
}



#endif

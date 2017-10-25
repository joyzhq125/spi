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


























































































  
#include "we_cfg.h"
#include "we_def.h"
#include "we_int.h"
#include "we_mem.h"
#include "we_cmmn.h"
#include "we_chrt.h"
#include "we_file.h"
#include "we_pipe.h"
#include "we_log.h"
#include "we_lib.h"
#include "we_prsr.h"
#include "we_hdr.h"
#include "we_sock.h"

#include "Pus_Main.h"
#include "Pus_If.h"
#include "Pus_Sig.h"
#include "Pus_Cfg.h"
#include "Pus_Conn.h"
#include "Pus_Msg.h"
#include "Pus_Prh.h"
#include "Pus_Vrsn.h"
#include "Pus_Dlg.h"
#include "Pus_Sia.h"
#include "Pus_Cmmn.h"
#include "stk_if.h"

#ifdef PUS_CONFIG_HTTP_PUSH  
#include "Pus_Http.h"
#include "sec_if.h"
#include "sec_lib.h"
#endif

#ifdef PUS_CONFIG_MULTIPART 
#include "Pus_Mprt.h"
#endif





#define PUS_STATE_NOT_ACTIVE                         0
#define PUS_STATE_STARTED                            1
#define PUS_STATE_CLOSING_DOWN                       2
#define PUS_STATE_TERMINATED                         3


#define PUS_MAX_STREAMS                              10

#define PUS_CPI_HDR_DEFAULT_VALUE               "*"
#define PUS_CPI_HDR_ACCEPT_DEFAULT_VALUE        "application/vnd.wap.sia, text/vnd.wap.si"
#define PUS_CPI_HDR_ACCEPT_CHR_DEFAULT_VALUE    "UTF-8"             


























#define PUS_WHITELIST_BEHAVIOR_DISCARD                 0
#define PUS_WHITELIST_BEHAVIOR_NOTIFY                  1




typedef struct {
  int                    instance_id;
  int                    type;
  int                    handle;
  int                    module;
  long                   pos;
  long                   endpos;
  char                  *pathname;
  char                  *buf;
} pus_stream_t;







jmp_buf pus_jmp_buf;
int     pus_inside_run;


int     pus_security_level;
int     pus_support_SL;
int     Pus_Sia_default_net_id;
int     Pus_Sia_default_conn_type;
int     pus_whitelist_behavior;
char   *pus_terminal_id;
char   *pus_prov_termid_list;
char   *pus_whitelist_smsc;
char   *pus_whitelist_ip;
char   *pus_whitelist_domain;
char   *pus_whitelist_domain_ip;
char   *pus_blacklist_smsc;
char   *pus_blacklist_ip;

pus_user_t            *pus_user_list;
Pus_Connection_t      *Pus_Connection_table[PUS_CFG_MAX_CONNECTIONS];
pus_client_conn_t     *pus_client_conn_list; 
char                  *pus_clients_cpi_tag;
int                    pus_handle;  
Pus_Msg_list_t        *Pus_Msg_list;


















int    sir_lockout_timer;

#ifdef PUS_CONFIG_HTTP_PUSH  
pus_cpi_hdrs_value_t      pus_cpi_hdr_values;
#endif




static int             pus_module_state;
static int             pus_stk_module_state;
static int             pus_ubs_module_state;

static pus_stream_t    pus_stream_table[PUS_MAX_STREAMS];





static void
Pus_Main (Pus_Signal_t *sig);

static void
pus_terminate_pus_module (void);

static void
pus_memory_exception_handler (void);

static void
pus_free_all_resources (void);

static void
pus_close_all_streams (void);

static int
pus_stream_file_error (int error_code);

static int
pus_stream_pipe_error (int error_code);

static void
Pus_Main_handle_new_push (stk_recv_request_t *p);

static void
Pus_Main_handle_stream_notification (int handle, int event_type);

static void
pus_handle_SL_cache_response (stk_http_reply_t *p);

#ifdef PUS_CONFIG_HTTP_PUSH
static void
Pus_Main_calculate_cpi_tag (void);
#endif

static void
Pus_Main_handle_dns_lookup_response (we_socket_host_by_name_t *p);

static Pus_Msg_list_t*
Pus_Main_create_new_message (stk_recv_request_t *p);

static void
Pus_Main_delete_unhandled_push (Pus_Msg_list_t *msg);

static int
Pus_Main_validate_against_whitelist (we_sockaddr_t fromAddr, int connection_type);

static int
pus_check_smsc_addrs_against_list (char* whitelist, we_sockaddr_t fromAddr, int return_code );

static int
pus_check_ip_addrs_against_list (char* whitelist, we_sockaddr_t fromAddr, int return_code );

static int
Pus_Main_get_ip_address (const char *s, we_sockaddr_t *addr);

static int
Pus_Main_get_smsc_address (const char *s, we_sockaddr_t *addr);

static int
Pus_Main_check_for_BCD_number (int digit);

int
pus_compare_addresses (we_sockaddr_t *addr1, we_sockaddr_t *addr2);












void
pus_init (void)
{
  int i;

  WE_MEM_INIT (WE_MODID_PUS, pus_memory_exception_handler, FALSE);

  WE_SIGNAL_REG_FUNCTIONS (WE_MODID_PUS, pus_convert, pus_destruct);

  WE_SIGNAL_REGISTER_QUEUE (WE_MODID_PUS);

  WE_MODULE_IS_CREATED (WE_MODID_PUS, PUS_VERSION);

  
  Pus_Signal_init ();
  Pus_Msg_init ();
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
  Pus_Prh_init ();
  Pus_Dlg_init ();
#ifdef PUS_CONFIG_MULTIPART 
  Pus_Mprt_init ();
#endif
#ifdef PUS_CONFIG_HTTP_PUSH
  Pus_Http_init ();
#endif
  Pus_Sia_init ();
#endif

  
  WE_REGISTRY_SUBSCRIBE (WE_MODID_PUS,
                          1, 
                          (char*)Pus_Main_get_path_as_string (PUS_REG_PATH_ROOT),
                          NULL,
                          TRUE);

  Pus_Signal_register_dst (PUS_MODULE_MAIN, Pus_Main);

  pus_module_state = PUS_STATE_NOT_ACTIVE;

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, "PUS: Initialized\n"));

  for (i = 0; i < PUS_CFG_MAX_CONNECTIONS; i++) {
    Pus_Connection_table[i] = NULL;
  }

  pus_user_list = NULL;
  pus_client_conn_list = NULL;
  Pus_Msg_list = NULL;
  pus_security_level = 0;
  pus_support_SL = 0;
  pus_whitelist_smsc = NULL;
  pus_whitelist_ip = NULL;
  pus_whitelist_domain = NULL;
  pus_whitelist_domain_ip = NULL;
  pus_blacklist_smsc = NULL;
  pus_blacklist_ip = NULL;
  pus_whitelist_behavior = 0;

#ifdef PUS_CONFIG_HTTP_PUSH 
  pus_terminal_id = NULL;
  pus_prov_termid_list = NULL;
  pus_clients_cpi_tag = NULL;
  pus_cpi_hdr_values.accept = NULL;
  pus_cpi_hdr_values.accept_app_id = NULL;
  pus_cpi_hdr_values.accept_charset = NULL;
  pus_cpi_hdr_values.accept_language = NULL;
#endif
  
  WE_MODULE_START (WE_MODID_PUS, WE_MODID_STK, NULL, NULL, NULL);
  STKif_startInterface ();
  pus_stk_module_state = PUS_STATE_NOT_ACTIVE;

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES
  WE_MODULE_START (WE_MODID_PUS, WE_MODID_UBS, NULL, NULL, NULL);
  UBSif_startInterface ();
  pus_ubs_module_state = PUS_STATE_NOT_ACTIVE;
#endif
}







void
pus_terminate (void)
{
  
  
  pus_free_all_resources ();

  WE_MODULE_TERMINATE (WE_MODID_PUS, WE_MODID_STK);

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES
  WE_MODULE_TERMINATE (WE_MODID_PUS, WE_MODID_UBS);
#endif 
  
  pus_module_state = PUS_STATE_CLOSING_DOWN;
}








static void
Pus_Main (Pus_Signal_t *sig)
{
  switch (sig->type) 
  {
  case PUS_MAIN_SIG_OPEN_PUSH_CONNECTION:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS: received signal PUS_MAIN_SIG_OPEN_PUSH_CONNECTION\n"));
    Pus_Conn_handle_open_connection ((WE_UINT8)sig->u_param, sig->p_param);
    break;

  case PUS_MAIN_SIG_CLOSE_PUSH_CONNECTION:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS: received signal PUS_MAIN_SIG_CLOSE_PUSH_CONNECTION\n"));
    Pus_Conn_handle_close_connection ((WE_UINT8)sig->u_param, (WE_INT16)sig->i_param1);
    break;




  case PUS_MAIN_SIG_STK_CONNECTION_CREATED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS: received signal PUS_MAIN_SIG_STK_CONNECTION_CREATED\n"));
    Pus_Conn_handle_connection_created (sig->p_param);
    break;
    
  case PUS_MAIN_SIG_STK_CONNECTION_CLOSED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS: received signal PUS_MAIN_SIG_STK_CONNECTION_CLOSED\n"));
    Pus_Conn_handle_connection_closed (sig->p_param);
    break;

  case PUS_MAIN_SIG_RECV_REQUEST:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS: received signal PUS_MAIN_SIG_RECV_PUSH\n"));
    Pus_Main_handle_new_push ((stk_recv_request_t*)sig->p_param);
    break;

  case PUS_MAIN_SIG_HTTP_REPLY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS: received signal PUS_MAIN_SIG_RECV_PUSH\n"));
    pus_handle_SL_cache_response (sig->p_param);
    break;

  case PUS_MAIN_SIG_RESPONSE_FROM_CONTENT_ROUTING:
     WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS: received signal PUS_MAIN_SIG_RESPONSE_FROM_CONTENT_ROUTING\n"));
    pus_handle_content_send_ack (sig->i_param1, sig->u_param);
    break;

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES

  case PUS_MAIN_SIG_WIDGET_ACTION:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,"received PUS_MAIN_SIG_WIDGET_ACTION\n"));
    Pus_Cmmn_widget_action (sig->p_param);
    break;
#endif




  case PUS_MAIN_SIG_HOST_BY_NAME:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,"received PUS_MAIN_SIG_WIDGET_ACTION\n"));
    Pus_Main_handle_dns_lookup_response (sig->p_param);
    break;




  case PUS_SIG_NOTIFY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,"received PUS_SIG_NOTIFY\n"));
    Pus_Main_handle_stream_notification (sig->i_param1, sig->u_param);
    break;
    

  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, "PUS: received unexpected signal: %d\n", sig->type));
    break;
  }

  Pus_Signal_delete (sig);
}

int
pus_new_handle (void)
{
  return pus_handle++;
}

















void
pus_stk_status (we_module_status_t *p)
{
  
  if ((p->status == MODULE_STATUS_ACTIVE) && (p->modId == WE_MODID_STK)) {
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES
    if (pus_ubs_module_state == PUS_STATE_STARTED) {
      WE_REGISTRY_GET (WE_MODID_PUS, 0, "/PUS", NULL);
    }
#else
    WE_REGISTRY_GET (WE_MODID_PUS, 0, "/PUS", NULL);
#endif
    pus_stk_module_state = PUS_STATE_STARTED;
  }

    
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES
  else if ((p->status == MODULE_STATUS_ACTIVE) && (p->modId == WE_MODID_UBS)) {
    if (pus_stk_module_state == PUS_STATE_STARTED) {
      WE_REGISTRY_GET (WE_MODID_PUS, 0, "/PUS", NULL);
    }
    pus_ubs_module_state = PUS_STATE_STARTED;
  }
#endif

    
  else if ((p->status == MODULE_STATUS_TERMINATED) && (p->modId == WE_MODID_STK)) {
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES
    if (pus_ubs_module_state == PUS_STATE_TERMINATED)
      pus_terminate_pus_module ();
    else if (pus_module_state != PUS_STATE_CLOSING_DOWN)
      WE_MODULE_TERMINATE (WE_MODID_PUS, WE_MODID_UBS);
#else
    pus_terminate_pus_module ();
#endif
    pus_stk_module_state = PUS_STATE_TERMINATED;
  }

   
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES
  else if ((p->status == MODULE_STATUS_TERMINATED) && (p->modId == WE_MODID_UBS))
  {
    if (pus_stk_module_state == PUS_STATE_TERMINATED)
      pus_terminate_pus_module ();
    else if (pus_module_state != PUS_STATE_CLOSING_DOWN)
      WE_MODULE_TERMINATE (WE_MODID_PUS, WE_MODID_STK);
    pus_ubs_module_state = PUS_STATE_TERMINATED; 
  }
#endif
  

  
  PUS_MEM_FREE (p->modVersion);
  PUS_MEM_FREE (p);  
}




void
pus_reg_response (we_registry_response_t *p)
{
  we_registry_param_t param; 
  int path,key;
  pus_open_push_connection_t *p_conn; 

  while (WE_REGISTRY_RESPONSE_GET_NEXT (p, &param)) {
    path = Pus_Main_get_path_as_int (param.path);
    key = Pus_Main_get_key_as_int (param.key);
   if (path == -1 || key == -1)
      continue;
   switch (path)   
   {
   case PUS_REG_PATH_DEFAULT_CONN:
     if (pus_module_state != PUS_STATE_STARTED) {
       if (key == PUS_REG_KEY_SMS) {
         p_conn = PUS_MEM_ALLOCTYPE (pus_open_push_connection_t);
         p_conn->handle = (WE_INT16)pus_new_handle ();
         p_conn->connectionType = STK_CONNECTION_TYPE_SMS_DATA;
         p_conn->portNumber = PUS_CFG_UDP_DEFAULT_PUSH_PORT;
         p_conn->networkAccountId = param.value_i;
         Pus_Conn_handle_open_connection (WE_MODID_PUS, p_conn); 
       }
       else if (key == PUS_REG_KEY_UDP) {
         p_conn = PUS_MEM_ALLOCTYPE (pus_open_push_connection_t);
         p_conn->handle = (WE_INT16)pus_new_handle ();
         p_conn->connectionType = STK_CONNECTION_TYPE_WSP_CL;
         p_conn->portNumber = PUS_CFG_UDP_DEFAULT_PUSH_PORT;
         p_conn->networkAccountId = param.value_i;
         Pus_Conn_handle_open_connection (WE_MODID_PUS, p_conn); 
       }
       else if (key == PUS_REG_KEY_WTLS) {
         p_conn = PUS_MEM_ALLOCTYPE (pus_open_push_connection_t);
         p_conn->handle = (WE_INT16)pus_new_handle ();
         p_conn->connectionType = STK_CONNECTION_TYPE_WSP_CL_WTLS;
         p_conn->portNumber = PUS_CFG_WTLS_DEFAULT_PUSH_PORT;
         p_conn->networkAccountId = param.value_i;
         Pus_Conn_handle_open_connection (WE_MODID_PUS, p_conn); 
       }
#ifdef PUS_CONFIG_HTTP_PUSH
       else if (key == PUS_REG_KEY_HTTP) {
         p_conn = PUS_MEM_ALLOCTYPE (pus_open_push_connection_t);
         p_conn->handle = (WE_INT16)pus_new_handle ();
         p_conn->connectionType = STK_CONNECTION_TYPE_HTTP;
         p_conn->portNumber = PUS_CFG_HTTP_DEFAULT_PUSH_PORT; 
         p_conn->networkAccountId = param.value_i;
         Pus_Conn_handle_open_connection (WE_MODID_PUS, p_conn); 
       }
#endif
     }
     break;
     
   case PUS_REG_PATH_ROOT:
     if (key == PUS_REG_KEY_SECURITY_LEVEL) {
       pus_security_level = (int) param.value_i;
     }
     if (key == PUS_REG_KEY_SUPPORT_SL) {
       pus_support_SL = (int) param.value_i;
     }
     if (key == PUS_REG_KEY_TERMINAL_ID) {
       pus_terminal_id = we_cmmn_strdup (WE_MODID_PUS, (const char *)param.value_bv);
     }
     if (key == PUS_REG_KEY_PROVISND_TERMINAL_ID) {
       pus_prov_termid_list = we_cmmn_strdup (WE_MODID_PUS, (const char *)param.value_bv);
     }
     break;
     
   case PUS_REG_PATH_WHITELIST:
     if (key == PUS_REG_KEY_WHITELIST_IP) {
       PUS_MEM_FREE (pus_whitelist_ip);
       pus_whitelist_ip = we_cmmn_strdup (WE_MODID_PUS, (const char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_WHITELIST_SMSC) {
       PUS_MEM_FREE (pus_whitelist_smsc);
       pus_whitelist_smsc = we_cmmn_strdup (WE_MODID_PUS, (const char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_BLACKLIST_IP) {
       PUS_MEM_FREE (pus_blacklist_ip);
       pus_blacklist_ip = we_cmmn_strdup (WE_MODID_PUS, (const char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_BLACKLIST_SMSC) {
       PUS_MEM_FREE (pus_blacklist_smsc);
       pus_blacklist_smsc = we_cmmn_strdup (WE_MODID_PUS, (const char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_WHITELIST_DOMAIN) {
       PUS_MEM_FREE (pus_whitelist_domain);
       pus_whitelist_domain = we_cmmn_strdup (WE_MODID_PUS, (const char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_WHITELIST_BEHAVIOR) {
       pus_whitelist_behavior = param.value_i;
     }
     break;
     
   case PUS_REG_PATH_SIR:
     if (key == PUS_REG_KEY_SIR_CONNTYPE) {
       Pus_Sia_default_conn_type = (int) param.value_i;
     }
     else if (key == PUS_REG_KEY_SIR_NET_ID) {
       Pus_Sia_default_net_id = (int) param.value_i;
     }
     break;

#ifdef PUS_CONFIG_HTTP_PUSH 
   case PUS_REG_PATH_CPI_INFO:
     if (key == PUS_REG_KEY_ACCEPT) {
       PUS_MEM_FREE (pus_cpi_hdr_values.accept);
       pus_cpi_hdr_values.accept = we_cmmn_strdup (WE_MODID_PUS, (char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_ACCEPT_CHARSET) {
       PUS_MEM_FREE (pus_cpi_hdr_values.accept_charset);
       pus_cpi_hdr_values.accept_charset = we_cmmn_strdup (WE_MODID_PUS, (char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_ACCEPT_LANGUAGE) {
       PUS_MEM_FREE (pus_cpi_hdr_values.accept_language);
       pus_cpi_hdr_values.accept_language = we_cmmn_strdup (WE_MODID_PUS, (char *)param.value_bv);
     }
     else if (key == PUS_REG_KEY_ACCEPT_APP_ID) {
       PUS_MEM_FREE (pus_cpi_hdr_values.accept_app_id);
       pus_cpi_hdr_values.accept_app_id = we_cmmn_strdup (WE_MODID_PUS, (char *)param.value_bv);
     }
     break;
#endif
   
   default:
    break; 
   };
  }

  
#ifdef PUS_CONFIG_HTTP_PUSH
  if (pus_cpi_hdr_values.accept == NULL)
    pus_cpi_hdr_values.accept = we_cmmn_strdup (WE_MODID_PUS, PUS_CPI_HDR_ACCEPT_DEFAULT_VALUE);
  
  if (pus_cpi_hdr_values.accept_app_id == NULL)
    pus_cpi_hdr_values.accept_app_id = we_cmmn_strdup (WE_MODID_PUS, PUS_CPI_HDR_DEFAULT_VALUE);
  
  if (pus_cpi_hdr_values.accept_charset == NULL)
    pus_cpi_hdr_values.accept_charset = we_cmmn_strdup (WE_MODID_PUS, 
                                                         PUS_CPI_HDR_ACCEPT_CHR_DEFAULT_VALUE); 
  if (pus_cpi_hdr_values.accept_language == NULL)
    pus_cpi_hdr_values.accept_language = we_cmmn_strdup (WE_MODID_PUS, PUS_CPI_HDR_DEFAULT_VALUE);

  
  Pus_Main_calculate_cpi_tag ();
#endif
  
  if (pus_module_state != PUS_STATE_STARTED) {
    WE_MODULE_IS_ACTIVE (WE_MODID_PUS);
    pus_module_state = PUS_STATE_STARTED;
  }

  WE_REGISTRY_RESPONSE_FREE (WE_MODID_PUS, p);
}








void
Pus_ConnectionOpened (WE_UINT8 dst, int external_handle, int errorCode)
{
  Pus_Connection_opened_t   pcc;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                     *signal_buffer, *user_data;

  pcc.handle = (WE_INT16)external_handle;
  pcc.errorCode = (WE_INT16)errorCode;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  pus_cvt_connection_opened (&cvt_obj, &pcc);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (PUS_SIG_CONNECTION_OPENED, WE_MODID_PUS,
                                     dst, length);
  if (signal_buffer != NULL) 
  {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    pus_cvt_connection_opened (&cvt_obj, &pcc);

    WE_SIGNAL_SEND (signal_buffer);
  }
}




void
Pus_ConnectionClosed (WE_UINT8 dst, int external_handle, int errorCode)
{
  Pus_Connection_closed_t  pcc;
  we_dcvt_t               cvt_obj;
  WE_UINT16               length;
  void                    *signal_buffer, *user_data;

  pcc.handle = (WE_INT16)external_handle;
  pcc.errorCode = (WE_INT16)errorCode;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  pus_cvt_connection_closed (&cvt_obj, &pcc);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (PUS_SIG_CONNECTION_CLOSED, WE_MODID_PUS,
                                     dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    pus_cvt_connection_closed (&cvt_obj, &pcc);

    WE_SIGNAL_SEND (signal_buffer);
  }
}








void
pus_handle_file_notification (int handle, int event_type)
{
  pus_stream_t        *stream;
  int                  i;

  for (i = 0; i < PUS_MAX_STREAMS; i++) {
    if ((pus_stream_table[i].type == PUS_STREAM_FILE_READ) &&
        (pus_stream_table[i].handle == handle)) {
      stream = &(pus_stream_table[i]);

      
      if (event_type == WE_FILE_EVENT_READ)
        event_type = PUS_STREAM_EVENT_READ_READY;
      else 
        break;

      PUS_SIGNAL_SENDTO_IIU (stream->module, PUS_SIG_NOTIFY, handle, 
                             stream->instance_id, event_type);      
      break;
    }
  }
}

void
pus_handle_pipe_notification (int handle, int event_type)
{
  pus_stream_t        *stream;
  int                  i;

  for (i = 0; i < PUS_MAX_STREAMS; i++) {
    if ((pus_stream_table[i].type == PUS_STREAM_PIPE_READ) &&
        (pus_stream_table[i].handle == handle)) {
      stream = &(pus_stream_table[i]);

      
      if ((event_type == WE_PIPE_EVENT_WRITE))
        break;

      if (event_type == WE_PIPE_EVENT_CLOSED)
        event_type = PUS_STREAM_EVENT_CLOSED;
#ifdef NEW_PIPE_ERROR_EVENT 
      else if (event_type == WE_PIPE_EVENT_CLOSED_ERROR)
        event_type = PUS_STREAM_EVENT_CLOSED;
#endif
      else if (event_type == WE_PIPE_EVENT_READ)
        event_type = PUS_STREAM_EVENT_READ_READY;
      else {
        break;
      }
      
      PUS_SIGNAL_SENDTO_IIU (stream->module, PUS_SIG_NOTIFY, handle, 
                             stream->instance_id, event_type);
      break;
    }
  }
}




int
pus_stream_init (int module, int wid, int type,
                 void *data, long datalen,
                 const char *pathname)
{
  pus_stream_t *stream = NULL;
  int           i;

  for (i = 0; i < PUS_MAX_STREAMS; i++) {
    if (pus_stream_table[i].type == 0) {
      stream = &(pus_stream_table[i]);
      break;
    }
  }
  if (stream == NULL)
    return -1;

  stream->module = module;
  stream->instance_id = wid;
  stream->pos = 0;
  stream->endpos = datalen;

  switch (type) {
  case PUS_STREAM_MEM_READ:
    stream->buf = data;
    stream->handle = -1;
    break;

  case PUS_STREAM_FILE_READ:
    stream->handle = WE_FILE_OPEN (WE_MODID_PUS, pathname, WE_FILE_SET_RDONLY, 0);
    if (stream->handle < 0) {
      return -1;
    }
    stream->pathname = we_cmmn_strdup (WE_MODID_PUS, pathname);
    break;

  case PUS_STREAM_PIPE_READ:
    stream->handle = WE_PIPE_OPEN (WE_MODID_PUS, pathname);
    if (stream->handle < 0) {
      return -1;
    }
    stream->pathname = we_cmmn_strdup (WE_MODID_PUS, pathname);
    break;

  default:
    return -1;
  }
  stream->type = type;

  return i;
}

int
pus_stream_register_handle (int wid, int type, int handle, int module, char* path_name )
{
  pus_stream_t *stream = NULL;
  int              i;

  for (i = 0; i < PUS_MAX_STREAMS; i++) {
    if (pus_stream_table[i].type == 0) {
      stream = &(pus_stream_table[i]);
      break;
    }
  }
  if (stream == NULL)
    return -1;

  stream->type = type;
  stream->instance_id = wid;
  stream->handle = handle;
  stream->module = module;
  stream->pathname = path_name;

  return i;
}


long
pus_stream_read (int stream_handle, void *buf, long buflen)
{
  pus_stream_t *stream;
  long          n = 0;

  if ((stream_handle < 0) || (stream_handle >= PUS_MAX_STREAMS))
    return PUS_STREAM_ERROR;

  stream = &(pus_stream_table[stream_handle]);

  switch (stream->type) {
  case PUS_STREAM_MEM_READ:
    n = stream->endpos - stream->pos;
    n = MIN (n, buflen);
    if (n <= 0)
      return PUS_STREAM_ERROR_EOF;

    memcpy (buf, stream->buf + stream->pos, n);
    stream->pos += n;
    break;

  case PUS_STREAM_FILE_READ:
    n = WE_FILE_READ (stream->handle, buf, buflen);
    if (n < 0) {
      n = pus_stream_file_error (n);
      if (n == PUS_STREAM_ERROR_DELAYED) {
        WE_FILE_SELECT (stream->handle, WE_FILE_EVENT_READ);
      }
    }
    break;

  case PUS_STREAM_PIPE_READ:
    n = WE_PIPE_READ (stream->handle, buf, buflen);
    if (n < 0) {
      n = pus_stream_pipe_error (n);
      if (n == PUS_STREAM_ERROR_DELAYED) {
        WE_PIPE_POLL (stream->handle);
      }
    }
    break;

  default:
    return PUS_STREAM_ERROR;
  }

  return n;
}

static int
pus_stream_file_error (int error_code)
{
  int r;

  if (error_code == WE_FILE_ERROR_DELAYED)
    r = PUS_STREAM_ERROR_DELAYED;
  else if (error_code == WE_FILE_ERROR_EOF)
    r = PUS_STREAM_ERROR_EOF;
  else
    r = PUS_STREAM_ERROR;

  return r;
}

static int
pus_stream_pipe_error (int error_code)
{
  int r;

  if (error_code == WE_PIPE_ERROR_DELAYED)
    r = PUS_STREAM_ERROR_DELAYED;
  else if (error_code == WE_PIPE_ERROR_CLOSED)
    r = PUS_STREAM_ERROR_EOF;
  else
    r = PUS_STREAM_ERROR;

  return r;
}

void
pus_stream_close (int stream_handle)
{
  pus_stream_t *stream;

  if ((stream_handle < 0) || (stream_handle >= PUS_MAX_STREAMS))
    return;

  stream = &(pus_stream_table[stream_handle]);

  if (stream->buf != NULL) {
    PUS_MEM_FREE (stream->buf);
    stream->buf = NULL;
  }

  switch (stream->type) {
  case PUS_STREAM_PIPE_READ:
    WE_PIPE_CLOSE (stream->handle);
    if (stream->pathname != NULL) {
      WE_PIPE_DELETE (stream->pathname);
      PUS_MEM_FREE ((void *)stream->pathname);
      stream->pathname = NULL;
    }
    break;

  case PUS_STREAM_FILE_READ:
    WE_FILE_CLOSE (stream->handle);
    if (stream->pathname != NULL) {
      PUS_MEM_FREE ((void *)stream->pathname);
      stream->pathname = NULL;
    }
    break;
  }
  
  stream->type = 0;
}







static void
pus_memory_exception_handler (void)
{
  if (pus_inside_run) {
    longjmp (pus_jmp_buf, 1);
  }
}





static void
pus_terminate_pus_module (void)
{

  if (pus_module_state != PUS_STATE_CLOSING_DOWN)  
    pus_free_all_resources ();

  WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_PUS);
  Pus_Signal_terminate ();
  WE_MODULE_IS_TERMINATED (WE_MODID_PUS);
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_PUS, "PUS: Terminated\n"));
  WE_KILL_TASK (WE_MODID_PUS);
}




static void
pus_free_all_resources (void) 
{
  pus_user_t  *tmp;
  int          i;

  for (i = 0; i < PUS_CFG_MAX_CONNECTIONS; i++) {
    if (Pus_Connection_table[i] != NULL) {
      STKif_closeConnection (WE_MODID_PUS, Pus_Connection_table[i]->connection_id);
      PUS_MEM_FREE (Pus_Connection_table[i]);
      Pus_Connection_table[i] = NULL;
    }
  }

  while ((tmp = pus_user_list) != NULL) {
    for (i = 0; i < PUS_CFG_MAX_CONNECTIONS_PER_USER; i++) {
      if (tmp->user_conn[i] != NULL) {
        PUS_MEM_FREE (tmp->user_conn[i]);
        tmp->user_conn[i] = NULL;
      }
    }
    pus_user_list = tmp->next;
    PUS_MEM_FREE (tmp);
  }

  
#ifdef PUS_CONFIG_HTTP_PUSH
  PUS_MEM_FREE (pus_clients_cpi_tag);
  PUS_MEM_FREE (pus_cpi_hdr_values.accept);
  PUS_MEM_FREE (pus_cpi_hdr_values.accept_app_id);
  PUS_MEM_FREE (pus_cpi_hdr_values.accept_charset);
  PUS_MEM_FREE (pus_cpi_hdr_values.accept_language);
#endif

  Pus_Msg_delete_all_message_in_list ();

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
  Pus_Prh_terminate ();
  Pus_Dlg_terminate ();
  Pus_Sia_terminate ();
#endif

  






  
  PUS_MEM_FREE (pus_terminal_id);
  PUS_MEM_FREE (pus_prov_termid_list);
  PUS_MEM_FREE (pus_whitelist_smsc);
  PUS_MEM_FREE (pus_whitelist_ip);
  PUS_MEM_FREE (pus_whitelist_domain);
  PUS_MEM_FREE (pus_whitelist_domain_ip);
  PUS_MEM_FREE (pus_blacklist_smsc);
  PUS_MEM_FREE (pus_blacklist_ip);

  pus_close_all_streams();

}




static void
pus_close_all_streams (void)
{
  int i;

  for (i = 0; i < PUS_MAX_STREAMS; i++) {
    if (pus_stream_table[i].type != 0)
      pus_stream_close (i);
  }
}


















static void
Pus_Main_handle_new_push (stk_recv_request_t *p)
{
  int               match, len;
  Pus_Msg_list_t   *msg;
  char             *cmpr, *index, *tmp;

  msg = Pus_Main_create_new_message (p);

  if (p->parentId < 0) 
    msg->connection_type = Pus_Conn_get_connection_type_by_connection_id (p->connectionId);
  else 
  {
    msg->connection_type = STK_CONNECTION_TYPE_HTTP;
  }

  if (msg->connection_type == STK_CONNECTION_TYPE_WSP_CL_WTLS) 
 {
    STKif_closeConnection (WE_MODID_PUS, p->connectionId);
  }

  match = Pus_Main_validate_against_whitelist (p->fromAddr, msg->connection_type);

  if (match == PUS_BLACKLIST_MATCH) {
    Pus_Main_delete_unhandled_push (msg);
    PUS_MEM_FREE (p);
    return;
  }
  
  else if (match == PUS_WHITELIST_DNS_LOOKUP) {
    

    PUS_MEM_FREE (pus_whitelist_domain_ip);
    msg->next_msg = Pus_Msg_list;
    Pus_Msg_list = msg;
    tmp = pus_whitelist_domain;
    do {
      index = we_cmmn_strnchr (tmp, ';' ,strlen (tmp));
      if (index != NULL)
        len = (index-tmp);
      else 
          len = strlen (tmp);
      cmpr = we_cmmn_strndup (WE_MODID_PUS, (const char*) tmp, len);
      if (cmpr != NULL) {
        WE_SOCKET_GET_HOST_BY_NAME (WE_MODID_PUS, msg->wid, cmpr, WE_SOCKET_DEFAULT_NID); 
        PUS_MEM_FREE (cmpr);
        msg->dialog_id++; 

      }
      tmp = index;
      if (tmp)
        tmp++;
    } while (index != NULL);
    return; 
  }

  else if (match == PUS_WHITELIST_NO_MATCH) 
  {
    if (pus_whitelist_behavior == PUS_WHITELIST_BEHAVIOR_DISCARD) 
    {
      Pus_Main_delete_unhandled_push (msg);
      PUS_MEM_FREE (p);
      return;
    }
    else 
      msg->whitelist_result = PUS_WHITELIST_NO_MATCH;
  }
  else
    msg->whitelist_result = PUS_WHITELIST_MATCH;

  msg->state = PUS_STATE_WHITELIST_DONE;
  msg->next_msg = Pus_Msg_list;
  Pus_Msg_list = msg;

#ifdef PUS_CONFIG_HTTP_PUSH
  if (msg->connection_type == STK_CONNECTION_TYPE_HTTP) 
  {
    PUS_SIGNAL_SENDTO_I (PUS_MODULE_HTTP, PUS_HTTP_SIG_NEW_MESSAGE, msg->wid);
  }
  else
#endif
#ifdef PUS_CONFIG_MULTIPART 
  if ((msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_MIXED) ||
      (msg->content_type_int == WE_MIME_TYPE_MULTIPART_MIXED)) 
 {
    PUS_SIGNAL_SENDTO_I (PUS_MODULE_MPRT, PUS_MPRT_SIG_NEW_MESSAGE, msg->wid);
  }
  else
#endif
    
    PUS_SIGNAL_SENDTO_I (PUS_MODULE_MSG, PUS_MSG_SIG_NEW_MESSAGE, msg->wid);

  PUS_MEM_FREE (p);
}




static Pus_Msg_list_t*
Pus_Main_create_new_message (stk_recv_request_t *p)
{
  Pus_Msg_list_t   *msg = PUS_MEM_ALLOCTYPE (Pus_Msg_list_t);
  WE_UINT32        tmp;

  msg->conn_id = p->connectionId;
  msg->url = p->url;
  msg->parent_id = p->parentId;
  msg->content = p->content;
  msg->fromAddr = p->fromAddr;
  msg->headers = p->headers;
  msg->method = p->method;
  msg->req_id = p->requestId;
  msg->content_location = NULL;
  Pus_Cmmn_get_content_type (p->headers, &tmp);
  msg->content_type_int = (int)tmp;
  msg->content_uri = NULL;
  msg->dialog_id = 0;
  msg->wid = (WE_INT16)pus_new_handle ();
  msg->initiator_uri = NULL;
  msg->parsed_info.co_data = NULL;
  msg->parsed_info.si_id = NULL;
  msg->parsed_info.href = NULL;
  msg->parsed_info.text = NULL;
  msg->parsed_info.priority = -1;
  msg->next_msg = NULL;
  msg->routing_type = 0;
  msg->routing_identifier = NULL;
  msg->state = PUS_STATE_NEW_MSG;
  msg->whitelist_result = 0;
  msg->msg_data = NULL;
  msg->msg_len = 0;
  msg->buf_list = NULL;
#ifdef PUS_CONFIG_MULTIPART
  msg->mp = NULL;
#endif

  return msg;
}


static void
Pus_Main_delete_unhandled_push (Pus_Msg_list_t *msg)
{
  switch (msg->content->dataType) {
  case WE_CONTENT_DATA_RAW:
    PUS_MEM_FREE (msg->content->_u.data);
    break;
  case WE_CONTENT_DATA_PIPE:
    PUS_MEM_FREE (msg->content->_u.pipeName);
    break;
  case WE_CONTENT_DATA_FILE:
    PUS_MEM_FREE (msg->content->_u.pathName);
    break;
  }
  PUS_MEM_FREE (msg->content->contentType);
  PUS_MEM_FREE (msg->content);
  PUS_MEM_FREE (msg->headers);
  PUS_MEM_FREE (msg->url);
  PUS_MEM_FREE (msg);
}



















static int
Pus_Main_validate_against_whitelist (we_sockaddr_t fromAddr, int connection_type) 
{
  
  int result;

  if (connection_type == STK_CONNECTION_TYPE_SMS_DATA) { 
    if ((pus_whitelist_smsc == NULL) && (pus_blacklist_smsc == NULL)) {
      return PUS_WHITELIST_MATCH;
    }

    

    if (pus_whitelist_smsc != NULL) {
      if ((result = pus_check_smsc_addrs_against_list (pus_whitelist_smsc, fromAddr, PUS_WHITELIST_MATCH)) != 0)
        return result;    
    }

    
    if (pus_blacklist_smsc != NULL) {
      if ((result = pus_check_smsc_addrs_against_list (pus_blacklist_smsc, fromAddr, PUS_BLACKLIST_MATCH)) != 0)
        return result;
    }
 
    return PUS_WHITELIST_NO_MATCH;    
  }

  else  {
    if ((pus_whitelist_ip == NULL) && (pus_whitelist_domain == NULL) && (pus_blacklist_ip == NULL)) {
      return PUS_WHITELIST_MATCH;
    }

    

    if (pus_whitelist_ip != NULL) {
      if ((result = pus_check_ip_addrs_against_list (pus_whitelist_ip, fromAddr, PUS_WHITELIST_MATCH))!= 0)
        return result;    
    }

    
    if (pus_whitelist_domain_ip != NULL) {
      if ((result = pus_check_ip_addrs_against_list (pus_whitelist_domain_ip, fromAddr, PUS_WHITELIST_MATCH))!= 0)
        return result; 
    }

    
    if (pus_blacklist_ip != NULL) {
      if ((result = pus_check_ip_addrs_against_list (pus_blacklist_ip, fromAddr, PUS_BLACKLIST_MATCH))!= 0)
        return result;
    }

#ifdef PUS_CONFIG_HTTP_PUSH    
    if (connection_type == STK_CONNECTION_TYPE_HTTP) {
      

      if (pus_whitelist_domain != NULL) {
        return PUS_WHITELIST_DNS_LOOKUP;
      }
    }
#endif

    return PUS_WHITELIST_NO_MATCH;    
  }
}






static int
pus_check_ip_addrs_against_list (char* whitelist, we_sockaddr_t fromAddr, int return_code ) 
{
  char *cmpr, *index, *tmp;
  we_sockaddr_t addr;
  int  len;
  
  tmp = we_cmmn_skip_blanks (whitelist);
    do {
      
      index = we_cmmn_strnchr (tmp, ';' ,strlen (tmp));
      if (index == NULL)
        len = strlen (tmp);
      else {
        len = index - tmp;
        index++; 
      }
      cmpr = we_cmmn_strndup (WE_MODID_PUS, (const char*) tmp, len);
      if (Pus_Main_get_ip_address (cmpr, &addr)) {
        if (pus_compare_addresses (&fromAddr, &addr)) {
          PUS_MEM_FREE (cmpr);
          return return_code;
        }
      }
      tmp = index;
      if (tmp != NULL)
        tmp = we_cmmn_skip_blanks (tmp);
      PUS_MEM_FREE (cmpr);
    } while (index != NULL);
  return 0;
}





static int
pus_check_smsc_addrs_against_list (char* whitelist, we_sockaddr_t fromAddr, int return_code ) 
{
  char *cmpr, *index, *tmp;
  we_sockaddr_t addr;
  int current_addr_len = 0;
  
  tmp = whitelist;
    do {
      
      index = we_cmmn_strnchr (tmp, ';' ,strlen (tmp));
      current_addr_len = index ? (index - tmp) : strlen (tmp);
      cmpr = we_cmmn_strndup (WE_MODID_PUS, (const char*) tmp, current_addr_len);
      if (Pus_Main_get_smsc_address (cmpr, &addr)) {
        if (pus_compare_addresses (&fromAddr, &addr)) {
          PUS_MEM_FREE (cmpr);
          return return_code;
        }
      }
      tmp = index;
      tmp++;
      PUS_MEM_FREE (cmpr);
    } while (index);
  return 0;
}




int
pus_compare_addresses (we_sockaddr_t *addr1, we_sockaddr_t *addr2)
{
  int i;
  int m = addr1->addrLen;

  if (addr1->addrLen != addr2->addrLen)
    return FALSE;

  for (i = 0; i < m; i++) {
    if (addr1->addr[i] != addr2->addr[i])
      return FALSE;
  }

  return TRUE;
}






static int
Pus_Main_get_ip_address (const char *s, we_sockaddr_t *addr)
{
  int         i, j, len;
  int         tmpaddr[4];
  char        tmp[8];
  const char *q;

  if (strlen (s) == 0) {
    addr->addrLen = 0;

    return TRUE;
  }
  if (!ct_isdigit (s[0])) {
    return FALSE;
  }

  for (i = 0; i < 4; i++)
    tmpaddr[i] = 0;

  for (i = 0; i < 4; i++) {
    q = strchr (s, '.');
    if (q == NULL) {
      if (i != 3)
        return FALSE;
      len = strlen (s);
    }
    else
      len = q - s;
    if ((len == 0) || (len > 3))
      return FALSE;
    for (j = 0; j < len; j++) {
      if (!ct_isdigit (s[j]))
        return FALSE;
      tmp[j] = s[j];
    }
    tmp[len] = '\0';
    tmpaddr[i] = atoi (tmp);
    if (tmpaddr[i] > 255)
      return FALSE;
    if (q != NULL) {
      s = q + 1;
    }
  }

  for (i = 0; i < 4; i++) {
    addr->addr[i] = (WE_UINT8)tmpaddr[i];
  }
  addr->addrLen = 4;

  return TRUE;
}







static int
Pus_Main_get_smsc_address (const char *s, we_sockaddr_t *addr) 
{
  int first_digit, second_digit, i;
  int len; 
  
  if (s == NULL) 
    return FALSE; 
  
  len = strlen(s); 
  addr->addrLen = 0;

  if (len == 0) {
    return TRUE;
  }

  for (i = 0; *s != '\0' ;i++ ) {
    
    first_digit = *s;   
    s++;
    first_digit = Pus_Main_check_for_BCD_number (first_digit);
    second_digit = *s;  
    if (*s !='\0')
      s++;



    second_digit = Pus_Main_check_for_BCD_number (second_digit);
    

    addr->addr[i] = (unsigned char)((second_digit << 4) | first_digit);

    addr->addrLen++;
  }
  return TRUE;
}




static int
Pus_Main_check_for_BCD_number (int digit)
{
  if (digit == 0x2a)        
    digit = 0x0a;           
  else if (digit == 0x23)   
    digit = 0x0b;
  else if (digit == 0x61)   
    digit = 0x0c;
  else if (digit == 0x62)   
    digit = 0x0d;
  else if (digit == 0x63)   
    digit = 0x0e;
  else if (digit == '\0')   
                            
    digit = 0x0f;
  else                       
    digit -= '0';
  
  return digit;
}











static void
Pus_Main_handle_dns_lookup_response (we_socket_host_by_name_t *p)
{
  Pus_Msg_list_t* msg = pus_find_msg_by_id (p->requestId);

  if (msg == NULL) {
    PUS_MEM_FREE (p);
    return;
  }
  
  if (msg->state == PUS_STATE_NEW_MSG) {
    we_sockaddr_t *addr = PUS_MEM_ALLOCTYPE (we_sockaddr_t);
    addr->addrLen = p->addrLen;
    memcpy (addr->addr, p->addr, p->addrLen);
    if (pus_compare_addresses (&(msg->fromAddr), addr)) {
      
      msg->state = PUS_STATE_WHITELIST_DONE;
    } 
    
    PUS_MEM_FREE (addr);
    msg->dialog_id--;

    if (msg->dialog_id == 0) {
      if (pus_whitelist_behavior == PUS_WHITELIST_BEHAVIOR_DISCARD) {
        pus_delete_msg_from_list (msg->wid);
        PUS_MEM_FREE (p);
        return;
      }
      else {
        msg->whitelist_result = PUS_WHITELIST_NO_MATCH;
        msg->state = PUS_STATE_WHITELIST_DONE;
      }
    }

    if (msg->state == PUS_STATE_WHITELIST_DONE) {

#ifdef PUS_CONFIG_HTTP_PUSH
      if (msg->connection_type == STK_CONNECTION_TYPE_HTTP) {
        PUS_SIGNAL_SENDTO_I (PUS_MODULE_HTTP, PUS_HTTP_SIG_NEW_MESSAGE, msg->wid);
      }
      else
#endif
#ifdef PUS_CONFIG_MULTIPART 
      if ((msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_MIXED) ||
          (msg->content_type_int == WE_MIME_TYPE_MULTIPART_MIXED)) {
        PUS_SIGNAL_SENDTO_I (PUS_MODULE_MPRT, PUS_MPRT_SIG_NEW_MESSAGE, msg->wid);
      }
      else
#endif
        
        PUS_SIGNAL_SENDTO_I (PUS_MODULE_MSG, PUS_MSG_SIG_NEW_MESSAGE, msg->wid);
    }  
  }
  
  PUS_MEM_FREE (p);

}











static void
Pus_Main_handle_stream_notification (int handle, int event_type)
{
  void         *buf;
  long          n;
  
  if (event_type == PUS_STREAM_EVENT_READ_READY) {
    for (;;) {
      buf = PUS_MEM_ALLOC (512);
      n = pus_stream_read (handle, buf, 512);
      if (n < 0) {
        if (n != PUS_STREAM_ERROR_DELAYED) 
          pus_stream_close (handle);
        PUS_MEM_FREE (buf);
        break;
      }
      PUS_MEM_FREE (buf);
    }
  }
}








static void
pus_handle_SL_cache_response (stk_http_reply_t *p)
{
  int          stream_handle;
  void        *buf;
  long         n;

  PUS_MEM_FREE (p->newUrl);
  PUS_MEM_FREE (p->headers);


  if (p->content->dataType == WE_CONTENT_DATA_RAW) 
  {
    PUS_MEM_FREE (p->content->_u.data);
  }
  else if (p->content->dataType == WE_CONTENT_DATA_PIPE) 
  {
    stream_handle = pus_stream_init (PUS_MODULE_MAIN, 0, PUS_STREAM_PIPE_READ, 
                                     NULL, 0, p->content->_u.pipeName);
    buf = PUS_MEM_ALLOC (512);
    while ((n = pus_stream_read (stream_handle, buf, 512)) > 0);   
    if (n < 0) {
      if (n != PUS_STREAM_ERROR_DELAYED) 
        pus_stream_close (stream_handle);
      }
    PUS_MEM_FREE (buf);
  }

  PUS_MEM_FREE (p->content->contentType);
  if (p->content->dataType == WE_CONTENT_DATA_PIPE)
    PUS_MEM_FREE (p->content->_u.pipeName);
  else if (p->content->dataType == WE_CONTENT_DATA_FILE)
    PUS_MEM_FREE (p->content->_u.pathName);
  PUS_MEM_FREE (p->content);
  PUS_MEM_FREE (p);
}

#ifdef PUS_CONFIG_HTTP_PUSH
static void
Pus_Main_calculate_cpi_tag (void)
{
  char          *str = NULL, *old_str = NULL;
  int            success, i = 32;
  char           hash_result[20];
  char           tmp_b64[32];
    
  if (pus_clients_cpi_tag == NULL) {
    pus_clients_cpi_tag = PUS_MEM_ALLOC (5);
  }
  
  str = we_cmmn_strcat (WE_MODID_PUS, old_str, pus_cpi_hdr_values.accept);
  
  old_str = str;
  str = we_cmmn_strcat (WE_MODID_PUS, old_str, pus_cpi_hdr_values.accept_app_id);
  PUS_MEM_FREE (old_str);
  
  old_str = str;
  str = we_cmmn_strcat (WE_MODID_PUS, old_str, pus_cpi_hdr_values.accept_charset);
  PUS_MEM_FREE (old_str);

  old_str = str;
  str = we_cmmn_strcat (WE_MODID_PUS, old_str, pus_cpi_hdr_values.accept_language);
  PUS_MEM_FREE (old_str);

  success = SEC_ERR_OK + 1;
  success = SEClib_hash (SEC_HASH_SHA, (const unsigned char*)str, strlen (str), 
                           (unsigned char*)hash_result);

  if (success != SEC_ERR_OK)
  {
    PUS_MEM_FREE (pus_clients_cpi_tag);
    pus_clients_cpi_tag = NULL;
    PUS_MEM_FREE (str);
    return;
  }
  else 
    we_cmmn_base64_encode (hash_result, 20, tmp_b64, &i);

  memcpy (pus_clients_cpi_tag, tmp_b64, 4);
  pus_clients_cpi_tag [4] = '\0';

  PUS_MEM_FREE (str);
}
#endif

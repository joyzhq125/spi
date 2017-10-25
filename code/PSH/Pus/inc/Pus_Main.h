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









#ifndef _Pus_Main_h
#define _Pus_Main_h

#ifndef _we_core_h
#include "we_core.h"
#endif

#ifndef _we_mem_h
#include "we_mem.h"
#endif

#ifndef _we_lib_h
#include "we_lib.h"
#endif

#ifndef _Pus_Cfg_h
#include "Pus_Cfg.h"
#endif

#ifndef _Pus_Msg_h
#include "Pus_Msg.h"
#endif

#ifndef _Pus_Dlg_h
#include "Pus_Dlg.h"
#endif

#ifdef PUS_CONFIG_MULTIPART
#ifndef _Pus_Mprt_h
#include "Pus_Mprt.h"
#endif
#endif





#define PUS_MEM_ALLOC(x)      WE_MEM_ALLOC (WE_MODID_PUS, (x))
#define PUS_MEM_ALLOCTYPE(x)  WE_MEM_ALLOCTYPE (WE_MODID_PUS, x)
#define PUS_MEM_FREE(x)       WE_MEM_FREE (WE_MODID_PUS, (x))





#define PUS_MAIN_SIG_OPEN_PUSH_CONNECTION            PUS_MODULE_MAIN + 1
#define PUS_MAIN_SIG_CLOSE_PUSH_CONNECTION           PUS_MODULE_MAIN + 2
#define PUS_MAIN_SIG_STK_CONNECTION_CREATED          PUS_MODULE_MAIN + 3
#define PUS_MAIN_SIG_STK_CONNECTION_CLOSED           PUS_MODULE_MAIN + 4
#define PUS_MAIN_SIG_RECV_REQUEST                    PUS_MODULE_MAIN + 5
#define PUS_MAIN_SIG_HTTP_REPLY                      PUS_MODULE_MAIN + 6
#define PUS_MAIN_SIG_RESPONSE_FROM_CONTENT_ROUTING   PUS_MODULE_MAIN + 7
#define PUS_MAIN_SIG_WIDGET_ACTION                   PUS_MODULE_MAIN + 8
#define PUS_MAIN_SIG_HOST_BY_NAME                    PUS_MODULE_MAIN + 10

#define PUS_SIG_NOTIFY                               PUS_MODULE_MAIN + 11


#define PUS_SIG_DIALOG_RESPONSE                      PUS_MODULE_MAIN + 14
 
    











  
#define PUS_REG_PATH_CPI_INFO               0
#define PUS_REG_PATH_DEFAULT_CONN           1
#define PUS_REG_PATH_SIR                    2
#define PUS_REG_PATH_WHITELIST              3
#define PUS_REG_PATH_ROOT                   4


 
#define PUS_REG_KEY_SMS                     0
#define PUS_REG_KEY_UDP                     1
#define PUS_REG_KEY_HTTP                    2   
#define PUS_REG_KEY_WTLS                    3
  
#define PUS_REG_KEY_SECURITY_LEVEL          4
#define PUS_REG_KEY_SUPPORT_SL              5

#define PUS_REG_KEY_SIR_CONNTYPE            6
#define PUS_REG_KEY_SIR_NET_ID              7

#define PUS_REG_KEY_ACCEPT                  8
#define PUS_REG_KEY_ACCEPT_CHARSET          9
#define PUS_REG_KEY_ACCEPT_LANGUAGE         10
#define PUS_REG_KEY_ACCEPT_APP_ID           11

#define PUS_REG_KEY_WHITELIST_IP            12
#define PUS_REG_KEY_WHITELIST_SMSC          13
#define PUS_REG_KEY_BLACKLIST_IP            14
#define PUS_REG_KEY_BLACKLIST_SMSC          15
#define PUS_REG_KEY_WHITELIST_DOMAIN        16
#define PUS_REG_KEY_WHITELIST_BEHAVIOR      17

#define PUS_REG_KEY_TERMINAL_ID             18
#define PUS_REG_KEY_PROVISND_TERMINAL_ID    19

#define PUS_STREAM_PIPE_READ                1
#define PUS_STREAM_FILE_READ                2
#define PUS_STREAM_MEM_READ                 3

#define PUS_STREAM_EVENT_CLOSED             1        
#define PUS_STREAM_EVENT_READ_READY         2


#define PUS_STREAM_ERROR_DELAYED                  -1
#define PUS_STREAM_ERROR_EOF                      -2
#define PUS_STREAM_ERROR                          -3


#define PUS_WHITELIST_NO_MATCH                        1
#define PUS_WHITELIST_MATCH                           2
#define PUS_WHITELIST_DNS_LOOKUP                      3 
#define PUS_BLACKLIST_MATCH                           4 




typedef struct Pus_Msg_list_st           Pus_Msg_list_t;
#ifdef PUS_CONFIG_MULTIPART
typedef struct Pus_Mprt_st               Pus_Mprt_t;
#endif


typedef struct Pus_Connection_st {
  int                        connection_id;
  int                        nbr_of_users;
  int                        state;
  int                        bearer;
  WE_UINT16                 portNumber;
  WE_INT16                  connectionType;
  WE_INT32                  networkAccountId;
} Pus_Connection_t;

typedef struct pus_user_conn_info_st {
  int                        external_handle;
  Pus_Connection_t          *conn;
} pus_user_conn_info_t;

typedef struct pus_user_st {
  struct pus_user_st        *next;
  WE_UINT8                  module_id;
  int                        num_open_connections;
  pus_user_conn_info_t      *user_conn[PUS_CFG_MAX_CONNECTIONS_PER_USER];
} pus_user_t;


typedef struct pus_client_conn_st {
  struct pus_client_conn_st *next;

  int                        conn_id;
  WE_INT16                  connectionType;
  unsigned char             *assumed_cpi_tag;
  int                        network_account_id;
} pus_client_conn_t;

typedef struct pus_cpi_hdrs_value_st {
  char             *accept;
  char             *accept_charset;
  char             *accept_language;
  char             *accept_app_id;
} pus_cpi_hdrs_value_t;

typedef struct pus_data_st {
  struct pus_data_st         *next;
  unsigned char              *buf;
  int                         buf_len;
} pus_data_t;

struct Pus_Msg_list_st {   
  struct Pus_Msg_list_st    *next_msg;
  
  we_sockaddr_t             fromAddr;
  int                        method;
  char                      *headers;
  stk_content_t             *content;
  int                        connection_type;
  
  char                      *url;
  int                        whitelist_result;
  int                        dialog_id;
  int                        state;
  WE_INT16                  wid;
  int                        routing_type;
  char                      *routing_identifier;
  int                        req_id;
  int                        conn_id;
  int                        parent_id;
  int                        content_type_int;
  char                      *initiator_uri;
  char                      *content_uri;
  char                      *content_location;
  pus_prs_info_t             parsed_info;
  
  pus_data_t                *buf_list;    
  WE_INT32                  msg_len;
  unsigned char             *msg_data;
#ifdef PUS_CONFIG_MULTIPART 
  Pus_Mprt_t                *mp;
#endif
};




extern jmp_buf pus_jmp_buf;
extern int     pus_inside_run;










extern pus_user_t            *pus_user_list;
extern Pus_Connection_t      *Pus_Connection_table[PUS_CFG_MAX_CONNECTIONS];
extern pus_client_conn_t     *pus_client_conn_list; 
extern int                    pus_handle;
extern char                  *pus_clients_cpi_tag;
extern Pus_Msg_list_t        *Pus_Msg_list;


extern int                    pus_security_level;
extern int                    pus_support_SL;
extern int                    Pus_Sia_default_net_id;
extern int                    Pus_Sia_default_conn_type;
extern int                    pus_whitelist_behavior;
extern char                  *pus_terminal_id;
extern char                  *pus_prov_termid_list;
extern char                  *pus_whitelist_ip;
extern char                  *pus_whitelist_smsc;
extern char                  *pus_whitelist_domain;
extern char                  *pus_whitelist_domain_ip;
extern char                  *pus_blacklist_smsc;
extern char                  *pus_blacklist_ip;

extern int                    sir_lockout_timer;

#ifdef PUS_CONFIG_HTTP_PUSH  
extern pus_cpi_hdrs_value_t   pus_cpi_hdr_values;
#endif




void
pus_init (void);

void
pus_terminate (void);

void
pus_reg_response (we_registry_response_t *p);

void
pus_stk_status (we_module_status_t *p);

int
pus_new_handle (void);

void
pus_handle_file_notification (int handle, int event_type);

void
pus_handle_pipe_notification (int handle, int event_type);

int
pus_stream_init (int module, int wid, int type,
                 void *data, long datalen,
                 const char *pathname);

long
pus_stream_read (int stream_handle, void *buf, long buflen);

int
pus_stream_register_handle (int wid, int type, int handle, int owner, char* path_name );

void
pus_stream_close (int stream_handle);





void
Pus_ConnectionOpened (WE_UINT8 dst, int external_handle, int errorCode);

void
Pus_ConnectionClosed (WE_UINT8 dst, int external_handle, int errorCode);





void
pus_handle_widget_action (we_widget_action_t *p);

#endif

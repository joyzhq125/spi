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
 * we_core.h
 *
 * Created by Ingmar Persson
 *
 * Revision history:
 *
 */
#ifndef _we_core_h
#define _we_core_h

#ifndef _we_dcvt_h
#include "we_dcvt.h"
#endif
#ifndef _we_int_h
#include "We_Int.h"
#endif
#ifndef _we_env_h
#include "We_Env.h"
#endif

/**********************************************************************
 * Global WE Signals in Moblie Suite
 **********************************************************************/

#define WE_SIG_MODULE_START                  ((WE_MODID_FRW << 8) + 0x01)
#define WE_SIG_MODULE_TERMINATE              ((WE_MODID_FRW << 8) + 0x02)
#define WE_SIG_MODULE_STATUS                 ((WE_MODID_FRW << 8) + 0x03)
#define WE_SIG_MODULE_STATUS_NOTIFY          ((WE_MODID_FRW << 8) + 0x04)
#define WE_SIG_MODULE_EXECUTE_COMMAND        ((WE_MODID_FRW << 8) + 0x05)
                                              
#define WE_SIG_TIMER_SET                     ((WE_MODID_FRW << 8) + 0x06)
#define WE_SIG_TIMER_RESET                   ((WE_MODID_FRW << 8) + 0x07)
#define WE_SIG_TIMER_EXPIRED                 ((WE_MODID_FRW << 8) + 0x08)
                                              
#define WE_SIG_WIDGET_NOTIFY                 ((WE_MODID_FRW << 8) + 0x09)
#define WE_SIG_WIDGET_ACTION                 ((WE_MODID_FRW << 8) + 0x0A)
#define WE_SIG_WIDGET_USEREVT                ((WE_MODID_FRW << 8) + 0x0B)
#define WE_SIG_WIDGET_ACCESS_KEY             ((WE_MODID_FRW << 8) + 0x0C)
                                              
#define WE_SIG_PIPE_NOTIFY                   ((WE_MODID_FRW << 8) + 0x0D)
                                              
#define WE_SIG_SOCKET_CONNECT_RESPONSE       ((WE_MODID_FRW << 8) + 0x0E)
#define WE_SIG_SOCKET_NOTIFY                 ((WE_MODID_FRW << 8) + 0x0F)
#define WE_SIG_SOCKET_HOST_BY_NAME           ((WE_MODID_FRW << 8) + 0x10)
                                              
#define WE_SIG_FILE_NOTIFY                   ((WE_MODID_FRW << 8) + 0x11)
#define WE_SIG_FILE_PATH_PROPERTY_RESP       ((WE_MODID_FRW << 8) + 0x12)
#define WE_SIG_FILE_QUOTA_RESP               ((WE_MODID_FRW << 8) + 0x13)
                                              
#define WE_SIG_REG_SET                       ((WE_MODID_FRW << 8) + 0x14)
#define WE_SIG_REG_GET                       ((WE_MODID_FRW << 8) + 0x15)
#define WE_SIG_REG_DELETE                    ((WE_MODID_FRW << 8) + 0x16)
                                              
#define WE_SIG_REG_RESPONSE                  ((WE_MODID_FRW << 8) + 0x17)
                                              
#define WE_SIG_CONTENT_SEND                  ((WE_MODID_FRW << 8) + 0x18)
#define WE_SIG_CONTENT_SEND_ACK              ((WE_MODID_FRW << 8) + 0x19)
                                              
#define WE_SIG_TEL_MAKE_CALL_RESPONSE        ((WE_MODID_FRW << 8) + 0x1A)
#define WE_SIG_TEL_SEND_DTMF_RESPONSE        ((WE_MODID_FRW << 8) + 0x1B)
                                              
#define WE_SIG_PB_ADD_ENTRY_RESPONSE         ((WE_MODID_FRW << 8) + 0x1C)
                                              
#define WE_SIG_GET_ICONS                     ((WE_MODID_FRW << 8) + 0x1D)
#define WE_SIG_GET_ICONS_RESPONSE            ((WE_MODID_FRW << 8) + 0x1E)
                                              
#define WE_SIG_OBJECT_ACTION                 ((WE_MODID_FRW << 8) + 0x1F)
                                              
#define WE_DLG_OPEN_RESPONSE_SIG             ((WE_MODID_FRW << 8) + 0x20)
#define WE_DLG_SAVE_RESPONSE_SIG             ((WE_MODID_FRW << 8) + 0x21)
#define WE_SAS_SAVE_AS_RESPONSE_SIG          ((WE_MODID_FRW << 8) + 0x22)
                                              
#define WE_SIG_REG_SUBSCRIBE                 ((WE_MODID_FRW << 8) + 0x23)

#define WE_SIG_CRPT_ENCRYPT_PKC_RESPONSE     ((WE_MODID_FRW << 8) + 0x24)
#define WE_SIG_CRPT_DECRYPT_PKC_RESPONSE     ((WE_MODID_FRW << 8) + 0x25)
#define WE_SIG_CRPT_VERIFY_SIGN_RESPONSE     ((WE_MODID_FRW << 8) + 0x26)
#define WE_SIG_CRPT_COMPUTE_SIGN_RESPONSE    ((WE_MODID_FRW << 8) + 0x27)
#define WE_SIG_CRPT_GENERATE_KEY_RESPONSE    ((WE_MODID_FRW << 8) + 0x28)
#define WE_SIG_CRPT_DH_KEYEXCHANGE_RESPONSE  ((WE_MODID_FRW << 8) + 0x29)
                                              
#define WE_SIG_TIMER_SET_PERSISTENT          ((WE_MODID_FRW << 8) + 0x2A)
#define WE_SIG_TIMER_RESET_PERSISTENT        ((WE_MODID_FRW << 8) + 0x2B)
#define WE_SIG_TIMER_PERSISTENT_EXPIRED      ((WE_MODID_FRW << 8) + 0x2C)


/* if new signals are added here, remember to update WE_SIG_LAST_ID */

#define WE_SIG_LAST_ID                       ((WE_MODID_FRW << 8) + 0x2D)


/**********************************************************************
 * Type definitions
 **********************************************************************/

typedef struct {
  char*           path;
  char*           key;
  WE_UINT8       type;
  WE_INT32       value_i;
  unsigned char*  value_bv;
  WE_UINT16      value_bv_length;
} we_registry_param_t;

typedef struct {
  WE_UINT32 screenHandle;
  WE_UINT32 embeddedWindowHandle; 
} we_embedded_data_t;

/* content structure used in content routing */
typedef struct  {
  WE_UINT8               routingFormat;
  char*                   routingIdentifier;
  char*                   contentUrl;
  char*                   contentSource;
  char*                   contentParameters;
  char*                   contentHeaders;
  WE_INT32               networkAccountId;
  WE_UINT8               contentDataType;
  WE_UINT32              contentDataLength;
  unsigned char*          contentData;
  char*                   contentType;
  WE_UINT8               embeddedObjectExists;
  we_embedded_data_t    *embeddedObjectData;
} we_content_data_t;

/* content structure used in object action */
typedef struct {
  char*           mime_type;    /* The mime type of the content data to perform an action on. */
  WeResourceType data_type;    /* Tells if the data comes from a file, pipe or buffer. */
  WE_UINT32      data_len;     /* Only used if buffer data. */
  unsigned char*  data;         /* Only used if buffer data. */
  char*           src_path;     /* The name of the file or pipe. Not used if buffer data. */
  char*           default_name; /* Optional file name. For example a destination file name. */
  char*           content_type; /* Set to NULL if content type is same as mime_type.
                                   Otherwise, set to DRM type or other envelope type. */
} we_act_content_t;


typedef struct {
  WE_UINT8 type;
  union {
    char      *s_value;
    WE_INT32  i_value;
  }_u;
} we_mime_list_t;

typedef struct {
  WE_UINT32 mime_hash;
  char      *s_mime;
  WE_INT32  i_mime;
  WE_UINT8  resource_type;
  char      *resource_str_id;
  WE_UINT32 resource_int_id;
} we_icon_list_t;

/* Sockets */
typedef struct {
  WE_INT32 iSocketId;
  WE_INT16 iResult;
} St_WeSockConnectResponse;

/**********************************************************************
 * Signal structures that are connected to the signals above.
 **********************************************************************/

/* Module */
typedef struct {
  WE_UINT8           modId;  /* The module that shall be started. */
  char*               startOptions;
  WE_UINT8           contentDataExists;
  we_content_data_t* contentData;
  /* we_content_area_t contentArea;*/
} we_module_start_t;

typedef struct {
  WE_UINT8     modId;  /* The module that shall be terminated. */
} we_module_terminate_t;

typedef struct {
  WE_UINT8     status;
  WE_UINT8     modId;
  char*         modVersion; /* Only used when the module is activated */
} we_module_status_t;

typedef struct  {
  WE_UINT8           srcModId;
  char*               commandOptions;
  WE_UINT8           contentDataExists;
  we_content_data_t* contentData;
 /* we_content_area_t contentArea;*/
} we_module_execute_cmd_t;

/* Timer */
typedef struct {
  WE_UINT32    timerID;
  WE_UINT32    timerInterval;
} we_timer_set_t;

typedef struct {
  WE_UINT32    timerID;
} we_timer_reset_t;

typedef struct {
  WE_UINT32    timerID;
} we_timer_expired_t;

/* Widget */
typedef struct {
  WE_UINT32          handle;
  WeNotificationType notificationType;
} we_widget_notify_t;

typedef struct {
  WE_UINT32    handle;
  WE_UINT32    action;
} we_widget_action_t;

typedef struct {
  WE_UINT32    handle;
  WeEvent      event;
} we_widget_userevt_t;

typedef struct {
  WE_UINT32    handle;
  WE_UINT32    accessKey;
} we_widget_access_key_t;


/* Pipes */
typedef struct {
  WE_INT32 handle;
  WE_INT16 eventType;
} we_pipe_notify_t;


/* Sockets */
typedef struct {
  WE_INT32 socketId;
  WE_INT16 result;
} we_socket_connect_response_t;

typedef struct {
  WE_INT32 socketId;
  WE_INT16 eventType;
} we_socket_notify_t;

typedef struct {
  WE_INT32     requestId;
  WE_INT16     result;
  WE_INT16     addrLen;
  unsigned char addr[16];
} we_socket_host_by_name_t;


/* File */
typedef struct {
  WE_INT32 fileHandle;
  WE_INT16 eventType;
} we_file_notify_t;

typedef struct {
  WE_INT32            wid;
  WE_UINT8            param_exist;
  we_path_property_t *p;
} we_file_path_property_resp_t;

typedef struct {
  WE_INT32          wid;
  WE_UINT8          param_exist;
  we_quota_t       *p;
} we_file_quota_resp_t;


/* Registry */
/* WE_SIG_REG_GET, WE_SIG_REG_DELETE */
typedef struct {
  WE_INT16 wid;
  char*     path;
  char*     key;
} we_registry_identifier_t;

/* WE_SIG_REG_SUBSCRIBE */
typedef struct {
  WE_INT16 wid;
  WE_BOOL  add;
  char*     path;
  char*     key;
} we_registry_subscription_t;

/* WE_SIG_REG_SET, WE_SIG_REG_RESPONSE */
typedef struct {
  WE_INT16        wid;
  char*            path;
  unsigned char*   next_element;
  unsigned char*   buffer_start;
  unsigned char*   buffer_end;
} we_registry_response_t;

/* Content routing */
typedef struct {
  WE_UINT8           destModId;
  WE_UINT8           sendAck;
  WE_UINT8           wid;
  WE_UINT8           useObjectAction;
  WE_UINT8           contentDataExists;
  char*               startOptions;
  we_content_data_t* contentData;
  /* we_content_area_t contentArea;*/
} we_content_send_t;

typedef struct {
  WE_UINT8  wid;
  WE_UINT8  status;
  WE_BOOL   caller_owns_pipe; /* caller of content rotuing has responsibility of the pipe */
                               /* might be TRUE when status != WE_CONTENT_STATUS_SUCCEED */
} we_content_send_ack_t;

/* Telephone */
typedef struct {
  WE_UINT16 telId;
  WE_INT16  result;
} we_tel_make_call_resp_t;

typedef struct {
  WE_UINT16 telId;
  WE_INT16  result;
} we_tel_send_dtmf_resp_t;

/* PhoneBook */
typedef struct {
  WE_UINT16 pbId;
  WE_INT16  result;
} we_pb_add_entry_resp_t;

/* Public Key Cryptography */
typedef struct {
  WE_INT16        wid;
  WE_INT16        result;  
  unsigned char   *buf;
  WE_INT16        bufLen;
} we_crpt_encrypt_pkc_resp_t;

typedef struct {
  WE_INT16        wid;
  WE_INT16        result;
  unsigned char   *buf;
  WE_INT16        bufLen;
} we_crpt_decrypt_pkc_resp_t;

typedef struct {
  WE_INT16        wid;
  WE_INT16        result;
} we_crpt_verify_signature_resp_t;

typedef struct {
  WE_INT16        wid;
  WE_INT16        result;
  unsigned char   *sig;
  WE_INT16        sigLen;
} we_crpt_compute_signature_resp_t;

typedef struct {
  WE_INT16           wid;
  WE_INT16           result;
  WE_UINT8           pkcAlg;
  we_crpt_pub_key_t  pubKey;
  we_crpt_priv_key_t privKey;
} we_crpt_generate_keypair_resp_t;

typedef struct {
  WE_INT16        wid;
  WE_INT16        result;
  unsigned char   *publicValue; 
  WE_INT16        publicValueLen;
  unsigned char   *secretValue;
  WE_INT16        secretLen;
} we_crpt_dh_keyexchange_resp_t;

typedef struct  {
  we_act_content_t    content;
  WE_INT32            excl_act_strings_cnt;
  char               **excl_act_strings;
} frw_object_action_t;


typedef struct {
  WE_UINT8         src;
  WE_UINT32        wid;
  WE_UINT16        n_items;
  we_mime_list_t  *mime_list;
}frw_get_icon_map_table_t;

typedef struct {
  WE_UINT32        wid;
  WE_UINT16        n_items;
  we_icon_list_t  *icon_list;
} frw_get_icons_resp_t;

typedef struct {
  WE_UINT32              wid;        /* operation identifier */
  long                    status;    /* status of operation */
  char*                   file_name;
} we_dlg_save_response_sig_t;

typedef struct {
  WE_UINT32              wid;        /* operation identifier */
  long                    status;    /* status of operation */
  char*                   file_name;
} we_dlg_open_response_sig_t;

typedef struct {
  WE_UINT32              wid;        /* operation identifier */
  long                    status;    /* status of operation */
  char*                   file_name;
} we_sas_save_as_response_sig_t;

/**********************************************************************
 * Conversion routines for signal structs
 **********************************************************************/

int
we_cvt_content_data (we_dcvt_t *obj, we_content_data_t *p);

int
we_cvt_act_content(we_dcvt_t *obj, we_act_content_t *p);

int
we_cvt_module_start (we_dcvt_t *obj, we_module_start_t *p);

int
we_cvt_module_terminate (we_dcvt_t *obj, we_module_terminate_t *p);

int
we_cvt_module_status (we_dcvt_t *obj, we_module_status_t *p);

int
we_cvt_module_execute_cmd (we_dcvt_t *obj, we_module_execute_cmd_t *p);

int
we_cvt_timer_set (we_dcvt_t *obj, we_timer_set_t *p);

int
we_cvt_timer_reset (we_dcvt_t *obj, we_timer_reset_t *p);

int
we_cvt_timer_expired (we_dcvt_t *obj, we_timer_expired_t *p);

int
we_cvt_wid_notify (we_dcvt_t *obj, we_widget_notify_t *p);

int
we_cvt_wid_action (we_dcvt_t *obj, we_widget_action_t *p);

int
we_cvt_wid_user_event (we_dcvt_t *obj, we_widget_userevt_t *p);

int
we_cvt_wid_access_key (we_dcvt_t *obj, we_widget_access_key_t *p);

int
we_cvt_pipe_notify (we_dcvt_t *obj, we_pipe_notify_t *p);

int
we_cvt_file_notify (we_dcvt_t *obj, we_file_notify_t *p);

int
we_cvt_file_path_property_resp (we_dcvt_t *obj, we_file_path_property_resp_t *p);

int
we_cvt_file_quota_resp (we_dcvt_t *obj, we_file_quota_resp_t *p);

int
we_cvt_socket_connect_response (we_dcvt_t *obj, we_socket_connect_response_t *p);

int
we_cvt_socket_notify (we_dcvt_t *obj, we_socket_notify_t *p);

int
we_cvt_socket_host_by_name (we_dcvt_t *obj, we_socket_host_by_name_t *p);

int
we_cvt_registry_identifier (we_dcvt_t *obj, we_registry_identifier_t *p);

int
we_cvt_registry_subscription (we_dcvt_t *obj, we_registry_subscription_t *p);

int
we_cvt_content_send (we_dcvt_t *obj, we_content_send_t *p);

int
we_cvt_content_send_ack (we_dcvt_t *obj, we_content_send_ack_t *p);

int
we_cvt_tel_make_call_resp (we_dcvt_t *obj, we_tel_make_call_resp_t *p);

int
we_cvt_tel_send_dtmf_resp (we_dcvt_t *obj, we_tel_send_dtmf_resp_t *p);

int
we_cvt_pb_add_entry_resp (we_dcvt_t *obj, we_pb_add_entry_resp_t *p);

int
we_cvt_crpt_encrypt_pkc_resp (we_dcvt_t *obj, 
                               we_crpt_encrypt_pkc_resp_t *p);

int
we_cvt_crpt_decrypt_pkc_resp (we_dcvt_t *obj, 
                               we_crpt_decrypt_pkc_resp_t *p);

int
we_cvt_crpt_verify_signature_resp (we_dcvt_t *obj, 
                                    we_crpt_verify_signature_resp_t *p);

int
we_cvt_crpt_compute_signature_resp (we_dcvt_t *obj, 
                                     we_crpt_compute_signature_resp_t *p);

int
we_cvt_crpt_generate_keypair_resp (we_dcvt_t *obj, 
                                    we_crpt_generate_keypair_resp_t *p);

int
we_cvt_crpt_dh_keyexchange_resp (we_dcvt_t *obj, 
                                  we_crpt_dh_keyexchange_resp_t *p);

int
we_cvt_icon_list(we_dcvt_t *obj, we_icon_list_t *p);

int
we_cvt_get_icons_resp (we_dcvt_t *obj, frw_get_icons_resp_t *p);

int
we_cvt_object_action(we_dcvt_t *obj, frw_object_action_t *p);

int
we_cvt_open_dlg_response (we_dcvt_t *obj, we_dlg_open_response_sig_t *p);

int
we_cvt_save_dlg_response (we_dcvt_t *obj, we_dlg_save_response_sig_t *p);

int
we_cvt_save_as_response (we_dcvt_t *obj, we_sas_save_as_response_sig_t *p);

/**********************************************************************
 * Module status
 **********************************************************************/

#define MODULE_STATUS_NULL          0
#define MODULE_STATUS_STANDBY       1
#define MODULE_STATUS_CREATED       2
#define MODULE_STATUS_ACTIVE        3
#define MODULE_STATUS_SHUTTING_DOWN 4
#define MODULE_STATUS_TERMINATED    5



/**********************************************************************
 * Signaling macros
 **********************************************************************/

#define WE_SIGNAL_REGISTER_QUEUE    TPIa_SignalRegisterQueue

#define WE_SIGNAL_DEREGISTER_QUEUE  TPIa_SignalDeregisterQueue

#define WE_SIGNAL_QUEUE_LENGTH      TPIa_SignalQueueLength

#define WE_SIGNAL_REG_FUNCTIONS     we_signal_reg_functions

#define WE_SIGNAL_CREATE            we_signal_create

#define WE_SIGNAL_SEND              we_signal_send

#define WE_SIGNAL_RETRIEVE          we_signal_retrieve

#define WE_SIGNAL_DESTRUCT          we_signal_destruct

#define WE_SIGNAL_GET_SIGNAL_ID     we_signal_get_signal_id

#define WE_SIGNAL_GET_SOURCE        we_signal_get_source

#define WE_SIGNAL_GET_DESTINATION   we_signal_get_destination

#define WE_SIGNAL_GET_USER_DATA     we_signal_get_user_data


/**********************************************************************
 * Type definitions to signaling:
 **********************************************************************/

typedef void* convert_function_t (WE_UINT8 module, WE_UINT16 signal, void* buffer);
typedef void receive_function_t (WE_UINT8 src_module, WE_UINT16 signal, void* p);
typedef void destruct_function_t (WE_UINT8 module, WE_UINT16 signal, void* p);


/**********************************************************************
 * Time/Timer macros
 **********************************************************************/

#define WE_TIME_ZONE_UNKNOWN        TPI_TIME_ZONE_UNKNOWN


#define WE_TIME_GET_CURRENT         TPIa_timeGetCurrent

#define WE_TIME_GET_TIME_ZONE       TPIa_timeGetTimeZone

#define WE_TIME_GET_SECURE_CLOCK    TPIa_timeSecureClock

#define WE_TIME_GET_TICKS           TPIa_timeGetTicks

#define WE_TIMER_SET                we_timer_set

#define WE_TIMER_RESET              we_timer_reset

#define WE_TIMER_SET_PERSISTENT     we_timer_set_persistent

#define WE_TIMER_RESET_PERSISTENT   we_timer_reset_persistent


/**********************************************************************
 * Module macros
 **********************************************************************/

#define WE_MODULE_IS_CREATED       we_module_is_created

#define WE_MODULE_IS_ACTIVE        we_module_is_active

#define WE_MODULE_IS_TERMINATED    we_module_is_terminated

#define WE_MODULE_START            we_module_start

#define WE_MODULE_TERMINATE        we_module_terminate

#define WE_KILL_TASK               TPIa_killTask


/**********************************************************************
 * Registry macros
 **********************************************************************/

#define WE_REGISTRY_TYPE_INT       0
#define WE_REGISTRY_TYPE_STR       1
#define WE_REGISTRY_TYPE_DELETED   2


#define WE_REGISTRY_SET_INIT           we_registry_set_init

#define WE_REGISTRY_SET_PATH           we_registry_set_path

#define WE_REGISTRY_SET_ADD_KEY_INT    we_registry_set_add_key_int

#define WE_REGISTRY_SET_ADD_KEY_STR    we_registry_set_add_key_str

#define WE_REGISTRY_SET_COMMIT         we_registry_set_commit

#define WE_REGISTRY_GET                we_registry_get

#define WE_REGISTRY_DELETE             we_registry_delete

#define WE_REGISTRY_SUBSCRIBE          we_registry_subscribe

#define WE_REGISTRY_RESPONSE_GET_NEXT  we_registry_response_get_next

#define WE_REGISTRY_RESPONSE_FREE      we_registry_response_free

/**********************************************************************
 * Content Routing macros
 **********************************************************************/

#define WE_ROUTING_TYPE_MIME           1
#define WE_ROUTING_TYPE_EXT            2
#define WE_ROUTING_TYPE_PAPPID         3
#define WE_ROUTING_TYPE_SCHEME         4

#define WE_CONTENT_DATA_RAW            0
#define WE_CONTENT_DATA_FILE           1
#define WE_CONTENT_DATA_PIPE           2
#define WE_CONTENT_DATA_NONE           3

#define WE_CONTENT_STATUS_SUCCEED      0
#define WE_CONTENT_STATUS_FAILED       1

#define WE_CONTENT_SEND                we_content_send

/**********************************************************************
 * Object Action macros
 **********************************************************************/
#define WE_OBJECT_ACTION               we_object_action

/**********************************************************************
 * Icon macros
 **********************************************************************/
#define WE_GET_ICONS                   we_get_icons

/**********************************************************************
 * Exported messaging functions:
 **********************************************************************/

void
we_signal_reg_functions (WE_UINT8 module, convert_function_t *cf,
                                            destruct_function_t *df);

void*
we_signal_create (WE_UINT16 sigUid, WE_UINT8 srcModId, WE_UINT8 destModId,
                   WE_UINT16 userDataLength);

void
we_signal_send (void* buffer);

int
we_signal_retrieve (WE_UINT8 modId, receive_function_t *rf);

void
we_signal_destruct (WE_UINT8 module, WE_UINT16 signal, void* p);

WE_UINT16
we_signal_get_signal_id (void* buffer);

WE_UINT8
we_signal_get_source (void* buffer);

WE_UINT8
we_signal_get_destination (void* buffer);

void*
we_signal_get_user_data (void* buffer, WE_UINT16* userDataLength);


/**********************************************************************
 * Exported timer functions:
 **********************************************************************/

void
we_timer_set (WE_UINT8 modId, int timerID, WE_UINT32 msInterval);

void
we_timer_reset (WE_UINT8 modId, int timerID);

void
we_timer_set_persistent (WE_UINT8 modId, int timerID, WE_UINT32 date_time);

void
we_timer_reset_persistent (WE_UINT8 modId, int timerID);

/**********************************************************************
 * Exported module functions:
 **********************************************************************/

void
we_module_is_created (WE_UINT8 module_src, char* modVersion);

void
we_module_is_active (WE_UINT8 module_src);

void
we_module_is_terminated (WE_UINT8 module_src);

void
we_module_start (WE_UINT8 module_src, WE_UINT8 module, char* startOptions,
                  we_content_data_t* contentData, void* contentArea);

void
we_module_terminate (WE_UINT8 module_src, WE_UINT8 module);


/**********************************************************************
 * Exported registry functions:
 **********************************************************************/

void*
we_registry_set_init (WE_UINT8 modId);

void
we_registry_set_path (void* handle, char* path);

void
we_registry_set_add_key_int (void* handle, char* key, WE_INT32 value);

void
we_registry_set_add_key_str (void* handle, char* key, unsigned char* value,
                              WE_UINT16 valueLength);

void
we_registry_set_commit (void* handle);

void
we_registry_get (WE_UINT8 modId, int wid, char* path, char* key);

void
we_registry_delete (WE_UINT8 modId, char* path, char* key);

void
we_registry_subscribe (WE_UINT8 modId, int wid, char* path, char* key, WE_BOOL add);

int
we_registry_response_get_next (we_registry_response_t* handle, we_registry_param_t* param);

void
we_registry_response_free (WE_UINT8 modId, we_registry_response_t* handle);


/**********************************************************************
 * Exported content routing functions:
 **********************************************************************/

void
we_content_send (WE_UINT8 src_modId, WE_UINT8 dest_modId, char* startOptions,
                  we_content_data_t* contentData, int sendAck, WE_UINT8 wid, int useObjectAction);


/**********************************************************************
 * Exported Icon functions:
 **********************************************************************/
#define WE_MIME_TYPE_STR 0x0001
#define WE_MIME_TYPE_INT 0x0002

#define WE_ICON_RESOURCE_TYPE_PREDEFINED 0x0001
#define WE_ICON_RESOURCE_TYPE_FILE       0x0002

void
we_get_icons (WE_UINT8 src_modId, WE_UINT32 wid, 
               WE_UINT16 n_items, we_mime_list_t mime_list[]);


/**********************************************************************
 * Exported Object Action functions:
 **********************************************************************/

void
we_object_action(WE_UINT8                src_modId, 
                  const we_act_content_t* content,
                  const char*              excl_act_strings[],
                  WE_INT32                excl_act_strings_cnt);

/**********************************************************************
 * Only used internally:
 **********************************************************************/

  /* 6 bytes = 2 bytes signal + 1 byte source + 1 byte dest + 2 bytes data length. */
#define WE_SIGNAL_HEADER_LENGTH   6


#define WE_REG_SET_TYPE_PATH             0
#define WE_REG_SET_TYPE_INT              1
#define WE_REG_SET_TYPE_STR              2
#define WE_REG_SET_TYPE_DELETED          3

#define WE_REG_MODID_INTEGRATION         -1

void*
we_registry_set_int_commit (void* handle);

void
we_registry_set_free_handle (WE_UINT8 modId, void* handle);

#endif

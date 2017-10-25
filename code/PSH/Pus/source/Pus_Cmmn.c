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










#include "we_def.h"
#include "we_hdr.h"
#include "we_cfg.h"

#include "Pus_Cfg.h"
#include "Pus_Dlg.h"
#include "Pus_Rc.h"
#include "Pus_Cmmn.h"
#include "Pus_Sia.h"




typedef struct pus_dialog_st {
  struct pus_dialog_st *next;

  int        type;
  int        pus_module;
  Pus_Dlg_t *dlg;
} pus_dialog_t;




static pus_dialog_t   *pus_dialogs;




#define PUS_REG_PATH_HASH_TABLE_SIZE                 7
#define PUS_REG_PATH_STRING_TABLE_SIZE               5

static const we_strtable_entry_t pus_reg_path_string_table[] = {
  {"/pus/cpi_info",             PUS_REG_PATH_CPI_INFO},
  {"/pus/def_conn",             PUS_REG_PATH_DEFAULT_CONN},
  {"/pus/sir",                  PUS_REG_PATH_SIR},
  {"/pus/wlist",                PUS_REG_PATH_WHITELIST},
  {"/pus",                      PUS_REG_PATH_ROOT}
};

static const unsigned char pus_reg_path_hash_table[] = {
   255, 4, 3, 255, 1, 2, 0
};

static const we_strtable_info_t pus_reg_path_sti = {
  pus_reg_path_hash_table,
  PUS_REG_PATH_HASH_TABLE_SIZE,
  WE_TABLE_SEARCH_INDEXING,
  pus_reg_path_string_table,
  PUS_REG_PATH_STRING_TABLE_SIZE
}; 

#define PUS_REG_KEY_HASH_TABLE_SIZE                 31
#define PUS_REG_KEY_STRING_TABLE_SIZE               20

static const we_strtable_entry_t pus_reg_key_string_table[] = {
  {"sms_nwa",           PUS_REG_KEY_SMS},
  {"udp_nwa",           PUS_REG_KEY_UDP},
  {"http_nwa",          PUS_REG_KEY_HTTP},
  {"wtls_nwa",          PUS_REG_KEY_WTLS},
  {"security",          PUS_REG_KEY_SECURITY_LEVEL},
  {"supprtsl",          PUS_REG_KEY_SUPPORT_SL},
  {"conntype",          PUS_REG_KEY_SIR_CONNTYPE},
  {"net_id",            PUS_REG_KEY_SIR_NET_ID},
  {"accept",            PUS_REG_KEY_ACCEPT},
  {"ac_chrs",           PUS_REG_KEY_ACCEPT_CHARSET},
  {"ac_lang",           PUS_REG_KEY_ACCEPT_LANGUAGE},
  {"ac_appid",          PUS_REG_KEY_ACCEPT_APP_ID},
  {"wlstip",            PUS_REG_KEY_WHITELIST_IP},
  {"wlstsmsc",          PUS_REG_KEY_WHITELIST_SMSC},
  {"blstip",            PUS_REG_KEY_BLACKLIST_IP},
  {"blstsmsc",          PUS_REG_KEY_BLACKLIST_SMSC},
  {"wlstname",          PUS_REG_KEY_WHITELIST_DOMAIN},
  {"behavior",          PUS_REG_KEY_WHITELIST_BEHAVIOR},
  {"term_id",           PUS_REG_KEY_TERMINAL_ID},
  {"prov_tid",           PUS_REG_KEY_PROVISND_TERMINAL_ID}
};

static const unsigned char pus_reg_key_hash_table[] = {
  255, 11, 8, 4, 13, 18, 255, 255, 5, 9,
  255, 0, 2, 255, 6, 255, 17, 16, 15, 1,
  255, 7, 12, 255, 10, 255, 255, 3, 14, 255,
  19
};


static const we_strtable_info_t pus_reg_key_sti = {
  pus_reg_key_hash_table,
  PUS_REG_KEY_HASH_TABLE_SIZE,  
  WE_TABLE_SEARCH_INDEXING,
  pus_reg_key_string_table,
  PUS_REG_KEY_STRING_TABLE_SIZE
};













void
Pus_Cmmn_get_content_type (char *headers, WE_UINT32 *content_type_int)
{
  we_hdr_t  *hdr = we_hdr_create (WE_MODID_PUS);
  we_hdr_add_headers_from_text (hdr, headers);

  if (!we_hdr_get_uint32 (hdr, WE_HDR_CONTENT_TYPE, content_type_int)) {
    *content_type_int = WE_MIME_TYPE_UNKNOWN;
  }
  we_hdr_delete (hdr);
}







int
Pus_Cmmn_read_data_from_stream_to_buf (int stream_handle, Pus_Msg_list_t* msg)
{
  long           n = 0;
  pus_data_t    *new_data_buf, *tmp;
  unsigned char *buf = PUS_MEM_ALLOC (512);

  while ((n = pus_stream_read (stream_handle, buf, 512)) > 0) {   
    
    new_data_buf = PUS_MEM_ALLOCTYPE (pus_data_t);
    new_data_buf->buf = buf;
    new_data_buf->buf_len = n;
    new_data_buf->next = NULL;
    
    if (msg->buf_list == NULL) 
      msg->buf_list = new_data_buf;
    else {
      
      for (tmp = msg->buf_list; tmp->next != NULL; tmp = tmp->next);
      tmp->next = new_data_buf;     
    }
  }
  if (n == PUS_STREAM_ERROR_EOF) {
    pus_stream_close (stream_handle);
    return FALSE;  
  }
  else if (n == PUS_STREAM_ERROR_DELAYED) {
    return TRUE;
  }
  else {
    
    pus_delete_msg_from_list (msg->wid);
    return TRUE;
      
  }
}
    






void
Pus_Cmmn_build_buf_from_data_buf_list (Pus_Msg_list_t* msg)
{
  WE_INT32     tot_len = 0, index = 0;
  pus_data_t   *tmp, *tmp2;

  
  
  for (tmp = msg->buf_list; tmp != NULL; tmp = tmp->next)
  {
    tot_len += tmp->buf_len; 
  }

  msg->msg_data = PUS_MEM_ALLOC (tot_len);
  msg->msg_len = tot_len;

  for (tmp = msg->buf_list; tmp != NULL; tmp = tmp2)
  {
    memcpy (msg->msg_data + index, tmp->buf, tmp->buf_len);
    index += tmp->buf_len;
    PUS_MEM_FREE (tmp->buf);
    tmp2 = tmp->next;
    PUS_MEM_FREE (tmp);
  }
  msg->buf_list = NULL;
}



int
pus_disp_start_whitelist_dialog (int mod_id, char* info)
{
  Pus_Dlg_t  *new_dialog;
  WE_UINT32 string_id[3];
  string_id[0] = PUS_STR_ID_NO_WHITELIST_MSG;
  string_id[1] = PUS_STR_ID_ACCEPT_MSG;
  string_id[2] = PUS_STR_ID_DECLINE_MSG;
  
  new_dialog = Pus_DlgCreate (PUS_DLG_WHITELIST_NO_MATCH, 4, string_id, info);
  if (new_dialog != NULL) {
    pus_register_dialog (mod_id, new_dialog);
  }
  return Pus_DlgGetDialogHandle (new_dialog);
}

void
pus_register_dialog (int pus_module, Pus_Dlg_t *dlg)
{
  pus_dialog_t *tmp_dlg = PUS_MEM_ALLOCTYPE (pus_dialog_t);

  tmp_dlg->pus_module = pus_module;
  tmp_dlg->dlg = dlg;

  tmp_dlg->next = pus_dialogs;
  pus_dialogs = tmp_dlg;
}

int
pus_deregister_dialog (Pus_Dlg_t *dlg)
{
  pus_dialog_t *tmp_dlg, *prev;

  for (prev = NULL, tmp_dlg = pus_dialogs; tmp_dlg != NULL;
       prev = tmp_dlg, tmp_dlg = tmp_dlg->next) {
    if (tmp_dlg->dlg == dlg) {
      if (prev != NULL)
        prev->next = tmp_dlg->next;
      else
        pus_dialogs = tmp_dlg->next;

      PUS_MEM_FREE (tmp_dlg);
      tmp_dlg = NULL;
      break;
    }
  }
  if (pus_dialogs == NULL)
    return FALSE;
  else
    return TRUE;
}

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
 



void
Pus_Cmmn_widget_action (we_widget_action_t *p) 
{
  pus_dialog_t *tmp_dlg;
  WE_UINT32    handle, action, dialog_handle;

  handle = p->handle;
  action = p->action;
  PUS_MEM_FREE (p);

  for (tmp_dlg = pus_dialogs; tmp_dlg != NULL; tmp_dlg = tmp_dlg->next) {
    dialog_handle = Pus_DlgGetDialogHandle (tmp_dlg->dlg);
    if (handle == dialog_handle) {
      Pus_DlgAction (tmp_dlg->dlg, action);
      PUS_SIGNAL_SENDTO_P (tmp_dlg->pus_module, PUS_SIG_DIALOG_RESPONSE,
                            tmp_dlg->dlg);
      return;
    }
  }
  
}
#endif







Pus_Msg_list_t*
pus_find_msg_by_id (int wid)
{
  Pus_Msg_list_t *msg;

  for (msg = Pus_Msg_list; msg != NULL; msg = msg->next_msg) {
    if (msg->wid == wid)
      break;
  }
  return msg;
}




void
pus_delete_msg_from_list (int wid)
{
  Pus_Msg_list_t   *tmp, *prev;

  for (prev = NULL, tmp = Pus_Msg_list; tmp != NULL; prev = tmp, tmp = tmp->next_msg) {
    if (tmp->wid == wid) {
      if (prev == NULL)
        Pus_Msg_list = tmp->next_msg;
      else
        prev->next_msg = tmp->next_msg;
      
      PUS_MEM_FREE (tmp->headers);
      if (tmp->content->dataType == WE_CONTENT_DATA_RAW)
        PUS_MEM_FREE (tmp->content->_u.data);
      else if (tmp->content->dataType == WE_CONTENT_DATA_PIPE)
        PUS_MEM_FREE (tmp->content->_u.pipeName);
      else if (tmp->content->dataType == WE_CONTENT_DATA_FILE)
        PUS_MEM_FREE (tmp->content->_u.pathName);
      PUS_MEM_FREE (tmp->content->contentType);
      PUS_MEM_FREE (tmp->content);
      PUS_MEM_FREE (tmp->url);
      PUS_MEM_FREE (tmp->routing_identifier);
      PUS_MEM_FREE (tmp->initiator_uri);
      PUS_MEM_FREE (tmp->content_location);
      PUS_MEM_FREE (tmp->content_uri);
      PUS_MEM_FREE (tmp->parsed_info.href);
      PUS_MEM_FREE (tmp->parsed_info.si_id);
      PUS_MEM_FREE (tmp->parsed_info.text);
      if (tmp->parsed_info.co_data != NULL) {
        pus_co_data_t   *old_next, *next = tmp->parsed_info.co_data;
        while (next != NULL) {
          PUS_MEM_FREE (next->text);
          old_next = next;
          next = next->next;
          PUS_MEM_FREE (old_next);
        }
      }
#ifdef PUS_CONFIG_MULTIPART 
     






#endif
      PUS_MEM_FREE (tmp);
      break;
    }
  }
}





const char*
Pus_Main_get_key_as_string (int key) {
  return we_cmmn_int2str (key, &pus_reg_key_sti);
}
 
const char*
Pus_Main_get_path_as_string (int key) {
  return we_cmmn_int2str (key, &pus_reg_path_sti);
}

int
Pus_Main_get_path_as_int (const char* path) {
  return we_cmmn_str2int_lc (path, strlen(path), &pus_reg_path_sti);
}

int
Pus_Main_get_key_as_int (const char* key) {
  return we_cmmn_str2int_lc (key, strlen(key), &pus_reg_key_sti);
}




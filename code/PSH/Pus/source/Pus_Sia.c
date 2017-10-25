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

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 

#include "we_neta.h"
#include "stk_if.h"

#include "Pus_Sia.h"
#include "Pus_Main.h"
#include "Pus_Cmmn.h"
#include "Pus_Sig.h"
#include "Pus_Dlg.h"
#include "Pus_Rc.h"



typedef struct Pus_Sia_item_st {
  struct Pus_Sia_item_st *next;

  unsigned char          *address;
  int                     address_len;
  int                     serverport;
  int                     connection_type;
  int                     bearer;
  WE_UINT32              wid;
  unsigned char          *assumed_cpi_tag;
} Pus_Sia_item_t;


typedef struct Pus_Sia_info_st {        
  struct Pus_Sia_info_st    *next;
  int                        serverport;
  int                        bearer;
  unsigned char             *address;
  int                        address_len;
} Pus_Sia_info_t;

typedef struct Pus_Sia_prot_opt_list_st {
  struct Pus_Sia_prot_opt_list_st  *next;

  int                               option;
} Pus_Sia_prot_opt_list_t;




Pus_Sia_item_t           *sia_list;





static Pus_Sia_item_t*
Pus_Sia_find_sia_in_list (WE_UINT32 wid);

static int
Pus_Sia_extract_sia ( unsigned char **cont_pts_wsp_list, WE_UINT32 *cont_pts_wsp_len,
                      unsigned char **cont_pts_http_list, WE_UINT32 *cont_pts_http_len,
                      unsigned char **cpi_tag_list, WE_UINT32 *cpi_tag_len,
                      unsigned char **prov_url, WE_UINT32 *prov_url_len, 
                      unsigned char *body, WE_UINT32 body_len, Pus_Sia_prot_opt_list_t *prot_opt_list);

static int
Pus_Sia_get_next_value_pair (we_dcvt_t *cvt_obj, WE_UINT32* int_value, 
                             unsigned char **string_value, int* error_code);

static Pus_Sia_info_t*
Pus_Sia_extract_address_type (unsigned char *cont_pts_wsp_list, int cont_pts_wsp_len);

static int
Pus_Sia_start_sia_dialog (void);


static Pus_Sia_item_t*
Pus_Sia_find_sia_in_list (WE_UINT32 wid);

static void
Pus_Sia_delete_sia_from_list (WE_UINT32 wid);






void
Pus_Sia_init (void)
{
  sia_list = NULL;
  Pus_Signal_register_dst (PUS_MODULE_SIA, Pus_Sia_main);
}






void
Pus_Sia_terminate (void)
{
  

  pus_client_conn_t *conn, *tmp;

  for (conn = pus_client_conn_list; conn != NULL; conn = tmp) {
    tmp = conn->next;
    PUS_MEM_FREE (conn->assumed_cpi_tag);
    PUS_MEM_FREE (conn);
  }
}
 




void
Pus_Sia_main (Pus_Signal_t* signal)
{
  int              answer, conn_id, more_dialogs;
  WE_INT32        networkAccountId;
  Pus_Sia_item_t  *sia_item = NULL;
  we_sockaddr_t   addr;
  Pus_Dlg_t *dlg = (Pus_Dlg_t*) signal->p_param;
  pus_client_conn_t *new_conn;


  if (signal->type == PUS_SIG_DIALOG_RESPONSE) 
 {
    
    sia_item = Pus_Sia_find_sia_in_list (Pus_DlgGetDialogHandle (dlg));

    if (sia_item == NULL)
      goto done;

    
    answer = Pus_DlgGetResponse (dlg);
    if (answer == PUS_DLG_RESPONSE_POSITIVE) 
   { 
      
      if (sia_item->bearer != TPI_SOCKET_BEARER_IP_ANY) 
      {
        networkAccountId = WE_NETWORK_ACCOUNT_GET_ID (sia_item->bearer);
        
        if (networkAccountId < 0)
          networkAccountId = Pus_Sia_default_net_id;  

      }
      else
        networkAccountId = Pus_Sia_default_net_id;
      
      memcpy (addr.addr, sia_item->address, sia_item->address_len);
      addr.addrLen = (WE_INT16)sia_item->address_len;
      addr.port = (WE_UINT16)sia_item->serverport;
      conn_id =  pus_new_handle ();
      STKif_createClientConnection (WE_MODID_PUS, conn_id,
                                      addr, sia_item->connection_type,
                                      networkAccountId);
      new_conn = PUS_MEM_ALLOCTYPE (pus_client_conn_t);
      new_conn->assumed_cpi_tag = sia_item->assumed_cpi_tag;
      new_conn->conn_id = conn_id;
      new_conn->next = pus_client_conn_list;
      new_conn->connectionType = (WE_INT16) (sia_item->connection_type);
      new_conn->network_account_id = networkAccountId;
      new_conn->assumed_cpi_tag = NULL;
      pus_client_conn_list = new_conn;
    }
    else 
   { 
      if (sir_lockout_timer)
        we_timer_reset (WE_MODID_PUS, 1);
      sir_lockout_timer = FALSE;
    }
    Pus_Sia_delete_sia_from_list (Pus_DlgGetDialogHandle (dlg));
  }
  
done:
  more_dialogs = pus_deregister_dialog (dlg); 
  Pus_DlgDelete (dlg, more_dialogs);
  Pus_Signal_delete (signal);
}





int
pus_handle_sia_push (stk_content_t *p)
{
  unsigned char       *cont_pts_wsp_list = NULL; 
  unsigned char       *prov_url = NULL;
  unsigned char       *cont_pts_http_list = NULL;
  unsigned char       *cpi_tag_list = NULL;
  char                *temp;
  WE_UINT32           cont_pts_wsp_len;
  WE_UINT32           prov_url_len = 0;
  WE_UINT32           cont_pts_http_len = 0;
  WE_UINT32           cpi_tag_len = 0;
  Pus_Sia_info_t      *wsp_p, *tmp, *http_p;
  Pus_Sia_item_t      *sia_item;
  unsigned char       *body;
  WE_UINT32           body_len;
  int                  index = 0;
  
  Pus_Sia_prot_opt_list_t   *prot_list = PUS_MEM_ALLOCTYPE (Pus_Sia_prot_opt_list_t);
  Pus_Sia_prot_opt_list_t   *tmp_prot_list; 
  prot_list->next = NULL;
  prot_list->option = -1;

  if (p->dataType == WE_CONTENT_DATA_PIPE) 
  {
    ;
    body = NULL;
    body_len = 0;
  }
  else if (p->dataType == WE_CONTENT_DATA_RAW) 
  {
    body = p->_u.data;
    body_len = p->dataLen;
  }
  else
    return FALSE;


  if (!Pus_Sia_extract_sia (&cont_pts_wsp_list, &cont_pts_wsp_len,
                            &cont_pts_http_list, &cont_pts_http_len,
                            &cpi_tag_list, &cpi_tag_len,
                            &prov_url, &prov_url_len, body, body_len, prot_list))
      return FALSE;
    
  
  wsp_p = Pus_Sia_extract_address_type (cont_pts_wsp_list, cont_pts_wsp_len);
  http_p = Pus_Sia_extract_address_type (cont_pts_http_list, cont_pts_http_len);

  if (wsp_p == NULL && http_p == NULL)
    return FALSE;

  


  while (wsp_p != NULL) 
 {
    sia_item = PUS_MEM_ALLOCTYPE (Pus_Sia_item_t);
    sia_item->address = wsp_p->address;
    sia_item->address_len = wsp_p->address_len;
    sia_item->bearer = wsp_p->bearer;
    sia_item->connection_type = STK_CONNECTION_TYPE_WSP_CO;
    sia_item->serverport = wsp_p->serverport;
    sia_item->wid = Pus_Sia_start_sia_dialog ();
    sia_item->next = sia_list;
    sia_list = sia_item;

    tmp = wsp_p;
    wsp_p = wsp_p->next;
    PUS_MEM_FREE (tmp);
  }

  
  while (http_p != NULL) {
    sia_item = PUS_MEM_ALLOCTYPE (Pus_Sia_item_t);
    sia_item->address = http_p->address;
    sia_item->address_len = http_p->address_len;
    sia_item->bearer = http_p->bearer;
    sia_item->connection_type = STK_CONNECTION_TYPE_HTTP_PUSH;
    sia_item->serverport = http_p->serverport;
   
    


    if (prot_list->option == 1) {
      temp = PUS_MEM_ALLOC (4);
      memcpy (temp, (cpi_tag_list + index), 4);
      index += 4;
      sia_item->assumed_cpi_tag = (unsigned char*)temp;
    }
    else {
      sia_item->assumed_cpi_tag = NULL;
    }
    prot_list = prot_list->next;

    sia_item->wid = Pus_Sia_start_sia_dialog ();
    sia_item->next = sia_list;
    sia_list = sia_item;

    tmp = http_p;
    http_p = http_p->next;
    PUS_MEM_FREE (tmp);   
  }

  PUS_MEM_FREE (prov_url);
  PUS_MEM_FREE (cont_pts_wsp_list);
  PUS_MEM_FREE (cont_pts_http_list);
  PUS_MEM_FREE (cpi_tag_list);

  tmp_prot_list = prot_list;
  while (tmp_prot_list != NULL) {
    tmp_prot_list = prot_list->next;
    PUS_MEM_FREE (prot_list);
    prot_list = tmp_prot_list;
  }
  return TRUE;
}





static int
Pus_Sia_extract_sia ( unsigned char **cont_pts_wsp_list, WE_UINT32 *cont_pts_wsp_len,
                      unsigned char **cont_pts_http_list, WE_UINT32 *cont_pts_http_len,
                      unsigned char **cpi_tag_list, WE_UINT32 *cpi_tag_len,
                      unsigned char **prov_url, WE_UINT32 *prov_url_len, 
                      unsigned char *body, WE_UINT32 body_len, Pus_Sia_prot_opt_list_t *prot_opt_list)
{
  




























  we_dcvt_t       cvt_obj;
  WE_UINT8        version;
  int              i, error_code = FALSE, first_time = TRUE;
  WE_UINT32       app_id_len, prot_opts_len, prot_opts;
  
  unsigned char   *app_id_list = NULL;    
  Pus_Sia_prot_opt_list_t *last;

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, body, body_len, WE_MODID_PUS);

  if ( !we_dcvt_uint8 (&cvt_obj, &version)) 
    return FALSE;

  for (;;) {
    
    if ( !Pus_Sia_get_next_value_pair (&cvt_obj, &app_id_len, &app_id_list, 
                                       &error_code))
      break;
     
    if ( !Pus_Sia_get_next_value_pair (&cvt_obj, &(*cont_pts_wsp_len), 
                                       &(*cont_pts_wsp_list), &error_code))
      break;
    
    if ( !Pus_Sia_get_next_value_pair (&cvt_obj, &(*cont_pts_http_len), &(*cont_pts_http_list), 
                                       &error_code))
      break;
     
    

    if ((!we_dcvt_uintvar (&cvt_obj, &prot_opts_len)) ||
        ((int)prot_opts_len + cvt_obj.pos > cvt_obj.length) ) {
      error_code = TRUE;
      break;
    }
    if (prot_opts_len > 0) {
      for (i = prot_opts_len; i > 0; i--)
      {
        

        if (!we_dcvt_uintvar (&cvt_obj, &prot_opts)) {
          error_code = TRUE;
          break;
        }
        if (!first_time) {
          for (last = prot_opt_list; last->next != NULL; last = last->next);
          last->next = PUS_MEM_ALLOCTYPE (Pus_Sia_prot_opt_list_t);
          last->next->next = NULL;
          last->next->option = prot_opts;
        }
        else
          prot_opt_list->option = prot_opts;
      }
    }
    
    
    if ( !Pus_Sia_get_next_value_pair (&cvt_obj, &(*prov_url_len), &(*prov_url), 
                                       &error_code))
      break;

    
    if ((!we_dcvt_uintvar (&cvt_obj, cpi_tag_len)) ||
        ((int)cpi_tag_len + cvt_obj.pos > cvt_obj.length) ){
      error_code = TRUE;
      break;
    }
    else if (*cpi_tag_len > 0) {
      *cpi_tag_len *= 4;
      if (!we_dcvt_uchar_vector (&cvt_obj, (WE_UINT16)(*cpi_tag_len), cpi_tag_list))
        error_code = TRUE; 
    }
    break;
  }
  if (error_code)
    return FALSE;

  
  PUS_MEM_FREE (app_id_list);
  return TRUE;
}







static int
Pus_Sia_get_next_value_pair (we_dcvt_t *cvt_obj, WE_UINT32* int_value, 
                             unsigned char **string_value, int* error_code)
{
  if ( (!we_dcvt_uintvar (cvt_obj, int_value)) ||
       ((int)*int_value + cvt_obj->pos > cvt_obj->length)) {
    *error_code = TRUE;
    return FALSE;
  }

  if ( (*int_value == 0) && (cvt_obj->pos == cvt_obj->length))
    return FALSE;  
  else if ( *int_value == 0)
    return TRUE;

  if (!we_dcvt_uchar_vector (cvt_obj, (WE_UINT16)*int_value, string_value)){
    *error_code = TRUE;
    return FALSE;
  }

  if (cvt_obj->pos == cvt_obj->length)
    return FALSE;  
  else
    return TRUE;  
}





static Pus_Sia_info_t*
Pus_Sia_extract_address_type (unsigned char *cont_pts_wsp_list, int cont_pts_wsp_len)
{
  Pus_Sia_info_t      *p = NULL, *tmp, *prev = NULL;
  int                  i = 0, step = 0;

  while ( i < cont_pts_wsp_len) {
    tmp = PUS_MEM_ALLOCTYPE (Pus_Sia_info_t);
    tmp->next = NULL; 
    tmp->address_len = (cont_pts_wsp_list[i] & 0x3F);
    if (cont_pts_wsp_list[i] & 0x80) {
      step++;
      tmp->bearer = (int)cont_pts_wsp_list[i + step];
    }
    if (cont_pts_wsp_list[i] & 0x40) {
      step++;
      tmp->serverport = cont_pts_wsp_list[i + step] << 8;
      step++;
      tmp->serverport += cont_pts_wsp_list[i + step]; 
    }
    step++;
    tmp->address = PUS_MEM_ALLOC (tmp->address_len);
    memcpy (tmp->address, (const char*)cont_pts_wsp_list + (i + step), tmp->address_len);
    i = i + step + tmp->address_len;
    step = 0;
    if (p == NULL)
      p = prev = tmp;
    else
      prev->next = tmp;
  }
  return p;
}




static int
Pus_Sia_start_sia_dialog (void)
{
  Pus_Dlg_t  *new_dialog;
  WE_UINT32 string_id[3];
  string_id[0] = PUS_STR_ID_SIA_CONFIRM_MSG;
  string_id[1] = PUS_STR_ID_OK;
  string_id[2] = PUS_STR_ID_CANCEL;
  
  new_dialog = Pus_DlgCreate (PUS_DLG_SIA_CONFIRM, 3, string_id, NULL);
  if (new_dialog != NULL) {
    pus_register_dialog (PUS_MODULE_SIA, new_dialog);
  }
  return Pus_DlgGetDialogHandle (new_dialog);
}


static Pus_Sia_item_t*
Pus_Sia_find_sia_in_list (WE_UINT32 wid)
{
  Pus_Sia_item_t    *sia;

  for (sia = sia_list; sia != NULL; sia = sia->next) {
    if (sia->wid == wid)
      break;
  }
  return sia;

}


static void
Pus_Sia_delete_sia_from_list (WE_UINT32 wid)
{
  Pus_Sia_item_t   *tmp, *prev;

  for (prev = NULL, tmp = sia_list; tmp != NULL; prev = tmp, tmp = tmp->next) {
    if (tmp->wid == wid) {
      if (prev == NULL)
        sia_list = tmp->next;
      else
        prev->next = tmp->next;
      
      PUS_MEM_FREE (tmp->address);

      PUS_MEM_FREE (tmp);
      break;
    }
  }
}
#endif

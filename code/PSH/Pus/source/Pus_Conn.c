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














































































#include "we_mem.h"
#include "we_neta.h"
#include "stk_if.h"
#include "Pus_If.h"
#include "Pus_Main.h"
#include "Pus_Conn.h"







#define PUS_CONN_STATE_REQUEST_TO_OPEN               1
#define PUS_CONN_STATE_OPEN                          2





static pus_user_t*
pus_find_user_by_module_id (int module_id);

static pus_client_conn_t*
Pus_Conn_find_client_connection_id (int connection_id);

static pus_user_conn_info_t*
pus_find_connection_by_external_handle (pus_user_t *user, WE_INT16 handle);

static Pus_Connection_t*
pus_socket_already_exist (pus_open_push_connection_t *p);

static void
pus_create_connection (pus_user_t *user, pus_open_push_connection_t *p);

static void
pus_add_conn_to_user (pus_user_t *user, Pus_Connection_t *conn, WE_INT16 p) ;

static void
pus_remove_connection_from_table (Pus_Connection_t *c);

static pus_user_t*
pus_create_user (WE_UINT8 module_id);

static void
pus_remove_client_connection_from_list (WE_UINT32 wid);







/*创建连接*/
void
Pus_Conn_handle_open_connection (WE_UINT8 module_id, pus_open_push_connection_t *p)
{
  pus_user_t         *user;

  user = pus_find_user_by_module_id (module_id);
  if (user == NULL) 
  {
    user = pus_create_user (module_id);
  }

  pus_create_connection (user, p);
  
  PUS_MEM_FREE (p);
}




void
Pus_Conn_handle_close_connection (WE_UINT8 module_id, WE_UINT16 handle)
{
  pus_user_t             *user;
  pus_user_conn_info_t   *user_conn_info;

  user = pus_find_user_by_module_id (module_id);
  if (user == NULL) {
    

  }
  else {
    if ((user_conn_info = pus_find_connection_by_external_handle (user, handle)) != NULL) 
    {
      if (user_conn_info->conn->nbr_of_users == 1) 
      {
        STKif_closeConnection (WE_MODID_PUS, user_conn_info->conn->connection_id);
        pus_remove_connection_from_table (user_conn_info->conn);
      }
      else
        user_conn_info->conn->nbr_of_users--;
    }    
  }
  
     
}




void
Pus_Conn_handle_connection_created (stk_connection_created_t *p)
{
  pus_user_t       *user;
  Pus_Connection_t *conn;
  int               i;
  
  if ((conn = pus_find_server_connection_by_connection_id (p->connectionId)) != NULL) 
  {
    conn->state = PUS_CONN_STATE_OPEN;  
    for (user = pus_user_list; user != NULL; user = user->next) 
   {
      for (i = 0; i < user->num_open_connections; i++) 
     {     
        if (user->user_conn[i]->conn == conn) 
        {    
          if (user->module_id != WE_MODID_PUS)
            Pus_ConnectionOpened (user->module_id, user->user_conn[i]->external_handle, PUS_ERR_OK);
        }
      }
    }

  }
  else 
  {
    if ((Pus_Conn_find_client_connection_id (p->connectionId)) &&
        (sir_lockout_timer)) 
    {
      we_timer_reset (WE_MODID_PUS, 1);
      sir_lockout_timer = FALSE;
    }
  }
  PUS_MEM_FREE (p);
  
}




void
Pus_Conn_handle_connection_closed (stk_connection_closed_t *p)
{
  pus_user_t        *user;
  Pus_Connection_t  *conn;
  int                i;
  
  if ((conn = pus_find_server_connection_by_connection_id (p->connectionId)) != NULL) {
    






    if (conn->connectionType == STK_CONNECTION_TYPE_WSP_CL_WTLS) {
      
      STKif_createServerConnection (WE_MODID_PUS, conn->connection_id,
                                    conn->portNumber, conn->connectionType, 
                                    conn->networkAccountId);
    }
    else {
      
      for (user = pus_user_list; user != NULL; user = user->next) {
        for (i = 0; i < user->num_open_connections; i++) {     
          if (user->user_conn[i]->conn == conn) {    
            if (user->module_id != WE_MODID_PUS)
              Pus_ConnectionClosed (user->module_id, user->user_conn[i]->external_handle, 
                                    PUS_ERR_CONN_CLOSED_BY_STK);
            user->num_open_connections--;
          }
        }
      }
      pus_remove_connection_from_table (conn);
    }
  }
  else { 
    pus_remove_client_connection_from_list (p->connectionId);
  }
  PUS_MEM_FREE (p);
  
}









static pus_user_t*
pus_find_user_by_module_id (int module_id)
{
  pus_user_t    *user;

  for (user = pus_user_list; user != NULL; user = user->next) 
  {
    if (user->module_id == module_id)
      return user;
  }

  return NULL;
}




Pus_Connection_t*
pus_find_server_connection_by_connection_id (int connection_id)
{
  int                  i;

  for (i = 0; i < PUS_CFG_MAX_CONNECTIONS; i++) 
    {
    if (Pus_Connection_table[i] != NULL) {
      if (Pus_Connection_table[i]->connection_id == connection_id)
        return Pus_Connection_table[i];
    }
  }
  
  return NULL;
}







int 
Pus_Conn_get_connection_type_by_connection_id (int connection_id)
{
  Pus_Connection_t  *conn = pus_find_server_connection_by_connection_id (connection_id);
  pus_client_conn_t *client_conn;

  if (conn == NULL) {
    client_conn = Pus_Conn_find_client_connection_id (connection_id);
    if (client_conn == NULL)
      return -1;
    else 
      return client_conn->connectionType;
  }
  else
    return conn->connectionType;
}







int 
Pus_Conn_get_network_acc_id_type_by_connection_id (int connection_id)
{
  Pus_Connection_t  *conn = pus_find_server_connection_by_connection_id (connection_id);
  pus_client_conn_t *client_conn;

  if (conn == NULL) {
    client_conn = Pus_Conn_find_client_connection_id (connection_id);
    if (client_conn == NULL)
      return -1;
    else 
      return client_conn->network_account_id;  
  }
  else
    return conn->networkAccountId;
}





unsigned char*
Pus_Conn_get_cpi_tag_by_connection_id (int connection_id)
{
  pus_client_conn_t *client_conn = Pus_Conn_find_client_connection_id (connection_id);

  if (client_conn == NULL)
    return NULL;
  else 
    return client_conn->assumed_cpi_tag;
}





static pus_client_conn_t*
Pus_Conn_find_client_connection_id (int connection_id)
{
  pus_client_conn_t *client_conn;

  for (client_conn = pus_client_conn_list; client_conn != NULL; client_conn = client_conn->next) {
    if (client_conn->conn_id == connection_id) 
      return client_conn;
  }
   
  return NULL;
}




static pus_user_conn_info_t*
pus_find_connection_by_external_handle (pus_user_t *user, WE_INT16 handle)
{
  int i;
  
  for (i = 0; i < user->num_open_connections; i++) 
  {
    if (user->user_conn[i]->external_handle == handle)
      return user->user_conn[i];
  }

  return NULL;
}




static Pus_Connection_t*
pus_socket_already_exist (pus_open_push_connection_t *p) 
{
  int            i;
  
  for (i = 0; i < PUS_CFG_MAX_CONNECTIONS; i++) 
  {
    if (Pus_Connection_table[i] != NULL) 
   {
      if ((Pus_Connection_table[i]->portNumber == p->portNumber) &&
          (Pus_Connection_table[i]->connectionType == p->connectionType)) 
      {
        return Pus_Connection_table[i];      
      }
    }    
  }

  return NULL;
}





static pus_user_t*
pus_create_user (WE_UINT8 module_id)
{
  pus_user_t *user = PUS_MEM_ALLOCTYPE (pus_user_t);
  int    i;

  user->module_id = module_id;
  user->num_open_connections = 0;
  for (i = 0; i < PUS_CFG_MAX_CONNECTIONS_PER_USER; i++)
    user->user_conn[i] = NULL;
  user->next = pus_user_list;
  pus_user_list = user;

  return user;
}




static void
pus_create_connection (pus_user_t *user, pus_open_push_connection_t *p)
{ 
  Pus_Connection_t *conn;
  int               i;
  
  if ((conn = pus_socket_already_exist (p)) != NULL) 
  {
    pus_add_conn_to_user (user, conn, p->handle);
    if (conn->networkAccountId == p->networkAccountId) 
    {
      if (conn->state == PUS_CONN_STATE_OPEN)
        Pus_ConnectionOpened (user->module_id, p->handle, PUS_ERR_OK_ALREADY_OPENED_BY_OTHER_MODULE);
    }
    else
      Pus_ConnectionOpened (user->module_id, p->handle, PUS_ERR_FAILED_ALREADY_EXIST_WITH_OTHER_NWA);        
    return;
  }

  if (pus_find_connection_by_external_handle(user, p->handle) != NULL) 
  {
    

    Pus_ConnectionOpened (user->module_id, p->handle, PUS_ERR_FAILED_HANDLE_ALREADY_EXIST);
    return;
  }

  if (user->num_open_connections >= PUS_CFG_MAX_CONNECTIONS) 
  {
    

    Pus_ConnectionOpened (user->module_id, p->handle, PUS_ERR_FAILED_TOO_MANY_CONN);
    return;
  }
  
  for (i = 0; Pus_Connection_table[i] != NULL; i++ );
  if (i >= PUS_CFG_MAX_CONNECTIONS) 
  {
    Pus_ConnectionOpened (user->module_id, p->handle, PUS_ERR_FAILED_TOO_MANY_CONN);
    return;
  }
  conn = PUS_MEM_ALLOCTYPE (Pus_Connection_t);
  conn->state = PUS_CONN_STATE_REQUEST_TO_OPEN;
  conn->connection_id = pus_new_handle();
  conn->connectionType = p->connectionType;
  conn->bearer = WE_NETWORK_ACCOUNT_GET_BEARER (p->networkAccountId);
  conn->portNumber = p->portNumber;
  conn->networkAccountId = p->networkAccountId;
  conn->nbr_of_users = 0;
  pus_add_conn_to_user (user, conn, p->handle);
  Pus_Connection_table[i] = conn;

  STKif_createServerConnection (WE_MODID_PUS, conn->connection_id,
                                  p->portNumber, p->connectionType, p->networkAccountId);
}




static void
pus_add_conn_to_user (pus_user_t *user, Pus_Connection_t *conn, WE_INT16 handle) 
{
  pus_user_conn_info_t   *new_user_conn_info = PUS_MEM_ALLOCTYPE (pus_user_conn_info_t);
  
  conn->nbr_of_users++;
  new_user_conn_info->conn = conn;
  new_user_conn_info->external_handle = handle;
  user->user_conn[user->num_open_connections] = new_user_conn_info;
  user->num_open_connections++;
}
    
    



static void
pus_remove_connection_from_table (Pus_Connection_t *c)
{
  int i;

  for (i = 0; i < PUS_CFG_MAX_CONNECTIONS; i++) {
    if (Pus_Connection_table[i] == c) {
      PUS_MEM_FREE (Pus_Connection_table [i]);
      Pus_Connection_table[i] = NULL;
    }
  }
}

static void
pus_remove_client_connection_from_list (WE_UINT32 wid)
{
  pus_client_conn_t   *tmp, *prev;

  for (prev = NULL, tmp = pus_client_conn_list; tmp != NULL; prev = tmp, tmp = tmp->next) {
    if (tmp->conn_id == (int)wid) {
      if (prev == NULL)
        pus_client_conn_list = tmp->next;
      else
        prev->next = tmp->next;
      
      PUS_MEM_FREE (tmp->assumed_cpi_tag);

      PUS_MEM_FREE (tmp);
      break;
    }
  }
}

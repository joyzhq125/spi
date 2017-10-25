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
 * We_Act.c
 *
 * Action package:
 * 
 *
 * Created by Simon Magnusson, Sep 10 2003.
 *
 * Revision  history:
 *
 * 
 */

#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Wid.h"
#include "We_Lib.h"
#include "We_Mem.h"
#include "We_Pck.h"
#include "We_Sas.h"
#include "We_Afi.h"
#include "We_Pipe.h"
#include "We_Cmmn.h"
#include "We_Act.h"
#include "We_Drc.h"


 /**********************************************************************
 * Defines 
 **********************************************************************/

/* Action types */
#define WE_MIME_ACTION_TYPE_EXTERNAL    1
#define WE_MIME_ACTION_TYPE_INTERNAL    2
#define WE_MIME_ACTION_TYPE_PACKET      3
#define WE_MIME_ACTION_TYPE_CATEGORY    4

/* Capability values must be power of 2 */
#define WE_ACTION_CAPABILITY_NONE       0
#define WE_ACTION_CAPABILITY_DRM        1

#define WE_ACT_STATE_IDLE      1
#define WE_ACT_STATE_MENU_WAIT 2
#define WE_ACT_STATE_SAVE_WAIT 3
#define WE_ACT_STATE_FINISHED  4

#define WE_ACT_STATE_AFI_OPEN_START 5
#define WE_ACT_STATE_AFI_OPEN_BUSY  6
#define WE_ACT_STATE_AFI_CLOSE_BUSY 7

#define WE_ACT_DRM_CONTENT_TYPE "application/vnd.oma.drm.content"
#define WE_ACT_DRM_MESSAGE_TYPE "application/vnd.oma.drm.message"


/**********************************************************************
 * Typedefs
 **********************************************************************/

typedef struct we_act_int_elem_st {
  WE_INT32                   i;
  struct we_act_int_elem_st *next;
} we_act_int_elem_t;

typedef struct we_act_window_st {
  struct we_act_window_st     *next;
  we_drc_t                    *dlg;
} we_act_window_t;

typedef struct we_act_op_st {
  struct we_act_op_st    *next;
  WE_INT32               wid;          /* operation identifier */
  int                     state;
  void                    *priv_data;
  long                    status;    /* status of operation */
  char*                   mime_type;
  char*                   file_name;
  WeResourceType         data_type;
  void*                   data;
  long                    data_len;
  char*                   src_path;
  we_pck_attr_list_t     *file_attrs;
  WE_INT32               sas_id;
  we_act_window_t        *window;
  WeScreenHandle         screen;
  char*                   content_type;
  WE_INT8                type;      /* which operation */
  we_act_int_elem_t      *int_elem;
  WE_INT32               num_elem;
  WE_INT32               afi_id;
  WE_INT32               action_id;
  char*                   posix_path;
  WE_BOOL                caller_owns_pipe;
} we_act_op_t;

typedef struct we_act_handle_st {
  WE_UINT8                  modid;
  we_act_op_t              *op;
  we_pck_handle_t          *sas_handle;
  we_pck_handle_t          *afi_handle;
} we_act_handle_t;

typedef struct we_act_mime_action_st {
  char*                      mime_types;
  char*                      act_cmd;
  WE_UINT8                  modid;
  WE_UINT32                 name_id;
  int                        act_type;
  int                        category;
  WE_UINT32                 blocked_permissions;
  WE_UINT32                 capabilities;
} we_act_mime_action_t;



/**********************************************************************
 * Mime Action table
 **********************************************************************/

#ifndef WE_NBR_OF_MIME_ACTIONS
#define WE_NBR_OF_MIME_ACTIONS 0
#endif

#if WE_NBR_OF_MIME_ACTIONS > 0
const static we_act_mime_action_t we_act_mime_action_table[WE_NBR_OF_MIME_ACTIONS] = WE_MIME_ACTIONS;
#else
const static we_act_mime_action_t *we_act_mime_action_table = NULL;
#endif

/**********************************************************************
 * Local functions
 **********************************************************************/

static we_act_op_t *
find_op_by_id (we_act_handle_t *handle, WE_INT32 wid)
{
  we_act_op_t *op = handle->op;

  while (op) {
    if (op->wid == wid)
      return op;
    op = op->next;
  }
  return NULL;
}

static we_act_int_elem_t *
get_new_int_elem (we_act_handle_t *handle, we_act_op_t *op)
{
  we_act_int_elem_t *int_elem = WE_MEM_ALLOCTYPE (handle->modid, we_act_int_elem_t);

  int_elem->i = 0;

  /* insert new int_elem first in list */
  int_elem->next = op->int_elem;
  op->int_elem = int_elem;
  return int_elem;
}

static void
free_act_int_elem(we_act_handle_t *handle, we_act_op_t *op, we_act_int_elem_t *int_elem)
{
  we_act_int_elem_t *temp_int_elem = op->int_elem;
  we_act_int_elem_t *prev_int_elem = NULL;

  /*Remove int_elem*/
  while (temp_int_elem != int_elem) {
    prev_int_elem = temp_int_elem;
    temp_int_elem = temp_int_elem->next;
  }
  if (prev_int_elem)
    prev_int_elem->next = int_elem->next;
  else
    op->int_elem = int_elem->next;

  /* Free int_elem*/
  WE_MEM_FREE(handle->modid, int_elem);
}


static we_act_window_t *
get_new_window (we_act_handle_t *handle, we_act_op_t *op)
{
  we_act_window_t *window = WE_MEM_ALLOCTYPE (handle->modid, we_act_window_t);

  window->dlg = 0;

  /* insert new window first in list */
  window->next = op->window;
  op->window = window;
  return window;
}

static void
free_act_window(we_act_handle_t *handle, we_act_op_t *op, we_act_window_t *window)
{
  we_act_window_t *temp_window = op->window;
  we_act_window_t *prev_window = NULL;

  /*Remove window*/

  while (temp_window != window) {
    prev_window = temp_window;
    temp_window = temp_window->next;
  }
  if (prev_window)
    prev_window->next = window->next;
  else
    op->window = window->next;

  /*Free window*/
  if (window->dlg)
    we_drcDelete (window->dlg);

  WE_MEM_FREE(handle->modid, window);

}

static we_act_op_t *
get_new_op (we_act_handle_t *handle)
{
  static int we_act_uniq_id = 1;

  we_act_op_t *op = WE_MEM_ALLOCTYPE (handle->modid, we_act_op_t);

  op->wid = we_act_uniq_id++;
  op->state = 0;
  op->mime_type = NULL;
  op->file_name = NULL;
  op->data_type = WeResourceFile;
  op->data = NULL;
  op->data_len = 0;
  op->src_path = NULL;
  op->file_attrs = NULL;
  op->priv_data = NULL;
  op->sas_id = 0;
  op->status = WE_ACT_ERROR_DELAYED;
  op->window = NULL;
  op->screen = 0;
  op->content_type = NULL;
  op->type = 0;
  op->int_elem = NULL;
  op->num_elem = 0;
  op->afi_id = 0;
  op->action_id = -1;
  op->posix_path = NULL;
  op->caller_owns_pipe = FALSE;
  /* insert new operation first in list */
  op->next = handle->op;
  handle->op = op;

  return op;
}

static void
free_act_op(we_act_handle_t *handle, we_act_op_t *op)
{
  we_act_op_t *temp_op = handle->op;
  we_act_op_t *prev_op = NULL;

  /*Remove op*/

  while (temp_op != op) {
    prev_op = temp_op;
    temp_op = temp_op->next;
  }
  if (prev_op)
    prev_op->next = op->next;
  else
    handle->op = op->next;
  
  /*Free op*/

  while (op->window)
    free_act_window(handle, op, op->window);

  while (op->int_elem)
    free_act_int_elem(handle, op, op->int_elem);

  if (op->mime_type)
    WE_MEM_FREE(handle->modid, op->mime_type);
  if (op->file_attrs)
    we_pck_attr_free (handle->modid, op->file_attrs);
  if (op->file_name)
    WE_MEM_FREE(handle->modid, op->file_name);
  if (op->src_path)
    WE_MEM_FREE(handle->modid, op->src_path);
  if (op->content_type)
    WE_MEM_FREE(handle->modid, op->content_type);
  if (op->posix_path)
    WE_MEM_FREE(handle->modid, op->posix_path);
 
  WE_MEM_FREE(handle->modid, op);
  op = NULL;
}

we_pck_attr_list_t *
create_attr_list_from_mime(WE_UINT8 modid, const char *mime_type)
{
  we_pck_attr_list_t *attr;
  
  we_pck_attr_init (&attr);
  
  if (we_pck_add_attr_string_value (modid, &attr, WE_PCK_ATTRIBUTE_MIME, mime_type) != TRUE)
    return NULL;

  return attr;
}

static WE_BOOL
we_act_capabilities_found(we_act_op_t *op, const we_act_mime_action_t *action)
{
  WE_BOOL found = TRUE;

  if (op->content_type != NULL) {
    if ((strcmp(op->content_type, WE_ACT_DRM_CONTENT_TYPE) == 0) ||
        (strcmp(op->content_type, WE_ACT_DRM_MESSAGE_TYPE) == 0)) {
      found = action->capabilities & WE_ACTION_CAPABILITY_DRM;
    }
  }
  return found;
}

static WE_BOOL
we_act_found_in_str_array(const char* to_find, const char* strings[], WE_INT32 strings_cnt)
{
  if ((to_find == NULL) || (strings == NULL) || (strings_cnt <= 0))
    return FALSE;

  while (strings_cnt) {
    if (*strings && (strcmp(to_find, *strings) == 0))
      return TRUE;
    strings++;
    strings_cnt--;
  }
  return FALSE;
}

static void
we_act_put_action_childs_in_op(we_act_handle_t *handle, we_act_op_t *op, WE_INT32 action_id, WE_INT32 permission,
                                const char* excl_act_strings[], WE_INT32 excl_act_strings_cnt)
{
  int i;
  we_act_int_elem_t *action_elem;

  while (op->int_elem)
    free_act_int_elem(handle, op, op->int_elem);

  op->num_elem = 0;
  for (i = 0 ; i < WE_NBR_OF_MIME_ACTIONS ; i++){
    if (we_act_mime_action_table[i].category == action_id){
      if (we_pck_mime_type_in_list (op->mime_type, we_act_mime_action_table[i].mime_types)){
         
        /* skip checking actions with wrong permission */
        if ((we_act_mime_action_table[i].blocked_permissions != WE_PERMISSION_NONE_BLOCKED) &&
            (we_act_mime_action_table[i].blocked_permissions & permission))
          continue;

        /* skip actions with wrong capabilities */
        if (!we_act_capabilities_found(op, we_act_mime_action_table + i))
          continue;

        /* skip excluded actions */
        if (we_act_found_in_str_array(we_act_mime_action_table[i].act_cmd, excl_act_strings,
                                       excl_act_strings_cnt))
          continue;

        action_elem = get_new_int_elem(handle, op);
        action_elem->i = i;
        op->num_elem++;
      }
    }
  }
}

static void
we_act_content_route(we_act_handle_t *handle, we_act_op_t *op, const we_act_mime_action_t *action)
{
  we_content_data_t    content_data;

  content_data.routingFormat = WE_ROUTING_TYPE_MIME;
  content_data.routingIdentifier = WE_MEM_ALLOC (handle->modid, strlen(op->mime_type) +1);
  strcpy(content_data.routingIdentifier, op->mime_type);
  content_data.contentUrl = we_cmmn_strdup (handle->modid, op->src_path);
  content_data.contentParameters = NULL;
  
  content_data.contentHeaders = WE_MEM_ALLOC (handle->modid, strlen("Content-Type: ") + strlen(op->mime_type) +1);
  strcpy (content_data.contentHeaders, "Content-Type: ");
  strcat (content_data.contentHeaders, op->mime_type);
  content_data.networkAccountId = -1;
  content_data.contentDataLength = op->data_len;
  content_data.contentData = op->data;
  
  content_data.contentSource = NULL;
  switch (op->data_type){
  case WeResourceFile:
    content_data.contentDataType = WE_CONTENT_DATA_FILE;
    content_data.contentSource = we_cmmn_strdup (handle->modid, op->src_path);
    break;
  case WeResourcePipe:
    content_data.contentDataType = WE_CONTENT_DATA_PIPE;
    content_data.contentSource = we_cmmn_strdup (handle->modid, op->src_path);
    break;
  case WeResourceBuffer:
    content_data.contentDataType = WE_CONTENT_DATA_RAW;
    break;
  default:
    content_data.contentDataType = WE_CONTENT_DATA_NONE;
    break;
  }

  content_data.contentType = we_cmmn_strdup (handle->modid, op->content_type);
  content_data.embeddedObjectExists = FALSE;
  content_data.embeddedObjectData = NULL;

  WE_CONTENT_SEND(handle->modid, action->modid, action->act_cmd, &content_data, FALSE, 0, FALSE);  

  WE_MEM_FREE (handle->modid, content_data.routingIdentifier);
  WE_MEM_FREE (handle->modid, content_data.contentUrl);
  WE_MEM_FREE (handle->modid, content_data.contentHeaders);
  if (content_data.contentSource)
    WE_MEM_FREE (handle->modid, content_data.contentSource);
  if (content_data.contentType)
    WE_MEM_FREE (handle->modid, content_data.contentType);
}


static int
we_act_execute_action (we_act_handle_t *handle, we_act_op_t *op, WE_INT32 action_id)
{
  const we_act_mime_action_t   *action = &(we_act_mime_action_table[action_id]);
  switch (action->act_type){
  case WE_MIME_ACTION_TYPE_EXTERNAL:
    TPIa_objectAction (action->act_cmd, op->mime_type, 
                       op->data_type, op->data, 
                       op->data_len, op->src_path, 
                       op->file_name, op->content_type);
    op->status = WE_ACT_OK;
    op->state = WE_ACT_STATE_FINISHED;
    op->caller_owns_pipe = FALSE;
    return WE_PACKAGE_OPERATION_COMPLETE;
  
  case WE_MIME_ACTION_TYPE_INTERNAL:
    we_act_content_route(handle, op, action);
    op->status = WE_ACT_OK;
    op->state = WE_ACT_STATE_FINISHED;
    op->caller_owns_pipe = FALSE;
    return WE_PACKAGE_OPERATION_COMPLETE;

  case WE_MIME_ACTION_TYPE_PACKET:
    if ( strcmp(action->act_cmd, WE_PACKET_OPERATION_ID_SAS) == 0){
      we_pck_result_t  result;
      WE_INT32         bit_flag = WE_SAS_ALLOW_OVERWRITE; /*Default value */
      we_sas_save_as_t save_as_data;

      if (!op->file_attrs) {
        op->file_attrs = create_attr_list_from_mime(handle->modid, op->mime_type);
      }

      save_as_data.handle = handle->sas_handle;
      save_as_data.file_name = op->file_name;
      save_as_data.mime_type = op->mime_type;
      save_as_data.data_type = op->data_type;
      save_as_data.data = op->data;
      save_as_data.data_len = op->data_len;
      save_as_data.pathname = op->src_path;
      save_as_data.file_attrs = op->file_attrs;
      save_as_data.screen = op->screen;
      save_as_data.bit_flag = bit_flag;
      save_as_data.title = 0;
      save_as_data.save_here_str = 0;

      op->sas_id = we_sas_create_save_as(&save_as_data);
      op->caller_owns_pipe = FALSE;

      we_sas_get_result(handle->sas_handle, op->sas_id, &result);

      if(((we_sas_result_save_as_t *)(result._u.data))->result!=WE_SAS_RETURN_OK && op->data_type==WE_CONTENT_DATA_PIPE && op->caller_owns_pipe==FALSE)
        WE_PIPE_DELETE((char *) op->data); // if data is pipe then delete pipe if save as failed

      switch (((we_sas_result_save_as_t *)(result._u.data))->result){
      case WE_SAS_RETURN_OK:
        op->status = WE_ACT_OK;
        op->state = WE_ACT_STATE_FINISHED;
        we_sas_result_free(handle->sas_handle, &result);
        return WE_PACKAGE_OPERATION_COMPLETE;

      case WE_SAS_RETURN_CANCELLED:
        op->status = WE_ACT_CANCELLED;
        op->state = WE_ACT_STATE_FINISHED;
        we_sas_result_free(handle->sas_handle, &result);
        return WE_PACKAGE_OPERATION_COMPLETE;

      case WE_SAS_RETURN_NOT_AVAILABLE:
        we_sas_result_free(handle->sas_handle, &result);
        op->status = WE_ACT_ERROR_DELAYED;
        op->state = WE_ACT_STATE_SAVE_WAIT;
        return WE_PACKAGE_SIGNAL_HANDLED;

      case WE_SAS_RETURN_ERROR:
        we_sas_result_free(handle->sas_handle, &result);
        goto error_label;
      }
    }
    else{
      /* Unknown packet action */
      op->status = WE_ACT_ERROR_INVALID;
      op->state = WE_ACT_STATE_FINISHED;
      return WE_PACKAGE_OPERATION_COMPLETE;
    }
  }

error_label:
  /* Error in packet action */
  op->status = WE_ACT_ERROR_INVALID;
  op->state = WE_ACT_STATE_FINISHED;
  return WE_PACKAGE_OPERATION_COMPLETE;
}


static void
we_act_create_action_menu (we_act_handle_t *handle, we_act_op_t *op)
{  
  we_act_window_t   *menu_window = get_new_window (handle, op);
  int                i = 0;
  we_act_int_elem_t *action_elem = op->int_elem;
  WE_INT32          *str_ids = (WE_INT32*) WE_MEM_ALLOC(handle->modid,
                                                  op->num_elem * sizeof(WE_INT32));
  while ((i < op->num_elem) && action_elem) {
    str_ids[i] = we_act_mime_action_table[action_elem->i].name_id;
    action_elem = action_elem->next;
    i++;
  }

  menu_window->dlg = we_drcCreate (handle->modid, op->screen,
                                    WE_DRC_OBJACT_SUBACTION_MENU,
                                    0, NULL, i, str_ids);

  WE_MEM_FREE(handle->modid, str_ids);
}



static int
we_act_handle_widget_action (we_act_handle_t *handle, WE_INT32 *wid, WeWindowHandle window_handle, WeActionHandle action_handle)
{
  we_act_op_t      *temp_op = handle->op;
  we_act_op_t      *found_op = NULL;
  we_act_window_t  *found_window = NULL;

  while (temp_op && (!found_op)) {
    we_act_window_t *window = temp_op->window;
    while (window && (!found_window)) {
      if (we_drcGetDialogHandle(window->dlg) == window_handle) {
        found_op = temp_op;
        found_window = window;
        *wid = temp_op->wid;
      }
      window = window->next;
    }
    temp_op = temp_op->next;
  }
  if (found_op) {
    int rsp;

    we_drcAction(found_window->dlg, action_handle);
    rsp = we_drcGetResponse(found_window->dlg);
    if (rsp == WE_DRC_RESPONSE_POSITIVE) {
      int                   i = 0;
      we_act_int_elem_t    *action_elem = found_op->int_elem;

      i = we_drcGetMenuIndex(found_window->dlg);
      while (i && action_elem) {
        i--;
        action_elem = action_elem->next;
      }

      if (!action_elem)
        return WE_PACKAGE_SIGNAL_HANDLED;

      free_act_window(handle, found_op, found_op->window);
      return we_act_execute_action (handle, found_op, action_elem->i);
    }
    else if (rsp == WE_DRC_RESPONSE_CANCEL){
      free_act_window(handle, found_op, found_op->window);
      found_op->status = WE_ACT_CANCELLED;
      found_op->state = WE_ACT_STATE_FINISHED;
      return WE_PACKAGE_OPERATION_COMPLETE;
    }
    return WE_PACKAGE_SIGNAL_HANDLED;
  }
  return WE_PACKAGE_SIGNAL_NOT_HANDLED;
}


/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialise the action package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t*
we_act_init (WE_UINT8 modid)
{
  we_act_handle_t *h = WE_MEM_ALLOCTYPE (modid, we_act_handle_t);

  if (h) {
    h->modid = modid;
    h->sas_handle = we_sas_init (modid);
    h->afi_handle = we_afi_init (modid);
    h->op = NULL;
  }

  return (we_pck_handle_t*)h;
}

/*
 * Runs the action package. When the module receives a signal that is
 * defined by WE, the module must call this function because the
 * action package might be the receiver.
 *
 * Returns:
 *    WE_PACKAGE_SIGNAL_NOT_HANDLED
 *      The signal was not handled by the package, the caller
 *      must process the signal by itself.
 *    WE_PACKAGE_SIGNAL_HANDLED
 *      The signal was handled by the package.
 *    WE_PACKAGE_OPERATION_COMPLETE
 *      The signal was handled by the package and the operation is
 *      finished. In this case the wid parameter indicates which operation
 *      that has finished. 
 */
int
we_act_handle_signal (we_pck_handle_t* handle, WE_UINT16 signal, void* p,
                       WE_INT32* wid)
{
  we_act_handle_t* act_handle = (we_act_handle_t*)handle;
  int dispatched;

  if (!handle || !p || !wid)
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;

  if (!(act_handle->op))
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;

  {
    WE_INT32 sas_id;
    int sas_return;
    sas_return = we_sas_handle_signal (act_handle->sas_handle, signal, p, &sas_id);
    if (sas_return == WE_PACKAGE_SIGNAL_HANDLED) {
      we_act_op_t *op = act_handle->op;
      while (op->sas_id != sas_id)
        op = op->next;
      *wid = op->wid;
      return WE_PACKAGE_SIGNAL_HANDLED;
    }
    else if (sas_return == WE_PACKAGE_OPERATION_COMPLETE) {
      we_pck_result_t result;
      we_act_op_t *op = act_handle->op;
      while (op->sas_id != sas_id)
        op = op->next;
      *wid = op->wid;

      we_sas_get_result(act_handle->sas_handle, op->sas_id, &result);

      if(((we_sas_result_save_as_t *)(result._u.data))->result!=WE_SAS_RETURN_OK && op->data_type==WE_CONTENT_DATA_PIPE && op->caller_owns_pipe==FALSE)
        WE_PIPE_DELETE((char *) op->data); // if data is pipe then delete pipe if save as failed

      switch (((we_sas_result_save_as_t *)(result._u.data))->result){
      case WE_SAS_RETURN_OK:
        op->status = WE_ACT_OK;
        op->state = WE_ACT_STATE_FINISHED;
        we_sas_result_free(act_handle->sas_handle, &result);
        return WE_PACKAGE_OPERATION_COMPLETE;

      case WE_SAS_RETURN_CANCELLED:
        op->status = WE_ACT_CANCELLED;
        op->state = WE_ACT_STATE_FINISHED;
        we_sas_result_free(act_handle->sas_handle, &result);
        return WE_PACKAGE_OPERATION_COMPLETE;

      case WE_SAS_RETURN_NOT_AVAILABLE:
        we_sas_result_free(act_handle->sas_handle, &result);
        op->status = WE_ACT_ERROR_DELAYED;
        op->state = WE_ACT_STATE_SAVE_WAIT;
        return WE_PACKAGE_SIGNAL_HANDLED;

      case WE_SAS_RETURN_ERROR:
        we_sas_result_free(act_handle->sas_handle, &result);
        op->status = WE_ACT_ERROR_INVALID;
        op->state = WE_ACT_STATE_FINISHED;
        return WE_PACKAGE_OPERATION_COMPLETE;
      }
    }
  }

  switch(signal) {
  case WE_SIG_WIDGET_ACTION:
    {
      we_widget_action_t* action = (we_widget_action_t*)p;
      WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, act_handle->modid,
                  "ACT: Received WE_SIG_WIDGET_ACTION\n"));
      dispatched = we_act_handle_widget_action (act_handle, wid, action->handle, action->action);
    }
    break;
  default:
    dispatched = WE_PACKAGE_SIGNAL_NOT_HANDLED;
    break;
  }
  return dispatched;
}

/*
 * Set private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after a
 * operation function has been called and an operation wid is available.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_act_set_private_data (we_pck_handle_t* handle, WE_INT32 wid, void* p)
{
  we_act_handle_t* act_handle = (we_act_handle_t*)handle;
  we_act_op_t *op = find_op_by_id (act_handle, wid);
  if (!op)
    return FALSE;
  op->priv_data = p;
  return TRUE;
}

/*
 * Get private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after
 * we_act_handle_signal has returned WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns NULL.
 */
void*
we_act_get_private_data (we_pck_handle_t* handle, WE_INT32 wid)
{
  we_act_handle_t* act_handle = (we_act_handle_t*)handle;
  we_act_op_t *op = find_op_by_id (act_handle, wid);
  if (!op)
    return NULL;
  return op->priv_data;
}


/*
 * Terminates the action package, release all resources allocated by
 * this package. If there are any unfinished operations, these operations are
 * deleted. Any private data must be released by the module itself.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_act_terminate (we_pck_handle_t* handle)
{
  we_act_handle_t* act_handle = (we_act_handle_t*)handle;

  if (!act_handle)
    return FALSE;

  while (act_handle->op){
    switch (act_handle->op->state) {
      case WE_ACT_STATE_SAVE_WAIT :
      case WE_ACT_STATE_AFI_OPEN_BUSY : 
      case WE_ACT_STATE_AFI_CLOSE_BUSY : return FALSE;
      default : break;
    }
    free_act_op(act_handle, act_handle->op);
  }
  we_sas_terminate(act_handle->sas_handle);
  we_afi_terminate(act_handle->afi_handle);
  WE_MEM_FREE(act_handle->modid, act_handle);
  
  return TRUE;
}

/**********************************************************************
 * Delete 
 **********************************************************************/

/*
 * Deletes an unfinished operation. Any private data is not released by this
 * function. The module call this function when the module of any
 * reason must delete the operation before the finished state has been
 * reached.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_act_delete (we_pck_handle_t* handle, WE_INT32 wid)
{
  we_act_handle_t* act_handle = (we_act_handle_t*)handle;
  we_act_op_t *op = find_op_by_id (act_handle, wid);
  
  if (!op)
    return FALSE;

  switch (op->state) {
    case WE_ACT_STATE_SAVE_WAIT :
    case WE_ACT_STATE_AFI_OPEN_BUSY : 
    case WE_ACT_STATE_AFI_CLOSE_BUSY : return FALSE;

    default : break;
  }

  free_act_op(act_handle, op);
  return TRUE;
}
/**********************************************************************
 * Result
 **********************************************************************/

/*
 * Get the result of a completed operation. The caller must
 * call this function when we_act_handle_signal has returned
 * WE_PACKAGE_OPERATION_COMPLETE for this specific operation. This
 * function also releases all allocated resources connected to the
 * specific operation , but only if the function is called when the
 * operation is finished. Any private data associated with this 
 * operation must be freed by the using module before this function
 * is called.
 *
 * Arguments:
 * handle       The handle to the struct that holds action package information.
 * wid           The wid to the operation instance
 * result       The function initializes this parameter with current result
 *              type containing status code and optionally result
 *              data of the operation.
 *
 *              function                  result has a data that should be casted to:
 *              we_act_get_actions       we_act_result_get_actions_t *
 *              we_act_object_action     WE_INT32 (WE_ACT_ return codes)
 *
 *              function                  result has a additional_data that should be casted to:
 *              we_act_object_action     we_act_error_result_object_action_t in case of 
 *                                        WE_PACKAGE_ERROR or WE_ACT_CANCELLED
 *
 *
 * Returns:     WE_PACKAGE_ERROR on failure
 *              WE_PACKAGE_COMPLETED if operation has finished
 *              WE_PACKAGE_BUSY if operation has not finished yet
 */

int
we_act_get_result(we_pck_handle_t* handle, WE_INT32 wid, we_pck_result_t* result)
{
  we_act_handle_t* act_handle = (we_act_handle_t*)handle;
  we_act_op_t *op;
  
  if (!result)
    return WE_PACKAGE_ERROR;

  result->type = 0;
  result->_u.i_val = 0;
  result->additional_data = NULL;
  
  if (!handle)
    return WE_PACKAGE_ERROR;

  op = find_op_by_id (act_handle, wid);
  if (op == NULL)
    return WE_PACKAGE_ERROR;
  
  result->type = op->type;

  switch (op->type){
  
  case WE_ACT_GET_ACTIONS:
    {
      we_act_result_get_actions_t *get_act_res;
      we_act_int_elem_t           *p = op->int_elem;
      we_act_action_entry_t       *q; 
      we_act_int_elem_t           *next;
      WE_INT32                    n = 0;

      /* copy result from list into array */
      if (op->num_elem > 1)
        n = op->num_elem - 1;
      get_act_res = WE_MEM_ALLOC(act_handle->modid, sizeof(we_act_result_get_actions_t) +
                                  (n * sizeof(we_act_action_entry_t)));
      q = get_act_res->action_list;
      while (p != NULL) {
        q->action_id = p->i;
        q->string_id = we_act_mime_action_table[p->i].name_id;
        q->act_cmd = we_cmmn_strdup(act_handle->modid,
                                     we_act_mime_action_table[p->i].act_cmd);
        q++;
        p = p->next;
      }
      get_act_res->num_actions = op->num_elem;

      /* remove list */
      p = op->int_elem;
      while (p != NULL) {
        next = p->next;
        WE_MEM_FREE(act_handle->modid, p);
        p = next;
      }
      op->int_elem = NULL;
      op->num_elem = 0;

      get_act_res->result = op->status;
      result->_u.data = get_act_res;
      free_act_op(act_handle, op);
      return WE_PACKAGE_COMPLETED;
    }


  case WE_ACT_OBJECT_ACTION:
    {
      we_act_error_result_object_action_t *err_res = NULL;

      result->_u.i_val = op->status;
      if ((op->status != WE_ACT_OK) && (op->status != WE_ACT_ERROR_DELAYED)) {
        err_res = WE_MEM_ALLOC(act_handle->modid,
                                sizeof(we_act_error_result_object_action_t));
        err_res->caller_owns_pipe = op->caller_owns_pipe;
      }
      switch (op->status){
      case WE_ACT_OK:
        free_act_op(act_handle, op);
        return WE_PACKAGE_COMPLETED;
      case WE_ACT_CANCELLED:
        result->additional_data = err_res;
        free_act_op(act_handle, op);
        return WE_PACKAGE_COMPLETED;
      case WE_ACT_ERROR_DELAYED:
        return WE_PACKAGE_BUSY;
      case WE_ACT_ERROR_ACCESS:
      case WE_ACT_ERROR_PATH:
      case WE_ACT_ERROR_INVALID:
        result->additional_data = err_res;
        free_act_op(act_handle, op);
        return WE_PACKAGE_ERROR;
      }
      return WE_PACKAGE_ERROR;
    }
  }
  
  return WE_PACKAGE_ERROR;
}

/*
 * Free the storage allocated in the get result function
 *
 * Argument:
 * result       Result data to be freed.
 *
 * Returns:     TRUE if success, otherwise FALSE. 
 */

int
we_act_result_free(we_pck_handle_t* handle, we_pck_result_t* result)
{
  we_act_handle_t* act_handle = (we_act_handle_t*)handle;

  if (!act_handle || !result)
    return FALSE;

  switch (result->type){
  
  case WE_ACT_GET_ACTIONS: {
    we_act_result_get_actions_t *get_act_res = (we_act_result_get_actions_t*) (result->_u.data);
    we_act_action_entry_t *q = get_act_res->action_list;
    WE_INT32 i = get_act_res->num_actions;
    while (i > 0) {
      WE_MEM_FREE(act_handle->modid, q->act_cmd);
      q++;
      i--;
    } 
    WE_MEM_FREE(act_handle->modid, get_act_res);
    break;
    }

  case WE_ACT_OBJECT_ACTION:
    WE_MEM_FREE(act_handle->modid, result->additional_data);
    result->additional_data = NULL;
    break;
  }

  return TRUE;
}

/**********************************************************************
 * Get actions
 **********************************************************************/

/* This operation is used to recieve the actions available for a specific
 * mime type.
 *
 * If a non-NULL content_type is passed in, then capability checking
 * might be applied besides checking the mime_type. That is, only
 * those actions with an associated mime_type and with some capability
 * will be returned by this function.
 *
 * The content_type is typically the DRM type or for example some envelope
 * type.
 * 
 * Presently capability checking for content type is applied if the
 * content_type is non-NULL and is equal to either the DRM-types
 * "application/vnd.oma.drm.content" or "application/vnd.oma.drm.message".
 * For these content_types, it is checked whether the action has been configured
 * with DRM capability (see WE_ACTION_CAPABILITY_DRM in We_Cfg.h).
 *
 * The parameter data_type can also be used to check capabilities. However,
 * presenlty there are no capability-checking that is triggered by the data_type.
 * 
 * handle                    The handle to the struct that holds all action package 
 *                           information.
 * mime_type                 The suitable actions for this mime type will be recieved
 * data_type                 Tells if the content is in a file, pipe or buffer.
 * content_type              Set to NULL if content type is same as mime_type.
 *                           Set also to NULL to turn of capability checking for
 *                           content type. Otherwise, set to DRM type or other 
 *                           envelope type.
 * check_blocked_permission  DRM permission of the file. Specifïes the
 *                           permission(s) to be checked wether blocked  
 *                           or not (requested permission).
 * excl_act_strings          An array of string pointers. Each string
 *                           (or command string) identifies an action
 *                           or a sub-action. All actions.and sub-actions
 *                           in this array will be excluded from the result.
 *                           That is, they are excluded when searching for
 *                           actions for the given mime type. NULL may be
 *                           passed. 
 * excl_act_strings_cnt      The number of strings in excl_act_strings.
 *
 * Returns:      An wid to this get action operation or WE_PACKAGE_ERROR
 *   
 */

WE_INT32
we_act_get_actions(we_pck_handle_t  *handle,
                    const char        *mime_type,
                    WeResourceType    data_type,
                    const char*        content_type,
                    WE_INT32          check_blocked_permission,
                    const char*        excl_act_strings[],
                    WE_INT32          excl_act_strings_cnt)
{
  we_act_handle_t            *act_handle = (we_act_handle_t*) handle;
  we_act_op_t                *op;
  const we_act_mime_action_t *action = we_act_mime_action_table;
  we_act_int_elem_t          *found_list = NULL;
  we_act_int_elem_t         **last_link = &found_list;
  we_act_int_elem_t          *p;
  int                          i;
  WE_INT32                    wid;

  if (!act_handle)
    return WE_PACKAGE_ERROR;

  op = get_new_op (act_handle);
  op->type = WE_ACT_GET_ACTIONS;
  op->num_elem = 0;
  op->data_type = data_type;
  if (content_type)
    op->content_type = we_cmmn_strdup(act_handle->modid, content_type);


  /* Search for matching mime types in the we_act_mime_action_table.
     Collect matching non-parent actions and parents of matching sub-actions. */
  for (i=0; i < WE_NBR_OF_MIME_ACTIONS; i++, action++) {

    /* skip checking parents */
    if (action->act_type == WE_MIME_ACTION_TYPE_CATEGORY)
      continue;

    if (we_pck_mime_type_in_list(mime_type, action->mime_types)) {

      /* skip checking actions with wrong permission */
      if ((action->blocked_permissions != WE_PERMISSION_NONE_BLOCKED) &&
          (action->blocked_permissions & check_blocked_permission))
        continue;

      /* skip actions with wrong capabilities */
      if (!we_act_capabilities_found(op, action))
        continue;

      /* skip excluded actions */
      if (we_act_found_in_str_array(action->act_cmd, excl_act_strings,
                                     excl_act_strings_cnt))
        continue; 

      wid = -1; /* The wid to add to the found list. -1 msans nothing to add */

      if (action->category < 0) /* i.e. has no parent */
        wid = i;

      else if (action->category < WE_NBR_OF_MIME_ACTIONS) {  /* i.e has valid parent */
        /* check if its parent is already in the found_list */
        for (p = found_list; p != NULL && p->i != action->category; p = p->next)
          ;
        if (p == NULL) /* not in found_list, add the match if parent is not excluded */
          if (!we_act_found_in_str_array(we_act_mime_action_table[action->category].act_cmd,
                                          excl_act_strings,
                                          excl_act_strings_cnt))
            wid = action->category;
      }

      if (wid >= 0) {
        /* add to found_list */
        p = WE_MEM_ALLOCTYPE(act_handle->modid, we_act_int_elem_t);
        p->i = wid;
        p->next = NULL;
        *last_link = p;
        last_link = &(p->next);
        op->num_elem++;
      }
    } /* end if we_pck_mime_type_in_list */

  } /* end for */

  op->int_elem = found_list;
  op->status = WE_ACT_OK;
  return op->wid;
}


/**********************************************************************
 * Object action
 **********************************************************************/

/* This operation performs the action specified by the action_id. If the action
 * needs more specified information from the end user, these dialogs are handled by
 * the operation
 * 
 * In case of succcessful result, the responsibility of any pipe content passed in,
 * is passed on to the action. In case of cancel or error, a pipe content might have 
 * been deleted. Whether a pipe was deleted or not during cancel or error, is indicated 
 * in the we_act_error_result_object_action_t returned in we_act_get_result. In case 
 * of cancel or failure and the pipe was not deleted, the responsibilty remains by the
 * caller.
 * 
 * Error handling
 * If object action returns WE_ACT_OK, the caller does not have to do any error handling.
 * However when WE_ACT_OK is returned for external and internal non-package actions, 
 * the receiver of the action may detect some error and in these cases it is the receiver 
 * that shall do error handling, if any.
 *
 * If object action returns an error, the act package will have made common error handling
 * that is applicable. Presently this is presenting an error dialog to the user.
 * Further, the only common error handling that presently might occur is during internal 
 * package actions (e.g. "save as" was cancelled or "save as" failed of other reasons).
 *
 * When object action returns an error, it is up to the caller to do other caller specific
 * error handling besides what has been done in the package.
 *
 * 
 * handle                    The handle to the struct that holds all action package 
 *                           information.
 * action_id                 The identifier to the action to perform
 * content                   The content data to perform an action on. All members 
 *                           except the data memeber is copied by the act package 
 *                           when starting the operation.
 *                           Note: If the content is of buffer type, the data member  
 *                           must be available until the operation is complete.
 *                           All members are passed on to the receiver of the object
 *                           action.
 * file_attrs                The file attributes of the file to perform the action on.
 * check_blocked_permission  DRM permission of the file. Specifïes the
 *                           permission(s) to be checked whether blocked  
 *                           or not (requested permission).
 * screen                    The screen that should be used for dialogs.
 * excl_act_strings          An array of string pointers. Each string
 *                           (or command string) identifies an action
 *                           or a sub-action. All sub-actions in this
 *                           array will be excluded in any dialogs to 
 *                           the end user. The same excl_act_strings
 *                           parameter as in we_act_get_actions should
 *                           be passed here.¨NULL may be passed.
 * excl_act_strings_cnt      The number of strings in excl_act_strings.
 *
 * Returns:     An wid to this object action operation or WE_PACKAGE_ERROR
 *   
 */
WE_INT32
we_act_object_action(we_pck_handle_t          *handle,
                      WE_INT32                  action_id,
                      const we_act_content_t   *content,
                      const we_pck_attr_list_t *file_attrs,
                      WE_INT32                  check_blocked_permission,
                      WeScreenHandle            screen,
                      const char*                excl_act_strings[],
                      WE_INT32                  excl_act_strings_cnt)
{
  we_act_handle_t*   act_handle = (we_act_handle_t*)handle;
  we_act_op_t       *op;

  if (!act_handle || !content) {
    return WE_PACKAGE_ERROR;
  }

  op = get_new_op (act_handle);
  op->data_type = content->data_type;
  op->data = content->data;
  op->data_len = content->data_len;
  op->screen = screen;
  op->type = WE_ACT_OBJECT_ACTION;

  if (content->mime_type && (action_id >= 0) && (action_id < WE_NBR_OF_MIME_ACTIONS))
    op->mime_type = we_cmmn_strdup(act_handle->modid, content->mime_type);
  else{
    free_act_op(act_handle, op);
    return WE_PACKAGE_ERROR;
  }

  if (content->src_path){
    op->src_path = WE_MEM_ALLOC(act_handle->modid, strlen(content->src_path) + 1 );
    strcpy(op->src_path, content->src_path);
  }

  op->caller_owns_pipe = content->src_path && (content->data_type == WeResourcePipe);

  if (file_attrs)
    op->file_attrs = we_pck_attr_dupl (act_handle->modid, file_attrs);

  if (content->default_name)
    op->file_name = we_cmmn_strdup(act_handle->modid, content->default_name);

  if (content->content_type)
    op->content_type = we_cmmn_strdup(act_handle->modid, content->content_type);

  we_act_put_action_childs_in_op(handle, op, action_id, check_blocked_permission,
                                  excl_act_strings, excl_act_strings_cnt);

  if (op->int_elem){
    op->state = WE_ACT_STATE_MENU_WAIT;
    op->status = WE_ACT_ERROR_DELAYED;
    we_act_create_action_menu(act_handle, op);
  }
  else{
    we_act_execute_action(handle, op, action_id);
  }

  return op->wid;
}




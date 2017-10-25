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
 * We_Sas.c
 *
 * Save as package:
 * 
 *
 * Created by Simon Magnusson, May 30 2003.
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
#include "We_Dlg.h"
#include "We_Afi.h"
#include "We_Mem.h"
#include "We_Sas.h"
#include "We_Pipe.h"

/**********************************************************************
 * Configuration
 **********************************************************************/

#define WE_SAS_BAR_POSITION            {5,10} /* Position in pixels */
#define WE_SAS_BAR_WINDOW_SIZE_HEIGHT  0.9 /* Size proportional to screen size */
#define WE_SAS_BAR_WINDOW_SIZE_WIDTH   0.9 /* Size proportional to screen size */
#define WE_SAS_BAR_SIZE_HEIGHT  0.1 /* Size proportional to window size */

/**********************************************************************
 * Defines 
 **********************************************************************/

#define WE_SAS_STATE_DLG_WAIT      1
#define WE_SAS_STATE_AFI_SAVE_WAIT 2
#define WE_SAS_STATE_MESSAGE_WAIT  3
#define WE_SAS_STATE_REMOVE_WAIT   4
#define WE_SAS_STATE_DELETE_WAIT   5
#define WE_SAS_STATE_FINISHED      6

typedef struct we_sas_op_st {
  struct we_sas_op_st  *next;
  WE_INT32              wid;          /* operation identifier */
  int                    state;
  void                  *priv_data;
  long                   status;    /* status of operation */
  char                  *mime_type;
  char                  *file_name;
  int                    data_type;
  void                  *data;
  long                   data_len;
  char                  *pathname;
  WE_INT32              dlg_id;
  WE_INT32              afi_id;
  we_pck_attr_list_t   *file_attrs;
  WE_INT32              bit_flag;
  WE_INT32              do_not_show_dialog;
  WeScreenHandle        screen;
  we_dlg_progress_bar_t *progress_data;
  WE_INT32              progess_dlg_id;
  WE_INT32              error_dlg_id;
} we_sas_op_t;

typedef struct we_sas_handle_st {
  WE_UINT8                  modid;
  we_sas_op_t              *op;
  we_pck_handle_t          *afi_handle;
  we_pck_handle_t          *dlg_handle;
} we_sas_handle_t;



/**********************************************************************
 * Local functions
 **********************************************************************/
#ifdef WE_CFG_DRM_DIRECTORY_STORAGE
/* Lowercase version of strstr */
static char *
we_strstr_lc(WE_UINT8 modId, const char *s1, const char *s2)
{

  char *dst1        = NULL;
  char *dst2        = NULL;
  char *result_dst1 = NULL;
  char *result_s1   = NULL;
  
  if ((s1 != NULL) && (s2 != NULL)) {
    dst1 = (char*)WE_MEM_ALLOC (modId, strlen(s1)+1);
    dst2 = (char*)WE_MEM_ALLOC (modId, strlen(s2)+1);

    we_cmmn_strcpy_lc (dst1, s1);
    we_cmmn_strcpy_lc (dst2, s2);

    result_dst1 = strstr (dst1, dst2);

    if(result_dst1 != NULL) {
      /* Pointer to first occurence in original string */
      result_s1 = ((char*)s1 + (result_dst1 - dst1));
    }
    
    WE_MEM_FREE(modId, dst1);
    WE_MEM_FREE(modId, dst2);
  }

  return result_s1;
}

static char*
get_mime_from_attr_list (we_pck_attr_list_t *attrlist)
{ 
  we_pck_attr_list_t *attribute = attrlist;

  while (attribute) {
    if (attribute->attribute == WE_PCK_ATTRIBUTE_MIME &&
        attribute->type == WE_PCK_ATTR_TYPE_STRING) {
       return attribute->_u.s;
    }
    attribute = attribute->next;
  }
  return NULL;
}

static void
we_sas_convert_mime_to_drm_mime (we_sas_handle_t *handle, we_sas_op_t *op)
{
  char *mime_type = NULL;
  
  mime_type = get_mime_from_attr_list (op->file_attrs);
  
  if (mime_type == NULL) {
    return;
  }

  if (we_strstr_lc (handle->modid, mime_type, "application/vnd.oma.drm.")) {

    char *new_mime_type = NULL;
    new_mime_type = we_cmmn_strcat(handle->modid, CONVERTED_DRM_MIME, op->mime_type);
    WE_MEM_FREE (handle->modid, op->mime_type);
    op->mime_type = new_mime_type;
  }
}

#endif /* WE_CFG_DRM_DIRECTORY_STORAGE */

static we_sas_op_t *
find_op_by_id (we_sas_handle_t *handle, WE_INT32 wid)
{
  we_sas_op_t *op = handle->op;

  while (op) {
    if (op->wid == wid)
      return op;
    op = op->next;
  }
  return NULL;
}


static we_sas_op_t *
get_new_op (we_sas_handle_t *handle)
{
  static int we_sas_uniq_id = 1;

  we_sas_op_t *op = WE_MEM_ALLOCTYPE (handle->modid, we_sas_op_t);

  op->wid = we_sas_uniq_id++;
  op->state = 0;
  op->mime_type = NULL;
  op->file_name = NULL;
  op->data_type = 0;
  op->data = NULL;
  op->data_len = 0;
  op->pathname = NULL;
  op->file_attrs = NULL;
  op->priv_data = NULL;
  op->dlg_id = 0;
  op->afi_id = 0;
  op->status = WE_SAS_RETURN_NOT_AVAILABLE;
  op->bit_flag = 0;
  op->do_not_show_dialog = FALSE;
  op->screen = 0;
  op->progress_data = NULL;

  /* insert new operation first in list */
  op->next = handle->op;
  handle->op = op;

  return op;
}


static void
free_sas_op (we_sas_handle_t *handle, we_sas_op_t *op)
{
  we_sas_op_t *temp_op = handle->op;
  we_sas_op_t *prev_op = NULL;

  /*Remove op*/
  while (temp_op != op) {
    prev_op = temp_op;
    temp_op = temp_op->next;
  }
  if (prev_op) {
    prev_op->next = op->next;
  }
  else {
    handle->op = op->next;
  }

  /*Free op*/
  if (op->state == WE_SAS_STATE_AFI_SAVE_WAIT) {
    we_pck_result_t delete_result;

    op->afi_id = we_afi_delete(handle->afi_handle, op->afi_id);
    we_afi_get_result (handle->afi_handle, op->afi_id, &delete_result);
    we_afi_result_free(handle->afi_handle, &delete_result);
  }

  if (op->progess_dlg_id) {
    we_dlg_delete (handle->dlg_handle, op->progess_dlg_id);
    op->progess_dlg_id = 0;
  }


  if (op->progress_data) {
    WE_MEM_FREE (handle->modid, op->progress_data);
  }
  WE_MEM_FREE (handle->modid, op->mime_type);
  we_pck_attr_free (handle->modid, op->file_attrs);
  WE_MEM_FREE (handle->modid, op->file_name);
  WE_MEM_FREE (handle->modid, op->pathname);
  WE_MEM_FREE (handle->modid, op);
  op = NULL;
}

static WE_INT32
get_size_from_attr_list (const we_pck_attr_list_t *attr)
{
  while (attr) {
    if (attr->attribute == WE_PCK_ATTRIBUTE_SIZE) {
      return attr->_u.i;
    }
    attr = attr->next;
  }

  return 0;
}

static int
we_sas_convert_afi_error(int afi_error)
{
   return afi_error-3;
}

static void
we_sas_create_error_dialog (we_sas_handle_t *handle,
                             we_sas_op_t     *op,
                             int               mess_id)
{
  we_dlg_message_t dlg_data;

  dlg_data.dialog_type = WeError;
  dlg_data.dlg_data.handle = handle->dlg_handle;
  dlg_data.dlg_data.screen = op->screen;
  dlg_data.dlg_data.title = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_SAS_ERROR_TITLE);
  dlg_data.nbr_of_str_ids = 1;
  dlg_data.str_ids = WE_MEM_ALLOCTYPE (handle->modid, WeStringHandle);
  dlg_data.str_ids[0] = WE_WIDGET_STRING_GET_PREDEFINED (mess_id);
  dlg_data.ok_string = 0;
  op->error_dlg_id = we_dlg_create_message (&dlg_data);
  WE_MEM_FREE (handle->modid,dlg_data.str_ids);
}

static void
we_sas_create_progressbar (we_sas_handle_t *handle, we_sas_op_t *op)
{  
  long  file_size = 0;

  op->progress_data = WE_MEM_ALLOCTYPE (handle->modid, we_dlg_progress_bar_t);
  file_size = get_size_from_attr_list(op->file_attrs);

  op->progress_data->propmask = WE_DLG_PROGRESS_SHOW_TEXT_TWO | WE_DLG_PROGRESS_NUMBER;
  op->progress_data->text_two = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_SAS_BYTES_SAVED);
  if (file_size == 0) {
    file_size = WE_SAS_CFG_BAR_DEFAULT_MAX_VALUE;    
  }

  op->progress_data->cancel_string = 0;
  op->progress_data->dlg_data.handle = handle->dlg_handle;
  op->progress_data->dlg_data.screen = op->screen;
  op->progress_data->dlg_data.title = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_SAS_BAR_TITLE);
  op->progress_data->max_value = file_size;
  op->progress_data->text_one = 0;
  op->progress_data->value = 0;
  
  op->progess_dlg_id = we_dlg_create_progressbar(op->progress_data);
}

static void
we_sas_update_progressbar (we_sas_handle_t *handle,
                            we_sas_op_t     *op,
                            long              bytes_saved)
{

  op->progress_data->value = (int)bytes_saved;

  we_dlg_update_progressbar (handle->dlg_handle, op->progess_dlg_id, op->progress_data);
}


static int
handle_dlg_complete (we_sas_handle_t* handle, WE_INT32 dlg_id, WE_INT32* wid)
{      
  we_pck_result_t           dlg_result;
  we_sas_op_t              *op = handle->op;
  we_dlg_result_save_as_t  *save_as_result;
  we_dlg_result_progress_t *progress_result;

  while (op->dlg_id != dlg_id && 
         op->progess_dlg_id != dlg_id && 
         op->error_dlg_id != dlg_id) {
    
    op = op->next;
  }
  *wid = op->wid;
  we_dlg_get_result (handle->dlg_handle, dlg_id, &dlg_result);
  if (op->dlg_id == dlg_id) {

    if (dlg_result.type == WE_DLG_TYPE_SAVE_AS) {

      save_as_result = ((we_dlg_result_save_as_t *)dlg_result._u.data); 
      if (save_as_result->result == WE_DLG_RETURN_OK) {

        we_pck_result_t  save_result;
        WE_INT32         res_value;

        op->file_name = we_cmmn_strdup (handle->modid, save_as_result->file_name);
        we_dlg_result_free (handle->dlg_handle, &dlg_result);

        /* create progressbar */
        we_sas_create_progressbar (handle, op);
      
        op->afi_id = we_afi_save (handle->afi_handle, op->file_name,
                                   op->data_type, op->data_len,
                                   op->data, op->pathname, op->file_attrs);
        we_afi_get_result (handle->afi_handle, op->afi_id, &save_result);
        res_value = save_result._u.i_val;

        if (save_result.type == WE_AFI_SAVE) {
          switch (res_value) {

          case  WE_AFI_ERROR_DELAYED:
            we_afi_result_free (handle->afi_handle, &save_result);
            op->state = WE_SAS_STATE_AFI_SAVE_WAIT;
            return WE_PACKAGE_SIGNAL_HANDLED;

          case WE_AFI_ERROR_FULL:
            we_afi_result_free (handle->afi_handle, &save_result);
            we_sas_create_error_dialog (handle, op, WE_STR_ID_SAS_ERROR_FULL);
            op->status = WE_SAS_ERROR_FULL;
            op->state = WE_SAS_STATE_MESSAGE_WAIT;
            return WE_PACKAGE_SIGNAL_HANDLED;

          case WE_AFI_ERROR_SIZE:
            we_afi_result_free (handle->afi_handle, &save_result);
            we_sas_create_error_dialog (handle, op, WE_STR_ID_SAS_ERROR_SIZE);
            op->status = WE_SAS_ERROR_SIZE;
            op->state = WE_SAS_STATE_MESSAGE_WAIT;
            return WE_PACKAGE_SIGNAL_HANDLED;

          case WE_AFI_OK:
            we_afi_result_free (handle->afi_handle, &save_result);
            if (op->progess_dlg_id) {
                we_dlg_delete (handle->dlg_handle, op->progess_dlg_id);
                op->progess_dlg_id = 0;
            }
            op->status = WE_SAS_RETURN_OK;
            op->state = WE_SAS_STATE_FINISHED;
            return WE_PACKAGE_OPERATION_COMPLETE;
          default:
            we_afi_result_free (handle->afi_handle, &save_result);
            op->status = we_sas_convert_afi_error(res_value); /* let error propagate upwards */
            op->state = WE_SAS_STATE_FINISHED;
            return WE_PACKAGE_OPERATION_COMPLETE;
          }
        }
      }
      else if (save_as_result->result == WE_DLG_RETURN_CANCELLED) {
        we_dlg_result_free(handle->dlg_handle, &dlg_result);
        op->status = WE_SAS_RETURN_CANCELLED;
        op->state = WE_SAS_STATE_FINISHED;
        return WE_PACKAGE_OPERATION_COMPLETE;
      }
    }
    we_dlg_result_free(handle->dlg_handle, &dlg_result);
  }
  else if (op->progess_dlg_id == dlg_id) {
    progress_result = ((we_dlg_result_progress_t *)dlg_result._u.data);
    we_dlg_result_free(handle->dlg_handle, &dlg_result);
    we_afi_delete (handle->afi_handle, op->afi_id); 
    op->status = WE_SAS_RETURN_CANCELLED;
    op->state = WE_SAS_STATE_FINISHED;
    return WE_PACKAGE_OPERATION_COMPLETE;
  }
  else if (op->error_dlg_id == dlg_id) {
    we_dlg_result_free(handle->dlg_handle, &dlg_result);
    op->status = WE_SAS_RETURN_CANCELLED;
    op->state = WE_SAS_STATE_FINISHED;
    return WE_PACKAGE_OPERATION_COMPLETE;
  }
  return WE_PACKAGE_SIGNAL_HANDLED;
}


/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialise the save as package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t*
we_sas_init (WE_UINT8 modid)
{
  we_sas_handle_t *h = WE_MEM_ALLOCTYPE (modid, we_sas_handle_t);

  if (h != NULL) {
    h->modid = modid;
    h->afi_handle = we_afi_init (modid);
    h->dlg_handle = we_dlg_init (modid);
    h->op = NULL;
  }

  return (we_pck_handle_t*)h;
}

/*
 * Runs the save as package. When the module receives a signal that is
 * defined by WE, the module must call this function because the
 * dialog package might be the receiver.
 *
 * Returns:
 *    WE_PACKAGE_SIGNAL_NOT_HANDLED
 *      The signal was not handled by the package, the calling module
 *      must process the signal by itself.
 *    WE_PACKAGE_SIGNAL_HANDLED
 *      The signal was handled by the package.
 *    WE_PACKAGE_OPERATION_COMPLETE
 *      The signal was handled by the package and the save as operation is 
 *      finished. In this case the wid parameter indicates which operation that 
 *      is finished. 
 */
int
we_sas_handle_signal (we_pck_handle_t *handle,
                       WE_UINT16        signal,
                       void             *p,
                       WE_INT32        *wid)
{
  we_sas_handle_t *sas_handle = (we_sas_handle_t*)handle;
  WE_INT32 dlg_id;
  int       dlg_return;
  WE_INT32 afi_id;
  int       afi_return;

  if ((handle == NULL) || (sas_handle->op == NULL)) {
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;
  }

  dlg_return = we_dlg_handle_signal (sas_handle->dlg_handle, signal, p, &dlg_id);
  if (dlg_return == WE_PACKAGE_SIGNAL_HANDLED) {
    we_sas_op_t *op = sas_handle->op;

    while (op->dlg_id != dlg_id && op->error_dlg_id != dlg_id) {
      op = op->next;
    }
    *wid = op->wid;

    return WE_PACKAGE_SIGNAL_HANDLED;
  }
  else if (dlg_return == WE_PACKAGE_OPERATION_COMPLETE) {
    return handle_dlg_complete (sas_handle, dlg_id, wid);
  }


  afi_return = we_afi_handle_signal (sas_handle->afi_handle, signal, p, &afi_id);
  if (afi_return == WE_PACKAGE_SIGNAL_HANDLED) {
    we_pck_result_t afi_result;
    we_sas_op_t    *op = sas_handle->op;

    while (op->afi_id != afi_id) {
      op = op->next;
    }
    *wid = op->wid;
    /* get the result to update progressbar */
    we_afi_get_result(sas_handle->afi_handle, afi_id, &afi_result);
    /* call progressbar update*/
    if (afi_result.type == WE_AFI_SAVE) {
      we_afi_additional_data_save_t *add;

      add = ((we_afi_additional_data_save_t *)afi_result.additional_data);
      we_sas_update_progressbar (sas_handle, op, add->bytes_saved);
    }
    we_afi_result_free(sas_handle->afi_handle, &afi_result);

    return WE_PACKAGE_SIGNAL_HANDLED;
  }
  else if (afi_return == WE_PACKAGE_OPERATION_COMPLETE) { 
    we_sas_op_t     *op = sas_handle->op;
    we_pck_result_t  afi_result;
    WE_INT32         res;

    if (op->progess_dlg_id) {
      we_dlg_delete (sas_handle->dlg_handle, op->progess_dlg_id);
      op->progess_dlg_id = 0;
    }

    we_afi_get_result(sas_handle->afi_handle, afi_id, &afi_result);
    while (op->afi_id != afi_id) {
      op = op->next;
    }
    *wid = op->wid;
    if (op->state == WE_SAS_STATE_REMOVE_WAIT) {
      we_afi_result_free(sas_handle->afi_handle, &afi_result);
      op->state = WE_SAS_STATE_FINISHED;

      return WE_PACKAGE_OPERATION_COMPLETE;
    }
    else if (op->state == WE_SAS_STATE_DELETE_WAIT) {
      we_afi_result_free(sas_handle->afi_handle, &afi_result);
      op->afi_id = we_afi_remove (sas_handle->afi_handle, op->file_name);
      if (op->afi_id >= 0){
        if (we_afi_get_result (sas_handle->afi_handle, op->afi_id, &afi_result) == WE_PACKAGE_BUSY) {
          we_afi_result_free(sas_handle->afi_handle, &afi_result);
          op->state = WE_SAS_STATE_REMOVE_WAIT;

          return WE_PACKAGE_SIGNAL_HANDLED;
        }
        we_afi_result_free(sas_handle->afi_handle, &afi_result);
      }
      op->state = WE_SAS_STATE_FINISHED;

      return WE_PACKAGE_OPERATION_COMPLETE;
    }
    res = (WE_INT32)afi_result._u.data;
    if (res != WE_AFI_OK) {
      if (res == WE_AFI_ERROR_FULL) {
        we_afi_result_free (sas_handle->afi_handle, &afi_result);
        we_sas_create_error_dialog (sas_handle, op, WE_STR_ID_SAS_ERROR_FULL);
        op->status = WE_SAS_ERROR_FULL;
        op->state = WE_SAS_STATE_MESSAGE_WAIT;
        return WE_PACKAGE_SIGNAL_HANDLED;
      }
      if (res == WE_AFI_ERROR_SIZE) {
        we_afi_result_free (sas_handle->afi_handle, &afi_result);
        we_sas_create_error_dialog (sas_handle, op, WE_STR_ID_SAS_ERROR_SIZE);
        op->status = WE_SAS_ERROR_SIZE;
        op->state = WE_SAS_STATE_MESSAGE_WAIT;
        return WE_PACKAGE_SIGNAL_HANDLED;
      }
      we_afi_result_free (sas_handle->afi_handle, &afi_result);
      op->status = res;
      op->state = WE_SAS_STATE_FINISHED;
      return WE_PACKAGE_OPERATION_COMPLETE;
    }

    we_afi_result_free (sas_handle->afi_handle, &afi_result);
    op->state = WE_SAS_STATE_FINISHED;
    op->status = WE_SAS_RETURN_OK;
    return WE_PACKAGE_OPERATION_COMPLETE;
  }

  return WE_PACKAGE_SIGNAL_NOT_HANDLED;
}

/*
 * Set private data connected to a specific save as operation. The private 
 * data might be used by the module to indicate which sub module that started 
 * the save as operation. This function may be called after a
 * we_sas_create_xxx function has been called.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_sas_set_private_data (we_pck_handle_t *handle, WE_INT32 wid, void *p)
{
  we_sas_handle_t *sas_handle = (we_sas_handle_t*)handle;
  we_sas_op_t     *op = find_op_by_id (sas_handle, wid);

  if (op == NULL) {
    return FALSE;
  }
  op->priv_data = p;

  return TRUE;
}

/*
 * Get private data connected to a specific save as operation. The private 
 * data might be used by the module to indicate which sub module that has
 * started the operation. This function may be called after 
 * we_sas_handle_signal returned WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns WE_PACKAGE_ERROR.
 */
void *
we_sas_get_private_data (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_sas_handle_t *sas_handle = (we_sas_handle_t*)handle;
  we_sas_op_t     *op = find_op_by_id (sas_handle, wid);

  if (op == NULL) {
    return NULL;
  }

  return op->priv_data;
}


/*
 * Terminates the save as package, release all resources allocated by
 * this package. If there is any open dialogs, these dialog are
 * deleted. Any private data must be released by the module itself.
 * Returns TRUE  if success, otherwise FALSE.
 */
int
we_sas_terminate (we_pck_handle_t* handle)
{
  we_sas_handle_t* sas_handle = (we_sas_handle_t*)handle;

  while (sas_handle->op) {
    free_sas_op(sas_handle, sas_handle->op);
  }
  we_afi_terminate(sas_handle->afi_handle);
  we_dlg_terminate(sas_handle->dlg_handle);
  WE_MEM_FREE(sas_handle->modid, sas_handle);

  return TRUE;
}

/**********************************************************************
 * Delete 
 **********************************************************************/

/*
 * Deletes a created save as operation. Any private data is not released by 
 * this function. The module call this function when the module of any
 * reason must delete the operation before the finished state has been
 * reached.
 * Returns TRUE  if success, otherwise FALSE.
 */
int
we_sas_delete (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_sas_handle_t *sas_handle = (we_sas_handle_t*)handle;
  we_sas_op_t     *op = find_op_by_id (sas_handle, wid);

  if (op == NULL) {
    return FALSE;
  }

  we_dlg_delete (sas_handle->dlg_handle, op->progess_dlg_id);
  we_dlg_delete (sas_handle->dlg_handle, op->error_dlg_id);
  free_sas_op(sas_handle, op);
  return TRUE;
}

/*
 * Get the result of a completed operation. The caller must
 * call this function when we_dlg_handle_signal has returned
 * WE_PACKAGE_OPERATION_COMPLETE for this specific dialog. This
 * function also releases all allocated resources connected to the
 * specific dialog , but only if the function is called when the
 * operation is finished. Any private data associated with this 
 * operation must be freed by the using module before this function
 * is called.
 * Returns TRUE if success, otherwise FALSE.
 */

int
we_sas_get_result (we_pck_handle_t  *handle,
                        WE_INT32          wid,
                        we_pck_result_t  *result)
{
  int res = 0;
  we_sas_result_save_as_t *sas_res;
  res = we_sas_get_result_ext(handle, wid, result);
  
  if (res == WE_PACKAGE_ERROR) {
    return FALSE;
  }

  sas_res = result->_u.data;
  switch (sas_res->result) {
  case WE_SAS_ERROR_ACCESS: 
  case WE_SAS_ERROR_DELAYED:
  case WE_SAS_ERROR_PATH:
  case WE_SAS_ERROR_INVALID:
  case WE_SAS_ERROR_SIZE:
  case WE_SAS_ERROR_FULL:
  case WE_SAS_ERROR_EOF:
  case WE_SAS_ERROR_EXIST:
  case WE_SAS_ERROR_INTERNAL:
    sas_res->result = WE_SAS_RETURN_ERROR;
    break;
  default:
    break;
  }

  if (res == WE_PACKAGE_COMPLETED ||
      res == WE_PACKAGE_BUSY) {
    return TRUE;
  }
  return FALSE;
}

/*
 * This function works as we_sas_get_result but has more 
 * detailed information about errors. The following error codes
 * has been added:
 * WE_SAS_ERROR_ACCESS
 * WE_SAS_ERROR_DELAYED
 * WE_SAS_ERROR_PATH
 * WE_SAS_ERROR_INVALID
 * WE_SAS_ERROR_SIZE
 * WE_SAS_ERROR_FULL
 * WE_SAS_ERROR_EOF
 * WE_SAS_ERROR_EXIST
 * WE_SAS_ERROR_INTERNAL
 */

int
we_sas_get_result_ext (we_pck_handle_t  *handle,
                        WE_INT32          wid,
                        we_pck_result_t  *result)
{
  we_sas_handle_t         *sas_handle = (we_sas_handle_t*)handle;
  we_sas_op_t             *op;
  we_sas_result_save_as_t *res;

  if (handle == NULL) {
    return WE_PACKAGE_ERROR;
  }

  op = find_op_by_id (sas_handle, wid);
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  result->type = WE_SAS_TYPE;
  res = result->_u.data = WE_MEM_ALLOCTYPE (sas_handle->modid,
                                             we_sas_result_save_as_t);
  res->file_name = NULL;
  res->result = op->status;
  switch (op->status) {

  case WE_SAS_RETURN_NOT_AVAILABLE:
    return WE_PACKAGE_BUSY;

  case WE_SAS_RETURN_OK:
    res->file_name = we_cmmn_strdup (sas_handle->modid, op->file_name);
    break;

  default:
    /*if data resource is pipe delete it.*/
    if (op->data_type == WE_PCK_DATA_TYPE_PIPE) {
      WE_PIPE_DELETE (op->pathname);
    }
  }
  free_sas_op (sas_handle, op);

  return WE_PACKAGE_COMPLETED;
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
we_sas_result_free (we_pck_handle_t *handle, we_pck_result_t *result)
{
  we_sas_handle_t         *sas_handle = (we_sas_handle_t *)handle;
  we_sas_result_save_as_t *save;

  if ((handle == NULL) || (result == NULL)) {
    return FALSE;
  }
  save = ((we_sas_result_save_as_t *)result->_u.data);
  if (save == NULL) {
    return FALSE;
  }
  if (result->type == WE_SAS_TYPE) {
    WE_MEM_FREE (sas_handle->modid, save->file_name);
    WE_MEM_FREE (sas_handle->modid, save);
  }

  return TRUE;
}


/*
 * Create a save as operation.
 *
 * Save as parameters:
 *  file_name             Suggested user friendly name of the file.
 *  mime_type             The mime type of file to save.
 *  data_type             Tells if the data to save comes from a pipe, file or
 *                        buffer. value must be one of the following constants:
 *                          WE_PCK_DATA_TYPE_FILE,
 *                          WE_PCK_DATA_TYPE_PIPE,
 *                          WE_PCK_DATA_TYPE_BUFFER
 *  data                  The data that shall be written to file. This 
 *                        parameter is only used when data comes from a 
 *                        buffer.
 *  data_len              The size of the data. This parameter is only used 
 *                        when data comes from a buffer.
 *  pathname              The name of the pipe or file. This parameter is only
 *                        used when data comes from a pipe or a file.
 *  screen                The screen that should be used for the dialog.
 *  file_attrs            the file attributes that should be stored with the
 *                        specified file.
 *  bit_flag              WE_SAS_ALLOW_OVERWRITE   -  if bit is set, the package will prompt
 *                          the user about the replace and possibly replace the existing file.
 *                        WE_SAS_DO_NOT_SHOW_DIALOG   -  if bit is set, the package will not 
 *                          allow the user to select position or name of the file to save, but
 *                          the file will be saved to a temporary default position with a default
 *                          name.
 *                        FRW_SAS_NO_EXTERNAL_MEMORY - If this bit is set no folders on
 *                          external memory will be possible to select as save destination.
 * 
 * Returns positive number denoting an wid if ok and WE_PACKAGE_ERROR otherwise.
 */
WE_INT32
we_sas_create_save_as (const we_sas_save_as_t *save_as_data)
{
  we_sas_handle_t *sas_handle = (we_sas_handle_t*)save_as_data->handle;
  we_sas_op_t     *op = get_new_op (sas_handle);

  op->data_type = save_as_data->data_type;
  op->data = save_as_data->data;
  op->data_len = save_as_data->data_len;
  op->screen = save_as_data->screen;
  if ((save_as_data->bit_flag & WE_SAS_ALLOW_OVERWRITE) > 0) {
    op->bit_flag |= WE_SAS_ALLOW_OVERWRITE;
  }

  if ((save_as_data->bit_flag & FRW_SAS_NO_EXTERNAL_MEMORY) > 0) {
    op->bit_flag |= FRW_SAS_NO_EXTERNAL_MEMORY;
  }

  if ((save_as_data->bit_flag & WE_SAS_DO_NOT_SHOW_DIALOG) == WE_SAS_DO_NOT_SHOW_DIALOG) {
    op->do_not_show_dialog = TRUE;
  }

#ifdef WE_SAS_CFG_DO_NOT_SHOW_DIALOG
    op->do_not_show_dialog = TRUE;
#endif

  if (save_as_data->pathname != NULL) {
    op->pathname = we_cmmn_strdup (sas_handle->modid, save_as_data->pathname);
  }
  if (save_as_data->file_attrs != NULL) {
    op->file_attrs = we_pck_attr_dupl (sas_handle->modid, save_as_data->file_attrs);
  }

  if (!op->do_not_show_dialog) {
    we_dlg_save_as_t save_as_dlg_data;

    save_as_dlg_data.dlg_data.handle = sas_handle->dlg_handle;
    save_as_dlg_data.dlg_data.screen = save_as_data->screen;
    save_as_dlg_data.dlg_data.title = save_as_data->title;

    save_as_dlg_data.file_name = save_as_data->file_name;
    save_as_dlg_data.save_here_str = save_as_data->save_here_str;
    save_as_dlg_data.mime_type = save_as_data->mime_type;
    save_as_dlg_data.bit_flag = op->bit_flag;
#ifdef WE_CFG_DRM_DIRECTORY_STORAGE
    op->mime_type = we_cmmn_strdup(sas_handle->modid, save_as_data->mime_type); 
    we_sas_convert_mime_to_drm_mime (sas_handle, op);
    save_as_dlg_data.mime_type = op->mime_type;
#endif
    op->dlg_id = we_dlg_create_save_as (&save_as_dlg_data);
    op->state = WE_SAS_STATE_DLG_WAIT;
  }
  else {
    we_pck_result_t save_result;

    op->file_name = we_cmmn_strdup (sas_handle->modid,
                                     WE_SAS_CFG_DEFAULT_FILENAME);
    op->afi_id = we_afi_save (sas_handle->afi_handle, op->file_name,
                               op->data_type, op->data_len,
                               op->data, op->pathname, op->file_attrs);
    we_afi_get_result (sas_handle->afi_handle, op->afi_id, &save_result);

    if (save_result.type == WE_AFI_SAVE) {
      WE_INT32 res = (WE_INT32)save_result._u.data;

      if (res == WE_AFI_ERROR_DELAYED) {
        op->state = WE_SAS_STATE_AFI_SAVE_WAIT;
      }
      else {
        op->status = res;
        op->state = WE_SAS_STATE_FINISHED;
      }
    }
    we_afi_result_free(sas_handle->afi_handle, &save_result);
  }

  return op->wid;
}

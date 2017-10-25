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


#include "We_Core.h"
#include "We_Log.h"
#include "We_Wid.h"
#include "We_Lib.h"
#include "We_Dlg.h"
#include "We_Afi.h"
#include "We_Mem.h"
#include "We_Pck.h"
#include "We_File.h"
#include "We_Dprc.h"

/*************************************************
 * Defines
 *************************************************/
#define WE_DLG_STATE_OPEN_NAVIGATE                   1
#define WE_DLG_STATE_OPEN_NAVIGATE_DELAYED           2
#define WE_DLG_STATE_OPEN_FINISHED                   3
#define WE_DLG_STATE_SAVE_AS_NAVIGATE                4
#define WE_DLG_STATE_SAVE_AS_NAVIGATE_DELAYED        5
#define WE_DLG_STATE_SAVE_AS_EDIT_NAME               6
#define WE_DLG_STATE_SAVE_AS_FINISHED                7
#define WE_DLG_STATE_SAVE_AS_FILE_EXISTS_DELAYED     8
#define WE_DLG_STATE_SAVE_AS_EXISTS_DIALOG           9
#define WE_DLG_STATE_SAVE_AS_EXISTS_DELETE_DELAYED  10
#define WE_DLG_STATE_SAVE_AS_MSG_DLG                11
#define WE_DLG_STATE_SAVE_AS_ICONS_DELAY            12
#define WE_DLG_STATE_OPEN_ICONS_DELAY               13
#define WE_DLG_STATE_NEW_DIR_NAME_EDIT              14
#define WE_DLG_STATE_NEW_DIR_AFI_DELAYED            15
#define WE_DLG_STATE_ERROR_POPUP                    16
#define WE_DLG_STATE_SAVE_AS_CANNOT_SAVE            17
#define WE_DLG_STATE_MESSAGE_DIALOG                 18
#define WE_DLG_STATE_CONFIRM_DIALOG                 19
#define WE_DLG_STATE_PROMPT_DIALOG                  20
#define WE_DLG_STATE_PROGRESS_DIALOG                21
#define WE_DLG_STATE_LOGIN_DIALOG                   22
#define WE_DLG_STATE_SAVE_AS_EDIT_NAME_ONLY         23

#define WE_DLG_PREVIEW_STATE_IDLE                    1
#define WE_DLG_PREVIEW_STATE_SLEEP                   2
#define WE_DLG_PREVIEW_STATE_BUSY                    3
/* preview delay in thousandth of seconds */
#define WE_DLG_PREVIEW_TIMER                         2000

#define WE_DLG_CONTENT_TYPE_IMAGE                    "image/jpeg,image/gif,\
                                                      image/png,image/vnd.wap.wbmp\
                                                      ,image/bmp"
/* max size in bytes of content to preview */
#define WE_DLG_CONTENT_MAX_SIZE                      30000 

typedef struct we_dlg_dialog_st {
  struct we_dlg_dialog_st *next;
  we_dprc_t               *handle;
} we_dlg_dialog_t;

typedef struct we_dlg_op_progress_st {
  we_dlg_dialog_t        *dialog;
  WE_UINT32               timer_id;
  we_dprc_progress_bar_t *data;
} we_dlg_op_progress_t;

typedef struct we_dlg_op_file_st {
  char*                          mime_types;
  char*                          file_name;
  char*                          default_name;
  char*                          entered_name;
  int                            current_focus;
  WE_INT32                      afi_id;
  WE_INT32                      bit_flag;
  int                            permission;
  we_afi_result_dir_entries_t  *dir_entries;
  int                            preview_state;
  int                            current_timer_id;
  WeImageHandle                 preview_image_handle;
  WeStringHandle                save_here_str;
  int                            nbr_items_in_file_dlg;
  int                            is_open_window;
} we_dlg_op_file_t;

typedef struct we_dlg_op_st {
  struct we_dlg_op_st     *next;
  WE_UINT32                wid;        /* operation identifier */
  WE_INT8                  type;
  int                       state;
  int                       prev_state;
  void                     *priv_data;
  long                      status;    /* status of operation */
  we_dlg_dialog_t         *dialog;
  we_dlg_result_callback   result_callback;
  WeStringHandle           title;
  WeScreenHandle           screen;
  we_dlg_op_progress_t    *progress_data;
  we_dlg_op_file_t        *file_data;
} we_dlg_op_t;


typedef struct we_dlg_handle_st {
  struct we_dlg_handle_st  *next;
  struct we_dlg_handle_st  *prev;
  WE_UINT8                  modid;
  we_dlg_op_t              *op;
  we_pck_handle_t          *afi_handle;
  WeImageHandle             folder_image;
  WeImageHandle             file_image;
  we_icon_list_t           *icons;
  WE_UINT16                 n_items;
  we_dlg_pkg_type_t         dlg_pkg_type;
} we_dlg_handle_t;

typedef struct we_dlg_index_list_st{
  struct    we_dlg_index_list_st  *next;
  int       index;
} we_dlg_index_list_t;


/*************************************************
 * Configuration
 *************************************************/

#define WE_DLG_MENU_SIZE_WIDTH   0.9
#define WE_DLG_MENU_SIZE_HEIGHT  0.9
#define WE_DLG_TEXT_POSITION     {10,10}
#define WE_DLG_TEXT_HEIGHT       20

/**********************************************************************
 * Function prototypes
 **********************************************************************/

static void
create_error_msg_dialog (we_dlg_handle_t *handle, we_dlg_op_t *op, 
                         WeStringHandle title, WeStringHandle str);

static void
create_error_popup_window(we_dlg_handle_t *handle, we_dlg_op_t *op, 
                          WE_INT32 text_id, int prev_state);
static void
create_name_edit_window(we_dlg_handle_t *handle, we_dlg_op_t *op);

/**********************************************************************
 * Local functions
 **********************************************************************/
static we_dlg_op_t *
find_op_by_id (we_dlg_handle_t *handle, WE_UINT32 wid)
{
  we_dlg_op_t *op = handle->op;

  while (op) {
    if (op->wid == wid) {
      return op;
    }
    op = op->next;
  }
  return NULL;
}

static char*
get_mime_from_attr_list(we_pck_attr_list_t *attrlist)
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

static int
file_is_write_protected (we_pck_attr_list_t *attrlist)
{
  we_pck_attr_list_t *attribute = attrlist;

  while (attribute) {
    if ( attribute->attribute == WE_PCK_ATTRIBUTE_FLAGS &&
         (attribute->_u.i & WE_PCK_FLAG_PROTECTED || 
          attribute->_u.i & WE_PCK_FLAG_USER_PROTECTED) ) {
      return TRUE;
    }
    attribute = attribute->next;
  }
  return FALSE;
}
/* Returns TRUE if Forward Locked, else FALSE */
static int
is_forward_locked (we_pck_attr_list_t *attrlist)
{
  we_pck_attr_list_t *attribute = attrlist;

  while (attribute) {
    if (attribute->attribute == WE_PCK_ATTRIBUTE_FLAGS &&
        attribute->_u.i & WE_PCK_FLAG_DRM_FORWARD_LOCK) {
      return TRUE;
    }
    attribute = attribute->next;
  }
  return FALSE;
}

static WeImageHandle
get_icon_handle (we_dlg_handle_t *handle, const char* mime_type)
{   
  int           i = 0;
  char         *end = NULL;
  char         *s = NULL;
  int           found = -1;
  WE_UINT32    hash1 = 0;
  WE_UINT32    hash2 = 0;
  WeCreateData image_data;

  if (mime_type == NULL) {
    return 0;
  }

  hash1 = we_cmmn_strhash (mime_type, strlen (mime_type));
  for (i = 0; i < handle->n_items; i++) {
    
    if (hash1 == handle->icons[i].mime_hash &&
        strcmp(mime_type, handle->icons[i].s_mime) == 0) {

      if (handle->icons[i].resource_type == WE_ICON_RESOURCE_TYPE_PREDEFINED) {
        return WE_WIDGET_IMAGE_GET_PREDEFINED (handle->icons[i].resource_int_id, NULL);
      }
      image_data.resource = handle->icons[i].resource_str_id;
      return WE_WIDGET_IMAGE_CREATE (handle->modid, &image_data, mime_type, 
                                      WeResourceFile,0);
    }
  }
  /*No specific type was found. Look for general types*/

  end = strchr(mime_type,'/');

  if (end == NULL) {
    return 0;
  }
  hash1 = we_cmmn_strhash ("*/*", strlen ("*/*"));
  s = WE_MEM_ALLOC (handle->modid, end - mime_type + 3);
  strncpy(s,mime_type, end - mime_type);
  s[end - mime_type] = '\0';
  strcat (s,"/*");
  hash2 = we_cmmn_strhash (s, strlen (s));

  for (i = 0; i < handle->n_items; i++) {
    if (hash1 == handle->icons[i].mime_hash) {
      if (strcmp("*/*", handle->icons[i].s_mime) == 0) {
        found = i;
        continue;
      }
    }
    if (hash2 == handle->icons[i].mime_hash) {
      if (strcmp(s, handle->icons[i].s_mime) == 0) {
        found = i;
        break;
      }
    }
  }

  WE_MEM_FREE (handle->modid, s);
  /*Check if found*/
  if (found < 0) {
    return 0;
  }
  if (handle->icons[found].resource_type == WE_ICON_RESOURCE_TYPE_PREDEFINED) {
    return WE_WIDGET_IMAGE_GET_PREDEFINED (handle->icons[found].resource_int_id, NULL);
  }
  else {
    image_data.resource = handle->icons[found].resource_str_id;
    return WE_WIDGET_IMAGE_CREATE (handle->modid, &image_data, 
                                    mime_type,WeResourceFile,0);
  }
  /* return 0; unreachable code due to return in both if and else above */
}

static void
free_dir_entries (WE_UINT8 modid, we_afi_result_dir_entries_t *result)
{
  int i = 0;

  for (i = 0; i < result->num_entries ; i++) {
    if (result->entry_list[i].attr_list) {
      we_pck_attr_free (modid, result->entry_list[i].attr_list);
    }
    if (result->entry_list[i].name) {
      WE_MEM_FREE(modid, result->entry_list[i].name);
    }
  }
  WE_MEM_FREE(modid, result);
}

static we_dlg_dialog_t *
get_new_dialog (we_dlg_handle_t *handle, we_dlg_op_t *op)
{
  we_dlg_dialog_t *dialog = WE_MEM_ALLOCTYPE (handle->modid, we_dlg_dialog_t);

  dialog->handle = NULL;
  /* insert new dialog first in list */
  dialog->next = op->dialog;
  op->dialog = dialog;
  return dialog;
}

static void
free_dlg_dialog(we_dlg_handle_t *handle, we_dlg_op_t *op, we_dlg_dialog_t *dialog)
{
  we_dlg_dialog_t *temp_dialog = op->dialog;
  we_dlg_dialog_t *prev_dialog = NULL;

  /*Remove dialog*/

  while (temp_dialog != dialog) {
    prev_dialog = temp_dialog;
    temp_dialog = temp_dialog->next;
  }
  if (prev_dialog) {
    prev_dialog->next = dialog->next;
  }
  else {
    op->dialog = dialog->next;
  }
  /*Free dialog*/  
  we_dprc_delete (dialog->handle);
  WE_MEM_FREE(handle->modid, dialog);
}

static we_dlg_op_t *
get_new_op (we_dlg_handle_t *handle, WE_INT8 type)
{
  static int we_dlg_uniq_id = 1;

  we_dlg_op_t *op = WE_MEM_ALLOCTYPE (handle->modid, we_dlg_op_t);
  
  op->wid = we_dlg_uniq_id++;
  op->type = type;
  op->state = 0;
  op->dialog  = NULL;
  op->progress_data = NULL;
  op->result_callback = NULL;
  op->title = 0;
  op->prev_state = 0;
  op->file_data = NULL;
  op->screen = 0;
  op->priv_data = NULL;
  op->status = WE_DLG_RETURN_NOT_AVAILABLE;

  /* insert new operation first in list */
  op->next = handle->op;
  handle->op = op;

  return op;
}

static void
free_dlg_op(we_dlg_handle_t *handle, we_dlg_op_t *op)
{
  we_dlg_op_t *temp_op = handle->op;
  we_dlg_op_t *prev_op = NULL;

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

  if (op->file_data) {
    if (op->file_data->mime_types) {
      WE_MEM_FREE(handle->modid, op->file_data->mime_types);
    }
    if (op->file_data->file_name) {
      WE_MEM_FREE(handle->modid, op->file_data->file_name);
    }
    if (op->file_data->default_name) {
      WE_MEM_FREE(handle->modid, op->file_data->default_name);
    }
    if (op->file_data->entered_name) {
      WE_MEM_FREE(handle->modid, op->file_data->entered_name);
    }
    if (op->file_data->dir_entries)  {
      free_dir_entries(handle->modid, op->file_data->dir_entries);
    }
    WE_MEM_FREE (handle->modid,  op->file_data);
    op->file_data = NULL;
  }

  if(op->progress_data) {
    if (op->progress_data->data) {
      WE_MEM_FREE (handle->modid, op->progress_data->data);
    }
    WE_MEM_FREE (handle->modid, op->progress_data);
    op->progress_data = NULL;
  }

  while (op->dialog) {
    free_dlg_dialog(handle, op, op->dialog);
  }

  WE_MEM_FREE(handle->modid, op);
  op = NULL;
}

static WeStringHandle
create_we_string (WE_UINT8 modid, const char* string)
{
  return WE_WIDGET_STRING_CREATE (modid, string, WeUtf8, strlen(string)+1, 0);
}

static void
we_dlg_start_preview_timer(we_dlg_handle_t *handle, we_dlg_op_t *op)
{
  static int we_dlg_uniq_timer_id = 5000; 

  op->file_data->current_timer_id = we_dlg_uniq_timer_id;
  WE_TIMER_SET(handle->modid, we_dlg_uniq_timer_id++, WE_DLG_PREVIEW_TIMER);
  op->file_data->preview_state = WE_DLG_PREVIEW_STATE_SLEEP;
}



/* Checks if path allows directoris to be created. Returns TRUE or FALSE. */
static int
allow_create_new_dir(WE_UINT8 modid, const char *path)
{
  we_pck_root_folders_t  *root_result = NULL;
  int i = 0;
  const char *urf = path;
  int max_depth;
  int depth = 1;
  
  if ((path == NULL) || (strlen (path) < 5) || (path[0] != '/')) {
    return FALSE;
  }

  max_depth = we_pck_get_urf_max_depth (urf);
  if(max_depth <= 1) {
    return FALSE;
  }

  we_pck_get_all_root_folders(modid, &root_result);

  while (i < WE_NBR_OF_USER_ROOT_FOLDERS && 
         strstr (urf, root_result->rf[i].pathname) != urf) {
    i++;
  }

  if (i >= WE_NBR_OF_USER_ROOT_FOLDERS) {
    WE_MEM_FREE (modid, root_result);
    return FALSE;
  }

  urf += strlen(root_result->rf[i].pathname);
  WE_MEM_FREE (modid, root_result);
  i = 0;
  while (urf[i] != '\0') {
    if(urf[i] == '/') {
      depth++;
    }
    i++;
  }
  return depth < max_depth;
}

/* sorting alphabetic with folders first */
static int
is_after_alphabetic(const char *name1, int isdir1,
                    const char *name2, int isdir2)
{
  if ((!isdir1) && isdir2) {
    return TRUE;
  }

  if (isdir1 && (!isdir2)) {
    return FALSE;
  }

  if (we_cmmn_strcmp_nc(name1, name2) >= 0) {
    return TRUE;
  }

  return FALSE;
}


static we_dlg_index_list_t *
we_dlg_get_index_list (we_dlg_handle_t *handle, 
                        we_dlg_op_t  *op,
                        we_afi_result_dir_entries_t *dir_entries)
{
  int                   i = 0;
  char                 *mime_type = NULL;
  we_afi_file_t       *entry_list;
  we_dlg_index_list_t *insert_entry = NULL;
  we_dlg_index_list_t *insert_before_entry = NULL;
  we_dlg_index_list_t *first_entry = NULL;
  we_dlg_index_list_t *insert_after_entry = NULL;

  entry_list = dir_entries->entry_list;
  for (i = 0; i < dir_entries->num_entries ; i++) {

    mime_type = get_mime_from_attr_list(entry_list[i].attr_list);

    /*if files mimetype is in mimelist insert entry in indexlist*/
    if ((mime_type == NULL ||
        !op->file_data->is_open_window ||
        we_pck_mime_type_in_list (mime_type, op->file_data->mime_types)) ||
       dir_entries->entry_list[i].is_dir == TRUE) {

      insert_entry = WE_MEM_ALLOCTYPE (handle->modid, we_dlg_index_list_t);
      insert_entry->index = i;
      insert_before_entry = first_entry;
      insert_after_entry = NULL;

      while ((insert_before_entry != NULL) && 
             is_after_alphabetic(entry_list[i].name,
                                 entry_list[i].is_dir,
                                 entry_list[insert_before_entry->index].name,
                                 entry_list[insert_before_entry->index].is_dir)) {

        insert_after_entry = insert_before_entry;
        insert_before_entry = insert_before_entry->next;
      }

      if (insert_after_entry) {
        insert_after_entry->next = insert_entry;
      }
      else {
        first_entry = insert_entry;
      }
      insert_entry->next = insert_before_entry;
    }
  }
  return first_entry;  
  
}


static void
delete_index_list(we_dlg_handle_t *handle, we_dlg_index_list_t *index_list)
{
  we_dlg_index_list_t *entry;

  while (index_list ){
    entry = index_list->next;
    WE_MEM_FREE(handle->modid, index_list);
    index_list = entry;
  }
}
/*
static int
get_index_from_index_list_number_restrict(we_dlg_index_list_t *index_list, 
                                           int number, 
                                           we_afi_result_dir_entries_t *dir_entries, 
                                           char *mime_list,
                                           int permission)
{
  int i=0;
  we_dlg_index_list_t *entry = index_list;
  char *mime_type = NULL;

  if (!entry) {
    return -1;
  }

  mime_type = get_mime_from_attr_list(dir_entries->entry_list[entry->index].attr_list);
  while ((mime_type && !(mime_type_in_list(mime_type, mime_list))) ||
        ( (permission == WE_DLG_PERMISSION_FORWARD) &&
          (is_forward_locked(dir_entries->entry_list[entry->index].attr_list))
        )) {
    
    entry = entry->next;
    if (!entry) {
      return -1;
    }
    mime_type = get_mime_from_attr_list(dir_entries->entry_list[entry->index].attr_list);
  }

  for (i = 0 ; i < number ;i ++) {
    char *mime_type = NULL;

    if (!entry) {
      return -1;
    }
    entry = entry->next;

    mime_type = get_mime_from_attr_list(dir_entries->entry_list[entry->index].attr_list);
    while (mime_type && !(mime_type_in_list(mime_type, mime_list)) ||
          ( (permission == WE_DLG_PERMISSION_FORWARD) && 
            (is_forward_locked(dir_entries->entry_list[entry->index].attr_list)) 
          )) {
      entry = entry->next;
      if (!entry) {
        return -1;
      }
      mime_type = get_mime_from_attr_list(dir_entries->entry_list[entry->index].attr_list);
    }
  }

  return entry->index;
}
*/

static int
get_index_from_index_list_number(we_dlg_index_list_t *index_list, int number)
{
  int i=0;
  we_dlg_index_list_t *entry = index_list;

  if (entry == NULL) {
    return -1;
  }
  
  for (i = 0 ; i < number ;i ++) {
    if (entry == NULL) {
      return -1;
    }
    entry = entry->next;
  }

  return entry->index;
}

static WE_INT32
get_size_from_attr_list(we_pck_attr_list_t *attrlist)
{ 
  we_pck_attr_list_t *attribute;

  attribute = attrlist;
  while (attribute) {
    if (attribute->attribute == WE_PCK_ATTRIBUTE_SIZE) {
      return attribute->_u.i;
    }
    attribute = attribute->next;
  }
  return 0;
}

static char*
size_to_string (we_dlg_handle_t *handle, WE_INT32 size , char *notation, 
                char* bString, char* KbString, char* MbString)
{ 
  long  sizeMBytes = 0;
  long  sizekBytes = 0;
  long  sizeBytes = 0;
  char  sizeBytesString[5];
  char  sizekBytesString[5]; 
  char  sizeMBytesString[5];
  char *return_string = NULL;
  
  if (size >= (1048567)) {
    sizeMBytes = (long) size / 1048567;
    sizekBytes = size % 1048567;
    sizekBytes = (long) ( ( (double)sizekBytes ) / 104856.7);
    sprintf (sizeMBytesString, "%ld", sizeMBytes);
    sprintf (sizekBytesString, "%ld", sizekBytes);
    return_string = WE_MEM_ALLOC(handle->modid, strlen(sizeMBytesString) + 
                                  strlen(sizekBytesString) + 
                                  strlen(notation) + strlen(MbString) + 1);
    strcpy(return_string, sizeMBytesString);
    strcat(return_string, notation);
    strcat(return_string, sizekBytesString);
    strcat(return_string, MbString);
    return return_string;
  }
  else if (size >= 1024) {
    sizekBytes = (long) size / 1024;
    sizeBytes = size % 1024;
    sizeBytes = (long) ( ( (double)sizeBytes ) / 102.4);
    sprintf (sizekBytesString, "%ld", sizekBytes);
    sprintf (sizeBytesString, "%ld", sizeBytes);
    return_string = WE_MEM_ALLOC(handle->modid, strlen(sizekBytesString) + 
                                  strlen(sizeBytesString) + 
                                  strlen(notation) + strlen(KbString) + 1);
    strcpy(return_string, sizekBytesString);
    strcat(return_string, notation);
    strcat(return_string, sizeBytesString);
    strcat(return_string, KbString);
    return return_string;
  }
  else {
    sizeBytes = size;
    sprintf (sizeBytesString, "%ld", sizeBytes);
    return_string = WE_MEM_ALLOC(handle->modid, strlen(sizeBytesString) + 
                                  strlen(bString) + 1);
    strcpy(return_string, sizeBytesString);
    strcat(return_string, bString);
    return return_string;
  }
}

static char *
get_string_from_file_size(we_dlg_handle_t *handle, WE_INT32 file_size)
{
  WeStringHandle string = 0;
  char            *text_size2 = NULL;
  char            *text_size3 = NULL;
  char            *text_size4 = NULL;
  char            *text_size5 = NULL;
  char            *text_size = NULL;
  int             length = 0;

  string = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_PROPERTIES_SIZE2);
  length = WE_WIDGET_STRING_GET_LENGTH (string, 1, WeUtf8)+1;
  text_size2 = WE_MEM_ALLOC (handle->modid, length);
  WE_WIDGET_STRING_GET_DATA (string, text_size2, WeUtf8);

  string = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_PROPERTIES_SIZE3);
  length = WE_WIDGET_STRING_GET_LENGTH (string, 1, WeUtf8)+1;
  text_size3 = WE_MEM_ALLOC(handle->modid, length);
  WE_WIDGET_STRING_GET_DATA(string, text_size3, WeUtf8);

  string =  WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_PROPERTIES_SIZE4);
  length =  WE_WIDGET_STRING_GET_LENGTH(string ,1, WeUtf8)+1;
  text_size4 = WE_MEM_ALLOC(handle->modid, length);
  WE_WIDGET_STRING_GET_DATA(string, text_size4, WeUtf8);
  
  string = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_PROPERTIES_SIZE5);
  length = WE_WIDGET_STRING_GET_LENGTH(string, 1, WeUtf8)+1;
  text_size5 = WE_MEM_ALLOC(handle->modid, length);
  WE_WIDGET_STRING_GET_DATA(string, text_size5, WeUtf8);
  
  text_size = size_to_string (handle, file_size, text_size2, text_size5, 
                              text_size3, text_size4);
  
  WE_MEM_FREE(handle->modid, text_size2);
  WE_MEM_FREE(handle->modid, text_size3);
  WE_MEM_FREE(handle->modid, text_size4);
  WE_MEM_FREE(handle->modid, text_size5);
  
  return text_size;
}

static int
add_entries_to_file_dlg (we_dlg_handle_t *handle, we_dlg_op_t *op,
                         we_afi_result_dir_entries_t *dir_entries,
                         int start_pos)
{
  we_dlg_index_list_t  *index_list_first_entry = NULL;
  we_dlg_index_list_t  *index_list = NULL;
  int                   i;
  int                   position = start_pos;

  index_list_first_entry = we_dlg_get_index_list(handle, op, dir_entries);
  index_list = index_list_first_entry;

  while (index_list) {
    we_dprc_menu_item_t item;
    i = index_list->index;
    if (dir_entries->entry_list[i].is_dir == TRUE) {
      WeStringHandle  string = 0;
      char            *dir_name = NULL;

      dir_name = we_pck_get_local_uf (handle->modid, 
                                       op->file_data->file_name, 
                                       dir_entries->entry_list[i].name);

      if (dir_name == NULL) {
        dir_name = dir_entries->entry_list[i].name;
      }
      string = create_we_string (handle->modid, dir_name);
      item.string1 = string;
      item.string2 = 0;
      item.image1 = handle->folder_image;
      item.image2 = 0;
      item.tooltip_string = 0;
      we_dprc_add_item_to_file_dlg (op->dialog->handle, &item, position++);
      op->file_data->nbr_items_in_file_dlg++;
      WE_WIDGET_RELEASE (string);
      if (dir_name != dir_entries->entry_list[i].name) {
        WE_MEM_FREE(handle->modid, dir_name);       
      }
    }
    else {
      WeStringHandle   string_1 = 0;
      WeStringHandle   string_2 = 0;
      char              *text_size = NULL;
  
      text_size = get_string_from_file_size (handle, 
                                            get_size_from_attr_list
                                            (dir_entries->entry_list[i].attr_list));

      string_1 = create_we_string (handle->modid, dir_entries->entry_list[i].name);

      string_2 = create_we_string (handle->modid, text_size);

      item.string1 = string_1;
      item.string2 = string_2;
      item.image1 = get_icon_handle (handle, get_mime_from_attr_list 
                                    (dir_entries->entry_list[i].attr_list));
      item.image2 = 0;
      item.tooltip_string = 0;
      we_dprc_add_item_to_file_dlg (op->dialog->handle, &item, position++);
      op->file_data->nbr_items_in_file_dlg++;
      WE_MEM_FREE(handle->modid, text_size);
      WE_WIDGET_RELEASE (string_1);
      WE_WIDGET_RELEASE (string_2);
    }
    index_list = index_list->next;
  }  
  delete_index_list(handle, index_list_first_entry);
  return position;
}

static int
we_dlg_show_root_folders (we_dlg_handle_t *handle, we_dlg_op_t *op,
                           const char* mime_types, int open_window)
{
  we_pck_root_folders_t *root_folders = NULL;
  int                     i = 0;
  int                     displayed_items = 0;

  if (op->title == 0) {
    we_dprc_set_title (op->dialog->handle,
        WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_TEXT_ROOT_FOLDER));
  }

  we_pck_get_urf_from_mime (handle->modid, mime_types,  &root_folders);
  for (i = 0; (i < root_folders->n_rf) ; i++) {
    if (open_window || !(((op->file_data->bit_flag & FRW_DLG_NO_EXTERNAL_MEMORY) > 0) && 
       (root_folders->rf[i].external == 1))) {
      we_dprc_menu_item_t item;

      item.string1 = WE_WIDGET_STRING_GET_PREDEFINED (root_folders->rf[i].str_id);
      item.string2 = 0;
      item.image1 = handle->folder_image;
      item.image2 = 0;
      item.tooltip_string = 0;
      displayed_items++;
      we_dprc_add_item_to_file_dlg (op->dialog->handle, &item, i);
      op->file_data->nbr_items_in_file_dlg++;
   }
  }

  if (open_window) {
    if (root_folders->n_rf) {
      we_dprc_set_file_dlg_item_state (op->dialog->handle, 0, 
                                        WE_CHOICE_ELEMENT_SELECTED);
    }
    op->file_data->current_focus = 0;
	WE_MEM_FREE( handle->modid, root_folders);
    return WE_DLG_STATE_OPEN_NAVIGATE;
  }

  WE_MEM_FREE( handle->modid, root_folders);
  
  if (displayed_items > 0) {
      we_dprc_set_file_dlg_item_state (op->dialog->handle, 0, 
                                        WE_CHOICE_ELEMENT_SELECTED);
    return WE_DLG_STATE_SAVE_AS_NAVIGATE;
  }
  else {
    WeStringHandle title = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_SAVE_ERROR);
    WeStringHandle msg = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_SAVE_NOT_ALLOWED);
    create_error_msg_dialog (handle, op, title, msg);
    return WE_DLG_STATE_SAVE_AS_MSG_DLG;
  }
}

static void
we_dlg_set_path_as_title (we_dlg_handle_t *handle, we_dprc_t *dialog,
                           const char* path)
{
  WeStringHandle loc_path = 0;
  char* str_path = NULL;

  str_path = we_pck_get_localized_pathname(handle->modid, path);
  if (str_path) {
    if (str_path[strlen(str_path)-1] == '/') {
      str_path[strlen(str_path)-1] = '\0';
    }
    loc_path = create_we_string( handle->modid, str_path+1);
    we_dprc_set_title (dialog, loc_path);

    WE_WIDGET_RELEASE (loc_path);
    WE_MEM_FREE(handle->modid, str_path);
  }
}

static int
we_dlg_update_open_file_dlg (we_dlg_handle_t *handle, we_dlg_op_t *op,
                                 we_afi_result_dir_entries_t *result)
{
  we_dlg_index_list_t *index_list_first_entry = NULL;
  we_dlg_index_list_t *index_list = NULL;
  int                   position = 0;

  index_list_first_entry = we_dlg_get_index_list (handle, op, result);
  index_list = index_list_first_entry;
  /*got entries*/
  if (op->file_data->dir_entries) {
    free_dir_entries(handle->modid, op->file_data->dir_entries);
  }
  op->file_data->dir_entries = result; /*save them*/

  position = add_entries_to_file_dlg (handle, op, result, 0);

  op->file_data->current_focus = 0;
  if (position) {
    int selection = 0;
    char* mime = NULL;
    we_pck_attr_list_t *attr_list;

    we_dprc_set_file_dlg_item_state (op->dialog->handle, 0, 
                                      WE_CHOICE_ELEMENT_SELECTED);

    index_list = index_list_first_entry;
    selection = get_index_from_index_list_number (index_list, 0);
    attr_list = op->file_data->dir_entries->entry_list[selection].attr_list;
    mime = get_mime_from_attr_list(attr_list);
    if (we_pck_mime_type_in_list(mime, WE_DLG_CONTENT_TYPE_IMAGE) && 
        op->file_data->dir_entries->entry_list[selection].is_dir == FALSE &&
        get_size_from_attr_list(attr_list) < WE_DLG_CONTENT_MAX_SIZE) {

      we_dlg_start_preview_timer(handle, op);
    }
  }
  delete_index_list(handle, index_list_first_entry);
  return WE_DLG_STATE_OPEN_NAVIGATE; 

}

static int
we_dlg_update_save_file_dlg (we_dlg_handle_t *handle, we_dlg_op_t *op,
                              we_afi_result_dir_entries_t *dir_entries)
{
  we_dprc_menu_item_t item = {0, 0, 0, 0, 0};
  we_pck_root_folders_t *root_folders = NULL;

  /* got entries */
  if (op->file_data->dir_entries) {
    free_dir_entries(handle->modid, op->file_data->dir_entries);
  }
  op->file_data->dir_entries = dir_entries; /* save them */

  if (handle->op->file_data->save_here_str) {
    item.string1 = handle->op->file_data->save_here_str;
    we_dprc_add_item_to_file_dlg (op->dialog->handle, &item, 0);
    op->file_data->nbr_items_in_file_dlg++;
  }
  else {
    item.string1 = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_DLG_TEXT_SAVE_HERE);
    we_dprc_add_item_to_file_dlg (op->dialog->handle, &item, 0);
    op->file_data->nbr_items_in_file_dlg++;
  }

  add_entries_to_file_dlg (handle, op, dir_entries, 1);

  we_dprc_set_file_dlg_item_state (op->dialog->handle, 0, 
                                    WE_CHOICE_ELEMENT_SELECTED);

  we_pck_get_urf_from_mime (handle->modid, op->file_data->mime_types,  
                             &root_folders);
  
  if (root_folders->n_rf == 1 && 
    we_pck_get_urf_max_depth (root_folders->rf[0].pathname) == 1) {

    create_name_edit_window (handle, op);
    WE_MEM_FREE(handle->modid, root_folders);
    return WE_DLG_STATE_SAVE_AS_EDIT_NAME;
  }
  WE_MEM_FREE(handle->modid, root_folders);

  return WE_DLG_STATE_SAVE_AS_NAVIGATE;
}

static void
we_dlg_handle_afi_error (we_dlg_handle_t *handle, we_dlg_op_t *op,
                          we_pck_result_t *pck_res)
{
  
  WE_UNUSED_PARAMETER(pck_res);

  switch (op->state) {
    case WE_DLG_STATE_SAVE_AS_EXISTS_DIALOG:
      op->state = WE_DLG_STATE_SAVE_AS_EDIT_NAME; 
      create_error_popup_window (handle, op, WE_STR_ID_DLG_ERROR_REPLACING_FILE, op->state);
      break;
    case WE_DLG_STATE_NEW_DIR_NAME_EDIT:
    case WE_DLG_STATE_NEW_DIR_AFI_DELAYED:
      create_error_popup_window (handle, op, WE_STR_ID_DLG_ERROR_CREATE_DIR, 
                                 WE_DLG_STATE_NEW_DIR_NAME_EDIT);
      break;
    case WE_DLG_STATE_SAVE_AS_NAVIGATE:
      create_error_popup_window (handle, op, WE_STR_ID_DLG_COMMON_ERROR, 
                                 op->state);
      break;
    default:
      break;
  }
}

static int
we_dlg_handle_afi_result (we_dlg_handle_t *handle, we_dlg_op_t *op,
                           int pck_state, we_pck_result_t *pck_res,
                           int is_open_window)
{
  we_afi_result_dir_entries_t *result = NULL;
  if (pck_res == NULL) {
    if (is_open_window) {
      return WE_DLG_STATE_OPEN_NAVIGATE; 
    }
    return WE_DLG_STATE_SAVE_AS_NAVIGATE;     
  }
  switch (pck_state) {
    case WE_PACKAGE_COMPLETED:
      result = (we_afi_result_dir_entries_t*)pck_res->_u.data;

      if (result->result == WE_AFI_ERROR_DELAYED) {
        we_afi_result_free (handle->afi_handle, pck_res);
        if (is_open_window) {
          return WE_DLG_STATE_OPEN_NAVIGATE_DELAYED;
        }
        return WE_DLG_STATE_SAVE_AS_NAVIGATE_DELAYED;
      }
      else if (result->result == WE_AFI_OK) {
        if (is_open_window) {
          return we_dlg_update_open_file_dlg (handle, op, result);
        }
        return we_dlg_update_save_file_dlg (handle, op, result);
      }
      else {
        we_dlg_handle_afi_error (handle, op, pck_res);
        return WE_DLG_STATE_ERROR_POPUP;
      }
      break;
    case WE_PACKAGE_BUSY:
      we_afi_result_free (handle->afi_handle, pck_res);
      if (is_open_window) {
        return WE_DLG_STATE_OPEN_NAVIGATE_DELAYED;
      }
      return WE_DLG_STATE_SAVE_AS_NAVIGATE_DELAYED;
      break;
    default:
      we_afi_result_free (handle->afi_handle, pck_res);
      if (is_open_window) {
        return WE_DLG_STATE_OPEN_NAVIGATE;
      }
      return WE_DLG_STATE_SAVE_AS_NAVIGATE;
  }
}

static int
we_dlg_update_open_window (we_dlg_handle_t *handle, we_dlg_op_t *op, 
                             const char* mime_types, const char* path)
{

  we_dprc_clear_file_dlg (op->dialog->handle);
  op->file_data->nbr_items_in_file_dlg = 0;

  if (path == NULL) {
    return we_dlg_show_root_folders (handle, op, mime_types, TRUE);
  }
  else {
    int pck_state = 0;
    we_pck_result_t pck_res;

    if (handle->icons == NULL) {
      WE_GET_ICONS (handle->modid, op->wid, 0, NULL);
      return WE_DLG_STATE_OPEN_NAVIGATE_DELAYED;
    }

    if (op->title == 0) {
      we_dlg_set_path_as_title (handle, op->dialog->handle, path);
    }

    op->file_data->afi_id = we_afi_get_dir_entries (handle->afi_handle, path, 0);
    pck_state = we_afi_get_result (handle->afi_handle, op->file_data->afi_id, &pck_res);
    we_dlg_handle_afi_result (handle, op, pck_state, &pck_res, TRUE);
  }
  return WE_DLG_STATE_OPEN_NAVIGATE;
}

static int
we_dlg_update_save_window (we_dlg_handle_t *handle, we_dlg_op_t *op, 
                             const char* mime_types, const char* path)
{
  we_dprc_clear_file_dlg (op->dialog->handle);
  op->file_data->nbr_items_in_file_dlg = 0;

  if (path == NULL){
    return we_dlg_show_root_folders (handle, op,
                                      mime_types, FALSE);
  }
  else {
    int pck_state = 0;
    we_pck_result_t pck_res;

    if (handle->icons == NULL) {
      WE_GET_ICONS (handle->modid, op->wid, 0, NULL);
      return WE_DLG_STATE_SAVE_AS_NAVIGATE_DELAYED;
    }
    if (op->title == 0) {
      WeStringHandle loc_path = 0;
      char* str_path = NULL;

      str_path = we_pck_get_localized_pathname(handle->modid, path);
      if (str_path) {
        if (str_path[strlen(str_path)-1] == '/') {
          str_path[strlen(str_path)-1] = '\0';
        }
        loc_path = create_we_string( handle->modid, str_path+1);
        we_dprc_set_title (op->dialog->handle, loc_path);
        WE_WIDGET_RELEASE (loc_path);
        WE_MEM_FREE(handle->modid, str_path);
      }
    }

    op->file_data->afi_id = we_afi_get_dir_entries (handle->afi_handle, path, 0);
    pck_state = we_afi_get_result (handle->afi_handle, op->file_data->afi_id, &pck_res);

    return we_dlg_handle_afi_result (handle, op, pck_state, &pck_res, FALSE);
  }
  /* return WE_DLG_STATE_SAVE_AS_NAVIGATE; unreachable code due to return in both if and else above */
}


static void
create_error_popup_window(we_dlg_handle_t *handle, we_dlg_op_t *op, 
                          WE_INT32 text_id, int prev_state)
{
  we_dprc_message_t message_data;
  we_dlg_dialog_t *dialog = NULL;

  dialog = get_new_dialog(handle, op);
  
  message_data.numStrId = 1;
  message_data.str_ids = WE_MEM_ALLOCTYPE (handle->modid, WeStringHandle);
  message_data.str_ids[0] = WE_WIDGET_STRING_GET_PREDEFINED (text_id);
  message_data.singel_dlg = TRUE;
  message_data.pakage_type = handle->dlg_pkg_type;
  message_data.title = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_COMMON_ERROR);
  message_data.ok_string = 0;

  dialog->handle = we_dprc_create (handle->modid, op->screen, 
                                    WE_DLG_MESSAGE_ERROR, &message_data);
  
  WE_MEM_FREE (handle->modid, message_data.str_ids);
  op->prev_state = prev_state;
  op->state = WE_DLG_STATE_ERROR_POPUP;
}

static void
create_name_edit_window(we_dlg_handle_t *handle, we_dlg_op_t *op)
{
  we_dprc_prompt_t       prompt_data;
  we_dlg_dialog_t       *dialog = NULL;
  WeStringHandle         default_text = 0;

  dialog = get_new_dialog(handle, op);
  
  prompt_data.cancel_string = 0;
  prompt_data.ok_string = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_SAVE);
  prompt_data.message = 0;
  prompt_data.title = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_TEXT_EDIT_FILENAME);
  prompt_data.singel_dlg = TRUE;
  prompt_data.pakage_type = handle->dlg_pkg_type;
  prompt_data.nbr_of_input_fields = 1;

  if (op->file_data->default_name) {
    default_text = create_we_string( handle->modid, op->file_data->default_name);
  }
  else {
    default_text = create_we_string( handle->modid, "");
  } 
  
  prompt_data.input_fields = WE_MEM_ALLOCTYPE(handle->modid, we_dprc_input_field_t);
  prompt_data.input_fields[0].default_text = default_text;
  prompt_data.input_fields[0].label = 0;
  prompt_data.input_fields[0].max_size = 0;
  prompt_data.input_fields[0].type = WeFile;

  dialog->handle = we_dprc_create (handle->modid, op->screen, 
                                    WE_DLG_PROMPT, &prompt_data);
  WE_MEM_FREE (handle->modid, prompt_data.input_fields);
}

static void
create_exists_dialog (we_dlg_handle_t *handle, we_dlg_op_t *op, char *name)
{
  we_dprc_confirm_t  confirm_data;
  we_dlg_dialog_t   *dialog = NULL;
  char               *dialog_text = NULL;
  char               *exists_text1 = NULL;
  char               *exists_text2 = NULL;
  WeStringHandle     string = 0;
  int                 length = 0;
  int                 i = 0;
  int                 overwite_protected = FALSE;

  for (i = 0; i < op->file_data->dir_entries->num_entries; i++) {
    if (!strcmp (op->file_data->dir_entries->entry_list[i].name, name)) {
      overwite_protected = file_is_write_protected (op->file_data->dir_entries->entry_list[i].attr_list);
      break;
    }
  }
  
  if ((op->file_data->bit_flag & WE_DLG_ALLOW_OVERWRITE) > 0 &&
      !overwite_protected) {

    string = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_EXISTS_TEXT1);  
    length = WE_WIDGET_STRING_GET_LENGTH (string, 1, WeUtf8)+1;
    exists_text1 = WE_MEM_ALLOC (handle->modid, length);
    WE_WIDGET_STRING_GET_DATA (string, exists_text1, WeUtf8);

    string = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_EXISTS_TEXT2);
    length = WE_WIDGET_STRING_GET_LENGTH (string, 1, WeUtf8)+1;
    exists_text2 = WE_MEM_ALLOC (handle->modid, length);    
    WE_WIDGET_STRING_GET_DATA (string, exists_text2, WeUtf8);
  }
  else {
    string = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_NO_REPLACE_TEXT1);
    length = WE_WIDGET_STRING_GET_LENGTH (string, 1, WeUtf8)+1;
    exists_text1 = WE_MEM_ALLOC (handle->modid, length);
    WE_WIDGET_STRING_GET_DATA (string, exists_text1, WeUtf8);

    string = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_NO_REPLACE_TEXT2);
    length = WE_WIDGET_STRING_GET_LENGTH (string, 1, WeUtf8)+1;
    exists_text2 = WE_MEM_ALLOC (handle->modid, length);   
    WE_WIDGET_STRING_GET_DATA (string, exists_text2, WeUtf8);
  }

  dialog_text = WE_MEM_ALLOC(handle->modid, strlen(exists_text1) + 
                              strlen(name) + strlen(exists_text2)+1);
  strcpy(dialog_text, exists_text1);
  strcat(dialog_text, name);
  strcat(dialog_text, exists_text2);

  dialog = get_new_dialog(handle, op);
  
  confirm_data.numStrId = 1;
  confirm_data.str_ids = WE_MEM_ALLOCTYPE (handle->modid, WeStringHandle);
  confirm_data.str_ids[0] = create_we_string(handle->modid, dialog_text);
  confirm_data.singel_dlg = TRUE;
  confirm_data.pakage_type = handle->dlg_pkg_type;
  confirm_data.title = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_DLG_EXISTS_TITLE);
  confirm_data.ok_string = 0;
  confirm_data.cancel_string = 0;

  dialog->handle = we_dprc_create (handle->modid, op->screen, 
                                    WE_DLG_CONFIRM, &confirm_data);
  
  WE_WIDGET_RELEASE (confirm_data.str_ids[0]);
  WE_MEM_FREE (handle->modid, confirm_data.str_ids);
  WE_MEM_FREE(handle->modid, exists_text1);
  WE_MEM_FREE(handle->modid, exists_text2);
  WE_MEM_FREE(handle->modid, dialog_text);

  op->state = WE_DLG_STATE_SAVE_AS_EXISTS_DIALOG;
}

static void
create_error_msg_dialog (we_dlg_handle_t *handle, we_dlg_op_t *op, 
                         WeStringHandle title, WeStringHandle str)
{
  we_dprc_message_t message_data;
  we_dlg_dialog_t *dialog = NULL;

  dialog = get_new_dialog(handle, op);
  
  message_data.numStrId = 1;
  message_data.str_ids = WE_MEM_ALLOCTYPE (handle->modid, WeStringHandle);
  message_data.str_ids[0] = str;
  message_data.singel_dlg = TRUE;
  message_data.pakage_type = handle->dlg_pkg_type;
  message_data.title = title;
  message_data.ok_string = 0;

  dialog->handle = we_dprc_create (handle->modid, op->screen, 
                                    WE_DLG_MESSAGE_ERROR, &message_data);
  
  WE_MEM_FREE (handle->modid, message_data.str_ids);
}

static void
create_new_dir_dialog(we_dlg_handle_t *handle, we_dlg_op_t *op)
{
  we_dprc_prompt_t       prompt_data;
  we_dlg_dialog_t *dialog = NULL;

  dialog = get_new_dialog(handle, op);
  
  prompt_data.cancel_string = 0;
  prompt_data.ok_string = 0;
  prompt_data.message = 0;
  prompt_data.title = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_TITLE_CREATE_DIR);
  prompt_data.singel_dlg = TRUE;
  prompt_data.pakage_type = handle->dlg_pkg_type;
  prompt_data.nbr_of_input_fields = 1;

  prompt_data.input_fields = WE_MEM_ALLOCTYPE(handle->modid, we_dprc_input_field_t);
  prompt_data.input_fields[0].default_text = 0;
  prompt_data.input_fields[0].label = 0;
  prompt_data.input_fields[0].max_size = 0;
  prompt_data.input_fields[0].type = WeFile;

  dialog->handle = we_dprc_create (handle->modid, op->screen, 
                                    WE_DLG_PROMPT, &prompt_data);
  WE_MEM_FREE (handle->modid, prompt_data.input_fields);
}

static int
we_dlg_try_create_dir (we_dlg_handle_t *handle, we_dlg_op_t *op, 
                        we_dlg_dialog_t *dialog)
{
  int                       i = 0;
  int                       length = 0;
  char                     *name = NULL;
  char                     *full_path = NULL;
  char                     *full_path_with_end = NULL;
  we_dprc_prompt_result_t  prompt_result;
  we_pck_result_t          result;

  we_dprc_get_response (dialog->handle, &prompt_result);

  length = WE_WIDGET_STRING_GET_LENGTH (prompt_result.string_list[0],
                                         TRUE, WeUtf8);
  name = WE_MEM_ALLOC (handle->modid, length);

  WE_WIDGET_STRING_GET_DATA(prompt_result.string_list[0], name, WeUtf8);
  for (i = 0;i < prompt_result.num_of_strings; i++) {
    WE_WIDGET_RELEASE (prompt_result.string_list[i]);
  }

  WE_MEM_FREE (handle->modid, prompt_result.string_list);

  full_path = we_cmmn_strcat(handle->modid, op->file_data->file_name, name);
  WE_MEM_FREE (handle->modid, name);
  
  full_path_with_end = we_cmmn_strcat(handle->modid, full_path, "/");
  WE_MEM_FREE(handle->modid, full_path);

  op->file_data->afi_id = we_afi_mkdir (handle->afi_handle, full_path_with_end);
  WE_MEM_FREE(handle->modid, full_path_with_end);
  
  switch (we_afi_get_result(handle->afi_handle, op->file_data->afi_id, &result)) {
  case WE_PACKAGE_COMPLETED:
    if (result._u.i_val == WE_AFI_OK) {
      free_dlg_dialog(handle, op, dialog);
      op->state = we_dlg_update_save_window (handle, op, 
                                              op->file_data->mime_types, 
                                              op->file_data->file_name);
    }
    else {
      we_dlg_handle_afi_error (handle, op, &result);
    }
    break;
  case WE_PACKAGE_ERROR:
    free_dlg_dialog(handle, op, dialog);
    op->state = we_dlg_update_save_window (handle, op, 
                                            op->file_data->mime_types, 
                                            op->file_data->file_name);
    break;
  case WE_PACKAGE_BUSY:
    op->state = WE_DLG_STATE_NEW_DIR_AFI_DELAYED;
    break;
  }
  
  we_afi_result_free(handle->afi_handle, &result); 
  return WE_PACKAGE_SIGNAL_HANDLED;
}

static void
create_cannot_save_dialog (we_dlg_handle_t *handle, we_dlg_op_t *op)
{
  we_dprc_message_t message_data;
  we_dlg_dialog_t *dialog = NULL;

  dialog = get_new_dialog(handle, op);
  
  message_data.numStrId = 1;
  message_data.str_ids = WE_MEM_ALLOCTYPE (handle->modid, WeStringHandle);
  message_data.str_ids[0] = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_CANNOT_SAVE);
  message_data.singel_dlg = TRUE;
  message_data.pakage_type = handle->dlg_pkg_type;
  message_data.title = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_COMMON_ERROR);
  message_data.ok_string = 0;

  dialog->handle = we_dprc_create (handle->modid, op->screen, 
                                    WE_DLG_MESSAGE_ERROR, &message_data);
  
  WE_MEM_FREE (handle->modid, message_data.str_ids);
  op->state = WE_DLG_STATE_SAVE_AS_CANNOT_SAVE;
}


static int
we_dlg_handle_file_exist (we_dlg_handle_t *handle, we_dlg_op_t *op, 
                           we_dlg_dialog_t *dialog)
{
  int dlg_res = we_dprc_get_response(dialog->handle, NULL);
  int overwite_protected = FALSE;
  int i = 0;

  for (i = 0; i < op->file_data->dir_entries->num_entries; i++) {
    if (!strcmp (op->file_data->dir_entries->entry_list[i].name, op->file_data->entered_name)) {
      overwite_protected = file_is_write_protected (op->file_data->dir_entries->entry_list[i].attr_list);
      break;
    }
  }
  
  if (dlg_res == WE_DLG_RESPONSE_POSITIVE && 
      !overwite_protected &&
      op->file_data->bit_flag & WE_DLG_ALLOW_OVERWRITE) {
    char              *path;
    we_pck_result_t  result;
    

    path = WE_MEM_ALLOC (handle->modid, strlen(op->file_data->file_name) + 
                          strlen(op->file_data->entered_name) + 1);
    strcpy(path, op->file_data->file_name);
    strcat(path, op->file_data->entered_name);
    WE_MEM_FREE(handle->modid, op->file_data->file_name);
    op->file_data->file_name = path;

    op->file_data->afi_id = we_afi_remove (handle->afi_handle, path);
    switch (we_afi_get_result(handle->afi_handle, op->file_data->afi_id, &result)) {
    case WE_PACKAGE_BUSY:
      op->state = WE_DLG_STATE_SAVE_AS_EXISTS_DELETE_DELAYED;
      we_afi_result_free(handle->afi_handle, &result);
      return WE_PACKAGE_SIGNAL_HANDLED;
      break;
    case WE_PACKAGE_COMPLETED:
      if (result._u.i_val == WE_AFI_OK) {
        op->status = WE_DLG_RETURN_OK;
      }
      else {
        we_dlg_handle_afi_error (handle, op, &result);
        we_afi_result_free(handle->afi_handle, &result);
        return WE_PACKAGE_SIGNAL_HANDLED;
      }
      break;
    default:
      /*Package replayed error*/
      we_dlg_handle_afi_error (handle, op, &result);
      we_afi_result_free(handle->afi_handle, &result);
      return WE_PACKAGE_SIGNAL_HANDLED;
      break;
    }
    we_afi_result_free(handle->afi_handle, &result);
    op->state = WE_DLG_STATE_SAVE_AS_FINISHED;        
    return WE_PACKAGE_OPERATION_COMPLETE;
  }
  else {
    /*Cancel exists dialog and reedit name*/
    free_dlg_dialog(handle, op, dialog);
    op->state = WE_DLG_STATE_SAVE_AS_EDIT_NAME;
    return WE_PACKAGE_SIGNAL_HANDLED;
  }
}

static int
we_dlg_handle_action (we_dlg_handle_t *handle, we_dlg_op_t *op, 
                       we_dlg_dialog_t *dialog, WeActionHandle action_handle)
{
  int rsp = 0;

  we_dprc_action (dialog->handle, action_handle);

  switch (op->state) {
  case WE_DLG_STATE_ERROR_POPUP:
    free_dlg_dialog(handle, op, dialog);
    op->state = op->prev_state;
    return WE_PACKAGE_SIGNAL_HANDLED;
  case WE_DLG_STATE_SAVE_AS_EXISTS_DIALOG:
    return we_dlg_handle_file_exist (handle, op, dialog);
  case WE_DLG_STATE_NEW_DIR_NAME_EDIT:
    {
      int dlg_res = we_dprc_get_response(dialog->handle, NULL);
      if (allow_create_new_dir(handle->modid, op->file_data->file_name)) {
        switch (dlg_res) {
        case WE_DLG_RESPONSE_POSITIVE:
          return we_dlg_try_create_dir(handle, op, dialog);
          /* dlg_res = dlg_res; unreachable code due to the return statement above */
          /* break; unreachable code due to the return statement above */
        case WE_DLG_RESPONSE_CANCEL:
          free_dlg_dialog(handle, op, dialog);
          op->state = WE_DLG_STATE_SAVE_AS_NAVIGATE;
          break;
        default:
          free_dlg_dialog(handle, op, dialog);
          op->state = WE_DLG_STATE_SAVE_AS_NAVIGATE;
        }
      }
      else {
        free_dlg_dialog(handle, op, dialog);
        op->state = WE_DLG_STATE_SAVE_AS_NAVIGATE;
      }
    }
    break;
  case WE_DLG_STATE_SAVE_AS_CANNOT_SAVE:
    op->status = WE_DLG_RETURN_CANCELLED;
    op->state = WE_DLG_STATE_SAVE_AS_FINISHED;        
    free_dlg_dialog(handle, op, dialog);
    return WE_PACKAGE_OPERATION_COMPLETE;
    break;
  case WE_DLG_STATE_SAVE_AS_EDIT_NAME:
  case WE_DLG_STATE_SAVE_AS_EDIT_NAME_ONLY:
    {
      we_dprc_prompt_result_t prompt_result;
      int dlg_res = we_dprc_get_response(dialog->handle, &prompt_result);
      switch (dlg_res) {
      case WE_DLG_RESPONSE_POSITIVE:
        {
          char              *name = NULL;
          char              *temp_file_name = NULL;
          int                i = 0;
          
          if (prompt_result.num_of_strings == 0) {
            return WE_PACKAGE_SIGNAL_HANDLED;
          }
          name = WE_MEM_ALLOC(handle->modid, 
                               WE_WIDGET_STRING_GET_LENGTH (prompt_result.string_list[0],
                               1, WeUtf8)+1);
          WE_WIDGET_STRING_GET_DATA(prompt_result.string_list[0], name, WeUtf8);
          for (i = 0;i < prompt_result.num_of_strings; i++) {
            WE_WIDGET_RELEASE(prompt_result.string_list[i]);
          }

          WE_MEM_FREE (handle->modid, prompt_result.string_list);
          if (*name == '\0') {
            create_error_popup_window (handle, op, WE_STR_ID_DLG_ERROR_NAME, 
                                       WE_DLG_STATE_SAVE_AS_EDIT_NAME);
            WE_MEM_FREE (handle->modid, name);
            return WE_PACKAGE_SIGNAL_HANDLED;
          }
          if (op->file_data->entered_name) {
            WE_MEM_FREE (handle->modid, op->file_data->entered_name);
          }
          op->file_data->entered_name = name;
               
          temp_file_name = WE_MEM_ALLOC(handle->modid, 
                                         strlen(op->file_data->file_name)
                                         + strlen(name) + 1 );

          strcpy(temp_file_name, op->file_data->file_name);
          strcat(temp_file_name, name);

          if (WE_FILE_GETSIZE (temp_file_name) != TPI_FILE_ERROR_PATH) {
            WE_MEM_FREE(handle->modid, temp_file_name);
            create_exists_dialog (handle, op, name);
            return WE_PACKAGE_SIGNAL_HANDLED;         
          }

          WE_MEM_FREE(handle->modid, op->file_data->file_name);
          op->file_data->file_name = temp_file_name;

          op->status = WE_DLG_RETURN_OK;
          op->state = WE_DLG_STATE_SAVE_AS_FINISHED;
          return WE_PACKAGE_OPERATION_COMPLETE;
        }
      case WE_DLG_RESPONSE_CANCEL:
        WE_MEM_FREE (handle->modid, prompt_result.string_list);
        if (op->state == WE_DLG_STATE_SAVE_AS_EDIT_NAME_ONLY) {
          /*No possibility to navigate if cancelled packet is completed*/
          op->status = WE_DLG_RETURN_CANCELLED;
          op->state = WE_DLG_STATE_SAVE_AS_FINISHED;
          return WE_PACKAGE_OPERATION_COMPLETE;
        }
        else {
          /*Exit dialog*/
          free_dlg_dialog(handle, op, dialog);
          op->state = WE_DLG_STATE_SAVE_AS_NAVIGATE;
          return WE_PACKAGE_SIGNAL_HANDLED;
        }
      default:
        /*Exit package*/
        op->status = WE_DLG_RETURN_CANCELLED;
        op->state = WE_DLG_STATE_SAVE_AS_FINISHED;
        return WE_PACKAGE_OPERATION_COMPLETE;
        break;
      }
    }
    case WE_DLG_STATE_SAVE_AS_NAVIGATE:
      {
        we_dprc_file_result_t result;
        int dlg_res = we_dprc_get_response(dialog->handle, &result);
        we_dprc_set_background_image (dialog->handle, 0);

        if (dlg_res == WE_DLG_RESPONSE_POSITIVE) {
          int selected_item = 0;
          selected_item = result.selected_item;
          if (selected_item >= 0) {
            if (op->file_data->file_name == NULL) {
              /*A root folder selected*/
               we_pck_root_folders_t    *root_folders;

              we_pck_get_urf_from_mime (handle->modid, op->file_data->mime_types,  &root_folders);            
              if (op->file_data->file_name) {
                WE_MEM_FREE(handle->modid, op->file_data->file_name);
              }
              op->file_data->file_name = we_cmmn_strdup (handle->modid, 
                                                          root_folders->rf[selected_item].pathname);

              op->state = we_dlg_update_save_window (handle, op, 
                                                       op->file_data->mime_types, 
                                                       op->file_data->file_name);
              WE_MEM_FREE( handle->modid, root_folders);
            }
            else if (selected_item == 0) {
              /* Save here pressed*/
              create_name_edit_window(handle, op);
              op->state = WE_DLG_STATE_SAVE_AS_EDIT_NAME;
            }
            else {
              /*a directory or a file selected*/
              we_dlg_index_list_t *index_list = we_dlg_get_index_list (handle, op, 
                                                                         op->file_data->dir_entries);
              int selection = get_index_from_index_list_number(index_list, selected_item-1);

              delete_index_list(handle, index_list);
              if (op->file_data->dir_entries->entry_list[selection].is_dir == TRUE){
                /*directory selected?*/
                char *temp_file_name; 
                int size = strlen(op->file_data->file_name) + 
                                  strlen(op->file_data->dir_entries->entry_list[selection].name)
                                  + 1 + 1;

                temp_file_name = WE_MEM_ALLOC(handle->modid, size);
                strcpy(temp_file_name, op->file_data->file_name);
                strcat(temp_file_name, op->file_data->dir_entries->entry_list[selection].name);
                strcat(temp_file_name, "/");
                WE_MEM_FREE(handle->modid, op->file_data->file_name);
                op->file_data->file_name = temp_file_name;
                op->state = we_dlg_update_save_window (handle, op, 
                                                        op->file_data->mime_types, 
                                                        op->file_data->file_name);
              }
              else {
                /*File selected*/
              }
            }
          }

        }
        else if (dlg_res == WE_DLG_RESPONSE_CANCEL) {
          if (op->file_data->file_name == NULL) {
            /*Exit dialog*/
            op->status = WE_DLG_RETURN_CANCELLED;
            op->state = WE_DLG_STATE_SAVE_AS_FINISHED;
            return WE_PACKAGE_OPERATION_COMPLETE;
          }
          else {
            char *parent_dir = we_pck_get_parent_dir (handle->modid, 
                                                       op->file_data->file_name);
            WE_MEM_FREE (handle->modid, op->file_data->file_name);
            op->file_data->file_name = parent_dir;
            if (strlen(op->file_data->file_name) == 1) {
                WE_MEM_FREE(handle->modid, op->file_data->file_name);
              op->file_data->file_name = NULL;
            }
            op->state = we_dlg_update_save_window (handle, op, 
                                                    op->file_data->mime_types, 
                                                    op->file_data->file_name);
          }
        }
        else if (dlg_res == WE_DLG_RESPONSE_MENU) {
          if (allow_create_new_dir(handle->modid, op->file_data->file_name)) {
            create_new_dir_dialog (handle, op);
            op->state = WE_DLG_STATE_NEW_DIR_NAME_EDIT;
          }
          else {
            create_error_msg_dialog (handle, op, 0,
                                     WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_DIR_NOT_ALLOWED));
            op->state = WE_DLG_STATE_NEW_DIR_NAME_EDIT;
          }
        }
      }
      break;
    case WE_DLG_STATE_OPEN_NAVIGATE:
      {
        we_dprc_file_result_t result;
        int dlg_res = we_dprc_get_response(dialog->handle, &result);

        /*set default background*/
        we_dprc_set_background_image (dialog->handle, 0);
        /*reset preview timer*/
        if (op->file_data->preview_state == WE_DLG_PREVIEW_STATE_SLEEP) {
          WE_TIMER_RESET(handle->modid, op->file_data->current_timer_id);
          op->file_data->preview_state = WE_DLG_PREVIEW_STATE_IDLE;
        }

        if (dlg_res == WE_DLG_RESPONSE_POSITIVE) {
          int selection = 0;

          /*get element in focus*/
          selection = result.selected_item;

          if (op->file_data->file_name == NULL) {
            we_pck_root_folders_t    *root_folders;

            we_pck_get_urf_from_mime (handle->modid, op->file_data->mime_types,  &root_folders);
            op->file_data->file_name = we_cmmn_strdup (handle->modid, 
                                                        root_folders->rf[selection].pathname);

            op->state = we_dlg_update_open_window (handle, op,
                                                     op->file_data->mime_types,
                                                     op->file_data->file_name);

            WE_MEM_FREE( handle->modid, root_folders);
          }
          else {
            char *temp_file_name;
            int j;
            we_dlg_index_list_t *index_list;
            index_list = we_dlg_get_index_list (handle, op, op->file_data->dir_entries);
            j = get_index_from_index_list_number(index_list, selection);
            delete_index_list(handle, index_list);
            if (j < 0) {
              return WE_PACKAGE_SIGNAL_HANDLED;
            }
            /*if forward locked its not possible to select*/
            
            if (is_forward_locked(op->file_data->dir_entries->entry_list[j].attr_list) && 
                op->file_data->permission == WE_DLG_PERMISSION_FORWARD) {
              return WE_PACKAGE_SIGNAL_HANDLED;
            }

            if (op->file_data->dir_entries->entry_list[j].is_dir == TRUE) {
              /*Directory selected*/
              char *temp_file_name;
              temp_file_name = WE_MEM_ALLOC(handle->modid,
                                             strlen(op->file_data->file_name) + 
                                             strlen(op->file_data->dir_entries->entry_list[j].name) +
                                             1 + 1);
              strcpy(temp_file_name, op->file_data->file_name);
              strcat(temp_file_name, op->file_data->dir_entries->entry_list[j].name);
              strcat(temp_file_name, "/");
              WE_MEM_FREE(handle->modid, op->file_data->file_name);
              op->file_data->file_name = temp_file_name;
              op->state = we_dlg_update_open_window (handle, op,
                                                      op->file_data->mime_types,
                                                      op->file_data->file_name);

            }
            else {
              /*file selected*/
              int size = strlen (op->file_data->file_name) + 
                                 strlen(op->file_data->dir_entries->entry_list[j].name) + 1;
              
              temp_file_name = WE_MEM_ALLOC(handle->modid, size);
              strcpy(temp_file_name, op->file_data->file_name);
              strcat(temp_file_name, op->file_data->dir_entries->entry_list[j].name);
              WE_MEM_FREE(handle->modid, op->file_data->file_name);
              op->file_data->file_name = temp_file_name;
              op->status = WE_DLG_RETURN_OK;
              op->state = WE_DLG_STATE_OPEN_FINISHED;
              return WE_PACKAGE_OPERATION_COMPLETE;
            }
          }
        }
        else if (dlg_res == WE_DLG_RESPONSE_CANCEL) {
          if (op->file_data->file_name == NULL) {
            /*Exit dialog*/
            op->status = WE_DLG_RETURN_CANCELLED;
            op->state = WE_DLG_STATE_OPEN_FINISHED;
            return WE_PACKAGE_OPERATION_COMPLETE;
          }
          else {
            char *parent_dir = we_pck_get_parent_dir (handle->modid, op->file_data->file_name);
            WE_MEM_FREE (handle->modid, op->file_data->file_name);
            op->file_data->file_name = parent_dir;
            if (strlen(op->file_data->file_name) == 1) {
              WE_MEM_FREE(handle->modid, op->file_data->file_name);
              op->file_data->file_name = NULL;
            }
            op->state = we_dlg_update_open_window (handle, op, 
                                                    op->file_data->mime_types,
                                                    op->file_data->file_name);
          }
        }
      }
    break;
   default:
    /*if a callback is used get result and call the fuction*/
    if (op->result_callback) {
      we_pck_result_t dlg_result;
      we_dlg_result_callback callback = op->result_callback;
      we_dlg_get_result (handle, op->wid, &dlg_result);
      callback(&dlg_result);
      we_dlg_result_free (handle, &dlg_result);
      return WE_PACKAGE_OPERATION_COMPLETE;
    }
    rsp = we_dprc_get_response (dialog->handle, NULL);
    if (rsp == WE_DLG_RESPONSE_POSITIVE) {
      op->status = WE_DLG_RETURN_OK;
    }
    else if (rsp == WE_DLG_RESPONSE_CANCEL) {
      op->status = WE_DLG_RETURN_CANCELLED;
    }
    return WE_PACKAGE_OPERATION_COMPLETE;
  }
  return WE_PACKAGE_SIGNAL_HANDLED;
}


static int
we_dlg_handle_widget_action (we_dlg_handle_t *handle, WE_INT32 *wid, 
                              WeWindowHandle window_handle, WeActionHandle action_handle)
{
  we_dlg_op_t      *op = handle->op;
  we_dlg_op_t      *found_op = NULL;
  we_dlg_dialog_t  *found_dialog = NULL;

  while (op && (found_op == NULL)) {
    we_dlg_dialog_t *dialog = op->dialog;
    while (dialog && (found_dialog == NULL)) {
      if (we_dprc_check_handle (dialog->handle, window_handle)) {
        found_op = op;
        found_dialog = dialog;
        *wid = op->wid;
      }
      dialog = dialog->next;
    }
    op = op->next;
  }
  if (found_op && found_dialog) {
    return we_dlg_handle_action (handle, found_op, found_dialog, action_handle);
  }
  return WE_PACKAGE_SIGNAL_NOT_HANDLED;
}


static void
we_dlg_navigate_handle_user_evt_up_down(we_dlg_handle_t *handle, we_dlg_op_t *op, 
                                         we_dlg_dialog_t *dialog, WeEventType eventType)
{
  int focus;
  int selection = 0;
  int last_choice = 0;
  we_dprc_file_result_t result;

  we_dprc_get_response (op->dialog->handle, &result);
  if (result.nbr_of_items <= 0) /* TR21380: if no items in the list, do nothing when a up or down key is pressed */
  {
    return;
  }
  focus = selection = result.selected_item;
  last_choice = op->file_data->nbr_items_in_file_dlg-1;

  switch (eventType){
  case WeKey_Up:
    if (selection != 0)
      focus = selection - 1;
    break;
  case WeKey_Down:
    if (selection < last_choice)
      focus = selection + 1;
    break;
  }
  
  if (selection != focus) {
    we_dlg_index_list_t *index_list; 
    char             *mime;
    
    we_dprc_set_file_dlg_item_state (dialog->handle, selection, 0);
    we_dprc_set_file_dlg_item_state (dialog->handle, focus, WE_CHOICE_ELEMENT_FOCUSED);

    op->file_data->current_focus = focus;

    if (op->file_data->preview_state == WE_DLG_PREVIEW_STATE_SLEEP) {
      WE_TIMER_RESET(handle->modid, op->file_data->current_timer_id);
    }
    op->file_data->preview_state = WE_DLG_PREVIEW_STATE_IDLE;
    
    /*set default image*/
    we_dprc_set_background_image (dialog->handle, 0);

    if (op->file_data->file_name == NULL) {
      return;
    }

    index_list = we_dlg_get_index_list (handle, op,
                                         op->file_data->dir_entries);

    selection = get_index_from_index_list_number (index_list,
                                                  op->file_data->current_focus);
    delete_index_list(handle, index_list);
    if (selection < 0) {
      return;
    }
    mime = get_mime_from_attr_list(op->file_data->dir_entries->entry_list[selection].attr_list);
    if (!we_pck_mime_type_in_list(mime, WE_DLG_CONTENT_TYPE_IMAGE) ||
       (op->file_data->dir_entries->entry_list[selection].is_dir == TRUE) ||
       (get_size_from_attr_list(op->file_data->dir_entries->entry_list[selection].attr_list) > 
        WE_DLG_CONTENT_MAX_SIZE)) {
      return;
    }
    we_dlg_start_preview_timer(handle, op);
  }

}

static int
we_dlg_handle_user_evt (we_dlg_handle_t *handle, WE_INT32 *wid, WE_UINT32 window_handle, 
                         WeEvent evt)
{
  we_dlg_op_t      *op = handle->op;
  we_dlg_op_t      *found_op = NULL;
  we_dlg_dialog_t  *found_dialog = NULL;

  while (op && (found_op == NULL)) {
    we_dlg_dialog_t *dialog = op->dialog;
    while (dialog && (found_dialog == NULL)) {
      if (we_dprc_check_handle (dialog->handle,window_handle)) {
        found_op = op;
        found_dialog = dialog;
        *wid = op->wid;
      }
      dialog = dialog->next;
    }
    op = op->next;
  }
  if (found_op) {
    if (evt.eventClass != WeRelease) {
      return WE_PACKAGE_SIGNAL_HANDLED;
    }
 
    if (found_op->state == WE_DLG_STATE_OPEN_NAVIGATE) {
      
      switch (evt.eventType) {

      case WeKey_Up:
        we_dlg_navigate_handle_user_evt_up_down(handle, found_op, found_dialog, WeKey_Up);
        break;

      case WeKey_Down:
        we_dlg_navigate_handle_user_evt_up_down(handle, found_op, found_dialog, WeKey_Down);
        break;
      default:
        break;
      }
    }
  }

  if (found_op) {
    return WE_PACKAGE_SIGNAL_HANDLED;
  }
  return WE_PACKAGE_SIGNAL_NOT_HANDLED;
}

static int
we_dlg_handle_progress_timer_expired (we_dlg_handle_t* dlg_handle, 
                                       WE_UINT32 timer_id, WE_INT32 *wid )
{
  we_dlg_op_t *op = dlg_handle->op;
  we_dlg_op_t *found_op = NULL;
  
  while (op && (found_op == NULL)) {
    if (op->progress_data->timer_id == timer_id) {
      found_op = op;
      *wid = op->wid;
    }
    op = op->next;
  }
  if (found_op && timer_id == found_op->progress_data->timer_id ) {
    (found_op->progress_data->data->value)++;

    if (found_op->progress_data->data->value > found_op->progress_data->data->max_value) {
      found_op->progress_data->data->value = 0;
    }
    WE_TIMER_SET (dlg_handle->modid, timer_id, found_op->progress_data->data->tick_time);
    we_dprc_update_progressbar (found_op->progress_data->dialog->handle, 
                                 found_op->progress_data->data);
    return WE_PACKAGE_SIGNAL_HANDLED;
  }
  return WE_PACKAGE_SIGNAL_NOT_HANDLED;
}

static int
we_dlg_handle_timer_expired(we_dlg_handle_t *handle, WE_INT32 *wid, int timer_id)
{
  char                    *path;
  const char              *mime;
  int                     selection;
  we_dlg_index_list_t    *index_list; 
  we_dlg_op_t            *op;
  we_dlg_op_t            *found_op = NULL;
  WeCreateData           createData;

  if (handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  
  op = handle->op;

  while (op && (found_op == NULL)) {
    if (op->file_data && op->file_data->current_timer_id == timer_id) {
      found_op = op;
    }
    op = op->next;
  }
  
  if (found_op == NULL) {
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;
  }

  op = found_op;
  *wid = op->wid;

  index_list = we_dlg_get_index_list (handle, op, op->file_data->dir_entries);
  selection = get_index_from_index_list_number (index_list, 
                                                op->file_data->current_focus);
  delete_index_list(handle, index_list);

  if (selection < 0) {
    return WE_PACKAGE_SIGNAL_HANDLED;  
  }
  mime = get_mime_from_attr_list(op->file_data->dir_entries->entry_list[selection].attr_list);

  if (mime == NULL) {
    return WE_PACKAGE_SIGNAL_HANDLED;
  }
  
  path = we_cmmn_strcat(handle->modid, op->file_data->file_name, 
                         op->file_data->dir_entries->entry_list[selection].name);

  WE_WIDGET_RELEASE (op->file_data->preview_image_handle);
  
  createData.resource = path;
  op->file_data->preview_image_handle = WE_WIDGET_IMAGE_CREATE (handle->modid, &createData,
                                                                 mime, WeResourceFile, 0);
  WE_MEM_FREE(handle->modid, path);

  we_dprc_set_background_image (op->dialog->handle, op->file_data->preview_image_handle);

  return WE_PACKAGE_SIGNAL_HANDLED;
}

/*******************************************************************
 * Global Functions
 *******************************************************************/

/*
 * Initialize the DLG package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t *
we_dlg_init (WE_UINT8 modid)
{
  we_dlg_handle_t *h = WE_MEM_ALLOCTYPE (modid, we_dlg_handle_t);

  if (h) {
    h->modid = modid;
    h->next = h->prev = NULL;
    h->afi_handle = we_afi_init (modid);
    h->op = NULL;
    h->folder_image = WE_WIDGET_IMAGE_GET_PREDEFINED (WE_IMAGE_ID_ICON_FOLDER, NULL);
    h->file_image = WE_WIDGET_IMAGE_GET_PREDEFINED (WE_IMAGE_ID_ICON_FILE, NULL);
    h->icons = NULL;
    h->n_items = 0;
    h->dlg_pkg_type = 0;
  }
  return (we_pck_handle_t *)h;
}

void
we_dlg_set_type (we_pck_handle_t *handle, we_dlg_pkg_type_t pakage_type)
{
  if (handle) { 
    ((we_dlg_handle_t*)handle)->dlg_pkg_type = pakage_type;
  }
}

void
we_dlg_set_in_focus(we_pck_handle_t *handle)
{
  if (((we_dlg_handle_t*)handle)->op) {
    we_dlg_dialog_t * dialog = NULL;
    dialog = ((we_dlg_handle_t*)handle)->op->dialog;
    we_dprc_set_in_focus(dialog->handle);
  }
}

int
we_dlg_get_result (we_pck_handle_t* handle, WE_INT32 wid,
                    we_pck_result_t* result)
{
  int res = 0;

  res = we_dlg_get_result_ext(handle, wid, result);  
  if (res == WE_PACKAGE_ERROR) {
    return FALSE;
  }
  return TRUE;
}

int
we_dlg_get_result_ext (we_pck_handle_t* handle, WE_INT32 wid,
                        we_pck_result_t* result)
{
  we_dlg_handle_t *dlg_handle = (we_dlg_handle_t*) handle;
  we_dlg_op_t     *op = NULL;
  
  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }

  op = find_op_by_id (dlg_handle, wid);  
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  if (op->status == WE_DLG_RETURN_NOT_AVAILABLE) {
    return WE_PACKAGE_BUSY;
  }

  if (op->type == WE_DLG_TYPE_OPEN) {
    result->type = WE_DLG_TYPE_OPEN;
    result->_u.data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dlg_result_open_t);
    ((we_dlg_result_open_t*)result->_u.data)->file_name = NULL;
    ((we_dlg_result_open_t*)result->_u.data)->result = op->status;
    if (op->status == WE_DLG_RETURN_OK) {
      char *return_file_name;

      return_file_name = WE_MEM_ALLOC (dlg_handle->modid, 
                                        strlen(op->file_data->file_name) + 1);
      strcpy(return_file_name, op->file_data->file_name);
      ((we_dlg_result_open_t*)result->_u.data)->file_name = return_file_name;
    }
    free_dlg_op(dlg_handle, op);
    return WE_PACKAGE_COMPLETED;
  }
  else if (op->type == WE_DLG_TYPE_SAVE_AS)
  {
    result->type = WE_DLG_TYPE_SAVE_AS;
    result->_u.data = WE_MEM_ALLOCTYPE(dlg_handle->modid, we_dlg_result_save_as_t);
    ((we_dlg_result_save_as_t*)result->_u.data)->file_name = NULL;
    ((we_dlg_result_save_as_t*)result->_u.data)->result = op->status;
    if (op->status == WE_DLG_RETURN_OK) {
      char *return_file_name;

      return_file_name = WE_MEM_ALLOC(dlg_handle->modid, strlen(op->file_data->file_name) + 1);
      strcpy(return_file_name, op->file_data->file_name);
      ((we_dlg_result_save_as_t*)result->_u.data)->file_name = return_file_name;
    }
    free_dlg_op(dlg_handle, op);
    return WE_PACKAGE_COMPLETED;
  }
  
  else if (op->state == WE_DLG_STATE_SAVE_AS_MSG_DLG) {
    result->type = WE_DLG_TYPE_SAVE_AS;
    result->_u.data = WE_MEM_ALLOCTYPE(dlg_handle->modid, we_dlg_result_save_as_t);
    ((we_dlg_result_save_as_t*)result->_u.data)->file_name = NULL;
    ((we_dlg_result_save_as_t*)result->_u.data)->result = op->status;
    return WE_PACKAGE_COMPLETED;
  }
  else if (op->type == WE_DLG_TYPE_MESSAGE) {
    result->type = WE_DLG_TYPE_MESSAGE;
    result->_u.data = WE_MEM_ALLOCTYPE(dlg_handle->modid, we_dlg_result_message_t);
    ((we_dlg_result_message_t *)result->_u.data)->result = op->status;
    free_dlg_op(dlg_handle, op);
    return WE_PACKAGE_COMPLETED;
  }
  else if (op->type == WE_DLG_TYPE_CONFIRM) {
    result->type = WE_DLG_TYPE_CONFIRM;
    result->_u.data = WE_MEM_ALLOCTYPE(dlg_handle->modid, we_dlg_result_confirm_t);
    ((we_dlg_result_confirm_t *)result->_u.data)->result = op->status;
    free_dlg_op(dlg_handle, op);
    return WE_PACKAGE_COMPLETED;
  }
  else if (op->type == WE_DLG_TYPE_PROGRESSBAR) {
    result->type = WE_DLG_TYPE_PROGRESSBAR;
    result->_u.data = WE_MEM_ALLOCTYPE(dlg_handle->modid, we_dlg_result_progress_t);
    ((we_dlg_result_progress_t *)result->_u.data)->result = op->status;
    free_dlg_op(dlg_handle, op);
    return WE_PACKAGE_COMPLETED;    
  }
  else if (op->type == WE_DLG_TYPE_PROMPT) {
    we_dprc_prompt_result_t  prompt_result;
    we_dlg_result_prompt_t  *dlg_result;
    int i;
    
    we_dprc_get_response (op->dialog->handle, &prompt_result);

    result->type = WE_DLG_TYPE_PROMPT;  
    result->_u.data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dlg_result_prompt_t);
    dlg_result = result->_u.data;

    dlg_result->result = op->status;
    dlg_result->nbr_of_input_fields = prompt_result.num_of_strings;  
    dlg_result->input_fields = WE_MEM_ALLOC (dlg_handle->modid, 
                                              sizeof(char*) * 
                                              prompt_result.num_of_strings);
    for (i = 0; i < prompt_result.num_of_strings;i++) {                                                                     
      int length = WE_WIDGET_STRING_GET_LENGTH (prompt_result.string_list[i],
                              /*length in bytes*/TRUE, WeUtf8);

      dlg_result->input_fields[i] = WE_MEM_ALLOC (dlg_handle->modid, length);
      
      WE_WIDGET_STRING_GET_DATA (prompt_result.string_list[i],
                                  dlg_result->input_fields[i],
                                  WeUtf8);
    }
    WE_MEM_FREE (dlg_handle->modid, prompt_result.string_list);
    free_dlg_op(dlg_handle, op);
    return WE_PACKAGE_COMPLETED;
  }
  else if (op->type == WE_DLG_TYPE_LOGIN) {
    int i = 0;
    int length = 0;
    we_dlg_result_login_t  *dlg_login_result = NULL;
    we_dprc_login_result_t login_result;

    we_dprc_get_response (op->dialog->handle, &login_result);

    result->type = WE_DLG_TYPE_LOGIN;
    
    result->_u.data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dlg_result_login_t);
    dlg_login_result = ((we_dlg_result_login_t *)result->_u.data);
    dlg_login_result->result = op->status;
    dlg_login_result->nbr_of_select_value = login_result.nbr_of_select_value;

    length = WE_WIDGET_STRING_GET_LENGTH (login_result.string_list[0], TRUE, WeUtf8);
    dlg_login_result->username = WE_MEM_ALLOC (dlg_handle->modid, length);      
    WE_WIDGET_STRING_GET_DATA (login_result.string_list[0],
                                dlg_login_result->username,
                                WeUtf8);

    length = WE_WIDGET_STRING_GET_LENGTH (login_result.string_list[1], TRUE, WeUtf8);
    dlg_login_result->password = WE_MEM_ALLOC (dlg_handle->modid, length);      
    WE_WIDGET_STRING_GET_DATA (login_result.string_list[1],
                                dlg_login_result->password,
                                WeUtf8);

    dlg_login_result->select_values = WE_MEM_ALLOC (dlg_handle->modid, 
                                                     sizeof(int) * 
                                                     login_result.nbr_of_select_value);

    for (i = 0; i < login_result.nbr_of_select_value; i++) {
      dlg_login_result->select_values[i] = login_result.select_value[i];
    }

    WE_MEM_FREE (dlg_handle->modid, login_result.select_value);
    free_dlg_op(dlg_handle, op);
    return WE_PACKAGE_COMPLETED;
  }
  return WE_PACKAGE_ERROR;
}


int
we_dlg_result_free(we_pck_handle_t* handle, we_pck_result_t* result)
{
  we_dlg_handle_t* dlg_handle = (we_dlg_handle_t*) handle;
  
  if (result == NULL || dlg_handle == NULL) {
    return FALSE;
  }
  
  if (result->_u.data) {
    switch (result->type) {
    case WE_DLG_TYPE_OPEN:
      if (((we_dlg_result_open_t*)result->_u.data)->file_name) {
        WE_MEM_FREE (dlg_handle->modid,
                      ((we_dlg_result_open_t*)result->_u.data)->file_name);
      }
      WE_MEM_FREE (dlg_handle->modid ,((we_dlg_result_open_t*)result->_u.data));
      break;
    case WE_DLG_TYPE_SAVE_AS:
      if (((we_dlg_result_save_as_t*)result->_u.data)->file_name) {
        WE_MEM_FREE (dlg_handle->modid,
                      ((we_dlg_result_save_as_t*)result->_u.data)->file_name);
      }
      WE_MEM_FREE (dlg_handle->modid, result->_u.data);
      break;
    case WE_DLG_TYPE_MESSAGE:
      WE_MEM_FREE(dlg_handle->modid, result->_u.data);
      break;
    case WE_DLG_TYPE_CONFIRM:
      WE_MEM_FREE(dlg_handle->modid, result->_u.data);
      break;
    case WE_DLG_TYPE_PROGRESSBAR:
      WE_MEM_FREE(dlg_handle->modid, result->_u.data);
      break;
    case WE_DLG_TYPE_PROMPT:
      {
        we_dlg_result_prompt_t *dlg_result = NULL;
        int i = 0;
        dlg_result = result->_u.data;
        for (i = 0; i < (int)dlg_result->nbr_of_input_fields; i++) {
          if (dlg_result->input_fields[i]) {
            WE_MEM_FREE( dlg_handle->modid, dlg_result->input_fields[i]);
          }
        }
        if (dlg_result->input_fields) {
          WE_MEM_FREE( dlg_handle->modid ,dlg_result->input_fields);
        }
        WE_MEM_FREE( dlg_handle->modid ,dlg_result);
      }
      break;
    case WE_DLG_TYPE_LOGIN:
      if (((we_dlg_result_login_t*)result->_u.data)->password) {
        WE_MEM_FREE( dlg_handle->modid , ((we_dlg_result_login_t*)result->_u.data)->password);
      }
      if (((we_dlg_result_login_t*)result->_u.data)->username) {
        WE_MEM_FREE( dlg_handle->modid , ((we_dlg_result_login_t*)result->_u.data)->username);
      }
      if (((we_dlg_result_login_t*)result->_u.data)->select_values) {
        WE_MEM_FREE( dlg_handle->modid , ((we_dlg_result_login_t*)result->_u.data)->select_values);
      }
      WE_MEM_FREE( dlg_handle->modid , ((we_dlg_result_login_t*)result->_u.data));
      break;
    default:
      break;
    }
  }
  return TRUE;
}

/*
 * Runs the DLG package. When the module receives a signal that is
 * defined by WE, the module must call this function because the
 * DLG package might be the receiver.
 *
 *
 * Returns:
 *    WE_PACKAGE_SIGNAL_NOT_HANDLED
 *    WE_PACKAGE_SIGNAL_HANDLED
 *    WE_PACKAGE_OPERATION_COMPLETE
 */
int
we_dlg_handle_signal (we_pck_handle_t *handle,
                       WE_UINT16        signal,
                       void             *p,
                       WE_INT32        *wid)
{
  we_dlg_handle_t* dlg_handle = (we_dlg_handle_t*) handle;
  WE_INT32 afi_id = 0;
  int afi_return = 0;
  int dispatched = 0;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;
  }

  afi_return = we_afi_handle_signal (dlg_handle->afi_handle, signal, p, &afi_id);
  if (afi_return == WE_PACKAGE_SIGNAL_HANDLED) {
    we_dlg_op_t *op = dlg_handle->op;
    
    while (op && op->file_data && 
           op->file_data->afi_id != afi_id) {
      op = op->next;
    }
    if (op) { 
      *wid = op->wid;
    }
    return WE_PACKAGE_SIGNAL_HANDLED;
  }
  else if (afi_return == WE_PACKAGE_OPERATION_COMPLETE) { 
    we_dlg_op_t *op = dlg_handle->op;
    we_pck_result_t afi_result;
          
    while (op && op->file_data && 
           op->file_data->afi_id != afi_id) {
      op = op->next;
    }
    if (op == NULL) {
      return WE_PACKAGE_OPERATION_COMPLETE; 
    }
    *wid = op->wid;

    we_afi_get_result(dlg_handle->afi_handle, afi_id, &afi_result);
    
    if (op->state == WE_DLG_STATE_OPEN_NAVIGATE_DELAYED) {
      we_afi_result_dir_entries_t *result = NULL;
      result = (we_afi_result_dir_entries_t*)afi_result._u.data;
      op->state = we_dlg_update_open_file_dlg (dlg_handle, op, result);
        /* Do not free afi_result as it will be used and freed later */
      return WE_PACKAGE_SIGNAL_HANDLED;
    }
    else if (op->state == WE_DLG_STATE_SAVE_AS_NAVIGATE_DELAYED) {
      we_afi_result_dir_entries_t *result = NULL;
      result = (we_afi_result_dir_entries_t*)afi_result._u.data;
      op->state = we_dlg_update_save_file_dlg (handle, op, result);
      /* Do not free afi_result as it will be used and freed later */
      return WE_PACKAGE_SIGNAL_HANDLED;
    }
    else if (op->state == WE_DLG_STATE_SAVE_AS_FILE_EXISTS_DELAYED) {
      if ((WE_INT32)(afi_result._u.data) != WE_AFI_ERROR_PATH) {
        create_exists_dialog(dlg_handle, op, op->file_data->entered_name);
      }
      else {
        char* temp_file_name;
        we_afi_result_free(dlg_handle->afi_handle, &afi_result);
        temp_file_name = WE_MEM_ALLOC (dlg_handle->modid, 
                                        strlen(op->file_data->file_name) + 
                                        strlen(op->file_data->entered_name) + 1);
        strcpy(temp_file_name, op->file_data->file_name);
        strcat(temp_file_name, op->file_data->entered_name);
        WE_MEM_FREE(dlg_handle->modid, op->file_data->file_name);
        op->file_data->file_name = temp_file_name;
        op->status = WE_DLG_RETURN_OK;
        op->state = WE_DLG_STATE_SAVE_AS_FINISHED;
        return WE_PACKAGE_OPERATION_COMPLETE;
      }
    }
    else if (op->state == WE_DLG_STATE_SAVE_AS_EXISTS_DELETE_DELAYED) {
      if (afi_result._u.i_val == WE_AFI_OK) {
        op->status = WE_DLG_RETURN_OK;
        we_afi_result_free (dlg_handle->afi_handle, &afi_result);
        op->state = WE_DLG_STATE_SAVE_AS_FINISHED;        
        return WE_PACKAGE_OPERATION_COMPLETE;
      }
      we_dlg_handle_afi_error (handle, op, &afi_result);
      we_afi_result_free (dlg_handle->afi_handle, &afi_result);
      return WE_PACKAGE_SIGNAL_HANDLED;
    }
    else if (op->state == WE_DLG_STATE_NEW_DIR_AFI_DELAYED){
      if (afi_result._u.i_val == WE_AFI_OK){
        op->state = we_dlg_update_save_window (dlg_handle, op, 
                                                op->file_data->mime_types, 
                                                op->file_data->file_name);
      }
      else {
        we_dlg_handle_afi_error (handle, op, &afi_result);
      }
    }
    we_afi_result_free(dlg_handle->afi_handle, &afi_result);
    return WE_PACKAGE_SIGNAL_HANDLED;
  }

  switch(signal) {
  case WE_SIG_WIDGET_ACTION:
    {
      we_widget_action_t* action = (we_widget_action_t*)p;
      WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, dlg_handle->modid,
                  "DLG: Received WE_SIG_WIDGET_ACTION\n"));
      dispatched = we_dlg_handle_widget_action (dlg_handle, wid, 
                                                 action->handle, action->action);
    }
    break;
  case WE_SIG_GET_ICONS_RESPONSE:
    {
      frw_get_icons_resp_t *z;
      we_dlg_op_t *op; 

      z = (frw_get_icons_resp_t*)p;
      op = find_op_by_id (dlg_handle, z->wid);
      
      if(op == NULL) {
        return WE_PACKAGE_SIGNAL_NOT_HANDLED;
      }

      if (dlg_handle->icons != NULL) {
        dispatched = WE_PACKAGE_SIGNAL_HANDLED;
      }
      dlg_handle->icons = z->icon_list;
      dlg_handle->n_items = z->n_items;
      
      z->icon_list = NULL;
      z->n_items   = 0;

      if (op->state == WE_DLG_STATE_SAVE_AS_NAVIGATE_DELAYED) {
      op->state = we_dlg_update_save_window (dlg_handle, op, 
                                              op->file_data->mime_types, 
                                              op->file_data->file_name);
      }
      else if (op->state == WE_DLG_STATE_OPEN_NAVIGATE_DELAYED) {
        op->state = we_dlg_update_open_window (dlg_handle, op, 
                                                op->file_data->mime_types, 
                                                op->file_data->file_name);
      }
      *wid = op->wid;
      dispatched = WE_PACKAGE_SIGNAL_HANDLED;
    }
    break;

  case WE_SIG_WIDGET_USEREVT:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, dlg_handle->modid,
                "DLG: Received WE_SIG_WIDGET_USEREVT\n"));
    dispatched = we_dlg_handle_user_evt (dlg_handle, wid, ((we_widget_userevt_t*)p)->handle,
                                          ((we_widget_userevt_t*)p)->event);
    break;  
  case WE_SIG_TIMER_EXPIRED:
      
      if (dlg_handle->op && dlg_handle->op->type == WE_DLG_TYPE_PROGRESSBAR) {
        dispatched = we_dlg_handle_progress_timer_expired (
          dlg_handle, ((we_timer_expired_t *)p)->timerID, wid);
      }
      else {
        WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, dlg_handle->modid,
                     "DLG: Received WE_SIG_TIMER_EXPIRED\n"));
        dispatched = we_dlg_handle_timer_expired (
          dlg_handle, wid, ((we_timer_expired_t *)p)->timerID);
      }
    break;
  default:
    dispatched = WE_PACKAGE_SIGNAL_NOT_HANDLED;
    break;
  }
  return dispatched;
}


/*
 * Set private data connected to a specific DLG identifier. The private data
 * might be used by the module to indicate which sub module that was
 * invoked the operation. 
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_dlg_set_private_data (we_pck_handle_t *handle, WE_INT32 wid, void *p)
{
  we_dlg_handle_t* dlg_handle = (we_dlg_handle_t*) handle;
  we_dlg_op_t *op = find_op_by_id (dlg_handle, wid);
  if (op == NULL) {
    return FALSE;
  }
  op->priv_data = p;
  return TRUE;
}

/*
 * Get private data connected to a specific identifier. The private data
 * might be used by the module to indicate where to continue execution.
 * This function may be called after we_afi_run has returned 
 * WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns NULL.
 */
void*
we_dlg_get_private_data (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_dlg_handle_t* dlg_handle = (we_dlg_handle_t*) handle;
  we_dlg_op_t *op = find_op_by_id (dlg_handle, wid);
  if (op == NULL) {
    return NULL;
  }
  return op->priv_data;
}

int
we_dlg_terminate (we_pck_handle_t* handle)
{
  we_dlg_handle_t* dlg_handle = (we_dlg_handle_t*) handle;
  we_dcvt_t cvt_obj;
    
  if(dlg_handle == NULL) {
    return FALSE;
  }

  while (dlg_handle->op) {
    free_dlg_op(dlg_handle, dlg_handle->op);
  }

  we_afi_terminate(dlg_handle->afi_handle);

  we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, dlg_handle->modid);
  we_dcvt_array (&cvt_obj, sizeof (we_icon_list_t), dlg_handle->n_items,
                  (void **)&(dlg_handle->icons), 
                  (we_dcvt_element_t *)we_cvt_icon_list);
  
  WE_MEM_FREE(dlg_handle->modid, dlg_handle);
  
  return TRUE;
}

int
we_dlg_delete (we_pck_handle_t* handle, WE_INT32 wid)
{
  we_dlg_handle_t* dlg_handle = (we_dlg_handle_t*) handle;
  we_dlg_op_t *op = find_op_by_id (dlg_handle, wid);
  
  if (op == NULL) {
    return FALSE;
  }
  free_dlg_op(dlg_handle, op);
  return TRUE;
}

/*
 * Create a save as dialog.
 *
 * Parameters:
 *  screen                The screen that should be used for the dialog.
 *  file_name             Suggested user friendly file name of the file.
 *  mime_type             The mime type of file to save.
 *  bit_flag              WE_DLG_ALLOW_OVERWRITE   -  if bit is set, the package will prompt
 *                          the user about the replace and possibly replace the existing file.
 *                        FRW_DLG_NO_EXTERNAL_MEMORY - If this bit is set no folders on
 *                          external memory will be possible to select as save destination.
 * 
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */
WE_INT32
we_dlg_create_save_as (const we_dlg_save_as_t* save_as_data)
{
  we_dlg_handle_t       *dlg_handle = NULL;
  we_dlg_op_file_t      *file_data = NULL;
  we_dlg_op_t           *op;
  we_pck_root_folders_t *root_folders;
  we_dprc_file_t         file_dlg_data;
  we_dlg_dialog_t       *dialog = NULL;

  if (save_as_data == NULL) {
    return  WE_PACKAGE_ERROR;
  }
  
  dlg_handle = (we_dlg_handle_t*) save_as_data->dlg_data.handle;

  if ((save_as_data->mime_type == NULL) || 
      (dlg_handle == NULL)) {
    return WE_PACKAGE_ERROR;
  }

  op = get_new_op (dlg_handle, WE_DLG_TYPE_SAVE_AS);
  op->file_data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dlg_op_file_t);
  file_data = op->file_data;

  file_data->dir_entries = NULL;
  file_data->permission = 0;
  file_data->mime_types = NULL;
  file_data->file_name = NULL;
  file_data->default_name = NULL;
  file_data->entered_name = NULL;
  file_data->afi_id = -1;
  file_data->preview_state = WE_DLG_PREVIEW_STATE_IDLE;
  file_data->current_timer_id = 0;
  file_data->preview_image_handle = 0;
  file_data->nbr_items_in_file_dlg = 0;
  file_data->is_open_window = FALSE;

  op->screen = save_as_data->dlg_data.screen;

  file_data->default_name = we_cmmn_strdup (dlg_handle->modid, 
                                             save_as_data->file_name);
  file_data->save_here_str = save_as_data->save_here_str;
  file_data->bit_flag = save_as_data->bit_flag;
  file_data->mime_types = we_cmmn_strdup (dlg_handle->modid, 
                                           save_as_data->mime_type);
  
  we_pck_get_urf_from_mime (dlg_handle->modid, save_as_data->mime_type,  
                             &root_folders);

  if (root_folders->n_rf == 0) {
    create_cannot_save_dialog(dlg_handle, op);
    WE_MEM_FREE(dlg_handle->modid, root_folders);
    return op->wid;
  }

  if (root_folders->n_rf == 1 &&
      !(((op->file_data->bit_flag & FRW_DLG_NO_EXTERNAL_MEMORY) > 0) && 
        (root_folders->rf[0].external == 1))) {

    op->file_data->file_name = we_cmmn_strdup (dlg_handle->modid, root_folders->rf[0].pathname);
  }

  op->state = WE_DLG_STATE_SAVE_AS_NAVIGATE; 

  dialog = get_new_dialog (dlg_handle, op);
  file_dlg_data.cancel_string  = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_BACK);
  file_dlg_data.ok_string = 0;
  file_dlg_data.items = NULL;
  file_dlg_data.nbr_of_items = 0;
  file_dlg_data.pakage_type = dlg_handle->dlg_pkg_type;
  file_dlg_data.type = WeExclusiveChoice;
  file_dlg_data.is_open_dialog = FALSE;

  if (save_as_data->dlg_data.title) {
    file_dlg_data.title = save_as_data->dlg_data.title;
    op->title = save_as_data->dlg_data.title;
  }
  else {
    file_dlg_data.title = WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_TEXT_ROOT_FOLDER);
    op->title = 0;
  }

  dialog->handle = we_dprc_create (dlg_handle->modid, op->screen, WE_DLG_FILE, &file_dlg_data);

  op->state = we_dlg_update_save_window (dlg_handle, op,
                                          op->file_data->mime_types,
                                          op->file_data->file_name);

  WE_MEM_FREE(dlg_handle->modid, root_folders);
  return op->wid;
}

/*
 * Create an open dialog.
 *
 * Open parameters:
 *  screen        The screen that should be used for the dialog.
 *  permission    Do not show files when this permission(s) is not valid.
 *                E.g. if the caller will send forward the selected
 *                content, any DRM protected content must not be showed
 *                to the end user.
 *  mime_types    Which type of files that will be shown to the user.
 *                Wildcards is allowed in this parameter.
 * 
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */

WE_INT32
we_dlg_create_open (const we_dlg_open_t* open_data)
{
  we_dprc_file_t         file_dlg_data;
  we_pck_root_folders_t *root_folders;
  we_dlg_handle_t       *dlg_handle;
  we_dlg_op_file_t      *file_data = NULL;
  we_dlg_op_t           *op;
  we_dlg_dialog_t       *dialog = NULL;
  
  if (open_data == NULL) {
    return FALSE;
  }
  dlg_handle = (we_dlg_handle_t*) open_data->dlg_data.handle;
  if (dlg_handle == NULL) {
    return FALSE;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_OPEN);
  op->file_data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dlg_op_file_t);
  file_data = op->file_data;
  file_data->dir_entries = NULL;
  file_data->permission = 0;
  file_data->mime_types = NULL;
  file_data->file_name = NULL;
  file_data->default_name = NULL;
  file_data->entered_name = NULL;
  file_data->afi_id = -1;
  file_data->preview_state = WE_DLG_PREVIEW_STATE_IDLE;
  file_data->current_timer_id = 0;
  file_data->preview_image_handle = 0;
  file_data->save_here_str = 0;
  file_data->permission = open_data->permission;
  file_data->nbr_items_in_file_dlg = 0;
  file_data->is_open_window = TRUE;

  op->state = WE_DLG_STATE_OPEN_NAVIGATE; 
  file_data->mime_types = WE_MEM_ALLOC (dlg_handle->modid, 
                                         strlen(open_data->mime_types) + 1);
  

  op->screen = open_data->dlg_data.screen;
  strcpy(op->file_data->mime_types, open_data->mime_types);
  
  we_pck_get_urf_from_mime (dlg_handle->modid, open_data->mime_types,  &root_folders);
  if (root_folders->n_rf == 1) {
    op->file_data->file_name = we_cmmn_strdup (dlg_handle->modid,
                                                root_folders->rf[0].pathname);
  }
  WE_MEM_FREE(dlg_handle->modid, root_folders);

  dialog = get_new_dialog(dlg_handle, op);
  file_dlg_data.cancel_string  = WE_WIDGET_STRING_GET_PREDEFINED (
    WE_STR_ID_DLG_ACTION_BACK);
  file_dlg_data.ok_string = 0;
  file_dlg_data.items = NULL;
  file_dlg_data.nbr_of_items = 0;
  file_dlg_data.pakage_type = dlg_handle->dlg_pkg_type;
  file_dlg_data.type = WeExclusiveChoice;
  file_dlg_data.is_open_dialog = TRUE;

  if (open_data->dlg_data.title) {
    file_dlg_data.title = open_data->dlg_data.title;
    op->title = open_data->dlg_data.title;
  }
  else {
    file_dlg_data.title = WE_WIDGET_STRING_GET_PREDEFINED (
      WE_STR_ID_DLG_TEXT_ROOT_FOLDER);
    op->title = 0;
  }

  dialog->handle = we_dprc_create (dlg_handle->modid, op->screen, WE_DLG_FILE, &file_dlg_data);

  we_dprc_event_subscribe (dialog->handle, WeKey_Up, 1);
  we_dprc_event_subscribe (dialog->handle, WeKey_Down, 1);

  op->state = we_dlg_update_open_window (dlg_handle, op, 
                                          op->file_data->mime_types,
                                          op->file_data->file_name);
  return op->wid;
}

/**********************************************************************
 * Message dialog
 **********************************************************************/
WE_INT32 
we_dlg_create_multi_message (const we_dlg_message_t* message_data)
{
  we_dlg_handle_t *dlg_handle = NULL;
  we_dlg_op_t *op = NULL;
  we_dlg_dialog_t *dialog = NULL;
  we_dprc_message_t dprc_message_data;

  if (message_data == NULL) {
    return WE_PACKAGE_ERROR;
  }
  dlg_handle = (we_dlg_handle_t *) message_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_MESSAGE);
  dialog = get_new_dialog(dlg_handle, op);

  dprc_message_data.numStrId = message_data->nbr_of_str_ids;
  dprc_message_data.str_ids = message_data->str_ids;
  dprc_message_data.title = message_data->dlg_data.title;
  dprc_message_data.singel_dlg = FALSE;
  dprc_message_data.pakage_type = dlg_handle->dlg_pkg_type;
  dprc_message_data.ok_string = message_data->ok_string;

  op->screen = message_data->dlg_data.screen;
  op->state = WE_DLG_STATE_MESSAGE_DIALOG; 
  op->result_callback = NULL;
  switch (message_data->dialog_type)
  {
  case we_dlg_alert:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen,
                                      WE_DLG_MESSAGE_ALERT, &dprc_message_data);
    break;
  case we_dlg_warning:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen, 
                                      WE_DLG_MESSAGE_WARNING, &dprc_message_data);
    break;
  case we_dlg_info:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen, 
                                      WE_DLG_MESSAGE_INFO, &dprc_message_data);
    break;
  case we_dlg_error:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen, 
                                      WE_DLG_MESSAGE_ERROR, &dprc_message_data);
    break;
  default:
    return WE_PACKAGE_ERROR;
    break;
  }
  return op->wid;
}

WE_INT32 
we_dlg_create_message (const we_dlg_message_t* message_data)
{
  we_dlg_handle_t *dlg_handle = NULL;
  we_dlg_op_t *op = NULL;
  we_dlg_dialog_t *dialog = NULL;
  we_dprc_message_t dprc_message_data;

  if (message_data  == NULL || 
      message_data->nbr_of_str_ids != 1) {
    return WE_PACKAGE_ERROR;
  }
  dlg_handle = (we_dlg_handle_t *) message_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_MESSAGE);
  dialog = get_new_dialog(dlg_handle, op);
  
  dprc_message_data.numStrId = message_data->nbr_of_str_ids;
  dprc_message_data.str_ids = message_data->str_ids;
  dprc_message_data.title = message_data->dlg_data.title;
  dprc_message_data.singel_dlg = TRUE;
  dprc_message_data.pakage_type = dlg_handle->dlg_pkg_type;
  dprc_message_data.ok_string = message_data->ok_string;

  op->screen = message_data->dlg_data.screen;
  op->state = WE_DLG_STATE_MESSAGE_DIALOG; 
  op->result_callback = NULL;
  
  switch (message_data->dialog_type)
  {
  case we_dlg_alert:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen, 
                                      WE_DLG_MESSAGE_ALERT, &dprc_message_data);
    break;
  case we_dlg_warning:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen, 
                                      WE_DLG_MESSAGE_WARNING, &dprc_message_data);
    break;
  case we_dlg_info:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen, 
                                      WE_DLG_MESSAGE_INFO, &dprc_message_data);
    break;
  case we_dlg_error:
    dialog->handle = we_dprc_create (dlg_handle->modid, message_data->dlg_data.screen, 
                                      WE_DLG_MESSAGE_ERROR, &dprc_message_data);
    break;
  default:
    return WE_PACKAGE_ERROR;
    break;
  }
  return op->wid;
}

/**********************************************************************
 * Confirm dialog
 **********************************************************************/

/*
 * Create a confirm dialog.
 *
 * Confirm parameters:
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */
WE_INT32
we_dlg_create_multi_confirm (const we_dlg_confirm_t* confirm_data)
{
  we_dlg_handle_t *dlg_handle = NULL;
  we_dlg_op_t *op = NULL;
  we_dlg_dialog_t *dialog = NULL;
  we_dprc_confirm_t dprc_confirm_data;

  if (confirm_data == NULL) {
    return WE_PACKAGE_ERROR;
  }
  dlg_handle = (we_dlg_handle_t *) confirm_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_CONFIRM);
  dialog = get_new_dialog(dlg_handle, op);
  
  dprc_confirm_data.title = confirm_data->dlg_data.title;

  dprc_confirm_data.numStrId = confirm_data->nbr_of_str_ids;
  dprc_confirm_data.str_ids = confirm_data->str_ids;
  dprc_confirm_data.singel_dlg = FALSE;
  dprc_confirm_data.ok_string = confirm_data->ok_string;
  dprc_confirm_data.cancel_string = confirm_data->cancel_string;
  dprc_confirm_data.pakage_type = dlg_handle->dlg_pkg_type;


  op->screen = confirm_data->dlg_data.screen;
  op->state = WE_DLG_STATE_CONFIRM_DIALOG; 
  op->result_callback = NULL;
  
  dialog->handle = we_dprc_create (dlg_handle->modid, confirm_data->dlg_data.screen,
                                    WE_DLG_CONFIRM, &dprc_confirm_data);
 
 return op->wid;
}

WE_INT32
we_dlg_create_confirm (const we_dlg_confirm_t* confirm_data)
{
  we_dlg_handle_t *dlg_handle = NULL;
  we_dlg_op_t *op = NULL;
  we_dlg_dialog_t *dialog = NULL;
  we_dprc_confirm_t dprc_confirm_data;

  if (confirm_data == NULL || 
      confirm_data->nbr_of_str_ids != 1) {
    return WE_PACKAGE_ERROR;
  }
  dlg_handle = (we_dlg_handle_t *) confirm_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }

  op = get_new_op (dlg_handle, WE_DLG_TYPE_CONFIRM);
  dialog = get_new_dialog(dlg_handle, op);

  dprc_confirm_data.title = confirm_data->dlg_data.title;

  dprc_confirm_data.numStrId = 1;
  dprc_confirm_data.str_ids = confirm_data->str_ids;
  dprc_confirm_data.singel_dlg = TRUE;
  dprc_confirm_data.ok_string = confirm_data->ok_string;
  dprc_confirm_data.cancel_string = confirm_data->cancel_string;
  dprc_confirm_data.pakage_type = dlg_handle->dlg_pkg_type;

  op->screen = confirm_data->dlg_data.screen;
  op->state = WE_DLG_STATE_CONFIRM_DIALOG; 
  op->result_callback = NULL;
  
  dialog->handle = we_dprc_create (dlg_handle->modid, confirm_data->dlg_data.screen,
                                    WE_DLG_CONFIRM, &dprc_confirm_data);
  
  return op->wid;
}

/**********************************************************************
 * Prompt dialog
 **********************************************************************/
WE_INT32
we_dlg_create_prompt (const we_dlg_prompt_t* prompt_data)
{
  we_dlg_handle_t  *dlg_handle = NULL;
  we_dlg_op_t      *op = NULL;
  we_dlg_dialog_t  *dialog;
  we_dprc_prompt_t dprc_prompt_data;

  if (prompt_data == NULL || 
      prompt_data->nbr_of_input_fields != 1) {
    return WE_PACKAGE_ERROR;
  }

  dlg_handle = (we_dlg_handle_t *) prompt_data->dlg_data.handle;
  
  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle,WE_DLG_TYPE_PROMPT);
  dialog = get_new_dialog (dlg_handle, op);

  dprc_prompt_data.title = prompt_data->dlg_data.title;
  dprc_prompt_data.message = prompt_data->message;
  dprc_prompt_data.nbr_of_input_fields = 1;

  dprc_prompt_data.input_fields = WE_MEM_ALLOCTYPE (dlg_handle->modid, 
                                                     we_dprc_input_field_t);

  dprc_prompt_data.input_fields->max_size = prompt_data->input_fields->max_size;
  dprc_prompt_data.input_fields->label = prompt_data->input_fields->label;
  dprc_prompt_data.input_fields->default_text = prompt_data->input_fields->default_text;
  dprc_prompt_data.input_fields->type = prompt_data->input_fields->type;
  dprc_prompt_data.singel_dlg = TRUE;
  dprc_prompt_data.pakage_type = dlg_handle->dlg_pkg_type;
  dprc_prompt_data.ok_string = prompt_data->ok_string;
  dprc_prompt_data.cancel_string = prompt_data->cancel_string;

  op->screen = prompt_data->dlg_data.screen;
  op->state = WE_DLG_STATE_PROMPT_DIALOG;   
  op->result_callback = NULL;

  dialog->handle = we_dprc_create (dlg_handle->modid, prompt_data->dlg_data.screen,
                                    WE_DLG_PROMPT, &dprc_prompt_data);

  WE_MEM_FREE (dlg_handle->modid, dprc_prompt_data.input_fields);
  return op->wid;
}

WE_INT32
we_dlg_create_multi_prompt(const we_dlg_prompt_t* prompt_data)
{
  we_dlg_handle_t  *dlg_handle = NULL;
  we_dlg_op_t      *op = NULL;
  we_dlg_dialog_t  *dialog; 
  we_dprc_prompt_t dprc_prompt_data;
  int i;

  if (prompt_data == NULL) {
    return WE_PACKAGE_ERROR;
  }

  dlg_handle = (we_dlg_handle_t *) prompt_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_PROMPT);
  dialog = get_new_dialog (dlg_handle, op);

  dprc_prompt_data.nbr_of_input_fields = prompt_data->nbr_of_input_fields; 
  dprc_prompt_data.input_fields = WE_MEM_ALLOC (dlg_handle->modid, 
                                                 sizeof(we_dprc_input_field_t) * 
                                                 dprc_prompt_data.nbr_of_input_fields);

  for (i = 0; i < prompt_data->nbr_of_input_fields; i++) {
    dprc_prompt_data.input_fields[i].max_size = prompt_data->input_fields[i].max_size;
    dprc_prompt_data.input_fields[i].label = prompt_data->input_fields[i].label;
    dprc_prompt_data.input_fields[i].default_text = prompt_data->input_fields[i].default_text;
    dprc_prompt_data.input_fields[i].type = prompt_data->input_fields[i].type;
  }
  dprc_prompt_data.title = prompt_data->dlg_data.title;
  dprc_prompt_data.message = prompt_data->message;
  dprc_prompt_data.singel_dlg = FALSE;
  dprc_prompt_data.pakage_type = dlg_handle->dlg_pkg_type;
  dprc_prompt_data.ok_string = prompt_data->ok_string;
  dprc_prompt_data.cancel_string = prompt_data->cancel_string;

  op->screen = prompt_data->dlg_data.screen;
  op->state = WE_DLG_STATE_PROMPT_DIALOG;   
  op->result_callback = NULL;

  dialog->handle = we_dprc_create (dlg_handle->modid, prompt_data->dlg_data.screen,
                                    WE_DLG_PROMPT, &dprc_prompt_data);

  WE_MEM_FREE (dlg_handle->modid, dprc_prompt_data.input_fields);

  return op->wid;
}

/**********************************************************************
 * progress bar
 **********************************************************************/

/*
 * Create a progress bar.
 *
 * parameters:
 * progress_data: Data that should be used for the dialog.
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */

WE_INT32
we_dlg_create_progressbar (const we_dlg_progress_bar_t* progress_data)
{
  we_dlg_handle_t        *dlg_handle = NULL;
  we_dlg_op_t            *op = NULL;
  we_dlg_dialog_t        *dialog = NULL;
  we_dprc_progress_bar_t dprc_progress_data;

  if (progress_data == NULL) {
    return WE_PACKAGE_ERROR;
  }

  dlg_handle = (we_dlg_handle_t *) progress_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_PROGRESSBAR);
  dialog = get_new_dialog (dlg_handle, op);

  dprc_progress_data.title = progress_data->dlg_data.title;

  dprc_progress_data.max_value = progress_data->max_value;
  dprc_progress_data.value = progress_data->value;
  dprc_progress_data.text_one = progress_data->text_one;
  dprc_progress_data.text_two = progress_data->text_two;
  dprc_progress_data.propmask = progress_data->propmask;
  dprc_progress_data.cancel_string = progress_data->cancel_string;
  dprc_progress_data.tick_time = 0;

  dprc_progress_data.pakage_type = dlg_handle->dlg_pkg_type;

  op->progress_data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dlg_op_progress_t);
  op->progress_data->data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dprc_progress_bar_t);
  op->progress_data->dialog = dialog;
  op->progress_data->timer_id = 0;

  op->progress_data->data->max_value = dprc_progress_data.max_value;
  op->progress_data->data->title = dprc_progress_data.title;
  op->progress_data->data->value = dprc_progress_data.value;
  op->progress_data->data->tick_time = dprc_progress_data.tick_time;
  op->progress_data->data->propmask = dprc_progress_data.propmask;
  op->progress_data->data->text_one = dprc_progress_data.text_one;
  op->progress_data->data->text_two = dprc_progress_data.text_two;
    

  op->screen = progress_data->dlg_data.screen;
  op->state = WE_DLG_STATE_PROGRESS_DIALOG;   
  op->result_callback = NULL;

  dialog->handle = we_dprc_create (dlg_handle->modid, progress_data->dlg_data.screen, 
                                    WE_DLG_PROGRESS_BAR, &dprc_progress_data);

  return op->wid;
}

WE_INT32
we_dlg_create_loop_progressbar (const we_dlg_progress_bar_t* progress_data,
                                 WE_UINT32 timer_sig, int tick_time)
{
  we_dlg_handle_t *dlg_handle = NULL;
  we_dlg_op_t *op = NULL;
  we_dlg_dialog_t *dialog = NULL;
  we_dprc_progress_bar_t dprc_progress_data;

  if (progress_data == NULL) {
    return WE_PACKAGE_ERROR;
  }

  dlg_handle = (we_dlg_handle_t *) progress_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_PROGRESSBAR);
  dialog = get_new_dialog (dlg_handle, op);

  dprc_progress_data.title = progress_data->dlg_data.title;

  dprc_progress_data.max_value = progress_data->max_value;
  dprc_progress_data.value = progress_data->value;
  dprc_progress_data.tick_time = tick_time;
  dprc_progress_data.pakage_type = dlg_handle->dlg_pkg_type;
  dprc_progress_data.text_one = progress_data->text_one;
  dprc_progress_data.text_two = progress_data->text_two;
  dprc_progress_data.propmask = progress_data->propmask;
  dprc_progress_data.cancel_string = progress_data->cancel_string;

  op->screen = progress_data->dlg_data.screen;
  op->state = WE_DLG_STATE_PROGRESS_DIALOG;   
  op->result_callback = NULL;

  dialog->handle = we_dprc_create (dlg_handle->modid, progress_data->dlg_data.screen, 
                                    WE_DLG_PROGRESS_BAR, &dprc_progress_data);

  op->progress_data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dlg_op_progress_t);
  op->progress_data->data = WE_MEM_ALLOCTYPE (dlg_handle->modid, we_dprc_progress_bar_t);
  op->progress_data->dialog = dialog;
  op->progress_data->timer_id = timer_sig;

  op->progress_data->data->max_value = dprc_progress_data.max_value;
  op->progress_data->data->title = dprc_progress_data.title;
  op->progress_data->data->value = dprc_progress_data.value;
  op->progress_data->data->tick_time = dprc_progress_data.tick_time;
  op->progress_data->data->propmask = dprc_progress_data.propmask;
  op->progress_data->data->text_one = dprc_progress_data.text_one;
  op->progress_data->data->text_two = dprc_progress_data.text_two;
    
  WE_TIMER_SET( dlg_handle->modid, timer_sig, dprc_progress_data.tick_time);
  return op->wid;
}

int
we_dlg_update_progressbar (we_pck_handle_t *handle, WE_UINT32 wid, 
                            const we_dlg_progress_bar_t* progress_data)
{
  we_dprc_progress_bar_t progress_dprc_data;
  we_dlg_handle_t* dlg_handle = (we_dlg_handle_t*) handle;
  we_dlg_op_t *op = NULL;

  if (handle == NULL) {
    return FALSE;
  }

  op = find_op_by_id (dlg_handle, wid);
  if (op == NULL) {
    return FALSE;
  }
  progress_dprc_data.cancel_string = progress_data->cancel_string;
  progress_dprc_data.max_value = progress_data->max_value;
  progress_dprc_data.ok_string = 0;
  progress_dprc_data.pakage_type = ((we_dlg_handle_t *)(handle))->dlg_pkg_type;
  progress_dprc_data.propmask = progress_data->propmask;
  progress_dprc_data.text_one = progress_data->text_one;
  progress_dprc_data.text_two = progress_data->text_two;
  progress_dprc_data.tick_time = 0;
  progress_dprc_data.title = progress_data->dlg_data.title;
  progress_dprc_data.value = progress_data->value;
  return we_dprc_update_progressbar (op->progress_data->dialog->handle, 
                                     &progress_dprc_data);
}


/**********************************************************************
 * login dialog
 **********************************************************************/

WE_INT32 
we_dlg_create_login (const we_dlg_login_t* login_data)
{
  we_dlg_handle_t  *dlg_handle = NULL;
  we_dlg_op_t      *op = NULL;
  we_dlg_dialog_t  *dialog;
  we_dprc_login_t   dprc_login_data;
  int i;

  if (login_data == NULL) {
    return WE_PACKAGE_ERROR;
  }
  dlg_handle = (we_dlg_handle_t *) login_data->dlg_data.handle;

  if (dlg_handle == NULL) {
    return WE_PACKAGE_ERROR;
  }
  op = get_new_op (dlg_handle, WE_DLG_TYPE_LOGIN);
  dialog = get_new_dialog (dlg_handle, op);
  
  dprc_login_data.title = login_data->dlg_data.title;
  dprc_login_data.pakage_type = dlg_handle->dlg_pkg_type;
  dprc_login_data.nbr_of_selection_boxes = login_data->nbr_of_selection_boxes;
  dprc_login_data.selectboxes = WE_MEM_ALLOC (dlg_handle->modid, 
                                sizeof(we_dprc_selectbox_t) * 
                                dprc_login_data.nbr_of_selection_boxes);
   
  for (i = 0; i < dprc_login_data.nbr_of_selection_boxes;i++) {
    dprc_login_data.selectboxes[i].select_value = login_data->selectboxes[i].default_value;
    dprc_login_data.selectboxes[i].select_str_ids = login_data->selectboxes[i].label;
  }
  
  dprc_login_data.input_fields[0].default_text = login_data->username.default_text;
  dprc_login_data.input_fields[0].label = login_data->username.label;
  dprc_login_data.input_fields[0].max_size = login_data->username.max_size;
  dprc_login_data.input_fields[0].type = login_data->username.type;

  dprc_login_data.input_fields[1].default_text = login_data->password.default_text;
  dprc_login_data.input_fields[1].label = login_data->password.label;
  dprc_login_data.input_fields[1].max_size = login_data->password.max_size;
  dprc_login_data.input_fields[1].type = login_data->password.type;

  dprc_login_data.ok_string = login_data->ok_string;
  dprc_login_data.cancel_string = login_data->cancel_string;
  
  op->screen = login_data->dlg_data.screen;
  op->state = WE_DLG_STATE_LOGIN_DIALOG;
  op->result_callback = NULL;
  dialog->handle = we_dprc_create (dlg_handle->modid, login_data->dlg_data.screen, 
                                    WE_DLG_LOGIN, &dprc_login_data); 
  WE_MEM_FREE (dlg_handle->modid, dprc_login_data.selectboxes);
  
  return op->wid;
}  

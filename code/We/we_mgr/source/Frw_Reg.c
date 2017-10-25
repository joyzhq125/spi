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




























































































































































#include "Frw_Reg.h"
#include "Frw.h"
#include "Frw_Int.h"
#include "Frw_Sig.h"

#include "We_File.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Cmmn.h"
#include "We_Mem.h"
#include "We_Errc.h"
#include "We_Chrt.h"











#define FRW_REG_FILE_INITIAL_SIZE                 1000
#define FRW_REG_FILE_SIZE_INCREMENT               1000
#define FRW_REG_MINBLOCKSIZE                      9
#define FRW_REG_BLOCK_HEADER_SIZE                 5
#define FRW_REG_MRB_SIZE                          (7 + FRW_REG_MAX_LENGTH_OF_PATH_KEY + 1 + 4)
                                                  


#define FRW_REG_STATE_NULL                        0
#define FRW_REG_STATE_OPEN                        1
#define FRW_REG_STATE_CLOSING                     2

#define FRW_REG_STATE_START_READING_BLOCK_HEADER  3
#define FRW_REG_STATE_READING_BLOCK_HEADER        4
#define FRW_REG_STATE_READING_BLOCK               5

#define FRW_REG_STATE_START_READING_VALUE_HEADER  6
#define FRW_REG_STATE_READING_VALUE_HEADER        7
#define FRW_REG_STATE_READING_VALUE               8

#define FRW_REG_STATE_WRITING_VALUE               9

#define FRW_REG_STATE_WRITING_VALUE_LOAD_TXT      10
#define FRW_REG_STATE_LOAD_TXT_READ               11
#define FRW_REG_STATE_LOAD_TXT_PARSE              12
#define FRW_REG_STATE_LOAD_TXT_OPEN               13

#define FRW_REG_STATE_WRITING_VALUE_ROOT          14


#define FRW_REG_BLOCK_TYPE_FREE                   0
#define FRW_REG_BLOCK_TYPE_INTERNAL               1
#define FRW_REG_BLOCK_TYPE_INTEGER                2
#define FRW_REG_BLOCK_TYPE_STRING                 3
#define FRW_REG_BLOCK_TYPE_UPDATE_POS             4   


#define FRW_REG_EXTERNAL                          "/external"

#define FRW_REG_SEQUENTIAL_IO_LIMIT               256     








#define frw_reg_read_list_out(p,q) \
  if (q == NULL) \
    frw_read_elem_list = p->next; \
  else \
    q->next = p->next; \
  FRW_FREE (p)

#define reg_log_str_null_filter(str)  ((str)?(str):"NULL")





typedef struct {
  int                    nrOfParams;
  we_registry_param_t  *params;
} frw_registry_batch_t;


typedef struct frw_free_list_st {
  struct frw_free_list_st *next;

  WE_UINT32 pos;       
  WE_UINT32 size;      
} frw_free_list_t;



#define ELEMENT_INTERNAL      0
#define ELEMENT_VALUE         1

typedef struct frw_reg_element_st {
  struct frw_reg_internal_st *parent_int; 
  char*                 name;

  int                   element_type; 
  union {
    struct frw_reg_internal_st  *p;   
    struct {
      WE_UINT32        pos;  
      WE_UINT32        size;  
    } value;
  } _u;
} frw_reg_element_t;

typedef struct frw_reg_internal_st {
  struct frw_reg_element_st *parent_elem; 
  WE_UINT32                pos;          
  WE_UINT32                size;         
  int                       num_elements;
  frw_reg_element_t         element[1];   
} frw_reg_internal_t;



typedef struct frw_read_element_st {
  struct frw_read_element_st  *next;
  WE_UINT32                  pos;          
  frw_reg_internal_t          *internal;    
  frw_reg_element_t           *element;     
  WE_UINT32                  value_size;   
} frw_read_element_t;



typedef struct frw_write_element_st {
  struct frw_write_element_st *next;
  int                         write_type; 
  union {
    frw_reg_internal_t        *internal;  
    WE_UINT32                file_pos;   
                                          
    struct {
      frw_reg_element_t       *p;         
      WE_INT32               value_int;  
      unsigned char*          value_str;  
    } value;
  } _u;
} frw_write_element_t;



typedef struct frw_reg_command_st {
  struct frw_reg_command_st  *next;
  int                     src_modId;
  int                     sig_num;
  void*                   sig_data;
  int                     wid;
  int                     batch_index;
  frw_reg_element_t*      batch_element;
} frw_reg_command_t;



typedef struct frw_write_task_st {
  struct frw_write_task_st *next;
  WE_UINT32               index;  
  char                     *buf;
  int                      pos;    
  int                      endpos;
} frw_write_task_t;


typedef struct frw_read_task_st {
  struct frw_read_task_st *next;
  WE_UINT32              pos;  
  we_registry_param_t*   param;
} frw_read_task_t;



typedef struct frw_reg_resp_integr_st {
  struct frw_reg_resp_integr_st *next;
  WE_UINT8                     resp_id;
  frw_reg_command_t*            task;
  we_registry_response_t*      repsonse;
} frw_reg_resp_integr_t;



typedef struct frw_subscription_st {
  WE_INT16                   wid;
  char                        *path;
  char                        *key;
  struct frw_subscription_st  *next;
} frw_subscription_t;

typedef struct frw_subscribed_modules_st {
  int                               moduleId;
  frw_subscription_t                *subscriptions;
  struct frw_subscribed_modules_st  *next;
} frw_subscribed_modules_t;






static WE_UINT32             frw_reg_size;       
static int                    frw_reg_enabled;    
static int                    frw_reg_state;      
static int                    frw_reg_file;       

static frw_reg_internal_t     *frw_reg_list;      
static frw_free_list_t        *frw_free_list;     

static frw_reg_command_t      *frw_command_list_first;  
static frw_reg_command_t      *frw_command_list_last;

static frw_write_task_t       *frw_write_task_list;     
static frw_write_element_t    *frw_write_elem_list;
static frw_read_element_t     *frw_read_elem_list;


static WE_UINT32             frw_reg_filepos;
static char                   *frw_reg_buf;
static int                    frw_reg_buflen;
static int                    frw_reg_pos;
static int                    frw_reg_endpos;

static frw_read_task_t        *frw_read_task_list;

static void                   *frw_reg_int_set_handle;  
static frw_reg_resp_integr_t  *frw_reg_int_response;    

static char                   *frw_reg_txt_file_name;
static int                    frw_reg_txt_file;         
static char                   *frw_reg_txt_buffer;
static WE_UINT32             frw_reg_txt_size;
static WE_UINT32             frw_reg_txt_pos;
static int                    frw_reg_init_done;
static char                   *frw_reg_def_file_name;

static int                    frw_reg_init_err_handled;
static int                    frw_reg_closing;
static WE_UINT32             frw_reg_sequential_io;   


static frw_subscribed_modules_t *frw_subscription_list;






static void
frw_reg_cvt_registry_resp (we_dcvt_t *obj, frw_registry_batch_t* p, int start_param, int end_param);

static int
frw_reg_handle_init_err (int reg_file_is_open);

static void
frw_reg_abort_startup (void);

static int
frw_reg_set (we_registry_param_t* param);

static void
frw_reg_main (frw_signal_t *sig);

static void
frw_reg_delete_command (void);

static void
frw_reg_txt_load (void);

static void
frw_reg_txt_read (void);

static void
frw_reg_txt_parse (void);

static void
frw_reg_complete_terminate (void);

static frw_registry_batch_t*
frw_reg_create_batch_struct (int nr_of_params);

#if defined(FRW_REG_LOG_WRITE_IN) || defined(FRW_REG_LOG_WRITE_END)
static void
frw_reg_log_param(const char           *heading,
                  int                   src_modId,
                  we_registry_param_t *param);

static void
frw_reg_log_task_reg_id(const char        *heading,
                        frw_reg_command_t *task);
#endif

#ifdef FRW_REG_LOG_WRITE_END
static void
frw_reg_log_batch_param(const char *heading);

static void
frw_reg_log_invalid_path_key(char *path, char* key);
#endif












static char *
frw_reg_strcat_extra (const char *s1, const char *s2)
{
  char *s_new = NULL;
  int   len1 = (s1 != NULL ? strlen (s1) : 0);
  int   len2 = (s2 != NULL ? strlen (s2) : 0);
  
  if ((s1 != NULL) || (s2 != NULL)) {
    s_new = (char*)FRW_ALLOC (len1 + len2 + 2);
    if (s1 != NULL) {
      strcpy (s_new, s1);
      s_new[len1] = '/';
    }
    if (s2 != NULL)
      strcpy (s_new + len1 + 1, s2);
  }

  return s_new;
}




static int
frw_reg_count_keys (frw_reg_internal_t* q, int nr_of_keys)
{
  int i;
    
  for (i = 0; i < q->num_elements; i++) {
    if (q->element[i].element_type == ELEMENT_VALUE)
      nr_of_keys++;
    else
      nr_of_keys = frw_reg_count_keys (q->element[i]._u.p, nr_of_keys);
  }
  return nr_of_keys;
}

static int
we_cvt_registry_batch_params (we_dcvt_t *obj, we_registry_param_t *q)
{
  if (!we_dcvt_string (obj, &(q->path)) ||
      !we_dcvt_string (obj, &(q->key)) ||
      !we_dcvt_uint8 (obj, &(q->type))) {
    return FALSE;
  }
  if (q->type == WE_REGISTRY_TYPE_INT){
    q->value_bv = NULL;
    q->value_bv_length = 0;
  }
  else
    q->value_i = 0;
  if (!we_dcvt_int32 (obj, &(q->value_i)) ||
      !we_dcvt_uint16 (obj, &(q->value_bv_length)) ||
      !we_dcvt_uchar_vector (obj, q->value_bv_length, &(q->value_bv))) {
    return FALSE;
  }
  return TRUE;
}

int
we_cvt_registry_batch (we_dcvt_t *obj, frw_registry_batch_t *p)
{
  if (!we_dcvt_int32 (obj, (WE_INT32*) &(p->nrOfParams)) ||
      !we_dcvt_array (obj, sizeof (we_registry_param_t), p->nrOfParams,
                       (void **)&(p->params), (we_dcvt_element_t *)we_cvt_registry_batch_params)) {
    return FALSE;
  }
  return TRUE;
}

static void
frw_reg_int_get_create_info (WE_UINT8 resp_id, frw_reg_command_t* task)
{
  frw_reg_resp_integr_t*  p;

  p = FRW_ALLOCTYPE (frw_reg_resp_integr_t);
  p->next = NULL;
  p->repsonse = NULL;
  p->resp_id = resp_id;
  p->task = task;

  if (frw_reg_int_response != NULL)
    p->next = frw_reg_int_response;
  frw_reg_int_response = p;
}

static int
frw_reg_int_get_send_response (frw_reg_command_t* task, we_registry_response_t* response)
{
  frw_reg_resp_integr_t*  p;

  p = frw_reg_int_response;
  while (p != NULL) {
    if (task == p->task) {
      p->repsonse = response;
      FRWa_registryResponse (p->resp_id);
      return TRUE;
    }
    p = p->next;
  }

  return FALSE;
}

static void
frw_reg_int_get_free_all (void)
{
  frw_reg_resp_integr_t*  p;

  while (frw_reg_int_response != NULL) {
    p = frw_reg_int_response;
    frw_reg_int_response = frw_reg_int_response->next;
    WE_REGISTRY_RESPONSE_FREE (WE_MODID_FRW, p->repsonse);
    FRW_FREE (p);
  }
}





static int
frw_reg_validate_path (char* path, int isWrite)
{
  int   i;
  char* s;

  if (path == NULL || *path != '/' || *(path + 1) == 0 || *(path + 1) == '/' || *(path + 1) == ' ')
    return FALSE;
  i = strlen (path);
  if (path[i-1] == ' ')
    return FALSE;
  if (path[i-1] == '/')
    path[i-1] = 0;

  if (isWrite) {
    path++;
    i = 0;
    while ((s = strchr (path, '/')) != NULL) {
      if ((s - path) > FRW_REG_MAX_LENGTH_OF_PATH_KEY || s == path)
        return FALSE;
      path = s + 1;
      i++;
    }
    if (strlen (path) > FRW_REG_MAX_LENGTH_OF_PATH_KEY)
      return FALSE;
    i++;
    if (i > FRW_REG_MAX_LEVELS_OF_PATHS)
      return FALSE;
  }
  return TRUE;
}

static int
frw_reg_validate_key (char* key)
{
  if (key == NULL || *key == 0 || *key == '/' || *key == ' ' || (strlen (key) > FRW_REG_MAX_LENGTH_OF_PATH_KEY))
    return FALSE;
  return TRUE;
}





static void
frw_reg_send_internal (int src_modId, frw_registry_batch_t* batch, WE_UINT16 length)
{
  we_dcvt_t              cvt_obj;
  void*                   buffer;
  we_registry_response_t *p;
  
  buffer = FRW_ALLOC (length);

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, buffer, length, WE_MODID_FRW);
  frw_reg_cvt_registry_resp (&cvt_obj, batch, 0, batch->nrOfParams);

  p = FRW_ALLOCTYPE (we_registry_response_t);
  p->wid = 0;
  p->path = NULL;
  p->buffer_start = p->next_element = buffer;
  p->buffer_end = p->buffer_start + length - 1;

  if (src_modId == WE_MODID_FRW)
    FRW_SIGNAL_SENDTO_P (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_REG_RESPONSE, p);
  else {
    if (frw_reg_int_get_send_response (frw_command_list_first, p) == FALSE) {
      
      FRW_FREE(buffer); 
      FRW_FREE(p);
    }
  }
}


static void
frw_reg_generate_response (void)
{
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *buffer, *user_data;
  frw_registry_batch_t* batch = (frw_registry_batch_t*)frw_command_list_first->sig_data;

  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  frw_reg_cvt_registry_resp (&cvt_obj, batch, 0, batch->nrOfParams);
  length = (WE_UINT16)cvt_obj.pos;

  if (frw_command_list_first->src_modId == WE_MODID_FRW || 
      frw_command_list_first->src_modId == WE_REG_MODID_INTEGRATION)
    frw_reg_send_internal (frw_command_list_first->src_modId, batch, length);
  else {
    buffer = WE_SIGNAL_CREATE (WE_SIG_REG_RESPONSE, WE_MODID_FRW, (WE_UINT8)frw_command_list_first->src_modId, (WE_UINT16)(length + 2));
    if (buffer != NULL){
      WE_INT16 temp = (WE_INT16)(frw_command_list_first->wid);
      user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
      we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
      we_dcvt_int16 (&cvt_obj, &temp);
      frw_reg_cvt_registry_resp (&cvt_obj, batch, 0, batch->nrOfParams);
      WE_SIGNAL_SEND (buffer);
    }
  }
  we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
  we_cvt_registry_batch (&cvt_obj, frw_command_list_first->sig_data);
}

static void
frw_reg_generate_empty_response (int modId)
{
  we_registry_response_t *p;
  void                    *buffer, *user_data;
  WE_UINT16              length;
  we_dcvt_t              cvt_obj;

  if (modId == WE_MODID_FRW || modId == WE_REG_MODID_INTEGRATION) {
    p = FRW_ALLOCTYPE (we_registry_response_t);
    p->wid = 0;
    p->path = NULL;
    p->buffer_end = p->buffer_start = p->next_element = FRW_ALLOC (1);
    if (modId == WE_MODID_FRW)
      FRW_SIGNAL_SENDTO_P (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_REG_RESPONSE, p);
    else {
      if (frw_reg_int_get_send_response (frw_command_list_first, p) == FALSE) {
        
        FRW_FREE(p);
      }
    }
  }
  else {
    buffer = WE_SIGNAL_CREATE (WE_SIG_REG_RESPONSE, WE_MODID_FRW, (WE_UINT8)modId, 3);
    if (buffer != NULL) {
      WE_INT16 temp = (WE_INT16)(frw_command_list_first->wid);
      user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
      we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
      we_dcvt_int16 (&cvt_obj, &temp);
      ((unsigned char*)(user_data))[cvt_obj.pos] = 0;
      WE_SIGNAL_SEND (buffer);
    }
  }
}





static frw_write_task_t *
frw_reg_new_write_task (WE_UINT32 index, int bufsize)
{
  frw_write_task_t *s, *t;

  s = FRW_ALLOCTYPE (frw_write_task_t);
  s->next = NULL;
  s->index = index;
  s->buf = FRW_ALLOC (bufsize);
  s->pos = 0;
  s->endpos = bufsize;
  memset(s->buf, 0, bufsize);

  if (frw_write_task_list == NULL)
    frw_write_task_list = s;
  else {
    for (t = frw_write_task_list; t->next != NULL; t = t->next);
    t->next = s;
  }

  return s;
}

static void
frw_reg_new_read_task (WE_UINT32 pos, we_registry_param_t* param)
{
  frw_read_task_t*  r;

  r = FRW_ALLOCTYPE (frw_read_task_t);
  r->pos = pos;
  r->param = param;

  if (frw_read_task_list == NULL){
      frw_read_task_list = r;
      r->next = NULL;
  }
  else{
     r->next = frw_read_task_list;
     frw_read_task_list = r;
  }
}

static int
frw_reg_read_op (void)
{
  int r;

   while (frw_reg_pos < frw_reg_endpos) {   
    r = WE_FILE_READ (frw_reg_file, frw_reg_buf + frw_reg_pos,
                       frw_reg_endpos - frw_reg_pos);
    if (r == WE_FILE_ERROR_DELAYED) {
      WE_FILE_SELECT (frw_reg_file, WE_FILE_EVENT_READ);
      frw_reg_sequential_io = 0;
      return 0;
    }
    else if (r < 0) {
      return -1;
    }
    frw_reg_pos += r;

    if (!frw_reg_init_done) {
      frw_reg_sequential_io += r;
      if (frw_reg_sequential_io > FRW_REG_SEQUENTIAL_IO_LIMIT) {
        FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
        frw_reg_sequential_io = 0;
        return 0;
      }
    }
  }

  return 1;
}

static int
frw_reg_write_op (frw_write_task_t *tk)
{
  int r;

  if (tk->pos == 0) {
    WE_FILE_SEEK (frw_reg_file, tk->index, TPI_FILE_SEEK_SET);
  }

  while (tk->pos < tk->endpos) {
    r = WE_FILE_WRITE (frw_reg_file, tk->buf + tk->pos,
                        tk->endpos - tk->pos);
    if ((r == WE_FILE_ERROR_DELAYED) || (r == 0)) {
      WE_FILE_SELECT (frw_reg_file, WE_FILE_EVENT_WRITE);
      return 0;
    }
    else if (r < 0) {
      return 1;
    }
    tk->pos += r;
  }

  return 1;
}






static frw_reg_internal_t*
frw_reg_get_internal (frw_reg_internal_t* p, char* name)
{
  int i;

  for (i = 0; i < p->num_elements; i++){
    if (p->element[i].element_type == ELEMENT_INTERNAL && we_cmmn_strcmp_nc (p->element[i].name, name) == 0)
      return p->element[i]._u.p;
  }
  return NULL;
}

static frw_reg_element_t*
frw_reg_get_value_element (frw_reg_internal_t* p, char* name)
{
  int i;

  for (i = 0; i < p->num_elements; i++){
    if (p->element[i].element_type == ELEMENT_VALUE && we_cmmn_strcmp_nc (p->element[i].name, name) == 0)
      return &(p->element[i]);
  }
  return NULL;
}






static int
frw_reg_cvt_write_elements (we_dcvt_t *obj, frw_reg_internal_t *p)
{
  long        j;
  int         i;
  WE_UINT32  pos;
  WE_UINT16  temp = (WE_UINT16)(p->num_elements);

  if (!we_dcvt_uint16 (obj, &temp))
    return FALSE;

  for (i = 0; i < p->num_elements; i++) {
    j = obj->pos;
    if (!we_dcvt_string (obj, &(p->element[i].name)))
      return FALSE;
    we_dcvt_change_pos (obj, (FRW_REG_MAX_LENGTH_OF_PATH_KEY + 1) - (obj->pos - j));
    if (p->element[i].element_type == ELEMENT_INTERNAL)
      pos = p->element[i]._u.p->pos;
    else
      pos = p->element[i]._u.value.pos;
    if (!we_dcvt_uint32 (obj, &pos))
      return FALSE;
  }
  return TRUE;
}

static int
frw_reg_cvt_write_block (we_dcvt_t *obj, frw_write_element_t *p, WE_UINT32 size)
{
  WE_UINT8 type = (WE_UINT8)(p->write_type);

  if (type == FRW_REG_BLOCK_TYPE_INTERNAL){
    if (!we_dcvt_uint32 (obj, &size) ||
        !we_dcvt_uint8 (obj, &type)) {
      return FALSE;
    }
    if (!frw_reg_cvt_write_elements (obj, p->_u.internal))
      return FALSE;
  }
  else if (type == FRW_REG_BLOCK_TYPE_INTEGER){
    if (!we_dcvt_uint32 (obj, &size) ||
        !we_dcvt_uint8 (obj, &type) ||
        !we_dcvt_int32 (obj, &(p->_u.value.value_int))) {
      return FALSE;
    }
  }
  else if (type == FRW_REG_BLOCK_TYPE_STRING){
    WE_UINT16 temp = (WE_UINT16)(p->_u.value.value_int);
    if (!we_dcvt_uint32 (obj, &size) ||
        !we_dcvt_uint8 (obj, &type) ||
        !we_dcvt_uint16 (obj, &temp) ||
        !we_dcvt_uchar_vector (obj, p->_u.value.value_int, &(p->_u.value.value_str))) {
      return FALSE;
    }
  }

  return TRUE;
}






static void
frw_reg_put_internal_into_tree (frw_reg_internal_t *internal, WE_UINT32 pos)
{
  frw_read_element_t  *p, *q;

  for (q = NULL, p = frw_read_elem_list; p != NULL; q = p, p = p->next) {
    if (p->pos == pos && p->internal == NULL && p->element != NULL){
      p->element->_u.p = internal;
      internal->parent_elem = p->element;
      p->element->element_type = ELEMENT_INTERNAL;
      frw_reg_read_list_out(p,q);
      return;
    }
  }
  p = FRW_ALLOCTYPE (frw_read_element_t);
  p->internal = internal;
  p->pos = pos;
  p->value_size = 0;
  p->element = NULL;
  p->next = frw_read_elem_list;
  frw_read_elem_list = p;
}

static void
frw_reg_put_element_into_tree (frw_reg_element_t *elem, WE_UINT32 pos)
{
  frw_read_element_t  *p, *q;

  for (q = NULL, p = frw_read_elem_list; p != NULL; q = p, p = p->next) {
    if (p->pos == pos && p->element == NULL) {
      if (p->internal != NULL){
        elem->_u.p = p->internal;
        p->internal->parent_elem = elem;
        elem->element_type = ELEMENT_INTERNAL;
        frw_reg_read_list_out(p,q);
        return;
      }
      else if (p->value_size != 0) {
        elem->_u.value.size = p->value_size;
        frw_reg_read_list_out(p,q);
        return;
      }
    }
  }
  p = FRW_ALLOCTYPE (frw_read_element_t);
  p->element = elem;
  p->pos = pos;
  p->value_size = 0;
  p->internal = NULL;
  p->next = frw_read_elem_list;
  frw_read_elem_list = p;
}

static void
frw_reg_put_value_into_tree (WE_UINT32 size, WE_UINT32 pos)
{
  frw_read_element_t  *p, *q;

  for (q = NULL, p = frw_read_elem_list; p != NULL; q = p, p = p->next) {
    if (p->pos == pos && p->element != NULL) {
      p->element->_u.value.size = size;
      frw_reg_read_list_out(p,q);
      return;
    }
  }
  p = FRW_ALLOCTYPE (frw_read_element_t);
  p->element = NULL;
  p->pos = pos;
  p->value_size = size;
  p->internal = NULL;
  p->next = frw_read_elem_list;
  frw_read_elem_list = p;
}

static int
frw_reg_cvt_read_block (we_dcvt_t *obj, frw_reg_internal_t *p)
{
  long        j;
  int         i;
  WE_UINT32  pos;

  if (p != frw_reg_list)
    frw_reg_put_internal_into_tree (p, p->pos);
  for (i = 0; i < p->num_elements; i++) {
    j = obj->pos;
    if (!we_dcvt_string (obj, &(p->element[i].name)))
      return FALSE;
    if (p->element[i].name == NULL) {
      p->element[i].name = WE_MEM_ALLOC (WE_MODID_FRW, 1);
      p->element[i].name[0] = 0;
    }
    we_dcvt_change_pos (obj, (FRW_REG_MAX_LENGTH_OF_PATH_KEY + 1) - (obj->pos - j));
    if (!we_dcvt_uint32 (obj, &pos))
      return FALSE;
    p->element[i].parent_int = p;
    p->element[i].element_type = ELEMENT_VALUE;   
    p->element[i]._u.value.pos = pos;
    frw_reg_put_element_into_tree (&(p->element[i]), pos);
  }

  return TRUE;
}






static WE_UINT32
frw_reg_add_new_block (frw_write_element_t *t)
{
  frw_free_list_t  *p, *q;
  frw_free_list_t  *pbest = NULL, *qbest = NULL;
  we_dcvt_t        cvt_obj;
  WE_UINT32        size, pos;
  int               len;
  frw_write_task_t       *tk;

  size = 0;
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  frw_reg_cvt_write_block (&cvt_obj, t, size);
  size = cvt_obj.pos;
    
  if (t->write_type == FRW_REG_BLOCK_TYPE_INTERNAL && t->_u.internal != NULL &&
      t->_u.internal->parent_elem == NULL) {
    if (size <= FRW_REG_MRB_SIZE){
      pos = 0;
      cvt_obj.pos = size = FRW_REG_MRB_SIZE;
      goto TASK_CREATE;
    }
    else
      return 0;
  }

  

  for (q = NULL, p = frw_free_list; p != NULL; q = p, p = p->next) {
    if ((p->size >= size) &&
        ((pbest == NULL) || (pbest->size > p->size))) {
      qbest = q;
      pbest = p;
    }
  }

  if (pbest == NULL) {
    



    for (q = NULL, p = frw_free_list; (p != NULL) && (p->next != NULL);
         q = p, p = p->next);

    if ((p == NULL) || (p->pos + p->size < frw_reg_size)) {
      
      pbest = FRW_ALLOCTYPE (frw_free_list_t);
      pbest->next = NULL;
      pbest->pos = frw_reg_size;
      pbest->size = FRW_REG_FILE_SIZE_INCREMENT;
      if (p != NULL) {
        p->next = pbest;
        qbest = p;
      }
      else {
        frw_free_list = pbest;
        qbest = NULL;
      }
    }
    else {
      
      pbest = p;
      qbest = q;
      pbest->size += FRW_REG_FILE_SIZE_INCREMENT;
    }

    frw_reg_size += FRW_REG_FILE_SIZE_INCREMENT;
    WE_FILE_SETSIZE (frw_reg_file, frw_reg_size);
  }

  pos = pbest->pos;
  if (pbest->size > size + FRW_REG_MINBLOCKSIZE) {
    
    pbest->size -= size;
    pbest->pos += size;
  }
  else {
    
    if (qbest == NULL)
      frw_free_list = pbest->next;
    else
      qbest->next = pbest->next;
    size = pbest->size;
    
    FRW_FREE (pbest);
    pbest = NULL;
  }

TASK_CREATE:
  if (t->write_type == FRW_REG_BLOCK_TYPE_INTERNAL)
    t->_u.internal->size = size;
  else
    t->_u.value.p->_u.value.size = size;

  len = cvt_obj.pos;
    
  if (pbest != NULL)
    len += 5;
  tk = frw_reg_new_write_task (pos, len);

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, tk->buf, tk->endpos, 0);
  frw_reg_cvt_write_block (&cvt_obj, t, size);
    
  if (pbest != NULL) {
    WE_UINT8 type = FRW_REG_BLOCK_TYPE_FREE;
    we_dcvt_uint32 (&cvt_obj, &(pbest->size));
    we_dcvt_uint8 (&cvt_obj, &type);
  }

  return pos;
}

static void
frw_reg_delete_block (WE_UINT32 block_pos, WE_UINT32 block_size)
{
  frw_free_list_t   *p, *q, *r;
  WE_UINT32        pos;
  WE_UINT32        size;
  we_dcvt_t        cvt_obj;
  frw_write_task_t  *tk;
  WE_UINT8         type = FRW_REG_BLOCK_TYPE_FREE;

  
  for (q = NULL, p = frw_free_list; p != NULL; q = p, p = p->next) {
    if (p->pos > block_pos)
      break;
  }

  if ((q != NULL) && (q->pos + q->size == block_pos)) {
    
    q->size += block_size;
    if ((p != NULL) && (block_pos + block_size == p->pos)) {
      
      q->size += p->size;
      q->next = p->next;
      FRW_FREE (p);
    }
    pos = q->pos;
    size = q->size;
  }
  else if ((p != NULL) && (block_pos + block_size == p->pos)) {
    
    p->pos = block_pos;
    p->size += block_size;
    pos = p->pos;
    size = p->size;
  }
  else {
    
    r = FRW_ALLOCTYPE (frw_free_list_t);
    r->next = p;
    if (q == NULL)
      frw_free_list = r;
    else
      q->next = r;
    pos = r->pos = block_pos;
    size = r->size = block_size;
  }

  
  tk = frw_reg_new_write_task (pos, 5);
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, tk->buf, tk->endpos, 0);
  we_dcvt_uint32 (&cvt_obj, &size);
  we_dcvt_uint8 (&cvt_obj, &type);
}






static void
frw_reg_create_update_task (frw_write_element_t* p, WE_UINT32 pos)
{
  frw_write_task_t       *tk;
  we_dcvt_t             cvt_obj;

  tk = frw_reg_new_write_task (p->_u.file_pos, 4);

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, tk->buf, tk->endpos, 0);
  we_dcvt_uint32 (&cvt_obj, &pos);
}

static void
frw_reg_write_elements (void)
{
  frw_write_element_t*  p;
  WE_UINT32            pos = 0;
  int                   i = 0;

  while (frw_write_elem_list != NULL){
    if (i > 0 && frw_write_elem_list->write_type == FRW_REG_BLOCK_TYPE_UPDATE_POS)
      frw_reg_create_update_task (frw_write_elem_list, pos);
    else {
      pos = frw_reg_add_new_block (frw_write_elem_list);
      if (frw_write_elem_list->write_type == FRW_REG_BLOCK_TYPE_INTERNAL)
        frw_write_elem_list->_u.internal->pos = pos;    
      else
        frw_write_elem_list->_u.value.p->_u.value.pos = pos;
    }
    p = frw_write_elem_list;
    frw_write_elem_list = frw_write_elem_list->next;
    FRW_FREE (p);
    i++;
  }
}

static void
frw_reg_insert_write_elem (void* p, int type, WE_INT32 val_int, unsigned char* val_str)
{
  frw_write_element_t*  element;

  element = FRW_ALLOCTYPE (frw_write_element_t);
  element->write_type = type;
  if (type == FRW_REG_BLOCK_TYPE_INTERNAL)
    element->_u.internal = p;
  else if (type == FRW_REG_BLOCK_TYPE_INTEGER){
    element->_u.value.p = p;
    element->_u.value.value_int = val_int;
  }
  else if (type == FRW_REG_BLOCK_TYPE_STRING){
    element->_u.value.p = p;
    element->_u.value.value_int = val_int;
    element->_u.value.value_str = val_str;
  }

  element->next = frw_write_elem_list;
  frw_write_elem_list = element;
}

static void
frw_reg_create_update_write_element (frw_reg_internal_t* q)
{
  frw_reg_internal_t*   r;
  frw_write_element_t*  element;
  int                   i;
  WE_UINT32            pos;

  if (q->parent_elem != NULL) {
    r = q->parent_elem->parent_int;
    for (i = 0; i < r->num_elements; i++) {
      if (strcmp (q->parent_elem->name, r->element[i].name) == 0)
        break;
    }
    if (i < r->num_elements) {
      pos = r->pos + 3 +                
            ((i + 1) * (FRW_REG_MAX_LENGTH_OF_PATH_KEY + 5));  
      element = FRW_ALLOCTYPE (frw_write_element_t);
      element->write_type = FRW_REG_BLOCK_TYPE_UPDATE_POS;
      element->_u.file_pos = pos;
      element->next = frw_write_elem_list;
      frw_write_elem_list = element;
    }
  }
}

static void
frw_reg_add_empty_element (frw_reg_internal_t** p)
{
  frw_reg_internal_t  *q, *r;
  int                 i;

  r = *p;
  if (r->num_elements > 0){
    q = FRW_ALLOC (sizeof(frw_reg_internal_t) + ((r->num_elements) * sizeof(frw_reg_element_t)));
    q->num_elements = r->num_elements + 1;
    q->parent_elem = r->parent_elem;
    if (r != frw_reg_list)
      r->parent_elem->_u.p = q;
    else
      frw_reg_list = q;
    for (i = 0; i < r->num_elements; i++){
      q->element[i].name = r->element[i].name;
      q->element[i].parent_int = q;
      q->element[i].element_type = r->element[i].element_type;
      if (r->element[i].element_type == ELEMENT_INTERNAL){
        q->element[i]._u.p = r->element[i]._u.p;
        q->element[i]._u.p->parent_elem = &(q->element[i]);
      }
      else {
        q->element[i]._u.value.pos = r->element[i]._u.value.pos;
        q->element[i]._u.value.size = r->element[i]._u.value.size;
      }
    }
    q->element[i].parent_int = q;
    if (q != frw_reg_list) {
      
      frw_reg_create_update_write_element (q);
      
      frw_reg_delete_block (r->pos, r->size);
    }
    FRW_FREE (r);
    *p = q;
  }
  else {
    r->num_elements = 1;
    r->element[0].parent_int = r;
  }
}

static frw_reg_internal_t*
frw_reg_add_internal (frw_reg_internal_t** p, char* name)
{
  frw_reg_internal_t  *q, *r;
  int                 i;

  frw_reg_add_empty_element (p);
  q = *p;
  i = q->num_elements - 1;
  q->element[i].name = FRW_ALLOC (strlen (name) + 1);
  strcpy (q->element[i].name, name);
  q->element[i].element_type = ELEMENT_INTERNAL;
  r = FRW_ALLOCTYPE (frw_reg_internal_t);
  r->num_elements = 0;
  r->parent_elem = &(q->element[i]);
  q->element[i]._u.p = r;

  frw_reg_insert_write_elem (q, FRW_REG_BLOCK_TYPE_INTERNAL, 0, NULL);

  return r;
}

static int
frw_reg_add_value (frw_reg_internal_t** p, we_registry_param_t* param)
{
  frw_reg_internal_t  *q;
  int                 i;

  frw_reg_add_empty_element (p);
  q = *p;
  i = q->num_elements - 1;
  q->element[i].name = we_cmmn_strdup (WE_MODID_FRW, param->key);
  q->element[i].element_type = ELEMENT_VALUE;

  frw_reg_insert_write_elem (q, FRW_REG_BLOCK_TYPE_INTERNAL, 0, NULL);
  if (param->type == WE_REGISTRY_TYPE_INT)
    frw_reg_insert_write_elem (&(q->element[i]), FRW_REG_BLOCK_TYPE_INTEGER, param->value_i, NULL);
  else
    frw_reg_insert_write_elem (&(q->element[i]), FRW_REG_BLOCK_TYPE_STRING, param->value_bv_length, param->value_bv);

  return i;
}






static void
frw_reg_clear_param (void)
{
  frw_registry_batch_t* batch = (frw_registry_batch_t*)frw_command_list_first->sig_data;
  we_registry_param_t* param = &(batch->params[frw_command_list_first->batch_index]);

  FRW_FREE (param->path);
  FRW_FREE (param->key);
  if (param->type == WE_REGISTRY_TYPE_STR)
    FRW_FREE (param->value_bv);

  param->path = NULL;
  param->key = NULL;
  param->value_bv = NULL;
}

static int
frw_reg_batch_execute_next (void)
{
  frw_registry_batch_t* batch = (frw_registry_batch_t*)(frw_command_list_first->sig_data);

  for (; frw_command_list_first->batch_index < ((frw_registry_batch_t*)(frw_command_list_first->sig_data))->nrOfParams;
      (frw_command_list_first->batch_index)++) {
    if (frw_reg_set (&(batch->params[frw_command_list_first->batch_index])))
      return TRUE;
    else {
#ifdef FRW_REG_LOG_WRITE_END
      frw_reg_log_batch_param("finished SET, FAILED");
#endif
      frw_reg_clear_param ();
    }
  }
  return FALSE;
}


static void
frw_reg_cvt_registry_resp (we_dcvt_t *obj, frw_registry_batch_t* p, int start_param, int end_param)
{
  int       i;
  char*     tmp_path;
  WE_UINT8 type;

  tmp_path = NULL;
  for (i = start_param; i < end_param; i++) {
    if (p->params[i].path != NULL) {
      if (tmp_path == NULL || strcmp (tmp_path, p->params[i].path) != 0) {
        type = WE_REG_SET_TYPE_PATH;
        we_dcvt_uint8 (obj, &type);
        we_dcvt_string (obj, &(p->params[i].path));
        tmp_path = p->params[i].path;
      }
      if (p->params[i].type == WE_REGISTRY_TYPE_INT) {
        type = WE_REG_SET_TYPE_INT;
        we_dcvt_uint8 (obj, &type);
        we_dcvt_string (obj, &(p->params[i].key));
        we_dcvt_int32 (obj, &(p->params[i].value_i));
      }
      else if (p->params[i].type == WE_REGISTRY_TYPE_STR) {
        type = WE_REG_SET_TYPE_STR;
        we_dcvt_uint8 (obj, &type);
        we_dcvt_string (obj, &(p->params[i].key));
        we_dcvt_uint16 (obj, &(p->params[i].value_bv_length));
        we_dcvt_uchar_vector (obj, p->params[i].value_bv_length, &(p->params[i].value_bv));
      }
      else {
        type = WE_REG_SET_TYPE_DELETED;
        we_dcvt_uint8 (obj, &type);
        we_dcvt_string (obj, &(p->params[i].key));
      }
    }
  }
}

static void
frw_reg_send_notification (int moduleId, WE_INT16 wid, frw_registry_batch_t* batch)
{
  void         *buffer, *user_data;
  WE_UINT16   length;
  we_dcvt_t   cvt_obj;

  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  frw_reg_cvt_registry_resp (&cvt_obj, batch, 0, batch->nrOfParams);
  length = (WE_UINT16)cvt_obj.pos;

  if (moduleId == WE_MODID_FRW) {
    we_registry_response_t *q;

    q = FRW_ALLOCTYPE (we_registry_response_t);
    q->wid = wid;
    q->path = NULL;
    q->buffer_start = q->next_element = FRW_ALLOC (length);
    q->buffer_end = q->buffer_start + length - 1;
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, q->buffer_start, length, WE_MODID_FRW);
    frw_reg_cvt_registry_resp (&cvt_obj, batch, 0, batch->nrOfParams);

    FRW_SIGNAL_SENDTO_P (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_REG_RESPONSE, q);
  }
  else if (moduleId != WE_REG_MODID_INTEGRATION)
  {
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: Send signal WE_SIG_REG_RESPONSE Dest %d\n", moduleId));
    buffer = WE_SIGNAL_CREATE (WE_SIG_REG_RESPONSE, WE_MODID_FRW, (WE_UINT8)moduleId, (WE_UINT16)(length + 2));
    if (buffer != NULL){
      user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
      we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
      (void)we_dcvt_int16 (&cvt_obj, &wid);
      frw_reg_cvt_registry_resp (&cvt_obj, batch, 0, batch->nrOfParams);
      WE_SIGNAL_SEND (buffer);
    }
  }
  else
  {
    we_registry_response_t *rsp = FRW_ALLOCTYPE (we_registry_response_t);
    frw_reg_resp_integr_t   *integr_elem = FRW_ALLOCTYPE (frw_reg_resp_integr_t);

    buffer = FRW_ALLOC (length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, buffer, length, WE_MODID_FRW);
    frw_reg_cvt_registry_resp (&cvt_obj, batch, 0, batch->nrOfParams);

    
    rsp->wid = wid;
    rsp->path = NULL;
    rsp->buffer_start = rsp->next_element = buffer;
    rsp->buffer_end = rsp->buffer_start + length - 1;
    integr_elem->repsonse = rsp;
    integr_elem->resp_id = (WE_UINT8) rsp->wid;
    integr_elem->task = NULL;
    if (frw_reg_int_response != NULL)
      integr_elem->next = frw_reg_int_response;
    else
      integr_elem->next = NULL;
    frw_reg_int_response = integr_elem;

    FRWa_registryResponse (integr_elem->resp_id);
  }
}






static void
frw_reg_notify_subscribers (int src_modId, const frw_registry_batch_t* p,
                            int start_param, int end_param)
{
  char*                     tmp_path;
  int                       i;
  frw_subscribed_modules_t *moduleListPtr = frw_subscription_list;
  frw_registry_batch_t     *copyBatch;

  tmp_path = NULL;
  for (i = start_param; i < end_param; i++) {
    if (p->params[i].path != NULL) {
      tmp_path = p->params[i].path;
      break;
    }
  }
  if (tmp_path == NULL)
    return;

  copyBatch = frw_reg_create_batch_struct (p->nrOfParams);
  
  while (moduleListPtr != NULL)
  {
    frw_subscription_t *subscrPtr = moduleListPtr->subscriptions;
    int                 index;
    int                 nrOfParams;
    WE_INT16           wid;

    
    if (src_modId == moduleListPtr->moduleId) {
      moduleListPtr = moduleListPtr->next;
      continue;
    }

    
    if ((frw_get_module_status ((WE_UINT8)moduleListPtr->moduleId) != MODULE_STATUS_CREATED &&
         frw_get_module_status ((WE_UINT8)moduleListPtr->moduleId) != MODULE_STATUS_ACTIVE)) {
      moduleListPtr = moduleListPtr->next;
      continue;
    }

     
    while (subscrPtr != NULL)
    {
      nrOfParams = 0;
      wid = subscrPtr->wid;
      
      do
      {
        for (index = start_param; index < end_param; index++)
        {
          unsigned subscrPathLength;

          if (p->params[index].path == NULL)
            continue;

          subscrPathLength = strlen(subscrPtr->path);;

          

          if ((subscrPtr->key == NULL) && (subscrPathLength < strlen(p->params[index].path)))
          {
            if (we_cmmn_strncmp_nc(subscrPtr->path, p->params[index].path, subscrPathLength) == 0)
            {
              if (p->params[index].path[subscrPathLength] == '/')
              {
                
                
                memcpy(&copyBatch->params[nrOfParams++], &p->params[index], sizeof(we_registry_param_t));
              }
            }     
          }
          else if (we_cmmn_strcmp_nc(subscrPtr->path, p->params[index].path) == 0)
          {
            
            if (subscrPtr->key != NULL)
            {
              
              if(p->params[index].key != NULL &&
                 we_cmmn_strcmp_nc(subscrPtr->key, p->params[index].key) == 0)
              {
                
                memcpy(&copyBatch->params[nrOfParams++], &p->params[index], sizeof(we_registry_param_t));
              }
            }
            else
            {
              
              memcpy(&copyBatch->params[nrOfParams++], &p->params[index], sizeof(we_registry_param_t));
            }
          }
        } 
        subscrPtr = subscrPtr->next;
      } while ((subscrPtr != NULL) && (subscrPtr->wid == wid));

      if (nrOfParams > 0)
      {
        copyBatch->nrOfParams = nrOfParams;
        frw_reg_send_notification (moduleListPtr->moduleId, wid, copyBatch);
      }
    } 

    moduleListPtr = moduleListPtr->next;
  } 

  FRW_FREE (copyBatch->params);
  FRW_FREE (copyBatch);
}



static void
frw_reg_batch_finish (void)
{
  frw_registry_batch_t* p = (frw_registry_batch_t*)frw_command_list_first->sig_data;
  we_dcvt_t            cvt_obj;

  if (p->nrOfParams > 0)
    frw_reg_notify_subscribers (frw_command_list_first->src_modId, p, 0, p->nrOfParams);

  we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
  we_cvt_registry_batch (&cvt_obj, frw_command_list_first->sig_data);
  frw_reg_delete_command ();
}

static frw_registry_batch_t*
frw_reg_create_batch_struct (int nr_of_params)
{
  frw_registry_batch_t  *t;

  t = FRW_ALLOCTYPE (frw_registry_batch_t);
  t->nrOfParams = nr_of_params;
  t->params = FRW_ALLOC (nr_of_params * sizeof (we_registry_param_t));
  return t;
}

static we_registry_param_t*
frw_reg_create_batch_tasks (frw_reg_internal_t* q, char* path, we_registry_param_t* param, int* first_param)
{
  int   i;
  char* path_new;
    
  for (i = 0; i < q->num_elements; i++) {
    if (q->element[i].element_type == ELEMENT_VALUE) {
      if (!*first_param)
        param++;
      else
        *first_param = 0;
      param->path = we_cmmn_strdup (WE_MODID_FRW, path);
      param->key = we_cmmn_strdup (WE_MODID_FRW, q->element[i].name);
      frw_reg_new_read_task (q->element[i]._u.value.pos, param);
    }
    else {
      path_new = frw_reg_strcat_extra (path, q->element[i].name);
      param = frw_reg_create_batch_tasks (q->element[i]._u.p, path_new, param, first_param);
      FRW_FREE (path_new);
    }
  }
  return param;
}


static we_registry_param_t*
frw_reg_create_delete_batch (frw_reg_internal_t* q, char* path, we_registry_param_t* param, int* first_param)
{
  int   i;
  char* path_new;
    
  for (i = 0; i < q->num_elements; i++) {
    if (q->element[i].element_type == ELEMENT_VALUE) {
      if (!*first_param)
        param++;
      else
        *first_param = 0;
      param->path = we_cmmn_strdup (WE_MODID_FRW, path);
      param->key = we_cmmn_strdup (WE_MODID_FRW, q->element[i].name);
      param->type = WE_REGISTRY_TYPE_DELETED;
    }
    else {
      path_new = frw_reg_strcat_extra (path, q->element[i].name);
      param = frw_reg_create_delete_batch (q->element[i]._u.p, path_new, param, first_param);
      FRW_FREE (path_new);
    }
  }
  return param;
}




static int
frw_reg_check_external (we_registry_identifier_t* param)
{
  int       len = strlen (FRW_REG_EXTERNAL);
  char*     p;

  if (param->path == NULL || we_cmmn_strncmp_nc (param->path, FRW_REG_EXTERNAL, len) != 0)
    return FALSE;

  if (param->key == NULL)
    return FALSE;

  p = &(param->path[len]);
  if (*p != '/' && *p != 0)
    return FALSE;

  len = strlen (param->path);
  if (param->path[len - 1] == '/')
    param->path[len - 1] = 0;

  return TRUE;
}

static void
frw_reg_store_command (frw_signal_t *sig)
{
  frw_reg_command_t   *task;

  task = FRW_ALLOCTYPE (frw_reg_command_t);
  task->src_modId = sig->i_param;
  task->batch_element = NULL;
  task->next = NULL;

  if (sig->type == FRW_REGISTRY_SIG_SET) {
    we_registry_response_t* handle = (we_registry_response_t*)sig->p_param;
    frw_registry_batch_t*    batch;
    we_registry_param_t     param;
    int                      i = 0;

    while (WE_REGISTRY_RESPONSE_GET_NEXT (handle, &param))
      i++;

    batch = frw_reg_create_batch_struct (i);

    handle->next_element = handle->buffer_start;
    handle->path = NULL;
    i = 0;
    while (WE_REGISTRY_RESPONSE_GET_NEXT (handle, &param)) {
      batch->params[i].type = param.type;
      batch->params[i].path = we_cmmn_strdup (WE_MODID_FRW, param.path);
      batch->params[i].key = we_cmmn_strdup (WE_MODID_FRW, param.key);
      batch->params[i].value_i = param.value_i;
      batch->params[i].value_bv_length = param.value_bv_length;
      if (param.type == WE_REGISTRY_TYPE_STR && param.value_bv_length > 0) {
        batch->params[i].value_bv = WE_MEM_ALLOC (WE_MODID_FRW, param.value_bv_length);
        memcpy (batch->params[i].value_bv, param.value_bv, param.value_bv_length);
      }
      else
        batch->params[i].value_bv = NULL;
      i++;
#ifdef FRW_REG_LOG_WRITE_IN
      frw_reg_log_param("received SET", task->src_modId, &param);
#endif
    }
    WE_REGISTRY_RESPONSE_FREE (WE_MODID_FRW, handle);

    task->sig_data = batch;
  }
  else if (sig->type == FRW_REGISTRY_SIG_SUBSCRIBE) {
    task->sig_data = sig->p_param;
    task->wid = ((we_registry_subscription_t*)sig->p_param)->wid;
  }
  else {
    task->sig_data = sig->p_param;
    task->wid = ((we_registry_identifier_t*)sig->p_param)->wid;
#ifdef FRW_REG_LOG_WRITE_IN
    if (sig->type == FRW_REGISTRY_SIG_DELETE) {
      frw_reg_log_task_reg_id("received DELETE", task);
    }
#endif
  }

  task->sig_num = sig->type;

  if (task->sig_num == FRW_REGISTRY_SIG_GET) {
    if (task->src_modId == WE_REG_MODID_INTEGRATION)
      frw_reg_int_get_create_info ((WE_UINT8)(sig->u_param1), task);
    else if (frw_reg_check_external (sig->p_param)) {
      
      we_dcvt_t   cvt_obj;
      WE_INT32 temp_id = task->wid | (((WE_INT32)(task->src_modId)) << 16);

      FRWa_externalGet (temp_id, ((we_registry_identifier_t*)sig->p_param)->path,
                                 ((we_registry_identifier_t*)sig->p_param)->key);
      we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
      we_cvt_registry_identifier (&cvt_obj, task->sig_data);
      FRW_FREE (task->sig_data);
      FRW_FREE (task);
      return;
    }
  }

  if (frw_command_list_last == NULL)
    frw_command_list_first = frw_command_list_last = task;
  else {
    frw_command_list_last->next = task;
    frw_command_list_last = task;
  }
}

static void
frw_reg_delete_command (void)
{
  frw_reg_command_t   *task;

  if (frw_command_list_first != NULL) {
    task = frw_command_list_first->next;

    FRW_FREE (frw_command_list_first->sig_data);
    FRW_FREE (frw_command_list_first);
    frw_command_list_first = task;
    if (frw_command_list_first == NULL)
      frw_command_list_last = NULL;
    else
      FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
  }

  if (frw_reg_closing && (frw_command_list_first == NULL)) {
    WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: registry termination continues.\n"));    
    frw_reg_complete_terminate();
  }
}

static void
frw_reg_update_value (void)
{
  frw_reg_internal_t*   r;
  frw_write_element_t*  write_element;
  int                   i;
  WE_UINT32            pos;
  frw_reg_element_t*    elem = frw_command_list_first->batch_element;
  frw_registry_batch_t* batch = (frw_registry_batch_t*)frw_command_list_first->sig_data;
  we_registry_param_t* param = &(batch->params[frw_command_list_first->batch_index]);


  frw_reg_delete_block (elem->_u.value.pos, elem->_u.value.size);
  r = elem->parent_int;
  for (i = 0; i < r->num_elements; i++) {
    if (strcmp (elem->name, r->element[i].name) == 0)
      break;
  }
  if (i < r->num_elements) {
    pos = r->pos + 3 +                
          ((i + 1) * (FRW_REG_MAX_LENGTH_OF_PATH_KEY + 5));  
    write_element = FRW_ALLOCTYPE (frw_write_element_t);
    write_element->write_type = FRW_REG_BLOCK_TYPE_UPDATE_POS;
    write_element->_u.file_pos = pos;
    write_element->next = frw_write_elem_list;
    frw_write_elem_list = write_element;
  }

  if (param->type == WE_REGISTRY_TYPE_INT)
    frw_reg_insert_write_elem (elem, FRW_REG_BLOCK_TYPE_INTEGER, param->value_i, NULL);
  else
    frw_reg_insert_write_elem (elem, FRW_REG_BLOCK_TYPE_STRING, param->value_bv_length, param->value_bv);

  frw_reg_write_elements ();
  frw_command_list_first->batch_element = NULL;
}

static void
frw_reg_set_state_open (void)
{
  frw_reg_state = FRW_REG_STATE_OPEN;
  if (frw_reg_init_done == FALSE) {
    frw_reg_init_done = TRUE;
    FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_SUB_MOD_INITIALISED);
    frw_reg_sequential_io = 0;
  }
}




static void
frw_reg_finish_command (void)
{
  we_dcvt_t          cvt_obj;
  int                 param_cleared = 0;

  if (frw_command_list_first != NULL) {
    switch (frw_command_list_first->sig_num) {
      case FRW_REGISTRY_SIG_SET:
        if (frw_reg_state == FRW_REG_STATE_START_READING_VALUE_HEADER) {
          frw_reg_state = FRW_REG_STATE_WRITING_VALUE;
          if (frw_command_list_first->batch_element != NULL) {
            frw_reg_update_value ();
            FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
            return;
          }
          else {
            
#ifdef FRW_REG_LOG_WRITE_END
            frw_reg_log_batch_param("finished SET, already stored");
#endif
            frw_reg_clear_param ();
            param_cleared = 1;
          }
        }
        if (frw_reg_state == FRW_REG_STATE_WRITING_VALUE) {
          if (!param_cleared) {
#ifdef FRW_REG_LOG_WRITE_END
            frw_reg_log_batch_param("finished SET");
#endif
            WE_FILE_FLUSH (frw_reg_file);
          }
          (frw_command_list_first->batch_index)++;
          if (frw_reg_batch_execute_next ()) {
            FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
            return;
          }
          else
            frw_reg_batch_finish ();  
        }
        break;
      case FRW_REGISTRY_SIG_DELETE:
#ifdef FRW_REG_LOG_WRITE_END
        frw_reg_log_task_reg_id("finished DELETE", frw_command_list_first);
#endif
        WE_FILE_FLUSH (frw_reg_file);
        we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
        we_cvt_registry_identifier (&cvt_obj, frw_command_list_first->sig_data);
        frw_reg_delete_command ();
        break;
      case FRW_REGISTRY_SIG_SUBSCRIBE:
        we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
        (void)we_cvt_registry_subscription (&cvt_obj, frw_command_list_first->sig_data);
        frw_reg_delete_command ();
        break;
      case FRW_REGISTRY_SIG_GET:
        frw_reg_generate_response ();
        frw_reg_delete_command ();
        break;
    }
  }
  frw_reg_set_state_open ();
}






static void
frw_reg_delete_element (frw_reg_internal_t* q)
{
  frw_reg_internal_t* p;
  we_dcvt_t          cvt_obj;
  frw_write_task_t    *tk;

  p = q->parent_elem->parent_int;
  if (p->num_elements == 1) {
    FRW_FREE (p->element[0].name);
    if (p != frw_reg_list)
      frw_reg_delete_element (p);
    else {
      p->num_elements = 0;
      tk = frw_reg_new_write_task (5, 2);
      tk->buf[0] = tk->buf[1] = 0;
    }
  }
  else {
    int i, j = 0;
    char* name = q->parent_elem->name;
      
    for (i = 0; i < p->num_elements; i++)
      if (strcmp (name, p->element[i].name) == 0) {
        j = i;
        break;
      }
    FRW_FREE (name);
   
    for (i = j; i < (p->num_elements - 1); i++) {
      p->element[i].name = p->element[i+1].name;
      p->element[i].element_type = p->element[i+1].element_type;
      p->element[i]._u = p->element[i+1]._u;
      if (p->element[i].element_type == ELEMENT_INTERNAL)
        p->element[i]._u.p->parent_elem = &(p->element[i]);
    }

    (p->num_elements)--;

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    frw_reg_cvt_write_elements (&cvt_obj, p);

    tk = frw_reg_new_write_task (p->pos + 5, cvt_obj.pos);

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, tk->buf, tk->endpos, 0);
    frw_reg_cvt_write_elements (&cvt_obj, p);
  }

  frw_reg_delete_block (q->pos, q->size);
  FRW_FREE (q);
}

static void
frw_reg_delete_all_down_elements (frw_reg_internal_t* q, int first_element, int remove_block)
{
  int i;

  if (q == NULL)
    return;
  for (i = 0; i < q->num_elements; i++) {
    if (q->element[i].element_type == ELEMENT_VALUE) {
      if (remove_block)
        frw_reg_delete_block (q->element[i]._u.value.pos, q->element[i]._u.value.size);
    }
    else
      frw_reg_delete_all_down_elements (q->element[i]._u.p, 0, remove_block);
    FRW_FREE (q->element[i].name);
  }
  if (first_element == 0){
    if (remove_block)
      frw_reg_delete_block (q->pos, q->size);
    FRW_FREE (q);
  }
}

static int
frw_reg_delete_value (frw_reg_internal_t* p, char* name)
{
  we_dcvt_t          cvt_obj;
  frw_write_task_t    *tk;
  int i, j;

  for (i = 0; i < p->num_elements; i++)
    if (p->element[i].element_type == ELEMENT_VALUE &&
        we_cmmn_strcmp_nc (p->element[i].name, name) == 0)
      break;
  if (i == p->num_elements)
    return FALSE;

  FRW_FREE (p->element[i].name);
  frw_reg_delete_block (p->element[i]._u.value.pos, p->element[i]._u.value.size);
  if (p->num_elements == 1)
    frw_reg_delete_element (p);
  else {
    for (j = i; j < (p->num_elements - 1); j++) {
      p->element[j].name = p->element[j+1].name;
      p->element[j].element_type = p->element[j+1].element_type;
      p->element[j]._u = p->element[j+1]._u;
      if (p->element[j].element_type == ELEMENT_INTERNAL)
        p->element[j]._u.p->parent_elem = &(p->element[j]);
    }
    (p->num_elements)--;

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    frw_reg_cvt_write_elements (&cvt_obj, p);

    tk = frw_reg_new_write_task (p->pos + 5, cvt_obj.pos);

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, tk->buf, tk->endpos, 0);
    frw_reg_cvt_write_elements (&cvt_obj, p);
  }
  return TRUE;
}






static int
frw_reg_read_value (frw_read_task_t* t)
{
  WE_UINT32          size;
  WE_INT32           value_int;
  WE_UINT16          bv_len;
  we_dcvt_t          cvt_obj;
  int                 r;
  WE_UINT8           type;
  WE_UINT8           param_block_type;

  switch (frw_reg_state) {
  case FRW_REG_STATE_START_READING_VALUE_HEADER:
    frw_reg_filepos = t->pos;
    if (frw_reg_filepos + FRW_REG_BLOCK_HEADER_SIZE >= frw_reg_size) {
      
      return 1;
    }
    frw_reg_pos = 0;
    frw_reg_endpos = FRW_REG_BLOCK_HEADER_SIZE;
    WE_FILE_SEEK (frw_reg_file, frw_reg_filepos, TPI_FILE_SEEK_SET);
    frw_reg_state = FRW_REG_STATE_READING_VALUE_HEADER;

  case FRW_REG_STATE_READING_VALUE_HEADER:
    if ((r = frw_reg_read_op ()) == 0) {
      return 0;
    }
    else if (r < 0) {
      
      return 1;
    }

    we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, frw_reg_buf, FRW_REG_BLOCK_HEADER_SIZE, 0);
    we_dcvt_uint32 (&cvt_obj, &size);
    we_dcvt_uint8 (&cvt_obj, &(type));

    if (frw_command_list_first->batch_element != NULL) {
      
      if (t->param->type == WE_REGISTRY_TYPE_INT)
        param_block_type = FRW_REG_BLOCK_TYPE_INTEGER;
      else if (t->param->type == WE_REGISTRY_TYPE_STR)
        param_block_type = FRW_REG_BLOCK_TYPE_STRING;
      else
        param_block_type = (WE_UINT8) -1;

      if (param_block_type != type) {
        
        WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW,
                      "FRW_REG: Received SET with wrong type (%d), ignored. path=%s key=%s\n",
                      t->param->type, t->param->path, reg_log_str_null_filter(t->param->key)));
        frw_command_list_first->batch_element = NULL; 
        frw_reg_state = FRW_REG_STATE_START_READING_VALUE_HEADER;
        return 1;
      }
    }

    if (type == FRW_REG_BLOCK_TYPE_INTEGER)
      t->param->type = WE_REGISTRY_TYPE_INT;
    else
      t->param->type = WE_REGISTRY_TYPE_STR;
    



    if (size > (frw_reg_size - frw_reg_filepos)) {
      
      return 1;
    }
    if (type == FRW_REG_BLOCK_TYPE_INTEGER || type == FRW_REG_BLOCK_TYPE_STRING){
      frw_reg_pos = FRW_REG_BLOCK_HEADER_SIZE;
      while ((int)size > frw_reg_buflen) {
        frw_reg_buflen += 256;
        FRW_FREE (frw_reg_buf);
        frw_reg_buf = FRW_ALLOC (frw_reg_buflen);
        we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, frw_reg_buf, FRW_REG_BLOCK_HEADER_SIZE, 0);
        we_dcvt_uint32 (&cvt_obj, &size);
        we_dcvt_uint8 (&cvt_obj, &(type));
      }
      frw_reg_endpos = size;
      frw_reg_state = FRW_REG_STATE_READING_VALUE;

    case FRW_REG_STATE_READING_VALUE:
      if ((r = frw_reg_read_op ()) == 0) {
        return 0;
      }
      else if (r < 0) {
      
        return 1;
      }

      we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, frw_reg_buf, frw_reg_endpos,
                     WE_MODID_FRW);
      we_dcvt_change_pos (&cvt_obj, FRW_REG_BLOCK_HEADER_SIZE);
        
      if (t->param->type == WE_REGISTRY_TYPE_INT) {
        we_dcvt_int32 (&cvt_obj, &(value_int));
          
        if (frw_command_list_first->batch_element != NULL) {
          if (t->param->value_i == value_int)
            frw_command_list_first->batch_element = NULL; 
        }
        else
          t->param->value_i = value_int;
        t->param->value_bv_length = 0;
        t->param->value_bv = NULL;
      }
      else {
        we_dcvt_uint16 (&cvt_obj, &(bv_len));
          
        if (frw_command_list_first->batch_element != NULL) {
          if (t->param->value_bv_length == bv_len && memcmp (t->param->value_bv, (cvt_obj.data + cvt_obj.pos), bv_len) == 0)
            frw_command_list_first->batch_element = NULL; 
        }
        else {
          we_dcvt_uchar_vector (&cvt_obj, bv_len, &(t->param->value_bv));
          t->param->value_bv_length = bv_len;
        }
        t->param->value_i = 0;
      }
    }
    frw_reg_state = FRW_REG_STATE_START_READING_VALUE_HEADER;
  }
  return 1;
}

static int
frw_reg_read_file (void)
{
  frw_free_list_t     *p, *q;
  WE_UINT32          size;
  WE_UINT8           type;
  WE_UINT16          nr_of_elements;
  we_dcvt_t          cvt_obj;
  frw_reg_internal_t  *res;
  int                 r;
  WE_UINT32          m_size;

  for (;;) {
    switch (frw_reg_state) {
    case FRW_REG_STATE_START_READING_BLOCK_HEADER:
      if (frw_reg_filepos + FRW_REG_BLOCK_HEADER_SIZE >= frw_reg_size) {
        if (frw_read_elem_list != NULL)
            
          frw_reg_handle_init_err (TRUE);
        else
          frw_reg_set_state_open ();
        return 1;
      }
      frw_reg_pos = 0;
      frw_reg_endpos = FRW_REG_BLOCK_HEADER_SIZE;
      WE_FILE_SEEK (frw_reg_file, frw_reg_filepos, TPI_FILE_SEEK_SET);
      frw_reg_state = FRW_REG_STATE_READING_BLOCK_HEADER;

    case FRW_REG_STATE_READING_BLOCK_HEADER:
      if ((r = frw_reg_read_op ()) == 0) {
        return 0;
      }
      else if (r < 0) {
        frw_reg_handle_init_err (TRUE);
        return 1;
      }

      we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, frw_reg_buf, FRW_REG_BLOCK_HEADER_SIZE, 0);
      we_dcvt_uint32 (&cvt_obj, &size);
      we_dcvt_uint8 (&cvt_obj, &type);
      
      if (size == 0 || (size > (frw_reg_size - frw_reg_filepos))) {
        
        frw_reg_handle_init_err (TRUE);
        return 1;
      }
      if (type == FRW_REG_BLOCK_TYPE_FREE) {
        p = FRW_ALLOCTYPE (frw_free_list_t);
        p->next = NULL;
        p->size = size;
        p->pos = frw_reg_filepos;
        frw_reg_filepos += size;
        if (frw_free_list == NULL)
          frw_free_list = p;
        else {
          for (q = frw_free_list; q->next != NULL; q = q->next);
          q->next = p;
        }
      }
      else if (type == FRW_REG_BLOCK_TYPE_INTEGER || type == FRW_REG_BLOCK_TYPE_STRING) {
        frw_reg_put_value_into_tree (size, frw_reg_filepos);
        frw_reg_filepos += size;
      }
      else if (type == FRW_REG_BLOCK_TYPE_INTERNAL) {
        frw_reg_pos = FRW_REG_BLOCK_HEADER_SIZE;
        while ((int)size > frw_reg_buflen) {
          frw_reg_buflen += 256;
          FRW_FREE (frw_reg_buf);
          frw_reg_buf = FRW_ALLOC (frw_reg_buflen);
          we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, frw_reg_buf, FRW_REG_BLOCK_HEADER_SIZE, 0);
          we_dcvt_uint32 (&cvt_obj, &size);
          we_dcvt_uint8 (&cvt_obj, &type);
        }
        frw_reg_endpos = size;
        frw_reg_state = FRW_REG_STATE_READING_BLOCK;

      case FRW_REG_STATE_READING_BLOCK:
        if ((r = frw_reg_read_op ()) == 0) {
          return 0;
        }
        else if (r < 0) {
          frw_reg_handle_init_err (TRUE);
          return 1;
        }

        we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, frw_reg_buf, frw_reg_endpos,
                       WE_MODID_FRW);
        we_dcvt_change_pos (&cvt_obj, FRW_REG_BLOCK_HEADER_SIZE);
        we_dcvt_uint16 (&cvt_obj, &nr_of_elements);

        m_size = sizeof (frw_reg_internal_t) +
                 ((nr_of_elements > 0) ? (nr_of_elements - 1) : 0) * sizeof (frw_reg_element_t);
        res = FRW_ALLOC (m_size);
        memset(res, 0, m_size);
        res->pos = frw_reg_filepos;
        res->size = frw_reg_endpos;
        res->parent_elem = NULL;
        res->num_elements = nr_of_elements;
        if (res->pos == 0)
          frw_reg_list = res;

        if (!frw_reg_cvt_read_block (&cvt_obj, res)) {
          
          frw_reg_handle_init_err (TRUE);
          return 1;
        }

        frw_reg_filepos += frw_reg_endpos;
      }
      else {
        
        frw_reg_handle_init_err (TRUE);
        return 1;
      }
      frw_reg_state = FRW_REG_STATE_START_READING_BLOCK_HEADER;
    }
  }
}






static int
frw_reg_set (we_registry_param_t* param)
{
  frw_reg_internal_t  *q, *u;
  char                temp[FRW_REG_MAX_LENGTH_OF_PATH_KEY+2];
  char                *s;
  frw_reg_element_t   *element;

  if (!frw_reg_validate_path (param->path, 1) ||
      (!frw_reg_validate_key (param->key))) {
#ifdef FRW_REG_LOG_WRITE_END
    frw_reg_log_invalid_path_key(param->path, param->key);
#endif
    return FALSE;
  }

  if (!(param->type == WE_REGISTRY_TYPE_STR && param->value_bv_length > FRW_REG_MAX_LENGTH_OF_STRING_VALUE)) {
    temp[0] = 0;
    if (frw_reg_list->num_elements == 0)
      q = frw_reg_add_internal (&frw_reg_list, temp); 
    else 
      q = frw_reg_list->element[0]._u.p;
    s = param->path;
    while ((s = frw_get_sub_path (s, temp)) != NULL) {
      if ((u = frw_reg_get_internal (q, temp)) == NULL)
        u = frw_reg_add_internal (&q, temp);
      q = u;
    }
      
    if (q != NULL) {
      if ((element = frw_reg_get_value_element (q, param->key)) == NULL)
          frw_reg_add_value (&q, param);
      else {
        frw_reg_new_read_task (element->_u.value.pos, param);
        frw_command_list_first->batch_element = element;
        frw_reg_state = FRW_REG_STATE_START_READING_VALUE_HEADER;
        return TRUE;
      }
      frw_reg_write_elements ();
      frw_reg_state = FRW_REG_STATE_WRITING_VALUE;
      return TRUE;
    }
  }
  return FALSE;
}

static int
frw_reg_delete (void* p)
{
  we_registry_identifier_t* param = (we_registry_identifier_t*)p;
  frw_reg_internal_t         *q;
  char                       temp[FRW_REG_MAX_LENGTH_OF_PATH_KEY + 2];
  char                       *s;
  frw_registry_batch_t       *t;

  if (!frw_reg_validate_path (param->path, 0) ||
      (frw_reg_list->num_elements == 0)) {
#ifdef FRW_REG_LOG_WRITE_END
    frw_reg_log_invalid_path_key(param->path, param->key);
#endif
    return FALSE;
  }

  q = frw_reg_list->element[0]._u.p;
  s = param->path;
  while ((s = frw_get_sub_path (s, temp)) != NULL && q != NULL) {
    q = frw_reg_get_internal (q, temp);
  }

    
  if (q == NULL)
    return FALSE;

  if (param->key != NULL) {
    if (!frw_reg_delete_value (q, param->key))
      return FALSE;
    t = frw_reg_create_batch_struct (1);
    t->params[0].path = param->path;
    t->params[0].key = param->key;
    t->params[0].type = WE_REGISTRY_TYPE_DELETED;
    frw_reg_notify_subscribers(frw_command_list_first->src_modId, t,
                               0, t->nrOfParams);
    FRW_FREE (t->params);
    FRW_FREE (t);
  }
  else {
    int i, nr_of_keys, first_param = 1;

    nr_of_keys = 0;
    nr_of_keys = frw_reg_count_keys (q, nr_of_keys);
    if (nr_of_keys > 0) {
      t = frw_reg_create_batch_struct (nr_of_keys);

      frw_reg_create_delete_batch (q, param->path, t->params, &first_param);
      frw_reg_notify_subscribers(frw_command_list_first->src_modId, t,
                                 0, t->nrOfParams);
      for (i = 0; i < t->nrOfParams; i++) {
        FRW_FREE (t->params[i].path);
        FRW_FREE (t->params[i].key);
      }
      FRW_FREE (t->params);
      FRW_FREE (t);
    }

    frw_reg_delete_all_down_elements (q, 1, 1);
    frw_reg_delete_element (q);
  }
  return TRUE;

}

static void
frw_reg_insert_subscr_list_item(int                                src_modId,
                                const we_registry_subscription_t* subscription)
{
  frw_subscription_t* newSubscrItem;
  frw_subscribed_modules_t *prevModuleListPtr = NULL;   
  frw_subscribed_modules_t *moduleListPtr = frw_subscription_list;    

  newSubscrItem = FRW_ALLOCTYPE (frw_subscription_t);
  newSubscrItem->wid = subscription->wid;
  newSubscrItem->path = we_cmmn_strdup (WE_MODID_FRW, subscription->path);
  newSubscrItem->key = we_cmmn_strdup (WE_MODID_FRW, subscription->key);
  newSubscrItem->next = NULL;

  
  while(moduleListPtr != NULL && 
        moduleListPtr->moduleId != src_modId)
  {
    prevModuleListPtr = moduleListPtr;
    moduleListPtr = moduleListPtr->next;
  }

  if(moduleListPtr != NULL)
  {
    
    
    frw_subscription_t *subscrPtr  = moduleListPtr->subscriptions;
    frw_subscription_t *prevPtr = NULL;
    
    while((subscrPtr != NULL) && (subscrPtr->wid <= subscription->wid))
    {
      prevPtr = subscrPtr;
      subscrPtr = subscrPtr->next;
    }

    if(prevPtr != NULL)
    {
      prevPtr->next = newSubscrItem;
    }
    else
    {
      moduleListPtr->subscriptions = newSubscrItem;
    }
    newSubscrItem->next = subscrPtr;
  }
  else
  {
    
    frw_subscribed_modules_t *newModuleItem = FRW_ALLOCTYPE (frw_subscribed_modules_t);
    newModuleItem->moduleId = src_modId;
    newModuleItem->next = NULL;
    newModuleItem->subscriptions = newSubscrItem;
    
    if(prevModuleListPtr != NULL)
    {
      prevModuleListPtr->next = newModuleItem;
    }
    else
    {
      frw_subscription_list = newModuleItem;
    }
    if (src_modId != WE_MODID_FRW)
    {
      frw_register_mod_term_notif((WE_UINT8) src_modId, FRW_SIG_DST_FRW_REGISTRY,
                                  FRW_REGISTRY_SIG_MOD_TERMINATED);
    }
  }

}

static void frw_reg_free_subscr_list(void)
{
  frw_subscribed_modules_t *moduleListPtr = frw_subscription_list;
  frw_subscribed_modules_t *nextPtr;

  while(moduleListPtr != NULL)
  {     
    frw_subscription_t *ptr  = moduleListPtr->subscriptions;

    while(ptr != NULL)
    {
      frw_subscription_t* tmp = ptr;
      ptr = ptr->next;
      FRW_FREE (tmp->key);
      FRW_FREE (tmp->path);
      FRW_FREE (tmp);
    }

    nextPtr = moduleListPtr->next;
    FRW_FREE(moduleListPtr);
    moduleListPtr = nextPtr;
  }
  frw_subscription_list = NULL;
}

static void
frw_reg_remove_module_subscr(WE_UINT8 modid)
{
  frw_subscribed_modules_t *moduleListPtr = frw_subscription_list;
  frw_subscribed_modules_t *prevPtr = NULL;

  while((moduleListPtr != NULL) && (moduleListPtr->moduleId != modid))
  {
    prevPtr = moduleListPtr;
    moduleListPtr = moduleListPtr->next;
  }

  if (moduleListPtr != NULL)
  {
    frw_subscription_t *ptr  = moduleListPtr->subscriptions;

    while(ptr != NULL)
    {
      frw_subscription_t* tmp = ptr;
      ptr = ptr->next;
      FRW_FREE (tmp->key);
      FRW_FREE (tmp->path);
      FRW_FREE (tmp);
    }

    if (prevPtr != NULL)
      prevPtr->next = moduleListPtr->next;
    else
      frw_subscription_list = moduleListPtr->next;

    FRW_FREE(moduleListPtr);
  }
}

static void
frw_reg_remove_subscr_list_item(frw_subscribed_modules_t* moduleListPtr,
                                frw_subscription_t*       subscrElement)
{
  if(moduleListPtr != NULL)
  {
    frw_subscription_t *prevPtr = NULL;
    frw_subscription_t *ptr  = moduleListPtr->subscriptions;

    while(ptr != NULL && ptr != subscrElement)
    {
      prevPtr = ptr;
      ptr = ptr->next;
    }

    if(ptr != NULL)
    {
      
      if(prevPtr != NULL)
      {
        prevPtr->next = subscrElement->next;
      }
      else
      {
        moduleListPtr->subscriptions = subscrElement->next;
      }

      FRW_FREE (subscrElement->key);
      FRW_FREE (subscrElement->path);
      FRW_FREE (subscrElement);
    }
  }
}

static int
frw_reg_subscribe (int src_modId, void* p)
{
  WE_BOOL                    itemHasBeenRemoved = FALSE;
  we_registry_subscription_t *param = (we_registry_subscription_t*)p;
  frw_subscribed_modules_t    *moduleListPtr = frw_subscription_list;

  if (param->add || param->path) 
    if (!frw_reg_validate_path (param->path, 0))
      return FALSE;

  while (moduleListPtr != NULL && 
         moduleListPtr->moduleId != src_modId)
  {
    moduleListPtr = moduleListPtr->next;
  }

  if (moduleListPtr != NULL)
  {
    
    
    
    frw_subscription_t *subscrPtr = moduleListPtr->subscriptions;
    while (subscrPtr != NULL)
    {
      WE_BOOL removeItem = FALSE;

      if (subscrPtr->wid == param->wid)
      {
        if (!param->add && (param->path == NULL)) {
          removeItem = TRUE;
        }
        else if (strlen(subscrPtr->path) < strlen(param->path))
        {
          unsigned subscrPathLength = strlen(subscrPtr->path);
          if (we_cmmn_strncmp_nc(subscrPtr->path, param->path, subscrPathLength) == 0)
          {
            if (param->path[subscrPathLength] == '/' && param->add)
            {
              
              removeItem = TRUE;
            }
          }     
        }
        else
        {
          if (we_cmmn_strncmp_nc(subscrPtr->path, param->path, strlen(param->path)) == 0)
          {
            if (strlen(subscrPtr->path) == strlen(param->path))
            {
              
              if (subscrPtr->key != NULL && param->key != NULL)
              {
                
                if (we_cmmn_strcmp_nc(subscrPtr->key, param->key) == 0)
                {
                  removeItem = TRUE;                
                }
              }
              else
              {
                removeItem = TRUE;
              }
            }
            else if(subscrPtr->path[strlen(param->path)] == '/')
            {
              
              removeItem = TRUE;
            }
          }
        }
      } 

      if (removeItem)
      {
        frw_subscription_t* deleteItemPtr = subscrPtr;
        subscrPtr = subscrPtr->next;
        frw_reg_remove_subscr_list_item(moduleListPtr, deleteItemPtr);
        itemHasBeenRemoved = TRUE;
      }
      else
      {
        subscrPtr = subscrPtr->next;
      }
    } 
  } 

  if (param->add)
  {
    
    frw_reg_insert_subscr_list_item(src_modId, param);
  }
  else
  {
    if(!itemHasBeenRemoved)
    {
      return FALSE;
    }
  }

  return TRUE;
}

static int
frw_reg_get (void** p)
{
  we_registry_identifier_t* param = (we_registry_identifier_t*)*p;
  frw_reg_internal_t         *q;
  char                       temp[FRW_REG_MAX_LENGTH_OF_PATH_KEY + 2];
  char                       *s;
  frw_registry_batch_t       *t;
  int                        nr_of_keys;
  frw_reg_element_t          *element;

  if (!frw_reg_validate_path (param->path, 0))
    return FALSE;

  if (frw_reg_list->num_elements == 0)
    return FALSE;

  q = frw_reg_list->element[0]._u.p;
  s = param->path;
  while ((s = frw_get_sub_path (s, temp)) != NULL && q != NULL)
    q = frw_reg_get_internal (q, temp);

    
  if (q == NULL)
    return FALSE;

  if (param->key != NULL) {
    if ((element = frw_reg_get_value_element (q, param->key)) != NULL) {
      t = frw_reg_create_batch_struct (1);
      t->params[0].path = param->path;
      t->params[0].key = param->key;
      *p = t;
      FRW_FREE (param);
      frw_reg_new_read_task (element->_u.value.pos, t->params);
    }
    else
      return FALSE;
  }
  else {
    int first_param = 1;

    nr_of_keys = 0;
    nr_of_keys = frw_reg_count_keys (q, nr_of_keys);
    t = frw_reg_create_batch_struct (nr_of_keys);
    *p = t;
    frw_reg_create_batch_tasks (q, param->path, t->params, &first_param);
    FRW_FREE (param->path);
    FRW_FREE (param);
  }
  return TRUE;
}






static void
frw_reg_create_new_reg_file (void)
{
  frw_reg_internal_t *p;
  frw_write_task_t   *t;
  we_dcvt_t          cvt_obj;
  WE_UINT32          size;
  WE_UINT8           type;
  WE_UINT16          nr_of_elements;

  p = FRW_ALLOCTYPE (frw_reg_internal_t); 
  p->pos = 0;
  p->num_elements = 0;
  p->parent_elem = NULL;
  frw_reg_list = p;

  





  frw_reg_list->size = size = FRW_REG_MRB_SIZE; 
  type = FRW_REG_BLOCK_TYPE_INTERNAL;
  nr_of_elements = 0;
  t = frw_reg_new_write_task (0, FRW_REG_MRB_SIZE + 5);  
  memset(t->buf, 0, FRW_REG_MRB_SIZE + 5);
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, t->buf, FRW_REG_MRB_SIZE + 5, WE_MODID_FRW);
  we_dcvt_uint32 (&cvt_obj, &size);
  we_dcvt_uint8 (&cvt_obj, &type);
  we_dcvt_uint16 (&cvt_obj, &nr_of_elements);
    
  we_dcvt_change_pos (&cvt_obj, size - cvt_obj.pos);
  size = frw_reg_size - size; 
  type = FRW_REG_BLOCK_TYPE_FREE;
  we_dcvt_uint32 (&cvt_obj, &size);
  we_dcvt_uint8 (&cvt_obj, &type);
  frw_reg_state = FRW_REG_STATE_WRITING_VALUE_ROOT;

  frw_free_list = FRW_ALLOCTYPE (frw_free_list_t);
  frw_free_list->next = NULL;
  frw_free_list->pos = FRW_REG_MRB_SIZE;
  frw_free_list->size = frw_reg_size - FRW_REG_MRB_SIZE;
}

static int
frw_reg_create_an_empty_file (void)
{
  int i;

  i = WE_FILE_MKDIR (FRW_REG_PATHNAME);
  if (i != TPI_FILE_OK && i != TPI_FILE_ERROR_EXIST) {
    
    frw_reg_buflen = 0;
    frw_reg_enabled = 0;
    FRW_FREE (frw_reg_buf);
    TPIa_error (WE_MODID_FRW, WE_ERR_SYSTEM_REG_ABORTED);
    return FALSE;
  }
  frw_reg_size = FRW_REG_FILE_INITIAL_SIZE;
  frw_reg_file = WE_FILE_OPEN (WE_MODID_FRW, FRW_REG_FILENAME, WE_FILE_SET_RDWR | TPI_FILE_SET_CREATE, frw_reg_size);
  if (frw_reg_file < 0) {
    
    frw_reg_buflen = 0;
    frw_reg_enabled = 0;
    FRW_FREE (frw_reg_buf);
    TPIa_error (WE_MODID_FRW, WE_ERR_SYSTEM_REG_ABORTED);
    return FALSE;
  }
  frw_register_file_handle (frw_reg_file, FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
  frw_reg_create_new_reg_file ();

  return TRUE;
}







static int
frw_reg_handle_init_err (int reg_file_is_open)
{
  
  if (reg_file_is_open) {
    frw_reg_abort_startup ();
  }
  else {
    WE_FILE_REMOVE (FRW_REG_FILENAME);
    if (!frw_reg_create_an_empty_file ()) {
      if (!frw_reg_init_done)
        FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_SUB_MOD_INITIALISED);
      frw_reg_init_done = TRUE;
      frw_reg_init_err_handled = TRUE;
      return FALSE;
    }
  }

  if (!frw_reg_init_err_handled && frw_reg_def_file_name) {
    
    WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: Load registry from text file: %s\n",
                  reg_log_str_null_filter(frw_reg_def_file_name)));
    frw_reg_state = FRW_REG_STATE_WRITING_VALUE_LOAD_TXT;
    FRW_FREE (frw_reg_txt_file_name);
    frw_reg_txt_file_name = frw_reg_def_file_name;
    frw_reg_def_file_name = NULL;
  }

  frw_reg_init_err_handled = TRUE;
  return TRUE;
}

void
frw_reg_init (const char* startup_file, const char* default_file)
{
  frw_reg_list = NULL;
  frw_write_task_list = NULL;
  frw_free_list = NULL;
  frw_command_list_first = NULL;
  frw_command_list_last = NULL;

  frw_write_elem_list = NULL;
  frw_read_elem_list = NULL;
  frw_read_task_list = NULL;
  frw_reg_enabled = 1;

  frw_reg_int_set_handle = NULL;
  frw_reg_int_response = NULL;
  frw_reg_txt_file_name = NULL;
  frw_reg_txt_buffer = NULL;
  frw_reg_init_done = FALSE;
  frw_reg_def_file_name = NULL;

  frw_reg_init_err_handled = FALSE;
  frw_reg_closing = FALSE;
  frw_reg_sequential_io = 0;
  frw_subscription_list = NULL;

  
  frw_reg_buflen = 100;
  frw_reg_buf = FRW_ALLOC (frw_reg_buflen);
  
  frw_signal_register_dst (FRW_SIG_DST_FRW_REGISTRY, frw_reg_main);

    
  if (startup_file != NULL) {
    WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: Load registry from text file: %s\n", reg_log_str_null_filter(startup_file)));
    WE_FILE_REMOVE (FRW_REG_FILENAME);
    if (frw_reg_create_an_empty_file () == FALSE) {
      return;
    }
    frw_sub_module_init_delay ();
    frw_reg_state = FRW_REG_STATE_WRITING_VALUE_LOAD_TXT;
    frw_reg_txt_file_name = we_cmmn_strdup (WE_MODID_FRW, startup_file);  
  }
  else {
    
    frw_sub_module_init_delay ();
    frw_reg_def_file_name = we_cmmn_strdup (WE_MODID_FRW, default_file);
    frw_reg_file = WE_FILE_OPEN (WE_MODID_FRW, FRW_REG_FILENAME, TPI_FILE_SET_RDWR, 0);
    if (frw_reg_file < 0) {
      if (!frw_reg_handle_init_err (FALSE)) {
        return;
      }
    }
    else {
      WE_INT32 tempSize;
      frw_register_file_handle (frw_reg_file, FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);

      tempSize = WE_FILE_SEEK (frw_reg_file, 0, TPI_FILE_SEEK_END);
      WE_FILE_SEEK (frw_reg_file, 0, TPI_FILE_SEEK_SET);
      if (tempSize < FRW_REG_MRB_SIZE) {
        frw_reg_size = FRW_REG_FILE_INITIAL_SIZE;
        frw_reg_create_new_reg_file ();
        WE_FILE_SETSIZE (frw_reg_file, frw_reg_size);
      }
      else {
        frw_reg_size = (WE_UINT32)tempSize;
        frw_reg_filepos = 0;
        frw_reg_state = FRW_REG_STATE_START_READING_BLOCK_HEADER;
      }
    }
  }
  FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
}

static void
frw_reg_remove_tree_structure (void)
{
  frw_reg_delete_all_down_elements (frw_reg_list, 0, 0);
}

static void
frw_reg_complete_terminate (void)
{
  frw_free_list_t  *p;

  frw_reg_free_subscr_list();
  WE_FILE_CLOSE (frw_reg_file);
  frw_deregister_file_handle (frw_reg_file);
  FRW_FREE (frw_reg_buf);
  frw_reg_buf = NULL;
  frw_reg_remove_tree_structure ();
  if (frw_reg_int_set_handle != NULL) {
    we_registry_set_free_handle (WE_MODID_FRW, frw_reg_int_set_handle);
    frw_reg_int_set_handle = NULL;
  }
  if (frw_reg_int_response != NULL)
    frw_reg_int_get_free_all ();

  while (frw_free_list != NULL) {
    p = frw_free_list;
    frw_free_list = p->next;
    FRW_FREE (p);
  }

  FRW_FREE (frw_reg_def_file_name);
  frw_reg_def_file_name = NULL;

  if (frw_reg_closing)
    FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_SUB_MOD_TERMINATED);
  frw_reg_enabled = FALSE;
  frw_reg_closing = FALSE;
}

void
frw_reg_terminate (void)
{
  if (!frw_reg_enabled)
    return;

  frw_reg_free_subscr_list();

  if ((frw_reg_state == FRW_REG_STATE_OPEN) && (frw_command_list_first == NULL)) {
    
    frw_reg_complete_terminate();
  }
  else {
    


    frw_reg_closing = TRUE;
    frw_sub_module_terminate_delay ();
  }
}

static void
frw_reg_abort_startup (void)
{
  frw_free_list_t     *p;
  frw_read_element_t  *s;

  frw_reg_free_subscr_list();
  frw_reg_remove_tree_structure ();

  while (frw_free_list != NULL) {
    p = frw_free_list;
    frw_free_list = p->next;
    FRW_FREE (p);
  }
  while (frw_read_elem_list != NULL) {
    s = frw_read_elem_list;
    frw_read_elem_list = s->next;
    FRW_FREE (s);
    
  }
  frw_reg_size = FRW_REG_FILE_INITIAL_SIZE;
  frw_reg_create_new_reg_file ();
  WE_FILE_SETSIZE (frw_reg_file, frw_reg_size);
  WE_ERROR (WE_MODID_FRW, WE_ERR_SYSTEM_REG_FILE_CORRUPT);
}

static void
frw_reg_process_task (void)
{
  we_dcvt_t cvt_obj;

  if (frw_command_list_first == NULL)
    return;

  switch (frw_command_list_first->sig_num) {
    case FRW_REGISTRY_SIG_SET:
      frw_command_list_first->batch_index = 0;
      if (!frw_reg_batch_execute_next ()) {
        we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
        we_cvt_registry_batch (&cvt_obj, frw_command_list_first->sig_data);
        frw_reg_delete_command ();
      }
      else
        FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
      break;
    case FRW_REGISTRY_SIG_DELETE:
      if (!frw_reg_delete (frw_command_list_first->sig_data)) {
#ifdef FRW_REG_LOG_WRITE_END
        frw_reg_log_task_reg_id("finished DELETE, FAILED", frw_command_list_first);
#endif
        we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
        we_cvt_registry_identifier (&cvt_obj, frw_command_list_first->sig_data);
        frw_reg_delete_command ();
      }
      else {
        frw_reg_state = FRW_REG_STATE_WRITING_VALUE;
        FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
      }
      break;
    case FRW_REGISTRY_SIG_SUBSCRIBE:
      if (!frw_reg_subscribe (frw_command_list_first->src_modId, frw_command_list_first->sig_data)) {
        WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW_REG: failed to subscribe/unsubscribe to registry\n"));
      }
      frw_reg_finish_command ();  
      break;
    case FRW_REGISTRY_SIG_GET:
      if (!frw_reg_get (&(frw_command_list_first->sig_data))) {
        frw_reg_generate_empty_response (frw_command_list_first->src_modId);
        we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
        we_cvt_registry_identifier (&cvt_obj, frw_command_list_first->sig_data);
        frw_reg_delete_command ();
      }
      else {
        frw_reg_state = FRW_REG_STATE_START_READING_VALUE_HEADER;
        FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
      }
      break;
  }
}

static void
frw_reg_run (void)
{
  frw_write_task_t *tk;
  frw_read_task_t  *r;

  while ((frw_reg_state == FRW_REG_STATE_START_READING_BLOCK_HEADER) ||
         (frw_reg_state == FRW_REG_STATE_READING_BLOCK_HEADER) ||
         (frw_reg_state == FRW_REG_STATE_READING_BLOCK)) {
    if (!frw_reg_read_file ())
      return;
  }

  if (frw_reg_state == FRW_REG_STATE_OPEN || frw_reg_state == FRW_REG_STATE_LOAD_TXT_OPEN)
    frw_reg_process_task ();
  else if (frw_reg_state == FRW_REG_STATE_WRITING_VALUE ||
           frw_reg_state == FRW_REG_STATE_WRITING_VALUE_LOAD_TXT  ||
           frw_reg_state == FRW_REG_STATE_WRITING_VALUE_ROOT) {
    while (frw_write_task_list != NULL) {
      tk = frw_write_task_list;

      if (!frw_reg_write_op (tk)) {
        return;
      }
      frw_write_task_list = tk->next;
      FRW_FREE (tk->buf);
      FRW_FREE (tk);
    }
    if (frw_reg_state == FRW_REG_STATE_WRITING_VALUE) 
      frw_reg_finish_command ();
    else if (frw_reg_state == FRW_REG_STATE_WRITING_VALUE_ROOT) {
      frw_reg_set_state_open ();
      frw_reg_process_task (); 
    }
    else
      frw_reg_txt_load ();
  }
  else if ((frw_reg_state == FRW_REG_STATE_START_READING_VALUE_HEADER) ||
           (frw_reg_state == FRW_REG_STATE_READING_VALUE_HEADER) ||
           (frw_reg_state == FRW_REG_STATE_READING_VALUE)) {
    while (frw_read_task_list != NULL) {
      r = frw_read_task_list;

      if (!frw_reg_read_value (r)) {
        return;
      }
      frw_read_task_list = r->next;
      FRW_FREE (r);
    }
    frw_reg_finish_command ();
  }
  else if (frw_reg_state == FRW_REG_STATE_LOAD_TXT_READ)
    frw_reg_txt_read ();
  else if (frw_reg_state == FRW_REG_STATE_LOAD_TXT_PARSE)
    frw_reg_txt_parse ();
}

static void
frw_reg_release_signal (frw_signal_t *sig)
{
  if (sig->type == FRW_REGISTRY_SIG_SET)
    WE_REGISTRY_RESPONSE_FREE (WE_MODID_FRW, sig->p_param);
  else if (sig->type == FRW_REGISTRY_SIG_DELETE || (sig->type == FRW_REGISTRY_SIG_GET)) {
    we_dcvt_t  cvt_obj;

    we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
    we_cvt_registry_identifier (&cvt_obj, sig->p_param);
    FRW_FREE (sig->p_param);
  }
  else if (sig->type == FRW_REGISTRY_SIG_SUBSCRIBE ) {
    we_dcvt_t  cvt_obj;

    we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
    (void)we_cvt_registry_subscription (&cvt_obj, sig->p_param);
    FRW_FREE (sig->p_param);
  }
}

static void
frw_reg_mod_terminated (WE_UINT8 modid)
{
  frw_reg_remove_module_subscr(modid);
}






static void
frw_reg_main (frw_signal_t *sig)
{
  if (!frw_reg_enabled) {
    frw_reg_release_signal (sig);
    frw_signal_delete (sig);
    return;
  }
  if (sig->type >= FRW_REGISTRY_SIG_SET && sig->type <= FRW_REGISTRY_SIG_GET) {
    switch (sig->type) {
      case FRW_REGISTRY_SIG_SET:
        WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_REG: received signal FRW_REGISTRY_SIG_SET\n"));
        break;
      case FRW_REGISTRY_SIG_DELETE:
        WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_REG: received signal FRW_REGISTRY_SIG_DELETE\n"));
        break;
      case FRW_REGISTRY_SIG_SUBSCRIBE:
        WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_REG: received signal FRW_REGISTRY_SIG_SUBSCRIBE\n"));
        break;
      case FRW_REGISTRY_SIG_GET:
        WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_REG: received signal FRW_REGISTRY_SIG_GET\n"));
        break;
    }
    frw_reg_store_command (sig);
    if (frw_reg_state == FRW_REG_STATE_OPEN)
      frw_reg_run ();
  }
  else if (sig->type == FRW_REGISTRY_SIG_RUN) {
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_REG: received signal FRW_REGISTRY_RUN\n"));
    frw_reg_run ();
  }
  else if (sig->type == FRW_REGISTRY_SIG_MOD_TERMINATED) {
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_REG: received signal FRW_REGISTRY_SIG_MOD_TERMINATED\n"));
    frw_reg_mod_terminated ((WE_UINT8) sig->i_param);
  }
  else
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_REG: received unknown signal: %d\n", sig->type));

  frw_signal_delete (sig);
}






void
frw_reg_int_set_init (void)
{
  if (!frw_reg_enabled)
    return;
  if (frw_reg_int_set_handle != NULL)
    we_registry_set_free_handle (WE_MODID_FRW, frw_reg_int_set_handle);

  frw_reg_int_set_handle = WE_REGISTRY_SET_INIT (WE_MODID_FRW);
}

void
frw_reg_int_set_path (char* path)
{
  if (!frw_reg_enabled)
    return;
  WE_REGISTRY_SET_PATH (frw_reg_int_set_handle, path);
}

void
frw_reg_int_set_add_key_int (char* key, WE_INT32 value)
{
  if (!frw_reg_enabled)
    return;
  WE_REGISTRY_SET_ADD_KEY_INT (frw_reg_int_set_handle, key, value);
}

void
frw_reg_int_set_add_key_str (char* key, unsigned char* value, WE_UINT16 valueLength)
{
  if (!frw_reg_enabled)
    return;
  WE_REGISTRY_SET_ADD_KEY_STR (frw_reg_int_set_handle, key, value, valueLength);
}

void
frw_reg_int_set_commit (void)
{
  void* p;

  if (!frw_reg_enabled)
    return;
  p = we_registry_set_int_commit (frw_reg_int_set_handle);
  if (p != NULL)
    FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_SET, WE_MODID_FRW, p);
  frw_reg_int_set_handle = NULL;
}

int
frw_reg_int_resp_get_next (WE_UINT8 wid, we_registry_param_t* param)
{
  frw_reg_resp_integr_t*  p;

  if (!frw_reg_enabled)
    return FALSE;
  p = frw_reg_int_response;
  while (p != NULL) {
    if (wid == p->resp_id)
      return WE_REGISTRY_RESPONSE_GET_NEXT (p->repsonse, param);
    p = p->next;
  }
  return FALSE;
}

void
frw_reg_int_resp_free (WE_UINT8 wid)
{
  frw_reg_resp_integr_t *p, *q;

  if (!frw_reg_enabled)
    return;
  p = frw_reg_int_response;
  q = NULL;
  while (p != NULL) {
    if (wid == p->resp_id) {
      if (q == NULL)
        frw_reg_int_response = p->next;
      else
        q->next = p->next;
      WE_REGISTRY_RESPONSE_FREE (WE_MODID_FRW, p->repsonse);
      FRW_FREE (p);
      break;
    }
    q = p;
    p = p->next;
  }
}






#define FRW_REG_TIME_SLOT                   3000
#define FRW_REG_IDENTIFIER_LENGTH           128

#define FRW_REG_TOKEN_LEFT_WBACKET          101
#define FRW_REG_TOKEN_EOF                   102
#define FRW_REG_TOKEN_EQUAL                 103
#define FRW_REG_TOKEN_KEY                   104

#define FRW_REG_TOKEN_ERROR                 199

#define FRW_REG_STATUS_FINISHED             0
#define FRW_REG_STATUS_ERROR               -1
#define FRW_REG_STATUS_PAUSE                1


typedef struct frw_reg_txt_parser_st {
  char*           buf;
  char*           s;
  int             token_type;
  char            token_ident[FRW_REG_IDENTIFIER_LENGTH + 1];
  int             value_type;
  WE_INT32       value_i;
  unsigned char*  value_bv;
} frw_reg_txt_parser_t;


typedef struct frw_reg_txt_key_st {
  struct frw_reg_txt_key_st *next;

  char*           name;
  int             type;
  WE_INT32       value_i;
  unsigned char*  value_bv;
} frw_reg_txt_key_t;

typedef struct frw_reg_txt_path_st {
  struct frw_reg_txt_path_st *next;

  char*               name;
  frw_reg_txt_key_t*  keys;
} frw_reg_txt_path_t;





static void *
frw_reg_txt_parse_init (char* reg_txt, int length)
{
  frw_reg_txt_parser_t *parser;

  if ((reg_txt == NULL) || (length == 0)) {
    FRW_FREE (reg_txt);
    return NULL;
  }

  parser = FRW_ALLOCTYPE (frw_reg_txt_parser_t);
  parser->buf = reg_txt;
  parser->s = reg_txt;

  return (void *)parser;
}





static void
frw_reg_txt_parse_terminate (void *reg_txt_handle)
{
  FRW_FREE (((frw_reg_txt_parser_t *)reg_txt_handle)->buf);
  
  FRW_FREE (reg_txt_handle);
}




static void
frw_reg_txt_skip_whitespace (char **str)
{
  char *s = *str;

  while (ct_iswhitespace (*s)) {
    s++;
  }

  *str = s;
}




static void
frw_reg_txt_skip_blank (char **str)
{
  char *s = *str;

  while (ct_isblank (*s)) {
    s++;
  }

  *str = s;
}




static int
frw_reg_txt_read_path (char **str, char *p)
{
  char *s;
  int   len = 0;

  for (s = *str; *s != '\0';) {
    if (ct_isalphanum (*s) || (*s == '/') || (*s == '_')) {
      
      if (len < FRW_REG_IDENTIFIER_LENGTH) {
        *p++ = *s;
        len++;
      }
      s++;
    }
    else if (*s == ']') {
      
      s++;
      break;
    }
    else
      return -1;
  }
  *p = '\0';
  *str = s;

  return len;
}




static int
frw_reg_txt_read_key (char **str, char *p)
{
  char *s, *t;
  int   len = 0;

  t = p;
  for (s = *str; *s != '\0';) {
    if (ct_isalphanum (*s) || (*s == '_') || (*s == '-') || (*s == ' ')) {
      if (len < FRW_REG_IDENTIFIER_LENGTH) {
        *p++ = *s;
        len++;
      }
      s++;
    }
    else if (*s == '=') {
      
      break;
    }
    else {
      *str = s;
      return -1;
    }
  }
  *p = '\0';
  len = we_cmmn_skip_trailing_blanks (t, len);
  t[len] = '\0';

  *str = s;

  return len;
}




static int
frw_reg_txt_read_string_value (char **str, char *buffer)
{
  char *s, *p;
  int   len = 0;

  p = buffer;
  for (s = *str; *s != '\0';) {
    if (*s == '"') {
      
      s++;
      break;
    }
    else if (*s == '\\') {
      
      s++;
      if (ct_ishex (*s)) {
        unsigned char  b[1];
        if (we_cmmn_hex2byte (s, b)) {
          if (buffer != NULL)
            *p++ = *b;
          len++;
          s += 2;
        }
        else
          s++;
      }
      else if ((*s == '\n') || (*s == '\f')) {
        s++;
      }
      else if (*s == '\r') {
        s++;
        if (*s == '\n')
          s++;
      }
      else if ((*s >= ' ') && (*s != 0x7f)) {
        
        if (buffer != NULL)
          *p++ = *s;
        len++;
        s++;
      }
    }

    else if ((((unsigned char)*s >= ' ') && (*s != 0x7f)) || (*s == 0x09)) {
      
      if (buffer != NULL)
        *p++ = *s;
      len++;
      s++;
    }
    else {
      

      len = -1;
      break;
    }
  }
  if (buffer != NULL)
    *p = '\0';
  *str = s;

  return len;
}




static int
frw_reg_txt_read_int_value (char **str, WE_INT32* value)
{
  char *s, *t;
  int   ret = FALSE;

  s = *str;
  frw_reg_txt_skip_blank (&s);

  if (ct_isdigit (*s) || *s == '-') {
    t = s;
    s++;
    for (; *s != '\0'; s++) {
      if (!ct_isdigit (*s))
        break;
    }
    if (!(s - t == 1 && *t == '-')) {
      *value = atol (t);
      ret = TRUE;
    }
  }

  *str = s;
  return ret;
}




static int
frw_reg_txt_read_hex_value (char **str, unsigned char* buffer)
{
  char          *s;
  unsigned char b[1];
  unsigned char *p;
  int           len = 0;

  p = buffer;
  frw_reg_txt_skip_blank (str);
  for (s = *str; *s != '\0';) {
    if (*s == '\r' || *s == '\n' || *s == '\f') {
      s++;
      break;
    }
    if (ct_ishex (*s)) {
      if (we_cmmn_hex2byte (s, b)) {
        if (buffer != NULL)
          *p++ = *b;
        len++;
        s += 2;
      }
      else {
        s++;
        len = -1;
        break;
      }
    }
    else if (*s == ',' || *s == ' ')
      s++;
    else if (*s == '\\') {
      s++;
      frw_reg_txt_skip_whitespace (&s);
    }
    else {
      len = -1;
      break;
    }
  }
  *str = s;

  return len;
}




static int
frw_reg_txt_read_value (char **str, frw_reg_txt_parser_t *parser)
{
  char *s, *t;
  int   len = 0;
  int   ret = FALSE;

  s = *str;
  parser->value_type = -1;
  parser->value_bv = NULL;
  parser->value_i = 0;
  frw_reg_txt_skip_blank (&s);
  if (*s == '"') {
    s++;
    t = s;
    len = frw_reg_txt_read_string_value (&t, NULL);
    if (len > 0) {
      parser->value_bv = FRW_ALLOC (len + 1);
      parser->value_i = len + 1;
      parser->value_type = WE_REGISTRY_TYPE_STR;
      frw_reg_txt_read_string_value (&s, (char*)(parser->value_bv));
      ret = TRUE;
    }
  }
  else if (we_cmmn_strncmp_nc (s, "int:", 4) == 0) {
    s += 4;
    if (frw_reg_txt_read_int_value (&s, &(parser->value_i)) == TRUE) {
      parser->value_type = WE_REGISTRY_TYPE_INT;
      ret = TRUE;
    }
  }
  else if (we_cmmn_strncmp_nc (s, "hex:", 4) == 0) {
    s += 4;
    t = s;
    len = frw_reg_txt_read_hex_value (&t, NULL);
    if (len > 0) {
      parser->value_bv = FRW_ALLOC (len);
      parser->value_i = len;
      parser->value_type = WE_REGISTRY_TYPE_STR;
      frw_reg_txt_read_hex_value (&s, parser->value_bv);
      ret = TRUE;
    }
  }
  else
    ret = FALSE;

  *str = s;

  return ret;
}




static void
frw_reg_txt_get_token (frw_reg_txt_parser_t *parser)
{
  char *s;
  char  c;
  int   token_type = FRW_REG_TOKEN_EOF;

  s = parser->s;

try_again:
  c = *s;
  switch (c) {
    case '\0':
      token_type = FRW_REG_TOKEN_EOF;
      break;

    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case '\f':
      frw_reg_txt_skip_whitespace (&s);
      goto try_again;
        

    case '#':
      
      do
        s++;
      while (*s && *s != '\n');
      goto try_again;

    case '=':
      token_type = FRW_REG_TOKEN_EQUAL;
      s++;
      if (!frw_reg_txt_read_value (&s, parser))
        token_type = FRW_REG_TOKEN_ERROR;
      break;

    case '[':
      s++;
      token_type = FRW_REG_TOKEN_LEFT_WBACKET;
      if (frw_reg_txt_read_path (&s, parser->token_ident) < 0)
        token_type = FRW_REG_TOKEN_ERROR;
      break;

    default:
      if (ct_isalphanum (c) || c == '_' || c == '-') {
        token_type = FRW_REG_TOKEN_KEY;
        if (frw_reg_txt_read_key (&s, parser->token_ident) < 0)
          token_type = FRW_REG_TOKEN_ERROR;
      }
      else
        token_type = FRW_REG_TOKEN_ERROR;
      break;

  }

  parser->s = s;
  parser->token_type = token_type;
}




static void
frw_reg_txt_delete_keys (frw_reg_txt_path_t* result, int parse_error)
{
  frw_reg_txt_path_t* path;
  frw_reg_txt_key_t*  key;

  while (result != NULL) {
    path = result;
    key = path->keys;
    while (path->keys != NULL) {
      key = path->keys;
      if (parse_error == 1) {
        FRW_FREE (key->name);
        FRW_FREE (key->value_bv);
      }
      else {
        if (key->type == -1)
          FRW_FREE (key->name);
      }
      path->keys = key->next;
      FRW_FREE (key);
    }
    FRW_FREE (path->name);
    result = path->next;
    FRW_FREE (path);
  }
}




static int
frw_reg_txt_parse_reg (void *reg_txt_handle, frw_reg_txt_path_t **result)
{
  frw_reg_txt_parser_t  *parser;
  frw_reg_txt_path_t    *last, *path;
  frw_reg_txt_key_t     *key, *tmp_key;
  

  *result = last = path = NULL;
  parser = (frw_reg_txt_parser_t*)reg_txt_handle;
   

  for (;;) {
    frw_reg_txt_get_token (parser);
    if (parser->token_type == FRW_REG_TOKEN_EOF)
      return FRW_REG_STATUS_FINISHED;

    switch (parser->token_type) {
    case FRW_REG_TOKEN_LEFT_WBACKET:
      if (parser->token_ident != NULL) {
        if (last != NULL && last->keys == NULL) {
          FRW_FREE (last->name);
          last->name = we_cmmn_strdup (WE_MODID_FRW, parser->token_ident);
        }
        else {
          path = FRW_ALLOCTYPE (frw_reg_txt_path_t);
          path->name = we_cmmn_strdup (WE_MODID_FRW, parser->token_ident);
          path->keys = NULL;
          path->next = NULL;
          if (*result == NULL)
            *result = path;
          else
            last->next = path;
          last = path;
        }
      }
      break;

    case FRW_REG_TOKEN_KEY:
      if (parser->token_ident != NULL && last != NULL) {
        for (key = last->keys; (key != NULL && key->next != NULL); key = key->next);

        if (key != NULL && key->type == -1) {
          FRW_FREE (key->name);
          key->name = we_cmmn_strdup (WE_MODID_FRW, parser->token_ident);
        }
        else {
          tmp_key = FRW_ALLOCTYPE (frw_reg_txt_key_t);
          tmp_key->name = we_cmmn_strdup (WE_MODID_FRW, parser->token_ident);
          tmp_key->type = -1;
          tmp_key->next = NULL;
          tmp_key->value_bv = NULL;

          if (key == NULL)
            last->keys = tmp_key;
          else
            key->next = tmp_key;
        }
      }
      break;

    case FRW_REG_TOKEN_EQUAL:
      if (parser->value_type != -1) {
        if (last == NULL || last->keys == NULL) {
          FRW_FREE (parser->value_bv);
          break;
        }
        for (key = last->keys; (key->next != NULL); key = key->next);

        if (key->type != -1) {
          FRW_FREE (parser->value_bv);
          break;
        }

        key->type = parser->value_type;
        key->value_bv = parser->value_bv;
        key->value_i = parser->value_i;
      }
      break;
    case FRW_REG_TOKEN_ERROR:
      frw_reg_txt_delete_keys (*result, 1);
      *result = NULL;
      return FRW_REG_STATUS_FINISHED;

    }
  }






   
}




static int
frw_reg_txt_count_keys (frw_reg_txt_path_t* path, we_registry_param_t* params)
{
  frw_reg_txt_key_t*  key;
  int                 i = 0;

  for (; path != NULL; path = path->next) {
    for (key = path->keys; key != NULL; key = key->next) {
      if (key->type != -1) {
        if (params != NULL) {
          params->type = (WE_UINT8)(key->type);
          params->path = we_cmmn_strdup (WE_MODID_FRW, path->name);
          params->key = key->name;
          if (key->type == WE_REGISTRY_TYPE_STR) {
            params->value_i = 0;
            params->value_bv = key->value_bv;
            params->value_bv_length = (WE_UINT16)(key->value_i);
          }
          else {
            params->value_i = key->value_i;
            params->value_bv = NULL;
            params->value_bv_length = 0;
          }
#ifdef FRW_REG_LOG_WRITE_IN
          frw_reg_log_param("parsed SET", WE_MODID_FRW, params);
#endif
          params++;
        }
        i++;
      }
    }
  }
  return i;
}




static int
frw_reg_txt_store_command (frw_reg_txt_path_t* result)
{
  frw_registry_batch_t*     batch;
  frw_reg_command_t*        task;
  int                       nr_of_keys;

  nr_of_keys = frw_reg_txt_count_keys (result, NULL);
  if (nr_of_keys > 0) {
    batch = frw_reg_create_batch_struct (nr_of_keys);
    frw_reg_txt_count_keys (result, batch->params);

    task = FRW_ALLOCTYPE (frw_reg_command_t);
    task->src_modId = WE_MODID_FRW;
    task->sig_num = FRW_REGISTRY_SIG_SET;
    task->sig_data = batch;
    task->batch_element = NULL;

    
    task->next = frw_command_list_first;
    frw_command_list_first = task;
    if (frw_command_list_last == NULL)
      frw_command_list_last = task;

  }
  frw_reg_txt_delete_keys (result, 0);
  return nr_of_keys;
}




static void
frw_reg_txt_parse (void)
{
  frw_reg_txt_path_t*   result = NULL;
  void*                 parser_handle;

  parser_handle = frw_reg_txt_parse_init (frw_reg_txt_buffer, frw_reg_txt_size + 1);
  if (parser_handle != NULL) {
      
    if (frw_reg_txt_parse_reg (parser_handle, &result) == FRW_REG_STATUS_FINISHED && result != NULL &&
        frw_reg_txt_store_command (result) > 0) {
      frw_reg_state = FRW_REG_STATE_LOAD_TXT_OPEN;
      FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
    }
    else {
      WE_ERROR (WE_MODID_FRW, WE_ERR_SYSTEM_REG_TXT_PARSE);
      frw_reg_set_state_open ();
    }
    frw_reg_txt_parse_terminate (parser_handle); 
  }
  else {
    WE_ERROR (WE_MODID_FRW, WE_ERR_SYSTEM_REG_TXT_PARSE);
    frw_reg_set_state_open ();
  }
}






static int
frw_reg_txt_read_op (void)
{
  int r;

   while (frw_reg_txt_pos < frw_reg_txt_size) {
    r = WE_FILE_READ (frw_reg_txt_file, frw_reg_txt_buffer + frw_reg_txt_pos,
                       frw_reg_txt_size - frw_reg_txt_pos);
    if (r == WE_FILE_ERROR_DELAYED) {
      WE_FILE_SELECT (frw_reg_txt_file, WE_FILE_EVENT_READ);
      return 0;
    }
    else if (r < 0) {
      return -1;
    }
    frw_reg_txt_pos += r;
  }

  return 1;
}




static void
frw_reg_txt_read (void)
{
  int r;

  if ((r = frw_reg_txt_read_op ()) == 0) {
    return;
  }
  else if (r < 0) {
    WE_ERROR (WE_MODID_FRW, WE_ERR_SYSTEM_REG_TXT_FILE);
    WE_FILE_CLOSE(frw_reg_txt_file);
    frw_deregister_file_handle(frw_reg_txt_file);
    FRW_FREE (frw_reg_txt_buffer);
    frw_reg_set_state_open ();
    return;
  }

  WE_FILE_CLOSE(frw_reg_txt_file);
  frw_deregister_file_handle(frw_reg_txt_file);

  frw_reg_state = FRW_REG_STATE_LOAD_TXT_PARSE;
  FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);

  return;
}




static void
frw_reg_txt_load (void)
{
  long size;

  frw_reg_txt_file = WE_FILE_OPEN(WE_MODID_FRW, frw_reg_txt_file_name, TPI_FILE_SET_RDONLY, 0);
  if (frw_reg_txt_file < 0) {
    WE_ERROR (WE_MODID_FRW, WE_ERR_SYSTEM_REG_TXT_FILE);
    FRW_FREE (frw_reg_txt_file_name);
    frw_reg_txt_file_name = NULL;
    frw_reg_set_state_open ();
    return;
  }

  
  size = WE_FILE_GETSIZE (frw_reg_txt_file_name);
  if (size < 0) {
    WE_FILE_CLOSE(frw_reg_txt_file);
    WE_ERROR (WE_MODID_FRW, WE_ERR_SYSTEM_REG_TXT_FILE);
    FRW_FREE (frw_reg_txt_file_name);
    frw_reg_txt_file_name = NULL;
    frw_reg_txt_file = 0;
    frw_reg_txt_size = 0;
    frw_reg_set_state_open ();
    return;
  }
  frw_reg_txt_size = size;
  frw_register_file_handle (frw_reg_txt_file, FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);

  FRW_FREE (frw_reg_txt_file_name);
  frw_reg_txt_file_name = NULL;
  frw_reg_txt_buffer = FRW_ALLOC (frw_reg_txt_size + 1);
  frw_reg_txt_buffer[frw_reg_txt_size] = 0;
  frw_reg_txt_pos = '\0';
  frw_reg_state = FRW_REG_STATE_LOAD_TXT_READ;

  FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_RUN);
}







#if defined(FRW_REG_LOG_WRITE_IN) || defined(FRW_REG_LOG_WRITE_END)

static void
frw_reg_log_param(const char           *heading,
                  int                   src_modId,
                  we_registry_param_t *param)
{
  char*           path;
  char*           key;
  unsigned char*  value_bv;

  if (heading == NULL)
    heading = "";

  if (param == NULL) {
    TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                 "FRW_REG: %s: ERROR: missing operation. src_modId: %d\n",
                 heading, src_modId);
    return;
  }

  path = (param->path) ? (param->path) : "(null)";
  key =  (param->key)  ? (param->key)  : "(null)";

  switch (param->type) {
  case WE_REGISTRY_TYPE_INT:
    TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                 "FRW_REG: %s: src_modId: %d, path: %s, key: %s, type: int, value: %d\n",
                 heading, src_modId, path, key, param->value_i);
    break;
  case WE_REGISTRY_TYPE_STR:
    value_bv = (param->value_bv) ? (param->value_bv) : (unsigned char*) "(null)";
    TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                 "FRW_REG: %s: src_modId: %d, path: %s, key: %s, type: str, value: %s, length: %d\n",
                 heading, src_modId, path, key, value_bv, param->value_bv_length);
    break;
  case WE_REGISTRY_TYPE_DELETED:
    TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                 "FRW_REG: %s: src_modId: %d, path: %s, key: %s, type: deleted\n",
                 heading, src_modId, path, key);
    break;
  default:
    TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                 "FRW_REG: %s: ERROR: undefined operation type. src_modId: %d, path: %s, key: %s\n",
                 heading, src_modId, path, key);
    break;
  }
}





static void
frw_reg_log_task_reg_id(const char        *heading,
                        frw_reg_command_t *task)
{
  int                        src_modId;
  we_registry_identifier_t *id_param;
  char                      *path;
  char                      *key;

  if (heading == NULL)
    heading = "";

  if (task == NULL) {
    TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                 "FRW_REG: %s: ERROR: missing operation (task).\n",
                 heading);
    return;
  }

  src_modId = task->src_modId;
  id_param = (we_registry_identifier_t*) task->sig_data;

  if (id_param == NULL) {
    TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                 "FRW_REG: %s: ERROR: missing operation (task). src_modId: %d\n",
                 heading, src_modId);
    return;
  }

  path = (id_param->path) ? (id_param->path) : "(null)";
  key =  (id_param->key)  ? (id_param->key)  : "(null)";

  TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
               "FRW_REG: %s: src_modId: %d, path: %s, key: %s, wid: %d\n",
               heading, src_modId, path, key, id_param->wid);
}

#endif

#ifdef FRW_REG_LOG_WRITE_END





static void
frw_reg_log_batch_param(const char *heading)
{
  frw_registry_batch_t* batch;
  we_registry_param_t* param;

  if (!frw_command_list_first)
    return;

  batch = (frw_registry_batch_t*) frw_command_list_first->sig_data;
  param = &(batch->params[frw_command_list_first->batch_index]);

  frw_reg_log_param(heading, frw_command_list_first->src_modId, param);
}

static void
frw_reg_log_invalid_path_key(char *path, char* key)
{
  path = path ? path : "(null)";
  key =  key  ? key  : "(null)";

  TPIa_logMsg (WE_LOG_DETAIL_LOW, WE_MODID_FRW,
               "FRW_REG: ERROR: invalid path or key. path: %s, key: %s\n",
               path, key);

}

#endif


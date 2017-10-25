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
 * We_Afi.c
 *
 * Advanced File Interface:
 * 
 * This package allow files to be saved with long (more than 8+3 
 * characters) file names. The file_name must be encoded using 
 * utf-8. 
 *
 * Created by Örjan Percy, May 06 2003.
 *
 * Revision  history:
 */
#include "We_Afi.h"
#ifdef WE_CFG_AFI_REDIRECT_INDEX
#include "We_Afir.h"
#endif
#include "We_Mem.h"
#include "We_File.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Pipe.h"
#include "We_Lib.h"  /* directory support */
#include "We_Mime.h"

/*************************************************
 * Defines
 *************************************************/

/* States for saving files */
#define WE_AFI_STATE_NULL                      0
#define WE_AFI_STATE_COMPLETED                 1
#define WE_AFI_SAVE_OPEN                     101
#define WE_AFI_SAVE_CLOSE                    102
#define WE_AFI_SAVE_FILE_OPEN                103
#define WE_AFI_SAVE_FILE_SRC_READ            104
#define WE_AFI_SAVE_FILE_SRC_CLOSE           105
#define WE_AFI_SAVE_DST_WRITE                106
#define WE_AFI_SAVE_PIPE_READ                107
#define WE_AFI_STATE_OPEN_EXTERNAL           108
#define WE_AFI_STATE_DELETE                  109
#define WE_AFI_SAVE_SET_ATTR                 110
#define WE_AFI_STATE_REMOVE_RESULT           111
#define WE_AFI_STATE_RENAME_RESULT           112

#define WE_AFII_STATE_NULL                      0
#define WE_AFII_STATE_READ_IDX                  1
#define WE_AFII_STATE_READ                      2
#define WE_AFII_STATE_WRITE_IDX                 3
#define WE_AFII_STATE_WRITE_F_REC               4
#define WE_AFII_STATE_ADD_FILE                  5
#define WE_AFII_STATE_READ_FR                   6
#define WE_AFII_STATE_INIT_READ_FILE            7
#define WE_AFII_STATE_RESET_F_REC               8
#define WE_AFII_STATE_CREATE_IDX               10
#define WE_AFII_STATE_CREATE_FR                11
#define WE_AFII_STATE_CREATE_COMPLETED         12
#define WE_AFII_STATE_CONT                     13
#define WE_AFII_STATE_CLEAN_INDEX              14
#define WE_AFII_STATE_CLEAN_INDEX_INIT         15
#define WE_AFII_STATE_RENAME_CHECK_SOURCE_NAME 16

#define WE_AFII_STATE_OP_NULL                   0
#define WE_AFII_STATE_OP_READ_IDX              20
#define WE_AFII_STATE_OP_ADD_IDX_BLOCK_INIT    21
#define WE_AFII_STATE_OP_ADD_IDX_BLOCK         22
#define WE_AFII_STATE_OP_READ_FR               23

#define WE_AFII_HEADER_MORE_DATA               1

#define WE_AFII_RECORD_USED_FLAG               1

#define WE_AFI_IDX_FILE_NAME           "index.ix"
#define WE_AFI_FR_FILE_NAME            "index.fr"


#define WE_AFII_IDX_HEADER_SIZE                8
#define WE_AFII_IDX_RECORD_SIZE               14
#define WE_AFII_IDX_ITEMS_IN_BLOCK            12

#define WE_AFII_IDX_BLOCK_SIZE   (WE_AFII_IDX_HEADER_SIZE + \
                                  (WE_AFII_IDX_RECORD_SIZE * \
                                   WE_AFII_IDX_ITEMS_IN_BLOCK))

#define WE_AFIF_MAX_FILES                     30
#define WE_AFI_MAX_READDIR_SIZE              256

#define NO_PARENT_ID                          -1
#define NO_FILE                               -1
#define NO_PIPE                               -1

/* Directories end with a '/'. */
#define IS_DIR(x) (x[strlen (x) - 1] == '/')

/* Number of attempts tried when generating a temporary filename for save. */
#define WE_AFI_SAVE_TMP_FILE_TRIES           20

/*************************************************
 * Types
 *************************************************/

typedef int (*we_afif_callback_t)(we_pck_handle_t *handle,
                                   WE_INT32         wid);

typedef struct {
  int                 wid;
  int                 type;
  int                 handle;
  long                pos;
  we_afif_callback_t cb;
} we_afif_file_t;

typedef struct {
  WE_INT32 result;
} we_afi_generic_result_t;

typedef struct {
  WE_UINT16 flags;
  WE_UINT32 hash;
  WE_UINT32 size;
  WE_UINT32 pos;
} we_afii_idx_record_t;

typedef struct {
  WE_BOOL             is_dir;
  char                *name;
  we_pck_attr_list_t *attributes;
} we_afii_file_record_t;

typedef struct we_afii_fr_item_st {
  long                        file_pos;
  we_afii_idx_record_t      *ir;
  struct we_afii_fr_item_st *next;
} we_afii_fr_item_t;

typedef struct {
  int                 n_items;
  we_afii_fr_item_t *first;
} we_afii_fr_list_t;

typedef struct we_afii_fr_search_st {
  we_afii_fr_item_t           *item;
  struct we_afii_fr_search_st *next;
} we_afii_fr_search_t;

/* For handling dir size on subdirectories */
typedef struct we_afi_dir_list_st {
  int                         pos;   /* current position in dir */
  int                         num_entries;
  char                       *path;
  struct we_afi_dir_list_st *next;
} we_afi_dir_list_t;

/*******************************
 * Read index op types
 *******************************/
typedef struct {
  int                     state;
  char                   *buf;
  long                    bufsize;
  long                    datalen;
  long                    pos;
  char                   *fr;
  int                     fr_size;
  long                    fr_pos;
  long                    ir_pos;
  we_afii_file_record_t *frt;
} we_afii_op_data_t;

typedef struct we_afi_op_st {
  struct we_afi_op_st *next, *prev;
  WE_INT32             wid;        /* operation identifier */
  WE_INT32             parent_id; /* Parent identifier for Save, Rename*/
  WE_UINT8             type;      /* Type of operation. Package specific*/
  int                   state;
  void                 *priv_data;
  int                   pipe;      /* created pipe */
  void                 *op_data;   /* operation dependent data */
} we_afi_op_t;

typedef struct we_afii_handle_st {
  WE_INT32                  wid;
  int                        state;
  int                        init_state;
  we_afii_op_data_t        *op_data;
  int                       idx_file;  /* file handle to index file */
  int                        fr_file;   /* file handle to file record */
  int                        n_idx_blocks;
  we_afii_fr_list_t        *frl;         /* List of all file records*/
  we_afii_fr_list_t        *remove_frl;
  we_afii_fr_search_t      *search;      /* search list wrt. frl */
  void                      *result;      /* holds result structure */
  struct we_afii_handle_st *next;
} we_afii_handle_t;

typedef struct {
  struct we_afii_handle_st *handle_list;
} we_afii_t;

typedef struct {
  WE_UINT8        modid;
  we_afi_op_t    *op;
  we_afii_t      *afii;
  we_afif_file_t  files[WE_AFIF_MAX_FILES];
  WE_UINT32       we_afi_uniq_id;
} we_afi_handle_t;


/*
 * operation specific types
 */
typedef struct {
  char                *dir;
  char                *src;
  int                  mode;
  we_pck_attr_list_t *attr;
} we_afi_open_t;

typedef struct {
  char                *dir;
  char                *src;
  char                *dst;
  we_pck_attr_list_t *attr;
} we_afi_rename_t;

typedef struct {
  char *dir;
} we_afi_dir_t;

typedef struct {
  long                 data_len;  /* data length */
  char                *dst_name;  /* destination file name */
  char                *src_name;  /* name of pipe/file to copy */
  char                *tmp_name;  /* temporary file name */
  int                  save_type; /* save mode, buffer, pipe or file */
  WE_INT32            save_id;   /* current save op wid */
  int                  src_fh;    /* source file handle when saving */
  int                  file_handle;
  long                 bytes_saved;
  int                  read_complete; /* no more data to read in save operation */
  WE_INT32            status;
  we_pck_attr_list_t *attr;      /* attributes */
  void                *read_data; /* read data, from file, buffer or pipe */
  WE_UINT32           flags;
} we_afi_save_t;

typedef struct {
  WE_INT32 file_handle;
  long      curr_pos;
  long      data_size;
} we_afi_read_t;

typedef struct {
  WE_INT32  file_handle;
  long       curr_pos;
  long       data_size;
  void      *data;
} we_afi_write_t;

typedef struct {
  WE_INT32 wid;
  WE_INT32 delete_id; /* the wid of the op to delete */
  WE_INT32 status;
} we_afi_delete_t;

typedef struct {
  int file_handle;
} we_afi_fh_t;

/*****************************************************************************
 * fwd decl
 *****************************************************************************/

static we_afi_op_t *
find_op_by_id (we_afi_handle_t *handle, WE_INT32 wid);

static we_afii_handle_t *
we_afii_get_afii_handle (we_afi_handle_t *h, WE_INT32 wid);

/*****************************************************************************
 * util
 *****************************************************************************/
/* Split a path into two components.
 * For directory: parent directory and directory
 * For file:      directory and file
 */
static void
we_afi_split_path (WE_UINT8    modid,
                    const char  *path,
                    char       **dir,
                    char       **file)
{
  const char *p = path + strlen (path) - 1;

  if (*path == '/') {
    if (*p == '/') {
      /* this is a dir */
      p--;
    }
    while ((p != NULL) && (*p != '/')) {
      p--;
    }
  }
  if ((*path != '/') || (p == NULL)) {
    /* this is a file */
    if (dir != NULL) {
      *dir = NULL;
    }
    if (file != NULL) {
      *file = we_cmmn_strdup (modid, path);
    }
    return;
  }

  p++;
  if (dir != NULL) {
    *dir = we_cmmn_strndup (modid, path, p - path);
  }
  if (file != NULL) {
    *file = we_cmmn_strdup (modid, p);
  }
}


/* Created attribute is read-only */
static we_pck_attr_list_t *
get_created_attr (WE_UINT8 modid)
{
  we_pck_attr_list_t *p = WE_MEM_ALLOCTYPE (modid, we_pck_attr_list_t);

  p->_u.i = WE_TIME_GET_CURRENT (); /* i.e. now */
  p->attribute = WE_PCK_ATTRIBUTE_CREATED;
  p->type = WE_PCK_ATTR_TYPE_INT;
  p->next = NULL;

  return p;
}


static int
has_created_attr (const we_pck_attr_list_t *p)
{
  if (p == NULL) {
    return FALSE;
  }

  while (p) {
    if (p->attribute == WE_PCK_ATTRIBUTE_CREATED) {
      return TRUE;
    }
    p = p->next;
  }

  return FALSE;
}


static void
we_afi_free_open (WE_UINT8 modid, we_afi_open_t *open)
{
  WE_MEM_FREE (modid, open->dir);
  WE_MEM_FREE (modid, open->src);
  we_pck_attr_free (modid, open->attr);
  WE_MEM_FREE (modid, open);
}


static void
we_afi_free_dir (WE_UINT8 modid, we_afi_dir_t *dir)
{
  WE_MEM_FREE (modid, dir->dir);
  WE_MEM_FREE (modid, dir);
}


static void
we_afi_free_save (WE_UINT8 modid, we_afi_save_t *save)
{
  we_pck_attr_free (modid, save->attr);
  WE_MEM_FREE (modid, save->dst_name);
  WE_MEM_FREE (modid, save->src_name);
  WE_MEM_FREE (modid, save);
}


static void
we_afi_free_read (WE_UINT8 modid, we_afi_read_t *read)
{
  WE_MEM_FREE (modid, read);
}


static void
we_afi_free_write (WE_UINT8 modid, we_afi_write_t *write)
{
  WE_MEM_FREE (modid, write);
}

/*****************************************************************************
 *  AFIR BEGIN
 *****************************************************************************/
static int
we_afii_cvt_attribute_list (we_dcvt_t *obj, we_pck_attr_list_t **p)
{
  switch (obj->operation) {
  case WE_DCVT_DECODE:
    {
      /* NOTE: the remaining obj is attributes */
      long           length = obj->length - obj->pos;
      unsigned char *s;
      
      we_dcvt_uchar_vector (obj, length, &s);
      *p = we_pck_attr_to_list (obj->module, length, s);
      WE_MEM_FREE (obj->module, s);
    }
    break;

  case WE_DCVT_ENCODE:
    {
      long           length;
      unsigned char *s;

      we_pck_attr_to_string (obj->module, *p, &length, &s);
      we_dcvt_uchar_vector (obj, length, &s);
      WE_MEM_FREE (obj->module, s);
    }
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += we_pck_attr_get_size (*p);
    break;

  case WE_DCVT_FREE:
    we_pck_attr_free (obj->module, *p);
    break;

  default:
    return FALSE;
  }

  return TRUE;
}


static int
we_afii_cvt_idx_file_record (we_dcvt_t *obj, we_afii_file_record_t *p)
{
  if (!we_dcvt_bool (obj, &(p->is_dir)) ||
      !we_dcvt_string (obj, &(p->name)) ||
      !we_afii_cvt_attribute_list (obj, &(p->attributes))) {
    return FALSE;
  }

  return TRUE;
}


static int
we_afii_cvt_idx_record (we_dcvt_t *obj, we_afii_idx_record_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->flags)) ||
      !we_dcvt_uint32 (obj, &(p->hash)) ||
      !we_dcvt_uint32 (obj, &(p->size)) ||
      !we_dcvt_uint32 (obj, &(p->pos))) {
    return FALSE;
  }

  return TRUE;
}


static we_afii_handle_t *
we_afii_find_handle_by_id (we_afi_handle_t *h, WE_INT32 wid)
{
  we_afii_handle_t *afii_h = h->afii->handle_list;

  while (afii_h != NULL) {
    if (afii_h->wid == wid) {
      return afii_h;
    }
    afii_h = afii_h->next;
  }

  return NULL;
}


static void
we_afii_remove_handle_by_id (we_afi_handle_t *h, WE_INT32 wid)
{
  we_afii_handle_t *afii_h = h->afii->handle_list;
  we_afii_handle_t *prev = NULL;

  while (afii_h != NULL) {
    if (afii_h->wid == wid) {
      if (prev == NULL) {
        h->afii->handle_list = afii_h->next;
      }
      else {
        prev->next = afii_h->next;
      }
      WE_MEM_FREE (h->modid, afii_h);
      return;
    }
    prev = afii_h;
    afii_h = afii_h->next;
  }
}

/**********************************************************************
 * Local File Record operations
 **********************************************************************/

/************************************************
 * Functions for building a list of file records
 ************************************************/
static void
we_afii_fr_list_add (WE_UINT8              modid,
                      we_afii_handle_t     *h,
                      long                   file_pos,
                      we_afii_idx_record_t *ir)
{
  we_afii_fr_item_t *p;
  we_afii_fr_item_t *prev;
  we_afii_fr_item_t *s;

  s = prev = h->frl->first;

  while ((s != NULL) && (s->ir->pos < ir->pos)) {
    prev = s;
    s = s->next;
  }

  p = WE_MEM_ALLOCTYPE (modid, we_afii_fr_item_t);
  p->file_pos = file_pos;
  p->ir = WE_MEM_ALLOCTYPE (modid, we_afii_idx_record_t);
  memcpy (p->ir, ir, sizeof (we_afii_idx_record_t));

  p->next = s;
  if (prev == s) {
    h->frl->first = (void *)p;
  }
  else {
    prev->next = p;
  }

  h->frl->n_items++;
}


static int
we_afii_fr_list_find_empty (we_afii_handle_t *h, WE_UINT32 size)
{
  we_afii_fr_item_t *p;
  int                 i = 0;

  p = h->frl->first;

  while ((p != NULL) && ((p->ir->pos - i) < size)) {
    i = p->ir->pos + p->ir->size;
    p = p->next;
  }

  return i;
}


static int
we_afii_fr_list_is_empty (we_afii_handle_t *h)
{
  if ((h->frl == NULL) || (h->frl->first == NULL)) {
    return TRUE;
  }

  return FALSE;
}


/*
 * Fills in a search record list given the filename or /dir supplied.
 * Returns TRUE if at least one item was found, otherwise FALSE.
 */
static int
we_afii_file_record_search (WE_UINT8          modid,
                             we_afii_handle_t *h,
                             const char        *filename)
{
  we_afii_fr_item_t *s;
  WE_UINT32          hash;

  h->search = NULL;
  if (we_afii_fr_list_is_empty (h) == TRUE) {
    return FALSE;
  }

  s = h->frl->first;
  hash = we_cmmn_strhash_lc (filename, strlen (filename));

  while (s != NULL) {
    if (s->ir->hash == hash) {
      we_afii_fr_search_t *p = h->search;

      /* found a match, store in search list */
      h->search = WE_MEM_ALLOCTYPE (modid, we_afii_fr_search_t);
      h->search->item = s;
      h->search->next = p;
    }
    s = s->next;
  }

  return h->search != NULL;
}


static long
we_afii_fr_get_free_idx_rec (we_afi_handle_t *h, we_afii_handle_t *afii_h)
{
  we_afii_fr_item_t  *s;
  int                  i;
  char                *used;
  int                  used_size;

  if (afii_h->frl == NULL) {
    /* NOTE: check only for frl !*/
    return -1;
  }

  used_size = afii_h->n_idx_blocks * WE_AFII_IDX_ITEMS_IN_BLOCK;
  used = WE_MEM_ALLOC (h->modid, used_size);
  memset (used, 0, used_size);

  s = afii_h->frl->first;

  /* fill in used list */
  while (s != NULL) {
    int block_nr = s->file_pos / WE_AFII_IDX_BLOCK_SIZE;
    int index;

    index = (s->file_pos -
            (WE_AFII_IDX_HEADER_SIZE * block_nr)) / WE_AFII_IDX_RECORD_SIZE;

    used[index] = 1;
    s = s->next;
  }

  for (i = 0; i < used_size; i++) {
    if (used[i] == 0) {
      int nr_blocks = (i / WE_AFII_IDX_ITEMS_IN_BLOCK) + 1;
      int pos = (i * WE_AFII_IDX_RECORD_SIZE) +
                (nr_blocks * WE_AFII_IDX_HEADER_SIZE);

      WE_MEM_FREE (h->modid, used);

      return pos;
    }
  }
  WE_MEM_FREE (h->modid, used);

  return -1;
}


static int
we_afii_fr_list_remove_item (WE_UINT8           modid,
                              we_afii_handle_t  *h,
                              we_afii_fr_item_t *remove)
{
  we_afii_fr_item_t *fr = h->frl->first;

  if (fr == remove) {
    /* found item; the first in list */
    h->frl->first = fr->next;
    goto found;
  }
  else {
    we_afii_fr_item_t *fr_prev = fr;

    fr = fr->next;
    while ((fr != NULL) && (fr != remove)) {
      fr_prev = fr;
      fr = fr->next;
    }
    if ((fr != NULL) && (fr == remove)) {
      fr_prev->next = fr->next;
      goto found;
    }
    return FALSE;
  }

found:
  WE_MEM_FREE (modid, fr->ir);
  WE_MEM_FREE (modid, fr);
  h->frl->n_items--;

  return TRUE;
}


/* Remove the first item in the current search list.
 * If remove_item is TRUE, the item is also removed from the frl list.
 */
static void
we_afii_fr_search_remove_first (WE_UINT8          modid,
                                 we_afii_handle_t *h,
                                 int                remove_item)
{
  we_afii_fr_search_t *p = h->search->next;

  if (remove_item == TRUE) {
    /* find the item in frl and remove it */
    we_afii_fr_list_remove_item (modid, h, h->search->item);
  }
  WE_MEM_FREE (modid, h->search);
  h->search = p;
}


static void
we_afii_fr_list_remove_first (WE_UINT8 modid, we_afii_fr_list_t *frl)
{
  we_afii_fr_item_t *p;

  if ((frl == NULL) || ((p = frl->first) == NULL)) {
    return;
  }

  frl->first = p->next;
  frl->n_items--;

  WE_MEM_FREE (modid, p->ir);
  WE_MEM_FREE (modid, p);
}


static void
we_afii_fr_list_remove (WE_UINT8 modid, we_afii_fr_list_t *frl)
{
  if (frl != NULL) {
    if (frl->first != NULL) {
      we_afii_fr_item_t *fr = frl->first;

      while (fr != NULL) {
        we_afii_fr_item_t *tmp_fr = fr->next;

        WE_MEM_FREE (modid, fr->ir);
        WE_MEM_FREE (modid, fr);
        fr = tmp_fr;
      }
    }
    WE_MEM_FREE (modid, frl);
    frl = NULL;
  }
}


static int
we_afii_cvt_to_fr (we_afi_handle_t        *h,
                    we_afii_file_record_t  *frt,
                    char                    *data,
                    int                      data_len)
{
  we_dcvt_t obj;

  we_dcvt_init (&obj, WE_DCVT_DECODE, data, data_len, h->modid);
  if (we_afii_cvt_idx_file_record (&obj, frt) == FALSE) {
    return FALSE;
  }

  return TRUE;
}


/************************************************
 * Create a file record
 ************************************************/
static void
we_afii_create_fr (WE_UINT8             modid,
                    int                   is_dir,
                    char                 *name,
                    we_pck_attr_list_t  *attr,
                    char                **buf,
                    int                  *buf_len)
{
  we_afii_file_record_t frt;
  we_dcvt_t             obj;
#ifdef WE_CFG_AFI_REDIRECT_INDEX
  char                  *new_name = NULL;

  frt.is_dir  = is_dir;
  if(is_dir == TRUE) {
    /* Path redirected to where index files are stored */
    new_name  = we_afir_idx_redirect_path (modid, (const char*) name);
    frt.name  = new_name;
  }
  else {
    frt.name  = name;
  }

#else
  frt.is_dir  = is_dir;
  frt.name = name;
#endif
  frt.attributes = attr;

  we_dcvt_init (&obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_afii_cvt_idx_file_record (&obj, &frt);

  *buf_len = obj.pos;
  *buf = (char *)WE_MEM_ALLOC (modid, *buf_len);

  we_dcvt_init (&obj, WE_DCVT_ENCODE, *buf, *buf_len, modid);
  we_afii_cvt_idx_file_record (&obj, &frt);

#ifdef WE_CFG_AFI_REDIRECT_INDEX
  WE_MEM_FREE(modid, new_name);
#endif

}


/************************************************
 * Create a index record item
 ************************************************/
static void
we_afii_create_ir (we_afii_handle_t     *h,
                    we_afii_idx_record_t *p,
                    char                  *item,
                    int                    fr_size)
{
  p->flags = (WE_UINT16)(WE_AFII_RECORD_USED_FLAG);
  p->hash  = we_cmmn_strhash_lc (item, strlen (item));
  p->size  = fr_size;
  p->pos   = we_afii_fr_list_find_empty (h, fr_size);
}


/* File-size attribute is read-only */
static we_pck_attr_list_t *
get_filesize_attr (WE_UINT8 modid, WE_UINT32 size)
{
  we_pck_attr_list_t *p = WE_MEM_ALLOCTYPE (modid, we_pck_attr_list_t);

  p->_u.i = size;
  p->attribute = WE_PCK_ATTRIBUTE_SIZE;
  p->type = WE_PCK_ATTR_TYPE_INT;
  p->next = NULL;

  return p;
}


static int
we_afi_has_attr (const we_pck_attr_list_t *p, WE_UINT32 attr)
{
  if (p == NULL)
    return FALSE;

  while (p != NULL) {
    if (p->attribute == attr)
      return TRUE;

    p = p->next;
  }

  return FALSE;
}


static int
we_afi_has_size_attr (const we_pck_attr_list_t *p)
{
  return we_afi_has_attr (p, WE_PCK_ATTRIBUTE_SIZE);
}


static int
we_afi_has_mime_attr (const we_pck_attr_list_t *p)
{
  return we_afi_has_attr (p, WE_PCK_ATTRIBUTE_MIME);
}


/*
 *  This function extracts all file records in the data.
 *  Returns: 1, if successful.
 *           0, if file was not found
 *          -1, if failure
 */
static int
we_afii_parse_file_rec (WE_UINT8 modid, we_afii_handle_t *h)
{
  we_dcvt_t                    obj;
  we_afi_result_dir_entries_t *r;
  we_afii_file_record_t        frt;
  we_afii_op_data_t           *op_data;
  int                           i;
  int                           found = FALSE;
  int                           ret = 1;

  op_data = h->op_data;
  we_dcvt_init (&obj, WE_DCVT_DECODE, op_data->buf,
                 op_data->datalen, modid);
  if (we_afii_cvt_idx_file_record (&obj, &frt) == FALSE) {
    return -1;
  }

  /* try to find the path name in the existing file list */
  r = (we_afi_result_dir_entries_t *) h->result;
  for (i = 0; (i < r->num_entries) && !found; i++) {
    /* performance: check only if no attributes set */
    if ((r->entry_list[i].attr_list == NULL) &&
        (strcmp (r->entry_list[i].name, frt.name) == 0)) {
      /* found a matching name */
      r->entry_list[i].attr_list = frt.attributes; /* fill in the attributes */
      /* do not modify the r->entry_list[i].name, it is already correct */
      found = TRUE;
      ret = 1;
    }
  }

  if (!found) {
    /* The file does not exist */ 
    we_pck_attr_free (modid, frt.attributes);
    ret = 0;
  }
  WE_MEM_FREE (modid, frt.name);

  r->result = WE_AFI_OK;
  return ret;
}


static int
we_afii_parse_idx_rec (WE_UINT8          modid,
                        we_afii_handle_t *h,
                        int               *more_data)
{
  we_dcvt_t             obj;
  WE_UINT32             header1;
  WE_UINT32             header2;
  int                    i;
  we_afii_idx_record_t  p;
  void                  *buf;
  long                   length;
  long                   block_start;

  *more_data  = FALSE;
  buf         = h->op_data->buf;
  length      = h->op_data->datalen;
  block_start = ((h->n_idx_blocks - 1) * WE_AFII_IDX_BLOCK_SIZE);
  we_dcvt_init (&obj, WE_DCVT_DECODE, buf, length, 1);

  if(!we_dcvt_uint32 (&obj, &(header1)) ||
     !we_dcvt_uint32 (&obj, &(header2))) {
    return -1;
  }

  if ((header1 & WE_AFII_HEADER_MORE_DATA) != 0) {
    *more_data = TRUE;
  }

  for (i = 0; i < WE_AFII_IDX_ITEMS_IN_BLOCK; i++) {
    if (we_afii_cvt_idx_record (&obj, &p) == FALSE) {
      return -1;
    }
    if ((p.flags & WE_AFII_RECORD_USED_FLAG) == 0) {
      continue;
    }
    we_afii_fr_list_add (modid, h,
                          (obj.pos - WE_AFII_IDX_RECORD_SIZE) + block_start,
                          &p);
  }

  return 0;
}
/*****************************************************************************
 *  AFIR END
 *****************************************************************************/


/*****************************************************************************
 *  AFIF BEGIN
 *****************************************************************************/
static we_afif_file_t *
we_afif_new_file_handle (we_afi_handle_t  *handle,
                          int                wid,
                          int                type,
                          int               *item)
{
  we_afif_file_t *file = NULL;
  int              i;

  for (i = 0; i < WE_AFIF_MAX_FILES; i++) {
    if (handle->files[i].type == 0) {
      file = &(handle->files[i]);
      break;
    }
  }
  if (file != NULL) {
    file->cb      = NULL;
    file->handle  = 0;
    file->wid      = wid;
    file->pos     = 0;
    file->type    = type;
  }
  *item = i;

  return file;
}

static void 
we_afif_release_file_handle (we_afi_handle_t  *handle, int item)
{
  handle->files[item].type = 0;
  return;
}

static int
we_afif_file_init (we_afi_handle_t *h,
                    int               wid,
                    int               type,
                    const char       *dir,
                    const char       *file_name)
{
  we_afif_file_t *file;
  int              i;
  int              mode;
  char            *path;
#ifdef WE_CFG_AFI_REDIRECT_INDEX
  char            *index_dir;
#endif

  if ((file = we_afif_new_file_handle (h, wid, type, &i)) == NULL) {
    return WE_AFI_ERROR_INVALID;
  }

  switch (type) {
  case WE_AFI_RDONLY:
    mode = WE_FILE_SET_RDONLY;
    break;

  case WE_AFI_WRONLY:
    mode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE;
    break;

  case WE_AFI_RDWR:
    mode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
    break;

  default:
    return WE_AFI_ERROR_INVALID;
  }

#ifdef WE_CFG_AFI_REDIRECT_INDEX

  /* Check if it´s an index file */
  if(we_cmmn_strcmp_nc (file_name, WE_AFI_IDX_FILE_NAME) == 0 ||
     we_cmmn_strcmp_nc (file_name, WE_AFI_FR_FILE_NAME) == 0) {
    
    /* Redirect path to dir with index files */
    index_dir = we_afir_idx_redirect_path(h->modid, dir);
    path = we_cmmn_strcat (h->modid, (const char*)index_dir, file_name);
    WE_MEM_FREE (h->modid, index_dir);
  }
  else {
    path = we_cmmn_strcat(h->modid, dir, file_name);
  }

#else
  path = we_cmmn_strcat(h->modid, dir, file_name);
#endif
  
  file->handle = WE_FILE_OPEN (h->modid, path, mode, 0);
  WE_MEM_FREE (h->modid, path);

  if (file->handle < 0) {
    we_afif_release_file_handle(h, i);
    return file->handle;
  }

  if (WE_FILE_SEEK (file->handle, 0, WE_FILE_SEEK_SET) != 0) {
    return WE_AFI_ERROR_INVALID;
  }

  return i;
}


static long
we_afif_file_read (we_afi_handle_t  *handle,
                    int                wid,
                    int                file_handle,
                    void              *buf,
                    long               buflen)
{
  we_afif_file_t *file;
  long             n;

  if ((file_handle < 0) || (file_handle >= WE_AFIF_MAX_FILES)) {
    return WE_AFI_ERROR_INTERNAL;
  }

  file = &(handle->files[file_handle]);
  file->wid = wid;

  n = WE_FILE_READ (file->handle, buf, buflen);
  if (n == WE_AFI_ERROR_DELAYED) {
    WE_FILE_SELECT (file->handle, WE_FILE_EVENT_READ);
  }

  return n;
}


static long
we_afif_file_write (we_afi_handle_t *handle,
                     int               wid,
                     int               file_handle,
                     void             *buf,
                     long              buflen)
{
  we_afif_file_t *file;
  long             n;

  if ((file_handle < 0) || (file_handle >= WE_AFIF_MAX_FILES)){
    return WE_AFI_ERROR_INTERNAL;
  }

  file = &(handle->files[file_handle]);
  file->wid = wid;
  n = WE_FILE_WRITE (file->handle, buf, buflen);
  if (n == WE_AFI_ERROR_DELAYED) {
    WE_FILE_SELECT (file->handle, WE_FILE_EVENT_WRITE);
  }

  return n;
}


static int
we_afif_file_seek (we_afi_handle_t *handle, int file_handle, long offset)
{
  we_afif_file_t *file;

  if ((file_handle < 0) || (file_handle >= WE_AFIF_MAX_FILES)) {
    return WE_AFI_ERROR_INTERNAL;
  }
  file = &(handle->files[file_handle]);
  if (WE_FILE_SEEK (file->handle, offset, WE_FILE_SEEK_SET) != offset) {
    return WE_AFI_ERROR_INVALID;
  }

  return 0;
}


static int
we_afif_set_callback (we_afi_handle_t    *h,
                       int                  file_handle,
                       we_afif_callback_t  cb)
{
  we_afif_file_t *file;

  if ((file_handle < 0) || (file_handle >= WE_AFIF_MAX_FILES)) {
    return WE_AFI_ERROR_INTERNAL;
  }

  file = &(h->files[file_handle]);
  file->cb = cb;

  return 0;
}


static void
we_afif_file_close (we_afi_handle_t *handle, int file_handle)
{
  we_afif_file_t *file;

  if ((file_handle < 0) || (file_handle >= WE_AFIF_MAX_FILES)) {
    return;
  }

  file = &(handle->files[file_handle]);
  WE_FILE_CLOSE (file->handle);
  we_afif_release_file_handle(handle, file_handle);
}

/*****************************************************************************
 *  AFIF END
 *****************************************************************************/


/*****************************************************************************
 *  AFII BEGIN
 *****************************************************************************/

/**********************************************************************
 * TASK IMPLEMENTATION
 **********************************************************************/
static int
get_attr_flags (const we_pck_attr_list_t *p)
{
  if (p == NULL) {
    return 0;
  }

  while (p != NULL) {
    if (p->attribute == WE_PCK_ATTRIBUTE_FLAGS) {
      return p->_u.i;
    }
    p = p->next;
  }

  return 0;
}


static int
we_afii_init_add_file_write (we_afi_handle_t *h, we_afii_handle_t *afii_h)
{
  we_afii_op_data_t    *op_data = afii_h->op_data;
  we_afii_idx_record_t  ir;
  we_dcvt_t             obj;
  we_afi_op_t          *op = find_op_by_id (h, afii_h->wid);
  we_afi_open_t        *open = (we_afi_open_t *)op->op_data;

  if (has_created_attr (open->attr) == FALSE) {
    we_pck_attr_list_t *attr = open->attr;

    open->attr = get_created_attr (h->modid);
    open->attr->next = attr;
  }
  we_afii_create_fr (h->modid, IS_DIR (open->src), open->src, open->attr,
                      &op_data->fr, &op_data->fr_size);

  we_afii_create_ir (afii_h, &ir, open->src, op_data->fr_size);
  op_data->fr_pos = ir.pos;
  we_dcvt_init (&obj, WE_DCVT_ENCODE, op_data->buf,
                 WE_AFII_IDX_BLOCK_SIZE, h->modid);
  if (!we_afii_cvt_idx_record (&obj, &ir)) {
    return WE_AFI_ERROR_PATH;
  }

  return WE_AFI_OK;
}

/**********************************************************************
 * Local AFI operations
 **********************************************************************/

/*****************************************************************
 * READ FILE RECORD TASK
 *****************************************************************/
static int
we_afii_read_file_record_task (we_afi_handle_t  *h,
                                we_afii_handle_t *afii_h)
{
  we_afii_op_data_t *op_data;
  long                n;
  we_afii_fr_item_t *fr;

  op_data = afii_h->op_data;
  for (;;) {
    switch (op_data->state) {

    case WE_AFII_STATE_OP_NULL:
      if (afii_h->search == NULL) {
        return WE_AFI_OK;
      }

      if ((fr = afii_h->search->item) == NULL) {
        return WE_AFI_OK;
      }

      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->bufsize = fr->ir->size;
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid, op_data->bufsize);
      memset (op_data->buf, 0, op_data->bufsize);
      op_data->datalen = 0;
      if (we_afif_file_seek (h, afii_h->fr_file, fr->ir->pos) < 0) {
        return WE_AFI_ERROR_INTERNAL;
      }
      op_data->state = WE_AFII_STATE_OP_READ_FR;

    case WE_AFII_STATE_OP_READ_FR:
      n = we_afif_file_read (h, afii_h->wid, afii_h->fr_file,
                              &op_data->buf[op_data->datalen],
                              op_data->bufsize - op_data->datalen);
      if ((n < 0) && (n != WE_AFI_ERROR_EOF)) {
        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize)) {
        continue;
      }

      /*Return but do not free buffer*/
      op_data->state = WE_AFII_STATE_OP_NULL;
      return WE_AFI_OK;

    default:
      return WE_AFI_ERROR_INTERNAL;
    }
  }
}


/*****************************************************************
 * ADD FILE TASK
 *****************************************************************/
static int
we_afii_add_file_task (we_afi_handle_t *h, we_afii_handle_t *afii_h)
{
  we_afii_op_data_t       *op_data = afii_h->op_data;
  long                      n;
  we_afi_generic_result_t *res;

  res = ((we_afi_generic_result_t *)afii_h->result);
  for (;;) {
    switch (op_data->state) {

    case WE_AFII_STATE_OP_NULL:
      op_data->pos = we_afii_fr_get_free_idx_rec (h, afii_h);
      if (op_data->pos < 0) {
        op_data->state = WE_AFII_STATE_OP_ADD_IDX_BLOCK_INIT;
        break;
      }
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid,
                                            WE_AFII_IDX_RECORD_SIZE);
      op_data->bufsize = WE_AFII_IDX_RECORD_SIZE;
      op_data->datalen = 0;
      n = we_afii_init_add_file_write (h, afii_h);
      if (n != WE_FILE_OK) {
        res->result = n;

        return n;
      }

      we_afif_file_seek (h, afii_h->idx_file, op_data->pos);
      op_data->state = WE_AFII_STATE_WRITE_IDX;

    case WE_AFII_STATE_WRITE_IDX:
      n = we_afif_file_write (h, afii_h->wid, afii_h->idx_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;

      if ((n > 0) && (op_data->datalen < op_data->bufsize))
        continue;

      /*Now write file record*/
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf     = op_data->fr;
      op_data->fr      = NULL;
      op_data->bufsize = op_data->fr_size;
      op_data->datalen = 0;
      if (we_afif_file_seek (h, afii_h->fr_file, op_data->fr_pos) < 0) {
        res->result = WE_AFI_ERROR_INTERNAL;

        return WE_AFI_ERROR_INTERNAL;
      }
      op_data->state = WE_AFII_STATE_WRITE_F_REC;

    case WE_AFII_STATE_WRITE_F_REC:
      n = we_afif_file_write (h, afii_h->wid, afii_h->fr_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize))
        continue;
      goto done;

    case WE_AFII_STATE_OP_ADD_IDX_BLOCK_INIT:
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid,
                                            WE_AFII_IDX_BLOCK_SIZE);
      memset (op_data->buf, 0, WE_AFII_IDX_BLOCK_SIZE);
      op_data->bufsize = WE_AFII_IDX_BLOCK_SIZE;
      op_data->datalen = 0;
      we_afif_file_seek (h, afii_h->idx_file,
                          (afii_h->n_idx_blocks * WE_AFII_IDX_BLOCK_SIZE));
      op_data->state   = WE_AFII_STATE_OP_ADD_IDX_BLOCK;

    case WE_AFII_STATE_OP_ADD_IDX_BLOCK:
      n = we_afif_file_write (h, afii_h->wid, afii_h->idx_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < WE_AFII_IDX_BLOCK_SIZE))
        continue;

      afii_h->n_idx_blocks++;
      op_data->state = WE_AFII_STATE_OP_NULL;
      break;

    default:
      goto done;
    }
  }

done:
  op_data->state = WE_AFII_STATE_OP_NULL;
  WE_MEM_FREE (h->modid, op_data->buf);
  op_data->buf = NULL;
  op_data->bufsize = 0;
  op_data->datalen = 0;
  res->result = WE_AFI_OK;
  WE_FILE_FLUSH ((h->files[afii_h->fr_file]).handle);
  WE_FILE_FLUSH ((h->files[afii_h->idx_file]).handle);

  return WE_AFI_OK;
}

/*****************************************************************
 * READ INDEX FILE TASK
 *****************************************************************/
static int
we_afii_read_idx_task (we_afi_handle_t *h, we_afii_handle_t *afii_h)
{
  int                 more_data = FALSE;
  WE_INT32           n;
  we_afii_op_data_t *op_data = afii_h->op_data;

  for (;;) {
    switch (op_data->state) {
    case WE_AFII_STATE_OP_NULL:
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid,
                                            WE_AFII_IDX_BLOCK_SIZE);
      op_data->bufsize = WE_AFII_IDX_BLOCK_SIZE;
      op_data->pos = 0;
      op_data->datalen = 0;
      op_data->state = WE_AFII_STATE_OP_READ_IDX;

    case WE_AFII_STATE_OP_READ_IDX:
      n = we_afif_file_read (h, afii_h->wid, afii_h->idx_file,
                              &op_data->buf[op_data->datalen],
                              WE_AFII_IDX_BLOCK_SIZE - op_data->datalen);
      if (n < 0) {
        if (n != WE_AFI_ERROR_EOF) {
          return n;
        }
      }
      else {
        op_data->datalen += n;
      }
      if ((n > 0) && (op_data->datalen < op_data->bufsize))
        continue;

      if ((op_data->datalen > 0) &&
        (op_data->datalen < WE_AFII_IDX_BLOCK_SIZE)) {
        return WE_AFI_ERROR_INTERNAL;
      }

      /* See if the file is empty */
      if ((n == WE_AFI_ERROR_EOF) && (op_data->datalen == 0)) {
        goto done;
      }

      afii_h->n_idx_blocks++;
      if (we_afii_parse_idx_rec (h->modid, afii_h, &more_data) < 0) {
        return WE_AFI_ERROR_INTERNAL;
      }

      op_data->datalen = 0;
      continue;

    default:
      return WE_AFI_ERROR_INTERNAL;
    }
  }

done:
  op_data->state = WE_AFII_STATE_OP_NULL;
  WE_MEM_FREE (h->modid, op_data->buf);
  op_data->buf = NULL;
  op_data->bufsize = 0;
  op_data->datalen = 0;

  return WE_AFI_OK;
}

/*****************************************************************
 * OPEN/CREATE FILE TASK
 *****************************************************************/
static int
we_afii_file_open_task (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t      *h = (we_afi_handle_t *)handle;
  we_afii_op_data_t    *op_data;
  we_afii_handle_t     *afii_h;
  we_afi_result_open_t *res;
  int                    n;
  int                    found = FALSE;
  we_afi_op_t          *op = find_op_by_id (h, wid);
  we_afi_open_t        *open;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  open = (we_afi_open_t *)op->op_data;
  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }

  op_data = afii_h->op_data;
  res = (we_afi_result_open_t *)afii_h->result;

  for (;;) {
    switch (afii_h->state) {

    case WE_AFII_STATE_NULL:
      afii_h->state = WE_AFII_STATE_READ_IDX;

    case WE_AFII_STATE_READ_IDX:
      if ((n = we_afii_read_idx_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }
      if ((n == WE_AFI_OK) &&
          ((found = we_afii_file_record_search (h->modid, afii_h,
                                                 open->src)) == TRUE)) {
        /* found at least one index entry that matches supplied filename */
        afii_h->state = WE_AFII_STATE_READ_FR;
        break;
      }
      if (op->type == WE_AFI_GET_ATTR) {
        /* Retrieving attributes, but file does not have any attributes*/
        res->result = WE_AFI_ERROR_PATH;

        return WE_AFI_ERROR_PATH;
      }

      if (found == FALSE) {
        char *s = we_cmmn_strcat (h->modid, open->dir, open->src);

        if (WE_FILE_GETSIZE (s) != WE_FILE_ERROR_PATH) {
         /* There are no need to do any other checks since non
          * AFI files cannot have any attributes.
          */
          res->result = WE_AFI_OK;
          WE_MEM_FREE (h->modid, s);

          return WE_AFI_OK;
        }
        if (WE_FILE_GETSIZE_DIR (s) != WE_FILE_ERROR_PATH) {
          res->result = WE_AFI_ERROR_EXIST;
          WE_MEM_FREE (h->modid, s);

          return WE_AFI_ERROR_EXIST;
        }
        WE_MEM_FREE (h->modid, s);
      }
      op_data->state = WE_AFII_STATE_NULL;
      afii_h->state = WE_AFII_STATE_ADD_FILE;
      break;

    case WE_AFII_STATE_ADD_FILE:
      /* Before writing, check that we have a writable flag. */
      if (open->mode == WE_AFI_RDONLY) {
        res->result = WE_AFI_ERROR_ACCESS;

        return WE_AFI_ERROR_ACCESS;
      }

      if ((n = we_afii_add_file_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }

      res->result = WE_AFI_OK;
      return WE_AFI_OK;

    case WE_AFII_STATE_READ_FR:
      if ((n = we_afii_read_file_record_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }

      {
        we_afii_file_record_t frt;

        if (! we_afii_cvt_to_fr (h, &frt, op_data->buf, op_data->datalen)) {
          res->result = WE_AFI_ERROR_INTERNAL;

          return WE_AFI_ERROR_INTERNAL;
        }
        open->attr = frt.attributes;
        /* Check if filename is the found file */
        if (strcmp (open->src, frt.name) != 0) {
          /* free the current search item and try next */
          we_afii_fr_search_remove_first (h->modid, afii_h, FALSE);
          if (afii_h->search == NULL) {
            /* Did not find file */

            if (op->type == WE_AFI_OPEN) {
              /* opening an non-existent file msans file creation */
              afii_h->state = WE_AFII_STATE_ADD_FILE;
              break;
            }
            res->result = WE_AFI_ERROR_PATH;

            return WE_AFI_ERROR_PATH;
          }
          break;
        }

        if ((open->mode == WE_AFI_WRONLY) || (open->mode == WE_AFI_RDWR)) {
          int flags;

          /* check that file is not protected */
          flags = get_attr_flags (open->attr);
          if ((flags & WE_PCK_FLAG_PROTECTED) |
              (flags & WE_PCK_FLAG_USER_PROTECTED)) {
            we_pck_attr_free (h->modid, open->attr);
            WE_MEM_FREE (h->modid, frt.name);
            res->result = WE_FILE_ERROR_ACCESS;

            return WE_FILE_ERROR_ACCESS;
          }
        }
        
        if (op->type == WE_AFI_GET_ATTR) {
          /* Before returning the result, add the file size to list. */
          if ((IS_DIR (open->src) == FALSE) &&
              (we_afi_has_size_attr (open->attr) == FALSE)) {
            we_pck_attr_list_t *attr;
            char                *path;

            path = we_cmmn_strcat (h->modid, open->dir, open->src);
            attr = get_filesize_attr (h->modid, WE_FILE_GETSIZE (path));
            attr->next = open->attr;
            open->attr = attr;
            WE_MEM_FREE (h->modid, path);
          }
          else {
            open->attr = frt.attributes;
          }
        }

        WE_MEM_FREE (h->modid, frt.name);
        res->result = WE_AFI_OK;

        return WE_AFI_OK;
      }

    default:
      res->result = WE_AFI_ERROR_INTERNAL;

      return WE_AFI_ERROR_INTERNAL;
    }
  }
}


/*
 * Read the directory contents for path. The result, if successful,
 * is returned in dir. dir is always allocated.
 */
static int
we_afif_read_dir (WE_UINT8                      modid,
                   const char                    *path,
                   we_afi_result_dir_entries_t **dir)
{
  int n_items = WE_FILE_GETSIZE_DIR (path);
  int pos;
  int dir_pos;

  if (n_items > 1) {
    *dir = WE_MEM_ALLOC (modid,
                          sizeof (we_afi_result_dir_entries_t) +
                          (sizeof (we_afi_file_t) * n_items - 1));
  }
  else {
    *dir = WE_MEM_ALLOC (modid, sizeof (we_afi_result_dir_entries_t));
  }

  if (n_items < 0) {
    (*dir)->entry_list[0].attr_list = NULL;
    (*dir)->entry_list[0].is_dir = FALSE;
    (*dir)->entry_list[0].name = NULL;
    (*dir)->num_entries = 0;
    (*dir)->result = n_items;

    return n_items;
  }

  for (pos = 0, dir_pos = 0; pos < n_items; pos++) {
    char name[WE_AFI_MAX_READDIR_SIZE];
    int  ret;
    int  type;
    long size;

    if ((ret = WE_FILE_READ_DIR (path, pos, name,
                                  WE_AFI_MAX_READDIR_SIZE,
                                  &type, &size)) < 0) {
      (*dir)->num_entries = dir_pos;
      (*dir)->result = ret;

      return ret;
    }

    /* Index files must not be added to the file list */
    /* In case of no index files in the same directorys */
    /* as the files we should not need to test this */

    if (strcmp (name, WE_AFI_FR_FILE_NAME) != 0 &&
        strcmp (name, WE_AFI_IDX_FILE_NAME) != 0) {
      (*dir)->entry_list[dir_pos].attr_list = NULL;
      (*dir)->entry_list[dir_pos].is_dir = (type == TPI_FILE_DIRTYPE);
      (*dir)->entry_list[dir_pos].name = we_cmmn_strdup (modid, name);
      dir_pos++;
    } 
  }
  (*dir)->num_entries = dir_pos;
  (*dir)->result = WE_AFI_OK;

  return WE_AFI_OK;
}


/*
 * Returns an attribute with the mimetype for the supplied file name.
 * Uses the file extension to find appropriate mime type.
 */
static we_pck_attr_list_t *
we_get_mime_attr (WE_UINT8 modid, const char *file)
{
  const char          *mime;
  const char          *ext = NULL;
  we_pck_attr_list_t *attr;

  if (file != NULL) {
    ext = strrchr (file, '.');
    if (ext != NULL) {
      ext++; /* do not include '.' */
    }
  }
  mime = we_mime_ext_to_mime (ext);
  we_pck_attr_init (&attr);
  if (we_pck_add_attr_string_value (modid, &attr, WE_PCK_ATTRIBUTE_MIME,
                                     mime) != TRUE) {
    return NULL;
  }

  return attr;
}


/*****************************************************************
 * GET DIRENTRIES
 *****************************************************************/
static void
we_afii_free_result_dir_entries (we_afi_handle_t             *h,
                                  we_afi_result_dir_entries_t *dir)
{
  int i;

  if (dir == NULL) {
    return;
  }
  for (i = 0; i < dir->num_entries; i++) {
    WE_MEM_FREE (h->modid, dir->entry_list[i].name);
    we_pck_attr_free (h->modid, dir->entry_list[i].attr_list);
  }
  WE_MEM_FREE (h->modid, dir);
}


static int
we_afii_get_direntries_task (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t             *h = (we_afi_handle_t *)handle;
  we_afii_op_data_t           *op_data;
  we_afii_fr_item_t           *p;
  int                           n;
  we_afii_handle_t            *afii_h;
  we_afi_result_dir_entries_t *res;
  we_afi_op_t                 *op = find_op_by_id (h, wid);
  we_afi_dir_t                *dir;

  if ((afii_h = we_afii_get_afii_handle (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  dir = (we_afi_dir_t *)(op->op_data);
  res = (we_afi_result_dir_entries_t *)(afii_h->result);
  op_data = afii_h->op_data;
  switch (afii_h->init_state) {

  case WE_AFII_STATE_NULL:
    if (IS_DIR (dir->dir) == FALSE) {
      res->result = WE_AFI_ERROR_PATH;
      return WE_AFI_ERROR_PATH;
    }
    if ((n = we_afif_file_init (h, wid, WE_AFI_RDWR, dir->dir,
                                 WE_AFI_IDX_FILE_NAME)) < 0) {
      res->result = n;
      return n;
    }
    afii_h->idx_file = n;

    if ((n = we_afif_file_init (h, wid, WE_AFI_RDWR, dir->dir,
                                 WE_AFI_FR_FILE_NAME)) < 0) {
      res->result = n;
      return n;
    }
    afii_h->fr_file = n;
    we_afif_set_callback (h, afii_h->idx_file, we_afii_get_direntries_task);
    we_afif_set_callback (h, afii_h->fr_file, we_afii_get_direntries_task);
    afii_h->init_state = WE_AFII_STATE_CREATE_COMPLETED;

  case WE_AFII_STATE_CREATE_COMPLETED:
    break;
  }

  for (;;) {
    switch (afii_h->state) {

    case WE_AFII_STATE_NULL:
    case WE_AFII_STATE_READ_IDX:
      if ((n = we_afii_read_idx_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;
        return n;
      }

      {
        int r;

        we_afii_free_result_dir_entries (h, res);
        res = 0;
        r = we_afif_read_dir (h->modid, dir->dir, &res); /* synchronous */
        afii_h->result = (void *)res;
        if (r < 0) {
          return res->result;
        }
      }
      afii_h->state = WE_AFII_STATE_INIT_READ_FILE;

    case WE_AFII_STATE_INIT_READ_FILE:
      if (we_afii_fr_list_is_empty (afii_h) == TRUE) {
        res->result = WE_AFI_OK;
        return res->result;
      }

      p = afii_h->frl->first;
      if (p != NULL) {
        op_data->fr_pos = p->ir->pos;
        op_data->fr_size = p->ir->size;
      }
      /* clean search list */
      while (afii_h->search != NULL) {
        we_afii_fr_search_remove_first (h->modid, afii_h, FALSE);
      }

      afii_h->state = WE_AFII_STATE_READ_FR;

      afii_h->search = WE_MEM_ALLOCTYPE (h->modid, we_afii_fr_search_t);
      afii_h->search->item = afii_h->frl->first;
      afii_h->search->next = NULL;

    case WE_AFII_STATE_READ_FR:
      {
        res->result = n = we_afii_read_file_record_task (h, afii_h);
        if (n < 0) {
          return n;
        }

        n = we_afii_parse_file_rec (h->modid, afii_h);
        WE_MEM_FREE (h->modid, afii_h->op_data->buf);
        afii_h->op_data->buf = NULL;
        afii_h->op_data->bufsize = 0;

        if (op_data->datalen < 0) {
          res->result = WE_AFI_OK;

          return res->result;  /* empty file, no more data */
        }

        if (n < 0) {
          res->result = WE_AFI_ERROR_INTERNAL;

          return res->result;
        }
        else if (n == 0) {
          we_afii_fr_item_t *tmp;

          tmp = afii_h->frl->first;
          afii_h->frl->first = afii_h->frl->first->next;
          afii_h->frl->n_items--;

          tmp->next = afii_h->remove_frl->first;
          afii_h->remove_frl->first = tmp;
          afii_h->remove_frl->n_items++;
        }
        else {
          we_afii_fr_list_remove_first (h->modid, afii_h->frl);
        }

        if (we_afii_fr_list_is_empty (afii_h) == TRUE) {
          if (afii_h->remove_frl->n_items > 0) {
            afii_h->state = WE_AFII_STATE_CLEAN_INDEX_INIT;
            break;
          }
          res->result = WE_AFI_OK;

          return res->result;
        }

        afii_h->state = WE_AFII_STATE_INIT_READ_FILE;
        break;
      }

    case WE_AFII_STATE_CLEAN_INDEX_INIT:
      {
        we_afii_fr_item_t *remove;

        /*Remove items with no corresponding file from ix file*/
        op_data->buf = (char *)WE_MEM_ALLOC (h->modid,
                                              WE_AFII_IDX_RECORD_SIZE);
        op_data->bufsize = WE_AFII_IDX_RECORD_SIZE;
        op_data->datalen = 0;
        memset (op_data->buf, 0, WE_AFII_IDX_RECORD_SIZE);

        remove = afii_h->remove_frl->first;
        we_afif_file_seek (h, afii_h->idx_file, remove->file_pos);
        /*Remove item from remove list*/
        we_afii_fr_list_remove_first (h->modid, afii_h->remove_frl);

        afii_h->state = WE_AFII_STATE_CLEAN_INDEX;
      }

    case WE_AFII_STATE_CLEAN_INDEX:
      {
        long r;

        r = we_afif_file_write (h, wid, afii_h->idx_file,
                                 &op_data->buf[op_data->datalen],
                                 op_data->bufsize - op_data->datalen);
        if (r < 0) {
          if (r == WE_AFI_ERROR_DELAYED) {
            return r;
          }

          return res->result;
        }

        op_data->datalen += r;
        if ((r > 0) && (op_data->datalen < op_data->bufsize))
          continue;

        if (afii_h->remove_frl->n_items > 0) {
          we_afii_fr_item_t *remove;

          op_data->datalen = 0;
          remove = afii_h->remove_frl->first;
          we_afif_file_seek (h, afii_h->idx_file, remove->file_pos);

          /* Remove item from remove list */
          we_afii_fr_list_remove_first (h->modid, afii_h->remove_frl);
          break;
        }
        res->result = WE_AFI_OK;

        return res->result;
       }

    default:
      res->result = WE_AFI_ERROR_INTERNAL;

      return res->result;
    }
  }
}


/*****************************************************************
 * REMOVE
 *****************************************************************/
/* Remove a file. No attributes */
static int
remove_file (WE_UINT8 modid, const we_afi_open_t *rem)
{
  char *item = we_cmmn_strcat (modid, rem->dir, rem->src);
  int   res = WE_FILE_REMOVE (item);

  WE_MEM_FREE (modid, item);

  return res;
}


#ifdef WE_CFG_AFI_REDIRECT_INDEX
static int
remove_dir (WE_UINT8 modid, const char *dir)
{
  char        *temp_dir;
  char        *idx; 
  char        *fr;
  int          status;

  temp_dir = we_afir_idx_redirect_path (modid, dir);
  idx = we_cmmn_strcat (modid, (const char*) temp_dir, WE_AFI_IDX_FILE_NAME);
  fr = we_cmmn_strcat (modid, (const char*) temp_dir, WE_AFI_FR_FILE_NAME);

  /* Try to remove index files. */
  WE_FILE_REMOVE (idx);
  WE_FILE_REMOVE (fr);
  WE_MEM_FREE (modid, idx);
  WE_MEM_FREE (modid, fr);

  status = WE_FILE_RMDIR (dir); /* path should now be an empty dir*/
  if(we_cmmn_strcmp_nc(dir, (const char*)temp_dir) == 0) {
    WE_MEM_FREE (modid, temp_dir);
    return status;
  }
  WE_FILE_RMDIR (temp_dir); /* remove index file folder */
  WE_MEM_FREE (modid, temp_dir);

  return status;
}
#else
static int
remove_dir (WE_UINT8 modid, const char *dir)
{
  char *idx = we_cmmn_strcat (modid, dir, WE_AFI_IDX_FILE_NAME);
  char *fr = we_cmmn_strcat (modid, dir, WE_AFI_FR_FILE_NAME);
  

  /* Try to remove index files. */
  WE_FILE_REMOVE (idx);
  WE_FILE_REMOVE (fr);
  WE_MEM_FREE (modid, idx);
  WE_MEM_FREE (modid, fr);

  return WE_FILE_RMDIR (dir); /* path should now be an empty dir*/

}
#endif

static int
we_afii_file_remove_task (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t         *h = (we_afi_handle_t *)handle;
  we_afii_handle_t        *afii_h;
  we_afii_op_data_t       *op_data;
  we_afii_fr_item_t       *fri;
  int                       r;
  we_afi_generic_result_t *res;
  we_afi_op_t             *op = find_op_by_id (h, wid);
  we_afi_open_t           *open;

  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  open = (we_afi_open_t *)(op->op_data);
  op_data = afii_h->op_data;
  res = ((we_afi_generic_result_t *)afii_h->result);
  for (;;) {
    switch (afii_h->state) {

    case WE_AFII_STATE_NULL:
    case WE_AFII_STATE_READ_IDX:
      if ((r = we_afii_read_idx_task (h, afii_h)) != WE_AFI_OK) {
        res->result = r;

        return r;
      }
      if (we_afii_file_record_search (h->modid, afii_h, open->src) == FALSE) {
        /* could not find file */
        int r = remove_file (h->modid, open);

        if (r < 0) {
          char *dir = we_cmmn_strcat (h->modid, open->dir, open->src);

          r = remove_dir (h->modid, dir); /* might be a directory */
          WE_MEM_FREE (h->modid, dir);
        }
        res->result = r;

        return r;
      }
      afii_h->state = WE_AFII_STATE_READ_FR;

    case WE_AFII_STATE_READ_FR:
      if ((r = we_afii_read_file_record_task (h, afii_h)) != WE_AFI_OK) {
        res->result = r;

        return r;
      }
      {
        we_afii_file_record_t frt;
        int                    flags;

        if (!we_afii_cvt_to_fr (h, &frt, op_data->buf, op_data->datalen)) {
          res->result = WE_AFI_ERROR_INTERNAL;

          return WE_AFI_ERROR_INTERNAL;
        }

        /* Check if filename is the found file */
        if (frt.name == NULL || strcmp (open->src, frt.name) != 0) {
          we_afii_fr_search_remove_first (h->modid, afii_h, FALSE);
          WE_MEM_FREE (h->modid, frt.name);
          we_pck_attr_free (h->modid, frt.attributes);
          if (afii_h->search == NULL) {
            char *filename = we_cmmn_strcat(h->modid, open->dir, open->src);
            int r = 0;
            /*Check if file or dir exists*/
            if (WE_FILE_GETSIZE (filename) >= 0) {
              r = remove_file (h->modid, open);
            }
            else if (WE_FILE_GETSIZE_DIR (filename) >= 0) {
              r = remove_dir (h->modid, filename);              
            }
            else {
              r = WE_AFI_ERROR_PATH;
            }
            res->result = r;
            WE_MEM_FREE (h->modid, filename);
            return r;
          }
          else if (frt.name == NULL) {
            res->result = WE_AFI_ERROR_INTERNAL;
            return WE_AFI_ERROR_INTERNAL;         
          }
          break;
        }

        WE_MEM_FREE (h->modid, op_data->buf);
        op_data->buf = NULL;

        /* check that file is not protected */
        flags = get_attr_flags (frt.attributes);
        if ((flags & WE_PCK_FLAG_PROTECTED) |
            (flags & WE_PCK_FLAG_USER_PROTECTED)) {
          we_pck_attr_free (h->modid, frt.attributes);
          WE_MEM_FREE (h->modid, frt.name);
          res->result = WE_FILE_ERROR_ACCESS;

          return WE_FILE_ERROR_ACCESS;
        }

        we_pck_attr_free (h->modid, frt.attributes);
        WE_MEM_FREE (h->modid, frt.name);

        if (frt.is_dir == TRUE) {
          char *dir = we_cmmn_strcat (h->modid, open->dir, open->src);
#ifdef WE_CFG_AFI_REDIRECT_INDEX
          char *index_dir = we_afir_idx_redirect_path(h->modid, (const char*) dir);

          if(we_cmmn_strcmp_nc(dir, index_dir) != 0) {
            /* index files are in the same dir as files */
            if (WE_FILE_GETSIZE_DIR (dir) > 2) {
              r = WE_AFI_ERROR_ACCESS; /* dir is not empty */
            }
            else {
              /* index files, file-folder removed */
              remove_dir (h->modid, dir);
            }
          }
          else {
            /* index files in separate folder */
            if (WE_FILE_GETSIZE_DIR (dir) > 0) {
              r = WE_AFI_ERROR_ACCESS; /* dir is not empty */
            }
            else {
              /* index files, ix-folder, file-folder removed */
              remove_dir (h->modid, dir);
            }
          }
          WE_MEM_FREE (h->modid, dir);
          WE_MEM_FREE (h->modid, index_dir);
#else
          if (WE_FILE_GETSIZE_DIR (dir) > 2) {
            r = WE_AFI_ERROR_ACCESS; /* dir is not empty */
          }
          else {
            remove_dir (h->modid, dir);
          }
          WE_MEM_FREE (h->modid, dir);
#endif
        }
        else {
          r = remove_file (h->modid, open);
        }

        if (r < 0) {
          res->result = r;

          return r;
        }
      }

      /* Set index and file record data to zeros. This is done even if the
       * file/dir removal was unsuccessful since that would msan that the 
       * entry in ix/fr files is wrong.
       */
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid,
                                            WE_AFII_IDX_RECORD_SIZE);
      op_data->bufsize = WE_AFII_IDX_RECORD_SIZE;
      op_data->datalen = 0;
      memset (op_data->buf, 0, WE_AFII_IDX_RECORD_SIZE);

      fri = afii_h->search->item;
      we_afif_file_seek (h, afii_h->idx_file, fri->file_pos);
      afii_h->state = WE_AFII_STATE_WRITE_IDX;

    case WE_AFII_STATE_WRITE_IDX:
      r = we_afif_file_write (h, wid, afii_h->idx_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (r < 0) {
        res->result = r;

        return r;
      }

      op_data->datalen += r;
      if ((r > 0) && (op_data->datalen < op_data->bufsize))
        continue;

      /*Now write file record*/
      WE_MEM_FREE (h->modid, op_data->buf);
      fri = afii_h->search->item;
      op_data->bufsize = fri->ir->size;
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid, op_data->bufsize);
      memset (op_data->buf, 0, op_data->bufsize);
      op_data->datalen = 0;
      if (we_afif_file_seek (h, afii_h->fr_file, fri->ir->pos) < 0) {
        res->result = WE_AFI_ERROR_INTERNAL;

        return WE_AFI_ERROR_INTERNAL;
      }
      afii_h->state = WE_AFII_STATE_WRITE_F_REC;

    case WE_AFII_STATE_WRITE_F_REC:
      r = we_afif_file_write (h, wid, afii_h->fr_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (r < 0) {
        res->result = r;

        return r;
      }

      op_data->datalen += r;
      if ((r > 0) && (op_data->datalen < op_data->bufsize))
        continue;

      goto done;
    }
  }

done:
  op_data->state = WE_AFII_STATE_OP_NULL;
  WE_MEM_FREE (h->modid, op_data->buf);
  op_data->buf = NULL;
  op_data->bufsize = 0;
  op_data->datalen = 0;
  res->result = WE_AFI_OK;
  WE_FILE_FLUSH ((h->files[afii_h->fr_file]).handle);
  WE_FILE_FLUSH ((h->files[afii_h->idx_file]).handle);

  return WE_AFI_OK;
}


/*****************************************************************
 * SET ATTRIBUTES
 *****************************************************************/
static int
we_afii_file_set_attr_task (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t         *h = (we_afi_handle_t *)handle;
  we_afii_handle_t        *afii_h;
  int                       n;
  we_afii_fr_item_t       *fri;
  we_afii_op_data_t       *op_data;
  we_afi_generic_result_t *res;
  we_afi_op_t             *op = find_op_by_id (h, wid);
  we_afi_open_t           *open;

  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  open = (we_afi_open_t *)op->op_data;
  op_data = afii_h->op_data;
  res = (we_afi_generic_result_t *)afii_h->result;
  for (;;) {
    switch (afii_h->state) {

    case WE_AFII_STATE_ADD_FILE:
      if ((n = we_afii_add_file_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }
      goto done;

    case WE_AFII_STATE_NULL:
    case WE_AFII_STATE_READ_IDX:
      if ((n = we_afii_read_idx_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }

      if (we_afii_file_record_search (h->modid, afii_h, open->src) == FALSE) {
        /* Setting an attribute on a file without attrs, thus add the file */
        char *filename = we_cmmn_strcat(h->modid, open->dir, open->src);
        /*check if file exist*/
        if (WE_FILE_GETSIZE (filename) >= 0 || WE_FILE_GETSIZE_DIR (filename)>= 0) {
          afii_h->state = WE_AFII_STATE_ADD_FILE;
          WE_MEM_FREE (h->modid,filename);
          break;
        }
        res->result = WE_AFI_ERROR_PATH;
        WE_MEM_FREE (h->modid,filename);
        return WE_AFI_ERROR_PATH;
      }
      afii_h->state = WE_AFII_STATE_READ_FR;

    case WE_AFII_STATE_READ_FR:
      if ((n = we_afii_read_file_record_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }

      op_data->frt = WE_MEM_ALLOCTYPE (h->modid, we_afii_file_record_t);
      if (! we_afii_cvt_to_fr (h, op_data->frt, op_data->buf,
                                op_data->datalen)) {
        WE_MEM_FREE (h->modid, op_data->frt);
        res->result = WE_AFI_ERROR_INTERNAL;

        return WE_AFI_ERROR_INTERNAL;
      }
      
      /* Check if filename is the found file */
      if (op_data->frt->name == NULL || strcmp (open->src, op_data->frt->name) != 0) {
        we_afii_fr_search_remove_first (h->modid, afii_h, FALSE);
        WE_MEM_FREE (h->modid, op_data->frt->name);
        we_pck_attr_free (h->modid, op_data->frt->attributes);
        WE_MEM_FREE (h->modid, op_data->frt);
        if (afii_h->search == NULL) {
          /* Setting an attribute on a file without attrs, thus add the file */
          char *filename = we_cmmn_strcat(h->modid, open->dir, open->src);
          if (WE_FILE_GETSIZE (filename) >= 0) {
            afii_h->state = WE_AFII_STATE_ADD_FILE;
            WE_MEM_FREE (h->modid, filename);
          }
          else {
            res->result = WE_AFI_ERROR_PATH;
            WE_MEM_FREE (h->modid,filename);
            return WE_AFI_ERROR_PATH;
          }
        }
        break;
      }

      /*Erase file record*/
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid, op_data->datalen);
      memset (op_data->buf, 0, op_data->datalen);
      op_data->bufsize = op_data->datalen;
      op_data->datalen = 0;
      fri = afii_h->search->item;
      if (we_afif_file_seek (h, afii_h->fr_file, fri->ir->pos) < 0) {
        res->result = WE_AFI_ERROR_INTERNAL;

        return WE_AFI_ERROR_INTERNAL;
      }

      afii_h->state = WE_AFII_STATE_RESET_F_REC;

    case WE_AFII_STATE_RESET_F_REC:
      n = we_afif_file_write (h, wid, afii_h->fr_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize)) {
        continue;
      }

      we_afii_init_add_file_write (h, afii_h);
      op_data->datalen = 0;
      op_data->bufsize = WE_AFII_IDX_RECORD_SIZE;
      WE_MEM_FREE (h->modid, op_data->frt->name);
      we_pck_attr_free (h->modid, op_data->frt->attributes);
      WE_MEM_FREE (h->modid, op_data->frt);

      fri = afii_h->search->item;
      we_afif_file_seek (h, afii_h->idx_file, fri->file_pos);
      afii_h->state = WE_AFII_STATE_WRITE_IDX;

    case WE_AFII_STATE_WRITE_IDX:
      n = we_afif_file_write (h, wid, afii_h->idx_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize)) {
        continue;
      }

      /*Now write file record*/
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = op_data->fr;
      op_data->fr = NULL;
      op_data->bufsize = op_data->fr_size;
      op_data->datalen = 0;
      if (we_afif_file_seek (h, afii_h->fr_file, op_data->fr_pos) < 0) {
        res->result = WE_AFI_ERROR_INTERNAL;

        return WE_AFI_ERROR_INTERNAL;
      }

      afii_h->state = WE_AFII_STATE_WRITE_F_REC;

    case WE_AFII_STATE_WRITE_F_REC:
      n = we_afif_file_write (h, afii_h->wid, afii_h->fr_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize)) {
        continue;
      }

      goto done;
    }
  }

done:
  op_data->state = WE_AFII_STATE_OP_NULL;
  WE_MEM_FREE (h->modid, op_data->buf);
  op_data->buf = NULL;
  op_data->bufsize = 0;
  op_data->datalen = 0;
  res->result = WE_AFI_OK;
  WE_FILE_FLUSH ((h->files[afii_h->fr_file]).handle);
  WE_FILE_FLUSH ((h->files[afii_h->idx_file]).handle);

  return WE_AFI_OK;
}


/* Rename a file. No attributes */
static int
rename_file (WE_UINT8 modid, we_afi_rename_t *ren)
{
  char *src = we_cmmn_strcat (modid, ren->dir, ren->src);
  char *dst = we_cmmn_strcat (modid, ren->dir, ren->dst);
  int   res = WE_FILE_RENAME (src, dst);

  WE_MEM_FREE (modid, src);
  WE_MEM_FREE (modid, dst);

  return res;
}


static int
we_afii_init_add_file_rename (we_afi_handle_t *h, we_afii_handle_t *afii_h)
{
  we_afii_op_data_t    *op_data = afii_h->op_data;
  we_afii_idx_record_t  ir;
  we_dcvt_t             obj;
  we_afi_op_t          *op = find_op_by_id (h, afii_h->wid);
  we_afi_rename_t      *ren = (we_afi_rename_t *)op->op_data;

  we_afii_create_fr (h->modid, IS_DIR (ren->dst), ren->dst, ren->attr,
                      &op_data->fr, &op_data->fr_size);

  we_afii_create_ir (afii_h, &ir, ren->dst, op_data->fr_size);
  op_data->fr_pos = ir.pos;
  we_dcvt_init (&obj, WE_DCVT_ENCODE, op_data->buf,
                 WE_AFII_IDX_BLOCK_SIZE, h->modid);
  if (!we_afii_cvt_idx_record (&obj, &ir)) {
    return WE_AFI_ERROR_INTERNAL;
  }

  return WE_AFI_OK;
}


/*****************************************************************
 * RENAME
 *****************************************************************/
static int
we_afii_file_rename_task (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t         *h = (we_afi_handle_t *)handle;
  we_afii_handle_t        *afii_h;
  we_afii_op_data_t       *op_data;
  int                       n;
  int                       flags;
  we_afii_fr_item_t       *fri;
  we_afi_generic_result_t *res;
  we_afi_op_t             *op = find_op_by_id (h, wid);
  we_afi_rename_t         *ren;

  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  ren = (we_afi_rename_t *)op->op_data;
  op_data = afii_h->op_data;
  res = ((we_afi_generic_result_t *)afii_h->result);
  for (;;) {
    switch (afii_h->state) {

    case WE_AFII_STATE_NULL:
      {
        char *dst = we_cmmn_strcat (h->modid, ren->dir, ren->dst);
        int   r;

        /* check if the new file name already exists */
        r = WE_FILE_OPEN (h->modid, dst, WE_FILE_SET_RDONLY, 0);
        WE_MEM_FREE (h->modid, dst);
        if (r > 0) {
          /* file exist */
          WE_FILE_CLOSE (r);
          res->result = WE_FILE_ERROR_EXIST;

          return WE_AFI_ERROR_EXIST;
        }
      }

    case WE_AFII_STATE_READ_IDX:
      if ((n = we_afii_read_idx_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }
      if (we_afii_file_record_search (h->modid, afii_h, ren->src) == FALSE) {
        /* no attributes, try hdi rename */
        res->result = rename_file (h->modid, ren);

        return res->result;
      }
      afii_h->state = WE_AFII_STATE_READ_FR;

    case WE_AFII_STATE_READ_FR:
      /* reading the source file record */
      if ((n = we_afii_read_file_record_task (h, afii_h)) != WE_AFI_OK) {
        res->result = n;

        return n;
      }
      {
        int success;

        op_data->frt = WE_MEM_ALLOCTYPE (h->modid, we_afii_file_record_t);
        success = we_afii_cvt_to_fr (h, op_data->frt, op_data->buf,
                                      op_data->datalen);

        if (strcmp (ren->src, op_data->frt->name) != 0) {
          we_afii_fr_search_remove_first (h->modid, afii_h, FALSE);
          WE_MEM_FREE (h->modid, op_data->frt->name);
          we_pck_attr_free (h->modid, op_data->frt->attributes);
          WE_MEM_FREE (h->modid, op_data->frt);

          if (afii_h->search == NULL) {
            char *filename = we_cmmn_strcat (h->modid, ren->dir, ren->src);
            int r = 0;

            /*Check if file or dir exists*/
            if (WE_FILE_GETSIZE (filename) >= 0 || 
                WE_FILE_GETSIZE_DIR (filename) >= 0) {
              r = rename_file (h->modid, ren);
            }
            else {
              r = WE_AFI_ERROR_PATH;
            }
            res->result = r;
            WE_MEM_FREE (h->modid, filename);
            return r;
          }
          break;
        }

        fri = afii_h->search->item;
        WE_MEM_FREE (h->modid, op_data->buf);
        op_data->buf = NULL;
        if (!success) {
          WE_MEM_FREE (h->modid, op_data->frt);
          res->result = WE_AFI_ERROR_INTERNAL;

          return WE_AFI_ERROR_INTERNAL;
        }
        ren->attr = op_data->frt->attributes;
      }

      /* check that file is not name protected */
      flags = get_attr_flags ((op_data->frt)->attributes);

      /* Write and name protection check */
      if ((flags & WE_PCK_FLAG_PROTECTED) | 
          (flags & WE_PCK_FLAG_USER_PROTECTED) |
          (flags & WE_PCK_FLAG_NAME_PROTECTED)) {
        we_pck_attr_free (h->modid, op_data->frt->attributes);
        WE_MEM_FREE (h->modid, op_data->frt->name);
        WE_MEM_FREE (h->modid, op_data->frt);
        res->result = WE_FILE_ERROR_ACCESS;

        return WE_FILE_ERROR_ACCESS;
      }

      /*Remove the file record from disk */
      op_data->bufsize = fri->ir->size;
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid, op_data->bufsize);
      memset (op_data->buf, 0, op_data->bufsize);
      op_data->datalen = 0;
      if (we_afif_file_seek (h, afii_h->fr_file, fri->ir->pos) < 0) {
        res->result = WE_AFI_ERROR_INTERNAL;

        return WE_AFI_ERROR_INTERNAL;
      }
      afii_h->state = WE_AFII_STATE_RESET_F_REC;

    case WE_AFII_STATE_RESET_F_REC:
      n = we_afif_file_write (h, wid, afii_h->fr_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize))
        continue;

      /* remove the item from the search list */
      we_afii_fr_search_remove_first (h->modid, afii_h, TRUE);

      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid, WE_AFII_IDX_RECORD_SIZE);
      op_data->datalen = 0;
      op_data->bufsize = WE_AFII_IDX_RECORD_SIZE;
      we_afii_init_add_file_rename (h, afii_h);

      op_data->pos = we_afii_fr_get_free_idx_rec (h, afii_h);
      if (op_data->pos < 0) {
        op_data->state = WE_AFII_STATE_OP_ADD_IDX_BLOCK_INIT;
        break;
      }

      WE_MEM_FREE (h->modid, op_data->frt->name);
      we_pck_attr_free (h->modid, op_data->frt->attributes);
      WE_MEM_FREE (h->modid, op_data->frt);

      we_afif_file_seek (h, afii_h->idx_file, op_data->pos);
      afii_h->state = WE_AFII_STATE_WRITE_IDX;

    case WE_AFII_STATE_WRITE_IDX:
      n = we_afif_file_write (h, wid, afii_h->idx_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize))
        continue;

      /*Now write file record*/
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = op_data->fr;
      op_data->fr = NULL;
      op_data->bufsize = op_data->fr_size;
      op_data->datalen = 0;
      if (we_afif_file_seek (h, afii_h->fr_file, op_data->fr_pos) < 0) {
        res->result = WE_AFI_ERROR_INTERNAL;

        return WE_AFI_ERROR_INTERNAL;
      }
      afii_h->state = WE_AFII_STATE_WRITE_F_REC;

    case WE_AFII_STATE_WRITE_F_REC:
      n = we_afif_file_write (h, afii_h->wid, afii_h->fr_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < op_data->bufsize))
        continue;

      /* Make the physical rename */
      res->result = rename_file (h->modid, ren);
      goto done;

    case WE_AFII_STATE_OP_ADD_IDX_BLOCK_INIT:
      WE_MEM_FREE (h->modid, op_data->buf);
      op_data->buf = (char *)WE_MEM_ALLOC (h->modid,
                                            WE_AFII_IDX_BLOCK_SIZE);
      memset (op_data->buf, 0, WE_AFII_IDX_BLOCK_SIZE);
      op_data->bufsize = WE_AFII_IDX_BLOCK_SIZE;
      op_data->datalen = 0;
      we_afif_file_seek (h, afii_h->idx_file,
                          (afii_h->n_idx_blocks * WE_AFII_IDX_BLOCK_SIZE));
      op_data->state   = WE_AFII_STATE_OP_ADD_IDX_BLOCK;

    case WE_AFII_STATE_OP_ADD_IDX_BLOCK:
      n = we_afif_file_write (h, afii_h->wid, afii_h->idx_file,
                               &op_data->buf[op_data->datalen],
                               op_data->bufsize - op_data->datalen);
      if (n < 0) {
        res->result = n;

        return n;
      }

      op_data->datalen += n;
      if ((n > 0) && (op_data->datalen < WE_AFII_IDX_BLOCK_SIZE))
        continue;

      afii_h->n_idx_blocks++;
      op_data->state = WE_AFII_STATE_OP_NULL;
      break;
    }
  }

done:
  op_data->state = WE_AFII_STATE_OP_NULL;
  WE_MEM_FREE (h->modid, op_data->buf);
  op_data->buf = NULL;
  op_data->bufsize = 0;
  op_data->datalen = 0;
  res->result = WE_AFI_OK;
  WE_FILE_FLUSH ((h->files[afii_h->fr_file]).handle);
  WE_FILE_FLUSH ((h->files[afii_h->idx_file]).handle);

  return WE_AFI_OK;
}


/*****************************************************************
 * WRITE TASK
 *****************************************************************/
static int
we_afii_file_write_task (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t         *h = (we_afi_handle_t *)handle;
  we_afii_handle_t        *afii_h;
  we_afi_generic_result_t *res;
  int                       n;
  we_afi_op_t             *op = find_op_by_id (h, wid);
  we_afi_write_t          *write;

  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  write = (we_afi_write_t *)(op->op_data);
  res = ((we_afi_generic_result_t *)afii_h->result);
  do {
    char *buf = (char *)(write->data);

    n = we_afif_file_write (h, wid, write->file_handle,
                             &buf[write->curr_pos],
                             write->data_size - write->curr_pos);
    res->result = n;
    if (n < 0) {
      /*If this was an error and not just a delay this operation is complete
        but when the result is fetched the buffer must not be deleted.
        Therefore set the buffer to NULL.*/
      return n;
    }
    write->curr_pos += n;
  } while ((n > 0) && (write->curr_pos < write->data_size));

  res->result = WE_AFI_OK;

  return WE_AFI_OK;
}


/*****************************************************************
 * READ TASK
 *****************************************************************/
static int
we_afii_file_read_task (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t      *h = (we_afi_handle_t *)handle;
  we_afii_handle_t     *afii_h;
  we_afii_op_data_t    *op_data;
  we_afi_result_read_t *res;
  int                    n;
  we_afi_op_t          *op = find_op_by_id (h, wid);
  we_afi_read_t        *read;

  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  read = (we_afi_read_t *)op->op_data;
  res = afii_h->result;
  op_data = afii_h->op_data;
  do {
    char *buf = (char *)(res->data);

    n = we_afif_file_read (h, wid, read->file_handle,
                            &buf[read->curr_pos],
                            read->data_size - read->curr_pos);
    res->result = n;
    if (n < 0) {
      if (n != WE_AFI_ERROR_EOF) {
       /*If this was an error and not just a delay this operation is complete
        but when the result is fetched the buffer must not be deleted.
        Therefore set the buffer to NULL.*/

        return n;
      }
    }
    else {
      read->curr_pos += n;
    }
  }
  while ((n > 0) && (read->curr_pos < read->data_size));

  res->data_len = read->curr_pos;

 /* This file must not be closed in the handle free function.*/
  op_data->buf = NULL;

  if (n == WE_AFI_ERROR_EOF) {
    return WE_AFI_ERROR_EOF;
  }
  res->result = WE_AFI_OK;

  return WE_AFI_OK;
}


static we_afii_op_data_t *
we_afii_create_op_data (WE_UINT8 modid)
{
  we_afii_op_data_t *op_data;

  op_data           = WE_MEM_ALLOCTYPE (modid, we_afii_op_data_t);
  op_data->state    = WE_AFII_STATE_OP_NULL;
  op_data->buf      = NULL;
  op_data->bufsize  = 0;
  op_data->datalen  = 0;
  op_data->pos      = 0;
  op_data->fr       = NULL;
  op_data->fr_pos   = 0;
  op_data->fr_size  = 0;
  op_data->ir_pos   = 0;
  op_data->frt      = NULL;

  return op_data;
}


/* returns the afii handle for the given wid or allocates a new if it does not
 * exist.
 */
static we_afii_handle_t *
we_afii_get_afii_handle (we_afi_handle_t *h, WE_INT32 wid)
{
  we_afii_handle_t *afii_h;

  /*Check if wid already taken*/
  if ((afii_h = we_afii_find_handle_by_id (h, wid)) != NULL) {
    return afii_h;
  }

  afii_h = WE_MEM_ALLOCTYPE (h->modid, we_afii_handle_t);
  afii_h->wid            = wid;
  afii_h->state         = WE_AFII_STATE_NULL;
  afii_h->init_state    = WE_AFII_STATE_NULL;
  afii_h->n_idx_blocks  = 0;
  afii_h->op_data       = we_afii_create_op_data (h->modid);
  afii_h->result        = NULL;
  afii_h->search        = NULL;
  afii_h->frl           = WE_MEM_ALLOCTYPE (h->modid, we_afii_fr_list_t);
  afii_h->frl->first    = NULL;
  afii_h->frl->n_items  = 0;
  afii_h->remove_frl    = WE_MEM_ALLOCTYPE (h->modid, we_afii_fr_list_t);
  afii_h->remove_frl->first = NULL;
  afii_h->remove_frl->n_items = 0;
  afii_h->next          = NULL;
  afii_h->search        = NULL;
  afii_h->fr_file       = NO_FILE;
  afii_h->idx_file      = NO_FILE;

  if (h->afii->handle_list == NULL) {
    h->afii->handle_list = afii_h;
  }
  else {
    afii_h->next = h->afii->handle_list;
    h->afii->handle_list = afii_h;
  }

  return afii_h;
}


static void
we_afii_free_op_data (we_afi_handle_t *h, we_afii_handle_t *afii_h)
{
  we_afii_op_data_t *op_data = afii_h->op_data;

  WE_MEM_FREE (h->modid, op_data->buf);
  WE_MEM_FREE (h->modid, op_data->fr);
  WE_MEM_FREE (h->modid, op_data);

  afii_h->op_data = NULL;
}

static void
we_afii_free_afii_handle (we_afi_handle_t *h, WE_INT32 wid)
{
  we_afii_handle_t *afii_h;

  /*Check if wid already taken*/
  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return;
  }

  afii_h->n_idx_blocks = 0;
  we_afif_file_close (h, afii_h->idx_file);
  afii_h->idx_file = NO_FILE;
  we_afif_file_close (h, afii_h->fr_file);
  afii_h->fr_file = NO_FILE;

  we_afii_free_op_data (h, afii_h);

  we_afii_fr_list_remove (h->modid, afii_h->frl);
  afii_h->frl = NULL;
  we_afii_fr_list_remove (h->modid, afii_h->remove_frl);
  afii_h->remove_frl = NULL;

  /* clean search list */
  while (afii_h->search != NULL) {
    we_afii_fr_search_remove_first (h->modid, afii_h, FALSE);
  }

  afii_h->state = WE_AFII_STATE_NULL;
  WE_MEM_FREE (h->modid, afii_h->result);

  we_afii_remove_handle_by_id (h, afii_h->wid);
}


static void
we_afii_free_handle (WE_UINT8 modid, we_afi_handle_t *h)
{
  we_afii_handle_t *afii_h;

  if (h->afii == NULL) {
    return;
  }

  /*Check if wid already taken*/
  afii_h = h->afii->handle_list;

  while (afii_h != NULL) {
    we_afii_handle_t *remove;

    we_afii_free_op_data (h, afii_h);
    we_afii_fr_list_remove (modid, afii_h->frl);

    remove = afii_h;
    afii_h = afii_h->next;
    WE_MEM_FREE (modid, remove);
  } 
  WE_MEM_FREE (modid, h->afii);
  h->afii = NULL;
}


/* common file open code for we_afi_open and we_afi_get_attributes */
static void
we_afii_file_open (we_afi_handle_t *h, WE_INT32 wid)
{
  we_afii_handle_t     *afii_h = we_afii_get_afii_handle (h, wid);
  we_afi_op_t          *op = find_op_by_id (h, wid);
  we_afi_open_t        *open = (we_afi_open_t *)(op->op_data);
  we_afi_result_open_t *res;
  int                    r;

  res = WE_MEM_ALLOCTYPE (h->modid, we_afi_result_open_t);
  res->file_handle = NO_FILE;
  res->result = WE_AFI_ERROR_DELAYED;
  afii_h->result = res;

  if ((r = we_afif_file_init (h, wid, open->mode, open->dir,
                               WE_AFI_IDX_FILE_NAME)) < 0) {
    res->result = r;
    return;
  }
  afii_h->idx_file = r;

  if ((r = we_afif_file_init (h, wid, open->mode, open->dir,
                               WE_AFI_FR_FILE_NAME)) < 0) {
    res->result = r;
    return;
  }
  afii_h->fr_file = r;

  we_afif_set_callback (h, afii_h->idx_file, we_afii_file_open_task);
  we_afif_set_callback (h, afii_h->fr_file, we_afii_file_open_task);
  we_afii_file_open_task (h, wid);
}


static void
we_afii_get_result_generic (we_afi_handle_t *h,
                             WE_INT32         wid,
                             WE_INT32        *result)
{
  we_afii_handle_t *afii_h;

  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    *result = WE_AFI_ERROR_INVALID;
    return;
  }

  if (afii_h->result == NULL) {
    *result = WE_AFI_ERROR_INVALID;
  }
  else {
    *result = ((we_afi_generic_result_t *)afii_h->result)->result;
  }

  if (*result != WE_AFI_ERROR_DELAYED) {
    we_afii_free_afii_handle (h, wid);
  }
}


static int
we_afii_file_exist (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t      *h = (we_afi_handle_t *)handle;
  we_afii_handle_t     *afii_h = we_afii_get_afii_handle (h, wid);
  int                    r;
  we_afi_op_t          *op = find_op_by_id (h, wid);
  we_afi_open_t        *open = (we_afi_open_t *)(op->op_data);
  we_afi_result_open_t *res = NULL;

  for (;;) {
    switch (afii_h->init_state) {

    case WE_AFII_STATE_NULL:
      res = WE_MEM_ALLOCTYPE (h->modid, we_afi_result_open_t);
      res->file_handle = NO_FILE;
      res->result = WE_AFI_ERROR_DELAYED;
      afii_h->result = res;

      if ((r = we_afif_file_init (h, wid, WE_AFI_RDONLY, open->dir,
                                   WE_AFI_IDX_FILE_NAME)) < 0) {
        res->result = r;
        return r;
      }
      afii_h->idx_file = r;

      if ((r = we_afif_file_init (h, wid, WE_AFI_RDONLY, open->dir,
                                   WE_AFI_FR_FILE_NAME)) < 0) {
        res->result = r;
        return r;
      }
      afii_h->fr_file = r;

      we_afif_set_callback (h, afii_h->idx_file, we_afii_file_exist);
      we_afif_set_callback (h, afii_h->fr_file, we_afii_file_exist);
      afii_h->init_state = WE_AFII_STATE_READ_IDX;

    case WE_AFII_STATE_READ_IDX:
      if ((r = we_afii_read_idx_task (h, afii_h)) != WE_AFI_OK) {
        res->result = r;
        return r;
      }
      if ((r == WE_AFI_OK) &&
          (we_afii_file_record_search (h->modid, afii_h, open->src) == TRUE)) {
        /* found at least one index entry that matches supplied filename */
        /* Have to read fr file*/
        afii_h->init_state = WE_AFII_STATE_READ_FR;
      }
      else {
        /* File does not have any attributes*/
        if (res) {
          res->result = WE_AFI_ERROR_PATH;
        }
        return WE_AFI_ERROR_PATH;
      }
      break;
    case WE_AFII_STATE_READ_FR:
      {
        we_afii_file_record_t   frt;

        if ((r = we_afii_read_file_record_task (h, afii_h)) != WE_AFI_OK) {
          res->result = r;
          return r;
        }

        if (! we_afii_cvt_to_fr (h, &frt, afii_h->op_data->buf, afii_h->op_data->datalen)) {
          res->result = WE_AFI_ERROR_INTERNAL;
          return WE_AFI_ERROR_INTERNAL;
        }

        /* Check if filename is the found file */
        if (frt.name != NULL && strcmp (open->src, frt.name) == 0) {
          res->result = WE_AFI_ERROR_EXIST;
          WE_MEM_FREE (h->modid, frt.name);
          we_pck_attr_free (h->modid, frt.attributes);
        }
        else {
          we_afii_fr_search_remove_first (h->modid, afii_h, FALSE);
          WE_MEM_FREE (h->modid, frt.name);
          we_pck_attr_free (h->modid, frt.attributes);
          if (afii_h->search != NULL) {
            afii_h->init_state = WE_AFII_STATE_READ_FR;
            break;
          }
          res->result = WE_AFI_ERROR_PATH;
        }
        WE_MEM_FREE (h->modid, afii_h->op_data->buf);
        afii_h->op_data->buf = NULL;
        return res->result;
      }
      break;

    default:
      r = WE_AFI_ERROR_INTERNAL;
    }
  }
  res = ((we_afi_result_open_t *)afii_h->result);
  res->result = r;

  return r;
}


/****************************************************************************
 *  AFII END
 ****************************************************************************/

/****************************************************************************
 *  AFI START
 ****************************************************************************/

/*
 * Initialise the AFI package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t *
we_afi_init (WE_UINT8 modid)
{
  we_afi_handle_t *h = WE_MEM_ALLOCTYPE (modid, we_afi_handle_t);
  int               i;

  if (h != NULL) {
    h->afii = NULL;
    h->modid = modid;
    h->op = NULL;
    h->we_afi_uniq_id = 1;

    for (i = 0; i < WE_AFIF_MAX_FILES; i++) {
      h->files[i].cb = NULL;
      h->files[i].handle = 0;
      h->files[i].wid = 0;
      h->files[i].pos = 0;
      h->files[i].type = 0;
    }
    h->afii = WE_MEM_ALLOCTYPE (modid, we_afii_t);
    h->afii->handle_list = NULL;
  }

  return (we_pck_handle_t *)h;
}


/*
 * Creates and add a new AFI operation to the list of current operations.
 */
static we_afi_op_t *
get_new_op (we_afi_handle_t *h, WE_UINT8 type)
{
  we_afi_op_t *op = WE_MEM_ALLOCTYPE (h->modid, we_afi_op_t);

  if (op != NULL) {
    op->wid = h->we_afi_uniq_id++;
    op->op_data = NULL;
    op->parent_id = NO_PARENT_ID;
    op->pipe = NO_PIPE;
    op->prev = NULL;
    op->priv_data = NULL;
    op->state = WE_AFI_STATE_NULL;
    op->type = type;

    /* insert new operation first in list */
    op->next = h->op;
  }
  if (h->op != NULL) {
    h->op->prev = op;
  }
  h->op = op;

  return op;
}


/*
 * Deletes an AFI operation.
 */
static void
free_op (we_afi_handle_t *handle, we_afi_op_t *op)
{
  /* take out op from the list */
  if ((op->prev != NULL) && (op->prev->next != NULL)) {
    op->prev->next = op->next;
  }
  if ((op->next != NULL) && (op->next->prev != NULL)) {
    op->next->prev = op->prev;
  }
  /* if first element in list to be removed, update the handle */
  if (handle->op == op) {
    handle->op = op->next;
  }

  WE_MEM_FREE (handle->modid, op);
}

/*
 * Finds the AFI operation with the wid supplied.
 * Returns NULL if no such operation exists.
 */
static we_afi_op_t *
find_op_by_id (we_afi_handle_t *h, WE_INT32 wid)
{
  we_afi_op_t *op = h->op;

  while (op != NULL) {
    if (op->wid == wid) {
      return op;
    }
    op = op->next;
  }

  return NULL;
}

/*
 * Finds the AFI operation with the pipe handle supplied.
 * Returns NULL if no such operation exists.
 */
static we_afi_op_t *
find_op_by_pipe (we_afi_handle_t *h, WE_INT32 pipe)
{
  we_afi_op_t *op = h->op;

  while (op != NULL) {
    if (op->pipe == pipe) {
      return op;
    }
    op = op->next;
  }

  return NULL;
}

/*
 * Send a file notify or pipe notify to the module to continue operation.
 */
static void
we_afi_send_signal_notity (WE_UINT8 modid,
                            int       handle,
                            int       data_type,
                            int       eventType)
{
  if (data_type == WE_PCK_DATA_TYPE_PIPE) {
    we_pipe_notify_t  p;
    we_dcvt_t         cvt_obj;
    WE_UINT16         length;
    void              *signal_buffer;
    void              *user_data;

    p.handle = handle;
    p.eventType = (WE_INT16)eventType;

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    we_cvt_pipe_notify (&cvt_obj, &p);
    length = (WE_UINT16)cvt_obj.pos;

    signal_buffer = WE_SIGNAL_CREATE (WE_SIG_PIPE_NOTIFY, modid, modid, length);
    if (signal_buffer != NULL) {
      user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
      we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
      we_cvt_pipe_notify (&cvt_obj, &p);

      WE_SIGNAL_SEND (signal_buffer);
    }
  }
  else {
    we_file_notify_t  p;
    we_dcvt_t         cvt_obj;
    WE_UINT16         length;
    void              *signal_buffer;
    void              *user_data;

    p.fileHandle = handle;
    p.eventType = (WE_INT16)eventType;

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    we_cvt_file_notify (&cvt_obj, &p);
    length = (WE_UINT16)cvt_obj.pos;

    signal_buffer = WE_SIGNAL_CREATE (WE_SIG_FILE_NOTIFY, modid, modid,
                                       length);
    if (signal_buffer != NULL) {
      user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
      we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
      we_cvt_file_notify (&cvt_obj, &p);

      WE_SIGNAL_SEND (signal_buffer);
    }
  }
}


/*
 * Set private data connected to a specific AFI identifier. The private data
 * can be used, for instance, by the module to indicate which sub module that
 * invoked the operation.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_afi_set_private_data (we_pck_handle_t *handle, WE_INT32 wid, void *p)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = find_op_by_id (h, wid);

  if (op != NULL) {
    op->priv_data = p;

    return TRUE;
  }

  return FALSE;
}


/* Get private data connected to a specific AFI identifier. The private data
 * can be used, for instance by the module to indicate where to continue
 * execution.
 * If success returns the private data, otherwise returns NULL.
 */
void *
we_afi_get_private_data (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t *h  = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = find_op_by_id (h, wid);

  if (op != NULL) {
    return op->priv_data;
  }

  return NULL;
}


/* Terminates the AFI package, release all resources allocated by
 * this package. If there are any open files or other operations,
 * these will be deleted. Any private data must be released by the
 * module itself before this function is called.
 */
int
we_afi_terminate (we_pck_handle_t *handle)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;

  if (h != NULL) {
    we_afi_op_t *op = h->op;

    while (op != NULL) {
      we_afi_op_t *tmp_op = op;

      
      op = op->next;
      WE_MEM_FREE (h->modid, tmp_op);
    }
    we_afii_free_handle (h->modid, handle);
    WE_MEM_FREE (h->modid, h);
  }

  return TRUE;
}


/**********************************************************************
 * Open
 **********************************************************************/
/* Initialise the specified file for read/write operations. It it not
 * necessary to open a file before its attributes are set/retrieved.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of
 *            the file to open. The filename cannot end with a '/'.
 * mode       The open mode to be used. Possible modes are:
 *              WE_AFI_RDONLY
 *              WE_AFI_WRONLY
 *              WE_AFI_RDWR
 * attr_list  The list of file attributes that are set on creation.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_open (we_pck_handle_t *handle, const char *file_name, int mode)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = get_new_op (h, WE_AFI_OPEN);
  we_afi_fh_t     *fh;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  fh = WE_MEM_ALLOCTYPE (h->modid, we_afi_fh_t);
  fh->file_handle = we_afif_file_init (h, op->wid, mode, NULL, file_name);
  op->op_data = (void *)fh;

  return op->wid;
}


/*
 * Gets the result from the Open operation.
 *
 * Arguments:
 * handle       The handle to the struct that holds the file package instance
 *              data.
 * wid           The wid returned with the Open operation
 * result       File handle or negative return code:
 *                WE_AFI_ERROR_ACCESS 
 *                WE_AFI_ERROR_DELAYED
 *                WE_AFI_ERROR_PATH
 *                WE_AFI_ERROR_INVALID
 *                WE_AFI_ERROR_FULL
 *                WE_AFI_ERROR_EXIST
 * Returns:     TRUE if success, otherwise FALSE. Note that this value msan
 *              that this function call was successful but it does not
 *              indicate that the file is opened.
 */
static int
we_afi_get_result_open (we_afi_handle_t      *h,
                         WE_INT32              wid,
                         we_afi_result_open_t *result)
{
  we_afi_op_t *op = find_op_by_id (h, wid);
  we_afi_fh_t *fh;

  if (op == NULL) {
    result->file_handle = NO_FILE;
    result->result = -1;

    return WE_PACKAGE_ERROR;
  }

  fh = (we_afi_fh_t *)op->op_data;
  if (fh->file_handle  >= 0) {
    result->file_handle = fh->file_handle;
    result->result = WE_AFI_OK;
  }
  else {
    result->file_handle = fh->file_handle;
    result->result = fh->file_handle;
  }

  WE_MEM_FREE (h->modid, fh);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Close
 **********************************************************************/
/* Close a file using the file handle returned from we_afi_open.
 *
 * handle       The handle to the struct that holds the file package
 *              instance data.
 * file_handle  File handle returned from the we_afi_open operation.
 *
 * Returns:     An wid or WE_PACKAGE_ERROR
 */
WE_INT32
we_afi_close (we_pck_handle_t* handle, WE_INT32 file_handle)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = get_new_op (h, WE_AFI_CLOSE);

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  we_afif_file_close (h, file_handle);

  return op->wid;
}

/* Gets the result from the Close call. Since the we_afi_close() function  
 * may be synchronous this function must be called directly after the 
 * we_afi_close call.
 *
 * Arguments:
 * 
 * handle      The handle to the struct that holds the file package instance
 *             data.
 * wid          The wid returned from the close operation
 * result      The function initializes this parameter with current status 
 *             code of the operation:
 *               WE_AFI_OK           
 *               WE_AFI_ERROR_DELAYED
 *               WE_AFI_ERROR_INVALID
 *
 * Returns:    TRUE if success, otherwise FALSE. Note that this value  
 *             msan that this function call was successful but it does 
 *             not indicate that the file is closed.
 */
static int
we_afi_get_result_close (we_afi_handle_t *h,
                          WE_INT32         wid,
                          WE_INT32        *result)
{
  we_afi_op_t *op = find_op_by_id (h, wid);

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  *result = WE_AFI_OK;
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Write
 **********************************************************************/
/* Writes the specified number of bytes to the file pointed at by
 * the file_handle parameter. The function does not return finished
 * until all bytes have been written. If all bytes cannot be written the 
 * function returns an error
 *
 * NOTE! It is not possible to make more than one (1) simultaneous write 
 *       operation on the same file handle.
 *
 * handle       The handle to the struct that holds the file package
 *              instance data.
 * file_handle  File handle returned from the we_afi_open operation.
 * data         The data to be written.
 * data_len     The size in bytes of the data parameter.
 *
 * Returns:     An wid for operation or WE_PACKAGE_ERROR if failure.
 */
WE_INT32
we_afi_write (we_pck_handle_t *handle,
               WE_INT32         file_handle,
               void             *data,
               long              data_len)
{
  we_afi_handle_t         *h = (we_afi_handle_t *)handle;
  we_afi_op_t             *op;
  we_afii_handle_t        *afii_h;
  we_afi_generic_result_t *res;
  we_afi_write_t          *write;

  if (file_handle < 0) {
    return WE_PACKAGE_ERROR;
  }

  op = get_new_op (h, WE_AFI_WRITE);
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  afii_h = we_afii_get_afii_handle (h, op->wid);
  write = WE_MEM_ALLOCTYPE (h->modid, we_afi_write_t);
  write->curr_pos = 0;
  write->data = data;
  write->data_size = data_len;
  write->file_handle = file_handle;
  op->op_data = (void *)write;

  res = WE_MEM_ALLOCTYPE (h->modid, we_afi_generic_result_t);
  afii_h->result = res;
  we_afif_set_callback (h, file_handle, we_afii_file_write_task);
  we_afii_file_write_task (h, op->wid);

  return op->wid;
}

/*
 * Gets the result from the open package. Since the we_afi_write() function
 * may synchronous this function must be called directly after
 * the Write call.
 *
 * Arguments:
 * 
 * handle      The handle to the struct that holds the file package instance
 *             data.
 * wid          The wid returned with the Open operation
 * result      The function initializes this parameter with current status 
 *             code of the operation:
 *               WE_AFI_OK
 *               WE_AFI_ERROR_ACCESS
 *               WE_AFI_ERROR_DELAYED
 *               WE_AFI_ERROR_INVALID
 *               WE_AFI_ERROR_SIZE
 *               WE_AFI_ERROR_FULL
 *
 * Returns:    TRUE if success, otherwise FALSE. Note that this value msan
 *             that this function call was successful but it does not
 *             indicate that the data is written.
 */
static int
we_afi_get_result_write (we_afi_handle_t *h,
                          WE_INT32         wid,
                          WE_INT32        *result)
{
  we_afi_op_t    *op = find_op_by_id (h, wid);
  we_afi_write_t *write;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  we_afii_get_result_generic (h, wid, result);
  if (*result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }
  write = (we_afi_write_t *)(op->op_data);
  we_afi_free_write (h->modid, write);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Read
 **********************************************************************/
/* Reads the specified number of bytes from the file associated with
 * the file_handle parameter. The function does not return finished
 * until all bytes have been read. If it is not possible to read the
 * specified number of bytes the number of actually read bytes will
 * be returned in the we_afi_get_result call.
 *
 * handle       The handle to the struct that holds the file package
 *              instance data.
 * file_handle  File handle returned from the we_afi_open operation.
 * data         Data buffer.
 * data_len     Number of bytes to read into the data buffer.
 *
 * Returns:     An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_read (we_pck_handle_t  *handle,
              WE_INT32          file_handle,
              void              *data,
              long               data_len)
{
  we_afi_handle_t      *h = (we_afi_handle_t *)handle;
  we_afi_op_t          *op = get_new_op (h, WE_AFI_READ);
  we_afii_handle_t     *afii_h = we_afii_get_afii_handle (h, op->wid);
  we_afi_result_read_t *res;
  we_afi_read_t        *read;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  read = WE_MEM_ALLOCTYPE (h->modid, we_afi_read_t);
  read->curr_pos = 0;
  read->data_size = data_len;
  read->file_handle = file_handle;
  op->op_data = (void *)read;

  res = WE_MEM_ALLOCTYPE (h->modid, we_afi_result_read_t);
  res->result = WE_AFI_ERROR_DELAYED;
  res->data = data;
  res->data_len = 0; /* start with zero bytes read */
  afii_h->result = res;

  we_afif_set_callback (h, file_handle, we_afii_file_read_task);
  we_afii_file_read_task (h, op->wid);

  return op->wid;
}

/*
 * Gets the result from the Read operation. Since the we_afi_read()
 * operation may be synchronous this function must be called directly after
 * the Read call.
 *
 * Arguments:
 * 
 * handle      The handle to the struct that holds the file package instance
 *             data.
 * wid          The wid returned with the Open operation
 * result      The function initializes this structure with the result of
 *             the read operation.
 * Returns:    TRUE if success, otherwise FALSE. Note that this value msan
 *             that this function call was successful but it does not 
 *             indicate that the data has been read.
 */
static int
we_afi_get_result_read (we_afi_handle_t       *h,
                         WE_INT32               wid,
                         we_afi_result_read_t  *result)
{
  we_afi_op_t          *op = find_op_by_id (h, wid);
  we_afii_handle_t     *afii_h;
  we_afi_result_read_t *res;
  we_afi_read_t        *read;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }

  read = (we_afi_read_t *)op->op_data;
  res = (we_afi_result_read_t *)afii_h->result;

  /* hand over result to caller */
  result->result   = res->result;
  result->data_len = res->data_len;
  result->data     = res->data;

  if (res->result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }

  we_afi_free_read (h->modid, read);
  we_afii_free_afii_handle (h, wid);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Get attributes
 **********************************************************************/
/* Get attributes associated with a specific file.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of the file
 *            that has the attributes that shall be returned.
 * bit_mask   NOTE: NOT IMPLEMENTED.
 *            Bit mask to specify which attributes to return. If this flag
 *            is set to 0 all attributes will be returned.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR,
 */
WE_INT32
we_afi_get_attributes (we_pck_handle_t *handle,
                        const char       *file_name,
                        WE_UINT32        bit_mask)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = get_new_op (h, WE_AFI_GET_ATTR);
  we_afi_open_t   *open;

  WE_UNUSED_PARAMETER (bit_mask); 
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  open = WE_MEM_ALLOCTYPE (h->modid, we_afi_open_t);
  we_afi_split_path (h->modid, file_name, &(open->dir), &(open->src));
  open->mode = WE_AFI_RDONLY;
  open->attr = NULL;
  op->op_data = (void *)open;

  we_afii_file_open (h, op->wid);

  return op->wid;
}

/*
 * Gets the result from the Get Attribute operation. Since the
 * we_afi_get_attributes() operation may be asynchronous, this function
 * must be called directly after the get attribute function.
 *
 * Arguments:
 *
 * handle   The handle to the struct that holds the file package
 *          instance data.
 * wid       The operation wid
 * result   result structure to be initialized by this function.
 *          Possible result codes:
 *            WE_AFI_OK
 *            WE_AFI_ERROR_DELAYED
 *            WE_AFI_ERROR_PATH
 *
 * Returns: TRUE if success, otherwise FALSE. Note that this value msan
 *          that this function call was successful but it does not 
 *          indicate that the attributes has been read.
 */
static int
we_afi_get_result_get_attr (we_afi_handle_t                        *h,
                             WE_INT32                                wid,
                             we_afi_result_get_attributes_t ** const result)
{
  we_afi_op_t          *op = find_op_by_id (h, wid);
  we_afii_handle_t     *afii_h;
  we_afi_result_open_t *res;
  we_afi_open_t        *open;
  char                  *path;
  long                   size;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  *result = WE_MEM_ALLOCTYPE (h->modid, we_afi_result_get_attributes_t);
  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    (*result)->result = WE_AFI_ERROR_INVALID;

    return WE_PACKAGE_ERROR;
  }

  /* since open was used for retrieving attributes */
  open = (we_afi_open_t *)(op->op_data);
  res = ((we_afi_result_open_t *)afii_h->result);
  (*result)->result = res->result;
  (*result)->attr_list = open->attr;
  if (res->result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }


  /* If result is negative and not delayed it does not have any attrs.
   * Simulate attributes by adding filesize and mime attrs.
   */
  if (res->result < 0) {
    path = we_cmmn_strcat (h->modid, open->dir, open->src);

    /* retrieve size of the file in question */
    if ((size = WE_FILE_GETSIZE (path)) < 0) {
      (*result)->result = WE_AFI_ERROR_PATH;
    }
    else {
      (*result)->result = WE_AFI_OK;
    }
    (*result)->attr_list = get_filesize_attr (h->modid, size);
    (*result)->attr_list->next = we_get_mime_attr (h->modid, path);
    WE_MEM_FREE (h->modid, path);
  }
  else if (IS_DIR (open->src) == FALSE) {
    /* check if derived attributes mime and size, must be added */

    we_pck_attr_list_t *attr;

    if (we_afi_has_size_attr ((*result)->attr_list) == FALSE) {
      path = we_cmmn_strcat (h->modid, open->dir, open->src);
      if ((size = WE_FILE_GETSIZE (path)) < 0) {
        size = 0;
      }
      attr = (*result)->attr_list;
      (*result)->attr_list = get_filesize_attr (h->modid, size);  /* never returns NULL */
      (*result)->attr_list->next = attr;
      WE_MEM_FREE (h->modid, path);
    }
    if (we_afi_has_mime_attr ((*result)->attr_list) == FALSE) {
      attr = (*result)->attr_list;
      (*result)->attr_list = we_get_mime_attr (h->modid, open->src); /* never returns NULL */
      (*result)->attr_list->next = attr;
    }
  }
  open->attr = NULL; /* hand over attributes to user */
  we_afi_free_open (h->modid, open);
  we_afii_free_afii_handle (h, wid);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Seek
 **********************************************************************/
/* Moves the file pointer of an open file, If the seek operation exceeds the
 * limits of the file an error is returned. Offset may be negative.
 * 
 * NOTE! This function is always synchronous and therefore do not implement
 *       a get result function.
 *
 * handle      The handle to the struct that holds the file package instance
 *             data.
 * file_handle AFI File handle returned from the Open operation   
 * offset      Distance in bytes to move 
 * seek_mode   Where from the offset should be counted
 *             TPI_FILE_SEEK_SET offset from start of file
 *             TPI_FILE_SEEK_CUR relative offest from current position
 *             TPI_FILE_SEEK_END offset from end of file  
 *
 * Returns:    The new position of the file pointer, or a negative value
 *             if an error occurred - error codes:
 *
 *             WE_AFI_ERROR_ACCESS The file was opened in 
 *                TPI_FILE_SET_APPEND mode. 
 *             WE_AFI_ERROR_INVALID e.g. the fileHandle argument is not
 *                an open file descriptor or the requested new position
 *                exceeds the limits of the file.
 */
WE_INT32
we_afi_seek (const we_pck_handle_t *handle,
              WE_INT32               file_handle,
              long                    offset,
              int                     seek_mode)
{
  we_afi_handle_t *h    = (we_afi_handle_t *)handle;
  we_afif_file_t  *file = NULL;

  if (file_handle >= WE_AFIF_MAX_FILES) {
    return WE_AFI_ERROR_INVALID;
  }
  file = &(h->files[file_handle]);
  if (file == NULL) {
    return WE_AFI_ERROR_INVALID;
  }

  return WE_FILE_SEEK (file->handle, offset, seek_mode);
}


/**********************************************************************
 * Remove
 **********************************************************************/
/* Remove a file or an empty directory.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * item       Null terminated string containing the unique name of the file
 *            or directory to delete.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_remove (we_pck_handle_t *handle, const char *item)
{
  we_afi_handle_t         *h = (we_afi_handle_t *)handle;
  we_afi_op_t             *op = get_new_op (h, WE_AFI_REMOVE);
  we_afii_handle_t        *afii_h = we_afii_get_afii_handle (h, op->wid);
  we_afi_generic_result_t *res;
  we_afi_open_t           *open;
  int                       r;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  open = WE_MEM_ALLOCTYPE (h->modid, we_afi_open_t);
  we_afi_split_path (h->modid, item, &(open->dir), &(open->src));
  open->mode = WE_AFI_RDWR;
  open->attr = NULL;
  op->op_data = (void *)open;

  res = WE_MEM_ALLOCTYPE (h->modid, we_afi_generic_result_t);
  res->result = WE_AFI_ERROR_DELAYED;
  afii_h->result = res;

  /* forbidden to remove a URF */
  if ((open->dir != NULL) &&
      (we_pck_is_uf (h->modid, open->dir, open->src) == TRUE)) {
    res->result = WE_AFI_ERROR_ACCESS;
    return op->wid;
  }

  if ((r = we_afif_file_init (h, op->wid, WE_AFI_RDWR, open->dir,
                               WE_AFI_IDX_FILE_NAME)) < 0) {
    res->result = r;
    return op->wid;
  }
  afii_h->idx_file = r;

  if ((r = we_afif_file_init (h, op->wid, WE_AFI_RDWR, open->dir,
                               WE_AFI_FR_FILE_NAME)) < 0) {
    res->result = r;
    return op->wid;
  }
  afii_h->fr_file = r;

  we_afif_set_callback (h, afii_h->idx_file, we_afii_file_remove_task);
  we_afif_set_callback (h, afii_h->fr_file, we_afii_file_remove_task);
  we_afii_file_remove_task (h, op->wid);

  return op->wid;
}

static int
we_afi_get_result_remove (we_afi_handle_t *h,
                           WE_INT32         wid,
                           WE_INT32        *result)
{
  we_afi_op_t   *op = find_op_by_id (h, wid);
  we_afi_open_t *open;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  we_afii_get_result_generic (h, wid, result);
  if (*result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }

  open = (we_afi_open_t *)op->op_data;
  we_afi_free_open (h->modid, open);
  free_op (h, op);
  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Set attributes
 **********************************************************************/
/* Set file attributes for a specific file.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of the file
 *            for which the attributes will be set.
 * attr_list  A list of attributes.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_set_attributes (we_pck_handle_t          *handle,
                        const char                *file_name,
                        const we_pck_attr_list_t *attr_list)
{
  we_afi_handle_t         *h  = (we_afi_handle_t *)handle;
  we_afi_op_t             *op = get_new_op (h, WE_AFI_SET_ATTR);
  we_afi_open_t           *open;
  int                       r;
  we_afii_handle_t        *afii_h = we_afii_get_afii_handle (h, op->wid);
  we_afi_generic_result_t *res;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  open = WE_MEM_ALLOCTYPE (h->modid, we_afi_open_t);
  we_afi_split_path (h->modid, file_name, &(open->dir), &(open->src));
  open->mode = WE_AFI_RDWR;
  open->attr = we_pck_attr_dupl (h->modid, attr_list);
  op->op_data = (void *)open;

  res = WE_MEM_ALLOCTYPE (h->modid, we_afi_generic_result_t);
  res->result = WE_AFI_ERROR_DELAYED;
  afii_h->result = res;

  if ((r = we_afif_file_init (h, op->wid, WE_AFI_RDWR, open->dir,
                               WE_AFI_IDX_FILE_NAME)) < 0) {
    res->result = r;
    return op->wid;
  }
  afii_h->idx_file = r;

  if ((r = we_afif_file_init (h, op->wid, WE_AFI_RDWR, open->dir,
                               WE_AFI_FR_FILE_NAME)) < 0) {
    res->result = r;
    return op->wid;
  }
  afii_h->fr_file = r;

  we_afif_set_callback (h, afii_h->idx_file, we_afii_file_set_attr_task);
  we_afif_set_callback (h, afii_h->fr_file, we_afii_file_set_attr_task);
  we_afii_file_set_attr_task (h, op->wid);

  return op->wid;
}

/*
 * Gets the result from the set attributes operation.
 *
 * Arguments:
 * handle      The handle to the struct that holds the file package instance
 *             data.
 * wid          The wid to the set attributes instance
 * result      The function initializes this parameter with current status 
 *             code of the operation:
 *               WE_AFI_OK
 *               WE_AFI_ERROR_DELAYED 
 *               WE_AFI_ERROR_PATH
 *               WE_AFI_ERROR_ACCESS 
 *               WE_AFI_ERROR_SIZE
 *               WE_AFI_ERROR_FULL
 *
 * Returns:    TRUE if success, otherwise FALSE. Note that this value msan
 *             that this function call was successful but it does not
 *             indicate that the attributes has been set.
 */
static int
we_afi_get_result_set_attributes (we_pck_handle_t *handle,
                                   WE_INT32         wid,
                                   WE_INT32        *result)
{
  we_afi_handle_t *h  = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = find_op_by_id (h, wid);
  we_afi_open_t   *open;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  we_afii_get_result_generic (h, wid, result);
  if (*result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }
  open = (we_afi_open_t *)op->op_data;
  we_afi_free_open (h->modid, open);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Get dir entries
 **********************************************************************/
/* Retrieves a list of all files and subdirectories for a given directory.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * dir_name   Null terminated string containing the unique name of the
 *            directory that shall be read.
 * bit_mask   NOTE: NOT IMPLEMENTED
 *            Bit mask to specify which attributes for each file and
 *            folder to include in the result.
 *            If this flag is set to 0 all attributes will be returned.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR,
 */
WE_INT32
we_afi_get_dir_entries (we_pck_handle_t *handle,
                         const char       *dir_name,
                         WE_UINT32        bit_mask)
{
  we_afi_handle_t             *h = (we_afi_handle_t *)handle;
  we_afi_op_t                 *op = get_new_op (h, WE_AFI_GET_DIR);
  we_afi_dir_t                *dir;
  we_afii_handle_t            *afii_h = we_afii_get_afii_handle (h, op->wid);
  we_afi_result_dir_entries_t *res;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  WE_UNUSED_PARAMETER (bit_mask); 
  res = WE_MEM_ALLOCTYPE (h->modid, we_afi_result_dir_entries_t);
  res->entry_list[0].attr_list = NULL;
  res->entry_list[0].is_dir = FALSE;
  res->entry_list[0].name = NULL;
  res->num_entries = 0;
  res->result = WE_AFI_ERROR_DELAYED;
  afii_h->result = (void *)res;

  dir = WE_MEM_ALLOCTYPE (h->modid, we_afi_dir_t);
  dir->dir = we_cmmn_strdup (h->modid, dir_name);
  op->op_data = (void *)dir;

  we_afii_get_direntries_task (h, op->wid);

  return op->wid;
}


static we_afi_result_dir_entries_t *
get_result_dir_entries_struct (WE_UINT8 modid, int value)
{
  we_afi_result_dir_entries_t *res;

  res = WE_MEM_ALLOCTYPE (modid, we_afi_result_dir_entries_t);
  res->entry_list[0].attr_list = NULL;
  res->entry_list[0].is_dir = FALSE;
  res->entry_list[0].name = NULL;
  res->num_entries = 0;
  res->result = value;

  return res;
}

/*
 * Gets the result from the get dir entries operation.
 */
static int
we_afi_get_result_get_dir_entries (we_afi_handle_t               *h,
                                    WE_INT32                       wid,
                                    we_afi_result_dir_entries_t  **result)
{
  we_afi_op_t                 *op = find_op_by_id (h, wid);
  we_afii_handle_t            *afii_h;
  we_afi_result_dir_entries_t *ls;
  we_afi_dir_t                *dir;
  we_afi_result_dir_entries_t *res;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if ((afii_h = we_afii_find_handle_by_id (h, wid)) == NULL) {
    return WE_PACKAGE_ERROR;
  }

  /*If the Get dir entries operation is not finished make a copy of 
    the result struct but if the operation is complete assign current
    result pointer and reset to NULL. This pointer will be released in 
    the result free function*/

  res = (we_afi_result_dir_entries_t *)(afii_h->result);
  if (res->result == WE_AFI_ERROR_DELAYED) {
    /* delayed; keep data */
    *result = get_result_dir_entries_struct (h->modid, res->result);

    return WE_PACKAGE_BUSY;
  }

  /* handover result to user */
  *result = ((we_afi_result_dir_entries_t *)afii_h->result);
  afii_h->result = NULL;

  dir = (we_afi_dir_t *)op->op_data;
  if (res->result == WE_AFI_OK) {
    int i;

    ls = *result;
    /* Add size and mimetype to files without attributes */
    for (i = 0; i < ls->num_entries; i++) {
      we_pck_attr_list_t *attr;

      if (ls->entry_list[i].is_dir == FALSE) {
        if (we_afi_has_size_attr (ls->entry_list[i].attr_list) == FALSE) {
          char *path;
          long  size;

          path = we_cmmn_strcat (h->modid, dir->dir, ls->entry_list[i].name);
          if ((size = WE_FILE_GETSIZE (path)) < 0) {
            size = 0;
          }
          attr = ls->entry_list[i].attr_list;
          ls->entry_list[i].attr_list = get_filesize_attr (h->modid, size);
          ls->entry_list[i].attr_list->next = attr;
          WE_MEM_FREE (h->modid, path);
        }
        if (we_afi_has_mime_attr (ls->entry_list[i].attr_list) == FALSE) {
          attr = ls->entry_list[i].attr_list;
          ls->entry_list[i].attr_list =
                      we_get_mime_attr (h->modid, ls->entry_list[i].name);
          ls->entry_list[i].attr_list->next = attr;
        }
      }
    }
  }
  we_afii_free_afii_handle (h, wid);
  we_afi_free_dir (h->modid, dir);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}

/**********************************************************************
 * Rename
 **********************************************************************/
/*
 * Gets the result from the rename package. Since the 
 * we_afi_rename() function may be synchronous, 
 * this function must be called directly after the we_afi_rename call.
 *
 * Arguments:
 * 
 * handle      The handle to the struct that holds the file package instance
 *             data.
 * wid          The wid returned from we_afi_rename
 * result      this function sets this parameter to the result code from 
 *             the we_afi_rename call:
 *               WE_AFI_OK
 *               WE_AFI_ERROR_DELAYED
 *               WE_AFI_ERROR_PATH
 *               WE_AFI_ERROR_EXIST
 * 
 * Returns:    TRUE if success, otherwise FALSE. Note that this value msan
 *             that this function call was successful but it does not 
 *             indicate that the file/directory is renamed.
 */
static int
we_afi_get_result_rename (we_afi_handle_t *h,
                           WE_INT32         wid,
                           WE_INT32        *result)
{
  we_afi_op_t     *op = find_op_by_id (h, wid);
  we_afi_rename_t *ren = (we_afi_rename_t *)op->op_data;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  we_afii_get_result_generic (h, wid, result);
  if (*result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }

  /* free rename data struct */
  WE_MEM_FREE (h->modid, ren->dir);
  WE_MEM_FREE (h->modid, ren->dst);
  WE_MEM_FREE (h->modid, ren->src);
  WE_MEM_FREE (h->modid, ren);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Rename
 **********************************************************************/
/* Rename the file or directory specified by old_name to the name
 * specified by new_name.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * old_name   Null terminated string with the old file or directory name.
 * new_name   Null terminated string with the new file or directory name.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR,
 */
WE_INT32
we_afi_rename (we_pck_handle_t *handle,
                const char       *old_name,
                const char       *new_name)
{
  we_afi_handle_t         *h  = (we_afi_handle_t *)handle;
  we_afi_op_t             *op = get_new_op (h, WE_AFI_RENAME);
  we_afi_rename_t         *ren;
  we_afii_handle_t        *afii_h;
  we_afi_generic_result_t *res;
  int                       r;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  afii_h = we_afii_get_afii_handle (h, op->wid);
  ren = WE_MEM_ALLOCTYPE (h->modid, we_afi_rename_t);
  we_afi_split_path (h->modid, old_name, &(ren->dir), &(ren->src));
  we_afi_split_path (h->modid, new_name, NULL, &(ren->dst));
  ren->attr = NULL;
  op->op_data = (void *)ren;

  res = WE_MEM_ALLOCTYPE (h->modid, we_afi_generic_result_t);
  res->result = WE_AFI_ERROR_DELAYED;
  afii_h->result = res;

  if (ren->dst == NULL) {
    res->result = WE_AFI_ERROR_ACCESS;
    return op->wid;
  }
  /* forbidden to rename a URF */
  if ((ren->dir != NULL) &&
      (we_pck_is_uf (h->modid, ren->dir, ren->dst) == TRUE)) {
    res->result = WE_AFI_ERROR_ACCESS;
    return op->wid;
  }

  /* Check source and destination filename */
  if ((ren->dst == NULL) || (ren->src == NULL)) {
    res->result = WE_AFI_ERROR_PATH;
    return op->wid;
  }

  /* If source and destination are the same, this is a no op. */
  if (strcmp (ren->dst, ren->src) == 0) {
    res->result = WE_AFI_OK;
    return op->wid;
  }

  if ((r = we_afif_file_init (h, op->wid, WE_AFI_RDWR, ren->dir,
                               WE_AFI_IDX_FILE_NAME)) < 0) {
    res->result = r;
    return op->wid;
  }
  afii_h->idx_file = r;

  if ((r = we_afif_file_init (h, op->wid, WE_AFI_RDWR, ren->dir,
                               WE_AFI_FR_FILE_NAME)) < 0) {
    res->result = r;
    return op->wid;
  }
  afii_h->fr_file = r;

  we_afif_set_callback (h, afii_h->idx_file, we_afii_file_rename_task);
  we_afif_set_callback (h, afii_h->fr_file, we_afii_file_rename_task);
  we_afii_file_rename_task (h, op->wid);

  return op->wid;
}


/**********************************************************************
 * Make Directory
 **********************************************************************/
/* Create a new directory.
 * 
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * dir_name   Null terminated string with the name of the directory to create.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_mkdir (we_pck_handle_t *handle, const char *dir_name)
{
  we_afi_handle_t *h  = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = get_new_op (h, WE_AFI_MKDIR);
  we_afi_dir_t    *dir;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  dir = WE_MEM_ALLOCTYPE (h->modid, we_afi_dir_t);
  dir->dir = we_cmmn_strdup (h->modid, dir_name);
  op->op_data = (void *)dir;

  return op->wid;
}


static int
make_dir (char *p)
{
  char *stop;
  int   res = 0;
  int   len = strlen (p);

  if (p[len - 1] == '/') {
    p[len - 1] = '\0';
  }

  stop = p;

  do {
    stop  = strchr (stop + 1, '/');

    if (stop != NULL) {
      *stop = '\0';
    }

    res = WE_FILE_MKDIR (p);
    if (!((res == WE_FILE_ERROR_EXIST) || (res == WE_FILE_OK))) {
      return res;
    }

    if (stop != NULL) {
      *stop = '/';
    }

  } while (stop != NULL);

  return res;
}


/*
 * Get the result from the make directory operation.
 */
int
we_afi_get_result_mkdir (we_afi_handle_t *h,
                          WE_INT32         wid,
                          WE_INT32        *result)
{
  we_afi_op_t  *op = find_op_by_id (h, wid);
  we_afi_dir_t *mkdir;
#ifdef WE_CFG_AFI_REDIRECT_INDEX
  char          *index_dir;
#endif

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  mkdir = (we_afi_dir_t *)(op->op_data);
  *result = make_dir (mkdir->dir); /* synchronous */

#ifdef WE_CFG_AFI_REDIRECT_INDEX
  /* Check result, if error bail out */
  if (*result != WE_FILE_OK && *result != WE_FILE_ERROR_EXIST) {
    return WE_PACKAGE_ERROR;  
  }

  /* Check if index files are to be stored in another folder */
  index_dir = we_afir_idx_redirect_path(h->modid, (const char*) mkdir->dir);

  WE_MEM_FREE (h->modid, index_dir);
#endif
  WE_MEM_FREE (h->modid, mkdir->dir);
  WE_MEM_FREE (h->modid, mkdir);

  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


static void
clean_dir_list (const we_afi_handle_t *h, we_afi_dir_list_t *list)
{
  while (list != NULL) {
    we_afi_dir_list_t *tmp = list->next;

    WE_MEM_FREE (h->modid, list->path);
    WE_MEM_FREE (h->modid, list);
    list = tmp;
  }
}


/**********************************************************************
 * Get Size of Directory
 **********************************************************************/
/* This function is used to determine the total size of the directory
 * including subdirectories.
 * 
 * handle      The handle to the struct that holds the file package instance
 *             data.
 * dir_name    Null terminated string describing path name of the directory.
 *
 * Returns:    The size of the directory or NULL if dir_name is invalid.
 *             NOTE! This operation is synchronous.
 */
long
we_afi_get_dir_size (const we_pck_handle_t *handle, const char *dir_name)
{
  we_afi_handle_t   *h = (we_afi_handle_t *)handle;
  we_afi_dir_list_t *list = WE_MEM_ALLOCTYPE (h->modid, we_afi_dir_list_t);
  we_afi_dir_list_t *last_item = list;
  char                name[WE_AFI_MAX_READDIR_SIZE];
  int                 type;
  long                size;
  long                total_size = 0;

  list->next = NULL;
  list->num_entries = WE_FILE_GETSIZE_DIR (dir_name);
  if (IS_DIR (dir_name) == FALSE) {
    list->path = we_cmmn_strcat (h->modid, dir_name, "/");
  }
  else {
    list->path = we_cmmn_strdup (h->modid, dir_name);
  }
  list->pos = 0;

  if (list->num_entries == TPI_FILE_ERROR_PATH) {
    if (list->num_entries < 0) {
      total_size = list->num_entries;
    }
    clean_dir_list (h, list);

    return total_size;
  }

  while ((list != NULL) && (list->pos < list->num_entries)) {
    if ((WE_FILE_READ_DIR (list->path, list->pos, name,
                            WE_AFI_MAX_READDIR_SIZE,
                            &type, &size)) != TPI_FILE_OK) {
      clean_dir_list (h, list);

      return total_size;
    }

    if (type == TPI_FILE_DIRTYPE) {
      we_afi_dir_list_t *item;
      char               *sub_dir;
      int                 num_entries;

      sub_dir = we_cmmn_str3cat (h->modid, list->path, name, "/");
      num_entries  = WE_FILE_GETSIZE_DIR (sub_dir);

      if (num_entries > 0) {
        item = WE_MEM_ALLOCTYPE (h->modid, we_afi_dir_list_t);
        item->path = sub_dir;
        item->num_entries = num_entries;
        item->pos = 0;
        item->next = NULL;
        last_item->next = item; /* insert last */
        last_item = item;
      }
      else {
        WE_MEM_FREE (h->modid, sub_dir);
      }
    }
    list->pos++;
    total_size += size;

    if (list->pos == list->num_entries) {
      we_afi_dir_list_t *tmp = list->next;

      WE_MEM_FREE (h->modid, list->path);
      WE_MEM_FREE (h->modid, list);
      list = tmp;
    }
  }
  clean_dir_list (h, list);

  return total_size;
}


/**********************************************************************
 * Exist
 **********************************************************************/
/* Check if a file exist with attributes.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the name of the file
 *            that should be checked for existence.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_exist (we_pck_handle_t *handle, const char *file_name)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = get_new_op (h, WE_AFI_EXIST);
  we_afi_open_t   *open;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  open = WE_MEM_ALLOCTYPE (h->modid, we_afi_open_t);
  we_afi_split_path (h->modid, file_name, &(open->dir), &(open->src));
  open->mode = WE_AFI_RDONLY;
  open->attr = NULL;
  op->op_data = (void *)open;
  we_afii_file_exist (handle, op->wid);

  return op->wid;
}


/*
 * Gets the result from the we_afi_exist operation.
 * Arguments:
 *
 * handle   The handle to the struct that holds the file package
 *          instance data.
 * wid       The operation wid
 * result   result structure to be initialized by this function.
 *          Possible result codes:
 *            WE_AFI_OK
 *            WE_AFI_ERROR_DELAYED
 *            WE_AFI_ERROR_PATH
 *
 * Returns: TRUE if success, otherwise FALSE. Note that this value msan
 *          that this function call was successful.
 */
static int
we_afi_get_result_exist (we_afi_handle_t *h,
                          WE_INT32         wid,
                          WE_INT32        *result)
{
  we_afi_op_t   *op = find_op_by_id (h, wid);
  we_afi_open_t *open;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  we_afii_get_result_generic (h, wid, result);
  if (*result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }
  open = (we_afi_open_t *)op->op_data;
  we_afi_free_open (h->modid, open);
  free_op (h, op);

  return WE_PACKAGE_COMPLETED;
}


static void
save_cmmn_cleanup (we_afi_handle_t *h,
                   we_afi_op_t     *op,
                   we_afi_save_t   *save)
{
  if (save->save_type == WE_PCK_DATA_TYPE_PIPE) {
    WE_PIPE_CLOSE (op->pipe);
    WE_PIPE_DELETE (save->src_name);
  }

  if ((save->save_type == WE_PCK_DATA_TYPE_PIPE) ||
      (save->save_type == WE_PCK_DATA_TYPE_FILE)) {
    WE_MEM_FREE (h->modid, save->read_data);
  }
  save->read_data = NULL; /* if called twice */
  we_afif_file_close (h, save->file_handle);
  WE_FILE_REMOVE (save->tmp_name);
  WE_MEM_FREE (h->modid, save->tmp_name);
}

/* Generate a file name that is unique in the specified directory
 * as indicated by the dest_name.
 * SIDE EFFECT: If no unique name is possible to find using the
 * algorithm, the destination name will be used instead.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
static char *
get_tmp_filename (WE_UINT8 modid, const char *dest_name)
{
  int nbr = 0;

  while (++nbr <= WE_AFI_SAVE_TMP_FILE_TRIES) {
    char  tmp_mark[5];
    char *s;

    sprintf (tmp_mark, "~%d", nbr);
    s = we_cmmn_strcat (modid, dest_name, tmp_mark);

    /* check that name does not already exist */
    if ((WE_FILE_GETSIZE (s) != TPI_FILE_ERROR_PATH) ||
        (WE_FILE_GETSIZE_DIR (s) != TPI_FILE_ERROR_PATH)) {
      WE_MEM_FREE (modid, s);
    }
    else {
      return s;
    }
  }

  return we_cmmn_strdup (modid, dest_name);
}


static int
we_afi_save_task (we_afi_handle_t *h, we_afi_op_t *op)
{
  int                    res;
  we_afi_result_open_t  open;
  we_afi_result_read_t  read;
  WE_INT32              write;
  WE_INT32              close;
  WE_INT32              attr;
  we_afi_op_t          *job_op;
  we_afi_save_t        *save = (we_afi_save_t *)op->op_data;

  switch (op->state) {
  case WE_AFI_STATE_NULL:
    /* If not in overwrite mode; check that destination file does not exist */
    if ((save->flags & WE_AFI_OVERWRITE) == FALSE) {
      if ((WE_FILE_GETSIZE (save->dst_name) != TPI_FILE_ERROR_PATH)) {
        /* file exist */
        save->status = WE_AFI_ERROR_EXIST;

        return WE_PACKAGE_ERROR;
      }
    }
    if (save->save_type == WE_PCK_DATA_TYPE_PIPE) {
      /* Reading from source : PIPE */
      op->pipe = WE_PIPE_OPEN (h->modid, save->src_name);
      if (op->pipe < 0) {
        save->status = WE_AFI_ERROR_INVALID;

        return WE_PACKAGE_ERROR;
      }
      /* read data from pipe into buffer */
      save->read_data = WE_MEM_ALLOC (h->modid, WE_AFI_MAX_FILE_READ_SIZE);

  case WE_AFI_SAVE_PIPE_READ:

/*read_from_pipe:*/
      op->state = WE_AFI_SAVE_PIPE_READ;
      save->data_len = WE_PIPE_READ (op->pipe, save->read_data,
                                      WE_AFI_MAX_FILE_READ_SIZE);
      if (save->data_len < 0) {
        if (save->data_len == WE_PIPE_ERROR_DELAYED) {
          res = WE_PIPE_POLL (op->pipe);
          if (res < 0) {
            save->status = WE_AFI_ERROR_INVALID;

            return WE_PACKAGE_ERROR;
          }
          save->status = WE_AFI_ERROR_DELAYED;

          return WE_PACKAGE_BUSY;
        }
        if (save->data_len == WE_PIPE_ERROR_BAD_HANDLE) {
          save->status = WE_AFI_ERROR_INVALID;

          return WE_PACKAGE_ERROR;
        }
        else {
          /* write end of pipe has been closed */
          save->read_complete = TRUE;
          res = WE_PIPE_CLOSE (op->pipe);
          if (res < 0) {
            save->status = WE_AFI_ERROR_INVALID;
           }
          res = WE_PIPE_DELETE (save->src_name);
          if (res < 0) {
            save->status = WE_AFI_ERROR_INVALID;
          }
          save->data_len = 0;
        }
      }
      goto write_dst;
    }
    else if (save->save_type == WE_PCK_DATA_TYPE_FILE) {
      /* Reading from source : FILE */
      save->save_id = we_afi_open (h, save->src_name, WE_AFI_RDONLY);
      if (save->save_id < 0) {
        save->status = WE_AFI_ERROR_INVALID;
        save_cmmn_cleanup (h, op, save);

        return WE_PACKAGE_COMPLETED;
      }
      job_op = find_op_by_id (h, save->save_id);
      if (job_op != NULL) {
        job_op->parent_id = op->wid;
      }

  case WE_AFI_SAVE_FILE_OPEN:
      op->state = WE_AFI_SAVE_FILE_OPEN;
      res = we_afi_get_result_open (h, save->save_id, &open);
      save->src_fh = open.file_handle;

      switch (res) {
      case WE_PACKAGE_BUSY:
        save->status = WE_AFI_ERROR_DELAYED;
        return WE_PACKAGE_BUSY;

      case WE_PACKAGE_ERROR:
        save->status = open.result;
        return WE_PACKAGE_ERROR;
      }

      /* read the source */
      save->read_data = WE_MEM_ALLOC (h->modid, WE_AFI_MAX_FILE_READ_SIZE);

read_from_file:
      save->save_id = we_afi_read (h, save->src_fh, save->read_data,
                                    WE_AFI_MAX_FILE_READ_SIZE);

      job_op = find_op_by_id (h, save->save_id);
      if (job_op != NULL) {
        job_op->parent_id = op->wid;
      }

  case WE_AFI_SAVE_FILE_SRC_READ:
      op->state = WE_AFI_SAVE_FILE_SRC_READ;
      res = we_afi_get_result_read (h, save->save_id, &read);

      if (res == WE_PACKAGE_BUSY) {
        save->status = WE_AFI_ERROR_DELAYED;
        return WE_PACKAGE_BUSY;
      }

      if (res != WE_PACKAGE_COMPLETED) {
        save->status = read.result;
        return res;
      }

      save->read_data = read.data;
      save->data_len = read.data_len;
      save->read_complete = (read.result == WE_AFI_ERROR_EOF);

      if ((save->read_complete == FALSE) && (read.result < 0)) {
        if (read.result == WE_AFI_ERROR_DELAYED) {
          save->status = WE_AFI_ERROR_DELAYED;
          return WE_PACKAGE_BUSY;
        }
        save->status = WE_AFI_ERROR_INVALID;
        save_cmmn_cleanup (h, op, save);

        return WE_PACKAGE_COMPLETED;
      }
      if (save->read_complete) {
        /* close the source file */
        int wid = we_afi_close (h, save->src_fh); /*synchronous */

        we_afi_get_result_close (h, wid, &close);
      }
    }

    /* fall through */
    /* Reading from source : BUFFER, which is already in memory */

write_dst:
    if (save->file_handle < 0) {
      /* generate a temporary name using the destination name */
      save->tmp_name = get_tmp_filename (h->modid, save->dst_name);
      save->save_id = we_afi_open (h, save->tmp_name, WE_AFI_WRONLY);
      job_op = find_op_by_id (h, save->save_id);
      if (job_op != NULL) {
        job_op->parent_id = op->wid;
      }

  case WE_AFI_SAVE_OPEN:
      op->state = WE_AFI_SAVE_OPEN;
      res = we_afi_get_result_open (h, save->save_id, &open);
      save->file_handle = open.file_handle;
      if (res == WE_PACKAGE_BUSY) {
        save->status = WE_AFI_ERROR_DELAYED;
        return WE_PACKAGE_BUSY;
      }
      else if ((res == WE_PACKAGE_ERROR) || (open.result < 0)) {
        save->status = open.result; /* let status info propagate to outermost op */
        save_cmmn_cleanup (h, op, save);

        return WE_PACKAGE_COMPLETED;
      }
    }

    if (save->data_len > 0) {
      save->save_id = we_afi_write (h, save->file_handle,
                                     save->read_data, save->data_len);
      job_op = find_op_by_id (h, save->save_id);
      if (job_op != NULL) {
        job_op->parent_id = op->wid;
      }

    case WE_AFI_SAVE_DST_WRITE:
      op->state = WE_AFI_SAVE_DST_WRITE;
      res = we_afi_get_result_write (h, save->save_id, &write);
      if (res == WE_PACKAGE_BUSY) {
        save->status = WE_AFI_ERROR_DELAYED;

        return WE_PACKAGE_BUSY;
      }
      else if ((res == WE_PACKAGE_ERROR) || (write < 0)) {
        save->status = write;
        save_cmmn_cleanup (h, op, save);

        return WE_PACKAGE_COMPLETED;
      }

      save->bytes_saved += save->data_len;
      if ((save->save_type == WE_PCK_DATA_TYPE_FILE) ||
          (save->save_type == WE_PCK_DATA_TYPE_PIPE)) {
        if (save->read_complete == FALSE) {
          /*Interrupt save and send signal:*/
          if (save->save_type == WE_PCK_DATA_TYPE_FILE) {
            goto read_from_file;
          }
          else {
            op->state = WE_AFI_SAVE_PIPE_READ;
            we_afi_send_signal_notity (h->modid, op->pipe,
                                        WE_PCK_DATA_TYPE_PIPE,
                                        TPI_PIPE_EVENT_READ);
          }
          save->status = WE_AFI_ERROR_DELAYED;

          return WE_PACKAGE_BUSY;
        }
      }
    }

    we_afi_close (h, save->file_handle);  /* synchronous*/
    we_afi_get_result_close (h, save->save_id, &close);

    if ((WE_FILE_GETSIZE (save->dst_name) != TPI_FILE_ERROR_PATH) &&
        (strcmp (save->dst_name, save->tmp_name) != 0)) {
      /* If file exist we are in overwrite mode and must remove the file.
       * The exception is if a temporary filename was not generated, in
       * which case the destination filename is used and must not be removed.
       */
      save->save_id = we_afi_remove (h, save->dst_name);
      job_op = find_op_by_id (h, save->save_id);
      if (job_op != NULL) {
        job_op->parent_id = op->wid;
      }

    case WE_AFI_STATE_REMOVE_RESULT:
      {
        WE_INT32 remove;

        op->state = WE_AFI_STATE_REMOVE_RESULT;
        res = we_afi_get_result_remove (h, save->save_id, &remove);
        if (res == WE_PACKAGE_BUSY) {
          save->status = WE_AFI_ERROR_DELAYED;

          return WE_PACKAGE_BUSY;
        }
        else if ((res == WE_PACKAGE_ERROR) || (remove < 0)) {
          save->status = remove;
          save_cmmn_cleanup (h, op, save);

          return WE_PACKAGE_COMPLETED;
        }
      }
    }

    save->save_id = we_afi_rename (h, save->tmp_name, save->dst_name);
    job_op = find_op_by_id (h, save->save_id);
    if (job_op != NULL) {
      job_op->parent_id = op->wid;
    }

  case WE_AFI_STATE_RENAME_RESULT:
    {
      WE_INT32 rename;

      op->state = WE_AFI_STATE_RENAME_RESULT;
      res = we_afi_get_result_rename (h, save->save_id, &rename);
      if (res == WE_PACKAGE_BUSY) {
        save->status = WE_AFI_ERROR_DELAYED;

        return WE_PACKAGE_BUSY;
      }
      else if ((res == WE_PACKAGE_ERROR) || (rename < 0)) {
        save->status = rename;
        save_cmmn_cleanup (h, op, save);

        return WE_PACKAGE_COMPLETED;
      }
    }

    save->save_id = we_afi_set_attributes (h, save->dst_name, save->attr);
    job_op = find_op_by_id (h, save->save_id);
    if (job_op != NULL) {
      job_op->parent_id = op->wid;
    }

  case WE_AFI_SAVE_SET_ATTR:
    op->state = WE_AFI_SAVE_SET_ATTR;
    res = we_afi_get_result_set_attributes (h, save->save_id, &attr);
    if (res == WE_PACKAGE_BUSY) {
      save->status = WE_AFI_ERROR_DELAYED;
      return WE_PACKAGE_BUSY;
    }
    else if (res == WE_PACKAGE_ERROR || attr < 0) {
      save->status = attr;
      save_cmmn_cleanup (h, op, save);

      return WE_PACKAGE_COMPLETED;
    }

  case WE_AFI_STATE_COMPLETED:
    save->file_handle = NO_FILE;
    op->state = WE_AFI_STATE_COMPLETED;
    save->status = WE_AFI_OK;
    break;

  case WE_AFI_STATE_DELETE:
    return WE_PACKAGE_BUSY;
  }

  save_cmmn_cleanup (h, op, save);

  return WE_PACKAGE_COMPLETED;
}


/**********************************************************************
 * Save
 **********************************************************************/
/* Save a pipe, file, or buffer in a file with a specified file name.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of the file
 *            to save.
 * data_type  Tells if the data to save comes from a pipe, file or buffer.
 *            The value must be one of the following:
 *                 WE_PCK_DATA_TYPE_FILE,
 *                 WE_PCK_DATA_TYPE_PIPE,
 *                 WE_PCK_DATA_TYPE_BUFFER
 * data_len   The size of the data.
 *            Used only when the data source is a buffer.
 * data       The data that shall be written to file.
 *            Used only when the data source is a buffer.
 * path_name  The name of the pipe or file. This parameter is only used when
 *            data comes from a pipe or file.
 * file_attr  File attributes.
 * flags      Save flags. Possible values are:
 *              WE_AFI_OVERWRITE
 *
 * Returns:   An wid to the save operation or WE_PACKAGE_ERROR
 */
WE_INT32
we_afi_save_ext (we_pck_handle_t          *handle,
                  const char                *file_name,
                  int                        data_type,
                  long                       data_len,
                  const char                *data,
                  const char                *path_name,
                  const we_pck_attr_list_t *file_attr,
                  WE_UINT32                 flags)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = get_new_op (h, WE_AFI_SAVE);
  we_afi_save_t   *save;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  save = WE_MEM_ALLOCTYPE (h->modid, we_afi_save_t);
  save->attr = we_pck_attr_dupl (h->modid, file_attr);
  save->bytes_saved = 0;
  save->data_len = data_len;
  save->dst_name = we_cmmn_strdup (h->modid, file_name);
  save->flags = flags;
  save->file_handle = NO_FILE;
  save->read_complete = FALSE;
  save->read_data = (void *)data; /* We must not alter data */
  save->save_id = -1;
  save->save_type = data_type;
  save->src_fh = NO_FILE;
  save->src_name  = we_cmmn_strdup (h->modid, path_name); /* pipe or file */
  save->status = WE_AFI_ERROR_DELAYED;
  save->tmp_name = NULL;

  op->op_data = (void *)save;

  if ((data_type == WE_PCK_DATA_TYPE_FILE) &&
      (strcmp (file_name, path_name) == 0)) {
    /* src, dst matches, this is a no op. */
    save->status = WE_AFI_OK;
  }
  else {
    we_afi_save_task (h, op);
  }

  return op->wid;
}

/* supplied for compatibility reasons */
WE_INT32
we_afi_save (we_pck_handle_t          *handle,
              const char                *file_name,
              int                        data_type,
              long                       data_len,
              const char                *data,
              const char                *path_name,
              const we_pck_attr_list_t *file_attr)
{
  return we_afi_save_ext (handle, file_name, data_type, data_len, data,
                           path_name, file_attr, 0);
}


/* Get the result from a save operation
 * 
 * Arguments:
 * handle   The handle to the struct that holds all file package information.
 * wid       The wid to the save instance
 * result   The function initializes this parameter with current status 
 *          code of the operation:
 *                WE_AFI_OK
 *                WE_AFI_ERROR_ACCESS
 *                WE_AFI_ERROR_DELAYED
 *                WE_AFI_ERROR_PATH
 *                WE_AFI_ERROR_INVALID
 *                WE_AFI_ERROR_SIZE
 *                WE_AFI_ERROR_FULL
 *                WE_AFI_ERROR_EXIST
 *
 * Returns: TRUE if success, otherwise FALSE. Note that this value msan that 
 *          this function call was successful, but it does not indicate that
 *          the file is saved.
 */
static int
we_afi_get_result_save (we_afi_handle_t               *h,
                         WE_INT32                       wid,
                         WE_INT32                      *result,
                         we_afi_additional_data_save_t *additional_data)
{
  we_afi_op_t   *op = find_op_by_id (h, wid);
  we_afi_save_t *save;
  int             res;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  save = (we_afi_save_t *)(op->op_data);
  *result = save->status;
  res = WE_PACKAGE_COMPLETED;

  switch (*result) {
  case WE_AFI_ERROR_ACCESS:
  case WE_AFI_ERROR_PATH:
  case WE_AFI_ERROR_INVALID:
  case WE_AFI_ERROR_SIZE:
  case WE_AFI_ERROR_FULL:
  case WE_AFI_ERROR_EXIST:
    res = WE_PACKAGE_ERROR;
    break;

  case WE_AFI_OK:
    res = WE_PACKAGE_COMPLETED;
    break;

  case WE_AFI_ERROR_DELAYED:
    res = WE_PACKAGE_BUSY;
    break;

  }
  additional_data->bytes_saved = save->bytes_saved;
  if (res == WE_PACKAGE_BUSY) {
    return WE_PACKAGE_BUSY;
  }

  we_afi_free_save (h->modid, save);
  free_op (h, op);

  return res;
}

/**********************************************************************
 * Delete operation. Must only be used on we_afi_save from a pipe.
 **********************************************************************/
static int
we_afi_delete_save (we_afi_handle_t *h, we_afi_op_t *op)
{
  we_afi_delete_t *del = (we_afi_delete_t *)(op->op_data);
  we_afi_op_t     *save_op = find_op_by_id (h, del->delete_id);
  int               res;
  we_pck_result_t  result;
  we_afi_save_t   *target_save;

  if (save_op == NULL) {
    return WE_PACKAGE_ERROR;
  }
  if (save_op->state != WE_AFI_STATE_DELETE) {
    /*Initiate the shut down process.*/
    op->state = save_op->state;
    save_op->parent_id = op->wid;
    save_op->state = WE_AFI_STATE_DELETE;
  }

  target_save = (we_afi_save_t *)save_op->op_data;
  del->status = WE_AFI_OK; /* restore to defined value */ 

  if (op->state == WE_AFI_SAVE_DST_WRITE) {
    res = we_afi_get_result_write (h, target_save->save_id, &(del->status));
    if (res == WE_PACKAGE_BUSY) {
      return WE_PACKAGE_BUSY;
    }
  }
  else if (op->state == WE_AFI_SAVE_OPEN) {
    res = we_afi_get_result (h, del->wid, &result);
    if (res == WE_PACKAGE_BUSY) {
      return WE_PACKAGE_BUSY;
    }
    else if (res == WE_PACKAGE_ERROR) {
      we_afi_result_open_t *open = (we_afi_result_open_t *)result._u.data;

      del->status = open->result;
    }
  }

  save_cmmn_cleanup (h, save_op, target_save); /* clean up the save op */
  we_afi_free_save (h->modid, target_save);
  free_op (h, save_op);

  if (del->status == WE_AFI_OK) {
    return WE_PACKAGE_COMPLETED;
  }

  return WE_PACKAGE_ERROR;
}


WE_INT32
we_afi_delete (we_pck_handle_t *handle, WE_INT32 wid)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *delete_op = find_op_by_id (h, wid);
  we_afi_save_t   *save;
  we_afi_op_t     *op;
  we_afi_delete_t *del;

  if ((delete_op == NULL) ||
      (delete_op->type != WE_AFI_SAVE)) {
    return WE_PACKAGE_ERROR;
  }
  save = (we_afi_save_t *)delete_op->op_data;
  if (save->save_type != WE_PCK_DATA_TYPE_PIPE) {
    return WE_PACKAGE_ERROR;
  }

  op = get_new_op (h, WE_AFI_DELETE);
  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  del = WE_MEM_ALLOCTYPE (h->modid, we_afi_delete_t);
  del->wid = -1;
  del->delete_id = wid;
  op->op_data = (void *)del;

  we_afi_delete_save (h, op);

  return op->wid;
}


static int
we_afi_get_result_delete (we_afi_handle_t *h,
                           WE_INT32         wid,
                           WE_INT32        *result)
{
  we_afi_op_t     *op = find_op_by_id (h, wid);
  we_afi_delete_t *del;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  del = (we_afi_delete_t *)(op->op_data);
  *result = del->status;
  if (*result == WE_AFI_ERROR_DELAYED) {
    return WE_PACKAGE_BUSY;
  }
  WE_MEM_FREE (h->modid, del);
  free_op (h, op);

  if (*result == WE_AFI_OK) {
    return WE_PACKAGE_COMPLETED;
  }

  return WE_PACKAGE_ERROR;
}

/**********************************************************************
 * Result functions
 **********************************************************************/

/*
 * Free the storage allocated in the get result function.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * result     Result data to be freed.
 *
 * Returns:   TRUE if success, otherwise FALSE.
 */
int
we_afi_result_free (we_pck_handle_t *handle, we_pck_result_t *result)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;

  if (result == NULL) {
    return FALSE;
  }

  switch (result->type) {

  case WE_AFI_OPEN:
  case WE_AFI_READ:
    WE_MEM_FREE (h->modid, result->_u.data);
    break;

  case WE_AFI_SET_ATTR:
  case WE_AFI_MKDIR:
  case WE_AFI_CLOSE:
  case WE_AFI_WRITE:
  case WE_AFI_REMOVE:
  case WE_AFI_RENAME:
  case WE_AFI_DELETE:
    break; /* Nothing allocated */

  case WE_AFI_SAVE:
    WE_MEM_FREE (h->modid, result->additional_data);
    break;

  case WE_AFI_GET_ATTR:
    {
      we_afi_result_get_attributes_t *attr;
      
      attr = (we_afi_result_get_attributes_t *)(result->_u.data);
      we_pck_attr_free (h->modid, attr->attr_list);
      WE_MEM_FREE (h->modid, attr);
      break;
    }

  case WE_AFI_GET_DIR:
    {
      we_afi_result_dir_entries_t *dir;

      dir = (we_afi_result_dir_entries_t *)(result->_u.data);
      we_afii_free_result_dir_entries (h, dir);
      break;
    }

  default:
    return FALSE;
  }

  return TRUE;
}


int
we_afi_get_result (we_pck_handle_t *handle,
                    WE_INT32         wid,
                    we_pck_result_t *result)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;
  we_afi_op_t     *op = find_op_by_id (h, wid);
  int               r;

  if (op == NULL) {
    return WE_PACKAGE_ERROR;
  }

  result->type = op->type;

  switch (op->type) {
  case WE_AFI_OPEN:
    result->_u.data = WE_MEM_ALLOCTYPE (h->modid, we_afi_result_open_t);
    r = we_afi_get_result_open (h, wid,
                                 (we_afi_result_open_t *)result->_u.data);
    break;

  case WE_AFI_CLOSE:
    r = we_afi_get_result_close (h, wid, &result->_u.i_val);
    break;

  case WE_AFI_READ:
    result->_u.data = WE_MEM_ALLOCTYPE (h->modid, we_afi_result_read_t);
    r = we_afi_get_result_read (h, wid, result->_u.data);
    break;

  case WE_AFI_WRITE:
    r = we_afi_get_result_write (h, wid, &result->_u.i_val);
    break;

  case WE_AFI_SAVE:
    result->additional_data = WE_MEM_ALLOCTYPE (h->modid,
                                              we_afi_additional_data_save_t);
    r = we_afi_get_result_save (h, wid, &result->_u.i_val,
                                 result->additional_data);
    break;

  case WE_AFI_REMOVE:
    r = we_afi_get_result_remove (h, wid, &result->_u.i_val);
    break;

  case WE_AFI_RENAME:
    r = we_afi_get_result_rename (h, wid, &result->_u.i_val);
    break;

  case WE_AFI_SET_ATTR:
    r = we_afi_get_result_set_attributes (h, wid, &result->_u.i_val);
    break;

  case WE_AFI_GET_ATTR:
    r = we_afi_get_result_get_attr (h, wid,
                      ((we_afi_result_get_attributes_t **)&result->_u.data));
    break;

  case WE_AFI_GET_DIR:
    r = we_afi_get_result_get_dir_entries (h, wid,
                         ((we_afi_result_dir_entries_t **)&result->_u.data));
    break;

  case WE_AFI_MKDIR:
    r = we_afi_get_result_mkdir (h, wid, &result->_u.i_val);
    break;

  case WE_AFI_DELETE:
    r = we_afi_get_result_delete (h, wid, &result->_u.i_val);
    break;

  case WE_AFI_EXIST:
    r = we_afi_get_result_exist (h, wid, &result->_u.i_val);
    break;

  default: 
    r = WE_PACKAGE_ERROR;
  }

  return r;
}


/* file notification */
static int
handle_file_notification (we_afi_handle_t *h, int handle, WE_INT32 *wid)
{
  we_afif_file_t   *file;
  int                i;
  int                ret;
  we_afii_handle_t *afii_h;

  for (i = 0; i < WE_AFIF_MAX_FILES; i++) {
    if (h->files[i].handle == handle) {
      file = &(h->files[i]);

      if ((afii_h = we_afii_find_handle_by_id (h, file->wid)) == NULL) {
        return WE_PACKAGE_SIGNAL_NOT_HANDLED;
      }

      ret = h->files[i].cb (h, file->wid);
      *wid = h->files[i].wid;

      if (ret < 0) {
        if (ret != WE_AFI_ERROR_DELAYED) {
          we_afi_generic_result_t *gen_res;

          gen_res = ((we_afi_generic_result_t *)afii_h->result);
          gen_res->result = ret;
          we_afif_file_close (h, afii_h->idx_file);
          we_afif_file_close (h, afii_h->fr_file);
          afii_h->idx_file = NO_FILE;
          afii_h->fr_file = NO_FILE;

          return WE_PACKAGE_OPERATION_COMPLETE;
        }

        return WE_PACKAGE_SIGNAL_HANDLED;
      }

      return WE_PACKAGE_OPERATION_COMPLETE;
    }
  }

  return WE_PACKAGE_SIGNAL_NOT_HANDLED;
}


/*
 * Runs the AFI package. When the module receives a signal defined by
 * WE, the module must call this function since the AFI package might
 * be the receiver.
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
we_afi_handle_signal (we_pck_handle_t *handle,
                       WE_UINT16        signal,
                       void             *p,
                       WE_INT32        *wid)
{
  we_afi_handle_t *h = (we_afi_handle_t *)handle;

  if (h == NULL) {
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;
  }

  switch (signal) {
  case WE_SIG_FILE_NOTIFY:
    {
      we_file_notify_t *file;
      int                res;
      we_afi_op_t      *op;
      we_afi_op_t      *parent_op;

      WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, h->modid,
                   "AFI: Received WE_SIG_FILE_NOTIFY\n"));
      file = (we_file_notify_t *)p;
      res = handle_file_notification (h, file->fileHandle, wid);
      if (res == WE_PACKAGE_SIGNAL_NOT_HANDLED) {
        return res;
      }

      if ((op = find_op_by_id (h, *wid)) == NULL) {
        return WE_PACKAGE_SIGNAL_NOT_HANDLED;
      }

      if (op->parent_id == NO_PARENT_ID) {
        return res;
      }

      /* Save or delete operation types */
      if ((parent_op = find_op_by_id (h, op->parent_id)) == NULL) {
        return WE_PACKAGE_SIGNAL_NOT_HANDLED;
      }

      if (parent_op->state == WE_AFI_STATE_DELETE) {
        if ((parent_op = find_op_by_id (h, parent_op->parent_id)) == NULL) {
          return WE_PACKAGE_SIGNAL_NOT_HANDLED;
        }
      }
      *wid = parent_op->wid;

      if (res == WE_PACKAGE_SIGNAL_HANDLED) {
        return res;
      }

      if (parent_op->type == WE_AFI_SAVE) {
        if (we_afi_save_task (h, parent_op) != WE_PACKAGE_BUSY) {
          return WE_PACKAGE_OPERATION_COMPLETE;
        }

        return WE_PACKAGE_SIGNAL_HANDLED;
      }
      else if (parent_op->type == WE_AFI_DELETE) {
        if (we_afi_delete_save (h, parent_op) != WE_PACKAGE_BUSY) {
          return WE_PACKAGE_OPERATION_COMPLETE;
        }

        return WE_PACKAGE_SIGNAL_HANDLED;
      }

      return res;
    }

  case WE_SIG_PIPE_NOTIFY:
    {
      we_pipe_notify_t *pipe;
      we_afi_op_t      *op;

      WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, h->modid,
                   "AFI: Received WE_SIG_PIPE_NOTIFY\n"));
      pipe = (we_pipe_notify_t *)p;
      op = find_op_by_pipe (h, pipe->handle);
      if (op == NULL) {
        return WE_PACKAGE_SIGNAL_NOT_HANDLED;
      }

      if (op->state == WE_AFI_STATE_DELETE) {
        *wid = op->parent_id;

        return WE_PACKAGE_SIGNAL_HANDLED;
      }

      *wid = op->wid;
      if (we_afi_save_task (h, op) != WE_PACKAGE_BUSY) {
        return WE_PACKAGE_OPERATION_COMPLETE;
      }

      return WE_PACKAGE_SIGNAL_HANDLED;
    }
  }

  return WE_PACKAGE_SIGNAL_NOT_HANDLED;
}
/****************************************************************************
 *  AFI END
 ****************************************************************************/

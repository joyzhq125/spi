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

#include "We_Lib.h"
#include "We_Mem.h"
#include "We_Log.h"
#include "We_Wid.h"
#include "We_Pck.h"

/**********************************************************************
 * Defines
 **********************************************************************/
#define WE_PCK_IDX_POSIX_PATH     0
#define WE_PCK_IDX_STR_RC_ID      1
#define WE_PCK_IDX_FLD_ID         2
#define WE_PCK_IDX_MIME           3
#define WE_PCK_IDX_MAX_DEPTH      4


/**********************************************************************
 * Types
 **********************************************************************/

typedef struct 
{
  const char *pathname;
  WE_UINT32  str_id;
  const char *mime;
  int         max_depth;
  int         external;
} we_pck_user_root_folder_t;

typedef struct 
{
  WE_UINT32  str_id;
  const char *pathname;
} we_pck_user_folder_t;


/* Description of mandatory package functions. */

/**********************************************************************
 * Functions to get information from the User Root Folder and 
 * User Folders table.
 **********************************************************************/

#if (WE_NBR_OF_USER_ROOT_FOLDERS < 1)
#error "WE_PCK.C: ERROR - At least one User Root Folder needs to be defined"
#endif 
  
const static we_pck_user_root_folder_t we_pck_urf_table[WE_NBR_OF_USER_ROOT_FOLDERS] = WE_USER_ROOT_FOLDERS;

#ifdef WE_USER_FOLDERS
const static we_pck_user_folder_t we_pck_uf_table[WE_NBR_OF_USER_FOLDERS] = WE_USER_FOLDERS;
#endif

int 
we_pck_get_urf_max_depth (const char *pathname)
{
  int i;

  for (i = 0; i < WE_NBR_OF_USER_ROOT_FOLDERS; i++)
  {
    if (strstr (pathname, we_pck_urf_table[i].pathname) == pathname)
      return we_pck_urf_table[i].max_depth;
  }

  return -1;
}

int 
we_pck_is_urf_path (const char *path)
{
  int i;

  for (i = 0; i < WE_NBR_OF_USER_ROOT_FOLDERS; i++)
  {
    if (strncmp (path,
                 we_pck_urf_table[i].pathname,
                 strlen (we_pck_urf_table[i].pathname)) == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/*
 * Returns the resource wid for "path". The path parameter is defined as follows:
 *
 * If type equals WE_USER_ROOT_FOLDER_TYPE path is the full path to a URF
 * and folder is NULL
 *
 * If type equals WE_USER_FOLDER_TYPE path is the full path to a UF
 */
WE_UINT32
we_pck_get_res_id_from_uf (WE_UINT8 modid, const char *path, const char *folder, int type)
{
  int i;

#ifndef WE_USER_FOLDERS
  WE_UNUSED_PARAMETER (folder);
  WE_UNUSED_PARAMETER (modid);
#endif

  if (type == WE_USER_ROOT_FOLDER_TYPE)
  {
    for (i = 0; i < WE_NBR_OF_USER_ROOT_FOLDERS; i++)
    {
      if (strcmp (path, we_pck_urf_table[i].pathname) == 0)
        return we_pck_urf_table[i].str_id;
    }
  }
  
#ifdef WE_USER_FOLDERS
  if (type == WE_USER_FOLDER_TYPE)
  {
    int   pl,n;
    char *p;
    if(path[strlen(path) - 1] != '/'){
      p = we_cmmn_str3cat(modid, path, "/",folder);   
    }
    else{
      p = we_cmmn_strcat(modid, path, folder);
    }
    pl = strlen (p);
    if(p[pl-1] == '/')
      pl--;
    /*Find user folder part*/
    for (n = 0; n < WE_NBR_OF_USER_FOLDERS; n++)
    {
      if (strncmp (p, we_pck_uf_table[n].pathname, pl) == 0){
        /*Found URF, now find uf part*/
        WE_MEM_FREE (modid, p);
        return we_pck_uf_table[n].str_id;
      }
    }
    WE_MEM_FREE (modid, p);
  }
#endif

return 0;
}

void
we_pck_get_urf_from_mime (WE_UINT8               mod_id,
                           const char             *mime_types, 
                           we_pck_root_folders_t **result)
{
  int                     i;
  we_pck_root_folders_t *r;
  const char * pMimeType;
  WE_BOOL isValidDir;


  r = WE_MEM_ALLOC(mod_id, sizeof (we_pck_root_folders_t) + 
                           (sizeof (we_pck_root_folder_item_t) * 
                            (WE_NBR_OF_USER_ROOT_FOLDERS - 1)));

  r->n_rf = 0;


  for (i = 0; i < WE_NBR_OF_USER_ROOT_FOLDERS; i++)
  {

    pMimeType = mime_types;
    isValidDir = FALSE;
    
    while ( *pMimeType != '\0' ) {

      /* we_pck_mime_type_in_list only uses the first mime type in the list.
         pMimeType sweeps through all types in mime_types, when a mime type
         that match the current urf we save the dir in r->rf
      */
      if ( we_pck_mime_type_in_list(pMimeType, we_pck_urf_table[i].mime) ) {
        isValidDir = TRUE;
        break;
      }

      
      /* mime type list is comma separated, find start of next mime type */
      while ((*pMimeType != ',') && (*pMimeType != '\0')) {
        pMimeType++;
      }
      if (*pMimeType == ','){
        pMimeType++;
      }

    }
    
    if ( isValidDir )
    {
      r->rf[r->n_rf].pathname   = we_pck_urf_table[i].pathname;
      r->rf[r->n_rf].str_id   = we_pck_urf_table[i].str_id;
      r->rf[r->n_rf].external = we_pck_urf_table[i].external;
      r->n_rf++;
    }
  }

  *result = r;
}

void
we_pck_get_all_root_folders (WE_UINT8 mod_id, we_pck_root_folders_t **result)
{
  we_pck_root_folders_t *r;
  int                     i;

  r = WE_MEM_ALLOC (mod_id, 
                     sizeof (we_pck_root_folders_t) + 
                     sizeof (we_pck_root_folder_item_t) * (WE_NBR_OF_USER_ROOT_FOLDERS - 1));
  
  r->n_rf = WE_NBR_OF_USER_ROOT_FOLDERS;
  
  for (i = 0; i < WE_NBR_OF_USER_ROOT_FOLDERS; i++)
  {
    r->rf[i].str_id   = we_pck_urf_table[i].str_id;
    r->rf[i].pathname   = we_pck_urf_table[i].pathname;
    r->rf[i].external = we_pck_urf_table[i].external;
  }
  
  *result = r;
}

void
we_pck_get_all_user_folders (WE_UINT8 mod_id, we_pck_user_folders_t **result)
{
  we_pck_user_folders_t *r = NULL;

#ifdef WE_USER_FOLDERS
  int i;
  r = WE_MEM_ALLOC (mod_id, 
                     sizeof (we_pck_user_folders_t) + 
                     sizeof (we_pck_user_folder_item_t) * (WE_NBR_OF_USER_FOLDERS - 1));
  
  r->n_uf = WE_NBR_OF_USER_FOLDERS;
  
  for (i = 0; i < WE_NBR_OF_USER_FOLDERS; i++)
  {
    r->uf[i].str_id = we_pck_uf_table[i].str_id;
    r->uf[i].pathname = we_pck_uf_table[i].pathname;
  }
#else
  mod_id = mod_id;
#endif

  *result = r;
}

char *
we_pck_get_localized_pathname (WE_UINT8 mod_id, const char *pathname)
{
  WeStringHandle  ms;
  WE_UINT32       str_id;
  int              type;
  const char      *s;
  const char      *e;
  char            *p;
  char            *r = NULL;
  char            *t = NULL;
  int              i;
  int              l = 0;

  type = WE_USER_ROOT_FOLDER_TYPE;

  s = pathname;
  r = we_cmmn_strdup (mod_id, "/");

  if ((s == NULL) || (*s != '/')) {
    goto error;
  }

  for (;;) {
    str_id = 0;
    if (type == WE_USER_FOLDER_TYPE) {
      /* have found URF; check with UF table */
      e = strchr (s, '/');
      if (e != NULL)
        l += (e - s);
      else
        l += strlen(s);

#ifdef WE_USER_FOLDERS

      for (i = 0; i < WE_NBR_OF_USER_FOLDERS; i++)
      {
        if (strncmp (pathname, we_pck_uf_table[i].pathname, l) == 0) {
          str_id = we_pck_uf_table[i].str_id;
          break;
        }
      }
#endif /*WE_USER_FOLDERS*/
      if (str_id == 0) {
        /* This was not a user folder. Consider rest of path as non UF.*/
        t = we_cmmn_strcat (mod_id, r, s);
        WE_MEM_FREE (mod_id, r);
        return t;
      }
      else {
        if (e != NULL) 
          s = e + 1;/*Step past slash*/
        else
          s = e;
      }
    }
    else {

      for (i = 0; i < WE_NBR_OF_USER_ROOT_FOLDERS; i++)
      {
        l = strlen (we_pck_urf_table[i].pathname);
        if (strncmp (s, we_pck_urf_table[i].pathname, l) == 0) {
          str_id = we_pck_urf_table[i].str_id;
          break;
        }
      }
      if (str_id == 0)
        goto error;
      
      s += l;

      if (*s == '/') {
        s++;/*Step past slash*/
      }

      type = WE_USER_FOLDER_TYPE; /* There can only be one root folder */
    }

    ms = WE_WIDGET_STRING_GET_PREDEFINED (str_id);
    p = WE_MEM_ALLOC (mod_id,
      WE_WIDGET_STRING_GET_LENGTH (ms, 1, WeUtf8) + 1);
    WE_WIDGET_STRING_GET_DATA (ms, p, WeUtf8);
    WE_WIDGET_RELEASE (ms);
    
    t = we_cmmn_str3cat(mod_id, r, p, "/");
    WE_MEM_FREE (mod_id, p);
    WE_MEM_FREE (mod_id, r);
    r = t;

    if ((s == NULL) || (*s == '\0'))
      break;
  }

  return r;

error:
  WE_MEM_FREE (mod_id, r);

  return NULL;
}

/* Returns TRUE if path is a user folder name. */
int
we_pck_is_uf (WE_UINT8 mod_id, const char *path, const char *folder)
{
  WE_UINT32 str_id;

  if ((str_id = we_pck_get_res_id_from_uf (mod_id, path, 
                                            folder, WE_USER_FOLDER_TYPE)) == 0)
    return FALSE;

  return TRUE;
}

/* Returns the localized user folder name. The current working directory (cwd)
 * is used to determine if user folder lookup should be applied.
 */
char *
we_pck_get_local_uf (WE_UINT8   mod_id,
                      const char *cwd,
                      const char *folder)
{
  WeStringHandle  ms;
  WE_UINT32       str_id;
  char            *r;
  
  /* folder is a user folder candidate */
  if ((str_id = we_pck_get_res_id_from_uf (mod_id, cwd, 
                                            folder, WE_USER_FOLDER_TYPE)) == 0)
    return NULL;
  
  ms = WE_WIDGET_STRING_GET_PREDEFINED (str_id);
  r = WE_MEM_ALLOC (mod_id,
    WE_WIDGET_STRING_GET_LENGTH (ms, 1, WeUtf8) + 1);
  WE_WIDGET_STRING_GET_DATA (ms, r, WeUtf8);
  WE_WIDGET_RELEASE (ms);
  return r;
  
  return NULL;
}

/*
 * Retrieve parent path
 */
char *
we_pck_get_parent_dir (WE_UINT8 mod_id, const char *path)
{
  char                      *parent_dir;
  we_pck_root_folders_t    *root_folders;
  int                        i;

  we_pck_get_all_root_folders (mod_id,  &root_folders);
  for (i = 0 ; i < root_folders->n_rf ; i++){
    if (strcmp (root_folders->rf[i].pathname, path) == 0){
      WE_MEM_FREE (mod_id, root_folders);
      return we_cmmn_strdup(mod_id, "/");
    }
  }
  parent_dir = we_cmmn_strdup (mod_id, path);
  parent_dir[strlen(parent_dir)-1] = '\0';
  parent_dir[strrchr(parent_dir, '/' ) - parent_dir + 1] = '\0';
  WE_MEM_FREE (mod_id, root_folders);
  return parent_dir;
}

/**********************************************************************
 * Functions to create file attribute lists
 **********************************************************************/

static we_pck_attr_list_t *
we_pck_get_attr_mem (WE_UINT8 modid)
{
  we_pck_attr_list_t *p = WE_MEM_ALLOCTYPE (modid, we_pck_attr_list_t);

  p->_u.i        = 0;
  p->_u.bv.b_len = 0;
  p->attribute   = 0;
  p->type        = 0;
  p->next        = NULL;

  return p;
}

void
we_pck_attr_init (we_pck_attr_list_t **p)
{
  *p = NULL;
}

void
we_pck_attr_free (WE_UINT8 modid, we_pck_attr_list_t *p)
{
  we_pck_attr_list_t *remove;

  if (p == NULL)
    return;

  while (p) {
    switch (p->type) {
    case WE_PCK_ATTR_TYPE_STRING:
      WE_MEM_FREE (modid, p->_u.s);
      break;

    case WE_PCK_ATTR_TYPE_INT:
      break;

    case WE_PCK_ATTR_TYPE_BYTE:
      WE_MEM_FREE (modid, p->_u.bv.b_value);
      break;
    }
    remove = p;
    p = p->next;
    WE_MEM_FREE (modid, remove);
  }
}

int
we_pck_add_attr_string_value (WE_UINT8              modid,
                               we_pck_attr_list_t  **p,
                               WE_UINT32             attribute,
                               const char            *s)
{
  we_pck_attr_list_t *q;

  q = we_pck_get_attr_mem (modid);
  q->_u.s = we_cmmn_strdup (modid, s);
  q->attribute = attribute;
  q->type = WE_PCK_ATTR_TYPE_STRING;
  q->next = *p;
  *p = q;

  return TRUE;
}

int
we_pck_add_attr_int_value (WE_UINT8             modid,
                            we_pck_attr_list_t **p,
                            WE_UINT32            attribute,
                            WE_INT32             i)
{
  we_pck_attr_list_t *q;

  q = we_pck_get_attr_mem (modid);
  q->_u.i = i;
  q->attribute = attribute;
  q->type = WE_PCK_ATTR_TYPE_INT;
  q->next = *p;
  *p = q;

  return TRUE;
}

int
we_pck_add_attr_byte_value (WE_UINT8              modid,
                             we_pck_attr_list_t  **p,
                             WE_UINT32             attribute,
                             const unsigned char   *bv,
                             long int               length)
{
  we_pck_attr_list_t *q;

  q = we_pck_get_attr_mem (modid);
  q->_u.bv.b_len = length;
  q->_u.bv.b_value = WE_MEM_ALLOC (modid, length);
  memcpy (q->_u.bv.b_value, bv, length);
  q->attribute = attribute;
  q->type = WE_PCK_ATTR_TYPE_BYTE;
  q->next = *p;
  *p = q;

  return TRUE;
}

int
we_pck_attr_get_size (const we_pck_attr_list_t *p)
{
  const we_pck_attr_list_t *q;
  int                        size = 0, len;

  if (p == NULL)
    return 0;

  q = p;
  /* calculate size */
  while (q) {
    switch (q->type) {
    case WE_PCK_ATTR_TYPE_STRING:
      len = strlen (q->_u.s) + 1; /* include NULL terminator */
      /* (type + attribute) + strlen + string */
      size += sizeof (WE_UINT8) + sizeof (WE_UINT32) + sizeof (len) + len;
      break;

    case WE_PCK_ATTR_TYPE_INT:
      len = sizeof (q->_u.i);
      size += sizeof (WE_UINT8) + sizeof (WE_UINT32) + sizeof (len) + len;
      break;

    case WE_PCK_ATTR_TYPE_BYTE:
      len = q->_u.bv.b_len;
      size += sizeof (WE_UINT8) + sizeof (WE_UINT32) + sizeof (len) + len;
      break;

    }
    q = q->next;
  }
  return size;
}

void
we_pck_attr_to_string (WE_UINT8                   modid,
                        const we_pck_attr_list_t  *p,
                        long                       *length,
                        unsigned char             **attr_string)
{
  const we_pck_attr_list_t *q;
  unsigned char             *r;
  int                        size;

  if (p == NULL) {
    *length = 0;
    *attr_string = 0;
    return;
  }

  *length = we_pck_attr_get_size (p);
  *attr_string = WE_MEM_ALLOC (modid, *length);
  r = *attr_string;

  /* copy the contents */
  q = p;
  while (q) {
    switch (q->type) {
    case WE_PCK_ATTR_TYPE_STRING:
      size = strlen (q->_u.s) + 1;              /* include NULL terminator */
      *r++ = q->type;                                   /* type: WE_UINT8 */
      memcpy (r, &q->attribute, sizeof (q->attribute)); /* attribute */
      r += sizeof (q->attribute);
      memcpy (r, &size, sizeof (size)); /* length */
      r += sizeof (size);
      memcpy (r, q->_u.s, size);        /* value */
      break;

    case WE_PCK_ATTR_TYPE_INT:
      size = sizeof (q->_u.i);
      *r++ = q->type;                                   /* type */
      memcpy (r, &q->attribute, sizeof (q->attribute)); /* attribute */
      r += sizeof (q->attribute);
      memcpy (r, &size, sizeof (size));                 /* length */
      r += sizeof (size);
      memcpy (r, &(q->_u.i), size);
      break;

    case WE_PCK_ATTR_TYPE_BYTE:
      size = q->_u.bv.b_len;
      *r++ = q->type;                                   /* type */
      memcpy (r, &q->attribute, sizeof (q->attribute)); /* attribute */
      r += sizeof (q->attribute);
      memcpy (r, &size, sizeof (size));                 /* length */
      r += sizeof (size);
      memcpy (r, q->_u.bv.b_value, size);
      break;

    default:
      size = 0;
      break;
    }
    r += size;
    q = q->next;
  }
}

we_pck_attr_list_t *
we_pck_attr_to_list (WE_UINT8      modid,
                      long           length,
                      unsigned char *attr_string)
{
  unsigned char       *p;
  int                  len;
  we_pck_attr_list_t *list = NULL;

  if (attr_string == NULL)
    return NULL;

  p = attr_string;

  /* The first byte of an attribute is the type with a value > 0. */
  while (p && (p - attr_string) < length) {
    WE_UINT8  type = *p++;
    WE_UINT32 attribute;

    memcpy (&attribute, p, sizeof (attribute)); /* attribute */
    p += sizeof (attribute);

    switch (type) {
      case WE_PCK_ATTR_TYPE_STRING:
        {
          char *s;

          memcpy (&len, p, sizeof (len)); /* length */
          p += sizeof (len);
          s = WE_MEM_ALLOC (modid, len);
          memcpy (s, p, len);
          p += len;
          we_pck_add_attr_string_value (modid, &list, attribute, s);
          WE_MEM_FREE (modid, s);
        }
        break;

      case WE_PCK_ATTR_TYPE_INT:
        {
          WE_UINT32 value;

          memcpy (&len, p, sizeof (len)); /* length */
          p += sizeof (len);
          memcpy (&value, p, sizeof (value)); /* value */
          p += sizeof (value);
          we_pck_add_attr_int_value (modid, &list, attribute, value);
        }
        break;

      case WE_PCK_ATTR_TYPE_BYTE:
        {
          unsigned char *bv;

          memcpy (&len, p, sizeof (len));
          p += sizeof (len);
          bv = WE_MEM_ALLOC (modid, len);
          memcpy (bv, p, len);
          p += len;
          we_pck_add_attr_byte_value (modid, &list, attribute, bv, len);
          WE_MEM_FREE (modid, bv);
        }
        break;
      default:
        /* found end of attributes */
        return list;
    }
  }
  return list;
}

we_pck_attr_list_t *
we_pck_attr_dupl (WE_UINT8                   modid,
                   const we_pck_attr_list_t  *p)
{
  long                 length;
  unsigned char       *str;
  we_pck_attr_list_t *p_out;

  we_pck_attr_to_string (modid, p, &length, &str);
  p_out = we_pck_attr_to_list (modid, length, str);
  WE_MEM_FREE (modid, str);

  return p_out;
}


/**********************************************************************
 * Utility functions for mime types
 **********************************************************************/

/* Searches for mime_type in mime_list which may contain wildcards (*).
   The search stops when the stop_str is encoutered in the mime_list.
   The stop_str may be NULL. It must not contain ','.
   The function returns TRUE if a match was found otherwise FALSE.
   If a non-empty stop_str was passed in, the next string if any
   in mime_list that follws the stop_str is returned in *mime_list.
*/
static WE_BOOL
we_pck_mime_type_in_list_int (const char *mime_type, const char **mime_list,
                               const char *stop_str)
{
  WE_BOOL      found = FALSE;
  const char    *m;
  const char    *list;
  unsigned int  stop_str_len = stop_str ? strlen(stop_str) : 0;

  if (mime_type == NULL || *mime_type == '\0' ||
      mime_list == NULL || *mime_list == NULL)
    return FALSE;

  list = *mime_list;
  *mime_list = NULL;

  /* iterate over every string in the mime list */
  while (*list != '\0') {

    if (!found) {
      /* compare mime strings */
      m = mime_type;
      while (*m != '\0') {
        if (*list == '*') {                                       /* wildcard mode */
          /* check criteria for leaving wildcard mode */
          if (we_cmmn_chrlc (*m) == we_cmmn_chrlc (list[1])) {
            list += 2; /* list[1] cannot be '\0', advance to succeeding char */
          }
        }
        else if (we_cmmn_chrlc (*m) == we_cmmn_chrlc (*list)) { /* normal mode */
          list++;
        }
        else { /* no match */
          break;
        }
        m++;
      }

      /* if still in wildcard mode, leave it */
      while (*list == '*') {
        list++;
      }

      found = (*m == '\0') && ((*list == '\0') || (*list == ','));
      if (found && (stop_str_len == 0)) {
        break;
      }
    } /* end if !found */

    /* advance to next mime string in list */
    while ((*list != '\0') && (*list++ != ','))
      ;

    /* break if stop string is found */
    if ((stop_str_len > 0) &&
        (we_cmmn_strncmp_nc (list, stop_str, stop_str_len) == 0)) {
      if (list[stop_str_len] == '\0') {
        /* stop_str was found last in list, no remaining list */
        break;
      }
      else if (list[stop_str_len] == ',') {
        /* stop_str was found, return any string after the ',' ending the stop_str */
        list += stop_str_len + 1;
        if (*list != '\0') {
          *mime_list = list;
        }
        break;
      }
    }
  } /* end while list */

  return found;
}

WE_BOOL
we_pck_mime_type_in_list (const char *mime_type, const char *mime_list)
{
  WE_BOOL     found;
  const char **list_p = &mime_list;

  /* special feature, to be backward compatible */
  if ((mime_type != NULL) && (strcmp (mime_type,"*/*")  == 0)) {
    return TRUE;
  }

  found = we_pck_mime_type_in_list_int(mime_type, list_p, "-");
  if (found && (*list_p != NULL)) {
    /* check if mime_type is excluded, *list_p points to where the exclude list starts */
    found = !we_pck_mime_type_in_list_int(mime_type, list_p, NULL);
  }
  return found;
}

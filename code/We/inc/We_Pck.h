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

#ifndef _we_pck_h
#define _we_pck_h

#ifndef _we_def_h
#include "We_Def.h"
#endif

#ifndef _we_rc_h
#include "We_Rc.h"
#endif

#ifndef _we_int_h
#include "We_Int.h"
#endif


/**********************************************************************
 * Defines
 **********************************************************************/

#define WE_PACKAGE_SIGNAL_HANDLED      1
#define WE_PACKAGE_SIGNAL_NOT_HANDLED  2
#define WE_PACKAGE_OPERATION_COMPLETE  3

/* Package return values */
#define WE_PACKAGE_ERROR              -1 /* An error has occurred */
#define WE_PACKAGE_COMPLETED           0 /* e.g. an operation is complete */
#define WE_PACKAGE_BUSY                1 /* e.g. operation delayed */

#define WE_PCK_DATA_TYPE_FILE          0
#define WE_PCK_DATA_TYPE_PIPE          1
#define WE_PCK_DATA_TYPE_BUFFER        2

#define WE_PCK_ATTR_TYPE_STRING        1
#define WE_PCK_ATTR_TYPE_INT           2
#define WE_PCK_ATTR_TYPE_BYTE          3


/*File Attributes*/
#define WE_PCK_ATTRIBUTE_VENDOR            0x0001
#define WE_PCK_ATTRIBUTE_DESCRIPTION       0x0002
#define WE_PCK_ATTRIBUTE_CREATED           0x0004  /*Read only attribute set automatically by AFI*/
#define WE_PCK_ATTRIBUTE_MIME              0x0008  /*Mimetype*/
#define WE_PCK_ATTRIBUTE_ICON              0x0010  /*Posix Path to icon file*/
#define WE_PCK_ATTRIBUTE_FLAGS             0x0020  /*Any of the flags defined below in file flags*/
#define WE_PCK_ATTRIBUTE_SIZE              0x0040  /*File size not including the size 
                                                      needed to store file attributes*/
#define WE_PCK_ATTRIBUTE_USER              0x0080  /*User supplied attribute which
                                                      is left untouched by the AFI.*/
#define WE_PCK_ATTRIBUTE_MIME_INTERNAL     0x0100  /* Internal mime type. If the MIME attribute 
                                                       is e.g. application/vnd.oma.drm.content,
                                                       this attribute describes the mime type of
                                                       the object inside the DRM content */
#define WE_PCK_ATTRIBUTE_RIGHTS_ISSUER_URL 0x0200  /* The rights issuer url to a DRM content. */

/*File Flags*/
#define WE_PCK_FLAG_PROTECTED        0x0001   /*This file is protected and cannot
                                                 be removed*/
#define WE_PCK_FLAG_USER_PROTECTED   0x0002   /*If set this file has been locked 
                                                 for changed by the user, Users may
                                                 unlock this protection*/
#define WE_PCK_FLAG_NAME_PROTECTED   0x0004   /*The name of this file is protected
                                                 and cannot be changed*/
#define WE_PCK_FLAG_DRM_FORWARD_LOCK 0x0008   /* This DRM file is forward locked. */

#define WE_PCK_FLAG_DRM_CONSUMABLE   0x0010   /* This DRM file has consumable rights. */


/* Other Defines*/
#define WE_USER_ROOT_FOLDER_TYPE       0
#define WE_USER_FOLDER_TYPE            1

/**********************************************************************
 * Types
 **********************************************************************/
typedef void we_pck_handle_t;

typedef struct {
  WE_INT32    wid;
  const void  *private_data;
} we_pck_pd_item_t;

typedef struct {
  int               num_items;
  we_pck_pd_item_t list[1];
} we_pck_pd_list_t;

/*Types used for handling file attributes.*/
typedef struct we_pck_attr_list_st {
  struct we_pck_attr_list_st *next;
  WE_UINT8      type;        /* Type of attribute value*/
  WE_UINT32     attribute;   /* The attribute */
  union {
    char        *s;
    WE_INT32    i;
    struct {
      WE_UINT8 *b_value;
      WE_INT32  b_len;
    } bv;
  } _u;
} we_pck_attr_list_t;

/*User Folder types*/
typedef struct {
  WE_UINT32   str_id;
  const char  *pathname;
  int          external;
} we_pck_root_folder_item_t;

typedef struct {
  int                        n_rf;
  we_pck_root_folder_item_t rf[1];
} we_pck_root_folders_t;

typedef struct {
  WE_UINT32   str_id;
  const char  *pathname;
} we_pck_user_folder_item_t;

typedef struct {
  int                        n_uf;
  we_pck_user_folder_item_t uf[1];
} we_pck_user_folders_t;

/* Result data type. Each module defines and use its own type values */
typedef struct {
  WE_INT8     type;  /* result type Package specific*/
  union {
    void      *data;  /* data to cast to result type */
    WE_INT32  i_val;
  } _u;
  void        *additional_data;  /* data to cast to additional result type */
} we_pck_result_t;

/* Description of mandatory package functions. */

/**********************************************************************
 * Functions to get information from the User Root Folder and 
 * User Folders table.
 **********************************************************************/


/*
 * This function returns the maximum folder depth of the specified root
 * folder.
 */
int 
we_pck_get_urf_max_depth (const char *pathname);

/*
 * This function returns TRUE if path starts with a root folder, 
 * otherwise FALSE.
 */
int 
we_pck_is_urf_path (const char *path);


/*
 * This function returns a list of root folders defined to be used to 
 * contain any of the specified mime types. mime_types is a null 
 * terminated comma separated list.
 */
void
we_pck_get_urf_from_mime (WE_UINT8               mod_id,
                           const char             *mime_types,
                           we_pck_root_folders_t **result);

/*
 * This function returns a list of all User Root Folders.
 */
void
we_pck_get_all_root_folders (WE_UINT8                mod_id,
                              we_pck_root_folders_t **result);

/*
 * This function returns a list of all User Folders.
 */
void
we_pck_get_all_user_folders (WE_UINT8                mod_id,
                              we_pck_user_folders_t **result);

/*
 * This function returns a localized version of the pathname parameter.
 * It is the callers responsibility to free the returned string
 */
char *
we_pck_get_localized_pathname (WE_UINT8 mod_id, const char *pathname);

/* Returns TRUE if the sub folder in "path" is a user folder name. */
int
we_pck_is_uf (WE_UINT8 mod_id, const char *path, const char *folder);

/*
 * This function returns a localized version of the user folder name
 * parameter. The current working directory is supplied in cwd.
 * It is the callers responsibility to free the returned string
 */
char *
we_pck_get_local_uf (WE_UINT8   mod_id,
                      const char *cwd,
                      const char *folder);
char *
we_pck_get_parent_dir (WE_UINT8 mod_id, const char *path);

/**********************************************************************
 * Functions to create file attribute lists
 **********************************************************************/
/*
 * Initializes the supplied file attribute list. Must be called 
 * before adding anything to the list.
 */
void
we_pck_attr_init (we_pck_attr_list_t **p);

/*
 * Free the memory held by the file attribute list pointer.
 */
void
we_pck_attr_free (WE_UINT8 modid, we_pck_attr_list_t *p);

/*
 * Adds a string attribute to the file attribute list.
 * Returns TRUE if successful.
 *
 * Arguments:
 * modid      (IN)      The module identifier 
 * p          (IN/OUT)  The list to which the attribute will be added.
 * attribute  (IN)      The attribute identifier as defined above.
 * s          (IN)      The string to add.
 */
int
we_pck_add_attr_string_value (WE_UINT8             modid,
                               we_pck_attr_list_t **p,
                               WE_UINT32            attribute,
                               const char           *s);

/*
 * Adds an integer attribute to the file attribute list.
 * Returns TRUE if successful.
 *
 * Arguments:
 * modid      (IN)      The module identifier 
 * p          (IN/OUT)  The list to which the attribute will be added.
 * attribute  (IN)      The attribute identifier as defined above.
 * i          (IN)      The integer value to add.
 */
int
we_pck_add_attr_int_value (WE_UINT8             modid,
                            we_pck_attr_list_t **p,
                            WE_UINT32            attribute,
                            WE_INT32             i);

/*
 * Adds a byte string attribute to the file attribute list.
 * Returns TRUE if successful.
 *
 * Arguments:
 * modid      (IN)      The module identifier 
 * p          (IN/OUT)  The list to which the attribute will be added.
 * attribute  (IN)      The attribute identifier as defined above.
 * bv         (IN)      The byte value to add.
 * length     (IN)      The length in bytes of the byte value to add.
 */
int
we_pck_add_attr_byte_value (WE_UINT8             modid,
                             we_pck_attr_list_t **p,
                             WE_UINT32            attribute,
                             const unsigned char  *bv,
                             long int              length);

/*
 * Returns the size in bytes of the memory occupied by the supplied file
 * attribute list.
 */
int
we_pck_attr_get_size (const we_pck_attr_list_t *p);

/*
 * Create a string corresponding to a file attribute list.
 * NOTE: it is the caller's responsibility to deallocate the string.
 */
void
we_pck_attr_to_string (WE_UINT8                   modid,
                        const we_pck_attr_list_t  *p,
                        long                       *length,
                        unsigned char             **attr_string);

/*
 * Create a file attribute list corresponding to a string.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned attribute list.
 */
we_pck_attr_list_t *
we_pck_attr_to_list (WE_UINT8      modid,
                      long           length,
                      unsigned char *attr_string);

/*
 * Duplicates the file attribute list supplied.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned file attribute list.
 */
we_pck_attr_list_t *
we_pck_attr_dupl (WE_UINT8                   modid,
                   const we_pck_attr_list_t  *p);


/**********************************************************************
 * Utility functions for mime types
 **********************************************************************/

WE_BOOL
we_pck_mime_type_in_list (const char *mime_type, const char *mime_list);


#endif


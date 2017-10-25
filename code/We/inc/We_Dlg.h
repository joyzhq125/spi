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

#ifndef _we_dlg_h
#define _we_dlg_h

#ifndef _we_def_h
#include "We_Def.h"
#endif

#ifndef _we_pck_h
#include "We_Pck.h"
#endif

#ifndef _we_int_h
#include "We_Int.h"
#endif


 /**********************************************************************
 * Defines 
 **********************************************************************/

/*Dialog return values*/
#define WE_DLG_RETURN_OK                0
#define WE_DLG_RETURN_CANCELLED        -1
#define WE_DLG_RETURN_TIME_OUT         -2
#define WE_DLG_RETURN_NOT_AVAILABLE    -3

/*Dialog result types*/
#define WE_DLG_TYPE_SAVE_AS             1
#define WE_DLG_TYPE_OPEN                2
#define WE_DLG_TYPE_CONFIRM             3
#define WE_DLG_TYPE_MESSAGE             4
#define WE_DLG_TYPE_PROMPT              5
#define WE_DLG_TYPE_LOGIN               6
#define WE_DLG_TYPE_PROGRESSBAR         7

/* bit flags */
/* Must be 0x0001 to allow backward compatibility*/
#define WE_DLG_ALLOW_OVERWRITE         0x0001 
/*This flag is used to prevent objects to be stored on external memory.*/
#define FRW_DLG_NO_EXTERNAL_MEMORY      0x0002 

#define WE_DLG_PROGRESS_SHOW_TEXT_ONE 0x0001
#define WE_DLG_PROGRESS_SHOW_TEXT_TWO 0x0002
#define WE_DLG_PROGRESS_PER_CENT      0x0004
#define WE_DLG_PROGRESS_NUMBER        0x0008
#define WE_DLG_PROGRESS_SHARE_OF      0x0010
#define WE_DLG_PROGRESS_NOT_CANCEL    0x0020


/*DRM Permissions*/
#define WE_DLG_PERMISSION_ALL              0x0001
#define WE_DLG_PERMISSION_FORWARD          0x0002


 /**********************************************************************
 * Types
 **********************************************************************/
typedef enum {
  we_dlg_alert       = WeAlert,
  we_dlg_warning     = WeWarning,
  we_dlg_info        = WeInfo,
  we_dlg_error       = WeError
} we_dialog_type_t;

typedef enum {
  we_dlg_pkg_type_normal = 0,
  we_dlg_pkg_type_secure
} we_dlg_pkg_type_t;

/*General dialog data*/
typedef struct we_dlg_st {
  we_pck_handle_t  *handle;
  WeScreenHandle   screen;
  WeStringHandle   title;
} we_dlg_t;

typedef void(*we_dlg_result_callback)(we_pck_result_t* result);

/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialise the dialog package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t *
we_dlg_init  (WE_UINT8 modid);

void
we_dlg_set_type (we_pck_handle_t *handle, we_dlg_pkg_type_t pakage_type);


/*
 * Runs the dialog package. When the module receives a signal that is
 * defined by WE, the module must call this function because the
 * dialog package might be the receiver.
 *
 * Returns
 *    WE_PACKAGE_SIGNAL_NOT_HANDLED
 *      The signal was not handled by the package, the caller
 *      must process the signal by itself.
 *    WE_PACKAGE_SIGNAL_HANDLED
 *      The signal was handled by the package.
 *    WE_PACKAGE_OPERATION_COMPLETE
 *      The signal was handled by the package and the dialog is
 *      finished. In this case the wid parameter indicates which dialog
 *      that has finished. 
 */
int
we_dlg_handle_signal (we_pck_handle_t* handle, WE_UINT16 signal,
                       void* p, WE_INT32* wid);

/*
 * Set private data connected to a specific dialog. The private data
 * might be used by the module to indicate which sub module that was
 * created the dialog. This function may be called after a
 * we_dlg_create_xxx function has been called.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_dlg_set_private_data (we_pck_handle_t* handle,
                          WE_INT32 wid, void* p);

/*
 * Get private data connected to a specific dialog. The private data
 * might be used by the module to indicate which sub module that was
 * created the dialog. This function may be called after
 * we_dlg_handle_signal has returned WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns NULL.
 */
void*
we_dlg_get_private_data (we_pck_handle_t* handle, WE_INT32 wid);

/*
 * Terminates the dialog package, release all resources allocated by
 * this package. If there is any open dialogs, these dialog are
 * deleted. Any private data must be released by the module itself.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_dlg_terminate (we_pck_handle_t* handle);

/**********************************************************************
 * Delete 
 **********************************************************************/

/*
 * Deletes a created dialog. Any private data is not released by this
 * function. The module call this function when the module of any
 * reason must delete the dialog before the finished state has been
 * reached.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_dlg_delete (we_pck_handle_t* handle, WE_INT32 wid);

/**********************************************************************
 * Result
 **********************************************************************/

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
we_dlg_get_result(we_pck_handle_t* handle, WE_INT32 wid, we_pck_result_t* result);

int
we_dlg_get_result_ext (we_pck_handle_t* handle, WE_INT32 wid, 
                        we_pck_result_t* result);

/*
 * Free the storage allocated in the get result function
 *
 * Argument:
 * result       Result data to be freed.
 *
 * Returns:     TRUE if success, otherwise FALSE. 
 */

int
we_dlg_result_free(we_pck_handle_t* handle, we_pck_result_t* result);

/**********************************************************************
 * Focus
 **********************************************************************/

/*
 * Sets the dialog in focus.
 */
void 
we_dlg_set_in_focus(we_pck_handle_t* handle);

/**********************************************************************
 * Save as dialog
 **********************************************************************/
/*
 * Create a save as dialog.
 *
 * Parameters:
 *  dlg_data              Generic dilaog data (handle, screen, title)     
 *  file_name             Suggested user friendly file name of the file.
 *  mime_type             The mime type of file to save.
 *  save_here_str         "save here" string. If 0 a default string will be used.
 *  bit_flag              WE_DLG_ALLOW_OVERWRITE   -  if bit is set, the package will prompt
 *                          the user about the replace and possibly replace the existing file.
 *                        FRW_DLG_NO_EXTERNAL_MEMORY - If this bit is set no folders on
 *                          external memory will be possible to select as save destination.
 * 
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */
typedef struct we_dlg_save_as_st {
  we_dlg_t       dlg_data;
  const char*     file_name;
  const char*     mime_type;
  WeStringHandle save_here_str;
  WE_INT32       bit_flag;
} we_dlg_save_as_t;

typedef struct {
  int   result;              /* WE_DLG_RETURN_OK - the user selected a
                                file name,
                                WE_DLG_RETURN_CANCELLED if user canceled
                                dialog.
                                WE_DLG_RETURN_NOT_AVAILABLE if the dialog
                                is still active*/                            
  char  *file_name;          /* If result was TRUE, there is a file
                                name (including path) in this parameter,
                                otherwise NULL. */
} we_dlg_result_save_as_t;



WE_INT32
we_dlg_create_save_as (const we_dlg_save_as_t* save_as_data);

/*
 * Get the result of a completed save as operation. The caller must
 * call this function when we_dlg_handle_signal has returned
 * WE_PACKAGE_OPERATION_COMPLETE for this specific dialog. This
 * function also releases all allocated resources connected to the
 * specific dialog , but only if the function is called when the
 * operation is finished.
 * Returns TRUE if success, otherwise FALSE.
 */

/**********************************************************************
 * Open dialog
 **********************************************************************/
/*
 * Create an open dialog.
 *
 * Open parameters:
 *  dlg_data      Generic dialog data (handle, screen, title).
 *  permission    Do not show files when this permission(s) is not valid.
 *                E.g. if the caller will send forward the selected
 *                content, any DRM protected content must not be showed
 *                to the end user.
 * 
 *                WE_DLG_PERMISSION_ALL              All files are shown
 *                WE_DLG_PERMISSION_FORWARD          Forward locked files 
 *                                                    are not shown
 *
 *  mime_types    Which type of files that will be shown to the user.
 *                Wildcards is allowed in this parameter.
 * 
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */
typedef struct we_dlg_open_st {
  we_dlg_t   dlg_data;
  int         permission;
  const char  *mime_types;
} we_dlg_open_t;

typedef struct {
  int   result;              /* WE_DLG_RETURN_OK - the user selected a
                                file,
                                WE_DLG_RETURN_CANCELLED if user canceled
                                dialog.
                                WE_DLG_RETURN_NOT_AVAILABLE if the dialog
                                is still active */
  char  *file_name;          /* If result was TRUE, there is a file
                                name (including path) in this parameter,
                                otherwise NULL. */
} we_dlg_result_open_t;


WE_INT32
we_dlg_create_open (const we_dlg_open_t* open_data);

/*
 * Get the result of a completed open operation. The caller must call
 * this function when we_dlg_handle_signal has returned
 * WE_PACKAGE_OPERATION_COMPLETE for this specific dialog. This
 * function also releases all allocated resources connected to the
 * specific dialog, but only if the function is called when the
 * operation is finished.
 * Returns TRUE if success, otherwise FALSE.
 */


/**********************************************************************
 * Message dialog
 **********************************************************************/
/*
 * Create a message dialog.
 *
 * Message parameters:
 *  dlg_data        Generic dilaog data (handle, screen, title)     
 *  dialog_type     Specifies the type of the dialog, The valid types are
 *                  PkgDlgAlert, PkgDlgWarning, PkgDlgInfo and PkgDlgError.
 *  nbr_of_str_ids  Number if items in messages array.
 *  str_ids         Array of WeStringHandle´s to display.
 *  ok_string       String connected to ok action if set to 0 a default string
 *                  will be used.
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */

/* message dialog */
typedef struct we_dlg_message_st {
  we_dlg_t         dlg_data;
  we_dialog_type_t dialog_type;
  int               nbr_of_str_ids;
  WeStringHandle   *str_ids;
  WeStringHandle   ok_string;
} we_dlg_message_t;

typedef struct {
  int   result;     /* WE_DLG_RETURN_OK - the user selected ok,
                       WE_DLG_RETURN_CANCELLED if user canceled dialog.
                       WE_DLG_RETURN_NOT_AVAILABLE if the dialog is still active*/
} we_dlg_result_message_t;

WE_INT32
we_dlg_create_message (const we_dlg_message_t* message_data);

WE_INT32 
we_dlg_create_multi_message (const we_dlg_message_t* message_data);

/**********************************************************************
 * Confirm dialog
 **********************************************************************/
/*
 * Create a confirm dialog.
 *
 * Confirm parameters:
 *  dlg_data        Generic dialog data (Handle, screen, title)
 *  nbr_of_str_ids  Number of string ids in str_ids array.
 *  str_ids         Array of WeStringHandle´s to display 
 *  ok_string       String connected to ok action if set to 0 a default string
 *                  will be used.
 *  cancel_string   String connected to cancel action if set to 0 a default string
 *                  will be used.
 * 
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */

typedef struct we_dlg_confirm_st {
  we_dlg_t        dlg_data;
  int              nbr_of_str_ids;
  WeStringHandle  *str_ids;
  WeStringHandle  ok_string;
  WeStringHandle  cancel_string;
} we_dlg_confirm_t;


typedef struct {
  int   result;  /* WE_DLG_RETURN_OK - the user selected ok,
                    WE_DLG_RETURN_CANCELLED if user canceled dialog.
                    WE_DLG_RETURN_NOT_AVAILABLE if the dialog is still active*/
} we_dlg_result_confirm_t;

WE_INT32
we_dlg_create_multi_confirm (const we_dlg_confirm_t* confirm_data);

WE_INT32
we_dlg_create_confirm (const we_dlg_confirm_t* confirm_data);

/**********************************************************************
 * Prompt dialog
 **********************************************************************/
/*
 * Create a Prompt dialog.
 *
 * Prompt parameters:
 *  dlg_data            Generic dialog data (Handle, screen, title).
 *  message             Text to display in dialog.
 *  nbr_of_input_fields Number of input fileds in input_fields array.
 *  ok_string           String connected to ok action. If 0 a default string
 *                      will be used.
 *  cancel_string       String connected to cancel action if 0 a default string
 *                      will be used.
 *  input_fields        An array of input fileds.
 * 
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */

typedef struct we_dlg_input_field_st {
  WeStringHandle label;
  WeStringHandle default_text;
  WeTextType     type;
  int             max_size;
} we_dlg_input_field_t;

typedef struct we_dlg_prompt_st {
  we_dlg_t              dlg_data;
  WeStringHandle        message;
  int                    nbr_of_input_fields;
  WeStringHandle        ok_string;
  WeStringHandle        cancel_string;
  we_dlg_input_field_t  *input_fields;
} we_dlg_prompt_t;

typedef struct {
  WE_UINT32  result; /* WE_DLG_RETURN_OK - the user selected ok,
                        WE_DLG_RETURN_CANCELLED if user canceled dialog.
                        WE_DLG_RETURN_NOT_AVAILABLE if the dialog is still active*/
  WE_UINT32  nbr_of_input_fields;
  char        **input_fields;
} we_dlg_result_prompt_t;

WE_INT32
we_dlg_create_prompt (const we_dlg_prompt_t* prompt_data);

WE_INT32
we_dlg_create_multi_prompt(const we_dlg_prompt_t* prompt_data);

/**********************************************************************
 * Progress bar
 **********************************************************************/
/*
 * Create a Progressbar.
 *
 * Prompt parameters:
 *  dlg_data        Generic dialog data (Handle, screen, title).
 *  max_value       Max value for the progressbar.
 *  value           start value for the progressbar.
 *  text_one        First text in progressbar (e.g Downloading) 
 *  text_two        Second text in progressbar (e.g Bytes)
 *  propmask        WE_DLG_PROGRESS_SHOW_TEXT_ONE
 *                  WE_DLG_PROGRESS_SHOW_TEXT_TWO
 *                  WE_DLG_PROGRESS_PROCENT
 *                  WE_DLG_PROGRESS_NUMBER
 *                  WE_DLG_PROGRESS_SHARE_OF
 *                  WE_DLG_PROGRESS_NOT_CANCEL
 *  cancel_string   String connected to cancel action if 0 a default string
 *                  will be used.
 * 
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */

typedef struct we_dlg_progress_bar_st {
  we_dlg_t       dlg_data;
  int             max_value;
  int             value;
  WeStringHandle text_one;
  WeStringHandle text_two;
  int             propmask;
  WeStringHandle cancel_string;
} we_dlg_progress_bar_t;

typedef struct {
  int   result;    /* WE_DLG_RETURN_CANCELLED if user canceled dialog.
                      WE_DLG_RETURN_NOT_AVAILABLE if the dialog is still active */
} we_dlg_result_progress_t;


WE_INT32
we_dlg_create_progressbar (const we_dlg_progress_bar_t* progress_data);

int
we_dlg_update_progressbar (we_pck_handle_t *handle, WE_UINT32 wid, 
                            const we_dlg_progress_bar_t* progress_data);

WE_INT32
we_dlg_create_loop_progressbar (const we_dlg_progress_bar_t* progress_data,
                                 WE_UINT32 timer_sig, int tick_time);

/**********************************************************************
 * Login dialog
 **********************************************************************/
/*
 * Create a login dialog.
 *
 * Prompt parameters:
 *  dlg_data               Generic dialog data (Handle, screen, title).
 *  username               Input filed used for username.
 *  password               Input filed used for password.
 *  nbr_of_selection_boxes Number of selection boxes in dialog.
 *  selectboxes            Array of specifications for each selection box.
 *  ok_string              String connected to ok action. If 0 a default string
 *                         will be used.
 *  cancel_string          String connected to cancel action if 0 a default string
 *                         will be used.
 *
 * Returns a positive identifier if success, otherwise WE_PACKAGE_ERROR.
 */

typedef struct we_dlg_selectbox_st {
  WeStringHandle label;
  int             default_value;/*WE_CHOICE_ELEMENT_SELECTED
                                  WE_CHOICE_ELEMENT_DISABLED
                                  WE_CHOICE_ELEMENT_FOCUSED*/
} we_dlg_selectbox_t;

typedef struct we_dlg_login_st {
  we_dlg_t             dlg_data;
  we_dlg_input_field_t username;
  we_dlg_input_field_t password;
  int                   nbr_of_selection_boxes;
  we_dlg_selectbox_t   *selectboxes;
  WeStringHandle       ok_string;
  WeStringHandle       cancel_string;
} we_dlg_login_t;

typedef struct {
  WE_UINT32 result;         /* WE_DLG_RETURN_OK - the user selected ok,
                                WE_DLG_RETURN_CANCELLED if user canceled dialog.
                                WE_DLG_RETURN_NOT_AVAILABLE if the dialog is still active*/
  char        *username;
  char        *password;
  WE_UINT32  nbr_of_select_value;
  int         *select_values;      /*WE_CHOICE_ELEMENT_SELECTED
                                   WE_CHOICE_ELEMENT_DISABLED
                                   WE_CHOICE_ELEMENT_FOCUSED*/
} we_dlg_result_login_t;

WE_INT32 
we_dlg_create_login (const we_dlg_login_t* login_data);

#endif/*_we_dlg_h*/

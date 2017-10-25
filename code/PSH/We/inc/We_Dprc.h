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
 * We_Dprc.h (we dialog package resources)
 *
 * Created by Stefan Svensson, Ons Nov 12 2003.
 *
 * Revision  history:
 *   031112, stsn: Created
 *
 */

#ifndef _we_dprc_h
#define _we_dprc_h

#ifndef  _we_def_h
#include "We_Def.h"
#endif

#ifndef  _we_int_h
#include "We_Int.h"
#endif

/**********************************************************************
 * Constants
 **********************************************************************/

/* Types of dialogues */
#define WE_DLG_MESSAGE_ALERT         1
#define WE_DLG_MESSAGE_WARNING       2
#define WE_DLG_MESSAGE_INFO          3
#define WE_DLG_MESSAGE_ERROR         4

#define WE_DLG_CONFIRM               5
#define WE_DLG_PROMPT                6
#define WE_DLG_PROGRESS_BAR          7
#define WE_DLG_LOGIN                 8
#define WE_DLG_MENU                  9
#define WE_DLG_FILE                 10

/* Response types */
#define WE_DLG_RESPONSE_POSITIVE     1
#define WE_DLG_RESPONSE_NEGATIVE     2
#define WE_DLG_RESPONSE_CANCEL       3
#define WE_DLG_RESPONSE_TIMEOUT      4
#define WE_DLG_RESPONSE_MENU         5

/**********************************************************************
 * Type definitions
 **********************************************************************/

typedef struct we_dprc_st we_dprc_t;

typedef enum {
  we_dprc_pkg_type_normal = 0,
  we_dprc_pkg_type_secure
} we_dprc_pkg_type_t;


/* message dialog */
typedef struct we_dprc_message_st {
  we_dprc_pkg_type_t pakage_type;
  WeStringHandle     title;
  int                 numStrId;
  WeStringHandle    *str_ids;
  int                 singel_dlg;
  WeStringHandle     ok_string;
} we_dprc_message_t;

/* confirm dialog */
typedef struct we_dprc_confirm_st {
  we_dprc_pkg_type_t pakage_type;
  WeStringHandle     title;
  int                 numStrId;
  WeStringHandle    *str_ids;
  int                 singel_dlg;
  WeStringHandle     ok_string;
  WeStringHandle     cancel_string;
} we_dprc_confirm_t;

/* prompt dialog*/
typedef struct we_dprc_input_field_st {
  WeStringHandle label;
  WeStringHandle default_text;
  WeTextType     type;
  int             max_size;
} we_dprc_input_field_t;

typedef struct we_dprc_prompt_st {
  we_dprc_pkg_type_t     pakage_type;
  WeStringHandle         title;
  WeStringHandle         message;
  int                     nbr_of_input_fields;
  we_dprc_input_field_t *input_fields;
  int                     singel_dlg;
  WeStringHandle         ok_string;
  WeStringHandle         cancel_string;
}we_dprc_prompt_t;

typedef struct we_dprc_prompt_result_st {
  WeStringHandle *string_list;
  int              num_of_strings;
}we_dprc_prompt_result_t;

/* progress bar */
typedef struct we_dprc_progress_bar_st {
  we_dprc_pkg_type_t pakage_type;
  WeStringHandle     title;
  WeStringHandle     text_one;
  WeStringHandle     text_two;
  int                 propmask;
  int                 max_value;
  int                 value;
  int                 tick_time;
  WeStringHandle     ok_string;
  WeStringHandle     cancel_string;
} we_dprc_progress_bar_t;

/* login dialog*/
typedef struct we_dprc_selectbox_st {
  WeStringHandle select_str_ids;
  int             select_value;
} we_dprc_selectbox_t;

typedef struct we_dprc_login_st {
  we_dprc_pkg_type_t     pakage_type;
  WeStringHandle         title;
  we_dprc_input_field_t  input_fields[2];
  int                     nbr_of_selection_boxes;
  we_dprc_selectbox_t   *selectboxes;
  WeStringHandle         ok_string;
  WeStringHandle         cancel_string;
} we_dprc_login_t;

typedef struct we_dprc_login_result_st {
  WeStringHandle string_list[2];
  int             nbr_of_select_value;
  int            *select_value;
} we_dprc_login_result_t;


typedef struct we_dprc_menu_item_st {
  WeStringHandle tooltip_string;
  WeStringHandle string1;
  WeStringHandle string2;
  WeImageHandle image1;
  WeImageHandle image2;
} we_dprc_menu_item_t;

typedef struct we_dprc_menu_st {
  we_dprc_pkg_type_t   pakage_type;
  WeStringHandle       title;
  WeChoiceType         type;
  int                   nbr_of_items;
  we_dprc_menu_item_t *items;
  WeStringHandle       ok_string;
  WeStringHandle       cancel_string;
} we_dprc_menu_t;

typedef struct we_dprc_file_st {
  we_dprc_pkg_type_t   pakage_type;
  WeStringHandle       title;
  WeChoiceType         type;
  int                   nbr_of_items;
  we_dprc_menu_item_t *items;
  WeStringHandle       ok_string;
  WeStringHandle       cancel_string;
  int                   is_open_dialog;
} we_dprc_file_t;

typedef struct we_dprc_file_result_st {
  int nbr_of_items;
  int selected_item;
} we_dprc_file_result_t;

typedef struct we_dprc_menu_result_st {
  int nbr_of_select_values;
  int *select_values;  
} we_dprc_menu_result_t;


/*********************************************************************
 * Exported functions
 **********************************************************************/

/*
 * Create a dialog.
 * Returns a pointer to a struct we_dprc_st, or NULL on error.
 */
we_dprc_t *
we_dprc_create (WE_UINT8 client_modid, WeScreenHandle screenHandle, int dialog_type, void *dlg_data);

/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 */
void
we_dprc_delete (we_dprc_t *dlg);

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
we_dprc_action (we_dprc_t *dlg, WE_UINT32 action);

/*
 * Return true if the handle is the dialog false otherwice.
 * Returns -1 on error. 
 */

int
we_dprc_check_handle (we_dprc_t *dlg, WE_UINT32 handle);
/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling we_drcAction).
 */
int
we_dprc_get_response (we_dprc_t *dlg, void* response_data);

/*
 *  Updates the progressbar width new values
 */
int
we_dprc_update_progressbar (we_dprc_t* dlg, we_dprc_progress_bar_t* dlg_data);

/*
 * Sets the dialog in focus.
 */
void
we_dprc_set_in_focus(we_dprc_t *dlg);

void
we_dprc_set_title (we_dprc_t *dlg, WeStringHandle title);

void
we_dprc_set_background_image (we_dprc_t *dlg, WeImageHandle image);

void
we_dprc_event_subscribe (we_dprc_t *dlg,int event_type, int subscribe);

/*
 *  file dialog fuctions
 */
int
we_dprc_add_item_to_file_dlg (we_dprc_t *dlg, we_dprc_menu_item_t* item_data,
                               int index);
void
we_dprc_clear_file_dlg (we_dprc_t *dlg);

void
we_dprc_set_file_dlg_item_state (we_dprc_t *dlg, int index, int state);

#endif/*_we_dprc_h*/

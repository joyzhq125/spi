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
 * Pus_Dlg.c
 *
 * Created by Kent Olsson, Wed Nov 21 18:23:35 2002.
 *
 * Revision history:
 *
 */


#include "we_def.h"
#include "we_int.h"
#include "we_cmmn.h"
#include "we_core.h"
#include "we_lib.h"
#include "we_wid.h"

#include "Pus_Dlg.h"
#include "Pus_Main.h"
#include "Pus_Rc.h"

/************************************************************
 * Global variables
 ************************************************************/

int pus_screen_handle;

/**********************************************************************
 * Constants
 **********************************************************************/

/* Number of handles used by different dialog types: */
#define PUS_DLG_SIA_CONFIRM_NUM_HANDLES                     6
#define PUS_DLG_WHITELIST_NO_MATCH_NUM_HANDLES      6

/* Indices into the array of handles */
#define PUS_DLG_DIALOG_HANDLE                     1

#define PUS_DLG_MESSAGE_HANDLE                    2

#define PUS_DLG_OK_STRING_HANDLE                  3
#define PUS_DLG_OK_ACTION_HANDLE                  4
#define PUS_DLG_CANCEL_STRING_HANDLE              5
#define PUS_DLG_CANCEL_ACTION_HANDLE              6

#define PUS_DLG_NO_SCREEN                         0

/**********************************************************************
 * Type definitions
 **********************************************************************/

/**********************************************************************
 * Local functions
 **********************************************************************/

static Pus_Dlg_t *
Pus_Dlg_create (int dialog_type, int num_handles);

static int
Pus_Dlg_create_sia_confirm_dialog (Pus_Dlg_t *dlg, WE_UINT32 message,
                                   WE_UINT32 ok, WE_UINT32 cancel);

static int
Pus_Dlg_create_whitelist_no_match_dialog (Pus_Dlg_t *dlg, WE_UINT32 message,
                                          WE_UINT32 ok, WE_UINT32 cancel, char* info);


/**********************************************************************
 * Exported functions
 **********************************************************************/

void
Pus_Dlg_init (void)
{
  pus_screen_handle = PUS_DLG_NO_SCREEN;
}

void
Pus_Dlg_terminate (void)
{
  if (pus_screen_handle != PUS_DLG_NO_SCREEN)
    WE_WIDGET_RELEASE (pus_screen_handle);
}

/*
 * Create a dialog.
 * Returns a pointer to a struct Pus_Dlg_st, or NULL on error.
 */
Pus_Dlg_t *
Pus_DlgCreate (int dialogType, int numStrings, WE_UINT32 strings[], char* info)
{
  Pus_Dlg_t *dlg = NULL;
  
  if (pus_screen_handle == PUS_DLG_NO_SCREEN) {
    pus_screen_handle = WE_WIDGET_SCREEN_CREATE (WE_MODID_PUS, 0);
    WE_WIDGET_DISPLAY_ADD_SCREEN (pus_screen_handle);
  }
  
  switch (dialogType) {
  case PUS_DLG_SIA_CONFIRM:
    if (numStrings < 3) {
      return NULL;
    }
    dlg = Pus_Dlg_create (dialogType, PUS_DLG_SIA_CONFIRM_NUM_HANDLES);
    Pus_Dlg_create_sia_confirm_dialog (dlg, strings[0], strings[1], strings[2]);
    break;

  case PUS_DLG_WHITELIST_NO_MATCH:
    if (numStrings < 3) {
      return NULL;
    }
    dlg = Pus_Dlg_create (dialogType, PUS_DLG_WHITELIST_NO_MATCH_NUM_HANDLES);
    Pus_Dlg_create_whitelist_no_match_dialog (dlg, strings[0], strings[1], strings[2], info);
    break;
    
  default:
    break;
  }

  if (dlg != NULL) {
    WE_WIDGET_SET_IN_FOCUS (pus_screen_handle, 1);
    WE_WIDGET_SET_IN_FOCUS (dlg->handle[PUS_DLG_DIALOG_HANDLE], 1);
  }

  return dlg;
}

/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 */
void
Pus_DlgDelete (Pus_Dlg_t *dlg, int moreDialogs)
{
  int i;

  /* NOTE: first the Dialog window must be removed from the Screen.
   * Then we release the Dialog window. This will cause all other
   * widgets that are attached to the Dialog to be removed from
   * the Dialog window */
  WE_WIDGET_REMOVE (pus_screen_handle, dlg->handle[PUS_DLG_DIALOG_HANDLE]);
  WE_WIDGET_RELEASE (dlg->handle[PUS_DLG_DIALOG_HANDLE]);
  for (i = dlg->num_handles; i > 1; i--) {
    WE_WIDGET_RELEASE (dlg->handle[i]);
  }

  if (!moreDialogs) {
    WE_WIDGET_REMOVE (pus_screen_handle, pus_screen_handle);
    WE_WIDGET_RELEASE (pus_screen_handle);
    pus_screen_handle = PUS_DLG_NO_SCREEN;
  }

  PUS_MEM_FREE (dlg);
}

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
Pus_DlgAction (Pus_Dlg_t *dlg, WE_UINT32 action)
{
  if (dlg != NULL) {
    if (dlg->handle[PUS_DLG_OK_ACTION_HANDLE] == action) {
      dlg->response = PUS_DLG_RESPONSE_POSITIVE;
    }
    else {
      dlg->response = PUS_DLG_RESPONSE_CANCEL;
    }
  }
}

/*
 * Return the type of dialog that we have (one of the Dialog types
 * above).
 * Returns 0 on error.
 */
int
Pus_DlgGetType (Pus_Dlg_t *dlg)
{
  if (dlg == NULL)
    return 0;

  return dlg->type;
}

/*
 * Return the Dialog Handle associated with the specified dialog.
 * Returns 0 on error. 
 */
WE_UINT32
Pus_DlgGetDialogHandle (Pus_Dlg_t *dlg)
{
  if (dlg != NULL) {
    return dlg->handle[PUS_DLG_DIALOG_HANDLE];
  }

  return 0;
}

/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling Pus_DlgAction).
 */
int
Pus_DlgGetResponse (Pus_Dlg_t *dlg)
{
  if (dlg != NULL) {
    return dlg->response;
  }

  return 0;
}


/**********************************************************************
 * Local functions
 **********************************************************************/

static Pus_Dlg_t *
Pus_Dlg_create (int dialog_type, int num_handles)
{
  Pus_Dlg_t *dlg = PUS_MEM_ALLOCTYPE (Pus_Dlg_t);

  dlg->type = dialog_type;
  dlg->num_handles = num_handles;

  return dlg;
}


static int
Pus_Dlg_create_sia_confirm_dialog (Pus_Dlg_t *dlg, WE_UINT32 message,
                                   WE_UINT32 ok, WE_UINT32 cancel)
{
  WE_UINT32      dialog_handle;
  WePosition     null_pos = {0, 0};
  WeStringHandle msg_handle = 0;
  WeStringHandle ok_string;
  WeStringHandle cancel_string;
  WeActionHandle ok_action;
  WeActionHandle cancel_action;
  
  /* Create message string */
  if (message != 0) {
    msg_handle   = WE_WIDGET_STRING_GET_PREDEFINED (message);
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, (char *)message, WeUtf8,
                                             strlen (message) + 1, 0);*/
  }

  /* Create Dialog */
  dialog_handle = WE_WIDGET_DIALOG_CREATE (WE_MODID_PUS, msg_handle,
                                            WeConfirmation, 0);
  
  /* Action Ok */
  if (ok != 0) {
    ok_string = WE_WIDGET_STRING_GET_PREDEFINED (ok);
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, (char *)ok, WeUtf8,
                                          strlen (ok) + 1, 0);*/
  }
  else {
    ok_string =  WE_WIDGET_STRING_GET_PREDEFINED (PUS_STR_ID_OK);
    
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, PUS_DLG_STRING_OK, WeUtf8,
                                          strlen (PUS_DLG_STRING_OK) + 1, 0);
                                          */

  }
  ok_action = WE_WIDGET_ACTION_CREATE (WE_MODID_PUS, ok_string,
                                        0, WeOk);
  WE_WIDGET_ADD_ACTION (dialog_handle, ok_action);
  dlg->handle[PUS_DLG_OK_STRING_HANDLE] = ok_string;
  dlg->handle[PUS_DLG_OK_ACTION_HANDLE] = ok_action;

  /* Action Cancel */
  if (cancel != 0) {
    cancel_string = WE_WIDGET_STRING_GET_PREDEFINED (cancel);
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, (char *)cancel, WeUtf8,
                                              strlen (cancel) + 1, 0);*/
  }
  else {
    cancel_string = WE_WIDGET_STRING_GET_PREDEFINED (PUS_STR_ID_CANCEL);
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, PUS_DLG_STRING_CANCEL,
                                              WeUtf8,
                                              strlen (PUS_DLG_STRING_CANCEL) + 1, 0);*/
  }
  cancel_action = WE_WIDGET_ACTION_CREATE (WE_MODID_PUS, cancel_string,
                                            0, WeCancel);
  WE_WIDGET_ADD_ACTION (dialog_handle, cancel_action);
  dlg->handle[PUS_DLG_CANCEL_STRING_HANDLE] = cancel_string;
  dlg->handle[PUS_DLG_CANCEL_ACTION_HANDLE] = cancel_action;

  dlg->handle[PUS_DLG_MESSAGE_HANDLE] = msg_handle;
  dlg->handle[PUS_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (pus_screen_handle,
                                dialog_handle, &null_pos);

  return 0;
}

static int
Pus_Dlg_create_whitelist_no_match_dialog (Pus_Dlg_t *dlg, WE_UINT32 message,
                                          WE_UINT32 ok, WE_UINT32 cancel, char* info)
{
  WE_UINT32        dialog_handle;
  WePosition       null_pos = {0, 0};
  WeStringHandle   msg_handle = 0;
  WeStringHandle   ok_string;
  WeStringHandle   cancel_string;
  WeStringHandle   message_text_handle = 0;
  WeActionHandle   ok_action;
  WeActionHandle   cancel_action;
  char             *predefined_text, *new_text = NULL;
  int               predefined_text_len;
  
  /* Create message string */
  if (message != 0) {
    WE_UINT32 info_length = strlen(info); /* Without the null termination */
    msg_handle   = WE_WIDGET_STRING_GET_PREDEFINED (message);
    predefined_text_len = WE_WIDGET_STRING_GET_LENGTH (msg_handle, 1, WeUtf8); /* Including the null termination */
    predefined_text = PUS_MEM_ALLOC (predefined_text_len);
    WE_WIDGET_STRING_GET_DATA (msg_handle, predefined_text, WeUtf8);
    new_text = PUS_MEM_ALLOC (predefined_text_len + 2 + info_length);
    sprintf(new_text, "%s: %s",predefined_text, info);
    WE_WIDGET_RELEASE (msg_handle);
    PUS_MEM_FREE (predefined_text);
    message_text_handle = WE_WIDGET_STRING_CREATE (WE_MODID_PUS, new_text, WeUtf8,
                                                    strlen (new_text) + 1, 0);
    PUS_MEM_FREE (new_text);
  }

  /* Create Dialog */
  dialog_handle = WE_WIDGET_DIALOG_CREATE (WE_MODID_PUS, message_text_handle,
                                            WeConfirmation, 0);

  /* Action Ok */
  if (ok != 0) {
    ok_string = WE_WIDGET_STRING_GET_PREDEFINED (ok);
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, (char *)ok, WeUtf8,
                                          strlen (ok) + 1, 0);*/
  }
  else {
    ok_string =  WE_WIDGET_STRING_GET_PREDEFINED (PUS_STR_ID_OK);
    
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, PUS_DLG_STRING_OK, WeUtf8,
                                          strlen (PUS_DLG_STRING_OK) + 1, 0);
                                          */

  }
  ok_action = WE_WIDGET_ACTION_CREATE (WE_MODID_PUS, ok_string,
                                        0, WeOk);
  WE_WIDGET_ADD_ACTION (dialog_handle, ok_action);
  dlg->handle[PUS_DLG_OK_STRING_HANDLE] = ok_string;
  dlg->handle[PUS_DLG_OK_ACTION_HANDLE] = ok_action;

  /* Action Cancel */
  if (cancel != 0) {
    cancel_string = WE_WIDGET_STRING_GET_PREDEFINED (cancel);
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, (char *)cancel, WeUtf8,
                                              strlen (cancel) + 1, 0);*/
  }
  else {
    cancel_string = WE_WIDGET_STRING_GET_PREDEFINED (PUS_STR_ID_CANCEL);
    /*WE_WIDGET_STRING_CREATE (WE_MODID_PUS, PUS_DLG_STRING_CANCEL,
                                              WeUtf8,
                                              strlen (PUS_DLG_STRING_CANCEL) + 1, 0);*/
  }
  cancel_action = WE_WIDGET_ACTION_CREATE (WE_MODID_PUS, cancel_string,
                                            0, WeCancel);
  WE_WIDGET_ADD_ACTION (dialog_handle, cancel_action);
  dlg->handle[PUS_DLG_CANCEL_STRING_HANDLE] = cancel_string;
  dlg->handle[PUS_DLG_CANCEL_ACTION_HANDLE] = cancel_action;

  dlg->handle[PUS_DLG_MESSAGE_HANDLE] = message_text_handle;
  dlg->handle[PUS_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (pus_screen_handle,
                                dialog_handle, &null_pos);
  return 0;
}


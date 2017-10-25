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
 * Frw_Dlg.c
 *
 * Created by Robert Moricz, Fri Oct 24 2003.
 *
 * Revision  history:
 *
 */

#include "Frw_Dlg.h"
#include "We_Rc.h"

#include "We_Def.h"
#include "We_Int.h"
#include "We_Mem.h"
#include "We_Wid.h"


/**********************************************************************
 * Constants
 **********************************************************************/

/* Indices into the array of handles */
#define FRW_DLG_SCREEN_HANDLE                     0
#define FRW_DLG_DIALOG_HANDLE                     1
#define FRW_DLG_OK_ACTION_HANDLE                  2
#define FRW_DLG_CANCEL_ACTION_HANDLE              3

#define FRW_DLG_MAX_NR_OF_HANDLES                 4


/**********************************************************************
 * Type definitions
 **********************************************************************/
struct frw_dlg_st {
  int        type;
  int        choice_elem_cnt;
  int        response;
  WE_UINT32 handle[FRW_DLG_MAX_NR_OF_HANDLES]; /* handle must be 0 if not used*/
};


/**********************************************************************
 * Local functions
 **********************************************************************/

static frw_dlg_t *
frw_dlg_create 
(
    WE_UINT32 uiScreenHandle, 
    int iDialogType
);


static int
frw_dlg_create_objact_action_menu 
(
    frw_dlg_t *pstDlg,
    int iNumStrId,
    const WE_INT32 iStrIds[]
);


static int
frw_dlg_create_objact_no_action_info(frw_dlg_t *pstDlg);



/**********************************************************************
 * Exported functions
 **********************************************************************/

/*
 * Create a dialog.
 * Returns a pointer to a struct frw_dlg_st, or NULL on error.
 */
frw_dlg_t *
frw_dlgCreate 
(
    WE_UINT32 uiScreenHandle, 
    int iDialogType,
    int iNumStrings, 
    const char *pcStrings[],
    int iNumStrId, 
    const WE_INT32 iStrIds[]
)
{
  frw_dlg_t *pstDlg = NULL;

  /* to avoid warnings for params not used */
  iNumStrings = iNumStrings;
  pcStrings = pcStrings;

  switch (iDialogType) {
  case FRW_DLG_OBJACT_ACTION_MENU:
    if ((iNumStrings != 0) || (iNumStrId <= 0)) {
      return NULL;
    }
    pstDlg = frw_dlg_create (uiScreenHandle, iDialogType);
    frw_dlg_create_objact_action_menu (pstDlg, iNumStrId, iStrIds);
    break;
  case FRW_DLG_OBJACT_NO_ACTION_INFO:
    if ((iNumStrings != 0) || (iNumStrId != 0)) {
      return NULL;
    }
    pstDlg = frw_dlg_create (uiScreenHandle, iDialogType);
    frw_dlg_create_objact_no_action_info (pstDlg);
    break;
    
  default:
    break;
  }

  if (pstDlg != NULL) {
    WE_WIDGET_SET_IN_FOCUS (pstDlg->handle[FRW_DLG_SCREEN_HANDLE], 1);
    WE_WIDGET_SET_IN_FOCUS (pstDlg->handle[FRW_DLG_DIALOG_HANDLE], 1);
  }

  return pstDlg;
}

/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 */
void
frw_dlgDelete (frw_dlg_t *pstDlg)
{
  int i;

  /* NOTE: first the Dialog window must be removed from the Screen.
   * Then we release the Dialog window. This will cause all other
   * widgets that are attached to the Dialog to be removed from
   * the Dialog window.
   * Also, we must take care not to delete the Screen Handle! */
  for (i = FRW_DLG_MAX_NR_OF_HANDLES - 1; i > 1; i--) {
    if (pstDlg->handle[i] != 0) {
      WE_WIDGET_REMOVE (pstDlg->handle[FRW_DLG_DIALOG_HANDLE], pstDlg->handle[i]);
      WE_WIDGET_RELEASE (pstDlg->handle[i]);
    }
  }
  WE_WIDGET_REMOVE (pstDlg->handle[FRW_DLG_SCREEN_HANDLE],
                     pstDlg->handle[FRW_DLG_DIALOG_HANDLE]);
  WE_WIDGET_RELEASE (pstDlg->handle[FRW_DLG_DIALOG_HANDLE]);
  WE_MEM_FREE(WE_MODID_FRW, pstDlg);
}

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
frw_dlgAction 
(
    frw_dlg_t *pstDlg, 
    WE_UINT32 uiAction
)
{
  if (pstDlg != NULL) {
    if (pstDlg->handle[FRW_DLG_OK_ACTION_HANDLE] == uiAction) {
      pstDlg->response = FRW_DLG_RESPONSE_POSITIVE;
    }
    else {
      pstDlg->response = FRW_DLG_RESPONSE_CANCEL;
    }
  }
}

/*
 * Return the type of dialog that we have (one of the Dialog types
 * above).
 * Returns 0 on error.
 */
int
frw_dlgGetType (frw_dlg_t *pstDlg)
{
  if (pstDlg == NULL)
    return 0;

  return pstDlg->type;
}

/*
 * Return the Dialog Handle associated with the specified dialog.
 * Returns 0 on error. 
 */
WE_UINT32
frw_dlgGetDialogHandle (frw_dlg_t *pstDlg)
{
  if (pstDlg != NULL) {
    return pstDlg->handle[FRW_DLG_DIALOG_HANDLE];
  }

  return 0;
}

/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling frw_dlgAction).
 */
int
frw_dlgGetResponse (frw_dlg_t *pstDlg)
{
  if (pstDlg != NULL) {
    return pstDlg->response;
  }

  return 0;
}

/*
 * Return the selected index incase of a menu Item
 * Returns -1 if index not found.
 */
int
frw_dlgGetMenuIndex (frw_dlg_t *pstDlg)
{
  int             j;
  WeWindowHandle WindowHdl;

  if (pstDlg == NULL)
    return -1;

  WindowHdl = pstDlg->handle[FRW_DLG_DIALOG_HANDLE];

  for (j = 0; j < pstDlg->choice_elem_cnt; j++) {
    if (WE_WIDGET_CHOICE_GET_ELEM_STATE(WindowHdl, j) & 
        WE_CHOICE_ELEMENT_SELECTED) {
      return j;
    }
  }
  return -1;
}


/**********************************************************************
 * Local functions
 **********************************************************************/

static frw_dlg_t *
frw_dlg_create 
(
    WE_UINT32 uiScreenHandle, 
    int iDialogType
)
{
  int i;
  frw_dlg_t *pstDlg = WE_MEM_ALLOCTYPE (WE_MODID_FRW, frw_dlg_t);

  for (i = 0; i < FRW_DLG_MAX_NR_OF_HANDLES; i++)
    pstDlg->handle[i] = 0;
  pstDlg->type = iDialogType;
  pstDlg->choice_elem_cnt = 0;
  pstDlg->response = 0;
  pstDlg->handle[FRW_DLG_SCREEN_HANDLE] = uiScreenHandle;
 
  return pstDlg;
}


static int
frw_dlg_create_objact_action_menu 
(
    frw_dlg_t *pstDlg,
    int iNumStrId,
    const WE_INT32 iStrIds[]
)
{
  WeActionHandle SelectHdl;
  WeActionHandle CancelHdl;
  WeWindowHandle WindowHdl;
  int             i;
  WePosition     Position = {0,0};

  SelectHdl = WE_WIDGET_ACTION_CREATE(WE_MODID_FRW, 
                                        WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_OK),     
                                        0, WeSelect);
  CancelHdl = WE_WIDGET_ACTION_CREATE(WE_MODID_FRW, 
                                        WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_CANCEL), 
                                        0, WeCancel);
  WindowHdl = WE_WIDGET_MENU_CREATE(WE_MODID_FRW,
                                      WeImplicitChoice,
                                      0,
                                      0,
                                      WE_CHOICE_ELEMENT_STRING_1,
                                      0, 0, 0, 0);

  WE_WIDGET_SET_TITLE(WindowHdl, 0, 0);
  WE_WIDGET_WINDOW_SET_PROPERTIES(WindowHdl, WE_WINDOW_PROPERTY_TITLE, 0);

  WE_WIDGET_ADD_ACTION(WindowHdl, SelectHdl);
  WE_WIDGET_ADD_ACTION(WindowHdl, CancelHdl);

  for (i = 0; i < iNumStrId; i++)
  {
    WE_WIDGET_CHOICE_SET_ELEMENT (WindowHdl, i, 0, WE_WIDGET_STRING_GET_PREDEFINED(iStrIds[i]), 0, 0, 0, 0, 0, 0);
  }
  
  pstDlg->choice_elem_cnt = iNumStrId;
  pstDlg->handle[FRW_DLG_CANCEL_ACTION_HANDLE] = CancelHdl;
  pstDlg->handle[FRW_DLG_OK_ACTION_HANDLE] = SelectHdl;
  pstDlg->handle[FRW_DLG_DIALOG_HANDLE] = WindowHdl;

  WE_WIDGET_SCREEN_ADD_WINDOW (pstDlg->handle[FRW_DLG_SCREEN_HANDLE], WindowHdl, &Position);

  return 0;
}


static int
frw_dlg_create_objact_no_action_info(frw_dlg_t *pstDlg)
{
  WeActionHandle SelectHdl;
  WeWindowHandle WindowHdl;
  WePosition     Position = {0,0};

  SelectHdl = WE_WIDGET_ACTION_CREATE(WE_MODID_FRW, 
                                        WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_OK),     
                                        0, WeOk);

  WindowHdl = WE_WIDGET_DIALOG_CREATE (WE_MODID_FRW, 
                                         WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_DLG_MIME_ACTION_NONE),
                                         WeInfo,
                                         0);

  /* Presently we use no titel 
  WE_WIDGET_SET_TITLE(window_hdl, 0);
  WE_WIDGET_WINDOW_SET_PROPERTIES(window_hdl, WE_WINDOW_PROPERTY_TITLE);
  */

  WE_WIDGET_ADD_ACTION(WindowHdl, SelectHdl);

  pstDlg->handle[FRW_DLG_OK_ACTION_HANDLE] = SelectHdl;
  pstDlg->handle[FRW_DLG_DIALOG_HANDLE] = WindowHdl;

  WE_WIDGET_SCREEN_ADD_WINDOW (pstDlg->handle[FRW_DLG_SCREEN_HANDLE], WindowHdl, &Position);

  return 0;
}


#if 0

TO BE CONTINUED .......

Ingång från frw
static void
we_cmmn_display_action_menu (we_action_op_t *op)
{
  int i;
  WePosition position = {0,0};

  op->window = get_new_window (); /* see below*/

  op->screen = WE_WIDGET_SCREEN_CREATE (WE_MODID_FRW, 0, 0);

  for (i = 0; i < op->num_actions; i++)
  {
    WE_WIDGET_CHOICE_SET_ELEMENT (op->window->window_hdl, i, WE_WIDGET_STRING_GET_PREDEFINED(op->actions[i].string_id),  0, 0, 0, TRUE);
  }
  
  WE_WIDGET_SCREEN_ADD_WINDOW (op->screen, op->window->window_hdl, &position, 0, 0 );

  /*Set window in focus*/
  WE_WIDGET_SET_IN_FOCUS(op->screen, 1);
  WE_WIDGET_SET_IN_FOCUS(op->window->window_hdl, 1);

}

static we_action_window_t *
get_new_window (void)
{
  we_action_window_t *window = WE_MEM_ALLOCTYPE (WE_MODID_FRW, we_action_window_t);


  window->select_hdl = WE_WIDGET_ACTION_CREATE(WE_MODID_FRW, 
                                                WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_OK),     
                                                0, WeSelect);
  window->cancel_hdl = WE_WIDGET_ACTION_CREATE(WE_MODID_FRW, 
                                                WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_CANCEL), 
                                                0, WeCancel);

  window->window_hdl = WE_WIDGET_MENU_APP_CREATE(WE_MODID_FRW,
                                             window->select_hdl,
                                             0, /*Element Position*/
                                             WE_CHOICE_ELEMENT_STRING_1, /*Bit Mask*/
                                             0x8000, /*Property Mask*/
                                             0 /*Default Style*/);

  WE_WIDGET_SET_TITLE(window->window_hdl,0);
  WE_WIDGET_WINDOW_SET_PROPERTIES(window->window_hdl, WE_WINDOW_PROPERTY_TITLE);

  WE_WIDGET_ADD_ACTION(window->window_hdl, window->cancel_hdl);

  return window;
}

Ingång från FRW
static void
we_cmmn_display_no_action_dialog (we_action_op_t *op)
{
  WePosition position = {0,0};

  op->window = get_new_info_dialog ();

  op->screen = WE_WIDGET_SCREEN_CREATE (WE_MODID_FRW, 0, 0);

  WE_WIDGET_SCREEN_ADD_WINDOW (op->screen, op->window->window_hdl, &position, 0, 0 );

  /*Set window in focus*/
  WE_WIDGET_SET_IN_FOCUS(op->screen, 1);
  WE_WIDGET_SET_IN_FOCUS(op->window->window_hdl, 1);
}

static we_action_window_t *
get_new_info_dialog (void)
{
  we_action_window_t *window = WE_MEM_ALLOCTYPE (WE_MODID_FRW, we_action_window_t);


  window->select_hdl = WE_WIDGET_ACTION_CREATE(WE_MODID_FRW, 
                                                WE_WIDGET_STRING_GET_PREDEFINED (WE_STR_ID_DLG_ACTION_OK),     
                                                0, WeOk);
  window->cancel_hdl = 0;

  window->window_hdl = WE_WIDGET_DIALOG_CREATE (WE_MODID_FRW, 
                                                 WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_DLG_MIME_ACTION_NONE),
                                                 WeInfo,
                                                 0, 0, 0);

  /* Presently we use no titel 
  WE_WIDGET_SET_TITLE(window->window_hdl, 0);
  WE_WIDGET_WINDOW_SET_PROPERTIES(window->window_hdl, WE_WINDOW_PROPERTY_TITLE);
  */

  WE_WIDGET_ADD_ACTION(window->window_hdl, window->select_hdl);

  return window;
}



Från CMS

static int
frw_dlg_create_error_message_dialog (frw_dlg_t *dlg, WeStringHandle msg_handle)
{
  WE_UINT32      dialog_handle;
  WePosition     null_pos = {0, 0};
  WeStringHandle ok_string;
  WeActionHandle ok_action;
  
  /* Create Dialog */
  dialog_handle = WE_WIDGET_DIALOG_CREATE (WE_MODID_CMS, msg_handle, WeAlert,
                                            0, 0, 0);
  /* Action OK */
  ok_string = WE_WIDGET_STRING_GET_PREDEFINED(CMS_STR_ID_OK);
  ok_action = WE_WIDGET_ACTION_CREATE (WE_MODID_CMS, ok_string, 0, WeOk);
  WE_WIDGET_ADD_ACTION (dialog_handle, ok_action);
  dlg->handle[FRW_DLG_OK_STRING_HANDLE] = ok_string;
  dlg->handle[FRW_DLG_OK_ACTION_HANDLE] = ok_action;

  dlg->handle[FRW_DLG_MESSAGE_HANDLE] = msg_handle;
  dlg->handle[FRW_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[FRW_DLG_SCREEN_HANDLE],
                                dialog_handle, &null_pos, 0, 0);

  return 0;
}


static int
frw_dlg_create_menu_dialog(frw_dlg_t *dlg, WE_UINT16 numEntries, 
                           const char *midletArray[], int title_cms_str_id)
{
  int i;
  WeWindowHandle dialog_handle;
  WeActionHandle ok_action = 0;
  WeActionHandle cancel_action = 0;
  WeDeviceProperties prop;
  WePosition menuPosition = { 0, 0};

  WE_WIDGET_DEVICE_GET_PROPERTIES(&prop);

  dialog_handle = WE_WIDGET_MENU_CREATE(WE_MODID_CMS,
                                         WeImplicitChoice,
                                         &prop.displaySize,
                                         ok_action,
                                         0, /*Element Position*/
                                         WE_CHOICE_ELEMENT_STRING_1, /*Bit Mask*/
                                         0x8000, /*Property Mask*/
                                         0 /*Default Style*/
                                         );

  WE_WIDGET_SET_TITLE (dialog_handle, WE_WIDGET_STRING_GET_PREDEFINED(title_cms_str_id));
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, WE_WINDOW_PROPERTY_TITLE);
  
  /* Create an OK action. */
  ok_action = WE_WIDGET_ACTION_CREATE (WE_MODID_CMS,
                           WE_WIDGET_STRING_GET_PREDEFINED(CMS_STR_ID_OK), 0, WeOk);
  WE_WIDGET_ADD_ACTION (dialog_handle, ok_action);
  
  /* Create a Cancel action.*/
  cancel_action = WE_WIDGET_ACTION_CREATE (WE_MODID_CMS,
                             WE_WIDGET_STRING_GET_PREDEFINED(CMS_STR_ID_CANCEL), 0, WeCancel);
  WE_WIDGET_ADD_ACTION (dialog_handle, cancel_action);

  for (i = 0; i < numEntries ; i++)
    WE_WIDGET_CHOICE_SET_ELEMENT (dialog_handle, i,
        WE_WIDGET_STRING_CREATE(WE_MODID_CMS, midletArray[i], WeUtf8, strlen(midletArray[i])+1,0) , 0, 0, 0, TRUE);

  dlg->handle[FRW_DLG_CANCEL_ACTION_HANDLE] = cancel_action;
  dlg->handle[FRW_DLG_OK_ACTION_HANDLE] = ok_action;
  dlg->handle[FRW_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[FRW_DLG_SCREEN_HANDLE], dialog_handle,
                                &menuPosition, 0, 0);
  
  return 0;
}

#endif

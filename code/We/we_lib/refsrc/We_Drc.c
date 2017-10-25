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
 * We_Drc.c
 *
 * Created by Robert Moricz, Fri Oct 24 2003.
 *
 * Revision  history:
 *
 */

#include "We_Drc.h"
#include "We_Rc.h"

#include "We_Def.h"
#include "We_Int.h"
#include "We_Mem.h"
#include "We_Wid.h"


/**********************************************************************
 * Constants
 **********************************************************************/

/* Indices into the array of handles */
#define WE_DRC_SCREEN_HANDLE                     0
#define WE_DRC_DIALOG_HANDLE                     1
#define WE_DRC_OK_ACTION_HANDLE                  2
#define WE_DRC_CANCEL_ACTION_HANDLE              3

#define WE_DRC_MAX_NR_OF_HANDLES                 4


#define WE_DRC_OBJACT_SUBMENU_POSITION     {10,10} /* Position in pixels */
#define WE_DRC_OBJACT_SUBMENU_SIZE_HEIGHT  0.9 /* Size proportional to screen size */
#define WE_DRC_OBJACT_SUBMENU_SIZE_WIDTH   0.9 /* Size proportional to screen size */


/**********************************************************************
 * Type definitions
 **********************************************************************/
struct we_drc_st {
  WE_UINT8  client_modid;
  int        type;
  int        choice_elem_cnt;
  int        response;
  WE_UINT32 handle[WE_DRC_MAX_NR_OF_HANDLES]; /* handle must be 0 if not used*/
};


/**********************************************************************
 * Local functions
 **********************************************************************/

static we_drc_t *
we_drc_create (WE_UINT8 client_modid, WE_UINT32 screen_handle,
                int dialog_type);


static int
we_drc_create_objact_subaction_menu (we_drc_t *dlg,
                                      int numStrId,
                                      const WE_INT32 str_ids[]);


/**********************************************************************
 * Exported functions
 **********************************************************************/

/*
 * Create a dialog.
 * Returns a pointer to a struct we_drc_st, or NULL on error.
 */
we_drc_t *
we_drcCreate (WE_UINT8 client_modid, WE_UINT32 screenHandle,
               int dialogType, int numStrings, const char *strings[],
               int numStrId, const WE_INT32 str_ids[])
{
  we_drc_t *dlg = NULL;

  /* to avoid warnings for params not used */
  numStrings = numStrings;
  strings = strings;

  switch (dialogType) {
  case WE_DRC_OBJACT_SUBACTION_MENU:
    if ((numStrings != 0) || (numStrId <= 0)) {
      return NULL;
    }
    dlg = we_drc_create (client_modid, screenHandle, dialogType);
    we_drc_create_objact_subaction_menu (dlg, numStrId, str_ids);
    break;
    
  default:
    break;
  }

  if (dlg != NULL) {
    WE_WIDGET_SET_IN_FOCUS (dlg->handle[WE_DRC_SCREEN_HANDLE], 1);
    WE_WIDGET_SET_IN_FOCUS (dlg->handle[WE_DRC_DIALOG_HANDLE], 1);
  }

  return dlg;
}

/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 */
void
we_drcDelete (we_drc_t *dlg)
{
  int i;

  /* NOTE: first the Dialog window must be removed from the Screen.
   * Then we release the Dialog window. This will cause all other
   * widgets that are attached to the Dialog to be removed from
   * the Dialog window.
   * Also, we must take care not to delete the Screen Handle! */
  for (i = WE_DRC_MAX_NR_OF_HANDLES - 1; i > 1; i--) {
    if (dlg->handle[i] != 0) {
      WE_WIDGET_REMOVE (dlg->handle[WE_DRC_DIALOG_HANDLE], dlg->handle[i]);
      WE_WIDGET_RELEASE (dlg->handle[i]);
    }
  }
  WE_WIDGET_REMOVE (dlg->handle[WE_DRC_SCREEN_HANDLE],
                     dlg->handle[WE_DRC_DIALOG_HANDLE]);
  WE_WIDGET_RELEASE (dlg->handle[WE_DRC_DIALOG_HANDLE]);
  WE_MEM_FREE(dlg->client_modid, dlg);
}

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
we_drcAction (we_drc_t *dlg, WE_UINT32 action)
{
  if (dlg != NULL) {
    if (dlg->handle[WE_DRC_OK_ACTION_HANDLE] == action) {
      dlg->response = WE_DRC_RESPONSE_POSITIVE;
    }
    else {
      dlg->response = WE_DRC_RESPONSE_CANCEL;
    }
  }
}

/*
 * Return the type of dialog that we have (one of the Dialog types
 * above).
 * Returns 0 on error.
 */
int
we_drcGetType (we_drc_t *dlg)
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
we_drcGetDialogHandle (we_drc_t *dlg)
{
  if (dlg != NULL) {
    return dlg->handle[WE_DRC_DIALOG_HANDLE];
  }

  return 0;
}

/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling we_drcAction).
 */
int
we_drcGetResponse (we_drc_t *dlg)
{
  if (dlg != NULL) {
    return dlg->response;
  }

  return 0;
}

/*
 * Return the selected index incase of a menu Item
 * Returns -1 if index not found.
 */
int
we_drcGetMenuIndex (we_drc_t *dlg)
{
  int j;
  WeWindowHandle window_hdl;

  if (dlg == NULL)
    return -1;

  window_hdl = dlg->handle[WE_DRC_DIALOG_HANDLE];

  for (j = 0; j < dlg->choice_elem_cnt; j++) {
    if (WE_WIDGET_CHOICE_GET_ELEM_STATE(window_hdl, j) &
        WE_CHOICE_ELEMENT_FOCUSED) {
      return j;
    }
  }
  return -1;
}


/**********************************************************************
 * Local functions
 **********************************************************************/

static we_drc_t *
we_drc_create (WE_UINT8 client_modid, WE_UINT32 screen_handle,
                int dialog_type)
{
  int i;
  we_drc_t *dlg = WE_MEM_ALLOCTYPE (client_modid, we_drc_t);

  for (i = 0; i < WE_DRC_MAX_NR_OF_HANDLES; i++)
    dlg->handle[i] = 0;
  dlg->client_modid = client_modid;
  dlg->type = dialog_type;
  dlg->choice_elem_cnt = 0;
  dlg->response = 0;
  dlg->handle[WE_DRC_SCREEN_HANDLE] = screen_handle;
 
  return dlg;
}


static int
we_drc_create_objact_subaction_menu (we_drc_t *dlg,
                                      int numStrId,
                                      const WE_INT32 str_ids[])
{
  WeDisplayProperties  prop;
  WePosition          windowPosition = WE_DRC_OBJACT_SUBMENU_POSITION;
  WeActionHandle      select_hdl;
  WeActionHandle      cancel_hdl;
  WeWindowHandle      window_hdl;
  int                  i;

  WE_WIDGET_DISPLAY_GET_PROPERTIES(&prop);

  prop.displaySize.height = (short) (prop.displaySize.height * WE_DRC_OBJACT_SUBMENU_SIZE_HEIGHT); 
  prop.displaySize.width = (short) (prop.displaySize.width * WE_DRC_OBJACT_SUBMENU_SIZE_WIDTH);
  
  window_hdl =  WE_WIDGET_MENU_CREATE(dlg->client_modid,
                                       WeExclusiveChoice,
                                       &prop.displaySize,
                                       0, /*Element Position*/
                                       WE_CHOICE_ELEMENT_STRING_1, /*Bit Mask*/
                                       0,/* parent menu handle */
                                       0,/* element index in parent menu */
                                       0x8000, /*Property Mask*/
                                       0 /*Default Style*/
                                       );

  WE_WIDGET_WINDOW_SET_PROPERTIES (window_hdl,0, 0);

  for (i = 0; i < numStrId; i++)
  {
    WE_WIDGET_CHOICE_SET_ELEMENT (window_hdl, i, 0, WE_WIDGET_STRING_GET_PREDEFINED(str_ids[i]), 0, 0, 0, 0, 0, 0);
  }

  cancel_hdl = WE_WIDGET_ACTION_CREATE (dlg->client_modid,
                                         WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_SAS_ACTION_CANCEL), 0, WeCancel );
  WE_WIDGET_ADD_ACTION (window_hdl, cancel_hdl);

  select_hdl = WE_WIDGET_ACTION_CREATE (dlg->client_modid,
                                         WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_DLG_ACTION_OK), 0, WeOk );
  WE_WIDGET_ADD_ACTION (window_hdl, select_hdl);

  dlg->choice_elem_cnt = numStrId;
  dlg->handle[WE_DRC_CANCEL_ACTION_HANDLE] = cancel_hdl;
  dlg->handle[WE_DRC_OK_ACTION_HANDLE] = select_hdl;
  dlg->handle[WE_DRC_DIALOG_HANDLE] = window_hdl;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DRC_SCREEN_HANDLE], window_hdl,
                                &windowPosition );
  return 0;
}

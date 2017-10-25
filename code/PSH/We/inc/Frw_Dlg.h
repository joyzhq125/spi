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
 * Frw_Dlg.h
 *
 * Created by Robert Moricz, Fri Oct 24 2003.
 *
 * Revision  history:
 *   031024, rmz: Created
 *
 */
#ifndef _frw_dlg_h
#define _frw_dlg_h

#ifndef  _we_def_h
#include "We_Def.h"
#endif

/**********************************************************************
 * Constants
 **********************************************************************/

/* Object action (objact) types of menues & dialogues */
#define FRW_DLG_OBJACT_ACTION_MENU                1
#define FRW_DLG_OBJACT_NO_ACTION_INFO             2

/* Response types */
#define FRW_DLG_RESPONSE_POSITIVE                 1
#define FRW_DLG_RESPONSE_NEGATIVE                 2
#define FRW_DLG_RESPONSE_CANCEL                   3


/**********************************************************************
 * Parameters for different dialog types
 **********************************************************************/

/*
 * The call to we_dlgCreate must have exactly this many elements
 * in the 'strings' array and str_ids array, for each of the Dialog
 * types below:
 *
 * FRW_DLG_OBJACT_NO_ACTION_INFO :
 *   0 string parameters & str_id parameters 
 *
 * FRW_DLG_OBJACT_ACTION_MENU:
 *   0 string parameter and n str_id parameters (n > 0): 
 *
 */

/**********************************************************************
 * Type definitions
 **********************************************************************/

typedef struct frw_dlg_st frw_dlg_t;


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
);


/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 */
void
frw_dlgDelete (frw_dlg_t *pstDlg);

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
frw_dlgAction 
(
    frw_dlg_t *pstDlg, 
    WE_UINT32 uiAction
);

/*
 * Return the type of dialog that we have (one of the Dialog types
 * above).
 * Returns 0 on error.
 */
int
frw_dlgGetType (frw_dlg_t *pstDlg);

/*
 * Return the Dialog Handle associated with the specified dialog.
 * Returns 0 on error. 
 */
WE_UINT32
frw_dlgGetDialogHandle (frw_dlg_t *pstDlg);


/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling frw_dlgAction).
 */
int
frw_dlgGetResponse (frw_dlg_t *pstDlg);

/*
 * Return the selected index incase of a menu Item
 * Returns -1 if index not found.
 */
int
frw_dlgGetMenuIndex (frw_dlg_t *pstDlg);


#endif

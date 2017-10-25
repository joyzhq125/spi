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
 * Pus_Dlg.h
 *
 * Created by Kent Olsson, Wed Nov 21 18:23:35 2002.
 *
 * Revision history:
 *
 */

#ifndef _Pus_Dlg_h
#define _Pus_Dlg_h


#ifndef  _we_def_h
#include "we_def.h"
#endif


/**********************************************************************
 * Constants
 **********************************************************************/

/* Types of dialogues */
#define PUS_DLG_SIA_CONFIRM                       1
#define PUS_DLG_WHITELIST_NO_MATCH                2

/* Response types */
#define PUS_DLG_RESPONSE_POSITIVE                 1
#define PUS_DLG_RESPONSE_NEGATIVE                 2
#define PUS_DLG_RESPONSE_CANCEL                   3

/**********************************************************************
 * Parameters for different dialog types
 **********************************************************************/

/*
 * The call to Pus_DlgCreate must have exactly this many elements
 * in the 'strings' array, for each of the Dialog types below:
 *
 * PUS_DLG_SIA_CONFIRM: 3 string parameters
 *   0: the message to display to the user
 *   1: string to display on the Ok button (or empty string/NULL for default)
 *   2: string to display on the Cancel button (or empty string/NULL for default)
 *
 */

/**********************************************************************
 * Type definitions
 **********************************************************************/
struct Pus_Dlg_st {
  int        type;
  int        response;
  int        num_handles;
  WE_UINT32 handle[8];
};

typedef struct Pus_Dlg_st Pus_Dlg_t;

/**********************************************************************
 * Exported functions
 **********************************************************************/
void
Pus_Dlg_init (void);

void
Pus_Dlg_terminate (void);

/*
 * Create a dialog.
 * Returns a pointer to a struct Pus_Dlg_st, or NULL on error.
 */
Pus_Dlg_t *
Pus_DlgCreate (int dialogType, int numStrings, WE_UINT32 strings[], char* info);

/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 * If moreDialogs = TRUE, teh screen should be removed and released
 */
void
Pus_DlgDelete (Pus_Dlg_t *dlg, int moreDialogs);

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
Pus_DlgAction (Pus_Dlg_t *dlg, WE_UINT32 action);

/*
 * Return the type of dialog that we have (one of the Dialog types
 * above).
 * Returns 0 on error.
 */
int
Pus_DlgGetType (Pus_Dlg_t *dlg);

/*
 * Return the Dialog Handle associated with the specified dialog.
 * Returns 0 on error. 
 */
WE_UINT32
Pus_DlgGetDialogHandle (Pus_Dlg_t *dlg);

/*
 * Return the input string from one of the input fields of
 * this dialog. The index of the requested input field is given
 * as 'idx', with indices starting at 0.
 * Returns NULL on error.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
Pus_DlgGetInput (Pus_Dlg_t *dlg, int idx);

/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling Pus_DlgAction).
 */
int
Pus_DlgGetResponse (Pus_Dlg_t *dlg);

#endif

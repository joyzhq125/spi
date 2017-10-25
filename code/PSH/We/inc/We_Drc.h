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
 * We_Drc.h (we dialog resources)
 *
 * Created by Robert Moricz, Fri Oct 24 2003.
 *
 * Revision  history:
 *   031024, rmz: Created
 *
 */
#ifndef _we_drc_h
#define _we_drc_h

#ifndef  _we_def_h
#include "We_Def.h"
#endif

/**********************************************************************
 * Constants
 **********************************************************************/

/* Object action (objact) types of menues & dialogues */
#define WE_DRC_OBJACT_SUBACTION_MENU             1

/* Response types */
#define WE_DRC_RESPONSE_POSITIVE                 1
#define WE_DRC_RESPONSE_NEGATIVE                 2
#define WE_DRC_RESPONSE_CANCEL                   3


/**********************************************************************
 * Parameters for different dialog types
 **********************************************************************/

/*
 * The call to we_dlgCreate must have exactly this many elements
 * in the 'strings' array and str_ids array, for each of the Dialog
 * types below:
 *
 * WE_DRC_OBJACT_SUBACTION_MENU:
 *   0 string parameter and n str_id parameters (n > 0): 
 * 
 */

/**********************************************************************
 * Type definitions
 **********************************************************************/

typedef struct we_drc_st we_drc_t;


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
               int numStrId, const WE_INT32 str_ids[]);

/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 */
void
we_drcDelete (we_drc_t *dlg);

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
we_drcAction (we_drc_t *dlg, WE_UINT32 action);

/*
 * Return the type of dialog that we have (one of the Dialog types
 * above).
 * Returns 0 on error.
 */
int
we_drcGetType (we_drc_t *dlg);

/*
 * Return the Dialog Handle associated with the specified dialog.
 * Returns 0 on error. 
 */
WE_UINT32
we_drcGetDialogHandle (we_drc_t *dlg);


/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling we_drcAction).
 */
int
we_drcGetResponse (we_drc_t *dlg);

/*
 * Return the selected index incase of a menu Item
 * Returns -1 if index not found.
 */
int
we_drcGetMenuIndex (we_drc_t *dlg);


#endif

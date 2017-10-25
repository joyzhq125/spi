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
 * uba_rfld.h
 * 
 * Description:
 *    This file contains code to handle renaming of folders
 *
 * Created by Martin Andersson, 2003/09/16.
 *
 * Revision history:
 *
 */
#ifndef UBA_RFLD_H
#define UBA_RFLD_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef _we_int_h
  #include "We_Int.h"
#endif

#ifndef _we_core_h
  #include "We_Core.h"
#endif

#ifndef UBA_DATA_H
  #include "Uba_Data.h"
#endif

#ifndef UBA_CUST_H
  #include "Uba_Cust.h"
#endif

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

/* Init the custom page, data must point to a folder object
 * return TRUE if successful 
 */
int
uba_rename_folder_view_init (uba_custom_page_t *p, 
                    uba_data_obj_t    *data,
                    WeScreenHandle    screen,
                    int                i);

/*
 *	Releases the allocated resources and widgets of p
 */
void 
uba_rename_folder_view_release(uba_custom_page_t *p);

/*
 *	Offer the signal to this custom page, return TRUE if sig used
 */
int 
uba_rename_folder_view_handle_sig (uba_custom_page_t   *p, 
                          uba_signal_t                *sig);

/*
 *	Some data has changed
 */
void 
uba_rename_folder_view_handle_data_event (uba_custom_page_t    *p, 
                                 const uba_data_obj_t *data,
                                 uba_data_event_t      event);

/*
 *	View this custom page
 */
void 
uba_rename_folder_view_show (uba_custom_page_t* p);


#endif      /*UBA_RFLD_H*/

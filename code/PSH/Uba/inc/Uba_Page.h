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
 * uba_page.h
 *
 * Created by Martin Andersson, 2003/08/22.
 *
 *
 * Handles on gui page (widget menu) as defined in uba_cfg.h
 * The page is connected to an object data
 *
 * Revision history:
 *
 */

#ifndef UBA_PAGE_H
#define UBA_PAGE_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_RCE_H
  #include "Uba_Rce.h"
#endif

#ifndef UBA_TEXT_H
  #include "Uba_Text.h"
#endif

#ifndef _we_int_h
  #include "We_Int.h"
#endif

#ifndef UBA_DATA_H
  #include "Uba_Data.h"
#endif

#ifndef UBA_PITM_H
  #include "Uba_Pitm.h"
#endif

/************************************************************************/
/* Type declarations                                                    */
/************************************************************************/

/* Defines the data for one page */
typedef struct uba_page_st{
  WeWindowHandle       window;            /* Handle to the page (type =widget menu) */
  const uba_rce_page_t *rc;                /* Resource struct from uba_config.c      */
  uba_page_item_t      *items;             /* Array of page items that mapps into    */
                                           /* the widget choice array                */
  uba_data_obj_t        data;              /* The data this page works on            */
  uba_text_t            title;             /* The dynamic title                      */
  uba_text_t            default_ok_label;  /* Text for the OK label                  */
  WeActionHandle       select;            /* widget handle for select button        */
  WeActionHandle       back;              /* widget handle for ok back              */
  int                   is_up_and_running; /* true if page is alive                  */
  WeScreenHandle       screen;  
} uba_page_t;

/************************************************************************/
/* Exported functions                                                   */
/**************o**********************************************************/

/*
 *	Init the struct p with the data object data and the resource pointer rc.
 *  This will create the GUI representation of rc and data
 */
int
uba_page_init (uba_page_t* p, uba_data_obj_t* data, WeScreenHandle screen,
               const uba_rce_page_t* rc);

/*
 *	Releases the resources allocated by p 
 */
void
uba_page_release (uba_page_t* p);


/*
 *	Set focus to p 
 */
void 
uba_page_show (const uba_page_t* p);

/*
 *	Called from uba_rce to notify widget actions
 */
void
uba_page_handle_widget_action (uba_page_t* p, we_widget_action_t* action);

/*
 *	Called for uba_rce to notify that the user presed menu key 
 */
void
uba_page_handle_menu_action (uba_page_t* p);

/*
 *	Called from rce to notify that the data data has changed.
 *  event = the type of change
 */
void
uba_page_handle_data_event (uba_page_t *p, const uba_data_obj_t *data, 
                            uba_data_event_t event);

/*
 *	Performs delete action menu item for all page items
 */
void
uba_page_do_delete_all (uba_page_t *p);

#endif      /*UBA_PAGE_H*/

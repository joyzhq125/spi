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
 * uba_menu.h
 *
 * Created by Klas Hermodsson, 2003/08/22.
 *
 * Defines functions and struct to handle a menu.
 * Create from the definitions in uba_cfg.h and uba_rce.h a wideget menu
 *
 *
 * Revision history:
 *
 */

#ifndef UBA_MENU_H
#define UBA_MENU_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_DATA_H
  #include "Uba_Data.h"
#endif

#ifndef UBA_RCE_H
  #include "Uba_Rce.h"
#endif

#ifndef _we_int_h
  #include "We_Int.h"
#endif

/************************************************************************/
/* Type declarations                                                    */
/************************************************************************/

/* Data struct */
typedef struct uba_menu_st{
  WeWindowHandle       window; /* The menu window */
  uba_data_obj_t        data;   /* The data object connected to this widget menu */
  const uba_rce_menu_t  *rc;    /* The resource that defines this menu */
  WeActionHandle       select; /* Action for select */
  WeActionHandle       back;   /* Action handle for back */
  WeScreenHandle       screen;
} uba_menu_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 * Allocates resources for this menu
 */
int
uba_menu_init (uba_menu_t* p, uba_data_obj_t* data, WeScreenHandle screen, 
               const uba_rce_menu_t* rc);

/*
 *	Release all allocated resources
 */
void
uba_menu_release (uba_menu_t* p);


/*
 *	Show p (must be attached)
 */
void 
uba_menu_show (uba_menu_t* p);

/*
 *	Called form RCE to notify widget actions
 */
void
uba_menu_handle_widget_action (uba_menu_t* p, we_widget_action_t* action);

/*
 *	Returns the resource for the default action or NULL if none
 */
const uba_rce_action_t*
uba_menu_get_default_action (const uba_rce_menu_t* rc);

/*
 *	Call this to notify the menu (p) that the data (data) has been 
 *  change/added/deleted (event)
 */
void
uba_menu_handle_data_event (uba_menu_t *p, const uba_data_obj_t *data, 
                            uba_data_event_t event);

#endif      /*UBA_MENU_H*/

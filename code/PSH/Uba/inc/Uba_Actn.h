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
 * uba_actn.h
 *
 * Created by Martin Andersson, 2003/08/28.
 *
 * Revision history:
 *
 */
/*
 *	This header file describes generic functions that an menu item can perform
 *  All functions have the same format as defined in the uba_rce_action_t typedef
 *  
 */

#ifndef UBA_ACTN_H
#define UBA_ACTN_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_RCE_H
  #include "Uba_Rce.h"
#endif

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *	View the page with page wid i and data object p
 */
void
uba_action_view_page (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *	View the menu with menu wid i and data object p
 */
void
uba_action_view_menu (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *	View a custom page with the custom page index i and the data object p, i2
 *  is sent to the custom pages init method.
 */
void
uba_action_view_custom_page(uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *	Send content routing the const string str to module wid i
 */
void
uba_action_static_content_routing (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *	Sends a content routing to module wid i with
 *  sprintfs str together with the hex version of a msg's handle
 *  NOTE that p must be of the type UBA_DATA_OBJECT_MSG!
 *
 *  Example: 
 *   i = WE_MODID_MSA
 *   str "-mode view -m %s"
 *
 *  result: content routing to MSA with the string "-mode view -m XXXXXXXX"
 *  where XXXXXXXX is the handle of the UBA_DATA_OBJECT_MSG in p in a
 *  hexadecimal string form.
 */
void
uba_action_content_routing_with_handle_as_hex_string (uba_data_obj_t* p, 
                                                      int i, int i2, const char* str);

/*	Sends a content routing to module wid i with
 *  sprintfs str together with the property key i2 (must be have a UTF8 property value)
 *  NOTE that p must be of the type UBA_DATA_OBJECT_MSG!
 *
 *  Example: 
 *   i = WE_MODID_MSA
 *   i2= UBS_MSG_KEY_FROM
 *   str "-mode new -to %s"
 *
 */
void
uba_action_content_routing_with_UTF8_property (uba_data_obj_t* p, 
                                                      int i, int i2, const char* str);

/*
 *	Sends content routing to module i with contentUrl taken from the data object.
 *  This will only work on data object of the type UBA_DATA_OBJECT_MSG and with
 *  the (valid) property UBS_MSG_KEY_URL
 */
void
uba_action_content_routing_with_URL (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *	Call voice mail using phone number stored in the registry. Delete message.
 *  NOTE that p must be of the type UBA_DATA_OBJECT_MSG!
 */
void
uba_action_call_del_voice_mail (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *  Calls UBSif_deleteMsg on the msg indicated by p.
 */
void
uba_action_ubs_delete_msg (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *  Calls UBSif_chnageMsg on the msg indicated by p by setting its property key i
 *  to str.
 */
void
uba_action_ubs_change_UTF8_property (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *  Performs the delete action for all page items on the top page.
 */
void
uba_action_top_page_delete_all (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *  Performs a sorting of the folder pointed to by p. The sort order is in i.
 */
void
uba_action_sort_folder (uba_data_obj_t* p, int i, int i2, const char* str);

/*
 *  Starts the module with the module wid i.
 */
void
uba_action_start_module (uba_data_obj_t* p, int i, int i2, const char* str);

#endif      /*UBA_ACTN_H*/

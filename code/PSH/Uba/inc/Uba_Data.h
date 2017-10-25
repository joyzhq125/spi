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
 * uba_data.h
 * 
 * This is the main file of the data part of the UBA module.
 * 
 * A major role of this file is to deal with communication with the UBS module.
 * All incoming signals from the UBS module are handled by this file.
 * 
 * This file maintains a list of all the messages as listed from the UBS module.
 * By using UBA_FOLDER_MAPPING in uba_cfg.h all messages are divided up into folders 
 * (see uba_fold.h).
 * 
 *
 * Created by Klas Hermodsson, 2003/08/20.
 *
 * Revision history:
 * 2004-02-17 (KOGR): uba_data_get_voice_mail_num declared
 *
 */

#ifndef UBA_DATA_H
#define UBA_DATA_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_CFG_H
  #include "Uba_Cfg.h"
#endif

#ifndef UBS_IF_H
  #include "Ubs_If.h"
#endif

#ifndef UBA_FOLD_H
  #include "Uba_Fold.h"
#endif

#ifndef _we_core_h
  #include "We_Core.h"
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/*
 *  An enumeration of data object events. These events are reported to 
 *  uba_rce_on_data_event so that the GUI can react to changes in the data.
 */
typedef enum{
  UBA_DATA_EVENT_NEW,
  UBA_DATA_EVENT_CHANGE,
  UBA_DATA_EVENT_DELETE,
  UBA_DATA_EVENT_ITEM_ADDED,
  UBA_DATA_EVENT_ITEM_DELETED
}uba_data_event_t;

/*
 *  An enumeration of data object types.
 */
typedef enum{
  UBA_DATA_OBJECT_STATIC, /* Static data object msans no data at all */
  UBA_DATA_OBJECT_FOLDER, /* Tells that the data object is in the data.folder */
  UBA_DATA_OBJECT_MSG     /* Tells that the data object is in the data.folder_item */
}uba_data_object_type_t;

/*
 *  The data object typedef.
 *  The type attribute tells which part of the data union is set.
 */
typedef struct uba_data_obj_st{
  uba_data_object_type_t type;
  union {
    uba_folder_t*       folder;
    uba_folder_item_t*  folder_item;
  }data;
}uba_data_obj_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *  Initiates the data part of the UBA.
 *  Registration of needed message types (UBA_HANDLED_MSG_TYPES, see uba_cfg.h) 
 *  towards UBS is started.
 */
void
uba_data_init (void);

/*
 *  Termination of the data part of the UBA.
 *  Deregistration of registered message types towards UBS is taken care of.
 */
void
uba_data_terminate (void);

/*
 *	Retrieves the folder data object from its folder index (see uba_cfg.h).
 */
uba_folder_t*
uba_data_get_folder (uba_folder_id_t folder_id);

/*
 *  Checks to see if the the data objects are equal. a and b are equal if
 *  they are of the same type and they point to the same folder/folder_iten.
 *  If a equals b then this function returns TRUE, else FALSE.
 */
int
uba_data_object_is_equal (const uba_data_obj_t *a, const uba_data_obj_t *b);

/*
 *  This utility functions finds a property in a message.
 *  Returns the property or NULL if it was not found.
 */
ubs_key_value_t*
uba_data_util_lookup_msg_property (ubs_msg_list_item_t *msg, int property_key);

/*
 *	Compares two key values 
 *
 *  Return Value Description 
 *  < 0  a less than  b 
 *  0    a identical to  b
 *  > 0  a greater than b
 *
 *  Strings less msans that "a" is lesser than "b"
 *  Bool    less msans that false is lesser than true
 *  int     less msans that  20 is lesser than 30
 *  raw data result from memcmp and other value types
 * 
 *  If not the same types the return value is a.type - b.type
 */
int
uba_data_util_compare_key_value (const ubs_key_value_t *a, const ubs_key_value_t *b);

WE_INT32
uba_data_util_get_intval(const ubs_key_value_t *p);

WE_UINT32
uba_data_util_get_uintval(const ubs_key_value_t *p);

WE_BOOL
uba_data_util_get_boolval(const ubs_key_value_t *p);

/*
 *	Retrieve voice mail number
 */
const char*
uba_data_get_voice_mail_num (void);

/*
 *  Set voice mail number
 *  If save_to_registry == TRUE: store in the registry
 */
void
uba_data_set_voice_mail_num (const char* numStr, int save_to_registry);

/************************************************************************/
/* Message handlers (Exported functions)                                */
/************************************************************************/

/*
 *	Signal handlers, called only from uba_main
 */

/*
 *  Message handler for the WE_SIG_REG_RESPONSE signal.
 */
void
uba_handle_data_msg_regResponse (int src_module, 
                                 unsigned org_sig, 
                                 we_registry_response_t* p);

/*
 *  Message handler for the UBS_SIG_REPLY_REGISTER signal.
 */
void
uba_data_handle_msg_reply_register(int src_module, 
                                   unsigned org_sig, 
                                   ubs_reply_register_t* p);

/*
 *  Message handler for the UBS_SIG_REPLY_MSG_LIST signal.
 */
void
uba_data_handle_msg_reply_list (int src_module, 
                                unsigned org_sig, 
                                ubs_reply_msg_list_t *p);

/*
 *  Message handler for the UBS_SIG_NOTIFY_MSG_DELETED signal.
 */
void
uba_data_handle_notify_msg_deleted (int src_module, 
                                    unsigned org_sig, 
                                    ubs_notify_msg_deleted_t *p);

/*
 *  Message handler for the UBS_SIG_NOTIFY_MSG_CHANGED signal.
 */
void
uba_data_handle_notify_msg_changed (int src_module, 
                                    unsigned org_sig, 
                                    ubs_notify_msg_changed_t *p);

/*
 *  Message handler for the UBS_SIG_NOTIFY_NEW_MSG signal.
 */
void
uba_data_handle_notify_new_msg (int src_module, 
                                unsigned org_sig, 
                                ubs_notify_new_msg_t *p);

#endif      /*UBA_DATA_H*/

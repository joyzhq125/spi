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
 * uba_data.c
 *
 * Created by Klas Hermodsson, 2003/08/20.
 *
 * Revision history:
 *  
 * 2004-01-21 (KLHE): Added uba_data_log_msg_list_item for logging
 *                      msg items (both new items and changes).
 * 2004-02-17 (KOGR): CR 13636, voice mail: uba_data_get_voice_mail_num and
 *                     uba_registry_settings added;
 *                     uba_handle_data_msg_regResponse modified
 * 2004-03-03 (KOGR): Adapted to WE Q04A1
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Data.h"

#include "Uba_Cfg.h"
#include "Uba_Main.h"
#include "Uba_Fold.h"
#include "We_Cmmn.h"
#ifdef WE_LOG_MODULE
  #include "We_Log.h"
#endif

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/* Internal use do not change */
#define UBA_MSG_TYPE_DISCONNECTED 0
/* Defines on msg type
 * t is the type as specified in ubs_cfg.h
 */
#define UBA_MSG_TYPE(t) { t,UBA_MSG_TYPE_DISCONNECTED }

#define UBA_END_MSG_TYPE_LIST -1

#define UBA_ALL_MESSAGE_TYPES -1

/**********************************************************************
 * Type declarations                                                      
 **********************************************************************/

/*
 *  The enumeration of states for the data part.
 */
typedef enum {
  UBA_DATA_STATE_STARTING,
  UBA_DATA_STATE_STARTED,
  UBA_DATA_STATE_STOPPING,
  UBA_DATA_STATE_STOPPED
} uba_data_state_t;

/*
 *  The enumeration of states for a message type.
 */
typedef enum
{
  UBA_DATA_MSG_TYPE_STATE_DISCONNECTED = UBA_MSG_TYPE_DISCONNECTED,
  UBA_DATA_MSG_TYPE_STATE_REGISTERING,
  UBA_DATA_MSG_TYPE_STATE_FETCHING,
  UBA_DATA_MSG_TYPE_STATE_CONNECTED,
  UBA_DATA_MSG_TYPE_STATE_DISCONNECTING
} uba_data_msg_type_state_t;

/*
 *	A placeholder for a message type wid and its state
 */
typedef struct {
  int                         msg_type;
  uba_data_msg_type_state_t   state;
}uba_data_msg_type_t;

/*
 *	A placeholder a folder mapping that maps a folder property string to a folder index
 */
typedef struct {
  int         msg_type;
  const char* folder;
  int         index;
}uba_folder_mapping_t;

/*
*   A placeholder for registry settings (not kept in other structs)
*/
typedef struct {
  char *voice_mail_number;
} uba_reg_settings_t;

/**********************************************************************
 * Global variables                                                      
 **********************************************************************/

/* The state of the data part of the UBA. */
static uba_data_state_t           uba_data_state;
/* The array of message types and their states */
static uba_data_msg_type_t        uba_data_msg_types[] = UBA_HANDLED_MSG_TYPES; 
/* The array of folders */
static uba_folder_t               uba_data_folders[UBA_FOLDER_COUNT];
/* A set of folder mapping rules that is used to divide a message to its folder 
depending on its folder property value */
static const uba_folder_mapping_t uba_data_folder_mapping[] = UBA_FOLDER_MAPPING;
/* The list of all messages */
static uba_data_item_t            *uba_data_items;

/* Settings from the registry */
static uba_reg_settings_t    uba_registry_settings;

/**********************************************************************
 * Forward declarations of local functions:
 **********************************************************************/

static uba_data_msg_type_t*
uba_data_lookup_msg_type (int type);

static void
uba_data_register_msg_type (uba_data_msg_type_t* p);

static void
uba_data_deregister_msg_type (uba_data_msg_type_t* p);

static void
uba_data_add_item (int msg_type, ubs_msg_list_item_t *msg);

static void
uba_data_change_item (int msg_type, ubs_msg_list_item_t *msg);

static void
uba_data_delete_item (int msg_type, ubs_msg_handle_t *handle);

static uba_folder_id_t
uba_data_lookup_folder (ubs_msg_list_item_t *msg, int msg_type);

#ifdef WE_LOG_MODULE
  static void
  uba_data_log_msg_list_item (int msg_type, const ubs_msg_list_item_t *msg);
#endif


/**********************************************************************
 * External functions
 **********************************************************************/

/*
 *  Initiates the data part of the UBA.
 *  Registration of needed message types (UBA_HANDLED_MSG_TYPES, see uba_cfg.h) 
 *  towards UBS is started.
 */
void
uba_data_init (void)
{
  uba_folder_id_t i;

  uba_data_items = NULL;
  uba_data_state = UBA_DATA_STATE_STARTING;

  /* Initiate default voice mail value number */
  uba_registry_settings.voice_mail_number = NULL;
  uba_data_set_voice_mail_num (UBA_DEFAULT_VOICE_MAIL_NUMBER, FALSE);

  /* Check registry for voice mail number and add registry subscription */
  WE_REGISTRY_GET (WE_MODID_UBA, 1, (char *)UBA_REG_PATH_SETTINGS,
                    (char*)UBA_REG_KEY_VOICE_MAIL_NUMBER);
  WE_REGISTRY_SUBSCRIBE (WE_MODID_UBA, /*wid*/1, (char *)UBA_REG_PATH_SETTINGS,
                    /*all keys*/NULL, /*add*/TRUE);
  
  /* Start registering the first msg type */
  uba_data_register_msg_type (uba_data_msg_types);
  
  /* init the folder array */
  for (i = 0; i < UBA_FOLDER_COUNT; ++i) {
    uba_folder_init (&(uba_data_folders[i]), i);
  }

  uba_data_init_done ();
}


/*
 *  Termination of the data part of the UBA.
 *  Deregistration of registered message types towards UBS is taken care of.
 */
void
uba_data_terminate (void)
{
  int               i = 0;
  uba_data_item_t   *item;

  uba_data_state = UBA_DATA_STATE_STOPPING;

  UBA_MEM_FREE (uba_registry_settings.voice_mail_number);

  /* Deregister all msg types */
  while (uba_data_msg_types[i].msg_type != UBA_END_MSG_TYPE_LIST) {
    if (uba_data_msg_types[i].state != UBA_DATA_MSG_TYPE_STATE_DISCONNECTED) {
      uba_data_deregister_msg_type (&(uba_data_msg_types[i]));
    }
    i++;
  }

  /* Dealloc folders */
  for (i = 0; i < UBA_FOLDER_COUNT; ++i) {
    uba_folder_release (&(uba_data_folders[i]));
  }

  /* Dealloc the data items */
  while (uba_data_items) {
    item = uba_data_items;
    uba_data_items = uba_data_items->next;
    uba_data_item_release (item);
    UBA_MEM_FREE (item);    
  }

  uba_terminate_data_done ();
}


/*
 *	Retrieves the folder data object from its folder index (see uba_cfg.h).
 */
uba_folder_t*
uba_data_get_folder (uba_folder_id_t folder_id)
{
  return &(uba_data_folders[folder_id]);
}


/*
 *  Checks to see if the the data objects are equal. a and b are equal if
 *  they are of the same type and they point to the same folder/folder_iten.
 *  If a equals b then this function returns TRUE, else FALSE.
 */
int
uba_data_object_is_equal (const uba_data_obj_t *a, const uba_data_obj_t *b)
{
  if (!a || !b) {
    return FALSE;
  }

  if (a->type != b->type) {
    return FALSE;
  }


  switch(a->type) {
  case UBA_DATA_OBJECT_FOLDER:
    return a->data.folder == b->data.folder;

  case UBA_DATA_OBJECT_MSG:
    return a->data.folder_item == b->data.folder_item;

  default:
    return TRUE;
  }
}

/*
 *  This utility functions finds a property in a message.
 *  Returns the property or NULL if it was not found.
 */

ubs_key_value_t*
uba_data_util_lookup_msg_property (ubs_msg_list_item_t *msg, int property_key)
{
  int i;

  if (!msg || !msg->titleList || (property_key < 0)) {
    return NULL;
  }

  for (i = 0; i < msg->titleListSize; ++i) {
    if (msg->titleList[i].key == property_key) {
      return &(msg->titleList[i]);
    }
  }

  return NULL;
}

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
 *  raw data result from memcmp(a,b,min(a.length,b.length) and other value types
 * 
 *  If not the same types the return value is a.type - b.type
 */
int
uba_data_util_compare_key_value (const ubs_key_value_t *a, const ubs_key_value_t *b)
{
  if (a->valueType != b->valueType){
    return a->valueType - b->valueType;
  }

  switch (a->valueType)
  {
  case UBS_VALUE_TYPE_UINT32:
    {
        WE_UINT32 aa = uba_data_util_get_uintval (a);
        WE_UINT32 bb = uba_data_util_get_uintval (b);
        if (aa == bb){
          return 0;
        }
     
        return (aa<bb) ? -1 : 1;
    }
  case UBS_VALUE_TYPE_INT32:      
    {
       WE_INT32 aa = uba_data_util_get_intval (a);
       WE_INT32 bb = uba_data_util_get_intval (b);
       if (aa == bb){
          return 0;
       }
     
       return (aa<bb) ? -1 : 1;
    }
  
  case UBS_VALUE_TYPE_UTF8:       
    return we_cmmn_strcmp_nc((const char*)a->value,(const char*)b->value);
  
  case UBS_VALUE_TYPE_BOOL:
    /* Note: bolean is true if != 0  -=>  -20 = true and 42 = true; */
    /* we convert true to 1 and false to 0 and returns the difference*/
    return (uba_data_util_get_boolval (a) ? 1 : 0) - 
           (uba_data_util_get_boolval (b) ? 1 : 0);
      
  case UBS_VALUE_TYPE_BINARY:
  default:
    {
      int res = memcmp(a->value,b->value,MIN(a->valueLen,a->valueLen));
      if (res == 0){
        /* Same byte array the lesser one is the one with the siallest array */
        res = a->valueLen - b->valueLen; 
      }
      return res;
    }
    
  }
}

WE_INT32
uba_data_util_get_intval(const ubs_key_value_t *p)
{
  

  return *((WE_INT32*)p->value);
}

WE_UINT32
uba_data_util_get_uintval(const ubs_key_value_t *p)
{
  

  return *((WE_UINT32*)p->value);
}

WE_BOOL
uba_data_util_get_boolval(const ubs_key_value_t *p)
{
  

  return *((WE_BOOL*)p->value);
}

const char*
uba_data_get_voice_mail_num (void)
{
  return uba_registry_settings.voice_mail_number;
}

void
uba_data_set_voice_mail_num (const char* numStr, int save_to_registry)
{
  /* Update voice mail variable */
  if (NULL != uba_registry_settings.voice_mail_number) {
    UBA_MEM_FREE (uba_registry_settings.voice_mail_number);
  }
  uba_registry_settings.voice_mail_number = UBA_MEM_ALLOC(strlen(numStr)+1);
	memcpy(uba_registry_settings.voice_mail_number, numStr, strlen(numStr)+1);

  /* Update registry */
  if (TRUE == save_to_registry) {
    void *handle = WE_REGISTRY_SET_INIT (WE_MODID_UBA);
    WE_REGISTRY_SET_PATH (handle, (char*)UBA_REG_PATH_SETTINGS);
    WE_REGISTRY_SET_ADD_KEY_STR (handle, (char*)UBA_REG_KEY_VOICE_MAIL_NUMBER,
                        (unsigned char*)numStr, (WE_UINT16)(strlen(numStr)+1));
    WE_REGISTRY_SET_COMMIT (handle);
  }
}


/************************************************************************/
/* Message handlers (Exported functions)                                */
/************************************************************************/


/*
 *  Message handler for the WE_SIG_REG_RESPONSE signal.
 */
void
uba_handle_data_msg_regResponse (int src_module, 
                                 unsigned org_sig, 
                                 we_registry_response_t* p)
{
  UNREFERENCED_PARAMETER(src_module);

  switch(uba_data_state)
  {
    
  default:
    {
      uba_folder_t*         folder;
      we_registry_param_t  param;

      if (TRUE != WE_REGISTRY_RESPONSE_GET_NEXT (p, &param)) {
        break;
      }

      /* Voice mail number: */
      if (0 == we_cmmn_strcmp_nc (param.key, (char*)UBA_REG_KEY_VOICE_MAIL_NUMBER))
      {
        if (param.type != WE_REGISTRY_TYPE_STR) {
          break;
        }
        uba_data_set_voice_mail_num ((char*)(param.value_bv), FALSE);
      }

      /* If a notify -> discard it */
      else if (p->wid == 0) {
        break;
      }

      else    /* Custom folders: */
      {
        folder = uba_data_get_folder (p->wid - 1);

        if (!folder) {
          
          break;
        }
        
        if (param.type != WE_REGISTRY_TYPE_STR) {
          break;
        }

        uba_folder_set_custom_name (folder, (char*)(param.value_bv), FALSE);
      }
    }
  }
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


/*
 *  Message handler for the UBS_SIG_REPLY_REGISTER signal.
 */
void
uba_data_handle_msg_reply_register (int src_module, 
                                    unsigned org_sig, 
                                    ubs_reply_register_t* p)
{
  UNREFERENCED_PARAMETER(src_module);

  switch(uba_data_state)
  {
    
  default:
    {
      uba_data_msg_type_t* msg_type = uba_data_lookup_msg_type(p->msgType);
      if (!msg_type){
        
        break;
      }
      
      if (p->result == UBS_RESULT_SUCCESS) {
        msg_type->state = UBA_DATA_MSG_TYPE_STATE_FETCHING;
      
        UBSif_getMsgList (WE_MODID_UBA, msg_type - uba_data_msg_types /* index in list */,
          p->msgType, 0, NULL, 0);
      }
      else {
        
        msg_type->state = UBA_DATA_MSG_TYPE_STATE_DISCONNECTED;
      }

      ++msg_type; /* move to the next in the array */
      if ( msg_type->msg_type != UBA_END_MSG_TYPE_LIST) {
        uba_data_register_msg_type(msg_type);
      }

    }
  }
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


/*
 *  Message handler for the UBS_SIG_REPLY_MSG_LIST signal.
 */
void
uba_data_handle_msg_reply_list (int                   src_module, 
                                unsigned              org_sig, 
                                ubs_reply_msg_list_t *p)
{
  int i;

  UNREFERENCED_PARAMETER(src_module);
  switch(uba_data_state)
  {
    
  default:
    {
      uba_data_msg_type_t* msg_type = &uba_data_msg_types[p->transactionId];

      /* add msgs to internal data structures */
      for (i = 0; i < p->msgListSize; ++i) {
        /* we use change item here since it might be a duplicate */
        uba_data_change_item (p->msgType, &(p->msgList[i]));
      }

      if (p->nextMsg != 0) {
        /* more msgs to fetch */
        UBSif_getMsgList (WE_MODID_UBA, p->transactionId, p->msgType, 0, NULL, p->nextMsg);
      }
      else {
        msg_type->state = UBA_DATA_MSG_TYPE_STATE_CONNECTED;
      }
    }  
  }
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


/*
 *  Message handler for the UBS_SIG_NOTIFY_MSG_DELETED signal.
 */
void
uba_data_handle_notify_msg_deleted (int src_module, 
                                    unsigned org_sig, 
                                    ubs_notify_msg_deleted_t *p)
{
  UNREFERENCED_PARAMETER(src_module);

  switch(uba_data_state)
  {
    
  default:
    {
      uba_data_msg_type_t* msg_type = uba_data_lookup_msg_type(p->msgType);
      if (!msg_type){
        
        break;
      }

      uba_data_delete_item (p->msgType, &(p->handle));
    }
  }
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


/*
 *  Message handler for the UBS_SIG_NOTIFY_MSG_CHANGED signal.
 */
void
uba_data_handle_notify_msg_changed (int src_module, 
                                    unsigned org_sig, 
                                    ubs_notify_msg_changed_t *p)
{
  UNREFERENCED_PARAMETER(src_module);

  switch(uba_data_state)
  {
    
  default:
    {
      uba_data_msg_type_t* msg_type = uba_data_lookup_msg_type(p->msgType);
      if (!msg_type){
        
        break;
      }

      uba_data_change_item (p->msgType, &(p->msg));
    }
  }
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


/*
 *  Message handler for the UBS_SIG_NOTIFY_NEW_MSG signal.
 */
void
uba_data_handle_notify_new_msg (int src_module, 
                                unsigned org_sig, 
                                ubs_notify_new_msg_t *p)
{
  UNREFERENCED_PARAMETER(src_module);

  switch(uba_data_state)
  {
    
  default:
    {
      uba_data_msg_type_t* msg_type = uba_data_lookup_msg_type(p->msgType);
      if (!msg_type){
        
        break;
      }

      uba_data_add_item (p->msgType, &(p->msg));
    }
  }
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


/**********************************************************************
 * Local functions
 **********************************************************************/

static uba_data_msg_type_t*
uba_data_lookup_msg_type (int type)
{
  uba_data_msg_type_t* msg_type = uba_data_msg_types;
  for ( ;msg_type->msg_type != UBA_END_MSG_TYPE_LIST && 
         msg_type->msg_type != type; ++msg_type){
    /* empty */
  }
  return (msg_type->msg_type != type ) ? NULL : msg_type;
}


static void
uba_data_register_msg_type (uba_data_msg_type_t* p)
{
  UBSif_register (WE_MODID_UBA, p->msg_type);
  p->state = UBA_DATA_MSG_TYPE_STATE_REGISTERING;
}


static void
uba_data_deregister_msg_type (uba_data_msg_type_t* p)
{
  UBSif_deregister (WE_MODID_UBA, p->msg_type);
  p->state = UBA_DATA_MSG_TYPE_STATE_DISCONNECTED;
}


static uba_data_item_t*
uba_data_find_item (int msg_type, ubs_msg_handle_t *handle)
{
  uba_data_item_t   *e = uba_data_items;
  WE_UINT32        hash = uba_data_item_util_calc_hash (handle);

  while (e && !uba_data_item_has_same_handle (e, hash, msg_type, handle)) {
    e = e->next;
  }

  return e;
}


static void
uba_data_add_item (int msg_type, ubs_msg_list_item_t *msg)
{
  uba_data_item_t   *e = UBA_MEM_ALLOCTYPE (uba_data_item_t);
  uba_folder_id_t  folder_index;

  

#ifdef WE_LOG_MODULE
  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "UBA: uba_data_add_item\n"));
  uba_data_log_msg_list_item (msg_type, msg);
#endif

  folder_index = uba_data_lookup_folder (msg, msg_type);

  uba_data_item_init (e, msg_type, msg);
  e->next = uba_data_items;
  uba_data_items = e;

  if (folder_index != UBA_NO_FOLDER_ID) {
    uba_folder_add (&(uba_data_folders[folder_index]), e);
    e->folder_index = folder_index;
  }
  else {
    uba_data_item_release (e);
    UBA_MEM_FREE (e);
  }
}


static void
uba_data_change_item (int msg_type, ubs_msg_list_item_t *msg)
{
  uba_data_item_t   *e = uba_data_find_item (msg_type, &(msg->handle));
  uba_folder_id_t   new_folder_index;
  int               i;

  if (!e) {
    uba_data_add_item (msg_type, msg);
    return;
  }

#ifdef WE_LOG_MODULE
  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "UBA: uba_data_change_item\n"));
  uba_data_log_msg_list_item (msg_type, msg);
#endif

  new_folder_index = uba_data_lookup_folder (msg, msg_type);

  if (e->folder_index != new_folder_index) {
    /* move the item between two folders */
    uba_folder_delete (&(uba_data_folders[e->folder_index]), e->folder_item);
    e->folder_item = NULL;
    e->folder_index = new_folder_index;
    uba_folder_add (&(uba_data_folders[new_folder_index]), e);
  }

  /* release the e->msg.titleList memory */
  for (i = 0; i < e->msg.titleListSize; ++i) {
    UBA_MEM_FREE (e->msg.titleList[i].value);
  }
  UBA_MEM_FREE (e->msg.titleList);

  /* take over the memory of the title list from the msg */
  e->msg.titleList = msg->titleList;
  msg->titleList = NULL;
  e->msg.titleListSize = msg->titleListSize;
  msg->titleListSize = 0;

  if (e->folder_index == new_folder_index) {
    /* only signal folder item change if it stays in the same folder */
    uba_folder_item_on_data_changed (e->folder_item);
    uba_folder_on_folder_item_changed (&(uba_data_folders[e->folder_index]));
  }
}


static void
uba_data_delete_item (int msg_type, ubs_msg_handle_t *handle)
{
  uba_data_item_t   *to_delete;
  uba_data_item_t   *e = uba_data_items;
  WE_UINT32        hash = uba_data_item_util_calc_hash (handle);

  if (!e) {
    return;
  }

  if (uba_data_item_has_same_handle (e, hash, msg_type, handle)) {
    if (e->folder_index != UBA_NO_FOLDER_ID) {
      uba_folder_delete (&(uba_data_folders[e->folder_index]), e->folder_item);
    }
    to_delete = e;
    uba_data_items = to_delete->next;

    uba_data_item_release (to_delete);
    UBA_MEM_FREE (to_delete);
    return;
  }

  while ((e->next) && !uba_data_item_has_same_handle (e->next, hash, msg_type, handle)) {
    e = e->next;
  }

  to_delete = e->next;

  if (to_delete) {
    if (to_delete->folder_index != UBA_NO_FOLDER_ID) {
      uba_folder_delete (&(uba_data_folders[to_delete->folder_index]), to_delete->folder_item);
    }
    e->next = to_delete->next;

    uba_data_item_release (to_delete);
    UBA_MEM_FREE (to_delete);
  }
}

#define UBA_DATA_MATCHES_MESSAGE_TYPE(msg_type, msg_type_map) \
 ((msg_type_map) == UBA_ALL_MESSAGE_TYPES || (msg_type) == (msg_type_map) )    

/*
 *  Returns the folder index of the message based on the folder property.
 *  NOTE: the last entry in the folder mapping is used as the folder where
 *  all non-mapped messages will end up in.
 */
static uba_folder_id_t
uba_data_lookup_folder (ubs_msg_list_item_t *msg, int msg_type)
{
  ubs_key_value_t            *property = uba_data_util_lookup_msg_property (msg, UBS_MSG_KEY_FOLDER);
  const uba_folder_mapping_t *fold_map = uba_data_folder_mapping;

  if (!property) {
    /* No folder, Try to find default folder wid*/
    
    /* To find the default folder this must be true
     * found = no folder (NULL, !folder_map->folder) and Message type match
     * This is negated in this while loop
     */
    while (fold_map->folder ||
           !UBA_DATA_MATCHES_MESSAGE_TYPE (msg_type, fold_map->msg_type)){
       ++fold_map;
    }
    return fold_map->index;
  }

  fold_map = uba_data_folder_mapping;

  /* To find a record
   * found = message type match AND ( no folder(null) or folder match )
   * if we negate that statement we get the while loop that loops untile one recor is
   * found.
   */
  while(!((UBA_DATA_MATCHES_MESSAGE_TYPE (msg_type, fold_map->msg_type) &&
          (!fold_map->folder || 
           0 == we_cmmn_strcmp_nc (fold_map->folder, (char*)(property->value)))))){
    ++fold_map;
  }
  
  
  return fold_map->index;
}


#ifdef WE_LOG_MODULE

static void
uba_data_log_msg_list_item (int msg_type, const ubs_msg_list_item_t *msg)
{
  char  str[1000];
  int   pos;
  int   i;
  int   j;

  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "UBA uba_data_log_msg_list_item START\n"));

  pos = 0;
  pos += sprintf (&str[pos], "\tMessage type: ");
  switch(msg_type) {
  case UBS_MSG_TYPE_PUSH_SI:
    pos += sprintf (&str[pos], "Push SI");
  	break;
  case UBS_MSG_TYPE_PUSH_SL:
    pos += sprintf (&str[pos], "Push SL");
  	break;
  case UBS_MSG_TYPE_MMS:
    pos += sprintf (&str[pos], "MMS");
  	break;
  case UBS_MSG_TYPE_EMS:   
    pos += sprintf (&str[pos], "EMS");
  	break;
  case UBS_MSG_TYPE_EMAIL:
    pos += sprintf (&str[pos], "Email");
  	break;
  default:
    pos += sprintf (&str[pos], "%i", msg_type);
  }
  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "%s\n", str));


  pos = 0;
  pos += sprintf (&str[pos], "\tHandle: 0x");
  for (i = 0; i < msg->handle.handleLen; ++i) {
    pos += sprintf (&str[pos], "%.2x", msg->handle.handle[i]);
  }
  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "%s\n", str));


  for (i = 0; i < msg->titleListSize; ++i) {
    pos = 0;
    switch(msg->titleList[i].key) {
    case UBS_MSG_KEY_FOLDER                :
      pos += sprintf (&str[pos], "\tFolder:       ");
    	break;
    case UBS_MSG_KEY_SUBTYPE               :
      pos += sprintf (&str[pos], "\tSubtype:      ");
    	break;
    case UBS_MSG_KEY_SUBJECT               :
      pos += sprintf (&str[pos], "\tSubject:      ");
    	break;
    case UBS_MSG_KEY_FROM                  :
      pos += sprintf (&str[pos], "\tFrom:         ");
    	break;
    case UBS_MSG_KEY_TIMESTAMP             :
      pos += sprintf (&str[pos], "\tTimestamp:    ");
    	break;
    case UBS_MSG_KEY_READ                  :
      pos += sprintf (&str[pos], "\tRead:         ");
    	break;
    case UBS_MSG_KEY_FORWARD_LOCK          :
      pos += sprintf (&str[pos], "\tFwd lock:     ");
    	break;
    case UBS_MSG_KEY_PRIORITY              :
      pos += sprintf (&str[pos], "\tPrio:         ");
    	break;
    case UBS_MSG_KEY_TO                    :
      pos += sprintf (&str[pos], "\tTo:           ");
    	break;
    case UBS_MSG_KEY_CREATED               :
      pos += sprintf (&str[pos], "\tCreated:      ");
    	break;
    case UBS_MSG_KEY_EXPIRES               :
      pos += sprintf (&str[pos], "\tExpires:      ");
    	break;
    case UBS_MSG_KEY_NBR_OF_ATTACHMENTS    :
      pos += sprintf (&str[pos], "\tAttachments:  ");
    	break;
    case UBS_MSG_KEY_REPORT_STATUS         :
      pos += sprintf (&str[pos], "\tReport status:       ");
    	break;
    case UBS_MSG_KEY_URL                   :
      pos += sprintf (&str[pos], "\tURL:          ");
    	break;
    case UBS_MSG_KEY_SIZE                  :
      pos += sprintf (&str[pos], "\tSize:         ");
    	break;
    case UBS_MSG_KEY_STORAGE               :
      pos += sprintf (&str[pos], "\tStorage:      ");
    	break;
    case UBS_MSG_KEY_NBR_OF_MESSAGES_WAITING:
      pos += sprintf (&str[pos], "\tNbr of msgs waiting: ");
    	break;
    case UBS_MSG_KEY_MSG_STATUS:
      pos += sprintf (&str[pos], "\tMsg status:   ");
    	break;
    default:
      pos += sprintf (&str[pos], "\t%i: ", msg->titleList[i].key);
    }

    switch(msg->titleList[i].valueType) {
    case UBS_VALUE_TYPE_UINT32    :
      pos += sprintf (&str[pos], "UINT32 [%lu]", uba_data_util_get_uintval (&(msg->titleList[i])));
    	break;
    case UBS_VALUE_TYPE_INT32     :
      pos += sprintf (&str[pos], "INT32 [%ld]", uba_data_util_get_intval (&(msg->titleList[i])));
    	break;
    case UBS_VALUE_TYPE_UTF8      :
      pos += sprintf (&str[pos], "UTF8, len %d [%s]", msg->titleList[i].valueLen, msg->titleList[i].value);
    	break;
    case UBS_VALUE_TYPE_BOOL      :
      pos += sprintf (&str[pos], "BOOL [%d]", uba_data_util_get_boolval (&(msg->titleList[i])));
    	break;
    case UBS_VALUE_TYPE_BINARY    :
      pos += sprintf (&str[pos], "BIN, len %d [0x", msg->titleList[i].valueLen);
      for (j = 0; j < msg->titleList[i].valueLen; ++j) {
        pos += sprintf (&str[pos], "%.2x", msg->titleList[i].value[j]);
      }
      pos += sprintf (&str[pos], "]");
    	break;
    default:
      pos += sprintf (&str[pos], "Unknown value type! (%d), len %d [0x", msg->titleList[i].valueType, msg->titleList[i].valueLen);
      for (j = 0; j < msg->titleList[i].valueLen; ++j) {
        pos += sprintf (&str[pos], "%.2x", msg->titleList[i].value[j]);
      }
      pos += sprintf (&str[pos], "]");
    }
  
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "%s\n", str));
  }

  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "UBA uba_data_log_msg_list_item END\n"));
}

/* WE_LOG_MODULE */
#endif

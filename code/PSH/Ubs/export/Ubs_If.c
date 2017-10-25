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
* ubs_if.c
*
* Created by Klas Hermodsson, May 30 2003.
*
* Revision  history:
*
*/
#include "We_Core.h"
#include "We_Mem.h"

#include "Ubs_If.h"
#include "We_Log.h"

/**********************************************************************
 * Forward declarations of local functions:
 **********************************************************************/

int
ubs_cvt_msg_handle (we_dcvt_t *obj, ubs_msg_handle_t *p);

int
ubs_cvt_key_value (we_dcvt_t *obj, ubs_key_value_t *p);

int
ubs_cvt_msg_list_item (we_dcvt_t *obj, ubs_msg_list_item_t *p);

int
ubs_cvt_msg (we_dcvt_t *obj, ubs_msg_t *p);

int
ubs_cvt_full_msg (we_dcvt_t *obj, ubs_full_msg_t *p);


/**********************************************************************
* API invoked from other modules
**********************************************************************/

void
UBSif_startInterface (void)
{
  WE_LOG_FC_BEGIN(UBSif_startInterface)
  WE_LOG_FC_PRE_IMPL

  WE_SIGNAL_REG_FUNCTIONS (WE_MODID_UBS, ubs_convert, ubs_destruct);

  WE_LOG_FC_END
}


/**********************************************************************
* Conversion routines for all signal parameter structs
**********************************************************************/

int 
ubs_cvt_register (we_dcvt_t *obj, ubs_register_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_uint16 (obj, &(p->transactionId))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_register (we_dcvt_t *obj, ubs_reply_register_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_int8 (obj, &(p->result))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_deregister (we_dcvt_t *obj, ubs_deregister_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->msgType))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_get_nbr_of_msgs (we_dcvt_t *obj, ubs_get_nbr_of_msgs_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_uint16 (obj, &(p->filterListSize)) ||
      !we_dcvt_array (obj, sizeof (ubs_key_value_t), p->filterListSize, 
                     (void **)&(p->filterList), (we_dcvt_element_t *)ubs_cvt_key_value)) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_nbr_of_msgs (we_dcvt_t *obj, ubs_reply_nbr_of_msgs_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_uint16 (obj, &(p->nbrOfMsgs))) {
    return FALSE;
  }

  return TRUE;

}


int 
ubs_cvt_get_msg_list (we_dcvt_t *obj, ubs_get_msg_list_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_uint16 (obj, &(p->filterListSize)) ||
      !we_dcvt_array (obj, sizeof (ubs_key_value_t), p->filterListSize, 
                     (void **)&(p->filterList), (we_dcvt_element_t *)ubs_cvt_key_value) ||
      !we_dcvt_uint16 (obj, &(p->startFromMsg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_msg_list (we_dcvt_t *obj, ubs_reply_msg_list_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_uint16 (obj, &(p->msgListSize)) ||
      !we_dcvt_array (obj, sizeof (ubs_msg_list_item_t), p->msgListSize, 
                    (void **)&(p->msgList), (we_dcvt_element_t *)ubs_cvt_msg_list_item) ||
      !we_dcvt_uint16 (obj, &(p->nextMsg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_get_msg (we_dcvt_t *obj, ubs_get_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_msg (we_dcvt_t *obj, ubs_reply_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_int8 (obj, &(p->result)) ||
      !ubs_cvt_msg (obj, &(p->msg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_get_full_msg (we_dcvt_t *obj, ubs_get_full_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_bool (obj, &(p->getPropertyList)) ||
      !we_dcvt_bool (obj, &(p->getHiddenPropertyList)) ||
      !we_dcvt_bool (obj, &(p->getData))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_full_msg (we_dcvt_t *obj, ubs_reply_full_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_int8 (obj, &(p->result)) ||
      !ubs_cvt_full_msg (obj, &(p->msg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_delete_msg (we_dcvt_t *obj, ubs_delete_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_delete_msg (we_dcvt_t *obj, ubs_reply_delete_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_int8 (obj, &(p->result))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_notify_msg_deleted (we_dcvt_t *obj, ubs_notify_msg_deleted_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_change_msg (we_dcvt_t *obj, ubs_change_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_bool (obj, &(p->changePropertyList)) ||
      !we_dcvt_bool (obj, &(p->changeHiddenPropertyList)) ||
      !we_dcvt_bool (obj, &(p->changeData)) ||
      !ubs_cvt_full_msg (obj, &(p->msg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_change_msg (we_dcvt_t *obj, ubs_reply_change_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_int8 (obj, &(p->result))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_notify_msg_changed (we_dcvt_t *obj, ubs_notify_msg_changed_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_list_item (obj, &(p->msg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_notify_new_msg (we_dcvt_t *obj, ubs_notify_new_msg_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_list_item (obj, &(p->msg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_create_msg (we_dcvt_t *obj, ubs_create_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_full_msg (obj, &(p->msg))) {
    return FALSE;
  }

  return TRUE;
}


int 
ubs_cvt_reply_create_msg (we_dcvt_t *obj, ubs_reply_create_msg_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_int8 (obj, &(p->result))) {
    return FALSE;
  }

  return TRUE;
}


int
ubs_cvt_notify_unread_msg_count (we_dcvt_t *obj, ubs_notify_unread_msg_count_t *p)
{
  if (!we_dcvt_uint8 (obj, &(p->msgType)) ||
      !we_dcvt_uint16 (obj, &(p->nbrOfUnreadMsgs))) {
    return FALSE;
  }

  return TRUE;
}


int
ubs_cvt_notify_invalidate_all (we_dcvt_t *obj, ubs_notify_invalidate_all_t *p)
{
  return we_dcvt_uint8 (obj, &(p->msgType));
}


int
ubs_cvt_set_msg_property (we_dcvt_t *obj, ubs_set_msg_property_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !ubs_cvt_key_value (obj, &(p->propertyValue))) {
    return FALSE;
  }

  return TRUE;
}



int
ubs_cvt_reply_set_msg_property (we_dcvt_t *obj, ubs_reply_set_msg_property_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->transactionId)) ||
      !we_dcvt_uint8 (obj, &(p->msgType)) ||
      !ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_int8 (obj, &(p->result))) {
    return FALSE;
  }

  return TRUE;
}


/**********************************************************************
**********************************************************************/

int
ubs_cvt_msg_handle (we_dcvt_t *obj, ubs_msg_handle_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->handleLen)) ||
      !we_dcvt_uchar_vector (obj, p->handleLen, &(p->handle))) {
    return FALSE;
  }

  return TRUE;
}


int
ubs_cvt_key_value (we_dcvt_t *obj, ubs_key_value_t *p)
{
  if (!we_dcvt_uint16 (obj, &(p->key)) ||
      !we_dcvt_uint8 (obj, &(p->valueType)) ||
      !we_dcvt_uint16 (obj, &(p->valueLen)) ||
      !we_dcvt_uchar_vector (obj, p->valueLen, &(p->value))) {
    return FALSE;
  }

  return TRUE;
}


int
ubs_cvt_msg_list_item (we_dcvt_t *obj, ubs_msg_list_item_t *p)
{
  if (!ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_uint16 (obj, &(p->titleListSize)) ||
      !we_dcvt_array (obj, sizeof (ubs_key_value_t), p->titleListSize, 
                      (void **)&(p->titleList), (we_dcvt_element_t *)ubs_cvt_key_value)) {
    return FALSE;
  }

  return TRUE;
}


int
ubs_cvt_msg (we_dcvt_t *obj, ubs_msg_t *p)
{
  if (!ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_uint16 (obj, &(p->propertyListSize)) ||
      !we_dcvt_array (obj, sizeof (ubs_key_value_t), p->propertyListSize, 
                   (void **)&(p->propertyList), (we_dcvt_element_t *)ubs_cvt_key_value)) {
    return FALSE;
  }

  return TRUE;
}


int
ubs_cvt_full_msg (we_dcvt_t *obj, ubs_full_msg_t *p)
{
  if (!ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !we_dcvt_uint16 (obj, &(p->propertyListSize)) ||
      !we_dcvt_array (obj, sizeof (ubs_key_value_t), p->propertyListSize, 
                   (void **)&(p->propertyList), (we_dcvt_element_t *)ubs_cvt_key_value) ||
      !we_dcvt_uint16 (obj, &(p->hiddenPropertyListSize)) ||
      !we_dcvt_array (obj, sizeof (ubs_key_value_t), p->hiddenPropertyListSize, 
             (void **)&(p->hiddenPropertyList), (we_dcvt_element_t *)ubs_cvt_key_value) ||
      !we_dcvt_uint32 (obj, &(p->dataLen)) ||
      !we_dcvt_uchar_vector (obj, p->dataLen, &(p->data))) {
    return FALSE;
  }

  return TRUE;
}


/**********************************************************************
* Signal-sending API
* These functions should be used by any other module that wants to
* send a signal to the UBS module.
**********************************************************************/

void 
UBSif_register (int src, int msgType)
{
  ubs_register_t        param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_register)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.transactionId = 0; 
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_register (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REGISTER, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_register (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_deregister (int src, int msgType)
{
  ubs_deregister_t      param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_deregister)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_deregister (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_DEREGISTER, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_deregister (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_getNbrOfMsgs (int src,
                        unsigned transactionId,
                        int msgType, 
                        unsigned filterListSize, 
                        ubs_key_value_t *filterList)
{
  ubs_get_nbr_of_msgs_t param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_getNbrOfMsgs)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)filterListSize, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.filterListSize = (WE_UINT16)filterListSize;
  param.filterList = filterList;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_get_nbr_of_msgs (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_GET_NBR_OF_MSGS, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_get_nbr_of_msgs (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_getMsgList (int src,
                      unsigned transactionId,
                      int msgType, 
                      unsigned filterListSize, 
                      ubs_key_value_t *filterList,
                      unsigned startFromMsg)
{
  ubs_get_msg_list_t    param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_getMsgList)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)filterListSize, NULL)
  
  WE_LOG_FC_UINT16((WE_UINT16)startFromMsg, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.filterListSize = (WE_UINT16)filterListSize;
  param.filterList = filterList;
  param.startFromMsg = (WE_UINT16)startFromMsg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_get_msg_list (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_GET_MSG_LIST, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_get_msg_list (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_getMsg (int src, 
                  unsigned transactionId,
                  int msgType, 
                  ubs_msg_handle_t *handle)
{
  ubs_get_msg_t         param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_getMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_get_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_GET_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_get_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_deleteMsg (int src,
                     unsigned transactionId,
                     int msgType,
                     ubs_msg_handle_t *handle)
{
  ubs_delete_msg_t      param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_deleteMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_delete_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_DELETE_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_delete_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


/* 
WE_UINT8 src,
WE_UINT16 transactionId
WE_UINT8 msgType
ubs_msg_handle_t handle
ubs_key_value_t propertyValue
*/
void 
UBSif_setMsgProperty (int src,
                      unsigned transactionId,
                      int msgType,
                      ubs_msg_handle_t *handle,
                      ubs_key_value_t *propertyValue)
{
  ubs_set_msg_property_t  param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_setMsgProperty)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.propertyValue = *propertyValue;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_set_msg_property (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_SET_MSG_PROPERTY, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_set_msg_property (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


/* Functions for modules that has their own message storage */

void
UBSif_replyRegister (int src, 
                        unsigned transactionId,
                        int msgType,
                        int result)
{
  ubs_reply_register_t    param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_replyRegister)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_register (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_REGISTER, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_reply_register (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_replyNbrOfMsgs (int src, 
                          unsigned transactionId,
                          int msgType,
                          unsigned nbrOfMsgs)
{
  ubs_reply_nbr_of_msgs_t param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_replyNbrOfMsgs)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)nbrOfMsgs, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.nbrOfMsgs = (WE_UINT16)nbrOfMsgs;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_nbr_of_msgs (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_NBR_OF_MSGS, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_reply_nbr_of_msgs (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_replyMsgList (int src, 
                        unsigned transactionId,
                        int msgType,
                        unsigned msgListSize,
                        ubs_msg_list_item_t *msgList,
                        unsigned nextMsg)
{
  ubs_reply_msg_list_t    param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_replyMsgList)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)msgListSize, NULL)
  
  WE_LOG_FC_UINT16((WE_UINT16)nextMsg, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.msgListSize = (WE_UINT16)msgListSize;
  param.msgList = msgList;
  param.nextMsg = (WE_UINT16)nextMsg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_msg_list (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_MSG_LIST, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_reply_msg_list (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_replyMsg (int src, 
                    unsigned transactionId,
                    int msgType,
                    int result,
                    ubs_msg_t *msg)
{
  ubs_reply_msg_t         param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_replyMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.result = (WE_INT8)result;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_reply_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_replyDeleteMsg (int src,
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result)
{
  ubs_reply_delete_msg_t  param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_replyDeleteMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_delete_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_DELETE_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_reply_delete_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_notifyMsgDeleted (int src, 
                            int msgType,
                            ubs_msg_handle_t *handle)
{
  ubs_notify_msg_deleted_t  param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_notifyMsgDeleted)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_msg_deleted (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_MSG_DELETED, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_notify_msg_deleted (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_notifyMsgChanged (int src, 
                            int msgType,
                            ubs_msg_list_item_t *msg)
{
  ubs_notify_msg_changed_t  param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_notifyMsgChanged)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_msg_changed (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_MSG_CHANGED, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_notify_msg_changed (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_notifyNewMsg (int src, 
                        int msgType,
                        ubs_msg_list_item_t *msg)
{
  ubs_notify_new_msg_t      param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_notifyNewMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_new_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_NEW_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_notify_new_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


/*
 *	The following interface function must be used by any module acting as owner of 
 *  a message type. This function must be called after the start of such a module if
 *  the number of unread messages is > 0.
 *  This function must also be called when the number of unread messages changes.
 *
 *  The UBS module may not be started at these occasions. If UBS is not started then it is
 *  the task of the calling module to start UBS.
 */
/*
WE_UINT8 src,
WE_UINT8 msgType, 
WE_UINT16 nbrOfUnreadMsgs, 
*/
void 
UBSif_notifyUnreadMsgCount (int src,
                            int msgType, 
                            unsigned nbrOfUnreadMsgs)
{
  ubs_notify_unread_msg_count_t   param;
  we_dcvt_t                      cvt_obj;
  WE_UINT16                      length;
  void                            *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_notifyUnreadMsgCount)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)nbrOfUnreadMsgs, NULL)
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.nbrOfUnreadMsgs = (WE_UINT16)nbrOfUnreadMsgs;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_unread_msg_count (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_UNREAD_MSG_COUNT, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_notify_unread_msg_count (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


/*
 *  This notification signals that all messages belonging to the indicated
 *  message type are no longer valid. All registered viewers will be notified
 *  and should re-fetch messages of this message type.
 */
/*
WE_UINT8 src,
WE_UINT8 msgType, 
*/
void 
UBSif_notifyInvalidateAll (int src, int msgType)
{
  ubs_notify_invalidate_all_t     param;
  we_dcvt_t                      cvt_obj;
  WE_UINT16                      length;
  void                            *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_notifyInvalidateAll)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_invalidate_all (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_INVALIDATE_ALL, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_notify_invalidate_all (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


/*
WE_UINT8 src,
WE_UINT16 transactionId,
WE_UINT8 msgType,
ubs_msg_handle_t *handle,
WE_INT8 result
*/
void 
UBSif_replySetMsgProperty (int src,
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result)
{
  ubs_reply_set_msg_property_t  param;
  we_dcvt_t                    cvt_obj;
  WE_UINT16                    length;
  void                          *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_replySetMsgProperty)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_set_msg_property (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_SET_MSG_PROPERTY, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_reply_set_msg_property (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


/* Functions for modules using the UBS as messages storage */

void 
UBSif_getFullMsg (int src, 
                      unsigned transactionId,
                      int msgType, 
                      ubs_msg_handle_t *handle, 
                      int getPropertyList, 
                      int getHiddenPropertyList, 
                      int getData)
{
  ubs_get_full_msg_t        param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_getFullMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_INT(getPropertyList, NULL)
  WE_LOG_FC_INT(getHiddenPropertyList, NULL)
  WE_LOG_FC_INT(getData, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.getPropertyList = getPropertyList;
  param.getHiddenPropertyList = getHiddenPropertyList;
  param.getData = getData;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_get_full_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_GET_FULL_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_get_full_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
UBSif_changeMsg (int src, 
                     unsigned transactionId,
                     int msgType, 
                     int changePropertyList,
                     int changeHiddenPropertyList, 
                     int changeData, 
                     ubs_full_msg_t *msg)
{
  ubs_change_msg_t          param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_changeMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_INT(changePropertyList, NULL)
  WE_LOG_FC_INT(changeHiddenPropertyList, NULL)
  WE_LOG_FC_INT(changeData, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.changePropertyList = changePropertyList;
  param.changeHiddenPropertyList = changeHiddenPropertyList;
  param.changeData = changeData;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_change_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_CHANGE_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_change_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END

}


void 
UBSif_createMsg (int src, 
                     unsigned transactionId,
                     int msgType, 
                     ubs_full_msg_t *msg)
{
  ubs_create_msg_t          param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(UBSif_createMsg)
  WE_LOG_FC_UINT8((WE_UINT8)src, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_create_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_CREATE_MSG, (WE_UINT8)src,
                                     WE_MODID_UBS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)src);
    ubs_cvt_create_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}



/**********************************************************************
* Convert from memory buffer for signal structs.
*/
void* 
ubs_convert (WE_UINT8 module, WE_UINT16 signal, void* buffer)
{
  we_dcvt_t  cvt_obj;
  void       *user_data;
  WE_UINT16  length;

  if (buffer == NULL)
    return NULL;

  user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
  if ((user_data == NULL) || (length == 0))
    return NULL;

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, user_data, length, module);

  switch (signal){
  case UBS_SIG_REGISTER:
    {
      ubs_register_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_register_t);
      ubs_cvt_register (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_REGISTER:
    {
      ubs_reply_register_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_register_t);
      ubs_cvt_reply_register (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_DEREGISTER:
    {
      ubs_deregister_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_deregister_t);
      ubs_cvt_deregister (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_GET_NBR_OF_MSGS:
    {
      ubs_get_nbr_of_msgs_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_get_nbr_of_msgs_t);
      ubs_cvt_get_nbr_of_msgs (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_NBR_OF_MSGS:
    {
      ubs_reply_nbr_of_msgs_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_nbr_of_msgs_t);
      ubs_cvt_reply_nbr_of_msgs (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_GET_MSG_LIST:
    {
      ubs_get_msg_list_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_get_msg_list_t);
      ubs_cvt_get_msg_list (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_MSG_LIST:
    {
      ubs_reply_msg_list_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_msg_list_t);
      ubs_cvt_reply_msg_list (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_GET_MSG:
    {
      ubs_get_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_get_msg_t);
      ubs_cvt_get_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_MSG:
    {
      ubs_reply_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_msg_t);
      ubs_cvt_reply_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_GET_FULL_MSG:
    {
      ubs_get_full_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_get_full_msg_t);
      ubs_cvt_get_full_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_FULL_MSG:
    {
      ubs_reply_full_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_full_msg_t);
      ubs_cvt_reply_full_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_DELETE_MSG:
    {
      ubs_delete_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_delete_msg_t);
      ubs_cvt_delete_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_DELETE_MSG:
    {
      ubs_reply_delete_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_delete_msg_t);
      ubs_cvt_reply_delete_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_NOTIFY_MSG_DELETED:
    {
      ubs_notify_msg_deleted_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_notify_msg_deleted_t);
      ubs_cvt_notify_msg_deleted (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_CHANGE_MSG:
    {
      ubs_change_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_change_msg_t);
      ubs_cvt_change_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_CHANGE_MSG:
    {
      ubs_reply_change_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_change_msg_t);
      ubs_cvt_reply_change_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_NOTIFY_MSG_CHANGED:
    {
      ubs_notify_msg_changed_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_notify_msg_changed_t);
      ubs_cvt_notify_msg_changed (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_NOTIFY_NEW_MSG:
    {
      ubs_notify_new_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_notify_new_msg_t);
      ubs_cvt_notify_new_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_CREATE_MSG:
    {
      ubs_create_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_create_msg_t);
      ubs_cvt_create_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_CREATE_MSG:
    {
      ubs_reply_create_msg_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_create_msg_t);
      ubs_cvt_reply_create_msg (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_NOTIFY_UNREAD_MSG_COUNT:
    {
      ubs_notify_unread_msg_count_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_notify_unread_msg_count_t);
      ubs_cvt_notify_unread_msg_count (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_NOTIFY_INVALIDATE_ALL:
    {
      ubs_notify_invalidate_all_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_notify_invalidate_all_t);
      ubs_cvt_notify_invalidate_all (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_SET_MSG_PROPERTY:
    {
      ubs_set_msg_property_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_set_msg_property_t);
      ubs_cvt_set_msg_property (&cvt_obj, p);
      return (void *)p;
    }

  case UBS_SIG_REPLY_SET_MSG_PROPERTY:
    {
      ubs_reply_set_msg_property_t *p;

      p = WE_MEM_ALLOCTYPE (module, ubs_reply_set_msg_property_t);
      ubs_cvt_reply_set_msg_property (&cvt_obj, p);
      return (void *)p;
    }

  }

  return NULL;
}


/*
* Released a signal structure of a signal.
*/
void 
ubs_destruct (WE_UINT8 module, WE_UINT16 signal, void* p)
{
  we_dcvt_t cvt_obj;

  if (p == NULL)
    return;

  we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);

  switch (signal){
  case UBS_SIG_REGISTER:
    ubs_cvt_register (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_REGISTER:
    ubs_cvt_reply_register (&cvt_obj, p);
    break;

  case UBS_SIG_DEREGISTER:
    ubs_cvt_deregister (&cvt_obj, p);
    break;

  case UBS_SIG_GET_NBR_OF_MSGS:
    ubs_cvt_get_nbr_of_msgs (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_NBR_OF_MSGS:
    ubs_cvt_reply_nbr_of_msgs (&cvt_obj, p);
    break;

  case UBS_SIG_GET_MSG_LIST:
    ubs_cvt_get_msg_list (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_MSG_LIST:
    ubs_cvt_reply_msg_list (&cvt_obj, p);
    break;

  case UBS_SIG_GET_MSG:
    ubs_cvt_get_msg (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_MSG:
    ubs_cvt_reply_msg (&cvt_obj, p);
    break;

  case UBS_SIG_GET_FULL_MSG:
    ubs_cvt_get_full_msg (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_FULL_MSG:
    ubs_cvt_reply_full_msg (&cvt_obj, p);
    break;

  case UBS_SIG_DELETE_MSG:
    ubs_cvt_delete_msg (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_DELETE_MSG:
    ubs_cvt_reply_delete_msg (&cvt_obj, p);
    break;

  case UBS_SIG_NOTIFY_MSG_DELETED:
    ubs_cvt_notify_msg_deleted (&cvt_obj, p);
    break;

  case UBS_SIG_CHANGE_MSG:
    ubs_cvt_change_msg (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_CHANGE_MSG:
    ubs_cvt_reply_change_msg (&cvt_obj, p);
    break;

  case UBS_SIG_NOTIFY_MSG_CHANGED:
    ubs_cvt_notify_msg_changed (&cvt_obj, p);
    break;

  case UBS_SIG_NOTIFY_NEW_MSG:
    ubs_cvt_notify_new_msg (&cvt_obj, p);
    break;

  case UBS_SIG_CREATE_MSG:
    ubs_cvt_create_msg (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_CREATE_MSG:
    ubs_cvt_reply_create_msg (&cvt_obj, p);
    break;

  case UBS_SIG_NOTIFY_UNREAD_MSG_COUNT:
    ubs_cvt_notify_unread_msg_count (&cvt_obj, p);
    break;

  case UBS_SIG_NOTIFY_INVALIDATE_ALL:
    ubs_cvt_notify_invalidate_all (&cvt_obj, p);
    break;

  case UBS_SIG_SET_MSG_PROPERTY:
    ubs_cvt_set_msg_property (&cvt_obj, p);
    break;

  case UBS_SIG_REPLY_SET_MSG_PROPERTY:
    ubs_cvt_reply_set_msg_property (&cvt_obj, p);
    break;
  }

  WE_MEM_FREE (module, p);
}





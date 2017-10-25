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
 * ubs_if.h
 *
 * Created by Klas Hermodsson, Mon May 19 16:56:01 2003.
 *
 * Revision  history:
 *
 * 2003-09-02 (KLHE): Added UBSif_replyChangeMsg and UBSif_notifyUnreadMsgCount.
 * 2003-09-22 (MADE): Added UBSif_notifyUnreadMsgCount
 * 2003-10-03 (KLHE): Added UBSif_notifyInvalidateAll,
 *                          UBSif_setMsgProperty and
 *                          UBSif_replySetMsgProperty.
 * 2003-10-07 (KLHE): Removed UBSif_replyChangeMsg.
 * 2003-10-28 (KLHE): Made UBS_MSG_KEY_TO a title property.
 * 2003-11-20 (KLHE): Added the define UBS_NBR_OF_SYSTEM_KEYS.
 *                          Added the include of ubs_ecfg.h which contains
 *                          the UBS_NBR_OF_USER_KEYS and the UBS_USER_TITLE_PROPERTY_ARRAY 
 *                          definition.
 * 2004-01-16 (KOGR): CR 13932: key UBS_MSG_KEY_STORAGE added
 * 2004-01-21 (KLHE): CR 13932: Added values for UBS_MSG_KEY_STORAGE as
 *                          UBS_MSG_KEY_VALUE_STORAGE_x.
 * 2004-01-22 (KLHE): Renamed UBS_MSG_KEY_READ_REPORT_REQUESTED to 
 *                          UBS_MSG_KEY_REPORT_STATUS and changed the type to UINT32.
 * 2004-02-16 (KOGR): CR 13636: key UBS_MSG_KEY_NBR_OF_MESSAGES_WAITING added
 * 2004-02-23 (KOGR): CR 13934/13626: added UBS_MSG_KEY_MSG_STATUS,
 *                    UBS_MSG_KEY_VALUE_MSG_STATUS_OK, UBS_MSG_KEY_VALUE_MSG_STATUS_INCOMPLETE
 * 2005-01-21 (HUZH): TR18100 and TR 17904: added UBS_MSG_KEY_CLASS
 */

#ifndef UBS_IF_H
#define UBS_IF_H

#ifndef _we_cfg_h
  #include "We_Cfg.h"
#endif

#ifndef _we_def_h
  #include "We_Def.h"
#endif

#ifndef _we_dcvt_h
  #include "We_Dcvt.h"
#endif

#ifndef UBS_ECFG_H
  #include "Ubs_Ecfg.h"
#endif

/**********************************************************************
 * Signals in the Unified Inbox API:
 **********************************************************************/

#define UBS_SIG_REGISTER                ((WE_MODID_UBS << 8) + 0x00)
#define UBS_SIG_REPLY_REGISTER          ((WE_MODID_UBS << 8) + 0x01)
#define UBS_SIG_DEREGISTER              ((WE_MODID_UBS << 8) + 0x02)
#define UBS_SIG_GET_NBR_OF_MSGS         ((WE_MODID_UBS << 8) + 0x03)
#define UBS_SIG_REPLY_NBR_OF_MSGS       ((WE_MODID_UBS << 8) + 0x04)
#define UBS_SIG_GET_MSG_LIST            ((WE_MODID_UBS << 8) + 0x05)
#define UBS_SIG_REPLY_MSG_LIST          ((WE_MODID_UBS << 8) + 0x06)
#define UBS_SIG_GET_MSG                 ((WE_MODID_UBS << 8) + 0x07)
#define UBS_SIG_REPLY_MSG               ((WE_MODID_UBS << 8) + 0x08)
#define UBS_SIG_GET_FULL_MSG            ((WE_MODID_UBS << 8) + 0x09)
#define UBS_SIG_REPLY_FULL_MSG          ((WE_MODID_UBS << 8) + 0x0a)
#define UBS_SIG_DELETE_MSG              ((WE_MODID_UBS << 8) + 0x0b)
#define UBS_SIG_REPLY_DELETE_MSG        ((WE_MODID_UBS << 8) + 0x0c)
#define UBS_SIG_NOTIFY_MSG_DELETED      ((WE_MODID_UBS << 8) + 0x0d)
#define UBS_SIG_CHANGE_MSG              ((WE_MODID_UBS << 8) + 0x0e)
#define UBS_SIG_REPLY_CHANGE_MSG        ((WE_MODID_UBS << 8) + 0x0f)
#define UBS_SIG_NOTIFY_MSG_CHANGED      ((WE_MODID_UBS << 8) + 0x10)
#define UBS_SIG_NOTIFY_NEW_MSG          ((WE_MODID_UBS << 8) + 0x11)
#define UBS_SIG_CREATE_MSG              ((WE_MODID_UBS << 8) + 0x12)
#define UBS_SIG_REPLY_CREATE_MSG        ((WE_MODID_UBS << 8) + 0x13)
#define UBS_SIG_NOTIFY_UNREAD_MSG_COUNT ((WE_MODID_UBS << 8) + 0x14)
#define UBS_SIG_NOTIFY_INVALIDATE_ALL   ((WE_MODID_UBS << 8) + 0x15)
#define UBS_SIG_SET_MSG_PROPERTY        ((WE_MODID_UBS << 8) + 0x16)
#define UBS_SIG_REPLY_SET_MSG_PROPERTY  ((WE_MODID_UBS << 8) + 0x17)

/**********************************************************************
 * Other constants:
 **********************************************************************/

#define UBS_NBR_OF_MSG_TYPES          (UBS_NBR_OF_SYSTEM_MSG_TYPES + UBS_NBR_OF_USER_MSG_TYPES)

#define UBS_NBR_OF_SYSTEM_MSG_TYPES   5
/* Message types */
#define UBS_MSG_TYPE_PUSH_SI          0x00
#define UBS_MSG_TYPE_PUSH_SL          0x01
#define UBS_MSG_TYPE_MMS              0x02
#define UBS_MSG_TYPE_EMS              0x03
#define UBS_MSG_TYPE_EMAIL            0x04

/* UBS usage roles */
#define UBS_ROLE_VIEWER               0x01
#define UBS_ROLE_OWNER                0x02 
#define UBS_ROLE_EDITOR               0x03

/* Result codes */
#define UBS_RESULT_SUCCESS            0x01
#define UBS_RESULT_FAILURE            0x02

/* Value types */
#define UBS_VALUE_TYPE_UINT32         0x00
#define UBS_VALUE_TYPE_INT32          0x01
#define UBS_VALUE_TYPE_UTF8           0x02
#define UBS_VALUE_TYPE_BOOL           0x03
#define UBS_VALUE_TYPE_BINARY         0x04

/* Max signal size (in bytes). Used to ensure that replyMsgList doesn't get too big */
#define UBS_MAX_SIGNAL_SIZE                   2000


/* internal folder names (values for UBS_MSG_KEY_FOLDER) */
#define UBS_MSG_KEY_VALUE_FOLDER_INBOX        "i"
#define UBS_MSG_KEY_VALUE_FOLDER_DRAFTS       "d"
#define UBS_MSG_KEY_VALUE_FOLDER_SENT         "s"
#define UBS_MSG_KEY_VALUE_FOLDER_OUTBOX       "o"
#define UBS_MSG_KEY_VALUE_FOLDER_TEMPLATES    "t"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_0   "0"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_1   "1"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_2   "2"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_3   "3"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_4   "4"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_5   "5"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_6   "6"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_7   "7"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_8   "8" 
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_9   "9"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_10  "10"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_11  "11"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_12  "12"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_13  "13"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_14  "14"
#define UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_15  "15"

/* Priority types (values for UBS_MSG_KEY_PRIORITY) */
#define UBS_MSG_KEY_VALUE_PRIORITY_LOW        0
#define UBS_MSG_KEY_VALUE_PRIORITY_NORMAL     1
#define UBS_MSG_KEY_VALUE_PRIORITY_HIGH       2

/* Storage location (values for UBS_MSG_KEY_STORAGE) */
#define UBS_MSG_KEY_VALUE_STORAGE_LOCAL       0
#define UBS_MSG_KEY_VALUE_STORAGE_SIM         1

/* Message status (values for UBS_MSG_KEY_MSG_STATUS) */
#define UBS_MSG_KEY_VALUE_MSG_STATUS_OK          0
#define UBS_MSG_KEY_VALUE_MSG_STATUS_INCOMPLETE  1

#define UBS_NBR_OF_KEYS                       (UBS_NBR_OF_SYSTEM_KEYS + UBS_NBR_OF_USER_KEYS)
#define UBS_NBR_OF_SYSTEM_KEYS                18

/* Key types */
#define UBS_MSG_KEY_FOLDER                    0     /* UBS_VALUE_TYPE_UTF8,
                                                        see UBS_MSG_KEY_VALUE_FOLDER_x */
#define UBS_MSG_KEY_SUBTYPE                   1     /* UBS_VALUE_TYPE_UINT32 */     
#define UBS_MSG_KEY_SUBJECT                   2     /* UBS_VALUE_TYPE_UTF8 */
#define UBS_MSG_KEY_FROM                      3     /* UBS_VALUE_TYPE_UTF8 */
#define UBS_MSG_KEY_TIMESTAMP                 4     /* UBS_VALUE_TYPE_UINT32, 
                                                        nbr of seconds since 1970 */
#define UBS_MSG_KEY_READ                      5     /* UBS_VALUE_TYPE_BOOL */
#define UBS_MSG_KEY_FORWARD_LOCK              6     /* UBS_VALUE_TYPE_BOOL */
#define UBS_MSG_KEY_PRIORITY                  7     /* UBS_VALUE_TYPE_UINT32, 
                                                        see UBS_MSG_KEY_VALUE_PRIORITY_x */
#define UBS_MSG_KEY_TO                        8     /* UBS_VALUE_TYPE_UTF8 */
#define UBS_MSG_KEY_CREATED                   9     /* UBS_VALUE_TYPE_UINT32, 
                                                        nbr of seconds since 1970 */
#define UBS_MSG_KEY_EXPIRES                   10    /* UBS_VALUE_TYPE_UINT32, 
                                                        nbr of seconds since 1970 */
#define UBS_MSG_KEY_NBR_OF_ATTACHMENTS        11    /* UBS_VALUE_TYPE_UINT32 */
#define UBS_MSG_KEY_REPORT_STATUS             12    /* UBS_VALUE_TYPE_UINT32 */
#define UBS_MSG_KEY_URL                       13    /* UBS_VALUE_TYPE_UTF8 */
#define UBS_MSG_KEY_SIZE                      14    /* UBS_VALUE_TYPE_UINT32 */
#define UBS_MSG_KEY_STORAGE                   15    /* UBS_VALUE_TYPE_UINT32,
                                                        see UBS_MSG_KEY_VALUE_STORAGE_x */
#define UBS_MSG_KEY_NBR_OF_MESSAGES_WAITING   16    /* UBS_VALUE_TYPE_UINT32 */
#define UBS_MSG_KEY_MSG_STATUS                17    /* UBS_VALUE_TYPE_UINT32 */
#define UBS_MSG_KEY_CLASS                     18    /* UBS_VALUE_TYPE_UINT32 TR 18100 and TR 17904*/

/**********************************************************************
 * UBS error codes to be used with the adapter function UBSa_error.
 **********************************************************************/
typedef enum
{
    /* The result codes in this enum shall be held in alphabetic order,
     * except OK that shall be the first and hence has a value equal to 0.
     */
    
    /*! OK */
    UBS_RESULT_OK,    

    /*! Error reading file. */
    UBS_RESULT_FILE_READ_ERROR,

    /*! Error when trying write to file */
    UBS_RESULT_FILE_WRITE_ERROR,

    /*! Index file is corrupt or does not exist. */
    UBS_RESULT_INDEX_FILE_ERROR,

    /*! Index file is to big */
    UBS_RESULT_INDEX_FILE_MAX_SIZE_ERROR,
    
    /*! Not enough memory for the requested operation. */
    UBS_RESULT_INSUFFICIENT_MEMORY_ERROR, 

    /*! Not enough persistent storage for the requested operation. */
    UBS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE_ERROR,

    /*! Number of messages exceeds maximum limit. */
    UBS_RESULT_NBR_OF_MESSAGES_EXCEEDS_MAX_ERROR
} ubs_result_code_t;

/*
 *  UBS_TITLE_PROPERTY_ARRAY defines an array where the key defines above is the index 
 *  into the array.
 *
 *  The first param is TRUE if the property with this index is to be included
 *  in the title list.
 *  The second param is a max length. A value of zero msans that there is no specified
 *  max length. Max length can only be used on keys where its value is a UTF8 string.
 *  Then max length msans the maximum number of characters in ther string.
 *  All strings which exceed this length SHOULD be truncated to the specified max length.
 *
 *  Example:
 *  The following line msans a key that is to be put in the title list and has a max length
 *  of 20 characters.
 *      {TRUE, 20}, \
 *
 *  Look in ubs_ecfg.h for the UBS_USER_TITLE_PROPERTY_ARRAY definition
 */

typedef struct {
  int isTitleProperty;
  int maxLen;
} ubs_title_property_array_element_t;

#define UBS_TITLE_PROPERTY_ARRAY UBS_USER_TITLE_PROPERTY_ARRAY

/**********************************************************************
 * Type Definitions:
 **********************************************************************/

/* unique message identifier (within its msgType) */
typedef struct {
  WE_UINT16            handleLen;
  unsigned char         *handle;  /* binary message WID for which UBS and viewers of 
                                  these messages does not know its contents or context */
} ubs_msg_handle_t;

/* A key-value pair placeholder */
typedef struct {
  WE_UINT16            key;
  WE_UINT8             valueType;
  WE_UINT16            valueLen;
  unsigned char         *value;
} ubs_key_value_t;

/* A representation of a message with enough info to visually represent 
the message in a list */
typedef struct {
  ubs_msg_handle_t      handle;
  WE_UINT16            titleListSize;
  ubs_key_value_t       *titleList;
} ubs_msg_list_item_t;

/* A placeholder for a message's second level of properties 
(e.g. needed to visualize "view properties") */
typedef struct {
  ubs_msg_handle_t      handle;
  WE_UINT16            propertyListSize;
  ubs_key_value_t       *propertyList;
} ubs_msg_t;

/* A placeholder for a complete message as needed when UBS is taking 
care of storage of messages of a certain msgType */
typedef struct {
  ubs_msg_handle_t      handle;
  WE_UINT16            propertyListSize;
  ubs_key_value_t       *propertyList;
  WE_UINT16            hiddenPropertyListSize;
  ubs_key_value_t       *hiddenPropertyList; /* meta information that is needed to 
                                              parse/understand/modify data (see below) */
  WE_UINT32            dataLen;
  unsigned char         *data; /* the actual message in its binary form */
} ubs_full_msg_t;


/**********************************************************************
 * Signals:
 **********************************************************************/

/* For the signal UBS_SIG_REGISTER              */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
} ubs_register_t;

/* For the signal UBS_SIG_REPLY_REGISTER        */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_INT8              result;
} ubs_reply_register_t;

/* For the signal UBS_SIG_DEREGISTER            */
typedef struct {
  WE_UINT8             msgType;
} ubs_deregister_t;

/* For the signal UBS_SIG_GET_NBR_OF_MSGS       */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_UINT16            filterListSize;
  ubs_key_value_t       *filterList;
} ubs_get_nbr_of_msgs_t;

/* For the signal UBS_SIG_REPLY_NBR_OF_MSGS     */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_UINT16            nbrOfMsgs;
} ubs_reply_nbr_of_msgs_t;

/* For the signal UBS_SIG_GET_MSG_LIST          */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_UINT16            filterListSize;
  ubs_key_value_t       *filterList;
  WE_UINT16            startFromMsg;
} ubs_get_msg_list_t;

/* For the signal UBS_SIG_REPLY_MSG_LIST        */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_UINT16            msgListSize;
  ubs_msg_list_item_t   *msgList;
  WE_UINT16            nextMsg;
} ubs_reply_msg_list_t;

/* For the signal UBS_SIG_GET_MSG               */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
} ubs_get_msg_t;

/* For the signal UBS_SIG_REPLY_MSG             */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_INT8              result;
  ubs_msg_t             msg;
} ubs_reply_msg_t;

/* For the signal UBS_SIG_GET_FULL_MSG          */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
  WE_BOOL              getPropertyList;
  WE_BOOL              getHiddenPropertyList;
  WE_BOOL              getData;
} ubs_get_full_msg_t;

/* For the signal UBS_SIG_REPLY_FULL_MSG        */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_INT8              result;
  ubs_full_msg_t        msg;
} ubs_reply_full_msg_t;

/* For the signal UBS_SIG_DELETE_MSG            */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
} ubs_delete_msg_t;

/* For the signal UBS_SIG_REPLY_DELETE_MSG      */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
  WE_INT8              result;
} ubs_reply_delete_msg_t;

/* For the signal UBS_SIG_NOTIFY_MSG_DELETED    */
typedef struct {
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
} ubs_notify_msg_deleted_t;

/* For the signal UBS_SIG_CHANGE_MSG            */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  WE_BOOL              changePropertyList;
  WE_BOOL              changeHiddenPropertyList;
  WE_BOOL              changeData;
  ubs_full_msg_t        msg;
} ubs_change_msg_t;

/* For the signal UBS_SIG_REPLY_CHANGE_MSG      */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
  WE_INT8              result;
} ubs_reply_change_msg_t;

/* For the signal UBS_SIG_NOTIFY_MSG_CHANGED    */
typedef struct {
  WE_UINT8             msgType;
  ubs_msg_list_item_t   msg;
} ubs_notify_msg_changed_t;

/* For the signal UBS_SIG_NOTIFY_NEW_MSG        */
typedef struct {
  WE_UINT8             msgType;
  ubs_msg_list_item_t   msg;
} ubs_notify_new_msg_t;

/* For the signal UBS_SIG_CREATE_MSG            */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_full_msg_t        msg;
} ubs_create_msg_t;

/* For the signal UBS_SIG_REPLY_CREATE_MSG            */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
  WE_INT8              result;
} ubs_reply_create_msg_t;

/* For the signal UBS_SIG_UNREAD_MSGS_NOTIFICATION */
typedef struct {
  WE_UINT8      msgType;
  WE_UINT16     nbrOfUnreadMsgs;
} ubs_notify_unread_msg_count_t;

/* For the signal UBS_SIG_NOTIFY_INVALIDATE_ALL */
typedef struct {
  WE_UINT8      msgType;
} ubs_notify_invalidate_all_t;

/* For the signal UBS_SIG_SET_MSG_PROPERTY */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
  ubs_key_value_t       propertyValue;
} ubs_set_msg_property_t;


/* For the signal UBS_SIG_REPLY_SET_MSG_PROPERTY */
typedef struct {
  WE_UINT16            transactionId;
  WE_UINT8             msgType;
  ubs_msg_handle_t      handle;
  WE_INT8              result;
} ubs_reply_set_msg_property_t;

/*
 * Called by any other module that wants to use the UBS module.
 */
void 
UBSif_startInterface (void);

/**********************************************************************
 * Signal-sending API
 * These functions should be used by any other module that wants to
 * send a signal to the UBS module.
 **********************************************************************/

/*********************************************************************/
/* Functions for modules that hold the viewer role                   */
/*********************************************************************/

/*
WE_UINT8 src, 
WE_UINT8 msgType
*/
void 
UBSif_register (int src, int msgType);

/*
WE_UINT8 src, 
WE_UINT8 msgType
*/
void 
UBSif_deregister (int src, int msgType);


/*********************************************************************/
/* Functions for modules that hold the viewer or editor role         */
/*********************************************************************/

/*
WE_UINT8 src,
WE_UINT16 transactionId,
WE_UINT8 msgType, 
WE_UINT16 filterListSize, 
ubs_key_value_t *filterList
*/
void 
UBSif_getNbrOfMsgs (int src,
                        unsigned transactionId,
                        int msgType, 
                        unsigned filterListSize, 
                        ubs_key_value_t *filterList);

/*
WE_UINT8 src,
WE_UINT16 transactionId,
WE_UINT8 msgType, 
WE_UINT16 filterListSize, 
ubs_key_value_t *filterList
WE_UINT16 startFromMsg
*/
void 
UBSif_getMsgList (int src,
                      unsigned transactionId,
                      int msgType, 
                      unsigned filterListSize, 
                      ubs_key_value_t *filterList,
                      unsigned startFromMsg);

/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType, 
ubs_msg_handle_t *handle
*/
void 
UBSif_getMsg (int src, 
                  unsigned transactionId,
                  int msgType, 
                  ubs_msg_handle_t *handle);

/*
WE_UINT8 src,
WE_UINT16 transactionId,
WE_UINT8 msgType,
ubs_msg_handle_t *handle
*/
void 
UBSif_deleteMsg (int src,
                     unsigned transactionId,
                     int msgType,
                     ubs_msg_handle_t *handle);

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
                      ubs_key_value_t *propertyValue);


/*********************************************************************/
/* Functions for modules that hold the owner role                    */
/*********************************************************************/

/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType,
WE_INT8 result
*/
void
UBSif_replyRegister (int src, 
                        unsigned transactionId,
                        int msgType,
                        int result);

/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType,
WE_UINT16 nbrOfMsgs,
*/
void 
UBSif_replyNbrOfMsgs (int src, 
                          unsigned transactionId,
                          int msgType,
                          unsigned nbrOfMsgs);

/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType,
WE_UINT16 msgListSize,
ubs_msg_list_item_t *msgList,
WE_UINT16 nextMsg
*/
void 
UBSif_replyMsgList (int src, 
                        unsigned transactionId,
                        int msgType,
                        unsigned msgListSize,
                        ubs_msg_list_item_t *msgList,
                        unsigned nextMsg);

/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType,
WE_INT8 result,
ubs_msg_t *msg
*/
void 
UBSif_replyMsg (int src, 
                    unsigned transactionId,
                    int msgType,
                    int result,
                    ubs_msg_t *msg);

/*
WE_UINT8 src,
WE_UINT16 transactionId,
WE_UINT8 msgType,
ubs_msg_handle_t *handle,
WE_INT8 result
*/
void 
UBSif_replyDeleteMsg (int src,
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result);

/*
WE_UINT8 src, 
WE_UINT8 msgType,
ubs_msg_handle_t *handle
*/
void 
UBSif_notifyMsgDeleted (int src, 
                            int msgType,
                            ubs_msg_handle_t *handle);

/*
WE_UINT8 src, 
WE_UINT8 msgType,
ubs_msg_list_item_t *msg
*/
void 
UBSif_notifyMsgChanged (int src, 
                            int msgType,
                            ubs_msg_list_item_t *msg);

/*
WE_UINT8 src, 
WE_UINT8 msgType,
ubs_msg_list_item_t *msg
*/
void 
UBSif_notifyNewMsg (int src, 
                        int msgType,
                        ubs_msg_list_item_t *msg);

/*
 *  The following interface function must be used by any module acting as owner of 
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
                            unsigned nbrOfUnreadMsgs);

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
UBSif_notifyInvalidateAll (int src, int msgType);


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
                          int result);


/*********************************************************************/
/* Functions for modules that hold the editor role                   */
/*********************************************************************/

/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType, 
ubs_msg_handle_t *handle, 
WE_BOOL getPropertyList, 
WE_BOOL getHiddenPropertyList, 
WE_BOOL getData
*/
void 
UBSif_getFullMsg (int src, 
                      unsigned transactionId,
                      int msgType, 
                      ubs_msg_handle_t *handle, 
                      int getPropertyList, 
                      int getHiddenPropertyList, 
                      int getData);


/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType, 
WE_BOOL changePropertyList,
WE_BOOL changeHiddenPropertyList, 
WE_BOOL changeData, 
ubs_full_msg_t *msg
*/
void 
UBSif_changeMsg (int src, 
                     unsigned transactionId,
                     int msgType, 
                     int changePropertyList,
                     int changeHiddenPropertyList, 
                     int changeData, 
                     ubs_full_msg_t *msg);


/*
WE_UINT8 src, 
WE_UINT16 transactionId,
WE_UINT8 msgType, 
ubs_full_msg_t *msg
*/
void 
UBSif_createMsg (int src, 
                     unsigned transactionId,
                     int msgType, 
                     ubs_full_msg_t *msg);




/**********************************************************************
 * Conversion routines for all signal owned by this module.
 * None of these functions should be called directly by another module.
 **********************************************************************/


/*
 *  UBS convert functions
 */

int 
ubs_cvt_register (we_dcvt_t *obj, ubs_register_t *p);

int 
ubs_cvt_reply_register (we_dcvt_t *obj, ubs_reply_register_t *p);

int 
ubs_cvt_deregister (we_dcvt_t *obj, ubs_deregister_t *p);

int 
ubs_cvt_get_nbr_of_msgs (we_dcvt_t *obj, ubs_get_nbr_of_msgs_t *p);

int 
ubs_cvt_reply_nbr_of_msgs (we_dcvt_t *obj, ubs_reply_nbr_of_msgs_t *p);

int 
ubs_cvt_get_msg_list (we_dcvt_t *obj, ubs_get_msg_list_t *p);

int 
ubs_cvt_reply_msg_list (we_dcvt_t *obj, ubs_reply_msg_list_t *p);

int 
ubs_cvt_get_msg (we_dcvt_t *obj, ubs_get_msg_t *p);

int 
ubs_cvt_reply_msg (we_dcvt_t *obj, ubs_reply_msg_t *p);

int 
ubs_cvt_get_full_msg (we_dcvt_t *obj, ubs_get_full_msg_t *p);

int 
ubs_cvt_reply_full_msg (we_dcvt_t *obj, ubs_reply_full_msg_t *p);

int 
ubs_cvt_delete_msg (we_dcvt_t *obj, ubs_delete_msg_t *p);

int 
ubs_cvt_reply_delete_msg (we_dcvt_t *obj, ubs_reply_delete_msg_t *p);

int 
ubs_cvt_notify_msg_deleted (we_dcvt_t *obj, ubs_notify_msg_deleted_t *p);

int 
ubs_cvt_change_msg (we_dcvt_t *obj, ubs_change_msg_t *p);

int 
ubs_cvt_reply_change_msg (we_dcvt_t *obj, ubs_reply_change_msg_t *p);

int 
ubs_cvt_notify_msg_changed (we_dcvt_t *obj, ubs_notify_msg_changed_t *p);

int 
ubs_cvt_notify_new_msg (we_dcvt_t *obj, ubs_notify_new_msg_t *p);

int 
ubs_cvt_create_msg (we_dcvt_t *obj, ubs_create_msg_t *p);
 
int 
ubs_cvt_reply_create_msg (we_dcvt_t *obj, ubs_reply_create_msg_t *p);

int
ubs_cvt_notify_unread_msg_count (we_dcvt_t *obj, ubs_notify_unread_msg_count_t *p);

int
ubs_cvt_notify_invalidate_all (we_dcvt_t *obj, ubs_notify_invalidate_all_t *p);

int
ubs_cvt_set_msg_property (we_dcvt_t *obj, ubs_set_msg_property_t *p);

int
ubs_cvt_reply_set_msg_property (we_dcvt_t *obj, ubs_reply_set_msg_property_t *p);

/*
 * Convert from memory buffer for signal structs.
 */
void* 
ubs_convert (WE_UINT8 module, WE_UINT16 signal, void* buffer);

/*
 * Released a signal structure of a signal.
 */
void 
ubs_destruct (WE_UINT8 module, WE_UINT16 signal, void* p);

#endif    /*UBS_IF_H*/

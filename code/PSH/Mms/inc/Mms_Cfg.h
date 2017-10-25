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
 
/*! \file Mms_Cfg.h
 *  \brief  Configuration definitions for the MMS Service. Defines with names 
 *          starting with MMS_DEFAULT is used as the default value for 
 *          configurations settings in the registry. Once the corresponding 
 *          setting in the registry has got a value, the registry setting will 
 *          be used.
 */

#ifndef MMS_CFG_H
#define MMS_CFG_H

#ifndef WE_CFG_H
#include "We_Cfg.h"
#endif

/****************************************************************************
 * Change, comment or uncomment the defines below in order to alter the 
 * behaviour and/or functionality of the MMS Client.
 ****************************************************************************/
/*!
 * This define is used to include the optional MMS_CUSTOM_DATA_SPEC because of HUAWEI project
 *
 * Comment the below define to leave out MMS_CUSTOM_DATA_SPEC.
 */
 /*#define MMS_CUSTOMDATA_IN_USE 0*/


/*!
 * This define is used to include the optional Unified-inbox
 * functionallity into the MMS service code.
 *
 * Comment the below define to leave out Unified-inbox functions.
 */
#ifdef WE_MODID_UBS
#define MMS_UBS_IN_USE 0
#endif


 
 /*! Should a notifyresp.ind with reject be sent if a notification is deleted by the end-user ?
 * 1= notifyresp.ind is sent.
 * 2= no notifyresp.ind is sent.
 */
 
#define  MMS_EXTENDED_NOTIFYRESPIND_HANDLING 0
 
 /*! Setting used in WE_MEM_INIT to specify memory allocation method.
 */
#define MMS_MEM_INIT_EXT FALSE

/*! Time in tenth of seconds before the MMS Service terminates outstanding 
 *  file operations. This setting is used during termination of the MMS Service.
 *  The time is given in tenth of seconds so a value of 50 msans 5 seconds.
 */
#define MMS_TERMINATE_FILE_TIMEOUT  50

/*! Defines the channel WID that the MMS Service uses towards the protocol 
 *  stack. */
#define MMS_STK_CHANNEL_ID 0

/*! Defines the connection type that the MMS Service uses towards the protocol 
 *  stack. This value is the default value for the registry setting 
 *  #MMS_REG_KEY_STK_CONNECTION_TYPE. */
#define MMS_DEFAULT_STK_CONNECTION_TYPE STK_CONNECTION_TYPE_HTTP

/*! Defines the name of the MMS Client as presented to the MMSC in the HTTP 
 *  header User-Agent. As long as no name is given, the HTTP header User-Agent
 *  will not be used. This value is the default value for the registry setting 
 *  #MMS_REG_KEY_USER_AGENT. */
#define MMS_DEFAULT_USER_AGENT ""

/*! Defines the network account that the MMS Service should use. 
 *  This value is the default value for the registry setting 
 *  #MMS_REG_KEY_NETWORK_ACCOUNT. */
#define MMS_DEFAULT_NETWORK_ACCOUNT 0

/*! Defines the scheme used to send messages to the MMS Proxy-Relay server. 
 *  This value is the default value for the registry setting 
 *  #MMS_REG_KEY_PROXY_RELAY_SCHEME. */
#define MMS_DEFAULT_PROXY_RELAY_SCHEME     "http://"

/*! Defines the MMS Proxy-Relay server to send messages to. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_PROXY_RELAY. The MMS 
 *  Proxy-Relay can be specified in the form of an IP 4 address, e.g. 
 *  "127.0.0.1". */
#define MMS_DEFAULT_PROXY_RELAY             "mmsc"

/*! Port on the MMS Proxy-Relay to use. This value is the default value for
 *  the registry setting #MMS_REG_KEY_PORT. 
 *  If set to zero the default scheme port is used.
 */
#define MMS_DEFAULT_PORT                    0

/*! URI for POST operations towards the MMS Proxy-Relay. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_POST_URI. */
#define MMS_DEFAULT_POST_URI                "/"

/*! Defines if a WAP Gateway will be used. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_GW_ADDRESS. */
#define MMS_DEFAULT_GW_ADDRESS              NULL

/*! HTTP port of the WAP Gateway. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_GW_HTTP_PORT. */
#define MMS_DEFAULT_GW_HTTP_PORT            0

/*! Password used to log in to the WAP Gateway. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_GW_PASSWORD. */
#define MMS_DEFAULT_GW_PASSWORD             NULL

/*! Realm used for HTTP authentication at the WAP Gateway. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_GW_REALM. */
#define MMS_DEFAULT_GW_REALM                NULL

/*! Secure port at the WAP Gateway. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_GW_SECURE_PORT. */
#define MMS_DEFAULT_GW_SECURE_PORT          0

/*! Username used to log in to the WAP Gateway. This value is the 
 *  default value for the registry setting #MMS_REG_KEY_GW_USERNAME. */
#define MMS_DEFAULT_GW_USERNAME             NULL

 /*! Defines additional HTTP headers that can be appended to every POST and
 *  GET operation towards the MMSC. If a value is set, it will be appended 
 *  as a HTTP header for all GET and POST operations. 
 *
 *  Default value for the registry setting #MMS_REG_KEY_CLIENT_HTTP_HEADER
 *
 *  These additional HTTP headers can be used for example for the X-MSISDN
 *  header that some MMSCs need. Other MMSCs instead need the HTTP header
 *  X-NOKIA-MSISDN or an equivalent header to identify the handset. 
 *
 *  So this configuration variable can for example have values like:
 *      "X-MSISDN:+46701234567"
 *      "X-NOKIA-MSISDN:+46701234567"
 *      "X-YOUR_COMPANY_NAME-MDN:1234567890"
 *      "X-MY-HEADER-1:xyz\nX-MY-HEADER-2:abc\nX-MY-HEADER-3:def"
*/
#define MMS_DEFAULT_CLIENT_HTTP_HEADER     NULL

/*! We accept data with all headers although we could restrict it to be 
*  "application/vnd.wap.mms-message". By accepting all headers we could
*  receive wrong data but since we do sanity checks of the PDUs, it won't
*  cause any harm. Instead we get better control of the data in the MMS 
*  Client which also can be used during debugging. In order to minimze 
*  the amount of data passed to the MMS Client (I don't know if it can 
*  happen), change these defines to "applic..." as described above. */
#define MMS_GET_HTTP_HEADER  "Accept: */*\n"
#define MMS_POST_HTTP_HEADER "Accept: */*\n" \
"Content-Type: " MMS_CONTENT "\n\n"

/*! Number of attempts before a GET operation is considered to have failed. */
#define MMS_GET_RETRY               3

/*! Number of retries before a POST operation is considered to have failed. */
#define MMS_POST_RETRY              3

/*! Number of retries before a POST operation is considered to have failed for MMS_M_SEND_REQ. */
#define MMS_POST_MSG_RETRY              3


/*! Time before a GET operation is considered to have timed out. The time
 *  is given in tenth of seconds so a value of 300 msans 30 seconds.
 */
#define MMS_GET_TIMEOUT             300

/*! Time before a POST operation is considered to have timed out. The time
 *  is given in tenth of seconds so a value of 300 msans 30 seconds.
 */
#define MMS_POST_TIMEOUT            300

/*! Time before a connect operation (i.e. the operation of starting 
 *  a WSP/HTTP session) is considered to have timed out. 
 *  The time is given in tenth of seconds so a value of 600 msans 60 seconds.
 */
#define MMS_CONNECT_TIMEOUT         600

/*! Defines if the MMS Client will disconnect from the network when idle.
 *  This value is the default value for the registry setting
 *  #MMS_REG_KEY_DISCONNECT_ON_IDLE.
 */
#define MMS_DEFAULT_DISCONNECT_ON_IDLE      FALSE

/*! Time in seconds before the MMS Client disconnects if the 
 *  registry setting #MMS_REG_KEY_DISCONNECT_ON_IDLE is set to TRUE.
 *  The time is given in tenth of seconds so a value of 150 msans 15 seconds.
 */
#define MMS_DISCONNECT_ON_IDLE_TIMEOUT  150

/*! This is a non-standard extension to the WAP Forum MMS Standard. 
 *  This define determines if M-NotifyResp is sent when a notification is 
 *  received.
 *
 *  This value is the default value for the registry setting
 *  #MMS_REG_KEY_NO_SMS_NOTIFY.
 *
 *  Set to FALSE Always send.
 *  Set to TRUE Only when bearer wasn't SMS.
 */
#define MMS_DEFAULT_NO_SMS_NOTIFY_RESP      FALSE

/*! The maximal size (in bytes) of messages to automatically download from the 
 *  MMS Proxy-Relay. This value is used when Immediate Retrieval is active and
 *  it is the default value for the registry setting 
 *  #MMS_REG_KEY_MAX_SIZE_OF_MSG. 
 */
#define MMS_DEFAULT_MAX_MSG_SIZE        32000

 /*! During message retrieval, defines the maximum size for a message. If 
 *  this limit is reached during delayed retrieval, the download is stopped
 *  and MMS_RESULT_MAX_RETRIEVAL_SIZE_EXCEEDED is returned in the call to 
 *  MMSa_newMessage. If this limit is reached during immediate retrieval,
 *  the download is stopped and MMS_RESULT_MAX_RETRIEVAL_SIZE_EXCEEDED 
 *  is returned in the call to MMSa_newMessage.
 *  This value is the default value for the 
*  #MMS_CFG_MAX_RETRIEVAL_SIZE configuration variable. */
#define MMS_DEFAULT_MAX_RETRIEVAL_SIZE   0

/*! Default roaming status.
 * Only used when MMS_ROAMING_USE_REGISTRY is defined
 */
#define MMS_DEFAULT_ROAMING_STATUS         MMS_NOT_ROAMING  

/*! Default network bearer availablilty status.
 */
#define MMS_DEFAULT_BEARER_STATUS          MMS_BEARER_AVAILABLE  

/*! Default retrieval method while in roaming.
 */
#define MMS_DEFAULT_ROAMING_MODE           MMS_ROAM_DELAYED


/*! Defines the maximum numbers of messages that can be stored in the 
 *  MMS folders.
 */
#define MMS_MAX_NO_OF_MSG       32

/*! Defines if an alternative subject text should be stored in the info file
 *  for received messages with an empty subject field. The alternative subject
 *  is created from text (TEXT_PLAIN) found in the body of message or entries.
 *  If no text is found the subject text will be left empty. Max 40 characters
 *  (MAX_MMT_SUBJECT_LEN) is copied as subject text.
 *
 *  Value 0 => Do not create an alternative subject (default)
 *  Value 1 => Create an alternative subject text, if possible.
 */
#define MMS_CREATE_ALT_SUBJECT_TEXT 0

/*! MMS keeps a table with information on all messages in all folders. This 
 *  table must be stored in persistent storage. The following definitions 
 *  defines which events that will write the table to persistent storage.
 *  One or more events can be chosen. If more events are chosen, the number of
 *  "disk" operations will increase. If less events are chosen, more data can
 *  be lost in case of a malfunction. Data that may be lost are e.g. what
 *  folder a message is stored in, if it has been read, received Read Reports
 *  and Delivery Reports and the Server Message WID.
 *
 *  Message done, stored and parsed */
#define MMS_FLDRMGR_MSG_DATA_SET   1
/*! Empty message is created */
#define MMS_FLDRMGR_CREATE         0
/*! Deletion of a message */
#define MMS_FLDRMGR_DELETE         0
/*! Move a message between folder */
#define MMS_FLDRMGR_MOVE           0
/*! New notif received */
#define MMS_FLDRMGR_NOTIF          1
/*! Read.status changed */
#define MMS_FLDRMGR_READMARK       1

/*! When a read report is created, the date could either be provided by the
 *  cellular or the MMSC.
 *  The advantage of letting the MMSC providing the date is that the cellular
 *  time and date migth be set wrongly by the user.
 *
 *  - A value of 0 => Date set by MMSC
 *  - A value of 1 => Date set by Cellular
 */
#define MMS_READ_REPORT_DATE       0

/*! Setting for sending read reports as multipart mixed. A read report for
 *  MMS Version 1.0 is normally sent with content type set to text/plain,
 *  but some operators requires them to be multipart mixed.
 *
 *  0 => RR 1.0 sent as text/plain (default)
 *  1 => RR 1.0 sent as multipart mixed
 */
#define MMS_READ_REPORT_MULTIPART_MIXED  0



/*!
 * Text length related configurations. These defines determines how long
 * text fields may be. Values is number of bytes.
 *
 * Max length for one address field. */
#define MAX_ADDRESS_LEN                 128
/*! Max length for content type. */
#define MAX_CONTENT_TYPE_LEN            100
/*! Max length for message class. */
#define MAX_STR_LEN_MSG_CLASS           64
/*! Max length for subject. */
#define MAX_SUBJECT_LEN                 100
/*! Max length for Entry header values. */
#define MAX_MMS_ENTRY_HEADER_VALUE_LEN  100
/*! Max length for Server Msg Id */
#define MAX_SERVER_MSG_ID               42
/*! Max length for Transaction Id */
#define MAX_LEN_TRANSACTION_ID          41

/*!
 * Length of strings held in the Master Message Table (MMT). This table is
 * used when listing messages in folders. The entire fields will always be 
 * stored in the message, these definitions only determine how much of the 
 * field that could be shown in listings. 
 *
 * Server message wid length */
#define MAX_MMT_SERVER_MSG_ID_LEN  MAX_SERVER_MSG_ID
/*! Address length */ 
#define MAX_MMT_ADDRESS_LEN        64
/*! Subject field length */
#define MAX_MMT_SUBJECT_LEN        40

/****************************************************************************
 * Memory related configuration definitions. The defines below determines how 
 * the MMS Client uses memory. Please read the manual for a longer description 
 * of them. All sizes for the following definitions is given in bytes.
 ****************************************************************************/

/*! Defines the maximum size of a pushed notification. If the notification is 
 * larger than this value, the notification cannot be handled. A value of 
 * 300 is sufficient in most cases. */
#define MMS_MAX_NOTIFICATION_SIZE   512

/*! Defines the maximum size of a chunk. Larger data items will be stored on
 *  file. If chosen to siall, all messages cannot be handled, e.g. if the
 *  MMS header does not fit within one memory chunk. */
#define MMS_MAX_CHUNK_SIZE          4096UL

/*! Defines if incoming PDUs shall be checked for a strict conformance to the
 *  MMS standard. For example, by allowing M-Retrieve.conf to not have a date 
 *  tag (which is mandatory) a blank date field MAY be shown to the user (if 
 *  the MMSC doesn't send it). This does not cause any problems in the MMS
 *  Service but can be valuable when running towards MMSCs that are not
 *  following the standard.

 *  Possible values:1 - a strict check will be performed.
 *                  0 - non-vital fields may be missing */
#define MMS_STRICT_PDU_CHECK        0

 /*! Defines if the client should interpret on the Transaction WID or not.
 *  The defines controls if te client shall answer with AcknowledgeInd or not
 *  when the Transaction WID is equal to the last used Transaction WID
 *  Possible values:1 - a check will be performed.
 *                  0 - no check will be performed 
 */
#define MMS_STRICT_TRANSACTION_ID_CHECK 1

/*! Defines how many notifications that can be queued during
 *	start-up of the MMS service.
 */
#define MMS_NOTIF_QUEUE_SIZE 4




/*
 *	This define specifies the amount of storage reserved in the MMT per message.
 *  This size MUST by the same as in the MMS_CUSTOM_DATA_SPEC defined in mmsuicfg.h.
 */
#ifdef MMS_CUSTOMDATA_IN_USE
#define MMS_CUSTOM_DATA_SIZE (1 /*+4*/)
#endif
/*
 *	This define specifies the wid of any custom data.
 *  They are used in the MMS_CUSTOM_DATA_SPEC defined in mmsuicfg.h.
 *  Also any application getting/setting custom data will use these defines.
 */
typedef enum
{
    MMS_CUSTOM_DATA_UBS_KEY       = 0x00,
    MMS_CUSTOM_DATA_POSTCARD      = 0x01,
    MMS_CUSTOM_DATA_PROTECTED     = 0x02,
    MMS_CUSTOM_DATA_ALBUM         = 0x03
} MmsCustomDataId;

/*
 *	This define specifies the data stored in the customdata field in the MMT.
 *  NOTE: The total data amount defined below MUST be the same as the value
 *  specified in MMS_CUSTOM_DATA_SIZE.
 *	
 * NOTE: The firest field is of type MmsCustomDataId, and is defined in Mms_Cfg.h
 */
#ifdef MMS_CUSTOMDATA_IN_USE
#define MMS_CUSTOM_DATA_SPEC { \
    { MMS_CUSTOM_DATA_POSTCARD, -1, MMS_VALUE_TYPE_BOOL, 1, FALSE, 0}, \
    /* { MMS_CUSTOM_DATA_UBS_KEY, UBS_MSG_KEY_MSG_STATUS, MMS_VALUE_TYPE_UINT32, 4, FALSE, 0}, */ \
    { 0, 0, 0, 0, FALSE, 0}, /* Termination of list (size = 0) */ \
}
#endif
/*
 * internal folder name translation table (from MMS folder to UBA folder STR
 */
#define MMS_STR_FOLDERS { \
    { UBS_MSG_KEY_VALUE_FOLDER_INBOX, MMS_INBOX }, \
    { UBS_MSG_KEY_VALUE_FOLDER_INBOX, MMS_NOTIFY }, \
    { UBS_MSG_KEY_VALUE_FOLDER_DRAFTS, MMS_DRAFTS }, \
    { UBS_MSG_KEY_VALUE_FOLDER_SENT, MMS_SENT }, \
    { UBS_MSG_KEY_VALUE_FOLDER_OUTBOX, MMS_OUTBOX }, \
    { UBS_MSG_KEY_VALUE_FOLDER_TEMPLATES, MMS_TEMPLATES }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_0, MMS_USER_DEFINED_FOLDER_OFFSET }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_1, MMS_USER_DEFINED_FOLDER_OFFSET*2 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_2, MMS_USER_DEFINED_FOLDER_OFFSET*4 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_3, MMS_USER_DEFINED_FOLDER_OFFSET*8 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_4, MMS_USER_DEFINED_FOLDER_OFFSET*16 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_5, MMS_USER_DEFINED_FOLDER_OFFSET*32 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_6, MMS_USER_DEFINED_FOLDER_OFFSET*64 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_7, MMS_USER_DEFINED_FOLDER_OFFSET*128 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_8, MMS_USER_DEFINED_FOLDER_OFFSET*256 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_9, MMS_USER_DEFINED_FOLDER_OFFSET*512 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_10, MMS_USER_DEFINED_FOLDER_OFFSET*1024 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_11, MMS_USER_DEFINED_FOLDER_OFFSET*2048 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_12, MMS_USER_DEFINED_FOLDER_OFFSET*4096 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_13, MMS_USER_DEFINED_FOLDER_OFFSET*8192 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_14, MMS_USER_DEFINED_FOLDER_OFFSET*16384 }, \
    { UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_15, MMS_USER_DEFINED_FOLDER_OFFSET*32768 }, \
    { NULL, 0 }} /* Termination of table */

/*! The default setting for MSISDN if nothing is implemented in /EXTERNAL 
 *  This setting will control which token the MMS Service will send in the 
 *  From field [OMA-MMS-ENC, 7.2.14] in outgoing PDUs. 
 *  Non-empty-string => Address-present-token
 *                "" => Insert-address-token 
 */
#define MMS_CFG_MSISDN                          "+46701234567"
#endif /* MMS_CFG_H */


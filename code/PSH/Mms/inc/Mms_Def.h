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

 
/*! \file Mms_Def.h
 * \brief  Data types and definitions used in the MMS client.
 */

#ifndef MMS_DEF_H
#define MMS_DEF_H

/*--- Definitions/Declarations ---*/

/* WE types/definition not yet implemented */
#ifndef WE_EOS
    #define WE_EOS '\0'
#endif /* WE_EOS */

typedef void* MmsTransactionHandle;
typedef void* MmsMsgHandle;

/*! \enum MmsVersion MMS Standard that the MMS Client understand. 
 *  Is defined as <Major version>.<Minor version>. 
 *  The major version is coded in 3 high/most significant bits and the minor 
 *  version in the 4 lower/least significant bits. Example: 0x10 = X001.0000 = 1.0
 *  and 0x12 = X001.0010 = 1.2.
 */
typedef enum
{
    MMS_VERSION_PREVIOUS_MAJOR  = 0x0F,
    MMS_VERSION_10              = 0x10,
    MMS_VERSION_11              = 0x11,
    MMS_VERSION_NEXT_MAJOR      = 0x20
} MmsVersion; 

/*! Content type of MMS messages as seen by the WSP/HTTP layer. */
#define MMS_CONTENT             "application/vnd.wap.mms-message"

/*! WINA assigned string for MMS registered PUSH Application WID. */
#define MMS_PUSH_APPLICATION_ID "x-wap-application:mms.ua" 

/*! WINA assigned number for MMS registered PUSH Application WID. */
#define MMS_PUSH_APPLICATION_NO "4" 

/*! Denotes all message WID and is used in operations on all messages at the 
 *  same time. */
#define MMS_ALL_MESSAGES    0

/*! Define for the SMIL media type as a string that can be used in 
 * content type. 
 */
#define MMS_MEDIA_TYPE_STRING_SMIL "application/smil"

/*! Define for the AMR (sound) media type as a string 
 *  that can be used in content type. 
 */
#define MMS_MEDIA_TYPE_STRING_AMR  "audio/AMR"

/*! Content type for forward locked message according to Ericcson. */
#define MMS_MEDIA_TYPE_STRING_FORWARD_LOCKED  "application/vnd.vzw.mms.protected"


/*! Content type for forward locked message according to OMA. */
#define MMS_MEDIA_TYPE_STRING_FORWARD_LOCKED_OMA  "application/vnd.oma.drm.message"

/*  The registry is used for storing configuration variables. To access a
 *  configuration variable, both the name of the variable and the path in the
 *  registry where the key is stored is needed. Configuration variables for
 *  MMS can contain either strings or integers. The comment after each registry
 *  key indicates the type of variable. */
#define MMS_REG_PATH_CLIENT              "/MMS/client"
#define MMS_REG_KEY_USER_AGENT                  "UsrAgent"  /* String */

#define MMS_REG_PATH_COM_STK             "/MMS/com/stk"
#define MMS_REG_KEY_NETWORK_ACCOUNT             "NetAccnt"  /* Integer */
#define MMS_REG_KEY_STK_CONNECTION_TYPE         "ConnType"  /* Integer */
#define MMS_REG_KEY_DISCONNECT_ON_IDLE          "DiscIdle"  /* Integer */
#define MMS_REG_KEY_CLIENT_HTTP_HEADER          "HTTPhdr"   /* String */

#define MMS_REG_PATH_COM_MMSC            "/MMS/com/mmsc"
#define MMS_REG_KEY_PORT                        "Port"      /* Integer */
#define MMS_REG_KEY_NO_SMS_NOTIFY_RESP          "SmsNotif"  /* Integer */
#define MMS_REG_KEY_PROXY_RELAY_VERSION         "Version"   /* Integer */
#define MMS_REG_KEY_POST_URI                    "Uri"       /* String */
#define MMS_REG_KEY_PROXY_RELAY                 "Name"      /* String */
#define MMS_REG_KEY_PROXY_RELAY_SCHEME          "Scheme"    /* String */

#define MMS_REG_PATH_COM_GW              "/MMS/com/gateway"
#define MMS_REG_KEY_GW_SECURE_PORT              "SecPort"   /* Integer */
#define MMS_REG_KEY_GW_HTTP_PORT                "HttpPort"  /* Integer */
#define MMS_REG_KEY_GW_ADDRESS                  "Address"   /* String */
#define MMS_REG_KEY_GW_REALM                    "Realm"     /* String */
#define MMS_REG_KEY_GW_USERNAME                 "UserName"  /* String */
#define MMS_REG_KEY_GW_PASSWORD                 "Password"  /* String */

#define MMS_REG_PATH_REPORT              "/MMS/report"
#define MMS_REG_KEY_REPORT_ALLOWED             "ReptAlow"   /* Integer */

#define MMS_REG_PATH_REPORT_TEXT         "/MMS/report/text"
#define MMS_REG_KEY_TEXT_YOUR_MSG               "YourMsg"   /* String */
#define MMS_REG_KEY_TEXT_FROM                   "From"      /* String */
#define MMS_REG_KEY_TEXT_TO                     "To"        /* String */
#define MMS_REG_KEY_TEXT_READ                   "Read"      /* String */
#define MMS_REG_KEY_TEXT_MSGID                  "MsgId"     /* String */
#define MMS_REG_KEY_TEXT_SENT                   "Sent"      /* String */
#define MMS_REG_KEY_TEXT_WAS_READ               "WasRead"   /* String */
#define MMS_REG_KEY_TEXT_SUBJECT                "Subject"   /* String */

#define MMS_REG_PATH_ADDRESS             "/MMS/address"
#define MMS_REG_KEY_FROM_ADDRESS_TYPE           "FrAdType"  /* Integer */
#define MMS_REG_KEY_FROM_ADDRESS_INSERT_TYPE    "InsType"   /* Integer */
/*#define MMS_REG_KEY_FROM_ADDRESS                "FromAddr"*/  /* String */
#define MMS_REG_KEY_FROM_NAME                   "FromName"  /* String */

#define MMS_REG_PATH_MSISDN              "/EXTERNAL"
#define MMS_REG_KEY_FROM_ADDRESS                "MSISDN"    /* String */

#define MMS_REG_PATH_RETRIEVAL           "/MMS/retr"
#define MMS_REG_KEY_MAX_RETRIEVAL_SIZE          "MxRetrSz" /* Integer */
#define MMS_REG_KEY_RETRIEVAL_ROAM_MODE         "RoamMode" /* Integer */

#define MMS_REG_PATH_IMMEDIATE_RETRIEVAL "/MMS/ImmRetr"
#define MMS_REG_KEY_IMMEDIATE_RETRIEVAL         "ImmRetr"   /* Integer */
#define MMS_REG_KEY_MAX_SIZE_OF_MSG             "MaxMsgSz"  /* Integer */
#define MMS_REG_KEY_CLASS_PERSONAL              "Personal"  /* Integer */
#define MMS_REG_KEY_CLASS_ADVERTISEMENT         "Advert"    /* Integer */
#define MMS_REG_KEY_CLASS_INFORMATIONAL         "Inform"    /* Integer */
#define MMS_REG_KEY_CLASS_AUTO                  "Auto"      /* Integer */
#define MMS_REG_KEY_ANONYMOUS_SENDER            "AnonSend"  /* Integer */
#define MMS_REG_KEY_TREAT_AS_DELAYED_RETRIEVAL  "DelRetr"   /* Integer */
#define MMS_REG_KEY_IMMEDIATE_RETRIEVAL_SERVER  "FromMmsc"  /* String */
#define MMS_REG_KEY_CLASS_STRING                "ClassStr"  /* String */

#define MMS_REG_PATH_NETWORK_STATUS      "/MMS/NtwkStat"
#define MMS_REG_KEY_NETWORK_STATUS_ROAMING      "Roaming"   /* Integer */
#define MMS_REG_KEY_NETWORK_STATUS_BEARER       "Bearer"    /* Integer */

 /*--- Types ---*/
/*! MmsMsgId A unique message identity for a message in the MMS Client. This 
 *  identity is used for all operations on a message. */
typedef WE_UINT32 MmsMsgId;

/*! \enum MmsReadStatus Indicates if the recepient has read the MM or deleted 
 *  it without reading it.
 */
typedef enum
{
    MMS_READ_STATUS_READ                            = 128,
    MMS_READ_STATUS_DELETED_WITHOUT_BEING_READ      = 129
} MmsReadStatus;

/*! \enum MmsAddressType This definition contains all types that an address
 *  in the MMS Client can have. */
typedef enum
{
    MMS_PLMN,       /*!< PLMN type of address, e.g. +467055566622 */
    MMS_EMAIL,      /*!< E-Mail type of address, e.g. foo@bar.com */
    MMS_IPV4,       /*!< IPv4 type of address, e.g. 123.456.789.123 */
    MMS_UNSUPPORTED /*!< Unknown address type, Not to be sent */
} MmsAddressType;

/*! \emum MmsMsgType This definition is used in the MmsGetHeader struct
 *   to tell if the message was an Send Request or Retrieve.confirm 
 *   messge
 */
typedef enum 
{
    MMS_MSG_TYPE_SEND_REQ = 128,      /*!< Send request message */
    MMS_MSG_TYPE_RETRIEVE_CONF = 132  /*!< Retrieve confirm */
}MmsMsgType;

/*! \enum MmsCharset Supported character sets within the MMS Client. */
typedef enum
{
    MMS_US_ASCII    = 3,   
    MMS_UTF8        = 106,
    MMS_UCS2        = 1000,
    MMS_UTF16BE     = 1013, 
    MMS_UTF16LE     = 1014, 
    /*< UTF-16 is to be used only if the document starts with a UTF-16 BOM  */
    MMS_UTF16       = 1015, 
    MMS_ISO_8859_1  = 4,
    MMS_UNKNOWN_CHARSET = 0
} MmsCharset;

/*! \struct MmsEncodedText 
 *  \brief Contains an encoded text and its charset. Different
 *         fields in an MMS header can have this type. 
 */
typedef struct
{
    MmsCharset  charset;    /*!< Well-known character set as defined by IANA */
    char        *text;      /*!< Text buffer, must be NULL terminated */
} MmsEncodedText;

/*! \enum MmsFolderType MMS Messages are stored in different folders. This enum 
 *  contains the different types of folders available.
 */
typedef enum
{
    MMS_NO_FOLDER = 0x00,     /*!< Folder not assiged, only for internal usage. */
    MMS_INBOX  = 0x01,        /*!< Incoming messages */
    MMS_OUTBOX = 0x02,        /*!< Outgoing messages */
    MMS_SENT   = 0x04,        /*!< Successfully sent messages */
    MMS_DRAFTS = 0x08,        /*!< Messages during work */
    MMS_NOTIFY = 0x10,        /*!< Notifications */
    MMS_HIDDEN = 0x20,        /*!< Hidden notifications (immediate) */
    MMS_TEMPLATES = 0x40,     /*!< Template folder */
    MMS_USER_DEFINED_FOLDER_OFFSET = 0x80, /*!< The offset for user defined folders  */
    MMS_ALL_FOLDERS = 0xFFFF  /*!< Use this value to make a request to all folders */
} MmsFolderType;

/*! \enum MmsFileTypes Types of files that is used in the Folder Manager */
typedef enum
{
    MMS_SUFFIX_TEMPLATE         = 't',  /*!< Send request template */
    MMS_SUFFIX_IMMEDIATE        = 'q',  /*!< notification file during immediate retrieval */
    MMS_SUFFIX_INFO             = 'i',  /*!< Infofile */ 
    MMS_SUFFIX_SEND_REQ         = 's',  /*!< send request message */
    MMS_SUFFIX_MSG              = 'm',  /*!< message file */ 
    MMS_SUFFIX_NOTIFICATION     = 'n',  /*!< notification file */  
    MMS_SUFFIX_READREPORT       = 'r',  /*!< read report */ 
    MMS_SUFFIX_DELIVERYREPORT   = 'd',  /*!< delivery report */
    MMS_SUFFIX_ERROR            = 'x'   /*!< error, not a suffix! */
} MmsFileType;

#ifdef MMS_UBS_IN_USE

typedef struct 
{
    const char               *strFolderId;
    int                       folderType;
} MmsStrFolderId;


#endif /* MMS_UBS_IN_USE */

/*! \enum MmsFromType The "From Address" can either be supplied by the 
 *  MMS Application or the MMS Proxy-Relay can be asked to insert it. 
 *  MmsFromType lists the different values that can be used.
 */
typedef enum
{
    MMS_FROM_ADDRESS_PRESENT = 128, /*!< The address is added by the client */
    MMS_FROM_INSERT_ADDRESS  = 129  /*!< The address is added by the server */
} MmsFromType;

/*! \struct MmsAddress 
 *  \brief An MMS address corresponds to the fields in this struct. 
 */
typedef struct
{
    MmsEncodedText name;        /*!< Display name (e.g. Bill Löfgren, AU-System) */
    char *address;              /*!< Address (e.g. bill.lofgren@ausystem.se) */
    MmsAddressType addrType;    /*!< Address Type (PLMN or EMAIL) */
} MmsAddress;

/*! \struct MmsAddressList
 *  \brief When more than one address is supplied for a field, e.g. several
 *         recipients in "To" field, the MmsAddressList contains all addresses. 
 */
typedef struct MmsAddressListStruct 
{
    /*! Address for current element */
    MmsAddress current;                 
    
    /*! Next address list element or NULL if last element */
    struct MmsAddressListStruct *next;  
} MmsAddressList;

/*! \enum MmsClassIdentifier An MMS message can have different message classes.
 *  Auto indicates a message that is automatically generated by the client. 
 *  When Message Class is AUTO, no Delivery-Report or Read-Reply should be 
 *  sent. When creating an MMS, the Message Class AUTO is not possible to set 
 *  from the MMS Application since this Message Class is reserved for 
 *  automatically generated messages in the Proxy-Relay or the MMS Client 
 *  (below the MMS Application level). The message class IS_TEXT is used when 
 *  message class is set as a text string value, for more information see: 
 *  #MmsMessageClass. 
 */
typedef enum
{
    MMS_MESSAGE_CLASS_NOT_SET       =   0,
    MMS_MESSAGE_CLASS_PERSONAL      = 128,
    MMS_MESSAGE_CLASS_ADVERTISEMENT = 129,
    MMS_MESSAGE_CLASS_INFORMATIONAL = 130,
    MMS_MESSAGE_CLASS_AUTO          = 131,
    MMS_MESSAGE_CLASS_IS_TEXT       = 254
} MmsClassIdentifier;

/*! \struct MmsMessageClass 
 *  \brief Contains the Class of the message. 
 *  Class can be an MmsClassIdentifier or a string. If string is used the 
 *  MmsClassIdentifier must be set to MMS_MESSAGE_CLASS_IS_TEXT. 
 *  If no Class is set the message is treated as Personal.
 */
typedef struct
{
    MmsClassIdentifier  classIdentifier;    /*!< Enum representation of a message class */
    char                *textString;        /*!< Textual representation of a message class */
} MmsMessageClass;

/*! \enum MmsPriority An MMS message can have different priorities. This enum 
 *  lists the possible values.
 */
typedef enum
{
    MMS_PRIORITY_NOT_SET    =   0,
    MMS_PRIORITY_LOW        = 128,
    MMS_PRIORITY_NORMAL     = 129,
    MMS_PRIORITY_HIGH       = 130
} MmsPriority;

/*! \enum MmsSenderVisibility The sender address in an MMS message can be shown
 *  or not for the recipient. Possible values are listen in this enum. 
 */
typedef enum
{
    MMS_SENDER_VISIBILITY_NOT_SET   =   0,  /*!< Show unless sender has secret address*/
    MMS_SENDER_HIDE                 = 128,  /*!< Don't show the address. */
    MMS_SENDER_SHOW                 = 129   /*!< Show even secret address. */
} MmsSenderVisibility;

/*! \enum MmsDeliveryReport Sending of a Delivery Report to the original 
 *  message sender can either be requested or not. This field is called 
 *  "Report-Allowed" in the MMS standard.
 */
typedef enum /* X-Mms-Delivery-Report field values */
{
    MMS_DELIVERY_REPORT_NOT_SET =   0,  /*!< Field not used (no part of standard values */
    MMS_DELIVERY_REPORT_YES     = 128,  /*!< Delivery report is requested. */
    MMS_DELIVERY_REPORT_NO      = 129   /*!< Delivery report is not requested. */
} MmsDeliveryReport;


/*! \enum MmsDeliveryReportAllowed
 *  When sending acknownledge back to the MMSC, this enum is used to define
 *  if the MMSC is allowed to send a delivery-report to the originator.
 *  This field is called "Report-Allowed" in the MMS standard.
 * X-Mms-Response-Allowed feild values
 */
typedef enum /* X-Mms-Response-Allowed field values */
{
    MMS_DELIVERY_REPORT_ALLOWED_NOT_SET =   0,  /*!< Field not used (no part of standard values */
    MMS_DELIVERY_REPORT_ALLOWED_YES     = 128,  /*!< Delivery report is allowed. */
    MMS_DELIVERY_REPORT_ALLOWED_NO      = 129   /*!< Delivery report is not allowed. */
} MmsDeliveryReportAllowed;


/*! \enum MmsReadReply This enum contains values possible for to set for the
 *  Read Reply request.
 */
typedef enum
{
    MMS_READ_REPLY_NOT_SET  =   0,
    MMS_READ_REPLY_YES      = 128,      /*!< Read reply is requested. */
    MMS_READ_REPLY_NO       = 129       /*!< Read reply is not requested. */
} MmsReadReply;

/*! \enum MmsStatus The status of an MMS message can be any of the following 
 *  values. 
 */
typedef enum
{
    MMS_STATUS_EXPIRED          = 128,
    MMS_STATUS_RETRIEVED        = 129,
    MMS_STATUS_REJECTED         = 130,
    MMS_STATUS_DEFERRED         = 131,
    MMS_STATUS_UNRECOGNIZED     = 132,
    MMS_STATUS_INDETERMINATE    = 133,
    MMS_STATUS_FORWARDED        = 134
} MmsStatus;

/*! \enum MmsTimeType The type of time used in fields like expiry time and
 *  delivery time.
 */
typedef enum
{
    MMS_TIME_ABSOLUTE           = 128,
    MMS_TIME_RELATIVE           = 129
} MmsTimeType;

typedef WE_UINT32 MmsTimeSec;      /*!< Time in sec from 1970-01-01 */ 

typedef struct  
{    
    MmsTimeType type;    
    MmsTimeSec  value;    
} MmsTime;


typedef WE_UINT32 MmsReplyChargingSize; /*!< Size allowed for a reply charging*/

/*! \enum MmsReplyCharging Indicates that the originator is willing to pay for 
 *  the Reply-MM (requested) or that a reply to this MM is free of charge for 
 *  the recepient (accepted).
 */
typedef enum
{
    MMS_REPLY_CHARGING_NOT_SET              =   0,  /*!< Reply Charging is not used */

    /* Used only in M_Send.req */
    MMS_REPLY_CHARGING_REQUESTED            = 128,  /*!< Used when sending a message */
    MMS_REPLY_CHARGING_REQUESTED_TEXT_ONLY  = 129,  /*!< Used when sending a message */

    /* Used only in M_Retrieve.conf and M_Notification.ind */
    MMS_REPLY_CHARGING_ACCEPTED             = 130,  /*!< Used when retrieving a message or notification */
    MMS_REPLY_CHARGING_ACCEPTED_TEXT_ONLY   = 131   /*!< Used when retrieving a message or notification */
} MmsReplyCharging;

/*! \struct MmsPrevSentBy Addresses of clients that previously has sent a  message*/ 
typedef struct MmsPrevSentByStruct
{
    WE_UINT32  forwardedCountValue;
    MmsAddress  sentBy;
    struct MmsPrevSentByStruct *next;
} MmsPrevSentBy;

/*! \struct MmsPrevSentDate date when a clients previously has sent a  message*/ 
typedef struct MmsPrevSentDateStruct
{
    WE_UINT32  forwardedCountValue;
    MmsTimeSec  date;
    struct MmsPrevSentDateStruct *next;
} MmsPrevSentDate;

/*! \enum MmsEntryHeaderValueType All valid entry header types. 
 *  This is a part of the message bodys different entries. */
typedef enum
{
    MMS_SHORT_CUT_SHIFT_DELIMITER,          /*!< Value is in shortCutShiftDelimiter */
    MMS_APPLICATION_HEADER,                 /*!< Type and value is in applicationHeader */
    MMS_WELL_KNOWN_CONTENT_BASE     = 0x0A, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_ENCODING = 0x0B, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_LANGUAGE = 0x0C, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_LENGTH   = 0x0D, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_LOCATION = 0x0E, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_MD5      = 0x0F, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_RANGE    = 0x10, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_TYPE     = 0x11, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_DISPOSITION = 0x2E, /*!< Value is in wellKnownFieldName */
    MMS_WELL_KNOWN_CONTENT_ID       = 0x40, /*!< Value is in wellKnownFieldName */ 
    MMS_SHIFT_DELIMITER             = 0x7f  /*!< Value is in shiftDelimiter */
} MmsEntryHeaderValueType;

/*! \struct MmsEntryHeaderData 
 *  \brief  Contains one entry header type and its value.
 *          This is a part of the message bodys different entries. 
 */
typedef struct
{
    unsigned char *name;     /*!< Well-known-field-name or a char.*/
    unsigned char *value;   /*!< Null terminated text string.*/
} MmsEntryHeaderData;

/*! \struct MmsEntryHeader 
 *  \brief The types and values of the MMS message bodys  entry header. 
 */
typedef struct MmsAllEntryHeaders
{
    /*! Info about which union type to use.*/
    MmsEntryHeaderValueType headerType; 

    union
    {
        /*! Value of a shift delimiter.*/
        unsigned char shiftDelimiter;   
        
        /*! Type of short cut shift delimiter.*/
        unsigned char shortCutShiftDelimiter; 
        
        /*! Value of a well known field name.*/
        unsigned char *wellKnownFieldName; 
        
        /*! Type and value of an application header.*/
        MmsEntryHeaderData applicationHeader;
    } value;

    /*! Next entry header for a body part, NULL if there is no more headers. */
    struct MmsAllEntryHeaders *next;
} MmsEntryHeader;

/*! \enum MmsKnownMediaType Contains the media-type values that the MMS Client supports.
 */
typedef enum
{
    /* Unknown */
    MMS_TYPE_ANY_ANY                            = 0x00,     

    /* Text */
    MMS_TYPE_TEXT_ANY                           = 0x01, 
    MMS_TYPE_TEXT_HTML                          = 0x02, 
    MMS_TYPE_TEXT_PLAIN                         = 0x03,
    MMS_TYPE_TEXT_X_HDML                        = 0x04,
    MMS_TYPE_TEXT_X_TTML                        = 0x05,
    MMS_TYPE_TEXT_X_VCALENDAR                   = 0x06,
    MMS_TYPE_TEXT_X_VCARD                       = 0x07,
    MMS_TYPE_TEXT_VND_WAP_WML                   = 0x08,
    MMS_TYPE_TEXT_VND_WAP_WMLSCRIPT             = 0x09,
    MMS_TYPE_TEXT_VND_WAP_CHANNEL               = 0x0A,

    /* Multipart */
    MMS_TYPE_MULTIPART_ANY                      = 0x0B,
    MMS_TYPE_MULTIPART_MIXED                    = 0x0C,
    MMS_TYPE_MULTIPART_FORM_DATA                = 0x0D,
    MMS_TYPE_MULTIPART_BYTERANGES               = 0x0E,
    MMS_TYPE_MULTIPART_ALTERNATIVE              = 0x0F,

    /* Application */
    MMS_TYPE_APP_ANY                            = 0x10,
    MMS_TYPE_APP_JAVA_VM                        = 0x11,
    MMS_TYPE_APP_X_WWW_FORM_URLENCODED          = 0x12,
    MMS_TYPE_APP_X_HDMLC                        = 0x13,
    MMS_TYPE_APP_VND_WAP_WMLC                   = 0x14,
    MMS_TYPE_APP_VND_WAP_WMLSCRIPTC             = 0x15,
    MMS_TYPE_APP_VND_WAP_CHANNELC               = 0x16,
    MMS_TYPE_APP_VND_WAP_UAPROF                 = 0x17,
    MMS_TYPE_APP_VND_WAP_WTLS_CA_CERTIFICATE    = 0x18,
    MMS_TYPE_APP_VND_WAP_WTLS_USER_CERTIFICATE  = 0x19,
    MMS_TYPE_APP_X_X509_CA_CERT                 = 0x1A,
    MMS_TYPE_APP_X_X509_USER_CERT               = 0x1B,

    /* Image */
    MMS_TYPE_IMAGE_ANY                          = 0x1C, 
    MMS_TYPE_IMAGE_GIF                          = 0x1D, 
    MMS_TYPE_IMAGE_JPEG                         = 0x1E,
    MMS_TYPE_IMAGE_TIFF                         = 0x1F, 
    MMS_TYPE_IMAGE_PNG                          = 0x20, 
    MMS_TYPE_IMAGE_VND_WAP_WBMP                 = 0x21,

    /* Application */
    MMS_VND_WAP_MULTIPART_ANY                   = 0x22, 
    MMS_VND_WAP_MULTIPART_MIXED                 = 0x23,
    MMS_TYPE_APP_VND_WAP_MPART_FORM_DATA        = 0x24,
    MMS_TYPE_APP_VND_WAP_MPART_BYTERANGES       = 0x25, 
    MMS_VND_WAP_MULTIPART_ALTERNATIVE           = 0x26, 
    MMS_TYPE_APP_XML                            = 0x27,

    /* Text */
    MMS_TYPE_TEXT_XML                           = 0x28,

    /* Application */
    MMS_TYPE_APP_VND_WAP_WBXML                  = 0x29,
    MMS_TYPE_APP_X_X968_CROSS_CERT              = 0x2A,
    MMS_TYPE_APP_X_X968_CA_CERT                 = 0x2B,
    MMS_TYPE_APP_X_X968_USER_CERT               = 0x2C,

    /* Text */
    MMS_TYPE_TEXT_VND_WAP_SI                    = 0x2D,

    /* Application */
    MMS_TYPE_APP_VND_WAP_SIC                    = 0x2E,

    /* Text */
    MMS_TYPE_TEXT_VND_WAP_SL                    = 0x2F,

    /* Application */
    MMS_TYPE_APP_VND_WAP_SLC                    = 0x30,

    /* Text */
    MMS_TYPE_TEXT_VND_WAP_CO                    = 0x31,

    /* Application */
    MMS_TYPE_APP_VND_WAP_COC                    = 0x32,
    MMS_VND_WAP_MULTIPART_RELATED               = 0x33, 
    MMS_TYPE_APP_VND_WAP_SIA                    = 0x34,

    /* Text */
    MMS_TYPE_TEXT_VND_WAP_CONNECTIVITY_XML      = 0x35,

    /* Application */
    MMS_TYPE_APP_VND_WAP_CONNECTIVITY_WBXML     = 0x36,
        
    /* DRM related media types */
    MMS_TYPE_APP_VND_OMA_DRM_MESSAGE            = 0x48,
    MMS_TYPE_APP_VND_OMA_DRM_CONTENT            = 0x49,
    MMS_TYPE_APP_VND_OMA_DRM_R_XML              = 0x4A,
    MMS_TYPE_APP_VND_OMA_DRM_R_WBXML            = 0x4B,

    /* Not a well known value */
    MMS_VALUE_AS_STRING                         = 0xFF 
    
} MmsKnownMediaType;

/*! \enum MmsParam Contains possible parameter values for the content-type 
 *  header field. [WAP-203, Table 38]
 */
typedef enum
{
    MMS_CHARSET         = 0x01, /*!< Well-known-charset */
    MMS_LEVEL           = 0x02, /*!< Version-value */
    MMS_TYPE            = 0x03, /*!< Integer-value */
    MMS_TYPE_REL        = 0x09, /*!< Text-Value, used with multipart related*/
    MMS_NAME            = 0x05, /*!< Text-Value deprecated from version 14*/
    MMS_FILENAME        = 0x06, /*!< Text-Value deprecated from version 14*/
    MMS_START_REL       = 0x0A, /*!< Text-Value, used with multipart related depricated from version 14*/
    MMS_START_INFO_REL  = 0x0B, /*!< Text-Value, used with multipart related depricated from version 14*/
    MMS_FILENAME_14     = 0x18, /*!< Text-Value new from version 14*/
    MMS_NAME_14         = 0x17,  /*!< Text-Value new from version 14*/
         MMS_START_REL_14       = 0x19, /*!< Text-Value, used with multipart related new from version 14*/
        MMS_START_INFO_REL_14  = 0x1A,  /*!< Text-Value, used with multipart related new from version 14*/
    MMS_TEXT_KEY        = 0xFE, /*!< Text-Value, untyped text key-value */
    MMS_UNHANDLED       = 0xFF  /*!< All parameters not handled by the MMS Client */
} MmsParam;

typedef struct
{
    unsigned char *strKey; /* Key value definition */
    MmsParam       assignedValue;
} MmsParamTextKey;

/*! \enum MmsParamType Describes how the content type parameter is 
 *  represented. e.g. as a string or as an integer value.
 */
typedef enum
{
    MMS_PARAM_STRING,
    MMS_PARAM_INTEGER 
} MmsParamType;

/*! \struct MmsAllParams 
 *  \brief Contains a list of all content-type parameters of a message.
 */
typedef struct MmsAllParamsStruct
{
    MmsParam        param;  /*!< MmsParam enum */
    
    /*! Param value type. MMS_PARAM_STRING = String, MMS_PARAM_INTEGER = int */
    MmsParamType    type;  
    
    unsigned char   *key;   /*!< Parameter text key, Token-text See WAP-203-WSP 8.4.2.1 */
    
    union
    {
        unsigned char   *string;    /*! The value is a RFC2616 encoded string or NULL */
        WE_UINT32      integer;    /*! The value is a number */
    } value;
    /*! Pointer to next param. NULL if no more params  */
    struct MmsAllParamsStruct *next;    
} MmsAllParams;

struct MmsDrmInfoStruct;


/*! \struct MmsContentType 
 *  \brief Contains the content-type value and belonging parameters.
 *         If there are no parameters "params" must be set to NULL!
 */
typedef struct
{
    /*! The media type is a known value */
    MmsKnownMediaType   knownValue; 
    /*! The media type is a us-ascii string */
    unsigned char       *strValue;      
    /*! Pointer to the content type params, NULL if there is no parameters */
    MmsAllParams        *params;    
    struct MmsDrmInfoStruct *drmInfo;       /*!< If NULL, the body part has no DRM protection */
} MmsContentType;

typedef enum
{
        MmsNoProtection = 0,        /* no DRM protection */
        MmsForwardLock = 1,         /* Forward locked */
        MmsSeparateDelivery = 2,    /* Separate delivery DRM (encrypted DCF) */
        MmsCombinedDelivery = 3     /* Combined delivery DRM, contains both rights and DCF object */
} MmsDrmType;

typedef struct MmsDrmInfoStruct
{
    MmsDrmType          drmType;          /* Type of DRM protection */
    MmsContentType      drmContainerType; /* Content-type of the container DRM */
    char                *dcfFile;         /* DCF file location */ 
} MmsDrmInfo;

/*! \struct MmsBodyInfoList 
 *  \brief A List with information about the parts of a body.
 */
typedef struct MmsBodyInfoStruct
{
    MmsContentType  *contentType;   /*!< The content type information. */
    MmsEntryHeader  *entryHeader;   /*!< The entry header information. */
    WE_UINT32      number;         /*!< Which part in the body. */
    WE_UINT32      startPos;       /*!< Start position for the body part (data). */
    WE_UINT32      size;           /*!< Total size of the body part (data). */
    
    /*! Number of sub parts for the current multipart. 0 if not a multipart */
    WE_UINT32      numOfEntries;   
     
    /*! Pointer to the next body info element in the list, NULL if last */
    struct MmsBodyInfoStruct *next;    
} MmsBodyInfoList;

/*! \struct MmsSetHeader 
 *  \brief A struct containing possible header fields. 
 *  All optional field must be set to 0 or NULL when not used.
 *
 *  Creation of MMS Message :
 *          Optional fields are marked with [O]
 *          and mandatory fields marked with [M] (i.e. must be set)
 */
typedef struct
{                            
    /*! [M] The address of the message sender. */ 
    MmsAddress from;                    

    /*! [O] The addressee of the message. The address field can contain one or
     *      several addresses. The parameter is a reference to a list 
     *      containing the recipient addresses. One element in the list carries 
     *      the address (PLMN or e-mail) and the text string representing the 
     *      recipient real name (e.g. Joe User) to be used in multimedia 
     *      messages. Please note that PLMN addresses cannot make use of real 
     *      names. A message needs always at least one specified address in one
     *      of the to, cc or bcc field.    
     */ 
    
    /*! [M] The insert method of the from address. */ 
    MmsFromType fromType;   

    MmsAddressList *to;                 

    /*! [O] See To field. */    
    MmsAddressList *cc;                 

    /*! [O] See To field. */ 
    MmsAddressList *bcc;                

    /*! [O] The text string containing the subject of the message. See also 
     *      configuration setting for the maximum length of this field in 
     *      Mms_Cfg.h.
     */
    MmsEncodedText subject;             

    /*! [O] Arrival time (in seconds since 1970-01-01) of the message at the 
     *      MMS Proxy-Relay. The MMS Proxy-Relay will generate this field if 
     *      not supplied.
     */
    MmsTimeSec date;                    

    /*! [M] The Content-Type contains a media type and a number of parameters. 
     *      Examples of media-types are: text/plain, image/jpg, 
     *      multipart/related etc. Examples of parameters are: start, type, 
     *      filename. The MMS Application must always set this field.
     */ 
    MmsContentType *contentType;        

    /*! [O] The message class field labels the message type as Auto, 
     *      Informational or Advertisement. When the message class is not set 
     *      the class will be interpreted as Personal at the recipient. It is 
     *      recommended not to set this field in normal MMS Applications. 
     */ 
    MmsMessageClass msgClass;           
    
    /*! [O] Defines the time when the message will be removed from the 
     *      recipient's MMS Proxy-Relay. If not set the maximum time will be 
     *      used. The maximum value is dependent on the MMS Proxy-Relay. 
     */
    MmsTime expiryTime;              

    /*! [O] This time value indicates for the MMS Proxy-Relay when the message 
     *      should be delivered to the recipient(s) at a later time. When this 
     *      field is not set the message is delivered immediately.
     */
    MmsTime deliveryTime;            

    /*! [O] This value defines the message priority. */ 
    MmsPriority priority;               

    /*! [O] This value is used to control if the "From" address is displayed 
     *      in the message at the recipient's MMS Application.
     */ 
    MmsSenderVisibility visibility;     

    /*! [O] This field is requesting the retrieving MMS Applications to send a 
     *      read reply message when the message was read. Note; a retrieving 
     *      MMS Application may cancel the request. A read reply message sent
     *      is seen as an ordinary message and the MMS Application controls 
     *      the contents.
     */ 
    MmsReadReply readReply;             

    /*! [O] The setting of this flag will cause the MMS Proxy-Relay to send 
     *      back a notification message when the recipient retrieves the 
     *      message. A retrieving MMS Application may prevent a Delivery 
     *      Report from being sent.
     */ 
    MmsDeliveryReport deliveryReport; 

    /*! [O] Allow reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */ 
    MmsReplyCharging replyCharging;

    /*! [O] Deadline for reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */
    MmsTime replyChargingDeadline;

    /*! [O] Deadline for reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */
    char *replyChargingId;

    /*! [O] Allowed size for a reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */
    MmsReplyChargingSize replyChargingSize;
} MmsSetHeader;

/*! \struct MmsGetHeader 
 *  \brief A struct containing possible header fields. 
 *  All fields are set to 0 or NULL when not used.
 *
 */
typedef struct
{                            
    /*! [M] The address of the message sender. */ 
    MmsAddress from;                    

    /*! [O] The addressee of the message. The address field can contain one or
     *      several addresses. The parameter is a reference to a list 
     *      containing the recipient addresses. One element in the list carries 
     *      the address (PLMN or e-mail) and the text string representing the 
     *      recipient real name (e.g. Joe User) to be used in multimedia 
     *      messages. Please note that PLMN addresses cannot make use of real 
     *      names. A message needs always at least one specified address in one
     *      of the to, cc or bcc field.    
     */ 
    MmsAddressList *to;                 

    /*! [O] See To field. */    
    MmsAddressList *cc;                 

    /*! [O] See To field. */ 
    MmsAddressList *bcc;                

    /*! [O] The text string containing the subject of the message. See also 
     *      configuration setting for the maximum length of this field in 
     *      Mms_Cfg.h.
     */
    MmsEncodedText subject;             

    /*! [O] Arrival time (in seconds since 1970-01-01) of the message at the 
     *      MMS Proxy-Relay. The MMS Proxy-Relay will generate this field if 
     *      not supplied.
     */
    MmsTimeSec date;                    

    /*! [O] The message class field labels the message type as Auto, 
     *      Informational or Advertisement. When the message class is not set 
     *      the class will be interpreted as Personal at the recipient. It is 
     *      recommended not to set this field in normal MMS Applications. 
     */ 
    MmsMessageClass msgClass;           

    /*! [O] Defines the time when the message will be removed from the 
     *      recipient's MMS Proxy-Relay. If not set the maximum time will be 
     *      used. The maximum value is dependent on the MMS Proxy-Relay. 
     */
    MmsTime expiryTime;              

    /*! [O] This time value indicates for the MMS Proxy-Relay when the message 
     *      should be delivered to the recipient(s) at a later time. When this 
     *      field is not set the message is delivered immediately.
     */
    MmsTime deliveryTime;            

    /*! [O] This value defines the message priority. */ 
    MmsPriority priority;               

    /*! [O] This value is used to control if the "From" address is displayed 
     *      in the message at the recipient's MMS Application.
     */ 
    MmsSenderVisibility visibility;     

    /*! [O] This field is requesting the retrieving MMS Applications to send a 
     *      read reply message when the message was read. Note; a retrieving 
     *      MMS Application may cancel the request. A read reply message sent
     *      is seen as an ordinary message and the MMS Application controls 
     *      the contents.
     */ 
    MmsReadReply readReply;             

    /*! [O] The setting of this flag will cause the MMS Proxy-Relay to send 
     *      back a notification message when the recipient retrieves the 
     *      message. A retrieving MMS Application may prevent a Delivery 
     *      Report from being sent.
     */ 
    MmsDeliveryReport deliveryReport; 
    
    /*! [M] Id of the message.
     *      This field is only used when retrieving a message, NOT used when 
     *      sending.
     */
    char *serverMessageId;

    /*! [O] Allow reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */ 
    MmsReplyCharging replyCharging;

    /*! [O] Deadline for reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */
    MmsTime replyChargingDeadline;

    /*! [O] Deadline for reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */
    char *replyChargingId;

    /*! [O] Allowed size for a reply charging. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater.
     */
    MmsReplyChargingSize replyChargingSize;
    
    /*! [O] Address information about which clients that previously has 
     *      sent a message. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater and it is only used when retrieving a message, NOT 
     *      used when sending.
     */
    MmsPrevSentBy *previouslySentBy;

    /*! [O] Date information about when clients has previously sent a message. 
     *      This field can only be used with MMS standard version 1.1 and 
     *      greater and it is only used when retrieving a message, NOT used 
     *      when sending.
     */
    MmsPrevSentDate *previouslySentDate;
      
    /*! [M] Defines the type of the message, Send Request or Retrieve Confrim */ 
    MmsMsgType messageType; 

    /*! [M] Is message forward locked (DRM)
     */
    WE_BOOL forwardLock;

    MmsContentType  *contentType;   /*!< The content type information. */

    MmsVersion version;             /*! [M] Endoding version of the PDU */
} MmsGetHeader;

/*! \struct MmsNotification  
 *  \brief This structure represents the header values received in an 
 *         M-Notification.ind message. Further, this struct is 
 *         used for reading a notification message.
 */
typedef struct 
{
    /*! The sender address. */
    MmsAddress from;         

    /*! The subject of the MMS message */
    MmsEncodedText subject;  

    /*! Class of the message. */
    MmsMessageClass msgClass;

    /*! The length of the message to fetch. */
    WE_UINT32 length;           

    /*! Time when the message is removed from the MMS Proxy-Relay. 
     * No expiry time type is needed since the value is always relative 
     */
    MmsTime expiryTime;   

    /*! [O] Allow reply charging. 
     * This value can only be used with MMS standard version 1.1 and greater
     */ 
    MmsReplyCharging replyCharging;

    /*! [O] Deadline for reply charging. 
     * This value can only be used with MMS standard version 1.1 and greater
     */
    MmsTime replyChargingDeadline;

    /*! [O] Deadline for reply charging. 
     * This value can only be used with MMS standard version 1.1 and greater
     */
    char *replyChargingId;

    /*! [O] Allowed size for a reply charging. 
     * This value can only be used with MMS standard version 1.1 and greater
     */
    MmsReplyChargingSize replyChargingSize;
     
    /*! Defines the location of the message */
    char *contentLocation;   

    /*! Notification transaction-WID */
    char *transactionId; 
    
    /*! Delivery report request */
    MmsDeliveryReport deliveryReport;
    
} MmsNotification;

/*! \struct MmsForward 
 *  \brief A struct containing possible header fields. 
 *  All optional field must be set to 0 or NULL when not used.
 *
 *  Creation of MMS Forward :
 *          Optional fields are marked with [O]
 *          and mandatory fields marked with [M] (i.e. must be set)
 */
typedef struct
{                            
    /*! [O] Arrival time of the message at the MMSProxy-Relay. 
     *  MMSProxy-Relay will generate this field if not supplied. */
    MmsTimeSec date;                    

    /*! [M] Address of the message sender. */ 
    MmsAddress from;                    

    /*! [M] The insert method of the from address. */ 
    MmsFromType fromType;   
    
    /*! [O] Address of the recipient. At least one of the addresses
     *  (To, cc, bcc) MUST be present. If not used it must be set to NULL. */ 
    MmsAddressList *to;                 

    /*! [O] See To field. */    
    MmsAddressList *cc;                 

    /*! [O] See To field. */ 
    MmsAddressList *bcc;                

    /*! [O] The type of expiry time, absolute or relative. Absolute msans that
     *      an absolute time (in seconds since 1970-01-01) is given. Relative
     *      msans that the server will add the given number of seconds to
     *      current time.
     */
    MmsTimeType expiryTimeType;

    /*! [O] Time until the message is removed from the MMS Proxy-Relay,
     *  not set equals max time.
     */
    MmsTime expiryTime;              

    /*! [O] Time the message should be delivered, not set equals immediate. */
    MmsTime deliveryTime;            

    /* [O] Sending of delivery report allowed to the previous sender of the 
     * message to be forwarded */
    MmsDeliveryReportAllowed reportAllowed;

    /*! [O] Request for a Delivery Report. */ 
    MmsDeliveryReport deliveryReport;   

    /*! [O] Request for a Read Reply. */ 
    MmsReadReply readReply;             
} MmsForward;

/*! \struct MmsReadOrigInd
 *  \brief A struct containing possible header fields for a Read Orig Ind PDU. 
 */
typedef struct
{
    /*! [M] Address of the message sender. */ 
    MmsAddress from;

    /*! [M] Arrival time of the message at the MMSProxy-Relay. 
     *  MMSProxy-Relay will generate this field if not supplied. */
    MmsTimeSec date;           

    /*! [M] Read-Status of the message. */
    MmsReadStatus readStatus;

    /* [M] Message-Id of the message. */
    char *serverMessageId;
} MmsReadOrigInd;

/*!
 * \enum MmsBodyPartTarget Information about where the bodypart will be added.
 */
typedef enum
{
    /*! The bodypart is added to a multipart message. */
    MMS_MULTIPART_MESSAGE,
    /*! The bodypart is added to a multipart entry file */
    MMS_MULTIPART_ENTRY,
    /*! The bodypart is added to a plain message (NOT a multipart).
     */
    MMS_PLAIN_MESSAGE
} MmsBodyPartTarget;



/*!
 * \struct MmsMessageInfo 
 * \brief Contains information about a message and body.
 */
typedef struct
{
    /*! Total number of message parts in a message body. */
    WE_UINT32          numOfMsgInfo;   
    /*! Size of the header. */
    WE_UINT32              headerSize; 
    /*! Number of sub parts for entire multipart message. 0 if not a multipart */
    WE_UINT32          numOfEntries;   
    /*! List with all content types in a message body  */
    MmsBodyInfoList     *list;  
} MmsMessageInfo;


/*! 
 * \struct MmsMessageInfoHandle
 * \breif Holds information about the message
 */
typedef struct
{
    unsigned char *buffer;
    WE_UINT32  bufferSize;
} MmsMessageInfoHandle;


/*! \struct MmsBodyParams 
 *  \brief A struct containing information about the body
 *         part. All parameters are used when adding a body part, hence
 *         none should be left unset.
 */
typedef struct
{
    MmsEntryHeader *entryHeader;        /*!< Entry headers. NULL if none. */ 
    MmsContentType contentType;         /*!< The content type information */
    MmsBodyPartTarget targetType;       /*!< The type of message the part is added to */
} MmsBodyParams; 

/*! MmsRequestId A unique WID for all active GET/POST requests towards STK */
typedef WE_INT16 MmsRequestId;

/*! \enum MmsNotificationType Possible notification types */
typedef enum
{
    /*! A normal notification used with immediate retrieval.
     */
    MMS_NOTIFICATION_IMMEDIATE,

    /*! A normal notification used with delayed retrieval.
     */
    MMS_NOTIFICATION_NORMAL,

    /*! A notification indicating that a retrieved 
     *  Multimedia Message is stored in a folder. 
     */
    MMS_NOTIFICATION_MESSAGE
} MmsNotificationType;

/*! \enum MmsRoamingStatus network roaming state.
 *
 * This enum is used in the registy to decide if the cllular is in roaming
 *
 */
typedef enum
{
    /*! Cellular is considered not to roaming.
     */
    MMS_NOT_ROAMING,

    /*! Cellular is considered to be roaming.
     */
    MMS_ROAMING
} MmsRoamingStatus;

/*! \enum MmsBearerStatus network bearer state.
 *
 * This enum is used in the registy to decide if the network
 * bearer is available for connection or not.
 *
 */
typedef enum
{
    /*! Network bearer is not available, attempts are futile.
     */
    MMS_BEARER_NOT_AVAILABLE,
    /*! Network beaerer is available for connection attempt.
     */
    MMS_BEARER_AVAILABLE
} MmsBearerStatus;


/*! \enum MmsRoamingMode How message notifications are handle while in roaming.
 *
 * This enum is used in the registy to decide how message notifications are
 * handled while the cellular is in roaming mode.
 *
 * Registry key MMS_REG_KEY_NETWORK_STATUS_ROAMING is used to decide roaming state.
 */
typedef enum
{
    /*! All message notifications are handled as in non-roaming.
     * The setting for delayed of immediate retrival is used.
     */
    MMS_ROAM_DEFAULT,

    /*! No notifications are immediately retrived..
     * But when roaming is ended, all notifications suitable for
     * immediate retrival are retrived automatically.
     * 
     */
    MMS_ROAM_RESTRICTED,

    /*! All notifications are deffered (delayed retrieval).
     */
    MMS_ROAM_DELAYED,

    /*! All message notifications are rejected in roaming.
     */
    MMS_ROAM_REJECT
} MmsRoamingMode;

/*! \enum MmsNotificationReason Reason why a notification is received */
typedef enum
{
    /*! The notification is received the normal way.
    */
    MMS_NOTIFICATION_REASON_NORMAL,
                
    /*! The notification is received because the immediate retrieval conditions 
    *  where not fulfilled (requires #MMS_CFG_IMMEDIATE_RETRIEVAL to be set 
    *  to TRUE).
    */
    MMS_NOTIFICATION_REASON_CONDITION_NOT_FULFILLED,
    
    /*! The notification is received because the immediate retrieval operation 
    *  failed (requires #MMS_CFG_IMMEDIATE_RETRIEVAL to be set to TRUE).
    */
    MMS_NOTIFICATION_REASON_RETIEVAL_FAILED,
    
    /*! The notification is received because the message size exceeded the 
    *  remaining persistent storage (requires #MMS_CFG_IMMEDIATE_RETRIEVAL 
    *  to be set to TRUE).
    */
    MMS_NOTIFICATION_REASON_FULL_PERSISTENT_STORAGE,
    
    /*! The notification is received because the message size exceeded the 
    *  maximum allowed download size (requires #MMS_CFG_IMMEDIATE_RETRIEVAL 
    *  to be set to TRUE).
    */
    MMS_NOTIFICATION_REASON_MAX_RETRIEVAL_SIZE_EXCEEDED
} MmsNotificationReason;

/*! \enum MmsResult Result values sent from the MMS Client. */
typedef enum
{
    /* The result codes in this enum shall be held in alphabetic order,
     * except OK that shall be the first and hence has a value equal to 0.
     */

   /*! The operation is successful and finished. */
    MMS_RESULT_OK,
    
    /*! The supplied buffer is full */
    MMS_RESULT_BUFFER_FULL,

   /*! Too many consective calls to a connector function have been made without 
    *  waiting for the outcome of the previous calls.
    */
    MMS_RESULT_BUSY,

    /*! A requested body part was not found */
    MMS_RESULT_BODY_PART_NOT_FOUND,

    /*! An #MMSc_sendMessage or #MMSc_retrieveMessage operation is cancelled
     *  by an #MMSc_cancel. */
    MMS_RESULT_CANCELLED_BY_USER,

    /*! An send or retrieve operation is cancelled
     *  by an the system, propably because network bearer is unavailable.
     */
    MMS_RESULT_CANCELLED_BY_SYSTEM,

    /*! Communication error: The address could not be resolved. */
    MMS_RESULT_COMM_ADDRESS_UNRESOLVED,
    
    /*! Communication error: Bad gateway. */
    MMS_RESULT_COMM_BAD_GATEWAY,

    /*! Communication error: Entity too large. */
    MMS_RESULT_COMM_ENTITY_TOO_LARGE,

    /*! Communication error: Unknown communication error. */
    MMS_RESULT_COMM_FAILED,

    /*! Communication error: File or message not found. */
    MMS_RESULT_COMM_FILE_NOT_FOUND,

    /*! Communication error: Communication not allowed, service or method 
     *  denied. */
    MMS_RESULT_COMM_FORBIDDEN,

    /*! Communication error: PDU did not contain expected tag. */
    MMS_RESULT_COMM_HEADER_TAG, 

    /*! Communication error: An illegal or badly formatted PDU was received. */
    MMS_RESULT_COMM_ILLEGAL_PDU,

    /*! Communication error: The operation (e.g. Reply Charging) had 
     *  limitations that were violated in the request. */
    MMS_RESULT_COMM_LIMITATIONS_NOT_MET,

    /*! Communication error: WSP/HTTP proxy needed or WSP/HTTP proxy 
     *  authentication required. */
    MMS_RESULT_COMM_PROXY_ERROR,

    /*! Communication error: The MMS Proxy-Relay did not recognize one PDU
     *  that the MMS Service sent. This is probably due to a misconfiguration
     *  of the MMS Proxy-Relay version. */
    MMS_RESULT_COMM_MMSC_VERSION,

    /*! Communication error: Error in TLS/WTLS setup. */
    MMS_RESULT_COMM_SECURITY_ERROR,

    /*! Communication error: Unknown MMS Proxy-Relay error. */
    MMS_RESULT_COMM_SERVER_ERROR,

    /*! Communication error: Wrong communication status. */
    MMS_RESULT_COMM_STATUS,

    /*! Communication error: Operation timed out, e.g. no contact with the MMS 
     *  Proxy-Relay. */
    MMS_RESULT_COMM_TIMEOUT,

    /*! Communication error: Unexpected transaction WID. */
    MMS_RESULT_COMM_TRANSACTION_ID, 

    /*! Communication error: Unauthorized. */
    MMS_RESULT_COMM_UNAUTHORIZED,

    /*! Communication error: Service unavailable or not supported. */
    MMS_RESULT_COMM_UNAVAILABLE,

    /*! Communication error: Unexpected message received. */
    MMS_RESULT_COMM_UNEXPECTED_MESSAGE, 

    /*! Communication error: Unsupported media type or content. */
    MMS_RESULT_COMM_UNSUPPORTED_MEDIA_TYPE,

    /*! Communication error: Unsupported version. */
    MMS_RESULT_COMM_UNSUPPORTED_VERSION,

    /*! Communication error: URI is too large. */
    MMS_RESULT_COMM_URI_TOO_LARGE,

    /*! A message for a not supported MMS version is received */
    MMS_RESULT_COMM_WRONG_MMS_VERSION,

    /*! Configuration is not correct. */
    MMS_RESULT_CONFIG_ERROR,

    /*! The current file operation cannot be performed at the moment. */
    MMS_RESULT_DELAYED,

    /*! An unknown error has occurred. No further information can be given. */
    MMS_RESULT_ERROR, 

    /*! The end-of-file has been reached for a file. This does not necessarily 
     *  indicate an error. */
    MMS_RESULT_EOF, 

    /*! Error when creating forward header. */
    MMS_RESULT_ERROR_CREATING_FORWARD,

    /*! Error when creating a Read Report. */
    MMS_RESULT_ERROR_CREATING_READ_REPORT,

    /*! Error when creating send header. */
    MMS_RESULT_ERROR_CREATING_SEND_HEADER,
    
    /*! Error creating backup file. */
    MMS_RESULT_FILE_BACKUP_ERROR,

    /*! A file is not found.*/
    MMS_RESULT_FILE_NOT_FOUND,
        
    /*! Error reading file. */
    MMS_RESULT_FILE_READ_ERROR,

    /*! Error when trying write to file */
    MMS_RESULT_FILE_WRITE_ERROR,

    /*! The folder is not found. */
    MMS_RESULT_FOLDER_NOT_FOUND, 

    /*! Index file is corrupt or does not exist. */
    MMS_RESULT_INDEX_FILE_ERROR,
    
    /*! Information file is corrupt or does not exist. */
    MMS_RESULT_INFO_FILE_ERROR,

    /*! Not enough persistent storage for the requested operation. */
    MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE,

    /*! Not enough memory for the requested operation. */
    MMS_RESULT_INSUFFICIENT_MEMORY, 
    
    /*! One of the fields To, Cc, Bcc is invalid */
    MMS_RESULT_INVALID_ADDRESS,

    /*! Invalid or not existing application header field. */
    MMS_RESULT_INVALID_APPLICATION_HEADER,
    
    /*! Invalid or not existing Content-Id. */
    MMS_RESULT_INVALID_CONTENT_ID,

    /*! Invalid or not existing Content-Location. */
    MMS_RESULT_INVALID_CONTENT_LOCATION,

    /*! Content type is invalid. */
    MMS_RESULT_INVALID_CONTENT_TYPE,
    
    /*! Expiry time is invalid. */
    MMS_RESULT_INVALID_EXPIRY_TIME,

    /*! The total size of the MMS header is invalid. */
    MMS_RESULT_INVALID_HEADER_SIZE,

    /*! Message class is invalid. */
    MMS_RESULT_INVALID_MSG_CLASS,

    /*! Reply Charging Deadline is invalid.  */
    MMS_RESULT_INVALID_REPLY_CHARGING_DEADLINE,

    /*! Size is invalid.  */
    MMS_RESULT_INVALID_SIZE,

    /*! The storage type is invalid. */
    MMS_RESULT_INVALID_STORAGE_TYPE,

    /*! Subject is invalid. */
    MMS_RESULT_INVALID_SUBJECT,

    /*! The MMS version number is invalid for the requested operation. */
    MMS_RESULT_INVALID_VERSION,
   
    /*! Date is invalid. */
    MMS_RESULT_INVALID_DATE,

    /*! Transaction WID missing. */
    MMS_RESULT_TRANSACTION_ID_MISSING,
    
    /*! The message is corrupt. */
    MMS_RESULT_MESSAGE_CORRUPT, 

    /*! The message was empty. */
    MMS_RESULT_MESSAGE_EMPTY, 
    
    /*! The message with the specified message WID could not be found. */
    MMS_RESULT_MESSAGE_ID_NOT_FOUND, 

    /*! No report should be sent to this message. */
    MMS_RESULT_NO_REPORT,

    /*! The queue holding immediate retrieval notifications is full. */
    MMS_RESULT_NOTIFICATION_QUEUE_FULL,

    /*! The recipient address is not set in the message. */
    MMS_RESULT_RECIPIENT_ADDRESS_NOT_SET,

    /*! The MMS Client has encountered an unrecoverable application error.
     *  The MMS Client must be stopped and restarted by the MMS Application.
     */
    MMS_RESULT_RESTART_NEEDED,
    
    /*! The MMS Client has been stopped due to an internal error. */
    MMS_RESULT_STOPPED,

    /*! The URI is not valid. */
    MMS_RESULT_URI_NOT_VALID,

    /*! The message type is wrong. */ 
    MMS_RESULT_WRONG_FILE_TYPE,

    /*! 
     * The received push notification was larger than 
     * MMS_MAX_NOTIFICATION_SIZE bytes.   
     */
    MMS_RESULT_MAX_NOTIFICATION_SIZE_EXCEEDED,
    
    /*! During a retrieval operation, the downloaded message was larger than 
     *  the configuration variable MMS_CFG_MAX_RETRIEVAL_SIZE.
     */
    MMS_RESULT_MAX_RETRIEVAL_SIZE_EXCEEDED,

    /*! Rights to extract body-part data is missing.
     * The body-part is protected by DRM.
     * The DRS is not able/allowed to pass the data allong.
     */
    MMS_RESULT_BODY_PART_MISSING_RIGHTS,

    /*! DRS module is not used then we do not support any DRM content
    */

    MMS_RESULT_DRM_NOT_SUPPORTED
} MmsResult;

/*! \struct MmsDeliveryInd holds the decoded elments from a M-Delivery.ind */
typedef struct
{
    WE_UINT8 version;    /*!< The verion of MMS (MMS Proxy/Realy) */
    char *messageId;      /*!< Unique reference assigned to the message */
    MmsAddress *to;       /*!< the recipent address */
    MmsTimeSec date;      /*!< date&time when the message was handled */
    MmsStatus status;     /*!< the status of the message wid send request */
    WE_UINT32 reportId;  /*!< Report WID */
} MmsDeliveryInd; 

/*! \struct MmsForwardConf 
 *  \brief A struct containing possible header fields for a forward confirm. 
 */
typedef struct
{             
    WE_UINT8 version;  /*!< The verion of MMS (MMS Proxy/Realy) */
    char *messageId;    /*!< Unique reference assigned to the message */
    void *responseText; /*!< Description which qualifies the response status value.*/
    MmsStatus status;   /*!< the status of the message wid send request      */
} MmsForwardConf;


/* 
 * ! \brief Signal struct replying progress status
 */ 
typedef struct 
{
    WE_UINT8 operation;        /*!< send/retrieve/imm-retrieve */
    WE_UINT8 state;            /*!< start/completed/progress/stop */
    MmsMsgId msgId;             /*!< folder manager file Id number */
    WE_UINT32 progress;        /*!< data read/sent */
    WE_UINT32 totalLength;     /*!< total data to send/read (estimate) */
    WE_UINT8 queueLength;      /*!< number of queued transmissions */
} MmsProgressStatus;

#define MMS_PROGRESS_STATUS_START      (0) /* Start of send/retrive */
#define MMS_PROGRESS_STATUS_COMPLETED  (1) /* Send/retrieve completed */
#define MMS_PROGRESS_STATUS_STOP       (2) /* fail/cancel */
#define MMS_PROGRESS_STATUS_PROGRESS   (3) /* Intermediate progress status */

#define MMS_PROGRESS_STATUS_SEND        (0) /* Send */
#define MMS_PROGRESS_STATUS_RETRIEVE    (1) /* Delayed retrive */
#define MMS_PROGRESS_STATUS_IMMRETRIEVE (2) /* Immediate retrieve */

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Prototypes ---*/

#endif  /* MMS_DEF_H */






















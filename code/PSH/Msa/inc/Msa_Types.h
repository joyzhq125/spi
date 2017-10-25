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

/* !\file matypes.h
 *  \brief Common data-types and constants used internally by the MSA.
 */

#ifndef MSA_TYPES_H
#define MSA_TYPES_H

#ifndef MMS_DEF_H
#error mms_def.h needs be included before matypes.h!
#endif

/******************************************************************************
 * Constants
 *****************************************************************************/

#define MSA_MAX_SMIL_OBJS           3   /*!< The maximum number of objects
                                             for one slide */

/*!\enum MsaConnectionType
 * \brief Network connection types 
 */
typedef enum
{
    MSA_CONNECTION_TYPE_HTTP,       /*!< HTTP */
    MSA_CONNECTION_TYPE_WSP_CL,     /*!< WAP connectionless  */
    MSA_CONNECTION_TYPE_WSP_CO      /*!< WAP connection oriented */
}MsaConnectionType;

/*! \enum MsaGetBpRspDest
 *	\brief Tells which FSM to send the response from the MMSif_getBodyPart to.
 */
typedef enum
{
    MSA_GET_BP_RSP_DEST_CRH = 0x01, /*!< The MMV FSM is the originator */
    MSA_GET_BP_RSP_DEST_MR          /*!< The MR FSM is the originator */
}MsaGetBpRspDest;

/*! \enum MsaGetHeaderRsp
 *	\brief Tells which FSM to send the response from the MMSif_getMsgHeader to.
 */
typedef enum
{
    MSA_GET_HEADER_RSP_MR = 0x01, /*!< The MR FSM is the originator */
    MSA_GET_HEADER_RSP_MCR        /*!< Forward the response to MCR  */
}MsaGetHeaderRsp;

/*! \enum MsaGetSkeletonRsp
 *	\brief Tells which FSM to send the response from the MMSif_getMsgSkeleton
 *         to.
 */
typedef enum
{
    MSA_GET_SKELETON_RSP_MR = 0x01, /*!< The MR FSM is the originator */
    MSA_GET_SKELETON_RSP_MCR        /*!< The MCR FSM is the originator */
}MsaGetSkeletonRsp;

/* === CONFIG MENU CONSTANTS === */

/*!\enum MsaConfigMenu
 *  
 */
typedef enum
{
    MSA_CONFIG_MENU_DEFAULT,  /*!< Entry point for menu system */
    MSA_CONFIG_MENU_ROOT,     /*!< Corresponds to #MsaConfigGroups */
    MSA_CONFIG_MENU_SEND,     /*!< Corresponds to #MsaConfigSend */
    MSA_CONFIG_MENU_FILTER,   /*!< Corresponds to #MsaConfigFilter */
    MSA_CONFIG_MENU_NETWORK,  /*!< Corresponds to #MsaConfigNetwork */
    MSA_CONFIG_MENU_MMS_SETTINGS,/*!< Corresponds to #MsaConfigSend */
    MSA_CONFIG_MENU_REP_CHRG_SETTINGS,/*< Corresponds to #MsaConfigReplyCharging*/
    MSA_CONFIG_MENU_RETRIEVE, /*!< Corresponds to #MsaConfigRetrieve */
    MSA_CONFIG_MENU_SIGNATURE,
    MSA_CONFIG_MENU_COUNT    /*!< Only for counting purposes */
}MsaConfigMenu;

/*! \enum MsaConfigGroups
 *  The different configuration groups/items
 */
typedef enum
{
    MSA_CONFIG_SENDING,                 /*!< Message sending properties */
    MSA_CONFIG_RETRIEVAL,               /*!< Message retrieval; delayed or immediate */
    MSA_CONFIG_FILTER,                  /*!< Filters for immediate retrieval */
    MSA_CONFIG_MMSC,                    /*!< Proxy host, URI, port and scheme */
    MSA_CONFIG_NETWORK,                 /*!< Network account, conn type, WAP gateway */
    MSA_CONFIG_GROUPS_COUNT             /*!< Not used, it is just a counter */
}MsaConfigGroups;

/*! \enum MsaConfigNetwork
 *  Configuration items for network
 */
typedef enum
{
    MSA_CONFIG_CONN_TYPE,   /*!< Connection type */
    MSA_CONFIG_NET_ACCOUNT, /*!< Network account */
    MSA_CONFIG_WAP_GW,      /*!< WAP gateway */
    MSA_CONFIG_LOGIN,       /*!< WAP GW Login */
    MSA_CONFIG_PASSWD,      /*!< WAP GW Password */
    MSA_CONFIG_REALM,       /*!< Realm */
    MSA_CONFIG_SEC_PORT,    /*!< Secure port */
    MSA_CONFIG_HTTP_PORT,   /*!< HTTP port */
    MSA_CONFIG_NETWORK_COUNT/*!< Not used, it is just a counter */
}MsaConfigNetwork;

/*! \enum MsaConfigFilter
 *  \brief Configuration items for message retrieval */
typedef enum
{
    MSA_CONFIG_IM_SERVER,       /*!< Immediate retrieval server */
    MSA_CONFIG_IM_SIZE,         /*!< Maximum msg size for immediate retrieval*/
    MSA_CONFIG_TREAT_AS_DEL,    /*!< Treat large msgs as delayed retrieval */
    MSA_CONFIG_RETR_SIZE,       /*!< Maximum retrieval size for a download */
    MSA_CONFIG_ALLOW_ANONYM,    /*!< Allow msgs with anonymous sender */
    MSA_CONFIG_ALLOW_PERSONAL,  /*!< Allow msgs with class PERSONAL */
    MSA_CONFIG_ALLOW_ADVERT,    /*!< Allow msgs with class ADVERTISEMENT */
    MSA_CONFIG_ALLOW_INFO,      /*!< Allow msgs with class INFORMATIONAL */
    MSA_CONFIG_ALLOW_AUTO,      /*!< Allow msgs with class AUTO */
    MSA_CONFIG_ALLOW_STRING,    /*!< Allow msgs with used defined string */
    Msa_CONFIG_RETR_COUNT       /*!< Not used, it is just a counter */
}MsaConfigFilter;

/*! \enum MsaConfigSend
 *  \brief Configuration items for message sending */
typedef enum
{
    MSA_CONFIG_EXP_TIME,    /*!< Expiry time */
    MSA_CONFIG_READ_REPORT, /*!< Read report */
    MSA_CONFIG_DELIV_REPORT,/*!< Delivery report */
    MSA_CONFIG_PRIORITY,    /*!< Priority */
    MSA_CONFIG_SAVE_ON_SEND,/*!< Save on send flag */
    MSA_CONFIG_SLIDE_DUR,   /*!< Slide duration */
    MSA_CONFIG_REPLY_CHRG_MENU, /*!< Reply charging menu */
    MSA_CONFIG_SENDER_VISIBILITY, /*!< Sender visibility, Show/Hide number */
    MSA_CONFIG_DELIVERY_TIME,           /*!< Delivery time */
    MSA_CONFIG_SIGNATURE_MENU,
    MSA_CONFIG_USE_SIGNATURE_MENU,
    MSA_CONFIG_SEND_COUNT   /*!< Not used, it is just a counter */
}MsaConfigSend;

/*! \enum MsaConfigRetrieve
 *  \brief Configuration items for message sending */
typedef enum
{
    MSA_CONFIG_HOME_NETWORK,    /*!< Home network */
    MSA_CONFIG_ROAM_NETWORK,    /*!< Roaming network */
    MSA_CONFIG_SEND_READ_REPORT,/*!< Send read report */
    MSA_CONFIG_DELIVERY_REPORT_ALLOWED, /*!< Delivery report allowed */
    MSA_CONFIG_RETRIEVE_COUNT   /*!< Not used, it is just a counter */
}MsaConfigRetrieve;

/*! \enum MsaConfigReplyCharging
 *  \brief Configuration items for reply charging */
typedef enum
{
    MSA_CONFIG_REPLY_CHARGING,          /*!< Reply charging */
    MSA_CONFIG_REPLY_CHARGING_DEADLINE, /*!< Reply charging deadline */
    MSA_CONFIG_REPLY_CHARGING_SIZE      /*!< Reply charging size */
}MsaConfigReplyCharging;

/*! \enum MsaConfigSignature
 *  \brief Configuration items for signature */
typedef enum
{
    MSA_CONFIG_USE_SIGNATURE,          /*!< Use signature */
    MSA_CONFIG_EDIT_SIGNATURE          /*!< Edit signature */
}MsaConfigSignature;


/*!\enum MsaAddrType
 * \brief Possible address-types when creating messages
 */
typedef enum
{
    MSA_ADDR_TYPE_NONE      = 0x00,     /*!< No valid address */
    MSA_ADDR_TYPE_PLMN      = 0x01,     /*!< Phone no */
    MSA_ADDR_TYPE_EMAIL     = 0x02      /*!< Email */
}MsaAddrType;

/*! \struct MsaMediaGroupType
 *  Media type definitions */
typedef enum
{
    MSA_MEDIA_GROUP_NONE  = 0x00,    /* !< None */
    MSA_MEDIA_GROUP_IMAGE = 0x01,    /* !< Image */
    MSA_MEDIA_GROUP_TEXT  = 0x02,    /* !< Text */
    MSA_MEDIA_GROUP_AUDIO = 0x04     /* !< Audio */
}MsaMediaGroupType;

/*! \typedef MsaTimeDef
 * 
 */
typedef enum
{
    MSA_1_HOUR   = 3600,    /*!< One hour */
    MSA_12_HOURS = 43200,   /*!< 12 hours */
    MSA_1_DAY    = 86400,   /*!< One day */
    MSA_1_WEEK   = 604800,  /*!< One week */
    MSA_1_YEAR   = 31449600 /*!< One year */
}MsaTimeDef;

/*! \typedef MsaSizeDef
 * 
 */
typedef enum
{
    MSA_1_KB   = 1024,
    MSA_10_KB  = 10240
}MsaSizeDef;

/*!\enum MsaMessageType */
typedef enum
{
    MSA_MR_PLAIN_TEXT,     /*!< The message is of type plain text */
    MSA_MR_SMIL,           /*!< The message is of type SMIL */
    MSA_MR_MULTIPART_MIXED,/*!< The message is of type Multipart/mixed */
    MSA_MR_MULTIPART_ALTERNATIVE, /*!< The message is of type 
                                Multipart/alternative*/
    MSA_MR_UNSUPPORTED     /*!< The message type is not supported */
}MsaMessageType;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*!\struct MsaLinkItem
 * \brief A data-structure for creating a list of links.
 */
typedef struct MsaLinkItemSt
{
    char *url;                  /*!< The link */
    struct MsaLinkItemSt *next; /*!< The next item or NULL */
}MsaLinkItem;

/*!\enum MsaConfig
 * \brief Configuration data that the user can set 
 */
typedef struct
{
    MmsAddressType      addressType;        /*!< MMS_EMAIL or MMS_PLMN*/
    unsigned int        proxyPort;          /*!< The proxy port e.g. 80 */
    int                 networkAccount;     /*!< The network account no */
    MsaConnectionType   connectionType;   /*!< The connection type */
    int                 retrievalMode;      /*!< 0 == automatic, 1 == always ask */
    int                 retrievalModeRoam;  /*!< 0 == automatic, 1 == always ask
                                             2 == Restricted */
    unsigned int        securePort;
    unsigned int        httpPort;
    unsigned int        maxMsgSize;         /*!< Maximum message size for Imm Ret*/
    unsigned int        maxRetrSize;        /*!< Maximum retrieval size */

    char                *fromName;          /*!< The name of user */
    char                *fromAddress;       /*!< The address of the user EMAIL or 
                                           PLMN*/
    char                *proxyHost;         /*!< host.domain.net */
    char                *proxyUri;          /*!< The proxy URI e.g. /proxy-relay */
    char                *proxyScheme;       /*!< The proxy relay scheme e.g. 
                                            http:// */
    char                *wapGateway;        /*!< WAP gateway */
    char                *login;             /*!< Login name for the WAP gateway */
    char                *passwd;            /*!< Password for the login */
    char                *realm;             /*!< Needed when using a proxy that 
                                             needs authentication */
    MmsVersion          proxyVersion;       /*!< The MMSC version */
	MsaTextAlignment    alignment;          /*!< The text alignment CR_17749*/

    /* Message settings */
    int                 saveOnSend;         /*!< 1=Save in sent fldr,0=Don't save*/
    int                 slideDuration;      /*!< Default slide duration in sec */
    WE_BOOL            readReport;         /*!< Read report ?? */
    WE_BOOL            deliveryReport;     /*!< Delivery report ??*/
    MsaPriorityType     priority;           /*!< Message priority */
    MsaExpiryTimeType   expiryTime;       /*!< The expiry date of the message */
    MsaReplyChargingType replyCharging;      /*!< Reply charging */
    MsaReplyChargingDeadlineType replyChargingDeadline; /*!< Reply charging deadline */
    MsaReplyChargingSizeType replyChargingSize;     /*!< Reply charging size */


    /* Immediate retrieval filters */
    int                 allowPersonal;      /*!< Allow class PERSONAL */
    int                 allowAdvert;        /*!< Allow class ADVERTISMENT */
    int                 allowInfo;          /*!< Allow class INFORMATIONAL */
    int                 allowAuto;          /*!< Allow class AUTO */
    int                 allowAnonym;        /*!< Allow anonymous senders */
    int                 treatAsDel;         /*!< Treat as delayed retrieval */
    char                *allowString;       /*!< Allow user defined string class */
    char                *imServer;          /*!< Immediate retrieval server */
    MsaVisibilityType   senderVisibility;   /*!< 1 == hide, 0 == show */
    int                 deliveryReportAllowed; /*!< 1 == No, 0 == Yes */
    int                 sendReadReport;     /*!< 2 == Never, 1 == On request, 
                                             0 == Always */
    int                 deliveryTime;       /*!< 3 == 24h, 2 == 12h, 1 == 1h, 
                                             0 == Immediate */

    /* Network Status variables */
    int                 roamingStatus;       /*! Roaming status,
                                             0 == not roaming, 1 == roaming */
    int                 bearerStatus;        /*! Bearer status, 
                                             0 == N/A, 1 == available */

    char                *signature;
    int                 useSignature;
} MsaConfig;

/*!\struct MsaAddrItem
 * \brief Data-structure for storing a destination address when creating 
 *        messages
 */
typedef struct MsaAddrItemSt
{
    MsaAddrType addrType;   /*!< The type of address, see #MsaAddrType */
    char *name;             /*!< The name of the dest user, e.g., 
                                 "Homer Simpson". UTF-8 is used for character 
                                 encoding. */
    char *address;          /*!< The address (according to #MsaAddrType) in 
                                 US-ASCII */
    struct MsaAddrItemSt *next; /*!< The next item */
}MsaAddrItem;

/*!\struct MsaMoFileRef
 * Data-structure for a media objects location */
typedef struct 
{
    char       *path;      /* !< Path to file */
    WE_UINT32 pos;        /* !< Position in file */
    WE_UINT32 size;       /* !< Size of object in file */
}MsaMoFileRef;

/*!\struct MsaMoDataItem 
 * Data-structure for one body part */
typedef struct MsaMoDataItemSt
{
    MmsContentType          *type;      /* !< The content-type 
                                              including parameters */
    char                    *name;      /* !< The name of the body-part */
    WE_UINT32              bpIndex;    /* !< The index of the body-part in 
                                              the message */
    unsigned char           *data;      /* !< Body-part data */
    WE_UINT32              size;       /* !< Body-part size in bytes */
    MsaMoFileRef            *file;      /* !< If allocated; MO location info */
    struct MsaMoDataItemSt  *next;      /* !< The next body-part or NULL */
}MsaMoDataItem;

/*!\struct MsaPropItem 
 * \brief Message properties 
 */
typedef struct
{
    MsaAddrItem *to;                /*!< To address */
    MsaAddrItem *cc;                /*!< Cc address */
    MsaAddrItem *bcc;               /*!< Bcc address */
    MsaAddrItem *from;              /*!< From address */
    char *subject;                  /*!< The message subject */
    WE_BOOL readReport;            /*!< Read report ?? */
    WE_BOOL deliveryReport;        /*!< Delivery report ??*/
    char *replyChargingId;          /*!< Reply charging wid */
    MsaPriorityType priority;       /*!< Message priority */
    MsaExpiryTimeType expiryTime;   /*!< The expiry date of the message */
    MsaReplyChargingType replyCharging; /*!< Reply charging */
    MsaReplyChargingDeadlineType replyChargingDeadline; /*!< Reply charging
                                                             deadline */
    MsaReplyChargingSizeType replyChargingSize; /*!< Reply charging size */
    MsaVisibilityType senderVisibility;   /*!< 1 == hide, 0 == show */
}MsaPropItem;

/*! \struct MsaMoItem
 *
 */
typedef struct 
{
    MsaMoDataItem   *object;    /*!< A reference to the object */
    WE_UINT32      textColor;  /*!< The text color */
    WE_UINT32      bgColor;    /*!< The background color */
    WE_UINT32      begin;      /*!< The object start time in ms */
    WE_UINT32      duration;   /*!< The object duration in ms */
}MsaObjInfo;

/*!\struct MsaSmilSlide
 * The representation for one slide 
 */
typedef struct MsaSmilSlideSt
{
    MsaObjInfo *objects[MSA_MAX_SMIL_OBJS]; /* !< The objects that are 
                                                  present in the slide */
    unsigned int duration;                  /* !< The slide duration */
    struct MsaSmilSlideSt *next;            /* !< The next slide or NULL */
}MsaSmilSlide;

/*!\struct MsaSmilInfo 
 * SMIL information 
 */
typedef struct
{
    WE_UINT32      bgColor;        /* !< The background color for the SMIL 
                                          presentation */
    MsaSmilSlide    *currentSlide;  /* !< The slide that is currently active*/
    MsaObjInfo      *currentMo;     /* !< The media object that is active */
    
    MsaMoDataItem   *mediaObjects;  /* !< The list of media objects */
    unsigned int    noOfSlides;     /* !< The total number of slides */
    MsaSmilSlide    *slides;        /* !< The list of slides */
    WE_UINT32      fgColorDefault;  /* !< The default color for text */
}MsaSmilInfo;

/*!\struct MsaMessage
 * \brief Used to transport a message through a signal when done here
 */
typedef struct 
{
    /* Startup data */
    MmsMsgId            msgId;      /*!< The current message wid */
    MmsFileType         fileType;   /*!< The type of message to open */

    /* Derived data */
    char            *fileName;      /*!< The filename of the message */
    MsaMessageType  msgType;        /*!< The message type */

    /* The message properties */
    MsaPropItem     *msgProperties; /*!< Message properties */    

    /* The new SMIL presentation */
    MsaSmilInfo     *smilInfo;      /*!< The new SMIL presentation */
}MsaMessage;

/*! \struct MsaCtContentInsert
 *  Used to start the ME FSM and insert content into a SMIL presentation
 */
typedef struct
{
    unsigned char   *buffer;    /*!< The buffer containing the image */
    WE_UINT32      bufferSize; /*!< The size of the buffer */
    char            *name;      /*!< The name of the media object */
    char*           mimeType;   /*!< The media type of the content */
    MsaPropItem     *propItem;  /*!< Message properties */
}MsaCtContentInsert;

#endif


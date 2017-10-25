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

/*! \file Mms_If.h
 * \brief  Exported signal functions towards the MMS Service.
 *
 * Exported connector functions, i.e. from Target Device Application to the MMS 
 * client. For every connector function, the following data are listed:
 * - A brief one line description of the function
 * - A longer description of the function
 * - The type of the function. It can be either:
 *     -# Signal function: The function encapsulates the passed parameters into
 *        a WE signal and sent it to the MMS Service 
 *        The result will be returned in a reply signal.
 *
 *     -# Converter function: unpack the recived signals into the data types
 *        used by the MFS. This function is to be registrated in WE
 *      
 * - Reply signal name
 * - The parameters for the function
 */

#ifndef MMS_IF_H
#define MMS_IF_H

#ifndef MMS_CFG_H  /* Check so the Mms_Cfg.h file is included before this file */
#error You must #include Mms_Cfg.h before this file.
#error The file Mms_Cfg.h contains configuration data for all other MMS files.
#error Please change the .c file that stopped from compiling because you came here
#error so that it includes Mms_Cfg.h before this file.
#endif /* MMS_CFG_H */

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*! \typedef MmsIfConvertFunction
 * Function pointer for convertion functions of the type mms_cvt_xxx(...)
 */
typedef int MmsIfConvertFunction(we_dcvt_t *obj, void *data);
typedef int MmsIfConvertFunctionArray(we_dcvt_t *obj, void *data, 
    WE_UINT32 size);

/*--- Constants ---*/

/* Signals */

/* 
 *!\brief Signal Identifications 
 *                        Format:  16 bit = ! 8bit   ! 8bit  ! 
 *                                          !  SigId ! Modid !
 */

/*--- Replied signals from MMS Service ---*/
#define MMS_SIG_CREATE_MSG_REPLY        ((WE_MODID_MMS << 8) + 0x01) 
#define MMS_SIG_DELETE_MSG_REPLY        ((WE_MODID_MMS << 8) + 0x02)
#define MMS_SIG_FORWARD_REQ_REPLY       ((WE_MODID_MMS << 8) + 0x03)
#define MMS_SIG_GET_NUM_OF_MSG_REPLY    ((WE_MODID_MMS << 8) + 0x04)
#define MMS_SIG_MOV_MSG_REPLY           ((WE_MODID_MMS << 8) + 0x05)
#define MMS_SIG_RETRIEVE_MSG_REPLY      ((WE_MODID_MMS << 8) + 0x06)
#define MMS_SIG_SEND_MSG_REPLY          ((WE_MODID_MMS << 8) + 0x07)
#define MMS_SIG_READ_REPORT_IND         ((WE_MODID_MMS << 8) + 0x08)
#define MMS_SIG_DELIVERY_REPORT         ((WE_MODID_MMS << 8) + 0x09)
#define MMS_SIG_ERROR                   ((WE_MODID_MMS << 8) + 0x0A)
#define MMS_SIG_NOTIFICATION            ((WE_MODID_MMS << 8) + 0x0B)
#define MMS_SIG_SEND_RR_REPLY           ((WE_MODID_MMS << 8) + 0x0C)
#define MMS_SIG_MSG_DONE_REPLY          ((WE_MODID_MMS << 8) + 0x0D)
#define MMS_SIG_GET_FLDR_REPLY          ((WE_MODID_MMS << 8) + 0x0E)
#define MMS_SIG_FOLDER_STATUS_REPLY     ((WE_MODID_MMS << 8) + 0x0F)
#define MMS_SIG_PROGRESS_STATUS_REPLY   ((WE_MODID_MMS << 8) + 0x10)
#define MMS_SIG_GET_MSG_INFO_REPLY      ((WE_MODID_MMS << 8) + 0x11)
#define MMS_SIG_GET_MSG_HEADER_REPLY    ((WE_MODID_MMS << 8) + 0x12) /* of type MmsGetMsgHeaderReply */
#define MMS_SIG_GET_MSG_SKELETON_REPLY  ((WE_MODID_MMS << 8) + 0x13) /* of type MmsGetSkeletonReply */
#define MMS_SIG_GET_BODY_PART_REPLY     ((WE_MODID_MMS << 8) + 0x14) /* of type MmsGetBodyPartReply */
#define MMS_SIG_GET_CUSTOM_DATA_REPLY   ((WE_MODID_MMS << 8) + 0x15) /* of type MmsGetCustomDataReply */
#define MMS_SIG_SET_CUSTOM_DATA_REPLY   ((WE_MODID_MMS << 8) + 0x16) /* of type MmsGetCustomDataReply */

/*--- Signals sent to MMS Service ---*/             
#define MMS_SIG_CREATE_MSG           ((WE_MODID_MMS << 8) + 0x30) 
#define MMS_SIG_DELETE_MSG           ((WE_MODID_MMS << 8) + 0x31) 
#define MMS_SIG_FORWARD_REQ          ((WE_MODID_MMS << 8) + 0x32) 
#define MMS_SIG_GET_NUM_OF_MSG       ((WE_MODID_MMS << 8) + 0x33) 
#define MMS_SIG_MOVE_MSG             ((WE_MODID_MMS << 8) + 0x34) 
#define MMS_SIG_RETRIEVE_MSG         ((WE_MODID_MMS << 8) + 0x35) 
#define MMS_SIG_RETRIEVE_MSG_CANCEL  ((WE_MODID_MMS << 8) + 0x36) 
#define MMS_SIG_SEND_MSG             ((WE_MODID_MMS << 8) + 0x37) 
#define MMS_SIG_SEND_MSG_CANCEL      ((WE_MODID_MMS << 8) + 0x38) 
#define MMS_SIG_READ_REPORT          ((WE_MODID_MMS << 8) + 0x39) 
#define MMS_SIG_CONFIG_STR           ((WE_MODID_MMS << 8) + 0x3A) 
#define MMS_SIG_MSG_DONE             ((WE_MODID_MMS << 8) + 0x3B) 
#define MMS_SIG_GET_FLDR             ((WE_MODID_MMS << 8) + 0x3C) 
#define MMS_SIG_SET_READ_MARK        ((WE_MODID_MMS << 8) + 0x3D) 
#define MMS_SIG_GET_REPORT           ((WE_MODID_MMS << 8) + 0x3E) 
#define MMS_SIG_FOLDER_STATUS        ((WE_MODID_MMS << 8) + 0x3F) 
#define MMS_SIG_APP_REG              ((WE_MODID_MMS << 8) + 0x40) 
#define MMS_SIG_APP_UNREG            ((WE_MODID_MMS << 8) + 0x41) 
#define MMS_SIG_GET_MSG_INFO         ((WE_MODID_MMS << 8) + 0x42) 
#define MMS_SIG_GET_MSG_HEADER       ((WE_MODID_MMS << 8) + 0x43) 
#define MMS_SIG_GET_MSG_SKELETON     ((WE_MODID_MMS << 8) + 0x44) 
#define MMS_SIG_GET_BODY_PART        ((WE_MODID_MMS << 8) + 0x45) 
#define MMS_SIG_GET_CUSTOM_DATA      ((WE_MODID_MMS << 8) + 0x46) 
#define MMS_SIG_SET_CUSTOM_DATA      ((WE_MODID_MMS << 8) + 0x47) 

/*--- Signal parameters ---*/

typedef enum
{
    MMS_VALUE_TYPE_UNKNOWN        = 0xFF,
        MMS_VALUE_TYPE_UINT32         = 0x00, /* Uses 4-bytes */
        MMS_VALUE_TYPE_INT32          = 0x01, /* Uses 4-bytes */
        MMS_VALUE_TYPE_UTF8           = 0x02, /* Definable size */
        MMS_VALUE_TYPE_BOOL           = 0x03, /* Uses 1-Byte */
        MMS_VALUE_TYPE_BINARY         = 0x04  /* Definable size */
} MmsCustomDataType;

typedef struct 
{
    MmsCustomDataId   wid;           /*!< Id reffering to this custom data spec. */
    int               ubsKey;       /*!< Related UBS property-key */
    MmsCustomDataType valueType;    /*!< UBS value type of property-key */
    int               dataLen;      /*!< Length of data */
    WE_BOOL          propagate;    /*!< Shall data propare at copy */
    int               offset;       /*!< Calulated by service (set to zero in define)*/
} MmsCustomDataSpec;

/* 
 *! \brief Signal struct for sending message file type
 */ 
typedef struct
{
    MmsFileType eFileType; /*!< The file type */
} st_MmsFileTypeSig; 


/* 
 *! \brief Signal struct for sending application registration 
 */ 
typedef struct
{
    WE_UINT8 source; /*!< The caller WE module wid */
} MmsAppReg; 
 
/*
 *! \brief Signal Struct sending message wid
 */
typedef struct 
{
   MmsMsgId  msgId; /*!< Message identification */
} MmsMsgSig;


typedef struct 
{
   WE_UINT8      source;   /*!< Requesting module */
   MmsFolderType  folderId; /*!< Folder identification */
} MmsFolderSig;

/*
 * ! \brief Signal Struct sending message move/create done 
 */
typedef struct 
{
    MmsMsgId msgId; /*!< Message identification */
    MmsFolderType folder; /*!< folder */
} MmsMsgFolderSig;

/*
 * ! \brief Signal Struct sending read report
 */
typedef struct 
{
    MmsMsgId msgId; /*!< Message identification */
    MmsReadStatus readStatus; /*! Read status */
} MmsSendReadReport; 

/* 
 * ! \brief Signal Struct for manage the read mark for a specified file 
 */
typedef struct 
{
    MmsMsgId msgId;  /*!< Message identification */
    WE_UINT8 value; /*!< 1= set or 0= clear */
} MmsSetReadMark;    
    
/*
 * !\brief Signal struct for sending PDU 
 */
typedef struct
{
    WE_UINT32 length;   /*!< length of PDU */
    unsigned char *pdu;  /*!< vector holding the PDU */
} MmsPduSig; 

/* 
 * !\breif Signal struct for retrieving status for the
 *   persistent storage (mms folder)
 */
typedef struct 
{
    WE_UINT32 usedMsgBytes; /*!< the total size of files visible in MMT list*/
    WE_UINT32 numOfMsg;     /*!< total number of message */ 
    WE_UINT32 totalFolderSize; /*!< total size of all files in the MMS folder */
} MmsFolderStatus; 

/* 
 *! \brief Signal struct for requesting custom data
 */ 
typedef struct
{
    WE_UINT32        msgId;        /* Message Id */
    MmsCustomDataId   wid;           /*!< Id referring to this custom data spec. */
    int               ubsKey;       /*!< Related UBS property-key */
} MmsCustomDataRequestSig; 

/* 
 *! \brief Signal struct for get/setting custom data
 */ 
typedef struct
{
    WE_UINT32        msgId;        /* Message Id */
    MmsCustomDataId   wid;           /*!< Id referring to this custom data spec. */
    int               ubsKey;       /*!< Related UBS property-key */
    MmsCustomDataType valueType;    /*!< UBS value type of property-key */
    int               dataLen;      /*!< Length of data */
    char              *data;        /*!< The data to set */
} MmsCustomDataSig;

/******************************************************************************* 
 * Signal parameters sent from the MMS Service 
 ******************************************************************************/

/*
 *! \brief Signal Struct replying result
 */
typedef struct 
{  
    MmsResult result; /*!< Returned operation code */
} MmsResultSig;

/*
 *! \brief Signal Struct replying result
 */
typedef struct 
{  
    MmsResult result; /*!< Returned operation code */
    WE_UINT32 msgId; /*!< Message reference identification */  
} MmsMsgDoneReplySig;

/*
 *! \brief Signal Structs replying data  
 */
typedef struct 
{
   MmsResult result; /*!< Returned operation code */
   MmsMsgId msgId;   /*!< Message reference identification */
} MmsMsgReplySig;

/*
 *! \brief Signal Structs replying data  
 */
typedef struct 
{
   MmsResult result; /*!< Returned operation code */
   MmsMsgId msgId;   /*!< Message reference identification */
   MmsMsgId rrId;    /*!< Read-report reference identification */
} MmsRRReplySig;

/*
 * ! \brief Signal struct replying number of message 
 */
typedef struct 
{ 
    MmsResult result;      /*!< Returned operation code */
    WE_UINT32 number;     /*!< Number of messages */
    MmsFolderType folder;  /*!< Folder identity */
} MmsMsgReplyNoOfMsg;

/*
 * ! \brief Signal struct service  indication for received message
 */
typedef struct
{
    MmsNotificationType type;   /*!< Type of notification, (eg. Notification */
    MmsMsgId notifId;           /*!< Message reference identification */
    WE_UINT32 size;            /*!< Size of message */

    /* Sender address, normally an MmsAddress but here splitted into plain types */
    MmsCharset  fromCharset;    /*!< Well-known character set as defined by IANA */
    char        *fromName;      /*!< Display name (e.g. Bill Löfgren, AU-System) */
    char        *fromAddress;   /*!< Address (e.g. bill.lofgren@ausystem.se) */
    MmsAddressType fromAddrType;/*!< Address Type (PLMN or EMAIL) */

    /* Subject, normally an MmsEncodedText but here splitted into plain types */
    MmsCharset  subjectCharset; /*!< Well-known character set as defined by IANA */
    char        *subjectText;   /*!< Text buffer, must be NULL terminated */
} MmsMsgReplyNotification;

/*
 * ! \brief Signal struct received read report 
 */
typedef struct 
{
    char *address;            /*!< Address (e.g. bill.lofgren@ausystem.se) */
    MmsAddressType addrType;  /*!< Address Type (PLMN or EMAIL) */
    MmsCharset  charset;      /*!< Well-known character set as defined by IANA */
    char        *text;        /*!< Text buffer, must be NULL terminated */ 
    MmsTimeSec date;          /*!< Arrival time of the message at the MMSProxy-Relay. */
    char *serverMessageId;    /*!< Unique reference assigned to the message */
    MmsReadStatus readStatus; /*!< Read-Status of the message.*/
    WE_UINT32 msgId;         /*!< Message is to the orignated message or zero if it is 
                                   not found */
    WE_UINT32 msgIdFile;     /*!< Message wid for the stored message */        
} MmsMsgReadReport; 

/*
 * ! \brief Signal struct get message header reply
 */
typedef struct 
{
    MmsResult result;         /*!< Result code 0 = OK */
    MmsMsgId msgId;           /*!< Msg-wid  */
    WE_UINT32 userData;      /*!< Custom data as sent in MMSif_getMsgHeader */
    MmsGetHeader *header;     /*!< Message header data structure */
} MmsGetMsgHeaderReply; 

/*
 * ! \brief Signal struct get message header reply
 */
typedef struct 
{
    MmsResult result;         /*!< Result code 0 = OK */
    MmsMsgId msgId;           /*!< Msg-wid containing body-part */
    WE_UINT32 userData;      /*!< Custom data as sent in MMSif_getMsgSkeleton */
    MmsBodyInfoList *infoList;/*!< List of body-part information */
} MmsGetSkeletonReply; 

/*
 * ! \brief Signal struct get body part
 */
typedef struct 
{
    MmsResult result;         /*!< Result code 0 = OK */
    MmsMsgId msgId;           /*!< Msg-wid containing body-part */
    WE_UINT32 userData;      /*!< Custom data as sent in MMSif_getBodyPart */
    WE_UINT16 bodyPartId;    /*!< Body-part index */
    unsigned char *pipeName;  /*!< Pipe returning body-part if successful (freed by receiver, signel destruct) */
    WE_UINT32 bodyPartSize;  /*!< Size of the body-part data beeing fetched. */
} MmsGetBodyPartReply; 
/*
 * ! \brief Signal struct get message request signal structure
 */
typedef struct 
{
    MmsMsgId msgId;           /*!< Msg-wid for request */
    WE_UINT32 userData;      /*!< Custom data to be replied back */
} MmsGetMsgRequest; 

/*
 * ! \brief Signal struct get message body-part request signal structure
 */
typedef struct 
{
    MmsMsgId msgId;           /*!< Msg-wid for request */
    WE_UINT16 bodyPartId;    /*!< Body-part Id */
    WE_UINT32 userData;      /*!< Custom data to be replied back */
} MmsGetBodyPartRequest; 

/* 
 * ! \brief Signal struct received delivery report
 */ 
typedef struct 
{
    char *serverMsgId;          /*!< The unique reference (assigned by the MMS Proxy-Relay)
                            to the message. */
    char *address;              /*!< The recipent address. */
    MmsAddressType addrType;    /*!< Address Type (PLMN or EMAIL) */
    MmsCharset  charset;        /*!< Well-known character set as defined by IANA */
    char        *text;          /*!< Arrival time of the message at the MMSProxy-Relay. */
    MmsTimeSec date;            /*!< Date and time when the message was handled */
    MmsStatus status;           /*!< Status of the message. */
    WE_UINT32 msgId;           /*!< Message wid to the orignated message or zero not
                                     found */ 
    WE_UINT32 msgIdFile;       /*!< Message wid for the stored message */    
} MmsMsgDeliveryReportInd;

/* 
 * ! \brief Signal struct for requesting a conent list of message.
 */ 
typedef struct
{
  MmsFolderType folderId;  /*!< The folder where message is stored */
  WE_INT32 from;          /*!< starting number in the list        */
  WE_INT32 to;            /*!< stoping number in the list         */
} MmsMsgContentReq;

#define MMS_DRM_STATUS_FWDL 1
#define MMS_DRM_STATUS_FWDL_SHIFT 0
#define MMS_DRM_STATUS_DCF  2
#define MMS_DRM_STATUS_DCF_SHIFT  1
/*
 *! \brief Information kept for every message stored in a folder.
 */
typedef struct 
{
    MmsFolderType folderId; /*!< the folder location */
    WE_UINT32 msgId;       /*!< message Id          */
    WE_UINT32 size;        /*!< size of the message */
    WE_UINT32 date;        /*!< date of the message */
    char serverMsgId[MAX_MMT_SERVER_MSG_ID_LEN]; /*!< server message wid */
    char address[MAX_MMT_ADDRESS_LEN]; /*!< address of sender or receiver of the message */
    char subject[MAX_MMT_SUBJECT_LEN]; /*!< the subject of the message */ 
	#ifdef MMS_CUSTOMDATA_IN_USE
	WE_UINT8 customData[MMS_CUSTOM_DATA_SIZE]; /*< Custom data defined in Mms_Cfg.h */
	#endif
	MmsFileType suffix;     /*!< define the type of file */
    WE_UINT8 sent;         /*!< is set to 1 if sent */
    WE_UINT8 read;         /*!< is set to 1 if read */ 
    WE_UINT8 rrReq;        /*!< no of requested read reports */ 
    WE_UINT8 rrRead;       /*!< no of read read reports */
    WE_UINT8 rrDel;        /*!< no of deleted read reports */
    WE_UINT8 drReq;        /*!< no of requested delivery reports */
    WE_UINT8 drRec;        /*!< no of received delivery reports */    
    WE_UINT8 drmStatus;    /*!< Bit0: Is message forward locked (DRM) Bit1: Does msg have DCF's */
    MmsPriority priority;   /*!< Priority of message */
	int  class;              /*!< Class of message TR18100 */
} MmsMessageFolderInfo;

typedef struct
{
    MmsMessageFolderInfo     msgFolderInfo;
    WE_UINT8                rrListLength;
    WE_UINT32               *rrList; /* List of MsdIds */
    WE_UINT8                drListLength;
    WE_UINT32               *drList; /* List of MsgIds */
} MmsMessageProperties;

/*
 * ! \brief Signal struct replying a list of stored message in folder 
 *          reply data MMS_SIG_GET_FOLDER_CONTENT
 */
typedef struct 
{
    WE_UINT32 noOfElements;                    /*!< Number of elements in the array below */ 
    MmsMessageProperties *msgPropertyList;      /*!<    First field in an array of unspecified length */
} MmsListOfProperties;


/*--- Prototypes ---*/

/*!
 * \brief Start the interface
 *
 * This function starts the interface towards the MMS Service. It must 
 * be called before any operation in the interface is used. 
 *
 * Type: Function call
 *
 *****************************************************************************/
void MMSif_startInterface(void);

/*!
 * \brief Registry the application to the service
 *
 * This function registry the MMS application (MSA) as a receiver for 
 * notification signals. Hence, this function needs to be called before the 
 * application can receive any spontanious signals. 
 *
 * Type: Function signal
 *      signal name: #MMS_SIG_APP_REG
 *      data type    #MmsAppReg
 *
 * \param source The calling module identity
 *****************************************************************************/
 void MMSif_applicationReg(WE_UINT8 source);
 
/*!
 * \brief Unregister the application from the service
 *
 * This function registry the MMS application (MSA) as a receiver for 
 * notification signals. Hence, this function needs to be called before the 
 * application can receive any spontanious signals. 
 *
 * Type: Function signal
 *      signal name: #MMS_SIG_APP_UNREG
 *      data type    #MmsAppReg
 *
 * \param source The calling module identity
 *****************************************************************************/
 void MMSif_applicationUnreg(WE_UINT8 source);
 
/*!
 * \brief Creates an empty message in the draft folder.
 *
 * All operations (e.g. send, move, modify) on messages in the MMS client uses 
 * a unique message WID. Before these operations can be performed, a message 
 * must be created in the Draft folder. This function performs both operations.
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #MMS_SIG_CREATE_MSG_REPLY
 *      data type    #MmsMsgReplySig
 *
 * \param source The calling module identity
 *****************************************************************************/
void MMSif_createMessage(WE_UINT8 source,  MmsFileType fileType);

/*!
 * \brief Fetches the message-header from a message.
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #MMS_SIG_GET_MSG_HEADER_REPLY
 *      data type    #MmsGetMsgHeaderReply
 *
 * \param source     The calling module identity
 * \param msgId      The message containing the body-part
*****************************************************************************/
void MMSif_getMsgHeader(WE_UINT8 source,
                       MmsMsgId msgId,
                       WE_UINT32 userData);

/*!
 * \brief Fetches the message-skeleton from a message.
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #MMS_SIG_GET_SKELETON_REPLY
 *      data type    #MmsReplyGetSkeleton
 *
 * The skeleton fetched will contain a list of body-parts and
 * for each body-part information will be provided regarding:
 * - BP WID
 * - Number of sub-parts
 * - BP content-type
 * - BP content size
 * - Entry-header
 * - DRM status
 * - DRM DCF-filename
 *
 * \param source     The calling module identity
 * \param msgId      The message containing the body-part
 *****************************************************************************/
void MMSif_getMsgSkeleton(WE_UINT8 source,
                          MmsMsgId msgId,
                          WE_UINT32 userData);

/*!
 * \brief Fetches a body part from a message.
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #MMS_SIG_GET_BODY_PART_REPLY
 *      data type    #MmsReplyGetBodyPart
 *
 * \param source     The calling module identity
 * \param msgId      The message containing the body-part
 * \param bodyPartId The index of the body-part.
*****************************************************************************/
void MMSif_getBodyPart(WE_UINT8 source,
                       MmsMsgId msgId, WE_UINT16 bodyPartId, 
                       WE_UINT32 userData);


/*
 *	Custom data functions:
 */
void MMSif_getCustomData(   WE_UINT32      msgId,        /* Message Id */
                            MmsCustomDataId wid,           /*!< Id referring to this custom data spec. */
                            int             ubsKey        /*!< Related UBS property-key */
);


void MMSif_setCustomData(   WE_UINT32        msgId,        /* Message Id */
                            MmsCustomDataId   wid,           /*!< Id referring to this custom data spec. */
                            int               ubsKey,       /*!< Related UBS property-key */
                            MmsCustomDataType valueType,    /*!< UBS value type of property-key */
                            int               dataLen,      /*!< Length of data */
                            char              *data         /*!< The data to set */
);

/*!
 * \brief Deletes a stored message.
 *
 * A previously stored message is deleted from whatever folder it is stored in.
 * Both body and header part of the message is deleted.
 *
 * Type: Signal Function
 *
 * Reply signal
 *      signal name: #MMS_SIG_DELETE_MSG_REPLY
 *      data type    #MmsResultSig
 *
 * \param source The calling module identity
 * \param msgId A unique identity for a multimedia message.
 *****************************************************************************/
void MMSif_deleteMessage(WE_UINT8 source, MmsMsgId msgId);

/*!
 * \brief Creates and sends a forward request to a MMS-Proxy-Relay.
 *
 * When a notification has been recieved, the MMS Application can send a 
 * request to the MMS-Proxy-Relay to directly forward the message to another 
 * recipient before the actually message has been fetched.
 *
 * Type: Signal Function
 *
 * Reply signal:
 *       signal name: #MMS_SIG_FORWARD_REQ_REPLY 
 *       data type:   MmsResultSig
 *
 * \param source The calling module identity
 * \param forward Information about the forward to be made, this parameter must 
 *        be copied if needed after the function returned.
 * \param contentlocation The content location from found in the notification 
 *        message to be used in the forward request.
 * \param version The protcol (PDU) version 
 *****************************************************************************/
MmsResult MMSif_forwardReq(WE_UINT8 source, const MmsForward *forward, 
     char *contentLocation, MmsVersion version);

/*!
 * \brief Get the number of messages in one or all folder.  
 *
 * Type: Signal Function
 *
 * Reply signal:
 *       signal name: #MMS_SIG_GET_NUM_OF_MSG_REPLY
 *       data type:   #MmsMsgReplyNoOfMsg
 *
 * \param source The calling module identity
 * \param folder What folder to count messages in. If the request concerns 
 *        all folders MMS_ALL_FOLDERS should be used. 
 *****************************************************************************/
void MMSif_getNumberOfMessages(WE_UINT8 source, MmsFolderType folder);

/*!
 * \brief Move a message between folders. 
 *
 * A message indicated by the message WID is moved from the current folder 
 * to the new folder that is given as parameter.
 *
 * Type: Signal Function 
 *
 * Reply signal:
 *       signal name: #MMS_SIG_MOV_MSG_REPLY
 *       data type:   #MmsResultSig
 *
 * \param source The calling module identity
 * \param msgId A unique identity for a multimedia message.
 * \param folder The folder that the message shall be moved to. 
 *        NOTE! MMS_ALL_FOLDERS is not a vaild value in this operation.
 *****************************************************************************/
void MMSif_moveMessage(WE_UINT8 source, MmsMsgId msgId,
    MmsFolderType toFolder);

/*!
 * \brief Cancel an ongoing Retrieve operation for a message.
 *
 * An ongoing Retrieve operation on a multimedia message is cancelled.
 * Only one type of operation can be executed at a time so the last
 * call to MMSc_retrieveMessage for the given notification WID is terminated. 
 *
 * Type: Signal Function
 *
 * Reply signal name: None.
 *
 * \param source The calling module identity
 * \param notifId The identity of a notification.
 *****************************************************************************/
void MMSif_retrieveCancel(WE_UINT8 source, MmsMsgId notifId);

/*!
 * \brief Retrieve a message from the MMS server.
 *
 * A multimedia message is retrieved from the MMS Proxy-Relay and is stored in the 
 * Inbox.
 *
 * Type: Signal Function
 *
 * Reply signal
 *       signal name: #MMS_SIG_RETRIEVE_MSG_REPLY
 *       data type:   #MmsMsgReplySig
 *                  
 * \param source The calling module identity
 * \param notifId The identity of a notification.
 *****************************************************************************/
void MMSif_retrieveMessage(WE_UINT8 source, MmsMsgId notifId);

/*!
 * \brief Cancel an ongoing Send operation for a message.
 *
 * An ongoing Send operation on a multimedia message is cancelled.
 * Only one type of operation can be executed at a time so the last call
 * to MMSc_sendMessage for the given message WID is terminated.
 *
 * Type: Signal Function 
 *
 * Reply signal name: None.
 *
 * \param source The calling module identity
 * \param msgId The unique identity for a multimedia message.
 *****************************************************************************/
void MMSif_sendCancel(WE_UINT8 source, MmsMsgId msgId);

/*!
 * \brief Sends a Multimedia Message
 * 
 * The Multimedia Message must exist and be stored in one 
 * of the folders before this function can be called. 
 *
 * Type: Signal Function 
 *
 * Reply signal:
 *       signal name: #MMS_SIG_SEND_MSG_REPLY
 *       data type:   #MmsResultSig
 *
 * \param source The calling module identity
 * \param msgId A unique identity for a multimedia message.
 *****************************************************************************/
void MMSif_sendMessage(WE_UINT8 source, MmsMsgId msgId);

/*!
 * \brief Sends a Read Report for a Multimedia Message
 * 
 * The TDA has detected that the Multimedia Message identified by msgId 
 * contains a Read Report flag and the TDA has decided that a Read Report shall 
 * be sent. When calling this function, all data needed will be collected an 
 * the Read Report will be sent.
 *
 * Type: Signal Function 
 *
 * Reply signal 
 *       signal name: #MMS_SIG_SEND_RR_REPLY
 *       data type:   #MmsResultSig
 *
 * \param source The calling module identity
 * \param msgId A unique identity for a multimedia message.
 * \param readStatus Status of the message to send read report on.
 *****************************************************************************/
void MMSif_sendReadReport(WE_UINT8 source, MmsMsgId msgId,
    MmsReadStatus readStatus);
                        
/*!
 * \brief Indicates end of messge creation
 *
 * Type: Signal Function 
 *
 * Reply signal: 
 *       signal name: #MMS_SIG_MSG_DONE_REPLY
 *       data type:   #MmsMsgReplySig
 *
 * This function indicates to the MMS client that the creation of a message is 
 * done. When creating a Multimedia Message, it can contain multiple parts. 
 * When this connector is called, no more parts will be added. 
 *
 * \param msgId A unique identity for a multimedia message.
 *****************************************************************************/
 void MMSif_createMessageDone(WE_UINT8 source, MmsMsgId msgId, MmsFolderType folder);

/*!
 * \brief Request a content of messages from a folder
 *
 * Type: Signal Function 
 *
 * Reply signal
 *       signal name: #MMS_SIG_GET_FLDR_REPLY
 *       data type: #MmsListOfProperties.
 *
 * This function reqests a list of message information stored in one of the 
 * folders.
 *
 * \param folderId The folder where message is stored.  
 * \param from  starting number in the list
 * \param to    stoping number in the list
 *****************************************************************************/
 void MMSif_getFolderContent(WE_UINT8 source, MmsFolderType folderId, 
      int from, int to);

/*!
 * \brief Request information about a specific messages
 *
 * Type: Signal Function 
 *
 * Reply signal
 *       signal name: #MMS_SIG_GET_MSG_INFO_REPLY
 *       data type: #MmsListOfProperties.
 *
 * This function reqests a information about a message.
 *
 * \param source, Module WID of the requesting module (caller).  
 * \param msgId, Message that information is requested for.
 *****************************************************************************/
 void MMSif_getMessageInfo(WE_UINT8 source, WE_UINT32 msgId);

/*!
 * \brief Set the the read mark file 
 *
 * Type: Signal Function 
 *
 * Related Signal: None
 *
 * This function manage the setting of the read mark value for a file. 
 * The function is used to mark the file if it has been read or not. 
 *
 * \param msgId The message wid that be assigned with the read mark
 * \param value Set value. 1 = read, 0 = not read. 
 *****************************************************************************/
void MMSif_setReadMark(WE_UINT8 source, MmsMsgId msgId, WE_UINT8 value);

/*!
 * \brief Get report conent
 * 
 * Type: Signal Function 
 * 
 * Related Signal: Depends of the report type 
 *  
 *                #MMS_SIG_READ_REPORT if the requested report was a 
 *                                     read report. 
 *                                     data type: MmsMsgReadReport  
 *
 *                #MMS_SIG_DELIVERY_REPORT if the requested report was a 
 *                                        delivery report.
 *                                        data type: MmsMsgDeliveryReportInd
 *
 *                #MMS_SIG_ERROR is used for reporting faults detected during 
 *                               this operation. 
 *
 * The function request for the content in a PDU of the type Delivery or 
 * Read Report. Note that Read Report is a PDU in 1.1 version and higher. 
 * The reports must be stored in the MMS folder.
 *****************************************************************************/
void MMSif_getReport(WE_UINT8 source, MmsMsgId msgId);

/*!
 * \brief Request the status of peristent storage in the MMS folder
 *
 *
 * Type: Signal Function
 *
 * Reply signal
 *      signal name: #MMS_SIG_FOLDER_STATUS_REPLY
 *      data type    #MmsFolderStatus
 *
 * \param source The calling module identity
 *****************************************************************************/
void MMSif_getFolderStatus(WE_UINT8 source, MmsFolderType folderId);

/*
 * WE functions 
 *
 *****************************************************************************/

WE_BOOL mmsSendSignalExt(WE_UINT8 senderModId, WE_UINT8 destModule,
    WE_UINT16 signal, void *data, MmsIfConvertFunction *cvtFunPtr); 
WE_BOOL mmsSendSignalExtArray(WE_UINT8 senderModId, WE_UINT8 destModule,
    WE_UINT16 signal, void *data, MmsIfConvertFunctionArray *cvtFunPtr, 
    WE_UINT32 size); 
WE_BOOL mmsSendSignalExtNoData(WE_UINT8 senderModId, WE_UINT8 destModule,
    WE_UINT16 signal);


/*!
 * \brief Deallocate a signal buffer
 *
 * This function is used by the WE to free signal memory.
 *
 * Type: Function call
 *
 * \param source The calling module identity
 * \param signal Signal identity
 * \param ptr The signal buffer
 *****************************************************************************/
void mmsDestruct(WE_UINT8 module, WE_UINT16 signal, void *ptr); 

/*!
 * \brief Converts a serielized signal buffer into signal struct
 *
 * The function is called from WE when signals is to be converted.
 *
 * Type: Function call
 *
 *
 * \param source The calling module identity
 * \param signal Signal identity
 * \param buffer The signal buffer
 *****************************************************************************/
void *mmsConvert (WE_UINT8 modId, WE_UINT16 signal, void *buffer);


/*!
 * \brief  Converts a parameters into a signal buffer for MmsContentType 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 *****************************************************************************/
int mms_cvt_MmsContentType(we_dcvt_t *obj, MmsContentType *data);

/*!
 * \brief Copies an MmsDrmInfo field
 *
 * \param toContent Destination (Must allocate memory before this call)
 * \param fromContent Source
 * \return TRUE if copy was successfull, FALSE otherwise
 *****************************************************************************/
int mms_cvt_MmsDrmInfo(we_dcvt_t *obj, MmsDrmInfo **data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsAllParams 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 *****************************************************************************/
int mms_cvt_MmsAllParams(we_dcvt_t *obj, MmsAllParams **data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsAppReg 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 *****************************************************************************/
int mms_cvt_MmsAppReg(we_dcvt_t *obj, MmsAppReg *data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsMsgFolderSig 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 *****************************************************************************/
int mms_cvt_MmsMsgFolder(we_dcvt_t *obj, MmsMsgFolderSig *data); 

/*!
 * \brief  Converts a parameters into a signal buffer for MmsSendReadReport 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 *****************************************************************************/
int mms_cvt_MmsSendReadReport(we_dcvt_t *obj, MmsSendReadReport *data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsMsgSig 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 *****************************************************************************/
int mms_cvt_MmsMsg(we_dcvt_t *obj, MmsMsgSig *data);


/*!
 * \brief  Converts a parameters into a signal buffer for MmsFolderSig 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 *****************************************************************************/
int mms_cvt_MmsFolderId(we_dcvt_t *obj, MmsFolderSig *data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsResultSig 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 ***********************************************************************/
int mms_cvt_MmsResult(we_dcvt_t *obj, MmsResultSig *data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsMsgReplySig 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsMsgReply(we_dcvt_t *obj, MmsMsgReplySig *data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsRRReplySig 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsRRReplySig(we_dcvt_t *obj, MmsRRReplySig *data);

/*!
 * \brief  Converts a parameters into a signal buffer for MmsMsgReplyNoOfMsg 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsMsgReplyNoOfMsg(we_dcvt_t *obj, MmsMsgReplyNoOfMsg *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsMsgReplyNotificationMsg 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsMsgNotificationMsg(we_dcvt_t *obj, 
    MmsMsgReplyNotification *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsMsgDeliveryReportInd 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsMsgDeliveryReport(we_dcvt_t *obj, 
    MmsMsgDeliveryReportInd *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsMsgReadReport 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsMsgReadReport(we_dcvt_t *obj, MmsMsgReadReport *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSetReadMark 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsSetReadMark(we_dcvt_t *obj, MmsSetReadMark *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSendPdu to be sent as a bit array
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \param len  The length of the data to convert
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsSendPdu(we_dcvt_t *obj, MmsPduSig *data, WE_UINT32 dataLen);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsFolderStatus 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsFolderStatus(we_dcvt_t *obj, MmsFolderStatus *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsProgress 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE
 *****************************************************************************/
int mms_cvt_MmsProgressStatus(we_dcvt_t *obj, MmsProgressStatus *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSigGetMsg 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetMsgRequest(we_dcvt_t *obj, MmsGetMsgRequest *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsGetBodyPartRequest 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetBodyPartRequest(we_dcvt_t *obj, MmsGetBodyPartRequest *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSigGetMsgHeader 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetMsgHeaderReply(we_dcvt_t *obj, MmsGetMsgHeaderReply *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSigGetMsgSkeleton 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetSkeletonReply(we_dcvt_t *obj, MmsGetSkeletonReply *data);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSigGetMsgBodyPart 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetBodyPartReply(we_dcvt_t *obj, MmsGetBodyPartReply *data);


/*!
 *======================================================================
 * Exported MMS data structure operation functions:
 */


/*!
 *====================================
 * Operations on MmsMessageProperties.
 */

/*!
 * \brief Free the internal structure of MmsMessageProperties.
 *
 * \param properties    Structure to be freed.
 * \param module        Memory owner.
 */
void MMSif_freeMmsMessageProperties(MmsMessageProperties *properties,
                                    WE_UINT8 module);

/*!
 * \brief Copy the internal structure of MmsMessageProperties.
 *
 * \param toProperties    Structure to be updated.
 * \param properties      Structure to be copied.
 * \param module          Memory owner.
 */
WE_BOOL MMSif_copyMmsMessageProperties(MmsMessageProperties *toProperties, 
                                        MmsMessageProperties *fromProperties,
                                        WE_UINT8 module);

/*!
 * \brief Free the internal structure of MmsMessageProperties.
 *
 * \param properties    Structure to be freed.
 * \param module        Memory owner.
 */
void MMSif_freeMmsListOfProperties(MmsListOfProperties *list,
                                    WE_UINT8 module);

/*!
 * \brief Copy the internal structure of MmsMessageProperties.
 *
 * \param toProperties    Structure to be updated.
 * \param properties      Structure to be copied.
 * \param module          Memory owner.
 */
WE_BOOL MMSif_copyMmsListOfProperties(MmsListOfProperties *toList, 
                                        MmsListOfProperties *fromList,
                                        WE_UINT8 module);

/*!
 * \brief Frees a MmsContentType data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param ct In: The data to free 
 * \param freeRoot In: Shall the base structure be freed?
 *****************************************************************************/
void MMSif_freeMmsContentType( WE_UINT8 modId, MmsContentType *ct, WE_BOOL freeRoot);

/*!
 * \brief Frees a MmsContentType data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param toCT: Data structure to fill-in 
 * \param fromCT: Data structure to copy 
 *****************************************************************************/
WE_BOOL MMSif_copyMmsContentType( WE_UINT8 modId, MmsContentType *toCT, MmsContentType *fromCT);

/*!
 * \brief Frees a MmsDrmInfo data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param ct In: The data to free 
 * \param freeRoot In: Shall the base structure be freed?
 *****************************************************************************/
void MMSif_freeMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *drmInfo, WE_BOOL freeRoot);

/*!
 * \brief Frees a MmsDrmInfo data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param toCT: Data structure to fill-in 
 * \param fromCT: Data structure to copy 
 *****************************************************************************/
WE_BOOL MMSif_copyMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *toDrmInfo, MmsDrmInfo *fromDrmInfo);

/*!
 * \brief Frees a MmsGetHeader data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param mHeader In: The data to free 
*****************************************************************************/
void MMSif_freeMmsBodyInfoList( WE_UINT8 modId, MmsBodyInfoList *list);

/*!
 * \brief Frees a MmsGetHeader data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param mHeader In: The data to free 
*****************************************************************************/
MmsBodyInfoList *MMSif_copyMmsBodyInfoList(WE_UINT8 modId, MmsBodyInfoList *list);

/*!
 * \brief Frees a MmsGetHeader data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param mHeader In: The data to free 
*****************************************************************************/
void MMSif_freeMmsGetHeader( WE_UINT8 modId, MmsGetHeader *mHeader);

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsMessageProperties 
 * 
 * \param obj  The WE obejct. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE
 *****************************************************************************/
int mms_cvt_MmsMessageProperties(we_dcvt_t *obj, MmsMessageProperties *data);

/*
 * Convert list of message properties into a signal buffer
 *
 * \param obj  The WE obejct. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE
*****************************************************************************/
int mms_cvt_MmsListOfProperties(we_dcvt_t *obj, MmsListOfProperties *data);

/*!
 * \brief Sends subscription signal to all registered application.
 *
 * \returns the module identification 
 *
 * ONLY FOR MMS SERVICE INTERNAL USAGE.
 *****************************************************************************/
void mmsSendApplSubscription(WE_UINT8 senderModId,
    WE_UINT16 signal, void *data, MmsIfConvertFunction *cvtFunPtr); 
#endif /* MMS_IF_H */



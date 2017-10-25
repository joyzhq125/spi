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

/* !\file mamcr.c
 *  \brief Message Creation FSM.
 */

/* WE */
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Int.h"
#include "We_Log.h"
#include "We_Lib.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mmsl_if.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Intsig.h"
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Rc.h"
#include "Msa_Mem.h"
#include "Msa_Mr.h"
#include "Msa_Mcr.h"
#include "Msa_Utils.h"
#include "Msa_Del.h"
#include "Msa_Conf.h"
#include "Msa_Utils.h"
#include "Msa_Async.h"
#include "Msa_Uipm.h"
#include "Msa_Moh.h"
#include "Msa_Addr.h"
#include "Msa_Slh.h"

/******************************************************************************
 * Macros
 *****************************************************************************/
#define SET_CHARSET_PARAM(allParams)        addIntParam(allParams, \
                                                MMS_CHARSET, MMS_UTF8)
#define SET_FILENAME_PARAM(allParams, str)  addStrParam(allParams, \
                                                MMS_FILENAME, str)
#define SET_START_REL_PARAM(allParams, str) addStrParam(allParams, \
                                                MMS_START_REL, str)
#define SET_TYPE_REL_PARAM(allParams, str)  addStrParam(allParams, \
                                                MMS_TYPE_REL,str)
#define SET_CONTENT_ID(entryHeads, str)     addStrEntry(entryHeads, \
                                                MMS_WELL_KNOWN_CONTENT_ID, str)
#define SET_CONTENT_LOCATION(entryHeads, str) addStrEntry(entryHeads, \
                                        MMS_WELL_KNOWN_CONTENT_LOCATION, str)
#define ORIGINAL_MSG_EXISTS(inst) (NULL != (inst)->origMsg)

#define IS_FORWARD_OP(inst) ((NULL != inst) && (NULL != inst->origMsg) && \
    (inst->origMsg->forwardMode))

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! The filename used for the SMIL part in multi-part related messages */
#define MSA_SMIL_FILE_NAME          "s.smil"

/*!\enum MsaCreateMsgStates
 * Internal states when creating a message */
typedef enum
{
    CREATE_MSG_ID,        /* !< Creating a new message WID */
    CREATE_HEADER,        /* !< Creating the message hader */
    READ_HEADER,          /* !< Reading the message header to persistent 
                                storage from the original message during fwd */
    READ_SKELETON,        /* !< Reading the message skeleton */
    WRITE_HEADER,         /* !< Writing the message header to persistent 
                                storage */
    WRITE_BODY_PART,      /* !< Writing message parts */
    WRITE_SMIL_PART,      /* !< Write the plain text part of a message */
    COPY_BODY,            /* !< Copy body during forward */
    FINALISE_MSG,         /* !< Finalising the message */
    MESSAGE_DONE_RESPONSE /* !< Message is finalised */
}MsaCreateMsgStates;

/******************************************************************************
 * Data-structures
 *****************************************************************************/
/*!\struct McrOriginalMessage 
 * If allocated, contains information about the original message
 */
typedef struct 
{
    MmsMsgId                msgId;          /*!< The message WID */
    MmsFileType             fileType;       /*!< Message type */
    WE_BOOL                del;            /*!< Delete when done ? */
    WE_BOOL                forwardMode;    /*!< This is a forward op  */
    char                    *srcFile;       /*!< message filename */
    char                    *dstFile;       /*!< new message filename */
    MmsGetHeader            *hdr;           /*!< original header */
    MmsBodyInfoList         *skeleton;      /*!< The message skeleton */
    WE_UINT32              leftToRead;     /*!< Data left to read */
    int                     readPos;        /*!< The size read so far */
    int                     sizeToWrite;    /*!< The size to write */
    WE_UINT32              copyBufferSize; /*!< size of copy-buffer */
    unsigned char           *copyBuffer;    /*!< Buffer for async copying */
    MsaAsyncOperationHandle asioReadHandle; /*!< Read handle for AsyncIO */
    MsaAsyncOperationHandle asioWriteHandle;/*!< Write handle for AsyncIO */
} McrOriginalMessage;


/*!\struct McrInstance 
 * Data structure used when creating a message */
typedef struct
{
    MmsMsgId                msgId;          /*!< The WID of the message */
    MmsFileType             msgType;        /*!< The message type of the new message */
    MmsBodyPartTarget       targetType;     /*!< The MMS message target type */
    MmsFolderType           saveInFolder;   /*!< Which folder to save the message in */
    MsaCreateMsgStates      state;          /*!< The current state */
    MsaStateMachine         fsm;            /*!< FSM to send result to */
    int                     sig;            /*!< Signal to send */
    MmsMsgHandle            msgHandle;      /*!< The handle of the message */
    MmsTransactionHandle    tHandle;        /*!< Transaction handle */
    MmsSetHeader            *msgHdr;        /*!< The header of the message */
    MsaMessage              *msg;           /*!< The content of the message */
    /* Body part information */
    MsaMoDataItem           *currentMo;     /*!< The media object that is 
                                                 currently being stored */
    MmsBodyParams           *bodyParams;    /*!< The params for the current 
                                                 media object */
    unsigned char           *tmpBuffer;     /*!< Buffer used for SMIL 
                                                 generation */
    unsigned int            tmpSize;        /*!< Size of SMIL generation 
                                                 buffer */
    /* Results from MMS lib operations */
    MmsResult               mmsResult;      /*!< Result of last MMSLib 
                                                 operation */
    McrOriginalMessage      *origMsg;       /*!< Data for original message */

}McrInstance;

/******************************************************************************
 * Variables
 *****************************************************************************/
static McrInstance *mcrInstance;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void mcrSignalHandler(MsaSignal *sig);
static void cleanupMcrInstance(MsaMcrResult result);
static void releaseSmilBuffer(McrInstance *instance);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*
 * \brief Inserts an integer param first in the list of params
 *
 * \param allParams The current list.
 * \param param     The param to insert.
 * \param value     The integer value for the param to insert.
 *****************************************************************************/
static void addIntParam(struct MmsAllParamsStruct **allParams, 
    MmsParam param, WE_UINT32 value)
{
    MmsAllParams *newParam = NULL;

    MSA_CALLOC(newParam, sizeof(MmsAllParams));
    /* Set param value */
    newParam->param = param;
    newParam->type = MMS_PARAM_INTEGER;
    newParam->value.integer = value;

    /* Insert param. Insert first in the list */
    newParam->next = *allParams;
    *allParams = (struct MmsAllParamsStruct *)newParam;
}

/*!
 * \brief Inserts a string param first in the list of params
 *
 * \param allParams The current list.
 * \param param The param to insert.
 * \param strValue The string value for the param to insert. The strValue can
 *                 be released after the call to this function.
 *****************************************************************************/
static void addStrParam(struct MmsAllParamsStruct **allParams, 
    MmsParam param, const char *strValue)
{
    MmsAllParams *newParam = NULL;

    if((*allParams) != NULL)
    {
        if((*allParams)->type == MMS_PARAM_STRING)
        {
            if(!strcmp((char*)(*allParams)->value.string, strValue))
            {
                return;
            }
        }
    }

    MSA_CALLOC(newParam, sizeof(MmsAllParams));
    /* Set param value */
    newParam->param = param;
    newParam->type = MMS_PARAM_STRING;
    newParam->value.string = 
        (unsigned char *)we_cmmn_strdup(WE_MODID_MSA, strValue);
    /* Insert param. Insert first in the list */
    newParam->next = *allParams;
    *allParams = (struct MmsAllParamsStruct *)newParam;
}

/*!
 * \brief Inserts a string entry first in the list of entries
 *
 * \param entryHeads The current list.
 * \param type The type of entry to insert.
 * \param strValue The string value for the entry to insert. The strValue can
 *                 be released after the call to this function.
 *****************************************************************************/
static void addStrEntry(MmsEntryHeader **entryHeads, 
    MmsEntryHeaderValueType type, const char *strValue)
{
    /* Content WID */
    MmsEntryHeader *newEntry;
    MSA_CALLOC(newEntry, sizeof(MmsEntryHeader));
    /* Set value */
    newEntry->headerType = type;
    newEntry->value.wellKnownFieldName = 
        (unsigned char *)we_cmmn_strdup(WE_MODID_MSA, strValue);
    /* Insert param. Insert first in the list */
    newEntry->next = *entryHeads;
    *entryHeads = newEntry;
}

/*! 
 * \brief De-allocates a #MmsBodyParams data structure.
 *
 * \param bodyParams The body param to release. The pointer is set to NULL.
 *****************************************************************************/
static void freeBodyParams(MmsBodyParams **bodyParams)
{
    MmsEntryHeader *entry;
    MmsEntryHeader *tmpEntry;

    if (NULL != *bodyParams)
    {
        /* Free params */
        MMSif_freeMmsContentType(WE_MODID_MSA, &(*bodyParams)->contentType, 
            FALSE);
        /* Free entries */
        entry = (*bodyParams)->entryHeader;
        while (NULL != entry)
        {
            /* Free content WID or content location string */
            if ( (MMS_WELL_KNOWN_CONTENT_ID == entry->headerType) ||
                 (MMS_WELL_KNOWN_CONTENT_LOCATION == entry->headerType))
            {
                MSA_FREE(entry->value.wellKnownFieldName);
            }
            tmpEntry = entry;
            entry = entry->next;
            MSA_FREE(tmpEntry);
        }
        MSA_FREE(*bodyParams);
        *bodyParams = NULL;
    }
}

/*!
 * \brief Initiates the message creation FSM by registering its signal handler
 *****************************************************************************/
void msaMcrInit(void)
{
    /* Register queue */
    mcrInstance   = NULL;
    msaSignalRegisterDst(MSA_MCR_FSM, mcrSignalHandler);
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, "MCR initialised\n"));
}

/*!
 * \brief Terminates the message editor.
 *****************************************************************************/
void msaMcrTerminate(void)
{
    /* Deregister queue */
    cleanupMcrInstance(MSA_MCR_CLEANUP_SILENT);
    mcrInstance   = NULL;
    msaSignalDeregister(MSA_MCR_FSM);
}

/*!
 * \brief Converts from MSA to MMS visibility
 * 
 * \param vis Visibility in MSA format 
 * \return Visibility in MMS format
 *****************************************************************************/
static MmsSenderVisibility visibilityMsaToMms(MsaVisibilityType vis)
{
    switch (vis)
    {
    case MSA_SENDER_HIDE:
        return MMS_SENDER_HIDE;
    case MSA_SENDER_SHOW:
        return MMS_SENDER_SHOW;
    default:
        return MMS_SENDER_VISIBILITY_NOT_SET;
    }
} /* visibilityMsaToMms */

/*!
 * \brief Converts between MSA and MMS message priorities 
 * 
 * \param prio  Msa priority
 * \return MMS priority
 *****************************************************************************/
static MmsPriority priorityMsaToMms(MsaPriorityType prio)
{
    switch (prio) 
    {
    case MSA_PRIORITY_LOW:
        return MMS_PRIORITY_LOW;
    case MSA_PRIORITY_NORMAL:
        return MMS_PRIORITY_NORMAL;
    case MSA_PRIORITY_HIGH:
        return MMS_PRIORITY_HIGH;
    default:
        return MMS_PRIORITY_NOT_SET;
    }
} /* priorityMsaToMms */

/*!
 * \brief Sets reply charging part of an MmsSetHeader
 * 
 * \param p The current message properties.
 * \param hdr The header to set according to the configuration.
 *****************************************************************************/
static void replyChargingMsaToMms(const MsaPropItem *p, MmsSetHeader *hdr)
{
    if (MSA_REPLY_CHARGING_ON == p->replyCharging ||
        MSA_REPLY_CHARGING_TEXT_ONLY == p->replyCharging)
    {
        hdr->replyCharging = ((MSA_REPLY_CHARGING_ON == p->replyCharging)
            ? MMS_REPLY_CHARGING_REQUESTED
            : MMS_REPLY_CHARGING_REQUESTED_TEXT_ONLY);
        /* Reply charging deadline */
        hdr->replyChargingDeadline.type = MMS_TIME_RELATIVE;

        switch(p->replyChargingDeadline)
        {
        case MSA_REPLY_CHARGING_DEADLINE_1_DAY:
            hdr->replyChargingDeadline.value = MSA_1_DAY;
            break;
        case MSA_REPLY_CHARGING_DEADLINE_1_WEEK:
            hdr->replyChargingDeadline.value = MSA_1_WEEK;
            break;
        case MSA_REPLY_CHARGING_DEADLINE_MAX:
            hdr->replyChargingDeadline.value = MSA_1_YEAR;
            break;
        default:
            hdr->replyChargingDeadline.value = MSA_1_DAY;
            break;
        }
        /* Reply charging size */
        switch(p->replyChargingSize)
        {
        case MSA_REPLY_CHARGING_SIZE_1_KB:
            hdr->replyChargingSize = MSA_1_KB;
            break;
        case MSA_REPLY_CHARGING_SIZE_10_KB:
            hdr->replyChargingSize = MSA_10_KB;
            break;
            case MSA_REPLY_CHARGING_SIZE_MAX:
            hdr->replyChargingSize = 0;
            break;
        default:
            hdr->replyChargingSize = 0;
            break;
        }
    }
    else
    {
        hdr->replyCharging = MMS_REPLY_CHARGING_NOT_SET;
    }    
    
    if ((NULL != p->replyChargingId) && (strlen(p->replyChargingId) > 0))
    {
        hdr->replyChargingId = MSA_ALLOC(strlen(p->replyChargingId) + 1);
        strcpy(hdr->replyChargingId, p->replyChargingId);
    }
    else
    {
        hdr->replyChargingId = NULL;
    }
} /* replyChargingMsaToMms */

/*!
 * \brief Sets expiry time
 * 
 * \param now Time right now
 * \param msaTime The expiry setting to use
 * \param time Expiry-time will be returned through this variable
 *****************************************************************************/
static void setExpiryTime(MmsTimeSec now, MsaExpiryTimeType msaTime, MmsTime *time)
{
    MmsTimeSec baseSeconds = 0; 

    /*  Base seconds is the base to add the chosen number of seconds to. If 
     *  relative time is used no base is added otherwise current time is used.
     */

    #if MSA_CONF_EXPIRY_TIME_RELATIVE
    {
        (void)now;
        baseSeconds = 0;
        time->type = MMS_TIME_RELATIVE;
    }
    #else /* absolute */
    {
        baseSeconds = now;
        time->type = MMS_TIME_ABSOLUTE;
    } /* if */
    #endif

    switch (msaTime) 
    {
    case MSA_EXPIRY_1_HOUR:
        time->value = baseSeconds + MSA_EXPIRY_1_HOUR_IN_SEC;
    	break;
    case MSA_EXPIRY_12_HOURS:
        time->value = baseSeconds + MSA_EXPIRY_12_HOURS_IN_SEC;
        break;
    case MSA_EXPIRY_1_DAY:
        time->value = baseSeconds + MSA_EXPIRY_1_DAY_IN_SEC;
        break;
    case MSA_EXPIRY_1_WEEK:
        time->value = baseSeconds + MSA_EXPIRY_1_WEEK_IN_SEC;
        break;
    case MSA_EXPIRY_MAX:
        time->value = baseSeconds + MSA_EXPIRY_MAX_IN_SEC;
        break;
    default:
        time->value = baseSeconds + MSA_EXPIRY_MAX_IN_SEC;
        break;
    }
} /* setExpiryTime */

/*! \brief Gets the delivery time.
 *	
 *****************************************************************************/
static void setDeliveryTime(MmsTimeSec currentTime, MmsTime *toSet, 
    MsaDeliveryTimeType dTime)
{
    MmsTimeSec t;

    switch(dTime)
    {
    case MSA_DELIVERY_TIME_IMMEDIATE:
        /* Do not set he value if no time is set */
        return;
    case MSA_DELIVERY_TIME_1_HOUR:
        t = MSA_1_HOUR;
        break;
    case MSA_DELIVERY_TIME_12_HOURS:
        t = MSA_12_HOURS;
        break;
    case MSA_DELIVERY_TIME_24_HOURS:
        t = MSA_1_DAY;
        break;
    default:
        /* Do not set he value if no time is set */
        return;
    }

    toSet->type = MMS_TIME_ABSOLUTE;
    toSet->value = currentTime + t;
}

/*!
 * \brief   Creates an MMS header. 
 * \param hdr           The header to put data in
 * \param p             A set of message properties
 * \param oHdr          The header of the original message
 * \param numBodyParts  The number of body-parts in the new message 
 * \param isSmil        TRUE if the message is of type multi-part-related, 
 *                      otherwise FALSE:
 * \return  TRUE on success, otherwise FALSE
 *****************************************************************************/
static void msaCreateMessageHeader(McrInstance  *inst, 
    const MmsGetHeader *oHdr, WE_UINT32 numBodyParts, WE_BOOL isSmil)
{
    MsaConfig *cfg;
    int len;
    
    if (NULL == inst)
    {
        return;
    }

    /* Create Header */
    MSA_CALLOC(inst->msgHdr, sizeof(MmsSetHeader));  
    MSA_CALLOC(inst->msgHdr->contentType, sizeof(MmsContentType));

    if (IS_FORWARD_OP(mcrInstance))
    {
        /*
         *	If we are in forward we need to copy the content type from the
         *  original message to the one we are creating. Assume this simple
         *  process works.
         */
        MMSif_copyMmsContentType(WE_MODID_MSA, inst->msgHdr->contentType, 
            oHdr->contentType);
    }
    else
    {
        /* SMIL */
        if (isSmil)
        {
            inst->msgHdr->contentType->knownValue = MMS_VND_WAP_MULTIPART_RELATED;
            /* Start rel */
            SET_START_REL_PARAM(&inst->msgHdr->contentType->params, 
                MSA_SMIL_FILE_NAME);
            /* Start type */
            SET_TYPE_REL_PARAM(&inst->msgHdr->contentType->params, 
                MMS_MEDIA_TYPE_STRING_SMIL);
            inst->targetType = MMS_MULTIPART_MESSAGE;
        }
        /* Multipart/Mixed */
        else if (numBodyParts > 0) 
        {
            inst->msgHdr->contentType->knownValue = MMS_VND_WAP_MULTIPART_MIXED;
            inst->targetType = MMS_MULTIPART_MESSAGE;
        }
        /* Plain text */
        else
        {
            inst->msgHdr->contentType->knownValue = MMS_TYPE_TEXT_PLAIN;
            SET_CHARSET_PARAM(&inst->msgHdr->contentType->params);
            inst->targetType = MMS_PLAIN_MESSAGE;
        }
    }
    
    /* From: */
    cfg = msaGetConfig();
    if ((NULL != cfg->fromName) && (strlen(cfg->fromName) > 0))
    {   /* John Doe */
        inst->msgHdr->from.name.text = MSA_ALLOC(strlen(cfg->fromName) + 1);
        strcpy(inst->msgHdr->from.name.text, cfg->fromName);
        inst->msgHdr->from.name.charset = MMS_UTF8;
    }
    inst->msgHdr->from.addrType = cfg->addressType; /* 070-12341234 vs foo@bar.net */
    len = (NULL != cfg->fromAddress) ? (int)strlen(cfg->fromAddress) : 0;
    if (len > 0)
    {
        inst->msgHdr->from.address = MSA_ALLOC((unsigned)len + 1);
        strcpy(inst->msgHdr->from.address, cfg->fromAddress);
        inst->msgHdr->fromType = MMS_FROM_ADDRESS_PRESENT;
    }
    else
    {
        inst->msgHdr->fromType = MMS_FROM_INSERT_ADDRESS;
    }

    /* To/Cc/Bcc, assume this simple copy process works. */
    (void)Msa_AddrListToMms(inst->msg->msgProperties->to, &inst->msgHdr->to);
    (void)Msa_AddrListToMms(inst->msg->msgProperties->cc, &inst->msgHdr->cc);
    (void)Msa_AddrListToMms(inst->msg->msgProperties->bcc, &inst->msgHdr->bcc);

    /* Subject: */
    if ((NULL != inst->msg->msgProperties->subject) && 
        (strlen(inst->msg->msgProperties->subject) > 0))
    {
        inst->msgHdr->subject.text = MSA_ALLOC(
            strlen(inst->msg->msgProperties->subject) + 1);
        strcpy(inst->msgHdr->subject.text, inst->msg->msgProperties->subject);
        inst->msgHdr->subject.charset = MMS_UTF8;
    }

    /* Date, must be done ahead of expiry and delivery time */
    inst->msgHdr->date = WE_TIME_GET_CURRENT();

    /* Read/delivery reports */
    inst->msgHdr->deliveryReport = inst->msg->msgProperties->deliveryReport 
        ? MMS_DELIVERY_REPORT_YES : MMS_DELIVERY_REPORT_NO;
    inst->msgHdr->readReply = inst->msg->msgProperties->readReport
        ? MMS_READ_REPLY_YES : MMS_READ_REPLY_NO;
    
    /* Message class */
    inst->msgHdr->msgClass.classIdentifier = MMS_MESSAGE_CLASS_NOT_SET;
    
    /* Priority */
    inst->msgHdr->priority = priorityMsaToMms(inst->msg->msgProperties->priority);
    
    /* Sender visibility */
    inst->msgHdr->visibility = visibilityMsaToMms(inst->msg->msgProperties->senderVisibility);
    
    /* Reply Charging */
    replyChargingMsaToMms(inst->msg->msgProperties, inst->msgHdr);

    /* Delivery time (absolute) */
    setDeliveryTime(inst->msgHdr->date, &(inst->msgHdr->deliveryTime), 
        msaGetConfig()->deliveryTime);
    
    /* Expiry time */
    setExpiryTime(inst->msgHdr->date, inst->msg->msgProperties->expiryTime, 
        &inst->msgHdr->expiryTime);
}

/*!
 * \brief              Creates a MmsBodyParams
 *
 * \param targetType The type of message that are created, see 
 *                   #MmsBodyPartTarget
 * \param contentType The content type of the body-part.
 * \param name        The name of the body-part. 
 * \return            The created MmsBodyParams
 *****************************************************************************/
static MmsBodyParams *msaCreateBodyParams(MmsBodyPartTarget targetType, 
    MmsContentType *contentType, const unsigned char *name)
{
    MmsBodyParams *bp;

    MSA_CALLOC(bp, sizeof(MmsBodyParams));
    /* Copy the current type */
    MMSif_copyMmsContentType(WE_MODID_MSA, &bp->contentType, contentType);

    /* Add the name of the body-part */
    if (NULL != name)
    {
        /* Content WID */
        SET_CONTENT_ID(&bp->entryHeader, (char *)name);
        /* Content location */
        SET_CONTENT_LOCATION(&bp->entryHeader, (char *)name);
    }

    switch (targetType)
    {
    case MMS_MULTIPART_MESSAGE:       
        bp->targetType = MMS_MULTIPART_MESSAGE;
        /* Set SMIL part to charset UTF-8 */
        if (0 == we_cmmn_strcmp_nc((const char *)contentType->strValue, 
            MMS_MEDIA_TYPE_STRING_SMIL))
        {
            SET_CHARSET_PARAM(&bp->contentType.params);
        }
        /* Always set the file name */
        SET_FILENAME_PARAM(&bp->contentType.params, (char *)name);
        break;
    case MMS_PLAIN_MESSAGE:
        bp->targetType = MMS_PLAIN_MESSAGE;
        break;
    case MMS_MULTIPART_ENTRY:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) erroneous message type!\n", __FILE__, __LINE__));
        break;
    }
    /* Set charset to UTF-8 if the body part is of type plain text */
    if (MSA_MEDIA_GROUP_TEXT == msaMimeToObjGroup((char *)contentType->strValue))
    {
        SET_CHARSET_PARAM(&bp->contentType.params);
    } /* if */

    return bp;
}

/*!
 *\brief Initializes the copy of the body
 *
 * \param dstFilePath Full path to destination file.
 * \param sourceFilePath Full path to source file.
 * \param startPos Start position for data to read from the source file.
 * \param dataSize Size of data in source file.
 * \param instance The current instance 
 *****************************************************************************/
static void msaCopyBody(const char *dstFilePath, const char *sourceFilePath, 
    WE_UINT32 startPos, WE_UINT32 dataSize, McrInstance *instance)
{
    WE_UINT32 bytesToRead = 0;
    
    MSA_FREE(instance->origMsg->copyBuffer);
    MSA_CALLOC(instance->origMsg->copyBuffer, MSA_ASIO_COPY_BUFFER_SIZE);
    instance->origMsg->copyBufferSize = MSA_ASIO_COPY_BUFFER_SIZE;
    
    instance->origMsg->readPos       = (int)startPos;
    instance->origMsg->leftToRead    = dataSize;

    /* create read async operation */
    instance->origMsg->asioReadHandle = 
        Msa_AsyncOperationStart(sourceFilePath, MSA_MCR_FSM, 0L, 
        MSA_SIG_MCR_ASIO_READ_RSP, MSA_ASYNC_MODE_READ);
    
     /* create write async operation */
    instance->origMsg->asioWriteHandle = 
        Msa_AsyncOperationStart(dstFilePath, MSA_MCR_FSM, 0L, 
        MSA_SIG_MCR_ASIO_WRITE_RSP, MSA_ASYNC_MODE_APPEND);

    instance->state = COPY_BODY;

    /* read first part of the file */
    if (instance->origMsg->leftToRead < instance->origMsg->copyBufferSize)
    {
        bytesToRead = (unsigned)instance->origMsg->leftToRead;
    }
    else
    {
        bytesToRead = (unsigned)instance->origMsg->copyBufferSize;
    } /* if */
    Msa_AsyncRead( instance->origMsg->asioReadHandle, 
        (WE_INT32)instance->origMsg->readPos, instance->origMsg->copyBuffer, 
        bytesToRead);

} /* msaCopyBody */

/*!
 *\brief Checks the result of the read operation.
 *
 * \param instance Instance information, e.g. information about the 
 *         current transaction.
 * \param bytesRead Number of bytes read in last read operation.
 * \param result Result of the read operation operation
 *
 * \return TRUE if successful, else FALSE
 *****************************************************************************/
static WE_BOOL handleReadOperation(const McrInstance *instance, int bytesRead, 
    MsaAsioResult result)
{
    /*lint -e{788} */
    switch (result)
    {
        case MSA_ASIO_RESULT_OK:
        case MSA_ASIO_RESULT_EOF:
            /* write data to file */
            instance->origMsg->readPos += (WE_INT32)bytesRead;
            instance->origMsg->leftToRead -= (WE_UINT32)bytesRead;
            instance->origMsg->sizeToWrite = (WE_INT32)bytesRead;
            return TRUE;
        case MSA_ASIO_RESULT_FILE_NOT_FOUND:
        case MSA_ASIO_RESULT_ERROR:
        case MSA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE:
            return FALSE;
        case MSA_ASIO_RESULT_DELAYED:
        default:
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "MSA_SIG_MCR_ASIO_READ_RSP invalid result code (%d)\n", 
                result));
            return FALSE;
    } /* switch */
} /* handleReadOperation */

/*!
 * \brief Checks the result of the write operation.
 * \param result Result of the write operation
 * \return TRUE if successful, else FALSE
 *****************************************************************************/
static WE_BOOL handleWriteOperation(MsaAsioResult result)
{
    /*lint -e{788} */
    switch(result)
    {
        case MSA_ASIO_RESULT_OK:
        case MSA_ASIO_RESULT_EOF:
            return TRUE;
        case MSA_ASIO_RESULT_FILE_NOT_FOUND:
        case MSA_ASIO_RESULT_ERROR:
        case MSA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE:
            return FALSE;
        case MSA_ASIO_RESULT_DELAYED:
        default:
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "MSA_SIG_MCR_ASIO_WRITE_RSP invalid result code (%d)\n", 
                result));
            return FALSE;
    } /* switch */
} /* handleWriteOperation */

/*! \brief Release the temporary buffer used for SMIL messages.
 *	
 * \param instance The current instance.
 *****************************************************************************/
static void releaseSmilBuffer(McrInstance *instance)
{
    if ((NULL != instance) && (NULL != instance->tmpBuffer))
    {
        MSA_FREE(instance->tmpBuffer);
        instance->tmpBuffer = NULL;
        instance->tmpSize = 0;
    }
}

/*!
 * \brief  Creates a message by filling in an MMS Header and all necessary 
 *         data, based on data from the GUI and the configuration in used. 
 *         The buffers are free'd here. 
 * \param  instance The current message cration instance.
 * \return See #MsaMcrResult.
 *****************************************************************************/
static int msaCreateMessageFSM(McrInstance *instance)
{
    WE_UINT32 numBodyParts;
    int fHandle;
    int index;
    MmsContentType contentType;
    WE_UINT32 bodySize;
    
    if (NULL == instance)
    {
        return MSA_MCR_FAILED;
    }

    switch (instance->state)
    {    
    case CREATE_MSG_ID:
        /*
         *	Start by creating a message in MMS
         */
        MMSif_createMessage(WE_MODID_MSA, instance->msgType);
        instance->state = CREATE_HEADER;
        return MSA_MCR_WAIT_RSP; /* wait for msgId */

    case CREATE_HEADER:
        /*
         *	Message created, generate header
         */
        msaPmSetProgress(10, NULL);
        instance->mmsResult = MMSlib_messageOpen(WE_MODID_MSA, 
            instance->msgId, instance->msgType, &instance->msgHandle);
        if (MMS_RESULT_OK != instance->mmsResult)
        {
            return MSA_MCR_FAILED;
        }
        
        instance->mmsResult = MMSlib_transactionOpen(WE_MODID_MSA, 
            &instance->tHandle);
        if (MMS_RESULT_OK != instance->mmsResult)
        {
            return MSA_MCR_FAILED;
        }
        /* on MMS_RESULT_OK, just keep on going */

        if (IS_FORWARD_OP(instance))
        {
            /* Read the header in the next state (to support async IO) */
            instance->state = READ_HEADER;
            /* Continue execution */
            MMSif_getMsgHeader(WE_MODID_MSA, instance->origMsg->msgId,
                MSA_GET_HEADER_RSP_MCR);

            return MSA_MCR_WAIT_RSP;
        }
        else
        {
            instance->state = WRITE_HEADER;
            /* Do not execute the entire work at once */
            (void)MSA_SIGNAL_SENDTO(MSA_MCR_FSM, MSA_SIG_MCR_FILE_NOTIFY);
        }
        return MSA_MCR_WAIT_RSP;

    case READ_HEADER:
        /* The header of the originating message is needed in order to set
         * the correct content type and number of body-parts for forwarding
         */
        msaPmSetProgress(20, NULL);
        
        instance->state = READ_SKELETON;
        /* Get the message skeleton since the "number of subparts", size and
         * the start position is needed in the forward operation
         */
        MMSif_getMsgSkeleton(WE_MODID_MSA, instance->origMsg->msgId, 
            MSA_GET_SKELETON_RSP_MCR);
        return MSA_MCR_WAIT_RSP;

    case READ_SKELETON:
        instance->state = WRITE_HEADER;
        /* Write the header in the next state */
        (void)MSA_SIGNAL_SENDTO(MSA_MCR_FSM, MSA_SIG_MCR_CONTINUE);
        break;

    case WRITE_HEADER:
        msaPmSetProgress(30, NULL);
        /* Create header, use 'name' as SMIL-indication */
        if (IS_FORWARD_OP(instance))
        {
            numBodyParts = instance->origMsg->skeleton->numOfEntries;
        }
        else
        {
            /*
             *	 Calculate number of body-parts
             */
            if ((NULL != instance->msg->smilInfo) && 
                (msaIsSmilValid(instance->msg->smilInfo))) 
            {
                /* SMIL, i.e. multipart/related, make room for the SMIL */
                numBodyParts = 
                    msaGetMoCount(instance->msg->smilInfo->mediaObjects) + 1;
            }
            else if ((NULL != instance->msg->smilInfo) && 
                     (NULL != instance->msg->smilInfo->mediaObjects))
            {
                /* Multipart/mixed or text - just the media objects or text */
                numBodyParts = 
                    msaGetMoCount(instance->msg->smilInfo->mediaObjects);

                if ((1 == numBodyParts) && (MSA_MEDIA_GROUP_TEXT == 
                    msaMimeToObjGroup(
                    (char *)instance->msg->smilInfo->mediaObjects->type->strValue)))
                {
                    /*
                     *	 When there is only one body-part and it's text; send 
                     *   it as a plain/text message w/o body-_PART_s
                     */
                    numBodyParts = 0;
                }
            }
            else
            {
                /* There are no body-parts! Save anyway. */
                numBodyParts = 0;
            }

        }
        if ((NULL == instance->msgHdr) && 
            (NULL != instance->msg->msgProperties))
        {
            msaCreateMessageHeader(instance, 
                IS_FORWARD_OP(instance) ? instance->origMsg->hdr : NULL, 
                numBodyParts, msaIsSmilValid(instance->msg->smilInfo));
        }
        else
        {
            return MSA_MCR_FAILED;
        }

        /* Set the generated header */
        instance->mmsResult = MMSlib_setMessageHeader(WE_MODID_MSA, 
            numBodyParts, instance->msgHdr, msaGetConfig()->proxyVersion,
            &instance->msgHandle, &instance->tHandle);
        if (MMS_RESULT_DELAYED == instance->mmsResult)
        {
            fHandle = MMSlib_getFileHandle(WE_MODID_MSA, &instance->tHandle);
            Msa_AsyncRegister(fHandle, MSA_MCR_FSM, MSA_SIG_MCR_FILE_NOTIFY, 0);
            return MSA_MCR_DELAYED; /* Async! .. we'll arrive here again, soon*/
        }
        else if (MMS_RESULT_OK != instance->mmsResult)
        {
            return MSA_MCR_FAILED;
        }
        /* on MMS_RESULT_OK, keep going */

        MMSlib_freeMmsSetHeader(WE_MODID_MSA, instance->msgHdr);
        MSA_FREE(instance->msgHdr);
        instance->msgHdr = NULL;

        /* prep for next state */
        if ((MMS_RESULT_OK != (instance->mmsResult = MMSlib_transactionClose(
            WE_MODID_MSA, &instance->tHandle))))
        {
            return MSA_MCR_FAILED;
        }
        if (!IS_FORWARD_OP(instance))
        {
            if (MMS_RESULT_OK != (instance->mmsResult = MMSlib_transactionOpen(
                WE_MODID_MSA, &instance->tHandle)))
            {
                return MSA_MCR_FAILED;
            }
        }
        /*  
         *  Write message parts (bodyparts), 
         *  start copying them (forwarding), 
         *  or finalize message
         */
        if ((NULL != instance->msg->smilInfo) && 
            (NULL != instance->msg->smilInfo->mediaObjects))
        {
            instance->state = WRITE_BODY_PART;
            instance->currentMo = instance->msg->smilInfo->mediaObjects;
        }
        else if (IS_FORWARD_OP(instance))
        {
            /*
             *	On forward we need to open the old message and copy
             *  the entire body.
             */
            instance->origMsg->dstFile = MMSlib_getMessageFullPath(
                WE_MODID_MSA, instance->msgId, instance->msgType);
            
            instance->origMsg->srcFile = MMSlib_getMessageFullPath(
                WE_MODID_MSA, instance->origMsg->msgId, 
                instance->origMsg->fileType);
            if (NULL == instance->origMsg->dstFile || 
                NULL == instance->origMsg->srcFile) 
            {
                return MSA_MCR_FAILED;
            }
            /*
             *	We must close the MMS here since we will do the body copy
             *  manually with AsyncIO.
             */
            instance->mmsResult = MMSlib_messageClose(WE_MODID_MSA, 
                &instance->msgHandle);
            if (MMS_RESULT_OK != instance->mmsResult)
            {
                return MSA_MCR_FAILED;
            }
            if (numBodyParts > 0)
            {
                /*  SMIL or Multipart/Mixed message
                 *	Copy the body from the original file so it will be exactly
                 *  the same. We must add +1 to the start position of the body
                 *  to go pass the number of body parts because this is added
                 *  by the call to MMSlib_setMessageHeader()
                 */
                msaCopyBody(instance->origMsg->dstFile, 
                    instance->origMsg->srcFile, 
                    instance->origMsg->skeleton->startPos + 1, 
                    instance->origMsg->skeleton->size - 1, instance);
            }
            else
            {
                /*  Text/Plain Message
                 *	Copy the body from the original file so it will be exactly
                 *  the same.
                 */
                msaCopyBody(instance->origMsg->dstFile, 
                    instance->origMsg->srcFile, 
                    instance->origMsg->skeleton->startPos, 
                    instance->origMsg->skeleton->size, instance);
            }
            instance->state = COPY_BODY;
        }
        else
        {
            instance->state = FINALISE_MSG;
        }
        /* Do not execute the entire work at once */
        (void)MSA_SIGNAL_SENDTO(MSA_MCR_FSM, MSA_SIG_MCR_FILE_NOTIFY);
        return MSA_MCR_WAIT_RSP;

    case WRITE_BODY_PART:
        index = msaGetMoIndex(instance->msg->smilInfo->mediaObjects, 
            instance->currentMo);
        if (-1 == index)
        {
            return MSA_MCR_FAILED;
        } /* if */

        bodySize = instance->currentMo->size;
#if MSA_CFG_USE_BODY_TEXT_NULL == 0
        /* The null termination of text should not be included in the body part */
        /* Check if this is a text body part adjust size */
        if ((msaMimeToObjGroup((char*)instance->currentMo->type->strValue) ==
            MSA_MEDIA_GROUP_TEXT))
        {
            /* Must remove NULL termination. Just reset size...do not change */
            /* the size of the actual MO. */
            bodySize = (WE_UINT32)strlen((char*)instance->currentMo->data);
        }
#endif
        /* Set progress to somewhere between 50 and 85 percent */
        msaPmSetProgress(50 + 35 * (unsigned int)index / msaGetMoCount(
            instance->msg->smilInfo->mediaObjects), NULL);
        /* Create body params if they are not previously created */
        if (NULL == instance->bodyParams)
        {

            if (NULL == (instance->bodyParams = msaCreateBodyParams(
                instance->targetType, instance->currentMo->type, 
                (unsigned char *)instance->currentMo->name)))
            {
                return MSA_MCR_FAILED;
            }
        }        
        /* Write message */
        instance->mmsResult = MMSlib_setMessageBody(WE_MODID_MSA, 
            instance->bodyParams, instance->currentMo->data, 
            bodySize, &instance->msgHandle, &instance->tHandle);
        if (MMS_RESULT_DELAYED == instance->mmsResult)
        {
            fHandle = MMSlib_getFileHandle(WE_MODID_MSA, &instance->tHandle);
            Msa_AsyncRegister(fHandle, MSA_MCR_FSM, MSA_SIG_MCR_FILE_NOTIFY, 0);
            /* Async! .. we'll arrive here again, soon. */
            return MSA_MCR_DELAYED; 
        }
        else if (MMS_RESULT_OK != instance->mmsResult)
        {
            return MSA_MCR_FAILED;
        }
        instance->mmsResult = MMSlib_transactionClose(WE_MODID_MSA, 
            &instance->tHandle);
        if (MMS_RESULT_OK != instance->mmsResult)
        {
            return MSA_MCR_FAILED;
        }
        /* Free body params */
        freeBodyParams(&instance->bodyParams);
        /* Next item */
        instance->currentMo = instance->currentMo->next;
        if ((NULL != instance->currentMo) || 
            msaIsSmilValid(instance->msg->smilInfo))
        {
            /* Open a new transaction for the next part or the smil part if 
               there are no more objects */
            instance->mmsResult = MMSlib_transactionOpen(WE_MODID_MSA, 
                &instance->tHandle);
            if (MMS_RESULT_OK != instance->mmsResult)
            {
                return MSA_MCR_FAILED;
            }
        }
        if (NULL == instance->currentMo)
        {
            instance->state = (msaIsSmilValid(instance->msg->smilInfo))
                ? WRITE_SMIL_PART : FINALISE_MSG;
        }
        /* Do not execute the entire work at once */
        (void)MSA_SIGNAL_SENDTO(MSA_MCR_FSM, MSA_SIG_MCR_FILE_NOTIFY);
        return MSA_MCR_WAIT_RSP;
        
    case WRITE_SMIL_PART:
        /* Only allocate dynamic data once */
        if (NULL == instance->tmpBuffer)
        {
            /* Check if the SMIL needs to be created or not */
            if (!msaCreateSmil(instance->msg->smilInfo, 
                (char **)&(instance->tmpBuffer), &(instance->tmpSize)))
            {
                return MSA_MCR_FAILED;
            }
            /* Create body params, start by creating a temporary content-type */
            memset(&contentType, 0, sizeof(MmsContentType));
            contentType.strValue = (unsigned char *)MMS_MEDIA_TYPE_STRING_SMIL;
            contentType.knownValue = MMS_VALUE_AS_STRING;
            if (NULL == (instance->bodyParams = msaCreateBodyParams(
                MMS_MULTIPART_MESSAGE, &contentType, 
                (unsigned char *)MSA_SMIL_FILE_NAME)))
            {
                return MSA_MCR_FAILED;
            }
        }
        msaPmSetProgress(90, NULL);
        /* Write message */
        instance->mmsResult = MMSlib_setMessageBody(WE_MODID_MSA, 
            instance->bodyParams, instance->tmpBuffer, instance->tmpSize, 
            &instance->msgHandle, &instance->tHandle);
        if (MMS_RESULT_DELAYED == instance->mmsResult)
        {
            fHandle = MMSlib_getFileHandle(WE_MODID_MSA, &instance->tHandle);
            Msa_AsyncRegister(fHandle, MSA_MCR_FSM, MSA_SIG_MCR_FILE_NOTIFY, 0);
            /* Async! .. we'll arrive here again, soon. */
            return MSA_MCR_DELAYED; 
        }
        else if (MMS_RESULT_OK != instance->mmsResult)
        {
            return MSA_MCR_FAILED;
        }
        freeBodyParams(&instance->bodyParams);
        instance->mmsResult = MMSlib_transactionClose(WE_MODID_MSA, 
            &instance->tHandle);
        if (MMS_RESULT_OK != instance->mmsResult)
        {
            return MSA_MCR_FAILED;
        }
        instance->state = FINALISE_MSG;
        /* Release the SMIL part */
        releaseSmilBuffer(instance);
        /* Do not execute the entire work at once */
        (void)MSA_SIGNAL_SENDTO(MSA_MCR_FSM, MSA_SIG_MCR_FILE_NOTIFY);
        return MSA_MCR_WAIT_RSP;

    case COPY_BODY:
        /*
         *  Copying the body is done in the signal handler, just return.
         */
        return MSA_MCR_WAIT_RSP;

    case FINALISE_MSG:
        msaPmSetProgress(95, NULL);
        if (IS_FORWARD_OP(instance)) 
        {
            /* Cleanup ASIO handles */
            if (NULL != mcrInstance->origMsg->asioReadHandle)
            {
                Msa_AsyncOperationStop(&mcrInstance->origMsg->asioReadHandle);
            } /* if */
            
            if (NULL != mcrInstance->origMsg->asioWriteHandle)
            {
                Msa_AsyncOperationStop(&mcrInstance->origMsg->asioWriteHandle);
            } /* if */
        }
        else
        {
            instance->mmsResult = MMSlib_messageClose(WE_MODID_MSA, 
                &instance->msgHandle);
            if (MMS_RESULT_OK != instance->mmsResult)
            {
                return MSA_MCR_FAILED;
            }
        }
        /* Check if the message is going to end up */
        MMSif_createMessageDone(WE_MODID_MSA, instance->msgId, 
            instance->saveInFolder);
        instance->state = MESSAGE_DONE_RESPONSE;
        return MSA_MCR_WAIT_RSP;
        
    case MESSAGE_DONE_RESPONSE:
        /* Delete the originating message if in "edit" mode, templates should 
           not be deleted */
        msaPmSetProgress(99, NULL);
        if (ORIGINAL_MSG_EXISTS(instance) && (instance->origMsg->del))
        {
            msaDeleteMsg(instance->origMsg->msgId, FALSE);
        }
        return MSA_MCR_OK;
        
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) erroneous state\n", __FILE__, __LINE__));
        break;
    }

    return MSA_MCR_FAILED;
}

/*!
 * \brief  Start of execution for the createMessage FSM. 
 *
 * \param fsm       callback FSM.
 * \param retSig    callback signal.
 * \param msg       message to create/send
 * \param origId    msgId of the originating msgId, if any, used for forwarding
 * \param folder    The folder to save the new message in.
 * \param del       Whether to delete original message afterwards
 * \param forwardMode Tells that this is a forward operation.
 *****************************************************************************/
void msaCreateMessage(MsaStateMachine fsm, unsigned int retSig, MsaMessage *msg, 
    MmsMsgId origId, WE_BOOL del, MmsFolderType folder, 
    MmsFileType origFileType, WE_BOOL forward)
{
    MsaCreateMessageSignal *sig;
        
    MSA_CALLOC(sig, sizeof(MsaCreateMessageSignal));
    sig->fsm            = fsm; 
    sig->retSig         = (int)retSig;
    sig->message        = msg;
    sig->origMsgId      = origId;
    sig->del            = del;
    sig->folder         = folder;
    sig->origFileType   = origFileType;
    sig->forwardMode    = forward;

    (void)MSA_SIGNAL_SENDTO_P(MSA_MCR_FSM, MSA_SIG_MCR_CREATE_MESSAGE, sig);
}

/*!
 * \brief Creates a instance for message creation
 * 
 * \return TRUE if successful, otherwise FALSE;
 *****************************************************************************/
static int createMcrInstance(const MsaCreateMessageSignal *msgData)
{
    /* Check that a instance can be created */
    if (NULL != mcrInstance) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) unable to create instance.\n", __FILE__, __LINE__));
        return FALSE;
    }
    else
    {
        /* Create instance */
        MSA_CALLOC(mcrInstance, sizeof(McrInstance));
        /* Copy pointer to message information structure */
        mcrInstance->fsm            = msgData->fsm;
        mcrInstance->sig            = msgData->retSig;
        mcrInstance->msg            = msgData->message;
        mcrInstance->saveInFolder   = msgData->folder;
        if (MMS_TEMPLATES == msgData->folder)
        {
            mcrInstance->msgType = MMS_SUFFIX_TEMPLATE;
        }
        else
        {
            mcrInstance->msgType = MMS_SUFFIX_SEND_REQ;
        }
        if (msgData->origMsgId > 0) 
        {
            MSA_CALLOC(mcrInstance->origMsg, sizeof(McrOriginalMessage));
            mcrInstance->origMsg->msgId          = msgData->origMsgId;
            mcrInstance->origMsg->del            = msgData->del;
            mcrInstance->origMsg->fileType       = msgData->origFileType;
            mcrInstance->origMsg->forwardMode    = msgData->forwardMode;
        }
        mcrInstance->state = CREATE_MSG_ID;
        return TRUE;
    }
}

/*!
 * \brief Deallocates the current instance and its resources.
 *
 * \param result The result to send back to the originating FSM.
 *****************************************************************************/
static void cleanupMcrInstance(MsaMcrResult result)
{
    if (NULL != mcrInstance)
    {
        /* Close transaction */
        if (NULL != mcrInstance->tHandle)
        {
            (void)MMSlib_transactionClose(WE_MODID_MSA, 
                &mcrInstance->tHandle);
        }

        /* Close message */
        if (NULL != mcrInstance->msgHandle)
        {
            (void)MMSlib_messageClose(WE_MODID_MSA, &mcrInstance->msgHandle);
            mcrInstance->msgHandle = 0;
        }

        if (NULL != mcrInstance->msgHdr)
        {
            MMSlib_freeMmsSetHeader(WE_MODID_MSA, mcrInstance->msgHdr);
            MSA_FREE(mcrInstance->msgHdr);
            mcrInstance->msgHdr = NULL;
        }

        /* Free originating message */
        if (ORIGINAL_MSG_EXISTS(mcrInstance)) 
        {
            if (NULL != mcrInstance->origMsg->hdr) 
            {
                /* Clean up the original header */
                MMSlib_freeMmsGetHeader(WE_MODID_MSA, 
                    mcrInstance->origMsg->hdr);
                MSA_FREE(mcrInstance->origMsg->hdr);
                mcrInstance->origMsg->hdr = NULL;
            }

            if (NULL != mcrInstance->origMsg->skeleton) 
            {
                /* Clean up the original header */
                MMSif_freeMmsBodyInfoList(WE_MODID_MSA, 
                    mcrInstance->origMsg->skeleton);
                mcrInstance->origMsg->skeleton = NULL;
            }

            /* Cleanup ASIO handles */
            if (NULL != mcrInstance->origMsg->asioReadHandle)
            {
                Msa_AsyncOperationStop( &mcrInstance->origMsg->asioReadHandle);
            } /* if */            
            if (NULL != mcrInstance->origMsg->asioWriteHandle)
            {
                Msa_AsyncOperationStop( &mcrInstance->origMsg->asioWriteHandle);
            } /* if */

            /* Free filenames */
            MSA_FREE(mcrInstance->origMsg->srcFile);
            mcrInstance->origMsg->srcFile = NULL;
            
            MSA_FREE(mcrInstance->origMsg->dstFile);
            mcrInstance->origMsg->dstFile = NULL;
            
            MSA_FREE(mcrInstance->origMsg->copyBuffer);
            MSA_FREE(mcrInstance->origMsg);
            mcrInstance->origMsg = NULL;
        }

        /* Stop messing with message */
        mcrInstance->msg = NULL;

        /* Body params */
        if (NULL != mcrInstance->bodyParams)
        {
            freeBodyParams(&mcrInstance->bodyParams);
        }
        /* Handle the temporary SMIL buffer */
        releaseSmilBuffer(mcrInstance);
        /* Remove progress meter just in case */
        msaPmViewRemove();
        
        /* Delete the message if creation fails */
        if (MSA_MCR_FAILED == result)
        {
            msaDeleteMsg(mcrInstance->msgId, FALSE);
        }

        /* Send response */
        if (MSA_MCR_CLEANUP_SILENT != result)
        {
            (void)MSA_SIGNAL_SENDTO_IUU(mcrInstance->fsm, mcrInstance->sig, 
                mcrInstance->msgType, result, mcrInstance->msgId);
        }

        /* Free instance */
        MSA_FREE(mcrInstance);
        mcrInstance = NULL;
    }
}

/*!
 * \brief The message editor signal handling logic
 * \param sig The received signal
 *****************************************************************************/
static void mcrSignalHandler(MsaSignal *sig)
{
    MmsResult res;

    switch(sig->type)
    {
    case MSA_SIG_MCR_CREATE_MESSAGE:
        if (!createMcrInstance((MsaCreateMessageSignal*)sig->p_param))
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
            MSA_FREE(sig->p_param);
            break;
        }
        MSA_FREE(sig->p_param);
        if (!msaPmViewShow(MSA_STR_ID_GENERATING_MESSAGE, TRUE,
            FALSE, 0, 0))
        {
            msaPmViewRemove();
        }
        if (MSA_MCR_FAILED == msaCreateMessageFSM(mcrInstance))
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
        }
        /* The data provided through p_param is used in the createMcrInstance
            function */
        break;


    case MSA_SIG_MCR_CREATE_MSG_RESPONSE:
        res = (MmsResult)sig->u_param1;
        if (MMS_RESULT_OK != res) 
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
            break; 
        }
        mcrInstance->msgId = (MmsMsgId)sig->u_param2;
        if (MSA_MCR_FAILED == msaCreateMessageFSM(mcrInstance))
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
        }
        break;

    case MSA_SIG_MCR_FILE_NOTIFY:
    case MSA_SIG_MCR_CONTINUE:
        if (MSA_MCR_FAILED == msaCreateMessageFSM(mcrInstance))
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
        }
        break;

    case MSA_SIG_MCR_ASIO_READ_RSP:
        if (!handleReadOperation(mcrInstance, (signed)sig->u_param2,
            (MsaAsioResult)sig->u_param1)) 
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
        }
        else 
        {
            /* write data to the end of the file */
            Msa_AsyncWrite(mcrInstance->origMsg->asioWriteHandle, 
                -1, mcrInstance->origMsg->copyBuffer, 
                (unsigned)mcrInstance->origMsg->sizeToWrite);            
        }
        break;

    case MSA_SIG_MCR_ASIO_WRITE_RSP:
        if (!handleWriteOperation((MsaAsioResult)sig->u_param1))
        {
            /* Failed to write */
            /* End saving, just clean up and return */
            cleanupMcrInstance(MSA_MCR_FAILED);
        }
        else
        {
            if (0 == mcrInstance->origMsg->leftToRead)
            {
                /* finish the job */
                mcrInstance->state = FINALISE_MSG;
                if (MSA_MCR_FAILED == msaCreateMessageFSM(mcrInstance))
                {
                    cleanupMcrInstance(MSA_MCR_FAILED);
                }
            }
            else
            {
                /* read next part of the message */
                Msa_AsyncRead(mcrInstance->origMsg->asioReadHandle, 
                    (int)mcrInstance->origMsg->readPos, 
                    mcrInstance->origMsg->copyBuffer, 
                    MIN(mcrInstance->origMsg->leftToRead, 
                    mcrInstance->origMsg->copyBufferSize));
            }/* if */
        }/* if */
        break;

    case MSA_SIG_MCR_GET_HEADER_RSP:
        if (MMS_RESULT_OK == sig->u_param1)
        {
            /* Set the header from the originating message */
            mcrInstance->origMsg->hdr = sig->p_param;
            if (!msaCreateMessageFSM(mcrInstance))
            {
                cleanupMcrInstance(MSA_MCR_FAILED);
            }
        }
        else
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
        }
        break;

    case MSA_SIG_MCR_GET_SKELETON_RSP:
        if (MMS_RESULT_OK == sig->u_param1)
        {
            /* Set the header from the originating message */
            mcrInstance->origMsg->skeleton = sig->p_param;
            if (!msaCreateMessageFSM(mcrInstance))
            {
                cleanupMcrInstance(MSA_MCR_FAILED);
            }
        }
        else
        {
            cleanupMcrInstance(MSA_MCR_FAILED);
        }
        break;

    case MSA_SIG_MCR_CREATE_MSG_DONE_RESPONSE:
        msaPmViewRemove();
        if (MMS_RESULT_OK != (MmsResult)sig->u_param1)
        {
            /* Cleanup */
            cleanupMcrInstance(MSA_MCR_FAILED);
            break;
        }
        
        cleanupMcrInstance((MSA_MCR_OK == msaCreateMessageFSM(mcrInstance)) 
            ? MSA_MCR_OK : MSA_MCR_FAILED);
        break;
        
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "MCR: Erroneous signal received (%d).\n", sig->type));
    }
    /* Remove signal data */
    msaSignalDelete(sig);
}

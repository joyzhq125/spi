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

/*! \file mamr.c
 *  \brief Handles the re-creation of a message, i.e., reads the content of a
 *         message from disk into a structure suited for MSA.
 */

/* WE */
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Int.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Dcvt.h"
#include "We_Wid.h"
#include "We_Chrs.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Def.h"
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Mem.h"
#include "Msa_Async.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Conf.h"
#include "Msa_Core.h"
#include "Msa_Utils.h"
#include "Msa_Slh.h"
#include "Msa_Uipm.h"
#include "Msa_Addr.h"
#include "Msa_Uicmn.h"
#include "Msa_Moh.h"

/******************************************************************************
 *Macros
 *****************************************************************************/

/* Tells if a body-part is containing a SMIL presentation */
#define IS_SMIL_PART(cc) ((cc->contentType->knownValue == MMS_VALUE_AS_STRING)\
    && (0 == we_cmmn_strcmp_nc((char *)cc->contentType->strValue,\
    MMS_MEDIA_TYPE_STRING_SMIL)

/******************************************************************************
 * Data-types
 *****************************************************************************/
/*!\struct McrInstance 
 * \brief Information MCR needs to keep while recreating a message
 */
typedef struct 
{
    /* Used internally */
    MsaStateMachine     fsm;             /*!< The callback FSM */
    unsigned int        signal;          /*!< The callback signal */
    MsaMrStartupMode    mode;            /*!< The start-up mode */
    WE_BOOL            onlyHeader;      /*!< Whether to ignore body-parts */
    SisOpenSmilRsp      *sisSmilInfo;    /*!< Information about the SMIL 
                                              presentation */
    SisGetSlideInfoRsp  *sisSlideInfo;   /*!< The list of slides */

    WE_UINT32          objCount;        /*!< The total number of objects */
    WE_UINT32          objIndex;        /*!< The current object */
    WE_UINT32          currentSlide;    /*!< The index of the processed slide*/

    struct 
    {
        MmsBodyInfoList     *bodyPart;   /*!< The current body-part index */
        unsigned char       *buffer;     /*!< */
    }current;
    unsigned int        sizeWarning;     /*!< Number of size warnings */

    MsaMessage          *msg;            /*!< The message being created */

    /* Retrieval data */
    MmsGetHeader        *msgHeader;      /*!< The retrieved header. */
    MmsBodyInfoList     *msgSkeleton;    /*!< The message structure. */

    /* Handles used when reading message content */
    MsaAsyncOperationHandle pipeOp;     /*!< Pipe-handle, used when reading 
                                              body-parts */    
}MsaMrInstance;

/******************************************************************************
 * Static variables 
 *****************************************************************************/
static MsaMrInstance *mrInstance;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static char *createNameFromEntryHeader(const MmsEntryHeader *entries);
static WE_BOOL createMrInstance(MsaMrInstance **inst, 
    const MsaMrStartData *mrData);
static void deleteMrInstance(MsaMrInstance **inst, WE_BOOL useCallback, 
    MsaMrResult result);
static WE_BOOL mrMoIsText(const MmsBodyInfoList *bi, int *charset);
static WE_BOOL msaIsSmil(const MmsBodyInfoList *bodyInfoList);
static MsaMessageType getMessageType(const MmsBodyInfoList *bodyInfoList);
static WE_BOOL validateAddresses(const MmsAddressList *list);
static void getMsgPropertiesExpiryTime(const MsaMrInstance *inst);
static void getMsgPropertiesReplyCharging(const MsaMrInstance *inst);
static void getMsgPropertiesSubject(const MsaMrInstance *inst);
static WE_BOOL getMsgPropAddresses(const MsaMrInstance *inst);
static WE_BOOL mrParseHeader(MsaMrInstance *inst);
static WE_BOOL mrReadBodyPart(MsaMrInstance *inst, char *pipeName, 
    WE_UINT32 bodyPartSize, MsaStateMachine retFsm, int retSig);
static WE_BOOL createMsaSlide(const MsaMrInstance *inst, 
    SisSlideInfo *slideInfo);
static void mrSignalHandler(MsaSignal *sig);
static void handleObjectTiming(const MsaSmilInfo *smilInfo, WE_UINT32 begin, 
    WE_UINT32 dur, WE_UINT32 end, MsaMediaGroupType gType);
static char *createNameFromParams(MmsAllParams **params);
static WE_UINT32 countObjects(const MmsBodyInfoList *list);
static void handleGetHeader(MsaMrInstance **inst, MmsResult result, 
    MmsGetHeader *header);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*! \brief Calculates how many body-parts that are present in a message.
 *	Note only body-parts containing content is counted.
 *
 * \param list The MMS structure.
 * \return The total number of body-parts.
 *****************************************************************************/
static WE_UINT32 countObjects(const MmsBodyInfoList *list)
{
    WE_UINT32 count = 0;
    /* Go through all body-parts */
    while(list != NULL)
    {
        /* List entries have more than zero entries and size zero is just a node */
        count += ((list->numOfEntries == 0) && (list->size > 0)) ? 1: 0;
        /* Next entry */
        list = list->next;
    }
    return count;
}

/*!
 * \brief Creates a name for one attachment.
 *
 * \param str The buffer to add the attachment name to.
 * \param strMaxLen The length of the buffer.
 * \param entries The entry header to check.
 * \return A name if one existed, otherwise NULL.
 *****************************************************************************/
static char *createNameFromEntryHeader(const MmsEntryHeader *entries)
{
    while(entries)
    {
        if (((MMS_WELL_KNOWN_CONTENT_LOCATION == entries->headerType) || 
            (MMS_WELL_KNOWN_CONTENT_ID == entries->headerType)) && 
            (NULL != entries->value.wellKnownFieldName))
        {
            return we_cmmn_strdup(WE_MODID_MSA, 
                (char*)entries->value.wellKnownFieldName);
        }
        entries = entries->next;
    }
    return NULL;
}

/*!
 * \brief Creates a name for one attachment.
 *
 * \param params The params to check.
 * \return The file name if present, otherwise NULL.
 *****************************************************************************/
static char *createNameFromParams(MmsAllParams **params)
{
    MmsAllParams *current;
    char         *ret;
    if (NULL == params)
    {
        return NULL;
    }
    current = *params;
    /* try to find an MMS_FILENAME or MMS_NAME param */
    while (NULL != current) 
    {
        if (((MMS_NAME == current->param) || (MMS_FILENAME == current->param)) &&
            (MMS_PARAM_STRING == current->type) &&
            (NULL != current->value.string))
        {
            /* Copy the name */
            ret = we_cmmn_strdup(WE_MODID_MSA, (char *)current->value.string);
            /* Remove the params from the content-type */
            msaRemoveMmsParam(params, MMS_NAME);
            msaRemoveMmsParam(params, MMS_FILENAME);
            /* Use only the first valid name */
            return ret;
        }
        current = current->next;
    }
    return NULL;
}

/*!
 * \brief Initiates the message re-creation FSM
 *****************************************************************************/
void msaMrInit(void)
{
    mrInstance = NULL;
    msaSignalRegisterDst(MSA_MR_FSM, mrSignalHandler);
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, "MR initialized.\n"));
}

/*!
 * \brief Terminates the message edit FSM.
 *****************************************************************************/
void msaMrTerminate(void)
{
    deleteMrInstance(&mrInstance, FALSE, MSA_MR_OK);
    /* Deregister queue */
    msaSignalDeregister(MSA_MR_FSM);
}

/*!
 * \brief Creates a MCR instance.
 *
 * \param inst The current instance.
 * \param mrData All needed startup data
 * \return TRUE if the instance was created, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL createMrInstance(MsaMrInstance **inst, 
    const MsaMrStartData *mrData)
{
    MsaMrInstance *newInst = NULL;

    if (NULL != *inst)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) MR instance is busy!\n", __FILE__, __LINE__));
        return FALSE;
    }

    /* Allocate the new instance */
    MSA_CALLOC(newInst, sizeof(MsaMrInstance));
    MSA_CALLOC(newInst->msg, sizeof(MsaMessage));
    MSA_CALLOC(newInst->msg->msgProperties, sizeof(MsaPropItem));
    
    /* Set-up initial data */
    newInst->mode           = mrData->mode;
    newInst->msg->fileType  = mrData->fileType;
    newInst->msg->msgId     = mrData->msgId;
    newInst->fsm            = mrData->retFsm;
    newInst->signal         = (unsigned int)mrData->retSig;
    *inst = newInst;
    /* Display progress meter */
    (void)msaPmViewShow(MSA_STR_ID_READING_MSG, TRUE, FALSE, 0, 0);
    return TRUE;
}

/*!
 * \brief Deletes a MCR instance and sends a callback signal as a result of 
 *        the operation.
 *
 * \param inst          The current instance.
 * \param useCallback   TRUE if the result of the operations is sent the the 
 *                      originating FSM, otherwise FALSE.
 * \param result        The result of the operation.
 *****************************************************************************/
static void deleteMrInstance(MsaMrInstance **inst, WE_BOOL useCallback, 
    MsaMrResult result)
{
    MsaMrInstance *current = *inst;
    /* Check that there is a valid instance to use */
    if ((inst != NULL) && (*inst != NULL))
    {
        /* Check if the SIS module needs to be stopped */
        if (msaIsModuleStarted(WE_MODID_SIS))
        {
            /* Close down the SIS module */
            msaModuleStop(WE_MODID_SIS, FALSE);
        }

        /* Send the response */
        if (useCallback)
        {
            if (MSA_MR_OK != result)
            {
                /* The operations has failed, send the response */
                (void)MSA_SIGNAL_SENDTO_U((*inst)->fsm, (int)(*inst)->signal, 
                    result);
            }
            else
            {
                /* Check if the message can be changed to SMIL */
                if ((MSA_MR_STARTUP_NORMAL == (*inst)->mode) && 
                    (current->msg->msgType != MSA_MR_SMIL) && 
                    (NULL != current->msg->smilInfo) && 
                    (NULL == current->msg->smilInfo->mediaObjects))
                {
                    msaInsertSlide(current->msg->smilInfo);
                    current->msg->msgType = MSA_MR_SMIL;
                }
                /* Everything went fine; send generated msg in signal */
                if ((MSA_MR_SMIL == current->msg->msgType) && 
                    (NULL != current->msg->smilInfo))
                {
                    (*inst)->msg->smilInfo->currentSlide = 
                        msaGetFirstSlide((*inst)->msg->smilInfo);
                } /* if */
                (void)MSA_SIGNAL_SENDTO_UUP((*inst)->fsm, 
                    (int)current->signal, result, (*inst)->sizeWarning,
                    (*inst)->msg);
                current->msg = NULL; /* won't be deleted, then */
            }
        }

        /* Deallocate SIS structures */
        if (NULL != current->sisSlideInfo)
        {
            SISif_freeSisGetSlideInfo(WE_MODID_MSA, current->sisSlideInfo);
            MSA_FREE(current->sisSlideInfo);
            current->sisSlideInfo = NULL;
        }
        if (NULL != current->sisSmilInfo)
        {
            SISif_freeSisOpenSmil(WE_MODID_MSA, current->sisSmilInfo);
            MSA_FREE(current->sisSmilInfo);
            current->sisSlideInfo = NULL;
        }

         /* Free header */
        if (NULL != (*inst)->msgHeader)
        {
            MMSif_freeMmsGetHeader(WE_MODID_MSA, (*inst)->msgHeader);
            MSA_FREE(current->msgHeader);
            current->msgHeader = NULL;
        }
        /* Free skeleton */
        if (NULL != (*inst)->msgSkeleton)
        {
            MMSif_freeMmsBodyInfoList(WE_MODID_MSA, (*inst)->msgSkeleton);
            (*inst)->msgSkeleton = NULL;
        }
        /* Async IO operations */
        if (NULL != current->pipeOp)
        {
            Msa_AsyncOperationStop(&(current->pipeOp));
            current->pipeOp = NULL;
        }
        /* Check if the temp buffer needs to be deleted */
        if (NULL != current->current.buffer)
        {
            MSA_FREE(current->current.buffer);
            current->current.buffer = NULL;
        }
        msaFreeMessage(&current->msg);
        /* Delete the instance */
        MSA_FREE(current);
        current = NULL;
        *inst = NULL;
        /* Done */
        /* Delete progress meter */
        msaPmViewRemove();
    }
}

/*!
 * \brief Checks if a media object is a text
 * \param bi Information about the body-part containing the media object 
 * \return TRUE if the MO is just text, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL mrMoIsText(const MmsBodyInfoList *bi, int *charset)
{
    MmsAllParams *params;
    unsigned int tmpCharset;

    if (NULL == bi->contentType)
    {
        return FALSE;
    }
    /* Check that this is a valid text type */
    if (MSA_MEDIA_GROUP_TEXT != msaMimeToObjGroup(
        (char *)bi->contentType->strValue))
    {
        /* This is not a valid text type */
        return FALSE;
    }

    /* First, try to find a charset param */
    params = bi->contentType->params;                    
    while (params)
    {
        if ((MMS_CHARSET == params->param) &&
            (MMS_PARAM_INTEGER == params->type) &&
            (WE_CHARSET_UNKNOWN != (tmpCharset = 
            msaIsValidCharset((MmsCharset)params->value.integer))))
        {
            /* We found a charset param! Must be text, then.. */
            *charset = (int)tmpCharset;
            return TRUE;
        }
        /* Keep on looping.. */
        params = params->next;        
    }

    /* If unsuccessful at finding charset, assume these are text anyway */
    *charset = WE_CHARSET_ASCII;
    return TRUE;
}

/*!
 * \brief Checks if a message contains a SMIL presentation.
 * 
 * \param msgHeader A pointer to a valid message header.
 * \return TRUE if the message contains SMIL presentation, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL msaIsSmil(const MmsBodyInfoList *bodyInfoList)
{
    if ((NULL != bodyInfoList) && 
        (NULL != bodyInfoList->contentType) &&
        (MMS_VND_WAP_MULTIPART_RELATED == 
        bodyInfoList->contentType->knownValue))
    {
        /* Traverse all body-parts in hunt of a SMIL presentation */
        while (NULL != bodyInfoList)
        {
            /* Check params for current body-part - is it a SMIL ? */
            if ((MMS_VALUE_AS_STRING == bodyInfoList->contentType->knownValue) &&
                (NULL != bodyInfoList->contentType->strValue) &&
                (0 == we_cmmn_strcmp_nc((char *)bodyInfoList->contentType->strValue, 
                MMS_MEDIA_TYPE_STRING_SMIL)))
            {
                return TRUE;
            }
            bodyInfoList = bodyInfoList->next;
        }
    }
    return FALSE;
}

/*!
 * \brief Finds out what type of message to edit.
 *
 * \param msgHeader The message header.
 * \return The type of message, see #MrMsgType.
 *****************************************************************************/
static MsaMessageType getMessageType(const MmsBodyInfoList *bodyInfoList)
{
    /* Check for plain text */
    if (NULL != bodyInfoList)
    {
       if (msaIsSmil(bodyInfoList))
        {
            return MSA_MR_SMIL;
        }
        else if (MMS_VND_WAP_MULTIPART_MIXED == 
            bodyInfoList->contentType->knownValue)
        {
            return MSA_MR_MULTIPART_MIXED;
        }
        else if (MMS_VND_WAP_MULTIPART_ALTERNATIVE ==
            bodyInfoList->contentType->knownValue)
        {
            return MSA_MR_MULTIPART_ALTERNATIVE;
        }
        else if (0 == bodyInfoList->numOfEntries)
        {
            return MSA_MR_PLAIN_TEXT;
        }
       
    }
    return MSA_MR_UNSUPPORTED;
}

/*!
 * \brief Check that the addresses used can be handled.
 *
 * \param list The list of messages.
 * \return TRUE if all address types can be handled, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL validateAddresses(const MmsAddressList *list)
{
    while(NULL != list)
    {
        if ((MMS_PLMN != list->current.addrType) && 
            (MMS_EMAIL != list->current.addrType))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Unsupported address type: %d!\n", __FILE__, 
                __LINE__,list->current.addrType));
            return FALSE;
        }
        list = list->next;
    }
    return TRUE;
}

/*!
 * \brief Gets the message property "Expiry Time" from the message header.
 *
 * \param inst The current instance.
 *****************************************************************************/
static void getMsgPropertiesExpiryTime(const MsaMrInstance *inst)
{
    MsaPropItem *propItem;
    long diff;

    propItem = inst->msg->msgProperties;

    if (MMS_TIME_ABSOLUTE == inst->msgHeader->expiryTime.type)
    {
        diff = (long)(inst->msgHeader->expiryTime.value - inst->msgHeader->date);
    }
    else
    {
        diff = (long)inst->msgHeader->expiryTime.value;        
    }
    /*  Only positive values please */
    if (diff < 0)
    {
        diff = -diff;
    }
    
    if (diff <= MSA_EXPIRY_1_HOUR_IN_SEC)
    {
        propItem->expiryTime = MSA_EXPIRY_1_HOUR;
    }
    else if (diff > MSA_EXPIRY_1_HOUR_IN_SEC && 
        diff <= MSA_EXPIRY_12_HOURS_IN_SEC) 
    {
        propItem->expiryTime = MSA_EXPIRY_12_HOURS;
    }
    else if (diff > MSA_EXPIRY_12_HOURS_IN_SEC && 
        diff <= MSA_EXPIRY_1_DAY_IN_SEC) 
    {
        propItem->expiryTime = MSA_EXPIRY_1_DAY;
    }
    else if (diff > MSA_EXPIRY_1_DAY_IN_SEC && 
        diff <= MSA_EXPIRY_1_WEEK_IN_SEC) 
    {
        propItem->expiryTime = MSA_EXPIRY_1_WEEK;
    }
    else
    {
        propItem->expiryTime = MSA_EXPIRY_MAX;
    }
}

/*!
 * \brief Copy reply charging properties into the proper fields from the
 *        message header.
 * \param inst The current instance.
 *****************************************************************************/
static void getMsgPropertiesReplyCharging(const MsaMrInstance *inst)
{
    MsaPropItem *propItem;

    if (!inst || !inst->msg || !inst->msg->msgProperties || !inst->msgHeader)
    {
        return;
    }

    if (((MSA_MR_STARTUP_REPLY == inst->mode) || 
        (MSA_MR_STARTUP_REPLY_ALL == inst->mode)) && 
        ((inst->msgHeader->replyCharging == MMS_REPLY_CHARGING_ACCEPTED)) ||
        ((inst->msgHeader->replyCharging == MMS_REPLY_CHARGING_ACCEPTED_TEXT_ONLY)))
    {
        /* If the forward mode is used then the message wid is needed in order
           to get a reply for free */
        if (NULL != inst->msgHeader->serverMessageId)
        {
            propItem = inst->msg->msgProperties;
            propItem->replyChargingId = MSA_ALLOC(strlen(
                inst->msgHeader->serverMessageId) + 1);
            strcpy(propItem->replyChargingId, inst->msgHeader->serverMessageId);
        }
    }
    else
    {
        /* If the the operation is not a reply operation then the reply charging
           information needs to be copied */
        /* Reply charging */
        switch(inst->msgHeader->replyCharging)
        {
        case MMS_REPLY_CHARGING_NOT_SET:
            inst->msg->msgProperties->replyCharging = MSA_REPLY_CHARGING_OFF;
            break;

        case MMS_REPLY_CHARGING_REQUESTED:
            inst->msg->msgProperties->replyCharging = MSA_REPLY_CHARGING_ON;
            break;

        case MMS_REPLY_CHARGING_REQUESTED_TEXT_ONLY:
            inst->msg->msgProperties->replyCharging = MSA_REPLY_CHARGING_TEXT_ONLY;
            break;

        case MMS_REPLY_CHARGING_ACCEPTED:
        case MMS_REPLY_CHARGING_ACCEPTED_TEXT_ONLY:
        default:
            inst->msg->msgProperties->replyCharging = 
                msaGetConfig()->replyCharging;
            break;
        }
        /* Get the default configuration for size on deadline if the reply 
           charging is used */
        if ((inst->msgHeader->messageType == MMS_MSG_TYPE_SEND_REQ) && 
            (inst->msg->msgProperties->replyCharging != MSA_REPLY_CHARGING_OFF))
        {
            /* Calculate the time */
            if (MMS_TIME_RELATIVE == inst->msgHeader->replyChargingDeadline.type)
            {
                /* Relative time indicated */
                if (0 == inst->msgHeader->replyChargingDeadline.value)
                {
                    inst->msg->msgProperties->replyChargingDeadline = MSA_REPLY_CHARGING_DEADLINE_MAX;
                }
                else if (inst->msgHeader->replyChargingDeadline.value <= MSA_1_DAY)
                {
                    inst->msg->msgProperties->replyChargingDeadline = MSA_REPLY_CHARGING_DEADLINE_1_DAY;
                }
                else if (inst->msgHeader->replyChargingDeadline.value <= MSA_1_WEEK)
                {
                    inst->msg->msgProperties->replyChargingDeadline = MSA_REPLY_CHARGING_DEADLINE_1_WEEK;
                }
                else
                {
                    inst->msg->msgProperties->replyChargingDeadline = MSA_REPLY_CHARGING_DEADLINE_MAX;
                }
            }
            else
            {
                /* Absolute time indicated */

                /*
                 *	MSA absolute value, assume longest time.
                 */
                inst->msg->msgProperties->replyChargingDeadline = MSA_REPLY_CHARGING_DEADLINE_MAX;                
            }

            /* Calculate the size */
            if (0 == inst->msgHeader->replyChargingSize)
            {
                inst->msg->msgProperties->replyChargingSize = MSA_REPLY_CHARGING_SIZE_MAX;
            }
            else if (inst->msgHeader->replyChargingSize <= MSA_1_KB)
            {
                inst->msg->msgProperties->replyChargingSize = MSA_REPLY_CHARGING_SIZE_1_KB;
            }
            else if (inst->msgHeader->replyChargingSize <= MSA_10_KB)
            {
                inst->msg->msgProperties->replyChargingSize = MSA_REPLY_CHARGING_SIZE_10_KB;
            }
            else
            {
                inst->msg->msgProperties->replyChargingSize = MSA_REPLY_CHARGING_SIZE_MAX;
            }
        }
    }
} 

/*!
 * \brief Gets the message property "Subject" from the message header.
 *
 * \param inst The current instance.
 *****************************************************************************/
static void getMsgPropertiesSubject(const MsaMrInstance *inst)
{
    MsaPropItem *propItem;
    char *str1;
    char *str2;
    WE_UINT32 prefixLength;
    WE_UINT32 oldSubjectLength; 
    WE_UINT32 newSubjectLength; 
    WE_UINT32 newSubjectSize;

    propItem = inst->msg->msgProperties;    

    if (MSA_MR_STARTUP_REPLY == inst->mode ||
        MSA_MR_STARTUP_REPLY_ALL == inst->mode || 
        MSA_MR_STARTUP_FORWARD == inst->mode) 
    {
        /* "RE: " or "FW: " should prefix subject */
        str1 = msaGetStringFromHandle(MSA_MR_STARTUP_FORWARD == inst->mode
            ? MSA_GET_STR_ID(MSA_STR_ID_FW)         
            : MSA_GET_STR_ID(MSA_STR_ID_RE));        

        if (inst->msgHeader->subject.text != NULL)
        {            
            prefixLength = (WE_UINT32)we_cmmn_utf8_strlen(str1);
            oldSubjectLength = (WE_UINT32)we_cmmn_utf8_strlen(
                inst->msgHeader->subject.text);

            /* must limit new subject to MSA_ME_SUBJECT_MAX_SIZE characters */
            if (oldSubjectLength > MSA_ME_SUBJECT_MAX_SIZE - prefixLength)
            {
                newSubjectLength = MSA_ME_SUBJECT_MAX_SIZE - prefixLength;
            }
            else
            {
                newSubjectLength = oldSubjectLength;
            }            
               
            /* we_cmmn_strndup() is not UTF8 aware, need to count bytes... 
             * Compare difference between the original string length and the 
             * and the limited string.
             */
            newSubjectSize = (WE_UINT32)(we_cmmn_utf8_str_incr(
                inst->msgHeader->subject.text, (int)newSubjectLength)
                - inst->msgHeader->subject.text);

            str2 = we_cmmn_strndup(WE_MODID_MSA, 
                inst->msgHeader->subject.text, (int)newSubjectSize);
            propItem->subject = we_cmmn_str3cat(WE_MODID_MSA, str1, str2, NULL);
            
            MSA_FREE(str1);       
            MSA_FREE(str2);
        }
        else
        {
            propItem->subject = str1;
        }
    }
    else
    {
        if ((NULL != inst->msgHeader->subject.text) && 
            (MMS_UTF8 == inst->msgHeader->subject.charset))
        {
            propItem->subject = MSA_ALLOC(
                strlen(inst->msgHeader->subject.text) + 1);
            strcpy(propItem->subject, inst->msgHeader->subject.text);
        }
    }
}

/*!
 * \brief Gets the message property "To" from the message header.
 *
 * \param inst The current instance.
 * \return TRUE on successful operation, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL getMsgPropAddresses(const MsaMrInstance *inst)
{
    MsaPropItem *propItem;

    propItem = inst->msg->msgProperties;

    if (MSA_MR_STARTUP_VIEW == inst->mode) 
    {
        /* in view mode there is no need to validate the addresses; show all */
        /* Copy all addresses */
        propItem->to = Msa_MmsToAddrList(inst->msgHeader->to, NULL);
        /* Verify From addresses */
        if (inst->msgHeader->from.address)
        {
            if (!Msa_AddrItemInsert(&(propItem->from), NULL, 
                Msa_MmsAddrTypeToMsa(inst->msgHeader->from.addrType),
                inst->msgHeader->from.address))
            {
                return FALSE;
            }
        }
        /* Copy all addresses */
        propItem->cc = Msa_MmsToAddrList(inst->msgHeader->cc, NULL);
    } 
    else if (MSA_MR_STARTUP_NORMAL == inst->mode) 
    {
        /* Verify To addresses */
        if (!validateAddresses(inst->msgHeader->to))
        {
            return FALSE;
        }
        /* Copy all addresses */
        propItem->to = Msa_MmsToAddrList(inst->msgHeader->to, NULL);
        /* Verify Cc addresses */
        if (!validateAddresses(inst->msgHeader->cc))
        {
            return FALSE;
        }
        /* Copy all addresses */
        propItem->cc = Msa_MmsToAddrList(inst->msgHeader->cc, NULL);
        /* Bcc addresses */
        if (inst->msg->fileType == MMS_SUFFIX_SEND_REQ)
        {
            if (!validateAddresses(inst->msgHeader->bcc))
            {
                return FALSE;
            }
            /* Copy all addresses */
            propItem->bcc = Msa_MmsToAddrList(inst->msgHeader->bcc, NULL);
        }
    }
    else if (MSA_MR_STARTUP_REPLY_ALL == inst->mode) 
    {
        /*
         *	REPLY_ALL algorithm: 
         *  1) copy CC and TO from the original message, excluding myself
         *  2) insert FROM-address of the original message into the TO header
         */

        /* Verify to addresses */
        if (!validateAddresses(inst->msgHeader->to))
        {
            return FALSE;
        }
        /* Copy all addresses except our own address */
        propItem->to = Msa_MmsToAddrList(inst->msgHeader->to, 
            msaGetConfig()->fromAddress);
        propItem->cc = Msa_MmsToAddrList(inst->msgHeader->cc, 
            msaGetConfig()->fromAddress);
    }
    
    if (MSA_MR_STARTUP_REPLY == inst->mode ||
        MSA_MR_STARTUP_REPLY_ALL == inst->mode) 
    {
        /* Copy from-address to to-address */
        if (inst->msgHeader->from.address)
        {
            if (!Msa_AddrItemInsert(&(propItem->to), NULL, 
                Msa_MmsAddrTypeToMsa(inst->msgHeader->from.addrType),
                inst->msgHeader->from.address))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) from address missing in originating message.\n", 
                    __FILE__, __LINE__));
                return FALSE;
            }
        }
    }
    return TRUE;
}

/*!
 * \brief Gets the message properties from the message header. Also analyzes
 *        the header. 
 *
 * \param inst The current instance.
 * \return TRUE on successful operation, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL mrParseHeader(MsaMrInstance *inst)
{
    MsaPropItem *propItem;

    propItem = inst->msg->msgProperties;

    if (NULL == inst->msgHeader->contentType)
    {
        return FALSE;
    }

    /* Sometimes only the header is needed.. (fwd, re, re all, view smil) */
    inst->onlyHeader =
        ((MSA_MR_STARTUP_FORWARD   == mrInstance->mode) ||
        (MSA_MR_STARTUP_REPLY      == mrInstance->mode) ||
        (MSA_MR_STARTUP_REPLY_ALL  == mrInstance->mode) ||
        ((MSA_MR_STARTUP_VIEW      == mrInstance->mode) 
        && (MSA_MR_SMIL            == mrInstance->msg->msgType))); 

    /* Handle addresses */
    if (!getMsgPropAddresses(inst))
    {
        return FALSE;
    }

    propItem->deliveryReport = (MMS_DELIVERY_REPORT_YES == 
        inst->msgHeader->deliveryReport);
    propItem->readReport = (MMS_READ_REPLY_YES == inst->msgHeader->readReply);

    switch(inst->msgHeader->priority) 
    {
    case MMS_PRIORITY_LOW:
        propItem->priority = MSA_PRIORITY_LOW;
        break;
    case MMS_PRIORITY_NORMAL:
        propItem->priority = MSA_PRIORITY_NORMAL;
        break;
    case MMS_PRIORITY_HIGH:
        propItem->priority = MSA_PRIORITY_HIGH;        
        break;
    default:
        /* If not set, use the default value */
        propItem->priority = msaGetConfig()->priority;
    /*lint -e{788} */
    }

    switch(inst->msgHeader->visibility)
    {
    case MMS_SENDER_HIDE:
        propItem->senderVisibility = MSA_SENDER_HIDE;
        break;
    case MMS_SENDER_SHOW:
    case MMS_SENDER_VISIBILITY_NOT_SET:
    default:
        /* If not set, use the default value */
        propItem->senderVisibility = msaGetConfig()->senderVisibility;
        break;
    }
           
    /*  Copy reply charging data to the proper fields */
    getMsgPropertiesReplyCharging(inst);

    /*  Get expiry time type and value */
    getMsgPropertiesExpiryTime(inst);

    /* subject */
    getMsgPropertiesSubject(inst);

    /* Dispose of retrieved header - it isn't needed anymore. */
    return TRUE;
}

/*!
 * \brief Handles reading of a single body-part 
 *
 * \param retFsm What FSM to call when encountering delayed reads
 * \param retSig The signal to send on delayed reads
 * \param number the body-part to read
 * \param data   Where to store the read body-part
 * \param size   Where to write the size of the read data
 * \return TRUE on successful and completed read, FALSE otherwise
 *****************************************************************************/
static WE_BOOL mrReadBodyPart(MsaMrInstance *inst, char *pipeName, 
    WE_UINT32 bodyPartSize, MsaStateMachine retFsm, int retSig)
{
    /*
     *	Init things on first arrival here (might get here several times due 
     *  to async file API ..) On second coming, we will have a transaction
     */
    if ((NULL == mrInstance) || (NULL == inst->current.bodyPart))
    {
        return FALSE;
    }
    /* Try to read the requested body-part */
    if (NULL == inst->current.buffer)
    {
        /* Allocate the buffer to recive the data in. Note that this is only
           performed the first time for each body-part */
        MSA_CALLOC(inst->current.buffer, bodyPartSize);
    }
    if (NULL == (inst->pipeOp = Msa_AsyncPipeOperationStart(pipeName, retFsm, 0, 
        (unsigned)retSig, MSA_ASYNC_MODE_READ)))
    {
        MSA_FREE(pipeName);
        return FALSE;
    }
    Msa_AsyncPipeRead(inst->pipeOp, inst->current.buffer, bodyPartSize);
    MSA_FREE(pipeName);
    return TRUE;
}

/* ! \brief Handles the response from the current pipe-operation.
 *
 * \param inst The current instance.
 *****************************************************************************/
static WE_BOOL handlePipeRsp(MsaMrInstance *inst)
{
    int             charset;
    MmsContentType  *contentType = NULL;
    char            *name;
    MsaMoDataItem   *current;

    /* The operations is performed, deallocate the pipe resources */
    Msa_AsyncOperationStop(&(inst->pipeOp));
    inst->pipeOp = NULL;

    /* Keep track of how many parts that have been read in order to provide
       a correct progress meter */
    mrInstance->objIndex += 1;

    /* Copy the originating content-type */
    contentType = MSA_ALLOC(sizeof(MmsContentType));
    MMSif_copyMmsContentType(WE_MODID_MSA, contentType, 
        inst->current.bodyPart->contentType);

    /*
     *	Body-part retrieved successfully - add it to media object list!
     *  (first converting to UTF8 if text)
     */
    if (mrMoIsText(inst->current.bodyPart, &charset))
    {
        /* This is a supported text type, do the conversion to UTF-8 */
        if (!msaConvertToUTF8(charset, &inst->current.bodyPart->size, 
            &(inst->current.buffer)))
        {
            return FALSE;
        }
        /* Do not use the old charset parameter */
        msaRemoveMmsParam(&contentType->params, MMS_CHARSET);
    } /* if */

    /*
     *	Check media object size limitations, warn later
     */
    if (MSA_MEDIA_GROUP_IMAGE == msaMimeToObjGroup(
        (char *)inst->current.bodyPart->contentType->strValue) &&
        inst->current.bodyPart->size > MSA_CFG_INSERT_IMAGE_MAX_SIZE ||
        MSA_MEDIA_GROUP_TEXT == msaMimeToObjGroup(
        (char *)inst->current.bodyPart->contentType->strValue) &&
        inst->current.bodyPart->size > MSA_CFG_INSERT_TEXT_MAX_SIZE)
    {
        mrInstance->sizeWarning++;
    }

    /* Name of media object */
    if (NULL != (name = 
        createNameFromParams(&inst->current.bodyPart->contentType->params)))
    {

    }else if (NULL == (name = 
        createNameFromEntryHeader(inst->current.bodyPart->entryHeader)))
    {
        /* No name could be extracted from the originating message, create
           a generated name */
        name = msaCreateFileNameFromType(
            (char *)inst->current.bodyPart->contentType->strValue);
    }
    /* Add the media object to the list of objects */ 
    if (NULL == (current = msaAddMo(&mrInstance->msg->smilInfo->mediaObjects, 
        inst->current.buffer, inst->current.bodyPart->size, contentType, 
        name)))
    {
        MMSif_freeMmsContentType(WE_MODID_MSA, contentType, TRUE);
        MSA_FREE(inst->current.buffer);
        inst->current.buffer = NULL;
        return FALSE;
    }
    /* Set the MMS body-part index */
    current->bpIndex = inst->current.bodyPart->number;
    /* The data is now handled by the media object-list */
    inst->current.buffer = NULL;

    return TRUE;
}

/*! \brief Handles the object timing.
 *
 * \param smilInfo The current SMIL presentation.
 * \param begin The start time for the object, in milliseconds.
 * \param dur The duration for the object, in milliseconds.
 * \param end The end time in milliseconds.
 * \param gType The type of object to set the timing for.
 *****************************************************************************/
static void handleObjectTiming(const MsaSmilInfo *smilInfo, WE_UINT32 begin, 
    WE_UINT32 dur, WE_UINT32 end, MsaMediaGroupType gType)
{
   /* Start */
    if (0 != begin)
    {
        (void)msaSetObjStart(smilInfo, gType, begin);
    }
    /* Duration */
    if (0 != dur)
    {
        (void)msaSetObjDuration(smilInfo, gType, dur);
    }
    else if (0 != end)
    {
        /* Create the duration from "begin" and "end" */
        (void)msaSetObjDuration(smilInfo, gType, end - begin);
    }
    /* If no valid stop time is present, then the max value for the slide
       is used by default, i.e., does not need be set */
}

/*! \brief Adds an object to a slide.
 *
 * \param  inst The current instance.
 * \param  obj The object to add.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL addObj(const MsaMrInstance *inst, const SisSlideElement *obj)
{
    MsaMoDataItem *item;
    /* Find the object in the list of objects */
    if (NULL == (item = msaFindObjectByMmsIndex(inst->msg->smilInfo->mediaObjects, 
        obj->objectId)))
    {
        /* No object found, this is terrible wrong */
        return FALSE;
    }
    /* Insert the media object */
    if (!msaSetSlideMoByRef(inst->msg->smilInfo, item))
    {
        return FALSE;
    }
    /* Handle the object duration */
    handleObjectTiming(inst->msg->smilInfo, obj->beginT, obj->duration, 
        obj->endT, msaMimeToObjGroup((char *)item->type->strValue));
    /* Color */
    if (!msaSetObjColor(inst->msg->smilInfo, msaMimeToObjGroup((char *)item->type->strValue),
        obj->fgColor, obj->bgColor))
    {
        /* The color failed this is a minor problem. Log and continue */
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s)(%d) Failed to set the color for a media object\n", __FILE__, 
            __LINE__));
    }

    return TRUE;
}

/*! \brief Reads the objects that are referenced from the current slide.
 *	
 * \param inst The current instance.
 * \param slideInfo The slide information.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL createMsaSlide(const MsaMrInstance *inst, 
    SisSlideInfo *slideInfo)
{
    WE_BOOL  textFirst = FALSE;
    SisRegion *imageRegion = NULL;
    SisRegion *textRegion = NULL;

    /* Get all references */
    SisSlideElement *image = SISif_getElementByType(slideInfo->elementList, SIS_OBJECT_TYPE_IMAGE);
    SisSlideElement *text  = SISif_getElementByType(slideInfo->elementList, SIS_OBJECT_TYPE_TEXT);
    SisSlideElement *audio = SISif_getElementByType(slideInfo->elementList, SIS_OBJECT_TYPE_AUDIO);

    /* No objects, dismiss the slide  */
    if ((NULL == image) && (NULL == text) && (NULL == audio))
    {
        return TRUE;
    }
    /* Insert a new slide */
    if (NULL == msaInsertSlide(inst->msg->smilInfo))
    {
        return FALSE;
    }
    /* Set the duration */
    msaSetDuration(inst->msg->smilInfo, slideInfo->duration);
    /* Get regions */
    if (image != NULL)
    {
        if (NULL == (imageRegion = SISif_getRegionFromId(inst->sisSmilInfo->regionList, 
            image->regionId)))
        {
            return FALSE;
        }
    }
    if (text != NULL)
    {
        if (NULL == (textRegion = SISif_getRegionFromId(inst->sisSmilInfo->regionList,
            text->regionId)))
        {
            return FALSE;
        }
    }

    /* Calculate whether the image region is further up than the text region */
    if ((NULL != text) && (NULL != textRegion) && (NULL != image) && (NULL != imageRegion))
    {        
        if (textRegion->position.cy.value < imageRegion->position.cy.value)
        {
            /* Text on top of image */
            textFirst = TRUE;
        }
        else if (textRegion->position.cy.value == 
            imageRegion->position.cy.value)
        {
            /* Check if the layout is left to right */
            if (textRegion->position.cx.value < imageRegion->position.cx.value)
            {
                /* Text on top of the image */
                textFirst = TRUE;
            }
        }
        /* Create the new objects in correct order */
        if (textFirst)
        {
            /* Text */
            if (!addObj(inst, text))
            {
                return FALSE;
            }
            /* Image */
            if (!addObj(inst, image))
            {
                return FALSE;
            }
        }
        else
        {

             /* Image */
            if (!addObj(inst, image))
            {
                return FALSE;
            }
            
            /* Text */
            if (!addObj(inst, text))
            {
                return FALSE;
            }
           
        }
    }
    else
    {
        /* Text */
        if ((NULL != text) && !addObj(inst, text))
        {
            return FALSE;
        }
        /* Image */
        if ((NULL != image) && !addObj(inst, image))
        {
            return FALSE;
        }
    }
    /* Audio */
    if ((NULL != audio) && !addObj(inst, audio))
    {
        return FALSE;
    }
    return TRUE;
}

/*! \brief Gets the next valid body-part
 *	
 * \param bodyPart The list of body-parts.
 * \param getNext Tells if the next entry are of interest or not.
 *****************************************************************************/
static void handleNextBodyPart(MmsBodyInfoList **bodyPart, WE_BOOL getNext)
{
    MmsBodyInfoList *current;

    if ((NULL == bodyPart) || (NULL == *bodyPart))
    {
        return;
    }
    current = (getNext) ? *bodyPart: (*bodyPart)->next;
    /* Do not handle the nodes in the tree in order to make a simple list */
    while ((NULL != current) && ((0 != current->numOfEntries) || 
        (current->size == 0) || IS_SMIL_PART(current)))))
    {
        current = current->next;
    }
    *bodyPart = current;
}

/*! \brief Handles the get header response.
 *
 * \param inst The current instance.
 * \param result The #MmsResult code for the operation.
 * \param header The #MmsgetHeader returned
 *****************************************************************************/
static void handleGetHeader(MsaMrInstance **inst, MmsResult result, 
    MmsGetHeader *header)
{
    MsaMrInstance *mrInst = *inst;
    if (MMS_RESULT_OK != result)
    {
        /* The operation failed, cleanup and report back */
        deleteMrInstance(inst, TRUE, MSA_MR_FAILED);
        return;
    }
    mrInst->msgHeader = header;
    /* Do not allow edit of forward locked PDU's */
    if ((mrInst->msgHeader->forwardLock) && 
        (mrInst->mode == MSA_MR_STARTUP_NORMAL || mrInst->mode == MSA_MR_STARTUP_FORWARD))
    {
        deleteMrInstance(inst, TRUE, MSA_MR_FORWARD_LOCK);
        return ;
    }
    if (!mrParseHeader(mrInst))
    {
        deleteMrInstance(inst, TRUE, MSA_MR_FAILED);
        return;
    }
    /* Sometimes only the header is needed.. (RE/FWD, for instance) */
    if (mrInst->onlyHeader)
    {
        /* All done! Lets close gracefully! */
        deleteMrInstance(inst, TRUE, MSA_MR_OK);
        return;
    }

    /* For SMIL messages to SIS is needed in order to parse the SMIL part */
    if (MSA_MR_SMIL == mrInstance->msg->msgType)
    {
        /* Start the SIS module, continue when the SIS is started */
        msaModuleStart(WE_MODID_SIS, TRUE, MSA_MR_FSM, 
            MSA_SIG_MR_SIS_RSP);
    }
    else
    {
        /* For none SMIL messages read all body-parts containing content  */
        /* Create a new SMIL presentation */

        /*  Insert code here for inserting media-objects outside the 
         *  SMIL-info structure once the media-object list has been broken 
         *  out from it
         */
        if (!msaCreateSmilInstance(&mrInst->msg->smilInfo))
        {
            msaCleanupSmilInstance(&(mrInst->msg->smilInfo));
            deleteMrInstance(inst, TRUE, MSA_MR_FAILED);
            return;
        }
        /* Get the message structure */
        MMSif_getMsgSkeleton(WE_MODID_MSA, mrInst->msg->msgId, 
            MSA_GET_SKELETON_RSP_MR);
    }
}

/*! \brief Handles the response from the call to MMSif_getMsgSkeleton
 *
 * \param inst The current instance
 * \param result The result of the MMSif_getMsgSkeleton operation, see 
 *               #MmsResult.
 * \parm skeleton The message skeleton.
 *****************************************************************************/
static void handleMsgSkeletonRsp(MsaMrInstance **inst, MmsResult result, 
    MmsBodyInfoList *skeleton)
{
    /* Handle the response data */
    if (MMS_RESULT_OK != result)
    {
        deleteMrInstance(inst, TRUE, MSA_MR_FAILED);
        return;
    }
    (*inst)->msgSkeleton = skeleton;

    /* Figure out what kind of object this is */
    (*inst)->msg->msgType = getMessageType((*inst)->msgSkeleton);
 
#ifndef MSA_BAD_SMIL_AS_MULTIPART_MIXED    
    if ((MSA_MR_SMIL == (*inst)->msg->msgType) && 
        (MSA_MR_STARTUP_VIEW == (*inst)->mode))
    {
        /* When started in "view" mode and the message type is "SMIL" then
           the SIA is going to display the content, i.e., clean-up and report */
        msaPmSetProgress(100, NULL);
        deleteMrInstance(inst, TRUE, MSA_MR_OK);
        return;
    }
#endif

    if((MSA_MR_PLAIN_TEXT!=(*inst)->msg->msgType) && ((*inst)->msgSkeleton->numOfEntries==0))
    {
         /* There where no valid body-parts to read, clean-up and report */
        deleteMrInstance(inst, TRUE, MSA_MR_OK);
        return;
    }
    /* Set the first object in the list to read from persistent storage */
    (*inst)->current.bodyPart = skeleton;
    (*inst)->objCount = countObjects(skeleton);
    handleNextBodyPart(&(*inst)->current.bodyPart, TRUE);

    /* Read body-part in the next state in order to support async IO */
    if (NULL != (*inst)->current.bodyPart)
    {
        /* Get the next part */
        MMSif_getBodyPart(WE_MODID_MSA, (*inst)->msg->msgId, 
            (WE_UINT8)(*inst)->current.bodyPart->number, MSA_GET_BP_RSP_DEST_MR);
    }
    else
    {
        /* There where no valid body-parts to read, clean-up and report */
        deleteMrInstance(inst, TRUE, MSA_MR_OK);
        return;
    }
}

/*! \brief Handles the MMSif_openSmil response.
 *
 * \param inst The current instance.
 * \param smilRsp The response from the operation
 *****************************************************************************/
static WE_BOOL handleSmilRsp(MsaMrInstance *inst, SisOpenSmilRsp *smilRsp)
{
    if ((NULL == smilRsp) || 
        (SIS_RESULT_OK != (SisResult)smilRsp->result))
    {
        return FALSE;
    } /* if */

    /* Create a new SMIL presentation */
    if (NULL == inst->msg->smilInfo)
    {
        /* The SMIL instance is not created, create it... */
        if (!msaCreateSmilInstance(&inst->msg->smilInfo))
        {
            return FALSE;
        }
    }
    inst->sisSmilInfo = smilRsp;
    inst->currentSlide = 0;
    inst->msg->smilInfo->bgColor = smilRsp->rootLayout.bgColor;
    return TRUE;
}

/*! \brief Handle the response from the SISif_getSlideInfo request
 *
 * \param inst The current instance
 * \param slideInfoRsp The response from the SISif_getSlideInfo request
 *****************************************************************************/
static WE_BOOL handleSlideInfo(MsaMrInstance *inst, 
    SisGetSlideInfoRsp *slideInfoRsp)
{
    SisSlideInfoList *slideInfo;
    /* Save the pointer to avoid memory leaks */
    inst->sisSlideInfo = slideInfoRsp;
    /* Handle the response */
    if (SIS_RESULT_OK != slideInfoRsp->result)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) The slide !\n", __FILE__, __LINE__));
        return FALSE;
    }
    /* Go through all slides */
    slideInfo = inst->sisSlideInfo->slideInfo;
    while(slideInfo != NULL)
    {
        if (!createMsaSlide(inst, &slideInfo->current))
        {
            return FALSE;
        }
        slideInfo = slideInfo->next;
    }
    /* Make sure that objects without reference are removed */
#ifndef MSA_BAD_SMIL_AS_MULTIPART_MIXED
    msaOptimizeMoList(inst->msg->smilInfo);
#endif
    return TRUE;
}

/*!
 * \brief The message re-creation signal handling logic.
 *
 * \param sig The received signal.
 *****************************************************************************/
static void mrSignalHandler(MsaSignal *sig)
{
    WE_BOOL ret;

    /* Guards */
    if ((mrInstance == NULL) && (sig->type != MSA_SIG_MR_START))
    {
        msaSignalDelete(sig);
        return;
    }

    switch(sig->type)
    {
    case MSA_SIG_MR_START:
        /*
         *	Initialization.
         */
        ret = createMrInstance(&mrInstance, (MsaMrStartData *)sig->p_param);
        if (!ret)
        {
            (void)MSA_SIGNAL_SENDTO_U(((MsaMrStartData *)sig->p_param)->retFsm, 
                ((MsaMrStartData *)sig->p_param)->retSig, MSA_MR_FAILED);
            MSA_FREE(sig->p_param);
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_FAILED);
            break;
        }
        MSA_FREE(sig->p_param);
        /* Read the header */
        MMSif_getMsgHeader(WE_MODID_MSA, mrInstance->msg->msgId, 
            MSA_GET_HEADER_RSP_MR);
        break;

    case MSA_SIG_MR_GET_HEADER_RSP:
        msaPmSetProgress(50, NULL);
        handleGetHeader(&mrInstance, (MmsResult)sig->u_param1, 
            (MmsGetHeader *)sig->p_param);
        break;
    
    case MSA_SIG_MR_GET_SKELETON_RSP:
        handleMsgSkeletonRsp(&mrInstance, (MmsResult)sig->u_param1, 
            (MmsBodyInfoList *) sig->p_param);
        break;


    case MSA_SIG_MR_GET_BODYPART_RSP:
        if (MMS_RESULT_OK != sig->u_param1)
        {
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_FAILED);
            break;
        }
        /* Body-parts of none SMIL related messages is going to be read here */
        if (NULL == mrInstance->current.bodyPart)
        {
            /* No use trying to read what's not there. Make do without. */
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_OK);
            break;
        } /* if */
        /*
         *	Read a body-part
         */
        if (!mrReadBodyPart(mrInstance, sig->p_param, sig->u_param2, MSA_MR_FSM,
            MSA_SIG_MR_PIPE_RSP))
        {
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_FAILED);
            break;
        } /* if */
        sig->p_param = NULL;
        break;
        
    case MSA_SIG_MR_PIPE_RSP:
        if (MSA_ASIO_RESULT_OK != sig->u_param1)
        {
            /* Operation failed clean-up and report */
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_FAILED);
            break;
        }
        /* Handle the body-part the have been read */
        if (!handlePipeRsp(mrInstance))
        {
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_FAILED);
            break;
        }
            /* Step the progress */
        msaPmSetProgress(50 + (mrInstance->objIndex*50)/mrInstance->objCount, 
            NULL);
        /* Next time, next body-part (or be done) */
        handleNextBodyPart(&mrInstance->current.bodyPart, FALSE);

        if (NULL != mrInstance->current.bodyPart) 
        {
            /* Get the next part */
            MMSif_getBodyPart(WE_MODID_MSA, mrInstance->msg->msgId, 
                (WE_UINT8)mrInstance->current.bodyPart->number, 
                MSA_GET_BP_RSP_DEST_MR);
            break;
        }
        else
        {
            if (mrInstance->msg->msgType == MSA_MR_SMIL)
            {
                /* This is a SMIL message, use the SIS in order to handle
                 * the presentation
                 */
                msaModuleStart(WE_MODID_SIS, TRUE, MSA_MR_FSM, 
                    MSA_SIG_MR_SIS_RSP);
            }
            else
            {
                /* This is a none-SMIL message, clean-up and report */
                deleteMrInstance(&mrInstance, TRUE, MSA_MR_OK);
            }
        } /* if */
        break;

    /*
     * SIS handling 
     */

    case MSA_SIG_MR_SIS_RSP:
        /*
         *	SIS is now started, let's open the SMIL.
         */
        SISif_openSmil(WE_MODID_MSA, mrInstance->msg->msgId, 0);
        break;

    case MSA_SIG_MR_OPEN_SMIL_RSP:
        /*
         *	The SMIL is now opened, we're ~50% done. Start parsing it. 
         */

        if (!handleSmilRsp(mrInstance, sig->p_param))
        {
            /* Failed to open the SMIL, treat the message as multi-part mixed. */
            mrInstance->msg->msgType = MSA_MR_MULTIPART_MIXED;
            /* Clean-up and report */
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_OK);
            break;
        }

        /* Get a list of all the slides */
        SISif_getSlideInfo(WE_MODID_MSA, 0, 
            mrInstance->sisSmilInfo->noOfSlides, 0);
        break;

    case MSA_SIG_MR_GET_SLIDE_INFO_RSP:
        /*
         *	Slide information retrieved. Time to start as many async file 
         *  reads as it takes to read all objects in the current slide.
         *  We will get MSA_SIG_MR_MO_READ signals laters for every op.
         */
        if (SIS_RESULT_OK != sig->u_param1)
        {
            /* Failed to open the SMIL, treat the message as multi-part mixed. */
            mrInstance->msg->msgType = MSA_MR_MULTIPART_MIXED;
            /* Clean-up and report */
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_OK);
            break;
        }
        /* Get the first slide and its media objects */
        if (!handleSlideInfo(mrInstance, sig->p_param))
        {
            deleteMrInstance(&mrInstance, TRUE, MSA_MR_FAILED);
            break;
        }
        /* The operation was completed, close the SMIL */
        SISif_closeSmil(WE_MODID_MSA, 0);
        break;

    case MSA_SIG_MR_CLOSE_SMIL_RSP:
        /*
         *	We're done with the SMIL reading/parsing. Time to close and report
         */
        deleteMrInstance(&mrInstance, TRUE, MSA_MR_OK);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "MR: Erroneous signal received (%d).\n", sig->type));
        break;
    }
    /* Remove signal data */
    msaSignalDelete(sig);
    /*lint -e{550} */
}

/*!
 * \brief Starts the MR FSM that will read a message from disk
 *
 * \param mode What mode to read message for (i.e. REPLY -> only read header)
 * \param msgID The msgId of the message to read
 * \param fileType The type of the message to read
 * \param retFsm The FSM to receive a signal when done
 * \param retSig The signal to receive when done
 *****************************************************************************/
void msaStartMr(MsaMrStartupMode mode, MmsMsgId msgId, MmsFileType fileType, 
                 MsaStateMachine retFsm, unsigned int retSig)
{
    MsaMrStartData *mrData;

    MSA_CALLOC(mrData, sizeof(MsaMrStartData));
    mrData->fileType   = fileType;
    mrData->msgId      = msgId;
    mrData->mode       = mode;
    mrData->retFsm     = retFsm;
    mrData->retSig     = (int)retSig;
    (void)MSA_SIGNAL_SENDTO_P(MSA_MR_FSM, MSA_SIG_MR_START, (void *)mrData);
}

/*!
 * \brief Deallocates a #MsaMessage structure completely
 *
 * \param msg The #MsaMessage structure to dispose of
 *****************************************************************************/
void msaFreeMessage(MsaMessage **msg)
{
    if ((NULL == msg) || (NULL == *msg))
    {
        return;
    } /* if */

    MSA_FREE((*msg)->fileName);
    
    /* message properties */
    msaFreePropItem((*msg)->msgProperties, FALSE);

    /* SMIL stuff */
    msaCleanupSmilInstance(&(*msg)->smilInfo);
    MSA_FREE(*msg);
    *msg = NULL;
}

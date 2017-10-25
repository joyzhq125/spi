/*
 * Copyright (C) Techfaith, 2002-2005.
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

/*--- Include files ---*/
/* WE */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Int.h"
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Chrs.h"
#include "We_Wid.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_def.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* SMA */
#include "Saintsig.h"
#include "Saasync.h"
#include "Sia_if.h"
#include "Samem.h"
#include "Satypes.h"
#include "Samain.h"
#include "Saload.h"
#include "Sauisig.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Samain.h"

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/* \struct SlideInstanceData Used to hold information between states */
typedef struct
{
    SiaStateMachine         callingFsm;     /*<! The calling FSM */
    int                     returnSig;      /*<! The return signal */
    const SiaObjectInfo     *objectInfo;    /*<! The object to load, 
                                                    do NOT free */
    WE_UINT32              handle;         /*<! Handle */
    char                    *textData;
    SiaAsyncOperationHandle asyncHandle;
    char                    *pipeName;
}LoadInstanceData;

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/
/*! Supported character sets for the MEA */
static LoadInstanceData loadInstance;
static const SiaCharsetItem supportedCharsets[] = SIS_CFG_SUPPORTED_CHARSET;
/******************************************************************************
 * Function prototypes
 *****************************************************************************/
static void loadSigHandler(SiaSignal *sig);
static void freeLoadInstanceData(LoadInstanceData *instance);
static void loadBodyPartToHandle(LoadInstanceData *instData);
static void sendLoadBpToHandleRsp(SiaLoadResult result, WE_UINT32 handle);
static WE_BOOL doLoadTextObject(const char *pipeName);
static WE_UINT32 getCharset(const MmsAllParams *paramList);
static WE_BOOL handleDataFromPipeRsp(SiaAsioResult result, WE_UINT32 size);

/*!
 *\brief Initiates the load object handler
 *
 *****************************************************************************/
void smaLoadInit(void)
{
    smaSignalRegisterDst(SIA_FSM_LOAD, loadSigHandler);
    memset(&loadInstance, 0x00, sizeof(LoadInstanceData));
} /* smaLoadInit */


/*!
 *\brief Terminates the content routing handler
 *
 *****************************************************************************/
void smaLoadTerminate(void)
{
    /* Remove any instance data */
    freeLoadInstanceData(&loadInstance);
    smaSignalDeregister(SIA_FSM_LOAD);
} /* smaLoadTerminate */


/*!
 *\brief Free slide instance data
 *****************************************************************************/
static void freeLoadInstanceData(LoadInstanceData *instance)
{
    if (instance == NULL)
    {
        return;
    } /* if */
    if (instance->pipeName != NULL)
    {
        SIA_FREE(instance->pipeName);
    }
    memset(instance, 0x00, sizeof(LoadInstanceData));
} /* freeSlideInstanceData */

/*!
 *\brief Signal-handler for the content routing FSM.
 *
 * \param sig The signal received.
 *****************************************************************************/
static void loadSigHandler(SiaSignal *sig)
{		
    if (NULL == sig)
    {
        /* no signal - just return */
        return;
    } /* if */

    switch (sig->type)
    {
    case SIA_SIG_LOAD_GET_MMS_BP_RSP:
        /* the response from mms get bp */
        loadBodyPartToHandle(&loadInstance);
        break;
    case SIA_SIG_LOAD_GET_DATA_FROM_PIPE_RSP:
        /* create resource from buffer */
        if (!handleDataFromPipeRsp((SiaAsioResult)sig->u_param1, 
                (WE_UINT32)sig->u_param2))
        {
            /* failed to load media object */
            sendLoadBpToHandleRsp(SIA_LOAD_RESULT_OK, loadInstance.handle);
            freeLoadInstanceData(&loadInstance);
        } /* if */
        break;
    case SIA_SIG_LOAD_RESOURCE_READY:
        /* resource loaded - now we're done! */
        (void)smaDeleteNotification(loadInstance.handle);
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_OK, loadInstance.handle);
        freeLoadInstanceData(&loadInstance);
        break;
    case SIA_SIG_LOAD_RESOURCE_FAILED:
        (void)smaDeleteNotification(loadInstance.handle);
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_ERROR, 0);
        freeLoadInstanceData(&loadInstance);
        break;
    default:
        /* unknown signal */
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA,
            "(%s) (%d) Unknown signal!\n", __FILE__, __LINE__));
        break;
    } /* switch */

    /* Deallocate the signal */
    smaSignalDelete(sig);
} /* loadSigHandler */



static WE_BOOL handleDataFromPipeRsp(SiaAsioResult result, WE_UINT32 size)
{   
    char *tmpBuffer;
    WE_UINT32 destSize;
    WE_UINT32 charset;
    
    smaAsyncOperationStop(&loadInstance.asyncHandle);
    loadInstance.asyncHandle = 0;
    
    if (result != SIA_ASIO_RESULT_OK) 
    {
        return FALSE;
    } /* if */
    
    charset = getCharset(loadInstance.objectInfo->obj.contentType.params);
    
    if (charset == WE_CHARSET_UTF_8)
    {
        smaHandleUtf8Bom(loadInstance.textData);
    }
    else if (charset != WE_CHARSET_ASCII)
    {
        /* do not convert if already utf-8 or ascii */
        tmpBuffer = convertToUTF8(charset, loadInstance.textData, 
            &size, &destSize);
        if (tmpBuffer)
        {
            SIA_FREE(loadInstance.textData);
            loadInstance.textData = tmpBuffer;
        } /* if */
    } /* if */

    loadInstance.handle = smaCreateString(loadInstance.textData);
    
    SIA_FREE(loadInstance.textData);
    loadInstance.textData = NULL;
    
    if (loadInstance.handle == 0)
    {
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_ERROR, loadInstance.handle);
    }
    else
    {
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_OK, loadInstance.handle);
    }
    freeLoadInstanceData(&loadInstance);    

    return TRUE;
}


/*!
 * \brief Checks if the character type can be displayed.
 *
 * \param charset The #MmsCharset character type.
 * \return a charset if the character can be used, otherwise WE_CHARSET_UNKNOWN
 *****************************************************************************/
WE_UINT32 smaIsValidCharset(MmsCharset charset)
{
    const SiaCharsetItem *current = supportedCharsets;
    unsigned int i;
    /* Check if it is possible to do display the charset type*/
    for (i = 0; current[i].msfCharset != MMS_UNKNOWN_CHARSET; i++)
    {
        if (current[i].mmsCharset == (WE_UINT32)charset)
        {
            return current[i].msfCharset;
        }
    }
    return WE_CHARSET_UNKNOWN;
}


static WE_UINT32 getCharset(const MmsAllParams *paramList)
{
    unsigned int tmpCharset;

    /* First, try to find a charset param */
    while (paramList)
    {
        if ((MMS_CHARSET == paramList->param) &&
            (MMS_PARAM_INTEGER == paramList->type) &&
            (WE_CHARSET_UNKNOWN != (tmpCharset = 
            smaIsValidCharset((MmsCharset)paramList->value.integer))))
        {
            /* We found a charset param! Must be text, then.. */
            return (WE_UINT32)tmpCharset;
        }
        /* Keep on looping.. */
        paramList = paramList->next;        
    }

    /* If unsuccessful at finding charset, assume these are text anyway */
    return WE_CHARSET_ASCII;
}


/*! \brief Handles the response signal from the current "get body-part" 
 *         operation.
 *
 * \param bodyPart The info about the current body-part, see 
 *                 #MmsReplyGetBodyPart.
 *****************************************************************************/
void smaLoadHandleGetBpRsp(const MmsGetBodyPartReply *bodyPart)
{
    if (bodyPart == NULL || bodyPart->pipeName == NULL)
    {
        /* not successful */
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_ERROR, 0);
        freeLoadInstanceData(&loadInstance);
    } 
    else
    {
        /* store pipe name in instance */
        loadInstance.pipeName = we_cmmn_strdup(WE_MODID_SIA, 
                (char *)bodyPart->pipeName);

        /* Send the response to the correct FSM */
        (void)SIA_SIGNAL_SENDTO(SIA_FSM_LOAD, SIA_SIG_LOAD_GET_MMS_BP_RSP);
    } /* if */
} /* smaLoadHandleGetBpRsp */


static void loadBodyPartToHandle(LoadInstanceData *instData)
{
    WeCreateData createData;
    const char *mimeType;
    SlsMediaObjectType objType;

    memset(&createData, 0x00, sizeof(WeCreateData));
    
    mimeType = (const char *)instData->objectInfo->obj.contentType.strValue;
    

   /* got the pipe name - create resource and wait 
    for response signal */
    objType = smaGetSlsMediaObjectType(mimeType);

    switch (objType)
    {
    case SIS_OBJECT_TYPE_TEXT:  /* Text object */
        /* load object to buffer */
        /* load from pipe to buffer - SIA_SIG_LOAD_GET_DATA_FROM_PIPE_RSP*/
        if (!doLoadTextObject(instData->pipeName))
        {
            sendLoadBpToHandleRsp(SIA_LOAD_RESULT_ERROR, instData->handle);
            freeLoadInstanceData(&loadInstance);
        }
        break;
    case SIS_OBJECT_TYPE_IMAGE: /* Image object */
    case SIS_OBJECT_TYPE_VIDEO: /* Video object */
    case SIS_OBJECT_TYPE_AUDIO: /* Audio object */
        /* create handle from resource */
        /* set pipe name */
        createData.resource = instData->pipeName;
        if (objType != SIS_OBJECT_TYPE_AUDIO)
        {
            instData->handle = WE_WIDGET_IMAGE_CREATE(WE_MODID_SIA, 
                &createData, mimeType, WeResourcePipe, 0);
        }
        else
        {
            instData->handle = WE_WIDGET_SOUND_CREATE(WE_MODID_SIA, 
                &createData, mimeType, WeResourcePipe);
        }
        
     
        if (instData->handle == 0)
        {
            sendLoadBpToHandleRsp(SIA_LOAD_RESULT_ERROR, instData->handle);
            freeLoadInstanceData(instData);
        }
        else
        {
            /* handle created, register */
            (void)smaRegisterNotification(instData->handle, 
                WeNotifyResourceReady, NULL, SIA_FSM_LOAD, 
                SIA_SIG_LOAD_RESOURCE_READY);
        
            (void)smaRegisterNotification(instData->handle, 
                WeNotifyResourceFailed, NULL, SIA_FSM_LOAD, 
                SIA_SIG_LOAD_RESOURCE_FAILED);

        }
        break;
    case SIS_OBJECT_TYPE_UNKNOWN:
    case SIS_OBJECT_TYPE_ATTACMENT:
    case SIS_OBJECT_TYPE_DRM:
    case SIS_OBJECT_TYPE_REF:
    default:
        /* unknown type */
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_ERROR, 0);
        freeLoadInstanceData(instData);
        break;
    } /* switch */
    
    if (instData->pipeName == NULL)
    {
        SIA_FREE(instData->pipeName);
        instData->pipeName = NULL;
    }
} /* loadBodyPartToHandle */

static WE_BOOL doLoadTextObject(const char *pipeName)
{
    WE_UINT32 size;

    if (pipeName == NULL)
    {
        return FALSE;
    } /* if */
    
    loadInstance.asyncHandle = 
        smaAsyncPipeOperationStart((const char *)pipeName, 
                                   SIA_FSM_LOAD, 
                                   0, 
                                   SIA_SIG_LOAD_GET_DATA_FROM_PIPE_RSP, 
                                   SIA_ASYNC_MODE_READ);

    size = loadInstance.objectInfo->obj.size;
    loadInstance.textData = SIA_ALLOC(size + 1);
    memset(loadInstance.textData + size, 0x00, 1);

    if (!loadInstance.textData) 
    {
        smaAsyncOperationStop(&loadInstance.asyncHandle);
        return FALSE;
    }
    else
    {    
        smaAsyncPipeRead(loadInstance.asyncHandle, 
                         loadInstance.textData, 
                         size);
    }
    
    
    return TRUE;
} /* doLoadTextObject */

void smaLoadBpToHandle(SiaStateMachine retFsm, int retSig, WE_UINT32 msgId, 
     const SiaObjectInfo *objInfo)
{
    /* store instance data */
    loadInstance.callingFsm = retFsm;
    loadInstance.returnSig = retSig;
    loadInstance.objectInfo = objInfo;

    if ((objInfo == NULL) || 
        !smaIsSupportedMediaObject((const char *)
        objInfo->obj.contentType.strValue))
    {
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_ERROR, 0);
    }
    else if (objInfo->handle == 0)
    {
        /* Get body part from mms */
        MMSif_getBodyPart(WE_MODID_SIA, (MmsMsgId)msgId, 
            (WE_UINT16)objInfo->obj.id, SIA_GET_BP_RSP_DEST_LOAD);
    }
    else
    {
        /* object already loaded */
        sendLoadBpToHandleRsp(SIA_LOAD_RESULT_OK, objInfo->handle);
    }

    
} /* smaLoadBpToHandle */


static void sendLoadBpToHandleRsp(SiaLoadResult result, WE_UINT32 handle)
{
    (void)SIA_SIGNAL_SENDTO_IU(loadInstance.callingFsm, 
        loadInstance.returnSig, (WE_INT16)result, handle);
}  /* sendLoadBpToHandleRsp */


/*!
 * \brief Gets the filename information from a MmsAllParams
 * 
 * \param param The parameters holding the filename
 *
 * \return the file name is found, else NULL
 *****************************************************************************/
const char *smaGetFileNameParam(const MmsAllParams *param)
{
	while (param != NULL)
    {
        if ((param->type == MMS_PARAM_STRING) && (param->param == MMS_FILENAME))
        {
			if ( param->value.string != NULL)
			{
				return (const char *)param->value.string;
			} /* if */
		} /* if */
        param = param->next;
    } /* while */    

	return NULL;
} /* getFileName */


/*!
 * \brief Gets the name information from a MmsAllParams
 * 
 * \param param The parameters holding the filename
 *
 * \return the file name is found, else NULL
 *****************************************************************************/
const char *smaGetNameParam(const MmsAllParams *param)
{
	while (param != NULL)
    {
        if ((param->type == MMS_PARAM_STRING) && (param->param == MMS_NAME))
        {
			if ( param->value.string != NULL)
			{
				return (const char *)param->value.string;
			} /* if */
		} /* if */
        param = param->next;
    } /* while */    

	return NULL;
} /* smaGetNameParam */

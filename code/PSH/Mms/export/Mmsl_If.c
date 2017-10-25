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

/*lint -e818 Skip const declaration advice */

/*! \file capimms.c
* \brief  Exported connector functions from the MMS Client.
*/

#include "We_Lib.h"    /* WE: ANSI C liwbary includes */
#include "We_File.h"   /* WE: FILE functions */
#include "We_Log.h"    /* WE: Signal logging */
#include "We_Mem.h"    /* WE: Memory hadling */
#include "We_Dcvt.h"    /* WE: Memory hadling */

#include "Mms_Cfg.h"    /* MMS: Configuration */
#include "Mms_Def.h"    /* MMS: Exported types in adapter/connector functions */
#include "Mutils.h"
#include "Mltypes.h"    /* MMS LIB TYPES */
#include "Mlcreate.h"   /* MMS: Functions for message creation */
#include "Mlpduc.h"     /* MMS: Functions for message creation */
#include "Mlpdup.h"     /* MMS: Functions for message parse */
#include "Mlfetch.h"    /* MMS: Functions for reading message */
#include "Mms_Cfg.h"    /* MMS: Exported MMS Service */
#include "Mms_If.h"     /* MMS: Exported MMS Service */
#include "Mmsl_If.h"    /* MMS: Exported MMS Service libary */
#include "Msig.h"
#include "Fldmgr.h"     /* MMS: folder manager */

#ifdef MMS_UBS_IN_USE
#include "Ubs_If.h"     /* UBS: UBS interface */
#endif
#ifdef WE_LOG_FC
#include "C_Mms_Script.h" /* MMS: Log functions for scripting */
#endif

#ifdef MMS_CUSTOMDATA_IN_USE
MmsCustomDataSpec g_mmsFldrCustomDataSpec[] = MMS_CUSTOM_DATA_SPEC;
#endif
/*! \brief Look-up of custom data position.
*
* \param index Index in spec of custom data (if (MMS_CUSTOM_DATA_UBS_KEY) UBS_key is used)
* \param ubsKey Property-key used if index is MMS_CUSTOM_DÁTA_UBS_KEY.
* \param spec (OUT) The custom-data-spec found, (NULL at failure)
* \return FALSE if not found
*/
#ifdef MMS_CUSTOMDATA_IN_USE

static WE_BOOL MMSlib_getCustomDataSpec(MmsCustomDataId wid, int ubsKey, MmsCustomDataSpec **spec)
{
	int i;

	if (wid != MMS_CUSTOM_DATA_UBS_KEY)
	{
		/*
		*	Calculate new index.
		*/
		i = 0;
		while (g_mmsFldrCustomDataSpec[i].dataLen > 0 &&
			g_mmsFldrCustomDataSpec[i].ubsKey != ubsKey
			)
		{
			i++;
		}
		if (g_mmsFldrCustomDataSpec[i].dataLen <= 0)
		{
			(*spec)= NULL;
			return FALSE;
		}
		else
		{
			(*spec) = &(g_mmsFldrCustomDataSpec[i]);
			return TRUE;
		}
	}
	else
	{
		i = 0;
		while (g_mmsFldrCustomDataSpec[i].dataLen > 0 &&
			g_mmsFldrCustomDataSpec[i].wid != wid
			)
		{
			i++;
		}
		if (g_mmsFldrCustomDataSpec[i].dataLen <= 0)
		{
			(*spec)= NULL;
			return FALSE;
		}
		else
		{
			(*spec) = &(g_mmsFldrCustomDataSpec[i]);
			return TRUE;
		}
	}
} /* MMSlib_getCustomDataSpec */

/*
* \brief Fetches data value from customdata
*
* \param module Module requesting get
* \param wid Custom data wid (if MMS_CUSTOM_DATA_UBS_KEY ubsKey is used)
* \param ubsKey Used if wid is set to MMS_CUSTOM_DATA_UBS_KEY
* \param msgInfo This structure conatains the source data.
* \param data This is the resulting data, requesting module must free data. (and cast)
* \param dataLen Binary size of "data" returned.
* \param valueType Type of the data returned (caller may check this before cast)
*/
WE_BOOL MMSlib_getCustomData(WE_UINT8 module,
							  MmsCustomDataId wid, int ubsKey, MmsMessageFolderInfo *msgInfo,
							  void **data, int *dataLen, MmsCustomDataType *valueType)
{
	MmsCustomDataSpec *spec = NULL;

	if (FALSE == MMSlib_getCustomDataSpec(wid, ubsKey, &spec))
	{
		/*
		*	Couldn't find custom data.
		*/
		(*data) = NULL;
		(*dataLen) = 0;
		(*valueType) = MMS_VALUE_TYPE_UNKNOWN;
		return FALSE;
	}
	else
	{
		if (spec == NULL || spec->offset < 0)
		{
			/*
			*	Couldn't find custom data.
			*/
			(*data) = NULL;
			(*dataLen) = 0;
			(*valueType) = MMS_VALUE_TYPE_UNKNOWN;
			return FALSE;
		}

		switch (spec->valueType)
		{
		case MMS_VALUE_TYPE_UINT32:
			(*dataLen) = sizeof(WE_INT32);
			(*data) = WE_MEM_ALLOC(module, spec->dataLen);
			if (*data == NULL)
			{
				return FALSE;
			}
			*((WE_UINT32*)(*data)) = (WE_UINT32) (
				msgInfo->customData[spec->offset] |
				msgInfo->customData[spec->offset+1] << 8 |
				msgInfo->customData[spec->offset+2] << 16 |
				msgInfo->customData[spec->offset+3] << 24);
			break;
		case MMS_VALUE_TYPE_INT32:
			(*dataLen) = sizeof(WE_INT32);
			(*data) = WE_MEM_ALLOC(module, spec->dataLen);
			if (*data == NULL)
			{
				return FALSE;
			}
			*((WE_INT32*)(*data)) =  (WE_INT32) (
				msgInfo->customData[spec->offset] |
				msgInfo->customData[spec->offset+1] << 8 |
				msgInfo->customData[spec->offset+2] << 16 |
				msgInfo->customData[spec->offset+3] << 24);
			break;
		case MMS_VALUE_TYPE_BOOL:
			(*dataLen) = sizeof(WE_BOOL);
			(*data) = WE_MEM_ALLOC(module, sizeof(WE_BOOL));
			if (*data == NULL)
			{
				return FALSE;
			}
			*((WE_BOOL*)(*data)) = *((WE_BOOL*)(msgInfo->customData+spec->offset));
			break;
		case MMS_VALUE_TYPE_BINARY:
		case MMS_VALUE_TYPE_UTF8:
			(*dataLen) = spec->dataLen;
			(*data) = WE_MEM_ALLOC(module, spec->dataLen);
			if (*data == NULL)
			{
				return FALSE;
			}
			memcpy(*data, msgInfo->customData+spec->offset, spec->dataLen);
			break;
		case MMS_VALUE_TYPE_UNKNOWN:                
		default:
			/*
			*	Unsupported type.
			*/
			(*data) = NULL;
			(*dataLen) = 0;
			(*valueType) = MMS_VALUE_TYPE_UNKNOWN;
			return FALSE;
			break;
		}

		(*valueType) = spec->valueType;

		return TRUE;
	}
} /* MMSlib_getCustomData */    

/*
* \brief Fetches data value from customdata
*
* \param msgInfo This structure conatains the destination data.
* \param data This is the source data,
* \param dataLen Binary size of "data" provided.
* \param spec Which custom data to set.
*/
WE_BOOL mmsSetCustomData(MmsMessageFolderInfo *msgInfo,
						  void *data, int dataLen, MmsCustomDataSpec *spec)
{
	if (spec == NULL ||             /* Faulty spec */
		spec->offset < 0 ||         /* Faulty spec definition */
		spec->dataLen < dataLen)    /* Data too large */
	{
		/*
		*	Couldn't set custom data.
		*/
		return FALSE;
	}

	switch (spec->valueType)
	{
	case MMS_VALUE_TYPE_INT32:
	case MMS_VALUE_TYPE_UINT32:
		msgInfo->customData[spec->offset] = (WE_UINT8) ((*((WE_UINT32*)data)) && 255);
		msgInfo->customData[spec->offset+1] = (WE_UINT8) (((*((WE_UINT32*)data)) >> 8) && 255);
		msgInfo->customData[spec->offset+2] = (WE_UINT8) (((*((WE_UINT32*)data)) >> 16) && 255);
		msgInfo->customData[spec->offset+3] = (WE_UINT8) (((*((WE_UINT32*)data)) >> 24) && 255);
		break;
	case MMS_VALUE_TYPE_BOOL:
		msgInfo->customData[spec->offset] = (WE_UINT8) (*((WE_BOOL*)data));
		break;
	case MMS_VALUE_TYPE_BINARY:
	case MMS_VALUE_TYPE_UTF8:
		memset(msgInfo->customData+spec->offset, 0, spec->dataLen);
		memcpy(msgInfo->customData+spec->offset, data, dataLen);
		break;
	case MMS_VALUE_TYPE_UNKNOWN:                
	default:
		/*
		*	Unsupported type.
		*/
		return FALSE;
		break;
	}

	return TRUE;
} /* mmsSetCustomData */    

MmsResult MMSlib_SetCustomData(WE_UINT8 module)
{
	return MMS_RESULT_ERROR;
} /* MMSlib_SetCustomData */

#endif

/*
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_getMessageHeader(WE_UINT8 modId,
								  MmsGetHeader *header, MmsBodyInfoList *bodyInfoList,
								  MmsMsgHandle *msg, MmsTransactionHandle *transaction)
{
	MmsResult result = MMS_RESULT_ERROR;
	MmsTransaction *tHandle = NULL;
	MmsMessage *mHandle = NULL;

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_getMessageHeader)
		WE_LOG_FC_UINT8( modId, NULL);
	WE_LOG_FC_PRE_IMPL
#endif 

		if ( msg == NULL || transaction == NULL || 
			*transaction == NULL)
		{
			/* not all params supplied */
			return MMS_RESULT_ERROR;
		} /* if */

		tHandle = (MmsTransaction *) *transaction;
		mHandle = (MmsMessage *) *msg;

		switch (tHandle->state)
		{
		case 1:
			/* getInfoFile */
			result = loadMessageInfoFile( modId, mHandle, tHandle);
			if (result != MMS_RESULT_OK)
			{
				break;
			} /* if */

			tHandle->state = 2;     /* next state */
			tHandle->subState = 1;  /* first substate (default value) */
			/*lint -fallthrough */
		case 2:
			/* get MMS header */
			result = getMmsHeader( modId, header, mHandle, tHandle);
			if ( result == MMS_RESULT_OK)
			{
				/* operation completed successfully */
				tHandle->state = 3;     /* next state */
				tHandle->subState = 1;  /* first substate */
				/* fallthroug to next state */
			}
			else if ( result == MMS_RESULT_DELAYED)
			{
				/* Operation was delayed */
				tHandle->state = 2; /* the same state must be called */
				break;  /* break and return */
			} 
			else
			{
				/* something went wrong - free header and return */
				freeMmsGetHeader( modId, header);
				break; /* break and return */
			} /* if */
			/*lint -fallthrough*/
		case 3:
			result = getMessageInfo( modId, header, bodyInfoList, mHandle, tHandle);
			if (result != MMS_RESULT_OK && result != MMS_RESULT_DELAYED)
			{
				/* something went wrong - free memory */
				freeMmsGetHeader( modId, header);
			} /* if */
			break;
		}
		/* we are done now */
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
			"MMSlib_getMessage(msgId = %d)\n", mHandle->msgId));

#ifdef WE_LOG_FC
		logMmsGetHeader( WE_LOG_FC_PARAMS_IMPLEMENT, header, bodyInfoList);
		WE_LOG_FC_INT ( result, NULL);    
		WE_LOG_FC_STRING( header->replyChargingId, NULL);
		WE_LOG_FC_END
#endif

			return result;     
} /* MMSlib_getMessageHeader */


/*
* Get a notification message.
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
int MMSlib_getFileHandle( WE_UINT8 modId, MmsTransactionHandle *transaction)
{
	MmsTransaction *tHandle = NULL;

	tHandle = (MmsTransaction *) *transaction;

	WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
		"MMSlib_getFileHandle(fileHandle = %d)\n", tHandle->fileHandle));
	return tHandle->fileHandle;
} /* MMSlib_getFileHandle */


/*
* Get a notification message.
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_getNotification( WE_UINT8 modId, MmsNotification *msgNotif,
								 MmsMsgHandle *msg, MmsTransactionHandle *transaction)
{
	MmsResult result = MMS_RESULT_OK;
	MmsTransaction *tHandle = NULL;
	MmsMessage *mHandle = NULL;
	MmsVersion ver;

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_getNotification)
		WE_LOG_FC_UINT8( modId, NULL);
	WE_LOG_FC_PRE_IMPL
#endif 

		tHandle = (MmsTransaction *) *transaction;
	mHandle = (MmsMessage *) *msg;
	switch ( tHandle->state )
	{
	case 1:
		/* get file size */
		tHandle->sizeOfBuf = WE_FILE_GETSIZE ( mHandle->fileName);
		if ( tHandle->sizeOfBuf <= 0)
		{
			/* file error */
			return MMS_RESULT_FILE_READ_ERROR;
		} /* if */
		tHandle->buf = WE_MEM_ALLOC( modId, (WE_UINT32)tHandle->sizeOfBuf);
		if (tHandle->buf == NULL)
		{
			WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
				"Unable to allocate memory in: MMSlib_getNotification(msgId = %d)\n", mHandle->msgId));
			return MMS_RESULT_INSUFFICIENT_MEMORY;
		} /* if */
		
		tHandle->bufferPos = tHandle->buf;
		tHandle->bytesLeft = (WE_UINT32)tHandle->sizeOfBuf;
		tHandle->fileHandle = mHandle->fileHandle;
		tHandle->isMessageFile = TRUE;
		tHandle->filePos = 0; /* beginning of file */
		tHandle->state = 2;  
		tHandle->subState = 1;
		/*lint -fallthrough*/
	case 2:
		result = readDataFromFile( modId, 
			(unsigned char **)&tHandle->bufferPos, &tHandle->bytesLeft, 
			tHandle->fileHandle, &tHandle->filePos);
		if ( result != MMS_RESULT_OK )
		{
			/* not complete or error */
			return result;
		} /* if */
		tHandle->state = 3;
		tHandle->bufferPos = tHandle->buf;
		/*lint -fallthrough*/
	case 3:
		/* parse the notification */
		result = parseMmsNotification( modId, tHandle->bufferPos,
			(WE_UINT32)tHandle->sizeOfBuf, msgNotif, &ver);
		if ( result != MMS_RESULT_OK)
		{
			freeMmsNotification( modId, msgNotif);
		} /* if */

		/* done */
		break;
	} /* switch */

#ifdef WE_LOG_FC
	logMmsNotification( WE_LOG_FC_PARAMS_IMPLEMENT, msgNotif);
	WE_LOG_FC_INT ( result, NULL);    
	WE_LOG_FC_END
#endif

		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
		"MMSlib_getNotification(msgId = %d)\n", mHandle->msgId));
	return result;     
} /* MMSlib_getNotification */

/*
* Get a attachment from a message body
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_getBodyPart(WE_UINT8 modId, WE_UINT32 number, 
							 unsigned char *data, WE_UINT32 *size, 
							 MmsMsgHandle *msg, MmsTransactionHandle *transaction)
{
	MmsResult result = MMS_RESULT_OK;
	MmsTransaction *tHandle = NULL;
	MmsMessage *mHandle = NULL;

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_getBodyPart)
		WE_LOG_FC_UINT8( modId, NULL);
	WE_LOG_FC_UINT32( number, NULL);
	WE_LOG_FC_PRE_IMPL
#endif /* WE_LOG_FC */

		tHandle = (MmsTransaction *) *transaction;
	mHandle = (MmsMessage *) *msg;

	switch ( tHandle->state)
	{
	case 1:
		/* prepare file read */
		tHandle->filePos = (long)getDataStart( number, mHandle);
		tHandle->counter = (int)getDataSize( number, mHandle);

		/*lint -fallthrough*/
	case 2:
		/* initialize read */
		tHandle->bufferPos = data;

		if ( (WE_UINT32) tHandle->counter < *size)
		{
			tHandle->bytesLeft = (WE_UINT32)tHandle->counter;
			*size = (WE_UINT32) tHandle->counter;
		}
		else
		{
			tHandle->bytesLeft = *size;
		}/* if */

		tHandle->fileHandle = mHandle->fileHandle;
		tHandle->isMessageFile = TRUE;
		tHandle->state = 3;
		/*lint -fallthrough*/
	case 3:
		/* Warning: This function returns the actual read bytes in bytesLeft... */
		result = readDataFromFile( modId, 
			(unsigned char **)&tHandle->bufferPos, 
			&tHandle->bytesLeft, tHandle->fileHandle, &tHandle->filePos);

		tHandle->counter -= (int)tHandle->bytesLeft;
		/* This is used next round reading */
		tHandle->bytesLeft = (WE_UINT32)tHandle->counter;
		if ( (result == MMS_RESULT_OK) && (tHandle->counter == 0))
		{
			/* all comlete */
			result = MMS_RESULT_OK;
			tHandle->state = 1;
		}
		else if ( (result == MMS_RESULT_OK) && (tHandle->counter > 0))
		{
			/* buffer is full */
			result = MMS_RESULT_BUFFER_FULL;
			tHandle->state = 2;
		} /* if */
		/* else result = MMS_RESULT_DELAYED by default */
		break;            
	default :
		
		break;
	} /* switch */

#ifdef WE_LOG_FC
	WE_LOG_FC_BYTES( data, *size, NULL);
	WE_LOG_FC_UINT32( *size, NULL);
	WE_LOG_FC_INT( result, NULL);
	WE_LOG_FC_END
#endif
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
		"MMSlib_getBodyPart( %d, %d)\n", number, mHandle->msgId));
	return result;
} /* MMSlib_getBodyPart */


/*
* Set a value to a part of the message body
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_setMessageBody (WE_UINT8 modId, MmsBodyParams *params,
								 unsigned char *data, WE_UINT32 dataSize, MmsMsgHandle *msg, 
								 MmsTransactionHandle *transaction)
{
	MmsResult result = MMS_RESULT_OK;
	MmsTransaction *tHandle = NULL;
	MmsMessage *mHandle = NULL;
	WE_UINT32 encHeaderLen = 0;
	unsigned char *encHeader = NULL;   /* buffer to hold the encoded "header" */

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_setMessageBody)
		WE_LOG_FC_UINT8( modId, NULL);    
	logMmsBodyParams( WE_LOG_FC_PARAMS_IMPLEMENT, params);
	WE_LOG_FC_UINT32( dataSize, NULL);
	WE_LOG_FC_BYTES( data, dataSize, NULL);
	WE_LOG_FC_PRE_IMPL
#endif



		tHandle = (MmsTransaction *) *transaction;
	mHandle = (MmsMessage *) *msg;

	switch ( tHandle->state)
	{
	case 1:
		if ( params->targetType != MMS_PLAIN_MESSAGE)
		{
			if ( ( encHeader = createBodyPartHeader( modId, &encHeaderLen, 
				params, dataSize, &result)) == NULL)
			{
				/* unable to create body header */
				return result;
			} /* if */

			tHandle->buf = encHeader;
			tHandle->bufferPos = tHandle->buf;
			tHandle->sizeOfBuf = (int)encHeaderLen;
			tHandle->bytesLeft = encHeaderLen;
			tHandle->filePos = -1;  /* -1 = Append */
			tHandle->state = 2;     /* next state */
		} /* if */
		tHandle->fileHandle = mHandle->fileHandle;
		tHandle->isMessageFile = TRUE;
		/*lint -fallthrough*/
	case 2:
		if ( params->targetType != MMS_PLAIN_MESSAGE)
		{
			/* write body header */
			result = writeDataToFile( modId, 
				(unsigned char **)&tHandle->bufferPos, 
				&tHandle->bytesLeft, tHandle->fileHandle, &tHandle->filePos);
			if ( (result != MMS_RESULT_OK))
			{
				return result;
			}/* if */

			/* body header written - free memory */
			WE_MEM_FREE( modId, tHandle->buf);
			tHandle->buf = NULL;
		} /* if */

		tHandle->bufferPos = data;
		tHandle->bytesLeft = dataSize;
		tHandle->filePos = -1; /* append */
		tHandle->state = 3;
		/*lint -fallthrough*/
	case 3:
		/* write body data */
		result = writeDataToFile( modId, 
			(unsigned char **)&tHandle->bufferPos, 
			&tHandle->bytesLeft, tHandle->fileHandle, &tHandle->filePos);
		if ( result != MMS_RESULT_OK)
		{
#ifdef WE_LOG_FC
			WE_LOG_FC_INT( result, NULL);
			WE_LOG_FC_END
#endif
				return result;
		} /* if */
		break;
	default:
		/* unknown "state" should not be here */
		result = MMS_RESULT_ERROR;
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, "MMSlib_setMessageBody: received unknown state"));
		break;
	} /* switch */

	WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
		"MMSlib_setMessageBody( fileHandle=%d, size=%d, targetType=%d)\n", 
		tHandle->fileHandle, dataSize, params->targetType));

#ifdef WE_LOG_FC
	WE_LOG_FC_INT( result, NULL);
	WE_LOG_FC_END
#endif
		return result;

} /* MMSlib_setMessageBody */


/*
* Set the message header to a value 
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_setMessageHeader(WE_UINT8 modId, WE_UINT32 numOfBodyParts, 
								  MmsSetHeader *header, MmsVersion version, MmsMsgHandle *msg, 
								  MmsTransactionHandle *transaction)
{
	MmsResult result = MMS_RESULT_OK;
	MmsTransaction *tHandle = NULL;
	MmsMessage *mHandle = NULL;
	unsigned char *buffer = NULL;
	WE_UINT32 bufferSize = 0;

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_setMessageHeader)
		WE_LOG_FC_UINT8( modId, NULL);
	WE_LOG_FC_UINT32( numOfBodyParts, NULL);
	WE_LOG_FC_INT( version, NULL);
	logMmsSetHeader( WE_LOG_FC_PARAMS_IMPLEMENT, header);
	WE_LOG_FC_PRE_IMPL
#endif /* WE_LOG_FC */

		tHandle = (MmsTransaction *) *transaction;
	mHandle = (MmsMessage *) *msg;

	switch( tHandle->state)
	{
	case 1:
		/*version = cfgGetInt(MMS_CFG_PROXY_RELAY_VERSION);*/
		if ( ( buffer = createWspSendHeader( modId, header, &bufferSize, &result, 
			version, numOfBodyParts)) == NULL)
		{
			/* error creating header */
			/* leave switch */
			break;
		}
		else 
		{
			tHandle->state = 2;
			tHandle->buf = buffer;
			tHandle->bufferPos = buffer;
			tHandle->bytesLeft = bufferSize;
			tHandle->fileHandle = mHandle->fileHandle;
			tHandle->isMessageFile = TRUE;
			tHandle->filePos = -1;  /* negative value for append */
		}/* if */

		/*lint -fallthrough*/
	case 2:
		/* writing from buffer to file */
		result = writeDataToFile( modId, (unsigned char **)&tHandle->bufferPos, 
			&tHandle->bytesLeft, tHandle->fileHandle, &tHandle->filePos);
		break;
	default:
		/* error!! unknown state */
		result = MMS_RESULT_ERROR;
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
			"MMSlib_setMessageHeader: received unknown state."));
		break;
	}

	if ( result == MMS_RESULT_OK)
	{
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
			"MMSlib_setHeader( wid=%d, numOfBodyParts=%d \n)", mHandle->fileHandle, 
			numOfBodyParts));
	} /* if */

#ifdef WE_LOG_FC
	WE_LOG_FC_INT ( result, NULL);    
	WE_LOG_FC_END
#endif
		return result;
} /* MMSlib_setMessageHeader */




/*
* Set the message header to a value 
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_messageOpen( WE_UINT8 modId, MmsMsgId msgId, 
							 MmsFileType msgType, MmsMsgHandle *msg)
{
	MmsResult result = MMS_RESULT_FILE_READ_ERROR;
	WE_INT32 readResult = 0;
	MmsMessage *mHandle = NULL;


#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_messageOpen)
		WE_LOG_FC_UINT8(modId, NULL); 
	WE_LOG_FC_UINT32(msgId, NULL); 
	WE_LOG_FC_INT(msgType, NULL); 
	WE_LOG_FC_PRE_IMPL
#endif

		if ( msgType != MMS_SUFFIX_NOTIFICATION && 
			msgType != MMS_SUFFIX_IMMEDIATE &&
			msgType != MMS_SUFFIX_SEND_REQ &&
			msgType != MMS_SUFFIX_TEMPLATE &&
			msgType != MMS_SUFFIX_MSG)
		{
			/* illigal message type */
			WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
				"MMSlib_messageOpen: illigal message type."));
			return MMS_RESULT_ERROR;
		} /* if */

		mHandle = WE_MEM_ALLOC( modId, sizeof(MmsMessage));
		if (mHandle == NULL)
		{
			WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
				"Unable to allocate memory in: MMSlib_messageOpen(msgId = %d)\n", msgId));
			return MMS_RESULT_INSUFFICIENT_MEMORY;
		} /* if */
		memset (mHandle, 0x00, sizeof(MmsMessage));

		mHandle->fileName = WE_MEM_ALLOC( modId, MMS_PATH_LEN);
		if (mHandle->fileName == NULL)
		{
			WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
				"Unable to allocate memory in: MMSlib_messageOpen(msgId = %d)\n", msgId));
			WE_MEM_FREE(modId, mHandle);
			return MMS_RESULT_INSUFFICIENT_MEMORY;
		} /* if */
		if (sprintf( mHandle->fileName, "%s%lx.%c", MMS_FOLDER, msgId, msgType) > 
			(int) (MMS_PATH_LEN))
		{
			WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
				"MMSlib_messageOpen(). Error creating filename"));
			return MMS_RESULT_RESTART_NEEDED;
		} /* if */

		readResult = WE_FILE_OPEN( modId, mHandle->fileName, WE_FILE_SET_RDWR, 0);

		if (readResult < 0)
		{
			WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
				"MMSlib_messageOpen(). Failed to open file:"));
			switch ( readResult )
			{
			case WE_FILE_ERROR_ACCESS:
				WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
					"MMSlib_messageOpen(). file access error"));
				break;
			case WE_FILE_ERROR_INVALID:
				WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
					"MMSlib_messageOpen(). fileName contains invalid characters"));
				break;
			case WE_FILE_ERROR_SIZE:
				WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
					"MMSlib_messageOpen(). File data size is too big for the file system"));
				break;
			case WE_FILE_ERROR_FULL :
				WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
					"MMSlib_messageOpen(). File system is full"));
				break;
			case WE_FILE_ERROR_PATH :
				WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
					"MMSlib_messageOpen(). File or path specified by fileName not found"));
				break;
			default:
				WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
					"MMSlib_messageOpen(). Unspecified error"));
				break;
			} /* switch*/
			result = MMS_RESULT_FILE_READ_ERROR;
		}
		else 
		{
			mHandle->fileHandle = readResult;
			result = MMS_RESULT_OK;
		} /* if */

		mHandle->msgId = msgId;
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
			"MmsLib_messageOpen( wid=%d\n)", msgId));

		*msg = (MmsMsgHandle) mHandle;

#ifdef WE_LOG_FC
		WE_LOG_FC_INT( result, NULL);
		WE_LOG_FC_END
#endif

			return result;
} /* MMSlib_openMessage */

/*
* Set the message header to a value 
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_messageClose( WE_UINT8 modId, MmsMsgHandle *msg)
{
	MmsResult result = MMS_RESULT_OK;
	MmsMessage *mHandle = NULL;

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_messageClose)
		WE_LOG_FC_UINT8(modId, NULL); 
	WE_LOG_FC_PRE_IMPL
#endif

		mHandle = (MmsMessage *) *msg;


	if (mHandle == NULL)
	{
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
			"MmsLib_closeMessage( wid= not set!\n)"));
		return MMS_RESULT_OK;
	} /* if */

	WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
		"MmsLib_closeMessage( wid=%d\n)", mHandle->msgId));

	(void) WE_FILE_CLOSE( mHandle->fileHandle);
	mHandle->fileHandle = 0;
	if (mHandle->fileName)
	{
		WE_MEM_FREE( modId, mHandle->fileName);
		mHandle->fileName = NULL;
	} /* if */
	mHandle->msgId = 0;
	if (mHandle->infoHandle.buffer != NULL)
	{
		WE_MEM_FREE( modId, mHandle->infoHandle.buffer);
		mHandle->infoHandle.buffer = NULL;
	} /* if */

	WE_MEM_FREE( modId, mHandle);
	mHandle = NULL;

	*msg = NULL;

#ifdef WE_LOG_FC
	WE_LOG_FC_INT( result, NULL);
	WE_LOG_FC_END
#endif
		return result;
} /* MMSlib_closeMessage */

/*
* Set the message header to a value 
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_transactionOpen( WE_UINT8 modId,
								 MmsTransactionHandle *transaction)
{

	MmsResult result = MMS_RESULT_OK;
	MmsTransaction *tHandle = NULL;

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_transactionOpen)
		WE_LOG_FC_UINT8(modId, NULL); 
	WE_LOG_FC_PRE_IMPL
#endif

		tHandle = WE_MEM_ALLOC( modId, sizeof( MmsTransaction));
	if (tHandle == NULL)
	{
		WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
			"Unable to allocate memory in: MMSlib_transactionOpen()\n"));
		return MMS_RESULT_INSUFFICIENT_MEMORY;
	} /* if */
	memset( tHandle, 0, sizeof(MmsTransaction));

	tHandle->state = 1;         /* defaul state */
	tHandle->subState = 1;      /* defaul state */

	*transaction = (MmsTransactionHandle)tHandle;
	WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
		"MMSlib_openTransaction( )"));

#ifdef WE_LOG_FC
	WE_LOG_FC_INT( result, NULL);
	WE_LOG_FC_END
#endif
		return result;

} /* MMSlib_openTransaction */


/*
* Set the message header to a value 
*
* Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
MmsResult MMSlib_transactionClose( WE_UINT8 modId,
								  MmsTransactionHandle *transaction)
{
	MmsResult result = MMS_RESULT_OK;
	MmsTransaction *tHandle = NULL;

#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_transactionClose)
		WE_LOG_FC_UINT8(modId, NULL); 
	WE_LOG_FC_PRE_IMPL
#endif

		tHandle = (MmsTransaction *) *transaction;

	if ( tHandle->buf != NULL)
	{
		WE_MEM_FREE( modId, tHandle->buf);
	} /* if */
	if (tHandle->tmpPtr != NULL)
	{
		WE_MEM_FREE( modId, tHandle->tmpPtr);
	} /* if */

	if ( !tHandle->isMessageFile && tHandle->fileHandle > 0)
	{
		/* we have to close this file. */
		(void) WE_FILE_CLOSE( tHandle->fileHandle);
	} /* if */

	WE_MEM_FREE( modId, tHandle);
	tHandle = NULL;
	*transaction = NULL;
	WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
		"MMSlib_closeTransaction()"));

#ifdef WE_LOG_FC
	WE_LOG_FC_INT( result, NULL);
	WE_LOG_FC_END
#endif

		return result;


} /* MMSlib_closeTransaction */


/*!
* \brief Frees a MmsGetHeader data structure.
*
*  Type: Function call
*
* \param modId In: WE module identification 
* \param mHeader In: The data to free 
*****************************************************************************/
void MMSlib_freeMmsGetHeader( WE_UINT8 modId, MmsGetHeader *mHeader)
{
#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_freeMmsGetHeader)
		WE_LOG_FC_UINT8(modId, NULL); 
	WE_LOG_FC_PRE_IMPL
#endif
		freeMmsGetHeader(modId, mHeader);

#ifdef WE_LOG_FC
	WE_LOG_FC_END
#endif
} /* MMSlib_freeMmsGetHeader */

/*!
* \brief Frees a MmsSetHeader data structure.
*
*  Type: Function call
*
* \param modId In: WE module identification 
* \param mHeader In: The data to free 
*****************************************************************************/
void MMSlib_freeMmsSetHeader( WE_UINT8 modId, MmsSetHeader *mHeader)
{
	freeMmsSetHeader(modId, mHeader);
} /* MMSlib_freeMmsSetHeader */

/*!
* \brief Frees a MmsBodyParams data structure.
*
*  Type: Function call
*
* \param modId In: WE module identification 
* \param bodyParams In: The data to free 
*****************************************************************************/
void MMSlib_freeMmsBodyParams( WE_UINT8 modId, MmsBodyParams *bodyParams)
{
	freeMmsBodyParams( modId, bodyParams);    
}

/*!
* \brief Frees a MmsNotification data structure.
*
*  Type: Function call
*
* \param modId In: WE module identification 
* \param mNotification in: The data to free 
*****************************************************************************/
void MMSlib_freeMmsNotification( WE_UINT8 modId, MmsNotification *mNotification)
{
#ifdef WE_LOG_FC
	WE_LOG_FC_BEGIN(MMSlib_freeMmsNotification)
		WE_LOG_FC_UINT8(modId, NULL); 
	WE_LOG_FC_PRE_IMPL
#endif
		freeMmsNotification(modId, mNotification);

#ifdef WE_LOG_FC
	WE_LOG_FC_END
#endif
} /* MMSlib_freeMmsNotification */

/*!
* \brief Builds a full path from a message wid.
*
*  Type: Function call
*
* \param modId In: WE module identification 
* \param msgId In: The mesesage wid.
* \param fileType In: The file suffix char
* \return the full path or NULL 
*****************************************************************************/
char *MMSlib_getMessageFullPath( WE_UINT8 modId, MmsMsgId msgId, 
								MmsFileType fileType)

{
	char *filePath = NULL;

	filePath = WE_MEM_ALLOC( modId, MMS_PATH_LEN);
	if ( filePath)    
	{
		if (sprintf( filePath, "%s%lx.%c", MMS_FOLDER, msgId, fileType) >
			(int) (MMS_PATH_LEN))
		{
			WE_MEM_FREE( modId, filePath);
			filePath = NULL;
		} /* if */
	} /* if */
	return filePath;
} /* MMSlib_getMessageFullPath */


#ifdef WE_LOG_FC  
WE_LOG_FC_DISPATCH_MAP_BEGIN(mmslib)

static MmsTransactionHandle transaction = 0;
static MmsMsgHandle msg = 0;
static MmsGetHeader mmsGetHeader;
static MmsBodyInfoList mmsBodyInfoList;

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_setMessageHeader)

enum {MAX_PARAMS = 2};
MmsSetHeader mmsHeader;
char strings[19][1024];
MmsAddressList bccList[MAX_PARAMS];
MmsAddressList ccList[MAX_PARAMS];
MmsAddress from;
MmsContentType contentType;
MmsAllParams params[MAX_PARAMS];
MmsAddressList toList[MAX_PARAMS];


memset(&mmsHeader, 0x00, sizeof(mmsHeader));
memset( strings, 0x00, sizeof(strings));

/* BCC */
memset( bccList, 0x00, sizeof(bccList));
bccList[0].current.address = &strings[0][0];
bccList[1].current.address = &strings[1][0];
bccList[0].current.name.text = &strings[2][0];
bccList[1].current.name.text = &strings[3][0];
bccList[0].next = &bccList[1];
mmsHeader.bcc = &bccList[0];

/* CC */
memset( ccList, 0x00, sizeof(ccList));
ccList[0].current.address = &strings[4][0];
ccList[1].current.address = &strings[5][0];
ccList[0].current.name.text = &strings[6][0];
ccList[1].current.name.text = &strings[7][0];
ccList[0].next = &ccList[1];
mmsHeader.cc = &ccList[0];

/* TO */
memset( toList, 0x00, sizeof(toList));
toList[0].current.address = &strings[8][0];
toList[1].current.address = &strings[9][0];
toList[0].current.name.text = &strings[10][0];
toList[1].current.name.text = &strings[11][0];
toList[0].next = &toList[1];
mmsHeader.to = &toList[0];

/* FROM */
memset( &from, 0x00, sizeof(from));
mmsHeader.from.name.text = &strings[12][0];
mmsHeader.from.address = &strings[13][0];

/* ContentType */
memset(&contentType, 0x00, sizeof(contentType));
contentType.strValue = (unsigned char *)&strings[14][0];
mmsHeader.contentType = &contentType;

/* ContentType Params*/
memset( params, 0x00, sizeof(params));
params[0].value.string = (unsigned char *)&strings[15][0];
params[1].value.string = (unsigned char *)&strings[16][0];
contentType.params = &params[0];

/* ReplyChargingID*/
mmsHeader.replyChargingId = &strings[17][0];

/* Subject */
mmsHeader.subject.text = &strings[18][0];

/* Message Class */
mmsHeader.msgClass.textString = &strings[19][0];

MMSlib_setMessageHeader( 0, 0, &mmsHeader, 0, &msg, &transaction);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END


WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_setMessageBody)

MmsResult result = MMS_RESULT_OK;
enum
{   MAX_DATA_SIZE   = 5000000,
MAX_PARAMS      = 2,
};
unsigned char *data;
MmsBodyParams bodyParams;
MmsEntryHeader entryHeaders[MAX_PARAMS];
MmsAllParams params[MAX_PARAMS];
unsigned char strings[10][1024];

data = MMS_ALLOC(MAX_DATA_SIZE);

memset ( data, 0x00, MAX_DATA_SIZE);
memset ( &bodyParams, 0x00, sizeof( MmsBodyParams));
memset ( strings, 0x00, sizeof( strings));


/* entry headers */
memset ( entryHeaders, 0x00, sizeof( entryHeaders));
entryHeaders[0].value.wellKnownFieldName = &strings[0][0];
entryHeaders[1].value.wellKnownFieldName = &strings[1][0];
entryHeaders[0].value.applicationHeader.value = &strings[2][0];
entryHeaders[1].value.applicationHeader.value = &strings[3][0];
entryHeaders[0].next = &entryHeaders[1];
bodyParams.entryHeader = &entryHeaders[0];

/* ContentType */
bodyParams.contentType.strValue = &strings[4][0];

/* ContentType - Params*/
memset( params, 0x00, sizeof(params));
params[0].value.string = (unsigned char *)&strings[5][0];
params[1].value.string = (unsigned char *)&strings[6][0];

/* ContentType - Params next*/
params[0].next = &params[1];
bodyParams.contentType.params = &params[0];
result = MMSlib_setMessageBody( 0, &bodyParams, data, 0, &msg, &transaction);

MMS_FREE(data);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_transactionOpen)
MmsResult result = MMS_RESULT_OK;
result = MMSlib_transactionOpen( 1, &transaction);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_transactionClose)
MmsResult result = MMS_RESULT_OK;
result = MMSlib_transactionClose( 1, &transaction);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_messageOpen)
MmsResult result = MMS_RESULT_OK;
char chr = 's';
result = MMSlib_messageOpen( 1, 1, chr, &msg);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_messageClose)
MmsResult result = MMS_RESULT_OK;
result = MMSlib_messageClose( 1, &msg);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_freeMmsGetHeader)
MMSlib_freeMmsGetHeader( 1, &mmsGetHeader);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END


WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_getMessageHeader)

enum {MAX_PARAMS = 2};
char strings[29][1024];
MmsAddressList bccList[MAX_PARAMS];
MmsAddressList ccList[MAX_PARAMS];
MmsAddress from;
MmsAddressList toList[MAX_PARAMS];
MmsPrevSentBy sentBy[MAX_PARAMS];
MmsPrevSentDate sentDate[MAX_PARAMS];
MmsContentType contentType;
MmsAllParams params[MAX_PARAMS];
MmsBodyInfoList bodyInfoList2;
MmsContentType contentType2;
MmsAllParams params2[MAX_PARAMS];

memset(&mmsGetHeader, 0x00, sizeof(mmsGetHeader));
memset( strings, 0x00, sizeof(strings));

/* BCC */
memset( bccList, 0x00, sizeof(bccList));
bccList[0].current.address = &strings[0][0];
bccList[1].current.address = &strings[1][0];
bccList[0].current.name.text = &strings[2][0];
bccList[1].current.name.text = &strings[3][0];
bccList[0].next = &bccList[1];
mmsGetHeader.bcc = &bccList[0];

/* CC */
memset( ccList, 0x00, sizeof(ccList));
ccList[0].current.address = &strings[4][0];
ccList[1].current.address = &strings[5][0];
ccList[0].current.name.text = &strings[6][0];
ccList[1].current.name.text = &strings[7][0];
ccList[0].next = &ccList[1];
mmsGetHeader.cc = &ccList[0];

/* TO */
memset( toList, 0x00, sizeof(toList));
toList[0].current.address = &strings[8][0];
toList[1].current.address = &strings[9][0];
toList[0].current.name.text = &strings[10][0];
toList[1].current.name.text = &strings[11][0];
toList[0].next = &toList[1];
mmsGetHeader.to = &toList[0];

/* FROM */
memset( &from, 0x00, sizeof(from));
mmsGetHeader.from.name.text = &strings[12][0];
mmsGetHeader.from.address = &strings[13][0];

/* ReplyChargingID*/
mmsGetHeader.replyChargingId = &strings[14][0];

/* Subject */
mmsGetHeader.subject.text = &strings[15][0];

/* Message Class */
mmsGetHeader.msgClass.textString = &strings[16][0];

/* Prev sent by */
memset ( sentBy, 0x00, sizeof(sentBy));
sentBy[0].sentBy.address = &strings[17][0];
sentBy[0].sentBy.name.text = &strings[18][0];
sentBy[0].next = &sentBy[1];
sentBy[1].sentBy.address = &strings[19][0];
sentBy[1].sentBy.name.text = &strings[20][0];
mmsGetHeader.previouslySentBy = &sentBy[0];

/* Prev sent date */
memset ( sentDate, 0x00, sizeof(sentBy));
sentDate[0].next = &sentDate[1];
mmsGetHeader.previouslySentDate = &sentDate[0];

/* ContentType */
memset(&contentType, 0x00, sizeof(contentType));
contentType.strValue = (unsigned char *)&strings[22][0];
mmsGetHeader.contentType = &contentType;

/* ContentType Params*/
memset( params, 0x00, sizeof(params));
params[0].value.string = (unsigned char *)&strings[23][0];
params[1].value.string = (unsigned char *)&strings[24][0];
contentType.params = &params[0];

/* Next bodyInfoList */
memset( &bodyInfoList2, 0x00, sizeof(bodyInfoList2));
mmsBodyInfoList.next = &bodyInfoList2;

/* ContentType2 */
memset(&contentType2, 0x00, sizeof(contentType2));
contentType.strValue = (unsigned char *)&strings[26][0];
bodyInfoList2.contentType = &contentType2;

/* ContentType Params2*/
memset( params2, 0x00, sizeof(params2));
params2[0].value.string = (unsigned char *)&strings[27][0];
params2[1].value.string = (unsigned char *)&strings[28][0];
contentType2.params = &params2[0];
MMSlib_getMessageHeader( 0, &mmsGetHeader, &mmsBodyInfoList, &msg, &transaction);

WE_LOG_FC_DISPATCH_MAP_ENTRY_END


WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_getNotification)

enum {MAX_PARAMS = 2};
MmsNotification mmsNotif;
char strings[19][1024];
MmsAddress from;

memset(&mmsNotif, 0x00, sizeof(mmsNotif));
memset( strings, 0x00, sizeof(strings));


/* FROM */
memset( &from, 0x00, sizeof(from));
mmsNotif.from.name.text = &strings[0][0];
mmsNotif.from.address = &strings[1][0];

/* Subject */
mmsNotif.subject.text = &strings[2][0];

/* ReplyChargingID*/
mmsNotif.replyChargingId = &strings[3][0];

/* content location */
mmsNotif.contentLocation = &strings[4][0];

/* message class */
mmsNotif.msgClass.textString = &strings[5][0];


MMSlib_getNotification( 0, &mmsNotif, &msg, &transaction);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSlib_getBodyPart)

MmsResult result = MMS_RESULT_OK;
enum
{   MAX_DATA_SIZE   = 5000000,
MAX_PARAMS      = 2,
};
unsigned char *data;
WE_UINT32 size = MAX_DATA_SIZE;

data = MMS_ALLOC(size);

memset ( data, 0x00, size);

result = MMSlib_getBodyPart( 0, 0, &data[0], &size, &msg, &transaction);

MMS_FREE(data);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END


WE_LOG_FC_DISPATCH_MAP_END
#endif 

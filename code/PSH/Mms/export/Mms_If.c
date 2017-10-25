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

/*! \file mms_if.c
 * \brief  Exported interface functions from the MMS Service.
 */

/*lint -e818 Skip const declaration advice */

/*--- Include files ---*/
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Core.h"   /* WE: System core call */ 
#include "We_Dcvt.h"   /* WE: Type conversion routines */ 
#include "We_Mem.h"    /* WE: Memory handling */
#include "We_Cfg.h"    /* WE: Module identification */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Errc.h"   /* WE: Error utilities */

#include "Mms_Cfg.h"    /* MMS: Configuration definitions */
#include "Mms_Def.h"    /* MMS: Exported types in adapter/connector functions */
#include "Mms_If.h"     /* MMS: Exported signal interface */
#include "Mmsl_If.h"    /* MMS: Exported MMS Service libary */
#include "Mutils.h"     /* MMS: Exported utility functions */

#include "Mltypes.h"    /* MMS LIB TYPES */
#include "Mlpduc.h"     /* MMS  Creation of forward req */
#include "Mlpdup.h"     /* MMS  Creation of forward req */
#include "Mcpdu.h"      /* MMS: Definitions for MMS PDU */
#include "Mlfieldp.h"   /* MMS: Utility functions parse MMS messages */
#include "Mlfetch.h"    /* MMS: LIB  FETCH */

#ifdef WE_LOG_FC
    #include "C_Mms_Script.h" /* MMS: Log functions for scripting */
    WE_LOG_FC_DISPATCH_MAP_DECLARE(mmslib)
#endif

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/ 

/*--- Static variables ---*/

/*--- Prototypes ---*/
/*****************************************************************************/

/*!
 * \brief Free the internal structure of MmsMessageProperties.
 *
 * \param properties    Structure to be freed.
 * \param module        Memory owner.
 */
void MMSif_freeMmsMessageProperties(MmsMessageProperties *properties,
                                    WE_UINT8 module)
{
    if (properties->rrList != NULL)
    {
        WE_MEM_FREE(module, properties->rrList);
        properties->rrList = NULL;
    }
    if (properties->drList != NULL)
    {
        WE_MEM_FREE(module, properties->drList);
        properties->drList = NULL;
    }
} /* MMSif_freeMmsMessageProperties */


/*!
 * \brief Copy the internal structure of MmsMessageProperties.
 *
 * \param toProperties    Structure to be updated.
 * \param properties      Structure to be copied.
 * \param module          Memory owner.
 */
WE_BOOL MMSif_copyMmsMessageProperties(MmsMessageProperties *toProperties, 
                                        MmsMessageProperties *fromProperties,
                                        WE_UINT8 module)
{
    int i;

    if (fromProperties == NULL || toProperties == NULL)
    {
        return FALSE;
    }

    toProperties->rrListLength = fromProperties->rrListLength;
    toProperties->drListLength = fromProperties->drListLength;

    if (NULL != fromProperties->rrList)
    {
        toProperties->rrList = WE_MEM_ALLOC(module, sizeof(WE_UINT32)*toProperties->rrListLength);
        if (NULL == toProperties->rrList)
        {
            return FALSE;
        }
        for(i=0; toProperties->rrListLength; i++)
        {
            toProperties->rrList[i] = fromProperties->rrList[i];
        }
    }
    else
    {
        toProperties->rrList = NULL;
        toProperties->rrListLength = 0;
    }
    if (NULL != fromProperties->rrList)
    {
        toProperties->drList = WE_MEM_ALLOC(module, sizeof(WE_UINT32)*toProperties->drListLength);
        if (NULL == toProperties->drList)
        {
            if (toProperties->rrList != NULL)
            {
                WE_MEM_FREE(module, toProperties->rrList);
            }
            return FALSE;
        }
        for(i=0; toProperties->rrListLength; i++)
        {
            toProperties->drList[i] = fromProperties->drList[i];
        }
    }
    else
    {
        toProperties->drList = NULL;
        toProperties->drListLength = 0;
    }

    memcpy(&toProperties->msgFolderInfo, &fromProperties->msgFolderInfo, sizeof(MmsMessageFolderInfo));

    return TRUE;
} /* MMSif_copyMmsMessageProperties */

/*!
 * \brief Free the internal structure of MmsMessageProperties.
 *
 * \param properties    Structure to be freed.
 * \param module        Memory owner.
 */
void MMSif_freeMmsListOfProperties(MmsListOfProperties *list,
                                    WE_UINT8 module)
{
    WE_UINT32 i;

    if (NULL != list)
    {
        if (NULL != list->msgPropertyList)
        {
            for(i=0; i<list->noOfElements; i++)
            {
                MMSif_freeMmsMessageProperties(&(list->msgPropertyList[i]), module);
            }
        }
        
        WE_MEM_FREE(module, list->msgPropertyList);
        list->msgPropertyList = NULL;
    }
} /* MMSif_freeMmsMessageProperties */


/*!
 * \brief Copy the internal structure of MmsMessageProperties.
 *
 * \param toProperties    Structure to be updated.
 * \param properties      Structure to be copied.
 * \param module          Memory owner.
 */
WE_BOOL MMSif_copyMmsListOfProperties(MmsListOfProperties *toList, 
                                        MmsListOfProperties *fromList,
                                        WE_UINT8 module)
{
    WE_UINT32 i;

    if (fromList == NULL || toList == NULL)
    {
        return FALSE;
    }

    toList->noOfElements = fromList->noOfElements;

    if (toList->noOfElements > 0)
    {
        toList->msgPropertyList = WE_MEM_ALLOC(module, sizeof(MmsMessageProperties)*toList->noOfElements);
        if (NULL == toList->msgPropertyList)
        {
            return FALSE;
        }
        
        for(i=0; i<toList->noOfElements; i++)
        {
            if (!MMSif_copyMmsMessageProperties(&(toList->msgPropertyList[i]), &(fromList->msgPropertyList[i]), module))
            {
                WE_MEM_FREE(module, toList->msgPropertyList);
                toList->msgPropertyList = NULL;
                return FALSE;
            }
        }
    }
    else
    {
        toList->msgPropertyList = NULL;
    }

    return TRUE;
} /* MMSif_copyMmsListOfProperties */

/*!
 * \brief Frees a MmsContentType data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param ct In: The data to free 
 * \param freeRoot In: Shall the base structure be freed?
 *****************************************************************************/
void MMSif_freeMmsContentType( WE_UINT8 modId, MmsContentType *ct, WE_BOOL freeRoot)
{
    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSif_freeMmsContentType)
        WE_LOG_FC_UINT8(modId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
    freeMmsContentType(modId, ct);
    if (freeRoot == TRUE)
    {
        WE_MEM_FREE(modId, ct);
    }

    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif
} /* MMSif_freeMmsContentType */

/*!
 * \brief Frees a MmsContentType data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param toCT: Data structure to fill-in 
 * \param fromCT: Data structure to copy 
*****************************************************************************/
WE_BOOL MMSif_copyMmsContentType( WE_UINT8 modId, MmsContentType *toCT, MmsContentType *fromCT)
{
    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSif_copyMmsContentType)
        WE_LOG_FC_UINT8(modId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
    return copyMmsContentType(modId, toCT, fromCT);

    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif
} /* MMSif_copyMmsContentType */

/*!
 * \brief Frees a MmsDrmInfo data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param ct In: The data to free 
 * \param freeRoot In: Shall the base structure be freed?
 *****************************************************************************/
void MMSif_freeMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *drmInfo, WE_BOOL freeRoot)
{
    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSif_freeMmsDrmInfo)
        WE_LOG_FC_UINT8(modId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
    freeMmsDrmInfo(modId, drmInfo);
    if (freeRoot == TRUE)
    {
        WE_MEM_FREE(modId, drmInfo);
    }

    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif
} /* MMSif_freeMmsContentType */

/*!
 * \brief Frees a MmsDrmInfo data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param toCT: Data structure to fill-in 
 * \param fromCT: Data structure to copy 
*****************************************************************************/
WE_BOOL MMSif_copyMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *toDrmInfo, MmsDrmInfo *fromDrmInfo)
{
    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSif_copyMmsDrmInfo)
        WE_LOG_FC_UINT8(modId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
    return copyMmsDrmInfo(modId, toDrmInfo, fromDrmInfo);

    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif
} /* MMSif_copyMmsDrmInfo */

/*!
 * \brief Frees a MmsBodyInfoList data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param mHeader In: The data to free 
*****************************************************************************/
void MMSif_freeMmsBodyInfoList( WE_UINT8 modId, MmsBodyInfoList *list)
{
    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSlib_freeMmsBodyInfoList)
        WE_LOG_FC_UINT8(modId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
    freeMmsBodyInfoList(modId, list);
    WE_MEM_FREE(modId, list);

    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif
} /* MMSif_freeMmsBodyInfoList */

/*!
 * \brief Frees a MmsGetHeader data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param mHeader In: The data to free 
*****************************************************************************/
MmsBodyInfoList *MMSif_copyMmsBodyInfoList(WE_UINT8 modId, MmsBodyInfoList *list)
{
    MmsBodyInfoList *result = NULL;
    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSlib_copyMmsBodyInfoList)
        WE_LOG_FC_UINT8(modId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
    result = copyMmsBodyInfoList(modId, list);

    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif

    return result;
} /* MMSif_copyMmsBodyInfoList */

/*!
 * \brief Frees a MmsGetHeader data structure.
 *
 *  Type: Function call
 *
 * \param modId In: WE module identification 
 * \param mHeader In: The data to free 
*****************************************************************************/
void MMSif_freeMmsGetHeader( WE_UINT8 modId, MmsGetHeader *mHeader)
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
} /* MMSif_freeMmsGetHeader */

/*****************************************************************************/
/*
 * Convert parameters into a signal buffer for MMS_SIG_CREATE_MSG
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsAllParams(we_dcvt_t *obj, MmsAllParams **data)
{
    MmsAllParams *tmpPtr;
    MmsAllParams *newParam;
    WE_UINT8 counter;
    WE_INT32 type;
    WE_INT32 param;

    if (obj->operation == WE_DCVT_FREE)
    {
        if (*data != NULL)
        {
            freeMmsContentTypeParams(obj->module, *data);
            WE_MEM_FREE(obj->module, *data);
            *data = NULL;
        }
        return TRUE;
    }
    
    if (obj->operation != WE_DCVT_DECODE)
    {
        tmpPtr = *data;
        counter = 0;
        while (NULL != tmpPtr)
        {
            counter++;
            tmpPtr = tmpPtr->next;
        }
    }

    if (!we_dcvt_uint8(obj, &counter))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */


    if (obj->operation == WE_DCVT_DECODE)
    {
        tmpPtr = NULL;
        *data = NULL;
    }
    else
    {
        tmpPtr = *data;
    }
    for(;counter>0; counter--)
    {
        if (obj->operation == WE_DCVT_DECODE)
        {
            newParam = WE_MEM_ALLOC(obj->module, sizeof(MmsAllParams));

            if (!we_dcvt_int32(obj, &param) ||
                !we_dcvt_int32(obj, &type) ||
                !we_dcvt_string(obj, (char**)&newParam->key))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }
                
            newParam->param = (MmsParam)param;
            newParam->type  = (MmsParamType)type;
            newParam->next  = NULL;

            switch (newParam->type)
            {
            case MMS_PARAM_STRING:
                if (!we_dcvt_string(obj, (char**)&newParam->value.string))
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                    return FALSE;                
                }                
                break;
            case MMS_PARAM_INTEGER:
                if (!we_dcvt_uint32(obj, &newParam->value.integer))
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                    return FALSE;                
                }                
                break;
                
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }
            
            if (tmpPtr == NULL)
            {
                *data = newParam;
                tmpPtr = newParam;
            }
            else
            {
                tmpPtr->next = newParam;
                tmpPtr = newParam;
            }
            
        }
        else /* obj->operation == Encode/Size */
        {
            if (tmpPtr == NULL)
            {
                /* Should never happen...*/
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }
            
            param = tmpPtr->param;
            type = tmpPtr->type;

            if (!we_dcvt_int32(obj, &param) ||
                !we_dcvt_int32(obj, &type) ||
                !we_dcvt_string(obj, (char**)&tmpPtr->key))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }

            switch (tmpPtr->type)
            {
            case MMS_PARAM_STRING:
                if (!we_dcvt_string(obj, (char**)&tmpPtr->value.string))
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                    return FALSE;                
                }                
                break;
            case MMS_PARAM_INTEGER:
                if (!we_dcvt_uint32(obj, &tmpPtr->value.integer))
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                    return FALSE;                
                }                
                break;
                
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }
            
            tmpPtr = tmpPtr->next;
        }
    }

    return TRUE;
} /* mms_cvt_MmsAllParams */

int mms_cvt_MmsDrmInfo(we_dcvt_t *obj, MmsDrmInfo **data)
{
    WE_INT32 drmType;
    
    if (obj->operation == WE_DCVT_FREE)
    {
        if (*data == NULL)
        {
            return TRUE;
        }
        
        freeMmsDrmInfo(obj->module, *data);
        WE_MEM_FREE(obj->module, *data);
        *data = NULL;
    }
    else if (obj->operation == WE_DCVT_DECODE)
    {
        if (!we_dcvt_int32(obj, &drmType))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
            return FALSE;                
        }
        
        if (drmType == MmsNoProtection)
        {
            *data = NULL;
        }
        else
        {
            (*data) = WE_MEM_ALLOC(obj->module, sizeof(MmsDrmInfo));
            if ((*data) == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                                
            }
            
            (*data)->drmType = (MmsDrmType) drmType;
            
            if (!we_dcvt_string(obj, &(*data)->dcfFile) ||
                !mms_cvt_MmsContentType(obj, &(*data)->drmContainerType))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }
        }
    }
    else /* Encode and size */
    {
        if (*data == NULL)
        {
            drmType = MmsNoProtection;
            if (!we_dcvt_int32(obj, &drmType))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }
        }
        else
        {
            drmType = (WE_INT32) (*data)->drmType;
            if (!we_dcvt_int32(obj, &drmType) ||
                !we_dcvt_string(obj, &(*data)->dcfFile) ||
                !mms_cvt_MmsContentType(obj, &(*data)->drmContainerType))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                
            }
        }
    }
    
    return TRUE;
} /* mms_cvt_MmsDrmInfo */

/*
 * Convert parameters into a signal buffer for MMS_SIG_CREATE_MSG
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsContentType(we_dcvt_t *obj, MmsContentType *data)
{
    WE_INT32 knownValue = data->knownValue;

    if (!we_dcvt_int32(obj, &knownValue) ||
        !we_dcvt_string(obj, (char**)&data->strValue) ||
        !mms_cvt_MmsAllParams(obj, &data->params) ||
        !mms_cvt_MmsDrmInfo(obj, &data->drmInfo))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */
    
    data->knownValue = (MmsKnownMediaType)knownValue;

    return TRUE;
} /* mms_cvt_MmsContentType */

/*
 * Convert parameters into a signal buffer for MMS_SIG_CREATE_MSG
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsFileType(we_dcvt_t *obj, st_MmsFileTypeSig *pstData)
{
    WE_UINT8 fileType = 0;
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        fileType = (WE_UINT8)pstData->eFileType; 
    } /* if */
    
    if (!we_dcvt_uint8(obj, &fileType))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */
    
    pstData->eFileType = (MmsFileType)fileType;
    return TRUE; 
} /* mms_cvt_createMessage */

/*
 * Convert parameters into a signal buffer for MmsAppReg
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsAppReg(we_dcvt_t *obj, MmsAppReg *data)
{
    WE_UINT8 source = 0;
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        source = (WE_UINT8)data->source; 
    } /* if */
        

    if (!we_dcvt_uint8(obj, &source))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->source = source; 
    return TRUE; 
} /* mms_cvt_MmsAppReg */

/*
 * Convert parameters into a signal buffer for MmsMsgMoveSig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgFolder(we_dcvt_t *obj, MmsMsgFolderSig *data)
{
    WE_UINT32 msgId = 0;
    WE_INT32 folder = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        msgId = (WE_UINT32)data->msgId; 
        folder = (WE_INT32)data->folder; 
    } /* if */

    if (!we_dcvt_uint32(obj, &msgId) ||
        !we_dcvt_int32(obj, &folder))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = msgId; 
    data->folder = (MmsFolderType)folder;
    return TRUE; 
} /* mms_cvt_MmsMsgFolder */

/*
 * Convert parameters into a signal buffer for MmsMsgMoveSig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgDone(we_dcvt_t *obj, MmsMsgSig *data)
{
    WE_UINT32 msgId = 0;
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        msgId = (WE_UINT32)data->msgId;
    } /* if */

    if (!we_dcvt_uint32(obj, &msgId))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = msgId;
    return TRUE;
} /* mms_cvt_MmsMsgDone */

/*
 * Convert parameters into a signal buffer for MmsSendReadReport
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsSendReadReport(we_dcvt_t *obj, MmsSendReadReport *data)
{
    WE_UINT32 msgId = 0;
    WE_INT32 readStatus = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        msgId = (WE_UINT32)data->msgId; 
        readStatus = (WE_INT32)data->readStatus; 
    } /* if */

    if (!we_dcvt_uint32(obj, &msgId) ||
        !we_dcvt_int32(obj, &readStatus))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */
    data->msgId = msgId;
    data->readStatus = (MmsReadStatus)readStatus; 
    return TRUE; 
} /* mms_cvt_MmsSendReadReport */

/*
 * Convert parameters into a signal buffer for struct MmsMsgSig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsg(we_dcvt_t *obj, MmsMsgSig *data)
{
    if (!we_dcvt_int32(obj, (WE_INT32*)&(data->msgId)))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    return TRUE; 
} /* mms_cvt_MmsMsg */

/*
 * Convert parameters into a signal buffer for struct MmsGetMsgRequest
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsGetMsgRequest(we_dcvt_t *obj, MmsGetMsgRequest *data)
{
    if (!we_dcvt_int32(obj, (WE_INT32*)&(data->msgId)) ||
        !we_dcvt_int32(obj, (WE_INT32*)&(data->userData)))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    return TRUE; 
} /* mms_cvt_MmsGetMsgRequest */

/*
 * Convert parameters into a signal buffer for struct MmsGetMsgRequest
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsGetBodyPartRequest(we_dcvt_t *obj, MmsGetBodyPartRequest *data)
{
    if (!we_dcvt_uint32(obj, &(data->msgId)) ||
        !we_dcvt_uint16(obj, &(data->bodyPartId))||
        !we_dcvt_uint32(obj, &(data->userData)))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    return TRUE; 
} /* mms_cvt_MmsGetBodyPartRequest */

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSigGetMsgHeader 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetMsgHeaderReply(we_dcvt_t *obj, MmsGetMsgHeaderReply *data)
{
    WE_UINT32 result;
    WE_UINT32 msgId;
    WE_UINT32 wspSize;
    unsigned char *wspHeader = NULL;

    msgId = (WE_UINT32) data->msgId;
    result = (WE_UINT32) data->result;

    if (
        !we_dcvt_uint32(obj, &(result)) ||
        !we_dcvt_uint32(obj, &(msgId)) ||
        !we_dcvt_uint32(obj, &(data->userData))
        )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = (MmsMsgId) msgId;
    data->result = (MmsResult) result;

    if (obj->operation == WE_DCVT_FREE)
    {
        if (NULL != data->header)
        {
            MMSlib_freeMmsGetHeader(obj->module, data->header);
            data->header = NULL;
        }
    }
    else if (MMS_RESULT_OK == data->result &&
        obj->operation == WE_DCVT_DECODE)
    {
        data->header = NULL;

        if (!we_dcvt_uint32(obj, &wspSize) ||
            !we_dcvt_uchar_vector(obj, (WE_INT32)wspSize, &wspHeader)
            )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
            return FALSE;
        } /* if */

        data->header = WE_MEM_ALLOC(obj->module, sizeof(MmsGetHeader));
        if (NULL == data->header)
        {
            WE_MEM_FREE(obj->module, wspHeader);
            return FALSE;
        }
        
        data->result = parseMmsHeader(  obj->module,
                wspHeader, wspSize, 
                data->header);
        
        /*
         *	Free the WSP-header (no longer needed).
         */
        WE_MEM_FREE(obj->module, wspHeader);
    }

    return TRUE; 
} /* mms_cvt_MmsGetMsgHeaderReply */


/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSigGetMsgSkeleton 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetSkeletonReply(we_dcvt_t *obj, MmsGetSkeletonReply *data)
{
    WE_UINT32 result;
    WE_UINT32 msgId;
    MmsResult cvt_res = MMS_RESULT_OK;
    WE_UINT32 size;
    MmsBodyInfoList *current=NULL;
    MmsBodyInfoList *newEntry=NULL;
    WE_UINT32 number = 0;
    unsigned char *wspEntryHeader=NULL;
    WE_UINT32 wspEntrySize;
    WE_BOOL prevIsMultipart = FALSE;
    MmsContentType tmpCT;
    
    msgId = (WE_UINT32) data->msgId;
    result = (WE_UINT32) data->result;

    if (
        !we_dcvt_uint32(obj, &(result)) ||
        !we_dcvt_uint32(obj, &(msgId)) ||
        !we_dcvt_uint32(obj, &(data->userData))
        )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = (MmsMsgId) msgId;
    data->result = (MmsResult) result;

    if (obj->operation == WE_DCVT_DECODE)
    {
        data->infoList = NULL;
    }

    /*
     *	If operation was successful get rest of data.
     */
    if (obj->operation == WE_DCVT_DECODE &&
        MMS_RESULT_OK == data->result)
    {
        /*
         *	Get first entry header size if any.
         */
        if (!we_dcvt_uint32(obj, &size))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
            return FALSE;
        }

        /*
         *	While there are entries to convert.
         */
        while (size > 0)
        {
            newEntry = WE_MEM_ALLOC(obj->module, sizeof(MmsBodyInfoList));
            if (NULL == newEntry)
            {
                /*
                 *	Free list of body-part entries.
                 */
                freeMmsBodyInfoList( obj->module, data->infoList);
                WE_MEM_FREE(obj->module, data->infoList);
                data->infoList = NULL;

                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;
            }
            
            newEntry->next = NULL;
            newEntry->entryHeader = NULL;
            newEntry->contentType = NULL;
            newEntry->number = number;

            if (current == NULL)
            {
                /*
                 *	Aha!, First entry.
                 */
                data->infoList = newEntry;
                current = newEntry;
            }
            else
            {
                current->next = newEntry;
                current = newEntry;
            }

            wspEntrySize = size;

            if (!we_dcvt_uint32(obj, &(newEntry->size)) ||
                !we_dcvt_uint32(obj, &(newEntry->startPos)) ||
                !we_dcvt_uint32(obj, &(newEntry->numOfEntries)) ||
                !we_dcvt_uchar_vector(obj, (WE_INT32)size, &wspEntryHeader)
                )
            {
                /*
                 *	Free list of body-part entries.
                 */
                freeMmsBodyInfoList( obj->module, data->infoList);
                WE_MEM_FREE(obj->module, data->infoList);
                data->infoList = NULL;

                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, obj->module, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;
            }

            if (number == 0)
            {
                if (parseContentType( obj->module,
                        &newEntry->contentType,
                        wspEntryHeader, wspEntrySize) == NULL)
                {
                    cvt_res = MMS_RESULT_MESSAGE_CORRUPT;
                }
            }
            else
            {
                cvt_res = parseBodyHeader( obj->module, prevIsMultipart, 
                                newEntry, wspEntryHeader, wspEntrySize);
            }

            if (MMS_RESULT_OK != cvt_res)
            {
                /*
                 *	Free list of body-part entries.
                 */
                freeMmsBodyInfoList( obj->module, data->infoList);
                WE_MEM_FREE(obj->module, data->infoList);
                data->infoList = NULL;

                data->result = cvt_res;
                return TRUE;
            }

            if (!mms_cvt_MmsDrmInfo(obj, &newEntry->contentType->drmInfo) ||
                !we_dcvt_uint32(obj, &(size)) /* Size of next entry */
                )
            {
                /*
                 *	Free list of body-part entries.
                 */
                freeMmsBodyInfoList( obj->module, data->infoList);
                WE_MEM_FREE(obj->module, data->infoList);
                data->infoList = NULL;

                return FALSE;
            }
                
            /* If DRM is active swap to content-types for simplicity */
            if (NULL != newEntry->contentType->drmInfo)
            {
                /*
                 *	Swap content-type data for DRM and contained CT.
                 */
                tmpCT.knownValue = newEntry->contentType->drmInfo->drmContainerType.knownValue;
                newEntry->contentType->drmInfo->drmContainerType.knownValue = newEntry->contentType->knownValue;
                newEntry->contentType->knownValue = tmpCT.knownValue;
                tmpCT.strValue = newEntry->contentType->drmInfo->drmContainerType.strValue;
                newEntry->contentType->drmInfo->drmContainerType.strValue = newEntry->contentType->strValue;
                newEntry->contentType->strValue = tmpCT.strValue;
            }

            /*
             *	Check if next entryheader start with "num-of-sub-parts".
             */
            prevIsMultipart = mmsIsMultiPart( newEntry->contentType->knownValue );

            /*
             *	Free the WSP entry header (not needed anymore).
             */
            WE_MEM_FREE(obj->module, wspEntryHeader);
            wspEntryHeader = NULL;
            
            number++;
        } /* While there are entries */
    }
    else if (obj->operation == WE_DCVT_FREE)
    {
        /*
         *	Free list of body-part entries.
         */
        freeMmsBodyInfoList( obj->module, data->infoList);
        WE_MEM_FREE(obj->module, data->infoList);
        data->infoList = NULL;
    }
    
    return TRUE; 
}

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsSigGetMsgBodyPart 
 * 
 * \param obj  The WE object. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE 
 *****************************************************************************/
int mms_cvt_MmsGetBodyPartReply(we_dcvt_t *obj, MmsGetBodyPartReply *data)
{
    WE_UINT32 result;
    WE_UINT32 msgId;
    char *pipeName;

    msgId = (WE_UINT32) data->msgId;
    result = (WE_UINT32) data->result;
    pipeName = (char *) data->pipeName;
    if (
        !we_dcvt_uint32(obj, &(result)) ||
        !we_dcvt_uint32(obj, &(msgId)) ||
        !we_dcvt_uint32(obj, &(data->userData)) ||
        !we_dcvt_uint32(obj, &(data->bodyPartSize)) ||
        !we_dcvt_uint16(obj, &(data->bodyPartId)) ||
        !we_dcvt_string(obj, &pipeName)
        )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = (MmsMsgId) msgId;
    data->result = (MmsResult) result;
    data->pipeName = (unsigned char*) pipeName;
    return TRUE; 
} /* mms_cvt_MmsGetBodyPartReply */

/*
 * Convert parameters into a signal buffer for struct MmsFolderSig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsFolderId(we_dcvt_t *obj, MmsFolderSig *data)
{
    WE_UINT32 folderId = 0;
    WE_UINT8  source = 0;
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        folderId = (WE_UINT32)data->folderId;
        source = (WE_UINT8)data->source;
    } /* if */
    if (!we_dcvt_uint32(obj, &folderId) ||
        !we_dcvt_uint8(obj, &source))
        {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->source = (WE_UINT8) source;
    data->folderId = (MmsFolderType)folderId;
    return TRUE; 
} /* mms_cvt_MmsFolderId */

/*
 * Convert parameters into a signal buffer for MmsSetConfigStr
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsSetReadMark(we_dcvt_t *obj, MmsSetReadMark *data)
{
    WE_UINT32 msgId = 0;
    WE_UINT8  value = 0;
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        msgId = (WE_UINT32) data->msgId; 
        value = (WE_UINT8) data->value; 
    } /* if */

    if (!we_dcvt_uint32(obj, &msgId) ||
        !we_dcvt_uint8(obj, &value) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = msgId; 
    data->value = value; 
    return TRUE; 
} /* mms_cvt_MmsSetReadMark */

/*
 * Convert parameters into a signal buffer for MmsPduSig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsSendPdu(we_dcvt_t *obj, MmsPduSig *data, WE_UINT32 dataLen)
{
    WE_UINT32 length = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        length = (WE_UINT32) data->length;
    } /* if */

    if (NULL == data || !we_dcvt_uint32(obj, &length) ||
        !we_dcvt_uchar_vector(obj, (long) dataLen, &data->pdu) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->length = length;
    return TRUE; 
} /* mms_cvt_MmsSendPdu */

/****************************************************************************
 * Convertion functions for reply signals 
 ****************************************************************************/

/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsMessageProperties 
 * 
 * \param obj  The WE obejct. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE
 *****************************************************************************/
int mms_cvt_MmsMessageFolderInfo(we_dcvt_t *obj, MmsMessageFolderInfo *data)
{
    WE_UINT32  folderId;     /*!< the folder location */
    WE_UINT8   drmStatus;   /*!< Bit0: is set to TRUE if DRM-forward-locked */
    WE_UINT8   priority;   /*!< Priority of message */
    WE_UINT8   suffix;       /*!< define the type of file */

    if (NULL == obj)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;        
    }

    if (obj->operation != WE_DCVT_DECODE)
    {
        folderId        = (WE_INT32) data->folderId;
        drmStatus       = (WE_UINT8) data->drmStatus;
        priority        = (WE_UINT8) data->priority;
        suffix          = (WE_UINT8) data->suffix;
    } /* if */
    
    if (!we_dcvt_uint32(obj,&folderId) ||
        !we_dcvt_uint32(obj, &data->msgId) ||
        !we_dcvt_uint32(obj, &data->size) ||
        !we_dcvt_uint32(obj, &data->date) ||
        !we_dcvt_uint8(obj, &priority) ||
        !we_dcvt_uint8(obj, &drmStatus) ||
        !we_dcvt_uint8(obj, &suffix) ||
        !we_dcvt_uint8(obj, &data->sent) ||
        !we_dcvt_uint8(obj, &data->read) ||
        !we_dcvt_uint8(obj, &data->rrReq) ||
        !we_dcvt_uint8(obj, &data->rrRead) ||
        !we_dcvt_uint8(obj, &data->rrDel) ||
        !we_dcvt_uint8(obj, &data->drReq) ||
        !we_dcvt_uint8(obj, &data->drRec) ||
        !we_dcvt_static_uchar_vector(obj, MAX_MMT_SERVER_MSG_ID_LEN, (unsigned char *)data->serverMsgId) ||
        !we_dcvt_static_uchar_vector(obj, MAX_MMT_ADDRESS_LEN, (unsigned char *)data->address) ||
        !we_dcvt_static_uchar_vector(obj, MAX_MMT_SUBJECT_LEN, (unsigned char *)data->subject)
        )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */
    
    data->folderId        = (MmsFolderType) folderId;
    data->drmStatus       = drmStatus;
    data->priority        = (MmsPriority) priority;
    data->suffix          = (MmsFileType) suffix;

    return TRUE; 
    
} /* mms_cvt_MmsMessageFolderInfo */


/*!
 * \brief  Converts a parameters into a signal buffer for 
 *         MmsMessageProperties 
 * 
 * \param obj  The WE obejct. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE
 *****************************************************************************/
int mms_cvt_MmsMessageProperties(we_dcvt_t *obj, MmsMessageProperties *data)
{
    WE_UINT8                rrListLength = 0;
    WE_UINT32                *rrList = NULL;
    WE_UINT8                drListLength = 0;
    WE_UINT32                *drList = NULL;
    int                      i;
    
    if (obj == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;        
    }

    if (obj->operation != WE_DCVT_DECODE)
    {
        rrListLength = data->rrListLength;
        drListLength = data->drListLength;
    } /* if */

    if (obj->operation == WE_DCVT_FREE)
    {
        if (rrListLength > 0 && NULL != data->rrList)
        {
            WE_MEM_FREE(obj->module, data->rrList);
        }
        if (drListLength > 0 && NULL != data->drList)
        {
            WE_MEM_FREE(obj->module, data->drList);
        }
        return TRUE;
    }
    
    if (!mms_cvt_MmsMessageFolderInfo(obj,&(data->msgFolderInfo)) ||
        !we_dcvt_uint8(obj, &rrListLength) ||
        !we_dcvt_uint8(obj, &drListLength))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    if (obj->operation == WE_DCVT_DECODE)
    {
        if (rrListLength > 0)
        {
            rrList = WE_MEM_ALLOC(obj->module, rrListLength*sizeof(WE_UINT32));
            if (NULL == rrList)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                        
            }
        }
        else
        {
            rrList = NULL;
        }
    }
    else
    {
        rrList = data->rrList;
    }
    if (rrList != NULL) 
    {
    
        for(i=0; i<rrListLength; i++)
        {
            if (!we_dcvt_uint32(obj, &(rrList[i])))
            {
                if (obj->operation == WE_DCVT_DECODE)
                {
                    WE_MEM_FREE(obj->module, rrList);
                }
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;            
            }
        }

    }

    if (obj->operation == WE_DCVT_DECODE)
    {
        if (drListLength > 0)
        {
            drList = WE_MEM_ALLOC(obj->module, drListLength*sizeof(WE_UINT32));
            if (NULL == drList)
            {
                WE_MEM_FREE(obj->module, rrList);

                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;                        
            }
        }
        else
        {
            drList = NULL;
        }
    }
    else
    {
        drList = data->drList;
    }
    
    if (drList != NULL) 
    {
    
        for(i=0; i<drListLength; i++)
        {
            if (!we_dcvt_uint32(obj, &(drList[i])))
            {
                if (obj->operation == WE_DCVT_DECODE)
                {
                    WE_MEM_FREE(obj->module, rrList);
                    WE_MEM_FREE(obj->module, drList);
                }
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;            
            }
        }
        
    }
    data->rrListLength = rrListLength;
    data->drListLength = drListLength;
    data->rrList = rrList;
    data->drList = drList;
    
    return TRUE; 
    
} /* mms_cvt_MmsMessageProperties */

/*
 * Convert list of message properties into a signal buffer
 *
 * \param obj  The WE obejct. 
 * \param data The data structure to convert to
 * \return TRUE if OK else FALSE
 *****************************************************************************/
int mms_cvt_MmsListOfProperties(we_dcvt_t *obj, MmsListOfProperties *data)
{
    WE_UINT32 noOfElements = 0;
    WE_UINT32 i;

   
    if (obj == NULL) 
    {
        return FALSE;
    }

    if (obj->operation != WE_DCVT_DECODE)
    {
        noOfElements = data->noOfElements;
    } /* if */


    if (obj->operation == WE_DCVT_FREE)
    {
        for(i=0; i<data->noOfElements; i++)
        {
            if (!mms_cvt_MmsMessageProperties(obj, &(data->msgPropertyList[i])))
            {
                return FALSE;
            }
        }

        if (NULL != data->msgPropertyList)
        {
            WE_MEM_FREE(obj->module, data->msgPropertyList);
        }
        return  TRUE;
    }    
    
    if (!we_dcvt_uint32(obj,&noOfElements))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    if (obj->operation == WE_DCVT_DECODE)
    {
        if (noOfElements > 0)
        {
            data->msgPropertyList = WE_MEM_ALLOC(obj->module, sizeof(MmsMessageProperties)*noOfElements);
        }
        else
        {
            data->msgPropertyList = NULL;
        }
    } /* if */

    for(i=0; i<noOfElements; i++)
    {
        if (!mms_cvt_MmsMessageProperties(obj, &(data->msgPropertyList[i])))
        {
            if (data->msgPropertyList != NULL && obj->operation == WE_DCVT_DECODE)
            {
                WE_MEM_FREE(obj->module, data->msgPropertyList);
            } /* if */
            
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
            return FALSE;
        } /* if */
    }
    
    data->noOfElements = noOfElements;

    return TRUE; 
} /* mms_cvt_MmsListOfProperties */

/*
 * Convert parameters into a signal buffer for MmsResultSig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsResult(we_dcvt_t *obj, MmsResultSig *data)
{
    WE_INT32 result = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        result = (WE_INT32)data->result;
    } /* if */

    if (!we_dcvt_int32(obj, &result))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->result = (MmsResult)result; 
    return TRUE; 
} /* mms_cvt_MmsResult */

/*
 * Convert parameters into a signal buffer for MmsMsgReplySig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgReply(we_dcvt_t *obj, MmsMsgReplySig *data)
{
    WE_INT32 result = 0;
    WE_UINT32 msgId  = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        result = (WE_INT32)data->result;
        msgId  = (WE_UINT32)data->msgId;
    } /* if */

    if (!we_dcvt_int32(obj,&result) ||
        !we_dcvt_uint32(obj, &msgId))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = msgId; 
    data->result = (MmsResult)result; 
    return TRUE; 
} /* mms_cvt_createMessageReply */

/*
 * Convert parameters into a signal buffer for MMS_SIG_SEND_RR_REPLY
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsRRReplySig(we_dcvt_t *obj, MmsRRReplySig *data)
{
    WE_INT32 result = 0;
    WE_UINT32 msgId  = 0;
    WE_UINT32 rrId  = 0;
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        result = (WE_INT32)data->result;
        msgId  = (WE_UINT32)data->msgId;
        rrId  = (WE_UINT32)data->rrId;
    } /* if */

    if (!we_dcvt_int32(obj,&result) ||
        !we_dcvt_uint32(obj, &msgId) ||
        !we_dcvt_uint32(obj, &rrId))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->rrId = rrId; 
    data->msgId = msgId; 
    data->result = (MmsResult)result; 
    return TRUE; 
} /* mms_cvt_MmsRRReply */


/*
 * Convert parameters into a signal buffer for MmsMsgReplySig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgDoneReply(we_dcvt_t *obj, MmsMsgDoneReplySig *data)
{
    WE_INT32 result = 0;
    WE_UINT32 msgId  = 0;
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        result = (WE_INT32)data->result;
        msgId  = (WE_UINT32)data->msgId;
    } /* if */
    
    if (!we_dcvt_int32(obj, &result) ||
        !we_dcvt_uint32(obj, &msgId))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->msgId = msgId; 
    data->result = (MmsResult)result; 
    return TRUE; 
} /* mms_cvt_createMsgDoneReply */

/*
 * Convert parameters into a signal buffer for MmsMsgReplySig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgReplyNoOfMsg(we_dcvt_t *obj, MmsMsgReplyNoOfMsg *data)
{
    WE_INT32 result = 0;
    WE_UINT32 number = 0;
    WE_INT32 folder = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        result = (WE_INT32)data->result;
        number = (WE_UINT32)data->number;
        folder = (WE_INT32)data->folder; 
    } /* if */

    if (!we_dcvt_int32(obj, &result) ||
        !we_dcvt_uint32(obj, &number) ||
        !we_dcvt_int32(obj, &folder))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->folder = (MmsFolderType)folder; 
    data->number = number; 
    data->result = (MmsResult)result; 
    return TRUE; 
} /* mms_cvt_MmsMsgReplyNoOfMsg */

/*
 * Convert parameters into a signal buffer for MmsMsgReplySig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgNotificationMsg(we_dcvt_t *obj, 
    MmsMsgReplyNotification *data)
{
    /* Must convert enums to known type */
    WE_INT32 type = 0;
    WE_UINT32 notifId = 0;
    WE_UINT32 fromCharset = 0;
    WE_UINT32 fromAddrType = 0;
    WE_UINT32 subjectCharset = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        type = (WE_INT32)data->type;
        notifId = (WE_UINT32)data->notifId;
        fromCharset = (WE_UINT32)data->fromCharset;
        fromAddrType = (WE_UINT32)data->fromAddrType;
        subjectCharset = (WE_UINT32)data->subjectCharset;
    } /* if */

    if (!we_dcvt_int32(obj, &type) ||
        !we_dcvt_uint32(obj, &notifId) ||
        !we_dcvt_uint32(obj, &fromCharset) ||
        !we_dcvt_string(obj, &data->fromName) ||
        !we_dcvt_string(obj, &data->fromAddress) ||
        !we_dcvt_uint32(obj, &fromAddrType) ||
        !we_dcvt_uint32(obj, &subjectCharset) ||
        !we_dcvt_string(obj, &data->subjectText) ||
        !we_dcvt_uint32(obj, &data->size))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->type = (MmsNotificationType)type; 
    data->notifId = notifId; 
    data->fromCharset = (MmsCharset)fromCharset;
    data->fromAddrType = (MmsAddressType)fromAddrType;
    data->subjectCharset = (MmsCharset)subjectCharset;

    return TRUE; 
} /* mms_cvt_MmsMsgNotificationMsg */

/*
 * Convert parameters into a signal buffer for MmsMsgReplySig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgReadReport(we_dcvt_t *obj, MmsMsgReadReport *data)
{  
    WE_INT32 addrType = 0;
    WE_INT32 charset = 0;
    WE_UINT32 date = 0;
    WE_INT32 readStatus = 0;
    WE_UINT32 msgId  = 0; 
    WE_UINT32 msgIdFile = 0; 

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        addrType = (WE_INT32)data->addrType; 
        charset = (WE_INT32)data->charset;
        date = (WE_UINT32)data->date; 
        readStatus = (WE_INT32)data->readStatus;
        msgId = data->msgId; 
        msgIdFile = data->msgIdFile;
    } /* if */
    
    if (!we_dcvt_string(obj, &(data->address)) ||
       !we_dcvt_int32(obj, &addrType) ||
       !we_dcvt_int32(obj, &charset) ||
       !we_dcvt_string(obj, &(data->text)) ||
       !we_dcvt_uint32(obj, &date) ||
       !we_dcvt_string(obj, &(data->serverMessageId)) ||
       !we_dcvt_int32(obj, &readStatus) ||
       !we_dcvt_uint32(obj, &msgId) ||
       !we_dcvt_uint32(obj, &msgIdFile) 
       )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
       return FALSE;
    } /* if */

   data->addrType = (MmsAddressType)addrType;
   data->charset = (MmsCharset)charset; 
   data->date = date; 
   data->readStatus = (MmsReadStatus)readStatus;
   data->msgId = msgId; 
   data->msgIdFile = msgIdFile; 
   
   return TRUE; 
} /* mms_cvt_MmsMsgReadReport */

/*
 * Convert parameters into a signal buffer for MmsMsgReplySig
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgDeliveryReport(we_dcvt_t *obj, MmsMsgDeliveryReportInd *data)
{
    WE_INT32 addrType = 0;
    WE_INT32 charset  = 0;
    WE_UINT32 date = 0;
    WE_INT32 status = 0;
    WE_UINT32 msgId = 0;
    WE_UINT32 msgIdFile = 0; 
    
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        addrType = (WE_INT32) data->addrType; 
        charset  = (WE_INT32) data->charset; 
        date = (WE_UINT32) data->date; 
        status = (WE_INT32) data->status; 
        msgId = (WE_UINT32) data->msgId; 
        msgIdFile = (WE_UINT32) data->msgIdFile; 
    } /* if */

    if (!we_dcvt_string(obj, &(data->serverMsgId)) ||
       !we_dcvt_string(obj, &(data->address)) ||
       !we_dcvt_int32(obj, &addrType) ||
       !we_dcvt_int32(obj, &charset) ||
       !we_dcvt_string(obj, &(data->text)) ||
       !we_dcvt_uint32(obj, &date) ||
       !we_dcvt_int32(obj, &status) ||
       !we_dcvt_uint32(obj, &msgId) ||
       !we_dcvt_uint32(obj, &msgIdFile) 
       )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
       return FALSE;
    } /* if */

    data->addrType = (MmsAddressType)addrType;
    data->charset = (MmsCharset)charset; 
    data->date = date; 
    data->status = (MmsStatus)status; 
    data->msgId = msgId;
    data->msgIdFile = msgIdFile;

    return TRUE; 
} /* mms_cvt_MmsMsgDeliveryReport */

/*
 * Convert parameters into a signal buffer for MmsMsgContentReq
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsMsgContentReq(we_dcvt_t *obj, MmsMsgContentReq *data)
{
    WE_INT32 folderId = 0;
    WE_INT32 from = 0;
    WE_INT32 to = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        folderId = (WE_INT32)data->folderId;
        from = (WE_INT32)data->from; 
        to = (WE_INT32)data->to; 
    } /* if */

    if (!we_dcvt_int32(obj, &folderId) ||
        !we_dcvt_int32(obj, &from) ||
        !we_dcvt_int32(obj, &to)) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    data->folderId = (MmsFolderType)folderId; 
    data->from = from; 
    data->to = to;
    return TRUE; 
} /* mms_cvt_MmsMsgContentReq */

/*
 * Convert parameters into a signal buffer for MmsFolderStatus
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsFolderStatus(we_dcvt_t *obj, MmsFolderStatus *data)
{
    WE_UINT32 usedMsgBytes = 0;
    WE_UINT32 numOfMsg = 0;
    WE_UINT32 totalFolderSize = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        usedMsgBytes = (WE_UINT32) data->usedMsgBytes;
        numOfMsg = (WE_UINT32)data->numOfMsg; 
        totalFolderSize = (WE_UINT32) data->totalFolderSize; 
    } /* if */

    if (!we_dcvt_uint32(obj, &usedMsgBytes) ||
        !we_dcvt_uint32(obj, &numOfMsg) ||
        !we_dcvt_uint32(obj, &totalFolderSize)) 
    {
         WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed for MmsFolderStatus.\n",
            __FILE__, __LINE__));
        return FALSE;
    } /* if */ 
    data->numOfMsg = numOfMsg; 
    data->totalFolderSize = totalFolderSize; 
    data->usedMsgBytes = usedMsgBytes; 
    return TRUE; 
} /* mms_cvt_MmsFolderStatus */

/*
 * Convert parameters into a signal buffer for MmsProgressStatus
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int mms_cvt_MmsProgressStatus(we_dcvt_t *obj, MmsProgressStatus *data)
{
    WE_UINT32 msgId = 0;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        msgId = data->msgId;
    } /* if */

    if (!we_dcvt_uint8(obj, &data->operation) ||
        !we_dcvt_uint8(obj, &data->state) ||
        !we_dcvt_uint32(obj, &data->progress) ||
        !we_dcvt_uint32(obj, &data->totalLength) ||
        !we_dcvt_uint8(obj, &data->queueLength) ||
        !we_dcvt_uint32(obj, &msgId)
        ) 
    {
        data->msgId = 0; 
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed for MmsFolderStatus.\n",
            __FILE__, __LINE__));
        return FALSE;
    } /* if */ 
    data->msgId = (MmsMsgId) msgId; 
    return TRUE; 
} /* mms_cvt_MmsProgressStatus */


/* 
 * Convert signals from serialized buffer into a signal struct
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 ****************************************************************************/
void *mmsConvert(WE_UINT8 modId, WE_UINT16 signal, void *buffer) 
{
    we_dcvt_t  cvtObj;   /* object to use */ 
    void       *userData; /* temp user data */
    WE_UINT16  length;   /* size of signal buffer */

    if (buffer == NULL)
    {
        return NULL;
    } /* if */

    userData = WE_SIGNAL_GET_USER_DATA(buffer, &length);

    if ((userData == NULL) || (length == 0))
    {
         /* memory error */ 
         return NULL;
    } /* if */
    
    we_dcvt_init(&cvtObj, WE_DCVT_DECODE, userData, length, modId);

    switch (signal)
    {        
    /*
     * Reply signals 
     */
    case MMS_SIG_APP_UNREG: 
    case MMS_SIG_APP_REG: 
        {
            MmsAppReg *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsAppReg);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsAppReg(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsAppReg failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }

    case MMS_SIG_CREATE_MSG_REPLY:
    case MMS_SIG_RETRIEVE_MSG_REPLY:  
    case MMS_SIG_MSG_DONE_REPLY:
        {
            MmsMsgReplySig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgReplySig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsgReply(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgReplySig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        } 

    case MMS_SIG_ERROR:
    case MMS_SIG_MOV_MSG_REPLY:  
    case MMS_SIG_DELETE_MSG_REPLY:    
    case MMS_SIG_SEND_MSG_REPLY:
    case MMS_SIG_FORWARD_REQ_REPLY:
        {
            MmsResultSig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsResultSig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsResult(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsResultSig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }  
 
    case MMS_SIG_SEND_RR_REPLY:
        {
            MmsRRReplySig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsRRReplySig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsRRReplySig(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsResultSig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */
            
            return ptr;
        }  
        
    case MMS_SIG_GET_NUM_OF_MSG_REPLY:
        {
            MmsMsgReplyNoOfMsg *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgReplyNoOfMsg);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsgReplyNoOfMsg(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgReplyNoOfMsg failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        } 
    case MMS_SIG_GET_FLDR_REPLY:
    case MMS_SIG_GET_MSG_INFO_REPLY:
        {
            MmsListOfProperties *ptr = NULL;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsListOfProperties);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if (!mms_cvt_MmsListOfProperties(&cvtObj, ptr))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsListOfProperties failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        } 
    case MMS_SIG_PROGRESS_STATUS_REPLY:
        {
            MmsProgressStatus *ptr = NULL;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsProgressStatus);
            if (!mms_cvt_MmsProgressStatus(&cvtObj, ptr))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsProgressStatus failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */
            
            return ptr;
        } 
        
    case MMS_SIG_READ_REPORT_IND:
        {
            MmsMsgReadReport *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgReadReport);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsgReadReport(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgReadReport failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }   
        
    case MMS_SIG_DELIVERY_REPORT:   
        {
            MmsMsgDeliveryReportInd *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgDeliveryReportInd);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsgDeliveryReport(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgDeliveryReportInd failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }   
        
    case MMS_SIG_NOTIFICATION:                
        {
            MmsMsgReplyNotification *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgReplyNotification);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsgNotificationMsg(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgReplyNotification failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }    
    case MMS_SIG_FOLDER_STATUS_REPLY:      
        {
            MmsFolderStatus *ptr; 
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsFolderStatus);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsFolderStatus(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion mms_cvt_MmsFolderStatus failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }

    case MMS_SIG_GET_MSG_HEADER_REPLY:
        {
            MmsGetMsgHeaderReply *ptr; 
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsGetMsgHeaderReply);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */

            if ( !mms_cvt_MmsGetMsgHeaderReply(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion mms_cvt_MmsGetMsgHeaderReply failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }

    case MMS_SIG_GET_MSG_SKELETON_REPLY:
        {
            MmsGetSkeletonReply *ptr; 
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsGetSkeletonReply);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */

            if ( !mms_cvt_MmsGetSkeletonReply(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion mms_cvt_MmsGetSkeletonReply failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }

    case MMS_SIG_GET_BODY_PART_REPLY:      
        {
            MmsGetBodyPartReply *ptr; 
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsGetBodyPartReply);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */

            if ( !mms_cvt_MmsGetBodyPartReply(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion mms_cvt_MmsGetBodyPartReply failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }

        
        /*
         * Received signals 
         */
    case MMS_SIG_CREATE_MSG:        
        {
            st_MmsFileTypeSig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, st_MmsFileTypeSig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsFileType(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion st_MmsFileTypeSig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */
            
            return ptr;
        }        
    case MMS_SIG_DELETE_MSG:
    case MMS_SIG_RETRIEVE_MSG: 
    case MMS_SIG_RETRIEVE_MSG_CANCEL:
    case MMS_SIG_SEND_MSG: 
    case MMS_SIG_SEND_MSG_CANCEL:
    case MMS_SIG_GET_REPORT:
    case MMS_SIG_GET_MSG_INFO:
        {
            MmsMsgSig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgSig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsg(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgSig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        } 
    case MMS_SIG_MSG_DONE:
    case MMS_SIG_MOVE_MSG:
        {
            MmsMsgFolderSig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgFolderSig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsgFolder(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgFolderSig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }
    case MMS_SIG_GET_FLDR: 
        {
            MmsMsgContentReq *ptr; 
            ptr = WE_MEM_ALLOCTYPE(modId, MmsMsgContentReq);            
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsMsgContentReq(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsMsgContentReq failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }
    case MMS_SIG_FOLDER_STATUS:     
    case MMS_SIG_GET_NUM_OF_MSG:
        {
            MmsFolderSig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE (modId, MmsFolderSig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
            if ( !mms_cvt_MmsFolderId(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsFolderSig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }
    case MMS_SIG_SET_READ_MARK: 
        {
             MmsSetReadMark *ptr; 
           
             ptr = WE_MEM_ALLOCTYPE (modId, MmsSetReadMark);
             if (ptr == NULL)
             {
                 WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                     "Unable to allocate memory in: mmsConvert()\n"));
                 return NULL;
             } /* if */
             if ( !mms_cvt_MmsSetReadMark(&cvtObj, ptr) )
             {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsSetReadMark failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
             } /* if */

             return ptr;
        }
    case MMS_SIG_READ_REPORT: 
        {      
             MmsSendReadReport *ptr; 
           
             ptr = WE_MEM_ALLOCTYPE (modId, MmsSendReadReport);
             if (ptr == NULL)
             {
                 WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                     "Unable to allocate memory in: mmsConvert()\n"));
                 return NULL;
             } /* if */
             if ( !mms_cvt_MmsSendReadReport(&cvtObj, ptr) )
             {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsSendReadReport failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
             } /* if */

             return ptr;
        }

    case MMS_SIG_FORWARD_REQ:
        {
            MmsPduSig *ptr;
            
            ptr = WE_MEM_ALLOCTYPE (modId, MmsPduSig);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
           
            if ( !mms_cvt_MmsSendPdu(&cvtObj, ptr, ((WE_UINT32)cvtObj.length) - sizeof(WE_UINT32)))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsPduSig failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }
    case MMS_SIG_GET_MSG_HEADER:
    case MMS_SIG_GET_MSG_SKELETON:
        {
            MmsGetMsgRequest *ptr;
            
            ptr = WE_MEM_ALLOCTYPE (modId, MmsGetMsgRequest);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
           
            if ( !mms_cvt_MmsGetMsgRequest(&cvtObj, ptr))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsGetMsgRequest failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }
    case MMS_SIG_GET_BODY_PART:
        {
            MmsGetBodyPartRequest *ptr;
            
            ptr = WE_MEM_ALLOCTYPE (modId, MmsGetBodyPartRequest);
            if (ptr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: mmsConvert()\n"));
                return NULL;
            } /* if */
           
            /* pachting the length of the vector! */
            if ( !mms_cvt_MmsGetBodyPartRequest(&cvtObj, ptr))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion MmsGetBodyPartRequest failed.\n", __FILE__, __LINE__));
                WE_MEM_FREE( modId, ptr);
                break;
            } /* if */

            return ptr;
        }

    default: 
        return NULL; 
    }   /* switch */  

    return NULL;
} /* mmsConvert */

/* 
 * Deallocate signal buffer
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void mmsDestruct(WE_UINT8 module, WE_UINT16 signal, void *ptr)
{
    we_dcvt_t cvtObj;

    if (ptr == NULL)
    {
       return; 
    } /* if */

    we_dcvt_init (&cvtObj, WE_DCVT_FREE, NULL, 0, module);

    switch (signal)
    {        
    /*
     * Reply signals 
     */
        
    case MMS_SIG_APP_REG:
    case MMS_SIG_APP_UNREG:
        {
            if ( !mms_cvt_MmsAppReg(&cvtObj, ptr) )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
            } /* if */
        }
        break;
    case MMS_SIG_CREATE_MSG_REPLY:
    case MMS_SIG_RETRIEVE_MSG_REPLY:
        if ( !mms_cvt_MmsMsgReply(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
        
    case MMS_SIG_MSG_DONE_REPLY:
        if ( !mms_cvt_MmsMsgDoneReply(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
 
    case MMS_SIG_ERROR:
    case MMS_SIG_MOV_MSG_REPLY:  
    case MMS_SIG_DELETE_MSG_REPLY:    
    case MMS_SIG_SEND_MSG_REPLY:
    case MMS_SIG_FORWARD_REQ_REPLY: 
        if ( !mms_cvt_MmsResult(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */

        break;

    case MMS_SIG_SEND_RR_REPLY:   
        if ( !mms_cvt_MmsRRReplySig(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        
        break;
        
    case MMS_SIG_GET_NUM_OF_MSG_REPLY:
        if ( !mms_cvt_MmsMsgReplyNoOfMsg(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
        
    case MMS_SIG_READ_REPORT_IND:
        if ( !mms_cvt_MmsMsgReadReport(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
        
    case MMS_SIG_DELIVERY_REPORT:               
        if ( !mms_cvt_MmsMsgDeliveryReport(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
        
    case MMS_SIG_NOTIFICATION:                
        if ( !mms_cvt_MmsMsgNotificationMsg(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
    case MMS_SIG_GET_MSG_INFO_REPLY:
    case MMS_SIG_GET_FLDR_REPLY:
        if ( !mms_cvt_MmsListOfProperties(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
    case MMS_SIG_FOLDER_STATUS_REPLY: 
        if ( !mms_cvt_MmsFolderStatus(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
    case MMS_SIG_PROGRESS_STATUS_REPLY:
        if ( !mms_cvt_MmsProgressStatus(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
    case MMS_SIG_GET_MSG_HEADER_REPLY:
        if ( !mms_cvt_MmsGetMsgHeaderReply(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
    case MMS_SIG_GET_MSG_SKELETON_REPLY:
        if ( !mms_cvt_MmsGetSkeletonReply(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
    case MMS_SIG_GET_BODY_PART_REPLY:
        if ( !mms_cvt_MmsGetBodyPartReply(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
        
    /*
     * Signals sent to the MMS Service
     */
    case MMS_SIG_CREATE_MSG:        
        if ( !mms_cvt_MmsFileType(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion MmsFileType failed.\n", __FILE__, __LINE__));
        } /* if */
        
        break;
    case MMS_SIG_MSG_DONE:
    case MMS_SIG_DELETE_MSG:
    case MMS_SIG_RETRIEVE_MSG: 
    case MMS_SIG_RETRIEVE_MSG_CANCEL:
    case MMS_SIG_SEND_MSG: 
    case MMS_SIG_SEND_MSG_CANCEL:
    case MMS_SIG_GET_REPORT:
    case MMS_SIG_GET_MSG_INFO:
        if ( !mms_cvt_MmsMsg(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion MmsMsg failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
        
    case MMS_SIG_MOVE_MSG:
        if ( !mms_cvt_MmsMsgFolder(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion MmsMsgFolder failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
    case MMS_SIG_GET_FLDR: 
        if ( !mms_cvt_MmsMsgContentReq(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion MmsMsgContentReq failed.\n", __FILE__, __LINE__));
        } /* if */

        break;
     case MMS_SIG_SET_READ_MARK:
        if ( !mms_cvt_MmsSetReadMark(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion MmsSetReadMark failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
     case MMS_SIG_FOLDER_STATUS: 
     case MMS_SIG_GET_NUM_OF_MSG:
        if ( !mms_cvt_MmsFolderId(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion  MmsFolderId failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
     case MMS_SIG_READ_REPORT: 
        if ( !mms_cvt_MmsSendReadReport(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion  MmsSendReadReport failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
     case MMS_SIG_FORWARD_REQ: 
                                            /* Patching the length of the vector*/
        if ( !mms_cvt_MmsSendPdu(&cvtObj, ptr, ((WE_UINT32)cvtObj.length) - sizeof(WE_UINT32)))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion  Forward request failed.\n", __FILE__, __LINE__));
        } /* if */
        break; 
     case MMS_SIG_GET_MSG_HEADER:
    case MMS_SIG_GET_MSG_SKELETON:
        if ( !mms_cvt_MmsGetMsgRequest(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;
    case MMS_SIG_GET_BODY_PART:
        if ( !mms_cvt_MmsGetBodyPartRequest(&cvtObj, ptr) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        } /* if */
        break;     default:  
        return; 
    } /* switch */    

    WE_MEM_FREE (module, ptr);
} /* mmsDestruct */

/* 
 * Application registration function, used to register the MMS application 
 * so that the MMS knows where to send notification messages. 
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 ****************************************************************************/
 void MMSif_applicationReg(WE_UINT8 source)
 {
    MmsAppReg  data;     /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_applicationReg)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_SIG_BEGIN("MMS_SIG_APP_REG", LS_SEND, source, (WE_UINT8)WE_MODID_MMS);

    WE_LOG_SIG_END();
    
    data.source = source; 

    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_APP_REG, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsAppReg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
 } /*MMSif_applicationReg */

/* 
 * Application registration function, used to register the MMS application 
 * so that the MMS knows where to send notification messages. 
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 ****************************************************************************/
 void MMSif_applicationUnreg(WE_UINT8 source)
 {
    MmsAppReg  data;     /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_applicationUnreg)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_SIG_BEGIN("MMS_SIG_APP_UNREG", LS_SEND, source, (WE_UINT8)WE_MODID_MMS);

    WE_LOG_SIG_END();
    
    data.source = source; 

    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_APP_UNREG, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsAppReg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
 } /*MMSif_applicationUnreg */

/*
 * Creates an empty message in a folder.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_createMessage(WE_UINT8 source, MmsFileType fileType)
{
    st_MmsFileTypeSig  stData;     /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_createMessage)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_INT32(fileType, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
        
    WE_LOG_SIG_BEGIN("MMS_SIG_CREATE_MSG", LS_SEND, source, (WE_UINT8)WE_MODID_MMS);
    WE_LOG_SIG_END();

    stData.eFileType = fileType;
    
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_CREATE_MSG, &stData, 
        (MmsIfConvertFunction *)mms_cvt_MmsFileType))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_createMessage */

/*
 * Indicate to the MMS client that the creation of a message is done.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_createMessageDone(WE_UINT8 source, MmsMsgId msgId, MmsFolderType folder)
{
    MmsMsgFolderSig  data;     /* signal struct data */

#ifdef WE_LOG_FC 
    WE_LOG_FC_BEGIN(MMSif_createMessageDone)
    WE_LOG_FC_UINT8(source, NULL); 
    WE_LOG_FC_UINT32(msgId, NULL); 
    WE_LOG_FC_INT32(folder, NULL); 
    WE_LOG_FC_PRE_IMPL
#endif

    WE_LOG_SIG_BEGIN("MMS_SIG_MSG_DONE", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    WE_LOG_SIG_END();

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
        "MMSlib_createMessageDone(%d)\n", msgId));
    data.msgId = msgId;
    data.folder = folder;

    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_MSG_DONE, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsgFolder))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */
#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif
} /* MMSlib_createMessageDone */

/*
 * Deletes a stored message.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_deleteMessage(WE_UINT8 source, MmsMsgId msgId)
{
    MmsMsgSig  data;     /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_deleteMessage)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_SIG_BEGIN("MMS_SIG_DELETE_MSG", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    WE_LOG_SIG_END();

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
        "MMSif_deleteMessage(%d)\n", msgId));
    data.msgId = msgId;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_DELETE_MSG, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_deleteMessage */

/*
 * Creates and sends a forward request to a MMS-Proxy-Relay.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
MmsResult MMSif_forwardReq(WE_UINT8 source, const MmsForward *forward, 
     char *contentLocation, MmsVersion version)
{
    MmsPduSig data;             /* pointer to the created PDU */
    WE_UINT32 size;
    MmsResult result;

    #ifdef WE_LOG_FC
        MmsForward *forwardDummy = (MmsForward *) forward;
        WE_LOG_FC_BEGIN(MMSif_forwardReq)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_STRING( contentLocation, NULL);
        if ( strlen( contentLocation) < 1)
        {
            contentLocation = NULL;
        } /* if */
        WE_LOG_FC_INT( version, NULL); 
        logMmsForward( WE_LOG_FC_PARAMS_IMPLEMENT, forwardDummy);
        WE_LOG_FC_PRE_IMPL
    #endif
    
    WE_LOG_SIG_BEGIN("MMS_SIG_FORWARD_REQ", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_END();   
    
   /* The Forward request PDU will be created here and sent to the 
    * MMS Service as a bit vector.  
    */ 

    /* One of the address fields must be present in the header
    * (To, Bcc. Cc) and from and content location.
    */ 
    if ((forward->to  == NULL) &&
        (forward->bcc == NULL) &&
        (forward->cc  == NULL) )
    {
        /* good bye */ 
        return MMS_RESULT_INVALID_ADDRESS;  
    } /* if */
    
    if (version!= MMS_VERSION_11)
    {
        /* A forward req. can not be sent in 1.0 PDU's */ 
         WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Forward Req. cannot be created in 1.0 version\n",
            __FILE__, __LINE__));
        return MMS_RESULT_INVALID_VERSION;
    } /* if */
    
    if (contentLocation ==  NULL)
    {
        /* Must be set */ 
         WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Content Location must be set in Forward Req.\n",
            __FILE__, __LINE__));
        return MMS_RESULT_URI_NOT_VALID;     
    } /* if */
    
    /* 
     * Build the forward request PDU, the memory is 
     * allocated in the function. 
     */
    data.pdu = createWspForward(source, forward, contentLocation, 
        &size, &result, version); 

    data.length = size; /* set the size of the pdu */

    if (result !=  MMS_RESULT_OK) 
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Failed to create Froward PDU.\n", __FILE__, __LINE__));
        return result;
    } /* if */

    /* Pack and send the PDU in a signal */ 
    if ( !mmsSendSignalExtArray( source, WE_MODID_MMS, MMS_SIG_FORWARD_REQ, 
        &data, (MmsIfConvertFunctionArray *)mms_cvt_MmsSendPdu, size))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */
    WE_MEM_FREE(source, data.pdu);
    return MMS_RESULT_OK;
}  /* MMSif_forwardReq */

/*
 * Get information about messages in a folder.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_getFolderContent(WE_UINT8 source, MmsFolderType folderId, 
      int from, int to)
{
    MmsMsgContentReq data; /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_getFolderContent)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_INT(folderId, NULL); 
        WE_LOG_FC_INT(from, NULL); 
        WE_LOG_FC_INT(to, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
 
    WE_LOG_SIG_BEGIN("MMS_SIG_GET_FLDR", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT16("folderId", folderId);
    WE_LOG_SIG_UINT32("from", from);
    WE_LOG_SIG_UINT32("to", to);
    WE_LOG_SIG_END();  

    data.folderId = folderId; 
    data.from = from;
    data.to = to;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_GET_FLDR, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsgContentReq))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_getFolderContent */

/*
 * Get information about a specific message.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_getMessageInfo(WE_UINT8 source, WE_UINT32 msgId)
{
    MmsMsgSig data; /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_getMessageInfo)
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
 
    WE_LOG_SIG_BEGIN("MMS_SIG_GET_MSG_INFO", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("msgId", msgId);
    WE_LOG_SIG_END();  

    data.msgId =  msgId;

    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_GET_MSG_INFO, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_getMessageInfo */



 /*
  * Request the folder status.
  *
  * Please note: Doxygen dokumentation, see file description in top of file.
*****************************************************************************/
void MMSif_getFolderStatus(WE_UINT8 source, MmsFolderType folderId)
{
    MmsFolderSig data;                          /* signal struct data */
    
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(MMSif_getFolderStatus)
        WE_LOG_FC_UINT8(source, NULL); 
    WE_LOG_FC_INT(folderId, NULL); 
    WE_LOG_FC_PRE_IMPL
#endif
        
        WE_LOG_SIG_BEGIN("MMS_SIG_FOLDER_STATUS", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_END();
    data.source = source;
    data.folderId = folderId;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_FOLDER_STATUS, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsFolderId))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif
} /* MMSif_getFolderStatus */
/*
 * Get the number of messages in a folder.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_getNumberOfMessages(WE_UINT8 source, MmsFolderType folder)
{
    MmsFolderSig data; /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_getNumberOfMessages)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_INT(folder, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
    
    WE_LOG_SIG_BEGIN("MMS_SIG_GET_NUM_OF_MSG", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("Folder",folder);
    WE_LOG_SIG_END();

    data.source = source;
    data.folderId = folder;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_GET_NUM_OF_MSG, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsFolderId))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_getNumberOfMessages */

/*
 * Get the conent of a read report(PDU 1.X) or or a delivery report 
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_getReport(WE_UINT8 source, MmsMsgId msgId)
{
    MmsMsgSig data;     /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_getReport)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_SIG_BEGIN("MMS_SIG_GET_REPORT", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    WE_LOG_SIG_END();

    data.msgId = msgId;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_GET_REPORT, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_getReport */

/*
 * Move a message between folders.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_moveMessage(WE_UINT8 source, MmsMsgId msgId, MmsFolderType toFolder)
{
    MmsMsgFolderSig data; /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_moveMessage)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_INT(toFolder, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
 
    WE_LOG_SIG_BEGIN("MMS_SIG_MOVE_MSG", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    WE_LOG_SIG_UINT32("MmsFolderType",toFolder);
    WE_LOG_SIG_END();
    
    data.msgId = msgId; 
    data.folder = toFolder;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_MOVE_MSG, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsgFolder))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_moveMessage */

/*
 * Cancel an ongoing Retrieve operation for a message. 
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_retrieveCancel(WE_UINT8 source, MmsMsgId notifId)
{
    MmsMsgSig  data;    /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_retrieveCancel)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(notifId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_SIG_BEGIN("MMS_SIG_RETRIEVE_MSG_CANCEL", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",notifId);
    WE_LOG_SIG_END();
    
    data.msgId = notifId;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_RETRIEVE_MSG_CANCEL,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_retrieveCancel */

/*
 * Retrieve a message from the MMS server.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_retrieveMessage(WE_UINT8 source, MmsMsgId notifId)
{
    MmsMsgSig  data;    /* signal struct data */
    
    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_retrieveMessage)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(notifId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_SIG_BEGIN("MMS_SIG_RETRIEVE_MSG", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",notifId);
    WE_LOG_SIG_END();

    data.msgId = notifId; 
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_RETRIEVE_MSG, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_retrieveMessage */

/*
 * Cancel an ongoing Retrieve operation for a message.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_sendCancel(WE_UINT8 source, MmsMsgId msgId)
{
    MmsMsgSig  data;    /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_sendCancel)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
 
    WE_LOG_SIG_BEGIN("MMS_SIG_SEND_MSG_CANCEL", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    WE_LOG_SIG_END();

    data.msgId = msgId; 
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_SEND_MSG_CANCEL, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_sendCancel */

/*
 * Sends a Multimedia Message
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_sendMessage(WE_UINT8 source, MmsMsgId msgId)
{
    MmsMsgSig data;     /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_sendMessage)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_SIG_BEGIN("MMS_SIG_SEND_MSG", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    WE_LOG_SIG_END();

    data.msgId = msgId;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_SEND_MSG, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_sendMessage */

/*
 * Sends a Read Report for a Multimedia Message
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_sendReadReport(WE_UINT8 source, MmsMsgId msgId,
    MmsReadStatus readStatus)
{
    MmsSendReadReport data; /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_sendReadReport)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_INT(readStatus, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
 
    WE_LOG_SIG_BEGIN("MMS_SIG_READ_REPORT", LS_SEND, source, (WE_UINT8)WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    WE_LOG_SIG_UINT32("ReadStatus",readStatus);
    WE_LOG_SIG_END();  

    data.msgId = msgId; 
    data.readStatus = readStatus;
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_READ_REPORT, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsSendReadReport))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_sendReadReport */

/*
 * Sets the read mark for a specified message Id 
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_setReadMark(WE_UINT8 source, MmsMsgId msgId, WE_UINT8 value)
{
    MmsSetReadMark  data; /* signal struct data */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_setReadMark)
        WE_LOG_FC_UINT8(source, NULL); 
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_UINT8(value, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
 
    WE_LOG_SIG_BEGIN("MMS_SIG_SET_READ_MARK", LS_SEND, source, (WE_UINT8) WE_MODID_MMS);
    WE_LOG_SIG_UINT32("MmsMsgId",msgId);
    /* WE_LOG_SIG_UINT8("Value",value); */
    WE_LOG_SIG_END();

    data.msgId = msgId; 
    data.value = value; 
    if ( !mmsSendSignalExt( source, WE_MODID_MMS, MMS_SIG_SET_READ_MARK, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsSetReadMark))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_setReadMark */

/*
 * Starts a this API 
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSif_startInterface(void)
{
    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(MMSif_startInterface)
        WE_LOG_FC_PRE_IMPL
    #endif
    
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
        "MMSif_startInterface(void)\n"));
    WE_SIGNAL_REG_FUNCTIONS(WE_MODID_MMS, mmsConvert, mmsDestruct);

    
    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
} /* MMSif_startInterface */

/*
 * \brief Used to send signals to (external) modules.
 *
 * \param senderModId   Who's requesting the send operation
 * \param destModule    Where to send to
 * \param signal        What signal to send
 * \param data          Data to send
 * \param cvtFunPtr     Function to convert data to a signal buffer
 * \result TRUE if signal successfully sent, FALSE otherwise
 *****************************************************************************/
WE_BOOL mmsSendSignalExt(WE_UINT8 senderModId, WE_UINT8 destModule,
    WE_UINT16 signal, void *data, MmsIfConvertFunction *cvtFunPtr) 
{
    we_dcvt_t  cvtObj;         /* memory signal object */ 
    void*       userData;       /* temp pointer for user data */
    void*       sigBuffer;      /* data to send */
    WE_UINT16  length = 0;     /* size of the signal buffer */
    WE_BOOL    ret = FALSE;
    
    if (cvtFunPtr == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): null function pointer!\n", 
            __FILE__, __LINE__));
        return FALSE;
    } /* if */
    
    /* Calculate the size of the data part */
    we_dcvt_init(&cvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    if (!cvtFunPtr(&cvtObj, data))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", 
            __FILE__, __LINE__));
        return FALSE;
    } /* if */

    length = (WE_UINT16)cvtObj.pos;
        
    /* Create and send signal */
    sigBuffer = WE_SIGNAL_CREATE(signal, senderModId, destModule, length);
    
    if (sigBuffer == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): WE_SIGNAL_CREATE failed.\n", 
            __FILE__, __LINE__));
        WE_ERROR( WE_MODID_MMS, WE_ERR_SYSTEM_FATAL);
    }
    else
    {   /* Add the data */
        userData = WE_SIGNAL_GET_USER_DATA( sigBuffer, &length);
        we_dcvt_init( &cvtObj, WE_DCVT_ENCODE, userData, length, senderModId);
        if (!cvtFunPtr(&cvtObj, data))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", 
                __FILE__, __LINE__));
            
            WE_SIGNAL_DESTRUCT(senderModId, signal, sigBuffer);
        }
        else
        {
            WE_SIGNAL_SEND(sigBuffer);
            ret = TRUE;
        }  /* if */
    } /* if */
    
    return ret;
} /* mmsSendSignalExt */

/*
 * \brief Used to send signals containing arrays to (external) modules.
 *
 * \param senderModId   Who's requesting the send operation
 * \param destModule    Where to send to
 * \param signal        What signal to send
 * \param data          Data to send
 * \param cvtFunPtr     Function to convert data to a signal buffer
 * \param size          Size of array
 * \result TRUE if signal successfully sent, FALSE otherwise
 *****************************************************************************/
WE_BOOL mmsSendSignalExtArray(WE_UINT8 senderModId, WE_UINT8 destModule,
    WE_UINT16 signal, void *data, MmsIfConvertFunctionArray *cvtFunPtr,
    WE_UINT32 size)
{
    we_dcvt_t  cvtObj;         /* memory signal object */ 
    void*       userData;       /* temp pointer for user data */
    void*       sigBuffer;      /* data to send */
    WE_UINT16  length = 0;     /* size of the signal buffer */
    WE_BOOL    ret = FALSE;
    
    if (cvtFunPtr == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): null function pointer!\n", 
            __FILE__, __LINE__));
        return FALSE;
    } /* if */
    
    /* Calculate the size of the data part */
    we_dcvt_init(&cvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    if (!cvtFunPtr(&cvtObj, data, size))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", 
            __FILE__, __LINE__));
        return FALSE;
    } /* if */

    length = (WE_UINT16)cvtObj.pos;
        
    /* Create and send signal */
    sigBuffer = WE_SIGNAL_CREATE(signal, senderModId, destModule, length);
    
    if (sigBuffer == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): WE_SIGNAL_CREATE failed.\n", 
            __FILE__, __LINE__));
        WE_ERROR( WE_MODID_MMS, WE_ERR_SYSTEM_FATAL);
    }
    else
    {   /* Add the data */
        userData = WE_SIGNAL_GET_USER_DATA( sigBuffer, &length);
        we_dcvt_init( &cvtObj, WE_DCVT_ENCODE, userData, length, senderModId);
        if (!cvtFunPtr(&cvtObj, data, size))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", 
                __FILE__, __LINE__));
            
            WE_SIGNAL_DESTRUCT(senderModId, signal, sigBuffer);
        }
        else
        {
            WE_SIGNAL_SEND(sigBuffer);
            ret = TRUE;
        }  /* if */
    } /* if */
    
    return ret;
} /* mmsSendSignalExtArray */

/*
 * \brief Used to send signals which has no data to (external) modules.
 *
 * \param senderModId   Who's requesting the send operation
 * \param destModule    Where to send to
 * \param signal        What signal to send
 * \result TRUE if signal successfully sent, FALSE otherwise
 *****************************************************************************/
WE_BOOL mmsSendSignalExtNoData(WE_UINT8 senderModId, WE_UINT8 destModule,
    WE_UINT16 signal) 
{
    we_dcvt_t  cvtObj;         /* memory signal object */ 
    void*       userData;       /* temp pointer for user data */
    void*       sigBuffer;      /* data to send */
    WE_UINT16  length = 0;     /* size of the signal buffer */
    
    /* Calculate the size of the data part */
    we_dcvt_init(&cvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    length = (WE_UINT16)cvtObj.pos;
    
    /* Create and send signal */
    sigBuffer = WE_SIGNAL_CREATE(signal, senderModId, destModule, length);
    
    if (sigBuffer == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): WE_SIGNAL_CREATE failed.\n", 
            __FILE__, __LINE__));
        WE_ERROR( WE_MODID_MMS, WE_ERR_SYSTEM_FATAL);
    }
    else
    {    
        userData = WE_SIGNAL_GET_USER_DATA(sigBuffer, &length);
        we_dcvt_init(&cvtObj, WE_DCVT_ENCODE, userData, length, senderModId);
        
        WE_SIGNAL_SEND (sigBuffer);
    } /* if */
    
    return sigBuffer != NULL;
} /* mmsSendSignalExtNoData */

/*!
 * \brief Fetches the message-header from a message.
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #MMS_SIG_GET_HEADER_REPLY
 *      data type    #MmsReplyGetHeader
 *
 * \param source     The calling module identity
 * \param msgId      The message containing the body-part
*****************************************************************************/
void MMSif_getMsgHeader(WE_UINT8 source,
                       MmsMsgId msgId,
                       WE_UINT32 userData)
{
    MmsGetMsgRequest data; /* signal struct data */
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(MMSif_getMsgHeader)
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_UINT32(userData, NULL);
    WE_LOG_FC_PRE_IMPL
#endif
        
    WE_LOG_SIG_BEGIN("MMS_SIG_GET_MSG_HEADER", LS_SEND, source, (WE_UINT8)WE_MODID_MMS);
        WE_LOG_SIG_UINT32("msgId", msgId); 
        WE_LOG_SIG_UINT32("userData", userData);
    WE_LOG_SIG_END();
    
    data.msgId = msgId;
    data.userData = userData;
    
    if ( !mmsSendSignalExt(source, WE_MODID_MMS,
            MMS_SIG_GET_MSG_HEADER, 
            &data,
            (MmsIfConvertFunction*)mms_cvt_MmsGetMsgRequest))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif
} /* MMSif_getMsgHeader */

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
                          WE_UINT32 userData)
{
    MmsGetMsgRequest data; /* signal struct data */
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(MMSif_getMsgSkeleton)
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_UINT32(userData, NULL);
    WE_LOG_FC_PRE_IMPL
#endif
        
    WE_LOG_SIG_BEGIN("MMS_SIG_GET_MSG_SKELETON", LS_SEND, source, (WE_UINT8)WE_MODID_MMS);
        WE_LOG_SIG_UINT32("msgId", msgId); 
        WE_LOG_SIG_UINT32("userData", userData);
    WE_LOG_SIG_END();
    
    data.msgId = msgId;
    data.userData = userData;
    
    if ( !mmsSendSignalExt(source, WE_MODID_MMS,
            MMS_SIG_GET_MSG_SKELETON, 
            &data,
            (MmsIfConvertFunction*)mms_cvt_MmsGetMsgRequest))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif        
}

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
                       MmsMsgId msgId,
                       WE_UINT16 bodyPartId, 
                       WE_UINT32 userData)
{
    MmsGetBodyPartRequest data; /* signal struct data */
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(MMSif_getBodyPart)
        WE_LOG_FC_UINT32(msgId, NULL); 
        WE_LOG_FC_UINT32(userData, NULL);
        WE_LOG_FC_UINT16(bodyPartId, NULL);
    WE_LOG_FC_PRE_IMPL
#endif
        
    WE_LOG_SIG_BEGIN("MMS_SIG_GET_BODY_PART", LS_SEND, source, (WE_UINT8)WE_MODID_MMS);
        WE_LOG_SIG_UINT32("msgId", msgId); 
        WE_LOG_SIG_UINT32("userData", userData);
        WE_LOG_SIG_UINT16("bodyPartId", bodyPartId);
    WE_LOG_SIG_END();
    
    data.msgId = (WE_UINT32) msgId;
    data.userData = userData;
    data.bodyPartId = bodyPartId;
    
    if ( !mmsSendSignalExt(source, WE_MODID_MMS,
        MMS_SIG_GET_BODY_PART, 
        &data,
        (MmsIfConvertFunction*)mms_cvt_MmsGetBodyPartRequest))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } /* if */
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif       
}


#ifdef WE_LOG_FC  

WE_LOG_FC_DISPATCH_MAP_BEGIN(mmsif)

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_startInterface)
MMSif_startInterface();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_createMessage)
MMSif_createMessage( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_deleteMessage)
MMSif_deleteMessage( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_getNumberOfMessages)
MMSif_getNumberOfMessages( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_moveMessage)
MMSif_moveMessage( 0, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_retrieveCancel)
MMSif_retrieveCancel( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_retrieveMessage)
MMSif_retrieveMessage( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_sendCancel)
MMSif_sendCancel( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_sendMessage)
MMSif_sendMessage( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_sendReadReport)
MMSif_sendReadReport( 0, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_createMessageDone)
MMSif_createMessageDone( 0, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_getFolderContent)
MMSif_getFolderContent( 0, 0, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_getMessageInfo)
MMSif_getMessageInfo( 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_setReadMark)
MMSif_setReadMark( 0, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSif_forwardReq)
    
    enum {MAX_PARAMS = 2};
    char strings[19][1024];
    MmsAddressList bccList[MAX_PARAMS];
    MmsAddressList ccList[MAX_PARAMS];
    MmsAddress from;
    MmsAddressList toList[MAX_PARAMS];
    MmsForward forward;
    
    memset(&forward, 0x00, sizeof(forward));
    memset( strings, 0x00, sizeof(strings));

    /* BCC */
    memset( bccList, 0x00, sizeof(bccList));
    bccList[0].current.address = &strings[0][0];
    bccList[1].current.address = &strings[1][0];
    bccList[0].current.name.text = &strings[2][0];
    bccList[1].current.name.text = &strings[3][0];
    bccList[0].next = &bccList[1];
    forward.bcc = &bccList[0];

    /* CC */
    memset( ccList, 0x00, sizeof(ccList));
    ccList[0].current.address = &strings[4][0];
    ccList[1].current.address = &strings[5][0];
    ccList[0].current.name.text = &strings[6][0];
    ccList[1].current.name.text = &strings[7][0];
    ccList[0].next = &ccList[1];
    forward.cc = &ccList[0];

    /* TO */
    memset( toList, 0x00, sizeof(toList));
    toList[0].current.address = &strings[8][0];
    toList[1].current.address = &strings[9][0];
    toList[0].current.name.text = &strings[10][0];
    toList[1].current.name.text = &strings[11][0];
    toList[0].next = &toList[1];
    forward.to = &toList[0];

    /* FROM */
    memset( &from, 0x00, sizeof(from));
    forward.from.name.text = &strings[12][0];
    forward.from.address = &strings[13][0];

    MMSif_forwardReq( 0, &forward, &strings[14][0], 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_SUB_DISPATCH_MAP(mmslib)

WE_LOG_FC_DISPATCH_MAP_END
#endif 

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
/* MMS */
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Core.h"   /* WE: System core call */ 
#include "We_Dcvt.h"   /* WE: Type conversion routines */ 
#include "We_Mem.h"    /* WE: Memory handling */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Wid.h"    /* WE: Signal logging */ 

/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_def.h"

/* SMA */
#include "Sia_if.h"
#include "Satypes.h"
#include "Samem.h"
#include "Saintsig.h"
#include "Sasls.h"

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

typedef struct 
{
    SiaStateMachine retFsm;
    int retSig;
    SiaSmilInfo *slideRsp;
} GetSlideInfoInstance;

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/
static GetSlideInfoInstance instData;


/*--- Prototypes ---*/
static void freeInstanceData(GetSlideInfoInstance *inst);

/*--- Functions ---*/

/*!\brief Init Sls instance
 *****************************************************************************/
void smaSisInit(void)
{
    memset(&instData, 0x00, sizeof(GetSlideInfoInstance));
}

/*!
 * \brief Terminate Sls instance
 *****************************************************************************/
void smaSlsTerminate(void)
{
    freeInstanceData(&instData);
}


void smaFreeSiaSmilInfo(SiaSmilInfo *smilInfo)
{
    if (smilInfo->regionList != NULL)
    {
        SISif_freeSlsRegionList(WE_MODID_SIA, smilInfo->regionList);
        SIA_FREE(smilInfo->regionList);
        smilInfo->regionList = NULL;
    } /* if */
    
    if (smilInfo->slideInfoList != NULL)
    {
        SISif_freeSlsSlideInfoList(WE_MODID_SIA, smilInfo->slideInfoList);
        SIA_FREE(smilInfo->slideInfoList);
        smilInfo->slideInfoList = NULL;
    } /* if */

    if (smilInfo->objectInfoList != NULL)
    {
        smaFreeSiaObjectInfoList(smilInfo->objectInfoList);
        SIA_FREE(smilInfo->objectInfoList);
        smilInfo->objectInfoList = NULL;
    } /* if */
}

void smaFreeSiaObjectInfo(SiaObjectInfo *objInfo)
{
    if (objInfo->handle != 0)
    {
        /* release handle if loaded */
        (void)WE_WIDGET_RELEASE(objInfo->handle);
    } /* if */
    if (objInfo == NULL)
    {
        return;
    }
    SISif_freeSlsObjectInfo(WE_MODID_SIA, &objInfo->obj);
} /* smaFreeSiaObjectInfo */

void smaFreeSiaObjectInfoList(SiaObjectInfoList *objList)
{
    SiaObjectInfoList *start = objList;
    SiaObjectInfoList *prev;
    
    while (objList)
    {
        smaFreeSiaObjectInfo(&objList->current);
        prev = objList;
        objList = objList->next;
        if (prev != start)
        {
            SIA_FREE(prev);
        }
    }
} /* smaFreeSiaObjectInfoList */


static void freeInstanceData(GetSlideInfoInstance *inst)
{
    if (inst == NULL)
    {
        return;
    }

    if (inst->slideRsp != NULL)
    {
        /* free SiaSmilInfo */
        smaFreeSiaSmilInfo(inst->slideRsp);
        SIA_FREE(inst->slideRsp);
    } /* if */
    memset(inst, 0x00, sizeof(GetSlideInfoInstance));
}

static void sendLoadRsp(const GetSlideInfoInstance *instance, 
    SiaResult result)
{
    if (result != SIA_RESULT_OK)
    {
        (void)SIA_SIGNAL_SENDTO_IP(instance->retFsm, instance->retSig, result, 
        NULL);
    }
    else
    {
        (void)SIA_SIGNAL_SENDTO_IP(instance->retFsm, instance->retSig, result, 
        instance->slideRsp);
    }

} /* sendLoadRsp */


void smaLoadSlideInfo(SiaStateMachine retFsm, int retSig, WE_UINT32 msgId)
{
    /* free any old data if it exists */
    freeInstanceData(&instData);
    
    instData.retFsm = retFsm;
    instData.retSig = retSig;
    instData.slideRsp = SIA_CALLOC(sizeof(SiaSmilInfo));

    SISif_openSmil(WE_MODID_SIA, msgId, 0);
} /* smaLoadSlideInfo */


static SiaResult convertSlsToSiaResult(SisResult result)
{
    switch(result) 
    {
    case SIS_RESULT_OK:
    	return SIA_RESULT_OK;
    case SIS_RESULT_BUSY:
    	return SIA_RESULT_BUSY;
    case SIS_RESULT_TO_MANY_SLIDES:
    	return SIA_RESULT_TO_MANY_SLIDES;
    case SIS_RESULT_MISSING_BODY_PART:
    case SIS_RESULT_PARSING_FAILED:
    case SIS_RESULT_BUFFER_FULL:
    case SIS_RESULT_FILE_READ_FAILURE:
    case SIS_RESULT_FILE_DELAY:
    case SIS_RESULT_SMIL_NOT_CREATED:
    case SIS_RESULT_ERROR:
    case SIS_RESULT_MISSING_MEDIA_OBJECTS:
    case SIS_RESULT_MISSING_MEDIA_OBJECT_SRC:
    case SIS_RESULT_SMIL_INVALID:
    default:
    	return SIA_RESULT_ERROR;
    }

} /* convertSlsToSiaResult */


/*! \brief handle information about the SMIL message and 
 *   prepare to retrieve media objects
 *  \return result of the operation
 *****************************************************************************/
void smaHandleOpenSmilRsp(const SisOpenSmilRsp *openSmilRsp)
{
    if (openSmilRsp == NULL || openSmilRsp->objectList == NULL || 
        openSmilRsp->result != SIS_RESULT_OK)
	{
		WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
			"SMA: SIS failed to open SMIL in SIS_OPEN_SMIL_RSP\n"));
        
        if (openSmilRsp)
        {
            sendLoadRsp(&instData, convertSlsToSiaResult(openSmilRsp->result));
        }
        else
        {
            sendLoadRsp(&instData, SIA_RESULT_ERROR);
        }
        freeInstanceData(&instData);
        return;
	} /* if */

    instData.slideRsp->noOfSlides = openSmilRsp->noOfSlides;
    SISif_copySlsRootLayout(&openSmilRsp->rootLayout, 
        &instData.slideRsp->rootLayout);


    if (openSmilRsp->regionList != NULL)
    {
        instData.slideRsp->regionList = SIA_CALLOC(sizeof(SisRegionList));
        SISif_copySlsRegionList(WE_MODID_SIA, openSmilRsp->regionList, 
            instData.slideRsp->regionList);
    } /* if */
    if (openSmilRsp->objectList != NULL)
    {
        instData.slideRsp->objectInfoList = 
            SIA_CALLOC(sizeof(SiaObjectInfoList));
        
        smaCopySlsObjectInfoList(openSmilRsp->objectList, 
            instData.slideRsp->objectInfoList);
    } /* if */
    
    /* get slide info - wait for signal */
    SISif_getSlideInfo(WE_MODID_SIA, 0, openSmilRsp->noOfSlides, 0);
} /* smaHandleOpenSmilRsp */


void smaCopySlsObjectInfo(const SisObjectInfo *source, SiaObjectInfo *dest)
{

    if (source == NULL || dest == NULL)
    {
        return;
    } /* if */

    dest->handle = 0;
    SISif_copyObjectInfo(WE_MODID_SIA, source, &dest->obj);
} /* smaCopySlsObjectInfo */


void smaCopySlsObjectInfoList(const SisObjectInfoList *source, 
    SiaObjectInfoList *dest)
{
    if (source == NULL || dest == NULL)
    {
        return;
    }

    while (source)
    {
        smaCopySlsObjectInfo(&source->current, 
            &dest->current);
        if (source->next != NULL)
        {
            dest->next = SIA_CALLOC(sizeof(SiaObjectInfoList));
            dest = dest->next;
        } /* if */
        
        source = source->next;
    } /* while */

} /* smaCopySlsObjectInfoList */



/*! \brief Handles the response from the SlsGetSlideInfo call.
 *  \param slsInfo (IN): The information about the SMIL
 *  \param slide (OUT): The information converted for use with Sia
 *  \return TRUE on success or if the module is of no importance. FALSE on error.
 *****************************************************************************/
void smaHandleGetSlideInfoRsp(const SisGetSlideInfoRsp *getSlideInfoRsp)
{
    if (getSlideInfoRsp == NULL || getSlideInfoRsp->slideInfo == NULL ||
        getSlideInfoRsp->result != SIS_RESULT_OK)
    {
        sendLoadRsp(&instData, SIA_RESULT_ERROR);
        freeInstanceData(&instData);
        return;
    } /* if */
    
    if (getSlideInfoRsp->slideInfo != NULL)
    {
        instData.slideRsp->slideInfoList = 
            SIA_CALLOC(sizeof(SisSlideInfoList));

        SISif_copySlideInfoList(WE_MODID_SIA, getSlideInfoRsp->slideInfo, 
            instData.slideRsp->slideInfoList);
    } /* if */
    
    /* done */
    sendLoadRsp(&instData, SIA_RESULT_OK);
    /* don't free RSP data */
    instData.slideRsp = NULL;
    freeInstanceData(&instData);
} /* smaHandleGetSlideInfoRsp */


WE_INT16 smaCvtSlsSizeValue(const SisSizeValue *sizeVal, WE_INT16 screenSize)
{
    WE_INT16 size;
    
    if (sizeVal->type == SIS_REL)
    {
        /* size is relative - convert to absolute */
        size = (WE_INT16)((sizeVal->value * screenSize) / 100);
    }
    else
    {
        size = (WE_INT16)sizeVal->value;
    } /* if */

    return size;
} /* smaCvtSlsSizeValue */

void smaCvtSlsSizeToSize(const SisSize *src, WeSize *dst, 
    const WeSize *dispSize)
{
    if (src == NULL || dst == NULL || dispSize == NULL)
    {
        return;
    } /* if */
    
    dst->height = smaCvtSlsSizeValue(&src->cy, dispSize->height);
    dst->width = smaCvtSlsSizeValue(&src->cx, dispSize->width);
} /* smaCvtSlsSizeToSize */

void smaCvtSlsSizeToPos(const SisSize *src, WePosition *dst, 
    const WeSize *dispSize)
{
    if (src == NULL || dst == NULL || dispSize == NULL)
    {
        return;
    } /* if */
    
    dst->y = smaCvtSlsSizeValue(&src->cy, dispSize->height);
    dst->x = smaCvtSlsSizeValue(&src->cx, dispSize->width);
} /* smaCvtSlsSizeToPos */


SiaObjectInfo *smaGetObjectInfo(WE_UINT32 bpId, SiaObjectInfoList *objList)
{
    while (objList)
    {
        if (objList->current.obj.id == bpId)
        {
            return &objList->current;
        } /* if */
        objList = objList->next;
    } /* while */

    return NULL;
} /* smaGetObjectInfo */


const SisRegion *smaGetRegion(WE_UINT32 regionId, const SisRegionList *regionList)
{
    while (regionList)
    {
        if (regionList->id == regionId)
        {
            return &regionList->region;
        } /* if */
        regionList = regionList->next;
    } /* while */

    return NULL;
} /* smaGetRegion */


SisSlideInfo *smaGetSlideInfo(WE_UINT32 slideNo, 
    SisSlideInfoList *slideInfoList)
{
    WE_UINT32 i = 0;

    while (slideInfoList)
    {
        if (i == slideNo)
        {
            return &slideInfoList->current;
        } /* if */
        slideInfoList = slideInfoList->next;
        i++;
    } /* while */

    /* item not found */
    return NULL;
} /* smaGetSlideInfo */

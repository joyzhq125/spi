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
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Wid.h"    /* WE: ANSI Standard libs allowed to use */

/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_def.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SMA */
#include "Saintsig.h"
#include "Sia_if.h"
#include "Satypes.h"
#include "Samain.h"
#include "Sauiform.h"
#include "Saslide.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Saload.h"
#include "Sasls.h"
#include "Saui.h"
#include "Sadsp.h"
#include "Sia_rc.h"
#include "Sia_cfg.h"

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/* \struct SlideInstanceData Used to hold information between states */
typedef struct
{
    WE_UINT32              msgId;
    SiaStateMachine         callingFsm;     /*<! The calling FSM */
    int                     returnSig;      /*<! The return signal */
    SisSlideElementList     *elementPtr;
    SiaSlideFormHandle      formHandle;
    WE_UINT32              duration;
}SlideInstanceData;

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

static SlideInstanceData instData;

/******************************************************************************
 * Function prototypes
 *****************************************************************************/
static void smaSlideMain(SiaSignal *sig);
static void freeSlideInstanceData(SlideInstanceData *instance);
static void handleLoadObjectRsp(WE_UINT32 handle);
static void loadObject(WE_UINT32 msgId, const SisSlideElementList *elementPtr);

/*!
 *\brief Initiates the SMA slide handler
 *
 *****************************************************************************/
void smaSlideInit(void)
{
    smaSignalRegisterDst(SIA_FSM_SLIDE, smaSlideMain);
    memset(&instData, 0x00, sizeof(SlideInstanceData));
} /* smaSbpInit */


/*!
 *\brief Terminates the content routing handler
 *
 *****************************************************************************/
void smaSlideTerminate(void)
{
    /* Remove any instance data */
    freeSlideInstanceData(&instData);
    smaSignalDeregister(SIA_FSM_SLIDE);
} /* smaSbpTerminate */


/*!
 *\brief Free slide instance data
 *****************************************************************************/
static void freeSlideInstanceData(SlideInstanceData *instance)
{
    if (instance == NULL)
    {
        return;
    } /* if */

    memset(instance, 0x00, sizeof(SlideInstanceData));
} /* freeSlideInstanceData */

/*!
 *\brief Signal-handler for the content routing FSM.
 *
 * \param sig The signal received.
 *****************************************************************************/
static void smaSlideMain(SiaSignal *sig)
{		
    if (NULL == sig)
    {
        /* no signal - just return */
        return;
    } /* if */

    switch (sig->type)
    {
    case SIA_SIG_SLIDE_LOAD_OBJ_RSP:
        handleLoadObjectRsp(sig->u_param1);
        break;
    default:
        /* unknown signal */
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA,
            "(%s) (%d) Unknown signal!\n", __FILE__, __LINE__));
        break;
    } /* switch */

    /* Deallocate the signal */
    smaSignalDelete(sig);
} /* smaSlideMain */


static void handleLoadObjectRsp(WE_UINT32 handle)
{
    WE_UINT32 formObjId;
    WeColor bgColor;
    WeColor fgColor;
    WE_UINT32 slsBgColor;
    WE_UINT32 slsFgColor;
    const WeColor *fColor = NULL;
    const WeColor *bColor = NULL;
    SiaObjectInfo *objInfo;
    WE_UINT32 tmpHandle;
    
    formObjId = 0;
    objInfo = smaGetObjectInfo((WE_UINT32)instData.elementPtr->current.objectId, 
        smaGetObjectInfoList());


    /* read background color */
    slsBgColor = SISlib_getColorValue(instData.elementPtr->current.paramList, 
        "background-color");
    
    if (
        (SIS_COLOR_NOT_USED != 
        (slsFgColor = SISlib_getColorValue(
        instData.elementPtr->current.paramList, "foreground-color"))) ||
        (SIS_COLOR_NOT_USED != 
        (slsFgColor = SISlib_getColorValue(
        instData.elementPtr->current.paramList, "foregroundcolor"))) ||
        (SIS_COLOR_NOT_USED != 
        (slsFgColor = SISlib_getColorValue(
        instData.elementPtr->current.paramList, "color"))) ||
		(SIS_COLOR_NOT_USED != 
        (slsFgColor = SISlib_getColorValue(
        instData.elementPtr->current.paramList, "textcolor")))
        )

    {
        /* do nothing */
    }

    
    if (smaIntegerToWeColor(slsBgColor, &bgColor))
    {
        bColor = &bgColor;
    }
	else if (smaIntegerToWeColor(smaGetRootLayout().bgColor , &bgColor))
	{
		bColor = &bgColor; /*TR_18628 if no specific background-color, use layout's bgColor*/
	} /* if */
    
    if (smaIntegerToWeColor(slsFgColor, &fgColor))
    {
       fColor = &fgColor;
    }

    if (objInfo == NULL || handle == 0)
    {
        /* object is not found - check alt text param, 
        else load broken image */
        switch (instData.elementPtr->current.objectType)
        {
        case SIS_OBJECT_TYPE_UNKNOWN:
        case SIS_OBJECT_TYPE_VIDEO:
        case SIS_OBJECT_TYPE_IMAGE:
        case SIS_OBJECT_TYPE_TEXT:
            /* show alt text or "broken image"*/
            if (instData.elementPtr->current.alt)
            {
                tmpHandle = smaCreateString(instData.elementPtr->current.alt);
                formObjId = smaAddTextObjectToForm(tmpHandle,
                    smaGetRegion(instData.elementPtr->current.regionId, 
                    smaGetRegionList()), fColor, bColor, 
                    (WE_UINT32)instData.elementPtr->current.objectId, 
                    instData.formHandle);
                (void)WE_WIDGET_RELEASE(handle);
            }
            else
            {
                /* show broken image */
                tmpHandle = SIA_GET_IMG_ID(SIA_IMG_BROKEN_IMAGE);
                formObjId = smaAddImageObjectToForm(tmpHandle,
                    smaGetRegion(instData.elementPtr->current.regionId, 
                    smaGetRegionList()), bColor, 
                    (WE_UINT32)instData.elementPtr->current.objectId, 
                    instData.formHandle);
            }
            
            break;
        case SIS_OBJECT_TYPE_AUDIO:
        case SIS_OBJECT_TYPE_REF:
        case SIS_OBJECT_TYPE_ATTACMENT:
        case SIS_OBJECT_TYPE_DRM:
        default:
            /* Do nothing, 
             */
            break;
        }
    }
    else
    {
   
            /* store handle in object */
        objInfo->handle = handle;

        switch (smaGetSlsMediaObjectType((const char *)
            objInfo->obj.contentType.strValue))
        {
        case SIS_OBJECT_TYPE_TEXT:
            /* Text */
            formObjId = smaAddTextObjectToForm(handle,
                smaGetRegion(instData.elementPtr->current.regionId, 
                smaGetRegionList()), fColor, bColor, 
                (WE_UINT32)instData.elementPtr->current.objectId, 
                instData.formHandle);
            break;
        case SIS_OBJECT_TYPE_IMAGE:
        case SIS_OBJECT_TYPE_VIDEO:
            /* Image or video */
            formObjId = smaAddImageObjectToForm(handle,
                smaGetRegion(instData.elementPtr->current.regionId, 
                smaGetRegionList()), bColor, 
                (WE_UINT32)instData.elementPtr->current.objectId,
                instData.formHandle);
            break;
        case SIS_OBJECT_TYPE_AUDIO:
            formObjId = smaAddAudioObjectToForm(handle, 
                (WE_UINT32)instData.elementPtr->current.objectId, 
                instData.formHandle);
            break;

        case SIS_OBJECT_TYPE_ATTACMENT:	    
        case SIS_OBJECT_TYPE_UNKNOWN:    
        case SIS_OBJECT_TYPE_DRM:
        case SIS_OBJECT_TYPE_REF:
        default:
            break;
        }
    }
    
    if (formObjId != 0)
    {
        /* check if duration or endT is used */
        if (instData.elementPtr->current.duration == 0)
        {
            instData.elementPtr->current.duration = SIA_MIN_SLIDE_DURATION;
        } /* if */

        if (instData.elementPtr->current.endT == 0 &&
            ((instData.elementPtr->current.duration + 
            instData.elementPtr->current.beginT) <= instData.duration))

        {
            instData.elementPtr->current.endT = 
                instData.elementPtr->current.beginT + 
                instData.elementPtr->current.duration;
        }
        else if (instData.elementPtr->current.endT == 0 &&
            ((instData.elementPtr->current.duration + 
            instData.elementPtr->current.beginT) > instData.duration))

        {
            instData.elementPtr->current.endT = instData.duration;
        }/* if */
            
            
            /* if */
        
        smaAddDispatchItem(formObjId, instData.elementPtr->current.beginT, 
            instData.elementPtr->current.endT);
    }
    
    /* load next object if any */
    if (instData.elementPtr->next != NULL)
    {
        instData.elementPtr = instData.elementPtr->next;
        loadObject(instData.msgId, instData.elementPtr);
    } 
    else
    {
        (void)SIA_SIGNAL_SENDTO(instData.callingFsm, instData.returnSig);
    } /* if */
    
} /* handleLoadObjectRsp */


void smaLoadSlide(WE_UINT32 msgId, WE_UINT8 number, 
    SisSlideInfoList *slideInfoList, const SiaSlideFormHandle formHandle, 
    SiaStateMachine callingFsm, int returnSig)
{
    SisSlideInfo *slideInfo;
    
    instData.callingFsm = callingFsm;
    instData.returnSig = returnSig;
    instData.formHandle = formHandle;
    instData.msgId = msgId;
    
    slideInfo = smaGetSlideInfo(number, slideInfoList);
    instData.elementPtr = slideInfo->elementList;
    instData.duration = slideInfo->duration;

    loadObject(instData.msgId, instData.elementPtr);

} /* smaLoadSlide */


static void loadObject(WE_UINT32 msgId, const SisSlideElementList *elementPtr)
{
    const SiaObjectInfo *objectInfo;

    objectInfo = smaGetObjectInfo((WE_UINT32)elementPtr->current.objectId, 
        smaGetObjectInfoList());

    smaLoadBpToHandle(SIA_FSM_SLIDE, SIA_SIG_SLIDE_LOAD_OBJ_RSP, 
        msgId, objectInfo);
} /* loadObject */

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
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Chrs.h"
#include "We_Tel.h"
#include "We_Mime.h"
#include "We_Act.h"
#include "We_Wid.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SMA */
#include "Sia_if.h"
#include "Sia_cfg.h"
#include "Samem.h"
#include "Satypes.h"
#include "Samain.h"
#include "Saintsig.h"
#include "Sauiform.h"
#include "Saslide.h"
#include "Sauidia.h"
#include "Sia_def.h"
#include "Sia_rc.h"
#include "Sasls.h"
#include "Saslide.h"
#include "Saui.h"
#include "Sadsp.h"
#include "Sacore.h"
#include "Sacrh.h"
#include "Smtr.h"
#include "Sasmnu.h"
#include "Saload.h"

/*--- Definitions/Declarations ---*/
/* Define the highest number a long can hold. 
   This is for function convertToUTF8() */
#define SIA_MAXIMUM_LONG_POSITIVE_VALUE	0x7FFFFFFF

/*! Defines the optional UTF-8 identifier */
#define SIA_UTF8_BOM                    "\xEF\xBB\xBF"

/*! Defines the first slide */

/*--- Types ---*/

/* \struct SiaPlayerInfo Static information */
typedef struct
{
    /* player information */
    WE_UINT32          msgId;          /*!< message id containing the SMIL */
    WE_UINT8           sourceModule;   /*!< source module that requested to play a SMIL */
    WE_UINT32          userData;       /*!< userData supplied when requesting to play smil*/
    
    WE_UINT32          selectedObj;    
    WE_BOOL            isPlaying;      /*!< is the state-machine in play mode or not*/
    
    SmtrLinkEntry       *selectedLink;
    /* display information */
    SiaSlideFormHandle  formHandle;       /*!< userData supplied when requesting to play smil*/

    /* SMIL information */
    WE_UINT8           noOfSlides;     /*!< number of slides in current SMIL */
    WE_UINT8           currentSlide;   /*!< current slide shown */
    
    SisRootLayout       rootLayout;     
    SisSlideInfoList    *slideInfoList;     /*!< information about the SMIL */
    SiaObjectInfoList   *objectList;        /*!< information about body parts */
    SisRegionList       *regionList;        /*!< information about regions */

    WE_INT32           result;             /*!< result when showing dialog */
} InstanceData;
/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

static const SiaMediaTypeItem supportedMediaTypes[] = SIA_CFG_SUPPORTED_MEDIA_TYPES;

/* Information about the SMIL player and the current state. */
static InstanceData instData;
/*--- Prototypes ---*/

static void signalHandler(SiaSignal *sig);
static void smaActionCallback(WE_UINT32 objId, SiaMoAction action);
static void handleObjAction(const SiaObjectInfo *objInfo, 
	const we_act_action_entry_t *actions, WE_UINT32 index);
static void freeInstanceData(InstanceData *inst);
static char *smaGenerateFileName(const char *extension);
static void updateRegion(void);
/*****************************************************************************
 * Exported functions                                                        *
 *****************************************************************************/

/*! \brief Initiates the SMA module. 
*****************************************************************************/
WE_BOOL smaMainInit(void)
{
    /* register signal handler */
    smaSignalRegisterDst(SIA_FSM_MAIN, signalHandler);
    memset(&instData, 0x00, sizeof(InstanceData));
    return TRUE;
} /* smaMainInit */

/*! \brief Terminate the SMA Core.
 *****************************************************************************/    
void smaMainTerminate(void)
{
    freeInstanceData(&instData);
} /* smaMainTerminate */

static void freeInstanceData(InstanceData *inst)
{
    smaStopSmil();

    smaDeleteSlideForm(inst->formHandle);

    if (inst->slideInfoList)
    {
        SISif_freeSlsSlideInfoList(WE_MODID_SIA, inst->slideInfoList);
        SIA_FREE(inst->slideInfoList);
    }
    if (inst->regionList)
    {
        SISif_freeSlsRegionList(WE_MODID_SIA, inst->regionList);
        SIA_FREE(inst->regionList);
    }
    if (inst->objectList)
    {
        smaFreeSiaObjectInfoList(inst->objectList);
        SIA_FREE(inst->objectList);
    }

    if (inst->selectedLink != NULL)
    {
        smtrFreeSmtrLinkEntry(&inst->selectedLink);
    }

    memset(inst, 0x00, sizeof(InstanceData));

} /* freeInstanceData */

void smaGenerateSlide(WE_UINT8 slideNo)
{
    WeColor bgColor;
    WeColor *bColor = NULL;
    SisSlideInfo *slideInfo;
    instData.currentSlide = slideNo;
    

    if (smaIntegerToWeColor(instData.rootLayout.bgColor, &bgColor))
    {
        bColor = &bgColor;
    }

    /* delete old form - if exists */
    if (instData.formHandle != 0)
    {
        smaDeleteSlideForm(instData.formHandle);
    } /* if */

    /* create new form */
    instData.formHandle = smaCreateSlideForm(bColor);
    
    /* delete old dispatcher */
    smaDeleteDispatcher();
    slideInfo = smaGetSlideInfo(instData.currentSlide, instData.slideInfoList);

    
    if (slideInfo != NULL)
    {
        if (slideInfo->duration == 0)
        {
            slideInfo->duration = SIA_MIN_SLIDE_DURATION;
        } /* if */

        (void)smaCreateDispatcher(smaActionCallback, 
            slideInfo->duration, 
            SIA_FSM_MAIN, SIA_SIG_MH_SLIDE_EXPIRE);
        smaLoadSlide(instData.msgId, slideNo, instData.slideInfoList, 
            instData.formHandle, SIA_FSM_MAIN, SIA_SIG_MH_LOAD_SLIDE_RSP);
    }

} /* smaGenerateSlide */


void smaHandlePlaySmilReq(WE_UINT8 module, WE_UINT32 msgId, 
    WE_UINT32 userData)
{
    instData.msgId = msgId;
    instData.sourceModule = module; 
    instData.userData = userData;

    setSiaIsRunning(TRUE);
    if (SIA_RESULT_OK == createGUI())
    {
        /* now open the SMIL message */
        smaLoadSlideInfo(SIA_FSM_MAIN,SIA_SIG_MH_LOAD_SLIDE_INFO_RSP, msgId);
    } 
    else
    {
        smaSendPlaySmilRsp(SIA_RESULT_ERROR);
    }/* if */
} /* smaHandlePlaySmilReq */



/*! \brief Sends result to calling module
 *  \param senderModule module to send result to
 *  \param result Result code to send to calling module
 *****************************************************************************/
void smaSendPlaySmilRsp(SiaResult result)
{
    we_dcvt_t      cvt;
    void           *sig;
    void           *sigData;
    WE_UINT16      length;
    SiaPlaySmilRsp  playRsp;

    playRsp.result = result;
    playRsp.userData = instData.userData;
    
    /* nothing to do if failed to stop playing */
    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    
    if (!Sia_cvt_SiaPlaySmilRsp(&cvt, &playRsp))
    {
        return;
    } /* if */

    length = (WE_UINT16)cvt.pos;

    sig = WE_SIGNAL_CREATE (SIA_SIG_PLAY_SMIL_RSP, WE_MODID_SIA, 
        instData.sourceModule, length);
    
    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);
    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, WE_MODID_SIA);
    
    if (!Sia_cvt_SiaPlaySmilRsp(&cvt, &playRsp))
    {
        return;
    } /* if */
    
    freeInstanceData(&instData);
    setSiaIsRunning(FALSE);


    WE_SIGNAL_SEND (sig);
} /* smaSendPlaySmilRsp */


/*!
 * \brief Signal handler
 * 
 * \param sig the signal
 *****************************************************************************/
static void signalHandler(SiaSignal *sig)
{
    SiaSmilInfo *loadRsp;
    const SiaObjectInfo *objInfo;
	we_act_action_entry_t *actions;
	WE_UINT32 actionCount;
    char *drmCt;

    switch (sig->type) 
    {
    case SIA_SIG_MH_LOAD_SLIDE_INFO_RSP:
        /* Message info loaded, start playing */
        if (sig->i_param == SIA_RESULT_OK)
        {
            loadRsp = (SiaSmilInfo *)sig->p_param;
            instData.slideInfoList = loadRsp->slideInfoList;
            instData.regionList = loadRsp->regionList;
		
            instData.objectList = loadRsp->objectInfoList;
            SISif_copySlsRootLayout(&loadRsp->rootLayout, &instData.rootLayout);
            instData.noOfSlides = loadRsp->noOfSlides;
            
            loadRsp->objectInfoList = NULL;
            loadRsp->regionList = NULL;
            loadRsp->slideInfoList = NULL;
            updateRegion();     
            SIA_FREE(loadRsp);
            /* start in mode */
            instData.isPlaying = TRUE;
            smaGenerateSlide(SIA_FIRST_SLIDE);
        } 
        else
        {
            /* Error */
            instData.result=sig->i_param;
            smaShowDialogWithCallback(SIA_GET_STR_ID(SIA_STR_DIA_CORRUPT_CONTENT),SIA_DIALOG_INFO,SIA_FSM_MAIN,SIA_SIG_MH_EXIT);
        }/* if */
        break;
    case SIA_SIG_MH_SHOW_SLIDE_MENU:
        /* get object actions */
        objInfo = smaGetObjectInfo((WE_UINT32)sig->i_param, instData.objectList);
        if (objInfo != NULL)
        {
            /* object found - get actions */
            drmCt = smaMakeDrmContentType(&objInfo->obj.contentType);

            instData.selectedObj = (WE_UINT32)sig->i_param;
            instData.selectedLink = sig->p_param;
            smaCrhGetActions(&objInfo->obj.contentType, 
                drmCt, SIA_FSM_MAIN, SIA_SIG_MH_HANDLE_ACTION_RSP);

            if (drmCt != NULL)
            {
                SIA_FREE(drmCt);
            } /* if */
        }
        else
        {
            /* object not found - so no actions available */
            (void)SIA_SIGNAL_SENDTO_UUP(SIA_FSM_MAIN, 
                SIA_SIG_MH_HANDLE_ACTION_RSP, SIA_CRH_ERROR, 0, NULL);
        }
        
        break;
    case SIA_SIG_MH_HANDLE_ACTION_RSP:
        actions = sig->p_param;
		actionCount = sig->u_param2;

        
		if (sig->u_param1 == SIA_CRH_OK)
        {
            (void)smaCreateSlideMenu(actions, actionCount, instData.objectList, 
                instData.selectedLink);
			actions = NULL;		/* hi-hack the pointer */
        }
        else
        {
            (void)smaCreateSlideMenu(NULL, 0, instData.objectList, 
                instData.selectedLink);
        }
		if (actions)
		{
			smaFreeActionList(actions, actionCount);
		}
        break;
    case SIA_SIG_MH_OBJ_ACTION:
        objInfo = smaGetObjectInfo(instData.selectedObj, 
            instData.objectList);
        handleObjAction(objInfo, sig->p_param, sig->u_param1);
        break;
    case SIA_SIG_MH_OBJ_ACTION_DONE:
        /* what to do? */
        
        break;
    case SIA_SIG_MH_SLIDE_EXPIRE:
        if (smaIsPlaying())
        {
            displayNextSlide();
        }
        else
        {
            smaSlideFormStopSound(instData.formHandle);
        }/* if */
        break;
    case SIA_SIG_MH_LOAD_SLIDE_RSP:
        /* slide is loaded - display it! */
        if (instData.isPlaying)
        {
            smaUnregisterStepEvent(instData.formHandle);
        }
        else
        {
            smaRegisterStepEvent(instData.formHandle, SIA_FSM_MAIN, 
                SIA_SIG_MH_STEP_EVENT);
        }  /* if */
        (void)smaShowSlideForm(instData.formHandle);
        smaStartDispatcher(instData.isPlaying);
        break;
    case SIA_SIG_GADGET_GOT_FOCUS:
        smaHandleGadgetNotif(WeNotifyFocus, sig->u_param1, 
            instData.formHandle);
        break;

    case SIA_SIG_GADGET_LOST_FOCUS:
        smaHandleGadgetNotif(WeNotifyLostFocus, sig->u_param1, 
            instData.formHandle);
        break;

    case SIA_SIG_EVENT_KEY_DOWN:
        smaHandleEvent(WeKey_Down, sig->u_param1, instData.formHandle);
        break;

    case SIA_SIG_EVENT_KEY_UP:
        smaHandleEvent(WeKey_Up, sig->u_param1, instData.formHandle);
        break;
    case SIA_SIG_MH_STEP_EVENT:
        if (sig->u_param1 == WeKey_Left)
        {
            smaDisplayPreviousSlide();
        }
        else if(sig->u_param1 == WeKey_Right)
        {
            smaDisplayNextSlide();
        }
        break;
    case SIA_SIG_MH_EXIT:
        EXIT_SMIL_PLAYER(instData.result);

        break;
    default:
        WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "MMSSYST: received unknown signal: %d\n", sig->type));
    }
    smaSignalDelete(sig);
} /* signalHandler */

/* If no specific region from incoming mms region, 
 * we will set default region to the objects
 */
static void updateRegion(void)
{		
	SisSlideElementList *temp2; 
	SisSlideInfoList *temp1; 
	int i = 0;

    if(instData.regionList == NULL)
	/* if anything is corrupt with the region, just split the display in 
	 * half and set size */
	{
	   /* we update regionId for all objects since regionId of all objects 
	    * are 0 if no region list is specified.
	    */
	   temp1 = instData.slideInfoList;
	   temp2 = instData.slideInfoList->current.elementList;
	   while(temp1)
	   {
			i = 0;
			while(temp2)
			{
				temp2->current.regionId = i;
				temp2 = temp2->next;
				i++;
			}
			temp1 = temp1->next;
			if(temp1)
			{
				temp2 = temp1->current.elementList;
			}
	   }
		/* corrupt region */
		instData.regionList = SIA_ALLOCTYPE(SisRegionList);
		instData.regionList->next = SIA_ALLOCTYPE(SisRegionList);
		instData.regionList->next->next = NULL;

		instData.regionList->id = 0;
		instData.regionList->region.name = NULL;
		instData.regionList->region.size.cx.value = 100;
		instData.regionList->region.size.cx.type= 1;
		instData.regionList->region.size.cy.value = 50;
		instData.regionList->region.size.cy.type= 1;
		instData.regionList->region.position.cx.value = 0;
		instData.regionList->region.position.cx.type= 1;
		instData.regionList->region.position.cy.value = 0;
		instData.regionList->region.position.cy.type = 1;

		instData.regionList->next->id = 1;
		instData.regionList->next->region.name = NULL;
		instData.regionList->next->region.size.cx.value = 100;
		instData.regionList->next->region.size.cx.type= 1;
		instData.regionList->next->region.size.cy.value = 50;
		instData.regionList->next->region.size.cy.type= 1;
		instData.regionList->next->region.position.cx.value = 0;
		instData.regionList->next->region.position.cx.type= 1;
		instData.regionList->next->region.position.cy.value = 50;
		instData.regionList->next->region.position.cy.type = 1;
   }
		
} /* updateRegion */

/*!
 * \brief Handle object actions
 *
 * \param instance The current instance.
 * \param index The index in the object action list.
 *****************************************************************************/
static void handleObjAction(const SiaObjectInfo *objInfo, 
	const we_act_action_entry_t *actions, WE_UINT32 index)
{
    we_pck_attr_list_t *fileAttributes;
    char *fileName;

    if (objInfo == NULL)
    {
        return;
    }
    fileName = smaCreateFileName(objInfo);

    /* Set file attributes */
    we_pck_attr_init(&fileAttributes);

    (void)we_pck_add_attr_string_value (WE_MODID_SIA, &fileAttributes, 
		WE_PCK_ATTRIBUTE_MIME, (const char *)objInfo->obj.contentType.strValue);
	
    (void)we_pck_add_attr_int_value (WE_MODID_SIA, &fileAttributes,
		WE_PCK_ATTRIBUTE_SIZE, (WE_INT32)objInfo->obj.size);

    /* Send the content to the specified module */
    smaCrhSendContent(fileAttributes, fileName, 
        SIA_FSM_MAIN, SIA_SIG_MH_OBJ_ACTION_DONE, &(actions[index]), 
        instData.msgId, (WE_UINT16)objInfo->obj.id);
    we_pck_attr_free(WE_MODID_SIA, fileAttributes);

    if (fileName != NULL)
    {
        SIA_FREE(fileName);
    } /* if */
}

SlsMediaObjectType smaGetSlsMediaObjectType(const char *mimeType)
{
    WE_UINT8 size;
    int i;

    size = (WE_UINT8)
        (sizeof(supportedMediaTypes)/sizeof(supportedMediaTypes[0]));

    for (i = 0; i < size; i++)
    {
        if (0 == we_cmmn_strcmp_nc(mimeType, supportedMediaTypes[i].mimeType))
        {
            return supportedMediaTypes[i].slsObjectType;
        } /* if */
    } /* for */

    return SIS_OBJECT_TYPE_UNKNOWN;
} /* smaGetSlsMediaObjectType */

WE_BOOL smaIsSupportedMediaObject(const char *mimeType)
{
    WE_UINT8 size;
    int i;

    size = (WE_UINT8)
        (sizeof(supportedMediaTypes)/sizeof(supportedMediaTypes[0]));

    for (i = 0; i < size; i++)
    {
        if (0 == we_cmmn_strcmp_nc(mimeType, supportedMediaTypes[i].mimeType))
        {
            return TRUE;
        } /* if */
    } /* for */

    return FALSE;
} /* smaIsSupportedObjectType */


/*!
 * \brief Calls the WE Telephone API function WE_TEL_MAKE_CALL. 
 *        Result is passed as TRUE/FALSE in u_param1 using the requested 
 *        return signal. Multiple calls using the same callback FSM/signal 
 *        combination is not possible!
 * 
 * \param fsm       The callback FSM.
 * \param signal    The callback signal.
 * \param number    The number to call
 * \param id        The id of this call operation
 *****************************************************************************/
void smaMakeCall(const char *number)
{
    WE_TEL_MAKE_CALL((WE_UINT8)WE_MODID_SIA, 0, number);
} /* smaMakeCall */

/*!
 * \brief Handles the response signal for the WE_TEL_MAKE_CALL operation
 * 
 * \param callRsp   The response data
 *****************************************************************************/
void smaMakeCallRsp(const we_tel_make_call_resp_t *callRsp)
{
    WE_BOOL ret;

    /* Analyze the callback data */
    ret = (WE_TEL_OK == callRsp->result);

    /* Show dialogs on error */
    if ((FALSE == ret) && (NULL != callRsp))
    {
        switch (callRsp->result)
        {
        case WE_TEL_ERROR_CALLED_PARTY_IS_BUSY:
        case WE_TEL_ERROR_CALLED_PARTY_NO_ANSWER:
            /* The called party is busy*/
            (void)smaShowDialog(SIA_STR_ID_CALL_BUSY, SIA_DIALOG_ERROR);    
            break;
        case WE_TEL_ERROR_NETWORK_NOT_AVAILABLE:
        case WE_TEL_ERROR_INVALID:
        case WE_TEL_ERROR_UNSPECIFIED:
        default:
            (void)smaShowDialog(SIA_STR_ID_CALL_FAILED, SIA_DIALOG_ERROR);    
            break;
        } /* switch */
    }
    else if (NULL == callRsp)
    {
        (void)smaShowDialog(SIA_STR_ID_CALL_FAILED, SIA_DIALOG_ERROR);    
    } /* if */

} /* smaMakeCallRsp */


static void smaActionCallback(WE_UINT32 objId, SiaMoAction action)
{
    if (SIA_MO_START == action) 
    {
        (void)smaDisplayFormObject(instData.formHandle, objId);
    }
    else
    {
        (void)smaHideFormObject(instData.formHandle, objId);
    } /* if */
    return;
} /* smaActionCallback */


/*!
 * \brief Converts a buffer to UTF8. This function can only be used if the 
 *        caller knows that the result buffer will be less or equal in size
 *        then SIA_MAX_TEXT_SIZE.
 * 
 * \param charset Charset of srcData buffer
 * \param srcData Buffer containing text to be converted
 * \param srcSize srcData buffer size
 * \param dstSize Size of the returned destination buffer
 * \return dstData Buffer containing converted text UTF8. 
 *         The caller must free this buffer, NULL if not converted
 ****************************************************************************/
char *convertToUTF8(WE_UINT32 charset, const char *srcData, 
    WE_UINT32 *srcSize, WE_UINT32 *dstSize)
{
    /* MMS to WE type conversion */
    we_charset_convert_t *convFunc;
	int length;	
	/* Set destSize to the highest number a long can hold.
	   This is to make the first convFunc() call write a 
	   correct value to destSize. If destSize is not set or 
	   set to 0, the first destSize will be 0 after convFunc(). */
    long destSize = SIA_MAXIMUM_LONG_POSITIVE_VALUE;
    char *smaDestBuffer = NULL;


    if ((charset == WE_CHARSET_UTF_8) || 
         (charset == WE_CHARSET_ASCII))
    {
        return NULL;
    }
    length = (int)srcSize;
    convFunc = we_charset_find_function_bom((int)charset, 
        WE_CHARSET_UTF_8, (char *)srcData, &length);

    if (NULL != convFunc)
    {
        /* Find buffer size */
        if (0 != convFunc((const char *)srcData, (long *)srcSize, NULL, &destSize))
        {
            return NULL;
        }
        /* Allocate buffer */
        smaDestBuffer = SIA_ALLOC((unsigned long)destSize+1);
        if (NULL == smaDestBuffer)
        {
            return NULL;
        }

        /* Do the conversion */
        if (0 != convFunc((const char *)srcData, (long *)srcSize, smaDestBuffer, 
            &destSize))
        {
            /*lint -e{774}*/
            SIA_FREE(smaDestBuffer);
            return NULL;
        }

        /* Append Null character just in case */
        smaDestBuffer[destSize] = 0;
        ++destSize;

        *srcSize = (WE_UINT32)length;
		*dstSize = (WE_UINT32)destSize;
        /* Removes the option UTF8 identifier if present */
        smaHandleUtf8Bom(smaDestBuffer);        
        return smaDestBuffer;
    }
    return smaDestBuffer;
} /* convertToUTF8 */


/*! \brief Removes the optional UTF-8 mark if it is present
 *
 * \param str The string to remove the "possible" UTF-8 mark from.
 *****************************************************************************/
void smaHandleUtf8Bom(char *str)
{
    if (NULL != str)
    {
        /* Check if the optional UTF-8 identifier is present */
        if ((strlen(str) > strlen(SIA_UTF8_BOM)) && 
            (memcmp(str, SIA_UTF8_BOM, strlen(SIA_UTF8_BOM)) == 0))
        {
            /* Move the content back in the buffer so that the identifier is
               removed */
            memmove(str, &(str[strlen(SIA_UTF8_BOM)]), 
                strlen(str) - strlen(SIA_UTF8_BOM) + 1);
        }
    }
} /* smaHandleUtf8Bom */


/*!
 * \brief Displays the previous slide
 * 
 * \param playerInfo A pointer to the information about the SMIL.
 *
 * \return The result of the operation.
 *****************************************************************************/
void smaDisplayNextSlide(void)
{
    if (instData.currentSlide == (instData.noOfSlides - 1))
    {
        instData.currentSlide = SIA_FIRST_SLIDE;
    } 
    else
    {
        instData.currentSlide++;
    }/* if */

    smaGenerateSlide(instData.currentSlide);
} /* smaDisplayNextSlide */


/*!
 * \brief Displays the previous slide
 * 
 * \param playerInfo A pointer to the information about the SMIL.
 *
 * \return The result of the operation.
 *****************************************************************************/
void smaDisplayPreviousSlide(void)
{
    if (instData.currentSlide == SIA_FIRST_SLIDE)
    {
        instData.currentSlide = (WE_UINT8)(instData.noOfSlides - 1);
    } 
    else
    {
        instData.currentSlide--;
    }/* if */

    smaGenerateSlide(instData.currentSlide);
} /* smaDisplayPreviousSlide */


/*!
 * \brief Rewinds the SMIL message
 * 
 * \param playerInfo A pointer to the information about the SMIL.
 *
 * \return The result of the operation.
 *****************************************************************************/
void smaRewindSmil(void)
{
    if (!smaIsPlaying())
    {
        /* NOT in play mode and NOT on first slide */
        instData.currentSlide = SIA_FIRST_SLIDE;

        smaGenerateSlide(instData.currentSlide);
    } /* if */
} /* smaRewindSmil */

/*!
 * \brief Display the next slide.
 * 
 * \param playerInfo A pointer to the information about the smil.
 *
 * \return The result of the operation.
 *****************************************************************************/
void displayNextSlide(void)
{
    WE_BOOL renderView = TRUE;

    if (smaIsPlaying() &&  (instData.currentSlide == 
        (instData.noOfSlides - 1)))
    {
        /* in play mode and on last slide */
        /* STOP */
        smaStopSmil();
        renderView = FALSE;
    }
    else if (!smaIsPlaying() && (instData.currentSlide == 
        (instData.noOfSlides - 1)))
    {
        /* in play mode and on last slide */
        instData.currentSlide = SIA_FIRST_SLIDE;
        renderView = TRUE;
    } 
    else
    {
        /* NOT on last slide */
        instData.currentSlide++;
        renderView = TRUE;
    } /* if */

    if (renderView)
    {
        /* load the slide */
        smaGenerateSlide(instData.currentSlide);
    } /* if */    
} /* displayNextSlide */

/*!
 * \brief Plays a SMIL
 * 
 * \param playerInfo A pointer to the information about the smil.
 *
 * \return The result of the operation.
 *****************************************************************************/
void smaPlaySmil(void)
{
    if (smaIsPlaying())
    {
        /* already running */
        return;
    } /* if */

	/*TR 18307 make immediate replay after the previous play possible */
     /*TR 18307 make immediate replay after the previous play possible */
    if(instData.currentSlide == instData.noOfSlides - 1){
               instData.currentSlide = SIA_FIRST_SLIDE;
    }
    
    
    instData.isPlaying = TRUE;
    smaUpdateSlideFormActions(smaIsPlaying(), instData.formHandle);
    smaGenerateSlide(instData.currentSlide);
    smaUnregisterStepEvent(instData.formHandle);

#ifdef JOFR
   (void)showStatusIcon(SIA_ACTION_PLAY);
#endif
   return;
} /* smaPlaySmil */


/*!
 * \brief Stop playing the SMIL
 * 
 * \param playerInfo A pointer to the information about the SMIL.
 *
 * \return The result of the operation.
 *****************************************************************************/
void smaStopSmil(void)
{
    if (!smaIsPlaying())
    {
        /* not currently running */
        return;
    } /* if */
    
    instData.isPlaying = FALSE;
    smaUpdateSlideFormActions(smaIsPlaying(), instData.formHandle);

    /* Stop playing */
    smaStopDispatcher();

    smaRegisterStepEvent(instData.formHandle, SIA_FSM_MAIN, 
        SIA_SIG_MH_STEP_EVENT);

#ifdef JOFR
    if (!smaShowStatusIcon(SIA_ACTION_STOP ))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: unable to "
            "create action image in stopSmil \n"));
        result = SIA_RESULT_ERROR;
    } /* if */
#endif
	
} /* smaStopSmil */


const SisRegionList *smaGetRegionList(void)
{
    return instData.regionList;
}

/* TR18628 */
const SisRootLayout smaGetRootLayout(void)
{
	return instData.rootLayout;
}

SiaObjectInfoList *smaGetObjectInfoList(void)
{
    return instData.objectList;
}

WE_BOOL smaIsPlaying(void)
{
    return instData.isPlaying;    
} /* smaIsPlaying */


WE_UINT32 smaGetMsgId(void)
{
	return instData.msgId;
}

char *smaCreateFileName(const SiaObjectInfo *objInfo)
{
    const char *fileName;
    const char *ending;
    char *fName;
    

    char *to = NULL;
    WE_UINT32 i = 0;
    WE_UINT32 e = 0;

    if (objInfo == NULL)
    {
        return NULL;
    }

    fileName = smaGetFileNameParam(objInfo->obj.contentType.params);
    if (fileName == NULL)
    {
        fileName = smaGetNameParam(objInfo->obj.contentType.params); 
    } /* if */

    if (fileName != NULL)
    {
        fName = we_cmmn_strdup(WE_MODID_SIA, fileName);
    }
    else
    {
        ending = we_mime_to_ext ((const char *)
            objInfo->obj.contentType.strValue);
        fName = smaGenerateFileName(ending);
    }
    if (fName == NULL)
    {
        return NULL;
    }
    to = (char*)SIA_ALLOC(strlen(fName) + 1);
    i = 0;
    e = 0;
    
    if (fName[i] == '"')
    {
        ++i;
    } /* if */
    if (fName[i] == '<')
    {
        ++i;
    } /* if */
    while (fName[i] && fName[i] != '>' && fName[i] != '"')
    {
        to[e] = fName[i];
        ++e;
        ++i;
    } /* while */
    to[e] = 0;
    SIA_FREE(fName);
    fName = to;
    return fName;
} /* smaCreateFileName */


/*!
 * \brief Generates a name for a file
 * \param extension What extension to set on the file. (max 3 char)
 * \return The name or NULL on error
 *****************************************************************************/
static char *smaGenerateFileName(const char *extension)
{
    char *name = NULL;
    unsigned int size = 0;
    static WE_UINT32 counter; /* Initialized to zero at start-up */
    WE_UINT32 currentTime = 0;
    unsigned int extensionSize = 0;

    if (NULL == extension)
    {
        return NULL;
    }

    extensionSize = strlen(extension);
    /* create name for text */
    currentTime = WE_TIME_GET_CURRENT();

    /* current-time + "." + extension + NULL */
    size = (sizeof(WE_UINT32)*2 + 1 + extensionSize + 1);
    
    name = SIA_ALLOC((unsigned long)size);
    memset(name, 0, size);
    /*lint -e{727} */
    sprintf(name, "%lx.%s", (long)(currentTime<<8)|(counter&0xff), 
        extension);
    /* Use a counter to prevent problems if the CPU is to fast, i.e., 
       the timer has not been increased between two calls, 256 calls
       to meaCreatefileName during the same time is not likely */
    ++counter;
    return name;
} /* smaGenerateFileName */


/*! \brief
 *
 * \param contentType The content-type of the object.
 * \return The DRM content-type including parameters, or NULL.
 *****************************************************************************/
char *smaMakeDrmContentType(const MmsContentType *contentType)
{
    if ((NULL != contentType) && (NULL != contentType->drmInfo) && 
        (NULL != contentType->drmInfo->drmContainerType.strValue))
    {
        return we_cmmn_strdup(WE_MODID_SIA, 
            (char *)contentType->drmInfo->drmContainerType.strValue);
    }
    return NULL;
}

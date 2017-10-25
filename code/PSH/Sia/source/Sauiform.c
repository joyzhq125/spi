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
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Wid.h"
#include "We_Log.h"
#include "We_Mem.h"
#include "We_Int.h"

#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"

#include "Sia_if.h"
#include "Sia_rc.h"
#include "Satypes.h"
#include "Samain.h"
#include "Samem.h"
#include "Saintsig.h"
#include "Sauiform.h"
#include "Sauisig.h"
#include "Sasls.h"
#include "Saui.h"
#include "Sacore.h"



/* MTR */
#include "Smtr.h"

/*--- Types ---*/

#define SIA_TEXT_GADGET_PADDING (5)
#define SIA_MAX_FORMS 5

typedef struct 
{
    WE_UINT32          handle;
    WeGadgetHandle     gadgetHandle;
    int                 mtrHandle;
    WePosition         pos;
    WE_BOOL            isSound;
    WE_UINT32          objId;
} SiaFormObject;

typedef struct SiaFormObjectListSt
{
    WE_UINT8                   id;
    SiaFormObject               current;
    struct SiaFormObjectListSt  *next;
} SiaFormObjectList;


/* Slide form information */
typedef struct
{
    WeWindowHandle     handle;
    WeSize             size;
    SiaFormObjectList   *objList;
    const SiaFormObject *currentObj;
    
    /* used for stepping */
    SiaStateMachine     stepRetFsm;
    int                 stepRetSig;
    
    /* actions */
    WeActionHandle actionOk;
    WeActionHandle actionStop;
    WeActionHandle actionMenu;

    /* Icon handles */
    WeGadgetHandle iconGadgetHandle;
} SiaSlideForm;

typedef struct 
{
    int          handle;        /* handle, negative if not used */
    SiaSlideForm *formPtr;
} FormInstances;

/******************************************************************************
 * Constants
 *****************************************************************************/
/*--- Static variables ---*/

static FormInstances formInstances[SIA_MAX_FORMS + 1];

/*--- Definitions/Declarations ---*/

    
/*--- Prototypes ---*/
static WE_BOOL createSlideFormActions(SiaSlideForm *formInfo, WE_BOOL isPlaying);
static void deleteSlideFormActions(SiaSlideForm *formInfo);
static const SiaFormObject *getFormObjFromGadgetHandle(
    const SiaFormObjectList *objList, WE_UINT32 gadgetHandle);
static const WE_UINT32 getFormObjCount(const SiaFormObjectList *objList);
static void handleOkAction(WeWindowHandle window);
static void handleStopAction(WeWindowHandle window);
static void handleMenuAction(WeWindowHandle window);
static void freeSlideForm(SiaSlideForm *form);
static void removeFormFromScreen(WeWindowHandle formHandle);
static void addFormToScreen(WeWindowHandle formHandle);
static void freeSiaFormObject(SiaFormObject *obj);
static SiaSlideForm *getFormInstanceByHandle(SiaSlideFormHandle handle);
static const SiaFormObject *getFormObjectFromId(const SiaSlideForm *form, 
    WE_UINT32 objId);
static void deleteFormInstance(SiaSlideFormHandle handle);
static SiaSlideFormHandle createFormInstance(SiaSlideForm **form);
static WE_UINT8 addItemToObjectList(SiaFormObjectList **listStart, 
    const SiaFormObject *objPtr);
static void copyFormObjectInfo(const SiaFormObject *src, SiaFormObject *dst);
static SiaFormObject *smaGetSelectedObject(SiaFormObjectList *objList);
static SiaSlideForm *getFormInstanceByWindowHandle(WeWindowHandle handle);
static void stopSound(SiaSlideForm *form);
static void disableObjectFocus(SiaSlideForm *form);
static void enableObjectFocus(SiaSlideForm *form);

void smaSlideFormInit(void)
{
    memset(formInstances, 0x00, sizeof(FormInstances)*(SIA_MAX_FORMS + 1));
}

void smaSlideFormTerminate(void)
{
    int i;

    for (i=0; i < SIA_MAX_FORMS; i++)
    {
        if (formInstances[i].formPtr != NULL)
        {
            freeSlideForm(formInstances[i].formPtr);
        } /* if */
    }
} 


void smaDeleteSlideForm(SiaSlideFormHandle formHandle)
{
    smaSlideFormStopSound(formHandle);
    deleteFormInstance(formHandle);
} /* smaDeleteSlideForm */


void smaHideSlideForm(SiaSlideFormHandle formHandle)
{
    SiaSlideForm *form;

    form = getFormInstanceByHandle(formHandle);

    if (form == NULL)
    {
        return;
    } /* if */

    removeFormFromScreen(form->handle);
} /* smaHideSlideForm */

SiaResult smaShowSlideForm(SiaSlideFormHandle formHandle)
{
    SiaResult result = SIA_RESULT_OK;
    SiaSlideForm *form;

    form = getFormInstanceByHandle(formHandle);

    if (form == NULL)
    {
        return SIA_RESULT_ERROR;
    } /* if */


    addFormToScreen(form->handle);

    if (!WE_WIDGET_HAS_FOCUS(form->handle))
    {
       
        if (-1 == WE_WIDGET_SET_IN_FOCUS (form->handle, TRUE))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: "
                "unable to set form in focus in widgetAction\n"));
            result = SIA_RESULT_ERROR;
        } /* if */
    } /* if */
    return result;
} /* smaHideSlideForm */

/*! \brief Updates slide form actions 
 *  \param isPlaying TRUE if currently in play mode
 *****************************************************************************/
void smaUpdateSlideFormActions(WE_BOOL isPlaying, 
    SiaSlideFormHandle formHandle)
{
    SiaSlideForm *form;

    form = getFormInstanceByHandle(formHandle);

    if (form == NULL)
    {
        return;
    } /* if */
    
    deleteSlideFormActions(form);
	(void)createSlideFormActions(form, isPlaying);
} /* smaUpdateSlideFormActions */


/*! \brief Creates a form
 *  \return TRUE on OK else FASLE
 *****************************************************************************/
SiaSlideFormHandle smaCreateSlideForm(const WeColor *backgroundColor)
{
    WeStyleHandle style;	
    WeColor bgColor = {255, 255, 255};

    WE_BOOL err = FALSE;
    SiaSlideFormHandle handle;
    SiaSlideForm *formInfo;

    if (0 == (handle = createFormInstance(&formInfo)))
    {
        return (SiaSlideFormHandle)0;
    } /* if */

    if (backgroundColor != NULL)
    {
        bgColor = *backgroundColor;
    } /* if */

    if (0 == (style = WE_WIDGET_STYLE_CREATE(WE_MODID_SIA, NULL, 
        &bgColor, NULL, NULL, NULL, NULL, NULL)))
    {
        deleteFormInstance(handle);
        return (SiaSlideFormHandle)0;
    } /* if */

    formInfo->handle = WE_WIDGET_FORM_CREATE(WE_MODID_SIA, NULL, 0, style);
    (void)WE_WIDGET_RELEASE(style);
    style = 0;

    if (formInfo->handle == 0)
    {
        err = TRUE;
    }
    else if (!createSlideFormActions(formInfo, smaIsPlaying()))
    {
        err = TRUE;
    }
    if (err)
	{
        deleteFormInstance(handle);
        handle = 0;
	} 
    else
    {
        /* save form size */
        smaGetWidnowInsideArea(formInfo->handle, &formInfo->size);
    }/* if */

    return handle;
} /* smaCreateSlideForm */



  
/*! \brief 
 * \param 
 * \return 
 *****************************************************************************/
WE_BOOL smaDisplayFormObject(SiaSlideFormHandle formHandle, WE_UINT32 objId)
{
    SiaSlideForm *form;
    const SiaFormObject *obj;
    
    form = getFormInstanceByHandle(formHandle);

    if (form == NULL || form->handle == 0)
    {
        return FALSE;
    } /* if */
    
    obj = getFormObjectFromId(form, objId);

    if (obj->isSound)
    {
        /* play sound */
        if (obj->handle != 0)
        {
            (void)WE_WIDGET_SOUND_PLAY(obj->handle);
        }
    }
    else
    {
        if (-1 == WE_WIDGET_WINDOW_ADD_GADGET(form->handle, obj->gadgetHandle, 
            &obj->pos))
        {
            return FALSE;
        } /* if */                
    }
    return TRUE;
} /* smaDisplayFormObject */


WE_BOOL smaHideFormObject(SiaSlideFormHandle formHandle, WE_UINT32 objId)
{
    SiaSlideForm *form;
    const SiaFormObject *obj;
    
    form = getFormInstanceByHandle(formHandle);
    
    if (form == NULL || form->handle == 0)
    {
        return FALSE;
    } /* if */
    
    obj = getFormObjectFromId(form, objId);

    if (obj->isSound)
    {
        /* play sound */
        if (obj->handle != 0)
        {
            (void)WE_WIDGET_SOUND_STOP(obj->handle);
        }

    }
    else
    {
        if (-1 == WE_WIDGET_REMOVE(form->handle, obj->gadgetHandle))
        {
            return FALSE;
        } /* if */
    } /* if */

    return TRUE;
} /* smaHideFormObject */


static void handleOkAction(WeWindowHandle window)
{
    (void)window;

    /* do not accept play action if in play mode */
    if (!smaIsPlaying())
    {
        /* play SMIL */
        smaPlaySmil();
    } /* if */
} /* handleOkAction */

static void handleStopAction(WeWindowHandle window)
{
    SiaSlideForm *form;
    
    (void)window;
    
   /* stop SMIL or exit SMIL player */
    if (smaIsPlaying())
    {
        form = getFormInstanceByWindowHandle(window);
        
        /* stop all sounds */
        stopSound(form);

        /* We are in play mode and user asked to stop SMIL*/
        smaStopSmil();
    }
    else
    {
        /* user asked to exit SMIL player */
		EXIT_SMIL_PLAYER(SIA_RESULT_OK);
    } /* if */
} /* handleStopAction */


static void stopSound(SiaSlideForm *form)
{
    const SiaFormObjectList *objList;
    const SiaFormObject *obj;

    if (form == NULL)
    {
        return;
    } /* if */
    objList = form->objList;

    while(objList)
    {
        obj = &objList->current;
        if (obj->handle != 0 && obj->isSound)
        {
            (void)WE_WIDGET_SOUND_STOP(obj->handle);
        } /* if */
        objList = objList->next;
    } /* while */
}

void smaSlideFormStopSound(SiaSlideFormHandle formHandle)
{
    SiaSlideForm *form;
    
    form = getFormInstanceByHandle(formHandle);
    stopSound(form);
}

static void handleMenuAction(WeWindowHandle window)
{
    SiaSlideForm *form = NULL;
    SiaFormObject *obj = NULL;
    WE_INT32 objId;
    SmtrLinkEntry *selectedLink = NULL;
    
    (void)window;

    form = getFormInstanceByWindowHandle(window);
    if (form == NULL)
    {
        return;
    } /* if */

    obj = smaGetSelectedObject(form->objList);

    if (obj != NULL)
    {
        objId = (WE_INT32)obj->objId;
        smtrFreeSmtrLinkEntry(&selectedLink);   
        /* get mtr info from object */
        (void)smtrHasActiveLink(obj->mtrHandle, 
            &selectedLink);
    }
    else
    {
        /* no object selected */
        objId = -1;
    }
    /* send signal to show menu */
    (void)SIA_SIGNAL_SENDTO_IP(SIA_FSM_MAIN, SIA_SIG_MH_SHOW_SLIDE_MENU, objId, 
        selectedLink);
} /* handleMenuAction */


/*!
 * \brief Check which widget is in focus
 * \return Index in list
 *****************************************************************************/
static SiaFormObject *smaGetSelectedObject(SiaFormObjectList *objList)
{
    while (objList)
    {
        if (objList->current.gadgetHandle != 0 && 
            WE_WIDGET_HAS_FOCUS(objList->current.gadgetHandle))
        {
            return &objList->current;
        }
        objList = objList->next;
    }
    return NULL;
}


WE_UINT32 smaAddAudioObjectToForm(const WeSoundHandle handle,
    WE_UINT32 objId, SiaSlideFormHandle formHandle)
{
    SiaFormObject   object; 
    SiaSlideForm    *form;

    form = getFormInstanceByHandle(formHandle);
    if (form == NULL)
    {
        return 0;
    }
    memset(&object, 0x00, sizeof(SiaFormObject));
    object.handle = (WE_UINT32)handle;
    object.isSound = TRUE;
    object.objId = objId;
        

    return addItemToObjectList(&form->objList, &object);
} /* smaAddAudioObjectToForm */


/*! \brief Handles an image object.
 * \param objInfo Object information
 * \param region region information
 * \param elementItem information about the current slide element
 * \param bgColor background color for the image gadget
 *
 *****************************************************************************/
WE_UINT32 smaAddImageObjectToForm(WeImageHandle handle,
                           const SisRegion *region,
                           const WeColor *bgColor,
                           WE_UINT32 objId,
                           SiaSlideFormHandle formHandle)
{
    WeColor        fColor;
    WeColor        bColor;
    WeSize         imgSize;
    WeImageZoom    imageZoom;
    WeSize         regSize;
    WeStyleHandle  style;
    WE_UINT32      ret = 0;
    SiaFormObject   object; 
    SiaSlideForm    *form;
    WeSize         gadgetSize;
    WeTextProperty textProp;
    WeSize         tmpImg;
    
    form = getFormInstanceByHandle(formHandle);
    
    if (handle == 0 ||
        region == NULL ||
        region->size.cx.value == 0 ||
        region->size.cy.value == 0 ||
        form == NULL)
    {
        return 0;
    } /* if */

    memset(&object, 0x00, sizeof(SiaFormObject));
    object.mtrHandle = -1;
    object.handle = (WE_UINT32)handle;
    object.isSound = FALSE;
    object.objId = objId;

    
    /* get image size */
    if (WE_WIDGET_GET_SIZE(handle, &imgSize) < 0)
    {
        /* unable to get image size */
        return 0;
    } /* if */

    (void)WE_WIDGET_STYLE_GET_DEFAULT(WeStringStyle, &fColor, &bColor,
        0, 0, 0, 0, &textProp);

    if (bgColor != NULL)
    {
        /* copy color value */
        bColor = *bgColor;
    } /* if */


    /* copy region size */
    smaCvtSlsSizeToPos(&region->position, &object.pos, &form->size);
    smaCvtSlsSizeToSize(&region->size, &regSize, &form->size);

    if ((regSize.height + object.pos.y) > form->size.height)
    {
        regSize.height = (WE_INT16)(form->size.height - 
            (regSize.height + object.pos.y));
    }

    if ((regSize.width + object.pos.x) > form->size.width)
    {
        regSize.width = (WE_INT16)(form->size.width - 
            (regSize.width + object.pos.x));
    }

    gadgetSize.height = regSize.height;
    gadgetSize.width = regSize.width;

    if (0 == imgSize.height || 0 == imgSize.width) 
    {
        /* has no size - set to auto fit */
        imageZoom = WeImageZoomAutofit;
        gadgetSize.height = regSize.height;
        gadgetSize.width = regSize.width;
    }
    /* Check if the image can fit horizontally in the region */
	else if (imgSize.height > regSize.height ||
        imgSize.width > regSize.width)
	{
        tmpImg = imgSize;
        
		/* Scale using the same aspect */
        if (tmpImg.height > regSize.height)
        {
            tmpImg.width = (WE_INT16)((tmpImg.width * regSize.height)/
                (tmpImg.height));
            tmpImg.height = regSize.height;
        }
        
        if (tmpImg.width > regSize.width)
        {
            tmpImg.height = (WE_INT16)((tmpImg.height * regSize.width)/
                (tmpImg.width));
            tmpImg.width = regSize.width;
        }

        imageZoom = WeImageZoomAutofit;
        gadgetSize.height = tmpImg.height;
        gadgetSize.width = tmpImg.width;
	}
    else if (WeImageZoomAutofit != (imageZoom = smaGetImageZoom(region->fit)))
    {
		/* center the object */

        /* Image position */
		object.pos.x = (WE_INT16)(object.pos.x + 
            (((WE_UINT16)(regSize.width - imgSize.width))>>1));

        object.pos.y = (WE_INT16)(object.pos.y + 
            (((WE_UINT16)(regSize.height - imgSize.height))>>1));

        gadgetSize.height = imgSize.height;
        gadgetSize.width = imgSize.width;
    } /* if */
    
    style = WE_WIDGET_STYLE_CREATE(WE_MODID_SIA, NULL, 
            &bColor, NULL, NULL, NULL, NULL, NULL);

    /* Create gadget */
    object.gadgetHandle = WE_WIDGET_IMAGE_GADGET_CREATE(
        WE_MODID_SIA, handle, &gadgetSize, imageZoom, 0, style);
    
    if (style != 0)
    {
        (void)WE_WIDGET_RELEASE(style);
        style = 0;
    } /* if */
    
    ret = addItemToObjectList(&form->objList, &object);
       
    return ret;
} /* smaAddImageObjectToForm */

static void enableObjectFocus(SiaSlideForm *form)
{
    const SiaFormObjectList *objList;
    const SiaFormObject *obj;

    if (form == NULL)
    {
        return;
    } /* if */
    objList = form->objList;

    while(objList)
    {
        obj = &objList->current;
        if (obj->gadgetHandle != 0 && !obj->isSound)
        {
            (void)WE_WIDGET_GADGET_SET_PROPERTIES (
               obj->gadgetHandle, WE_GADGET_PROPERTY_FOCUS, 0);
        } /* if */
        objList = objList->next;
    } /* while */
}


static void disableObjectFocus(SiaSlideForm *form)
{
    const SiaFormObjectList *objList;
    const SiaFormObject *obj;

    if (form == NULL)
    {
        return;
    } /* if */
    objList = form->objList;

    while(objList)
    {
        obj = &objList->current;
        if (obj->gadgetHandle != 0 && !obj->isSound)
        {
            (void)WE_WIDGET_GADGET_SET_PROPERTIES (
               obj->gadgetHandle, 0, 0);
        } /* if */
        objList = objList->next;
    } /* while */
}

/*! \brief Handles a text object.
 * \param object Object information
 * \param data Text data, data MUST be NULL terminated
 * \param size of data
 * \return TRUE on OK else FALSE
 *****************************************************************************/
WE_UINT32 smaAddTextObjectToForm(WeStringHandle handle,
                                  const SisRegion *region,
                                  const WeColor *fgColor,
                                  const WeColor *bgColor,
                                  WE_UINT32 objId,
                                  SiaSlideFormHandle formHandle)
{
    WeColor fColor;
    WeColor bColor;
    SmtrCreateRspData *rsp = NULL;
    SmtrCreateTextBoxProperties prop;
    WeSize         regSize;
    SiaSlideForm    *form;
    SiaFormObject   object;
    WE_UINT32      ret;
    WeTextProperty textProp;
   
    form = getFormInstanceByHandle(formHandle);

    if (handle == 0 ||
        region == NULL ||
        region->size.cx.value == 0 ||
        region->size.cy.value == 0 ||
        form == NULL )
    {
        return 0;
    } /* if */

    /* set default return values */
    memset(&object, 0x00, sizeof(SiaFormObject));
    object.mtrHandle = -1;
    object.handle = (WE_UINT32)handle;
    object.isSound = FALSE;
    object.objId = objId;

    (void)WE_WIDGET_STYLE_GET_DEFAULT(WeStringStyle, &fColor, &bColor,
            0, 0, 0, 0, &textProp);
    
    if (fgColor != NULL)
    {
        fColor = *fgColor;
    }/* if */

    if (bgColor != NULL)
    {
        bColor = *bgColor;
    } /* if */
   
    /* copy region size */
    smaCvtSlsSizeToPos(&region->position, &object.pos, &form->size);
    smaCvtSlsSizeToSize(&region->size, &regSize, &form->size);
    
    if ((regSize.height + object.pos.y) > form->size.height)
    {
        regSize.height = (WE_INT16)(form->size.height - object.pos.y);
    }

    if ((regSize.width + object.pos.x) > form->size.width)
    {
        regSize.width = (WE_INT16)(form->size.width - object.pos.x);
    }
    
   
    /* MTR STUFF */
    
    /* set properties */
    memset(&prop, 0, sizeof(SmtrCreateTextBoxProperties));
    prop.alignment = WeCenter;
    prop.flags = SMTR_PROP_RENDER_LINKS | SMTR_PROP_VERTICAL_RESIZE | 
        SMTR_PROP_HORIZONTAL_RESIZE;
    prop.schemes = 0x7FFFFFFF; /* All schemes */
    memcpy(&prop.size, &regSize, sizeof(WeSize));
    prop.size.width -= (2 * SIA_TEXT_GADGET_PADDING);
    memcpy(&prop.textColor, &fColor, sizeof(WeColor));
    memcpy(&prop.bgColor, &bColor, sizeof(WeColor));
    
    /* create "gadget" */
    if (SMTR_RESULT_OK != smtrCreateTextBox(handle, &prop, &rsp)) 
    {
        if (rsp) 
        {
            smtrFreeSmtrCreateRspData(&rsp);
        }	
        return 0;
    } /* if */
    
    /* register for events */
    if (smaRegisterNotification(rsp->gadgetHandle, WeNotifyFocus, NULL, 
        SIA_FSM_MAIN, SIA_SIG_GADGET_GOT_FOCUS) < 0)
    {
        if (rsp) 
        {
            smtrFreeSmtrCreateRspData(&rsp);
        }	
        return 0;
    } /* if */
    if (smaRegisterNotification(rsp->gadgetHandle, WeNotifyLostFocus, 0, 
        SIA_FSM_MAIN, SIA_SIG_GADGET_LOST_FOCUS) < 0)
    {
        if (rsp) 
        {
            smtrFreeSmtrCreateRspData(&rsp);
        }	
        return 0;
    } /* if */
   
    /* center the gadget */
    object.pos.x = (WE_INT16)(object.pos.x + 
        (((WE_UINT16)(regSize.width - rsp->gadgetSize.width))>>1));
    object.pos.y = (WE_INT16)(object.pos.y + 
        (((WE_UINT16)(regSize.height - rsp->gadgetSize.height))>>1));

    /* Save gadget handle and position so they can be added to form later */
    object.gadgetHandle = rsp->gadgetHandle;
    object.mtrHandle = rsp->instance;
    
    ret = addItemToObjectList(&form->objList, &object);

    if (rsp) 
    {
        smtrFreeSmtrCreateRspData(&rsp);
    }	
    return ret;
} /* smaAddTextObjectToForm */

                                  
/*! \brief 
 *
 * \return The type of media that is currently "selected", or 
 *         MEA_MEDIA_GROUP_NONE if no object is selected.
 *****************************************************************************/
void smaHandleGadgetNotif(WeNotificationType notif, WE_UINT32 handle, 
    SiaSlideFormHandle formHandle)
{
    const SiaFormObject *obj;
    SiaSlideForm    *form;
    
    form = getFormInstanceByHandle(formHandle);

    if (form == NULL)
    {
        return;
    }
    
    obj = getFormObjFromGadgetHandle(form->objList, handle);
    
    if (WeNotifyFocus == notif) 
    {
        form->currentObj = obj;
        if (obj == NULL)
        {
            return;
        } /* if */

        /* Register user events */
        (void)smaRegisterUserEvent(form->handle, WeKey_Down, NULL, 
            SIA_FSM_MAIN, SIA_SIG_EVENT_KEY_DOWN);
        (void)smaRegisterUserEvent(form->handle, WeKey_Up, NULL, 
            SIA_FSM_MAIN, SIA_SIG_EVENT_KEY_UP);
    }
    else if (WeNotifyLostFocus == notif)
    {
        form->currentObj = NULL;
        (void)smaDeleteUserEvent(form->handle, WeKey_Down);
        (void)smaDeleteUserEvent(form->handle, WeKey_Up); 
    }
}



/*! \brief 
 *
 * \return The type of media that is currently "selected", or 
 *         MEA_MEDIA_GROUP_NONE if no object is selected.
 *****************************************************************************/
void smaHandleEvent(WeEventType event, WE_UINT32 handle,
    SiaSlideFormHandle formHandle)
{
    WE_BOOL lostFocus;
    SiaSlideForm    *form;
    const SiaFormObject *obj;
    
    form = getFormInstanceByHandle(formHandle);
    if (form == NULL)
    {
        return;
    }

    (void)handle;
    
    if (WeKey_Down == event || WeKey_Up == event)
    {
        obj = form->currentObj;
        
        if (obj == NULL)
        {
            return;
        } /* if */

        (void)smtrRedraw(obj->mtrHandle, event, &lostFocus);

        /* Check if we lost focus and if we have at least one more object
         * on the form we can give control to high level API */
        if (lostFocus)
        {
            if (getFormObjCount(form->objList) > 1)
            {  
                (void)smaDeleteUserEvent(form->handle, WeKey_Down);
                (void)smaDeleteUserEvent(form->handle, WeKey_Up);
            } /* if */ 
        }
    }
}



static SiaSlideForm *getFormInstanceByWindowHandle(WeWindowHandle handle)
{
    int i;
    for (i=1; i<= SIA_MAX_FORMS; i++)
    {
        if (formInstances[i].formPtr->handle == handle)
        {
            return formInstances[i].formPtr;
        }
    }
    return NULL;
}


static SiaSlideForm *getFormInstanceByHandle(SiaSlideFormHandle handle)
{
    if (handle <= SIA_MAX_FORMS)
    {
        return formInstances[handle].formPtr;
    }
    else
    {
        return NULL;
    }
}

static SiaSlideFormHandle createFormInstance(SiaSlideForm **form)
{
    int i;

    for (i=1; i <= SIA_MAX_FORMS; i++)
    {
        if (0 == formInstances[i].handle)
        {
            /* found an empty spot */
            formInstances[i].handle = i;
            formInstances[i].formPtr = SIA_CALLOC(sizeof(SiaSlideForm));
            *form = formInstances[i].formPtr;
            return formInstances[i].handle;
        } /* if */
    }
    return 0;
}

static void deleteFormInstance(SiaSlideFormHandle handle)
{
    int i;

    for (i=1; i <= SIA_MAX_FORMS; i++)
    {
        if (handle == formInstances[i].handle)
        {
            if (formInstances[i].formPtr != NULL)
            {
                freeSlideForm(formInstances[i].formPtr);
                SIA_FREE(formInstances[i].formPtr);
                formInstances[i].formPtr = NULL;
                formInstances[i].handle = 0;
            }
            return;
        } /* if */
    }
}


static void removeFormFromScreen(WeWindowHandle formHandle)
{
    (void)WE_WIDGET_REMOVE(smaGetScreenHandle(), formHandle);
} /* removeFormFromScreen */

static void addFormToScreen(WeWindowHandle formHandle)
{
    (void)smaBindAndShowWindow(formHandle, smaGetPosLeftTop());
} /* addFormToScreen */


static void freeSiaFormObject(SiaFormObject *obj)
{
    /* Don't release handle */
    if (obj->mtrHandle != -1)
    {
        /* mtr is used - nothing to do if this failes */
        (void)smtrDeleteTextBox(obj->mtrHandle);
    }
    else
    {
        if (obj->gadgetHandle)
        {
            (void)WE_WIDGET_RELEASE(obj->gadgetHandle);
            obj->gadgetHandle = 0;
        }
    }
    obj->gadgetHandle = 0;
    obj->mtrHandle = -1;
}

static void freeSiaFormObjectList(SiaFormObjectList *objList)
{
    SiaFormObjectList *tmpPtr;
    SiaFormObjectList *listStart = objList;

    while(objList)
    {
        freeSiaFormObject(&objList->current);
        tmpPtr = objList;
        objList = objList->next;
        
        if (tmpPtr != listStart)
        {
            SIA_FREE(tmpPtr);
        } /* if */
    }
}

static void freeSlideForm(SiaSlideForm *form)
{
    if (form == NULL)
    {
        return;
    }
        
    deleteSlideFormActions(form);
    
    if (form->iconGadgetHandle != 0)
    {
        (void)WE_WIDGET_REMOVE(form->handle, 
			form->iconGadgetHandle);
		(void)WE_WIDGET_RELEASE(form->iconGadgetHandle);
        form->iconGadgetHandle = 0;
    } /* if */

    if (form->handle)
    {
        (void)smaDeleteWindow(form->handle);
        form->handle = 0;
    } /* if */
    
    form->currentObj = NULL;
    if (form->objList)
    {
        freeSiaFormObjectList(form->objList);
        SIA_FREE(form->objList);
    } /* if */

} /* freeSlideForm */

/*! \brief Removes and releases all actions for the slide form
*****************************************************************************/
static void deleteSlideFormActions(SiaSlideForm *formInfo)
{
    if (formInfo == NULL)
    {
        return;
    } /* if */
	
    /* remove and release actions if already exist */
	if (formInfo->actionMenu != 0)
    {
        (void)smaDeleteAction(formInfo->actionMenu);
        formInfo->actionMenu = 0;
    } /* if */

	if (formInfo->actionOk != 0)
    {
        (void)smaDeleteAction(formInfo->actionOk);
        formInfo->actionOk = 0;
    } /* if */

	if (formInfo->actionStop != 0)
    {
        (void)smaDeleteAction(formInfo->actionStop);
        formInfo->actionStop = 0;
    } /* if */
} /* deleteSlideFormActions */


/*! \brief Create and adds actions to the slide form
 *  \return TRUE on OK else FASLE
 *****************************************************************************/
static WE_BOOL createSlideFormActions(SiaSlideForm *formInfo, 
    WE_BOOL isPlaying)
{
    WE_BOOL ret = TRUE;
	
	if (isPlaying)
    {
        /* create action */
		formInfo->actionStop = smaCreateAction(formInfo->handle, WeStop, 
            handleStopAction, 0, 0, 
            SIA_STR_ID_ACTION_STOP);

        if(formInfo->actionStop == 0)
		{
			/* unable to create and add action */
			ret = FALSE;
		} /* if */
	}
    else
    {
		/* create action */
        formInfo->actionOk = smaCreateAction(formInfo->handle, WeOk, 
            handleOkAction, 0, 0, SIA_STR_ID_ACTION_PLAY);
		
        formInfo->actionStop = smaCreateAction(formInfo->handle, WeStop, 
            handleStopAction, 0, 0, SIA_STR_ID_ACTION_BACK);

        formInfo->actionMenu = smaCreateAction(formInfo->handle, WeMenu, 
            handleMenuAction, 0, 0, SIA_STR_ID_ACTION_MENU);
		
		/* add actions */
		if((formInfo->actionOk == 0) || 
			(formInfo->actionStop == 0) ||
			(formInfo->actionMenu == 0))
		{
			/* unable to create and add action */
			ret = FALSE;	
		} /* if */
    } /* if */
    
    
    if (ret == FALSE)
	{
		/* something went wrong */
		deleteSlideFormActions(formInfo);
	} /* if */
	
    return ret;
} /* createSlideFormActions */

static void copyFormObjectInfo(const SiaFormObject *src, SiaFormObject *dst)
{
    memcpy(dst, src, sizeof(SiaFormObject));
}

static WE_UINT8 addItemToObjectList(SiaFormObjectList **listStart, 
    const SiaFormObject *objPtr)
{
    SiaFormObjectList *listPtr ;
    WE_UINT8 currentId;

    if (objPtr == NULL)
    {
        return 0;
    }

    if (*listStart == NULL)
    {
        *listStart = SIA_CALLOC(sizeof(SiaFormObjectList));
        listPtr = *listStart;
        currentId = 1;
    }
    else
    {
        listPtr = *listStart;
        
        while (listPtr->next != NULL)
        {
            listPtr = listPtr->next;
        } /* while */
        listPtr->next = SIA_CALLOC(sizeof(SiaFormObjectList));
        currentId = listPtr->id;
        listPtr = listPtr->next;
    } /* if */

    listPtr->id = ++currentId;
    copyFormObjectInfo(objPtr, &listPtr->current);

    return listPtr->id;
} /* addItemToObjectList */

static const SiaFormObject *getFormObjFromGadgetHandle(
    const SiaFormObjectList *objList, WE_UINT32 gadgetHandle)
{
    while (objList)
    {
        if (objList->current.gadgetHandle == gadgetHandle)
        {
            return &objList->current;
        } /* if */
        objList = objList->next;
    } /* while */

    return NULL;
} /* getFormObjFromGadgetHandle */


static const WE_UINT32 getFormObjCount(const SiaFormObjectList *objList)
{
    WE_UINT32 i = 0;

    while (objList)
    {
        objList = objList->next;
        i++;
    } /* while */

    return i;

} /* getFormObjCount */

static const SiaFormObject *getFormObjectFromId(const SiaSlideForm *form, 
    WE_UINT32 objId)
{
    SiaFormObjectList *objList;
    if (form == NULL)
    {
        return 0;
    } /* if */
    
    objList = form->objList;
    
    while (objList)
    {
        
        if (objList->id == objId)
        {
            return &objList->current;
        } /* if */
        objList = objList->next;
    } /* while */

    return 0;
} /* getFormObjectFromId */


static void handleKeyLeft(WeWindowHandle winHandle)
{
    SiaSlideForm *form;
    
    form = getFormInstanceByWindowHandle(winHandle);

    if (form == NULL)
    {
        return;
    }
       
    (void)SIA_SIGNAL_SENDTO_U(form->stepRetFsm, form->stepRetSig, WeKey_Left);
}

static void handleKeyRight(WeWindowHandle winHandle)
{
    SiaSlideForm *form;
    
    form = getFormInstanceByWindowHandle(winHandle);

    if (form == NULL)
    {
        return;
    }
       
    (void)SIA_SIGNAL_SENDTO_U(form->stepRetFsm, form->stepRetSig, WeKey_Right);
}


void smaRegisterStepEvent(SiaSlideFormHandle formHandle, 
    SiaStateMachine fsm, int sig)
{
    SiaSlideForm *form;

    form = getFormInstanceByHandle(formHandle);

    if (form == NULL)
    {
        return;
    }
    form->stepRetFsm = fsm;
    form->stepRetSig = sig;
    
    enableObjectFocus(form);

    (void)smaRegisterUserEvent(form->handle, WeKey_Left, handleKeyLeft, 0, 0);
    (void)smaRegisterUserEvent(form->handle, WeKey_Right, handleKeyRight, 0, 0);
}


void smaUnregisterStepEvent(SiaSlideFormHandle formHandle)
{
    SiaSlideForm *form;

    form = getFormInstanceByHandle(formHandle);

    if (form == NULL)
    {
        return;
    }

    disableObjectFocus(form);
    
    (void)smaDeleteUserEvent(form->handle, WeKey_Left);
    (void)smaDeleteUserEvent(form->handle, WeKey_Right);
}
